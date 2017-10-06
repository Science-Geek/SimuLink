/***************************************************************
* Copyright (C) 2011, Wuhan University
* All rights reserved.
*
* �ļ�����	FindCorner.c
* �����������ǵ�У��ģ��
*
* ��ǰ�汾��1.0
* ��    �ߣ�������
* ������ڣ�2016��9��12��
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



#define CODE_LENGTH   0.6   //�г��Ŵ�0.77715
                              //����1.13

//��̬��������
static int edge_x[128];     //���x����
static int edge_y[128];     //�ݱ�y����
static int edge_x_up[128];    //�ϱ�y����
static int edge_x_down[128];
static int edge_x_left[128];   //���x����
static int edge_x_right[128];
static int edge_data_buff[128];   //�����ݻ���

static int fitting_deviation[100];    //ÿ�������ֱ�ߵ�ƫ��
static int fitting_point_valid[100];  //ÿ���������ֵ

static int fitting_difference[100];
static int fitting_targe_diff_mean[100];
static int fitting_targe_num[100];
static int fitting_targe_point[100];

unsigned char data_buffer[1024];
extern unsigned char CropImg[3276800];

extern FILE *fp1;   //ȫ�ֱ������ⲿ����
extern float min_len;
extern float max_len;
extern int file_out_range;
extern int file_error;

extern unsigned int filenum;

static int RMB_BACKGROUND_THRED = 50;  //������ֵ�����޸ģ�
static int EDGE_DEGREE_THRED = 25;    //ͼ�������б��ֵ���ڲ���
static float EDGE_REFER_THRED = 1.5;   //������-��ǲ�-��ֵ���ڲ���


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

//���������ߵĽ�������
//*_line_udΪ�ϱ߻��±�,*_line_lrΪ������ұ�
INLINE void CalcCrosspoint(EdgeInfo_ *_line_up, EdgeInfo_ *_line_down, EdgeInfo_ *_line_lr, FloatPoint_ *_point_up, FloatPoint_ *_point_down)
{
	float k_temp;  //���ұ߾���xy����ת�����k,b
	float b_temp;

	if (_line_lr->k != 0)   //����ֱ�߷����󽻵�
	{
		k_temp = 1 / _line_lr->k;   //xy����ת��
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

//�õ���һ��ķ�������
//_other_tri[]Ϊ��һ�������, *_corner_infoΪ�ǵ���Ϣ�ṹ��, mapping_offsetΪCISƫ��
int GetOtherCoordinate(IntPoint_ _other_tri[], CornerCorrectInfo_ *_corner_info, CoordinatesInfo_ *_coordinates1)
{
	//ȷ�����������Ӧ��λ��
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

//�õ�����任�õ�����
//_dst_tri[]ΪУ��������,*_corner_infoΪ�ǵ���Ϣ�ṹ��, _affine_widthΪ�������, _affine_heightΪ�����߶�, _src_tri[]ΪǮ������,
void CalcAffineCoordinate(IntPoint_ _dst_tri[], int _width, int _height, int _affine_width, int _affine_height, CoordinatesInfo_ *_coordinates, IntPoint_ _src_tri[], int _tri_type)
{
	_dst_tri[0].y = (_height - _affine_height) / 2;   //ȷ��У�����������
	_dst_tri[0].x = (_width - _affine_width) / 2;

	//ȷ�����������Ӧ��λ��
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

//�������������÷���任����
//_src_tri[]ΪУ��ǰ����, _dst_tri[]ΪУ��������, *_corner_infoΪ�ǵ���Ϣ�ṹ��
void GetAffineMat(IntPoint_ _src_tri[], IntPoint_ _dst_tri[], int _affine_operator_trans[])
{
	int i;   //ѭ������
	double a[6 * 6];    //����任��ʽ����
	double affine_operator[6];

	for (i = 0; i < 3; i++)   //���ݷ���任��ʽȷ����Ӧ����
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
	gaus(a, affine_operator, 6);  //��˹��Ԫ�������Է�����÷���任����

	for (i = 0; i<6; i++)
	{
		_affine_operator_trans[i] = (int)(affine_operator[i] * 65536 >= 0 ? affine_operator[i] * 65536 + 0.5 : affine_operator[i] * 65536 - 0.5);
	}
}
#if 0
//�ҵ�Ǯ���������ϵĵ�
//*_img_breakage_checkΪͼ�������⿪��, *_src_imgΪ����ͼ��, *__corner_infoΪ�ǵ���Ϣ�ṹ��, _thredΪ������ֵ, *_row_point_numΪ��ߵ���, *_vert_point_numΪ�ݱߵ���, _last_statusΪͼ���ϴε�״̬
int GetMoneyEdgePoint(int *_img_breakage_check, IplImage_ *_src_img, CornerCorrectInfo_ *_corner_info, int _thred, int *_row_point_num, int *_vert_point_num, int _last_status)
{
	int i;  //ѭ������
	int j;
	int k;
	int cnt;
	int width = _src_img->width;
	int height = _src_img->height;
	int width_step = _src_img->width_step;  //Դͼ����
	int edge_up_cnt = 0;    //�ҵ����ϱ�Ե����
	int edge_up_cnt1 = 0;   //���ߵ��ϱ�Ե����
	int edge_down_cnt = 0;
	int edge_down_cnt1 = 0;
	int img_ignore_point_l;   //���Ե��������
	int img_ignore_point_r;
	int row_step = 2 << (ROW_2_POWER - 1);   //����ҵ�Ĳ���
	int vert_step = 2 << (VERT_2_POWER - 1);

	unsigned char *psrc;

	memset(edge_x_up, -1, (width >> ROW_2_POWER)*sizeof(int));
	memset(edge_x_down, -1, (width >> ROW_2_POWER)*sizeof(int));
	memset(edge_x_left, -1, (height >> VERT_2_POWER)*sizeof(int));
	memset(edge_x_right, -1, (height >> VERT_2_POWER)*sizeof(int));

	_thred = _thred & 0xff;
	*_img_breakage_check = 1;   	//��ʼ������Ϊ��������������

	////������ұ�̫�����ױ�����
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

	int cross_rows = 16; //��
	int cross_cols = 8;  //��

	//���ϱ�Ե����±�Ե��
	for (i = img_ignore_point_l >> ROW_2_POWER; i<((width - img_ignore_point_r) >> ROW_2_POWER); i++)
	{
		psrc = _src_img->image_data + row_step * i; 
		for (j = cross_rows; j<height * 2 / 3; j += cross_rows)     //�����������ϱ�Ե��
		{
			if (*(psrc + j*width_step) > _thred)  //�ȸ�croos_rows���ҵ�����ֵ>��ֵ�ĵ�
			{
				cnt = 0;
				for (k = 0; k < cross_rows+3; k++)  //��������
				{
					if (*(psrc + ((j - cross_rows) + k)*width_step) > _thred)
					{
						if (++cnt > 3)      //����4�д�����ֵ�����������
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
				if (cnt > 3)   //�ҵ��ˣ��ж��Ƿ�Ϊ���ߵ㣬����
				{
					if (edge_x_up[i] == 0)   //���ߵ��Ϊ-2������������
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
		//�����������±�Ե��
		if (edge_x_up[i] >= 0 || edge_x_up[i] == -2)
		{
			for (j = height - 1 - cross_rows; j >= height / 3; j -= cross_rows)  
			{
				if (*(psrc + j*width_step) > _thred)      //�ȸ�croos_rows���ҵ�����ֵ>��ֵ�ĵ�
				{
					cnt = 0;
					for (k = 0; k < cross_rows+3; k++)   //��������
					{
						if (*(psrc + ((j + cross_rows) - k)*width_step) > _thred)
						{
							if (++cnt > 3)      //����4�д�����ֵ�����������
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
					if (cnt > 3)     //�ҵ��ˣ��ж��Ƿ�Ϊ���ߵ㣬����
					{
						if (edge_x_down[i] == height - 1)   //���ߵ��Ϊ-2������������
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
	
	if (edge_up_cnt+edge_up_cnt1<20)	//�����Ե������1�붼������˵��ͼ��Դ������
	{
		if (1 == (_last_status & 0x01))	//�ϴ���ɲ��״̬
		{
			return ERR_CORNER_IMGSOURCE;				//�ж�Ϊͼ��Դ����
		}
		else
		{
			return ERR_CORNER_IMGCIS;        //�ж�ΪCIS����
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
	//�����Ե����ұ�Ե��
	for (i = 0; i < (height >> VERT_2_POWER); i++)   
	{
        psrc = _src_img->image_data + width_step*(i << VERT_2_POWER);
		for (j = img_ignore_point_l + cross_cols; j < width / 3; j += cross_cols)   //�������������Ե��
		{
			if (*(psrc + j) > _thred)         //�ȸ�croos_cols���ҵ�����ֵ>��ֵ�ĵ�
			{
				cnt = 0;
				for (k = 0; k < cross_cols+3; k++)   //��������
				{
					if (*(psrc + (j - cross_cols) + k) > _thred)
					{
						if (++cnt > 3)     //����4�д�����ֵ�����������
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
				if (cnt > 3)   //�ҵ��ˣ�����
				{
					break;
				}			
			}			
		}

		for (j = width - img_ignore_point_r - cross_cols; j >= 2 * width / 3; j -= cross_cols)   //�����������ұ�Ե��
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
				if (cnt > 3)  //�ҵ��ˣ�����
				{
					break;
				}
			}
		}		
	}

	//���������ѭ����Ӧ��ÿ�μ��㡣  ����
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

//�ҵ�Ǯ���������ϵĵ�
//*_img_breakage_checkΪͼ�������⿪��, *_src_imgΪ����ͼ��, *__corner_infoΪ�ǵ���Ϣ�ṹ��, _thredΪ������ֵ, *_row_point_numΪ��ߵ���, *_vert_point_numΪ�ݱߵ���, _last_statusΪͼ���ϴε�״̬
#if 1
int GetMoneyEdgePoint(int *_img_breakage_check, IplImage_ *_src_img, CornerCorrectInfo_ *_corner_info, int _thred, int *_row_point_num, int *_vert_point_num, int _last_status)
{
	int i;  //ѭ������
	int j;
	int k;
	int cnt;
	int width = _src_img->width;
	int height = _src_img->height;
	int width_step = _src_img->width_step;  //Դͼ����
	int edge_up_cnt = 0;  //�ҵ����ϱ�Ե����
	int edge_up_cnt1 = 0;   //���ߵ��ϱ�Ե����
	int edge_down_cnt = 0;
	int edge_down_cnt1 = 0;
	int img_ignore_point_l;   //���Ե��������
	int img_ignore_point_r;
	int row_step = 2 << (ROW_2_POWER - 1);   //����ҵ�Ĳ���
	int vert_step = 2 << (VERT_2_POWER - 1);

	unsigned char *psrc;

	memset(edge_x_up, -1, (width >> ROW_2_POWER)*sizeof(int));
	memset(edge_x_down, -1, (width >> ROW_2_POWER)*sizeof(int));
	memset(edge_x_left, -1, (height >> VERT_2_POWER)*sizeof(int));
	memset(edge_x_right, -1, (height >> VERT_2_POWER)*sizeof(int));

	_thred = _thred & 0xff;
	*_img_breakage_check = 1;       //��ʼ������Ϊ��������������

	//������ұ�̫�����ױ�����
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

	int cross_rows = 8; //��
	int cross_cols = 16;  //��

	//���ϱ�Ե����±�Ե��
	for (i = img_ignore_point_l >> ROW_2_POWER; i<((width - img_ignore_point_r) >> ROW_2_POWER); i++)
	{
		psrc = _src_img->image_data + row_step * i;

		//?? x=row_step * i
		int x = row_step * i;

		for (j = cross_rows; j<height * 2 / 3; j += cross_rows)     //�����������ϱ�Ե��
		{
			unsigned char value = *(psrc + j*width_step);

			if (value > _thred)  //�ȸ�croos_rows���ҵ�����ֵ>��ֵ�ĵ�
			{
				cnt = 0;
				for (k = 0; k < cross_rows + 3; k++)  //��������
				{
					value = *(psrc + ((j - cross_rows) + k)*width_step);
					if (value > _thred)  //?? x=row_step * i
					{
						if (++cnt > 3)      //����4�д�����ֵ�����������
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
				if (cnt > 3)   //�ҵ��ˣ��ж��Ƿ�Ϊ���ߵ㣬����
				{
					if (edge_x_up[i] == 0)   //���ߵ��Ϊ-2������������
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
		//�����������±�Ե��
		if (edge_x_up[i] >= 0 || edge_x_up[i] == -2)
		{
			for (j = height - 1 - cross_rows; j >= height / 3; j -= cross_rows)
			{
				unsigned char value = *(psrc + j*width_step);

				if (value > _thred) //?? x=row_step * i       //�ȸ�croos_rows���ҵ�����ֵ>��ֵ�ĵ�
				{
					cnt = 0;
					for (k = 0; k < cross_rows + 3; k++)   //��������
					{
						value = *(psrc + ((j + cross_rows) - k)*width_step);
						if (value > _thred) //?? x=row_step * i
						{
							if (++cnt > 3)      //����4�д�����ֵ�����������
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
					if (cnt > 3)     //�ҵ��ˣ��ж��Ƿ�Ϊ���ߵ㣬����
					{
						if (edge_x_down[i] == height - 1)   //���ߵ��Ϊ-2������������
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

	if (edge_up_cnt + edge_up_cnt1<20)    //�����Ե������1�붼������˵��ͼ��Դ������
	{
		if (1 == (_last_status & 0x01)) //�ϴ���ɲ��״̬
		{
			return ERR_CORNER_IMGSOURCE;                //�ж�Ϊͼ��Դ����
		}
		else
		{
			return ERR_CORNER_IMGCIS;        //�ж�ΪCIS����
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
	//�����Ե����ұ�Ե��
	for (i = 0; i < (height >> VERT_2_POWER); i++)
	{
		psrc = _src_img->image_data + width_step*(i << VERT_2_POWER);
		for (j = img_ignore_point_l + cross_cols; j < width / 3; j += cross_cols)   //�������������Ե��
		{
			int x = j;
			unsigned char value = *(psrc + j);

			if (value > _thred)    //?? x=j       //�ȸ�croos_cols���ҵ�����ֵ>��ֵ�ĵ�
			{
				cnt = 0;
				for (k = 0; k < cross_cols + 3; k++)   //��������
				{
					x = ((j - cross_cols) + k);
					value = *(psrc + (j - cross_cols) + k);
					if (value > _thred)  //?? x=(j - cross_cols) + k)
					{
						if (++cnt > 3)     //����4�д�����ֵ�����������
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
				if (cnt > 3)   //�ҵ��ˣ�����
				{
					break;
				}
			}
		}

		for (j = width - img_ignore_point_r - cross_cols; j >= 2 * width / 3; j -= cross_cols)   //�����������ұ�Ե��
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
				if (cnt > 3)  //�ҵ��ˣ�����
				{
					break;
				}
			}
		}
	}

	//���������ѭ����Ӧ��ÿ�μ��㡣  ����
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

//ȷ��CIS����������Ҫ���Ե���
int IgnoreCISCol(int *_left, int *_right, IplImage_ *_src_img)
{
	int i = 0;
	int j = 0;
	unsigned char* psrc;
	int count = 0;  //��¼���ϵ�����ֵ�����ظ���

	//ȷ��CIS��ߵ�����ֵ�����ظ������ڵ㣩
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
		*_left = 16;    //ͼ�����Ҳ����16�����ص�
	}
	else
	{
		*_left = IMAGE_IGNORE_POINT;
	}

	//ȷ��CIS�ұߵ�����ֵ�����ظ������ڵ㣩
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
		*_right = 16;    //ͼ�����Ҳ����16�����ص�
	}
	else
	{
		*_right = IMAGE_IGNORE_POINT;
	}

	return 0;
}

//�ҵ�Ǯ���ĸ��ǵ�����
//*_img_breakage_checkΪͼ�������⿪��, *_srcΪ����ͼ��, *_corner_infoΪ�ǵ���Ϣ�ṹ��, _last_statusΪͼ���ϴε�״̬
int FindCornerAndAngle(int *_img_breakage_check, unsigned char *_src, CornerCorrectInfo_ *_corner_info, int _last_status)
{
	int width = _corner_info->width;
	int height = _corner_info->height;
	int row_point_num;  //��߲�������
	int vert_point_num; //�ݱ߲�������
	int background_thred = RMB_BACKGROUND_THRED;   //������ֵ
	int confidence_thred = EDGE_CONFIDENCE_THRED;
	int ret = 0;
	int i;

	int parallel_thred;
	int switch_temp;

	IplImage_ src_img;  //ͼ������

	ReleaseImage(&src_img);
	CreateImage(&src_img, width, height, _src);    //����ͼ��
	src_img.width_step = _corner_info->width_step;

	//  gClearDirt(&src_img);   //���CIS������ɵı�������

	//���б���ͳһ�����
	ret = GetMoneyEdgePoint(_img_breakage_check, &src_img, _corner_info, background_thred, &row_point_num, &vert_point_num, _last_status);
	if (ret < 0)
		return ret;

	_corner_info->up_edge_info.type = 0;   //ȷ���ߵ�����
	_corner_info->down_edge_info.type = 1;
	_corner_info->left_edge_info.type = 2;
	_corner_info->right_edge_info.type = 3;

	//��С���˷����ֱ��
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

	if (ret < 0)  //��ϳ���������
	{
		return ERR_CORNER_EDGE_DEGREE;
	}
	int degree_thred = EDGE_DEGREE_THRED * 2;
	if (_corner_info->up_edge_info.degree >= degree_thred || _corner_info->down_edge_info.degree >= degree_thred || _corner_info->up_edge_info.degree <= -degree_thred || _corner_info->down_edge_info.degree <= -degree_thred) //��ǹ��󣬱�����
	{
		return ERR_CORNER_EDGE_DEGREE;
	}

	int refer_thred = EDGE_REFER_THRED * 2;
	//�ж������Ƿ�ƽ�У����������
	if ((_corner_info->up_edge_info.degree != 0 || _corner_info->up_edge_info.b != 0) && (_corner_info->down_edge_info.degree != 0 || _corner_info->down_edge_info.b != height - 1))
		//�ж����±�Ե�Ƿ��ڴ����������ڴ�������ж��Ƿ�ƽ��
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
		if (switch_temp< -parallel_thred || switch_temp>parallel_thred)  //�����±�������8��
		{
			if (_corner_info->up_edge_info.confidence >= confidence_thred)
			{
				if (_corner_info->down_edge_info.confidence >= confidence_thred)
				{
					//�����±����ŶȾ�������ֵ���±����Ŷȴ����ϱߣ����±�Ϊ��ȷ�ߣ������ϱ�
					if (_corner_info->down_edge_info.confidence > _corner_info->up_edge_info.confidence)
					{
						ret = FittingEdgeRefer(edge_x, edge_x_up, row_point_num, 0, &_corner_info->up_edge_info, &_corner_info->down_edge_info);    //up
					}
					else
					{
						ret = FittingEdgeRefer(edge_x, edge_x_down, row_point_num, 0, &_corner_info->down_edge_info, &_corner_info->up_edge_info);  //down
					}
				}
				else   //���ϱ����Ŷȴ�����ֵ���±����Ŷ�С����ֵ�������±�
				{
					ret = FittingEdgeRefer(edge_x, edge_x_down, row_point_num, 0, &_corner_info->down_edge_info, &_corner_info->up_edge_info);  //down
				}
			}
			//���ϱ����Ŷ�С����ֵ���±����Ŷȴ�����ֵ�������ϱ�
			else if (_corner_info->down_edge_info.confidence >= confidence_thred)
			{
				ret = FittingEdgeRefer(edge_x, edge_x_up, row_point_num, 0, &_corner_info->up_edge_info, &_corner_info->down_edge_info);    //up
			}
			else  //�����±����ŶȾ�С����ֵ������
			{
				return  ERR_CORNER_PARALLEL_DEGREE;
			}
			if (ret < 0)
			{
				return ERR_CORNER_PARALLEL_DEGREE;
			}
		}
	}

	//�ж������Ƿ�ƽ�У����������
	parallel_thred = abs(abs(_corner_info->left_edge_info.degree) + abs(_corner_info->right_edge_info.degree) - 360) / 2;
	parallel_thred = (int)(parallel_thred*0.15 + 0.5);
	if (parallel_thred < 8)
	{
		parallel_thred = 4;
	}
	switch_temp = abs(_corner_info->left_edge_info.degree) - abs(_corner_info->right_edge_info.degree);
	if (switch_temp< -parallel_thred || switch_temp>parallel_thred)   //�����ұ�������8��
	{
		if (_corner_info->left_edge_info.confidence>92 && _corner_info->right_edge_info.confidence>92)
		{
			ret = ERR_COORNER_BIG_ANGLE;
		}
		else if (_corner_info->left_edge_info.confidence >= confidence_thred)
		{
			if (_corner_info->right_edge_info.confidence >= confidence_thred)
			{
				//�����ұ����ŶȾ�������ֵ���ұ����Ŷȴ�����ߣ����ұ�Ϊ��ȷ�ߣ��������
				if (_corner_info->right_edge_info.confidence > _corner_info->left_edge_info.confidence)
				{
					ret = FittingEdgeRefer(edge_y, edge_x_left, vert_point_num, 1, &_corner_info->left_edge_info, &_corner_info->right_edge_info);  //left
				}
				else
				{
					ret = FittingEdgeRefer(edge_y, edge_x_right, vert_point_num, 1, &_corner_info->right_edge_info, &_corner_info->left_edge_info); //right
				}
			}
			else   //��������Ŷȴ�����ֵ���ұ����Ŷ�С����ֵ�������ұ�
			{
				ret = FittingEdgeRefer(edge_y, edge_x_right, vert_point_num, 1, &_corner_info->right_edge_info, &_corner_info->left_edge_info); //right
			}
		}
		//��������Ŷ�С����ֵ���ұ����Ŷȴ�����ֵ���������
		else if (_corner_info->right_edge_info.confidence >= confidence_thred)
		{
			ret = FittingEdgeRefer(edge_y, edge_x_left, vert_point_num, 1, &_corner_info->left_edge_info, &_corner_info->right_edge_info);  //left
		}
		else    //�����ұ����ŶȾ�С����ֵ������
		{
			return  ERR_CORNER_PARALLEL_DEGREE;
		}
		if (ret < 0 && ret != ERR_COORNER_BIG_ANGLE)
		{
			return ERR_CORNER_PARALLEL_DEGREE;
		}
	}

	FloatPoint_ corner[4];  //�ǵ�����
	CalcCrosspoint(&_corner_info->up_edge_info, &_corner_info->down_edge_info, &_corner_info->left_edge_info, corner, corner + 3);    //�����ĸ��ǵ�����
	CalcCrosspoint(&_corner_info->up_edge_info, &_corner_info->down_edge_info, &_corner_info->right_edge_info, corner + 1, corner + 2);

	for (i = 0; i < 4; i++)     //ͼ��Ǯ���ĸ�������
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
	printf("�۽������ %-8f, %-8f ,%-8f, %-8f \n", _corner_info->areaSize[0], _corner_info->areaSize[1], _corner_info->areaSize[2], _corner_info->areaSize[3]);
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

//����һ�������С���˷�������ֱ�ߣ��õ�ֱ�ߵ�k,b,degree,confidence��ֵ
//_thredΪ������ֵ, *_xΪ���x����, *_y_srcΪ���y����, _cntΪ����, _typeΪ�ߵ�����, *edge_line_infoΪ�߽ṹ��, *_src_imgΪ����ͼ��
int FittingEdge(int _thred, int *_x, int *_y_src, int _cnt, int _type, EdgeInfo_ *_edge_line_info, IplImage_ *_src_img)
{
	int del_point = 0;  //ɾ���ĵ���
	int i;  //ѭ������
	int num = 0;

	int deviation_max;  //���ƫ�ƾ���
	int deviation_plus_max;  //�������ƫ��
	int deviation_minus_max;
	int deviation_plus_cnt;  //����ƫ�Ƶĵ���
	int deviation_minus_cnt;
	int plus_index;  //�������ƫ�Ƶ������
	int minus_index;
	int plus_valid_num;  //����ƫ�Ƶ���Ч����
	int minus_valid_num;
	int direction;  //ƫ�Ʒ���
	int nthred = _thred + 10;

	int real_y;  //��ļ���yֵ

	long long A = 0;  //��С���˷��Ĺ�ʽ����
	long long B = 0;
	long long C = 0;
	long long D = 0;

	long long k, b;  //ֱ��k,b
	double temp;
	double degree;  //ֱ�����

	int *y = edge_data_buff;
	memcpy(y, _y_src, _cnt*sizeof(int));

	_edge_line_info->confidence = 0;

	for (i = 0; i<_cnt; i++)    //����ABCD����С���˷�
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
		temp = (double)(B * B - num * D);     //���ֱ��
		if (temp == 0)
		{
			k = 0;
			b = 0;
		}
		else
		{
			k = (long long)((A * B - num * C) / temp * 1024);		//1024�ľ���
			b = (long long)((B * C - A * D) / temp * 1024);		//1024�ľ���
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

				if (_type)   //�����ұ�
				{
					if (_x[i] >= 0 && _x[i]<_src_img->height && real_y >= 0 && real_y<_src_img->width)
					{
						fitting_point_valid[i] = *(unsigned char*)(_src_img->image_data + _x[i] * _src_img->width_step + real_y);  //��Ӧ����real_y�ɣ���
					}
					else
					{
						fitting_point_valid[i] = 0;
					}
				}
				else     //�����±�
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
				if (fitting_deviation[i]<0)   //������ֱ���·������ҷ���
				{
					deviation_minus_cnt++;   //����ƫ�Ƶĵ���+1
					if (fitting_point_valid[i] > nthred)
					{
						minus_valid_num++;   //����ƫ�Ƶ���Ч����+1
					}
					if (fitting_deviation[i] < deviation_minus_max)   //�ҵ�����ƫ�����ĵ�
					{
						deviation_minus_max = fitting_deviation[i];
						minus_index = i;
					}
				}
				else                       //������ֱ���Ϸ������󷽣�
				{
					deviation_plus_cnt++;     //����ƫ�Ƶĵ���+��
					if (fitting_point_valid[i] > nthred)
					{
						plus_valid_num++;     //����ƫ�Ƶ���Ч����+1
					}
					if (fitting_deviation[i] > deviation_plus_max)   //�ҵ�����ƫ�����ĵ�
					{
						deviation_plus_max = fitting_deviation[i];
						plus_index = i;
					}
				}
			}
		}

		deviation_max = deviation_plus_max>-deviation_minus_max ? deviation_plus_max : -deviation_minus_max;  //��¼���ƫ��

		if (deviation_max < 3072)  //���ƫ��С��3����������
		{
			break;
		}

		if (-deviation_minus_max<3072)	//���������ƫ��С��3���������ƫ�ƴ���3����ȥ������ƫ�����ĵ㣬direction = 1
		{
			direction = 1;
		}
		else if (deviation_plus_max<3072)	//���������ƫ��С��3���������ƫ�ƴ���3����ȥ������ƫ�����ĵ㣬direction = 0
		{
			direction = 0;
		}
		//�������ƫ�����������ƫ�ƶ�����3��������Ƚ���Ч����
		else if (minus_valid_num * 100 / deviation_minus_cnt > plus_valid_num * 100 / deviation_plus_cnt)
		{
			direction = 1;
		}
		else
		{
			direction = 0;
		}

		if (direction)   //direction = 1,ȥ������ƫ�����ĵ�
		{
			del_point++;
			A -= y[plus_index];	//data_y.sum ();
			B -= _x[plus_index];//data_x.sum ();
			C -= _x[plus_index] * y[plus_index];//(data_x * data_y).sum();
			D -= _x[plus_index] * _x[plus_index];//(data_x * data_X).sum();
			y[plus_index] = -1;
			num--;
		}
		else      //direction = 0,ȥ������ƫ�����ĵ�
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

	_edge_line_info->confidence = (_edge_line_info->confidence - del_point) * 100 / _edge_line_info->confidence;  //����ߵ����Ŷ�

	if (_type)  //����ߵ����
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

//�������ŶȽϸߵıߣ�������һ����
//*_xΪ���x����, *_yΪ���y����, _cntΪ����, _typeΪ�ߵ�����, *EdgeLineInfoΪ��������, *ReferEdgeLineInfoΪ��ȷ��
int FittingEdgeRefer(int *_x, int *_y_src, int _cnt, int _type, EdgeInfo_ *_edge_line_info, EdgeInfo_ *_refer_edge_line_info)
{
	int i;  //ѭ������
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
	long long k = _refer_edge_line_info->k * 1024;   //�ο��ߵ�k,b
	long long b = _refer_edge_line_info->b * 1024;

	int *y = edge_data_buff;
	memcpy(y, _y_src, _cnt*sizeof(int));

	_edge_line_info->confidence = 0;
	for (i = 0; i<_cnt; i++)   //������Ч�������¼��������ߵ����Ŷ�
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
			fitting_deviation[i] = k*_x[i] + b - y[i] * 1024;   //�������϶�Ӧ��ľ���
			if (i>0 && fitting_deviation[i - 1] != -9999999)
			{
				fitting_difference[i] = fitting_deviation[i - 1] - fitting_deviation[i];  //�������϶�Ӧ��ľ�����ǰһ������Ĳ�ֵ
				if (abs(fitting_difference[i]) < 3072)  //����ǰһ������С
				{
					if (state == 0)  //��һ����ֵС�ڵĵ�
					{
						state = 1;
						fitting_targe_point[targe_cnt] = 2;
						fitting_targe_diff_mean[targe_cnt] = abs(fitting_difference[i]);  //�õ�߾���ǰһ����߾�ľ��Բ�ֵ
						fitting_targe_num[targe_cnt] = fitting_deviation[i - 1] + fitting_deviation[i]; //�õ�߾���ǰһ����߾����
						standard = (fitting_deviation[i - 1] + fitting_deviation[i]) / 2;  //�õ�߾���ǰһ����߾�ľ�ֵ
					}
					else
					{
						if (abs(standard - fitting_deviation[i]) <3072)  //����֮ǰ��ƽ���������ϴ�
						{

							fitting_targe_diff_mean[targe_cnt] += abs(fitting_difference[i]);
							fitting_targe_num[targe_cnt] += fitting_deviation[i];
							standard = (standard*fitting_targe_point[targe_cnt] + fitting_deviation[i]) / (fitting_targe_point[targe_cnt] + 1); //��ֵ
							fitting_targe_point[targe_cnt]++;//����
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
		return -1;	//û���ҵ�ƽ�е���
	}

	targe_result = 0;
	for (i = 0; i<targe_cnt; i++)
	{
		if (targe_result != i)	//�ȽϿ��ų̶�
		{
			if (fitting_targe_point[targe_result] > fitting_targe_point[i])	//�������Ŷ����
			{
				if (fitting_targe_point[targe_result] < 1.5*fitting_targe_point[i])	//������಻��
				{
					//�ٿ���ֵ
					if (fitting_targe_diff_mean[targe_result] > fitting_targe_diff_mean[i])
					{
						targe_result = i;
					}
				}
			}
			else
			{
				if (1.5*fitting_targe_point[targe_result] > fitting_targe_point[i])	//������಻��
				{
					//�ٿ���ֵ
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
		return -1;	//û���ҵ�
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

//�õ������ߵ����Ŷ�
//�õ������ߵ����Ŷ�

int bais[128] = { 0 }; //�ֲ���������
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
		//k = (double)(_y1 - _y2) / (double)(_x1 - _x2) * 1024; //�����е�����
		//b = (_y1 - (k >> 10)*_x1) * 1024;

		for (int i = 0; i < _cnt; i++)
		{
			if (_edge_x[i] >= _x1&&_edge_x[i] <= _x2)  //�ڽǵ�֮����
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
	else if (_x1 == _x2)  //ˮƽ
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
	else       //��ֱ
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

	_corner_info->edge_feature[_edge_type].confidence = (10 - mean<0) ? 0 : (10 - mean);           // �þ�ֵ�������ƫ�Ƶ�������ʾ
	_corner_info->edge_feature[_edge_type].deviation = deviation_point;
	_corner_info->edge_feature[_edge_type].mean = mean;
	_corner_info->edge_feature[_edge_type].var = var;

	return ret;
}

//�õ��ߵļн�
int GetAngle(EdgeInfo_ *_edge_a, EdgeInfo_ *_edge_b, int _point)   //����ֱ������degree��Ϣ
{
	int ret = 0;
	double degree_a = atan(_edge_a->k) / 3.14 * 180;   //ˮƽ��
	degree_a = abs(degree_a);

	double degree_b = 0.0;
	if (_edge_b->k != 0)
	{
		degree_b = atan(1 / _edge_b->k) / 3.14 * 180;   //��ֱ��
	}
	else
	{
		degree_b = 90;
	}
	degree_b = abs(degree_b);
	if (_edge_a->k>0)        //ˮƽ��б�ʴ���0
	{
		if (_point == 0 || _point == 2)
		{
			ret = ((180 - degree_b - degree_a) - (int)(180 - degree_b - degree_a)) >= 0.5 ? (int)(180 - degree_b - degree_a) + 1 : (int)(180 - degree_b - degree_a);   //��������
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

//�����۽ǵ����

int GetAreaOfBreak(CornerCorrectInfo_ *_corner_info,int _row_cnt,int _ver_cnt,int *_edge_x,int *_edge_x_point,int *_edge_y,int *_edge_y_point,int _point_type)
{
	int x_start = 0, y_start = 0;  //����С
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

		k1 = _corner_info->up_edge_info.k;  //double���ͣ�Ϊ0��
		b1 = _corner_info->up_edge_info.b;

		k2 = _corner_info->left_edge_info.k;
		b2 = _corner_info->left_edge_info.b;

		for (int i = 0; i < _row_cnt ; i++)   //ˮƽ�ϱ�
		{
			if (_edge_x[i] >= x_start&&_edge_x[i] <= x_end)
			{
				if (_edge_x_point[i]>0)
				{
					y = k1*_edge_x[i] + b1;
					deviation = abs(y - _edge_x_point[i]);
					if (deviation <= 3)                      //��������������
					{
						point1.x = _edge_x[i];
						//point1.y = _edge_x_point[i];
						point1.y = y;
						break;
					}
				}
			}
		}

		for (int i = 0; i < _ver_cnt ; i++)               //��ֱ���
		{
			if (_edge_y[i] >= y_start&&_edge_y[i] <= y_end)
			{
				if (_edge_y_point[i]>0)
				{
					y = k2*_edge_y[i] + b2;
					deviation =abs(y - _edge_y_point[i]);
					if (deviation <= 3)
					{
						point2.y = _edge_y[i];             //����
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

		for (int i = _row_cnt - 1; i >= 0 ; i--)       // ˮƽ�ϱ�
		{
			if (_edge_x[i] >= x_start&&_edge_x[i] <= x_end)   
			{
				if (_edge_x_point[i]>0)
				{
					y = k1*_edge_x[i] + b1;
					deviation = abs(y - _edge_x_point[i]);
					if (deviation <= 3)                      //��������������
					{
						point1.x = _edge_x[i];
						point1.y = y;
						break;
					}
				}
			}
		}

		for (int i = 0; i < _ver_cnt ; i++)                //��ֱ�ұ�
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

		for (int i = _row_cnt - 1; i >= 0 ; i--)   //ˮƽ�±�
		{
			if (_edge_x[i] >= x_start&&_edge_x[i] <= x_end)
			{
				if (_edge_x_point[i] > 0)
				{
					y = k1*_edge_x[i] + b1;
					deviation = abs(y - _edge_x_point[i]);
					if (deviation <= 3)                      //��������������
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
		for (int i = 0; i < _row_cnt ; i++)    //ˮƽ�±�
		{
			if (_edge_x[i] >= x_start&&_edge_x[i] <= x_end)
			{
				if (_edge_x_point[i]>0)
				{
					y = k1*_edge_x[i] + b1;
					deviation = abs(y - _edge_x_point[i]);
					if (deviation <= 3)                      //��������������
					{
						point1.x = _edge_x[i];
						point1.y = y;
						break;
					}
				}
			}
		}
		 
		for (int i = _ver_cnt-1; i > 0; i--)     //��ֱ���
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
	
	CalculateArea(_corner_info,point1,point2,point3,_point_type);  //_point1Ϊˮƽ�ߵ㣬_point2Ϊ��ֱ�ߵ㣬_point3Ϊ�ǵ�
	//DrawEdgeCorner();

	return 0;
}

//��������ͽǶȼ������
int CalculateArea(CornerCorrectInfo_ *_corner_info, IntPoint_ _point1, IntPoint_ _point2, IntPoint_ _point3,int _point_type)  //_point1Ϊˮƽ�ߵ㣻 _point2Ϊ��ֱ�ߵ�
{
	if (_point1.x==-1||_point2.y==-1)
	{
		_corner_info->areaSize[_point_type] = -1.0;
		return -1;
	}
	float angel_ = 0;
	float length1 = sqrt(1.0*(_point3.x - _point1.x)*(_point3.x - _point1.x) + (_point3.y - _point1.y)*(_point3.y - _point1.y));
	float length2 = sqrt(1.0*(_point3.x - _point2.x)*(_point3.x - _point2.x) + (_point3.y - _point2.y)*(_point3.y - _point2.y));

	if (abs(_point1.x-_point3.x)<16+8) //�ж������ĵ�
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

//ɾ�������±��ϵ�α���ұ�Ե��
//*_xΪ���x����, *_yΪ���y����, _cntΪ����, *_edge_line_infoΪ����Ϣ�ṹ��
void DelErrPoint(int *_x, int *_y, int _cnt, EdgeInfo_ *_edge_line_info)
{
	int i;
	int num = _cnt / 2;
	double deviation;

	if (_edge_line_info->type == 0)   //���ձ�Ϊ�ϱ�
	{
		for (i = 0; i < num; i++)
		{
			if (_y[i] >= 0)
			{
				deviation = abs((_edge_line_info->k*_y[i] + _edge_line_info->b) - _x[i]);
				if (deviation <= 3)     //ȥ�����ұߵĵ���ʵ�������ϱߵĵ�
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
	else        //���ձ�Ϊ�±�
	{
		for (i = _cnt - 1; i >= num; i--)
		{
			if (_y[i] >= 0)
			{
				deviation = abs((_edge_line_info->k*_y[i] + _edge_line_info->b) - _x[i]);
				if (deviation <= 3)      //ȥ�����ұߵĵ���ʵ�������±ߵĵ�
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

//��˹��Ԫ�������Է�������÷���任����
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

//����Ǯ��ʵ�ʳ���
//*_srcΪ����ͼ��, *_corner_infoΪ�ǵ���Ϣ�ṹ��, *_coordinatesΪͼ����ֽ�ҵĽǵ�����
//*_srcΪ����ͼ��, *_corner_infoΪ�ǵ���Ϣ�ṹ��
int CalcRealLengthOfMoney(unsigned char *_src, CornerCorrectInfo_ *_corner_info)
{
	int i;
	int width = _corner_info->width;
	int height = _corner_info->height;
	int width_step = _corner_info->width_step;

	unsigned char *code_value = data_buffer;//(int*)malloc(height*sizeof(int));

	if (_corner_info->width < 725)   //ͼ����С��725���޷���ȡ����ֵ������0
	{
		return 0;
	}
	for (i = 0; i < height; i++)   //��ȡ����ֵ
	{
		code_value[i] = _src[i*width_step + 724];   //ͼ���724��Ϊ����ֵ
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
*����Ǯ�Ľǵ���
*----------------------------------------------------*/
//�ҵ�Ǯ���ĸ��ǵ�����
//*_srcΪ����ͼ��, *_corner_infoΪ�ǵ���Ϣ�ṹ��
//�ҵ�Ǯ���ĸ��ǵ�����
//*_srcΪ����ͼ��, *_corner_infoΪ�ǵ���Ϣ�ṹ��, _last_statusΪͼ���ϴε�״̬
int FindHalfMoneyCorner(unsigned char *_src, CornerCorrectInfo_ *_corner_info, int _last_status)
{
	int width = _corner_info->width;
	int height = _corner_info->height;
	int row_point_num;  //��߲�������
	int vert_point_num; //�ݱ߲�������
	int background_thred = RMB_BACKGROUND_THRED;   //������ֵ
	int confidence_thred = EDGE_CONFIDENCE_THRED;
	int ret;
	int i;

	int parallel_thred;
	int switch_temp;

	IplImage_ src_img;  //ͼ������

	ReleaseImage(&src_img);
	CreateImage(&src_img, width, height, _src);    //����ͼ��
	src_img.width_step = _corner_info->width_step;

	//gClearDirt(&src_img);   //���CIS������ɵı�������

	//���б���ͳһ�����
	ret = GetHalfMoneyEdgePoint(&src_img, _corner_info, background_thred, &row_point_num, &vert_point_num, _last_status);
	if (ret < 0)
		return ret;

	_corner_info->up_edge_info.type = 0;   //ȷ���ߵ�����
	_corner_info->down_edge_info.type = 1;
	_corner_info->left_edge_info.type = 2;
	_corner_info->right_edge_info.type = 3;

	//��С���˷����ֱ��
	//�±�Ե����ȥ��ֻ����ϱ�
	ret = FittingEdge(background_thred, edge_x, edge_x_up, row_point_num, 0, &_corner_info->up_edge_info, &src_img);	//up
	DelErrPoint(edge_y, edge_x_left, vert_point_num, &_corner_info->up_edge_info);
	DelErrPoint(edge_y, edge_x_right, vert_point_num, &_corner_info->up_edge_info);
	_corner_info->down_edge_info.b = height - 1;
	
	ret |= FittingEdge(background_thred, edge_y, edge_x_left, vert_point_num, 1, &_corner_info->left_edge_info, &src_img);	//left
	ret |= FittingEdge(background_thred, edge_y, edge_x_right, vert_point_num, 1, &_corner_info->right_edge_info, &src_img);	//right

	if (ret < 0)  //��ϳ���������
	{
		return ERR_CORNER_EDGE_DEGREE;
	}
	if (_corner_info->up_edge_info.degree >= 50 || _corner_info->down_edge_info.degree >= 50 || _corner_info->up_edge_info.degree <= -50 || _corner_info->down_edge_info.degree <= -50) //��ǹ��󣬱�����
	{
		return ERR_CORNER_EDGE_DEGREE;
	}

	//�ж������Ƿ�ƽ�У����������
	parallel_thred = abs(_corner_info->left_edge_info.degree + _corner_info->right_edge_info.degree) / 2;
	parallel_thred = (int)(parallel_thred*0.15 + 0.5);
	if (parallel_thred < 8)
	{
		parallel_thred = 8;
	}
	switch_temp = abs(_corner_info->left_edge_info.degree) - abs(_corner_info->right_edge_info.degree);
	if (switch_temp< -parallel_thred || switch_temp>parallel_thred)   //�����ұ�������8��
	{
		if (_corner_info->left_edge_info.confidence >= confidence_thred)
		{
			if (_corner_info->right_edge_info.confidence >= confidence_thred)
			{
				//�����ұ����ŶȾ�������ֵ���ұ����Ŷȴ�����ߣ����ұ�Ϊ��ȷ�ߣ��������
				if (_corner_info->right_edge_info.confidence > _corner_info->left_edge_info.confidence)
				{
					ret = FittingEdgeRefer(edge_y, edge_x_left, vert_point_num, 1, &_corner_info->left_edge_info, &_corner_info->right_edge_info);	//left
				}
				else
				{
					ret = FittingEdgeRefer(edge_y, edge_x_right, vert_point_num, 1, &_corner_info->right_edge_info, &_corner_info->left_edge_info);	//right
				}
			}
			else   //��������Ŷȴ�����ֵ���ұ����Ŷ�С����ֵ�������ұ�
			{
				ret = FittingEdgeRefer(edge_y, edge_x_right, vert_point_num, 1, &_corner_info->right_edge_info, &_corner_info->left_edge_info);	//right
			}
		}
		//��������Ŷ�С����ֵ���ұ����Ŷȴ�����ֵ���������
		else if (_corner_info->right_edge_info.confidence >= confidence_thred)
		{
			ret = FittingEdgeRefer(edge_y, edge_x_left, vert_point_num, 1, &_corner_info->left_edge_info, &_corner_info->right_edge_info);	//left
		}
		else    //�����ұ����ŶȾ�С����ֵ������
		{
			return  ERR_CORNER_PARALLEL_DEGREE;
		}
		if (ret < 0)
		{
			return ERR_CORNER_PARALLEL_DEGREE;
		}
	}

	FloatPoint_ corner[4];  //�ǵ�����
	CalcCrosspoint(&_corner_info->up_edge_info, &_corner_info->down_edge_info, &_corner_info->left_edge_info, corner, corner + 3);    //�����ĸ��ǵ�����
	CalcCrosspoint(&_corner_info->up_edge_info, &_corner_info->down_edge_info, &_corner_info->right_edge_info, corner + 1, corner + 2);

	for (i = 0; i < 4; i++)     //ͼ��Ǯ���ĸ�������
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

//�ҵ�����Ǯ���������ϵĵ�
//*_src_imgΪ����ͼ��, *__corner_infoΪ�ǵ���Ϣ�ṹ��, _thredΪ������ֵ, *_row_point_numΪ��ߵ���, *_vert_point_numΪ�ݱߵ���, _last_statusΪͼ���ϴε�״̬
int GetHalfMoneyEdgePoint(IplImage_ *_src_img, CornerCorrectInfo_ *_corner_info, int _thred, int *_row_point_num, int *_vert_point_num, int _last_status)
{
	int i;  //ѭ������
	int j;
	int k;
	int cnt;
	int width = _src_img->width;
	int height = _src_img->height;
	int width_step = _src_img->width_step;  //Դͼ����
	int edge_up_cnt = 0;  //�ҵ����ϱ�Ե����
	int edge_up_cnt1 = 0;   //���ߵ��ϱ�Ե����
	int img_ignore_point_l;   //���Ե��������
	int img_ignore_point_r;
	int row_step = 2 << (ROW_2_POWER - 1);   //����ҵ�Ĳ���
	int vert_step = 2 << (VERT_HALF_2_POWER - 1);

	unsigned char *psrc;

	memset(edge_x_up, -1, (width >> ROW_2_POWER)*sizeof(int));
	memset(edge_x_down, -1, (width >> ROW_2_POWER)*sizeof(int));
	memset(edge_x_left, -1, (height >> VERT_HALF_2_POWER)*sizeof(int));
	memset(edge_x_right, -1, (height >> VERT_HALF_2_POWER)*sizeof(int));

	_thred = _thred & 0xff;

	//������ұ�̫�����ױ�����
	if (0 != IgnoreCISCol(&img_ignore_point_l, &img_ignore_point_r, _src_img))
		return -1;	//??

	int cross_rows = 16;
	int cross_cols = 8;

	//���ϱ�Ե��
	for (i = img_ignore_point_l >> ROW_2_POWER; i<((width - img_ignore_point_r) >> ROW_2_POWER); i++)
	{
		psrc = _src_img->image_data + row_step * i;
		for (j = cross_rows; j<height * 2 / 3; j += cross_rows)     //�����������ϱ�Ե��
		{
			if (*(psrc + j*width_step) > _thred)  //�ȸ�croos_rows���ҵ�����ֵ>��ֵ�ĵ�
			{
				cnt = 0;
				for (k = 0; k < cross_rows + 3; k++)  //��������
				{
					if (*(psrc + ((j - cross_rows) + k)*width_step) > _thred)
					{
						if (++cnt > 3)      //����4�д�����ֵ�����������
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
				if (cnt > 3)   //�ҵ��ˣ��ж��Ƿ�Ϊ���ߵ㣬����
				{
					if (edge_x_up[i] == 0)   //���ߵ��Ϊ-2
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

	if (edge_up_cnt + edge_up_cnt1<20)	//�����Ե������1�붼������˵��ͼ��Դ������
	{
		if (1 == (_last_status & 0x01))	//�ϴ���ɲ��״̬
		{
			return ERR_CORNER_IMGSOURCE;				//�ж�Ϊͼ��Դ����
		}
		else
		{
			return ERR_CORNER_IMGCIS;        //�ж�ΪCIS����
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
	//�����Ե����ұ�Ե��
	for (i = 0; i < (height >> VERT_HALF_2_POWER); i++)
	{
		psrc = _src_img->image_data + width_step*(i << VERT_HALF_2_POWER);
		for (j = img_ignore_point_l + cross_cols; j < width / 3; j += cross_cols)   //�������������Ե��
		{
			if (*(psrc + j) > _thred)         //�ȸ�croos_cols���ҵ�����ֵ>��ֵ�ĵ�
			{
				cnt = 0;
				for (k = 0; k < cross_cols + 3; k++)   //��������
				{
					if (*(psrc + (j - cross_cols) + k) > _thred)
					{
						if (++cnt > 3)     //����4�д�����ֵ�����������
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
				if (cnt > 3)   //�ҵ��ˣ�����
				{
					break;
				}
			}
		}

		for (j = width - img_ignore_point_r - cross_cols; j >= 2 * width / 3; j -= cross_cols)   //�����������ұ�Ե��
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
				if (cnt > 3)  //�ҵ��ˣ�����
				{
					break;
				}
			}
		}
	}

	//���������ѭ����Ӧ��ÿ�μ��㡣  ����
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

//�������Ǯ��ʵ�ʳ���
//*_srcΪ����ͼ��, *_corner_infoΪ�ǵ���Ϣ�ṹ��, *_coordinatesΪͼ����ֽ�ҵĽǵ�����
int CalcHalfRealLengthOfMoney(unsigned char *_src, CornerCorrectInfo_ *_corner_info, CoordinatesInfo_ *_coordinates)
{
	int i;
	int width = _corner_info->width;
	int height = _corner_info->height;
	int width_step = _corner_info->width_step;
	int code_col = 724;

	unsigned char *code_value = data_buffer;//(int*)malloc(height*sizeof(int));

	if (_corner_info->width < 725)   //ͼ����С��725���޷���ȡ����ֵ������0
	{
		return 0;
	}
	for (i = 0; i < height; i++)   //��ȡ����ֵ
	{
		code_value[i] = _src[i*width_step + code_col];   //ͼ���724��Ϊ����ֵ
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

//����Ǯ��һ���ߵ�ʵ�ʳ��Ȳ�����
//_point0, _point1Ϊ����������, _heightΪͼ��߶�, *_code_dataΪ����ֵ����
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
	if (up < 0 || down>_height-1)  //���y���곬����Χ���޷���ȡ����ֵ���޷�����ʵ�ʳ��ȣ�����-999
	{
		return -999;
	}

	float code_len = GetCodeLength(up, down, _code_data);  //����������������̳���


	vert_len = code_len*CODE_LENGTH;
	real_length = sqrt(hori_len*hori_len + vert_len*vert_len);

	return real_length;
}

//��ȡ������������̳���
//_upΪ��С��y����, _downΪ�ϴ��y����, *_code_dataΪ����ֵ����
float GetCodeLength(int _up, int _down, unsigned char *_code_data)
{
	int i;
	
	float code_len;
	float first_code_len;   //��һ������ֵ�ĳ���
	float last_code_len;    //���һ������ֵ�ĳ���
	int code_val=-1;
	int cnt=-1;
	int num = _down - _up + 1;     //����
	int *vert_point = (int *)malloc((_down - _up + 1)*sizeof(int));    //��Ų�ͬ����ֵ�ĸ���
		
	for (i = 0; i <= _down-_up; i++)
	{
		vert_point[i] = 0;
	}

	for (i = _up; i <= _down; i++)
	{
		if (_code_data[i] != code_val)   //����ǰ����ֵ��������һ������ֵ��
		{
			code_val = _code_data[i];
			cnt++;
		}
		vert_point[cnt]++;
	}
	cnt++;

	if (cnt > 1)       //���г���1������ֵ
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
	//else if (cnt == 2)   //��ֻ��2������ֵ
	//{
	//	first_code_len = (float)vert_point[0];
	//}
	else if (cnt == 1)   //��ֻ��1������ֵ
	{
		code_len = 1;
	}

	free(vert_point);
	return code_len;

}

//�õ�����ͼ�ķ���任�õ�����
//_dst_tri[]ΪУ��������,*_corner_infoΪ�ǵ���Ϣ�ṹ��, _affine_widthΪ�������, _affine_heightΪ�����߶�, _src_tri[]ΪǮ������,
void CalcHalfAffineCoordinate(IntPoint_ _dst_tri[], CornerCorrectInfo_ *_corner_info, int _affine_width, int _affine_height, IntPoint_ _src_tri[])
{
	_dst_tri[0].y = (_corner_info->height - _affine_height) / 2;   //ȷ��У�����������
	_dst_tri[0].x = (_corner_info->width - _affine_width) / 2;

	//ȷ�����������Ӧ��λ��
	if (_corner_info->edge_real_len[2] > 0)  //��down�����Ϊ׼
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
	else     //��down���ұ�Ϊ׼
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

//�õ�����Ǯ��һ��ķ�������
//_other_tri[]Ϊ��һ�������, *_corner_infoΪ�ǵ���Ϣ�ṹ��, *_coordinates1Ϊ��һ��ͼ����ֽ�ҽǵ�����
int GetHalfOtherCoordinate(IntPoint_ _other_tri[], CornerCorrectInfo_ *_corner_info, CoordinatesInfo_ *_coordinates1)
{
	//ȷ�����������Ӧ��λ��
	if (_corner_info->edge_real_len[2] > 0)    //�����Ϊ׼
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
	 
	if (abs(edge->b)>=1e-15||abs(edge->k)>=1e-15)  //��0
	{
		ret=abs(edge->k*point->x - point->y + edge->b) / sqrt(1 + edge->k*edge->k);
	}
	if (abs(edge->k)<1e-15)
	{
		ret = abs(edge->b - point->y);
	}
	return ret;
}