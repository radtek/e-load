#pragma once

//#include <vector>
#include "fpspread1.h"
#include "MFCButtonEx.h"
// CCableSizingTableView form view

class CCableSizingTableView : public CFormView
{
	DECLARE_DYNCREATE(CCableSizingTableView)

protected:
	CCableSizingTableView();           // protected constructor used by dynamic creation
	virtual ~CCableSizingTableView();

public:
	enum { IDD = IDD_CABLESIZINGVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

private:
	CFpspread1 m_wndSpread;
	CMFCButtonEx m_wndDeleteButton;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFileSave();
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	int m_nRowCount;
	int m_nColCount;
public:
	static int CreateHeaderAndFillData(CFpspread1& wndSpread , const string& rFilePath);
	int CompareSafetyVoltage(void);
	afx_msg void OnFilePrint();
	int WriteCableSizingToTxtFile(void);
	int ResetValueOverThousandExceptFirstColumn(void);
protected:
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	int CableSizingSettingTableDataColorSet(void);
	afx_msg void OnBnClickedButtonDelete();
	virtual void OnInitialUpdate();
};


