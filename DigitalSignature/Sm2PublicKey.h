#pragma once
#include <boost/scoped_ptr.hpp>
#include "publickey.h"
#include "sm2.h"
#include "../PublicSupport/Dat2cTypeDef.h"

namespace Protocol
{
	class CProtocol;
}

namespace DigitalSignature {

typedef unsigned int typeAddr;

class CEvpKey;

class CSm2PublicKey :
	public CPublicKey
{
public:
	CSm2PublicKey(std::string keyPath,Protocol::CProtocol & pl);
	virtual ~CSm2PublicKey(void);

	virtual bool ValidKey();
	virtual int getKeyLength();

	virtual int ParseSignature(unsigned char * inBuf, int inL, int dsIndex,unsigned char * outBuf, int & outL); //����ǩ��
	virtual int CalcSecretDataLength(unsigned char * buf,bool bCalcByFrame);	

	int AssembleCheckKeyCon(unsigned char * dstBuf, bool bConAct);
	int AssembleUpdateKeyCon(unsigned char * dstBuf, bool bConAct);

private:
	virtual int Authentic(unsigned char *dgst, int dgstlen,unsigned char *sig, int siglen);                     //��֤

	void AddNegativeCon(unsigned char funCode_C,share_commpoint_ptr pritstationPtr);

	int encrypt(unsigned char * src,int srcLength,unsigned char * dst);                           //����
	int FreeKey();

	int LoadKey(std::string keyPath);
	void SaveKey();

	int setBnXY(const unsigned char * bn_x,int xL,const unsigned char * bn_y,int yL,bool bSave = false);
	int InitBnXY(std::string BigNumX,std::string BigNumY);
	int setBigNumX(const unsigned char * src,int length);
	int setBigNumY(const unsigned char * src,int length);

	int LoadBinCfg(std::string filename);
	int LoadTxtCfg(std::string filename);
	int LoadXmlCfg(std::string filename);
	void SaveXmlCfg(std::string filename);

	int setRandom(unsigned char * val);
	int setRandom();

	typeAddr getAddr();
	int setAddr(typeAddr val);

	int getKeyIndex();
	int setKeyIndex(int val);
	
	int getSm2KeyBits();
	int setSm2KeyBits(int val);

private:
	enum
	{
		max_key_num = 4,
		random_num = 4,
		random_length = 8 * random_num,
	};

	boost::scoped_ptr<CEvpKey> evpKey_;      //�ԳƼ��ܴ�����
	
	std::string keyPath_;                    //�ǶԳ���Կ�ļ���
	ECCrefPublicKey sm2Key_;                 //�ǶԳƼ�����Կ
	unsigned char keyIndex_;                 //�ǶԳ���Կ���
	std::string id_;                         //�ǶԳ���ԿID
	bool validKey_;                          //�ǶԳ���Կ�ĺϷ����б�־�����sm2�Ŀ��Ȼû������ṩ�����Կ�Ϸ��Ե�API (�t_�s)#
	unsigned char pucRandom_[random_num + 1];//����˽Կǩ���˷������������

	Protocol::CProtocol & pl_;               //��Ҳ���������ģ������Կ���͸�����Ƶ���ϵ�Ǳ�̬��
	typeAddr addr_;                          //�ն˵�ַ
};

};//namespace DigitalSignature

