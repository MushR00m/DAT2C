#include <boost/bind.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>
#include "SerialPort.h"
#include "../Protocol/Protocol.h"
#include "../FileSystem/Markup.h"
#include "CommPara.h"

namespace CommInterface {

const std::string strSerialCfgName = "SerialPort.xml";

//xml��㶨��
#define strSerialRoot "SerialPort"
#define strBaudRate "BaudRate"
#define strDataBits "DataBits"
#define strStopBits "StopBits"
#define strStopBitOne "1"
#define strStopBitOnePointFive "1.5"
#define strStopBitTwo "2"
#define strParity "Parity"
#define strParityNone "none"
#define strParityOdd "odd"
#define strParityEven "even"
#define strFlowControl "FlowControl"
#define strFlowControlNone "none"
#define strFlowControlSoftware "software"
#define strFlowControlHardware "hardware"

//seiral prot default para
#define Default_BaudRate 9600
#define Default_DataBits 8
#define Default_StopBits boost::asio::serial_port::stop_bits::one
#define Default_Parity boost::asio::serial_port::parity::none
#define Default_FlowControl boost::asio::serial_port::flow_control::none

CSerialPort::CSerialPort(CCommPara & para,boost::asio::io_service& io_service)
						:CCommInterface(para,io_service),
						serial_port_(io_service),
						timerRecv_(io_service,boost::posix_time::minutes(recv_time_out)),
						timerRetry_(io_service,boost::posix_time::seconds(retry_time_out))
{
	retryTimes_ = 0;
	port_no_ = para.getLocalPort();
}

CSerialPort::~CSerialPort(void)
{
	if (getConnectAlive())
	{
		CloseConnect();
	}

	UninitConnect();
}

int CSerialPort::InitSerialPort()
{
	if (getConnectAlive())
	{
		serial_port_.set_option(boost::asio::serial_port::baud_rate(Default_BaudRate)); 
		serial_port_.set_option(boost::asio::serial_port::flow_control(Default_FlowControl));
		serial_port_.set_option(boost::asio::serial_port::parity(Default_Parity)); 
		serial_port_.set_option(boost::asio::serial_port::stop_bits(Default_StopBits));
		serial_port_.set_option(boost::asio::serial_port::character_size(Default_DataBits)); 

		if (LoadSerialCfg(strSerialCfgName))
		{
			AddStatusLogWithSynT("δ�ܴ������ļ���ʼ�����ڣ�ʹ��Ĭ��ֵ��ʼ����\n");
		}

		return 0;
	}

	return -1;
}

void CSerialPort::SaveSerialAttrib(FileSystem::CMarkup & xml)
{
	boost::asio::serial_port::baud_rate baudrate;
	serial_port_.get_option(baudrate);
	xml.AddElem(strBaudRate,baudrate.value());

	boost::asio::serial_port::character_size databits;
	serial_port_.get_option(databits);
	xml.AddElem(strDataBits,databits.value());

	boost::asio::serial_port::stop_bits stopbits;
	serial_port_.get_option(stopbits);
	switch (stopbits.value())
	{
	case boost::asio::serial_port::stop_bits::one:
		xml.AddElem(strStopBits,strStopBitOne);
		break;

	case boost::asio::serial_port::stop_bits::onepointfive:
		xml.AddElem(strStopBits,strStopBitOnePointFive);
		break;

	case boost::asio::serial_port::stop_bits::two:
		xml.AddElem(strStopBits,strStopBitTwo);
		break;

	default:
		break;
	}

	boost::asio::serial_port::parity paritys;
	serial_port_.get_option(paritys);
	switch (paritys.value())
	{
	case boost::asio::serial_port::parity::none:
		xml.AddElem(strParity,strParityNone);
		break;

	case boost::asio::serial_port::parity::odd:
		xml.AddElem(strParity,strParityOdd);
		break;

	case boost::asio::serial_port::parity::even:
		xml.AddElem(strParity,strParityEven);
		break;

	default:
		break;
	}

	boost::asio::serial_port::flow_control flowcontrol;
	serial_port_.get_option(flowcontrol);
	switch (flowcontrol.value())
	{
	case boost::asio::serial_port::flow_control::none:
		xml.AddElem(strFlowControl,strFlowControlNone);
		break;

	case boost::asio::serial_port::flow_control::software:
		xml.AddElem(strFlowControl,strFlowControlSoftware);
		break;

	case boost::asio::serial_port::flow_control::hardware:
		xml.AddElem(strFlowControl,strFlowControlHardware);
		break;

	default:
		break;
	}
}
void CSerialPort::SaveSerialCfg(std::string fileName)
{
	std::vector<std::string> strVec;
	boost::algorithm::split(strVec,port_no_,boost::algorithm::is_any_of("/"));

	if (strVec.size() < 1)
	{
		return;
	}

	std::string port_node = *(strVec.end() - 1);

	FileSystem::CMarkup xml;

	if (xml.Load(fileName))
	{
		xml.ResetMainPos();
		xml.FindElem();  //root strSerialPort
		xml.IntoElem();  //enter strSerialPort

		xml.ResetMainPos();
		if (xml.FindElem(port_node))
		{
			xml.RemoveElem();

			xml.AddElem(port_node);
			xml.IntoElem();                  //enter port_no_

			SaveSerialAttrib(xml);

			xml.OutOfElem();                 //out port_no_
		}
		else
		{
			xml.AddElem(port_node);
			xml.IntoElem();                  //enter port_no_

			SaveSerialAttrib(xml);

			xml.OutOfElem();                 //out port_no_
		}


		xml.OutOfElem();  //out strSerialPort
	}
	else
	{
		xml.SetDoc(strXmlHead);
		xml.SetDoc(strSerialXsl);

		xml.AddElem(strSerialRoot);
		xml.IntoElem();                  //enter strSerialRoot

		xml.AddElem(port_node);
		xml.IntoElem();                  //enter port_no_

		SaveSerialAttrib(xml);

		xml.OutOfElem();                 //out port_no_

		xml.OutOfElem();                 //out strSerialRoot
	}
}

int CSerialPort::LoadSerialCfg(std::string fileName)
{
	int ret = -1;

	FileSystem::CMarkup xml;
	
	if (!xml.Load(fileName))
	{
		return ret;
	}

	xml.ResetMainPos();
	xml.FindElem();  //root strSerialPort
	xml.IntoElem();  //enter strSerialPort

	std::vector<std::string> strVec;
	boost::algorithm::split(strVec,port_no_,boost::algorithm::is_any_of("/"));

	if (strVec.size() < 1)
	{
		return ret;
	}

	std::string port_node = *(strVec.end() - 1);

	xml.ResetMainPos();
	if (xml.FindElem(port_node))
	{
		xml.IntoElem();

		xml.ResetMainPos();
		if (xml.FindElem(strBaudRate))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned int baudrate = boost::lexical_cast<unsigned int>(strTmp);

				if (baudrate >= 300 /*&& baudrate <= 115200*/)
				{
					serial_port_.set_option(boost::asio::serial_port::baud_rate(baudrate));
					ret = 0;
				}
				else
				{
					std::ostringstream ostr;
					ostr<<"�Ƿ��Ĵ��ڲ����ʲ�����"<<baudrate<<"����ʹ��Ĭ��ֵ\n";
					AddStatusLogWithSynT(ostr.str());
				}
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<"�Ƿ��Ĵ��ڲ����ʲ�����"<<e.what()<<"����ʹ��Ĭ��ֵ\n";
				AddStatusLogWithSynT(ostr.str());
			}
		}

		xml.ResetMainPos();
		if (xml.FindElem(strDataBits))
		{
			//unsigned short databits = Default_DataBits;

			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short databits = boost::lexical_cast<unsigned short>(strTmp);

				if (databits >= 4 && databits <= 8)
				{
					serial_port_.set_option(boost::asio::serial_port::character_size(databits));
					ret = 0;
				}
				else
				{
					std::ostringstream ostr;
					ostr<<"�Ƿ��Ĵ�������λ������"<<databits<<"����ʹ��Ĭ��ֵ\n";
					AddStatusLogWithSynT(ostr.str());
				}
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<"�Ƿ��Ĵ�������λ������"<<e.what()<<"����ʹ��Ĭ��ֵ\n";
				AddStatusLogWithSynT(ostr.str());
			}
		}

		xml.ResetMainPos();
		if (xml.FindElem(strStopBits))
		{
			//float stopbits = 1;
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				float stopbits = boost::lexical_cast<float>(strTmp);

				if (stopbits == 1)
				{
					serial_port_.set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));
					ret = 0;
				}
				else if(stopbits == 1.5)
				{
					serial_port_.set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::onepointfive));
					ret = 0;
				}
				else if (stopbits == 2)
				{
					serial_port_.set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::two));
					ret = 0;
				}
				else
				{
					std::ostringstream ostr;
					ostr<<"�Ƿ��Ĵ���ֹͣλ������"<<stopbits<<"����ʹ��Ĭ��ֵ\n";
					AddStatusLogWithSynT(ostr.str());
				}
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<"�Ƿ��Ĵ���ֹͣλ������"<<e.what()<<"����ʹ��Ĭ��ֵ\n";
				AddStatusLogWithSynT(ostr.str());
			}
		}

		xml.ResetMainPos();
		if (xml.FindElem(strParity))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);

			if (boost::iequals(strTmp,strParityNone))
			{
				serial_port_.set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::none));
				ret = 0;
			}
			else if (boost::iequals(strTmp,strParityOdd))
			{
				serial_port_.set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::odd));
				ret = 0;
			}
			else if (boost::iequals(strTmp,strParityEven))
			{
				serial_port_.set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::even));
				ret = 0;
			}
			else
			{
				std::ostringstream ostr;
				ostr<<"�Ƿ��Ĵ���У��λ������"<<strTmp<<"����ʹ��Ĭ��ֵ\n";
				AddStatusLogWithSynT(ostr.str());
			}
		}

		xml.ResetMainPos();
		if (xml.FindElem(strFlowControl))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);

			if (boost::iequals(strTmp,strFlowControlNone))
			{
				serial_port_.set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::none));
				ret = 0;
			}
			else if (boost::iequals(strTmp,strFlowControlHardware))
			{
				serial_port_.set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::hardware));
				ret = 0;
			}
			else if (boost::iequals(strTmp,strFlowControlSoftware))
			{
				serial_port_.set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::software));
				ret = 0;
			}
			else
			{
				std::ostringstream ostr;
				ostr<<"�Ƿ��Ĵ��������Ʋ�����"<<strTmp<<"����ʹ��Ĭ��ֵ\n";
				AddStatusLogWithSynT(ostr.str());
			}
		}

		xml.OutOfElem();
	}

	xml.OutOfElem();
	
	return ret;
}

int CSerialPort::InitConnect(void)
{
	EnableLog();

	int ret = 0;

	ret = EnableProtocol();
	if (ret)
	{
		AddStatusLogWithSynT("Serialͨ����ʼ����Լʧ�ܣ�\n");
		return ret;
	}

	AddStatusLogWithSynT("��ʼ��Serialͨ���ɹ���\n");

	return ret;
}

void CSerialPort::UninitConnect( void )
{
	timerRecv_.cancel();
	timerRetry_.cancel();

	DisableProtocol();

	DisableLog();

	AddStatusLogWithSynT("����ʼ��Serialͨ����\n");
}

int CSerialPort::OpenConnect(void)
{
	boost::system::error_code err;
	if(serial_port_.open(port_no_,err))
	{
		std::ostringstream ostr;
		ostr<<"Serialͨ�����Դ�"<<port_no_<<"ʧ��:"<<err.message()<<std::endl;
		AddStatusLogWithSynT(ostr.str());

		ResetTimerRetry(true);

		return -1;
	}

	retryTimes_ = 0;
	ResetTimerRetry(false);

	if (InitSerialPort())
	{
		std::ostringstream ostr;
		ostr<<"Serialͨ�����Գ�ʼ��"<<port_no_<<"ʧ�ܡ�"<<std::endl;
		AddStatusLogWithSynT(ostr.str());
	}
	
	InitProtocol();

	std::ostringstream ostr;
	ostr<<"Serialͨ����"<<port_no_<<"�ɹ���"<<std::endl;
	AddStatusLogWithSynT(ostr.str());

	ReadFromConnect(recv_data_);

	return 0;
}

int CSerialPort::CloseConnect(void)
{
	timerRecv_.cancel();

	boost::system::error_code err;
	serial_port_.cancel(err);
	if(serial_port_.close(err))
	{
		std::ostringstream ostr;
		ostr<<"Serialͨ�����Թر�"<<port_no_<<"ʧ��"<<std::endl;
		AddStatusLogWithSynT(ostr.str());

		return -1;
	}

	UninitProtocol();
	
	std::ostringstream ostr;
	ostr<<"Serialͨ���ر�"<<port_no_<<"�ɹ���"<<std::endl;
	AddStatusLogWithSynT(ostr.str());

	return 0;
}

int CSerialPort::ReadFromConnect(unsigned char * buf, size_t bytes_transferred /* = 0 */)
{
	if (getConnectAlive())
	{
		serial_port_.async_read_some(boost::asio::buffer(buf,max_length),
			boost::bind(&CSerialPort::handle_read,
				this,//shared_from_this(),
				buf,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));

		timerRecv_.expires_from_now(boost::posix_time::minutes(recv_time_out));
		timerRecv_.async_wait(boost::bind(&CSerialPort::handle_timerRecv,
			this,//shared_from_this(),
			boost::asio::placeholders::error));
	}
	else
	{
		RetryConnect();

		AddStatusLogWithSynT("Serialͨ�����Զ�����ʱ����ͨ��δͨѶ������������ͨ����\n");

		return -1;
	}

	return 0;
}

int CSerialPort::WriteToConnect(const unsigned char * buf, size_t bytes_transferred /* = 0 */)
{
	if (getConnectAlive())
	{
		RecordFrameData(buf,bytes_transferred,false);

		serial_port_.async_write_some(boost::asio::buffer(buf,bytes_transferred),
			boost::bind(&CSerialPort::handle_write,
				this,//shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		AddStatusLogWithSynT("Serialͨ������д����ʱ����ͨ��δͨѶ������������ͨ����\n");

		return -1;
	}

	return 0;
}

bool CSerialPort::getConnectAlive(void)
{
	return serial_port_.is_open();
}

int CSerialPort::RetryConnect(void)
{
	if (getConnectAlive())
	{
		CloseConnect();
	}
	
	OpenConnect();

	return 0;
}

void CSerialPort::handle_read( unsigned char * buf,const boost::system::error_code& error,size_t bytes_transferred )
{
	if(!error)
	{
		timerRecv_.cancel();

		RecordFrameData(buf,bytes_transferred,true);

		//����CProtocolģ�鴦��recvbuf��bytes_transferred�ֽڵı���
		if (protocol_)
		{
			protocol_->RecvProcess(buf,bytes_transferred);
		}
		
		//�ټ���������
		ReadFromConnect(buf);
	}
	else if (error == boost::asio::error::operation_aborted)
	{
		//ͨ�������������ر���,˭�ص�˭����������do nothing
		AddStatusLogWithSynT("Serialͨ�������ݹ����з���ͨ�����رգ��ȴ������ӡ�\n");
		return;
	}
	else
	{
		//����������ϵͳ�׳��Ĵ���û���ദ������������
		AddStatusLogWithSynT("Serialͨ�������ݹ����з���һ��δ֪���󣬳���������ͨ����\n");
		RetryConnect();
	}
}

void CSerialPort::handle_write(const boost::system::error_code& error,size_t bytes_transferred)
{
	if(error == boost::asio::error::operation_aborted)
	{
		//ͨ�������������ر���,˭�ص�˭����������do nothing
		AddStatusLogWithSynT("Serialͨ��д���ݹ����з���ͨ�����رգ��ȴ������ӡ�\n");
		return;
	}
	else if(error)
	{
		//����������ϵͳ�׳��Ĵ���û���ദ������������
		AddStatusLogWithSynT("Serialͨ��д���ݹ����з���һ��δ֪���󣬳���������ͨ����\n");
		RetryConnect();
	}
}

void CSerialPort::handle_timerRecv(const boost::system::error_code& error)
{
	if (!error)
	{
		//����recv_time_outʱ��û���յ��κα��ģ���������ͨ����
		std::ostringstream ostr;
		ostr<<"Serialͨ���Ѿ�"<<recv_time_out<<"����ʱ��δ�յ��κα��ģ�����������ͨ����"<<std::endl;
		AddStatusLogWithSynT(ostr.str());

		RetryConnect();
	}
}

void CSerialPort::handle_timerRetry(const boost::system::error_code& error)
{
	if (!error)
	{
		//����retry_time_outʱ��,���³��Դ򿪴���
		std::ostringstream ostr;
		ostr<<"Serialͨ�����Դ򿪴���"<<port_no_<<std::endl;
		AddStatusLogWithSynT(ostr.str());

		RetryConnect();
	}
}

void CSerialPort::ResetTimerRetry(bool bContinue)
{
	if (bContinue)
	{
		if (retryTimes_++ > retry_times)
		{
			timerRetry_.expires_from_now(boost::posix_time::minutes(reconnect_time_out));
			retryTimes_ = 0;
		}
		else
		{
			timerRetry_.expires_from_now(boost::posix_time::seconds(retry_time_out));
		}

		timerRetry_.async_wait(boost::bind(&CSerialPort::handle_timerRetry,
			this,//shared_from_this(),
			boost::asio::placeholders::error));
	}
	else
	{
		timerRetry_.cancel();
	}
}

} //namespace CommInterface


