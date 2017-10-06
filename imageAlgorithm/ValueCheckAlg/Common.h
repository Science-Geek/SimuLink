/***************************************************************
* Copyright (C) 2011, Wuhan University
* All rights reserved.
*
* �ļ�����	Common.h
* ��������: ��������ͷ�ļ�
*
* ��ǰ�汾��1.0
* ��    �ߣ���ά��
* ������ڣ�2016��9��20��
*
***************************************************************/
#ifndef COMMON_H_
#define COMMON_H_


#include "Recognition.h"


#define RET_ERROR -1
#define RET_OK 0
#define CV_IMAGE_ELEM(image, elemtype, row, col) (((elemtype*)((image)->imageData + (image)->widthStep*(row)))[(col)])
#define uchar unsigned char

#define LUT_LEN_HUE 32
#define PI 3.1415926
#define MAX_PRO_VER 30         //��Ԥ�ڵ�������汾��Ϣ��Ϊ�Ժ������ã���ͬ�������ļ���Ӧ��ͬ�ĳ���汾����
#define CURR_PRO_VER 20140601    //��ǰ�ĳ���汾��Ϣ����ʱ�䵥λ
#define MAX_CURRENCY_NUM  32  //֧�ֵ���������
#define CUR_CURRENCY_NUM  6    //��ǰ�ı�����
#define MAX_CONFIG_NUM  60     //֧��ͬһ�������������ļ���
#define CURR_NAME_LEN 16      //����������


//CISͨ��������˳��
typedef enum  CIS_ChanType_
{
	G_IR = 0,     //G+IR
	IR_G_IR,    //IR+G+IR
	B_R_G,      //B+R+G
	IR_B_R_G    //IR+B+R+G


}CIS_ChanType_;

//��Ҽ��ʱ���õķ�����֧�ֲ�ͬ�ı��ֲ��ò�ͬ�ķ���
typedef enum  MethodType_
{
	METHOD1 = 0,  //����1
	METHOD2,    //����2
	METHOD3,    //����3
	METHOD4     //����4
}MethodType_;

typedef enum  CurrencyType_
{
	CNY = 0,  	//�����
	TWD,    	//̨��
	HKD,   		//�۱�
	USD,    	//��Ԫ
	EURO,   	//ŷԪ
	JPY,     	//��Ԫ
	GBP,     	//Ӣ��
	AUD,     	//��Ԫ
	CAD,		//���ô��
	SGD,		//�¼��±�
	CHF,		//��ʿ����
	RUB,		//¬��
	MOP,		//���ű�
	KRW,		//��Ԫ
	PHP,		//���������
	ZAR,		//�Ϸ�����
	ETB,		//��������Ǳȶ�
	SAR,		//ɳ�����Ƕ�
	PGK,		//�Ͳ����¼����ǻ���
	THB,		//̩��̩��
	NZD,		//������Ԫ
	DKK,		//�������
	NOK,		//Ų������
	SEK,		//������
	IDR,    	//ӡ���
	BUK,     	//����
	VND,		//Խ�϶�
	AED,		//��������
	//TRY,		//����������

	MYR,		//�������Ǳ�
	BRL         //������
}CurrencyType_;

//���
typedef enum  DenoType_
{
	D1 = 1,
	D2 = 2,
	D5 = 5,
	D10 = 10,
	D20 = 20,
	D50 = 50,
	D100 = 100,
	D200 = 200,
	D500 = 500,
	D1000 = 1000,
	D2000 = 2000,
	D5000 = 5000,
	D10000 = 10000,
	D20000 = 20000,
	D50000 = 50000,
	D100000 = 100000,
	D200000 = 200000,
	D500000 = 500000
}DenoType_;

//ÿһ�����İ汾��
typedef struct  VerInfo_
{
	volatile unsigned int D1_Num;
	volatile unsigned int D2_Num;
	volatile unsigned int D5_Num;
	volatile unsigned int D10_Num;
	volatile unsigned int D20_Num;
	volatile unsigned int D50_Num;
	volatile unsigned int D100_Num;
	volatile unsigned int D200_Num;
	volatile unsigned int D500_Num;
	volatile unsigned int D1000_Num;
	volatile unsigned int D2000_Num;
	volatile unsigned int D5000_Num;
	volatile unsigned int D10000_Num;
	volatile unsigned int D20000_Num;
	volatile unsigned int D50000_Num;
	volatile unsigned int D100000_Num;
	volatile unsigned int D200000_Num;
	volatile unsigned int D500000_Num;
}VerInfo_;

//���������
extern int DCTParInit();
extern int Str2CurrType(char *pStr, CurrencyType_ *pCurrencyType_Ret);
void InversrImage(IplImage_* pSrcImg, IplImage_* pDstImg);

#endif
