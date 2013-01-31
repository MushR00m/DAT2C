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
#include "PrivateKey.h"
#include "../PublicSupport/Dat2cPublicAPI.h"

namespace DigitalSignature {

const unsigned char DI_SIGN_START_TAG = 0x16;

CPrivateKey::CPrivateKey(void)
{
}

CPrivateKey::~CPrivateKey(void)
{
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
int CPrivateKey::AssembleSignature(const unsigned char * inBuf, int inL, unsigned char * outBuf, int & outL)
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

}; //namespace DigitalSignature

