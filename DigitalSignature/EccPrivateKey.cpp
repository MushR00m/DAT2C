#include <sstream>
#include <iomanip>
#include <iostream>
#include <cstring>

#if defined(__linux__)
#include <netinet/in.h>
#endif

#if defined(_WIN32)
#include <winsock2.h>
#endif

#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/md5.h>

#include "../PublicSupport/Dat2cPublicAPI.h"
#include "EccPrivateKey.h"

namespace DigitalSignature {

const char rnd_seed[] = "string to make the random number generator think it has entropy";
const size_t DI_SIGN_LENGTH = 96;
const unsigned char DI_SIGN_START_TAG = 0x16;

using namespace std;

CEccPrivateKey::CEccPrivateKey(std::string keypath, std::string passwd)
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

CEccPrivateKey::CEccPrivateKey(std::string keypath)
{
	key_ = NULL;

	LoadKey(keypath,NULL);
}

CEccPrivateKey::~CEccPrivateKey(void)
{
	FreeKey();
}

int CEccPrivateKey::getKeyLength()
{
	return DI_SIGN_LENGTH;
}

bool CEccPrivateKey::ValidKey()
{
	if (key_)
	{
		return EC_KEY_check_key(key_) == 1;
	}

	return false;
}

 //װ��˽Կ
//************************************
// Method:    LoadKey
// FullName:  CECC::LoadKey
// Access:    private 
// Returns:   int
// Qualifier:
// Parameter: std::string keypath ˽Կ�ļ�·��
// Parameter: std::string passwd ˽Կ�ļ��ı������û����дNULL
// Return:    0���ɹ� ������ʧ��
//************************************
int CEccPrivateKey::LoadKey( std::string keypath, const char * passwd )
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

	key_ = PEM_read_bio_ECPrivateKey(bio,NULL,NULL,(void *)passwd);
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
// FullName:  CECC::FreeKey
// Access:    private 
// Returns:   int
// Qualifier:
// Return:    0���ɹ� ������ʧ��
//************************************
int CEccPrivateKey::FreeKey()
{
	if(key_)
	{
		EC_KEY_free(key_);
	}

	key_ = NULL;

	ERR_free_strings();

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
int CEccPrivateKey::Sign(const unsigned char *dgst, int dgstlen,unsigned char *sig, unsigned int *siglen)
{
	if (!key_)
	{
		ostringstream ostr;
		ostr<<"��Կδ��װ��"<<endl;
		cerr<<ostr.str();

		return -1;
	}

	int ret = ECDSA_sign(0,dgst,dgstlen,sig,siglen,key_);
	if (ret != 1) 
	{
		ostringstream ostr;
		ostr<<"ECDSA_sign: "<<ERR_error_string(ERR_get_error(),NULL)<<endl;
		cerr<<ostr.str();

		return -1;
	}

	return 0;
}

//************************************
// Method:    AssembleSignature
// FullName:  CECC::AssembleSignature
// Access:    public 
// Returns:   int
// Qualifier: ��װǩ�� out = ԭʼ����|16H|L2|C|S|Timestamp(4bytes)|Label|ǩ������ L2: ֮��ı��ĳ���(�������Ʊ�־C�������ֽ�S����ȫ��ǩLabel��ǩ�����ݵĳ���)
// Parameter: unsigned char * inBuf ԭʼ����
// Parameter: int inL ԭʼ�����ܳ���
// Parameter: unsigned char * outBuf ����ǩ����ı���
// Parameter: int & outL ����Ϊout�������Ĵ�С�����ǩ������������ĵĴ�С
// Return:    0���ɹ� ������ʧ��
//************************************
int CEccPrivateKey::AssembleSignature(const unsigned char * inBuf, int inL, unsigned char * outBuf, int & outL)
{
	using namespace std;

	// �������
	if (!ValidKey())
	{
		ostringstream ostr;
		ostr<<"˽����Կδ��װ��"<<endl;
		cerr<<ostr.str();

		return -1;
	}

	if (!inBuf || !outBuf)
	{
		ostringstream ostr;
		ostr<<"����������������ָ��Ƿ�"<<endl;
		cerr<<ostr.str();

		return -1;
	}

	if (outL - inL < getKeyLength())
	{
		ostringstream ostr;
		ostr<<"�����������С outL = "<<outL<<" inL = "<<inL<<",Need (outL - inL) >= "<<getKeyLength()<<endl;
		cerr<<ostr.str();

		return -1;
	}

	// ��ʼ
	if (outBuf != inBuf)
	{
		memcpy(outBuf,inBuf,inL);
	}

	int count = inL;
	outBuf[count++] = DI_SIGN_START_TAG; // �������ݿ�ʼ��־
	outBuf[count++] = 0;    // ���ȣ�����0
	outBuf[count++] = 0;    // C��־����ʱ��0
	outBuf[count++] = 0;    // S��־����ʱ��0
	count += ValToBuf(&outBuf[count],htonl((u_long)time(NULL)),4); // ʱ���
	memset(&outBuf[count],0,16);
	count += 16; // ��ȫ��ǩ��������

	MD5_CTX md5;
	unsigned char hashed[16];
	MD5_Init(&md5);
	MD5_Update(&md5,outBuf,count);
	MD5_Final(hashed,&md5);

	cout<<"Prv hash buf:";
	for (int i=0;i<count;i++)
	{
		cout<<std::hex<<std::uppercase<<std::setw(2)<<std::setfill('0')<<(short)outBuf[i];
	}
	cout<<endl;

	cout<<"Prv hash val:";
	for (int i=0;i<16;i++)
	{
		cout<<std::hex<<std::uppercase<<std::setw(2)<<std::setfill('0')<<(short)hashed[i];
	}
	cout<<endl;

	unsigned int sig_len;
	int ret = Sign(hashed,sizeof(hashed),&outBuf[count],&sig_len);
	if (ret) 
	{
		ostringstream ostr;
		ostr<<"ǩ��ʧ��"<<endl;
		cerr<<ostr.str();

		return -1;
	}

	cout<<"Prv sign val:";
	for (int i=0;i<(int)sig_len;i++)
	{
		cout<<std::hex<<std::uppercase<<std::setw(2)<<std::setfill('0')<<(short)outBuf[count + i];
	}
	cout<<endl;

	int AddFrameLength = count - inL + sig_len;
	outBuf[inL + 1] = AddFrameLength - 2;
	outL = inL + AddFrameLength;

	return 0;
}

};//namespace DigitalSignature

