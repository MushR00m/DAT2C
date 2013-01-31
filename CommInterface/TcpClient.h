#pragma once
#include "CommInterface.h"

namespace CommInterface {

using boost::asio::ip::tcp;

class CTcpClient :
	//public boost::enable_shared_from_this<CTcpClient>,
	public CCommInterface
{
public:
	CTcpClient(CCommPara & para,boost::asio::io_service& io_service);
	virtual ~CTcpClient(void);

	virtual int InitConnect(void);
	virtual void UninitConnect(void);
	virtual int OpenConnect(void);
	virtual int CloseConnect(void);
	virtual int RetryConnect(void);
	virtual int WriteToConnect(const unsigned char * buf,size_t bytes_transferred);
	virtual bool getConnectAlive(void);
	//tcp::socket& socket(void);
	
private:
	virtual int ReadFromConnect(unsigned char * buf,size_t bytes_transferred = 0);
	//void handle_resolve(const boost::system::error_code& error, tcp::resolver::iterator endpoint_iterator);
	void handle_connect(const boost::system::error_code& err);
	void handle_read(unsigned char * buf,const boost::system::error_code& error,size_t bytes_transferred);
	void handle_write(const boost::system::error_code& error,size_t bytes_transferred);
	void handle_timerRecv(const boost::system::error_code& error);
	void handle_timerReconnect(const boost::system::error_code& error);
	tcp::endpoint getNextEndPoint();

private:
	enum 
	{ 
		recv_time_out = 7,           //���ճ�ʱ(��λ:����)
		reconnect_times = 20,        //�ж����Ӳ��ϵ�connnect��������
		retry_time_out = 3,          //������socket�ĳ�ʱ������λ���룩
		reconnect_time_out = 8,       //�ж����Ӳ��Ϻ�������Ӽ��(��λ:����)

		max_length = MAX_TCP_MTU,
	};

	size_t ReconnectTimes_;

	tcp::socket socket_;
	std::vector<tcp::endpoint> remoteEndPoints_;
	int remoteEndPointIndex_;

	boost::asio::deadline_timer timerRecv_;
	boost::asio::deadline_timer timerReconnect_;

	unsigned char recv_data_[max_length];
};

} //namespace CommInterface




