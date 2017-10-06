#ifndef _JULONG_DATACHANGE_TMR_H_
#define JULONG_DATACHANGE_TMR_H_
#include "machine_type.h"
#include "analyze_diff_machine.h"

#ifdef JULONG_TMR

#include "macro_cfg.h"
#include "alg_lib.h"

#define RAW_NUM  2


void IRdataInit(OneDSignal *AlgSignal_p, OneDSignal* IRdata);
void IR_DataChange(OneDSignal *AlgSignal_p, TdnData_Alg *AlgData_p, int Lasterror);
void BMHD_DataChange(OneDSignal *AlgSignal_p, TdnData_Alg *AlgData_p);

extern void DataChange(int LastError);

#endif
#endif