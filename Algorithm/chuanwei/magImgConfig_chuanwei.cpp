/*
* magImgConfig.cpp
*
*  Created on: 2017-8-18
*      Author: KangSong
*/

//#include <stdafx.h>
#include "string.h"
#include "math.h"
#include "../magImg.h"
#include "../magImgType.h"

#ifdef WIN32_
#include "imgEmulateTool.h"
#endif

#ifdef CHUANWEI

//CIS 前为-  后为+ 单位: wheel	(是否翻倍)
#define MM_WHEEL_OFFSET		-37
#define LM_WHEEL_OFFSET		52
#define RM_WHEEL_OFFSET		52
#define LM_ADJUST			4
#define RM_ADJUST			0


// mag sensor attr 
struct tagMagSensorZone_Foreign gMagSensorZone_Foreign[TDN_MAX] =
{
	//	74	106	157	618	586	535
	{ gMagSensorZone_Foreign + 1, NULL, NULL, TDN_LM1, MAG_ZONE_LOCATION, 29 + LM_ADJUST, 20, LM_WHEEL_OFFSET },		//LM0
	{ gMagSensorZone_Foreign + 2, NULL, NULL, TDN_LM2, MAG_ZONE_LOCATION, 49 + LM_ADJUST, 20, LM_WHEEL_OFFSET },		//LM1
	{ gMagSensorZone_Foreign + 3, NULL, NULL, TDN_LM3, MAG_ZONE_LOCATION, 88 + LM_ADJUST, 20, LM_WHEEL_OFFSET },		//LM2
	{ gMagSensorZone_Foreign + 4, NULL, NULL, TDN_LM4, MAG_ZONE_LOCATION, 108 + LM_ADJUST, 20, LM_WHEEL_OFFSET },		//LM3
	{ gMagSensorZone_Foreign + 5, NULL, NULL, TDN_LM5, MAG_ZONE_LOCATION, 147 + LM_ADJUST, 20, LM_WHEEL_OFFSET },		//LM4
	{ gMagSensorZone_Foreign + 6, NULL, NULL, TDN_LM6, MAG_ZONE_LOCATION, 167 + LM_ADJUST, 20, LM_WHEEL_OFFSET },		//LM5
	{ gMagSensorZone_Foreign + 7, NULL, NULL, TDN_LM7, MAG_ZONE_LOCATION, 206 + LM_ADJUST, 20, LM_WHEEL_OFFSET },		//LM6
	{ gMagSensorZone_Foreign + 8, NULL, NULL, TDN_LM8, MAG_ZONE_LOCATION, 226 + LM_ADJUST, 20, LM_WHEEL_OFFSET },		//LM7

	{ gMagSensorZone_Foreign + 9, NULL, NULL, TDN_RM1, MAG_ZONE_LOCATION, 671 + RM_ADJUST, 20, RM_WHEEL_OFFSET },	//RM0
	{ gMagSensorZone_Foreign + 10, NULL, NULL, TDN_RM2, MAG_ZONE_LOCATION, 652 + RM_ADJUST, 20, RM_WHEEL_OFFSET },	//RM1
	{ gMagSensorZone_Foreign + 11, NULL, NULL, TDN_RM3, MAG_ZONE_LOCATION, 612 + RM_ADJUST, 20, RM_WHEEL_OFFSET },	//RM2
	{ gMagSensorZone_Foreign + 12, NULL, NULL, TDN_RM4, MAG_ZONE_LOCATION, 592 + RM_ADJUST, 20, RM_WHEEL_OFFSET },	//RM3
	{ gMagSensorZone_Foreign + 13, NULL, NULL, TDN_RM5, MAG_ZONE_LOCATION, 553 + RM_ADJUST, 20, RM_WHEEL_OFFSET },	//RM4
	{ gMagSensorZone_Foreign + 14, NULL, NULL, TDN_RM6, MAG_ZONE_LOCATION, 533 + RM_ADJUST, 20, RM_WHEEL_OFFSET },	//RM5
	{ gMagSensorZone_Foreign + 15, NULL, NULL, TDN_RM7, MAG_ZONE_LOCATION, 494 + RM_ADJUST, 20, RM_WHEEL_OFFSET },	//RM6
	{ gMagSensorZone_Foreign + 16, NULL, NULL, TDN_RM8, MAG_ZONE_LOCATION, 474 + RM_ADJUST, 20, RM_WHEEL_OFFSET },	//RM7  

	{ gMagSensorZone_Foreign + 17, NULL, NULL, TDN_LMM, MAG_ZONE_LOCATION, 254, 20, MM_WHEEL_OFFSET },		//LSM
	{ gMagSensorZone_Foreign + 18, NULL, NULL, TDN_LMM1, MAG_ZONE_LOCATION, 274, 20, MM_WHEEL_OFFSET },		//RSM
	{ gMagSensorZone_Foreign + 19, NULL, NULL, TDN_RMM1, MAG_ZONE_LOCATION, 427, 20, MM_WHEEL_OFFSET },		//LSM
	{ NULL, NULL, NULL, TDN_RMM, MAG_ZONE_LOCATION, 447, 20, MM_WHEEL_OFFSET },		//RSM
	//   141	571
	//{ gMagSensorZone_Foreign + 21, NULL, NULL, TDN_IRLC, MAG_MATH_LOCATION, 141, 8, 63 },	//LOCATINO1
	//{ NULL, NULL, NULL, TDN_IRRC, MAG_MATH_LOCATION, 571, 8, 63 },	//LOCATINO2

	//{ gMagSensorZone_Foreign + 23, NULL, NULL, UM1_UV, MAG_ZONE_LOCATION, 280, 80, MM_WHEEL_OFFSET },	//uvl
	//{ NULL, NULL, NULL, UM2_UV, MAG_ZONE_LOCATION, 360, 80, MM_WHEEL_OFFSET },//	257	435			//uvr

};

#endif