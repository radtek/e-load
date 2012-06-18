#include "StdAfx.h"
#include "Observer.h"
#include "ELoadDocData.h"
#include <Assert.h>

CObserver::CObserver(CWnd* pOwner , OBSERVER_ON_EVENT_CALLBACK pOnEventCallback) : m_pOwner(pOwner) , m_pOnEventCallback(pOnEventCallback)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	docData.AttachObserver(this);
}

CObserver::~CObserver(void)
{
	try
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		docData.DetachObserver(this);
	}
	catch(...)
	{
	}
}

/**
	@author	BHK
*/
int CObserver::FireEvent( CEventData& EventData )
{
	if(CEventData::MODIFY_ITEM_PROP == EventData.m_eEventType)
	{
		const int nItemCount = EventData.m_SelectionSet.GetItemCount();
		for(int i = 0;i < nItemCount;++i)
		{
			EventData.m_SelectionSet.GetItemAt(i)->SetModifiedFlag(true);
		}
	}

	CELoadDocData& docData = CELoadDocData::GetInstance();
	return docData.DispatchEvent( this , EventData );
}

/**
*/
int CObserver::OnEvent(CEventData* pEventData)
{
	assert(pEventData && "pEventData is NULL");

	if(pEventData)
	{
		///m_pOwner->SendMessage(WM_NOTIFY , (WPARAM)CObserver::FROM_OBSERVER , (LPARAM)pEventData);
		if(NULL != m_pOnEventCallback) m_pOnEventCallback( this , pEventData );

		return ERROR_SUCCESS;
	}
	
	return ERROR_INVALID_PARAMETER;
}
