#pragma once
#ifndef CommPara_H
#define CommPara_H

#include <vector>

namespace FileSystem
{
	class CMarkup;
}

namespace CommInterface {

class CCommPara
{
public:
	CCommPara(void);
	CCommPara(const CCommPara & rhs);
	CCommPara & operator = (const CCommPara & rhs);
	~CCommPara(void);

	void SaveXmlCfg(FileSystem::CMarkup & xml);
	int LoadXmlCfg(FileSystem::CMarkup & xml);

	//file name api
	std::string getProtocolCfgFileName();
	bool getEnableSpecialProtocolCfg();
	std::string getCommChannelStatusLogFileName();
	bool getEnableChannelStatusLog();
	std::string getCommChannelFrameLogFileName();
	bool getEnableChannelFrameLog();
	std::string getCommChannelStatusLogType();
	std::string getCommChannelFrameLogType();
	std::string getCommChannelStatusLogLimit();
	std::string getCommChannelFrameLogLimit();

	bool getPrintFrameTerm();
	bool getPrintStatusTerm();

	std::string getLocalIP();
	std::string getLocalPort();
	std::string getBroadcastIP();
	std::string getBroadcastPort();
	std::string getMulticastIP();
	std::string getMulticastPort();
	std::string getRemoteID();
	std::string getMatchType();
	std::string getCommChannelType();
	std::string getProtocolType();

	int setRemoteID(std::string val);

	int getNextRemotePara(std::string & ip,std::string port);
	bool MatchRemoteIP(std::string val);
	int getRemoteIPSum();
	int getRemotePortSum();
	std::string getRemoteIP(int index);
	std::string getRemotePort(int index);
	bool getbDisableCommpointByCloseConnect();

	//para format api
	static bool AssertIPFormat(std::string val);
	static bool AssertNetPortFormat(std::string val);
	static bool AssertSerialPortFormat(std::string val);

private:
	int setProtocolCfgFileName(std::string val);
	int setCommChannelStatusLogFileName(std::string val);
	int setCommChannelFrameLogFileName(std::string val);
	int setCommChannelStatusLogType(std::string val);
	int setCommChannelFrameLogType(std::string val);
	int setCommChannelStatusLogLimit(std::string val);
	int setCommChannelFrameLogLimit(std::string val);

	int setPrintFrameTerm(bool val);
	int setPrintStatusTerm(bool val);

	int setLocalIP(std::string val);
	int setLocalPort(std::string val);
	int setBroadcastIP(std::string val);
	int setBoardcastPort(std::string val);
	int setMulticastIP(std::string val);
	int setMulticastPort(std::string val);
	int setCommChannelType(std::string val);
	int setProtocolType(std::string val);
	int setMatchType(std::string val);

	int AddRemoteIP(std::string val);
	int AddRemotePort(std::string val);
	void ClearRemoteIP();
	void ClearRemotePort();

	int setbDisableCommpointByCloseConnect(bool val);

private:
	std::string localIP_;                      //����IP
	std::string localPort_;                    //���ض˿�
	std::string multicastIP_;                  //�ಥ��ַ
	std::string multicastPort_;                //�ಥ�˿ں�
	std::string broadcastIP_;                  //�㲥��ַ
	std::string broadcastPort_;                //�㲥�˿�
	std::vector<std::string> remoteIPs_;       //�Զ�IP����
	std::vector<std::string> remotePorts_;     //�Զ˶˿ڼ���
	int remoteParaIndex_;

	std::string remoteID_;                     //�Զ�ID��ʶ

	std::string commChannelType_;              //ͨѶͨ������
	std::string commServerMatchType_;          //������ͨ����ƥ�䷽ʽ
	bool bPrintFrame_;                         //����ʾ�ն˴�ӡ����
	bool bPrintStatus_;                        //����ʾ�ն˴�ӡ״̬

	std::string commChannelStatusLog_;          //����ͨ��״̬��־
	std::string commChannelFrameLog_;           //����ͨ��������־
	std::string commChannelStatusLogType_;
	std::string commChannelFrameLogType_;
	std::string commChannelStatusLogLimit_;
	std::string commChannelFrameLogLimit_;

	std::string protocolType_;                 //��Լ����
	std::string protocolCfgFile_;              //��Լ�����ļ���

	bool DisableCommpointByCloseConnect_;      //�����ر�ͨ����ʱ���Ƿ����̸���ͨѶ���ͨѶ״̬����Ϊfalse�Ļ�Ҳ����ζ��ͨ����״̬��������Ӱ�쵽��ͨ����ͨѶ���״̬������ͨѶ��ָ�ͨѶ���������ϱ���Ļ���ʱ�䡣Ŀǰֻ��TcpClientʹ������һ���ԡ�
};

};//namespace CommInterface 

#endif //#ifndef CommPara_H
