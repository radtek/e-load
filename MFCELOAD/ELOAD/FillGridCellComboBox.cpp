// FillGridCellComboBox.cpp: implementation of the CFillGridCellComboBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include <gui/gridctrl/GridCtrl.h>
#include "FillGridCellComboBox.h"
#include "ELoadItem.h"
#include "Tokenizer.h"

#include <map>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CFillGridCellComboBox, CGridCellEx)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CFillGridCellComboBox::CFillGridCellComboBox()
{
	CString rString = CGridCellEx::DATA_TYPE_STRING.c_str();
	if( IS_COMBOBOX(rString) )
	{
		m_rDataType = rString.Right(rString.GetLength() - 9);
		m_rDataType = m_rDataType.Left(m_rDataType.GetLength() - 1);
	}
	m_nRow = -1;
	m_nCol = -1;
}

CFillGridCellComboBox::~CFillGridCellComboBox()
{
}


/**
	@brief 

	@author	
*/
BOOL CFillGridCellComboBox::Edit(int nRow, int nCol, CRect rect, CPoint /* point */, UINT nID, UINT nChar)
{
	int i = 0;
	m_bEditing = TRUE;
	m_nRow = nRow; 
	m_nCol = nCol;
	if(NULL == m_pEditWnd)
	{
		int nHeight = rect.Height();
		rect.bottom = rect.bottom + 5*nHeight + ::GetSystemMetrics(SM_CYHSCROLL);
		CMyComboBox* pComboBox = new CMyComboBox(this);
		BOOL b = pComboBox->Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWN | CBS_HASSTRINGS | EM_SETREADONLY ,rect , (CWnd*)GetGrid() , 101);
		if(TRUE == b)
		{
			m_font.CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, HANGUL_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_MODERN, "Arial"); 
			pComboBox->SetFont(&m_font);
			vector<string> oResult;
			CTokenizer<CIsComma>::Tokenize(oResult , m_rDataType.operator LPCSTR() , CIsComma());
			for(vector<string>::iterator itr = oResult.begin();itr != oResult.end();++itr)
			{
				pComboBox->InsertString(i , itr->c_str());
			}
			CString rCellText = GetText();
			if(FALSE == rCellText.IsEmpty())
			{
				const int count = pComboBox->GetCount();
				for(int i = 0;i < count;i++)
				{
					CString rLBText;
					pComboBox->GetLBText(i , rLBText);
					if(rLBText == GetText())
					{
						pComboBox->SetCurSel(i);
						break;
					}
				}
			}
			else
			{
				CString rString;
				rString.Format("%c" , nChar);
				rString.MakeUpper();
				const int count = pComboBox->GetCount();
				for(int i = 0;i < count;i++)
				{
					CString rLBText;
					pComboBox->GetLBText(i , rLBText);
					int at = rString.Find(rLBText , 0);
					if(-1 != at )
					{
						pComboBox->SetCurSel(i);
						break;
					}
				}
			}
			pComboBox->GetWindowText(m_rValue);
		}
		m_pEditWnd = pComboBox;
		m_pEditWnd->SetFocus();
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

		m_pEditWnd->GetWindowText(m_rValue);
	}

	return TRUE;
}
/**	
	@brief	The CFieldTypeGridCell::EndEdit function
	@	
*/
void CFillGridCellComboBox::EndEdit(const CString& rComboEdit)
{
	if (m_pEditWnd && m_pEditWnd->IsWindowVisible())
	{
		CMyComboBox* pComboBox = ((CMyComboBox*)m_pEditWnd);

		CString rText;
		const int nIndex = pComboBox->GetCurSel();
		if(-1 != nIndex )
		{
			pComboBox->GetLBText(nIndex , rText);
			///pComboBox->GetWindowText(rText);
			SetText(rText);

			m_pEditWnd->ShowWindow(SW_HIDE);

			///if(m_rValue != rText)
			{
				// Send Notification to parent
				GV_DISPINFO dispinfo;
				dispinfo.hdr.hwndFrom = GetGrid()->GetSafeHwnd();
				dispinfo.hdr.idFrom   = GetGrid()->GetDlgCtrlID();

				rComboEdit.IsEmpty() ? (dispinfo.hdr.code    = GVN_ENDLABELEDIT) : (dispinfo.hdr.code = GVN_CB_SELCHANGE);

				dispinfo.item.mask    = LVIF_TEXT|LVIF_PARAM;
				dispinfo.item.row     = m_nRow;
				dispinfo.item.col     = m_nCol;
				dispinfo.item.strText  = rText;
				dispinfo.item.lParam  = (LPARAM)0;

				CWnd* pOwner = GetGrid()->GetParent();
				if (pOwner)
					pOwner->SendMessage(WM_NOTIFY, GetGrid()->GetDlgCtrlID(), (LPARAM)&dispinfo );
				m_rValue = rText;
			}
		}
		else
		{
			m_pEditWnd->ShowWindow(SW_HIDE);
			// Send Notification to parent
			GV_DISPINFO dispinfo;
			dispinfo.hdr.hwndFrom = GetGrid()->GetSafeHwnd();
			dispinfo.hdr.idFrom   = GetGrid()->GetDlgCtrlID();
			dispinfo.hdr.code     = GVN_ENDLABELEDIT;
			dispinfo.item.mask    = LVIF_TEXT|LVIF_PARAM;
			dispinfo.item.row     = m_nRow;
			dispinfo.item.col     = m_nCol;
			dispinfo.item.strText  = "";
			dispinfo.item.lParam  = (LPARAM)0;

			CWnd* pOwner = GetGrid()->GetParent();
			if (pOwner)
				pOwner->SendMessage(WM_NOTIFY, GetGrid()->GetDlgCtrlID(), (LPARAM)&dispinfo );
			m_rValue = "";
		}

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
			if(m_nRow + 1 < m_pGrid->GetColumnCount())
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


	m_bEditing = FALSE;
}

// CMyComboBox
IMPLEMENT_DYNAMIC(CMyComboBox, CComboBox)

CMyComboBox::CMyComboBox(CFillGridCellComboBox* pCell) : m_pCell(pCell)
{
}

CMyComboBox::~CMyComboBox()
{
}

BEGIN_MESSAGE_MAP(CMyComboBox, CComboBox)
        ON_CONTROL_REFLECT(CBN_SELCHANGE, &CMyComboBox::OnCbnSelchange)
END_MESSAGE_MAP()

// CMyComboBox message handlers
void CMyComboBox::OnCbnSelchange()
{
	// TODO: Add your control notification handler code here
	m_pCell->EndEdit("COMBOEDIT");
}
