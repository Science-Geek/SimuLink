/*
* analyze_diff_machine.h
*
*  Created on: 2017-8-19
*
*  Author: YanCan
*/
#ifndef _ANALYZE_DIFF_MACHINE_H_
#define _ANALYZE_DIFF_MACHINE_H_
#include <QString>
#include "machine_type.h"

#ifdef JULONG_TMR
#include "julong_tmr/macro_cfg.h"
#endif

#ifdef YUECHUANG
#include "yuechuang/macro_cfg.h"
#endif

#ifdef BAIJIA
#include "baijia/macro_cfg.h"
#endif

#ifdef RONGHE
#include "ronghe/macro_cfg.h"
#endif

#ifdef KANGYI
#include "kangyi/macro_cfg.h"
#endif

#ifdef CHUANWEI
#include "chuanwei/macro_cfg.h"
#endif

#define CD4051_CHANNEL_NUM		8
#define MATER_CHANNEL_SIZE		4096
/*-------------------------------------------------------------------------------------*/
#define ONE_DATA_BYTE			4				//一个ADC通道的数据大小，4个字节，带ADC通道号
#define USEFUL_ADC_CHANNEL_NUM	(ONE_LINE_BYTE_NUM / ONE_DATA_BYTE) - 2
/*-------------------------------------------------------------------------------------*/

typedef struct tagOneDSignal
{
	int* Wheel;
	int* Vol;
	int Length;
	int* Time;

}OneDSignal;

typedef struct Data_Alg
{
	OneDSignal IRData[IR_NUM];
	OneDSignal UVMidData[UV_MID_NUM];
	OneDSignal UVSideData[UV_SIDE_NUM];
	OneDSignal EIRData[EIR_NUM];
	OneDSignal BM;
	OneDSignal HD;
	OneDSignal LMM[MM_NUM];
	OneDSignal LMM_HD;
	OneDSignal RMM[MM_NUM];
	OneDSignal RMM_HD;
	OneDSignal LSM[SM_NUM];
	OneDSignal RSM[SM_NUM];

} TdnData_Alg;

typedef struct UI_Result_
{
	int angle;
	int Height;
	int ERRtype;
	int Amount;
	int Direction;
	int VersionM;
	int SirVer;
	int ImgAmount;
	int ImgVersion;
	int ErrTypeNum;
	int ErrTypeBuf[5];
	int New_Amount;
	int New_ERRtype;

}Ui_RESULT_RMB;

enum magSensorType_e
{
	TDN_BM,			TDN_HD,				TDN_LMM,	TDN_LMM1,		TDN_RMM,	
	TDN_RMM1,		TDN_LM1,				TDN_LM2,		TDN_LM3,			TDN_LM4,		
	TDN_LM5,			TDN_LM6,				TDN_LM7,		TDN_LM8,			TDN_RM1,		
	TDN_RM2,			TDN_RM3,				TDN_RM4,		TDN_RM5,			TDN_RM6,		
	TDN_RM7,			TDN_RM8,				TDN_IRLC,		TDN_IRRC,			TDN_IRL1,		
	TDN_IRR1,			TDN_MUV_U_1,	TDN_MUV_D_1,	TDN_MUV_U_2,	TDN_MUV_D_2,	
	TDN_SUV_U,		TDN_SUV_D,

	TDN_MAX,
};

#if((defined JULONG_TMR)||(defined BAIJIA)||(defined RONGHE)||(defined YUECHUANG)||(defined CHUANWEI))
/*------2------------4------------6------------0------------1-------------3----------*/
static int ADC0_ChannelMap[CD4051_CHANNEL_NUM][USEFUL_ADC_CHANNEL_NUM] =
{
	{TDN_MAX,	TDN_MUV_U_1,	TDN_BM,		TDN_MAX,	TDN_MAX,	TDN_MAX},
	{TDN_MAX,	TDN_MUV_U_2,	TDN_HD,		TDN_MAX,	TDN_MAX,	TDN_MAX},
	{TDN_IRRC,	TDN_SUV_U,			TDN_MAX,		TDN_MAX,	TDN_MAX,	TDN_MAX},
	{TDN_IRLC,	TDN_MAX,				TDN_RMM,	TDN_MAX,	TDN_MAX,	TDN_MAX},
	{TDN_MAX,	TDN_MUV_D_1,	TDN_MAX,		TDN_MAX,	TDN_MAX,	TDN_MAX},
	{TDN_MAX,	TDN_MUV_D_2,	TDN_LMM,	TDN_MAX,	TDN_MAX,	TDN_MAX},
	{TDN_MAX,	TDN_SUV_D,			TDN_RMM1,	TDN_MAX,	TDN_MAX,	TDN_MAX},
	{TDN_MAX,	TDN_MAX,				TDN_LMM1,	TDN_MAX,	TDN_MAX,	TDN_MAX},

};
/*-----2----- -----4----- -----6------ ----0-------- --1---- -------3----------*/
static int ADC1_ChannelMap[CD4051_CHANNEL_NUM][USEFUL_ADC_CHANNEL_NUM] =
{
	{TDN_IRR1,	TDN_MAX,	TDN_MAX,	TDN_MAX,	TDN_LM1,	TDN_RM1},
	{TDN_MAX,	TDN_MAX,	TDN_MAX,	TDN_MAX,	TDN_LM2,	TDN_RM2},
	{TDN_MAX,	TDN_MAX,	TDN_MAX,	TDN_MAX,	TDN_LM3,	TDN_RM3},
	{TDN_MAX,	TDN_MAX,	TDN_MAX,	TDN_MAX,	TDN_LM4,	TDN_RM4},
	{TDN_IRL1,		TDN_MAX,	TDN_MAX,	TDN_MAX,	TDN_LM5,	TDN_RM5},
	{TDN_MAX,	TDN_MAX,	TDN_MAX,	TDN_MAX,	TDN_LM6,	TDN_RM6},
	{TDN_MAX,	TDN_MAX,	TDN_MAX,	TDN_MAX,	TDN_LM7,	TDN_RM7},
	{TDN_MAX,	TDN_MAX,	TDN_MAX,	TDN_MAX,	TDN_LM8,	TDN_RM8},
};

#elif(defined KANGYI)
/*-----0---------------2----------------4------------6---------*/
static int ADC0_ChannelMap[CD4051_CHANNEL_NUM][USEFUL_ADC_CHANNEL_NUM] =
{
	{ TDN_IRR1, TDN_MUV_U_1, TDN_MAX, TDN_MAX },
	{ TDN_MAX, TDN_MAX, TDN_MAX, TDN_MAX },
	{ TDN_MAX, TDN_MAX, TDN_MAX, TDN_MAX },
	{ TDN_MAX, TDN_MAX, TDN_MAX, TDN_MAX },
	{ TDN_IRL1, TDN_MUV_D_1, TDN_MAX, TDN_MAX },
	{ TDN_MAX, TDN_MAX, TDN_MAX, TDN_MAX },
	{ TDN_MAX, TDN_MAX, TDN_MAX, TDN_MAX },
	{ TDN_MAX, TDN_MAX, TDN_MAX, TDN_MAX },
};
/*-----0---------------2----------------4------------6---------*/
static int ADC1_ChannelMap[CD4051_CHANNEL_NUM][USEFUL_ADC_CHANNEL_NUM] =
{
	{ TDN_MAX, TDN_MAX, TDN_LM1, TDN_BM },
	{ TDN_MAX, TDN_MAX, TDN_LM2, TDN_HD },
	{ TDN_IRRC, TDN_MAX, TDN_LM3, TDN_RMM },
	{ TDN_IRLC, TDN_MAX, TDN_LM4, TDN_RMM1 },
	{ TDN_MAX, TDN_MAX, TDN_RM1, TDN_LMM },
	{ TDN_MAX, TDN_MAX, TDN_RM2, TDN_LMM1 },
	{ TDN_MAX, TDN_MAX, TDN_RM3, TDN_MAX },
	{ TDN_MAX, TDN_MAX, TDN_RM4, TDN_MAX },
};

#endif


bool Data_Analyze(int Adc_Index, char *Temp, int Total_DataLen);
void CPU_DataBuf_Init();
void CPU_DataBuf_Delete(OneDSignal *OneDSignal_p);
bool One_Raw_Data_Analyze(QString filepath_s, int raw_num);


extern OneDSignal AlgSignal[TDN_MAX];
extern TdnData_Alg *AlgDataBuf;
extern Ui_RESULT_RMB *Currency_Result;

#endif