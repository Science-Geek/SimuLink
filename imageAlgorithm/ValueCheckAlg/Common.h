/***************************************************************
* Copyright (C) 2011, Wuhan University
* All rights reserved.
*
* 文件名：	Common.h
* 功能描述: 辅助工具头文件
*
* 当前版本：1.0
* 作    者：张维夏
* 完成日期：2016年9月20日
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
#define MAX_PRO_VER 30         //可预期的最大程序版本信息，为以后升级用，不同的配置文件对应不同的程序版本方法
#define CURR_PRO_VER 20140601    //当前的程序版本信息，以时间单位
#define MAX_CURRENCY_NUM  32  //支持的最大币种数
#define CUR_CURRENCY_NUM  6    //当前的币种数
#define MAX_CONFIG_NUM  60     //支持同一币种最大的配置文件数
#define CURR_NAME_LEN 16      //币种名长度


//CIS通道的排列顺序
typedef enum  CIS_ChanType_
{
	G_IR = 0,     //G+IR
	IR_G_IR,    //IR+G+IR
	B_R_G,      //B+R+G
	IR_B_R_G    //IR+B+R+G


}CIS_ChanType_;

//外币检测时采用的方法，支持不同的币种采用不同的方法
typedef enum  MethodType_
{
	METHOD1 = 0,  //方法1
	METHOD2,    //方法2
	METHOD3,    //方法3
	METHOD4     //方法4
}MethodType_;

typedef enum  CurrencyType_
{
	CNY = 0,  	//人民币
	TWD,    	//台币
	HKD,   		//港币
	USD,    	//美元
	EURO,   	//欧元
	JPY,     	//日元
	GBP,     	//英镑
	AUD,     	//澳元
	CAD,		//加拿大币
	SGD,		//新加坡币
	CHF,		//瑞士法郎
	RUB,		//卢布
	MOP,		//澳门币
	KRW,		//韩元
	PHP,		//菲尼宾比索
	ZAR,		//南非兰特
	ETB,		//埃塞俄比亚比尔
	SAR,		//沙特里亚尔
	PGK,		//巴布亚新几内亚基纳
	THB,		//泰国泰铢
	NZD,		//新西兰元
	DKK,		//丹麦克朗
	NOK,		//挪威克朗
	SEK,		//瑞典克朗
	IDR,    	//印尼盾
	BUK,     	//缅甸币
	VND,		//越南盾
	AED,		//阿联酋币
	//TRY,		//土耳其里拉

	MYR,		//马来西亚币
	BRL         //巴西币
}CurrencyType_;

//面额
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

//每一种面额的版本数
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

//外币类型名
extern int DCTParInit();
extern int Str2CurrType(char *pStr, CurrencyType_ *pCurrencyType_Ret);
void InversrImage(IplImage_* pSrcImg, IplImage_* pDstImg);

#endif
