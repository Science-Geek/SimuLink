/***************************************************************
* Copyright (C) 2011, Wuhan University
* All rights reserved.
*
* �ļ�����	FindCorner.h
* �����������ǵ�У��ģ��
*
* ��ǰ�汾��1.0
* ��    �ߣ�������
* ������ڣ�2016��9��12��
*
***************************************************************/
#ifndef FINDCORNER_H
#define FINDCORNER_H

#include "Recognition.h"
#include "FindCornerExt.h"

#define ROW_2_POWER  4              //����ҵ���-ָ�����ڲ���
#define VERT_2_POWER  3             //�ݱ��ҵ���-ָ�����ڲ���
#define IMAGE_IGNORE_POINT  32      //�ҵ���Եı�Ե�������ڲ���
#define VERT_HALF_2_POWER  2             //�ݱ��ҵ���-ָ�����ڲ���
#define IMAGE_WIDTH_NEW	720   //Դͼ����ͨ������ȣ��ڲ���
#define IMAGE_WIDTH_OLD	688
#define EDGE_CONFIDENCE_THRED   40    //������ʱ�����Ŷ���ֵ���ڲ���
#define HALF_UP     0         //����Ǯ�����ϱߣ��ڲ���
#define HALF_DOWN   1         // ����Ǯ�����±ߣ��ڲ���

#define U8 unsigned char
#define U32 unsigned int
#define u8 unsigned char

//---------------��������

//�õ���һ��ķ�������
//_other_tri[]Ϊ��һ�������, *_corner_infoΪ�ǵ���Ϣ�ṹ��, mapping_offsetΪCISƫ��
int GetOtherCoordinate(IntPoint_ _other_tri[], CornerCorrectInfo_ *_corner_info, CoordinatesInfo_ *_coordinates1);

//�õ�����任�õ�����
//_dst_tri[]ΪУ��������,*_corner_infoΪ�ǵ���Ϣ�ṹ��, _affine_widthΪ�������, _affine_heightΪ�����߶�, _src_tri[]ΪǮ������,
void CalcAffineCoordinate(IntPoint_ _dst_tri[], int _width, int _height, int _affine_width, int _affine_height, CoordinatesInfo_ *_coordinates, IntPoint_ _src_tri[], int _tri_type);

//�������������÷���任����
//_src_tri[]ΪУ��ǰ����, _dst_tri[]ΪУ��������, _affine_operator_trans[]Ϊ����任����
void GetAffineMat(IntPoint_ _src_tri[], IntPoint_ _dst_tri[], int _affine_operator_trans[]);

//���CIS�������⣬�����������
// *_imgΪԴͼ��
int gClearDirt(IplImage_ *_img);

//�ҵ�Ǯ���������ϵĵ�
//*_img_breakage_checkΪͼ�������⿪��, *_src_imgΪ����ͼ��, *__corner_infoΪ�ǵ���Ϣ�ṹ��, _thredΪ������ֵ, *_row_point_numΪ��ߵ���, *_vert_point_numΪ�ݱߵ���, _last_statusΪͼ���ϴε�״̬
int GetMoneyEdgePoint(int *_img_breakage_check, IplImage_ *_src_img, CornerCorrectInfo_ *_corner_info, int _thred, int *_row_point_num, int *_vert_point_num, int _last_status);

int judgeTwoMoney(int *_pup, int *_pdown, int _cnt,int _height);

//ȷ��CIS����������Ҫ���Ե���
//*_left����ߺ��Ե��У�*_right���ұߺ��Ե��У�*_src_imgΪ����ͼ��
int IgnoreCISCol(int *_left, int *_right, IplImage_ *_src_img);

void GetTriType(CornerCorrectInfo_ *_corner_info, int *_tri_type);
//�ҵ�Ǯ���ĸ��ǵ�����
//*_img_breakage_checkΪͼ�������⿪��, *_srcΪ����ͼ��, *_corner_infoΪ�ǵ���Ϣ�ṹ��, _last_statusΪͼ���ϴε�״̬
int FindCornerAndAngle(int *_img_breakage_check, unsigned char *_src, CornerCorrectInfo_ *_corner_info, int _last_status);

//����һ�������С���˷�������ֱ�ߣ��õ�ֱ�ߵ�k,b,degree,confidence��ֵ
//_thredΪ������ֵ, *_xΪ���x����, *_y_srcΪ���y����, _cntΪ����, _typeΪ�ߵ�����, *edge_line_infoΪ�߽ṹ��, *_src_imgΪ����ͼ��
int FittingEdge(int _thred, int *_x, int *_y_src, int _cnt, int _type, EdgeInfo_ *_edge_line_info, IplImage_ *_src_img);

//�������ŶȽϸߵıߣ�������һ����
//*_xΪ���x����, *_yΪ���y����, _cntΪ����, _typeΪ�ߵ�����, *EdgeLineInfoΪ��������, *ReferEdgeLineInfoΪ��ȷ��
int FittingEdgeRefer(int *_x, int *_y, int _cnt, int _type, EdgeInfo_ *_edge_line_info, EdgeInfo_ *_refer_edge_line_info);

//�õ������ߵ����ŶȺͽǶ�
int GetConfidence(int _x1, int _y1, int _x2, int _y2, int _cnt, int *_edge_x, int *_edge_point, EdgeInfo_ *_edge_info, CornerCorrectInfo_ *_corner_info, int _edge_type);

//�õ��ߵļн�
int GetAngle(EdgeInfo_ *_dege_a, EdgeInfo_ *_edge_b, int _point);

//�����۽ǵ����
int GetAreaOfBreak(CornerCorrectInfo_ *_corner_info, int _row_cnt, int _ver_cnt, int *_edge_x, int *_edge_x_point, int *_edge_y, int *_edge_y_point, int _point_type);

//��������ͽǶȼ������
int CalculateArea(CornerCorrectInfo_ *_corner_info, IntPoint_ _point1, IntPoint_ _point2, IntPoint_ _point3, int _point_type);

//ɾ�������±��ϵ�α���ұ�Ե��
//*_xΪ���x����, *_yΪ���y����, _cntΪ����, *_edge_line_infoΪ����Ϣ�ṹ��
void DelErrPoint(int *_x, int *_y, int _cnt, EdgeInfo_ *_edge_line_info);

//��˹��Ԫ�������Է�������÷���任����
int gaus(double _a[], double _b[], int _n);

//����Ǯ��ʵ�ʳ���
//*_srcΪ����ͼ��, *_corner_infoΪ�ǵ���Ϣ�ṹ��, *_coordinatesΪͼ����ֽ�ҵĽǵ�����
int CalcRealLengthOfMoney(unsigned char *_src, CornerCorrectInfo_ *_corner_info);
/*---------------------------------------------------
*����Ǯ�Ľǵ���
*----------------------------------------------------*/
//�ҵ�Ǯ���ĸ��ǵ�����
//*_srcΪ����ͼ��, *_corner_infoΪ�ǵ���Ϣ�ṹ��, _last_statusΪͼ���ϴε�״̬
int FindHalfMoneyCorner(unsigned char *_src, CornerCorrectInfo_ *_corner_info, int _last_status);

//�ҵ�����Ǯ���������ϵĵ�
//*_src_imgΪ����ͼ��, *__corner_infoΪ�ǵ���Ϣ�ṹ��, _thredΪ������ֵ, *_row_point_numΪ��ߵ���, *_vert_point_numΪ�ݱߵ���, _last_statusΪͼ���ϴε�״̬
int GetHalfMoneyEdgePoint(IplImage_ *_src_img, CornerCorrectInfo_ *_corner_info, int _thred, int *_row_point_num, int *_vert_point_num, int _last_status);

//����Ǯ��ʵ�ʳ���
//*_srcΪ����ͼ��, *_corner_infoΪ�ǵ���Ϣ�ṹ��, *_coordinatesΪͼ����ֽ�ҵĽǵ�����
int CalcHalfRealLengthOfMoney(unsigned char *_src, CornerCorrectInfo_ *_corner_info, CoordinatesInfo_ *_coordinates);

//����Ǯ��һ���ߵ�ʵ�ʳ��Ȳ�����
//_point0, _point1Ϊ����������, _heightΪͼ��߶�, *_code_dataΪ����ֵ����
float CalcEdgeRealLength(IntPoint_ _point0, IntPoint_ _point1, int _height, unsigned char *_code_data);

//��ȡ������������̳���
//_upΪ��С��y����, _downΪ�ϴ��y����, *_code_dataΪ����ֵ����
float GetCodeLength(int _up, int _down, unsigned char *_code_data);

//�õ�����ͼ�ķ���任�õ�����
//_dst_tri[]ΪУ��������,*_corner_infoΪ�ǵ���Ϣ�ṹ��, _affine_widthΪ�������, _affine_heightΪ�����߶�, _src_tri[]ΪǮ������,
void CalcHalfAffineCoordinate(IntPoint_ _dst_tri[], CornerCorrectInfo_ *_corner_info, int _affine_width, int _affine_height, IntPoint_ _src_tri[]);

//�õ�����Ǯ��һ��ķ�������
//_other_tri[]Ϊ��һ�������, *_corner_infoΪ�ǵ���Ϣ�ṹ��, *_coordinates1Ϊ��һ��ͼ����ֽ�ҽǵ�����
int GetHalfOtherCoordinate(IntPoint_ _other_tri[], CornerCorrectInfo_ *_corner_info, CoordinatesInfo_ *_coordinates1);

#ifdef SAVE_CORNER_IMG
//��ǳ�Ǯ��4����Ե�ߡ���Ե��ͽǵ�
//*_src_imgΪ����ͼ��, *__corner_infoΪ�ǵ���Ϣ�ṹ��, _row_pointΪ��ߵ���, _vert_pointΪ�ݱߵ���
void DrawImageEdgeCorner(IplImage_ *_src_img, CornerCorrectInfo_ *_corner_info, int _row_point_num, int _vert_point_num, char *file_name);
#endif

#endif