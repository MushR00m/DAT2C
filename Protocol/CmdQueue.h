#ifndef CmdQueue_H
#define CmdQueue_H

#include <vector>
#include <boost/any.hpp>
#include <boost/thread/mutex.hpp>
#include "../PublicSupport/Dat2cTypeDef.h"

namespace Protocol {

//cmd type def
const short FRAME_USR = 10;
const short START_ACT            = FRAME_USR + 1;
const short START_CONFIRM        = FRAME_USR + 2;
const short STOP_ACT             = FRAME_USR + 3;
const short STOP_CONFIRM         = FRAME_USR + 4;
const short TEST_ACT             = FRAME_USR + 5;
const short TEST_CONFIRM         = FRAME_USR + 6;
const short S_GRAM_FRAME         = FRAME_USR + 7;	
const short CALL_ALL_DATA_ACT    = FRAME_USR + 8;
const short CALL_ALL_DATA_CON    = FRAME_USR + 9;
const short CALL_ALL_DATA_OVER   = FRAME_USR + 10;
const short CALL_ALL_DD_ACT      = FRAME_USR + 11;
const short CALL_ALL_YM_CON      = FRAME_USR + 12;
const short CALL_ALL_YM_OVER     = FRAME_USR + 13;
const short SYN_TIME_ACT         = FRAME_USR + 14;
const short SYN_TIME_CON         = FRAME_USR + 15;
const short YK_SEL_ACT           = FRAME_USR + 16;
const short YK_SEL_CON           = FRAME_USR + 17;
const short YK_EXE_ACT           = FRAME_USR + 18;
const short YK_EXE_CON           = FRAME_USR + 19;
const short YK_CANCEL_ACT        = FRAME_USR + 20;
const short YK_CANCEL_CON        = FRAME_USR + 21;
const short REQUIRE_LINK         = FRAME_USR + 22;
const short RESET_LINK           = FRAME_USR + 23;
const short CONFIRM_ACK          = FRAME_USR + 24;
const short CONFIRM_NACK         = FRAME_USR + 25;
const short CALL_PRIMARY_DATA    = FRAME_USR + 26;
const short CALL_SECONDARY_DATA  = FRAME_USR + 27;
const short ALL_YX_DATA          = FRAME_USR + 28;
const short ALL_YC_DATA          = FRAME_USR + 29;
const short ALL_YM_DATA          = FRAME_USR + 30;
const short COS_DATA_UP          = FRAME_USR + 31;
const short SOE_DATA_UP          = FRAME_USR + 32;
const short YCVAR_DATA_UP        = FRAME_USR + 33;
const short YK_OVER_CON          = FRAME_USR + 34;
const short LINK_STATUS          = FRAME_USR + 35;
const short END_INIT             = FRAME_USR + 36;
const short E5                   = FRAME_USR + 37;
const short QUERY_DATA           = FRAME_USR + 38;
const short RESET_CMD            = FRAME_USR + 39;
const short TEST_CMD             = FRAME_USR + 40;
const short SINGLE_YX_DATA       = FRAME_USR + 41;
const short SINGLE_YC_DATA       = FRAME_USR + 42;
const short SINGLE_YM_DATA       = FRAME_USR + 43;
const short TRANSMIT_FRAME       = FRAME_USR + 44;
const short YC_SEND_BYTIME       = FRAME_USR + 45;//��ʱ����ң��
const short TRANS_DELAY_ACT      = FRAME_USR + 46;
const short TRANS_DELAY_DOWNLOAD = FRAME_USR + 47;

const short SEL_DEF_FRAME = FRAME_USR + 47;
const short CALL_EXTEND_CPUINFO  = SEL_DEF_FRAME + 1;
const short CALL_EXTEND_PARA     = SEL_DEF_FRAME + 2;
const short SET_EXTEND_PARA      = SEL_DEF_FRAME + 3;
const short CALL_HISTORY_DATA    = SEL_DEF_FRAME + 4;
const short CALL_HISTORY_DATAS   = SEL_DEF_FRAME + 5;
const short CALL_FAULT_RECORD    = SEL_DEF_FRAME + 6;
const short CALL_YK_RECORD       = SEL_DEF_FRAME + 7;
const short CALL_YX_RECORD       = SEL_DEF_FRAME + 8;
const short EXTEND_CPUINFO_DATA  = SEL_DEF_FRAME + 9;
const short EXTEND_PARA_DATA     = SEL_DEF_FRAME + 10;
const short EXTEND_PARA_CON      = SEL_DEF_FRAME + 11;
const short HISTORY_DATA_UP      = SEL_DEF_FRAME + 12;
const short FAULT_RECORD_DATA    = SEL_DEF_FRAME + 13;
const short FAULT_RECORD_CON     = SEL_DEF_FRAME + 14;
const short YK_RECORD_DATA       = SEL_DEF_FRAME + 15;
const short YK_RECORD_CON        = SEL_DEF_FRAME + 16;
const short YX_RECORD_DATA       = SEL_DEF_FRAME + 17;
const short YX_RECORD_CON        = SEL_DEF_FRAME + 18;
const short CHECK_PUB_KEY        = SEL_DEF_FRAME + 19;
const short UPDATE_PUB_KEY       = SEL_DEF_FRAME + 20;

//����for�ɶ�
const short SEL_DEF_PROTECT = SEL_DEF_FRAME + 20;
const short CALL_JB_PARA_CON                    = SEL_DEF_PROTECT + 1;//
const short SEND_JB_PARA_CON                    = SEL_DEF_PROTECT + 2;//
const short ACT_JB_PARA_CON                     = SEL_DEF_PROTECT + 3;//
const short DEACT_JB_PARA_CON                   = SEL_DEF_PROTECT + 4;//
const short JB_SIGNAL_RESET_CON                 = SEL_DEF_PROTECT + 5;//
const short JB_SOE_CON                          = SEL_DEF_PROTECT + 6;//
const short CALL_JB_PARA_ACT                    = SEL_DEF_PROTECT + 7;//
const short SEND_JB_PARA_ACT                    = SEL_DEF_PROTECT + 8;//
const short ACT_JB_PARA_ACT                     = SEL_DEF_PROTECT + 9;//
const short DEACT_JB_PARA_ACT                   = SEL_DEF_PROTECT + 10;//
const short JB_SIGNAL_RESET_ACT                 = SEL_DEF_PROTECT + 11;//

//BF533�Զ����Լ��ʼ ZHANGZHIHUA
const short SEL_BF533_FRAME = SEL_DEF_PROTECT + 11;
const short CALL_EQU_PARA_ACT                   = SEL_BF533_FRAME +1;//�ٻ�װ�ò���
const short CALL_EQU_PARA_CON                   = SEL_BF533_FRAME +2;//�ٻ�װ�ò���Ӧ����
const short DOWNLOAD_EQU_PARA                   = SEL_BF533_FRAME +3;//��װװ�ò���
const short DOWNLOAD_EQU_PARA_SUCESS            = SEL_BF533_FRAME +4;//��װװ�ò����ɹ�
const short DOWNLOAD_EQU_PARA_SUCESS_CON        = SEL_BF533_FRAME +5;//��װװ�ò����ɹ�
const short DOWNLOAD_DELAY_CONRL                = SEL_BF533_FRAME +6;//��װ����������
const short DOWNLOAD_DELAY_CONTRL_CHECK         = SEL_BF533_FRAME +7;//��װ���������ַ�У
const short DOWNLOAD_DELAY_CONTRL_SUCESS        = SEL_BF533_FRAME +8;//������������װ�ɹ�
const short DOWNLOAD_DELAY_CONTRL_SUCESS_CON    = SEL_BF533_FRAME +9;//������������װ�ɹ�
const short SIGNAL_RESET                        = SEL_BF533_FRAME +10;//�źŸ���
const short SIGNAL_RESET_CON                    = SEL_BF533_FRAME +11;//�źŸ���ȷ��
const short DOWNLOAD_CHANNEL_PARA               = SEL_BF533_FRAME +12;//��װͨ������
const short DOWNLOAD_CHANNEL_PARA_CHECK         = SEL_BF533_FRAME +13;//��װͨ��������У
const short DOWNLOAD_CHANNEL_PARA_SUCESS        = SEL_BF533_FRAME +14;//��װͨ�������ɹ�
const short DOWNLOAD_CHANNEL_PARA_SUCESS_CON    = SEL_BF533_FRAME +15;//��װͨ�������ɹ�
const short DOWNLOAD_CHANNEL_COMREL             = SEL_BF533_FRAME +16;//��װͨ����Ϲ�ϵ
const short DOWNLOAD_CHANNEL_COMREL_CHECK       = SEL_BF533_FRAME +17;//��װͨ����Ϲ�ϵ��У
const short DOWNLOAD_CHANNEL_COMREL_SUCESS      = SEL_BF533_FRAME +18;//��װͨ����Ϲ�ϵ�ɹ�
const short DOWNLOAD_CHANNEL_COMREL_SUCESS_CON  = SEL_BF533_FRAME +19;//��װͨ����Ϲ�ϵ�ɹ�
const short CALL_LINE_DATA_ACT                  = SEL_BF533_FRAME +20;//�ٻ���·����
const short LINE_CYC_TIME_CON                   = SEL_BF533_FRAME +21;//��·ȫң�ⶨʱ����
const short DOWNLOAD_LINE_VAL                   = SEL_BF533_FRAME +22;//��·��ֵ��װ
const short DOWNLOAD_LINE_VAL_CHECK             = SEL_BF533_FRAME +23;//��·��ֵ��װ��У
const short LINE_VAL_VER                        = SEL_BF533_FRAME +24;//��·��ֵУ��
const short LINE_VAL_VER_QYC                    = SEL_BF533_FRAME +25;//��·��ֵУ���ȫң��
const short LINE_VAL_VER_SUCESS                 = SEL_BF533_FRAME +26;//��·��ֵУ��ɹ�
const short DOWNLOAD_LINE_COEF                  = SEL_BF533_FRAME +27;//��װ��·ͨ��ϵ��
const short DOWNLOAD_LINE_COEF_CHECK            = SEL_BF533_FRAME +28;//��װ��·ͨ��ϵ����У
const short DSP_VERSION_INQ                     = SEL_BF533_FRAME +29;//DSP�汾��ѯ
const short DSP_VERSION_INQ_CON                 = SEL_BF533_FRAME +29;//DSP�汾��ѯ�ظ�
const short DOWNLOAD_DELAY_VAL                  = SEL_BF533_FRAME +30;//��װ������ֵ
const short DOWNLOAD_DELAY_VAL_CHECK            = SEL_BF533_FRAME +31;//��װ������ֵ��У
const short DOWNLOAD_DELAY_VAL_SUCESS           = SEL_BF533_FRAME +32;//��װ������ֵ�ɹ�
const short DOWNLOAD_DELAY_VAL_SUCESS_CON       = SEL_BF533_FRAME +33;//��װ������ֵ�ɹ�
const short DELAY_EVENT_CON                     = SEL_BF533_FRAME +34;//�����¼�����
const short ALL_QYC_CON                         = SEL_BF533_FRAME +35;//ȫң��
const short ALL_QYX_CON                         = SEL_BF533_FRAME +36;//ȫң��
const short ACK_CON                             = SEL_BF533_FRAME +37;//ACK
const short HARMONIC_ACK                        = SEL_BF533_FRAME +38;//�ٻ�г��
const short HARMONIC_CON                        = SEL_BF533_FRAME +39;//�ٻ�г���ظ�
const short SEND_FILENAME_ACK                   = SEL_BF533_FRAME +40;//�����ļ���
const short SEND_FILENAME_CON                   = SEL_BF533_FRAME +41;//�����ļ���ȷ��
const short SEND_FILEBODY_ACK                   = SEL_BF533_FRAME +42;//�����ļ���
const short SEND_FILEBODY_CON                   = SEL_BF533_FRAME +43;//�����ļ���ȷ��
const short CALL_FILENAME_ACK                   = SEL_BF533_FRAME +44;//�ٻ��ļ���
const short CALL_FILENAME_CON                   = SEL_BF533_FRAME +45;//�ٻ��ļ���ȷ��
const short CALL_FILEBODY_ACK                   = SEL_BF533_FRAME +46;//�ٻ��ļ����ͱ���
const short CALL_VALCOEF_ACK                    = SEL_BF533_FRAME +47;//�ٻ���ֵϵ��
const short CALL_VALCOEF_CON                    = SEL_BF533_FRAME +48;//�ٻ���ֵϵ���ظ�
const short REBOOT_ACK                          = SEL_BF533_FRAME +49;//װ�ø�λ
const short REBOOT_CON                          = SEL_BF533_FRAME +50;//װ�ø�λ
const short CALL_TIME_ACK                       = SEL_BF533_FRAME +51;//У��ʱ��
const short CALL_TIME_CON                       = SEL_BF533_FRAME +52;//У��ʱ��
const short DOWNLOAD_LINE_CON                   = SEL_BF533_FRAME +53;//��װ��·ͨ��ϵ��
const short BOARD_REQ_ACK                       = SEL_BF533_FRAME +54;//�����ѯ
const short BOARD_REQ_CON                       = SEL_BF533_FRAME +55;//�����ѯ
const short CALL_PROVAL_ACK                     = SEL_BF533_FRAME +56;//�ٻ�������ֵ�ͱ���������
const short CALL_PROVAL_CON                     = SEL_BF533_FRAME +57;//�ٻ�������ֵ�ͱ���������
const short CALL_CHTYPE_ACK                     = SEL_BF533_FRAME +58;//�ٻ�ͨ������
const short CALL_CHTYPE_CON                     = SEL_BF533_FRAME +59;//�ٻ�ͨ������
const short CALL_LINEPARA_ACK                   = SEL_BF533_FRAME +60;//�ٻ�ͨ����Ϲ�ϵ
const short CALL_LINEPARA_CON                   = SEL_BF533_FRAME +61;//�ٻ�ͨ����Ϲ�ϵ
const short CALL_YMDATA_ACK                     = SEL_BF533_FRAME +62;//�ٻ����ֵ��
const short CALL_YMDATA_CON                     = SEL_BF533_FRAME +62;//�ٻ����ֵ�Ȼظ�
const short SET_BF518_PARA                      = SEL_BF533_FRAME +63;//����BF518��ز���
const short BATTERY_ACTIVE                      = SEL_BF533_FRAME +64;//��ػ
const short BATTERY_ACTIVE_CON                  = SEL_BF533_FRAME +65;//��ػȷ��
const short DOWNLOAD_PARA_CON                   = SEL_BF533_FRAME +66;//��װ�����ɹ�
const short BATTERY_ACTIVE_OVER                 = SEL_BF533_FRAME +67;//��ػ�˳�
const short BATTERY_ACTIVE_OVER_CON             = SEL_BF533_FRAME +68;//��ػ�˳�ȷ��
const short DOWNLOAD_LINE_BVAL                  = SEL_BF533_FRAME +69;//��·����������ֵ��װ
const short DOWNLOAD_LINE_BVAL_CHECK            = SEL_BF533_FRAME +70;//��·����������ֵ��װ��У
const short LINE_BVAL_VER                       = SEL_BF533_FRAME +71;//��·����������ֵУ��
const short LINE_BVAL_VER_QYC                   = SEL_BF533_FRAME +72;//��·����������ֵУ������ֵ
const short LINE_BVAL_VER_SUCESS                = SEL_BF533_FRAME +73;//��·����������ֵУ��ɹ�
const short LINE_DCVAL_VER                      = SEL_BF533_FRAME +74;//��·ֱ������ֵУ��
const short LINE_DCVAL_VER_SUCESS               = SEL_BF533_FRAME +75;//��·ֱ������ֵУ��ɹ�
const short LINE_DCVAL_VER_QYC                  = SEL_BF533_FRAME +76;//��·ֱ������ֵУ���ֵ
const short EVENT_MESSAGE                       = SEL_BF533_FRAME +77;//ZHANGZIHUA 20110309
const short CALL_PM_ANG                         = SEL_BF533_FRAME +78;//�ٻ��������
const short CALL_PM_ANG_CON                     = SEL_BF533_FRAME +79;//�ٻ�������ǻظ�
const short CALL_INTERFACE_PARA                 = SEL_BF533_FRAME +80;//�ٻ��ӿڲ���
const short CALL_INTERFACE_PARA_CON             = SEL_BF533_FRAME +81;
const short CALL_CRC_CON                        = SEL_BF533_FRAME +82;
const short THRANS_PUKEY_CON                    = SEL_BF533_FRAME +83;
const short REBOOT_BF533                        = SEL_BF533_FRAME +84;
const short REBOOT_BF533_CON                    = SEL_BF533_FRAME +85;
const short T_BOOT_BF533                        = SEL_BF533_FRAME +86;
const short ERASE_BF533                         = SEL_BF533_FRAME +87;
const short ERASE_BF533_CON                     = SEL_BF533_FRAME +88;
const short DOWN_BF533_PRO                      = SEL_BF533_FRAME +89;
const short DOWN_BF533_CON                      = SEL_BF533_FRAME +90;
const short YCI_SEND_BYTIME                     = SEL_BF533_FRAME +91;

//default cmd priority
const unsigned char START_ACT_PRIORITY            = 5;
const unsigned char START_CONFIRM_PRIORITY        = 5;
const unsigned char STOP_ACT_PRIORITY             = 4;
const unsigned char STOP_CONFIRM_PRIORITY         = 4;
const unsigned char TEST_ACT_PRIORITY             = 6;
const unsigned char TEST_CONFIRM_PRIORITY         = 6;
const unsigned char S_GRAM_FRAME_PRIORITY         = 1;	
const unsigned char CALL_ALL_DATA_ACT_PRIORITY    = 5;
const unsigned char CALL_ALL_DATA_CON_PRIORITY    = 5;
const unsigned char CALL_ALL_DATA_OVER_PRIORITY   = 5;
const unsigned char CALL_ALL_DD_ACT_PRIORITY      = 4;
const unsigned char CALL_ALL_DD_CON_PRIORITY      = 4;
const unsigned char CALL_ALL_YM_OVER_PRIORITY     = 4;
const unsigned char SYN_TIME_ACT_PRIORITY         = 6;
const unsigned char SYN_TIME_CON_PRIORITY         = 6;
const unsigned char YK_SEL_ACT_PRIORITY           = 7;
const unsigned char YK_SEL_CON_PRIORITY           = 7;
const unsigned char YK_EXE_ACT_PRIORITY           = 7;
const unsigned char YK_EXE_CON_PRIORITY           = 7;
const unsigned char YK_CANCEL_ACT_PRIORITY        = 7;
const unsigned char YK_CANCEL_CON_PRIORITY        = 7;
const unsigned char YK_OVER_PRIORITY              = 6;
const unsigned char REQUIRE_LINK_PRIORITY         = 5;
const unsigned char RESET_LINK_PRIORITY           = 5;
const unsigned char CALL_PRIMARY_DATA_PRIORITY    = 6;
const unsigned char CALL_SECONDARY_DATA_PRIORITY  = 2;
const unsigned char ALL_YX_DATA_PRIORITY          = 5;
const unsigned char ALL_YC_DATA_PRIORITY          = 4;
const unsigned char ALL_YM_DATA_PRIORITY          = 4;
const unsigned char COS_DATA_UP_PRIORITY          = 4;
const unsigned char SOE_DATA_UP_PRIORITY          = 3;
const unsigned char YCVAR_DATA_UP_PRIORITY        = 2;
const unsigned char CONFIRM_ACK_PRIORITY          = 5;
const unsigned char CONFIRM_NACK_PRIORITY         = 5;
const unsigned char LINK_STATUS_PRIORITY          = 5;
const unsigned char END_INIT_PRIORITY             = 3;
const unsigned char E5_PRIORITY                   = 1;
const unsigned char QUERY_DATA_PRIORITY           = 1;
const unsigned char RESET_CMD_PRIORITY            = 3;
const unsigned char TEST_CMD_PRIORITY             = 3;  
const unsigned char SINGLE_YX_DATA_PRIORITY       = 4;
const unsigned char SINGLE_YC_DATA_PRIORITY       = 4;
const unsigned char SINGLE_YM_DATA_PRIORITY       = 4;
const unsigned char TRANSMIT_FRAME_PRIORITY       = 1;
const unsigned char TRANS_DELAY_ACT_PRIORITY      = 5;
const unsigned char TRANS_DELAY_DOWNLOAD_PRIORITY = 7;

const unsigned char CALL_EXTEND_CPUINFO_PRIORITY  = 6;
const unsigned char CALL_EXTEND_PARA_PRIORITY     = 6;
const unsigned char SET_EXTEND_PARA_PRIORITY      = 6;
const unsigned char CALL_HISTORY_DATA_PRIORITY    = 5;
const unsigned char CALL_HISTORY_DATAS_PRIORITY   = 5;
const unsigned char CALL_FAULT_RECORD_PRIORITY    = 5;
const unsigned char CALL_YK_RECORD_PRIORITY       = 5;
const unsigned char CALL_YX_RECORD_PRIORITY       = 5;
const unsigned char EXTEND_CPUINFO_DATA_PRIORITY  = 6;
const unsigned char EXTEND_PARA_DATA_PRIORITY     = 6;
const unsigned char EXTEND_PARA_CON_PRIORITY      = 6;
const unsigned char HISTORY_DATA_UP_PRIORITY      = 5;
const unsigned char FAULT_RECORD_DATA_PRIORITY    = 5;
const unsigned char FAULT_RECORD_CON_PRIORITY     = 5;
const unsigned char YK_RECORD_DATA_PRIORITY       = 5;
const unsigned char YK_RECORD_CON_PRIORITY        = 5;
const unsigned char YX_RECORD_DATA_PRIORITY       = 5;
const unsigned char YX_RECORD_CON_PRIORITY        = 5;
const unsigned char HIGH_PRIORITY                 = 8;
const unsigned char CHECK_PUB_KEY_PRIORITY        = 7;
const unsigned char UPDATE_PUB_KEY_PRIORITY       = 7;
const unsigned char YC_SEND_BYTIME_PRIORITY       = 4;

//����for�ɶ�
const short CALL_JB_PARA_CON_PRIORITY           = 2;
const short SEND_JB_PARA_CON_PRIORITY           = 2;
const short ACT_JB_PARA_CON_PRIORITY            = 2;
const short DEACT_JB_PARA_CON_PRIORITY          = 2;
const short JB_SIGNAL_RESET_CON_PRIORITY        = 2;
const short JB_SOE_CON_PRIORITY                 = 2;
const short CALL_JB_PARA_ACT_PRIORITY           = 2;
const short SEND_JB_PARA_ACT_PRIORITY           = 2;
const short ACT_JB_PARA_ACT_PRIORITY            = 2;
const short DEACT_JB_PARA_ACT_PRIORITY          = 2;
const short JB_SIGNAL_RESET_ACT_PRIORITY        = 2;

//BF533�Զ����Լ��ʼ ZHANGZHIHUA
const short CALL_EQU_PARA_ACT_PRIORITY                   = 5;//�ٻ�װ�ò���
const short CALL_EQU_PARA_CON_PRIORITY                   = 5;//�ٻ�װ�ò���Ӧ����
const short DOWNLOAD_EQU_PARA_PRIORITY                   = 5;//��װװ�ò���
const short DOWNLOAD_EQU_PARA_SUCESS_PRIORITY            = 5;//��װװ�ò����ɹ�
const short DOWNLOAD_EQU_PARA_SUCESS_CON_PRIORITY        = 5;//��װװ�ò����ɹ�
const short DOWNLOAD_DELAY_CONTRL_PRIORITY               = 5;//��װ����������
const short DOWNLOAD_DELAY_CONTRL_CHECK_PRIORITY         = 5;//��װ���������ַ�У
const short DOWNLOAD_DELAY_CONTRL_SUCESS_PRIORITY        = 5;//������������װ�ɹ�
const short DOWNLOAD_DELAY_CONTRL_SUCESS_CON_PRIORITY    = 5;//������������װ�ɹ�
const short SIGNAL_RESET_PRIORITY                        = 5;//�źŸ���
const short SIGNAL_RESET_CON_PRIORITY                    = 5;//�źŸ���ȷ��
const short DOWNLOAD_CHANNEL_PARA_PRIORITY               = 5;//��װͨ������
const short DOWNLOAD_CHANNEL_PARA_CHECK_PRIORITY         = 5;//��װͨ��������У
const short DOWNLOAD_CHANNEL_PARA_SUCESS_PRIORITY        = 5;//��װͨ�������ɹ�
const short DOWNLOAD_CHANNEL_PARA_SUCESS_CON_PRIORITY    = 5;//��װͨ�������ɹ�
const short DOWNLOAD_CHANNEL_COMREL_PRIORITY             = 5;//��װͨ����Ϲ�ϵ
const short DOWNLOAD_CHANNEL_COMREL_CHECK_PRIORITY       = 5;//��װͨ����Ϲ�ϵ��У
const short DOWNLOAD_CHANNEL_COMREL_SUCESS_PRIORITY      = 5;//��װͨ����Ϲ�ϵ�ɹ�
const short DOWNLOAD_CHANNEL_COMREL_SUCESS_CON_PRIORITY  = 5;//��װͨ����Ϲ�ϵ�ɹ�
const short CALL_LINE_DATA_ACT_PRIORITY                  = 5;//�ٻ���·����
const short LINE_CYC_TIME_CON_PRIORITY                   = 5;//��·ȫң�ⶨʱ����
const short DOWNLOAD_LINE_VAL_PRIORITY                   = 5;//��·��ֵ��װ
const short DOWNLOAD_LINE_VAL_CHECK_PRIORITY             = 5;//��·��ֵ��װ��У
const short LINE_VAL_VER_PRIORITY                        = 5;//��·��ֵУ��
const short LINE_VAL_VER_QYC_PRIORITY                    = 5;//��·��ֵУ���ȫң��
const short LINE_VAL_VER_SUCESS_PRIORITY                 = 5;//��·��ֵУ��ɹ�
const short DOWNLOAD_LINE_COEF_PRIORITY                  = 5;//��װ��·ͨ��ϵ��
const short DOWNLOAD_LINE_COEF_CHECK_PRIORITY            = 5;//��װ��·ͨ��ϵ����У
const short DSP_VERSION_INQ_PRIORITY                     = 5;//DSP�汾��ѯ
const short DSP_VERSION_INQ_CON_PRIORITY                 = 5;//DSP�汾��ѯ�ظ�
const short DOWNLOAD_DELAY_VAL_PRIORITY                  = 5;//��װ������ֵ
const short DOWNLOAD_DELAY_VAL_CHECK_PRIORITY            = 5;//��װ������ֵ��У
const short DOWNLOAD_DELAY_VAL_SUCESS_PRIORITY           = 5;//��װ������ֵ�ɹ�
const short DOWNLOAD_DELAY_VAL_SUCESS_CON_PRIORITY       = 5;//��װ������ֵ�ɹ�
const short DELAY_EVENT_CON_PRIORITY                     = 5;//�����¼�����
const short HARMONIC_ACK_PRIORITY                        = 5;//�ٻ�г��
const short HARMONIC_CON_PRIORITY                        = 5;//�ٻ�г���ظ�
const short SEND_FILENAME_ACK_PRIORITY                   = 5;//�����ļ���
const short SEND_FILENAME_CON_PRIORITY                   = 5;//�����ļ���ȷ��
const short SEND_FILEBODY_ACK_PRIORITY                   = 5;//�����ļ�
const short SEND_FILEBODY_CON_PRIORITY                   = 5;//�����ļ�ȷ��
const short CALL_FILENAME_ACK_PRIORITY                   = 5;//�ٻ��ļ���
const short CALL_FILENAME_CON_PRIORITY                   = 5;//�ٻ��ļ���ȷ��
const short CALL_FILEBODY_ACK_PRIORITY                   = 5;//�ٻ��ļ����ͱ���
const short CALL_VALCOEF_ACK_PRIORITY                    = 5;//�ٻ��ļ����ͱ���
const short CALL_VALCOEF_CON_PRIORITY                    = 5;//�ٻ��ļ����ͱ���
const short REBOOT_ACK_PRIORITY                          = 5;//װ�ø�λ
const short REBOOT_CON_PRIORITY                          = 5;//װ�ø�λ
const short CALL_TIME_ACK_PRIORITY                       = 5;//ʱ��У��
const short CALL_TIME_CON_PRIORITY                       = 5;//ʱ��У��
const short DOWNLOAD_LINE_CON_PRIORITY                   = 5;//��װ��·ϵ��
const short BOARD_REQ_ACK_PRIORITY                       = 5;//�����ѯ
const short BOARD_REQ_CON_PRIORITY                       = 5;//�����ѯ
const short CALL_PROVAL_ACK_PRIORITY                     = 5;
const short CALL_PROVAL_CON_PRIORITY                     = 5;
const short CALL_CHTYPE_ACK_PRIORITY                     = 5;
const short CALL_CHTYPE_CON_PRIORITY                     = 5;
const short CALL_LINEPARA_ACK_PRIORITY                   = 5;
const short CALL_LINEPARA_CON_PRIORITY                   = 5;
const short CALL_YMDATA_ACK_PRIORITY                     = 5;
const short CALL_YMDATA_CON_PRIORITY                     = 5;
const short SET_BF518_PARA_PRIORITY                      = 5;
const short BATTERY_ACTIVE_PRIORITY                      = 5;
const short BATTERY_ACTIVE_CON_PRIORITY                  = 5;
const short DOWNLOAD_PARA_CON_PRIORITY                   = 5;
const short BATTERY_ACTIVE_OVER_PRIORITY                 = 5;
const short BATTERY_ACTIVE_OVER_CON_PRIORITY             = 5;
const short DOWNLOAD_LINE_BVAL_PRIORITY                  = 5;//��·����������ֵ��װ
const short DOWNLOAD_LINE_BVAL_CHECK_PRIORITY            = 5;//��·����������ֵ��װ��У
const short LINE_BVAL_VER_PRIORITY                       = 5;//��·����������ֵУ��
const short LINE_BVAL_VER_QYC_PRIORITY                   = 5;//��·����������ֵУ������ֵ
const short LINE_BVAL_VER_SUCESS_PRIORITY                = 5;//��·����������ֵУ��ɹ�
const short LINE_DCVAL_VER_PRIORITY                      = 5;
const short LINE_DCVAL_VER_SUCESS_PRIORITY               = 5;
const short LINE_DCVAL_VER_QYC_PRIORITY                  = 5;
const short EVENT_MESSAGE_PRIORITY                       = 5;
const short CALL_PM_ANG_PRIORITY                         = 5;
const short CALL_PM_ANG_CON_PRIORITY                     = 5;
const short CALL_INTERFACE_PARA_PRIORITY                 = 5;
const short CALL_INTERFACE_PARA_CON_PRIORITY             = 5;
const short CALL_CRC_CON_PRIORITY                        = 5;
const short THRANS_PUKEY_CON_PRIORITY                    = 5;

const short REBOOT_BF533_PRIORITY                        = 6;
const short REBOOT_BF533_CON_PRIORITY                    = 6;
const short T_BOOT_BF533_PRIORITY                        = 6;
const short ERASE_BF533_PRIORITY                         = 6;
const short ERASE_BF533_CON_PRIORITY                     = 6;
const short DOWN_BF533_PRO_PRIORITY                      = 6;
const short DOWN_BF533_CON_PRIORITY                      = 6;
const short YCI_SEND_BYTIME_PRIORITY                     = 3;


class CCmd
{
public:
	CCmd(typeCmd CmdType,unsigned char priority,share_commpoint_ptr commpoint,boost::any val);
	CCmd(typeCmd CmdType,unsigned char priority,share_commpoint_ptr commpoint);
	CCmd(void);
	~CCmd(void);
	unsigned char getPriority();
	typeCmd getCmdType();
	boost::any getVal();
	share_commpoint_ptr getCommPoint();
	int setCommPoint(share_commpoint_ptr val);
private:
	typeCmd CmdType_;
	unsigned char priority_;
	boost::any val_;
	share_commpoint_ptr commpoint_;
};

class CCmdQueue
{
public:
	CCmdQueue(void);
	~CCmdQueue(void);

	int getSendCmdQueueSum();
	void ClearSendCmdQueue();
	int getMaxPriopriySendCmd(CCmd & dstVal);

	int getWaitCmdQueueSum();
	int getWaitCmdQueueCount(share_commpoint_ptr commpoint);
	void ClearWaitCmdQueue();
	int getMaxPriopriyWaitCmd(CCmd & dstVal);
	int getMaxPriopriyWaitCmdByPointPtr(share_commpoint_ptr ptr,CCmd & dstVal);

	int AddSendCmdVal(CCmd val);
	int AddSendCmdVal(typeCmd CmdType,unsigned char priority,share_commpoint_ptr commpoint);
	int AddSendCmdVal(typeCmd CmdType,unsigned char priority,share_commpoint_ptr commpoint,boost::any val);
	
	int AddWaitCmdVal(CCmd val);
	int AddWaitCmdVal(typeCmd CmdType,unsigned char priority,share_commpoint_ptr commpoint);
	int AddWaitCmdVal(typeCmd CmdType,unsigned char priority,share_commpoint_ptr commpoint,boost::any val);
	int AddOnlyWaitCmdWithoutVal(typeCmd CmdType,unsigned char priority,share_commpoint_ptr commpoint,boost::any val);
	int AddOnlyWaitCmdByCmdType(typeCmd CmdType,unsigned char priority,share_commpoint_ptr commpoint,boost::any val);

protected:
	bool SearchSendCmdQueue(typeCmd CmdType,unsigned char Priority,share_commpoint_ptr Commpoint);
	bool SearchSendCmdQueue(typeCmd CmdType);

private:
	bool CompareCmdEqual(typeCmd srcCmdType,unsigned char srcPriority,share_commpoint_ptr srcCommpoint, typeCmd dstCmdType,unsigned char dstPriority,share_commpoint_ptr dstCommpoint);
	bool CompareCmdEqual(typeCmd srcCmdType,typeCmd dstCmdType);

//protected:
//	static bool ComparePrioprity(CCmd cmd1,CCmd cmd2);

private:
	std::vector<CCmd> SendCmdQueue_;
	std::vector<CCmd> WaitCmdQueue_;
	boost::mutex SendCmdMutex_;
	boost::mutex WaitCmdMutex_;
};

};//namespace Protocol {

#endif //#ifndef CmdQueue_H
