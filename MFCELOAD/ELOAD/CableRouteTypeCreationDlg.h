#pragma once

#include "MFCButtonEx.h"
#include "afxcmn.h"
#include "afxwin.h"
#include <gui/FilterEdit/BoostBaseFilterEdit.h>
#include "WizardPropertyPage.h"
#include "TreeListCtrlEx.h"

// CCableRouteTypeCreationDlg dialog

class CCableRouteTypeCreationDlg : public CWizardPropertyPage
{
	DECLARE_DYNAMIC(CCableRouteTypeCreationDlg)

public:
	CCableRouteTypeCreationDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCableRouteTypeCreationDlg();

// Dialog Data
	enum { IDD = IDD_CABLE_ROUTE_TYPE_CREATION_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CTreeListCtrlEx		m_wndRouteTreeListCtrl;

	CBoostBaseFilterEdit	m_wndRouteTypeEdit;
	CBoostBaseFilterEdit	m_wndRouteLengthEdit;

	CComboBox		m_wndRouteDirectionCombo;
	CComboBox		m_wndTdCombo;
	CComboBox		m_wndDefaultsizeCombo;

	CMFCButtonEx		m_wndAddButton;
	CMFCButtonEx		m_wndOkButton;
	CMFCButtonEx		m_wndCancelButton;

	CMFCButtonEx	m_wndBackButton;
	CMFCButtonEx	m_wndNextButton;

	CImageList		m_imgList;
	bool		m_bDeleted;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedAddButton();
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMClickRouteTypeTree(NMHDR *pNMHDR, LRESULT *pResult);

	HTREEITEM AddRootTrayItem(const CString& rRouteType, const CString& rTD);
	HTREEITEM AddChildTrayItem(HTREEITEM hParent, const CString& rRouteType, const CString& rLength, const CString& rTD, const CString& rDirection, const CString& rDefaultSize);
	
	int InitEditCtrl(void);
	int InitDirectionCombo(void);
	int SaveData(void);
	int DisplayRouteTreeCtrl(void);
	int ClearEdit(void);
	int CreateImageList(void);
	int FindNodeByString(HTREEITEM hItem, const CString& rName);
	BOOL DeleteSelectedItem(HTREEITEM hItem, const CString& rItemText);
	BOOL IsExistRouteTypeName(const CString& rRouteTypeName);

	int InitTdCombo(void);
	int InitDefaultSizeCombo(void);
	afx_msg void OnBnClickedWizback();
	afx_msg void OnBnClickedWiznext();
	BOOL Init(void);
	afx_msg void OnBnClickedCancel();
	int CreateRouteTypeTreeListCtrl(void);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	BOOL CheckDirection(const CString& rDirection);
	afx_msg void OnCbnSelchangeTdCombo();
	afx_msg void OnCableRouteDelete();
};
