#pragma once

#include "Protocol.h"

namespace Protocol {

class CH103_DigiproII
	:public CProtocol
{
public:
	CH103_DigiproII(boost::asio::io_service & io_service);
	virtual ~CH103_DigiproII(void);

	virtual int LoadXmlCfg(std::string filename);
	virtual void SaveXmlCfg(std::string filename);

protected:
	virtual int CheckFrameHead(unsigned char * buf,size_t & exceptedBytes);
	virtual int CheckFrameTail(unsigned char * buf,size_t exceptedBytes);
	virtual int ParseFrameBody(unsigned char * buf,size_t exceptedBytes);

	virtual int AssembleFrameHead(size_t bufIndex, unsigned char * buf, CCmd & cmd);
	virtual int AssembleFrameBody(size_t bufIndex, unsigned char * buf, CCmd & cmd);
	virtual int AssembleFrameTail(size_t bufBegin, size_t bufIndex, unsigned char * buf, CCmd & cmd);

	virtual int InitProtocol();
	virtual void UninitProtocol();

private:
	void InitObjectIndex();
	//void InitDefaultStartAddr();
	//void InitDefaultFrameElem();
	//void InitFrameLocation(size_t FrameHead);
	void InitDefaultTimeOut();
	void InitDefaultTimer(boost::asio::io_service & io_service);

	void handle_timerCallAllData(const boost::system::error_code& error,share_commpoint_ptr point);
	void handle_timerSynTime(const boost::system::error_code& error,share_commpoint_ptr point);
	void handle_timerYkSel(const boost::system::error_code& error,share_commpoint_ptr point,size_t yk_no);
	void handle_timerCallAllEvent(const boost::system::error_code& error,share_terminal_ptr terminalPtr);

	void ResetTimerCallAllData(share_commpoint_ptr point,bool bContinue = true,unsigned short val = 0);
	void ResetTimerSynTime(share_commpoint_ptr point,bool bContinue = true,unsigned short val = 0);
	void ResetTimerYkSel(share_commpoint_ptr point,size_t yk_no,bool bContinue = false,unsigned short val = 0);
	void ResetTimerCallAllEvent(share_terminal_ptr terminalPtr,bool bContinue = true,unsigned short val = 0);

	int AssembleSynTime(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr, boost::posix_time::ptime time);
	int AssembleCallAllData(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr);
	int AssembleYKSel(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr,unsigned char yk_obj, unsigned char yk_code);
	int AssembleYKExe(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr,unsigned char yk_obj, unsigned char yk_code);
	int AssembleEventCon(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr);

	int ParseAllData(unsigned char * buf, share_terminal_ptr terminalPtr);
	int ParseAllEvent(unsigned char * buf, share_terminal_ptr terminalPtr);
	int ParseYkSelCon(unsigned char * buf, share_terminal_ptr terminalPtr);

	int TransYkCmdToPara(int yk_no,typeYktype yk_type,unsigned char & yk_obj,unsigned char & yk_code);
	int TransYkParaToCmd(unsigned char yk_obj,unsigned char yk_code,int & yk_no,typeYktype & yk_type);

private:
	enum H103_DigiproIIPara
	{
		BroadCastAddr = 255,

		DEFAULT_SYX_START_ADDR = 149,                       //Ĭ�ϵ���yx��ʼ��ַ
		DEFAULT_DYX_START_ADDR = 401,                       //Ĭ��˫��yx��ʼ��ַ

		DEFAULT_YC_START_ADDR = 92,                         //Ĭ��yc��ʼ��ַ
		DEFAULT_SYK_START_ADDR = 48,                        //Ĭ�ϵ���yk��ʼ��ַ
		DEFAULT_DYK_START_ADDR = 48,                        //Ĭ��˫��yk��ʼ��ַ
		DEFAULT_YM_START_ADDR = 6,                          //Ĭ��ym��ʼ��ַ

		DEFAULT_FrameTypeLength = 1,                            //Ĭ�ϱ������ͱ�ʶ���ֽڳ���
		DEFAULT_InfoNumLength = 1,                              //Ĭ����Ϣ����Ŀ��ʶ���ֽڳ���
		DEFAULT_TransReasonLength = 1,                          //Ĭ�ϴ���ԭ���ʶ���ֽڳ���
		DEFAULT_AsduAddrLength = 1,                             //Ĭ��װ�õ�ַ��ʶ���ֽڳ���
		DEFAULT_InfoAddrLength = 1,                             //Ĭ����Ϣ���ַ��ʶ���ֽڳ���
		DEFAULT_FunTypeLength = 1,                              //Ĭ�Ϲ������ͱ�ʶ���ֽڳ���
				
		DEFAULT_timeOutCallAllData = 1,
		MIN_timeOutCallAllData = 1,
		DEFAULT_timeOutSynTime = 300,
		MIN_timeOutSynTime = 60,
		DEFAULT_timeOutYkSel = 10,
		DEFAULT_timeOutCallAllEvent = 1800,
		MIN_timeOutCallAllEvent = 600,

	};

	static size_t H103ObjectCounter_;

	unsigned short timeOutCallAllData_;
	timerPtr timerCallAllData_;
	unsigned short timeOutSynTime_;
	timerPtr timerSynTime_;
	unsigned short timeOutYkSel_;
	timerPtr timerYkSel_;
	unsigned short timeOutCallAllEvent_;
	timerPtr timerCallAllEvent_;
};

}; //namespace Protocol 

