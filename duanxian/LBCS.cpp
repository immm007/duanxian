#include "stdafx.h"
#include "LBCS.h"
#include "Debug.hpp"
#include <vector>
#include <algorithm>

using namespace std;

void LBCS::calculate1(int len, float* outs, float* n, float* gap)
/*计算连扳次数，依赖通达信传来的涨停类型数据：
2、收盘涨停
1、触及涨停
0、未触及涨停
n、上次连板次数保持有效的周期数
gap、断掉连板后的缓冲周期数
*/
{
	m_flag = shared_ptr<Flag>{ new Flag{ static_cast<int>(*n),static_cast<int>(*gap) } };
	if (!m_flag->check_input())
	{
		Debug::show("一般情况下N必须大于Gap");
		return;
	}
	//Debug* debug = Debug::create();
	vector<int> cached_zt{8};//记录缓冲期内缓存的涨停K线位置
	for (int i = 0; i < len; ++i)
	{
		float zt_type = m_ztqks[i];
		if (m_flag->is_special_case() && i == len - 1 && zt_type != 2)//特殊情况下对最后一根K线特殊处理
		{
			outs[i] = 0;
			continue;
		}
		if (m_flag->never_zhangting())//在数据窗口内还没有涨停过
		{
			if (zt_type == 2)
			{
				outs[i] = 1;
				m_flag->refresh(i, m_highs[i], i);
			}
			else
			{
				outs[i] = 0;
			}
		}
		else //数据窗口内之前有涨停过
		{
			if (m_flag->is_in_buffer_time(i))//还在缓冲期内
			{
				int cached_num = cached_zt.size();
				if (m_flag->is_price_higher(m_highs[i]))
				{
					if (zt_type == 2) //涨停突破新高
					{
						outs[i] = outs[i - 1] + cached_num + 1;
						cached_zt.clear();
						m_flag->refresh(i, m_highs[i], i);
					}
					else
					{
						if (cached_num != 0)
						{
							outs[i] = outs[i - 1] + cached_num;
							cached_zt.clear();
						}
						else
						{
							outs[i] = outs[i - 1];
						}
						m_flag->refresh(i, m_highs[i]);
					}
				}
				else//处理没有创新高的情况
				{
					if (zt_type == 2)
					{
						int dist = m_flag->dist_from_llp(i);
						if (dist == 0)
						{
							Debug::show("dist不可能为0");
							outs[i - 1] = 255;//便于调试
							return;
						}
						//虽然理论上dist不可能为1，因为不可能出现涨停接涨停却不创新高的情况
						//但是某些极端情况，比如当天正好除权，是可能出现的
						else if (dist == 1)
						{
							Debug::show("非常少见的除权日及其前一天都涨停");
							outs[len - 1] = 256;//便于调试
							return;
						}
						else if (dist == 2)
						{
							outs[i] = outs[i - 1] + 1;
							m_flag->refresh(i, m_highs[i], i);
						}
						else
						{
							if (!cached_zt.empty() && i == cached_zt.back() + 1)//缓冲期内出现两连板却没创新高的极端情况
							{
								outs[i] = outs[i - 1] + cached_num + 1;
								cached_zt.clear();
								m_flag->refresh(i, m_highs[i], i);
							}
							else
							{
								outs[i] = outs[i - 1];
								cached_zt.push_back(i);
							}
						}
					}
					else
					{
						outs[i] = outs[i - 1];
					}
				}
			}
			else//已经不在缓冲期了
			{
				if (!cached_zt.empty())//既然还有未处理的缓存涨停次数，这里一定是首次脱离缓冲期
				{
					//这里代码主要是看看缓存的涨停K线能否与当前K线构成新的一轮连板次数计数
					//注意这个事实：缓存的涨停不可能有连续的
					int j = i - 1;
					for (; j > m_flag->get_buffer_start_pos(); --j)//寻找缓冲期最近未涨停的K线位置
					{
						if (m_ztqks[j] != 2)
						{
							break;
						}
					}
					int tail_lbcs = i - 1 - j;
					if (tail_lbcs == 0)
					{
						if (zt_type == 2)
						{
							handle_outbuffer_zt(len, outs, i);
						}
						else
						{
							outs[i] = m_flag->is_in_valid_time(i) ? outs[i - 1] : 0;
						}
					}
					else if (tail_lbcs == 1)
					{
						if (zt_type == 2)
						{
							outs[i] = 2;
							m_flag->refresh(i, m_highs[i], i);
						}
						else
						{
							outs[i] = m_flag->is_in_valid_time(i) ? outs[i - 1] : 0;
						}
					}
					else//不可能存在缓存着两个连续的涨停板
					{
						Debug::show("不可能存在缓存着两个连续的涨停板");
						outs[len - 1] = 257;//便于调试
						return;
					}
					cached_zt.clear();
				}
				else//不管是否首次不在缓冲期，只要没有缓存的涨停次数，处理逻辑是一样的
				{
					if (zt_type == 2)
					{
						handle_outbuffer_zt(len, outs, i);
					}
					else
					{
						outs[i] = m_flag->is_in_valid_time(i) ? outs[i - 1] : 0;
					}
				}
			}
		}
	}
}

void LBCS::calculate2(int len, float* outs, float* closes, float* useless)
//计算连板结束后的跌幅
{
	int bsp = m_flag->get_buffer_start_pos();
	if (len - bsp > 120)//现在距离缓冲期起始位置太远了
	{
		return;
	}
	float cur_high = m_flag->get_high();
	float high = *max_element(m_highs + bsp + 1, m_highs + len);
	if (high >= cur_high) //后面创出了新高
	{
		for (int j = len - 1; j > bsp; --j)
		{
			if (m_highs[j] == high)
			{
				bsp = j;
				break;
			}
		}
	}
	else
	{
		high = cur_high;
	}
	for (int i = bsp; i < len; i++)
	{
		outs[i] = (closes[i] - high) / high;
	}
}

//处理缓冲期外的涨停
void LBCS::handle_outbuffer_zt(int len, float* outs, int i)
{
	if (i + 1 < len && m_ztqks[i + 1] == 2)//peek一下是否有两连板的机会
	{
		outs[i] = 1;
		m_flag->refresh(i, m_highs[i], i);
	}
	else
	{
		if (m_flag->is_in_valid_time(i))
		{
			outs[i] = outs[i - 1];
		}
		else
		{
			outs[i] = 1;
			m_flag->refresh(i, m_highs[i], i);;
		}
	}
}