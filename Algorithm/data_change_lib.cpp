#include "machine_type.h"
#include "analyze_diff_machine.h"

#ifdef JULONG_TMR
#include "julong_tmr/data_change.h"
#endif

#ifdef YUECHUANG
#include "yuechuang/data_change.h"
#endif

#ifdef BAIJIA
#include "baijia/data_change_baijia.h"
#endif

#ifdef RONGHE
#include "ronghe/data_change_ronghe.h"
#endif

#ifdef KANGYI
#include "kangyi/data_change_kangyi.h"
#endif

#ifdef CHUANWEI
#include "chuanwei/data_change_chuanwei.h"
#endif

#define MAX_DROP 10
#define CASH_TOO_NARROW 40
int FindValidStartEnd(OneDSignal *IRData, int *Start, int *End)
{
	int i = 0;
	int BeginLowFlg = 0;
	int EndLowFlg = 0;
	int Flg = 0;
	int VolDropBuf[MAX_DROP * 2] = { 0 };
	int VolRiseBuf[MAX_DROP * 2] = { 0 };
	int SegNum = 0;
	int FirstSeg = 0;
	int LastSeg = 0;
	int SegLenBuf[MAX_DROP * 2] = { 0 };
	int MaxSeg = 0;
	int MaxSegLen = 0;
	if (IRData->Length <= 0)
	{
		return -1;
	}
	if (IRData->Vol[0] < REF_VOL_THRED)
	{
		BeginLowFlg = 1;
	}
	if (IRData->Vol[IRData->Length - 1] < REF_VOL_THRED)
	{
		EndLowFlg = 1;
	}

	for (i = 0; i < IRData->Length - 1; i++)
	{
#if 0//模拟跑抄数据顺序问题，不能打开
		if (IRData->Wheel[i] < LastCashEndWheel)
		{
			continue;
		}
#endif
		if (Flg == 0)
		{
			if (IRData->Vol[i] < REF_VOL_THRED)
			{
				if ((i + IR_CASH_WIDTH_MIN_CNT < IRData->Length) && (IRData->Vol[i + IR_CASH_WIDTH_MIN_CNT] < REF_VOL_THRED))
				{
					VolDropBuf[SegNum] = i;
					Flg = 1;
				}
			}
		}

		if (Flg == 1)
		{
			if (IRData->Vol[i] > REF_VOL_THRED)
			{
				if ((i + IR_CASH_WIDTH_MIN_CNT < IRData->Length) && (IRData->Vol[i + IR_CASH_WIDTH_MIN_CNT] > REF_VOL_THRED))
				{
					VolRiseBuf[SegNum] = i;
					SegNum++;
					Flg = 0;
					if (SegNum >= MAX_DROP)
					{
						break;
					}
				}
			}
		}
	}

	if (Flg == 1)
	{
		VolRiseBuf[SegNum] = IRData->Length - 1;
		SegNum++;
		Flg = 0;
	}

	if (SegNum == 0)
	{
		return -1;
	}
	for (i = 0; i < SegNum; i++)
	{
		SegLenBuf[i] = IRData->Wheel[VolRiseBuf[i]] - IRData->Wheel[VolDropBuf[i]];
	}
	for (i = 0; i < SegNum; i++)
	{
		if (MaxSegLen < SegLenBuf[i])
		{
			MaxSegLen = SegLenBuf[i];
			MaxSeg = i;
		}
	}

	FirstSeg = 0;
	LastSeg = SegNum - 1;
	if (SegNum == 1)
	{
		*Start = VolDropBuf[0];
		*End = VolRiseBuf[0];
	}
	if (SegNum == 2)
	{
#if 0
		if ((BeginLowFlg == 1) || (EndLowFlg == 1))
		{
			*Start = VolDropBuf[MaxSeg];
			*End = VolRiseBuf[MaxSeg];
			return 0;
		}
		*Start = VolDropBuf[FirstSeg];
		*End = VolRiseBuf[LastSeg];
#endif
		*Start = VolDropBuf[MaxSeg];
		*End = VolRiseBuf[MaxSeg];
		return 0;


	}
	if (SegNum >= 3)
	{
		if ((MaxSeg == FirstSeg) || (MaxSeg == LastSeg))
		{
			*Start = VolDropBuf[MaxSeg];
			*End = VolRiseBuf[MaxSeg];
			return 0;
		}
		if ((BeginLowFlg == 1) && (SegLenBuf[FirstSeg] < CASH_TOO_NARROW / 2))
		{
			SegNum--;
			FirstSeg++;
		}
		if ((EndLowFlg == 1) && (SegLenBuf[LastSeg] < CASH_TOO_NARROW / 2))
		{
			SegNum--;
			LastSeg--;
		}
		*Start = VolDropBuf[FirstSeg];
		*End = VolRiseBuf[LastSeg];
	}
	return 0;
}

void IR_DataChange(OneDSignal *AlgSignal_p, TdnData_Alg *AlgData_p, int Lasterror)
{
	int i = 0;
	int ret = 0;
	int validstart = 0;
	int validend = 0;

	OneDSignal IRData_l[IR_NUM] = { 0 };

	IRdataInit(AlgSignal_p, IRData_l);

	for (i = 0; i < IR_NUM; i++)
	{
		validstart = 0;
		validend = 0;
		ret = FindValidStartEnd(&IRData_l[i], &validstart, &validend);

		if (ret < 0)
		{
			AlgData_p->IRData[i].Length = 1;              ///后面可能用到了Length-1，避免异常
			AlgData_p->IRData[i].Vol = IRData_l[i].Vol;
			AlgData_p->IRData[i].Wheel = IRData_l[i].Wheel;
			AlgData_p->IRData[i].Time = IRData_l[i].Time;
		}
		else
		{
			AlgData_p->IRData[i].Length = validend - (validstart + 1);
			AlgData_p->IRData[i].Vol = &IRData_l[i].Vol[validstart];
			AlgData_p->IRData[i].Wheel = &IRData_l[i].Wheel[validstart];
			AlgData_p->IRData[i].Time = &IRData_l[i].Time[validstart];
		}
		validstart = 0;
		validend = 0;
	}

}

void BMHD_DataChange(OneDSignal *TdnData_p, TdnData_Alg *AlgData_p)
{
	int i = 0;
	int StartIndex = 0;
	int EndIndex = 0;
	int BMHD_PrepStartWheel = 0;
	int BMHD_PrepEndwheel = 0;
	int widthLeft;
	int widthRight;

	BMHD_PrepStartWheel = (AlgData_p->IRData[IRRC].Wheel[0] + AlgData_p->IRData[IRLC].Wheel[0]) / 2 - OFFSET_BM_IR_Y - SM_WHEEL_ERR;
	BMHD_PrepEndwheel = (AlgData_p->IRData[IRRC].Wheel[AlgData_p->IRData[IRRC].Length - 1] +
		AlgData_p->IRData[IRLC].Wheel[AlgData_p->IRData[IRLC].Length - 1]) / 2 - OFFSET_BM_IR_Y + SM_WHEEL_ERR;


	for (i = 0; i < TdnData_p->Length; i++)
	{
		if (TdnData_p->Wheel[i] > BMHD_PrepStartWheel)
		{
			if ((TdnData_p->Wheel[i] - BMHD_PrepStartWheel) <= MMSM_ERR_WHEEL)
			{
				StartIndex = i;
				break;
			}
		}

		if (TdnData_p->Wheel[i] == BMHD_PrepStartWheel)
		{
			StartIndex = i;
			break;
		}
	}
	for (i = (TdnData_p->Length - 1); i >= 0; i--)
	{
		if (TdnData_p->Wheel[i] < BMHD_PrepEndwheel)
		{
			if ((BMHD_PrepEndwheel - TdnData_p->Wheel[i]) <= MMSM_ERR_WHEEL)
			{
				EndIndex = i;
				break;
			}
		}

		if (TdnData_p->Wheel[i] == BMHD_PrepEndwheel)
		{
			EndIndex = i;
			break;
		}
	}

	if (StartIndex == 0 && EndIndex == 0)
	{
		AlgData_p->BM.Length = TdnData_p[TDN_BM].Length;
		AlgData_p->BM.Vol = TdnData_p[TDN_BM].Vol;
		AlgData_p->BM.Wheel = TdnData_p[TDN_BM].Wheel;
		AlgData_p->BM.Time = TdnData_p[TDN_BM].Time;

		AlgData_p->HD.Length = TdnData_p[TDN_HD].Length;
		AlgData_p->HD.Vol = TdnData_p[TDN_HD].Vol;
		AlgData_p->HD.Wheel = TdnData_p[TDN_HD].Wheel;
		AlgData_p->HD.Time = TdnData_p[TDN_HD].Time;
	}
	else if (StartIndex == 0 || EndIndex == 0)
	{
		if (StartIndex == 0)
		{
			AlgData_p->BM.Length = EndIndex + 1;
			AlgData_p->BM.Vol = TdnData_p[TDN_BM].Vol;
			AlgData_p->BM.Wheel = TdnData_p[TDN_BM].Wheel;
			AlgData_p->BM.Time = TdnData_p[TDN_BM].Time;

			AlgData_p->HD.Length = EndIndex + 1;
			AlgData_p->HD.Vol = TdnData_p[TDN_HD].Vol;
			AlgData_p->HD.Wheel = TdnData_p[TDN_HD].Wheel;
			AlgData_p->HD.Time = TdnData_p[TDN_HD].Time;
		}
		else if (EndIndex == 0)
		{
			AlgData_p->BM.Length = TdnData_p[TDN_BM].Length - (StartIndex + 1);
			AlgData_p->HD.Length = TdnData_p[TDN_BM].Length - (StartIndex + 1);
			if (AlgData_p->BM.Length < 1)
			{
				AlgData_p->BM.Length = 1;
			}

			AlgData_p->BM.Vol = &TdnData_p[TDN_BM].Vol[StartIndex];
			AlgData_p->BM.Wheel = &TdnData_p[TDN_BM].Wheel[StartIndex];
			AlgData_p->BM.Time = &TdnData_p[TDN_BM].Time[StartIndex];

			AlgData_p->HD.Vol = &TdnData_p[TDN_HD].Vol[StartIndex];
			AlgData_p->HD.Wheel = &TdnData_p[TDN_HD].Wheel[StartIndex];
			AlgData_p->HD.Time = &TdnData_p[TDN_HD].Time[StartIndex];

		}
		else
		{
			AlgData_p->BM.Length = TdnData_p[TDN_BM].Length;
			AlgData_p->BM.Vol = TdnData_p[TDN_BM].Vol;
			AlgData_p->BM.Wheel = TdnData_p[TDN_BM].Wheel;
			AlgData_p->BM.Time = TdnData_p[TDN_BM].Time;

			AlgData_p->HD.Length = TdnData_p[TDN_HD].Length;
			AlgData_p->HD.Vol = TdnData_p[TDN_HD].Vol;
			AlgData_p->HD.Wheel = TdnData_p[TDN_HD].Wheel;
			AlgData_p->HD.Time = TdnData_p[TDN_HD].Time;
		}
	}
	else
	{
		AlgData_p->BM.Length = EndIndex - StartIndex;
		AlgData_p->HD.Length = EndIndex - StartIndex;
		if (AlgData_p->BM.Length < 1)
		{
			AlgData_p->BM.Length = 1;
		}
		AlgData_p->BM.Vol = &TdnData_p[TDN_BM].Vol[StartIndex];
		AlgData_p->BM.Wheel = &TdnData_p[TDN_BM].Wheel[StartIndex];
		AlgData_p->BM.Time = &TdnData_p[TDN_BM].Time[StartIndex];

		AlgData_p->HD.Vol = &TdnData_p[TDN_HD].Vol[StartIndex];
		AlgData_p->HD.Wheel = &TdnData_p[TDN_HD].Wheel[StartIndex];
		AlgData_p->HD.Time = &TdnData_p[TDN_HD].Time[StartIndex];

	}
}