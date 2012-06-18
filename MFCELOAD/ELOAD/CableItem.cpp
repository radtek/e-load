#include "StdAfx.h"
#include "CableItem.h"
#include "ELoadDocData.h"

using namespace ITEMS;

IMPLEMENTS_ELOAD_ITEM(CCableItem , CELoadItem , "CABLE")

CCableItem::CCableItem(void) : CELoadItem(), m_pLoadItem(NULL)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CELoadItemProp* pItemProp = docData.m_ItemPropMap[CCableItem::TypeString()];
	if(pItemProp)
	{
		m_pProp = pItemProp->Clone();
		m_pProp->SetOwner(this);
		m_pCurProp = pItemProp->Clone();

		//! Cable의 기본값을 설정한다.
		prop()->SetOriginalValue(_T("General") , _T("Cable Category") , _T("Power Cable"));
		prop()->SetValue(_T("General") , _T("Cable Category") , _T("Power Cable"));
	}
}

CCableItem::~CCableItem(void)
{
	try
	{
		if(NULL != m_pProp)    SAFE_DELETE(m_pProp);
		if(NULL != m_pCurProp) SAFE_DELETE(m_pCurProp);

		for(list<CELoadItemProp*>::iterator itr = m_BelowCalculatedResultList.begin();itr != m_BelowCalculatedResultList.end();++itr)
		{
			SAFE_DELETE(*itr);
		}
		m_BelowCalculatedResultList.clear();

		for(list<CELoadItemProp*>::iterator itr = m_OverCalculatedResultList.begin();itr != m_OverCalculatedResultList.end();++itr)
		{
			SAFE_DELETE(*itr);
		}
		m_OverCalculatedResultList.clear();

		if(NULL != m_pLoadItem)
		{
			//! Load Item에 등록된 자신의 포인터를 삭제한다.
			vector<CELoadItem*>::iterator where = find(m_pLoadItem->m_CableRefEntry.begin() , m_pLoadItem->m_CableRefEntry.end(),this);
			if(where != m_pLoadItem->m_CableRefEntry.end()) m_pLoadItem->m_CableRefEntry.erase(where);
			m_pLoadItem = NULL;

			//! Tray 에 쌓인 Cable을 삭제
			CELoadDocData& docData = CELoadDocData::GetInstance();
			list<CELoadItem*> TrayItemList;
			docData.GetELoadItemListOf(TrayItemList , CTrayItem::TypeString());
			for(list<CELoadItem*>::iterator itr = TrayItemList.begin(); itr != TrayItemList.end();++itr)
			{
				CTrayItem *pTrayItem = static_cast<CTrayItem*>(*itr);
				if(pTrayItem) pTrayItem->RemoveCableItem(this);
			}

			list<CELoadItem*> RouteItemList;
			docData.GetELoadItemListOf(RouteItemList, CRouteItem::TypeString());
			for(list<CELoadItem*>::iterator itr = RouteItemList.begin(); itr != RouteItemList.end();++itr)
			{
				CRouteItem *pRouteItem = static_cast<CRouteItem*>(*itr);
				if(pRouteItem)
				{
					pRouteItem->RemoveHVPowerCableItem(this);
					pRouteItem->RemoveLVPowerCableItem(this);
					pRouteItem->RemoveControlCableItem(this);
				}
			}
		}
	}
	catch(...)
	{
	}
}

/**
	@brief

	@author	BHK
*/
string CCableItem::TableName()
{
	static const string __TABLE_NAME__ = _T("T_CABLE_ITEM");

	return __TABLE_NAME__;
}

void CCableItem::SetLoadItemPtr( CLoadItem* pLoadItem )
{
	m_pLoadItem = pLoadItem;
}

CLoadItem* CCableItem::GetLoadItemPtr() const
{
	return m_pLoadItem;
}

string CCableItem::GetCategoryString()
{
	return CCableItem::TypeString();
}

string CCableItem::GetName() const
{
        CELoadItemProp* pProp = prop();
	if(pProp) return pProp->GetValue("General" , "CABLE ID");

	return string("");
}

int CCableItem::SetName( const string& name )
{
        CELoadItemProp* pProp = prop();
	if(pProp) return pProp->SetValue("General", "CABLE ID" , name);

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author	BHK
*/
int CCableItem::Copy(CELoadItem* pCopied)
{
	assert(pCopied && "pCopied is NULL");
	CELoadItem::Copy(pCopied);

	if(pCopied && pCopied->IsKindOf(CCableItem::TypeString()))
	{
		CCableItem* pCableItem = static_cast<CCableItem*>(pCopied);

		m_pLoadItem = pCableItem->m_pLoadItem;
		m_pCurProp->Copy(pCableItem->m_pCurProp);
		
		for(list<CELoadItemProp*>::iterator itr = m_BelowCalculatedResultList.begin();itr != m_BelowCalculatedResultList.end();++itr)
		{
			SAFE_DELETE(*itr);
		}
		m_BelowCalculatedResultList.clear();
		for(list<CELoadItemProp*>::iterator itr = pCableItem->m_BelowCalculatedResultList.begin();itr != pCableItem->m_BelowCalculatedResultList.end();++itr)
		{
			m_BelowCalculatedResultList.push_back((*itr)->Clone());
		}

		for(list<CELoadItemProp*>::iterator itr = m_OverCalculatedResultList.begin();itr != m_OverCalculatedResultList.end();++itr)
		{
			SAFE_DELETE(*itr);
		}
		m_BelowCalculatedResultList.clear();
		for(list<CELoadItemProp*>::iterator itr = pCableItem->m_OverCalculatedResultList.begin();itr != pCableItem->m_OverCalculatedResultList.end();++itr)
		{
			m_OverCalculatedResultList.push_back((*itr)->Clone());
		}

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	

	@author	BHK
*/
int ITEMS::CCableItem::GetCalculatedResultList(list<CELoadItemProp*>& rhs)
{
	rhs.clear();

	rhs.insert(rhs.begin() , m_BelowCalculatedResultList.begin() , m_BelowCalculatedResultList.end());
	rhs.insert(rhs.end() , m_OverCalculatedResultList.begin() , m_OverCalculatedResultList.end());

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
CELoadItemProp* ITEMS::CCableItem::GetPropForBelowCalculatedResult()
{
	if(m_BelowCalculatedResultList.size() < TRY_CALCULATED_RESULTS)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();

		CELoadItemProp* pItemProp = docData.m_ItemPropMap[CCableItem::TypeString()];
		CELoadItemProp* pProp = pItemProp->Clone();
		m_BelowCalculatedResultList.push_back(pProp);

		return pProp;
	}
	else
	{
		CELoadItemProp* pProp = m_BelowCalculatedResultList.front();
		m_BelowCalculatedResultList.erase(m_BelowCalculatedResultList.begin());
		m_BelowCalculatedResultList.push_back(pProp);

		return pProp;
	}

	return NULL;
}

/**
	@brief	

	@author	BHK
*/
CELoadItemProp* ITEMS::CCableItem::GetPropForOverCalculatedResult()
{
	if(m_OverCalculatedResultList.size() < TRY_CALCULATED_RESULTS)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();

		CELoadItemProp* pItemProp = docData.m_ItemPropMap[CCableItem::TypeString()];
		CELoadItemProp* pProp = pItemProp->Clone();
		m_OverCalculatedResultList.push_back(pProp);

		return pProp;
	}
	else
	{
		CELoadItemProp* pProp = m_OverCalculatedResultList.front();
		m_OverCalculatedResultList.erase(m_OverCalculatedResultList.begin());
		m_OverCalculatedResultList.push_back(pProp);

		return pProp;
	}

	return NULL;
}

/**
	@brief	계산 결과를 저장한 내용을 클리어한다.

	@author	BHK
*/
int ITEMS::CCableItem::ClearCalculatedResult(void)
{
	for(list<CELoadItemProp*>::iterator itr = m_BelowCalculatedResultList.begin();itr != m_BelowCalculatedResultList.end();++itr)
	{
		SAFE_DELETE(*itr);
	}
	m_BelowCalculatedResultList.clear();

	for(list<CELoadItemProp*>::iterator itr = m_OverCalculatedResultList.begin();itr != m_OverCalculatedResultList.end();++itr)
	{
		SAFE_DELETE(*itr);
	}
	m_OverCalculatedResultList.clear();

	return ERROR_SUCCESS;
}
