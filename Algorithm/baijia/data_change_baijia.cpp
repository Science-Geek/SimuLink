#include "data_change_baijia.h"

#ifdef BAIJIA

void UV_DataChange(OneDSignal *AlgSignal_p, TdnData_Alg *AlgData_p)
{
	AlgData_p->UVMidData[UV_MID_DIR1] = AlgSignal_p[TDN_MUV_U_1];
	AlgData_p->UVMidData[UV_MID_DIR2] = AlgSignal_p[TDN_MUV_U_2];
	AlgData_p->UVMidData[UV_MID_DIR3] = AlgSignal_p[TDN_MUV_D_2];
	AlgData_p->UVMidData[UV_MID_DIR4] = AlgSignal_p[TDN_MUV_D_1];
	AlgData_p->UVSideData[UV_SIDE_DIR12] = AlgSignal_p[TDN_SUV_U];
	AlgData_p->UVSideData[UV_SIDE_DIR34] = AlgSignal_p[TDN_SUV_D];
	return;
}

void IRdataInit(OneDSignal *AlgSignal_p, OneDSignal* IRdata)
{

	memcpy(&IRdata[IRLC], &AlgSignal_p[TDN_IRLC], sizeof(OneDSignal));
	memcpy(&IRdata[IRRC], &AlgSignal_p[TDN_IRRC], sizeof(OneDSignal));
	memcpy(&IRdata[IRL1], &AlgSignal_p[TDN_IRL1], sizeof(OneDSignal));
	memcpy(&IRdata[IRR1], &AlgSignal_p[TDN_IRR1], sizeof(OneDSignal));

}

void MMSM_DataChangeOne_TMR(OneDSignal *TdnData_p, OneDSignal *MMSignal_p, int PrepValidStartWheel, int PrepValidEndwheel, float Slope, int Cnt, int flag, int MAG_IR_Offset_X, int MAG_IR_OFFset_Y, int Wheel_Err)
{
	int i = 0;
	int StartIndex = 0;
	int EndIndex = 0;
	int StartWheel = 0;
	int StopWheel = 0;
	if (flag == 1)
	{
		StartWheel = PrepValidStartWheel + DISTANCE_MM_Y;
		StopWheel = PrepValidEndwheel + DISTANCE_MM_Y;
	}
	else
	{
		StartWheel = PrepValidStartWheel + (MAG_IR_Offset_X + (Gap_MAG * Cnt)) * Slope;
		StopWheel = PrepValidEndwheel + (MAG_IR_Offset_X + (Gap_MAG * Cnt)) * Slope;
	}
	//after brake ,wheel may be jump ,then,we may can't find the wheel,so,add five wheel 
	StartWheel += MAG_IR_OFFset_Y - Wheel_Err;
	StopWheel += MAG_IR_OFFset_Y + Wheel_Err;

	for (i = 0; i < TdnData_p->Length; i++)
	{
		if (TdnData_p->Wheel[i] > StartWheel)
		{
			if ((TdnData_p->Wheel[i] - StartWheel) <= MMSM_ERR_WHEEL)
			{
				StartIndex = i;
				break;
			}
		}

		if (TdnData_p->Wheel[i] == StartWheel)
		{
			StartIndex = i;
			break;
		}
	}
	for (i = (TdnData_p->Length - 1); i >= 0; i--)
	{
		if (TdnData_p->Wheel[i] < StopWheel)
		{
			if ((StopWheel - TdnData_p->Wheel[i]) <= MMSM_ERR_WHEEL)
			{
				EndIndex = i;
				break;
			}
		}

		if (TdnData_p->Wheel[i] == StopWheel)
		{
			EndIndex = i;
			break;
		}
	}
	if (StartIndex == 0 && EndIndex == 0)
	{
		MMSignal_p->Length = TdnData_p->Length;
		MMSignal_p->Vol = TdnData_p->Vol;
		MMSignal_p->Wheel = TdnData_p->Wheel;
		MMSignal_p->Time = TdnData_p->Time;
	}
	else if (StartIndex == 0 || EndIndex == 0)
	{
		if (StartIndex == 0)
		{
			MMSignal_p->Length = EndIndex + 1;
			MMSignal_p->Vol = TdnData_p->Vol;
			MMSignal_p->Wheel = TdnData_p->Wheel;
			MMSignal_p->Time = TdnData_p->Time;
		}
		else if (EndIndex == 0)
		{
			MMSignal_p->Length = TdnData_p->Length - (StartIndex + 1);
			if (MMSignal_p->Length < 1)
			{
				MMSignal_p->Length = 1;
			}
			MMSignal_p->Vol = &TdnData_p->Vol[StartIndex];
			MMSignal_p->Wheel = &TdnData_p->Wheel[StartIndex];
			MMSignal_p->Time = &TdnData_p->Time[StartIndex];
		}
		else
		{
			MMSignal_p->Length = TdnData_p->Length;
			MMSignal_p->Vol = TdnData_p->Vol;
			MMSignal_p->Wheel = TdnData_p->Wheel;
			MMSignal_p->Time = TdnData_p->Time;
		}
	}
	else
	{
		MMSignal_p->Length = EndIndex - StartIndex;
		if (MMSignal_p->Length < 1)
		{
			MMSignal_p->Length = 1;
		}
		MMSignal_p->Vol = &TdnData_p->Vol[StartIndex];
		MMSignal_p->Wheel = &TdnData_p->Wheel[StartIndex];
		MMSignal_p->Time = &TdnData_p->Time[StartIndex];

	}
#ifdef MMSM_DEBUG 
	MMSignal_p->Time[0] = StartIndex;
#endif
}

void MMSM_DataChange_JULONG_TMR(OneDSignal *TdnData_p, TdnData_Alg *AlgData_p)
{
	int i = 0;
	int MM_Flag = 0;
	int MM_PrepStartWheel = 0;
	int MM_PrepEndwheel = 0;
	int LRM_PrepStartWheel = 0;
	int LRM_PrepEndwheel = 0;
	float Distance_X = 0.0f;
	float Distance_Y = 0.0f;
	float Slope_MM = 0.0f;
	int widthLeft = 0;
	int widthRight = 0;

	/*——————— 中磁和多段磁根据计数红外对管LCRC进行初定位———————*/
	MM_PrepStartWheel = (AlgData_p->IRData[IRRC].Wheel[0] + AlgData_p->IRData[IRLC].Wheel[0]) / 2;
	MM_PrepEndwheel = (AlgData_p->IRData[IRRC].Wheel[AlgData_p->IRData[IRRC].Length - 1] +
		AlgData_p->IRData[IRLC].Wheel[AlgData_p->IRData[IRLC].Length - 1]) / 2;
	LRM_PrepStartWheel = (AlgData_p->IRData[IRR1].Wheel[0] + AlgData_p->IRData[IRL1].Wheel[0]) / 2;
	LRM_PrepEndwheel = (AlgData_p->IRData[IRR1].Wheel[AlgData_p->IRData[IRR1].Length - 1] +
		AlgData_p->IRData[IRL1].Wheel[AlgData_p->IRData[IRL1].Length - 1]) / 2;

	/*———————再根据左右计数红外斜率进行角度矫正———————*/
	if (DISTANCE_MM_IR_X)
	{
		Distance_X = DISTANCE_MM_IR_X;
		Distance_Y = (float)((AlgData_p->IRData[IRRC].Wheel[0] - AlgData_p->IRData[IRLC].Wheel[0]) +
			(AlgData_p->IRData[IRRC].Wheel[AlgData_p->IRData[IRRC].Length - 1] - AlgData_p->IRData[IRLC].Wheel[AlgData_p->IRData[IRLC].Length - 1])
			) / 2;
		Slope_MM = Distance_Y / Distance_X;
	}

	/*——————用于验证两个对管的宽度是否相差过大，不然计算角度不准————————————*/
	widthLeft = AlgData_p->IRData[IRLC].Wheel[AlgData_p->IRData[IRLC].Length - 1] - AlgData_p->IRData[IRLC].Wheel[0];
	widthRight = AlgData_p->IRData[IRRC].Wheel[AlgData_p->IRData[IRRC].Length - 1] - AlgData_p->IRData[IRRC].Wheel[0];
	if (ABS(widthLeft, widthRight) > IR_WIDTH_ERR)
	{
		MM_Flag = 1;
	}
	/*———————————————数据转换层————————————————*/


	MMSM_DataChangeOne_TMR(&TdnData_p[TDN_LMM], &AlgData_p->LMM[0], MM_PrepStartWheel, MM_PrepEndwheel, Slope_MM, i, MM_Flag, OFFSET_LMM_IR_X, OFFSET_MM_IR_Y, MM_WHEEL_ERR);
	MMSM_DataChangeOne_TMR(&TdnData_p[TDN_LMM1], &AlgData_p->LMM[1], MM_PrepStartWheel, MM_PrepEndwheel, Slope_MM, i, MM_Flag, OFFSET_LMM_IR_X, OFFSET_MM_IR_Y, MM_WHEEL_ERR);

	MMSM_DataChangeOne_TMR(&TdnData_p[TDN_RMM], &AlgData_p->RMM[1], MM_PrepStartWheel, MM_PrepEndwheel, Slope_MM, i, MM_Flag, OFFSET_RMM_IR_X, OFFSET_MM_IR_Y, MM_WHEEL_ERR);
	MMSM_DataChangeOne_TMR(&TdnData_p[TDN_RMM1], &AlgData_p->RMM[0], MM_PrepStartWheel, MM_PrepEndwheel, Slope_MM, i, MM_Flag, OFFSET_RMM_IR_X, OFFSET_MM_IR_Y, MM_WHEEL_ERR);

	for (i = 0; i < SM_NUM; i++)
	{
		MMSM_DataChangeOne_TMR(&TdnData_p[TDN_LM1 + i], &AlgData_p->LSM[SM_NUM - 1 - i], LRM_PrepStartWheel, LRM_PrepEndwheel, Slope_MM, i - 7, MM_Flag, OFFSET_LM1_IR_X, OFFSET_LRM_IR_Y, SM_WHEEL_ERR);
		MMSM_DataChangeOne_TMR(&TdnData_p[TDN_RM1 + i], &AlgData_p->RSM[i], LRM_PrepStartWheel, LRM_PrepEndwheel, Slope_MM, i, MM_Flag, OFFSET_RM1_IR_X, OFFSET_LRM_IR_Y, SM_WHEEL_ERR);
		//#ifdef WIN32
		//		char filePath[256];
		//		_mkdir("imgRes\\magWave\\");
		//		sprintf(filePath, "imgRes\\magWave\\LM%d.bmp", SM_NUM - 1 - i);
		//		drawMagWaveImg(filePath, AlgData_p->LSM[SM_NUM - 1 - i].Vol, AlgData_p->LSM[SM_NUM - 1 - i].Length);
		//		sprintf(filePath, "imgRes\\magWave\\RM%d.bmp", i);
		//		drawMagWaveImg(filePath, AlgData_p->RSM[i].Vol, AlgData_p->RSM[i].Length);
		//#endif
	}

#ifdef VC_PRINT_DEBUG

	FILE *myfile1;
	char nameout[100];
	int m = 0;
	int n = 0;		//边磁或者中磁的个数
	unsigned short int MatlabChar;

	for (n = 0; n < SM_NUM; n++)
	{
		sprintf(nameout, "C:\\1金标Debug\\LM%d.txt", n);
		fopen_s(&myfile1, nameout, "ab+");
		for (m = 0; m < AlgData_p->LSM[n].Length; m++)
		{
			MatlabChar = (unsigned short int)AlgData_p->LSM[n].Vol[m];
			fwrite(&MatlabChar, sizeof(MatlabChar), 1, myfile1);
		}
		fclose(myfile1);

		sprintf(nameout, "C:\\1金标Debug\\RM%d.txt", n);
		fopen_s(&myfile1, nameout, "ab+");
		for (m = 0; m < AlgData_p->RSM[n].Length; m++)
		{
			MatlabChar = (unsigned short int)AlgData_p->RSM[n].Vol[m];
			fwrite(&MatlabChar, sizeof(MatlabChar), 1, myfile1);
		}
		fclose(myfile1);

	}

	for (n = 0; n < MM_NUM; n++)
	{
		sprintf(nameout, "C:\\1金标Debug\\LMM%d.txt", n);
		fopen_s(&myfile1, nameout, "ab+");
		for (m = 0; m < AlgData_p->LMM[n].Length; m++)
		{
			MatlabChar = (unsigned short int)AlgData_p->LMM[n].Vol[m];
			fwrite(&MatlabChar, sizeof(MatlabChar), 1, myfile1);
		}
		fclose(myfile1);

		sprintf(nameout, "C:\\1金标Debug\\RMM%d.txt", n);
		fopen_s(&myfile1, nameout, "ab+");
		for (m = 0; m < AlgData_p->RMM[n].Length; m++)
		{
			MatlabChar = (unsigned short int)AlgData_p->RMM[n].Vol[m];
			fwrite(&MatlabChar, sizeof(MatlabChar), 1, myfile1);
		}
		fclose(myfile1);
	}

#endif

}

void DataChange(int LastError)
{

	/*OneDSignal *AlgSignal_p = NULL;
	TdnData_Alg *AlgData_p = NULL;

	AlgSignal_p = AlgSignal;

	AlgData_p = &AlgDataBuf;

	memset((char *)AlgData_p, 0, sizeof(AlgData_p));*/

	//LastErrorManage(LastError);

	//DistanceManage();

	UV_DataChange(AlgSignal, AlgDataBuf);

	IR_DataChange(AlgSignal, AlgDataBuf, LastError);

	BMHD_DataChange(AlgSignal, AlgDataBuf);

	MMSM_DataChange_JULONG_TMR(AlgSignal, AlgDataBuf);

	return;
}








#endif