#pragma once

#include <boost/asio/deadline_timer.hpp>

namespace PublicSupport {

class CZdTimer
	:public boost::asio::deadline_timer
{
public:
	CZdTimer(boost::asio::io_service & io_service);
	~CZdTimer(void);

	//ÿ������xxСʱ�����ӡ���ʱ���
	boost::posix_time::time_duration expires_zd_hour(boost::posix_time::ptime cur_lt,size_t hour_val);   //ÿhour_valСʱ����0ʱ��ʼ��
	boost::posix_time::time_duration expires_zd_minutes(boost::posix_time::ptime cur_lt,size_t min_val); //ÿmin_val���ӣ���0�ֿ�ʼ����
	boost::posix_time::time_duration expires_zd_seconds(boost::posix_time::ptime cur_lt,size_t sec_val); //ÿsec_val�룬��0�뿪ʼ����

	//ÿ��Ȼ��λʱ���ĳʱ��
	boost::posix_time::time_duration expires_time_everyday(boost::posix_time::ptime cur_lt,boost::posix_time::time_duration time_val); //ÿ���ĳʱĳ��ĳ��
	boost::posix_time::time_duration expires_time_everyhour(boost::posix_time::ptime cur_lt,boost::posix_time::time_duration time_val); //ÿСʱ��ĳ��ĳ��
	boost::posix_time::time_duration expires_time_everyminute(boost::posix_time::ptime cur_lt,boost::posix_time::time_duration time_val); //ÿ���ӵ�ĳ��

	boost::posix_time::time_duration expires_time_everyyear(boost::posix_time::ptime cur_lt,boost::posix_time::ptime time_val); //ÿ���ĳ��ʱ��

};

};//namespace PublicSupport

