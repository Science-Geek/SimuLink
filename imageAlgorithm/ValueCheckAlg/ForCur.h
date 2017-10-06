/********************************************************************
创建:	2014/05/07   18:34
文件: 	E:\项目\点钞机\红外图像鉴伪\外币\外币检测 V1.0.0\inc\ForCurDet.h
语言:	h
作者:	曾霖
单位:	武汉大学

purpose:
*********************************************************************/
#ifndef FORCURDET_H_
#define FORCURDET_H_

#include "Common.h"
#include "windows.h"
#include "ImageDCT.h"


#define IMG_FOREIGNCASH_VERSION		160921
#define FOR_CUR_TEMP_CONFIG_MAGIC 0x50614375

//配置文件修改模式
typedef enum  ConfModifyType_
{
	ADD = 0,          //添加配置文件，即增加新的配置文件
	REPLACE,        //替换某个模板文件
	AOTO_MATCH,     //自动模式，程序智能确定是“添加”还是“更新”配置文件
	DELET           //删除模式，即删除某个BIN配置文件
}ConfModifyType_;


////当前程序的运行模式
typedef enum  RunningMode_
{
	DISABLE,              //不识别 啥都不做
	SELF_TRAINING,        //学习模式
	DETECT                //检测模式
}RunningMode_;

//面额和版本信息
typedef struct  DenoVerInfo_
{
	DenoType_ DenoType; //面额
	volatile int Ver;   //版本
}DenoVerInfo_;

//参数配置修改结构体
typedef struct  ParConfigModify_
{
	DenoVerInfo_ DenoVerInfo;        //面额和版本信息
	ConfModifyType_ ConfModifyType;  //修改模式
}ParConfigModify_;

////程序版本信息
typedef struct  ProgVerInfo_
{
	volatile unsigned int ProVer;        //程序版本
	MethodType_ MethodType;              //当前的程序版本对应核心算法版本

}ProgVerInfo_;

//需要保存在配置文件中的参数总配置结构体
typedef struct  ParConfigUnionSave_
{
	char CurrencyName[CURR_NAME_LEN];  //币种信息
	short ValidTemplateNum;               //有效的模板数量
	short templteType;
}ParConfigUnionSave_;


typedef struct  ParConfigCurrency_
{
	unsigned int magicNum;
	unsigned int configVersion;
	unsigned int memDevice;
	unsigned int startAddr;
	unsigned int configStartOffset;
	unsigned int binStartOffset;
	unsigned int DCTStartOffset;
	unsigned int TDCTStartOffset;
	unsigned int totalMemSize;
	unsigned int configMemSize;
	unsigned int binMemSize;
	unsigned char *pBuf;
	unsigned char *pBinBuf;
	unsigned char *pDCT;
	unsigned int DCT_len;
	unsigned char *pTDCT;
	unsigned int TDCT_len;
	ParConfigUnionSave_ *ParConfigUnionSave[MAX_CURRENCY_NUM];	//建立时，指针需要定位
	unsigned short *pH_Template[MAX_CURRENCY_NUM][MAX_CONFIG_NUM];
	unsigned int validBinNum;
	ProgVerInfo_ ProgVerInfo;
	char reserve[128];
}ParConfigCurrency_;

//参数总配置结构体
typedef struct  ParConfigUnion_
{
	CIS_ChanType_ CIS_ChanType;                  //CIS模式
	ProgVerInfo_ ProgVerInfo;                    //程序版本信息
	RunningMode_ RunningMode;                    //运行模式
	CurrencyType_ curType;                       //币种
	unsigned int ExcluThrd[MAX_CONFIG_NUM];                      //排他性阈值
	char curTypePath[128];                        //当前币种的配置信息路径
	ParConfigModify_ ParConfigModify;             //配置参数修改指针
	ParConfigUnionSave_ *pParConfigUnionSave;     //配置参数保存指针
	VerInfo_ VerInfo;                             //各面额的版本数目
	int ValidTemplateNum;                         //有效的模板数量
	unsigned short *pH_Template[MAX_CONFIG_NUM];  //模板指针
}ParConfigUnion_;

//参数总配置结构体
extern ParConfigUnion_ gParConfigUnion;

//需要保存在配置文件中的参数总配置结构体
extern ParConfigUnionSave_ gParConfigUnionSave;

extern ParConfigCurrency_ ParConfigCurrency;

extern int DCTParInit();

extern int TemplateSet(int currency);

//图像面额版本方向检测程序
extern int ImageDCT_Det(CashVVD_AT* _pSimilary, IplImage_* pImg_bottom, IplImage_* pImg_up, PicDataInfo_ *_pic_Info);

//1元和5元的补刀函数 20160112 fanyihua
extern int ValueDetection(PicDataInfo_ *PicDataInfo);


#endif
