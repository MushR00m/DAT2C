#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "UdpClient.h"
#include "../Protocol/Protocol.h"
#include "CommPara.h"

namespace CommInterface {

CUdpClient::CUdpClient(CCommPara & para,boost::asio::io_service & io_service)
					   :CCommInterface(para,io_service),
					   socket_(io_service)
{
	StartBind();
}

void CUdpClient::StartBind()
{
	if (CCommPara::AssertNetPortFormat(para_.getLocalPort()))
	{
		try
		{
			int port = boost::lexical_cast<int>(para_.getLocalPort());
			if (CCommPara::AssertIPFormat(para_.getLocalIP()))
			{
				boost::system::error_code ec;
				socket_.open(udp::v4());
				socket_.bind(udp::endpoint(boost::asio::ip::address::from_string(para_.getLocalIP()),port),ec);
				if(ec)
				{
					std::ostringstream ostr;
					ostr<<"��UdpClient��Socket��IP:"<<para_.getLocalIP()<<"Port:"<<para_.getLocalPort()<<"ʧ�� "<<ec<<std::endl;
					AddStatusLogWithSynT(ostr.str());
					std::cerr<<ostr.str();
				}
				else
				{
					std::ostringstream ostr;
					ostr<<"��UdpClient��Socket��IP:"<<para_.getLocalIP()<<"Port:"<<para_.getLocalPort()<<"�ɹ���"<<std::endl;
					AddStatusLogWithSynT(ostr.str());
				}
			}
			else
			{
				socket_.open(udp::v4());
				socket_.bind(udp::endpoint(udp::v4(),port));
			}

			return;
		}
		catch(boost::bad_lexical_cast & e)
		{
			std::ostringstream ostr;
			ostr<<"��UdpClient��Socket��IP:"<<para_.getLocalIP()<<"Port:"<<para_.getLocalPort()<<"ʧ�� "<<e.what()<<std::endl;
			AddStatusLogWithSynT(ostr.str());
			std::cerr<<ostr.str();
		}
		catch(boost::system::system_error & e)
		{
			std::ostringstream ostr;
			ostr<<"��UdpClient��Socket��IP:"<<para_.getLocalIP()<<"Port:"<<para_.getLocalPort()<<"ʧ�� "<<e.what()<<std::endl;
			AddStatusLogWithSynT(ostr.str());
			std::cerr<<ostr.str();
		}
	}
	else
	{
		socket_.open(udp::v4());
	}
}

CUdpClient::~CUdpClient(void)
{
	if (getConnectAlive())
	{
		CloseConnect();
	}

	UninitConnect();
}

int CUdpClient::InitConnect(void)
{
	EnableLog();

	int EndPointNum = (para_.getRemoteIPSum() < para_.getRemotePortSum()) ? para_.getRemoteIPSum() : para_.getRemotePortSum();

	remoteEndPoints_.clear();

	for (int i=0;i<EndPointNum;i++)
	{
		udp::resolver resolver(io_service_);
		std::string ip = para_.getRemoteIP(i);
		std::string port = para_.getRemotePort(i);

		if (CCommPara::AssertIPFormat(ip) && CCommPara::AssertNetPortFormat(port))
		{
			udp::resolver::query query(udp::v4(), ip, port);
			boost::system::error_code error;
			udp::resolver::iterator endpoint_iterator = resolver.resolve(query,error);
			if (!error)
			{
				remoteEndPoints_.push_back(*endpoint_iterator);
			}
		}
	}

	if (remoteEndPoints_.size() > 0)
	{
		AddStatusLogWithSynT("UdpClient����Զ��endpoint�ɹ���\n");
	}

	if((CCommPara::AssertIPFormat(para_.getBroadcastIP())) && (CCommPara::AssertNetPortFormat(para_.getBroadcastPort())))
	{
		try
		{
			int port = boost::lexical_cast<int>(para_.getBroadcastPort());
			broadcast_endpoint_.reset(new udp::endpoint(boost::asio::ip::address::from_string(para_.getBroadcastIP()),port));
			AddStatusLogWithSynT("UdpClient�����㲥�ɹ�\n");
		}
		catch(...)
		{
		}
	}

	int ret = EnableProtocol();
	if (ret)
	{
		AddStatusLogWithSynT("UdpClientͨ����ʼ����Լʧ�ܣ�\n");
		return ret;
	}

	AddStatusLogWithSynT("��ʼ��UdpClientͨ���ɹ���\n");

	return 0;
}

void CUdpClient::UninitConnect(void)
{
	DisableProtocol();

	DisableLog();

	AddStatusLogWithSynT("����ʼ��UdpClientͨ����\n");
}

udp::endpoint CUdpClient::getNextEndPoint()
{
	if (remoteEndPoints_.size() == 0)
	{
		return udp::endpoint();
	}

	if (remoteEndPointIndex_ < 0 || remoteEndPointIndex_ >= (int)remoteEndPoints_.size())
	{
		remoteEndPointIndex_ = 0;
	}

	udp::endpoint ret = remoteEndPoints_[remoteEndPointIndex_];

	remoteEndPointIndex_ = (++remoteEndPointIndex_) % remoteEndPoints_.size();

	return ret;
}

int CUdpClient::OpenConnect(void)
{
	InitProtocol();

	AddStatusLogWithSynT("UdpClientͨ������socket�ɹ���\n");

	ReadFromConnect(recv_data_);

	return 0;
}

int CUdpClient::CloseConnect(void)
{
	boost::system::error_code ec;
	socket_.cancel(ec);
	socket_.shutdown(udp::socket::shutdown_both,ec);

	socket_.close();

	UninitProtocol();

	AddStatusLogWithSynT("UdpClientͨ���ر�socket��\n");

	return 0;
}

int CUdpClient::RetryConnect(void)
{
	//CloseConnect();
	//OpenConnect();

	return 0;
}

int CUdpClient::ReadFromConnect(unsigned char * buf,size_t bytes_transferred)
{
	if(CCommPara::AssertNetPortFormat(para_.getLocalPort()))
	{
		udp::endpoint remote_endpoint;

		socket_.async_receive_from(boost::asio::buffer(buf,max_length),remote_endpoint,
			boost::bind(&CUdpClient::handle_read,
			this,//shared_from_this(),
			buf,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));

		AddStatusLogWithSynT("UdpClientͨ����ʼ������\n");
	}

	return 0;
}

int CUdpClient::WriteToBroadCast(const unsigned char * buf,size_t bytes_transferred)
{
	if(broadcast_endpoint_)
	{
		RecordFrameData(buf,bytes_transferred,false);

		boost::asio::socket_base::broadcast option(true);
		socket_.set_option(option);

		socket_.async_send_to(boost::asio::buffer(buf, bytes_transferred),*broadcast_endpoint_,
			boost::bind(&CUdpClient::handle_write,
			this,//shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));

		return 0;
	}

	return -1;
}

int CUdpClient::WriteToConnect(const unsigned char * buf,size_t bytes_transferred)
{
	if (remoteEndPoints_.size() == 0)
	{
		AddStatusLogWithSynT("UdpClientͨ��δ�ܽ������Զ�ͨѶ��,ֻ�ܷ��㲥���ġ�\n");
		return -1;
	}

	boost::asio::socket_base::broadcast option(true);
	socket_.set_option(option);

	RecordFrameData(buf,bytes_transferred,false);

	socket_.async_send_to(boost::asio::buffer(buf, bytes_transferred),getNextEndPoint(),
		boost::bind(&CUdpClient::handle_write,
		this,//shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));

	return 0;
}

void CUdpClient::handle_read(unsigned char * buf,const boost::system::error_code& error,size_t bytes_transferred)
{
	if(!error)
	{
		RecordFrameData(buf,bytes_transferred,true);

		//����CProtocolģ�鴦��recvbuf��bytes_transferred�ֽڵı���
		if (protocol_)
		{
			protocol_->RecvProcess(buf,bytes_transferred);
		}

		//�ټ���������
		ReadFromConnect(buf);
	}
	else
	{
		AddStatusLogWithSynT("UdpClientͨ�������ݹ����з�������\n");
	
		CloseConnect();
		UninitConnect();
		InitConnect();
		OpenConnect();
	}
}

void CUdpClient::handle_write(const boost::system::error_code& error,size_t bytes_transferred)
{
	if (error)
	{
		AddStatusLogWithSynT("UdpClientͨ��д���ݹ����з�������\n");
	}
}

bool CUdpClient::getConnectAlive(void)
{
	return socket_.is_open();
}

}; //namespace CommInterface 

