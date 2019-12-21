#include "stdafx.h"
#include "TCalcFuncSets.h"
#include "Flag.hpp"
#include <vector>

using namespace std;


//生成的dll及相关依赖dll请拷贝到通达信安装目录的T0002/dlls/下面,再在公式管理器进行绑定

char CODE[8];
float* HIGHS = NULL;

void fill_with_zero(int len, float* outs)
{
	for (int i = 0; i < len; i++)
	{
		outs[i] = 0;
	}
}

//处理缓冲期外的涨停
void handle_outbuffer_zt(int len, float* outs, float* zt_types, Flag& flag, int i)
{
	if (i + 1 < len && zt_types[i + 1] == 2)//peek一下是否有两连板的机会
	{
		outs[i] = 1;
		flag.refresh(i, HIGHS[i], i);
	}
	else
	{
		if (flag.is_in_valid_time(i))
		{
			outs[i] = outs[i - 1];
		}
		else
		{
			outs[i] = 1;
			flag.refresh(i, HIGHS[i], i);;
		}
	}
}

void set_data(int len, float* outs, float* code, float* highs, float* notused)
//设置数据
{
	sprintf(CODE, "%f", *code);
	HIGHS = highs;
	fill_with_zero(len, outs);
}

void lbcs(int len, float* outs, float* n, float* _gap, float* zt_types)
/*计算连扳次数，依赖通达信传来的涨停类型数据：
2、收盘涨停
1、触及涨停
0、未触及涨停
n、上次连板次数保持有效的周期数
gap、断掉连板后的缓冲周期数
*/
{
	int N = static_cast<int>(*n);
	int gap = static_cast<int>(*_gap);
	Flag flag{ N,gap};
	if (!flag.check_input(N, gap))
	{
		return;
	}
	vector<int> cached_zt;//记录缓冲期内缓存的涨停K线位置
	for (int i = 0; i < len; ++i)
	{
		float zt_type = zt_types[i];
		if (flag.is_special_case() && i == len - 1 && zt_type!=2)
		{
			outs[i] = 0;
			continue;
		}
		if (flag.never_zhangting())//在数据窗口内还没有涨停过
		{
			if (zt_type == 2)
			{
				outs[i] = 1;
				flag.refresh(i, HIGHS[i], i);
			}
			else
			{
				outs[i] = 0;
			}
		}
		else //数据窗口内之前有涨停过
		{
			if (flag.is_in_buffer_time(i))//还在缓冲期内
			{
				int cached_num = cached_zt.size();
				if (flag.is_price_higher(HIGHS[i]))
				{
					if (zt_type == 2) //涨停突破新高
					{
						outs[i] = outs[i - 1] + cached_num + 1;
						cached_zt.clear();
						flag.refresh(i,HIGHS[i],i);
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
						flag.refresh(i, HIGHS[i]);
					}
				}
				else//处理没有创新高的情况
				{
					if (zt_type == 2)
					{
						int dist = flag.dist_from_llp(i);
						//事实上dist不可能为0和1，因为不可能出现涨停接涨停却不创新高的情况
						if ( dist == 0)
						{
							MessageBoxA(NULL, "dist不可能为0！", CODE, MB_OK);
							return;
						}
						else if(dist ==1)
						{
							MessageBoxA(NULL, "dist不可能为1！", CODE, MB_OK);
							return;
						}
						else if (dist == 2)
						{
							outs[i] = outs[i - 1] + 1;
							flag.refresh(i, HIGHS[i], i);
						}
						else
						{
							if ( !cached_zt.empty() && i == cached_zt.back() + 1)//缓冲期内出现两连板却没创新高的极端情况
							{
								outs[i] = outs[i - 1] + cached_num + 1;
								cached_zt.clear();
								flag.refresh(i, HIGHS[i], i);
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
					for (; j > flag.get_buffer_start_pos(); --j)//寻找缓冲期最近未涨停的K线位置
					{
						if (zt_types[j] != 2)
						{
							break;
						}
					}
					int tail_lbcs = i - 1 - j;
					if (tail_lbcs == 0)
					{
						if (zt_type == 2)
						{
							handle_outbuffer_zt(len, outs, zt_types, flag, i);
						}
						else
						{
							outs[i] = flag.is_in_valid_time(i) ? outs[i - 1] : 0;
						}
					}
					else if (tail_lbcs == 1)
					{
						if (zt_type == 2)
						{
							outs[i] = 2;
							flag.refresh(i,HIGHS[i],i);
						}
						else
						{
							outs[i] = flag.is_in_valid_time(i) ? outs[i - 1] : 0;
						}
					}
					else//不可能存在缓存着两个连续的涨停板
					{
						MessageBoxA(NULL, "不应该出现缓存着连续的涨停板！", CODE, MB_OK); 
						return;
					}
					cached_zt.clear();
				}
				else//不管是否首次不在缓冲期，只要没有缓存的涨停次数，处理逻辑是一样的
				{
					if (zt_type == 2)
					{
						handle_outbuffer_zt(len, outs, zt_types, flag, i);
					}
					else
					{
						outs[i] = flag.is_in_valid_time(i) ? outs[i - 1] : 0;
					}
				}
			}
		}
	}
}

//加载的函数
PluginTCalcFuncInfo g_CalcFuncSets[] = 
{
	{ 1,(pPluginFUNC)&set_data },
	{ 2,(pPluginFUNC)&lbcs },
	{0,NULL},
};

//导出给TCalc的注册函数
BOOL RegisterTdxFunc(PluginTCalcFuncInfo** pFun)
{
	if(*pFun==NULL)
	{
		(*pFun)=g_CalcFuncSets;
		return TRUE;
	}
	return FALSE;
}
