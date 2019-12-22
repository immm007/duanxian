#pragma once
#include <map>
#include <memory>

class Flag
{
public:
	Flag() {}
	Flag(int _N, int _gap, int bsp = -1, int llp = -1, float ch = -1)
	{
		N = _N;
		gap = _gap;
		refresh(bsp, ch, llp);
	}

	void refresh(int bsp, float ch, int llp)
	{
		buffer_start_pos = bsp;
		current_high = ch;
		last_lbzt_pos = llp;
	}

	void refresh(int bsp, float ch)
	{
		buffer_start_pos = bsp;
		current_high = ch;
	}

	bool is_in_buffer_time(int i) const
	{
		return i - buffer_start_pos <= gap;
	}

	bool is_in_valid_time(int i) const
	{
		return i - buffer_start_pos <= N;
	}

	bool is_price_higher(float price) const
	{
		return price >= current_high;
	}

	bool never_zhangting() const
	{
		return buffer_start_pos == -1;
	}

	int dist_from_llp(int i) const
	{
		return i - last_lbzt_pos;
	}

	int get_buffer_start_pos() const
	{
		return buffer_start_pos;
	}

	bool is_special_case() const
	{
		return N == 0;//����һ�����г��߶Ȱ��������
	}

	bool check_input(int N, int gap)
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

class LBCS
{
public:
	static void set_data(int len, float* outs, float* code, float* highs, float* ztqks);
	static std::shared_ptr<LBCS> get(float* code);

	void calculate(int len, float* outs, float* n, float* gap);
	void set_highs(float* highs) { this->highs = highs; }
	void set_ztqks(float* ztqks) { this->ztqks = ztqks; }
private:
	static std::map<std::string, std::shared_ptr<LBCS>> container;

	LBCS(const std::string& _code) :code{_code} { }
	void handle_outbuffer_zt(int len, float* outs, int i);

	std::string code;
	Flag flag;
	float* highs;
	float* ztqks;
};

