#include <sstream>
#include <iomanip>
#include <iostream>

#include "../PublicSupport/Dat2cPublicAPI.h"
#include "Sm2PrivateKey.h"

namespace DigitalSignature {

const unsigned char bn_line_index = 1;
const size_t DI_SIGN_LENGTH = 96;

using namespace std;

CSm2PrivateKey::CSm2PrivateKey(std::string keypath, std::string passwd)
{
	if (passwd.empty())
	{
		LoadKey(getFileStr(keypath,bn_line_index),NULL);
	}
	else
	{
		LoadKey(getFileStr(keypath,bn_line_index),passwd.c_str());
	}
}

CSm2PrivateKey::CSm2PrivateKey(std::string keypath)
{
	LoadKey(getFileStr(keypath,bn_line_index),NULL);
}

CSm2PrivateKey::~CSm2PrivateKey(void)
{
	FreeKey();
}

int CSm2PrivateKey::getKeyLength()
{
	return DI_SIGN_LENGTH;
}

bool CSm2PrivateKey::ValidKey()
{
	return true;
}

 //װ��˽Կ
//************************************
// Method:    LoadKey
// FullName:  CECC::LoadKey
// Access:    private 
// Returns:   int
// Qualifier:
// Parameter: std::string keypath ˽Կ�Ĵ�������
// Parameter: std::string passwd ˽Կ�ļ��ı������û����дNULL
// Return:    0���ɹ� ������ʧ��
//************************************
int CSm2PrivateKey::LoadKey( std::string strBigNum, const char * passwd )
{
	return 0;
}

//************************************
// Method:    FreeKey
// FullName:  CECC::FreeKey
// Access:    private 
// Returns:   int
// Qualifier:
// Return:    0���ɹ� ������ʧ��
//************************************
int CSm2PrivateKey::FreeKey()
{
	return 0;
}

//************************************
// Method:    Sign
// FullName:  CECC::Sign
// Access:    private 
// Returns:   int
// Qualifier:
// Parameter: const unsigned char * dgst ǩ����������
// Parameter: int dgstlen ǩ���������ݳ���
// Parameter: unsigned char * sig ǩ�����������
// Parameter: unsigned int * siglen ǩ��������ݳ���
// Return:    0���ɹ� ������ʧ��
//************************************
int CSm2PrivateKey::Sign(const unsigned char *dgst, int dgstlen,unsigned char *sig, unsigned int *siglen)
{
	return 0;
}

};//namespace DigitalSignature
