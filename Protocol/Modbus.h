#pragma once
#include "Protocol.h"
#include "ModbusCMD.h"

namespace Protocol {

class CModbus:
	public CProtocol
{
public:
	CModbus(boost::asio::io_service & io_service);
	virtual ~CModbus(void);

	virtual int LoadXmlCfg(std::string filename);
	virtual void SaveXmlCfg(std::string filename);

protected:
	//frame api
	virtual int CheckFrameHead(unsigned char * buf,size_t & exceptedBytes);
	virtual int CheckFrameTail(unsigned char * buf,size_t exceptedBytes);
	virtual int ParseFrameBody(unsigned char * buf,size_t exceptedBytes);

	virtual int AssembleFrameHead(size_t bufIndex, unsigned char * buf, CCmd & cmd);
	virtual int AssembleFrameBody(size_t bufIndex, unsigned char * buf, CCmd & cmd);
	virtual int AssembleFrameTail(size_t bufBegin, size_t bufIndex, unsigned char * buf, CCmd & cmd);
	
	virtual int InitProtocol();
	virtual void UninitProtocol();

	//comm points api
	//virtual int AddCommPoint(share_commpoint_ptr val);
	//virtual int AddCommPoint(weak_commpoint_ptr val);
	//virtual int DelCommPoint(int index);

	//xml api
	int LoadModbusCmd(FileSystem::CMarkup & xml,stModbusCmd & cmd);
	void SaveModbusCmd(FileSystem::CMarkup & xml,stModbusCmd & cmd);

private:
	void InitObjectIndex();
	void InitDefaultFrameElem();
	void InitFrameLocation(size_t FrameHead);
	void InitDefaultTimeOut();
	void InitDefaultTimer(boost::asio::io_service & io_service);
	void InitDefaultDefPara();

	unsigned short getCurMessageNO();
	int getCmdParaIndexByAddr(unsigned short addr);

	//timer
	void handle_timerSynTime(const boost::system::error_code& error,share_commpoint_ptr point);
	void handle_timerHeartFrame(const boost::system::error_code& error,share_commpoint_ptr point);
	void handle_timerYkExe(const boost::system::error_code& error,share_commpoint_ptr point,size_t yk_no);

	void ResetTimerSynTime(share_commpoint_ptr point,bool bContinue = true,unsigned short val = 0);
	void ResetTimerHeartFrame(share_commpoint_ptr point,bool bContinue = true,unsigned short val = 0);
	void ResetTimerYkExe(share_commpoint_ptr point,size_t yk_no,bool bContinue = false,unsigned short val = 0);

	//frame api
	int CheckFrameHead_RTUMode(unsigned char * buf,size_t & exceptedBytes);
	int CheckFrameHead_TCPMode(unsigned char * buf,size_t & exceptedBytes);
	int CheckFrameTail_RTUMode(unsigned char * buf,size_t exceptedBytes);
	int CheckFrameTail_TCPMode(unsigned char * buf,size_t exceptedBytes);

	int AssembleFrameHead_RTUMode(size_t bufIndex, unsigned char * buf, CCmd & cmd);
	int AssembleFrameHead_TCPMode(size_t bufIndex, unsigned char * buf, CCmd & cmd);
	int AssembleFrameTail_RTUMode(size_t bufBegin, size_t bufIndex, unsigned char * buf, CCmd & cmd);
	int AssembleFrameTail_TCPMode(size_t bufBegin, size_t bufIndex, unsigned char * buf, CCmd & cmd);
	int AssembleSynTime(size_t bufIndex, unsigned char * buf, stModbusCmd modbusCmd, boost::posix_time::ptime time);
	int AssembleQueryData(size_t bufIndex, unsigned char * buf, stModbusCmd modbusCmd);
	int AssembleYKExe(size_t bufIndex, unsigned char * buf, stModbusCmd modbusCmd, unsigned short yk_code);

	int ParseReadDI(unsigned char * buf,share_terminal_ptr terminalPtr);
	int ParseReadDO(unsigned char * buf,share_terminal_ptr terminalPtr);
	int ParseReadAI(unsigned char * buf,share_terminal_ptr terminalPtr);
	int ParseReadAO(unsigned char * buf,share_terminal_ptr terminalPtr);
	int ParseWriteDO(unsigned char * buf,share_terminal_ptr terminalPtr);
	int ParseWriteAO(unsigned char * buf,share_terminal_ptr terminalPtr);
	int ParseWriteDOs(unsigned char * buf,share_terminal_ptr terminalPtr);
	int ParseWriteAOs(unsigned char * buf,share_terminal_ptr terminalPtr);

	//proctocol para api
	int setDeviceAddrLength(unsigned short val);
	int setFunCodeLength(unsigned short val);
	int setErrCodeLength(unsigned short val);
	int setByteCountLength(unsigned short val);
	int setRegisterAddrLength(unsigned short val);
	int setSingleDataLength(unsigned short val);
	int setCrcLength(unsigned val);

	int setTimeOutSynTime(unsigned short val);
	int setTimeOutHeartFrame(unsigned short val);
	int setTimeOutYkExe(unsigned short val);

	int setModbusMode(unsigned char val);
	static std::string TransModeToString(unsigned char val);
	static int TransModeFromString(std::string val);
	
private:
	static size_t ModbusObjectCounter_;

	enum ModbusPara
	{
		DEFAULT_DeviceAddrLength = 1,
		DEFAULT_FunCodeLength = 1,
		DEFAUTL_ErrCodeLength = 1,
		DEFAULT_ByteCountLength = 1,
		DEFAULT_RegisterAddrLength = 2,
		DEFAULT_SingleDataLength= 2,
		DEFAULT_CheckCrcLength = 2,

		DEFAULT_timeOutHeartFrame = 2,
		MIN_timeOutHeartFrame = 1,
		DEFAULT_timeOutSynTime = 1800,
		MIN_timeOutSynTime = 300,
		DEFAULT_timeOutYkExe = 25
	};

	unsigned short DeviceAddrLength_; //��ַԪ���ֽڳ���
	unsigned short FunCodeLength_;    //������Ԫ���ֽڳ���
	unsigned short ErrCodeLength_;    //�쳣��Ԫ���ֽڳ���
	unsigned short ByteCountLength_;  //�ֽڼ���Ԫ���ֽڳ���
	unsigned short RegisterAddrLength_;//�Ĵ�����ַԪ���ֽڳ���
	unsigned short SingleDataLength_; //����Ԫ���ֽڳ���
	unsigned short CRCLength_;        //CRCУ��Ԫ���ֽڳ���

	unsigned short DeviceAddrLocation_;//��ַԪ���ֽ�λ��
	unsigned short FunCodeLocation_;   //������Ԫ���ֽ�λ��
	unsigned short DataLocation_;      //����Ԫ���ֽ�λ��

	//Modbus��Լ������������
	bool bLowDataFirst_;               //����Ԫ�ص�λ�ֽ���ǰ��true����λ��ǰ��false����λ��ǰ
	bool bLowCRCFirst_;                //CRCԪ�ص�λ�ֽ���ǰ��true����λ��ǰ��false����λ��ǰ
	unsigned char mode_;               //��Լģʽ��RTU_MODE��TCP_MODE��ASCII_MODE

	//modbus �����������
	std::vector<CModbusCmdPara> modbusCmdPara_;
	stModbusCmd curCmd_;
	unsigned short MessageNO_;

	//timer
	unsigned short timeOutHeartFrame_;
	timerPtr timerHeartFrame_;
	unsigned short timeOutSynTime_;
	timerPtr timerSynTime_;
	unsigned short timeOutYkExe_;
	timerPtr timerYkExe_;

};

};//namespace Protocol
