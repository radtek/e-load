#pragma once
#include "fpspread1.h"

// CPowerCableVerificationView form view

class CPowerCableVerificationView : public CFormView
{
	DECLARE_DYNCREATE(CPowerCableVerificationView)

protected:
	CPowerCableVerificationView();           // protected constructor used by dynamic creation
	virtual ~CPowerCableVerificationView();

public:
	enum { IDD = IDD_CABLESIZINGRESULTVIEW };
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
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	int DisplayCableResultData(void);
	int CreateHeader(void);
	int AddColumnItem(const int& nRow, const int& nColumn, const string& rText);
	int AdjustSheetTextAlign(void);
	afx_msg void OnFileSave();
	///int SetSheetIdColor(void);
private:
	int DisplayCableResultDataInBus(int& nRow , CBusItem* pBusItem);
};


