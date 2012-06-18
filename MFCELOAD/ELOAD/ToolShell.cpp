// ToolShell.cpp : implementation file
//

#include "stdafx.h"
#include <assert.h>
#include "ToolShell.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolShell

CToolShell::CToolShell() : m_pCurrentTool(NULL)
{
	const string rLogFilePath = GetExecPath() + "temp\\log.html";
	m_pvarURL = new CComVariant(rLogFilePath.c_str());
	CString self = "_self";
	m_pvarself = new CComVariant(self);
	m_pvarEmpty= new CComVariant;
}

CToolShell::~CToolShell()
{
	try
	{
		if(m_pvarURL)   SAFE_DELETE(m_pvarURL);
		if(m_pvarself)  SAFE_DELETE(m_pvarself);
		if(m_pvarEmpty) SAFE_DELETE(m_pvarEmpty);
	}
	catch(...)
	{
	}
}


BEGIN_MESSAGE_MAP(CToolShell, CWnd)
	//{{AFX_MSG_MAP(CToolShell)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CToolShell message handlers

/**
 * Create:
 *
 * @param pParent 
 * @return void 
 */
void CToolShell::Create(CWnd *pParent)
{
	assert(pParent && "pParent is NULL");

	CRect rect(0,0,38,56);
	CWnd::Create(NULL , "" , WS_CHILD | WS_VISIBLE , rect, pParent, 0x100);
}

/**
	* OnCreate:
	*
	* @param lpCreateStruct 
	* @return int 
 */
int CToolShell::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(TRUE == m_wndBrowser.CreateControl(CLSID_WebBrowser, _T("Error"),WS_CHILD | WS_VISIBLE,CRect(0,0,600,300), this, NULL))
	{
		LPUNKNOWN lpUnk = m_wndBrowser.GetControlUnknown();
		HRESULT hr = lpUnk->QueryInterface(IID_IWebBrowser2, (void**) &m_pBrowserApp);
	}
	
	return 0;
}

/**
	@brief	

	@author BHK	

	@date 2009-03-31 오전 10:30:16	

	@param	rURL log파일 경로

	@return		
*/
int CToolShell::Navigate2(const CString& rURL)
{
	m_pCurrentTool = NULL;
	m_wndBrowser.ShowWindow(SW_SHOW);
	
	*(m_pvarURL) = rURL;
	m_pBrowserApp->Navigate2(m_pvarURL , m_pvarEmpty , m_pvarself , m_pvarEmpty , m_pvarEmpty);
	
	return ERROR_SUCCESS;
}

BOOL CToolShell::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);

	CBrush brush(RGB(177,177,177));
	pDC->FillRect(&rect , &brush);

	return TRUE;
	//return CWnd::OnEraseBkgnd(pDC);
}

/**
	@brief	

	@author BHK	

	@date 2009-04-27 오후 7:31:36	

	@param	

	@return		
*/
void CToolShell::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if(m_wndBrowser.GetSafeHwnd())
	{
		m_wndBrowser.SetWindowPos(NULL , 0 , 0 , cx , cy, SWP_NOMOVE | SWP_NOZORDER);
	}
}
