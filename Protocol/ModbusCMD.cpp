#include <boost/algorithm/string/predicate.hpp>
#include "ModbusCMD.h"

namespace Protocol {

#define strYxDataType "YxDataType"
#define strYcDataType "YcDataType"
#define strYkDataType "YkDataType"
#define strYmDataType "YmDataType"
#define strSynTimeType "SynTimeType"

CModbusCmdPara::CModbusCmdPara(typeAddr addr)
							  :addr_(addr)
{
	//bInitialized_ = false;

	InitDefaultPara();
}

CModbusCmdPara::CModbusCmdPara(const CModbusCmdPara & rhs)
{
	addr_ = rhs.addr_;
	//bInitialized_ = rhs.bInitialized_;
	ReadIndex_ = rhs.ReadIndex_;
	ReadCmdSet_ = rhs.ReadCmdSet_;
	WriteCmdSet_ = rhs.WriteCmdSet_;
}

CModbusCmdPara::~CModbusCmdPara(void)
{
}

CModbusCmdPara & CModbusCmdPara::operator = (const CModbusCmdPara & rhs)
{
	if (this == &rhs)
	{
		return *this;
	}

	addr_ = rhs.addr_;
	//bInitialized_ = rhs.bInitialized_;
	ReadIndex_ = rhs.ReadIndex_;
	ReadCmdSet_ = rhs.ReadCmdSet_;
	WriteCmdSet_ = rhs.WriteCmdSet_;

	return *this;
}

//************************************
// Method:    InitDefaultPara
// FullName:  Protocol::CModbusCmdPara::InitDefaultPara
// Access:    private 
// Returns:   int
// Qualifier: ��ʼ���������
//************************************
int CModbusCmdPara::InitDefaultPara()
{
	ReadIndex_ = 0;

	return 0;
}

//************************************
// Method:    AddReadCmd
// FullName:  Protocol::CModbusCmdPara::AddReadCmd
// Access:    public 
// Returns:   int
// Qualifier: ���һ�����Ĵ�������
// Parameter: stModbusCmd val��Modbus���������ݽṹ
//************************************
int CModbusCmdPara::AddReadCmd(stModbusCmd val)
{
	ReadCmdSet_.push_back(val);

	//bInitialized_ = true;

	return 0;
}

//************************************
// Method:    AddWriteCmd
// FullName:  Protocol::CModbusCmdPara::AddWriteCmd
// Access:    public 
// Returns:   int
// Qualifier: ���һ��д�Ĵ�������
// Parameter: stModbusCmd val��Modbus���������ݽṹ
//************************************
int CModbusCmdPara::AddWriteCmd(stModbusCmd val)
{
	WriteCmdSet_.push_back(val);

	//bInitialized_ = true;

	return 0;
}

//************************************
// Method:    DelReadCmd
// FullName:  Protocol::CModbusCmdPara::DelReadCmd
// Access:    public 
// Returns:   int��0ɾ���ɹ���other����
// Qualifier: ɾ��һ�����Ĵ�������
// Parameter: int index�������ڶ����е����
//************************************
int CModbusCmdPara::DelReadCmd(int index)
{
	if (index < 0 || index >= (int)ReadCmdSet_.size())
	{
		return -1;
	}

	ReadCmdSet_.erase(ReadCmdSet_.begin() + index);

	return 0;
}

//************************************
// Method:    DelWriteCmd
// FullName:  Protocol::CModbusCmdPara::DelWriteCmd
// Access:    public 
// Returns:   int��0ɾ���ɹ���other����
// Qualifier: ɾ��һ��д�Ĵ�������
// Parameter: int index�������ڶ��������
//************************************
int CModbusCmdPara::DelWriteCmd(int index)
{
	if (index < 0 || index >= (int)WriteCmdSet_.size())
	{
		return -1;
	}

	WriteCmdSet_.erase(WriteCmdSet_.begin() + index);

	return 0;
}

//************************************
// Method:    getNextReadCmd
// FullName:  Protocol::CModbusCmdPara::getNextReadCmd
// Access:    public 
// Returns:   int��0ִ�гɹ���other�޷���ȡָ������
// Qualifier: �����һ�����Ĵ�������
// Parameter: Protocol::stModbusCmd & val��������������
//************************************
int CModbusCmdPara::getNextReadCmd(Protocol::stModbusCmd &val)
{
	if (ReadCmdSet_.size() <= 0)
	{
		return -1;
	}

	val = ReadCmdSet_[ReadIndex_];

	ReadIndex_ = (ReadIndex_ + 1) % ReadCmdSet_.size();


	return 0;
}

//************************************
// Method:    getWriteCmd
// FullName:  Protocol::CModbusCmdPara::getWriteCmd
// Access:    public 
// Returns:   int��0ִ�гɹ���other�޷���ȡָ������
// Qualifier: �����������ƥ��д�Ĵ�������
// Parameter: unsigned char PointDataType���������ͱ�ʶ
// Parameter: size_t PointIndex�����ݵ��
// Parameter: stModbusCmd & val��������������
//************************************
int CModbusCmdPara::getWriteCmd(unsigned char PointDataType,size_t PointIndex,stModbusCmd & val)
{
	for (int i=0;i<(int)WriteCmdSet_.size();i++)
	{
		if((WriteCmdSet_[i].PointDataType_ == PointDataType)&&(WriteCmdSet_[i].PointIndex_ == PointIndex))
		{
			val = WriteCmdSet_[i];

			return 0;
		}
	}

	return -1;
}

//************************************
// Method:    getReadCmdSum
// FullName:  Protocol::CModbusCmdPara::getReadCmdSum
// Access:    public 
// Returns:   int
// Qualifier: ���ض��������������
//************************************
int CModbusCmdPara::getReadCmdSum()
{
	return ReadCmdSet_.size();
}

//************************************
// Method:    getWriteCmdSum
// FullName:  Protocol::CModbusCmdPara::getWriteCmdSum
// Access:    public 
// Returns:   int
// Qualifier: ����д�������������
//************************************
int CModbusCmdPara::getWriteCmdSum()
{
	return WriteCmdSet_.size();
}

//************************************
// Method:    getReadCMD
// FullName:  Protocol::CModbusCmdPara::getReadCMD
// Access:    public 
// Returns:   Protocol::stModbusCmd
// Qualifier: ������ֵ���һ��������
// Parameter: int index
//************************************
stModbusCmd CModbusCmdPara::getReadCMD(int index)
{
	if (index < 0 || index >= (int)ReadCmdSet_.size())
	{
		return stModbusCmd();
	}

	return ReadCmdSet_[index];
}

//************************************
// Method:    getWriteCMD
// FullName:  Protocol::CModbusCmdPara::getWriteCMD
// Access:    public 
// Returns:   Protocol::stModbusCmd
// Qualifier: ������ֵ���һ��д����
// Parameter: int index
//************************************
stModbusCmd CModbusCmdPara::getWriteCMD(int index)
{
	if (index < 0 || index >= (int)WriteCmdSet_.size())
	{
		return stModbusCmd();
	}

	return WriteCmdSet_[index];
}

//************************************
// Method:    TransDataTypeToString
// FullName:  Protocol::CModbusCmdPara::TransDataTypeToString
// Access:    public 
// Returns:   std::string
// Qualifier: cmd������������ʹӳ���굽xml�ļ��ַ�ֵ��ת��
// Parameter: char val
//************************************
std::string CModbusCmdPara::TransDataTypeToString(char val)
{
	std::string ret = "";

	switch (val)
	{
	case YxDataType:
		ret = strYxDataType;
		break;

	case YcDataType:
		ret = strYcDataType;
		break;

	case YkDataType:
		ret = strYkDataType;
		break;

	case YmDataType:
		ret = strYmDataType;
		break;

	case SynTimeType:
		ret = strSynTimeType;
		break;

	default:
		ret = "";
		break;
	}

	return ret;
}

//************************************
// Method:    TransDataTypeFromString
// FullName:  Protocol::CModbusCmdPara::TransDataTypeFromString
// Access:    public 
// Returns:   int
// Qualifier: cmd������������ʹ�xml�ļ��ַ�ֵ��������ת��
// Parameter: std::string val
//************************************
int CModbusCmdPara::TransDataTypeFromString(std::string val)
{
	int ret = -1;

	if (boost::iequals(strYxDataType,val))
	{
		ret = YxDataType;
	}
	else if (boost::iequals(strYcDataType,val))
	{
		ret = YcDataType;
	}
	else if (boost::iequals(strYkDataType,val))
	{
		ret = YkDataType;
	}
	else if (boost::iequals(strYmDataType,val))
	{
		ret = YmDataType;
	}
	else if (boost::iequals(strSynTimeType,val))
	{
		ret = SynTimeType;
	}

	return ret;
}

//************************************
// Method:    getbInit
// FullName:  Protocol::CModbusCmdPara::getbInit
// Access:    public 
// Returns:   bool
// Qualifier: �ж���������Ƿ��Ѿ�����ʼ����
//************************************
//bool CModbusCmdPara::getbInit()
//{
//	return bInitialized_;
//}

//************************************
// Method:    getAddr
// FullName:  Protocol::CModbusCmdPara::getAddr
// Access:    public 
// Returns:   unsigned short
// Qualifier: �������������Ӧ��ͨѶ���ĵ�ַ
//************************************
typeAddr CModbusCmdPara::getAddr()
{
	return addr_;
}
};//namespace Protocol 
