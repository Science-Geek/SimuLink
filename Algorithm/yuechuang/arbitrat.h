#ifndef	_ARAITRAT_H
#define	_ARAITRAT_H
#include "machine_type.h"

#ifdef YUECHUANG
#include "Currency_Check_Manage.h"
#include "macro_cfg.h"
#include "data_change.h"
#include "ir_lib.h"
#include "bm_lib.h"
#include "mmsm_lib.h"


#define MATCH_COUNT_SURE		3
#define MIN_CASH_LEVEL			1
#define MACTH_COUNT_SURE_FIANL	7
#define JUDGE_WIDE_ADD_MATCH(a) if(a > JUDGE_LEVEL_2){a += 3;}

enum About_Angle_Check_
{
	IF_ANGLE_ADD_MATCH = 30,
	IF_ANGLE_CHECK = 20,
};

enum BM_GapNum_About_MAX_MIN_
{
	BM_GAP_MAX = 30,
	BM_GAP_MIN = 9,
};

enum Brake_Flag_
{
	BRAKE_FLG_CLR = 0,
	BRAKE_FLG_SET
};

enum BM_Threshold_Flag_
{
	BM_LOW_THRESHOLD = 0,
	BM_HIGH_THRESHOLD,
};

/*--------------------About_RMB_Width-------------------*/
enum Rmb__Width //67 68 69 70
{
	MAX_100_YUAN_WIDTH = 70,		//130
	MIN_100_YUAN_WIDTH = 64,		//115
	ERROR_100_YUAN_WIDTH = 60,
	MAX_50_20_10_YUAN_WIDTH = 63,	//115
	MIN_50_20_10_YUAN_WIDTH = 57,	//104
	MAX_1_5_YUAN_WIDTH = 56,		//104
	MIN_1_5_YUAN_WIDTH = 50,		//88
	CASH_TOO_WIDTH = 80,
	CASH_TOO_NARROW = 40,
};

enum Cash_Amount_
{
	CASH_NO_MATCH = 0,
	CASH_100_15,
	CASH_5,
	CASH_100,
	CASH_20,
	CASH_20_99,
	CASH_10,
	CASH_50,
	CASH_1,
	CASH_MAX
};

enum Cash_Level_
{
	JUDGE_LEVEL_0 = 0,
	JUDGE_LEVEL_1,
	JUDGE_LEVEL_2,
	JUDGE_LEVEL_3,
	JUDGE_LEVEL_4,
	JUDGE_LEVEL_5,
	JUDGE_LEVEL_6,
	JUDGE_LEVEL_7,
	JUDGE_LEVEL_8,
	JUDGE_LEVEL_9,
	JUDGE_LEVEL_10,
};
enum JUDGE_AMOUNT_LEVER
{
	BM_100_HIGH_SHOLD = 72,
	BM_100_MID_SHOLD = 48,
	BM_100_LOW_SHOLD = 24,
	BM_100_MIN_SHOLD = 12,
	BM_50_HIGH_SHOLD = 72,
	BM_50_MID_SHOLD = 48,
	BM_50_LOW_SHOLD = 36,
	BM_50_MIN_SHOLD = 12,
	BM_20_HIGH_SHOLD = 48,
	BM_20_MID_SHOLD = 27,
	BM_20_LOW_SHOLD = 15,
	BM_20_MIN_SHOLD = 6,
	BM_10_HIGH_SHOLD = 48,
	BM_10_MID_SHOLD = 27,
	BM_10_LOW_SHOLD = 15,
	BM_10_MIN_SHOLD = 6,
	BM_5_MAIN_HIGH_SHOLD = 60,
	BM_5_MAIN_MID_SHOLD = 36,
	BM_5_MAIN_LOW_SHOLD = 18,
	BM_5_MAIN_MIN_SHOLD = 6,
	BM_100_15VER_MAIN_HIGH_SHOLD = 56,
	BM_100_15VER_MAIN_MID_SHOLD = 32,
	BM_100_15VER_MAIN_LOW_SHOLD = 20,
	BM_100_15VER_MAIN_MIN_SHOLD = 8,
	BM_100_15VER_HIGH_SHOLD = 44,
	BM_100_15VER_MID_SHOLD = 34,
	BM_100_15VER_LOW_SHOLD = 20,
	BM_100_15VER_MIN_SHOLD = 8,
	BM_20_99VER_HIGH_SHOLD = 1,
	BM_20_99VER_MID_SHOLD = 0,
	BM_20_99VER_LOW_SHOLD = 0,
	BM_20_99VER_MIN_SHOLD = 0,
};

typedef struct araitrat_param
{
	float angle;
	float slope;
	int Dir;
	int Width;
	int Width_IR[IR_NUM];
	int Height;
	int DataLen;
	int ChongZhangFlg;
	//int SM_Direction_Score[TDN_DIR_MAX];
	//BM_PARAM BM_Low_Param;
	BM_PARAM BM_Mid_Param;
	BM_PARAM BM_High_Param;
	///*———新算法得分———*/
	//BM_PARAM New_BM_Mid_Param;
	//BM_PARAM New_BM_High_Param;

	//HD_PARAM HD_Param;
	BM_LEVEL_PARAM LevelNoWidthHigh;
	BM_LEVEL_PARAM LevelNoWidthMid;
	////mm
	//MMResult LMMresult;
	//MMResult RMMresult;

	//MMSMResult_half LMMresultFront;
	//MMSMResult_half RMMresultFront;
	//MMSMResult_half LMMresultBack;
	//MMSMResult_half RMMresultBack;

	////sm
	//MMSMResult_half LSMresultFront[SM_NUM];
	//MMSMResult_half RSMresultFront[SM_NUM];
	//MMSMResult_half LSMresultBack[SM_NUM];
	//MMSMResult_half RSMresultBack[SM_NUM];

	//MMSM_DIR_PARAM LSM_TMR_Front[SM_NUM];
	//MMSM_DIR_PARAM LSM_TMR_Back[SM_NUM];
	//MMSM_DIR_PARAM RSM_TMR_Front[SM_NUM];
	//MMSM_DIR_PARAM RSM_TMR_Back[SM_NUM];
	////wheel param
	MMSM_WHEEL_PARAM BM_WheelParam;
	MMSM_WHEEL_PARAM HD_WheelParam;

	MMSM_WHEEL_PARAM LSM_TMR_WheelParam[SM_NUM];
	MMSM_WHEEL_PARAM RSM_TMR_WheelParam[SM_NUM];
	MMSM_WHEEL_PARAM LSM_TMR_WheelParamLow[SM_NUM];
	MMSM_WHEEL_PARAM RSM_TMR_WheelParamLow[SM_NUM];

	MMSM_WHEEL_PARAM LMM_WheelParam[MM_NUM];
	MMSM_WHEEL_PARAM RMM_WheelParam[MM_NUM];
	////uv
	//UV_PARAM UV_Param;

	////you wu chongzhang
	IR_OVERLAP_INFO OverlapVol_Info[IR_NUM];
	int ChannalBackGroundVol[IR_NUM];
	int CenterVol[IR_NUM];
}ARAITRAT_PARAM;

void BM_Araitrat(RESULT_RMB *RMBresult, int LastError, FuncSwitch *funcswitch, Sensitive *Sensitive_p);
void ChongZhangRejudge(RESULT_RMB *RMBresult, int flg);
void Overlap_Arbitrat(RESULT_RMB* RMBresult, Sensitive *Sensitive_p, int DubiousVolNum, int Sensi_Offset_Cnt, int Dubious_IR_Thred);
extern void Araitrat(RESULT_RMB * RMBresult, int LastError, Sensitive *Sensitive_p, FuncSwitch *funcswitch);

extern ARAITRAT_PARAM AraitratParam;

extern int LastErrorBuf[LAST_ERR_BUF_LEN];
#endif

#endif