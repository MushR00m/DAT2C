#pragma once
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/variant.hpp>
#include <boost/date_time/time_duration.hpp>
#include "../PublicSupport/Dat2cTypeDef.h"
#include "../CommInterface/CommPara.h"

//�Զ���ͨѶ�������
const unsigned char PRISTATION_NODE = 1;
const unsigned char TERMINAL_NODE = 2;

//֪ͨ�¼�����
const unsigned char COMM_EVENT = 1;
const unsigned char YX_EVENT = 2;
const unsigned char YK_EVENT = 3;
const unsigned char SELF_EVENT = 4;

namespace FileSystem
{
	class CMarkup;
}

namespace Protocol
{
	class CCmd;
}

namespace CommInterface 
{
	class CCommInterface;
	class CServerInterface;
}

namespace DataBase {

typedef boost::shared_ptr<CommInterface::CCommInterface> share_comm_ptr;
typedef boost::shared_ptr<CommInterface::CServerInterface> share_server_ptr;
typedef boost::variant<share_comm_ptr,share_server_ptr> share_pointval_ptr;

class CCommPoint
{
public:
	//CCommPoint();
	CCommPoint(boost::asio::io_service & io_service);
	virtual ~CCommPoint(void);

	int EnableCommunication(CmdRecallSlotType slotVal,share_pointval_ptr ptr = share_pointval_ptr(),bool bNewPtr = true);
	void DisableCommunication();

	void SaveXmlCfg(FileSystem::CMarkup & xml);
	int LoadXmlCfg(FileSystem::CMarkup & xml);

	//Addr api
	typeAddr getAddr();
	int setAddr(typeAddr val);
	typeAddr getBroadCastAddr();
	int setBroadCastAddr(typeAddr val);
	bool getbAcceptBroadCast();
	int setbAcceptBroadCast(bool val);
	
	void ClearServerPtr();
	void SetServerPtr(share_server_ptr valPtr);
	share_server_ptr getServerPtr();

	void ClearCommPtr();
	void ResetCommPtr(CommInterface::CCommInterface * valPtr);
	void ResetCommPtr(share_comm_ptr valPtr);
	share_comm_ptr getCommPtr();

	int AddCmdVal(Protocol::CCmd val);
	int AddCmdVal(typeCmd CmdType,unsigned char priority,share_commpoint_ptr commpoint,boost::any val);
	int AddCmdVal(typeCmd CmdType,unsigned char priority,share_commpoint_ptr commpoint);
	void clearCmdQueue();
	int getCmdQueueSum();
	int getMaxPriopriyCmd(Protocol::CCmd & dstVal);

	//�麯���ӿ�
	virtual share_commpoint_ptr getSelfPtr() = 0;
	virtual SigConnection ConnectSubAliveSig(CmdRecallSlotType slotVal) = 0;
	virtual SigConnection ConnectSubTempSig(CmdRecallSlotType slotVal) = 0;
	//virtual void ClearDataBaseQuality(bool active);

	//��ԽAPI
	void AddSelfPointToProtocol();
	//void AddSelfPointToServerPtr();
	int ResetTimerRecv(size_t LostRecvTimeOut);
	void handle_timerRecv(const boost::system::error_code& error);

	//xml cfg api
	//std::string TransCommChannelTypeToString(unsigned char val);
	//char TransCommChannelTypeFromString(std::string val);
	//std::string TransProtocolTypeToString(unsigned short val);
	//short TransProtocolTyeFromString(std::string val);
	
	//comm api
	virtual bool getCommActive();
	virtual void setCommActive(bool val);
	bool getInitCommPointFlag();
	void setInitCommPointFlag(bool val);
	bool getSynTCommPointFlag();
	void setSynTCommPointFlag(bool val);
	bool getDelayCommPointFlag();
	void setDelayCommPointFlag(bool val);
	bool CheckLostAnserTimesPlus(size_t LostAnswerTimesOut);

	unsigned char getCommPointType();

	//cmd recall api
	int ConnectCmdRecallSig(CmdRecallSlotType slotVal);
	int DisconnectCmdRecallSig();

	int ConnectCmdRelaySig(CmdRecallSlotType slotVal);
	int DisconnectCmdRelaySig(bool bForceClose);
	//bool getSubTempSigConnection();
	void ProcessRelayCmd(typeCmd cmdType,unsigned char ReturnCode,share_commpoint_ptr point,boost::any val);

	std::string getLocalPort();
	std::string getRemoteID();
	bool MatchRemoteIP(std::string val);
	CommInterface::CCommPara & getCommPara();

	virtual int InitLocalServices(CmdRecallSlotType slotVal,bool SlotReady);
	virtual void UnInitLocalServices();

	//��ʾͨѶ�㵱ǰ��ͨѶ״̬
	int NotifyEventStatus(typeCmd EventType,unsigned char ReturnCode);

	boost::posix_time::time_duration getTdDiff();
	int setTdDiff(boost::posix_time::time_duration val);

	//encrypt api
	bool getEncryptOutside();
	int setEncryptOutside(bool val);

private:
	bool MatchRemoteID(std::string val);
	int setRemoteID(std::string val);

	void MulCmdRelaySigCounterPlus();
	bool MulCmdRelaySigCounterCut();
	void ClearMulCmdRelaySigCounter();

	int ConnectEventStatusSig(CmdRecallSlotType slotVal);
	void DisconnectEventStatusSig();

protected:
	bool bCommActive_;                          //��ͨѶ����Ƿ�ͨѶ����
	size_t iLostAnswerTimes_;                   //�ý��δ��Ӧ����Ĵ���
	boost::asio::deadline_timer timerRecv_;     //���ձ��ĳ�ʱ��ʱ��
	
	unsigned char pointType_;                   //ͨѶ�������
	
	share_comm_ptr commPtr_;                    //ָ��ͨѶͨ����ָ��
	share_server_ptr serverPtr_;                //ָ�����ͨ����ָ��

	boost::asio::io_service & io_service_;      //IO���������

	typeAddr addr_;                             //ͨѶ��ַ��ʶ
	bool bAceeptBroadcast_;                     //��վ���Ƿ���չ㲥�����鲥
	typeAddr BroadCastAddr_;                    //���չ㲥�����鲥�ĵ�ַ
	bool bFcbFlag_;

	bool bDelayDownload_;                       //ͬ��ͨѶ�����ʱ���
	bool bSynTCommPoint_;                       //ͬ��ͨѶ���ʱ���� 
	bool bInitCommPoint_;                       //��ʼ��ͨѶ������ݱ��

	CmdRecallSignalType RelaySig_;              //�ź��м�ת��
	int MulCmdRelaySigCounter_;                 //����ź����ӵļ�����
	SigConnection RelaySigConnection_;
	SigConnection SubTempSigConnection_;

	CmdRecallSignalType EventStatusSig_;         //�¼�״̬����ź�
	SigConnection EventStatusConnection_;        //�¼�״̬������� 
	
	//�¼�״̬ע��ı�ʶ���
	int CommRegisterNO_;
	int YxReisterNO_;
	int YkRegisterNO_;
	int SelfRegisterNO_;

	CommInterface::CCommPara commPara_; //ͨѶ����

	boost::posix_time::time_duration td_diff_;

	bool bEncryptOutside_;
};

};  //namespace DataBase
