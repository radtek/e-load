// TreeListCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "TreeListCtrlEx.h"


// CTreeListCtrlEx

IMPLEMENT_DYNAMIC(CTreeListCtrlEx, CTreeListCtrl)

CTreeListCtrlEx::CTreeListCtrlEx()
: m_bDeleteEvent(false)
{

}

CTreeListCtrlEx::~CTreeListCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CTreeListCtrlEx, CTreeListCtrl)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CTreeListCtrlEx::OnTvnSelchanged)
	ON_NOTIFY_REFLECT(NM_CLICK, &CTreeListCtrlEx::OnNMClick)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_CABLE_ROUTE_DELETE, &CTreeListCtrlEx::OnCableRouteDelete)
END_MESSAGE_MAP()



// CTreeListCtrlEx message handlers
void CTreeListCtrlEx::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	POINT pt;
	::GetCursorPos(&pt);
	ScreenToClient(&pt);
	UINT uFlags;
	HTREEITEM hItem = HitTest(point , &uFlags);
	if((NULL != hItem)/* && (TVHT_ONITEM & uFlags)*/)
	{
		CHeaderCtrl* pHeaderCtrl = GetHeaderCtrl();
		const int nItemCount = pHeaderCtrl->GetItemCount();
		for(int i = 0;i < nItemCount;++i)
		{
			CRect rect;
			pHeaderCtrl->GetItemRect(i , &rect);

			if((rect.left<=point.x) && (rect.right>=point.x))
			{
				GetItemRect(hItem , i , &rect , TVHT_ONITEM);
				struct TREELISTINFO tlistinfo;
				tlistinfo.hwndFrom = GetSafeHwnd();
				tlistinfo.idFrom   = GetDlgCtrlID();
				tlistinfo.code     = WM_LBUTTONDBLCLK;
				tlistinfo.hItem    = hItem;
				tlistinfo.nCol     = i;
				tlistinfo.rect     = rect;
				GetParent()->SendMessage(WM_NOTIFY , WPARAM(GetDlgCtrlID()) , LPARAM(&tlistinfo));
			}
		}
	}

	CTreeListCtrl::OnLButtonDblClk(nFlags, point);
}

/**
	@brief	트리에서 이벤트를 넘겨준다.

	@author KHS	

	@return		
**/
void CTreeListCtrlEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CTreeListCtrl::OnLButtonDown(nFlags, point);
}

/**
	@brief	

	@author BHK	

	@date 2009-05-28 오후 1:24:28	

	@param	

	@return		
*/
void CTreeListCtrlEx::OnRButtonUp(UINT nFlags, CPoint point)
{
	POINT pt;
	::GetCursorPos(&pt);
	ScreenToClient(&pt);
	UINT uFlags;
	HTREEITEM hItem = HitTest(point , &uFlags);
	if((NULL != hItem)/* && (TVHT_ONITEM & uFlags)*/)
	{
		CHeaderCtrl* pHeaderCtrl = GetHeaderCtrl();
		const int nItemCount = pHeaderCtrl->GetItemCount();
		for(int i = 0;i < nItemCount;++i)
		{
			CRect rect;
			pHeaderCtrl->GetItemRect(i , &rect);

			if((rect.left<=point.x) && (rect.right>=point.x))
			{
				GetItemRect(hItem , i , &rect , TVHT_ONITEM);
				static struct TREELISTINFO tlistinfo;
				tlistinfo.hwndFrom = GetSafeHwnd();
				tlistinfo.idFrom   = GetDlgCtrlID();
				tlistinfo.code     = WM_RBUTTONUP;
				tlistinfo.hItem    = hItem;
				tlistinfo.nCol     = i;
				tlistinfo.rect     = rect;
				GetParent()->SendMessage(WM_NOTIFY , WPARAM(GetDlgCtrlID()) , LPARAM(&tlistinfo));
			}
		}
	}

	CTreeListCtrl::OnRButtonUp(nFlags, point);
}

void CTreeListCtrlEx::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	GetParent()->SendMessage(WM_LBUTTONDOWN , WPARAM(GetDlgCtrlID()) , LPARAM(&pNMTreeView));

	*pResult = 0;
}

/**
	@brief	TREELIST CTRL에서 빈 공백을 클릭시 아이템 선택 해제

	@author KHS	

	@date 2009-05-15 오후 4:32:32	

	@param	

	@return		
**/
void CTreeListCtrlEx::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint p(GetCurrentMessage()->pt);
	CPoint pt(p);
	ScreenToClient(&pt);
	Select(HitTest(pt), TVGN_CARET);

	*pResult = 0;
}

void CTreeListCtrlEx::OnRButtonDown(UINT nFlags, CPoint point)
{
	if(m_bDeleteEvent)
	{
		CPoint ptTree = point;
		HTREEITEM hTreeItem = HitTest(ptTree);
		if(hTreeItem != NULL)
		{
			SetFocus();
			SelectItem(hTreeItem);

			ClientToScreen(&point);
			
			CMenu menu;
			if(menu.CreatePopupMenu())
			{
				menu.AppendMenu(0 , ID_CABLE_ROUTE_DELETE , _T("Delete"));
				menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, point.y, this);
			}
		}
	}

	//CTreeListCtrl::OnRButtonDown(nFlags, point);
}

/**
	@brief	DELETE EVENT

	@author KHS	

	@date 2009-07-14 오후 4:08:26	

	@param	

	@return		
**/
void CTreeListCtrlEx::OnCableRouteDelete()
{
	GetParent()->SendMessage(WM_COMMAND , ID_CABLE_ROUTE_DELETE);
}

/**
	@brief	DELETE EVENT SETTING

	@author KHS	

	@date 2009-07-14 오후 4:08:13	

	@param	

	@return		
**/
int CTreeListCtrlEx::SetDeleteEvent(bool bDeleteEvent)
{
	m_bDeleteEvent = bDeleteEvent;

	return ERROR_SUCCESS;
}
