/***************************************************************
* Copyright (C) 2011, Wuhan University
* All rights reserved.
*
* 文件名：	Recognition.h
* 功能描述：
*
* 当前版本：1.0
* 作    者：
* 完成日期：2016年9月18日
*
***************************************************************/
#ifndef Recognition_H
#define Recognition_H
#include <QString.h>
#include <QStringList.h>
#define U8 unsigned char
#define U32 unsigned int

#define CIS_B               0
#define CIS_T               1

/*************************** 测试平台 ****************************/
#define MODULE_TEST                //模块测试or整体测试

#define LENGTH_CHECK_ONOFF         //长度判面额开关
#define VVD_CHECK_ONOFF            //面额版本方向识别开关
#define CashIDOCR_CHECK_ONOFF      //冠字号检测开关
#define MULTISPECTRAL_CHECK_ONOFF  //多光谱检测开关
#define SPLICING_CHECK_ONOFF       //拼接币检测开关
#define WRITING_CHECK_ONOFF        //反宣币检测开关

#define SAVE_IMG               //原图存图开关
//#define SAVE_VVD_IMG           //版本面额方向存图开关
//#define SAVE_CashIDOCR_IMG     //冠字号存图开关
//#define SAVE_MULTISPECTRAL_IMG //多光谱存图开关
//#define SAVE_SPLICING_IMG      //拼接币存图开关
//#define SAVE_WRITING_IMG       //反宣币存图开关

//#define SAVE_CORNER_IMG    //角点标记存图开关
//#define SAVE_SPLICE_BIN
/******************************************鉴伪部分定接口所用（以下）***************************************/
#define RMB100 0x00000000    //面额编号
#define RMB50  0x00010000
#define RMB20  0x00020000
#define RMB10  0x00030000
#define RMB5   0x00040000
#define RMB1   0x00050000
#define VERSION15    0x00000000    //版本编号
#define VERSION05    0x00001000
#define VERSION99    0x00002000
#define LEVEL0      0x00000000 //灵敏度（LEVEL0表示区域关闭）
#define LEVEL1      0x00000001
#define LEVEL2      0x00000002
#define LEVEL3      0x00000003
#define LEVEL4      0x00000004
#define LEVEL5      0x00000005
#define LEVEL6      0x00000006
#define LEVEL7      0x00000007
#define LEVEL8      0x00000008
#define LEVEL9      0x00000009
#define NO_SAMPLE   0x00000000   //是否采样
#define SAMPLE      0x00010000   
#define ROW_ONE     0x00000100   //采样率
#define ROW_TWO     0x00000200
#define ROW_THREE	0x00000300
#define ROW_FOUR	0x00000400
#define COL_ONE     0x00000001
#define COL_TWO     0x00000002
#define COL_THREE	0x00000003
#define COL_FOUR	0x00000004
/******************************************鉴伪部分定接口所用（以上）***************************************/


//图像大小结构体
typedef struct tagImgSizeInfo
{
	int   chnl_num;
	int	width;
	int	height;
	int 	width_step;
	int	channel_offset;
	tagImgSizeInfo()
	{
		width = 0;
		height = 0;
		width_step = 0;
	}
}ImgSizeInfo;

/************************共用（江+张+林+李）**********************/
#define BEGIN_ACTION(msg) printf(msg)
//图像数组结构体
typedef struct IplImage_
{
	int width;
	int height;
	int width_step;
	unsigned char *image_data;
	int offset_r;
	int offset_g;
	int offset_b;
	int offset_ir;
	int offset_tr;
	int offset_oir;
	int valid;
}IplImage_;

int loadBitmapFromFile(const char *filePath, U8 **bits, ImgSizeInfo* _imgsize);    //修改过，去掉全局变量
int CreateImage(IplImage_ *_img, int _width, int _height, U8 *_image_data);
void ReleaseImage(IplImage_ *_img);
void SaveGrayBitmap(const char *fileName, const U8 *imageData, int width, int height);


/******************************(江+张）****************************/
#define u8 unsigned char
#define WHOLE_IMAGE    1        //整张钱
#define HALF_IMAGE     0        //半张钱
#define TYPE_ROTATE_LINE    0   //线性插值
#define TYPE_ROTATE_LINEAR  1   //双线性插值

//仅江
#define DIRECTION_LEFT		0   //正图，冠字号在左边
#define DIRECTION_RIGHT		1   //倒图，冠字号在右边
#define DIRECTION_SAME		2   //不管

//坐标结构体
typedef struct CoordinatesInfo
{
	int x[4];
	int y[4];
}CoordinatesInfo_;

typedef struct
{
	int xstart;
	int ystart;
	int	width;
	int height;
	int width_step;
	int rotate_angle;			//0.5度为单位
	int	direction;				//左右，由于要算重心，截图要方向一致
	int channel;				//0y,1u
	int type;					//双线性1，线性0
	int mean;
}RotateZoneInfo_;

//20161129  FSN所用结构体  Fanyihua
typedef struct ID_cropImgInfo_
{
	int xstart;
	int ystart;
	int	width;
	int height;
	int width_step;
	int imgType;
	unsigned char* grayData;
	unsigned char* bitData;
	unsigned char* saveData;
	int length;
	unsigned short SynNum;
}ID_cropImgInfo_;

/*************清分结构体****************/
typedef struct QFInfo_
{
	float score;   //清分机得分
	float score_ir; //IR清分得分
	int level; //清分机等级
	int thred1;
	int thred2;
	//int dct_score; //DCT得分（预留）
}QFInfo_;

typedef struct BreakageTypeResult_   //破损结果
{
	int result;
	float lostarea_rate;
}BreakageTypeResult;

typedef struct PicDataInfo
{
	int		file_num;
	int		value;				//w
	int		cash_type;
	int		version;			//
	int		direction;			//

	int		last_error;
	int		channle_num;	    //w
	//int		ID_err;				//w

	int		height;
	int		width;
	int 	width_step;
	int affine_width;   // 校正后宽度
	int affine_height;  // 校正后高度
	int mapping_offset; // 上面图与下面图的偏移
	int img_type;

	float edge_real_len[4];       //4条边的实际边长
	int missing_info[2];   //图像缺失信息  -1无缺失，0-3缺失方向

	CoordinatesInfo_	coordinates_up;
	CoordinatesInfo_	coordinates_bottom;
	int	affine_operator_trans_up[6];// 上面图仿射参数左移16位
	int	affine_operator_trans_bottom[6];// 下面图仿射参数右移16位

	int img_breakage_check;  //图像破损检测开关
	BreakageTypeResult BreakResult[4];

	unsigned char *up_data;  //上面图首地址
	unsigned char *bottom_data;
	int		IR_data_offset;		//偏移都是从该数据其实地址开始计算
	int		IR_img_offset[2];    //IR通道图偏移
	int		TR_img_offset[2];    //投射通道图偏移
	int		UV_img_offset;
	int		R_img_offset;
	int		G_img_offset;
	int		B_img_offset;

	unsigned int all_wheels[384 * 6];

	char	ID[16];				//w
	char	ID_Row[16];
	int		ID_Err;				//w
	//冠字号201611.08
	RotateZoneInfo_		ID_ZONE;
	RotateZoneInfo_		ID_ZONE_VERTICAL;
	RotateZoneInfo_		IR_PART;
	RotateZoneInfo_		GRAY_PART;
	ID_cropImgInfo_		ID_Crop;
	int 	jiabi;

	/*********清分数组***********/
	QFInfo_             QF_Info;
}PicDataInfo_;

typedef struct FloatPoint //浮点坐标结构体
{
	float x; /* X坐标, 通常以0为基点*/
	float y; /* Y坐标, 通常以0为基点*/
}FloatPoint_;

typedef struct IntPoint //整形坐标结构体
{
	int x; /* X坐标, 通常以0为基点*/
	int y; /* Y坐标, 通常以0为基点*/
}IntPoint_;

typedef struct ZoneInfo
{
	int xstart;
	int ystart;
	int	width;
	int height;
	//int width_step;
	int type;					//双线性1，线性0

	//冠字号201611.08
	int		minx;	//w
	int		maxx;	//w
	int		miny;
	int		maxy;
	int		angle;
	int		zone;
	int		mean;
	int		Beijing;
}ZoneInfo_;

enum CornerName
{
	CORNE_DOWN_LEFT = 0,
	CORNE_DOWN_RIGHT,
	CORNE_UP_RIGHT,
	CORNER_UP_LEFT
};
enum EdgeName
{
	EDGE_DOWN = 0,
	EDGE_UP,
	EDGE_RIGHT,
	EDGE_LEFT
};

int ImgDataInit(unsigned char* _img_up, unsigned char* _img_bottom, PicDataInfo_ *pic_data_info, int _cash_type, ImgSizeInfo* _img_size);



//void Image_Alg_Main(LPCTSTR _filePath, PicDataInfo_ *pic_info);
extern void Image_Alg_Manager(QString _filePath, PicDataInfo_ *pic_info);
extern int PackAllWheels(PicDataInfo_ *pic_info);
#endif