#include "StdAfx.h"
#include "ItemMemStrategy.h"
#include "ELoadDocData.h"

CItemMemStrategy::CItemMemStrategy(void) : m_pStrategyState(NULL)
{
}

CItemMemStrategy::~CItemMemStrategy(void)
{
	m_pStrategyState = NULL;
}

CItemMemStrategy& CItemMemStrategy::GetInstance()
{
	static CItemMemStrategy __instance__;

	return __instance__;
}

int CItemMemStrategy::OnCreate(ITEMS::CELoadItem* pItem)
{
	assert(pItem);

	if(pItem)
	{
		if(m_pStrategyState)
			return m_pStrategyState->OnCreate(pItem);

		CELoadDocData& docData = CELoadDocData::GetInstance();
		list<CELoadItem*>* pItemList = docData.GetELoadItemListPtrOf(pItem->GetTypeString());
		if(pItemList) pItemList->push_back(pItem);
		
		return ERROR_SUCCESS;
	}

        return ERROR_BAD_ENVIRONMENT;
}

int CItemMemStrategy::OnDelete(ITEMS::CELoadItem* pItem)
{
	assert(pItem);

	if(pItem)
	{
		if(m_pStrategyState)
			return m_pStrategyState->OnDelete(pItem);

		CELoadDocData& docData = CELoadDocData::GetInstance();
		list<CELoadItem*>* pItemList = docData.GetELoadItemListPtrOf(pItem->GetTypeString());
		if(pItemList)
		{
			list<CELoadItem*>::iterator where = find(pItemList->begin(),pItemList->end(),pItem);
			if(where != pItemList->end()) pItemList->erase(where);
		}

		return ERROR_SUCCESS;
	}

        return ERROR_BAD_ENVIRONMENT;
}

