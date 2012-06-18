#if !defined(AFX_SIMPLEPROGRESSBAR_H__BC470DEA_FB74_425A_B860_34C9D4CAF6E8__INCLUDED_)
#define AFX_SIMPLEPROGRESSBAR_H__BC470DEA_FB74_425A_B860_34C9D4CAF6E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SimpleProgressBar.h : header file
//
#include <gui/TextProgressCtrl.h>
#include "resource.h"

class CSimpleProgressBar : public CDialog
{
	CBitmap m_bitmap;
	CSize m_sizeBitmap;
	CString m_sStatusText;

// Construction
public:
	CSimpleProgressBar();
	CSimpleProgressBar(CWnd * pWndParent);

	enum { IDD = IDD_SIMPLE_PROGRESSBAR };
// Attributes
public:
	CFont m_Font;
// Operations
public:
	bool Create(CWnd * pWndParent);

	static bool RegisterSplashWndClass();
	static void PassMsgLoop(
		bool bEnableOnIdleCalls
		);
	void SetStatusText(LPCTSTR sStatusText , const int& nProgress);

private:
	void __initControls();
	CTextProgressCtrl m_wndProgressCtrl;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimpleProgressBar)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSimpleProgressBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSimpleProgressBar)
	///afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
}; // class CSimpleProgressBar

#define SIMPLE_PROGRESSBAR_WNDCLASS _T("PDMProgressBar")

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMPLEPROGRESSBAR_H__BC470DEA_FB74_425A_B860_34C9D4CAF6E8__INCLUDED_)
