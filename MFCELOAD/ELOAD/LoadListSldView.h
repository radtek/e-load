#pragma once

#include "fpspread1.h"

// CLoadListSldView form view

class CLoadListSldView : public CFormView
{
	DECLARE_DYNCREATE(CLoadListSldView)

protected:
	CLoadListSldView();           // protected constructor used by dynamic creation
	virtual ~CLoadListSldView();

public:
	enum { IDD = IDD_LOAD_LIST_SLD_VIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);

private:
	CFpspread1	m_wndSpread;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	int CreateSheetHeader(void);
	int DisplayLoadListSldData(void);
	int AdjustSheetTextAlign(void);
	afx_msg void OnFilePrint();
	int ExportToExcel(const CString& rFolderPath);
};


