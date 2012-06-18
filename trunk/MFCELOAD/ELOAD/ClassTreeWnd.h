//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a sample for BCGControlBar Library Professional Edition
// Copyright (C) 1998-2008 BCGSoft Ltd.
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
//*******************************************************************************
//
#if !defined(AFX_CLASSTREEWND_H__0517A828_D18F_11D3_A727_009027900694__INCLUDED_)
#define AFX_CLASSTREEWND_H__0517A828_D18F_11D3_A727_009027900694__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClassTreeWnd.h : header file
//
#include "BusItem.h"
/////////////////////////////////////////////////////////////////////////////
// CClassTreeWnd window

class CClassTreeWnd : public CTreeCtrl
{
// Construction
public:
	CClassTreeWnd();

// Attributes
public:
	typedef struct
	{
		CString rItemText;
		CString rItemImage , rSelectedImage;
		CString rCommand;
		CString rTypeStr , rHeaderText;
	}ELOAD_ITEM_FILE_STRUCT;
// Operations
public:
	void FillClassView ();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClassTreeWnd)
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL
	map<HTREEITEM,ELOAD_ITEM_FILE_STRUCT*> m_ELoadItemFileStructMap;
	void CreateObjTreeNode(CTreeCtrl* pTreeCtrl , HTREEITEM hParent, ifstream& ifile);
// Implementation
public:
	virtual ~CClassTreeWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CClassTreeWnd)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblClick(NMHDR *pNMHDR, LRESULT *pResult);
	int AddTreeNodeOf(CELoadItem* pItem);
	int DeleteTreeNodeOf(CELoadItem* pItem);
private:
	HTREEITEM FindItemByCommand(HTREEITEM hItem , const CString& rCommand);
public:
	int ModifyBusItem(CBusItem* pBusItem);
        int UpdateShowLoadBusItem();
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	int DeleteCableSizingTableFile(HTREEITEM hItem);
private:
	int AddRevDataTreeNode(HTREEITEM hItem , const CString& rRevNo);
public:
	int RemakeRevDataTreeNodeOf(const string& rProductName);
	int DisplayLoadItemCountOf(CELoadItem* pItem);
	int DisplayCableItemCount(void);
	int DisplayArea(void);
private:
	int DeleteChildNodeOf(HTREEITEM hItem);
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRevItemDelete();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLASSTREEWND_H__0517A828_D18F_11D3_A727_009027900694__INCLUDED_)
