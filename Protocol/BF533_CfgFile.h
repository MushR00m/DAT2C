#pragma once
#include "Protocol.h"

namespace FileSystem
{
	class CLog;
	class CMarkup;
}

namespace Protocol
{
class CBF533_CfgFile
{
public:
	CBF533_CfgFile(boost::asio::io_service & io_service);
	~CBF533_CfgFile(void);
	//�ļ����궨��
	#define      BFBasePara                     "BasePara.xml"
	#define      BFChanneltypePara              "ChanneltypePara.xml"
	#define      BFDevSanyaoPara                "DevSanyaoPara.xml"
	#define      BFInterfacePara                "InterfacePara.xml"
	#define      BFLinePara                     "LinePara.xml"
	#define      BFMaster1YcZFTab               "Master1YcZFTab.xml"
	#define      BFMaster1YxZFTab               "Master1YxZFTab.xml"
	#define      BFMaster2YcZFTab               "Master2YcZFTab.xml"
	#define      BFMaster2YxZFTab               "Master2YxZFTab.xml"
	#define      BFProtectValPara               "ProtectValPara.xml"
	#define      BFYMData                       "YMData.xml"
	#define      BFYkRecord                     "YkRecord.xml"
	#define      BFFaultRecord                  "FaultRecord.xml"
	#define      BFSOERecord                    "EventRecord.xml"
	#define      BFExtremum                     "Extremum.xml"
	#define      AllExtremum                    "AllExtremum.xml"
	#define      AllHisData                     "AllHisData.xml"
	#define      RecordFileCfg                  "RecordFileCfg.xml"  //����ѭ���ļ������������ֵͳ�Ʒ�ʽ��NoCurRecord
	#define      NoCurRecord                    "NoCurRecord.xml" 
public:
	//�����ļ�����ӿ�
	void Creat_BasePara(void);
	void Creat_DevSanyaoPara(void);
	void Creat_ChanneltypePara(void);
	void Creat_InterfacePara(void);
	void Creat_LinePara(void);
	void Creat_Master1YcZFTab(void);
	void Creat_Master1YxZFTab(void);
	void Creat_Master2YcZFTab(void);
	void Creat_Master2YxZFTab(void);
	void Creat_ProtectValPara(void);
	//��ֵͳ����غ���
	void Creat_Extremum(int linenum,boost::posix_time::ptime time);
	void Creat_HistoricalData(int linenum,boost::posix_time::ptime time);
	void Creat_BFYMData(int LineNum);
	void Creat_BFFaultRecord(void);
	void Creat_BFSOERecord(void);
	void Creat_RecordFileCfg(void);
	void Creat_NoCurRecord(unsigned int LineNum);

	int getip(char *name,char *ip_buf,char *mask);
	int setip(char *name,char *ip,char *mask);
};//class CBF533_CfgFile

};//namespace Protocol


