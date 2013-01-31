#include "SelfNode.h"
#include "LinkNode.h"
#include "LoadSwtich.h"

namespace DistributedFA {

CSelfNode::CSelfNode(DataBase::CDAOperate & da_op)
{
	ChangeNodeType(new CLoadSwtich(da_op));
}

CSelfNode::~CSelfNode(void)
{
}

bool CSelfNode::CheckTieSwitch()
{
	if (powerNode_ && loadNode_) //��Դ��͸��ɲ඼���ڽӵ�
	{
		if ((powerNode_->getPosition() == PositionClose) && (getPosition() == PositionOpen) && (loadNode_->getPosition() == PositionClose)) //�����߿���λ�ú��Լ��Ŀ���λ��3���ж��Լ��Ƿ�Ϊ���翪��
		{
			return true;
		}
	}
	else if (powerNode_ && !loadNode_) //��Դ�����ڽӵ�����ɲ�û�У�����������·β�˽��
	{
		if ((powerNode_->getPosition() == PositionClose) && (getPosition() == PositionOpen)) //�����߿���λ�ú��Լ��Ŀ���λ��2���ж��Լ��Ƿ�Ϊ���翪��
		{
			return true;
		}
	}
	else if (loadNode_ && !powerNode_) //���ɲ����ڽӵ����Դ��û�У�����������·ͷ�˽��
	{
		if ((getPosition() == PositionOpen) && (loadNode_->getPosition() == PositionClose)) //�����߿���λ�ú��Լ��Ŀ���λ��2���ж��Լ��Ƿ�Ϊ���翪��
		{
			return true;
		}
	}

	return false;
}

bool CSelfNode::CheckSelfFault()
{
	if (powerNode_ && loadNode_) //��Դ��͸��ɲ඼���ڽӵ�
	{
		if ((powerNode_->getPosition() == PositionClose) && (getPosition() == PositionClose) && (loadNode_->getPosition() == PositionClose)) //����λ�� 1 1 1
		{
			if ((powerNode_->getProtect() == ProtectedPositive) && (getProtect() == ProtectedPositive) && (loadNode_->getProtect() == ProtectedNagetive))     //�����ź� 1 1 0
			{
				return true;
			}
			else if((powerNode_->getProtect() == ProtectedNagetive) && (getProtect() == ProtectedPositive) && (loadNode_->getProtect() == ProtectedPositive)) //�����ź� 0 1 1
			{
				return true;
			}
			else if((powerNode_->getProtect() == ProtectedPositive) && (getProtect() == ProtectedNagetive) && (loadNode_->getProtect() == ProtectedNagetive)) //�����ź� 1 0 0
			{
				return true;
			}
			else if((powerNode_->getProtect() == ProtectedNagetive) && (getProtect() == ProtectedNagetive) && (loadNode_->getProtect() == ProtectedPositive)) //�����ź� 0 0 1
			{
				return true;
			}
		}
		else if ((powerNode_->getPosition() == PositionOpen) && (getPosition() == PositionClose) && (loadNode_->getPosition() == PositionClose)) //����λ�� 0 1 1
		{
			if((powerNode_->getProtect() == ProtectedNagetive) && (getProtect() == ProtectedPositive) && (loadNode_->getProtect() == ProtectedPositive))      //�����ź� 0 1 1
			{
				return true;
			}
			else if((powerNode_->getProtect() == ProtectedNagetive) && (getProtect() == ProtectedNagetive) && (loadNode_->getProtect() == ProtectedPositive)) //�����ź� 0 0 1
			{
				return true;
			}
		}
		else if((powerNode_->getPosition() == PositionClose) && (getPosition() == PositionClose) && (loadNode_->getPosition() == PositionOpen)) //����λ�� 1 1 0
		{
			if ((powerNode_->getProtect() == ProtectedPositive) && (getProtect() == ProtectedPositive) && (loadNode_->getProtect() == ProtectedNagetive))     //�����ź� 1 1 0
			{
				return true;
			}
			else if((powerNode_->getProtect() == ProtectedPositive) && (getProtect() == ProtectedNagetive) && (loadNode_->getProtect() == ProtectedNagetive)) //�����ź� 1 0 0
			{
				return true;
			}
		}
	}
	else if (powerNode_ && !loadNode_) //��Դ�����ڽӵ�����ɲ�û�У�����������·β�˽��
	{
		if ((powerNode_->getPosition() == PositionClose) && (getPosition() == PositionClose)) //����λ�� 1 1
		{
			if ((powerNode_->getProtect() == ProtectedPositive) && (getProtect() == ProtectedPositive))      //�����ź� 1 1
			{
				return true;
			}
			else if ((powerNode_->getProtect() == ProtectedPositive) && (getProtect() == ProtectedNagetive)) //�����ź� 1 0
			{
				return true;
			}
		}
	}
	else if (loadNode_ && !powerNode_) //���ɲ����ڽӵ����Դ��û�У�����������·ͷ�˽��
	{
		if ((getPosition() == PositionClose) && loadNode_->getPosition() == PositionClose) //����λ�� 1 1
		{
			if (getProtect() == ProtectedPositive && loadNode_->getProtect() == ProtectedNagetive)          //�����ź� 1 0
			{
				return true;
			}
		}
		else if ((getPosition() == PositionClose) && loadNode_->getPosition() == PositionOpen) //����λ�� 1 0
		{
			if (getProtect() == ProtectedPositive && loadNode_->getProtect() == ProtectedNagetive)          //�����ź� 1 0
			{
				return true;
			}
		}
	}

	return false;
}

int CSelfNode::getPosition()
{
	if (selfNodeType_)
	{
		return selfNodeType_->getPosition();
	}

	return -1;
}

int CSelfNode::getProtect()
{
	if (selfNodeType_)
	{
		return selfNodeType_->getProtect();
	}

	return -1;
}

int CSelfNode::OpenSwitchSel()
{
	if (selfNodeType_)
	{
		return selfNodeType_->OpenSwitchSel();
	}

	return -1;
}

int CSelfNode::OpenSwitchExe()
{
	if (selfNodeType_)
	{
		return selfNodeType_->OpenSwitchExe();
	}

	return -1;
}

int CSelfNode::CloseSwitchSel()
{
	if (selfNodeType_)
	{
		return selfNodeType_->CloseSwitchSel();
	}
	
	return -1;
}

int CSelfNode::CloseSwitchExe()
{
	if (selfNodeType_)
	{
		return selfNodeType_->CloseSwitchExe();
	}

	return -1;
}

void CSelfNode::ChangeNodeType(COperateNode * nodeType)
{
	selfNodeType_.reset(nodeType);
}

}; //namespace DistributedFA
