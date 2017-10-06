/*
* Entry_Alg.h
*
*  Created on: 2017-8-19
*
*  Author: YanCan
*/
#pragma once
#include <qstring.h>
#include "my_common.h"
#include "ui_Main_Window.h"


class EntryAlg
{
public:

	//进入算法的函数，相当于算法程序的入口。若运行成功返回true，否则返回false。
	bool Algorithm_Entry(char *_pData, int _nDataLen, QString _filePath, int CashNum);

	//bool Analyse(BYTE *_pData, int _nDataLen);



private:
	Common_Tools Com_Tools;
	//保存数据交由底层处理的缓冲区
	char *m_pBufData;
	//缓冲区长度
	int m_nBufLen;
	//数据长度
	int m_nDataLen;

	int My_Raw_Num;			//第几个raw
	int My_File_Num;			//第几张钱

public:
	EntryAlg();
	~EntryAlg();
};
