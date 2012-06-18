#pragma once
#include "MFCButtonEx.h"

// CProjectInfoDlg dialog

class CProjectInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CProjectInfoDlg)

public:
	CProjectInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProjectInfoDlg();

// Dialog Data
	enum { IDD = IDD_PROJECT_INFO };
	CMFCButtonEx	m_wndOKButton , m_wndCancelButton;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	int LoadProjectRecordFromDatabase(void);
public:
	afx_msg void OnBnClickedOk();
};
