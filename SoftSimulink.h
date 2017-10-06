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
	bool OpenOneFile(QString File_FullPath);		//打开单个文件
	bool OpenFolder(QString File_FullPath);		//打开文件夹

protected:
	//处理函数
	bool Manage(QString File_FullPath,int CashNum);
	bool eventFilter(QObject*, QEvent*);		//事件过滤器
	void Plot_InstallEventFilter(void);				//注册事件函数
	void Plot_RemoveEventFilter(void);			//注销事件函数
//public:
//	SoftSimulink(void);
//	~SoftSimulink(void);

private slots:
	void Find_File();
	void ClickedBtnAnalysisRawFile();
	void Clear_TextEdit();
	void Analysis_Again(const QModelIndex &);

private:
	EntryAlg m_Entry;			//引导进入底层函数的类
	char *m_pBufData;		//保存文件数据的缓冲区
	int m_nBufLen;			//数据缓冲区的长度
	int Ui_File_Num;			//第几张钱
	Simu_MainWindow My_Ui;
};
