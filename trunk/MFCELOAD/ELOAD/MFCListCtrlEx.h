#pragma once

#include <list>
using namespace std;

#include "Observer.h"
// CMFCListCtrlEx

class CMFCListCtrlEx : public CMFCListCtrl
{
	DECLARE_DYNAMIC(CMFCListCtrlEx)

public:
	CMFCListCtrlEx();
	virtual ~CMFCListCtrlEx();

protected:
	DECLARE_MESSAGE_MAP()
public:
        afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
        static int OnEventFromObserver(CObserver* pObserver , CEventData* pEventData);
private:
	string m_rBusID , m_rArea;
        CObserver*      m_pObserver;
public:
	virtual int AddELoadItem(CELoadItem* pItem);
	virtual int RemakeListCtrl(const string& rHeaderText , list<CELoadItem*>* pELoadItemList);
        afx_msg void OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
	void SetSortColumn (int iColumn, BOOL bAscending = TRUE, BOOL bAdd = FALSE);
	void RemoveSortColumn (int iColumn);
	void EnableMultipleSort (BOOL bEnable = TRUE);
	BOOL IsMultipleSort () const;
	int OnCompareItems (LPARAM lParam1, LPARAM lParam2, int iColumn);
private:
	string m_rCommandString , m_rColumnTitleString;
public:
        afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
        void InsertItemCount(void);
        afx_msg void OnItemDelete();
        afx_msg void OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
        afx_msg void OnLvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
        void DisplayItemCount(void);
        ///afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
        afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
        afx_msg void OnItemCopy();
private:
	int ShowSelectedItemInPropertiesBar(const bool& bOnShow);
	bool m_bShowPropertiesBarAfterEndLabelEdit;
public:
	afx_msg void OnCopyTransformer();
	int CreateItem(const string& rTypeString);
	string GetBusID() const;
	int SetBusID(const string& rBusID);
	string GetArea() const;
	int SetArea(const string& rArea);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	int SetColumnTitleString(const string& rColumnTitleString);
	int SetCommandString( const string& rCommandString );
	string GetCommandString() const;
	void OnItemDeleteAll();
private:
	int SelectAsFirstVisibleItem(const string& rItemText);
};


class CMFCCableListCtrlEx : public CMFCListCtrlEx
{
	DECLARE_DYNAMIC(CMFCCableListCtrlEx)

public:
	CMFCCableListCtrlEx();
	virtual ~CMFCCableListCtrlEx();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual int AddELoadItem(CELoadItem* pItem);
	int RemakeListCtrl(const string& rHeaderText , list<CELoadItem*>* pELoadItemList);
	int OnCompareItems (LPARAM lParam1, LPARAM lParam2, int iColumn);
};

class CMFCTransformerListCtrlEx : public CMFCListCtrlEx
{
	DECLARE_DYNAMIC(CMFCTransformerListCtrlEx)

public:
	CMFCTransformerListCtrlEx();
	virtual ~CMFCTransformerListCtrlEx();

protected:
	DECLARE_MESSAGE_MAP()
public:
	int RemakeListCtrl(const string& rHeaderText , list<CELoadItem*>* pELoadItemList);
};