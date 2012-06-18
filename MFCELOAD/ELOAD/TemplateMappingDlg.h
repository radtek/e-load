#pragma once
#include "afxwin.h"
#include "MFCButtonEx.h"

// CTemplateMappingDlg dialog

class CTemplateMappingDlg : public CDialog
{
	DECLARE_DYNAMIC(CTemplateMappingDlg)

public:
	CTemplateMappingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTemplateMappingDlg();

// Dialog Data
	enum { IDD = IDD_TEMPLATE_MAPPING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox		m_wndUserCableTemplateCombo,m_wndLoadListTemplateCombo;
	CComboBox		m_wndCableScheduleTemplateCombo,m_wndDrumScheduleTemplateCombo;
	CMFCButtonEx		m_wndOkButton;
	CMFCButtonEx		m_wndCancelButton;

	CString	m_rUserCableTemplate,m_rLoadListTemplate;
	CString	m_rCableScheduleTemplate,m_rDrumScheduleTemplate;

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedOk();
};
