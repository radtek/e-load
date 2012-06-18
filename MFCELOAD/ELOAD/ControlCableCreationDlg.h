#pragma once

// CControlCableCreationDlg dialog

class CControlCableCreationDlg : public CDialog
{
	DECLARE_DYNAMIC(CControlCableCreationDlg)

public:
	CControlCableCreationDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CControlCableCreationDlg();

// Dialog Data
	enum { IDD = IDD_CONTROL_CABLE_CREATION };

	CString m_rDCSDesignLength;
	CMFCButton m_wndOKButton , m_wndCancelButton;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	int m_nLoadControlCableRadio;
};
