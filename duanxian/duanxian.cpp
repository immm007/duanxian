#include "stdafx.h"
#include "TCalcFuncSets.h"
#include "Formula.hpp"
#include <vector>
#include <map>
#include <memory>
#include "LBCS.h"

std::vector<std::map<float, std::shared_ptr<Formula>>> FORMULAS{ 5 };

void register_formula(Formula* formula)
{
	 FORMULAS[formula->ID()][formula->code()] = std::shared_ptr<Formula>{ formula };
}
//连板次数相关
 void init_lbcs(int len, float* outs, float* code, float* highs, float* ztqks)
{
	register_formula(new LBCS{ code,highs,ztqks });
}

void lbcs(int len, float* outs, float* code, float* n, float* gap)
{
	 FORMULAS[0][*code]->calculate1(len, outs, n, gap);
}

void lbcs_df(int len, float* outs, float* code, float* closes, float*)
{
	FORMULAS[0][*code]->calculate2(len, outs, closes);
}

//加载的函数
PluginTCalcFuncInfo g_CalcFuncSets[] =
{
	{ 1,(pPluginFUNC)&init_lbcs },
	{ 2,(pPluginFUNC)&lbcs },
	{ 3,(pPluginFUNC)&lbcs_df },
	{ 0,NULL },
};

//导出给TCalc的注册函数
BOOL RegisterTdxFunc(PluginTCalcFuncInfo** pFun)
{
	if (*pFun == NULL)
	{
		(*pFun) = g_CalcFuncSets;
		return TRUE;
	}
	return FALSE;
}