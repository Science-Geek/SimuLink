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

	//�����㷨�ĺ������൱���㷨�������ڡ������гɹ�����true�����򷵻�false��
	bool Algorithm_Entry(char *_pData, int _nDataLen, QString _filePath, int CashNum);

	//bool Analyse(BYTE *_pData, int _nDataLen);



private:
	Common_Tools Com_Tools;
	//�������ݽ��ɵײ㴦��Ļ�����
	char *m_pBufData;
	//����������
	int m_nBufLen;
	//���ݳ���
	int m_nDataLen;

	int My_Raw_Num;			//�ڼ���raw
	int My_File_Num;			//�ڼ���Ǯ

public:
	EntryAlg();
	~EntryAlg();
};
