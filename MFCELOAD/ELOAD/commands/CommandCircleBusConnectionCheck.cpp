#include "StdAfx.h"
#include "..\ELoadDocData.h"
#include "CommandCircleBusConnectionCheck.h"

#include <queue>
using namespace COMMAND;

CCommandCircleBusConnectionCheck::CCommandCircleBusConnectionCheck(void)
{
}

CCommandCircleBusConnectionCheck::~CCommandCircleBusConnectionCheck(void)
{
	try
	{
		
	}
	catch(...)
	{
	}
}

/**
	@brief	bus들이 순환 연결이 되는지 검사한다.

	@author BHK	

	@date 2009-04-06 오전 11:19:41	

	@param	

	@return	ERROR_SUCCESS = 순환 연결이 되지 않으면. 그렇지 않으면 ERROR_BAD_ENVIRONMENT	
*/
int CCommandCircleBusConnectionCheck::Execute()
{
	list<_BusNode*> nodeEntry;
	Prepare(nodeEntry);

	list<_BusNode*> visitedNodeEntry;
	for(list<_BusNode*>::iterator itr = nodeEntry.begin();itr != nodeEntry.end();++itr)
	{
		visitedNodeEntry.clear();

		queue<_BusNode*> nodeQueue;
		nodeQueue.push(*itr);
		visitedNodeEntry.push_back(*itr);
		while(!nodeQueue.empty())
		{
			_BusNode* pNode = nodeQueue.front(); nodeQueue.pop();
			for(list<_link>::iterator jtr = pNode->_linkEntry.begin();jtr != pNode->_linkEntry.end();++jtr)
			{
				_BusNode* pTo = jtr->_pTo;
				if((pNode == jtr->_pFrom) && (NULL != pTo))
				{
					list<_BusNode*>::iterator where = find(visitedNodeEntry.begin() , visitedNodeEntry.end() , pTo);
					if(where != visitedNodeEntry.end())
					{
						for(list<_BusNode*>::iterator itr = nodeEntry.begin();itr != nodeEntry.end();++itr) SAFE_DELETE(*itr);
						return ERROR_BAD_ENVIRONMENT;
					}
					nodeQueue.push(pTo);
					visitedNodeEntry.push_back(pTo);
				}
			}
		}
	}

	for(list<_BusNode*>::iterator itr = nodeEntry.begin();itr != nodeEntry.end();++itr) SAFE_DELETE(*itr);
	return ERROR_SUCCESS;
}

struct MatchNodeName : public binary_function<CCommandCircleBusConnectionCheck::_BusNode*,string,bool>
{
	bool operator ()(CCommandCircleBusConnectionCheck::_BusNode* lhs , const string& rhs) const
	{
		return (lhs->rName == rhs);
	}
};

/**
	@brief	bus에 대당하는 node들을 생성하고 각 node들을 link로 연결한다.

	@author BHK	

	@date 2009-04-06 오전 11:31:31	

	@param	

	@return		
*/
int COMMAND::CCommandCircleBusConnectionCheck::Prepare(list<_BusNode*>& nodeEntry)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> BusItemList;
	docData.GetELoadItemListOf(BusItemList , CBusItem::TypeString());
	//if(pItemList)
	{
		for(list<CELoadItem*>::iterator itr = BusItemList.begin();itr != BusItemList.end();++itr)
		{
			if((*itr)->IsDeleted()) continue;

			_BusNode* pNode = new _BusNode;
			if(pNode)
			{
				pNode->rName = (*itr)->GetName();
				nodeEntry.push_back(pNode);
			}
		}

		for(list<_BusNode*>::iterator jtr = nodeEntry.begin();jtr != nodeEntry.end();++jtr)
		{
			CItemSelectionSet SelectionSet;
			docData.GetLoadItemSetInBus(SelectionSet , (*jtr)->rName);
			
			for(int i = 0;i < SelectionSet.GetItemCount();++i)
			{
				CELoadItem* pELoadItem = SelectionSet.GetItemAt(i);
				const string rConnectedBusID = pELoadItem->prop()->GetValue(_T("Panel") , _T("Connect Bus"));
				if(!rConnectedBusID.empty())
				{
					list<_BusNode*>::iterator where = find_if(nodeEntry.begin() , nodeEntry.end() , bind2nd(MatchNodeName(),rConnectedBusID));
					if(where != nodeEntry.end())
					{
						_link link;
						link._pFrom = (*jtr);
						link._pTo   = (*where);
						(*jtr)->_linkEntry.push_back(link);
					}
				}
			}
		}
	}

	return ERROR_SUCCESS;
}
