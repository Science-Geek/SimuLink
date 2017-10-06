#ifndef _BAIJIA_MACRO_CFG_H_
#define _BAIJIA_MACRO_CFG_H_
#include "machine_type.h"

#ifdef BAIJIA

#define EDGE_DETECTION_VOID(len,sub)	if(len < sub){return;} 
#define EDGE_DETECTION_INT(len,sub)	if(len > sub){return 0;} 

#define ONE_LINE_BYTE_NUM		32			//曾意给cpu一排数据的字节数
typedef  unsigned int WheelData;
#define PEAK_PRECISION_WHEEL	16
#define ABS(m,n)	(((m)>(n))?((m)-(n)):((n)-(m)))

enum Sensor_Num
{
	IR_NUM = 4,
	UV_MID_NUM = 4,
	UV_SIDE_NUM = 2,
	EIR_NUM = 1,
	MM_NUM = 2,
	SM_NUM = 8,
	TOTEL_MAG_NUM = 20,//all mag num
};

/*------------------------About_UV-----------------------------*/
enum UV_Mid_Direction
{
	UV_MID_DIR1 = 0,
	UV_MID_DIR2,
	UV_MID_DIR3,
	UV_MID_DIR4,
};

enum UV_Side_Direction
{
	UV_SIDE_DIR12 = 0,
	UV_SIDE_DIR34,
};

/*------------------------About_IR-----------------------------*/
enum IR_Name_
{
	IRLC = 0,
	IRRC,
	IRL1,
	IRR1,
};


enum IR_Threshold_Info
{
	REF_VOL_THRED = 2500,   //状态电压阈值
	IR_CASH_WIDTH_MIN_CNT = 10,
	IR_WIDTH_ERR = 15,						//两个红外对管宽度允许误差
};

enum IR_About_Overlap_
{
	SENSI_ZHANLIAN_YOUWU = 50,
	OVERLAP_CALIBRETE_VOL = 500,
	DUBIOUS_VOL_NUM_THRED = 400,
	SENSI_VOL_NUM_OFFSET = 50,
	DUBIOUS_IR_THRED_CNT = 2,
};
/*------------------------About_DataChange-----------------------------*/

enum DataChange_Info		
{
	DISTANCE_MM_IR_X = 108,				//71.5mm			码盘单距		0.662998
	OFFSET_BM_IR_Y = 10,						// 3.7mm 大磁 和同一排上的计数红外的Y轴偏移码盘数

	OFFSET_LMM_IR_X = -32,				// 22mm 左中磁 与同一排上的计数红外的X轴上的偏移码盘
	OFFSET_RMM_IR_X = 32,				// 22mm 右中磁 与同一排上的计数红外的X轴上的偏移码盘
	OFFSET_MM_IR_Y = 5,					// 6.74mm 中磁 和同一排上的计数红外的Y轴偏移码盘数，单位:个数
	MM_WHEEL_ERR = 10,					// 中磁 码盘允许误差

	OFFSET_LM1_IR_X = -40,				// 36mm 左边磁 与对应红外中心再X轴上的偏移码盘数，单位：个数
	OFFSET_RM1_IR_X = 40,					// 36mm 右边磁 与对应红外中心再X轴上的偏移码盘数，单位：个数
	OFFSET_LRM_IR_Y = 12,					// 5.08边磁 和同一排上的计数红外的Y轴偏移码盘数，单位:个数
	SM_WHEEL_ERR = 18,						// 边磁 码盘允许误差

	Gap_MAG = 11,								//15mm 各个边磁之间的中心间距，单位：码盘个数

	MMSM_ERR_WHEEL = 3,
	DISTANCE_MM_Y = 5,
};


/*------------------------About_BM-----------------------------*/
enum BM_About_BigGap
{
	BM_GAP_NUM_MAX = 64,
};

enum BM_Threshold_
{
	BM_VOL_MAX = 3800,
	BM_VOL_MIN = 2400,

	BM_VALID_HIGH_SUB = 200,
	BM_HIGH_VOL = 3600,

	BM_VALID_MID_SUB = 1000,
	BM_MID_VOL = 2800,

	BM_VALID_LOW_SUB = 3300,
	BM_LOW_VOL = 500,

	BM_VALID_VOL = 2800,
	BMBASELINE = 2300,    ///边磁基准电压

	BM_INVALID_PEAK_MIN = 0,

};

/*------------------------About_MMSM-----------------------------*/
enum MMSM_Name_
{
	LMM = 0,
	LMM1,
	RMM = 0,
	RMM1,

	LSM1 = 0,
	LSM2, LSM3, LSM4,
	LSM5, LSM6, LSM7,	
	LSM8,

	RSM1 = 0,
	RSM2, RMS3, RSM4,
	RSM5, RSM6, RSM7, 
	RSM8,
};

/*------------------------About_LastErr-----------------------------*/
enum LastError
{
	LAST_ERR_INDEX = IR_NUM,
	LAST_ERR_BUF_LEN,
};

#endif

#endif