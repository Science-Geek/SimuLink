/***************************************************************
* Copyright (C) 2011, Wuhan University
* All rights reserved.
*
* 文件名：	Common.c
* 功能描述: 辅助工具函数
*
* 当前版本：1.0
* 作    者：张维夏
* 完成日期：2016年9月20日
*
***************************************************************/

//#include <stdafx.h>

#include "Common.h"
#include "string.h"
#include "windows.h"


void InversrImage(IplImage_* pSrcImg, IplImage_* pDstImg)	//由于BMP数据时反的，实际计算数据需要反转
{
	int i0, j0;
	for (i0 = 0; i0 < pSrcImg->height; i0++)
	{
		for (j0 = 0; j0 < pSrcImg->width; j0++)
		{
			pDstImg->image_data[i0*pDstImg->width_step + j0] = pSrcImg->image_data[(pSrcImg->height - i0 - 1)*pSrcImg->width_step + j0];
			//pRecv_Y->imageData[i0*pRecv_Y->widthStep+j0] = pImg_Y->imageData[i0*pRecv_Y->widthStep+j0];
		}
	}
}

//字符串转换成币种类型
int Str2CurrType(char *pStr, CurrencyType_ *pCurrencyType_Ret)
{
	CurrencyType_ CurrencyType_Ret;
	if (strcmp(pStr, "HKD") == 0)
	{
		CurrencyType_Ret = HKD;
	}
	else if (strcmp(pStr, "GBP") == 0)
	{
		CurrencyType_Ret = GBP;
	}
	else if (strcmp(pStr, "CNY") == 0)
	{
		CurrencyType_Ret = CNY;
	}
	else if (strcmp(pStr, "BRL") == 0)
	{
		CurrencyType_Ret = BRL;
	}
	else if (strcmp(pStr, "EURO") == 0)
	{
		CurrencyType_Ret = EURO;
	}
	else if (strcmp(pStr, "AUD") == 0)
	{
		CurrencyType_Ret = AUD;
	}
	else if (strcmp(pStr, "SGD") == 0)
	{
		CurrencyType_Ret = SGD;
	}
	else if (strcmp(pStr, "CAD") == 0)
	{
		CurrencyType_Ret = CAD;
	}
	else if (strcmp(pStr, "CHF") == 0)
	{
		CurrencyType_Ret = CHF;
	}
	else if (strcmp(pStr, "RUB") == 0)
	{
		CurrencyType_Ret = RUB;
	}
	else if (strcmp(pStr, "KRW") == 0)
	{
		CurrencyType_Ret = KRW;
	}
	else if (strcmp(pStr, "PHP") == 0)
	{
		CurrencyType_Ret = PHP;
	}
	else if (strcmp(pStr, "TWD") == 0)
	{
		CurrencyType_Ret = TWD;
	}
	else if (strcmp(pStr, "THB") == 0)
	{
		CurrencyType_Ret = THB;
	}
	else if (strcmp(pStr, "DKK") == 0)
	{
		CurrencyType_Ret = DKK;
	}
	else if (strcmp(pStr, "NOK") == 0)
	{
		CurrencyType_Ret = NOK;
	}
	else if (strcmp(pStr, "SEK") == 0)
	{
		CurrencyType_Ret = SEK;
	}
	else if (strcmp(pStr, "NZD") == 0)
	{
		CurrencyType_Ret = NZD;
	}
	else if (strcmp(pStr, "ETB") == 0)
	{
		CurrencyType_Ret = ETB;
	}
	else if (strcmp(pStr, "SAR") == 0)
	{
		CurrencyType_Ret = SAR;
	}
	else if (strcmp(pStr, "PGK") == 0)
	{
		CurrencyType_Ret = PGK;
	}
	else if (strcmp(pStr, "MOP") == 0)
	{
		CurrencyType_Ret = MOP;
	}
	else if (strcmp(pStr, "ZAR") == 0)
	{
		CurrencyType_Ret = ZAR;
	}
	else if (strcmp(pStr, "USD") == 0)
	{
		CurrencyType_Ret = USD;
	}
	else if (strcmp(pStr, "JPY") == 0)
	{
		CurrencyType_Ret = JPY;
	}
	else if (strcmp(pStr, "BUK") == 0)
	{
		CurrencyType_Ret = BUK;
	}
	else
	{
		return RET_ERROR;
	}
	memcpy(pCurrencyType_Ret, &CurrencyType_Ret, sizeof(CurrencyType_));
	return RET_OK;
}
