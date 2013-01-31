

namespace DataBase {
	//����ΪBF518/BF533ͨѶ��ؽṹ��
	struct PDZ101_STRUCT
	{
		unsigned char   CMD_IMAG;             //���ľ���,����ACK��NAKָ��ʱ
		unsigned long int Check_Sum; //У���
		unsigned int    SendTimes;
		unsigned int    ReSendTime;
	};

	struct LINEPPARA
	{
		unsigned int LineNUM;
	};
    
	struct stEVENT
	{
		unsigned char Type; //1:BF533 2:other

		unsigned int  No;
		unsigned long EventTime;
		unsigned char EventD_Type;
		unsigned char EventC_Type;
		unsigned int EventValue;

		unsigned char LineNo;
		unsigned int  FaultNo;
		unsigned char SPE;
		unsigned int  YcValue;
		unsigned int  millisecond;
		unsigned char min;
		unsigned char hour;
		unsigned char day;
		unsigned char month;
		unsigned int  year;
	};

	struct stJBPARA
	{
		unsigned char  Num;
		unsigned char  LineNo[48];
		unsigned int ParaNo[48];
		unsigned int Value[48];
	};

	struct stPMPara
	{
		unsigned char timeout_flag;
		unsigned int  LineNo;
		unsigned int  AngU1;
		unsigned int  AngU2;
		unsigned int  AngU3;
		unsigned int  AngCI1;
		unsigned int  AngCI2;
		unsigned int  AngCI3;
		unsigned int  AngBI1;
		unsigned int  AngBI2;
		unsigned int  AngBI3;
		unsigned int  AngU0;
		unsigned int  AngI0;
	};

	struct stLine_Val
	{
		unsigned char timeout_flag;
		size_t       Line_no;
		unsigned int Flag_Ua;
		unsigned int Flag_Ub;
		unsigned int Flag_Uc;
		unsigned int Flag_CIa;
		unsigned int Flag_CIb;
		unsigned int Flag_CIc;
		unsigned int Flag_BIa;
		unsigned int Flag_BIb;
		unsigned int Flag_BIc;
		unsigned int Flag_U0;
		unsigned int Flag_I0;
		unsigned int Flag_Angle_UaIa;
		unsigned int Flag_Angle_UbIb;
		unsigned int Flag_Angle_UcIc;
		unsigned int Flag_Fre_Val;
		unsigned int Flag_Cos_Val;
		unsigned int Flag_P_Val;
		unsigned int Flag_Q_Val;

		unsigned int D_Value_Ua;
		unsigned int D_Value_Ub;
		unsigned int D_Value_Uc;
		unsigned int D_Value_CIa;
		unsigned int D_Value_CIb;
		unsigned int D_Value_CIc;
		unsigned int D_Value_BIa;
		unsigned int D_Value_BIb;
		unsigned int D_Value_BIc;
		unsigned int D_Value_U0;
		unsigned int D_Value_I0;
		unsigned int Angle_UaIa;
		unsigned int Angle_UbIb;
		unsigned int Angle_UcIc;
		unsigned int Fre_Val;
		unsigned int Cos_Val;
		unsigned int P_Val;
		unsigned int Q_Val;
	};

	struct stHarmonic
	{
		size_t       Line_no;
		size_t       harmonic_no;

		unsigned int Value_Ua;
		unsigned int Value_Ub;
		unsigned int Value_Uc;
		unsigned int Value_Ia;
		unsigned int Value_Ib;
		unsigned int Value_Ic;
		unsigned int Value_U0;
		unsigned int Value_I0;
		unsigned int P_Val;
		unsigned int Q_Val;
		unsigned int Per_Val;
	};

	struct stLine_ValCoef  //��·��ֵϵ��
	{
		unsigned char timeout_flag;
		size_t       Line_no;

		unsigned int Flag_Ua;
		unsigned int Flag_Ub;
		unsigned int Flag_Uc;
		unsigned int Flag_CIa;
		unsigned int Flag_CIb;
		unsigned int Flag_CIc;
		unsigned int Flag_BIa;
		unsigned int Flag_BIb;
		unsigned int Flag_BIc;
		unsigned int Flag_U0;
		unsigned int Flag_I0;
		unsigned int Flag_Angle_UaIa;
		unsigned int Flag_Angle_UbIb;
		unsigned int Flag_Angle_UcIc;
		unsigned int Flag_Fre_Val;
		unsigned int Flag_Cos_Val;
		unsigned int Flag_P_Val;
		unsigned int Flag_Q_Val;

		unsigned int H_Value_Ua;
		unsigned int H_Value_Ub;
		unsigned int H_Value_Uc;
		unsigned int H_Value_CIa;
		unsigned int H_Value_CIb;
		unsigned int H_Value_CIc;
		unsigned int H_Value_BIa;
		unsigned int H_Value_BIb;
		unsigned int H_Value_BIc;
		unsigned int H_Value_U0;
		unsigned int H_Value_I0;
		unsigned int Angle_UaIa;
		unsigned int Angle_UbIb;
		unsigned int Angle_UcIc;
	};

	struct stBoardInq
	{
		unsigned int YkBoardNum;
		unsigned int YxBoardNum;
		unsigned int YcBoardNum;
	};

	struct stLine_BCurVal  //����������ֵϵ��
	{
		size_t       Line_No;
		unsigned int D_Val_BI1;
		unsigned int D_Val_BI2;
        unsigned int D_Val_BI3;
	};

	struct stLine_BCurVerVal  //����������ֵϵ��
	{
		size_t       timeout_flag;
		size_t       Line_No;
		unsigned int V_Val_BI1;
		unsigned int V_Val_BI2;
		unsigned int V_Val_BI3;
	};

	struct stDCVerVal //ֱ����У��
	{
	   unsigned char timeout_flag; //�˴�����֪ͨPDAУ��ɹ�����ʧ��
       size_t  ChannelNo;
	   unsigned int V_Val_DC;
	};

	struct stDCValCoef //ֱ����У��ϵ����ѯ
	{
		size_t  ChannelNo;
		unsigned int V_HVal_DC;
	};

	struct stBF533Base     //װ�ò�����װʱ�ṹ��
	{
		unsigned int       BoardType;
		unsigned int       Type        ;    //װ������
		unsigned int       Version      ;    //DSP����汾��
		unsigned int       YKHZTime  ;  	//���غ�բʱ��
		unsigned int       YKTZTime  ;  	//������բʱ��
		unsigned int       YXLvBoTime  ;  	//ң���˲�ʱ��
		unsigned int       ChannelSum ;     //ͨ������
		unsigned int       SYXNUM;
		unsigned int       LineNum;
		unsigned int       AotuRstYxTime ;   //����ң���Զ�����ʱ��
		unsigned short     AotuRstYxFlag ;   	//����ң���Զ�����Ͷ�˱�־
		unsigned short     HuoHua_AutoFlag; // �Զ��Ͷ�˱�־
		unsigned short     HuoHua_Day  ;    // �Զ�������������
		unsigned short     Pro_Rst_Time;    //���������Զ�����ʱ��     ZHANGZHIHUA
		unsigned short     Flag_Pro_Rst;    //���������Զ�����Ͷ�˱�־ ZHANGZHIHUA
		unsigned int       I_Rated;         //�����ֵ
	} ;	//BF533Base

	struct    stProVal  //������ֵ�����������ֽṹ��
	{
		unsigned short OverLoad     ;      //������Ͷ���� 0���˳� 1������ 
		unsigned short OL_Alarm_F     ;    //�澯 �� 0���˳� 1���澯
		unsigned int   OverLoadValue;         //�����ɶ�ֵ  
		unsigned int   OverLoadTime;        //������ʱ�䶨ֵ  
		unsigned int   OverLoadTZWait;        //��������բ��ʱ  

		unsigned short   I_PRO     ;      //I�α���Ͷ���� 0���˳� 1������  
		unsigned short   OF_Alarm_F     ; //�澯 �� 0���˳� 1���澯
		unsigned int  I_PROValue;         //I�α�����ֵ  
		unsigned int  I_PROTime;        //I�α���ʱ�䶨ֵ  
		unsigned int  I_PROTZWait;       //��բ��ʱ  

		unsigned short   II_PRO;      //II�α���Ͷ���� 0���˳�1������ 
		unsigned short   II_Alarm_F     ; //�澯 �� 0���˳� 1���澯
		unsigned int  II_PROValue;         //II�α�����ֵ  
		unsigned int  II_PROTime;        //II�α���ʱ�䶨ֵ  
		unsigned int  II_PROTZWait;       //��բ��ʱ  

		unsigned int  InverseFlag;    //��ʱ��Ͷ�� 0���˳�1��Ͷ��
		unsigned int  InverseType;    //��ʱ������ 0��һ�㷴ʱ��,  1���ǳ���ʱ�ޣ�2�����˷�ʱ��

		unsigned short   I0_PRO     ;      //�������Ͷ���� 0���˳�1���澯2������ 
		unsigned short   I0_Alarm_F ;
		unsigned int  I0_PROValue;         //���������ֵ  
		unsigned int  I0_PROTime;        //�������ʱ�䶨ֵ  
		unsigned int  I0_PROTZWait;

		unsigned short   I0_II_PRO     ;      //����II����Ͷ���� 0���˳�1���澯2������ 
		unsigned short   I0_II_Alarm_F ;
		unsigned int  I0_II_PROValue;         //����II������ֵ  
		unsigned int  I0_II_PROTime;        //����II����ʱ�䶨ֵ  
		unsigned int  I0_II_PROTZWait;

		unsigned short   U0_PRO     ;      //�����ѹͶ���� 0���˳�1���澯2������  
		unsigned int  U0_PROValue;         //�����ѹ��ֵ  
		unsigned int  U0_PROTime;        //�����ѹʱ�䶨ֵ  

		unsigned int  Accel_T;        //�����ʱ�䶨ֵ
		unsigned int  Accel_F;

		unsigned int   OverU_PRO_F; //��ѹ����  0���˳� 1���澯
		unsigned int   OverU_Alarm_F; //��ѹ�澯  0���˳� 1���澯
		unsigned int  OverU_P_Val;    //��ѹ������ֵ
		unsigned int  OverU_Ck_T;     //��ѹ�����ʱ��
		unsigned int  OverU_PROTZWait;     //��ѹ��բ��ʱʱ��


		unsigned short   Reclose_PRO    ;    //�غ�բͶ���� 0���˳�1���澯  
		unsigned int  Reclose_PROTime;        //�غ�բʱ�䶨ֵ 

		unsigned short Low_PRO;          //�͵�ѹ����Ͷ���� ZHANGZHIHUA
		unsigned int  Low_Lock;          //�͵�ѹ������ֵ   ZHANGZHIHUA 

		unsigned short FAFlag; 
		unsigned short Local_FAFlag;
		unsigned int   FACheckTime;
		unsigned int   FAOpenDalayTime;         
		unsigned int   FAPowerSideAddr;        
		unsigned int   FALoadSideAddr;   
		unsigned int   FAOverLoadLoseVolSum;
		unsigned int   FAReturnToZeroTime;
	};
};//namespace DataBase

