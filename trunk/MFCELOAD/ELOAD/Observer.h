#pragma once

#include "EventData.h"

class CObserver;
typedef int (*OBSERVER_ON_EVENT_CALLBACK)(CObserver* , CEventData*);

class CObserver
{
public:
	enum
	{
		FROM_OBSERVER = 1234
	};

	CObserver(CWnd* pOwner , int (*pOnEventCallback)(CObserver*,CEventData*));
	virtual ~CObserver(void);
	int FireEvent( CEventData& EventData );
	int OnEvent(CEventData* pEventData);

	CWnd* owner() const{ return m_pOwner; }
private:
	CWnd* m_pOwner;
	OBSERVER_ON_EVENT_CALLBACK m_pOnEventCallback;
};
