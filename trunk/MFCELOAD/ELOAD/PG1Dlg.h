#pragma once

#include "EventData.h"
// CPG1Dlg dialog

class CPG1Dlg : public CDialog
{
	DECLARE_DYNAMIC(CPG1Dlg)

public:
	CPG1Dlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPG1Dlg();

// Dialog Data
	enum { IDD = IDD_PG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	int CalculatePG1(void);
public:
	afx_msg void OnEnChangeEditPg1Wl();
	afx_msg void OnEnChangeEditPg1L();
	afx_msg void OnEnChangeEditPg1Cos();
	int SetEventDataValues(CEventData*  pEventData);
};
