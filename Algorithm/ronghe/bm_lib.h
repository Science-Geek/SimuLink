#ifndef	_BM_CHECK_H
#define	_BM_CHECK_H
#include "machine_type.h"

#ifdef RONGHE
#include "macro_cfg.h"
#include "alg_lib.h"
#include "data_change_ronghe.h"

#define BM_PEAK_MIN_POINT		1
#define GAP_DIFF(a,b)	(((a)>(b))?((a)-(b)):((b)-(a)))

typedef struct bm_big_gap_param
{
	int BM_100_BigGapNum;
	int BM_50_BigGapNum;
	int BM_100_15_BigGapNum;
	int BM_10_BigGapNum;
	int BM_10_MidGapNum;
	int BM_5_BigGapNum;
	int BM_5_MidGapNum;
}BM_BIG_GAP_PARAM;

typedef struct bm_araitrat_param
{
	BM_BIG_GAP_PARAM BM_BigGapNum;
	int BM_100_v15_Match;
	int BM_100_v15_Narrow_Match;
	int BM_100_v15_Wide_Match;
	int BM_100_Match;
	int BM_50_Match;
	int BM_20_Match;
	int BM_20_v99_Match;
	int BM_10_Match;
	int BM_5_Match;
	int BM_GapNum;
	int HD_GapNum;
	int BM_geshu;
	int CashMaxLevel;
	int CashMaxIndex;
}BM_PARAM;

enum RMB_Amount_
{
	BM_ERROR = 0,
	BM_100_V05_V99 = 100,
	BM_100_V15 = 100,
	BM_100 = 100,
	BM_50 = 50,
	BM_20 = 20,
	BM_10 = 10,
	BM_5 = 5,
	BM_1 = 1,
};

enum bm_100_param
{
	BM100_MATCH_MAX_DIFF = 7,
	BM100_FORM = 70,		//62
	BM100_MAX_NUM = 16,		//默认大磁数量最大值
	BM100_MIN_NUM = 15,			//默认大磁数量最小值
};
enum bm_50_param
{
	BM50_MATCH_MAX_DIFF = 7,		//8
	BM50_FORM_1 = 53,			//48
	BM50_FORM_2 = 75,			//73
	BM50_FORM_1_2_SUB = 20,
	BM50_BUF_MAX_LEN = 6,
	BM50_FORM_2_MAX = 82,		//79
	BM50_FORM_2_MIN = 41,		//37
	BM50_MAX_NUM = 16,
	BM50_MIN_NUM = 15,
};
enum bm_50_low_param
{
	BM_50_LOW_FORM = 52,
	BM_50_LOW_BIG_FORM_MAX_SUB = 7,
	BM_50_LOW_FORM_1 = 23,
	BM_50_LOW_FORM_2 = 30,
	BM_50_LOW_MAX_SUB = 5,
	BM_50_LOW_FORM_LEN = 3,
};

enum bm_1020_parm
{
	BM_1020_GAP_PERIOD = 3,
	BM_1020_FEATURE_INVALID_VALUE = 11,	//11
	BM_20_FEATURE_THRESHOLD_VALUE = 11,	//11
	BM_10_FEATURE_THRESHOLD_VALUE = 12,	//12
	BM_20_GAP_SUB_MIN = -4,				//0
	BM_20_MIN_P_THRESHOLD_VALUE = 18,
	BM_10_MIN_P_THRESHOLD_VALUE = 22,

	BM_1020_NARROW_GAP_MAX = 28,		//28
	BM_1020_NARROW_GAP_MIN = 13,		//9
	BM_1020_MID_GAP_MAX = 56,			//58
	BM_1020_WIDE_GAP_MAX = 68,			//68
	BM_1020_MID_WIDE_GAP_MIN = 43,		//36

	BM_1020_MAX_NUM = 24,
	BM_1020_MIN_NUM = 21,
	BM_10_FORM = 52,
	BM_10_MATCH_MAX_DIFF = 6,
	BM_20_FORM1 = 52,
	BM_20_FORM2 = 64,
	BM_20_MATCH_MAX_DIFF = 6,
};

enum BM_100_V15
{
	BM_100_15VER_NARROW_PEAK = 19,		//18
	BM_100_15VER_WIDE_PEAK = 82,		//74
	BM_100_15VER_NARROW_DIFF = 6,
	BM_100_15VER_WIDE_DIFF = 9,
	BM_100_15VER_STANDARD_POINT = 2,
	BM_100_15VER_STANDARD_MUTI_POINT = 6,
	BM_100_15VER_MATCH = 0,
	BM_100_15VER_NARROW_MATCH = 0,
	BM_100_15VER_WIDE_MATCH = 1,
	BM_100_15VER_MAX_NUM = 21,
	BM_100_15VER_MIN_NUM = 17,
};

enum BM_5_FORM
{
	BM_5_ZHINENG_FORM_LEN = 4,
	BM_5_ZHINENG_WEAK_FORM_LEN = 3,
	BM_5_FORM_LEN = 6,
	BM_5_WEAK_FORM_LEN = 5,
};
enum BM_5_POINT
{
	BM_5_MIN_POINT = 3,
	BM_5_STANDARD_POINT = 6,
	BM_5_STANDARD_MUTI_POINT = 12,
	BM_5_WIDE_DIFF = 8,
	BM_5_GAP_DIFF = 8,
	BM_5_NARROW_DIFF = 6,
	BM_5_FORM_1 = 23,			//22
	BM_5_FORM_2 = 50,			//41
	BM_5_FORM_3 = 23,			//22
	BM_5_FORM_4 = 50,			//41
	BM_5_FORM_5 = 70,			//63
	BM_5_FORM_6 = 67,			//59
	BM_5_WEAK_FORM_1 = 23,
	BM_5_WEAK_FORM_2 = 49,
	BM_5_WEAK_FORM_3 = 70,
	BM_5_WEAK_FORM_4 = 70,
	BM_5_WEAK_FORM_5 = 70,
	BM_5_FORM_MAX = 12,
	BM_5_WEAK_FORM_MAX = 10,
	BM_5_MAX_NUM = 21,
	BM_5_MIN_NUM = 19,
};

enum BM_20_FORM_
{
	BM_20_v99_FORM_1 = 30,
	BM_20_v99_FORM_1_MAX = 39,	//37
	BM_20_v99_FORM_1_MIN = 29,	//23
	BM_20_v99_FORM_2 = 94,
	BM_20_v99_FORM_2_MIN = 99,	//87
	BM_20_v99_FORM_2_MAX = 109,	//101
	BM_20_v99_FORM_3 = 44,
	BM_20_v99_FORM_3_MAX = 55,	//51
	BM_20_v99_FORM_3_MIN = 45,	//37
};

enum BigGap_About_353
{
	BM_100_BIG_GAP_MAX = BM100_FORM * 2,
	BM_100_15_BIG_GAP_MAX = BM_100_15VER_WIDE_PEAK * 2,
	BM_50_BIG_GAP_MAX = BM50_FORM_2 * 2,
	BM_10_20_BIG_GAP_MAX = BM_1020_WIDE_GAP_MAX * 2,
	BM_10_20_BIG_GAP = 102,//BM_1020_WIDE_GAP_MAX +BM_1020_WIDE_GAP_MAX/2
	BM_5_BIG_GAP_MAX = 124,//BM_5_FORM_6*2 - 10
	BM_5_BIG_GAP = 100,//BM_5_FORM_6 + BM_5_FORM_6/2,
};

extern int BM_GapMidBuf[BM_GAP_NUM_MAX];
extern int BM_GapMidLen;
extern int BM_PeakMidWheelBuf[BM_GAP_NUM_MAX];
extern int BM_PeakMidWheelLen;
extern int BM_GapHighBuf[BM_GAP_NUM_MAX];
extern int BM_GapHighLen;
extern int BM_PeakHighWheelBuf[BM_GAP_NUM_MAX];
extern int BM_PeakHighWheelLen;

extern int BM_check(OneDSignal * pOneDSignal);
extern int PrecisionWheelManageOne(OneDSignal* BMHD_Signal_p, int Index);
#endif
#endif