//#include <stdafx.h>
#include "mmsm_lib.h"
#include "math.h"

#ifdef JULONG_TMR
#include "julong_tmr/arbitrat.h"
#endif

#ifdef YUECHUANG
#include "yuechuang/arbitrat.h"
#endif

#ifdef BAIJIA
#include "baijia/arbitrat.h"
#endif

#ifdef RONGHE
#include "ronghe/arbitrat.h"
#endif

#ifdef KANGYI
#include "kangyi/arbitrat.h"
#endif

#ifdef CHUANWEI
#include "chuanwei/arbitrat.h"
#endif

#ifdef MMSM_LIB

int FinMaxPeakVol(OneDSignal *pOneDSignal_p, int Start, int End)
{
	int i = 0;
	int TempVol = 0;
	for (i = Start; i <= End; i++)
	{
		if (pOneDSignal_p->Vol[i] > TempVol)
		{
			TempVol = pOneDSignal_p->Vol[i];
		}
	}
	return TempVol;
}

int FinMinPeakVol(OneDSignal *pOneDSignal_p, int Start, int End)
{
	int i = 0;
	int Jump = 0;
	int TempVol = 5000;
	for (i = Start; i <= End; i++)
	{
		if (pOneDSignal_p->Vol[i] < TempVol)
		{
			TempVol = pOneDSignal_p->Vol[i];
		}
	}
	return TempVol;
}

void SM_FindUpDownPeak(OneDSignal *SM_Singnal_p, MMSM_WHEEL_PARAM *pMMSM_WheelParam, int Up_Threshold, int Down_Threshold)
{
	int i = 0;
	int j = 0;
	int Jump = 0;
	int Max = 0;
	int PeakPosBuf[MMSM_POS_BUF_MAX] = { 0 };		//上半周
	int PeakPosLen = 0;
	int Up_RiseFallFlg = 0;
	int Down_RiseFallFlg = 0;
	int WheelBufLen = 0;
	int VolBufLen = 0;
	int SM_WheelBuf[MMSM_POS_BUF_MAX] = { 0 };
	int SM_VolBuf[MMSM_POS_BUF_MAX] = { 0 };
	int UpDownFlgBuf[MMSM_POS_BUF_MAX] = { 0 };
	EDGE_DETECTION_VOID(SM_Singnal_p->Length, 1);
	pMMSM_WheelParam->Vol = SM_Singnal_p->Vol;
	pMMSM_WheelParam->Length = SM_Singnal_p->Length;
	for (i = 0; i < SM_Singnal_p->Length; i++)
	{
		if (Up_RiseFallFlg == 1)			//寻找上半周的上升沿码盘
		{
			if ((SM_Singnal_p->Vol[i + 1] < Up_Threshold) && (SM_Singnal_p->Vol[i] >= Up_Threshold))
			{
				UpDownFlgBuf[PeakPosLen >> 1] = 1;
				PeakPosBuf[PeakPosLen] = i + 1;
				PeakPosLen++;
				Up_RiseFallFlg = 0;
			}
		}
		if (Down_RiseFallFlg == 1)			//寻找下半周的下降沿码盘
		{
			if ((SM_Singnal_p->Vol[i + 1] > Down_Threshold) && (SM_Singnal_p->Vol[i] <= Down_Threshold))
			{
				UpDownFlgBuf[PeakPosLen >> 1] = 0;
				PeakPosBuf[PeakPosLen] = i + 1;
				PeakPosLen++;
				Down_RiseFallFlg = 0;
			}
		}
		if (Down_RiseFallFlg == 0)			//寻找上半周的上升沿码盘
		{
			if ((SM_Singnal_p->Vol[i] < Up_Threshold) && (SM_Singnal_p->Vol[i + 1] >= Up_Threshold))
			{
				PeakPosBuf[PeakPosLen] = i;
				PeakPosLen++;
				Up_RiseFallFlg = 1;
			}
		}
		if (Up_RiseFallFlg == 0)			//寻找下半周的下降沿码盘
		{
			if ((SM_Singnal_p->Vol[i] > Down_Threshold) && (SM_Singnal_p->Vol[i + 1] <= Down_Threshold))
			{
				PeakPosBuf[PeakPosLen] = i;
				PeakPosLen++;
				Down_RiseFallFlg = 1;
			}
		}
		if (PeakPosLen >= MMSM_POS_BUF_MAX)
		{
			break;
		}
	}
	if (PeakPosLen < 1)
	{
		return;
	}
	for (i = 0; i < PeakPosLen; i += 2)
	{
		Jump = 0;
		if (i < PeakPosLen) 		//上半周存在峰值
		{
			if (UpDownFlgBuf[i >> 1] == 1)
			{
				for (j = PeakPosBuf[i]; j <= PeakPosBuf[i + 1]; j++)
				{
					if (SM_Singnal_p->Vol[j] > Up_Threshold)
					{
						Jump++;
					}
				}
				if (Jump > MMSM_PEAK_MIN_POINT)
				{
					SM_WheelBuf[WheelBufLen] = PrecisionWheelManageOne(SM_Singnal_p, PeakPosBuf[i]);
					WheelBufLen++;
					SM_WheelBuf[WheelBufLen] = PrecisionWheelManageOne(SM_Singnal_p, PeakPosBuf[i + 1]);
					WheelBufLen++;
					SM_VolBuf[VolBufLen] = FinMaxPeakVol(SM_Singnal_p, PeakPosBuf[i], PeakPosBuf[i + 1]);
					VolBufLen++;
				}
			}
			else if (UpDownFlgBuf[i >> 1] == 0)
			{
				for (j = PeakPosBuf[i]; j <= PeakPosBuf[i + 1]; j++)
				{
					if (SM_Singnal_p->Vol[j] < Up_Threshold)
					{
						Jump++;
					}
				}
				if (Jump > MMSM_PEAK_MIN_POINT)
				{
					SM_WheelBuf[WheelBufLen] = PrecisionWheelManageOne(SM_Singnal_p, PeakPosBuf[i]);
					WheelBufLen++;
					SM_WheelBuf[WheelBufLen] = PrecisionWheelManageOne(SM_Singnal_p, PeakPosBuf[i + 1]);
					WheelBufLen++;
					SM_VolBuf[VolBufLen] = FinMinPeakVol(SM_Singnal_p, PeakPosBuf[i], PeakPosBuf[i + 1]);
					VolBufLen++;
				}
			}
		}
		if (WheelBufLen >= MMSM_POS_BUF_MAX)
		{
			break;
		}
	}
	memcpy(pMMSM_WheelParam->PeakVolBuf, SM_VolBuf, WheelBufLen*sizeof(int));
	pMMSM_WheelParam->PeakVolLen = VolBufLen;
	memcpy(pMMSM_WheelParam->PrecisionWheelBuf, SM_WheelBuf, WheelBufLen*sizeof(int));
	pMMSM_WheelParam->PrecisionWheelLen = WheelBufLen;
	pMMSM_WheelParam->WheelOffset = CashFirstWheel;
#ifdef MMSM_DEBUG
	memcpy(pMMSM_WheelParam->PrecisionIndexBuf, PeakPosBuf, PeakPosLen*sizeof(int));
	pMMSM_WheelParam->PrecisionIndexLen = PeakPosLen;
#endif
	return;
}

#define MM_SM_MAX_THRED 2500
#define MM_SM_MIN_THRED 600
void MagImg_AllMagCheck(TdnData_Alg *AlgData_p)
{
	int i = 0;
	SM_FindUpDownPeak(&AlgData_p->BM, &AraitratParam.BM_WheelParam, 3500, 300);

	SM_FindUpDownPeak(&AlgData_p->LMM[0], &AraitratParam.LMM_WheelParam[0], MM_SM_MAX_THRED, MM_SM_MIN_THRED);
	SM_FindUpDownPeak(&AlgData_p->LMM[1], &AraitratParam.LMM_WheelParam[1], MM_SM_MAX_THRED, MM_SM_MIN_THRED);
	SM_FindUpDownPeak(&AlgData_p->RMM[0], &AraitratParam.RMM_WheelParam[0], MM_SM_MAX_THRED, MM_SM_MIN_THRED);
	SM_FindUpDownPeak(&AlgData_p->RMM[1], &AraitratParam.RMM_WheelParam[1], MM_SM_MAX_THRED, MM_SM_MIN_THRED);

	for (i = 0; i < SM_NUM; i++)
	{
		SM_FindUpDownPeak(&AlgData_p->LSM[i], &AraitratParam.LSM_TMR_WheelParam[i], MM_SM_MAX_THRED, MM_SM_MIN_THRED);
		SM_FindUpDownPeak(&AlgData_p->RSM[i], &AraitratParam.RSM_TMR_WheelParam[i], MM_SM_MAX_THRED, MM_SM_MIN_THRED);
	}
}

#endif