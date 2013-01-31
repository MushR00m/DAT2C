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
#include "EccPublicKey.h"

namespace DigitalSignature {

const char rnd_seed[] = "string to make the random number generator think it has entropy";
const unsigned char DI_SIGN_START_TAG = 0x16;
const unsigned char HEAD_FRAME_LENGTH = 2;

using namespace std;

CEccPublicKey::CEccPublicKey(std::string keypath)
{
	key_ = NULL;

	LoadKey(keypath);
}

CEccPublicKey::~CEccPublicKey(void)
{
	FreeKey();
}

int CEccPublicKey::getKeyLength()
{
	if (key_)
	{
		return EVP_PKEY_size(key_);
	}

	return -1;
}

//װ�ع�Կ
//************************************
// Method:    LoadKey
// FullName:  CEccPublicKey::LoadKey
// Access:    private 
// Returns:   int
// Qualifier:
// Parameter: std::string keypath ECC֤���ļ�·��
// Return:    0���ɹ� ������ʧ��
//************************************
int CEccPublicKey::LoadKey(std::string keypath)
{
	RAND_seed(rnd_seed, sizeof rnd_seed); /* or OAEP may fail */
	OpenSSL_add_all_algorithms();
	OpenSSL_add_all_ciphers();
	OpenSSL_add_all_digests();
	ERR_load_crypto_strings();

	// ����ecc֤��
	BIO * bio = BIO_new_file(keypath.c_str(),"rb");
	if (!bio)
	{
		ostringstream ostr;
		ostr<<"LoadKey: "<<ERR_error_string(ERR_get_error(),NULL)<<endl;
		cerr<<ostr.str();
		return -1;
	}

	X509 * cert = PEM_read_bio_X509(bio,NULL,NULL,NULL);
	if (!cert) 
	{
		ostringstream ostr;
		ostr<<"LoadKey: "<<ERR_error_string(ERR_get_error(),NULL)<<endl;
		cerr<<ostr.str();
		return -1;
	}

	key_ = X509_get_pubkey(cert);
	if (!key_)
	{
		ostringstream ostr;
		ostr<<"LoadKey: "<<ERR_error_string(ERR_get_error(),NULL)<<endl;
		cerr<<ostr.str();
		return -1;
	}

	//cout<<key_->pkey.ec<<endl;

	X509_free(cert);
	BIO_free(bio);

	return 0;
}

//************************************
// Method:    FreeKey
// FullName:  CEccPublicKey::FreeKey
// Access:    private 
// Returns:   int
// Qualifier: �ͷ��ڴ�
// Return:    0���ɹ� ������ʧ��
//************************************
int CEccPublicKey::FreeKey()
{
	if (key_)
	{
		EVP_PKEY_free(key_);
	}

	key_ = NULL;

	ERR_free_strings();

	return 0;
}

bool CEccPublicKey::ValidKey()
{
	if (key_)
	{
		return true;
	}

	return false;
}

//************************************
// Method:    Authentic
// FullName:  Authentic
// Access:    public 
// Returns:   int
// Qualifier: 
// Parameter: const unsigned char * dgst ǩ����������
// Parameter: int dgstlen ǩ���������ݳ���
// Parameter: const unsigned char * sig ǩ��
// Parameter: int siglen ǩ������
// Return:    0����֤��ȷ ������ʧ��
//************************************
int CEccPublicKey::Authentic(unsigned char *dgst, int dgstlen,unsigned char *sig, int siglen)
{
	RAND_seed(rnd_seed, sizeof rnd_seed); /* or OAEP may fail */

	if (!key_)
	{
		ostringstream ostr;
		ostr<<"can't load key"<<endl;
		cerr<<ostr.str();

		return -1;
	}

	int ret = ECDSA_verify(0,dgst,dgstlen,sig,siglen,key_->pkey.ec);
	if (ret != 1) 
	{
		ostringstream ostr;
		ostr<<"ECDSA_verify: "<<ERR_error_string(ERR_get_error(),NULL)<<endl;
		cerr<<ostr.str();

		return -1;
	}

	return 0;
}

//************************************
// Method:    ParseSignature
// FullName:  Algorithm::CDigitalSignature::ParseSignature
// Access:    public 
// Returns:   int
// Qualifier: /*����ǩ�� */
// Parameter: const unsigned char * inBuf ����ǩ���ı���
// Parameter: int inL ����ǩ���ı����ܳ���
// Parameter: unsigned char * outBuf ����ԭʼ�ı���
// Parameter: int & outL ����Ϊout�������Ĵ�С�����ԭʼ�ı��ĵĴ�С
// Return:    0���ɹ� ������ʧ��
//************************************
int CEccPublicKey::ParseSignature(unsigned char * inBuf, int inL, int dsIndex,unsigned char * outBuf, int & outL) //����ǩ��
{
	using namespace std;

	// �������
	if (!ValidKey())
	{
		ostringstream ostr;
		ostr<<"������Կδ��װ��"<<endl;
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

	if (dsIndex < 0 || dsIndex >= inL)
	{
		ostringstream ostr;
		ostr<<"����ǩ���ı���λ�÷Ƿ�"<<endl;
		cerr<<ostr.str();
		return -1;
	}

	// ���ǩ���ı��ĵ�������
	int count = dsIndex;
	if (inBuf[count++] != DI_SIGN_START_TAG) //��鿪ʼ��־
	{
		ostringstream ostr;
		ostr<<"����ǩ���Ŀ�ʼ��־�Ƿ�"<<endl;
		cerr<<ostr.str();
		return -1;
	}

	unsigned char dsL = inBuf[count++];
	unsigned char dsLIndex = count - 1;
	inBuf[count - 1] = 0;
	if(dsIndex + dsL + 2 > inL) // ��鳤��
	{
		ostringstream ostr;
		ostr<<"����ǩ���ĳ��ȷǷ�"<<endl;
		cerr<<ostr.str();
		return -1;
	}

	// �����Ʊ�־������
	unsigned char Contrl_C = inBuf[count++];
	unsigned char Contrl_S = inBuf[count++];

	//���ʱ���
	time_t send_time = ntohl(BufToVal(&inBuf[count],4));
	count += 4;

	// ����طŹ���(���ϵͳ��ʱ������ô�죿)
	time_t now = time(NULL);
	if ((send_time < last_send_time_)||((now-last_local_time_) - (send_time-last_send_time_) > vaild_time_diff))
	{
		ostringstream ostr;
		ostr<<"����ǩ����ʱ����Ƿ�"<<"send_time = "<<send_time<<" last_send_time = "<<last_send_time_<<" now = "<<now<<" last_local_time = "<<last_local_time_<<endl;
		cerr<<ostr.str();
		return -1;
	}
	last_send_time_ = send_time;
	last_local_time_ = now;

	// ��鰲ȫ��ǩ������
	count += 16;

	// ��ǩ
	MD5_CTX md5;
	unsigned char hashed[16];
	MD5_Init(&md5);
	MD5_Update(&md5,inBuf,count);
	MD5_Final(hashed,&md5);

	cout<<"Pub hash buf:";
	for (int i=0;i<count;i++)
	{
		cout<<std::hex<<std::uppercase<<std::setw(2)<<std::setfill('0')<<(short)inBuf[i];
	}
	cout<<endl;

	cout<<"Pub hash val:";
	for (int i=0;i<16;i++)
	{
		cout<<std::hex<<std::uppercase<<std::setw(2)<<std::setfill('0')<<(short)hashed[i];
	}
	cout<<endl;

	int sing_len = (dsL + 2) - (count - dsIndex);
	cout<<"Pub sign val:";
	for (int i=0;i<sing_len;i++)
	{
		cout<<std::hex<<std::uppercase<<std::setw(2)<<std::setfill('0')<<(short)inBuf[count + i];
	}
	cout<<endl;

	int ret = Authentic(hashed,sizeof(hashed),&inBuf[count],sing_len);
	if (ret) 
	{
		ostringstream ostr;
		ostr<<"Authentic fail"<<endl;
		cerr<<ostr.str();

		return -1;
	}

	if (inBuf != outBuf)
	{
		memcpy(outBuf,inBuf,dsIndex);
	}

	outL = dsIndex;
	
	ostringstream ostr;
	ostr<<"Authentic OK!"<<endl;
	cerr<<ostr.str();

	inBuf[dsLIndex] = dsL;

	return 0;
}

//************************************
// Method:    CalcSecretDataLength
// FullName:  DigitalSignature::CPublicKey::CalcSecretDataLength
// Access:    virtual public 
// Returns:   int
// Qualifier:
// Parameter: unsigned char * buf
//************************************
int CEccPublicKey::CalcSecretDataLength(unsigned char * buf,bool bCalcByFrame)
{
	size_t count = 0;

	count += HEAD_FRAME_LENGTH;
	count += buf[1];

	return count;
}

};//namespace DigitalSignature 
