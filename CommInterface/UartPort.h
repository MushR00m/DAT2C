#pragma once
#include "CommInterface.h"

namespace FileSystem
{
	class CMarkup;
}

namespace CommInterface {

class CUartPort:
	public CCommInterface
{
public:
	CUartPort(CCommPara & para,boost::asio::io_service& io_service);
	virtual ~CUartPort(void);

public:
	virtual int InitConnect(void);
	virtual void UninitConnect(void);
	virtual int OpenConnect(void);
	virtual int CloseConnect(void);
	virtual int RetryConnect(void);
	virtual int WriteToConnect(const unsigned char * buf, size_t bytes_transferred);
	//virtual bool getConnectAlive(void);

private:
	int InitSerialPort();
	void SaveSerialCfg(std::string fileName);
	void SaveSerialAttrib(FileSystem::CMarkup & xml);
	int LoadSerialCfg(std::string fileName);
	virtual int ReadFromConnect(unsigned char * buf, size_t bytes_transferred = 0);
	//void handle_read(unsigned char * buf,const boost::system::error_code& error,size_t bytes_transferred);
	void handle_write(const boost::system::error_code& error,size_t bytes_transferred);
	void handle_timerRecv(const boost::system::error_code& error);
	void handle_timerRetry(const boost::system::error_code& error);
	void ResetTimerRetry(bool bContinue);

	void UartRecvThread(unsigned char * buf);

private:
	enum
	{
		recv_time_out = 14,          //���ճ�ʱ(��λ:����)
		retry_time_out = 15,         //���³��Դ򿪴��ڼ��ʱ�䣨��λ���룩
		retry_times = 20,			 //�ж��޷��򿪴��ڵ�retry����
		reconnect_time_out = 30,      //�ж��޷��򿪴��ں��������ʱ����(��λ:����)

		max_length = 1024,
	};

	std::string port_no_;
	unsigned int BaudRate_;
	unsigned char LCR_Val;
	int uart_fd;

	size_t retryTimes_;
	//boost::asio::serial_port serial_port_;
	//boost::asio::deadline_timer timerRecv_;
	boost::asio::deadline_timer timerRetry_;

	unsigned char recv_data_[max_length];

	//int UARTxPthread;
	//int UARTxNum;
	//bool Frist_Flag;
	//bool ConnectAlive;
	
};

} //namespace CommInterface



