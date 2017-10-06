#include "ir_lib.h"


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

#define CASH_FIRST_WHEEL_SUB	10
void CashFirstWheelManage(TdnData_Alg* AlgData_p, WheelData *CashFirstWheel)
{
	WheelData FirstWheel = 0;
	FirstWheel = AlgData_p->BM.Wheel[0];
	if (FirstWheel > CASH_FIRST_WHEEL_SUB)
	{
		FirstWheel -= CASH_FIRST_WHEEL_SUB;
	}
	else
	{
		FirstWheel = 0;
	}
	*CashFirstWheel = FirstWheel;
}

/*————————————红外角度算法————————————*/
int GetAngle(TdnData_Alg* AlgData_p)
{
#ifdef IR_ANGLE_LIB
	float slope1 = 0.0f;
	float slope2 = 0.0f;
	float angle1 = 0.0f;
	float angle2 = 0.0f;
	float Distance_X = DISTANCE_MM_IR_X;
	float Distance_Y = (float)(AlgData_p->IRData[IRRC].Wheel[0] - AlgData_p->IRData[IRLC].Wheel[0]);
	float Distance_Y1 = (float)(AlgData_p->IRData[IRRC].Wheel[AlgData_p->IRData[IRRC].Length - 1]
		- AlgData_p->IRData[IRLC].Wheel[AlgData_p->IRData[IRLC].Length - 1]);

	slope1 = Distance_Y / Distance_X;
	slope2 = Distance_Y1 / Distance_X;
	angle1 = fabs(atan(slope1) * 180 / 3.14);     ///开始边沿原结果
	angle2 = fabs(atan(slope2) * 180 / 3.14);      ///结束边沿原结果
	AraitratParam.angle = angle1 > angle2 ? angle1 : angle2;
	AraitratParam.slope = angle1 > angle2 ? slope1 : slope2;
#endif
	return 0;
}


/*————————————红外宽度算法————————————*/
int GetWidth(TdnData_Alg * AlgData_p)
{
#ifdef IR_WIDTH_LIB
	int i = 0;
	for (i = 0; i < IR_NUM; i++)
	{
		if (AlgData_p->IRData[i].Length > 0)
		{
			AraitratParam.Width_IR[i] = (ABS(AlgData_p->IRData[i].Wheel[AlgData_p->IRData[i].Length - 1], AlgData_p->IRData[i].Wheel[0])*cos(atan(AraitratParam.slope)));
		}
		else
		{
			AraitratParam.Width_IR[i] = 0;  ////DATA err
		}
	}
	AraitratParam.Width = AraitratParam.Width_IR[IRLC] > AraitratParam.Width_IR[IRRC] ? AraitratParam.Width_IR[IRLC] : AraitratParam.Width_IR[IRRC];
#endif
	return 0;
}

/*————————————计算有效红外平均电压————————————*/
int GetBackGroundVol(TdnData_Alg * AlgData_p, int Lasterror)
{
#ifdef IR_BACKGROUND_LIB
	int BackGroundVol = 0;
	int MaxVol = 0;
	int i = 0;
	int j = 0;
	int len = 0;

	for (i = 0; i < IR_NUM; i++)
	{
		/*if (IrInvalidBuf[i] == 1)
		{
			continue;
		}*/
		if (AlgData_p->IRData[i].Length <= 1)
		{
			continue;
		}
		if (AraitratParam.Width_IR[i] <= MIN_1_5_YUAN_WIDTH)
		{
			continue;
		}
		if (LastErrorBuf[i] == 0)
		{
			BackGroundVol = 0;
			for (j = 0; j < AlgData_p->IRData[i].Length - 1; j++)
			{
				BackGroundVol += AlgData_p->IRData[i].Vol[j];
			}
			AraitratParam.ChannalBackGroundVol[i] = BackGroundVol / AlgData_p->IRData[i].Length;
			//IntermediateResult.IR_BackGroud[i] = AraitratParam.ChannalBackGroundVol[i];
			BackGroundVol = 0;
			len = AlgData_p->IRData[i].Length * 7 / 8;
			if (len <= 1)
			{
				continue;
			}
			for (j = AlgData_p->IRData[i].Length / 8; j < len; j++)
			{
				BackGroundVol += AlgData_p->IRData[i].Vol[j];
			}
			AraitratParam.CenterVol[i] = BackGroundVol / (len - 1);
		}
	}

	return 0;
#endif
}

/*———————————红外IR重张算法—————————————*/
int Overlap_check(TdnData_Alg * AlgData_p, Sensitive *Sensitive_p, int CalibrateVol, int Sensi_Offset_Vol)
{
	int i = 0;
	int j = 0;
	int Thred_Offset = 0;
	int DubiousVolNum = 0;			//可疑电压的个数
	int TotalDubiousVol = 0;		//可疑的电压总量
	int AveDubiousVol = 0;			//可疑电压的平均值
	int Calibrate_Vol = 0;			//校准电压，缺省为700
	if (Sensitive_p->Err_256 == 0)
	{
		return 0;
	}
	if (Sensitive_p->Err_256 < 1)
	{
		Sensitive_p->Err_256 = 1;
	}
	if (Sensitive_p->Err_256 > 9)
	{
		Sensitive_p->Err_256 = 9;
	}
	if (Sensitive_p->Err_256)
	{
		Thred_Offset = (Sensitive_p->Err_256 - 5) * Sensi_Offset_Vol;
	}

	for (i = 0; i < IR_NUM; i++)
	{
		if (AraitratParam.CenterVol[i])
		{
			Calibrate_Vol = CalibrateVol + Thred_Offset;
		}
		else
		{
			Calibrate_Vol = CalibrateVol;
		}

		for (j = 0; j < AlgData_p->IRData[i].Length; j++)
		{
			if (AlgData_p->IRData[i].Vol[j] < Calibrate_Vol && AraitratParam.CenterVol[i])
			{
				TotalDubiousVol += AlgData_p->IRData[i].Vol[j];
				DubiousVolNum++;
			}
		}
		if (DubiousVolNum)
		{
			AveDubiousVol = TotalDubiousVol / DubiousVolNum;
		}

		AraitratParam.OverlapVol_Info[i].DubiousVolNum = DubiousVolNum;
		AraitratParam.OverlapVol_Info[i].AveDubiousVol = AveDubiousVol;
#if 0
		if (DubiousVolNum)
		{
			FILE *myfile1;
			unsigned short int MatlabChar;

			myfile1 = fopen("C:\\123456\\VolNum.txt", "ab+");
			MatlabChar = (short int)DubiousVolNum;
			fwrite(&MatlabChar, sizeof(MatlabChar), 1, myfile1);
			fclose(myfile1);
		}
#endif
	}

	return 0;
}
