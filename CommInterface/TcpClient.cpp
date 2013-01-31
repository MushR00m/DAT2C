#include <boost/bind.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include "TcpClient.h"
#include "../Protocol/Protocol.h"
#include "CommPara.h"

namespace CommInterface {

CTcpClient::CTcpClient(CCommPara & para, boost::asio::io_service& io_service)
					   :CCommInterface(para,io_service),
					   socket_(io_service),
					   timerRecv_(io_service,boost::posix_time::minutes(recv_time_out)),
					   timerReconnect_(io_service,boost::posix_time::minutes(reconnect_time_out))
{
	ReconnectTimes_ = 0;
	remoteEndPointIndex_  = 0;
}

CTcpClient::~CTcpClient(void)
{
	if (getConnectAlive())
	{
		CloseConnect();
	}

	UninitConnect();
}

int CTcpClient::InitConnect(void)
{
	EnableLog();

	int ret = 0;

	int EndPointNum = (para_.getRemoteIPSum() < para_.getRemotePortSum()) ? para_.getRemoteIPSum() : para_.getRemotePortSum();

	remoteEndPoints_.clear();

	for (int i=0;i<EndPointNum;i++)
	{
		tcp::resolver resolver(io_service_);
		std::string ip = para_.getRemoteIP(i);
		std::string port = para_.getRemotePort(i);

		if (CCommPara::AssertIPFormat(ip) && CCommPara::AssertNetPortFormat(port))
		{
			tcp::resolver::query query(tcp::v4(), ip, port);
			boost::system::error_code error;
			tcp::resolver::iterator endpoint_iterator = resolver.resolve(query,error);
			if (!error)
			{
				tcp::endpoint remote_point = *endpoint_iterator;
				remoteEndPoints_.push_back(remote_point);
			}
		}
	}

	if (remoteEndPoints_.size() > 0)
	{
		AddStatusLogWithSynT("TcpClient����endpoint�ɹ���\n");
	}
	else
	{
		AddStatusLogWithSynT("TcpClient����endpointʧ�ܡ�\n");

		return -1;
	}

	socket_.open(tcp::v4());

	if (CCommPara::AssertIPFormat(para_.getLocalIP()) && CCommPara::AssertNetPortFormat(para_.getLocalPort()))
	{
		try
		{
			size_t LocalPort = boost::lexical_cast<size_t>(para_.getLocalPort());
			socket_.bind(tcp::endpoint(boost::asio::ip::address::from_string(para_.getLocalIP()),LocalPort));

			std::ostringstream ostr;
			ostr<<"��TcpClient��Socket��IP:"<<para_.getLocalIP()<<"Port:"<<para_.getLocalPort()<<"�ɹ���"<<std::endl;
			AddStatusLogWithSynT(ostr.str());
		}
		catch(...)
		{
			std::ostringstream ostr;
			ostr<<"��TcpClient��Socket��IP:"<<para_.getLocalIP()<<"Port:"<<para_.getLocalPort()<<"ʧ�ܣ�"<<std::endl;
			AddStatusLogWithSynT(ostr.str());

		}
	}
	
	ret = EnableProtocol();
	if (ret)
	{
		AddStatusLogWithSynT("TcpClient��ʼ����Լʧ�ܣ�\n");
		return ret;
	}

	AddStatusLogWithSynT("��ʼ��TcpClientͨ���ɹ���\n");

	return ret;
}

void CTcpClient::UninitConnect(void)
{
	//resolver_.cancel();

	DisableProtocol();

	remoteEndPoints_.clear();

	DisableLog();

	AddStatusLogWithSynT("����ʼ��TcpClientͨ����\n");
}

tcp::endpoint CTcpClient::getNextEndPoint()
{
	if (remoteEndPoints_.size() == 0)
	{
		return tcp::endpoint();
	}

	if (remoteEndPointIndex_ < 0 || remoteEndPointIndex_ >= (int)remoteEndPoints_.size())
	{
		remoteEndPointIndex_ = 0;
	}

	tcp::endpoint ret = remoteEndPoints_[remoteEndPointIndex_];

	remoteEndPointIndex_ = (++remoteEndPointIndex_) % remoteEndPoints_.size();

	return ret;
}

int CTcpClient::OpenConnect(void)
{
	if (remoteEndPoints_.size() == 0)
	{
		AddStatusLogWithSynT("TcpClientͨ��δ�ܽ������Զ�ͨѶ��\n");
		return -1;
	}

	tcp::endpoint point = getNextEndPoint();

	socket_.async_connect(point,	
		boost::bind(&CTcpClient::handle_connect, 
		this,//shared_from_this(), 
		boost::asio::placeholders::error));

	std::ostringstream ostr;
	ostr<<"TcpClientͨ����������socket��"<<point.address().to_string()<<":"<<point.port()<<std::endl;
	AddStatusLogWithSynT(ostr.str());
		
	return 0;
}

int CTcpClient::CloseConnect(void)
{
	timerRecv_.cancel();

	boost::system::error_code ec;
	socket_.cancel(ec);
	socket_.shutdown(tcp::socket::shutdown_both,ec);

	socket_.close();

	UninitProtocol(para_.getbDisableCommpointByCloseConnect());

	AddStatusLogWithSynT("TcpClientͨ���ر�socket��\n");
				
	return 0;
}

int CTcpClient::RetryConnect(void)
{
	timerReconnect_.cancel();

	if (getConnectAlive())
	{
		CloseConnect();
	}
	
	OpenConnect();

	return 0;
}
int CTcpClient::ReadFromConnect( unsigned char * buf,size_t bytes_transferred /*= 0*/ )
{
	if (getConnectAlive())
	{
		socket_.async_read_some(boost::asio::buffer(buf,max_length),
			boost::bind(&CTcpClient::handle_read,
				this,//shared_from_this(),
				buf,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));

		timerRecv_.expires_from_now(boost::posix_time::minutes(recv_time_out));
		timerRecv_.async_wait(boost::bind(&CTcpClient::handle_timerRecv,
			this,//shared_from_this(),
			boost::asio::placeholders::error));
	}
	else
	{
		RetryConnect();

		AddStatusLogWithSynT("TcpClientͨ�����Զ�����ʱ����ͨ��δͨѶ������������ͨ����\n");

		return -1;
	}
	
	
	return 0;
}

int CTcpClient::WriteToConnect( const unsigned char * buf,size_t bytes_transferred )
{
	if (getConnectAlive())
	{
		RecordFrameData(buf,bytes_transferred,false);

		socket_.async_send(boost::asio::buffer(buf, bytes_transferred),
			boost::bind(&CTcpClient::handle_write, 
				this,//shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		AddStatusLogWithSynT("TcpClientͨ������д����ʱ����ͨ��δͨѶ���ȴ�������ͨ����\n");

		return -1;
	}
	

	return 0;
}

/*
void CTcpClient::handle_resolve(const boost::system::error_code& error, tcp::resolver::iterator endpoint_iterator)
{
	if (!error) 
	{
		//�ɹ���������ַ������������飬������
		endpoint_ = *endpoint_iterator;
		//OpenConnect();
	}
	else
	{
		//��������
		std::cout << "Error: " << error.message() << "\n";
	}

}
*/

void CTcpClient::handle_connect( const boost::system::error_code& err )
{
	if (!err)
	{
		// connect���ӳɹ�,��ʼ������
		ReconnectTimes_ = 0;
		timerReconnect_.cancel();

		InitProtocol();
		
		AddStatusLogWithSynT("TcpClientͨ������socket�ɹ���\n");

		ReadFromConnect(recv_data_);
	}
	else
	{
		//���еĵ㶼����ʧ���ˣ���������
		if(ReconnectTimes_++ <= reconnect_times)
		{
			timerReconnect_.expires_from_now(boost::posix_time::seconds(retry_time_out));
			timerReconnect_.async_wait(boost::bind(&CTcpClient::handle_timerReconnect,
				this,//shared_from_this(),
				boost::asio::placeholders::error));

			std::ostringstream ostr;
			ostr<<"TcpClientͨ������socketʧ�ܵ�"<<ReconnectTimes_<<"�Σ�"<<retry_time_out<<"��󽫳�������ͨ����"<<std::endl;
			AddStatusLogWithSynT(ostr.str());

			//RetryConnect();
		}
		else
		{
			//��������ReconnectTimes���ˣ������չ���,����һ����ʱ��ÿreconnect_time_outʱ�����½���һ������
			timerReconnect_.expires_from_now(boost::posix_time::minutes(reconnect_time_out));
			timerReconnect_.async_wait(boost::bind(&CTcpClient::handle_timerReconnect,
				this,//shared_from_this(),
				boost::asio::placeholders::error));

			UninitProtocol(true);
			
			std::ostringstream ostr;
			ostr<<"TcpClientͨ������socketʧ����"<<ReconnectTimes_<<"�Σ����ȴ�"<<reconnect_time_out<<"�����ٳ���������ͨ����"<<std::endl;
			AddStatusLogWithSynT(ostr.str());

			ReconnectTimes_ = 0;
		}
		
	}

}

void CTcpClient::handle_read( unsigned char * buf,const boost::system::error_code& error,size_t bytes_transferred )
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
		//socket�����������ر���,˭�ص�˭����������do nothing
		AddStatusLogWithSynT("TcpClientͨ�������ݹ����з���ͨ�����رգ��ȴ������ӡ�\n");
		return;
	}
	else if(error == boost::asio::error::timed_out)
	{
		//socket���ӳ�ʱ
		AddStatusLogWithSynT("TcpClientͨ�������ݹ����з�������socket�����ӳ�ʱ������������ͨ����\n");
		//RetryConnect();
		timerReconnect_.expires_from_now(boost::posix_time::seconds(retry_time_out));
		timerReconnect_.async_wait(boost::bind(&CTcpClient::handle_timerReconnect,
			this,//shared_from_this(),
			boost::asio::placeholders::error));
	}
	else if (error == boost::asio::error::eof)
	{
		//�Զ˹ر�socket����
		AddStatusLogWithSynT("TcpClientͨ�������ݹ����з������󣬶Զ˹ر�socket������������ͨ����\n");
		//RetryConnect();
		timerReconnect_.expires_from_now(boost::posix_time::seconds(retry_time_out));
		timerReconnect_.async_wait(boost::bind(&CTcpClient::handle_timerReconnect,
			this,//shared_from_this(),
			boost::asio::placeholders::error));
	}
	else
	{
		//����������ϵͳ�׳��Ĵ���û���ദ������������
		AddStatusLogWithSynT("TcpClientͨ�������ݹ����з���һ��δ����Ĵ���,����������ͨ����\n");
		//RetryConnect();
		timerReconnect_.expires_from_now(boost::posix_time::seconds(retry_time_out));
		timerReconnect_.async_wait(boost::bind(&CTcpClient::handle_timerReconnect,
			this,//shared_from_this(),
			boost::asio::placeholders::error));
	}
}

void CTcpClient::handle_write( const boost::system::error_code& error,size_t bytes_transferred)
{
	if(error == boost::asio::error::operation_aborted)
	{
		//ͨ�������������ر���,˭�ص�˭����������do nothing
		AddStatusLogWithSynT("TcpClientͨ��д���ݹ����з���ͨ�����رգ��ȴ������ӡ�\n");
		return;
	}
	else if(error == boost::asio::error::timed_out)
	{
		//socket���ӳ�ʱ
		AddStatusLogWithSynT("TcpClientͨ��д���ݹ����з�������socket�����ӳ�ʱ������������ͨ����\n");
		//RetryConnect();
		timerReconnect_.expires_from_now(boost::posix_time::seconds(retry_time_out));
		timerReconnect_.async_wait(boost::bind(&CTcpClient::handle_timerReconnect,
			this,//shared_from_this(),
			boost::asio::placeholders::error));
	}
	else if (error == boost::asio::error::eof)
	{
		//�Զ˹ر�socket����
		AddStatusLogWithSynT("TcpClientͨ��д���ݹ����з������󣬶Զ˹ر�socket������������ͨ����\n");
		//RetryConnect();
		timerReconnect_.expires_from_now(boost::posix_time::seconds(retry_time_out));
		timerReconnect_.async_wait(boost::bind(&CTcpClient::handle_timerReconnect,
			this,//shared_from_this(),
			boost::asio::placeholders::error));
	}
	else if(error)
	{
		//����������ϵͳ�׳��Ĵ���û���ദ�����ǲ��ܰ�
		AddStatusLogWithSynT("TcpClientͨ��д���ݹ����з���һ��δ����Ĵ���,�ȴ�������ͨ����\n");
		//RetryConnect();
		//timerReconnect_.expires_from_now(boost::posix_time::seconds(retry_time_out));
		//timerReconnect_.async_wait(boost::bind(&CTcpClient::handle_timerReconnect,
		//	this,//shared_from_this(),
		//	boost::asio::placeholders::error));
	}

}

bool CTcpClient::getConnectAlive(void)
{
	return socket_.is_open();
}

/*
tcp::socket& CTcpClient::socket(void)
{
	return socket_;
}
*/

void CTcpClient::handle_timerRecv(const boost::system::error_code& error)
{
	if (!error)
	{
		//����recv_time_outʱ��û���յ��κα��ģ���������ͨ����
		std::ostringstream ostr;
		ostr<<"TcpClientͨ���Ѿ�"<<recv_time_out<<"����ʱ��δ�յ��κα��ģ�����������ͨ����"<<std::endl;
		AddStatusLogWithSynT(ostr.str());

		RetryConnect();
	}
}

void CTcpClient::handle_timerReconnect(const boost::system::error_code& error)
{
	if (!error)
	{
		RetryConnect();
	}
}

} //namespace CommInterface


