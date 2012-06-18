// AssignLoadToBusDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELoad.h"
#include "MainFrm.h"
#include "AssignLoadToBusDlg.h"
#include "ELoadDocData.h"

// CAssignLoadToBusDlg dialog

IMPLEMENT_DYNCREATE(CAssignLoadToBusDlg, CDialog)

CAssignLoadToBusDlg::CAssignLoadToBusDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAssignLoadToBusDlg::IDD, pParent) , m_wndResize(TRUE)
{

}

CAssignLoadToBusDlg::~CAssignLoadToBusDlg()
{
}

BEGIN_EASYSIZE_MAP(CAssignLoadToBusDlg)
	EASYSIZE(IDC_STATIC_VIRTUAL_LOAD_ASSIGNMENT,ES_BORDER,ES_BORDER, ES_BORDER,ES_BORDER,0)

	EASYSIZE(IDC_LIST_BUS,ES_BORDER,ES_BORDER, IDC_LIST_LOAD,ES_BORDER,0)
	EASYSIZE(IDC_LIST_LOAD,IDC_LIST_BUS,ES_BORDER, ES_BORDER,ES_BORDER,0)
	
	EASYSIZE(IDC_STATIC_NOT_ASSIGNED,ES_BORDER,IDC_LIST_BUS,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_LIST_NOT_ASSIGNED_LOAD,ES_BORDER,IDC_STATIC_NOT_ASSIGNED,IDC_LIST_LOAD,ES_BORDER,0)

	EASYSIZE(IDC_STATIC_MOVE_TO_ORIGINAL_BUS,ES_BORDER,ES_KEEPSIZE, ES_KEEPSIZE,ES_BORDER,0)
	EASYSIZE(IDC_BUTTON_MOVE_TO_ORIGINAL_BUS,ES_BORDER,ES_KEEPSIZE, ES_KEEPSIZE,ES_BORDER,0)

	EASYSIZE(IDOK,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDCANCEL,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP

void CAssignLoadToBusDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX , IDC_LIST_BUS , m_wndBusListCtrl);
	DDX_Control(pDX , IDC_LIST_LOAD , m_wndLoadListCtrl);
	DDX_Control(pDX , IDC_LIST_NOT_ASSIGNED_LOAD , m_wndNotAssignedLoadListCtrl);
	
	DDX_Control(pDX , IDC_COMBO_AREA , m_wndAreaCombo);

	DDX_Control(pDX , IDC_BUTTON_MOVE_TO_ORIGINAL_BUS , m_wndMoveToOriginalBus);
	
	DDX_Control(pDX , IDOK , m_wndOKButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);

	CDialog::DoDataExchange(pDX);
}

/**
	@brief	

	@author	BHK
*/
BOOL CAssignLoadToBusDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	BeginWaitCursor();

	m_hIcon = AfxGetApp()->LoadIcon(IDI_GRABBER);
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();

	m_wndLoadListCtrl.SetImageList (&(pApp->m_ELoadImages), LVSIL_SMALL);
	m_wndLoadListCtrl.Sort(0);

	m_wndBusListCtrl.InsertColumn(0 , _T("Bus ID.")  , LVCFMT_LEFT , 100);
        m_wndBusListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES);
	m_wndBusListCtrl.SetImageList (&(pApp->m_ELoadImages), LVSIL_SMALL);

	m_wndLoadListCtrl.InsertColumn(0 , _T("Load ID.")  , LVCFMT_LEFT , 100);
	m_wndLoadListCtrl.InsertColumn(1 , _T("Voltage") , LVCFMT_LEFT , 50);	//! 2011.01.25 - added by HumKyung
	m_wndLoadListCtrl.InsertColumn(2 , _T("Rating") , LVCFMT_LEFT , 50);
	m_wndLoadListCtrl.InsertColumn(3 , _T("Original Bus ID.") , LVCFMT_LEFT , 100);
        m_wndLoadListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

	m_wndNotAssignedLoadListCtrl.InsertColumn(0 , _T("Load ID.")  , LVCFMT_LEFT , 100);
	m_wndNotAssignedLoadListCtrl.InsertColumn(1 , _T("Rating") , LVCFMT_LEFT , 50);
	m_wndNotAssignedLoadListCtrl.InsertColumn(2 , _T("Original Bus ID.") , LVCFMT_LEFT , 100);
        m_wndNotAssignedLoadListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
	m_wndNotAssignedLoadListCtrl.SetImageList (&(pApp->m_ELoadImages), LVSIL_SMALL);
	m_wndNotAssignedLoadListCtrl.Sort(0);

	CRect rect1 , rect2;
	m_wndBusListCtrl.GetWindowRect(&rect1);
	ScreenToClient(&rect1);
	m_wndNotAssignedLoadListCtrl.GetWindowRect(&rect2);
	ScreenToClient(&rect2);
	m_wndResize.Create( NULL, "", WS_VISIBLE | WS_CHILD,CRect(rect1.right + 3,rect1.top,rect1.right+6,rect2.bottom), this , ID_RESIZE );

	DisplayBusItem(false);
	DisplayLoadInBus(m_wndNotAssignedLoadListCtrl , NOT_ASSIGNED);

	CELoadDocData& docData = CELoadDocData::GetInstance();
	//! load의 bus id 속성을 미리 저장해 둔다.
	list<CELoadItem*> LoadItemList;
	docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
	///if(pItemList)
	{
		for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
		{
			if((*itr)->IsDeleted()) continue;

			//! Area ComboBox를 채운다.
			const string rArea  = (*itr)->prop()->GetValue(_T("Location") , _T("Area"));
			if(!rArea.empty())
			{
				const int nItem = m_wndAreaCombo.FindString(-1 , rArea.c_str());
				if(-1 == nItem) m_wndAreaCombo.AddString(rArea.c_str());
			}

			BusInfo busInfo;
			busInfo.rBusID = (*itr)->prop()->GetValue(_T("Panel") , _T("Bus ID"));
			busInfo.rLoadVolt = (*itr)->prop()->GetValue(_T("Rating") , _T("Rated Voltage"));
			busInfo.rPhase = (*itr)->prop()->GetValue(_T("Rating") , _T("Phase"));
			m_LoadBusInfoMap.insert(make_pair(*itr , busInfo));
		}
		const int nItem = m_wndAreaCombo.AddString(_T("*"));
		m_wndAreaCombo.SetCurSel(nItem);
	}

	/////////////////////////////////////////////
	//// Set up initial variables
	m_bDragging = false;
	m_nDragIndex = -1;
	m_nDropIndex = -1;
	m_pDragImage = NULL;

	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
        m_wndOKButton.SetIcon(IDI_OK_18X18);
	m_wndOKButton.SetTextHotColor(TextHotColor);
	m_wndCancelButton.SetIcon(IDI_CANCEL_18X18);
	m_wndCancelButton.SetTextHotColor(TextHotColor);

	EndWaitCursor();

	INIT_EASYSIZE;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CAssignLoadToBusDlg, CDialog)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_LOAD, OnBegindragListRight)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDOK, &CAssignLoadToBusDlg::OnBnClickedOk)
	ON_NOTIFY(NM_CLICK, IDC_LIST_BUS, &CAssignLoadToBusDlg::OnNMClickListBus)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_NOT_ASSIGNED_LOAD, &CAssignLoadToBusDlg::OnLvnBegindragListNotAssignedLoad)
	ON_COMMAND(ID_RESIZE, OnVertResize)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_TO_ORIGINAL_BUS, &CAssignLoadToBusDlg::OnBnClickedButtonMoveToOriginalBus)
	ON_BN_CLICKED(IDCANCEL, &CAssignLoadToBusDlg::OnBnClickedCancel)
	ON_CBN_SELENDOK(IDC_COMBO_AREA, &CAssignLoadToBusDlg::OnCbnSelendokComboArea)
END_MESSAGE_MAP()


/**
	@brief	

	@author	HumKyung.BAEK
*/
void CAssignLoadToBusDlg::OnMouseMove(UINT nFlags, CPoint point) 
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

		// Convert from screen coordinates to drop target client coordinates
		pDropWnd->ScreenToClient(&pt);

		//If we are hovering over a CListCtrl we need to adjust the highlights
		if(pDropWnd->IsKindOf(RUNTIME_CLASS(CMFCListCtrl)) && (pDropWnd->GetSafeHwnd() == m_wndBusListCtrl.GetSafeHwnd()))
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
		else if(pDropWnd->IsKindOf(RUNTIME_CLASS(CMFCListCtrl)) && (pDropWnd->GetSafeHwnd() == m_wndLoadListCtrl.GetSafeHwnd()))
		{
			TRACE("m_nDropIndex is not 100\n");
			//Note that we can drop here
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			CListCtrl* pList = (CListCtrl*)pDropWnd;
			
			/*
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
			*/
		}
		else if(pDropWnd->IsKindOf(RUNTIME_CLASS(CMFCListCtrl)) && (pDropWnd->GetSafeHwnd() == m_wndNotAssignedLoadListCtrl.GetSafeHwnd()))
		{
			TRACE("m_nDropIndex is not 100\n");
			//Note that we can drop here
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			CListCtrl* pList = (CListCtrl*)pDropWnd;

			/*
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
			*/
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
	@brief	

	@author	HumKyung.BAEK
*/
void CAssignLoadToBusDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
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
		if (pDropWnd->IsKindOf (RUNTIME_CLASS (CMFCListCtrl)))
		{
			m_pDropList = (CMFCTextColorListCtrl*)pDropWnd; //Set pointer to the list we are dropping on
			DropItemOnList(m_pDragList, m_pDropList); //Call routine to perform the actual drop
		}
	}
	
	CDialog::OnLButtonUp(nFlags, point);
}

/**
	@brief	called when user drop item.

	@author	HumKyung.BAEK

	@date	????.??.??

	@param	pDragList
	@param	pDropList

	@return
*/
void CAssignLoadToBusDlg::DropItemOnList(CMFCTextColorListCtrl* pDragList , CMFCTextColorListCtrl* pDropList)
{
	vector<long> LoadItemIndexEntry;
	GetSelectedLineDataRecord(*pDragList , LoadItemIndexEntry);

	if((pDropList->GetSafeHwnd() == m_wndBusListCtrl.GetSafeHwnd()) || 
		(pDropList->GetSafeHwnd() == m_wndLoadListCtrl.GetSafeHwnd()) ||
		(pDropList->GetSafeHwnd() == m_wndNotAssignedLoadListCtrl.GetSafeHwnd()))
	{
		///if(-1 != m_wndBusListCtrl.HitTest(point , &uFlags))
		{
			// Turn off hilight for previous drop target
			pDropList->SetItemState (m_nDropIndex, 0, LVIS_DROPHILITED);
			// Redraw previous item
			pDropList->RedrawItems (m_nDropIndex, m_nDropIndex);

			CString rString;
			string rDropBusID;
			for( vector< long >::iterator itr = LoadItemIndexEntry.begin();itr != LoadItemIndexEntry.end();++itr)
			{
				CLoadItem* pLoadItem = (CLoadItem*)pDragList->GetItemData(*itr);
				if(pLoadItem)
				{
					if(pDropList->GetSafeHwnd() == m_wndBusListCtrl.GetSafeHwnd())
					{
						const string rBusID = pDropList->GetItemText(m_nDropIndex , 0).operator LPCSTR();
						CBusItem* pBusItem = (CBusItem*)(pDropList->GetItemData(m_nDropIndex));
						if(pBusItem)
						{
							const string rLoadVolt = pBusItem->prop()->GetValue(_T("General") , _T("Load Voltage"));
							const string rPhase = pBusItem->prop()->GetValue(_T("General") , _T("Phase"));

							pLoadItem->prop()->SetValue(_T("Panel") , _T("Bus ID") , rBusID);
							pLoadItem->prop()->SetValue(_T("Rating") ,_T("Rated Voltage") , rLoadVolt);
							pLoadItem->prop()->SetValue(_T("Rating") ,_T("Phase") , rPhase);
						}

						rDropBusID = rBusID;
					}
					else if(pDropList->GetSafeHwnd() == m_wndLoadListCtrl.GetSafeHwnd())
					{
						POSITION pos = m_wndBusListCtrl.GetFirstSelectedItemPosition();
						while(pos)
						{
							const int nItem  = m_wndBusListCtrl.GetNextSelectedItem(pos);
							const string rBusID = m_wndBusListCtrl.GetItemText(nItem , 0).operator LPCSTR();
							
							CBusItem* pBusItem = (CBusItem*)(m_wndBusListCtrl.GetItemData(nItem));
							if(pBusItem)
							{
								const string sLoadVolt = pBusItem->prop()->GetValue(_T("General") , _T("Load Voltage"));
								const string rPhase = pBusItem->prop()->GetValue(_T("General") , _T("Phase"));

								pLoadItem->prop()->SetValue(_T("Panel") , _T("Bus ID") , rBusID);
								pLoadItem->prop()->SetValue(_T("Rating") ,_T("Rated Voltage") , sLoadVolt);
								pLoadItem->prop()->SetValue(_T("Rating") ,_T("Phase") , rPhase);
							}

							rDropBusID = rBusID;
						}
					}
					else if(pDropList->GetSafeHwnd() == m_wndNotAssignedLoadListCtrl.GetSafeHwnd())
					{
						pLoadItem->prop()->SetValue(_T("Panel") , _T("Bus ID") , _T(""));
						rDropBusID = _T("");
					}
				}
			}
			
			for( vector< long >::reverse_iterator itr = LoadItemIndexEntry.rbegin();itr != LoadItemIndexEntry.rend();++itr)
			{
				pDragList->DeleteItem(*itr);
			}
			
			POSITION pos = m_wndBusListCtrl.GetFirstSelectedItemPosition();
			while(pos)
			{
				const int nItem  = m_wndBusListCtrl.GetNextSelectedItem(pos);		
				const string rBusID = m_wndBusListCtrl.GetItemText(nItem , 0).operator LPCSTR();
				DisplayLoadInBus(m_wndLoadListCtrl , rBusID);
				break;
			}
			if(pDropList->GetSafeHwnd() == m_wndNotAssignedLoadListCtrl.GetSafeHwnd())
			{
				CString rArea = _T("*");
				const int nItem = m_wndAreaCombo.GetCurSel();
				if(-1 != nItem) m_wndAreaCombo.GetLBText(nItem , rArea);
				DisplayLoadInBus(*pDropList , NOT_ASSIGNED , rArea.operator LPCSTR());
			}
		}
	}
	else
	{
		for(vector<long>::iterator itr = LoadItemIndexEntry.begin();itr != LoadItemIndexEntry.end();++itr)
		{
			m_wndLoadListCtrl.SetItemState(*itr, 0, LVIS_DROPHILITED);
		}
		m_wndLoadListCtrl.RedrawItems(0, m_wndLoadListCtrl.GetItemCount());
		m_wndLoadListCtrl.UpdateWindow();
	}
}

/**
	@brief	

	@author	BHK
*/
void CAssignLoadToBusDlg::OnBegindragListRight(NMHDR* pNMHDR, LRESULT* pResult) 
{
	//This routine sets the parameters for a Drag and Drop operation.
	//It sets some variables to track the Drag/Drop as well
	// as creating the drag image to be shown during the drag.
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if(NULL == pNMLV) return;

	//// Save the index of the item being dragged in m_nDragIndex
	////  This will be used later for retrieving the info dragged
	m_nDragIndex = pNMLV->iItem;

	//// Create a drag image
	POINT pt;
	int nOffset = -10; //offset in pixels for drag image (positive is up and to the left; neg is down and to the right)
	if((pNMLV->hdr.hwndFrom == m_wndLoadListCtrl.GetSafeHwnd()) && (m_wndLoadListCtrl.GetSelectedCount() > 0)) //! more than one item is selected
	{
		pt.x = nOffset;
		pt.y = nOffset;

		m_pDragImage = m_wndLoadListCtrl.CreateDragImage(m_nDragIndex, &pt);
		ASSERT(m_pDragImage); //make sure it was created
		//We will call delete later (in LButtonUp) to clean this up

		CBitmap bitmap;
		if(m_wndLoadListCtrl.GetSelectedCount() > 1) //more than 1 item in list is selected
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
		m_pDragList = &m_wndLoadListCtrl; //make note of which list we are dragging from
		m_pDropList =  &m_wndBusListCtrl;	//at present the drag list is the drop list

		//// Capture all mouse messages
		SetCapture ();
	}

	*pResult = 0;
}

/**
	@brief	

	@author	BHK
*/
int CAssignLoadToBusDlg::AddLoadItemToListCtrl(CLoadItem* pLoadItem)
{
	assert(pLoadItem && "pLoadItem is NULL");

	if(pLoadItem)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		
		pLoadItem->SetBusId(_T(""));
		const int nImage = docData.GetIconNo(pLoadItem);
		const int nItem = m_wndLoadListCtrl.InsertItem(0 , pLoadItem->GetName().c_str() , nImage);
		m_wndLoadListCtrl.SetItemData(nItem , (DWORD_PTR)pLoadItem);
		m_wndLoadListCtrl.RedrawWindow();

		m_wndLoadListCtrl.Sort(0 , m_wndLoadListCtrl.GetHeaderCtrl().IsAscending());
		
		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-13 오전 9:49:04	

	@param	

	@return		
*/
void CAssignLoadToBusDlg::OnBnClickedOk()
{
	OnOK();	
}

/**
	@brief	

	@author HumKyung.BAEK	

	@date 2009-04-11 오전 9:04:34	

	@param	

	@return		
**/
int CAssignLoadToBusDlg::DisplayBusItem(const bool& bUpdate)
{
	m_wndBusListCtrl.SetRedraw(FALSE);
	if(false == bUpdate) m_wndBusListCtrl.DeleteAllItems();
	
	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> BusGroupItemList;
	docData.GetELoadItemListOf(BusGroupItemList , CBusGroupItem::TypeString());
	//if(pItemList)
	{
		int i = 0;
		for(list<CELoadItem*>::iterator itr = BusGroupItemList.begin();itr != BusGroupItemList.end();++itr)
		{
			CBusGroupItem* pBusGroupItem = static_cast<CBusGroupItem*>(*itr);
			if(pBusGroupItem->IsDeleted() || (INCOMING == pBusGroupItem->GetName())) continue;

			for(vector<CBusItem*>::iterator jtr = pBusGroupItem->m_BusRefEntry.begin();jtr != pBusGroupItem->m_BusRefEntry.end();++jtr)
			{
				if((*jtr)->IsDeleted()) continue;

				const string rBusID = (*jtr)->GetName();
				double nKW = 0.f , nKVAR = 0.f;
				(*jtr)->GetLoadSummaryResultData(nKW , nKVAR);
				const double nKVA = SAFE_ROUND(sqrt(nKW*nKW + nKVAR*nKVAR) , 2);

				ostringstream oss;
				oss << nKVA;
				if(false == bUpdate)
				{
					const int nIconNo = docData.GetIconNo(*jtr);
					const int nItem = m_wndBusListCtrl.InsertItem(i++,rBusID.c_str(), nIconNo);
					if(-1 != nItem)
					{
						m_wndBusListCtrl.SetItemText(nItem , 1 , oss.str().c_str());
						m_wndBusListCtrl.SetItemText(nItem , 2 , oss.str().c_str());
						m_wndBusListCtrl.SetItemData(nItem , DWORD_PTR(*jtr));
					}
				}
				else
				{
					LVFINDINFO info;
					info.flags = LVFI_PARTIAL|LVFI_STRING;
					info.psz = rBusID.c_str();

					const int nItem = m_wndBusListCtrl.FindItem(&info);
					if(-1 != nItem)
					{
						m_wndBusListCtrl.SetItemText(nItem , 2 , oss.str().c_str());
					}
				}
			}
		}
	}
	m_wndBusListCtrl.SetRedraw(TRUE);

	return ERROR_SUCCESS;
}

/**
	@brief	Bus ID를 가진 Load를 표시한다.

	@author BHK	

	@date 2009-04-11 오전 9:06:00	

	@param	

	@return		
**/
int CAssignLoadToBusDlg::DisplayLoadInBus(CMFCTextColorListCtrl& ListCtrl , const string& rBusID , const string& rInterestingArea)
{
	CItemSelectionSet SelectionSet;
	CELoadDocData& docData = CELoadDocData::GetInstance();
	docData.GetLoadItemSetInBus(SelectionSet , rBusID);

	ListCtrl.SetRedraw(FALSE);
	ListCtrl.DeleteAllItems();
	ListCtrl.m_ItemColorMap.clear();
	for(int i = 0;i < SelectionSet.GetItemCount();++i)
	{
		CLoadItem* pLoadItem = static_cast<CLoadItem*>(SelectionSet.GetItemAt(i));
		if(pLoadItem)
		{
			CELoadItemProp *pItemProp = pLoadItem->prop();
			const string rArea = pItemProp->GetValue(_T("Location") , _T("Area"));
			if((_T("*") != rInterestingArea) && (rArea != rInterestingArea)) continue;
			
			const int nImage = docData.GetIconNo(pLoadItem);
			const int nItem = ListCtrl.InsertItem(0 , pLoadItem->GetName().c_str() , nImage);
			if(-1 != nItem)
			{
				ListCtrl.SetItemData(nItem , DWORD_PTR(pLoadItem));

				//! 2011.01.25 - added by HumKyung
				const string sRatedVoltage = pItemProp->GetValue(_T("Rating") , _T("Rated Voltage"));
				ListCtrl.SetItemText(nItem , 1 , sRatedVoltage.c_str());
				//! 

				double nRatingCapacity = 0.f;
				pLoadItem->CalcRatingCapacity(nRatingCapacity);
				
				ostringstream oss;
				oss << nRatingCapacity;
				ListCtrl.SetItemText(nItem , 2 , oss.str().c_str());
				
				map<CELoadItem*,BusInfo>::iterator where = m_LoadBusInfoMap.find(pLoadItem);
				if(where != m_LoadBusInfoMap.end())
				{
					ListCtrl.SetItemText(nItem , 3 , where->second.rBusID.c_str());

					if(pLoadItem->GetBusId() != where->second.rBusID)
					{
						ListCtrl.m_ItemColorMap[DWORD_PTR(pLoadItem)].push_back(RGB(255,0,0));
						ListCtrl.m_ItemColorMap[DWORD_PTR(pLoadItem)].push_back(RGB(255,0,0));
						ListCtrl.m_ItemColorMap[DWORD_PTR(pLoadItem)].push_back(RGB(255,0,0));
						ListCtrl.m_ItemColorMap[DWORD_PTR(pLoadItem)].push_back(RGB(255,0,0));
					}
					else
					{
						ListCtrl.m_ItemColorMap[DWORD_PTR(pLoadItem)].push_back(RGB(0,0,0));
						ListCtrl.m_ItemColorMap[DWORD_PTR(pLoadItem)].push_back(RGB(0,0,0));
						ListCtrl.m_ItemColorMap[DWORD_PTR(pLoadItem)].push_back(RGB(0,0,0));
						ListCtrl.m_ItemColorMap[DWORD_PTR(pLoadItem)].push_back(RGB(0,0,0));
					}
				}
			}
		}
	}
	ListCtrl.SetRedraw(TRUE);
	ListCtrl.RedrawItems(0 , m_wndLoadListCtrl.GetItemCount());
	
	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-11 오전 9:07:41	

	@param	

	@return		
**/
void CAssignLoadToBusDlg::OnNMClickListBus(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	if(-1 != pNMItemActivate->iItem)
	{
		///m_nCurBusSel = pNMItemActivate->iItem;

		CBusItem* pBusItem = (CBusItem*)(m_wndBusListCtrl.GetItemData(pNMItemActivate->iItem));
		if(pBusItem) DisplayLoadInBus(m_wndLoadListCtrl , pBusItem->GetName());
	}

	*pResult = 0;
}

/**
	@brief	ListCtrl에서 선택된 아이템을 구한다.

	@author BHK	

	@date 2009-04-11 오전 9:24:22	

	@param	

	@return		
**/
void CAssignLoadToBusDlg::GetSelectedLineDataRecord(CMFCListCtrl& ListCtrl , vector<long>& LoadItemIndexEntry)
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

	@author BHK	

	@date 2009-04-11 오후 2:55:57	

	@param	

	@return		
**/
BOOL CAssignLoadToBusDlg::OnEraseBkgnd(CDC* pDC)
{
	const BOOL res = CDialog::OnEraseBkgnd(pDC);

	if(m_hIcon)
	{
		CRect rect;
		GetClientRect(&rect);
		int res = ::DrawIconEx(pDC->GetSafeHdc(), rect.Width() - 16 , rect.Height() - 16 , m_hIcon , 16 , 16 , 0 , NULL , DI_NORMAL);
	}

	return res;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-11 오후 2:59:07	

	@param	

	@return		
**/
void CAssignLoadToBusDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	UPDATE_EASYSIZE;

	if(m_wndResize.GetSafeHwnd())
	{
		CRect rect1,rect2;
		m_wndBusListCtrl.GetWindowRect(&rect1);
		ScreenToClient(&rect1);
		m_wndNotAssignedLoadListCtrl.GetWindowRect(&rect2);
		ScreenToClient(&rect2);

		m_wndResize.SetWindowPos( NULL, rect1.right + 3 , rect1.top , 3 , rect2.bottom - rect1.top , SWP_NOZORDER | SWP_NOACTIVATE );
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-04-11 오후 3:00:00	

	@param	

	@return		
**/
void CAssignLoadToBusDlg::OnLvnBegindragListNotAssignedLoad(NMHDR *pNMHDR, LRESULT *pResult)
{
	//This routine sets the parameters for a Drag and Drop operation.
	//It sets some variables to track the Drag/Drop as well
	// as creating the drag image to be shown during the drag.
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if(NULL == pNMLV) return;

	OnBeginDrag(pNMLV , m_wndNotAssignedLoadListCtrl);

	*pResult = 0;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-11 오후 3:02:35	

	@param	

	@return		
**/
int CAssignLoadToBusDlg::OnBeginDrag(LPNMLISTVIEW pNMLV , CMFCTextColorListCtrl& ListCtrl)
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
	@brief	

	@author BHK	

	@date 2009-04-07 오후 7:10:32	

	@param	

	@return		
*/
void CAssignLoadToBusDlg::OnVertResize() 
{
	CRect rect;
	m_wndResize.GetWindowRect( rect );
	ScreenToClient( rect );

	m_wndResize.SetWindowPos(NULL , rect.left + m_wndResize.m_adjust_width, rect.top , 3 , rect.Height() , SWP_NOZORDER | SWP_NOSIZE);

	CRect wndRect;
	GetClientRect(&wndRect);
	DetermineLayout(wndRect.Width() , wndRect.Height());
}

/**
	@brief	

	@author BHK	

	@date 2009-04-07 오후 7:08:08	

	@param	

	@return		
*/
int CAssignLoadToBusDlg::DetermineLayout(int cx, int cy)
{
	CRect rect1 , rect2;
	m_wndResize.GetWindowRect(&rect1);
	ScreenToClient(&rect1);

	m_wndBusListCtrl.GetWindowRect(&rect2);
	ScreenToClient(&rect2);
	cx = rect1.left - rect2.left;
	m_wndBusListCtrl.SetWindowPos(NULL , rect2.left , rect2.top , cx - 3 ,rect2.Height(),SWP_NOZORDER | SWP_NOMOVE);
	m_wndBusListCtrl.RedrawWindow();

	m_wndNotAssignedLoadListCtrl.GetWindowRect(&rect2);
	ScreenToClient(&rect2);
	cx = rect1.left - rect2.left;
	m_wndNotAssignedLoadListCtrl.SetWindowPos(NULL , rect2.left , rect2.top , cx - 3 ,rect2.Height(),SWP_NOZORDER | SWP_NOMOVE);
	m_wndNotAssignedLoadListCtrl.RedrawWindow();

	m_wndLoadListCtrl.GetWindowRect(&rect2);
	ScreenToClient(&rect2);
	cx = rect2.right - rect1.right;
	m_wndLoadListCtrl.SetWindowPos(NULL , rect1.right + 3 , rect2.top , cx - 3 ,rect2.Height(),SWP_NOZORDER);
	m_wndLoadListCtrl.RedrawWindow();

	return ERROR_SUCCESS;
}

/**
	@brief	load의 bus id를 assign 이전의 값으로 되돌린다.

	@author BHK	

	@date 2009-04-13 오전 9:27:20	

	@param	

	@return		
*/
void CAssignLoadToBusDlg::OnBnClickedButtonMoveToOriginalBus()
{
	MoveToOriginalBus();
}

/**
	@brief	return load information to original value.

	@author BHK	

	@date 2009-04-13 오전 9:28:28	

	@param	

	@return		
*/
int CAssignLoadToBusDlg::MoveToOriginalBus(void)
{
	for(map<CELoadItem*,BusInfo>::iterator itr = m_LoadBusInfoMap.begin();itr != m_LoadBusInfoMap.end();++itr)
	{
		itr->first->prop()->SetValue(_T("Panel") , _T("Bus ID") , itr->second.rBusID);
		
		itr->first->prop()->SetValue(_T("Rating") , _T("Rated Voltage") , itr->second.rLoadVolt);
		itr->first->prop()->SetValue(_T("Rating") , _T("Phase") , itr->second.rPhase);
	}

	DisplayBusItem(true);
	DisplayLoadInBus(m_wndNotAssignedLoadListCtrl , NOT_ASSIGNED);

	return ERROR_SUCCESS;
}

/**
	@brief	assign한 내용을 무효화하고 다이얼로그를 종료한다.

	@author BHK	

	@date 2009-04-13 오전 9:42:26	

	@param	

	@return		
*/
void CAssignLoadToBusDlg::OnBnClickedCancel()
{
	MoveToOriginalBus();

	OnCancel();
}

/**
	@brief	

	@author BHK	

	@date 2009-05-28 오후 4:05:17	

	@param	

	@return		
*/
void CAssignLoadToBusDlg::OnCbnSelendokComboArea()
{
	CString rArea = _T("*");
	const int nItem = m_wndAreaCombo.GetCurSel();
	if(-1 != nItem)
	{
		m_wndAreaCombo.GetLBText(nItem , rArea);
		DisplayLoadInBus(m_wndNotAssignedLoadListCtrl , NOT_ASSIGNED , rArea.operator LPCSTR());
	}
}
