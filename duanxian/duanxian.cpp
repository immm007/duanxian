#include "stdafx.h"
#include "TCalcFuncSets.h"
#include <stdio.h>
#include <assert.h>

//���ɵ�dll���������dll�뿽����ͨ���Ű�װĿ¼��T0002/dlls/����,���ڹ�ʽ���������а�

float* OPENS = NULL;
float* CLOSES = NULL;
float* HIGHS = NULL;

void set_data(int len, float* outs, float* opens, float* closes, float* highs)
//��������
{
	OPENS = opens;
	CLOSES = closes;
	HIGHS = highs;
}

void lbcs(int len, float* outs, float* n, float* _gap, float* zt_types)
/*�����������������ͨ���Ŵ�������ͣ�������ݣ�
2��������ͣ
1��������ͣ
0��δ������ͣ
n���ϴ�������Ч������
gap���ϵ������Ļ���������
*/
{
	int N = static_cast<int>(*n);
	int gap = static_cast<int>(*_gap);
	bool possible_first_lb = false;//ָʾ�Ƿ�����״����壬�ڴ���ǻ���������ʱ����
	int cached_ztcs = 0;//�����ڻ������ͣ����
	int pos = -1;//�ǻ�������ʼ��λ�ã�ֻ��������ͣ�����ͣ��󻺳��ڴ��¸�K�ߵ�λ��
	float current_high;//��ǰ��߼�
	for (int i = 0; i < len; ++i)
	{
		float zt_type = zt_types[i];
		if (pos == -1)//���ݴ����ڻ�û���״������
		{
			if (zt_type == 2)
			{
				if (!possible_first_lb)//��һ���װ�
				{
					outs[i] = 1;
					possible_first_lb = true;
				}
				else //���ݴ������׸�������
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
		else //���ݴ�����֮ǰ�����
		{
			if (i - pos <= gap)//���ڻ�������
			{
				if (HIGHS[i] >= current_high)
				{
					if (zt_type == 2) //��ͣͻ���¸�
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
					pos = i;//���¸�Ҫˢ�»�������ʼλ��
					current_high = HIGHS[i];
				}
				else//����û�д��¸ߵ����
				{
					if (zt_type == 2)
					{
						if (i - pos < 2) //������û���¸ߵķ�����ͣ
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
			else//�Ѿ����ڻ�������
			{
				if (cached_ztcs != 0)//����һ�����״β��ڻ����ڣ�����possible_first_lbһ����false
				{
					int j = i - 1;
					for (; j >= pos; --j)//Ѱ�һ��������δ��ͣ��K��λ��
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
				else//�����Ƿ��״β��ڻ����ڣ�ֻҪû�л������ͣ�����������߼���һ����
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
