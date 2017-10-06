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
	My_Raw_Num = 0;		//raw的个数
	My_File_Num = 0;		//第几张钱
	g_FileNum = 0;				// 磁图像用: 钱张数
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
	/*从文件路径中取文件名，并根据各参数得到对应的信息*/
	QStringList filePath_List = _filePath.split("/");
	QString fileName = filePath_List[filePath_List.size() - 1];//取文件名 xxxxx.raw
	QStringList fileName_Name_Suffix = fileName.split(".");// fileName_Name_Suffix[0]:xxxxx , fileName_Name_Suffix[1]:raw
	QStringList fileName_List = fileName_Name_Suffix[0].split("_");// 名字的小字符串

	My_File_Num++;
	g_FileNum = CashNum;
	qDebug("正在解析第\t%d\t张钱！", My_File_Num);

	/*——————————————————图像部分开始解析文件并仿真——————————————————*/
	PicDataInfo_ pic_info;
	memset(&pic_info, 0, sizeof(pic_info));
#ifdef MAG_IMG_DEBUG
	//--->图像算法入口
	Image_Alg_Manager(_filePath, &pic_info);
#endif
	/*——————————————————主控部分开始解析文件并仿真——————————————————*/
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
	qDebug("开始第%d个文件的磁图像算法\n", My_File_Num);
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

/*------------------另一种解析方式（保留）----------------------*/
	/*qDebug("正在解析第\t%d\t个raw文件:", Raw_Num);
	qDebug() << QString(_filePath).toStdString().c_str();
	int FalseDataFlg;
	char *temp = m_pBufData;

	CPU_DataBuf_Init();

	FalseDataFlg = Data_Analyze(Raw_Num - 1, temp, m_nDataLen);		//第一次数据解析
	if (FalseDataFlg == false)
	{
		qDebug("注意：出现CD4051数据通道号错误！");
		return 0;
	}
	
	Raw_Num++;
	_filePath[_filePath.size() - 5] = (Raw_Num - 1) + '0';
	qDebug("正在解析第\t%d\t个raw文件:", Raw_Num);
	qDebug() << QString(_filePath).toStdString().c_str();

	FalseDataFlg = Data_Analyze(Raw_Num - 1, temp, m_nDataLen);		//第二次数据解析
	if (FalseDataFlg == false)
	{
		qDebug("注意：出现CD4051数据通道号错误！");
		return 0;
	}*/			
					

	

	return true;
}