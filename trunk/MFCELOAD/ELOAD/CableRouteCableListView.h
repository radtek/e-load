#pragma once


#include "fpspread1.h"
#include "CableItem.h"
using namespace ITEMS;
// CCableRouteCableListView form view

class CCableRouteCableListView : public CFormView
{
	DECLARE_DYNCREATE(CCableRouteCableListView)

protected:
	CCableRouteCableListView();           // protected constructor used by dynamic creation
	virtual ~CCableRouteCableListView();

public:
	enum { IDD = IDD_CABLEROUTECABLELISTVIEW };
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

private:
	CFpspread1	m_wndSpread;
public:
	int DisplayCableRouteCableList(void);
	int CreateSheetHeader(void);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFilePrint();
	afx_msg void OnFileSave();
	int AdjustSheetTextAlign(void);
};


