#pragma once

#include "LoadItem.h"
#include "MFCButtonEx.h"
// CShowRemarkDlg dialog

class CShowRemarkDlg : public CDialog
{
	DECLARE_DYNAMIC(CShowRemarkDlg)

public:
	CShowRemarkDlg(ITEMS::CLoadItem* pLoadItem , CWnd* pParent = NULL);   // standard constructor
	virtual ~CShowRemarkDlg();

// Dialog Data
	enum { IDD = IDD_SHOW_REMARK };

	CMFCListCtrl m_wndRemarkListCtrl;
	CMFCButton m_wndAddButton;
	CMFCButtonEx m_wndOKButton , m_wndCancelButton;

	ITEMS::CLoadItem*  m_pLoadItem;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedOk();

};
