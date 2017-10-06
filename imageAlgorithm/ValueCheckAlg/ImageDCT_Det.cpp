/***************************************************************
* Copyright (C) 2011, Wuhan University
* All rights reserved.
*
* 文件名：	ImageDCT_Det.c
* 功能描述: 图像DCT面额版本方向检测函数
*
* 当前版本：1.0
* 作    者：张维夏
* 完成日期：2016年9月20日
*
***************************************************************/
//#include <stdafx.h>

#include <stdio.h>
#include "string.h"
#include "ForCur.h"
#include "CurrencyType.h"
#include "../FindCornerAlg/FindCornerExt.h"



DCTHanMingCode gExincludeTemplate[MAX_CONFIG_NUM * 4];

//参数总配置结构体
ParConfigUnion_ gParConfigUnion;
//需要保存在配置文件中的参数总配置结构体
ParConfigUnionSave_ gParConfigUnionSave;

ParConfigCurrency_ ParConfigCurrency;

static unsigned char ZONE_IR[200 * 200];      //图像缓存：第三通道
static unsigned char ZONE_IR2[200 * 200];      //图像缓存：第三通道


int ImageDCT_Det(CashVVD_AT* _pSimilary, IplImage_* pImg_bottom, IplImage_* pImg_up, PicDataInfo_ *_pic_Info)
{
	DCTHanMingCode pHM;
	DCTHanMingPattern gHMPattern;
	int thred = 0;

	memset(&gHMPattern, 0, sizeof(DCTHanMingPattern));
	memset(&pHM, 0, sizeof(DCTHanMingCode));

	gParConfigUnion.pParConfigUnionSave = (ParConfigUnionSave_*)&gParConfigUnionSave;

	gHMPattern.validnum = 4 * gParConfigUnionSave.ValidTemplateNum;
	gHMPattern.pattern = gExincludeTemplate;

	if (GetFeatureInfo(&pHM, pImg_bottom, pImg_up) >= 0)
	{
		ImgDCTParttenMatch(_pSimilary, &gHMPattern, &pHM);
		//ImgDCTMatchWithValue(_pSimilary, &gHMPattern, &pHM, /*_pic_Info->value*/100);   //给定面额下的模板匹配
	}
	else
	{
		return RET_ERROR;
	}

	thred = EXCLUDE_THRESHOLD;
	//thred = 99999999;
	if (_pSimilary->cash.value == 1 && strcmp(gParConfigUnionSave.CurrencyName, "USD") == 0)
	{
		thred = EXCLUDE_THRESHOLD + 1000;
	}
	else if (strcmp(gParConfigUnionSave.CurrencyName, "BUK") == 0)
	{
		thred = EXCLUDE_THRESHOLD + 500;
	}
	else if (strcmp(gParConfigUnionSave.CurrencyName, "AED") == 0)
	{
		thred = EXCLUDE_THRESHOLD + 1000;
	}

	if (_pSimilary[0].unsimilary > thred)
	{
		_pSimilary->cash.value = 0;
		_pSimilary->cash.version = 0;
		_pSimilary->cash.direction = -1;
		return IMGCUR_ERRORTYPE_OTHER;
	}
	else
	{
		//printf("最小不相似度:%d ", _pSimilary[0].unsimilary);
		return RET_OK;
	}
}

void VersionRecheck(PicDataInfo_ *_pic_info)
{
	if (_pic_info->cash_type == CURRENCY_TYPE_RMB)
	{
		if (_pic_info->version == 90 || _pic_info->version == 1512 || _pic_info->value == 1 || _pic_info->value == 0)   //90版，航天币，1元以及非本类币无需补刀
		{
			return;
		}

		RotateZoneInfo_ zone_info, zone_info1;
		U8* p_up_ir = _pic_info->up_data + _pic_info->IR_img_offset;			//下面ir图
		U8* p_bottom_ir = _pic_info->bottom_data + _pic_info->IR_img_offset;	//下面ir图
		IplImage_ img_up_ir_src;  //创建单通道图像数组
		ReleaseImage(&img_up_ir_src);
		CreateImage(&img_up_ir_src, _pic_info->width, _pic_info->height, p_up_ir);    //创建图像
		img_up_ir_src.width_step = _pic_info->width_step;
		IplImage_ img_bottom_ir_src;
		ReleaseImage(&img_bottom_ir_src);
		CreateImage(&img_bottom_ir_src, _pic_info->width, _pic_info->height, p_bottom_ir);    //创建bottom图ir图像
		img_bottom_ir_src.width_step = _pic_info->width_step;
		int version = 0;


		if (_pic_info->value == 5)
		{
			zone_info.xstart = 98;
			zone_info.ystart = 10;
			zone_info.width = 72;
			zone_info.height = 28;
			IplImage_ img_ir1;
			ReleaseImage(&img_ir1);
			CreateImage(&img_ir1, zone_info.width, zone_info.height, ZONE_IR);

			zone_info1.xstart = 362;
			zone_info1.ystart = 14;
			zone_info1.width = 52;
			zone_info1.height = 28;
			IplImage_ img_ir2;
			ReleaseImage(&img_ir2);
			CreateImage(&img_ir2, zone_info1.width, zone_info1.height, ZONE_IR2);

			if (_pic_info->direction == 2 || _pic_info->direction == 3)
			{
				VVDGetZoneCoordinates(&zone_info, _pic_info);
				WarpAffineZoneInt_Samp_One(&img_ir1, &img_up_ir_src, &zone_info, _pic_info->affine_operator_trans_up, 0, 1, 1);
				//SaveGrayBitmap("pGrayInk1.bmp", img_ir1.image_data, img_ir1.width, img_ir1.height);
				VVDGetZoneCoordinates(&zone_info1, _pic_info);
				WarpAffineZoneInt_Samp_One(&img_ir2, &img_up_ir_src, &zone_info1, _pic_info->affine_operator_trans_up, 0, 1, 1);
				//SaveGrayBitmap("pGrayInk2.bmp", img_ir2.image_data, img_ir2.width, img_ir2.height);
			}
			else
			{
				VVDGetZoneCoordinates(&zone_info, _pic_info);
				WarpAffineZoneInt_Samp_One(&img_ir1, &img_bottom_ir_src, &zone_info, _pic_info->affine_operator_trans_bottom, 0, 1, 1);
				//SaveGrayBitmap("pGrayInk1.bmp", img_ir1.image_data, img_ir1.width, img_ir1.height);
				VVDGetZoneCoordinates(&zone_info1, _pic_info);
				WarpAffineZoneInt_Samp_One(&img_ir2, &img_bottom_ir_src, &zone_info1, _pic_info->affine_operator_trans_bottom, 0, 1, 1);
				//SaveGrayBitmap("pGrayInk2.bmp", img_ir2.image_data, img_ir2.width, img_ir2.height);
			}
			//version = _pic_info->version;
			_pic_info->version = VersionDect2(_pic_info, &img_ir1, &img_ir2);
			//if (_pic_info->version == -1)
			//{
			//	_pic_info->version = version;
			//}

		}
		else if (_pic_info->value == 10)
		{
			zone_info.xstart = 126;
			zone_info.ystart = 20;
			zone_info.width = 72;
			zone_info.height = 32;

			IplImage_ img_ir1;
			ReleaseImage(&img_ir1);
			CreateImage(&img_ir1, zone_info.width, zone_info.height, ZONE_IR);

			zone_info1.xstart = 548;
			zone_info1.ystart = 72;
			zone_info1.width = 32;
			zone_info1.height = 80;
			IplImage_ img_ir2;
			ReleaseImage(&img_ir2);
			CreateImage(&img_ir2, zone_info1.width, zone_info1.height, ZONE_IR2);

			if (_pic_info->direction == 0 || _pic_info->direction == 1)
			{
				VVDGetZoneCoordinates(&zone_info, _pic_info);
				WarpAffineZoneInt_Samp_One(&img_ir1, &img_bottom_ir_src, &zone_info, _pic_info->affine_operator_trans_bottom, 0, 1, 1);
				//SaveGrayBitmap("pGrayInk1.bmp", img_ir1.image_data, img_ir1.width, img_ir1.height);
				VVDGetZoneCoordinates(&zone_info1, _pic_info);
				WarpAffineZoneInt_Samp_One(&img_ir2, &img_up_ir_src, &zone_info1, _pic_info->affine_operator_trans_up, 0, 1, 1);
				//SaveGrayBitmap("pGrayInk2.bmp", img_ir2.image_data, img_ir2.width, img_ir2.height);
			}
			else
			{
				VVDGetZoneCoordinates(&zone_info, _pic_info);
				WarpAffineZoneInt_Samp_One(&img_ir1, &img_up_ir_src, &zone_info, _pic_info->affine_operator_trans_up, 0, 1, 1);
				//SaveGrayBitmap("pGrayInk1.bmp", img_ir1.image_data, img_ir1.width, img_ir1.height);
				VVDGetZoneCoordinates(&zone_info1, _pic_info);
				WarpAffineZoneInt_Samp_One(&img_ir2, &img_bottom_ir_src, &zone_info1, _pic_info->affine_operator_trans_bottom, 0, 1, 1);
				//SaveGrayBitmap("pGrayInk2.bmp", img_ir2.image_data, img_ir2.width, img_ir2.height);
			}

			//version = _pic_info->version;
			_pic_info->version = VersionDect2(_pic_info, &img_ir1, &img_ir2);
			//if (_pic_info->version == -1)
			//{
			//	_pic_info->version = version;
			//}

		}
		else if (_pic_info->value == 20)
		{
			zone_info.xstart = 140;
			zone_info.ystart = 8;
			zone_info.width = 72;
			zone_info.height = 28;
			IplImage_  img_ir1;
			ReleaseImage(&img_ir1);
			CreateImage(&img_ir1, zone_info.width, zone_info.height, ZONE_IR);

			zone_info1.xstart = 546;
			zone_info1.ystart = 60;
			zone_info1.width = 32;
			zone_info1.height = 88;
			IplImage_ img_ir2;
			ReleaseImage(&img_ir2);
			CreateImage(&img_ir2, zone_info1.width, zone_info1.height, ZONE_IR2);

			if (_pic_info->direction == 0 || _pic_info->direction == 1)
			{
				VVDGetZoneCoordinates(&zone_info, _pic_info);
				WarpAffineZoneInt_Samp_One(&img_ir1, &img_bottom_ir_src, &zone_info, _pic_info->affine_operator_trans_bottom, 0, 1, 1);
				//SaveGrayBitmap("pGrayInk1.bmp", img_ir1.image_data, img_ir1.width, img_ir1.height);
				VVDGetZoneCoordinates(&zone_info1, _pic_info);
				WarpAffineZoneInt_Samp_One(&img_ir2, &img_up_ir_src, &zone_info1, _pic_info->affine_operator_trans_up, 0, 1, 1);
				//SaveGrayBitmap("pGrayInk2.bmp", img_ir2.image_data, img_ir2.width, img_ir2.height);
			}
			else
			{
				VVDGetZoneCoordinates(&zone_info, _pic_info);
				WarpAffineZoneInt_Samp_One(&img_ir1, &img_up_ir_src, &zone_info, _pic_info->affine_operator_trans_up, 0, 1, 1);
				//SaveGrayBitmap("pGrayInk1.bmp", img_ir1.image_data, img_ir1.width, img_ir1.height);
				VVDGetZoneCoordinates(&zone_info1, _pic_info);
				WarpAffineZoneInt_Samp_One(&img_ir2, &img_bottom_ir_src, &zone_info1, _pic_info->affine_operator_trans_bottom, 0, 1, 1);
				//SaveGrayBitmap("pGrayInk2.bmp", img_ir2.image_data, img_ir2.width, img_ir2.height); 
			}
			//version = _pic_info->version;
			_pic_info->version = VersionDect2(_pic_info, &img_ir1, &img_ir2);
			//if (_pic_info->version == -1)
			//{
			//	_pic_info->version = version;
			//}
		}
		else if (_pic_info->value == 50)
		{
			zone_info.xstart = 16;
			zone_info.ystart = 12;
			zone_info.width = 128;
			zone_info.height = 32;

			IplImage_ img_ir;
			ReleaseImage(&img_ir);
			CreateImage(&img_ir, zone_info.width, zone_info.height, ZONE_IR);

			if (_pic_info->direction == 0 || _pic_info->direction == 1)
			{
				VVDGetZoneCoordinates(&zone_info, _pic_info);
				WarpAffineZoneInt_Samp_One(&img_ir, &img_up_ir_src, &zone_info, _pic_info->affine_operator_trans_up, 0, 1, 1);
				SaveGrayBitmap("pGrayInk.bmp", img_ir.image_data, img_ir.width, img_ir.height);
			}
			else
			{
				VVDGetZoneCoordinates(&zone_info, _pic_info);
				WarpAffineZoneInt_Samp_One(&img_ir, &img_bottom_ir_src, &zone_info, _pic_info->affine_operator_trans_bottom, 0, 1, 1);
				SaveGrayBitmap("pGrayInk.bmp", img_ir.image_data, img_ir.width, img_ir.height);
			}

			//version = _pic_info->version;
			_pic_info->version = VersionDect(_pic_info, &img_ir, 0);
			//if (_pic_info->version == -1)
			//{
			//	_pic_info->version = version;
			//}
		}
		else
		{
			zone_info.xstart = 160;
			zone_info.ystart = 112;
			zone_info.width = 144;
			zone_info.height = 72;

			IplImage_ img_ir;
			ReleaseImage(&img_ir);
			CreateImage(&img_ir, zone_info.width, zone_info.height, ZONE_IR);

			if (_pic_info->direction == 0 || _pic_info->direction == 1)
			{
				VVDGetZoneCoordinates(&zone_info, _pic_info);
				WarpAffineZoneInt_Samp_One(&img_ir, &img_up_ir_src, &zone_info, _pic_info->affine_operator_trans_up, 0, 1, 1);
				//SaveGrayBitmap("pGrayInk.bmp", img_ir.image_data, img_ir.width, img_ir.height);
			}
			else
			{
				VVDGetZoneCoordinates(&zone_info, _pic_info);
				WarpAffineZoneInt_Samp_One(&img_ir, &img_bottom_ir_src, &zone_info, _pic_info->affine_operator_trans_bottom, 0, 1, 1);
				//SaveGrayBitmap("pGrayInk.bmp", img_ir.image_data, img_ir.width, img_ir.height);
			}

			//version = _pic_info->version;
			_pic_info->version = VersionDect(_pic_info, &img_ir, 1); //此处逻辑有bug，已修复
			//if (_pic_info->version == -1)
			//{
			//	_pic_info->version = version;
			//}

			if (_pic_info->version != 15) //如果不是15版，再进行05和99的判别
			{
				zone_info.xstart = 16;
				zone_info.ystart = 12;
				zone_info.width = 128;
				zone_info.height = 32;


				ReleaseImage(&img_ir);
				CreateImage(&img_ir, zone_info.width, zone_info.height, ZONE_IR);

				if (_pic_info->direction == 0 || _pic_info->direction == 1)
				{
					VVDGetZoneCoordinates(&zone_info, _pic_info);
					WarpAffineZoneInt_Samp_One(&img_ir, &img_up_ir_src, &zone_info, _pic_info->affine_operator_trans_up, 0, 1, 1);
					//SaveGrayBitmap("pGrayInk.bmp", img_ir.image_data, img_ir.width, img_ir.height);
				}
				else
				{
					VVDGetZoneCoordinates(&zone_info, _pic_info);
					WarpAffineZoneInt_Samp_One(&img_ir, &img_bottom_ir_src, &zone_info, _pic_info->affine_operator_trans_bottom, 0, 1, 1);
					//SaveGrayBitmap("pGrayInk.bmp", img_ir.image_data, img_ir.width, img_ir.height);
				}

				_pic_info->version = VersionDect(_pic_info, &img_ir, 0);
				//if (_pic_info->version == -1)
				//{
				//	_pic_info->version = version;
				//}
			}
		}
	}
	else if (_pic_info->cash_type == CURRENCY_TYPE_BUK)
	{
		RotateZoneInfo_ zone_info;
		U8* p_up_ir = _pic_info->up_data + _pic_info->IR_img_offset;			//下面ir图
		U8* p_bottom_ir = _pic_info->bottom_data + _pic_info->IR_img_offset;	//下面ir图
		IplImage_ img_up_ir_src;  //创建单通道图像数组
		ReleaseImage(&img_up_ir_src);
		CreateImage(&img_up_ir_src, _pic_info->width, _pic_info->height, p_up_ir);    //创建图像
		img_up_ir_src.width_step = _pic_info->width_step;
		IplImage_ img_bottom_ir_src;
		ReleaseImage(&img_bottom_ir_src);
		CreateImage(&img_bottom_ir_src, _pic_info->width, _pic_info->height, p_bottom_ir);    //创建bottom图ir图像
		img_bottom_ir_src.width_step = _pic_info->width_step;


		int ret = 0;

		if (_pic_info->value == 10000)
		{
			zone_info.xstart = 520;
			zone_info.ystart = 6;
			zone_info.width = 80;
			zone_info.height = 48;
			IplImage_ img_ir;
			ReleaseImage(&img_ir);
			CreateImage(&img_ir, zone_info.width, zone_info.height, ZONE_IR);

			IplImage_ _image_new_bin;
			ReleaseImage(&_image_new_bin);
			CreateImage(&_image_new_bin, zone_info.width, zone_info.height, ZONE_IR2);

			if (_pic_info->direction == 0 || _pic_info->direction == 1)
			{
				VVDGetZoneCoordinates(&zone_info, _pic_info);
				WarpAffineZoneInt_Samp_One(&img_ir, &img_up_ir_src, &zone_info, _pic_info->affine_operator_trans_up, 0, 1, 1);

				ret = Binaryzation_VR(_image_new_bin, img_ir, 0, 1);

				SaveGrayBitmap("pGrayInk1.bmp", img_ir.image_data, img_ir.width, img_ir.height);
				SaveGrayBitmap("newbin.bmp", _image_new_bin.image_data, _image_new_bin.width, _image_new_bin.height);
			}
			else
			{
				VVDGetZoneCoordinates(&zone_info, _pic_info);
				WarpAffineZoneInt_Samp_One(&img_ir, &img_bottom_ir_src, &zone_info, _pic_info->affine_operator_trans_bottom, 0, 1, 1);

				ret = Binaryzation_VR(_image_new_bin, img_ir, 0, 1);
				SaveGrayBitmap("pGrayInk1.bmp", img_ir.image_data, img_ir.width, img_ir.height);
				SaveGrayBitmap("newbin.bmp", _image_new_bin.image_data, _image_new_bin.width, _image_new_bin.height);
			}
			_pic_info->version = VersionDect_BUK(_pic_info, &_image_new_bin);

			//SaveGrayBitmap("pGrayInk1.bmp", img_ir.image_data, img_ir.width, img_ir.height);
			//SaveGrayBitmap("newbin.bmp", _image_new_bin.image_data, _image_new_bin.width, _image_new_bin.height);

		}
	}

}



//版本判别
int VersionDect(PicDataInfo_ *_pic_info, IplImage_ *pImg_src, int _method)
{
	int i = 0, j = 0, SumLeft = 0, SumRight = 0;
	int HisLeft[256] = { 0 }, HisRight[256] = { 0 };
	int SD_left = 0, SD_right = 0;
	int del = 0, del2 = 0;
	int thred = 0;

	if (_method == 0) //针对05版和99版的50、100的判别
	{
		for (i = 0; i < pImg_src->height; i++)
		{
			for (j = 0; j < pImg_src->width; j++)
			{
				if (j < pImg_src->width / 2)
				{
					if (pImg_src->image_data[i*pImg_src->width_step + j] < 50)
					{
						del++;
					}
					else
					{
						SumLeft += pImg_src->image_data[i*pImg_src->width_step + j];
						HisLeft[pImg_src->image_data[i*pImg_src->width_step + j]]++;
					}
				}
				else
				{
					if (pImg_src->image_data[i*pImg_src->width_step + j] < 50)
					{
						del2++;
					}
					else
					{
						SumRight += pImg_src->image_data[i*pImg_src->width_step + j];
						HisRight[pImg_src->image_data[i*pImg_src->width_step + j]]++;
					}
				}
			}
		}
		if (pImg_src->width*pImg_src->height / 2 == del || pImg_src->width*pImg_src->height / 2 == del2)
		{
			return RET_ERROR;
		}
		else
		{ 
			SumLeft /= (pImg_src->width*pImg_src->height / 2 - del);
			SumRight /= (pImg_src->width*pImg_src->height / 2 - del2);
		}
		SD_left = 0;
		SD_right = 0;
		for (i = 0; i < 256; i++)
		{
			SD_left += (i - SumLeft)*(i - SumLeft)*HisLeft[i];
			SD_right += (i - SumRight)*(i - SumRight)*HisRight[i];
		}
		if (del+del2 > 300 && _pic_info->value != 100 && _pic_info->value != 50)
		{
			SD_left /= 10;
			SD_right /= 10;
		}

		//if (del > 300 && (_pic_info->value == 100 || _pic_info->value == 50))
		//{
		//	SD_left /= 2;
		//}


		if (_pic_info->value == 100 || _pic_info->value == 50)
		{
			if (SD_left > SD_right /*&& SumLeft < SumRight*/)
			{
				return 05;
			}
			else
			{
				return 99;
			}
		}
	}
	else if (_method == 1)   //针对15版100的判别（判断是不是15版100，如果是则直接得到结果，如果不是则继续调用method 0进行05和99版的判别）
	{
		for (i = 0; i < pImg_src->height; i++)
		{
			for (j = 0; j < pImg_src->width; j++)
			{
				if (pImg_src->image_data[i*pImg_src->width_step + j] < 5)
				{
					del++;
				}
				else
				{
					SumLeft += pImg_src->image_data[i*pImg_src->width_step + j];
					HisLeft[pImg_src->image_data[i*pImg_src->width_step + j]]++;
				}
			}
		}

		if (pImg_src->width*pImg_src->height == del)
		{
			return RET_ERROR;  //全黑图，有问题
		}
		else
		{
			SumLeft /= (pImg_src->width*pImg_src->height - del);
		}
		SD_left = 0;

		for (i = 0; i < 256; i++)
		{
			SD_left += (i - SumLeft)*(i - SumLeft)*HisLeft[i];
		}
		if (SumLeft == 0)
		{
			SumLeft = 1;
		}

	    thred = SD_left / (SumLeft*SumLeft);		

		if (thred > 600)
		{
			return 15;
		}
		else
		{
			return 5; //如果不是15版，暂先返回5，后续还会再次判断
		}
	}
	else
	{
		for (i = 0; i < pImg_src->height; i++)
		{
			for (j = 0; j < pImg_src->width; j++)
			{
				if (j < pImg_src->width / 2)
				{
					SumLeft += pImg_src->image_data[i*pImg_src->width_step + j];
				}
				else
				{
					SumRight += pImg_src->image_data[i*pImg_src->width_step + j];
				}
			}
		}

		thred = abs(SumLeft - SumRight);
		float ratio = 0;

		if (SumRight == 0)
		{
			SumRight = 1;
		}

		if (SumLeft == 0)
		{
			SumLeft = 1;
		}

		if (SumLeft >= SumRight)
		{
			ratio = (float)SumLeft / (float)SumRight;
		}
		else
		{
			ratio = (float)SumRight / (float)SumLeft;
		}

		if (_pic_info->value == 100 || _pic_info->value == 50)
		{
			if (thred > 450000 || ratio > 2.5)
			{
				return 05;
			}
			else
			{
				return 99;
			}
		}
	}

	return RET_ERROR;  //全黑图，有问题
}

int VersionDect2(PicDataInfo_ *_pic_info, IplImage_ *pImg_src1, IplImage_ *pImg_src2)
{
	int i = 0, j = 0, SumLeft = 0;
	int HisLeft[256] = { 0 };
	int SD_left = 0;
	int del = 0;
	float thred1, thred2;

	for (i = 0; i < pImg_src1->height; i++)
	{
		for (j = 0; j < pImg_src1->width; j++)
		{
			if (pImg_src1->image_data[i*pImg_src1->width_step + j] < 50)
			{
				del++;
			}
			else
			{
				SumLeft += pImg_src1->image_data[i*pImg_src1->width_step + j];
				HisLeft[pImg_src1->image_data[i*pImg_src1->width_step + j]]++;
			}
		}
	}

	if (pImg_src1->width*pImg_src1->height == del)
	{
		return RET_ERROR;
	}
	else
	{
		SumLeft /= (pImg_src1->width*pImg_src1->height - del);
	}

	SD_left = 0;

	for (i = 0; i < 256; i++)
	{
		SD_left += (i - SumLeft)*(i - SumLeft)*HisLeft[i];
	}

	if (SumLeft == 0)
	{
		SumLeft = 1;
	}

	thred1 = SD_left / SumLeft;

	/////////////////////////////

	memset(HisLeft, 0, sizeof(HisLeft));
	SumLeft = 0;
	del = 0;

	for (i = 0; i < pImg_src2->height; i++)
	{
		for (j = 0; j < pImg_src2->width; j++)
		{
			if (pImg_src2->image_data[i*pImg_src2->width_step + j] < 50)
			{
				del++;
			}
			else
			{
				SumLeft += pImg_src2->image_data[i*pImg_src2->width_step + j];
				HisLeft[pImg_src2->image_data[i*pImg_src2->width_step + j]]++;
			}
		}
	}

	if (pImg_src2->width*pImg_src2->height == del)
	{
		return RET_ERROR;
	}
	else
	{
		SumLeft /= (pImg_src2->width*pImg_src2->height - del);
	}
	SD_left = 0;

	for (i = 0; i < 256; i++)
	{
		SD_left += (i - SumLeft)*(i - SumLeft)*HisLeft[i];
	}

	if (SumLeft == 0)
	{
		SumLeft = 1;
	}

	thred2 = SD_left / SumLeft;


	if (_pic_info->value == 5)
	{
		if (thred1 + thred2 > 9000)
		{
			return 05;
		}
		else
		{
			return 99;
		}
	}
	else if (_pic_info->value == 20)
	{
		if (thred1 + thred2 > 12000)
		{
			return 05;
		}
		else
		{
			return 99;
		}
	}
	else
	{
		if (thred1 + thred2 > 15000)
		{
			return 05;
		}
		else
		{
			return 99;
		}
	}
}

int VVDGetZoneCoordinates(RotateZoneInfo_ *zone_info, PicDataInfo_ *_pic_info)
{
	if (_pic_info->direction == 0 || _pic_info->direction == 2)
	{
		zone_info->ystart = zone_info->ystart + (_pic_info->height - _pic_info->affine_height)*0.5;
		zone_info->xstart = zone_info->xstart + (_pic_info->width - _pic_info->affine_width)*0.5;
		zone_info->direction = 0;
	}
	if (_pic_info->direction == 1 || _pic_info->direction == 3)
	{
		zone_info->ystart = (_pic_info->affine_height - zone_info->ystart - zone_info->height) + (_pic_info->height - _pic_info->affine_height)*0.5;
		zone_info->xstart = (_pic_info->affine_width - zone_info->xstart - zone_info->width) + (_pic_info->width - _pic_info->affine_width)*0.5;
		zone_info->direction = 1;
	}

	return 1;
}


int Binaryzation_VR(IplImage_ _image_new_bin, IplImage_ _image_source, double _offset, int _enable)
{
	register unsigned char *new_image = _image_new_bin.image_data;
	register unsigned char *image = _image_source.image_data;
	int width = _image_new_bin.width;
	int height = _image_new_bin.height;
	int width_step = _image_new_bin.width_step;
	int histogram[256] = { 0 };
	memset(&histogram, 0, 256);
	if ((_image_new_bin.width > _image_source.width) || (_image_new_bin.height > _image_source.height))
	{
		return -1;
	}

	unsigned int th;
	int i;
	int j;
	int k;
	unsigned int thre = 100;
	unsigned char* image_temp;

	for (i = 2; i < height; i++)
	{
		for (j = 2; j < width - 2; j++)
		{
			image_temp = image + i*width_step + j;
			histogram[*image_temp]++;
		}
	}

	int background = 0;
	int front = 0;
	int back_sum = 0;
	int front_sum = 0;
	float back_avg = 0.0;
	float front_avg = 0.0;
	float  total = 0.0;
	float temp = 0.0;

	for (th = 40; th < 220; th++){
		background = 0;
		front = 0;
		back_sum = 0;
		front_sum = 0;
		for (j = 0; j <= th; j++)
		{
			background += histogram[j];
			back_sum += j*histogram[j];
		}

		back_avg = (background == 0) ? 0 : ((float)back_sum / background);

		for (k = th + 1; k < 256; k++)
		{
			front += histogram[k];
			front_sum += k*histogram[k];
		}
		front_avg = (float)(front == 0) ? 0 : ((float)front_sum / front);

		temp = background*front*(front_avg - back_avg)*(front_avg - back_avg);

		if (temp > total)
		{
			total = temp;
			thre = th;
		}
	}

	if (_enable == 1)
	{
		thre -= thre / 10;
	}
	for (i = 0; i < height; i++)
	{
		for (j = 0; j<width; j++)
		{
			*new_image = (*image>(thre)) ? 0 : 255;
			new_image++;
			image++;
		}
		image += width_step - width;
		new_image += width_step - width;
	}
	return thre;
}



void VersionRecheck_ShaChe(PicDataInfo_ *_pic_info)
{
	//if (_pic_info->version == 90 || _pic_info->version == 1512 || _pic_info->value == 1 || _pic_info->value == 0)   //90版，航天币，1元以及非本类币无需补刀
	//{
	//	return;
	//}

	RotateZoneInfo_ zone_info, zone_info1;
	U8* p_up_ir = _pic_info->up_data + _pic_info->IR_img_offset;			//下面ir图
	U8* p_bottom_ir = _pic_info->bottom_data + _pic_info->IR_img_offset;	//下面ir图
	IplImage_ img_up_ir_src;  //创建单通道图像数组
	ReleaseImage(&img_up_ir_src);
	CreateImage(&img_up_ir_src, _pic_info->width, _pic_info->height, p_up_ir);    //创建图像
	img_up_ir_src.width_step = _pic_info->width_step;
	IplImage_ img_bottom_ir_src;
	ReleaseImage(&img_bottom_ir_src);
	CreateImage(&img_bottom_ir_src, _pic_info->width, _pic_info->height, p_bottom_ir);    //创建bottom图ir图像
	img_bottom_ir_src.width_step = _pic_info->width_step;
	int version = 0;
	int ret = 0;

	IplImage_ img_ir;

	if (_pic_info->value == 100)
	{
		zone_info.xstart = 160;
		zone_info.ystart = 112;
		zone_info.width = 144;
		zone_info.height = 72;

		ReleaseImage(&img_ir);
		CreateImage(&img_ir, zone_info.width, zone_info.height, ZONE_IR);

		if (_pic_info->direction == 0 || _pic_info->direction == 1)
		{
			VVDGetZoneCoordinates(&zone_info, _pic_info);
			WarpAffineZoneInt_Samp_One(&img_ir, &img_up_ir_src, &zone_info, _pic_info->affine_operator_trans_up, 0, 1, 1);
			//SaveGrayBitmap("pGrayInk15.bmp", img_ir.image_data, img_ir.width, img_ir.height);
		}
		else
		{
			VVDGetZoneCoordinates(&zone_info, _pic_info);
			WarpAffineZoneInt_Samp_One(&img_ir, &img_bottom_ir_src, &zone_info, _pic_info->affine_operator_trans_bottom, 0, 1, 1);
			//SaveGrayBitmap("pGrayInk15.bmp", img_ir.image_data, img_ir.width, img_ir.height);
		}

		version = _pic_info->version;
		_pic_info->version = VersionDect(_pic_info, &img_ir, 1);
		if (_pic_info->version == -1)
		{
			_pic_info->version = version;
		}
	}

	if (_pic_info->version != 15)
	{
		IplImage_ _image_new_bin;

		if (_pic_info->value == 100)
		{
			zone_info.xstart = 200;
			zone_info.ystart = 80;
			zone_info.width = 140;
			zone_info.height = 56;
		}
		else
		{
			zone_info.xstart = 165;
			zone_info.ystart = 80;
			zone_info.width = 140;
			zone_info.height = 80;
		}

		ReleaseImage(&img_ir);
		CreateImage(&img_ir, zone_info.width, zone_info.height, ZONE_IR);

		ReleaseImage(&_image_new_bin);
		CreateImage(&_image_new_bin, zone_info.width, zone_info.height, ZONE_IR);


		if (_pic_info->direction == 2 || _pic_info->direction == 3)
		{
			VVDGetZoneCoordinates(&zone_info, _pic_info);
			WarpAffineZoneInt_Samp_One(&img_ir, &img_up_ir_src, &zone_info, _pic_info->affine_operator_trans_up, 0, 1, 1);
			//PerspectiveZoneInt_Samp_One(&img_ir, &img_bottom_ir_src, &zone_info, _pic_info->perspective_operator_trans_bottom, 0, 1, 1);
			//SaveGrayBitmap("pGrayInk1.bmp", img_ir.image_data, img_ir.width, img_ir.height);
			ret = Binaryzation_VR(_image_new_bin, img_ir, 0, 0);
			//SaveGrayBitmap("pGrayInk2.bmp", _image_new_bin.image_data, _image_new_bin.width, _image_new_bin.height);
		}
		else
		{
			VVDGetZoneCoordinates(&zone_info, _pic_info);
			WarpAffineZoneInt_Samp_One(&img_ir, &img_bottom_ir_src, &zone_info, _pic_info->affine_operator_trans_bottom, 0, 1, 1);
			//PerspectiveZoneInt_Samp_One(&img_ir, &img_up_ir_src, &zone_info, _pic_info->perspective_operator_trans_up, 0, 1, 1);
			//SaveGrayBitmap("pGrayInk1.bmp", img_ir.image_data, img_ir.width, img_ir.height);
			ret = Binaryzation_VR(_image_new_bin, img_ir, 0, 0);
			//SaveGrayBitmap("pGrayInk2.bmp", _image_new_bin.image_data, _image_new_bin.width, _image_new_bin.height);
		}
		_pic_info->version = VersionDect(_pic_info, &img_ir, 2);

	}
	else
	{
		return; //暂不支持其他面额
	}
}


int VersionDect_BUK(PicDataInfo_ *_pic_info, IplImage_ *pImg_src)
{
	int i = 0, j = 0, SumLeft = 0;
	int HisLeft[256] = { 0 };
	int SD_left = 0;
	int del = 0;
	float thred1;
	int cnt = 0;

	//for (i = 0; i < pImg_src->height; i++)
	//{
	//	for (j = 0; j < pImg_src->width; j++)
	//	{
	//		if (pImg_src->image_data[i*pImg_src->width_step + j] < 50)
	//		{
	//			del++;
	//		}
	//		else
	//		{
	//			SumLeft += pImg_src->image_data[i*pImg_src->width_step + j];
	//			HisLeft[pImg_src->image_data[i*pImg_src->width_step + j]]++;
	//		}
	//	}
	//}

	//if (pImg_src->width*pImg_src->height == del)
	//{
	//	return RET_ERROR;
	//}
	//else
	//{
	//	SumLeft /= (pImg_src->width*pImg_src->height - del);
	//}

	//SD_left = 0;

	//for (i = 0; i < 256; i++)
	//{
	//	SD_left += (i - SumLeft)*(i - SumLeft)*HisLeft[i];
	//}

	//if (SumLeft == 0)
	//{
	//	SumLeft = 1;
	//}

	//thred1 = SD_left / SumLeft;

	//if (thred1 >= 10000)
	//{
	//	return 2;
	//}
	//else
	//{
	//	return 1;
	//}

	for (i = 0; i < pImg_src->height; i++)
	{
		for (j = 0; j < pImg_src->width; j++)
		{
			HisLeft[pImg_src->image_data[i*pImg_src->width_step + j]]++ ;
		}
	}

	if (pImg_src->height * pImg_src->width == 0)
	{
		return -1;
	}
	thred1 = (float)HisLeft[255] / (pImg_src->height * pImg_src->width);
	//printf("%f \n", thred1);
	if (thred1 > 0.2)
	{
		return 2;
	}
	else
	{
		return 1;
	}

}