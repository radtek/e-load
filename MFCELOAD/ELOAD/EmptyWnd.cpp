// EmptyWnd.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "EmptyWnd.h"


// CEmptyWnd

IMPLEMENT_DYNAMIC(CEmptyWnd, CWnd)

CEmptyWnd::CEmptyWnd()
{

}

CEmptyWnd::~CEmptyWnd()
{
}


BEGIN_MESSAGE_MAP(CEmptyWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CEmptyWnd message handlers



void CEmptyWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CWnd::OnPaint() for painting messages
	CRect rect;
	GetClientRect(&rect);
	dc.DrawText("empty" , &rect , DT_SINGLELINE|DT_CENTER|DT_VCENTER);
}

BOOL CEmptyWnd::OnEraseBkgnd(CDC* pDC)
{
	CBrush brush;
	brush.CreateSolidBrush(RGB(255,255,255));

	CRect rect;
	GetClientRect(&rect);
	pDC->FillRect(&rect,&brush);

	return TRUE;
}
