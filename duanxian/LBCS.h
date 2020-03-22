#pragma once
#include "Formula.hpp"
#include <Memory>

class Flag
{
public:
	Flag() {}
	Flag(int _N, int _gap, int bsp = -1, int llp = -1, float ch = -1) :N{ _N }, gap{_gap}
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
		return N == 0;//这是一种求市场高度板特殊情况
	}

	inline float get_high() const
	{
		return current_high;
	}

	bool check_input()
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
	int buffer_start_pos;//记录连板缓冲期起始的位置，只可能是涨停板或之后缓冲期内创新高K线的位置
	int last_lbzt_pos;//记录连板计数中最近的一个非缓存涨停位置
	float current_high;//当前最高价
};

class LBCS: public Formula
{
public:
	LBCS(float* code, float* highs, float* ztqks) :Formula{ code }, m_highs{ highs }, m_ztqks{ ztqks } {}

	void calculate1(int len, float* outs, float* arg1, float* arg2);
	void calculate2(int len, float* outs, float* arg1, float* arg2);
	int ID() const { return 0; }

private:
	std::shared_ptr<Flag> m_flag;

	void handle_outbuffer_zt(int len, float* outs, int i);

	float* m_highs;
	float* m_ztqks;
};

