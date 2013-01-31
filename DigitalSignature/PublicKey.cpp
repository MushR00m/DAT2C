#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstring>

#if defined(__linux__)
#include <netinet/in.h>
#endif

#if defined(_WIN32)
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"libeay32.lib")
#endif

#include <openssl/md5.h>
#include "PublicKey.h"
#include "../PublicSupport/Dat2cPublicAPI.h"

namespace DigitalSignature {

const unsigned char DI_SIGN_START_TAG = 0x16;
const unsigned char HEAD_FRAME_LENGTH = 2;

CPublicKey::CPublicKey(void)
{
	last_send_time_ = 0;
	last_local_time_ = 0;
}

CPublicKey::~CPublicKey(void)
{
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
int CPublicKey::ParseSignature(unsigned char * inBuf, int inL, int dsIndex,unsigned char * outBuf, int & outL) //����ǩ��
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
int CPublicKey::CalcSecretDataLength( unsigned char * buf,bool bCalcByFrame )
{
	size_t count = 0;

	count += HEAD_FRAME_LENGTH;
	count += buf[1];

	return count;
}

}; //namespace DigitalSignature
