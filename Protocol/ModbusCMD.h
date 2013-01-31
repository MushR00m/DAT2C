#pragma once
#include <vector>
#include "../PublicSupport/Dat2cTypeDef.h"

namespace Protocol {

const char YxDataType = 1;
const char YcDataType = 2;
const char YmDataType = 3;
const char YkDataType = 4;
const char SynTimeType = 5;

struct stModbusCmd
{
	unsigned short FunCode_;            //���Ĺ�����
	unsigned short RegisterAddr_;      //Ҫ�����ļĴ�����ַ
	unsigned short DataNum_;           //Ҫ��������������

	unsigned char PointDataType_;      //��Ӧ���ݴ洢ʵ������������
	size_t PointIndex_;                //��Ӧ���ݴ洢ʵ�������ݵ��

	stModbusCmd(unsigned char funCode,unsigned short registerAddr,unsigned short DataNum,unsigned char PointDataType,size_t PointIndex)
		:FunCode_(funCode),
		RegisterAddr_(registerAddr),
		DataNum_(DataNum),
		PointDataType_(PointDataType),
		PointIndex_(PointIndex)
	{

	}

	stModbusCmd()
	{
		FunCode_ = 0;
		RegisterAddr_ = 0;
		DataNum_ = 0;
		PointDataType_ = 0;
		PointIndex_ = 0;
	}
};

class CModbusCmdPara
{
public:
	CModbusCmdPara(typeAddr addr);
	CModbusCmdPara(const CModbusCmdPara & rhs);
	virtual ~CModbusCmdPara(void);
	CModbusCmdPara & operator = (const CModbusCmdPara & rhs);

	//bool getbInit();
	typeAddr getAddr();
	int getNextReadCmd(stModbusCmd & val);
	stModbusCmd getReadCMD(int index);
	stModbusCmd getWriteCMD(int index);
	int getWriteCmd(unsigned char PointDataType,size_t PointIndex,stModbusCmd & val);
	int AddReadCmd(stModbusCmd val);
	int AddWriteCmd(stModbusCmd val);
	int DelReadCmd(int index);
	int DelWriteCmd(int index);
	int getReadCmdSum();
	int getWriteCmdSum();
	static std::string TransDataTypeToString(char val);
	static int TransDataTypeFromString(std::string val);
	
private:
	int InitDefaultPara();

private:
	int ReadIndex_;                        //�������������ı���ָ��
	typeAddr addr_;                  //�����������Ӧ��ͨѶ���ĵ�ַ
	//bool bInitialized_;                    //����������Ƿ񱻳�ʼ����
	std::vector<stModbusCmd> ReadCmdSet_;  //������������
	std::vector<stModbusCmd> WriteCmdSet_; //������������
};

};//namespace Protocol 

