/*
* Entry_Alg.cpp
*
*  Created on: 2017-8-19
*
*  Author: YanCan
*/

#include "Entry_Alg.h"
#include "Currency_Check_Manage.h"

/*--------imageAlgorithm_headFile---*/
#include "imageAlgorithm\Recognition.h"
/*-------------------------------------------*/

#include <QStringList.h>
#include <qdebug.h>
#pragma execution_character_set("utf-8")
int g_FileNum = 0;
//#include <cv.h>
//#include <highgui.h>

EntryAlg::EntryAlg()
{
	m_pBufData = NULL;
	m_nBufLen = 1024 * 1024;
	My_Raw_Num = 0;		//raw�ĸ���
	My_File_Num = 0;		//�ڼ���Ǯ
	g_FileNum = 0;				// ��ͼ����: Ǯ����
	Common_Tools::GetMemory((void **)&m_pBufData, m_nBufLen);
}

EntryAlg::~EntryAlg()
{
	if (m_pBufData != NULL)
	{
		Common_Tools::FreeMemory((void **)&m_pBufData);
		m_nBufLen = 0;
	}
}

bool EntryAlg::Algorithm_Entry(char *_pData, int _nDataLen, QString _filePath, int CashNum)
{
	/*���ļ�·����ȡ�ļ����������ݸ������õ���Ӧ����Ϣ*/
	QStringList filePath_List = _filePath.split("/");
	QString fileName = filePath_List[filePath_List.size() - 1];//ȡ�ļ��� xxxxx.raw
	QStringList fileName_Name_Suffix = fileName.split(".");// fileName_Name_Suffix[0]:xxxxx , fileName_Name_Suffix[1]:raw
	QStringList fileName_List = fileName_Name_Suffix[0].split("_");// ���ֵ�С�ַ���

	My_File_Num++;
	g_FileNum = CashNum;
	qDebug("���ڽ�����\t%d\t��Ǯ��", My_File_Num);

	/*������������������������������������ͼ�񲿷ֿ�ʼ�����ļ������桪����������������������������������*/
	PicDataInfo_ pic_info;
	memset(&pic_info, 0, sizeof(pic_info));
#ifdef MAG_IMG_DEBUG
	//--->ͼ���㷨���
	Image_Alg_Manager(_filePath, &pic_info);
#endif
	/*���������������������������������������ز��ֿ�ʼ�����ļ������桪����������������������������������*/
	RESULT_RMB RMB_result = { 0 };
	FuncSwitch FuncSwitch = { 0 };
	SensitiveParam.Err_256 = 5;
	SensitiveParam.Err_257 = 0;
	SensitiveParam.Err_453 = 1;
	SensitiveParam.Err_403 = 0;
	SensitiveParam.Err_459 = 1;
	SensitiveParam.Err_351 = 0;
	SensitiveParam.Err_353 = 5;
	SensitiveParam.Err_354 = 7;
	SensitiveParam.Err_201 = 1;
	SensitiveParam.Err_202 = 3;
	SensitiveParam.Err_454 = 1;
	SensitiveParam.Err_455 = 1;
	SensitiveParam.Err_456 = 1;
	SensitiveParam.Err_457 = 1;
	SensitiveParam.Err_458 = 1;
	SensitiveParam.Err_459 = 1;
	FuncSwitch.RMB_4th = 1;
	FuncSwitch.RMB_Space = 1;

	int LastError = 0;
	My_Raw_Num = 0;
	CPU_DataBuf_Init();

	for (My_Raw_Num = 0; My_Raw_Num < RAW_NUM;)
		One_Raw_Data_Analyze(_filePath, ++My_Raw_Num);

	Currency_CheckManage(&RMB_result, LastError, &SensitiveParam, &FuncSwitch);

	memcpy(Currency_Result, &RMB_result, sizeof(RMB_result));

#ifdef MAG_IMG_DEBUG
	qDebug("��ʼ��%d���ļ��Ĵ�ͼ���㷨\n", My_File_Num);
	if (pic_info.all_wheels[0] > 0)
	{
		unsigned int hight = pic_info.height < 385 ? pic_info.height : 385;
		MagImg_imgPro(pic_info.up_data,
			pic_info.coordinates_up.x,
			pic_info.coordinates_up.y,
			pic_info.width,
			pic_info.height,
			pic_info.channle_num,
			pic_info.value,
			pic_info.version,
			pic_info.direction,
			pic_info.all_wheels,
			hight*pic_info.channle_num,
			0,
			&SensitiveParam);

		RMB_result.ERRtype = MagImg_masterPro();
		MagImgFree();

	}
#endif

	//CPU_DataBuf_Delete(AlgSignal);

/*------------------��һ�ֽ�����ʽ��������----------------------*/
	/*qDebug("���ڽ�����\t%d\t��raw�ļ�:", Raw_Num);
	qDebug() << QString(_filePath).toStdString().c_str();
	int FalseDataFlg;
	char *temp = m_pBufData;

	CPU_DataBuf_Init();

	FalseDataFlg = Data_Analyze(Raw_Num - 1, temp, m_nDataLen);		//��һ�����ݽ���
	if (FalseDataFlg == false)
	{
		qDebug("ע�⣺����CD4051����ͨ���Ŵ���");
		return 0;
	}
	
	Raw_Num++;
	_filePath[_filePath.size() - 5] = (Raw_Num - 1) + '0';
	qDebug("���ڽ�����\t%d\t��raw�ļ�:", Raw_Num);
	qDebug() << QString(_filePath).toStdString().c_str();

	FalseDataFlg = Data_Analyze(Raw_Num - 1, temp, m_nDataLen);		//�ڶ������ݽ���
	if (FalseDataFlg == false)
	{
		qDebug("ע�⣺����CD4051����ͨ���Ŵ���");
		return 0;
	}*/			
					

	

	return true;
}