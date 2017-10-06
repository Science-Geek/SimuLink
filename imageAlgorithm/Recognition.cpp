/***************************************************************
* Copyright (C) 2011, Wuhan University
* All rights reserved.
*
* 文件名：	Recognition.c
* 功能描述：初始化以及图片操作
*
* 当前版本：1.0
* 作    者：
* 完成日期：2016年9月26日
*
***************************************************************/
//#include <stdafx.h>
#include <qdebug.h>
#include <qfile.h>

#include "stdio.h"
#include "windows.h"
#include "wingdi.h"
#include "Recognition.h"

#include "FindCornerAlg/FindCornerExt.h"
#include "CurrencyType.h"
#include "ValueCheckAlg/ImageDCT.h"

#define IMG_CHANNEL 5

#if 0
int loadBitmapFromFile(const char *filePath, U8 **bits, ImgSizeInfo* _imgsize)     //修改过，去掉了全局变量
{
	FILE *fp = fopen(filePath, "rb");
	if (fp == NULL) {
		return 0;
	}
	BITMAPFILEHEADER bfh;
	if (fread(&bfh, sizeof(bfh), 1, fp) != 1) {
		fclose(fp);
		return 0;
	}
	BITMAPINFOHEADER bih;
	if (fread(&bih, sizeof(bih), 1, fp) != 1) {
		fclose(fp);
		return 0;
	}
	if (bih.biBitCount != 8) {
		fclose(fp);
		printf("unsupported bitmap format.\n");
		return 0;
	}
	fseek(fp, bfh.bfOffBits - sizeof(bfh) - sizeof(bih), SEEK_CUR);
	int imageSize = (bih.biWidth + 3) / 4 * 4 * bih.biHeight;
	U8 *buffer = (U8 *)malloc(imageSize);
	_imgsize->width = bih.biWidth;   //添加以适应输入图像的宽度高度随机变化
	_imgsize->height = bih.biHeight;
	_imgsize->width_step = imageSize / bih.biHeight;

	if (fread(buffer, 1, imageSize, fp) != imageSize) {
		fclose(fp);
		return 0;
	}

	fclose(fp);

	*bits = buffer;

	return imageSize;
}

int CreateImage(IplImage_ *_img, int _width, int _height, U8 *_image_data)
{
	if (_img->valid)
	{
		return -1;	//这个图像结构体已经有用了
	}
	if (_width & 3)	//是否是4的整数倍
	{
		_img->width_step = (_width & (~3)) + 4;
	}
	else
	{
		_img->width_step = _width;
	}
	_img->width = _width;
	_img->height = _height;
	_img->image_data = _image_data;
	_img->valid = 1;
	return 0;
}

void ReleaseImage(IplImage_ *_img)
{
	_img->width_step = 0;
	_img->width = 0;
	_img->height = 0;
	_img->image_data = 0;
	_img->valid = 0;
}

void SaveGrayBitmap(const char *fileName, const U8 *imageData, int width, int height)
{
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
	DWORD palette[256];
	int i, imageSize;
	FILE *fp = fopen(fileName, "wb");
	int widthS = (width + 3) / 4 * 4;
	imageSize = widthS* height;


	if (fp == NULL)
	{
		return;
	}

	memset(&bfh, 0, sizeof(bfh));
	bfh.bfType = 0x4d42;
	bfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * 4 + imageSize;
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * 4;
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;
	fwrite(&bfh, sizeof(bfh), 1, fp);

	memset(&bih, 0, sizeof(bih));
	bih.biSize = sizeof(bih);
	bih.biWidth = width;
	bih.biHeight = height;
	bih.biPlanes = 1;
	bih.biBitCount = 8;
	bih.biCompression = 0;
	bih.biSizeImage = imageSize;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;
	fwrite(&bih, sizeof(bih), 1, fp);

	for (i = 0; i < 256; i++)
	{
		palette[i] = (i << 16) | (i << 8) | i;
	}
	fwrite(palette, sizeof(palette), 1, fp);

	for (i = 0; i < height; i++)
	{
		fwrite(imageData + (height - i - 1)*widthS, 1, widthS, fp);
	}

	fclose(fp);
}


//void Image_Alg_Main(LPCTSTR _filePath,PicDataInfo_ *pic_info)
//{
//	char TopImageBuf[256] = { 0 };
//	char BottomImageBuf[256] = { 0 };
//	Getfilename(_filePath, TopImageBuf, BottomImageBuf);
//	
//	U8 *up_data = NULL, *bottom_data = NULL;
//	int ret_up, ret_down;
//	ImgSizeInfo imgsize_org;
//	ret_up = loadBitmapFromFile(TopImageBuf, &up_data, &imgsize_org);
//	ret_down = loadBitmapFromFile(BottomImageBuf, &bottom_data, &imgsize_org);
//
//	if (!(ret_up || ret_down))             //读图失败，不予处理
//	{
//		return;
//	}
//
//	//得到2张原图
//	IplImage_ up_img;
//	IplImage_ down_img;
//	ReleaseImage(&up_img);
//	ReleaseImage(&down_img);
//	CreateImage(&up_img, imgsize_org.width, imgsize_org.height, up_data);
//	CreateImage(&down_img, imgsize_org.width, imgsize_org.height, bottom_data);
//
//	/*************************************找角点（只找一次，所以独立出来）*************************************/
//	
//	ImgDataInit(up_img.image_data, down_img.image_data, pic_info, CURRENCY_TYPE_RMB, &imgsize_org);
//
//	int tri_type[2] = { 0, 0 };
//	CornerCorrectInfo_ corner_info;  //角点信息结构体定义
//	memset(&corner_info, 0, sizeof(corner_info));  //初始化
//	pic_info->missing_info[0] = -1;
//	pic_info->missing_info[1] = -1;
//
//	IR_PicInfo_ Up_IR_info, Down_IR_info;
//	memset(&Up_IR_info, 0, sizeof(Up_IR_info));
//	memset(&Down_IR_info, 0, sizeof(Down_IR_info));
//	int corner_ret = GetAffineCorner(pic_info, &corner_info, &Up_IR_info, &Down_IR_info, tri_type);
//
//	IplImage_ up_gray_DCT;
//	IplImage_ down_gray_DCT;
//	GetAffineParameter(pic_info, tri_type);
//	GetAffineImg(&up_gray_DCT, &down_gray_DCT, pic_info);
//	ValueVersionDirectionMain(pic_info, &up_gray_DCT, &down_gray_DCT);
//
//#ifdef SAVE_VVD_IMG
//	char valuefile[300];
//	char img_patch[100] = "E:\\";
//	sprintf(valuefile, "%s%d_up_grey.bmp", img_patch, j);
//	SaveGrayBitmap(valuefile, up_gray_DCT.image_data, up_gray_DCT.width, up_gray_DCT.height);
//	sprintf(valuefile, "%s%d_down_grey.bmp", img_patch, j);
//	SaveGrayBitmap(valuefile, down_gray_DCT.image_data, down_gray_DCT.width, down_gray_DCT.height);
//#endif
//
//	//版本补刀算法
//	if (pic_info->cash_type == CURRENCY_TYPE_RMB)
//	{
//		ImgDataInit(up_img.image_data, down_img.image_data, pic_info, CURRENCY_TYPE_RMB, &imgsize_org);
//		GetAffineParameter(pic_info, tri_type);
//		VersionRecheck(pic_info);
//	}
//
//	free(up_data);
//	free(bottom_data);
//
//	return;
//}



void Getfilename(QString _filePath, char TopImageBuf[], char BottomImageBuf[])
{
	char FileName[256] = { 0 };
	char* tmp_p = NULL;

	char FilePathStr[256] = { 0 }; //
	//int num = WideCharToMultiByte(CP_OEMCP, NULL, _filePath, -1, NULL, 0, NULL, FALSE);
	//WideCharToMultiByte(CP_OEMCP, NULL, _filePath, -1, FilePathStr, num, NULL, FALSE);

	char tmpBuf[] = "adc0.raw";
	//ir image pos 
	memcpy(TopImageBuf, FilePathStr, strlen(FilePathStr));
	memcpy(BottomImageBuf, FilePathStr, strlen(FilePathStr));

	tmp_p = strstr(TopImageBuf, tmpBuf);  //char *strstr(const char *str1, const char *str2);找出str2字符串在str1字符串中第一次出现的位置（不包括str2的串结束符）。返回该位置的指针，如找不到，返回空指针。
	if (tmp_p != NULL)
	{
		memcpy(tmp_p, "cis_top.bmp", strlen("cis_top.bmp"));
	}
	tmp_p = strstr(BottomImageBuf, tmpBuf);
	if (tmp_p != NULL)
	{
		memcpy(tmp_p, "cis_bottom.bmp", strlen("cis_bottom.bmp"));
	}
	//TRACE("top:%s\n", TopImageBuf);
	//TRACE("bottom:%s\n", BottomImageBuf);

}
#endif
#if (IMG_CHANNEL == 6)
int PackAllWheels(PicDataInfo_ *pic_info)
{
	unsigned *upcis = (unsigned *)pic_info->up_data;
	unsigned r_off = pic_info->R_img_offset >> 2;
	unsigned g_off = pic_info->G_img_offset >> 2;
	unsigned b_off = pic_info->B_img_offset >> 2;
	unsigned ir_off = pic_info->IR_img_offset[CIS_T] >> 2;
	unsigned tr_off = pic_info->TR_img_offset[CIS_T] >> 2;
	unsigned oir_off = pic_info->UV_img_offset >> 2;
	unsigned width = pic_info->width >> 2;
	unsigned hight = pic_info->height;
	unsigned limit = sizeof(pic_info->all_wheels) / (pic_info->channle_num*sizeof(unsigned));
	unsigned l_off = 189;
	unsigned w_off = 188;
	unsigned tmp, errcnt = 0, lastw = 0;

	if (hight > limit) hight = limit;

	/* used by master analysis */
	for (int i = 0; i < hight; ++i)
	{
		tmp = upcis[width*i + r_off + w_off];
		if (tmp < lastw) ++errcnt;
		pic_info->all_wheels[6 * i + 0] = lastw = tmp;
		tmp = upcis[width*i + g_off + w_off];
		if (tmp < lastw) ++errcnt;
		pic_info->all_wheels[6 * i + 1] = lastw = tmp;
		tmp = upcis[width*i + b_off + w_off];
		if (tmp < lastw) ++errcnt;
		pic_info->all_wheels[6 * i + 2] = lastw = tmp;
		tmp = upcis[width*i + ir_off + w_off];
		if (tmp < lastw) ++errcnt;
		pic_info->all_wheels[6 * i + 3] = lastw = tmp;
		tmp = upcis[width*i + tr_off + w_off];
		if (tmp < lastw) ++errcnt;
		pic_info->all_wheels[6 * i + 4] = lastw = tmp;
		tmp = upcis[width*i + oir_off + w_off];
		if (tmp < lastw) ++errcnt;
		pic_info->all_wheels[6 * i + 5] = lastw = tmp;
	}

	return 1;
}
#elif (IMG_CHANNEL == 5)

int PackAllWheels(PicDataInfo_ *pic_info)
{
	unsigned *upcis = (unsigned *)pic_info->up_data;
	unsigned r_off = pic_info->R_img_offset >> 2;
	unsigned g_off = pic_info->G_img_offset >> 2;
	unsigned b_off = pic_info->B_img_offset >> 2;
	unsigned ir_off = pic_info->IR_img_offset[CIS_T] >> 2;
	unsigned tr_off = pic_info->TR_img_offset[CIS_T] >> 2;
	//  unsigned oir_off = PicDataInfo.UV_img_offset >> 2;
	unsigned width = pic_info->width >> 2;
	unsigned hight = pic_info->height;
	unsigned limit = sizeof(pic_info->all_wheels) / (pic_info->channle_num * sizeof(unsigned));
	unsigned l_off = 189;
	unsigned w_off = 188;
	unsigned tmp, errcnt = 0, lastw = 0;

	if (hight > limit) hight = limit;

	/* used by master analysis */
	for (int i = 0; i < hight; ++i)
	{
		tmp = upcis[width*i + r_off + w_off];
		if (tmp < lastw) ++errcnt;
		pic_info->all_wheels[5 * i + 0] = lastw = tmp;

		tmp = upcis[width*i + g_off + w_off];
		if (tmp < lastw) ++errcnt;
		pic_info->all_wheels[5 * i + 1] = lastw = tmp;

		tmp = upcis[width*i + b_off + w_off];
		if (tmp < lastw) ++errcnt;
		pic_info->all_wheels[5 * i + 2] = lastw = tmp;

		tmp = upcis[width*i + ir_off + w_off];
		if (tmp < lastw) ++errcnt;
		pic_info->all_wheels[5 * i + 3] = lastw = tmp;

		tmp = upcis[width*i + tr_off + w_off];
		if (tmp < lastw) ++errcnt;
		pic_info->all_wheels[5 * i + 4] = lastw = tmp;

		//      tmp = upcis[width*i + oir_off + w_off];
		//      if (tmp < lastw) ++errcnt;
		//      PicDataInfo.all_wheels[6*i+5] = lastw = tmp;
	}

	return 0;
}
#endif
int CreateImage(IplImage_ *_img, int _width, int _height, U8 *_image_data)
{
	if (_img->valid)
	{
		return -1;	//这个图像结构体已经有用了
	}
	if (_width & 3)	//是否是4的整数倍
	{
		_img->width_step = (_width & (~3)) + 4;
	}
	else
	{
		_img->width_step = _width;
	}
	_img->width = _width;
	_img->height = _height;
	_img->image_data = _image_data;
	_img->valid = 1;
	return 0;
}

void ReleaseImage(IplImage_ *_img)
{
	_img->width_step = 0;
	_img->width = 0;
	_img->height = 0;
	_img->image_data = 0;
	_img->valid = 0;
}

int loadBitmapFromFile(QString qfilePath, U8 **bits, ImgSizeInfo* _imgsize)     
{
	QFile my_filePath(qfilePath);
	if (!my_filePath.open(QFile::ReadOnly))
	{
		//打开文件失败
		qDebug() << "门打不开!";
		return false;
	}
	const char* filePath;
	QByteArray ba = qfilePath.toLocal8Bit().data();
	filePath = ba.data();

	FILE *fp = fopen(filePath, "rb");
	if (fp == NULL) {
		return 0;
	}

	BITMAPFILEHEADER bfh;
	if (fread(&bfh, sizeof(bfh), 1, fp) != 1) {
		fclose(fp);
		return 0;
	}
	BITMAPINFOHEADER bih;
	if (fread(&bih, sizeof(bih), 1, fp) != 1) {
		fclose(fp);
		return 0;
	}
	if (bih.biBitCount != 8) {
		fclose(fp);
		printf("unsupported bitmap format.\n");
		return 0;
	}
	fseek(fp, bfh.bfOffBits - sizeof(bfh)-sizeof(bih), SEEK_CUR);
	int imageSize = (bih.biWidth + 3) / 4 * 4 * bih.biHeight;
	U8 *buffer = (U8 *)malloc(imageSize);
	_imgsize->width = bih.biWidth;   //添加以适应输入图像的宽度高度随机变化
	_imgsize->height = bih.biHeight / IMG_CHANNEL;
	_imgsize->width_step = imageSize / bih.biHeight;
	_imgsize->channel_offset = _imgsize->width * _imgsize->height;
	_imgsize->chnl_num = IMG_CHANNEL;

	if (fread(buffer, 1, imageSize, fp) != imageSize) {
		fclose(fp);
		return 0;
	}

	fclose(fp);

	*bits = buffer;

	return imageSize;
}

void Image_Alg_Manager(QString _filePath, PicDataInfo_ *pic_info)
{
	/*从文件路径中取文件名，并根据各参数得到对应的信息*/
	QStringList filePath_List = _filePath.split("_");
	filePath_List[filePath_List.size() - 1] = "cis_top.bmp";
	QString topCisfilePath = filePath_List.join("_");					//取文件名 cis_top.bmp
	filePath_List[filePath_List.size() - 1] = "cis_bottom.bmp";
	QString bottomCisfilePath = filePath_List.join("_");			//取文件名 cis_bottom.bmp

	//Getfilename(_filePath, TopImageBuf, BottomImageBuf);

	U8 *up_data = NULL, *bottom_data = NULL;
	int ret_up, ret_down;
	ImgSizeInfo imgsize_org;
	memset(&imgsize_org, 0, sizeof(ImgSizeInfo));
	ret_up = loadBitmapFromFile(topCisfilePath, &up_data, &imgsize_org);
	ret_down = loadBitmapFromFile(bottomCisfilePath, &bottom_data, &imgsize_org);

	if (!(ret_up || ret_down))             //读图失败，不予处理
	{
		return;
	}

	//得到2张原图
	IplImage_ up_img;
	IplImage_ down_img;
	ReleaseImage(&up_img);
	ReleaseImage(&down_img);
	CreateImage(&up_img, imgsize_org.width, imgsize_org.height, up_data);
	CreateImage(&down_img, imgsize_org.width, imgsize_org.height, bottom_data);

	/*************************************找角点（只找一次，所以独立出来）*************************************/

	ImgDataInit(up_img.image_data, down_img.image_data, pic_info, CURRENCY_TYPE_RMB, &imgsize_org);
	PackAllWheels(pic_info);
	int tri_type[2] = { 0, 0 };
	CornerCorrectInfo_ corner_info;  //角点信息结构体定义
	memset(&corner_info, 0, sizeof(corner_info));  //初始化
	pic_info->missing_info[0] = -1;
	pic_info->missing_info[1] = -1;

	IR_PicInfo_ Up_IR_info, Down_IR_info;
	memset(&Up_IR_info, 0, sizeof(Up_IR_info));
	memset(&Down_IR_info, 0, sizeof(Down_IR_info));
	int corner_ret = GetAffineCorner(pic_info, &corner_info, &Up_IR_info, &Down_IR_info, tri_type);

	IplImage_ up_gray_DCT;
	IplImage_ down_gray_DCT;

	GetAffineParameter(pic_info, tri_type);
	GetAffineImg(&up_gray_DCT, &down_gray_DCT, pic_info);
	//ValueVersionDirectionMain(pic_info, &up_gray_DCT, &down_gray_DCT);

#ifdef SAVE_VVD_IMG
	char valuefile[300];
	char img_patch[100] = "E:\\";
	sprintf(valuefile, "%s%d_up_grey.bmp", img_patch, j);
	SaveGrayBitmap(valuefile, up_gray_DCT.image_data, up_gray_DCT.width, up_gray_DCT.height);
	sprintf(valuefile, "%s%d_down_grey.bmp", img_patch, j);
	SaveGrayBitmap(valuefile, down_gray_DCT.image_data, down_gray_DCT.width, down_gray_DCT.height);
#endif

#if 0
	//版本补刀算法
	if (pic_info->cash_type == CURRENCY_TYPE_RMB)
	{
		ImgDataInit(up_img.image_data, down_img.image_data, pic_info, CURRENCY_TYPE_RMB, &imgsize_org);
		GetAffineParameter(pic_info, tri_type);
		VersionRecheck(pic_info);
	}
#endif

	//free(up_data);
	//free(bottom_data);


	return;
}


