#pragma once
#include "afxwin.h"
#include "MFCButtonEx.h"
#include "XGroupBox.h"
#include "LabelStatic.h"

// CDataResultDlg dialog

class CDataResultDlg : public CDialog
{
	DECLARE_DYNAMIC(CDataResultDlg)

public:
	CDataResultDlg(const CString& rMsg, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDataResultDlg();

// Dialog Data
	enum { IDD = IDD_DATA_RESULT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CString		m_rMsg;
	CMFCButtonEx	m_wndOkButton;

	CLabelStatic	m_wnd1Static;
	CLabelStatic	m_wnd2Static;
	CLabelStatic	m_wnd3Static;
	CLabelStatic	m_wnd4Static;
	CLabelStatic	m_wnd5Static;
	CLabelStatic	m_wnd6Static;
	CLabelStatic	m_wnd7Static;

	CXGroupBox	m_wndResultGroupStatic;

public:
	virtual BOOL OnInitDialog();
};
