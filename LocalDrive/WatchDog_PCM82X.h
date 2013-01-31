#pragma once
#include <boost/asio.hpp>
#include "WatchDog.h"
//#include "tick_count_timer.h"

//namespace FileSystem
//{
//	class CLog;
//}

namespace LocalDrive {

class CWatchDog_PCM82X
	:public CWatchDog
{
public:
	CWatchDog_PCM82X(boost::asio::io_service & io_service,bool  bEnableLog);
	virtual ~CWatchDog_PCM82X(void);
    int run();
    int stop();
	int reset();
	

private:
	int OpenWatchDog();
	void CloseWatchDog();
	void handle_timerFeedWatchDog(const boost::system::error_code& error);
	//int AddLogWithSynT(std::string strVal);
	void ResetTimerFeedWatchDog(bool bContinue,unsigned short timeVal);

private:
	enum
	{
		DefaultWatchDogTimeOut = 180, //Ĭ�Ͽ��Ź���ʱʱ�� ��λ����
		DefaultWatchDogTimeFeed = 120 //Ĭ��ι��ʱ�� ��λ����
	};

	int WatchDogHandle_;
	int WatchDogTimeOut_;
	int WatchDogTimeFeed_;
    unsigned long feedCount_;
	boost::asio::io_service & io_service_;
	boost::asio::deadline_timer timerFeedWatchDog_;
	//tick_count_timer timerFeedWatchDog_;
	//boost::scoped_ptr<FileSystem::CLog> Log_;
};

}//namespace LocalDrive

