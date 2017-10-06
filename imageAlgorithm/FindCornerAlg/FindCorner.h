/***************************************************************
* Copyright (C) 2011, Wuhan University
* All rights reserved.
*
* 文件名：	FindCorner.h
* 功能描述：角点校正模块
*
* 当前版本：1.0
* 作    者：江燕婷
* 完成日期：2016年9月12日
*
***************************************************************/
#ifndef FINDCORNER_H
#define FINDCORNER_H

#include "Recognition.h"
#include "FindCornerExt.h"

#define ROW_2_POWER  4              //横边找点间隔-指数（内部）
#define VERT_2_POWER  3             //纵边找点间隔-指数（内部）
#define IMAGE_IGNORE_POINT  32      //找点忽略的边缘列数（内部）
#define VERT_HALF_2_POWER  2             //纵边找点间隔-指数（内部）
#define IMAGE_WIDTH_NEW	720   //源图（单通道）宽度（内部）
#define IMAGE_WIDTH_OLD	688
#define EDGE_CONFIDENCE_THRED   40    //修正边时的置信度阈值（内部）
#define HALF_UP     0         //半张钱保留上边（内部）
#define HALF_DOWN   1         // 半张钱保留下边（内部）

#define U8 unsigned char
#define U32 unsigned int
#define u8 unsigned char

//---------------函数声明

//得到另一面的仿射坐标
//_other_tri[]为另一面的坐标, *_corner_info为角点信息结构体, mapping_offset为CIS偏移
int GetOtherCoordinate(IntPoint_ _other_tri[], CornerCorrectInfo_ *_corner_info, CoordinatesInfo_ *_coordinates1);

//得到仿射变换用的坐标
//_dst_tri[]为校正后坐标,*_corner_info为角点信息结构体, _affine_width为仿射后宽度, _affine_height为仿射后高度, _src_tri[]为钱的坐标,
void CalcAffineCoordinate(IntPoint_ _dst_tri[], int _width, int _height, int _affine_width, int _affine_height, CoordinatesInfo_ *_coordinates, IntPoint_ _src_tri[], int _tri_type);

//根据两组坐标获得仿射变换参数
//_src_tri[]为校正前坐标, _dst_tri[]为校正后坐标, _affine_operator_trans[]为仿射变换参数
void GetAffineMat(IntPoint_ _src_tri[], IntPoint_ _dst_tri[], int _affine_operator_trans[]);

//解决CIS积灰问题，清除背景灰条
// *_img为源图像
int gClearDirt(IplImage_ *_img);

//找到钱的四条边上的点
//*_img_breakage_check为图像破损检测开关, *_src_img为输入图像, *__corner_info为角点信息结构体, _thred为背景阈值, *_row_point_num为横边点数, *_vert_point_num为纵边点数, _last_status为图像上次的状态
int GetMoneyEdgePoint(int *_img_breakage_check, IplImage_ *_src_img, CornerCorrectInfo_ *_corner_info, int _thred, int *_row_point_num, int *_vert_point_num, int _last_status);

int judgeTwoMoney(int *_pup, int *_pdown, int _cnt,int _height);

//确定CIS左右两边需要忽略的列
//*_left是左边忽略的列，*_right是右边忽略的列，*_src_img为输入图像
int IgnoreCISCol(int *_left, int *_right, IplImage_ *_src_img);

void GetTriType(CornerCorrectInfo_ *_corner_info, int *_tri_type);
//找到钱的四个角点坐标
//*_img_breakage_check为图像破损检测开关, *_src为输入图像, *_corner_info为角点信息结构体, _last_status为图像上次的状态
int FindCornerAndAngle(int *_img_breakage_check, unsigned char *_src, CornerCorrectInfo_ *_corner_info, int _last_status);

//根据一组点由最小二乘法多次拟合直线，得到直线的k,b,degree,confidence等值
//_thred为背景阈值, *_x为点的x坐标, *_y_src为点的y坐标, _cnt为点数, _type为边的类型, *edge_line_info为边结构体, *_src_img为输入图像
int FittingEdge(int _thred, int *_x, int *_y_src, int _cnt, int _type, EdgeInfo_ *_edge_line_info, IplImage_ *_src_img);

//根据置信度较高的边，修正另一条边
//*_x为点的x坐标, *_y为点的y坐标, _cnt为点数, _type为边的类型, *EdgeLineInfo为待修正边, *ReferEdgeLineInfo为正确边
int FittingEdgeRefer(int *_x, int *_y, int _cnt, int _type, EdgeInfo_ *_edge_line_info, EdgeInfo_ *_refer_edge_line_info);

//得到四条边的置信度和角度
int GetConfidence(int _x1, int _y1, int _x2, int _y2, int _cnt, int *_edge_x, int *_edge_point, EdgeInfo_ *_edge_info, CornerCorrectInfo_ *_corner_info, int _edge_type);

//得到边的夹角
int GetAngle(EdgeInfo_ *_dege_a, EdgeInfo_ *_edge_b, int _point);

//计算折角的面积
int GetAreaOfBreak(CornerCorrectInfo_ *_corner_info, int _row_cnt, int _ver_cnt, int *_edge_x, int *_edge_x_point, int *_edge_y, int *_edge_y_point, int _point_type);

//根据三点和角度计算面积
int CalculateArea(CornerCorrectInfo_ *_corner_info, IntPoint_ _point1, IntPoint_ _point2, IntPoint_ _point3, int _point_type);

//删除在上下边上的伪左右边缘点
//*_x为点的x坐标, *_y为点的y坐标, _cnt为点数, *_edge_line_info为边信息结构体
void DelErrPoint(int *_x, int *_y, int _cnt, EdgeInfo_ *_edge_line_info);

//高斯消元法解线性方程组求得仿射变换参数
int gaus(double _a[], double _b[], int _n);

//计算钱的实际长度
//*_src为输入图像, *_corner_info为角点信息结构体, *_coordinates为图像中纸币的角点坐标
int CalcRealLengthOfMoney(unsigned char *_src, CornerCorrectInfo_ *_corner_info);
/*---------------------------------------------------
*半张钱的角点检测
*----------------------------------------------------*/
//找到钱的四个角点坐标
//*_src为输入图像, *_corner_info为角点信息结构体, _last_status为图像上次的状态
int FindHalfMoneyCorner(unsigned char *_src, CornerCorrectInfo_ *_corner_info, int _last_status);

//找到半张钱的三条边上的点
//*_src_img为输入图像, *__corner_info为角点信息结构体, _thred为背景阈值, *_row_point_num为横边点数, *_vert_point_num为纵边点数, _last_status为图像上次的状态
int GetHalfMoneyEdgePoint(IplImage_ *_src_img, CornerCorrectInfo_ *_corner_info, int _thred, int *_row_point_num, int *_vert_point_num, int _last_status);

//计算钱的实际长度
//*_src为输入图像, *_corner_info为角点信息结构体, *_coordinates为图像中纸币的角点坐标
int CalcHalfRealLengthOfMoney(unsigned char *_src, CornerCorrectInfo_ *_corner_info, CoordinatesInfo_ *_coordinates);

//计算钱的一条边的实际长度并返回
//_point0, _point1为两个点坐标, _height为图像高度, *_code_data为码盘值数据
float CalcEdgeRealLength(IntPoint_ _point0, IntPoint_ _point1, int _height, unsigned char *_code_data);

//获取两点间纵向码盘长度
//_up为较小的y坐标, _down为较大的y坐标, *_code_data为码盘值数据
float GetCodeLength(int _up, int _down, unsigned char *_code_data);

//得到半张图的仿射变换用的坐标
//_dst_tri[]为校正后坐标,*_corner_info为角点信息结构体, _affine_width为仿射后宽度, _affine_height为仿射后高度, _src_tri[]为钱的坐标,
void CalcHalfAffineCoordinate(IntPoint_ _dst_tri[], CornerCorrectInfo_ *_corner_info, int _affine_width, int _affine_height, IntPoint_ _src_tri[]);

//得到半张钱另一面的仿射坐标
//_other_tri[]为另一面的坐标, *_corner_info为角点信息结构体, *_coordinates1为另一面图像中纸币角点坐标
int GetHalfOtherCoordinate(IntPoint_ _other_tri[], CornerCorrectInfo_ *_corner_info, CoordinatesInfo_ *_coordinates1);

#ifdef SAVE_CORNER_IMG
//标记出钱的4条边缘线、边缘点和角点
//*_src_img为输入图像, *__corner_info为角点信息结构体, _row_point为横边点数, _vert_point为纵边点数
void DrawImageEdgeCorner(IplImage_ *_src_img, CornerCorrectInfo_ *_corner_info, int _row_point_num, int _vert_point_num, char *file_name);
#endif

#endif