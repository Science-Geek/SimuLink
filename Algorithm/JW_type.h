#ifndef JW_type_H_
#define JW_type_H_

#define JW_ZONE_WIDTH			50
/************ ����ͼ�� ****************/
#define JW_UV_IMAGE_ZONE		0		//����ͼ��:�����Ϲ�LED��Դ����ʽCISͼ�񴫸���
/************ �׹�ͼ�� ****************/
#define JW_GRAY_IMAGE_ZONE		50		//�׹�ͼ��
#define JW_GRAY_IMAGE_VALUE		51		//�׹�ͼ�����ʶ��
/************ ����ͼ�� ****************/
#define JW_IR_IMAGE_ZONE		100		//����ͼ��
#define JW_IR_IMAGE_INK			101		//�����ī
#define JW_IR_IMAGE_EMBLEM		102		//����
#define JW_IR_IMAGE_100			103		//�����м�100
#define JW_IR_IMAGE_YILIN		104		//��������;
#define JW_IR_IMAGE_HALL_L		105		//��������������
#define JW_IR_IMAGE_HALL_R		106		//��������������
#define JW_IR_IMAGE_HALL_100	107		//�������100
/************ �����ͼ�� ****************/
#define JW_MULTISPECTRAL_ZONE	150	 	//�����ͼ��
#define JW_BREADTH_ERROR		166 	//������

#define JW_IR_IMG_FOREIGN		170		//��Һ����α��ɲ��
#define JW_IR_IMG_ID_BRAKE		171		//��ҹ��ֺ�ɲ��
/************ �����ѧ ****************/
#define JW_UV_ANALOG_ZONE		200		//�����ѧ = ������
#define	JW_UV_ANALOG_MID_NONE	201		//��������
#define	JW_UV_ANALOG_PAPER		202     //��ֽ
#define	JW_UV_ANALOG_TYPE3		203     //���ֵ���ڽӽ�
#define	JW_UV_ANALOG_TYPE4		204     //���͹�����ֵ���󣬰�ֽ
#define	JW_UV_ANALOG_TYPE5		205     //���ֵλ���뷽�򲻷�
#define	JW_UV_ANALOG_TYPE6		206     //���岻����
#define JW_UV_ANALOG_ZONE_END	249
/************ �����ѧ ****************/
#define JW_IR_ANALOG_ZONE		250		//�����ѧ:����Թ�
#define JW_IR_ANALOG_INK		251		//�����ī
#define JW_IR_ANALOG_ID			252		//���ֺ�
#define JW_IR_ANALOG_HALL		253		//����������
#define JW_IR_ANALOG_HALL_100	254		//�������100
#define JW_IR_ANALOG_PROG		255		//�����쳣
#define JW_IR_ZHAN_LIAN			256		//2�ų�Ʊճ��
#define JW_IR_YOU_WU            257		//���۱�
#define JW_IR_SM_VER_DIFF		258		//ƴ�ӱ�
#define JW_IR_SM_POS_SAME		259 	//�ߴź���īλ����ͬ
#define JW_IR_ANALOG_ZONE_END	299
/************ ӫ������ ****************/
#define JW_FLUO_ZONE			300		//ӫ������:��������
#define JW_FLUO_LEFT			301		//ӫ������:������
#define JW_FLUO_RIGHT			302		//ӫ������:������
#define JW_FLUO_MIDDLE			303		//ӫ������:������
#define JW_FLUO_ZONE_END		349
/************ ��ȫ�� ****************/
#define JW_BM_HD_ZONE			350		//��ȫ��:���
#define JW_BM_HD_MISALL			351		//��ȫ��:BMHD_Check �д��������������
#define JW_BM_HD_NOBM			352		//��ȫ��:BMHD_Check �д��������Ȳ������޴�ţ�
#define JW_BM_100_LOSE_PEAK		353		//��ȫ��:100��ȫ��ȱʧ
#define JW_HD_ERR				354		//��ȫ��:hd�ߵ�ƥ��
#define JW_HD_VER15_DISTURB		355		//��ȫ��:hd���Ź���
#define JW_BM_TOO_MUCH			356
/************ ��ͼ�� ****************/
#define JW_M_IMAGE_ZONE			400		//��ͼ��:λ�ã�����
#define JW_M_IMAGE_LNOTONLY		401		//LRSM_Check��������±ߴŲ�Ψһ
#define JW_M_IMAGE_RNOTONLY		402		//LRSM_Check���Ҳ����±ߴŲ�Ψһ
#define JW_M_IMAGE_POSERR		403		//LRSM_Check�бߴ�Ψһ��λ�ò���
#define JW_M_IMAGE_VALUE		410		//ͼ���������������š��������
/************ ������ ****************/
#define JW_M_FEATURE_ZONE		450		//������:��������
#define JW_M_FEATURE_LCROSS		451		//������:LRSM_Check�бߴ�Ψһ����ߴŴ�Խ�����㣨���ɣ�
#define JW_M_FEATURE_RCROSS		452		//������:LRSM_Check�бߴ�Ψһ���ұߴŴ�Խ�����㣨���ɣ�
#define JW_M_FEATURE_NOSM		453		//������:LRSM_Check�����ұߴŶ���ȹ�С���ϸ��ޱߴţ�
#define JW_M_RED_NOSM			454		//������:sm red no mag
#define JW_M_BLACK_NOSM			455		//������:sm black no mag
#define JW_M_BACK_CHAR_NOSM		456		//������:����ƴ���ַ��޴�
#define JW_M_BACK_NUM_NOSM		457		//������:LRSM_Check���ұߴŲ������ȸ�����
#define JW_M_FEATURE_DUM		458		//������:LRSM_Check��15�治����2���д�����
#define JW_M_FEATURE_DOUBLE_MAG	459		//������:ͬһ��ߴ�ǰ�������Ʊߴ�
#define JW_M_FOREIGN_MAG		463		//������:��Ҵ�����������
#define JW_M_ZONE_END			499
/************ ���ֺ� ****************/
#define JW_ID_ZONE		500		//���ֺ�
#define JW_ID_ERROR		501		//���ֺ�δʶ��
#define JW_ID_Double	503		//˫���ֺŲ���
/************ ���� ****************/
#define JW_OTHER_ZONE			550		//����
#define JW_OTHER_SLOPE			551		//��б
#define JW_OTHER_SPLIT			552		//����С�����
#define JW_OTHER_CON			553		//����
#define JW_OTHER_INCOMPLETE		554		//��ȱ���������
#define JW_OTHER_DATA_SHORT		555		//���ݹ���
#define JW_OTHER_MLINK			556		//������
#define JW_OTHER_ADHERE			557		//ճ����100Ԫ
#define JW_OTHER_REVERSE		558		//�ص�
#define JW_OTHER_SPLIT_IGNORE	559		//�ƶ����µķָ�
#define JW_OTHER_WORK_STATE		560		//����״̬���󣬸��㷨�Ĺ���״̬��ʵ�ʲ���
#define JW_OTHER_DATA_ERROR		561		//���ݴ���
#define JW_OTHER_WHEEL_ERROR	562		//��������
#define JW_OTHER_TAPE_STUDY_OK	563		//����ֽѧϰ�ɹ�
#define JW_OTHER_TAPE_STUDY_Err	564		//����ֽѧϰʧ��
#define JW_OTHER_IR_INCOMPLETE	567		//��ȱ�����⣩
#define JW_OTHER_CORNER_FORLD	568		//�۽�
#define JW_OTHER_OUTLINE		569		//���̱�
#define JW_OTHER_JOINT			570		//ƴ�ӱ�
#define JW_OTHER_PARALLEL_DEGREE	571	//Ť��
#define JW_OTHER_ERR_CORNER_IMGCIS   572  //�ǵ����
#define JW_OTHER_ERR_CORNER_IMGSOURCE   573  // �ǵ����
#define JW_OTHER_CIS_SLOPE		574		//ͼ����б
#define JW_OTHER_IMAGE_HOLE         575             //hole 
#define JW_OTHER_FALUN			580		//������
#define JW_OTHER_VALUE_DIFF		901		//��������ͬ
#define JW_OTHER_VERSION_DIFF	902		//�����Ű汾��ͬ
#define JW_OTHER_SYSTEM_RESET	903		//����
#define JW_OTHER_BLACK_LISET	904		//������
#define JW_OTHER_VALUE_ERR		905		//ͼ��������ȷ������ͬ
#define JW_OTHER_VERSION_ERR	906		//ͼ��������ȷ���İ汾��ͬ
#define JW_OTHER_INSIDE_BLACK_LISET	907		//????
#define JW_OTHER_VALUE_ERR_IR     908           //LC RC ���˺����ʶ������
#define JW_OTHER_SUSPECT_CASH     910           //�ɱ�

/************ ���� ****************/
#define JW_TAPE_ZONE		600		//����
#define JW_TAPE_IR0			600
#define JW_TAPE_IR1			601
#define JW_TAPE_IR2			602
#define JW_TAPE_IR3			603
#define JW_TAPE_IR4			604
#define JW_TAPE_IR5			605
#define JW_TAPE_IR6			606
#endif
