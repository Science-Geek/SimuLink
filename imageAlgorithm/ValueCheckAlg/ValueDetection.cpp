
//#include <stdafx.h>

#include "stdio.h"
#include<math.h>
#include "string.h"
#include "../Recognition.h"
#include "ForCur.h"


//1元和5元面额识别的参数
int valuesvm[52] =
{ -100746, 81152, 31301, -45466, -102276, -127176, -95622, 32387, -21889, 230644, 55828, -2571, -812,
237045, 114306, 89889, 16374, 19668, -54009, -164485, -164595, -194368, 220324, 232477, 5224, -140,
136065, 115972, 31766, -34187, -81175, -112573, -95640, 7458, -4459, 175560, 81636, -467, -90, -19203,
245685, 113697, -63199, 5831, -33084, -118857, -136494, -180301, 166035, 214771, 12501, 38
};

//float valuesvm[52] =
//{ -1.537255e+00, 1.238288e+00, 4.776162e-01, -6.937585e-01, -1.560604e+00, -1.940546e+00, -1.459082e+00, 4.941929e-01, -3.340054e-01, 3.519341e+00, 8.518669e-01, -3.922850e-02, -1.239010e-02, 3.617017e+00, 1.744167e+00, 1.371605e+00, 2.498443e-01, 3.001080e-01, -8.241076e-01, -2.509841e+00, -2.511521e+00, -2.965824e+00, 3.361873e+00, 3.547313e+00, 7.971636e-02, -2.139015e-03, 2.076181e+00, 1.769598e+00, 4.847077e-01, -5.216583e-01, -1.238631e+00, -1.717725e+00, -1.459353e+00, 1.138002e-01, -6.804402e-02, 2.678833e+00, 1.245662e+00, -7.120588e-03, -1.377096e-03, -2.930220e-01, 3.748858e+00, 1.734885e+00, -9.643332e-01, 8.896963e-02, -5.048238e-01, -1.813617e+00, -2.082727e+00, -2.751177e+00, 2.533491e+00, 3.277145e+00, 1.907535e-01, 5.758531e-04
//};

int ComputeRandGHist(IplImage_* image, int histfeature[13])
{
	int i, j;

	U8 *imagedata = image->image_data;
	int width = image->width;
	int height = image->height;
	int widthstep = image->width_step;
	long long graynum = 0;
	int allnum = 0;
	int average;

// 统计均值，防止亮度影响直方图的分布
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			if (imagedata[i*widthstep + j] != 0)
			{
				graynum += imagedata[i*widthstep + j];
				allnum++;
			}
		}
	}

	if (allnum == 0)
	{
		return -1;
	}
	average = graynum / allnum;

	// 统计位于-100~160范围内的像素的点数，作为特征，其中有13个bin。
	int bin[13];
	memset(bin, 0, sizeof(bin));
	int data;
	int k = 20;
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			data = imagedata[i*widthstep + j];
			if (data != 0)
			{
				data -= average;
				if (data >= k * (-5) && data < k * (-4))
				{
					bin[0]++;
				}
				else if (data >= k * (-4) && data < k * (-3))
				{
					bin[1]++;
				}
				else if (data >= k * (-3) && data < k * (-2))
				{
					bin[2]++;
				}
				else if (data >= k * (-2) && data < k * (-1))
				{
					bin[3]++;
				}
				else if (data >= k * (-1) && data < k * (0))
				{
					bin[4]++;
				}
				else if (data >= k * (0) && data < k * 1)
				{
					bin[5]++;
				}
				else if (data >= k * 1 && data < k * 2)
				{
					bin[6]++;
				}
				else if (data >= k * 2 && data < k * 3)
				{
					bin[7]++;
				}
				else if (data >= k * 3 && data < k * 4)
				{
					bin[8]++;
				}
				else if (data >= k * 4 && data < k * 5)
				{
					bin[9]++;
				}
				else if (data >= k * 5 && data < k * 6)
				{
					bin[10]++;
				}
				else if (data >= k * 6 && data < k * 7)
				{
					bin[11]++;
				}
				else if (data >= k * 7 && data < k * 8)
				{
					bin[12]++;
				}
			}

		}
	}
	long long score = 0;
	for (i = 0; i < 13; i++)
	{
		score = bin[i];
		score = score << 12;
		histfeature[i] = score / allnum;
	}

	return 0;
}

/******************
函数名：ValueDetection
输入：PicDataInfo结构体
功能：1元和5元面额判断的补刀函数。
主要思想：利用R和G通道统计直方图的方法，统计出13个bin内的像素个数，然后进行svm训练。
在应用层直接调用此函数即可判断1元还是5元。
***********************/
int ValueDetection(PicDataInfo_ *PicDataInfo)
{

	U8* Y_R_Img = PicDataInfo->up_data + PicDataInfo->R_img_offset;
	U8* U_R_Img = PicDataInfo->bottom_data + PicDataInfo->R_img_offset;

	U8* Y_G_Img = PicDataInfo->up_data + PicDataInfo->G_img_offset;
	U8* U_G_Img = PicDataInfo->bottom_data + PicDataInfo->G_img_offset;

	IplImage_ pImgR_Y;
	IplImage_ pImgR_U;
	IplImage_ pImgG_Y;
	IplImage_ pImgG_U;

	ReleaseImage(&pImgR_Y);
	ReleaseImage(&pImgR_U);
	ReleaseImage(&pImgG_Y);
	ReleaseImage(&pImgG_U);

	CreateImage(&pImgR_Y, PicDataInfo->width, PicDataInfo->height, Y_R_Img);
	CreateImage(&pImgR_U, PicDataInfo->width, PicDataInfo->height, U_R_Img);
	CreateImage(&pImgG_Y, PicDataInfo->width, PicDataInfo->height, Y_G_Img);
	CreateImage(&pImgG_U, PicDataInfo->width, PicDataInfo->height, U_G_Img);

	pImgR_Y.width_step = pImgR_U.width_step = PicDataInfo->width_step;
	pImgG_Y.width_step = pImgG_U.width_step = PicDataInfo->width_step;

	int error = 0;
	int allnum = PicDataInfo->width * PicDataInfo->height;
	int histfeature[52];
	memset(histfeature, 0, sizeof(histfeature));

	//提取特征
	error = ComputeRandGHist(&pImgR_Y, histfeature);
	if (error < 0)
	{
		return -1;
	}
	error = ComputeRandGHist(&pImgG_Y, histfeature + 13);
	if (error < 0)
	{
		return -1;
	}
	error = ComputeRandGHist(&pImgR_U, histfeature + 26);
	if (error < 0)
	{
		return -1;
	}
	error = ComputeRandGHist(&pImgG_U, histfeature + 39);
	if (error < 0)
	{
		return -1;
	}

	//计算得分，如果大于0，则是5元；小于0，则是1元。
	int i = 0;
	int score = 0;
	for (i = 0; i < 52; i++)
	{
		score += ((histfeature[i] * valuesvm[i]) >> 20);
		//score += ((histfeature[i] * valuesvm[i]));
	}
	//score = score >> 16;
	if (score > 0)
	{
		PicDataInfo->value = 5;
	}
	else
	{
		PicDataInfo->value = 1;
	}
	return 0;
}