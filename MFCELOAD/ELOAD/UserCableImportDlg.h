#pragma once

#include "MFCButtonEx.h"

// CUserCableImportDlg dialog

class CUserCableImportDlg : public CDialog
{
	DECLARE_DYNAMIC(CUserCableImportDlg)

public:
	CUserCableImportDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CUserCableImportDlg();

// Dialog Data
	enum { IDD = IDD_USER_CABLE_IMPORT };
	CMFCButtonEx m_wndOpenButton;
	CMFCButtonEx m_wndOKButton;
	CMFCButtonEx m_wndCancelButton;

	CString m_rUserCableExcelPath;

	int m_nOption;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedDirOpen();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedRadioUpdateLoads();
	afx_msg void OnBnClickedRadioNewLoads();
};
