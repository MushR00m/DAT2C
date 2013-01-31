#include <sstream>
#include <iomanip>
#include <iostream>

#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rand.h>

#include "../PublicSupport/Dat2cPublicAPI.h"
#include "RsaPrivateKey.h"

namespace DigitalSignature {

const char rnd_seed[] = "string to make the random number generator think it has entropy";
const size_t DI_SIGN_LENGTH = 96;

using namespace std;

CRsaPrivateKey::CRsaPrivateKey(std::string keypath, std::string passwd)
{
	key_ = NULL;

	if (passwd.empty())
	{
		LoadKey(keypath,NULL);
	}
	else
	{
		LoadKey(keypath,passwd.c_str());
	}
}

CRsaPrivateKey::CRsaPrivateKey(std::string keypath)
{
	key_ = NULL;

	LoadKey(keypath,NULL);
}

CRsaPrivateKey::~CRsaPrivateKey(void)
{
	FreeKey();
}

int CRsaPrivateKey::getKeyLength()
{
	return DI_SIGN_LENGTH;
}

bool CRsaPrivateKey::ValidKey()
{
	if (key_)
	{
		return RSA_check_key(key_) == 1;
	}

	return false;
}

//װ��˽Կ
//************************************
// Method:    LoadKey
// FullName:  CRSA::LoadKey
// Access:    private 
// Returns:   int
// Qualifier:
// Parameter: std::string keypath ˽Կ�ļ�·��
// Parameter: std::string passwd ˽Կ�ļ��ı������û����дNULL
// Return:    0���ɹ� ������ʧ��
//************************************
int CRsaPrivateKey::LoadKey( std::string keypath, const char * passwd )
{
	RAND_seed(rnd_seed, sizeof(rnd_seed)); /* or OAEP may fail */
	OpenSSL_add_all_algorithms();
	OpenSSL_add_all_ciphers();
	OpenSSL_add_all_digests();
	ERR_load_crypto_strings();

	BIO * bio = BIO_new_file(keypath.c_str(),"rb");
	if (!bio) 
	{
		ostringstream ostr;
		ostr<<"LoadKey: "<<ERR_error_string(ERR_get_error(),NULL)<<endl;
		cerr<<ostr.str();
		return -1;
	}

	key_ = PEM_read_bio_RSAPrivateKey(bio,NULL,NULL,(void *)passwd);
	if (!key_) 
	{
		ostringstream ostr;
		ostr<<"LoadKey: "<<ERR_error_string(ERR_get_error(),NULL)<<endl;
		cerr<<ostr.str();
		return -1;
	}

	if (!ValidKey()) 
	{
		FreeKey();

		ostringstream ostr;
		ostr<<"LoadKey: "<<ERR_error_string(ERR_get_error(),NULL)<<endl;
		cerr<<ostr.str();
		return -1;
	}

	BIO_free(bio);

	return 0;
}

//************************************
// Method:    FreeKey
// FullName:  CRSA::FreeKey
// Access:    private 
// Returns:   int
// Qualifier:
// Return:    0���ɹ� ������ʧ��
//************************************
int CRsaPrivateKey::FreeKey()
{
	if(key_)
	{
		RSA_free(key_);
	}

	key_ = NULL;

	ERR_free_strings();

	return 0;
}

//************************************
// Method:    Sign
// FullName:  CRSA::Sign
// Access:    private 
// Returns:   int
// Qualifier:
// Parameter: const unsigned char * dgst ǩ����������
// Parameter: int dgstlen ǩ���������ݳ���
// Parameter: unsigned char * sig ǩ�����������
// Parameter: unsigned int * siglen ǩ��������ݳ���
// Return:    0���ɹ� ������ʧ��
//************************************
int CRsaPrivateKey::Sign(const unsigned char *dgst, int dgstlen,unsigned char *sig, unsigned int *siglen)
{
	if (!key_)
	{
		ostringstream ostr;
		ostr<<"��Կδ��װ��"<<endl;
		cerr<<ostr.str();

		return -1;
	}
	int ret = RSA_sign(0,dgst,dgstlen,sig,siglen,key_);
	if (ret != 1) 
	{
		ostringstream ostr;
		ostr<<"RSA_sign: "<<ERR_error_string(ERR_get_error(),NULL)<<endl;
		cerr<<ostr.str();

		return -1;
	}

	return 0;
}

};//namespace DigitalSignature

