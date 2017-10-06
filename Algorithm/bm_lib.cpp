#include "machine_type.h"

#ifdef JULONG_TMR
#include "julong_tmr/arbitrat.h"
#include "julong_tmr/bm_lib.h"
#endif

#ifdef YUECHUANG
#include "yuechuang/arbitrat.h"
#include "yuechuang/bm_lib.h"
#endif

#ifdef BAIJIA
#include "baijia/arbitrat.h"
#include "baijia/bm_lib.h"
#endif

#ifdef RONGHE
#include "ronghe/arbitrat.h"
#include "ronghe/bm_lib.h"
#endif

#ifdef KANGYI
#include "kangyi/arbitrat.h"
#include "kangyi/bm_lib.h"
#endif

#ifdef CHUANWEI
#include "chuanwei/arbitrat.h"
#include "chuanwei/bm_lib.h"
#endif

int BM_GapMidBuf[BM_GAP_NUM_MAX] = { 0 };
int BM_GapMidLen = 0;

int BM_GapHighBuf[BM_GAP_NUM_MAX] = { 0 };
int BM_GapHighLen = 0;

int BM_PeakMidWheelBuf[BM_GAP_NUM_MAX] = { 0 };
int BM_PeakMidWheelLen = 0;

int BM_PeakHighWheelBuf[BM_GAP_NUM_MAX] = { 0 };
int BM_PeakHighWheelLen = 0;

void BM_Check_100(int *BM_GapBuf, int BM_GapLen, BM_PARAM* BM_Param_p)
{
	int i = 0;		/// 循环变量							
	int MatchNum = 0;
	int Match = 0;
	int MatchBuf[BM_GAP_NUM_MAX] = { 0 };

	if (BM_GapLen == 0)
	{
		return;
	}
	for (i = 0; i < BM_GapLen; i++)
	{
		MatchBuf[i] = GAP_DIFF(BM_GapBuf[i], BM100_FORM);
	}

	// 统计BM间距与100特征的分段匹配度
	for (i = 0; i < BM_GapLen; i++)
	{
		if (MatchBuf[i] <= BM100_MATCH_MAX_DIFF)
		{
			MatchNum++;
		}
		else
		{
#ifdef BM_MATCH_DEBUG
			if ((BM_Match_Flg == 1) && (MatchNum >= 3))
			{
				for (i = 0; i < BM_GapLen; i++)
				{
					if (MatchBuf[i] <= BM100_MATCH_MAX_DIFF)
					{
						BM_100_MatchBuf[i] = 1;
					}
					else
					{
						BM_100_MatchBuf[i] = 0;
					}
				}
			}


#endif

			if (MatchNum == 3)
			{
				Match += 3;
			}
			else if (MatchNum == 4)
			{
				Match += 6;
			}
			else if (MatchNum == 5)
			{
				Match += 12;
			}
			else if (MatchNum == 6)
			{
				Match += 24;
			}
			else if (MatchNum > 6)
			{
				Match += 24 + (12 * (MatchNum - 6));
			}
			MatchNum = 0;
		}
	}
#ifdef BM_MATCH_DEBUG
	if ((BM_Match_Flg == 1) && (MatchNum >= 3))
	{
		for (i = 0; i < BM_GapLen; i++)
		{
			if (MatchBuf[i] <= BM100_MATCH_MAX_DIFF)
			{
				BM_100_MatchBuf[i] = 1;
			}
			else
			{
				BM_100_MatchBuf[i] = 0;
			}
		}
	}


#endif
	if (MatchNum == 3)
	{
		Match += 3;
	}
	else if (MatchNum == 4)
	{
		Match += 6;
	}
	else if (MatchNum == 5)
	{
		Match += 12;
	}
	else if (MatchNum == 6)
	{
		Match += 24;
	}
	else if (MatchNum > 6)
	{
		Match += 24 + (12 * (MatchNum - 6));
	}
	BM_Param_p->BM_100_Match = Match;

	return;
}

void BM_Check_50(int *BM_GapBuf, int BM_GapLen, BM_PARAM* BM_Param_p)
{
	int i = 0;
	int MatchNum = 0;
	int ucFeature = BM50_FORM_1;
	int Match = 0;
	int MatchBuf[BM_GAP_NUM_MAX] = { 0 };

	if (BM_GapLen == 0)
	{
		return;
	}
	for (i = 0; i < BM_GapLen; i++)
	{
		if ((BM_GapBuf[i] > BM50_FORM_2_MIN) && (BM_GapBuf[i] < BM50_FORM_2_MAX))
		{
			MatchBuf[i] = GAP_DIFF(BM_GapBuf[i], ucFeature);
		}
		else
		{
			MatchBuf[i] = 127;
		}
		ucFeature = (ucFeature == BM50_FORM_1) ? BM50_FORM_2 : BM50_FORM_1;
	}
	//73 53 73 53 73 53
	for (i = 0; i < BM_GapLen; ++i)
	{
		if (MatchBuf[i] <= BM50_MATCH_MAX_DIFF)
		{
			MatchNum++;
		}
		else
		{
#ifdef BM_MATCH_DEBUG
			if ((BM_Match_Flg == 1) && (MatchNum >= 3))
			{
				int j = 0;
				for (j = 0; j < MatchNum; j++)
				{
					BM_50_MatchBuf[i - j - 1] = 1;
				}
			}
#endif

			if (MatchNum == 3)
			{
				Match += 3;
			}
			else if (MatchNum == 4)
			{
				Match += 6;
			}
			else if (MatchNum == 5)
			{
				Match += 12;
			}
			else if (MatchNum == 6)
			{
				Match += 24;
			}
			else if (MatchNum > 6)
			{
				Match += 24 + (12 * (MatchNum - 6));
			}
			MatchNum = 0;
		}
	}
#ifdef BM_MATCH_DEBUG
	if ((BM_Match_Flg == 1) && (MatchNum >= 3))
	{
		int j = 0;
		for (j = 0; j < MatchNum; j++)
		{
			BM_50_MatchBuf[i - j - 1] = 1;
		}
	}
#endif	
	if (MatchNum == 3)
	{
		Match += 3;
	}
	else if (MatchNum == 4)
	{
		Match += 6;
	}
	else if (MatchNum == 5)
	{
		Match += 12;
	}
	else if (MatchNum == 6)
	{
		Match += 24;
	}
	else if (MatchNum > 6)
	{
		Match += 24 + (12 * (MatchNum - 6));
	}
	MatchNum = 0;
	//53 73 53 73 53 73
	for (i = 0; i < BM_GapLen; ++i)
	{
		if (MatchBuf[i] >= (BM50_FORM_1_2_SUB - BM50_MATCH_MAX_DIFF) && MatchBuf[i] <= (BM50_FORM_1_2_SUB + BM50_MATCH_MAX_DIFF))
		{
			MatchNum++;

		}
		else
		{
#ifdef BM_MATCH_DEBUG
			if ((BM_Match_Flg == 1) && (MatchNum >= 3))
			{
				int j = 0;
				for (j = 0; j < MatchNum; j++)
				{
					BM_50_MatchBuf[i - j - 1] = 1;
				}
			}
#endif
			if (MatchNum == 3)
			{
				Match += 3;
			}
			else if (MatchNum == 4)
			{
				Match += 6;
			}
			else if (MatchNum == 5)
			{
				Match += 12;
			}
			else if (MatchNum == 6)
			{
				Match += 24;
			}
			else if (MatchNum > 6)
			{
				Match += 24 + (12 * (MatchNum - 6));
			}
			MatchNum = 0;
		}
	}
#ifdef BM_MATCH_DEBUG
	if ((BM_Match_Flg == 1) && (MatchNum >= 3))
	{
		int j = 0;
		for (j = 0; j < MatchNum; j++)
		{
			BM_50_MatchBuf[i - j - 1] = 1;
		}
	}
#endif

	if (MatchNum == 3)
	{
		Match += 3;
	}
	else if (MatchNum == 4)
	{
		Match += 6;
	}
	else if (MatchNum == 5)
	{
		Match += 12;
	}
	else if (MatchNum == 6)
	{
		Match += 24;
	}
	else if (MatchNum > 6)
	{
		Match += 24 + (12 * (MatchNum - 6));
	}
	MatchNum = 0;

	BM_Param_p->BM_50_Match = Match;
	return;
}

int BM_Check_20_10_One_Group(int *BM_GapBuf, int BM_GapLen, int i, int Offset_1, int Offset_2)
{
	int WideGap = 0;
	int MidGap = 0;
	int NarrowGap = 0;
	int index_1 = 0;
	int index_2 = 0;
	int GapSub = 0;
	index_1 = i + Offset_1;
	index_2 = i + Offset_2;
	NarrowGap = BM_GapBuf[i];
	if ((index_1 < 0) || (index_1 >(BM_GapLen - 1)) || (index_2 < 0) || (index_2 >(BM_GapLen - 1)))
	{
		return BM_ERROR;
	}

	if (BM_GapBuf[index_1] < BM_1020_WIDE_GAP_MAX && BM_GapBuf[index_2] < BM_1020_WIDE_GAP_MAX)
	{
		if (BM_GapBuf[index_1] > BM_GapBuf[index_2])
		{
			MidGap = BM_GapBuf[index_2];
			WideGap = BM_GapBuf[index_1];
		}
		else
		{
			MidGap = BM_GapBuf[index_1];
			WideGap = BM_GapBuf[index_2];
		}
		if ((MidGap > BM_1020_MID_WIDE_GAP_MIN) && (MidGap < BM_1020_MID_GAP_MAX) && (WideGap > BM_1020_MID_WIDE_GAP_MIN)
			&& (WideGap < BM_1020_WIDE_GAP_MAX))
		{
			GapSub = MidGap + NarrowGap - WideGap;
#if 0
			FILE *myfile1;
			char MatlabChar;

			if (GapSub > 14)
			{
				fopen_s(&myfile1, "C:\\1金标Debug\\GapSubMax14.txt", "ab+");
				MatlabChar = (char)GapSub;
				fwrite(&MatlabChar, sizeof(MatlabChar), 1, myfile1);
				fclose(myfile1);
			}
			else
			{
				fopen_s(&myfile1, "C:\\1金标Debug\\GapSubMin14.txt", "ab+");
				MatlabChar = (char)GapSub;
				fwrite(&MatlabChar, sizeof(MatlabChar), 1, myfile1);
				fclose(myfile1);
			}
#endif
		}
		else
		{
			GapSub = BM_1020_FEATURE_INVALID_VALUE;
		}
		if ((GapSub < BM_20_FEATURE_THRESHOLD_VALUE) && (GapSub > BM_20_GAP_SUB_MIN))//是否为20元
		{
			return BM_20;
		}
		else if (GapSub > BM_10_FEATURE_THRESHOLD_VALUE)//是否为10元
		{
			return BM_10;
		}
	}
	return 0;
}

void BM_Check_20_10(int *BM_GapBuf, int BM_GapLen, BM_PARAM* BM_Param_p)
{
	int i = 0;
	int PreMatchPos = 0xFF;
	int WideGap = 0;
	int MidGap = 0;
	int NarrowGap = 0;
	int Macth_20_Num = 0;
	int Macth_10_Num = 0;
	int MacthResult = 0;
	int ret = 0;
	int MutiNum = 0;
	int Match_10 = 0;
	int Match_20 = 0;

	if (BM_GapLen == 0)
	{
		return;
	}
	for (i = 0; i < BM_GapLen; i++)
	{
		Macth_20_Num = 0;
		Macth_10_Num = 0;
		if ((BM_GapBuf[i] <= BM_1020_NARROW_GAP_MAX) && (BM_GapBuf[i] >= BM_1020_NARROW_GAP_MIN))
		{
			NarrowGap = BM_GapBuf[i];
			ret = BM_Check_20_10_One_Group(BM_GapBuf, BM_GapLen, i, -1, 1);
			if (ret == BM_20)
			{
				Macth_20_Num++;
			}
			else if (ret == BM_10)
			{
				Macth_10_Num++;
			}


			ret = BM_Check_20_10_One_Group(BM_GapBuf, BM_GapLen, i, -1, -2);
			if (ret == BM_20)
			{
				Macth_20_Num++;
			}
			else if (ret == BM_10)
			{
				Macth_10_Num++;
			}


			ret = BM_Check_20_10_One_Group(BM_GapBuf, BM_GapLen, i, 1, 2);
			if (ret == BM_20)
			{
				Macth_20_Num++;
			}
			else if (ret == BM_10)
			{
				Macth_10_Num++;
			}


			if ((Macth_10_Num > 0) && (Macth_20_Num > 0))
			{
				MacthResult = 0;
				PreMatchPos = 0xFFFF;
			}
			else if (Macth_10_Num > Macth_20_Num)
			{
				if (i - PreMatchPos == BM_1020_GAP_PERIOD)
				{
					if (MacthResult == BM_10)
					{
						MutiNum = 3;
					}
					else
					{
						MutiNum = 2;
					}
					if (NarrowGap >= BM_10_MIN_P_THRESHOLD_VALUE)
					{
						Match_10 += 4 * MutiNum;
					}
					else
					{
						Match_10 += 3 * MutiNum;
					}
				}
				else
				{
					if (NarrowGap >= BM_10_MIN_P_THRESHOLD_VALUE)
					{
						Match_10 += 4;
					}
					else
					{
						Match_10 += 3;
					}
				}
				MacthResult = BM_10;
				PreMatchPos = i;
#ifdef BM_MATCH_DEBUG
				if (BM_Match_Flg == 1)
				{
					if (i == 0)
					{
						BM_10_MatchBuf[i] = 1;
						BM_10_MatchBuf[i + 1] = 1;
					}
					else if (i == BM_GapLen - 1)
					{
						BM_10_MatchBuf[i] = 1;
						BM_10_MatchBuf[i - 1] = 1;
					}
					else
					{
						BM_10_MatchBuf[i + 1] = 1;
						BM_10_MatchBuf[i] = 1;
						BM_10_MatchBuf[i - 1] = 1;
					}
				}
#endif
			}
			else if (Macth_20_Num > Macth_10_Num)
			{
				if (i - PreMatchPos == BM_1020_GAP_PERIOD)
				{
					if (MacthResult == BM_20)
					{
						MutiNum = 3;
					}
					else
					{
						MutiNum = 2;
					}
					if (NarrowGap <= BM_20_MIN_P_THRESHOLD_VALUE)
					{
						Match_20 += 4 * MutiNum;
					}
					else
					{
						Match_20 += 3 * MutiNum;
					}
				}
				else
				{
					if (NarrowGap <= BM_20_MIN_P_THRESHOLD_VALUE)
					{
						Match_20 += 4;
					}
					else
					{
						Match_20 += 3;
					}
				}
				MacthResult = BM_20;
				PreMatchPos = i;
#ifdef BM_MATCH_DEBUG
				if (BM_Match_Flg == 1)
				{
					if (i == 0)
					{
						BM_20_MatchBuf[i] = 1;
						BM_20_MatchBuf[i + 1] = 1;
					}
					else if (i == BM_GapLen - 1)
					{
						BM_20_MatchBuf[i] = 1;
						BM_20_MatchBuf[i - 1] = 1;
					}
					else
					{
						BM_20_MatchBuf[i + 1] = 1;
						BM_20_MatchBuf[i] = 1;
						BM_20_MatchBuf[i - 1] = 1;
					}
				}
#endif
			}
			else
			{
				MacthResult = 0;
				PreMatchPos = 0xFFFF;
			}

		}
	}
	BM_Param_p->BM_20_Match = Match_20;
	BM_Param_p->BM_10_Match = Match_10;

	return;
}

unsigned char BM_5_Form_Match(int *BM_GapBuf, int BM_GapLen, BM_PARAM* BM_Param_p, unsigned char i, unsigned char Form_num)
{
	unsigned char j = 0;
	unsigned char BM_5_ForwardForm[BM_5_ZHINENG_FORM_LEN] = { 0 };
	unsigned char BM_5_BackForm[BM_5_ZHINENG_FORM_LEN] = { 0 };
	unsigned char MatchNum = 0;
	unsigned char GapDiff = 0;
	unsigned char CombineDiff = 0;
	unsigned char Len = 0;
	unsigned char CntForward = 0;
	unsigned char CntBack = 0;
	unsigned char Cnt = 0;
	unsigned char ForwardIndex = 0;
	unsigned char BackIndex = 0;
	int Match = 0;
	unsigned char ret = 0;
	unsigned char FormIndex = 0;
	unsigned char GapBufIndex = 0;
	unsigned char ContinueAddMatchFlg = 0;
	ForwardIndex = i;
	BackIndex = i + 1;
	switch (Form_num)
	{
	case 1:
	{
			  BM_5_ForwardForm[0] = BM_5_FORM_6;
			  BM_5_ForwardForm[1] = BM_5_FORM_5;
			  BM_5_ForwardForm[2] = BM_5_FORM_4;
			  BM_5_ForwardForm[3] = BM_5_FORM_3;
			  BM_5_BackForm[0] = BM_5_FORM_3;
			  BM_5_BackForm[1] = BM_5_FORM_4;
			  BM_5_BackForm[2] = BM_5_FORM_5;
			  BM_5_BackForm[3] = BM_5_FORM_6;
			  break;
	}
	case 2:
	{
			  BM_5_ForwardForm[0] = BM_5_FORM_2;
			  BM_5_ForwardForm[1] = BM_5_FORM_1;
			  BM_5_ForwardForm[2] = BM_5_FORM_6;
			  BM_5_ForwardForm[3] = BM_5_FORM_5;
			  BM_5_BackForm[0] = BM_5_FORM_5;
			  BM_5_BackForm[1] = BM_5_FORM_6;
			  BM_5_BackForm[2] = BM_5_FORM_1;
			  BM_5_BackForm[3] = BM_5_FORM_2;
			  break;
	}
	case 3:
	{
			  BM_5_ForwardForm[0] = BM_5_FORM_5;
			  BM_5_ForwardForm[1] = BM_5_FORM_6;
			  BM_5_ForwardForm[2] = BM_5_FORM_1;
			  BM_5_ForwardForm[3] = BM_5_FORM_2;
			  BM_5_BackForm[0] = BM_5_FORM_2;
			  BM_5_BackForm[1] = BM_5_FORM_1;
			  BM_5_BackForm[2] = BM_5_FORM_6;
			  BM_5_BackForm[3] = BM_5_FORM_5;
			  break;
	}
	case 4:
	{
			  BM_5_ForwardForm[0] = BM_5_FORM_3;
			  BM_5_ForwardForm[1] = BM_5_FORM_4;
			  BM_5_ForwardForm[2] = BM_5_FORM_5;
			  BM_5_ForwardForm[3] = BM_5_FORM_6;
			  BM_5_BackForm[0] = BM_5_FORM_6;
			  BM_5_BackForm[1] = BM_5_FORM_5;
			  BM_5_BackForm[2] = BM_5_FORM_4;
			  BM_5_BackForm[3] = BM_5_FORM_3;
			  break;
	}
	}
	//forward
	if ((ForwardIndex - BM_5_ZHINENG_FORM_LEN) < 0)
	{
		Len = ForwardIndex;
	}
	else
	{
		Len = BM_5_ZHINENG_FORM_LEN;
	}
	for (FormIndex = 0, GapBufIndex = 1; GapBufIndex <= Len; FormIndex++, GapBufIndex++)
	{
		GapDiff = GAP_DIFF(BM_5_ForwardForm[FormIndex], BM_GapBuf[ForwardIndex - GapBufIndex]);
		if ((FormIndex + 1) <= (BM_5_ZHINENG_FORM_LEN - 1))
		{
			CombineDiff = GAP_DIFF((BM_5_ForwardForm[FormIndex] + BM_5_ForwardForm[FormIndex + 1]), BM_GapBuf[ForwardIndex - GapBufIndex]);
#ifdef JULONG
			CombineDiff = 255;
#endif

		}
		else
		{
			CombineDiff = 255;
		}
		if (GapDiff <= BM_5_GAP_DIFF)
		{
			CntForward++;
		}
		else if (CombineDiff <= BM_5_GAP_DIFF)
		{
			FormIndex++;
			CntForward++;
		}
		else
		{
			break;
		}
	}
	//back
	if ((BackIndex + BM_5_ZHINENG_FORM_LEN) >= (BM_GapLen - 1))
	{
		Len = (BM_GapLen - BackIndex - 1);
	}
	else
	{
		Len = BM_5_ZHINENG_FORM_LEN;
	}

	for (FormIndex = 0, GapBufIndex = 1; GapBufIndex <= Len; FormIndex++, GapBufIndex++)
	{
		GapDiff = GAP_DIFF(BM_5_BackForm[FormIndex], BM_GapBuf[BackIndex + GapBufIndex]);
		if ((FormIndex + 1) <= (BM_5_ZHINENG_FORM_LEN - 1))
		{
			CombineDiff = GAP_DIFF((BM_5_BackForm[FormIndex] + BM_5_BackForm[FormIndex + 1]), BM_GapBuf[BackIndex + GapBufIndex]);
#ifdef JULONG
			CombineDiff = 255;
#endif
		}
		else
		{
			CombineDiff = 255;
		}
		if (GapDiff <= BM_5_GAP_DIFF)
		{
			MatchNum++;
			CntBack++;
		}
		else if (CombineDiff <= BM_5_GAP_DIFF)
		{
			MatchNum++;
			FormIndex++;
			CntBack++;
		}
		else
		{
			break;
		}
	}

	Cnt = CntBack + CntForward;
	if (Cnt == 3)
	{
		Match += BM_5_STANDARD_MUTI_POINT;
		ContinueAddMatchFlg = 1;
	}
	else if (Cnt == 2)
	{
		Match += BM_5_STANDARD_POINT;
		ContinueAddMatchFlg = 1;
	}
	else if (Cnt >= 4)
	{
		Match += BM_5_STANDARD_POINT + (Cnt - 4)*BM_5_STANDARD_MUTI_POINT;
		ContinueAddMatchFlg = 1;
	}
#ifdef BM_MATCH_DEBUG	
	if ((BM_Match_Flg == 1) && (Cnt >= 3))
	{
		int j = 0;
		BM_5_MatchBuf[i] = 1;
		BM_5_MatchBuf[i + 1] = 1;
		for (j = 0; j < CntBack; j++)
		{
			BM_5_MatchBuf[i + 2 + j] = 1;
		}
		for (j = 0; j < CntForward; j++)
		{
			BM_5_MatchBuf[i - 1 - j] = 1;
		}
	}
#endif


	if (MatchNum >= 3)
	{
		ret = 6;
	}
	BM_Param_p->BM_5_Match += Match;

	if ((ContinueAddMatchFlg == 1) && (ret == 0))
	{
		return (CntBack + 1);
	}
	return ret;
}

void BM_Check_5(int *BM_GapBuf, int BM_GapLen, BM_PARAM* BM_Param_p)
{
	unsigned char i = 0;
	unsigned char Gap = 0;
	unsigned char First = 0;
	unsigned char Second = 0;
	unsigned char Third = 0;
	unsigned char ret = 0;
	int NarrowCnt = 0;

	if (BM_GapLen == 0)
	{
		return;
	}
	if (BM_GapLen >= BM_GAP_MIN)
	{
		for (i = 0; i < BM_GapLen - 1; i++)
		{
			First = GAP_DIFF(BM_GapBuf[i], BM_5_FORM_1);
			if (First <= BM_5_NARROW_DIFF)
			{
				NarrowCnt++;
			}
		}
		if (NarrowCnt < 2)//narraw too less
		{
			return;
		}
	}
	for (i = 0; i < BM_GapLen - 1; i++)
	{

		First = GAP_DIFF(BM_GapBuf[i], BM_5_FORM_1);
		Second = GAP_DIFF(BM_GapBuf[i + 1], BM_5_FORM_2);


		if ((First <= BM_5_NARROW_DIFF) && (Second <= BM_5_GAP_DIFF))
		{
			ret = BM_5_Form_Match(BM_GapBuf, BM_GapLen, BM_Param_p, i, 1);
			if (ret != 0)
			{
				i += ret - 1;
				continue;
			}
			ret = BM_5_Form_Match(BM_GapBuf, BM_GapLen, BM_Param_p, i, 2);
			if (ret != 0)
			{
				i += ret - 1;
				continue;
			}
		}
		First = GAP_DIFF(BM_GapBuf[i], BM_5_FORM_2);
		Second = GAP_DIFF(BM_GapBuf[i + 1], BM_5_FORM_1);
		if ((First <= BM_5_GAP_DIFF) && (Second <= BM_5_NARROW_DIFF))
		{
			ret = BM_5_Form_Match(BM_GapBuf, BM_GapLen, BM_Param_p, i, 3);
			if (ret != 0)
			{
				i += ret - 1;
				continue;
			}
			ret = BM_5_Form_Match(BM_GapBuf, BM_GapLen, BM_Param_p, i, 4);
			if (ret != 0)
			{
				i += ret - 1;
				continue;
			}
		}
	}
	return;
}

void BM_100_15_Looking_For_Forward(int *BM_GapBuf, int BM_GapLen, BM_PARAM* BM_Param_p, unsigned char i_index, unsigned char flag)
{
	unsigned char Index = i_index;
	unsigned char GapDiff = 0;
	unsigned char Multiplying = 0;
	unsigned int match = 0;
	if (Index == 0)
	{
		return;
	}
	while (Index > 0)
	{
		Index--;
		GapDiff = GAP_DIFF(BM_GapBuf[Index], BM_100_15VER_WIDE_PEAK);
		if (GapDiff <= BM_100_15VER_WIDE_DIFF)
		{
#ifdef BM_MATCH_DEBUG	
			if (BM_Match_Flg == 1)
			{
				BM_100_V15_MatchBuf[Index] = 1;
			}
#endif
			if (Multiplying < 2)
			{
				Multiplying++;
			}
			match += BM_100_15VER_STANDARD_MUTI_POINT*Multiplying;
		}
		else if ((Index - 1) >= 0)
		{
			GapDiff = GAP_DIFF(BM_GapBuf[Index] + BM_GapBuf[Index - 1], BM_100_15VER_WIDE_PEAK);
			if (GapDiff <= BM_100_15VER_WIDE_DIFF)
			{
				match += BM_100_15VER_STANDARD_POINT;
				Index--;
#ifdef BM_MATCH_DEBUG	
				if (BM_Match_Flg == 1)
				{
					BM_100_V15_MatchBuf[Index] = 1;
				}
#endif
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
	if (flag == BM_100_15VER_WIDE_MATCH)
	{
		BM_Param_p->BM_100_v15_Wide_Match += match;
	}
	else if (flag == BM_100_15VER_MATCH)
	{
		BM_Param_p->BM_100_v15_Match += match;
	}
}

unsigned char BM_100_15_Looking_For_Back(int *BM_GapBuf, int BM_GapLen, BM_PARAM* BM_Param_p, unsigned char i_index, unsigned char flag)
{
	unsigned char Index = i_index;
	unsigned char I_add_num = 0;
	unsigned char GapDiff = 0;
	unsigned char Multiplying = 0;
	unsigned int match = 0;
	if (Index == 0)
	{
		return 0;
	}
	while (Index < (BM_GapLen - 1))
	{
		Index++;
		I_add_num++;
		GapDiff = GAP_DIFF(BM_GapBuf[Index], BM_100_15VER_WIDE_PEAK);
		if (GapDiff <= BM_100_15VER_WIDE_DIFF)
		{
#ifdef BM_MATCH_DEBUG	
			if (BM_Match_Flg == 1)
			{
				BM_100_V15_MatchBuf[Index] = 1;
			}
#endif
			if (Multiplying < 2)
			{
				Multiplying++;
			}
			match += BM_100_15VER_STANDARD_MUTI_POINT*Multiplying;
		}
		else if ((Index + 1) < BM_GapLen)
		{
			GapDiff = GAP_DIFF(BM_GapBuf[Index] + BM_GapBuf[Index + 1], BM_100_15VER_WIDE_PEAK);
			if (GapDiff <= BM_100_15VER_WIDE_DIFF)
			{
				match += BM_100_15VER_STANDARD_POINT;
				Index++;
				I_add_num++;
#ifdef BM_MATCH_DEBUG	
				if (BM_Match_Flg == 1)
				{
					BM_100_V15_MatchBuf[Index] = 1;
				}
#endif
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
	if (flag == BM_100_15VER_WIDE_MATCH)
	{
		BM_Param_p->BM_100_v15_Wide_Match += match;
	}
	else if (flag == BM_100_15VER_MATCH)
	{
		BM_Param_p->BM_100_v15_Match += match;
	}
	return I_add_num;
}

void BM_Check_100_15(int *BM_GapBuf, int BM_GapLen, BM_PARAM* BM_Param_p)
{
	unsigned char i = 0;
	unsigned char First = 0;
	unsigned char Second = 0;
	unsigned char ret = 0;

	if (BM_GapLen == 0)
	{
		return;
	}
	for (i = 0; i < BM_GapLen - 1; i++)
	{
		//find double narrow peak
		First = GAP_DIFF(BM_GapBuf[i], BM_100_15VER_NARROW_PEAK);
		Second = GAP_DIFF(BM_GapBuf[i + 1], BM_100_15VER_NARROW_PEAK);
		//TRACE("i = %d First:%d Second:%d\n",i,First,Second);
		if ((First <= BM_100_15VER_NARROW_DIFF) && (Second <= BM_100_15VER_NARROW_DIFF))
		{
#ifdef BM_MATCH_DEBUG
			if (BM_Match_Flg == 1)
			{
				BM_100_V15_MatchBuf[i] = 1;
				BM_100_V15_MatchBuf[i + 1] = 1;
			}
#endif
			BM_Param_p->BM_100_v15_Match += BM_100_15VER_STANDARD_POINT;
			//find  continuous wide peak
			//Looking for forward gap
			BM_100_15_Looking_For_Forward(BM_GapBuf, BM_GapLen, BM_Param_p, i, BM_100_15VER_MATCH);
			//Looking for back gap
			BM_100_15_Looking_For_Back(BM_GapBuf, BM_GapLen, BM_Param_p, i + 1, BM_100_15VER_MATCH);
		}
	}
	//find wide peak
	for (i = 0; i < BM_GapLen - 1; i++)
	{
		//find double wide peak
		First = GAP_DIFF(BM_GapBuf[i], BM_100_15VER_WIDE_PEAK);
		Second = GAP_DIFF(BM_GapBuf[i + 1], BM_100_15VER_WIDE_PEAK);
		//TRACE("i = %d First:%d Second:%d\n",i,First,Second);
		if ((First <= BM_100_15VER_WIDE_DIFF) && (Second <= BM_100_15VER_WIDE_DIFF))
		{
#ifdef BM_MATCH_DEBUG	
			if (BM_Match_Flg == 1)
			{
				BM_100_V15_MatchBuf[i] = 1;
				BM_100_V15_MatchBuf[i + 1] = 1;
			}

#endif
			BM_Param_p->BM_100_v15_Wide_Match += BM_100_15VER_STANDARD_POINT;
			//find  continuous wide peak
			//Looking for forward gap
			BM_100_15_Looking_For_Forward(BM_GapBuf, BM_GapLen, BM_Param_p, i, BM_100_15VER_WIDE_MATCH);
			//Looking for back gap
			ret = BM_100_15_Looking_For_Back(BM_GapBuf, BM_GapLen, BM_Param_p, i + 1, BM_100_15VER_WIDE_MATCH);
			i += ret;
		}
	}
}

void BM_Check_20_99(int *BM_GapBuf, int BM_GapLen, BM_PARAM* BM_Param_p)
{
	unsigned char i = 0;

	if (BM_GapLen < 4)
	{
		return;
	}
	for (i = 0; i < BM_GapLen - 4; ++i)
	{
		if ((BM_GapBuf[i] > BM_20_v99_FORM_1_MIN) && (BM_GapBuf[i] < BM_20_v99_FORM_1_MAX)
			&& (BM_GapBuf[i + 1] > BM_20_v99_FORM_2_MIN) && (BM_GapBuf[i + 1] < BM_20_v99_FORM_2_MAX)
			&& (BM_GapBuf[i + 2] > BM_20_v99_FORM_3_MIN) && (BM_GapBuf[i + 2] < BM_20_v99_FORM_3_MAX)
			&& (BM_GapBuf[i + 3] > BM_20_v99_FORM_2_MIN) && (BM_GapBuf[i + 3] < BM_20_v99_FORM_2_MAX))
		{
			BM_Param_p->BM_20_v99_Match = BM_20;
		}
	}
	for (i = 0; i < BM_GapLen - 4; ++i)
	{
		if ((BM_GapBuf[i] > BM_20_v99_FORM_3_MIN) && (BM_GapBuf[i] < BM_20_v99_FORM_3_MAX)
			&& (BM_GapBuf[i + 1] > BM_20_v99_FORM_2_MIN) && (BM_GapBuf[i + 1] < BM_20_v99_FORM_2_MAX)
			&& (BM_GapBuf[i + 2] > BM_20_v99_FORM_1_MIN) && (BM_GapBuf[i + 2] < BM_20_v99_FORM_1_MAX)
			&& (BM_GapBuf[i + 3] > BM_20_v99_FORM_2_MIN) && (BM_GapBuf[i + 3] < BM_20_v99_FORM_2_MAX))
		{
			BM_Param_p->BM_20_v99_Match = BM_20;
		}
	}
	return;
}

int PrecisionWheelManageOne(OneDSignal* BMHD_Signal_p, int Index)
{
	int i = 0;
	int Wheel = 0;
	int FirstWheel = 0;
	int TotelCnt = 0;
	int PreCnt = 0;
	int PrecisionWheel = 0;

	FirstWheel = CashFirstWheel;

	TotelCnt = 1;

	Wheel = BMHD_Signal_p->Wheel[Index];

	for (i = Index + 1; i < BMHD_Signal_p->Length; i++)
	{
		if (BMHD_Signal_p->Wheel[i] != Wheel)
		{
			break;
		}
		TotelCnt++;
	}
	for (i = Index - 1; i >= 0; i--)
	{
		if (BMHD_Signal_p->Wheel[i] != Wheel)
		{
			break;
		}
		PreCnt++;
		TotelCnt++;
	}
	if (PreCnt == 0)
	{
		PrecisionWheel = (Wheel - FirstWheel)*PEAK_PRECISION_WHEEL;
	}
	else
	{
		PrecisionWheel = (Wheel - FirstWheel)*PEAK_PRECISION_WHEEL + PreCnt*PEAK_PRECISION_WHEEL / TotelCnt;
	}
	return PrecisionWheel;
}

int PrecisionWheelManage(OneDSignal* BMHD_Signal_p, int Begin, int End)
{

	int PrecisionWheelBegin = 0;
	int PrecisionWheelEnd = 0;
	int PeakPrecisionWheel = 0;


	PrecisionWheelBegin = PrecisionWheelManageOne(BMHD_Signal_p, Begin);
	PrecisionWheelEnd = PrecisionWheelManageOne(BMHD_Signal_p, End);
	PeakPrecisionWheel = (PrecisionWheelBegin + PrecisionWheelEnd) / 2;
#ifdef HD_DEBUG
	BM_PeakMidIndexBuf[BM_PeakMidIndexLen] = (Begin + End) / 2;
	BM_PeakMidIndexLen++;
#endif
	return PeakPrecisionWheel;
}

void BM_FindPeak(OneDSignal* BMHD_Signal_p, int *WheelBuf, int *WheelLen, int Threshold)
{
	int i = 0;
	int j = 0;
	int Jump = 0;
	int PeakPosBuf[BM_GAP_NUM_MAX * 2] = { 0 };
	int PeakPosLen = 0;
	int RiseFallFlg = 0;
	int WheelBufLen = 0;

	EDGE_DETECTION_VOID(BMHD_Signal_p->Length, 1);
	for (i = 0; i < BMHD_Signal_p->Length; i++)
	{
		if (RiseFallFlg == 0)
		{		//寻找上升沿码盘
			if ((BMHD_Signal_p->Vol[i] < Threshold) && (BMHD_Signal_p->Vol[i + 1] >= Threshold))
			{
				if (ABS(i, PeakPosBuf[PeakPosLen - 1]) <= BM_INVALID_PEAK_MIN)
				{
					PeakPosBuf[PeakPosLen - 1] = 0;
					PeakPosLen--;
					RiseFallFlg = 1;
					continue;
				}
				PeakPosBuf[PeakPosLen] = i;
				PeakPosLen++;
				RiseFallFlg = 1;
			}
		}

		if (RiseFallFlg == 1)
		{		//寻找下降沿码盘
			if ((BMHD_Signal_p->Vol[i] >= Threshold) && (BMHD_Signal_p->Vol[i + 1] < Threshold))
			{
				PeakPosBuf[PeakPosLen] = i + 1;
				PeakPosLen++;
				RiseFallFlg = 0;
			}

		}

		if (PeakPosLen >= BM_GAP_NUM_MAX * 2)
		{
			break;
		}
	}

	EDGE_DETECTION_VOID(PeakPosLen, 1);
	for (i = 0; i < PeakPosLen; i += 2)
	{
		Jump = 0;
		for (j = PeakPosBuf[i]; j <= PeakPosBuf[i + 1]; j++)
		{
			if (BMHD_Signal_p->Vol[j] > Threshold)
			{
				Jump++;
			}
		}
		if (Jump > BM_PEAK_MIN_POINT)
		{
			WheelBuf[WheelBufLen] = PrecisionWheelManage(BMHD_Signal_p, PeakPosBuf[i], PeakPosBuf[i + 1]);											//取峰值中间点
			WheelBufLen++;
		}

		if (WheelBufLen >= BM_GAP_NUM_MAX)
		{
			break;
		}
	}
	*WheelLen = WheelBufLen;
	return;
}
//算出峰间距
void PeakGapManage(int *PeakGapBuf, int *PeakGapLen, int *PeakWheelBuf, int PeakWheelLen)
{
	int i = 0;
	int Cnt = 0;
	int WheelBuf[BM_GAP_NUM_MAX] = { 0 };

	EDGE_DETECTION_VOID(PeakWheelLen, 1);
	for (i = 0; i < PeakWheelLen - 1; i++)
	{
		PeakGapBuf[Cnt] = PeakWheelBuf[i + 1] - PeakWheelBuf[i];
		Cnt++;
	}
	*PeakGapLen = Cnt;
	return;
}

void BM_GapCheck(int *BM_GapBuf, int BM_GapLen, BM_PARAM* BM_Param_p)
{
	int i = 0;
	int Begin = 0;
	int End = 0;
	EDGE_DETECTION_VOID(BM_GapLen, 1);
	if (SensitiveParam.Err_353 >= 7)
	{
		Begin = 0;
		End = BM_GapLen;
	}
	else
	{
		Begin = 1;
		End = BM_GapLen - 1;
	}
	for (i = Begin; i < End; i++)
	{
		if ((BM_GapBuf[i] > BM_100_BIG_GAP_MAX))
		{
			BM_Param_p->BM_BigGapNum.BM_100_BigGapNum++;
		}
		if ((BM_GapBuf[i] > BM_100_15_BIG_GAP_MAX))
		{
			BM_Param_p->BM_BigGapNum.BM_100_15_BigGapNum++;
		}
		if ((BM_GapBuf[i] > BM_50_BIG_GAP_MAX))
		{
			BM_Param_p->BM_BigGapNum.BM_50_BigGapNum++;
		}
		if (BM_GapBuf[i] > BM_10_20_BIG_GAP_MAX)
		{
			BM_Param_p->BM_BigGapNum.BM_10_BigGapNum++;
		}
		if (BM_GapBuf[i] > BM_10_20_BIG_GAP)
		{
			BM_Param_p->BM_BigGapNum.BM_10_MidGapNum++;
		}
		if (BM_GapBuf[i] > BM_5_BIG_GAP_MAX)
		{
			BM_Param_p->BM_BigGapNum.BM_5_BigGapNum++;
		}
		if (BM_GapBuf[i] > BM_5_BIG_GAP)
		{
			BM_Param_p->BM_BigGapNum.BM_5_MidGapNum++;
		}
	}
	return;
}

int BM_check(OneDSignal * pOneDSignal)
{

	memset(BM_GapMidBuf, 0, sizeof(BM_GapMidBuf));
	BM_GapMidLen = 0;

	memset(BM_GapHighBuf, 0, sizeof(BM_GapHighBuf));
	BM_GapHighLen = 0;

	memset(BM_PeakMidWheelBuf, 0, sizeof(BM_PeakMidWheelBuf));
	BM_PeakMidWheelLen = 0;

	memset(BM_PeakHighWheelBuf, 0, sizeof(BM_PeakHighWheelBuf));
	BM_PeakHighWheelLen = 0;

#ifdef BM_MATCH_DEBUG	
	memset(BM_100_V15_MatchBuf, 0, sizeof(BM_100_V15_MatchBuf));
	memset(BM_100_MatchBuf, 0, sizeof(BM_100_MatchBuf));
	memset(BM_50_MatchBuf, 0, sizeof(BM_50_MatchBuf));
	memset(BM_20_MatchBuf, 0, sizeof(BM_20_MatchBuf));
	memset(BM_10_MatchBuf, 0, sizeof(BM_10_MatchBuf));
	memset(BM_5_MatchBuf, 0, sizeof(BM_5_MatchBuf));
	BM_MatchBufLen = 0;
	BM_Match_Flg = 1;
#endif

	//mid
	BM_FindPeak(pOneDSignal, BM_PeakMidWheelBuf, &BM_PeakMidWheelLen, BM_MID_VOL);

	PeakGapManage(BM_GapMidBuf, &BM_GapMidLen, BM_PeakMidWheelBuf, BM_PeakMidWheelLen);

	BM_GapCheck(BM_GapMidBuf, BM_GapMidLen, &AraitratParam.BM_Mid_Param);

	BM_Check_100(BM_GapMidBuf, BM_GapMidLen, &AraitratParam.BM_Mid_Param);

	BM_Check_100_15(BM_GapMidBuf, BM_GapMidLen, &AraitratParam.BM_Mid_Param);

	BM_Check_50(BM_GapMidBuf, BM_GapMidLen, &AraitratParam.BM_Mid_Param);

	BM_Check_20_10(BM_GapMidBuf, BM_GapMidLen, &AraitratParam.BM_Mid_Param);

	BM_Check_5(BM_GapMidBuf, BM_GapMidLen, &AraitratParam.BM_Mid_Param);

	BM_Check_20_99(BM_GapMidBuf, BM_GapMidLen, &AraitratParam.BM_Mid_Param);

	//IntermediateResult.BM_LevelMid.BM_GAP = BM_GapMidLen;

	AraitratParam.BM_Mid_Param.BM_GapNum = BM_GapMidLen;

#ifdef BM_MATCH_DEBUG	
	BM_Match_Flg = 0;
#endif	

	//high
#ifdef HD_DEBUG
	memset(BM_PeakMidIndexBuf, 0, sizeof(BM_PeakMidIndexBuf));
	BM_PeakMidIndexLen = 0;
#endif

	BM_FindPeak(pOneDSignal, BM_PeakHighWheelBuf, &BM_PeakHighWheelLen, BM_HIGH_VOL);

	PeakGapManage(BM_GapHighBuf, &BM_GapHighLen, BM_PeakHighWheelBuf, BM_PeakHighWheelLen);

	BM_GapCheck(BM_GapHighBuf, BM_GapHighLen, &AraitratParam.BM_High_Param);

	BM_Check_100(BM_GapHighBuf, BM_GapHighLen, &AraitratParam.BM_High_Param);

	BM_Check_100_15(BM_GapHighBuf, BM_GapHighLen, &AraitratParam.BM_High_Param);

	BM_Check_50(BM_GapHighBuf, BM_GapHighLen, &AraitratParam.BM_High_Param);

	BM_Check_20_10(BM_GapHighBuf,BM_GapHighLen,&AraitratParam.BM_High_Param);

	BM_Check_5(BM_GapHighBuf, BM_GapHighLen, &AraitratParam.BM_High_Param);

	BM_Check_20_99(BM_GapHighBuf, BM_GapHighLen, &AraitratParam.BM_High_Param);

	//IntermediateResult.BM_LevelHigh.BM_GAP = BM_GapHighLen;

	AraitratParam.BM_High_Param.BM_GapNum = BM_GapHighLen;



#ifdef VC_PRINT_DEBUG
	FILE *myfile1;
	char MatlabChar;

	/*fopen_s(&myfile1, "C:\\1金标Debug\\BM_info.txt", "a+");
	fprintf(myfile1, "MID:\tBM数量=%d\t\t BM间距数量=%d\t\t HIGH:\tBM数量=%d\t\t BM间距数量=%d\n", BM_PeakMidWheelLen, BM_GapMidLen, BM_PeakHighWheelLen, BM_GapHighLen);
	fclose(myfile1);*/

	/*----------统计MidHighGap(直观观察)--------*/
	/*myfile1 = fopen("C:\\1金标Debug\\MidHighGap.txt", "a+");
	fprintf(myfile1, "MidGap:\t\t");
	for (int i = 0; i < 25; i++)
	{
	fprintf(myfile1, "%d\t", BM_GapMidBuf[i]);
	}
	fprintf(myfile1, "\n");

	fprintf(myfile1, "HighGap:\t\t");
	for (int i = 0; i < 25; i++)
	{
	fprintf(myfile1, "%d\t", BM_GapHighBuf[i]);
	}
	fprintf(myfile1, "\n\n");
	fclose(myfile1);*/

	/*----------统计个面额得分(Matlab用)--------*/
	//if (AraitratParam.BM_Mid_Param.BM_100_Match /*|| AraitratParam.BM_High_Param.BM_100_Match*/)
	//{
	fopen_s(&myfile1, "C:\\1金标Debug\\100_05_Score.txt", "ab+");
	MatlabChar = (unsigned char)AraitratParam.BM_Mid_Param.BM_100_Match;
	fwrite(&MatlabChar, sizeof(MatlabChar), 1, myfile1);
	fclose(myfile1);
	//}
	//if (AraitratParam.BM_Mid_Param.BM_50_Match/* || AraitratParam.BM_High_Param.BM_50_Match*/)
	//{
	fopen_s(&myfile1, "C:\\1金标Debug\\50_Score.txt", "ab+");
	MatlabChar = (unsigned char)AraitratParam.BM_Mid_Param.BM_50_Match;
	fwrite(&MatlabChar, sizeof(MatlabChar), 1, myfile1);
	fclose(myfile1);
	//}
	//if (AraitratParam.BM_Mid_Param.BM_100_v15_Match /*|| AraitratParam.BM_High_Param.BM_100_v15_Match*/)
	//{
	fopen_s(&myfile1, "C:\\1金标Debug\\100_15_Score.txt", "ab+");
	MatlabChar = (unsigned char)AraitratParam.BM_Mid_Param.BM_100_v15_Match;
	fwrite(&MatlabChar, sizeof(MatlabChar), 1, myfile1);
	fclose(myfile1);
	//}
	//if (AraitratParam.BM_Mid_Param.BM_10_Match /*|| AraitratParam.BM_High_Param.BM_10_Match */)
	//{
	fopen_s(&myfile1, "C:\\1金标Debug\\10_Score.txt", "ab+");
	MatlabChar = (unsigned char)AraitratParam.BM_Mid_Param.BM_10_Match;
	fwrite(&MatlabChar, sizeof(MatlabChar), 1, myfile1);
	fclose(myfile1);
	//}
	//if (AraitratParam.BM_Mid_Param.BM_20_Match /*|| AraitratParam.BM_High_Param.BM_20_Match*/)
	//{
	fopen_s(&myfile1, "C:\\1金标Debug\\20_Score.txt", "ab+");
	MatlabChar = (unsigned char)AraitratParam.BM_Mid_Param.BM_20_Match;
	fwrite(&MatlabChar, sizeof(MatlabChar), 1, myfile1);
	fclose(myfile1);
	//}
	//if (AraitratParam.BM_Mid_Param.BM_5_Match /*|| AraitratParam.BM_High_Param.BM_5_Match*/)
	//{
	fopen_s(&myfile1, "C:\\1金标Debug\\5_Score.txt", "ab+");
	MatlabChar = (unsigned char)AraitratParam.BM_Mid_Param.BM_5_Match;
	fwrite(&MatlabChar, sizeof(MatlabChar), 1, myfile1);
	fclose(myfile1);
	//}

#endif

	return 0;
}