#include "StdAfx.h"
#include "ItemSelectionSet.h"
#include <Assert.h>
#include "ELoadDocData.h"
#include "MFCPropEx.h"
#include "MainFrm.h"

CItemSelectionSet::CItemSelectionSet(void)
{
}

CItemSelectionSet::~CItemSelectionSet(void)
{
}

CItemSelectionSet::CItemSelectionSet(const CItemSelectionSet& rhs)
{
	(*this) = rhs;
}

/**
	@brief	대입 연산자

	@author	BAEK HUM KYUNG
*/
CItemSelectionSet& CItemSelectionSet::operator =(const CItemSelectionSet& rhs)
{
	if(this != &rhs)
	{
		m_ItemEntry.clear();
		m_ItemEntry.insert(m_ItemEntry.begin() , rhs.m_ItemEntry.begin() , rhs.m_ItemEntry.end());
	}

	return (*this);
}

int CItemSelectionSet::Add(CELoadItem* pItem)
{
	assert(pItem && "pItem is NULL");

	if(pItem)
	{
		m_ItemEntry.push_back(pItem);
		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

int CItemSelectionSet::GetItemCount() const
{
	return m_ItemEntry.size();
}

CELoadItem* CItemSelectionSet::GetItemAt( const size_t& at )
{
	if(at < m_ItemEntry.size()) return m_ItemEntry[at];

	return NULL;
}

int CItemSelectionSet::RemoveAt( const size_t& at )
{
	CELoadItem* pItem = GetItemAt(at);
	if(NULL != pItem)
	{
		vector<CELoadItem*>::iterator where = find(m_ItemEntry.begin() , m_ItemEntry.end() , pItem);
		m_ItemEntry.erase( where );

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}


/**
	@brief	변경되었는지 여부를 리턴한다.

	@author	BHK
*/
bool CItemSelectionSet::IsModified(const string& rCategory , const string& rKey )
{
	bool res(false);

	for(vector<CELoadItem*>::iterator itr = m_ItemEntry.begin();itr != m_ItemEntry.end();++itr)
	{
		CELoadItemProp* pItemProp = (*itr)->prop();
		CItemPropCategory* pCategory = pItemProp->GetCategoryByName(rCategory);
		if(pCategory->IsModified(rKey)) return true;
	}

	return res;
}

/**
	@brief	Lock Item여부를 리턴한다.(Group Lock Item이 아님.)

	@author	BHK
*/
bool CItemSelectionSet::IsLockItem(const string& rCategory , const string& rKey )
{
	if(!m_ItemEntry.empty())
	{
		CItemPropCategory* pCategory = m_ItemEntry.front()->prop()->GetCategoryByName(rCategory);
		if(pCategory)
		{
			CItemPropCategory::ItemPropTag* pItemPropTag = pCategory->GetItemPropTag(rKey);
			if(pItemPropTag) return pItemPropTag->IsLockItem();
		}
	}
	
	return false;
}

/**
	@brief	Lock 여부를 리턴한다.

	@author	BHK
*/
bool CItemSelectionSet::IsLock(const string& rCategory , const string& rKey )
{
	bool bUnlock = false , bLock = false;
	for(vector<CELoadItem*>::iterator itr = m_ItemEntry.begin();itr != m_ItemEntry.end();++itr)
	{
		CELoadItemProp* pItemProp = (*itr)->prop();
		CItemPropCategory* pCategory = pItemProp->GetCategoryByName(rCategory);
		if(pCategory->IsLocked(rKey))
			bLock = true;
		else	bUnlock = true;

		if((true == bLock) && (true == bUnlock)) return false;
	}

	if(bLock) return true;
	if(bUnlock) return false;
	return false;
}

/**
	@brief	Lock 여부를 설정한다.

	@author	BHK
*/
int CItemSelectionSet::SetLock(const string& rCategory , const string& rKey , const bool& bLock)
{
	for(vector<CELoadItem*>::iterator itr = m_ItemEntry.begin();itr != m_ItemEntry.end();++itr)
	{
		CELoadItemProp* pItemProp = (*itr)->prop();
		CItemPropCategory* pCategory = pItemProp->GetCategoryByName(rCategory);
		if(pCategory) pCategory->SetLock(rKey , bLock);
	}

	return ERROR_SUCCESS;
}

/**
*/
string CItemSelectionSet::GetValue( const string& rCategory , const string& rKey )
{
	string res("");

	for(vector<CELoadItem*>::iterator itr = m_ItemEntry.begin();itr != m_ItemEntry.end();++itr)
	{
		CELoadItemProp* pItemProp = (*itr)->prop();
		string rValue = pItemProp->GetValue( rCategory , rKey );
		if(res.empty()) 
			res = rValue;
		else if(res != rValue)
		{
			res = "*";
			break;
		}
	}

	return res;
}

/**
	@brief	

	@author	BHK
*/
string CItemSelectionSet::GetOriginalValue( const string& rCategory , const string& rKey )
{
	string res("");

	for(vector<CELoadItem*>::iterator itr = m_ItemEntry.begin();itr != m_ItemEntry.end();++itr)
	{
		CELoadItemProp* pItemProp = (*itr)->prop();
		string rOriginValue = pItemProp->GetOriginalValue( rCategory , rKey );
		if(res.empty()) 
			res = rOriginValue;
		else if(res != rOriginValue)
		{
			res = "*";
			break;
		}
	}

	return res;
}

/**
	@brief	

	@author	BHK
*/
string CItemSelectionSet::GetUnitStr( const string& rCategory , const string& rKey )
{
	string res("");

	for(vector<CELoadItem*>::iterator itr = m_ItemEntry.begin();itr != m_ItemEntry.end();++itr)
	{
		CELoadItemProp* pItemProp = (*itr)->prop();
		string rUnit = pItemProp->GetUnitStr( rCategory , rKey );
		if(res.empty()) 
			res  = rUnit;
		else if(res != rUnit)
		{
			res = "*";
			break;
		}
	}

	return res;
}

/**
	@brief	

	@author	BHK
*/
int CItemSelectionSet::SetUnitStr( const string& rCategory , const string& rKey , const string& rUnitStr )
{
	for(vector<CELoadItem*>::iterator itr = m_ItemEntry.begin();itr != m_ItemEntry.end();++itr)
	{
		CELoadItemProp* pItemProp = (*itr)->prop();
		pItemProp->SetUnitStr(rCategory , rKey , rUnitStr);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
string CItemSelectionSet::GetDataType( const string& rCategory , const string& rKey )
{
	string res("");

	for(vector<CELoadItem*>::iterator itr = m_ItemEntry.begin();itr != m_ItemEntry.end();++itr)
	{
		CELoadItemProp* pItemProp = (*itr)->prop();
		string rDataType = pItemProp->GetDataType( rCategory , rKey );
		if(res.empty()) 
			res  = rDataType;
		else if(res != rDataType)
		{
			res = "*";
			break;
		}
	}

	return res;
}

/**
        @brief 

        @author BHK, KHS
*/
int CItemSelectionSet::SetValue( const string& rCategory , const string& rKey , const string& rValue)
{       
	CELoadDocData& docData = CELoadDocData::GetInstance();

	for(vector<CELoadItem*>::iterator itr = m_ItemEntry.begin();itr != m_ItemEntry.end();++itr)
	{
		CELoadItemProp* pItemProp = (*itr)->prop();
		if((*itr)->IsKindOf(CBusItem::TypeString()) || (*itr)->IsKindOf(CBusGroupItem::TypeString()))
		{
			if("Bus ID" == rKey)
			{
				if(docData.IsExistItemName(CBusItem::TypeString(), rValue))
				{
					string rPreId = pItemProp->GetValue(rCategory, rKey);
					pItemProp->SetValue( rCategory , rKey , rPreId);

					::MessageBox(NULL, "The name exists already!! ", "Warning!", MB_OK|MB_ICONWARNING );
					return ERROR_INVALID_ACCESS;
				}
			}
			else if("Bus Group ID" == rKey)
			{
				if(docData.IsExistItemName(CBusGroupItem::TypeString(), rValue))
				{
					string rPreId = pItemProp->GetValue(rCategory, rKey);
					pItemProp->SetValue( rCategory , rKey , rPreId);

					MessageBox(NULL, "The name exists already!! ", "Warning!", MB_OK|MB_ICONWARNING );
					return ERROR_INVALID_ACCESS;
				}
			}
		}
		pItemProp->SetValue( rCategory , rKey , rValue );
	}

	return ERROR_SUCCESS;
}

/**
*/
string CItemSelectionSet::GetTypeString()
{
	if(!m_ItemEntry.empty())
	{
		return m_ItemEntry.front()->GetTypeString();
	}

	return string("");
}

/**
        @brief BUS 아이템 이름의 중복을 체크한다. TRUE =  중복, FALSE = 중복안함.

	@author	KHS
*/
BOOL CItemSelectionSet::CheckDuplicationOfName(const CString& rName)
{
        CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> BusGroupList;
	docData.GetELoadItemListOf(BusGroupList , CBusGroupItem::TypeString());
	///if(pBusGroupList)
	{
		for(list<CELoadItem*>::iterator itr = BusGroupList.begin(); itr != BusGroupList.end();++itr)
		{
			CBusGroupItem* pBusGroup = static_cast<CBusGroupItem*>(*itr);
			if(pBusGroup)
			{
				if(rName == pBusGroup->GetName().c_str()) return TRUE;
				for(vector<CBusItem*>::iterator jtr = pBusGroup->m_BusRefEntry.begin(); jtr != pBusGroup->m_BusRefEntry.end();++jtr)
				{
					if(rName == (*jtr)->GetName().c_str()) return TRUE;
				}
			}
		}
	}

        return FALSE;
}

/**
	@brief	

	@author	BHK
*/
int CItemSelectionSet::Clear(void)
{
	m_ItemEntry.clear();

	return ERROR_SUCCESS;
}
