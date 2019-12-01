#include "stdafx.h"
#include "TCalcFuncSets.h"
#include <stdio.h>
#include <assert.h>

//生成的dll及相关依赖dll请拷贝到通达信安装目录的T0002/dlls/下面,再在公式管理器进行绑定

float* OPENS = NULL;
float* CLOSES = NULL;
float* HIGHS = NULL;

void set_data(int len, float* outs, float* opens, float* closes, float* highs)
//设置数据
{
	OPENS = opens;
	CLOSES = closes;
	HIGHS = highs;
}

void lbcs(int len, float* outs, float* n, float* _gap, float* zt_types)
/*计算连扳次数，依赖通达信传来的涨停类型数据：
2、收盘涨停
1、触及涨停
0、未触及涨停
n、上次连板有效周期数
gap、断掉连板后的缓冲周期数
*/
{
	int N = static_cast<int>(*n);
	int gap = static_cast<int>(*_gap);
	bool possible_first_lb = false;//指示是否可能首次连板，在处理非缓冲期数据时候用
	int cached_ztcs = 0;//缓冲期缓存的涨停次数
	int pos = -1;//记缓冲期起始的位置，只可能是涨停板或暂停板后缓冲期创新高K线的位置
	float current_high;//当前最高价
	for (int i = 0; i < len; ++i)
	{
		float zt_type = zt_types[i];
		if (pos == -1)//数据窗口内还没有首次连板过
		{
			if (zt_type == 2)
			{
				if (!possible_first_lb)//又一个首板
				{
					outs[i] = 1;
					possible_first_lb = true;
				}
				else //数据窗口内首个二连板
				{
					outs[i] = 2;
					pos = i;
					possible_first_lb = false;
					current_high = HIGHS[i];
				}
			}
			else
			{
				outs[i] = 0;
				possible_first_lb = false;
			}
		}
		else //数据窗口内之前连板过
		{
			if (i - pos <= gap)//还在缓冲期内
			{
				if (HIGHS[i] >= current_high)
				{
					if (zt_type == 2) //涨停突破新高
					{
						outs[i] = outs[i - 1] + cached_ztcs + 1;
						cached_ztcs = 0;
					}
					else
					{
						if (cached_ztcs != 0)
						{
							outs[i] = outs[i - 1] + cached_ztcs;
							cached_ztcs = 0;
						}
						else
						{
							outs[i] = outs[i - 1];
						}
					}
					pos = i;//创新高要刷新缓冲期起始位置
					current_high = HIGHS[i];
				}
				else//处理没有创新高的情况
				{
					if (zt_type == 2)
					{
						if (i - pos < 2) //可能有没创新高的反包涨停
						{
							outs[i] = outs[i - 1] + 1;
						}
						else
						{
							cached_ztcs++;
							outs[i] = outs[i - 1];
						}					}
					else
					{
						outs[i] = outs[i - 1];
					}
				}
			}
			else//已经不在缓冲期了
			{
				if (cached_ztcs != 0)//这里一定是首次不在缓冲期，所以possible_first_lb一定是false
				{
					int j = i - 1;
					for (; j >= pos; --j)//寻找缓冲期最近未涨停的K线位置
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
							outs[i] = (i - pos <= N) ? outs[i - 1] : 1;
							possible_first_lb = true;
						}
						else
						{
							outs[i] = (i - pos <= N) ? outs[i - 1] : 0;
						}
					}
					else if (tail_lbcs == 1)
					{
						if (zt_type == 2)
						{
							outs[i] = 2;
							pos = i;
							current_high = HIGHS[i];
						}
						else
						{
							outs[i] = (i - pos <= N) ? outs[i - 1] : 0;
						}
					}
					else
					{
						outs[i] = zt_type == 2 ? tail_lbcs + 1 : tail_lbcs;
						pos = i;
						current_high = HIGHS[i];
					}
					cached_ztcs = 0;
				}
				else//不管是否首次不在缓冲期，只要没有缓存的涨停次数，处理逻辑是一样的
				{
					if (zt_type == 2)
					{
						if (!possible_first_lb)
						{
							outs[i] = (i - pos <= N) ? outs[i - 1] : 1;
							possible_first_lb = true;
						}
						else
						{
							outs[i] = 2;
							pos = i;
							possible_first_lb = false;
							current_high = HIGHS[i];
						}
					}
					else
					{
						outs[i] = (i - pos <= N) ? outs[i - 1] : 0;
						possible_first_lb = false;
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
