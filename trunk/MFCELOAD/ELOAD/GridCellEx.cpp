// GridCellBaseEx.cpp : implementation file
//

#include "stdafx.h"
#include "ELoad.h"
#include "ELoadDocData.h"
#include "GridCellEx.h"

string CGridCellEx::DATA_TYPE_STRING;
// CGridCellEx

IMPLEMENT_DYNCREATE(CGridCellEx, CGridCell)

CGridCellEx::CGridCellEx() : m_pFont(NULL) , m_nRow(-1) , m_nCol(-1)
, m_bEndEdit(false)
{
	//! copy data type
	m_rDataType = CGridCellEx::DATA_TYPE_STRING.c_str();
	m_rUnit.Empty();
}

CGridCellEx::~CGridCellEx()
{
	try
	{
	}
	catch(...)
	{
	}
}

/**
        @brief 

	@author	
*/
BOOL CGridCellEx::Edit(int nRow, int nCol, CRect rect, CPoint /* point */, UINT nID, UINT nChar)
{
        m_bEditing = TRUE;
        m_nRow = nRow;
        m_nCol = nCol;
        
	if(NULL == m_pEditWnd)
        {
		const DWORD dwStyle = WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL;

		CInPlaceBoostBaseFilterEdit* pWndEdit = new CInPlaceBoostBaseFilterEdit;
                if(NULL != pWndEdit)
                {
			if((m_rDataType.GetLength() > 6) && ("REGEXP" == m_rDataType.Left(6)))
			{
				CString rRegExpKey = m_rDataType.Right(m_rDataType.GetLength() - 7);
				rRegExpKey = rRegExpKey.Left(rRegExpKey.GetLength() - 1);
				
				CELoadDocData& docData = CELoadDocData::GetInstance();
				const string rRegExp = docData.GetRegExp(rRegExpKey.operator LPCSTR());
				(!rRegExp.empty()) ? pWndEdit->SetRegEx(rRegExp.c_str()) : pWndEdit->SetRegEx(TEXT_REGEXP);

				pWndEdit->Create(GetGrid() , rect , dwStyle , nID , nRow , nCol , GetGrid()->GetCell(nRow , nCol)->GetText() , nChar);
				pWndEdit->CreateToolTip(GetGrid() , _T("데이터 형식이 일치하지 않습니다."));
				pWndEdit->SetBackgroundColourFocus (RGB (150, 200, 255));
				pWndEdit->SetForegroundColourError (RGB (255, 255, 255));
				pWndEdit->SetBackgroundColourError (RGB (255, 0, 0));

				const CString rCellText = GetGrid()->GetCell(nRow,nCol)->GetText();
			}
			else
			{
				pWndEdit->SetRegEx(TEXT_REGEXP);

				pWndEdit->Create(GetGrid() , rect , dwStyle , nID , nRow , nCol , GetGrid()->GetCell(nRow , nCol)->GetText() , nChar);
				pWndEdit->CreateToolTip(GetGrid() , _T("데이터 형식이 일치하지 않습니다."));
				pWndEdit->SetBackgroundColourFocus (RGB (150, 200, 255));
				pWndEdit->SetForegroundColourError (RGB (255, 255, 255));
				pWndEdit->SetBackgroundColourError (RGB (255, 0, 0));

			}
			m_pEditWnd = pWndEdit;
			m_pEditWnd->SetFocus();
                }
        }
        else
        {
                CRect wndRect;
                m_pEditWnd->GetWindowRect(&wndRect);
                int height = wndRect.bottom - wndRect.top;
                rect.bottom = rect.top + height;
                m_pEditWnd->MoveWindow(rect);
                m_pEditWnd->ShowWindow(SW_NORMAL);
                m_pEditWnd->SetFocus();

                ///m_pEditWnd->GetWindowText(m_rValue);
        }

        return TRUE;
}

/**
	@brief	

	@author	BHK
*/
void CGridCellEx::OnEndEdit()
{
	CGridCell::OnEndEdit();

	m_bEndEdit = true;

	if((-1 != m_nRow) && (-1 != m_nCol))
	{
		CCellID cell = m_pGrid->GetFocusCell();
		const int nOldWidth = m_pGrid->GetColumnWidth(cell.col);
		m_pGrid->AutoSizeColumn(cell.col);
		if( nOldWidth > m_pGrid->GetColumnWidth(cell.col))
		{
			m_pGrid->SetColumnWidth(cell.col, nOldWidth);
		}
		m_pGrid->RedrawWindow();

		//! Row가 마지막이 아닐때만 포커스 아래로 이동
                if(m_nRow + 1 < m_pGrid->GetColumnCount() && (GetKeyState(VK_DOWN) == 0 || GetKeyState(VK_DOWN) == 1) && (GetKeyState(VK_LEFT) == 0 || GetKeyState(VK_LEFT) == 1)&& (GetKeyState(VK_RIGHT) == 0 || GetKeyState(VK_RIGHT) == 1)&& (GetKeyState(VK_UP) == 0 || GetKeyState(VK_UP) == 1))
		{
			CCellRange range;
			range.Set(m_nRow + 1 , m_nCol , m_nRow + 1 , m_nCol);
			m_pGrid->SetSelectedRange(range);
			m_pGrid->SetFocusCell(m_nRow + 1, m_nCol);
		}
	}

	m_nRow = -1;
	m_nCol = -1;
}

/**
	@brief	값이 변경되었을때 색상을 붉게 표기한다.

	@author	BHK
*/
COLORREF CGridCellEx::GetTextClr() const
{
	CItemPropCategory::ItemPropTag* pTag = (CItemPropCategory::ItemPropTag*)GetData();
	if(pTag && pTag->IsModified()) return MODIFIED_COLOR;

	return RGB(0,0,0);
}

/**
	@brief	값이 변경되었을때 BOLD FONT를 리턴한다.

	@author	BHK
*/
CFont* CGridCellEx::GetFontObject() const
{
	static CFont BOLDFONT;
	static BOOL FIRST = TRUE;

	if(TRUE == FIRST)
	{
		CGridDefaultCell *pDefaultCell = (CGridDefaultCell*) GetDefaultCell();
		if(NULL != pDefaultCell)
		{
			LOGFONT* pLogFont = pDefaultCell->GetFont();
			if(pLogFont)
			{
				LOGFONT LogFont = *pLogFont;
				LogFont.lfWeight = FW_BOLD;
				BOLDFONT.CreateFontIndirect(&LogFont);
			}
		}
		FIRST = FALSE;
	}

	CItemPropCategory::ItemPropTag* pTag = (CItemPropCategory::ItemPropTag*)GetData();
	if(pTag && pTag->IsModified()) return &(BOLDFONT);

	return CGridCell::GetFontObject(); 
}

int CGridCellEx::OnDrawValue(CDC* pDC, CRect rect)
{
	if(!m_rUnit.IsEmpty())
	{
		DrawText(pDC->m_hDC, CString(GetText() + CString(_T(" ")) + m_rUnit).operator LPCSTR() , -1, rect, GetFormat() | DT_NOPREFIX);
	}
	else	return CGridCell::OnDrawValue(pDC , rect);

	return ERROR_SUCCESS;
}