#ifndef	_CURRENCY_CHECK_MANAGE_H_
#define	_CURRENCY_CHECK_MANAGE_H_
#include "machine_type.h"

#ifdef JULONG_TMR
#include "julong_tmr\data_change.h"
#endif

#ifdef YUECHUANG
#include "yuechuang\data_change.h"
#endif

#ifdef BAIJIA
#include "baijia\data_change_baijia.h"
#endif

#ifdef RONGHE
#include "ronghe\data_change_ronghe.h"
#endif

#ifdef KANGYI
#include "kangyi\data_change_kangyi.h"
#endif

#ifdef CHUANWEI
#include "chuanwei\data_change_chuanwei.h"
#endif

#define IF_ANGLE_SLOPE			20
#define RMB_ERR_TYPE_BUF_MAX	5

enum
{
	TDN_VER_NONE = 0,
	TDN_VER_UNIDENTIFICATION = -1,   ///版本未识别
	TDN_VER_05 = 5,
	TDN_VER_15 = 15,
	TDN_VER_99 = 99,
	TDN_VER_4TH = 90,
	TDN_VER_SPACE = 100,
};

enum
{
	TDN_DIR_ERR = 0,
	TDN_DIR_1 = 1,
	TDN_DIR_2 = 2,
	TDN_DIR_3 = 3,
	TDN_DIR_4 = 4,
	TDN_DIR_MAX,
};

enum height_macro
{
	HEIGHT_100 = 100,
	HEIGHT_50_20_10 = 80,
	HEIGHT_5_1 = 5
};

typedef struct BM_LevelParam
{
	int BM_100_15;
	int BM_100;
	int BM_50;
	int BM_20;
	int BM_20_99;
	int BM_10;
	int BM_5;
	int BM_GAP;
}BM_LEVEL_PARAM;

typedef struct IntermediateResult_
{
	BM_LEVEL_PARAM BM_LevelHigh;
	BM_LEVEL_PARAM BM_LevelMid;
	int LSM_PulseNum[SM_NUM];
	int RSM_PulseNum[SM_NUM];
	int LMM_PulseNum;
	int LMM1_PulseNum;
	int RMM_PulseNum;
	int RMM1_PulseNum;
	int IR_BackGroud[IR_NUM];
}INTERMEDIATE_RESULT;

typedef struct tagResult_RMB
{
	int angle;
	int Height;
	int ERRtype;
	int Amount;
	int Direction;
	int VersionM;
	int SirVer;

	int ImgAmount;
	int ImgVersion;

	int ErrTypeNum;
	int ErrTypeBuf[RMB_ERR_TYPE_BUF_MAX];
	int New_Amount;
	int New_ERRtype;
	
}RESULT_RMB;

typedef struct Sensitive
{
	int Err_351;  	//磁面额灵敏度上升，关闭后也不会完全消失
	int Err_353;	//检测05 99 100元磁残缺
	int Err_354;	//15版hd检测
	int Err_403;	//边磁位置检测
	int Err_453;	//边磁有无检测
	int Err_454;	//边磁红无
	int Err_455;	//边磁黑无
	int Err_456;	//背面拼音无磁
	int Err_457;	//背面数字无磁
	int Err_458;	//15版背面中磁无
	int Err_459;	//暂未用
	int Err_251;	//暂未用
	int Err_252;	//暂未用
	int Err_253;	//暂未用
	int Err_256;	//暂未用
	int Err_257;	//暂未用
	int Err_258;	//暂未用
	int Err_259;	//暂未用
	int Err_201;	//中紫外
	int Err_202;	//边紫外检白纸
}Sensitive;

typedef struct FuncSwitch
{
	bool RMB_4th;
	bool RMB_Space;
}FuncSwitch;

typedef struct PriorResult
{
	int Amount;//面额
	int CashType;//币种
	int Version;//版本
	int Direction;//方向
}PRIOR_RESULT;

//extern TdnData_Alg AlgDataBuf;
//extern OneDSignal AlgSignal[CPU_MAX];
extern Sensitive SensitiveParam;

extern WheelData CashFirstWheel;

extern INTERMEDIATE_RESULT IntermediateResult;//动态调试结构体

void Currency_CheckManage(RESULT_RMB *RMB_Result, int LastError, Sensitive *Sensitive_p, FuncSwitch *funcswitch);

extern int MagImg_imgPro(unsigned char *pTopData, int *top_x, int *top_y, int width, int height, int channel, int amount, int version, int dirction, unsigned int *pWheel, int wheelLen, int ImgMosaicsLine, Sensitive *Sensitive_p);//0正常非0报错代码

extern int MagImg_masterPro();

extern void magImg_reset();

extern void MagImgFree();

#endif