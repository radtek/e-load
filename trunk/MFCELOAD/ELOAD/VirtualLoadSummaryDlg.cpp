// VirtualLoadSummaryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "MainFrm.h"
#include "VirtualLoadSummaryDlg.h"
#include "ELoadDocData.h"

int CVirtualLoadSummaryDlg::CX = 0;
int CVirtualLoadSummaryDlg::CY = 0;
// CVirtualLoadSummaryDlg dialog

IMPLEMENT_DYNAMIC(CVirtualLoadSummaryDlg, CDialog)

CVirtualLoadSummaryDlg::CVirtualLoadSummaryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVirtualLoadSummaryDlg::IDD, pParent) , m_wndResize(TRUE)
{
	m_bDragging = FALSE;
}

CVirtualLoadSummaryDlg::~CVirtualLoadSummaryDlg()
{
}

void CVirtualLoadSummaryDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX , IDC_LIST_BUS , m_wndBusListCtrl);
	DDX_Control(pDX , IDC_LIST_LOAD , m_wndLoadListCtrl);
	DDX_Control(pDX , IDC_LIST_TRANSFORMER , m_wndTransformerListCtrl);
	
	DDX_Control(pDX , IDC_BUTTON_MOVE_TO_ORIGINAL_BUS , m_wndMoveToOriginalBus);
	
	DDX_Control(pDX , IDOK , m_wndOKButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);

	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CVirtualLoadSummaryDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_LIST_BUS, &CVirtualLoadSummaryDlg::OnNMClickListBus)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_LOAD, &CVirtualLoadSummaryDlg::OnLvnBegindragListLoad)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_BUTTON_MOVE_TO_ORIGINAL_BUS, &CVirtualLoadSummaryDlg::OnBnClickedButtonMoveToOriginalBus)
	ON_BN_CLICKED(IDOK, &CVirtualLoadSummaryDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CVirtualLoadSummaryDlg::OnBnClickedCancel)
	ON_COMMAND(ID_RESIZE, OnVertResize)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CVirtualLoadSummaryDlg)
	EASYSIZE(IDC_STATIC_VIRTUAL_LOAD_ASSIGNMENT,ES_BORDER,ES_BORDER, ES_BORDER,ES_BORDER,0)

	EASYSIZE(IDC_LIST_BUS,ES_BORDER,ES_BORDER, IDC_LIST_LOAD,ES_BORDER,0)
	EASYSIZE(IDC_LIST_LOAD,IDC_LIST_BUS,ES_BORDER, ES_BORDER,ES_BORDER,0)
	
	EASYSIZE(IDC_STATIC_TRANSFORMER_CAPACITY,ES_BORDER,IDC_LIST_BUS,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_LIST_TRANSFORMER,ES_BORDER,IDC_STATIC_TRANSFORMER_CAPACITY,IDC_LIST_LOAD,ES_BORDER,0)

	EASYSIZE(IDC_STATIC_MOVE_TO_ORIGINAL_BUS,ES_BORDER,ES_KEEPSIZE, ES_KEEPSIZE,ES_BORDER,0)
	EASYSIZE(IDC_BUTTON_MOVE_TO_ORIGINAL_BUS,ES_BORDER,ES_KEEPSIZE, ES_KEEPSIZE,ES_BORDER,0)

	EASYSIZE(IDOK,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDCANCEL,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP

// CVirtualLoadSummaryDlg message handlers

/**
	@brief	다이얼로그 초기화.

	@author BHK	

	@date 2009-04-03 오후 1:50:17	

	@param	

	@return		
*/
BOOL CVirtualLoadSummaryDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_wndBusListCtrl.InsertColumn(0 , _T("Bus ID.")  , LVCFMT_LEFT , 100);
	m_wndBusListCtrl.InsertColumn(1 , _T("Calculated [KVA]") , LVCFMT_RIGHT , 110);
	m_wndBusListCtrl.InsertColumn(2 , _T("Changed [KVA]") , LVCFMT_RIGHT , 110);
	m_wndBusListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES);

	m_wndLoadListCtrl.InsertColumn(0 , _T("Load ID.")  , LVCFMT_LEFT , 100);
	m_wndLoadListCtrl.InsertColumn(1 , _T("Rating") , LVCFMT_RIGHT , 50);
	m_wndLoadListCtrl.InsertColumn(2 , _T("Original Bus ID.") , LVCFMT_LEFT , 100);
	m_wndLoadListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

	m_wndTransformerListCtrl.InsertColumn(0 , _T("TR. ID.")  , LVCFMT_LEFT , 100);
	m_wndTransformerListCtrl.InsertColumn(1 , _T("Calculated [KVA]") , LVCFMT_RIGHT , 110);
	m_wndTransformerListCtrl.InsertColumn(2 , _T("Changed [KVA].") , LVCFMT_RIGHT , 110);
	m_wndTransformerListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

	CRect rect1 , rect2;
	m_wndBusListCtrl.GetWindowRect(&rect1);
	ScreenToClient(&rect1);
	m_wndTransformerListCtrl.GetWindowRect(&rect2);
	ScreenToClient(&rect2);
	m_wndResize.Create( NULL, "", WS_VISIBLE | WS_CHILD,CRect(rect1.right + 3,rect1.top,rect1.right+6,rect2.bottom), this , ID_RESIZE );

	//! ICON
	SetIcon(AfxGetApp()->LoadIcon(IDI_MOTOR) , FALSE);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_GRABBER);
	
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	m_wndBusListCtrl.SetImageList(&(pApp->m_ELoadImages), LVSIL_SMALL);
	m_wndLoadListCtrl.SetImageList(&(pApp->m_ELoadImages), LVSIL_SMALL);
	m_wndTransformerListCtrl.SetImageList(&(pApp->m_ELoadImages), LVSIL_SMALL);

	//! BUTTON
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	m_wndOKButton.SetTextHotColor(TextHotColor);
	m_wndCancelButton.SetTextHotColor(TextHotColor);
	m_wndMoveToOriginalBus.SetTextHotColor(TextHotColor);

	HICON hOKIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_OK_18X18),
		IMAGE_ICON,
		18,18, 0 );
	m_wndOKButton.SetIcon(hOKIcon);
	
	HICON hCancelIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_CANCEL_18X18),
		IMAGE_ICON,
		18,18, 0 );
	m_wndCancelButton.SetIcon(hCancelIcon);
	
	DisplayBusItem();
///	m_wndLoadListCtrl.SetSortColumn(0);
	DisplayTransformerItem();
	
	//! load의 bus id 속성을 미리 저장해 둔다.
	list<CELoadItem*> LoadItemList;
	docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
	//if(pItemList)
	{
		for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
		{
			const string rBusID = (*itr)->prop()->GetValue(_T("Panel") , _T("Bus ID"));
			m_LoadBusIDMap.insert(make_pair(*itr , rBusID));
		}
	}

	m_nCurBusSel = -1;

	INIT_EASYSIZE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	

	@author BHK	

	@date 2009-04-06 오후 7:09:28	

	@param	

	@return		
*/
BOOL CVirtualLoadSummaryDlg::OnEraseBkgnd(CDC* pDC)
{
	BOOL res = CDialog::OnEraseBkgnd(pDC);

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

	@date 2009-04-07 오후 7:10:32	

	@param	

	@return		
*/
void CVirtualLoadSummaryDlg::OnVertResize() 
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

	@date 2009-04-06 오후 7:12:29	

	@param	

	@return		
*/
void CVirtualLoadSummaryDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	UPDATE_EASYSIZE;

	if(m_wndResize.GetSafeHwnd())
	{
		CRect rect1,rect2;
		m_wndBusListCtrl.GetWindowRect(&rect1);
		ScreenToClient(&rect1);
		m_wndTransformerListCtrl.GetWindowRect(&rect2);
		ScreenToClient(&rect2);

		m_wndResize.SetWindowPos( NULL, rect1.right + 3 , rect1.top , 3 , rect2.bottom - rect1.top , SWP_NOZORDER | SWP_NOACTIVATE );
	}

	CX = cx;
	CY = cy;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-06 오후 8:06:06	

	@param	

	@return		
*/
void CVirtualLoadSummaryDlg::OnNMClickListBus(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	if(-1 != pNMItemActivate->iItem)
	{
		m_nCurBusSel = pNMItemActivate->iItem;

		CBusItem* pBusItem = (CBusItem*)(m_wndBusListCtrl.GetItemData(pNMItemActivate->iItem));
		if(pBusItem) DisplayLoadInBus(pBusItem->GetName());
	}

	*pResult = 0;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-06 오후 8:10:42	

	@param	

	@return		
*/
int CVirtualLoadSummaryDlg::DisplayLoadInBus(const string& rBusId)
{
	CItemSelectionSet SelectionSet;
	CELoadDocData& docData = CELoadDocData::GetInstance();
	docData.GetLoadItemSetInBus(SelectionSet , rBusId);

	m_wndLoadListCtrl.SetRedraw(FALSE);
	m_wndLoadListCtrl.DeleteAllItems();
	m_wndLoadListCtrl.m_ItemColorMap.clear();
	for(int i = 0;i < SelectionSet.GetItemCount();++i)
	{
		CLoadItem* pLoadItem = static_cast<CLoadItem*>(SelectionSet.GetItemAt(i));
		if(pLoadItem)
		{
			CELoadItemProp *pItemProp = pLoadItem->prop();
			int nImage = docData.GetIconNo(pLoadItem);
			const int nItem = m_wndLoadListCtrl.InsertItem(i , pLoadItem->GetName().c_str() , nImage);
			if(-1 != nItem)
			{
				m_wndLoadListCtrl.SetItemData(nItem , DWORD_PTR(pLoadItem));

				CBusItem* pBusItem = NULL;
				string rLoadCapacity;
				const string rConnectedBusID = pItemProp->GetValue(_T("Panel") , _T("Connect Bus"));
				const string rLoadCategory   = pItemProp->GetValue(_T("Type") , _T("Load Category"));
				if((SUBBUS == rLoadCategory) && !rConnectedBusID.empty() && (pBusItem = docData.FindBusItemByName(rConnectedBusID)))
				{
					double nFactors[3] = {0.f , 0.f , 0.f} , nKW[3] = {0.f , 0.f , 0.f} , nKVAR[3] = {0.f , 0.f , 0.f};
					pBusItem->GetLoadSummaryCalcData(nFactors , nKW , nKVAR);
					const double nContinuous  = atof(pBusItem->prop()->GetValue(_T("General") , _T("Continuous")).c_str());
					const double nIntermittent=atof(pBusItem->prop()->GetValue(_T("General") , _T("Intermittent")).c_str());
					const double nStandby = nContinuous*0.01*atof(pBusItem->prop()->GetValue(_T("General") , _T("Stand-by")).c_str());
					const double nLoadCapacity = sqrt(nKW[0]*nKW[0] + nKVAR[0]*nKVAR[0]) + 
						nIntermittent*0.01*sqrt(nKW[1]*nKW[1] + nKVAR[1]*nKVAR[1]) + 
						nStandby*0.01*sqrt(nKW[2]*nKW[2] + nKVAR[2]*nKVAR[2]);

					ostringstream oss;
					oss << SAFE_ROUND(nLoadCapacity,2);
					rLoadCapacity = oss.str();
				}
				else
				{
					rLoadCapacity = pItemProp->GetValue("Load Summary", "KVA");
				}
				m_wndLoadListCtrl.SetItemText(nItem , 1 , rLoadCapacity.c_str());
				
				map<CELoadItem*,string>::iterator where = m_LoadBusIDMap.find(pLoadItem);
				if(where != m_LoadBusIDMap.end())
				{
					const string rLoadID = pLoadItem->GetName();
					const string rBusID = pLoadItem->prop()->GetValue(_T("Panel") , _T("Bus ID"));

					m_wndLoadListCtrl.SetItemText(nItem , 2 , where->second.c_str());

					if(rBusID != where->second)
					{
						m_wndLoadListCtrl.m_ItemColorMap[DWORD_PTR(pLoadItem)].push_back(RGB(255,0,0));	//! #0
						m_wndLoadListCtrl.m_ItemColorMap[DWORD_PTR(pLoadItem)].push_back(RGB(255,0,0));	//! #1
						m_wndLoadListCtrl.m_ItemColorMap[DWORD_PTR(pLoadItem)].push_back(RGB(255,0,0));
					}
					else
					{
						m_wndLoadListCtrl.m_ItemColorMap[DWORD_PTR(pLoadItem)].push_back(RGB(0,0,0));	//! #0
						m_wndLoadListCtrl.m_ItemColorMap[DWORD_PTR(pLoadItem)].push_back(RGB(0,0,0));	//! #1
						m_wndLoadListCtrl.m_ItemColorMap[DWORD_PTR(pLoadItem)].push_back(RGB(0,0,0));
					}
				}
			}
		}
	}
	m_wndLoadListCtrl.SetRedraw(TRUE);
	m_wndLoadListCtrl.RedrawItems(0 , m_wndLoadListCtrl.GetItemCount());
	
	return ERROR_SUCCESS;
}

/**
	@brief	drag 시작.

	@author BHK	

	@date 2009-04-07 오전 9:22:17	

	@param	

	@return		
*/
void CVirtualLoadSummaryDlg::OnLvnBegindragListLoad(NMHDR *pNMHDR, LRESULT *pResult)
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
		m_pDropWnd =  &m_wndBusListCtrl;	//at present the drag list is the drop list

		//// Capture all mouse messages
		SetCapture ();
	}

	*pResult = 0;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-07 오전 9:26:36	

	@param	

	@return		
*/
void CVirtualLoadSummaryDlg::OnMouseMove(UINT nFlags, CPoint point)
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
		if (pDropWnd != m_pDropWnd)
		{
			if (m_nDropIndex != -1) //If we drag over the CListCtrl header, turn off the hover highlight
			{
				TRACE("m_nDropIndex is -1\n");
				CMFCListCtrl* pList = (CMFCListCtrl*)m_pDropWnd;
				//VERIFY (pList->SetItemState (m_nDropIndex, 0, LVIS_DROPHILITED));
				// redraw item
				//VERIFY (pList->RedrawItems (m_nDropIndex, m_nDropIndex));
				//pList->UpdateWindow ();
				m_nDropIndex = -1;
			}
			else //If we drag out of the CListCtrl altogether
			{
				TRACE("m_nDropIndex is not -1\n");
				//CListCtrl* pList = (CListCtrl*)m_pDropWnd;
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
		m_pDropWnd = pDropWnd;

		// Convert from screen coordinates to drop target client coordinates
		pDropWnd->ScreenToClient(&pt);

		//If we are hovering over a CListCtrl we need to adjust the highlights
		if(pDropWnd->IsKindOf(RUNTIME_CLASS (CMFCListCtrl)) && (pDropWnd->GetSafeHwnd() == m_wndBusListCtrl.GetSafeHwnd()))
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

	@author BHK	

	@date 2009-04-07 오전 9:27:34	

	@param	

	@return		
*/
void CVirtualLoadSummaryDlg::OnLButtonUp(UINT nFlags, CPoint point)
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
			m_pDropList = (CMFCListCtrl*)pDropWnd; //Set pointer to the list we are dropping on
			DropItemOnList(m_pDragList, m_pDropList); //Call routine to perform the actual drop
		}
	}

	CDialog::OnLButtonUp(nFlags, point);
}

/**
	@brief	target window에 drop했을 때 호출된다.

	@author BHK	

	@date 2009-04-07 오전 9:28:47	

	@param	

	@return		
*/
void CVirtualLoadSummaryDlg::DropItemOnList(CListCtrl *pDragList, CListCtrl *pDropList)
{
	vector<long> LoadItemIndexEntry;
	GetSelectedLineDataRecord(LoadItemIndexEntry);

	if(pDropList->GetSafeHwnd() == m_wndBusListCtrl.GetSafeHwnd())
	{
		UINT uFlags;
		///if(-1 != m_wndBusListCtrl.HitTest(point , &uFlags))
		{
			// Turn off hilight for previous drop target
			m_wndBusListCtrl.SetItemState (m_nDropIndex, 0, LVIS_DROPHILITED);
			//! Select current item
			m_wndBusListCtrl.SetItemState(m_nDropIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
			// Redraw previous item
			m_wndBusListCtrl.RedrawItems (m_nDropIndex, m_nDropIndex);

			CString rString;
			string rDropBusID;
			for( vector< long >::iterator itr = LoadItemIndexEntry.begin();itr != LoadItemIndexEntry.end();++itr)
			{
				CLoadItem* pLoadItem = (CLoadItem*)m_wndLoadListCtrl.GetItemData(*itr);
				if(pLoadItem)
				{
					const string rBusID = m_wndBusListCtrl.GetItemText(m_nDropIndex , 0).operator LPCSTR();
					pLoadItem->prop()->SetValue(_T("Panel") , _T("Bus ID") , rBusID);
					rDropBusID = rBusID;
				}
			}
			DisplayBusItem(true);
			DisplayTransformerItem(true);

			{
				for( vector< long >::reverse_iterator itr = LoadItemIndexEntry.rbegin();itr != LoadItemIndexEntry.rend();++itr)
				{
					m_wndLoadListCtrl.DeleteItem(*itr);
				}
				DisplayLoadInBus(rDropBusID);
			}

			m_wndBusListCtrl.RedrawWindow();
			m_wndLoadListCtrl.RedrawWindow();
			m_wndTransformerListCtrl.RedrawWindow();
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

	@author BHK	

	@date 2009-04-07 오전 9:30:02	

	@param	

	@return		
*/
void CVirtualLoadSummaryDlg::GetSelectedLineDataRecord(vector<long>& LoadItemIndexEntry)
{
	POSITION pos = m_wndLoadListCtrl.GetFirstSelectedItemPosition();
	while(pos)
	{
		const int nItem  = m_wndLoadListCtrl.GetNextSelectedItem(pos);		
		LoadItemIndexEntry.push_back( nItem );
	}
}

/**
	@brief	BUS ITEM들을 표시한다.(INCOMING BUS GROUP에 포함된 BUS도 표기한다.)

	@author HumKyung.BAEK	

	@date 2009-04-07 오전 11:26:09	

	@param	

	@return		
*/
int CVirtualLoadSummaryDlg::DisplayBusItem(const bool& bUpdate)
{
	m_wndBusListCtrl.SetRedraw(FALSE);
	if(false == bUpdate) m_wndBusListCtrl.DeleteAllItems();
	
	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> BusGroupItemList;
	docData.GetELoadItemListOf(BusGroupItemList , CBusGroupItem::TypeString());
	int i = 0;
	for(list<CELoadItem*>::iterator itr = BusGroupItemList.begin();itr != BusGroupItemList.end();++itr)
	{
		CBusGroupItem* pBusGroupItem = static_cast<CBusGroupItem*>(*itr);
		if(pBusGroupItem->IsDeleted()) continue;

		for(vector<CBusItem*>::iterator jtr = pBusGroupItem->m_BusRefEntry.begin();jtr != pBusGroupItem->m_BusRefEntry.end();++jtr)
		{
			if((*jtr)->IsDeleted()) continue;

			const string rBusID = (*jtr)->GetName();
			double nKW = 0.f , nKVAR = 0.f;
			(*jtr)->GetLoadSummaryResultData(nKW , nKVAR);
			double nKVA = sqrt(nKW*nKW + nKVAR*nKVAR);
			nKVA = SAFE_ROUND(nKVA , 2);

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

					m_wndBusListCtrl.m_ItemColorMap[DWORD_PTR(*jtr)].push_back(RGB(0,0,0));
					m_wndBusListCtrl.m_ItemColorMap[DWORD_PTR(*jtr)].push_back(RGB(0,0,0));
					m_wndBusListCtrl.m_ItemColorMap[DWORD_PTR(*jtr)].push_back(RGB(0,0,0));
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
					if(m_wndBusListCtrl.GetItemText(nItem , 1) != m_wndBusListCtrl.GetItemText(nItem , 2))
					{
						m_wndBusListCtrl.m_ItemColorMap[m_wndBusListCtrl.GetItemData(nItem)][0] = RGB(255,0,0);
						m_wndBusListCtrl.m_ItemColorMap[m_wndBusListCtrl.GetItemData(nItem)][1] = RGB(255,0,0);
						m_wndBusListCtrl.m_ItemColorMap[m_wndBusListCtrl.GetItemData(nItem)][2] = RGB(255,0,0);
					}
					else
					{
						m_wndBusListCtrl.m_ItemColorMap[m_wndBusListCtrl.GetItemData(nItem)][0] = RGB(0,0,0);
						m_wndBusListCtrl.m_ItemColorMap[m_wndBusListCtrl.GetItemData(nItem)][1] = RGB(0,0,0);
						m_wndBusListCtrl.m_ItemColorMap[m_wndBusListCtrl.GetItemData(nItem)][2] = RGB(0,0,0);
					}
					
				}
			}
		}
	}
	m_wndBusListCtrl.SetRedraw(TRUE);

	return ERROR_SUCCESS;
}

/**
	@brief	display transformer informations.

	@author BHK	

	@date 2009-04-07 오후 2:27:53	

	@param	

	@return		
*/
int CVirtualLoadSummaryDlg::DisplayTransformerItem(const bool& bUpdate)
{
	m_wndBusListCtrl.SetRedraw(FALSE);
	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> TrItemList;
	docData.GetELoadItemListOf(TrItemList , CTransformerItem::TypeString());
	int i = 0;
	for(list<CELoadItem*>::iterator itr = TrItemList.begin();itr != TrItemList.end();++itr,++i)
	{
		CTransformerItem* pTrItem = (CTransformerItem*)(*itr);
		const string rTrID = pTrItem->GetName();
		double nCapacity = pTrItem->CalculateCapacity();
		
		nCapacity = SAFE_ROUND(nCapacity , 0);	//! remove under zero.
		
		ostringstream oss;
		oss << nCapacity;

		if(false == bUpdate)
		{
			const int nItem = m_wndTransformerListCtrl.InsertItem(i,rTrID.c_str(), TRANSFORMER_ICON);
			if(-1 != nItem)
			{

				m_wndTransformerListCtrl.SetItemText(nItem , 1 , oss.str().c_str());
				m_wndTransformerListCtrl.SetItemText(nItem , 2 , oss.str().c_str());

				m_wndTransformerListCtrl.m_ItemColorMap[DWORD_PTR(*itr)].push_back(RGB(0,0,0));
				m_wndTransformerListCtrl.m_ItemColorMap[DWORD_PTR(*itr)].push_back(RGB(0,0,0));
				m_wndTransformerListCtrl.m_ItemColorMap[DWORD_PTR(*itr)].push_back(RGB(0,0,0));

				m_wndTransformerListCtrl.SetItemData(nItem , DWORD_PTR(*itr));
			}
		}
		else
		{
			LVFINDINFO info;
			info.flags = LVFI_PARTIAL|LVFI_STRING;
			info.psz = rTrID.c_str();

			const int nItem = m_wndTransformerListCtrl.FindItem(&info);
			if(-1 != nItem)
			{
				m_wndTransformerListCtrl.SetItemText(nItem , 2 , oss.str().c_str());
				if(m_wndTransformerListCtrl.GetItemText(nItem , 1) != m_wndTransformerListCtrl.GetItemText(nItem , 2))
				{
					m_wndTransformerListCtrl.m_ItemColorMap[m_wndTransformerListCtrl.GetItemData(nItem)][0] = RGB(255,0,0);
					m_wndTransformerListCtrl.m_ItemColorMap[m_wndTransformerListCtrl.GetItemData(nItem)][1] = RGB(255,0,0);
					m_wndTransformerListCtrl.m_ItemColorMap[m_wndTransformerListCtrl.GetItemData(nItem)][2] = RGB(255,0,0);
				}
				else
				{
					m_wndTransformerListCtrl.m_ItemColorMap[m_wndTransformerListCtrl.GetItemData(nItem)][0] = RGB(0,0,0);
					m_wndTransformerListCtrl.m_ItemColorMap[m_wndTransformerListCtrl.GetItemData(nItem)][1] = RGB(0,0,0);
					m_wndTransformerListCtrl.m_ItemColorMap[m_wndTransformerListCtrl.GetItemData(nItem)][2] = RGB(0,0,0);
				}
				
			}
		}
	}
	m_wndBusListCtrl.SetRedraw(TRUE);

	return ERROR_SUCCESS;
}

/**
	@brief	MOVE TO ORIGINAL BUS BUTTON CLICK

	@author KHS	

	@return		
**/
void CVirtualLoadSummaryDlg::OnBnClickedButtonMoveToOriginalBus()
{
	BeginWaitCursor();
	MoveToOriginalBus();

	m_wndBusListCtrl.RedrawWindow();
	m_wndLoadListCtrl.RedrawWindow();
	m_wndTransformerListCtrl.RedrawWindow();
	EndWaitCursor();
}

/**
	@brief	MOVE TO ORIGINAL BUS

	@author KHS	

	@return		
**/
int CVirtualLoadSummaryDlg::MoveToOriginalBus(void)
{
	//! 원래의 BUS ID로 되돌린다.
	for(map<CELoadItem*,string>::iterator itr = m_LoadBusIDMap.begin();itr != m_LoadBusIDMap.end();++itr)
	{
		itr->first->prop()->SetValue(_T("Panel") , _T("Bus ID") , itr->second);
	}

	DisplayBusItem(true);
	
	string rBusID;
	POSITION pos = m_wndBusListCtrl.GetFirstSelectedItemPosition();
	while(pos)
	{
		const int nItem = m_wndBusListCtrl.GetNextSelectedItem(pos);
		if(-1 != nItem) rBusID = m_wndBusListCtrl.GetItemText(nItem , 0).operator LPCSTR();
	}
	DisplayLoadInBus(rBusID);
	DisplayTransformerItem(true);

	return ERROR_SUCCESS;
}

void CVirtualLoadSummaryDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if(IDOK == ::MessageBox(NULL, _T("변경한 Load Summary를 적용하시겠습니까?"), _T("Warning") , MB_OKCANCEL| MB_ICONWARNING))
	{
		OnOK();
		{
		}
	}
}

void CVirtualLoadSummaryDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	MoveToOriginalBus();
	OnCancel();
}

/**
	@brief	

	@author BHK	

	@date 2009-04-07 오후 7:08:08	

	@param	

	@return		
*/
int CVirtualLoadSummaryDlg::DetermineLayout(int cx, int cy)
{
	CRect rect1 , rect2;
	m_wndResize.GetWindowRect(&rect1);
	ScreenToClient(&rect1);

	m_wndBusListCtrl.GetWindowRect(&rect2);
	ScreenToClient(&rect2);
	cx = rect1.left - rect2.left;
	m_wndBusListCtrl.SetWindowPos(NULL , rect2.left , rect2.top , cx - 3 ,rect2.Height(),SWP_NOZORDER | SWP_NOMOVE);
	m_wndBusListCtrl.RedrawWindow();

	m_wndTransformerListCtrl.GetWindowRect(&rect2);
	ScreenToClient(&rect2);
	cx = rect1.left - rect2.left;
	m_wndTransformerListCtrl.SetWindowPos(NULL , rect2.left , rect2.top , cx - 3 ,rect2.Height(),SWP_NOZORDER | SWP_NOMOVE);
	m_wndTransformerListCtrl.RedrawWindow();

	m_wndLoadListCtrl.GetWindowRect(&rect2);
	ScreenToClient(&rect2);
	cx = rect2.right - rect1.right;
	m_wndLoadListCtrl.SetWindowPos(NULL , rect1.right + 3 , rect2.top , cx - 3 ,rect2.Height(),SWP_NOZORDER);
	m_wndLoadListCtrl.RedrawWindow();

	return ERROR_SUCCESS;
}
