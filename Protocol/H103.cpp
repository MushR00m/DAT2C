#include <boost/bind.hpp>
#include <boost/scoped_array.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include "H103.h"
#include "../PublicSupport/Dat2cPublicAPI.h"
#include "../DataBase/Terminal.h"
#include "../DataBase/YxPoint.h"
#include "../DataBase/YkPoint.h"

namespace Protocol {

const std::string strDefaultCfg = "H103Cfg.xml";
size_t CH103::H103ObjectCounter_ = 1;

//���101��Լ��YK������
const unsigned char DYK_TYPE_OPEN = 0x01;
const unsigned char DYK_TYPE_CLOSE = 0x02;

//���103��Լ�Ŀ�������
const unsigned char DIR_PRM = 0x40;
const unsigned char ACT_FCB = 0x20;
const unsigned char NACK_FCB = 0;
const unsigned char ACT_FCV = 0x10;
const unsigned char NACK_FCV = 0;
const unsigned char ACT_ACD = 0x20;
const unsigned char NACK_ACD = 0;

//103��Լ�����ͱ�ʶ����
const unsigned char M_TM_TA_3   = 0x01;                //��1�� ��=��ʱ��ı���
const unsigned char M_TMR_TA_3  = 0x02;                //��2�� ��=�������ʱ��Ĵ�ʱ��ı���
const unsigned char M_MEI_NA_3  = 0x03;                //��3�� ��=����ֵI
const unsigned char M_TME_TA_3  = 0x04;                //��4�� ��=�������ʱ��Ĵ�ʱ��ı���ֵ
const unsigned char M_IR_NA_3   = 0x05;                //��5�� ��=��ʶ  M_IRC_NA_3��M_IRF_NA_3��M_IRS_NA_3��M_PO_NA_3
const unsigned char C_SYN_TA_3  = 0x06;                //��6�� ��=ʱ��ͬ��
const unsigned char M_SYN_TA_3  = 0x06;                //��6�� ��=ʱ��ͬ��
const unsigned char C_IGI_NA_3  = 0x07;                //��7�� ��=�ܲ�ѯ(���ٻ�)����
const unsigned char M_TGI_NA_3  = 0x08;                //��8�� ��=�ܲ�ѯ(���ٻ�)��ֹ
const unsigned char M_MEII_NA_3 = 0x09;                //��9�� ��=����ֵ��
const unsigned char C_GD_NA_3   = 0x0a;                //��10����=ͨ�÷�������
const unsigned char M_GD_NA_3   = 0x0a;                //��10����=ͨ�÷�������
const unsigned char M_GI_NA_3   = 0x0b;                //��11����=ͨ�÷����ʶ
const unsigned char M_MEIII_NA_3= 0x0f;                // <15> ��=����ֵIII
const unsigned char C_GRC_NA_3  = 0x14;                //��20����=һ������
const unsigned char C_GC_NA_3   = 0x15;                //��21����=ͨ�÷�������
const unsigned char M_LRD_TA_3  = 0x17;                //��23����=����¼���Ŷ���
const unsigned char C_ODT_NA_3  = 0x18;                //��24����=�Ŷ����ݴ��������
const unsigned char C_ADT_NA_3  = 0x19;                //��25����=�Ŷ����ݴ�����Ͽ�
const unsigned char M_RTD_TA_3  = 0x1a;                //��26����=�Ŷ����ݴ���׼������
const unsigned char M_RTC_NA_3  = 0x1b;                //��27����=����¼��ͨ������׼������
const unsigned char M_RTT_NA_3  = 0x1c;                //��28����=����־��״̬��λ����׼������
const unsigned char M_TDT_TA_3  = 0x1d;                //��29����=���ʹ���־��״̬��λ
const unsigned char M_TDN_NA_3  = 0x1e;                //��30����=�����Ŷ�ֵ
const unsigned char M_EOT_NA_3  = 0x1f;                //��31����=���ͽ���

const unsigned char M_IT_NA_3   = 0x24;                // <36> ��=������
const unsigned char M_IT_TA_3   = 0x25;                // <37> ��=��ʱ���������
const unsigned char M_SP_NA_3   = 0x28;                // <40> ��=COS
const unsigned char M_SP_TA_3   = 0x29;                // <41> ��=SOE
const unsigned char M_DP_NA_3   = 0x2a;                // <42> ��=˫��COS
const unsigned char M_DP_TA_3   = 0x2b;                // <43> ��=˫��SOE
const unsigned char ASDU44      = 0x2c;                // <44> ��=ȫң������ ң���ַ�ʽ
const unsigned char ASDU46      = 0x2e;                // <46> ��=ȫ˫��ң�� ң���ַ�ʽ
const unsigned char ASDU50      = 0x32;                // <50> ��=ȫң��
const unsigned char ASDU51      = 0x33;                // <51> : =�仯ң��
const unsigned char ASDU64      = 0x40;                // <64> ��=ң��
const unsigned char ASDU88      = 0x58;                // <88> ��=�ٻ������

//103��Լ����ԭ���ʶ����
const unsigned char trans_spont = 0x01;                //��1�� ��=�Է�(ͻ��)
const unsigned char trans_cyc = 0x02;                  //��2�� ��=ѭ��
const unsigned char trans_reset_fcb = 0x03;            //��3�� ��=��λ֡����λ(FCB)
const unsigned char trans_reset_cu = 0x04;             //��4�� ��=��λͨ�ŵ�Ԫ(CU)
const unsigned char trans_init = 0x05;                 //��5�� ��=����/��������
const unsigned char trans_switch_on = 0x06;            //��6�� ��=��Դ����
const unsigned char trans_test_mod = 0x07;             //��7�� ��=����ģʽ
const unsigned char trans_cs = 0x08;                   //��8�� ��=ʱ��ͬ��
const unsigned char trans_gi = 0x09;                   //��9�� ��=�ܲ�ѯ(���ٻ�)
const unsigned char trans_gi_term= 0x0a;               //��10����=�ܲ�ѯ(���ٻ�)��ֹ	
const unsigned char trans_operate_loc = 0x0b;          //��11����=���ز�����ͬH101�еĵ�����������ķ�����Ϣ
const unsigned char trans_operate_rem = 0x0c;          //��12����=Զ��������ͬH101�е�Զ����������ķ�����Ϣ
const unsigned char trans_com_positive_con = 0x14;     //��20����=����Ŀ϶��Ͽ�
const unsigned char trans_com_negate_con = 0x15;       //��21����=����ķ��Ͽ�
const unsigned char trans_disturbance_data = 0x1f;     //��31����=�Ŷ����ݵĴ���
const unsigned char trans_gwc_positive_con = 0x28;     //��40����=ͨ�÷���д����Ŀ϶��Ͽ�
const unsigned char trans_gwc_negate_con = 0x29;       //��41����=ͨ�÷���д����ķ��Ͽ�
const unsigned char trans_grc_v_con = 0x2a;            //��42����=��ͨ�÷����������Ч������Ӧ
const unsigned char trans_grc_iv_con = 0x2b;           //��43����=��ͨ�÷����������Ч������Ӧ
const unsigned char trans_gw_con = 0x2c;               //��44����=ͨ�÷���дȷ��

//���103��Լ�Ĺ����붨��-���ӷ���
const unsigned char M_CON_NA_3 = 0x00;                 // <0> ��=ȷ��֡     ȷ��
const unsigned char M_BY_NA_3 = 0x01;                  // <1> ��=ȷ��֡     ��·æ��δ�յ�����
const unsigned char M_AV_NA_3 = 0x08;                  // <8> ��=��Ӧ֡     ��������Ӧ����֡
const unsigned char M_NV_NA_3 = 0x09;                  // <0> ��=��Ӧ֡     �����ٻ�������
const unsigned char M_LKR_NA_3 = 0x0b;                 // <11> ��=ȷ��֡    ��·״̬

//���101��Լ�Ĺ����붨��-���Ʒ���
const unsigned char C_RCU_NA_3 = 0x00;                 // <0> ��=����/ȷ��֡   ��λͨ�ŵ�Ԫ��CU��
const unsigned char C_REQ_NA_3 = 0x03;                 // <3> ��=����/ȷ��֡   ��������
const unsigned char C_NEQ_NA_3 = 0x04;                 // <4> ��=����/�޻ش�֡ �������� 
const unsigned char C_RFB_NA_3 = 0x07;                 // <7> ��=����/ȷ��֡   ��λ֡����λ��FCB��
const unsigned char C_PLK_NA_3 = 0x09;
const unsigned char C_PL1_NA_3 = 0x0a;                 // <10> ��=����/��Ӧ     �ٻ�1���û�����
const unsigned char C_PL2_NA_3 = 0x0b;                 // <11> ��=����/��Ӧ     �ٻ�2���û�����

//���103��Լ�Ĺ�������
const unsigned char fun_DistanceP = 0x80;              //<128> ��=���뱣�������ݷ�Χ��  t(z)	���뱣��	Distance protection 
const unsigned char fun_OvercurrentP = 0xa0;           //<160> ��=�������������ݷ�Χ��  I	���������	Overcurrent protection
const unsigned char fun_TransformerDP = 0xb0;          //<176> ��=��ѹ������������ݷ�Χ��  ��IT	��ѹ�������	Transformer differential protection
const unsigned char fun_LineDP = 0xc0;                 //<192> ��=��·����������ݷ�Χ��  ��IL	��·�����	Line differential protection
const unsigned char fun_GEN = 0xfe;                    //<254> ��=ͨ�÷��๦��
const unsigned char fun_GLB = 0xff;                    //<255> ��=ȫ�ֹ�������

//���103��Լ����Ϣ���
const unsigned char inf_reset_fcb = 0x02;              //��3����=��λ֡����λ(FCB)
const unsigned char inf_reset_cu = 0x03;               //��4����=��λͨ�ŵ�Ԫ(CU)
const unsigned char inf_init = 0x04;                   //��5����=����/��������
const unsigned char inf_switch_on = 0x05;              //��6����=��Դ����

//���103��Լ��ͨ�÷������ݱ�ʶ


//���103��Լ�ı��ı�ʶ
const unsigned char EnableISQ = 0x80;
const unsigned char DisableISQ = 0x00;
const unsigned char QOI = 0x14;

const unsigned char NormalType = 1;
const unsigned char NoShortFrameType = 2;

//xml�����ļ���㶨��
#define strProtocolType "ProtocolType"
#define strNormalType "NormalType"
#define strNoShortFrameType "NoShortFrameType"

CH103::CH103(boost::asio::io_service & io_service)
			:CProtocol(io_service)
{
	bActiveRepeatFrame_ = true;
	SynCharNum_ = 5;
	h103Type_ = NormalType;

	InitObjectIndex();
	InitDefaultStartAddr();
	InitDefaultFrameElem();
	InitDefaultTimer(io_service);

	LoadXmlCfg(strDefaultCfg);
	//SaveXmlCfg(strDefaultCfg);
}

CH103::~CH103(void)
{
	H103ObjectCounter_--;
}

int CH103::LoadXmlCfg(std::string filename)
{
	FileSystem::CMarkup xml;

	if (!xml.Load(filename))
	{
		std::cout<<"δ����filename�����ļ�"<<filename<<"��"<<std::endl;

		return -1;
	}

	xml.ResetMainPos();
	xml.FindElem();  //root strProtocolRoot
	xml.IntoElem();  //enter strProtocolRoot

	CProtocol::LoadXmlCfg(xml);

	xml.ResetMainPos();
	if (xml.FindElem(strProtocolType))
	{
		std::string strTmp = xml.GetData();
		boost::algorithm::trim(strTmp);
		if (boost::iequals(strNormalType,strTmp))
		{
			h103Type_ = NormalType;
		}
		else if (boost::iequals(strNoShortFrameType,strTmp))
		{
			h103Type_ = NoShortFrameType;
		}
	}

	xml.ResetMainPos();
	if (xml.FindElem(strInfoAddr))
	{
		xml.IntoElem();

		xml.ResetMainPos();
		if (xml.FindElem(strSYxStartAddr))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short addr = boost::lexical_cast<unsigned short>(strTmp);
				setSYX_START_ADDR(addr);
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setSYX_START_ADDR(DEFAULT_SYX_START_ADDR);
			}
		}

		xml.ResetMainPos();
		if (xml.FindElem(strDYxStartAddr))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short addr = boost::lexical_cast<unsigned short>(strTmp);
				setDYX_START_ADDR(addr);
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setDYX_START_ADDR(DEFAULT_DYX_START_ADDR);
			}
		}

		xml.ResetMainPos();
		if (xml.FindElem(strYcStartAddr))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short addr = boost::lexical_cast<unsigned short>(strTmp);
				setYC_START_ADDR(addr);
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setYC_START_ADDR(DEFAULT_YC_START_ADDR);
			}
		}

		xml.ResetMainPos();
		if (xml.FindElem(strSYkStartAddr))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short addr = boost::lexical_cast<unsigned short>(strTmp);
				setSYK_START_ADDR(addr);
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setSYK_START_ADDR(DEFAULT_SYK_START_ADDR);
			}
		}

		xml.ResetMainPos();
		if (xml.FindElem(strDYkStartAddr))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short addr = boost::lexical_cast<unsigned short>(strTmp);
				setDYK_START_ADDR(addr);
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setDYK_START_ADDR(DEFAULT_DYK_START_ADDR);
			}
		}

		xml.ResetMainPos();
		if (xml.FindElem(strYmStartAddr))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short addr = boost::lexical_cast<unsigned short>(strTmp);
				setYM_START_ADDR(addr);
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setYM_START_ADDR(DEFAULT_YM_START_ADDR);
			}
		}

		xml.OutOfElem();
	}

	xml.ResetMainPos();
	if (xml.FindElem(strFrameElemLength))
	{
		xml.IntoElem();

		xml.ResetMainPos();
		if (xml.FindElem(strFrameTypeLength))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short len = boost::lexical_cast<unsigned short>(strTmp);
				setFrameTypeLength(len);
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setFrameTypeLength(DEFAULT_FrameTypeLength);
			}
		}

		xml.ResetMainPos();
		if (xml.FindElem(strInfoNumLength))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short len = boost::lexical_cast<unsigned short>(strTmp);
				setInfoNumLength(len);
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setInfoNumLength(DEFAULT_InfoNumLength);
			}
		}

		xml.ResetMainPos();
		if (xml.FindElem(strTransReasonLength))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short len = boost::lexical_cast<unsigned short>(strTmp);
				setTransReasonLength(len);
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setTransReasonLength(DEFAULT_TransReasonLength);
			}
		}

		xml.ResetMainPos();
		if (xml.FindElem(strAsduAddrLength))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short len = boost::lexical_cast<unsigned short>(strTmp);
				setAsduAddrLength(len);
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setAsduAddrLength(DEFAULT_AsduAddrLength);
			}
		}

		xml.ResetMainPos();
		if (xml.FindElem(strInfoAddrLength))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short len = boost::lexical_cast<unsigned short>(strTmp);
				setInfoAddrLength(len);
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setInfoAddrLength(DEFAULT_InfoAddrLength);
			}
		}

		xml.ResetMainPos();
		if (xml.FindElem(strFunTypeLength))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short len = boost::lexical_cast<unsigned short>(strTmp);
				setFunTypeLength(len);
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setFunTypeLength(DEFAULT_FunTypeLength);
			}
		}

		xml.OutOfElem();
	}

	xml.ResetMainPos();
	if (xml.FindElem(strTimer))
	{
		xml.IntoElem();

		xml.ResetMainPos();
		if (xml.FindElem(strTimeOutQueryUnActivePoint))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short timeout = boost::lexical_cast<unsigned short>(strTmp);
				if(!setTimeOutQueryUnActivePoint(timeout))
				{
					bUseTimeOutQueryUnActivePoint_ = true;
				}
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setTimeOutQueryUnActivePoint(DEFAULT_timeOutQueryUnActivePoint);
				bUseTimeOutQueryUnActivePoint_ = false;
			}
		}

		xml.ResetMainPos();
		if (xml.FindElem(strTimeOutRequireLink))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short timeout = boost::lexical_cast<unsigned short>(strTmp);
				setTimeOutRequrieLink(timeout);
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setTimeOutRequrieLink(DEFAULT_timeOutRequireLink);
			}
		}

		xml.ResetMainPos();
		if (xml.FindElem(strTimeOutCallAllData))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short timeout = boost::lexical_cast<unsigned short>(strTmp);
				setTimeOutCallAllData(timeout);
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setTimeOutCallAllData(DEFAULT_timeOutCallAllData);
			}
		}

		xml.ResetMainPos();
		if (xml.FindElem(strTimeOutSynTime))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short timeout = boost::lexical_cast<unsigned short>(strTmp);
				setTimeOutSynTime(timeout);
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setTimeOutSynTime(DEFAULT_timeOutSynTime);
			}
		}

		xml.ResetMainPos();
		if (xml.FindElem(strTimeOutHeartFrame))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short timeout = boost::lexical_cast<unsigned short>(strTmp);
				setTimeOutHeartFrame(timeout);
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setTimeOutHeartFrame(DEFAULT_timeOutHeartFrame);
			}
		}

		xml.ResetMainPos();
		if (xml.FindElem(strTimeOutYkSel))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short timeout = boost::lexical_cast<unsigned short>(strTmp);
				setTimeOutYkSel(timeout);
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setTimeOutYkSel(DEFAULT_timeOutYkSel);
			}
		}

		xml.ResetMainPos();
		if (xml.FindElem(strTimeOutYkExe))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short timeout = boost::lexical_cast<unsigned short>(strTmp);
				setTimeOutYkExe(timeout);
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setTimeOutYkExe(DEFAULT_timeOutYkExe);
			}
		}

		xml.ResetMainPos();
		if (xml.FindElem(strTimeOutYkCancel))
		{
			std::string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			try
			{
				unsigned short timeout = boost::lexical_cast<unsigned short>(strTmp);
				setTimeOutYkCancel(timeout);
			}
			catch(boost::bad_lexical_cast& e)
			{
				std::ostringstream ostr;
				ostr<<e.what();

				setTimeOutYkCancel(DEFAULT_timeOutYkCancel);
			}
		}

		xml.OutOfElem();
	}

	xml.OutOfElem(); //out strProtocolRoot

	return 0;
}

void CH103::SaveXmlCfg(std::string filename)
{
	FileSystem::CMarkup xml;
	xml.SetDoc(strXmlHead);
	xml.SetDoc(strProtocolXsl);

	xml.AddElem(strProtocolRoot);
	xml.IntoElem();

	CProtocol::SaveXmlCfg(xml);

	switch(h103Type_)
	{
	case NormalType:
		xml.AddElem(strProtocolType,strNormalType);
		break;

	case NoShortFrameType:
		xml.AddElem(strProtocolType,strNoShortFrameType);
		break;

	default:
		break;
	}

	xml.AddElem(strInfoAddr);
	bool bSave = false;
	xml.IntoElem();

	if (SYX_START_ADDR_ != DEFAULT_SYX_START_ADDR)
	{
		xml.AddElem(strSYxStartAddr,SYX_START_ADDR_);
		bSave = true;
	}

	if (DYX_START_ADDR_ != DEFAULT_DYX_START_ADDR)
	{
		xml.AddElem(strDYxStartAddr,DYX_START_ADDR_);
		bSave = true;
	}

	if (YC_START_ADDR_ != DEFAULT_YC_START_ADDR)
	{
		xml.AddElem(strYcStartAddr,YC_START_ADDR_);
		bSave = true;
	}

	if (SYK_START_ADDR_ != DEFAULT_SYK_START_ADDR)
	{
		xml.AddElem(strSYkStartAddr,SYK_START_ADDR_);
		bSave = true;
	}

	if (DYK_START_ADDR_ != DEFAULT_DYK_START_ADDR)
	{
		xml.AddElem(strDYkStartAddr,DYK_START_ADDR_);
		bSave = true;
	}

	if (YM_START_ADDR_ != DEFAULT_YM_START_ADDR)
	{
		xml.AddElem(strYmStartAddr,YM_START_ADDR_);
		bSave = true;
	}

	xml.OutOfElem();
	if (!bSave)
	{
		xml.RemoveElem();
	}

	xml.AddElem(strFrameElemLength);
	bSave = false;
	xml.IntoElem();

	if (FrameTypeLength_ != DEFAULT_FrameTypeLength)
	{
		xml.AddElem(strFrameElemLength,FrameTypeLength_);
		bSave = true;
	}

	if (InfoNumLength_ != DEFAULT_InfoNumLength)
	{
		xml.AddElem(strInfoNumLength,InfoNumLength_);
		bSave = true;
	}

	if (TransReasonLength_ != DEFAULT_TransReasonLength)
	{
		xml.AddElem(strTransReasonLength,TransReasonLength_);
		bSave = true;
	}

	if (AsduAddrLength_ != DEFAULT_AsduAddrLength)
	{
		xml.AddElem(strAsduAddrLength,AsduAddrLength_);
		bSave = true;
	}

	if (InfoAddrLength_ != DEFAULT_InfoAddrLength)
	{
		xml.AddElem(strInfoAddrLength,InfoAddrLength_);
		bSave = true;
	}

	if (FunTypeLength_ != DEFAULT_FunTypeLength)
	{
		xml.AddElem(strFunTypeLength,FunTypeLength_);
		bSave = true;
	}

	xml.OutOfElem();
	if (!bSave)
	{
		xml.RemoveElem();
	}

	xml.AddElem(strTimer);
	bSave = false;
	xml.IntoElem();

	if (bUseTimeOutQueryUnActivePoint_ && (timeOutQueryUnActivePoint_ != DEFAULT_timeOutQueryUnActivePoint))
	{
		xml.AddElem(strTimeOutQueryUnActivePoint,timeOutQueryUnActivePoint_);
		bSave = true;
	}

	if (timeOutRequireLink_ != DEFAULT_timeOutRequireLink)
	{
		xml.AddElem(strTimeOutRequireLink,timeOutRequireLink_);
		bSave = true;
	}

	if (timeOutCallAllData_ != DEFAULT_timeOutCallAllData)
	{
		xml.AddElem(strTimeOutCallAllData,timeOutCallAllData_);
		bSave = true;
	}

	if (timeOutCallAllDD_ != DEFAULT_timeOutCallAllDD)
	{
		xml.AddElem(strTimeOutCallAllDD,timeOutCallAllDD_);
		bSave = true;
	}

	if (timeOutSynTime_ != DEFAULT_timeOutSynTime)
	{
		xml.AddElem(strTimeOutSynTime,timeOutSynTime_);
		bSave = true;
	}

	if (timeOutHeartFrame_ != DEFAULT_timeOutHeartFrame)
	{
		xml.AddElem(strTimeOutHeartFrame,timeOutHeartFrame_);
		bSave = true;
	}

	if (timeOutYkSel_ != DEFAULT_timeOutYkSel)
	{
		xml.AddElem(strTimeOutYkSel,timeOutYkSel_);
		bSave = true;
	}

	if (timeOutYkExe_ != DEFAULT_timeOutYkExe)
	{
		xml.AddElem(strTimeOutYkExe,timeOutYkExe_);
		bSave = true;
	}

	if (timeOutYkCancel_ != DEFAULT_timeOutYkCancel)
	{
		xml.AddElem(strTimeOutYkCancel,timeOutYkCancel_);
		bSave = true;
	}

	xml.OutOfElem();
	if (!bSave)
	{
		xml.RemoveElem();
	}

	xml.OutOfElem();

	xml.Save(filename);
}

int CH103::CheckFrameHead(unsigned char * buf,size_t & exceptedBytes)
{
	if ((buf[0] == 0x68) && (buf[3] == 0x68) && (buf[1] == buf[2]))
	{
		exceptedBytes = buf[1] + 6;
		return 0;
	}
	else if (buf[0] == 0x10)
	{
		exceptedBytes = 4 + AsduAddrLength_;
		return 0;
	}

	return -1;
}

int CH103::CheckFrameTail(unsigned char * buf,size_t exceptedBytes)
{
	size_t sum = 0;

	if ((exceptedBytes == 5) && (buf[0] == 0x10))
	{
		sum = CalcCheckSumByte(&buf[1],exceptedBytes - 3);
	}
	else if ((exceptedBytes > 5) && (buf[0] == 0x68) && (buf[3] == 0x68))
	{
		sum = CalcCheckSumByte(&buf[4],exceptedBytes - 6);
	}

	if ((buf[exceptedBytes -1] == 0x16) && (buf[exceptedBytes - 2] == sum))
	{
		//std::ostringstream ostr;//zyq
		//ostr<<"У����뱨��β�����ȷsum="<<sum<<std::endl;
		//AddStatusLogWithSynT(ostr.str());
//		AddStatusLogWithSynT("У��ͱ���β�����ȷ;\n");//zyq
		return 0;
	}
	/*std::ostringstream ostr;//zyq
	ostr.str("");
	ostr<<"У����뱨��β������sum="<<sum<<std::endl;
	AddStatusLogWithSynT(ostr.str());*/

	return -1;
}

int CH103::ParseFrameBody(unsigned char * buf,size_t exceptedBytes)
{
	int ret = 0;

	unsigned char funcType = buf[0];

	int Addr = getAddrByRecvFrame(buf);
	std::ostringstream ostr;//zyq
	/*ostr.str("");
	ostr<<"ParseFrameBody��addr, "<<Addr<<std::endl;
	AddStatusLogWithSynT(ostr.str());*/

	if (Addr < 0)
	{
		return Addr;
	}

	int terminalIndex = getCommPointIndexByAddrCommType(TERMINAL_NODE,Addr);
	//std::ostringstream ostr;//zyq
	/*ostr.str("");
	ostr<<"ParseFrameBody��terminalIndex=getCommPointIndexByAddrCommType=, "<<terminalIndex<<std::endl;
	AddStatusLogWithSynT(ostr.str());*/

	share_terminal_ptr terminalPtr;
	if (terminalIndex >= 0)
	{
		setLastRecvPointIndex(terminalIndex);
		//AddStatusLogWithSynT("��ParseFrameBody����ִ�е�setLastRecvPointIndex(terminalIndex);\n");//zyq

		terminalPtr = boost::dynamic_pointer_cast<DataBase::CTerminal>(getCommPoint(terminalIndex).lock());
	}

	if (!terminalPtr)
	{
		std::ostringstream ostr;
		ostr.str("");
		ostr<<"H103��Լ���ܸ��ݽ��ձ����еĵ�ַƥ��terminal ptr,��֡���Ľ����ᱻ������"<<std::endl;
		AddStatusLogWithSynT(ostr.str());

		return -1;
	}

	switch (funcType)
	{
	case 0x10:
		ret = ParseShortFrame(buf,terminalPtr);
		break;

	case 0x68:
		ret = ParseLongFrame(buf,terminalPtr);
		break;

	default:
		{
			std::ostringstream ostr;
			ostr.str("");
			ostr<<"δ����ı���ͷ,buf[0] = "<<funcType<<std::endl;
			AddStatusLogWithSynT(ostr.str());
		}
		break;
	}

	if (ret < 0)
	{
		ostr.str("");
		ostr<<"ParseFrameBody����return ret, ParseShortFrame��ParseLongFrame����ֵ����"<<ret<<std::endl;
		AddStatusLogWithSynT(ostr.str());

		return ret;
	}
	ostr.str("");
	ostr<<"ParseFrameBody��������ֵ��return terminalIndex, "<<terminalIndex<<std::endl;
	AddStatusLogWithSynT(ostr.str());

	return terminalIndex;

}

int CH103::getFCB(share_terminal_ptr terminalPtr)
{
	if(terminalPtr->getCurFcbFlag())
	{
		return ACT_FCB;
	}
	else
	{
		return NACK_FCB;
	}
}

int CH103::getAddrByRecvFrame(unsigned char * buf)
{
	unsigned char funcType = buf[0];
	unsigned char dirPrmByte = 0;
	int addr = -1;
	switch (funcType)
	{
	case 0x10:
		dirPrmByte = buf[1];
		addr = BufToVal(&buf[2],AsduAddrLength_);
		break;

	case 0x68:
		dirPrmByte = buf[4];
		if (BufToVal(&buf[5],AsduAddrLength_) == BufToVal(&buf[AsduAddrLocation_],AsduAddrLength_))
		{
			addr = BufToVal(&buf[AsduAddrLocation_],AsduAddrLength_);
		}
		else
		{
			addr = -1;
		}

		break;

	default:
		addr = -1;
		break;
	}

	if ((dirPrmByte & DIR_PRM) == DIR_PRM) //��鱨�ķ���λ��־
	{
		return -1;
	}

	return addr;
}

int CH103::AssembleFrameHead(size_t bufIndex, unsigned char * buf, CCmd & cmd)
{
	size_t count = bufIndex;

	switch (cmd.getCmdType())
	{
	case REQUIRE_LINK:
		buf[count++] = 0x10;
		break;

	case RESET_LINK:
		buf[count++] = 0x10;
		break;

	case CALL_PRIMARY_DATA:
		buf[count++] = 0x10;
		break;

	case CALL_SECONDARY_DATA:
		buf[count++] = 0x10;
		break;

	default:
		buf[count++] = 0x68;
		buf[count++] = 0;
		buf[count++] = 0;
		buf[count++] = 0x68;
		break;
	}

	return count - bufIndex;
}

int CH103::AssembleFrameBody(size_t bufIndex, unsigned char * buf, CCmd & cmd)
{
	int bytesAssemble = 0;

	share_terminal_ptr terminalPtr;

	if (cmd.getCommPoint())
	{
		if (cmd.getCommPoint()->getCommPointType() == TERMINAL_NODE)
		{
			terminalPtr = boost::dynamic_pointer_cast<DataBase::CTerminal>(cmd.getCommPoint());
		}
	}

	if (!terminalPtr)
	{
		std::ostringstream ostr;
		ostr.str("");
		ostr<<"H103��Լ���ܴӷ��������л��terminal ptr��cmdtype = "<<cmd.getCmdType()<<"�����������ᱻ���͡�"<<std::endl;
		AddStatusLogWithSynT(ostr.str());

		return -1;
	}

	switch (cmd.getCmdType())
	{
	case CALL_PRIMARY_DATA:
		if (h103Type_ != NoShortFrameType)
		{
			bytesAssemble = AssemblePrimaryData(bufIndex,buf,terminalPtr);
		}
		else
		{
			bytesAssemble = -1;
		}
		
		if (bytesAssemble > 0)
		{
			setWaitingForAnswer(cmd.getCommPoint());//����Ҫ�ظ���־
		}
		break;

	case CALL_SECONDARY_DATA:
		if (h103Type_ != NoShortFrameType)
		{
			bytesAssemble = AssembleSecondaryData(bufIndex,buf,terminalPtr);
		}
		else
		{
			bytesAssemble = -1;
		}
		
		if (bytesAssemble > 0)
		{
			setWaitingForAnswer(cmd.getCommPoint());//����Ҫ�ظ���־
			ResetTimerHeartFrame(terminalPtr,true);
		}
		break;

	case REQUIRE_LINK:
		if (h103Type_ != NoShortFrameType)
		{
			bytesAssemble = AssembleRequireLink(bufIndex,buf,terminalPtr);
		}
		else
		{
			bytesAssemble = -1;
		}
		
		if (bytesAssemble > 0)
		{
			setWaitingForAnswer(cmd.getCommPoint());//����Ҫ�ظ���־
			ResetTimerResetLink(terminalPtr,true);
		}
		break;

	case RESET_LINK:
		if (h103Type_ != NoShortFrameType)
		{
			bytesAssemble = AssembleResetLink(bufIndex,buf,terminalPtr);
		}
		else
		{
			bytesAssemble = -1;
		}
		
		if (bytesAssemble > 0)
		{
			setWaitingForAnswer(cmd.getCommPoint());//����Ҫ�ظ���־
			ResetTimerResetLink(terminalPtr,true);
		}
		break;

	case CALL_ALL_DATA_ACT:
		bytesAssemble = AssembleCallAllData(bufIndex,buf,terminalPtr);
		if (bytesAssemble > 0)
		{
			setWaitingForAnswer(cmd.getCommPoint());//����Ҫ�ظ���־
			ResetTimerCallAllData(terminalPtr,true);
		}
		break;

	case CALL_ALL_DD_ACT:
		bytesAssemble = AssembleCallAllDD(bufIndex,buf,terminalPtr);
		if (bytesAssemble > 0)
		{
			setWaitingForAnswer(cmd.getCommPoint());//����Ҫ�ظ���־
			ResetTimerCallAllDD(terminalPtr,true);
		}
		break;

	case SYN_TIME_ACT:
		bytesAssemble = AssembleSynTime(bufIndex,buf,terminalPtr,boost::posix_time::microsec_clock::local_time());
		if (bytesAssemble > 0)
		{
			ResetTimerSynTime(terminalPtr,true);
		}
		break;

	case YK_SEL_ACT:
		{
			int yk_no;
			try
			{
				yk_no = boost::any_cast<int>(cmd.getVal());
			}
			catch(const boost::bad_any_cast & e)
			{
				std::ostringstream ostr;
				ostr.str("");
				ostr<<"ң��ѡ�������ң�ص�Ų����Ƿ���"<<e.what()<<std::endl;
				AddStatusLogWithSynT(ostr.str());
				return -1;
			}

			typeYktype yk_type = DataBase::YkOtherType;
			try
			{
				yk_type = terminalPtr->getYkType(yk_no);
			}
			catch(PublicSupport::dat2def_exception & err)
			{
				std::ostringstream ostr;
				ostr.str("");
				ostr<<"ң��ѡ�������ң�ص�Ų�������"<<err.what()<<std::endl;
				AddStatusLogWithSynT(ostr.str());
				return -1;
			}

			if (yk_type == DataBase::YkClose)
			{
				if (terminalPtr->getbHYkDouble(yk_no))
				{
					bytesAssemble = AssembleDoubleYKSel(bufIndex,buf,terminalPtr,yk_no,DYK_TYPE_CLOSE);
				}
				else
				{
					AddStatusLogWithSynT("�޷������ң������\n");
				}
			}
			else if (yk_type == DataBase::YkOpen)
			{
				if (terminalPtr->getbHYkDouble(yk_no))
				{
					bytesAssemble = AssembleDoubleYKSel(bufIndex,buf,terminalPtr,yk_no,DYK_TYPE_OPEN);
				}
				else
				{
					AddStatusLogWithSynT("�޷������ң������\n");
				}
			}
			else
			{
				AddStatusLogWithSynT("ң��ѡ�������ң�����Ͳ����Ƿ���\n");
				return -1;
			}

			if (bytesAssemble > 0)
			{
				//if (!DataBase::CYkPoint::CheckYkStatusDevelopWithSel(terminalPtr->getYkStatus(yk_no),DataBase::YkSelSend))
				if((terminalPtr->loadYkPointPtr(yk_no))->SendSelEvent())
				{
					//std::ostringstream ostr;
					//ostr<<"curStatus:"<<(int)terminalPtr->getYkStatus(yk_no)<<"NextStatus:"<<(int)DataBase::YkSelSend<<std::endl;
					//AddStatusLogWithSynT(ostr.str());
					AddStatusLogWithSynT("����ң��ѡ���������ң�ص�ǰң��״̬�����ϣ��˳������͸����\n");
					return -1;
				}
				//terminalPtr->setYkStatus(yk_no,DataBase::YkSelSend);

				setWaitingForAnswer(cmd.getCommPoint());//����Ҫ�ظ���־
				ResetTimerYkSel(terminalPtr,yk_no,true);
			}
		}
		break;

	case YK_EXE_ACT:
		{
			int yk_no;
			try
			{
				yk_no = boost::any_cast<int>(cmd.getVal());
			}
			catch(const boost::bad_any_cast & e)
			{
				std::ostringstream ostr;
				ostr.str("");
				ostr<<"ң��ִ�������ң�ص�Ų����Ƿ���"<<e.what()<<std::endl;
				AddStatusLogWithSynT(ostr.str());
				return -1;
			}

			typeYktype yk_type = DataBase::YkOtherType;
			try
			{
				yk_type = terminalPtr->getYkType(yk_no);
			}
			catch(PublicSupport::dat2def_exception & err)
			{
				std::ostringstream ostr;
				ostr.str("");
				ostr<<"ң��ִ�������ң�ص�Ų�������"<<err.what()<<std::endl;
				AddStatusLogWithSynT(ostr.str());
				return -1;
			}

			if (yk_type == DataBase::YkClose)
			{
				if (terminalPtr->getbHYkDouble(yk_no))
				{
					bytesAssemble = AssembleDoubleYKExe(bufIndex,buf,terminalPtr,yk_no,DYK_TYPE_CLOSE);
				}
				else
				{
					AddStatusLogWithSynT("�޷������ң������\n");
				}

			}
			else if (yk_type == DataBase::YkOpen)
			{
				if (terminalPtr->getbHYkDouble(yk_no))
				{
					bytesAssemble = AssembleDoubleYKExe(bufIndex,buf,terminalPtr,yk_no,DYK_TYPE_OPEN);
				}
				else
				{
					AddStatusLogWithSynT("�޷������ң������\n");
				}

			}
			else
			{
				AddStatusLogWithSynT("ң��ִ�������ң�����Ͳ����Ƿ���\n");
				return -1;
			}

			if (bytesAssemble > 0)
			{
				//if (!DataBase::CYkPoint::CheckYkStatusDevelopWithSel(terminalPtr->getYkStatus(yk_no),DataBase::YkExeSend))
				if((terminalPtr->loadYkPointPtr(yk_no))->SendExeEvent())
				{
					//std::ostringstream ostr;
					//ostr<<"curStatus:"<<(int)terminalPtr->getYkStatus(yk_no)<<"NextStatus:"<<(int)DataBase::YkExeSend<<std::endl;
					//AddStatusLogWithSynT(ostr.str());
					AddStatusLogWithSynT("����ң��ִ���������ң�ص�ǰң��״̬�����ϣ��˳������͸����\n");
					return -1;
				}
				//terminalPtr->setYkStatus(yk_no,DataBase::YkExeSend);

				setWaitingForAnswer(cmd.getCommPoint());//����Ҫ�ظ���־
				ResetTimerYkExe(terminalPtr,yk_no,true);
			}
		}
		break;

	case YK_CANCEL_ACT:
		{
			int yk_no;
			try
			{
				yk_no = boost::any_cast<int>(cmd.getVal());
			}
			catch(const boost::bad_any_cast & e)
			{
				std::ostringstream ostr;
				ostr.str("");
				ostr<<"ң��ȡ�������ң�ص�Ų����Ƿ���"<<e.what()<<std::endl;
				AddStatusLogWithSynT(ostr.str());
				return -1;
			}

			typeYktype yk_type = DataBase::YkOtherType;
			try
			{
				yk_type = terminalPtr->getYkType(yk_no);
			}
			catch(PublicSupport::dat2def_exception & err)
			{
				std::ostringstream ostr;
				ostr.str("");
				ostr<<"ң��ȡ�������ң�ص�Ų�������"<<err.what()<<std::endl;
				AddStatusLogWithSynT(ostr.str());
				return -1;
			}

			if (yk_type == DataBase::YkClose)
			{
				if (terminalPtr->getbHYkDouble(yk_no))
				{
					bytesAssemble = AssembleDoubleYKCancel(bufIndex,buf,terminalPtr,yk_no,DYK_TYPE_CLOSE);
				}
				else
				{
					AddStatusLogWithSynT("�޷������ң������\n");
				}

			}
			else if (yk_type == DataBase::YkOpen)
			{
				if (terminalPtr->getbHYkDouble(yk_no))
				{
					bytesAssemble = AssembleDoubleYKCancel(bufIndex,buf,terminalPtr,yk_no,DYK_TYPE_OPEN);
				}
				else
				{
					AddStatusLogWithSynT("�޷������ң������\n");
				}

			}
			else
			{
				AddStatusLogWithSynT("ң��ȡ�������ң�����Ͳ����Ƿ���\n");
				return -1;
			}

			if (bytesAssemble > 0)
			{
				//if (!DataBase::CYkPoint::CheckYkStatusDevelopWithSel(terminalPtr->getYkStatus(yk_no),DataBase::YkCancelSend))
				if((terminalPtr->loadYkPointPtr(yk_no))->SendCancelEvent())
				{
					//std::ostringstream ostr;
					//ostr<<"curStatus:"<<(int)terminalPtr->getYkStatus(yk_no)<<"NextStatus:"<<(int)DataBase::YkCancelSend<<std::endl;
					//AddStatusLogWithSynT(ostr.str());
					AddStatusLogWithSynT("����ң��ȡ���������ң�ص�ǰң��״̬�����ϣ��˳������͸����\n");
					return -1;
				}
				//terminalPtr->setYkStatus(yk_no,DataBase::YkCancelSend);

				setWaitingForAnswer(cmd.getCommPoint());//����Ҫ�ظ���־
				ResetTimerYkSel(terminalPtr,yk_no,false);
				ResetTimerYkCancel(terminalPtr,yk_no,true);
			}
		}
		break;

	default:
		break;
	}

	return bytesAssemble;
}

int CH103::AssembleFrameTail( size_t bufBegin, size_t bufIndex, unsigned char * buf, CCmd & cmd)
{
	int length = bufIndex - bufBegin;
	if (length <= 1)
	{
		return -1;
	}

	size_t count = bufIndex;

	switch (cmd.getCmdType())
	{
	case REQUIRE_LINK:
		buf[count++] = CalcCheckSumByte(&buf[bufBegin + 1],length - 1);
		buf[count++] = 0x16;
		break;

	case RESET_LINK:
		buf[count++] = CalcCheckSumByte(&buf[bufBegin + 1],length - 1);
		buf[count++] = 0x16;
		break;

	case CALL_PRIMARY_DATA:
		buf[count++] = CalcCheckSumByte(&buf[bufBegin + 1],length - 1);
		buf[count++] = 0x16;
		break;

	case CALL_SECONDARY_DATA:
		buf[count++] = CalcCheckSumByte(&buf[bufBegin + 1],length - 1);
		buf[count++] = 0x16;
		break;

	default:
		{
			int framelength = length - 4;
			if (framelength <= 0 || framelength > max_frame_length_)
			{
				return -1;
			}

			buf[count++] = CalcCheckSumByte(&buf[bufBegin + 4],framelength);
			buf[count++] = 0x16;
			buf[bufBegin + 1] = framelength & 0xff;
			buf[bufBegin + 2] = framelength & 0xff;
		}
		break;
	}

	return count - bufIndex;
}

int CH103::InitProtocol()
{
	CProtocol::InitProtocol();

	InitFrameLocation(5 + AsduAddrLength_);

	if(getCommPointSum() > 0)
	{
		share_commpoint_ptr nextPoint = getFirstCommPoint();
		if (nextPoint)
		{
			switch(h103Type_)
			{
			case NoShortFrameType:
				{
					AddSendCmdVal(CALL_ALL_DATA_ACT,CALL_ALL_DATA_ACT_PRIORITY,nextPoint);
					AddSendCmdVal(SYN_TIME_ACT,SYN_TIME_ACT_PRIORITY,nextPoint);
				}
				break;

			default:
				{
					//AddSendCmdVal(REQUIRE_LINK,REQUIRE_LINK_PRIORITY,nextPoint);
					AddSendCmdVal(RESET_LINK,RESET_LINK_PRIORITY,nextPoint);
					std::ostringstream ostr;//zyq
					ostr.str("");
					ostr<<"���RESET_LINK��nextPoint="<<nextPoint<<std::endl;
					AddStatusLogWithSynT(ostr.str());

				}
				break;
			}
		}
	}

	AddStatusLogWithSynT("H103��Լ��ͨ���򿪳ɹ���\n");

	return 0;
}

void CH103::UninitProtocol()
{
	CProtocol::UninitProtocol();

	AddStatusLogWithSynT("H103��Լ��ͨ���رճɹ���\n");
}

void CH103::InitObjectIndex()
{
	ProtocolObjectIndex_ = H103ObjectCounter_++;
}

void CH103::InitDefaultStartAddr()
{
	SYX_START_ADDR_ = DEFAULT_SYX_START_ADDR;                              //����yx��ʼ��ַ
	DYX_START_ADDR_ = DEFAULT_DYX_START_ADDR;                              //˫��yx��ʼ��ַ
	YC_START_ADDR_ =  DEFAULT_YC_START_ADDR;                               //yc��ʼ��ַ
	SYK_START_ADDR_ = DEFAULT_SYK_START_ADDR;                              //����yk��ʼ��ַ
	DYK_START_ADDR_ = DEFAULT_DYK_START_ADDR;                              //˫��yk��ʼ��ַ
	YM_START_ADDR_ =  DEFAULT_YM_START_ADDR;                               //ym��ʼ��ַ
}

void CH103::InitDefaultFrameElem()
{
	FrameTypeLength_ =   DEFAULT_FrameTypeLength;                           //�������ͱ�ʶ���ֽڳ���
	InfoNumLength_ =     DEFAULT_InfoNumLength;                             //��Ϣ����Ŀ��ʶ���ֽڳ���
	TransReasonLength_ = DEFAULT_TransReasonLength;                         //����ԭ���ʶ���ֽڳ���
	AsduAddrLength_ =    DEFAULT_AsduAddrLength;                            //װ�õ�ַ��ʶ���ֽڳ���
	InfoAddrLength_ =    DEFAULT_InfoAddrLength;                            //��Ϣ���ַ��ʶ���ֽڳ���
	FunTypeLength_ =     DEFAULT_FunTypeLength;                             //�������ͱ�ʶ���ֽڳ���
}

void CH103::InitFrameLocation(size_t FrameHead)
{
	FrameTypeLocation_ = FrameHead;
	InfoNumLocation_ = FrameTypeLocation_ + FrameTypeLength_;
	TransReasonLocation_ = InfoNumLocation_ + InfoNumLength_;
	AsduAddrLocation_ = TransReasonLocation_ + TransReasonLength_;
	FunTypeLocation_ = AsduAddrLocation_ + AsduAddrLength_;
	InfoAddrLocation_ = FunTypeLocation_ + FunTypeLength_;
	DataLocation_ = InfoAddrLocation_ + InfoAddrLength_;
}

void CH103::InitDefaultTimeOut()
{
	//bUseTimeOutQueryUnActivePoint_ = false;
	bUseTimeOutQueryUnActivePoint_ = true;//zyq
	timeOutQueryUnActivePoint_ = DEFAULT_timeOutQueryUnActivePoint;
	timeOutRequireLink_ = DEFAULT_timeOutRequireLink;
	timeOutCallAllData_ = DEFAULT_timeOutCallAllData;
	timeOutCallAllDD_ = DEFAULT_timeOutCallAllDD;
	timeOutSynTime_ = DEFAULT_timeOutSynTime;
	timeOutHeartFrame_ = DEFAULT_timeOutHeartFrame;
	timeOutYkSel_ = DEFAULT_timeOutYkSel;
	timeOutYkExe_ = DEFAULT_timeOutYkExe;
	timeOutYkCancel_ = DEFAULT_timeOutYkCancel;
}

void CH103::InitDefaultTimer(boost::asio::io_service & io_service)
{
	InitDefaultTimeOut();

	using namespace boost::asio;
	using namespace boost::posix_time;

	timerResetLink_.reset(new deadline_timer(io_service,seconds(timeOutRequireLink_)));
	AddTimer(timerResetLink_);

	timerCallAllData_.reset(new deadline_timer(io_service,seconds(getMeanvalueOfPointsSum(MIN_timeOutCallAllData,timeOutCallAllData_))));
	AddTimer(timerCallAllData_);

	timerCallAllDD_.reset(new deadline_timer(io_service,seconds(getMeanvalueOfPointsSum(MIN_timeOutCallAllDD,timeOutCallAllDD_))));
	AddTimer(timerCallAllDD_);

	timerSynTime_.reset(new deadline_timer(io_service,seconds(getMeanvalueOfPointsSum(MIN_timeOutSynTime,timeOutSynTime_))));
	AddTimer(timerSynTime_);

	timerHeartFrame_.reset(new deadline_timer(io_service,seconds(timeOutHeartFrame_)));
	AddTimer(timerHeartFrame_);

	timerYkSel_.reset(new deadline_timer(io_service,seconds(timeOutYkSel_)));
	AddTimer(timerYkSel_);

	timerYkExe_.reset(new deadline_timer(io_service,seconds(timeOutYkExe_)));
	AddTimer(timerYkExe_);

	timerYkCancel_.reset(new deadline_timer(io_service,seconds(timeOutYkCancel_)));
	AddTimer(timerYkCancel_);
}

int CH103::QueryUnAliveCommPoint(share_commpoint_ptr point)
{
	if (point)
	{
		if (bUseTimeOutQueryUnActivePoint_)
		{
			timeOutRequireLink_ = timeOutQueryUnActivePoint_;

			std::ostringstream ostr;//zyq
			ostr<<"Point sum = "<<getCommPointSum()<<std::endl;
			for (int i=0;i<getCommPointSum();i++)
			{
				ostr<<"point["<<i<<"]="<<getCommPoint(i).lock()<<std::endl;
			}
			ostr<<"����QueryUnAliveCommPoint�У�point="<<point<<std::endl<<"timeOutRequireLink_="<<timeOutRequireLink_<<std::endl;//zyq
			AddStatusLogWithSynT(ostr.str());

		}

		ResetTimerResetLink(point,true);

		return 0;
	}

	return -1;
}

void CH103::handle_timerResetLink(const boost::system::error_code& error,share_commpoint_ptr point)
{
	if (!error)
	{
		share_commpoint_ptr val = getNextCommPoint(TERMINAL_NODE,false,point);
		std::ostringstream ostr;//zyq
		ostr.str("");
		ostr<<"handle_timerResetLink�У�getNextCommPoint��point="<<point<<std::endl<<"val="<<val<<std::endl;//zyq
		AddStatusLogWithSynT(ostr.str());

		if (val)
		{
			//AddSendCmdVal(REQUIRE_LINK,REQUIRE_LINK_PRIORITY,val);
			AddSendCmdVal(RESET_LINK,RESET_LINK_PRIORITY,val);
			ostr.str("");
			ostr<<"���RESET_LINK���t/pointֵ����ǰval��val="<<val<<std::endl;//zyq
			AddStatusLogWithSynT(ostr.str());

		}
	}
}

void CH103::handle_timerCallAllData(const boost::system::error_code& error,share_commpoint_ptr point)
{
	if (!error)
	{
		share_commpoint_ptr val = getNextCommPoint(TERMINAL_NODE,true,point);
		/*std::ostringstream ostr;//zyq
		ostr.str("");
		ostr<<"handle_timerCallAllData�У�getNextCommPoint��point="<<point<<std::endl<<"val="<<val<<std::endl;//zyq
		AddStatusLogWithSynT(ostr.str());*/
		if (val)
		{
			AddSendCmdVal(CALL_ALL_DATA_ACT,CALL_ALL_DATA_ACT_PRIORITY,val);
			/*ostr.str("");
			ostr<<"�������CALL_ALL_DATA_ACT���t/pointֵ����ǰval��val="<<val<<std::endl;//zyq
			AddStatusLogWithSynT(ostr.str());*/
		}
	}
}

void CH103::handle_timerCallAllDD(const boost::system::error_code& error,share_commpoint_ptr point)
{
	if (!error)
	{
		share_commpoint_ptr val = getNextCommPoint(TERMINAL_NODE,true,point);
		if (val)
		{
			AddSendCmdVal(CALL_ALL_DD_ACT,CALL_ALL_DD_ACT_PRIORITY,val);
		}
	}
}

void CH103::handle_timerSynTime(const boost::system::error_code& error,share_commpoint_ptr point)
{
	if (!error)
	{
		share_commpoint_ptr val = getNextCommPoint(TERMINAL_NODE,true,point);
		/*std::ostringstream ostr;//zyq
		ostr.str("");
		ostr<<"handle_timerSynTime�У�getNextCommPoint��point="<<point<<std::endl<<"val="<<val<<std::endl;//zyq
		AddStatusLogWithSynT(ostr.str());*/
		if (val)
		{
			AddSendCmdVal(SYN_TIME_ACT,SYN_TIME_ACT_PRIORITY,val);
			/*ostr.str("");
			ostr<<"��Ӷ�ʱSYN_TIME_ACT���t/pointֵ����ǰval��val="<<val<<std::endl;//zyq
			AddStatusLogWithSynT(ostr.str());*/
		}
	}
}

void CH103::handle_timerHeartFrame(const boost::system::error_code& error,share_commpoint_ptr point)
{
	if (!error)
	{
		share_commpoint_ptr val = getNextCommPoint(TERMINAL_NODE,true,point);
		/*std::ostringstream ostr;//zyq
		ostr<<"Point sum = "<<getCommPointSum()<<std::endl;
		for (int i=0;i<getCommPointSum();i++)
		{
			ostr<<"point["<<i<<"]="<<getCommPoint(i).lock()<<std::endl;
		}
		ostr<<"handle_timerHeartFrame�У�getNextCommPoint��point="<<point<<"val="<<val<<std::endl;//zyq
		AddStatusLogWithSynT(ostr.str());*/
		if (val)
		{
			AddSendCmdVal(CALL_SECONDARY_DATA,CALL_SECONDARY_DATA_PRIORITY,val);
			/*ostr.str("");
			ostr<<"��Ӷ�������CALL_SECONDARY_DATA���prepoint="<<point<<"curpoint"<<val<<std::endl;//zyq
			AddStatusLogWithSynT(ostr.str());*/
		}
	}
}

void CH103::handle_timerYkSel(const boost::system::error_code& error,share_commpoint_ptr point,size_t yk_no)
{
	if (!error)
	{
		if (point)
		{
			if (point->getCommPointType() == TERMINAL_NODE)
			{
				share_terminal_ptr terminalPtr = boost::dynamic_pointer_cast<DataBase::CTerminal>(point);
				if (terminalPtr)
				{
					//terminalPtr->setYkStatus(yk_no,DataBase::YkSelTimeOut);
					(terminalPtr->loadYkPointPtr(yk_no))->TimeOutEvent();
				}
			}

			CmdConSig_(YK_SEL_CON,RETURN_CODE_TIMEOUT,point,(int)yk_no);
			AddStatusLogWithSynT("H103��Լң��ѡ�����ʱ��\n");
		}
	}
}

void CH103::handle_timerYkExe(const boost::system::error_code& error,share_commpoint_ptr point,size_t yk_no)
{
	if (!error)
	{
		if (point)
		{
			if (point->getCommPointType() == TERMINAL_NODE)
			{
				share_terminal_ptr terminalPtr = boost::dynamic_pointer_cast<DataBase::CTerminal>(point);
				if (terminalPtr)
				{
					//terminalPtr->setYkStatus(yk_no,DataBase::YkExeTimeOut);
					(terminalPtr->loadYkPointPtr(yk_no))->TimeOutEvent();
				}
			}

			CmdConSig_(YK_EXE_CON,RETURN_CODE_TIMEOUT,point,(int)yk_no);
			AddStatusLogWithSynT("H101��Լң��ִ�����ʱ��\n");
		}
	}
}

void CH103::handle_timerYkCancel(const boost::system::error_code& error,share_commpoint_ptr point,size_t yk_no)
{
	if (!error)
	{
		if (point)
		{
			if (point->getCommPointType() == TERMINAL_NODE)
			{
				share_terminal_ptr terminalPtr = boost::dynamic_pointer_cast<DataBase::CTerminal>(point);
				if (terminalPtr)
				{
					//terminalPtr->setYkStatus(yk_no,DataBase::YkCancelTimeOut);
					(terminalPtr->loadYkPointPtr(yk_no))->TimeOutEvent();
				}
			}

			CmdConSig_(YK_CANCEL_CON,RETURN_CODE_TIMEOUT,point,(int)yk_no);
			AddStatusLogWithSynT("H101��Լң��ȡ�����ʱ��\n");
		}
	}
}

void CH103::ResetTimerResetLink(share_commpoint_ptr point,bool bContinue /*= true*/, unsigned short val/* = 0*/)
{
	if (bContinue)
	{
		if (val == 0)
		{
			timerResetLink_->expires_from_now(boost::posix_time::seconds(timeOutRequireLink_));
			/*std::ostringstream ostr;//zyq
			ostr.str("");
			ostr<<"ResetTimerResetLink��timeOutRequireLink_="<<timeOutRequireLink_<<"��point="<<point<<std::endl;
			AddStatusLogWithSynT(ostr.str());*/

		}
		else
		{
			timerResetLink_->expires_from_now(boost::posix_time::seconds(val));
		}

		timerResetLink_->async_wait(boost::bind(&CH103::handle_timerResetLink,this,boost::asio::placeholders::error,point));
	}
	else
	{
		timerResetLink_->cancel();
	}
}

void CH103::ResetTimerCallAllData(share_commpoint_ptr point,bool bContinue /*= true*/,unsigned short val/* = 0*/)
{
	if (bContinue)
	{
		//share_commpoint_ptr nextPoint = getNextCommPoint(TERMINAL_NODE,boost::logic::indeterminate,point);
		share_commpoint_ptr nextPoint = getNextCommPoint(TERMINAL_NODE,true,point);
		std::ostringstream ostr;//zyq
		ostr.str("");
		ostr<<"ResetTimerCallAllData�У�getNextCommPoint��point="<<point<<",nextPoint=ԭhandle_timerCallAllData��val="<<nextPoint<<std::endl;//zyq
		AddStatusLogWithSynT(ostr.str());

		if(nextPoint)
		{
			if(nextPoint->getInitCommPointFlag())
			{
				if (val == 0)
				{
					timerCallAllData_->expires_from_now(boost::posix_time::seconds(getMeanvalueOfPointsSum(MIN_timeOutCallAllData,timeOutCallAllData_)));
					ostr.str("");
					ostr<<"ResetTimerCallAllData��timeOutCallAllData_="<<timeOutCallAllData_<<"��nextPoint="<<nextPoint<<std::endl;
					AddStatusLogWithSynT(ostr.str());
				}
				else
				{
					timerCallAllData_->expires_from_now(boost::posix_time::seconds(val));
				}
			}
			else
			{
				timerCallAllData_->expires_from_now(boost::posix_time::seconds(MIN_timeOutCallAllData));
			}

			timerCallAllData_->async_wait(boost::bind(&CH103::handle_timerCallAllData,this,boost::asio::placeholders::error,point));
			AddStatusLogWithSynT("��ִ��handle_timerCallAllData;\n");//zyq

		}
	}
	else
	{
		timerCallAllData_->cancel();
	}
}

void CH103::ResetTimerCallAllDD(share_commpoint_ptr point,bool bContinue /*= true*/,unsigned short val/* = 0*/)
{
	if (bContinue)
	{
		if (val == 0)
		{
			timerCallAllDD_->expires_from_now(boost::posix_time::seconds(getMeanvalueOfPointsSum(MIN_timeOutCallAllDD,timeOutCallAllDD_)));
		}
		else
		{
			timerCallAllDD_->expires_from_now(boost::posix_time::seconds(val));
		}

		timerCallAllDD_->async_wait(boost::bind(&CH103::handle_timerCallAllDD,this,boost::asio::placeholders::error,point));
	}
	else
	{
		timerCallAllDD_->cancel();
	}
}

void CH103::ResetTimerSynTime(share_commpoint_ptr point,bool bContinue /*= true*/,unsigned short val/* = 0*/)
{
	if (bContinue)
	{
		std::ostringstream ostr;//zyq
		/*ostr.str("");
		ostr<<"ResetTimerSynTime�У�����point="<<point<<std::endl;//zyq
		AddStatusLogWithSynT(ostr.str());*/
		if (val == 0)
		{
			timerSynTime_->expires_from_now(boost::posix_time::seconds(timeOutSynTime_));
			/*ostr.str("");
			ostr<<"ResetTimerSynTime��timeOutSynTime_="<<timeOutCallAllData_<<std::endl;
			AddStatusLogWithSynT(ostr.str());*/
		}
		else
		{
			timerSynTime_->expires_from_now(boost::posix_time::seconds(val));
		}

		timerSynTime_->async_wait(boost::bind(&CH103::handle_timerSynTime,this,boost::asio::placeholders::error,point));
		AddStatusLogWithSynT("��ִ��handle_timerCallAllData;\n");//zyq
	}
	else
	{
		timerSynTime_->cancel();
	}
}

void CH103::ResetTimerHeartFrame(share_commpoint_ptr point,bool bContinue /*= true*/,unsigned short val/* = 0*/)
{
	if (bContinue)
	{
		if (val == 0)
		{
			timerHeartFrame_->expires_from_now(boost::posix_time::seconds(timeOutHeartFrame_));
		}
		else
		{
			timerHeartFrame_->expires_from_now(boost::posix_time::seconds(val));
		}

		timerHeartFrame_->async_wait(boost::bind(&CH103::handle_timerHeartFrame,this,boost::asio::placeholders::error,point));
	}
	else
	{
		timerHeartFrame_->cancel();
	}
}

void CH103::ResetTimerYkSel(share_commpoint_ptr point,size_t yk_no,bool bContinue /*= false*/,unsigned short val/* = 0*/)
{
	if (bContinue)
	{
		if (val == 0)
		{
			timerYkSel_->expires_from_now(boost::posix_time::seconds(timeOutYkSel_));
		}
		else
		{
			timerYkSel_->expires_from_now(boost::posix_time::seconds(val));
		}

		timerYkSel_->async_wait(boost::bind(&CH103::handle_timerYkSel,this,boost::asio::placeholders::error,point,yk_no));
	}
	else
	{
		timerYkSel_->cancel();
	}
}

void CH103::ResetTimerYkExe(share_commpoint_ptr point,size_t yk_no,bool bContinue /*= false*/,unsigned short val/* = 0*/)
{
	if (bContinue)
	{
		if (val == 0)
		{
			timerYkExe_->expires_from_now(boost::posix_time::seconds(timeOutYkExe_));
		}
		else
		{
			timerYkExe_->expires_from_now(boost::posix_time::seconds(val));
		}

		timerYkExe_->async_wait(boost::bind(&CH103::handle_timerYkExe,this,boost::asio::placeholders::error,point,yk_no));
	}
	else
	{
		timerYkExe_->cancel();
	}
}

void CH103::ResetTimerYkCancel(share_commpoint_ptr point,size_t yk_no,bool bContinue /*= false*/,unsigned short val/* = 0*/)
{
	if (bContinue)
	{
		if (val == 0)
		{
			timerYkCancel_->expires_from_now(boost::posix_time::seconds(timeOutYkCancel_));
		}
		else
		{
			timerYkCancel_->expires_from_now(boost::posix_time::seconds(val));
		}

		timerYkCancel_->async_wait(boost::bind(&CH103::handle_timerYkCancel,this,boost::asio::placeholders::error,point,yk_no));
	}
	else
	{
		timerYkCancel_->cancel();
	}
}

//para api
int CH103::setSYX_START_ADDR(size_t val)
{
	if (val < 0 || val >= 0x4001)
	{
		return -1;
	}

	SYX_START_ADDR_ = val;

	return 0;
}

int CH103::setDYX_START_ADDR(size_t val)
{
	if (val < 0 || val >= 0x4001)
	{
		return -1;
	}

	DYX_START_ADDR_ = val;

	return 0;
}

int CH103::setYC_START_ADDR(size_t val)
{
	if (val < 0x101 || val >= 0x6001)
	{
		return -1;
	}

	YC_START_ADDR_ = val;

	return 0;
}

int CH103::setSYK_START_ADDR(size_t val)
{
	if (val < 0x701)
	{
		return -1;
	}

	SYK_START_ADDR_ = val;

	return 0;
}

int CH103::setDYK_START_ADDR(size_t val)
{
	if (val < 0x701)
	{
		return -1;
	}

	DYK_START_ADDR_ = val;

	return 0;
}

int CH103::setYM_START_ADDR(size_t val)
{
	if (val < 0x701)
	{
		return -1;
	}

	YM_START_ADDR_ = val;

	return 0;
}

int CH103::setFrameTypeLength(unsigned short val)
{
	if (val <= 0 || val > 4)
	{
		return -1;
	}

	FrameTypeLength_ = val;

	return 0;
}

int CH103::setInfoNumLength(unsigned short val)
{
	if (val <= 0 || val > 4)
	{
		return -1;
	}

	InfoNumLength_ = val;

	return 0;
}

int CH103::setTransReasonLength(unsigned short val)
{
	if (val <= 0 || val > 8)
	{
		return -1;
	}

	TransReasonLength_ = val;

	return 0;
}

int CH103::setAsduAddrLength(unsigned short val)
{
	if (val <= 0 || val > 8)
	{
		return -1;
	}

	AsduAddrLength_ = val;

	return 0;
}

int CH103::setInfoAddrLength(unsigned short val)
{
	if (val <= 0 || val > 12)
	{
		return -1;
	}

	InfoAddrLength_ = val;

	return 0;
}

int CH103::setFunTypeLength(unsigned short val)
{
	if (val <= 0 || val > 4)
	{
		return -1;
	}

	FunTypeLength_ = val;

	return 0;
}

int CH103::setTimeOutQueryUnActivePoint(unsigned short val)
{
	if (val < MIN_timeOutQueryUnActivePoint)
	{
		return -1;
	}

	timeOutQueryUnActivePoint_ = val;

	return 0;
}

int CH103::setTimeOutRequrieLink(unsigned short val)
{
	if (val < MIN_timeOutRequireLink || val > 60)
	{
		return -1;
	}

	timeOutRequireLink_ = val;

	return 0;
}

int CH103::setTimeOutCallAllData(unsigned short val)
{
	if (val < 60 || val > 12000)
	{
		return -1;
	}

	timeOutCallAllData_ = val;

	return 0;
}

int CH103::setTimeOutCallAllDD(unsigned short val)
{
	if (val < 60 || val > 12000)
	{
		return -1;
	}

	timeOutCallAllDD_ = val;

	return 0;
}

int CH103::setTimeOutSynTime(unsigned short val)
{
	if (val < 60 || val > 12000)
	{
		return -1;
	}

	timeOutSynTime_ = val;

	return 0;
}

int CH103::setTimeOutHeartFrame(unsigned short val)
{
	if (val <= 0 || val > 60) 
	{
		return -1;
	}

	timeOutHeartFrame_ = val;

	return 0;
}

int CH103::setTimeOutYkSel(unsigned short val)
{
	if (val <= 0 || val > 300)
	{
		return -1;
	}

	timeOutYkSel_ = val;

	return 0;
}

int CH103::setTimeOutYkExe(unsigned short val)
{
	if (val <= 0 || val > 300)
	{
		return -1;
	}

	timeOutYkExe_ = val;

	return 0;
}

int CH103::setTimeOutYkCancel(unsigned short val)
{
	if (val <= 0 || val > 300)
	{
		return -1;
	}

	timeOutYkCancel_ = val;

	return 0;
}

int CH103::AssemblePrimaryData(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr)
{
	size_t count = bufIndex;

	count += ValToBuf(&buf[count],C_PL1_NA_3 | getFCB(terminalPtr) | DIR_PRM | ACT_FCV,FrameTypeLength_);
	count += ValToBuf(&buf[count],terminalPtr->getAddr(),AsduAddrLength_);

	return count - bufIndex;
}

int CH103::AssembleSecondaryData(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr)
{
	size_t count = bufIndex;

	count += ValToBuf(&buf[count],C_PL2_NA_3 | getFCB(terminalPtr) | DIR_PRM | ACT_FCV,FrameTypeLength_);
	count += ValToBuf(&buf[count],terminalPtr->getAddr(),AsduAddrLength_);

	return count - bufIndex;
}

int CH103::AssembleRequireLink(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr)
{
	size_t count = bufIndex;

	count += ValToBuf(&buf[count],C_PLK_NA_3 | getFCB(terminalPtr) | DIR_PRM | ACT_FCV,FrameTypeLength_);
	count += ValToBuf(&buf[count],terminalPtr->getAddr(),AsduAddrLength_);

	return count - bufIndex;
}

int CH103::AssembleResetLink(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr)
{
	size_t count = bufIndex;

	count += ValToBuf(&buf[count],C_RCU_NA_3 | DIR_PRM | NACK_FCV,FrameTypeLength_);
	count += ValToBuf(&buf[count],terminalPtr->getAddr(),AsduAddrLength_);

	return count - bufIndex;
}

int CH103::AssembleCallAllData(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr)
{
	size_t count = bufIndex;

	count += ValToBuf(&buf[count],C_REQ_NA_3 | getFCB(terminalPtr)| DIR_PRM | ACT_FCV,1);
	count += ValToBuf(&buf[count],terminalPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],C_IGI_NA_3,FrameTypeLength_);
	count += ValToBuf(&buf[count],(0x01| EnableISQ),InfoNumLength_);
	count += ValToBuf(&buf[count],trans_gi,TransReasonLength_);
	count += ValToBuf(&buf[count],terminalPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],fun_GLB,FunTypeLength_);
	count += ValToBuf(&buf[count],0,InfoAddrLength_);
	buf[count++] = QOI;

	return count - bufIndex;
}

int CH103::AssembleCallAllDD(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr)
{
	size_t count = bufIndex;

	count += ValToBuf(&buf[count],C_REQ_NA_3 | getFCB(terminalPtr)| DIR_PRM | ACT_FCV,1);
	count += ValToBuf(&buf[count],terminalPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],ASDU88,FrameTypeLength_);
	count += ValToBuf(&buf[count],(0x01| DisableISQ),InfoNumLength_);
	count += ValToBuf(&buf[count],trans_cyc,TransReasonLength_);
	count += ValToBuf(&buf[count],terminalPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],1,FunTypeLength_);
	count += ValToBuf(&buf[count],0,InfoAddrLength_);
	buf[count++] = 0x45;
	buf[count++] = QOI;

	return count - bufIndex;
}

int CH103::AssembleSynTime(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr, boost::posix_time::ptime time)
{
	size_t count = bufIndex;

	//count += ValToBuf(&buf[count],C_REQ_NA_3 | getFCB(terminalPtr)| DIR_PRM | ACT_FCV,1);
	count += ValToBuf(&buf[count],C_NEQ_NA_3 | getFCB(terminalPtr)| DIR_PRM | 0,1);
	count += ValToBuf(&buf[count],BroadCastAddr,AsduAddrLength_);
	count += ValToBuf(&buf[count],C_SYN_TA_3,FrameTypeLength_);
	count += ValToBuf(&buf[count],(0x01| EnableISQ),InfoNumLength_);
	count += ValToBuf(&buf[count],trans_cs,TransReasonLength_);
	count += ValToBuf(&buf[count],BroadCastAddr,AsduAddrLength_);
	count += ValToBuf(&buf[count],fun_GLB,FunTypeLength_);
	count += ValToBuf(&buf[count],0,InfoAddrLength_);
	boost::posix_time::time_duration td = time.time_of_day();
	count += ValToBuf(&buf[count],td.total_milliseconds() % MinutesRemainderMillisecs,2);
	buf[count++] = td.minutes() & 0x3f;
	buf[count++] = td.hours() & 0x1f;
	boost::gregorian::date::ymd_type ymd = time.date().year_month_day();
	buf[count++] = ymd.day & 0x1f;
	buf[count++] = ymd.month & 0x0f;
	buf[count++] = ymd.year % 100;

	return count - bufIndex;
}

int CH103::AssembleDoubleYKSel(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr,unsigned short yk_no, unsigned char yk_code)
{
	size_t count = bufIndex;

	count += ValToBuf(&buf[count],C_REQ_NA_3 | getFCB(terminalPtr)| DIR_PRM | ACT_FCV,1);
	count += ValToBuf(&buf[count],terminalPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],ASDU64,FrameTypeLength_);
	count += ValToBuf(&buf[count],0x01,InfoNumLength_);
	count += ValToBuf(&buf[count],trans_operate_rem,TransReasonLength_);
	count += ValToBuf(&buf[count],terminalPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],1,FunTypeLength_);
	count += ValToBuf(&buf[count],DYK_START_ADDR_ + yk_no,InfoAddrLength_);
	buf[count++] = 0x80 | yk_code;
	buf[count++] = QOI;

	return count - bufIndex;
}

int CH103::AssembleDoubleYKExe(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr,unsigned short yk_no, unsigned char yk_code)
{
	size_t count = bufIndex;

	count += ValToBuf(&buf[count],C_REQ_NA_3 | getFCB(terminalPtr)| DIR_PRM | ACT_FCV,1);
	count += ValToBuf(&buf[count],terminalPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],ASDU64,FrameTypeLength_);
	count += ValToBuf(&buf[count],0x01,InfoNumLength_);
	count += ValToBuf(&buf[count],trans_operate_rem,TransReasonLength_);
	count += ValToBuf(&buf[count],terminalPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],1,FunTypeLength_);
	count += ValToBuf(&buf[count],DYK_START_ADDR_ + yk_no,InfoAddrLength_);
	buf[count++] = 0x00 | yk_code;
	buf[count++] = QOI;

	return count - bufIndex;
}

int CH103::AssembleDoubleYKCancel(size_t bufIndex, unsigned char * buf, share_terminal_ptr terminalPtr,unsigned short yk_no,unsigned char yk_code)
{
	size_t count = bufIndex;

	count += ValToBuf(&buf[count],C_REQ_NA_3 | getFCB(terminalPtr)| DIR_PRM | ACT_FCV,1);
	count += ValToBuf(&buf[count],terminalPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],ASDU64,FrameTypeLength_);
	count += ValToBuf(&buf[count],0x01,InfoNumLength_);
	count += ValToBuf(&buf[count],trans_operate_rem,TransReasonLength_);
	count += ValToBuf(&buf[count],terminalPtr->getAddr(),AsduAddrLength_);
	count += ValToBuf(&buf[count],1,FunTypeLength_);
	count += ValToBuf(&buf[count],DYK_START_ADDR_ + yk_no,InfoAddrLength_);
	buf[count++] = 0xc0 | yk_code;
	buf[count++] = QOI;

	return count - bufIndex;
}

int CH103::ParseShortFrame(unsigned char * buf, share_terminal_ptr terminalPtr)
{
	unsigned char ACD = buf[1] & ACT_ACD;
	/*std::ostringstream ostr;//zyq
	ostr.str("");
	ostr<<"����ACD="<<(int)ACD<<std::endl;
	AddStatusLogWithSynT(ostr.str());*/

	if (ACD == ACT_ACD)
	{
		AddOnlySendCmdWithoutVal(CALL_PRIMARY_DATA,CALL_PRIMARY_DATA_PRIORITY,terminalPtr,boost::any());
		/*ostr.str("");
		ostr<<"ACDλΪ20,�����һ������CALL_PRIMARY_DATA��terminalPtr��point=val=; "<<terminalPtr<<std::endl;//zyq
		AddStatusLogWithSynT(ostr.str());*/

	}

	unsigned char ContrCode = buf[1] & 0x0f;
	/*ostr.str("");
	ostr<<"�̱��ĵĿ�����09/29/00; "<<terminalPtr<<std::endl;//zyq
	AddStatusLogWithSynT(ostr.str());*/


	switch (ContrCode)
	{
	case M_CON_NA_3: //�϶��Ͽ�
		switch (getLastSendCmd())
		{
		case RESET_LINK:
			AddSendCmdVal(SYN_TIME_ACT,SYN_TIME_ACT_PRIORITY,terminalPtr);
			//AddStatusLogWithSynT("�յ�RESET_LINK�ظ����ģ���Ӷ�ʱ�����١���������cmd;");//zyq
			//ostr.str("");
			//ostr<<"��װ��sub���-1="<<terminalPtr<<std::endl;
			//AddStatusLogWithSynT(ostr.str());
			AddSendCmdVal(CALL_ALL_DATA_ACT,CALL_ALL_DATA_ACT_PRIORITY,terminalPtr);
			//AddStatusLogWithSynT("�յ�RESET_LINK�ظ����ģ��������cmd;");//zyq
			//ostr.str("");
			//ostr<<"��װ��sub���-1="<<terminalPtr<<std::endl;
			//AddStatusLogWithSynT(ostr.str());
			AddSendCmdVal(CALL_SECONDARY_DATA,CALL_SECONDARY_DATA_PRIORITY,terminalPtr);
			//AddStatusLogWithSynT("�յ�RESET_LINK�ظ����ģ���Ӷ�ʱ�����١���������cmd;");//zyq
			//ostr.str("");
			//ostr<<"��װ��sub���-1="<<terminalPtr<<std::endl;
			//AddStatusLogWithSynT(ostr.str());
			break;

		case YK_SEL_ACT:
			if (ACD == 0)
			{
				AddSendCmdVal(CALL_SECONDARY_DATA,CALL_SECONDARY_DATA_PRIORITY,terminalPtr);
				AddStatusLogWithSynT("�յ�ң��ѡ��ȷ��YK_SEL_ACT�ظ����ģ���Ӷ�������cmd;\n");//zyq

			}
			break;

		case YK_EXE_ACT:
			if (ACD == 0)
			{
				AddSendCmdVal(CALL_SECONDARY_DATA,CALL_SECONDARY_DATA_PRIORITY,terminalPtr);
			}
			break;

		case YK_CANCEL_ACT:
			if (ACD == 0)
			{
				AddSendCmdVal(CALL_SECONDARY_DATA,CALL_SECONDARY_DATA_PRIORITY,terminalPtr);
			}
			break;

		case CALL_ALL_DATA_ACT:
			if (ACD == 0)
			{
				AddSendCmdVal(CALL_SECONDARY_DATA,CALL_SECONDARY_DATA_PRIORITY,terminalPtr);
				/*AddStatusLogWithSynT("�յ�����ȷ��CALL_ALL_DATA_ACT�ظ����ģ���Ӷ�������cmd;");//zyq
				ostr.str("");
				ostr<<"��װ��sub���-1="<<terminalPtr<<std::endl;
				AddStatusLogWithSynT(ostr.str());*/
			}
			break;

		case CALL_ALL_DD_ACT:
			if (ACD == 0)
			{
				AddSendCmdVal(CALL_SECONDARY_DATA,CALL_SECONDARY_DATA_PRIORITY,terminalPtr);
			}
			break;

		case SYN_TIME_ACT:
			if (ACD == 0)
			{
				AddSendCmdVal(CALL_SECONDARY_DATA,CALL_SECONDARY_DATA_PRIORITY,terminalPtr);
				//AddStatusLogWithSynT("�յ���ʱȷ��SYN_TIME_ACT�ظ����ģ���Ӷ�������cmd;");//zyq
				//ostr.str("");
				//ostr<<"��װ��sub���-1="<<terminalPtr<<std::endl;
				//AddStatusLogWithSynT(ostr.str());

			}
			break;

		default:
			break;
		}
		break;

	case M_BY_NA_3: //���Ͽ�
		{
			terminalPtr->setCommActive(false);
			//AddSendCmdVal(REQUIRE_LINK,REQUIRE_LINK_PRIORITY,terminalPtr);
			AddSendCmdVal(RESET_LINK,RESET_LINK_PRIORITY,terminalPtr);
			AddStatusLogWithSynT("���Ͽɻظ����ģ����RESET_LINKcmd;");//zyq
			//ostr.str("");
			//ostr<<"��װ��sub���-1="<<terminalPtr<<std::endl;
			//AddStatusLogWithSynT(ostr.str());

			return -1;
		}
		break;

	case M_NV_NA_3: //�������������
		{
			AddStatusLogWithSynT("������������ݱ��ģ�\n");//zyq

			if (getLastSendCmd() == RESET_LINK)
			{
				terminalPtr->setCommActive(false);
				//AddSendCmdVal(REQUIRE_LINK,REQUIRE_LINK_PRIORITY,terminalPtr);
				AddSendCmdVal(RESET_LINK,RESET_LINK_PRIORITY,terminalPtr);
				AddStatusLogWithSynT("�����һ֡ΪRESET_LINK���������RESET_LINK��\n");//zyq
				//ostr.str("");
				//ostr<<"��װ��sub���-1="<<terminalPtr<<std::endl;
				//AddStatusLogWithSynT(ostr.str());
				return -1;
			}

		}

		break;

	case M_LKR_NA_3: //��·״̬
		if (getLastSendCmd() == REQUIRE_LINK)
		{
			AddSendCmdVal(RESET_LINK,REQUIRE_LINK_PRIORITY,terminalPtr);
		}
		break;

	default:
		{
			/*std::ostringstream ostr;
			ostr.str("");
			ostr<<"���ձ��Ĵ���δ����ı������� Control_Code ="<<ContrCode<<std::endl;
			AddStatusLogWithSynT(ostr.str());*/
		}
		break;
	}

	return 0;
}

int CH103::ParseLongFrame(unsigned char * buf, share_terminal_ptr terminalPtr)
{
	unsigned char ACD = buf[4] & ACT_ACD;
	std::ostringstream ostr;//zyq
	/*ostr.str("");
	ostr<<"����ACD="<<(int)ACD<<std::endl;
	AddStatusLogWithSynT(ostr.str());*/

	if (ACD == ACT_ACD)
	{
		AddOnlySendCmdWithoutVal(CALL_PRIMARY_DATA,CALL_PRIMARY_DATA_PRIORITY,terminalPtr,boost::any());
		/*ostr.str("");
		ostr<<"ACDλΪ20,�����һ������CALL_PRIMARY_DATA��terminalPtr��point=val=); "<<terminalPtr<<std::endl;
		AddStatusLogWithSynT(ostr.str());*/

	}

	size_t FrameType = BufToVal(&buf[FrameTypeLocation_],FrameTypeLength_);        //���ͱ�ʶ
	size_t TransReason = BufToVal(&buf[TransReasonLocation_],TransReasonLength_);  //����ԭ��
	size_t funType = BufToVal(&buf[FunTypeLocation_],FunTypeLength_);              //��������
	/*ostr.str("");
	ostr<<" funType ="<<funType<<std::endl;
	AddStatusLogWithSynT(ostr.str());*/



	switch(FrameType)
	{
	case ASDU50:
		ParseYCDataWithValid(buf,terminalPtr);
		break;

	case ASDU44:
		ParseAllSingleYXByte(buf,terminalPtr);
		break;

	case ASDU46:
		ParseAllDoubleYXByte(buf,terminalPtr);
		break;

	case M_SP_NA_3://40=28h
		ParseSingleYx(buf,terminalPtr);
		break;

	case M_SP_TA_3://41=29h
		ParseSingleYxWithTE(buf,terminalPtr);
		break;

	case M_IT_NA_3:
		ParseYMData(buf,terminalPtr);
		break;

	case M_IT_TA_3:
		ParseYMDataWithTE(buf,terminalPtr);
		break;

	case ASDU64:
		{
			unsigned char yk_code = buf[DataLocation_] & 0xc0;
			ostr.str("");
			ostr<<"YKѡ��80��ִ��00��ȡ��co��У����YK_CODE ="<<(int)yk_code<<std::endl;
			AddStatusLogWithSynT(ostr.str());
			switch(yk_code)
			{
			case 0x80:
				ParseDoubleYKSelCon(buf,terminalPtr);
				break;

			case 0x00:
				ParseDoubleYKExeCon(buf,terminalPtr);
				break;

			case 0xc0:
				ParseDoubleYKCancelCon(buf,terminalPtr);
				break;

			default:
				{
					std::ostringstream ostr;
					ostr.str("");
					ostr<<"YK��У���Ĵ���δ�����YK_CODE ="<<yk_code<<std::endl;
					AddStatusLogWithSynT(ostr.str());
				}
				break;
			}
		}
		break;

	case M_IR_NA_3:
		//ostr.str("");//zyq
		//ostr<<"�յ�sub���-1="<<terminalPtr<<"��05װ�ñ�ʶ���ģ�";
		//AddStatusLogWithSynT(ostr.str());
		ParseDevMark(buf,terminalPtr);
		//AddStatusLogWithSynT("�����ɹ�;\n");//zyq

		break;

	case M_SYN_TA_3://06��ʱ
		ParseSynTimeCon(buf,terminalPtr);
		break;

	case M_TGI_NA_3://���ٽ���
		ParseAllDataCallOver(buf,terminalPtr);
		//AddStatusLogWithSynT("�յ����ٽ�������;\n");//zyq

		break;

	case M_TM_TA_3://1
		//ParseDoubleYxWithTE(buf,terminalPtr);
		{

			if ( TransReason == 0x01 )
			{
				ParseDoubleProtectYxWithSOE(buf,terminalPtr);
			}
			else if ( TransReason == 0x09 )
			{

				ParseDoubleProtectYxWithCallAll(buf,terminalPtr);
				//ostr.str("");
				//ostr<<"���٣�������Ϣ���ͱ��ģ�terminalPtr="<<terminalPtr<<std::endl;
				//AddStatusLogWithSynT(ostr.str());
			}
			else
			{
				ostr.str("");
				ostr<<"������Ϣ���ͱ��Ĵ���δ�����TransReason ="<<TransReason<<std::endl;
				AddStatusLogWithSynT(ostr.str());
			}
		}
		break;

	case M_TMR_TA_3://2
		ParseDoubleProtectYxWithTE(buf,terminalPtr);
		break;

	case ASDU51:
		ParseYCDataWithValid(buf,terminalPtr);
		break;

	case M_DP_NA_3:
		ParseDoubleYx(buf,terminalPtr);
		break;

	case M_DP_TA_3:
		ParseDoubleYxWithTE(buf,terminalPtr);
		break;

	case M_MEIII_NA_3:
		ParseYCDataWithValid(buf,terminalPtr);
		break;

	case M_MEII_NA_3:
		ParseYCDataWithValid(buf,terminalPtr);
		break;

	case M_MEI_NA_3:
		ParseYCDataWithValid(buf,terminalPtr);
		break;

	case M_GD_NA_3:
		ParseAssortData(buf,terminalPtr);
		 break;

	case 23:
		break;

	default:
		{
//			std::ostringstream ostr;
			/*ostr.str("");
			ostr<<"���ձ��Ĵ���δ����ı������� FRAME_TYPE ="<<FrameType<<std::endl;
			AddStatusLogWithSynT(ostr.str());*/
		}
		break;
	}

	return 0;
}

int CH103::ParseSynTimeCon(unsigned char * buf, share_terminal_ptr terminalPtr)
{
	terminalPtr->setSynTCommPointFlag(true);

	ResetTimerSynTime(terminalPtr,true);

	return 0;
}

int CH103::ParseAllDataCallOver(unsigned char * buf, share_terminal_ptr terminalPtr)
{
	terminalPtr->setInitCommPointFlag(true);

	ResetTimerCallAllData(terminalPtr,true);

	return 0;
}

int CH103::ParseDevMark(unsigned char * buf, share_terminal_ptr terminalPtr)
{
	return 0;
}

int CH103::ParseYCDataWithValid(unsigned char * buf, share_terminal_ptr terminalPtr)
{
	int info_num = BufToVal(&buf[InfoNumLocation_],InfoNumLength_) & (~(0x80<<(InfoNumLength_ - 1) * 8));
	int info_ISQ = (BufToVal(&buf[InfoNumLocation_],InfoNumLength_) >> ((InfoNumLength_ - 1) * 8)) & 0x80;
	
	if (info_num <= 0)
	{
		AddStatusLogWithSynT( "�յ�YC���ģ�������Ϣ����Ŀ���󣬲�����ñ��ġ�\n" );
		return -1;
	}

	if (info_num > (int)terminalPtr->getRecvYcSum())
	{
		info_num = (int)terminalPtr->getRecvYcSum();
	}

	int count = 0;

	if (info_ISQ == DisableISQ)
	{
		int InfoDataLength = 2;
		int info_addr = BufToVal(&buf[InfoAddrLocation_],InfoAddrLength_) - YC_START_ADDR_;

		for (int i=0;i<info_num;i++)
		{
			if (info_addr+i >= 0 && info_addr+i < (int)terminalPtr->getRecvYcSum())
			{
				unsigned short yc = BufToVal(&buf[DataLocation_ + i*InfoDataLength],InfoDataLength);
				unsigned char ycVaild = yc & 0x07;
				unsigned short signBit = yc & 0x8000;
				unsigned short ycVal = 0;
				if (signBit == 0)
				{
					ycVal = ((yc & 0x7ff8) >> 3);
				}
				else if (signBit == 1)
				{
					ycVal = (~(((~(yc & 0x7ff8)) + 0x0008) >> 3)) + 0x0001;
				}
			
				terminalPtr->SaveYcQuality(info_addr + i,ycVaild);
				int ret = terminalPtr->SaveOriYcVal(info_addr + i,ycVal,terminalPtr->getInitCommPointFlag());
				if (ret == DataBase::CauseActiveData)
				{
					count++;
				}
			}
		}
	} 
	else if(info_ISQ == EnableISQ)
	{
		int InfoDataLength = InfoAddrLength_ + 2;

		for (int i=0;i<info_num;i++)
		{
			int info_addr = BufToVal(&buf[InfoAddrLocation_ + i*InfoDataLength],InfoAddrLength_) - YC_START_ADDR_;
			if (info_addr >= 0 && info_addr < (int)terminalPtr->getRecvYcSum())
			{
				unsigned short yc = BufToVal(&buf[DataLocation_ + i*InfoDataLength],2);
				unsigned char ycVaild = yc & 0x07;
				unsigned short signBit = yc & 0x8000;
				//unsigned short ycVal = ((yc & 0x7ff8) >> 3) | signBit;
				unsigned short ycVal = 0;
				if (signBit == 0)
				{
					ycVal = ((yc & 0x7ff8) >> 3);
				}
				else if (signBit == 1)
				{
					ycVal = (~(((~(yc & 0x7ff8)) + 0x0008) >> 3)) + 0x0001;
				}
				terminalPtr->SaveYcQuality(info_addr,ycVaild);
				int ret = terminalPtr->SaveOriYcVal(info_addr,ycVal,terminalPtr->getInitCommPointFlag());
				if (ret == DataBase::CauseActiveData)
				{
					count++;
				}
			}
		}
	}

	if (count > 0)
	{
		std::ostringstream ostr;
		ostr.str("");
		ostr<<"��У��λ��YC���Ĳ�����ycvar����Ŀ��"<<count<<std::endl;
		AddStatusLogWithSynT(ostr.str());
		CmdConSig_(YCVAR_DATA_UP,RETURN_CODE_ACTIVE,terminalPtr,count);
	}

	return 0;
}

int CH103::ParseAllDoubleYXByte(unsigned char * buf, share_terminal_ptr terminalPtr)
{
	int info_num = BufToVal(&buf[InfoNumLocation_],InfoNumLength_) & (~(0x80<<(InfoNumLength_ - 1) * 8));
	int info_ISQ = (BufToVal(&buf[InfoNumLocation_],InfoNumLength_) >> ((InfoNumLength_ - 1) * 8)) & 0x80;

	if (info_num == 0 || (info_num - 1)*16 > (int)terminalPtr->getRecvYxSum())
	{
		AddStatusLogWithSynT( "�յ�˫��ȫYX�ֱ��ģ�������Ϣ����Ŀ���󣬲�����ñ��ġ�\n" );
		return -1;
	}

	int count = 0;

	if (info_ISQ == DisableISQ)
	{
		int InfoDataLength = 5;
		int info_addr = BufToVal(&buf[InfoAddrLocation_],InfoAddrLength_) - DYX_START_ADDR_;

		for (int i=0;i<info_num;i++)
		{
			unsigned char yxQuality = buf[DataLocation_ + 4 + i*InfoDataLength] & 0xf0;

			unsigned char yx_val = buf[DataLocation_ + i*InfoDataLength];
			for (int j=0;j<4;j++)
			{
				int index = info_addr + i*8 + j;
				if (index >= 0 && index < (int)terminalPtr->getRecvYxSum())
				{
					unsigned char lowBit = 0;
					unsigned char hightBit = 0;

					if ((yx_val & BYTE_CHECK_TRUE[j*2]) > 0)
					{
						lowBit = 0x01;
					}
					else
					{
						lowBit = 0;
					}

					if ((yx_val & BYTE_CHECK_TRUE[j*2 + 1]) > 0)
					{
						hightBit = 0x02;
					}
					else
					{
						hightBit = 0;
					}

					terminalPtr->SaveYxType(index,DataBase::double_yx_point);
					int ret = terminalPtr->SaveOriYxVal(index,(lowBit | hightBit) & 0x03);
					if (ret = DataBase::CauseActiveData)
					{
						count++;
					}
				}

				terminalPtr->SaveYxQuality(index,yxQuality);
			}

			yx_val = buf[DataLocation_ + 1 + i*InfoDataLength];
			for (int j=0;j<4;j++)
			{
				int index = info_addr + i*8 + 4 + j;
				if((index >= 0) && (index < (int)terminalPtr->getRecvYxSum()))
				{
					unsigned char lowBit = 0;
					unsigned char hightBit = 0;

					if ((yx_val & BYTE_CHECK_TRUE[j*2]) > 0)
					{
						lowBit = 0x01;
					}
					else
					{
						lowBit = 0;
					}

					if ((yx_val & BYTE_CHECK_TRUE[j*2 + 1]) > 0)
					{
						hightBit = 0x02;
					}
					else
					{
						hightBit = 0;
					}

					terminalPtr->SaveYxType(index,DataBase::double_yx_point);
					int ret = terminalPtr->SaveOriYxVal(index,(lowBit | hightBit) & 0x03);
					if (ret = DataBase::CauseActiveData)
					{
						count++;
					}

				}

				terminalPtr->SaveYxQuality(index,yxQuality);
			}
		}
	}
	else if (info_ISQ == EnableISQ)
	{
		int InfoDataLength = InfoAddrLength_ + 5;

		for (int i=0;i<info_num;i++)
		{
			int info_addr = BufToVal(&buf[InfoAddrLocation_ + i*InfoDataLength],InfoAddrLength_) - DYX_START_ADDR_;
			unsigned char yxQuality = buf[DataLocation_ + 4 + i*InfoDataLength] & 0xf0;

			unsigned char yx_val = buf[DataLocation_ + i*InfoDataLength];
			for (int j=0;j<4;j++)
			{
				int index = info_addr + j;
				if (index >= 0 && index < (int)terminalPtr->getRecvYxSum())
				{
					unsigned char lowBit = 0;
					unsigned char hightBit = 0;

					if ((yx_val & BYTE_CHECK_TRUE[j*2]) > 0)
					{
						lowBit = 0x01;
					}
					else
					{
						lowBit = 0;
					}

					if ((yx_val & BYTE_CHECK_TRUE[j*2 + 1]) > 0)
					{
						hightBit = 0x02;
					}
					else
					{
						hightBit = 0;
					}

					terminalPtr->SaveYxType(index,DataBase::double_yx_point);
					int ret = terminalPtr->SaveOriYxVal(index,(lowBit | hightBit) & 0x03);
					if (ret = DataBase::CauseActiveData)
					{
						count++;
					}
				}

				terminalPtr->SaveYxQuality(index,yxQuality);
			}

			yx_val = buf[DataLocation_ + 1 + i*InfoDataLength];
			for (int j=0;j<8;j++)
			{
				int index = info_addr + 4 + j;
				if((index >= 0) && (index < (int)terminalPtr->getRecvYxSum()))
				{
					unsigned char lowBit = 0;
					unsigned char hightBit = 0;

					if ((yx_val & BYTE_CHECK_TRUE[j*2]) > 0)
					{
						lowBit = 0x01;
					}
					else
					{
						lowBit = 0;
					}

					if ((yx_val & BYTE_CHECK_TRUE[j*2 + 1]) > 0)
					{
						hightBit = 0x02;
					}
					else
					{
						hightBit = 0;
					}

					terminalPtr->SaveYxType(index,DataBase::double_yx_point);
					int ret = terminalPtr->SaveOriYxVal(index,(lowBit | hightBit) & 0x03);
					if (ret = DataBase::CauseActiveData)
					{
						count++;
					}
				}

				terminalPtr->SaveYxQuality(index,yxQuality);
			}
		}
	}

	if (count > 0)
	{
		std::ostringstream ostr;
		ostr.str("");
		ostr<<"ȫ˫��YX�ֱ��Ĳ�����COS����Ŀ��"<<count<<std::endl;
		AddStatusLogWithSynT(ostr.str());
		CmdConSig_(COS_DATA_UP,RETURN_CODE_ACTIVE,terminalPtr,count);
	}

	return 0;
}

int CH103::ParseAllSingleYXByte(unsigned char * buf, share_terminal_ptr terminalPtr)
{
	int info_num = BufToVal(&buf[InfoNumLocation_],InfoNumLength_) & (~(0x80<<(InfoNumLength_ - 1) * 8));
	int info_ISQ = (BufToVal(&buf[InfoNumLocation_],InfoNumLength_) >> ((InfoNumLength_ - 1) * 8)) & 0x80;
		
	if (info_num == 0 || (info_num - 1)*16 > (int)terminalPtr->getRecvYxSum())
	{
		AddStatusLogWithSynT( "�յ�����ȫYX�ֱ��ģ�������Ϣ����Ŀ���󣬲�����ñ��ġ�\n" );
		return -1;
	}

	int count = 0;

	if (info_ISQ == DisableISQ)
	{
		int InfoDataLength = 5;
		int info_addr = BufToVal(&buf[InfoAddrLocation_],InfoAddrLength_) - SYX_START_ADDR_;

		for (int i=0;i<info_num;i++)
		{
			//unsigned char yxQuality = buf[DataLocation_ + 4 + i*InfoDataLength] & 0xf0;
			unsigned char yxQuality = buf[DataLocation_ + 4 + i*InfoDataLength] & 0xf1;

			unsigned char yx_val = buf[DataLocation_ + i*InfoDataLength];
			for (int j=0;j<8;j++)
			{
				int index = info_addr + i*16 + j;
				/*std::ostringstream ostr;//zyq
				ostr.str("");
     			ostr<<"�յ����ٵ���ң�ŵ��index="<<index;
				AddStatusLogWithSynT(ostr.str());*/

				if (index >= 0 && index < (int)terminalPtr->getRecvYxSum())
				{
					if ((yx_val & BYTE_CHECK_TRUE[j]) > 0)
					{
						terminalPtr->SaveYxType(index,DataBase::single_yx_point);
						int ret = terminalPtr->SaveOriYxVal(index,1);
						//AddStatusLogWithSynT(";ң��ֵΪ1;\n");//zyq
						if (ret == DataBase::CauseActiveData)
						{
							count++;
						}
					}
					else
					{
						terminalPtr->SaveYxType(index,DataBase::single_yx_point);
						int ret = terminalPtr->SaveOriYxVal(index,0);
						//AddStatusLogWithSynT(";ң��ֵΪ0;\n");//zyq

						if (ret == DataBase::CauseActiveData)
						{
							count++;
						}
					}
				}

				terminalPtr->SaveYxQuality(index,yxQuality);

			}

			yx_val = buf[DataLocation_ + 1 + i*InfoDataLength];
			for (int j=0;j<8;j++)
			{
				int index = info_addr + i*16 + 8 + j;
				if((index >= 0) && (index < (int)terminalPtr->getRecvYxSum()))
				{
					if ((yx_val & BYTE_CHECK_TRUE[j]) > 0)
					{
						terminalPtr->SaveYxType(index,DataBase::single_yx_point);
						int ret = terminalPtr->SaveOriYxVal(index,1);
						//AddStatusLogWithSynT("ң��ֵΪ1;\n");//zyq

						if (ret == DataBase::CauseActiveData)
						{
							count++;
						}
					}
					else
					{
						terminalPtr->SaveYxType(index,DataBase::single_yx_point);
						int ret = terminalPtr->SaveOriYxVal(index,0);
						//AddStatusLogWithSynT("ң��ֵΪ0;\n");//zyq
						if (ret == DataBase::CauseActiveData)
						{
							count++;
						}
					}
				}

				terminalPtr->SaveYxQuality(index,yxQuality);
			}
		}
	}
	else if (info_ISQ == EnableISQ)
	{
		int InfoDataLength = InfoAddrLength_ + 5;

		for (int i=0;i<info_num;i++)
		{
			int info_addr = BufToVal(&buf[InfoAddrLocation_ + i*InfoDataLength],InfoAddrLength_) - SYX_START_ADDR_;
			//unsigned char yxQuality = buf[DataLocation_ + 4 + i*InfoDataLength] & 0xf0;
			unsigned char yxQuality = buf[DataLocation_ + 4 + i*InfoDataLength] & 0xf1;

			unsigned char yx_val = buf[DataLocation_ + i*InfoDataLength];
			for (int j=0;j<8;j++)
			{
				int index = info_addr + j;
				if (index >= 0 && index < (int)terminalPtr->getRecvYxSum())
				{
					if ((yx_val & BYTE_CHECK_TRUE[j]) > 0)
					{
						terminalPtr->SaveYxType(index,DataBase::single_yx_point);
						int ret = terminalPtr->SaveOriYxVal(index,1);
						if (ret == DataBase::CauseActiveData)
						{
							count++;
						}
					}
					else
					{
						terminalPtr->SaveYxType(index,DataBase::single_yx_point);
						int ret = terminalPtr->SaveOriYxVal(index,0);
						if (ret == DataBase::CauseActiveData)
						{
							count++;
						}
					}
				}

				terminalPtr->SaveYxQuality(index,yxQuality);
			}

			yx_val = buf[DataLocation_ + 1 + i*InfoDataLength];
			for (int j=0;j<8;j++)
			{
				int index = info_addr + 8 + j;
				if((index >= 0) && (index < (int)terminalPtr->getRecvYxSum()))
				{
					if ((yx_val & BYTE_CHECK_TRUE[j]) > 0)
					{
						terminalPtr->SaveYxType(index,DataBase::single_yx_point);
						int ret = terminalPtr->SaveOriYxVal(index,1);
						if (ret == DataBase::CauseActiveData)
						{
							count++;
						}
					}
					else
					{
						terminalPtr->SaveYxType(index,DataBase::single_yx_point);
						int ret = terminalPtr->SaveOriYxVal(index,0);
						if (ret == DataBase::CauseActiveData)
						{
							count++;
						}
					}
				}

				terminalPtr->SaveYxQuality(index,yxQuality);
			}
		}
	}

	if (count > 0)
	{
		std::ostringstream ostr;
		ostr.str("");
		ostr<<"ȫ����YX�ֱ��Ĳ�����COS����Ŀ��"<<count<<std::endl;
		AddStatusLogWithSynT(ostr.str());
		CmdConSig_(COS_DATA_UP,RETURN_CODE_ACTIVE,terminalPtr,count);
	}

	return 0;
}

int CH103::ParseSingleYx(unsigned char * buf, share_terminal_ptr terminalPtr)
{
	int info_num = BufToVal(&buf[InfoNumLocation_],InfoNumLength_) & (~(0x80<<(InfoNumLength_ - 1) * 8));
	int info_ISQ = (BufToVal(&buf[InfoNumLocation_],InfoNumLength_) >> ((InfoNumLength_ - 1) * 8)) & 0x80;
	
	if (info_num <= 0)
	{
		AddStatusLogWithSynT("�յ�����COS���ģ�������Ϣ����Ŀ���󣬲�����ñ��ġ�\n" );
		return -1;
	}

	if (info_ISQ == DisableISQ)
	{
		int InfoDataLength = 1;
		int info_addr = BufToVal(&buf[InfoAddrLocation_],InfoAddrLength_) - SYX_START_ADDR_;

		for (int i=0;i<info_num;i++)
		{
			if ((info_addr + i >= 0) && (info_addr + i < (int)terminalPtr->getRecvYxSum()))
			{
				unsigned char YxQuality = buf[DataLocation_ + i*InfoDataLength] & 0xf0;
				unsigned char YxBitVal = buf[DataLocation_ + i*InfoDataLength] & 0x01;

				terminalPtr->SaveYxQuality(info_addr+i,YxQuality);
				terminalPtr->SaveYxType(info_addr+i,DataBase::single_yx_point);
				terminalPtr->SaveCosPoint(info_addr+i,YxBitVal,DataBase::single_yx_point,YxQuality);

				std::ostringstream ostr;
				ostr.str("");
				ostr<<"�յ�����COS�����"<<info_addr+i<<"��YXֵ"<<std::hex<<std::uppercase<<std::setw(2)<<std::setfill('0')<<(short)YxBitVal<<std::endl;
				AddStatusLogWithSynT(ostr.str());
			} 
			else
			{
				AddStatusLogWithSynT("�յ�����COS��������Ϣ���ַ���󣬲�����õ㡣\n");
			}
		}
	} 
	else if(info_ISQ == EnableISQ)
	{
		int InfoDataLength = 3;

		for (int i=0;i<info_num;i++)
		{
			int info_addr = BufToVal(&buf[InfoAddrLocation_ + i*InfoDataLength],InfoAddrLength_) - SYX_START_ADDR_;
			if (info_addr >= 0 && info_addr < (int)terminalPtr->getRecvYxSum())
			{
				unsigned char YxQuality = buf[DataLocation_ + i*InfoDataLength] & 0xf0;
				unsigned char YxBitVal = buf[DataLocation_ + i*InfoDataLength] & 0x01;

				terminalPtr->SaveYxQuality(info_addr,YxQuality);
				terminalPtr->SaveYxType(info_addr,DataBase::single_yx_point);
				terminalPtr->SaveCosPoint(info_addr,YxBitVal,DataBase::single_yx_point,YxQuality);

				std::ostringstream ostr;
				ostr.str("");
				ostr<<"�յ�����COS�����"<<info_addr<<"��YXֵ"<<std::hex<<std::uppercase<<std::setw(2)<<std::setfill('0')<<(short)YxBitVal<<std::endl;
				AddStatusLogWithSynT(ostr.str());
			} 
			else
			{
				AddStatusLogWithSynT("�յ�����COS��������Ϣ���ַ���󣬲�����õ㡣\n");
			}
		}
	}

	if (info_num > 0)
	{
		CmdConSig_(COS_DATA_UP,RETURN_CODE_ACTIVE,terminalPtr,info_num);
		//AddStatusLogWithSynT("�յ�����COS�����COS������վ��\n");//zyq

	}

	return 0;
}

int CH103::ParseSingleYxWithTE(unsigned char * buf, share_terminal_ptr terminalPtr)
{
	int info_num = BufToVal(&buf[InfoNumLocation_],InfoNumLength_) & (~(0x80<<(InfoNumLength_ - 1) * 8));
	int info_ISQ = (BufToVal(&buf[InfoNumLocation_],InfoNumLength_) >> ((InfoNumLength_ - 1) * 8)) & 0x80;

	if (info_num <= 0)
	{
		AddStatusLogWithSynT("�յ�����SOE���ģ�������Ϣ����Ŀ���󣬲�����ñ��ġ�\n" );
		return -1;
	}

	/*
	if (info_ISQ == DisableISQ)
	{
		AddStatusLogWithSynT( "�յ�����SOE���ģ�������Ϣ��ISQλ���󣬲�����ñ��ġ�\n" );
		return -1;
	}
	*/

	using namespace boost::posix_time;
	ptime lt = boost::posix_time::microsec_clock::local_time();

	if (info_ISQ == DisableISQ)
	{
		unsigned char frame_length = buf[1] + 4;
		unsigned char InfoDataLength = (frame_length - DataLocation_) / info_num;
		if ( InfoDataLength >= 8)
		{
			InfoDataLength = 8;
		}
		else if (InfoDataLength < 8 && InfoDataLength >= 5)
		{
			InfoDataLength = 5;
		}

		int info_addr = BufToVal(&buf[InfoAddrLocation_],InfoAddrLength_) - SYX_START_ADDR_;

		for (int i=0;i<info_num;i++)
		{
			if (info_addr + i>= 0 && info_addr + i< (int)terminalPtr->getRecvYxSum())
			{
				unsigned char YxQuality = buf[DataLocation_ + i*InfoDataLength] & 0xf0; 
				unsigned char YxBitVal = buf[DataLocation_ + i*InfoDataLength] & 0x01;
				unsigned short millisecond = BufToVal(&buf[DataLocation_ + 1 + i*InfoDataLength],2);
				unsigned char minute = buf[DataLocation_ + 3 + i*InfoDataLength] & 0x3f;
				unsigned char Hour = buf[DataLocation_ + 4 + i*InfoDataLength] & 0x1f;
				time_duration td(Hour,minute,millisecond / 1000,millisecond % 1000);

				std::ostringstream ostr;

				if (InfoDataLength >= 8)
				{
					unsigned char Day = buf[DataLocation_ + 5 + i*InfoDataLength] & 0x1f;
					unsigned char Month = buf[DataLocation_ + 6 + i*InfoDataLength] & 0x0f;
					unsigned short Year = (buf[DataLocation_ + 7 + i*InfoDataLength] & 0x7f) + (((lt.date()).year() / 100) * 100);
					boost::gregorian::date dt(Year,Month,Day);
					ptime timeVal(dt,td);

					terminalPtr->SaveYxQuality(info_addr+i,YxQuality);
					terminalPtr->SaveYxType(info_addr+i,DataBase::single_yx_point);
					terminalPtr->SaveSoePoint(info_addr+i,YxBitVal,DataBase::single_yx_point,timeVal,YxQuality);

					ostr.str("");
					ostr<<"�յ�����SOE�����"<<info_addr+i<<"��YXֵ"<<std::hex<<std::uppercase<<std::setw(2)<<std::setfill('0')<<(short)YxBitVal<<"��"<<to_simple_string(timeVal)<<std::endl;
				}
				else
				{
					ptime timeVal(lt.date(),td);

					terminalPtr->SaveYxQuality(info_addr+i,YxQuality);
					terminalPtr->SaveYxType(info_addr+i,DataBase::single_yx_point);
					terminalPtr->SaveSoePoint(info_addr+i,YxBitVal,DataBase::single_yx_point,timeVal,YxQuality);

					ostr.str("");
					ostr<<"�յ�����SOE�����"<<info_addr+i<<"��YXֵ"<<std::hex<<std::uppercase<<std::setw(2)<<std::setfill('0')<<(short)YxBitVal<<"��"<<to_simple_string(timeVal)<<std::endl;
				}

				AddStatusLogWithSynT(ostr.str());
			}
		}
	} 
	else if(info_ISQ == EnableISQ)
	{
		unsigned char frame_length = buf[1] + 4;
		unsigned char InfoDataLength = (frame_length - InfoAddrLocation_) / info_num;
		if ( InfoDataLength >= InfoAddrLength_ + 8)
		{
			InfoDataLength = InfoAddrLength_ + 8;
		}
		else if (InfoDataLength < InfoAddrLength_ + 8 && InfoDataLength >= InfoAddrLength_ + 5)
		{
			InfoDataLength = InfoAddrLength_ + 5;
		}

		for (int i=0;i<info_num;i++)
		{
			int info_addr = BufToVal(&buf[InfoAddrLocation_ + i*InfoDataLength],InfoAddrLength_) - SYX_START_ADDR_;
			if (info_addr >= 0 && info_addr < (int)terminalPtr->getRecvYxSum())
			{
				unsigned char YxQuality = buf[DataLocation_ + i*InfoDataLength] & 0xf0; 
				unsigned char YxBitVal = buf[DataLocation_ + i*InfoDataLength] & 0x01;
				unsigned short millisecond = BufToVal(&buf[DataLocation_ + 1 + i*InfoDataLength],2);
				unsigned char minute = buf[DataLocation_ + 3 + i*InfoDataLength] & 0x3f;
				//unsigned char Hour = buf[DataLocation_ + 4 + i*InfoDataLength] & 0x1f;
				unsigned char Hour = buf[DataLocation_ + 4 + i*InfoDataLength] & 0x7f;
				time_duration td(Hour,minute,millisecond / 1000,millisecond % 1000);

				std::ostringstream ostr;

				if (InfoDataLength >= InfoAddrLength_ + 8)
				{
					unsigned char Day = buf[DataLocation_ + 5 + i*InfoDataLength] & 0x1f;
					unsigned char Month = buf[DataLocation_ + 6 + i*InfoDataLength] & 0x0f;
					unsigned short Year = (buf[DataLocation_ + 7 + i*InfoDataLength] & 0x7f) + (((lt.date()).year() / 100) * 100);
					boost::gregorian::date dt(Year,Month,Day);
					ptime timeVal(dt,td);

					terminalPtr->SaveYxQuality(info_addr,YxQuality);
					terminalPtr->SaveYxType(info_addr,DataBase::single_yx_point);
					terminalPtr->SaveSoePoint(info_addr,YxBitVal,DataBase::single_yx_point,timeVal,YxQuality);

					ostr.str("");
					ostr<<"�յ�����SOE�����"<<info_addr<<"��YXֵ"<<std::hex<<std::uppercase<<std::setw(2)<<std::setfill('0')<<(short)YxBitVal<<"��"<<to_simple_string(timeVal)<<std::endl;
				}
				else
				{
					ptime timeVal(lt.date(),td);

					terminalPtr->SaveYxQuality(info_addr,YxQuality);
					terminalPtr->SaveYxType(info_addr,DataBase::single_yx_point);
					terminalPtr->SaveSoePoint(info_addr,YxBitVal,DataBase::single_yx_point,timeVal,YxQuality);

					ostr.str("");
					ostr<<"�յ�����SOE�����"<<info_addr<<"��YXֵ"<<std::hex<<std::uppercase<<std::setw(2)<<std::setfill('0')<<(short)YxBitVal<<"��"<<to_simple_string(timeVal)<<std::endl;
				}

				AddStatusLogWithSynT(ostr.str());
			} 
			else
			{
				AddStatusLogWithSynT("�յ�����SOE��������Ϣ���ַ���󣬲�����õ㡣\n");
			}
		}
	}

	if (info_num > 0)
	{
		CmdConSig_(SOE_DATA_UP,RETURN_CODE_ACTIVE,terminalPtr,info_num);
		//AddStatusLogWithSynT("�յ�����COS�����COS������վ��\n");//zyq

	}

	return 0;
}

int CH103::ParseYMData(unsigned char * buf, share_terminal_ptr terminalPtr)
{
	int info_num = BufToVal(&buf[InfoNumLocation_],InfoNumLength_) & (~(0x80<<(InfoNumLength_ - 1) * 8));
	int info_ISQ = (BufToVal(&buf[InfoNumLocation_],InfoNumLength_) >> ((InfoNumLength_ - 1) * 8)) & 0x80;

	if (info_num <= 0)
	{
		AddStatusLogWithSynT( "�յ�ȫYM���ģ�������Ϣ����Ŀ���󣬲�����ñ��ġ�\n" );
		return -1;
	}

	if (info_num > (int)terminalPtr->getRecvYmSum())
	{
		info_num = terminalPtr->getRecvYmSum();
	}

	if (info_ISQ == DisableISQ)
	{
		int InfoDataLength  = 5;
		int info_addr = BufToVal(&buf[InfoAddrLocation_],InfoAddrLength_) - YM_START_ADDR_;

		for (int i=0;i<info_num;i++)
		{
			if (info_addr + i>= 0 && info_addr + i< (int)terminalPtr->getRecvYmSum())
			{
				unsigned int YmVal = BufToVal(&buf[DataLocation_ + i*InfoDataLength],4);
				//unsigned char YmSQ = BufToVal(&buf[DataLocation_ + 4 + i*InfoDataLength],1) & 0x1f;
				//unsigned char YmQuality = BufToVal(&buf[DataLocation_ + 4 + i*InfoDataLength],1) & 0xe0;
				unsigned char YmQuality = BufToVal(&buf[DataLocation_ + 4 + i*InfoDataLength],1);

				terminalPtr->SaveYmQuality(info_addr + i,YmQuality);
				terminalPtr->SaveOriYmVal(info_addr + i,YmVal);
			}
		}
		unsigned char RII = buf[DataLocation_ + 5 + (info_num - 1)*InfoDataLength];
	}
	else if (info_ISQ == EnableISQ)
	{
		int InfoDataLength = InfoAddrLength_ + 5;

		for (int i=0;i<info_num;i++)
		{
			int info_addr = BufToVal(&buf[InfoAddrLocation_ + i*InfoDataLength],InfoAddrLength_) - YM_START_ADDR_;

			if (info_addr >= 0 && info_addr < (int)terminalPtr->getRecvYmSum())
			{
				unsigned int YmVal = BufToVal(&buf[DataLocation_ + i*InfoDataLength],4);
				unsigned char YmQuality = BufToVal(&buf[DataLocation_ + 4 + i*InfoDataLength],1);

				terminalPtr->SaveYmQuality(info_addr + i,YmQuality);
				terminalPtr->SaveOriYmVal(info_addr + i,YmVal);
			}
		}
	}

	return 0;
}

int CH103::ParseYMDataWithTE(unsigned char * buf, share_terminal_ptr terminalPtr)
{
	int info_num = BufToVal(&buf[InfoNumLocation_],InfoNumLength_) & (~(0x80<<(InfoNumLength_ - 1) * 8));
	int info_ISQ = (BufToVal(&buf[InfoNumLocation_],InfoNumLength_) >> ((InfoNumLength_ - 1) * 8)) & 0x80;

	if (info_num <= 0)
	{
		AddStatusLogWithSynT( "�յ�ȫYM���ģ�������Ϣ����Ŀ���󣬲�����ñ��ġ�\n" );
		return -1;
	}

	if (info_num > (int)terminalPtr->getRecvYmSum())
	{
		info_num = (int)terminalPtr->getRecvYmSum();
	}

	if (info_ISQ == DisableISQ)
	{
		int InfoDataLength  = 9;
		int info_addr = BufToVal(&buf[InfoAddrLocation_],InfoAddrLength_) - YM_START_ADDR_;

		for (int i=0;i<info_num;i++)
		{
			if (info_addr + i>= 0 && info_addr + i< (int)terminalPtr->getRecvYmSum())
			{
				unsigned int YmVal = BufToVal(&buf[DataLocation_ + i*InfoDataLength],4);
				unsigned char YmQuality = BufToVal(&buf[DataLocation_ + 4 + i*InfoDataLength],1);

				terminalPtr->SaveYmQuality(info_addr + i,YmQuality);
				terminalPtr->SaveOriYmVal(info_addr + i,YmVal);
			}
		}
	}
	else if (info_ISQ == EnableISQ)
	{
		int InfoDataLength = InfoAddrLength_ + 9;

		for (int i=0;i<info_num;i++)
		{
			int info_addr = BufToVal(&buf[InfoAddrLocation_ + i*InfoDataLength],InfoAddrLength_) - YM_START_ADDR_;

			if (info_addr >= 0 && info_addr < (int)terminalPtr->getRecvYmSum())
			{
				unsigned int YmVal = BufToVal(&buf[DataLocation_ + i*InfoDataLength],4);
				unsigned char YmQuality = BufToVal(&buf[DataLocation_ + 4 + i*InfoDataLength],1);

				terminalPtr->SaveYmQuality(info_addr + i,YmQuality);
				terminalPtr->SaveOriYmVal(info_addr + i,YmVal);
			}
		}
	}

	return 0;
}

int CH103::ParseDoubleYKSelCon(unsigned char * buf, share_terminal_ptr terminalPtr)
{
	int yk_no = BufToVal(&buf[InfoAddrLocation_],InfoAddrLength_) - DYK_START_ADDR_;
	unsigned char yk_type = buf[DataLocation_] & 0x03;
	std::ostringstream ostr;//zyq
	ostr.str("");
	ostr<<"yk_no = "<<yk_no<<" �յ�ң��ѡ��У����,����˫��ң��ѡ���������"<<"yk_type = "<<(int)yk_type<<std::endl;
	AddStatusLogWithSynT(ostr.str());

	if (yk_no < 0 || yk_no >= (int)terminalPtr->getYkSum())
	{
//		std::ostringstream ostr;//zyq
		ostr.str("");
		ostr<<"yk_no = "<<yk_no<<" �յ�ң��ѡ��У���ģ�����ң�ص�Ų����ϣ��˳��������֡���ġ�"<<std::endl;
		AddStatusLogWithSynT(ostr.str());
		return -1;
	}

	//if (!DataBase::CYkPoint::CheckYkStatusDevelopWithSel(terminalPtr->getYkStatus(yk_no),DataBase::YkSelCon))
	if((terminalPtr->loadYkPointPtr(yk_no))->SelResponEvent())
	{
		std::ostringstream ostr;
		//ostr.str("");
		//ostr<<"curStatus:"<<(int)terminalPtr->getYkStatus(yk_no)<<"NextStatus:"<<(int)DataBase::YkSelCon<<std::endl;
		//AddStatusLogWithSynT(ostr.str());
		AddStatusLogWithSynT("�յ�103ң��ѡ��У���ģ����ǵ�ǰң��״̬�����ϣ��˳��������֡���ġ�\n");
		return -1;
	}

	if (yk_type == DYK_TYPE_OPEN)
	{
		if(terminalPtr->getYkType(yk_no) != DataBase::YkOpen)
		{
			ResetTimerYkSel(terminalPtr,yk_no,false);
			//terminalPtr->setYkStatus(yk_no,DataBase::YkReady);
			(terminalPtr->loadYkPointPtr(yk_no))->ClearYkState();
			CmdConSig_(YK_SEL_CON,RETURN_CODE_NEGATIVE,terminalPtr,yk_no);
			AddStatusLogWithSynT("ң��ѡ��У���ĵ�ң��������Ԥ�ڲ�����\n");
			return -1;
		}
	}
	else if (yk_type == DYK_TYPE_CLOSE)
	{
		if (terminalPtr->getYkType(yk_no) != DataBase::YkClose)
		{
			ResetTimerYkSel(terminalPtr,yk_no,false);
			//terminalPtr->setYkStatus(yk_no,DataBase::YkReady);
			(terminalPtr->loadYkPointPtr(yk_no))->ClearYkState();
			CmdConSig_(YK_SEL_CON,RETURN_CODE_NEGATIVE,terminalPtr,yk_no);
			AddStatusLogWithSynT("ң��ѡ��У���ĵ�ң��������Ԥ�ڲ�����\n");
			return -1;
		}
	}
	else
	{
		ResetTimerYkSel(terminalPtr,yk_no,false);
		//terminalPtr->setYkStatus(yk_no,DataBase::YkReady);
		(terminalPtr->loadYkPointPtr(yk_no))->ClearYkState();
		CmdConSig_(YK_SEL_CON,RETURN_CODE_NEGATIVE,terminalPtr,yk_no);
		AddStatusLogWithSynT("ң��ѡ��У���ĵ�ң��������Ԥ�ڲ�����\n");
		return -1;
	}

	ResetTimerYkSel(terminalPtr,yk_no,false);
	AddStatusLogWithSynT("ң��ѡ��������ʱ��ResetTimerYkSel��\n");

	//terminalPtr->setYkStatus(yk_no,DataBase::YkSelCon);
	CmdConSig_(YK_SEL_CON,RETURN_CODE_ACTIVE,terminalPtr,yk_no);
	ostr.str("");
	ostr<<"yk_no = "<<yk_no<<" ����վ����������ң��ѡ��У���ġ�RETURN_CODE_ACTIVE=2="<<(int)RETURN_CODE_ACTIVE<<std::endl;
	AddStatusLogWithSynT(ostr.str());

	return 0;
}

int CH103::ParseDoubleYKExeCon(unsigned char * buf, share_terminal_ptr terminalPtr)
{
	int yk_no = BufToVal(&buf[InfoAddrLocation_],InfoAddrLength_) - DYK_START_ADDR_;
	unsigned char yk_type = buf[DataLocation_] & 0x03;

	if (yk_no < 0 || yk_no >= (int)terminalPtr->getYkSum())
	{
		std::ostringstream ostr;
		ostr.str("");
		ostr<<"yk_no = "<<yk_no<<" �յ�ң��ִ�з�У���ģ�����ң�ص�Ų����ϣ��˳��������֡���ġ�"<<std::endl;
		AddStatusLogWithSynT(ostr.str());
		return -1;
	}

	//if (!DataBase::CYkPoint::CheckYkStatusDevelopWithSel(terminalPtr->getYkStatus(yk_no),DataBase::YkExeCon))
	if((terminalPtr->loadYkPointPtr(yk_no))->ExeResponEvent())
	{
		//std::ostringstream ostr;
		//ostr<<"curStatus:"<<(int)terminalPtr->getYkStatus(yk_no)<<"NextStatus:"<<(int)DataBase::YkExeCon<<std::endl;
		//AddStatusLogWithSynT(ostr.str());
		AddStatusLogWithSynT("�յ�ң��ִ�з�У���ģ����ǵ�ǰң��״̬�����ϣ��˳��������֡���ġ�\n");
		return -1;
	}

	if (yk_type == DYK_TYPE_OPEN)
	{
		if(terminalPtr->getYkType(yk_no) != DataBase::YkOpen)
		{
			ResetTimerYkExe(terminalPtr,yk_no,false);
			//terminalPtr->setYkStatus(yk_no,DataBase::YkReady);
			(terminalPtr->loadYkPointPtr(yk_no))->ClearYkState();
			CmdConSig_(YK_EXE_CON,RETURN_CODE_NEGATIVE,terminalPtr,yk_no);
			AddStatusLogWithSynT("ң��ִ�з�У���ĵ�ң��������Ԥ�ڲ�����\n");
			return -1;
		}
	}
	else if (yk_type == DYK_TYPE_CLOSE)
	{
		if (terminalPtr->getYkType(yk_no) != DataBase::YkClose)
		{
			ResetTimerYkExe(terminalPtr,yk_no,false);
			//terminalPtr->setYkStatus(yk_no,DataBase::YkReady);
			(terminalPtr->loadYkPointPtr(yk_no))->ClearYkState();
			CmdConSig_(YK_EXE_CON,RETURN_CODE_NEGATIVE,terminalPtr,yk_no);
			AddStatusLogWithSynT("ң��ִ�з�У���ĵ�ң��������Ԥ�ڲ�����\n");
			return -1;
		}
	}
	else
	{
		ResetTimerYkExe(terminalPtr,yk_no,false);
		//terminalPtr->setYkStatus(yk_no,DataBase::YkReady);
		(terminalPtr->loadYkPointPtr(yk_no))->ClearYkState();
		CmdConSig_(YK_EXE_CON,RETURN_CODE_NEGATIVE,terminalPtr,yk_no);
		AddStatusLogWithSynT("ң��ִ�з�У���ĵ�ң��������Ԥ�ڲ�����\n");
		return -1;
	}

	ResetTimerYkExe(terminalPtr,yk_no,false);
	//terminalPtr->setYkStatus(yk_no,DataBase::YkExeCon);
	CmdConSig_(YK_EXE_CON,RETURN_CODE_ACTIVE,terminalPtr,yk_no);

	return 0;
}

int CH103::ParseDoubleYKCancelCon(unsigned char * buf, share_terminal_ptr terminalPtr)
{
	int yk_no = BufToVal(&buf[InfoAddrLocation_],InfoAddrLength_) - DYK_START_ADDR_;
	unsigned char yk_type = buf[DataLocation_] & 0x03;

	if (yk_no < 0 || yk_no >= (int)terminalPtr->getYkSum())
	{
		std::ostringstream ostr;
		ostr.str("");
		ostr<<"yk_no = "<<yk_no<<" �յ�ң��ȡ����У���ģ�����ң�ص�Ų����ϣ��˳��������֡���ġ�"<<std::endl;
		AddStatusLogWithSynT(ostr.str());
		return -1;
	}

	//if (!DataBase::CYkPoint::CheckYkStatusDevelopWithSel(terminalPtr->getYkStatus(yk_no),DataBase::YkCancelCon))
	if((terminalPtr->loadYkPointPtr(yk_no))->CancelResponEvent())
	{
		//std::ostringstream ostr;
		//ostr<<"curStatus:"<<(int)terminalPtr->getYkStatus(yk_no)<<"NextStatus:"<<(int)DataBase::YkCancelCon<<std::endl;
		//AddStatusLogWithSynT(ostr.str());
		AddStatusLogWithSynT("�յ�ң��ȡ����У���ģ����ǵ�ǰң��״̬�����ϣ��˳��������֡���ġ�\n");
		return -1;
	}

	if (yk_type == DYK_TYPE_OPEN)
	{
		if(terminalPtr->getYkType(yk_no) != DataBase::YkOpen)
		{
			ResetTimerYkCancel(terminalPtr,yk_no,false);
			//terminalPtr->setYkStatus(yk_no,DataBase::YkReady);
			(terminalPtr->loadYkPointPtr(yk_no))->ClearYkState();
			CmdConSig_(YK_CANCEL_CON,RETURN_CODE_NEGATIVE,terminalPtr,yk_no);
			AddStatusLogWithSynT("ң��ȡ����У���ĵ�ң��������Ԥ�ڲ�����\n");
			return -1;
		}
	}
	else if (yk_type == DYK_TYPE_CLOSE)
	{
		if (terminalPtr->getYkType(yk_no) != DataBase::YkClose)
		{
			ResetTimerYkCancel(terminalPtr,yk_no,false);
			//terminalPtr->setYkStatus(yk_no,DataBase::YkReady);
			(terminalPtr->loadYkPointPtr(yk_no))->ClearYkState();
			CmdConSig_(YK_CANCEL_CON,RETURN_CODE_NEGATIVE,terminalPtr,yk_no);
			AddStatusLogWithSynT("ң��ȡ����У���ĵ�ң��������Ԥ�ڲ�����\n");
			return -1;
		}
	}
	else
	{
		ResetTimerYkCancel(terminalPtr,yk_no,false);
		//terminalPtr->setYkStatus(yk_no,DataBase::YkReady);
		(terminalPtr->loadYkPointPtr(yk_no))->ClearYkState();
		CmdConSig_(YK_CANCEL_CON,RETURN_CODE_NEGATIVE,terminalPtr,yk_no);
		AddStatusLogWithSynT("ң��ȡ����У���ĵ�ң��������Ԥ�ڲ�����\n");
		return -1;
	}

	ResetTimerYkCancel(terminalPtr,yk_no,false);
	//terminalPtr->setYkStatus(yk_no,DataBase::YkCancelCon);
	CmdConSig_(YK_CANCEL_CON,RETURN_CODE_ACTIVE,terminalPtr,yk_no);

	return 0;
}

int CH103::ParseDoubleYx(unsigned char * buf, share_terminal_ptr terminalPtr)
{
	int info_num = BufToVal(&buf[InfoNumLocation_],InfoNumLength_) & (~(0x80<<(InfoNumLength_ - 1) * 8));
	int info_ISQ = (BufToVal(&buf[InfoNumLocation_],InfoNumLength_) >> ((InfoNumLength_ - 1) * 8)) & 0x80;

	if (info_num <= 0)
	{
		AddStatusLogWithSynT("�յ�˫��COS���ģ�������Ϣ����Ŀ���󣬲�����ñ��ġ�\n" );
		return -1;
	}

	if (info_ISQ == DisableISQ)
	{
		int InfoDataLength = 1;
//		int info_addr = BufToVal(&buf[InfoAddrLocation_],InfoAddrLength_) - DYX_START_ADDR_;
		int info_addr = BufToVal(&buf[InfoAddrLocation_],InfoAddrLength_) + DYX_START_ADDR_ - SYX_START_ADDR_ - 1;

		for (int i=0;i<info_num;i++)
		{
			if ((info_addr + i >= 0) && (info_addr + i < (int)terminalPtr->getRecvYxSum()))
			{
				unsigned char YxQuality = buf[DataLocation_ + i*InfoDataLength] & 0xf0;
				unsigned char YxBitVal = buf[DataLocation_ + i*InfoDataLength] & 0x03;

				terminalPtr->SaveYxQuality(info_addr+i,YxQuality);
				terminalPtr->SaveYxType(info_addr+i,DataBase::double_yx_point);
				terminalPtr->SaveCosPoint(info_addr+i,YxBitVal,DataBase::double_yx_point,YxQuality);

				std::ostringstream ostr;
				ostr.str("");
				ostr<<"�յ�˫��COS�����"<<info_addr+i<<"��YXֵ"<<std::hex<<std::uppercase<<std::setw(2)<<std::setfill('0')<<(short)YxBitVal<<std::endl;
				AddStatusLogWithSynT(ostr.str());
			} 
			else
			{
				AddStatusLogWithSynT("�յ�˫��COS��������Ϣ���ַ���󣬲�����õ�\n");
			}
		}
	}
	else if (info_ISQ == EnableISQ)
	{
		int InfoDataLength = InfoAddrLength_ + 1;

		for (int i=0;i<info_num;i++)
		{
//			int info_addr = BufToVal(&buf[InfoAddrLocation_ + i*InfoDataLength],InfoAddrLength_) - DYX_START_ADDR_;
			int info_addr = BufToVal(&buf[InfoAddrLocation_],InfoAddrLength_) + DYX_START_ADDR_ - SYX_START_ADDR_ - 1;
			if (info_addr >= 0 && info_addr < (int)terminalPtr->getRecvYxSum())
			{
				unsigned char YxQuality = buf[DataLocation_ + i*InfoDataLength] & 0xf0;
				unsigned char YxBitVal = buf[DataLocation_ + i*InfoDataLength] & 0x03;

				terminalPtr->SaveYxQuality(info_addr,YxQuality);
				terminalPtr->SaveYxType(info_addr,DataBase::double_yx_point);
				terminalPtr->SaveCosPoint(info_addr,YxBitVal,DataBase::double_yx_point,YxQuality);

				std::ostringstream ostr;
				ostr.str("");
				ostr<<"�յ�˫��COS�����"<<info_addr<<"��YXֵ"<<std::hex<<std::uppercase<<std::setw(2)<<std::setfill('0')<<(short)YxBitVal<<std::endl;
				AddStatusLogWithSynT(ostr.str());
			} 
			else
			{
				AddStatusLogWithSynT("�յ�˫��COS��������Ϣ���ַ���󣬲�����õ�\n");
			}
		}
	}

	if (info_num > 0)
	{
		CmdConSig_(COS_DATA_UP,RETURN_CODE_ACTIVE,terminalPtr,info_num);
	}

	return 0;
}

int CH103::ParseDoubleProtectYxWithCallAll(unsigned char * buf, share_terminal_ptr terminalPtr)
{
	int info_num = BufToVal(&buf[InfoNumLocation_],InfoNumLength_) & (~(0x80<<(InfoNumLength_ - 1) * 8));
	int info_ISQ = (BufToVal(&buf[InfoNumLocation_],InfoNumLength_) >> ((InfoNumLength_ - 1) * 8)) & 0x80;
	AddStatusLogWithSynT( "���뱣����������\n" );

	if (info_num == 0 || (info_num - 1)*16 > (int)terminalPtr->getRecvYxSum())
	{
		AddStatusLogWithSynT( "�յ�����˫�㱣����ϢYX�ֱ��ģ�������Ϣ����Ŀ���󣬲�����ñ��ġ�\n" );
		return -1;
	}

	int count = 0;

	if (info_ISQ == DisableISQ)
	{
		int InfoDataLength = 5;
		int info_addr = BufToVal(&buf[InfoAddrLocation_],InfoAddrLength_) + DYX_START_ADDR_ - SYX_START_ADDR_ - 1;
		std::ostringstream ostr;//zyq
		ostr.str("");
		ostr<<"�յ����ٱ�����info_addr="<<info_addr<<"��DYX_START_ADDR_="<<(int)DYX_START_ADDR_<<std::endl;
		AddStatusLogWithSynT(ostr.str());

		for (int i=0;i<info_num;i++)
		{
			unsigned char yxQuality = buf[DataLocation_ + i*InfoDataLength] & 0xf0;

			unsigned char yx_val = buf[DataLocation_ + i*InfoDataLength];
			for (int j=0;j<1;j++)
			{
				AddStatusLogWithSynT( "����for��\n" );

				int index = info_addr + i*8 + j;
				if (index >= 0 && index < (int)terminalPtr->getRecvYxSum())
				{
					unsigned char lowBit = 0;
					unsigned char hightBit = 0;

					if ((yx_val & BYTE_CHECK_TRUE[j*2]) > 0)
					{
						lowBit = 0x01;
					}
					else
					{
						lowBit = 0;
					}

					if ((yx_val & BYTE_CHECK_TRUE[j*2 + 1]) > 0)
					{
						hightBit = 0x02;
					}
					else
					{
						hightBit = 0;
					}

					terminalPtr->SaveYxType(index,DataBase::double_yx_point);
					int ret = terminalPtr->SaveOriYxVal(index,(lowBit | hightBit) & 0x03);
					if (ret = DataBase::CauseActiveData)
					{
						count++;
					}
				}

				terminalPtr->SaveYxQuality(index,yxQuality);
			}
		}
	}
	else if (info_ISQ == EnableISQ)
	{
		int InfoDataLength = InfoAddrLength_ + 5;

		for (int i=0;i<info_num;i++)
		{
			int info_addr = BufToVal(&buf[InfoAddrLocation_ + i*InfoDataLength],InfoAddrLength_) + DYX_START_ADDR_ - SYX_START_ADDR_ - 1;
			std::ostringstream ostr;//zyq
			ostr.str("");
			ostr<<"�յ�����˫�㱣����Ϣ��info_addr="<<info_addr<<"�յ�����˫�㱣����Ϣ��DYX_START_ADDR_="<<(int)DYX_START_ADDR_<<std::endl;
			AddStatusLogWithSynT(ostr.str());
			unsigned char yxQuality = buf[DataLocation_ + i*InfoDataLength] & 0xf0;

			unsigned char yx_val = buf[DataLocation_ + i*InfoDataLength];

			for (int j=0;j<1;j++)
			{
				int index = info_addr + j;
				if (index >= 0 && index < (int)terminalPtr->getRecvYxSum())
				{
					unsigned char lowBit = 0;
					unsigned char hightBit = 0;

					if ((yx_val & BYTE_CHECK_TRUE[j*2]) > 0)
					{
						lowBit = 0x01;
					}
					else
					{
						lowBit = 0;
					}

					if ((yx_val & BYTE_CHECK_TRUE[j*2 + 1]) > 0)
					{
						hightBit = 0x02;
					}
					else
					{
						hightBit = 0;
					}

					terminalPtr->SaveYxType(index,DataBase::double_yx_point);
					int zyqyx_val = (lowBit | hightBit) & 0x03;
					//std::ostringstream ostr;//zyq
					ostr.str("");
					ostr<<"ң�ŵ��index="<<index<<"��yxֵ="<<zyqyx_val<<std::endl;
					AddStatusLogWithSynT(ostr.str());

					int ret = terminalPtr->SaveOriYxVal(index,(lowBit | hightBit) & 0x03);
					if (ret = DataBase::CauseActiveData)
					{
						count++;
					}
				}

				terminalPtr->SaveYxQuality(index,yxQuality);
			}

/*			yx_val = buf[DataLocation_ + 1 + i*InfoDataLength];
			for (int j=0;j<8;j++)
			{
				int index = info_addr + 4 + j;
				if((index >= 0) && (index < (int)terminalPtr->getYxSum()))
				{
					unsigned char lowBit = 0;
					unsigned char hightBit = 0;

					if ((yx_val & BYTE_CHECK_TRUE[j*2]) > 0)
					{
						lowBit = 0x01;
					}
					else
					{
						lowBit = 0;
					}

					if ((yx_val & BYTE_CHECK_TRUE[j*2 + 1]) > 0)
					{
						hightBit = 0x02;
					}
					else
					{
						hightBit = 0;
					}

					terminalPtr->SaveYxType(index,DataBase::double_yx_point);
					int ret = terminalPtr->SaveOriYxVal(index,(lowBit | hightBit) & 0x03);
					if (ret = DataBase::CauseActiveData)
					{
						count++;
					}
				}

				terminalPtr->SaveYxQuality(index,yxQuality);
			}*/
		}
	}

	if (count > 0)
	{
		std::ostringstream ostr;
		ostr.str("");
		ostr<<"�յ�����˫�㱣����ϢYX�ֱ��Ĳ�����COS����Ŀ��"<<count<<std::endl;
		AddStatusLogWithSynT(ostr.str());
		CmdConSig_(COS_DATA_UP,RETURN_CODE_ACTIVE,terminalPtr,count);
		//AddStatusLogWithSynT("�������COS������վ��\n");

	}

	return 0;
}
int CH103::ParseDoubleProtectYxWithSOE(unsigned char * buf, share_terminal_ptr terminalPtr)
{
	int info_num = BufToVal(&buf[InfoNumLocation_],InfoNumLength_) & (~(0x80<<(InfoNumLength_ - 1) * 8));
	int info_ISQ = (BufToVal(&buf[InfoNumLocation_],InfoNumLength_) >> ((InfoNumLength_ - 1) * 8)) & 0x80;

	if (info_num <= 0)
	{
		AddStatusLogWithSynT("�յ�������Ϣ˫��SOE���ģ�������Ϣ����Ŀ���󣬲�����ñ��ġ�\n" );
		return -1;
	}

	/*
	if (info_ISQ == DisableISQ)
	{
		AddStatusLogWithSynT( "�յ�����SOE���ģ�������Ϣ��ISQλ���󣬲�����ñ��ġ�\n" );
		return -1;
	}
	*/

	using namespace boost::posix_time;
	ptime lt = boost::posix_time::microsec_clock::local_time();

	if (info_ISQ == DisableISQ)
	{
	    unsigned char frame_length = buf[1] + 4;
		unsigned char InfoDataLength = (frame_length - DataLocation_) / info_num;
		if ( InfoDataLength >= 8)
		{
			InfoDataLength = 8;
		}
		else if (InfoDataLength < 8 && InfoDataLength >= 5)
		{
			InfoDataLength = 5;
		}

//		int info_addr = BufToVal(&buf[InfoAddrLocation_],InfoAddrLength_) - DYX_START_ADDR_;

		for (int i=0;i<info_num;i++)
		{
			int info_addr = BufToVal(&buf[InfoAddrLocation_ + i*InfoDataLength],InfoAddrLength_) + DYX_START_ADDR_ - SYX_START_ADDR_ -1;

			if (info_addr + i>= 0 && info_addr + i< (int)terminalPtr->getRecvYxSum())
			{
				unsigned char YxQuality = buf[DataLocation_ + i*InfoDataLength] & 0xf0; 
				unsigned char YxBitVal = buf[DataLocation_ + i*InfoDataLength] & 0x03;
				unsigned short millisecond = BufToVal(&buf[DataLocation_ + 1 + i*InfoDataLength],2);
				unsigned char minute = buf[DataLocation_ + 3 + i*InfoDataLength] & 0x3f;
				unsigned char Hour = buf[DataLocation_ + 4 + i*InfoDataLength] & 0x7f;
				time_duration td(Hour,minute,millisecond / 1000,millisecond % 1000);

				std::ostringstream ostr;
				
				if (InfoDataLength >= 8)
				{
					unsigned char Day = buf[DataLocation_ + 5 + i*InfoDataLength] & 0x1f;
					unsigned char Month = buf[DataLocation_ + 6 + i*InfoDataLength] & 0x0f;
					unsigned short Year = (buf[DataLocation_ + 7 + i*InfoDataLength] & 0x7f) + (((lt.date()).year() / 100) * 100);
					boost::gregorian::date dt(Year,Month,Day);
					ptime timeVal(dt,td);
					terminalPtr->SaveYxQuality(info_addr,YxQuality);
					terminalPtr->SaveYxType(info_addr,DataBase::double_yx_point);
					terminalPtr->SaveSoePoint(info_addr,YxBitVal,DataBase::double_yx_point,timeVal,YxQuality);
					terminalPtr->SaveCosPoint(info_addr,YxBitVal,DataBase::double_yx_point,YxQuality);

					ostr.str("");
					ostr<<"�յ�˫�㱣����λ�����"<<info_addr<<"��YXֵ"<<std::hex<<std::uppercase<<std::setw(2)<<std::setfill('0')<<(short)YxBitVal<<"��"<<to_simple_string(timeVal)<<std::endl;
				}
				else
				{
					ptime timeVal(lt.date(),td);

					terminalPtr->SaveYxQuality(info_addr,YxQuality);
					terminalPtr->SaveYxType(info_addr,DataBase::double_yx_point);
					terminalPtr->SaveSoePoint(info_addr,YxBitVal,DataBase::double_yx_point,timeVal,YxQuality);
					terminalPtr->SaveCosPoint(info_addr,YxBitVal,DataBase::double_yx_point,YxQuality);

					ostr.str("");
					ostr<<"�յ�˫�㱣����λ�����"<<info_addr<<"��YXֵ"<<std::hex<<std::uppercase<<std::setw(2)<<std::setfill('0')<<(short)YxBitVal<<"��"<<to_simple_string(timeVal)<<std::endl;
				}

				AddStatusLogWithSynT(ostr.str());
			}
		}
	} 
	else if(info_ISQ == EnableISQ)
	{
		unsigned char frame_length = buf[1] + 4;
		unsigned char InfoDataLength = (frame_length - InfoAddrLocation_) / info_num;
		if ( InfoDataLength >= InfoAddrLength_ + 8)
		{
			InfoDataLength = InfoAddrLength_ + 8;
		}
		else if (InfoDataLength < InfoAddrLength_ + 8 && InfoDataLength >= InfoAddrLength_ + 5)
		{
			InfoDataLength = InfoAddrLength_ + 5;
		}

		for (int i=0;i<info_num;i++)
		{
			int info_addr = BufToVal(&buf[InfoAddrLocation_ + i*InfoDataLength],InfoAddrLength_) + DYX_START_ADDR_ - SYX_START_ADDR_ -1;
			if (info_addr >= 0 && info_addr < (int)terminalPtr->getRecvYxSum())
			{
				unsigned char YxQuality = buf[DataLocation_ + i*InfoDataLength] & 0xf0; 
				unsigned char YxBitVal = buf[DataLocation_ + i*InfoDataLength] & 0x03;
				unsigned short millisecond = BufToVal(&buf[DataLocation_ + 1 + i*InfoDataLength],2);
				unsigned char minute = buf[DataLocation_ + 3 + i*InfoDataLength] & 0x3f;
				//unsigned char Hour = buf[DataLocation_ + 4 + i*InfoDataLength] & 0x1f;
				unsigned char Hour = buf[DataLocation_ + 4 + i*InfoDataLength] & 0x7f;
				time_duration td(Hour,minute,millisecond / 1000,millisecond % 1000);

				std::ostringstream ostr;

				if (InfoDataLength >= InfoAddrLength_ + 8)
				{
					unsigned char Day = buf[DataLocation_ + 5 + i*InfoDataLength] & 0x1f;
					unsigned char Month = buf[DataLocation_ + 6 + i*InfoDataLength] & 0x0f;
					unsigned short Year = (buf[DataLocation_ + 7 + i*InfoDataLength] & 0x7f) + (((lt.date()).year() / 100) * 100);
					boost::gregorian::date dt(Year,Month,Day);
					ptime timeVal(dt,td);

					terminalPtr->SaveYxQuality(info_addr,YxQuality);
					terminalPtr->SaveYxType(info_addr,DataBase::double_yx_point);
					terminalPtr->SaveSoePoint(info_addr,YxBitVal,DataBase::double_yx_point,timeVal,YxQuality);
					terminalPtr->SaveCosPoint(info_addr,YxBitVal,DataBase::double_yx_point,YxQuality);

					ostr.str("");
					ostr<<"�յ�˫�㱣����λ�����"<<info_addr<<"��YXֵ"<<std::hex<<std::uppercase<<std::setw(2)<<std::setfill('0')<<(short)YxBitVal<<"��"<<to_simple_string(timeVal)<<std::endl;
				}
				else
				{
					ptime timeVal(lt.date(),td);

					terminalPtr->SaveYxQuality(info_addr,YxQuality);
					terminalPtr->SaveYxType(info_addr,DataBase::double_yx_point);
					terminalPtr->SaveSoePoint(info_addr,YxBitVal,DataBase::double_yx_point,timeVal,YxQuality);
					terminalPtr->SaveCosPoint(info_addr,YxBitVal,DataBase::double_yx_point,YxQuality);

					ostr.str("");
					ostr<<"�յ�˫�㱣����λ�����"<<info_addr<<"��YXֵ"<<std::hex<<std::uppercase<<std::setw(2)<<std::setfill('0')<<(short)YxBitVal<<"��"<<to_simple_string(timeVal)<<std::endl;
				}

				AddStatusLogWithSynT(ostr.str());
			} 
			else
			{
				AddStatusLogWithSynT("�յ�������Ϣ˫��SOE��������Ϣ���ַ���󣬲�����õ㡣\n");
			}
		}
		unsigned char SIN = buf[DataLocation_ + 5 + (info_num - 1)*InfoDataLength];
		//���ٻ�ʱ���ϴη��͵�SIN�Ա�

	}

	if (info_num > 0)
	{
		CmdConSig_(SOE_DATA_UP,RETURN_CODE_ACTIVE,terminalPtr,info_num);
		CmdConSig_(COS_DATA_UP,RETURN_CODE_ACTIVE,terminalPtr,info_num);
		AddStatusLogWithSynT("�յ�˫�㱣��COS��SOE�����COS��SOE��������վ��\n");

	}


	return 0;
}
int CH103::ParseDoubleYxWithTE(unsigned char * buf, share_terminal_ptr terminalPtr)
{
	int info_num = BufToVal(&buf[InfoNumLocation_],InfoNumLength_) & (~(0x80<<(InfoNumLength_ - 1) * 8));
	int info_ISQ = (BufToVal(&buf[InfoNumLocation_],InfoNumLength_) >> ((InfoNumLength_ - 1) * 8)) & 0x80;

	if (info_num <= 0)
	{
		AddStatusLogWithSynT("�յ�˫��SOE���ģ�������Ϣ����Ŀ���󣬲�����ñ��ġ�\n" );
		return -1;
	}

	/*
	if (info_ISQ == DisableISQ)
	{
		AddStatusLogWithSynT( "�յ�����SOE���ģ�������Ϣ��ISQλ���󣬲�����ñ��ġ�\n" );
		return -1;
	}
	*/

	using namespace boost::posix_time;
	ptime lt = boost::posix_time::microsec_clock::local_time();

	if (info_ISQ == DisableISQ)
	{
		unsigned char frame_length = buf[1] + 4;
		unsigned char InfoDataLength = (frame_length - DataLocation_) / info_num;
		if ( InfoDataLength >= 8)
		{
			InfoDataLength = 8;
		}
		else if (InfoDataLength < 8 && InfoDataLength >= 5)
		{
			InfoDataLength = 5;
		}

//		int info_addr = BufToVal(&buf[InfoAddrLocation_],InfoAddrLength_) - DYX_START_ADDR_;
		int info_addr = BufToVal(&buf[InfoAddrLocation_],InfoAddrLength_) + DYX_START_ADDR_ - SYX_START_ADDR_ -1;

		for (int i=0;i<info_num;i++)
		{
			if (info_addr + i>= 0 && info_addr + i< (int)terminalPtr->getRecvYxSum())
			{
				unsigned char YxQuality = buf[DataLocation_ + i*InfoDataLength] & 0xf0; 
				unsigned char YxBitVal = buf[DataLocation_ + i*InfoDataLength] & 0x03;
				unsigned short millisecond = BufToVal(&buf[DataLocation_ + 1 + i*InfoDataLength],2);
				unsigned char minute = buf[DataLocation_ + 3 + i*InfoDataLength] & 0x3f;
				unsigned char Hour = buf[DataLocation_ + 4 + i*InfoDataLength] & 0x7f;
				time_duration td(Hour,minute,millisecond / 1000,millisecond % 1000);

				std::ostringstream ostr;
				
				if (InfoDataLength >= 8)
				{
					unsigned char Day = buf[DataLocation_ + 5 + i*InfoDataLength] & 0x1f;
					unsigned char Month = buf[DataLocation_ + 6 + i*InfoDataLength] & 0x0f;
					unsigned short Year = (buf[DataLocation_ + 7 + i*InfoDataLength] & 0x7f) + (((lt.date()).year() / 100) * 100);
					boost::gregorian::date dt(Year,Month,Day);
					ptime timeVal(dt,td);

					terminalPtr->SaveYxQuality(info_addr+i,YxQuality);
					terminalPtr->SaveYxType(info_addr+i,DataBase::double_yx_point);
					terminalPtr->SaveSoePoint(info_addr+i,YxBitVal,DataBase::double_yx_point,timeVal,YxQuality);

					ostr.str("");
					ostr<<"�յ�˫��SOE�����"<<info_addr+i<<"��YXֵ"<<std::hex<<std::uppercase<<std::setw(2)<<std::setfill('0')<<(short)YxBitVal<<"��"<<to_simple_string(timeVal)<<std::endl;
				}
				else
				{
					ptime timeVal(lt.date(),td);

					terminalPtr->SaveYxQuality(info_addr+i,YxQuality);
					terminalPtr->SaveYxType(info_addr+i,DataBase::double_yx_point);
					terminalPtr->SaveSoePoint(info_addr+i,YxBitVal,DataBase::double_yx_point,timeVal,YxQuality);

					ostr.str("");
					ostr<<"�յ�˫��SOE�����"<<info_addr+i<<"��YXֵ"<<std::hex<<std::uppercase<<std::setw(2)<<std::setfill('0')<<(short)YxBitVal<<"��"<<to_simple_string(timeVal)<<std::endl;
				}

				AddStatusLogWithSynT(ostr.str());
			}
		}
	} 
	else if(info_ISQ == EnableISQ)
	{
		unsigned char frame_length = buf[1] + 4;
		unsigned char InfoDataLength = (frame_length - InfoAddrLocation_) / info_num;
		if ( InfoDataLength >= InfoAddrLength_ + 8)
		{
			InfoDataLength = InfoAddrLength_ + 8;
		}
		else if (InfoDataLength < InfoAddrLength_ + 8 && InfoDataLength >= InfoAddrLength_ + 5)
		{
			InfoDataLength = InfoAddrLength_ + 5;
		}

		for (int i=0;i<info_num;i++)
		{
			//int info_addr = BufToVal(&buf[InfoAddrLocation_],InfoAddrLength_) + DYX_START_ADDR_ - SYX_START_ADDR_ -1;

			int info_addr = BufToVal(&buf[InfoAddrLocation_ + i*InfoDataLength],InfoAddrLength_)  + DYX_START_ADDR_ - SYX_START_ADDR_ -1;
			if (info_addr >= 0 && info_addr < (int)terminalPtr->getRecvYxSum())
			{
				unsigned char YxQuality = buf[DataLocation_ + i*InfoDataLength] & 0xf0; 
				unsigned char YxBitVal = buf[DataLocation_ + i*InfoDataLength] & 0x03;
				unsigned short millisecond = BufToVal(&buf[DataLocation_ + 1 + i*InfoDataLength],2);
				unsigned char minute = buf[DataLocation_ + 3 + i*InfoDataLength] & 0x3f;
				//unsigned char Hour = buf[DataLocation_ + 4 + i*InfoDataLength] & 0x1f;
				unsigned char Hour = buf[DataLocation_ + 4 + i*InfoDataLength] & 0x7f;
				time_duration td(Hour,minute,millisecond / 1000,millisecond % 1000);

				std::ostringstream ostr;

				if (InfoDataLength >= InfoAddrLength_ + 8)
				{
					unsigned char Day = buf[DataLocation_ + 5 + i*InfoDataLength] & 0x1f;
					unsigned char Month = buf[DataLocation_ + 6 + i*InfoDataLength] & 0x0f;
					unsigned short Year = (buf[DataLocation_ + 7 + i*InfoDataLength] & 0x7f) + (((lt.date()).year() / 100) * 100);
					boost::gregorian::date dt(Year,Month,Day);
					ptime timeVal(dt,td);

					terminalPtr->SaveYxQuality(info_addr,YxQuality);
					terminalPtr->SaveYxType(info_addr,DataBase::double_yx_point);
					terminalPtr->SaveSoePoint(info_addr,YxBitVal,DataBase::double_yx_point,timeVal,YxQuality);

					ostr.str("");
					ostr<<"�յ�˫��SOE�����"<<info_addr<<"��YXֵ"<<std::hex<<std::uppercase<<std::setw(2)<<std::setfill('0')<<(short)YxBitVal<<"��"<<to_simple_string(timeVal)<<std::endl;
				}
				else
				{
					ptime timeVal(lt.date(),td);

					terminalPtr->SaveYxQuality(info_addr,YxQuality);
					terminalPtr->SaveYxType(info_addr,DataBase::double_yx_point);
					terminalPtr->SaveSoePoint(info_addr,YxBitVal,DataBase::double_yx_point,timeVal,YxQuality);

					ostr.str("");
					ostr<<"�յ�˫��SOE�����"<<info_addr<<"��YXֵ"<<std::hex<<std::uppercase<<std::setw(2)<<std::setfill('0')<<(short)YxBitVal<<"��"<<to_simple_string(timeVal)<<std::endl;
				}

				AddStatusLogWithSynT(ostr.str());
			} 
			else
			{
				AddStatusLogWithSynT("�յ�˫��SOE��������Ϣ���ַ���󣬲�����õ㡣\n");
			}
		}
		unsigned char SIN = buf[DataLocation_ + 5 + (info_num - 1)*InfoDataLength];
		//���ٻ�ʱ���ϴη��͵�SIN�Ա�

	}

	if (info_num > 0)
	{
		CmdConSig_(SOE_DATA_UP,RETURN_CODE_ACTIVE,terminalPtr,info_num);
	}

	return 0;
}

int CH103::ParseDoubleProtectYxWithTE(unsigned char * buf, share_terminal_ptr terminalPtr)
{
	int info_num = BufToVal(&buf[InfoNumLocation_],InfoNumLength_) & (~(0x80<<(InfoNumLength_ - 1) * 8));
	int info_ISQ = (BufToVal(&buf[InfoNumLocation_],InfoNumLength_) >> ((InfoNumLength_ - 1) * 8)) & 0x80;

	if (info_num <= 0)
	{
		AddStatusLogWithSynT("�յ�˫�㱣����λ���ģ�������Ϣ����Ŀ���󣬲�����ñ��ġ�\n" );
		return -1;
	}

	/*
	if (info_ISQ == DisableISQ)
	{
		AddStatusLogWithSynT( "�յ�����SOE���ģ�������Ϣ��ISQλ���󣬲�����ñ��ġ�\n" );
		return -1;
	}
	*/

	using namespace boost::posix_time;
	ptime lt = boost::posix_time::microsec_clock::local_time();

	if (info_ISQ == DisableISQ)
	{
		unsigned char frame_length = buf[1] + 4;
		unsigned char InfoDataLength = (frame_length - DataLocation_) / info_num;
		if ( InfoDataLength >= 12)
		{
			InfoDataLength = 12;
		}
		else if (InfoDataLength < 12 && InfoDataLength >= 9)
		{
			InfoDataLength = 9;
		}

//		int info_addr = BufToVal(&buf[InfoAddrLocation_],InfoAddrLength_) - DYX_START_ADDR_;
		int info_addr = BufToVal(&buf[InfoAddrLocation_],InfoAddrLength_) + DYX_START_ADDR_ - SYX_START_ADDR_ -1;

		for (int i=0;i<info_num;i++)
		{

			if (info_addr + i>= 0 && info_addr + i< (int)terminalPtr->getRecvYxSum())
			{
				unsigned char YxQuality = buf[DataLocation_ + i*InfoDataLength] & 0xf0; 
				unsigned char YxBitVal = buf[DataLocation_ + i*InfoDataLength] & 0x03;
				unsigned short millisecond = BufToVal(&buf[DataLocation_ + 5 + i*InfoDataLength],2);
				unsigned char minute = buf[DataLocation_ + 7 + i*InfoDataLength] & 0x3f;
				unsigned char Hour = buf[DataLocation_ + 8 + i*InfoDataLength] & 0x7f;
				time_duration td(Hour,minute,millisecond / 1000,millisecond % 1000);
				AddStatusLogWithSynT("�ѽ��뱣��������λ����;\n");//zyq

				std::ostringstream ostr;

				if (InfoDataLength >= 12)
				{
					unsigned char Day = buf[DataLocation_ + 9 + i*InfoDataLength] & 0x1f;
					unsigned char Month = buf[DataLocation_ + 10 + i*InfoDataLength] & 0x0f;
					unsigned short Year = (buf[DataLocation_ + 11 + i*InfoDataLength] & 0x7f) + (((lt.date()).year() / 100) * 100);
					boost::gregorian::date dt(Year,Month,Day);
					ptime timeVal(dt,td);

					terminalPtr->SaveYxQuality(info_addr+i,YxQuality);
					terminalPtr->SaveYxType(info_addr+i,DataBase::double_yx_point);
					terminalPtr->SaveSoePoint(info_addr+i,YxBitVal,DataBase::double_yx_point,timeVal,YxQuality);
					terminalPtr->SaveCosPoint(info_addr+i,YxBitVal,DataBase::double_yx_point,YxQuality);

					ostr.str("");
					ostr<<"�յ�˫�㱣����λ�����"<<info_addr+i<<"��YXֵ"<<std::hex<<std::uppercase<<std::setw(2)<<std::setfill('0')<<(short)YxBitVal<<"��"<<to_simple_string(timeVal)<<std::endl;
				}
				else
				{
					ptime timeVal(lt.date(),td);

					terminalPtr->SaveYxQuality(info_addr+i,YxQuality);
					terminalPtr->SaveYxType(info_addr+i,DataBase::double_yx_point);
					terminalPtr->SaveSoePoint(info_addr+i,YxBitVal,DataBase::double_yx_point,timeVal,YxQuality);
					terminalPtr->SaveCosPoint(info_addr+i,YxBitVal,DataBase::double_yx_point,YxQuality);

					ostr.str("");
					ostr<<"�յ�˫�㱣��������λ�����"<<info_addr+i<<"��YXֵ"<<std::hex<<std::uppercase<<std::setw(2)<<std::setfill('0')<<(short)YxBitVal<<"��"<<to_simple_string(timeVal)<<std::endl;
				}

				AddStatusLogWithSynT(ostr.str());
			}
		}
	} 
	else if(info_ISQ == EnableISQ)
	{
		unsigned char frame_length = buf[1] + 4;
		unsigned char InfoDataLength = (frame_length - InfoAddrLocation_) / info_num;
		if ( InfoDataLength >= InfoAddrLength_ + 12)
		{
			InfoDataLength = InfoAddrLength_ + 12;
		}
		else if (InfoDataLength < InfoAddrLength_ + 12 && InfoDataLength >= InfoAddrLength_ + 9)
		{
			InfoDataLength = InfoAddrLength_ + 9;
		}

		for (int i=0;i<info_num;i++)
		{
			int info_addr = BufToVal(&buf[InfoAddrLocation_ + i*InfoDataLength],InfoAddrLength_) + DYX_START_ADDR_ - SYX_START_ADDR_ -1;
//			int info_addr = BufToVal(&buf[InfoAddrLocation_ + i*InfoDataLength],InfoAddrLength_) - DYX_START_ADDR_;
			if (info_addr >= 0 && info_addr < (int)terminalPtr->getRecvYxSum())
			{
				unsigned char YxQuality = buf[DataLocation_ + i*InfoDataLength] & 0xf0; 
				unsigned char YxBitVal = buf[DataLocation_ + i*InfoDataLength] & 0x03;
				unsigned short millisecond = BufToVal(&buf[DataLocation_ + 5 + i*InfoDataLength],2);
				unsigned char minute = buf[DataLocation_ + 7 + i*InfoDataLength] & 0x3f;
				unsigned char Hour = buf[DataLocation_ + 8 + i*InfoDataLength] & 0x7f;
				time_duration td(Hour,minute,millisecond / 1000,millisecond % 1000);
				AddStatusLogWithSynT("�ѽ��뱣����λ����;\n");//zyq

				std::ostringstream ostr;

				if (InfoDataLength >= InfoAddrLength_ + 12)
				{
					unsigned char Day = buf[DataLocation_ + 9 + i*InfoDataLength] & 0x1f;
					unsigned char Month = buf[DataLocation_ + 10 + i*InfoDataLength] & 0x0f;
					unsigned short Year = (buf[DataLocation_ + 11 + i*InfoDataLength] & 0x7f) + (((lt.date()).year() / 100) * 100);
					boost::gregorian::date dt(Year,Month,Day);
					ptime timeVal(dt,td);

					terminalPtr->SaveYxQuality(info_addr,YxQuality);
					terminalPtr->SaveYxType(info_addr,DataBase::double_yx_point);
					terminalPtr->SaveSoePoint(info_addr,YxBitVal,DataBase::double_yx_point,timeVal,YxQuality);
					terminalPtr->SaveCosPoint(info_addr,YxBitVal,DataBase::double_yx_point,YxQuality);
					ostr.str("");
					ostr<<"�յ�˫�㱣����λ�����"<<info_addr<<"��YXֵ"<<std::hex<<std::uppercase<<std::setw(2)<<std::setfill('0')<<(short)YxBitVal<<"��"<<to_simple_string(timeVal)<<std::endl;
				}
				else
				{
					ptime timeVal(lt.date(),td);

					terminalPtr->SaveYxQuality(info_addr,YxQuality);
					terminalPtr->SaveYxType(info_addr,DataBase::double_yx_point);
					terminalPtr->SaveSoePoint(info_addr,YxBitVal,DataBase::double_yx_point,timeVal,YxQuality);
					terminalPtr->SaveCosPoint(info_addr,YxBitVal,DataBase::double_yx_point,YxQuality);
					ostr.str("");
					ostr<<"�յ�˫�㱣����λ�����"<<info_addr<<"��YXֵ"<<std::hex<<std::uppercase<<std::setw(2)<<std::setfill('0')<<(short)YxBitVal<<"��"<<to_simple_string(timeVal)<<std::endl;
				}

				AddStatusLogWithSynT(ostr.str());
			} 
			else
			{
				AddStatusLogWithSynT("�յ�˫�㱣����λ��������Ϣ���ַ���󣬲�����õ㡣\n");
			}
		}
	}

	if (info_num > 0)
	{
		CmdConSig_(SOE_DATA_UP,RETURN_CODE_ACTIVE,terminalPtr,info_num);
		CmdConSig_(COS_DATA_UP,RETURN_CODE_ACTIVE,terminalPtr,info_num);
		AddStatusLogWithSynT("�յ�˫�㱣��COS��SOE�����COS��SOE��������վ��\n");
		
	}

	return 0;
}

//ͨ�÷������ݱ���
int CH103::ParseAssortData(unsigned char * buf,share_terminal_ptr terminalPtr)
{
	int trans_reason = BufToVal(&buf[TransReasonLocation_],TransReasonLength_);
	int funType = BufToVal(&buf[FunTypeLocation_],FunTypeLength_);
	int info_addr = BufToVal(&buf[InfoAddrLocation_],InfoAddrLength_);
	int index = DataLocation_;
	int RII = BufToVal(&buf[index++],1);
	int NGD = BufToVal(&buf[index++],1);

	if (NGD <= 0)
	{
		return -1;
	}

	boost::scoped_array<unsigned char> GIN_groupNO(new unsigned char[NGD]);
	boost::scoped_array<unsigned char> GIN_itemNO(new unsigned char[NGD]);
	boost::scoped_array<unsigned char> KOD(new unsigned char[NGD]);
	boost::scoped_array<unsigned char> GDD_type(new unsigned char[NGD]);
	boost::scoped_array<unsigned char> GDD_length(new unsigned char[NGD]);
	boost::scoped_array<unsigned char> GDD_num(new unsigned char[NGD]);
	
	typedef unsigned int GIDdataType;
		
	for (int i=0;i<NGD;i++)
	{
		GIN_groupNO[i] = BufToVal(&buf[index++],1);
		GIN_itemNO[i] = BufToVal(&buf[index++],1);
		KOD[i] = BufToVal(&buf[index++],1);
		GDD_type[i] = BufToVal(&buf[index++],1);
		GDD_length[i] = BufToVal(&buf[index++],1);
		GDD_num[i] = BufToVal(&buf[index++],1);

		if (GDD_length[i] > 0 && GDD_num[i] > 0)
		{
			
		}
		
	}
	

	return 0;
}

};//namespace Protocol
