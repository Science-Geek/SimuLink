/***************************************************************
* Copyright (C) 2011, Wuhan University
* All rights reserved.
*
* �ļ�����	Recognition.h
* ����������
*
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2016��9��18��
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

/*************************** ����ƽ̨ ****************************/
#define MODULE_TEST                //ģ�����or�������

#define LENGTH_CHECK_ONOFF         //����������
#define VVD_CHECK_ONOFF            //���汾����ʶ�𿪹�
#define CashIDOCR_CHECK_ONOFF      //���ֺż�⿪��
#define MULTISPECTRAL_CHECK_ONOFF  //����׼�⿪��
#define SPLICING_CHECK_ONOFF       //ƴ�ӱҼ�⿪��
#define WRITING_CHECK_ONOFF        //�����Ҽ�⿪��

#define SAVE_IMG               //ԭͼ��ͼ����
//#define SAVE_VVD_IMG           //�汾�����ͼ����
//#define SAVE_CashIDOCR_IMG     //���ֺŴ�ͼ����
//#define SAVE_MULTISPECTRAL_IMG //����״�ͼ����
//#define SAVE_SPLICING_IMG      //ƴ�ӱҴ�ͼ����
//#define SAVE_WRITING_IMG       //�����Ҵ�ͼ����

//#define SAVE_CORNER_IMG    //�ǵ��Ǵ�ͼ����
//#define SAVE_SPLICE_BIN
/******************************************��α���ֶ��ӿ����ã����£�***************************************/
#define RMB100 0x00000000    //�����
#define RMB50  0x00010000
#define RMB20  0x00020000
#define RMB10  0x00030000
#define RMB5   0x00040000
#define RMB1   0x00050000
#define VERSION15    0x00000000    //�汾���
#define VERSION05    0x00001000
#define VERSION99    0x00002000
#define LEVEL0      0x00000000 //�����ȣ�LEVEL0��ʾ����رգ�
#define LEVEL1      0x00000001
#define LEVEL2      0x00000002
#define LEVEL3      0x00000003
#define LEVEL4      0x00000004
#define LEVEL5      0x00000005
#define LEVEL6      0x00000006
#define LEVEL7      0x00000007
#define LEVEL8      0x00000008
#define LEVEL9      0x00000009
#define NO_SAMPLE   0x00000000   //�Ƿ����
#define SAMPLE      0x00010000   
#define ROW_ONE     0x00000100   //������
#define ROW_TWO     0x00000200
#define ROW_THREE	0x00000300
#define ROW_FOUR	0x00000400
#define COL_ONE     0x00000001
#define COL_TWO     0x00000002
#define COL_THREE	0x00000003
#define COL_FOUR	0x00000004
/******************************************��α���ֶ��ӿ����ã����ϣ�***************************************/


//ͼ���С�ṹ��
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

/************************���ã���+��+��+�**********************/
#define BEGIN_ACTION(msg) printf(msg)
//ͼ������ṹ��
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

int loadBitmapFromFile(const char *filePath, U8 **bits, ImgSizeInfo* _imgsize);    //�޸Ĺ���ȥ��ȫ�ֱ���
int CreateImage(IplImage_ *_img, int _width, int _height, U8 *_image_data);
void ReleaseImage(IplImage_ *_img);
void SaveGrayBitmap(const char *fileName, const U8 *imageData, int width, int height);


/******************************(��+�ţ�****************************/
#define u8 unsigned char
#define WHOLE_IMAGE    1        //����Ǯ
#define HALF_IMAGE     0        //����Ǯ
#define TYPE_ROTATE_LINE    0   //���Բ�ֵ
#define TYPE_ROTATE_LINEAR  1   //˫���Բ�ֵ

//����
#define DIRECTION_LEFT		0   //��ͼ�����ֺ������
#define DIRECTION_RIGHT		1   //��ͼ�����ֺ����ұ�
#define DIRECTION_SAME		2   //����

//����ṹ��
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
	int rotate_angle;			//0.5��Ϊ��λ
	int	direction;				//���ң�����Ҫ�����ģ���ͼҪ����һ��
	int channel;				//0y,1u
	int type;					//˫����1������0
	int mean;
}RotateZoneInfo_;

//20161129  FSN���ýṹ��  Fanyihua
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

/*************��ֽṹ��****************/
typedef struct QFInfo_
{
	float score;   //��ֻ��÷�
	float score_ir; //IR��ֵ÷�
	int level; //��ֻ��ȼ�
	int thred1;
	int thred2;
	//int dct_score; //DCT�÷֣�Ԥ����
}QFInfo_;

typedef struct BreakageTypeResult_   //������
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
	int affine_width;   // У������
	int affine_height;  // У����߶�
	int mapping_offset; // ����ͼ������ͼ��ƫ��
	int img_type;

	float edge_real_len[4];       //4���ߵ�ʵ�ʱ߳�
	int missing_info[2];   //ͼ��ȱʧ��Ϣ  -1��ȱʧ��0-3ȱʧ����

	CoordinatesInfo_	coordinates_up;
	CoordinatesInfo_	coordinates_bottom;
	int	affine_operator_trans_up[6];// ����ͼ�����������16λ
	int	affine_operator_trans_bottom[6];// ����ͼ�����������16λ

	int img_breakage_check;  //ͼ�������⿪��
	BreakageTypeResult BreakResult[4];

	unsigned char *up_data;  //����ͼ�׵�ַ
	unsigned char *bottom_data;
	int		IR_data_offset;		//ƫ�ƶ��ǴӸ�������ʵ��ַ��ʼ����
	int		IR_img_offset[2];    //IRͨ��ͼƫ��
	int		TR_img_offset[2];    //Ͷ��ͨ��ͼƫ��
	int		UV_img_offset;
	int		R_img_offset;
	int		G_img_offset;
	int		B_img_offset;

	unsigned int all_wheels[384 * 6];

	char	ID[16];				//w
	char	ID_Row[16];
	int		ID_Err;				//w
	//���ֺ�201611.08
	RotateZoneInfo_		ID_ZONE;
	RotateZoneInfo_		ID_ZONE_VERTICAL;
	RotateZoneInfo_		IR_PART;
	RotateZoneInfo_		GRAY_PART;
	ID_cropImgInfo_		ID_Crop;
	int 	jiabi;

	/*********�������***********/
	QFInfo_             QF_Info;
}PicDataInfo_;

typedef struct FloatPoint //��������ṹ��
{
	float x; /* X����, ͨ����0Ϊ����*/
	float y; /* Y����, ͨ����0Ϊ����*/
}FloatPoint_;

typedef struct IntPoint //��������ṹ��
{
	int x; /* X����, ͨ����0Ϊ����*/
	int y; /* Y����, ͨ����0Ϊ����*/
}IntPoint_;

typedef struct ZoneInfo
{
	int xstart;
	int ystart;
	int	width;
	int height;
	//int width_step;
	int type;					//˫����1������0

	//���ֺ�201611.08
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