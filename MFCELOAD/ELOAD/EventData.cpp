#include "StdAfx.h"
#include "EventData.h"

DEFINITION_USER_MESSAGE(CEventData,TYPE_NULL)
DEFINITION_USER_MESSAGE(CEventData,SELECT_ITEM_CATEGORY)
DEFINITION_USER_MESSAGE(CEventData,SELECT_ITEM)
DEFINITION_USER_MESSAGE(CEventData,MODIFY_ITEM_PROP)
DEFINITION_USER_MESSAGE(CEventData,ADD_ITEM)
DEFINITION_USER_MESSAGE(CEventData,DELETE_ITEM)
DEFINITION_USER_MESSAGE(CEventData,REORDER_ITEM)

CEventData::CEventData( const string& rItemCategory , UINT eventType ) : m_rItemCategory(rItemCategory) , m_eEventType(eventType)
{
	m_pFromObserver = NULL;
}

CEventData::CEventData( const CEventData& rhs )
{
	(*this) = rhs;	
}

CEventData::~CEventData(void)
{
}

/**
	@brief	대입 연산자

	@author	BHK
*/
CEventData& CEventData::operator=( const CEventData& rhs )
{
	if(this != &rhs)
	{
		m_eEventType    = rhs.m_eEventType;
		m_rItemCategory = rhs.m_rItemCategory;
		m_SelectionSet  = rhs.m_SelectionSet;

		m_pFromObserver = rhs.m_pFromObserver;
	}

	return (*this);
}

int CEventData::GetItemCount() const
{
	return m_SelectionSet.GetItemCount();
}

CELoadItem* CEventData::GetItemAt( const size_t& at )
{
	return m_SelectionSet.GetItemAt(at);
}

int CEventData::Add(CELoadItem* pItem)
{
	return m_SelectionSet.Add(pItem);
}