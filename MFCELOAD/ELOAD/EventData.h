#pragma once

#include "ELoadItem.h"
#include "ItemSelectionSet.h"

#include <vector>
using namespace std;

#define	TYPE_NULL_MSG			_T("ELOAD_TYPE_NULL")
#define	SELECT_ITEM_CATEGORY_MSG	_T("ELOAD_SELECT_ITEM_CATEGORY")
#define	SELECT_ITEM_MSG			_T("ELOAD_SELECT_ITEM")
#define	MODIFY_ITEM_PROP_MSG		_T("ELOAD_MODIFY_ITEM_PROP")
#define	ADD_ITEM_MSG			_T("ELOAD_ADD_ITEM_PROP")
#define	DELETE_ITEM_MSG			_T("ELOAD_DELETE_ITEM_PROP")
#define	REORDER_ITEM_MSG		_T("ELOAD_REORDER_ITEM_PROP")

#define DECLARE_USER_MESSAGE(name) \
	static UINT name;

#define DEFINITION_USER_MESSAGE(classname,name) \
	UINT classname::name = ::RegisterWindowMessage(name##_MSG);

class CObserver;
class CEventData
{
public:
	/*
	typedef enum
	{
                TYPE_NULL               = 0,
		SELECT_ITEM_CATEGORY	= 1,
		SELECT_ITEM		= 2,
		MODIFY_ITEM_PROP	= 3
	}EventType;
	*/

	CEventData( const string& rItemCategory , UINT eventType );
	CEventData( const CEventData& rhs );
	CEventData& operator=( const CEventData& rhs );
	virtual ~CEventData(void);

	int SetItemCategory( const string& rItemCategory )
	{ 
		m_rItemCategory = rItemCategory;
		return ERROR_SUCCESS;
	}
	string GetItemCategory() const { return m_rItemCategory; }
	int GetItemCount() const;
	CELoadItem* GetItemAt( const size_t& at );
	int Add(CELoadItem* pItem);
	virtual int Execute(const bool& bSetOriginalValue = false){ return ERROR_SUCCESS;};
	
	CObserver*		m_pFromObserver;
	UINT      		m_eEventType;
	CItemSelectionSet	m_SelectionSet;
	string			m_rNewValue;

	DECLARE_USER_MESSAGE(TYPE_NULL)
	DECLARE_USER_MESSAGE(SELECT_ITEM_CATEGORY)
	DECLARE_USER_MESSAGE(SELECT_ITEM)
	DECLARE_USER_MESSAGE(MODIFY_ITEM_PROP)
	DECLARE_USER_MESSAGE(ADD_ITEM)
	DECLARE_USER_MESSAGE(DELETE_ITEM)
	DECLARE_USER_MESSAGE(REORDER_ITEM)
private:
	string			m_rItemCategory;
};
