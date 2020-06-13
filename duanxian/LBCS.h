#pragma once
#include "Formula.hpp"
#include <Memory>
#include "Flag.hpp"

class LBCS: public Formula
{
public:
	LBCS(float* code, float* highs, float* ztqks) :Formula{ code }, m_highs{ highs }, m_ztqks{ ztqks } {}
	LBCS(const LBCS&) = delete;
	LBCS& operator=(const LBCS&) = delete;
	LBCS(LBCS&&) = delete;

	virtual ~LBCS() {}

	void calculate1(int len, float* outs, float* n, float* gap);
	void calculate2(int len, float* outs, float* closes, float* useless);
	int ID() const { return ID_LBCS; }

private:
	std::shared_ptr<Flag> m_flag;

	void handle_outbuffer_zt(int len, float* outs, int i);

	float* m_highs;
	float* m_ztqks;
};

