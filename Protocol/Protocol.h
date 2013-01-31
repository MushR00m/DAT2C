#pragma once
#include <vector>
#include <boost/asio.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/function.hpp>
#include <iostream>
#include "../PublicSupport/LoopBuf.h"
#include "CmdQueue.h"

namespace FileSystem
{
	class CLog;
	class CMarkup;
	class CFileHandle;
}

namespace CommInterface
{
	class CCommInterface;
}

namespace DigitalSignature
{
	class CPublicKey;
	class CPrivateKey;
}

namespace Protocol {

typedef boost::shared_ptr<boost::asio::deadline_timer> timerPtr;
typedef boost::signals2::signal<int(unsigned char cmdType,unsigned char * buf,size_t length)> CommSignalType;
typedef CommSignalType::slot_type CommSlotType;
typedef std::vector<unsigned char> myframe;

const unsigned char WriteData = 1;
const unsigned char ReadData = 2;
const unsigned char OpenConnect = 3;
const unsigned char CloseConnect = 4;
const unsigned char ReConnect = 5;
const unsigned char BroadCast = 6;

//xml node api
#define strProtocolRoot "ProtocolROOT"

#define strInfoAddr "InfoAddr"
#define strSYxStartAddr "SYX_START_ADDR"
#define strDYxStartAddr "DYX_START_ADDR"
#define strYcStartAddr "YC_START_ADDR"
#define strSYkStartAddr "SYK_START_ADDR"
#define strDYkStartAddr "DYK_START_ADDR"
#define strYmStartAddr "YM_START_ADDR"
#define strHisStartAddr "HIS_START_ADDR"

#define strFrameElemLength "ElemLength"
#define strFrameLenLength "FrameLenLength"                               //���ĳ��ȱ�ʶ���ֽڳ���
#define strFrameTypeLength "FrameTypeLength"                             //�������ͱ�ʶ���ֽڳ���
#define strInfoNumLength "InfoNumLength"                                 //��Ϣ����Ŀ��ʶ���ֽڳ���
#define strTransReasonLength "TransReasonLength"                         //����ԭ���ʶ���ֽڳ���
#define strAsduAddrLength "AsduAddrLength"                               //װ�õ�ַ��ʶ���ֽڳ���
#define strInfoAddrLength "InfoAddrLength"                               //��Ϣ���ַ��ʶ���ֽڳ���
#define strFunTypeLength "FunTypeLength"                                 //�������ͱ�ʶ���ֽڳ���

#define strTimer "Timer"
#define strTimeOutQueryUnActivePoint "TimeOutQueryUnActivePoint"
#define strTimeOutRequireLink "TimeOutRequireLink"
#define strTimeOutCallAllData "TimeOutCallAllData"
#define strTimeOutCallAllDD "TimeOutCallAllDD"
#define strTimeOutSynTime "TimeOutSynTime"
#define strTimeOutHeartFrame "TimeOutHeartFrame"
#define strTimeOutYkSel "TimeOutYkSel"
#define strTimeOutYkExe "TimeOutYkExe"
#define strTimeOutYkCancel "TimeOutYkCancel"
#define strTimeOutCallPara "TimeOutCallPara"
#define strTimeOutSetPara "TimeOutSetPara"
#define strHeartTimeOutResetLink "HeartTimeOutResetLink"

class CProtocol :
	public CCmdQueue
{
public:
	CProtocol(boost::asio::io_service & io_service);
	virtual ~CProtocol(void);

public:
	//cmd api
	int AddSendCmdVal(CCmd val);
	int AddSendCmdVal(typeCmd CmdType,unsigned char priority,share_commpoint_ptr commpoint);
	int AddSendCmdVal(typeCmd CmdType,unsigned char priority,share_commpoint_ptr commpoint,boost::any val);
	int AddOnlySendCmdWithoutVal(typeCmd CmdType,unsigned char priority,share_commpoint_ptr commpoint,boost::any val);
	int AddOnlySendCmdByCmdType(typeCmd CmdType,unsigned char priority,share_commpoint_ptr commpoint,boost::any val);
	
	//comm points api
	int AddCommPoint(share_commpoint_ptr val);
	int AddCommPoint(weak_commpoint_ptr val);
	int DelCommPoint(int index);
	weak_commpoint_ptr getCommPoint(int index);
	int getCommPointIndexByAddrCommType(unsigned char commTypeVal,unsigned short addrVal,unsigned short addrLen = 0);
	int getCommPointSum();
	void ClearCommPoint();
	int DisableAllCommPoints();
	
	//����ͨѶ���
	share_commpoint_ptr getFirstCommPoint();
	share_commpoint_ptr getNextCommPoint(unsigned char pointType,boost::logic::tribool bCommActive,size_t curIndex);
	share_commpoint_ptr getNextCommPoint(unsigned char pointType,boost::logic::tribool bCommActive,share_commpoint_ptr point);
	share_commpoint_ptr getNextCommPointBySelfDef(unsigned char pointType,boost::function<bool(share_commpoint_ptr)> CheckTrueFunC,size_t curIndex);
	share_commpoint_ptr getNextCommPointBySelfDef(unsigned char pointType,boost::function<bool(share_commpoint_ptr)> CheckTrueFunC,share_commpoint_ptr point);

	//recv and send frame main api
	int RecvProcess(unsigned char * buf, size_t count);
	int SendProcess(unsigned char * buf, CCmd & cmd);

	//virtual init api
	virtual int InitProtocol();
	virtual void UninitProtocol();

	//virtual xml cfg api
	virtual int LoadXmlCfg(std::string filename) = 0;
	virtual void SaveXmlCfg(std::string filename) = 0;
	int LoadXmlCfg(FileSystem::CMarkup & xml);
	void SaveXmlCfg(FileSystem::CMarkup & xml);

	//sig api
	int ConnectCmdRecallSig(CmdRecallSlotType slotVal);
	int DisconnectCmdRecallSig();
	int ConnectSubAliveSig();
	int DisconnectSubAliveSig();
	bool getSubAliveSigConnection();
	//int ConnectSubYkSig();
	//int DisconnectSubYkSig();
	virtual void ProcessSubAliveSig(typeCmd cmdType,unsigned char ReturnCode,share_commpoint_ptr point,boost::any val);
	//virtual void ProcessSubYkSig(typeCmd cmdType,unsigned char ReturnCode,share_commpoint_ptr point,size_t index);

	SigConnection ConnectCommSingal(CommSlotType slotVal);

	//CFileHnadle  api
protected:
	unsigned long FileHandleGetRemainLength(void);
	unsigned long FileHandleGetTotalLength(void);
	std::string FileHandleGetFileName(void);
	unsigned int FileHandleSetTotalLength(unsigned long Totallen);
	int FileHandleWrite(void);
	int FileHandleWriteByByte(void);
	int FileHandleRead(void);
	int FileHandleReadFor533Pro(void);
	int FileHandleOutFile(unsigned char * filedata,int length);
	int FileHandleGetFile(unsigned char * filedata,int length);
	int FileHandleBegain(std::string name);
	int FileHandleFinish(void);

	//frame api
	virtual int MatchFrameHead(size_t & exceptedBytes);
	virtual int CheckFrameHead(unsigned char * buf,size_t & exceptedBytes) = 0;
	virtual int CheckFrameTail(unsigned char * buf,size_t exceptedBytes) = 0;
	virtual int ParseFrameBody(unsigned char * buf,size_t exceptedBytes) = 0;                  //����ͨѶ�ڵ��������е����

	virtual int AssembleFrameHead(size_t bufIndex, unsigned char * buf, CCmd & cmd) = 0;
	virtual int AssembleFrameBody(size_t bufIndex, unsigned char * buf, CCmd & cmd) = 0;
	virtual int AssembleFrameTail(size_t bufBegin, size_t bufIndex, unsigned char * buf, CCmd & cmd) = 0;

	bool CheckFrameRepeatOutCount(size_t & count);

	int getFrameBufLeft();

	//comm api
	int WriteDatas(unsigned char * buf,size_t length);
	int WriteBroadCast(unsigned char * buf,size_t length);
	int ReConnnectChannel();

	//protocol api
	share_commpoint_ptr getWaitingForAnswer();
	void setWaitingForAnswer(share_commpoint_ptr val);
	bool getbConsecutiveSend();
	void setbConsecutiveSend(bool val);
	virtual int QueryUnAliveCommPoint(share_commpoint_ptr point);
	int getCommPointIndexByPtr(share_commpoint_ptr point);
	int getMeanvalueOfPointsSum(unsigned short minVal,unsigned short val);

	//cmd api
	void handle_SendCmd();
	void ClearFrameRepeatFlag();
	void ClearWaitAnswerFlag();
	typeCmd getLastSendCmd();
	typeCmd getLastRecvCmd();
	int setLastSendCmd(typeCmd val);
	int setLastRecvCmd(typeCmd val);
	int getLastSendPointIndex();
	int getLastRecvPointIndex();
	int setLastSendPointIndex(int val);
	int setLastRecvPointIndex(int val);
	int getLastFrameLength();

	//commpint api
	int MathcCommPoint(weak_commpoint_ptr val);
	int InitAllCommPoints();
	int EnableCommPoint(share_commpoint_ptr point);
	//int setCommPointsDataBaseQuality(bool active);
	//bool getAllCommPointAlive();

	//log api
	int AddStatusLog(std::string strVal);
	int AddStatusLogWithSynT(std::string strVal);
	int RecordFrameData(const unsigned char * buf,size_t count,unsigned char datatype);
	int EnableStatusLog(std::string filename,std::string filetype,std::string limit = "");
	int EnableFrameLog(std::string filename,std::string filetype,std::string limit = "");
	void DisableStatusLog();
	void DisableFrameLog();

	//timer api
	void StopAllTimer();
	void AddTimer(timerPtr val);
	//void handle_timerWaitForAnswer(const boost::system::error_code& error,size_t index);
	//void ResetTimerWaitForAnswer(size_t commpointIndex,bool bContinue = true,unsigned short val = 0);
	void handle_timerWaitForAnswer(const boost::system::error_code& error,share_commpoint_ptr point);
	void ResetTimerWaitForAnswer(share_commpoint_ptr point,bool bContinue = true,unsigned short val = 0);
	void handle_timerConsecutiveSend(const boost::system::error_code& error);
	void ResetTimerConsecutiveSend(bool bContinue = true,unsigned short val = 0);

	bool getPrintFrameTerm();
	int setPrintFrameTerm(bool val);
	bool getPrintStatusTerm();
	int setPrintStatusTerm(bool val);
	int PrintInfoToTerm(std::string strVal);
	bool getClearQuality();
	int setClearQuality(bool val);
	bool getClearEvent();
	int setClearEvent(bool val);

	//boradcast api
	bool getBroadCastSend();
	int setBroadCastSend(bool val);

	//key api
	int getSignStartIndex();

	int CalcSecretDataLength(unsigned char * buf,size_t keyIndex,bool bCalcByFrame = true);

	int EnablePubKey();
	int EnablePriKey();
	bool CheckPubKey();
	bool CheckPriKey();

	int encrypt(size_t bufBegin,size_t bufCount,unsigned char * buf);
	int decrypt(size_t bufBegin,size_t bufCount,size_t dsStartIndex,unsigned char * buf);

	int CheckPlusHead(unsigned char * buf,size_t & exceptedBytes);
	int CheckPlusTail(unsigned char * buf,size_t exceptedBytes);
	int ParseFrame_Plus(unsigned char * buf,size_t exceptedBytes);

	int AssembleCheckPubKeyCon(size_t bufIndex,unsigned char * buf,share_commpoint_ptr pristationPtr,bool bConAct);
	int AssembleUpdatePubKeyCon(size_t bufIndex,unsigned char * buf,share_commpoint_ptr pristationPtr,bool bConAct);

private:
	int AddFrameLog(std::string strVal);
	int AddFrameLogWithSynT(std::string strVal);
		
	//key api
	std::string getSecretKeyPath();
	std::string getSecretKeyType();
	int setSecretKeyType(std::string val);
	int setSecretKeyPath(std::string val);

	int setSignStart(int SignStart);

	//std::string TransSecretKeyTypeToString(unsigned char val);
	//unsigned char TransSecretKeyTypeFromString(std::string val);

	int NotifySendCmd();
	
protected:
	enum errorType
	{
		NO_SYN_HEAD = 1,
		LESS_RECV_BYTE = 2,

		max_send_length = 1024,       //���ͻ�������󳤶�
		max_recv_length = 4096,       //���ջ�������󳤶�

		WaitforanswerTimeOut = 3,     //�ȵ�Ӧ���ĳ�ʱ��ʱ���Ĭ��ֵ ��λ����
		frameRepeatSum = 2,           //������Ҫ�ط����ܴ�����Ĭ��ֵ
		LostAnswerTimesOut = 30,      //���ٴ���Ӧ�����Ժ���ΪͨѶ�жϵĴ���Ĭ��ֵ
		LostRecvTimeOut = 180,        //�೤ʱ��δ�յ������Ժ���ΪͨѶ�жϵ�ʱ��Ĭ��ֵ����λ����
		ConsecutiveSendTimeOut = 200  //����������֡����֮����Ҫ��ʱ������Ĭ��ֵ ��λ������
	};
	
	size_t ProtocolObjectIndex_;

	//std::vector<weak_pristation_ptr> pristations_; //���ڱ�ͨ������վָ�뼯��
	//std::vector<weak_terminal_ptr> terminals_;     //���ڱ�ͨ�����ն�ָ�뼯��
	std::vector<weak_commpoint_ptr> commPoints_;     //���ڱ�ͨ����ͨѶ���ָ�뼯��
	//int iCurCommpointIndex_;                      //��ǰ�����ͨѶ������
	share_commpoint_ptr CurWaitCommpointPtr_;        //��ǰ��Ҫ�ȴ�Ӧ���ͨѶ���ָ�룬Ϊ�ձ�ʾû����ҪӦ��Ľ�㡣

	bool bActiveDataUp_;                             //�ù�Լ�Ƿ��������������ݵ�

	bool bActiveRepeatFrame_;                        //�ù�Լ�Ƿ���Ҫ�ط�������
	size_t iFrameRepeatSum_;                         //������Ҫ�ط����ܴ���
	size_t iLostAnswerTimesOut_;                     //���ٴ���Ӧ�����Ժ���ΪͨѶ�ж�

	//bool bWaitingForAnswer_;                       //��Ҫ�ȴ����Ļ�Ӧ
	unsigned short timeOutWaitforAnswer_;            //�ȵ�Ӧ���ĳ�ʱ��ʱ�� ��λ����

	bool bConsecutiveSend_;                          //��Ҫ�ȴ���Ϣʱ��ŷ���һ֡����
	unsigned short timeOutConsecutiveSend_;          //����������֡����֮����Ҫ��ʱ���� ��λ������

	bool bActiveDataRecv_;                           //�ù�Լ�Ƿ���Ҫ�����յ����������ж�����ͨѶ״��
	unsigned short timeOutLostRecv_;                 //�೤ʱ��δ�յ������Ժ���ΪͨѶ�жϣ���λ����

	bool bRepeatLastFrame_;                          //�ط���һ֡����
	size_t iFrameRepeatCount_;                       //��ǰ�����Ѿ��ط��Ĵ���
	size_t SynCharNum_;                              //����ͬ��ͷ�ĳ���
	bool bBroadCastSend_;                            //�Ƿ��͹㲥����

	bool bClearDataBaseQuality_;                     //�Ƿ��ڹ�Լ��ʼ���׶ν��������ݿ��Ʒ������ֵ��ΪʧЧ�������յ����ݱ��ĺ���Ҫ�ھ����Լ���ֶ���Ʒ��������Ϊ��Ч��
	bool bClearEventLoadPtr_;                        //�Ƿ��ڹ�Լ��ʼ���׶�����CPristation�ĸ澯�¼���Loadָ�룬Ĭ��false

	bool bInitQuality_;                              //ʹ��Լ�µ��ն�ͨѶ�������Ʒ�������ܳ�ʼ����־:CCommPoint::bInitCommPoint_Ӱ��
	
	std::string SecretKeyType_;                      //����ʹ����Կ���ܽ���
	std::string KeyPath_;                            //��Կ�ļ�·������
	int SignStart_;                                  //ǩ�����Ŀ�ʼλ��
	
	CommSignalType CommSig_;
	CmdRecallSignalType CmdConSig_;
	SigConnection CommSigConnection_;
	SigConnection CmdConSigConnection_;
	SigConnection SubAliveSigConnection_;
	//boost::signals::connection SubYkSigConnection_;

	//bool bFirstCallAllData_;                         //��Լ�Ƿ��һ���ٻ�����

	int max_frame_length_;

	//PublicSupport::CLoopBuf  loopbuf_;                   //���ձ��ĵ�ѭ��������
	boost::scoped_ptr<PublicSupport::CLoopBuf> recv_buf_;   //���ձ��ĵ�ѭ��������

	//unsigned char send_data_[max_send_length];           //���ͱ��Ļ�����
	boost::scoped_array<unsigned char> send_buf_;         //���ͱ��Ļ�����
	int iLastFrameLength_;                              //��һ֡���ͱ��ĵĳ���

private:
	//��ʱ��
	boost::asio::deadline_timer timerWaitForAnswer_;   //�ȴ�Ӧ���ĵĶ�ʱ�� ʱ����λ����
	boost::asio::deadline_timer timerConsecutiveSend_; //����������֡����֮����Ҫ��ʱ�����Ķ�ʱ�� ʱ����λ������
	std::vector<timerPtr> timers_;                     //�����Լʹ�õĶ�ʱ������ ʱ����λ����

	//IO����
	boost::asio::io_service & io_service_;

	typeCmd LastSendCmd_;
	int LastSendPointIndex_;
	typeCmd LastRecvCmd_;
	int LastRecvPointIndex_;

	//ָ����־�ļ���ָ��
	boost::scoped_ptr<FileSystem::CLog> statusLog_;
	boost::scoped_ptr<FileSystem::CLog> frameLog_;
	boost::scoped_ptr<FileSystem::CFileHandle> FileHandle_;
	//std::string statusLogName_;
	//std::string frameLogName_;
	bool bPrintFrame_;
	bool bPrintStatus_;

	//key
	boost::shared_ptr<DigitalSignature::CPublicKey> pubKey_;
	boost::shared_ptr<DigitalSignature::CPrivateKey> priKey_;
};

} //Protocol
