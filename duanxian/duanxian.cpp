#include "stdafx.h"
#include "TCalcFuncSets.h"
#include "Formula.hpp"
#include <vector>
#include <map>
#include <memory>
#include "LBTJ.h"

std::vector<std::map<float, std::shared_ptr<Formula>>> FORMULAS{ 5 };

void register_formula(Formula* formula)
{
	 FORMULAS[formula->ID()][formula->code()] = std::shared_ptr<Formula>{ formula };
}
//连板次数相关
 void init_lbtj(int len, float* outs, float* code, float* highs, float* ztqks)
{
	register_formula(new LBTJ{ code,highs,ztqks });
}

void lbtj(int len, float* outs, float* code, float* n, float* gap)
{
	 FORMULAS[ID_LBTJ][*code]->calculate1(len, outs, n, gap);
}

//加载的函数
PluginTCalcFuncInfo g_CalcFuncSets[] =
{
	{ 1,(pPluginFUNC)&init_lbtj },
	{ 2,(pPluginFUNC)&lbtj },
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