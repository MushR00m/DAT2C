#include <boost/bind.hpp>
#include "Modbus.h"
#include "../PublicSupport/Dat2cPublicAPI.h"
#include "../FileSystem/Markup.h"
#include "../DataBase/Terminal.h"
#include "../DataBase/YkPoint.h"
#include "../DataBase/YxPoint.h"

namespace Protocol {

const std::string strDefaultCfg = "ModbusCfg.xml";
size_t CModbus::ModbusObjectCounter_ = 1;

const unsigned char TCP_MODE_HEAD_LENGTH = 6;

const unsigned char RTU_MODE = 1;
const unsigned char TCP_MODE = 2;
const unsigned char ASCII_MODE = 3;

//funCode ����
const unsigned char ReadDO = 0x01;         //����Ȧ
const unsigned char ReadDI = 0x02;         //��������ɢ��
const unsigned char ReadAO = 0x03;         //�����ּĴ���
const unsigned char ReadAI = 0x04;         //������Ĵ���
const unsigned char WriteDO = 0x05;        //д������Ȧ
const unsigned char WriteAO = 0x06;        //д�������ּĴ���
const unsigned char WriteDOs = 0x0f;       //д�����Ȧ
const unsigned char WriteAOs = 0x10;       //д������ּĴ���
const unsigned char ReadGRR = 0x14;        //���ļ���¼
const unsigned char WriteGRR = 0x15;       //д�ļ���¼

//ң�طֺ�բ����
const unsigned short YK_TYPE_OPEN = 0;
const unsigned short YK_TYPE_CLOSE = 0xff00;

//xml ��㶨��
#define strErrCodeLength "ErrCodeLength"
#define strSingleDataLength "SingleDataLength"
#define strCrcLength "CrcLength"
#define strMode "Mode"
#define strRtuMode "RTU_MODE"
#define strTcpMode "TCP_MODE"
#define strASCIIMode "ASCII_MODE"
#define strLowDataFirst "LowDataFirst"
#define strLowCrcFirst "LowCrcFirst"
#define strModbusCMD "ModbusCMD"
#define strCommPoint "CommPoint"
#define strCommPointAddr "Addr"
#define strReadCMD "ReadCMD"
#define strWriteCMD "WriteCMD"
#define strFunCode "FunCode"
#define strRegisterAddr "RegisterAddr"
#define strDataNum "DataNum"
#define strDataType "DataType"
#define strDataIndex "DataIndex"

//************************************
// Method:    CModbus
// FullName:  Protocol::CModbus::CModbus
// Access:    public 
// Returns:   
// Qualifier: :CProtocol(io_service)
// Parameter: boost::asio::io_service & io_service
//************************************
CModbus::CModbus(boost::asio::io_service & io_service)
				:CProtocol(io_service)
{
	SynCharNum_ = 4;

	InitObjectIndex();
	InitDefaultDefPara();
	InitDefaultFrameElem();
	InitDefaultTimer(io_service);

	LoadXmlCfg(strDefaultCfg);

}

CModbus::~CModbus(void)
{
	ModbusObjectCounter_--;
}

//************************************
// Method:    LoadXmlCfg
// FullName:  Protocol::CModbus::LoadXmlCfg
// Access:    public 
// Returns:   int��0ִ�гɹ���other����
// Qualifier: ����xml�����ļ�����ʼ������
// Parameter: std::string filename��xml�ļ���
//************************************
int CModbus::LoadXmlCfg(std::string filename)
{
	FileSystem::CMarkup xml;

	if (!xml.Load(filename))
	{
		return -1;
	}

	xml.ResetMainPos();
	xml.FindElem();  //root strProtocolRoot
	xml.IntoElem();  //enter strProtocolRoot

	CProtocol::LoadXmlCfg(xml);

	xml.ResetMainPos();
	if (xml.FindElem(strMode))
	{
		std::string strTmp = xml.GetData();
		boost::algorithm::trim(strTmp);
		setModbusMode(TransModeFromString(strTmp));
	}

	xml.ResetMainPos();
	if (xml.FindElem(strLowDataFirst))
	{
		std::string strTmp = xml.GetData();
		boost::algorithm::trim(strTmp);
		if (boost::iequals(strboolTrue,strTmp))
		{
			bLowDataFirst_ = true;
		}
		else
		{
			bLowDataFirst_ = false;
		}
	}
	
	xml.ResetMainPos();
	if (xml.FindElem(strLowCrcFirst))
	{
		std::string strTmp = xml.GetData();
		boost::algorithm::trim(strTmp);
		if (boost::iequals(strboolFalse,strTmp))
		{
			bLowCRCFirst_ = false;
		}
		else
		{
			bLowCRCFirst_ = true;
		}
	}

	xml.ResetMainPos();
	if (xml.FindElem(strFrameElemLength))
	{
		xml.IntoElem(); //enter strFrameElemLength

		xml.ResetMainPos();
		if (xml.FindElem(strFrameTypeLength))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short len = boost::lexical_cast<unsigned short>(strTmp);
				setFunCodeLength(len);
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setFunCodeLength(DEFAULT_FunCodeLength);
			}
		}

		xml.ResetMainPos();
		if (xml.FindElem(strInfoNumLength))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short len = boost::lexical_cast<unsigned short>(strTmp);
				setByteCountLength(len);
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setByteCountLength(DEFAULT_ByteCountLength);
			}
		}

		xml.ResetMainPos();
		if (xml.FindElem(strAsduAddrLength))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short len = boost::lexical_cast<unsigned short>(strTmp);
				setDeviceAddrLength(len);
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setDeviceAddrLength(DEFAULT_DeviceAddrLength);
			}
		}

		xml.ResetMainPos();
		if (xml.FindElem(strSingleDataLength))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short len = boost::lexical_cast<unsigned short>(strTmp);
				setSingleDataLength(len);
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setSingleDataLength(DEFAULT_SingleDataLength);
			}
		}

		xml.ResetMainPos();
		if (xml.FindElem(strErrCodeLength))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short len = boost::lexical_cast<unsigned short>(strTmp);
				setErrCodeLength(len);
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setErrCodeLength(DEFAUTL_ErrCodeLength);
			}
		}

		xml.ResetMainPos();
		if (xml.FindElem(strCrcLength))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short len = boost::lexical_cast<unsigned short>(strTmp);
				setCrcLength(len);
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setCrcLength(DEFAULT_CheckCrcLength);
			}
		}

		xml.ResetMainPos();
		if (xml.FindElem(strInfoAddrLength))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short len = boost::lexical_cast<unsigned short>(strTmp);
				setRegisterAddrLength(len);
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setRegisterAddrLength(DEFAULT_RegisterAddrLength);
			}
		}

		xml.OutOfElem();//out strFrameElemLength
	}

	xml.ResetMainPos();
	if (xml.FindElem(strTimer))
	{
		xml.IntoElem();  //enter strTimer

		xml.ResetMainPos();
		if (xml.FindElem(strTimeOutSynTime))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short timeout = boost::lexical_cast<unsigned short>(strTmp);
				setTimeOutSynTime(timeout);
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setTimeOutSynTime(DEFAULT_timeOutSynTime);
			}
		}

		xml.ResetMainPos();
		if (xml.FindElem(strTimeOutHeartFrame))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short timeout = boost::lexical_cast<unsigned short>(strTmp);
				setTimeOutHeartFrame(timeout);
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setTimeOutHeartFrame(DEFAULT_timeOutHeartFrame);
			}
		}

		xml.ResetMainPos();
		if (xml.FindElem(strTimeOutYkExe))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short timeout = boost::lexical_cast<unsigned short>(strTmp);
				setTimeOutYkExe(timeout);
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setTimeOutYkExe(DEFAULT_timeOutYkExe);
			}
		}

		xml.OutOfElem(); //out strTimer
	}

	xml.ResetMainPos();
	if (xml.FindElem(strModbusCMD))
	{
		xml.IntoElem();  //enter strModbusCMD

		while (xml.FindElem(strCommPoint))
		{
			boost::scoped_ptr<CModbusCmdPara> para;
			para.reset();

			xml.IntoElem();  //enter strCommPoint
			if (xml.FindElem(strCommPointAddr))
			{
				std::string strTmp = xml.GetData();
				boost::algorithm::trim(strTmp);
				try
				{
					unsigned short addr = boost::lexical_cast<unsigned short>(strTmp);
					para.reset(new CModbusCmdPara(addr));
					
				}
				catch(boost::bad_lexical_cast& e)
				{
					std::ostringstream ostr;
					ostr<<e.what();
				}
			}

			if (!para)
			{
				continue;
			}

			while (xml.FindElem(strReadCMD))
			{
				stModbusCmd cmdTmp;
				
				xml.IntoElem(); //enter strReadCMD
				int ret = LoadModbusCmd(xml,cmdTmp);
				xml.OutOfElem();//out strReadCMD

				if (!ret)
				{
					para->AddReadCmd(cmdTmp);
				}
			}
			
			while (xml.FindElem(strWriteCMD))
			{
				stModbusCmd cmdTmp;

				xml.IntoElem(); //enter strWriteCMD
				LoadModbusCmd(xml,cmdTmp);
				xml.OutOfElem();//out strWriteCMD

				para->AddWriteCmd(cmdTmp);
			}

			xml.OutOfElem(); //out strCommPoint

			modbusCmdPara_.push_back(*para);
		}

		xml.OutOfElem(); //out strModbusCMD
	}

	xml.OutOfElem();  //out strProtocolRoot

	return 0;
}

//************************************
// Method:    SaveXmlCfg
// FullName:  Protocol::CModbus::SaveXmlCfg
// Access:    public 
// Returns:   void
// Qualifier: ����ǰ���ñ���Ϊxml�ļ�
// Parameter: std::string filename��xml�ļ���
//************************************
void CModbus::SaveXmlCfg(std::string filename)
{
	FileSystem::CMarkup xml;
	xml.SetDoc(strXmlHead);
	xml.SetDoc(strProtocolXsl);

	xml.AddElem(strProtocolRoot);
	xml.IntoElem();                   //enter strProtocolRoot

	CProtocol::SaveXmlCfg(xml);

	xml.AddElem(strMode,TransModeToString(mode_));

	if (bLowDataFirst_)
	{
		xml.AddElem(strLowDataFirst,strboolTrue);
	}

	if (!bLowCRCFirst_)
	{
		xml.AddElem(strLowCrcFirst,strboolFalse);
	}

	xml.AddElem(strFrameElemLength);
	bool bSave = false;
	xml.IntoElem();

	if (FunCodeLength_ != DEFAULT_FunCodeLength)
	{
		xml.AddElem(strFrameTypeLength,FunCodeLength_);
		bSave = true;
	}

	if (ByteCountLength_ != DEFAULT_ByteCountLength)
	{
		xml.AddElem(strInfoNumLength,ByteCountLength_);
		bSave = true;
	}

	if (ErrCodeLength_ != DEFAUTL_ErrCodeLength)
	{
		xml.AddElem(strErrCodeLength,ErrCodeLength_);
		bSave = true;
	}

	if (SingleDataLength_ != DEFAULT_SingleDataLength)
	{
		xml.AddElem(strSingleDataLength,SingleDataLength_);
		bSave = true;
	}

	if (CRCLength_ != DEFAULT_CheckCrcLength)
	{
		xml.AddElem(strCrcLength,CRCLength_);
		bSave = true;
	}

	if (DeviceAddrLength_ != DEFAULT_DeviceAddrLength)
	{
		xml.AddElem(strAsduAddrLength,DeviceAddrLength_);
		bSave = true;
	}

	if (RegisterAddrLength_ != DEFAULT_RegisterAddrLength)
	{
		xml.AddElem(strInfoAddrLength,RegisterAddrLength_);
		bSave = true;
	}

	xml.OutOfElem();
	if (!bSave)
	{
		xml.RemoveElem();
	}

	xml.AddElem(strTimer);
	bSave = false;
	xml.IntoElem();  //enter strTimer

	if (timeOutSynTime_ != DEFAULT_timeOutSynTime)
	{
		xml.AddElem(strTimeOutSynTime,timeOutSynTime_);
		bSave = true;
	}

	if (timeOutHeartFrame_ != DEFAULT_timeOutHeartFrame)
	{
		xml.AddElem(strTimeOutHeartFrame,timeOutHeartFrame_);
		bSave = true;
	}

	if (timeOutYkExe_ != DEFAULT_timeOutYkExe)
	{
		xml.AddElem(strTimeOutYkExe,timeOutYkExe_);
		bSave = true;
	}

	xml.OutOfElem(); //out strTimer
	if (!bSave)
	{
		xml.RemoveElem();
	}

	if (modbusCmdPara_.size() > 0)
	{
		bSave = false;
		xml.AddElem(strModbusCMD);
		xml.IntoElem(); //enter strModbusCMD
		for (size_t i=0;i<modbusCmdPara_.size();i++)
		{
			if (modbusCmdPara_[i].getReadCmdSum() > 0 || modbusCmdPara_[i].getWriteCmdSum() > 0)
			{
				bSave = true;
				xml.AddElem(strCommPoint); 
				xml.IntoElem();            //enter strCommPoint
				xml.AddElem(strCommPointAddr,modbusCmdPara_[i].getAddr());
				for (int j=0;j<modbusCmdPara_[i].getReadCmdSum();j++)
				{
					stModbusCmd cmdTmp = modbusCmdPara_[i].getReadCMD(j);

					xml.AddElem(strReadCMD); 
					SaveModbusCmd(xml,cmdTmp);
					xml.OutOfElem();         //out strReadCMD
				}
				for (int j=0;j<modbusCmdPara_[i].getWriteCmdSum();j++)
				{
					stModbusCmd cmdTmp = modbusCmdPara_[i].getWriteCMD(j);

					xml.AddElem(strReadCMD); 
					xml.IntoElem();          //enter strWriteCMD
					SaveModbusCmd(xml,cmdTmp);
					xml.OutOfElem();         //out strWriteCMD
				}

				xml.OutOfElem();           //out strCommPoint
			}
		}

		xml.OutOfElem();//out strModbusCMD
		if (!bSave)
		{
			xml.RemoveElem();
		}
	}

	xml.OutOfElem();          //out strProtocolRoot

	xml.Save(filename);
}

int CModbus::LoadModbusCmd(FileSystem::CMarkup & xml,stModbusCmd & cmd)
{
	int ret = 0;

	if (xml.FindElem(strFunCode))
	{
		std::string strTmp = xml.GetData();
		boost::algorithm::trim(strTmp);
		try
		{
			unsigned short funCode = boost::lexical_cast<unsigned short>(strTmp);
			cmd.FunCode_ = funCode;
		}
		catch(boost::bad_lexical_cast& e)
		{
			e.what();
			ret = -1;
		}
	}
	else
	{
		ret = -1;
	}

	if (xml.FindElem(strRegisterAddr))
	{
		std::string strTmp = xml.GetData();
		boost::algorithm::trim(strTmp);
		try
		{
			unsigned short registerAddr = boost::lexical_cast<unsigned short>(strTmp);
			cmd.RegisterAddr_ = registerAddr;
		}
		catch(boost::bad_lexical_cast& e)
		{
			e.what();
			ret = -1;
		}
	}
	else
	{
		ret = -1;
	}

	if (xml.FindElem(strDataNum))
	{
		std::string strTmp = xml.GetData();
		boost::algorithm::trim(strTmp);
		try
		{
			unsigned short dataNum = boost::lexical_cast<unsigned short>(strTmp);
			cmd.DataNum_ = dataNum;
		}
		catch(boost::bad_lexical_cast& e)
		{
			e.what();
			ret = -1;
		}
	}
	else
	{
		ret = -1;
	}

	if (xml.FindElem(strDataType))
	{
		std::string strTmp = xml.GetData();
		boost::algorithm::trim(strTmp);

		char dataType = CModbusCmdPara::TransDataTypeFromString(strTmp);

		if (dataType >= 0)
		{
			cmd.PointDataType_ = dataType;
		}
		else
		{
			ret = -1;
		}
	}
	else
	{
		ret = -1;
	}

	if (xml.FindElem(strDataIndex))
	{
		std::string strTmp = xml.GetData();
		boost::algorithm::trim(strTmp);
		try
		{
			size_t dataIndex = boost::lexical_cast<size_t>(strTmp);
			cmd.PointIndex_ = dataIndex;
		}
		catch(boost::bad_lexical_cast& e)
		{
			e.what();
			ret = -1;
		}
	}
	else
	{
		ret = -1;
	}

	return ret;
}

void CModbus::SaveModbusCmd(FileSystem::CMarkup & xml,stModbusCmd & cmd)
{
	xml.AddElem(strFunCode,cmd.FunCode_);
	xml.AddElem(strRegisterAddr,cmd.RegisterAddr_);
	xml.AddElem(strDataNum,cmd.DataNum_);
	xml.AddElem(strDataType,CModbusCmdPara::TransDataTypeToString(cmd.PointDataType_));
	xml.AddElem(strDataIndex,cmd.RegisterAddr_);
}

//************************************
// Method:    InitProtocol
// FullName:  Protocol::CModbus::InitProtocol
// Access:    virtual protected 
// Returns:   int
// Qualifier: ��ʼ����Լ������ͨ���������ӵ�ʱ�����
//************************************
int CModbus::InitProtocol()
{
	CProtocol::InitProtocol();

	switch (mode_)
	{
	case TCP_MODE:
		InitFrameLocation(TCP_MODE_HEAD_LENGTH);
		break;

	default:
		InitFrameLocation(0);
		break;
	}
	
	if(getCommPointSum() > 0)
	{
		share_commpoint_ptr nextPoint = getNextCommPoint(TERMINAL_NODE,false,getCommPointSum() - 1);
//		share_commpoint_ptr nextPoint = getFirstCommPoint();
		if (nextPoint)
		{
			AddSendCmdVal(SYN_TIME_ACT,SYN_TIME_ACT_PRIORITY,nextPoint);
			AddSendCmdVal(QUERY_DATA,QUERY_DATA_PRIORITY,nextPoint);
		}
	}

	AddStatusLogWithSynT("Modbus��Լ��ͨ���򿪳ɹ���\n");

	return 0;
}

//************************************
// Method:    UninitProtocol
// FullName:  Protocol::CModbus::UninitProtocol
// Access:    virtual protected 
// Returns:   void
// Qualifier: ����ʼ����Լ������ͨ���Ͽ����ӵ�ʱ�����
//************************************
void CModbus::UninitProtocol()
{
	CProtocol::UninitProtocol();

	AddStatusLogWithSynT("Modbus��Լ��ͨ���رճɹ���\n");
}

//************************************
// Method:    InitObjectIndex
// FullName:  Protocol::CModbus::InitObjectIndex
// Access:    private 
// Returns:   void
// Qualifier: ��ʼ������ʵ���ļ�����
//************************************
void CModbus::InitObjectIndex()
{
	MessageNO_ = 0;

	ProtocolObjectIndex_ = ModbusObjectCounter_++;
}

//************************************
// Method:    InitDefaultDefPara
// FullName:  Protocol::CModbus::InitDefaultDefPara
// Access:    private 
// Returns:   void
// Qualifier: ��ʼ������
//************************************
void CModbus::InitDefaultDefPara()
{
	mode_ = RTU_MODE;
	
	bLowDataFirst_ = false;
	bLowCRCFirst_ = true;
}

//************************************
// Method:    InitDefaultFrameElem
// FullName:  Protocol::CModbus::InitDefaultFrameElem
// Access:    private 
// Returns:   void
// Qualifier: ��ʼ������Ԫ�ص��ֽڳ���
//************************************
void CModbus::InitDefaultFrameElem()
{
	DeviceAddrLength_ = DEFAULT_DeviceAddrLength;
	FunCodeLength_ = DEFAULT_FunCodeLength;
	ErrCodeLength_ = DEFAUTL_ErrCodeLength;
	ByteCountLength_ = DEFAULT_ByteCountLength;
	RegisterAddrLength_ = DEFAULT_RegisterAddrLength;
	SingleDataLength_ = DEFAULT_SingleDataLength;
	CRCLength_ = DEFAULT_CheckCrcLength;

	//InitFrameLocation(0);
}

//************************************
// Method:    InitFrameLocation
// FullName:  Protocol::CModbus::InitFrameLocation
// Access:    private 
// Returns:   void
// Qualifier: ��ʼ������Ԫ�ص��ֽ�λ��
// Parameter: size_t FrameHead
//************************************
void CModbus::InitFrameLocation(size_t FrameHead)
{
	DeviceAddrLocation_ = FrameHead;
	FunCodeLocation_ = DeviceAddrLocation_ + DeviceAddrLength_;
	DataLocation_ = FunCodeLocation_ + FunCodeLength_;
}

//************************************
// Method:    InitDefaultTimeOut
// FullName:  Protocol::CModbus::InitDefaultTimeOut
// Access:    private 
// Returns:   void
// Qualifier: ��ʼ��������ʱ���ĳ�ʱʱ��
//************************************
void CModbus::InitDefaultTimeOut()
{
	timeOutHeartFrame_ = DEFAULT_timeOutHeartFrame;
	timeOutSynTime_ = DEFAULT_timeOutSynTime;
	timeOutYkExe_ = DEFAULT_timeOutYkExe;
}

//************************************
// Method:    InitDefaultTimer
// FullName:  Protocol::CModbus::InitDefaultTimer
// Access:    private 
// Returns:   void
// Qualifier: ��ʼ����ʱ��
// Parameter: boost::asio::io_service & io_service��IO���������
//************************************
void CModbus::InitDefaultTimer(boost::asio::io_service & io_service)
{
	InitDefaultTimeOut();

	using namespace boost::asio;
	using namespace boost::posix_time;

	timerSynTime_.reset(new deadline_timer(io_service,seconds(getMeanvalueOfPointsSum(MIN_timeOutSynTime,timeOutSynTime_))));
	AddTimer(timerSynTime_);

	timerHeartFrame_.reset(new deadline_timer(io_service,seconds(timeOutHeartFrame_)));
	AddTimer(timerHeartFrame_);

	timerYkExe_.reset(new deadline_timer(io_service,seconds(timeOutYkExe_)));
	AddTimer(timerYkExe_);
}

//************************************
// Method:    getCurMessageNO
// FullName:  Protocol::CModbus::getCurMessageNO
// Access:    private 
// Returns:   unsigned short
// Qualifier: TCP_MODEģʽ�У���÷�����Ϣ�ļ�����
//************************************
unsigned short CModbus::getCurMessageNO()
{
	MessageNO_ = (++MessageNO_)%0xffff;

	return MessageNO_;
}

//************************************
// Method:    getCmdParaIndexByAddr
// FullName:  Protocol::CModbus::getCmdParaIndexByAddr
// Access:    private 
// Returns:   int
// Qualifier: ���ݵ�ַ��ò�������
// Parameter: unsigned short addr
//************************************
int CModbus::getCmdParaIndexByAddr(unsigned short addr)
{
	for (size_t i=0;i<modbusCmdPara_.size();i++)
	{
		if (addr == modbusCmdPara_[i].getAddr())
		{
			return i;
		}
	}

	return -1;
}

//************************************
// Method:    AddCommPoint
// FullName:  Protocol::CModbus::AddCommPoint
// Access:    virtual protected 
// Returns:   int��0ִ�гɹ���other����
// Qualifier: ���Լ�������һ��ͨѶ���ָ��
// Parameter: share_commpoint_ptr val��ָ��ͨѶ����ָ��
//************************************
//int CModbus::AddCommPoint(share_commpoint_ptr val)
//{
//	int ret = CProtocol::AddCommPoint(val);

//	CModbusCmdPara para(val->getAddr());
//	modbusCmdPara_.push_back(para);

	//LoadXmlCfg(strDefaultCfg);

//	return ret;
//}

//************************************
// Method:    AddCommPoint
// FullName:  Protocol::CModbus::AddCommPoint
// Access:    virtual protected 
// Returns:   int��0ִ�гɹ���other����
// Qualifier: ���Լ�������һ��ͨѶ���ָ��
// Parameter: share_commpoint_ptr val��ָ��ͨѶ����ָ��
//************************************
//int CModbus::AddCommPoint(weak_commpoint_ptr val)
//{
//	int ret = -1;

//	share_commpoint_ptr point = val.lock();

//	if (point)
//	{
//		ret = CProtocol::AddCommPoint(val);

//		CModbusCmdPara para(point->getAddr());
//		modbusCmdPara_.push_back(para);

		//LoadXmlCfg(strDefaultCfg);
//	}
	

//	return ret;
//}

//************************************
// Method:    DelCommPoint
// FullName:  Protocol::CModbus::DelCommPoint
// Access:    virtual protected 
// Returns:   int��0ִ�гɹ���other����
// Qualifier: �ӹ�Լ����ɾ��һ��ͨѶ����ָ��
// Parameter: int index��ͨѶ����������е����
//************************************
//int CModbus::DelCommPoint(int index)
//{
//	if (index >= 0 && index < (int)modbusCmdPara_.size())
//	{
//		modbusCmdPara_.erase(modbusCmdPara_.begin() + index);
//	}

//	return CProtocol::DelCommPoint(index);
//}

//frame api

//************************************
// Method:    CheckFrameHead
// FullName:  Protocol::CModbus::CheckFrameHead
// Access:    virtual protected 
// Returns:   int
// Qualifier: У�鱨��ͷ
// Parameter: unsigned char * buf��ָ���Ļ�������ָ��
// Parameter: size_t & exceptedBytes������������ڴ���֡���ĵ��ֽ���
//************************************
int CModbus::CheckFrameHead(unsigned char * buf,size_t & exceptedBytes)
{
	int ret = -1;

	switch (mode_)
	{
	case TCP_MODE:
		ret = CheckFrameHead_TCPMode(buf,exceptedBytes);
		break;

	default:
		ret = CheckFrameHead_RTUMode(buf,exceptedBytes);
		break;
	}

	return ret;
}

//************************************
// Method:    CheckFrameHead_TCPMode
// FullName:  Protocol::CModbus::CheckFrameHead_TCPMode
// Access:    virtual protected 
// Returns:   int
// Qualifier: У�鱨��ͷ��TCP��ʽ
// Parameter: unsigned char * buf��ָ���Ļ�������ָ��
// Parameter: size_t & exceptedBytes������������ڴ���֡���ĵ��ֽ���
//************************************
int CModbus::CheckFrameHead_TCPMode(unsigned char * buf,size_t & exceptedBytes)
{
	int len = BufToVal(&buf[TCP_MODE_HEAD_LENGTH - 2],2);

	exceptedBytes = len + TCP_MODE_HEAD_LENGTH;

	return 0;
}

//************************************
// Method:    CheckFrameHead_RTUMode
// FullName:  Protocol::CModbus::CheckFrameHead_RTUMode
// Access:    virtual protected 
// Returns:   int
// Qualifier: У�鱨��ͷ��RTU��ʽ
// Parameter: unsigned char * buf��ָ���Ļ�������ָ��
// Parameter: size_t & exceptedBytes������������ڴ���֡���ĵ��ֽ���
//************************************
int CModbus::CheckFrameHead_RTUMode(unsigned char * buf,size_t & exceptedBytes)
{
	int funCode = BufToVal(&buf[FunCodeLocation_],FunCodeLength_,bLowDataFirst_);

	if(((funCode >> ((FunCodeLength_ - 1) * 8)) & 0x80) > 0)
	{
		exceptedBytes = DeviceAddrLength_ + FunCodeLength_ + ErrCodeLength_ + CRCLength_;
		return 0;
	}

	int ret = -1;

	switch (funCode)
	{
	case WriteDO:
		exceptedBytes = DeviceAddrLength_ + FunCodeLength_ + RegisterAddrLength_ + SingleDataLength_ + CRCLength_;
		ret = 0;
		break;

	case WriteAO:
		exceptedBytes = DeviceAddrLength_ + FunCodeLength_ + RegisterAddrLength_ + SingleDataLength_ + CRCLength_;
		ret = 0;
		break;

	case WriteDOs:
		exceptedBytes = DeviceAddrLength_ + FunCodeLength_ + RegisterAddrLength_ + SingleDataLength_ + CRCLength_;
		ret = 0;
		break;

	case WriteAOs:
		exceptedBytes = DeviceAddrLength_ + FunCodeLength_ + RegisterAddrLength_ + SingleDataLength_ + CRCLength_;
		ret = 0;
		break;

	default:
		{
			int byteCount = BufToVal(&buf[DataLocation_],ByteCountLength_,bLowDataFirst_);
			if (byteCount >= 0)
			{
				exceptedBytes = DeviceAddrLength_ + FunCodeLength_ + ByteCountLength_ + byteCount + CRCLength_;
				ret = 0;
			}
			else
			{
				exceptedBytes = 0;
				ret = -1;
			}
		}
		break;
	}

	return ret;
}

//************************************
// Method:    CheckFrameTail
// FullName:  Protocol::CModbus::CheckFrameTail
// Access:    virtual protected 
// Returns:   int
// Qualifier: У�鱨��β
// Parameter: unsigned char * buf��ָ���Ļ�������ָ��
// Parameter: size_t exceptedBytes����֡���ĵĳ���
//************************************
int CModbus::CheckFrameTail(unsigned char * buf,size_t exceptedBytes)
{
	int ret = -1;

	switch (mode_)
	{
	case TCP_MODE:
		ret = CheckFrameTail_TCPMode(buf,exceptedBytes);
		break;

	default:
		ret = CheckFrameTail_RTUMode(buf,exceptedBytes);
		break;
	}

	return ret;
}

//************************************
// Method:    CheckFrameTail_TCPMode
// FullName:  Protocol::CModbus::CheckFrameTail_TCPMode
// Access:    virtual protected 
// Returns:   int
// Qualifier: У�鱨��β��TCP��ʽ
// Parameter: unsigned char * buf��ָ���Ļ�������ָ��
// Parameter: size_t exceptedBytes����֡���ĵĳ���
//************************************
int CModbus::CheckFrameTail_TCPMode(unsigned char * buf,size_t exceptedBytes)
{
	return 0;
}

//************************************
// Method:    CheckFrameTail_RTUMode
// FullName:  Protocol::CModbus::CheckFrameTail_RTUMode
// Access:    virtual protected 
// Returns:   int
// Qualifier: У�鱨��β��RTU��ʽ
// Parameter: unsigned char * buf��ָ���Ļ�������ָ��
// Parameter: size_t exceptedBytes����֡���ĵĳ���
//************************************
int CModbus::CheckFrameTail_RTUMode(unsigned char * buf,size_t exceptedBytes)
{
	if (exceptedBytes <= CRCLength_)
	{
		return -1;
	}

	if (CRCLength_ == 2)
	{
		int CalcCRC = CRC16(buf,exceptedBytes - CRCLength_);
		int FrameCRC = BufToVal(&buf[exceptedBytes - CRCLength_],CRCLength_,bLowCRCFirst_);

		if (CalcCRC == FrameCRC)
		{
			return 0;
		}
	}

	return -1;
}

//************************************
// Method:    ParseFrameBody
// FullName:  Protocol::CModbus::ParseFrameBody
// Access:    virtual protected 
// Returns:   int
// Qualifier: ������������
// Parameter: unsigned char * buf��ָ���Ļ�������ָ��
// Parameter: size_t exceptedBytes����֡���ĵĳ���
//************************************
int CModbus::ParseFrameBody(unsigned char * buf,size_t exceptedBytes)
{
	int ret = 0;

	share_commpoint_ptr val = getCommPoint(getLastSendPointIndex()).lock();
	if (!val)
	{
		std::ostringstream ostr;
		ostr<<"Modbus��Լδ����ȷ��¼��һ֡���ͱ��ĵ�ͨѶ���,��֡���Ľ����ᱻ������"<<std::endl;
		AddStatusLogWithSynT(ostr.str());
		return -1;
	}

	unsigned short addr = BufToVal(&buf[DeviceAddrLocation_],DeviceAddrLength_,bLowDataFirst_);
	if(val->getAddr() != addr)
	{
		std::ostringstream ostr;
		ostr<<"Modbus��Լ��¼����һ֡���ͱ��ĵ�ַ�ͱ��ν��յ����ĵ�ַ��һ��,��֡���Ľ����ᱻ������"<<std::endl;
		AddStatusLogWithSynT(ostr.str());
		return -1;
	}

	setLastRecvPointIndex(getLastSendPointIndex());

	unsigned short funCode = BufToVal(&buf[FunCodeLocation_],FunCodeLength_,bLowDataFirst_);
	if (curCmd_.FunCode_ != funCode)
	{
		std::ostringstream ostr;
		ostr<<"Modbus��Լ��¼����һ֡���ͱ��ĵĹ�����ͱ��ν��յ����ĵ�ַ��һ��,��֡���Ľ����ᱻ������"<<std::endl;
		AddStatusLogWithSynT(ostr.str());
		return -1;
	}

	share_terminal_ptr terminalPtr = boost::dynamic_pointer_cast<DataBase::CTerminal>(val);
	if (!terminalPtr)
	{
		std::ostringstream ostr;
		ostr<<"Modbus��Լ���ܸ��ݽ��ձ����еĵ�ַƥ��terminal ptr,��֡���Ľ����ᱻ������"<<std::endl;
		AddStatusLogWithSynT(ostr.str());

		return -1;
	}

	switch (funCode)
	{
	case ReadDI:
		ParseReadDI(buf,terminalPtr);
		break;

	case ReadDO:
		ParseReadDO(buf,terminalPtr);
		break;

	case ReadAO:
		ParseReadAO(buf,terminalPtr);
		break;

	case ReadAI:
		ParseReadAI(buf,terminalPtr);
		break;

	default:
		break;
	}

	if (ret < 0)
	{
		return ret;
	}
	
	return getLastSendPointIndex();
}

//************************************
// Method:    ParseReadDI
// FullName:  Protocol::CModbus::ParseReadDI
// Access:    private 
// Returns:   int
// Qualifier: ����DI���ݱ���
// Parameter: unsigned char * buf�����Ļ�����ָ��
// Parameter: share_terminal_ptr terminalPtr����Ӧ�ն�ָ��
//************************************
int CModbus::ParseReadDI(unsigned char * buf,share_terminal_ptr terminalPtr)
{
	if (curCmd_.PointDataType_ != YxDataType)
	{
		return -1;
	}

	int info_addr = curCmd_.PointIndex_;
	int byteCount = BufToVal(&buf[DataLocation_],ByteCountLength_,bLowDataFirst_);

	int count = 0;
	for (int i=0;i<byteCount;i++)
	{
		unsigned char YxByteVal = buf[DataLocation_ + 1 + i];
		for (int j=0;j<8;j++)
		{
			if ((YxByteVal & BYTE_CHECK_TRUE[j]) > 0)
			{
				terminalPtr->SaveYxType(info_addr + i*16 + j,DataBase::single_yx_point);
				int ret = terminalPtr->SaveOriYxVal(info_addr + i*16 + j,1,true);
				if (ret == DataBase::CauseActiveData)
				{
					count++;
				}
			}
			else
			{
				terminalPtr->SaveYxType(info_addr + i*16 + j,DataBase::single_yx_point);
				int ret = terminalPtr->SaveOriYxVal(info_addr + i*16 + j,0,true);
				if (ret == DataBase::CauseActiveData)
				{
					count++;
				}
			}
		}
	}

	if (count > 0)
	{
		std::ostringstream ostr;
		ostr<<"DI���ݱ��Ĳ�����COS����Ŀ��"<<count<<std::endl;
		AddStatusLogWithSynT(ostr.str());
		CmdConSig_(COS_DATA_UP,RETURN_CODE_ACTIVE,terminalPtr,count);
	}

	return 0;
}

//************************************
// Method:    ParseReadDO
// FullName:  Protocol::CModbus::ParseReadDO
// Access:    private 
// Returns:   int
// Qualifier: ����DO���ݱ���
// Parameter: unsigned char * buf�����Ļ�����ָ��
// Parameter: share_terminal_ptr terminalPtr����Ӧ�ն�ָ��
//************************************
int CModbus::ParseReadDO(unsigned char * buf,share_terminal_ptr terminalPtr)
{
	if (curCmd_.PointDataType_ != YxDataType)
	{
		return -1;
	}

	int info_addr = curCmd_.PointIndex_;
	int byteCount = BufToVal(&buf[DataLocation_],ByteCountLength_,bLowDataFirst_);

	int count = 0;
	for (int i=0;i<byteCount;i++)
	{
		unsigned char YxByteVal = buf[DataLocation_ + 1 + i];
		for (int j=0;j<8;j++)
		{
			if ((YxByteVal & BYTE_CHECK_TRUE[j]) > 0)
			{
				terminalPtr->SaveYxType(info_addr + i*16 + j,DataBase::single_yx_point);
				int ret = terminalPtr->SaveOriYxVal(info_addr + i*16 + j,1,true);
				if (ret == DataBase::CauseActiveData)
				{
					count++;
				}
			}
			else
			{
				terminalPtr->SaveYxType(info_addr + i*16 + j,DataBase::single_yx_point);
				int ret = terminalPtr->SaveOriYxVal(info_addr + i*16 + j,0,true);
				if (ret == DataBase::CauseActiveData)
				{
					count++;
				}
			}
		}
	}

	if (count > 0)
	{
		std::ostringstream ostr;
		ostr<<"DI���ݱ��Ĳ�����COS����Ŀ��"<<count<<std::endl;
		AddStatusLogWithSynT(ostr.str());
		CmdConSig_(COS_DATA_UP,RETURN_CODE_ACTIVE,terminalPtr,count);
	}

	return 0;
}

//************************************
// Method:    ParseReadAI
// FullName:  Protocol::CModbus::ParseReadAI
// Access:    private 
// Returns:   int
// Qualifier: ����DI���ݱ���
// Parameter: unsigned char * buf�����Ļ�����ָ��
// Parameter: share_terminal_ptr terminalPtr����Ӧ�ն�ָ��
//************************************
int CModbus::ParseReadAI(unsigned char * buf,share_terminal_ptr terminalPtr)
{
	int info_addr = curCmd_.PointIndex_;
	int byteCount = BufToVal(&buf[DataLocation_],ByteCountLength_,bLowDataFirst_);
	int count=0;

	if (curCmd_.PointDataType_ == YcDataType)
	{
		for (int i=0;i<(byteCount / 2);i++)
		{
			unsigned short YcVal = BufToVal(&buf[DataLocation_ + 1 + i*2],2,bLowDataFirst_);
			terminalPtr->SaveYcQuality(info_addr + i,true);
			int ret=terminalPtr->SaveOriYcVal(info_addr + i,YcVal,true);
			if (ret == DataBase::CauseActiveData)//�ж��Ƿ�����仯ң��
			{
				count++;
			}
		}
		if (count > 0)
		{
			std::ostringstream ostr;
			//std::cout<<"�����˱仯ң�⣬����countΪ��"<<count<<std::endl;
			ostr<<"ȫYC���Ĳ�����ycvar����Ŀ��"<<count<<std::endl;
			CmdConSig_(YCVAR_DATA_UP,RETURN_CODE_ACTIVE,terminalPtr,count);
		}
	}
	else if (curCmd_.PointDataType_ == YmDataType)
	{
		for (int i=0;i<(byteCount / 2);i++)
		{
			unsigned short YmVal = BufToVal(&buf[DataLocation_ + 1 + i*2],2,bLowDataFirst_);
			terminalPtr->SaveOriYmVal(info_addr + i,YmVal);
		}
	}
	else if (curCmd_.PointDataType_ == SynTimeType)
	{
		//do nothing temporarily
	}

	return 0;
}

//************************************
// Method:    ParseReadAO
// FullName:  Protocol::CModbus::ParseReadAO
// Access:    private 
// Returns:   int
// Qualifier: ����AO���ݱ���
// Parameter: unsigned char * buf�����Ļ�����ָ��
// Parameter: share_terminal_ptr terminalPtr����Ӧ�ն�ָ��
//************************************
int CModbus::ParseReadAO(unsigned char * buf,share_terminal_ptr terminalPtr)
{
	int info_addr = curCmd_.PointIndex_;
	int byteCount = BufToVal(&buf[DataLocation_],ByteCountLength_,bLowDataFirst_);
	int count=0;

	if (curCmd_.PointDataType_ == YcDataType)
	{
		for (int i=0;i<(byteCount / 2);i++)
		{
			unsigned short YcVal = BufToVal(&buf[DataLocation_ + 1 + i*2],2,bLowDataFirst_);
			terminalPtr->SaveYcQuality(info_addr + i,true);
			int ret=terminalPtr->SaveOriYcVal(info_addr + i,YcVal,true);
			if (ret == DataBase::CauseActiveData)//�ж��Ƿ�����仯ң��
			{
				count++;
			}
		}
		if (count > 0)
		{
			std::ostringstream ostr;
			//std::cout<<"�����˱仯ң�⣬����countΪ��"<<count<<std::endl;
			ostr<<"ȫYC���Ĳ�����ycvar����Ŀ��"<<count<<std::endl;
			CmdConSig_(YCVAR_DATA_UP,RETURN_CODE_ACTIVE,terminalPtr,count);
		}
	}
	else if (curCmd_.PointDataType_ == YmDataType)
	{
		for (int i=0;i<(byteCount / 2);i++)
		{
			unsigned short YmVal = BufToVal(&buf[DataLocation_ + 1 + i*2],2,bLowDataFirst_);
			terminalPtr->SaveOriYmVal(info_addr + i,YmVal);
		}
	}
	else if (curCmd_.PointDataType_ == SynTimeType)
	{
		//do nothing temporarily
	}
	
	return 0;
}

//************************************
// Method:    ParseWriteDO
// FullName:  Protocol::CModbus::ParseWriteDO
// Access:    private 
// Returns:   int
// Qualifier: ����дDO��Ӧ����
// Parameter: unsigned char * buf�����Ļ�����ָ��
// Parameter: share_terminal_ptr terminalPtr����Ӧ�ն�ָ��
//************************************
int CModbus::ParseWriteDO(unsigned char * buf,share_terminal_ptr terminalPtr)
{
	if (curCmd_.PointDataType_ == YkDataType)
	{
		ResetTimerYkExe(terminalPtr,curCmd_.PointIndex_,false);
		//terminalPtr->setYkStatus(curCmd_.PointIndex_,DataBase::YkExeCon);
		(terminalPtr->loadYkPointPtr(curCmd_.PointIndex_))->ExeResponEvent();
		CmdConSig_(YK_EXE_CON,RETURN_CODE_ACTIVE,terminalPtr,curCmd_.PointIndex_);
	}

	return 0;
}

//************************************
// Method:    ParseWriteAO
// FullName:  Protocol::CModbus::ParseWriteAO
// Access:    private 
// Returns:   int
// Qualifier: ����дAO��Ӧ����
// Parameter: unsigned char * buf�����Ļ�����ָ��
// Parameter: share_terminal_ptr terminalPtr����Ӧ�ն�ָ��
//************************************
int CModbus::ParseWriteAO(unsigned char * buf,share_terminal_ptr terminalPtr)
{
	return 0;
}

//************************************
// Method:    ParseWriteDOs
// FullName:  Protocol::CModbus::ParseWriteDOs
// Access:    private 
// Returns:   int
// Qualifier: ����д���DO��Ӧ����
// Parameter: unsigned char * buf�����Ļ�����ָ��
// Parameter: share_terminal_ptr terminalPtr����Ӧ�ն�ָ��
//************************************
int CModbus::ParseWriteDOs(unsigned char * buf,share_terminal_ptr terminalPtr)
{
	return 0;
}

//************************************
// Method:    ParseWriteAOs
// FullName:  Protocol::CModbus::ParseWriteAOs
// Access:    private 
// Returns:   int
// Qualifier: ����д���AO��Ӧ����
// Parameter: unsigned char * buf�����Ļ�����ָ��
// Parameter: share_terminal_ptr terminalPtr����Ӧ�ն�ָ��
//************************************
int CModbus::ParseWriteAOs(unsigned char * buf,share_terminal_ptr terminalPtr)
{
	return 0;
}

//************************************
// Method:    AssembleFrameHead
// FullName:  Protocol::CModbus::AssembleFrameHead
// Access:    virtual protected 
// Returns:   int
// Qualifier: ��װ����ͷ
// Parameter: size_t bufIndex����ǰҪ��װ�ı��Ļ�����λ��
// Parameter: unsigned char * buf��ָ���Ļ�������ָ��
// Parameter: CCmd cmd����װ���ĵ�����
//************************************
int CModbus::AssembleFrameHead(size_t bufIndex, unsigned char * buf, CCmd & cmd)
{
	int ret = -1;

	switch (mode_)
	{
	case TCP_MODE:
		ret = AssembleFrameHead_TCPMode(bufIndex,buf,cmd);
		break;

	default:
		ret = AssembleFrameHead_RTUMode(bufIndex,buf,cmd);
		break;
	}

	return ret;
}

//************************************
// Method:    AssembleFrameHead_RTUMode
// FullName:  Protocol::CModbus::AssembleFrameHead_RTUMode
// Access:    virtual protected 
// Returns:   int
// Qualifier: ��װ����ͷ
// Parameter: size_t bufIndex����ǰҪ��װ�ı��Ļ�����λ��
// Parameter: unsigned char * buf��ָ���Ļ�������ָ��
// Parameter: CCmd cmd����װ���ĵ�����
//************************************
int CModbus::AssembleFrameHead_RTUMode(size_t bufIndex, unsigned char *buf, Protocol::CCmd & cmd)
{
	size_t count = bufIndex;

	switch (cmd.getCmdType())
	{
	case QUERY_DATA:
		{
			//int index = getCommPointIndexByPtr(cmd.getCommPoint());
			int index = getCmdParaIndexByAddr(cmd.getCommPoint()->getAddr());
			if (index >= 0)
			{
				size_t pointCount = 0;
				for (;pointCount<modbusCmdPara_.size();pointCount++)
				{
					if(!modbusCmdPara_[index].getNextReadCmd(curCmd_))
					{
						int point = getCommPointIndexByAddrCommType(TERMINAL_NODE,modbusCmdPara_[index].getAddr());
						if (point >= 0)
						{
							share_commpoint_ptr val = getCommPoint(point).lock();
							if (val)
							{
								cmd.setCommPoint(val);
								count += ValToBuf(&buf[count],val->getAddr(),DeviceAddrLength_,bLowDataFirst_);
								break;
							}
						}
					}

					index = (++index)%modbusCmdPara_.size();
				}

				if (pointCount >= modbusCmdPara_.size())
				{
					return -1;
				}
			}
			else
			{
				return -1;
			}
		}
		break;

	case SYN_TIME_ACT:
		{
			//int index = getCommPointIndexByPtr(cmd.getCommPoint());
			int index = getCmdParaIndexByAddr(cmd.getCommPoint()->getAddr());
			if (index >= 0)
			{
				size_t pointCount = 0;
				for (;pointCount<modbusCmdPara_.size();pointCount++)
				{
					if(!modbusCmdPara_[index].getWriteCmd(SynTimeType,0,curCmd_))
					{
						int point = getCommPointIndexByAddrCommType(TERMINAL_NODE,modbusCmdPara_[index].getAddr());
						if(point >= 0)
						{
							share_commpoint_ptr val = getCommPoint(point).lock();
							if (val)
							{
								cmd.setCommPoint(val);
								count += ValToBuf(&buf[count],val->getAddr(),DeviceAddrLength_,bLowDataFirst_);
								break;
							}
						}
					}

					index = (++index)%modbusCmdPara_.size();
				}

				if (pointCount >= modbusCmdPara_.size())
				{
					return -1;
				}
			}
			else
			{
				return -1;
			}
		}
		break;

	default:
		count += ValToBuf(&buf[count],(cmd.getCommPoint())->getAddr(),DeviceAddrLength_,bLowDataFirst_);
		break;
	}
	
	return count - bufIndex;
}

//************************************
// Method:    AssembleFrameHead_TCPMode
// FullName:  Protocol::CModbus::AssembleFrameHead_TCPMode
// Access:    virtual protected 
// Returns:   int
// Qualifier: ��װ����ͷ
// Parameter: size_t bufIndex����ǰҪ��װ�ı��Ļ�����λ��
// Parameter: unsigned char * buf��ָ���Ļ�������ָ��
// Parameter: CCmd cmd����װ���ĵ�����
//************************************
int CModbus::AssembleFrameHead_TCPMode(size_t bufIndex, unsigned char * buf, CCmd & cmd)
{
	size_t count = bufIndex;

	count += ValToBuf(&buf[count],getCurMessageNO(),2,bLowDataFirst_);
	count += ValToBuf(&buf[count],0,2,bLowDataFirst_);
	count += ValToBuf(&buf[count],0,2,bLowDataFirst_);

	count += AssembleFrameHead_RTUMode(count,buf,cmd);

	return count - bufIndex;
}

//************************************
// Method:    AssembleFrameBody
// FullName:  Protocol::CModbus::AssembleFrameBody
// Access:    virtual protected 
// Returns:   int
// Qualifier: ��װ��������
// Parameter: size_t bufIndex����ǰҪ��װ�ı��Ļ�����λ��
// Parameter: unsigned char * buf��ָ���Ļ�������ָ��
// Parameter: CCmd cmd����װ���ĵ�����
//************************************
int CModbus::AssembleFrameBody(size_t bufIndex, unsigned char * buf, CCmd & cmd)
{
	int bytesAssemble = 0;

	share_terminal_ptr terminalPtr;

	if (cmd.getCommPoint())
	{
		if (cmd.getCommPoint()->getCommPointType() == TERMINAL_NODE)
		{
			terminalPtr = boost::dynamic_pointer_cast<DataBase::CTerminal>(cmd.getCommPoint());
		}
	}

	if (!terminalPtr)
	{
		std::ostringstream ostr;
		ostr<<"Modbus��Լ���ܴӷ��������л��terminal ptr��cmdtype = "<<cmd.getCmdType()<<"�����������ᱻ���͡�"<<std::endl;
		AddStatusLogWithSynT(ostr.str());

		return -1;
	}

	switch (cmd.getCmdType())
	{
	case SYN_TIME_ACT:
		bytesAssemble = AssembleSynTime(bufIndex,buf,curCmd_,boost::posix_time::microsec_clock::local_time());
		if (bytesAssemble > 0)
		{
			setWaitingForAnswer(cmd.getCommPoint());
			ResetTimerSynTime(terminalPtr,true);
		}
		break;

	case QUERY_DATA:
		bytesAssemble = AssembleQueryData(bufIndex,buf,curCmd_);
		if (bytesAssemble > 0)
		{
			setWaitingForAnswer(cmd.getCommPoint());
			ResetTimerHeartFrame(terminalPtr,true);
		}
		break;

	case YK_SEL_ACT:
		{
			int yk_no;
			try
			{
				yk_no = boost::any_cast<int>(cmd.getVal());
			}
			catch(const boost::bad_any_cast & e)
			{
				std::ostringstream ostr;
				ostr<<"ң��ѡ�������ң�ص�Ų����Ƿ���"<<e.what()<<std::endl;
				AddStatusLogWithSynT(ostr.str());
				return -1;
			}

			//int index = getCommPointIndexByPtr(cmd.getCommPoint());
			int index = getCmdParaIndexByAddr(cmd.getCommPoint()->getAddr());
			if (index < 0 || index >= (int)modbusCmdPara_.size())
			{
				std::ostringstream ostr;
				ostr<<"ң��ѡ�������ӳ���ն˲����Ƿ���"<<std::endl;
				AddStatusLogWithSynT(ostr.str());
				return -1;
			}

			if(modbusCmdPara_[index].getWriteCmd(YkDataType,yk_no,curCmd_))
			{
				std::ostringstream ostr;
				ostr<<"ң��ѡ�������ӳ��Modbus�����Ƿ���"<<std::endl;
				AddStatusLogWithSynT(ostr.str());
				return -1;
			}

			typeYktype yk_type = DataBase::YkOtherType;
			try
			{
				yk_type = terminalPtr->getYkType(yk_no);
			}
			catch(PublicSupport::dat2def_exception & err)
			{
				std::ostringstream ostr;
				ostr<<"ң��ѡ�������ң�ص�Ų�������"<<err.what()<<std::endl;
				AddStatusLogWithSynT(ostr.str());
				return -1;
			}

			//if (!DataBase::CYkPoint::CheckYkStatusDevelopNoSel(terminalPtr->getYkStatus(yk_no),DataBase::YkSelSend))
			if((terminalPtr->loadYkPointPtr(yk_no))->SendSelEvent())
			{
				//std::ostringstream ostr;
				//ostr<<"curStatus:"<<(int)terminalPtr->getYkStatus(yk_no)<<"NextStatus:"<<(int)DataBase::YkSelSend<<std::endl;
				//AddStatusLogWithSynT(ostr.str());
				AddStatusLogWithSynT("����ң��ѡ���������ң�ص�ǰң��״̬�����ϣ��˳������͸����\n");
				return -1;
			}

			bytesAssemble = 0;
			//terminalPtr->setYkStatus(yk_no,DataBase::YkSelCon);
			CmdConSig_(YK_SEL_CON,RETURN_CODE_ACTIVE,terminalPtr,yk_no);
		}
		break;

	case YK_EXE_ACT:
		{
			int yk_no;
			try
			{
				yk_no = boost::any_cast<int>(cmd.getVal());
			}
			catch(const boost::bad_any_cast & e)
			{
				std::ostringstream ostr;
				ostr<<"ң��ִ�������ң�ص�Ų����Ƿ���"<<e.what()<<std::endl;
				AddStatusLogWithSynT(ostr.str());
				return -1;
			}

			//int index = getCommPointIndexByPtr(cmd.getCommPoint());
			int index = getCmdParaIndexByAddr(cmd.getCommPoint()->getAddr());
			if (index < 0 || index >= (int)modbusCmdPara_.size())
			{
				std::ostringstream ostr;
				ostr<<"ң��ִ�������ӳ���ն˲����Ƿ���"<<std::endl;
				AddStatusLogWithSynT(ostr.str());
				return -1;
			}

			if(modbusCmdPara_[index].getWriteCmd(YkDataType,yk_no,curCmd_))
			{
				std::ostringstream ostr;
				ostr<<"ң��ִ�������ӳ��Modbus�����Ƿ���"<<std::endl;
				AddStatusLogWithSynT(ostr.str());
				return -1;
			}

			typeYktype yk_type = DataBase::YkOtherType;
			try
			{
				yk_type = terminalPtr->getYkType(yk_no);
			}
			catch(PublicSupport::dat2def_exception & err)
			{
				std::ostringstream ostr;
				ostr<<"ң��ִ�������ң�ص�Ų�������"<<err.what()<<std::endl;
				AddStatusLogWithSynT(ostr.str());
				return -1;
			}

			if (yk_type == DataBase::YkClose)
			{
				bytesAssemble = AssembleYKExe(bufIndex,buf,curCmd_,YK_TYPE_CLOSE);
			}
			else if (yk_type == DataBase::YkOpen)
			{
				bytesAssemble = AssembleYKExe(bufIndex,buf,curCmd_,YK_TYPE_OPEN);
			}
			else
			{
				AddStatusLogWithSynT("ң��ִ�������ң�����Ͳ����Ƿ���\n");
				return -1;
			}

			if (bytesAssemble > 0)
			{
				//if (!DataBase::CYkPoint::CheckYkStatusDevelopNoSel(terminalPtr->getYkStatus(yk_no),DataBase::YkExeSend))
				if((terminalPtr->loadYkPointPtr(yk_no))->SendExeEvent())
				{
					//std::ostringstream ostr;
					//ostr<<"curStatus:"<<(int)terminalPtr->getYkStatus(yk_no)<<"NextStatus:"<<(int)DataBase::YkExeSend<<std::endl;
					//AddStatusLogWithSynT(ostr.str());
					AddStatusLogWithSynT("����ң��ִ���������ң�ص�ǰң��״̬�����ϣ��˳������͸����\n");
					return -1;
				}
				//terminalPtr->setYkStatus(yk_no,DataBase::YkExeSend);

				setWaitingForAnswer(cmd.getCommPoint());
				ResetTimerYkExe(terminalPtr,yk_no,true);
			}
		}
		break;

	case YK_CANCEL_ACT:
		{
			int yk_no;
			try
			{
				yk_no = boost::any_cast<int>(cmd.getVal());
			}
			catch(const boost::bad_any_cast & e)
			{
				std::ostringstream ostr;
				ostr<<"ң��ȡ�������ң�ص�Ų����Ƿ���"<<e.what()<<std::endl;
				AddStatusLogWithSynT(ostr.str());
				return -1;
			}

			//int index = getCommPointIndexByPtr(cmd.getCommPoint());
			int index = getCmdParaIndexByAddr(cmd.getCommPoint()->getAddr());
			if (index < 0 || index >= (int)modbusCmdPara_.size())
			{
				std::ostringstream ostr;
				ostr<<"ң��ȡ�������ӳ���ն˲����Ƿ���"<<std::endl;
				AddStatusLogWithSynT(ostr.str());
				return -1;
			}

			if(modbusCmdPara_[index].getWriteCmd(YkDataType,yk_no,curCmd_))
			{
				std::ostringstream ostr;
				ostr<<"ң��ȡ�������ӳ��Modbus�����Ƿ���"<<std::endl;
				AddStatusLogWithSynT(ostr.str());
				return -1;
			}

			typeYktype yk_type = DataBase::YkOtherType;
			try
			{
				yk_type = terminalPtr->getYkType(yk_no);
			}
			catch(PublicSupport::dat2def_exception & err)
			{
				std::ostringstream ostr;
				ostr<<"ң��ȡ�������ң�ص�Ų�������"<<err.what()<<std::endl;
				AddStatusLogWithSynT(ostr.str());
				return -1;
			}

			//if (!DataBase::CYkPoint::CheckYkStatusDevelopNoSel(terminalPtr->getYkStatus(yk_no),DataBase::YkCancelSend))
			if((terminalPtr->loadYkPointPtr(yk_no))->SendCancelEvent())
			{
				//std::ostringstream ostr;
				//ostr<<"curStatus:"<<(int)terminalPtr->getYkStatus(yk_no)<<"NextStatus:"<<(int)DataBase::YkCancelSend<<std::endl;
				//AddStatusLogWithSynT(ostr.str());
				AddStatusLogWithSynT("����ң��ȡ���������ң�ص�ǰң��״̬�����ϣ��˳������͸����\n");
				return -1;
			}

			bytesAssemble = 0;
			//terminalPtr->setYkStatus(yk_no,DataBase::YkCancelCon);
			CmdConSig_(YK_CANCEL_CON,RETURN_CODE_ACTIVE,terminalPtr,yk_no);
		}
		break;

	default:
		break;
	}

	return bytesAssemble;
}

//************************************
// Method:    AssembleYKExe
// FullName:  Protocol::CModbus::AssembleYKExe
// Access:    private 
// Returns:   int
// Qualifier: ��װң�ر���
// Parameter: size_t bufIndex����ǰҪ��װ�ı��Ļ�����λ��
// Parameter: unsigned char * buf��ָ���Ļ�������ָ��
// Parameter: stModbusCmd modbusCmd�� Modbus��Լ�������
// Parameter: unsigned short yk_code��ң�طֺ�բ������
//************************************
int CModbus::AssembleYKExe(size_t bufIndex, unsigned char * buf, stModbusCmd modbusCmd, unsigned short yk_code)
{
	size_t count = bufIndex;

	count += ValToBuf(&buf[count],modbusCmd.FunCode_,FunCodeLength_,bLowDataFirst_);
	count += ValToBuf(&buf[count],modbusCmd.RegisterAddr_,RegisterAddrLength_,bLowDataFirst_);
	count += ValToBuf(&buf[count],yk_code,SingleDataLength_,bLowDataFirst_);

	return count - bufIndex;
}

//************************************
// Method:    AssembleSynTime
// FullName:  Protocol::CModbus::AssembleSynTime
// Access:    private 
// Returns:   int
// Qualifier: ��װ��ʱ����
// Parameter: size_t bufIndex����ǰҪ��װ�ı��Ļ�����λ��
// Parameter: unsigned char * buf��ָ���Ļ�������ָ��
// Parameter: stModbusCmd modbusCmd�� Modbus��Լ�������
// Parameter: boost::posix_time::ptime time�� ϵͳʱ��
//************************************
int CModbus::AssembleSynTime(size_t bufIndex, unsigned char * buf, stModbusCmd modbusCmd, boost::posix_time::ptime time)
{
	size_t count = bufIndex;

	count += ValToBuf(&buf[count],modbusCmd.FunCode_,FunCodeLength_,bLowDataFirst_);
	count += ValToBuf(&buf[count],modbusCmd.RegisterAddr_,RegisterAddrLength_,bLowDataFirst_);
	count += ValToBuf(&buf[count],modbusCmd.DataNum_,SingleDataLength_,bLowDataFirst_);
	count += ValToBuf(&buf[count],modbusCmd.DataNum_ * SingleDataLength_,ByteCountLength_,bLowDataFirst_);
	boost::gregorian::date::ymd_type ymd = time.date().year_month_day();
	count += ValToBuf(&buf[count],ymd.year % 100,2,bLowDataFirst_);
	buf[count++] = ymd.month & 0x0f;
	buf[count++] = ymd.day & 0x1f;
	boost::posix_time::time_duration td = time.time_of_day();
	buf[count++] = td.hours() & 0x1f;
	buf[count++] = td.minutes() & 0x3f;
	count += ValToBuf(&buf[count],td.total_milliseconds() % MinutesRemainderMillisecs,2,bLowDataFirst_);

	return count - bufIndex;
}

//************************************
// Method:    AssembleQueryData
// FullName:  Protocol::CModbus::AssembleQueryData
// Access:    private 
// Returns:   int
// Qualifier:
// Parameter: size_t bufIndex����ǰҪ��װ�ı��Ļ�����λ��
// Parameter: unsigned char * buf��ָ���Ļ�������ָ��
// Parameter: stModbusCmd modbusCmd��Modbus��Լ�������
//************************************
int CModbus::AssembleQueryData(size_t bufIndex, unsigned char * buf, stModbusCmd modbusCmd)
{
	size_t count = bufIndex;

	count += ValToBuf(&buf[count],modbusCmd.FunCode_,FunCodeLength_,bLowDataFirst_);
	count += ValToBuf(&buf[count],modbusCmd.RegisterAddr_,RegisterAddrLength_,bLowDataFirst_);
	count += ValToBuf(&buf[count],modbusCmd.DataNum_,SingleDataLength_,bLowDataFirst_);
	
	return count - bufIndex;
}

//************************************
// Method:    AssembleFrameTail
// FullName:  Protocol::CModbus::AssembleFrameTail
// Access:    virtual protected 
// Returns:   int
// Qualifier: ��װ����β
// Parameter: size_t bufBegin����֡��װ���ĵĿ�ʼλ��
// Parameter: size_t bufIndex����ǰҪ��װ�ı��Ļ�����λ��
// Parameter: unsigned char * buf��ָ���Ļ�������ָ��
// Parameter: typeCmd cmdType�����������
//************************************
int CModbus::AssembleFrameTail(size_t bufBegin, size_t bufIndex, unsigned char * buf, CCmd & cmd)
{
	int ret = -1;

	switch (mode_)
	{
	case TCP_MODE:
		ret = AssembleFrameTail_TCPMode(bufBegin,bufIndex,buf,cmd);
		break;

	default:
		ret = AssembleFrameTail_RTUMode(bufBegin,bufIndex,buf,cmd);
		break;
	}

	return ret;
}

//************************************
// Method:    AssembleFrameTail_RTUMode
// FullName:  Protocol::CModbus::AssembleFrameTail_RTUMode
// Access:    virtual protected 
// Returns:   int
// Qualifier: ��װ����β
// Parameter: size_t bufBegin����֡��װ���ĵĿ�ʼλ��
// Parameter: size_t bufIndex����ǰҪ��װ�ı��Ļ�����λ��
// Parameter: unsigned char * buf��ָ���Ļ�������ָ��
// Parameter: typeCmd cmdType�����������
//************************************
int CModbus::AssembleFrameTail_RTUMode(size_t bufBegin, size_t bufIndex, unsigned char *buf, CCmd & cmd)
{
	int length = bufIndex - bufBegin;
	if (length <= 0)
	{
		return -1;
	}

	size_t count = bufIndex;

	count += ValToBuf(&buf[count],CRC16(&buf[bufBegin],length),CRCLength_,bLowCRCFirst_);
	
	return count - bufIndex;
}

//************************************
// Method:    AssembleFrameTail_TCPMode
// FullName:  Protocol::CModbus::AssembleFrameTail_TCPMode
// Access:    virtual protected 
// Returns:   int
// Qualifier: ��װ����β
// Parameter: size_t bufBegin����֡��װ���ĵĿ�ʼλ��
// Parameter: size_t bufIndex����ǰҪ��װ�ı��Ļ�����λ��
// Parameter: unsigned char * buf��ָ���Ļ�������ָ��
// Parameter: typeCmd cmdType�����������
//************************************
int CModbus::AssembleFrameTail_TCPMode(size_t bufBegin, size_t bufIndex, unsigned char * buf, CCmd & cmd)
{
	int length = bufIndex - bufBegin;
	if (length < TCP_MODE_HEAD_LENGTH)
	{
		return -1;
	}

	ValToBuf(&buf[bufBegin + TCP_MODE_HEAD_LENGTH - 2],length - TCP_MODE_HEAD_LENGTH,2,bLowDataFirst_);

	return length;
}

//************************************
// Method:    ResetTimerSynTime
// FullName:  Protocol::CModbus::ResetTimerSynTime
// Access:    private 
// Returns:   void
// Qualifier: ��������ʱ��У�鶨ʱ��
// Parameter: share_commpoint_ptr point�� ָ���ϴ�У��ʱ�ӵ�ͨѶ����ָ��
// Parameter: bool bContinue�� �Ƿ�������ö�ʱ��
// Parameter: unsigned short val�� ����ʱ��������ֵ
//************************************
void CModbus::ResetTimerSynTime(share_commpoint_ptr point,bool bContinue /*= true*/,unsigned short val/* = 0*/)
{
	if (bContinue)
	{
		share_commpoint_ptr nextPoint = getNextCommPoint(TERMINAL_NODE,true,point);
		if(nextPoint)
		{
			if(nextPoint->getSynTCommPointFlag())
			{
				if (val == 0)
				{
					timerSynTime_->expires_from_now(boost::posix_time::seconds(getMeanvalueOfPointsSum(MIN_timeOutSynTime,timeOutSynTime_)));
				}
				else
				{
					timerSynTime_->expires_from_now(boost::posix_time::seconds(val));
				}
			}
			else
			{
				timerSynTime_->expires_from_now(boost::posix_time::seconds(MIN_timeOutSynTime));
			}

			timerSynTime_->async_wait(boost::bind(&CModbus::handle_timerSynTime,this,boost::asio::placeholders::error,point));
		}
	}
	else
	{
		timerSynTime_->cancel();
	}
}

//************************************
// Method:    ResetTimerHeartFrame
// FullName:  Protocol::CModbus::ResetTimerHeartFrame
// Access:    private 
// Returns:   void
// Qualifier: ����������ѯ���ݶ�ʱ��
// Parameter: share_commpoint_ptr point�� ָ���ϴ���ѯ��ͨѶ����ָ��
// Parameter: bool bContinue�� �Ƿ�������ö�ʱ��
// Parameter: unsigned short val�� ����ʱ��������ֵ
//************************************
void CModbus::ResetTimerHeartFrame(share_commpoint_ptr point,bool bContinue /*= true*/,unsigned short val/* = 0*/)
{
	if (bContinue)
	{
		if (val == 0)
		{
			timerHeartFrame_->expires_from_now(boost::posix_time::seconds(timeOutHeartFrame_));
		}
		else
		{
			timerHeartFrame_->expires_from_now(boost::posix_time::seconds(val));
		}

		timerHeartFrame_->async_wait(boost::bind(&CModbus::handle_timerHeartFrame,this,boost::asio::placeholders::error,point));
	}
	else
	{
		timerHeartFrame_->cancel();
	}
}

//************************************
// Method:    ResetTimerYkExe
// FullName:  Protocol::CModbus::ResetTimerYkExe
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: share_commpoint_ptr point�� ָ���ϴ�У��ʱ�ӵ�ͨѶ����ָ��
// Parameter: size_t yk_no��ң�ص��
// Parameter: bool bContinue�� �Ƿ�������ö�ʱ��
// Parameter: unsigned short val�� ����ʱ��������ֵ
//************************************
void CModbus::ResetTimerYkExe(share_commpoint_ptr point,size_t yk_no,bool bContinue /*= false*/,unsigned short val/* = 0*/)
{
	if (bContinue)
	{
		if (val == 0)
		{
			timerYkExe_->expires_from_now(boost::posix_time::seconds(timeOutYkExe_));
		}
		else
		{
			timerYkExe_->expires_from_now(boost::posix_time::seconds(val));
		}

		timerYkExe_->async_wait(boost::bind(&CModbus::handle_timerYkExe,this,boost::asio::placeholders::error,point,yk_no));
	}
	else
	{
		timerYkExe_->cancel();
	}
}

//************************************
// Method:    handle_timerSynTime
// FullName:  Protocol::CModbus::handle_timerSynTime
// Access:    private 
// Returns:   void
// Qualifier: ����ʱ��У���¼�
// Parameter: const boost::system::error_code & error�� �쳣����
// Parameter: share_commpoint_ptr point�� ָ���ϴδ����ͨѶ����ָ��
//************************************
void CModbus::handle_timerSynTime(const boost::system::error_code& error,share_commpoint_ptr point)
{
	if (!error)
	{
		share_commpoint_ptr val = getNextCommPoint(TERMINAL_NODE,true,point);
		if (val)
		{
			AddSendCmdVal(SYN_TIME_ACT,SYN_TIME_ACT_PRIORITY,val);
		}
	}
}

//************************************
// Method:    handle_timerHeartFrame
// FullName:  Protocol::CModbus::handle_timerHeartFrame
// Access:    private 
// Returns:   void
// Qualifier: ������ѯ�����¼�
// Parameter: const boost::system::error_code & error�� �쳣����
// Parameter: share_commpoint_ptr point�� ָ���ϴδ����ͨѶ����ָ��
//************************************
void CModbus::handle_timerHeartFrame(const boost::system::error_code& error,share_commpoint_ptr point)
{
	if (!error)
	{
		share_commpoint_ptr val = getNextCommPoint(TERMINAL_NODE,true,point);
		if (val)
		{
			AddSendCmdVal(QUERY_DATA,QUERY_DATA_PRIORITY,val);
		}
	}
}

//************************************
// Method:    handle_timerYkExe
// FullName:  Protocol::CModbus::handle_timerYkExe
// Access:    private 
// Returns:   void
// Qualifier: ����ң��ִ�г�ʱ�¼�
// Parameter: const boost::system::error_code & error�� �쳣����
// Parameter: share_commpoint_ptr point�� ָ�����ͨѶ����ָ��
// Parameter: size_t yk_no��ң�ص��
//************************************
void CModbus::handle_timerYkExe(const boost::system::error_code& error,share_commpoint_ptr point,size_t yk_no)
{
	if (!error)
	{
		if (point)
		{
			if (point->getCommPointType() == TERMINAL_NODE)
			{
				share_terminal_ptr terminalPtr = boost::dynamic_pointer_cast<DataBase::CTerminal>(point);
				if (terminalPtr)
				{
					//terminalPtr->setYkStatus(yk_no,DataBase::YkExeTimeOut);
					(terminalPtr->loadYkPointPtr(yk_no))->TimeOutEvent();
				}
			}

			CmdConSig_(YK_EXE_CON,RETURN_CODE_TIMEOUT,point,yk_no);
			AddStatusLogWithSynT("Modbus��Լң��ִ�����ʱ��\n");
		}
	}
}

//************************************
// Method:    setTimeOutSynTime
// FullName:  Protocol::CModbus::setTimeOutSynTime
// Access:    private 
// Returns:   int
// Qualifier: ����ʱ��У�鶨ʱ����ʱֵ
// Parameter: unsigned short val ��ʱֵ
//************************************
int CModbus::setTimeOutSynTime(unsigned short val)
{
	if (val < 60 || val > 12000)
	{
		return -1;
	}

	timeOutSynTime_ = val;

	return 0;
}

//************************************
// Method:    setTimeOutHeartFrame
// FullName:  Protocol::CModbus::setTimeOutHeartFrame
// Access:    private 
// Returns:   int
// Qualifier: ������ѯ���ݶ�ʱ����ʱֵ
// Parameter: unsigned short val ��ʱֵ
//************************************
int CModbus::setTimeOutHeartFrame(unsigned short val)
{
	if (val <= 0 || val > 60) 
	{
		return -1;
	}

	timeOutHeartFrame_ = val;

	return 0;
}

//************************************
// Method:    setTimeOutYkExe
// FullName:  Protocol::CModbus::setTimeOutYkExe
// Access:    private 
// Returns:   int
// Qualifier: ����ң�س�ʱ��ʱ����ʱֵ
// Parameter: unsigned short val ��ʱֵ
//************************************
int CModbus::setTimeOutYkExe(unsigned short val)
{
	if (val <= 0 || val > 300)
	{
		return -1;
	}

	timeOutYkExe_ = val;

	return 0;
}

//************************************
// Method:    setFunCodeLength
// FullName:  Protocol::CModbus::setFunCodeLength
// Access:    private 
// Returns:   int
// Qualifier: ���ù�������ϢԪ�ص��ֽڳ���
// Parameter: unsigned short val
//************************************
int CModbus::setFunCodeLength(unsigned short val)
{
	if (val <= 0 || val > 4)
	{
		return -1;
	}

	FunCodeLength_ = val;

	return 0;
}

//************************************
// Method:    setDeviceAddrLength
// FullName:  Protocol::CModbus::setDeviceAddrLength
// Access:    private 
// Returns:   int
// Qualifier: ���õ�ַԪ���ֽڳ���
// Parameter: unsigned short val
//************************************
int CModbus::setDeviceAddrLength(unsigned short val)
{
	if (val < 1 || val > 4)
	{
		return -1;
	}

	DeviceAddrLength_ = val;

	return 0;
}

//************************************
// Method:    setErrCodeLength
// FullName:  Protocol::CModbus::setErrCodeLength
// Access:    private 
// Returns:   int
// Qualifier: �����쳣��Ԫ���ֽڳ���
// Parameter: unsigned short val
//************************************
int CModbus::setErrCodeLength(unsigned short val)
{
	if (val < 1 || val > 4)
	{
		return -1;
	}

	ErrCodeLength_ = val;

	return 0;
}
//************************************
// Method:    setByteCountLength
// FullName:  Protocol::CModbus::setByteCountLength
// Access:    private 
// Returns:   int
// Qualifier: �����ֽڼ���Ԫ���ֽڳ���
// Parameter: unsigned short val
//************************************
int CModbus::setByteCountLength(unsigned short val)
{
	if (val < 1 || val > 4)
	{
		return -1;
	}

	ErrCodeLength_ = val;

	return 0;
}

//************************************
// Method:    setRegisterAddrLength
// FullName:  Protocol::CModbus::setRegisterAddrLength
// Access:    private 
// Returns:   int
// Qualifier: ���üĴ�����ַԪ���ֽڳ���
// Parameter: unsigned short val
//************************************
int CModbus::setRegisterAddrLength(unsigned short val)
{
	if (val < 1 || val > 8)
	{
		return -1;
	}

	ErrCodeLength_ = val;

	return 0;
}

//************************************
// Method:    setSingleDataLength
// FullName:  Protocol::CModbus::setSingleDataLength
// Access:    private 
// Returns:   int
// Qualifier: ��������Ԫ���ֽڳ���
// Parameter: unsigned short val
//************************************
int CModbus::setSingleDataLength(unsigned short val)
{
	if (val < 1 || val > 8)
	{
		return -1;
	}

	ErrCodeLength_ = val;

	return 0;
}

//************************************
// Method:    setCrcLength
// FullName:  Protocol::CModbus::setCrcLength
// Access:    private 
// Returns:   int
// Qualifier: ����CRCУ��Ԫ���ֽڳ���
// Parameter: unsigned val
//************************************
int CModbus::setCrcLength(unsigned val)
{
	if (val < 1 || val > 8)
	{
		return -1;
	}

	ErrCodeLength_ = val;

	return 0;
}

//************************************
// Method:    setModbusMode
// FullName:  Protocol::CModbus::setModbusMode
// Access:    private 
// Returns:   int
// Qualifier: ����mode����
// Parameter: unsigned char val
//************************************
int CModbus::setModbusMode(unsigned char val)
{
	mode_ = val;

	return 0;
}

//************************************
// Method:    TransModeToString
// FullName:  Protocol::CModbus::TransModeToString
// Access:    private 
// Returns:   std::string
// Qualifier: Modbusģʽ������xml�ļ��ַ�ֵ��������ת��
// Parameter: unsigned char val
//************************************
std::string CModbus::TransModeToString(unsigned char val)
{
	std::string ret = "";

	switch (val)
	{
	case RTU_MODE:
		ret = strRtuMode;
		break;

	case TCP_MODE:
		ret = strTcpMode;
		break;

	case ASCII_MODE:
		ret = strASCIIMode;
		break;

	default:
		ret = strRtuMode;
		break;
	}

	return ret;
}

//************************************
// Method:    TransModeFromString
// FullName:  Protocol::CModbus::TransModeFromString
// Access:    private 
// Returns:   int
// Qualifier: Modbusģʽ�����ӳ���굽xml�ļ��ַ�ֵ��ת��
// Parameter: std::string val
//************************************
int CModbus::TransModeFromString(std::string val)
{
	int ret = -1;

	if (boost::iequals(strRtuMode,val))
	{
		ret = RTU_MODE;
	}
	else if (boost::iequals(strTcpMode,val))
	{
		ret = TCP_MODE;
	}
	else if (boost::iequals(strASCIIMode,val))
	{
		ret = ASCII_MODE;
	}
	else
	{
		ret = RTU_MODE;
	}

	return ret;
}

}//namespace Protocol {
