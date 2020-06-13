#pragma once
#include "Formula.hpp"
#include <Memory>
#include "Flag.hpp"


class LBTJ: public Formula
{
public:
	LBTJ(float* code, float* highs, float* ztqks) :Formula{ code }, m_highs{ highs }, m_ztqks{ ztqks } {}
	LBTJ(const LBTJ&) = delete;
	LBTJ& operator=(const LBTJ&) = delete;
	LBTJ(LBTJ&&) = delete;

	void calculate1(int len, float* outs, float* n, float* useless);
	int ID() const { return ID_LBTJ; }

private:
	std::shared_ptr<Flag> m_flag;

	float* m_highs;
	float* m_ztqks;
};

