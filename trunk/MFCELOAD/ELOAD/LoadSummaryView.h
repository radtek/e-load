#pragma once
#include "fpspread1.h"
#include <vector>


// CLoadSummaryView form view

class CLoadSummaryView : public CFormView
{
	DECLARE_DYNCREATE(CLoadSummaryView)
protected:
	CLoadSummaryView();           // protected constructor used by dynamic creation
	virtual ~CLoadSummaryView();

public:
	enum { IDD = IDD_LOADSUMMARYVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	int ReCalcLayout(void);
	CFpspread1 m_wndSpread;

	CMFCTabCtrl m_wndTab;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileSave();
	afx_msg void OnPrintLoadSummary();

	BOOL LoadTotalTempSheet(void);
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
private:
	void CreateTotalSheetHeader(void);
	void CreateBusSheetHeader(void);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
private:
	int AdjustTotalSheetTextAlign(void);
	int AdjustBusSheetTextAlign(void);
	void SetBusSheetItemIdColor();
	void SetTotalSheetItemIdColor();
public:
	static int ExportToExcel(const CString& rFolderPath , const CString& rType);
private:
	int ApplyModifiedColorToCell(void);
};


