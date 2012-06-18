#include "StdAfx.h"
#include "ELoad.h"
#include "BusGroupItemProp.h"
#include "BusGroupItem.h"
#include "ELoadDocData.h"

#include <algorithm>
#include <functional>

using namespace ITEMS;

IMPLEMENTS_ELOAD_ITEM(CBusGroupItem , CELoadItem , "BUS_GROUP")

CBusGroupItem::CBusGroupItem( ) : CELoadItem()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CELoadItemProp* pItemProp = docData.m_ItemPropMap[CBusGroupItem::GetCategoryString()];
	m_pProp = pItemProp->Clone();
	m_pProp->SetOwner(this);
}

CBusGroupItem::~CBusGroupItem(void)
{
	try

	{
		if(NULL != m_pProp) SAFE_DELETE(m_pProp);
		m_BusRefEntry.clear();
	}
	catch(...)
	{
	}
}


string CBusGroupItem::GetName() const
{
	CELoadItemProp* pProp = prop();
	if(pProp) return pProp->GetValue("GENERAL" , "BUS GROUP ID");

	return string("");
}

/**
	@brief	Bus Group�� �̸��� �����Ѵ�. �̶� ���� Bus���� Bus Group�� ���� ���� �����Ѵ�.

	@author BHK	

	@date 2009-08-26 ���� 1:46:23	

	@param	

	@return		
*/
int CBusGroupItem::SetName( const string& name)
{
	CELoadItemProp* pProp = prop();
	if(pProp)
	{
		pProp->SetValue(_T("GENERAL") , _T("BUS GROUP ID") , name);
		for(vector<CBusItem*>::iterator itr = m_BusRefEntry.begin();itr != m_BusRefEntry.end();++itr)
		{
			(*itr)->prop()->SetValue(_T("General") , _T("Bus Group") , name);
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

string CBusGroupItem::GetCategoryString()
{
	return CBusGroupItem::TypeString();
}


/**
	@brief  Bus Pointer�� ����  BusItem�� �����Ѵ�.
*/
int CBusGroupItem::RemoveBusItem( CBusItem* pBusItem )
{
	try
	{
		vector<CBusItem*>::iterator where = find(m_BusRefEntry.begin(), m_BusRefEntry.end(), pBusItem );
		if(where != m_BusRefEntry.end())
		{
			(*where)->SetDeleteFlag(true);
			m_BusRefEntry.erase(where);
		}
	}
	catch(exception& ex)
	{
		AfxMessageBox(ex.what());
		return ERROR_BAD_ENVIRONMENT;
	}
	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BAEK HUM KYUNG
*/
int CBusGroupItem::Copy( CBusGroupItem* pCopied )
{
	if(pCopied)
	{
		CELoadItem::Copy(pCopied);
		m_pProp->SetOwner(this);

		/*for(vector<CBusItem*>::iterator itr = pCopied->m_BusEntry.begin();itr != pCopied->m_BusEntry.end();++itr)
		{
		m_BusEntry.push_back( (*itr)->Clone() );
		}*/

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	Order No�� ���Ѵ�.

	@author	BHK
*/
int CBusGroupItem::GetOrderNo() const
{
	CItemPropCategory* pCategory = prop()->GetCategoryByName("General");
	if(pCategory)
	{
		const string rOrderNo = pCategory->GetValue("Order No");
		return atoi(rOrderNo.c_str());
	}

	return -1;
}

/**
	@brief	�� ������

	@author	BHK
*/
bool fnCompareBusOrderNo ( CBusItem* lhs , CBusItem* rhs )
{
	assert(lhs && rhs && "lhs or rhs is NULL");

	if(lhs && rhs)
	{
		try
		{
			return (lhs->GetOrderNo() < rhs->GetOrderNo());
		}
		catch(...)
		{
			ELOAD_LOG4CXX_ERROR(mylogger, "" , 6);
		}
	}

	return false;
}

/**
	@brief	BUS�� ORDER NO������ �����Ѵ�.

	@author	BHK
*/
int CBusGroupItem::SortBusByOrderNo()
{
	sort(m_BusRefEntry.begin() , m_BusRefEntry.end() , fnCompareBusOrderNo);

	return ERROR_SUCCESS;
}

/**
	@brief	BUS�̸����� BUS�� ã�´�.

	@author	BHK

	@return	CBusItem* or NULL
*/
CBusItem* CBusGroupItem::FindBusByName(const string& rBusName)
{
	vector<CBusItem*>::iterator where = find_if(m_BusRefEntry.begin(),m_BusRefEntry.end(),bind2nd(MatchBusName(),rBusName));
	if(m_BusRefEntry.end() != where) return (*where);

	return NULL;
}

/**
	@brief

	@author	BHK
*/
string CBusGroupItem::TableName()
{
	static const string __TABLE_NAME__ = _T("T_BUS_GROUP_ITEM");

	return __TABLE_NAME__;
}