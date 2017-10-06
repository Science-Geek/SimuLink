#include "analyze_diff_machine.h"

#include <qdebug.h>
#include <qfile.h>
#include <qdatastream.h>
#include <qmessagebox.h>
#pragma execution_character_set("utf-8")

OneDSignal AlgSignal[TDN_MAX] = { 0 };
TdnData_Alg *AlgDataBuf;
Ui_RESULT_RMB *Currency_Result;

//���ݳ��ȳ�ʼ��
void CPU_DataLen_Init(OneDSignal *OneDSignal_p)
{
	int i = 0;
	for (i = 0; i < TDN_MAX; i++)
	{
		OneDSignal_p[i].Length = 0;
	}
}

//�����ڴ�ռ�
void CPU_DataBuf_Init()			
{
	int i = 0;

	for (i = 0; i < TDN_MAX; i++)
	{
		AlgSignal[i].Vol = new int[MATER_CHANNEL_SIZE*sizeof(int)];
		AlgSignal[i].Wheel = new int[MATER_CHANNEL_SIZE*sizeof(int)];
		AlgSignal[i].Time = new int[MATER_CHANNEL_SIZE*sizeof(int)];
	}

	AlgDataBuf = new TdnData_Alg;
	Currency_Result = new Ui_RESULT_RMB;
}

//�ͷ��ڴ�ռ�
void CPU_DataBuf_Delete(OneDSignal *OneDSignal_p)		
{
	int i = 0;
	for (i = 0; i < TDN_MAX; i++)
	{
		delete(OneDSignal_p[i].Vol);
		delete(OneDSignal_p[i].Wheel);
		delete(OneDSignal_p[i].Time);
	}

	delete AlgDataBuf;
	delete Currency_Result;
}

//��������ͨ������
void OneChannelBit_Analyze(OneDSignal *OneDSignal_p, int Wheel_PerLine, int Channel_Bit, unsigned int *Packet_Head, int LineNum, int PerLine_IntNum, int PerLine_AllNum, int ADC_ChannelMap[CD4051_CHANNEL_NUM][USEFUL_ADC_CHANNEL_NUM])
{
	int j = 0;
	int Voltage = 0;

	for (j = 0; j < PerLine_IntNum; j++)
	{
		if (ADC_ChannelMap[Channel_Bit][j] == TDN_MAX)
		{
			continue;			//����λ���޴�������������ȥ��һ�У�
		}
		Voltage = (*(Packet_Head + LineNum * PerLine_AllNum + j)) & 0xFFFF;		//��ȡ��ѹֵ
		if (OneDSignal_p[ADC_ChannelMap[Channel_Bit][j]].Length < MATER_CHANNEL_SIZE)
		{
			OneDSignal_p[ADC_ChannelMap[Channel_Bit][j]].Vol[OneDSignal_p[ADC_ChannelMap[Channel_Bit][j]].Length] = Voltage;
			OneDSignal_p[ADC_ChannelMap[Channel_Bit][j]].Wheel[OneDSignal_p[ADC_ChannelMap[Channel_Bit][j]].Length] = Wheel_PerLine;
			OneDSignal_p[ADC_ChannelMap[Channel_Bit][j]].Length++;
		}
	}

}


//���ݽ���
bool Data_Analyze(int Adc_Index, char *Temp, int LineNum_CPU,int Bytes_PerLine)
{
	int i = 0;
	int j = 0;
	int Wheel_PerLine = 0;
	int Voltage = 0;
	int Channel_Bit = 0;
	int PerLine_IntNum = 0;
	int PerLine_AllNum = 0;
	int ADC_ChannelMap[CD4051_CHANNEL_NUM][USEFUL_ADC_CHANNEL_NUM] = { 0 };
	unsigned int *Packet_Head = NULL;
	OneDSignal *OneDSignal_p = NULL;

	//LineNum_CPU = Total_DataLen / ONE_LINE_BYTE_NUM;	//����������
	PerLine_IntNum = USEFUL_ADC_CHANNEL_NUM;		//����ѹֵ�ĵ�ADCͨ��������6�У����ٸ�int�������������������ֵ��CD4051ͨ��ֵ
	PerLine_AllNum = Bytes_PerLine / ONE_DATA_BYTE;	//ÿһ���ܼƵ�ADCͨ��������8�У����ٸ�int�� ====>> 6+2�����д����ͬ����
	Packet_Head = (unsigned int*)Temp;
	OneDSignal_p = AlgSignal;

	if (Adc_Index == 0)
	{
		memcpy(ADC_ChannelMap, ADC0_ChannelMap, sizeof(ADC_ChannelMap));
		CPU_DataLen_Init(OneDSignal_p);
	}
	else if (Adc_Index == 1)
		memcpy(ADC_ChannelMap, ADC1_ChannelMap, sizeof(ADC_ChannelMap));
	else
		return false;

	for (i = 0; i < LineNum_CPU; i++)
	{
		Wheel_PerLine = (*(Packet_Head + (i * PerLine_AllNum) + USEFUL_ADC_CHANNEL_NUM));				 //��ȡ����ֵ
		Channel_Bit = (*(Packet_Head + (i * PerLine_AllNum) + USEFUL_ADC_CHANNEL_NUM + 1)) & 0xF;	 //��ȡͨ��λ

		if (Channel_Bit < 0 || Channel_Bit > CD4051_CHANNEL_NUM - 1)
				return false;		//�����쳣ͨ����

		OneChannelBit_Analyze(OneDSignal_p, Wheel_PerLine, Channel_Bit, Packet_Head, i, PerLine_IntNum, PerLine_AllNum, ADC_ChannelMap);
	}

	return true;

}

//���ݽ�������Raw
bool One_Raw_Data_Analyze(QString filepath_s,int raw_num)
{

	qDebug("���ڽ�����\t%d\t��raw�ļ�:", raw_num);
	filepath_s[filepath_s.size() - 5] = (raw_num - 1) + '0';
	qDebug() << QString(filepath_s).toStdString().c_str();

	int LineNum_CPU;
	int FalseDataFlg;
	char *temp_p;
	QFile my_filePath(filepath_s);
	if (!my_filePath.open(QFile::ReadOnly))
	{
		//���ļ�ʧ��
		qDebug() << "Raw�ļ���ʧ��!";
		return false;
	}
	temp_p = new char[sizeof(unsigned char)*my_filePath.size()];
	QDataStream input(&my_filePath);
	input.readRawData(temp_p, my_filePath.size());
	my_filePath.close();

	LineNum_CPU = my_filePath.size() / ONE_LINE_BYTE_NUM;	//����������
	FalseDataFlg = Data_Analyze(raw_num - 1, temp_p, LineNum_CPU, ONE_LINE_BYTE_NUM);		//��һ�����ݽ���
	delete temp_p;
	if (FalseDataFlg == false)
	{
		QMessageBox::critical(NULL, "����~", "ע�⣺����CD4051����ͨ���Ŵ���!", QMessageBox::Ok, QMessageBox::Ok);
		qDebug("ע�⣺����CD4051����ͨ���Ŵ���");
		return false;
	}
	
	return true;
}