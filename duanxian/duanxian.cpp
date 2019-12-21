#include "stdafx.h"
#include "TCalcFuncSets.h"
#include "Flag.hpp"
#include <vector>

using namespace std;


//���ɵ�dll���������dll�뿽����ͨ���Ű�װĿ¼��T0002/dlls/����,���ڹ�ʽ���������а�

char CODE[8];
float* HIGHS = NULL;

void fill_with_zero(int len, float* outs)
{
	for (int i = 0; i < len; i++)
	{
		outs[i] = 0;
	}
}

//�������������ͣ
void handle_outbuffer_zt(int len, float* outs, float* zt_types, Flag& flag, int i)
{
	if (i + 1 < len && zt_types[i + 1] == 2)//peekһ���Ƿ���������Ļ���
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
//��������
{
	sprintf(CODE, "%f", *code);
	HIGHS = highs;
	fill_with_zero(len, outs);
}

void lbcs(int len, float* outs, float* n, float* _gap, float* zt_types)
/*�����������������ͨ���Ŵ�������ͣ�������ݣ�
2��������ͣ
1��������ͣ
0��δ������ͣ
n���ϴ��������������Ч��������
gap���ϵ������Ļ���������
*/
{
	int N = static_cast<int>(*n);
	int gap = static_cast<int>(*_gap);
	Flag flag{ N,gap};
	if (!flag.check_input(N, gap))
	{
		return;
	}
	vector<int> cached_zt;//��¼�������ڻ������ͣK��λ��
	for (int i = 0; i < len; ++i)
	{
		float zt_type = zt_types[i];
		if (flag.is_special_case() && i == len - 1 && zt_type!=2)
		{
			outs[i] = 0;
			continue;
		}
		if (flag.never_zhangting())//�����ݴ����ڻ�û����ͣ��
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
		else //���ݴ�����֮ǰ����ͣ��
		{
			if (flag.is_in_buffer_time(i))//���ڻ�������
			{
				int cached_num = cached_zt.size();
				if (flag.is_price_higher(HIGHS[i]))
				{
					if (zt_type == 2) //��ͣͻ���¸�
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
				else//����û�д��¸ߵ����
				{
					if (zt_type == 2)
					{
						int dist = flag.dist_from_llp(i);
						//��ʵ��dist������Ϊ0��1����Ϊ�����ܳ�����ͣ����ͣȴ�����¸ߵ����
						if ( dist == 0)
						{
							MessageBoxA(NULL, "dist������Ϊ0��", CODE, MB_OK);
							return;
						}
						else if(dist ==1)
						{
							MessageBoxA(NULL, "dist������Ϊ1��", CODE, MB_OK);
							return;
						}
						else if (dist == 2)
						{
							outs[i] = outs[i - 1] + 1;
							flag.refresh(i, HIGHS[i], i);
						}
						else
						{
							if ( !cached_zt.empty() && i == cached_zt.back() + 1)//�������ڳ���������ȴû���¸ߵļ������
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
			else//�Ѿ����ڻ�������
			{
				if (!cached_zt.empty())//��Ȼ����δ����Ļ�����ͣ����������һ�����״����뻺����
				{
					//���������Ҫ�ǿ����������ͣK���ܷ��뵱ǰK�߹����µ�һ�������������
					//ע�������ʵ���������ͣ��������������
					int j = i - 1;
					for (; j > flag.get_buffer_start_pos(); --j)//Ѱ�һ��������δ��ͣ��K��λ��
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
					else//�����ܴ��ڻ�����������������ͣ��
					{
						MessageBoxA(NULL, "��Ӧ�ó��ֻ�������������ͣ�壡", CODE, MB_OK); 
						return;
					}
					cached_zt.clear();
				}
				else//�����Ƿ��״β��ڻ����ڣ�ֻҪû�л������ͣ�����������߼���һ����
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

//���صĺ���
PluginTCalcFuncInfo g_CalcFuncSets[] = 
{
	{ 1,(pPluginFUNC)&set_data },
	{ 2,(pPluginFUNC)&lbcs },
	{0,NULL},
};

//������TCalc��ע�ắ��
BOOL RegisterTdxFunc(PluginTCalcFuncInfo** pFun)
{
	if(*pFun==NULL)
	{
		(*pFun)=g_CalcFuncSets;
		return TRUE;
	}
	return FALSE;
}
