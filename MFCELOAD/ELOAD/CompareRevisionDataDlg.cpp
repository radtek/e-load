// ViewRevisionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "CompareRevisionDataDlg.h"
#include "ELoadDocData.h"
#include "Tables/ELOADTable.h"

// CCompareRevisionDataDlg dialog

IMPLEMENT_DYNAMIC(CCompareRevisionDataDlg, CDialog)

CCompareRevisionDataDlg::CCompareRevisionDataDlg(const CString& rProductName , CWnd* pParent /*=NULL*/)
	: CDialog(CCompareRevisionDataDlg::IDD, pParent) , m_rProductName(rProductName)
{
	m_nLeftSideRevNo = m_nRightSideRevNo = -1;

	m_bDragging = FALSE;
}

CCompareRevisionDataDlg::~CCompareRevisionDataDlg()
{
}

void CCompareRevisionDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_LIST_REVISION , m_wndRevisionListCtrl);
	DDX_Control(pDX , IDC_LIST_LEFT_PRODUCT , m_wndLeftProductListCtrl);
	DDX_Control(pDX , IDC_LIST_RIGHT_PRODUCT , m_wndRightProductListCtrl);

	DDX_Control(pDX , IDOK , m_wndOKButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);
}


BEGIN_MESSAGE_MAP(CCompareRevisionDataDlg, CDialog)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_REVISION, &CCompareRevisionDataDlg::OnLvnBegindragListRevision)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
//	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_LEFT_PRODUCT, &CCompareRevisionDataDlg::OnLvnItemchangedListLeftProduct)
//	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_RIGHT_PRODUCT, &CCompareRevisionDataDlg::OnLvnItemchangedListRightProduct)
	ON_NOTIFY(NM_CLICK, IDC_LIST_LEFT_PRODUCT, &CCompareRevisionDataDlg::OnNMClickListLeftProduct)
	ON_NOTIFY(NM_CLICK, IDC_LIST_RIGHT_PRODUCT, &CCompareRevisionDataDlg::OnNMClickListRightProduct)
	ON_BN_CLICKED(IDOK, &CCompareRevisionDataDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CCompareRevisionDataDlg message handlers
/**
	@brief	

	@author BHK	

	@date 2009-04-28 오전 10:51:03	

	@param	

	@return		
*/
int CCompareRevisionDataDlg::CreateRevisionListCtrl(void)
{
	m_wndRevisionListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES);
	m_wndRevisionListCtrl.InsertColumn(0 , _T("PRODUCT NAME") , LVCFMT_LEFT , 150);
	m_wndRevisionListCtrl.InsertColumn(1 , _T("REVISION NO") , LVCFMT_LEFT , 100);
	m_wndRevisionListCtrl.InsertColumn(2 , _T("DATE") , LVCFMT_LEFT , 100);
	m_wndRevisionListCtrl.InsertColumn(3 , _T("DESC") , LVCFMT_LEFT , 500);

	m_wndLeftProductListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES);
	m_wndLeftProductListCtrl.InsertColumn(0 , _T("PRODUCT NAME") , LVCFMT_LEFT , 150);
	m_wndLeftProductListCtrl.InsertColumn(1 , _T("REVISION NO") , LVCFMT_LEFT , 100);
	m_wndLeftProductListCtrl.InsertColumn(2 , _T("DATE") , LVCFMT_LEFT , 100);
	m_wndLeftProductListCtrl.InsertColumn(3 , _T("DESC") , LVCFMT_LEFT , 300);

	m_wndRightProductListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES);
	m_wndRightProductListCtrl.InsertColumn(0 , _T("PRODUCT NAME") , LVCFMT_LEFT , 150);
	m_wndRightProductListCtrl.InsertColumn(1 , _T("REVISION NO") , LVCFMT_LEFT , 100);
	m_wndRightProductListCtrl.InsertColumn(2 , _T("DATE") , LVCFMT_LEFT , 100);
	m_wndRightProductListCtrl.InsertColumn(3 , _T("DESC") , LVCFMT_LEFT , 300);

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-28 오전 11:19:07	

	@param	

	@return		
*/
int CCompareRevisionDataDlg::DisplayRevisionData(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string rMDBFilePath = docData.GetProjectMDBFilePath();

	CADODB adoDB;
	const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rConnectionString))
	{
		adoDB.OpenQuery(STRING_T(_T("SELECT * FROM T_PRODUCT_REVISION WHERE C_PRODUCT_NAME = '")) + m_rProductName.operator LPCTSTR() + STRING_T(_T("' ORDER BY C_REVISION_NO DESC")));
		LONG lRecordCount = 0L;
		adoDB.GetRecordCount(&lRecordCount);

		for(int i = 0;i < lRecordCount;++i)
		{
			const int nItem = m_wndRevisionListCtrl.InsertItem(i , m_rProductName);
			if(-1 != nItem)
			{
				STRING_T strValue;
				adoDB.GetFieldValue(i , _T("C_REVISION_NO") , &strValue);
				m_wndRevisionListCtrl.SetItemText(nItem , 1 , strValue.c_str());
				adoDB.GetFieldValue(i , _T("C_DATE") , &strValue);
				m_wndRevisionListCtrl.SetItemText(nItem , 2 , strValue.c_str());
				adoDB.GetFieldValue(i , _T("C_DESC") , &strValue);
				m_wndRevisionListCtrl.SetItemText(nItem , 3 , strValue.c_str());
			}
		}
	}
	else
	{
		AfxMessageBox(_T("Fail to connect database"));
	}

	return 0;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-28 오전 11:19:03	

	@param	

	@return		
*/
BOOL CCompareRevisionDataDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	HICON hIcon = AfxGetApp()->LoadIcon(IDI_REVISION);
	SetIcon(hIcon , FALSE);

	//! 테이블이 없으면 생성합니다.
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string rProjectMDBFilePath = docData.GetProjectMDBFilePath();

	TABLES::CELOADTable table;
	table.CreateTable(rProjectMDBFilePath , _T("T_PRODUCT_REVISION"));
	//!

	CreateRevisionListCtrl();
	DisplayRevisionData();

	m_wndOKButton.SetIcon(IDI_OK_18X18);
	m_wndCancelButton.SetIcon(IDI_CANCEL_18X18);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	

	@author BHK	

	@date 2009-04-28 오전 11:32:52	

	@param	

	@return		
*/
void CCompareRevisionDataDlg::OnLvnBegindragListRevision(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if(NULL == pNMLV) return;

	OnBeginDrag(pNMLV , m_wndRevisionListCtrl);

	*pResult = 0;
}

/**
	@brief	BEGIN DRAG

	@author BHK	

	@return		
**/
int CCompareRevisionDataDlg::OnBeginDrag(LPNMLISTVIEW pNMLV, CListCtrl& ListCtrl)
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

		ListCtrl.SetFocus();
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

	@date 2009-04-28 오전 11:36:57	

	@param	

	@return		
*/
void CCompareRevisionDataDlg::OnMouseMove(UINT nFlags, CPoint point)
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
				CListCtrl* pList = (CListCtrl*)m_pDropList;
				m_nDropIndex = -1;
			}
			else //If we drag out of the CListCtrl altogether
			{
				TRACE("m_nDropIndex is not -1\n");
			}
		}

		// Save current window pointer as the CListCtrl we are dropping onto
		m_pDropList = (CListCtrl*)pDropWnd;

		// Convert from screen coordinates to drop target client coordinates
		pDropWnd->ScreenToClient(&pt);

		//If we are hovering over a CListCtrl we need to adjust the highlights
		if(pDropWnd->IsKindOf(RUNTIME_CLASS(CMFCListCtrl)) && (pDropWnd->GetSafeHwnd() == m_wndLeftProductListCtrl.GetSafeHwnd()))
		{
			TRACE("m_nDropIndex is not 100\n");
			//Note that we can drop here
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			CListCtrl* pListCtrl = (CListCtrl*)pDropWnd;
		}
		else if(pDropWnd->IsKindOf(RUNTIME_CLASS(CMFCListCtrl)) && (pDropWnd->GetSafeHwnd() == m_wndRightProductListCtrl.GetSafeHwnd()))
		{
			TRACE("m_nDropIndex is not 100\n");
			//Note that we can drop here
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			CListCtrl* pList = (CListCtrl*)pDropWnd;
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

	@date 2009-04-28 오전 11:40:36	

	@param	

	@return		
*/
void CCompareRevisionDataDlg::OnLButtonUp(UINT nFlags, CPoint point)
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
			m_pDropList = (CListCtrl*)pDropWnd; //Set pointer to the list we are dropping on
			DropItemOnList(m_pDragList, m_pDropList); //Call routine to perform the actual drop
		}
	}

	CDialog::OnLButtonUp(nFlags, point);
}

/**
	@brief	DROP ITEM IN SCHEMATIC TREELIST CTRL

	@author KHS	

	@return		
**/
void CCompareRevisionDataDlg::DropItemOnList(CListCtrl* pDragList , CListCtrl* pDropList)
{
	if((pDropList->GetSafeHwnd() == m_wndLeftProductListCtrl.GetSafeHwnd()) || 
		(pDropList->GetSafeHwnd() == m_wndRightProductListCtrl.GetSafeHwnd()) )
	{
		CString rRevNo = pDragList->GetItemText(m_nDragIndex , 1);

		pDropList->DeleteAllItems();
		CELoadDocData& docData = CELoadDocData::GetInstance();
		const string rMDBFilePath = docData.GetProjectMDBFilePath();

		CADODB adoDB;
		const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;
		if(TRUE == adoDB.DBConnect(rConnectionString))
		{
			CString rFormat(_T("SELECT * FROM T_PRODUCT_REVISION WHERE C_PRODUCT_NAME = '%s' AND C_REVISION_NO = %d")) , rSQL;

			rSQL.Format(rFormat , m_rProductName , atoi(rRevNo));
			adoDB.OpenQuery(rSQL.operator LPCTSTR());
			LONG lRecordCount = 0L;
			adoDB.GetRecordCount(&lRecordCount);
			if(1 == lRecordCount)
			{
				STRING_T rDate , rDesc;
				int nItem = pDropList->InsertItem(0 , m_rProductName);
				if(-1 != nItem)
				{
					CString strValue;
					pDropList->SetItemText(nItem , 1 , rRevNo);
					adoDB.GetFieldValue(0 , _T("C_DATE") , &rDate);
					pDropList->SetItemText(nItem , 2 , rDate.c_str());
					adoDB.GetFieldValue(0 , _T("C_DESC") , &rDesc);
					pDropList->SetItemText(nItem , 3 , rDesc.c_str());

					////////////////////////////////////////////////////////////////////////////////////////////////////
					if(pDropList->GetSafeHwnd() == m_wndLeftProductListCtrl.GetSafeHwnd())
					{
						if(m_nRightSideRevNo == atoi(rRevNo))
						{
							AfxMessageBox(_T("비교하려는 REVISION이 동일합니다."));
							return;
						}
						m_nLeftSideRevNo = atoi(rRevNo);
					}
					else if(pDropList->GetSafeHwnd() == m_wndRightProductListCtrl.GetSafeHwnd())
					{
						if(m_nLeftSideRevNo == atoi(rRevNo))
						{
							AfxMessageBox(_T("비교하려는 REVISION이 동일합니다."));
							return;
						}
						m_nRightSideRevNo = atoi(rRevNo);
					}
				}
			}
		}
	}
	else
	{
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-04-28 오후 1:26:46	

	@param	

	@return		
*/
void CCompareRevisionDataDlg::OnNMClickListLeftProduct(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	CPoint point;
	GetCursorPos(&point);
	m_wndLeftProductListCtrl.ScreenToClient(&point);
	UINT uFlags;
	const int nItem = m_wndLeftProductListCtrl.HitTest(point , &uFlags);
	if(-1 != nItem)
	{
		m_nLeftSideRevNo = atoi(m_wndLeftProductListCtrl.GetItemText(nItem , 0));
		m_wndRightProductListCtrl.SetItemState(nItem , LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		m_nRightSideRevNo= atoi(m_wndRightProductListCtrl.GetItemText(nItem , 0));
	}
	else
	{
		m_nLeftSideRevNo = m_nRightSideRevNo = -1;
	}

	*pResult = 0;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-28 오후 1:27:13	

	@param	

	@return		
*/
void CCompareRevisionDataDlg::OnNMClickListRightProduct(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	CPoint point;
	GetCursorPos(&point);
	m_wndRightProductListCtrl.ScreenToClient(&point);
	UINT uFlags;
	const int nItem = m_wndRightProductListCtrl.HitTest(point , &uFlags);
	if(-1 != nItem)
	{
		m_nRightSideRevNo = atoi(m_wndRightProductListCtrl.GetItemText(nItem , 0));
		m_wndLeftProductListCtrl.SetItemState(nItem , LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		m_nLeftSideRevNo = atoi(m_wndLeftProductListCtrl.GetItemText(nItem , 0));
	}
	else
	{
		m_nLeftSideRevNo = m_nRightSideRevNo = -1;
	}

	*pResult = 0;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-28 오후 1:41:14	

	@param	

	@return		
*/
void CCompareRevisionDataDlg::OnBnClickedOk()
{
	if((-1 != m_nLeftSideRevNo) && (-1 != m_nRightSideRevNo) && (m_nLeftSideRevNo != m_nRightSideRevNo))
	{
		return OnOK();
	}

	AfxMessageBox(_T("Revision 비교 작업을 할 수 없습니다."));
}
