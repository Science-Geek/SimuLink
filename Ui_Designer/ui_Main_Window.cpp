/**************************************************************************

Copyright:ZMVision Technology

Author: YanCan

Date:2017��8��19�� 12:42:10

Description:����UI_������  cpp

**************************************************************************/

#include <QtWidgets/QMainWindow>
#include "ui_Main_Window.h"

#pragma execution_character_set("utf-8")

void  Simu_MainWindow::Setup_SumilinkWindow(QMainWindow *SimulinkClass)
{
	if (SimulinkClass->objectName().isEmpty())
	{
		SimulinkClass->setObjectName(QStringLiteral("SimulinkClass"));
		SimulinkClass->setWindowState(Qt::WindowMaximized);
	}
	//�ļ��������Hbox��������һ�ţ�------------------------------------
	//�ļ�·����ǩ
	label_filePath = new QLabel;
	label_filePath->setObjectName(QStringLiteral("label_filePath"));
	label_filePath->setText(QApplication::translate("SimulinkClass", "\346\226\207\344\273\266\350\267\257\345\276\204\357\274\232", 0));
	label_filePath->setMinimumSize(0, 28);	//width heigh
	//�ļ�·���༭��
	textEdit_filePath = new QLineEdit;
	textEdit_filePath->setObjectName(QStringLiteral("textEdit_filePath"));
	textEdit_filePath->setMinimumSize(700, 28);
	//Ѱ���ļ���ť
	pushBtn_openFile = new QPushButton;
	pushBtn_openFile->setDefault(true);
	pushBtn_openFile->setText(QApplication::translate("SimulinkClass", "���ļ�"/*\350\212\235\351\272\273\345\274\200\351\227\250*/, 0));
	pushBtn_openFile->setMinimumSize(40, 28);

	filePath_Hbox = new QHBoxLayout();
	filePath_Hbox->addWidget(label_filePath);
	filePath_Hbox->addWidget(textEdit_filePath);
	filePath_Hbox->addWidget(pushBtn_openFile);
	filePath_Hbox->addStretch();
	filePath_Hbox->setSpacing(10);  //���ð�ť�ļ��Ϊ10

	//������ť����� Hbox���ڶ��ţ�---------------------------------
	//�����ļ���ť
	pushBtn_analyzeFile = new QPushButton;
	pushBtn_analyzeFile->setDefault(true);
	pushBtn_analyzeFile->setText(QApplication::translate("SimulinkClass", "\345\274\200\345\247\213\350\247\243\346\236\220"/*\357\274\201*/, 0));
	pushBtn_analyzeFile->setMinimumSize(160, 40);
	//��մ�ӡ��Ϣ��ť
	pushBtn_clearTextEdit = new QPushButton;
	pushBtn_clearTextEdit->setDefault(true);
	pushBtn_clearTextEdit->setText(QApplication::translate("SimulinkClass", "��մ�ӡ��Ϣ", 0));
	pushBtn_clearTextEdit->setMaximumSize(100, 40);

	analyzeFile_Hbox = new QHBoxLayout();
	analyzeFile_Hbox->addWidget(pushBtn_clearTextEdit, Qt::AlignLeft);
	analyzeFile_Hbox->addStretch();
	analyzeFile_Hbox->addWidget(pushBtn_analyzeFile);
	analyzeFile_Hbox->addStretch();

	//���Ʋ������ Vbox�������ţ�--------------------------------------
	//��ͼ�ָ��
	splitterMain = new QSplitter(Qt::Vertical);
	splitterMain->setObjectName(QStringLiteral("splitterMain"));
	splitterMain->setDisabled(true);
	//BM
	splitterMain_BMIR = new QSplitter(Qt::Horizontal, splitterMain);
	splitterMain_BMIR->setObjectName(QStringLiteral("splitterMain_BMIR"));
	splitterMain_BMIR->setMinimumHeight(200);
	//MM
	splitter_MM = new QSplitter(Qt::Horizontal, splitterMain);
	splitter_MM->setObjectName(QStringLiteral("splitter_MM"));
	splitter_MM->setMinimumHeight(200);
	splitterMain_LMM = new QSplitter(Qt::Horizontal, splitter_MM);
	splitterMain_LMM->setObjectName(QStringLiteral("splitterMain_LMM"));
	splitterMain_RMM = new QSplitter(Qt::Horizontal, splitter_MM);
	splitterMain_RMM->setObjectName(QStringLiteral("splitterMain_RMM"));
	//SM
	splitter_SM = new QSplitter(Qt::Horizontal, splitterMain);
	splitter_SM->setObjectName(QStringLiteral("splitter_SM"));
	splitter_SM->setMinimumHeight(200);
	splitterMain_LSM = new QSplitter(Qt::Horizontal, splitter_SM);
	splitterMain_LSM->setObjectName(QStringLiteral("splitterMain_LSM"));
	splitterMain_RSM = new QSplitter(Qt::Horizontal, splitter_SM);
	splitterMain_RSM->setObjectName(QStringLiteral("splitterMain_RSM"));
	/*-------------------��ʼ����Ӧ�Ļ�ͼ������ʼ------------------*/
	int i = 0;
	//BM IR
	for (i = 0; i < 4; i++)
	{
		Wave_Widget_BM_IR[i] = new QCustomPlot(splitterMain_BMIR);
		InitPlotPanel(Wave_Widget_BM_IR[i]);
	}
	//MM IR
	for (i = 0; i < 3; i++)
	{
		Wave_Widget_LMM_IR[i] = new QCustomPlot(splitterMain_LMM);
		Wave_Widget_RMM_IR[i] = new QCustomPlot(splitterMain_RMM);
		InitPlotPanel(Wave_Widget_LMM_IR[i]);
		InitPlotPanel(Wave_Widget_RMM_IR[i]);
	}
	//SM
	for (i = 0; i < 8; i++)
	{
		Wave_Widget_LSM[i] = new QCustomPlot(splitterMain_LSM);
		Wave_Widget_RSM[i] = new QCustomPlot(splitterMain_RSM);
		InitPlotPanel(Wave_Widget_LSM[i]);
		InitPlotPanel(Wave_Widget_RSM[i]);
	}
	/*-------------------��ʼ����Ӧ�Ļ�ͼ��������------------------*/
	plotWave_Vbox = new QVBoxLayout();
	plotWave_Vbox->addWidget(splitterMain);

	//�ı���Ϣ��ӡ���ڣ������ţ�-----------------------------------------
	Cash_Result_Table = new QTableView;
	Cash_Result_Table->setObjectName(QStringLiteral("Cash_Result_Table"));
	One_Cash_Info = new QStandardItemModel;
	Cash_Info_TableView_Init(Cash_Result_Table, One_Cash_Info);
	
	Img_Of_Label = new QLabel;
	Img_Of_Label->setObjectName(QStringLiteral("Img_Of_Label"));
	Img_Of_Label->setMinimumHeight(300);
	
	My_Img = new QImage;

	printText_Hbox = new QHBoxLayout();	
	printText_Hbox->addWidget(Cash_Result_Table);
	printText_Hbox->addWidget(Img_Of_Label);
	printText_Hbox->setStretchFactor(Cash_Result_Table, 5);
	printText_Hbox->setStretchFactor(Img_Of_Label, 4);


	//�����ڿ�ܣ�ȫ�ּܹ���----------------------------------------------
	Main_WindowWidget = new QWidget(SimulinkClass);
	Main_WindowWidget->setWindowState(Qt::WindowMaximized);
	SimulinkClass->setCentralWidget(Main_WindowWidget);

	Main_Vbox = new QVBoxLayout(Main_WindowWidget);
	Main_Vbox->setObjectName(QStringLiteral("Main_Vbox"));
	Main_Vbox->addLayout(filePath_Hbox);
	Main_Vbox->addLayout(analyzeFile_Hbox);
	Main_Vbox->addLayout(plotWave_Vbox);
	Main_Vbox->addLayout(printText_Hbox);

	SimulinkClass->show();

	//��������˫��ȫ��-------------------------
	oneWave = new QCustomPlot;
	Init_MaxWave(oneWave);
	dataTable = new QTableWidget;
	Init_DataTableWidget(dataTable);
	oneMaxWidget = new QWidget;

	HboxMaxWave = new QHBoxLayout(oneMaxWidget);
	HboxMaxWave->addWidget(oneWave);
	HboxMaxWave->addWidget(dataTable);
	HboxMaxWave->setStretchFactor(oneWave, 8);
	HboxMaxWave->setStretchFactor(dataTable, 2);

	//oneMaxWidget->show();

}

void  Simu_MainWindow::RemoveAllPlotPanel(void)
{
	//������зָ������е��������
	const QObjectList& list1 = splitterMain_BMIR->children();
	const QObjectList& list2 = splitterMain_LMM->children();
	const QObjectList& list3 = splitterMain_RMM->children();
	const QObjectList& list4 = splitterMain_LSM->children();
	const QObjectList& list5 = splitterMain_RSM->children();

	QObject *deleteObj;
	const char *deleteClassName;

	for (int i = 0; i<list1.length(); i++)
	{
		deleteObj = list1[i];
		//qDebug() << deleteObj;
		deleteClassName = deleteObj->metaObject()->className();
		if (strcmp(deleteClassName, "QCustomPlot") == 0) {
			delete deleteObj;
			i--;
		}
		deleteObj = NULL;
		deleteClassName = NULL;
	}
	for (int i = 0; i<list2.length(); i++)
	{
		deleteObj = list2[i];
		//qDebug() << deleteObj;
		deleteClassName = deleteObj->metaObject()->className();
		if (strcmp(deleteClassName, "QCustomPlot") == 0) {
			delete deleteObj;
			i--;
		}
		deleteObj = NULL;
		deleteClassName = NULL;
	}
	for (int i = 0; i<list3.length(); i++)
	{
		deleteObj = list3[i];
		//qDebug() << deleteObj;
		deleteClassName = deleteObj->metaObject()->className();
		if (strcmp(deleteClassName, "QCustomPlot") == 0) {
			delete deleteObj;
			i--;
		}
		deleteObj = NULL;
		deleteClassName = NULL;
	}
	for (int i = 0; i<list4.length(); i++)
	{
		deleteObj = list4[i];
		//qDebug() << deleteObj;
		deleteClassName = deleteObj->metaObject()->className();
		if (strcmp(deleteClassName, "QCustomPlot") == 0) {
			delete deleteObj;
			i--;
		}
		deleteObj = NULL;
		deleteClassName = NULL;
	}
	for (int i = 0; i<list5.length(); i++)
	{
		deleteObj = list5[i];
		//qDebug() << deleteObj;
		deleteClassName = deleteObj->metaObject()->className();
		if (strcmp(deleteClassName, "QCustomPlot") == 0) {
			delete deleteObj;
			i--;
		}
		deleteObj = NULL;
		deleteClassName = NULL;
	}

}

/**
*	��ʼ�����л�ͼ�õĻ���
*	@splitter_p ��ͼ���ڻ����ķָ���ָ��
*	@PLOTNum	����Ҫ����������
*/
void Simu_MainWindow::InitPlotPanel(QCustomPlot *plotPanel)
{
	QBrush Background_Color(QColor(100, 100, 100));//���ñ���ɫ
	plotPanel->setBackground(Background_Color);
	plotPanel->setMaximumSize(500, 200);
	plotPanel->addGraph();
	plotPanel->rescaleAxes();
	plotPanel->yAxis->setRange(0, 4500);
	plotPanel->xAxis->setTickLabels(true);
	plotPanel->xAxis->setTicks(true);
	plotPanel->yAxis->setTickLabels(false);
	plotPanel->yAxis->setTicks(false);

}

/**
*	���»�ͼ�����ϵ�����
*	@splitter_p ��ͼ���ڻ����ķָ���ָ��
*	@PLOTIndex	���µĻ�ͼ�ڷָ����е�λ��
*	@MadeData_p	���µ�����
*	@color		��ͼ����ɫ	1	��ɫ;0	��ɫ
*/
void Simu_MainWindow::RefreshPlotPanelData(QSplitter *splitter_p, int PLOTIndex, OneDSignal *MadeData_p, int color)
{
	const QObjectList& Childrenlist = splitter_p->children();
	QCustomPlot *findPlotPanel = NULL;
	int i = 0;
	int index = 0;
	int findIndex = 0;
	const char *deleteClassName;
	QObject *indexObject;
	for (index = Childrenlist.length() - 1; index >= 0; index--)
	{
		indexObject = Childrenlist[index];
		deleteClassName = indexObject->metaObject()->className();
		if (strcmp(deleteClassName, "QCustomPlot") == 0) {
			if (findIndex == PLOTIndex) 
			{
				findPlotPanel = (QCustomPlot*)indexObject;
				break;
			}
			else
				findIndex++;
		}
	}
	if (findPlotPanel == NULL) {
		qDebug() << "û���ҵ���Ӧλ�õ�QCustomPlot";
		return;
	}

	if (color == 0)
		findPlotPanel->graph(0)->setPen(QPen(Qt::red, 1));
	else
		findPlotPanel->graph(0)->setPen(QPen(Qt::green, 1));

	
	////���������ɱ������Ż�ͼ����������//�ֱ���x��y���������,LengthΪ���ݳ���
	QVector<double> x(MadeData_p->Length), y(MadeData_p->Length);
	for (i = 0; i < MadeData_p->Length; i++)
	{
		x[i] = i + 1;
		y[i] = MadeData_p->Vol[i];
	}
	findPlotPanel->graph(0)->setData(x, y);
	findPlotPanel->xAxis->setRange(0, i);
	findPlotPanel->yAxis->setRange(0, 4500);
	findPlotPanel->replot();

	for (i = 0; i < 10; i++)
	{
		QApplication::processEvents();		// todo ������ڿ��ܲ����Ż���
		QApplication::processEvents();
		QApplication::processEvents();
		QApplication::processEvents();
	}
}

/**
*	Ǯ�Ҵ�ӡ��Ϣ�������ʼ�� 
*	@cash_tableview ���
*	@table_head	 ��ͷ
*/
void Simu_MainWindow::Cash_Info_TableView_Init(QTableView *cash_tableview, QStandardItemModel *table_head)
{
	table_head->setHorizontalHeaderItem(0, new  QStandardItem(QObject::tr("���")));
	table_head->setHorizontalHeaderItem(1, new  QStandardItem(QObject::tr("����·��")));
	table_head->setHorizontalHeaderItem(2, new  QStandardItem(QObject::tr("�Ƕ�")));
	table_head->setHorizontalHeaderItem(3, new  QStandardItem(QObject::tr("���")));
	table_head->setHorizontalHeaderItem(4, new  QStandardItem(QObject::tr("�汾")));
	table_head->setHorizontalHeaderItem(5, new  QStandardItem(QObject::tr("����")));
	table_head->setHorizontalHeaderItem(6, new  QStandardItem(QObject::tr("���")));
	table_head->setHorizontalHeaderItem(7, new  QStandardItem(QObject::tr("�������")));

	cash_tableview->setModel(table_head);

	cash_tableview->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
	cash_tableview->horizontalHeader()->setStretchLastSection(true);//���ó�������
	cash_tableview->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}"); //���ñ�ͷ����ɫ
	cash_tableview->horizontalHeader()->setSectionsClickable(false); //���ñ�ͷ���ɵ����Ĭ�ϵ�����������
	//cash_tableview->setStyleSheet("background-color:lightGray;");
	cash_tableview->setStyleSheet("selection-background-color:darkCyan;"); //����ѡ�б���ɫ
	cash_tableview->verticalHeader()->setDefaultSectionSize(15); //�����и�
	cash_tableview->setShowGrid(false); //���ò���ʾ������
	cash_tableview->verticalHeader()->setVisible(false); //���ô�ֱͷ���ɼ�
	cash_tableview->setFrameShape(QFrame::NoFrame);//�����ޱ߿�
	cash_tableview->setSelectionBehavior(QAbstractItemView::SelectRows);//����ѡ��ʱΪ����ѡ��
	cash_tableview->setEditTriggers(QAbstractItemView::NoEditTriggers);//���ñ��ĵ�ԪΪֻ�����ԣ������ܱ༭ 
	//cash_tableview->setMouseTracking(true);//������tips
	cash_tableview->setMinimumHeight(260);

}

/**
*	��̬����һ��Ǯ�Ľ����Ϣ
*/
void Simu_MainWindow::Insert_OneCash_Result_Info(Ui_RESULT_RMB *Cash_Resulut, int cash_num, QString file_Path, QStandardItemModel *one_cash_info)
{
	one_cash_info->setItem(cash_num, 0, new QStandardItem(QString("%1").arg(cash_num)));
	one_cash_info->setItem(cash_num, 1, new QStandardItem(QString("%1").arg(file_Path)));
	one_cash_info->setItem(cash_num, 2, new QStandardItem(QString("%1").arg(Cash_Resulut->angle)));
	one_cash_info->setItem(cash_num, 3, new QStandardItem(QString("%1").arg(Cash_Resulut->Height)));
	one_cash_info->setItem(cash_num, 4, new QStandardItem(QString("%1").arg(Cash_Resulut->VersionM)));
	one_cash_info->setItem(cash_num, 5, new QStandardItem(QString("%1").arg(Cash_Resulut->Direction)));
	one_cash_info->setItem(cash_num, 6, new QStandardItem(QString("%1").arg(Cash_Resulut->Amount)));
	one_cash_info->setItem(cash_num, 7, new QStandardItem(QString("%1").arg(Cash_Resulut->ERRtype)));

	one_cash_info->item(cash_num, 0)->setTextAlignment(Qt::AlignCenter);
	one_cash_info->item(cash_num, 1)->setTextAlignment(Qt::AlignCenter);
	one_cash_info->item(cash_num, 2)->setTextAlignment(Qt::AlignCenter);
	one_cash_info->item(cash_num, 3)->setTextAlignment(Qt::AlignCenter);
	one_cash_info->item(cash_num, 4)->setTextAlignment(Qt::AlignCenter);
	one_cash_info->item(cash_num, 5)->setTextAlignment(Qt::AlignCenter);
	one_cash_info->item(cash_num, 6)->setTextAlignment(Qt::AlignCenter);
	one_cash_info->item(cash_num, 7)->setTextAlignment(Qt::AlignCenter);

	//one_cash_info->item()
	if (Cash_Resulut->ERRtype)
	{
		one_cash_info->item(cash_num,7)->setBackground(Qt::red);
	}
	else
	{
		one_cash_info->item(cash_num,7)->setBackground(Qt::green);
	}
	one_cash_info->sort(7, Qt::DescendingOrder);		//����,�������ǰ��
}

/**
*	��ʼ��ȫ����ά���λ���
*	@pWave ����ָ��
*/
void Simu_MainWindow::Init_MaxWave(QCustomPlot *pWave)
{
	QBrush Background_Color(QColor(0, 0, 0));//���ñ���ɫ
	pWave->setBackground(Background_Color);
	pWave->addGraph();
	pWave->rescaleAxes();
	pWave->yAxis->setRange(0, 5000);
	pWave->xAxis->setTickLabels(true);
	pWave->xAxis->setTicks(true);
	pWave->yAxis->setTickLabels(true);
	pWave->yAxis->setTicks(true);
	pWave->graph(0)->setPen(QPen(Qt::green, 2));
}
/**
*	����ȫ����ά����
*	@MadeData_p	���µ�����
*	@pWave ����ָ��
*/
void Simu_MainWindow::PlotMaxWave(OneDSignal *MadeData_p, QCustomPlot *pWave)
{
	int  i = 0;

	//���������ɱ������Ż�ͼ����������//�ֱ���x��y���������,LengthΪ���ݳ���
	QVector<double> x(MadeData_p->Length), y(MadeData_p->Length);
	for (i = 0; i < MadeData_p->Length; i++)
	{
		x[i] = i + 1;
		y[i] = MadeData_p->Vol[i];
	}
	pWave->graph(0)->setData(x, y);
	pWave->xAxis->setRange(0, i);
	pWave->yAxis->setRange(0, 5000);
	pWave->replot();

}
/**
*	��ʼ��ȫ�����λ���
*	@pDataTable	���ָ��
*/
void Simu_MainWindow::Init_DataTableWidget(QTableWidget *pDataTable)
{
	/*--set tableHead begin--*/
	pDataTable->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}"); //���ñ�ͷ����ɫ
	pDataTable->horizontalHeader()->setStretchLastSection(true);//���ó�������
	pDataTable->horizontalHeader()->setSectionsClickable(false); //���ñ�ͷ���ɵ����Ĭ�ϵ�����������
	pDataTable->horizontalHeader()->setFixedHeight(25); //���ñ�ͷ�ĸ߶�Ϊ25
	/*--set tableHead end--*/

	//pDataTable->setSelectionBehavior(QAbstractItemView::SelectRows);  //����ѡ����Ϊʱÿ��ѡ��һ��
	pDataTable->setSelectionMode(QAbstractItemView::SingleSelection);		//���õ���ѡ��
	pDataTable->verticalHeader()->setDefaultSectionSize(20); //�����и�
	pDataTable->setShowGrid(false); //���ò���ʾ������
	pDataTable->verticalHeader()->setVisible(false); //���ô�ֱͷ���ɼ�
	pDataTable->setFrameShape(QFrame::NoFrame);//�����ޱ߿�
	pDataTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);//��ʾ������
	pDataTable->setEditTriggers(QAbstractItemView::NoEditTriggers);//���ñ��ĵ�ԪΪֻ�����ԣ������ܱ༭ 
	pDataTable->setColumnCount(3);//����������Num��Vol��Wheel
	pDataTable->setEditTriggers(QAbstractItemView::NoEditTriggers);//���õ�Ԫ�񲻿ɱ༭
	//pDataTable->setRowCount(RowNum);//��������

	//������ͷ
	QStringList heads;
	heads << "���(x��)" << "����" << "��ѹֵ(y��)";
	pDataTable->setHorizontalHeaderLabels(heads);

}

/**
*	˫������ȫ��չʾϸ�ں���
*	@MadeData_p	���µ�����
*/
void Simu_MainWindow::DoubleClick_MaxWave(OneDSignal *MadeData_p, QCustomPlot *oneWave)
{
	int i = 0;
	PlotMaxWave(MadeData_p, oneWave);

	oneMaxWidget->setWindowState(Qt::WindowMaximized);

	oneMaxWidget->show();

	//for (i = 0; i < 2; i++)
	//{
	//	QApplication::processEvents();		// todo ������ڿ��ܲ����Ż���
	//	QApplication::processEvents();
	//}
}








//void Simu_MainWindow::showToolTip(const QModelIndex &index)
//{
//	if (!index.isValid())
//		return;
//
//	int row = index.row();
//	QString file_name = list_file.at(row);
//
//	if (file_name.isEmpty())
//		return;
//
//	QToolTip::showText(QCursor::pos(), file_name);
//}

