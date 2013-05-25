#include <boost/bind.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/exception/all.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <stack>
#include "Algorithm.h"
#include "OutLetSwitch.h"
#include "../FileSystem/Markup.h"
#include "../Protocol/Protocol.h"
#include "../FileSystem/Log.h"
#include "../FileSystem/LogFactory.h"
#include "../DataBase/DAOperate.h"
#include "../DataBase/YxPoint.h"

namespace CentralizedFA {

using namespace std;

const string strDA = "DA";
const string strSwitchNode = "SwitchNode";
const string strLogName = "LogName";
const string strInputYx = "InputYx";
const string strOutputYx = "OutputYx";
const string strPolar = "Polar";
const string strWaitYxTime = "WaitYxTime";
const string strWaitYkTime = "WaitYkTime";

//const string strSignResetTime = "SignResetTime";
//const string strOverLoadI  = "OverLoadChannel1";//��ʱ��¼����ǰ��Դ�������С
//const string strOverLoadII = "OverLoadChannel2";//��ʱ��¼����ǰ��Դ�������С
//const string strCheckOverLoad = "CheckOverLoad";//Ԥ�лָ����뿪�ع����� Ͷ��
//const string strOverLoadValue = "OverLoadValue";

const string strCheckFADisable = "CheckFADisable";

typedef CMyBTreeNode<share_switchnode_ptr> SwitchTreeNode;

const unsigned char OutYx_FA_Enable = 0;         //����FAͶ��
const unsigned char OutYx_FA_MulFault = 1;      //FA���ع���
//const unsigned char OutYx_FA_OverLoadAll =2;     //������

CCentralizedDA::CCentralizedDA(/*boost::asio::io_service & io_service,*/std::string id,DataBase::CDAOperate & da_op)
						:id_(id),
						da_op_(da_op)
						//io_service_(io_service),
						//TimerSignReset_(io_service),
						//TimerGetLoad_(io_service)
{
	thread_run_ = false;

	//SignResetTime_=SIGN_RESET_TIME_DEFAULT;

	//CheckOverLoad_=0;
	//bCheckOverLoad_=false;
	//OverLoadValue_=6000;

	CheckFADisable_=false;
	//YcOverLoadI_=0;
	//YcOverLoadII_=0;

	//YcOverLoadValue1_=0;
	//YcOverLoadValue2_=0;

	//bSwitchRefuseSign_=false;
	//bDisableOutLed_=true;

	bReset_ = false;

	WaitYxTime_=500;//ms
	WaitYkTime_=5;//ms

	log_.reset();
}

CCentralizedDA::~CCentralizedDA(void)
{
}

int CCentralizedDA::AddLog(std::string val)
{
#ifdef Debug_DA
	std::cout<<val;
#endif

	if (log_)
	{
		return log_->AddRecordWithSynT(val);
	}

	return -1;
}

std::string CCentralizedDA::getID()
{
	return id_;
}

int CCentralizedDA::ConnectSubAlgorithmSig(CmdRecallSignalType & sig)
{
	SubAlgorithmSigConnection_ = sig.connect(boost::bind(&CCentralizedDA::ProcessAlgorithmSig,this,_1,_2,_3,_4));

	return 0;
}
int CCentralizedDA::DisconnectSubAlgorithmSig()
{
	SubAlgorithmSigConnection_.disconnect();

	return 0;
}

int CCentralizedDA::InitAlgorithm()
{
	return LoadXmlCfg(id_);
}

void CCentralizedDA::UninitAlgorithm()
{

}

share_switchnode_ptr CCentralizedDA::getWitchNodeByID(typeID val)
{
	for (std::vector<share_switchnode_ptr>::iterator it = OriSwitchNodes_.begin();it != OriSwitchNodes_.end();it++)
	{
		if ((*it)->getSwitchID() == val)
		{
			return *it;
		}
	}

	return share_switchnode_ptr();
}

int CCentralizedDA::BuildLeafQueue(queue<share_switchnode_ptr> & leafQueue)
{
	if (!tree_.IsEmpty())
	{
		if (tree_.IsLeaf())
		{
			leafQueue.push(tree_.GetCurrentData());
		}

		SwitchTreeNode * curPtr = tree_.GetCurrentPointer();
		bool bRet = tree_.FirstChild();
		while(bRet)
		{
			BuildLeafQueue(leafQueue);
			bRet = tree_.NextSibling();
		}
		tree_.SetCurrentPointer(curPtr);
	}

	return 0;
}

int CCentralizedDA::BuildForest()
{
	if(tree_.Root())                 //ɾ����
	{
		tree_.RemovesubTree();
	}

	//�������ӵ�
	faultSet_.clear();
	ykCloseSet_.clear();
	ykOpenSet_.clear();
	outLetswitchSet_.clear();

	if (OriSwitchNodes_.size() == 0)
	{
		return -1;
	}

	tree_.BuildRoot(share_switchnode_ptr()); //����һ���ս����Ϊ���������ܸ�����ɭ��ת��Ϊһ�ö�����

	queue<share_switchnode_ptr> searchQueue;
	vector<share_switchnode_ptr> dealSwitchNodes = OriSwitchNodes_;

	for (vector<share_switchnode_ptr>::iterator it = dealSwitchNodes.begin(); it != dealSwitchNodes.end();) //�ҵ�Դ���͵Ŀ��ؽ�㣬��Ϊ����
	{
		if ((*it)->getbPowerNode()) //�����Դ��㣬����Ϊ�ո�����Ů
		{
			if (tree_.Root()) //currentָ��ָ�������
			{
				tree_.InsertChild(*it);

				std::ostringstream ostr;
				ostr<<"Power Node = "<<(*it)->getSwitchID()<<std::endl;
				AddLog(ostr.str());

				if ((*it)->getSwitchPosition())
				{
					std::ostringstream ostr;
					ostr<<"Search Node Push "<<(*it)->getSwitchID()<<std::endl;
					AddLog(ostr.str());

					searchQueue.push(*it); //���Դ�㿪��λ��Ϊ��λ����Դ��������������
				}

				it = dealSwitchNodes.erase(it); //�ӿ��ض�����ȡ��������Ľ��
				continue;
			}
		}

		it++;
	}

	while(!searchQueue.empty()) //������������
	{
		share_switchnode_ptr node = searchQueue.front();
		
		if (tree_.Find(node)) //currentָ��ָ��ǰ���
		{
			for (vector<share_switchnode_ptr>::iterator it = dealSwitchNodes.begin(); it != dealSwitchNodes.end();) 
			{
				if((*it)->CheckNodebLinked(node->getSwitchID())) //�ҳ��뵱ǰ��������ӹ�ϵ��δ������Ľ��
				{
					tree_.InsertChild(*it); //���ӽ���Ϊ��ǰ���ĺ���
					if ((*it)->getSwitchPosition())
					{
						searchQueue.push(*it);

						std::ostringstream ostr;
						ostr<<"Search Node Push "<<(*it)->getSwitchID()<<std::endl;
						AddLog(ostr.str());
					}

					it = dealSwitchNodes.erase(it);
					continue;
				}

				it++;
			}
		}

		searchQueue.pop();
	}

	queue<share_switchnode_ptr> leafQueue;
	if (tree_.Root()) //currentָ��ָ�������
	{
		BuildLeafQueue(leafQueue);  //�����������н�㣬����Ҷ������LeafQueue����
	}

	while(!leafQueue.empty()) //����������Ҷ���
	{
		share_switchnode_ptr node = leafQueue.front();

		if (!node->getSwitchPosition()) //���ĳ��Ҷ��㿪��״̬Ϊ��
		{
			if (tree_.Find(node))
			{
				SwitchTreeNode * srcLinkPtr = tree_.GetCurrentPointer(); //���浱ǰҶ����ָ��

				share_switchnode_ptr parent;
				vector<share_switchnode_ptr> brotherSet;
				if (tree_.Parent()) //ָ��ָ��node���ĸ����
				{
					parent = tree_.GetCurrentData(); //��¼nodeҶ��㸸��������
					bool bRet = tree_.FirstChild(); //ָ��ָ��node��㸸���ĵ�һ����Ů���
					while(bRet)
					{
						brotherSet.push_back(tree_.GetCurrentData()); //����node��������ֵܽ�������
						bRet = tree_.NextSibling();
					}
				}
				else
				{
					parent.reset();
				}

				SwitchTreeNode * dstLinkPtr = NULL;
				for (vector<share_switchnode_ptr>::iterator it = OriSwitchNodes_.begin();it != OriSwitchNodes_.end();it++)
				{ 
					if(node->CheckNodebLinked((*it)->getSwitchID())) //���������뵱ǰ��������ӹ�ϵ�Ľ��
					{
						if (parent != NULL) //��ǰ����и����
						{
							if(*it != parent) //�ų��ø��ڵ�
							{
								bool isBrother = false;
								for (vector<share_switchnode_ptr>::iterator mt = brotherSet.begin();mt != brotherSet.end();mt++)
								{
									if (*mt == *it) //�����ӽ��Ϊ�ֵܽ��,������Ҫ��
									{
										isBrother = true;
										break;
									}
								}

								if (!isBrother) //�����ӽ�㲻���ֵܽ����ж�����֦
								{
									if (tree_.Find(*it))
									{
										if (dstLinkPtr) //��node�����һ���������ӵ㣬ȡ����Ϊ�����ĵ�Ϊ����֦
										{
											SwitchTreeNode * dstLinkBackup = tree_.GetCurrentPointer();
											while(!tree_.IsRoot())
											{
												tree_.Parent();
												if (dstLinkPtr == tree_.GetCurrentPointer())
												{
													break;
												}
											}

											if (tree_.IsRoot())
											{
												dstLinkPtr = dstLinkBackup;
											}
										}
										else //��node���ֻ��һ�����ӵ㣬���õ���Ϊ����֦
										{
											dstLinkPtr = tree_.GetCurrentPointer();
										}
									}
								}
							}
						}
						else //��ǰ���û�и����
						{
							if (tree_.Find(*it)) //ȡ����뵱ǰ��������ӹ�ϵ�Ľ���зǸ��Ľ����Ϊ��ǰ��������֦
							{
								if (dstLinkPtr)
								{
									SwitchTreeNode * dstLinkBackup = tree_.GetCurrentPointer();
									while(!tree_.IsRoot())
									{
										tree_.Parent();
										if (dstLinkPtr == tree_.GetCurrentPointer())
										{
											break;
										}
									}

									if (tree_.IsRoot())
									{
										dstLinkPtr = dstLinkBackup;
									}
								}
								else //��node���ֻ��һ�����ӵ㣬���õ���Ϊ����֦
								{
									dstLinkPtr = tree_.GetCurrentPointer();
								}
							}
						}
					}
				}
				
				if (dstLinkPtr)
				{
					tree_.SetLink(srcLinkPtr,dstLinkPtr); //Ϊ������㽨������֦
				}
			}
		}

		leafQueue.pop();
	}

	return 0;
}

int CCentralizedDA::CheckFault()
{
	bool bRet = false;

	if (tree_.Root()) //currentָ��ָ��ɭ�ֵĿ�����
	{
		bRet = tree_.FirstChild(); //currentָ��ָ���һ������
	}

	queue<share_switchnode_ptr> faultPowerQueue;
	queue<share_switchnode_ptr> resumeQueue;
	while(bRet) //ѭ��������������������Ů��㣬���������е�Դ���
	{
		share_switchnode_ptr node = tree_.GetCurrentData();

		if(node)
		{
			if (node->getSwitchProtected()) //Դ��ı��������ź�����
			{
				share_outletswitch_ptr outlet = node->getOutLetSwitch();
				if (outlet && outlet->AllowYx()) //�����Դ��ǰ��һ�����߿���,���Ҹó��߿�����Ҫ�ж�ң��
				{
					if ((!outlet->getSwitchPosition()) && (outlet->getSwitchProtected())) //�жϳ��߿��صı��������Ϳ���λ���ź�
					{
						faultPowerQueue.push(node);
					}
				}
				else
				{
					faultPowerQueue.push(node);
				}

				faultSet_.push_back(node); //����⵽�Ĺ��Ͻ���¼����
			}
			else //���Դ��û�й����źţ������ڶ�·��ȥ���������
			{
				share_outletswitch_ptr outlet = node->getOutLetSwitch();
				if (outlet && outlet->AllowYx()) //�����Դ��ǰ��һ�����߿���,���Ҹó��߿�����Ҫ�ж�ң��
				{
					if ((!outlet->getSwitchPosition()) && (outlet->getSwitchProtected())) //�жϳ��߿��صı��������Ϳ���λ���źţ�������߿��ض�����Դ������û�й����źţ�˵��������Դ��ͳ��ֿ���֮��
					{
						outlet->setAllowYk(false);
						bReset_ = true;

						ykOpenSet_.push_back(node);
						resumeQueue.push(node);
					}
				}
			}
		}

		bRet = tree_.NextSibling();
	}

	int result = 0;

	if (!faultPowerQueue.empty())
	{
		result = SearchFaultNode(faultPowerQueue);
	}

	if (!resumeQueue.empty())
	{
		result = Resume(resumeQueue);
	}

	return result;
}

int CCentralizedDA::SearchFaultNode(std::queue<share_switchnode_ptr> & powerQueue)
{
	queue<share_switchnode_ptr> insulateQueue;

	while(!powerQueue.empty())
	{
		share_switchnode_ptr powerNode = powerQueue.front();
		share_switchnode_ptr node = powerQueue.front();

		if (tree_.Find(node)) //ָ��ָ��node���
		{
			queue<SwitchTreeNode *> scopeQueue; //������ȱ�������ʱ�õĴ����Ů���Ķ���
			if (!tree_.IsEmpty())
			{
				SwitchTreeNode * curPtr = tree_.GetCurrentPointer(); //���浱ǰ���ָ��
				scopeQueue.push(curPtr);

				while(!scopeQueue.empty()) //������ȱ�������
				{
					SwitchTreeNode * tmpPtr = scopeQueue.front();

					tree_.SetCurrentPointer(tmpPtr);
					share_switchnode_ptr parentNode = tree_.GetCurrentData();
					share_switchnode_ptr node = tree_.GetCurrentData();

					bool parentNoFault = false;
					bool parentNeedInsulate = false;
					if (node->getSwitchProtected()) //��ǰ��㷢�ֹ���ң��
					{
						if (tree_.IsLeaf()) //����ǰ���ΪҶ��㣬�ý�㼴Ϊ�����ն˽��֮һ
						{
							insulateQueue.push(node); //����������

							share_outletswitch_ptr outletswitch = powerNode->getOutLetSwitch(); 
							if (outletswitch) //����ù��Ͻ���Դ��ǰ��һ�����߿���
							{
								if (outletswitch->AllowYk()) //�ó��ֿ�������ң��
								{
									AddOutLetSwitch(outletswitch); //���ó��߿��ض����������բ�ĳ��߿��ض���
								}
							}

							tree_.FirstChild();

							parentNeedInsulate = false;
						}
						else //����ǰ���Ϊ��Ҷ���
						{
							tree_.FirstChild(); //׼������������Ů���

							parentNeedInsulate = true;//������Ů���󣬿����жϳ���ǰ���ҲΪ�����ն˽��
						}

						parentNoFault = false;

						faultSet_.push_back(node); //����⵽�Ĺ��Ͻ���¼����
					}
					else //��ǰ���δ���ֹ���ң��
					{
						tree_.FirstChild();

						parentNeedInsulate = false;
						parentNoFault = true; //������Ů���󣬿����жϳ�����һ��ң����
					}

					while(!tree_.IsEmpty()) //������ǰ����������Ů���
					{
						scopeQueue.push(tree_.GetCurrentPointer());
						share_switchnode_ptr node = tree_.GetCurrentData();
						if (node->getSwitchProtected()) //ĳ��Ů��㷢�ֹ���ң��
						{
							parentNeedInsulate = false; //����Ů���ĸ���㲻����Ϊ�����ն˽��
							if (parentNoFault) //����Ů���ĸ����δ���ֹ���ң��
							{
								cerr<<"����ң����"<<endl;
								return -1;  //����ң���󱨣�����ȡ�κθ����ָ���ʩ
							}

							faultSet_.push_back(node); //����⵽�Ĺ��Ͻ���¼����
						}

						tree_.NextSibling();
					}

					if (parentNeedInsulate) //����������Ů�����жϳ���ǰ���Ϊ�����ն˽��
					{
						insulateQueue.push(parentNode);

						share_outletswitch_ptr outletswitch = powerNode->getOutLetSwitch(); 
						if (outletswitch) //����ù��Ͻ���Դ��ǰ��һ�����߿���
						{
							if (outletswitch->AllowYk()) //�ó��ֿ�������ң��
							{
								AddOutLetSwitch(outletswitch); //���ó��߿��ض����������բ�ĳ��߿��ض���
							}
						}
					}

					scopeQueue.pop();
				}

				tree_.SetCurrentPointer(curPtr);
			}
		}

		powerQueue.pop();
	}

	int ret = 0;

	if (!insulateQueue.empty())
	{
		ret = Insulate(insulateQueue);
	}

	return ret;
}

int CCentralizedDA::Insulate(queue<share_switchnode_ptr> & insulateQueue)
{
	queue<share_switchnode_ptr> resumeQueue;

	while(!insulateQueue.empty())
	{
		share_switchnode_ptr node = insulateQueue.front();

		if (node->getSwitchPosition()) //��������Ǻ�բλ��
		{
			ykOpenSet_.push_back(node); //���ÿ��ؼ����բ����
		}
		
		if(tree_.Find(node))
		{
			bool bRet = tree_.FirstChild();
			while(bRet) //�����ù����ն˽�����Ů���
			{
				share_switchnode_ptr node = tree_.GetCurrentData();
				if (node->getSwitchPosition()) //��������Ǻ�բλ��
				{
					ykOpenSet_.push_back(node); //���ÿ��ؼ����բ����
					resumeQueue.push(node);  //������Ů������ָ�����
				}

				bRet = tree_.NextSibling();
			}
		}

		insulateQueue.pop();
	}

	int ret = 0;

	if (!resumeQueue.empty())
	{
		ret = Resume(resumeQueue);
	}

	return ret;
}

int CCentralizedDA::Resume(queue<share_switchnode_ptr> & resumeQueue)
{
	typedef queue<share_switchnode_ptr> share_switchnode_queue;
	vector<share_switchnode_queue> resume_set;

	while(!resumeQueue.empty())
	{
		share_switchnode_ptr node = resumeQueue.front();
		
		if (tree_.Find(node))
		{
			share_switchnode_ptr rootNode = node; //��¼��resume�����н��Ϊ�������������������
			SwitchTreeNode * curPtr = tree_.GetCurrentPointer(); //���浱ǰ���ָ��

			queue<share_switchnode_ptr> tmp_queue;
			stack<SwitchTreeNode *> tmp_stack;
			do 
			{
				while(!tree_.IsEmpty())
				{
					if (tree_.HasLinkNode()) //����õ�������֦
					{
						share_switchnode_ptr node = tree_.GetCurrentData();
						share_switchnode_ptr linknode = tree_.GetLinkData();

						if (node != rootNode) //���������֦�ĸý�㲻�������ĸ���㣬������resume�����еĽ��
						{
							//�жϸý�������������ң��λ�ã�����ң��λ���෴����������������λ�Ľ��������բ����
							if (node->getSwitchPosition() && (!linknode->getSwitchPosition()))
							{
								tmp_queue.push(linknode);
							}
							else if ((!node->getSwitchPosition()) && linknode->getSwitchPosition())
							{
								tmp_queue.push(node);
							}
						}
						else //���������֦�ĸý���������ĸ���㣬����resume�����еĽ��
						{
							if (!linknode->getSwitchPosition()) //ֻ�����������λ�ڷ�Ϊ���������������������������բ����
							{
								tmp_queue.push(linknode);
							}
						}
					}

					tmp_stack.push(tree_.GetCurrentPointer());
					tree_.FirstChild();
				}

				while(tree_.IsEmpty() && (!tmp_stack.empty()))
				{
					tree_.SetCurrentPointer(tmp_stack.top());
					tmp_stack.pop();
 
					if (tree_.GetCurrentData() != rootNode) //����������������δ��λ�������ĸ���㣬��resume�����еĽ�㣬���������������
					{
						tree_.NextSibling();
					}
					else //���������������Ѿ���λ�������ĸ���㣬��resume�����еĽ�㣬�򱾿�����������������׼��������resume��������һ�����Ϊ����������
					{
						tree_.SetCurrentPointer(NULL);
					}
				}
			}
			while (!tree_.IsEmpty());

			tree_.SetCurrentPointer(curPtr);//��ָ��ָ������λ��

			if (tmp_queue.size() == 1) //���resume�����е�ĳ���ֻ��һ������֦
			{
				ykCloseSet_.push_back(tmp_queue.front());
				tmp_queue.pop();
			}
			else if (tmp_queue.size() > 1) //���resume�����е�ĳ����ж�������֦
			{
				resume_set.push_back(tmp_queue); //������֦������뱸ѡ����������
			}
		}

		resumeQueue.pop();
	}

	if (resume_set.size() > 0) //���resume������ĳ�����һ���������翪�أ��������㷨ѡ��һ������
	{
		bool bAddYkCloseQueue=true;
		while(bAddYkCloseQueue)
		{
			bAddYkCloseQueue = false; //Ĭ�ϱ�whileѭ��ִֻ��һ��

			for (vector<share_switchnode_queue>::iterator it = resume_set.begin();it != resume_set.end();) //�������е��ж������֦�Ľ��
			{
				for(size_t j=0;j<(*it).size();j++) //����ĳ��������֦һ��
				{
					//vector<share_switchnode_ptr> ykCloseQueueTmp = ykCloseQueue_; //��YKCloseQueue��������Copy
					share_switchnode_ptr ykNode = (*it).front();

					bool bUsed = false;
					for (vector<share_switchnode_ptr>::iterator mt = ykCloseSet_.begin(); mt != ykCloseSet_.end();mt++)//����YKCloseQueue����
					{
						if (ykNode == *mt) //���ĳ��������Ѿ�����������ʹ��
						{
							(*it).pop(); //�ӱ�ѡ�����н����������ɾ��
							bUsed = true;
							break;
						}
					}

					if (!bUsed) //������������û�б������κη���ʹ��
					{
						(*it).pop();
						(*it).push(ykNode); //�������������뱸ѡ�������е����λ��,�������ڱ����ڶ��Ρ�
					}
				}

				if ((*it).size() <= 1) //���ĳ��ѡ����������֦�ĸ����Ѿ�С��1
				{
					if ((*it).size() > 0) 
					{
						ykCloseSet_.push_back((*it).front()); //��������������YKCloseQueue���С�
						(*it).pop();

						bAddYkCloseQueue = true;
					}

					it = resume_set.erase(it);
					continue;
				}

				it++;
			}
		}

		for (vector<share_switchnode_queue>::iterator it = resume_set.begin();it != resume_set.end();) //whileѭ���������ټ��һ��ʣ�µı�ѡ����
		{
			//�Ӷ�����������ѡ���һ������YKCloseQueue���У���Ϊ�ñ�ѡ�����ĺ�բ���
			ykCloseSet_.push_back((*it).front());
			it = resume_set.erase(it);
		}
	}

	return 0;
}

int CCentralizedDA::AddOutLetSwitch(share_outletswitch_ptr outlet)
{
	for (vector<share_outletswitch_ptr>::iterator it = outLetswitchSet_.begin(); it != outLetswitchSet_.end(); it++)
	{
		if ((*it) == outlet)
		{
			return 1;
		}
	}

	outLetswitchSet_.push_back(outlet);

	return 0;
}

void CCentralizedDA::ClearAlgorithmData()
{
	if (tree_.Root())
	{
		tree_.RemovesubTree();
	}

	faultSet_.clear();
	OriSwitchNodes_.clear();
	ykCloseSet_.clear();
	ykOpenSet_.clear();
	outLetswitchSet_.clear();
}

int CCentralizedDA::LoadXmlCfg(std::string filename)
{
	FileSystem::CMarkup xml;

	if (!xml.Load(filename))
	{
		std::cerr<<"����xml�����ļ�"<<filename<<"ʧ�ܣ�"<<std::endl;

		return -1;
	}

	ClearAlgorithmData();

	xml.ResetMainPos();
	xml.FindElem(); //root
	xml.IntoElem();  //enter root

	xml.ResetChildPos();
	if (xml.FindElem(strLogName))
	{
		std::string strTmp = xml.GetData();
		boost::trim(strTmp);
		log_.reset(FileSystem::CLogFactory::CreateLog(strTmp,FileSystem::strTextLog));
	}

	xml.ResetMainPos();
	int count = 0;
	while (xml.FindElem(strSwitchNode))
	{
		share_switchnode_ptr switchPtr(new CSwitchNode(da_op_));

		xml.IntoElem();  //enter strSwitchNode

		try
		{
			switchPtr->LoadXmlCfg(xml);
		}
		catch(PublicSupport::dat2def_exception & e)
		{
			switchPtr.reset();

			std::string const * strPtr = boost::get_error_info<boost::errinfo_type_info_name>(e);
			if (strPtr)
			{
				std::ostringstream ostr;
				ostr<<"��ʼ����"<<count<<"��"<<strSwitchNode<<"ʧ�ܣ�";

				std::string const * strPtrMiddle = boost::get_error_info<errinfo_middletype_name>(e);
				if (strPtrMiddle)
				{
					ostr<<" item info:"<<(*strPtrMiddle);
				}

				int const * indexPtr = boost::get_error_info<boost::errinfo_errno>(e);
				if (indexPtr)
				{
					ostr<<" data no:"<<(*indexPtr)<<"��";
				}

				ostr<<"error info:"<<(*strPtr)<<"��";
				ostr<< "����������ý�������,���˳�DA"<<std::endl;
				AddLog(ostr.str());
			}

			return -1;
		}

		xml.OutOfElem(); //out strSwitchNode

		OriSwitchNodes_.push_back(switchPtr);

		count++;
	}

	xml.ResetMainPos();
	while(xml.FindElem(strInputYx))
	{
		try
		{
			string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			size_t index = boost::lexical_cast<size_t>(strTmp);

			strTmp = xml.GetAttrib(strPolar);
			bool polar = true;
			if (boost::iequals(strTmp,strboolFalse))
			{
				polar = false;
			}

			strTmp = xml.GetAttrib(strOutputYx);
			int out = boost::lexical_cast<int>(strTmp);

			stIn val(index,out,polar);
			input_set_.push_back(val);
		}
		catch(boost::bad_lexical_cast & e)
		{
			ostringstream ostr;
			ostr<<"�Ƿ�������ң�Ų���:"<<e.what();
			cerr<<ostr.str();
		}
	}

	xml.ResetMainPos();
	while(xml.FindElem(strOutputYx))
	{
		try
		{
			string strTmp = xml.GetData();
			boost::algorithm::trim(strTmp);
			size_t index = boost::lexical_cast<size_t>(strTmp);
			strTmp = xml.GetAttrib(strPolar);
			bool polar = true;
			if (boost::iequals(strTmp,strboolFalse))
			{
				polar = false;
			}
			stOut val(index,polar);
			output_set_.push_back(val);
		}
		catch(boost::bad_lexical_cast & e)
		{
			ostringstream ostr;
			ostr<<"�Ƿ������ң�Ų���:"<<e.what();
			cerr<<ostr.str();
		}
	}

	//xml.ResetMainPos();
	//if(xml.FindElem(strSignResetTime))
	//	{
	//		std::string strTmp = xml.GetData();
	//		boost::algorithm::trim(strTmp);
	//		try
	//		{
	//			unsigned short timeout = boost::lexical_cast<unsigned short>(strTmp);
	//			SignResetTime_=timeout;
	//		}
	//		catch(boost::bad_lexical_cast& e)
	//		{
	//			std::ostringstream ostr;
	//			ostr<<e.what();
	//		}
	//	}

	//xml.ResetMainPos();
	//if (xml.FindElem(strOverLoadI))
	//{
	//	try
	//	{
	//		string strTmp = xml.GetData();
	//		boost::algorithm::trim(strTmp);
	//		unsigned short value = boost::lexical_cast<size_t>(strTmp);
	//		YcOverLoadI_=value;
	//	}
	//	catch(boost::bad_lexical_cast& e)
	//	{
	//		ostringstream ostr;
	//		ostr<<"�Ƿ���ң���Ų���:"<<e.what();

	//		throw PublicSupport::dat2def_exception()<<boost::errinfo_type_info_name(ostr.str());
	//	}
	//}
	//xml.ResetMainPos();
	//if (xml.FindElem(strOverLoadII))
	//{
	//	try
	//	{
	//		string strTmp = xml.GetData();
	//		boost::algorithm::trim(strTmp);
	//		unsigned short value = boost::lexical_cast<size_t>(strTmp);
	//		YcOverLoadII_=value;
	//	}
	//	catch(boost::bad_lexical_cast& e)
	//	{
	//		ostringstream ostr;
	//		ostr<<"�Ƿ���ң���Ų���:"<<e.what();

	//		throw PublicSupport::dat2def_exception()<<boost::errinfo_type_info_name(ostr.str());
	//	}
	//}

	//xml.ResetMainPos();
	//if (xml.FindElem(strCheckOverLoad))
	//{
	//	string strTmp = xml.GetData();
	//	boost::algorithm::trim(strTmp);
	//	if (boost::iequals(strTmp,strboolTrue))
	//	{
	//		bCheckOverLoad_ = true;
	//	}
	//	else
	//	{
	//		bCheckOverLoad_ = false;
	//	}
	//}

	xml.ResetMainPos();
	if (xml.FindElem(strCheckFADisable))
	{
		std::string strTmp = xml.GetData();
		boost::algorithm::trim(strTmp);
		try
		{
			int value = boost::lexical_cast<int>(strTmp);
			CheckFADisable_=value;
		}
		catch(boost::bad_lexical_cast& e)
		{
			std::ostringstream ostr;
			ostr<<e.what();
		}
	}

	xml.ResetMainPos();
	if(xml.FindElem(strWaitYxTime))
	{
		std::string strTmp = xml.GetData();
		boost::algorithm::trim(strTmp);
		try
		{
			int timeout = boost::lexical_cast<int>(strTmp);
			WaitYxTime_=timeout;
		}
		catch(boost::bad_lexical_cast& e)
		{
			std::ostringstream ostr;
			ostr<<e.what();
		}
	}


	xml.ResetMainPos();
	if(xml.FindElem(strWaitYkTime))
	{
		std::string strTmp = xml.GetData();
		boost::algorithm::trim(strTmp);
		try
		{
			int timeout = boost::lexical_cast<int>(strTmp);
			WaitYkTime_=timeout;
		}
		catch(boost::bad_lexical_cast& e)
		{
			std::ostringstream ostr;
			ostr<<e.what();
		}
	}


	//xml.ResetMainPos();
	//if(xml.FindElem(strOverLoadValue))
	//{
	//	std::string strTmp = xml.GetData();
	//	boost::algorithm::trim(strTmp);
	//	try
	//	{
	//		int value = boost::lexical_cast<int>(strTmp);
	//		OverLoadValue_=value;
	//	}
	//	catch(boost::bad_lexical_cast& e)
	//	{
	//		std::ostringstream ostr;
	//		ostr<<e.what();
	//	}
	//}


	xml.OutOfElem(); //out root

	return 0;
}

void CCentralizedDA::SaveXmlCfg(std::string filename)
{
	FileSystem::CMarkup xml;
	xml.SetDoc(strXmlHead);
	xml.SetDoc(strSubXsl);

	xml.AddElem(strDA);
	xml.IntoElem();//enter root

	if (log_)
	{
		xml.AddElem(strLogName,log_->getLogPath());
	}

	for (vector<share_switchnode_ptr>::iterator it = OriSwitchNodes_.begin(); it != OriSwitchNodes_.end();it++)
	{
		xml.AddElem(strSwitchNode);

		xml.IntoElem();  //enter strSwitchNode

		(*it)->SaveXmlCfg(xml);

		xml.OutOfElem(); //out strSwitchNode
	}

	for(std::vector<stIn>::iterator it = input_set_.begin();it != input_set_.end();it++)
	{
		std::string polar;
		if((*it).polar_)
		{
			polar = strboolTrue;
		}
		else
		{
			polar = strboolFalse;
		}

		xml.AddElem(strInputYx,(*it).index_);
		xml.AddAttrib(strPolar,polar);
		xml.AddAttrib(strOutputYx,(*it).out_);
	}

	for(std::vector<stOut>::iterator it = output_set_.begin();it != output_set_.end();it++)
	{
		std::string polar;
		if ((*it).polar_)
		{
			polar = strboolTrue;
		}
		else
		{
			polar = strboolFalse;
		}

		xml.AddElem(strOutputYx,(*it).index_);
		xml.AddAttrib(strPolar,polar);
	}

	xml.OutOfElem();//out root
	xml.Save(filename);//save xml file
}

int CCentralizedDA::OpenSwitch(share_switchnode_ptr node)
{
	if((node)->AllowYk())
	{
		int ret = (node)->OpenSwitch();
		if (ret)
		{
			std::ostringstream ostr;
			ostr<<"���뿪��"<<(node)->getSwitchID()<<"ʧ�ܣ��˳�����DA"<<std::endl;
			AddLog(ostr.str());

			return ret;
		}

		return WaitYkResult(node,boost::posix_time::second_clock::local_time(),false);
	}

	return 0;
}

int CCentralizedDA::CloseSwitch(share_switchnode_ptr node)
{
	if(((node)->AllowYk())/*&&(!bSwitchRefuseSign_)*/)
	{
		int ret = (node)->CloseSwitch();
		if (ret)
		{
			std::ostringstream ostr;
			ostr<<"�ָ����翪��"<<(node)->getSwitchID()<<"ʧ�ܣ��˳�����DA"<<std::endl;
			AddLog(ostr.str());

			return ret;
		}

		return WaitYkResult(node,boost::posix_time::second_clock::local_time(),true);
	}

	return 0;
}

int CCentralizedDA::OpenOutlet(share_outletswitch_ptr node)
{
	return 0;
}

int CCentralizedDA::CloseOutlet(share_outletswitch_ptr node)
{
	if((node)->AllowYk())
	{
		int ret = (node)->CloseSwitch();
		if (ret)
		{
			std::ostringstream ostr;
			ostr<<"�ָ����߿���"<<(node)->getSwitchID()<<"ʧ�ܣ��˳�����DA"<<std::endl;
			AddLog(ostr.str());

			return ret;
		}

		return WaitYkResult(node,boost::posix_time::second_clock::local_time(),true);

	}

	return 0;
}

int CCentralizedDA::OutPutYk()
{
	for (vector<share_switchnode_ptr>::iterator it = ykOpenSet_.begin();it != ykOpenSet_.end();it++) //�������
	{
		//(*it)->WriteOutputYx(OutYx_FA_Act,1,true);
		int ret = OpenSwitch(*it);
		if(ret)
		{
			return ret;
		}
        
		boost::thread::sleep(boost::get_system_time() + boost::posix_time::milliseconds(10));
	} 

	for (vector<share_switchnode_ptr>::iterator it = ykCloseSet_.begin();it != ykCloseSet_.end();it++) //�ָ�����
	{
		//(*it)->WriteOutputYx(OutYx_FA_Act,1,true);
		int ret = CloseSwitch(*it);
		if (ret)
		{
			return ret;
		}

		boost::thread::sleep(boost::get_system_time() + boost::posix_time::milliseconds(10));
	}

	for (vector<share_outletswitch_ptr>::iterator it = outLetswitchSet_.begin();it != outLetswitchSet_.end();it++) //�ָ����߿���
	{
		//(*it)->WriteOutputYx(OutYx_FA_Act,1,true);
		int ret = CloseOutlet(*it);
		if (ret)
		{
			return ret;
		}

		boost::thread::sleep(boost::get_system_time() + boost::posix_time::milliseconds(10));
	}

	return 0;
}

int CCentralizedDA::WaitYkResult(share_outletswitch_ptr node,boost::posix_time::ptime begin_time,bool excepted_swichposition)
{
	boost::posix_time::ptime now_time = boost::posix_time::second_clock::local_time();
	if((now_time >= begin_time) && (now_time - begin_time <= boost::posix_time::seconds(wait_yxcon_timeout)))
	{
		boost::unique_lock<boost::mutex> uLock(yxMutex_,boost::try_to_lock);
		if (uLock.owns_lock())
		{
			boost::posix_time::time_duration wait_time = boost::posix_time::seconds(wait_yxcon_timeout) - (now_time - begin_time);

            boost::thread::sleep(boost::get_system_time() + boost::posix_time::milliseconds(5));//10*500

			if(yxCond_.timed_wait(uLock,boost::get_system_time() + wait_time))
			{
				uLock.unlock();

				if (CheckInput())
				{
					std::ostringstream ostr;
					ostr<<"���뿪��"<<node->getSwitchID()<<"�����У���⵽����ң���쳣���˳�����DA"<<std::endl;
					AddLog(ostr.str());

					return -1;
				}

				if (CheckNewFaultNode())
				{
					std::ostringstream ostr;
					ostr<<"���뿪��"<<node->getSwitchID()<<"�����У���⵽�µĹ��ϣ��˳�����DA"<<std::endl;
					AddLog(ostr.str());

					FaMulFault(true);

					return -1;
				}


				if ((node)->getSwitchPosition() == excepted_swichposition)
				{
					std::ostringstream ostr;
					ostr<<"���뿪��"<<node->getSwitchID()<<"���յ�����λ��ң�ű�λ��������ң����ɣ�����DA"<<std::endl;
					AddLog(ostr.str());

					//(node)->WriteOutputYx(OutYx_FA_Over,1,true);
					node->SwitchActOver();

					return 0;
				}
				else
				{
					return WaitYkResult(node,begin_time,excepted_swichposition);
				}
			}
			else
			{
				std::ostringstream ostr;
				ostr<<"���뿪��"<<node->getSwitchID()<<"�󣬵ȴ���ʱδ���յ�����λ��ң�ű�λ���˳�����DA"<<std::endl;
				AddLog(ostr.str());
                
				//bSwitchRefuseSign_=true;
				//(node)->WriteOutputYx(OutYx_FA_Refuse,1,true);//�ܶ�
				node->SwitchActError(); //��

				return -1;
			}
		}
		else
		{
			std::ostringstream ostr;
			ostr<<"���뿪��"<<node->getSwitchID()<<"����ȡ��DA�߳���Ȩ�ޣ��˳�����DA"<<std::endl;
			AddLog(ostr.str());

			return -1;
		}
	}
	else
	{
		std::ostringstream ostr;
		ostr<<"���뿪��"<<node->getSwitchID()<<"��ʱ��δ���յ�����λ��ң�ű�λ���˳�����DA"<<std::endl;
		AddLog(ostr.str());

		//bSwitchRefuseSign_=true;
		//(node)->WriteOutputYx(OutYx_FA_Refuse,1,true);
		node->SwitchActError(); //��

		return -1;
	}
}

void CCentralizedDA::ProcessAlgorithmSig(typeCmd cmdType,unsigned char ReturnCode,share_commpoint_ptr point,boost::any val)
{
	switch(cmdType)
	{
	case Protocol::COS_DATA_UP:
		{
			if (ReturnCode == RETURN_CODE_ACTIVE || ReturnCode == RETURN_CODE_CMDSEND)
			{
				boost::unique_lock<boost::mutex> uLock(yxMutex_,boost::try_to_lock);
				if (uLock.owns_lock())
				{
					yxCond_.notify_one();
				}
			}
		}
		break;

	case Protocol::SOE_DATA_UP:
		{
			if (ReturnCode == RETURN_CODE_ACTIVE || ReturnCode == RETURN_CODE_CMDSEND)
			{
				boost::unique_lock<boost::mutex> uLock(yxMutex_,boost::try_to_lock);
				if (uLock.owns_lock())
				{
					yxCond_.notify_one();
				}
			}
		}
		break;

	default:
		break;
	}
}

void CCentralizedDA::ResetCfg()
{
	if (bReset_)
	{
		bReset_ = false;
		
		InitAlgorithm();
	}
}

void CCentralizedDA::thread_fun_da()
{
	boost::thread::sleep(boost::get_system_time() + boost::posix_time::milliseconds(2000));

    FaAct(true);

	while(thread_run_)
	{
		boost::thread::sleep(boost::get_system_time() + boost::posix_time::milliseconds(4000));

		if (!CheckInput())
		{
			try
			{
				ResetCfg();

				BuildForest();

				CheckFault();

				OutPutYk();
			}
			catch(PublicSupport::dat2def_exception & e)
			{
				std::string const * strPtr = boost::get_error_info<boost::errinfo_type_info_name>(e);
				if (strPtr)
				{
					std::ostringstream ostr;

					std::string const * strPtrMiddle = boost::get_error_info<errinfo_middletype_name>(e);
					if (strPtrMiddle)
					{
						ostr<<" item info:"<<(*strPtrMiddle);
					}

					int const * indexPtr = boost::get_error_info<boost::errinfo_errno>(e);
					if (indexPtr)
					{
						ostr<<" data no:"<<(*indexPtr)<<"��";
					}

					ostr<<"error info:"<<(*strPtr)<<"��";
					ostr<< "DA����ʧ�ܣ���������"<<std::endl;

					AddLog(ostr.str());
				}
			}
		}
		else
		{
			std::ostringstream ostr;
			ostr<<"����ң�ż��ʧ�ܣ���ֹ����DA"<<std::endl;
			AddLog(ostr.str());
		}

		FaMulFault(false);

		boost::unique_lock<boost::mutex> uLock(yxMutex_);

		yxCond_.wait(uLock);

		uLock.unlock();
	}

	FaAct(false);
}

void CCentralizedDA::FaAct(bool act)
{
	if (act)
	{
		WriteOutputYx(OutYx_FA_Enable,1,true);
	}
	else
	{
		WriteOutputYx(OutYx_FA_Enable,0,true);
	}
}

void CCentralizedDA::FaMulFault(bool act)
{
	if (act)
	{
		WriteOutputYx(OutYx_FA_MulFault,1,true);
	}
	else
	{
		WriteOutputYx(OutYx_FA_MulFault,0,true);
	}
}

int CCentralizedDA::start(CmdRecallSignalType & sig)
{
	ConnectSubAlgorithmSig(sig);

	thread_run_ = true;
	boost::thread thrd(boost::bind(&CCentralizedDA::thread_fun_da,this));

	return 0;
}

void CCentralizedDA::stop()
{
	DisconnectSubAlgorithmSig();

	thread_run_ = false;
	yxCond_.notify_one();
}

bool CCentralizedDA::CheckInputYxSet()
{
	for (std::vector<stIn>::iterator it = input_set_.begin(); it != input_set_.end(); it++)
	{
		size_t yxindex = (*it).index_;
		bool bPolar = (*it).polar_;

		bool bSingleYx = (da_op_.getYxType(yxindex) == DataBase::single_yx_point);
		typeYxval yxval = da_op_.getYxVal(yxindex);
		bool bRet = false;

		if (bSingleYx)
		{
			if (bPolar)
			{
				bRet = (yxval == 0x00);
			}
			else
			{
				bRet = (yxval == 0x01);
			}
		}
		else
		{
			if (bPolar)
			{
				bRet = (yxval == 0x01);
			}
			else
			{
				bRet = (yxval == 0x10);
			}
		}

		if (!bRet && (*it).out_ > 0)
		{
			WriteInputYx(*it,1,true);

			return bRet;
		}
	}

	return true;
}

int CCentralizedDA::WriteInputYx(stIn st,typeYxval val ,bool bSingleType)
{
	if (st.out_ > 0)
	{
		if (!st.polar_)
		{
			if (bSingleType)
			{
				val = ((~val) & 0x01);
			}
			else
			{
				val = ((~val) & 0x03);
			}
		}
		
		da_op_.TrigCosEvent(st.out_,val,bSingleType);

		return 0;
	}

	return -1;
}

int CCentralizedDA::WriteOutputYx(int outputindex,typeYxval val,bool bSingleType /* = true*/)
{
	if (outputindex < 0 || outputindex >= (int)output_set_.size())
	{
		return -1;
	}

	if (!output_set_[outputindex].polar_)
	{
		if (bSingleType)
		{
			val = ((~val) & 0x01);
		}
		else
		{
			val = ((~val) & 0x03);
		}
	}

	da_op_.TrigCosEvent(output_set_[outputindex].index_,val,bSingleType);

	return 0;
}

int CCentralizedDA::CheckInput()
{
	if (!CheckInputYxSet())
	{
		return -1;
	}

	for(vector<share_switchnode_ptr>::iterator it = OriSwitchNodes_.begin(); it != OriSwitchNodes_.end(); it++)
	{
		if (!(*it)->CheckInputYxSet())
		{
			return -1;
		}
	}

	return 0;
}

bool CCentralizedDA::CheckFaultAlreadyExist(share_switchnode_ptr node)
{
	for (vector<share_switchnode_ptr>::iterator it = faultSet_.begin(); it != faultSet_.end();it++)
	{
		if ((*it) == node)
		{
			return true;
		}
	}

	return false;
}

bool CCentralizedDA::CheckNewFaultNode()
{
	for (vector<share_switchnode_ptr>::iterator it = OriSwitchNodes_.begin();it != OriSwitchNodes_.end();it++)
	{
		if ((*it)->getSwitchProtected())
		{
			if (!CheckFaultAlreadyExist(*it))
			{
				return true;
			}
		}
	}

	return false;
}

//bool CAlgorithmDA::CheckFaultSwitch(bool excepted_swichposition)
//{
//	for (vector<share_switchnode_ptr>::iterator it = OriSwitchNodes_.begin();it != OriSwitchNodes_.end();it++)
//	{
//		if ((*it)->getSwitchPosition()==excepted_swichposition)
//		{
//				return true;
//		}
//	}
//
//	return false;
//}



//bool CAlgorithmDA::ResetOutputYx(typeYxval val,bool bSingleType)
//{
//	int count = 0;
////	for (std::vector<stInOut>::iterator it = output_set_.begin(); it != output_set_.end(); it++)
//	{
//		if (!output_set_[1].polar_)
//		{
//			if (bSingleType)
//			{
//				val = ((~val) & 0x01);
//			}
//			else
//			{
//				val = ((~val) & 0x03);
//			}
//		}
//		int ret = da_op_.SaveOriYxVal(output_set_[1].index_,val,true/*terminalPtr->getInitCommPointFlag()*/);
//		if (ret == 5)
//		{
//			count++;
//		}
//		if (count > 0)
//		{
//			da_op_.TrigCosEvent(output_set_[1].index_,val,bSingleType);
//		}
//
//	}
//
//	return true;
//}
//
//void CAlgorithmDA::SignReset()
//{
//	ResetOutputYx(0,true);
//
//	for(vector<share_switchnode_ptr>::iterator it = OriSwitchNodes_.begin(); it != OriSwitchNodes_.end(); it++)
//	{
//		(*it)->ResetOutputYx(0,true);
//	}
//}
//
//void CAlgorithmDA::GetLoad()
//{
//    size_t value1=da_op_.getYcVal(YcOverLoadI_);
//	size_t value2=da_op_.getYcVal(YcOverLoadII_);
//	if((value1<=5000)&&((value1>=100)))
//	{
//       YcOverLoadValue1_=value1;
//	}
//	if((value2<=5000)&&((value2>=100)))
//	{
//		YcOverLoadValue2_=value2;
//	}
//	for(vector<share_switchnode_ptr>::iterator it = OriSwitchNodes_.begin(); it != OriSwitchNodes_.end(); it++)
//	{
//		(*it)->GetLoad();
//	}
//	
//}
//
//void CAlgorithmDA::handle_timerSignReset(const boost::system::error_code& error)
//{
//	if(!error)
//	{
//		SignReset();
//	}
//
//}
//void CAlgorithmDA::handle_timerGetLoad(const boost::system::error_code& error)
//{
//	if(!error)
//	{
//		GetLoad();
//		ResetTimerGetLoad(true,5);
//	}
//
//}
//
//void CAlgorithmDA::ResetTimerSignReset(bool bContinue,unsigned short timeVal)
//{
//	if (bContinue)
//	{
//		TimerSignReset_.expires_from_now(boost::posix_time::seconds(timeVal));
//		TimerSignReset_.async_wait(boost::bind(&CAlgorithmDA::handle_timerSignReset,this,boost::asio::placeholders::error));
//	}
//	else
//	{
//		TimerSignReset_.cancel();
//	}
//}
//
//void CAlgorithmDA::ResetTimerGetLoad(bool bContinue,unsigned short timeVal)
//{
//	if (bContinue)
//	{
//		TimerGetLoad_.expires_from_now(boost::posix_time::seconds(timeVal));
//		TimerGetLoad_.async_wait(boost::bind(&CAlgorithmDA::handle_timerGetLoad,this,boost::asio::placeholders::error));
//	}
//	else
//	{
//		TimerGetLoad_.cancel();
//	}
//}
//
//int CAlgorithmDA::CheckLoadOver()
//{
//	//std::cout<<"load"<<YcOverLoadValue1_<<","<<GetSwitchLoad()<<","<<YcOverLoadValue2_<<","<<OverLoadValue_<<","<<std::endl;
//    
//	int value ;
////		if(YcOverLoadValue1_ <= GetSwitchLoad()) value =YcOverLoadValue2_;
////		else value=(YcOverLoadValue1_-GetSwitchLoad())+YcOverLoadValue2_;
//	value=(GetSwitchLoad())+YcOverLoadValue2_;
//	std::cout<<"value"<<value<<std::endl;
//	if(value >= OverLoadValue_)
//	{
//		return 1;
//	}
//	
//	return 0;
//}
//
//int CAlgorithmDA::GetSwitchLoad()
//{
////	vector<share_switchnode_ptr>::iterator it = ykOpenSet_.end(); //�������
//		return ykOpenSet_[1]->GetSwitchLoad();//(*it)
//}
//
//bool CAlgorithmDA::CheckFASwitchDisable()
//{
////	std::vector<stInOut>::iterator it = input_set_.begin();
//	{
//		size_t yxindex = CheckFADisable_;
//		//bool bPolar = input_set_[0].polar_;
//
//		bool bSingleYx = (da_op_.getYxType(yxindex) == DataBase::single_yx_point);
//		typeYxval yxval = da_op_.getYxVal(yxindex);
//
//
//		bool bRet = false;
//			/*if (bPolar)*/
//			{
//	//			bRet = (yxval == 0x00);
//			}
//			//else
//			{
//				bRet = (yxval == 0x01);
//			}
//
//		if (!bRet)
//		{
//			std::cout<<"Ӳѹ���FA���ܿ����˳�"<<std::endl;
//			return true;
//		}
//	}
//	return false;
//}

}; //namespace FeederAutomation 

