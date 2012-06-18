#pragma once

#include "TreeListCtrl.h"
#include "MFCButtonEx.h"
#include "afxcmn.h"
#include "afxwin.h"
#include <gui/FilterEdit/BoostBaseFilterEdit.h>
#include "WizardPropertyPage.h"
#include "TreeListCtrlEx.h"

// CableRouteGroupCreationDlg dialog

class CCableRouteGroupCreationDlg : public CWizardPropertyPage
{
	DECLARE_DYNAMIC(CCableRouteGroupCreationDlg)

public:
	CCableRouteGroupCreationDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCableRouteGroupCreationDlg();

// Dialog Data
	enum { IDD = IDD_CABLE_ROUTE_GROUP_CREATION_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CBoostBaseFilterEdit	m_wndRouteGrEdit;
	CBoostBaseFilterEdit	m_wndRouteGrDescEdit;

	CEdit	m_wndStartEdit;
	CEdit	m_wndEndEdit;
	
	CMFCButtonEx	m_wndStartButton;
	CMFCButtonEx	m_wndEndButton;

	CMFCButtonEx	m_wndRouteGrAddButton;
	CMFCButtonEx	m_wndSaveButton;
	CMFCButtonEx	m_wndCancelButton;

	CMFCButtonEx	m_wndBackButton;
	CMFCButtonEx	m_wndNextButton;

	CTreeListCtrlEx	m_wndRouteTreeListCtrl;
	CTreeListCtrlEx	m_wndRouteGroupTreeListCtrl;

	CImageList	m_imgList;

	bool bDeleteRouteGr;

public:
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedRouteGrAddButton();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedStartButton();
	afx_msg void OnBnClickedEndButton();

	int DisplayRouteTreeCtrl(void);
	int DisplayRouteGroupTreeCTrl(void);
	int InitEditCtrl(void);
	int ClearEdit(void);

	HTREEITEM AddRootTrayItem(const CString& rRouteType, const CString& rLength, const CString& rTD, const CString& rDefaultSize, CTreeListCtrlEx& TreeCtrl);
	HTREEITEM AddRootTrayItem(const CString& rRouteGroup, const CString& rDesc, const CString& rDesignLength, CTreeListCtrlEx& TreeCtrl);
	HTREEITEM AddChildTrayItem(HTREEITEM hParent, CTreeListCtrlEx& TreeCtrl, const CString&  rRouteType,const CString& rLength, const CString& rTD, const CString& rDirection,const CString& rDefaultSize);

	int FindNodeByString(HTREEITEM hItem, const CString& rName);
	int FindNodeByStringReverse(HTREEITEM hItem, const CString& rName);
	int CreateImageList(void);
	int SaveData(void);
	BOOL IsExistRouteTypeName(const CString& rRouteGroup);
	
	BOOL FindSiblingNodeByString(HTREEITEM hItem, const CString& rItemText);
	afx_msg void OnBnClickedWizback();
	afx_msg void OnBnClickedWiznext();
	BOOL Init(void);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	int CreateRouteTypeTreeListCtrl(void);
	int CreateRouteGroupTreeListCtrl(void);
	
	afx_msg void OnCableRouteDelete();
};
