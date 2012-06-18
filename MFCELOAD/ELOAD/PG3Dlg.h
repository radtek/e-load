#pragma once

#include "EventData.h"
// CPG3Dlg dialog

class CPG3Dlg : public CDialog
{
	DECLARE_DYNAMIC(CPG3Dlg)

public:
	CPG3Dlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPG3Dlg();

// Dialog Data
	enum { IDD = IDD_PG3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	int CalculatePG3(void);
public:
	int SetEventDataValues(CEventData*  pEventData);

	afx_msg void OnEnChangeEditCosql();
	afx_msg void OnEnChangeEditK();
	afx_msg void OnEnChangeEditCosqg();
	afx_msg void OnEnChangeEditW0();
	afx_msg void OnEnChangeEditQlmax();
};
