#pragma once

// CControlCableTypeInfoDlg dialog
#include "MFCButtonEx.h"
#include "afxwin.h"

class CControlCableTypeInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CControlCableTypeInfoDlg)

public:
	CControlCableTypeInfoDlg(const CString& rOldCableType , const CString& rOldPrefix , const CString& rOldSuffix, const CString& rOldDesc,  CWnd* pParent = NULL);   // standard constructor
	virtual ~CControlCableTypeInfoDlg();

// Dialog Data
	enum { IDD = IDD_CONTROLCABLE_TYPE_INFO };

	CMFCButtonEx m_wndOKButton , m_wndCancelButton;

	CString		m_rOldCableType, m_rCableType;
	CString		m_rOldPrefix, m_rPrefix;
	CString		m_rOldSuffix, m_rSuffix;
	CString		m_rOldDesc, m_rDesc;

	CEdit		m_wndBeforeCableTypeEdit;
	CEdit		m_wndBeforeDescEdit;
	CEdit		m_wndBeforePrefixEdit;
	CEdit		m_wndBeforeSuffixEdit;
	
	CComboBox	m_wndAfterCableTypeCombo;
	CEdit		m_wndAfterSuffixEdit;
	CEdit		m_wndAfterPrefixEdit;
	CEdit		m_wndAfterDescEdit;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	
	int FillControlCableTypeComboBox(void);
	afx_msg void OnBnClickedOk();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
