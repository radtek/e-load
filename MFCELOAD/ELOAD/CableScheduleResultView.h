#pragma once
#include "fpspread1.h"



// CCableScheduleResultView form view

class CCableScheduleResultView : public CFormView
{
	DECLARE_DYNCREATE(CCableScheduleResultView)

protected:
	CCableScheduleResultView();           // protected constructor used by dynamic creation
	virtual ~CCableScheduleResultView();

public:
	enum { IDD = IDD_USER_CABLE_RESULT_VIEW };
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
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	static int CreateSheetHeader(CFpspread1& m_wndSpread);
	int DisplayCableScheduleResult(void);
	int AdjustSheetTextAlign(void);
	afx_msg void OnFileSave();
	afx_msg void OnFilePrint();
	static int UncompressExcelFile(const CString& rExcelPath, const CString& rFolderPath);
	static int ExportToExcel(const CString& rFilePath);
private:
	int WriteCableScheduleToTextFile(const string& rFilePath , const vector<string>& MappingList);
	int WriteCableItemToTextFile(ofstream& ofile , CCableItem* pCableItem , const vector<string>& MappingList);
};


