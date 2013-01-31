#include <boost/bind.hpp>
#include "TcpSession.h"
#include "../Protocol/Protocol.h"

namespace CommInterface {

using boost::asio::ip::tcp;

CTcpSession::CTcpSession(CCommPara & para,boost::asio::io_service& io_service)
						:CCommInterface(para,io_service),
						socket_(io_service),
						timerRecv_(io_service,boost::posix_time::minutes(recv_time_out))
						
{
	
}

CTcpSession::~CTcpSession(void)
{
	if (getConnectAlive())
	{
		CloseConnect();
	}

	UninitConnect();
}

tcp::socket& CTcpSession::socket(void)
{
	return socket_;
}

int CTcpSession::InitConnect(void)
{
	EnableLog();

	int ret = 0;

	ret = EnableProtocol();
	if (ret)
	{
		AddStatusLogWithSynT("TcpSessionͨ����ʼ����Լʧ�ܣ�\n");
		return ret;
	}

	AddStatusLogWithSynT("��ʼ��TcpSessionͨ���ɹ���\n");

	return ret;
}

void CTcpSession::UninitConnect(void)
{
	DisableProtocol();

	DisableLog();

	AddStatusLogWithSynT("����ʼ��TcpSessionͨ����\n");
}

int CTcpSession::OpenConnect(void)
{
	InitProtocol();

	std::ostringstream ostr;
	ostr<<"һ���µ�TcpSessionͨ�������ɹ����Զ���Ϣ��"<<socket_.remote_endpoint().address().to_string()<<":"<<socket_.remote_endpoint().port()<<std::endl;

	AddStatusLogWithSynT(ostr.str());

	ReadFromConnect(recv_data_);

	return 0;
}

int CTcpSession::CloseConnect(void)
{
	timerRecv_.cancel();

	boost::system::error_code ec;
	socket_.cancel(ec);
	socket_.shutdown(tcp::socket::shutdown_both,ec);

	socket_.close();

	UninitProtocol();

	AddStatusLogWithSynT("TcpSessionͨ���ر����ӡ�\n");

	return 0;
}

int CTcpSession::ReadFromConnect(unsigned char * buf, size_t bytes_transferred /* = 0 */)
{
	if (getConnectAlive())
	{
		socket_.async_read_some(boost::asio::buffer(buf,max_length),
			boost::bind(&CTcpSession::handle_read,
			shared_from_this(),
			buf,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));

		timerRecv_.expires_from_now(boost::posix_time::minutes(recv_time_out));
		timerRecv_.async_wait(boost::bind(&CTcpSession::handle_timerRecv,
			shared_from_this(),
			boost::asio::placeholders::error));
	}
	else
	{
		CloseConnect();

		AddStatusLogWithSynT("TcpSessionͨ�����Զ�����ʱ����ͨ��δͨѶ������������ͨ����\n");

		return -1;
	}

	return 0;
}

int CTcpSession::WriteToConnect(const unsigned char * buf, size_t bytes_transferred /* = 0 */)
{
	if (getConnectAlive())
	{
		RecordFrameData(buf,bytes_transferred,false);

		socket_.async_send(boost::asio::buffer(buf, bytes_transferred),
			boost::bind(&CTcpSession::handle_write, 
			shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		AddStatusLogWithSynT("TcpSessionͨ������д����ʱ����ͨ��δͨѶ������������ͨ����\n");

		return -1;
	}

	return 0;
}

bool CTcpSession::getConnectAlive(void)
{
	return socket_.is_open();
}

int CTcpSession::RetryConnect(void)
{
	CloseConnect();
	
	return 0;
}

void CTcpSession::handle_read( unsigned char * buf,const boost::system::error_code& error,size_t bytes_transferred )
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
		AddStatusLogWithSynT("TcpSessionͨ�������ݹ����з���ͨ�����رգ��ȴ������ӡ�\n");
	}
	else if(error == boost::asio::error::timed_out)
	{
		//socket���ӳ�ʱ
		AddStatusLogWithSynT("TcpSessionͨ�������ݹ����з�������socket�����ӳ�ʱ�����Թر�ͨ����\n");
		CloseConnect();
	}
	else if (error == boost::asio::error::eof)
	{
		//�Զ˹ر�socket����
		AddStatusLogWithSynT("TcpSessionͨ�������ݹ����з������󣬶Զ˹ر�socket�����Թر�ͨ����\n");
		CloseConnect();
	}
	else
	{
		//����������ϵͳ�׳��Ĵ���û���ദ������������
		AddStatusLogWithSynT("TcpSessionͨ�������ݹ����з���һ��δ����Ĵ���,���Թر�ͨ����\n");
		CloseConnect();
	}
}

void CTcpSession::handle_write(const boost::system::error_code& error,size_t bytes_transferred)
{
	if(error == boost::asio::error::operation_aborted)
	{
		//ͨ�������������ر���,˭�ص�˭����������do nothing
		AddStatusLogWithSynT("TcpSessionͨ��д���ݹ����з���ͨ�����رգ��ȴ������ӡ�\n");
		return;
	}
	else if(error == boost::asio::error::timed_out)
	{
		//socket���ӳ�ʱ
		AddStatusLogWithSynT("TcpSessionͨ��д���ݹ����з�������socket�����ӳ�ʱ�����Թر�ͨ����\n");
		CloseConnect();
	}
	else if (error == boost::asio::error::eof)
	{
		//�Զ˹ر�socket����
		AddStatusLogWithSynT("TcpSessionͨ��д���ݹ����з������󣬶Զ˹ر�socket�����Թر�ͨ����\n");
		CloseConnect();
	}
	else if(error)
	{
		//����������ϵͳ�׳��Ĵ���û���ദ������������
		AddStatusLogWithSynT("TcpSessionͨ��д���ݹ����з���һ��δ����Ĵ���,���Թر�ͨ����\n");
		CloseConnect();
	}
}

void CTcpSession::handle_timerRecv(const boost::system::error_code& error)
{
	if (!error)
	{
		//����recv_time_outʱ��û���յ��κα��ģ���������ͨ����
		std::ostringstream ostr;
		ostr<<"TcpSessionͨ���Ѿ�"<<recv_time_out<<"����ʱ��δ�յ��κα��ģ�����������ͨ����"<<std::endl;
		AddStatusLogWithSynT(ostr.str());

		CloseConnect();
	}
}

int CTcpSession::setCommPara(CCommPara val)
{
	para_ = val;

	return 0;
}

} //namespace CommInterface


