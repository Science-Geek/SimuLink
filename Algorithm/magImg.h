/*
* magImg.h
*
*  Created on: 2017-6-9
*      Author: KangSong
*/

#ifndef MAG_IMG_H_
#define MAG_IMG_H_

#include "machine_type.h"

#ifdef MAG_IMG_DEBUG
#define X86_X64_DEBUG		1
#define PRINT_RES			0
#define MAGIMG_DEBUG_TOOL	1
#else
#define X86_X64_DEBUG		0
#define PRINT_RES			0
#define MAGIMG_DEBUG_TOOL	0
#endif


#if X86_X64_DEBUG

#include "stdint.h"
#endif

#define CUSTOM_JL			0
#define CUSTOM_YC				1
#define CUSTOM_RH				2
#define CUSTOM_KY				3
#define CUSTOM_BJ				4
#define CUSTOM_CW				5

#ifdef JULONG_TMR
#define CUSTOM_TYPE				CUSTOM_JL
#include "julong_tmr/arbitrat.h"
#endif

#ifdef YUECHUANG
#define CUSTOM_TYPE				CUSTOM_YC
#include "yuechuang/arbitrat.h"
#endif

#ifdef KANGYI
#define CUSTOM_TYPE				CUSTOM_KY
#include "kangyi/arbitrat.h"
#endif

#ifdef BAIJIA
#define CUSTOM_TYPE				CUSTOM_BJ
#include "baijia/arbitrat.h"
#endif

#ifdef RONGHE
#define CUSTOM_TYPE				CUSTOM_RH
#include "ronghe/arbitrat.h"
#endif

#ifdef CHUANWEI
#define CUSTOM_TYPE				CUSTOM_CW
#include "chuanwei/arbitrat.h"
#endif

//客户



typedef unsigned int wheelData_t;
typedef short tdnData_t;
typedef unsigned int uint32_t;
#if 0
typedef enum {
	BM,
	HD,

	RM_0,
	RM_1,
	RM_2,
	RM_3,
	RM_4,
	RM_5,
	RM_6,
	RM_7,

	LM_0,
	LM_1,
	LM_2,
	LM_3,
	LM_4,
	LM_5,
	LM_6,
	LM_7,

	RMM0,
	RMM1,	//RM saturation
	LMM0,
	LMM1,	//LM saturation

	IR_IN,
	IR1_R1,
	IR1_RC,
	IR1_LC,
	IR1_L1,

	UM1_UV,
	UM2_UV,
	US1_UV,
	UUV,

	DM1_UV,
	DM2_UV,
	DS1_UV,
	DUV,

	TDN_NONE,

	CIS,
	MAG_SENSOR_MAX
}magSensorType_e;		//主控设备类型
#endif
typedef struct{
	int mag;
	int zone;
}magImgScore;

struct tagMagImgRes
{
	struct tagMagImgRes *next;
	magSensorType_e sensor;
	magImgScore ID_red;
	magImgScore ID_black;
	magImgScore back_char;
	magImgScore back_num;
	magImgScore mm;
	magImgScore v15_big_100;
	magImgScore jamming;	
};

typedef enum
{
	STRONG_MAG,				//强
	WEAK_MAG,					//弱
	MEDIUM_MAG,			//中
	NONE_MAG,					//无
}magAttr_Foreign;			//主控设备类型

typedef struct
{
	magAttr_Foreign magAttr;
	//int energy;
}tagMagSignalInfo;

typedef struct
{
	magAttr_Foreign magAttr;
	int16_t percent;		//交集所占百分比
	int16_t windowPer;		//分析窗口所占区域百分比
	uint32_t area;
}tagMagZoneInfo;

struct tagMagImgRes_Foreign
{
	struct tagMagImgRes_Foreign *next;
	tagMagZoneInfo ZoneInfo;
	tagMagSignalInfo SignalInfo;
	magSensorType_e sensor;
	magAttr_Foreign magAttr;
	int start;
	int end;
	int zone;
	int percent;
	int area;
	int totalZone;
	int falseZone;
	int score;
	bool valid;
};




//void magImg_reset();
void magImg_imgInfoSet(unsigned char *pTopData,int *top_x,int *top_y,int width,int height,int channel,int amount,int version,int dirction,wheelData_t *pWheel,int wheelLen);


#if X86_X64_DEBUG
//void magImg_debugSet(IplImage *pImg_Y,IplImage *pImg_U);
//void MagImgRes_print(struct tagMagImgRes *pMagRes);
#endif
#endif
