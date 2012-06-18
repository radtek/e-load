#pragma once
#include <gui/FilterEdit/BoostBaseFilterEdit.h>

// CPowerCableSizingOfFaultCurrentDlg dialog

class CPowerCableSizingOfFaultCurrentDlg : public CDialog
{
	DECLARE_DYNAMIC(CPowerCableSizingOfFaultCurrentDlg)

public:
	CPowerCableSizingOfFaultCurrentDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPowerCableSizingOfFaultCurrentDlg();

	CMFCButton m_wndClearButton;
	CMFCButton m_wndApplyButton,m_wndCancelButton;

	CBoostBaseFilterEdit m_wndFaultClearingTimeEdit;
	CBoostBaseFilterEdit m_wndFaultCurrentEdit;

	static int __CABLE_CONDUCTOR__;
	static int __INSULATION_MATERIAL__;
	static CString __POWER_CABLE_SIZING__;
	static CString __FAULT_CLEARING_TIME__;
	static CString __FAULT_CURRENT__;
// Dialog Data
	enum { IDD = IDD_POWER_CABLE_SIZING_OF_FAULT_CURRENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonCalc();
	afx_msg void OnBnClickedRadioCu();
	afx_msg void OnBnClickedRadioAl();
	afx_msg void OnBnClickedRadioPvc();
	afx_msg void OnBnClickedRadioXlpe();
	afx_msg void OnEnChangeEditFaultClearingTime();
	afx_msg void OnEnChangeEditFaultCurrent();
	afx_msg void OnBnClickedButtonClear();

	int LoadData(void);
	int DisplayData(void);
	int SaveData(void);
	afx_msg void OnBnClickedOk();
};
