// BusDiagramChildFrame.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "BusView.h"
#include "BusToBusView.h"
#include "BusDiagramChildFrame.h"

// CBusDiagramChildFrame

IMPLEMENT_DYNCREATE(CBusDiagramChildFrame, CMDIChildWndEx)

CBusDiagramChildFrame::CBusDiagramChildFrame()
{

}

CBusDiagramChildFrame::~CBusDiagramChildFrame()
{
}

BEGIN_MESSAGE_MAP(CBusDiagramChildFrame, CMDIChildWndEx)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CBusDiagramChildFrame message handlers

int CBusDiagramChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	/*
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_BOTTOM
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC;
	CRect rcBorders = CRect(0, 0, 0, 0);
	if (!m_wndDrawToolBar.CreateEx(this, TBSTYLE_FLAT, dwStyle, rcBorders, IDR_DRAW_TOOLBAR) ||
		!m_wndDrawToolBar.LoadToolBar(IDR_DRAW_TOOLBAR))
	{
		AfxMessageBox("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	m_wndDrawToolBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndDrawToolBar);
	*/

	return 0;
}

/**
	@brief	

	@author	HumKyung

	@date	2011.01.25

	@param	lpcs
	@param	pContext

	@return
*/
BOOL CBusDiagramChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	CString sIniFilePath = GetExecPath() + _T("Setting\\ELoad.ini");
	const int iWidth = GetPrivateProfileInt(_T("ELoad") , _T("BusToBus_Width") , 300 , sIniFilePath);

	m_wndSplitter.CreateStatic( this , 1 , 2 ) ;
	m_wndSplitter.CreateView( 0 , 0 , RUNTIME_CLASS(CBusToBusView) , CSize(iWidth,100) , pContext ) ;
	m_wndSplitter.CreateView( 0 , 1 , RUNTIME_CLASS(CBusView) , CSize(300 , 100) , pContext ) ;
	

	return true ;
}