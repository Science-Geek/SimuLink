/***************************************************************
* Copyright (C) 2011, Wuhan University
* All rights reserved.
*
* 文件名：	ImageDCT.h
* 功能描述: 图像面额版本方向识别头文件
*
* 当前版本：1.0
* 作    者：张维夏
* 完成日期：2016年9月20日
*
***************************************************************/
#ifndef EXCLUDE_H_
#define EXCLUDE_H_



#include "Common.h"



#define HM_W_H_H 16
#define HM_W_H_W 16
#define HM_DIMENSION (HM_W_H_H*HM_W_H_W*2) //正反面的
#define IMGCUR_ERRORTYPE_NORMAL 0
#define IMGCUR_ERRORTYPE_OTHER 286
#define IMGCUR_ERRORTYPE_FOUR 284 
#define HM_TEMPLATE_NUM 300
#define AFFINE_HEIGHT 48
#define AFFINE_WIDTH 192

#define SAMPLE_NUM 100
#define EXCLUDE_THRESHOLD 9000

#define TEMPLATE_PATH "imageAlgorithm\\ValueCheckAlg\\templateBin\\DSPtempV5.8.7.4.bin"

typedef struct tagCashVVD
{
	int value;     //面额
	int version;   //版本
	int direction; //方向
}CashVVD;

typedef struct tagCashVVD_AT
{
	CashVVD	cash;			//
	int		unsimilary;		//不相似度
}CashVVD_AT;	

//汉明模板//
typedef struct tagDCTHanMingCode
{
	char TemplateCurName[CURR_NAME_LEN];  //当前模板所属币种
	int value;     //面额
	int version;   //版本
	int direction; //方向
	unsigned char aHanMing[HM_DIMENSION];
}DCTHanMingCode;

typedef struct tagDCTHanMingPattern
{
	DCTHanMingCode	*pattern;	//模板
	int				validnum;	//有效模板数量
}DCTHanMingPattern;


//DCT变换子程序
//
void ImageDCT(int * _pTDCT_res, IplImage_ *_pImg, short *_pDCTpara, short *_pTDCTpara);

void ImageDCT_float(float * pTDCT_res, IplImage_ * pImg_HM, float * pDCTpara, float * pTDCTpara);//DCT变换子程序

//获取汉明码
int GetHMCode(unsigned char *_pHMCode, int *_pTDCT_info, int _width);

int GetHMCode_float(unsigned char *_pHMCode, float *_pTDCT_info, int _width);

//模板匹配
void ImgDCTParttenMatch(CashVVD_AT *_pSimilary, DCTHanMingPattern *_pHMPatten, DCTHanMingCode *_pHM);

//给定面额下的模板匹配
void ImgDCTMatchWithValue(CashVVD_AT *_pSimilary, DCTHanMingPattern *_pHMPatten, DCTHanMingCode *_pHM, int value);

int GetFeatureInfo(DCTHanMingCode *_pHM, IplImage_* pImg_HM_bottom, IplImage_* pImg_HM_up);

extern DCTHanMingCode gExincludeTemplate[MAX_CONFIG_NUM * 4];

//版本补刀函数
void VersionRecheck(PicDataInfo_ *_pic_info);

//刹车后版本补刀
void VersionRecheck_ShaChe(PicDataInfo_ *_pic_info);

//100,50的版本补刀
int VersionDect(PicDataInfo_ *_pic_info, IplImage_ *pImg_src, int _method);

//20,10,5的版本补刀
int VersionDect2(PicDataInfo_ *_pic_info, IplImage_ *pImg_src1, IplImage_ *pImg_src2);

//BUK版本补刀
int VersionDect_BUK(PicDataInfo_ *_pic_info, IplImage_ *pImg_src);

//版本补刀抠图函数
int VVDGetZoneCoordinates(RotateZoneInfo_ *zone_info, PicDataInfo_ *_pic_info);

int ValueVersionDirectionMain(PicDataInfo_ *_pic_info, IplImage_ *img_up, IplImage_ *img_bottom);

//大津法判版本
int Binaryzation_VR(IplImage_ _image_new_bin, IplImage_ _image_source, double _offset, int _enable);

#endif
