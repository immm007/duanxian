#include "stdafx.h"
#include "TCalcFuncSets.h"
#include "LBCS.h"

using namespace std;

//���ɵ�dll���������dll�뿽����ͨ���Ű�װĿ¼��T0002/dlls/����,���ڹ�ʽ���������а�

void set_lbcs_data(int len, float* outs, float* code, float* highs, float* ztqks)
//��������
{
	LBCS::set_data(len, outs, code, highs, ztqks);
}

void lbcs(int len, float* outs, float* code, float* n, float* gap)
{
	auto lbcs = LBCS::get(code);
	lbcs->calculate_lbcs(len, outs, n, gap);
}

void df(int len, float* outs, float* code, float* closes, float*)
{
	auto lbcs = LBCS::get(code);
	lbcs->calculate_df(len, outs, closes);
}

//���صĺ���
PluginTCalcFuncInfo g_CalcFuncSets[] = 
{
	{ 1,(pPluginFUNC)&set_lbcs_data },
	{ 2,(pPluginFUNC)&lbcs },
	{ 3,(pPluginFUNC)&df },
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
