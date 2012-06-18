
// CableSizingTableChildFrm.cpp : CCableSizingTableChildFrame 클래스의 구현
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

// CCableSizingTableChildFrame 생성/소멸

CCableSizingTableChildFrame::CCableSizingTableChildFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CCableSizingTableChildFrame::~CCableSizingTableChildFrame()
{
}


BOOL CCableSizingTableChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서 Window 클래스 또는 스타일을 수정합니다.
	if( !CMDIChildWndEx::PreCreateWindow(cs) )
		return FALSE;
	cs.lpszName = _T("ELECTRICAL LOAD DATABASE");

	return TRUE;
}

// CCableSizingTableChildFrame 진단

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

// CCableSizingTableChildFrame 메시지 처리기
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
