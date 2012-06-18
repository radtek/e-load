
// ELOADView.cpp : CELOADView 클래스의 구현
//

#include "stdafx.h"
#include "ELOAD.h"

#include "ELOADDoc.h"
#include "ELoadDocData.h"
#include "FillGridCellComboBox.h"
#include <Tokenizer.h>
#include <isstring.h>
#include <string>
#include <vector>
#include "GridCellEx.h"
#include "commands/CommandAutoCalculateFLC.h"

#include "ELOADView.h"

using namespace COMMAND;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CELOADView

IMPLEMENT_DYNCREATE(CELOADView, CView)

BEGIN_MESSAGE_MAP(CELOADView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_SAVE, &CELOADView::OnFileSave)
	ON_WM_KEYDOWN()
        ON_COMMAND(ID_EDIT_PASTE, &CELOADView::OnEditPaste)
        ON_COMMAND(ID_EDIT_COPY, &CELOADView::OnEditCopy)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CELOADView 생성/소멸

CELOADView::CELOADView()
{
	// TODO: add construction code here
        m_pObserver = new CObserver(this , &CELOADView::OnEventFromObserver);
        m_bHeaderSort = false;
	m_bBlockMoveNext = false;
}

CELOADView::~CELOADView()
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

BOOL CELOADView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CELOADView 그리기

void CELOADView::OnDraw(CDC* /*pDC*/)
{
	CELOADDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}

void CELOADView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CELOADView::OnContextMenu(CWnd* pWnd, CPoint point)
{
///	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CELOADView 진단

#ifdef _DEBUG
void CELOADView::AssertValid() const
{
	CView::AssertValid();
}

void CELOADView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CELOADDoc* CELOADView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CELOADDoc)));
	return (CELOADDoc*)m_pDocument;
}
#endif //_DEBUG


// CELOADView 메시지 처리기
/**
	@brief	VIEW가 생성되고 난 바로 뒤에....

	@author	KHS,BHK
*/
int CELOADView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CRect empty;
        GetClientRect(empty);

	if(FALSE == m_dlgMultiSort.Create(CMultiSortDlg::IDD , this))
	{
		AfxMessageBox("Can't create multi sort dialog.");
                return -1;
	}
	m_dlgMultiSort.ShowWindow(SW_NORMAL);

        if(FALSE == m_wndGridCtrl.Create(empty, this , 100))
        {
                AfxMessageBox("Can't create grid ctrl.");
                return -1;
        }
        // GridCtrl Font
        CFont font;
        font.CreateFont(-11,0,0,0,400,FALSE,FALSE,FALSE,HANGUL_CHARSET,3,2,1,VARIABLE_PITCH | FF_MODERN,"Arial"); 
        m_wndGridCtrl.SetFont(&font);
        font.DeleteObject();

        // fill it up with stuff
        m_wndGridCtrl.SetEditable(TRUE);
        m_wndGridCtrl.EnableDragAndDrop(FALSE);
        m_wndGridCtrl.SetRowResize(FALSE);
        m_wndGridCtrl.SetColumnResize(TRUE);
        m_wndGridCtrl.SetListMode(FALSE);
        m_wndGridCtrl.SetSingleRowSelection(TRUE);
        m_wndGridCtrl.SetSortAscending(FALSE);
        m_wndGridCtrl.SetHeaderSort(TRUE);
        m_wndGridCtrl.SetSingleColSelection(FALSE);
        m_wndGridCtrl.EnableTitleTips(FALSE);
	m_wndGridCtrl.EnableDragAndDrop(FALSE);
	m_wndGridCtrl.SetFixedRowSelection(TRUE);
        /////////////////////////////////////////////////////////////////
        try 
        {
                m_wndGridCtrl.SetRowCount(5);
                m_wndGridCtrl.SetColumnCount(10 + 2);
                m_wndGridCtrl.SetFixedRowCount(1);
                m_wndGridCtrl.SetFixedColumnCount(1);
		
		m_wndGridCtrl.SetScrollLockColumnCount(1);	//! SET SCROLL LOCK COLUMN
        }
        catch (CMemoryException* e)
        {
                e->ReportError();
                e->Delete();
                return FALSE;
        }

	OnHeaderSort(); 
        try
        {
		CELoadDocData& docData = CELoadDocData::GetInstance();
                CELoadItemProp* pProp = docData.m_ItemPropMap[CLoadItem::TypeString()];
                if(pProp)
                {
                        int nColSize = 0;
                        const size_t nCategorySize = pProp->GetCategoryCount();
                        for(size_t i = 0; i < nCategorySize;++i)
                        {
                                CItemPropCategory* pCategory = pProp->GetCategoryAt(i);
                                if(NULL == pCategory) continue;

                                const int nItemSize = pCategory->GetItemCount();
                                nColSize += nItemSize;
                        }

                        m_wndGridCtrl.SetColumnCount( nColSize + 1);
                }

                // HEADER를 WRITE한다.
                int nRow = 0;
                ///for(vector<CELoadItem*>::iterator itr = pDoc->m_ItemEntry.begin();itr != pDoc->m_ItemEntry.end();++itr)
                {
                        int nCol = 0;
                        GV_ITEM Item;
                        Item.mask = GVIF_TEXT|GVIF_FORMAT;

                        ///CELoadItemProp* pProp = (*itr)->prop();

                        const size_t nCategorySize = pProp->GetCategoryCount();
                        for(size_t i = 0; i < nCategorySize;i++)
                        {
                                CItemPropCategory* pCategory = pProp->GetCategoryAt(i);
                                if(NULL == pCategory) continue;

                                const int nItemSize = pCategory->GetItemCount();
                                CString rCategoryName = pCategory->name();
                                for(int j = 0;j < nItemSize;j++)
                                {
                                        CItemPropCategory::ItemPropTag* pTag = pCategory->GetItemAt(j);
                                        if(NULL == pTag) continue;

                                        Item.row = 0;
                                        Item.col = nCol + j + 1;
                                        if((nItemSize - 1) == j) nCol = nCol + j + 1;
                                        Item.nFormat = DT_LEFT|DT_WORDBREAK;//DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
                                        //CString rText(rCategoryName + "," + pTag->rKey.c_str());
                                        CString rText(pTag->rKey.c_str());
                                        Item.strText = rText;
                                        if(TRUE == m_wndGridCtrl.SetItem(&Item))
					{
						//! header cell에 ItemPropTag 포인터를 저장한다.
						m_wndGridCtrl.SetItemData(0 , Item.col , (LPARAM)pTag);
					}
                                }
                        }
                        ///break;

			WriteELoadItemsToGrid();
			m_wndGridCtrl.AutoSizeColumns();
                }
        }
        catch (exception& e)
        {
                AfxMessageBox(e.what());
                return -1;
        }

        ///fill rows/cols with text
        /*
	for(int nRow = 0; nRow < m_wndGridCtrl.GetRowCount();nRow++)
        {
                for(int nCol = 0;nCol < m_wndGridCtrl.GetColumnCount();nCol++)
                {
                        GV_ITEM Item;
                        Item.mask = GVIF_TEXT|GVIF_FORMAT;
                        Item.row = nRow;
                        Item.col = nCol;
                        if ((nRow < 1) && (nCol > 0))
                        {
                                Item.nFormat = DT_CENTER|DT_WORDBREAK;
                                int nChar = int('A');
                                nChar += (nCol - 1);
                                Item.strText.Format(_T("%c"),char(nChar));
                        } 
                        else if ((nCol < 1) && (nRow > 0)) 
                        {
                                Item.nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
                                Item.strText.Format(_T("%d"),nRow);
                        } 
                        else 
                        {
                        }
                        m_wndGridCtrl.SetItem(&Item);
                }	
        }
        m_wndGridCtrl.AutoSizeRows();
	*/

	return 0;
}

void CELOADView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if(m_dlgMultiSort.GetSafeHwnd())
	{
		m_dlgMultiSort.SetWindowPos(NULL, 0, 0, cx, 32, SWP_NOACTIVATE | SWP_NOZORDER);
	}

	if(m_wndGridCtrl.GetSafeHwnd())
	{
		CRect rectClient;
		GetClientRect (rectClient);
		
		m_wndGridCtrl.SetWindowPos (NULL, 0, 32, rectClient.Width (), rectClient.Height () - 32,
							SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

/**
	@brief	

	@author	BHK
*/
void CELOADView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
}

/**
	@brief	OBSERVER를 통해 호출되는 루틴

	@author	BHK
*/
int CELOADView::OnEventFromObserver(CObserver* pObserver , CEventData* pEventData)
{
	if(CEventData::MODIFY_ITEM_PROP == pEventData->m_eEventType)
	{
		((CELOADView*)pObserver->owner())->UpdateELoadItems(pEventData->m_SelectionSet);
	}

	return ERROR_SUCCESS;
}

/**
	@brief  : Show ELoadItem

	@author  KHS,BHK
*/
int CELOADView::WriteELoadItemsToGrid()
{
	CELOADDoc* pDoc = GetDocument();

	if(pDoc && m_wndGridCtrl.GetSafeHwnd())
        {
		CELoadDocData& docData = CELoadDocData::GetInstance();
		const COLORREF ItemIdColor   = docData.GetColorCode("ITEM_ID_BACK_COLOR");
		const COLORREF ReadOnlyColor = docData.GetColorCode("READ_ONLY_BACK_COLOR");

		// 필드
		int nRow = 1;
		size_t nItemCount = 0;
		for(vector<CELoadItem*>::iterator itr = pDoc->m_ItemEntry.begin();itr != pDoc->m_ItemEntry.end();++itr)
		{
			int nCol = 0;
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;

			CELoadItemProp* pProp = (*itr)->prop();
			CString rId = (*itr)->GetName().c_str();
			if(pProp)
			{
				Item.row = nRow;
				Item.col = nCol;
				Item.nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
				Item.strText = (*itr)->GetName().c_str();
				m_wndGridCtrl.SetItem(&Item);

				const size_t nCategorySize = pProp->GetCategoryCount();
				for(size_t i = 0; i < nCategorySize;i++)
				{
					CItemPropCategory* pCategory = pProp->GetCategoryAt(i);
					if(NULL == pCategory) continue;

					const int nItemSize = pCategory->GetItemCount();
					nItemCount = nCategorySize * nItemSize;
					for(int j = 0;j < nItemSize;j++)
					{
						CItemPropCategory::ItemPropTag* pTag = pCategory->GetItemAt(j);
						if(NULL == pTag) continue;

						Item.row = nRow;
						Item.col = nCol + j + 1;
						if((nItemSize - 1) == j) nCol = nCol + j + 1;
						if("REGEXP(NUMBER)" == pTag->rValueType)
						{
							Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
						}
						else
						{
							Item.nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
						}
					
						if( "R" == pTag->rAccess)
						{
							CGridCell* pCell = (CGridCell*)m_wndGridCtrl.GetCell(Item.row , Item.col);
							pCell->SetState(pCell->GetState() | GVIS_READONLY);
							pCell->SetBackClr(ReadOnlyColor);
						}

						if("ITEM ID" == pTag->rKey)
						{
							CGridCell* pCell = (CGridCell*)m_wndGridCtrl.GetCell(Item.row , Item.col);
							pCell->SetBackClr( ItemIdColor );
						}
						
						Item.strText = pTag->rValue.c_str();
						m_wndGridCtrl.SetItem(&Item);
						const CString rValueType = pTag->rValueType.c_str();
						if( IS_COMBOBOX(rValueType) )
						{
							CGridCellEx::DATA_TYPE_STRING = rValueType;
							m_wndGridCtrl.SetCellType(Item.row, Item.col , RUNTIME_CLASS(CFillGridCellComboBox));
						}
						else
						{
							CGridCellEx::DATA_TYPE_STRING = rValueType;
							m_wndGridCtrl.SetCellType(Item.row, Item.col , RUNTIME_CLASS(CGridCellEx));
						}
						m_wndGridCtrl.SetItemData(Item.row , Item.col , (LPARAM)pTag);
					}
				}
			}
			Item.col = 0;
			Item.strText.Format(" %d " , nRow);
			m_wndGridCtrl.SetItem(&Item);
			m_wndGridCtrl.SetItemData(nRow , 0 , (LPARAM)*itr);
			++nRow;
		}

		m_wndGridCtrl.AutoSizeRows();
		m_wndGridCtrl.AutoSizeColumns();
		m_wndGridCtrl.RedrawWindow();

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief  LOAD ITEM 속성이 변경 되었을때...

	@author	BHK
*/
BOOL CELOADView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class
	if(wParam == m_wndGridCtrl.GetDlgCtrlID())
	{
		NM_GRIDVIEW* pGridView = (NM_GRIDVIEW*)(lParam);

		if(GVN_ENDLABELEDIT == pGridView->hdr.code)
		{
			CELoadItem* pItem = (CELoadItem*)m_wndGridCtrl.GetItemData(pGridView->iRow , 0);
			if(pItem)
			{
				CString rTemp = pItem->GetName().c_str();
				CGridCellBase* pCell = m_wndGridCtrl.GetCell(0, pGridView->iColumn);
				CString rText  = pCell->GetText();
				pCell = m_wndGridCtrl.GetCell(pGridView->iRow, pGridView->iColumn);
				CString rValue = pCell->GetText();

				CItemPropCategory::ItemPropTag* pTag = (CItemPropCategory::ItemPropTag *)m_wndGridCtrl.GetItemData(0 , pGridView->iColumn);
				if(pTag && pTag->pParent)
				{			
					//! set changed value
					string rCategory = pTag->pParent->name();
					pItem->prop()->SetValue(rCategory , pTag->rKey.c_str() , rValue.operator LPCSTR());

					CELoadDocData& docData = CELoadDocData::GetInstance();
					//! 자동 계산을 수행하는 COMMAND가 등록되어 있으면 COMMAND를 실행한다.
					auto_ptr<CEventData> command(docData.CreateAutoCalculateCommandOf(pItem->GetTypeString() , rCategory , pTag->rKey));
					if(NULL != command.get())
					{
						command->m_SelectionSet.Add(pItem);
						command->Execute();
						UpdateELoadItems(command->m_SelectionSet);
					}
					
					//! ITEM이 변경되었다는 것은 OBSERVER를 통해 통보한다.
					{
						CEventData EventData(pItem->GetTypeString() , CEventData::MODIFY_ITEM_PROP);
						EventData.m_SelectionSet.Add(pItem);
						m_pObserver->FireEvent( EventData );
					}
				}
				else
				{
					ELOAD_LOG4CXX_WARN(mylogger , "" , 5);
				}
			}
		}
	}

	return CView::OnNotify(wParam, lParam, pResult);
}
/**
        @brief  For Copy & Paste of Gerictrl
*/
void CELOADView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	CCellID cell = m_wndGridCtrl.GetFocusCell();
	if (!m_wndGridCtrl.IsValid(cell))
	{
		CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}
	if (IsCTRLpressed())
	{
		switch (nChar)
		{
		case 'C':
			OnEditCopy();
			return;
		case 'V':
			OnEditPaste();
			return;
		}
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CELOADView::OnEditPaste()
{
	m_bBlockMoveNext = true;
	m_wndGridCtrl.OnEditPaste();
	m_bBlockMoveNext = false;

	//! 다음 칸으로 SELECTED CELL을 이동한다.
	CCellRange cellRange = m_wndGridCtrl.GetSelectedCellRange();
	if(cellRange.GetMaxRow() + 1 < m_wndGridCtrl.GetRowCount())
	{
		m_wndGridCtrl.SetSelectedRange(cellRange.GetMaxRow() + 1,cellRange.GetMinCol(),cellRange.GetMaxRow() + 1,cellRange.GetMinCol());
		m_wndGridCtrl.SetFocusCell(cellRange.GetMaxRow() + 1,cellRange.GetMinCol());
	}
	else
	{
		m_wndGridCtrl.SetSelectedRange(cellRange.GetMaxRow(),cellRange.GetMinCol(),cellRange.GetMaxRow(),cellRange.GetMinCol());
		m_wndGridCtrl.SetFocusCell(cellRange.GetMaxRow(),cellRange.GetMinCol());
	}
}

void CELOADView::OnEditCopy()
{
	// TODO: Add your command handler code here
	m_wndGridCtrl.OnEditCopy();
}
void CELOADView::OnHeaderSort()
{
	m_bHeaderSort = !m_bHeaderSort;
	m_wndGridCtrl.SetHeaderSort(m_bHeaderSort);
}

/**
	@brief	창이 활성화 될때 불리어 진다.

	@author	BHK
*/
void CELOADView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	// TODO: Add your specialized code here and/or call the base class
	if(TRUE == bActivate)
	{
		/*
		string rItemCategoryString;
		CEventData EventData("" , CEventData::SELECT_ITEM);
		for(vector<CELoadItem*>::iterator itr = GetDocument()->m_ItemEntry.begin(); itr !=GetDocument()->m_ItemEntry.end();++itr)
		{
		rItemCategoryString = (*itr)->GetTypeString();
		EventData.m_SelectionSet.Add((*itr));
		//const size_t& nCount = GetDocument()->m_ItemEntry.size();
		}
		EventData.SetItemCategory( rItemCategoryString );
		m_pObserver->FireEvent(EventData);
		*/
	}

	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

/**
	@brief  Load 데이타를 DATABASE에 저장한다.
	BUS_GROUP & BUS는 순서대로 저장...

	@author	KHS
*/
void CELOADView::OnFileSave()
{
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp) pApp->OnSaveDataBase();
}

/**
	@brief	document가 수정되고 나서 불리어 진다.

	@author	BHK
*/
void CELOADView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	if(m_wndGridCtrl.GetSafeHwnd())
	{
		CELOADDoc* pDoc = GetDocument();
                CELoadDocData& docData = CELoadDocData::GetInstance();
                const size_t nSize = pDoc->m_ItemEntry.size();
                m_wndGridCtrl.SetRowCount( nSize + 1 ); //! + 1 because of header row

		WriteELoadItemsToGrid();
	}
	//if(m_wndGridCtrl.GetSafeHwnd()) m_wndGridCtrl.RedrawWindow();
}

/**
	@brief	BACKGROUND는 지울 필요가 없다.(grid ctrl이 VIEW에 꽉차기 때문에)

	@author	BHK
*/
BOOL CELOADView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

/**
	@brief	

	@author	BHK
*/
int CELOADView::UpdateELoadItems(CItemSelectionSet& ItemSelectionSet)
{
	const int nItemCount = ItemSelectionSet.GetItemCount();
	for(int i = 0;i < nItemCount;++i)
	{
		CELoadItem* pItem = ItemSelectionSet.GetItemAt(i);
		int nRow = 0;
		for(nRow = 1;nRow < m_wndGridCtrl.GetRowCount();++nRow)
		{
			CELoadItem* pData = (CELoadItem*)m_wndGridCtrl.GetItemData(nRow,0);
			if(pData == pItem) break;
		}

		if(nRow < m_wndGridCtrl.GetRowCount())
		{
			for(int j = 1;j < m_wndGridCtrl.GetColumnCount();++j)
			{
				CItemPropCategory::ItemPropTag* pTag = (CItemPropCategory::ItemPropTag*)m_wndGridCtrl.GetItemData(0,j);
				if(pTag)
				{
					CGridCellBase* pCell = m_wndGridCtrl.GetCell(nRow,j);

					string rCategory = pTag->pParent->name();
					string rKey = pTag->rKey;
					pCell->SetText(pItem->prop()->GetValue(rCategory , rKey).c_str());
				}
			}
		}
	}

	m_wndGridCtrl.RedrawWindow();

	return ERROR_SUCCESS;
}

void CELOADView::OnMouseMove(UINT nFlags, CPoint point)
{
	
	CView::OnMouseMove(nFlags, point);
}

/**
	@brief	

	@author	BHK
*/
CString CELOADView::GetCurSelColumnName(int& nColNo)
{
	nColNo = -1;

	CCellRange cellRange = m_wndGridCtrl.GetSelectedCellRange();
	if(-1 != cellRange.GetMinCol())
	{
		nColNo = cellRange.GetMinCol();
		return m_wndGridCtrl.GetItemText(0,nColNo);
	}

	return "";
}

/**
	@brief	

	@author	BHK
*/
int CELOADView::OnMultiSort(void)
{
	vector<int> MultiSortCellNoEntry , MultiSortDirEntry;
	m_dlgMultiSort.GetMultiSortInfoEntry(MultiSortCellNoEntry , MultiSortDirEntry);
	m_wndGridCtrl.SetMultiSortInfoEntry(MultiSortCellNoEntry , MultiSortDirEntry);
	m_wndGridCtrl.MultiSortItems(m_wndGridCtrl.GetFixedRowCount() , m_wndGridCtrl.GetRowCount() - 1);
	m_wndGridCtrl.Invalidate();

	return ERROR_SUCCESS;
}
