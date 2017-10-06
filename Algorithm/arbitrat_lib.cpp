#include "machine_type.h"
#include "JW_type.h"


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

void BM_AraitratOne(RESULT_RMB *Result, BM_PARAM *BM_Param, int LastError, BM_LEVEL_PARAM *BmLevelParam_p, BM_LEVEL_PARAM *RmbResult_p, int Flg)
{
	unsigned char JudgeBuf[CASH_MAX] = { 0 };
	unsigned char i = 0, max = 0, index = 0;
	unsigned char BrakeFlg = BRAKE_FLG_CLR;
	int angle = 0;
	int width = 0;
	int IfSameFlg = 0;
	if ((LastErrorBuf[IRLC] == 0) && (LastErrorBuf[IRRC] == 0))
	{
		BrakeFlg = BRAKE_FLG_CLR;
	}
	else
	{
		BrakeFlg = BRAKE_FLG_SET;
	}

	angle = AraitratParam.angle;
	width = AraitratParam.Width;

	if ((width <= MAX_100_YUAN_WIDTH) && (width >= MIN_100_YUAN_WIDTH))
	{
		Result->Height = HEIGHT_100;
		AraitratParam.Height = HEIGHT_100;
	}
	if ((width <= MAX_1_5_YUAN_WIDTH) && (width >= MIN_1_5_YUAN_WIDTH))
	{
		Result->Height = HEIGHT_5_1;
		AraitratParam.Height = HEIGHT_5_1;
	}
	if ((width <= MAX_50_20_10_YUAN_WIDTH) && (width >= MIN_50_20_10_YUAN_WIDTH))
	{
		Result->Height = HEIGHT_50_20_10;
		AraitratParam.Height = HEIGHT_50_20_10;
	}
	/*——————————15版100——————————*/
	if ((BM_Param->BM_100_v15_Match >= BM_100_15VER_MAIN_HIGH_SHOLD))// || (BM_Param->BM_100_v15_Wide_Match >= BM_100_15VER_HIGH_SHOLD))
	{
		JudgeBuf[CASH_100_15] = JUDGE_LEVEL_9;
	}
	else if ((BM_Param->BM_100_v15_Match >= BM_100_15VER_MAIN_MID_SHOLD))// || (BM_Param->BM_100_v15_Wide_Match >= BM_100_15VER_MID_SHOLD))
	{
		JudgeBuf[CASH_100_15] = JUDGE_LEVEL_7;
	}
	else if ((BM_Param->BM_100_v15_Match >= BM_100_15VER_MAIN_LOW_SHOLD))// || (BM_Param->BM_100_v15_Wide_Match >= BM_100_15VER_LOW_SHOLD))
	{
		JudgeBuf[CASH_100_15] = JUDGE_LEVEL_5;
	}
	else if ((BM_Param->BM_100_v15_Match >= BM_100_15VER_MAIN_MIN_SHOLD))// || (BM_Param->BM_100_v15_Wide_Match >= BM_100_15VER_MIN_SHOLD))
	{
		JudgeBuf[CASH_100_15] = JUDGE_LEVEL_3;
	}
	else if ((BM_Param->BM_100_v15_Match > 0))// || (BM_Param->BM_100_v15_Wide_Match > 0))
	{
		JudgeBuf[CASH_100_15] = JUDGE_LEVEL_1;
	}
	if (Flg == BM_LOW_THRESHOLD)
	{
		//100
		if (BM_Param->BM_100_Match >= BM_100_HIGH_SHOLD)
		{
			JudgeBuf[CASH_100] = JUDGE_LEVEL_9;
		}
		else if (BM_Param->BM_100_Match >= BM_100_MID_SHOLD)
		{
			JudgeBuf[CASH_100] = JUDGE_LEVEL_7;
		}
		else if (BM_Param->BM_100_Match >= BM_100_LOW_SHOLD)
		{
			JudgeBuf[CASH_100] = JUDGE_LEVEL_5;
		}
		else if (BM_Param->BM_100_Match >= BM_100_MIN_SHOLD)
		{
			JudgeBuf[CASH_100] = JUDGE_LEVEL_3;
		}
		else if (BM_Param->BM_100_Match > 0)
		{
			JudgeBuf[CASH_100] = JUDGE_LEVEL_1;
		}
		//50
		if (BM_Param->BM_50_Match >= BM_50_HIGH_SHOLD)
		{
			JudgeBuf[CASH_50] = JUDGE_LEVEL_9;
		}
		else if (BM_Param->BM_50_Match >= BM_50_MID_SHOLD)
		{
			JudgeBuf[CASH_50] = JUDGE_LEVEL_7;
		}
		else if (BM_Param->BM_50_Match >= BM_50_LOW_SHOLD)
		{
			JudgeBuf[CASH_50] = JUDGE_LEVEL_5;
		}
		else if (BM_Param->BM_50_Match >= BM_50_MIN_SHOLD)
		{
			JudgeBuf[CASH_50] = JUDGE_LEVEL_3;
		}
		else if (BM_Param->BM_50_Match > 0)
		{
			JudgeBuf[CASH_50] = JUDGE_LEVEL_1;
		}
	}
	// 20 10
	if (BM_Param->BM_20_Match > (BM_Param->BM_10_Match + MATCH_COUNT_SURE))
	{
		if (BM_Param->BM_20_Match >= BM_20_HIGH_SHOLD)
		{
			JudgeBuf[CASH_20] = JUDGE_LEVEL_9;
		}
		else if (BM_Param->BM_20_Match >= BM_20_MID_SHOLD)
		{
			JudgeBuf[CASH_20] = JUDGE_LEVEL_7;
		}
		else if (BM_Param->BM_20_Match >= BM_20_LOW_SHOLD)
		{
			JudgeBuf[CASH_20] = JUDGE_LEVEL_5;
		}
		else if (BM_Param->BM_20_Match >= BM_20_MIN_SHOLD)
		{
			JudgeBuf[CASH_20] = JUDGE_LEVEL_3;
		}
		else if (BM_Param->BM_20_Match > 0)
		{
			JudgeBuf[CASH_20] = JUDGE_LEVEL_1;
		}
	}
	else if (BM_Param->BM_10_Match > (BM_Param->BM_20_Match + MATCH_COUNT_SURE))
	{
		if (BM_Param->BM_10_Match >= BM_10_HIGH_SHOLD)
		{
			JudgeBuf[CASH_10] = JUDGE_LEVEL_9;
		}
		else if (BM_Param->BM_10_Match >= BM_10_MID_SHOLD)
		{
			JudgeBuf[CASH_10] = JUDGE_LEVEL_7;
		}
		else if (BM_Param->BM_10_Match >= BM_10_LOW_SHOLD)
		{
			JudgeBuf[CASH_10] = JUDGE_LEVEL_5;
		}
		else if (BM_Param->BM_10_Match >= BM_10_MIN_SHOLD)
		{
			JudgeBuf[CASH_10] = JUDGE_LEVEL_3;
		}
		else if (BM_Param->BM_10_Match > 0)
		{
			JudgeBuf[CASH_10] = JUDGE_LEVEL_1;
		}
	}

	// 5
	if (BM_Param->BM_5_Match >= BM_5_MAIN_HIGH_SHOLD)
	{
		JudgeBuf[CASH_5] = JUDGE_LEVEL_9;
	}
	else if (BM_Param->BM_5_Match >= BM_5_MAIN_MID_SHOLD)
	{
		JudgeBuf[CASH_5] = JUDGE_LEVEL_7;
	}
	else if (BM_Param->BM_5_Match >= BM_5_MAIN_LOW_SHOLD)
	{
		JudgeBuf[CASH_5] = JUDGE_LEVEL_5;
	}
	else if (BM_Param->BM_5_Match >= BM_5_MAIN_MIN_SHOLD)
	{
		JudgeBuf[CASH_5] = JUDGE_LEVEL_3;
	}
	else if (BM_Param->BM_5_Match > 0)
	{
		JudgeBuf[CASH_5] = JUDGE_LEVEL_1;
	}

	// 99 ban 20
	if (BM_Param->BM_20_v99_Match == BM_20)
	{
		JudgeBuf[CASH_20_99] = JUDGE_LEVEL_7;
	}
	RmbResult_p->BM_100_15 = JudgeBuf[CASH_100_15];
	RmbResult_p->BM_100 = JudgeBuf[CASH_100];
	RmbResult_p->BM_50 = JudgeBuf[CASH_50];
	RmbResult_p->BM_20 = JudgeBuf[CASH_20];
	RmbResult_p->BM_20_99 = JudgeBuf[CASH_20_99];
	RmbResult_p->BM_10 = JudgeBuf[CASH_10];
	RmbResult_p->BM_5 = JudgeBuf[CASH_5];
	RmbResult_p->BM_GAP = BmLevelParam_p->BM_GAP;


	//wide add match
	if ((BrakeFlg == BRAKE_FLG_CLR) && (angle < IF_ANGLE_ADD_MATCH))
	{
		if ((width <= MAX_100_YUAN_WIDTH) && (width >= MIN_100_YUAN_WIDTH))
		{
			JUDGE_WIDE_ADD_MATCH(JudgeBuf[CASH_100]);
			JUDGE_WIDE_ADD_MATCH(JudgeBuf[CASH_100_15]);
			if (JudgeBuf[CASH_5] >= 2)
			{
				JudgeBuf[CASH_5] -= 2;
			}
			else
			{
				JudgeBuf[CASH_5] = 0;
			}
		}
		if ((width <= MAX_1_5_YUAN_WIDTH) && (width >= MIN_1_5_YUAN_WIDTH))
		{
			JUDGE_WIDE_ADD_MATCH(JudgeBuf[CASH_5]);
			if (JudgeBuf[CASH_100] > 0)
			{
				JudgeBuf[CASH_100]--;
			}
			if (JudgeBuf[CASH_100_15] > 0)
			{
				JudgeBuf[CASH_100_15]--;
			}
		}
		if ((width <= MAX_50_20_10_YUAN_WIDTH) && (width >= MIN_50_20_10_YUAN_WIDTH))
		{
			JUDGE_WIDE_ADD_MATCH(JudgeBuf[CASH_50]);
			JUDGE_WIDE_ADD_MATCH(JudgeBuf[CASH_10]);
			JUDGE_WIDE_ADD_MATCH(JudgeBuf[CASH_20]);
			JUDGE_WIDE_ADD_MATCH(JudgeBuf[CASH_20_99]);

			if (JudgeBuf[CASH_5] >= 2)
			{
				JudgeBuf[CASH_5] -= 2;
			}
			else
			{
				JudgeBuf[CASH_5] = 0;
			}
		}
	}

	//judge
	for (i = CASH_100_15; i < CASH_MAX; i++)
	{
		if (JudgeBuf[i] == max)
		{
			IfSameFlg++;
		}
		if (JudgeBuf[i] > max)
		{
			max = JudgeBuf[i];
			index = i;
			IfSameFlg = 0;
		}

	}
	BmLevelParam_p->BM_100_15 = JudgeBuf[CASH_100_15];
	BmLevelParam_p->BM_100 = JudgeBuf[CASH_100];
	BmLevelParam_p->BM_50 = JudgeBuf[CASH_50];
	BmLevelParam_p->BM_20 = JudgeBuf[CASH_20];
	BmLevelParam_p->BM_20_99 = JudgeBuf[CASH_20_99];
	BmLevelParam_p->BM_10 = JudgeBuf[CASH_10];
	BmLevelParam_p->BM_5 = JudgeBuf[CASH_5];

	if (IfSameFlg > 0)
	{
		BM_Param->CashMaxLevel = max;
		BM_Param->CashMaxIndex = CASH_NO_MATCH;
	}
	else
	{
		BM_Param->CashMaxLevel = max;
		BM_Param->CashMaxIndex = index;
	}

	return;
}

void BM_AraitratManage(RESULT_RMB *Result, int LastError, FuncSwitch *funcswitch, Sensitive *Sensitive_p)
{
	int max = 0;
	int index = 0;
	int angle = 0;
	int width = 0;
	int BrakeFlg = 0;
	int BM_GapNum = 0;
	int MinLevel = 0;
	int Match20 = 0;
	int Match10 = 0;

	width = AraitratParam.Width;
	if ((LastErrorBuf[IRLC] - LastErrorBuf[IRRC]) == 0)
	{
		angle = AraitratParam.angle;
	}
	else
	{
		angle = 0;
	}


	if (LastErrorBuf[LAST_ERR_INDEX] == 0)
	{
		BrakeFlg = BRAKE_FLG_CLR;
	}
	else
	{
		BrakeFlg = BRAKE_FLG_SET;
	}


	if (BrakeFlg == BRAKE_FLG_CLR)
	{
		if (Sensitive_p->Err_351 > 9)
		{
			MinLevel = JUDGE_LEVEL_0;
		}
		if (Sensitive_p->Err_351 >= JUDGE_LEVEL_7)
		{
			MinLevel = JUDGE_LEVEL_9;
		}
		else if (Sensitive_p->Err_351 >= JUDGE_LEVEL_5)
		{
			MinLevel = JUDGE_LEVEL_7;
		}
		else if (Sensitive_p->Err_351 >= JUDGE_LEVEL_3)
		{
			MinLevel = JUDGE_LEVEL_5;
		}
		else if (Sensitive_p->Err_351 >= JUDGE_LEVEL_1)
		{
			MinLevel = JUDGE_LEVEL_3;
		}
		else if (Sensitive_p->Err_351 >= JUDGE_LEVEL_0)
		{
			MinLevel = JUDGE_LEVEL_0;
		}
	}
	else
	{
		MinLevel = JUDGE_LEVEL_0;
	}
#ifdef JULONG_TMR
	MinLevel = JUDGE_LEVEL_0;
#endif
	if (AraitratParam.BM_High_Param.CashMaxIndex == 0)
	{
		max = AraitratParam.BM_Mid_Param.CashMaxLevel;
		index = AraitratParam.BM_Mid_Param.CashMaxIndex;
		BM_GapNum = AraitratParam.BM_Mid_Param.BM_GapNum;
	}
	else if (AraitratParam.BM_Mid_Param.CashMaxIndex == 0)
	{
		max = AraitratParam.BM_High_Param.CashMaxLevel;
		index = AraitratParam.BM_High_Param.CashMaxIndex;
		BM_GapNum = AraitratParam.BM_High_Param.BM_GapNum;
	}
	else if (AraitratParam.BM_High_Param.CashMaxLevel > AraitratParam.BM_Mid_Param.CashMaxLevel)
	{
		max = AraitratParam.BM_High_Param.CashMaxLevel;
		index = AraitratParam.BM_High_Param.CashMaxIndex;
		BM_GapNum = AraitratParam.BM_High_Param.BM_GapNum;
	}
	else if (AraitratParam.BM_High_Param.CashMaxLevel < AraitratParam.BM_Mid_Param.CashMaxLevel)
	{
		max = AraitratParam.BM_Mid_Param.CashMaxLevel;
		index = AraitratParam.BM_Mid_Param.CashMaxIndex;
		BM_GapNum = AraitratParam.BM_Mid_Param.BM_GapNum;
	}
	else if (AraitratParam.BM_High_Param.CashMaxLevel == AraitratParam.BM_Mid_Param.CashMaxLevel)
	{
		if (AraitratParam.BM_High_Param.CashMaxLevel == JUDGE_LEVEL_0)
		{
			max = 0;
			index = CASH_NO_MATCH;
			BM_GapNum = AraitratParam.BM_Mid_Param.BM_GapNum;
		}
		else if (AraitratParam.BM_High_Param.CashMaxIndex == AraitratParam.BM_Mid_Param.CashMaxIndex)
		{

			max = AraitratParam.BM_High_Param.CashMaxLevel;
			index = AraitratParam.BM_High_Param.CashMaxIndex;
			if (AraitratParam.BM_High_Param.BM_GapNum > AraitratParam.BM_Mid_Param.BM_GapNum)
			{
				BM_GapNum = AraitratParam.BM_High_Param.BM_GapNum;
			}
			else
			{
				BM_GapNum = AraitratParam.BM_Mid_Param.BM_GapNum;
			}

		}
		else if (((AraitratParam.BM_High_Param.CashMaxIndex == CASH_10) || (AraitratParam.BM_High_Param.CashMaxIndex == CASH_20))
			&& (((AraitratParam.BM_Mid_Param.CashMaxIndex == CASH_10) || (AraitratParam.BM_Mid_Param.CashMaxIndex == CASH_20))))
		{
			Match20 = AraitratParam.BM_High_Param.BM_20_Match + AraitratParam.BM_Mid_Param.BM_20_Match;
			Match10 = AraitratParam.BM_High_Param.BM_10_Match + AraitratParam.BM_Mid_Param.BM_10_Match;
			if ((Match20 - Match10) > MACTH_COUNT_SURE_FIANL)
			{
				max = AraitratParam.BM_Mid_Param.CashMaxLevel;
				index = CASH_20;
				BM_GapNum = AraitratParam.BM_Mid_Param.BM_GapNum;
			}
			else if ((Match10 - Match20) > MACTH_COUNT_SURE_FIANL)
			{
				max = AraitratParam.BM_Mid_Param.CashMaxLevel;
				index = CASH_10;
				BM_GapNum = AraitratParam.BM_Mid_Param.BM_GapNum;
			}
			else
			{
				max = JUDGE_LEVEL_0;
				index = CASH_NO_MATCH;
				BM_GapNum = AraitratParam.BM_Mid_Param.BM_GapNum;
			}
		}
		else if ((AraitratParam.BM_High_Param.CashMaxIndex == CASH_100) && (AraitratParam.BM_Mid_Param.CashMaxIndex == CASH_5))
		{
			max = AraitratParam.BM_High_Param.CashMaxLevel;
			index = AraitratParam.BM_High_Param.CashMaxIndex;
			BM_GapNum = AraitratParam.BM_High_Param.BM_GapNum;
		}
		else
		{
			if ((AraitratParam.BM_High_Param.CashMaxIndex == CASH_100) || (AraitratParam.BM_High_Param.CashMaxIndex == CASH_100_15)
				|| (AraitratParam.BM_High_Param.CashMaxIndex == CASH_50))
			{
				max = AraitratParam.BM_Mid_Param.CashMaxLevel;
				index = AraitratParam.BM_Mid_Param.CashMaxIndex;
				BM_GapNum = AraitratParam.BM_Mid_Param.BM_GapNum;
			}
			else
			{
				max = AraitratParam.BM_High_Param.CashMaxLevel;
				index = AraitratParam.BM_High_Param.CashMaxIndex;
				BM_GapNum = AraitratParam.BM_High_Param.BM_GapNum;
			}
		}



	}

	if (width < CASH_TOO_NARROW)
	{
		Result->ERRtype = JW_OTHER_INCOMPLETE;
		return;
	}
	if ((width > CASH_TOO_WIDTH) && (BrakeFlg == BRAKE_FLG_CLR))
	{
		Result->ERRtype = JW_OTHER_CON;
		return;
	}

	if ((max <= JUDGE_LEVEL_5) && (angle >= IF_ANGLE_SLOPE))
	{
		Result->ERRtype = JW_OTHER_SLOPE;
		return;
	}
	else if (((index == CASH_10) || (index == CASH_20)) && (angle >= IF_ANGLE_SLOPE))
	{
		Result->ERRtype = JW_OTHER_SLOPE;
		return;
	}
	else if (max <= MIN_CASH_LEVEL)
	{
		if (angle >= IF_ANGLE_SLOPE)
		{
			Result->ERRtype = JW_OTHER_SLOPE;
			return;
		}
		if (BM_GapNum < BM_GAP_MIN)
		{
			Result->ERRtype = JW_BM_HD_NOBM;
			return;
		}
		Result->ERRtype = JW_BM_HD_MISALL;
		return;
	}

	switch (index)
	{
	case CASH_100:
	{
					 Result->Amount = BM_100;
					 break;
	}
	case CASH_100_15:
	{
						if ((funcswitch->RMB_4th == 1) && (max <= 3))
						{
							Result->Amount = BM_ERROR;
							Result->ERRtype = JW_BM_HD_MISALL;
						}
						else
						{
							Result->Amount = BM_100;
							Result->VersionM = TDN_VER_15;
						}
						break;
	}
	case CASH_50:
	{
					Result->Amount = BM_50;
					break;
	}
	case CASH_20:
	{
					Result->Amount = BM_20;
					Result->VersionM = TDN_VER_05;
					break;
	}
	case CASH_20_99:
	{
					   Result->Amount = BM_20;
					   Result->VersionM = TDN_VER_99;
					   break;
	}
	case CASH_10:
	{
					Result->Amount = BM_10;
					break;
	}
	case CASH_5:
	{
				   Result->Amount = BM_5;
				   break;
	}
	case CASH_1:
	{
				   Result->Amount = BM_1;
				   break;
	}
	default:
	{
			   Result->Amount = BM_ERROR;
			   Result->ERRtype = JW_BM_HD_MISALL;
	}
	}
	if ((BM_GapNum < BM_GAP_MIN) && (Result->Amount != BM_5))
	{
		Result->ERRtype = JW_BM_HD_NOBM;
		Result->Amount = 0;
		Result->VersionM = TDN_VER_NONE;
	}
	else if ((max < MinLevel) && (BM_GapNum < BM_GAP_MIN) && (Result->Amount == BM_5))
	{
		Result->ERRtype = JW_BM_HD_NOBM;
	}
	else if (max < MinLevel)
	{
		Result->ERRtype = JW_BM_HD_MISALL;
	}
	// big gap manage
	else if ((Sensitive_p->Err_353 > 0) && (LastErrorBuf[LAST_ERR_INDEX] == 0))
	{
		if ((Result->Amount == BM_100) && (Result->VersionM != TDN_VER_15))
		{
			if ((AraitratParam.BM_Mid_Param.BM_BigGapNum.BM_100_BigGapNum > 0) && (AraitratParam.BM_High_Param.BM_BigGapNum.BM_100_BigGapNum > 0))
			{
				Result->ERRtype = JW_BM_100_LOSE_PEAK;
			}
		}
		else if ((Result->Amount == BM_100) && (Result->VersionM == TDN_VER_15))
		{
			if ((AraitratParam.BM_Mid_Param.BM_BigGapNum.BM_100_15_BigGapNum > 0) && (AraitratParam.BM_High_Param.BM_BigGapNum.BM_100_15_BigGapNum > 0))
			{
				Result->ERRtype = JW_BM_100_LOSE_PEAK;
			}
		}
		else if (Result->Amount == BM_50)
		{
			if ((AraitratParam.BM_Mid_Param.BM_BigGapNum.BM_50_BigGapNum > 0) && (AraitratParam.BM_High_Param.BM_BigGapNum.BM_50_BigGapNum > 0))
			{
				Result->ERRtype = JW_BM_100_LOSE_PEAK;
			}
		}
		else if (Result->Amount == BM_5)
		{
			if ((AraitratParam.BM_Mid_Param.BM_BigGapNum.BM_5_BigGapNum > 0) && (AraitratParam.BM_High_Param.BM_BigGapNum.BM_5_BigGapNum > 0))
			{
				Result->ERRtype = JW_BM_100_LOSE_PEAK;
			}
			if ((AraitratParam.BM_Mid_Param.BM_BigGapNum.BM_5_MidGapNum > 1) && (AraitratParam.BM_High_Param.BM_BigGapNum.BM_5_MidGapNum > 1))
			{
				Result->ERRtype = JW_BM_100_LOSE_PEAK;
			}
		}
		else if (Result->Amount == BM_10)
		{
			if ((AraitratParam.BM_Mid_Param.BM_BigGapNum.BM_10_BigGapNum > 0) && (AraitratParam.BM_High_Param.BM_BigGapNum.BM_10_BigGapNum > 0))
			{
				Result->ERRtype = JW_BM_100_LOSE_PEAK;
			}
			if ((AraitratParam.BM_Mid_Param.BM_BigGapNum.BM_10_MidGapNum > 1) && (AraitratParam.BM_High_Param.BM_BigGapNum.BM_10_MidGapNum > 1))
			{
				Result->ERRtype = JW_BM_100_LOSE_PEAK;
			}
		}
		else if ((Result->VersionM == TDN_VER_05) && (Result->Amount == BM_20))
		{
			if ((AraitratParam.BM_Mid_Param.BM_BigGapNum.BM_10_BigGapNum > 0) && (AraitratParam.BM_High_Param.BM_BigGapNum.BM_10_BigGapNum > 0))
			{
				Result->ERRtype = JW_BM_100_LOSE_PEAK;
			}
			if ((AraitratParam.BM_Mid_Param.BM_BigGapNum.BM_10_MidGapNum > 1) && (AraitratParam.BM_High_Param.BM_BigGapNum.BM_10_MidGapNum > 1))
			{
				Result->ERRtype = JW_BM_100_LOSE_PEAK;
			}
		}
	}

}

void BM_Araitrat(RESULT_RMB *RMBresult, int LastError, FuncSwitch *funcswitch, Sensitive *Sensitive_p)
{
	BM_AraitratOne(RMBresult, &AraitratParam.BM_High_Param, LastError, &IntermediateResult.BM_LevelHigh, &AraitratParam.LevelNoWidthHigh, BM_HIGH_THRESHOLD);

	BM_AraitratOne(RMBresult, &AraitratParam.BM_Mid_Param, LastError, &IntermediateResult.BM_LevelMid, &AraitratParam.LevelNoWidthMid, BM_LOW_THRESHOLD);

	BM_AraitratManage(RMBresult, LastError, funcswitch, Sensitive_p);
}

void ChongZhangRejudge(RESULT_RMB *RMBresult, int flg)
{
	int BrakeFlg = 0;

	if (flg == 0)
	{
		return;
	}

	if (RMBresult->ERRtype == JW_BM_HD_NOBM)
	{
		return;
	}
	if (RMBresult->ERRtype != 0)
	{
		RMBresult->ERRtype = JW_IR_ZHAN_LIAN;
	}

	return;
}

void Overlap_Arbitrat(RESULT_RMB* RMBresult, Sensitive *Sensitive_p, int DubiousVolNum, int Sensi_Offset_Cnt, int Dubious_IR_Thred)
{
	int i = 0;
	int IR_DubiousChannelCnt = 0;			//重张可疑红外通道
	int IR_InvalidChannel = 0;
	int Dubious_VolNum_Thred = 0;
	if (Sensitive_p->Err_256)
	{
		Dubious_VolNum_Thred = DubiousVolNum + (5 - Sensitive_p->Err_256) * Sensi_Offset_Cnt;
	}
	else
	{
		return;
	}
	for (i = 0; i < IR_NUM; i++)
	{
		if (AraitratParam.Width_IR[i] < CASH_TOO_NARROW)
		{
			IR_InvalidChannel++;
		}
		if (AraitratParam.OverlapVol_Info[i].DubiousVolNum > Dubious_VolNum_Thred)
		{
			IR_DubiousChannelCnt++;
		}
	}
#ifndef ITENAL
	if (IR_InvalidChannel >= Dubious_IR_Thred)
	{
		Dubious_IR_Thred--;
	}
#endif
	if (IR_DubiousChannelCnt > Dubious_IR_Thred)			//Dubious_IR_Cnt这个值尽量别改
	{
		RMBresult->ERRtype = JW_IR_ZHAN_LIAN;
	}

#if 0
	FILE *myfile1;
	unsigned char MatlabChar;

	myfile1 = fopen("C:\\123456\\ChannelCnt.txt", "ab+");
	MatlabChar = (unsigned char)IR_DubiousChannelCnt;
	fwrite(&MatlabChar, sizeof(MatlabChar), 1, myfile1);
	fclose(myfile1);
#endif

}