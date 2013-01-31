#pragma once
//#include <boost/shared_ptr.hpp>
#include "../PublicSupport/Dat2cTypeDef.h"

namespace FileSystem
{
	class CMarkup;
}

namespace DataBase {

class CYcPoint;

typedef CYcPoint * typeYcPtr;
typedef typeYcval typeLinePara;

struct  stRealData
{
	typeYcPtr Ua_;
	typeYcPtr Ub_;
	typeYcPtr Uc_;
	typeYcPtr U0_;
	typeYcPtr Ia_;
	typeYcPtr Ib_;
	typeYcPtr Ic_;
	typeYcPtr I0_;
	typeYcPtr Iap_;
	typeYcPtr Ibp_;
	typeYcPtr Icp_;
	typeYcPtr P_;
	typeYcPtr Q_;
	typeYcPtr F_;
	typeYcPtr ANGa_;
	typeYcPtr ANGb_;
	typeYcPtr ANGc_;
	typeYcPtr Cos_;
};

struct stProtectPara
{
	typeLinePara i_over_1_;
};

class CLine
{
public:
	CLine(void);
	~CLine(void);

	int LoadXmlCfg(FileSystem::CMarkup & xml);
	void SaveXmlCfg(FileSystem::CMarkup & xml);

private:
	stRealData real_;     //ʵʱ���ݡ�����ָ��CYcPoint��ָ��
	stProtectPara para_;  //������ֵ����ֵ���ݱ���
};

};//namespace DataBase

