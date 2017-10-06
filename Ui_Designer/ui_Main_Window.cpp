/**************************************************************************

Copyright:ZMVision Technology

Author: YanCan

Date:2017年8月19日 12:42:10

Description:仿真UI_主界面  cpp

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
	//文件操作相关Hbox画布（第一排）------------------------------------
	//文件路径标签
	label_filePath = new QLabel;
	label_filePath->setObjectName(QStringLiteral("label_filePath"));
	label_filePath->setText(QApplication::translate("SimulinkClass", "\346\226\207\344\273\266\350\267\257\345\276\204\357\274\232", 0));
	label_filePath->setMinimumSize(0, 28);	//width heigh
	//文件路径编辑栏
	textEdit_filePath = new QLineEdit;
	textEdit_filePath->setObjectName(QStringLiteral("textEdit_filePath"));
	textEdit_filePath->setMinimumSize(700, 28);
	//寻找文件按钮
	pushBtn_openFile = new QPushButton;
	pushBtn_openFile->setDefault(true);
	pushBtn_openFile->setText(QApplication::translate("SimulinkClass", "打开文件"/*\350\212\235\351\272\273\345\274\200\351\227\250*/, 0));
	pushBtn_openFile->setMinimumSize(40, 28);

	filePath_Hbox = new QHBoxLayout();
	filePath_Hbox->addWidget(label_filePath);
	filePath_Hbox->addWidget(textEdit_filePath);
	filePath_Hbox->addWidget(pushBtn_openFile);
	filePath_Hbox->addStretch();
	filePath_Hbox->setSpacing(10);  //设置按钮的间距为10

	//解析按钮等相关 Hbox（第二排）---------------------------------
	//解析文件按钮
	pushBtn_analyzeFile = new QPushButton;
	pushBtn_analyzeFile->setDefault(true);
	pushBtn_analyzeFile->setText(QApplication::translate("SimulinkClass", "\345\274\200\345\247\213\350\247\243\346\236\220"/*\357\274\201*/, 0));
	pushBtn_analyzeFile->setMinimumSize(160, 40);
	//清空打印信息按钮
	pushBtn_clearTextEdit = new QPushButton;
	pushBtn_clearTextEdit->setDefault(true);
	pushBtn_clearTextEdit->setText(QApplication::translate("SimulinkClass", "清空打印信息", 0));
	pushBtn_clearTextEdit->setMaximumSize(100, 40);

	analyzeFile_Hbox = new QHBoxLayout();
	analyzeFile_Hbox->addWidget(pushBtn_clearTextEdit, Qt::AlignLeft);
	analyzeFile_Hbox->addStretch();
	analyzeFile_Hbox->addWidget(pushBtn_analyzeFile);
	analyzeFile_Hbox->addStretch();

	//绘制波形相关 Vbox（第三排）--------------------------------------
	//绘图分割窗口
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
	/*-------------------初始化对应的画图画布开始------------------*/
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
	/*-------------------初始化对应的画图画布结束------------------*/
	plotWave_Vbox = new QVBoxLayout();
	plotWave_Vbox->addWidget(splitterMain);

	//文本信息打印窗口（第四排）-----------------------------------------
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


	//主窗口框架（全局架构）----------------------------------------------
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

	//单个波形双击全屏-------------------------
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
	//清除所有分隔窗口中的所有组件
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
*	初始化所有画图用的画布
*	@splitter_p 画图所在画布的分隔窗指针
*	@PLOTNum	所需要画布的数量
*/
void Simu_MainWindow::InitPlotPanel(QCustomPlot *plotPanel)
{
	QBrush Background_Color(QColor(100, 100, 100));//设置背景色
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
*	更新画图画布上的数据
*	@splitter_p 画图所在画布的分隔窗指针
*	@PLOTIndex	更新的画图在分隔窗中的位置
*	@MadeData_p	更新的数据
*	@color		画图的颜色	1	绿色;0	红色
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
		qDebug() << "没有找到对应位置的QCustomPlot";
		return;
	}

	if (color == 0)
		findPlotPanel->graph(0)->setPen(QPen(Qt::red, 1));
	else
		findPlotPanel->graph(0)->setPen(QPen(Qt::green, 1));

	
	////定义两个可变数组存放绘图的坐标数据//分别存放x和y坐标的数据,Length为数据长度
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
		QApplication::processEvents();		// todo 这个后期看能不能优化，
		QApplication::processEvents();
		QApplication::processEvents();
		QApplication::processEvents();
	}
}

/**
*	钱币打印信息结果表格初始化 
*	@cash_tableview 表格
*	@table_head	 表头
*/
void Simu_MainWindow::Cash_Info_TableView_Init(QTableView *cash_tableview, QStandardItemModel *table_head)
{
	table_head->setHorizontalHeaderItem(0, new  QStandardItem(QObject::tr("序号")));
	table_head->setHorizontalHeaderItem(1, new  QStandardItem(QObject::tr("绝对路径")));
	table_head->setHorizontalHeaderItem(2, new  QStandardItem(QObject::tr("角度")));
	table_head->setHorizontalHeaderItem(3, new  QStandardItem(QObject::tr("宽度")));
	table_head->setHorizontalHeaderItem(4, new  QStandardItem(QObject::tr("版本")));
	table_head->setHorizontalHeaderItem(5, new  QStandardItem(QObject::tr("方向")));
	table_head->setHorizontalHeaderItem(6, new  QStandardItem(QObject::tr("面额")));
	table_head->setHorizontalHeaderItem(7, new  QStandardItem(QObject::tr("报错代码")));

	cash_tableview->setModel(table_head);

	cash_tableview->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
	cash_tableview->horizontalHeader()->setStretchLastSection(true);//设置充满表宽度
	cash_tableview->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}"); //设置表头背景色
	cash_tableview->horizontalHeader()->setSectionsClickable(false); //设置表头不可点击（默认点击后进行排序）
	//cash_tableview->setStyleSheet("background-color:lightGray;");
	cash_tableview->setStyleSheet("selection-background-color:darkCyan;"); //设置选中背景色
	cash_tableview->verticalHeader()->setDefaultSectionSize(15); //设置行高
	cash_tableview->setShowGrid(false); //设置不显示格子线
	cash_tableview->verticalHeader()->setVisible(false); //设置垂直头不可见
	cash_tableview->setFrameShape(QFrame::NoFrame);//设置无边框
	cash_tableview->setSelectionBehavior(QAbstractItemView::SelectRows);//设置选中时为整行选中
	cash_tableview->setEditTriggers(QAbstractItemView::NoEditTriggers);//设置表格的单元为只读属性，即不能编辑 
	//cash_tableview->setMouseTracking(true);//鼠标跟随tips
	cash_tableview->setMinimumHeight(260);

}

/**
*	动态插入一张钱的结果信息
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
	one_cash_info->sort(7, Qt::DescendingOrder);		//降序,报错的排前面
}

/**
*	初始化全屏二维波形画布
*	@pWave 画布指针
*/
void Simu_MainWindow::Init_MaxWave(QCustomPlot *pWave)
{
	QBrush Background_Color(QColor(0, 0, 0));//设置背景色
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
*	绘制全屏二维波形
*	@MadeData_p	更新的数据
*	@pWave 画布指针
*/
void Simu_MainWindow::PlotMaxWave(OneDSignal *MadeData_p, QCustomPlot *pWave)
{
	int  i = 0;

	//定义两个可变数组存放绘图的坐标数据//分别存放x和y坐标的数据,Length为数据长度
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
*	初始化全屏波形画布
*	@pDataTable	表格指针
*/
void Simu_MainWindow::Init_DataTableWidget(QTableWidget *pDataTable)
{
	/*--set tableHead begin--*/
	pDataTable->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}"); //设置表头背景色
	pDataTable->horizontalHeader()->setStretchLastSection(true);//设置充满表宽度
	pDataTable->horizontalHeader()->setSectionsClickable(false); //设置表头不可点击（默认点击后进行排序）
	pDataTable->horizontalHeader()->setFixedHeight(25); //设置表头的高度为25
	/*--set tableHead end--*/

	//pDataTable->setSelectionBehavior(QAbstractItemView::SelectRows);  //设置选择行为时每次选择一行
	pDataTable->setSelectionMode(QAbstractItemView::SingleSelection);		//设置单个选中
	pDataTable->verticalHeader()->setDefaultSectionSize(20); //设置行高
	pDataTable->setShowGrid(false); //设置不显示格子线
	pDataTable->verticalHeader()->setVisible(false); //设置垂直头不可见
	pDataTable->setFrameShape(QFrame::NoFrame);//设置无边框
	pDataTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);//显示滚动条
	pDataTable->setEditTriggers(QAbstractItemView::NoEditTriggers);//设置表格的单元为只读属性，即不能编辑 
	pDataTable->setColumnCount(3);//设置列数，Num，Vol，Wheel
	pDataTable->setEditTriggers(QAbstractItemView::NoEditTriggers);//设置单元格不可编辑
	//pDataTable->setRowCount(RowNum);//设置行数

	//创建表头
	QStringList heads;
	heads << "序号(x轴)" << "码盘" << "电压值(y轴)";
	pDataTable->setHorizontalHeaderLabels(heads);

}

/**
*	双击波形全屏展示细节函数
*	@MadeData_p	更新的数据
*/
void Simu_MainWindow::DoubleClick_MaxWave(OneDSignal *MadeData_p, QCustomPlot *oneWave)
{
	int i = 0;
	PlotMaxWave(MadeData_p, oneWave);

	oneMaxWidget->setWindowState(Qt::WindowMaximized);

	oneMaxWidget->show();

	//for (i = 0; i < 2; i++)
	//{
	//	QApplication::processEvents();		// todo 这个后期看能不能优化，
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

