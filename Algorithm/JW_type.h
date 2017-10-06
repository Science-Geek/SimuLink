#ifndef JW_type_H_
#define JW_type_H_

#define JW_ZONE_WIDTH			50
/************ 紫外图像 ****************/
#define JW_UV_IMAGE_ZONE		0		//紫外图像:采用紫光LED光源反射式CIS图像传感器
/************ 白光图像 ****************/
#define JW_GRAY_IMAGE_ZONE		50		//白光图像
#define JW_GRAY_IMAGE_VALUE		51		//白光图像面额识别
/************ 红外图像 ****************/
#define JW_IR_IMAGE_ZONE		100		//红外图像
#define JW_IR_IMAGE_INK			101		//光变油墨
#define JW_IR_IMAGE_EMBLEM		102		//国徽
#define JW_IR_IMAGE_100			103		//正面中间100
#define JW_IR_IMAGE_YILIN		104		//正面衣领;
#define JW_IR_IMAGE_HALL_L		105		//反面左人民大会堂
#define JW_IR_IMAGE_HALL_R		106		//反面右人民大会堂
#define JW_IR_IMAGE_HALL_100	107		//大会堂下100
/************ 多光谱图像 ****************/
#define JW_MULTISPECTRAL_ZONE	150	 	//多光谱图像
#define JW_BREADTH_ERROR		166 	//幅宽检测

#define JW_IR_IMG_FOREIGN		170		//外币红外鉴伪号刹车
#define JW_IR_IMG_ID_BRAKE		171		//外币冠字号刹车
/************ 紫外光学 ****************/
#define JW_UV_ANALOG_ZONE		200		//紫外光学 = 中紫外
#define	JW_UV_ANALOG_MID_NONE	201		//中紫外无
#define	JW_UV_ANALOG_PAPER		202     //白纸
#define	JW_UV_ANALOG_TYPE3		203     //最大值过于接近
#define	JW_UV_ANALOG_TYPE4		204     //饱和过多或均值过大，白纸
#define	JW_UV_ANALOG_TYPE5		205     //最大值位置与方向不符
#define	JW_UV_ANALOG_TYPE6		206     //波峰不够大
#define JW_UV_ANALOG_ZONE_END	249
/************ 红外光学 ****************/
#define JW_IR_ANALOG_ZONE		250		//红外光学:红外对管
#define JW_IR_ANALOG_INK		251		//光变油墨
#define JW_IR_ANALOG_ID			252		//冠字号
#define JW_IR_ANALOG_HALL		253		//左人民大会堂
#define JW_IR_ANALOG_HALL_100	254		//大会堂下100
#define JW_IR_ANALOG_PROG		255		//程序异常
#define JW_IR_ZHAN_LIAN			256		//2张钞票粘连
#define JW_IR_YOU_WU            257		//油污币
#define JW_IR_SM_VER_DIFF		258		//拼接币
#define JW_IR_SM_POS_SAME		259 	//边磁和油墨位置相同
#define JW_IR_ANALOG_ZONE_END	299
/************ 荧光特征 ****************/
#define JW_FLUO_ZONE			300		//荧光特征:左右紫外
#define JW_FLUO_LEFT			301		//荧光特征:左紫外
#define JW_FLUO_RIGHT			302		//荧光特征:右紫外
#define JW_FLUO_MIDDLE			303		//荧光特征:右紫外
#define JW_FLUO_ZONE_END		349
/************ 安全线 ****************/
#define JW_BM_HD_ZONE			350		//安全线:大磁
#define JW_BM_HD_MISALL			351		//安全线:BMHD_Check 中大磁所有面额都不满足
#define JW_BM_HD_NOBM			352		//安全线:BMHD_Check 中大磁特征宽度不够（无大磁）
#define JW_BM_100_LOSE_PEAK		353		//安全线:100安全线缺失
#define JW_HD_ERR				354		//安全线:hd高低匹配
#define JW_HD_VER15_DISTURB		355		//安全线:hd干扰过多
#define JW_BM_TOO_MUCH			356
/************ 磁图像 ****************/
#define JW_M_IMAGE_ZONE			400		//磁图像:位置，有无
#define JW_M_IMAGE_LNOTONLY		401		//LRSM_Check中左侧上下边磁不唯一
#define JW_M_IMAGE_RNOTONLY		402		//LRSM_Check中右侧上下边磁不唯一
#define JW_M_IMAGE_POSERR		403		//LRSM_Check中边磁唯一但位置不对
#define JW_M_IMAGE_VALUE		410		//图像测出的面额不符，报磁。。猥琐流
/************ 磁特征 ****************/
#define JW_M_FEATURE_ZONE		450		//磁特征:波形特征
#define JW_M_FEATURE_LCROSS		451		//磁特征:LRSM_Check中边磁唯一但左边磁穿越数不足（宽松）
#define JW_M_FEATURE_RCROSS		452		//磁特征:LRSM_Check中边磁唯一但右边磁穿越数不足（宽松）
#define JW_M_FEATURE_NOSM		453		//磁特征:LRSM_Check中左右边磁都宽度过小（严格无边磁）
#define JW_M_RED_NOSM			454		//磁特征:sm red no mag
#define JW_M_BLACK_NOSM			455		//磁特征:sm black no mag
#define JW_M_BACK_CHAR_NOSM		456		//磁特征:背面拼音字符无磁
#define JW_M_BACK_NUM_NOSM		457		//磁特征:LRSM_Check中右边磁不符合先高特征
#define JW_M_FEATURE_DUM		458		//磁特征:LRSM_Check中15版不符合2个中磁特征
#define JW_M_FEATURE_DOUBLE_MAG	459		//磁特征:同一侧边磁前后都有相似边磁
#define JW_M_FOREIGN_MAG		463		//磁特征:外币磁特征不符合
#define JW_M_ZONE_END			499
/************ 冠字号 ****************/
#define JW_ID_ZONE		500		//冠字号
#define JW_ID_ERROR		501		//冠字号未识别
#define JW_ID_Double	503		//双冠字号不等
/************ 其他 ****************/
#define JW_OTHER_ZONE			550		//其他
#define JW_OTHER_SLOPE			551		//倾斜
#define JW_OTHER_SPLIT			552		//破损（小面积）
#define JW_OTHER_CON			553		//连张
#define JW_OTHER_INCOMPLETE		554		//残缺（大面积）
#define JW_OTHER_DATA_SHORT		555		//数据过短
#define JW_OTHER_MLINK			556		//磁连钞
#define JW_OTHER_ADHERE			557		//粘连币100元
#define JW_OTHER_REVERSE		558		//回倒
#define JW_OTHER_SPLIT_IGNORE	559		//破洞导致的分割
#define JW_OTHER_WORK_STATE		560		//工作状态错误，给算法的工作状态与实际不符
#define JW_OTHER_DATA_ERROR		561		//数据错误
#define JW_OTHER_WHEEL_ERROR	562		//码盘跳变
#define JW_OTHER_TAPE_STUDY_OK	563		//胶带纸学习成功
#define JW_OTHER_TAPE_STUDY_Err	564		//胶带纸学习失败
#define JW_OTHER_IR_INCOMPLETE	567		//残缺（红外）
#define JW_OTHER_CORNER_FORLD	568		//折角
#define JW_OTHER_OUTLINE		569		//长短币
#define JW_OTHER_JOINT			570		//拼接币
#define JW_OTHER_PARALLEL_DEGREE	571	//扭曲
#define JW_OTHER_ERR_CORNER_IMGCIS   572  //角点错误
#define JW_OTHER_ERR_CORNER_IMGSOURCE   573  // 角点错误
#define JW_OTHER_CIS_SLOPE		574		//图像倾斜
#define JW_OTHER_IMAGE_HOLE         575             //hole 
#define JW_OTHER_FALUN			580		//反宣币
#define JW_OTHER_VALUE_DIFF		901		//与首张面额不同
#define JW_OTHER_VERSION_DIFF	902		//与首张版本不同
#define JW_OTHER_SYSTEM_RESET	903		//重启
#define JW_OTHER_BLACK_LISET	904		//黑名单
#define JW_OTHER_VALUE_ERR		905		//图像与主控确定的面额不同
#define JW_OTHER_VERSION_ERR	906		//图像与主控确定的版本不同
#define JW_OTHER_INSIDE_BLACK_LISET	907		//????
#define JW_OTHER_VALUE_ERR_IR     908           //LC RC 坏了后面额识别问题
#define JW_OTHER_SUSPECT_CASH     910           //疑币

/************ 胶带 ****************/
#define JW_TAPE_ZONE		600		//胶带
#define JW_TAPE_IR0			600
#define JW_TAPE_IR1			601
#define JW_TAPE_IR2			602
#define JW_TAPE_IR3			603
#define JW_TAPE_IR4			604
#define JW_TAPE_IR5			605
#define JW_TAPE_IR6			606
#endif
