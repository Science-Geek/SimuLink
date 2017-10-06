/***************************************************************

*
***************************************************************/
#ifndef LOCATECORNEREXT_H
#define LOCATECORNEREXT_H


#define ERR_CORNER				        -50	
#define ERR_CORNER_IMGCIS			    ERR_CORNER-1	//CIS有问题
#define ERR_CORNER_IMGSOURCE			ERR_CORNER-2	//图像源有问题
#define ERR_CORNER_EDGE_DEGREE			ERR_CORNER-3	//角度过大
#define ERR_CORNER_PARALLEL_DEGREE		ERR_CORNER-4	//通过判断是否平行,边线角度不对
#define ERR_COORNER_BIG_ANGLE           ERR_CORNER-5    //大折角
#define ERR_TWO_IMG                     ERR_CORNER-6

//边信息结构体
typedef struct EdgeInfo
{
	double	k;	//边的参数
	double	b;
	int     degree;   //边的倾角
	int		confidence;   //边的置信度
	int     type;   //边的类型
}EdgeInfo_;

//四个角的角度
typedef struct Angle
{
	int angle_up_left;
	int angle_up_right;
	int angle_down_left;
	int angle_down_right;
}Angle_;

//边的特征
typedef struct EdgeFeature
{
	float confidence;
	float var;           //边点的方差
	float mean;          //边点的均值
	int deviation;     //边偏移的点数

}EdgeFeature_;

typedef struct
{
	int		height;
	int		width;
	int 		width_step;
	unsigned char *image_data;
	CoordinatesInfo_	coordinates;        // 单通道图中纸币的四个角坐标
	EdgeInfo_			UpEdgeInfo;
	EdgeInfo_			DownEdgeInfo;
	EdgeInfo_			LeftEdgeInfo;
	EdgeInfo_			RightEdgeInfo;

	int type;
}IR_PicInfo_;

//角点信息结构体
typedef struct CornerCorrectInfo
{
	int width;         //单通道图像宽
	int width_step;     // 全图宽
	int height;        //全图高

	CoordinatesInfo_	coordinates;        // 单通道图中纸币的四个角坐标
	EdgeInfo_			up_edge_info;       // 纸币的上边信息
	EdgeInfo_		    down_edge_info;     // 纸币的下边信息
	EdgeInfo_			left_edge_info;     // 纸币的左边信息
	EdgeInfo_			right_edge_info;    // 纸币的右边信息
	int *edge_x;
	int *edge_y;
	int *edge_x_up;      //上边的点的y坐标
	int *edge_x_down;
	int *edge_x_left;    //左边的点的x坐标
	int *edge_x_right;

	float edge_real_len[4];       //4条边的实际边长

	//图像特征
	Angle_ angle;
	EdgeFeature_ edge_feature[4];
	float areaSize[4];  //对应四个折角的面积
	int missing_info;
}CornerCorrectInfo_;

//边信息结构体
typedef struct IntEdge
{
	long long	k;	//边的参数
	long long	b;
	int     type;   //边的类型
}IntEdge_;

//修改找角点的背景阈值函数
int SetCornerBackgroundThred(int _thred);

int ValueCheckByLength(PicDataInfo_ *_pic_data_info, int confidence_up, int confidence_down);

int GetAffineCorner(PicDataInfo_ *_pic_data_info, struct CornerCorrectInfo *_corner_info, IR_PicInfo_*Up_IR_info, IR_PicInfo_*Down_IR_info, int _tri_type[]);

//角点校正模块：确定纸币的四个角点坐标，计算仿射变换参数
//_up_img为上面图像源；_down_img为下面图像源；*_pic_data_info为图像信息结构体
int GetAffineParameter(PicDataInfo_ *_pic_data_info, int _tri_type[]);

int GetAffineImg(IplImage_ *money_grey_up, IplImage_ *money_grey_bottom, PicDataInfo_ *_pic_data_info);
//获得仿射后的钱的ir图和grey图
//获得仿射后的钱的ir图和grey图
//*_src_ir_img为原始ir图, *p_src_grey为原始grey指针, *_new_ir_img为新的ir图, *_new_grey_img为新的grey图, *zone_info为区域信息, *Mat为仿射参数
int WarpAffineMoneyInt(IplImage_ *_src_ir_img, u8 *p_src_grey, IplImage_ *_new_grey_img, ZoneInfo_ *zone_info, int *Mat);

//区域截图函数
int WarpAffineZoneInt(IplImage_ *_new_img, IplImage_ *_src_img, RotateZoneInfo_ *zone_info, int *_mat);

//带抽样的区域截图函数
int WarpAffineZoneInt_Samp(IplImage_ *_new_ir_img, IplImage_ *_new_grey_img, IplImage_ *_src_ir_img, IplImage_ *_src_grey_img, RotateZoneInfo_ *zone_info, int *_mat, int _sample_ena, int _row_samp_rate, int _col_samp_rate);

//带抽样的区域截图函数-单通道
int WarpAffineZoneInt_Samp_One(IplImage_ *_new_img, IplImage_ *_src_img, RotateZoneInfo_ *zone_info, int *_mat, int _sample_ena, int _row_samp_rate, int _col_samp_rate);

int getZoneCoordinates(int coord[][3], int* val_rows, int* del_rows, int width, int height, RotateZoneInfo_ *zone_info, int *_mat);

#endif