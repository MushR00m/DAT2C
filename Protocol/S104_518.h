#pragma once
#include "S104.h"
#include "BF533_DataBase.h"
#include "BF533_CfgFile.h"
#include "BF533.h"

namespace Protocol {


class CS104_518:
	public CS104,
	public CBF533_CfgFile
{
public:
	CS104_518(boost::asio::io_service & io_service);
	~CS104_518(void);

	virtual int LoadXmlCfg(std::string filename);
	virtual void SaveXmlCfg(std::string filename);

	void AddDownloadCmd(share_pristation_ptr pristationPtr);
				
protected:
	virtual int CheckFrameHead(unsigned char * buf,size_t & exceptedBytes);
	virtual int ParseFrameBody(unsigned char * buf,size_t exceptedBytes);

	virtual int AssembleFrameHead(size_t bufIndex, unsigned char * buf, CCmd & cmd);
	virtual int AssembleFrameBody(size_t bufIndex, unsigned char * buf, CCmd & cmd);
	virtual int AssembleFrameTail(size_t bufBegin,size_t bufIndex, unsigned char * buf, CCmd & cmd);

	virtual void ProcessSubYkSig(typeCmd cmdType,unsigned char ReturnCode,share_commpoint_ptr point,boost::any index);
	virtual void ProcessSubAliveSig(typeCmd cmdType,unsigned char ReturnCode,share_commpoint_ptr point,boost::any val);

private:
	void InitFileOpt();
	void InitDefaultTimeOut();
	void InitDefaultTimer(boost::asio::io_service & io_service);

	//para api
	int setJB_EVENT_START_ADDR(size_t val);
	int setJB_PARA_START_ADDR(size_t val);

	//timer
	int setTimeOutReboot(unsigned short val);

	void handle_timerSetJBPara(const boost::system::error_code& error,share_pristation_ptr point);
	void handle_timerValVer(const boost::system::error_code& error,share_commpoint_ptr point);
	void handle_timerReboot(const boost::system::error_code& error,share_commpoint_ptr point);
	void handle_timerCallHVal(const boost::system::error_code& error,share_commpoint_ptr point);
	void handle_timerDownloadPara(const boost::system::error_code& error,share_commpoint_ptr point);
	void handle_timerBCurVer(const boost::system::error_code& error,share_commpoint_ptr point);//����������ֵУ��
	void handle_timerDcVer(const boost::system::error_code& error,share_commpoint_ptr point);//����������ֵУ��	
	void handle_timerCallPm(const boost::system::error_code& error,share_commpoint_ptr point);
	void handle_timerCallFileDisConect(const boost::system::error_code& error,share_commpoint_ptr point);

	void ResetTimerSetJBPara(share_pristation_ptr point,bool bContinue = false,unsigned short val = 0);
	void ResetTimerValVer(share_commpoint_ptr point,bool bContinue = false,unsigned short val = 0);
	void ResetTimerReboot(share_commpoint_ptr point,bool bContinue = false,unsigned short val = 0);
	void ResetTimerCallHVal(share_commpoint_ptr point,bool bContinue = false,unsigned short val = 0);
	void ResetTimerDowloadPara(share_commpoint_ptr point,bool bContinue = false,unsigned short val = 0);
	void ResetTimerBCurVer(share_commpoint_ptr point,bool bContinue = false,unsigned short val = 0);
	void ResetTimerDcVer(share_commpoint_ptr point,bool bContinue = false,unsigned short val = 0);
	void ResetTimerCallPm(share_commpoint_ptr point,bool bContinue = false,unsigned short val = 0);
	void ResetTimerCallFileDisConect(share_commpoint_ptr point,bool bContinue = false,unsigned short val = 0);


	//send frame assemble
    int AssembleLineValVerSucess(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd);
	int AssembleHarmonicCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd);
	int AssembleCallValCoefCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd);
	int AssembleSendFlieCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr);
	int AssembleSendFlieBodyCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd);
	int AssembleCallFlieNameCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd);
	int AssembleCallFlieBody(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd);//�����ļ���
	int AssembleRebootCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr);
	int AssembleSignalResetCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd);
	int AssembleCallTimeCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,boost::posix_time::ptime time);
	int AssembleDownLoadLineHValCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd);
	int AssembleBoardInq(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd);
	int AssembleBattryActiveCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr);
	int AssembleBattryActiveOverCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr);
	int AssembleDownLoadParaCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd);
	int AssembleLineBValVer(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd);
	int AssembleDcValVerSucess(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd);
	int AssembleCallPMCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd);
	int AssembleCallCRCCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr);
	int AssembleCpuVerInqCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd);
	int AssembleCallJBParaCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,size_t info_num,int startIndex,CCmd & cmd);
	int AssembleSendJBParaCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,size_t info_num);
	int AssembleActJBParaCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr);
	int AssembleDeactJBParaCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr);
	int AssembleJBSignalResetCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr);
	int AssemJBSOE(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,size_t info_num,CCmd & cmd);
		
	//recv frame parse
	int ParseFrame_I(unsigned char * buf,size_t exceptedBytes);
	int ParseFileFrame(unsigned char * buf, share_pristation_ptr pristationPtr);
	int ParseCallJBPara(unsigned char * buf,share_pristation_ptr pristationPtr);
	int ParseSendJBPara(unsigned char * buf,share_pristation_ptr pristationPtr);
	int ParseActALLPara(unsigned char * buf,share_pristation_ptr pristationPtr);
	int ParseDeacALLtPara(unsigned char * buf,share_pristation_ptr pristationPtr);
	int ParseSignalReset(unsigned char * buf,share_pristation_ptr pristationPtr);
	int ParseLineValVer(unsigned char * buf,share_pristation_ptr pristationPtr);//��ֵУ��
	int ParseHarmonicAck(unsigned char * buf,share_pristation_ptr pristationPtr);
	int ParseSendFileAck(unsigned char * buf,share_pristation_ptr pristationPtr);//�����ļ�
	int ParseRecvFile(unsigned char * buf,share_pristation_ptr pristationPtr);//�����ļ�����
	int ParseCallFileAck(unsigned char * buf,share_pristation_ptr pristationPtr);//�ٻ��ļ�����
	int ParseCallVallCoef(unsigned char * buf,share_pristation_ptr pristationPtr);//�ٻ���ֵϵ��
	int ParseRbootAck(unsigned char * buf,share_pristation_ptr pristationPtr);//װ�ø�λ
	int ParseDownLoadLineHVal(unsigned char * buf,share_pristation_ptr pristationPtr);//��װ��ֵϵ��
	int ParseCpuVerReq(unsigned char * buf,share_pristation_ptr pristationPtr);//��װ��ֵϵ��
	int ParseBValVer(unsigned char * buf,share_pristation_ptr pristationPtr);//��װ��������У�鶨ֵ
	int ParseDcValVer(unsigned char * buf,share_pristation_ptr pristationPtr);//ֱ������ֵУ��
	int ParseCallPM(unsigned char * buf,share_pristation_ptr pristationPtr);//�ٻ��������
	int ParseCallCRC(unsigned char * buf,share_pristation_ptr pristationPtr);
	int ParseRemoteSingalReset(unsigned char * buf,share_pristation_ptr pristationPtr);

	//protocol func api
	void ReadProtectValue();
	void WriteProtectValue();
	int Read_ProtectValPara();
	int Write_ProtectValPara();

	int getAddrByRecvFrame(unsigned char * buf);
		
private:
	enum s104Para
	{
        //Ĭ����ʷ������ʼ��ַ
		DEFAULT_JB_EVENT_START_ADDR = 0x2001,                         //Ĭ�ϼ̱��¼���ʼ��ַ
		DEFAULT_JB_PARA_START_ADDR = 0x3001,                         //Ĭ�ϼ̱���ֵ��ʼ��ַ
		DEFAULT_JB_PARA_VALUE_START_ADDR = 0x3051,                         //Ĭ�ϼ̱���ֵ��ʼ��ַ
		DEFAULT_JB_EVENT_NUM       = 20,                         //Ĭ�ϼ̱�
		DEFAULT_JB_PARA_NUM    = 150,                         //Ĭ�ϼ̱�
		DEFAULT_timeOutCallPara = 15,
		DEFAULT_timeOutSetPara = 25,
		DEFAULT_timeOutSetJBPara = 60,
		DEFAULT_timeOutCallFileDisConect = 30, //�ٻ���ض�ֵʱ�Ͽ��źŵĳ�ʱʱ��
		DEFAULT_timeOutValVer = 5,
		DEFAULT_timeOutReboot = 3,             //3s֮��λ
		DEFAULT_timeOutCallHVal = 5,
		DEFAULT_timeOutDownloadPara = 60,
		DEFAULT_timeOutBCurVer = 8,
		DEFAULT_timeOutDcVer = 8,
		DEFAULT_timeOutCallPm = 8,
		DEFAULT_Eaqul_Flag          = false
			
	};

	size_t JB_EVENT_START_ADDR_;
	size_t JB_PARA_START_ADDR_;
	size_t JB_PARA_VALUE_START_ADDR_;
	size_t JB_EVENT_NUM_;
	size_t JB_PARA_NUM_;

	unsigned short timeOutValVer_;
	timerPtr ValVerTimer_;
	unsigned short timeOutReboot_;
	timerPtr RebootTimer_;
	unsigned short timeOutCallHVal_; //�ٻ���ֵϵ��
	timerPtr CallHvalTimer_;
	unsigned short timeOutDownloadPara_;
	timerPtr timerDownloadPara_;
	unsigned short timeOutBCurVer_;
	timerPtr timerBCurVer_;
	unsigned short timeOutDcVer_;
	timerPtr timerDcVer_;
	unsigned short timeOutCallPm_;
	timerPtr timerCallPm_;

	unsigned short timeOutSetJBPara_;
	timerPtr timerSetJBPara_;
	unsigned short timeOutCallFileDisConect_;
	timerPtr timerCallFileDisConect_;

public:
	DataBase::stBF533Base BF533Base;
	DataBase::stProVal ProVal[10];

	int This_Length_;
	int Total_NUM;
	int This_N0;
	int JB_Line_No_;
	int JB_Para_No_;
	int JB_Para[28][100];
	int JB_Parasave[50];
	int JB_Event[100];
	int JB_Event_Original[100];

	int JB_Paratab[200];
	int JB_Paratab_Original[100];
	int JB_Tabnum[100];
};

};//namespace Protocol

