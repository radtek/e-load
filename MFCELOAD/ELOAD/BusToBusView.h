#pragma once

///#include <aDrawCtrl/inc/IsDrawEditor.h>
#include "BusToBusDoc.h"
///#include "BusToBusRelationFactory.h"
///#include "BusToBusEditor.h"
#include <gui/gridctrl/GridCtrl.h>
#include <gui/gridctrl/GridCellCombo.h>

// CBusToBusView view

class CGridCellBusGroupCombo : public CGridCellCombo
{
	DECLARE_DYNCREATE(CGridCellBusGroupCombo)
public:
	CGridCellBusGroupCombo();

	// editing cells
public:
	virtual BOOL  Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
};

class CGridCellBusCombo : public CGridCellCombo
{
	DECLARE_DYNCREATE(CGridCellBusCombo)
public:
	CGridCellBusCombo();

	// editing cells
private:
	//! 2011.02.08 added by HumKyung
	CString m_sBusGroupName;
public:
	int SetBusGroupName(const CString& sBusGroupName);
	//!
	virtual BOOL  Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
};

class CBusToBusView : public CView
{
	DECLARE_DYNCREATE(CBusToBusView)

protected:
	CBusToBusView();           // protected constructor used by dynamic creation
	virtual ~CBusToBusView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
protected:
	DECLARE_MESSAGE_MAP()

	//! 2011.01.05 - added by HumKyung
	CGridCtrl m_wndGridCtrl;
	CMFCButton m_wndAddButton , m_wndDeleteButton , m_wndSaveButton;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnInitialUpdate();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedSave();
	afx_msg void OnDestroy();
};
