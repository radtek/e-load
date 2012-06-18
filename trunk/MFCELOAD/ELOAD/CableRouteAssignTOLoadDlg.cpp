// CableRouteAssignTOLoadDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "CableRouteAssignTOLoadDlg.h"
#include "ELoadDocData.h"
#include "ELOAD.h"

// CCableRouteAssignTOLoadDlg dialog

IMPLEMENT_DYNAMIC(CCableRouteAssignTOLoadDlg, CDialog)

CCableRouteAssignTOLoadDlg::CCableRouteAssignTOLoadDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCableRouteAssignTOLoadDlg::IDD, pParent)
{
	m_bDragging = FALSE;
	m_nDragIndex = -1;
	m_nDropIndex = -1;
	m_pDragImage = NULL;
}

CCableRouteAssignTOLoadDlg::~CCableRouteAssignTOLoadDlg()
{
}

void CCableRouteAssignTOLoadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ROUTE_GROUP_LIST, m_wndRouteGrListCtrl);
	DDX_Control(pDX, IDC_ASSIGN_LOAD_LIST, m_wndAssignLoadListCtrl);
	DDX_Control(pDX, IDC_NOT_ASSIGN_LIST, m_wndNotAssignLoadListCtrl);
	DDX_Control(pDX, IDC_LOAD_AREA_COMBO, m_wndLoadAreaCombo);
	DDX_Control(pDX, IDOK, m_wndOkButton);
	DDX_Control(pDX, IDCANCEL, m_wndCancelButton);
	DDX_Control(pDX, IDC_LOAD_BUS_COMBO, m_wndSortByBusCombo);
}


BEGIN_MESSAGE_MAP(CCableRouteAssignTOLoadDlg, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_ROUTE_GROUP_LIST, &CCableRouteAssignTOLoadDlg::OnNMClickRouteGroupList)
	ON_CBN_SELCHANGE(IDC_LOAD_AREA_COMBO, &CCableRouteAssignTOLoadDlg::OnCbnSelchangeLoadAreaCombo)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY(LVN_BEGINDRAG, IDC_NOT_ASSIGN_LIST, &CCableRouteAssignTOLoadDlg::OnLvnBegindragNotAssignList)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_ASSIGN_LOAD_LIST, &CCableRouteAssignTOLoadDlg::OnLvnBegindragAssignLoadList)
	ON_BN_CLICKED(IDOK, &CCableRouteAssignTOLoadDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CCableRouteAssignTOLoadDlg::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_LOAD_BUS_COMBO, &CCableRouteAssignTOLoadDlg::OnCbnSelchangeLoadBusCombo)
END_MESSAGE_MAP()


// CCableRouteAssignTOLoadDlg message handlers

BOOL CCableRouteAssignTOLoadDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_hIcon = AfxGetApp()->LoadIcon(IDI_GRABBER);

	CreateImageList();

	InitRouteGroupListCtrl();
	InitAssignListCtrl();
	InitNotAssignListCtrl();

	if(ERROR_SUCCESS != DisplayRouteGroupData())
	{
		return FALSE;
	}
	
	CELoadDocData& docData = CELoadDocData::GetInstance();

	//! load의 Route Group 속성을 미리 저장해 둔다.
	m_wndLoadAreaCombo.AddString("*");
	m_wndSortByBusCombo.AddString("*");

	list<CELoadItem*> LoadItemList;
	docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString(), "*");

	{
		for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
		{
			const string rRouteGroup = (*itr)->prop()->GetValue(_T("Cable Route") , _T("Route Group"));
			const string rArea = (*itr)->prop()->GetValue(_T("Location"), _T("Area"));
			const string rBusId = (*itr)->prop()->GetValue(_T("Panel"), _T("Bus ID"));

			m_LoadRouteGroupMap.insert(make_pair(*itr , rRouteGroup));

			//! LOAD AREA COMBO 박스에 AREA를 UNIQUE 하게 삽입
			if((-1 == m_wndLoadAreaCombo.FindString(0, rArea.c_str())) && (!rArea.empty()))
			{
				m_wndLoadAreaCombo.AddString(rArea.c_str());
			}
		}
	}


	list<CELoadItem*> BusGroupItemList;
	docData.GetELoadItemListOf(BusGroupItemList, CBusGroupItem::TypeString());
	{
		for(list<CELoadItem*>::iterator itr = BusGroupItemList.begin(); itr != BusGroupItemList.end();++itr)
		{
			CBusGroupItem *pBusGroupItem = static_cast<CBusGroupItem*>(*itr);
			for(vector<CBusItem*>::iterator jtr = pBusGroupItem->m_BusRefEntry.begin(); jtr != pBusGroupItem->m_BusRefEntry.end();++jtr)
			{
				const string rBusId = (*jtr)->GetName();
				if((-1 == m_wndSortByBusCombo.FindString(0, rBusId.c_str())) && (!rBusId.empty()))
				{
					m_wndSortByBusCombo.AddString(rBusId.c_str());
				}
			}
		}
	}
	
	//! USER BUS ITEM
	list<CELoadItem*> UserBusItemList;
	docData.GetELoadItemListOf(UserBusItemList, CBusItem::TypeString(), "User");
	{
		for(list<CELoadItem*>::iterator itr = UserBusItemList.begin(); itr != UserBusItemList.end();++itr)
		{
			const string rBusId = (*itr)->GetName();
			if((-1 == m_wndSortByBusCombo.FindString(0, rBusId.c_str())) && (!rBusId.empty()))
			{
				m_wndSortByBusCombo.AddString(rBusId.c_str());
			}
		}
	}


	m_wndLoadAreaCombo.SetCurSel(0);
	OnCbnSelchangeLoadAreaCombo();

	m_wndSortByBusCombo.SetCurSel(0);
	OnCbnSelchangeLoadBusCombo();

	//!BUTTON SETTING
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	m_wndOkButton.SetTextHotColor(TextHotColor);
	m_wndCancelButton.SetTextHotColor(TextHotColor);

	m_wndOkButton.SetIcon(IDI_OK_18X18);
	m_wndCancelButton.SetIcon(IDI_CANCEL_18X18);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	INIT ROUTE GROUP LIST CTRL

	@author KHS	

	@return		
**/
int CCableRouteAssignTOLoadDlg::InitRouteGroupListCtrl(void)
{
	const CString __HEADER__[] = {_T("Route Gr."), _T("Route Gr. Desc"), _T("Design Length")};
	const int __SIZE__[] = {60, 90, 90};

	int nCount = sizeof(__HEADER__) / sizeof(__HEADER__[0]);

	for(int i = 0;i < nCount;i++)
	{
		m_wndRouteGrListCtrl.InsertColumn(i, __HEADER__[i] , LVCFMT_LEFT , __SIZE__[i]);
	}

	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	m_wndRouteGrListCtrl.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES);
	m_wndRouteGrListCtrl.SetImageList (&(pApp->m_ELoadImages), LVSIL_SMALL);
	m_wndRouteGrListCtrl.Sort(0);
	
	return ERROR_SUCCESS;
}

/**
	@brief	INIT ASSIGNED LOAD LIST CTRL

	@author KHS	

	@return		
**/
int CCableRouteAssignTOLoadDlg::InitAssignListCtrl(void)
{
	const CString __HEADER__[] = {_T("Load Id"), _T("Rating"), _T("Area")};
	const int __SIZE__[] = {100, 68, 70};

	int nCount = sizeof(__HEADER__) / sizeof(__HEADER__[0]);

	for(int i = 0;i < nCount;i++)
	{
		m_wndAssignLoadListCtrl.InsertColumn(i, __HEADER__[i] , LVCFMT_LEFT , __SIZE__[i]);
	}

	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	m_wndAssignLoadListCtrl.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES);
	m_wndAssignLoadListCtrl.SetImageList (&(pApp->m_ELoadImages), LVSIL_SMALL);
	m_wndAssignLoadListCtrl.Sort(0);

	return ERROR_SUCCESS;
}

/**
	@brief	INIT NOT ASSIGNED LOAD LIST CTRL

	@author KHS	

	@return		
**/
int CCableRouteAssignTOLoadDlg::InitNotAssignListCtrl(void)
{
	const CString __HEADER__[] = {_T("Load Id"), _T("Rating"), _T("Area")};
	const int __SIZE__[] = {100, 68, 70};

	int nCount = sizeof(__HEADER__) / sizeof(__HEADER__[0]);

	for(int i = 0;i < nCount;i++)
	{
		m_wndNotAssignLoadListCtrl.InsertColumn(i, __HEADER__[i] , LVCFMT_LEFT , __SIZE__[i]);
	}
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	m_wndNotAssignLoadListCtrl.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES);
	m_wndNotAssignLoadListCtrl.SetImageList (&(pApp->m_ELoadImages), LVSIL_SMALL);
	m_wndNotAssignLoadListCtrl.Sort(0);


	return ERROR_SUCCESS;
}

/**
	@brief	DISPLAY ROUTE GROUP DATA

	@author KHS	

	@return		
**/
int CCableRouteAssignTOLoadDlg::DisplayRouteGroupData(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pTable = docData.GetTableOf(_T("T_ROUTE_GROUP"));
	if(pTable)
	{
		for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin(); itr != pTable->m_RecordEntry.end();++itr)
		{
			const string rRouteGroup = itr->FieldValueMap[_T("C_ROUTE_GROUP")];
			const string rDesc = itr->FieldValueMap[_T("C_DESC")];
			const string rDesignLength = itr->FieldValueMap[_T("C_DESIGN_LENGTH")];
			const string rRouteType = itr->FieldValueMap[_T("C_ROUTE_TYPE")];
			
			if(!IsExistRouteGroupItem(rRouteGroup.c_str()))
			{
				InsertItemInRouteGroupListCtrl(rRouteGroup.c_str(), rDesc.c_str(), rDesignLength.c_str());
			}
		}

		return ERROR_SUCCESS;
	}
	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	DISPLAY LOAD ITEM

	@author KHS	

	@return		
**/
int CCableRouteAssignTOLoadDlg::DisplayLoadHasRouteGroup(CMFCTextColorListCtrl& ListCtrl , const string& rRouteGroup, const string& rArea, const string rBusId)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> LoadItemList;
	docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString(), "*");
	///if(pItemList)
	{
		int i = 0;

		ListCtrl.SetRedraw(FALSE);
		ListCtrl.DeleteAllItems();
		ListCtrl.m_ItemColorMap.clear();
		for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
		{
			if((*itr)->IsDeleted()) continue;

			CLoadItem* pLoadItem = static_cast<CLoadItem*>(*itr);
			if(pLoadItem)
			{
				CELoadItemProp *pItemProp = pLoadItem->prop();
				const string rValue = pItemProp->GetValue(_T("Cable Route") , _T("Route Group"));
				const string rLocationArea = pItemProp->GetValue(_T("Location") , _T("Area"));
				const string rPanelBusId = pItemProp->GetValue(_T("Panel"), _T("Bus ID"));

				if((rArea != "*") && (rArea != rLocationArea)) continue;
				if((rBusId != "*") && (rBusId != rPanelBusId)) continue;
				if(rValue != rRouteGroup) continue;
				
				const int nImage = docData.GetIconNo(pLoadItem);
				const int nItem = ListCtrl.InsertItem(i++ , pLoadItem->GetName().c_str() , nImage);
				if(-1 != nItem)
				{
					ListCtrl.SetItemData(nItem , DWORD_PTR(pLoadItem));

					double nRatingCapacity = 0.f;
					pLoadItem->CalcRatingCapacity(nRatingCapacity);
					
					ostringstream oss;
					oss << nRatingCapacity;
					ListCtrl.SetItemText(nItem , 1 , oss.str().c_str());
					

					////////////////
					map<CELoadItem*,string>::iterator where = m_LoadRouteGroupMap.find(pLoadItem);
					if(where != m_LoadRouteGroupMap.end())
					{
						ListCtrl.SetItemText(nItem , 2 , rLocationArea.c_str());

						if(rRouteGroup != where->second)
						{
							ListCtrl.m_ItemColorMap[DWORD_PTR(pLoadItem)].push_back(RGB(255,0,0));
							ListCtrl.m_ItemColorMap[DWORD_PTR(pLoadItem)].push_back(RGB(255,0,0));
							ListCtrl.m_ItemColorMap[DWORD_PTR(pLoadItem)].push_back(RGB(255,0,0));
						}
						else
						{
							ListCtrl.m_ItemColorMap[DWORD_PTR(pLoadItem)].push_back(RGB(0,0,0));
							ListCtrl.m_ItemColorMap[DWORD_PTR(pLoadItem)].push_back(RGB(0,0,0));
							ListCtrl.m_ItemColorMap[DWORD_PTR(pLoadItem)].push_back(RGB(0,0,0));
						}
					}
				}
			}
		}
		ListCtrl.SetRedraw(TRUE);
		ListCtrl.RedrawItems(0 , ListCtrl.GetItemCount());
	}
	
	return ERROR_SUCCESS;
}

/**
	@brief	INSERT ITEM IN ROUTE GROUP LIST

	@author KHS	

	@return		
**/
int CCableRouteAssignTOLoadDlg::InsertItemInRouteGroupListCtrl(const CString& rRouteGroup, const CString& rDesc, const CString& rDesignLength)
{
	int nItem = m_wndRouteGrListCtrl.GetItemCount();
	nItem = m_wndRouteGrListCtrl.InsertItem(nItem, rRouteGroup,0);
	m_wndRouteGrListCtrl.SetItemText(nItem, 1, rDesc);
	m_wndRouteGrListCtrl.SetItemText(nItem, 2, rDesignLength);

	m_wndRouteGrListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES);

	return ERROR_SUCCESS;
}

/**
	@brief	 ROUTE GROUP DATA 중복 체크

	@author KHS	

	@return		
**/
BOOL CCableRouteAssignTOLoadDlg::IsExistRouteGroupItem(const CString& rRouteGroup)
{
	for(int i = 0;i < m_wndRouteGrListCtrl.GetItemCount();i++)
	{
		CString rItemText = m_wndRouteGrListCtrl.GetItemText(i, 0);
		if(rItemText == rRouteGroup) return TRUE;
	}
	return FALSE;
}

/**
	@brief	이미지 리스트 생성

	@author KHS	

	@return		
**/
int CCableRouteAssignTOLoadDlg::CreateImageList(void)
{
	UINT uiBmpId = IDB_CLASS_VIEW_HC;

	CBitmap bmp;
	if (!bmp.LoadBitmap (uiBmpId))
	{
		TRACE(_T ("Can't load bitmap: %x\n"), uiBmpId);
		ASSERT (FALSE);
		return FALSE;
	}
	BITMAP bmpObj;
	bmp.GetBitmap (&bmpObj);

	UINT nFlags = ILC_MASK | ILC_COLOR24;

	/*
	m_imgList.Create (16, bmpObj.bmHeight, nFlags, 0, 0);
	m_imgList.Add (&bmp, RGB (255, 0, 0));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_BUS_GROUP));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_BUS));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_LOAD));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_MOTOR));
	*/

	return ERROR_SUCCESS;
}

/**
	@brief	Route Group List Click

	@author KHS	

	@return		
**/
void CCableRouteAssignTOLoadDlg::OnNMClickRouteGroupList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	if(-1 != pNMItemActivate->iItem)
	{
		const string rRouteGroup = m_wndRouteGrListCtrl.GetItemText(pNMItemActivate->iItem , 0).operator LPCSTR();
	/*	CString rArea, rBusId;
		int nSelItem = m_wndLoadAreaCombo.GetCurSel();
		if(-1 != nSelItem) m_wndLoadAreaCombo.GetLBText(nSelItem, rArea);

		nSelItem = m_wndSortByBusCombo.GetCurSel();
		if(-1 != nSelItem) m_wndSortByBusCombo.GetLBText(nSelItem, rBusId);*/

		if(!rRouteGroup.empty()) DisplayLoadHasRouteGroup(m_wndAssignLoadListCtrl , rRouteGroup, "*", "*");
	}

	*pResult = 0;
}

/**
	@brief	LOAD AREA 콤보 박스 선택 변경 시

	@author KHS	

	@return		
**/
void CCableRouteAssignTOLoadDlg::OnCbnSelchangeLoadAreaCombo()
{
	// TODO: Add your control notification handler code here
	int nItem = m_wndLoadAreaCombo.GetCurSel();
	if(-1 != nItem)
	{
		CString rArea, rBusId;
		m_wndLoadAreaCombo.GetLBText(nItem, rArea);

		nItem = m_wndSortByBusCombo.GetCurSel();
		if(-1 != nItem) m_wndSortByBusCombo.GetLBText(nItem, rBusId);
		DisplayLoadHasRouteGroup(m_wndNotAssignLoadListCtrl, "", rArea.operator LPCSTR(), rBusId.operator LPCSTR());
	}
}

/**
	@brief	LBUTTON UP

	@author KHS	

	@return		
**/
void CCableRouteAssignTOLoadDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_bDragging)
	{
		// Release mouse capture, so that other controls can get control/messages
		ReleaseCapture ();
		
		// Note that we are NOT in a drag operation
		m_bDragging = FALSE;
		
		// End dragging image
		m_pDragImage->DragLeave (GetDesktopWindow ());
		m_pDragImage->EndDrag ();
		delete m_pDragImage; //must delete it because it was created at the beginning of the drag
		
		CPoint pt (point); //Get current mouse coordinates
		ClientToScreen (&pt); //Convert to screen coordinates
		// Get the CWnd pointer of the window that is under the mouse cursor
		CWnd* pDropWnd = WindowFromPoint (pt);
		ASSERT (pDropWnd); //make sure we have a window pointer
		// If window is CListCtrl, we perform the drop
		if ((m_pDragList != pDropWnd) && pDropWnd->IsKindOf (RUNTIME_CLASS (CMFCListCtrl)))
		{
			m_pDropList = (CMFCTextColorListCtrl*)pDropWnd; //Set pointer to the list we are dropping on
			DropItemOnList(m_pDragList, m_pDropList); //Call routine to perform the actual drop
		}
	}
	
	CDialog::OnLButtonUp(nFlags, point);
}

/**
	@brief	MOUSE MOVE

	@author KHS	

	@return		
**/
void CCableRouteAssignTOLoadDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bDragging)
	{
		//// Move the drag image
		CPoint pt(point);	//get our current mouse coordinates
		ClientToScreen(&pt); //convert to screen coordinates
		m_pDragImage->DragMove(pt); //move the drag image to those coordinates
		// Unlock window updates (this allows the dragging image to be shown smoothly)
		m_pDragImage->DragShowNolock(false);

		//// Get the CWnd pointer of the window that is under the mouse cursor
		CWnd* pDropWnd = WindowFromPoint (pt);
		ASSERT(pDropWnd); //make sure we have a window

		//// If we drag outside current window we need to adjust the highlights displayed
		if (pDropWnd != m_pDropList)
		{
			if (m_nDropIndex != -1) //If we drag over the CListCtrl header, turn off the hover highlight
			{
				TRACE("m_nDropIndex is -1\n");
				CMFCListCtrl* pList = (CMFCListCtrl*)m_pDropList;
				//VERIFY (pList->SetItemState (m_nDropIndex, 0, LVIS_DROPHILITED));
				// redraw item
				//VERIFY (pList->RedrawItems (m_nDropIndex, m_nDropIndex));
				//pList->UpdateWindow ();
				m_nDropIndex = -1;
			}
			else //If we drag out of the CListCtrl altogether
			{
				TRACE("m_nDropIndex is not -1\n");
				//CListCtrl* pList = (CListCtrl*)m_pDropList;
				//int i = 0;
				//int nCount = pList->GetItemCount();
				//for(i = 0; i < nCount; i++)
				//{
				//	pList->SetItemState(i, 0, LVIS_DROPHILITED);
				//}
				//pList->RedrawItems(0, nCount);
				//pList->UpdateWindow();
			}
		}

		// Save current window pointer as the CListCtrl we are dropping onto
		m_pDropList = (CMFCTextColorListCtrl*)pDropWnd;
		if(m_pDropList != m_pDragList)
		{
			// Convert from screen coordinates to drop target client coordinates
			pDropWnd->ScreenToClient(&pt);

			//If we are hovering over a CListCtrl we need to adjust the highlights
			if(pDropWnd->IsKindOf(RUNTIME_CLASS(CMFCListCtrl)) && (pDropWnd->GetSafeHwnd() == m_wndAssignLoadListCtrl.GetSafeHwnd()))
			{
				TRACE("m_nDropIndex is not 100\n");
				//Note that we can drop here
				SetCursor(LoadCursor(NULL, IDC_ARROW));
				CListCtrl* pList = (CListCtrl*)pDropWnd;
			}
			else if(pDropWnd->IsKindOf(RUNTIME_CLASS(CMFCListCtrl)) && (pDropWnd->GetSafeHwnd() == m_wndRouteGrListCtrl.GetSafeHwnd()))
			{
				TRACE("m_nDropIndex is not 100\n");
				//Note that we can drop here
				SetCursor(LoadCursor(NULL, IDC_ARROW));
				UINT uFlags;
				CListCtrl* pList = (CListCtrl*)pDropWnd;

				// Turn off hilight for previous drop target
				pList->SetItemState (m_nDropIndex, 0, LVIS_DROPHILITED);
				// Redraw previous item
				pList->RedrawItems (m_nDropIndex, m_nDropIndex);

				// Get the item that is below cursor
				m_nDropIndex = ((CListCtrl*)pDropWnd)->HitTest(pt, &uFlags);
				if(-1 != m_nDropIndex)
				{
					// Highlight it
					pList->SetItemState(m_nDropIndex, LVIS_DROPHILITED, LVIS_DROPHILITED);
					// Redraw item
					pList->RedrawItems(m_nDropIndex, m_nDropIndex);
					pList->UpdateWindow();
				}else	SetCursor(LoadCursor(NULL, IDC_NO));
			}
			else if(pDropWnd->IsKindOf(RUNTIME_CLASS(CMFCListCtrl)) && (pDropWnd->GetSafeHwnd() == m_wndNotAssignLoadListCtrl.GetSafeHwnd()))
			{
				TRACE("m_nDropIndex is not 100\n");
				//Note that we can drop here
				SetCursor(LoadCursor(NULL, IDC_ARROW));
				CListCtrl* pList = (CListCtrl*)pDropWnd;
			}
		}
		else
		{
			//If we are not hovering over a CListCtrl, change the cursor
			// to note that we cannot drop here
			SetCursor(LoadCursor(NULL, IDC_NO));
		}
		// Lock window updates
		m_pDragImage->DragShowNolock(true);
	}
		
	CDialog::OnMouseMove(nFlags, point);
}

/**
	@brief	Drag Load List

	@author KHS	

	@return		
**/
void CCableRouteAssignTOLoadDlg::OnLvnBegindragAssignLoadList(NMHDR *pNMHDR, LRESULT *pResult)
{
	//This routine sets the parameters for a Drag and Drop operation.
	//It sets some variables to track the Drag/Drop as well
	// as creating the drag image to be shown during the drag.
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if(NULL == pNMLV) return;

	//// Save the index of the item being dragged in m_nDragIndex
	////  This will be used later for retrieving the info dragged
	m_nDragIndex = pNMLV->iItem;

	OnBeginDrag(pNMLV , m_wndAssignLoadListCtrl);

	*pResult = 0;
}

/**
	@brief	Drag Not Assign List

	@author KHS	

	@return		
**/

void CCableRouteAssignTOLoadDlg::OnLvnBegindragNotAssignList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if(NULL == pNMLV) return;

	OnBeginDrag(pNMLV , m_wndNotAssignLoadListCtrl);

	*pResult = 0;
}


/**
	@brief	BEGIN DRAG

	@author KHS	

	@return		
**/
int CCableRouteAssignTOLoadDlg::OnBeginDrag(LPNMLISTVIEW pNMLV , CMFCTextColorListCtrl& ListCtrl)
{
	//// Save the index of the item being dragged in m_nDragIndex
	////  This will be used later for retrieving the info dragged
	m_nDragIndex = pNMLV->iItem;

	//// Create a drag image
	POINT pt;
	int nOffset = -10; //offset in pixels for drag image (positive is up and to the left; neg is down and to the right)
	if((pNMLV->hdr.hwndFrom == ListCtrl.GetSafeHwnd()) && (ListCtrl.GetSelectedCount() > 0)) //! more than one item is selected
	{
		pt.x = nOffset;
		pt.y = nOffset;

		m_pDragImage = ListCtrl.CreateDragImage(m_nDragIndex, &pt);
		ASSERT(m_pDragImage); //make sure it was created
		//We will call delete later (in LButtonUp) to clean this up

		CBitmap bitmap;
		if(ListCtrl.GetSelectedCount() > 1) //more than 1 item in list is selected
			//bitmap.LoadBitmap(IDB_BITMAP_MULTI);
			bitmap.LoadBitmap(IDB_BITMAP_MULTI_BOXES);
		else	bitmap.LoadBitmap(IDB_BITMAP_BOX);
		m_pDragImage->Replace(0, &bitmap, &bitmap);

		//// Change the cursor to the drag image
		////	(still must perform DragMove() in OnMouseMove() to show it moving)
		m_pDragImage->BeginDrag(0, CPoint(nOffset, nOffset - 4));
		m_pDragImage->DragEnter(GetDesktopWindow(), pNMLV->ptAction);

		//// Set dragging flag and others
		m_bDragging = TRUE;	//we are in a drag and drop operation
		m_nDropIndex = -1;	//we don't have a drop index yet
		m_pDragList = &ListCtrl; //make note of which list we are dragging from
		m_pDropList = &ListCtrl; //at present the drag list is the drop list

		//// Capture all mouse messages
		SetCapture ();
	}

	return ERROR_SUCCESS;
}

/**
	@brief	ListCtrl에서 선택된 아이템을 구한다.

	@author KHS

	@return		
**/
void CCableRouteAssignTOLoadDlg::GetSelectedLoadItems(CMFCListCtrl& ListCtrl , vector<long>& LoadItemIndexEntry)
{
	POSITION pos = ListCtrl.GetFirstSelectedItemPosition();
	while(pos)
	{
		const int nItem  = ListCtrl.GetNextSelectedItem(pos);		
		LoadItemIndexEntry.push_back( nItem );
	}
}
/**
	@brief	

	@author KHS

	@return		
**/
string CCableRouteAssignTOLoadDlg::GetSelectedRouteGroupFromRouteGrListCtrl(void)
{
	POSITION pos = m_wndRouteGrListCtrl.GetFirstSelectedItemPosition();
	while(pos)
	{
		const int nItem = m_wndRouteGrListCtrl.GetNextSelectedItem(pos);
		if(-1 != nItem)
		{
			return (m_wndRouteGrListCtrl.GetItemText(nItem , 0).operator LPCSTR());
		}
	}

	return string();
}
/**
	@brief	called when user drop item.

	@author KHS	

	@return		
**/
void CCableRouteAssignTOLoadDlg::DropItemOnList(CMFCTextColorListCtrl* pDragList , CMFCTextColorListCtrl* pDropList)
{
	vector<long> LoadItemIndexEntry;
	GetSelectedLoadItems(*pDragList , LoadItemIndexEntry);

	if(	(pDropList->GetSafeHwnd() == m_wndAssignLoadListCtrl.GetSafeHwnd()) ||
		(pDropList->GetSafeHwnd() == m_wndRouteGrListCtrl.GetSafeHwnd()) ||
		(pDropList->GetSafeHwnd() == m_wndNotAssignLoadListCtrl.GetSafeHwnd()))
	{
		{
			// Turn off hilight for previous drop target
			pDropList->SetItemState(m_nDropIndex, 0, LVIS_DROPHILITED);
			pDropList->SetItemState(m_nDropIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
			// Redraw previous item
			pDropList->RedrawItems (m_nDropIndex, m_nDropIndex);
			
			CString rString;
			string rDropRouteGroup;
			if(pDropList->GetSafeHwnd() == m_wndAssignLoadListCtrl.GetSafeHwnd())
			{
				rDropRouteGroup = GetSelectedRouteGroupFromRouteGrListCtrl();
				if(rDropRouteGroup.empty()) return;
			}

			for(vector< long >::iterator itr = LoadItemIndexEntry.begin();itr != LoadItemIndexEntry.end();++itr)
			{
				CLoadItem* pLoadItem = (CLoadItem*)pDragList->GetItemData(*itr);
				if(pLoadItem)
				{
					if(pDropList->GetSafeHwnd() == m_wndAssignLoadListCtrl.GetSafeHwnd())
					{
						pLoadItem->prop()->SetValue(_T("Cable Route") , _T("Route Group") , rDropRouteGroup);
					}
					else if(pDropList->GetSafeHwnd() == m_wndRouteGrListCtrl.GetSafeHwnd())
					{
						const string rRouteGroup = pDropList->GetItemText(m_nDropIndex , 0).operator LPCSTR();
						pLoadItem->prop()->SetValue(_T("Cable Route") , _T("Route Group") , rRouteGroup);
						rDropRouteGroup = rRouteGroup;
					}
					else if(pDropList->GetSafeHwnd() == m_wndNotAssignLoadListCtrl.GetSafeHwnd())
					{
						pLoadItem->prop()->SetValue(_T("Cable Route") , _T("Route Group") , _T(""));
						rDropRouteGroup = _T("");
					}
				}
			}

			//! 드래그 한곳의 아이템을 삭제한다.
			for( vector< long >::reverse_iterator itr = LoadItemIndexEntry.rbegin();itr != LoadItemIndexEntry.rend();++itr)
			{
				pDragList->DeleteItem(*itr);
			}

			CString rArea, rBusId;
			int nIndex = m_wndLoadAreaCombo.GetCurSel();
			if(-1 != nIndex) m_wndLoadAreaCombo.GetLBText(nIndex, rArea);
			
			nIndex = m_wndSortByBusCombo.GetCurSel();
			if(-1 != nIndex) m_wndSortByBusCombo.GetLBText(nIndex, rBusId);

			if(!rDropRouteGroup.empty()) DisplayLoadHasRouteGroup(m_wndAssignLoadListCtrl , rDropRouteGroup, "*", "*");
			DisplayLoadHasRouteGroup(m_wndNotAssignLoadListCtrl , _T(""), rArea.operator LPCSTR(), rBusId.operator LPCSTR());
		}
	}
}

/**
	@brief	SAVE OK

	@author KHS	

	@return		
**/
void CCableRouteAssignTOLoadDlg::OnBnClickedOk()
{
	//! Item의 현재 Route Group으로 Map 데이터를 갱신한다.
	for(map<CELoadItem*,string>::iterator itr = m_LoadRouteGroupMap.begin();itr != m_LoadRouteGroupMap.end();++itr)
	{
		const string rRouteGroup = itr->first->prop()->GetValue(_T("Cable Route") , _T("Route Group"));
		itr->second = rRouteGroup;
	}

	///OnOK();
}

/**
	@brief	CANCEL

	@author KHS	

	@return		
**/
void CCableRouteAssignTOLoadDlg::OnBnClickedCancel()
{
	//! Map에 있는 데이터로 Item의 Route Group을 채운다.
	for(map<CELoadItem*,string>::iterator itr = m_LoadRouteGroupMap.begin();itr != m_LoadRouteGroupMap.end();++itr)
	{
		itr->first->prop()->SetValue(_T("Cable Route") , _T("Route Group") , itr->second);
	}

	OnCancel();
}

/**
	@brief	Assigned Load의 RouteGroup을 원상복귀

	@author KHS	

	@date 2009-05-26 오전 10:25:21	

	@param	

	@return		
**/
int CCableRouteAssignTOLoadDlg::MoveToOriginalRouteGroup(void)
{
	for(map<CELoadItem*,string>::iterator itr = m_LoadRouteGroupMap.begin();itr != m_LoadRouteGroupMap.end();++itr)
	{
		itr->first->prop()->SetValue(_T("Cable Route") , _T("Route Group") , itr->second);
	}

	DisplayRouteGroupData();
	
	CString rLoadArea, rBusId;
	int nSelItem = m_wndLoadAreaCombo.GetCurSel();
	if( -1 != nSelItem) m_wndLoadAreaCombo.GetLBText(nSelItem , rLoadArea);

	nSelItem = m_wndSortByBusCombo.GetCurSel();
	if(-1 != nSelItem) m_wndSortByBusCombo.GetLBText(nSelItem, rBusId);

	if(m_wndRouteGrListCtrl.GetItemCount() > 0)
	{
		const string rRouteGroup = m_wndRouteGrListCtrl.GetItemText(0 , 0);
		m_wndRouteGrListCtrl.SetFocus();
		// Turn off hilight for previous drop target
		m_wndRouteGrListCtrl.SetItemState(0, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		// Redraw previous item
		m_wndRouteGrListCtrl.RedrawItems (0, 0);
		
		DisplayLoadHasRouteGroup(m_wndAssignLoadListCtrl , rRouteGroup, rLoadArea.operator LPCSTR(), rBusId.operator LPCSTR());
	}
	DisplayLoadHasRouteGroup(m_wndNotAssignLoadListCtrl , _T(""), rLoadArea.operator LPCSTR(), rBusId.operator LPCSTR());

	return ERROR_SUCCESS;
}

/**
	@brief	SORT BY BUS COMBOBOX Select Change

	@author KHS	

	@date 2009-06-01 오전 9:25:53	

	@param	

	@return		
**/
void CCableRouteAssignTOLoadDlg::OnCbnSelchangeLoadBusCombo()
{
	// TODO: Add your control notification handler code here
	int nItem = m_wndLoadAreaCombo.GetCurSel();
	if(-1 != nItem)
	{
		CString rArea, rBusId;
		m_wndLoadAreaCombo.GetLBText(nItem, rArea);

		nItem = m_wndSortByBusCombo.GetCurSel();
		if(-1 != nItem) m_wndSortByBusCombo.GetLBText(nItem, rBusId);
		DisplayLoadHasRouteGroup(m_wndNotAssignLoadListCtrl, "", rArea.operator LPCSTR(), rBusId.operator LPCSTR());
	}
}
