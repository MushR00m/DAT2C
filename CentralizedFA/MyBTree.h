#pragma once

#ifndef MYBTREE_H
#define MYBTREE_H

#include <iostream>

/**********************************************************************************************************
������ʾɭ�ֵĶ������࣬����Ů�����ֵ�
**********************************************************************************************************/
namespace CentralizedFA {

template <class ElemType> class CMyBTree;

template <class ElemType> class CMyBTreeNode
{
	friend class CMyBTree<ElemType>;
private:
	ElemType data;
	CMyBTreeNode<ElemType> *firstChild, *nextSibling, *link; 			//���ӣ����ֵܣ�����֦���ӵĽ��
	CMyBTreeNode(ElemType value=0, CMyBTreeNode<ElemType> *fc=NULL, CMyBTreeNode<ElemType> *ns=NULL, CMyBTreeNode<ElemType> *lk=NULL)
		:data(value), firstChild(fc), nextSibling(ns), link(lk){}
};

template <class ElemType> class CMyBTree
{
public:
	CMyBTree()
	{
		root=current=NULL;
	}

	bool Root();           																				//currentָ������
	bool FirstChild();     																				//currentָ������
	bool NextSibling();    																				//currentָ�����ֵ�
	void BuildRoot(ElemType rootVal);  																		//��������
	void InsertChild(ElemType value);  																		//������Ů���
	bool Find(ElemType target);        																		//����Ŀ��ֵ�ҽ��
	bool Parent();                 																		//currentָ�򸸽��
	bool IsRoot();                 																		//�жϵ�ǰ����Ƿ�λ�����
	void RemovesubTree();          																		//ɾ���Ե�ǰ���Ϊ��������
	void SetLink(CMyBTreeNode<ElemType> * SrcLink,CMyBTreeNode<ElemType> * DesLink);  //��������֦
	ElemType GetCurrentData();         																		//��õ�ǰ����ֵ
	ElemType GetLinkData();            																		//��õ�ǰ�������֦���ӽ���ֵ
	CMyBTreeNode<ElemType> * GetCurrentPointer();  															//���ָ��ǰ����ָ��
	void SetCurrentPointer(CMyBTreeNode<ElemType> * target);  												//��current���ָ��Ŀ����
	bool IsEmpty()                  																	//�жϵ�ǰ����Ƿ�λΪ��
	{
		return current==NULL;
	}
	bool IsLeaf()                   																	//�жϵ�ǰ����Ƿ�Ϊ��Ҷ
	{
		return current->firstChild==NULL;
	}
	bool HasLinkNode()              																	//�жϵ�ǰ����Ƿ�������֦
	{
		return current->link!=NULL;
	}

private:
	CMyBTreeNode<ElemType> *root, *current;
	bool Find( CMyBTreeNode<ElemType> *p, ElemType target);
	void RemovesubTree( CMyBTreeNode<ElemType> *p);
	bool FindParent( CMyBTreeNode<ElemType> *t, CMyBTreeNode<ElemType> *p);
};

//��������
template <class ElemType> void CMyBTree<ElemType>::BuildRoot(ElemType rootVal)
{
	root=current=new CMyBTreeNode<ElemType>(rootVal);
}

//�жϵ�ǰ����Ƿ�λ�����
template <class ElemType> bool CMyBTree<ElemType>::IsRoot()
{
	if(root==NULL)
		return false;
	else
		return current==root;
}

//currentָ������
template <class ElemType> bool CMyBTree<ElemType>::Root()                        
{
	if(root==NULL)
	{
		current = NULL;
		return false;
	}
	else
	{
		current=root;
		return true;
	}
}

//��������֦
template <class ElemType> void CMyBTree<ElemType>::SetLink(CMyBTreeNode<ElemType> * SrcLink,CMyBTreeNode<ElemType> * DesLink)
{
	SrcLink->link=DesLink;
	DesLink->link=SrcLink;
	//printf("SrcLink=%d\n",SrcLink->data);
	//printf("DesLink=%d\n",DesLink->data);
}

//��õ�ǰ����ֵ
template <class ElemType>	ElemType CMyBTree<ElemType>::GetCurrentData()
{
	if(!IsEmpty())
	{
		return current->data;
	}

	return ElemType();
}

//��õ�ǰ�������֦���ӽ���ֵ
template <class ElemType> ElemType CMyBTree<ElemType>::GetLinkData()
{
	if(HasLinkNode())
	{
		return current->link->data;
	}

	return ElemType();
}

//���ָ��ǰ����ָ��
template <class ElemType>	CMyBTreeNode<ElemType> * CMyBTree<ElemType>::GetCurrentPointer()
{
	return current;
}

//��current���ָ��Ŀ����
template <class ElemType>	void CMyBTree<ElemType>::SetCurrentPointer(CMyBTreeNode<ElemType> * target)
{
	current=target;
}

//currentָ������
template <class ElemType> bool CMyBTree<ElemType>::FirstChild()
{
	if(current!=NULL&&current->firstChild!=NULL)
	{
		current=current->firstChild;
		return true;
	}
	current=NULL;
	return false;
}

//currentָ�����ֵ�
template <class ElemType> bool CMyBTree<ElemType>::NextSibling()
{
	if(current!=NULL&&current->nextSibling!=NULL)
	{
		current = current -> nextSibling;
		return true;
	}
	current=NULL;
	return false;
}

//������Ů���
template <class ElemType> void CMyBTree<ElemType>::InsertChild(ElemType value)
{
	CMyBTreeNode<ElemType> * newNode =new CMyBTreeNode<ElemType>(value);
	newNode->data=value;
	if(current->firstChild==NULL)
	{
		current->firstChild=newNode;
		//printf("Current firstChild data=%d\n",current->firstChild->data);
	}
	else
	{
		CMyBTreeNode<ElemType> *p =current->firstChild;
		while(p->nextSibling != NULL)
		{
			p=p->nextSibling;
		}
		p->nextSibling=newNode;
		//printf("firstChild nextSibling data=%d\n",newNode->data);
	}

}

//����Ŀ��ֵ�ҽ��
template <class ElemType>	bool CMyBTree<ElemType>::Find( CMyBTreeNode<ElemType> *p, ElemType target)
{
	bool result=false;
	if(p->data==target)
	{
		result=true;
		current=p;
		//printf("Find success=%d\n",current->data);
	}
	else
	{
		CMyBTreeNode<ElemType> *q = p->firstChild;
		while(q!=NULL&&!(result = Find(q,target)))
		{
			q = q->nextSibling;
			//printf("Finding\n");
		}
	}
	return result;
}

//����Ŀ��ֵ�ҽ��
template <class ElemType> bool CMyBTree<ElemType>::Find(ElemType target)
{
	if(root==NULL)
	{
		return false;
	}
	return Find(root,target);
}

//Parent�������õ��ڲ�����
template <class ElemType> bool CMyBTree<ElemType>::FindParent( CMyBTreeNode<ElemType> *t, CMyBTreeNode<ElemType> *p)
{
	CMyBTreeNode<ElemType> *q = t->firstChild;
	while(q!=NULL&&q!=p)
	{
		bool ret=FindParent(q,p);
		if(ret)
		{
			return ret;
		}
		q = q->nextSibling;
	}
	if(q!=NULL && q==p)
	{
		current=t;
		return true;
	}
	else
		return false;
}

//currentָ�򸸽��
template <class ElemType> bool CMyBTree<ElemType>::Parent()
{
	CMyBTreeNode<ElemType> * p =current, *t;
	if(current == NULL||current == root)
	{
		current = NULL;
		return false;
	}
	t = root;
	bool ret = FindParent(t,p);
	return ret;
}

//ɾ���Ե�ǰ���Ϊ��������
template <class ElemType> void CMyBTree<ElemType>::RemovesubTree( CMyBTreeNode<ElemType> *p)
{
	CMyBTreeNode<ElemType> * q=p->firstChild, *next;
	while(q!=NULL)
	{
		next=q->nextSibling;
		RemovesubTree(q);
		q=next;
	}
	delete p;
}

//ɾ���Ե�ǰ���Ϊ��������
template <class ElemType> void CMyBTree<ElemType>::RemovesubTree()
{
	if(current!=NULL)
	{
		if(current==root)
		{
			root=NULL;
		}
		RemovesubTree(current);
		current=NULL;
	}
}

}; //namespace FeederAutomation

#endif //#ifndef MYBTREE_H

