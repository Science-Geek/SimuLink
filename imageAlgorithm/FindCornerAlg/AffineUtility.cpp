/***************************************************************
* Copyright (C) 2011, Wuhan University
* All rights reserved.
*
* �ļ�����	Recognition.c
* ����������
*
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�
*
***************************************************************/
//#include <stdafx.h>

#include "stdio.h"
#include "Recognition.h"
#include "CurrencyType.h"
#include "FindCorner.h"
#include "windows.h"
#include "wingdi.h"


#define LENGTH100   156
#define LENGTH50    150
#define LENGTH20    145
#define LENGTH10    140
#define LENGTH5     135
#define LENGTH1     130

#define CROP_IMG_SIZE			3276800   //ͼ�񻺴��С��VC���ԣ�
unsigned char CropImg[CROP_IMG_SIZE];   //ͼ�񻺴棨�ڲ����ԣ�

#define SAVE_IMAGE     1   //����ͼ��VC���ԣ�

unsigned char affine_data_up[144000];     //DCT������Ǯ
unsigned char affine_data_down[144000];   //DCT������Ǯ

int ImgDataInit(unsigned char* _img_up, unsigned char* _img_bottom, PicDataInfo_ *pic_data_info, int _cash_type, ImgSizeInfo* _img_size)
{
	int width = _img_size->width;   
	int height = _img_size->height;
	//int width = GetParaImgWidth();
	//int height = GetParaImgHeight();

	pic_data_info->cash_type = _cash_type;
	pic_data_info->up_data = _img_up;
	pic_data_info->bottom_data = _img_bottom;

	if (pic_data_info->cash_type == CURRENCY_TYPE_RMB
		|| pic_data_info->cash_type == CURRENCY_TYPE_JPY
		//      || pic_data_info->cash_type == CURRENCY_TYPE_EUR2)
		|| pic_data_info->cash_type == CURRENCY_TYPE_BUK)
	{
		pic_data_info->height = height;
		pic_data_info->width = width;
		pic_data_info->width_step = _img_size->width;
		pic_data_info->R_img_offset = 0 * _img_size->channel_offset;
		pic_data_info->G_img_offset = 1 * _img_size->channel_offset;
		pic_data_info->B_img_offset = 2 * _img_size->channel_offset;
		pic_data_info->IR_img_offset[CIS_B] = 4 * _img_size->channel_offset;
		pic_data_info->IR_img_offset[CIS_T] = 3 * _img_size->channel_offset;
		pic_data_info->TR_img_offset[CIS_B] = 3 * _img_size->channel_offset;
		pic_data_info->TR_img_offset[CIS_T] = 4 * _img_size->channel_offset;
		pic_data_info->UV_img_offset = 5 * _img_size->channel_offset;
		pic_data_info->channle_num = _img_size->chnl_num;
	}
	else if (pic_data_info->cash_type == CURRENCY_TYPE_HKD ||
		pic_data_info->cash_type == CURRENCY_TYPE_GBP ||
		pic_data_info->cash_type == CURRENCY_TYPE_EUR ||
		pic_data_info->cash_type == CURRENCY_TYPE_AUD ||
		pic_data_info->cash_type == CURRENCY_TYPE_CAD ||
		pic_data_info->cash_type == CURRENCY_TYPE_SGD ||
		pic_data_info->cash_type == CURRENCY_TYPE_FRF ||
		pic_data_info->cash_type == CURRENCY_TYPE_USD ||
		pic_data_info->cash_type == CURRENCY_TYPE_RUB ||
		pic_data_info->cash_type == CURRENCY_TYPE_BUK)
	{
		pic_data_info->height = height;
		pic_data_info->width = width;
		if (pic_data_info->width == 736)
		{
			pic_data_info->width = 720;
		}
		pic_data_info->width_step = width * 3;
		pic_data_info->R_img_offset = 0;
		pic_data_info->G_img_offset = pic_data_info->width_step / 3;
		pic_data_info->IR_img_offset[CIS_B] = 2 * (pic_data_info->width_step / 3);
		pic_data_info->IR_img_offset[CIS_T] = 2 * (pic_data_info->width_step / 3);
		pic_data_info->B_img_offset = pic_data_info->width_step / 3;
		pic_data_info->channle_num = 3;
	}
	else
	{
		pic_data_info->height = height;
		pic_data_info->width = width;
		if (pic_data_info->width == 736)
		{
			pic_data_info->width = 720;
		}
		pic_data_info->width_step = width * 3;
		pic_data_info->R_img_offset = 0;
		pic_data_info->G_img_offset = pic_data_info->width_step / 3;
		pic_data_info->IR_img_offset[CIS_B] = 2 * (pic_data_info->width_step / 3);
		pic_data_info->IR_img_offset[CIS_T] = 2 * (pic_data_info->width_step / 3);
		pic_data_info->B_img_offset = pic_data_info->width_step / 3;
		pic_data_info->channle_num = 3;
	}

	//CIS上面和下面的相对偏移
	if (pic_data_info->width == IMAGE_WIDTH_NEW)
	{
		pic_data_info->mapping_offset = 0;
	}
	else if (pic_data_info->width == IMAGE_WIDTH_OLD)
	{
		pic_data_info->mapping_offset = 32;
	}
	else if (pic_data_info->width == 736)
	{
		pic_data_info->mapping_offset = -16;
	}
	else if (pic_data_info->width == 768)
	{
		pic_data_info->mapping_offset = -48;
	}
	else if (pic_data_info->width == 1024)
	{
		pic_data_info->mapping_offset = -304;
	}

	if (pic_data_info->value == 0)
	{
		pic_data_info->affine_width = 192 + 2;
		pic_data_info->affine_height = 48 + 2;
		return 0;
	}

	if (pic_data_info->cash_type == CURRENCY_TYPE_RMB)
	{
		if (pic_data_info->value == 100 || pic_data_info->value == 50)
		{
			pic_data_info->affine_width = 600;
			pic_data_info->affine_height = 240;
		}
		else if (pic_data_info->value == 20 || pic_data_info->value == 10)
		{
			pic_data_info->affine_width = 600;
			pic_data_info->affine_height = 240;
		}
		else if (pic_data_info->value == 5)
		{
			pic_data_info->affine_width = 560;
			pic_data_info->affine_height = 180;
		}
		else if (pic_data_info->value == 1)
		{
			pic_data_info->affine_width = 510;
			pic_data_info->affine_height = 170;
		}
	}
	else if (pic_data_info->cash_type == CURRENCY_TYPE_JPY)
	{
		pic_data_info->affine_width = 600;
		pic_data_info->affine_height = 200;
	}
	else if (pic_data_info->cash_type == CURRENCY_TYPE_USD)
	{
		pic_data_info->affine_width = 620;
		pic_data_info->affine_height = 180;
	}
	else if (pic_data_info->cash_type == CURRENCY_TYPE_HKD)     
	{
		pic_data_info->affine_width = 600;
		pic_data_info->affine_height = 240;
	}
	else if (pic_data_info->cash_type == CURRENCY_TYPE_GBP)    
	{
		pic_data_info->affine_width = 600;
		pic_data_info->affine_height = 240;
	}
	else if (pic_data_info->cash_type == CURRENCY_TYPE_BUK)
	{
		pic_data_info->affine_height = 180;
		pic_data_info->affine_width = 620;
	}
	else if (pic_data_info->cash_type == CURRENCY_TYPE_EUR)
	{
		if (pic_data_info->value == 500)
		{
			pic_data_info->affine_width = 630;
			pic_data_info->affine_height = 170;
		}
		else if (pic_data_info->value == 200)
		{
			pic_data_info->affine_width = 600;
			pic_data_info->affine_height = 170;
		}
		else if (pic_data_info->value == 100)
		{
			pic_data_info->affine_width = 580;
			pic_data_info->affine_height = 170;
		}
		else if (pic_data_info->value == 50)
		{
			pic_data_info->affine_width = 550;
			pic_data_info->affine_height = 160;
		}
		else if (pic_data_info->value == 20)
		{
			pic_data_info->affine_width = 530;
			pic_data_info->affine_height = 160;
		}
		else if (pic_data_info->value == 10)
		{
			pic_data_info->affine_width = 500;
			pic_data_info->affine_height = 140;
		}
		else if (pic_data_info->value == 5)
		{
			pic_data_info->affine_width = 480;
			pic_data_info->affine_height = 120;
		}
	}
	//  else if (pic_data_info->cash_type == CURRENCY_TYPE_EUR2)
	//  {
	//      pic_data_info->affine_height = 150;
	//      pic_data_info->affine_width = 600;
	//      pic_data_info->cash_type = CURRENCY_TYPE_EUR;
	//  }
	else
	{
		return -1;
	}

	return 0;
}


int GetAffineCorner(PicDataInfo_ *_pic_data_info, struct CornerCorrectInfo *_corner_info, IR_PicInfo_*Up_IR_info, IR_PicInfo_*Down_IR_info, int _tri_type[])
{
	U8* p_up_ir = _pic_data_info->up_data + _pic_data_info->IR_img_offset[CIS_T];
	U8* p_bottom_ir = _pic_data_info->bottom_data + _pic_data_info->IR_img_offset[CIS_B];

	int i;
	int corner_ret = 0;
	int last_status = _pic_data_info->last_error;
	int img_breakage_check = 0;

	_corner_info->width = _pic_data_info->width;    //ͼ��Ŀ��ߣ�������ɲ��״̬��Ϣ��ֵ��corner_info�ṹ��
	_corner_info->height = _pic_data_info->height;
	_corner_info->width_step = _pic_data_info->width_step;
	_corner_info->missing_info = -1;
	corner_ret = FindCornerAndAngle(&img_breakage_check, p_bottom_ir, _corner_info, last_status);  //����ͼ�ҽǵ�
	
	if (corner_ret < 0 && corner_ret!=ERR_COORNER_BIG_ANGLE)
	{
		return corner_ret;
	}

	GetTriType(_corner_info, _tri_type + 1);   //�õ�����ͼ�ķ�����������

	for (int i = 0; i < 4; i++)
	{
		_pic_data_info->coordinates_bottom.x[i] = _corner_info->coordinates.x[i];
		_pic_data_info->coordinates_bottom.y[i] = _corner_info->coordinates.y[i];
	}
	_pic_data_info->missing_info[1] = _corner_info->missing_info;

	Down_IR_info->width = _pic_data_info->width;
	Down_IR_info->width_step = _pic_data_info->width_step;
	Down_IR_info->height = _pic_data_info->height;
	Down_IR_info->image_data = _pic_data_info->bottom_data + _pic_data_info->IR_img_offset[CIS_B];
	Down_IR_info->coordinates = _corner_info->coordinates;
	Down_IR_info->UpEdgeInfo = _corner_info->up_edge_info;
	Down_IR_info->DownEdgeInfo = _corner_info->down_edge_info;
	Down_IR_info->LeftEdgeInfo = _corner_info->left_edge_info;
	Down_IR_info->RightEdgeInfo = _corner_info->right_edge_info;
	Down_IR_info->type = 0;

#ifdef SAVE_CORNER_IMG    //�ǵ��Ǵ�ͼ����
	IplImage_ p_image; //������ͼ��
	ReleaseImage(&p_image);
	CreateImage(&p_image, _corner_info->width, _corner_info->height, p_bottom_ir);
	p_image.width_step = _corner_info->width_step;
	char file_name[128];
	sprintf(file_name, "E:\\edge_corner_bottom_%d.bmp", _pic_data_info->file_num);
	int row_point = _corner_info->width >> ROW_2_POWER;
	int vert_point = _corner_info->height >> VERT_2_POWER;

	DrawImageEdgeCorner(&p_image, _corner_info, row_point, vert_point, file_name);

#endif

	CalcRealLengthOfMoney(p_bottom_ir, _corner_info);
	_pic_data_info->edge_real_len[0] = _corner_info->edge_real_len[0];
	_pic_data_info->edge_real_len[1] = _corner_info->edge_real_len[1];
	_pic_data_info->edge_real_len[2] = _corner_info->edge_real_len[2];
	_pic_data_info->edge_real_len[3] = _corner_info->edge_real_len[3];

    if (corner_ret == ERR_COORNER_BIG_ANGLE)
	{
		return ERR_COORNER_BIG_ANGLE;
	}
	_corner_info->height = _pic_data_info->height;
	_corner_info->missing_info = -1;
	corner_ret = FindCornerAndAngle(&img_breakage_check, p_up_ir, _corner_info, last_status);  //����ͼ�ҽǵ�
	if (corner_ret < 0)
	{
		return corner_ret;
	}
	GetTriType(_corner_info, _tri_type);   //�õ�����ͼ�ķ�����������

	for (int i = 0; i < 4; i++)
	{
		_pic_data_info->coordinates_up.x[i] = _corner_info->coordinates.x[i];
		_pic_data_info->coordinates_up.y[i] = _corner_info->coordinates.y[i];
	}
	_pic_data_info->missing_info[0] = _corner_info->missing_info;

	Up_IR_info->width = _pic_data_info->width;
	Up_IR_info->width_step = _pic_data_info->width_step;
	Up_IR_info->height = _pic_data_info->height;
	Up_IR_info->image_data = _pic_data_info->up_data + _pic_data_info->IR_img_offset[CIS_T];
	Up_IR_info->coordinates = _corner_info->coordinates;
	Up_IR_info->UpEdgeInfo = _corner_info->up_edge_info;
	Up_IR_info->DownEdgeInfo = _corner_info->down_edge_info;
	Up_IR_info->LeftEdgeInfo = _corner_info->left_edge_info;
	Up_IR_info->RightEdgeInfo = _corner_info->right_edge_info;
	Up_IR_info->type = 1;

#ifdef SAVE_CORNER_IMG    //�ǵ��Ǵ�ͼ����
	{
		IplImage_ p_image; //������ͼ��
		ReleaseImage(&p_image);
		CreateImage(&p_image, _corner_info->width, _corner_info->height, p_up_ir);
		p_image.width_step = _corner_info->width_step;
		char file_name[128];
		sprintf(file_name, "E:\\edge_corner_up_%d.bmp", _pic_data_info->file_num);
		int row_point = _corner_info->width >> ROW_2_POWER;
		int vert_point = _corner_info->height >> VERT_2_POWER;

		DrawImageEdgeCorner(&p_image, _corner_info, row_point, vert_point, file_name);
	}
#endif

	return 0;
}

//�ǵ�У��ģ�飺ȷ��ֽ�ҵ��ĸ��ǵ����꣬�������任����
//_up_imgΪ����ͼ��Դ��_down_imgΪ����ͼ��Դ��*_pic_data_infoΪͼ����Ϣ�ṹ��
int GetAffineParameter(PicDataInfo_ *_pic_data_info, int _tri_type[])
{
	int i;

	IntPoint_ src_tri[3];     //Ǯ�Ľǵ�����
	IntPoint_ dst_tri[3];        //У����Ǯ�Ľǵ����꣨���У�

	int width = _pic_data_info->width;    //ͼ��Ŀ��ߣ�������ɲ��״̬��Ϣ��ֵ��corner_info�ṹ��
	int height_up = _pic_data_info->height;
	int height_dowm = _pic_data_info->height;
	int affine_width = _pic_data_info->affine_width;
	int affine_height = _pic_data_info->affine_height;

	CalcAffineCoordinate(dst_tri, width, height_up, affine_width, affine_height, &_pic_data_info->coordinates_up, src_tri, _tri_type[0]);  //�õ�����任�õ�����

	GetAffineMat(src_tri, dst_tri, _pic_data_info->affine_operator_trans_up);   //��÷���任����

	CalcAffineCoordinate(dst_tri, width, height_dowm, affine_width, affine_height, &_pic_data_info->coordinates_bottom, src_tri, _tri_type[1]);  //�õ�����任�õ�����

	GetAffineMat(src_tri, dst_tri, _pic_data_info->affine_operator_trans_bottom);   //��÷���任����

	return 0;
}

int GetAffineImg(IplImage_ *money_grey_up, IplImage_ *money_grey_bottom, PicDataInfo_ *_pic_data_info)
{
	U8* p_up_ir = _pic_data_info->up_data + _pic_data_info->IR_img_offset[CIS_T];
	U8* p_bottom_ir = _pic_data_info->bottom_data + _pic_data_info->IR_img_offset[CIS_B];
	U8* p_up_grey = _pic_data_info->up_data + _pic_data_info->G_img_offset;           //����greyͼ
	U8* p_bottom_grey = _pic_data_info->bottom_data + _pic_data_info->G_img_offset;   //����greyͼ
	IplImage_ up_ir_img;  //������ͨ��ͼ������
	ReleaseImage(&up_ir_img);
	CreateImage(&up_ir_img, _pic_data_info->width, _pic_data_info->height, p_up_ir);    //����ͼ��
	up_ir_img.width_step = _pic_data_info->width_step;
	IplImage_ bottom_ir_img;
	ReleaseImage(&bottom_ir_img);
	CreateImage(&bottom_ir_img, _pic_data_info->width, _pic_data_info->height, p_bottom_ir);    //����bottomͼirͼ��
	bottom_ir_img.width_step = _pic_data_info->width_step;

	ZoneInfo_ zone;
	zone.width = _pic_data_info->affine_width - 2;
	zone.height = _pic_data_info->affine_height - 2;
	zone.xstart = (_pic_data_info->width - zone.width)*0.5;		//�ο���ƫ��
	zone.ystart = (_pic_data_info->height - zone.height)*0.5;

	ReleaseImage(money_grey_up);
	ReleaseImage(money_grey_bottom);

	CreateImage(money_grey_up, zone.width, zone.height, affine_data_up);
	CreateImage(money_grey_bottom, zone.width, zone.height, affine_data_down);

	WarpAffineMoneyInt(&up_ir_img, p_up_grey, money_grey_up, &zone, _pic_data_info->affine_operator_trans_up);

	WarpAffineMoneyInt(&bottom_ir_img, p_bottom_grey, money_grey_bottom, &zone, _pic_data_info->affine_operator_trans_bottom);
	/*---------------------------------------------------------------*/

	return 0;
}

void GetTriType(CornerCorrectInfo_ *_corner_info, int *_tri_type)
{
	//ȷ�����������Ӧ��λ��
	//if ((abs(_corner_info->left_edge_info.degree - _corner_info->right_edge_info.degree) <= 4)
	//	|| (_corner_info->left_edge_info.confidence > _corner_info->right_edge_info.confidence))
	//{
	//	*_tri_type = 0;
	//}
	//else
	//{
	//	*_tri_type = 1;
	//}

	/******************/
	if (_corner_info->edge_feature[0].confidence >= _corner_info->edge_feature[1].confidence)
	{
		if (_corner_info->edge_feature[2].confidence >= _corner_info->edge_feature[3].confidence)
		{
			*_tri_type = 0;
		}
		else
		{
			*_tri_type = 1;
		}
	}
	else
	{
		if (_corner_info->edge_feature[2].confidence >= _corner_info->edge_feature[3].confidence)
		{

			*_tri_type = 2;
	     }
		else
		{
			*_tri_type = 3;
		}
	}
	/******************/
	
	}

int ValueCheckByLength(PicDataInfo_ *_pic_data_info, int confidence_up, int confidence_down)
{

	float real_len = _pic_data_info->edge_real_len[0];
	if (_pic_data_info->edge_real_len[0] > 0 && _pic_data_info->edge_real_len[1] > 0)
	{
		if (confidence_up >= confidence_down)
		{
			real_len = _pic_data_info->edge_real_len[0];
		}
		else
		{
			real_len = _pic_data_info->edge_real_len[1];
		}
	}
	else if (_pic_data_info->edge_real_len[0] > 0)
	{
		real_len = _pic_data_info->edge_real_len[0];
	}
	else
	{
		real_len = _pic_data_info->edge_real_len[1];
	}

	float range = 2.5;
	int cal_value = 0;
	if (real_len > (LENGTH100 - range) && real_len < (LENGTH100 + range))
	{
		cal_value = 100;
	}
	else if (real_len >(LENGTH50 - range) && real_len < (LENGTH50 + range))
	{
		cal_value = 50;
	}
	else if (real_len >(LENGTH20 - range) && real_len < (LENGTH20 + range))
	{
		cal_value = 20;
	}
	else if (real_len >(LENGTH10 - range) && real_len < (LENGTH10 + range))
	{
		cal_value = 10;
	}
	else if (real_len >(LENGTH5 - range) && real_len < (LENGTH5 + range))
	{
		cal_value = 5;
	}
	else if (real_len >(LENGTH1 - range) && real_len < (LENGTH1 + range))
	{
		cal_value = 1;
	}

	return cal_value;
}

//int WarpAffineMoneyInt_org(IplImage_ *_src_ir_img, u8 *p_src_grey, IplImage_ *_new_ir_img, IplImage_ *_new_grey_img, ZoneInfo_ *zone_info, int *Mat)
//{
//	int i, j;
//	int i1, j1;
//	int i0, j0;
//	int decimal_i, decimal_j;
//	int f12, f34;
//	int coordinate1;
//
//	int width = _src_ir_img->width;   // Դͼ��Ŀ�Ⱥ͸߶�
//	int height = _src_ir_img->height;
//	int widthstep = _src_ir_img->width_step;
//	int new_width = _new_ir_img->width;       // ��ת��ͼ��Ŀ�Ⱥ͸߶�
//	int new_height = _new_ir_img->height;
//
//	int y_start = zone_info->ystart;
//	int x_start = zone_info->xstart;
//
//	unsigned char *p_new_ir = (unsigned char*)_new_ir_img->image_data;
//	unsigned char *p_new_grey = (unsigned char*)_new_grey_img->image_data;
//	unsigned char *p_src_ir = (unsigned char*)_src_ir_img->image_data;
//	//unsigned char *p_src_grey = (unsigned char*)_src_grey_img->image_data;
//
//	zone_info->type = TYPE_ROTATE_LINE;
//
//	int sjtmp = 0;
//	int sitmp = 0;
//	int srcpos = 0;
//	if (zone_info->type == TYPE_ROTATE_LINE)    //���Բ�ֵ
//	{
//		int flag = 1;
//
//		i = y_start;
//		j = x_start;
//		j1 = (j * Mat[0] + i * Mat[1] + Mat[2] + 32768) >> 16;
//		i1 = (j * Mat[3] + i * Mat[4] + Mat[5] + 32768) >> 16;
//
//		if (!((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1)))   // �ж��Ƿ���Դͼ��Χ��
//		{
//			flag = 0;
//		}
//
//		i = y_start + new_height - 1;
//		j = x_start;
//		j1 = (j * Mat[0] + i * Mat[1] + Mat[2] + 32768) >> 16;
//		i1 = (j * Mat[3] + i * Mat[4] + Mat[5] + 32768) >> 16;
//
//		if (!((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1)))   // �ж��Ƿ���Դͼ��Χ��
//		{
//			flag = 0;
//		}
//
//		i = y_start;
//		j = x_start + new_width - 1;
//		j1 = (j * Mat[0] + i * Mat[1] + Mat[2] + 32768) >> 16;
//		i1 = (j * Mat[3] + i * Mat[4] + Mat[5] + 32768) >> 16;
//
//		if (!((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1)))   // �ж��Ƿ���Դͼ��Χ��
//		{
//			flag = 0;
//		}
//
//		i = y_start + new_height - 1;
//		j = x_start + new_width - 1;
//		j1 = (j * Mat[0] + i * Mat[1] + Mat[2] + 32768) >> 16;
//		i1 = (j * Mat[3] + i * Mat[4] + Mat[5] + 32768) >> 16;
//
//		if (!((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1)))   // �ж��Ƿ���Դͼ��Χ��
//		{
//			flag = 0;
//		}
//
//		if (flag == 1)
//		{
//
//			for (i = y_start; i < y_start + new_height; i++)
//			{
//				sjtmp = i * Mat[1] + Mat[2] + 32768 + (x_start - 1)*Mat[0];
//				sitmp = i * Mat[4] + Mat[5] + 32768 + (x_start - 1)*Mat[3];
//
//				for (j = x_start; j < x_start + new_width; j++)     // ���ͼ��ÿ�н��в���
//				{
//					sjtmp += Mat[0];
//					sitmp += Mat[3];
//					j1 = sjtmp >> 16;
//					i1 = sitmp >> 16;
//
//					srcpos = i1*widthstep;
//					*p_new_ir = p_src_ir[srcpos + j1];
//					*p_new_grey = p_src_grey[srcpos + j1];
//
//					p_new_ir++;
//					p_new_grey++;
//				}
//				p_new_ir += _new_ir_img->width_step - new_width;
//				p_new_grey += _new_grey_img->width_step - new_width;
//			}
//		}
//		else
//		{
//
//			for (i = y_start; i < y_start + new_height; i++)
//			{
//				sjtmp = i * Mat[1] + Mat[2] + 32768 + (x_start - 1)*Mat[0];
//				sitmp = i * Mat[4] + Mat[5] + 32768 + (x_start - 1)*Mat[3];
//
//				for (j = x_start; j < x_start + new_width; j++)     // ���ͼ��ÿ�н��в���
//				{
//					sjtmp += Mat[0];
//					sitmp += Mat[3];
//					j1 = sjtmp >> 16;
//					i1 = sitmp >> 16;
//
//					if ((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1))   // �ж��Ƿ���Դͼ��Χ��
//					{
//						srcpos = i1*widthstep;
//						*p_new_ir = p_src_ir[srcpos + j1];
//						*p_new_grey = p_src_grey[srcpos + j1];
//					}
//					else
//					{
//						*p_new_ir = 0;
//						*p_new_grey = 0;
//					}
//					p_new_ir++;
//					p_new_grey++;
//				}
//				p_new_ir += _new_ir_img->width_step - new_width;
//				p_new_grey += _new_grey_img->width_step - new_width;
//			}
//		}
//
//	}
//	else     //˫���Բ�ֵ
//	{
//		for (i = y_start; i < y_start + new_height; i++)
//		{
//			for (j = x_start; j < x_start + new_width; j++)	// ���ͼ��ÿ�н��в���
//			{
//				j0 = j * Mat[0] + i * Mat[1] + Mat[2];
//				i0 = j * Mat[3] + i * Mat[4] + Mat[5];
//
//				decimal_i = i0 & 0xffff;
//				decimal_j = j0 & 0xffff;
//				i1 = i0 >> 16;
//				j1 = j0 >> 16;
//				coordinate1 = i1*widthstep + j1;
//
//				if ((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1))	// �ж��Ƿ���Դͼ��Χ��
//				{
//					f12 = (p_src_ir[coordinate1] << 16) + decimal_j*(p_src_ir[coordinate1 + 1] - p_src_ir[coordinate1]);
//					f34 = (p_src_ir[coordinate1 + widthstep] << 16) + decimal_j*(p_src_ir[coordinate1 + widthstep + 1] - p_src_ir[coordinate1 + widthstep]);
//					*p_new_ir = abs(f12 + decimal_i* ((f34 - f12) >> 16)) >> 16;
//
//					f12 = (p_src_grey[coordinate1] << 16) + decimal_j*(p_src_grey[coordinate1 + 1] - p_src_grey[coordinate1]);
//					f34 = (p_src_grey[coordinate1 + widthstep] << 16) + decimal_j*(p_src_grey[coordinate1 + widthstep + 1] - p_src_grey[coordinate1 + widthstep]);
//					*p_new_grey = abs(f12 + decimal_i* ((f34 - f12) >> 16)) >> 16;
//				}
//				else
//				{
//					*p_new_ir = 0;
//					*p_new_grey = 0;
//				}
//				p_new_ir++;
//				p_new_grey++;
//			}
//			p_new_ir += _new_ir_img->width_step - new_width;
//			p_new_grey += _new_grey_img->width_step - new_width;
//		}
//	}
//	return 0;
//}

int WarpAffineMoneyInt(IplImage_ *_src_ir_img, u8 *p_src_grey, IplImage_ *_new_grey_img, ZoneInfo_ *zone_info, int *Mat)
{
	int i, j;
	int i1, j1;
	int i0, j0;
	int decimal_i, decimal_j;
	int f12, f34;
	int coordinate1;

	int width = _src_ir_img->width;   // Դͼ��Ŀ�Ⱥ͸߶�
	int height = _src_ir_img->height;
	int widthstep = _src_ir_img->width_step;
	int new_width = _new_grey_img->width;       // ��ת��ͼ��Ŀ�Ⱥ͸߶�
	int new_height = _new_grey_img->height;

	int y_start = zone_info->ystart;
	int x_start = zone_info->xstart;

	unsigned char *p_new_grey = (unsigned char*)_new_grey_img->image_data;

	zone_info->type = TYPE_ROTATE_LINE;

	int sjtmp = 0;
	int sitmp = 0;
	int srcpos = 0;
	if (zone_info->type == TYPE_ROTATE_LINE)    //���Բ�ֵ
	{
		int flag = 1;

		i = y_start;
		j = x_start;
		j1 = (j * Mat[0] + i * Mat[1] + Mat[2] + 32768) >> 16;
		i1 = (j * Mat[3] + i * Mat[4] + Mat[5] + 32768) >> 16;

		if (!((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1)))   // �ж��Ƿ���Դͼ��Χ��
		{
			flag = 0;
		}

		i = y_start + new_height - 1;
		j = x_start;
		j1 = (j * Mat[0] + i * Mat[1] + Mat[2] + 32768) >> 16;
		i1 = (j * Mat[3] + i * Mat[4] + Mat[5] + 32768) >> 16;

		if (!((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1)))   // �ж��Ƿ���Դͼ��Χ��
		{
			flag = 0;
		}

		i = y_start;
		j = x_start + new_width - 1;
		j1 = (j * Mat[0] + i * Mat[1] + Mat[2] + 32768) >> 16;
		i1 = (j * Mat[3] + i * Mat[4] + Mat[5] + 32768) >> 16;

		if (!((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1)))   // �ж��Ƿ���Դͼ��Χ��
		{
			flag = 0;
		}

		i = y_start + new_height - 1;
		j = x_start + new_width - 1;
		j1 = (j * Mat[0] + i * Mat[1] + Mat[2] + 32768) >> 16;
		i1 = (j * Mat[3] + i * Mat[4] + Mat[5] + 32768) >> 16;

		if (!((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1)))   // �ж��Ƿ���Դͼ��Χ��
		{
			flag = 0;
		}

		if (flag == 1)
		{

			for (i = y_start; i < y_start + new_height; i++)
			{
				sjtmp = i * Mat[1] + Mat[2] + 32768 + (x_start - 1)*Mat[0];
				sitmp = i * Mat[4] + Mat[5] + 32768 + (x_start - 1)*Mat[3];

				for (j = x_start; j < x_start + new_width; j++)     // ���ͼ��ÿ�н��в���
				{
					sjtmp += Mat[0];
					sitmp += Mat[3];
					j1 = sjtmp >> 16;
					i1 = sitmp >> 16;

					srcpos = i1*widthstep;
					*p_new_grey = p_src_grey[srcpos + j1];

					p_new_grey++;
				}
				p_new_grey += _new_grey_img->width_step - new_width;
			}
		}
		else
		{

			for (i = y_start; i < y_start + new_height; i++)
			{
				sjtmp = i * Mat[1] + Mat[2] + 32768 + (x_start - 1)*Mat[0];
				sitmp = i * Mat[4] + Mat[5] + 32768 + (x_start - 1)*Mat[3];

				for (j = x_start; j < x_start + new_width; j++)     // ���ͼ��ÿ�н��в���
				{
					sjtmp += Mat[0];
					sitmp += Mat[3];
					j1 = sjtmp >> 16;
					i1 = sitmp >> 16;

					if ((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1))   // �ж��Ƿ���Դͼ��Χ��
					{
						srcpos = i1*widthstep;
						*p_new_grey = p_src_grey[srcpos + j1];
					}
					else
					{
						*p_new_grey = 0;
					}
					p_new_grey++;
				}
				p_new_grey += _new_grey_img->width_step - new_width;
			}
		}

	}
	else     //˫���Բ�ֵ
	{
		for (i = y_start; i < y_start + new_height; i++)
		{
			for (j = x_start; j < x_start + new_width; j++)	// ���ͼ��ÿ�н��в���
			{
				j0 = j * Mat[0] + i * Mat[1] + Mat[2];
				i0 = j * Mat[3] + i * Mat[4] + Mat[5];

				decimal_i = i0 & 0xffff;
				decimal_j = j0 & 0xffff;
				i1 = i0 >> 16;
				j1 = j0 >> 16;
				coordinate1 = i1*widthstep + j1;

				if ((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1))	// �ж��Ƿ���Դͼ��Χ��
				{
					f12 = (p_src_grey[coordinate1] << 16) + decimal_j*(p_src_grey[coordinate1 + 1] - p_src_grey[coordinate1]);
					f34 = (p_src_grey[coordinate1 + widthstep] << 16) + decimal_j*(p_src_grey[coordinate1 + widthstep + 1] - p_src_grey[coordinate1 + widthstep]);
					*p_new_grey = abs(f12 + decimal_i* ((f34 - f12) >> 16)) >> 16;
				}
				else
				{
					*p_new_grey = 0;
				}
				p_new_grey++;
			}
			p_new_grey += _new_grey_img->width_step - new_width;
		}
	}
	return 0;
}

//�����ͼ����
int WarpAffineZoneInt(IplImage_ *_new_img, IplImage_ *_src_img, RotateZoneInfo_ *zone_info, int *_mat)
{
	int i, j;
	int i1, j1;
	int i0, j0;
	int decimal_i, decimal_j;
	int f12, f34;
	int x, y, Coordinate1;

	int width = _src_img->width;   // Դͼ��Ŀ�Ⱥ͸߶�
	int height = _src_img->height;
	int widthstep = _src_img->width_step;
	int new_width = _new_img->width;       // ��ת��ͼ��Ŀ�Ⱥ͸߶�
	int new_height = _new_img->height;
	int new_width_step = _new_img->width_step;

	int y_start = zone_info->ystart;
	int x_start = zone_info->xstart;

	unsigned char *p_new = _new_img->image_data;
	unsigned char *p_src = _src_img->image_data;

	if (zone_info->type == TYPE_ROTATE_LINE)
	{
		/*********** ���Բ�ֵ ****************/
		if (zone_info->direction == DIRECTION_RIGHT)    //��ͼ����ͼʱ��������棬���ҷ�ת�����Եõ���ͼ��ͼ
		{
			for (i = y_start; i < y_start + new_height; i++)
			{
				for (j = x_start + new_width - 1; j >= x_start; j--)
				{
					j1 = (j * _mat[0] + i * _mat[1] + _mat[2] + 32768) >> 16;
					i1 = (j * _mat[3] + i * _mat[4] + _mat[5] + 32768) >> 16;

					if ((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1))  // �ж��Ƿ���Դͼ��Χ��
					{
						*p_new = p_src[i1*widthstep + j1];
					}
					else
						*p_new = 0;

					p_new++;
				}
				p_new += _new_img->width_step - new_width;
			}
		}
		else if (zone_info->direction == DIRECTION_LEFT)    //��ͼ����ͼʱ�������ϴ棬���·�ת�����Եõ���ͼ��ͼ
		{
			for (i = y_start + new_height - 1; i >= y_start; i--)
			{
				for (j = x_start; j < x_start + new_width; j++)
				{
					j1 = (j * _mat[0] + i * _mat[1] + _mat[2] + 32768) >> 16;
					i1 = (j * _mat[3] + i * _mat[4] + _mat[5] + 32768) >> 16;

					if ((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1))  // �ж��Ƿ���Դͼ��Χ��
					{
						*p_new = p_src[i1*widthstep + j1];
					}
					else
					{
						*p_new = 0;
					}
					p_new++;
				}
				p_new += _new_img->width_step - new_width;
			}
		}
		else if (zone_info->direction == DIRECTION_SAME)    //����ͼ��ʲô����ģ�����ת��ֱ�ӽ�ͼ
		{
			for (i = y_start; i < y_start + new_height; i++)
			{
				for (j = x_start; j < x_start + new_width; j++)
				{
					j1 = (j * _mat[0] + i * _mat[1] + _mat[2] + 32768) >> 16;
					i1 = (j * _mat[3] + i * _mat[4] + _mat[5] + 32768) >> 16;

					if ((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1))	// �ж��Ƿ���Դͼ��Χ��
					{
						*p_new = p_src[i1*widthstep + j1];
					}
					else
					{
						*p_new = 0;
					}
					p_new++;
				}
				p_new += _new_img->width_step - new_width;
			}
		}
		else
		{
			return -1;
		}
	}
	else
	{
		/*********** ˫���Բ�ֵ ****************/
		if (zone_info->direction == DIRECTION_RIGHT)
		{
			for (i = y_start; i < y_start + new_height; i++)
			{
				for (j = x_start + new_width - 1; j >= x_start; j--)
				{
					j0 = j * _mat[0] + i * _mat[1] + _mat[2];
					i0 = j * _mat[3] + i * _mat[4] + _mat[5];

					decimal_i = i0 & 0xffff;
					decimal_j = j0 & 0xffff;
					i1 = i0 >> 16;
					j1 = j0 >> 16;
					Coordinate1 = i1*widthstep + j1;

					if ((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1))  // �ж��Ƿ���Դͼ��Χ��
					{
						f12 = (p_src[Coordinate1] << 16) + decimal_j*(p_src[Coordinate1 + 1] - p_src[Coordinate1]);
						f34 = (p_src[Coordinate1 + widthstep] << 16) + decimal_j*(p_src[Coordinate1 + widthstep + 1] - p_src[Coordinate1 + widthstep]);
						*p_new = abs(f12 + decimal_i* ((f34 - f12) >> 16)) >> 16;
					}
					else
					{
						*p_new = 0;
					}
					p_new++;
				}
				p_new += _new_img->width_step - new_width;
			}
		}
		else if (zone_info->direction == DIRECTION_LEFT)
		{
			for (i = y_start + new_height - 1; i >= y_start; i--)
			{
				for (j = x_start; j < x_start + new_width; j++)
				{
					j0 = j * _mat[0] + i * _mat[1] + _mat[2];
					i0 = j * _mat[3] + i * _mat[4] + _mat[5];

					decimal_i = i0 & 0xffff;
					decimal_j = j0 & 0xffff;
					i1 = i0 >> 16;
					j1 = j0 >> 16;
					Coordinate1 = i1*widthstep + j1;

					if ((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1))	// �ж��Ƿ���Դͼ��Χ��
					{
						f12 = (p_src[Coordinate1] << 16) + decimal_j*(p_src[Coordinate1 + 1] - p_src[Coordinate1]);
						f34 = (p_src[Coordinate1 + widthstep] << 16) + decimal_j*(p_src[Coordinate1 + widthstep + 1] - p_src[Coordinate1 + widthstep]);
						*p_new = abs(f12 + decimal_i* ((f34 - f12) >> 16)) >> 16;
					}
					else
					{
						*p_new = 0;
					}
					p_new++;
				}
				p_new += _new_img->width_step - new_width;
			}
		}
		else if (zone_info->direction == DIRECTION_SAME)
		{
			for (i = y_start; i < y_start + new_height; i++)
			{
				for (j = x_start; j < x_start + new_width; j++)
				{
					j0 = j * _mat[0] + i * _mat[1] + _mat[2];
					i0 = j * _mat[3] + i * _mat[4] + _mat[5];

					decimal_i = i0 & 0xffff;
					decimal_j = j0 & 0xffff;
					i1 = i0 >> 16;
					j1 = j0 >> 16;
					Coordinate1 = i1*widthstep + j1;

					if ((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1))	// �ж��Ƿ���Դͼ��Χ��
					{
						f12 = (p_src[Coordinate1] << 16) + decimal_j*(p_src[Coordinate1 + 1] - p_src[Coordinate1]);
						f34 = (p_src[Coordinate1 + widthstep] << 16) + decimal_j*(p_src[Coordinate1 + widthstep + 1] - p_src[Coordinate1 + widthstep]);
						*p_new = abs(f12 + decimal_i* ((f34 - f12) >> 16)) >> 16;
					}
					else
					{
						*p_new = 0;
					}
					p_new++;
				}
				p_new += _new_img->width_step - new_width;
			}
		}
		else
		{
			return -1;
		}
	}

	return 0;
}

//�������������ͼ����
int WarpAffineZoneInt_Samp(IplImage_ *_new_ir_img, IplImage_ *_new_grey_img, IplImage_ *_src_ir_img, IplImage_ *_src_grey_img, RotateZoneInfo_ *zone_info, int *_mat, int _sample_ena, int _row_samp_rate, int _col_samp_rate)
{
	int i, j;
	int i1, j1;

	int row_offset, col_offset;

	int row_samp;
	int col_samp;
	if (_sample_ena == 1)
	{
		row_samp = _row_samp_rate;
		col_samp = _col_samp_rate;
	}
	else
	{
		row_samp = 1;
		col_samp = 1;
	}

	int width = _src_ir_img->width;   // Դͼ��Ŀ�Ⱥ͸߶�
	int height = _src_ir_img->height;
	int widthstep = _src_ir_img->width_step;
	int new_width = _new_ir_img->width*col_samp;       // ��ת��ͼ��Ŀ�Ⱥ͸߶�
	int new_height = _new_ir_img->height*row_samp;

	int y_start = zone_info->ystart;    //С�����ͼ����ʼ����
	int x_start = zone_info->xstart;

	unsigned char *p_new_ir = _new_ir_img->image_data;
	unsigned char *p_new_grey = _new_grey_img->image_data;
	unsigned char *p_src_ir = _src_ir_img->image_data;
	unsigned char *p_src_grey = _src_grey_img->image_data;

	int srcpos = 0;
	/*********** ���Բ�ֵ ****************/
	int flag = 1;

	i = y_start;
	j = x_start;
	j1 = (j * _mat[0] + i * _mat[1] + _mat[2] + 32768) >> 16;
	i1 = (j * _mat[3] + i * _mat[4] + _mat[5] + 32768) >> 16;

	if (!((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1)))   // �ж��Ƿ���Դͼ��Χ��
	{
		flag = 0;
	}

	i = y_start + new_height - row_samp;
	j = x_start;
	j1 = (j * _mat[0] + i * _mat[1] + _mat[2] + 32768) >> 16;
	i1 = (j * _mat[3] + i * _mat[4] + _mat[5] + 32768) >> 16;

	if (!((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1)))   // �ж��Ƿ���Դͼ��Χ��
	{
		flag = 0;
	}

	i = y_start;
	j = x_start + new_width - col_samp;
	j1 = (j * _mat[0] + i * _mat[1] + _mat[2] + 32768) >> 16;
	i1 = (j * _mat[3] + i * _mat[4] + _mat[5] + 32768) >> 16;

	if (!((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1)))   // �ж��Ƿ���Դͼ��Χ��
	{
		flag = 0;
	}

	i = y_start + new_height - row_samp;
	j = x_start + new_width - col_samp;
	j1 = (j * _mat[0] + i * _mat[1] + _mat[2] + 32768) >> 16;
	i1 = (j * _mat[3] + i * _mat[4] + _mat[5] + 32768) >> 16;

	if (!((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1)))   // �ж��Ƿ���Դͼ��Χ��
	{
		flag = 0;
	}

	if (flag == 1)
	{
		if (zone_info->direction == DIRECTION_RIGHT)   //��ΪYͨ��ͼ�ҹ��ֺ����ң���direction=1��3
		{
			for (i = y_start; i < y_start + new_height; i += row_samp)
			{
				col_offset = i * _mat[1] + _mat[2] + 32768;
				row_offset = i * _mat[4] + _mat[5] + 32768;
				for (j = x_start + new_width - col_samp; j >= x_start; j -= col_samp)   // ���ͼ��ÿ�н��в���
				{
					j1 = (j * _mat[0] + col_offset) >> 16;
					i1 = (j * _mat[3] + row_offset) >> 16;

					srcpos = i1*widthstep;
					*p_new_ir = p_src_ir[srcpos + j1];
					*p_new_grey = p_src_grey[srcpos + j1];

					p_new_ir++;
					p_new_grey++;
				}
				p_new_ir += _new_ir_img->width_step - _new_ir_img->width;
				p_new_grey += _new_grey_img->width_step - _new_grey_img->width;
			}
		}
		else if (zone_info->direction == DIRECTION_LEFT)     //��ΪYͨ��ͼ�ҹ��ֺ����󣬼�direction=0��2
		{
			for (i = y_start + new_height - row_samp; i >= y_start; i -= row_samp)
			{
				// ���ͼ��ÿ�н��в���
				col_offset = i * _mat[1] + _mat[2] + 32768;
				row_offset = i * _mat[4] + _mat[5] + 32768;
				for (j = x_start; j < x_start + new_width; j += col_samp)
				{
					j1 = (j * _mat[0] + col_offset) >> 16;
					i1 = (j * _mat[3] + row_offset) >> 16;

					srcpos = i1*widthstep;
					*p_new_ir = p_src_ir[srcpos + j1];
					*p_new_grey = p_src_grey[srcpos + j1];

					p_new_ir++;
					p_new_grey++;
				}
				p_new_ir += _new_ir_img->width_step - _new_ir_img->width;
				p_new_grey += _new_grey_img->width_step - _new_grey_img->width;
			}
		}
	}
	else
	{
		if (zone_info->direction == DIRECTION_RIGHT)   //��ΪYͨ��ͼ�ҹ��ֺ����ң���direction=1��3
		{
			for (i = y_start; i < y_start + new_height; i += row_samp)
			{
				col_offset = i * _mat[1] + _mat[2] + 32768;
				row_offset = i * _mat[4] + _mat[5] + 32768;
				for (j = x_start + new_width - col_samp; j >= x_start; j -= col_samp)   // ���ͼ��ÿ�н��в���
				{
					j1 = (j * _mat[0] + col_offset) >> 16;
					i1 = (j * _mat[3] + row_offset) >> 16;

					if ((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1))   // �ж��Ƿ���Դͼ��Χ��
					{
						srcpos = i1*widthstep;
						*p_new_ir = p_src_ir[srcpos + j1];
						*p_new_grey = p_src_grey[srcpos + j1];
					}
					else
					{
						*p_new_grey = *p_new_ir = 0;
					}
					p_new_ir++;
					p_new_grey++;
				}
				p_new_ir += _new_ir_img->width_step - _new_ir_img->width;
				p_new_grey += _new_grey_img->width_step - _new_grey_img->width;
			}
		}
		else if (zone_info->direction == DIRECTION_LEFT)     //��ΪYͨ��ͼ�ҹ��ֺ����󣬼�direction=0��2
		{
			for (i = y_start + new_height - row_samp; i >= y_start; i -= row_samp)
			{
				// ���ͼ��ÿ�н��в���
				col_offset = i * _mat[1] + _mat[2] + 32768;
				row_offset = i * _mat[4] + _mat[5] + 32768;
				for (j = x_start; j < x_start + new_width; j += col_samp)
				{
					j1 = (j * _mat[0] + col_offset) >> 16;
					i1 = (j * _mat[3] + row_offset) >> 16;
					// �ж��Ƿ���Դͼ��Χ��
					if ((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1))
					{
						srcpos = i1*widthstep;
						*p_new_ir = p_src_ir[srcpos + j1];
						*p_new_grey = p_src_grey[srcpos + j1];
					}
					else
					{
						*p_new_grey = *p_new_ir = 0;
					}
					p_new_ir++;
					p_new_grey++;
				}
				p_new_ir += _new_ir_img->width_step - _new_ir_img->width;
				p_new_grey += _new_grey_img->width_step - _new_grey_img->width;
			}
		}
	}

	return 0;
}

//�������������ͼ����
int WarpAffineZoneInt_Samp_One(IplImage_ *_new_img, IplImage_ *_src_img, RotateZoneInfo_ *zone_info, int *_mat, int _sample_ena, int _row_samp_rate, int _col_samp_rate)
{
	int i, j;
	int i1, j1;

	int row_offset, col_offset;

	int row_samp;
	int col_samp;
	if (_sample_ena == 1)
	{
		row_samp = _row_samp_rate;
		col_samp = _col_samp_rate;
	}
	else
	{
		row_samp = 1;
		col_samp = 1;
	}

	int width = _src_img->width;   // Դͼ��Ŀ�Ⱥ͸߶�
	int height = _src_img->height;
	int widthstep = _src_img->width_step;
	int new_width = _new_img->width*col_samp;       // ��ת��ͼ��Ŀ�Ⱥ͸߶�
	int new_height = _new_img->height*row_samp;

	int y_start = zone_info->ystart;    //С�����ͼ����ʼ����
	int x_start = zone_info->xstart;

	unsigned char *p_new = _new_img->image_data;
	unsigned char *p_src = _src_img->image_data;

	/*********** ���Բ�ֵ ****************/
	int flag = 1;

	i = y_start;
	j = x_start;
	j1 = (j * _mat[0] + i * _mat[1] + _mat[2] + 32768) >> 16;
	i1 = (j * _mat[3] + i * _mat[4] + _mat[5] + 32768) >> 16;

	if (!((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1)))   // �ж��Ƿ���Դͼ��Χ��
	{
		flag = 0;
	}

	i = y_start + new_height - row_samp;
	j = x_start;
	j1 = (j * _mat[0] + i * _mat[1] + _mat[2] + 32768) >> 16;
	i1 = (j * _mat[3] + i * _mat[4] + _mat[5] + 32768) >> 16;

	if (!((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1)))   // �ж��Ƿ���Դͼ��Χ��
	{
		flag = 0;
	}

	i = y_start;
	j = x_start + new_width - col_samp;
	j1 = (j * _mat[0] + i * _mat[1] + _mat[2] + 32768) >> 16;
	i1 = (j * _mat[3] + i * _mat[4] + _mat[5] + 32768) >> 16;

	if (!((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1)))   // �ж��Ƿ���Դͼ��Χ��
	{
		flag = 0;
	}

	i = y_start + new_height - row_samp;
	j = x_start + new_width - col_samp;
	j1 = (j * _mat[0] + i * _mat[1] + _mat[2] + 32768) >> 16;
	i1 = (j * _mat[3] + i * _mat[4] + _mat[5] + 32768) >> 16;

	if (!((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1)))   // �ж��Ƿ���Դͼ��Χ��
	{
		flag = 0;
	}

	if (flag == 1)
	{
		if (zone_info->direction == DIRECTION_RIGHT)   //��ΪYͨ��ͼ�ҹ��ֺ����ң���direction=1��3
		{
			for (i = y_start; i < y_start + new_height; i += row_samp)
			{
				// ���ͼ��ÿ�н��в���
				col_offset = i *  _mat[1] + _mat[2] + 32768;
				row_offset = i *  _mat[4] + _mat[5] + 32768;
				for (j = x_start + new_width - col_samp; j >= x_start; j -= col_samp)
				{
					j1 = (j *  _mat[0] + col_offset) >> 16;
					i1 = (j *  _mat[3] + row_offset) >> 16;
			
					*p_new = p_src[i1*widthstep + j1];
					
					p_new++;
				}
				p_new += _new_img->width_step - _new_img->width;
			}
		}
		else if (zone_info->direction == DIRECTION_LEFT)     //��ΪYͨ��ͼ�ҹ��ֺ����󣬼�direction=0��2
		{
			for (i = y_start + new_height - row_samp; i >= y_start; i -= row_samp)
			{
				// ���ͼ��ÿ�н��в���
				col_offset = i * _mat[1] + _mat[2] + 32768;
				row_offset = i * _mat[4] + _mat[5] + 32768;
				for (j = x_start; j < x_start + new_width; j += col_samp)
				{
					j1 = (j * _mat[0] + col_offset) >> 16;
					i1 = (j * _mat[3] + row_offset) >> 16;
					
					*p_new = p_src[i1*widthstep + j1];
					
					p_new++;
				}
				p_new += _new_img->width_step - _new_img->width;
			}
		}
	}
	else
	{
		if (zone_info->direction == DIRECTION_RIGHT)   //��ΪYͨ��ͼ�ҹ��ֺ����ң���direction=1��3
		{
			for (i = y_start; i < y_start + new_height; i += row_samp)
			{
				// ���ͼ��ÿ�н��в���
				col_offset = i *  _mat[1] + _mat[2] + 32768;
				row_offset = i *  _mat[4] + _mat[5] + 32768;
				for (j = x_start + new_width - col_samp; j >= x_start; j -= col_samp)
				{
					j1 = (j *  _mat[0] + col_offset) >> 16;
					i1 = (j *  _mat[3] + row_offset) >> 16;

					// �ж��Ƿ���Դͼ��Χ��
					if ((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1))
					{
						*p_new = p_src[i1*widthstep + j1];
					}
					else
					{
						*p_new = 0;
					}
					p_new++;
				}
				p_new += _new_img->width_step - _new_img->width;
			}
		}
		else if (zone_info->direction == DIRECTION_LEFT)     //��ΪYͨ��ͼ�ҹ��ֺ����󣬼�direction=0��2
		{
			for (i = y_start + new_height - row_samp; i >= y_start; i -= row_samp)
			{
				// ���ͼ��ÿ�н��в���
				col_offset = i * _mat[1] + _mat[2] + 32768;
				row_offset = i * _mat[4] + _mat[5] + 32768;
				for (j = x_start; j < x_start + new_width; j += col_samp)
				{
					j1 = (j * _mat[0] + col_offset) >> 16;
					i1 = (j * _mat[3] + row_offset) >> 16;
					// �ж��Ƿ���Դͼ��Χ��
					if ((j1 >= 0) && (j1 < width - 1) && (i1 >= 0) && (i1 < height - 1))
					{
						*p_new = p_src[i1*widthstep + j1];
					}
					else
					{
						*p_new = 0;
					}
					p_new++;
				}
				p_new += _new_img->width_step - _new_img->width;
			}
		}
	}
	
	return 0;
}

int getZoneCoordinates(int coord[][3], int* val_rows, int* del_rows, int width, int height, RotateZoneInfo_ *zone_info, int *_mat)
{
	*val_rows = 0;
	*del_rows = 0;

	int i, j;
	int new_width = zone_info->width;       // ��ת��ͼ��Ŀ�Ⱥ͸߶�
	int new_height = zone_info->height;
	int y_start = zone_info->ystart;    //С�����ͼ����ʼ����
	int x_start = zone_info->xstart;

	IntPoint_ point[4];

	i = y_start;
	j = x_start;
	point[0].x = (j * _mat[0] + i * _mat[1] + _mat[2] + 32768) >> 16;
	point[0].y = (j * _mat[3] + i * _mat[4] + _mat[5] + 32768) >> 16;

	i = y_start;
	j = x_start + new_width - 1;
	point[1].x = (j * _mat[0] + i * _mat[1] + _mat[2] + 32768) >> 16;
	point[1].y = (j * _mat[3] + i * _mat[4] + _mat[5] + 32768) >> 16;

	i = y_start + new_height - 1;
	j = x_start + new_width - 1;
	point[2].x = (j * _mat[0] + i * _mat[1] + _mat[2] + 32768) >> 16;
	point[2].y = (j * _mat[3] + i * _mat[4] + _mat[5] + 32768) >> 16;

	i = y_start + new_height - 1;
	j = x_start;
	point[3].x = (j * _mat[0] + i * _mat[1] + _mat[2] + 32768) >> 16;
	point[3].y = (j * _mat[3] + i * _mat[4] + _mat[5] + 32768) >> 16;

	int row_start[4];
	int row_index[4];
	int temp;
	IntEdge_ up_edge, down_edge, left_edge, right_edge;
	IntEdge_ pline_max, pline_min;

	if (point[1].x != point[0].x)
	{
		up_edge.k = ((point[1].y - point[0].y)<<16) / (point[1].x - point[0].x);
		up_edge.b = (point[1].y<<16) - up_edge.k*point[1].x;
	}
	else
	{
		up_edge.k = 0;
		up_edge.b = 0;
	}
	if (point[2].x != point[3].x)
	{
		down_edge.k = ((point[2].y - point[3].y)<<16) / (point[2].x - point[3].x);
		down_edge.b = (point[2].y << 16) - down_edge.k*point[2].x;
	}
	else
	{
		down_edge.k = 0;
		down_edge.b = 0;
	}
	if (point[3].y != point[0].y)
	{
		left_edge.k = ((point[3].x - point[0].x) << 16) / (point[3].y - point[0].y);
		left_edge.b = (point[3].x << 16) - left_edge.k*point[3].y;
	}
	else
	{
		left_edge.k = 0;
		left_edge.b = 0;
	}
	if (point[2].y != point[1].y)
	{
		right_edge.k = ((point[2].x - point[1].x) << 16) / (point[2].y - point[1].y);
		right_edge.b = (point[2].x << 16) - right_edge.k*point[2].y;
	}
	else
	{
		right_edge.k = 0;
		right_edge.b = 0;
	}
	up_edge.type = 0;
	down_edge.type = 1;
	left_edge.type = 2;
	right_edge.type = 3;

	for (i = 0; i < 4; i++)
		row_start[i] = point[i].y;  //rowStart[i]Ϊ4�������y����
	row_index[0] = 0;
	row_index[1] = 1;
	row_index[2] = 2;
	row_index[3] = 3;
	//rowStart[i]���μ�¼4������y��С�����ֵ��rowIndex[i]���μ�¼y��С��������Ķ������
	for (i = 0; i < 3; i++)	//ð������-��С����    //rowStart����С�����������򣬶�Ӧ��rowIndex��¼ԭ��������
	{
		for (j = i + 1; j < 4; j++)
		{
			if (row_start[i] > row_start[j])
			{
				temp = row_start[i];
				row_start[i] = row_start[j];
				row_start[j] = temp;
				row_index[i] = j;
				row_index[j] = i;
			}
		}
	}

	if ((row_start[3] - row_start[0] + 1)>500)  //���곬�����鷶Χ
	{
		return -1;
	}

	int column_start;
	int column_end;
	if (row_index[0] == 0)	//���ϱ�     //��С��y�����Ͻǣ������Ϊpline_min���ϱ�Ϊpline_max
	{
		pline_max = up_edge;
		pline_min = left_edge;
	}
	else if (row_index[0] == 1)	//���ϱ�    //��С��y�����Ͻ�
	{
		pline_max = right_edge;
		pline_min = up_edge;
	}
	else
	{
		return -1;		//û���ҵ����ʵı�
	}

	for (i = row_start[0]; i < row_start[1]; i++)
	{
		if (pline_min.type < 2)
			column_start = ((i << 16) - pline_min.b) / pline_min.k;	//��pline_minΪ���±�
		else
			column_start = (pline_min.k*i + pline_min.b)>>16;

		if (pline_max.type < 2)
			column_end = ((i << 16) - pline_max.b) / pline_max.k;   //��pline_maxΪ���±�
		else
			column_end = (pline_max.k*i + pline_max.b)>>16;

		if (column_start >= 0 && column_end < width && i>0 && i < height)
		{
			coord[*val_rows][0] = i;
			coord[*val_rows][1] = column_start;
			coord[*val_rows][2] = column_end;
			(*val_rows)++;
		}
		else
		{
			(*del_rows)++;
		}
	}
	pline_max = right_edge;
	pline_min = left_edge;

	for (; i <= row_start[2]; i++)
	{
		column_start = (i*pline_min.k + pline_min.b)>>16;
		column_end = (i*pline_max.k + pline_max.b)>>16;

		if (column_start >= 0 && column_end < width && i>0 && i < height)
		{
			coord[*val_rows][0] = i;
			coord[*val_rows][1] = column_start;
			coord[*val_rows][2] = column_end;
			(*val_rows)++;
		}
		else
		{
			(*del_rows)++;
		}
	}

	if (row_index[3] == 3)		//����    //�������Ϊ��͵㣬pline_minΪ��ߣ�pline_maxΪ�ұ�
	{
		pline_max = down_edge;
		pline_min = left_edge;
	}
	else if (row_index[3] == 2)	//����
	{
		pline_max = right_edge;
		pline_min = down_edge;
	}
	else
	{
		return -1;
	}

	if (down_edge.k != 0)
	{
		for (; i <= row_start[3]; i++)
		{
			if (pline_min.type < 2)
				column_start = ((i << 16) - pline_min.b) / pline_min.k;	//���±�
			else
				column_start = (pline_min.k*i + pline_min.b)>>16;

			if (pline_max.type < 2)
				column_end = ((i << 16) - pline_max.b) / pline_max.k;
			else
				column_end = (pline_max.k*i + pline_max.b)>>16;

			if (column_start >= 0 && column_end < width && i>0 && i < height)
			{
				coord[*val_rows][0] = i;
				coord[*val_rows][1] = column_start;
				coord[*val_rows][2] = column_end;
				(*val_rows)++;
			}
			else
			{
				(*del_rows)++;
			}
		}
	}
}

//int getZoneCoordinates(int coord[][3], int* val_rows, int* del_rows, int width, int height, RotateZoneInfo_ *zone_info, int *_mat)
// {
//	*val_rows = 0;
//	*del_rows = 0;
//
//	int i, j;
//	int new_width = zone_info->width;       // ��ת��ͼ��Ŀ�Ⱥ͸߶�
//	int new_height = zone_info->height;
//	int y_start = zone_info->ystart;    //С�����ͼ����ʼ����
//	int x_start = zone_info->xstart;
//
//	IntPoint_ point[4];
//
//	i = y_start;
//	j = x_start;
//	point[0].x = (j * _mat[0] + i * _mat[1] + _mat[2] + 32768) >> 16;
//	point[0].y = (j * _mat[3] + i * _mat[4] + _mat[5] + 32768) >> 16;
//
//	i = y_start;
//	j = x_start + new_width - 1;
//	point[1].x = (j * _mat[0] + i * _mat[1] + _mat[2] + 32768) >> 16;
//	point[1].y = (j * _mat[3] + i * _mat[4] + _mat[5] + 32768) >> 16;
//
//	i = y_start + new_height - 1;
//	j = x_start + new_width - 1;
//	point[2].x = (j * _mat[0] + i * _mat[1] + _mat[2] + 32768) >> 16;
//	point[2].y = (j * _mat[3] + i * _mat[4] + _mat[5] + 32768) >> 16;
//
//	i = y_start + new_height - 1;
//	j = x_start;
//	point[3].x = (j * _mat[0] + i * _mat[1] + _mat[2] + 32768) >> 16;
//	point[3].y = (j * _mat[3] + i * _mat[4] + _mat[5] + 32768) >> 16;
//
//	int row_start[4];
//	int row_index[4];
//	int temp;
//	EdgeInfo_ up_edge_info, down_edge_info, left_edge_info, right_edge_info;
//	EdgeInfo_ pline_max, pline_min;
//
//	if (point[1].x != point[0].x)
//	{
//		up_edge_info.k = (double)(point[1].y - point[0].y) / (point[1].x - point[0].x);
//		up_edge_info.b = point[1].y - up_edge_info.k*point[1].x;
//	}
//	else
//	{
//		up_edge_info.k = 0;
//		up_edge_info.b = 0;
//	}
//	if (point[2].x != point[3].x)
//	{
//		down_edge_info.k = (double)(point[2].y - point[3].y) / (point[2].x - point[3].x);
//		down_edge_info.b = point[2].y - down_edge_info.k*point[2].x;
//	}
//	else
//	{
//		down_edge_info.k = 0;
//		down_edge_info.b = 0;
//	}
//	if (point[3].y != point[0].y)
//	{
//		left_edge_info.k = (double)(point[3].x - point[0].x) / (point[3].y - point[0].y);
//		left_edge_info.b = point[3].x - left_edge_info.k*point[3].y;
//	}
//	else
//	{
//		left_edge_info.k = 0;
//		left_edge_info.b = 0;
//	}
//	if (point[2].y != point[1].y)
//	{
//		right_edge_info.k = (double)(point[2].x - point[1].x) / (point[2].y - point[1].y);
//		right_edge_info.b = point[2].x - right_edge_info.k*point[2].y;
//	}
//	else
//	{
//		right_edge_info.k = 0;
//		right_edge_info.b = 0;
//	}
//	up_edge_info.type = 0;
//	down_edge_info.type = 1;
//	left_edge_info.type = 2;
//	right_edge_info.type = 3;
//
//	for (i = 0; i < 4; i++)
//		row_start[i] = point[i].y;  //rowStart[i]Ϊ4�������y����
//	row_index[0] = 0;
//	row_index[1] = 1;
//	row_index[2] = 2;
//	row_index[3] = 3;
//	//rowStart[i]���μ�¼4������y��С�����ֵ��rowIndex[i]���μ�¼y��С��������Ķ������
//	for (i = 0; i < 3; i++)	//ð������-��С����    //rowStart����С�����������򣬶�Ӧ��rowIndex��¼ԭ��������
//	{
//		for (j = i + 1; j < 4; j++)
//		{
//			if (row_start[i] > row_start[j])
//			{
//				temp = row_start[i];
//				row_start[i] = row_start[j];
//				row_start[j] = temp;
//				row_index[i] = j;
//				row_index[j] = i;
//			}
//		}
//	}
//
//	if ((row_start[3] - row_start[0] + 1)>500)  //���곬�����鷶Χ
//	{
//		return -1;
//	}
//
//	int column_start;
//	int column_end;
//	if (row_index[0] == 0)	//���ϱ�     //��С��y�����Ͻǣ������Ϊpline_min���ϱ�Ϊpline_max
//	{
//		pline_max = up_edge_info;
//		pline_min = left_edge_info;
//	}
//	else if (row_index[0] == 1)	//���ϱ�    //��С��y�����Ͻ�
//	{
//		pline_max = right_edge_info;
//		pline_min = up_edge_info;
//	}
//	else
//	{
//		return -1;		//û���ҵ����ʵı�
//	}
//
//	for (i = row_start[0]; i < row_start[1]; i++)
//	{
//		if (pline_min.type < 2)
//			column_start = (i - pline_min.b) / pline_min.k;	//��pline_minΪ���±�
//		else
//			column_start = pline_min.k*i + pline_min.b;
//
//		if (pline_max.type < 2)
//			column_end = (i - pline_max.b) / pline_max.k;   //��pline_maxΪ���±�
//		else
//			column_end = pline_max.k*i + pline_max.b;
//
//		if (column_start >= 0 && column_end < width && i>0 && i < height)
//		{
//			coord[*val_rows][0] = i;
//			coord[*val_rows][1] = column_start;
//			coord[*val_rows][2] = column_end;
//			(*val_rows)++;
//		}
//		else
//		{
//			(*del_rows)++;
//		}
//	}
//	pline_max = right_edge_info;
//	pline_min = left_edge_info;
//
//	for (; i <= row_start[2]; i++)
//	{
//		column_start = i*pline_min.k + pline_min.b;
//		column_end = i*pline_max.k + pline_max.b;
//
//		if (column_start >= 0 && column_end < width && i>0 && i < height)
//		{
//			coord[*val_rows][0] = i;
//			coord[*val_rows][1] = column_start;
//			coord[*val_rows][2] = column_end;
//			(*val_rows)++;
//		}
//		else
//		{
//			(*del_rows)++;
//		}
//	}
//
//	if (row_index[3] == 3)		//����    //�������Ϊ��͵㣬pline_minΪ��ߣ�pline_maxΪ�ұ�
//	{
//		pline_max = down_edge_info;
//		pline_min = left_edge_info;
//	}
//	else if (row_index[3] == 2)	//����
//	{
//		pline_max = right_edge_info;
//		pline_min = down_edge_info;
//	}
//	else
//	{
//		return -1;
//	}
//
//	if (down_edge_info.k != 0)
//	{
//		for (; i <= row_start[3]; i++)
//		{
//			if (pline_min.type < 2)
//				column_start = (i - pline_min.b) / pline_min.k;	//���±�
//			else
//				column_start = pline_min.k*i + pline_min.b;
//
//			if (pline_max.type < 2)
//				column_end = (i - pline_max.b) / pline_max.k;
//			else
//				column_end = pline_max.k*i + pline_max.b;
//
//			if (column_start >= 0 && column_end < width && i>0 && i < height)
//			{
//				coord[*val_rows][0] = i;
//				coord[*val_rows][1] = column_start;
//				coord[*val_rows][2] = column_end;
//				(*val_rows)++;
//			}
//			else
//			{
//				(*del_rows)++;
//			}
//		}
//	}
//}

#ifdef SAVE_CORNER_IMG
/*---------------------------------------------------- - */
//��ǳ�Ǯ��4����Ե�ߡ���Ե��ͽǵ�
//*_src_imgΪ����ͼ��, *__corner_infoΪ�ǵ���Ϣ�ṹ��, _row_pointΪ��ߵ���, _vert_pointΪ�ݱߵ���
void DrawImageEdgeCorner(IplImage_ *_src_img, CornerCorrectInfo_ *_corner_info, int _row_point_num, int _vert_point_num, char *file_name)
{
	U8 *p_image = (U8*)malloc(_src_img->width*_src_img->height);

	////����ͼ������
	for (int i = 0; i < _src_img->height; i++)
	{
		for (int j = 0; j < _src_img->width; j++)
		{
			p_image[i*_src_img->width + j] = _src_img->image_data[i*_src_img->width_step + j];
		}
	}
	//memcpy(p_image,_src_img->imageData,_src_img->height*_src_img->width);
	int i, j, k;

	//����4����Ե�ߣ���Ϊ��ɫ
	//���±�Ե��
	for (i = 0; i<_src_img->width; i++)
	{
		j = _corner_info->up_edge_info.k*i + _corner_info->up_edge_info.b;
		k = _corner_info->down_edge_info.k*i + _corner_info->down_edge_info.b;
		if (j >= 0 && j<_src_img->height)
		{
			*(unsigned char*)(p_image + j*_src_img->width + i) = 200;//r
		}

		if (k >= 0 && k<_src_img->height)
		{
			*(unsigned char*)(p_image + k*_src_img->width + i) = 200;//r
		}
	}

	//���ұ�Ե��
	for (i = 0; i<_src_img->height; i++)
	{
		j = _corner_info->left_edge_info.k*i + _corner_info->left_edge_info.b;
		k = _corner_info->right_edge_info.k*i + _corner_info->right_edge_info.b;
		if (j >= 0 && j<_src_img->width)
		{
			*(unsigned char*)(p_image + i*_src_img->width + j) = 200;//r
		}

		if (k >= 0 && k<_src_img->width)
		{
			*(unsigned char*)(p_image + i*_src_img->width + k) = 200;//r
		}
	}

	SaveGrayBitmap(file_name, p_image, _src_img->width, _src_img->height);   //���������irͼ��ͼ

	free(p_image);
}
#endif