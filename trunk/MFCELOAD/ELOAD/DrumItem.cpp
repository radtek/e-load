#include "StdAfx.h"
#include "DrumItem.h"
#include "ELoadDocData.h"

using namespace ITEMS;

IMPLEMENTS_ELOAD_ITEM(CDrumItem , CELoadItem , "DRUM")

CDrumItem::CDrumItem(void): CELoadItem()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CELoadItemProp* pItemProp = docData.m_ItemPropMap[CDrumItem::TypeString()];
	if(pItemProp)
	{
		m_pProp = pItemProp->Clone();
		m_pProp->SetOwner(this);
		m_pCurProp = pItemProp->Clone();
	}
}

CDrumItem::~CDrumItem(void)
{
	try
	{
		if(NULL != m_pProp)    SAFE_DELETE(m_pProp);
		if(NULL != m_pCurProp) SAFE_DELETE(m_pCurProp);
	}
	catch(...)
	{
	}
}

string CDrumItem::TableName()
{
	static const string __TABLE_NAME__ = _T("T_DRUM_ITEM");

	return __TABLE_NAME__;
}

string CDrumItem::GetCategoryString()
{
	return CDrumItem::TypeString();
}

string CDrumItem::GetName() const
{
	CELoadItemProp* pProp = prop();
	if(pProp) return pProp->GetValue("General" , "Drum Id");

	return string("");
}

int CDrumItem::SetName( const string& name )
{
	CELoadItemProp* pProp = prop();
	if(pProp) return pProp->SetValue("General", "Drum Id" , name);

	return ERROR_BAD_ENVIRONMENT;
}

int CDrumItem::Copy(CELoadItem* pCopied)
{
	assert(pCopied && "pCopied is NULL");
	CELoadItem::Copy(pCopied);

	if(pCopied && pCopied->IsKindOf(CDrumItem::TypeString()))
	{
		CDrumItem* pDrumItem = static_cast<CDrumItem*>(pCopied);
		m_pCurProp->Copy(pDrumItem->m_pCurProp);

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	Add Cable

	@author KHS	

	@date 2009-09-22 오후 2:53:38	

	@param	

	@return		
**/
int ITEMS::CDrumItem::AddCable(const CABLE_RECORD& record)
{
	m_CableRecordList.push_back(record);
	
	return ERROR_SUCCESS;
}


static bool fnCompareCableItemLength(CDrumItem::CABLE_RECORD lhs ,CDrumItem::CABLE_RECORD rhs )
{
	return (atof(lhs.rLength) > atof(rhs.rLength));
}

/**
	@brief	

	@author KHS	

	@date 2009-09-28 오후 5:11:07	

	@param	

	@return		
**/
int ITEMS::CDrumItem::GetCableRecordList(list<CABLE_RECORD>& CableRecordList)
{
	::stable_sort(m_CableRecordList.begin(), m_CableRecordList.end(), fnCompareCableItemLength);

	for(list<CABLE_RECORD>::iterator itr = m_CableRecordList.begin(); itr != m_CableRecordList.end();++itr)
	{
		CableRecordList.push_back(*itr);
	}

	return ERROR_SUCCESS;
}
/**
	@brief	

	@author KHS	

	@date 2009-09-29 오후 5:21:36	

	@param	

	@return		
**/
int ITEMS::CDrumItem::ClearCableRecord(void)
{
	m_CableRecordList.clear();

	return ERROR_SUCCESS;
}
