//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a sample for BCGControlBar Library Professional Edition
// Copyright (C) 1998-2008 BCGSoft Ltd.
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
//*******************************************************************************
//
// ClassTreeWnd.cpp : implementation file
//

#include "stdafx.h"
#include "ELoad.h"
#include "ClassTreeWnd.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "BusGroupItem.h"
#include "BusItem.h"
#include <vector>
#include "ELoadDocData.h"
#include "ELoadItemViewBar.h"
#include "commands/CommandRevisionDelete.h"
#include "CableBMSummaryDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClassTreeWnd

CClassTreeWnd::CClassTreeWnd()
{
}

CClassTreeWnd::~CClassTreeWnd()
{
	try
	{
		for(map<HTREEITEM,ELOAD_ITEM_FILE_STRUCT*>::iterator itr = m_ELoadItemFileStructMap.begin();itr != m_ELoadItemFileStructMap.end();++itr)
		{
			SAFE_DELETE(itr->second);
		}
		m_ELoadItemFileStructMap.clear();
	}
	catch(...)
	{
	}
}


BEGIN_MESSAGE_MAP(CClassTreeWnd, CTreeCtrl)
	//{{AFX_MSG_MAP(CClassTreeWnd)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(NM_CLICK, &CClassTreeWnd::OnNMClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CClassTreeWnd::OnNMDblClick)
	ON_WM_KEYUP()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_REV_ITEM_DELETE, &CClassTreeWnd::OnRevItemDelete)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClassTreeWnd message handlers

/**
	@brief	

	@author	BHK
*/
void CClassTreeWnd::FillClassView ()
{
	CString rModulePath = GetExecPath();

	HTREEITEM hItem = NULL;

	string rLoadItemFilePath(CString(rModulePath + _T("Setting\\LOAD.ITEM")).operator LPCSTR());

	ifstream ifile(rLoadItemFilePath.c_str());
	if(ifile.is_open())
	{
		CreateObjTreeNode(this , NULL, ifile);
	}
}

/**
	@brief  Read 'Load.Item' file and then create tree.

        @author	BHK
*/
void CClassTreeWnd::CreateObjTreeNode(CTreeCtrl* pTreeCtrl , HTREEITEM hParent, ifstream& ifile)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string rMDBFilePath = docData.GetProjectMDBFilePath();

	HTREEITEM hItem = NULL;
	while(!ifile.eof() && ifile.good())
	{
		string aLine;
		getline(ifile, aLine);
		
		if(aLine.empty() || "*" == aLine || "[" == aLine) continue;

		CString rStr(aLine.c_str());
		rStr.Trim();
		if(("}" == rStr) || ("]" == rStr)) break;

		if("{" == rStr) 
		{
			CreateObjTreeNode(pTreeCtrl, hItem, ifile);
			SetItemState (hParent, TVIS_BOLD, TVIS_BOLD);
			Expand(hParent , TVE_EXPAND);
		}
		else if(_T("") != rStr)
		{
			vector<string> oResult;
			CTokenizer<CIsFromString>::Tokenize(oResult , rStr.operator LPCSTR() , CIsFromString(","));
			CString rItemImage("0") , rSelectedImage("0") , rItemText("0") , rCommand("SHOW NONE");
			if(oResult.size() > 3)
			{
				ELOAD_ITEM_FILE_STRUCT* pItemFileStruct = new ELOAD_ITEM_FILE_STRUCT;
				if(pItemFileStruct)
				{
					pItemFileStruct->rItemImage = oResult[1].c_str();
					pItemFileStruct->rSelectedImage = oResult[2].c_str();
					pItemFileStruct->rItemText   = oResult[0].c_str();
					pItemFileStruct->rCommand    = oResult[3].c_str();
					pItemFileStruct->rTypeStr    = (oResult.size() > 4) ? oResult[4].c_str() : _T("");
					pItemFileStruct->rHeaderText = (oResult.size() > 5) ? oResult[5].c_str() : _T("");
					pItemFileStruct->rItemImage.Trim();
					pItemFileStruct->rSelectedImage.Trim();
					pItemFileStruct->rItemText.Trim();
					pItemFileStruct->rCommand.Trim();

					TV_INSERTSTRUCT TI;
					TI.hParent     = hParent;
					TI.item.iImage = atoi(pItemFileStruct->rItemImage);
					TI.item.iSelectedImage = atoi(pItemFileStruct->rSelectedImage);
					TI.item.mask   = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
					TI.item.pszText = const_cast<char*>(pItemFileStruct->rItemText.operator LPCTSTR());
					
					hItem = pTreeCtrl->InsertItem(&TI);
					if(NULL != hItem)
					{
						pTreeCtrl->SetItemData(hItem , DWORD_PTR(pItemFileStruct));
						m_ELoadItemFileStructMap.insert(make_pair(hItem , pItemFileStruct));
					}

					////////////////////////////////////////////////////////////////////////////////////////////////////
					//! PRODUCT 중에서 REVISION DATA가 있을 경우에 TREE에 REVISION NO를 표시한다.
					map<HTREEITEM , ELOAD_ITEM_FILE_STRUCT*>::iterator where = m_ELoadItemFileStructMap.find(hParent);
					if((NULL != hItem) && (where != m_ELoadItemFileStructMap.end()))
					{
						const CString rProductName = GetItemText(hItem);

						if((_T("SHOW CALCULATION") == where->second->rCommand) || 
							((_T("SHOW NONE") == where->second->rCommand) && 
							_T("Auto Power Cable") == rProductName || 
							_T("Auto Control Cable") == rProductName ||
							_T("Cable Schedule") == rProductName ||
							_T("Drum Schedule") == rProductName))
						{
							RemakeRevDataTreeNodeOf(rProductName.operator LPCSTR());
						}
					}
				}
			}
		}
	}

	DisplayCableItemCount(); //! CABLE ITEM COUNT를 표시한다.
	DisplayArea();
}

/**
	@brief	

	@author BHK	

	@date 2009-05-15 오전 12:29:28	

	@param	

	@return		
**/
BOOL CClassTreeWnd::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT (pNMHDR != NULL);

	if (pNMHDR->code == TTN_SHOW && GetToolTips () != NULL)
	{
		GetToolTips ()->SetWindowPos (&wndTop, -1, -1, -1, -1,
			SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}

void CClassTreeWnd::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	int i = 0;
	//! DO NOTHING
}

/**
	@brief	left mouse clicked on this control.

	@author	BHK

	@date	?

	@param

	@return
*/
void CClassTreeWnd::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	
	UINT uFlags = 0;
	HTREEITEM hItem = HitTest(point , &uFlags);
	if(hItem && (uFlags & TVHT_ONITEM))	//! ITEM TEXT를 클릭 했을때...
	{
		CELoadItemViewBar* pELoadItemViewBar = (CELoadItemViewBar*)(GetParent()->GetParent());
		if(pELoadItemViewBar)
		{
			HTREEITEM hParentItem = GetParentItem(hItem);
			ELOAD_ITEM_FILE_STRUCT* pItemFileStruct = 
				(NULL != hParentItem) ? (ELOAD_ITEM_FILE_STRUCT*)GetItemData(hParentItem) : NULL;

			{
				const CString rItemText = GetItemText(hItem);
				pItemFileStruct = (ELOAD_ITEM_FILE_STRUCT*)GetItemData(hItem);

				TVITEM item;
				item.hItem = hItem;
				item.mask  = TVIF_IMAGE;
				GetItem(&item);

				if(REVISION_ICON == item.iImage)
				{
					map<HTREEITEM,ELOAD_ITEM_FILE_STRUCT*>::iterator where = m_ELoadItemFileStructMap.find(hItem);
					if(where == m_ELoadItemFileStructMap.end())
					{
						HTREEITEM hParent = GetParentItem(hItem);
						if(NULL != hParent)
						{
							map<HTREEITEM,ELOAD_ITEM_FILE_STRUCT*>::iterator where = m_ELoadItemFileStructMap.find(hParent);
							if((where != m_ELoadItemFileStructMap.end()) && (_T("SHOW ALL BUS LOAD SUMMARY") == where->second->rCommand))
							{
								pELoadItemViewBar->OnClickTreeItem(_T("SHOW ALL REV BUS LOAD SUMMARY|") + rItemText);
							}
							else if((where != m_ELoadItemFileStructMap.end()) && (_T("SHOW ALL PDB LOAD SUMMARY") == where->second->rCommand))
							{
								pELoadItemViewBar->OnClickTreeItem(_T("SHOW ALL REV PDB LOAD SUMMARY|") + rItemText);
							}
							else if((where != m_ELoadItemFileStructMap.end()) && (_T("SHOW TRANSFORMER SIZING") == where->second->rCommand))
							{
								pELoadItemViewBar->OnClickTreeItem(_T("SHOW REV TRANSFORMER SIZING|") + rItemText);
							}
							else if((where != m_ELoadItemFileStructMap.end()) && (_T("SHOW POWER CABLE") == where->second->rCommand))
							{
								pELoadItemViewBar->OnClickTreeItem(_T("SHOW REV CABLE SIZING RESULT|") + rItemText);
							}
							else if((where != m_ELoadItemFileStructMap.end()) && (_T("SHOW CABLE SIZING TABLE") == where->second->rCommand))
							{
								pELoadItemViewBar->OnClickTreeItem(_T("SHOW CABLE SIZING TABLE|REV(") + rItemText + _T(")"));
							}
							else if((where != m_ELoadItemFileStructMap.end()) && (_T("SHOW CONTROL CABLE") == where->second->rCommand))
							{
								//!Control Cable 추가
								pELoadItemViewBar->OnClickTreeItem(_T("SHOW REV CONTROL CABLE RESULT|") + rItemText);
							}
							else if((where != m_ELoadItemFileStructMap.end()) && (_T("SHOW DRUM SCHEDULE") == where->second->rCommand))
							{
								//!drum schedule 추가
								pELoadItemViewBar->OnClickTreeItem(_T("SHOW REV DRUM SCHEDULE|") + rItemText);
							}
							else if((where != m_ELoadItemFileStructMap.end()) && (_T("SHOW CABLE SCHEDULE") == where->second->rCommand))
							{
								//!cable schedule 추가
								pELoadItemViewBar->OnClickTreeItem(_T("SHOW REV CABLE SCHEDULE|") + rItemText);
							}
						}
					}
				}
				else if((BUS_ICON != item.iImage) && (SUBBUS_ICON != item.iImage))
				{
					if(NULL != pItemFileStruct)
						pELoadItemViewBar->OnClickTreeItem(pItemFileStruct->rCommand , pItemFileStruct->rTypeStr , pItemFileStruct->rHeaderText);
					else
					{
						map<HTREEITEM,ELOAD_ITEM_FILE_STRUCT*>::iterator where = m_ELoadItemFileStructMap.find(hParentItem);
						if(where != m_ELoadItemFileStructMap.end())
						{
							pELoadItemViewBar->OnClickTreeItem(where->second->rCommand + _T("|") + CString(rItemText) , _T("") , _T(""));
						}
					}
				}
				else if((BUS_ICON == item.iImage) || (SUBBUS_ICON == item.iImage))
				{
					CString rValue;
					const int nFound = rItemText.Find(_T(" ("));
					if(-1 != nFound) 
					{
						rValue = rItemText.Left(nFound);
					}

					CString rCommand  = _T("SHOW LOAD IN BUS|") + rValue;
					pELoadItemViewBar->OnClickTreeItem(rCommand , _T("BUS") , _T("Bus Item"));
				}
			}
		}
	}

	*pResult = 0;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-06 오후 6:56:38	

	@param	

	@return		
*/
void CClassTreeWnd::OnNMDblClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
}

/**
	@brief	생성된 ITEM을 자기에게 맞는 TREE NODE에 붙인다.

	@author BHK	

	@date 2009-04-04 오전 1:46:09	

	@param	

	@return		
**/
int CClassTreeWnd::AddTreeNodeOf(CELoadItem* pItem)
{
	assert(pItem && "pItem is NULL");

	if(pItem)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		if(pItem->IsKindOf(CBusItem::TypeString()))
		{
			CBusItem* pBusItem = static_cast<CBusItem*>(pItem);
			const string rBusId = pBusItem->GetName();
			const string rBusGroupId = pBusItem->prop()->GetValue(_T("General") , _T("Bus Group"));

			HTREEITEM hRoot = GetRootItem();
			HTREEITEM hParentItem = FindItemByCommand(hRoot , (SUBBUS == rBusGroupId) ? _T("SHOW PDB") : _T("SHOW LOAD"));
			if(NULL == hParentItem) return ERROR_BAD_ENVIRONMENT;

			//! delete if bus item is already created.
			HTREEITEM hChild = GetChildItem(hParentItem);
			while(hChild)
			{
				if(pBusItem == (CBusItem*)GetItemData(hChild)) return ERROR_SUCCESS;
				hChild = GetNextSiblingItem(hChild);
			}

			CELoadItemProp* pItemProp = pBusItem->prop();
			if(pItemProp)
			{
				CELoadDocData& docData = CELoadDocData::GetInstance();
				CItemSelectionSet SelectionSet;
				docData.GetLoadItemSetInBus(SelectionSet , rBusId);
				
				CString rItemText;
				rItemText.Format(_T("%s (%d)") , rBusId.c_str() , SelectionSet.GetItemCount());
				int nPDBCount = 0 , nLOADCount = 0 , nNotAssignedCount = 0;
				set<string> AreaSet;
				list<CELoadItem*> LoadItemList;
				docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
				//if(pItemList)
				{
					for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
					{
						if((*itr)->IsDeleted()) continue;

						const string rArea  = (*itr)->prop()->GetValue(_T("Location") , _T("Area"));
						AreaSet.insert(rArea);

						const string rBusID = (*itr)->prop()->GetValue(_T("Panel") , _T("Bus ID"));
						CBusItem* pBusItem = docData.FindBusItemByName(rBusID);
						if(pBusItem)
						{
							const string rBusGroup = pBusItem->prop()->GetValue(_T("General") , _T("Bus Group"));
							(SUBBUS == rBusGroup) ? ++nPDBCount : ++nLOADCount;
						}else	++nNotAssignedCount;
					}
				}
				map<HTREEITEM,ELOAD_ITEM_FILE_STRUCT*>::iterator itr;
				for(itr = m_ELoadItemFileStructMap.begin();itr != m_ELoadItemFileStructMap.end();++itr)
				{
					if((_T("SHOW PDB") == itr->second->rCommand))
					{
						CString rItemText;
						rItemText.Format("%s (%d)" , itr->second->rItemText , nPDBCount);
						SetItemText(itr->first , rItemText);
					}
					else if((_T("SHOW LOAD") == itr->second->rCommand))
					{
						CString rItemText;
						rItemText.Format("%s (%d)" , _T("BUS") , nLOADCount);
						SetItemText(itr->first , rItemText);
					}
					else if((_T("SHOW NOT ASSIGNED LOAD") == itr->second->rCommand))
					{
						CString rItemText;
						rItemText.Format("%s (%d)" , _T("NONE") , nNotAssignedCount);
						SetItemText(itr->first , rItemText);
					}
				}

				vector<CBusItem*> BusItemEntry;
				docData.GetAllBusItems(BusItemEntry);
				string rBusGroupID = pItemProp->GetValue(_T("General") , _T("Bus Group"));
				CBusGroupItem* pBusGroupItem = docData.FindBusGroupItemByName(rBusGroupID);
				int nBusGroupOrderNo = atoi(pBusGroupItem->prop()->GetValue(_T("General") , _T("Order No")).c_str());
				int nBusOrderNo = atoi(pItemProp->GetValue(_T("General") , _T("Order No")).c_str());
				const long nTotalOrderNo = (nBusGroupOrderNo * BusItemEntry.size()) + nBusOrderNo;

				//! find position to insert new tree item.
				HTREEITEM hInsertAfter = NULL , hInsertPrev = NULL;
				HTREEITEM hChildItem = GetChildItem(hParentItem);
				while(NULL != hChildItem)
				{
					CBusItem* pBusItemData = (CBusItem*)GetItemData(hChildItem);
					rBusGroupID = pBusItemData->prop()->GetValue(_T("General") , _T("Bus Group"));
					pBusGroupItem = docData.FindBusGroupItemByName(rBusGroupID);
					nBusGroupOrderNo = atoi(pBusGroupItem->prop()->GetValue(_T("General") , _T("Order No")).c_str());
					nBusOrderNo = atoi(pBusItemData->prop()->GetValue(_T("General") , _T("Order No")).c_str());
					const long nPrevTotalOrderNo = (nBusGroupOrderNo * BusItemEntry.size()) + nBusOrderNo;
					
					if(nTotalOrderNo < nPrevTotalOrderNo)
					{
						hInsertAfter = GetPrevSiblingItem(hChildItem);
						break;
					}
					hChildItem = GetNextSiblingItem(hChildItem);
				}
				
				///
				TV_INSERTSTRUCT TI;
				TI.hParent      = hParentItem;
				if(NULL != hInsertAfter) TI.hInsertAfter = hInsertAfter;
				TI.item.iImage  = (SUBBUS == rBusGroupId) ? SUBBUS_ICON : BUS_ICON;
				TI.item.iSelectedImage = (SUBBUS == rBusGroupId) ? SUBBUS_ICON : BUS_ICON;
				TI.item.mask    = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
				TI.item.pszText = (LPSTR)(rItemText.operator LPCSTR());
				HTREEITEM hItem = InsertItem(&TI);
				if(hItem) SetItemData( hItem , (DWORD_PTR)pBusItem);
				SetFocus();
				SelectItem(hItem);
			}
		}
		else if(pItem->IsKindOf(CLoadItem::TypeString()))
		{
			DisplayLoadItemCountOf(pItem);
		}
		else if(pItem->IsKindOf(CCableItem::TypeString()))
		{
			DisplayCableItemCount();
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
int CClassTreeWnd::DeleteTreeNodeOf(CELoadItem* pItem)
{
	assert(pItem && "pItem is NULL");

	if(pItem)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		if(pItem->IsKindOf(CBusItem::TypeString()))
		{
			CBusItem* pBusItem = static_cast<CBusItem*>(pItem);

			const string rBusId = pBusItem->GetName();
			const string rBusGroupId = pBusItem->prop()->GetValue(_T("General") , _T("Bus Group"));
			HTREEITEM hItem = FindItemByCommand(GetRootItem() , (SUBBUS == rBusGroupId) ? _T("SHOW PDB") : _T("SHOW LOAD")) , hChild = NULL;
			if(hItem && (hChild = GetChildItem(hItem)))
			{
				while(hChild)
				{
					if( pBusItem == (CBusItem*)GetItemData(hChild))
					{
						DeleteItem(hChild);
						return ERROR_SUCCESS;
					}
					hChild = GetNextSiblingItem(hChild);
				}
			}

			return ERROR_SUCCESS;
		}
		else if(pItem->IsKindOf(CLoadItem::TypeString()))
		{
			DisplayLoadItemCountOf(pItem);
		}
		else if(pItem->IsKindOf(CCableItem::TypeString()))
		{
			DisplayCableItemCount();
		}
		else if(pItem->IsKindOf(CBusGroupItem::TypeString()))
		{
			CBusGroupItem* pBusGroupItem = static_cast<CBusGroupItem*>(pItem);

			const string rBusGroupId = pBusGroupItem->prop()->GetValue(_T("General") , _T("Bus Group ID"));
			HTREEITEM hItem = FindItemByCommand(GetRootItem() , (SUBBUS == rBusGroupId) ? _T("SHOW PDB") : _T("SHOW LOAD")) , hChild = NULL;
			if(hItem && (hChild = GetChildItem(hItem)))
			{
				for(vector<CBusItem*>::iterator itr = pBusGroupItem->m_BusRefEntry.begin();itr != pBusGroupItem->m_BusRefEntry.end();++itr)
				{
					hChild = GetChildItem(hItem);
					while(hChild)
					{
						if( (*itr) == (CBusItem*)GetItemData(hChild))
						{
							DeleteItem(hChild);
							break;
						}
						hChild = GetNextSiblingItem(hChild);
					}
				}
			}

			return ERROR_SUCCESS;
		}
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	해당 COMMAND를 가진 HTREEITEM을 찾는다.

	@author	BHK
*/
HTREEITEM CClassTreeWnd::FindItemByCommand(HTREEITEM hItem , const CString& rCommand)
{
	for(map<HTREEITEM,ELOAD_ITEM_FILE_STRUCT*>::iterator itr = m_ELoadItemFileStructMap.begin();itr != m_ELoadItemFileStructMap.end();++itr)
	{
		if(itr->second->rCommand == rCommand) return itr->first;
	}

	return NULL;
}

/**
	@brief	BUS ITEM이 변경시 TREE에 표시되는 부분을 변경한다.

	@author	BHK
*/
int CClassTreeWnd::ModifyBusItem(CBusItem* pBusItem)
{
	assert(pBusItem && "pBusItem is NULL");

	if(pBusItem)
	{
		const string rBusGroupId = pBusItem->prop()->GetValue(_T("General") , _T("Bus Group"));
		HTREEITEM hItem = FindItemByCommand(GetRootItem() , (SUBBUS == rBusGroupId) ? _T("SHOW PDB") : _T("SHOW LOAD")) , hChild = NULL;
		if(hItem && (hChild = GetChildItem(hItem)))
		{
			const string rBusId = pBusItem->GetName();
			while(hChild)
			{
				if( pBusItem == (CBusItem*)GetItemData(hChild))
				{
					CELoadDocData& docData = CELoadDocData::GetInstance();
					CItemSelectionSet SelectionSet;
					docData.GetLoadItemSetInBus(SelectionSet , pBusItem->GetName());
					CString rItemText;
					rItemText.Format(_T("%s (%d)") , pBusItem->GetName().c_str() , SelectionSet.GetItemCount());
					SetItemText(hChild , rItemText);
					
					return ERROR_SUCCESS;
				}
				hChild = GetNextSiblingItem(hChild);
			}
		}
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief  SHow Load Bus Tree 를 Update 한다

	@author KHS
*/
int CClassTreeWnd::UpdateShowLoadBusItem()
{
	//! 기존의 TREE NODE들을 삭제한다.
	HTREEITEM hItem = FindItemByCommand(GetRootItem() , _T(_T("SHOW LOAD")));
	CString rItemText = GetItemText(hItem);
	if(hItem)
	{
		HTREEITEM hChild = GetChildItem(hItem);

		while(hChild)
		{
			CString rChildText = GetItemText(hChild);
			HTREEITEM hSiblingItem = GetNextSiblingItem(hChild);
			DeleteItem(hChild);
			CString rSiblingText = GetItemText(hSiblingItem);
			hChild = hSiblingItem;
		}
	}

	hItem = FindItemByCommand(GetRootItem() , _T("SHOW PDB"));
	rItemText = GetItemText(hItem);
	if(hItem)
	{
		HTREEITEM hChild = GetChildItem(hItem);

		while(hChild)
		{
			CString rChildText = GetItemText(hChild);
			HTREEITEM hSiblingItem = GetNextSiblingItem(hChild);
			DeleteItem(hChild);
			CString rSiblingText = GetItemText(hSiblingItem);
			hChild = hSiblingItem;
		}
	}
	//!

	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> BusGroupList;
	docData.GetELoadItemListOf(BusGroupList , CBusGroupItem::TypeString());
	{
		for(list<CELoadItem*>::iterator itr = BusGroupList.begin(); itr != BusGroupList.end();++itr)
		{
			CBusGroupItem* pBusGroup = static_cast<CBusGroupItem*>(*itr);
			if(pBusGroup->IsDeleted()) continue;

			for(vector<CBusItem*>::iterator jtr = pBusGroup->m_BusRefEntry.begin(); jtr != pBusGroup->m_BusRefEntry.end();++jtr)
			{
				if((*jtr)->IsDeleted()) continue;

				AddTreeNodeOf((*jtr));
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief

	@author	BHK
*/
void CClassTreeWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(VK_DELETE == nChar)
	{
		HTREEITEM hSelItem = GetSelectedItem();
		if(NULL != hSelItem)
		{
			HTREEITEM hParentItem = GetParentItem(hSelItem);
			if(NULL != hParentItem)
			{
				ELOAD_ITEM_FILE_STRUCT* pItemFileStruct = (ELOAD_ITEM_FILE_STRUCT*)GetItemData(hParentItem);
				if((NULL != pItemFileStruct) && (_T("SHOW CABLE SIZING TABLE") == pItemFileStruct->rCommand))
				{
					DeleteCableSizingTableFile(hSelItem);
				}
			}
		}
	}

	CTreeCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}

/**
	@brief hItem의 텍스트 이름을 가진 파일을 삭제하고 hItem도 트리에서 삭제한다.

	@author BHK	

	@date 2009-04-10 오후 3:15:38	

	@param	

	@return		
*/
int CClassTreeWnd::DeleteCableSizingTableFile(HTREEITEM hItem)
{
	assert(hItem && "hItem is NULL");

	if(hItem)
	{
		const CString rTreeItemText = GetItemText(hItem);
		DeleteItem(hItem);

		CELoadDocData& docData = CELoadDocData::GetInstance();
		CString rCableSizeDataFile = CString(docData.GetProjectFolderPath().c_str()) + "CableSizing\\" + rTreeItemText + ".TXT";
		::DeleteFile(rCableSizeDataFile);
	}

	return 0;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-06 오후 5:11:07	

	@param	

	@return		
*/
int CClassTreeWnd::AddRevDataTreeNode(HTREEITEM hParent , const CString& rRevNo)
{
	HTREEITEM hItem = InsertItem(rRevNo , hParent , hParent);
	if(NULL != hItem) CTreeCtrl::SetItemImage(hItem , REVISION_ICON , REVISION_ICON);

	return ERROR_SUCCESS;
}

/**
	@brief	ProductName에 해당하는 Rev Data를 database에서 읽어서 트리를 구축한다.

	@author BHK	

	@date 2009-05-07 오후 4:14:11	

	@param	

	@return		
*/
int CClassTreeWnd::RemakeRevDataTreeNodeOf(const string& rProductName)
{
	HTREEITEM hParent = NULL;
	for(map<HTREEITEM,ELOAD_ITEM_FILE_STRUCT*>::iterator itr = m_ELoadItemFileStructMap.begin();itr != m_ELoadItemFileStructMap.end();++itr)
	{
		if(itr->second->rItemText.MakeUpper() == CString(rProductName.c_str()).MakeUpper())
		{
			hParent = itr->first;
			break;
		}
	}

	if(NULL != hParent)
	{
		//! 일단 밑에 붙은 REV. DATA TREE NODE를 삭제한다.
		HTREEITEM hItem = GetChildItem(hParent);
		while(hItem)
		{
			HTREEITEM hSibling = GetNextSiblingItem(hItem);
			DeleteItem(hItem);
			hItem = hSibling;
		}

		//! database에서 REV. DATA를 읽어 트리에 삽입한다.
		CELoadDocData& docData = CELoadDocData::GetInstance();
		const string rMDBFilePath = docData.GetProjectMDBFilePath();

		CADODB adoDB;
		const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;
		if(TRUE == adoDB.DBConnect(rConnectionString))
		{
			CString rFormat(_T("SELECT * FROM T_PRODUCT_REVISION WHERE C_PRODUCT_NAME = '%s' ORDER BY C_REVISION_NO DESC")) , rSQL;

			rSQL.Format(rFormat , rProductName.c_str());
			adoDB.OpenQuery(rSQL.operator LPCTSTR());
			LONG lRecordCount = 0L;
			adoDB.GetRecordCount(&lRecordCount);
			for(int i = 0;i < lRecordCount;++i)
			{
				STRING_T strValue;
				adoDB.GetFieldValue(i , _T("C_REVISION_NO") , &strValue);

				HTREEITEM hItem = InsertItem(strValue.c_str() , hParent , hParent);
				if(NULL != hItem) SetItemImage(hItem , REVISION_ICON , REVISION_ICON);
			}
		}
	}

	return 0;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-15 오전 12:53:50	

	@param	

	@return		
**/
int CClassTreeWnd::DisplayLoadItemCountOf(CELoadItem* pItem)
{
	if(pItem)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();

		const string rBusID = pItem->prop()->GetValue(_T("Panel") , _T("Bus ID"));
		string rBusGroup;
		CBusItem* pBusItem = docData.FindBusItemByName(rBusID);
		if(pBusItem) rBusGroup = pBusItem->prop()->GetValue(_T("General") , _T("Bus Group"));

		string rCommandString;
		map<HTREEITEM,ELOAD_ITEM_FILE_STRUCT*>::iterator itr;
		for(itr = m_ELoadItemFileStructMap.begin();itr != m_ELoadItemFileStructMap.end();++itr)
		{
			if(!rBusGroup.empty())
			{
				if((SUBBUS == rBusGroup) && (_T("SHOW PDB") == itr->second->rCommand))
				{
					rCommandString = itr->second->rCommand;
					break;
				}
				else if((SUBBUS != rBusGroup) && (_T("SHOW LOAD") == itr->second->rCommand))
				{
					rCommandString = itr->second->rCommand;
					break;
				}
			}
			else if((_T("SHOW NOT ASSIGNED LOAD") == itr->second->rCommand))
			{
				rCommandString = itr->second->rCommand;
				break;
			}
		}
		if(itr != m_ELoadItemFileStructMap.end())
		{
			int nCount = 0;
			if(_T("SHOW NOT ASSIGNED LOAD") == rCommandString)
			{
				list<CELoadItem*> LoadItemList;
				docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
				for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
				{
					const string rBusID = (*itr)->prop()->GetValue(_T("Panel") , _T("Bus ID"));
					CBusItem* pBusItem = docData.FindBusItemByName(rBusID);
					if(NULL == pBusItem) ++nCount;
				}
			}
			else
			{
				HTREEITEM hChild = GetChildItem(itr->first);
				while(hChild)
				{
					CString rItemText = GetItemText(hChild);
					const int nFound = rItemText.Find(_T(" ("));
					if(-1 != nFound) rItemText = rItemText.Left(nFound);

					CItemSelectionSet SelectionSet;
					docData.GetLoadItemSetInBus(SelectionSet , rItemText.operator LPCSTR());
					if(CString(rBusID.c_str()) == rItemText)
					{
						rItemText.Format("%s (%d)" , rItemText , SelectionSet.GetItemCount());
						SetItemText(hChild , rItemText);
					}
					nCount += SelectionSet.GetItemCount();

					hChild = GetNextSiblingItem(hChild);
				}
			}
			CString rItemText;
			rItemText.Format(_T("%s (%d)") , rBusGroup.empty() ? _T("NONE") : ((SUBBUS == rBusGroup) ? SUBBUS : _T("BUS")) , nCount);
			SetItemText(itr->first , rItemText);
		}

		DisplayArea();

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-28 오후 5:26:53	

	@param	

	@return		
*/
int CClassTreeWnd::DisplayArea(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	////////////////////////////////////////////////////////////////////////////////////////////////////
	set<string> AreaNameSet;
	list<CELoadItem*> LoadItemList;
	docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
	///if(pItemList)
	{
		for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
		{
			if((*itr)->IsDeleted()) continue;
			const string rArea = (*itr)->prop()->GetValue(_T("Location") , _T("Area"));
			if(!rArea.empty()) AreaNameSet.insert(rArea);
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////

	map<HTREEITEM,ELOAD_ITEM_FILE_STRUCT*>::iterator itr;
	for(itr = m_ELoadItemFileStructMap.begin();itr != m_ELoadItemFileStructMap.end();++itr)
	{
		if(_T("SHOW AREA") == itr->second->rCommand)
		{
			CString rItemText = GetItemText(itr->first);
			const int nFound = rItemText.Find(_T(" ("));
			if(-1 != nFound) rItemText = rItemText.Left(nFound);

			CString rNewItemText;
			rNewItemText.Format("%s (%d)" , rItemText , AreaNameSet.size());
			SetItemText(itr->first , rNewItemText);

			DeleteChildNodeOf(itr->first);
			for(set<string>::iterator jtr = AreaNameSet.begin();jtr != AreaNameSet.end();++jtr)
			{
				InsertItem(jtr->c_str() , AREA_ICON , AREA_ICON , itr->first);
			}
			InsertItem(_T("NONE") , itr->first);
			
			break;
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-15 오전 10:19:46	

	@param	

	@return		
*/
int CClassTreeWnd::DisplayCableItemCount(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	////////////////////////////////////////////////////////////////////////////////////////////////////
	int nPowerCableCount = 0 , nControlCableCount = 0, nUserCableCount = 0;
	list<CELoadItem*> CableItemList;
	docData.GetELoadItemListOf(CableItemList , CCableItem::TypeString(), _T("*"));
	//if(pItemList)
	{
		for(list<CELoadItem*>::iterator itr = CableItemList.begin();itr != CableItemList.end();++itr)
		{
			if((*itr)->IsDeleted()) continue;
			const string rCategory = (*itr)->prop()->GetValue(_T("General") , _T("Cable Category"));
			const string rCreatedBy = (*itr)->prop()->GetValue(_T("General") , _T("Created By"));
			if((_T("Power Cable") == rCategory) && (_T("Auto") == rCreatedBy))
				++nPowerCableCount;
			else if((_T("Control Cable") == rCategory) && (_T("Auto") == rCreatedBy))
				++nControlCableCount;
			else if(_T("User") == rCreatedBy)
			{
				++nUserCableCount;
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////

	map<HTREEITEM,ELOAD_ITEM_FILE_STRUCT*>::iterator itr;
	for(itr = m_ELoadItemFileStructMap.begin();itr != m_ELoadItemFileStructMap.end();++itr)
	{
		if(_T("SHOW POWER CABLE") == itr->second->rCommand)
		{
			CString rItemText = GetItemText(itr->first);
			const int nFound = rItemText.Find(_T(" ("));
			if(-1 != nFound) rItemText = rItemText.Left(nFound);

			CString rNewItemText;
			rNewItemText.Format("%s (%d)" , rItemText , nPowerCableCount);
			SetItemText(itr->first , rNewItemText);
		}
		else if(_T("SHOW CONTROL CABLE") == itr->second->rCommand)
		{
			CString rItemText = GetItemText(itr->first);
			const int nFound = rItemText.Find(_T(" ("));
			if(-1 != nFound) rItemText = rItemText.Left(nFound);

			CString rNewItemText;
			rNewItemText.Format("%s (%d)" , rItemText , nControlCableCount);
			SetItemText(itr->first , rNewItemText);
		}
		else if(_T("SHOW USER CABLE") == itr->second->rCommand)
		{
			CString rItemText = GetItemText(itr->first);
			const int nFound = rItemText.Find(_T(" ("));
			if(-1 != nFound) rItemText = rItemText.Left(nFound);

			CString rNewItemText;
			rNewItemText.Format("%s (%d)" , rItemText , nUserCableCount);
			SetItemText(itr->first , rNewItemText);
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-28 오후 5:32:38	

	@param	

	@return		
*/
int CClassTreeWnd::DeleteChildNodeOf(HTREEITEM hItem)
{
	assert(hItem && "hItem is NULLL");

	if(hItem)
	{
		HTREEITEM hChild = GetChildItem(hItem);

		while(hChild)
		{
			CString rChildText = GetItemText(hChild);
			HTREEITEM hSiblingItem = GetNextSiblingItem(hChild);
			DeleteItem(hChild);
			CString rSiblingText = GetItemText(hSiblingItem);
			hChild = hSiblingItem;
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	KEY DOWN  FOR REVISON DELETE

	@author KHS	

	@date 2009-06-19 오후 1:45:29	

	@param	

	@return		
**/
void CClassTreeWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(VK_DELETE == nChar)
	{
		OnRevItemDelete();
	}

	CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

/**
	@brief	

	@author KHS	

	@date 2009-07-13 오전 11:16:22	

	@param	

	@return		
**/
void CClassTreeWnd::OnRevItemDelete()
{
	HTREEITEM hItem = GetSelectedItem();
	if(hItem)
	{
		CELoadItemViewBar* pELoadItemViewBar = (CELoadItemViewBar*)(GetParent()->GetParent());
		if(pELoadItemViewBar)
		{
			map<HTREEITEM,ELOAD_ITEM_FILE_STRUCT*>::iterator where = m_ELoadItemFileStructMap.find(hItem);
			if(where == m_ELoadItemFileStructMap.end())
			{
				HTREEITEM hParent = GetParentItem(hItem);
				if(NULL != hParent)
				{
					map<HTREEITEM,ELOAD_ITEM_FILE_STRUCT*>::iterator where = m_ELoadItemFileStructMap.find(hParent);
					if((where != m_ELoadItemFileStructMap.end()) && (_T("SHOW ALL BUS LOAD SUMMARY") == where->second->rCommand))
					{
						const CString rItemText = GetItemText(hItem);
						if(IDOK == ::MessageBox(NULL, "Revison " + rItemText + " 을 삭제하시겠습니까?", _T("Warning") , MB_OKCANCEL| MB_ICONWARNING))
						{
							COMMAND::CCommandRevisionDelete cmdRevisionDelete;
							if(ERROR_SUCCESS == cmdRevisionDelete.Execute(rItemText.operator LPCSTR(), _T("Load Summary-BUS")))
							{
								DeleteItem(hItem);
							}

							if(ERROR_SUCCESS == cmdRevisionDelete.Execute(rItemText.operator LPCSTR(), _T("Load Summary-SubBUS")))
							{
								///DeleteItem(hItem);
								RemakeRevDataTreeNodeOf(_T("Load Summary-SubBUS"));
							}
						}
					}
					else if((where != m_ELoadItemFileStructMap.end()) && (_T("SHOW ALL PDB LOAD SUMMARY") == where->second->rCommand))
					{
						const CString rItemText = GetItemText(hItem);
						if(IDOK == ::MessageBox(NULL, "Revison " + rItemText + " 을 삭제하시겠습니까?", _T("Warning") , MB_OKCANCEL| MB_ICONWARNING))
						{
							COMMAND::CCommandRevisionDelete cmdRevisionDelete;
							if(ERROR_SUCCESS == cmdRevisionDelete.Execute(rItemText.operator LPCSTR(), _T("Load Summary-SubBUS")))
							{
								DeleteItem(hItem);
							}

							if(ERROR_SUCCESS == cmdRevisionDelete.Execute(rItemText.operator LPCSTR(), _T("Load Summary-BUS")))
							{
								RemakeRevDataTreeNodeOf(_T("Load Summary-BUS"));
								///DeleteItem(hItem);
							}
						}
					}
					else if((where != m_ELoadItemFileStructMap.end()) && (_T("SHOW TRANSFORMER SIZING") == where->second->rCommand))
					{
						const CString rItemText = GetItemText(hItem);
						if(IDOK == ::MessageBox(NULL, "Revison " + rItemText + " 을 삭제하시겠습니까?", "Warning" , MB_OKCANCEL| MB_ICONWARNING))
						{
							COMMAND::CCommandRevisionDelete cmdRevisionDelete;
							if(ERROR_SUCCESS == cmdRevisionDelete.Execute(rItemText.operator LPCSTR(), _T("Transformer Sizing")))
							{
								DeleteItem(hItem);
							}
						}
					}
					else if((where != m_ELoadItemFileStructMap.end()) && (_T("SHOW POWER CABLE") == where->second->rCommand))
					{
						const CString rItemText = GetItemText(hItem);
						if(IDOK == ::MessageBox(NULL, "Revison " + rItemText + " 을 삭제하시겠습니까?", "Warning" , MB_OKCANCEL| MB_ICONWARNING))
						{
							COMMAND::CCommandRevisionDelete cmdRevisionDelete;
							if(ERROR_SUCCESS == cmdRevisionDelete.Execute(rItemText.operator LPCSTR(), _T("Auto Power Cable")))
							{
								DeleteItem(hItem);
							}
						}
					}
					else if((where != m_ELoadItemFileStructMap.end()) && (_T("SHOW CONTROL CABLE") == where->second->rCommand))
					{
						const CString rItemText = GetItemText(hItem);
						if(IDOK == ::MessageBox(NULL, "Revison " + rItemText + " 을 삭제하시겠습니까?", "Warning" , MB_OKCANCEL| MB_ICONWARNING))
						{
							COMMAND::CCommandRevisionDelete cmdRevisionDelete;
							if(ERROR_SUCCESS == cmdRevisionDelete.Execute(rItemText.operator LPCSTR(), _T("Auto Control Cable")))
							{
								DeleteItem(hItem);
							}
						}
					}
					else if((where != m_ELoadItemFileStructMap.end()) && (_T("SHOW CABLE SIZING TABLE") == where->second->rCommand))
					{
						const CString rItemText = GetItemText(hItem);
						if(IDOK == ::MessageBox(NULL, "Revison " + rItemText + " 을 삭제하시겠습니까?", "Warning" , MB_OKCANCEL| MB_ICONWARNING))
						{
							COMMAND::CCommandRevisionDelete cmdRevisionDelete;
							if(ERROR_SUCCESS == cmdRevisionDelete.Execute(rItemText.operator LPCSTR(), _T("Cable Sizing Table")))
							{
								DeleteItem(hItem);
							}
						}
					}
					else if((where != m_ELoadItemFileStructMap.end()) && (_T("SHOW CABLE SCHEDULE") == where->second->rCommand))
					{
						const CString rItemText = GetItemText(hItem);
						if(IDOK == ::MessageBox(NULL, "Revison " + rItemText + " 을 삭제하시겠습니까?", "Warning" , MB_OKCANCEL| MB_ICONWARNING))
						{
							COMMAND::CCommandRevisionDelete cmdRevisionDelete;
							if(ERROR_SUCCESS == cmdRevisionDelete.Execute(rItemText.operator LPCSTR(), _T("Cable Schedule")))
							{
								DeleteItem(hItem);
							}
						}
					}
					else if((where != m_ELoadItemFileStructMap.end()) && (_T("SHOW DRUM SCHEDULE") == where->second->rCommand))
					{
						const CString rItemText = GetItemText(hItem);
						if(IDOK == ::MessageBox(NULL, "Revison " + rItemText + " 을 삭제하시겠습니까?", "Warning" , MB_OKCANCEL| MB_ICONWARNING))
						{
							COMMAND::CCommandRevisionDelete cmdRevisionDelete;
							if(ERROR_SUCCESS == cmdRevisionDelete.Execute(rItemText.operator LPCSTR(), _T("Drum Schedule")))
							{
								DeleteItem(hItem);
							}
						}
					}
				}
			}
		}
	}
}
