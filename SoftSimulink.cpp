/*
* SoftSimulink.cpp
*
*  Created on: 2017-8-19
*
*  Author: YanCan
*/
#include "SoftSimulink.h"
#include <QFileDialog.h>
#include <QFileInfo>
#include "Algorithm\machine_type.h"
#ifdef JULONG_TMR
#include "Algorithm\julong_tmr\macro_cfg.h"
#endif

#pragma execution_character_set("utf-8")

static int CashNum = 0;

SoftSimulink::SoftSimulink(QWidget *parent)
	: QMainWindow(parent)
{
	My_Ui.Setup_SumilinkWindow(this);

	connect(My_Ui.pushBtn_openFile, SIGNAL(clicked()), this, SLOT(Find_File()));
	connect(My_Ui.pushBtn_analyzeFile, SIGNAL(clicked()), this, SLOT(ClickedBtnAnalysisRawFile()));
	connect(My_Ui.pushBtn_clearTextEdit, SIGNAL(clicked()), this, SLOT(Clear_TextEdit()));
	connect(My_Ui.Cash_Result_Table, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(Analysis_Again(const QModelIndex &)));

}


//�¼�������
bool SoftSimulink::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::MouseButtonDblClick)
	{
		if (obj == My_Ui.Wave_Widget_BM_IR[0])
		{
			qDebug("666");
		}
	}
	//int i = 0;
	////BM IR
	//for (i = 0; i < 4; i++)
	//{
	//	if (obj == My_Ui.Wave_Widget_BM_IR[i] && event->type() == QEvent::MouseButtonDblClick)
	//	{
	//		switch (i)
	//		{
	//			case 0: My_Ui.DoubleClick_MaxWave(&AlgDataBuf->IRData[IRLC], My_Ui.oneWave); return true;
	//			case 1: My_Ui.DoubleClick_MaxWave(&AlgDataBuf->BM, My_Ui.oneWave); return true;
	//			case 2: My_Ui.DoubleClick_MaxWave(&AlgDataBuf->HD, My_Ui.oneWave); return true;
	//			case 3: My_Ui.DoubleClick_MaxWave(&AlgDataBuf->IRData[IRRC], My_Ui.oneWave); return true;
	//			default:
	//				return true;
	//		}
	//	}
	//}
	////MM IR
	//for (i = 0; i < 3; i++)
	//{
	//	if (obj == My_Ui.Wave_Widget_LMM_IR[i] && event->type() == QEvent::MouseButtonDblClick)
	//	{
	//		switch (i)
	//		{
	//			case 0: My_Ui.DoubleClick_MaxWave(&AlgDataBuf->LMM[LMM], My_Ui.oneWave); return true;
	//			case 1: My_Ui.DoubleClick_MaxWave(&AlgDataBuf->LMM[LMM1], My_Ui.oneWave); return true;
	//			case 2: My_Ui.DoubleClick_MaxWave(&AlgDataBuf->IRData[IRL1], My_Ui.oneWave); return true;
	//			default:
	//				return true;
	//		}
	//	}
	//	else if (obj == My_Ui.Wave_Widget_RMM_IR[i] && event->type() == QEvent::MouseButtonDblClick)
	//	{
	//		switch (i)
	//		{
	//			case 0: My_Ui.DoubleClick_MaxWave(&AlgDataBuf->IRData[IRR1], My_Ui.oneWave); return true;
	//			case 1: My_Ui.DoubleClick_MaxWave(&AlgDataBuf->RMM[RMM1], My_Ui.oneWave); return true;
	//			case 2: My_Ui.DoubleClick_MaxWave(&AlgDataBuf->RMM[RMM], My_Ui.oneWave); return true;
	//			default:
	//				return true;
	//		}
	//	}
	//}
	////SM
	//for (i = 0; i < 8; i++)
	//{
	//	if (obj == My_Ui.Wave_Widget_LSM[i] && event->type() == QEvent::MouseButtonDblClick)
	//	{
	//		My_Ui.DoubleClick_MaxWave(&AlgDataBuf->LSM[i], My_Ui.oneWave); 
	//		return true;
	//	}
	//	else if (obj == My_Ui.Wave_Widget_RSM[i] && event->type() == QEvent::MouseButtonDblClick)
	//	{
	//		My_Ui.DoubleClick_MaxWave(&AlgDataBuf->RSM[SM_NUM - 1 - i], My_Ui.oneWave);
	//		return true;
	//	}
	//}

	return QMainWindow::eventFilter(obj, event);
}
//ע���¼�����
void SoftSimulink::Plot_InstallEventFilter(void)
{
	int i = 0;
	//BM IR
	for (i = 0; i < 4; i++)
	{
		My_Ui.Wave_Widget_BM_IR[i]->installEventFilter(this);
	}
	//MM IR
	for (i = 0; i < 3; i++)
	{
		My_Ui.Wave_Widget_LMM_IR[i]->installEventFilter(this);
		My_Ui.Wave_Widget_RMM_IR[i]->installEventFilter(this);
	}
	//SM
	for (i = 0; i < 8; i++)
	{
		My_Ui.Wave_Widget_LSM[i]->installEventFilter(this);
		My_Ui.Wave_Widget_RSM[i]->installEventFilter(this);
	}
}
//ע���¼�����
void SoftSimulink::Plot_RemoveEventFilter(void)
{
	int i = 0;
	//BM IR
	for (i = 0; i < 4; i++)
	{
		My_Ui.Wave_Widget_BM_IR[i]->removeEventFilter(this);
	}
	//MM IR
	for (i = 0; i < 3; i++)
	{
		My_Ui.Wave_Widget_LMM_IR[i]->removeEventFilter(this);
		My_Ui.Wave_Widget_RMM_IR[i]->removeEventFilter(this);
	}
	//SM
	for (i = 0; i < 8; i++)
	{
		My_Ui.Wave_Widget_LSM[i]->removeEventFilter(this);
		My_Ui.Wave_Widget_RSM[i]->removeEventFilter(this);
	}
}
//Ѱ���ļ�
void SoftSimulink::Find_File()
{
	QString s = QFileDialog::getExistingDirectory(this, "֥�鿪��!", ".");
	My_Ui.textEdit_filePath->setText(s);
}
//�����ļ�
void SoftSimulink::ClickedBtnAnalysisRawFile()
{
	int i = 0;
	bool True_Flag;
	QString file_fullpath;
	QFileInfo filePath_Info;

	file_fullpath = My_Ui.textEdit_filePath->text();

	//���δѡ���ļ�����ʲôҲ����
	if (file_fullpath.isEmpty())
	{
		return;
	}
	CashNum = 0;
	//�Ƚ��ý�����ť����ֹ����
	My_Ui.pushBtn_analyzeFile->setEnabled(false);
	//ע����ͼ���ڣ���ֹ����
	Plot_RemoveEventFilter(); 
	qDebug() << "ע����ͼ����!";
	True_Flag = OpenFolder(file_fullpath);
	if (!True_Flag)
	{
		//������ť
		My_Ui.pushBtn_analyzeFile->setEnabled(true);
		QMessageBox::warning(NULL, "��������", "�ļ���·��������!", QMessageBox::Ok, QMessageBox::Ok);
		qDebug() << "�ļ���·��������!";
	}
	//������ϣ�����������ť
	My_Ui.pushBtn_analyzeFile->setEnabled(true);
	//ע�ửͼ����
	Plot_InstallEventFilter();
	qDebug() << "��ͼ����ע��ɹ�!";
}
//��մ�ӡ����
void SoftSimulink::Clear_TextEdit()
{
	int i = 0;
	int Cnt = My_Ui.One_Cash_Info->rowCount();
	for (i = Cnt - 1; i > -1; i--)
	{
		My_Ui.One_Cash_Info->removeRow(i);
	}

	My_Ui.Cash_Info_TableView_Init(My_Ui.Cash_Result_Table,My_Ui.One_Cash_Info);
	//CPU_DataBuf_Delete(AlgSignal);
}
//˫����ӡ���ڵ��ļ��������ٴν���
void SoftSimulink::Analysis_Again(const QModelIndex &)
{
	CashNum = 0;
	QModelIndex index = My_Ui.Cash_Result_Table->currentIndex();
	QString Click_filePath = index.data().toString();	//��n�е�1�е�����

	if (index.isValid() && Click_filePath.right(5) == "0.raw")
	{
		CashNum = index.row() + 1;
		//���봦����
		if (!Manage(Click_filePath, CashNum))
		{
			//����ʧ��
			qDebug() << "��Ҳ���ļ��򲻿�!";
			return ;
		}

		QString ImgPath = QCoreApplication::applicationDirPath();
		ImgPath.append(QString("/imgRes/magImg/magZone_%1.bmp").arg(CashNum));
		if (!My_Ui.My_Img->load(ImgPath))		//����ͼƬ
		{
			qDebug("���ļ�ʧ�ܣ�");
			return;
		}
		My_Ui.Img_Of_Label->setPixmap(QPixmap::fromImage(*My_Ui.My_Img));	//drawImg
		My_Ui.Img_Of_Label->setAlignment(Qt::AlignCenter);
		QApplication::processEvents();
		QApplication::processEvents();
		return ;
	}
	else
	{
		return;
	}
}
//�򿪵����ļ�
bool SoftSimulink::OpenOneFile(QString File_FullPath)
{

	if (File_FullPath.right(5) == "0.raw")
	{
		CashNum++;
		//���봦����
		if (!Manage(File_FullPath, CashNum))
		{
			//����ʧ��
			qDebug() << "��Ҳ���ļ��򲻿�!";
			//m_File.Close();
			return false;
		}
		return true;
	}
	else
	{
		return false;
	}

}
//���ļ���
bool SoftSimulink::OpenFolder(QString File_FullPath)
{
	int i = 0;
	QDir Dir(File_FullPath);
	QFileInfo filePath_Info(File_FullPath);
	QString Absolute_FilePath;
	QStringList Filters;
	QStringList string_list;
	Ui_File_Num = 0;

	if (filePath_Info.isFile())
	{
		//���ļ����е����ļ�����
		OpenOneFile(File_FullPath);
	}
	else
	{
		//�ж�·���Ƿ����
		if (!Dir.exists())
		{
			return false;
		}
		//��ȡ��ѡ�ļ����͹�����
		Filters << QString("*.raw");
		//��������������ù�����
		QDirIterator Dir_Iterator(File_FullPath, Filters, QDir::Files, QDirIterator::Subdirectories);
		while (Dir_Iterator.hasNext())
		{
			Dir_Iterator.next();
			filePath_Info = Dir_Iterator.fileInfo();
			Absolute_FilePath = filePath_Info.absoluteFilePath();
			//string_list.append(Absolute_FilePath);

			if (!filePath_Info.isDir())
			{
				m_pBufData = NULL;
				m_nBufLen = 1024 * 1024;
				Common_Tools::GetMemory((void **)&m_pBufData, m_nBufLen);
				//���ļ����е����ļ�����
				OpenOneFile(Absolute_FilePath);			// <-----------���ļ����е����ļ�����
				if (m_pBufData != NULL)
				{
					Common_Tools::FreeMemory((void **)&m_pBufData);
					m_nBufLen = 0;
				}
			}
			else
			{
				//ѭ�������ҵ����ļ�
				OpenFolder(Absolute_FilePath);
			}

		}
	}
	return true;
}
//�ļ�����
bool SoftSimulink::Manage(QString File_FullPath, int CashNum)
{
	int nDataLen = 0;
	QFile My_File(File_FullPath);			//���ڴ򿪵����ļ�

	//��ָ�����ļ�
	if (!My_File.open(QFile::ReadOnly))
	{
		//���ļ�ʧ��
		qDebug() << "�ļ��򲻿�����!";
		return false;
	}

	//�����жϻ������Ƿ���
	nDataLen = My_File.size();
	if (m_nBufLen < nDataLen)
	{
		m_nBufLen = nDataLen;
		Common_Tools::FreeMemory(reinterpret_cast<void **>(&m_pBufData));
		Common_Tools::GetMemory(reinterpret_cast<void **>(&m_pBufData), m_nBufLen);
	}
	//���ļ��е����ݶ��뻺����
	QDataStream input(&My_File);
	input.readRawData(m_pBufData, nDataLen);
	//�ر��ļ�
	My_File.close();
	//�ͷ��ڴ�
	CPU_DataBuf_Delete(AlgSignal);

	//�����ײ�
	if (m_Entry.Algorithm_Entry(m_pBufData, nDataLen, File_FullPath, CashNum))
	{
		int i = 0;
		Ui_File_Num++;		
		QModelIndex index = My_Ui.Cash_Result_Table->currentIndex();

		if (!index.isValid())
		{
			My_Ui.Insert_OneCash_Result_Info(Currency_Result, Ui_File_Num, File_FullPath, My_Ui.One_Cash_Info);
		}

		My_Ui.RefreshPlotPanelData(My_Ui.splitterMain_BMIR, 0, &AlgDataBuf->IRData[IRLC],0);
		My_Ui.RefreshPlotPanelData(My_Ui.splitterMain_BMIR, 1, &AlgDataBuf->BM, 0);
		My_Ui.RefreshPlotPanelData(My_Ui.splitterMain_BMIR, 2, &AlgDataBuf->HD, 0);
		My_Ui.RefreshPlotPanelData(My_Ui.splitterMain_BMIR, 3, &AlgDataBuf->IRData[IRRC], 1);

		My_Ui.RefreshPlotPanelData(My_Ui.splitterMain_LMM, 0, &AlgDataBuf->LMM[LMM], 0);		//��
		My_Ui.RefreshPlotPanelData(My_Ui.splitterMain_LMM, 1, &AlgDataBuf->LMM[LMM1], 0);		//��
		My_Ui.RefreshPlotPanelData(My_Ui.splitterMain_LMM, 2, &AlgDataBuf->IRData[IRL1], 0);

		My_Ui.RefreshPlotPanelData(My_Ui.splitterMain_RMM, 0, &AlgDataBuf->IRData[IRR1], 1);
		My_Ui.RefreshPlotPanelData(My_Ui.splitterMain_RMM, 1, &AlgDataBuf->RMM[RMM1], 1);		//��
		My_Ui.RefreshPlotPanelData(My_Ui.splitterMain_RMM, 2, &AlgDataBuf->RMM[RMM], 1);		//��

		for (i = 0; i < SM_NUM; i++)
		{
			My_Ui.RefreshPlotPanelData(My_Ui.splitterMain_LSM, i, &AlgDataBuf->LSM[LSM1 + i], 0);
		}
		for (i = 0; i < SM_NUM; i++)
		{
			My_Ui.RefreshPlotPanelData(My_Ui.splitterMain_RSM, i, &AlgDataBuf->RSM[SM_NUM - 1 - i], 1);
		}

		Sleep(100);
		return true;
	}
	else
	{
		//����ʧ�ܵ����
		return false;
	}
	return true;
}




//SoftSimulink::SoftSimulink(void)
//{
//	m_pBufData = NULL;
//	m_nBufLen = 1024 * 1024;
//	Raw_Num = 0;
//	Common_Tools::GetMemory((void **)&m_pBufData, m_nBufLen);
//
//}
//
//SoftSimulink::~SoftSimulink(void)
//{
//	if (m_pBufData != NULL)
//	{
//		Common_Tools::FreeMemory((void **)&m_pBufData);
//		m_nBufLen = 0;
//	}
//}