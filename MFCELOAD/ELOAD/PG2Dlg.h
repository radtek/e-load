#pragma once

#include "EventData.h"
// CPG2Dlg dialog

class CPG2Dlg : public CDialog
{
	DECLARE_DYNAMIC(CPG2Dlg)

public:
	CPG2Dlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPG2Dlg();

// Dialog Data
	enum { IDD = IDD_PG2 };
	CMFCButton m_wndSave;
	int m_nRadio;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedRadio5();
	afx_msg void OnBnClickedRadio6();
	afx_msg void OnBnClickedRadio7();
	afx_msg void OnBnClickedRadio8();
	afx_msg void OnEnChangeEditMaxStartingCapacity();
	afx_msg void OnEnChangeEditCoeff();
private:
	int CalculatePG2(void);
public:
	int SetEventDataValues(CEventData*  pEventData);

	afx_msg void OnEnChangeEditPlmax();
	afx_msg void OnEnChangeEditV();
	afx_msg void OnEnChangeEditXd1();
	afx_msg void OnEnChangeEditXd2();
	int DisplayPLmaxEquation(const string& rBusID);
	afx_msg void OnEnChangeEditMotorMaxStartingCapacity();
};
