#pragma once
#include "Terminal.h"

namespace DataBase {

const int CHANNELMAX = 40;     //���ͨ������
const int LINEMAX = 10;        //�����·��

struct  stYCChannel//ͨ��������װʱ���ýṹ��
{
	unsigned short Type    ;   //ͨ������ 0����ѹ 1������ 2��ֱ�� 3��CVT
	unsigned int para[2]   ;   //ͨ��У�����
};

struct stBF533Base     //װ�ò����ṹ��
{
	unsigned int       Type        ;    //װ������
	unsigned int       Version      ;   //DSP����汾��
	unsigned int       YKHZTime  ;  	//���غ�բʱ��
	unsigned int       YKTZTime  ;  	//������բʱ��
	unsigned int       YXLvBoTime  ;  	//ң���˲�ʱ��
	
	unsigned int       AotuRstYxTime ;  //����ң���Զ�����ʱ��
	unsigned short     AotuRstYxFlag ;  //����ң���Զ�����Ͷ�˱�־
	unsigned short     HuoHua_AutoFlag; // �Զ��Ͷ�˱�־
	unsigned short     HuoHua_Day  ;    // �Զ�������������
	unsigned short     Pro_Rst_Time;    //���������Զ�����ʱ��     ZHANGZHIHUA
	unsigned short     Flag_Pro_Rst;    //���������Զ�����Ͷ�˱�־ ZHANGZHIHUA
	unsigned int       I_Rated;         //�����ֵ

	unsigned int       ChannelSum ;     //ͨ������
	unsigned int       SYXNUM;
	unsigned int       LineNum;
};

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
struct  stLinePara       //��·ͨ����Ϲ�ϵ
{
	unsigned short Vol_Chan[3]   ;  //��ѹͨ��
	unsigned short Cur_Chan[3]   ;  //����ͨ��
	unsigned short BCur_Chan[3]; //��������ͨ��
	unsigned short U0_Chan   ;  //�����ѹͨ��
	unsigned short I0_Chan   ;  //�������ͨ��	

	int            YxStart; //��ң����ʼ��ַ
	unsigned short YkNo;//YK���back
};

struct stEXTYCVAL 
{
	int Ua;
	int Ub;
	int Uc;
	int Ia;
	int Ib;
	int Ic;
	int U0;
	int I0;
	int Fre;
	int Fac;
	int AP;
	int RP;
};

class CBF533Terminal :
	public CTerminal
{
public:
	virtual ~CBF533Terminal(void);

	//xml cfg api
	virtual void SaveXmlCfg(FileSystem::CMarkup & xml);
	virtual int LoadXmlCfg(FileSystem::CMarkup & xml);

	int AddGeneralCmd(Protocol::CCmd cmdVal);
	int ActiveBattery(bool val);
	//int SynTime(void);
	int Reconnect();

	int getTerminalYcINum(int tIndex);
	

	static CBF533Terminal * getMyInstance(boost::asio::io_service & io_service,CSubStation & sub);

private:
	void setYcISendNum(int val);
	void setYcISendFeq(int val);
	int  getYcISendNum(void);
	void InitDefaultTimer(boost::asio::io_service & io_service);
	void InitDefaultTimeOut(void);

	void ResetTimerYcISend(bool bContinue = false,unsigned short val = 0);

	void handle_timerYcISend(const boost::system::error_code& error);

private:
	CBF533Terminal(boost::asio::io_service & io_service,CSubStation & sub);

private:
	enum BF533Default
	{
		DEFAULT_timeOutYcISend       = 4,
	};

	static CBF533Terminal * instance_;

	stBF533Base dev_para_;
	std::vector<stYCChannel> YcChannelSet_;//stYCChannel YcChannel_[CHANNELMAX];
	std::vector<stProVal> proValSet_; //stProVal proVal_[LINEMAX];
	std::vector<stLinePara> linePara_; //stLinePara linePara[LINEMAX];

	std::vector<int> TerminalYcINum_;

	bool YcISendFlage_;
	int YcISendNum_;
	int timeOutYcISendTime_;
	boost::scoped_ptr<boost::asio::deadline_timer> timerYcISend_;
};

};//namespace DataBase
