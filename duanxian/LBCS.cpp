#include "stdafx.h"
#include "LBCS.h"
#include "Debug.hpp"
#include <vector>
#include <algorithm>

using namespace std;

void LBCS::calculate1(int len, float* outs, float* n, float* gap)
/*�����������������ͨ���Ŵ�������ͣ�������ݣ�
2��������ͣ
1��������ͣ
0��δ������ͣ
n���ϴ��������������Ч��������
gap���ϵ������Ļ���������
*/
{
	int N = static_cast<int>(*n);
	int GAP = static_cast<int>(*gap);
	m_flag = shared_ptr<Flag>{ new Flag{ N,GAP } };
	if (!m_flag->check_input())
	{
		Debug::show("һ�������N�������Gap");
		return;
	}
	Debug* debug = Debug::create();
	vector<int> cached_zt;//��¼�������ڻ������ͣK��λ��
	for (int i = 0; i < len; ++i)
	{
		float zt_type = m_ztqks[i];
		if (m_flag->is_special_case() && i == len - 1 && zt_type != 2)//��������¶����һ��K�����⴦��
		{
			outs[i] = 0;
			continue;
		}
		if (m_flag->never_zhangting())//�����ݴ����ڻ�û����ͣ��
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
		else //���ݴ�����֮ǰ����ͣ��
		{
			if (m_flag->is_in_buffer_time(i))//���ڻ�������
			{
				int cached_num = cached_zt.size();
				if (m_flag->is_price_higher(m_highs[i]))
				{
					if (zt_type == 2) //��ͣͻ���¸�
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
				else//����û�д��¸ߵ����
				{
					if (zt_type == 2)
					{
						int dist = m_flag->dist_from_llp(i);
						if (dist == 0)
						{
							Debug::show("dist������Ϊ0");
							outs[i - 1] = 255;//���ڵ���
							return;
						}
						//��Ȼ������dist������Ϊ1����Ϊ�����ܳ�����ͣ����ͣȴ�����¸ߵ����
						//����ĳЩ������������統�����ó�Ȩ���ǿ��ܳ��ֵ�
						else if (dist == 1)
						{
							debug->show("�ǳ��ټ��ĳ�Ȩ�ռ���ǰһ�춼��ͣ");
							outs[len - 1] = 256;//���ڵ���
							return;
						}
						else if (dist == 2)
						{
							outs[i] = outs[i - 1] + 1;
							m_flag->refresh(i, m_highs[i], i);
						}
						else
						{
							if (!cached_zt.empty() && i == cached_zt.back() + 1)//�������ڳ���������ȴû���¸ߵļ������
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
			else//�Ѿ����ڻ�������
			{
				if (!cached_zt.empty())//��Ȼ����δ����Ļ�����ͣ����������һ�����״����뻺����
				{
					//���������Ҫ�ǿ����������ͣK���ܷ��뵱ǰK�߹����µ�һ�������������
					//ע�������ʵ���������ͣ��������������
					int j = i - 1;
					for (; j > m_flag->get_buffer_start_pos(); --j)//Ѱ�һ��������δ��ͣ��K��λ��
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
					else//�����ܴ��ڻ�����������������ͣ��
					{
						Debug::show("�����ܴ��ڻ�����������������ͣ��");
						outs[len - 1] = 257;//���ڵ���
						return;
					}
					cached_zt.clear();
				}
				else//�����Ƿ��״β��ڻ����ڣ�ֻҪû�л������ͣ�����������߼���һ����
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

void LBCS::calculate2(int len, float * outs, float * closes, float* arg2)
//�������������ĵ���
{
	int bsp = m_flag->get_buffer_start_pos();
	if (len - bsp > 120)//���ھ��뻺������ʼλ��̫Զ��
	{
		return;
	}
	float cur_high = m_flag->get_high();
	float high = *max_element(m_highs + bsp + 1, m_highs + len);
	if (high >= cur_high) //���洴�����¸�
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

//�������������ͣ
void LBCS::handle_outbuffer_zt(int len, float* outs, int i)
{
	if (i + 1 < len && m_ztqks[i + 1] == 2)//peekһ���Ƿ���������Ļ���
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