//#include <stdafx.h>

#include "machine_type.h"
#ifdef MAG_IMG_DEBUG
#include  <stdio.h>
#include <QtCore>
#include <QFile>
#endif

#include "string.h"
#include "math.h"
#include "magImg.h"
#include "magImgType.h"
#include "CurrencyType.h"


extern struct tagMagSensorZone_Foreign gMagSensorZone_Foreign[TDN_MAX];

#ifdef WIN32
#include  <direct.h>
#endif

#if X86_X64_DEBUG
static IplImage *pImg_Y,*pImg_U;
#endif
#if MAGIMG_DEBUG_TOOL
#include "magImgDebug.h"
#endif

static float wheelMean;
static wheelData_t wheelStat[WHEEL_STAT_MAX] = { 0 };
static wheelData_t wheelStart = 0;
static wheelData_t wheelEnd = 0;
static wheelData_t ImgMosaicsEndWheel;
static wheelData_t ImgMosaicsStartWheel;

static int wheelStatNum = 0;
static int wheelDatLen = 0;
static int ImgMosaics = 0;
static int ImgMosaicsIndex = 0;

tagMagImg_Info imgInfo;
static bool peakFilter[MMSM_POS_BUF_MAX/2];
static struct tagMagClass MagClass[magnet_class_max];
static struct tagMagImgRes magImgRes[TDN_MAX];
struct tagMagImgRes_Foreign magImgRes_Foreign[TDN_MAX];
static int magClassCnt;
static int magImgResCnt = 0;
static int magImgStatCnt = 0;

static struct tagMagneticZone_Foreign MagneticZone_Foreign[128];
struct magWindowList gMagWindowList[128];
struct tagMagneticZone_Foreign *pMagneticZone;
static int32_t MagneticZoneCnt = 0;
static int WinCnt = 0;
tagMagArbitPara gMagArbitPara;

#if X86_X64_DEBUG

#define TYPE_ROTATE_LINE	0		//线性
#define TYPE_ROTATE_BILI	1		//双线性
#define DIRECTION_LEFT		0
#define DIRECTION_RIGHT		1
#define DIRECTION_SAME		2

bool imgChannleCross = false;

typedef struct RotateZoneInfo_
{
	int32_t xStart;
	int32_t yStart;
	int32_t	width;
	int32_t height;
	int32_t widthStep;
	int32_t RotateAngle;			//0.5度为单位
	int32_t	Direction;				//左右，由于要算重心，截图要方向一致
	int32_t channel;				//0y,1u
	int32_t type;					//双线性1，线性0
	int32_t mean;
}RotateZoneInfo_;

int32_t WarpAffineZoneInt(IplImage* pBmpImage, IplImage* pNewImage, RotateZoneInfo_ *ID_ZONE, int *Mat)
{
	int32_t i, j, widthstep;
	// 源图像的宽度和高度
	int32_t lWidth;
	int32_t lHeight;
	// 旋转后图像的宽度和高度
	int32_t lNewWidth;
	int32_t lNewHeight;
	int32_t yStart, xStart;
	int32_t i1, j1;

	int32_t i0, j0, decimal_i, decimal_j;
	//	int32_t i1,j1;
	int32_t f12, f34, Coordinate1;

	uint8_t *P_NewImage = (uint8_t*)pNewImage->imageData;
	uint8_t *P_BmpImage = (uint8_t*)pBmpImage->imageData;


	lWidth = pBmpImage->width;
	lHeight = pBmpImage->height;
	lNewWidth = pNewImage->width;
	lNewHeight = pNewImage->height;
	widthstep = pBmpImage->widthStep;
	yStart = ID_ZONE->yStart;
	xStart = ID_ZONE->xStart;

	if (ID_ZONE->type == TYPE_ROTATE_LINE)
	{
		/*********** 线性插值 ****************/
		if (ID_ZONE->Direction == DIRECTION_RIGHT)
		{
			for (i = yStart; i < yStart + lNewHeight; i++)
			{
				// 针对图像每列进行操作
				for (j = xStart + lNewWidth - 1; j >= xStart; j--)
				{
					j1 = (j * Mat[0] + i * Mat[1] + Mat[2] + 32768) >> 16;
					i1 = (j * Mat[3] + i * Mat[4] + Mat[5] + 32768) >> 16;

					// 判断是否在源图范围内
					if ((j1 >= 0) && (j1 < lWidth - 1) && (i1 >= 0) && (i1 < lHeight - 1))
					{
						*P_NewImage = P_BmpImage[i1*widthstep + j1];
					}
					else
						*P_NewImage = 0;

					P_NewImage++;
				}
				P_NewImage += pNewImage->widthStep - lNewWidth;
			}


		}
		else if (ID_ZONE->Direction == DIRECTION_LEFT)
		{
			for (i = yStart + lNewHeight - 1; i >= yStart; i--)
			{
				// 针对图像每列进行操作
				for (j = xStart; j < xStart + lNewWidth; j++)
				{
					j1 = (j * Mat[0] + i * Mat[1] + Mat[2] + 32768) >> 16;
					i1 = (j * Mat[3] + i * Mat[4] + Mat[5] + 32768) >> 16;

					// 判断是否在源图范围内
					if ((j1 >= 0) && (j1 < lWidth - 1) && (i1 >= 0) && (i1 < lHeight - 1))
					{
						*P_NewImage = P_BmpImage[i1*widthstep + j1];
					}
					else
						*P_NewImage = 0;

					P_NewImage++;
				}
				P_NewImage += pNewImage->widthStep - lNewWidth;
			}
		}
		else if (ID_ZONE->Direction == DIRECTION_SAME)
		{
			for (i = yStart; i < yStart + lNewHeight; i++)
			{
				// 针对图像每列进行操作
				for (j = xStart; j < xStart + lNewWidth; j++)
				{
					j1 = (j * Mat[0] + i * Mat[1] + Mat[2] + 32768) >> 16;
					i1 = (j * Mat[3] + i * Mat[4] + Mat[5] + 32768) >> 16;

					// 判断是否在源图范围内
					if ((j1 >= 0) && (j1 < lWidth - 1) && (i1 >= 0) && (i1 < lHeight - 1))
					{
						*P_NewImage = P_BmpImage[i1*widthstep + j1];
					}
					else
						*P_NewImage = 0;

					P_NewImage++;
				}
				P_NewImage += pNewImage->widthStep - lNewWidth;
			}
		}
		else
		{
			return -1;
		}
	}
	else
	{
		/*********** 双线性插值 ****************/
		if (ID_ZONE->Direction == DIRECTION_RIGHT)
		{
			for (i = yStart; i < yStart + lNewHeight; i++)
			{
				// 针对图像每列进行操作
				for (j = xStart + lNewWidth - 1; j >= xStart; j--)
				{
					j0 = j * Mat[0] + i * Mat[1] + Mat[2];
					i0 = j * Mat[3] + i * Mat[4] + Mat[5];

					decimal_i = i0 & 0xffff;
					decimal_j = j0 & 0xffff;
					i1 = i0 >> 16;
					j1 = j0 >> 16;
					Coordinate1 = i1*widthstep + j1;
					// 判断是否在源图范围内
					if ((j1 >= 0) && (j1 < lWidth - 1) && (i1 >= 0) && (i1 < lHeight - 1))
					{
						f12 = (P_BmpImage[Coordinate1] << 16) + decimal_j*(P_BmpImage[Coordinate1 + 1] - P_BmpImage[Coordinate1]);
						f34 = (P_BmpImage[Coordinate1 + widthstep] << 16) + decimal_j*(P_BmpImage[Coordinate1 + widthstep + 1] - P_BmpImage[Coordinate1 + widthstep]);
						*P_NewImage = abs(f12 + decimal_i* ((f34 - f12) >> 16)) >> 16;

					}
					else
						*P_NewImage = 0;

					P_NewImage++;
				}
				P_NewImage += pNewImage->widthStep - lNewWidth;
			}


		}
		else if (ID_ZONE->Direction == DIRECTION_LEFT)
		{
			for (i = yStart + lNewHeight - 1; i >= yStart; i--)
			{
				// 针对图像每列进行操作
				for (j = xStart; j < xStart + lNewWidth; j++)
				{
					j0 = j * Mat[0] + i * Mat[1] + Mat[2];
					i0 = j * Mat[3] + i * Mat[4] + Mat[5];

					decimal_i = i0 & 0xffff;
					decimal_j = j0 & 0xffff;
					i1 = i0 >> 16;
					j1 = j0 >> 16;
					Coordinate1 = i1*widthstep + j1;
					// 判断是否在源图范围内
					if ((j1 >= 0) && (j1 < lWidth - 1) && (i1 >= 0) && (i1 < lHeight - 1))
					{
						f12 = (P_BmpImage[Coordinate1] << 16) + decimal_j*(P_BmpImage[Coordinate1 + 1] - P_BmpImage[Coordinate1]);
						f34 = (P_BmpImage[Coordinate1 + widthstep] << 16) + decimal_j*(P_BmpImage[Coordinate1 + widthstep + 1] - P_BmpImage[Coordinate1 + widthstep]);
						*P_NewImage = abs(f12 + decimal_i* ((f34 - f12) >> 16)) >> 16;

					}
					else
						*P_NewImage = 0;

					P_NewImage++;
				}
				P_NewImage += pNewImage->widthStep - lNewWidth;
			}
		}
		else if (ID_ZONE->Direction == DIRECTION_SAME)
		{
			for (i = yStart; i < yStart + lNewHeight; i++)
			{
				// 针对图像每列进行操作
				for (j = xStart; j < xStart + lNewWidth; j++)
				{
					j0 = j * Mat[0] + i * Mat[1] + Mat[2];
					i0 = j * Mat[3] + i * Mat[4] + Mat[5];

					decimal_i = i0 & 0xffff;
					decimal_j = j0 & 0xffff;
					i1 = i0 >> 16;
					j1 = j0 >> 16;
					Coordinate1 = i1*widthstep + j1;
					// 判断是否在源图范围内
					if ((j1 >= 0) && (j1 < lWidth - 1) && (i1 >= 0) && (i1 < lHeight - 1))
					{
						f12 = (P_BmpImage[Coordinate1] << 16) + decimal_j*(P_BmpImage[Coordinate1 + 1] - P_BmpImage[Coordinate1]);
						f34 = (P_BmpImage[Coordinate1 + widthstep] << 16) + decimal_j*(P_BmpImage[Coordinate1 + widthstep + 1] - P_BmpImage[Coordinate1 + widthstep]);
						*P_NewImage = abs(f12 + decimal_i* ((f34 - f12) >> 16)) >> 16;

					}
					else
						*P_NewImage = 0;

					P_NewImage++;
				}
				P_NewImage += pNewImage->widthStep - lNewWidth;
			}
		}
		else
		{
			return -1;
		}
	}


	return 0;
}

void magImg_debugSet(unsigned char *pimgTopData, int width, int height, int channle)
{
	int offset = 0;
	IplImage *pImg_Y = cvCreateImage(cvSize(width, height*channle), IPL_DEPTH_8U, 1);
	memcpy(pImg_Y->imageData, pimgTopData, width*height*channle);
	cvSaveImage("imgRes\\magImg\\pImg_Y.bmp", pImg_Y);
	imgInfo.pImg_Y = pImg_Y;
	imgInfo.pImg_U = NULL;
	RotateZoneInfo_ zone;
	zone.type = TYPE_ROTATE_LINE;
	zone.Direction = DIRECTION_SAME;
	zone.xStart = 0;
	zone.yStart = 0;
	if (imgChannleCross)
	{
		offset = imgInfo.width * 3;	//	IR
		height = pImg_Y->height;
	}
	else
	{
		offset = imgInfo.width*imgInfo.height * 2;	//	IR
		height = pImg_Y->height / channle;
	}

	imgInfo.pImg_IR = cvCreateImage(cvSize(720, height), IPL_DEPTH_8U, 1);
	IplImage * pImgAffine = cvCreateImage(cvSize(720, 270), IPL_DEPTH_8U, 1);

	for (int i = 0; i<imgInfo.pImg_IR->height; i++)
	{
		for (int j = 0; j<imgInfo.pImg_IR->width; j++)
		{
			imgInfo.pImg_IR->imageData[i*imgInfo.pImg_IR->widthStep + j] = pImg_Y->imageData[i*pImg_Y->widthStep + offset + j];
		}
	}

	WarpAffineZoneInt(imgInfo.pImg_IR, pImgAffine, &zone, imgInfo.AffineOpt);

	_mkdir("imgRes\\magImg");
	cvSaveImage("imgRes\\magImg\\ir.bmp", imgInfo.pImg_IR);
	cvSaveImage("imgRes\\magImg\\affine.bmp", pImgAffine);
	cvReleaseImage(&pImgAffine);

}

#if 0
const char *magSensorNameGet(magSensorType_e SensorType)
{
	switch (SensorType)
	{
	case BM:
		return "BM";
	case HD:
		return "HD";
	case LM_0:
		return "LM0";
	case LM_1:
		return "LM1";
	case LM_2:
		return "LM2";
	case LM_3:
		return "LM3";
	case LM_4:
		return "LM4";
	case LM_5:
		return "LM5";
	case LM_6:
		return "LM6";
	case LM_7:
		return "LM7";

	case RM_0:
		return "RM0";
	case RM_1:
		return "RM1";
	case RM_2:
		return "RM2";
	case RM_3:
		return "RM3";
	case RM_4:
		return "RM4";
	case RM_5:
		return "RM5";
	case RM_6:
		return "RM6";
	case RM_7:
		return "RM7";

	case RMM0:
		return "RMM0";
	case RMM1:
		return "RMM1";
	case LMM0:
		return "LMM0";
	case LMM1:
		return "LMM1";

	case IR1_R1:
		return "IR1_R1";
	case IR1_RC:
		return "IR1_RC";
	case IR1_LC:
		return "IR1_LC";
	case IR1_L1:
		return "IR1_L1";
	case UM1_UV:
		return "UM1_UV";
	case UM2_UV:
		return "UM2_UV";
	case DM1_UV:
		return "DM1_UV";
	case DM2_UV:
		return "DM2_UV";
	default:
		return "UNKNOW";
	}
}

const char *magZoneNameGet(magZoneMeaning_e zone)
{
	static char ZoneName[256];
	switch (zone)
	{
	case CNY_ID_RED:
		return "CNY_ID_RED";
	case CNY_ID_BLACK:
		return "CNY_ID_BLACK";
	case CNY_MM:
		return "CNY_MM";
	case CNY_V2015_CHAR:
		return "CNY_V2015_CHAR";
	case CNY_V2015_NUM:
		return "CNY_V2015_NUM";
	case CNY_V2015_BIG_100:
		return "CNY_V2015_BIG_100";
	default:
		sprintf(ZoneName, "zone%d", zone);
		return ZoneName;
	}
}

unsigned char *MagImgInit(char *FilePathStr)
{
	int i = 0;
	int line = 0;
	int chNum = 0;
	int height = 0;
	char fileName[256] = { 0 };
	char filePath[256] = { 0 };

	strcpy(fileName, FilePathStr);

	fileName[strlen(FilePathStr) - strlen(TDN_FILE_SUFFIX)] = 0;

	sprintf(filePath, "%s%s", fileName, IMG_U_FILE_SUFFIX);
	pImg_Y = cvLoadImage(filePath, CV_LOAD_IMAGE_GRAYSCALE);
	sprintf(filePath, "%s%s", fileName, IMG_D_FILE_SUFFIX);
	pImg_U = cvLoadImage(filePath, CV_LOAD_IMAGE_GRAYSCALE);
	if (pImg_Y != NULL && pImg_U != NULL)
	{
		return  (unsigned char*)pImg_Y->imageData;

	}

	return NULL;
}
#endif
void MagImgFree()
{

	if (imgInfo.pImg_Y != NULL)
	{
		cvReleaseImage(&imgInfo.pImg_Y);
	}
	if (imgInfo.pImg_U != NULL)
	{
		cvReleaseImage(&imgInfo.pImg_U);
	}
	if (imgInfo.pImg_IR != NULL)
	{
		cvReleaseImage(&imgInfo.pImg_IR);
	}
}

#ifdef MAG_IMG_DEBUG_

void MagImgRes_print(struct tagMagImgRes *pMagRes)
{
	struct tagMagImgRes *pRes;
	for (pRes = pMagRes; pRes != NULL; pRes = pRes->next)
	{

		saveImgLog("magImgRes", ":%s	ID_RED[%3d,%3d],ID_BLACK[%3d,%3d],MM[%3d,%3d],CHAR[%3d,%3d],NUM[%3d,%3d],JAMMING[%3d,%3d],BIG_100[%3d,%3d]", magSensorNameGet(pRes->sensor),
			pRes->ID_red.mag, pRes->ID_red.zone, pRes->ID_black.mag, pRes->ID_black.zone, pRes->mm.mag, pRes->mm.zone,
			pRes->back_char.mag, pRes->back_char.zone, pRes->back_num.mag, pRes->back_num.zone, pRes->jamming.mag, pRes->jamming.zone, pRes->v15_big_100.mag, pRes->v15_big_100.zone);
	}
}


void saveMagImgStat(const char *path, int score)
{
	FILE *magImgStatFile;
	char statLog[256];
	if ((magImgStatFile = fopen(path, "rb+")) == NULL)
	{
		if ((magImgStatFile = fopen(path, "wb")) == NULL)
		{
			return;
		}
	}

	fseek(magImgStatFile, 0, SEEK_END);
	sprintf(statLog, "%d\r\n", score);
	fwrite(statLog, strlen(statLog), 1, magImgStatFile);
	fclose(magImgStatFile);
}

void saveStatErrImg(const char *path, magSensorType_e sensor, int score, int zone)
{
	char imgCheckPic[256];
	MMSM_WHEEL_PARAM *pPeakInfo = tdnPeakInfoGet(sensor);
	_mkdir("imgRes\\magImg");
	sprintf(imgCheckPic, "imgRes\\magImg\\magZone%d.bmp", g_FileNum);
	IplImage *pZone = cvLoadImage(imgCheckPic, CV_LOAD_IMAGE_COLOR);

	if (pZone == NULL)
		return;

	int height = pZone->height + 500;
	int width = pPeakInfo->Length > pZone->width ? pPeakInfo->Length : pZone->width;

	IplImage *pImg = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
	cvZero(pImg);
	for (int i = 0; i<pZone->height; i++)
		memcpy(pImg->imageData + i*pImg->widthStep + (pImg->width - pZone->width) / 2, pZone->imageData + i*pZone->widthStep, pZone->width*pZone->nChannels);

	for (int i = 1; i<pPeakInfo->Length; i++)
		cvLine(pImg, cvPoint(i - 1, pZone->height + 499 - pPeakInfo->Vol[i - 1] / 6), cvPoint(i, pZone->height + 499 - pPeakInfo->Vol[i] / 6), CV_RGB(0, 255, 0), 2);

	sprintf(imgCheckPic, "%s\\%d_%d_%d_%d_%s_.bmp", path, magImgStatCnt, g_FileNum, score, zone, magSensorNameGet(sensor));
	cvSaveImage(imgCheckPic, pImg);
	cvReleaseImage(&pImg);
	cvReleaseImage(&pZone);
}

void magImgRes_stat(struct tagMagImgRes *pMagRes)
{
	struct tagMagImgRes *pRes, *pResRed = NULL, *pResBlack = NULL;
	bool id_zone_weakly = false;
	int id_red_single_max = 0;
	int id_black_single_max = 0;

	_mkdir("stat");
	for (pRes = pMagRes; pRes != NULL; pRes = pRes->next)
	{

		if (pRes->ID_red.zone > 0 && pRes->ID_black.zone == 0)
		{
			if (pRes->ID_red.mag > id_red_single_max)
			{
				id_red_single_max = pRes->ID_red.mag;
				pResRed = pRes;
			}


		}


		if (pRes->ID_red.zone == 0 && pRes->ID_black.zone > 0 && pRes->ID_black.zone <40)
		{

			if (pRes->ID_black.mag > id_black_single_max)
			{
				id_black_single_max = pRes->ID_black.mag;
				pResBlack = pRes;
			}
		}
		if (pRes->ID_red.zone > 0)
		{
			if (pRes->sensor >= LM_0 && pRes->sensor <= LM_7)
			{
				saveMagImgStat("stat\\LM_ID_redMag.txt", pRes->ID_red.mag);
				saveMagImgStat("stat\\LM_ID_redZone.txt", pRes->ID_red.zone);
			}
			else
			{
				saveMagImgStat("stat\\RM_ID_redMag.txt", pRes->ID_red.mag);
				saveMagImgStat("stat\\RM_ID_redZone.txt", pRes->ID_red.zone);
			}
		}

		if (pRes->ID_black.zone > 0)
		{
			if (pRes->sensor >= LM_0 && pRes->sensor <= LM_7)
			{
				saveMagImgStat("stat\\LM_ID_blackMag.txt", pRes->ID_black.mag);
				saveMagImgStat("stat\\LM_ID_blackZone.txt", pRes->ID_black.zone);
			}
			else
			{
				saveMagImgStat("stat\\RM_ID_blackMag.txt", pRes->ID_black.mag);
				saveMagImgStat("stat\\RM_ID_blackZone.txt", pRes->ID_black.zone);
			}
		}

		if (pRes->mm.zone > 0 && pRes->mm.mag <40)
		{
			if (pRes->sensor >= LM_0 && pRes->sensor <= LM_7)
			{
				saveMagImgStat("stat\\LM_ID_mmMag.txt", pRes->mm.mag);
				saveMagImgStat("stat\\LM_ID_mmZone.txt", pRes->mm.zone);
			}
			else
			{
				saveMagImgStat("stat\\RM_ID_mmMag.txt", pRes->mm.mag);
				saveMagImgStat("stat\\RM_ID_mmZone.txt", pRes->mm.zone);
			}
		}

		if (pRes->back_char.zone > 0 && pRes->back_char.mag <40)
		{
			if (pRes->sensor >= LM_0 && pRes->sensor <= LM_7)
			{
				saveMagImgStat("stat\\LM_ID_charMag.txt", pRes->back_char.mag);
				saveMagImgStat("stat\\LM_ID_charZone.txt", pRes->back_char.zone);
			}
			else
			{
				saveMagImgStat("stat\\RM_ID_charMag.txt", pRes->back_char.mag);
				saveMagImgStat("stat\\RM_ID_charZone.txt", pRes->back_char.zone);
			}
		}

		if (pRes->back_num.zone > 0 && pRes->back_num.mag <40)
		{
			if (pRes->sensor >= LM_0 && pRes->sensor <= LM_7)
			{
				saveMagImgStat("stat\\LM_ID_numMag.txt", pRes->back_num.mag);
				saveMagImgStat("stat\\LM_ID_numZone.txt", pRes->back_num.zone);
			}
			else
			{
				saveMagImgStat("stat\\RM_ID_numMag.txt", pRes->back_num.mag);
				saveMagImgStat("stat\\RM_ID_numZone.txt", pRes->back_num.zone);
			}
		}
	}

	if (id_red_single_max < 30 && pResRed != NULL)
	{
		_mkdir("stat\\IR_red");
		saveStatErrImg("stat\\IR_red", pResRed->sensor, pResRed->ID_red.mag, pResRed->ID_red.zone);
		id_zone_weakly = true;
	}

	if (id_black_single_max <30 && pResBlack != NULL)
	{
		_mkdir("stat\\ID_black");
		saveStatErrImg("stat\\ID_black", pResBlack->sensor, pResBlack->ID_black.mag, pResBlack->ID_black.zone);
		id_zone_weakly = true;
	}

	if (id_zone_weakly)
	{
		for (pRes = pMagRes; pRes != NULL; pRes = pRes->next)
		{
			if (pRes->ID_red.zone > 0 && pRes->ID_black.zone > 0)
			{
				if (pRes->sensor >= LM_1 && pRes->sensor <= LM_5)
				{
					saveMagImgStat("stat\\LM_mix_Mag.txt", pRes->ID_red.mag);
					saveMagImgStat("stat\\LM_mix_red.txt", pRes->ID_red.zone);
					saveMagImgStat("stat\\LM_mix_black.txt", pRes->ID_black.zone);

				}
				else
				{
					saveMagImgStat("stat\\RM_mix_Mag.txt", pRes->ID_red.mag);
					saveMagImgStat("stat\\RM_mix_red.txt", pRes->ID_red.zone);
					saveMagImgStat("stat\\RM_mix_black.txt", pRes->ID_black.zone);
				}
			}
		}
	}

	//saveMagImgStat("stat\\magImgStat_ID_blck.txt",score_ID_black);
	//saveMagImgStat("stat\\magImgStat_back_num.txt",score_back_num);
	//saveMagImgStat("stat\\magImgStat_back_char.txt",score_back_char);
	//if(score_ID_red < 40 && score_ID_red>=0)
	//{
	//	_mkdir("stat\\IR_red");
	//	saveStatErrImg("stat\\IR_red",sensor_ID_red,score_ID_red);
	//}

	//if(score_ID_black < 40 && score_ID_black>=0)
	//{
	//	_mkdir("stat\\ID_black");
	//	saveStatErrImg("stat\\ID_black",sensor_ID_black,score_ID_black);
	//}

	//if(score_back_char < 40 && score_back_char>=0)
	//{
	//	_mkdir("stat\\back_char");
	//	saveStatErrImg("stat\\back_char",sensor_back_char,score_back_char);
	//}


	//if(score_back_num < 40 && score_back_num>=0)
	//{
	//	_mkdir("stat\\back_num");
	//	saveStatErrImg("stat\\back_num",sensor_back_num,score_back_num);
	//}


	//saveImgLog("magImgResStat","ID_RED%3d,ID_BLACK%3d,CHAR%3d,NUM%3d\n",score_ID_red,score_ID_black,score_back_char,score_back_num);
	magImgStatCnt++;
}
#endif	

#endif

void magArbitConfig(int32_t currencyType)
{
	gMagArbitPara.MM_weakenEnable = true;
	gMagArbitPara.reverseWeaken = false;
	if (currencyType == CURRENCY_TYPE_USD)
	{
		gMagArbitPara.falseThred = 20;
		gMagArbitPara.suspiciousThred = 45;
		gMagArbitPara.winMoveZone = 10;
		gMagArbitPara.SM_mediumThred = 50;
		gMagArbitPara.SM_weakThred = 35;
		gMagArbitPara.SM_noneThred = 0;
		gMagArbitPara.MM_mediumThred = 55;
		gMagArbitPara.MM_weakThred = 35;
		gMagArbitPara.MM_noneThred = 0;
		gMagArbitPara.affeckEnable = true;
		gMagArbitPara.onlyZoneEnable = false;
	}
	else if (currencyType == CURRENCY_TYPE_RUB)
	{
		gMagArbitPara.falseThred = 45;
		gMagArbitPara.suspiciousThred = 65;
		gMagArbitPara.winMoveZone = 10;
		gMagArbitPara.SM_mediumThred = 55;
		gMagArbitPara.SM_weakThred = 25;
		gMagArbitPara.SM_noneThred = 0;

		gMagArbitPara.MM_mediumThred = 55;
		gMagArbitPara.MM_weakThred = 25;
		gMagArbitPara.MM_noneThred = 0;

		gMagArbitPara.affeckEnable = false;
		gMagArbitPara.onlyZoneEnable = true;
	}
	else if (currencyType == CURRENCY_TYPE_EUR)
	{
		gMagArbitPara.falseThred = 20;
		gMagArbitPara.suspiciousThred = 45;
		gMagArbitPara.winMoveZone = 10;

		gMagArbitPara.SM_mediumThred = 55;
		gMagArbitPara.SM_weakThred = 35;
		gMagArbitPara.SM_noneThred = 0;

		gMagArbitPara.MM_mediumThred = 55;
		gMagArbitPara.MM_weakThred = 35;
		gMagArbitPara.MM_noneThred = 0;

		gMagArbitPara.affeckEnable = true;
		gMagArbitPara.onlyZoneEnable = true;
	}
	else
	{
		gMagArbitPara.falseThred = 20;
		gMagArbitPara.suspiciousThred = 45;
		gMagArbitPara.winMoveZone = 10;

		gMagArbitPara.SM_mediumThred = 55;
		gMagArbitPara.SM_weakThred = 35;
		gMagArbitPara.SM_noneThred = 0;

		gMagArbitPara.MM_mediumThred = 55;
		gMagArbitPara.MM_weakThred = 35;
		gMagArbitPara.MM_noneThred = 0;

		gMagArbitPara.affeckEnable = true;
		gMagArbitPara.onlyZoneEnable = false;
	}
}

// 区域无磁
void magZoneWeakenJudgment(struct magWindowList *pWIN_Head, struct magWindowList *pWinList, int moveRange, int start, int end)
{
	struct magWindowList *pWinMag;
	struct tagMagClass *pMagClass;
	short weakenZone = 0, winZone;
	int move;

	winZone = pWinList->pMagImgAnalyZone.end - pWinList->pMagImgAnalyZone.start + 1;

	//if(pWinList->pAnalyZone.end < end+6 && pWinList->pAnalyZone.end > end-12)
	//{
	//	if(winZone >= 12)
	//		weakenZone += 12;
	//	else
	//		weakenZone += winZone;
	//}

	pWinMag = pWinList->prev;
	pWinMag = pWinList;
	move = 0;
	while (pWinMag != pWIN_Head)
	{
		move = pWinList->pMagImgAnalyZone.start - pWinMag->pMagImgAnalyZone.end;
		if (move > moveRange)
			break;
		if (pWinMag->type == InvalidMagnetic || pWinMag->type == validMagnetic)
		{
			if (winZone >= 8)
				weakenZone += 8;
			else
				weakenZone += winZone;
#if WIN32_VC&GENERAL_DEBUG_PRT_
			TRACE("磁性弱判:win %d~%d zone%d 根据 win %d~%d\n", pWinList->pAnalyZone.start, pWinList->pAnalyZone.end,
				weakenZone, pWinMag->pAnalyZone.start, pWinMag->pAnalyZone.end);
#endif
			if (pWinMag->pMagImgAnalyZone.end - pWinMag->pMagImgAnalyZone.start + 1 < 16)
				pWinMag->type = validMagnetic;
			break;
		}
		pWinMag = pWinMag->next;
	}

	pWinMag = pWinList->next;
	move = 0;
	while (pWinMag != pWIN_Head)
	{
		move = pWinMag->pMagImgAnalyZone.start - pWinList->pMagImgAnalyZone.end;
		if (move > moveRange)
			break;
		if (pWinMag->type == validMagnetic)
		{
			pMagClass = pWinMag->pMagImgAnalyZone.pMagClass;
			for (pWinMag = pWinMag->next; pWinMag != pWIN_Head; pWinMag = pWinMag->next)
			{
				if (pWinMag->pMagImgAnalyZone.pMagClass != pMagClass)
					break;
				if (pWinMag->type == InvalidMagnetic)
				{
#if WIN32_VC&GENERAL_DEBUG_PRT_
					TRACE("磁性弱判:win %d~%d zone%d 根据 win %d~%d 磁性\n", pWinList->pAnalyZone.start, pWinList->pAnalyZone.end,
						weakenZone, pWinMag->pAnalyZone.start, pWinMag->pAnalyZone.end);
#endif
					weakenZone += pWinMag->pMagImgAnalyZone.end - pWinMag->pMagImgAnalyZone.start + 1;
					if (winZone * 2 > pWinMag->pMagImgAnalyZone.end - pWinMag->pMagImgAnalyZone.start + 1)
						pWinMag->type = validMagnetic;
					break;
				}
			}
			if (pWinMag == pWIN_Head)
				break;
		}
		else if (pWinMag->type == InvalidMagnetic)
		{
			if (winZone >= 8)
				weakenZone += 8;
			else
				weakenZone += winZone;
#if WIN32_VC&GENERAL_DEBUG_PRT_
			TRACE("磁性弱判:win %d~%d zone%d 根据 win %d~%d 磁性\n", pWinList->pAnalyZone.start, pWinList->pAnalyZone.end,
				weakenZone, pWinMag->pAnalyZone.start, pWinMag->pAnalyZone.end);
#endif
			if (pWinMag->pMagImgAnalyZone.end - pWinMag->pMagImgAnalyZone.start + 1 < 16)
				pWinMag->type = validMagnetic;

			break;
		}
		pWinMag = pWinMag->next;
	}

	if (weakenZone > winZone / 3)
	{
		pWinList->type = validMagnetic;
	}
}

// 无效磁
struct magWindowList *mathInvalidWin(struct magWindowList *pWinMag, struct magWindowList *pWIN_Head)
{
	struct tagMagClass *pMagClass;
	pMagClass = pWinMag->pMagImgAnalyZone.pMagClass;
	for (pWinMag = pWinMag->next; pWinMag != pWIN_Head; pWinMag = pWinMag->next)
	{
		if (pWinMag->pMagImgAnalyZone.pMagClass != pMagClass)
			break;
		if (pWinMag->type == nonmagnetic)
		{
#if WIN32_VC&GENERAL_DEBUG_PRT_
			TRACE("磁性弱判:win %d~%d zone%d 根据 win %d~%d 磁性\n", pWinList->pAnalyZone.start, pWinList->pAnalyZone.end,
				weakenZone, pWinMag->pAnalyZone.start, pWinMag->pAnalyZone.end);
#endif
			return pWinMag;
		}
	}
	return NULL;
}

void magSingleWeakenJudgment(struct magWindowList *pWIN_Head, struct magWindowList *pWinList, int moveRange, int start, int end)
{
	struct magWindowList *pWinMag, *pWinInvalid;
	short weakenZone = 0, winZone;
	int move, preZone;
	//进钞干扰导致多磁
	winZone = pWinList->pMagImgAnalyZone.end - pWinList->pMagImgAnalyZone.start + 1;
	if (pWinList->pMagImgAnalyZone.start < start + 6 && pWinList->pMagImgAnalyZone.end > start - 6)
	{
		if (winZone >= 8)
			weakenZone += 8;
		else
			weakenZone += winZone;
	}

	//有效磁的延后性导致的多磁
	pWinMag = pWinList->prev;
	if (pWinMag != pWIN_Head)
	{
		if (pWinMag->type == validMagnetic)
		{
			if (winZone >= 8)
				weakenZone += 8;
			else
				weakenZone += winZone;
#if WIN32_VC&GENERAL_DEBUG_PRT_
			TRACE("磁性弱判:win %d~%d zone%d 根据 win %d~%d\n", pWinList->pAnalyZone.start, pWinList->pAnalyZone.end,
				weakenZone, pWinMag->pAnalyZone.start, pWinMag->pAnalyZone.end);
#endif
		}
	}

	//匹配错位导致的多磁
	//	pWinMag = pWIN_Head->next;
	//	move = 0; 
	for (pWinMag = pWIN_Head->next; pWinMag != pWIN_Head; pWinMag = pWinMag->next)
	{
		move = pWinMag->pMagImgAnalyZone.start - pWinList->pMagImgAnalyZone.end;
		preZone = pWinList->pMagImgAnalyZone.start - pWinMag->pMagImgAnalyZone.end;
		if (move > moveRange)
			break;
		if (preZone > moveRange || pWinList == pWinMag)
			continue;

		if (pWinMag->type == validMagnetic)
		{
			pWinInvalid = mathInvalidWin(pWinMag, pWIN_Head);
			if (pWinInvalid != NULL)
			{
				weakenZone += pWinMag->pMagImgAnalyZone.end - pWinMag->pMagImgAnalyZone.start + 1;
				if (winZone * 2 > pWinMag->pMagImgAnalyZone.end - pWinMag->pMagImgAnalyZone.start + 1)
					pWinMag->type = validMagnetic;
				break;
			}
			else
			{
				if (winZone >= 8)
					weakenZone += 8;
				else
					weakenZone += winZone;
			}

		}
		else if (pWinMag->type == nonmagnetic)
		{
			weakenZone += pWinMag->pMagImgAnalyZone.end - pWinMag->pMagImgAnalyZone.start + 1;
#if WIN32_VC&GENERAL_DEBUG_PRT_
			TRACE("磁性弱判:win %d~%d zone%d 根据 win %d~%d 区域\n", pWinList->pAnalyZone.start, pWinList->pAnalyZone.end,
				weakenZone, pWinMag->pAnalyZone.start, pWinMag->pAnalyZone.end);
#endif
			break;
		}
		else if (pWinMag->pMagImgAnalyZone.ZoneInfo.magAttr != NONE_MAG && pWinMag->pMagImgAnalyZone.SignalInfo.magAttr == NONE_MAG)
		{
			weakenZone += pWinMag->pMagImgAnalyZone.end - pWinMag->pMagImgAnalyZone.start + 1;
#if WIN32_VC&GENERAL_DEBUG_PRT_
			TRACE("磁性弱判:win %d~%d zone%d 根据 win %d~%d 区域\n", pWinList->pAnalyZone.start, pWinList->pAnalyZone.end,
				weakenZone, pWinMag->pAnalyZone.start, pWinMag->pAnalyZone.end);
#endif
		}
	}


	if (weakenZone > winZone / 3)
	{
		pWinList->type = validMagnetic;
	}
}

static void magZoneScoreCalc(struct tagMagImgRes_Foreign *pResList, struct magWindowList *pWINlist)
{
	int winZone;
	if (pResList == NULL || pWINlist == NULL)
		return;

	if (/*gMagArbitPara.onlyZoneEnable &&*/ pResList->magAttr == NONE_MAG)
	{
#if WIN32_VC_
		TRACE("%s:res %d~%d zone %s total%d 无效-只关注有磁区域\n", magSensorNameGet(sensorType), pResList->start, pResList->end,
			magAttrStrGet(pResList->attr), pResList->end - pResList->start + 1);
#endif
		return;
	}
	pResList->end = pWINlist->pMagImgAnalyZone.end;
	if (pWINlist->pMagImgAnalyZone.pMagClass != NULL)
	{
		winZone = pWINlist->pMagImgAnalyZone.pMagClass->endimg - pWINlist->pMagImgAnalyZone.pMagClass->startimg + 1;
	}
	else
	{
		winZone = 0;
	}
	if (pWINlist->type == nonmagnetic || pWINlist->type == InvalidMagnetic)
	{
		pResList->totalZone += winZone;
		pResList->falseZone += winZone;
	}
	else if (pWINlist->type == validMagnetic)
	{
		pResList->totalZone += winZone;

	}
	if (pResList->totalZone > 3)
	{

		if (pResList->magAttr == MEDIUM_MAG && pResList->falseZone < pResList->totalZone)
			pResList->score = 100;
		else if (pResList->magAttr == NONE_MAG && pResList->falseZone < 10)
			pResList->score = 100;
		else
			pResList->score = pResList->falseZone <= 0 ? 100 : (pResList->totalZone - pResList->falseZone) * 100 / pResList->totalZone;
		pResList->valid = true;
#if WIN32_VC&GENERAL_DEBUG_PRT_
		TRACE("%s:res %d~%d zone %s total%d false%d score%d\n", magSensorNameGet(sensorType), pResList->start, pResList->end,
			magAttrStrGet(pResList->attr), pResList->totalZone, pResList->falseZone, pResList->score);
#endif
	}
	else
		pResList->valid = false;

	//	if(pResList->end - pResList->start > 3)
	//	{
	//		if(pResList->totalZone > 0)
	//		{
	//
	//			pResList->score = pResList->falseZone <= 0 ? 100 : (pResList->totalZone -pResList->falseZone)*100 / pResList->totalZone;
	//#if WIN32_VC&GENERAL_DEBUG_PRT
	//			TRACE("%s:res %d~%d zone %s total%d false%d score%d\n",magSensorNameGet(sensorType),pResList->start,pResList->end,
	//				magAttrStrGet(pResList->attr),pResList->totalZone,pResList->falseZone,pResList->score);
	//#endif
	//			pResList->valid = 1;
	//
	//		}
	//#if WIN32_VC
	//		else
	//		{
	//			TRACE("%s:res %d~%d zone %s total%d 无效-属性转换\n",magSensorNameGet(sensorType),pResList->start,pResList->end,
	//				magAttrStrGet(pResList->attr),pResList->end - pResList->start+1);
	//		}
	//#endif
	//	}
	//#if WIN32_VC
	//	else
	//	{
	//		TRACE("%s:res %d~%d zone %s total%d 无效-区域过小\n",magSensorNameGet(sensorType),pResList->start,pResList->end,
	//			magAttrStrGet(pResList->attr),pResList->end - pResList->start+1);
	//	}
	//#endif
}

struct tagMagImgRes_Foreign *newMagImgRes()
{
	struct tagMagImgRes_Foreign *pNew;
	if (magImgResCnt < 128)
	{
		pNew = magImgRes_Foreign + magImgResCnt;
		pNew->next = NULL;
		pNew->totalZone = 0;
		pNew->falseZone = 0;
		pNew->valid = false;
		pNew->start = 0;
		pNew->end = 0;
		pNew->zone = 0;
		pNew->percent = 0;
		pNew->area = 0;
		pNew->score = 0;
		magImgResCnt++;
		return pNew;
	}
	return NULL;
}

void WinListSave(struct magWindowList *pWinHead, struct magWindowList *pWinList)
{
	pWinHead->prev->next = pWinList;
	pWinList->prev = pWinHead->prev;
	pWinList->next = pWinHead;
	pWinHead->prev = pWinList;
}

struct magWindowList *newWinList()
{
	struct magWindowList *pNew;
	if (WinCnt < 500)
	{
		pNew = gMagWindowList + WinCnt;
		pNew->next = pNew;
		pNew->prev = pNew;
		pNew->type = ignoreMagnetic;
		WinCnt++;
		return pNew;
	}
#if WIN32_VC_
	TRACE("%s-%d文件:newWinList full！！！！\n", g_FilePathName, g_FileNum);
#endif
	return NULL;
}

int magWindosCheck(struct magWindowList *pWINlist)	//,TDNAreaInf *Res,int start,int end,int magStart,int magEnd)
{
	//int zone = pWINlist->pAnalyZone.end - pWINlist->pAnalyZone.start +1;

	switch (pWINlist->pMagImgAnalyZone.ZoneInfo.magAttr)
	{
	case NONE_MAG:
		if (pWINlist->pMagImgAnalyZone.SignalInfo.magAttr == STRONG_MAG || pWINlist->pMagImgAnalyZone.SignalInfo.magAttr == MEDIUM_MAG)
		{
			pWINlist->type = InvalidMagnetic;
			return 0;
		}
		break;
	case WEAK_MAG:
		if (pWINlist->pMagImgAnalyZone.SignalInfo.magAttr == STRONG_MAG || pWINlist->pMagImgAnalyZone.SignalInfo.magAttr == MEDIUM_MAG)
		{
			pWINlist->type = validMagnetic;
			return 0;
		}
		break;
	case MEDIUM_MAG:
	case STRONG_MAG:
		if (pWINlist->pMagImgAnalyZone.SignalInfo.magAttr == NONE_MAG)
		{
			pWINlist->type = nonmagnetic;
			return 0;
		}
		else
		{
			pWINlist->type = validMagnetic;
			return 0;
		}
	default:
#if WIN32_VC_
		TRACE("%s-%d文件:magWindosCheck errAttr %d！！！！\n", pWINlist->pAnalyZone.ZoneInfo.magAttr);
#endif
		break;
	}
	pWINlist->type = ignoreMagnetic;
	return 0;

}

bool magImgWheelStat(wheelData_t *dat,int len,int ImgMosaicsLine)
{
	int i,fullWheelAdd,fullWheelAddCnt;
	wheelData_t j;
	wheelDatLen = len;
	wheelStat[0] = 0;
	wheelStatNum = 1;
	if (ImgMosaicsLine > 0)
	{
		ImgMosaics = ImgMosaicsLine * imgInfo.channel;
		if (ImgMosaics >= len)
			ImgMosaics = 0;
		else
		{
			ImgMosaicsEndWheel = dat[ImgMosaics]*2;
			ImgMosaicsStartWheel =  dat[ImgMosaics - imgInfo.channel]* 2;
		}
	}
	else
		ImgMosaics = 0;

	fullWheelAdd = 0;
	fullWheelAddCnt = 0;

	for(i = 1;i<wheelDatLen;i++)
	{
		if(dat[i] != dat[i-1])
		{
			wheelStat[wheelStatNum] = (i + wheelStat[wheelStatNum - 1]) / 2;
			wheelStatNum++;
			
			if (dat[i] - dat[i - 1] > 100)
			{
				return false;
			}
			else if (dat[i] - dat[i - 1] > 1)
			{
				for (j = dat[i - 1] + 1; j < dat[i]; j++)
				{
					wheelStat[wheelStatNum] = i;
					wheelStatNum++;
					wheelStat[wheelStatNum] = i;
					wheelStatNum++;

				}
			}
			else if (wheelStatNum > 2)
			{
				fullWheelAdd += i- wheelStat[wheelStatNum - 2];
				fullWheelAddCnt += 2;
			}

			wheelStat[wheelStatNum] = i;
			wheelStatNum++;
		}
	}

	wheelStart = dat[0]*2;
	wheelEnd = dat[wheelDatLen-1]*2;

	wheelMean = fullWheelAdd /(float)fullWheelAddCnt;


	if(wheelStat[wheelStatNum - 1] != wheelDatLen-1)
	{
		//		lastWheelEnd = temp + wheelStat[wheelStatNum-1];
		wheelStat[wheelStatNum] = wheelDatLen-1;
		wheelStatNum++;
	}


	if(wheelEnd != wheelStart+wheelStatNum-1)
	{
#if WIN32_VC
		//TRACE("%s %s-%d文件:imgWheelStat falied！！！！\n",LOG_ERR,g_FilePathName,g_FileNum);
#endif
	}
	return true;
}

int transWheel2Img(wheelData_t start,int subWheel,int scale,int diff,int diffScale ,int height)
{
	int res=0,offset;
	int channel = 5;
	wheelData_t wheel,magWheel;
	magWheel = start*diffScale + subWheel*diffScale / scale;
	//magWheel = start + subWheel  / scale;
	wheel = magWheel - diff;

	if (ImgMosaics > 0)
	{
		if (diff < 0)
		{
			if (wheel > ImgMosaicsStartWheel && magWheel < ImgMosaicsEndWheel)
			{
				wheel += ImgMosaicsEndWheel - ImgMosaicsStartWheel;//(wheel > ImgMosaicsEndWheel?ImgMosaicsEndWheel: wheel) - ImgMosaicsStartWheel;
			}
		}
		else
		{
			if (wheel < ImgMosaicsEndWheel   && magWheel > ImgMosaicsStartWheel)
			{
				wheel -= ImgMosaicsEndWheel - ImgMosaicsStartWheel;// (wheel < ImgMosaicsStartWheel ? ImgMosaicsStartWheel : wheel);
			}
		}

	}

	if (wheel < wheelStart + 1)
	{
		offset = (wheelStart + 1 - wheel) * wheelMean;
		res = (wheelStat[1] - offset) / channel;
		return 0;
	}

	if (wheel > wheelStart + wheelStatNum - 2)
	{
		offset = (wheel - (wheelStart + wheelStatNum - 2)) * wheelMean;
		res = (wheelStat[wheelStatNum - 2] + offset) / channel;
		return height - 1;
	}

	offset = wheel - wheelStart;
	if (offset >= 0 && offset < wheelStatNum)
	{
		res = wheelStat[offset];
		//if((wheel&1) && offset + 1<wheelStatNum)
		//{
		//	res += pWheelStat[offset+1] - res;
		//}
		res /= channel;
	}

	return res;
}

//img info set
static int gaus(double a[],double b[],int n)
{
	int l,k,i,j,is,p,q;
	double d,t;
	int js[6];
	//    js=(int *)malloc(n*sizeof(int));
	l=1;
	for (k=0;k<=n-2;k++)
	{ d=0.0;
	for (i=k;i<=n-1;i++)
		for (j=k;j<=n-1;j++)
		{ t=fabs(a[i*n+j]);
	if (t>d) { d=t; js[k]=j; is=i;}
	}
	if (d+1.0==1.0) l=0;
	else
	{ if (js[k]!=k)
	for (i=0;i<=n-1;i++)
	{ p=i*n+k; q=i*n+js[k];
	t=a[p]; a[p]=a[q]; a[q]=t;
	}
	if (is!=k)
	{ for (j=k;j<=n-1;j++)
	{ p=k*n+j; q=is*n+j;
	t=a[p]; a[p]=a[q]; a[q]=t;
	}
	t=b[k]; b[k]=b[is]; b[is]=t;
	}
	}
	if (l==0)
	{ //free(js); printf("fail\n");

		return(0);
	}
	d=a[k*n+k];
	for (j=k+1;j<=n-1;j++)
	{ p=k*n+j; a[p]=a[p]/d;}
	b[k]=b[k]/d;
	for (i=k+1;i<=n-1;i++)
	{ for (j=k+1;j<=n-1;j++)
	{ p=i*n+j;
	a[p]=a[p]-a[i*n+k]*a[k*n+j];
	}
	b[i]=b[i]-a[i*n+k]*b[k];
	}
	}
	d=a[(n-1)*n+n-1];
	if (fabs(d)+1.0==1.0)
	{ //free(js); printf("fail\n");
		return(0);
	}
	b[n-1]=b[n-1]/d;
	for (i=n-2;i>=0;i--)
	{ t=0.0;
	for (j=i+1;j<=n-1;j++)
		t=t+a[i*n+j]*b[j];
	b[i]=b[i]-t;
	}
	js[n-1]=n-1;
	for (k=n-1;k>=0;k--)
		if (js[k]!=k)
		{ t=b[k]; b[k]=b[js[k]]; b[js[k]]=t;}
		// free(js);
		return(1);
}

static void GetAffineMat(tagMagImg_Info *pImgInfo)
{
	imgPoint2D32f srcTri[3],dstTri[3];
	double AffineOperator_y[6];

	dstTri[0].y = (pImgInfo->adaptHeight - pImgInfo->affineHeight)/2;
	dstTri[0].x = (pImgInfo->adaptWidth - pImgInfo->affineWidth)/2;

	dstTri[1].y = (pImgInfo->adaptHeight - pImgInfo->affineHeight)/2;
	dstTri[1].x = (pImgInfo->adaptWidth + pImgInfo->affineWidth)/2;

	dstTri[2].y = (pImgInfo->adaptHeight + pImgInfo->affineHeight)/2;
	dstTri[2].x = (pImgInfo->adaptWidth + pImgInfo->affineWidth)/2;

	srcTri[0].x = pImgInfo->top_x[0];
	srcTri[0].y = pImgInfo->top_y[0];
	srcTri[1].x = pImgInfo->top_x[1];
	srcTri[1].y = pImgInfo->top_y[1];
	srcTri[2].x = pImgInfo->top_x[2];
	srcTri[2].y = pImgInfo->top_y[2];



	int i = 0;
	double a[6 * 6] = { 0 };
	for(i = 0; i < 3; i++ )
	{
		int j = i*12;
		int k = i*12+6;
		a[j] = a[k + 3] = dstTri[i].x;//srcTri[i].x
		a[j + 1] = a[k + 4] = dstTri[i].y;//srcTri[i].y;

		a[j+2] = a[k+5] = 1;
		a[j+3] = a[j+4] = a[j+5] = 0;
		a[k] = a[k+1] = a[k+2] = 0;
		AffineOperator_y[i * 2] = srcTri[i].x;//dstTri[i].x;
		AffineOperator_y[i * 2 + 1] = srcTri[i].y;//dstTri[i].y;

	}

	gaus(a,AffineOperator_y,6);

	for(i=0;i<6;i++)
	{
		pImgInfo->AffineOpt[i] =(int)(AffineOperator_y[i] *65536>=0?AffineOperator_y[i] *65536+0.5:AffineOperator_y[i] *65536-0.5);
	}

}

#define mediumThred  35//55
#define weakThred  20//25
#define noneThred  10//15

static void magZoneAttrJudge(tagMagAnalyZone_Foreign *pAnalyZone)
{
	if (pAnalyZone->ZoneInfo.magAttr == MEDIUM_MAG || pAnalyZone->ZoneInfo.magAttr == STRONG_MAG)
	{
		if (pAnalyZone->ZoneInfo.percent < noneThred)
		{
#if WIN32_VC&GENERAL_DEBUG_PRT_
			TRACE("窗口%d,%d per%d %s 转为 %s\n", pAnalyZone->start, pAnalyZone->end, pAnalyZone->ZoneInfo.percent, magAttrStrGet(pAnalyZone->ZoneInfo.magAttr), magAttrStrGet(NONE_MAG));
#endif
			pAnalyZone->ZoneInfo.magAttr = NONE_MAG;
		}
		else if (pAnalyZone->ZoneInfo.percent < weakThred)
		{
#if WIN32_VC&GENERAL_DEBUG_PRT_
			TRACE("窗口%d,%d per%d %s 转为 %s\n", pAnalyZone->start, pAnalyZone->end, pAnalyZone->ZoneInfo.percent, magAttrStrGet(pAnalyZone->ZoneInfo.magAttr), magAttrStrGet(WEAK_MAG));
#endif
			pAnalyZone->ZoneInfo.magAttr = WEAK_MAG;
		}
		else if (pAnalyZone->ZoneInfo.percent < mediumThred)
		{
			if (pAnalyZone->ZoneInfo.magAttr == STRONG_MAG)
			{
#if WIN32_VC&GENERAL_DEBUG_PRT_
				TRACE("窗口%d,%d per%d %s 转为 %s\n", pAnalyZone->start, pAnalyZone->end, pAnalyZone->ZoneInfo.percent, magAttrStrGet(pAnalyZone->ZoneInfo.magAttr), magAttrStrGet(MEDIUM_MAG));
#endif
				pAnalyZone->ZoneInfo.magAttr = MEDIUM_MAG;
			}
			else
			{
#if WIN32_VC&GENERAL_DEBUG_PRT_
				TRACE("窗口%d,%d per%d %s 转为 %s\n", pAnalyZone->start, pAnalyZone->end, pAnalyZone->ZoneInfo.percent, magAttrStrGet(pAnalyZone->ZoneInfo.magAttr), magAttrStrGet(WEAK_MAG));
#endif
				pAnalyZone->ZoneInfo.magAttr = WEAK_MAG;
			}

		}
	}
}

void magImg_imgInfoSet(unsigned char *pTopData,int *top_x,int *top_y,int width,int height,int channel,int amount,int version,int dirction,wheelData_t *pWheel,int wheelLen,int ImgMosaicsLine)
{
	int i,j;

	for(i=0;i<4;i++)
	{
		imgInfo.top_x[i] = top_x[i];
		imgInfo.top_y[i] = top_y[i];
	}

	imgInfo.amount = amount;
	imgInfo.channel = channel;
	imgInfo.direction = dirction;
	imgInfo.height = height;
	imgInfo.version = version;
	imgInfo.width = width;
	imgInfo.adaptHeight = 270;
	imgInfo.adaptWidth = 720;
	imgInfo.affineWidth = 600;
	imgInfo.affineHeight = 250;
	imgInfo.widthStep = imgInfo.channel;
	imgInfo.ImgMosaicsLine = ImgMosaicsLine;
	if(magImgWheelStat(pWheel,wheelLen, ImgMosaicsLine))
	{
		GetAffineMat(&imgInfo);
		imgInfo.imgInitialize = true;
	}
}

void magImg_magInfoSet(ARAITRAT_PARAM *pMagInfo)
{
	if(imgInfo.imgInitialize == true)
	{
		imgInfo.pMagInfo = pMagInfo;
		imgInfo.tdnInitialize = true;
	}
}
//magimg location

static struct tagMagClass *newMagClass(wheelData_t wStart,wheelData_t wEnd)
{
	
	if(magClassCnt < magnet_class_max)
	{
		MagClass[magClassCnt].startW = wStart;
		MagClass[magClassCnt].endW = wEnd;
		MagClass[magClassCnt].peakNum = 1;
		MagClass[magClassCnt].coverZone = 0;
		MagClass[magClassCnt].next = NULL;
		magClassCnt++;
		return MagClass+magClassCnt-1;
	}
#if X86_X64_DEBUG
	qDebug("newMagClass failed\n");
#endif
	return NULL;
}

void mag_Peakfilter(MMSM_WHEEL_PARAM *pMagPeak,bool *peakFilter,int filteThred)
{
	int i;
	for(i=0;i<pMagPeak->PrecisionWheelLen;i+=2)
	{
		if(pMagPeak->PrecisionWheelBuf[i+1] - pMagPeak->PrecisionWheelBuf[i] < filteThred)
			peakFilter[i>>1] = false;
		else
			peakFilter[i>>1] = true;
	}

}

struct tagMagClass *mag_classify(MMSM_WHEEL_PARAM *pMagPeak,bool *peakFilter,int clusteringThred)
{

	struct tagMagClass *pHead,*pAnalyClass;
	int i;

	pHead = pAnalyClass = NULL;

	for(i=0;i<pMagPeak->PrecisionWheelLen;i+=2)
	{
		if(peakFilter[i>>1])
		{
			if(pAnalyClass != NULL)
			{
				if(pMagPeak->PrecisionWheelBuf[i] - pAnalyClass->endW <= clusteringThred)
				{
					pAnalyClass->endW = pMagPeak->PrecisionWheelBuf[i+1];
					pAnalyClass->peakNum++;
				}
				else
				{
					pAnalyClass->wheelLen = pAnalyClass->endW - pAnalyClass->startW + 1;
					if(pHead == NULL)
						pHead = pAnalyClass;
					else
					{
						pAnalyClass->next = pHead;
						pHead = pAnalyClass;
					}
					pAnalyClass = NULL;
				}
			}

			if(pAnalyClass == NULL)
			{
				pAnalyClass = newMagClass(pMagPeak->PrecisionWheelBuf[i],pMagPeak->PrecisionWheelBuf[i+1]);
				if(pAnalyClass == NULL)
					return NULL;
			}

		}
	}

	if(pAnalyClass == NULL)
		return pHead;

	pAnalyClass->wheelLen = pAnalyClass->endW - pAnalyClass->startW + 1;

	if(pHead == NULL)
		pHead = pAnalyClass;
	else
	{
		pAnalyClass->next = pHead;
		pHead = pAnalyClass;
	}

	return pHead;
}

static MMSM_WHEEL_PARAM *tdnPeakInfoGet(magSensorType_e sensorType){
	switch(sensorType)
	{
	case TDN_BM:
		return &AraitratParam.BM_WheelParam;
	case TDN_LM1:
		return &AraitratParam.LSM_TMR_WheelParam[0];
	case TDN_LM2:
		return &AraitratParam.LSM_TMR_WheelParam[1];
	case TDN_LM3:
		return &AraitratParam.LSM_TMR_WheelParam[2];
	case TDN_LM4:
		return &AraitratParam.LSM_TMR_WheelParam[3];
	case TDN_LM5:
		return &AraitratParam.LSM_TMR_WheelParam[4];
	case TDN_LM6:
		return &AraitratParam.LSM_TMR_WheelParam[5];
	case TDN_LM7:
		return &AraitratParam.LSM_TMR_WheelParam[6];
	case TDN_LM8:
		return &AraitratParam.LSM_TMR_WheelParam[7];

	case TDN_RM1:
		return &AraitratParam.RSM_TMR_WheelParam[0];
	case TDN_RM2:
		return &AraitratParam.RSM_TMR_WheelParam[1];
	case TDN_RM3:
		return &AraitratParam.RSM_TMR_WheelParam[2];
	case TDN_RM4:
		return &AraitratParam.RSM_TMR_WheelParam[3];
	case TDN_RM5:
		return &AraitratParam.RSM_TMR_WheelParam[4];
	case TDN_RM6:
		return &AraitratParam.RSM_TMR_WheelParam[5];
	case TDN_RM7:
		return &AraitratParam.RSM_TMR_WheelParam[6];
	case TDN_RM8:
		return &AraitratParam.RSM_TMR_WheelParam[7];
	
	case TDN_LMM:
		return &AraitratParam.LMM_WheelParam[0];
	case TDN_RMM:
		return &AraitratParam.RMM_WheelParam[0];
	case TDN_LMM1:
		return &AraitratParam.LMM_WheelParam[1];
	case TDN_RMM1:
		return &AraitratParam.RMM_WheelParam[1];

	default:
		break;
	}
	return NULL;
}

bool zoneCornerAlreadySet_Foreign(tagMagImg_Info *pImgInfo, struct tagMagZone_Foreign *pMagZone)
{
	switch (pMagZone->zone)
	{
	case CNY_ID_ALL:
		return (pImgInfo->idRegionInitialize&pImgInfo->IDCorner[0].valid&pImgInfo->IDCorner[9].valid);
	case CNY_ID_RED:
		return (pImgInfo->idRegionInitialize&pImgInfo->IDCorner[0].valid&pImgInfo->IDCorner[3].valid);
	case CNY_ID_BLACK:
		return (pImgInfo->idRegionInitialize&pImgInfo->IDCorner[4].valid&pImgInfo->IDCorner[9].valid);
	}

	return false;
}

static tagZoneCorner *AlreadySetCornetGet_Foreign(tagMagImg_Info *pImgInfo,struct tagMagZone_Foreign *pMagZone)
{
	static tagZoneCorner ZoneCorner;
	int i,temp;
	int *idTab,*cornerTab;
	int all_dir_1_2_id[4] = {9,0,0,9};
	int all_dir_0_3_id[4] = {0,9,9,0};

	int red_dir_1_2_id[4] = {3,0,0,3};
	int red_dir_0_3_id[4] = {0,3,3,0};

	int black_dir_1_2_id[4] = {9,4,4,9};
	int black_dir_0_3_id[4] = {4,9,9,4};

	int dir_pos_corner[4] = {0,1,2,3};
	int dir_neg_corner[4] = {1,0,3,2};

	switch(pMagZone->zone)
	{
	case CNY_ID_ALL:
		if(pImgInfo->direction == 1 || pImgInfo->direction == 2)
			idTab = all_dir_1_2_id;
		else
			idTab = all_dir_0_3_id;


		break;
	case CNY_ID_RED:
		if(pImgInfo->direction == 1 || pImgInfo->direction == 2)
			idTab = red_dir_1_2_id;
		else
			idTab = red_dir_0_3_id;

		break;
	case CNY_ID_BLACK:
		if(pImgInfo->direction == 1 || pImgInfo->direction == 2)
			idTab = black_dir_1_2_id;
		else
			idTab = black_dir_0_3_id;
			
		break;
	default:
		ZoneCorner.corner[0].x = 0;
		ZoneCorner.corner[0].y = 0;
		ZoneCorner.corner[1].x = 0;
		ZoneCorner.corner[1].y = 0;
		ZoneCorner.corner[2].x = 0;
		ZoneCorner.corner[2].y = 0;
		ZoneCorner.corner[3].x = 0;
		ZoneCorner.corner[3].y = 0;
		ZoneCorner.xMax = 0;
		ZoneCorner.xMin = 0;
		return &ZoneCorner;
	}

	if(pImgInfo->direction == 2 || pImgInfo->direction == 3)
		cornerTab = dir_neg_corner;
	else
		cornerTab = dir_pos_corner;

	for(i=0;i<4;i++)
	{
		if(pImgInfo->direction == 2 || pImgInfo->direction == 3)
		{
			ZoneCorner.corner[i].x = 720- pImgInfo->IDCorner[idTab[i]].corner[cornerTab[i]].x;
			ZoneCorner.corner[i].y = pImgInfo->IDCorner[idTab[i]].corner[cornerTab[i]].y;
		}
		else
		{
			ZoneCorner.corner[i].x = pImgInfo->IDCorner[idTab[i]].corner[cornerTab[i]].x;
			ZoneCorner.corner[i].y = pImgInfo->IDCorner[idTab[i]].corner[cornerTab[i]].y;
		}

	}


	ZoneCorner.xMax = -9999;
	ZoneCorner.xMin = 9999;
	for(i=0;i<4;i++)
	{
		if (ZoneCorner.corner[i].x > ZoneCorner.xMax)
			ZoneCorner.xMax = ZoneCorner.corner[i].x;
		if (ZoneCorner.corner[i].x < ZoneCorner.xMin)
			ZoneCorner.xMin = ZoneCorner.corner[i].x;
	}

	return &ZoneCorner;
	
}

static tagZoneCorner *magZoneCornerGet_Foreign(tagMagImg_Info *pImgInfo, struct tagMagZone_Foreign *pMagZone)
{
	static tagZoneCorner ZoneCorner;
	imgPoint cornerTemp, cornerDir, *pCorner;
	int *Mat = pImgInfo->AffineOpt;

	//if (zoneCornerAlreadySet_Foreign(pImgInfo, pMagZone))
		//return AlreadySetCornetGet_Foreign(pImgInfo, pMagZone);
	if (pImgInfo->direction == 0)
	{
		cornerDir.x = pMagZone->xStart;
		cornerDir.y = pImgInfo->adaptHeight - pMagZone->yStart - pMagZone->height;
	}
	else if (pImgInfo->direction == 1)
	{
		cornerDir.x = pImgInfo->adaptWidth - pMagZone->xStart - pMagZone->width;
		cornerDir.y = pMagZone->yStart;
	}
	else if (pImgInfo->direction == 2)
	{
		cornerDir.x = pImgInfo->adaptWidth - pMagZone->xStart - pMagZone->width;
		cornerDir.y = pImgInfo->adaptHeight - pMagZone->yStart - pMagZone->height;
	}
	else if (pImgInfo->direction == 3)
	{
		cornerDir.x = pMagZone->xStart;
		cornerDir.y = pMagZone->yStart;
	}
	else
	{
		cornerDir.x = pMagZone->xStart;
		cornerDir.y = pImgInfo->adaptHeight - pMagZone->yStart - pMagZone->height;
	}
	cornerTemp.x = cornerDir.x;
	cornerTemp.y = cornerDir.y;

	ZoneCorner.xMax = -9999;
	ZoneCorner.xMin = 9999;
	pCorner = ZoneCorner.corner + 0;
	pCorner->x = (cornerTemp.x * Mat[0] + cornerTemp.y * Mat[1] + Mat[2] + 32768) >> 16;
	pCorner->y = (cornerTemp.x * Mat[3] + cornerTemp.y * Mat[4] + Mat[5] + 32768) >> 16;
	if (pCorner->x > ZoneCorner.xMax)
		ZoneCorner.xMax = pCorner->x;
	if (pCorner->x < ZoneCorner.xMin)
		ZoneCorner.xMin = pCorner->x;

	cornerTemp.x = cornerDir.x + pMagZone->width;
	cornerTemp.y = cornerDir.y;
	pCorner = ZoneCorner.corner + 1;
	pCorner->x = (cornerTemp.x * Mat[0] + cornerTemp.y * Mat[1] + Mat[2] + 32768) >> 16;
	pCorner->y = (cornerTemp.x * Mat[3] + cornerTemp.y * Mat[4] + Mat[5] + 32768) >> 16;
	if (pCorner->x > ZoneCorner.xMax)
		ZoneCorner.xMax = pCorner->x;
	if (pCorner->x < ZoneCorner.xMin)
		ZoneCorner.xMin = pCorner->x;

	cornerTemp.x = cornerDir.x + pMagZone->width;
	cornerTemp.y = cornerDir.y + pMagZone->height;
	pCorner = ZoneCorner.corner + 2;
	pCorner->x = (cornerTemp.x * Mat[0] + cornerTemp.y * Mat[1] + Mat[2] + 32768) >> 16;
	pCorner->y = (cornerTemp.x * Mat[3] + cornerTemp.y * Mat[4] + Mat[5] + 32768) >> 16;
	if (pCorner->x > ZoneCorner.xMax)
		ZoneCorner.xMax = pCorner->x;
	if (pCorner->x < ZoneCorner.xMin)
		ZoneCorner.xMin = pCorner->x;

	cornerTemp.x = cornerDir.x;
	cornerTemp.y = cornerDir.y + pMagZone->height;
	pCorner = ZoneCorner.corner + 3;
	pCorner->x = (cornerTemp.x * Mat[0] + cornerTemp.y * Mat[1] + Mat[2] + 32768) >> 16;
	pCorner->y = (cornerTemp.x * Mat[3] + cornerTemp.y * Mat[4] + Mat[5] + 32768) >> 16;
	if (pCorner->x > ZoneCorner.xMax)
		ZoneCorner.xMax = pCorner->x;
	if (pCorner->x < ZoneCorner.xMin)
		ZoneCorner.xMin = pCorner->x;

	return &ZoneCorner;
}

static struct tagMagneticZone_Foreign *newMagneticZone()
{
	struct tagMagneticZone_Foreign *pZone;
	if (MagneticZoneCnt < 128)
	{
		pZone = MagneticZone_Foreign + MagneticZoneCnt;
		pZone->next = NULL;
		pZone->zone = 0;
		MagneticZoneCnt++;
		return pZone;
	}
	return NULL;
}

static struct tagMagneticZone_Foreign *saveNewZone(struct tagMagZone_Foreign *pSensorZone)
{

	pMagneticZone = newMagneticZone();
	pMagneticZone->attr = pSensorZone->attr;
	pMagneticZone->zone = pSensorZone->zone;

	return pMagneticZone;
}

int MagneticZoneCalc_Foreign(struct tagMagSensorZone_Foreign *pSensorZone, struct tagMagZone_Foreign *pMagZone, imgPoint *pCorner, int channel)
{
	int i, calcCornerCnt;
	imgPoint corner[4];
	imgPoint cornerCalc[4];
	int referenceLeft[4] = { 1, 2, 1, 2 };
	int referenceRight[4] = { 3, 0, 3, 0 };
	//struct tagMagneticZone_Foreign *pMagneticZone;
	float p, a, b, c;
#if X86_X64_DEBUG
	int infoLen = wheelDatLen;
#endif
	for (i = 0; i<4; i++)
	{
		if (pCorner[i].x >= pSensorZone->position && pCorner[i].x<pSensorZone->position + pSensorZone->range)
		{
			corner[i].x = pCorner[i].x;
			corner[i].y = pCorner[i].y;
		}
		else if (pCorner[i].x < pSensorZone->position)
		{
			if (pCorner[referenceLeft[i]].x < pSensorZone->position)
			{
#if X86_X64_DEBUG
				qDebug("magneticzonecalc:del the corner\n");
#endif
				corner[i].x = -1;
				corner[i].y = -1;
			}
			else
			{
				corner[i].x = pSensorZone->position;
				corner[i].y = ((corner[i].x - pCorner[referenceLeft[i]].x)*(pCorner[i].y - pCorner[referenceLeft[i]].y) /
					(pCorner[i].x - pCorner[referenceLeft[i]].x)) + pCorner[referenceLeft[i]].y;
			}
			//corner[i].y=pCorner[i].y-(pCorner[i].x - position)*slope;
		}
		else if (pCorner[i].x >= pSensorZone->position + pSensorZone->range)
		{
			if (pCorner[referenceRight[i]].x >= pSensorZone->position + pSensorZone->range)
			{
#if X86_X64_DEBUG
				qDebug("magneticzonecalc:del the corner\n");
#endif
				corner[i].x = -1;
				corner[i].y = -1;
			}
			else
			{
				corner[i].x = pSensorZone->position + pSensorZone->range;
				corner[i].y = ((corner[i].x - pCorner[referenceRight[i]].x)*(pCorner[i].y - pCorner[referenceRight[i]].y) /
					(pCorner[i].x - pCorner[referenceRight[i]].x)) + pCorner[referenceRight[i]].y;
			}
		}
#if X86_X64_DEBUG
		else
		{
			qDebug("magneticzonecalc:什么鬼情况\n");
		}
#endif
	}
	pMagneticZone = saveNewZone(pMagZone);
	//pMagneticZone->zone = pMagZone->zone;
	//pMagneticZone->attr = pMagZone->attr;
	pMagneticZone->sensorType = pSensorZone->type;

	calcCornerCnt = 0;
	pMagneticZone->start = 9999;
	pMagneticZone->end = -9999;

	for (i = 0; i<4; i++)
	{
		if (corner[i].x >= 0 && corner[i].y >= 0)
		{
			if (pMagneticZone->start > corner[i].y)
				pMagneticZone->start = corner[i].y;
			if (pMagneticZone->end < corner[i].y)
				pMagneticZone->end = corner[i].y;
			cornerCalc[calcCornerCnt].x = corner[i].x;
			cornerCalc[calcCornerCnt].y = corner[i].y;
			calcCornerCnt++;
		}
	}

	if (calcCornerCnt == 4)	//计算四边形
	{
		a = sqrt((float)(cornerCalc[0].x - cornerCalc[1].x)*(cornerCalc[0].x - cornerCalc[1].x) + (cornerCalc[0].y - cornerCalc[1].y)*(cornerCalc[0].y - cornerCalc[1].y));
		b = sqrt((float)(cornerCalc[0].x - cornerCalc[3].x)*(cornerCalc[0].x - cornerCalc[3].x) + (cornerCalc[0].y - cornerCalc[3].y)*(cornerCalc[0].y - cornerCalc[3].y));
		c = sqrt((float)(cornerCalc[3].x - cornerCalc[1].x)*(cornerCalc[3].x - cornerCalc[1].x) + (cornerCalc[3].y - cornerCalc[1].y)*(cornerCalc[3].y - cornerCalc[1].y));
		p = (a + b + c) / 2;
		pMagneticZone->area = (int)sqrt(p*(p - a)*(p - b)*(p - c));

		a = sqrt((float)(cornerCalc[2].x - cornerCalc[1].x)*(cornerCalc[2].x - cornerCalc[1].x) + (cornerCalc[2].y - cornerCalc[1].y)*(cornerCalc[2].y - cornerCalc[1].y));
		b = sqrt((float)(cornerCalc[2].x - cornerCalc[3].x)*(cornerCalc[2].x - cornerCalc[3].x) + (cornerCalc[2].y - cornerCalc[3].y)*(cornerCalc[2].y - cornerCalc[3].y));
		c = sqrt((float)(cornerCalc[3].x - cornerCalc[1].x)*(cornerCalc[3].x - cornerCalc[1].x) + (cornerCalc[3].y - cornerCalc[1].y)*(cornerCalc[3].y - cornerCalc[1].y));
		p = (a + b + c) / 2;
		pMagneticZone->area += (int)sqrt(p*(p - a)*(p - b)*(p - c));
		pMagneticZone->percent = pMagneticZone->area * 100 / (pSensorZone->range*(pMagneticZone->end - pMagneticZone->start + 1));
#if X86_X64_DEBUG
		if (pMagneticZone->start*channel >= infoLen || pMagneticZone->start < 0)
			qDebug("MagneticZoneCalc:start%d no wheel\n", pMagneticZone->start);
		if (pMagneticZone->end*channel >= infoLen || pMagneticZone->end < 0)
			qDebug("MagneticZoneCalc:end%d no wheel\n", pMagneticZone->end);
#endif
		//pMagneticZone->start = pImgInfo[pMagneticZone->start*4];
		//pMagneticZone->end = pImgInfo[pMagneticZone->end*4];
	}
	else if (calcCornerCnt == 3)	//计算三角形
	{
		a = sqrt((float)(cornerCalc[0].x - cornerCalc[1].x)*(cornerCalc[0].x - cornerCalc[1].x) + (cornerCalc[0].y - cornerCalc[1].y)*(cornerCalc[0].y - cornerCalc[1].y));
		b = sqrt((float)(cornerCalc[0].x - cornerCalc[2].x)*(cornerCalc[0].x - cornerCalc[2].x) + (cornerCalc[0].y - cornerCalc[2].y)*(cornerCalc[0].y - cornerCalc[2].y));
		c = sqrt((float)(cornerCalc[2].x - cornerCalc[1].x)*(cornerCalc[2].x - cornerCalc[1].x) + (cornerCalc[2].y - cornerCalc[1].y)*(cornerCalc[2].y - cornerCalc[1].y));
		p = (a + b + c) / 2;
		pMagneticZone->area = (int)sqrt(p*(p - a)*(p - b)*(p - c));
		pMagneticZone->percent = pMagneticZone->area * 100 / (pSensorZone->range*(pMagneticZone->end - pMagneticZone->start + 1));
#if X86_X64_DEBUG&PRINT_RES
		if (pMagneticZone->start*channel >= infoLen || pMagneticZone->start < 0)
			TRACE("MagneticZoneCalc:start%d no wheel\n", pMagneticZone->start);
		if (pMagneticZone->end*channel >= infoLen || pMagneticZone->end < 0)
			TRACE("MagneticZoneCalc:end%d no wheel\n", pMagneticZone->end);
#endif
		//pMagneticZone->start = pImgInfo[pMagneticZone->start*4];
		//pMagneticZone->end = pImgInfo[pMagneticZone->end*4];
	}
	else
	{
#if X86_X64_DEBUG&PRINT_RES
		TRACE("MagneticZoneCalc:面积不可计算\n");
#endif
	}
	//	drawMagZoneOverlap(cornerCalc,calcCornerCnt);
	return 0;
}

#define MAG_IMG_ALINE_LINE	14
static tagMagAnalyZone_Foreign *magImgZoneScoring_Foreign(struct tagMagneticZone_Foreign *pMagneticZone, struct tagMagClass *pMagClass, int analysisPosi)
{
	//struct tagMagClass *pClass;
	//int coverStart, coverEnd, align, magCoverZone, score, percent, distance;
	//bool match = false;
	//if (pSensorZone->pMagClass == NULL)
	//{
	//	pStat->coverageRate = 0;
	//}
	//else
	//{
	//	pStat->coverageZone = 0;
	//	pStat->statStart = false;
	//	for (pClass = pSensorZone->pMagClass; pClass != NULL; pClass = pClass->next)
	//	{
	//		if (pClass->endimg + MAG_IMG_ALINE_LINE < pMagneticZone->start || pClass->startimg - MAG_IMG_ALINE_LINE > pMagneticZone->end)
	//			continue;
	//		if (abs(pMagneticZone->start - pClass->startimg) <= MAG_IMG_ALINE_LINE)
	//			align = pMagneticZone->start - pClass->startimg;
	//		else if (abs(pMagneticZone->end - pClass->endimg) <= MAG_IMG_ALINE_LINE)
	//			align = pMagneticZone->end - pClass->endimg;
	//		else if (pMagneticZone->start > pClass->startimg)
	//			align = MAG_IMG_ALINE_LINE;
	//		else
	//			align = -MAG_IMG_ALINE_LINE;
	//		coverStart = pClass->startimg + align > pMagneticZone->start ? pClass->startimg + align : pMagneticZone->start;
	//		coverEnd = pClass->endimg + align < pMagneticZone->end ? pClass->endimg + align : pMagneticZone->end;
	//		distance = 0;
	//		if (pStat->statStart)
	//		{
	//			if (pStat->coverStart <= coverEnd)
	//				distance = 0;
	//			else
	//				distance = pStat->coverStart - coverEnd - 1;
	//			if (pStat->coverStart > coverStart)
	//				pStat->coverStart = coverStart;
	//			if (pStat->coverEnd < coverEnd)
	//				pStat->coverEnd = coverEnd;
	//		}
	//		else
	//		{
	//			pStat->statStart = true;
	//			pStat->coverStart = coverStart;
	//			pStat->coverEnd = coverEnd;
	//		}
	//		/* 需要连接区域*/
	//		if (distance < 30)
	//			pStat->coverageZone += coverEnd - coverStart + distance + 1;
	//		else
	//		pStat->coverageZone += coverEnd - coverStart + 1;
	//		magCoverZone = pStat->coverEnd - pStat->coverStart + 1;
	//		if (magCoverZone > pClass->coverZone)
	//			pClass->coverZone = magCoverZone;
	//		match = true;
	//	}
	//	if (match)
	//		//pStat->coverageRate = (pStat->coverEnd-pStat->coverStart+1)*100/(pMagneticZone->end-pMagneticZone->start+1);
	//		pStat->coverageRate = pStat->coverageZone * 100 / (pMagneticZone->end - pMagneticZone->start + 1);
	//	else
	//		pStat->coverageRate = 0;
	//}
	//pStat->coverageRate = pStat->coverageRate >100 ? 100 : pStat->coverageRate;

	static tagMagAnalyZone_Foreign MagAnalyZone;

	MagAnalyZone.SignalInfo.magAttr = NONE_MAG;
	MagAnalyZone.ZoneInfo.magAttr = NONE_MAG;
	MagAnalyZone.pMagClass = pMagClass;					 //磁
	MagAnalyZone.pMagneticZone = pMagneticZone; //窗口
	MagAnalyZone.end = analysisPosi;

	if (pMagClass == NULL)			//有区域无磁
	{
		MagAnalyZone.start = pMagneticZone->start;
		MagAnalyZone.ZoneInfo.magAttr = pMagneticZone->attr;
		if (pMagneticZone->attr != NONE_MAG)
		{
			MagAnalyZone.ZoneInfo.percent = pMagneticZone->percent;
			MagAnalyZone.ZoneInfo.area = pMagneticZone->area;
			MagAnalyZone.ZoneInfo.windowPer = 100;
		}
	}
	else if (pMagneticZone == NULL)	//有磁，无区域
	{
#if WIN32_VC&GENERAL_DEBUG_PRT
		TRACE("区域选取:有磁，无区域\n");
#endif
		MagAnalyZone.start = pMagClass->startimg;		//pMagClass->startW+pMagClass->wheelLen;
	}
	//else if (pMagClass->startimg <= analysisPosi)		//窗口与磁交汇
	else if ((pMagClass->startimg < pMagneticZone->start && pMagClass->endimg > pMagneticZone->start)
	|| (pMagClass->startimg < pMagneticZone->end && pMagClass->endimg > pMagneticZone->end)
	/*|| (pMagClass->startimg < pMagneticZone->start && pMagClass->endimg > pMagneticZone->end)*/)					//窗口与磁交汇 有磁在窗口外,磁完全包含窗口
	{
		//if (pMagClass->startimg <= pMagneticZone->end  && pMagClass->endimg >= pMagneticZone->start)	//定位区域内
		//{
			MagAnalyZone.start = pMagneticZone->start < pMagClass->startimg ? pMagClass->startimg : pMagneticZone->start;
			MagAnalyZone.end = pMagneticZone->end < pMagClass->endimg ? pMagneticZone->end : pMagClass->endimg;
			MagAnalyZone.ZoneInfo.magAttr = pMagneticZone->attr;
			if (pMagneticZone->attr != NONE_MAG)
			{
				MagAnalyZone.ZoneInfo.percent = pMagneticZone->percent;
				MagAnalyZone.ZoneInfo.windowPer = (MagAnalyZone.end - MagAnalyZone.start + 1) / (pMagneticZone->end - pMagneticZone->start + 1);
				MagAnalyZone.ZoneInfo.area = pMagneticZone->area * MagAnalyZone.ZoneInfo.windowPer / 100;
			}
			MagAnalyZone.SignalInfo.magAttr = STRONG_MAG;
			//	MagAnalyZone.SignalInfo.energy = pMagClass->energy;
		//}
//		else
//		{
//#if WIN32_VC&GENERAL_DEBUG_PRT
//			TRACE("区域选取:定位区域问题1\n");
//#endif
//			if (pMagneticZone->start < pMagClass->endimg)
//				MagAnalyZone.end = pMagneticZone->end;
//			else
//				MagAnalyZone.end = pMagClass->endimg;
//			MagAnalyZone.ZoneInfo.magAttr = NONE_MAG;
//			MagAnalyZone.pMagneticZone = NULL;
//			MagAnalyZone.SignalInfo.magAttr = STRONG_MAG;
//			//	MagAnalyZone.SignalInfo.energy = pMagClass->energy;
//		}
	}
	else if (pMagClass->startimg > pMagneticZone->start && pMagClass->endimg < pMagneticZone->end)		//窗口与磁交汇  磁完全再窗口内
	{
		MagAnalyZone.start = pMagneticZone->start < pMagClass->startimg ? pMagClass->startimg : pMagneticZone->start;
		MagAnalyZone.end = pMagneticZone->end < pMagClass->endimg ? pMagneticZone->end : pMagClass->endimg;
		MagAnalyZone.ZoneInfo.magAttr = pMagneticZone->attr;
		if (pMagneticZone->attr != NONE_MAG)
		{
			MagAnalyZone.ZoneInfo.percent = pMagneticZone->percent;
			MagAnalyZone.ZoneInfo.windowPer = (MagAnalyZone.end - MagAnalyZone.start + 1) / (pMagneticZone->end - pMagneticZone->start + 1);
			MagAnalyZone.ZoneInfo.area = pMagneticZone->area * MagAnalyZone.ZoneInfo.windowPer / 100;
		}
		MagAnalyZone.SignalInfo.magAttr = STRONG_MAG;
	}
	else											//窗口与图无交汇
	{
		//if (pMagneticZone->start > analysisPosi)	//窗口与图无交汇
		//{
#if WIN32_VC&GENERAL_DEBUG_PRT
		TRACE("区域选取:定位区域问题2\n");
#endif
		MagAnalyZone.start = pMagneticZone->start;
		MagAnalyZone.ZoneInfo.magAttr = NONE_MAG;
		MagAnalyZone.SignalInfo.magAttr = NONE_MAG;
		//MagAnalyZone.SignalInfo.energy = 0;
		MagAnalyZone.pMagClass = NULL;
		MagAnalyZone.pMagneticZone = NULL;
		//}
		//else
		//{
		//	if (pMagneticZone->end >= pMagClass->startimg)
		//	{
		//		MagAnalyZone.end = pMagClass->startimg - 1;
		//	}
		//	else
		//	{
		//		MagAnalyZone.end = pMagneticZone->end;
		//	}
		//	MagAnalyZone.SignalInfo.magAttr = NONE_MAG;
		//	MagAnalyZone.pMagClass = NULL;
		//	MagAnalyZone.ZoneInfo.magAttr = pMagneticZone->attr;
		//	if (pMagneticZone->attr != NONE_MAG)
		//	{
		//		MagAnalyZone.ZoneInfo.percent = pMagneticZone->percent;
		//		MagAnalyZone.ZoneInfo.area = pMagneticZone->area;
		//		MagAnalyZone.ZoneInfo.windowPer = 100;
		//	}
		//}
	}
	


	return &MagAnalyZone;



#if X86_X64_DEBUG&PRINT_RES
	score = pStat->coverageRate * pMagneticZone->percent / 100;

	TRACE("ZoneScoring:%s zone:%s cover%d score%d\n", magSensorNameGet(pSensorZone->type), magZoneNameGet(pMagneticZone->zone), pStat->coverageRate, score);
#endif
}

static inline void resInit_Foreign(struct tagMagImgRes_Foreign *pRes, magSensorType_e type)
{
	memset(pRes, 0, sizeof(struct tagMagImgRes_Foreign));
	pRes->sensor = type;
}

struct tagNoteMagZone_Foreign *NoteMagZoneGet_Foreign(int amunt, int ver)
{
	struct tagNoteMagZone_Foreign *pNote = NULL;
	/*for (pNote = USD_NoteMagZone; pNote != NULL; pNote = pNote->next)
	{
		if (pNote->amount == amunt && pNote->version == ver)
			break;
	}*/
	return pNote;
}

//Foreign
#define MAG_IMG_DEBUG  0	//白纸验证开关
struct tagMagImgRes_Foreign * magImgPro_Foreign()
{
	struct tagMagSensorZone_Foreign *pSensorZone;
	struct tagNoteMagZone_Foreign *pNoteMagZone;
	struct tagMagZone_Foreign *pMagZone;
	struct tagMagneticZone_Foreign *MagZone_p,ID_redZone, ID_balckZone;
	tagMagAnalyZone_Foreign *pMagImgAnalyZone;
	MMSM_WHEEL_PARAM *pPeakInfo;
	struct tagMagClass *pClass, *MagClass_p;
	struct tagMagImgRes_Foreign *pMagRes = NULL, *pMagImgResLast, *pCheckRes;
	tagMagZoneStat MagZoneStat;
	tagZoneCorner *pCorner;
	struct magWindowList WinListHead , *pWINlist;
	int i, score;
	MagneticZoneCnt = 0;

	pMagRes = NULL;
	if (!imgInfo.imgInitialize || !imgInfo.tdnInitialize)
		return pMagRes;
	//magArbitConfig(CURRENCY_TYPE_USD);

#if MAG_IMG_DEBUG
	pNoteMagZone = NoteMagZoneGet_Foreign(imgInfo.amount, imgInfo.version);		//标记磁区域，选币种的 面额和版本
	if (pNoteMagZone == NULL)
		return pMagRes;
#endif

	magClassCnt = 0;
	magImgResCnt = 0;

#if X86_X64_DEBUG&MAGIMG_DEBUG_TOOL
	IplImage *pImg = NULL;
	IplImage *imgSrc = imgInfo.pImg_IR;
	if (imgSrc != NULL)
	{
		pImg = cvCreateImage(cvSize(imgInfo.adaptWidth, imgInfo.height), IPL_DEPTH_8U, 3);
		for (int i = 0; i<imgSrc->height; i++)
		{
			for (int j = 0; j<imgSrc->width; j++)
			{
				pImg->imageData[i*pImg->widthStep + j * 3] = imgSrc->imageData[i*imgSrc->widthStep + j];
				pImg->imageData[i*pImg->widthStep + j * 3 + 1] = imgSrc->imageData[i*imgSrc->widthStep + j];
				pImg->imageData[i*pImg->widthStep + j * 3 + 2] = imgSrc->imageData[i*imgSrc->widthStep + j];

			}
		}
		drawMagSensor_Foreign(imgSrc, pImg, gMagSensorZone_Foreign);		//画线
	}
#endif

	for (pSensorZone = gMagSensorZone_Foreign; pSensorZone != NULL; pSensorZone = pSensorZone->next)
	{
		int CntFlg = 0;
		pSensorZone->pMagClass = NULL;
		pSensorZone->pRes = NULL;
		pPeakInfo = tdnPeakInfoGet(pSensorZone->type);
		if (pPeakInfo == NULL)
			continue;
		mag_Peakfilter(pPeakInfo, peakFilter, 1);
		pSensorZone->pMagClass = mag_classify(pPeakInfo, peakFilter, 80);
		for (pClass = pSensorZone->pMagClass; pClass != NULL; pClass = pClass->next)
		{
			pClass->startimg = transWheel2Img(pPeakInfo->WheelOffset, pClass->startW, 16, pSensorZone->offset, 2, imgInfo.height);
			pClass->endimg = transWheel2Img(pPeakInfo->WheelOffset, pClass->endW, 16, pSensorZone->offset, 2, imgInfo.height);
			if (CntFlg == 0)
			{
				pSensorZone->lastEnd = pClass->endimg;
				CntFlg = 1;
			}
			if (pClass->next == NULL)
				pSensorZone->firstStart = pClass->startimg;
		}
	}
#if X86_X64_DEBUG&MAGIMG_DEBUG_TOOL
	drawMagImg(pImg);	//画主控有磁区域
#endif

#if MAG_IMG_DEBUG
	//循环区域
	for (pMagZone = pNoteMagZone->pMagZone; pMagZone != NULL; pMagZone = pMagZone->next)
	{
		MagZoneStat.statStart = false;
		WinListHead.next = &WinListHead;
		WinListHead.prev = &WinListHead;
		WinListHead.type = ignoreMagnetic;
		//循环传感器
		for (pSensorZone = gMagSensorZone_Foreign; pSensorZone != NULL; pSensorZone = pSensorZone->next)
		{
			//if(pSensorZone->pMagClass == NULL)
			//	continue;
			if (pSensorZone->Arrt == MAG_MATH_LOCATION)
				continue;
			//if(zoneCornerAlreadySet(&imgInfo, pMagZone))
			//	pCorner = AlreadySetCornetGet(&imgInfo, pMagZone);
			//else
			pCorner = magZoneCornerGet_Foreign(&imgInfo, pMagZone);
#if X86_X64_DEBUG&MAGIMG_DEBUG_TOOL
			drawMagZone(pImg, pCorner->corner);		// draw Mag from Img Mark
#endif

			//标定的应有磁区域 扫过了哪些 对应区域的传感器
			if (pSensorZone->position < pCorner->xMax && pSensorZone->position + pSensorZone->range > pCorner->xMin)
			{
				MagneticZoneCalc_Foreign(pSensorZone, pMagZone,pCorner->corner, imgInfo.channel);

				if (pMagneticZone->percent > 0)
				{
					MagClass_p = pSensorZone->pMagClass;	//磁
					MagZone_p = pMagneticZone;					//窗口
					int AnalysisPosi = MagZone_p->end;
					WinCnt = 0;
					
					//循环有磁分布的区域
					while (MagClass_p != NULL && MagZone_p != NULL)
					{
						AnalysisPosi = MagZone_p->end;

						pMagImgAnalyZone = magImgZoneScoring_Foreign(MagZone_p, MagClass_p, AnalysisPosi);

						magZoneAttrJudge(pMagImgAnalyZone);

						if (pMagImgAnalyZone->ZoneInfo.magAttr != NONE_MAG || pMagImgAnalyZone->SignalInfo.magAttr != NONE_MAG)
						{
							pWINlist = newWinList();
							if (pWINlist != NULL)
							{
								memcpy(&pWINlist->pMagImgAnalyZone, pMagImgAnalyZone, sizeof(tagMagAnalyZone_Foreign));
								magWindosCheck(pWINlist);
								WinListSave(&WinListHead, pWINlist);
							}
						}

						AnalysisPosi = MagClass_p->endimg - 1;
						if (MagClass_p != NULL && AnalysisPosi < MagClass_p->endimg)
						{
							MagClass_p = MagClass_p->next;
						}

						if (MagZone_p != NULL && AnalysisPosi < MagZone_p->end)
						{
							MagZone_p = MagZone_p->next;
						}
					}
				}
			}
		}

			//根据上下文弱判
			for (pWINlist = WinListHead.next; pWINlist != &WinListHead; pWINlist = pWINlist->next)
			{
				/*if (pWINlist->type == InvalidMagnetic)
					magSingleWeakenJudgment(&WinListHead, pWINlist, gMagArbitPara.winMoveZone, pSensorLoaction->start, pSensorLoaction->end);
					else if (pWINlist->type == nonmagnetic)
					magZoneWeakenJudgment(&WinListHead, pWINlist, gMagArbitPara.winMoveZone, pSensorLoaction->start, pSensorLoaction->end);*/

				if (pMagRes == NULL
					|| pWINlist->pMagImgAnalyZone.ZoneInfo.magAttr != pCheckRes->SignalInfo.magAttr
					|| pCheckRes->zone != pWINlist->pMagImgAnalyZone.pMagneticZone->zone)
				{
					pCheckRes = newMagImgRes();
					if (pMagRes == NULL)
						pMagRes = pCheckRes;
					else
						pMagImgResLast->next = pCheckRes;
					pMagImgResLast = pCheckRes;

					pCheckRes->magAttr = pWINlist->pMagImgAnalyZone.ZoneInfo.magAttr;
					if (pWINlist->pMagImgAnalyZone.pMagneticZone != NULL)
						pCheckRes->zone = pWINlist->pMagImgAnalyZone.pMagneticZone->zone;
					pCheckRes->start = pWINlist->pMagImgAnalyZone.start;
					pCheckRes->sensor = pWINlist->pMagImgAnalyZone.pMagneticZone->sensorType;
				}
				magZoneScoreCalc(pCheckRes, pWINlist);
			}
 

	}
#endif

#if X86_X64_DEBUG&MAGIMG_DEBUG_TOOL
	if (pImg != NULL)
	{
		char imgCheckPic[256];
		if (imgInfo.ImgMosaicsLine>0)
			cvLine(pImg, cvPoint(0, imgInfo.ImgMosaicsLine), cvPoint(719, imgInfo.ImgMosaicsLine), CV_RGB(255, 0, 0), 2);

		sprintf(imgCheckPic, "imgRes\\magImg\\magZone_%d.bmp", g_FileNum);
		cvSaveImage(imgCheckPic, pImg);

		const char* ImgResPath;
		QString ImgPath = QCoreApplication::applicationDirPath();
		ImgPath.append("/imgRes/magImg");
		QDir ImgResDir(ImgPath);
		if (!ImgResDir.exists())
			ImgResDir.mkpath(ImgPath);
		ImgPath.append(QString("/magZone_%1.bmp").arg(g_FileNum));
		QByteArray ba = ImgPath.toLocal8Bit().data();
		ImgResPath = ba.data();
		cvSaveImage(ImgResPath, pImg);

		cvReleaseImage(&pImg);
	}
	if (imgInfo.pImg_IR != NULL)
		cvReleaseImage(&imgInfo.pImg_IR);
#endif

	return pMagRes;
}



void magImg_reset()
{
	memset(&imgInfo,0,sizeof(imgInfo));
	imgInfo.idRegionInitialize = imgInfo.imgInitialize = imgInfo.tdnInitialize = false;
	//printf("magImg_reset :img: %d, master: %d  \n", imgInfo.imgInitialize,imgInfo.tdnInitialize);

}

int MagImg_imgPro(unsigned char *pTopData,int *top_x,int *top_y,int width,int height,int channel,int amount,int version,int dirction,unsigned int *pWheel,int wheelLen,int ImgMosaicsLine,Sensitive *Sensitive_p)
{

	if (pTopData == NULL)
		return 0;
	
	EDGE_DETECTION_INT(pWheel[0],pWheel[wheelLen - 1]);
	
	//if(ABS(pWheel[0],pWheel[wheelLen - 1]) < 20)
		//return 0;
	

	magImg_imgInfoSet(pTopData,top_x,top_y,width,height,channel,amount,version,dirction,pWheel,wheelLen, ImgMosaicsLine);
	
#ifdef MAG_IMG_DEBUG
	magImg_debugSet(pTopData,width,height,channel);
#endif



	return 0;
}

int MagImg_masterPro()
{
	int ErrType = 0;

	magImg_magInfoSet(&AraitratParam);
		
	if (!imgInfo.imgInitialize || !imgInfo.tdnInitialize)
		return 0;
		
	struct tagMagImgRes_Foreign *pMagRes = magImgPro_Foreign();

	if(pMagRes == NULL)
	{
		return 0;
	}
		
	//ErrType = MagImgArbitration_Foreign(pMagRes, &SensitiveParam);


#ifdef WIN32_
		FILE *myfile1;
		fopen_s(&myfile1, "imgRes\\magImg\\Result.txt", "ab+");
		fprintf(myfile1, "面额: %d  版本: %d  方向: %d  报错代码: %d\n", imgInfo.amount,imgInfo.version,imgInfo.direction,ErrType);
		fclose(myfile1);
		//MagImgRes_print(pMagRes);
		//magImgRes_stat(pMagRes);
		MagImgFree();
#endif

	return ErrType;
}