/***************************************************************

*
***************************************************************/
#ifndef LOCATECORNEREXT_H
#define LOCATECORNEREXT_H


#define ERR_CORNER				        -50	
#define ERR_CORNER_IMGCIS			    ERR_CORNER-1	//CIS������
#define ERR_CORNER_IMGSOURCE			ERR_CORNER-2	//ͼ��Դ������
#define ERR_CORNER_EDGE_DEGREE			ERR_CORNER-3	//�Ƕȹ���
#define ERR_CORNER_PARALLEL_DEGREE		ERR_CORNER-4	//ͨ���ж��Ƿ�ƽ��,���߽ǶȲ���
#define ERR_COORNER_BIG_ANGLE           ERR_CORNER-5    //���۽�
#define ERR_TWO_IMG                     ERR_CORNER-6

//����Ϣ�ṹ��
typedef struct EdgeInfo
{
	double	k;	//�ߵĲ���
	double	b;
	int     degree;   //�ߵ����
	int		confidence;   //�ߵ����Ŷ�
	int     type;   //�ߵ�����
}EdgeInfo_;

//�ĸ��ǵĽǶ�
typedef struct Angle
{
	int angle_up_left;
	int angle_up_right;
	int angle_down_left;
	int angle_down_right;
}Angle_;

//�ߵ�����
typedef struct EdgeFeature
{
	float confidence;
	float var;           //�ߵ�ķ���
	float mean;          //�ߵ�ľ�ֵ
	int deviation;     //��ƫ�Ƶĵ���

}EdgeFeature_;

typedef struct
{
	int		height;
	int		width;
	int 		width_step;
	unsigned char *image_data;
	CoordinatesInfo_	coordinates;        // ��ͨ��ͼ��ֽ�ҵ��ĸ�������
	EdgeInfo_			UpEdgeInfo;
	EdgeInfo_			DownEdgeInfo;
	EdgeInfo_			LeftEdgeInfo;
	EdgeInfo_			RightEdgeInfo;

	int type;
}IR_PicInfo_;

//�ǵ���Ϣ�ṹ��
typedef struct CornerCorrectInfo
{
	int width;         //��ͨ��ͼ���
	int width_step;     // ȫͼ��
	int height;        //ȫͼ��

	CoordinatesInfo_	coordinates;        // ��ͨ��ͼ��ֽ�ҵ��ĸ�������
	EdgeInfo_			up_edge_info;       // ֽ�ҵ��ϱ���Ϣ
	EdgeInfo_		    down_edge_info;     // ֽ�ҵ��±���Ϣ
	EdgeInfo_			left_edge_info;     // ֽ�ҵ������Ϣ
	EdgeInfo_			right_edge_info;    // ֽ�ҵ��ұ���Ϣ
	int *edge_x;
	int *edge_y;
	int *edge_x_up;      //�ϱߵĵ��y����
	int *edge_x_down;
	int *edge_x_left;    //��ߵĵ��x����
	int *edge_x_right;

	float edge_real_len[4];       //4���ߵ�ʵ�ʱ߳�

	//ͼ������
	Angle_ angle;
	EdgeFeature_ edge_feature[4];
	float areaSize[4];  //��Ӧ�ĸ��۽ǵ����
	int missing_info;
}CornerCorrectInfo_;

//����Ϣ�ṹ��
typedef struct IntEdge
{
	long long	k;	//�ߵĲ���
	long long	b;
	int     type;   //�ߵ�����
}IntEdge_;

//�޸��ҽǵ�ı�����ֵ����
int SetCornerBackgroundThred(int _thred);

int ValueCheckByLength(PicDataInfo_ *_pic_data_info, int confidence_up, int confidence_down);

int GetAffineCorner(PicDataInfo_ *_pic_data_info, struct CornerCorrectInfo *_corner_info, IR_PicInfo_*Up_IR_info, IR_PicInfo_*Down_IR_info, int _tri_type[]);

//�ǵ�У��ģ�飺ȷ��ֽ�ҵ��ĸ��ǵ����꣬�������任����
//_up_imgΪ����ͼ��Դ��_down_imgΪ����ͼ��Դ��*_pic_data_infoΪͼ����Ϣ�ṹ��
int GetAffineParameter(PicDataInfo_ *_pic_data_info, int _tri_type[]);

int GetAffineImg(IplImage_ *money_grey_up, IplImage_ *money_grey_bottom, PicDataInfo_ *_pic_data_info);
//��÷�����Ǯ��irͼ��greyͼ
//��÷�����Ǯ��irͼ��greyͼ
//*_src_ir_imgΪԭʼirͼ, *p_src_greyΪԭʼgreyָ��, *_new_ir_imgΪ�µ�irͼ, *_new_grey_imgΪ�µ�greyͼ, *zone_infoΪ������Ϣ, *MatΪ�������
int WarpAffineMoneyInt(IplImage_ *_src_ir_img, u8 *p_src_grey, IplImage_ *_new_grey_img, ZoneInfo_ *zone_info, int *Mat);

//�����ͼ����
int WarpAffineZoneInt(IplImage_ *_new_img, IplImage_ *_src_img, RotateZoneInfo_ *zone_info, int *_mat);

//�������������ͼ����
int WarpAffineZoneInt_Samp(IplImage_ *_new_ir_img, IplImage_ *_new_grey_img, IplImage_ *_src_ir_img, IplImage_ *_src_grey_img, RotateZoneInfo_ *zone_info, int *_mat, int _sample_ena, int _row_samp_rate, int _col_samp_rate);

//�������������ͼ����-��ͨ��
int WarpAffineZoneInt_Samp_One(IplImage_ *_new_img, IplImage_ *_src_img, RotateZoneInfo_ *zone_info, int *_mat, int _sample_ena, int _row_samp_rate, int _col_samp_rate);

int getZoneCoordinates(int coord[][3], int* val_rows, int* del_rows, int width, int height, RotateZoneInfo_ *zone_info, int *_mat);

#endif