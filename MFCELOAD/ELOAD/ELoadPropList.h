#pragma once

#include "ELoadItemProp.h"
#include "EventData.h"
#include "MFCPropEx.h"


// CELoadPropList

class CELoadPropList : public CMFCPropertyGridCtrl
{
	DECLARE_DYNAMIC(CELoadPropList)

public:
	CELoadPropList();
	virtual ~CELoadPropList();

	static int IDEAL_WIDTH;

	int ModifyPropertyEnableIfNeed(CItemSelectionSet& SelectionSet);
	int InitPropList(CELoadItemProp* pItemProp);
	int AddCategory(CItemPropCategory* pCategory);
        int SetValue( const string& rCategory , const string& rkey , const string& rValue , const string& rOriginalValue = _T("IGNORE ORIGINAL VALUE"));
	void CommitModifiedValues(void);
protected:
	afx_msg LRESULT OnItemSelected(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	void CommitModifiedValues(CMFCPropertyGridProperty* pProp);
	CMFCPropertyGridProperty* AddPropSubItem( CMFCPropEx* pGroup , CItemPropCategory::ItemPropTag* pItemPropCategory );
	int SetEventDataValues( CEventData* pEventData );
};

