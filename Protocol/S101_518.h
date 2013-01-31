#pragma once
#include "S101.h"
#include "BF533_CfgFile.h"

namespace Protocol{

class CS101_518:
	public CS101,
	public CBF533_CfgFile
{
public:
	CS101_518(boost::asio::io_service & io_service);
	~CS101_518(void);

	virtual int LoadXmlCfg(std::string filename);
	virtual void SaveXmlCfg(std::string filename);

	void SaveFile(share_pristation_ptr pristationPtr);
	int  GetFileData(share_pristation_ptr pristationPtr);
	void AddDownloadCmd(share_pristation_ptr pristationPtr);

protected:
	virtual int CheckFrameHead(unsigned char * buf,size_t & exceptedBytes);
	virtual int CheckFrameTail(unsigned char * buf,size_t exceptedBytes);
	virtual int ParseFrameBody(unsigned char * buf,size_t exceptedBytes);

	virtual int AssembleFrameHead(size_t bufIndex, unsigned char * buf, CCmd & cmd);
	virtual int AssembleFrameBody(size_t bufIndex, unsigned char * buf, CCmd & cmd);
	virtual int AssembleFrameTail(size_t bufBegin, size_t bufIndex, unsigned char * buf, CCmd & cmd);

	virtual void ProcessSubYkSig(typeCmd cmdType,unsigned char ReturnCode,share_commpoint_ptr point,boost::any index);
	virtual void ProcessSubAliveSig(typeCmd cmdType,unsigned char ReturnCode,share_commpoint_ptr point,boost::any val);

	int getAddrByRecvFrame(unsigned char * buf);

private:
	void InitFileOpt();
	void InitDefaultTimer(boost::asio::io_service & io_service);
	void InitDefaultTimeOut();

	//int setTimeOutReboot(unsigned short val);

	void handle_timerValVer(const boost::system::error_code& error,share_commpoint_ptr point);
	void handle_timerReboot(const boost::system::error_code& error,share_commpoint_ptr point);
	void handle_timerCallHVal(const boost::system::error_code& error,share_commpoint_ptr point);
	void handle_timerDownloadPara(const boost::system::error_code& error,share_commpoint_ptr point);
	void handle_timerBCurVer(const boost::system::error_code& error,share_commpoint_ptr point);//����������ֵУ��
	void handle_timerDcVer(const boost::system::error_code& error,share_commpoint_ptr point);//����������ֵУ��
	void handle_timerCallPm(const boost::system::error_code& error,share_commpoint_ptr point);
	void handle_timerCallFileDisConect(const boost::system::error_code& error,share_commpoint_ptr point);

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
	int AssembleCpuVerInqCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd);
	int AssembleLineBValVer(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd);
	int AssembleDcValVerSucess(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd);
	int AssembleCallPMCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd);
	int AssembleCallCRCCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr);

	//recv frame parse
	int ParseLongFrame(unsigned char * buf, share_pristation_ptr pristationPtr,size_t exceptedBytes);
	
	int ParseFileFrame(unsigned char * buf, share_pristation_ptr pristationPtr);
	int ParseLineValVer(unsigned char * buf,share_pristation_ptr pristationPtr);//��ֵУ��
	int ParseHarmonicAck(unsigned char * buf,share_pristation_ptr pristationPtr);
	int ParseSendFileAck(unsigned char * buf,share_pristation_ptr pristationPtr);//�����ļ�
	int ParseRecvFile(unsigned char * buf,share_pristation_ptr pristationPtr);//�����ļ�����
	int ParseCallFileAck(unsigned char * buf,share_pristation_ptr pristationPtr);//�ٻ��ļ�����
	int ParseCallVallCoef(unsigned char * buf,share_pristation_ptr pristationPtr);//�ٻ���ֵϵ��
	int ParseRbootAck(unsigned char * buf,share_pristation_ptr pristationPtr);//װ�ø�λ
	int ParseDownLoadLineHVal(unsigned char * buf,share_pristation_ptr pristationPtr);//��װ��ֵϵ��
	int ParseCpuVerReq(unsigned char * buf,share_pristation_ptr pristationPtr);//
	int ParseBValVer(unsigned char * buf,share_pristation_ptr pristationPtr);//��װ��������У�鶨ֵ
	int ParseDcValVer(unsigned char * buf,share_pristation_ptr pristationPtr);//ֱ������ֵУ��
	int ParseCallPM(unsigned char * buf,share_pristation_ptr pristationPtr);//�ٻ��������
	int ParseCallCRC(unsigned char * buf,share_pristation_ptr pristationPtr);//�ٻ�CRCУ����
	int ParseRemoteSingalReset(unsigned char * buf,share_pristation_ptr pristationPtr);

public:
	//int FileLength_;
	int This_Length_;
	int Total_NUM;
	int This_N0;

private:
	enum s101_518Para
	{
		DEFAULT_timeOutCallFileDisConect = 30, //�ٻ���ض�ֵʱ�Ͽ��źŵĳ�ʱʱ��
		DEFAULT_timeOutValVer = 5,
		DEFAULT_timeOutReboot = 3,             //3s֮��λ
		DEFAULT_timeOutCallHVal = 5,
		DEFAULT_timeOutDownloadPara = 60,
		DEFAULT_timeOutBCurVer = 8,
		DEFAULT_timeOutDcVer = 8,
		DEFAULT_timeOutCallPm = 8,
	};

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
	unsigned short timeOutCallFileDisConect_;
	timerPtr timerCallFileDisConect_;
};

};//namespace Protocol

