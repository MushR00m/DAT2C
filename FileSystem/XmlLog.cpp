#include "XmlLog.h"
#include <stdio.h>
#include <stdarg.h>
//#include "Markup.h"
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include "../PublicSupport/Dat2cTypeDef.h"
using namespace std;

namespace FileSystem {

const std::string strXmlRecord = "XmlRecord";
const std::string strRecordCounter = "RecordCounter";
const std::string strRecord = "Record";
const std::string strClock = "Time";
const std::string strMaxcounter = "Maxcounter";
//const std::string strXmlHead ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");

CXmlLog::CXmlLog(std::string filename,std::string OpenMode,int limit)
				:CLog(filename,OpenMode)
{
	//strXmlLogPath_ = filename;
	//strMode_ = OpenMode;
	iMaxCounter_ = limit;

	CheckXmlFirstLoad(filename,xml_);

}

CXmlLog::CXmlLog(std::string filename,std::string OpenMode)
				:CLog(filename,OpenMode)
{
	//strXmlLogPath_ = filename;
	//strMode_ = OpenMode;
	iMaxCounter_ = Default_Maxcounter;

	CheckXmlFirstLoad(filename,xml_);

}

CXmlLog::~CXmlLog(void)
{
}

std::string CXmlLog::getFileType()
{
	return strXmlLog;
}

int CXmlLog::getMaxCounter()
{
	return iMaxCounter_;
}

int CXmlLog::setMaxCounter(int val)
{
	iMaxCounter_ = val;

	return 0;
}

/*int CXmlLog::InitXmlLog(std::string filename,std::string OpenMode)
{
	strXmlLogPath_ = filename;
	strMode_ = OpenMode;

	return 0;
}*/

int CXmlLog::CreateEmptyXmlLog(std::string filename,CMarkup & xml)
{
	//FileSystem::CMarkup xml;

	xml.SetDoc(strXmlHead);
	xml.AddElem(strXmlRecord);

	xml.IntoElem();
	xml.AddElem(strRecordCounter,0);
	xml.OutOfElem();

	WriteToDB(filename,xml);
	
	return 0;
}

int CXmlLog::RecordData(std::string var,bool strSynT,CMarkup & xml)
{
	//CMarkup xml;

	CheckXmlLogExsit(strLogPath_,xml);

	xml.FindElem(); 
	xml.IntoElem(); //into root

	xml.AddElem(strRecord,var);			
	if (strSynT)
	{
		boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
		string strTime = to_simple_string(now);
		xml.AddAttrib(strClock,strTime);
	}

	int ret = ResetRecordCounter(xml);

	return ret;
}


int CXmlLog::ResetRecordCounter(CMarkup & xml)
{
	xml.ResetMainPos();
	if (xml.FindElem(strRecordCounter))
	{
		int count = 0;
		try
		{
			count = boost::lexical_cast<int>(xml.GetData());
		}
		catch(boost::bad_lexical_cast & e)
		{
			std::cerr<<"CXmlLog::WriteToFile "<<e.what()<<std::endl;
		}

		if (count < getMaxCounter())
		{
			xml.SetData(++count);
		}
		else
		{
			xml.SetData(getMaxCounter());	

			xml.ResetMainPos();
			if(xml.FindElem(strRecord))
			{
				xml.RemoveElem();
			}
		}

		xml.OutOfElem(); //out of root
	}
	else
	{
		xml.OutOfElem(); //out of root

		xml.ResetMainPos();
		CheckXmlLogCounter(xml);
	}

	WriteToDB(strLogPath_.c_str(),xml);

	return 0;
}

int CXmlLog::CheckXmlLogExsit(std::string filename,CMarkup & xml)
{
	if (!xml.Load(filename))
	{
		remove(filename.c_str()); //ɾ��ԭ���ķ�xml��ʽͬ���ļ����ļ���������ʲôҲ����

		CreateEmptyXmlLog(filename,xml); //����һ�����ļ�

		xml.Load(filename); //���¸�xml���ø�ֵ
	}

	return 0;
}

int CXmlLog::CheckXmlFirstLoad(std::string filename,CMarkup & xml)
{
	if (xml.Load(filename))
	{
		if(CheckXmlLogCounter(xml))
		{
			WriteToDB(filename,xml);
		}
	}

	return 0;
}

int CXmlLog::CheckXmlLogCounter( CMarkup & xml )
{
	int ret = 0;

	xml.ResetMainPos();
	xml.FindElem();
	xml.IntoElem(); //enter root

	int count=0;
	while (xml.FindElem(strRecord))
	{
		count++;
	}

	if (count > getMaxCounter())
	{
		xml.ResetMainPos();
		for (int i=0;i<count - getMaxCounter();i++)
		{
			if (xml.FindElem(strRecord))
			{
				xml.RemoveElem();
			}
		}

		ret = 1; //�÷���ֵret֪ͨ������xml�ļ��ı���
	}
	else
	{
		xml.ResetMainPos();
		if (!xml.FindElem(strRecordCounter))
		{
			xml.AddElem(strRecordCounter,count);

			ret = 1; //�÷���ֵret֪ͨ������xml�ļ��ı���
		}
	}

	xml.OutOfElem(); //out of root

	return ret;
}


int CXmlLog::AddRecord(std::string strval)
{
	if (strLogPath_.empty())
	{
		return -1;
	}

	return RecordData(strval,false,xml_);
}

int CXmlLog::AddRecordWithSynT(std::string strval)
{
	if (strLogPath_.empty())
	{
		return -1;
	}

	return RecordData(strval,true,xml_);
}

bool CXmlLog::WriteToDB(std::string filename,CMarkup & xml)
{
	boost::unique_lock<boost::mutex> uLock(mutex_);
	
	return xml.Save(filename);
}

};//namespace PublicSupport 

