#include "stdafx.h"
#include "LBTJ.h"
#include "Debug.hpp"

using namespace std;

void LBTJ::calculate1(int len, float * outs, float * n, float * useless)
/*
��ʵ�õ��������ͳ��:��������Զ��1;������ͣ����
*/
{
	m_flag = shared_ptr<Flag>{ new Flag{ static_cast<int>(*n),1} };
	if (!m_flag->check_input())
	{
		Debug::show("һ�������N�������Gap");
		return;
	}
	for (int i = 0; i < len; ++i)
	{
		float zt_type = m_ztqks[i];
		if (m_flag->is_special_case() && i == len - 1 && zt_type != 2)//��������¶����һ��K�����⴦��
		{
			outs[i] = 0;
			continue;
		}
		if (m_flag->never_zhangting())//�����ݴ����ڻ�û�д�����ͣ��
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
		else //���ݴ�����֮ǰ����ͣ��
		{
			if (m_flag->is_in_buffer_time(i))//���ڻ�������,����һ���Ǵ��¸���һ��K��
			{
				if (m_flag->is_price_higher(m_highs[i]))
				{
					if (zt_type == 2) //��ͣͻ���¸�
					{
						outs[i] = outs[i - 1] + 1;
						m_flag->refresh(i, m_highs[i], i);
					}
					else if(zt_type == 1)//������ͣ��ȻҲ�Ǵ��¸�
					{
						outs[i] = outs[i - 1] + 0.5;
						m_flag->refresh(i, m_highs[i], i);
					}
					else//�������¸�,ҲҪˢ�»���
					{
						outs[i] = outs[i - 1];
						m_flag->refresh(i, m_highs[i]);
					}
				}
				else//����û�д��¸ߵ����,������һ������Ч����
				{
					outs[i] = outs[i - 1];
				}
			}
			else//�Ѿ����ڻ�������
			{
				if (zt_type > 0)
				{
					if (m_flag->is_in_valid_time(i))
					{
						//peekһ���Ƿ�������
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
					else//��ʼ�µ�����ͳ������
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
