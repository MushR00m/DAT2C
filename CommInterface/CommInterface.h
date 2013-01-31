#pragma once
#include <boost/scoped_ptr.hpp>
#include <boost/asio.hpp>
#include "../PublicSupport/Dat2cTypeDef.h"
#include "CommPara.h"

namespace FileSystem
{
	class CLog;
}

namespace Protocol
{
	class CCmd;
	class CProtocol;
}

namespace CommInterface {

class CCommPara;

class CCommInterface
{
public:
	CCommInterface(CCommPara & para,boost::asio::io_service & io_service);
	virtual ~CCommInterface(void);

	//connect api
	virtual int InitConnect(void) = 0;
	virtual void UninitConnect(void) = 0;
	virtual int OpenConnect(void) = 0;
	virtual int CloseConnect(void) = 0;
	virtual int ReadFromConnect(unsigned char * buf,size_t bytes_transferred = 0) = 0;
	virtual int WriteToConnect(const unsigned char * buf,size_t bytes_transferred) = 0;
	virtual int WriteToBroadCast(const unsigned char * buf,size_t bytes_transferred);
	virtual bool getConnectAlive(void);
	virtual int RetryConnect();

	//comm points api
	int AddCommPoint(share_commpoint_ptr val);
	//int AddCommPoint(weak_commpoint_ptr val);
	int DelCommPoint(int index);
	weak_commpoint_ptr getCommPoint(int index);
	int getCommPointSum();
	void ClearCommPoint();

	//protocol api
	//int setProtocoType(unsigned short val);
	int AddCmdVal(Protocol::CCmd val);
	void clearCmdQueue();
	int getCmdQueueSum();
	int getMaxPriopriyCmd(Protocol::CCmd & dstVal);
	//bool getDataActiveUp();
	//int LoadProtocolXmlCfg(std::string filename);
	//void SaveProtocolXmlCfg(std::string filename);

	//cmd recall api
	int ConnectCmdRecallSig(CmdRecallSlotType slotVal);
	int DisconnectCmdRecallSig();

protected:
	//protocol api
	int EnableProtocol();
	void DisableProtocol();

	int InitProtocol();
	void UninitProtocol(bool bDisableCommpoint = true);

	int ProcessProtocolSingal(unsigned char commType,unsigned char * buf,size_t length);
	int ConnectProtocolSingal();
	int DisconnectProtocolSingal();

	//log api
	int AddStatusLog(std::string strVal);
	int AddStatusLogWithSynT(std::string strVal);
	int RecordFrameData(const unsigned char * buf,size_t count,bool bRecv);
	int EnableLog();
	void DisableLog();

private:
	int AddFrameLog(std::string strVal);
	int AddFrameLogWithSynT(std::string strVal);
	int PrintInfoToTerm(std::string strVal);

	//log ptr enable
	int EnableStatusLog(std::string filename,std::string filetype,std::string limit = "");
	int EnableFrameLog(std::string filename,std::string filetype,std::string limit = "");
	void DisableStatusLog();
	void DisableFrameLog();

protected:
	//enum 
	//{
	//	max_length = 1024 ,
	//};

	boost::scoped_ptr<Protocol::CProtocol> protocol_;   // ָ���Լ��ָ��
	boost::asio::io_service & io_service_;
	//CCommPara & para_;
	CCommPara para_; //ͨѶ��������ʵ�����������õķ�ʽ������Ϊ���Ժ������߸���ͨѶ��������չ��

private:
	//ָ����־�ļ���ָ��
	boost::scoped_ptr<FileSystem::CLog> statusLog_;
	boost::scoped_ptr<FileSystem::CLog> frameLog_;

	//ͨѶ��Լ��ͨѶͨ��֮����ź���
	SigConnection ProtocolSingalConnection_;
};

} // namespace CommInterface
