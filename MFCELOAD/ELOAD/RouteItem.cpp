#include "StdAfx.h"
#include "RouteItem.h"
#include "EloadDocData.h"

using namespace ITEMS;

IMPLEMENTS_ELOAD_ITEM(CRouteItem , CELoadItem , "ROUTE")


class CSort_LengthComp
{
public:
	bool operator()(const CCableItem* lhs , const CCableItem* rhs) const
	{
		const string lLength = lhs->prop()->GetValue(_T("General"), _T("Length"));
		const string rLength = rhs->prop()->GetValue(_T("General"), _T("Length"));

		return atof(lLength.c_str()) > atof(rLength.c_str());
	}
};

CRouteItem::CRouteItem() : CELoadItem()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CELoadItemProp* pItemProp = docData.m_ItemPropMap[CRouteItem::TypeString()];
	if(pItemProp)
	{
		m_pProp = pItemProp->Clone();
		m_pProp->SetOwner(this);
	}
	m_bSummary = false;
}

CRouteItem::~CRouteItem(void)
{
	try
	{
		if(NULL != m_pProp)    SAFE_DELETE(m_pProp);

		m_HV_PowerCableEntry.clear();
		m_LV_PowerCableEntry.clear();
		m_ControlCableEntry.clear();
		
		m_TrayItemEntry.clear();
		
	}
	catch(...)
	{
	}
}

string ITEMS::CRouteItem::TableName(void)
{
	static const string __TABLE_NAME__ = _T("T_ROUTE_ITEM");

	return __TABLE_NAME__;
}

/**
	@brief	

	@author KHS	

	@return		
**/
string ITEMS::CRouteItem::GetName(void) const
{
	CELoadItemProp* pProp = prop();
	if(pProp) return pProp->GetValue("General" , "Route ID");

	return string("");
}

/**
	@brief	

	@author KHS	

	@return		
**/
int ITEMS::CRouteItem::SetName(const string& rRouteName)
{
	CELoadItemProp* pProp = prop();
	if(pProp) return pProp->SetValue("General", "Route ID" , rRouteName);

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	ROUTE��  HV POWER CABLE�� �״´�.

	@author KHS	

	@return		
**/
int ITEMS::CRouteItem::AddHVPowerCable(CCableItem* pCableItem)
{
	ASSERT(pCableItem && "pCableItem is NULL");

	m_HV_PowerCableEntry.push_back(pCableItem);
	
	return ERROR_SUCCESS;
}

/**
	@brief	ROUTE��  LV POWER CABLE�� �״´�.

	@author KHS	

	@return		
**/
int ITEMS::CRouteItem::AddLVPowerCable(CCableItem* pCableItem)
{
	ASSERT(pCableItem && "pCableItem is NULL");

	m_LV_PowerCableEntry.push_back(pCableItem);
	
	return ERROR_SUCCESS;
}

/**
	@brief	ROUTE��  CONTROL CABLE�� �״´�.

	@author KHS	

	@return		
**/
int ITEMS::CRouteItem::AddControlCable(CCableItem* pCableItem)
{
	ASSERT(pCableItem && "pCableItem is NULL");

	m_ControlCableEntry.push_back(pCableItem);
	
	return ERROR_SUCCESS;
}

/**
	@brief	������ TRAY�� ����

	@author KHS	

	@date 2009-05-19 ���� 7:09:53	

	@param	

	@return		
**/
int ITEMS::CRouteItem::AddTrayItem(CTrayItem* pTrayItem)
{
	ASSERT(pTrayItem && "pTrayItem is NULL");

	m_TrayItemEntry.push_back(pTrayItem);

	return ERROR_SUCCESS;
}



/**
	@brief	

	@author KHS	

	@date 2009-05-28 ���� 5:35:16	

	@param	

	@return		
**/
bool ITEMS::CRouteItem::IsSummary(void) const
{
	return m_bSummary;
}

/**
	@brief	Summary set

	@author KHS	

	@date 2009-05-28 ���� 5:35:19	

	@param	

	@return		
**/
int ITEMS::CRouteItem::SetSummaryFlag(const bool& bSummary)
{
	m_bSummary = bSummary;

	return ERROR_SUCCESS;
}

/**
	@brief	HV POWER CABLE�� Length������ ����

	@author KHS	

	@date 2009-05-29 ���� 9:53:04	

	@param	

	@return		
**/
int ITEMS::CRouteItem::SortHVPowerCableByLength(void)
{
	sort(m_HV_PowerCableEntry.begin(), m_HV_PowerCableEntry.end(), CSort_LengthComp());

	return ERROR_SUCCESS;
}


/**
	@brief	LV POWER CABLE�� Length������ ����

	@author KHS	

	@date 2009-05-29 ���� 9:53:04	

	@param	

	@return		
**/
int ITEMS::CRouteItem::SortLVPowerCableByLength(void)
{
	sort(m_LV_PowerCableEntry.begin(), m_LV_PowerCableEntry.end(), CSort_LengthComp());
	return ERROR_SUCCESS;
}

/**
	@brief	CONTROL CABLE�� Length������ ����

	@author KHS	

	@date 2009-05-29 ���� 9:53:04	

	@param	

	@return		
**/
int ITEMS::CRouteItem::SortControlCableByLength(void)
{
	sort(m_ControlCableEntry.begin(), m_ControlCableEntry.end(), CSort_LengthComp());

	return ERROR_SUCCESS;
}

/**
	@brief	REMOVE CABLE

	@author KHS	

	@date 2009-07-09 ���� 4:00:46	

	@param	

	@return		
**/
int ITEMS::CRouteItem::RemoveHVPowerCableItem(CCableItem* pCableItem)
{
	ASSERT(pCableItem && "pCableItem is NULL");

	if(pCableItem)
	{
		vector<CCableItem*>::iterator where = find(m_HV_PowerCableEntry.begin() , m_HV_PowerCableEntry.end() , pCableItem);
		if(where != m_HV_PowerCableEntry.end()) m_HV_PowerCableEntry.erase(where);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	REMOVE CABLE

	@author KHS	

	@date 2009-07-09 ���� 4:00:46	

	@param	

	@return		
**/
int ITEMS::CRouteItem::RemoveLVPowerCableItem(CCableItem* pCableItem)
{
	ASSERT(pCableItem && "pCableItem is NULL");

	if(pCableItem)
	{
		vector<CCableItem*>::iterator where = find(m_LV_PowerCableEntry.begin() , m_LV_PowerCableEntry.end() , pCableItem);
		if(where != m_LV_PowerCableEntry.end()) m_LV_PowerCableEntry.erase(where);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	REMOVE CABLE

	@author KHS	

	@date 2009-07-09 ���� 4:00:46	

	@param	

	@return		
**/
int ITEMS::CRouteItem::RemoveControlCableItem(CCableItem* pCableItem)
{
	ASSERT(pCableItem && "pCableItem is NULL");

	if(pCableItem)
	{
		vector<CCableItem*>::iterator where = find(m_ControlCableEntry.begin() , m_ControlCableEntry.end() , pCableItem);
		if(where != m_ControlCableEntry.end()) m_ControlCableEntry.erase(where);
	}

	return ERROR_SUCCESS;
}