#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include "UdpSession.h"
#include "CommPara.h"
#include "../Protocol/Protocol.h"

namespace CommInterface {

CUdpSession::CUdpSession(CCommPara & para,boost::asio::io_service& io_service,udp::socket & socket)
						:CCommInterface(para,io_service),
						socket_(socket)
{
}

CUdpSession::~CUdpSession(void)
{
	if (getConnectAlive())
	{
		CloseConnect();
	}

	UninitConnect();
}

int CUdpSession::InitConnect(void)
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
		AddStatusLogWithSynT("UdpSession����Զ��endpoint�ɹ���\n");
	}

	if((CCommPara::AssertIPFormat(para_.getBroadcastIP())) && (CCommPara::AssertNetPortFormat(para_.getBroadcastPort())))
	{
		try
		{
			int port = boost::lexical_cast<int>(para_.getBroadcastPort());
			broadcast_endpoint_.reset(new udp::endpoint(boost::asio::ip::address::from_string(para_.getBroadcastIP()),port));
			AddStatusLogWithSynT("UdpSession�����㲥�ɹ�\n");
		}
		catch(...)
		{
		}
	}

	int ret = EnableProtocol();
	if (ret)
	{
		AddStatusLogWithSynT("UdpSessionͨ����ʼ����Լʧ�ܣ�\n");
		return ret;
	}

	AddStatusLogWithSynT("��ʼ��UdpSessionͨ���ɹ���\n");

	return 0;
}

void CUdpSession::UninitConnect(void)
{
	DisableProtocol();

	DisableLog();

	AddStatusLogWithSynT("����ʼ��UdpSessionͨ����\n");
}

int CUdpSession::OpenConnect(void)
{
	InitProtocol();

	AddStatusLogWithSynT("һ���µ�UdpSessionͨ�������ɹ���\n");

	return 0;
}

int CUdpSession::CloseConnect(void)
{
	//boost::system::error_code ec;
	//socket_.cancel(ec);
	//socket_.shutdown(udp::socket::shutdown_both,ec);
	//socket_.close();

	UninitProtocol();

	AddStatusLogWithSynT("UdpSessionͨ���ر����ӡ�\n");

	return 0;
}

int CUdpSession::RetryConnect(void)
{
	//CloseConnect();
	//OpenConnect();

	return 0;
}

int CUdpSession::WriteToBroadCast(const unsigned char * buf,size_t bytes_transferred)
{
	if(broadcast_endpoint_)
	{
		RecordFrameData(buf,bytes_transferred,false);

		boost::asio::socket_base::broadcast option(true);
		socket_.set_option(option);

		socket_.async_send_to(boost::asio::buffer(buf, bytes_transferred),*broadcast_endpoint_,
			boost::bind(&CUdpSession::handle_write,
			this,//shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));

		return 0;
	}

	return -1;
}

int CUdpSession::WriteToConnect(const unsigned char * buf, size_t bytes_transferred)
{
	if (remoteEndPoints_.size() == 0)
	{
		AddStatusLogWithSynT("UdpSessionͨ��δ�ܽ������Զ�ͨѶ��,ֻ�ܷ��㲥���ġ�\n");
		return -1;
	}

	RecordFrameData(buf,bytes_transferred,false);

	socket_.async_send_to(boost::asio::buffer(buf, bytes_transferred),getNextEndPoint(),
		boost::bind(&CUdpSession::handle_write,
		this,//shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));

	return 0;
}

int CUdpSession::ReadFromConnect(unsigned char * buf, size_t bytes_transferred)
{
	int ret = 0;

	RecordFrameData(buf,bytes_transferred,true);

	//����CProtocolģ�鴦��recvbuf��bytes_transferred�ֽڵı���
	if (protocol_)
	{
		ret = protocol_->RecvProcess(buf,bytes_transferred);
	}

	return ret;
}

bool CUdpSession::getConnectAlive(void)
{
	return socket_.is_open();
}

void CUdpSession::handle_write(const boost::system::error_code& error,size_t bytes_transferred)
{
	if (error)
	{
		AddStatusLogWithSynT("UdpSessionͨ��д���ݹ����з�������\n");
	}
}

udp::endpoint CUdpSession::getNextEndPoint()
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

bool CUdpSession::MatchRemoteIP(boost::asio::ip::address ipVal)
{
	for (size_t i=0;i<remoteEndPoints_.size();i++)
	{
		if (remoteEndPoints_[i].address() == ipVal)
		{
			return true;
		}
	}

	return false;
}

}; //namespace CommInterface 
