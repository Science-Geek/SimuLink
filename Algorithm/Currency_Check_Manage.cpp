#include "Currency_Check_Manage.h"
#include "ir_lib.h"
#include "magImg.h"

#ifdef JULONG_TMR
#include "julong_tmr/arbitrat.h"
#include "julong_tmr/macro_cfg.h"
#include "julong_tmr/bm_lib.h"
#endif

#ifdef YUECHUANG
#include "yuechuang/arbitrat.h"
#include "yuechuang/macro_cfg.h"
#include "yuechuang/bm_lib.h"
#endif

#ifdef BAIJIA
#include "baijia/arbitrat.h"
#include "baijia/macro_cfg.h"
#include "baijia/bm_lib.h"
#endif

#ifdef RONGHE
#include "ronghe/arbitrat.h"
#include "ronghe/macro_cfg.h"
#include "ronghe/bm_lib.h"
#endif

#ifdef KANGYI
#include "kangyi/arbitrat.h"
#include "kangyi/macro_cfg.h"
#include "kangyi/bm_lib.h"
#endif

#ifdef CHUANWEI
#include "chuanwei/arbitrat.h"
#include "chuanwei/macro_cfg.h"
#include "chuanwei/bm_lib.h"
#endif

//OneDSignal AlgSignal[CPU_MAX] = { 0 };
//
//TdnData_Alg AlgDataBuf = { 0 };

INTERMEDIATE_RESULT IntermediateResult = { 0 };

WheelData CashFirstWheel = 0;

Sensitive SensitiveParam = { 0 };


void Currency_CheckManage(RESULT_RMB *RMB_Result, int LastError, Sensitive *Sensitive_p, FuncSwitch *funcswitch)
{
	/*TdnData_Alg *AlgData_p = NULL;

	AlgData_p = &AlgDataBuf;*/

	//memset(&AraitratParam, 0, sizeof(ARAITRAT_PARAM));

	memset(RMB_Result, 0, sizeof(RESULT_RMB));

	CashFirstWheel = 0;

	DataChange(LastError);

	CashFirstWheelManage(AlgDataBuf, &CashFirstWheel);

	GetAngle(AlgDataBuf);

	GetWidth(AlgDataBuf);

	GetBackGroundVol(AlgDataBuf, LastError);

	BM_check(&AlgDataBuf->BM);

	MagImg_AllMagCheck(AlgDataBuf);

	Overlap_check(AlgDataBuf, Sensitive_p, OVERLAP_CALIBRETE_VOL, SENSI_ZHANLIAN_YOUWU);

	Araitrat(RMB_Result, LastError, Sensitive_p, funcswitch);
}