// AssignSchematicTypeToLoadDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELoad.h"
#include "MainFrm.h"
#include "AssignSchematicTypeToLoadDlg.h"
#include "ELoadDocData.h"

// CAssignSchematicTypeToLoadDlg dialog
struct MatchSchemType : public binary_function<CELOADTable::Record, string, bool>
{
        bool operator()(const CELOADTable::Record lhs , const string& rhs) const
	{
		const map<string,string>::const_iterator where = lhs.FieldValueMap.find(_T("C_TYPE"));
		if(where != lhs.FieldValueMap.end())
		{
			return (rhs == where->second);
		}

		return false;
	}
};

IMPLEMENT_DYNCREATE(CAssignSchematicTypeToLoadDlg, CWizardPropertyPage)

CAssignSchematicTypeToLoadDlg::CAssignSchematicTypeToLoadDlg(CWnd* pParent /*=NULL*/)
	: CWizardPropertyPage(CAssignSchematicTypeToLoadDlg::IDD/*, pParent*/ /*, m_wndResize(TRUE*/)
{

}

CAssignSchematicTypeToLoadDlg::~CAssignSchematicTypeToLoadDlg()
{
	m_bPdbIncludeCheck = FALSE;
}

BEGIN_EASYSIZE_MAP(CAssignSchematicTypeToLoadDlg)
	EASYSIZE(IDC_STATIC_ASSIGN_SCHEMATIC_TO_LOAD,ES_BORDER,ES_BORDER, ES_BORDER,ES_BORDER,0)

	EASYSIZE(IDC_LIST_SCHEMATIC_TYPE,ES_BORDER,ES_BORDER, IDC_LIST_LOAD,ES_BORDER,0)
	EASYSIZE(IDC_LIST_LOAD,IDC_LIST_SCHEMATIC_TYPE,ES_BORDER, ES_BORDER,ES_BORDER,0)
	
	EASYSIZE(IDC_STATIC_NOT_ASSIGNED,ES_BORDER,IDC_LIST_SCHEMATIC_TYPE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_LIST_NOT_ASSIGNED_LOAD,ES_BORDER,IDC_STATIC_NOT_ASSIGNED,IDC_LIST_LOAD,ES_BORDER,0)

	EASYSIZE(IDC_STATIC_MOVE_TO_ORIGINAL_SCHEMATIC_TYPE,ES_BORDER,ES_KEEPSIZE, ES_KEEPSIZE,ES_BORDER,0)
	EASYSIZE(IDC_BUTTON_MOVE_TO_ORIGINAL_SCHEMATIC_TYPE,ES_BORDER,ES_KEEPSIZE, ES_KEEPSIZE,ES_BORDER,0)

	EASYSIZE(IDOK,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDCANCEL,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP

void CAssignSchematicTypeToLoadDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX , IDC_LIST_SCHEMATIC_TYPE , m_wndSchematicTypeListCtrl);
	DDX_Control(pDX , IDC_LIST_LOAD , m_wndLoadListCtrl);
	DDX_Control(pDX , IDC_LIST_NOT_ASSIGNED_LOAD , m_wndNotAssignedLoadListCtrl);

	DDX_Control(pDX , IDC_COMBO_BUS , m_wndBusCombo);
	DDX_Control(pDX , IDC_BUTTON_MOVE_TO_ORIGINAL_SCHEMATIC_TYPE , m_wndMoveToOriginalSchematicType);

	DDX_Control(pDX, ID_WIZBACK, m_wndBackButton);
	DDX_Control(pDX, ID_WIZNEXT, m_wndNextButton);

	DDX_Control(pDX , IDOK , m_wndOKButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);
	DDX_Control(pDX , ID_SCHEMATIC_CLEAR_BUTTON, m_wndClearButton);

	DDX_Check(pDX , IDC_PDB_INCLUDE_CHECK , m_bPdbIncludeCheck);

	DDX_Control(pDX, IDC_TOTAL_ASSIGNED_LOAD_COUNT_EDIT, m_wndTotalLoadCountEdit);

	CDialog::DoDataExchange(pDX);
}

/**
	@brief	

	@author	BHK
*/
BOOL CAssignSchematicTypeToLoadDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	BeginWaitCursor();

	m_hIcon = AfxGetApp()->LoadIcon(IDI_GRABBER);
	
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	m_wndSchematicTypeListCtrl.SetImageList (&(pApp->m_ELoadImages) , LVSIL_SMALL);
	m_wndSchematicTypeListCtrl.Sort(0);

	m_wndSchematicTypeListCtrl.InsertColumn(0 , _T("Type")  , LVCFMT_LEFT , 100);
	m_wndSchematicTypeListCtrl.InsertColumn(1 , _T("Desc.") , LVCFMT_LEFT , 160);
	m_wndSchematicTypeListCtrl.InsertColumn(2 , _T("Load Count.") , LVCFMT_LEFT , 100);
	m_wndSchematicTypeListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

	m_wndLoadListCtrl.InsertColumn(0 , _T("Load ID.")  , LVCFMT_LEFT , 100);
	m_wndLoadListCtrl.InsertColumn(1 , _T("Rating") , LVCFMT_LEFT , 70);
	m_wndLoadListCtrl.InsertColumn(2 , _T("Existing SCHEM") , LVCFMT_LEFT , 120);
	m_wndLoadListCtrl.InsertColumn(3 , _T("Bus ID") , LVCFMT_LEFT , 100);
        m_wndLoadListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES);
	m_wndLoadListCtrl.SetImageList (&(pApp->m_ELoadImages), LVSIL_SMALL);

	//! combobox에 BUS NAME을 채운다.
	DisplayBusInComboBox();

	/*const int nItem = m_wndBusCombo.FindString(-1 , _T("*"));
	if(-1 != nItem) m_wndBusCombo.SetCurSel(nItem);*/

	m_wndNotAssignedLoadListCtrl.InsertColumn(0 , _T("Load ID.")  , LVCFMT_LEFT , 100);
	m_wndNotAssignedLoadListCtrl.InsertColumn(1 , _T("Rating") , LVCFMT_LEFT , 70);
	m_wndNotAssignedLoadListCtrl.InsertColumn(2 , _T("Existing SCHEM") , LVCFMT_LEFT , 120);
	m_wndNotAssignedLoadListCtrl.InsertColumn(3 , _T("Bus ID") , LVCFMT_LEFT , 100);
        m_wndNotAssignedLoadListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
	m_wndNotAssignedLoadListCtrl.SetImageList (&(pApp->m_ELoadImages), LVSIL_SMALL);
	m_wndNotAssignedLoadListCtrl.SetSortColumn(0);
	m_wndNotAssignedLoadListCtrl.Sort(0);

	CRect rect1 , rect2;
	m_wndSchematicTypeListCtrl.GetWindowRect(&rect1);
	ScreenToClient(&rect1);
	m_wndNotAssignedLoadListCtrl.GetWindowRect(&rect2);
	ScreenToClient(&rect2);

	/////////////////////////////////////////////
	//// Set up initial variables
	m_bDragging = FALSE;
	m_nDragIndex = -1;
	m_nDropIndex = -1;
	m_pDragImage = NULL;

	CELoadDocData& docData = CELoadDocData::GetInstance();
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	m_wndBackButton.SetTextHotColor(TextHotColor);
	m_wndNextButton.SetTextHotColor(TextHotColor);

	m_wndClearButton.SetTextHotColor(TextHotColor);

	m_wndBackButton.SetIcon(IDI_PRE);
	m_wndNextButton.SetIcon(IDI_NEXT);

        m_wndOKButton.SetIcon(IDI_OK_18X18);
	m_wndOKButton.SetTextHotColor(TextHotColor);
	m_wndCancelButton.SetIcon(IDI_CANCEL_18X18);
	m_wndCancelButton.SetTextHotColor(TextHotColor);
	
	LoadData();

	EndWaitCursor();

	INIT_EASYSIZE;

	return Init();  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CAssignSchematicTypeToLoadDlg, CDialog)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_LOAD, OnBegindragListRight)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDOK, &CAssignSchematicTypeToLoadDlg::OnBnClickedOk)
	ON_NOTIFY(NM_CLICK, IDC_LIST_SCHEMATIC_TYPE, &CAssignSchematicTypeToLoadDlg::OnNMClickListSchematicType)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_NOT_ASSIGNED_LOAD, &CAssignSchematicTypeToLoadDlg::OnLvnBegindragListNotAssignedLoad)
	ON_COMMAND(ID_RESIZE, OnVertResize)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_TO_ORIGINAL_SCHEMATIC_TYPE, &CAssignSchematicTypeToLoadDlg::OnBnClickedButtonMoveToOriginalSchematicType)
	ON_BN_CLICKED(IDCANCEL, &CAssignSchematicTypeToLoadDlg::OnBnClickedCancel)
	ON_BN_CLICKED(ID_WIZBACK, &CAssignSchematicTypeToLoadDlg::OnBnClickedWizback)
	ON_BN_CLICKED(ID_WIZNEXT, &CAssignSchematicTypeToLoadDlg::OnBnClickedWiznext)
	ON_CBN_SELENDOK(IDC_COMBO_BUS, &CAssignSchematicTypeToLoadDlg::OnCbnSelendokComboBus)
	ON_BN_CLICKED(IDC_PDB_INCLUDE_CHECK, &CAssignSchematicTypeToLoadDlg::OnBnClickedPdbIncludeCheck)
	ON_BN_CLICKED(ID_SCHEMATIC_CLEAR_BUTTON, &CAssignSchematicTypeToLoadDlg::OnBnClickedSchematicClearButton)
END_MESSAGE_MAP()


/**
	@brief	

	@author	BHK
*/
void CAssignSchematicTypeToLoadDlg::OnMouseMove(UINT nFlags, CPoint point) 
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
			if(pDropWnd->IsKindOf(RUNTIME_CLASS(CMFCListCtrl)) && (pDropWnd->GetSafeHwnd() == m_wndLoadListCtrl.GetSafeHwnd()))
			{
				TRACE("m_nDropIndex is not 100\n");
				//Note that we can drop here
				SetCursor(LoadCursor(NULL, IDC_ARROW));
				CListCtrl* pList = (CListCtrl*)pDropWnd;
			}
			else if(pDropWnd->IsKindOf(RUNTIME_CLASS(CMFCListCtrl)) && (pDropWnd->GetSafeHwnd() == m_wndSchematicTypeListCtrl.GetSafeHwnd()))
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
			else if(pDropWnd->IsKindOf(RUNTIME_CLASS(CMFCListCtrl)) && (pDropWnd->GetSafeHwnd() == m_wndNotAssignedLoadListCtrl.GetSafeHwnd()))
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
	@brief	

	@author	BHK
*/
void CAssignSchematicTypeToLoadDlg::OnLButtonUp(UINT nFlags, CPoint point) 
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
		if ((m_pDragList != pDropWnd) && pDropWnd->IsKindOf (RUNTIME_CLASS (CMFCListCtrl)))
		{
			m_pDropList = (CMFCTextColorListCtrl*)pDropWnd; //Set pointer to the list we are dropping on
			DropItemOnList(m_pDragList, m_pDropList); //Call routine to perform the actual drop
		}
	}
	
	CDialog::OnLButtonUp(nFlags, point);
}

/**
	@brief	called when user drop item.

	@author	BHK
*/
void CAssignSchematicTypeToLoadDlg::DropItemOnList(CMFCTextColorListCtrl* pDragList , CMFCTextColorListCtrl* pDropList)
{
	vector<long> LoadItemIndexEntry;
	GetSelectedLoadItems(*pDragList , LoadItemIndexEntry);

	if(	(pDropList->GetSafeHwnd() == m_wndLoadListCtrl.GetSafeHwnd()) ||
		(pDropList->GetSafeHwnd() == m_wndSchematicTypeListCtrl.GetSafeHwnd()) ||
		(pDropList->GetSafeHwnd() == m_wndNotAssignedLoadListCtrl.GetSafeHwnd()))
	{
		{
			// Turn off hilight for previous drop target
			pDropList->SetItemState (m_nDropIndex, 0, LVIS_DROPHILITED);
			pDropList->SetItemState(m_nDropIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
			// Redraw previous item
			pDropList->RedrawItems (m_nDropIndex, m_nDropIndex);
			

			CString rString;
			string rDropSchematicType;
			//if(pDropList->GetSafeHwnd() == m_wndLoadListCtrl.GetSafeHwnd())
			{
				rDropSchematicType = GetSelectedSchematicTypeFromSchematicListCtrl();
				if(rDropSchematicType.empty()) return;
			}

			for( vector< long >::iterator itr = LoadItemIndexEntry.begin();itr != LoadItemIndexEntry.end();++itr)
			{
				CLoadItem* pLoadItem = (CLoadItem*)pDragList->GetItemData(*itr);
				if(pLoadItem)
				{
					if(pDropList->GetSafeHwnd() == m_wndLoadListCtrl.GetSafeHwnd())
					{
						pLoadItem->prop()->SetValue(_T("Control Cable") , _T("Schematic Ty") , rDropSchematicType);
					}
					else if(pDropList->GetSafeHwnd() == m_wndSchematicTypeListCtrl.GetSafeHwnd())
					{
						const string rSchematicType = pDropList->GetItemText(m_nDropIndex , 0).operator LPCSTR();
						pLoadItem->prop()->SetValue(_T("Control Cable") , _T("Schematic Ty") , rSchematicType);
						rDropSchematicType = rSchematicType;
					}
					else if(pDropList->GetSafeHwnd() == m_wndNotAssignedLoadListCtrl.GetSafeHwnd())
					{
						pLoadItem->prop()->SetValue(_T("Control Cable") , _T("Schematic Ty") , _T(""));
						//rDropSchematicType = _T("");
					}
				}
			}

			//! 드래그 한곳의 아이템을 삭제한다.
			/*for( vector< long >::reverse_iterator itr = LoadItemIndexEntry.rbegin();itr != LoadItemIndexEntry.rend();++itr)
			{
				pDragList->DeleteItem(*itr);
			}*/
			int nSelItem = -1;

			POSITION pos = m_wndSchematicTypeListCtrl.GetFirstSelectedItemPosition();
			if(pos)
			{
				nSelItem = m_wndSchematicTypeListCtrl.GetNextSelectedItem(pos);
			}

			DisplaySchematicType();
			
			m_wndSchematicTypeListCtrl.SetItemState(nSelItem, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);

			if(!rDropSchematicType.empty()) DisplayLoadHasSchematicType(m_wndLoadListCtrl , rDropSchematicType);
			
			CString rBusID = _T("*");
			const int nItem = m_wndBusCombo.GetCurSel();
			if(-1 != nItem) m_wndBusCombo.GetLBText(nItem , rBusID);
			DisplayLoadHasSchematicType(m_wndNotAssignedLoadListCtrl , _T("") , rBusID.operator LPCSTR());
			
			m_bModified = true;
		}
	}
}

/**
	@brief	

	@author	BHK
*/
void CAssignSchematicTypeToLoadDlg::OnBegindragListRight(NMHDR* pNMHDR, LRESULT* pResult) 
{
	//This routine sets the parameters for a Drag and Drop operation.
	//It sets some variables to track the Drag/Drop as well
	// as creating the drag image to be shown during the drag.
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if(NULL == pNMLV) return;

	//// Save the index of the item being dragged in m_nDragIndex
	////  This will be used later for retrieving the info dragged
	m_nDragIndex = pNMLV->iItem;

	OnBeginDrag(pNMLV , m_wndLoadListCtrl);
	/*
	//// Create a drag image
	POINT pt;
	int nOffset = -10; //offset in pixels for drag image (positive is up and to the left; neg is down and to the right)
	if((pNMLV->hdr.hwndFrom == m_wndLoadListCtrl.GetSafeHwnd()) && (m_wndLoadListCtrl.GetSelectedCount() > 0)) //! more than one item is selected
	{
		pt.x = nOffset;
		pt.y = nOffset;

		m_pDragImage = m_wndSchematicTypeListCtrl.CreateDragImage(m_nDragIndex, &pt);
		ASSERT(m_pDragImage); //make sure it was created
		//We will call delete later (in LButtonUp) to clean this up

		CBitmap bitmap;
		if(m_wndSchematicTypeListCtrl.GetSelectedCount() > 1) //more than 1 item in list is selected
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
		m_pDragList = &m_wndSchematicTypeListCtrl; //make note of which list we are dragging from
		m_pDropList =  &m_wndLoadListCtrl;	//at present the drag list is the drop list

		//// Capture all mouse messages
		SetCapture ();
	}
	*/

	*pResult = 0;
}

/**
	@brief	

	@author	BHK
*/
int CAssignSchematicTypeToLoadDlg::AddLoadItemToListCtrl(CLoadItem* pLoadItem)
{
	assert(pLoadItem && "pLoadItem is NULL");

	if(pLoadItem)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();

		pLoadItem->SetBusId("");
		
		const int nImage = docData.GetIconNo(pLoadItem);
		const int nItem = m_wndSchematicTypeListCtrl.InsertItem(0 , pLoadItem->GetName().c_str() , nImage);

		m_wndSchematicTypeListCtrl.SetItemData(nItem , (DWORD_PTR)pLoadItem);
		m_wndSchematicTypeListCtrl.RedrawWindow();

		m_wndSchematicTypeListCtrl.Sort(0 , m_wndSchematicTypeListCtrl.GetHeaderCtrl().IsAscending());
		
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
void CAssignSchematicTypeToLoadDlg::OnBnClickedOk()
{
	m_bModified = false;

	CELoadDocData& docData = CELoadDocData::GetInstance();
	docData.Save(CLoadItem::TableName() , CLoadItem::TypeString() , AfxGetMainWnd());
	
	m_LoadSchematicTypeMap.clear();

	//! load의 Schematic Type 속성을 미리 저장해 둔다.
	list<CELoadItem*> LoadItemList;
	docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
	{
		for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
		{
			const string rSchematicType = (*itr)->prop()->GetValue(_T("Control Cable") , _T("Schematic Ty"));
			m_LoadSchematicTypeMap.insert(make_pair(*itr , rSchematicType));
		}
	}
	
	Init();

	SaveData();
}

/**
	@brief	

	@author BHK	

	@date 2009-04-11 오전 9:04:34	

	@param	

	@return		
**/
int CAssignSchematicTypeToLoadDlg::DisplaySchematicType()
{
	m_wndLoadListCtrl.SetRedraw(FALSE);
	m_wndLoadListCtrl.DeleteAllItems();
	
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_TYPE"));
	if(pTable)
	{
		m_wndSchematicTypeListCtrl.DeleteAllItems();

		int i = 0;
		int nTotalLoadCount = 0;
		for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin();itr != pTable->m_RecordEntry.end();++itr)
		{
			const string rSchematicType = itr->FieldValueMap[_T("C_TYPE")];
			const string rDesc = itr->FieldValueMap[_T("C_DESC")];

			const int nItem = m_wndSchematicTypeListCtrl.InsertItem(i++ , rSchematicType.c_str());
			if(-1 != nItem) m_wndSchematicTypeListCtrl.SetItemText(nItem , 1 , rDesc.c_str());

			list<CELoadItem*> LoadItemList;
			docData.GetELoadItemListOf(LoadItemList, CLoadItem::TypeString());
			int nItemCount = 0;
			for(list<CELoadItem*>::iterator itr = LoadItemList.begin(); itr != LoadItemList.end();++itr)
			{
				CLoadItem *pLoadItem = static_cast<CLoadItem*>(*itr);
				if(pLoadItem)
				{
					const string rLoadSchematicType = pLoadItem->prop()->GetValue(_T("Control Cable") , _T("Schematic Ty"));
					if(rSchematicType == rLoadSchematicType)
					{
						nItemCount++;
					}
				}
			}
			CString rItemCount;
			rItemCount.Format("%d", nItemCount);

			nTotalLoadCount += nItemCount;
			m_wndSchematicTypeListCtrl.SetItemText(nItem, 2, rItemCount);

		}
		CString rTotalLoadCount;
		rTotalLoadCount.Format("%d", nTotalLoadCount);
		m_wndTotalLoadCountEdit.SetWindowText(rTotalLoadCount);
	}
	m_wndLoadListCtrl.SetRedraw(TRUE);

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-11 오전 9:06:00	

	@param	

	@return		
**/
int CAssignSchematicTypeToLoadDlg::DisplayLoadHasSchematicType(CMFCTextColorListCtrl& ListCtrl , const string& rSchematicType , const string& rInterestingBusID)
{
	UpdateData(TRUE);

	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> LoadItemList;
	docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
	CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_TYPE"));
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
				const string rBusID = pItemProp->GetValue(_T("Panel") , _T("Bus ID"));
				const string rValue = pItemProp->GetValue(_T("Control Cable") , _T("Schematic Ty"));
				if((_T("*") != rInterestingBusID) && (rBusID != rInterestingBusID)) continue;

				//! NOT Assigned LOAD List
				if(pTable && rSchematicType.empty() && !rValue.empty())
				{
					vector<CELOADTable::Record>::iterator where = find_if(pTable->m_RecordEntry.begin(), pTable->m_RecordEntry.end(),bind2nd(MatchSchemType(), rValue));
					if(where != pTable->m_RecordEntry.end())
					{
						continue;
					}
				}
				else
				{
					//! LOAD LIST
					if(rValue != rSchematicType) continue;
				}
				
				CBusItem* pBusItem = docData.FindBusItemByName(rBusID);
				if(pBusItem)
				{
					if(!m_bPdbIncludeCheck)
					{
						if(SUBBUS == pBusItem->prop()->GetValue(_T("General"), _T("Bus Group"))) continue;
					}
				}

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
					
					map<CELoadItem*,string>::iterator where = m_LoadSchematicTypeMap.find(pLoadItem);
					if(where != m_LoadSchematicTypeMap.end())
					{
						ListCtrl.SetItemText(nItem , 2 , where->second.c_str());

						if(rSchematicType != where->second)
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

					ListCtrl.SetItemText(nItem , 3 , rBusID.c_str());
				}
			}
		}
		ListCtrl.SetRedraw(TRUE);
		ListCtrl.SetSortColumn(0);
		ListCtrl.RedrawItems(0 , ListCtrl.GetItemCount());
	}
	
	return ERROR_SUCCESS;
}


/**
	@brief	Schematic Type을 가진 Load를 표기한다.

	@author BHK	

	@date 2009-04-11 오전 9:07:41	

	@param	

	@return		
**/
void CAssignSchematicTypeToLoadDlg::OnNMClickListSchematicType(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	if(-1 != pNMItemActivate->iItem)
	{
		const string rSchematicType = m_wndSchematicTypeListCtrl.GetItemText(pNMItemActivate->iItem , 0).operator LPCSTR();
		if(!rSchematicType.empty()) DisplayLoadHasSchematicType(m_wndLoadListCtrl , rSchematicType);
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
void CAssignSchematicTypeToLoadDlg::GetSelectedLoadItems(CMFCListCtrl& ListCtrl , vector<long>& LoadItemIndexEntry)
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
BOOL CAssignSchematicTypeToLoadDlg::OnEraseBkgnd(CDC* pDC)
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
void CAssignSchematicTypeToLoadDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	UPDATE_EASYSIZE;

	/*if(m_wndResize.GetSafeHwnd())
	{
		CRect rect1,rect2;
		m_wndLoadListCtrl.GetWindowRect(&rect1);
		ScreenToClient(&rect1);
		m_wndNotAssignedLoadListCtrl.GetWindowRect(&rect2);
		ScreenToClient(&rect2);

		m_wndResize.SetWindowPos( NULL, rect1.right + 3 , rect1.top , 3 , rect2.bottom - rect1.top , SWP_NOZORDER | SWP_NOACTIVATE );
	}*/
}

/**
	@brief	

	@author BHK	

	@date 2009-04-11 오후 3:00:00	

	@param	

	@return		
**/
void CAssignSchematicTypeToLoadDlg::OnLvnBegindragListNotAssignedLoad(NMHDR *pNMHDR, LRESULT *pResult)
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
int CAssignSchematicTypeToLoadDlg::OnBeginDrag(LPNMLISTVIEW pNMLV , CMFCTextColorListCtrl& ListCtrl)
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
void CAssignSchematicTypeToLoadDlg::OnVertResize() 
{
	/*CRect rect;
	m_wndResize.GetWindowRect( rect );
	ScreenToClient( rect );

	m_wndResize.SetWindowPos(NULL , rect.left + m_wndResize.m_adjust_width, rect.top , 3 , rect.Height() , SWP_NOZORDER | SWP_NOSIZE);*/

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
int CAssignSchematicTypeToLoadDlg::DetermineLayout(int cx, int cy)
{
	CRect rect1 , rect2;
	/*m_wndResize.GetWindowRect(&rect1);
	ScreenToClient(&rect1);*/

	m_wndSchematicTypeListCtrl.GetWindowRect(&rect2);
	ScreenToClient(&rect2);
	cx = rect1.left - rect2.left;
	m_wndSchematicTypeListCtrl.SetWindowPos(NULL , rect2.left , rect2.top , cx - 3 ,rect2.Height(),SWP_NOZORDER | SWP_NOMOVE);
	m_wndSchematicTypeListCtrl.RedrawWindow();

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
	@brief	load의 Schematic Type를 assign 이전의 값으로 되돌린다.

	@author BHK	

	@date 2009-04-13 오전 9:27:20	

	@param	

	@return		
*/
void CAssignSchematicTypeToLoadDlg::OnBnClickedButtonMoveToOriginalSchematicType()
{
	m_bModified = false;

	MoveToOriginalSchematicType();
}

/**
	@brief	Load의 Schematic Type을 편집하기 전의 값으로 되돌린다.

	@author BHK	

	@date 2009-04-13 오전 9:28:28	

	@param	

	@return		
*/
int CAssignSchematicTypeToLoadDlg::MoveToOriginalSchematicType(void)
{
	for(map<CELoadItem*,string>::iterator itr = m_LoadSchematicTypeMap.begin();itr != m_LoadSchematicTypeMap.end();++itr)
	{
		itr->first->prop()->SetValue(_T("Control Cable") , _T("Schematic Ty") , itr->second);
	}

	DisplaySchematicType();
	if(m_wndSchematicTypeListCtrl.GetItemCount() > 0)
	{
		const string rSchematicType = m_wndSchematicTypeListCtrl.GetItemText(0 , 0);
		m_wndSchematicTypeListCtrl.SetFocus();
		// Turn off hilight for previous drop target
		m_wndSchematicTypeListCtrl.SetItemState(0, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		// Redraw previous item
		m_wndSchematicTypeListCtrl.RedrawItems (0, 0);

		DisplayLoadHasSchematicType(m_wndLoadListCtrl , rSchematicType);
	}

	CString rBusID = _T("*");
	const int nItem = m_wndBusCombo.GetCurSel();
	if(-1 != nItem) m_wndBusCombo.GetLBText(nItem , rBusID);
	DisplayLoadHasSchematicType(m_wndNotAssignedLoadListCtrl , _T("") , rBusID.operator LPCSTR());

	return ERROR_SUCCESS;
}

/**
	@brief	assign한 내용을 무효화하고 다이얼로그를 종료한다.

	@author BHK	

	@date 2009-04-13 오전 9:42:26	

	@param	

	@return		
*/
void CAssignSchematicTypeToLoadDlg::OnBnClickedCancel()
{
	if(m_bModified)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		if(IDYES == AfxMessageBox(docData.GetStringOf(48).c_str() , MB_YESNO|MB_ICONWARNING ))
		{
			OnBnClickedOk();
		}
		else
		{
			MoveToOriginalSchematicType();

			GetParent()->SendMessage(WM_COMMAND, IDCANCEL);
		}
	}
	else
	{
		GetParent()->SendMessage(WM_COMMAND, IDCANCEL);
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-04-21 오후 2:14:20	

	@param	

	@return		
*/
string CAssignSchematicTypeToLoadDlg::GetSelectedSchematicTypeFromSchematicListCtrl(void)
{
	POSITION pos = m_wndSchematicTypeListCtrl.GetFirstSelectedItemPosition();
	while(pos)
	{
		const int nItem = m_wndSchematicTypeListCtrl.GetNextSelectedItem(pos);
		if(-1 != nItem)
		{
			return (m_wndSchematicTypeListCtrl.GetItemText(nItem , 0).operator LPCSTR());
		}
	}

	return string();
}

/**
	@brief	

	@author BHK	

	@date 2009-04-22 오후 3:19:17	

	@param	

	@return		
*/
void CAssignSchematicTypeToLoadDlg::OnBnClickedWizback()
{
	GetParent()->SendMessage(WM_COMMAND , ID_WIZBACK , 0);
}

/**
	@brief	

	@author BHK	

	@date 2009-04-22 오후 3:20:24	

	@param	

	@return		
*/
void CAssignSchematicTypeToLoadDlg::OnBnClickedWiznext()
{
	if(m_bModified)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		if(IDYES == AfxMessageBox(docData.GetStringOf(48).c_str() , MB_YESNO|MB_ICONWARNING ))
		{
			OnBnClickedOk();
			GetParent()->SendMessage(WM_COMMAND , ID_WIZNEXT, 0);
		}
		else
		{
			GetParent()->SendMessage(WM_COMMAND , ID_WIZNEXT, 0);
		}
		
	}
	else
	{
		GetParent()->SendMessage(WM_COMMAND , ID_WIZNEXT, 0);
	}
}

/**
	@brief	다이얼로그 초기화

	@author KHS	

	@return		
**/
BOOL CAssignSchematicTypeToLoadDlg::Init(void)
{
	const int nItem = m_wndBusCombo.FindString(-1 , _T("*"));
	if(-1 != nItem) m_wndBusCombo.SetCurSel(nItem);

	DisplaySchematicType();
	if(m_wndSchematicTypeListCtrl.GetItemCount() > 0)
	{
		const string rSchematicType = m_wndSchematicTypeListCtrl.GetItemText(0 , 0);
		m_wndSchematicTypeListCtrl.SetFocus();
		// Turn off hilight for previous drop target
		m_wndSchematicTypeListCtrl.SetItemState(0, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		// Redraw previous item
		m_wndSchematicTypeListCtrl.RedrawItems (0, 0);

		DisplayLoadHasSchematicType(m_wndLoadListCtrl , rSchematicType);
	}
	DisplayLoadHasSchematicType(m_wndNotAssignedLoadListCtrl , _T("") , _T("*"));

	CELoadDocData& docData = CELoadDocData::GetInstance();
	//! load의 Schematic Type 속성을 미리 저장해 둔다.
	list<CELoadItem*> LoadItemList;
	docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
	{
		for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
		{
			const string rSchematicType = (*itr)->prop()->GetValue(_T("Control Cable") , _T("Schematic Ty"));
			m_LoadSchematicTypeMap.insert(make_pair(*itr , rSchematicType));
		}
	}

	return TRUE;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-28 오후 3:02:01	

	@param	

	@return		
*/
void CAssignSchematicTypeToLoadDlg::OnCbnSelendokComboBus()
{
	CString rBusID = _T("*");
	const int nItem = m_wndBusCombo.GetCurSel();
	if(-1 != nItem) m_wndBusCombo.GetLBText(nItem , rBusID);
	DisplayLoadHasSchematicType(m_wndNotAssignedLoadListCtrl , _T("") , rBusID.operator LPCSTR());
}

/**
	@brief	PDB INCLUDE CHECK

	@author KHS	

	@date 2009-06-29 오후 1:31:27	

	@param	

	@return		
**/
void CAssignSchematicTypeToLoadDlg::OnBnClickedPdbIncludeCheck()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	m_LoadSchematicTypeMap.clear();

	//! load의 Schematic Type 속성을 미리 저장해 둔다.
	list<CELoadItem*> LoadItemList;
	docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
	{
		for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
		{
			const string rSchematicType = (*itr)->prop()->GetValue(_T("Control Cable") , _T("Schematic Ty"));
			m_LoadSchematicTypeMap.insert(make_pair(*itr , rSchematicType));
		}
	}

	CString rBusID;
	const int nItem = m_wndBusCombo.GetCurSel();
	if(-1 != nItem) m_wndBusCombo.GetLBText(nItem, rBusID);
	DisplayBusInComboBox();

	const int nFound = m_wndBusCombo.FindString(-1 , rBusID);
	if(-1 == nFound) 
	{
		int nSelItem = m_wndBusCombo.FindString(-1, "*");
		if(-1 != nSelItem)
		{
			m_wndBusCombo.SetCurSel(nSelItem);
		}
		rBusID = _T("*");
	}
	else
	{
		m_wndBusCombo.SetCurSel(nFound);
	}

	DisplayLoadHasSchematicType(m_wndNotAssignedLoadListCtrl , _T("") , rBusID.operator LPCSTR());
}

/**
	@brief	Load에 Schematic Type 이 존재하지 않는 것들을 초기화

	@author KHS	

	@date 2009-06-29 오후 1:39:41	

	@param	

	@return		
**/
void CAssignSchematicTypeToLoadDlg::OnBnClickedSchematicClearButton()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> LoadItemList;
	docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
	CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_TYPE"));

	if(pTable)
	{
		for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
		{
			if((*itr)->IsDeleted()) continue;

			CLoadItem* pLoadItem = static_cast<CLoadItem*>(*itr);
			if(pLoadItem)
			{
				const string rSchematicType = pLoadItem->prop()->GetValue(_T("Control Cable") , _T("Schematic Ty"));
				if(!rSchematicType.empty())
				{
					vector<CELOADTable::Record>::iterator where = find_if(pTable->m_RecordEntry.begin(), pTable->m_RecordEntry.end(), bind2nd(MatchSchemType(), rSchematicType));
					if(where == pTable->m_RecordEntry.end())
					{
						pLoadItem->prop()->SetValue(_T("Control Cable"), _T("Schematic Ty"), _T(""));
						pLoadItem->prop()->SetOriginalValue(_T("Control Cable"), _T("Schematic Ty"), _T(""));

					}
					m_bModified = true;
				}
			}
		}
		m_LoadSchematicTypeMap.clear();

		//! load의 Schematic Type 속성을 미리 저장해 둔다.
		list<CELoadItem*> LoadItemList;
		docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
		{
			for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
			{
				const string rSchematicType = (*itr)->prop()->GetValue(_T("Control Cable") , _T("Schematic Ty"));
				m_LoadSchematicTypeMap.insert(make_pair(*itr , rSchematicType));
			}
		}

		CString rBusID;
		const int nItem = m_wndBusCombo.GetCurSel();
		if(-1 != nItem) m_wndBusCombo.GetLBText(nItem, rBusID);
		DisplayLoadHasSchematicType(m_wndNotAssignedLoadListCtrl , _T("") , rBusID.operator LPCSTR());
	}
}

/**
	@brief SAVE DATA

	@author KHS	

	@date 2009-06-29 오후 2:31:39	

	@param	

	@return		
**/
int CAssignSchematicTypeToLoadDlg::SaveData(void)
{
	UpdateData(TRUE);

	CELoadDocData& docData = CELoadDocData::GetInstance();
	
	CString rPdbInclude;
	rPdbInclude.Format("%d", m_bPdbIncludeCheck);

	CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
	pSettingTable->SetFieldValue(_T("C_PDB_INCLUDE"), rPdbInclude.operator LPCSTR());
	
	return ERROR_SUCCESS;
}

/**
	@brief	LOAD DATA

	@author KHS	

	@date 2009-06-29 오후 2:42:18	

	@param	

	@return		
**/
int CAssignSchematicTypeToLoadDlg::LoadData(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	
	CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
	string* pValue = pSettingTable->GetFieldValue(_T("C_PDB_INCLUDE"));
	if(pValue)
	{
		m_bPdbIncludeCheck = atoi(pValue->c_str());
	}

	UpdateData(FALSE);

	return ERROR_SUCCESS;
}

/**
	@brief	COMBO BOX에 BUS 이름을 채운다.

	@author BHK	

	@date 2009-06-30 오전 11:10:14	

	@param	

	@return		
*/
int CAssignSchematicTypeToLoadDlg::DisplayBusInComboBox(void)
{
	m_wndBusCombo.ResetContent();

	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> BusGroupList;
	docData.GetELoadItemListOf(BusGroupList , CBusGroupItem::TypeString());
	{
		CButton* pButton = (CButton*)(GetDlgItem(IDC_PDB_INCLUDE_CHECK));
		const bool bIncludePDB = (pButton && pButton->GetCheck()) ? true : false;

		m_wndBusCombo.AddString(_T("*"));
		for(list<CELoadItem*>::iterator itr = BusGroupList.begin(); itr != BusGroupList.end();++itr)
		{
			CBusGroupItem* pBusGroup = static_cast<CBusGroupItem*>(*itr);
			if(pBusGroup->IsDeleted()) continue;
			
			const string rID = pBusGroup->prop()->GetValue(_T("General") , _T("Bus Group ID"));
			if((false == bIncludePDB) && (SUBBUS == rID)) continue;

			for(vector<CBusItem*>::iterator jtr = pBusGroup->m_BusRefEntry.begin(); jtr != pBusGroup->m_BusRefEntry.end();++jtr)
			{
				if((*jtr)->IsDeleted()) continue;

				m_wndBusCombo.AddString((*jtr)->GetName().c_str());
			}
		}
	}

	return ERROR_SUCCESS;
}
