/*
* SoftSimulink.h
*
*  Created on: 2017-8-19
*
*  Author: YanCan
*/
#pragma once
#include <QtWidgets/QMainWindow>
#include "Entry_Alg.h"
#include "my_common.h"
#include "ui_Main_Window.h"

class SoftSimulink : public QMainWindow
{
	Q_OBJECT

public:
	SoftSimulink(QWidget *parent = Q_NULLPTR);
	bool OpenOneFile(QString File_FullPath);		//�򿪵����ļ�
	bool OpenFolder(QString File_FullPath);		//���ļ���

protected:
	//������
	bool Manage(QString File_FullPath,int CashNum);
	bool eventFilter(QObject*, QEvent*);		//�¼�������
	void Plot_InstallEventFilter(void);				//ע���¼�����
	void Plot_RemoveEventFilter(void);			//ע���¼�����
//public:
//	SoftSimulink(void);
//	~SoftSimulink(void);

private slots:
	void Find_File();
	void ClickedBtnAnalysisRawFile();
	void Clear_TextEdit();
	void Analysis_Again(const QModelIndex &);

private:
	EntryAlg m_Entry;			//��������ײ㺯������
	char *m_pBufData;		//�����ļ����ݵĻ�����
	int m_nBufLen;			//���ݻ������ĳ���
	int Ui_File_Num;			//�ڼ���Ǯ
	Simu_MainWindow My_Ui;
};
