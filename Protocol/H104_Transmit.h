#pragma once
#include "protocol.h"

namespace Protocol {

typedef unsigned short typeFrameCounter;

struct stSynTimePara 
{
	unsigned short IFrameSendCounter_;
	unsigned short IFrameRecvCounter_;
	typeAddr addr_;
};

class CH104_Transmit :
	public CProtocol
{
public:
	CH104_Transmit(boost::asio::io_service & io_service);
	virtual ~CH104_Transmit(void);

	virtual int CheckFrameHead(unsigned char * buf,size_t & exceptedBytes);
	virtual int CheckFrameTail(unsigned char * buf,size_t exceptedBytes);
	virtual int ParseFrameBody(unsigned char * buf,size_t exceptedBytes);                  //����ͨѶ�ڵ��������е����

	virtual int AssembleFrameHead(size_t bufIndex, unsigned char * buf, CCmd & cmd);
	virtual int AssembleFrameBody(size_t bufIndex, unsigned char * buf, CCmd & cmd);
	virtual int AssembleFrameTail(size_t bufBegin, size_t bufIndex, unsigned char * buf, CCmd & cmd);

	virtual int LoadXmlCfg(std::string filename);
	virtual void SaveXmlCfg(std::string filename);

	static typeFrameCounter FrameCounterPlus(typeFrameCounter OriCounter,int plus_num);

private:
	int setFrameLenLength(unsigned short val);
	int setIGramCounterLength(unsigned short val);
	int setFrameTypeLength(unsigned short val);
	int setInfoNumLength(unsigned short val);
	int setTransReasonLength(unsigned short val);
	int setAsduAddrLength(unsigned short val);
	int setInfoAddrLength(unsigned short val);

	void InitDefaultFrameElem();
	void InitFrameLocation(size_t frameHead);

	bool getbIgnoreSynTime();
	int setbIgnoreSynTime(bool val);
	stSynTimePara getIgnorePara();
	int setIgnorePara(stSynTimePara val);

	int AssembleSynTime(size_t bufIndex, unsigned char * buf, stSynTimePara para,boost::posix_time::ptime time);

private:
	enum H104_TransmitPara
	{
		DEFAULT_FrameLenLength = 1,                             //Ĭ�ϱ��ĳ��ȱ�ʶ���ֽڳ���
		DEFAULT_IGramCounterLength = 2,
		DEFAULT_FrameTypeLength = 1,                            //Ĭ�ϱ������ͱ�ʶ���ֽڳ���
		DEFAULT_InfoNumLength = 1,                              //Ĭ����Ϣ����Ŀ��ʶ���ֽڳ���
		DEFAULT_TransReasonLength = 2,                          //Ĭ�ϴ���ԭ���ʶ���ֽڳ���
		DEFAULT_AsduAddrLength = 2,                             //Ĭ��װ�õ�ַ��ʶ���ֽڳ���
		DEFAULT_InfoAddrLength = 3,                             //Ĭ����Ϣ���ַ��ʶ���ֽڳ���
	};

	unsigned short FrameLenLength_;                              //���ĳ��ȱ�ʶ���ֽڳ���
	unsigned short IGramCounterLength_;
	unsigned short FrameTypeLength_;                             //�������ͱ�ʶ���ֽڳ���
	unsigned short InfoNumLength_;                               //��Ϣ����Ŀ��ʶ���ֽڳ���
	unsigned short TransReasonLength_;                           //����ԭ���ʶ���ֽڳ���
	unsigned short AsduAddrLength_;                              //װ�õ�ַ��ʶ���ֽڳ���
	unsigned short InfoAddrLength_;                              //��Ϣ���ַ��ʶ���ֽڳ���

	unsigned short FrameLenLocation_;                            //���ĳ��ȱ�ʶ���ֽڶ�λ
	unsigned short RecvCounterLocation_;
	unsigned short SendCounterLocation_;
	unsigned short FrameTypeLocation_;                           //�������ͱ�ʶ���ֽڶ�λ
	unsigned short InfoNumLocation_;                             //��Ϣ����Ŀ��ʶ���ֽڶ�λ
	unsigned short TransReasonLocation_;                         //����ԭ���ʶ���ֽڶ�λ
	unsigned short AsduAddrLocation_;                            //װ�õ�ַ��ʶ���ֽڶ�λ
	unsigned short InfoAddrLocation_;                            //��Ϣ���ַ��ʶ���ֽڶ�λ
	unsigned short DataLocation_;                                //���ݱ�ʶ���ֽڶ�λ

	bool bIgnoreSynTime_;
	stSynTimePara synPara_;
};

};//namespace Protocol

