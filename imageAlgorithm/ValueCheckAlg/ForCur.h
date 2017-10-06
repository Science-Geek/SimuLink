/********************************************************************
����:	2014/05/07   18:34
�ļ�: 	E:\��Ŀ\�㳮��\����ͼ���α\���\��Ҽ�� V1.0.0\inc\ForCurDet.h
����:	h
����:	����
��λ:	�人��ѧ

purpose:
*********************************************************************/
#ifndef FORCURDET_H_
#define FORCURDET_H_

#include "Common.h"
#include "windows.h"
#include "ImageDCT.h"


#define IMG_FOREIGNCASH_VERSION		160921
#define FOR_CUR_TEMP_CONFIG_MAGIC 0x50614375

//�����ļ��޸�ģʽ
typedef enum  ConfModifyType_
{
	ADD = 0,          //��������ļ����������µ������ļ�
	REPLACE,        //�滻ĳ��ģ���ļ�
	AOTO_MATCH,     //�Զ�ģʽ����������ȷ���ǡ���ӡ����ǡ����¡������ļ�
	DELET           //ɾ��ģʽ����ɾ��ĳ��BIN�����ļ�
}ConfModifyType_;


////��ǰ���������ģʽ
typedef enum  RunningMode_
{
	DISABLE,              //��ʶ�� ɶ������
	SELF_TRAINING,        //ѧϰģʽ
	DETECT                //���ģʽ
}RunningMode_;

//���Ͱ汾��Ϣ
typedef struct  DenoVerInfo_
{
	DenoType_ DenoType; //���
	volatile int Ver;   //�汾
}DenoVerInfo_;

//���������޸Ľṹ��
typedef struct  ParConfigModify_
{
	DenoVerInfo_ DenoVerInfo;        //���Ͱ汾��Ϣ
	ConfModifyType_ ConfModifyType;  //�޸�ģʽ
}ParConfigModify_;

////����汾��Ϣ
typedef struct  ProgVerInfo_
{
	volatile unsigned int ProVer;        //����汾
	MethodType_ MethodType;              //��ǰ�ĳ���汾��Ӧ�����㷨�汾

}ProgVerInfo_;

//��Ҫ�����������ļ��еĲ��������ýṹ��
typedef struct  ParConfigUnionSave_
{
	char CurrencyName[CURR_NAME_LEN];  //������Ϣ
	short ValidTemplateNum;               //��Ч��ģ������
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
	ParConfigUnionSave_ *ParConfigUnionSave[MAX_CURRENCY_NUM];	//����ʱ��ָ����Ҫ��λ
	unsigned short *pH_Template[MAX_CURRENCY_NUM][MAX_CONFIG_NUM];
	unsigned int validBinNum;
	ProgVerInfo_ ProgVerInfo;
	char reserve[128];
}ParConfigCurrency_;

//���������ýṹ��
typedef struct  ParConfigUnion_
{
	CIS_ChanType_ CIS_ChanType;                  //CISģʽ
	ProgVerInfo_ ProgVerInfo;                    //����汾��Ϣ
	RunningMode_ RunningMode;                    //����ģʽ
	CurrencyType_ curType;                       //����
	unsigned int ExcluThrd[MAX_CONFIG_NUM];                      //��������ֵ
	char curTypePath[128];                        //��ǰ���ֵ�������Ϣ·��
	ParConfigModify_ ParConfigModify;             //���ò����޸�ָ��
	ParConfigUnionSave_ *pParConfigUnionSave;     //���ò�������ָ��
	VerInfo_ VerInfo;                             //�����İ汾��Ŀ
	int ValidTemplateNum;                         //��Ч��ģ������
	unsigned short *pH_Template[MAX_CONFIG_NUM];  //ģ��ָ��
}ParConfigUnion_;

//���������ýṹ��
extern ParConfigUnion_ gParConfigUnion;

//��Ҫ�����������ļ��еĲ��������ýṹ��
extern ParConfigUnionSave_ gParConfigUnionSave;

extern ParConfigCurrency_ ParConfigCurrency;

extern int DCTParInit();

extern int TemplateSet(int currency);

//ͼ�����汾���������
extern int ImageDCT_Det(CashVVD_AT* _pSimilary, IplImage_* pImg_bottom, IplImage_* pImg_up, PicDataInfo_ *_pic_Info);

//1Ԫ��5Ԫ�Ĳ������� 20160112 fanyihua
extern int ValueDetection(PicDataInfo_ *PicDataInfo);


#endif
