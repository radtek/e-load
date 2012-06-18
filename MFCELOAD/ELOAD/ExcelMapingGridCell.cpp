// ExcelMapingGridCell.cpp: implementation of the CExcelMapingGridCell class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include <gui/gridctrl/GridCtrl.h>
#include "ExcelMapingGridCell.h"
#include <gui/FilterEdit/UIntEdit.h>
#include <gui/FilterEdit/BoostBaseFilterEdit.h>

#include <map>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CExcelMapingGridCell, CGridCell)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExcelMapingGridCell::CExcelMapingGridCell()
{

}

CExcelMapingGridCell::~CExcelMapingGridCell()
{
}

/**
        @brief  

        @author KHS
*/
BOOL CExcelMapingGridCell::Edit(int nRow, int nCol, CRect rect, CPoint /* point */, UINT nID, UINT nChar)
{
        int i=0;
        m_bEditing = TRUE;
        if(NULL == m_pEditWnd)
        {
                if(1 == nChar)
                {
                        int nHeight = rect.Height();
                        rect.bottom = rect.bottom + 5*nHeight + ::GetSystemMetrics(SM_CYHSCROLL);

                        CMyComboBoxEx* pComboBox = new CMyComboBoxEx(this);
                        BOOL b = pComboBox->Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_HASSTRINGS ,rect , (CWnd*)GetGrid() , 101);
                        if(TRUE == b)
                        {
                                //m_font.CreateFont(-11,0,0,0,400,FALSE,FALSE,FALSE,ANSI_CHARSET,3,2,1,VARIABLE_PITCH | FF_MODERN,"굴림"); 
                                //pComboBox->SetFont(&m_font);
                                pComboBox->SetFont(pComboBox->GetFont());

                                static char *pMap[] = { "A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","W","X","Y","Z"};
                                int count = sizeof(pMap) / sizeof(char*);
                                for(i=0;i < count;i++)
                                {
                                        pComboBox->InsertString(i , pMap[i]);
                                }

                                for(i=0;i < count;i++)
                                {
                                        pComboBox->InsertString(i + count, CString(pMap[0]) + CString(pMap[i]));
                                }

                                CString rCellText = GetText();
                                if(FALSE == rCellText.IsEmpty())
                                {
                                        count = pComboBox->GetCount();
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

                                        count = pComboBox->GetCount();
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
                        }

                        m_pEditWnd = pComboBox;
                        pComboBox = NULL;
                        m_pEditWnd->SetFocus();
                }
                
                else
                {
                        DWORD dwStyle = WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_CENTER;
                        CBoostBaseFilterEdit* pWndEdit = new CBoostBaseFilterEdit;
                        if(pWndEdit)
                        {
                                pWndEdit->Create(dwStyle, rect, GetGrid(), nID);
                                CString rRegEx="([-_+/,])*";
                                pWndEdit->SetRegEx(rRegEx);
                                pWndEdit->CreateToolTip(GetGrid() , "사용할 수 없는 글자입니다.");
                                m_pEditWnd = pWndEdit;
                                
                                CString rString;
                                rString.Format("%c" , nChar);
                                m_pEditWnd->SetWindowText(rString);

                                m_pEditWnd->SetFocus();
                        }
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
        }

        return TRUE;
}

/**	
	@brief	The CFieldTypeGridCell::EndEdit function


	@return	void	
*/

void CExcelMapingGridCell::EndEdit()
{
        if (m_pEditWnd && m_pEditWnd->IsWindowVisible())
        {
                if(m_pEditWnd->IsKindOf(RUNTIME_CLASS(CMyComboBoxEx)))
                {
                        CMyComboBoxEx* pComboBox = ((CMyComboBoxEx*)m_pEditWnd);
                        CString rText;
                        const int nIndex = pComboBox->GetCurSel();
                        if(-1 != nIndex )
                        {
                                pComboBox->GetLBText(nIndex , rText);
                                SetText(rText);

                                m_pEditWnd->ShowWindow(SW_HIDE);
                                m_pEditWnd = NULL;
                        }
                        else
                        {
                                SetText(_T(""));
                                m_pEditWnd->ShowWindow(SW_HIDE);
                                m_pEditWnd = NULL;
                        }
                }
                else if(m_pEditWnd->IsKindOf(RUNTIME_CLASS(CBoostBaseFilterEdit)))
                {
                        CBoostBaseFilterEdit* pWndFilterEdit = (CBoostBaseFilterEdit*)m_pEditWnd;
                        if(pWndFilterEdit)
                        {
                                CString rText;
                                pWndFilterEdit->GetWindowText(rText);
                                SetText(rText);
                                m_pEditWnd->ShowWindow(SW_HIDE);
                                m_pEditWnd= NULL;
                        }
                }
        }

	m_bEditing = FALSE;
}
// CMyComboBox
IMPLEMENT_DYNAMIC(CMyComboBoxEx, CComboBox)

CMyComboBoxEx::CMyComboBoxEx(CExcelMapingGridCell* pCell) : m_pCell(pCell)
{
}

CMyComboBoxEx::~CMyComboBoxEx()
{
}

BEGIN_MESSAGE_MAP(CMyComboBoxEx, CComboBox)
        ON_CONTROL_REFLECT(CBN_SELCHANGE, &CMyComboBoxEx::OnCbnSelchange)
        ON_CONTROL_REFLECT(CBN_KILLFOCUS, &CMyComboBoxEx::OnCbnKillfocus)
END_MESSAGE_MAP()

// CMyComboBox message handlers
void CMyComboBoxEx::OnCbnSelchange()
{
        // TODO: Add your control notification handler code here
        m_pCell->EndEdit();
}

void CMyComboBoxEx::OnCbnKillfocus()
{
        // TODO: Add your control notification handler code here
        m_pCell->EndEdit();
}
