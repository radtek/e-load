// MFCListCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "ELoad.h"
#include "MFCListCtrlEx.h"
#include "ELoadDocData.h"
#include "PropertiesWnd.h"
#include "commands/CommandAutoFillMCCBProp.h"
#include "MainFrm.h"

#include "WizardDlg.h"
#include "SchematicTypeCreationDlg.h"
#include "ControlTypeCreationDlg.h"
// CMFCListCtrlEx

static BOOL _HitF2_ = FALSE;

IMPLEMENT_DYNAMIC(CMFCListCtrlEx, CMFCListCtrl)

CMFCListCtrlEx::CMFCListCtrlEx() : m_pObserver(NULL) , m_bShowPropertiesBarAfterEndLabelEdit(false)
{
	m_iSortedColumn = -1;
	m_bAscending = TRUE;
	m_bMarkSortedColumn = FALSE;
	m_clrSortedColumn = (COLORREF)-1;
	m_hOldFont = NULL;

	m_pObserver = new CObserver(this , &CMFCListCtrlEx::OnEventFromObserver);
}

CMFCListCtrlEx::~CMFCListCtrlEx()
{
        try
	{
		if(m_pObserver) SAFE_DELETE(m_pObserver);
	}
	catch(...)
	{
	}
}


BEGIN_MESSAGE_MAP(CMFCListCtrlEx, CMFCListCtrl)
	ON_WM_LBUTTONDBLCLK()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, &CMFCListCtrlEx::OnLvnColumnclick)
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_ITEM_DELETE, &CMFCListCtrlEx::OnItemDelete)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, &CMFCListCtrlEx::OnLvnEndlabeledit)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, &CMFCListCtrlEx::OnLvnBeginlabeledit)
	///ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(NM_CLICK, &CMFCListCtrlEx::OnNMClick)
	ON_COMMAND(ID_ITEM_COPY, &CMFCListCtrlEx::OnItemCopy)
	ON_COMMAND(ID_COPY_TRANSFORMER, &CMFCListCtrlEx::OnCopyTransformer)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CMFCListCtrlEx message handlers

/**
        @brief  리스트 아이템 더블클릭 - 선택한 아이템들을 PropertiesBar에 나타낸다.

        @author	KHS
*/
void CMFCListCtrlEx::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	//! 속성창이 열려 있으면 닫는다.
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if(pFrame->m_wndPropertiesBar.IsVisible())
	{
		pFrame->m_wndPropertiesBar.ShowPane(FALSE , FALSE , FALSE);
	}

        if(IsWindowVisible()) ShowSelectedItemInPropertiesBar(true);
        
	CMFCListCtrl::OnLButtonDblClk(nFlags, point);
}
/**
        @brief  해더 클릭시 정렬

        @author	KHS
*/
void CMFCListCtrlEx::OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult)
{
        NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	ASSERT (pNMListView != NULL);

	int iColumn = pNMListView->iSubItem;
	BOOL bShiftIsPressed = (::GetAsyncKeyState (VK_SHIFT) & 0x8000);
	int nColumnState = GetHeaderCtrl ().GetColumnState (iColumn);
	BOOL bAscending = TRUE;
	
	if (nColumnState != 0)
	{
		bAscending = nColumnState <= 0;
	}

	Sort (iColumn, bAscending, bShiftIsPressed && IsMultipleSort ());
	*pResult = 0;
}

void CMFCListCtrlEx::SetSortColumn (int iColumn, BOOL bAscending, BOOL bAdd)
{
	GetHeaderCtrl ().SetSortColumn (iColumn, bAscending, bAdd);
}
//*********************************************************************************
void CMFCListCtrlEx::RemoveSortColumn (int iColumn)
{
	GetHeaderCtrl ().RemoveSortColumn (iColumn);
}

//*********************************************************************************
void CMFCListCtrlEx::EnableMultipleSort (BOOL bEnable)
{
	GetHeaderCtrl ().EnableMultipleSort (bEnable);
}

//*********************************************************************************
BOOL CMFCListCtrlEx::IsMultipleSort () const
{
	return ((CMFCListCtrl*) this)->GetHeaderCtrl ().IsMultipleSort ();
}

/**
	@brief	KEYDOWN
        @author	KHS
*/
void CMFCListCtrlEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
        // TODO: Add your message handler code here and/or call default
        if (IsWindowVisible())
        {
                switch (nChar)
                {
			case VK_DELETE:
				OnItemDelete();
				break;
			case VK_F2:
			{
				POSITION pos = GetFirstSelectedItemPosition();
				while(pos)
				{
					const int nItem = GetNextSelectedItem(pos);
					PostMessage ( LVM_EDITLABEL, nItem , 0 );
                                        _HitF2_ = TRUE;
					break;
				}
			}
				break;
			case VK_DOWN: case VK_UP:
			{
				CMFCListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
				
				CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
				if(pFrame->m_wndPropertiesBar.IsVisible()) ShowSelectedItemInPropertiesBar(false);
			}
				return;
                }
        }

        CMFCListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

/**
        @brief  Delete Selected Item
        
	@author	KHS
*/
void CMFCListCtrlEx::OnItemDelete()
{
	if(IsWindowVisible() && (GetItemCount() > 0))
	{
		if( IDYES != MessageBox(_T("Delete Item, Really? "), _T("Warning!"), MB_YESNO|MB_ICONWARNING ))
		{
			return;
		}

		CEventData eventData(((CELoadItem*)GetItemData(0))->GetTypeString() , CEventData::DELETE_ITEM);
		CELoadDocData& docData = CELoadDocData::GetInstance();
		SetRedraw(FALSE);
		for (int nItem = GetItemCount() - 1 ; nItem >= 0 ; nItem--)
		{
			UINT  uState = GetItemState(nItem, LVIS_SELECTED);
			if (LVIS_SELECTED == (uState & LVIS_SELECTED))
			{
				CELoadItem* pItem = (CELoadItem*)GetItemData( nItem );
				if(pItem)
				{
					pItem->SetDeleteFlag(true);
					DeleteItem(nItem);

					eventData.m_SelectionSet.Add(pItem);
				}
			} 
		}
		SetRedraw(TRUE);
		InsertItemCount();

		//! OBSERVER를 통해 새로운 ITEM이 삭제되었다는 것을 알린다.
		if(m_pObserver) m_pObserver->FireEvent(eventData);

		//! 속성창이 열려있으면 닫는다.
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if(pFrame->m_wndPropertiesBar.IsVisible())
		{
			pFrame->m_wndPropertiesBar.ShowPane(FALSE , FALSE , FALSE);
		}
	}
}

/**
	@brief	List에 있는 모든 Item을 삭제한다.

	@author BHK	

	@date 2009-07-02 오후 1:32:02	

	@param	

	@return		
*/
void CMFCListCtrlEx::OnItemDeleteAll()
{
	if(IsWindowVisible() && (GetItemCount() > 0))
	{
		if( IDYES != MessageBox(_T("Delete Item, Really? "), _T("Warning!"), MB_YESNO|MB_ICONWARNING ))
		{
			return;
		}

		CEventData eventData(((CELoadItem*)GetItemData(0))->GetTypeString() , CEventData::DELETE_ITEM);
		CELoadDocData& docData = CELoadDocData::GetInstance();
		for (int nItem = GetItemCount() - 1 ; nItem >= 0 ; nItem--)
		{
			CELoadItem* pItem = (CELoadItem*)GetItemData( nItem );
			if(pItem)
			{
				pItem->SetDeleteFlag(true);
				eventData.m_SelectionSet.Add(pItem);
			}
		}
		DeleteAllItems();
		InsertItemCount();

		//! OBSERVER를 통해 새로운 ITEM이 삭제되었다는 것을 알린다.
		if(m_pObserver) m_pObserver->FireEvent(eventData);

		//! 속성창이 열려있으면 닫는다.
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if(pFrame->m_wndPropertiesBar.IsVisible())
		{
			pFrame->m_wndPropertiesBar.ShowPane(FALSE , FALSE , FALSE);
		}
	}
}

/**
        @brief  Preview List Item Count
        @author	KHS
*/
void CMFCListCtrlEx::InsertItemCount(void)
{
        int nColWidth = GetColumnWidth(0);
        DeleteColumn(1);
        CString rItemCount;
        rItemCount.Format("All Count : %d", GetItemCount());
        InsertColumn(1, rItemCount, LVCFMT_LEFT, nColWidth);
}

/**
	@brief	called when label edit is ended.

	@author	KHS,BHK
*/
void CMFCListCtrlEx::OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	SetFocus();
	LPCSTR lszText  = pDispInfo->item.pszText;
	if(NULL == lszText) return;

	CELoadItem* pItem = (CELoadItem*)(GetItemData(pDispInfo->item.iItem));

	//! 이름 중복 체크
	CELoadDocData& docData = CELoadDocData::GetInstance();
	if(docData.IsExistItemName(pItem->GetTypeString() , lszText))
	{
		MessageBox("The name exists already!! ", "Warning!", MB_OK|MB_ICONWARNING );
		return;
	}

	POSITION pos = GetFirstSelectedItemPosition();
	int nItem = GetNextSelectedItem(pos);
	if(nItem >= 0 )
	{
		CELoadItem* pItem = (CELoadItem*)GetItemData(nItem);
		if(pItem)
		{
			if(ERROR_SUCCESS == pItem->SetName(lszText))
			{
				CEventData EventData(pItem->GetTypeString(), CEventData::MODIFY_ITEM_PROP);
				EventData.Add( pItem );
				m_pObserver->FireEvent(EventData);
			}
		}
		SetItemText(nItem ,0 , lszText);
		Sort(0 , GetHeaderCtrl().IsAscending());	//! 오름차순으로 정렬

		SelectAsFirstVisibleItem(lszText);

		if(true == m_bShowPropertiesBarAfterEndLabelEdit)
		{
			ShowSelectedItemInPropertiesBar(true);
			m_bShowPropertiesBarAfterEndLabelEdit = false;
		}
	}

	*pResult = 0;
}

void CMFCListCtrlEx::OnLvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
        NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
        // TODO: Add your control notification handler code here
        *pResult = (TRUE == _HitF2_) ? 0 : 1;
        
	_HitF2_ = FALSE;
}

/**
	@breif	this is sorting function.

	@author	BHK
*/
int CMFCListCtrlEx::OnCompareItems (LPARAM lParam1, LPARAM lParam2, int iColumn)
{
	CELoadItem* pItem1 = (CELoadItem*)lParam1;
	CELoadItem* pItem2 = (CELoadItem*)lParam2;

	if((0 == iColumn) && pItem1 && pItem2)
	{
		CString rString1 = pItem1->GetName().c_str();
		rString1.MakeUpper();
		CString rString2 = pItem2->GetName().c_str();
		rString2.MakeUpper();

		return strcmp(rString1 , rString2);
	}
        
	return 0;

}

/**
        @brief  called by observer.

        @author BHK
*/
int CMFCListCtrlEx::OnEventFromObserver(CObserver* pObserver , CEventData* pEventData)
{
	//! DO NOTHING

	return ERROR_SUCCESS;
}
/**
        @brief  Created New LOAD ITEM

        @author KHS
*/
/*
void CMFCListCtrlEx::OnItemNew()
{
        // TODO: Add your command handler code here
        if(IsWindowVisible())
        {
                if(GetSelectedCount() < 1) return;
                
                //!이름 중복 체크
		CELoadDocData& docData = CELoadDocData::GetInstance();
		if(docData.IsExistItemName(CLoadItem::TypeString() , UNNAMED_ITEM))
                {
                        MessageBox("The name exists already!! ", "Warning!", MB_OK|MB_ICONWARNING );
                        return;
                }
                
                CEventData EventData("" , CEventData::MODIFY_ITEM_PROP);
                string rItemCategoryString;

		CELoadItemProp* pLoadProp = docData.m_ItemPropMap[CLoadItem::TypeString()];

		CLoadItem* pLoadItem = static_cast<CLoadItem*>(docData.CreateELoadItemOf(CLoadItem::TypeString()));
                if(pLoadItem)
                {
                        pLoadItem->SetProp(pLoadProp->Clone());
                        pLoadItem->SetName( UNNAMED_ITEM );
                        pLoadItem->prop()->SetValue("Type", "Load Category", "MOTOR");

                        const int nCount = GetItemCount();
                        const int nItem = InsertItem(nCount + 1 , UNNAMED_ITEM);
                        if(-1 != nItem) SetItemData( nItem , (DWORD_PTR)pLoadItem);

                        ///docData.m_LoadItemEntry.push_back(pLoadItem);

                        Sort(0 , GetHeaderCtrl().IsAscending());	//! 오름차순으로 정렬
                        DisplayItemCount();

                        //! 정보를 속성창으로 넘겨준다.
                        rItemCategoryString = pLoadItem->GetTypeString();
                        EventData.m_SelectionSet.Add( pLoadItem );
                        docData.m_SelectedItemEntry.push_back(pLoadItem);
                        EventData.SetItemCategory( rItemCategoryString );

                        m_pObserver->FireEvent(EventData);

                        //!스트롤바 이동
                        CRect rect;
                        GetItemRect(0, rect, LVIR_BOUNDS);   // 한칸의 높이를 측정   
                        const int nItemCount = GetItemCount();
                        for(int i = 0; i < nItemCount;i++)
                        {
                                CString rText = GetItemText(i,0);
                                if( CString(UNNAMED_ITEM) == rText )
                                {
                                        POSITION pos = GetFirstSelectedItemPosition();
                                        if( pos )
                                        {
                                                const int nSelItem   = GetNextSelectedItem( pos );
                                                SetItemState(nSelItem, 0, LVIS_SELECTED );
                                                int cy = rect.Height() * (i - nSelItem);
                                                CSize szOrigin( 0, cy );
                                                Scroll(szOrigin);
                                        }
                                        SetFocus();
                                        SetItemState(i,LVIS_SELECTED ,LVIS_SELECTED);
                                        SendMessage(WM_KEYDOWN,VK_F2,0);
                                        m_bShowPropertiesBarAfterEndLabelEdit = true;

                                        break;
                                }
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
*/

/**
        @brief display Load Item Count

	@author	KHS
*/
void CMFCListCtrlEx::DisplayItemCount(void)
{
	TCHAR szBuf[MAX_PATH] = {'\0',};

	//! COLUMN의 텍스트를 읽어온다.
	LVCOLUMN lvcolumn;
	lvcolumn.mask = LVCF_TEXT;
	lvcolumn.cchTextMax = MAX_PATH;
	lvcolumn.pszText= szBuf;
	GetColumn(0 , &lvcolumn);
	CString rString = lvcolumn.pszText;
	const int at = rString.Find(_T("("));
	if(-1 != at) rString = rString.Left(at);

	CString rItemText;
	rItemText.Format(_T("%s(%d)") , rString , GetItemCount());

	lvcolumn.mask = LVCF_TEXT;
	lvcolumn.pszText = LPSTR(rItemText.operator LPCSTR());
	SetColumn(0 , &lvcolumn);
}

/**
        @brief 리스트 아이템 마우스 클릭

	@author	KHS
*/
/*
void CMFCListCtrlEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	CMFCListCtrl::OnLButtonDown(nFlags, point);
}
*/

/**
        @brief 리스트 아이템 마우스 클릭

	@author	KHS

	@date	?
*/
void CMFCListCtrlEx::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE>(pNMHDR);

	if(IsWindowVisible())
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if(pFrame->m_wndPropertiesBar.IsVisible()) ShowSelectedItemInPropertiesBar(false);
	}
	*pResult = 0;
}

/**
	@brief	ITEM을 복사 생성한다.

	@author	KHS,BHK
*/
void CMFCListCtrlEx::OnItemCopy()
{
	// TODO: Add your command handler code here
	if(IsWindowVisible())
	{
		if(GetSelectedCount() < 1) return;

		POSITION pos = GetFirstSelectedItemPosition();
		const int nItem   = GetNextSelectedItem(pos);
		const CString rItemText = GetItemText(nItem , 0);

		CELoadItem* pELoadItem = (CELoadItem*)GetItemData(nItem);
		if(pELoadItem)
		{
			const string rNewItemName = string(rItemText.operator LPCSTR()) + _T("_Copy");

			//! 이름 중복 체크
			CELoadDocData& docData = CELoadDocData::GetInstance();
			if(docData.IsExistItemName(pELoadItem->GetTypeString() , rNewItemName))
			{
				MessageBox("The name exists already!! ", "Warning!", MB_OK|MB_ICONWARNING );
				return;
			}

			CEventData EventData("" , CEventData::MODIFY_ITEM_PROP);
			string rItemCategoryString;

			CELoadItem* pCopiedItem = pELoadItem->Clone();
			if(NULL != pCopiedItem)
			{
				pCopiedItem->SetName( rNewItemName.c_str() );
				const int nCount = GetItemCount();
				const int nItem = InsertItem(nCount + 1 , rNewItemName.c_str() , docData.GetIconNo(pCopiedItem));
				if(-1 != nItem) SetItemData( nItem , (DWORD_PTR)pCopiedItem);

				Sort(0 , GetHeaderCtrl().IsAscending());	//! 오름차순으로 정렬
				DisplayItemCount();

				//! 정보를 속성창으로 넘겨준다.
				rItemCategoryString = pCopiedItem->GetTypeString();
				EventData.m_SelectionSet.Add( pCopiedItem );
				docData.m_SelectedItemEntry.push_back(pCopiedItem);
				EventData.SetItemCategory( rItemCategoryString );

				m_pObserver->FireEvent(EventData);
			}

			SelectAsFirstVisibleItem(rNewItemName);
			SendMessage(WM_KEYDOWN,VK_F2,0);
			m_bShowPropertiesBarAfterEndLabelEdit = true;
			//! 속성창을 띄운다.
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			if(!pFrame->m_wndPropertiesBar.IsVisible())
			{
				pFrame->m_wndPropertiesBar.ShowPane(TRUE , FALSE , TRUE);
			}
			/*
			//!스트롤바 이동
			CRect rect;
			GetItemRect(0,rect,LVIR_BOUNDS);   // 한칸의 높이를 측정   
			const int nItemCount = GetItemCount();
			for(int i = 0; i < nItemCount;i++)
			{
				CString rText = GetItemText(i,0);
				if( CString(rNewItemName.c_str()) == rText )
				{
					POSITION pos = GetFirstSelectedItemPosition();
					if(pos)
					{
						const int nSelItem   = GetNextSelectedItem( pos );
						SetItemState(nSelItem, 0, LVIS_SELECTED );
						int cy = rect.Height() * ( i - nSelItem );
						CSize szOrigin( 0, cy );
						Scroll( szOrigin );
					}
					SetFocus();
					SetItemState(i,LVIS_SELECTED ,LVIS_SELECTED);
					SendMessage(WM_KEYDOWN,VK_F2,0);
					m_bShowPropertiesBarAfterEndLabelEdit = true;

					break;
				}
			}

			//! 속성창을 띄운다.
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			if(!pFrame->m_wndPropertiesBar.IsVisible())
			{
				pFrame->m_wndPropertiesBar.ShowPane(TRUE , FALSE , TRUE);
			}
			*/
			//! OBSERVER를 통해 새로운 ITEM이 생성되었다는 것을 알린다.
			if(m_pObserver)
			{
				CEventData eventData(pELoadItem->GetTypeString() , CEventData::ADD_ITEM);
				eventData.m_SelectionSet.Add(pELoadItem);
				m_pObserver->FireEvent(eventData);
			}
		}
	}
}

/**
	@brief	

	@param	bOnShow when bOnShow is true, show property window if property window is hidden and close data sheet.

	@author	BHK
*/
int CMFCListCtrlEx::ShowSelectedItemInPropertiesBar(const bool& bOnShow)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	docData.m_SelectedItemEntry.clear();

	CEventData EventData(_T("") , CEventData::SELECT_ITEM);

	string rItemCategoryString;
	POSITION pos = GetFirstSelectedItemPosition();
	while(pos)
	{
		const int nItem   = GetNextSelectedItem(pos);
		CELoadItem* pItem = (CELoadItem*)GetItemData( nItem );
		if(pItem)
		{
			rItemCategoryString = pItem->GetTypeString();
			EventData.m_SelectionSet.Add( pItem );

			docData.m_SelectedItemEntry.push_back(pItem);
		}
	}

	if(!rItemCategoryString.empty())
	{
		EventData.SetItemCategory( rItemCategoryString );

		//! 창을 보여주기 전에 속성창으로 이벤트 발생
		m_pObserver->FireEvent(EventData);

		if(bOnShow)
		{
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			if(!pFrame->m_wndPropertiesBar.IsVisible())
			{
				pFrame->m_wndPropertiesBar.ShowPane(TRUE , FALSE , TRUE);
			}
		}
	}
	else
	{
		CString rItemText;
		POSITION pos = GetFirstSelectedItemPosition();
		while(pos)
		{
			const int nItem = GetNextSelectedItem(pos);
			rItemText = GetItemText(nItem , 0);
			break;
		}

		if(!rItemText.IsEmpty())
		{
			HDITEM hdi;
			enum   { sizeOfBuffer = 256 };
			TCHAR  lpBuffer[sizeOfBuffer];
			bool   fFound = false;

			hdi.mask = HDI_TEXT;
			hdi.pszText = lpBuffer;
			hdi.cchTextMax = sizeOfBuffer;
			GetHeaderCtrl().GetItem(0 , &hdi);
			const CString rHeaderString = CString(hdi.pszText).MakeUpper();
			if(0 == rHeaderString.Find(_T("SCHEMATIC")))
			{
				CWizardDlg dlg(0);
				CSchematicTypeCreationDlg _dlg2(rItemText.operator LPCSTR());
				dlg.AddPage(CSchematicTypeCreationDlg::IDD , &_dlg2 , _T("C2 - Schematic Type Creation"));
				dlg.DoModal();
			}
			else if(0 == rHeaderString.Find(_T("IO")))
			{
				CWizardDlg dlg(0);
				CControlTypeCreationDlg _dlg(_T("LCP IO TYPE"));
				dlg.AddPage(CControlTypeCreationDlg::IDD , &_dlg , _T("C1 - Control Type Creation"));
				dlg.DoModal();
			}
			else if(0 == rHeaderString.Find(_T("LCS")))
			{
				CWizardDlg dlg(0);
				CControlTypeCreationDlg _dlg(_T("LCS or LOCAL TYPE"));
				dlg.AddPage(CControlTypeCreationDlg::IDD , &_dlg , _T("C1 - Control Type Creation"));
				dlg.DoModal();
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	pELoadItemList에 들어 있는 ITEM을 표시한다.

	@author	HumKyung

	@date	????.??.??

	@param	rHeaderText
	@param	pELoadItemList

	@return	int
*/
int CMFCListCtrlEx::RemakeListCtrl(const string& rHeaderText , list<CELoadItem*>* pELoadItemList)
{
	assert(pELoadItemList && "pELoadItemList is NULL");

	if(pELoadItemList)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();

		SetRedraw(FALSE);

		const int nColumnWidth = GetColumnWidth(0);
		DeleteAllItems();
		DeleteColumn(0);

		InsertColumn(0, rHeaderText.c_str() , LVCFMT_LEFT, nColumnWidth);
		int nItem = 0;
		for(list<CELoadItem*>::iterator itr = pELoadItemList->begin();itr != pELoadItemList->end();++itr)
		{
			//! 삭제한 아이템은 보여주지 않는다.
			if((*itr)->IsDeleted()) continue;

			const string rName = (*itr)->GetName();
			const int item = InsertItem(nItem++ , rName.c_str());
			if(-1 != item)
			{
				SetItemData( item , (DWORD_PTR)(*itr) );
				SetItem(item , 0 , LVIF_IMAGE , NULL , docData.GetIconNo(*itr) , 0 , 0 , 0);
			}
		}
		Sort(0 , GetHeaderCtrl().IsAscending());
		DisplayItemCount();

		SetRedraw(TRUE);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	ITEM을 추가합니다.

	@author	BHK
*/
int CMFCListCtrlEx::AddELoadItem(CELoadItem* pItem)
{
	assert(pItem && "pItem is NULL");

	if(pItem)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();

		const string rName = pItem->GetName();
		const int nItem = InsertItem(GetItemCount() , rName.c_str() ,  docData.GetIconNo(pItem));
		if(-1 != nItem) SetItemData( nItem , (DWORD_PTR)(pItem) );
		
		Sort(0 , GetHeaderCtrl().IsAscending());
		DisplayItemCount();
	}

	return ERROR_SUCCESS;
}

/**
	@brief	TRANSFORMER ITEM을 복사 생성한다.

	@author	BHK
*/
void CMFCListCtrlEx::OnCopyTransformer()
{
	OnItemCopy();
}

/**
	@brief	rTypeString에 맞는 Item을 생성합니다.

	@author	BHK
*/
int CMFCListCtrlEx::CreateItem(const string& rTypeString)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	
	if(docData.IsExistItemName(rTypeString ,UNNAMED_ITEM)) 
	{
		MessageBox("The unnamed exists already!! ", "Warning!", MB_OK|MB_ICONWARNING );
		return ERROR_BAD_ENVIRONMENT;
	}

	CELoadItemProp* pLoadProp = docData.m_ItemPropMap[rTypeString];
	CELoadItem* pItem = static_cast<CELoadItem*>(docData.CreateELoadItemOf(rTypeString));
	if(pItem)
	{
		pItem->SetName( UNNAMED_ITEM );

		if(rTypeString == CLoadItem::TypeString())
		{
			CLoadItem* pLoadItem = static_cast<CLoadItem*>(pItem);
			if(!m_rBusID.empty())
			{
				pItem->prop()->SetOriginalValue(_T("Panel") , _T("Bus ID") , m_rBusID);
				pItem->prop()->SetValue(_T("Panel") , _T("Bus ID") , m_rBusID);
				
				CBusItem* pBusItem = (CBusItem*)docData.FindBusItemByName(m_rBusID);
				if(pBusItem)
				{
					const string rLoadVolt = pBusItem->prop()->GetValue(_T("General") , _T("Load Voltage"));
					pItem->prop()->SetOriginalValue(_T("Rating") , _T("Rated Voltage") , rLoadVolt);
					pItem->prop()->SetValue(_T("Rating") , _T("Rated Voltage") , rLoadVolt);

					const string rPhase    = pBusItem->prop()->GetValue(_T("General") , _T("Phase"));
					pItem->prop()->SetOriginalValue(_T("Rating") , _T("Phase") , rPhase);
					pItem->prop()->SetValue(_T("Rating") , _T("Phase") , rPhase);
				}
			}
			
			if(!m_rArea.empty())
			{
				pItem->prop()->SetValue(_T("Location") , _T("Area") , m_rArea);
				pItem->prop()->SetOriginalValue(_T("Location") , _T("Area") , m_rArea);
			}
			////////////////////////////////////////////////////////////////////////////////////////////////////
			COMMAND::CCommandAutoFillMCCBProp cmdAutoFillMCCBProp;
			cmdAutoFillMCCBProp.m_SelectionSet.Add(pLoadItem);
			cmdAutoFillMCCBProp.Execute();
		}
		else if(rTypeString == CCableItem::TypeString())
		{
			if(_T("Power Cable Item") == m_rColumnTitleString)
			{
				pItem->prop()->SetOriginalValue(_T("General") , _T("Cable Category") , _T("Power Cable"));
				pItem->prop()->SetValue(_T("General") , _T("Cable Category") , _T("Power Cable"));
			}
			else if(_T("Control Cable Item") == m_rColumnTitleString)
			{
				pItem->prop()->SetOriginalValue(_T("General") , _T("Cable Category") , _T("Control Cable"));
				pItem->prop()->SetValue(_T("General") , _T("Cable Category") , _T("Control Cable"));
			}
		}

		const int nCount = GetItemCount();
		const int nItem = InsertItem(nCount + 1 , UNNAMED_ITEM , docData.GetIconNo(pItem));
		if(-1 != nItem) SetItemData( nItem , (DWORD_PTR)pItem);

		Sort(0 , GetHeaderCtrl().IsAscending());	//! 오름차순으로 정렬
		DisplayItemCount();

		//! 정보를 속성창으로 넘겨준다.
		CEventData EventData(_T("") , CEventData::MODIFY_ITEM_PROP);
		EventData.m_SelectionSet.Add( pItem );
		docData.m_SelectedItemEntry.push_back(pItem);
		EventData.SetItemCategory( rTypeString );
		m_pObserver->FireEvent(EventData);

		SelectAsFirstVisibleItem(UNNAMED_ITEM);
		SendMessage(WM_KEYDOWN,VK_F2,0);
		m_bShowPropertiesBarAfterEndLabelEdit = true;
		//! 속성창을 띄운다.
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if(!pFrame->m_wndPropertiesBar.IsVisible())
		{
			pFrame->m_wndPropertiesBar.ShowPane(TRUE , FALSE , TRUE);
		}
/*
		//!스트롤바 이동
		CRect rect;
		GetItemRect(0, rect, LVIR_BOUNDS);   // 한칸의 높이를 측정   
		const int nItemCount = GetItemCount();
		for(int i = 0; i < nItemCount;i++)
		{
			CString rText = GetItemText(i,0);
			if( CString(UNNAMED_ITEM) == rText )
			{
				POSITION pos = GetFirstSelectedItemPosition();
				if( pos )
				{
					const int nSelItem   = GetNextSelectedItem( pos );
					SetItemState(nSelItem, 0, LVIS_SELECTED );
					int cy = rect.Height() * (i - nSelItem);
					CSize szOrigin( 0, cy );
					Scroll(szOrigin);
				}
				SetFocus();
				SetItemState(i,LVIS_SELECTED ,LVIS_SELECTED);
				SendMessage(WM_KEYDOWN,VK_F2,0);
				m_bShowPropertiesBarAfterEndLabelEdit = true;

				break;
			}
			//! 속성창을 띄운다.
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			if(!pFrame->m_wndPropertiesBar.IsVisible())
			{
				pFrame->m_wndPropertiesBar.ShowPane(TRUE , FALSE , TRUE);
			}
		}
*/

		//! OBSERVER를 통해 새로운 ITEM이 생성되었다는 것을 알린다.
		if(m_pObserver)
		{
			CEventData eventData(pItem->GetTypeString() , CEventData::ADD_ITEM);
			eventData.m_SelectionSet.Add(pItem);
			m_pObserver->FireEvent(eventData);
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-05 오후 3:49:00	

	@param	

	@return		
**/
string CMFCListCtrlEx::GetBusID() const
{
	return m_rBusID;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-03 오전 11:39:18	

	@param	

	@return		
*/
int CMFCListCtrlEx::SetBusID(const string& rBusID)
{
	m_rBusID = rBusID;
	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-28 오후 7:28:44	

	@param	

	@return		
*/
string CMFCListCtrlEx::GetArea() const
{
	return m_rArea;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-28 오후 7:28:58	

	@param	

	@return		
*/
int CMFCListCtrlEx::SetArea(const string& rArea)
{
	m_rArea = rArea;
	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-13 오후 1:32:45	

	@param	

	@return		
*/
int CMFCListCtrlEx::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMFCListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	InsertColumn(0, m_rColumnTitleString.c_str() , LVCFMT_LEFT, 180);
	
	return 0;
}

/**
	@brief	

	@author BHK	

	@date 2009-06-18 오후 1:18:00	

	@param	

	@return		
*/
int CMFCListCtrlEx::SetCommandString( const string& rCommandString )
{
	m_rCommandString = rCommandString;

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-06-18 오후 1:18:17	

	@param	

	@return		
*/
string CMFCListCtrlEx::GetCommandString() const
{
	return m_rCommandString;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CMFCCableListCtrlEx, CMFCListCtrlEx)

CMFCCableListCtrlEx::CMFCCableListCtrlEx() : CMFCListCtrlEx()
{
}

CMFCCableListCtrlEx::~CMFCCableListCtrlEx()
{
        try
	{
	}
	catch(...)
	{
	}
}


BEGIN_MESSAGE_MAP(CMFCCableListCtrlEx, CMFCListCtrlEx)
END_MESSAGE_MAP()

// CMFCCableListCtrlEx message handlers
/**
	@breif	this is sorting function.

	@author	BHK
*/
int CMFCCableListCtrlEx::OnCompareItems (LPARAM lParam1, LPARAM lParam2, int iColumn)
{
	CELoadItem* pItem1 = (CELoadItem*)lParam1;
	CELoadItem* pItem2 = (CELoadItem*)lParam2;

	if((0 == iColumn) && pItem1 && pItem2)
	{
		if(pItem1->IsKindOf(CCableItem::TypeString()) && pItem2->IsKindOf(CCableItem::TypeString()))
		{
			CString rString1 = pItem1->GetName().c_str();
			rString1.MakeUpper();
			CString rString2 = pItem2->GetName().c_str();
			rString2.MakeUpper();

			return strcmp(rString1 , rString2);
		}
	}
	else if((1 == iColumn) && pItem1 && pItem2)
	{
		if(pItem1->IsKindOf(CCableItem::TypeString()) && pItem2->IsKindOf(CCableItem::TypeString()))
		{
			CString rString1 = pItem1->prop()->GetValue(_T("General") , _T("Load Id")).c_str();
			rString1.MakeUpper();
			CString rString2 = pItem2->prop()->GetValue(_T("General") , _T("Load Id")).c_str();
			rString2.MakeUpper();

			return strcmp(rString1 , rString2);
		}
	}

	return 0;

}

/**
	@brief	pELoadItemList에 들어 있는 ITEM을 표시한다.

	@author	BHK
*/
int CMFCCableListCtrlEx::RemakeListCtrl(const string& rHeaderText , list<CELoadItem*>* pELoadItemList)
{
	assert(pELoadItemList && "pELoadItemList is NULL");

	if(pELoadItemList)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();

		SetRedraw(FALSE);

		const int nColumnWidth = GetColumnWidth(0);
		DeleteAllItems();
		DeleteColumn(1);
		DeleteColumn(0);

		InsertColumn(0, rHeaderText.c_str() , LVCFMT_LEFT, nColumnWidth);
		InsertColumn(1, _T("Load ID"), LVCFMT_LEFT, 100);

		int nItem = 0;
		for(list<CELoadItem*>::iterator itr = pELoadItemList->begin();itr != pELoadItemList->end();++itr)
		{
			//! 삭제한 아이템은 보여주지 않는다.
			if((*itr)->IsDeleted()) continue;

			const string rName = (*itr)->GetName();
			const int item = InsertItem(nItem++ , rName.c_str());
			if(-1 != item)
			{
				SetItemData( item , (DWORD_PTR)(*itr) );
				SetItem(item , 0 , LVIF_IMAGE , NULL , docData.GetIconNo(*itr) , 0 , 0 , 0);

				if((*itr)->IsKindOf(CCableItem::TypeString()))
				{
					const string rLoadID = (*itr)->prop()->GetValue(_T("General") , _T("Load Id"));
					SetItemText(item , 1 , rLoadID.c_str());
				}
			}
		}
		//! Load ID순으로 소팅한다.
		Sort(1 , GetHeaderCtrl().IsAscending());

		SetRedraw(TRUE);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	ITEM을 추가합니다.

	@author	BHK
*/
int CMFCCableListCtrlEx::AddELoadItem(CELoadItem* pItem)
{
	assert(pItem && "pItem is NULL");

	if(pItem)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();

		const string rName = pItem->GetName();
		const int nItem = InsertItem(GetItemCount() , rName.c_str() ,  docData.GetIconNo(pItem));
		if(-1 != nItem)
		{
			SetItemData( nItem , (DWORD_PTR)(pItem) );

			if(pItem->IsKindOf(CCableItem::TypeString()))
			{
				const string rLoadID = pItem->prop()->GetValue(_T("General") , _T("Load Id"));
				SetItemText(nItem , 1 , rLoadID.c_str());
			}
		}
		
		Sort(0 , GetHeaderCtrl().IsAscending());
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-13 오후 1:36:38	

	@param	

	@return		
*/
int CMFCListCtrlEx::SetColumnTitleString(const string& rColumnTitleString)
{
	m_rColumnTitleString = rColumnTitleString;
	
	return ERROR_SUCCESS;
}

/**
	@brief	아이템을 리스트의 맨 앞에 보이도록 한다.

	@author BHK	

	@date 2009-05-15 오전 11:06:05	

	@param	

	@return		
*/
int CMFCListCtrlEx::SelectAsFirstVisibleItem(const string& rItemText)
{
	//!스트롤바 이동
	CRect rect;
	GetItemRect(0,rect,LVIR_BOUNDS);   // 한칸의 높이를 측정   
	const int nItemCount = GetItemCount();
	for(int i = 0; i < nItemCount;++i)
	{
		const CString rText = GetItemText(i,0);
		if( CString(rItemText.c_str()) == rText)
		{
			SCROLLINFO si;
			ZeroMemory(&si,sizeof(&si));
			si.cbSize = sizeof(si);
			si.fMask = SIF_ALL;
			GetScrollInfo(SB_VERT,&si); // get current SCROLLINFO

			//! 기존의 선택한 ITEM에서 선택 마크를 지운다.
			POSITION pos = GetFirstSelectedItemPosition();
			if(pos)
			{
				const int nSelItem   = GetNextSelectedItem( pos );
				SetItemState(nSelItem, 0, LVIS_SELECTED );
			}
			const int cy = rect.Height() * (i - si.nPos);
			CSize szOrigin( 0, cy );
			Scroll( szOrigin );

			SetFocus();
			SetItemState(i,LVIS_SELECTED | LVIS_FOCUSED ,LVIS_SELECTED | LVIS_FOCUSED);
			break;
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CMFCTransformerListCtrlEx, CMFCListCtrlEx)

CMFCTransformerListCtrlEx::CMFCTransformerListCtrlEx() : CMFCListCtrlEx()
{
}

CMFCTransformerListCtrlEx::~CMFCTransformerListCtrlEx()
{
        try
	{
	}
	catch(...)
	{
	}
}


BEGIN_MESSAGE_MAP(CMFCTransformerListCtrlEx, CMFCListCtrlEx)
END_MESSAGE_MAP()

// CMFCTransformerListCtrlEx message handlers


/**
	@brief	pELoadItemList에 들어 있는 ITEM을 표시한다.

	@author	HumKyung

	@date	2011.01.05

	@param	rHeaderText
	@param	pELoadItemList

	@return	int
*/
int CMFCTransformerListCtrlEx::RemakeListCtrl(const string& rHeaderText , list<CELoadItem*>* pELoadItemList)
{
	assert(pELoadItemList && "pELoadItemList is NULL");

	if(pELoadItemList)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();

		SetRedraw(FALSE);

		const int nColumnWidth = GetColumnWidth(0);
		DeleteAllItems();
		DeleteColumn(0);

		InsertColumn(0, rHeaderText.c_str() , LVCFMT_LEFT, nColumnWidth);
		int nItem = 0;
		for(list<CELoadItem*>::iterator itr = pELoadItemList->begin();itr != pELoadItemList->end();++itr)
		{
			//! 삭제한 아이템은 보여주지 않는다.
			if((*itr)->IsDeleted()) continue;
			//! don't show virutal transformer
			if(0 == (*itr)->GetName().find(_T("DEL "))) continue;

			const string rName = (*itr)->GetName();
			const int item = InsertItem(nItem++ , rName.c_str());
			if(-1 != item)
			{
				SetItemData( item , (DWORD_PTR)(*itr) );
				SetItem(item , 0 , LVIF_IMAGE , NULL , docData.GetIconNo(*itr) , 0 , 0 , 0);
			}
		}
		Sort(0 , GetHeaderCtrl().IsAscending());
		DisplayItemCount();

		SetRedraw(TRUE);
	}

	return ERROR_SUCCESS;
}