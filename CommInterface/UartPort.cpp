#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>
#include "UartPort.h"
#include "../Protocol/Protocol.h"
#include "../FileSystem/Markup.h"
#include "CommPara.h"


namespace CommInterface {

const std::string strSerialCfgName = "SerialPort.xml";

////xml��㶨��
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
#define Default_LcrVal 0x03

CUartPort::CUartPort(CCommPara & para,boost::asio::io_service& io_service)
	:CCommInterface(para,io_service),
	//serial_port_(io_service),
	//timerRecv_(io_service,boost::posix_time::minutes(recv_time_out)),
	timerRetry_(io_service,boost::posix_time::seconds(retry_time_out))
{
	retryTimes_ = 0;
	port_no_ = para.getLocalPort();
    LCR_Val = Default_LcrVal;
	uart_fd = -1;
}

CUartPort::~CUartPort(void)
{
	if (getConnectAlive())
	{
		CloseConnect();
	}

	UninitConnect();
}

int CUartPort::InitSerialPort()
{
	if (getConnectAlive())
	{
#if defined(_BF518_)
		ioctl(uart_fd,Default_BaudRate,3);
#endif //#if defined(_BF518_)

		if (LoadSerialCfg(strSerialCfgName))
		{
			AddStatusLogWithSynT("δ�ܴ������ļ���ʼ�����ڣ�ʹ��Ĭ��ֵ��ʼ����\n");
		}

		return 0;
	}

	return -1;
}

void CUartPort::SaveSerialAttrib(FileSystem::CMarkup & xml)
{
	xml.AddElem(strBaudRate,BaudRate_);
}

void CUartPort::SaveSerialCfg(std::string fileName)
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

int CUartPort::LoadSerialCfg(std::string fileName)
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
					LCR_Val = LCR_Val & 0xFC;

					if (databits == 5)
					{
						LCR_Val = (LCR_Val | 0x00);
					} 
					else if(databits == 6)
					{
						LCR_Val = (LCR_Val | 0x01);
					}
					else if(databits == 7)
					{
						LCR_Val = (LCR_Val | 0x02);
					}
					else if(databits == 8)
					{
						LCR_Val = (LCR_Val | 0x03);
					}
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
					LCR_Val = (LCR_Val | 0x00);
					ret = 0;
				}
				else if(stopbits == 1.5)
				{
					LCR_Val = (LCR_Val | 0x04);
					ret = 0;
				}
				else if (stopbits == 2)
				{
					LCR_Val = (LCR_Val | 0x04);
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
                LCR_Val = (LCR_Val | 0x00);
				ret = 0;
			}
			else if (boost::iequals(strTmp,strParityOdd))
			{
				LCR_Val = (LCR_Val | 0x08);
				ret = 0;
			}
			else if (boost::iequals(strTmp,strParityEven))
			{
			    LCR_Val = (LCR_Val | 0x18);
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
		if (xml.FindElem(strBaudRate))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				BaudRate_ = boost::lexical_cast<unsigned int>(strTmp);

				if (BaudRate_ >= 300 && BaudRate_ <= 921600)
				{
                    #if defined(_BF518_)
					ioctl(uart_fd,BaudRate_,LCR_Val);
					ret = 0;
                    #endif //#if defined(_BF518_)
				}
				else
				{
					std::ostringstream ostr;
					ostr<<"�Ƿ��Ĵ��ڲ����ʲ�����"<<BaudRate_<<"����ʹ��Ĭ��ֵ\n";
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

		xml.OutOfElem();
	}

	xml.OutOfElem();

	return ret;
}

int CUartPort::InitConnect(void)
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

void CUartPort::UninitConnect( void )
{
	//timerRecv_.cancel();
	timerRetry_.cancel();

	DisableProtocol();

	DisableLog();

	AddStatusLogWithSynT("����ʼ��Serialͨ����\n");
}

int CUartPort::OpenConnect(void)
{

# if defined (_BF518_)
	uart_fd = open(port_no_.c_str(),O_RDWR);
#endif //# if defined (_BF518_)

	if (!uart_fd)
	{
		std::ostringstream ostr;
		ostr<<"Serialͨ�����Դ�"<<port_no_<<"ʧ��"<<std::endl;
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

int CUartPort::CloseConnect(void)
{
	//timerRecv_.cancel();

#if defined(_BF518_)
	if(close(uart_fd))
	{
		std::ostringstream ostr;
		ostr<<"Serialͨ�����Թر�"<<port_no_<<"ʧ��"<<std::endl;
		AddStatusLogWithSynT(ostr.str());

		return -1;
	}
#endif //#if defined(_BF518_)

	UninitProtocol();

	std::ostringstream ostr;
	ostr<<"Serialͨ���ر�"<<port_no_<<"�ɹ���"<<std::endl;
	AddStatusLogWithSynT(ostr.str());

	return 0;
}

void CUartPort::UartRecvThread(unsigned char * buf)
{
	boost::asio::deadline_timer blockTimer(io_service_,boost::posix_time::milliseconds(40));//milliseconds  microseconds

#if defined (_BF518_)

	while (true)
	{
		usleep(30000);
		int bytes_transferred = read(uart_fd,buf,1024);

		if (bytes_transferred > 0)
		{
			RecordFrameData(buf,bytes_transferred,true);

			//����CProtocolģ�鴦��recvbuf��bytes_transferred�ֽڵı���
			if (protocol_)
			{
				protocol_->RecvProcess(buf,bytes_transferred);
			}
		}
		else
		{
			blockTimer.wait();
		}
	}

#endif //#if defined (_BF518_)

}

int CUartPort::ReadFromConnect(unsigned char * buf, size_t bytes_transferred /* = 0 */)
{

	boost::thread thrd(boost::bind(&CUartPort::UartRecvThread,this,buf));
	//thrd.join();
	  
	return 0;
}

int CUartPort::WriteToConnect(const unsigned char * buf, size_t bytes_transferred /* = 0 */)
{
	if (getConnectAlive())
	{
		RecordFrameData(buf,bytes_transferred,false);
        
#if defined(_BF518_)

		write(uart_fd,buf,bytes_transferred);

#endif //#if defined(_BF518_)
	}

	return 0;
}

int CUartPort::RetryConnect(void)
{
	if (getConnectAlive())
	{
		CloseConnect();
	}

	OpenConnect();

	return 0;
}

//void CUartPort::handle_read( unsigned char * buf,const boost::system::error_code& error,size_t bytes_transferred )
//{
//	if(!error)
//	{
//		timerRecv_.cancel();
//
//		RecordFrameData(buf,bytes_transferred,true);
//
//		//����CProtocolģ�鴦��recvbuf��bytes_transferred�ֽڵı���
//		if (protocol_)
//		{
//			protocol_->RecvProcess(buf,bytes_transferred);
//		}
//
//		//�ټ���������
//		ReadFromConnect(buf);
//	}
//	else if (error == boost::asio::error::operation_aborted)
//	{
//		//ͨ�������������ر���,˭�ص�˭����������do nothing
//		AddStatusLogWithSynT("Serialͨ�������ݹ����з���ͨ�����رգ��ȴ������ӡ�\n");
//		return;
//	}
//	else
//	{
//		//����������ϵͳ�׳��Ĵ���û���ദ������������
//		AddStatusLogWithSynT("Serialͨ�������ݹ����з���һ��δ֪���󣬳���������ͨ����\n");
//		RetryConnect();
//	}
//}

void CUartPort::handle_write(const boost::system::error_code& error,size_t bytes_transferred)
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

void CUartPort::handle_timerRecv(const boost::system::error_code& error)
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

void CUartPort::handle_timerRetry(const boost::system::error_code& error)
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

void CUartPort::ResetTimerRetry(bool bContinue)
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

		timerRetry_.async_wait(boost::bind(&CUartPort::handle_timerRetry,
			this,//shared_from_this(),
			boost::asio::placeholders::error));
	}
	else
	{
		timerRetry_.cancel();
	}
}

} //namespace CommInterface



