#ifndef	_IR_LIB_H_
#define	_IR_LIB_H_
#include "machine_type.h"
#include "Currency_Check_Manage.h"

#ifdef JULONG_TMR
#include "julong_tmr/macro_cfg.h"
#include "julong_tmr/alg_lib.h"
#include "julong_tmr/data_change.h"
#endif

#ifdef YUECHUANG
#include "yuechuang/macro_cfg.h"
#include "yuechuang/alg_lib.h"
#include "yuechuang/data_change.h"
#endif

#ifdef BAIJIA
#include "baijia/macro_cfg.h"
#include "baijia/alg_lib.h"
#include "baijia/data_change_baijia.h"
#endif

#ifdef RONGHE
#include "ronghe/macro_cfg.h"
#include "ronghe/alg_lib.h"
#include "ronghe/data_change_ronghe.h"
#endif

#ifdef KANGYI
#include "kangyi/macro_cfg.h"
#include "kangyi/alg_lib.h"
#include "kangyi/data_change_kangyi.h"
#endif

#ifdef CHUANWEI
#include "chuanwei/macro_cfg.h"
#include "chuanwei/alg_lib.h"
#include "chuanwei/data_change_chuanwei.h"
#endif

typedef struct Overlap_Info
{
	int DubiousVolNum;		//可疑电压的个数
	int AveDubiousVol;			//可疑电压的平均值
}IR_OVERLAP_INFO;

int GetAngle(TdnData_Alg* AlgData_p);

int GetWidth(TdnData_Alg * AlgData_p);

int GetBackGroundVol(TdnData_Alg * AlgData_p, int Lasterror);

int Overlap_check(TdnData_Alg * AlgData_p, Sensitive *Sensitive_p, int CalibrateVol, int Sensi_Offset);

void CashFirstWheelManage(TdnData_Alg* AlgData_p, WheelData *CashFirstWheel);

#endif