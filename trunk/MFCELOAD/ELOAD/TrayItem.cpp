#include "StdAfx.h"
#include "TrayItem.h"

#include "EloadDocData.h"

using namespace ITEMS;


IMPLEMENTS_ELOAD_ITEM(CTrayItem , CELoadItem , "TRAY")

CTrayItem::CTrayItem(void) : CELoadItem()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CELoadItemProp* pItemProp = docData.m_ItemPropMap[CTrayItem::TypeString()];
	if(pItemProp)
	{
		m_pProp = pItemProp->Clone();
		m_pProp->SetOwner(this);

		//prop()->SetOriginalValue(_T("General") , _T("Size") , _T("900"));
		//prop()->SetValue(_T("General") , _T("Size") , _T("900"));
	}
}

CTrayItem::~CTrayItem(void)
{
	try
	{
		if(NULL != m_pProp)    SAFE_DELETE(m_pProp);

		//if(m_pRouteItem != NULL) SAFE_DELETE(m_pRouteItem);

		m_CableEntry.clear();
	}
	catch(...)
	{
	}
}

/**
	@brief	TABLE NAME

	@author KHS	

	@date 2009-05-19 오후 5:18:38	

	@param	

	@return		
**/
string ITEMS::CTrayItem::TableName(void)
{
	static const string __TABLE_NAME__ = _T("T_TRAY_ITEM");

	return __TABLE_NAME__;
}

/**
	@brief	

	@author KHS	

	@date 2009-05-19 오후 5:19:31	

	@param	

	@return		
**/
string ITEMS::CTrayItem::GetName(void)
{
	CELoadItemProp* pProp = prop();
	if(pProp) return pProp->GetValue("General" , "Tray ID");

	return string("");
}

/**
	@brief	

	@author KHS	

	@date 2009-05-19 오후 5:19:30	

	@param	

	@return		
**/
int ITEMS::CTrayItem::SetName(const string& rName)
{
	CELoadItemProp* pProp = prop();
	if(pProp) return pProp->SetValue("General", "Tray ID" , rName);

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author KHS	

	@date 2009-05-19 오후 5:41:11	

	@param	

	@return		
**/
void ITEMS::CTrayItem::SetRouteItemPtr(CRouteItem* pRouteItem)
{
	m_pRouteItem = pRouteItem;
}

/**
	@brief	

	@author KHS	

	@date 2009-05-19 오후 5:44:38	

	@param	

	@return		
**/
CRouteItem* ITEMS::CTrayItem::GetRouteItemPtr(void) const
{
	return m_pRouteItem;
}

/**
	@brief	

	@author KHS	

	@date 2009-05-19 오후 5:44:40	

	@param	

	@return		
**/
string ITEMS::CTrayItem::GetRouteName(void)
{
	return m_pProp->GetValue(_T("General") , _T("Route ID"));
}

/**
	@brief	ADD CABLE

	@author KHS	

	@date 2009-05-19 오후 7:25:31	

	@param	

	@return		
**/
int ITEMS::CTrayItem::AddCableItem(CCableItem* pCableItem)
{
	ASSERT(pCableItem && "pCableItem is NULL");

	m_CableEntry.push_back(pCableItem);

	return ERROR_SUCCESS;
}

/**
	@brief	REMOVE CABLE

	@author KHS	

	@date 2009-07-09 오후 4:00:46	

	@param	

	@return		
**/
int ITEMS::CTrayItem::RemoveCableItem(CCableItem* pCableItem)
{
	ASSERT(pCableItem && "pCableItem is NULL");

	if(pCableItem)
	{
		vector<CCableItem*>::iterator where = find(m_CableEntry.begin() , m_CableEntry.end() , pCableItem);
		if(where != m_CableEntry.end()) m_CableEntry.erase(where);
	}

	return ERROR_SUCCESS;
}

