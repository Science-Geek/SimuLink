/***************************************************************
* Copyright (C) 2011, Wuhan University
* All rights reserved.
*
* �ļ�����	ImageDCT.h
* ��������: ͼ�����汾����ʶ��ͷ�ļ�
*
* ��ǰ�汾��1.0
* ��    �ߣ���ά��
* ������ڣ�2016��9��20��
*
***************************************************************/
#ifndef EXCLUDE_H_
#define EXCLUDE_H_



#include "Common.h"



#define HM_W_H_H 16
#define HM_W_H_W 16
#define HM_DIMENSION (HM_W_H_H*HM_W_H_W*2) //�������
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
	int value;     //���
	int version;   //�汾
	int direction; //����
}CashVVD;

typedef struct tagCashVVD_AT
{
	CashVVD	cash;			//
	int		unsimilary;		//�����ƶ�
}CashVVD_AT;	

//����ģ��//
typedef struct tagDCTHanMingCode
{
	char TemplateCurName[CURR_NAME_LEN];  //��ǰģ����������
	int value;     //���
	int version;   //�汾
	int direction; //����
	unsigned char aHanMing[HM_DIMENSION];
}DCTHanMingCode;

typedef struct tagDCTHanMingPattern
{
	DCTHanMingCode	*pattern;	//ģ��
	int				validnum;	//��Чģ������
}DCTHanMingPattern;


//DCT�任�ӳ���
//
void ImageDCT(int * _pTDCT_res, IplImage_ *_pImg, short *_pDCTpara, short *_pTDCTpara);

void ImageDCT_float(float * pTDCT_res, IplImage_ * pImg_HM, float * pDCTpara, float * pTDCTpara);//DCT�任�ӳ���

//��ȡ������
int GetHMCode(unsigned char *_pHMCode, int *_pTDCT_info, int _width);

int GetHMCode_float(unsigned char *_pHMCode, float *_pTDCT_info, int _width);

//ģ��ƥ��
void ImgDCTParttenMatch(CashVVD_AT *_pSimilary, DCTHanMingPattern *_pHMPatten, DCTHanMingCode *_pHM);

//��������µ�ģ��ƥ��
void ImgDCTMatchWithValue(CashVVD_AT *_pSimilary, DCTHanMingPattern *_pHMPatten, DCTHanMingCode *_pHM, int value);

int GetFeatureInfo(DCTHanMingCode *_pHM, IplImage_* pImg_HM_bottom, IplImage_* pImg_HM_up);

extern DCTHanMingCode gExincludeTemplate[MAX_CONFIG_NUM * 4];

//�汾��������
void VersionRecheck(PicDataInfo_ *_pic_info);

//ɲ����汾����
void VersionRecheck_ShaChe(PicDataInfo_ *_pic_info);

//100,50�İ汾����
int VersionDect(PicDataInfo_ *_pic_info, IplImage_ *pImg_src, int _method);

//20,10,5�İ汾����
int VersionDect2(PicDataInfo_ *_pic_info, IplImage_ *pImg_src1, IplImage_ *pImg_src2);

//BUK�汾����
int VersionDect_BUK(PicDataInfo_ *_pic_info, IplImage_ *pImg_src);

//�汾������ͼ����
int VVDGetZoneCoordinates(RotateZoneInfo_ *zone_info, PicDataInfo_ *_pic_info);

int ValueVersionDirectionMain(PicDataInfo_ *_pic_info, IplImage_ *img_up, IplImage_ *img_bottom);

//����а汾
int Binaryzation_VR(IplImage_ _image_new_bin, IplImage_ _image_source, double _offset, int _enable);

#endif
