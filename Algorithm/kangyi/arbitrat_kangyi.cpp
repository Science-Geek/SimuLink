#include "arbitrat.h"
#include "JW_type.h"

#ifdef KANGYI
ARAITRAT_PARAM AraitratParam = { 0 };
int LastErrorBuf[LAST_ERR_BUF_LEN] = { 0 };

void Araitrat(RESULT_RMB * RMBresult, int LastError, Sensitive *Sensitive_p, FuncSwitch *funcswitch)
{
	int ret = 0;


	memset(RMBresult, 0, sizeof(RESULT_RMB));

	RMBresult->angle = AraitratParam.angle;

	BM_Araitrat(RMBresult, LastError, funcswitch, Sensitive_p);

#ifdef NEW_BM_LIB
	New_BM_Arbitrat(RMBresult, LastError, funcswitch, Sensitive_p);
#endif

	//HD_Arbitrate(RMBresult, &AraitratParam.HD_Param, Sensitive_p);


#ifdef IR_BACKGROUND_LIB

	Overlap_Arbitrat(RMBresult, Sensitive_p, DUBIOUS_VOL_NUM_THRED, SENSI_VOL_NUM_OFFSET, DUBIOUS_IR_THRED_CNT);

	if (RMBresult->ERRtype == JW_IR_ZHAN_LIAN)
	{
		AraitratParam.ChongZhangFlg = 1;
	}

#endif

	//MMSM_Araitrat(RMBresult, Sensitive_p, LastError);

	//SM_IdentifyAraitrat(RMBresult,Sensitive_p,LastError);

	//UV_Arbitrate(RMBresult,Sensitive_p);

	ChongZhangRejudge(RMBresult, AraitratParam.ChongZhangFlg);

	if (RMBresult->ERRtype != 0)
	{
		return;
	}
#ifdef IR_YOUWU_LIB
		YouWu_Araitrat(RMBresult, &AraitratParam, Sensitive_p);
#endif
	

}
#endif