/***************************************************************
* Copyright (C) 2011, Wuhan University
* All rights reserved.
*
* 文件名：	FindCorner.c
* 功能描述：角点校正模块
*
* 当前版本：1.0
* 作    者：江燕婷
* 完成日期：2016年9月12日
*
***************************************************************/
//#include <stdafx.h>
//#include "cv.h"
//#include "highgui.h"

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <windows.h>

#include "FindCorner.h"
//#include "Recognition.h"



#define CODE_LENGTH   0.6   //中钞信达0.77715
                              //万联1.13

//静态变量定义
static int edge_x[128];     //横边x坐标
static int edge_y[128];     //纵边y坐标
static int edge_x_up[128];    //上边y坐标
static int edge_x_down[128];
static int edge_x_left[128];   //左边x坐标
static int edge_x_right[128];
static int edge_data_buff[128];   //边数据缓存

static int fitting_deviation[100];    //每个点相对直线的偏移
static int fitting_point_valid[100];  //每个点的像素值

static int fitting_difference[100];
static int fitting_targe_diff_mean[100];
static int fitting_targe_num[100];
static int fitting_targe_point[100];

unsigned char data_buffer[1024];
extern unsigned char CropImg[3276800];

extern FILE *fp1;   //全局变量的外部声明
extern float min_len;
extern float max_len;
extern int file_out_range;
extern int file_error;

extern unsigned int filenum;

static int RMB_BACKGROUND_THRED = 50;  //背景阈值（可修改）
static int EDGE_DEGREE_THRED = 25;    //图像最大倾斜阈值（内部）
static float EDGE_REFER_THRED = 1.5;   //边修正-倾角差-阈值（内部）


enum
{
	CORNER_EDGE_UP = 0,
	CORNER_EDGE_DOWN,
	CORNER_EDGE_LEFT,
	CORNER_EDGE_RIGHT
};

enum
{
	TOP_LEFT_POINT = 0,
	TOP_RIGHT_POINT,
	DOWN_RIGHT_POINT,
	DOWN_LEFT_POINT
};

int SetCornerBackgroundThred(int _thred)
{
	RMB_BACKGROUND_THRED = _thred;

	return 0;
}

#ifdef TMS320C6000
#else

#ifdef _MSC_VER
#define INLINE __inline
#else
#define INLINE inline
#endif

#endif

//计算两条边的交点坐标
//*_line_ud为上边或下边,*_line_lr为坐标或右边
INLINE void CalcCrosspoint(EdgeInfo_ *_line_up, EdgeInfo_ *_line_down, EdgeInfo_ *_line_lr, FloatPoint_ *_point_up, FloatPoint_ *_point_down)
{
	float k_temp;  //左右边经过xy坐标转换后的k,b
	float b_temp;

	if (_line_lr->k != 0)   //根据直线方程求交点
	{
		k_temp = 1 / _line_lr->k;   //xy坐标转换
		b_temp = -_line_lr->b / _line_lr->k;
		_point_up->x = (_line_up->b - b_temp) / (k_temp - _line_up->k);  //(bb0[1]-bb0[0]) / (kk0[0]-kk0[1]);
		_point_down->x = (_line_down->b - b_temp) / (k_temp - _line_down->k);  //(bb1[0]-bb0[0]) / (kk0[0]-kk1[0]);
	}
	else
	{
		_point_up->x = _line_lr->b;
		_point_down->x = _line_lr->b;
	}
	_point_up->y = _line_up->k*_point_up->x + _line_up->b;
	_point_down->y = _line_down->k*_point_down->x + _line_down->b;
}

//得到另一面的仿射坐标
//_other_tri[]为另一面的坐标, *_corner_info为角点信息结构体, mapping_offset为CIS偏移
int GetOtherCoordinate(IntPoint_ _other_tri[], CornerCorrectInfo_ *_corner_info, CoordinatesInfo_ *_coordinates1)
{
	//确定两组坐标对应的位置
	if ((abs(_corner_info->left_edge_info.degree - _corner_info->right_edge_info.degree) <= 4)
		|| (_corner_info->left_edge_info.confidence > _corner_info->right_edge_info.confidence))
	{
		_other_tri[0].x = _coordinates1->x[0];
		_other_tri[0].y = _coordinates1->y[0];
		_other_tri[1].x = _coordinates1->x[1];
		_other_tri[1].y = _coordinates1->y[1];
		_other_tri[2].x = _coordinates1->x[3];
		_other_tri[2].y = _coordinates1->y[3];
	}
	else
	{
		_other_tri[0].x = _coordinates1->x[1];
		_other_tri[0].y = _coordinates1->y[1];
		_other_tri[1].x = _coordinates1->x[2];
		_other_tri[1].y = _coordinates1->y[2];
		_other_tri[2].x = _coordinates1->x[3];
		_other_tri[2].y = _coordinates1->y[3];
	}

	return 0;
}

//得到仿射变换用的坐标
//_dst_tri[]为校正后坐标,*_corner_info为角点信息结构体, _affine_width为仿射后宽度, _affine_height为仿射后高度, _src_tri[]为钱的坐标,
void CalcAffineCoordinate(IntPoint_ _dst_tri[], int _width, int _height, int _affine_width, int _affine_height, CoordinatesInfo_ *_coordinates, IntPoint_ _src_tri[], int _tri_type)
{
	_dst_tri[0].y = (_height - _affine_height) / 2;   //确定校正后起点坐标
	_dst_tri[0].x = (_width - _affine_width) / 2;

	//确定两组坐标对应的位置
	if (_tri_type==0)
	{
		_dst_tri[1].y = _dst_tri[0].y;
		_dst_tri[1].x = _dst_tri[0].x + _affine_width;

		_dst_tri[2].y = _dst_tri[0].y + _affine_height;
		_dst_tri[2].x = _dst_tri[0].x;

		_src_tri[0].x = _coordinates->x[0];
		_src_tri[0].y = _coordinates->y[0];
		_src_tri[1].x = _coordinates->x[1];
		_src_tri[1].y = _coordinates->y[1];
		_src_tri[2].x = _coordinates->x[3];
		_src_tri[2].y = _coordinates->y[3];
	}
	else if (_tri_type == 1)
	{
		_dst_tri[1].y = _dst_tri[0].y;
		_dst_tri[1].x = _dst_tri[0].x + _affine_width;

		_dst_tri[2].y = _dst_tri[0].y + _affine_height;
		_dst_tri[2].x = _dst_tri[1].x;

		_src_tri[0].x = _coordinates->x[0];
		_src_tri[0].y = _coordinates->y[0];
		_src_tri[1].x = _coordinates->x[1];
		_src_tri[1].y = _coordinates->y[1];
		_src_tri[2].x = _coordinates->x[2];
		_src_tri[2].y = _coordinates->y[2];
	}
	else if (_tri_type == 2)
	{
		_dst_tri[1].y = _dst_tri[0].y + _affine_height;
		_dst_tri[1].x = _dst_tri[0].x + _affine_width;

		_dst_tri[2].y = _dst_tri[1].y;
		_dst_tri[2].x = _dst_tri[0].x;

		_src_tri[0].x = _coordinates->x[0];
		_src_tri[0].y = _coordinates->y[0];
		_src_tri[1].x = _coordinates->x[2];
		_src_tri[1].y = _coordinates->y[2];
		_src_tri[2].x = _coordinates->x[3];
		_src_tri[2].y = _coordinates->y[3];
	}
	else
	{
		_dst_tri[0].x = _width - _dst_tri[0].x;

		_dst_tri[1].y = _dst_tri[0].y + _affine_height;
		_dst_tri[1].x = _dst_tri[0].x;

		_dst_tri[2].y = _dst_tri[1].y;
		_dst_tri[2].x = _dst_tri[0].x - _affine_width;

		_src_tri[0].x = _coordinates->x[1];
		_src_tri[0].y = _coordinates->y[1];
		_src_tri[1].x = _coordinates->x[2];
		_src_tri[1].y = _coordinates->y[2];
		_src_tri[2].x = _coordinates->x[3];
		_src_tri[2].y = _coordinates->y[3];
	}

}

//根据两组坐标获得仿射变换参数
//_src_tri[]为校正前坐标, _dst_tri[]为校正后坐标, *_corner_info为角点信息结构体
void GetAffineMat(IntPoint_ _src_tri[], IntPoint_ _dst_tri[], int _affine_operator_trans[])
{
	int i;   //循环变量
	double a[6 * 6];    //仿射变换公式参数
	double affine_operator[6];

	for (i = 0; i < 3; i++)   //根据仿射变换公式确定对应参数
	{
		int j = i * 12;
		int k = i * 12 + 6;
		a[j] = a[k + 3] = _dst_tri[i].x;
		a[j + 1] = a[k + 4] = _dst_tri[i].y;

		a[j + 2] = a[k + 5] = 1;
		a[j + 3] = a[j + 4] = a[j + 5] = 0;
		a[k] = a[k + 1] = a[k + 2] = 0;
		affine_operator[i * 2] = _src_tri[i].x;
		affine_operator[i * 2 + 1] = _src_tri[i].y;
	}
	gaus(a, affine_operator, 6);  //高斯消元法解线性方程组得仿射变换参数

	for (i = 0; i<6; i++)
	{
		_affine_operator_trans[i] = (int)(affine_operator[i] * 65536 >= 0 ? affine_operator[i] * 65536 + 0.5 : affine_operator[i] * 65536 - 0.5);
	}
}
#if 0
//找到钱的四条边上的点
//*_img_breakage_check为图像破损检测开关, *_src_img为输入图像, *__corner_info为角点信息结构体, _thred为背景阈值, *_row_point_num为横边点数, *_vert_point_num为纵边点数, _last_status为图像上次的状态
int GetMoneyEdgePoint(int *_img_breakage_check, IplImage_ *_src_img, CornerCorrectInfo_ *_corner_info, int _thred, int *_row_point_num, int *_vert_point_num, int _last_status)
{
	int i;  //循环变量
	int j;
	int k;
	int cnt;
	int width = _src_img->width;
	int height = _src_img->height;
	int width_step = _src_img->width_step;  //源图步长
	int edge_up_cnt = 0;    //找到的上边缘点数
	int edge_up_cnt1 = 0;   //贴边的上边缘点数
	int edge_down_cnt = 0;
	int edge_down_cnt1 = 0;
	int img_ignore_point_l;   //忽略的左边列数
	int img_ignore_point_r;
	int row_step = 2 << (ROW_2_POWER - 1);   //横边找点的步长
	int vert_step = 2 << (VERT_2_POWER - 1);

	unsigned char *psrc;

	memset(edge_x_up, -1, (width >> ROW_2_POWER)*sizeof(int));
	memset(edge_x_down, -1, (width >> ROW_2_POWER)*sizeof(int));
	memset(edge_x_left, -1, (height >> VERT_2_POWER)*sizeof(int));
	memset(edge_x_right, -1, (height >> VERT_2_POWER)*sizeof(int));

	_thred = _thred & 0xff;
	*_img_breakage_check = 1;   	//初始化设置为符合破损检测条件

	////解决左右边太靠近白边问题
	//if (0 != IgnoreCISCol(&img_ignore_point_l, &img_ignore_point_r, _src_img))
	//	return -1;	//??

	img_ignore_point_l = 20;
	img_ignore_point_r = 20; 

	if (width > 720)
	{
		width = 720;
	}

#ifdef OPENCV_DEBUG_
	IplImage * pImg=cvCreateImage(cvSize (_src_img->width,_src_img->height),IPL_DEPTH_8U,1);
	cvZero(pImg);
	for(int i=0;i<_src_img->height;i++)
	{
		for(int j=0;j<_src_img->width;j++)
		{
			pImg->imageData[i*pImg->widthStep+j] = _src_img->image_data[i*_src_img->width_step+j];
		}
	}
	cvSaveImage("corner.bmp",pImg);
#endif

	int cross_rows = 16; //行
	int cross_cols = 8;  //列

	//找上边缘点和下边缘点
	for (i = img_ignore_point_l >> ROW_2_POWER; i<((width - img_ignore_point_r) >> ROW_2_POWER); i++)
	{
		psrc = _src_img->image_data + row_step * i; 
		for (j = cross_rows; j<height * 2 / 3; j += cross_rows)     //从上往下找上边缘点
		{
			if (*(psrc + j*width_step) > _thred)  //先隔croos_rows行找到像素值>阈值的点
			{
				cnt = 0;
				for (k = 0; k < cross_rows+3; k++)  //再逐行找
				{
					if (*(psrc + ((j - cross_rows) + k)*width_step) > _thred)
					{
						if (++cnt > 3)      //连续4行大于阈值，点存入数组
						{
							edge_x_up[i] = (j - cross_rows) + k - 3;
							break;
						}
					}
					else
					{
						cnt = 0;
					}
				}
				if (cnt > 3)   //找到了，判断是否为贴边点，跳出
				{
					if (edge_x_up[i] == 0)   //贴边点记为-2，不做破损检测
					{
						edge_x_up[i] = -2;
						edge_up_cnt1++;
						*_img_breakage_check = 0;
					}
					else
					{
						edge_up_cnt++;
					}
					break;
				}
			}					
		}
		//从下往上找下边缘点
		if (edge_x_up[i] >= 0 || edge_x_up[i] == -2)
		{
			for (j = height - 1 - cross_rows; j >= height / 3; j -= cross_rows)  
			{
				if (*(psrc + j*width_step) > _thred)      //先隔croos_rows行找到像素值>阈值的点
				{
					cnt = 0;
					for (k = 0; k < cross_rows+3; k++)   //再逐行找
					{
						if (*(psrc + ((j + cross_rows) - k)*width_step) > _thred)
						{
							if (++cnt > 3)      //连续4行大于阈值，点存入数组
							{
								edge_x_down[i] = (j + cross_rows) - k + 3;
								break;
							}
						}
						else
						{
							cnt = 0;
						}
					}
					if (cnt > 3)     //找到了，判断是否为贴边点，跳出
					{
						if (edge_x_down[i] == height - 1)   //贴边点记为-2，不做破损检测
						{
							edge_x_down[i] = -2;
							edge_down_cnt1++;
							*_img_breakage_check = 0;
						}
						else
						{
							edge_down_cnt++;
						}
						break;
					}
				}			
			}
		}
	}
	
	/*if (edge_up_cnt1>15 || edge_down_cnt1>15)
	{
		int ret = judgeTwoMoney(edge_x_up, edge_x_down, width >> ROW_2_POWER, height);
		if (ret < 0)
		{
			return ret;
		}
	}*/
	
	if (edge_up_cnt+edge_up_cnt1<20)	//如果边缘查找连1半都不到，说明图像源有问题
	{
		if (1 == (_last_status & 0x01))	//上次是刹车状态
		{
			return ERR_CORNER_IMGSOURCE;				//判断为图像源错误
		}
		else
		{
			return ERR_CORNER_IMGCIS;        //判断为CIS错误
		}
	}

	if (edge_up_cnt1>25 && edge_up_cnt<5)
	{
		for (i = 0; i<width >> ROW_2_POWER; i++)
		{
			if (edge_x_up[i] == -2)
			{
				edge_x_up[i] = 0;
			}
		}
	}

	if (edge_down_cnt1>25 && edge_down_cnt<5)
	{
		for (i = 0; i<width >> ROW_2_POWER; i++)
		{
			if (edge_x_down[i] == -2)
			{
				edge_x_down[i] = height - 1;
			}
		}
	}

	//through y
	//找左边缘点和右边缘点
	for (i = 0; i < (height >> VERT_2_POWER); i++)   
	{
        psrc = _src_img->image_data + width_step*(i << VERT_2_POWER);
		for (j = img_ignore_point_l + cross_cols; j < width / 3; j += cross_cols)   //从左往右找左边缘点
		{
			if (*(psrc + j) > _thred)         //先隔croos_cols列找到像素值>阈值的点
			{
				cnt = 0;
				for (k = 0; k < cross_cols+3; k++)   //再逐列找
				{
					if (*(psrc + (j - cross_cols) + k) > _thred)
					{
						if (++cnt > 3)     //连续4列大于阈值，点存入数组
						{
							edge_x_left[i] = (j - cross_cols )+ k - 3;
							break;
						}
					}
					else
					{
						cnt = 0;
					}	
				}
				if (cnt > 3)   //找到了，跳出
				{
					break;
				}			
			}			
		}

		for (j = width - img_ignore_point_r - cross_cols; j >= 2 * width / 3; j -= cross_cols)   //从右往左找右边缘点
		{
			if (*(psrc + j) > _thred)
			{
				cnt = 0;
				for (k = 0; k < cross_cols+3; k++)
				{
					if (*(psrc + (j + cross_cols) - k) > _thred)
					{
						if (++cnt > 3)
						{
							edge_x_right[i] = (j + cross_cols )- k + 3;
							break;
						}
					}
					else
					{
						cnt = 0;
					}

				}
				if (cnt > 3)  //找到了，跳出
				{
					break;
				}
			}
		}		
	}

	//下面的两个循环不应该每次计算。  ？？
	for (i = 0; i<(width >> ROW_2_POWER); i++)
	{
		edge_x[i] = i * row_step;
	}

	for (i = 0; i<(height >> VERT_2_POWER); i++)
	{
		edge_y[i] = i * vert_step;
	}

	*_row_point_num = width >> ROW_2_POWER;
	*_vert_point_num = height >> VERT_2_POWER;
	return 0;
}
#endif

//找到钱的四条边上的点
//*_img_breakage_check为图像破损检测开关, *_src_img为输入图像, *__corner_info为角点信息结构体, _thred为背景阈值, *_row_point_num为横边点数, *_vert_point_num为纵边点数, _last_status为图像上次的状态
#if 1
int GetMoneyEdgePoint(int *_img_breakage_check, IplImage_ *_src_img, CornerCorrectInfo_ *_corner_info, int _thred, int *_row_point_num, int *_vert_point_num, int _last_status)
{
	int i;  //循环变量
	int j;
	int k;
	int cnt;
	int width = _src_img->width;
	int height = _src_img->height;
	int width_step = _src_img->width_step;  //源图步长
	int edge_up_cnt = 0;  //找到的上边缘点数
	int edge_up_cnt1 = 0;   //贴边的上边缘点数
	int edge_down_cnt = 0;
	int edge_down_cnt1 = 0;
	int img_ignore_point_l;   //忽略的左边列数
	int img_ignore_point_r;
	int row_step = 2 << (ROW_2_POWER - 1);   //横边找点的步长
	int vert_step = 2 << (VERT_2_POWER - 1);

	unsigned char *psrc;

	memset(edge_x_up, -1, (width >> ROW_2_POWER)*sizeof(int));
	memset(edge_x_down, -1, (width >> ROW_2_POWER)*sizeof(int));
	memset(edge_x_left, -1, (height >> VERT_2_POWER)*sizeof(int));
	memset(edge_x_right, -1, (height >> VERT_2_POWER)*sizeof(int));

	_thred = _thred & 0xff;
	*_img_breakage_check = 1;       //初始化设置为符合破损检测条件

	//解决左右边太靠近白边问题
	//if (0 != IgnoreCISCol(&img_ignore_point_l, &img_ignore_point_r, _src_img))
	//  return -1;  //??

	img_ignore_point_l = 20;
	img_ignore_point_r = 20;

	if (width > 720)
	{
		width = 720;
	}

#ifdef OPENCV_DEBUG_
	IplImage * pImg = cvCreateImage(cvSize(_src_img->width, _src_img->height), IPL_DEPTH_8U, 1);
	cvZero(pImg);
	for (int i = 0; i<_src_img->height; i++)
	{
		for (int j = 0; j<_src_img->width; j++)
		{
			pImg->imageData[i*pImg->widthStep + j] = _src_img->image_data[i*_src_img->width_step + j];
		}
	}
	cvSaveImage("corner.bmp", pImg);
#endif

	int cross_rows = 8; //行
	int cross_cols = 16;  //列

	//找上边缘点和下边缘点
	for (i = img_ignore_point_l >> ROW_2_POWER; i<((width - img_ignore_point_r) >> ROW_2_POWER); i++)
	{
		psrc = _src_img->image_data + row_step * i;

		//?? x=row_step * i
		int x = row_step * i;

		for (j = cross_rows; j<height * 2 / 3; j += cross_rows)     //从上往下找上边缘点
		{
			unsigned char value = *(psrc + j*width_step);

			if (value > _thred)  //先隔croos_rows行找到像素值>阈值的点
			{
				cnt = 0;
				for (k = 0; k < cross_rows + 3; k++)  //再逐行找
				{
					value = *(psrc + ((j - cross_rows) + k)*width_step);
					if (value > _thred)  //?? x=row_step * i
					{
						if (++cnt > 3)      //连续4行大于阈值，点存入数组
						{
							edge_x_up[i] = (j - cross_rows) + k - 3;
							break;
						}
					}
					else
					{
						cnt = 0;
					}
				}
				if (cnt > 3)   //找到了，判断是否为贴边点，跳出
				{
					if (edge_x_up[i] == 0)   //贴边点记为-2，不做破损检测
					{
						edge_x_up[i] = -2;
						edge_up_cnt1++;
						*_img_breakage_check = 0;
					}
					else
					{
						edge_up_cnt++;
					}
					break;
				}
			}
		}
		//从下往上找下边缘点
		if (edge_x_up[i] >= 0 || edge_x_up[i] == -2)
		{
			for (j = height - 1 - cross_rows; j >= height / 3; j -= cross_rows)
			{
				unsigned char value = *(psrc + j*width_step);

				if (value > _thred) //?? x=row_step * i       //先隔croos_rows行找到像素值>阈值的点
				{
					cnt = 0;
					for (k = 0; k < cross_rows + 3; k++)   //再逐行找
					{
						value = *(psrc + ((j + cross_rows) - k)*width_step);
						if (value > _thred) //?? x=row_step * i
						{
							if (++cnt > 3)      //连续4行大于阈值，点存入数组
							{
								edge_x_down[i] = (j + cross_rows) - k + 3;
								break;
							}
						}
						else
						{
							cnt = 0;
						}
					}
					if (cnt > 3)     //找到了，判断是否为贴边点，跳出
					{
						if (edge_x_down[i] == height - 1)   //贴边点记为-2，不做破损检测
						{
							edge_x_down[i] = -2;
							edge_down_cnt1++;
							*_img_breakage_check = 0;
						}
						else
						{
							edge_down_cnt++;
						}
						break;
					}
				}
			}
		}
	}

	//  if (edge_up_cnt1>15 || edge_down_cnt1>15)
	//  {
	//      int ret = judgeTwoMoney(edge_x_up, edge_x_down, width >> ROW_2_POWER, height);
	//      if (ret < 0)
	//      {
	//          return ret;
	//      }
	//  }

	if (edge_up_cnt + edge_up_cnt1<20)    //如果边缘查找连1半都不到，说明图像源有问题
	{
		if (1 == (_last_status & 0x01)) //上次是刹车状态
		{
			return ERR_CORNER_IMGSOURCE;                //判断为图像源错误
		}
		else
		{
			return ERR_CORNER_IMGCIS;        //判断为CIS错误
		}
	}

	if (edge_up_cnt1>25 && edge_up_cnt<5)
	{
		for (i = 0; i<width >> ROW_2_POWER; i++)
		{
			if (edge_x_up[i] == -2)
			{
				edge_x_up[i] = 0;
			}
		}
	}

	if (edge_down_cnt1>25 && edge_down_cnt<5)
	{
		for (i = 0; i<width >> ROW_2_POWER; i++)
		{
			if (edge_x_down[i] == -2)
			{
				edge_x_down[i] = height - 1;
			}
		}
	}

	//through y
	//找左边缘点和右边缘点
	for (i = 0; i < (height >> VERT_2_POWER); i++)
	{
		psrc = _src_img->image_data + width_step*(i << VERT_2_POWER);
		for (j = img_ignore_point_l + cross_cols; j < width / 3; j += cross_cols)   //从左往右找左边缘点
		{
			int x = j;
			unsigned char value = *(psrc + j);

			if (value > _thred)    //?? x=j       //先隔croos_cols列找到像素值>阈值的点
			{
				cnt = 0;
				for (k = 0; k < cross_cols + 3; k++)   //再逐列找
				{
					x = ((j - cross_cols) + k);
					value = *(psrc + (j - cross_cols) + k);
					if (value > _thred)  //?? x=(j - cross_cols) + k)
					{
						if (++cnt > 3)     //连续4列大于阈值，点存入数组
						{
							edge_x_left[i] = (j - cross_cols) + k - 3;
							break;
						}
					}
					else
					{
						cnt = 0;
					}
				}
				if (cnt > 3)   //找到了，跳出
				{
					break;
				}
			}
		}

		for (j = width - img_ignore_point_r - cross_cols; j >= 2 * width / 3; j -= cross_cols)   //从右往左找右边缘点
		{

			int x = j;
			unsigned char value = *(psrc + j);

			if (value > _thred)  //?? x=j
			{
				cnt = 0;
				for (k = 0; k < cross_cols + 3; k++)
				{
					x = ((j + cross_cols) - k);
					value = *(psrc + (j + cross_cols) - k);
					if (value > _thred)  //?? x=(j + cross_cols) - k)
					{
						if (++cnt > 3)
						{
							edge_x_right[i] = (j + cross_cols) - k + 3;
							break;
						}
					}
					else
					{
						cnt = 0;
					}

				}
				if (cnt > 3)  //找到了，跳出
				{
					break;
				}
			}
		}
	}

	//下面的两个循环不应该每次计算。  ？？
	for (i = 0; i<(width >> ROW_2_POWER); i++)
	{
		edge_x[i] = i * row_step;
	}

	for (i = 0; i<(height >> VERT_2_POWER); i++)
	{
		edge_y[i] = i * vert_step;
	}

	*_row_point_num = width >> ROW_2_POWER;
	*_vert_point_num = height >> VERT_2_POWER;
	return 0;
}
#endif

//确定CIS左右两边需要忽略的行
int IgnoreCISCol(int *_left, int *_right, IplImage_ *_src_img)
{
	int i = 0;
	int j = 0;
	unsigned char* psrc;
	int count = 0;  //记录列上低于阈值的像素个数

	//确定CIS左边低于阈值的像素个数（黑点）
	for (i = 6; i<10; i++)
	{
		psrc = _src_img->image_data + i*_src_img->width_step;
		for (j = 5; j<17; j++)
		{
			if (*(psrc + j)<15)
			{
				count++;
				break;
			}
		}
	}
	if (count>2)
	{
		*_left = 16;    //图像左右侧忽略16个像素点
	}
	else
	{
		*_left = IMAGE_IGNORE_POINT;
	}

	//确定CIS右边低于阈值的像素个数（黑点）
	count = 0;
	for (i = 6; i<10; i++)
	{
		psrc = _src_img->image_data + i*_src_img->width_step;
		for (j = _src_img->width - 5; j>_src_img->width - 17; j--)
		{
			if (*(psrc + j)<15)
			{
				count++;
				break;
			}
		}
	}
	if (count>2)
	{
		*_right = 16;    //图像左右侧忽略16个像素点
	}
	else
	{
		*_right = IMAGE_IGNORE_POINT;
	}

	return 0;
}

//找到钱的四个角点坐标
//*_img_breakage_check为图像破损检测开关, *_src为输入图像, *_corner_info为角点信息结构体, _last_status为图像上次的状态
int FindCornerAndAngle(int *_img_breakage_check, unsigned char *_src, CornerCorrectInfo_ *_corner_info, int _last_status)
{
	int width = _corner_info->width;
	int height = _corner_info->height;
	int row_point_num;  //横边采样点数
	int vert_point_num; //纵边采样点数
	int background_thred = RMB_BACKGROUND_THRED;   //背景阈值
	int confidence_thred = EDGE_CONFIDENCE_THRED;
	int ret = 0;
	int i;

	int parallel_thred;
	int switch_temp;

	IplImage_ src_img;  //图像数组

	ReleaseImage(&src_img);
	CreateImage(&src_img, width, height, _src);    //创建图像
	src_img.width_step = _corner_info->width_step;

	//  gClearDirt(&src_img);   //清除CIS积灰造成的背景灰条

	//所有币种统一用这个
	ret = GetMoneyEdgePoint(_img_breakage_check, &src_img, _corner_info, background_thred, &row_point_num, &vert_point_num, _last_status);
	if (ret < 0)
		return ret;

	_corner_info->up_edge_info.type = 0;   //确定边的类型
	_corner_info->down_edge_info.type = 1;
	_corner_info->left_edge_info.type = 2;
	_corner_info->right_edge_info.type = 3;

	//最小二乘法拟合直线
	ret = FittingEdge(background_thred, edge_x, edge_x_up, row_point_num, 0, &_corner_info->up_edge_info, &src_img);    //up
	ret |= FittingEdge(background_thred, edge_x, edge_x_down, row_point_num, 0, &_corner_info->down_edge_info, &src_img);   //down

	if (_corner_info->up_edge_info.k < 0)
	{
		DelErrPoint(edge_y, edge_x_left, vert_point_num, &_corner_info->up_edge_info);
		DelErrPoint(edge_y, edge_x_right, vert_point_num, &_corner_info->down_edge_info);
	}
	else
	{
		DelErrPoint(edge_y, edge_x_right, vert_point_num, &_corner_info->up_edge_info);
		DelErrPoint(edge_y, edge_x_left, vert_point_num, &_corner_info->down_edge_info);
	}

	ret |= FittingEdge(background_thred, edge_y, edge_x_left, vert_point_num, 1, &_corner_info->left_edge_info, &src_img);  //left
	ret |= FittingEdge(background_thred, edge_y, edge_x_right, vert_point_num, 1, &_corner_info->right_edge_info, &src_img);    //right

	if (ret < 0)  //拟合出错，报错返回
	{
		return ERR_CORNER_EDGE_DEGREE;
	}
	int degree_thred = EDGE_DEGREE_THRED * 2;
	if (_corner_info->up_edge_info.degree >= degree_thred || _corner_info->down_edge_info.degree >= degree_thred || _corner_info->up_edge_info.degree <= -degree_thred || _corner_info->down_edge_info.degree <= -degree_thred) //倾角过大，报错返回
	{
		return ERR_CORNER_EDGE_DEGREE;
	}

	int refer_thred = EDGE_REFER_THRED * 2;
	//判断上下是否平行，修正错误边
	if ((_corner_info->up_edge_info.degree != 0 || _corner_info->up_edge_info.b != 0) && (_corner_info->down_edge_info.degree != 0 || _corner_info->down_edge_info.b != height - 1))
		//判断上下边缘是否在窗口里，如果都在窗口里，才判断是否平行
	{
		parallel_thred = abs(_corner_info->up_edge_info.degree + _corner_info->down_edge_info.degree) / 2;
		parallel_thred = (int)(parallel_thred*0.1 + 0.5);
		//if (parallel_thred < refer_thred)
		//{
		//  parallel_thred = refer_thred;
		//}
		if (parallel_thred < 8)
		{
			parallel_thred = 4;
		}
		switch_temp = _corner_info->up_edge_info.degree - _corner_info->down_edge_info.degree;
		if (switch_temp< -parallel_thred || switch_temp>parallel_thred)  //若上下边相差大于8度
		{
			if (_corner_info->up_edge_info.confidence >= confidence_thred)
			{
				if (_corner_info->down_edge_info.confidence >= confidence_thred)
				{
					//若上下边置信度均大于阈值且下边置信度大于上边，以下边为正确边，修正上边
					if (_corner_info->down_edge_info.confidence > _corner_info->up_edge_info.confidence)
					{
						ret = FittingEdgeRefer(edge_x, edge_x_up, row_point_num, 0, &_corner_info->up_edge_info, &_corner_info->down_edge_info);    //up
					}
					else
					{
						ret = FittingEdgeRefer(edge_x, edge_x_down, row_point_num, 0, &_corner_info->down_edge_info, &_corner_info->up_edge_info);  //down
					}
				}
				else   //若上边置信度大于阈值而下边置信度小于阈值，修正下边
				{
					ret = FittingEdgeRefer(edge_x, edge_x_down, row_point_num, 0, &_corner_info->down_edge_info, &_corner_info->up_edge_info);  //down
				}
			}
			//若上边置信度小于阈值而下边置信度大于阈值，修正上边
			else if (_corner_info->down_edge_info.confidence >= confidence_thred)
			{
				ret = FittingEdgeRefer(edge_x, edge_x_up, row_point_num, 0, &_corner_info->up_edge_info, &_corner_info->down_edge_info);    //up
			}
			else  //若上下边置信度均小于阈值，报错
			{
				return  ERR_CORNER_PARALLEL_DEGREE;
			}
			if (ret < 0)
			{
				return ERR_CORNER_PARALLEL_DEGREE;
			}
		}
	}

	//判断左右是否平行，修正错误边
	parallel_thred = abs(abs(_corner_info->left_edge_info.degree) + abs(_corner_info->right_edge_info.degree) - 360) / 2;
	parallel_thred = (int)(parallel_thred*0.15 + 0.5);
	if (parallel_thred < 8)
	{
		parallel_thred = 4;
	}
	switch_temp = abs(_corner_info->left_edge_info.degree) - abs(_corner_info->right_edge_info.degree);
	if (switch_temp< -parallel_thred || switch_temp>parallel_thred)   //若左右边相差大于8度
	{
		if (_corner_info->left_edge_info.confidence>92 && _corner_info->right_edge_info.confidence>92)
		{
			ret = ERR_COORNER_BIG_ANGLE;
		}
		else if (_corner_info->left_edge_info.confidence >= confidence_thred)
		{
			if (_corner_info->right_edge_info.confidence >= confidence_thred)
			{
				//若左右边置信度均大于阈值且右边置信度大于左边，以右边为正确边，修正左边
				if (_corner_info->right_edge_info.confidence > _corner_info->left_edge_info.confidence)
				{
					ret = FittingEdgeRefer(edge_y, edge_x_left, vert_point_num, 1, &_corner_info->left_edge_info, &_corner_info->right_edge_info);  //left
				}
				else
				{
					ret = FittingEdgeRefer(edge_y, edge_x_right, vert_point_num, 1, &_corner_info->right_edge_info, &_corner_info->left_edge_info); //right
				}
			}
			else   //若左边置信度大于阈值而右边置信度小于阈值，修正右边
			{
				ret = FittingEdgeRefer(edge_y, edge_x_right, vert_point_num, 1, &_corner_info->right_edge_info, &_corner_info->left_edge_info); //right
			}
		}
		//若左边置信度小于阈值而右边置信度大于阈值，修正左边
		else if (_corner_info->right_edge_info.confidence >= confidence_thred)
		{
			ret = FittingEdgeRefer(edge_y, edge_x_left, vert_point_num, 1, &_corner_info->left_edge_info, &_corner_info->right_edge_info);  //left
		}
		else    //若左右边置信度均小于阈值，报错
		{
			return  ERR_CORNER_PARALLEL_DEGREE;
		}
		if (ret < 0 && ret != ERR_COORNER_BIG_ANGLE)
		{
			return ERR_CORNER_PARALLEL_DEGREE;
		}
	}

	FloatPoint_ corner[4];  //角点坐标
	CalcCrosspoint(&_corner_info->up_edge_info, &_corner_info->down_edge_info, &_corner_info->left_edge_info, corner, corner + 3);    //计算四个角点坐标
	CalcCrosspoint(&_corner_info->up_edge_info, &_corner_info->down_edge_info, &_corner_info->right_edge_info, corner + 1, corner + 2);

	for (i = 0; i < 4; i++)     //图中钱的四个角坐标
	{
		_corner_info->coordinates.x[i] = corner[i].x >= 0 ? (int)(corner[i].x + 0.5) : (int)(corner[i].x - 0.5);
		_corner_info->coordinates.y[i] = corner[i].y >= 0 ? (int)(corner[i].y + 0.5) : (int)(corner[i].y - 0.5);
	}

	_corner_info->edge_x = edge_x;
	_corner_info->edge_y = edge_y;
	_corner_info->edge_x_up = edge_x_up;
	_corner_info->edge_x_down = edge_x_down;
	_corner_info->edge_x_left = edge_x_left;
	_corner_info->edge_x_right = edge_x_right;

	/******************************/
	GetConfidence(_corner_info->coordinates.x[0], _corner_info->coordinates.y[0], _corner_info->coordinates.x[1], _corner_info->coordinates.y[1], row_point_num, edge_x, edge_x_up, &_corner_info->up_edge_info, _corner_info, CORNER_EDGE_UP);
	GetConfidence(_corner_info->coordinates.x[3], _corner_info->coordinates.y[3], _corner_info->coordinates.x[2], _corner_info->coordinates.y[2], row_point_num, edge_x, edge_x_down, &_corner_info->down_edge_info, _corner_info, CORNER_EDGE_DOWN);

	GetConfidence(_corner_info->coordinates.y[0], _corner_info->coordinates.x[0], _corner_info->coordinates.y[3], _corner_info->coordinates.x[3], vert_point_num, edge_y, edge_x_left, &_corner_info->left_edge_info, _corner_info, CORNER_EDGE_LEFT);
	GetConfidence(_corner_info->coordinates.y[1], _corner_info->coordinates.x[1], _corner_info->coordinates.y[2], _corner_info->coordinates.x[2], vert_point_num, edge_y, edge_x_right, &_corner_info->right_edge_info, _corner_info, CORNER_EDGE_RIGHT);


	_corner_info->angle.angle_up_left = GetAngle(&_corner_info->up_edge_info, &_corner_info->left_edge_info, 3);
	_corner_info->angle.angle_up_right = GetAngle(&_corner_info->up_edge_info, &_corner_info->right_edge_info, 2);
	_corner_info->angle.angle_down_left = GetAngle(&_corner_info->down_edge_info, &_corner_info->left_edge_info, 0);
	_corner_info->angle.angle_down_right = GetAngle(&_corner_info->down_edge_info, &_corner_info->right_edge_info, 1);
	/******************************/

	GetAreaOfBreak(_corner_info, row_point_num, vert_point_num, edge_x, edge_x_up, edge_y, edge_x_left, TOP_LEFT_POINT);
	GetAreaOfBreak(_corner_info, row_point_num, vert_point_num, edge_x, edge_x_up, edge_y, edge_x_right, TOP_RIGHT_POINT);

	GetAreaOfBreak(_corner_info, row_point_num, vert_point_num, edge_x, edge_x_down, edge_y, edge_x_right, DOWN_RIGHT_POINT);
	GetAreaOfBreak(_corner_info, row_point_num, vert_point_num, edge_x, edge_x_down, edge_y, edge_x_left, DOWN_LEFT_POINT);

#ifdef SAVE_AREA
	printf("折角面积： %-8f, %-8f ,%-8f, %-8f \n", _corner_info->areaSize[0], _corner_info->areaSize[1], _corner_info->areaSize[2], _corner_info->areaSize[3]);
#endif // SAVE_AREA

	int *p_edge[2];
	p_edge[0] = edge_x_up;
	p_edge[1] = edge_x_down;
	for (i = 0; i < 4; i++)
	{
		if (_corner_info->coordinates.y[i] < -5 || _corner_info->coordinates.y[i]>height + 4)
		{
			int x = _corner_info->coordinates.x[i];
			int k = x >> ROW_2_POWER;
			if (p_edge[i / 2][k] == -2 || p_edge[i / 2][k + 1] == -2 || p_edge[i / 2][k - 1] == -2)
			{
				_corner_info->areaSize[i] = 0;
				_corner_info->missing_info = i;
			}
		}
	}

	return ret;
}

//根据一组点由最小二乘法多次拟合直线，得到直线的k,b,degree,confidence等值
//_thred为背景阈值, *_x为点的x坐标, *_y_src为点的y坐标, _cnt为点数, _type为边的类型, *edge_line_info为边结构体, *_src_img为输入图像
int FittingEdge(int _thred, int *_x, int *_y_src, int _cnt, int _type, EdgeInfo_ *_edge_line_info, IplImage_ *_src_img)
{
	int del_point = 0;  //删除的点数
	int i;  //循环变量
	int num = 0;

	int deviation_max;  //最大偏移距离
	int deviation_plus_max;  //向上最大偏移
	int deviation_minus_max;
	int deviation_plus_cnt;  //向上偏移的点数
	int deviation_minus_cnt;
	int plus_index;  //向上最大偏移点的索引
	int minus_index;
	int plus_valid_num;  //向上偏移的有效点数
	int minus_valid_num;
	int direction;  //偏移方向
	int nthred = _thred + 10;

	int real_y;  //点的计算y值

	long long A = 0;  //最小二乘法的公式计算
	long long B = 0;
	long long C = 0;
	long long D = 0;

	long long k, b;  //直线k,b
	double temp;
	double degree;  //直线倾角

	int *y = edge_data_buff;
	memcpy(y, _y_src, _cnt*sizeof(int));

	_edge_line_info->confidence = 0;

	for (i = 0; i<_cnt; i++)    //计算ABCD，最小二乘法
	{
		if (y[i] >= 0)
		{
			_edge_line_info->confidence++;
			if (_x[i] >= 0)
			{
				A += y[i];	//data_y.sum ();
				B += _x[i];//data_x.sum ();
				C += _x[i] * y[i];//(data_x * data_y).sum();
				D += _x[i] * _x[i];//(data_x * data_X).sum();

				num++;
			}
		}
	}

	if (_edge_line_info->confidence == 0)
	{
		_edge_line_info->b = 0;
		_edge_line_info->k = 0;
		_edge_line_info->degree = 0;
		return 0;
	}

	while (1)
	{
		temp = (double)(B * B - num * D);     //拟合直线
		if (temp == 0)
		{
			k = 0;
			b = 0;
		}
		else
		{
			k = (long long)((A * B - num * C) / temp * 1024);		//1024的精度
			b = (long long)((B * C - A * D) / temp * 1024);		//1024的精度
		}
		
		deviation_max = 0;
		deviation_plus_max = 0;
		deviation_minus_max = 0;
		deviation_plus_cnt = 0;
		deviation_minus_cnt = 0;
		plus_valid_num = 0;
		minus_valid_num = 0;

		for (i = 0; i<_cnt; i++)
		{
			if (y[i] >= 0)
			{
				real_y = (k*_x[i] + b + 512) >> 10;
				fitting_deviation[i] = (k*_x[i] + b) - (y[i] * 1024);

				if (_type)   //对左右边
				{
					if (_x[i] >= 0 && _x[i]<_src_img->height && real_y >= 0 && real_y<_src_img->width)
					{
						fitting_point_valid[i] = *(unsigned char*)(_src_img->image_data + _x[i] * _src_img->width_step + real_y);  //不应该是real_y吧？？
					}
					else
					{
						fitting_point_valid[i] = 0;
					}
				}
				else     //对上下边
				{
					if (_x[i] >= 0 && _x[i]<_src_img->width && real_y >= 0 && real_y<_src_img->height)
					{
						fitting_point_valid[i] = *(unsigned char*)(_src_img->image_data + real_y*_src_img->width_step + _x[i]);
					}
					else
					{
						fitting_point_valid[i] = 0;
					}
				}
				if (fitting_deviation[i]<0)   //若点在直线下方（或右方）
				{
					deviation_minus_cnt++;   //向下偏移的点数+1
					if (fitting_point_valid[i] > nthred)
					{
						minus_valid_num++;   //向下偏移的有效点数+1
					}
					if (fitting_deviation[i] < deviation_minus_max)   //找到向下偏移最多的点
					{
						deviation_minus_max = fitting_deviation[i];
						minus_index = i;
					}
				}
				else                       //若点在直线上方（或左方）
				{
					deviation_plus_cnt++;     //向上偏移的点数+！
					if (fitting_point_valid[i] > nthred)
					{
						plus_valid_num++;     //向上偏移的有效点数+1
					}
					if (fitting_deviation[i] > deviation_plus_max)   //找到向上偏移最多的点
					{
						deviation_plus_max = fitting_deviation[i];
						plus_index = i;
					}
				}
			}
		}

		deviation_max = deviation_plus_max>-deviation_minus_max ? deviation_plus_max : -deviation_minus_max;  //记录最大偏移

		if (deviation_max < 3072)  //最大偏移小于3，跳出迭代
		{
			break;
		}

		if (-deviation_minus_max<3072)	//若向下最大偏移小于3且向上最大偏移大于3，则去掉向上偏移最大的点，direction = 1
		{
			direction = 1;
		}
		else if (deviation_plus_max<3072)	//若向上最大偏移小于3且向下最大偏移大于3，则去掉向下偏移最大的点，direction = 0
		{
			direction = 0;
		}
		//向上最大偏移与向下最大偏移都大于3的情况，比较有效点数
		else if (minus_valid_num * 100 / deviation_minus_cnt > plus_valid_num * 100 / deviation_plus_cnt)
		{
			direction = 1;
		}
		else
		{
			direction = 0;
		}

		if (direction)   //direction = 1,去掉向上偏移最大的点
		{
			del_point++;
			A -= y[plus_index];	//data_y.sum ();
			B -= _x[plus_index];//data_x.sum ();
			C -= _x[plus_index] * y[plus_index];//(data_x * data_y).sum();
			D -= _x[plus_index] * _x[plus_index];//(data_x * data_X).sum();
			y[plus_index] = -1;
			num--;
		}
		else      //direction = 0,去掉向下偏移最大的点
		{
			del_point++;
			A -= y[minus_index];	//data_y.sum ();
			B -= _x[minus_index];//data_x.sum ();
			C -= _x[minus_index] * y[minus_index];//(data_x * data_y).sum();
			D -= _x[minus_index] * _x[minus_index];//(data_x * data_X).sum();
			y[minus_index] = -1;
			num--;
		}	
	}

	if (B * B == num * D)
	{
		_edge_line_info->b = 0;
		_edge_line_info->k = 0;
		_edge_line_info->degree = 0;
		return -1;
	}

	_edge_line_info->k = (A * B - num * C) / (double)(B * B - num * D);
	_edge_line_info->b = (B * C - A * D) / (double)(B * B - num * D);

	_edge_line_info->confidence = (_edge_line_info->confidence - del_point) * 100 / _edge_line_info->confidence;  //计算边的置信度

	if (_type)  //计算边的倾角
	{
		if (_edge_line_info->k != 0)
		{
			degree = atan(1 / _edge_line_info->k) / 3.14 * 180;
		}
		else
		{
			degree = 90;
		}
	}
	else
	{
		degree = atan(_edge_line_info->k) / 3.14 * 180;
	}

	if (degree >= 0)
		_edge_line_info->degree = (int)((degree * 2) + 0.5);
	else
		_edge_line_info->degree = (int)((degree * 2) - 0.5);

	return 0;
}

//根据置信度较高的边，修正另一条边
//*_x为点的x坐标, *_y为点的y坐标, _cnt为点数, _type为边的类型, *EdgeLineInfo为待修正边, *ReferEdgeLineInfo为正确边
int FittingEdgeRefer(int *_x, int *_y_src, int _cnt, int _type, EdgeInfo_ *_edge_line_info, EdgeInfo_ *_refer_edge_line_info)
{
	int i;  //循环变量
	int del_point = 0;
	int state = 0;
	int targe_cnt = 0;
	int targe_result;
	int difference_min = 10000;
	int standard;
	int refer;

	int num = 0;
	long long A = 0;
	long long B = 0;
	long long C = 0;
	long long D = 0;
	double degree;
	long long k = _refer_edge_line_info->k * 1024;   //参考边的k,b
	long long b = _refer_edge_line_info->b * 1024;

	int *y = edge_data_buff;
	memcpy(y, _y_src, _cnt*sizeof(int));

	_edge_line_info->confidence = 0;
	for (i = 0; i<_cnt; i++)   //根据有效点数重新计算修正边的置信度
	{
		if (y[i] >= 0)
		{
			_edge_line_info->confidence++;
		}
	}

	for (i = 0; i<100; i++)
	{
		fitting_deviation[i] = -9999999;
		fitting_difference[i] = 9999999;
	}

	for (i = 0; i<_cnt; i++)
	{
		if (y[i] >= 0)
		{
			fitting_deviation[i] = k*_x[i] + b - y[i] * 1024;   //两条边上对应点的距离
			if (i>0 && fitting_deviation[i - 1] != -9999999)
			{
				fitting_difference[i] = fitting_deviation[i - 1] - fitting_deviation[i];  //两条边上对应点的距离与前一个距离的差值
				if (abs(fitting_difference[i]) < 3072)  //若与前一个相差较小
				{
					if (state == 0)  //第一个差值小于的点
					{
						state = 1;
						fitting_targe_point[targe_cnt] = 2;
						fitting_targe_diff_mean[targe_cnt] = abs(fitting_difference[i]);  //该点边距与前一个点边距的绝对差值
						fitting_targe_num[targe_cnt] = fitting_deviation[i - 1] + fitting_deviation[i]; //该点边距与前一个点边距相加
						standard = (fitting_deviation[i - 1] + fitting_deviation[i]) / 2;  //该点边距与前一个点边距的均值
					}
					else
					{
						if (abs(standard - fitting_deviation[i]) <3072)  //若与之前的平均距离相差较大
						{

							fitting_targe_diff_mean[targe_cnt] += abs(fitting_difference[i]);
							fitting_targe_num[targe_cnt] += fitting_deviation[i];
							standard = (standard*fitting_targe_point[targe_cnt] + fitting_deviation[i]) / (fitting_targe_point[targe_cnt] + 1); //均值
							fitting_targe_point[targe_cnt]++;//点数
						}
						else
						{
							state = 0;
							targe_cnt++;
						}
					}
				}
				else
				{
					if (state == 1)
					{
						state = 0;
						targe_cnt++;
					}
				}
			}
		}
		else
		{
			if (state == 1)
			{
				state = 0;
				targe_cnt++;
			}
		}
	}

	if (state == 1)
	{
		targe_cnt++;
	}
	if (targe_cnt == 0)
	{
		return -1;	//没有找到平行的线
	}

	targe_result = 0;
	for (i = 0; i<targe_cnt; i++)
	{
		if (targe_result != i)	//比较可信程度
		{
			if (fitting_targe_point[targe_result] > fitting_targe_point[i])	//点数可信度最大
			{
				if (fitting_targe_point[targe_result] < 1.5*fitting_targe_point[i])	//点数差距不大
				{
					//再看均值
					if (fitting_targe_diff_mean[targe_result] > fitting_targe_diff_mean[i])
					{
						targe_result = i;
					}
				}
			}
			else
			{
				if (1.5*fitting_targe_point[targe_result] > fitting_targe_point[i])	//点数差距不大
				{
					//再看均值
					if (fitting_targe_diff_mean[targe_result] > fitting_targe_diff_mean[i])
					{
						targe_result = i;
					}
				}
				else
				{
					targe_result = i;
				}
			}
		}

	}

	refer = fitting_targe_num[targe_result] / fitting_targe_point[targe_result];
	del_point = 0;
	for (i = 0; i<_cnt; i++)
	{
		if (fitting_deviation[i] != -9999999)
		{
			difference_min = fitting_deviation[i] - refer;
			if (abs(difference_min) >= 3072)
			{
				y[i] = -1;
				del_point++;
			}
		}
	}

	if (_edge_line_info->confidence - del_point < (_edge_line_info->confidence >> 2))
	{
		return -1;	//没有找到
	}

	for (i = 0; i<_cnt; i++)
	{
		if (y[i] >= 0)
		{
			if (_x[i] >= 0)
			{
				A += y[i];	//data_y.sum ();
				B += _x[i];//data_x.sum ();
				C += _x[i] * y[i];//(data_x * data_y).sum();
				D += _x[i] * _x[i];//(data_x * data_X).sum();

				num++;
			}
		}
	}

	if ((B * B - num * D) == 0)
	{
		_edge_line_info->k = 0;
		_edge_line_info->b = 0;
	}
	else
	{
		_edge_line_info->k = (A * B - num * C) / (double)(B * B - num * D);
		_edge_line_info->b = (B * C - A * D) / (double)(B * B - num * D);
	}	

	if (_type)
	{
		if (_edge_line_info->k != 0)
		{
			degree = atan(1 / _edge_line_info->k) / 3.14 * 180;
		}
		else
		{
			degree = 90;
		}
	}
	else
	{
		degree = atan(_edge_line_info->k) / 3.14 * 180;
	}

	if (degree >= 0)
		_edge_line_info->degree = (int)((degree * 2) + 0.5);
	else
		_edge_line_info->degree = (int)((degree * 2) - 0.5);

	return 0;
}

//得到四条边的置信度
//得到四条边的置信度

int bais[128] = { 0 }; //局部可能死机
int GetConfidence(int _x1, int _y1, int _x2, int _y2, int _cnt, int *_edge_x, int *_edge_point, EdgeInfo_ *_edge_info, CornerCorrectInfo_ *_corner_info, int _edge_type)
{
	double k = _edge_info->k;
	double b = _edge_info->b;

	int valid_point = 0;
	int deviation_sum = 0;
	
	int deviation_point = 0;
	float mean = -1;
	float var = -1;
	int ret = -1;

	if (_x1 != _x2&&_y1 != _y2)
	{
		//k = (double)(_y1 - _y2) / (double)(_x1 - _x2) * 1024; //测试有点问题
		//b = (_y1 - (k >> 10)*_x1) * 1024;

		for (int i = 0; i < _cnt; i++)
		{
			if (_edge_x[i] >= _x1&&_edge_x[i] <= _x2)  //在角点之间求
			{
				if (_edge_point[i]>0)
				{
					valid_point++;
					int y = k*_edge_x[i] + b;
					bais[i] = abs(y - _edge_point[i]);
					if (bais[i]>3)
					{
						deviation_point++;
					}
					deviation_sum += bais[i];
				}
			}
		}

		if (valid_point != 0 && (valid_point - 1)!=0)
		{
			mean = (float)deviation_sum / valid_point;
			float sum = 0;
			for (int i = 0; i < _cnt; i++)
			{
				if (bais[i]>0)
				{
					sum += (bais[i] - mean)*(bais[i] - mean);
				}
			}
			var = sum / (valid_point - 1);
		}
	}
	else if (_x1 == _x2)  //水平
	{
		for (int i = 0; i < _cnt; i++)
		{
			if (_edge_x[i] >= _y1&&_edge_x[i] <= _y2)
			{
				if (_edge_point[i]>0)
				{
					valid_point++;
					bais[i] = abs(_x1 - _edge_point[i]);
					if (bais[i] > 3)
					{
						deviation_point++;
					}
					deviation_sum += bais[i];
				}
			}

		}
		if (valid_point != 0 && (valid_point - 1) != 0)
		{
			mean = (float)deviation_sum / valid_point;
			float sum = 0;
			for (int i = 0; i < _cnt; i++)
			{
				if (bais[i]>0)
				{
					sum += (bais[i] - mean)*(bais[i] - mean);
				}
			}
			var = sum / (valid_point - 1);
		}
	}
	else       //垂直
	{
		for (int i = 0; i < _cnt; i++)
		{
			if (_edge_x[i] >= _x1&&_edge_x[i] <= _x2)
			{
				if (_edge_point[i]>0)
				{
					valid_point++;
					bais[i] = abs(_y1 - _edge_point[i]);
					if (bais[i] > 3)
					{
						deviation_point++;
					}
					deviation_sum += bais[i];
				}
			}
		}
		if (valid_point != 0 && (valid_point - 1) != 0)
		{
			mean = (float)deviation_sum / valid_point;
			float sum = 0;
			for (int i = 0; i < _cnt; i++)
			{
				if (bais[i]>0)
				{
					sum += (bais[i] - mean)*(bais[i] - mean);
				}
			}
			var = sum / (valid_point - 1);
		}
	}

	_corner_info->edge_feature[_edge_type].confidence = (10 - mean<0) ? 0 : (10 - mean);           // 用均值，方差和偏移点数来表示
	_corner_info->edge_feature[_edge_type].deviation = deviation_point;
	_corner_info->edge_feature[_edge_type].mean = mean;
	_corner_info->edge_feature[_edge_type].var = var;

	return ret;
}

//得到边的夹角
int GetAngle(EdgeInfo_ *_edge_a, EdgeInfo_ *_edge_b, int _point)   //可以直接利用degree信息
{
	int ret = 0;
	double degree_a = atan(_edge_a->k) / 3.14 * 180;   //水平边
	degree_a = abs(degree_a);

	double degree_b = 0.0;
	if (_edge_b->k != 0)
	{
		degree_b = atan(1 / _edge_b->k) / 3.14 * 180;   //垂直边
	}
	else
	{
		degree_b = 90;
	}
	degree_b = abs(degree_b);
	if (_edge_a->k>0)        //水平边斜率大于0
	{
		if (_point == 0 || _point == 2)
		{
			ret = ((180 - degree_b - degree_a) - (int)(180 - degree_b - degree_a)) >= 0.5 ? (int)(180 - degree_b - degree_a) + 1 : (int)(180 - degree_b - degree_a);   //四舍五入
		}
		else
		{
			ret = ret = ((degree_b + degree_a) - (int)(degree_b + degree_a)) >= 0.5 ? (int)(degree_b + degree_a) + 1 : (int)(degree_b + degree_a);
		}
	}
	else
	{
		if (_point == 0 || _point == 2)
		{

			ret = ret = ((degree_b + degree_a) - (int)(degree_b + degree_a)) >= 0.5 ? (int)(degree_b + degree_a) + 1 : (int)(degree_b + degree_a);
		}
		else
		{
			ret = ((180 - degree_b - degree_a) - (int)(180 - degree_b - degree_a)) >= 0.5 ? (int)(180 - degree_b - degree_a) + 1 : (int)(180 - degree_b - degree_a);
		}
	}

	return ret;

}

//计算折角的面积

int GetAreaOfBreak(CornerCorrectInfo_ *_corner_info,int _row_cnt,int _ver_cnt,int *_edge_x,int *_edge_x_point,int *_edge_y,int *_edge_y_point,int _point_type)
{
	int x_start = 0, y_start = 0;  //按大小
	int x_end = 0, y_end = 0;
	double k1 = 0.0, b1 = 0.0, k2 = 0.0, b2 = 0.0;
	
	int y = 0, deviation = 0;

	IntPoint_  point1 = { -1, -1 }, point2 = { -1, -1 }, point3 = {-1,-1};
	
	if (_point_type == TOP_LEFT_POINT)
	{
		x_start = _corner_info->coordinates.x[0];
		x_end = _corner_info->coordinates.x[1];

		y_start = _corner_info->coordinates.y[0];
		y_end = _corner_info->coordinates.y[3];

		point3.x = x_start;
		point3.y = y_start;

		k1 = _corner_info->up_edge_info.k;  //double类型，为0？
		b1 = _corner_info->up_edge_info.b;

		k2 = _corner_info->left_edge_info.k;
		b2 = _corner_info->left_edge_info.b;

		for (int i = 0; i < _row_cnt ; i++)   //水平上边
		{
			if (_edge_x[i] >= x_start&&_edge_x[i] <= x_end)
			{
				if (_edge_x_point[i]>0)
				{
					y = k1*_edge_x[i] + b1;
					deviation = abs(y - _edge_x_point[i]);
					if (deviation <= 3)                      //可以连续几个点
					{
						point1.x = _edge_x[i];
						//point1.y = _edge_x_point[i];
						point1.y = y;
						break;
					}
				}
			}
		}

		for (int i = 0; i < _ver_cnt ; i++)               //垂直左边
		{
			if (_edge_y[i] >= y_start&&_edge_y[i] <= y_end)
			{
				if (_edge_y_point[i]>0)
				{
					y = k2*_edge_y[i] + b2;
					deviation =abs(y - _edge_y_point[i]);
					if (deviation <= 3)
					{
						point2.y = _edge_y[i];             //交换
						point2.x =y;
						break;
					}
				}
			}
		}
	}

	if (_point_type == TOP_RIGHT_POINT)
	{
		x_start = _corner_info->coordinates.x[0];
		x_end = _corner_info->coordinates.x[1];

		y_start = _corner_info->coordinates.y[1];
		y_end = _corner_info->coordinates.y[2];

		point3.x = x_end;
		point3.y = y_start;

		k1 = _corner_info->up_edge_info.k;
		b1 = _corner_info->up_edge_info.b;

		k2 = _corner_info->right_edge_info.k;
		b2 = _corner_info->right_edge_info.b;

		for (int i = _row_cnt - 1; i >= 0 ; i--)       // 水平上边
		{
			if (_edge_x[i] >= x_start&&_edge_x[i] <= x_end)   
			{
				if (_edge_x_point[i]>0)
				{
					y = k1*_edge_x[i] + b1;
					deviation = abs(y - _edge_x_point[i]);
					if (deviation <= 3)                      //可以连续几个点
					{
						point1.x = _edge_x[i];
						point1.y = y;
						break;
					}
				}
			}
		}

		for (int i = 0; i < _ver_cnt ; i++)                //垂直右边
		{ 
			if (_edge_y[i] >= y_start&&_edge_y[i] <= y_end)
			{
				if (_edge_y_point[i]>0)
				{
					y = k2*_edge_y[i] + b2;
					deviation = abs(y - _edge_y_point[i]);
					if (deviation <= 3)
					{
						point2.y = _edge_y[i];
						point2.x = y;
						break;
					}
				}
			}
		}
	}
	if (_point_type == DOWN_RIGHT_POINT)
	{
		x_start = _corner_info->coordinates.x[3];
		x_end = _corner_info->coordinates.x[2];

		y_start = _corner_info->coordinates.y[1];
		y_end = _corner_info->coordinates.y[2];

		point3.x = x_end;
		point3.y = y_end;

		k1 = _corner_info->down_edge_info.k;
		b1 = _corner_info->down_edge_info.b;

		k2 = _corner_info->right_edge_info.k;
		b2 = _corner_info->right_edge_info.b;

		for (int i = _row_cnt - 1; i >= 0 ; i--)   //水平下边
		{
			if (_edge_x[i] >= x_start&&_edge_x[i] <= x_end)
			{
				if (_edge_x_point[i] > 0)
				{
					y = k1*_edge_x[i] + b1;
					deviation = abs(y - _edge_x_point[i]);
					if (deviation <= 3)                      //可以连续几个点
					{
						point1.x = _edge_x[i];
						point1.y = y;
						break;
					}
				}
			}
		}

		for (int i = _ver_cnt - 1; i >= 0 ; i--)
		{
			if (_edge_y[i] >= y_start&&_edge_y[i] <= y_end)
			{
				if (_edge_y_point[i] > 0)
				{
					y = k2*_edge_y[i] + b2;
					deviation = abs(y - _edge_y_point[i]);
					if (deviation <= 3)
					{
						point2.y = _edge_y[i];
						point2.x = y;
						break;
					}
				}
			}
		}
	}
	if (_point_type == DOWN_LEFT_POINT)
	{
		x_start = _corner_info->coordinates.x[3];
		x_end = _corner_info->coordinates.x[2];

		y_start = _corner_info->coordinates.y[0];
		y_end = _corner_info->coordinates.y[3];

		point3.x = x_start;
		point3.y = y_end;

		k1 = _corner_info->down_edge_info.k;
		b1 = _corner_info->down_edge_info.b;

		k2 = _corner_info->left_edge_info.k;
		b2 = _corner_info->left_edge_info.b;
		for (int i = 0; i < _row_cnt ; i++)    //水平下边
		{
			if (_edge_x[i] >= x_start&&_edge_x[i] <= x_end)
			{
				if (_edge_x_point[i]>0)
				{
					y = k1*_edge_x[i] + b1;
					deviation = abs(y - _edge_x_point[i]);
					if (deviation <= 3)                      //可以连续几个点
					{
						point1.x = _edge_x[i];
						point1.y = y;
						break;
					}
				}
			}
		}
		 
		for (int i = _ver_cnt-1; i > 0; i--)     //垂直左边
		{
			if (_edge_y[i] >= y_start&&_edge_y[i] <= y_end)
			{
				if (_edge_y_point[i]>0)
				{
					y = k2*_edge_y[i] + b2;
					deviation = abs(y - _edge_y_point[i]);
					if (deviation <= 3)
					{
						point2.y = _edge_y[i];
						point2.x = y;
						break;
					}
				}
			}
		}
	}
	
	CalculateArea(_corner_info,point1,point2,point3,_point_type);  //_point1为水平边点，_point2为垂直边点，_point3为角点
	//DrawEdgeCorner();

	return 0;
}

//根据三点和角度计算面积
int CalculateArea(CornerCorrectInfo_ *_corner_info, IntPoint_ _point1, IntPoint_ _point2, IntPoint_ _point3,int _point_type)  //_point1为水平边点； _point2为垂直边点
{
	if (_point1.x==-1||_point2.y==-1)
	{
		_corner_info->areaSize[_point_type] = -1.0;
		return -1;
	}
	float angel_ = 0;
	float length1 = sqrt(1.0*(_point3.x - _point1.x)*(_point3.x - _point1.x) + (_point3.y - _point1.y)*(_point3.y - _point1.y));
	float length2 = sqrt(1.0*(_point3.x - _point2.x)*(_point3.x - _point2.x) + (_point3.y - _point2.y)*(_point3.y - _point2.y));

	if (abs(_point1.x-_point3.x)<16+8) //判断正常的点
	{
		_corner_info->areaSize[_point_type] = 0.0;
		return 0;
	}
	if (abs(_point3.y-_point2.y)<8+4)
	{
		_corner_info->areaSize[_point_type] = 0.0;
		return 0;
	}

	float area = 0.5*length1*length2;
	if (_point_type==TOP_LEFT_POINT)
	{
		angel_ = _corner_info->angle.angle_up_left;
	}
	if (_point_type==TOP_RIGHT_POINT)
	{
		angel_ = _corner_info->angle.angle_up_right;
	}
	if (_point_type == DOWN_RIGHT_POINT)
	{
		angel_ = _corner_info->angle.angle_down_right;
	}
	if (_point_type==DOWN_LEFT_POINT)
	{
		angel_ = _corner_info->angle.angle_down_left;
	}
	angel_ = (angel_ / 180)*3.14;
	_corner_info->areaSize[_point_type] = area*sin(angel_);

	return 0;
}

//删除在上下边上的伪左右边缘点
//*_x为点的x坐标, *_y为点的y坐标, _cnt为点数, *_edge_line_info为边信息结构体
void DelErrPoint(int *_x, int *_y, int _cnt, EdgeInfo_ *_edge_line_info)
{
	int i;
	int num = _cnt / 2;
	double deviation;

	if (_edge_line_info->type == 0)   //对照边为上边
	{
		for (i = 0; i < num; i++)
		{
			if (_y[i] >= 0)
			{
				deviation = abs((_edge_line_info->k*_y[i] + _edge_line_info->b) - _x[i]);
				if (deviation <= 3)     //去掉左右边的点中实际上在上边的点
				{
					_y[i] = -1;
				}
				else
				{
					break;
				}
			}
		}
	}
	else        //对照边为下边
	{
		for (i = _cnt - 1; i >= num; i--)
		{
			if (_y[i] >= 0)
			{
				deviation = abs((_edge_line_info->k*_y[i] + _edge_line_info->b) - _x[i]);
				if (deviation <= 3)      //去掉左右边的点中实际上在下边的点
				{
					_y[i] = -1;
				}
				else
				{
					break;
				}
			}
		}
	}
}

//高斯消元法解线性方程组求得仿射变换参数
int gaus(double _a[], double _b[], int _n)
{
	int l, k, i, j, is, p, q;
	double d, t;
	int js[6];
	//    js=(int *)malloc(n*sizeof(int));
	l = 1;
	for (k = 0; k <= _n - 2; k++)
	{
		d = 0.0;
		for (i = k; i <= _n - 1; i++)
			for (j = k; j <= _n - 1; j++)
			{
			t = fabs(_a[i*_n + j]);
			if (t>d) { d = t; js[k] = j; is = i; }
			}
		if (d + 1.0 == 1.0) l = 0;
		else
		{
			if (js[k] != k)
				for (i = 0; i <= _n - 1; i++)
				{
				p = i*_n + k; q = i*_n + js[k];
				t = _a[p]; _a[p] = _a[q]; _a[q] = t;
				}
			if (is != k)
			{
				for (j = k; j <= _n - 1; j++)
				{
					p = k*_n + j; q = is*_n + j;
					t = _a[p]; _a[p] = _a[q]; _a[q] = t;
				}
				t = _b[k]; _b[k] = _b[is]; _b[is] = t;
			}
		}
		if (l == 0)
		{ //free(js); printf("fail\n");
			return(0);
		}
		d = _a[k*_n + k];
		for (j = k + 1; j <= _n - 1; j++)
		{
			p = k*_n + j; _a[p] = _a[p] / d;
		}
		_b[k] = _b[k] / d;
		for (i = k + 1; i <= _n - 1; i++)
		{
			for (j = k + 1; j <= _n - 1; j++)
			{
				p = i*_n + j;
				_a[p] = _a[p] - _a[i*_n + k] * _a[k*_n + j];
			}
			_b[i] = _b[i] - _a[i*_n + k] * _b[k];
		}
	}
	d = _a[(_n - 1)*_n + _n - 1];
	if (fabs(d) + 1.0 == 1.0)
	{ //free(js); printf("fail\n");
		return(0);
	}
	_b[_n - 1] = _b[_n - 1] / d;
	for (i = _n - 2; i >= 0; i--)
	{
		t = 0.0;
		for (j = i + 1; j <= _n - 1; j++)
			t = t + _a[i*_n + j] * _b[j];
		_b[i] = _b[i] - t;
	}
	js[_n - 1] = _n - 1;
	for (k = _n - 1; k >= 0; k--)
		if (js[k] != k)
		{
		t = _b[k]; _b[k] = _b[js[k]]; _b[js[k]] = t;
		}
	// free(js);
	return(1);
}

//计算钱的实际长度
//*_src为输入图像, *_corner_info为角点信息结构体, *_coordinates为图像中纸币的角点坐标
//*_src为输入图像, *_corner_info为角点信息结构体
int CalcRealLengthOfMoney(unsigned char *_src, CornerCorrectInfo_ *_corner_info)
{
	int i;
	int width = _corner_info->width;
	int height = _corner_info->height;
	int width_step = _corner_info->width_step;

	unsigned char *code_value = data_buffer;//(int*)malloc(height*sizeof(int));

	if (_corner_info->width < 725)   //图像宽度小于725，无法获取码盘值，返回0
	{
		return 0;
	}
	for (i = 0; i < height; i++)   //获取码盘值
	{
		code_value[i] = _src[i*width_step + 724];   //图像第724列为码盘值
	}

	IntPoint_ corner[4];

	for (i = 0; i < 4; i++)
	{
		corner[i].x = _corner_info->coordinates.x[i];
		corner[i].y = _corner_info->coordinates.y[i];
	}

	_corner_info->edge_real_len[0] = CalcEdgeRealLength(corner[0], corner[1], height, code_value);
	_corner_info->edge_real_len[1] = CalcEdgeRealLength(corner[3], corner[2], height, code_value);
	_corner_info->edge_real_len[2] = CalcEdgeRealLength(corner[3], corner[0], height, code_value);
	_corner_info->edge_real_len[3] = CalcEdgeRealLength(corner[2], corner[1], height, code_value);

#ifdef FILE_PRINTF
	int truelen = TRUE_LENGTH;
	float len = _corner_info->up_edge_real_len;
	fprintf(fp1, "picture %d, %f\n", filenum,len);
	if (len > 0)
	{
		if (len < min_len)
		{
			min_len = len;
		}
		else if (len > max_len)
		{
			max_len = len;
		}
		if (len > truelen + 2 || len < truelen - 2)
		{
			file_error++;
			fprintf(fp2, "%s\n", fileName);
		}
	}
	else
	{
		file_out_range++;
	}
#endif

	return 0;
}


/*---------------------------------------------------
*半张钱的角点检测
*----------------------------------------------------*/
//找到钱的四个角点坐标
//*_src为输入图像, *_corner_info为角点信息结构体
//找到钱的四个角点坐标
//*_src为输入图像, *_corner_info为角点信息结构体, _last_status为图像上次的状态
int FindHalfMoneyCorner(unsigned char *_src, CornerCorrectInfo_ *_corner_info, int _last_status)
{
	int width = _corner_info->width;
	int height = _corner_info->height;
	int row_point_num;  //横边采样点数
	int vert_point_num; //纵边采样点数
	int background_thred = RMB_BACKGROUND_THRED;   //背景阈值
	int confidence_thred = EDGE_CONFIDENCE_THRED;
	int ret;
	int i;

	int parallel_thred;
	int switch_temp;

	IplImage_ src_img;  //图像数组

	ReleaseImage(&src_img);
	CreateImage(&src_img, width, height, _src);    //创建图像
	src_img.width_step = _corner_info->width_step;

	//gClearDirt(&src_img);   //清除CIS积灰造成的背景灰条

	//所有币种统一用这个
	ret = GetHalfMoneyEdgePoint(&src_img, _corner_info, background_thred, &row_point_num, &vert_point_num, _last_status);
	if (ret < 0)
		return ret;

	_corner_info->up_edge_info.type = 0;   //确定边的类型
	_corner_info->down_edge_info.type = 1;
	_corner_info->left_edge_info.type = 2;
	_corner_info->right_edge_info.type = 3;

	//最小二乘法拟合直线
	//下边缘被截去，只拟合上边
	ret = FittingEdge(background_thred, edge_x, edge_x_up, row_point_num, 0, &_corner_info->up_edge_info, &src_img);	//up
	DelErrPoint(edge_y, edge_x_left, vert_point_num, &_corner_info->up_edge_info);
	DelErrPoint(edge_y, edge_x_right, vert_point_num, &_corner_info->up_edge_info);
	_corner_info->down_edge_info.b = height - 1;
	
	ret |= FittingEdge(background_thred, edge_y, edge_x_left, vert_point_num, 1, &_corner_info->left_edge_info, &src_img);	//left
	ret |= FittingEdge(background_thred, edge_y, edge_x_right, vert_point_num, 1, &_corner_info->right_edge_info, &src_img);	//right

	if (ret < 0)  //拟合出错，报错返回
	{
		return ERR_CORNER_EDGE_DEGREE;
	}
	if (_corner_info->up_edge_info.degree >= 50 || _corner_info->down_edge_info.degree >= 50 || _corner_info->up_edge_info.degree <= -50 || _corner_info->down_edge_info.degree <= -50) //倾角过大，报错返回
	{
		return ERR_CORNER_EDGE_DEGREE;
	}

	//判断左右是否平行，修正错误边
	parallel_thred = abs(_corner_info->left_edge_info.degree + _corner_info->right_edge_info.degree) / 2;
	parallel_thred = (int)(parallel_thred*0.15 + 0.5);
	if (parallel_thred < 8)
	{
		parallel_thred = 8;
	}
	switch_temp = abs(_corner_info->left_edge_info.degree) - abs(_corner_info->right_edge_info.degree);
	if (switch_temp< -parallel_thred || switch_temp>parallel_thred)   //若左右边相差大于8度
	{
		if (_corner_info->left_edge_info.confidence >= confidence_thred)
		{
			if (_corner_info->right_edge_info.confidence >= confidence_thred)
			{
				//若左右边置信度均大于阈值且右边置信度大于左边，以右边为正确边，修正左边
				if (_corner_info->right_edge_info.confidence > _corner_info->left_edge_info.confidence)
				{
					ret = FittingEdgeRefer(edge_y, edge_x_left, vert_point_num, 1, &_corner_info->left_edge_info, &_corner_info->right_edge_info);	//left
				}
				else
				{
					ret = FittingEdgeRefer(edge_y, edge_x_right, vert_point_num, 1, &_corner_info->right_edge_info, &_corner_info->left_edge_info);	//right
				}
			}
			else   //若左边置信度大于阈值而右边置信度小于阈值，修正右边
			{
				ret = FittingEdgeRefer(edge_y, edge_x_right, vert_point_num, 1, &_corner_info->right_edge_info, &_corner_info->left_edge_info);	//right
			}
		}
		//若左边置信度小于阈值而右边置信度大于阈值，修正左边
		else if (_corner_info->right_edge_info.confidence >= confidence_thred)
		{
			ret = FittingEdgeRefer(edge_y, edge_x_left, vert_point_num, 1, &_corner_info->left_edge_info, &_corner_info->right_edge_info);	//left
		}
		else    //若左右边置信度均小于阈值，报错
		{
			return  ERR_CORNER_PARALLEL_DEGREE;
		}
		if (ret < 0)
		{
			return ERR_CORNER_PARALLEL_DEGREE;
		}
	}

	FloatPoint_ corner[4];  //角点坐标
	CalcCrosspoint(&_corner_info->up_edge_info, &_corner_info->down_edge_info, &_corner_info->left_edge_info, corner, corner + 3);    //计算四个角点坐标
	CalcCrosspoint(&_corner_info->up_edge_info, &_corner_info->down_edge_info, &_corner_info->right_edge_info, corner + 1, corner + 2);

	for (i = 0; i < 4; i++)     //图中钱的四个角坐标
	{
		_corner_info->coordinates.x[i] = corner[i].x >= 0 ? (int)(corner[i].x + 0.5) : (int)(corner[i].x - 0.5);
		_corner_info->coordinates.y[i] = corner[i].y >= 0 ? (int)(corner[i].y + 0.5) : (int)(corner[i].y - 0.5);
	}

	_corner_info->edge_x = edge_x;
	_corner_info->edge_y = edge_y;
	_corner_info->edge_x_up = edge_x_up;
	_corner_info->edge_x_down = edge_x_down;
	_corner_info->edge_x_left = edge_x_left;
	_corner_info->edge_x_right = edge_x_right;

	return 0;
}

//找到半张钱的三条边上的点
//*_src_img为输入图像, *__corner_info为角点信息结构体, _thred为背景阈值, *_row_point_num为横边点数, *_vert_point_num为纵边点数, _last_status为图像上次的状态
int GetHalfMoneyEdgePoint(IplImage_ *_src_img, CornerCorrectInfo_ *_corner_info, int _thred, int *_row_point_num, int *_vert_point_num, int _last_status)
{
	int i;  //循环变量
	int j;
	int k;
	int cnt;
	int width = _src_img->width;
	int height = _src_img->height;
	int width_step = _src_img->width_step;  //源图步长
	int edge_up_cnt = 0;  //找到的上边缘点数
	int edge_up_cnt1 = 0;   //贴边的上边缘点数
	int img_ignore_point_l;   //忽略的左边列数
	int img_ignore_point_r;
	int row_step = 2 << (ROW_2_POWER - 1);   //横边找点的步长
	int vert_step = 2 << (VERT_HALF_2_POWER - 1);

	unsigned char *psrc;

	memset(edge_x_up, -1, (width >> ROW_2_POWER)*sizeof(int));
	memset(edge_x_down, -1, (width >> ROW_2_POWER)*sizeof(int));
	memset(edge_x_left, -1, (height >> VERT_HALF_2_POWER)*sizeof(int));
	memset(edge_x_right, -1, (height >> VERT_HALF_2_POWER)*sizeof(int));

	_thred = _thred & 0xff;

	//解决左右边太靠近白边问题
	if (0 != IgnoreCISCol(&img_ignore_point_l, &img_ignore_point_r, _src_img))
		return -1;	//??

	int cross_rows = 16;
	int cross_cols = 8;

	//找上边缘点
	for (i = img_ignore_point_l >> ROW_2_POWER; i<((width - img_ignore_point_r) >> ROW_2_POWER); i++)
	{
		psrc = _src_img->image_data + row_step * i;
		for (j = cross_rows; j<height * 2 / 3; j += cross_rows)     //从上往下找上边缘点
		{
			if (*(psrc + j*width_step) > _thred)  //先隔croos_rows行找到像素值>阈值的点
			{
				cnt = 0;
				for (k = 0; k < cross_rows + 3; k++)  //再逐行找
				{
					if (*(psrc + ((j - cross_rows) + k)*width_step) > _thred)
					{
						if (++cnt > 3)      //连续4行大于阈值，点存入数组
						{
							edge_x_up[i] = (j - cross_rows) + k - 3;
							break;
						}
					}
					else
					{
						cnt = 0;
					}
				}
				if (cnt > 3)   //找到了，判断是否为贴边点，跳出
				{
					if (edge_x_up[i] == 0)   //贴边点记为-2
					{
						edge_x_up[i] = -2;
						edge_up_cnt1++;
					}
					else
					{
						edge_up_cnt++;
					}
					break;
				}
			}
		}
	}

	if (edge_up_cnt + edge_up_cnt1<20)	//如果边缘查找连1半都不到，说明图像源有问题
	{
		if (1 == (_last_status & 0x01))	//上次是刹车状态
		{
			return ERR_CORNER_IMGSOURCE;				//判断为图像源错误
		}
		else
		{
			return ERR_CORNER_IMGCIS;        //判断为CIS错误
		}
	}

	if (edge_up_cnt1>25 && edge_up_cnt<5)
	{
		for (i = 0; i<width >> ROW_2_POWER; i++)
		{
			if (edge_x_up[i] == -2)
			{
				edge_x_up[i] = 0;
			}
		}
	}

	//through y
	//找左边缘点和右边缘点
	for (i = 0; i < (height >> VERT_HALF_2_POWER); i++)
	{
		psrc = _src_img->image_data + width_step*(i << VERT_HALF_2_POWER);
		for (j = img_ignore_point_l + cross_cols; j < width / 3; j += cross_cols)   //从左往右找左边缘点
		{
			if (*(psrc + j) > _thred)         //先隔croos_cols列找到像素值>阈值的点
			{
				cnt = 0;
				for (k = 0; k < cross_cols + 3; k++)   //再逐列找
				{
					if (*(psrc + (j - cross_cols) + k) > _thred)
					{
						if (++cnt > 3)     //连续4列大于阈值，点存入数组
						{
							edge_x_left[i] = (j - cross_cols) + k - 3;
							break;
						}
					}
					else
					{
						cnt = 0;
					}
				}
				if (cnt > 3)   //找到了，跳出
				{
					break;
				}
			}
		}

		for (j = width - img_ignore_point_r - cross_cols; j >= 2 * width / 3; j -= cross_cols)   //从右往左找右边缘点
		{
			if (*(psrc + j) > _thred)
			{
				cnt = 0;
				for (k = 0; k < cross_cols + 3; k++)
				{
					if (*(psrc + (j + cross_cols) - k) > _thred)
					{
						if (++cnt > 3)
						{
							edge_x_right[i] = (j + cross_cols) - k + 3;
							break;
						}
					}
					else
					{
						cnt = 0;
					}

				}
				if (cnt > 3)  //找到了，跳出
				{
					break;
				}
			}
		}
	}

	//下面的两个循环不应该每次计算。  ？？
	for (i = 0; i<(width >> ROW_2_POWER); i++)
	{
		edge_x[i] = i * row_step;
	}

	for (i = 0; i<(height >> VERT_HALF_2_POWER); i++)
	{
		edge_y[i] = i * vert_step;
	}

	*_row_point_num = width >> ROW_2_POWER;
	*_vert_point_num = height >> VERT_HALF_2_POWER;
	return 0;
}

//计算半张钱的实际长度
//*_src为输入图像, *_corner_info为角点信息结构体, *_coordinates为图像中纸币的角点坐标
int CalcHalfRealLengthOfMoney(unsigned char *_src, CornerCorrectInfo_ *_corner_info, CoordinatesInfo_ *_coordinates)
{
	int i;
	int width = _corner_info->width;
	int height = _corner_info->height;
	int width_step = _corner_info->width_step;
	int code_col = 724;

	unsigned char *code_value = data_buffer;//(int*)malloc(height*sizeof(int));

	if (_corner_info->width < 725)   //图像宽度小于725，无法获取码盘值，返回0
	{
		return 0;
	}
	for (i = 0; i < height; i++)   //获取码盘值
	{
		code_value[i] = _src[i*width_step + code_col];   //图像第724列为码盘值
	}

	IntPoint_ corner[4];

	for (i = 0; i < 4; i++)
	{
		corner[i].x = _coordinates->x[i];
		corner[i].y = _coordinates->y[i];
	}

	_corner_info->edge_real_len[0] = CalcEdgeRealLength(corner[0], corner[1], height, code_value);
	float len;
	if (corner[0].y > corner[1].y)
	{
		_corner_info->edge_real_len[2] = CalcEdgeRealLength(corner[3], corner[0], height, code_value);
		len = _corner_info->edge_real_len[2];
	}
	else
	{
		_corner_info->edge_real_len[3] = CalcEdgeRealLength(corner[2], corner[1], height, code_value);
		len = _corner_info->edge_real_len[3];
	}

#ifdef FILE_PRINTF
	int truelen = 76;
	//float len = _corner_info->left_edge_real_len;
	fprintf(fp1, "picture %d, %f\n", filenum,len);
	if (len > 0)
	{
		if (len < min_len)
		{
			min_len = len;
		}
		else if (len > max_len)
		{
			max_len = len;
		}
		if (len > truelen + 2 || len < truelen - 2)
		{
			file_error++;
		}
	}
	else
	{
		file_out_range++;
	}
#endif

	return 0;
}

//计算钱的一条边的实际长度并返回
//_point0, _point1为两个点坐标, _height为图像高度, *_code_data为码盘值数据
float CalcEdgeRealLength(IntPoint_ _point0, IntPoint_ _point1, int _height, unsigned char *_code_data)
{	
	float hori_len = (abs(_point1.x - _point0.x)+1)*0.254;
	float vert_len;
	float real_length;
	int up;
	int down;

	if (_point0.y == _point1.y)
	{
		return hori_len;
	}

	if (_point0.y < _point1.y)
	{
		up = _point0.y;
		down = _point1.y;
	}
	else
	{
		up = _point1.y;
		down = _point0.y;
	}
	if (up < 0 || down>_height-1)  //点的y坐标超出范围，无法获取码盘值，无法计算实际长度，返回-999
	{
		return -999;
	}

	float code_len = GetCodeLength(up, down, _code_data);  //计算两点间纵向码盘长度


	vert_len = code_len*CODE_LENGTH;
	real_length = sqrt(hori_len*hori_len + vert_len*vert_len);

	return real_length;
}

//获取两点间纵向码盘长度
//_up为较小的y坐标, _down为较大的y坐标, *_code_data为码盘值数据
float GetCodeLength(int _up, int _down, unsigned char *_code_data)
{
	int i;
	
	float code_len;
	float first_code_len;   //第一个码盘值的长度
	float last_code_len;    //最后一个码盘值的长度
	int code_val=-1;
	int cnt=-1;
	int num = _down - _up + 1;     //点数
	int *vert_point = (int *)malloc((_down - _up + 1)*sizeof(int));    //存放不同码盘值的个数
		
	for (i = 0; i <= _down-_up; i++)
	{
		vert_point[i] = 0;
	}

	for (i = _up; i <= _down; i++)
	{
		if (_code_data[i] != code_val)   //若当前码盘值不等于上一个码盘值，
		{
			code_val = _code_data[i];
			cnt++;
		}
		vert_point[cnt]++;
	}
	cnt++;

	if (cnt > 1)       //若有超过1种码盘值
	{
		first_code_len = (float)vert_point[0] / vert_point[1];
		if (first_code_len > 1)
		{
			first_code_len = 1;
		}
		last_code_len = (float)vert_point[cnt - 1] / vert_point[cnt - 2];
		if (last_code_len > 1)
		{
			last_code_len = 1;
		}
		code_len = first_code_len + last_code_len + cnt - 2;
	}
	//else if (cnt == 2)   //若只有2种码盘值
	//{
	//	first_code_len = (float)vert_point[0];
	//}
	else if (cnt == 1)   //若只有1种码盘值
	{
		code_len = 1;
	}

	free(vert_point);
	return code_len;

}

//得到半张图的仿射变换用的坐标
//_dst_tri[]为校正后坐标,*_corner_info为角点信息结构体, _affine_width为仿射后宽度, _affine_height为仿射后高度, _src_tri[]为钱的坐标,
void CalcHalfAffineCoordinate(IntPoint_ _dst_tri[], CornerCorrectInfo_ *_corner_info, int _affine_width, int _affine_height, IntPoint_ _src_tri[])
{
	_dst_tri[0].y = (_corner_info->height - _affine_height) / 2;   //确定校正后起点坐标
	_dst_tri[0].x = (_corner_info->width - _affine_width) / 2;

	//确定两组坐标对应的位置
	if (_corner_info->edge_real_len[2] > 0)  //以down面左边为准
	{
		_dst_tri[1].y = _dst_tri[0].y;
		_dst_tri[1].x = _dst_tri[0].x + _affine_width;

		_dst_tri[2].y = _dst_tri[0].y + _affine_height;
		_dst_tri[2].x = _dst_tri[0].x + _affine_width;

		_src_tri[0].x = _corner_info->coordinates.x[0];
		_src_tri[0].y = _corner_info->coordinates.y[0];
		_src_tri[1].x = _corner_info->coordinates.x[1];
		_src_tri[1].y = _corner_info->coordinates.y[1];
		_src_tri[2].x = _corner_info->coordinates.x[2];
		_src_tri[2].y = _corner_info->coordinates.y[2];
	}
	else     //以down面右边为准
	{
		_dst_tri[1].y = _dst_tri[0].y;
		_dst_tri[1].x = _dst_tri[0].x + _affine_width;

		_dst_tri[2].y = _dst_tri[0].y + _affine_height;
		_dst_tri[2].x = _dst_tri[0].x;

		_src_tri[0].x = _corner_info->coordinates.x[0];
		_src_tri[0].y = _corner_info->coordinates.y[0];
		_src_tri[1].x = _corner_info->coordinates.x[1];
		_src_tri[1].y = _corner_info->coordinates.y[1];
		_src_tri[2].x = _corner_info->coordinates.x[3];
		_src_tri[2].y = _corner_info->coordinates.y[3];
	}
}

//得到半张钱另一面的仿射坐标
//_other_tri[]为另一面的坐标, *_corner_info为角点信息结构体, *_coordinates1为另一面图像中纸币角点坐标
int GetHalfOtherCoordinate(IntPoint_ _other_tri[], CornerCorrectInfo_ *_corner_info, CoordinatesInfo_ *_coordinates1)
{
	//确定两组坐标对应的位置
	if (_corner_info->edge_real_len[2] > 0)    //以左边为准
	{
		_other_tri[0].x = _coordinates1->x[0];
		_other_tri[0].y = _coordinates1->y[0];
		_other_tri[1].x = _coordinates1->x[1];
		_other_tri[1].y = _coordinates1->y[1];
		_other_tri[2].x = _coordinates1->x[3];
		_other_tri[2].y = _coordinates1->y[3];
	}
	else
	{
		_other_tri[0].x = _coordinates1->x[0];
		_other_tri[0].y = _coordinates1->y[0];
		_other_tri[1].x = _coordinates1->x[1];
		_other_tri[1].y = _coordinates1->y[1];
		_other_tri[2].x = _coordinates1->x[2];
		_other_tri[2].y = _coordinates1->y[2];
	}

	return 0;
}

int GetPointToEdgeLength(EdgeInfo_ *edge, IntPoint_ *point)
{
	int ret = -1;
	 
	if (abs(edge->b)>=1e-15||abs(edge->k)>=1e-15)  //非0
	{
		ret=abs(edge->k*point->x - point->y + edge->b) / sqrt(1 + edge->k*edge->k);
	}
	if (abs(edge->k)<1e-15)
	{
		ret = abs(edge->b - point->y);
	}
	return ret;
}