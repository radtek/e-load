#pragma once

#include <gui/EditListBox/EditListBox.h>
#include "MFCButtonEx.h"

// CCableScheduleSettingDlg dialog

class CCableScheduleSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CCableScheduleSettingDlg)

public:
	CCableScheduleSettingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCableScheduleSettingDlg();

// Dialog Data
	enum { IDD = IDD_CABLE_SCHEDULE_SETTING };
	CDragListBox m_wndBusIDListBox;
	CMFCButtonEx m_wndCreationButton;
	CMFCButtonEx m_wndOKButton , m_wndCancelButton;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	CFont* m_pFont;
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonCreation();
};
