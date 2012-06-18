
// CableSizingTableChildFrm.cpp : CCableSizingTableChildFrame Ŭ������ ����
//

#include "stdafx.h"
#include "ELOAD.h"

#include "CableSizingTableChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CCableSizingTableChildFrame

IMPLEMENT_DYNCREATE(CCableSizingTableChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CCableSizingTableChildFrame, CMDIChildWndEx)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CCableSizingTableChildFrame ����/�Ҹ�

CCableSizingTableChildFrame::CCableSizingTableChildFrame()
{
	// TODO: ���⿡ ��� �ʱ�ȭ �ڵ带 �߰��մϴ�.
}

CCableSizingTableChildFrame::~CCableSizingTableChildFrame()
{
}


BOOL CCableSizingTableChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡�� Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.
	if( !CMDIChildWndEx::PreCreateWindow(cs) )
		return FALSE;
	cs.lpszName = _T("ELECTRICAL LOAD DATABASE");

	return TRUE;
}

// CCableSizingTableChildFrame ����

#ifdef _DEBUG
void CCableSizingTableChildFrame::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CCableSizingTableChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}
#endif //_DEBUG

// CCableSizingTableChildFrame �޽��� ó����
int CCableSizingTableChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.Create(this , TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_BUS_ENTRY))
	{
		return -1;   //Failed to create ribbon bar
	}
	
	return 0;
}
