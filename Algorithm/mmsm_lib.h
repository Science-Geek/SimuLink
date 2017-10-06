#ifndef	_MMSM_LIB_H
#define	_MMSM_LIB_H
#include "machine_type.h"

#ifdef JULONG_TMR
#include "macro_cfg.h"
#include "alg_lib.h"
#include "julong_tmr/data_change.h"
#endif

#ifdef YUECHUANG
#include "macro_cfg.h"
#include "alg_lib.h"
#include "yuechuang/data_change.h"
#endif

#ifdef BAIJIA
#include "macro_cfg.h"
#include "alg_lib.h"
#include "baijia/data_change_baijia.h"
#endif

#ifdef RONGHE
#include "macro_cfg.h"
#include "alg_lib.h"
#include "ronghe/data_change_ronghe.h"
#endif

#ifdef KANGYI
#include "macro_cfg.h"
#include "alg_lib.h"
#include "kangyi/data_change_kangyi.h"
#endif

#ifdef CHUANWEI
#include "macro_cfg.h"
#include "alg_lib.h"
#include "chuanwei/data_change_chuanwei.h"
#endif

#define MMSM_POS_BUF_MAX 400   
#define MMSM_PEAK_MIN_POINT	1		//峰最低保持点数

typedef struct OneSholdInfo 
{
	int MidWheel;
	int PulseNum;
	int PulseWidth;
	
}ONE_SHOLD_INFO;
//中、边磁结果结构体
typedef struct MMSMResult_half
{
	ONE_SHOLD_INFO HighInfo;
	ONE_SHOLD_INFO LowInfo;
	ONE_SHOLD_INFO LowerInfo;
	int Exist;
	int Level;
	int PosPer;
	int WidthPer;
	int StartWheel;
	int EndWheel;
}MMSM_RESULT_HALF;

 //中磁结果结构体
typedef struct tagMMResult
{
	int Mean_Ecode;
	int Num_Pulse;
	int Width_PulseGroup;
}MMResult;
typedef struct MMSM_PrecisionWheelParam
{
	int PeakVolBuf[MMSM_POS_BUF_MAX / 2];
	int PeakVolLen;
	int PrecisionWheelBuf[MMSM_POS_BUF_MAX];
	int PrecisionWheelLen;
	int WheelOffset;
	int* Vol;
	int Length;
#ifdef MMSM_DEBUG
	int PrecisionIndexBuf[MMSM_POS_BUF_MAX];
	int PrecisionIndexLen;
#endif
}MMSM_WHEEL_PARAM;
extern int MMSM_check(TdnData_Alg *AlgData_p);

extern void MagImg_AllMagCheck(TdnData_Alg *AlgData_p);

#endif
