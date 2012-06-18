// GridCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "ELoad.h"
#include "GridCtrlEx.h"


// CGridCtrlEx

IMPLEMENT_DYNAMIC(CGridCtrlEx, CGridCtrl)

CGridCtrlEx::CGridCtrlEx()
{
	m_pFindDialog = NULL;
	m_bRemoveShowSelAlwaysAtFindDialogExit = false;

	m_rFindName = _T("");
	m_bMatchCase = false;
	m_bMatchWholeWord = false;
	m_bSearchDown= true;
}

CGridCtrlEx::~CGridCtrlEx()
{

}

const UINT CGridCtrlEx::m_FindDialogMessage = RegisterWindowMessage(FINDMSGSTRING);

BEGIN_MESSAGE_MAP(CGridCtrlEx, CGridCtrl)
        ON_WM_RBUTTONDOWN()
        ON_COMMAND(ID_PASTECTRL, &CGridCtrlEx::OnPastectrl)
        ON_COMMAND(ID_COPYCTRL, &CGridCtrlEx::OnCopyctrl)
        ON_WM_RBUTTONUP()
        ON_WM_MOUSEACTIVATE()
	ON_WM_KEYDOWN()
	ON_REGISTERED_MESSAGE(m_FindDialogMessage, OnFindDialogMessage)
END_MESSAGE_MAP()

// CGridCtrlEx message handlers

void CGridCtrlEx::OnRButtonDown(UINT nFlags, CPoint point)
{
        // TODO: Add your message handler code here and/or call default
        ClientToScreen(&point);
        if(IsWindowVisible())
        {
                theApp.GetContextMenuManager()->ShowPopupMenu(IDR_GRID_CONTEXT_MENU,
                        point.x , point.y , this, TRUE);
        }
        //CGridCtrl::OnRButtonDown(nFlags, point);
}
void CGridCtrlEx::OnPastectrl()
{
        // TODO: Add your command handler code here
        OnEditPaste();
}

void CGridCtrlEx::OnCopyctrl()
{
        // TODO: Add your command handler code here
        OnEditCopy();
}

void CGridCtrlEx::OnRButtonUp(UINT nFlags, CPoint point)
{
        // TODO: Add your message handler code here and/or call default
        
        //CGridCtrl::OnRButtonUp(nFlags, point);
}

/**
	@brief	multi sort function.

	@author	BHK
*/
BOOL CGridCtrlEx::MultiSortItems(const int& low , const int& high)
{
///	if (high == -1)
///		high = GetRowCount() - 1;
	
	int lo = low;
	int hi = high;
	const int mid= int((lo + hi)*0.5);

	ResetSelectedRange();
	SetFocusCell(-1, - 1);

	for(int i = low;i < high;++i)
	{
		for(int j = i + 1;j <= high;++j)
		{
			if(MultiCompare(i , j) > 0)
			{
				for (int col = 0; col < GetColumnCount(); col++)
				{
					CGridCellBase *pCell = GetCell(i, col);
					SetCell(i, col, GetCell(j, col));
					SetCell(j, col, pCell);
				}
				UINT nRowHeight    = m_arRowHeights[i];
				m_arRowHeights[i] = m_arRowHeights[j];
				m_arRowHeights[j] = nRowHeight;
			}
		}
	}
	
	return TRUE;
}

/**
	@brief	

	@author	
*/
int CGridCtrlEx::MultiCompare(const int& lhs, const int& rhs)
{
	int nIndex = 0;
	for(vector<int>::iterator itr = m_MultiSortCellNoEntry.begin();itr != m_MultiSortCellNoEntry.end();++itr,++nIndex)
	{
		if(-1 == (*itr)) continue;

		CGridCellBase* pLhsCell = GetCell(lhs , *itr);
		CGridCellBase* pRhsCell = GetCell(rhs , *itr);
		int res = _tcscmp(pLhsCell->GetText() , pRhsCell->GetText());
		if(0 != res)
		{
			return (0 == m_MultiSortDirEntry[nIndex]) ? res : -res;
		}
	}
	
	return 0;
}

int CGridCtrlEx::SetMultiSortInfoEntry(const vector<int>& MultiSortCellNoEntry , const vector<int>& MultiSortDirEntry)
{
	m_MultiSortCellNoEntry.clear();
	m_MultiSortCellNoEntry.insert(m_MultiSortCellNoEntry.begin() , MultiSortCellNoEntry.begin() , MultiSortCellNoEntry.end());

	m_MultiSortDirEntry.clear();
	m_MultiSortDirEntry.insert(m_MultiSortDirEntry.begin() , MultiSortDirEntry.begin() , MultiSortDirEntry.end());

	return ERROR_SUCCESS;
}
/*
        @brief  마우스 드레그 텍스트 복사
        @author KHS
*/
int CGridCtrlEx::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
        // TODO: Add your message handler code here and/or call default
        //CCellRange cellRange = CGridCtrl::GetSelectedCellRange();
        //if(-1 != cellRange.GetMaxRow())
        //{
        //        int nColCount = cellRange.GetMaxCol() - cellRange.GetMaxCol();
        //        if( 0 == nColCount)
        //        {
        //                int nCellCount = cellRange.GetMaxRow() - cellRange.GetMinRow();
        //                int nCol = cellRange.GetMinCol();
        //                int nRow = cellRange.GetMinRow();//GVIS_READONLY

        //                CGridCell* pCell = (CGridCell*)GetCell(nRow, nCol);
        //                if( 138 != pCell->GetState())
        //                {
        //                        CString rText = pCell->GetText();
        //                        for(int i = 1;i < nCellCount + 1;i++)
        //                        {
        //                                SendMessageToParent(nRow + i , nCol, GVN_BEGINLABELEDIT);
        //                                SetItemText(nRow + i , nCol, rText);
        //                                SetModified(TRUE, nRow + i , nCol);
        //                                SendMessageToParent(nRow + i , nCol, GVN_ENDLABELEDIT);
        //                        }
        //                }
        //        }
        //}
        return CGridCtrl::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

/**
	@brief	Find Replace Dialog 

	@author KHS	

	@date 2009-05-26 오후 1:34:14	

	@param	

	@return		
**/
int CGridCtrlEx::OnFind(void)
{
	CCellID cell = GetFocusCell();
	if(cell.IsValid())
	{
		m_pFindDialog = new CFindReplaceDialog();
		if(m_pFindDialog)
		{
			m_pFindDialog->Create(TRUE, "", NULL, FR_DOWN, this);
		}
		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;;
}

void CGridCtrlEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	CCellID cell = GetFocusCell();
	if (!IsValid(cell))
	{
		CGridCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}
	if (IsCTRLpressed())
	{
		switch (nChar)
		{
		case 'F':
			OnFind();
			return;
		}
	}
	else 
	{
		switch (nChar)
		{
		case VK_F3:
			OnFindContinue();
			return;
		}
	}
	CGridCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}
/**
	@brief	Next find

	@author KHS	

	@date 2009-05-26 오후 2:30:07	

	@param	

	@return		
**/
LRESULT CGridCtrlEx::OnFindDialogMessage(WPARAM wParam, LPARAM lParam)
{
	ASSERT(m_pFindDialog != NULL);

	// If the FR_DIALOGTERM flag is set,
	// invalidate the handle identifying the dialog box.
	if (m_pFindDialog->IsTerminating())
	{
		if (m_bRemoveShowSelAlwaysAtFindDialogExit)
		{
			SetWindowLong(GetSafeHwnd(), GWL_STYLE,
				GetWindowLong(GetSafeHwnd(), 
				GWL_STYLE) & (~((LONG)TVS_SHOWSELALWAYS)));

			RedrawWindow();
		}

		m_pFindDialog = NULL;
		return 0;
	}

	// If the FR_FINDNEXT flag is set,
	// call the application-defined search routine
	// to search for the requested string.
	if(m_pFindDialog->FindNext())
	{
		//read data from dialog
		m_rFindName = m_pFindDialog->GetFindString();
		m_bMatchCase = m_pFindDialog->MatchCase() == TRUE;
		m_bMatchWholeWord = m_pFindDialog->MatchWholeWord() == TRUE;
		m_bSearchDown = m_pFindDialog->SearchDown() == TRUE;

		//with given name do search
		FindWhatYouNeed(m_rFindName, m_bMatchCase, m_bMatchWholeWord, m_bSearchDown);
	}

	return 0;
}

/**
	@brief	FindName을 찾는다.

	@author KHS	

	@date 2009-05-26 오후 3:18:13	

	@param	

	@return		
**/
bool CGridCtrlEx::FindWhatYouNeed(CString FindName, bool bMatchCase, bool bMatchWholeWord, bool bSearchDown)
{
	if(bMatchCase == false)
		FindName.MakeUpper();

	bool bContinueSearch = true;
	
	CCellID startCell = GetFocusCell();
	CCellID actCell = startCell;

	if(IsValid(actCell))
	{
		goto L_SEARCH;
	}

	do
	{
		actCell.col = 1;
		actCell.row = 1;
		for(actCell = bSearchDown ? actCell
			: GetPrevCell(actCell);
			bContinueSearch && IsValid(actCell);)
		{
			if(actCell == startCell)
			{
				bContinueSearch = false;
				break;
			}
			{
				CString rName = CGridCtrlEx::GetItemText(actCell.row, actCell.col);
				if(bMatchCase == false) rName.MakeUpper();

				if((bMatchWholeWord && rName == FindName) || (!bMatchWholeWord && (rName.Find(FindName) >= 0)))
				{
					SetSelectedRange(actCell.row, actCell.col,actCell.row, actCell.col);
					SetFocusCell(actCell);
					//!SCROL BAR MOVE
					ScrollMove(startCell, actCell);

					return true;
				}
			}

L_SEARCH:
			actCell = bSearchDown ? GetNextCell(actCell) : GetPrevCell(actCell);
		}
		if(!IsValid(actCell))
		{
			bContinueSearch = false;
		}
	}while(bContinueSearch);

	return false;	// not found
}

/**
	@brief	find Next cell

	@author KHS	

	@date 2009-05-26 오후 2:41:29	

	@param	

	@return		
**/
CCellID CGridCtrlEx::GetNextCell(CCellID cell)
{
	const int nRowCount = GetRowCount();
	const int nColCount = GetColumnCount();

	if(cell.col < nColCount - 1)
	{
		cell.col += 1;
	}
	else
	{
		if(cell.row < nRowCount - 1)
		{
			cell.row += 1;
			cell.col = 1;
		}
		else
		{
			cell.col = -1;
			cell.row = -1;
		}
	}
	return cell;
}

/**
	@brief	find Prev cell

	@author KHS	

	@date 2009-05-26 오후 2:41:29	

	@param	

	@return		
**/
CCellID CGridCtrlEx::GetPrevCell(CCellID cell)
{
	const int nRowCount = GetRowCount();
	const int nColCount = GetColumnCount();

	if(cell.col > 1)
	{
		cell.col = cell.col - 1;
	}
	else
	{
		if(cell.row > 1)
		{
			cell.col = nColCount - 1;
			cell.row -= 1;
		}
		else
		{
			cell.col = -1;
			cell.row = -1;
		}
	}
	return cell;
}

/**
	@brief	F3 을 누르면 선택된 Cell과 같은 텍스트를 찾는다.

	@author KHS	

	@date 2009-05-26 오후 3:40:55	

	@param	

	@return		
**/
bool CGridCtrlEx::OnFindContinue(void)
{
	if(!m_rFindName.IsEmpty())
	{
		return FindWhatYouNeed(m_rFindName, m_bMatchCase, m_bMatchWholeWord, m_bSearchDown);
	}
	return false;
}

/**
	@brief	Move Scroll Bar Move to Focus Cell

	@author KHS	

	@date 2009-05-26 오후 5:33:50	

	@param	

	@return		
**/
int CGridCtrlEx::ScrollMove(CCellID startCell, CCellID actCell)
{
	if(IsValid(startCell) && IsValid(actCell))
	{
		int nSubCol = actCell.col - startCell.col;
		if(nSubCol > 0)
		{
			for(int i =0;i < nSubCol - 1;i++)
			{
				SendMessage(WM_HSCROLL, SB_LINERIGHT, 0); 
			}
		}
		else
		{
			for(int i =0;i < (-nSubCol) -1;i++)
			{
				SendMessage(WM_HSCROLL, SB_LINELEFT, 0); 
			}
		}
		int nSubRow = actCell.row - startCell.row;
		if(nSubRow > 0)
		{
			for(int i =0;i < nSubRow;i++)
			{
				SendMessage(WM_VSCROLL, SB_LINEDOWN, 0); 
			}
		}
		else
		{
			for(int i =0;i < -nSubRow;i++)
			{
				SendMessage(WM_VSCROLL, SB_LINEUP, 0); 
			}
		}

		return ERROR_SUCCESS;
	}
	
	return ERROR_BAD_ENVIRONMENT;
}