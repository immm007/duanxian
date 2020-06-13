#pragma once

#define ID_LBCS 0
#define ID_LBTJ 1

class Formula
{
public:
	Formula(float* code) :m_code{code} {}
	Formula(const Formula&) = delete;
	Formula& operator=(const Formula&) = delete;
	Formula(Formula&&) = delete;
	virtual ~Formula() {}

	inline float code() const { return *m_code; }

	virtual void calculate1(int len, float* outs, float* arg1 = nullptr, float* arg2 = nullptr) {};
	virtual void calculate2(int len, float* outs, float* arg1 = nullptr, float* arg2 = nullptr) {};
	virtual int ID() const = 0;

private:
	float* m_code;
};

