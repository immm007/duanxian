#pragma once
class Flag
{
public:
	Flag() {}
	Flag(int _N, int _gap, int bsp = -1, int llp = -1, float ch = -1) :N{ _N }, gap{ _gap }
	{
		refresh(bsp, ch, llp);
	}

	inline void refresh(int bsp, float ch, int llp)
	{
		buffer_start_pos = bsp;
		current_high = ch;
		last_lbzt_pos = llp;
	}

	inline void refresh(int bsp, float ch)
	{
		buffer_start_pos = bsp;
		current_high = ch;
	}

	inline bool is_in_buffer_time(int i) const
	{
		return i - buffer_start_pos <= gap;
	}

	inline bool is_in_valid_time(int i) const
	{
		return i - buffer_start_pos <= N;
	}

	inline bool is_price_higher(float price) const
	{
		return price >= current_high;
	}

	inline bool never_zhangting() const
	{
		return buffer_start_pos == -1;
	}

	inline int dist_from_llp(int i) const
	{
		return i - last_lbzt_pos;
	}

	inline int get_buffer_start_pos() const
	{
		return buffer_start_pos;
	}

	inline bool is_special_case() const
	{
		return N == 0;//����һ�����г��߶Ȱ��������
	}

	inline float get_high() const
	{
		return current_high;
	}

	inline bool check_input()
	{
		if (N > gap)
		{
			return true;
		}
		else
		{
			if (is_special_case())
			{
				return true;
			}
			return false;
		}
	}
private:
	int N;
	int gap;
	int buffer_start_pos;//��¼���建������ʼ��λ�ã�ֻ��������ͣ���֮�󻺳����ڴ��¸�K�ߵ�λ��
	int last_lbzt_pos;//��¼��������������һ���ǻ�����ͣλ��
	float current_high;//��ǰ��߼�
};

