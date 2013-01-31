#pragma once
#include "Protocol.h"
#include "BF533_CfgFile.h"

namespace Protocol {

	class CBF533
		:public CProtocol,
		public CBF533_CfgFile
	{
	public:
		CBF533(boost::asio::io_service & io_service);
		virtual ~CBF533(void);

		virtual int LoadXmlCfg(std::string filename);
		virtual void SaveXmlCfg(std::string filename);



		virtual int CheckFrameHead(unsigned char * buf,size_t & exceptedBytes);
		virtual int CheckFrameTail(unsigned char * buf,size_t exceptedBytes);
		virtual int ParseFrameBody(unsigned char * buf,size_t exceptedBytes);                  //����ͨѶ�ڵ��������е����

		virtual int AssembleFrameHead(size_t bufIndex, unsigned char * buf, CCmd & cmd);
		virtual int AssembleFrameBody(size_t bufIndex, unsigned char * buf, CCmd & cmd);
		virtual int AssembleFrameTail(size_t bufBegin, size_t bufIndex, unsigned char * buf, CCmd & cmd);

		virtual int InitProtocol();// ��ʼ����Լ


	private:
#define SYN_TIME         1200   //�Զ����Ͷ�ʱ����ʱ�䣬��λs
#define BodyAddr         1      //����У���ʱ����ʼ��ַ
#define CHANNELMAX       40     //���ͨ������
#define LINEMAX          10     //�����·��

#define BF518Ver         133    //BF518�汾�� 1.33
#define DSPVer           133    //DSP�汾�� 1.33

#define TimerError        "My God!"

	public:
		size_t SOEStroeNum_ ;      //�洢SOE���������
		bool   SOEStroeFlag_;
		size_t FaultRecordMax_ ;     //�洢�����¼��������
		bool   FaultRecordFlag_;
		char   YMDataRecordType_;       //���ֵ�ȵ�ͳ�Ʒ�ʽ
		bool   YMDataFlag_ ;
		size_t YMDataRecordNum_;        //���ֵ�ȵļ�¼�������
		size_t YMDataCount_;            //ÿһ�μ�¼���������
		size_t DcChannelNo_;

		bool AutoDownloadCfgFlag_;
		std::string AutoDownloadCfgType_;

		unsigned short  UMaxLimit_;
		unsigned short  UMinLimit_;
		unsigned short  IMaxLimit_;
		unsigned short  IMinLimit_;
		unsigned short  ResetYcOverIndex_;
		unsigned short  NoCurDeadVal_;
		bool            NoCurRecord_;
		bool            YcOverAlarm_;






	public:

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
			unsigned short     U_SwitchFlag;//��ѹ�л�Ͷ����
			unsigned short     PT_L_Arlam;//PT����Ͷ����
		} BF533Base;	

		struct stBF533HBase     //װ�ò����ٻ�ʱ�ṹ��
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
			unsigned short     HuoHua_Day  ;   // �Զ�������������
			unsigned short     Pro_Rst_Time;//���������Զ�����ʱ��     ZHANGZHIHUA
			unsigned short     Flag_Pro_Rst;//���������Զ�����Ͷ�˱�־ ZHANGZHIHUA
			unsigned int       I_Rated;       //�����
            unsigned short     U_SwitchFlag;//��ѹ�л�Ͷ����
			unsigned short     PT_L_Arlam;//PT����Ͷ����
		} BF533HBase;	

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
		}ExtYcVal[LINEMAX];

		struct  stYCChannel//ͨ��������װʱ���ýṹ��
		{
			unsigned short Type    ;   //ͨ������ 0����ѹ 1������ 2��ֱ�� 3��CVT
			unsigned int para[2]   ;   //ͨ��У�����
		}YcChannel[CHANNELMAX];

		struct  stHYCChannel//ͨ��������װʱ���ýṹ��
		{
			unsigned short Type    ;   //ͨ������ 0����ѹ 1������ 2��ֱ�� 3��CVT
			unsigned int para[2]   ;   //ͨ��У�����
		}HYcChannel[CHANNELMAX];


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
		}ProVal[LINEMAX];

		struct stProVal HProVal[LINEMAX];


		struct  stLinePara       //��·ͨ����Ϲ�ϵ
		{
			unsigned short Vol_Chan[3]   ;  //��ѹͨ��
			unsigned short Cur_Chan[3]   ;  //����ͨ��
			unsigned short BCur_Chan[3]; //��������ͨ��
			unsigned short U0_Chan   ;  //�����ѹͨ��
			unsigned short I0_Chan   ;  //�������ͨ��	

			int            YxStart; //��ң����ʼ��ַ
			unsigned short YkNo;//YK���back
			unsigned short IO_Flage;//��·�Ƿ�Ϊ���߱�־
		}LinePara[LINEMAX];


		struct  stHLinePara       //��·ͨ����Ϲ�ϵ���ٻ���
		{
			unsigned short Vol_Chan[3]   ;  //��ѹͨ��
			unsigned short Cur_Chan[3]   ;  //����ͨ��
			unsigned short BCur_Chan[3]   ;  //��������ͨ��
			unsigned short U0_Chan   ;  //�����ѹͨ��
			unsigned short I0_Chan   ;  //�������ͨ��	

			int            YxStart; //��ң����ʼ��ַ
			unsigned short YkNo;//YK���back
			unsigned short IO_Flage;//��·�Ƿ�Ϊ���߱�־
		}HLinePara[LINEMAX];

		struct YMDATA
		{
			double YMDataVal;
		}YMData[LINEMAX];

		struct stNoCurVal 
		{
            unsigned short Ua;
			unsigned short Ub;
			unsigned short Uc;
			unsigned short Ia;
			unsigned short Ib;
			unsigned short Ic;
			bool           NoCurFlage;
			bool           Author_;
		}NoCurVal[LINEMAX];

		struct stYcOverVal 
		{
			bool            YcOveredFlage_;
		}YcOverVal[LINEMAX];


	private:
		//��ȡ�ļ�����ӿ�
		int Read_BasePara(void);
		int Read_DevSanyaoPara(void);
		int Read_ChanneltypePara(void);
		int Read_ProtectValPara(void);
		int Read_InterfacePara(void);
		int Read_LinePara(void);
		int Read_Master1YcZFTab(void);
		int Read_Master1YxZFTab(void);
		int Read_Master2YcZFTab(void);
		int Read_Master2YxZFTab(void);
		int LoadRecodFileCfg(void);
		//д���ļ��ӿ�
		int Write_BasePara(void);//�����յ���װ�ò������Ľ�����װ�����ͺͰ汾�Ŵ���XML�ļ�
		int Write_DevSanyaoPara(void);//�����յ���װ�ò������Ľ����������װ�ò�������XML�ļ�
		int Write_ChanneltypePara(void);//�����յ���ͨ�����ͱ��Ľ�ͨ�����ʹ���XML�ļ�
		int Write_ProtectValPara(void);//�����յ��ı�����ֵ�����������ֱ��Ĵ���XML�ļ�
		int Write_LinePara(void);//�����յ���ͨ����Ϲ�ϵ���Ľ�ͨ����Ϲ�ϵ����XML�ļ�
		int Write_InterfacePara(void);

		void XML_Text(void);
		void XmlReadTest(void);
		void XmlWriteTest(void);

		int SaveHistoricalData(share_terminal_ptr terminalPtr);
		int SaveExtremum(share_terminal_ptr terminalPtr);
		int SaveYMData(int LineNum);
		int SaveFaultRecord(unsigned char * buf);
		int SaveSOERecord(int No,unsigned char Val,unsigned long Time);
		int SaveNoCurRecord(unsigned short LineNo);

		int SetBF518Para(void);
		int ModfiySetipSh(char Eth1IP[24],char Eth2IP[24],char Eth1MASK[24],char Eth2MASK[24]);
		std::string TransDoubleToString(double Data); //�������double����ת��Ϊ�ַ�������
		std::string TransIntToString(int Data); //�������int����ת��Ϊ�ַ�������
		char TransStringToDate(std::string Str);//�������ļ��е��ַ���ת����ʱ���־
		bool TransStringTobool(std::string Str);
		unsigned int  TransStringToInt(std::string Str);//���ļ����ʮ��������ת����ʮ����
		std::string TransIntToHexString(unsigned int Data);//���յ�����ֵת����ʮ�����Ʊ��ʾ���ַ���

		void setAutoDownloadCfgVal(std::string val);
		std::string getAutoDownloadCfgVal(void);
		void AutoDownloadCfg(share_terminal_ptr terminalPtr);

		void setUMaxLimit(unsigned short val);
		void setUMinLimit(unsigned short val);
		void setIMaxLimit(unsigned short val);
		void setIMinLimit(unsigned short val);
		void setResetYcOverIndex(unsigned short val);
		void setNoCurDeadVal(unsigned short val);
		//void setYcOveredFlage(bool val);
		unsigned short getUMaxLimit(void);
		unsigned short getUMinLimit(void);
		unsigned short getIMaxLimit(void);
		unsigned short getIMinLimit(void);
		unsigned short getResetYcOverIndex(void);
		unsigned short getNoCurDeadVal(void);
		//bool getYcOveredFlage(void);

	private:
		//���ķ��ʹ�����
		int AssembleCallAllData(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr);//��װ���ٱ���
		int AssembleCallEquPara(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr);//�ٻ�װ�ò���
		int AssembleSingleYKExe(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr,unsigned short yk_no, unsigned char yk_code);   //�·�ң��ִ��ָ��
		int AssembleYkSel(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr);//��ң��ѡ��ָʾ��
		int AssembleYkCancel(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr);//��ң��ѡ��ָʾ��
		int AssembleDownEquPara(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr);//��װװ�ò���
		int AssembleDownEquParaSucess(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr);//��װװ�ò����ɹ�
		int AssembleDownDelayVal(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr,int line_no);//��װ������ֵ
		int AssembleDownDelayValSucess(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr);//��װ������ֵ�ɹ�
		int AssembleDownDelayContrl(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr,int line_no);//��װ����������
		int AssembleDownDelayContrlSucess(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr);//��װ���������ֳɹ�
		int AssembleSignalRest(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr,int line_no);//�źŸ���
		int AssembleSynTime(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr,boost::posix_time::ptime time);//�㲥��ʱ
		int AssembleDownChannelPara(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr);//��װͨ������
		int AssembleDownChannelParaSucess(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr);//��װͨ�������ɹ�
		int AssembleDownChannelComrel(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr);//��װͨ����Ϲ�ϵ
		int AssembleDownChannelComrelSucess(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr);//��װͨ����Ϲ�ϵ�ɹ�
		int AssembleCallLineData(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr);//�ٻ���·����
		int AssembleDownLineVal(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr,CCmd & cmd);//��װ��·��ֵ
		int AssembleLineValVer(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr);//��·��ֵУ��
		int AssembleDspVersionInq(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr);//DSP�汾��ѯ
		int AssembleHarmonicAck(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr,CCmd & cmd);
		int AssembleCallValCoef(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr,CCmd & cmd);//�ٻ���ϵ��
		int AssembleDownLoadLineHVal(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr,CCmd & cmd);
		int AssembleBoardInquiry(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr);
		int AssembleCallProVal(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr);
		int AssembleCallChType(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr);
		int AssembleCallLinePara(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr);
		int AssembleCallYMDataAck(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr);
		int AssembleBatteryActiveAck(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr);
		int AssembleBatteryActiveOverAck(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr);//��ػ�˳�
		int AssembleDownLineBVal(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr,CCmd & cmd);//��װ��·��������У�鶨ֵ
		int AssembleLineValBVer(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr);//��·����������ֵУ��
		int AssembleDcValBVer(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr,CCmd & cmd);
		int AssembleCallPm(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr,CCmd & cmd);
		int AssembleRebootBf533(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr);
		int AssembleTbootBf533(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr);
		int AssembleEraseBf533(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr);
		int AssembleDownBf533Pro(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr,CCmd & cmd);


		//���Ľ��մ�����
	private:
		int getAddrByRecvFrame(unsigned char * buf);
		int ParseLongFrame(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);
		int ParseSingleYKSelCon(unsigned char * buf, share_terminal_ptr terminalPtr);
		int ParseSingleYKExeCon(unsigned char * buf, share_terminal_ptr terminalPtr);//YK ִ��ȷ��
		int ParseSingleYKCancelCon(unsigned char * buf, share_terminal_ptr terminalPtr);
		int ParseDownloadEquCeck(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);//װ�ò�����װ��У
		int ParseDownloadDelayValCeck(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);//������ֵ��װ��У
		int ParseDownloadDelayContrlCheck(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);//������������װ��У
		int ParseDownloadChannelParaCheck(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);//ͨ��������װ��У
		int ParseDownloadChannelComrelCheck(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);//ͨ����Ϲ�ϵ��װ��У
		int ParseDownloadLineValCheck(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);//��·��ֵ��װ��У
		int ParseDownloadLineCoefCheck(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);//��·ͨ��ϵ����װ��У
		int ParseAllYCData(unsigned char * buf, share_terminal_ptr terminalPtr);//ȫң�ⱨ��
		int ParseLineCycTime(unsigned char * buf, share_terminal_ptr terminalPtr);//����·ȫң�ⶨʱ���ͱ���
		int ParseAllYxCon(unsigned char * buf, share_terminal_ptr terminalPtr);//ȫң�Ųⱨ��
		int ParseSaveAllYX(int info_addr,share_terminal_ptr terminalPtr,unsigned char Data,unsigned char div,int offset);//�洢ȫң�ű���
		int ParseSingleSOE(unsigned char * buf, share_terminal_ptr terminalPtr);//����SOE����
		int ParseLineValVerQyc(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);//����ֵУ���ȫң��
		int ParseHarmonicCon(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes); //�ٻ�г���ظ�
		int ParseDownloadDownDelayValSucess(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);
		int ParseDownloadDownDelayContrlSucess(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);
		int ParseCallValCoefCon(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);
		int ParseDownLoadLineCoef(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);
		int ParseBoardInqCon(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);
		int ParseCallEquCon(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);
		int ParseCallEquSucess(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);
		int ParseCallProValCon(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);
		int ParseCallProVal(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);
		int ParseCallProContrl(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);
		int ParseCallProValSucess(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);
		int ParseCallChTypeSucess(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);
		int ParseCallChTypeAck(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);
		int ParseCallLinePara(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);
		int ParseCallLineParaSucess(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);
		int ParseCallYMDataCon(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);
		int ParseDownloadLineBValCheck(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);//��·��������У�鶨ֵ��װ��У
		int ParseLineBValVer(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);//����ֵУ���ȫң��
		int ParseFaultEvent(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);
		int ParseCycDcIT(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);//ֱ�������¶ȶ�ʱ����
		int ParseDcValVerSucess(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);
		int ParseCallPm(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);
		int ParseSingalReset(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);
		int ParseCallDspVersion(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);
		int ParseRebootBf533(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);
		int ParseTbootBf533(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);
		int ParseEraseBf533(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);
		int ParseDownBf533Pro(unsigned char * buf, share_terminal_ptr terminalPtr,size_t exceptedBytes);

		int YcValOverAlarm(unsigned char * buf, share_terminal_ptr terminalPtr);
		int RecordLineLoad(unsigned char * buf, share_terminal_ptr terminalPtr);

		//��ʱ��������
	private:
		void InitDefaultTimer(boost::asio::io_service & io_service);

		//	void ResetTimerCallAllData (share_commpoint_ptr point,bool bContinue = true,unsigned short val = 0);
		void ResetTimerSynTime(share_commpoint_ptr point,bool bContinue = true,unsigned short val = 0);
		void ResetTimerYkExe(share_commpoint_ptr point,int yk_no,bool bContinue = true ,unsigned short val = 0);
		void ResetTimerYkCancel(share_commpoint_ptr point,size_t yk_no,bool bContinue = true ,unsigned short val = 0);
		void ResetTimerChannelError(share_commpoint_ptr point,bool bContinue = true,unsigned short val = 0);
		void ResetTimerSaveHis(share_terminal_ptr point,bool bContinue = true,unsigned short val = 0);
		void ResetTimerCallYMData(share_terminal_ptr point,bool bContinue = true,unsigned short val = 0);
		void ResetTimerAllData(share_terminal_ptr point,bool bContinue = true,unsigned short val = 0);
		void ResetTimerBattryActive(share_terminal_ptr point,bool bContinue = true,unsigned short val = 0);
		void ResetTimerBattryActiveOver(share_terminal_ptr point,bool bContinue = true,unsigned short val = 0);	
		void ResetTimerReboot(share_commpoint_ptr point,bool bContinue = false,unsigned short val = 0);
		void ResetTimerSingalReset(share_commpoint_ptr point,bool bContinue = false,unsigned short val = 0);
		void ResetTimerRebootBf533(share_terminal_ptr terminalPtr,bool bContinue = false,unsigned short val = 0);
		void ResetTimerTBootBf533(share_terminal_ptr terminalPtr,bool bContinue = false,unsigned short val = 0);

		//	void handle_timerCallAllDataTime(const boost::system::error_code& error,share_commpoint_ptr point);
		void handle_timerSynTime(const boost::system::error_code& error,share_commpoint_ptr point);
		void handle_timerYkExe(const boost::system::error_code& error,share_commpoint_ptr point,int yk_no);
		void handle_timerChannelError(const boost::system::error_code& error,share_commpoint_ptr point);
		void handle_timerYkCancel(const boost::system::error_code& error,share_commpoint_ptr point,size_t yk_no);
		void handle_timerSaveHis(const boost::system::error_code& error,share_terminal_ptr point);
		void handle_timerCallYMData(const boost::system::error_code& error,share_terminal_ptr point);
		void handle_timerAllData(const boost::system::error_code& error,share_terminal_ptr point);
		void handle_timerBattryActive(const boost::system::error_code& error,share_terminal_ptr point);
		void handle_timerBattryActiveOver(const boost::system::error_code& error,share_terminal_ptr point);
		void handle_timerReboot(const boost::system::error_code& error,share_commpoint_ptr point);
		void handle_timerSingalReset(const boost::system::error_code& error,share_commpoint_ptr point);
		void handle_timerRebootBf533(const boost::system::error_code& error,share_terminal_ptr terminalPtr);
		void handle_timerTBootBf533(const boost::system::error_code& error,share_terminal_ptr terminalPtr);

		//��ʼ����غ���
		void InitDefaultFrameElem();
		void InitDefaultTimeOut();
		void InitDefaultConfig();
		void InitDefaultFileData();//��ʼ���ļ��洢���Ĭ�ϲ���
		void InitDefaultPara();
		void InitDefaultBF533Download();

		//��ӡ��Ϣ�ӿ�
		void PrintMessage(const char PrintBuf[]);

	private:
		void setBf533ProName(std::string str);
		void setBf533AutoloadFlage(bool flage);
		bool getBf533AutoloadFlage(void);
		std::string getBf533ProName(void);

		//BF533����
	private:
		unsigned short FrameTypeLocation_;                           //�������ͱ�ʶ���ֽڶ�λ
		unsigned short FrameTypeLength_;                             //�������ͱ�ʶ���ֽڳ���

	public:
		int  Line_No_ ;
		bool    FristFlag;
		size_t  SendTimes;
		size_t  Check_Sum;
		unsigned char Check_BF533Sum;
		size_t  ReSendTime;
		unsigned int Line_NUM;
		bool    bAssembleDelayValAll_;
		bool    bAllowTBoot_;
		bool    bAllowReboot_;
		bool    bYcAddRand_;//YCֵ�������
	private:
		bool    bBF533AutoDownload_;
		std::string BF533ProName;
		int This_Length_;
		int Total_NUM;
		int This_N0;

		//��ʱ��������
	private:
		//unsigned short timeOutCallAllDataTime_;
		//timerPtr CallAllDataTimer_;
		unsigned short timeOutSynTime_;
		timerPtr timerSynTime_;
		unsigned short timeOutYkExe_;
		timerPtr timerYkExe_;
		unsigned short timeOutChannelError_;
		timerPtr ChannelErrorTimer_;           //�ж�ͨ���Ƿ�������������������
		unsigned short timeOutYkCancel_;
		timerPtr YkCancelTimer_;           //�ж�ң��ѡ��֮���Ƿ��յ�ң��ִ�У���ʱ���Զ�ȡ��ң��
		unsigned short timeOutSaveHis_;
		timerPtr SaveHisTimer_; 
		unsigned short timeOutCallYMData_;
		timerPtr CallYMDataTimer_;            //��ʱ�ٻ����ֵ
		unsigned short timeOutAllData_;
		timerPtr AllDataTimer_;            //��ʱ�ٻ����ֵ
		unsigned short timeOutBattryActive_;
		timerPtr BattryActiveTimer_;            //��ʱ������ػ
		unsigned short timeOutBattryActiveOver_;
		timerPtr BattryActiveOverTimer_;            //��ʱ������ػ�˳�
		unsigned short timeOutSingalReset_;
		timerPtr SingalResetTimer_;            //��ʱ������ػ�˳�
		unsigned short timeOutRebootBf533_;
		timerPtr ResetRebootBf533Timer_;
		unsigned short timeOutTBootBf533_;
		timerPtr ResetTBootBf533Timer_;      

		//��ʱ����ر���
	private:
		enum BF533Default
		{
			//DEFAULT_timeOutCallAllDataTime = 5,
			DEFAULT_timeOutYkCancel        = 10,
			DEFAULT_timeOutSaveHis         = 300,//300��
			//MIN_timeOutCallAllData         = 1,
			DEFAULT_timeOutSynTime         = 1800,
			MIN_timeOutSynTime             = 300,
			DEFAULT_timeOutYkExe           = 5,
			DEFAULT_timeOutChannelError    = 10,
			MIN_timeOutChannelError        = 2,
			DEFAULT_timeOutCallYMData      = 300,   //�����ٻ����洢������� 300��
			DEFAULT_timeOutAllData         = 10,
			DEFAULT_SOEStroeNum            = 40,
			DEFAULT_timeOutReboot          = 3,             //3s֮��λ
			DEFAULT_FaultRecordMax         = 20,
			DEFAULT_LineNum                = 10,//Ĭ����·��������Ҫ��Ϊ���ֱ����������ַ
			DEFAULT_timeOutBattryActive    = 480,//7*24  //��ػ����ʱ�䣬��λΪСʱ,20��
			DEFAULT_timeOutBattryActiveOver    = 120, //����Զ���˳�ʱ�䣬��λΪ��
			DEFAULT_timeOutSingalReset         = 3,
			DEFAULT_timeOutResetYcOver         = 10,    //ң��Խ�޸澯����
			DEFAULT_timeOutRebootBf533         = 3,
			DEFAULT_timeOutTBootBf533          = 100
		};

	};//BF533

}; //Protocol 
