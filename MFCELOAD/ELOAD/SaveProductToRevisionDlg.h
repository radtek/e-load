#pragma once

#include "MFCButtonEx.h"
// CSaveProductToRevisionDlg dialog

class CSaveProductToRevisionDlg : public CDialog
{
	DECLARE_DYNAMIC(CSaveProductToRevisionDlg)

public:
	CSaveProductToRevisionDlg(const CString& rProductName , CWnd* pParent = NULL);   // standard constructor
	virtual ~CSaveProductToRevisionDlg();

// Dialog Data
	enum { IDD = IDD_SAVE_PRODUCT_TO_REVISION };

	CMFCListCtrl m_wndRevisionListCtrl;
	CMFCButtonEx   m_wndOKButton , m_wndCancelButton;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	CString m_rProductName;
	UINT    m_nLastRevNo;

	int CreateRevisionListCtrl(void);
	int DisplayRevisionData(void);
public:
	afx_msg void OnBnClickedOk();
	int InsertLoadItemInCableSizingResultDoc(void);
};
