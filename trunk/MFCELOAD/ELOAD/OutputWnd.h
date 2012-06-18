
#pragma once
#include "ToolShell.h"
#include <string>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// COutputList â

class COutputList : public CListBox
{
// �����Դϴ�.
public:
	COutputList();

// �����Դϴ�.
public:
	virtual ~COutputList();

protected:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnViewOutput();

	DECLARE_MESSAGE_MAP()
};

class COutputWnd : public CDockablePane
{
// �����Դϴ�.
public:
	COutputWnd();

// Ư���Դϴ�.
protected:
	CFont m_Font;

	CMFCTabCtrl	m_wndTabs;

	///COutputList m_wndOutputBuild;
	COutputList	m_wndOutputLog;
	CToolShell m_wndBrowser;
	///CWnd m_wndBrowser;
	///IWebBrowser2* m_pBrowserApp;
	///CComVariant *m_pvarURL , *m_pvarself , *m_pvarEmpty; 
	///COutputList m_wndOutputFind;

protected:
///	void FillBuildWindow();
///	void FillDebugWindow();
///	void FillFindWindow();

	void AdjustHorzScroll(CListBox& wndListBox);

// �����Դϴ�.
public:
	virtual ~COutputWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
public:
	int ClearLogWnd();
	int AppendLogMsg(const string& rLog);
	int Navigate2(const CString& rURL);
};

