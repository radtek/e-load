// ControlCableGridCtrl.cpp : implementation file
//
#include "stdafx.h"
#include "ELOAD.h"
#include "ControlCableGridCtrl.h"


// CControlCableGridCtrl
IMPLEMENT_DYNAMIC(CControlCableGridCtrl, CGridCtrl)

BEGIN_MESSAGE_MAP(CControlCableGridCtrl, CGridCtrl)
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_CONTROLTYPECREATION_INSERT, &CControlCableGridCtrl::OnControlcableInsert)
	ON_COMMAND(ID_CONTROLTYPECREATION_DELETE, &CControlCableGridCtrl::OnControlCableDelete)
	ON_WM_LBUTTONDOWN()
	ON_WM_ENTERIDLE()
END_MESSAGE_MAP()

// CControlTypeCreationGridCtrl message handlers

void CControlCableGridCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_MouseClickCell = GetCellFromPt(point, FALSE);

	/*ClientToScreen(&point);
	if(IsWindowVisible())
	{
		theApp.GetContextMenuManager()->ShowPopupMenu(IDR_CONTROL_TYPE_CREATION_MENU,
			point.x , point.y , this , TRUE);
	}*/
	//CGridCtrl::OnRButtonDown(nFlags, point);
}
void CControlCableGridCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
	//CGridCtrl::OnRButtonUp(nFlags, point);
}

void CControlCableGridCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CGridCtrl::OnLButtonDown(nFlags, point);

	m_MouseClickCell = GetCellFromPt(point, FALSE);

	NMHDR nm;
	nm.hwndFrom = GetSafeHwnd();
	nm.idFrom  = GetDlgCtrlID();
	
	CWnd* pWnd = GetParent();
	if(pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CMFCTabCtrl)))
		GetParent()->GetParent()->SendMessage(WM_NOTIFY, WM_LBUTTONDOWN, (LPARAM)&nm);
	else
		GetParent()->SendMessage(WM_NOTIFY, WM_LBUTTONDOWN, (LPARAM)&nm);
}

void CControlCableGridCtrl::OnControlcableInsert()
{
	NMHDR nm;
	nm.hwndFrom = GetSafeHwnd();
	nm.idFrom  = GetDlgCtrlID();

	CWnd* pWnd = GetParent();
	if(pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CMFCTabCtrl)))
		GetParent()->GetParent()->SendMessage(WM_NOTIFY, ID_CONTROLTYPECREATION_INSERT, (LPARAM)&nm);
	else
		GetParent()->SendMessage(WM_NOTIFY, ID_CONTROLTYPECREATION_INSERT, (LPARAM)&nm);
	
}

void CControlCableGridCtrl::OnControlCableDelete()
{
	NMHDR nm;
	nm.hwndFrom = GetSafeHwnd();
	nm.idFrom  = GetDlgCtrlID();

	CWnd* pWnd = GetParent();
	if(pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CMFCTabCtrl)))
		GetParent()->GetParent()->SendMessage(WM_NOTIFY, ID_CONTROLTYPECREATION_DELETE, (LPARAM)&nm);
	else
		GetParent()->SendMessage(WM_NOTIFY, ID_CONTROLTYPECREATION_DELETE, (LPARAM)&nm);
}

void CControlCableGridCtrl::OnEnterIdle(UINT nWhy, CWnd* pWho)
{

	CGridCtrl::OnEnterIdle(nWhy, pWho);

	// TODO: Add your message handler code here
}

/**
	@brief	GRID 해당 COLUMN에서 같은 텍스트를 찾는다.

	@author KHS	

	@return	 찾으면 TRUE  아니면 FALSE
**/
BOOL CControlCableGridCtrl::FindString(const int& nCol, const CString& rString)
{
	for(int i =1; i < GetRowCount();i++)
	{
		if(rString == GetItemText(i, nCol)) return TRUE;
	}
	return FALSE;
}
