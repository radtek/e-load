#pragma once

#include "fpspread1.h"

// CTransformerSizingResultView form view

class CTransformerSizingResultView : public CFormView
{
	DECLARE_DYNCREATE(CTransformerSizingResultView)

protected:
	CTransformerSizingResultView();           // protected constructor used by dynamic creation
	virtual ~CTransformerSizingResultView();

public:
	enum { IDD = IDD_TRANSFORMER_SIZING_RESULT_VIEW };
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
	CFpspread1 m_wndSpread;
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	int DisplayTranformerSizingResultData(void);
	int AdjustSheetTextAlign(void);
	static int CreateHeader(CFpspread1& wndSpread);
	int AddColumnItem(const int& nRow , const int& nColumn , const string& rText);
	string* GetLoadSummaryDataForBus(const string& rBusId);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFileSave();
	int SetSheetIdColor(void);
	afx_msg void OnFilePrint();

	static int ExportToExcel(const CString& rFilePath);
private:
};


