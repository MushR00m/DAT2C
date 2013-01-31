#include <boost/bind.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/scoped_array.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/exception/all.hpp>

#include "S101_518.h"
#include "BF533_DataBase.h"
#include "../PublicSupport/Dat2cPublicAPI.h"
#include "../FileSystem/Markup.h"
#include "../DataBase/PriStation.h"
#include "../DataBase/YkPoint.h"
#include "../DataBase/YxPoint.h"
#include "../DataBase/SoePoint.h"
#include "../DataBase/YcVarPoint.h"

//#include <fstream>

namespace Protocol{

const unsigned int CRC_Code = 0x000A;

const unsigned char DIR_PRM = 0x00;
const std::string strDefaultCfg = "S101_518Cfg.xml";

//���101��Լ��YK������
//const unsigned char DYK_OPEN_NEGATIVE = 0;
//const unsigned char DYK_TYPE_OPEN = 0x01;
//const unsigned char DYK_TYPE_CLOSE = 0x02;
//const unsigned char SYK_TYPE_OPEN = 0;
//const unsigned char SYK_TYPE_CLOSE = 0x01;
//const unsigned char DYK_CLOSE_NEGATIVE = 0x03;

//���ֱ�����Ϣ��Ԫ�ص����������һ������������ֵ��127����Ϣ����ĿԪ����ռ�ֽ���Ϊ1�����)
//const int INFONUM_LIMIT_ALLYXFRAME = 64;
//const int INFONUM_LIMIT_ALLYCFRAME = 48;
//const int INFONUM_LIMIT_ALLDDFRAME = 28;
//const int INFONUM_LIMIT_COSFRAME = 32;
//const int INFONUM_LIMIT_SOEFRAME = 16;
//const int INFONUM_LIMIT_YCVARFRAME = 24;

const unsigned char ACT_FCV = 0x10; //0001 0000
const unsigned char NACK_FCV = 0;
const unsigned char ACT_ACD = 0x20; //0010 0000
const unsigned char NACK_ACD = 0;
const unsigned char ACT_FCB = 0x20; //0010 0000
const unsigned char NACK_FCB = 0;

//���BF533����
const unsigned char M_SG_RE_1 = 0xA6; //Զ���źŸ���
const unsigned char M_FI_RE_3 = 0xA0;  //�ٻ��ļ����ͱ�־
const unsigned char M_FI_SE_3 = 0xA1;  //�����ļ����ͱ�־
const unsigned char M_LV_VR_3 = 0xA2;  //��ֵУ�����ͱ�־
const unsigned char M_VA_VR_3 = 0xA3;  //�ٻ���ֵϵ�����ͱ�־
const unsigned char M_VA_DL_3 = 0xA4;  //��װ��ֵϵ�����ͱ�־
const unsigned char M_ZD_RE_3 = 0xA5;  //װ�ø�λ���ͱ�־
const unsigned char M_SG_RE_3 = 0xA6;  //Զ���źŸ������ͱ�־
const unsigned char M_HA_MO_3 = 0xA7;  //�ٻ�г���������ͱ�־
const unsigned char M_CA_TM_3 = 0xA9;  //�ٻ�ʱ�����ͱ�־
const unsigned char M_BD_RQ_3 = 0xB0;  //�����ѯ���ͱ�־
const unsigned char M_AC_BT_3 = 0xB1;  //������ػ���ͱ�־
const unsigned char M_OV_BT_3 = 0xB2;  //�˳���ػ���ͱ�־
const unsigned char M_DL_PA_3 = 0xB3;  //��װ�����ɹ�
const unsigned char M_RE_VR_3 = 0xB4;  //CPU�汾��ѯ
const unsigned char M_DL_BV_3 = 0xB5;  //��������У��
const unsigned char M_DC_VR_3 = 0xB6;  //ֱ����У�����ͱ�־
const unsigned char M_PM_CA_3 = 0xB7;  //������ǲ�ѯ���ͱ�־
const unsigned char M_CA_CR_3 = 0xB8;  //�ٻ�CRC���ͱ�־


//���101��Լ�Ĵ���ԭ����
//const unsigned char trans_cyc = 0x01;
//const unsigned char trans_back = 0x02;
const unsigned char trans_spont = 0x03; //ͻ��
const unsigned char trans_init = 0x04;  //��ʼ��
const unsigned char trans_req = 0x05;   //���������
const unsigned char trans_act = 0x06;   //����
const unsigned char trans_actcon = 0x07;//����ȷ��
const unsigned char trans_deact = 0x08; //ֹͣ����
const unsigned char trans_deactcon = 0x09;//ֹͣ����ȷ��
const unsigned char trans_actterm = 0x0a; //�������
//const unsigned char trans_retrem = 0x0b;
//const unsigned char trans_retloc = 0x0c;
//const unsigned char trans_file = 0x0d;
const unsigned char trans_all = 0x14;     //��Ӧ���ٻ�
//const unsigned char trans_ykfailed = 0x30;

//���101��Լ�ı������ͱ�ʶ����
//const unsigned char M_SP_NA_1 = 0x01;  //����ʱ��ĵ�����Ϣ
//const unsigned char M_DP_NA_1 = 0x03;  //
////const unsigned char M_BO_NA_1 = 0x07;
//const unsigned char M_ME_NA_1 = 0x09;  //ң��仯��Ӧ֡
////const unsigned char M_ME_NB_1 = 0x0b;
////const unsigned char M_ME_NC_1 = 0x0d;
//const unsigned char M_IT_NA_1 = 0x0f;
////const unsigned char M_PS_NA_1 = 0x14;
//const unsigned char M_ME_ND_1 = 0x15;
//const unsigned char M_SP_TB_1 = 0x1e; //30 S0E
//const unsigned char M_DP_TB_1 = 0x1f;
////const unsigned char M_ME_TD_1 = 0x22;
////const unsigned char M_ME_TE_1 = 0x23;
////const unsigned char M_ME_TF_1 = 0x24;
////const unsigned char M_IT_TB_1 = 0x25;
//const unsigned char M_SC_NA_1 = 0x2d;
//const unsigned char M_DC_NA_1 = 0x2e; //ң�ز���
//const unsigned char M_EI_NA_1 = 0x46; //70 ��վ��ʼ��������Ӧ֡
//const unsigned char M_IC_NA_1 = 0x64; //���ٻ�����֡
//const unsigned char M_CI_NA_1 = 0x65; 
//const unsigned char M_CS_NA_1 = 0x67; //ͬ����ʱ

//���101��Լ�Ĺ����붨��-���ӷ���
//const unsigned char M_CON_NA_3 = 0x00;                 // <0> ��=ȷ��֡     ȷ��
//const unsigned char M_BY_NA_3 = 0x01;                  // <1> ��=ȷ��֡     ��·æ��δ�յ�����
const unsigned char M_AV_NA_3 = 0x08;                  // <8> ��=��Ӧ֡     ��������Ӧ����֡
const unsigned char M_NV_NA_3 = 0x09;                  // <0> ��=��Ӧ֡     �����ٻ�������
//const unsigned char M_LKR_NA_3 = 0x0b;                 // <11> ��=ȷ��֡    ��·״̬

//���101��Լ�Ĺ����붨��-���Ʒ���
//const unsigned char C_RCU_NA_3 = 0x00;                 // <0> ��=����/ȷ��֡   ��λͨ�ŵ�Ԫ��CU��
//const unsigned char C_REQ_NA_3 = 0x03;                 // <3> ��=����/ȷ��֡   ��������
//const unsigned char C_CAL_FI_3 = 0x08;                 // <3> ��=����/ȷ��֡   ��������
//const unsigned char C_NEQ_NA_3 = 0x04;                 // <4> ��=����/�޻ش�֡ �������� 
//const unsigned char C_RFB_NA_3 = 0x07;                 // <7> ��=����/ȷ��֡   ��λ֡����λ��FCB��
//const unsigned char C_PLK_NA_3 = 0x09;
//const unsigned char C_PL1_NA_3 = 0x0a;                 // <10> ��=����/��Ӧ     �ٻ�1���û�����
//const unsigned char C_PL2_NA_3 = 0x0b;                 // <11> ��=����/��Ӧ     �ٻ�2���û�����

//���BF533����
const unsigned char C_SG_RE_3 = 0x03;  //Զ���źŸ��鹦����

//const unsigned char EnableISQ = 0x00;
//const unsigned char DisableISQ = 0x80;

#define FileNameLen        50
#define FileLen     512  //�ٻ��ļ�ʱÿ֡�����������ĳ���

CS101_518::CS101_518(boost::asio::io_service & io_service)
	:CS101(io_service),
	CBF533_CfgFile(io_service)
{
	SynCharNum_ = 4;
	iFrameRepeatSum_ = 1;
	bActiveRepeatFrame_ = true;

	InitFileOpt(); 

	InitDefaultTimer(io_service);

	LoadXmlCfg(strDefaultCfg);
}

CS101_518::~CS101_518(void)
{
	
}

void CS101_518::ProcessSubYkSig(typeCmd cmdType,unsigned char ReturnCode,share_commpoint_ptr point,boost::any indexd)
{
	CS101::ProcessSubYkSig(cmdType,ReturnCode,point,indexd);

	if (!point)
	{
		return;
	}

	if (point->getCommPointType() != PRISTATION_NODE)
	{
		return;
	}

	share_pristation_ptr pristationPtr = boost::dynamic_pointer_cast<DataBase::CPriStation>(point);

	if (!pristationPtr)
	{
		return;
	}

	switch (cmdType)
	{
	case CALL_EQU_PARA_CON:
		{
			AddSendCmdVal(CALL_EQU_PARA_CON,CALL_EQU_PARA_CON_PRIORITY,pristationPtr,indexd);
		}
		break;

	case CALL_PROVAL_CON:
		{
			AddSendCmdVal(CALL_PROVAL_CON,CALL_PROVAL_CON_PRIORITY,pristationPtr,indexd);
		}
		break;
	case CALL_CHTYPE_CON:
		{
			AddSendCmdVal(CALL_CHTYPE_CON,CALL_CHTYPE_CON_PRIORITY,pristationPtr,indexd);
		}
		break;
	case CALL_LINEPARA_CON:
		{
			AddSendCmdVal(CALL_LINEPARA_CON,CALL_LINEPARA_CON_PRIORITY,pristationPtr,indexd);
		}
		break;
	case CALL_INTERFACE_PARA_CON:
		{
			AddSendCmdVal(CALL_INTERFACE_PARA_CON,CALL_INTERFACE_PARA_CON_PRIORITY,pristationPtr,indexd);
		}
		break;

	default:
		break;
	}
}

void CS101_518::ProcessSubAliveSig(typeCmd cmdType,unsigned char ReturnCode,share_commpoint_ptr point,boost::any val)
{
	CProtocol::ProcessSubAliveSig(cmdType,ReturnCode,point,val);

	switch(cmdType)
	{
	case LINE_VAL_VER_QYC:
		for (int i=0;i<getCommPointSum();i++)
		{
			share_commpoint_ptr commPoint = getCommPoint(i).lock();
			if (commPoint)
			{
				if (commPoint->getCommPointType() == PRISTATION_NODE && ReturnCode == RETURN_CODE_ACTIVE)
				{
					AddSendCmdVal(LINE_VAL_VER_QYC,LINE_VAL_VER_QYC_PRIORITY,commPoint,val);
				}
			}
		}
		break;

	case LINE_DCVAL_VER_SUCESS:
		for (int i=0;i<getCommPointSum();i++)
		{
			share_commpoint_ptr commPoint = getCommPoint(i).lock();
			if (commPoint)
			{
				if (commPoint->getCommPointType() == PRISTATION_NODE && ReturnCode == RETURN_CODE_ACTIVE)
				{
					AddSendCmdVal(LINE_DCVAL_VER_SUCESS,LINE_DCVAL_VER_SUCESS_PRIORITY,commPoint,val);
				}
			}
		}
		break;

	case HARMONIC_CON:
		for (int i=0;i<getCommPointSum();i++)
		{
			share_commpoint_ptr commPoint = getCommPoint(i).lock();
			if (commPoint)
			{
				if (commPoint->getCommPointType() == PRISTATION_NODE && ReturnCode == RETURN_CODE_ACTIVE)
				{
					AddSendCmdVal(HARMONIC_CON,HARMONIC_CON_PRIORITY,commPoint,val);
				}
			}
		}
		break;

	case CALL_VALCOEF_CON:
		for (int i=0;i<getCommPointSum();i++)
		{
			share_commpoint_ptr commPoint = getCommPoint(i).lock();
			if (commPoint)
			{
				if (commPoint->getCommPointType() == PRISTATION_NODE && ReturnCode == RETURN_CODE_ACTIVE)
				{
					AddSendCmdVal(CALL_VALCOEF_CON,CALL_VALCOEF_CON_PRIORITY,commPoint,val);
				}
			}
		}
		break;

	case BOARD_REQ_CON:
		for (int i=0;i<getCommPointSum();i++)
		{
			share_commpoint_ptr commPoint = getCommPoint(i).lock();
			if (commPoint)
			{
				if (commPoint->getCommPointType() == PRISTATION_NODE && ReturnCode == RETURN_CODE_ACTIVE)
				{
					AddSendCmdVal(BOARD_REQ_CON,CALL_VALCOEF_CON_PRIORITY,commPoint,val);
				}
			}
		}
		break;

	case SIGNAL_RESET_CON:
		for (int i=0;i<getCommPointSum();i++)
		{
			share_commpoint_ptr commPoint = getCommPoint(i).lock();
			if (commPoint)
			{
				if (commPoint->getCommPointType() == PRISTATION_NODE && ReturnCode == RETURN_CODE_ACTIVE)
				{
					AddSendCmdVal(SIGNAL_RESET_CON,SIGNAL_RESET_CON_PRIORITY,commPoint,val);
				}
			}
		}
		break;

	case BATTERY_ACTIVE_CON:
		for (int i=0;i<getCommPointSum();i++)
		{
			share_commpoint_ptr commPoint = getCommPoint(i).lock();
			if (commPoint)
			{
				if (commPoint->getCommPointType() == PRISTATION_NODE && ReturnCode == RETURN_CODE_ACTIVE)
				{
					AddSendCmdVal(BATTERY_ACTIVE_CON,BATTERY_ACTIVE_CON_PRIORITY,commPoint,val);
				}
			}
		}
		break;

	case BATTERY_ACTIVE_OVER_CON:
		for (int i=0;i<getCommPointSum();i++)
		{
			share_commpoint_ptr commPoint = getCommPoint(i).lock();
			if (commPoint)
			{
				if (commPoint->getCommPointType() == PRISTATION_NODE && ReturnCode == RETURN_CODE_ACTIVE)
				{
					AddSendCmdVal(BATTERY_ACTIVE_OVER_CON,BATTERY_ACTIVE_OVER_CON_PRIORITY,commPoint,val);
				}
			}
		}
		break;

	case LINE_BVAL_VER_QYC:
		for (int i=0;i<getCommPointSum();i++)
		{
			share_commpoint_ptr commPoint = getCommPoint(i).lock();
			if (commPoint)
			{
				if (commPoint->getCommPointType() == PRISTATION_NODE && ReturnCode == RETURN_CODE_ACTIVE)
				{
					AddSendCmdVal(LINE_BVAL_VER_QYC,LINE_BVAL_VER_QYC_PRIORITY,commPoint,val);
				}
			}
		}
		break;

	case LINE_BVAL_VER_SUCESS:
		for (int i=0;i<getCommPointSum();i++)
		{
			share_commpoint_ptr commPoint = getCommPoint(i).lock();
			if (commPoint)
			{
				if (commPoint->getCommPointType() == PRISTATION_NODE && ReturnCode == RETURN_CODE_ACTIVE)
				{
					AddSendCmdVal(LINE_BVAL_VER_SUCESS,LINE_BVAL_VER_SUCESS_PRIORITY,commPoint,val);
				}
			}
		}
		break;

	case DOWNLOAD_PARA_CON:
		for (int i=0;i<getCommPointSum();i++)
		{
			share_commpoint_ptr commPoint = getCommPoint(i).lock();
			if (commPoint)
			{
				if (commPoint->getCommPointType() == PRISTATION_NODE && ReturnCode == RETURN_CODE_ACTIVE)
				{
					AddSendCmdVal(DOWNLOAD_PARA_CON,DOWNLOAD_PARA_CON_PRIORITY,commPoint,val);
				}
			}
		}
		break;

	case CALL_PM_ANG_CON:
		for (int i=0;i<getCommPointSum();i++)
		{
			share_commpoint_ptr commPoint = getCommPoint(i).lock();
			if (commPoint)
			{
				if (commPoint->getCommPointType() == PRISTATION_NODE && ReturnCode == RETURN_CODE_ACTIVE)
				{
					AddSendCmdVal(CALL_PM_ANG_CON,CALL_PM_ANG_CON_PRIORITY,commPoint,val);
				}
			}
		}
		break;

	case DSP_VERSION_INQ_CON:
		for (int i=0;i<getCommPointSum();i++)
		{
			share_commpoint_ptr commPoint = getCommPoint(i).lock();
			if (commPoint)
			{
				if (commPoint->getCommPointType() == PRISTATION_NODE && ReturnCode == RETURN_CODE_ACTIVE)
				{
					AddSendCmdVal(DSP_VERSION_INQ_CON,DSP_VERSION_INQ_CON_PRIORITY,commPoint,val);
				}
			}
		}
		break;

	case CALL_INTERFACE_PARA_CON:
		for (int i=0;i<getCommPointSum();i++)
		{
			share_commpoint_ptr commPoint = getCommPoint(i).lock();
			if (commPoint)
			{
				if (commPoint->getCommPointType() == PRISTATION_NODE && ReturnCode == RETURN_CODE_ACTIVE)
				{
					AddSendCmdVal(CALL_INTERFACE_PARA_CON,CALL_INTERFACE_PARA_CON_PRIORITY,commPoint,val);
				}
			}
		}
		break;
	}
}

int CS101_518::LoadXmlCfg(std::string filename)
{
	FileSystem::CMarkup xml;

	if (!xml.Load(filename))
	{
		return -1;
	}

	xml.ResetMainPos();
	xml.FindElem();  //root strProtocolRoot
	xml.IntoElem();  //enter strProtocolRoot

	CProtocol::LoadXmlCfg(xml);

	xml.OutOfElem(); //out strProtocolRoot

	return 0;
}

void CS101_518::SaveXmlCfg(std::string filename)
{
	FileSystem::CMarkup xml;
	xml.SetDoc(strXmlHead);
	xml.SetDoc(strProtocolXsl);

	xml.AddElem(strProtocolRoot);
	xml.IntoElem();

	CProtocol::SaveXmlCfg(xml);

	xml.OutOfElem();

	xml.Save(filename);
}

int CS101_518::CheckFrameHead(unsigned char * buf,size_t & exceptedBytes)
{
	if((buf[0] == 0x88) && (buf[3] == 0x88))//�����ļ�����
	{
		exceptedBytes = BufToVal(&buf[1],2)  + 6;

		return 0;
	}

	return CS101::CheckFrameHead(buf,exceptedBytes);
}

int CS101_518::CheckFrameTail(unsigned char * buf,size_t exceptedBytes)
{
	if ((exceptedBytes > 5) && (buf[0] == 0x88) && (buf[3] == 0x88)) //������
	{
		size_t sum = CalcCheckSumByte(&buf[4],exceptedBytes - 6); //����У���

		if ((buf[exceptedBytes -1] == 0x16) && (buf[exceptedBytes - 2] == sum))//��鱨��β��ȷ
		{
			return 0;
		}
	}

	return CS101::CheckFrameTail(buf,exceptedBytes);
}

int CS101_518::ParseLongFrame(unsigned char * buf, share_pristation_ptr pristationPtr,size_t exceptedBytes)
{
	unsigned char FCV = buf[4] & ACT_FCV;
	unsigned char FCB = buf[4] & ACT_FCB;

	size_t FrameType = BufToVal(&buf[FrameTypeLocation_],FrameTypeLength_);        //���ͱ�ʶ
	size_t TransReason = BufToVal(&buf[TransReasonLocation_],TransReasonLength_);  //����ԭ��
	unsigned char Data_Code = buf[DataLocation_] & 0x80;

	switch (FrameType)
	{
	case M_LV_VR_3://��ֵУ��
		{
			if (CheckFCB(FCV,FCB,DOWNLOAD_LINE_VAL,pristationPtr))
			{
				std::cout<<"���FCVλ����"<<std::endl;
				return 0;
			}
			ParseLineValVer(buf,pristationPtr);
		}
		break;

	case M_DC_VR_3://ֱ������ֵУ��
		{
			if (CheckFCB(FCV,FCB,LINE_DCVAL_VER,pristationPtr))
			{
				std::cout<<"���FCVλ����"<<std::endl;
				return 0;
			}
			ParseDcValVer(buf,pristationPtr);
		}
		break;

	case M_HA_MO_3://�ٻ�г��
		{

			if (CheckFCB(FCV,FCB,HARMONIC_ACK,pristationPtr))
			{
				std::cout<<"���FCVλ����"<<std::endl;
				return 0;
			}
			ParseHarmonicAck(buf,pristationPtr);
		}
		break;

	case M_VA_VR_3://�ٻ���ֵϵ�� 0xA2
		{
			AddStatusLogWithSynT("�յ��ٻ���ֵϵ��ָ��\n");
			if (CheckFCB(FCV,FCB,CALL_VALCOEF_ACK,pristationPtr))
			{
				std::cout<<"���FCVλ����"<<std::endl;
				return 0;
			}
			ParseCallVallCoef(buf,pristationPtr);
		}
		break;

	case M_ZD_RE_3://װ�ø�λ
		{
			//         std::cout<<"S101װ�ø�λָ��... "<<std::endl; 
			if (CheckFCB(FCV,FCB,REBOOT_ACK,pristationPtr))
			{
				std::cout<<"���FCVλ����"<<std::endl;
				return 0;
			}
			ParseRbootAck(buf,pristationPtr);
		}
		break;

	case M_SG_RE_3://Զ���źŸ���
		{
			//	     std::cout<<"S101Զ���źŸ���ָ��... "<<std::endl; 
			if (CheckFCB(FCV,FCB,SIGNAL_RESET,pristationPtr))
			{
				std::cout<<"���FCVλ����"<<std::endl;
				return 0;
			}
             ParseRemoteSingalReset(buf,pristationPtr);
			//CCmd cmd(SIGNAL_RESET,SIGNAL_RESET_PRIORITY,pristationPtr,255);//255 reset all
			//pristationPtr->AddBF533Cmd(0,cmd);//���ָ�����,ValΪ�ṹ��

			//ConnectSubYkSig(pristationPtr);//����Ϣ��������
		}
		break;

	case M_CA_TM_3://ʱ��У��
		{
			if (CheckFCB(FCV,FCB,CALL_TIME_ACK,pristationPtr))
			{
				std::cout<<"���FCVλ����"<<std::endl;
				return 0;
			}
			AddSendCmdVal(CALL_TIME_CON,CALL_TIME_CON_PRIORITY,pristationPtr);
		}
		break;

	case M_VA_DL_3://��װ��ֵϵ��
		{
			if (CheckFCB(FCV,FCB,DOWNLOAD_LINE_COEF,pristationPtr))
			{
				std::cout<<"���FCVλ����"<<std::endl;
				return 0;
			}
			ParseDownLoadLineHVal(buf,pristationPtr);//��װ��ֵϵ��
		}
		break;

	case M_BD_RQ_3://�����ѯ
		{
			if (CheckFCB(FCV,FCB,BOARD_REQ_ACK,pristationPtr))
			{
				std::cout<<"���FCVλ����"<<std::endl;
				return 0;
			}
			CCmd cmd(BOARD_REQ_ACK,BOARD_REQ_ACK_PRIORITY,pristationPtr);
			pristationPtr->AddBF533Cmd(0,cmd);//���ָ�����,ValΪ�ṹ��

			//ConnectSubYkSig(pristationPtr);//����Ϣ��������
		}
		break;

	case M_AC_BT_3://��ػ
		{
			if (CheckFCB(FCV,FCB,BOARD_REQ_ACK,pristationPtr))
			{
				std::cout<<"���FCVλ����"<<std::endl;
				return 0;
			}

			CCmd cmd(BATTERY_ACTIVE,BATTERY_ACTIVE_PRIORITY,pristationPtr);
			pristationPtr->AddBF533Cmd(0,cmd);//���ָ�����,ValΪ�ṹ��

			//ConnectSubYkSig(pristationPtr);//����Ϣ��������
		}
		break;

	case M_OV_BT_3://��ػ����
		{
			if (CheckFCB(FCV,FCB,BATTERY_ACTIVE_OVER,pristationPtr))
			{
				std::cout<<"���FCVλ����"<<std::endl;
				return 0;
			}
			CCmd cmd(BATTERY_ACTIVE_OVER,BATTERY_ACTIVE_OVER_PRIORITY,pristationPtr);
			pristationPtr->AddBF533Cmd(0,cmd);//���ָ�����,ValΪ�ṹ��

			//ConnectSubYkSig(pristationPtr);//����Ϣ��������
		}
		break;

	case M_RE_VR_3:  //0xB4
		{
			if (CheckFCB(FCV,FCB,BATTERY_ACTIVE_OVER,pristationPtr))
			{
				std::cout<<"���FCVλ����"<<std::endl;
				return 0;
			}
			ParseCpuVerReq(buf,pristationPtr);
		}
		break;

	case M_DL_BV_3://0xB5 ��������У�鶨ֵ��װ
		{
			if (CheckFCB(FCV,FCB,DOWNLOAD_LINE_BVAL,pristationPtr))
			{
				std::cout<<"���FCVλ����"<<std::endl;
				return 0;
			}
			ParseBValVer(buf,pristationPtr);
		}
		break;

	case M_PM_CA_3://0xB7 �ٻ��������
		{
			if (CheckFCB(FCV,FCB,DOWNLOAD_LINE_BVAL,pristationPtr))
			{
				std::cout<<"���FCVλ����"<<std::endl;
				return 0;
			}
			ParseCallPM(buf,pristationPtr);
		}
		break;

	case M_CA_CR_3://0xB8 �ٻ�CRC
		{
			if (CheckFCB(FCV,FCB,DOWNLOAD_LINE_BVAL,pristationPtr))
			{
				std::cout<<"���FCVλ����"<<std::endl;
				return 0;
			}
			ParseCallCRC(buf,pristationPtr);
		}
		break;

	default:
		{
			return CS101::ParseLongFrame(buf,pristationPtr,exceptedBytes);
		}
		break;
	}

	return 0;
}

int CS101_518::ParseFileFrame(unsigned char * buf, share_pristation_ptr pristationPtr)
{
	unsigned char FCV = buf[4] & ACT_FCV;
	unsigned char FCB = buf[4] & ACT_FCB;
	size_t FrameType = BufToVal(&buf[FrameTypeLocation_],FrameTypeLength_);        //���ͱ�ʶ
	size_t TransReason = BufToVal(&buf[TransReasonLocation_],TransReasonLength_);  //����ԭ��
	unsigned char Data_Code = buf[DataLocation_] & 0x80;

	switch (FrameType)
	{
	case M_FI_SE_3:  //�����ļ�
		{
			if(TransReason == trans_act)//�����ļ����� 0x06
			{
				if (CheckFCB(FCV,FCB,SEND_FILENAME_ACK,pristationPtr))
				{
					return 0;
				}
				//		   std::cout<<"�����ļ���... .."<<std::endl; 
				ParseSendFileAck(buf,pristationPtr);
			}

			else if(TransReason == trans_deact)//����ԭ��Ϊ0x08
			{
				if (CheckFCB(FCV,FCB,SEND_FILENAME_ACK,pristationPtr))
				{
					return 0;
				}
				//		  std::cout<<"�����ļ���... ..."<<std::endl; 
				ParseRecvFile(buf,pristationPtr);
			}
		}
		break;

	case M_FI_RE_3:
		{
			if(TransReason == trans_act)//�ٻ��ļ����� 0x06
			{
				if (CheckFCB(FCV,FCB,SEND_FILENAME_ACK,pristationPtr))
				{
					return 0;
				}

				AddStatusLogWithSynT("�����ٻ��ļ�����ָ��.. ...\n");
				ParseCallFileAck(buf,pristationPtr);
			}
		}
		break;


	default :
		{
			std::ostringstream ostr;
			ostr<<"���ձ��Ĵ���δ����ı������� FRAME_TYPE ="<<FrameType<<std::endl;
			AddStatusLogWithSynT(ostr.str());
		}
		break;
	}
	return 0;
}

int CS101_518::ParseFrameBody(unsigned char * buf,size_t exceptedBytes)
{
	int ret = 0;

	unsigned char funcType = buf[0];

	int Addr = getAddrByRecvFrame(buf);
	if (Addr < 0)
	{
		return Addr;
	}

	int pristationIndex = getCommPointIndexByAddrCommType(PRISTATION_NODE,Addr);
	share_pristation_ptr pristationPtr;
	if (pristationIndex >= 0)
	{
		setLastRecvPointIndex(pristationIndex);
		pristationPtr = boost::dynamic_pointer_cast<DataBase::CPriStation>(getCommPoint(pristationIndex).lock());
	}

	if (!pristationPtr)
	{
		//std::ostringstream ostr;
		//ostr<<"S101��Լ���ܸ��ݽ��ձ����еĵ�ַƥ��pristation ptr,��֡���Ľ����ᱻ������"<<std::endl;
		//AddStatusLogWithSynT(ostr.str());

		return -1;
	}

	switch (funcType)
	{
	case 0x10:
		ret = ParseShortFrame(buf,pristationPtr);
		break;

	case 0x68:
		ret = ParseLongFrame(buf,pristationPtr,exceptedBytes);
		break;

	case 0x88:
		ret = ParseFileFrame(buf,pristationPtr);
		break;

	default:
		{
			std::ostringstream ostr;
			ostr<<"δ����ı���ͷ,buf[0] = "<<funcType<<std::endl;
			AddStatusLogWithSynT(ostr.str());
		}
		break;
	}

	if (ret < 0)
	{
		return ret;
	}

	return pristationIndex;
}

int CS101_518::ParseLineValVer(unsigned char * buf,share_pristation_ptr pristationPtr)
{
	int Location = 12;  //��·������λ��

	DataBase::stLine_Val Val;  

	Val.Line_no = BufToVal(&buf[Location ++],1);  //��ȡ��·��

	{
		Val.Flag_Ua = BufToVal(&buf[Location ++],1);
		Val.D_Value_Ua = BufToVal(&buf[Location],2); 
		Location += 2;
		Val.Flag_Ub = BufToVal(&buf[Location ++],1);
		Val.D_Value_Ub = BufToVal(&buf[Location],2); 
		Location += 2;
		Val.Flag_Uc = BufToVal(&buf[Location ++],1);
		Val.D_Value_Uc = BufToVal(&buf[Location],2); 
		Location += 2;
		Val.Flag_CIa = BufToVal(&buf[Location ++],1);
		Val.D_Value_CIa = BufToVal(&buf[Location],2); 
		Location += 2;
		Val.Flag_CIb = BufToVal(&buf[Location ++],1);
		Val.D_Value_CIb = BufToVal(&buf[Location],2); 
		Location += 2;
		Val.Flag_CIc = BufToVal(&buf[Location ++],1);
		Val.D_Value_CIc = BufToVal(&buf[Location],2); 
		Location += 2;
		Val.Flag_BIa = BufToVal(&buf[Location ++],1);
		Val.D_Value_BIa = BufToVal(&buf[Location],2); 
		Location += 2;
		Val.Flag_BIb = BufToVal(&buf[Location ++],1);
		Val.D_Value_BIb = BufToVal(&buf[Location],2); 
		Location += 2;
		Val.Flag_BIc = BufToVal(&buf[Location ++],1);
		Val.D_Value_BIc = BufToVal(&buf[Location],2); 
		Location += 2;
		Val.Flag_U0 = BufToVal(&buf[Location ++],1);
		Val.D_Value_U0 = BufToVal(&buf[Location],2); 
		Location += 2;
		Val.Flag_I0 = BufToVal(&buf[Location ++],1);
		Val.D_Value_I0 = BufToVal(&buf[Location],2); 
		Location += 2;
		Val.Flag_Angle_UaIa = BufToVal(&buf[Location ++],1);
		Val.Angle_UaIa = BufToVal(&buf[Location],2); 
		Location += 2;
		Val.Flag_Angle_UbIb = BufToVal(&buf[Location ++],1);
		Val.Angle_UbIb = BufToVal(&buf[Location],2); 
		Location += 2;
		Val.Flag_Angle_UcIc = BufToVal(&buf[Location ++],1);
		Val.Angle_UcIc = BufToVal(&buf[Location],2); 
	}   

	CCmd cmd(DOWNLOAD_LINE_VAL,DOWNLOAD_LINE_VAL_PRIORITY,pristationPtr,Val);
	pristationPtr->AddBF533Cmd(0,cmd);//���ָ�����,ValΪ�ṹ��

	//ConnectSubYkSig(pristationPtr);//����Ϣ��������
	ResetTimerValVer(pristationPtr,true,0);

	return 0;
}

int CS101_518::ParseHarmonicAck(unsigned char * buf,share_pristation_ptr pristationPtr)
{
	int Location = 12;  //��·������λ��
	DataBase::stHarmonic Val;

	Val.Line_no = BufToVal(&buf[Location ++],1);  //��ȡ��·��
	Val.harmonic_no = BufToVal(&buf[Location ++],1);

	CCmd cmd(HARMONIC_ACK,HARMONIC_ACK_PRIORITY,pristationPtr,Val);
	pristationPtr->AddBF533Cmd(0,cmd);//���ָ�����,ValΪ�ṹ��

	//ConnectSubYkSig(pristationPtr);//����Ϣ��������

	return 0;
}

int CS101_518::ParseSendFileAck(unsigned char * buf,share_pristation_ptr pristationPtr)//�յ��ļ������ĺ�ظ�ȷ��
{
	int Location = 12;   //�ļ�����һ���ֽ�

	char Name[FileNameLen];
	int Namelen_;
	memset(Name,0,FileNameLen);

	Namelen_ = (BufToVal(&buf[1],2) - 11);//�ļ�����ռ���ֽ���
	//memset(NameBuf,0,20);

	//for(int i = 0;i < NameLength_ ;i ++)//�����ļ���
	//{
	//	NameBuf[i] = buf[Location ++];
	//}

	for(int i = 0;i < Namelen_ ;i ++)//�����ļ���
	{
		Name[i] = buf[Location ++];
	}

	std::ostringstream filename;
	filename<<Name;
	FileHandleBegain(filename.str());//��ʼ����CFileHandle������ļ�����

	//FileLength_ = (BufToVal(&buf[Location],3));

	FileHandleSetTotalLength((BufToVal(&buf[Location],3))); //д�����ļ�����

	//std::ostringstream ostr;
	//ostr<<"�ļ����ͳ��ȷֱ�Ϊ��"<< "�ļ�����"<< Name <<",���ȣ�"<< FileLength_<<",�ļ�������NameLength_��"<< Namelen_<<std::endl; 
	//AddStatusLogWithSynT(ostr.str());

	AddSendCmdVal(SEND_FILENAME_CON,SEND_FILENAME_CON_PRIORITY,pristationPtr);
	return 0;
}

int CS101_518::ParseRecvFile(unsigned char * buf,share_pristation_ptr pristationPtr)
{
	int Location = 10; //�ļ���һ���ֽ�
	int val;

	Total_NUM = buf[Location ++];
	This_N0 = buf[Location ++];

	This_Length_ = (BufToVal(&buf[1],2) - 8);

	//����CFileHandle���ļ�������
	FileHandleOutFile(&buf[Location],This_Length_);

	//for(i = 0;i< This_Length_;i ++)
	//{
	//	FileDataBuf[FileDataPtr_ ++] = buf[Location ++];
	//}

	std::ostringstream ostr;
	ostr<<"�յ��ļ�����"<< Total_NUM <<"֡������Ϊ"<< This_N0 << "֡�����ݳ���" << This_Length_ <<std::endl; 
	AddStatusLogWithSynT(ostr.str());

	if(Total_NUM == This_N0)
	{
		val = 15;//0x0F��ʾ���ͳɹ�
		//		std::cout<<FileDataBuf<<std::endl; 
		//SaveFile(pristationPtr);

		//����CFileHandle���ļ�������
		FileHandleWrite();

		AddDownloadCmd(pristationPtr);

		FileHandleFinish();

		//FileDataPtr_ = 0;
	}
	else 
	{
		val = 10; //0x0a��ʾȷ��
	}
	//  std::cout<<"��ʼ���ָ����� ..."<<std::endl; 
	AddSendCmdVal(SEND_FILEBODY_CON,SEND_FILEBODY_CON_PRIORITY,pristationPtr,val);
	return 0;
}

int CS101_518::ParseCallFileAck(unsigned char * buf,share_pristation_ptr pristationPtr)
{
	int Location = 12;   //�ļ�����һ���ֽ�
	char Val;

	char Name[FileNameLen];
	int Namelen_;
	memset(Name,0,FileNameLen);

	Namelen_ = (BufToVal(&buf[1],2) - 8);//�ļ�����ռ���ֽ���
	//memset(NameBuf,0,20);

	for(int i = 0;i < Namelen_ ;i ++)//�����ļ���
	{
		Name[i] = buf[Location ++];
	}

	std::ostringstream filename;
	filename<<Name;

	//����CFileHandle���ļ�������
	FileHandleBegain(filename.str());

	std::ostringstream ostr;
	ostr<<"��Ҫ�ٻ����ļ���Ϊ��"<< Name <<",�ļ�������Ϊ��"<<Namelen_<<std::endl; 
	AddStatusLogWithSynT(ostr.str());

	if((strcmp(BFBasePara,Name)==0)||(strcmp(BFDevSanyaoPara,Name)==0))
	{
		CCmd cmd(CALL_EQU_PARA_ACT,CALL_EQU_PARA_ACT_PRIORITY,pristationPtr);
		pristationPtr->AddBF533Cmd(0,cmd);//���ָ�����,֪ͨ���¹�Լ�ٻ�װ�ò���

		ConnectSubYkSig(pristationPtr);//����Ϣ��������
		ResetTimerCallFileDisConect(pristationPtr,true,0);
	}
	else if(strcmp(BFProtectValPara,Name)==0)//�ٻ�������ֵ
	{
		//   std::cout<<"��ʼ��BF533���ָ�� ... "<<std::endl; 
		CCmd cmd(CALL_PROVAL_ACK,CALL_PROVAL_ACK_PRIORITY,pristationPtr);
		pristationPtr->AddBF533Cmd(0,cmd);//���ָ�����,֪ͨ���¹�Լ�ٻ�װ�ò���

		ConnectSubYkSig(pristationPtr);//����Ϣ��������
		ResetTimerCallFileDisConect(pristationPtr,true,60);
	}
	else if(strcmp(BFChanneltypePara,Name)==0) 
	{
		//   std::cout<<"��ʼ��BF533���ָ�� ... "<<std::endl; 
		CCmd cmd(CALL_CHTYPE_ACK,CALL_CHTYPE_ACK_PRIORITY,pristationPtr);
		pristationPtr->AddBF533Cmd(0,cmd);//���ָ�����,֪ͨ���¹�Լ�ٻ�װ�ò���

		ConnectSubYkSig(pristationPtr);//����Ϣ��������
		ResetTimerCallFileDisConect(pristationPtr,true,0);
	}
	else if(strcmp(BFLinePara,Name)==0) 
	{
		//   std::cout<<"��ʼ��BF533���ָ�� ... "<<std::endl; 
		CCmd cmd(CALL_LINEPARA_ACK,CALL_LINEPARA_ACK_PRIORITY,pristationPtr);
		pristationPtr->AddBF533Cmd(0,cmd);//���ָ�����,֪ͨ���¹�Լ�ٻ�װ�ò���

		ConnectSubYkSig(pristationPtr);//����Ϣ��������
		ResetTimerCallFileDisConect(pristationPtr,true,0);
	}
	else if(strcmp(BFInterfacePara,Name)==0) 
	{
		//   std::cout<<"��ʼ��BF533���ָ�� ... "<<std::endl; 
		CCmd cmd(CALL_INTERFACE_PARA,CALL_INTERFACE_PARA_PRIORITY,pristationPtr);
		pristationPtr->AddBF533Cmd(0,cmd);//���ָ�����,֪ͨ���¹�Լ�ٻ�װ�ò���

		ConnectSubYkSig(pristationPtr);//����Ϣ��������
		ResetTimerCallFileDisConect(pristationPtr,true,0);
	}
	else
	{
		//����CFileHandle���ļ�������
		if (FileHandleRead() > 0)
		{
			Val = 0x07;
			Total_NUM = ((FileHandleGetTotalLength() - 1) / FileLen) + 1;
		}
		else
		{
			Val = 0x05;
		}
		//if(GetFileData(pristationPtr) == 0)
		//{
		//	Val = 0x07;
		//}
		//else
		//{
		//	Val = 0x05;
		//}
		AddSendCmdVal(CALL_FILENAME_CON,CALL_FILENAME_CON_PRIORITY,pristationPtr,Val);//��ӻظ�ָ��
	}

	return 0;
}

int CS101_518::ParseCallVallCoef(unsigned char * buf,share_pristation_ptr pristationPtr)//�ٻ���ֵϵ��
{
	int Location = 12;   //�ļ�����һ���ֽ�
	DataBase::stLine_ValCoef Val;

	Val.Line_no = BufToVal(&buf[Location],1);  //��ȡ��·��

	CCmd cmd(CALL_VALCOEF_ACK,CALL_VALCOEF_ACK_PRIORITY,pristationPtr,Val);
	pristationPtr->AddBF533Cmd(0,cmd);//���ָ�����,ValΪ�ṹ��
	//ConnectSubYkSig(pristationPtr);//����Ϣ��������

	ResetTimerCallHVal(pristationPtr,true,0);
	return 0;

}

int CS101_518::ParseDownLoadLineHVal(unsigned char * buf,share_pristation_ptr pristationPtr)
{
	int Location = 12;   //�ļ�����һ���ֽ�
	DataBase::stLine_ValCoef HVal;

	HVal.Line_no = BufToVal(&buf[Location ++],1);  //��ȡ��·��
	HVal.Flag_Ua = BufToVal(&buf[Location ++],1);
	HVal.H_Value_Ua = BufToVal(&buf[Location ],2);
	Location += 2;
	HVal.Flag_Ub = BufToVal(&buf[Location ++],1);
	HVal.H_Value_Ub = BufToVal(&buf[Location ],2);
	Location += 2;
	HVal.Flag_Uc = BufToVal(&buf[Location ++],1);
	HVal.H_Value_Uc = BufToVal(&buf[Location ],2);
	Location += 2;
	HVal.Flag_CIa = BufToVal(&buf[Location ++],1);
	HVal.H_Value_CIa = BufToVal(&buf[Location ],2);
	Location += 2;
	HVal.Flag_CIb = BufToVal(&buf[Location ++],1);
	HVal.H_Value_CIb = BufToVal(&buf[Location ],2);
	Location += 2;
	HVal.Flag_CIc = BufToVal(&buf[Location ++],1);
	HVal.H_Value_CIc = BufToVal(&buf[Location ],2);
	Location += 2;
	HVal.Flag_BIa = BufToVal(&buf[Location ++],1);
	HVal.H_Value_BIa = BufToVal(&buf[Location ],2);
	Location += 2;
	HVal.Flag_BIb = BufToVal(&buf[Location ++],1);
	HVal.H_Value_BIb = BufToVal(&buf[Location ],2);
	Location += 2;
	HVal.Flag_BIc = BufToVal(&buf[Location ++],1);
	HVal.H_Value_BIc = BufToVal(&buf[Location ],2);
	Location += 2;
	HVal.Flag_U0 = BufToVal(&buf[Location ++],1);
	HVal.H_Value_U0 = BufToVal(&buf[Location ],2);
	Location += 2;
	HVal.Flag_I0 = BufToVal(&buf[Location ++],1);
	HVal.H_Value_I0 = BufToVal(&buf[Location ],2);
	Location += 2;
	HVal.Flag_Angle_UaIa = BufToVal(&buf[Location ++],1);
	HVal.Angle_UaIa = BufToVal(&buf[Location ],2);
	Location += 2;
	HVal.Flag_Angle_UbIb = BufToVal(&buf[Location ++],1);
	HVal.Angle_UbIb = BufToVal(&buf[Location ],2);
	Location += 2;
	HVal.Flag_Angle_UcIc = BufToVal(&buf[Location ++],1);
	HVal.Angle_UcIc = BufToVal(&buf[Location ],2);

	CCmd cmd(DOWNLOAD_LINE_COEF,DOWNLOAD_LINE_COEF_PRIORITY,pristationPtr,HVal);
	pristationPtr->AddBF533Cmd(0,cmd);//���ָ�����,ValΪ�ṹ��
	//ConnectSubYkSig(pristationPtr);//����Ϣ��������
	AddSendCmdVal(DOWNLOAD_LINE_CON,DOWNLOAD_LINE_CON_PRIORITY,pristationPtr,HVal);
	return 0;
}

int CS101_518::ParseRbootAck(unsigned char * buf,share_pristation_ptr pristationPtr)
{
	AddSendCmdVal(REBOOT_CON,REBOOT_CON_PRIORITY,pristationPtr);
	return 0;
}

int CS101_518::ParseCpuVerReq(unsigned char * buf,share_pristation_ptr pristationPtr)
{
	CCmd cmd(DSP_VERSION_INQ,DSP_VERSION_INQ_PRIORITY,pristationPtr,0);
	pristationPtr->AddBF533Cmd(0,cmd);//���ָ�����,ValΪ�ṹ��
	//AddSendCmdVal(DSP_VERSION_INQ_CON,DSP_VERSION_INQ_CON_PRIORITY,pristationPtr);
	return 0;
}

int CS101_518::ParseBValVer(unsigned char * buf,share_pristation_ptr pristationPtr)
{
	int Location = 12;  
	DataBase::stLine_BCurVal Val;
	Val.Line_No = BufToVal(&buf[Location],1);
	Location += 1;
	Val.D_Val_BI1 = BufToVal(&buf[Location],2);
	Location += 2;
	Val.D_Val_BI2 = BufToVal(&buf[Location ],2);
	Location += 2;
	Val.D_Val_BI3 = BufToVal(&buf[Location ],2);

	//std::cout<<"S101�յ��ı���������ֵΪ��"<<"Val.D_Val_BI1:"<<Val.D_Val_BI1<<" Val.D_Val_BI2:"<<Val.D_Val_BI2<<" Val.D_Val_BI3:"<<Val.D_Val_BI3<<std::endl;

	CCmd cmd(DOWNLOAD_LINE_BVAL,DOWNLOAD_LINE_BVAL_PRIORITY,pristationPtr,Val);
	pristationPtr->AddBF533Cmd(0,cmd);//���ָ�����,ValΪ�ṹ��

	//ConnectSubYkSig(pristationPtr);//����Ϣ��������

	ResetTimerBCurVer(pristationPtr,true,0);

	return 0;
}

int CS101_518::ParseDcValVer(unsigned char * buf,share_pristation_ptr pristationPtr)
{
	int Location = 12;  
	DataBase::stDCVerVal Val;
	Val.ChannelNo = BufToVal(&buf[Location],1);
	Location += 1;
	Val.V_Val_DC = BufToVal(&buf[Location],2);

	CCmd cmd(LINE_DCVAL_VER,LINE_DCVAL_VER_PRIORITY,pristationPtr,Val);
	pristationPtr->AddBF533Cmd(0,cmd);//���ָ�����,ValΪ�ṹ��

	//ConnectSubYkSig(pristationPtr);//����Ϣ��������

	ResetTimerDcVer(pristationPtr,true,0);
	return 0;
}

int CS101_518::ParseCallPM(unsigned char * buf,share_pristation_ptr pristationPtr)
{
	int Location = 12;

	DataBase::stPMPara Val;

	int LineNo = BufToVal(&buf[Location],1);

	Val.LineNo = LineNo;

	CCmd cmd(CALL_PM_ANG,CALL_PM_ANG_PRIORITY,pristationPtr,Val);
	pristationPtr->AddBF533Cmd(0,cmd);//���ָ�����,ValΪ�ṹ��

	ResetTimerCallPm(pristationPtr,true,0);


	return 0;
}

int CS101_518::ParseCallCRC(unsigned char * buf,share_pristation_ptr pristationPtr)
{
	AddSendCmdVal(CALL_CRC_CON,CALL_CRC_CON_PRIORITY,pristationPtr);
	return 0;
}

int CS101_518::ParseRemoteSingalReset(unsigned char * buf,share_pristation_ptr pristationPtr)
{
	int Location = 12;
	int LineNo_ = BufToVal(&buf[Location],1);

	CCmd cmd(SIGNAL_RESET,SIGNAL_RESET_PRIORITY,pristationPtr,LineNo_);//255 reset all
	pristationPtr->AddBF533Cmd(0,cmd);//���ָ�����,ValΪ�ṹ��
	return 0;
}

void CS101_518::AddDownloadCmd(share_pristation_ptr pristationPtr)
{
	//char Name[FileNameLen];
	//memset(Name,0,FileNameLen);
	std::string Name;
	Name = FileHandleGetFileName();

	if(strcmp(BFBasePara,Name.c_str())==0)
	{
		////std::cout<<"�յ��ļ�Ϊ��BasePara.xml" <<std::endl; 
		//pristationPtr->AddNormlCmd( 0,DOWNLOAD_EQU_PARA_,0);//�����װװ�ò���ָ�����
		//ConnectSubYkSig(pristationPtr);//����Ϣ��������
		//ResetTimerDowloadPara(pristationPtr,true,0);
	}
	else if(strcmp(BFDevSanyaoPara,Name.c_str())==0)
	{
		//   std::cout<<"�յ��ļ�Ϊ��DevSanyaoPara.xml,��ʼ���ָ��... ..."<<std::endl; 
		CCmd cmd(DOWNLOAD_EQU_PARA,DOWNLOAD_EQU_PARA_PRIORITY,pristationPtr);
		pristationPtr->AddBF533Cmd(0,cmd);//�����װװ�ò���ָ�����
		//ConnectSubYkSig(pristationPtr);//����Ϣ��������
		ResetTimerDowloadPara(pristationPtr,true,0);
	}
	else if(strcmp(BFChanneltypePara,Name.c_str())==0)
	{
		//   std::cout<<"�յ��ļ�Ϊ��ChanneltypePara.xml" <<std::endl;
		CCmd cmd(DOWNLOAD_CHANNEL_PARA,DOWNLOAD_CHANNEL_PARA_PRIORITY,pristationPtr);
		pristationPtr->AddBF533Cmd(0,cmd);
		//ConnectSubYkSig(pristationPtr);//����Ϣ��������
		ResetTimerDowloadPara(pristationPtr,true,0);
	}
	else if(strcmp(BFInterfacePara,Name.c_str())==0)
	{
		//  std::cout<<"�յ��ļ�Ϊ��InterfacePara.xml"<<std::endl; 
		CCmd cmd(SET_BF518_PARA,SET_BF518_PARA_PRIORITY,pristationPtr);
		pristationPtr->AddBF533Cmd(0,cmd);
		//ConnectSubYkSig(pristationPtr);//����Ϣ��������
		//	ResetTimerDowloadPara(pristationPtr,true,0);
	}
	else if(strcmp(BFLinePara,Name.c_str())==0)
	{
		//  std::cout<<"�յ��ļ�Ϊ��LinePara.xml"<<std::endl; 
		CCmd cmd(DOWNLOAD_CHANNEL_COMREL,DOWNLOAD_CHANNEL_COMREL_PRIORITY,pristationPtr);
		pristationPtr->AddBF533Cmd(0,cmd);
		//ConnectSubYkSig(pristationPtr);//����Ϣ��������
		ResetTimerDowloadPara(pristationPtr,true,0);
	}
	else if(strcmp(BFMaster1YcZFTab,Name.c_str())==0)
	{
		//  std::cout<<"�յ��ļ�Ϊ��Master1YcZFTab.xml" <<std::endl; 
	}
	else if(strcmp(BFMaster1YxZFTab,Name.c_str())==0)
	{
		//  std::cout<<"�յ��ļ�Ϊ��Master1YxZFTab.xml"<<std::endl; 
	}
	else if(strcmp(BFMaster2YcZFTab,Name.c_str())==0)
	{
		//std::cout<<"�յ��ļ�Ϊ��Master2YcZFTab.xml" <<std::endl; 
	}
	else if(strcmp(BFMaster2YxZFTab,Name.c_str())==0)
	{
		//  std::cout<<"�յ��ļ�Ϊ��Master2YxZFTab.xml"<<std::endl; 
	}
	else if(strcmp(BFProtectValPara,Name.c_str())==0)
	{
		AddStatusLogWithSynT("�յ��ļ�Ϊ��ProtectValPara.xml\n");
		CCmd cmd(DOWNLOAD_DELAY_VAL,DOWNLOAD_DELAY_VAL_PRIORITY,pristationPtr,0xFF);//0=>0xff ZHANGZHIHUA 20100508 �˴����봫0
		pristationPtr->AddBF533Cmd(0,cmd);
		//ConnectSubYkSig(pristationPtr);//����Ϣ��������
		ResetTimerDowloadPara(pristationPtr,true,0);
		AddStatusLogWithSynT("�յ��ļ�Ϊ��ProtectValPara.xml\\\\\\\\\\\n");
	}
}

int CS101_518::getAddrByRecvFrame(unsigned char * buf)
{
	unsigned char funcType = buf[0];
	unsigned char ContrCode = 0;
	int addr = -1;

	switch (funcType)
	{
	case 0x88:
		{
			ContrCode = buf[4];
			if (BufToVal(&buf[5],AsduAddrLength_) == BufToVal(&buf[AsduAddrLocation_],AsduAddrLength_))
			{
				addr = BufToVal(&buf[AsduAddrLocation_],AsduAddrLength_);
			}
			else
			{
				addr = -1;
			}

			if ((ContrCode & 0x40) != 0x40) //��鱨�ķ���λ��־
			{
				std::ostringstream ostr;
				ostr<<"������������ֱ��ķ����־λ����,��֡���Ľ����ᱻ������Control_Code ="<<ContrCode<<std::endl;
				AddStatusLogWithSynT(ostr.str());
				return -1;
			}
		}
		break;

	default:
		addr = CS101::getAddrByRecvFrame(buf);
		break;
	}

	return addr;
}

int CS101_518::AssembleLineValVerSucess(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd)
{
	size_t count = bufIndex;

	DataBase::stLine_Val VerQYC;
	VerQYC = boost::any_cast<DataBase::stLine_Val>(cmd.getVal());

	count += ValToBuf(&buf[count],M_AV_NA_3 | DIR_PRM,1);   //������Ϊ0x08
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],M_LV_VR_3,FrameTypeLength_); //���ͱ�־0xA2
	count += ValToBuf(&buf[count],1,InfoNumLength_);

	count += ValToBuf(&buf[count],VerQYC.timeout_flag,TransReasonLength_);

	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],0,InfoAddrLength_);  //��Ϣ���ַ 
	count += ValToBuf(&buf[count],VerQYC.Line_no,1);
	count += ValToBuf(&buf[count],VerQYC.D_Value_Ua,2);
	count += ValToBuf(&buf[count],VerQYC.D_Value_Ub,2);
	count += ValToBuf(&buf[count],VerQYC.D_Value_Uc,2);
	count += ValToBuf(&buf[count],VerQYC.D_Value_CIa,2);
	count += ValToBuf(&buf[count],VerQYC.D_Value_CIb,2);
	count += ValToBuf(&buf[count],VerQYC.D_Value_CIc,2);
	count += ValToBuf(&buf[count],VerQYC.D_Value_BIa,2);
	count += ValToBuf(&buf[count],VerQYC.D_Value_BIb,2);
	count += ValToBuf(&buf[count],VerQYC.D_Value_BIc,2);
	count += ValToBuf(&buf[count],VerQYC.D_Value_U0,2);
	count += ValToBuf(&buf[count],VerQYC.D_Value_I0,2);
	count += ValToBuf(&buf[count],VerQYC.Angle_UaIa,2);
	count += ValToBuf(&buf[count],VerQYC.Angle_UbIb,2);
	count += ValToBuf(&buf[count],VerQYC.Angle_UcIc,2);
	count += ValToBuf(&buf[count],VerQYC.Fre_Val,2);
	count += ValToBuf(&buf[count],VerQYC.Cos_Val,2);
	count += ValToBuf(&buf[count],VerQYC.P_Val,2);
	count += ValToBuf(&buf[count],VerQYC.Q_Val,2);

	//std::cout<<"���͵Ķ�ֵУ���ȫң��Ϊ��"<<std::endl;
	//std::cout<<VerQYC.D_Value_Ua<<"/"<<VerQYC.D_Value_Ub<<"/"<<VerQYC.D_Value_Uc<<std::endl;
	//std::cout<<VerQYC.D_Value_CIa<<"/"<<VerQYC.D_Value_CIb<<"/"<<VerQYC.D_Value_CIc<<std::endl;
	//std::cout<<VerQYC.D_Value_U0<<"/"<<VerQYC.D_Value_I0<<std::endl;
	//std::cout<<VerQYC.Fre_Val<<"/"<<VerQYC.Cos_Val<<"/"<<VerQYC.P_Val<<std::endl;

	return count - bufIndex;
}

int CS101_518::AssembleHarmonicCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd)
{
	size_t count = bufIndex;

	DataBase::stHarmonic Harmonic;
	Harmonic = boost::any_cast<DataBase::stHarmonic>(cmd.getVal());

	count += ValToBuf(&buf[count],M_AV_NA_3 | DIR_PRM,1);   //������Ϊ0x08
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],M_HA_MO_3,FrameTypeLength_); //���ͱ�־0xA7
	count += ValToBuf(&buf[count],1,InfoNumLength_);
	count += ValToBuf(&buf[count],0x07,TransReasonLength_);
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],0,InfoAddrLength_);  //��Ϣ���ַ 

	count += ValToBuf(&buf[count],Harmonic.Line_no,1); 
	count += ValToBuf(&buf[count],Harmonic.harmonic_no,1); 
	count += ValToBuf(&buf[count],Harmonic.Value_Ua,2); 
	count += ValToBuf(&buf[count],Harmonic.Value_Ub,2); 
	count += ValToBuf(&buf[count],Harmonic.Value_Uc,2); 
	count += ValToBuf(&buf[count],Harmonic.Value_Ia,2); 
	count += ValToBuf(&buf[count],Harmonic.Value_Ib,2);
	count += ValToBuf(&buf[count],Harmonic.Value_Ic,2);
	count += ValToBuf(&buf[count],Harmonic.Value_U0,2);
	count += ValToBuf(&buf[count],Harmonic.Value_I0,2);
	count += ValToBuf(&buf[count],Harmonic.P_Val,2);
	count += ValToBuf(&buf[count],Harmonic.Q_Val,2);
	count += ValToBuf(&buf[count],Harmonic.Per_Val,2);

	return count - bufIndex;

}

int CS101_518::AssembleCallValCoefCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd)
{
	size_t count = bufIndex;

	DataBase::stLine_ValCoef ValCoef;
	ValCoef = boost::any_cast<DataBase::stLine_ValCoef>(cmd.getVal());

	count += ValToBuf(&buf[count],M_AV_NA_3 | DIR_PRM,1);   //������Ϊ0x08
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],M_VA_VR_3,FrameTypeLength_); //���ͱ�־0xA3
	count += ValToBuf(&buf[count],1,InfoNumLength_);
	count += ValToBuf(&buf[count],ValCoef.timeout_flag,TransReasonLength_);
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],0,InfoAddrLength_);  //��Ϣ���ַ 

	count += ValToBuf(&buf[count],ValCoef.Line_no,1); 
	//count += ValToBuf(&buf[count],ValCoef.Flag_Ua,1); 
	count += ValToBuf(&buf[count],ValCoef.H_Value_Ua,2); 
	//count += ValToBuf(&buf[count],ValCoef.Flag_Ub,1);
	count += ValToBuf(&buf[count],ValCoef.H_Value_Ub,2); 
	//count += ValToBuf(&buf[count],ValCoef.Flag_Uc,1);
	count += ValToBuf(&buf[count],ValCoef.H_Value_Uc,2); 
	//count += ValToBuf(&buf[count],ValCoef.Flag_CIa,1);
	count += ValToBuf(&buf[count],ValCoef.H_Value_CIa,2); 
	//count += ValToBuf(&buf[count],ValCoef.Flag_CIb,1);
	count += ValToBuf(&buf[count],ValCoef.H_Value_CIb,2);
	//count += ValToBuf(&buf[count],ValCoef.Flag_CIc,1);
	count += ValToBuf(&buf[count],ValCoef.H_Value_CIc,2); 
	//count += ValToBuf(&buf[count],ValCoef.Flag_BIa,1);
	count += ValToBuf(&buf[count],ValCoef.H_Value_BIa,2); 
	//count += ValToBuf(&buf[count],ValCoef.Flag_BIb,1);
	count += ValToBuf(&buf[count],ValCoef.H_Value_BIb,2);
	//count += ValToBuf(&buf[count],ValCoef.Flag_BIc,1);
	count += ValToBuf(&buf[count],ValCoef.H_Value_BIc,2);
	//count += ValToBuf(&buf[count],ValCoef.Flag_U0,1);
	count += ValToBuf(&buf[count],ValCoef.H_Value_U0,2); 
	//count += ValToBuf(&buf[count],ValCoef.Flag_I0,1);
	count += ValToBuf(&buf[count],ValCoef.H_Value_I0,2); 
	//count += ValToBuf(&buf[count],ValCoef.Flag_Angle_UaIa,1);
	count += ValToBuf(&buf[count],ValCoef.Angle_UaIa,2); 
	//count += ValToBuf(&buf[count],ValCoef.Flag_Angle_UbIb,1);
	count += ValToBuf(&buf[count],ValCoef.Angle_UbIb,2);
	//count += ValToBuf(&buf[count],ValCoef.Flag_Angle_UcIc,1);
	count += ValToBuf(&buf[count],ValCoef.Angle_UcIc,2); 

	return count - bufIndex;
}

int CS101_518::AssembleSendFlieCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr)
{
	char Name[FileNameLen];
	std::string filename;
	size_t count = bufIndex;
	int Namelen_;

	memset(Name,0,FileNameLen);

	filename = FileHandleGetFileName();

	Namelen_ = filename.length();

	memcpy(Name,filename.c_str(),Namelen_);


	count += ValToBuf(&buf[count],M_AV_NA_3 | DIR_PRM,1);   //������Ϊ0x08
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],M_FI_SE_3,FrameTypeLength_); //���ͱ�־0xA1
	count += ValToBuf(&buf[count],1,InfoNumLength_);
	count += ValToBuf(&buf[count],trans_actcon,TransReasonLength_);//����ԭ��Ϊ0x07
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],0,InfoAddrLength_);  //��Ϣ���ַ 

	for(int i = 0;i < Namelen_;i ++)
	{
		count += ValToBuf(&buf[count],Name[i],1);
	}

	count += ValToBuf(&buf[count],FileHandleGetTotalLength(),3);

	return count - bufIndex;
}

int CS101_518::AssembleSendFlieBodyCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd)
{
	size_t count = bufIndex;
	int Val =  boost::any_cast<int>(cmd.getVal());

	count += ValToBuf(&buf[count],M_AV_NA_3 | DIR_PRM,1);   //������Ϊ0x08
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],M_FI_SE_3,FrameTypeLength_); //���ͱ�־0xA1
	count += ValToBuf(&buf[count],(This_Length_ & 0x80),InfoNumLength_);
	count += ValToBuf(&buf[count],Val,TransReasonLength_);//����ԭ��Ϊ0x0a��ʾȷ�ϣ�0x0F��ʾ���
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],Total_NUM,1);  //��Ϣ���ַ 
	count += ValToBuf(&buf[count],This_N0,1);  //��֡��
	count += ValToBuf(&buf[count],0x14,1);  //0x14

	return count - bufIndex;
}

int CS101_518::AssembleCallFlieNameCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd)
{
	char Name[FileNameLen];
	std::string filename;
	size_t count = bufIndex;
	char Val =  0x05;
	int Namelen_;

	Val =  boost::any_cast<char>(cmd.getVal());
	memset(Name,0,FileNameLen);

	filename = FileHandleGetFileName();

	Namelen_ = filename.length();

	//std::cout<<"filename="<<filename<<"Namelen_="<<Namelen_<<std::endl;

	memcpy(Name,filename.c_str(),Namelen_);

	count += ValToBuf(&buf[count],M_AV_NA_3 | DIR_PRM,1);   //������Ϊ0x08
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],M_FI_RE_3,FrameTypeLength_); //���ͱ�־0xA0
	count += ValToBuf(&buf[count],1,1);
	count += ValToBuf(&buf[count],Val,TransReasonLength_);//����ԭ��Ϊ0x07��ʾȷ�ϣ�0x05��ʾ�ļ�������
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],0,2);
	for(int i = 0;i < Namelen_;i ++)
	{
		count += ValToBuf(&buf[count],Name[i],1); 
	}

	count += ValToBuf(&buf[count],FileHandleGetTotalLength(),3);  //�ļ����� 

	if(Val == 0x07)
	{
		//std::cout<<"���ٻ����ļ����ڣ�׼������... ..."<<std::endl; 
		AddWaitCmdVal(CALL_FILEBODY_ACK,CALL_FILEBODY_ACK_PRIORITY,pristationPtr,1);

		//AddStatusLogWithSynT("���CALL_FILEBODY_ACK���... \n");
	}
	else
	{      
		FileHandleFinish();
		std::ostringstream ostr;
		ostr<<"���ٻ����ļ������ڻ��߷��������0x07�ɹ���0x0A��ʧ�ܣ�����ֵValΪ��"<< Val <<std::endl; 
		AddStatusLogWithSynT(ostr.str());
	}
	//std::cout<<"��װ��������ɺ󣬷����ļ�count��ֵΪ��"<<count<<std::endl;
	return count - bufIndex;
}

int CS101_518::AssembleCallFlieBody(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd)
{
	size_t count = bufIndex;
	size_t len;
	char reason;
	//std::cout<<"��ʼ��װ����׼�������ļ�... ..." <<std::endl; 
	if(FileHandleGetRemainLength() > FileLen)
	{
		len = FileLen;
		//SendFileLength_ -= FileLen;
	}
	else
	{
		len = FileHandleGetRemainLength();
	}

	int Val =  boost::any_cast<int>(cmd.getVal());

	This_N0 = Val;

	count += ValToBuf(&buf[count],M_AV_NA_3 | DIR_PRM,1);   //������Ϊ0x08
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],M_FI_RE_3,FrameTypeLength_); //���ͱ�־0xA0
	count += ValToBuf(&buf[count],1,1);
	count += ValToBuf(&buf[count],0x14,TransReasonLength_);
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],Total_NUM,1);
	count += ValToBuf(&buf[count],This_N0,1);
	//std::cout<<"����ָ�����ڵ�λ��Ϊ��"<< FileLoadPtr_ <<std::endl; 

	//for(size_t i = 0;i < len; i ++)
	//{
	//	//std::cout<<CallFileBuf[FileLoadPtr_]<<std::endl; 
	//	//count += ValToBuf(&buf[count],CallFileBuf[FileLoadPtr_ ++],1);
	//	buf[count ++] = CallFileBuf[FileLoadPtr_ ++];
	//}

	count += FileHandleGetFile(&buf[count],len);

	// std::cout<<"����ָ�����ڵ�λ��Ϊ��"<< FileLoadPtr_ <<std::endl; 
	//std::cout<<"���ٻ����ļ����ڣ���֡��Ϊ��"<< Total_NUM <<",��֡Ϊ��"<<This_N0<<std::endl; 
	if(Total_NUM > This_N0)
	{
		Val ++;
		//  std::cout<<"��ʼ��Ӽ�������ָ��. ..."<<std::endl; 
		AddWaitCmdVal(CALL_FILEBODY_ACK,CALL_FILEBODY_ACK_PRIORITY,pristationPtr,Val);
		//  std::cout<<"���ָ����� ... "<<std::endl; 
	}
	else 
	{
		AddStatusLogWithSynT("�ļ�������ɣ���ʼ���ͽ���֡... ...\n");

		reason = 0x0A;
		AddWaitCmdVal(CALL_FILENAME_CON,CALL_FILENAME_CON_PRIORITY,pristationPtr,reason);
	}

	return count - bufIndex;
}

int CS101_518::AssembleRebootCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr)
{
	size_t count = bufIndex;
	count += ValToBuf(&buf[count],M_AV_NA_3 | DIR_PRM,1);   //������Ϊ0x08
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],M_ZD_RE_3,FrameTypeLength_); //���ͱ�־0xA5
	count += ValToBuf(&buf[count],1,1);
	count += ValToBuf(&buf[count],0x07,TransReasonLength_);
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],0,2);
	return count - bufIndex;
}

int CS101_518::AssembleCallTimeCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,boost::posix_time::ptime time)
{
	size_t count = bufIndex;

	count += ValToBuf(&buf[count],M_AV_NA_3 | DIR_PRM,1);   //������Ϊ0x08
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],M_CA_TM_3,FrameTypeLength_); //���ͱ�־0xA9
	count += ValToBuf(&buf[count],1,1);
	count += ValToBuf(&buf[count],0x07,TransReasonLength_);
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],0,2);

	boost::posix_time::time_duration td = time.time_of_day();
	count += ValToBuf(&buf[count],td.total_milliseconds() % MinutesRemainderMillisecs,2);
	buf[count++] = td.minutes() & 0x3f;
	buf[count++] = td.hours() & 0x1f;
	boost::gregorian::date::ymd_type ymd = time.date().year_month_day();
	buf[count++] = ymd.day & 0x1f;
	buf[count++] = ymd.month & 0x0f;
	buf[count++] = ymd.year % 100;

	std::ostringstream ostr;
	ostr<<"����ʱ���ǣ�"<< ymd.year<<"��"<< ymd.month <<"��"<< ymd.day <<"��"<<std::endl; 
	AddStatusLogWithSynT(ostr.str());

	return count - bufIndex;
}

int CS101_518::AssembleDownLoadLineHValCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd)
{
	size_t count = bufIndex;

	DataBase::stLine_ValCoef HVal;

	HVal =  boost::any_cast<DataBase::stLine_ValCoef>(cmd.getVal());

	count += ValToBuf(&buf[count],M_AV_NA_3 | DIR_PRM,1);   //������Ϊ0x08
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],M_VA_DL_3,FrameTypeLength_); //���ͱ�־0xA4
	count += ValToBuf(&buf[count],1,1);
	count += ValToBuf(&buf[count],0x07,TransReasonLength_);
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],0,2);

	buf[count++] =  HVal.Line_no;
	buf[count++] =  HVal.Flag_Ua;
	buf[count++] = (HVal.H_Value_Ua & 0xff);
	buf[count++] = ((HVal.H_Value_Ua >> 8)& 0xff);
	buf[count++] =  HVal.Flag_Ub;
	buf[count++] = (HVal.H_Value_Ub & 0xff);
	buf[count++] = ((HVal.H_Value_Ub >> 8)& 0xff);
	buf[count++] =  HVal.Flag_Uc;
	buf[count++] = (HVal.H_Value_Uc & 0xff);
	buf[count++] = ((HVal.H_Value_Uc >> 8)& 0xff);
	buf[count++] =  HVal.Flag_CIa;
	buf[count++] = (HVal.H_Value_CIa & 0xff);
	buf[count++] = ((HVal.H_Value_CIa >> 8)& 0xff);
	buf[count++] =  HVal.Flag_CIb;
	buf[count++] = (HVal.H_Value_CIb & 0xff);
	buf[count++] = ((HVal.H_Value_CIb >> 8)& 0xff);
	buf[count++] =  HVal.Flag_CIc;
	buf[count++] = (HVal.H_Value_CIc & 0xff);
	buf[count++] = ((HVal.H_Value_CIc >> 8)& 0xff);
	buf[count++] =  HVal.Flag_BIa;
	buf[count++] = (HVal.H_Value_BIa & 0xff);
	buf[count++] = ((HVal.H_Value_BIa >> 8)& 0xff);
	buf[count++] =  HVal.Flag_BIb;
	buf[count++] = (HVal.H_Value_BIb & 0xff);
	buf[count++] = ((HVal.H_Value_BIb >> 8)& 0xff);
	buf[count++] =  HVal.Flag_BIc;
	buf[count++] = (HVal.H_Value_BIc & 0xff);
	buf[count++] = ((HVal.H_Value_BIc >> 8)& 0xff);
	buf[count++] =  HVal.Flag_U0;
	buf[count++] = (HVal.H_Value_U0 & 0xff);
	buf[count++] = ((HVal.H_Value_U0 >> 8)& 0xff);
	buf[count++] =  HVal.Flag_I0;
	buf[count++] = (HVal.H_Value_I0 & 0xff);
	buf[count++] = ((HVal.H_Value_I0 >> 8)& 0xff);
	buf[count++] =  HVal.Flag_Angle_UaIa;
	buf[count++] = (HVal.Angle_UaIa & 0xff);
	buf[count++] = ((HVal.Angle_UaIa >> 8)& 0xff);
	buf[count++] =  HVal.Flag_Angle_UbIb;
	buf[count++] = (HVal.Angle_UbIb & 0xff);
	buf[count++] = ((HVal.Angle_UbIb >> 8)& 0xff);
	buf[count++] =  HVal.Flag_Angle_UcIc;
	buf[count++] = (HVal.Angle_UcIc & 0xff);
	buf[count++] = ((HVal.Angle_UcIc >> 8)& 0xff);

	return count - bufIndex; 
}


int CS101_518::AssembleDownLoadParaCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd)
{
	size_t count = bufIndex;
	char reason = boost::any_cast<char>(cmd.getVal());

	count += ValToBuf(&buf[count],M_AV_NA_3 | DIR_PRM,1);   //������Ϊ0x08
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],M_DL_PA_3,FrameTypeLength_); //���ͱ�־0xB3
	count += ValToBuf(&buf[count],1,1);
	count += ValToBuf(&buf[count],reason,TransReasonLength_);
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],0,2);

	return count - bufIndex;
}

int CS101_518::AssembleBoardInq(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd)
{
	size_t count = bufIndex;

	DataBase::stBoardInq Data;

	Data =  boost::any_cast<DataBase::stBoardInq>(cmd.getVal());

	count += ValToBuf(&buf[count],M_AV_NA_3 | DIR_PRM,1);   //������Ϊ0x08
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],M_BD_RQ_3,FrameTypeLength_); //���ͱ�־0xAA
	count += ValToBuf(&buf[count],1,1);
	count += ValToBuf(&buf[count],0x07,TransReasonLength_);
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],0,2);
	count += ValToBuf(&buf[count],Data.YxBoardNum,1);
	count += ValToBuf(&buf[count],Data.YkBoardNum,1);
	count += ValToBuf(&buf[count],Data.YcBoardNum,1);

	return count - bufIndex; 

}

int CS101_518::AssembleBattryActiveCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr)
{
	size_t count = bufIndex;
	count += ValToBuf(&buf[count],M_AV_NA_3 | DIR_PRM,1);   //������Ϊ0x08
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],M_AC_BT_3,FrameTypeLength_); //���ͱ�־0xB1
	count += ValToBuf(&buf[count],1,1);
	count += ValToBuf(&buf[count],0x07,TransReasonLength_);
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],0,2);
	return count - bufIndex; 
}

int CS101_518::AssembleBattryActiveOverCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr)
{
	size_t count = bufIndex;
	count += ValToBuf(&buf[count],M_AV_NA_3 | DIR_PRM,1);   //������Ϊ0x08
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],M_OV_BT_3,FrameTypeLength_); //���ͱ�־0xB2
	count += ValToBuf(&buf[count],1,1);
	count += ValToBuf(&buf[count],0x07,TransReasonLength_);
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],0,2);
	return count - bufIndex; 
}

int CS101_518::AssembleSignalResetCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd)
{
	size_t count = bufIndex;

	unsigned char Reason;
	int LineNo_ = boost::any_cast<int>(cmd.getVal());
	int LineNo;

	if (LineNo_ >= 0)
	{
		Reason = 0x07;
		LineNo = LineNo_;
	} 
	else
	{
		Reason = 0x0a;
		LineNo = 255;
	}


	count += ValToBuf(&buf[count],M_AV_NA_3 | DIR_PRM,1);   //������Ϊ0x08
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],M_SG_RE_3,FrameTypeLength_); //���ͱ�־0xA6
	count += ValToBuf(&buf[count],1,1);
	count += ValToBuf(&buf[count],Reason,TransReasonLength_);
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],0,2);
	count += ValToBuf(&buf[count],LineNo,1);
	
	return count - bufIndex; 
}

int CS101_518::AssembleCpuVerInqCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd)
{
	size_t count = bufIndex;
	int CpuVer = boost::any_cast<int>(cmd.getVal());

	count += ValToBuf(&buf[count],M_AV_NA_3 | DIR_PRM,1);   //������Ϊ0x08
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],M_RE_VR_3,FrameTypeLength_); //���ͱ�־0xB4
	count += ValToBuf(&buf[count],1,1);
	count += ValToBuf(&buf[count],0x07,TransReasonLength_);
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],0,2);
	count += ValToBuf(&buf[count],CpuVer,2);
	return count - bufIndex; 
}

int CS101_518::AssembleLineBValVer(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd)
{
	DataBase::stLine_BCurVerVal Val;
	Val = boost::any_cast<DataBase::stLine_BCurVerVal>(cmd.getVal());
	size_t count = bufIndex;
	count += ValToBuf(&buf[count],M_AV_NA_3 | DIR_PRM,1);   //������Ϊ0x08
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],M_DL_BV_3,FrameTypeLength_); //���ͱ�־0xB5
	count += ValToBuf(&buf[count],1,1);
	count += ValToBuf(&buf[count],Val.timeout_flag,TransReasonLength_);
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],0,2);
	count += ValToBuf(&buf[count],Val.Line_No,1);
	count += ValToBuf(&buf[count],Val.V_Val_BI1,2);
	count += ValToBuf(&buf[count],Val.V_Val_BI2,2);
	count += ValToBuf(&buf[count],Val.V_Val_BI3,2);
	//std::cout<<"S101���͵ı�������У���ֵΪ��"<<"Val.V_Val_BI1:"<<Val.V_Val_BI1<<" Val.V_Val_BI2:"<<Val.V_Val_BI2<<" Val.V_Val_BI3:"<<Val.V_Val_BI3<<std::endl;
	return count - bufIndex; 
}

int CS101_518::AssembleDcValVerSucess(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd)
{
	DataBase::stDCVerVal Val;
	Val = boost::any_cast<DataBase::stDCVerVal>(cmd.getVal());
	size_t count = bufIndex;
	count += ValToBuf(&buf[count],M_AV_NA_3 | DIR_PRM,1);   //������Ϊ0x08
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],M_DC_VR_3,FrameTypeLength_); //���ͱ�־0xB5
	count += ValToBuf(&buf[count],1,1);
	count += ValToBuf(&buf[count],Val.timeout_flag,TransReasonLength_);
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],0,2);
	count += ValToBuf(&buf[count],Val.ChannelNo,1);
	count += ValToBuf(&buf[count],Val.V_Val_DC,2);
	//std::cout<<"S101���͵�ֱ����У���ֵΪ��"<<Val.V_Val_DC<<std::endl;
	return count - bufIndex; 
}

int CS101_518::AssembleCallPMCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr,CCmd & cmd)
{
	DataBase::stPMPara Val;
	Val = boost::any_cast<DataBase::stPMPara>(cmd.getVal());
	size_t count = bufIndex;
	count += ValToBuf(&buf[count],M_AV_NA_3 | DIR_PRM,1);   //������Ϊ0x08
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],M_PM_CA_3,FrameTypeLength_); //���ͱ�־0xB5
	count += ValToBuf(&buf[count],1,1);
	count += ValToBuf(&buf[count],Val.timeout_flag,TransReasonLength_);
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],0,InfoAddrLength_);
	count += ValToBuf(&buf[count],Val.LineNo,1);
	count += ValToBuf(&buf[count],Val.AngU1,2);
	count += ValToBuf(&buf[count],Val.AngU2,2);
	count += ValToBuf(&buf[count],Val.AngU3,2);
	count += ValToBuf(&buf[count],Val.AngCI1,2);
	count += ValToBuf(&buf[count],Val.AngCI2,2);
	count += ValToBuf(&buf[count],Val.AngCI3,2);
	count += ValToBuf(&buf[count],Val.AngBI1,2);
	count += ValToBuf(&buf[count],Val.AngBI2,2);
	count += ValToBuf(&buf[count],Val.AngBI3,2);
	count += ValToBuf(&buf[count],Val.AngU0,2);
	count += ValToBuf(&buf[count],Val.AngI0,2);
	//std::cout<<"S101���͵�ֱ����У���ֵΪ��"<<Val.V_Val_DC<<std::endl;
	return count - bufIndex; 
}

int  CS101_518::AssembleCallCRCCon(size_t bufIndex,unsigned char * buf,share_pristation_ptr pristationPtr)
{
	size_t count = bufIndex;

	count += ValToBuf(&buf[count],M_AV_NA_3 | DIR_PRM,1);   //������Ϊ0x08
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],M_CA_CR_3,FrameTypeLength_); //���ͱ�־0xB5
	count += ValToBuf(&buf[count],1,1);
	count += ValToBuf(&buf[count],0x07,TransReasonLength_);
	count += ValToBuf(&buf[count],pristationPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],0,InfoAddrLength_);
	count += ValToBuf(&buf[count],CRC_Code,2);

	return count - bufIndex; 
}


int CS101_518::AssembleFrameHead(size_t bufIndex, unsigned char * buf, CCmd & cmd)
{
	size_t count = bufIndex;

	switch (cmd.getCmdType())
	{
	case SEND_FILENAME_CON:
		{
			buf[count++] = 0x88;
			buf[count++] = 0;
			buf[count++] = 0;
			buf[count++] = 0x88;
		}
		break;

	case SEND_FILEBODY_CON:
		{
			buf[count++] = 0x88;
			buf[count++] = 0;
			buf[count++] = 0;
			buf[count++] = 0x88;
		}
		break;

	case CALL_FILENAME_CON:
		{
			buf[count++] = 0x88;
			buf[count++] = 0;
			buf[count++] = 0;
			buf[count++] = 0x88;
		}
		break;

	case CALL_FILEBODY_ACK:
		{
			buf[count++] = 0x88;
			buf[count++] = 0;
			buf[count++] = 0;
			buf[count++] = 0x88;
		}
		break;

	default:
		return CS101::AssembleFrameHead(bufIndex,buf,cmd);
		break;
	}

	return count - bufIndex;
}

int CS101_518::AssembleFrameBody(size_t bufIndex, unsigned char * buf, CCmd & cmd)
{
	int bytesAssemble = 0;

	share_pristation_ptr pristationPtr;

	if (cmd.getCommPoint())
	{
		if (cmd.getCommPoint()->getCommPointType() == PRISTATION_NODE)
		{
			pristationPtr = boost::dynamic_pointer_cast<DataBase::CPriStation>(cmd.getCommPoint());
		}
	}

	if (!pristationPtr)
	{
		std::ostringstream ostr;
		ostr<<"S101��Լ���ܴӷ��������л��pristation ptr��cmdtype = "<<cmd.getCmdType()<<"�����������ᱻ���͡�"<<std::endl;
		AddStatusLogWithSynT(ostr.str());

		return -1;
	}

	switch (cmd.getCmdType())
	{
	case LINE_VAL_VER_QYC:
		{
			bytesAssemble = AssembleLineValVerSucess(bufIndex,buf,pristationPtr,cmd);
			if (bytesAssemble > 0)
			{
				DisconnectSubYkSig(pristationPtr,false);//����Ϣ�Ͽ�
				ResetTimerValVer(pristationPtr,false,0);
				CheckACD(bufIndex,buf,pristationPtr);
			}
		}
		break;

	case LINE_DCVAL_VER_SUCESS:
		{
			bytesAssemble = AssembleDcValVerSucess(bufIndex,buf,pristationPtr,cmd);
			if (bytesAssemble > 0)
			{
				ResetTimerDcVer(pristationPtr,false,0);
				CheckACD(bufIndex,buf,pristationPtr);
			}
			DisconnectSubYkSig(pristationPtr,false);//����Ϣ�Ͽ�
		}
		break;

	case HARMONIC_CON:
		{
			bytesAssemble = AssembleHarmonicCon(bufIndex,buf,pristationPtr,cmd);
			if (bytesAssemble > 0)
			{

				AddStatusLogWithSynT("��ʼ����... ...\n");

				DisconnectSubYkSig(pristationPtr,false);//����Ϣ�Ͽ�
				CheckACD(bufIndex,buf,pristationPtr);
			}
		}
		break;

	case CALL_VALCOEF_CON:
		{
			bytesAssemble = AssembleCallValCoefCon(bufIndex,buf,pristationPtr,cmd);
			if (bytesAssemble > 0)
			{
				DisconnectSubYkSig(pristationPtr,false);//����Ϣ�Ͽ�
				CheckACD(bufIndex,buf,pristationPtr);
			}
			ResetTimerCallHVal(pristationPtr,false,0);
		}
		break;

	case SIGNAL_RESET_CON:
		{
			bytesAssemble = AssembleSignalResetCon(bufIndex,buf,pristationPtr,cmd);
			if (bytesAssemble > 0)
			{
				DisconnectSubYkSig(pristationPtr,false);//����Ϣ�Ͽ�
				CheckACD(bufIndex,buf,pristationPtr);
			}
		}
		break;

	case SEND_FILENAME_CON:
		{
			bytesAssemble = AssembleSendFlieCon(bufIndex,buf,pristationPtr);
			if (bytesAssemble > 0)
			{
				CheckACD(bufIndex,buf,pristationPtr);
			}
		}
		break;

	case SEND_FILEBODY_CON:
		{
			bytesAssemble = AssembleSendFlieBodyCon(bufIndex,buf,pristationPtr,cmd);
			if (bytesAssemble > 0)
			{
				CheckACD(bufIndex,buf,pristationPtr);
			}
		}
		break;

	case CALL_FILENAME_CON:
		{

			bytesAssemble = AssembleCallFlieNameCon(bufIndex,buf,pristationPtr,cmd);
			if (bytesAssemble > 0)
			{
				CheckACD(bufIndex,buf,pristationPtr);
			}
		}
		break;

	case CALL_FILEBODY_ACK:
		{     
			//std::cout<<"�յ�ָ��CALL_FILEBODY_ACK... "<<std::endl; 
			bytesAssemble = AssembleCallFlieBody(bufIndex,buf,pristationPtr,cmd);
			if (bytesAssemble > 0)
			{
				CheckACD(bufIndex,buf,pristationPtr);
			}
		}
		break;

	case REBOOT_CON:
		{

			bytesAssemble = AssembleRebootCon(bufIndex,buf,pristationPtr);
			if (bytesAssemble > 0)
			{
				CheckACD(bufIndex,buf,pristationPtr);
				ResetTimerReboot(pristationPtr,true,0);
			}	 
		}
		break;

	case CALL_TIME_CON:
		{
			bytesAssemble = AssembleCallTimeCon(bufIndex,buf,pristationPtr,boost::posix_time::microsec_clock::local_time());
			if (bytesAssemble > 0)
			{
				CheckACD(bufIndex,buf,pristationPtr);
			}	 
		}
		break;

	case DOWNLOAD_LINE_CON:
		{
			bytesAssemble = AssembleDownLoadLineHValCon(bufIndex,buf,pristationPtr,cmd);
			if (bytesAssemble > 0)
			{
				CheckACD(bufIndex,buf,pristationPtr);
			}	 
		}
		break;

	case DOWNLOAD_PARA_CON:
		{
			//std::cout<<"S101�յ�DOWNLOAD_PARA_CON... ..."<<std::endl;
			bytesAssemble = AssembleDownLoadParaCon(bufIndex,buf,pristationPtr,cmd);
			if (bytesAssemble > 0)
			{
				ResetTimerDowloadPara(pristationPtr,false,0);
				DisconnectSubYkSig(pristationPtr,false);//����Ϣ�Ͽ�
				CheckACD(bufIndex,buf,pristationPtr);
			}	 
		}
		break;

	case BOARD_REQ_CON:
		{
			bytesAssemble = AssembleBoardInq(bufIndex,buf,pristationPtr,cmd);
			if (bytesAssemble > 0)
			{
				DisconnectSubYkSig(pristationPtr,false);//����Ϣ�Ͽ�
				CheckACD(bufIndex,buf,pristationPtr);
			}	 
		}
		break;

	case CALL_EQU_PARA_CON:
		{
			//std::cout<<"S101�յ��ٻ�װ�ò����ɹ�ָ��... ..."<<std::endl; 
			char Val;
			if (FileHandleRead() > 0)
			{
				Val = 0x07;
				Total_NUM = ((FileHandleGetTotalLength() - 1) / FileLen) + 1;
			}
			else
			{
				Val = 0x05;
			}
			AddSendCmdVal(CALL_FILENAME_CON,CALL_FILENAME_CON_PRIORITY,pristationPtr,Val);

			DisconnectSubYkSig(pristationPtr,false);//����Ϣ�Ͽ�
			ResetTimerCallFileDisConect(pristationPtr);

			CheckACD(bufIndex,buf,pristationPtr);
		}
		break;

	case CALL_INTERFACE_PARA_CON:
		{
			char Val;
			if (FileHandleRead() > 0)
			{
				Val = 0x07;
				Total_NUM = ((FileHandleGetTotalLength() - 1) / FileLen) + 1;
			}
			else
			{
				Val = 0x05;
			}
			AddSendCmdVal(CALL_FILENAME_CON,CALL_FILENAME_CON_PRIORITY,pristationPtr,Val);

			DisconnectSubYkSig(pristationPtr,false);//����Ϣ�Ͽ�
			ResetTimerCallFileDisConect(pristationPtr);

			CheckACD(bufIndex,buf,pristationPtr);
		}
		break;
		
	case CALL_CHTYPE_CON:
		{
			//std::cout<<"S101�յ��ٻ�ͨ�����ͳɹ�ָ��... ..."<<std::endl; 
			char Val;
			if (FileHandleRead() > 0)
			{
				Val = 0x07;
				Total_NUM = ((FileHandleGetTotalLength() - 1) / FileLen) + 1;
			}
			else
			{
				Val = 0x05;
			}
			AddSendCmdVal(CALL_FILENAME_CON,CALL_FILENAME_CON_PRIORITY,pristationPtr,Val);

			DisconnectSubYkSig(pristationPtr,false);//����Ϣ�Ͽ�
			ResetTimerCallFileDisConect(pristationPtr);

			CheckACD(bufIndex,buf,pristationPtr);
		}
		break;

	case CALL_PROVAL_CON:
		{
			//std::cout<<"S101�յ��ٻ�������ֵ�����������ֳɹ�ָ��... ..."<<std::endl; 
			char Val;
			if (FileHandleRead() > 0)
			{
				Val = 0x07;
				Total_NUM = ((FileHandleGetTotalLength() - 1) / FileLen) + 1;
			}
			else
			{
				Val = 0x05;
			}
			AddSendCmdVal(CALL_FILENAME_CON,CALL_FILENAME_CON_PRIORITY,pristationPtr,Val);

			DisconnectSubYkSig(pristationPtr,false);//����Ϣ�Ͽ�
			ResetTimerCallFileDisConect(pristationPtr);

			CheckACD(bufIndex,buf,pristationPtr);
		}
		break;

	case CALL_LINEPARA_CON:
		{
			//        std::cout<<"S101�յ��ٻ�ͨ����Ϲ�ϵ�ɹ�ָ��... ..."<<std::endl; 
			char Val;
			if (FileHandleRead() > 0)
			{
				Val = 0x07;
				Total_NUM = ((FileHandleGetTotalLength() - 1) / FileLen) + 1;
			}
			else
			{
				Val = 0x05;
			}
			AddSendCmdVal(CALL_FILENAME_CON,CALL_FILENAME_CON_PRIORITY,pristationPtr,Val);

			DisconnectSubYkSig(pristationPtr,false);//����Ϣ�Ͽ�
			ResetTimerCallFileDisConect(pristationPtr);

			CheckACD(bufIndex,buf,pristationPtr);
		}
		break;

	case BATTERY_ACTIVE_CON:
		{
			//std::cout<<"S101�յ���ػ�ɹ�ָ��... ..."<<std::endl; 
			bytesAssemble = AssembleBattryActiveCon(bufIndex,buf,pristationPtr);
			if (bytesAssemble > 0)
			{
				CheckACD(bufIndex,buf,pristationPtr);
			}
			//DisconnectSubYkSig(pristationPtr);//����Ϣ�Ͽ�
		}
		break;

	case BATTERY_ACTIVE_OVER_CON:
		{
			//std::cout<<"S101�յ���ػ�˳��ɹ�ָ��... ..."<<std::endl; 
			bytesAssemble = AssembleBattryActiveOverCon(bufIndex,buf,pristationPtr);
			if (bytesAssemble > 0)
			{
				CheckACD(bufIndex,buf,pristationPtr);
			}
			//DisconnectSubYkSig(pristationPtr);//����Ϣ�Ͽ�
		}
		break;

	case DSP_VERSION_INQ_CON:
		{
			bytesAssemble = AssembleCpuVerInqCon(bufIndex,buf,pristationPtr,cmd);
			if (bytesAssemble > 0)
			{
				CheckACD(bufIndex,buf,pristationPtr);
			}
		}
		break;

	case LINE_BVAL_VER_QYC:
		{
			bytesAssemble = AssembleLineBValVer(bufIndex,buf,pristationPtr,cmd);
			if (bytesAssemble > 0)
			{
				ResetTimerBCurVer(pristationPtr,false,0);
				CheckACD(bufIndex,buf,pristationPtr);
			}
			DisconnectSubYkSig(pristationPtr,false);//����Ϣ�Ͽ�
		}
		break;

	case CALL_PM_ANG_CON:
		{
			bytesAssemble = AssembleCallPMCon(bufIndex,buf,pristationPtr,cmd);
			if (bytesAssemble > 0)
			{
				ResetTimerCallPm(pristationPtr,false,0);
				CheckACD(bufIndex,buf,pristationPtr);
			}
			//DisconnectSubYkSig(pristationPtr);//����Ϣ�Ͽ�
		}
		break;

	case CALL_CRC_CON:
		{
			bytesAssemble = AssembleCallCRCCon(bufIndex,buf,pristationPtr);
			if (bytesAssemble > 0)
			{
				CheckACD(bufIndex,buf,pristationPtr);
			}
		}
		break;

	default:
		{
			return CS101::AssembleFrameBody(bufIndex,buf,cmd);
			break;
		}
	}
	return bytesAssemble;
}

int CS101_518::AssembleFrameTail( size_t bufBegin, size_t bufIndex, unsigned char * buf, CCmd & cmd )
{
	int length = bufIndex - bufBegin;
	if (length <= 1)
	{
		return -1;
	}

	size_t count = bufIndex;

	switch (cmd.getCmdType())
	{
	case SEND_FILENAME_CON:
		{
			int framelength = length - 4;
			if (framelength <= 0 || framelength > max_frame_length_)
			{
				return -1;
			}
			buf[count++] = CalcCheckSumByte(&buf[bufBegin + 4],framelength);
			buf[count++] = 0x16;
			buf[bufBegin + 1] = framelength & 0xff;
			buf[bufBegin + 2] = (framelength >> 8) & 0xff;
			//		std::cout<<"��ɱ���β����װ... "<<std::endl;
		}
		break;

	case SEND_FILEBODY_CON:
		{
			int framelength = length - 4;
			if (framelength <= 0 || framelength > max_frame_length_)
			{
				return -1;
			}
			buf[count++] = CalcCheckSumByte(&buf[bufBegin + 4],framelength);
			buf[count++] = 0x16;
			buf[bufBegin + 1] = framelength & 0xff;
			buf[bufBegin + 2] = (framelength >> 8) & 0xff;
			//		std::cout<<"��ɱ���β����װ... "<<std::endl;
		}
		break;

	case CALL_FILENAME_CON:
		{
			//	    std::cout<<"CALL_FILENAME_CON����β����װ��ʼ... ..."<<std::endl;
			int framelength = length - 4;
			//std::cout<<"bufIndex��"<<bufIndex<<",bufBegin:"<< bufBegin <<",framelength:"<<framelength<<std::endl;
			if (framelength <= 0 || framelength > max_frame_length_)
			{
				AddStatusLogWithSynT("CALL_FILENAME_CON����β����װ����... ...\n");
				return -1;
			}
			buf[count++] = CalcCheckSumByte(&buf[bufBegin + 4],framelength);
			buf[count++] = 0x16;
			buf[bufBegin + 1] = framelength & 0xff;
			buf[bufBegin + 2] = (framelength >> 8) & 0xff;
			//		std::cout<<"CALL_FILENAME_CON����β����װ���... ..."<<std::endl;
		}
		break;

	case CALL_FILEBODY_ACK:
		{
			int framelength = length - 4;
			//if (framelength <= 0 || framelength > max_frame_length_)
			//{
			//	return -1;
			//}
			buf[count++] = CalcCheckSumByte(&buf[bufBegin + 4],framelength);
			buf[count++] = 0x16;
			buf[bufBegin + 1] = framelength & 0xff;
			buf[bufBegin + 2] = (framelength >> 8) & 0xff;
		}
		break;

	default:
		{
			return CS101::AssembleFrameTail(bufBegin,bufIndex,buf,cmd);
		}
		break;
	}

	return count - bufIndex;
}

void CS101_518::InitDefaultTimeOut()
{
	timeOutValVer_ = DEFAULT_timeOutValVer;
	timeOutReboot_ = DEFAULT_timeOutReboot;
	timeOutCallHVal_ = DEFAULT_timeOutCallHVal;
	timeOutDownloadPara_ = DEFAULT_timeOutDownloadPara;
	timeOutBCurVer_  = DEFAULT_timeOutBCurVer;
	timeOutDcVer_    = DEFAULT_timeOutDcVer;
	timeOutCallPm_   = DEFAULT_timeOutCallPm;
	timeOutCallFileDisConect_ = DEFAULT_timeOutCallFileDisConect;
}

void CS101_518::InitDefaultTimer(boost::asio::io_service & io_service)
{
	InitDefaultTimeOut();

	using namespace boost::asio;
	using namespace boost::posix_time;

	ValVerTimer_.reset(new deadline_timer(io_service,seconds(timeOutValVer_)));
	AddTimer(ValVerTimer_);

	RebootTimer_.reset(new deadline_timer(io_service,seconds(timeOutReboot_)));
	AddTimer(RebootTimer_);

	CallHvalTimer_.reset(new deadline_timer(io_service,seconds(timeOutCallHVal_)));
	AddTimer(CallHvalTimer_);

	timerDownloadPara_.reset(new deadline_timer(io_service,seconds(timeOutDownloadPara_)));
	AddTimer(timerDownloadPara_);

	timerBCurVer_.reset(new deadline_timer(io_service,seconds(timeOutBCurVer_)));
	AddTimer(timerBCurVer_);

	timerDcVer_.reset(new deadline_timer(io_service,seconds(timeOutDcVer_)));
	AddTimer(timerDcVer_);

	timerCallPm_.reset(new deadline_timer(io_service,seconds(timeOutCallPm_)));
	AddTimer(timerCallPm_);

	timerCallFileDisConect_.reset(new deadline_timer(io_service,seconds(timeOutCallFileDisConect_)));
	AddTimer(timerCallFileDisConect_);
}

void CS101_518::InitFileOpt()
{
	Total_NUM = 0;
	This_N0 = 0;
}

void CS101_518::handle_timerBCurVer(const boost::system::error_code& error,share_commpoint_ptr point)
{
	//std::cout<<"TimerBCurVer��ʱ... ..."<<std::endl;
	DataBase::stLine_BCurVerVal VerTimeOut;
	VerTimeOut.timeout_flag = 0x0a;
	if (!error)
	{
		if (point)
		{
			AddSendCmdVal(LINE_BVAL_VER_QYC,LINE_BVAL_VER_QYC_PRIORITY,point,VerTimeOut);
		}
	}
}

void CS101_518::handle_timerCallPm(const boost::system::error_code& error,share_commpoint_ptr point)
{
	//std::cout<<"TimerBCurVer��ʱ... ..."<<std::endl;
	DataBase::stPMPara CallPmTimeOut;
	CallPmTimeOut.timeout_flag = 0x0a;
	if (!error)
	{
		if (point)
		{
			AddSendCmdVal(CALL_PM_ANG_CON,CALL_PM_ANG_CON_PRIORITY,point,CallPmTimeOut);
		}
	}
}

void CS101_518::handle_timerCallFileDisConect(const boost::system::error_code& error,share_commpoint_ptr point)
{
	if (!error)
	{
		if (point)
		{
			DisconnectSubYkSig(point,false);
		}
	}
	ResetTimerCallFileDisConect(point);
}

void CS101_518::handle_timerDcVer(const boost::system::error_code& error,share_commpoint_ptr point)
{
	//std::cout<<"TimerBCurVer��ʱ... ..."<<std::endl;
	DataBase::stDCVerVal VerTimeOut;
	VerTimeOut.timeout_flag = 0x0a;
	if (!error)
	{
		if (point)
		{
			AddSendCmdVal(LINE_DCVAL_VER_SUCESS,LINE_DCVAL_VER_SUCESS_PRIORITY,point,VerTimeOut);
		}
	}
}

void CS101_518::handle_timerValVer(const boost::system::error_code& error,share_commpoint_ptr point)
{

	DataBase::stLine_Val VerTimeOut;
	VerTimeOut.timeout_flag = 0x0a;
	if (!error)
	{
		if (point)
		{
			AddSendCmdVal(LINE_VAL_VER_QYC,LINE_VAL_VER_QYC_PRIORITY,point,VerTimeOut);
		}
	}
}

void CS101_518::handle_timerReboot(const boost::system::error_code& error,share_commpoint_ptr point)
{
	// std::cout<<"��λ��ʱ����ʱ... "<<std::endl; 
	system("reboot");
}

void CS101_518::handle_timerCallHVal(const boost::system::error_code& error,share_commpoint_ptr point)
{
	// std::cout<<"�ٻ���ֵϵ����ʱ����ʱ... ..."<<std::endl; 
	DataBase::stLine_ValCoef  ValCoefTimeOut;
	ValCoefTimeOut.timeout_flag = 0x0a;
	if (!error)
	{
		if (point)
		{
			AddSendCmdVal(CALL_VALCOEF_CON,CALL_VALCOEF_CON_PRIORITY,point,ValCoefTimeOut);
		}
	}
}

void CS101_518::handle_timerDownloadPara(const boost::system::error_code& error,share_commpoint_ptr point)
{
	//std::cout<<"��װ������ʱ����ʱ... "<<std::endl; 
	char Val = 0x05;
	if (!error)
	{
		if (point)
		{
			AddWaitCmdVal(DOWNLOAD_PARA_CON,DOWNLOAD_PARA_CON_PRIORITY,point,Val);
		}
	}
}

void CS101_518::ResetTimerBCurVer(share_commpoint_ptr point,bool bContinue/* = false*/,unsigned short val/* = 0*/)
{
	//std::cout<<"��ʼ��ʼ��ResetTimerBCurVer... "<<std::endl;
	if (bContinue)
	{
		if (val == 0)
		{
			timerBCurVer_->expires_from_now(boost::posix_time::seconds(timeOutBCurVer_));
		}
		else
		{
			timerBCurVer_->expires_from_now(boost::posix_time::seconds(val));
		}

		timerBCurVer_->async_wait(boost::bind(&CS101_518::handle_timerBCurVer,this,boost::asio::placeholders::error,point));
	}
	else
	{
		timerBCurVer_->cancel();
	}
}

void CS101_518::ResetTimerCallPm(share_commpoint_ptr point,bool bContinue/* = false*/,unsigned short val/* = 0*/)
{
	//std::cout<<"��ʼ��ʼ��ResetTimerDcVer... "<<std::endl;
	if (bContinue)
	{
		if (val == 0)
		{
			timerCallPm_->expires_from_now(boost::posix_time::seconds(timeOutCallPm_));
		}
		else
		{
			timerCallPm_->expires_from_now(boost::posix_time::seconds(val));
		}

		timerCallPm_->async_wait(boost::bind(&CS101_518::handle_timerCallPm,this,boost::asio::placeholders::error,point));
	}
	else
	{
		timerCallPm_->cancel();
	}
}

void CS101_518::ResetTimerCallFileDisConect(share_commpoint_ptr point,bool bContinue/* = false*/,unsigned short val/* = 0*/)
{
	if (bContinue)
	{
		if (val == 0)
		{
			timerCallFileDisConect_->expires_from_now(boost::posix_time::seconds(timeOutCallFileDisConect_));
		}
		else
		{
			timerCallFileDisConect_->expires_from_now(boost::posix_time::seconds(val));
		}

		timerCallFileDisConect_->async_wait(boost::bind(&CS101_518::handle_timerCallFileDisConect,this,boost::asio::placeholders::error,point));
	}
	else
	{
		timerCallFileDisConect_->cancel();
	}
}

void CS101_518::ResetTimerDcVer(share_commpoint_ptr point,bool bContinue/* = false*/,unsigned short val/* = 0*/)
{
	//std::cout<<"��ʼ��ʼ��ResetTimerDcVer... "<<std::endl;
	if (bContinue)
	{
		if (val == 0)
		{
			timerDcVer_->expires_from_now(boost::posix_time::seconds(timeOutDcVer_));
		}
		else
		{
			timerDcVer_->expires_from_now(boost::posix_time::seconds(val));
		}

		timerDcVer_->async_wait(boost::bind(&CS101_518::handle_timerDcVer,this,boost::asio::placeholders::error,point));
	}
	else
	{
		timerDcVer_->cancel();
	}
}

void CS101_518::ResetTimerValVer(share_commpoint_ptr point,bool bContinue,unsigned short val)
{
	if (bContinue)
	{
		if (val == 0)
		{
			ValVerTimer_->expires_from_now(boost::posix_time::seconds(timeOutValVer_));
		}
		else
		{
			ValVerTimer_->expires_from_now(boost::posix_time::seconds(val));
		}
		ValVerTimer_->async_wait(boost::bind(&CS101_518::handle_timerValVer,this,boost::asio::placeholders::error,point));
	}
	else
	{
		ValVerTimer_->cancel();
	}
}

void  CS101_518::ResetTimerReboot(share_commpoint_ptr point,bool bContinue,unsigned short val)
{
	if (bContinue)
	{
		if (val == 0)
		{
			RebootTimer_->expires_from_now(boost::posix_time::seconds(timeOutReboot_));
		}
		else
		{
			RebootTimer_->expires_from_now(boost::posix_time::seconds(val));
		}
		RebootTimer_->async_wait(boost::bind(&CS101_518::handle_timerReboot,this,boost::asio::placeholders::error,point));
	}
	else
	{
		RebootTimer_->cancel();
	}
}

void CS101_518::ResetTimerCallHVal(share_commpoint_ptr point,bool bContinue,unsigned short val)
{
	if (bContinue)
	{
		if (val == 0)
		{
			CallHvalTimer_->expires_from_now(boost::posix_time::seconds(timeOutCallHVal_));
		}
		else
		{
			CallHvalTimer_->expires_from_now(boost::posix_time::seconds(val));
		}
		CallHvalTimer_->async_wait(boost::bind(&CS101_518::handle_timerCallHVal,this,boost::asio::placeholders::error,point));
	}
	else
	{
		CallHvalTimer_->cancel();
	}
}

void CS101_518::ResetTimerDowloadPara(share_commpoint_ptr point,bool bContinue /*= false*/,unsigned short val /*= 0*/)
{
	if (bContinue)
	{
		if (val == 0)
		{
			timerDownloadPara_->expires_from_now(boost::posix_time::seconds(timeOutDownloadPara_));
		}
		else
		{
			timerDownloadPara_->expires_from_now(boost::posix_time::seconds(val));
		}
		timerDownloadPara_->async_wait(boost::bind(&CS101_518::handle_timerDownloadPara,this,boost::asio::placeholders::error,point));
	}
	else
	{
		timerDownloadPara_->cancel();
	}
}

//int CS101_518::setTimeOutReboot(unsigned short val)
//{
//	if (val <= 0 || val > 300)
//	{
//		return -1;
//	}
//
//	timeOutReboot_ = val;
//
//	return 0;
//}


};//namespace Protocol



