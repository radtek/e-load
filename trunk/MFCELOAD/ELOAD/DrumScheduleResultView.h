#pragma once

#include "fpspread1.h"

// CDrumScheduleResultView form view

class CDrumScheduleResultView : public CFormView
{
	DECLARE_DYNCREATE(CDrumScheduleResultView)

protected:
	CDrumScheduleResultView();           // protected constructor used by dynamic creation
	virtual ~CDrumScheduleResultView();

public:
	enum { IDD = IDD_DRUMSCHEDULERESULTVIEW };
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
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

private:
	CFpspread1	m_wndSpread;
public:
	afx_msg void OnFilePrint();
	afx_msg void OnFileSave();

	int DisplayDrumScheduleResult(void);
	
	int AdjustSheetTextAlign(void);
	int AdjustTotalSheetTextAlign(void);
	int AdjustCableTotalSheetTextAlign(void);

	static int ExportToExcel(const CString& rFolderPath);

	static int CreateSheetHeader(CFpspread1& m_wndSpread);
	static int CreateTotalSheetHeader(CFpspread1& m_wndSpread);
	static int CreateCableTotalSheetHeader(CFpspread1& m_wndSpread);
};


