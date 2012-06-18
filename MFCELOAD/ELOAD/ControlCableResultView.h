#pragma once

#include "fpspread1.h"

// CControlCableResultView form view

class CControlCableResultView : public CFormView
{
	DECLARE_DYNCREATE(CControlCableResultView)

protected:
	CControlCableResultView();           // protected constructor used by dynamic creation
	virtual ~CControlCableResultView();

public:
	enum { IDD = IDD_CONTROL_CABLE_RESULT_VIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

private:
	CFpspread1	m_wndSpread;
public:
	afx_msg void OnFilePrint();
	int DisplayControlCableResult(void);
	int CreateSheetHeader(void);
	int AdjustSheetTextAlign(void);
	afx_msg void OnFileSave();
	static int ExportToExcel(const CString& rFolderPath);
	static int SetBackgroundColor(CFpspread1& wndSpread);
};


