/*
* magImgConfig.cpp
*
*  Created on: 2017-8-18
*      Author: KangSong
*/

#include "string.h"
#include "math.h"
#include "../magImg.h"
#include "../magImgType.h"

#ifdef WIN32
//#include "imgEmulateTool.h"
#endif

#ifdef KANGYI
#define MM_WHEEL_OFFSET		82			//720/180 = 4pix = 1mm
#define LM_WHEEL_OFFSET		75
#define RM_WHEEL_OFFSET		75
#define LM_ADJUST			20
#define RM_ADJUST			-20
#define LMM_ADJUST		20
#define RMM_ADJUST		-20

// mag sensor attr
struct tagMagSensorZone_Foreign gMagSensorZone_Foreign[TDN_MAX] =
{
	{ gMagSensorZone_Foreign + 1, NULL, NULL, TDN_LM1, MAG_ZONE_LOCATION, 42 + LM_ADJUST, 20, LM_WHEEL_OFFSET },		//LM0
	{ gMagSensorZone_Foreign + 2, NULL, NULL, TDN_LM2, MAG_ZONE_LOCATION, 62 + LM_ADJUST, 20, LM_WHEEL_OFFSET },		//LM1
	{ gMagSensorZone_Foreign + 3, NULL, NULL, TDN_LM3, MAG_ZONE_LOCATION, 101 + LM_ADJUST, 20, LM_WHEEL_OFFSET },		//LM2
	{ gMagSensorZone_Foreign + 4, NULL, NULL, TDN_LM4, MAG_ZONE_LOCATION, 121 + LM_ADJUST, 20, LM_WHEEL_OFFSET },		//LM3

	{ gMagSensorZone_Foreign + 5, NULL, NULL, TDN_RM4, MAG_ZONE_LOCATION, 578 + RM_ADJUST, 20, RM_WHEEL_OFFSET },	//RM0
	{ gMagSensorZone_Foreign + 6, NULL, NULL, TDN_RM3, MAG_ZONE_LOCATION, 598+ RM_ADJUST, 20, RM_WHEEL_OFFSET },	//RM1
	{ gMagSensorZone_Foreign + 7, NULL, NULL, TDN_RM2, MAG_ZONE_LOCATION, 637 + RM_ADJUST, 20, RM_WHEEL_OFFSET },	//RM2
	{ gMagSensorZone_Foreign + 8, NULL, NULL, TDN_RM1, MAG_ZONE_LOCATION, 657 + RM_ADJUST, 20, RM_WHEEL_OFFSET },	//RM3

	{ gMagSensorZone_Foreign + 9, NULL, NULL, TDN_LMM1, MAG_ZONE_LOCATION, 242 + LMM_ADJUST, 20, MM_WHEEL_OFFSET },		//LSM
	{ gMagSensorZone_Foreign + 10, NULL, NULL, TDN_LMM, MAG_ZONE_LOCATION, 262 + LMM_ADJUST, 20, MM_WHEEL_OFFSET },		//RSM
	{ gMagSensorZone_Foreign + 11, NULL, NULL, TDN_RMM1, MAG_ZONE_LOCATION, 438 + RMM_ADJUST, 20, MM_WHEEL_OFFSET },		//LSM
	{ gMagSensorZone_Foreign + 12, NULL, NULL, TDN_RMM, MAG_ZONE_LOCATION, 458 + RMM_ADJUST, 20, MM_WHEEL_OFFSET },		//RSM

	{ gMagSensorZone_Foreign + 13, NULL, NULL, TDN_IRLC, MAG_MATH_LOCATION, 141, 8, 63 },	//LOCATINO1
	{ NULL, NULL, NULL, TDN_IRRC, MAG_MATH_LOCATION, 571, 8, 63 },	//LOCATINO2

	//{ gMagSensorZone + 9,NULL,NULL,UM1_UV,MAG_ZONE_LOCATION,280,80,MM_WHEEL_OFFSET },	//uvl
	//{ NULL,NULL,NULL,UM2_UV,MAG_ZONE_LOCATION,360,80,MM_WHEEL_OFFSET },//	257	435			//uvr
};
#endif

