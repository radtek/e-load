
#pragma once

#include "BusGroupItemProp.h"
#include "BusItemProp.h"
#include "Observer.h"
#include "ItemSelectionSet.h"
#include "ELoadPropList.h"
#include "ELoadPropTabWnd.h"
#include "MFCPropEx.h"

class CPropertiesToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CPropertiesWnd : public CDockablePane
{
// �����Դϴ�.
public:
	CPropertiesWnd();

	void AdjustLayout();

// Ư���Դϴ�.
public:
	void SetVSDotNetLook(BOOL bSet)
	{
		for(map<CString , CWnd*>::iterator itr = m_PropWndEntry.begin();itr != m_PropWndEntry.end();++itr)
		{
			if(itr->second->IsKindOf(RUNTIME_CLASS(CELoadPropList)))
			{
				CELoadPropList* pPropList = static_cast<CELoadPropList*>(itr->second);
				pPropList->SetVSDotNetLook (bSet);
				pPropList->SetGroupNameFullWidth (bSet);
			}
			else if(itr->second->IsKindOf(RUNTIME_CLASS(CELoadPropTabWnd)))
			{
				CELoadPropTabWnd* pPropTabWnd = static_cast<CELoadPropTabWnd*>(itr->second);
				pPropTabWnd->SetVSDotNetLook (bSet);
				pPropTabWnd->SetGroupNameFullWidth (bSet);
			}
		}
	}

protected:
	CFont m_fntPropList;
	CComboBox m_wndObjectCombo;
	CPropertiesToolBar m_wndToolBar;
	
	CObserver*			m_pObserver;
	CItemSelectionSet		m_SelectionSet;

	map<CString , CWnd*>            m_PropWndEntry;		//! Property Type���� ������ �����͸� �����Ѵ�.
	CWnd*                           m_pActivePropWnd;       //! ���� Ȱ��ȭ�� PropList�� ����Ų��.

// �����Դϴ�.
public:
	CObserver* observer() const
	{
		return m_pObserver;
	}
	CItemSelectionSet& GetSelectionSet()
	{
		return m_SelectionSet;
	}
	void CommitModifiedValues(void);
	int ChangeContents(const CString& rTreeText , CEventData* pEventData = NULL);

	virtual ~CPropertiesWnd();
	static int OnEventFromObserver(CObserver* pObserver , CEventData* pEventData);
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnExpandAllProperties();
	afx_msg void OnUpdateExpandAllProperties(CCmdUI* pCmdUI);
	afx_msg void OnSortProperties();
	afx_msg void OnUpdateSortProperties(CCmdUI* pCmdUI);
	afx_msg void OnProperties1();
	afx_msg void OnUpdateProperties1(CCmdUI* pCmdUI);
	afx_msg void OnProperties2();
	afx_msg void OnUpdateProperties2(CCmdUI* pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg LRESULT OnPropertyChanged(WPARAM, LPARAM);
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()

	int FindPropGroupOf( CString& rPropGroupFilePath , const CString& rItemName );
	int SetPropValues( const CString& rPropName , CEventData* pEventData );
	void InitPropList(const CString& rTreeText);
	void SetPropListFont();
public:
	int SetItemLockProp(const string& rCategory , const string& rKey , const BOOL& bLock);
	int UpdateAllProperties(const string& rItemType);
private:
	int ModifyPropertyEnableIfNeed(const string& rCategoryName , const string& rKeyName);
};
