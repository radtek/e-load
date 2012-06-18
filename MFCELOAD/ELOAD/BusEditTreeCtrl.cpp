// BusEditTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ELoad.h"
#include "BusEditTreeCtrl.h"
#include "BusItem.h"
#include "BusGroupItem.h"
#include "ELoadDocData.h"
#include "ELoadItemVIewBar.h"
#include "MainFrm.h"
#include <Assert.h>
#include "AssignLoadToBusDlg.h"
#include "commands\CommandAutoFillOnBusIDChanged.h"
#include <sstream>

static BOOL _HitF2_ = FALSE;
#define	NOT_ASSIGNEDED_COLOR	RGB(100,100,100)

// CBusEditTreeCtrl

IMPLEMENT_DYNAMIC(CBusEditTreeCtrl, CEditTreeCtrl)

CBusEditTreeCtrl::CBusEditTreeCtrl() : m_pObserver(NULL)
, m_rPreBusItemName(_T("")) , m_rActionString(_T("SHOW BUS"))
{
	///bLbuttonClick = false;

	m_pObserver = new CObserver(this , &CBusEditTreeCtrl::OnEventFromObserver);
}

CBusEditTreeCtrl::~CBusEditTreeCtrl()
{
	try
	{
		if(m_pObserver) delete m_pObserver;
		m_pObserver = NULL;
	}
	catch(...)
	{
	}
}


BEGIN_MESSAGE_MAP(CBusEditTreeCtrl, CEditTreeCtrl)
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_CREATE_BUS, &CBusEditTreeCtrl::OnCreateBus)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_COPYBUS, &CBusEditTreeCtrl::OnCopyBusGroup)
	ON_COMMAND(ID_BUS_COPY, &CBusEditTreeCtrl::OnCopyBus)
	ON_COMMAND(ID_DELETE_BUS_GROUP, &CBusEditTreeCtrl::OnDeleteBusGroup)
	ON_COMMAND(ID_BUS_DELETE, &CBusEditTreeCtrl::OnDeleteBus)
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, &CBusEditTreeCtrl::OnTvnBeginlabeledit)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, &CBusEditTreeCtrl::OnTvnEndlabeledit)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CBusEditTreeCtrl::OnTvnSelchanged)
END_MESSAGE_MAP()

// CBusEditTreeCtrl message handlers
/**
	@brief	이름이 INCOMING인 BUS와 BUS GROUP , SUBBUS인 BUS GROUP은 이동이 불가하다.

	@author BHK	

	@date 2009-04-03 오후 11:32:44	

	@param	

	@return		
**/
bool CBusEditTreeCtrl::CanDragItem(TVITEM & item)
{
	if(item.hItem)
	{
		HTREEITEM hParent = GetParentItem(item.hItem);
		if(hParent && INCOMING == GetItemText(hParent) || (INCOMING == GetItemText(item.hItem))) return false;
		if(SUBBUS == GetItemText(item.hItem)) return false;
	}

	return true;
}

/**
	@brief	INCOMING , SUBBUS위에는 DROP할수 없다.

	@author BHK	

	@date 2009-04-10 오후 2:52:31	

	@param	

	@return		
*/
bool CBusEditTreeCtrl::CanDropItem(HTREEITEM hDrag, HTREEITEM hDrop, EDropHint hint)
{
	///assert(hDrag && hDrop && "hDrag or hDrop is NULL");
	if((NULL != hDrag) && (NULL != hDrop))
	{
		if(hDrag == hDrop) return false;

		//! INCOMING , SUBBUS위에는 DROP할수 없다.
		HTREEITEM hParent = GetParentItem(hDrop);
		if(hParent && INCOMING == GetItemText(hParent) || (INCOMING == GetItemText(hDrop))) return false;
		if(SUBBUS == GetItemText(hDrop)) return false;

		CELoadItem* pDragItemData = (CELoadItem*)GetItemData(hDrag);
		CELoadItem* pDropItemData = (CELoadItem*)GetItemData(hDrop);

		const string rDragString = pDragItemData->GetTypeString().c_str();
		const string rDropString = pDropItemData->GetTypeString().c_str();
		if((CBusItem::TypeString() == rDragString) && (CBusItem::TypeString() == rDropString)) 
		{
			if(DROP_CHILD == hint) 
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		else if((CBusItem::TypeString() == rDragString) && (CBusGroupItem::TypeString() == rDropString)) 
		{
			if(DROP_CHILD == hint) 
			{
				CString rDropText = GetItemText(hDrop);
				return (NOT_ASSIGNED == rDropText) ? false : true;
			}
			else
				return false;
		}
		else if((CBusGroupItem::TypeString() == rDragString) && (CBusItem::TypeString() == rDropString))
		{
			return false;
		}

		else if((CBusGroupItem::TypeString() == rDragString) && (CBusGroupItem::TypeString() == rDropString))
		{
			if(DROP_CHILD == hint) 
			{
				return false;
			}
			else
			{
				CString rDragItemText = GetItemText(hDrag);
				CString rDropItemText = GetItemText(hDrop);
				if( NOT_ASSIGNED == rDropItemText || NOT_ASSIGNED == rDragItemText)
				{
					return false;
				}
				else
				{
					return true;
				}
			}
		}

		return true;
		/*enum EDropHint
		{
		DROP_BELOW,
		DROP_ABOVE,
		DROP_CHILD,
		DROP_NODROP
		};*/
	}
	return false;
}
/**
	@brief  BusItem move after Drag & Drop , BUS_GROUP은 이동만하고 BUS는 그룹이동을 한다.

	@author  KHS,BHK

	@date	????.??.??

	@param

	@return
*/
void CBusEditTreeCtrl::DragEnd()
{
	//! CHANGE BUS_GROUP of BUS
	HTREEITEM hDrag = m_pDragData->GetDragItem();
	HTREEITEM hDrop = m_pDragData->GetDropTarget();

	//! 드랍 실패시 종료
	EDropHint eHint;
	HTREEITEM hDropItem = GetDropTarget(eHint);
	if(!CanDropItem(hDrag, hDropItem, eHint))
	{
		CEditTreeCtrl::DragEnd();
		return;
	}

	CELoadItem* pELoadItem = (CELoadItem*)GetItemData(hDrag);
	CBusItem* pBusItem = (CBusItem*)GetItemData(hDrag);
	HTREEITEM hDragParent = GetParentItem(hDrag);
	HTREEITEM hDropParent = GetParentItem(hDrop);
	if( (NULL != hDragParent) && (NULL != hDrop) && (hDropParent != hDragParent))
	{
		CBusGroupItem* pBusGroupItem = (CBusGroupItem*)GetItemData(hDragParent);

		CString rDropParent;
		CBusGroupItem* pDropParent = NULL;
		if(NULL == hDropParent)
		{
			rDropParent = GetItemText(hDrop);
			pDropParent = (CBusGroupItem*)GetItemData(hDrop);
		}
		else
		{
			rDropParent = GetItemText(hDropParent);
			pDropParent = (CBusGroupItem*)GetItemData(hDropParent);
		}

		CELoadDocData& docData = CELoadDocData::GetInstance();

		//! BUS GROUP은 CONTINUE...
	}

	CEditTreeCtrl::DragEnd();
	SetOrderNo();   //! BUS 보여지는 순서를 SORTING한다.
	//! OBSERVER를 통해 새로운 BUS 혹은 BUS_GROUP의 .ORDER NO가 변경되었다는 것을 알린다.
	if(m_pObserver)
	{
		CEventData eventData(pELoadItem->GetTypeString() , CEventData::REORDER_ITEM);
		eventData.m_SelectionSet.Add(pELoadItem);
		m_pObserver->FireEvent(eventData);
	}
	SelectedItemPropView();
}

/**
	@brief  BUS TREE R Button Click

	@author	BHK
*/
void CBusEditTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if(_T("SHOW BUS") == m_rActionString)
	{
		// Select clicked item:
		CPoint ptTree = point;
		HTREEITEM hTreeItem = HitTest (ptTree);
		if (hTreeItem != NULL)
		{       
			SetFocus();
			SelectItem (hTreeItem);
		}
		ClientToScreen(&point);

		HTREEITEM hItem = GetSelectedItem();
		///! Parents Tree
		if(NULL == GetParentItem(hItem))
		{
			theApp.GetContextMenuManager()->ShowPopupMenu(IDR_CONTEXT_BUS_MENU,
				point.x, point.y, this, TRUE);
		}
		//! BUS
		else
		{
			theApp.GetContextMenuManager()->ShowPopupMenu(IDR_BUS_COPY_MENU,
				point.x, point.y, this, TRUE);
		}
	}

	//CEditTreeCtrl::OnRButtonDown(nFlags, point);
}

void CBusEditTreeCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
}

/**
	@brief	create new bus item.

	@author	HumKyung

	@date	????.??.??

	@param	

	@return	
*/
void CBusEditTreeCtrl::OnCreateBus()
{
	//! BUS 이름 중복 체크
	string rText = UNNAMED_STR;
	
	CELoadDocData& docData = CELoadDocData::GetInstance();
	if(docData.IsExistItemName(CBusItem::TypeString() , rText))
	{
		MessageBox(_T("The name exists already!! "), _T("Warning!") , MB_OK|MB_ICONWARNING );
		return;
	}

	HTREEITEM hItem = GetSelectedItem();
	CString rItemText = GetItemText(hItem);
	if(hItem)
	{
		list<CELoadItem*> pBusGroupList;
		docData.GetELoadItemListOf(pBusGroupList , CBusGroupItem::TypeString());
		if(!pBusGroupList.empty())
		{
			CBusItem* pBusItem = static_cast<CBusItem*>(docData.CreateELoadItemOf(CBusItem::TypeString()));
			if(pBusItem)
			{
				CELoadItemProp* pItemProp = pBusItem->prop();
				pItemProp->SetOriginalValue(_T("GENERAL") , _T("BUS ID"), UNNAMED_STR);
				pItemProp->SetValue(_T("GENERAL") , _T("BUS ID"), UNNAMED_STR);

				CELoadItem* pItem = (CELoadItem*)GetItemData(hItem);
				if(pItem->IsKindOf(CBusGroupItem::TypeString()))
				{
					CBusGroupItem* pBusGroupItem = static_cast<CBusGroupItem*>(pItem);
					pItemProp->SetOriginalValue(_T("GENERAL") , _T("BUS GROUP") , pBusGroupItem->GetName());
					pItemProp->SetValue(_T("GENERAL") , _T("BUS GROUP") , pBusGroupItem->GetName());

					if(pBusGroupItem)
					{
						//! Bus Group에서 가져온다.
						pItemProp->SetValue(_T("General") , _T("Bus Voltage") , 
							pBusGroupItem->prop()->GetValue(_T("General") , _T("Bus Voltage")));
						pItemProp->SetValue(_T("General") , _T("Load Voltage") , 
							pBusGroupItem->prop()->GetValue(_T("General") , _T("Load Voltage")));
						pItemProp->SetValue(_T("General") , _T("Phase") , 
							pBusGroupItem->prop()->GetValue(_T("General") , _T("Phase")));
						pItemProp->SetValue(_T("General") , _T("Description") , 
							pBusGroupItem->prop()->GetValue(_T("General") , _T("Description")));

						pBusGroupItem->AddBus(pBusItem);
						pBusItem->SetBusGroupItemPtr( pBusGroupItem );
						int res = AddBusTreeCtrlItem( pBusItem );
					}
					SetOrderNo();   //! BUS 보여지는 순서를 SORTING한다.
					//! OBSERVER를 통해 새로운 BUS가 생성되었다는 것을 알린다.
					if(m_pObserver)
					{
						CEventData eventData(CBusItem::TypeString() , CEventData::ADD_ITEM);
						eventData.m_SelectionSet.Add(pBusItem);
						m_pObserver->FireEvent(eventData);
					}

					//! EDITLABEL
					SendMessage(WM_KEYDOWN,VK_F2,0);
					SelectedItemPropView();

					//! 속성창을 띄운다.
					CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
					if(!pFrame->m_wndPropertiesBar.IsVisible())
					{
						pFrame->m_wndPropertiesBar.ShowPane(TRUE , FALSE , TRUE);
					}
				}
			}
		}
	}
	else
	{
		//AfxMessageBox("Not Found, BUS GROUP !");
		//return;
	}
}

/**
	@brief	add bus item to treectrl

	@author	BHK
*/
int CBusEditTreeCtrl::AddBusTreeCtrlItem( CBusItem* pBusItem )
{
	assert(pBusItem && "pBusItem is NULL");

	if(pBusItem)
	{
		HTREEITEM hRoot = GetRootItem();
		CString rRootItem = GetItemText(hRoot);
		if(NULL != hRoot)
		{
			HTREEITEM hBusGroupItem = FindBusGroupItem( pBusItem->GetBusGroupItemPtr() ,hRoot);
			if(NULL != hBusGroupItem)
			{
				CELoadItemProp* pItemProp = pBusItem->prop();
				string rBusId = pItemProp->GetValue(_T("GENERAL") , _T("BUS ID"));
				const CString rItemText = GetItemText(hBusGroupItem);

				TV_INSERTSTRUCT TI;
				TI.hParent     = hBusGroupItem;
				TI.item.iImage = (SUBBUS == rItemText) ? SUBBUS_ICON : BUS_ICON;
				TI.item.iSelectedImage = (SUBBUS == rItemText) ? SUBBUS_ICON : BUS_ICON;
				TI.item.mask   = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
				TI.item.pszText = (LPSTR)( rBusId.c_str() );
				HTREEITEM hItem = InsertItem(&TI);
				if(hItem)
				{
					SetItemData( hItem , (DWORD_PTR)pBusItem);
					/*if(NOT_ASSIGNED == GetItemText(hBusGroupItem))
					SetItemColor(hItem,NOT_ASSIGNEDED_COLOR);*/
				}
				SetFocus();
				SelectItem(hItem);

				//! fire event
				CEventData EventData(pBusItem->GetTypeString(), CEventData::SELECT_ITEM);
				EventData.m_pFromObserver = m_pObserver;
				EventData.Add( pBusItem );
				m_pObserver->FireEvent(EventData);

				Expand(hBusGroupItem, TVE_EXPAND);
			}

			return ERROR_SUCCESS;
		}
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	add bus group item to treectrl

	@author	
*/
HTREEITEM CBusEditTreeCtrl::AddBusGroupTreeCtrlItem( CBusGroupItem* pBusGroupItem )
{
	assert(pBusGroupItem && "pBusGroupItem is NULL");
	HTREEITEM hItem = NULL;

	if(pBusGroupItem)
	{
		CELoadItemProp* pItemProp = pBusGroupItem->prop();
		string rBusGroupId = pItemProp->GetValue("GENERAL","BUS GROUP ID");

		TV_INSERTSTRUCT TI;
		CString rItemText;
		HTREEITEM hPrevSiblingItem = GetRootItem();
		HTREEITEM hSelectedItem    = GetSelectedItem();

		CString TESTSelItemTExt = GetItemText(hSelectedItem);
		HTREEITEM PPP = GetParentItem(hSelectedItem);
		CString PARENTTEXT = GetItemText(PPP);

		if(GetParentItem(hSelectedItem)) hSelectedItem = GetParentItem(hSelectedItem);
		if(NULL != hSelectedItem)
		{
			rItemText = GetItemText(hSelectedItem);
			hPrevSiblingItem = GetPrevSiblingItem(hSelectedItem);
			if(!hPrevSiblingItem) hPrevSiblingItem = TVI_FIRST;
		}
		TI.hInsertAfter = (SUBBUS == rItemText) ? hPrevSiblingItem : hSelectedItem;

		TI.hParent      = NULL;
		TI.item.iImage  = BUS_GROUP_ICON;
		TI.item.iSelectedImage = BUS_GROUP_ICON;
		TI.item.mask    = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		TI.item.pszText = (LPSTR)( rBusGroupId.c_str() );
		hItem = InsertItem(&TI);
		if(hItem)
		{
			SetItemData( hItem , (DWORD_PTR)pBusGroupItem);
		}
		SetFocus();
		SelectItem(hItem);
		SelectSetFirstVisible(hItem);

		//! fire event
		CEventData EventData(pBusGroupItem->GetTypeString() , CEventData::SELECT_ITEM);
		EventData.Add( pBusGroupItem );
		m_pObserver->FireEvent(EventData);

		return hItem;
	}

	return hItem;
}

/**
	@brief	BUS GROUP의 HTREEITEM을 구한다.

	@author	BHK
*/
HTREEITEM CBusEditTreeCtrl::FindBusGroupItem( CBusGroupItem* pBusGroupItem , HTREEITEM hItem )
{
	assert(pBusGroupItem && "pBusGroupItem is NULL");
	assert(hItem && "hItem is NULL");

	if(pBusGroupItem && hItem )
	{
		HTREEITEM hSiblingItem = hItem;
		while(hSiblingItem)
		{
			CBusGroupItem* ptr = (CBusGroupItem*)GetItemData(hSiblingItem);
			if(ptr == pBusGroupItem) return hSiblingItem;

			HTREEITEM hChild = GetChildItem(hSiblingItem);
			if(NULL != hChild)
			{
				CBusGroupItem* ptr = (CBusGroupItem*)GetItemData(hSiblingItem);
				if((NULL != ptr) && (pBusGroupItem == ptr)) return hSiblingItem;

				HTREEITEM hBusGroupItem = FindBusGroupItem( pBusGroupItem , hChild );
				if(NULL != hBusGroupItem) return hBusGroupItem;
			}
			hSiblingItem = GetNextSiblingItem(hSiblingItem);
		}
	}

	return NULL;
}

/**
	@brief  속성창을 띄운다, 선택 아이템의 속성창을 바꿔준다.
*/
void CBusEditTreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if("SHOW BUS" == m_rActionString)
	{
		HTREEITEM hTreeItem = GetSelectedItem();
		if(NULL == hTreeItem) return;
		CString rText = GetItemText(hTreeItem);
		CELoadDocData& docData = CELoadDocData::GetInstance();

		HTREEITEM hItem = GetSelectedItem();
		///! Parents Tree
		if(NULL == GetParentItem(hItem))
		{
			if((NOT_ASSIGNED == rText) || (SUBBUS == rText) || (INCOMING == rText)) return;
			//! BUS GROUP
			CELoadItem* pItem = (CELoadItem*)GetItemData(hItem);
			if(pItem->IsKindOf(CBusGroupItem::TypeString()))
			{
				CBusGroupItem* pBusGroupItem = static_cast<CBusGroupItem*>(pItem);

				CEventData EventData(pBusGroupItem->TypeString(), CEventData::SELECT_ITEM);
				EventData.Add( pBusGroupItem );
				m_pObserver->FireEvent(EventData);
			}
		}
		else
		{
			//! BUS
			CELoadItem* pItem = (CELoadItem*)GetItemData(hItem);
			if(pItem->IsKindOf(CBusItem::TypeString()))
			{
				CBusItem* pBusItem = static_cast<CBusItem*>(pItem);

				CEventData EventData(pBusItem->TypeString() , CEventData::SELECT_ITEM);
				EventData.Add( pBusItem );
				m_pObserver->FireEvent(EventData);
			}

		}
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if(!pFrame->m_wndPropertiesBar.IsVisible())
		{
			pFrame->m_wndPropertiesBar.ShowPane(TRUE , FALSE , TRUE);
		}
		//! 뷰를 닫는다
		//CELOADApp* pApp = (CELOADApp*)AfxGetApp();
		//pApp->CloseDocument("BUS");
	}
	else if("SHOW LOAD SUMMARY IN BUS" == m_rActionString)
	{
	}

	//CEditTreeCtrl::OnLButtonDblClk(nFlags, point);
}
/**
	@brief  선택한 아이템의 속성창 리스트 변경

*/
void CBusEditTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if("SHOW BUS" == m_rActionString)
	{
		CString rText;
		CPoint ptTree = point;
		HTREEITEM hTreeItem = HitTest (ptTree);
		if (hTreeItem != NULL)
		{       
			SetFocus();
			SelectItem (hTreeItem);
			rText = GetItemText(hTreeItem);
		}

		CELoadDocData& docData = CELoadDocData::GetInstance();
		SelectedItemPropView();
	}

	CEditTreeCtrl::OnLButtonDown(nFlags, point);
}
/**
	@brief  BUS_GROUP과 하위  BUS를 COPY
*/
void CBusEditTreeCtrl::OnCopyBusGroup()
{
	// TODO: Add your command handler code here
	HTREEITEM hItem = GetSelectedItem();
	CString rItemText = GetItemText(hItem);
	//! BUS 아이템 이름 중복 체크
	string rCopyItemText = string(rItemText.operator LPCSTR()) + "_copy";
	CELoadDocData& docData = CELoadDocData::GetInstance();
	if(docData.IsExistItemName(CBusGroupItem::TypeString() , rCopyItemText))
	{
		MessageBox("The name exists already!! ", "Warning!", MB_OK|MB_ICONWARNING );
		return;
	}
	if (hItem /*&& NOT_ASSIGNED != rItemText*/ && (SUBBUS != rItemText) && (INCOMING != rItemText))
	{
		SetFocus();
		HTREEITEM hChildItem = GetChildItem(hItem);

		CELoadDocData& docData = CELoadDocData::GetInstance();
		///! BUS_GROUP
		if(NULL == GetParentItem(hItem))
		{
			CELoadItem* pItem = (CELoadItem*)GetItemData(hItem);
			if(pItem->IsKindOf(CBusGroupItem::TypeString()))
			{
				CBusGroupItem* pBusGroupItem = static_cast<CBusGroupItem*>(pItem);
				if(pBusGroupItem)
				{
					CELoadItem* pCopiedBusGroupItem = pBusGroupItem->Clone();
					string rBusGroupName = pCopiedBusGroupItem->GetName();
					rBusGroupName+= _T("_copy");
					pCopiedBusGroupItem->SetName(rBusGroupName);
					///docData.m_BusGroupEntry.push_back((CBusGroupItem*)pCopyBusGroupItem);
					AddBusGroupTreeCtrlItem((CBusGroupItem*)pCopiedBusGroupItem);

					SetOrderNo();
					//! OBSERVER를 통해 새로운 BUS GROUP이 생성되었다는 것을 알린다.
					if(m_pObserver)
					{
						CEventData eventData(CBusGroupItem::TypeString() , CEventData::ADD_ITEM);
						eventData.m_SelectionSet.Add(pCopiedBusGroupItem);
						m_pObserver->FireEvent(eventData);
					}
					SelectedItemPropView();

					//! EDITLABEL
					SendMessage(WM_KEYDOWN,VK_F2,0);

					//! 속성창을 띄운다.
					CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
					if(!pFrame->m_wndPropertiesBar.IsVisible())
					{
						pFrame->m_wndPropertiesBar.ShowPane(TRUE , FALSE , TRUE);
					}
				}
			}
		}
	}

}
/**
	@brief  BUS COPY
*/
void CBusEditTreeCtrl::OnCopyBus()
{
	// TODO: Add your command handler code here
	HTREEITEM hItem = GetSelectedItem();
	CString rItemText = GetItemText(hItem);

	//! 아이템 이름 중복 체크
	string rCopyItemText = string(rItemText.operator LPCSTR()) + "_copy";
	CELoadDocData& docData = CELoadDocData::GetInstance();
	if(docData.IsExistItemName(CBusItem::TypeString() , rCopyItemText))
	{
		MessageBox("The name exists already!! ", "Warning!", MB_OK|MB_ICONWARNING );
		return;
	}

	if(hItem )
	{
		SetFocus();
		HTREEITEM hParent = GetParentItem(hItem);

		CELoadItem* pItem = (CELoadItem*)GetItemData(hItem);
		CELoadItem* pParentItem = (CELoadItem*)GetItemData(hParent);
		if(pItem->IsKindOf(CBusItem::TypeString()))
		{
			CBusItem* pBusItem = static_cast<CBusItem*>(pItem);
			if(pBusItem)
			{
				CELoadItem* pCopyBusItem = pBusItem->Clone();
				string rBusName = pCopyBusItem->GetName();
				rBusName += _T("_copy");
				pCopyBusItem->SetName(rBusName);
				CBusGroupItem* pBusGroupItem = static_cast<CBusGroupItem*>(pParentItem);
				if(pBusGroupItem)
				{
					pBusGroupItem->AddBus((CBusItem*)pCopyBusItem);
					((CBusItem*)pCopyBusItem)->SetBusGroupItemPtr( pBusGroupItem );
					const int res = AddBusTreeCtrlItem((CBusItem*)pCopyBusItem);
					
					SetOrderNo();
					//! OBSERVER를 통해 새로운 BUS가 생성되었다는 것을 알린다.
					if(m_pObserver)
					{
						CEventData eventData(CBusItem::TypeString() , CEventData::ADD_ITEM);
						eventData.m_SelectionSet.Add(pCopyBusItem);
						m_pObserver->FireEvent(eventData);
					}
					SelectedItemPropView();

					//!EDITLABEL
					SendMessage(WM_KEYDOWN,VK_F2,0);

					//! 속성창을 띄운다.
					CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
					if(!pFrame->m_wndPropertiesBar.IsVisible())
					{
						pFrame->m_wndPropertiesBar.ShowPane(TRUE , FALSE , TRUE);
					}
				}
			}
		}
	}
}

/**
	@brief	Delete BUS_GROUP(SUBBUS , INCOMING 은 삭제하지 못하도록 한다.)

	@author KHS,HumKyung

	@date	????.??.??

	@param

	@return
*/
void CBusEditTreeCtrl::OnDeleteBusGroup()
{
	HTREEITEM hParent = GetSelectedItem();
	//! 삭제될 BUS_GROUP의 자식들을 복사해서 붙인다.
	if (hParent)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();

		const string rItemText = GetItemText(hParent);
		list<string>::iterator where = find(docData.m_lstSystemBusGroup.begin() , docData.m_lstSystemBusGroup.end() , rItemText);
		if(where != docData.m_lstSystemBusGroup.end())
		{
			AfxMessageBox( docData.GetStringOf(66).c_str());
			return;
		}

		if( IDYES != MessageBox(docData.GetStringOf(67).c_str() , _T("Warning!") , MB_YESNO|MB_ICONWARNING ))
		{
			return;
		}

		SetFocus();
		HTREEITEM hChildItem = GetChildItem(hParent);

		CELoadItem *pParent    = (CELoadItem*)GetItemData(hParent);
		if(pParent->IsKindOf(CBusGroupItem::TypeString()))
		{
			CBusGroupItem *pBusGroupItem = static_cast<CBusGroupItem*>(pParent);
			if(pBusGroupItem)
			{
				{
					//! DELETE FLAG를 TRUE로 설정한다.
					for(vector<CBusItem*>::iterator itr = pBusGroupItem->m_BusRefEntry.begin();itr != pBusGroupItem->m_BusRefEntry.end();++itr)
					{
						(*itr)->SetDeleteFlag(true);
					}
				}

				//!! 마지막으로 삭제한다.
				CELoadDocData& docData = CELoadDocData::GetInstance();
				pBusGroupItem->SetDeleteFlag(true);
				DeleteItem(hParent);
				SetOrderNo();
				//! OBSERVER를 통해 BUS GROUP이 삭제되었다는 것을 알린다.
				if(m_pObserver)
				{
					CEventData eventData(CBusGroupItem::TypeString() , CEventData::DELETE_ITEM);
					eventData.m_SelectionSet.Add(pBusGroupItem);
					m_pObserver->FireEvent(eventData);
				}

				//! 속성창이 열려있으면 닫는다.
				CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
				if(pFrame->m_wndPropertiesBar.IsVisible())
				{
					pFrame->m_wndPropertiesBar.ShowPane(FALSE , FALSE , FALSE);
				}
			}
		}
	}
}

/**
	@brief	Delete BUS

	@author	KHS
*/
void CBusEditTreeCtrl::OnDeleteBus()
{
	if( IDYES != MessageBox("Delete Bus, Really? ", "Warning!", MB_YESNO|MB_ICONWARNING ))
	{
		return;
	}
	HTREEITEM hItem = GetSelectedItem();
	HTREEITEM hParentItem = GetParentItem(hItem);
	if (hParentItem )
	{
		CELoadItem *pItem = (CELoadItem*)GetItemData(hItem);
		CELoadItem * pParentItem = (CELoadItem*)GetItemData(hParentItem);
		if(pItem->IsKindOf(CBusItem::TypeString()) && pParentItem->IsKindOf(CBusGroupItem::TypeString()))
		{
			CBusGroupItem* pBusGroupItem = static_cast<CBusGroupItem*>(pParentItem);
			CBusItem* pBusItem = static_cast<CBusItem*>(pItem);
			pBusItem->SetDeleteFlag(true);
			DeleteItem(hItem);
			SetOrderNo();

			//! 속성창이 열려있으면 닫는다.
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			if(pFrame->m_wndPropertiesBar.IsVisible())
			{
				pFrame->m_wndPropertiesBar.ShowPane(FALSE , FALSE , FALSE);
			}

			//! OBSERVER를 통해 BUS가 삭제되었다는 것을 알린다.
			if(m_pObserver)
			{
				CEventData eventData(CBusItem::TypeString() , CEventData::DELETE_ITEM);
				eventData.m_SelectionSet.Add(pBusItem);
				m_pObserver->FireEvent(eventData);
			}
		}
	}
}
/**
	@brief  KEYDOWN

	@author KHS
*/
void CBusEditTreeCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if (IsWindowVisible())
	{
		switch (nChar)
		{
		case VK_F2:
			{
				_HitF2_ = TRUE;
				HTREEITEM hSelItem = GetSelectedItem();
				PostMessage ( TVM_EDITLABEL, 0 , (LPARAM)hSelItem );
			}
			break;
		}
	}

	CEditTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

/**
	@brief  아이템을 삭제한다.

	@author KHS
*/
void CBusEditTreeCtrl::DeleteKeyDown(void)
{
	HTREEITEM hItem = GetSelectedItem();
	CEdit* pEdit = this->GetEditControl();
	if(pEdit == NULL)
	{
		HTREEITEM hParentItem = GetParentItem(hItem);
		if(hParentItem)
		{
			OnDeleteBus();
		}
		else
		{
			OnDeleteBusGroup();
		}
	}
}
/**
	@brief  Edit Label start
*/
void CBusEditTreeCtrl::OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: Add your control notification handler code here
	//! 변경전의 Bus Name을 저장한다.
	LPCSTR lszText = pTVDispInfo->item.pszText;
	m_rPreBusItemName = lszText;
	*pResult = (TRUE == _HitF2_) ? 0 : 1;

	_HitF2_ = FALSE;
	///*pResult = 0;
}

/**
	@brief  Edit Label End

	@author	BHK
*/
void CBusEditTreeCtrl::OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);

	SetFocus();
	LPCSTR lszText = pTVDispInfo->item.pszText;

	//! COPY, NEW 생성시 BUS ID 변경 안했을 시에 종료
	if(NULL == lszText) return;

	HTREEITEM hItem = GetSelectedItem();
	CString rItemText = GetItemText(hItem);
	if(lszText && (rItemText != SUBBUS) && (rItemText != INCOMING))
	{
		HTREEITEM hItem = GetSelectedItem();
		if(hItem)
		{
			CELoadDocData& docData = CELoadDocData::GetInstance();

			CELoadItem* pItem = (CELoadItem*)GetItemData(hItem);
			if(pItem->IsKindOf(CBusItem::TypeString()))
			{
				//! BUS 아이템 이름 중복 체크
				if(docData.IsExistItemName(CBusItem::TypeString() , lszText))
				{
					MessageBox(_T("The name exists already!! ") , _T("Warning!") , MB_OK|MB_ICONWARNING );
					return;
				}

				CBusItem* pBusItem = static_cast<CBusItem*>(pItem);
				if(pBusItem)
				{
					//! BUS ID가 변경되었을때 미치는 다른 것들을 처리한다.
					{
						COMMAND::CCommandAutoFillOnBusIDChanged cmd;
						cmd.m_SelectionSet.Add(pBusItem);
						cmd.m_rNewValue = string(lszText);
						cmd.Execute(false);
					}
					pBusItem->SetName(string(lszText));

					CEventData EventData(pBusItem->GetTypeString(), CEventData::MODIFY_ITEM_PROP);
					EventData.Add( pBusItem );
					m_pObserver->FireEvent(EventData);
				}
			}
			else if( pItem->IsKindOf(CBusGroupItem::TypeString()))
			{
				//! BUS 아이템 이름 중복 체크
				if(docData.IsExistItemName(CBusGroupItem::TypeString() , lszText))
				{
					MessageBox(_T("The name exists already!! "), _T("Warning!") , MB_OK|MB_ICONWARNING );
					return;
				}

				CBusGroupItem* pBusGroupItem = static_cast<CBusGroupItem*>(pItem);
				if(pBusGroupItem)
				{
					pBusGroupItem->SetName(lszText);

					CEventData EventData(pBusGroupItem->GetTypeString(), CEventData::MODIFY_ITEM_PROP);
					EventData.Add( pBusGroupItem );
					m_pObserver->FireEvent(EventData);
				}
			}
			SetItemText(hItem , lszText);
		}
	}
	*pResult = 0;
}

/**
	@brief  UPDATE BUS TREE
*/
int CBusEditTreeCtrl::UpdateBusTreeView(HTREEITEM hItem , CELoadItem* pItem )
{
	while(hItem)
	{
		if(pItem == (CELoadItem*)GetItemData(hItem))
		{
			SetItemText(hItem , pItem->GetName().c_str());
			return ERROR_SUCCESS;
		}

		HTREEITEM hChild = GetChildItem(hItem);
		if(hChild) UpdateBusTreeView(hChild , pItem);
		hItem = GetNextSiblingItem(hItem);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-14 오후 11:25:04	

	@param	

	@return		
**/
int CBusEditTreeCtrl::OnEventFromObserver(CObserver* pObserver , CEventData* pEventData)
{
	if(CEventData::MODIFY_ITEM_PROP == pEventData->m_eEventType)
	{
		if(_T("BUS_GROUP") == pEventData->GetItemCategory())
		{
			const int nItemCount = pEventData->GetItemCount();
			for(int i = 0;i < nItemCount;++i)
			{
				HTREEITEM hItem = ((CBusEditTreeCtrl*)(pObserver->owner()))->GetRootItem();
				((CBusEditTreeCtrl*)(pObserver->owner()))->UpdateBusTreeView( hItem , pEventData->GetItemAt(i) );
			}
		}
		else if(_T("BUS") ==  pEventData->GetItemCategory())
		{
			const int nItemCount = pEventData->GetItemCount();
			for(int i = 0;i < nItemCount;++i)
			{
				HTREEITEM hItem = ((CBusEditTreeCtrl*)(pObserver->owner()))->GetRootItem();
				((CBusEditTreeCtrl*)(pObserver->owner()))->UpdateBusTreeView( hItem , pEventData->GetItemAt(i) );
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief  control이 DELETE키를 처리하기 앞서 DeleteKeyDown함수를 호출한다.

	@author BHK
*/
BOOL CBusEditTreeCtrl::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)   // 키보드가 눌렸을때 처리
	{
		switch (pMsg->wParam)
		{ 
		case VK_DELETE:
			{
				DeleteKeyDown();
				return 0;
			}

		}

	}

	return CEditTreeCtrl::PreTranslateMessage(pMsg);
}
/**
	@brief  Order No를 설정 한다.

	@author	BHK
*/
int CBusEditTreeCtrl::SetOrderNo(void)
{
	HTREEITEM hRootItem = GetRootItem();
	if(hRootItem)
	{
		int nBusGroupIndex = 0;
		HTREEITEM hBusGroupSiblingItem = hRootItem;
		do
		{
			///CString rRoot = GetItemText(hRootItem);
			CELoadItem *pItem = (CELoadItem*)GetItemData(hBusGroupSiblingItem);
			if( pItem->IsKindOf(CBusGroupItem::TypeString()))
			{
				CBusGroupItem* pBusGroupItem = static_cast<CBusGroupItem*>(pItem);
				pBusGroupItem->ClearAllBusItems();

				CELoadItemProp* pItemProp = pBusGroupItem->prop();
				const string rBusGroupID = pItemProp->GetValue(_T("General") , _T("Bus Group ID"));
				CString rString;
				rString.Format(_T("%d") , ++nBusGroupIndex);
				pItemProp->SetValue(_T("GENERAL") , _T("ORDER NO") , rString.operator LPCSTR());

				//! fire event
				/*
				CEventData EventData(pBusGroupItem->GetTypeString(), CEventData::MODIFY_ITEM_PROP);
				EventData.m_pFromObserver = m_pObserver;
				EventData.Add( pBusGroupItem );
				m_pObserver->FireEvent(EventData);
				*/

				HTREEITEM hBusItem = GetChildItem(hBusGroupSiblingItem);
				if(hBusItem)
				{
					int nBusIndex = 0;
					HTREEITEM hBusSiblingItem = hBusItem;
					do
					{
						CELoadItem *pItem = (CELoadItem*)GetItemData(hBusSiblingItem);
						if( pItem->IsKindOf(CBusItem::TypeString()))
						{
							CBusItem* pBusItem = static_cast<CBusItem*>(pItem);
							pBusGroupItem->AddBus(pBusItem);

							CELoadItemProp* pItemProp = pBusItem->prop();
							//! Bus Group 갱신
							pItemProp->SetValue(_T("GENERAL") , _T("BUS GROUP") , rBusGroupID);
							CString rString;
							rString.Format(_T("%d") , ++nBusIndex);
							pItemProp->SetValue(_T("GENERAL") , _T("ORDER NO") , rString.operator LPCSTR());

							//! fire event
							/*
							CEventData EventData(pBusItem->GetTypeString(), CEventData::MODIFY_ITEM_PROP);
							EventData.m_pFromObserver = m_pObserver;
							EventData.Add( pBusItem );
							m_pObserver->FireEvent(EventData);
							*/
						}
					}
					while(NULL != (hBusSiblingItem = GetNextSiblingItem(hBusSiblingItem)));
				}
			}
		}while(NULL != (hBusGroupSiblingItem = GetNextSiblingItem(hBusGroupSiblingItem)));
	}

	//!실제 BUS 데이타를 정렬한다.
	CELoadDocData& docData = CELoadDocData::GetInstance();
	docData.SortBusGroupByOrderNo();

	return ERROR_SUCCESS;
}

/**
	@brief COPY, EDITLABEL, NEW 실행 후 선택 아이템 속성창을 보여준다.

	@author	KHS
*/
int CBusEditTreeCtrl::SelectedItemPropView(void)
{
	HTREEITEM hSelItem = GetSelectedItem();
	CString rText = GetItemText(hSelItem);
	///! Parents Tree
	if(NULL == GetParentItem(hSelItem))
	{
		if((NOT_ASSIGNED == rText) || (SUBBUS == rText) || (INCOMING == rText)) return ERROR_BAD_ENVIRONMENT;
		//! BUS GROUP
		CELoadItem* pItem = (CELoadItem*)GetItemData(hSelItem);
		if(pItem->IsKindOf(CBusGroupItem::TypeString()))
		{
			CBusGroupItem* pBusGroupItem = static_cast<CBusGroupItem*>(pItem);

			CEventData EventData(pBusGroupItem->GetTypeString() , CEventData::SELECT_ITEM);
			EventData.Add( pBusGroupItem );
			m_pObserver->FireEvent(EventData);
			return ERROR_SUCCESS;
		}
	}
	else
	{
		//! BUS
		CELoadItem* pItem = (CELoadItem*)GetItemData(hSelItem);
		if(pItem->IsKindOf(CBusItem::TypeString()))
		{
			CBusItem* pBusItem = static_cast<CBusItem*>(pItem);

			CEventData EventData(pBusItem->GetTypeString() , CEventData::SELECT_ITEM);
			EventData.Add( pBusItem );
			m_pObserver->FireEvent(EventData);
			return ERROR_SUCCESS;
		}
	}
	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	action string을 설정한다.

	@author	BHK
*/
int CBusEditTreeCtrl::SetActionString(const string& rActionString)
{
	m_rActionString = rActionString;

	return ERROR_SUCCESS;
}

/**
	@brief	BUS TREE ITEM SELECTED

	@author KHS	

	@date 2009-05-13 오후 1:00:41	

	@param	

	@return		
**/
void CBusEditTreeCtrl::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	if(_T("SHOW BUS") == m_rActionString)
	{
		CString rText;
		HTREEITEM hTreeItem = pNMTreeView->itemNew.hItem;
		if (hTreeItem != NULL)
		{       
			SetFocus();
			SelectItem (hTreeItem);
			rText = GetItemText(hTreeItem);
		}

		CELoadDocData& docData = CELoadDocData::GetInstance();
		SelectedItemPropView();
	}
	*pResult = 0;
}
