#include "stdafx.h"
#include "LBTJ.h"
#include "Debug.hpp"

using namespace std;

void LBTJ::calculate1(int len, float * outs, float * n, float * useless)
/*
更实用的连板次数统计:缓冲期永远是1;触及涨停算半次
*/
{
	m_flag = shared_ptr<Flag>{ new Flag{ static_cast<int>(*n),1} };
	if (!m_flag->check_input())
	{
		Debug::show("一般情况下N必须大于Gap");
		return;
	}
	for (int i = 0; i < len; ++i)
	{
		float zt_type = m_ztqks[i];
		if (m_flag->is_special_case() && i == len - 1 && zt_type != 2)//特殊情况下对最后一根K线特殊处理
		{
			outs[i] = 0;
			continue;
		}
		if (m_flag->never_zhangting())//在数据窗口内还没有触及涨停过
		{
			if (zt_type > 0)
			{
				outs[i] = (zt_type == 1) ? 0.5 : 1;
				m_flag->refresh(i, m_highs[i], i);
			}
			else
			{
				outs[i] = 0;
			}
		}
		else //数据窗口内之前有涨停过
		{
			if (m_flag->is_in_buffer_time(i))//还在缓冲期内,这里一定是创新高下一根K线
			{
				if (m_flag->is_price_higher(m_highs[i]))
				{
					if (zt_type == 2) //涨停突破新高
					{
						outs[i] = outs[i - 1] + 1;
						m_flag->refresh(i, m_highs[i], i);
					}
					else if(zt_type == 1)//触及涨停自然也是创新高
					{
						outs[i] = outs[i - 1] + 0.5;
						m_flag->refresh(i, m_highs[i], i);
					}
					else//单纯创新高,也要刷新缓冲
					{
						outs[i] = outs[i - 1];
						m_flag->refresh(i, m_highs[i]);
					}
				}
				else//处理没有创新高的情况,缓冲期一定在有效期内
				{
					outs[i] = outs[i - 1];
				}
			}
			else//已经不在缓冲期了
			{
				if (zt_type > 0)
				{
					if (m_flag->is_in_valid_time(i))
					{
						//peek一下是否能连板
						if (i != len - 1 && m_ztqks[i + 1] > 0)
						{
							outs[i] = (zt_type == 1) ? 0.5 : 1;
							m_flag->refresh(i, m_highs[i], i);
						}
						else
						{
							outs[i] = outs[i - 1];
						}
					}
					else//开始新的连板统计周期
					{
						outs[i] = (zt_type == 1) ? 0.5 : 1;
						m_flag->refresh(i, m_highs[i], i);
					}
				}
				else
				{
					if (m_flag->is_in_valid_time(i))
					{
						outs[i] = outs[i - 1];
					}
					else
					{
						outs[i] = 0;
					}
				}
			}
		}
	}
}
