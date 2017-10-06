/*
 * magImgType.h
 *
 *  Created on: 2017-6-11
 *      Author: KangSong
 */

#ifndef MAG_IMG_TYPE_H_
#define MAG_IMG_TYPE_H_
#include <cv.h>
#include <highgui.h>

#ifdef JULONG_TMR
#include "julong_tmr/arbitrat.h"
#endif

#ifdef YUECHUANG
#include "yuechuang/arbitrat.h"
#endif

#ifdef KANGYI
#include "kangyi/arbitrat.h"
#endif

#ifdef BAIJIA
#include "baijia/arbitrat.h"
#endif

#ifdef RONGHE
#include "ronghe/arbitrat.h"
#endif

#ifdef CHUANWEI
#include "chuanwei/arbitrat.h"
#endif

#define ID_REGION_NUM	16
typedef enum
{
	MAG_ZONE_LOCATION,		//ó?óú′???óò?¨??
	MAG_MATH_LOCATION		//ó?óú′?í????￥???¨??
}magSensorArrt_e;

typedef enum
{
	CNY_ID_ALL,
	CNY_ID_RED,
	CNY_ID_BLACK,
	CNY_MM,
	CNY_V2015_LM,
	CNY_V2015_CHAR,
	CNY_V2015_NUM,
	CNY_V2015_BIG_100,
	CNY_BM
}magZoneMeaning_e;

typedef struct {
	int x;
	int y;
}imgPoint;

typedef struct {
	float x;
	float y;
}imgPoint2D32f;

typedef struct {
	imgPoint corner[4];
	int xMax;
	int xMin;
}tagZoneCorner;

typedef struct {
	imgPoint corner[4];
	bool valid;
}tagIDCorner;

typedef struct  
{
	bool imgInitialize;
	bool tdnInitialize;
	bool idRegionInitialize;
	//img
	int top_x[4];
	int top_y[4];
	int width;
	int height;
	int adaptWidth;
	int adaptHeight;
	int affineWidth;
	int affineHeight;
	int channel;
	int amount;
	int version;
	int direction;
	int	AffineOpt[6];
	int widthStep; 
	int ImgMosaicsLine;
	tagIDCorner IDCorner[ID_REGION_NUM];
	int id_num;
#if X86_X64_DEBUG
	IplImage *pImg_Y;
	IplImage *pImg_U;
	IplImage *pImg_IR;
#endif
	//tdn
	ARAITRAT_PARAM *pMagInfo;
}tagMagImg_Info;

struct tagTdnMagSensorRes
{
	struct tagTdnMagSensorRes *next;
	struct tagMagClass *pMagClass;
	wheelData_t start;
	wheelData_t end;
	magSensorType_e type;
};

struct tagMagZone
{
	struct tagMagZone *next;
	magZoneMeaning_e	zone;
	int xStart;
	int yStart;
	int width;
	int height;
};

typedef enum 
{
	SENSOR_BM_EN = 1,
	SENSOR_SM_EN = (1 << 1),
	SENSOR_LM_EN = (1 << 2),
	SENSOR_MM_EN = (1 << 3),
	SENSOR_HD_EN = (1 << 4),
	SENSOR_SMALL_EN = SENSOR_SM_EN + SENSOR_LM_EN + SENSOR_MM_EN,
	SENSOR_ALL_EN = SENSOR_BM_EN + SENSOR_SM_EN + SENSOR_LM_EN + SENSOR_MM_EN + SENSOR_HD_EN
}magSensorEnable_Foreign;

struct tagMagZone_Foreign
{
	struct tagMagZone_Foreign *next;
	int zone;
	int xStart;
	int yStart;
	int width;
	int height;
	magAttr_Foreign attr;
	magSensorEnable_Foreign sensorEnable;
};

struct tagMagClass
{
	struct tagMagClass *next;
	wheelData_t startW;
	wheelData_t endW;
	wheelData_t wheelLen;
	short startimg;
	short endimg;
	short coverZone;
	short peakNum;
	//	int energy;
};

struct tagMagneticZone
{
	struct tagMagneticZone *next;
	//	magAttr_e attr;
	//	magSensorEnable_e sensorEnable;
#if MM_STAT
	int zoneMin;
	int zoneMax;
	int coverMin;
	int coverMax;
#endif
	int start;
	int end;
	int percent;
	magZoneMeaning_e zone;
	int area;
};

struct tagMagneticZone_Foreign
{
	struct tagMagneticZone_Foreign *next;
	magAttr_Foreign attr;
	magSensorType_e sensorType;
	int zone;
	int start;
	int end;
	int percent;
	int area;
};

struct tagMagSensorZone
{
	struct tagMagSensorZone *next;
	struct tagMagClass *pMagClass;
	struct tagMagImgRes *pRes;
	magSensorType_e type;
	magSensorArrt_e Arrt;
	short position;
	short range;
	int offset;	
};

struct tagMagSensorZone_Foreign
{
	struct tagMagSensorZone_Foreign *next;
	struct tagMagClass *pMagClass;
	struct tagMagImgRes_Foreign *pRes;
	magSensorType_e type;
	magSensorArrt_e Arrt;
	short position;
	short range;
	int offset;
	short firstStart;
	short lastEnd;
};

typedef struct{
	int coverStart;
	int coverEnd;
	int coverageZone;
	int coverageRate;
	bool statStart;
}tagMagZoneStat;

struct tagNoteMagZone
{
	struct tagNoteMagZone *next;
	struct tagMagZone *pMagZone;
	struct tagMagZone *pID_part;
	int amount;
	int version;
};

struct tagNoteMagZone_Foreign
{
	struct tagNoteMagZone_Foreign *next;
	struct tagMagZone_Foreign *pMagZone;
	int amount;
	int version;
};



typedef struct
{
	tagMagZoneInfo ZoneInfo;
	tagMagSignalInfo SignalInfo;
	struct tagMagneticZone_Foreign *pMagneticZone;
	struct tagMagClass *pMagClass;
	short start;
	short end;
	short firstStart;
	short lastEnd;
}tagMagAnalyZone_Foreign;



typedef enum
{
	nonmagnetic,			//无磁
	InvalidMagnetic,		//无效磁
	//weaklyMagnetic,			//磁弱
	//StrongMagnetic,			//磁强
	validMagnetic,			//有效磁
	ignoreMagnetic			//可忽略的
}magJudgeType_e;

struct magWindowList
{
	struct magWindowList *prev;
	struct magWindowList *next;
	tagMagAnalyZone_Foreign pMagImgAnalyZone;
	magJudgeType_e type;
};

typedef struct {
	int32_t falseThred;
	int32_t suspiciousThred;
	bool	affeckEnable;
	bool	onlyZoneEnable;
	int32_t winMoveZone;
	int32_t SM_noneThred;
	int32_t SM_weakThred;
	int32_t SM_mediumThred;
	int32_t MM_noneThred;
	int32_t MM_weakThred;
	int32_t MM_mediumThred;
	bool	MM_weakenEnable;
	bool	reverseWeaken;		//美元背面磁有减弱
}tagMagArbitPara;

#define WHEEL_STAT_MAX		512
#define magnet_class_max	256
#define MAG_IMG_RES_NUM		16

extern tagMagImg_Info imgInfo;

#endif
