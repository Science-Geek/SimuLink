// 多光谱鉴伪.cpp : 定义控制台应用程序的入口点。
//
//#include <stdafx.h>

#include "Common.h"
#include "CurrencyType.h"
#include "ForCur.h"
#include "stdio.h" 
#include "string.h" 

int ValueVersionDirectionMain(PicDataInfo_ *_pic_info, IplImage_ *img_up, IplImage_ *img_bottom)
{
	int ret = 0;

	CashVVD_AT _pSimilary = { 0 };

	ret = DCTParInit();
	ret = TemplateSet(_pic_info->cash_type);

	if (ret = ImageDCT_Det(&_pSimilary, img_bottom, img_up, _pic_info) != 0)
	{
		//printf("发生错误！错误代码： %d \r\n", ret);
		return RET_ERROR;
	}
	else
	{
		_pic_info->value = _pSimilary.cash.value;
		if (_pic_info->cash_type == CURRENCY_TYPE_RMB)
		{
			if (_pSimilary.cash.version == 1) _pic_info->version = 90;
			if (_pSimilary.cash.version == 2) _pic_info->version = 99;
			if (_pSimilary.cash.version == 3) _pic_info->version = 05;
			if (_pSimilary.cash.version == 4) _pic_info->version = 15;
			if (_pSimilary.cash.version == 5) _pic_info->version = 1512;
		}
		else
		{
			_pic_info->version = _pSimilary.cash.version;
		}

		_pic_info->direction = _pSimilary.cash.direction;
	}

	return 1;
}