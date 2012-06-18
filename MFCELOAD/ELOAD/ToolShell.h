#if !defined(AFX_TOOLSHELL_H__6E17B8FB_1790_42C7_9502_C4D0AC243055__INCLUDED_)
#define AFX_TOOLSHELL_H__6E17B8FB_1790_42C7_9502_C4D0AC243055__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolShell.h : header file
//
#include <afxhtml.h>
/////////////////////////////////////////////////////////////////////////////
// CToolShell window

class CToolShell : public CWnd
{
// Construction
public:
	CToolShell();
// Attributes
public:
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolShell)
	//}}AFX_VIRTUAL

// Implementation
public:
	void Create(CWnd *pParent);
	virtual ~CToolShell();

	// Generated message map functions
protected:
	//{{AFX_MSG(CToolShell)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CWnd m_wndBrowser;
	IWebBrowser2* m_pBrowserApp;
	CWnd* m_pCurrentTool;

	CComVariant *m_pvarURL , *m_pvarself , *m_pvarEmpty; 

public:
	int Navigate2(const CString& rURL);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLSHELL_H__6E17B8FB_1790_42C7_9502_C4D0AC243055__INCLUDED_)
