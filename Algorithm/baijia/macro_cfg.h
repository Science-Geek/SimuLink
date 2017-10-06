#ifndef _BAIJIA_MACRO_CFG_H_
#define _BAIJIA_MACRO_CFG_H_
#include "machine_type.h"

#ifdef BAIJIA

#define EDGE_DETECTION_VOID(len,sub)	if(len < sub){return;} 
#define EDGE_DETECTION_INT(len,sub)	if(len > sub){return 0;} 

#define ONE_LINE_BYTE_NUM		32			//�����cpuһ�����ݵ��ֽ���
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
	REF_VOL_THRED = 2500,   //״̬��ѹ��ֵ
	IR_CASH_WIDTH_MIN_CNT = 10,
	IR_WIDTH_ERR = 15,						//��������Թܿ���������
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
	DISTANCE_MM_IR_X = 108,				//71.5mm			���̵���		0.662998
	OFFSET_BM_IR_Y = 10,						// 3.7mm ��� ��ͬһ���ϵļ��������Y��ƫ��������

	OFFSET_LMM_IR_X = -32,				// 22mm ���д� ��ͬһ���ϵļ��������X���ϵ�ƫ������
	OFFSET_RMM_IR_X = 32,				// 22mm ���д� ��ͬһ���ϵļ��������X���ϵ�ƫ������
	OFFSET_MM_IR_Y = 5,					// 6.74mm �д� ��ͬһ���ϵļ��������Y��ƫ������������λ:����
	MM_WHEEL_ERR = 10,					// �д� �����������

	OFFSET_LM1_IR_X = -40,				// 36mm ��ߴ� ���Ӧ����������X���ϵ�ƫ������������λ������
	OFFSET_RM1_IR_X = 40,					// 36mm �ұߴ� ���Ӧ����������X���ϵ�ƫ������������λ������
	OFFSET_LRM_IR_Y = 12,					// 5.08�ߴ� ��ͬһ���ϵļ��������Y��ƫ������������λ:����
	SM_WHEEL_ERR = 18,						// �ߴ� �����������

	Gap_MAG = 11,								//15mm �����ߴ�֮������ļ�࣬��λ�����̸���

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
	BMBASELINE = 2300,    ///�ߴŻ�׼��ѹ

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