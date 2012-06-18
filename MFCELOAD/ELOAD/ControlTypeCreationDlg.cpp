// ControlTypeCreationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "ControlTypeCreationDlg.h"
#include "ELoadDocData.h"
#include <util/ado/ADODB.h>
#include "FillGridCellComboBox.h"
#include "GridCellEx.h"
#include "Tables/ControlTypeTable.h"
#include <gui/GridCtrl/GridCellCheck.h>
#include <gui/GridCtrl/GridCellCombo.h>

static const string __CATEGORIES__[] = {"LCS or LOCAL TYPE" , "LCP IO TYPE" , "DCS IO TYPE"};
/*
struct MatchType : public binary_function<CELOADTable::Record, string, bool>
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
*/
// CControlTypeCreationDlg dialog

IMPLEMENT_DYNAMIC(CControlTypeCreationDlg, CWizardPropertyPage)

CControlTypeCreationDlg::CControlTypeCreationDlg(const string& rInitCategory , CWnd* pParent /*=NULL*/)
	: CWizardPropertyPage(CControlTypeCreationDlg::IDD/*, pParent*/) , m_rCurCategory(rInitCategory)
{
	m_nPageIndex = -1;
	m_psp.dwFlags |= PSP_DEFAULT|PSP_HIDEHEADER;
}

CControlTypeCreationDlg::~CControlTypeCreationDlg()
{
}

void CControlTypeCreationDlg::DoDataExchange(CDataExchange* pDX)
{
	CWizardPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDCANCEL, m_wndCancelButton);
	DDX_Control(pDX, IDOK, m_wndOkButton);

	DDX_Control(pDX, ID_WIZBACK, m_wndBackButton);
	DDX_Control(pDX, ID_WIZNEXT, m_wndNextButton);

	DDX_Control(pDX, IDC_BUTTON_ADD, m_wndAddButton);
	DDX_Control(pDX, IDC_BUTTON_DELETE, m_wndDeleteButton);
}


BEGIN_MESSAGE_MAP(CControlTypeCreationDlg, CWizardPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CControlTypeCreationDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CControlTypeCreationDlg::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDOK, &CControlTypeCreationDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_LCS_BUTTON, &CControlTypeCreationDlg::OnBnClickedLcsButton)
	ON_BN_CLICKED(IDC_LCP_BUTTON, &CControlTypeCreationDlg::OnBnClickedLcpButton)
	ON_BN_CLICKED(IDC_DCS_BUTTON, &CControlTypeCreationDlg::OnBnClickedDcsButton)
	ON_BN_CLICKED(ID_WIZBACK, &CControlTypeCreationDlg::OnBnClickedWizback)
	ON_BN_CLICKED(ID_WIZNEXT, &CControlTypeCreationDlg::OnBnClickedWiznext)
	ON_BN_CLICKED(IDCANCEL, &CControlTypeCreationDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CControlTypeCreationDlg message handlers

BOOL CControlTypeCreationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	///m_rCurCategory = __CATEGORIES__[0];

	m_wndLCSTypeButton.Attach(IDC_LCS_BUTTON, this);
	m_wndLCPTypeButton.Attach(IDC_LCP_BUTTON, this);
	m_wndDCSTypeButton.Attach(IDC_DCS_BUTTON, this);

	m_wndLCSTypeButton.SetColor(RGB(0,0,0), RGB(196,222,241));
	m_wndLCPTypeButton.SetColor(RGB(0,0,0), RGB(255,255,255));
	m_wndDCSTypeButton.SetColor(RGB(0,0,0), RGB(255,255,255));

	CreateTabCtrl();
	DisplayData();

	//! BUTTON ICON SETTING
	CELoadDocData & docData = CELoadDocData::GetInstance();
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	m_wndOkButton.SetTextHotColor(TextHotColor);
	m_wndCancelButton.SetTextHotColor(TextHotColor);
	m_wndAddButton.SetTextHotColor(TextHotColor);
	m_wndDeleteButton.SetTextHotColor(TextHotColor);
	m_wndDeleteButton.SetIcon(IDI_DEL);
	m_wndAddButton.SetIcon(IDI_ADD);

	m_wndBackButton.SetTextHotColor(TextHotColor);
	m_wndNextButton.SetTextHotColor(TextHotColor);
	m_wndBackButton.SetIcon(IDI_PRE);
	m_wndNextButton.SetIcon(IDI_NEXT);

	m_wndOkButton.SetIcon(IDI_OK_18X18);
	m_wndCancelButton.SetIcon(IDI_CANCEL_18X18);

	return Init();  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	CREATE TAB CONTROL

	@author KHS	

	@return		
**/
int CControlTypeCreationDlg::CreateTabCtrl(void)
{
	if(ERROR_SUCCESS == CreateLscOrLocalTypeGridCtrl())
	{
	}
	if(ERROR_SUCCESS == CreateLcpIoTypeGridCtrl())
	{
	}
	if(ERROR_SUCCESS == CreateDcsIoTypeGridCtrl())
	{
	}
	
	if(_T("LCS or LOCAL TYPE") == m_rCurCategory)
	{
		OnBnClickedLcsButton();
		m_wndLCSTypeButton.SetFocus();

		m_wndLscOrLocalTypeGridCtrl.ShowWindow(TRUE);
		m_wndLcpIoTypeGridCtrl.ShowWindow(FALSE);
		m_wndDcsIoTypeGridCtrl.ShowWindow(FALSE);
	}
	else
	{
		OnBnClickedLcpButton();
		m_wndLCPTypeButton.SetFocus();

		m_wndLscOrLocalTypeGridCtrl.ShowWindow(FALSE);
		m_wndLcpIoTypeGridCtrl.ShowWindow(TRUE);
		m_wndDcsIoTypeGridCtrl.ShowWindow(FALSE);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	Create Lsc Or Local Type GridCtrl

	@author KHS	

	@return		
**/
int CControlTypeCreationDlg::CreateLscOrLocalTypeGridCtrl(void)
{
	static const CString __HEADERS__[] = {_T(" ") , _T("LCS TYPE") , _T("LCS DESC")};

	CRect rect;
	GetDlgItem(IDC_GRID_STATIC)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	if(m_wndLscOrLocalTypeGridCtrl.Create(rect , this , 100))
	{
		m_wndLscOrLocalTypeGridCtrl.SetColumnCount(sizeof(__HEADERS__) / sizeof(__HEADERS__[0]));
		m_wndLscOrLocalTypeGridCtrl.SetFixedRowCount(1);

		m_wndLscOrLocalTypeGridCtrl.SetEditable(TRUE);
		m_wndLscOrLocalTypeGridCtrl.SetRowResize(FALSE);
		m_wndLscOrLocalTypeGridCtrl.SetColumnResize(TRUE);
		
		// 해더
		const int nColumnCount = m_wndLscOrLocalTypeGridCtrl.GetColumnCount();
		for(int i = 0;i < nColumnCount;++i)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = 0;
			Item.col = i;
			Item.nFormat = DT_LEFT|DT_WORDBREAK | DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX;
			Item.strText = __HEADERS__[i];
			m_wndLscOrLocalTypeGridCtrl.SetItem(&Item);
		}

		m_wndLscOrLocalTypeGridCtrl.AutoSizeColumns();
		m_wndLscOrLocalTypeGridCtrl.SetColumnWidth(0 , 28);
		m_wndLscOrLocalTypeGridCtrl.SetColumnWidth(1 , 150);
		m_wndLscOrLocalTypeGridCtrl.SetColumnWidth(2 , 200);
		m_wndLscOrLocalTypeGridCtrl.AutoSizeRows();

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	Create Lcp Io Typ eGridCtrl

	@author KHS	

	@return		
**/
int CControlTypeCreationDlg::CreateLcpIoTypeGridCtrl(void)
{
	static const CString __HEADERS__[] = {_T(" ") , _T("LCP IO TYPE") , _T("IO TYPE") , _T("LCP IO DESC")};

	/*CRect rect;
	rect.SetRectEmpty();*/
	CRect rect;
	GetDlgItem(IDC_GRID_STATIC)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	if(m_wndLcpIoTypeGridCtrl.Create(rect , this , 101))
	{
		m_wndLcpIoTypeGridCtrl.SetColumnCount(sizeof(__HEADERS__) / sizeof(__HEADERS__[0]));
		m_wndLcpIoTypeGridCtrl.SetFixedRowCount(1);

		m_wndLcpIoTypeGridCtrl.SetEditable(TRUE);
		m_wndLcpIoTypeGridCtrl.SetRowResize(FALSE);
		m_wndLcpIoTypeGridCtrl.SetColumnResize(TRUE);
		
		// 해더
		const int nColumnCount = m_wndLcpIoTypeGridCtrl.GetColumnCount();
		for(int i = 0;i < nColumnCount;++i)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = 0;
			Item.col = i;
			Item.nFormat = DT_LEFT|DT_WORDBREAK | DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX;
			Item.strText = __HEADERS__[i];
			m_wndLcpIoTypeGridCtrl.SetItem(&Item);
		}

		m_wndLcpIoTypeGridCtrl.AutoSizeColumns();
		m_wndLcpIoTypeGridCtrl.SetColumnWidth(0 , 28);
		m_wndLcpIoTypeGridCtrl.SetColumnWidth(1 , 150);
		m_wndLcpIoTypeGridCtrl.SetColumnWidth(3 , 150);
		m_wndLcpIoTypeGridCtrl.AutoSizeRows();

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	Create Dcs Io Type GridCtrl

	@author KHS	

	@return		
**/
int CControlTypeCreationDlg::CreateDcsIoTypeGridCtrl(void)
{
	static const CString __HEADERS__[] = {_T(" " ) , _T("DCS IO TYPE") , _T("IO TYPE"), _T("DCS IO DESC"), _T("TAG NAMING RULE"),_T(" ")};

	CRect rect;
	GetDlgItem(IDC_GRID_STATIC)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	if(m_wndDcsIoTypeGridCtrl.Create(rect , this , 102))
	{
		m_wndDcsIoTypeGridCtrl.SetColumnCount(sizeof(__HEADERS__) / sizeof(__HEADERS__[0]));
		m_wndDcsIoTypeGridCtrl.SetFixedRowCount(1);

		m_wndDcsIoTypeGridCtrl.SetEditable(TRUE);
		m_wndDcsIoTypeGridCtrl.SetRowResize(FALSE);
		m_wndDcsIoTypeGridCtrl.SetColumnResize(TRUE);
		
		// 해더
		const int nColumnCount = m_wndDcsIoTypeGridCtrl.GetColumnCount();
		for(int i = 0;i < nColumnCount;++i)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = 0;
			Item.col = i;
			Item.nFormat = DT_LEFT|DT_WORDBREAK | DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX;
			Item.strText = __HEADERS__[i];
			m_wndDcsIoTypeGridCtrl.SetItem(&Item);

		}
		
		m_wndDcsIoTypeGridCtrl.AutoSizeColumns();
		m_wndDcsIoTypeGridCtrl.SetColumnWidth(0 , 28);
		m_wndDcsIoTypeGridCtrl.SetColumnWidth(1 , 150);
		m_wndDcsIoTypeGridCtrl.SetColumnWidth(3 , 150);
		m_wndDcsIoTypeGridCtrl.SetColumnWidth(4 , 80);
		m_wndDcsIoTypeGridCtrl.SetColumnWidth(5 , 80);
		m_wndDcsIoTypeGridCtrl.AutoSizeRows();
		
		m_wndDcsIoTypeGridCtrl.MergeCells(0, 4,0, 5);

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}


/**
	@brief	GRID CTRL에 한줄 아이템 추가

	@author KHS	

	@return		
**/
void CControlTypeCreationDlg::OnControltypecreationInsert()
{
	if(m_wndLscOrLocalTypeGridCtrl.IsWindowVisible())
	{	
		InsertItemInLscOrLocalTypeGridCtrl();
		m_wndLscOrLocalTypeGridCtrl.RedrawWindow();
	}
	else if(m_wndLcpIoTypeGridCtrl.IsWindowVisible())
	{
		InsertItemInLcpIoTypeGridCtrl();
		m_wndLcpIoTypeGridCtrl.RedrawWindow();
	}
	else if(m_wndDcsIoTypeGridCtrl.IsWindowVisible())
	{
		InsertItemDcsIoTypeGridCtrl();
		m_wndDcsIoTypeGridCtrl.RedrawWindow();
	}
}

/**
	@brief	GRID CTRL에서 한줄 아이템 삭제

	@author KHS	

	@return		
**/
void CControlTypeCreationDlg::OnControltypecreationDelete()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CGridCtrl* pGridCtrl = GetGridCtrlOf(m_rCurCategory);
	if(pGridCtrl)
	{
		pGridCtrl->SetFocus();
		CCellRange range = pGridCtrl->GetSelectedCellRange();

		CControlTypeTable* pTable = static_cast<CControlTypeTable*>(docData.GetTableOf(_T("T_CONTROL_TYPE")));	//! database내용을 다 읽어온 상태...
		if(pTable)
		{
			for(int i = range.GetMaxRow();i >= range.GetMinRow();--i)
			{
				const string rType = pGridCtrl->GetItemText(i, 1);
				vector<CELOADTable::Record>::iterator where = find_if(pTable->m_RecordEntry.begin(),pTable->m_RecordEntry.end(), bind2nd(MatchType(), rType));
				if(where != pTable->m_RecordEntry.end())
				{
					pTable->m_RecordEntry.erase(where);
				}


				pGridCtrl->DeleteRow(i);
			}
		}
		pGridCtrl->RedrawWindow();
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-04-20 오후 7:48:02	

	@param	

	@return		
*/
int CControlTypeCreationDlg::InsertItemIntoGridCtrl(CGridCtrl* pGridCtrl , CELOADTable::Record& record)
{
	assert(pGridCtrl && "pGridCtrl is NULL");

	if(pGridCtrl)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		const COLORREF ReadOnlyColor = docData.GetColorCode("READ_ONLY_BACK_COLOR");

		const string rCategory = record.FieldValueMap[_T("C_CATEGORY")];
		const string rCheck = record.FieldValueMap[_T("C_CHECK")];
		const CString rType    = record.FieldValueMap[_T("C_TYPE")].c_str();
		const CString rDesc    = record.FieldValueMap[_T("C_DESC")].c_str();
		const CString rIOType  = record.FieldValueMap[_T("C_IO_TYPE")].c_str();
		const CString rNamingRulePrefix  = record.FieldValueMap[_T("C_NAMING_RULE_PREFIX")].c_str();

		pGridCtrl->SetRowCount(pGridCtrl->GetRowCount() + 1);
		const int nRow = pGridCtrl->GetRowCount() - 1;
		const int nColCount = pGridCtrl->GetColumnCount();
		
		for(int i = 0;i < nColCount;++i)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = nRow;
			Item.col = i;
			Item.nFormat = DT_LEFT|DT_WORDBREAK | DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX;
			if(0 == i)
			{
				pGridCtrl->SetCellType(Item.row, Item.col , RUNTIME_CLASS(CGridCellCheck));
				CGridCellCheck* pCellCheck = (CGridCellCheck*)pGridCtrl->GetCell(Item.row , Item.col);
				if(pCellCheck) pCellCheck->SetCheck(_T("YES") == rCheck);
			}
			else if(1 == i)
			{
				Item.strText = rType;
				pGridCtrl->SetCellType(Item.row, Item.col , RUNTIME_CLASS(CGridCellEx));
			}
			else if(2 == i)
			{
				if(__CATEGORIES__[0] == rCategory)
				{
					Item.strText = rDesc;
					pGridCtrl->SetCellType(Item.row, Item.col , RUNTIME_CLASS(CGridCellEx));
				}
				else if((__CATEGORIES__[1] == rCategory) || (__CATEGORIES__[2] == rCategory))
				{
					Item.strText = rIOType;
					
					pGridCtrl->SetCellType(Item.row, Item.col , RUNTIME_CLASS(CGridCellCombo));
					CGridCellCombo* pCellCombo = (CGridCellCombo*)pGridCtrl->GetCell(Item.row , Item.col);
					if(pCellCombo)
					{
						CStringArray ar;
						ar.Add(_T("AI"));
						ar.Add(_T("DI"));
						ar.Add(_T("DO"));
						
						pCellCombo->SetOptions(ar);
					}
				}
			}
			else if(3 == i)
			{
				Item.strText = rDesc;
				pGridCtrl->SetCellType(Item.row, Item.col , RUNTIME_CLASS(CGridCellEx));
			}
			else if(4 == i)
			{
				Item.strText = rNamingRulePrefix;
				pGridCtrl->SetCellType(Item.row, Item.col , RUNTIME_CLASS(CGridCellEx));
			}
			else if(5 == Item.col)
			{
				Item.nFormat = DT_CENTER|DT_WORDBREAK | DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX;
				Item.strText = _T("LOAD ID");
				pGridCtrl->SetCellType(Item.row, Item.col , RUNTIME_CLASS(CGridCellEx));

				CGridCellBase* pCell = m_wndDcsIoTypeGridCtrl.GetCell(Item.row , Item.col);
				if(pCell)
				{
					pCell->SetState(pCell->GetState() | GVIS_READONLY);
					pCell->SetBackClr(ReadOnlyColor);
				}
			}

			pGridCtrl->SetItem(&Item);
		}
		pGridCtrl->SetRowHeight(pGridCtrl->GetRowCount() - 1, pGridCtrl->GetRowHeight(0));

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	Lsc Or Local Type Grid에 아이템을 추가한다.

	@author KHS		

	@return		
**/
int CControlTypeCreationDlg::InsertItemInLscOrLocalTypeGridCtrl(const CString& rLscType, const CString& rLscDesc)
{
	m_wndLscOrLocalTypeGridCtrl.SetRowCount(m_wndLscOrLocalTypeGridCtrl.GetRowCount() + 1);
	const int nRow = m_wndLscOrLocalTypeGridCtrl.GetRowCount() - 1;
	const int nColCount = m_wndLscOrLocalTypeGridCtrl.GetColumnCount();
	
	for(int i = 0;i < nColCount;++i)
	{
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.row = nRow;
		Item.col = i;
		Item.nFormat = DT_LEFT|DT_WORDBREAK | DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX;
		if(0 == i)
		{
			m_wndLscOrLocalTypeGridCtrl.SetCellType(Item.row, Item.col , RUNTIME_CLASS(CGridCellCheck));
		}
		else if(1 == i)
		{
			Item.strText = rLscType;
			m_wndLscOrLocalTypeGridCtrl.SetCellType(Item.row, Item.col , RUNTIME_CLASS(CGridCellEx));
		}
		else
		{
			Item.strText = rLscDesc;
			m_wndLscOrLocalTypeGridCtrl.SetCellType(Item.row, Item.col , RUNTIME_CLASS(CGridCellEx));
		}

		m_wndLscOrLocalTypeGridCtrl.SetItem(&Item);
		
	}
	m_wndLscOrLocalTypeGridCtrl.SetRowHeight(m_wndLscOrLocalTypeGridCtrl.GetRowCount() - 1, m_wndLscOrLocalTypeGridCtrl.GetRowHeight(0));

	return ERROR_SUCCESS;
}

/**
	@brief	Lcp Io Type Grid에 아이템을 추가한다.

	@author KHS		

	@return		
**/
int CControlTypeCreationDlg::InsertItemInLcpIoTypeGridCtrl(const CString& rLcpIoType, const CString& rLcpIoDesc)
{
	m_wndLcpIoTypeGridCtrl.SetRowCount(m_wndLcpIoTypeGridCtrl.GetRowCount() + 1);
	const int nRow = m_wndLcpIoTypeGridCtrl.GetRowCount() - 1;
	const int nColCount = m_wndLcpIoTypeGridCtrl.GetColumnCount();
	
	for(int i = 0;i < nColCount;++i)
	{
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.row = nRow;
		Item.col = i;
		Item.nFormat = DT_LEFT|DT_WORDBREAK | DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX;
		if(0 == i)
		{
			m_wndLcpIoTypeGridCtrl.SetCellType(Item.row, Item.col , RUNTIME_CLASS(CGridCellCheck));
		}
		else if(1 == i)
		{
			Item.strText = rLcpIoType;
			m_wndLcpIoTypeGridCtrl.SetCellType(Item.row, Item.col , RUNTIME_CLASS(CGridCellEx));
		}
		else
		{
			Item.strText = rLcpIoDesc;
			m_wndLcpIoTypeGridCtrl.SetCellType(Item.row, Item.col , RUNTIME_CLASS(CGridCellEx));
		}

		m_wndLcpIoTypeGridCtrl.SetItem(&Item);
	}
	m_wndLcpIoTypeGridCtrl.SetRowHeight(m_wndLcpIoTypeGridCtrl.GetRowCount() - 1, m_wndLcpIoTypeGridCtrl.GetRowHeight(0));

	return ERROR_SUCCESS;
}

/**
	@brief	Dsc Io Type Grid에 아이템을 추가한다.

	@author KHS		

	@return		
**/
int CControlTypeCreationDlg::InsertItemDcsIoTypeGridCtrl(const CString& rDcsIoType, const CString& rIoType, const CString& rDcsIoDesc, const CString& rPrefix, const CString& rLoadId)
{
	CELoadDocData &docData = CELoadDocData::GetInstance();
	const COLORREF ReadOnlyColor = docData.GetColorCode("READ_ONLY_BACK_COLOR");

	m_wndDcsIoTypeGridCtrl.SetRowCount(m_wndDcsIoTypeGridCtrl.GetRowCount() + 1);
	const int nRow  = m_wndDcsIoTypeGridCtrl.GetRowCount()- 1;
	const int nColCount = m_wndDcsIoTypeGridCtrl.GetColumnCount();

	for(int i = 0;i < nColCount;++i)
	{
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.row = nRow;
		Item.col = i;
		Item.nFormat = DT_LEFT|DT_WORDBREAK | DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX;
		if(0 == i)
		{
			m_wndDcsIoTypeGridCtrl.SetCellType(Item.row, Item.col , RUNTIME_CLASS(CGridCellCheck));
		}
		if(1 == i)
		{
			Item.strText = rDcsIoType;
			m_wndDcsIoTypeGridCtrl.SetCellType(Item.row, Item.col , RUNTIME_CLASS(CGridCellEx));
		}
		else if(2 == i)
		{
			Item.strText = rIoType;
			m_wndDcsIoTypeGridCtrl.SetCellType(Item.row, Item.col , RUNTIME_CLASS(CGridCellEx));
		}
		else if(3 == i)
		{
			Item.strText = rDcsIoDesc;
			m_wndDcsIoTypeGridCtrl.SetCellType(Item.row, Item.col , RUNTIME_CLASS(CGridCellEx));
		}
		else if(4 == i)
		{
			Item.strText = rPrefix;
			m_wndDcsIoTypeGridCtrl.SetCellType(Item.row, Item.col , RUNTIME_CLASS(CGridCellEx));
		}
		else if(5 == Item.col)
		{
			Item.nFormat = DT_CENTER|DT_WORDBREAK | DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX;
			Item.strText = rLoadId;
			m_wndDcsIoTypeGridCtrl.SetCellType(Item.row, Item.col , RUNTIME_CLASS(CGridCellEx));
		}

		m_wndDcsIoTypeGridCtrl.SetItem(&Item);
		if( 2 == Item.col )
		{
			CGridCellEx::DATA_TYPE_STRING = "COMBOBOX[DI,DO,AI]";
			m_wndDcsIoTypeGridCtrl.SetCellType(Item.row, Item.col , RUNTIME_CLASS(CFillGridCellComboBox));
		}
		else if(5 == Item.col)
		{
			CGridCellBase* pCell = m_wndDcsIoTypeGridCtrl.GetCell(Item.row , Item.col);
			if(pCell)
			{
				pCell->SetState(pCell->GetState() | GVIS_READONLY);
				pCell->SetBackClr(ReadOnlyColor);
			}
		}
	}
	m_wndDcsIoTypeGridCtrl.SetColumnWidth(4 , 50);
	m_wndDcsIoTypeGridCtrl.SetColumnWidth(5 , 80);
	m_wndDcsIoTypeGridCtrl.SetRowHeight(m_wndDcsIoTypeGridCtrl.GetRowCount() - 1, m_wndDcsIoTypeGridCtrl.GetRowHeight(0));

	return ERROR_SUCCESS;
}

BOOL CControlTypeCreationDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class
	if(wParam == m_wndLscOrLocalTypeGridCtrl.GetDlgCtrlID())
	{
		NM_GRIDVIEW* pGridView = (NM_GRIDVIEW*)(lParam);
		if((GVN_ENDLABELEDIT == pGridView->hdr.code) && (pGridView->iColumn == 1))
		{
			CString rItemText = m_wndLscOrLocalTypeGridCtrl.GetItemText(pGridView->iRow , pGridView->iColumn);
			CELoadDocData& docData = CELoadDocData::GetInstance();
			CControlTypeTable* pTable = static_cast<CControlTypeTable*>(docData.GetTableOf(_T("T_CONTROL_TYPE")));	//! database내용을 다 읽어온 상태...
			if(pTable)
			{
				vector<CELOADTable::Record>::iterator where = find_if(pTable->m_RecordEntry.begin(), pTable->m_RecordEntry.end(),bind2nd(MatchType(),rItemText));
				if(where != pTable->m_RecordEntry.end())
				{
					m_wndLscOrLocalTypeGridCtrl.SetItemText(pGridView->iRow , pGridView->iColumn, "");
					::MessageBoxA(NULL, "The Control Type exists already !!", "Warning", MB_OK|MB_ICONWARNING);
				}
			}
		}
	}
	else if(wParam == m_wndLcpIoTypeGridCtrl.GetDlgCtrlID())
	{
		NM_GRIDVIEW* pGridView = (NM_GRIDVIEW*)(lParam);
		if((GVN_ENDLABELEDIT == pGridView->hdr.code) && (pGridView->iColumn == 1))
		{
			CString rItemText = m_wndLcpIoTypeGridCtrl.GetItemText(pGridView->iRow , pGridView->iColumn);
			CELoadDocData& docData = CELoadDocData::GetInstance();
			CControlTypeTable* pTable = static_cast<CControlTypeTable*>(docData.GetTableOf(_T("T_CONTROL_TYPE")));	//! database내용을 다 읽어온 상태...
			if(pTable)
			{
				vector<CELOADTable::Record>::iterator where = find_if(pTable->m_RecordEntry.begin(), pTable->m_RecordEntry.end(),bind2nd(MatchType(),rItemText));
				if(where != pTable->m_RecordEntry.end())
				{
					m_wndLcpIoTypeGridCtrl.SetItemText(pGridView->iRow , pGridView->iColumn,"");
					::MessageBoxA(NULL, "The Control Type exists already !!", "Warning", MB_OK|MB_ICONWARNING);
				}
			}
		}
	}
	else if(wParam == m_wndDcsIoTypeGridCtrl.GetDlgCtrlID())
	{
		NM_GRIDVIEW* pGridView = (NM_GRIDVIEW*)(lParam);
		if((GVN_ENDLABELEDIT == pGridView->hdr.code) && (pGridView->iColumn == 1))
		{
			CString rItemText = m_wndDcsIoTypeGridCtrl.GetItemText(pGridView->iRow , pGridView->iColumn);
			CELoadDocData& docData = CELoadDocData::GetInstance();
			CControlTypeTable* pTable = static_cast<CControlTypeTable*>(docData.GetTableOf(_T("T_CONTROL_TYPE")));	//! database내용을 다 읽어온 상태...
			if(pTable)
			{
				vector<CELOADTable::Record>::iterator where = find_if(pTable->m_RecordEntry.begin(),pTable->m_RecordEntry.end(),bind2nd(MatchType(),rItemText));
				if(where != pTable->m_RecordEntry.end())
				{
					m_wndDcsIoTypeGridCtrl.SetItemText(pGridView->iRow , pGridView->iColumn,"");
					::MessageBoxA(NULL, "The Control Type exists already !!", "Warning", MB_OK|MB_ICONWARNING);
				}
			}
		}
	}

	return CWizardPropertyPage::OnNotify(wParam, lParam, pResult);
}

BOOL CControlTypeCreationDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if((m_wndLscOrLocalTypeGridCtrl.GetSafeHwnd() != pMsg->hwnd) && (FALSE == ::IsChild(m_wndLscOrLocalTypeGridCtrl.GetSafeHwnd() , pMsg->hwnd)) && (pMsg->message == WM_KEYDOWN)
		&& (m_wndLcpIoTypeGridCtrl.GetSafeHwnd() != pMsg->hwnd) && (FALSE == ::IsChild(m_wndLcpIoTypeGridCtrl.GetSafeHwnd() , pMsg->hwnd)) 
		&&(m_wndDcsIoTypeGridCtrl.GetSafeHwnd() != pMsg->hwnd) && (FALSE == ::IsChild(m_wndDcsIoTypeGridCtrl.GetSafeHwnd() , pMsg->hwnd)) )
	{
		if(pMsg->wParam == VK_ESCAPE) return TRUE;
		if(pMsg->wParam == VK_RETURN) return TRUE;
	}
	else if(pMsg->message == WM_LBUTTONDOWN)
	{
	}
	return CDialog::PreTranslateMessage(pMsg);
}
/**
	@brief	ADD BUTTON CLICK

	@author KHS	

	@return		
**/
void CControlTypeCreationDlg::OnBnClickedButtonAdd()
{
	m_bModified = true;

	CGridCtrl* pGridCtrl = GetGridCtrlOf(m_rCurCategory);
	if(pGridCtrl)
	{
		CELOADTable::Record record;

		record.FieldValueMap.insert(make_pair(_T("C_CATEGORY") , m_rCurCategory));
		record.FieldValueMap.insert(make_pair(_T("C_CHECK") , _T("NO")));
		record.FieldValueMap.insert(make_pair(_T("C_TYPE") , _T("")));
		record.FieldValueMap.insert(make_pair(_T("C_IO_TYPE") , _T("")));
		record.FieldValueMap.insert(make_pair(_T("C_DESC") , _T("")));
		record.FieldValueMap.insert(make_pair(_T("C_NAMING_RULE_PREFIX") , _T("")));

		InsertItemIntoGridCtrl(pGridCtrl , record);
	}

	///OnControltypecreationInsert();
}

/**
	@brief	DELETE BUTTON CLICK

	@author KHS	

	@return		
**/
void CControlTypeCreationDlg::OnBnClickedButtonDelete()
{
	// TODO: Add your control notification handler code here
	m_bModified = true;
	OnControltypecreationDelete();
}

/**
	@brief	

	@author BHK	

	@date 2009-04-22 오후 4:58:18	

	@param	

	@return		
*/
void CControlTypeCreationDlg::OnBnClickedOk()
{

	if(ERROR_SUCCESS == SaveData())
	{
		m_bModified = false;
		AfxMessageBox("SAVED");
	}
	///OnOK();
}

/**
	@brief	

	@author BHK	

	@date 2009-04-20 오후 7:49:12	

	@param	

	@return		
*/
CGridCtrl* CControlTypeCreationDlg::GetGridCtrlOf(const string& rCategory)
{
	CGridCtrl* pGridCtrl = NULL;
	for(int i = 0;i < SIZE_OF_ARRAY(__CATEGORIES__);++i)
	{
		if(_T("LCS or LOCAL TYPE") == rCategory)
		{
			pGridCtrl = &m_wndLscOrLocalTypeGridCtrl;
		}
		else if(_T("LCP IO TYPE") == rCategory)
		{
			pGridCtrl = &m_wndLcpIoTypeGridCtrl;
		}
		else if(_T("DCS IO TYPE") == rCategory)
		{
			pGridCtrl = &m_wndDcsIoTypeGridCtrl;
		}
	}

	return pGridCtrl;
}

/**
	@brief	DB 데이타를 화면에 출력해준다.

	@author KHS	

	@return		
**/
int CControlTypeCreationDlg::DisplayData(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CControlTypeTable* pTable = static_cast<CControlTypeTable*>(docData.GetTableOf(_T("T_CONTROL_TYPE")));	//! database내용을 다 읽어온 상태...
	if(pTable)
	{
		for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin();itr != pTable->m_RecordEntry.end();++itr)
		{
			const string rCategory = itr->FieldValueMap[_T("C_CATEGORY")];
			CGridCtrl* pGridCtrl = GetGridCtrlOf(rCategory);

			if(pGridCtrl)
			{
				InsertItemIntoGridCtrl(pGridCtrl , *itr);
			}
/*
			for(int i = 0;i < m_wndControlTypeCreationTabCtrl.GetTabsNum();++i)
			{
				CString strLabel;
				m_wndControlTypeCreationTabCtrl.GetTabLabel(i , strLabel);
				if(rCategory == strLabel.operator LPCSTR())
				{
					CGridCtrl* pGridCtrl = (CGridCtrl*)m_wndControlTypeCreationTabCtrl.GetTabWnd(i);
					
					if("LCS or LOCAL TYPE" == rCategory)
					{
						const CString rType = itr->FieldValueMap[_T("C_TYPE")].c_str();
						const CString rDesc = itr->FieldValueMap[_T("C_DESC")].c_str();

						InsertItemInLscOrLocalTypeGridCtrl(rType, rDesc);
						break;
					}
					else if("LCP IO TYPE" == rCategory )
					{
						const CString rType = itr->FieldValueMap[_T("C_TYPE")].c_str();
						const CString rDesc = itr->FieldValueMap[_T("C_DESC")].c_str();

						InsertItemInLcpIoTypeGridCtrl(rType, rDesc);
						break;
					}
					else if("DCS IO TYPE" == rCategory)
					{
						const CString rType = itr->FieldValueMap[_T("C_TYPE")].c_str();
						const CString rIoType = itr->FieldValueMap[_T("C_IO_TYPE")].c_str();
						const CString rDesc = itr->FieldValueMap[_T("C_DESC")].c_str();
						const CString rPrefix = itr->FieldValueMap[_T("C_NAMING_RULE_PREFIX")].c_str();

						InsertItemDcsIoTypeGridCtrl(rType, rIoType, rDesc, rPrefix);
						break;
					}
				}
			}
*/
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	화면의 데이타들을 DB에 저장한다.

	@author KHS	

	@return		
**/
int CControlTypeCreationDlg::SaveData(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CControlTypeTable* pTable = static_cast<CControlTypeTable*>(docData.GetTableOf(_T("T_CONTROL_TYPE")));
	if(pTable)
	{
		pTable->Clear();

		for(int i = 0;i < SIZE_OF_ARRAY(__CATEGORIES__);++i)
		{
			CGridCtrl* pGridCtrl = GetGridCtrlOf(__CATEGORIES__[i]);
			if(pGridCtrl)
			{
				if(0 == i)
				{
					for(int j = 1;j < pGridCtrl->GetRowCount();++j)//! 헤더 부분은 제외.
					{
						//! TYPE이 없으면 저장하지 않는다.
						if(pGridCtrl->GetItemText(j , 1).IsEmpty()) continue;

						CELOADTable::Record record;

						record.FieldValueMap.insert(make_pair(_T("C_CATEGORY") , __CATEGORIES__[i]));
						CGridCellCheck* pCellCheck = (CGridCellCheck*)pGridCtrl->GetCell(j,0);
						if(pCellCheck)
						{
							record.FieldValueMap.insert(make_pair(_T("C_CHECK") , pCellCheck->GetCheck() ? _T("YES") : _T("NO")));
						}else	record.FieldValueMap.insert(make_pair(_T("C_CHECK") , _T("NO")));
						
						record.FieldValueMap.insert(make_pair(_T("C_TYPE") , pGridCtrl->GetItemText(j , 1).operator LPCSTR()));
						record.FieldValueMap.insert(make_pair(_T("C_DESC") , pGridCtrl->GetItemText(j , 2).operator LPCSTR()));

						pTable->Insert(record);
					}
				}
				else if(1 == i)
				{
					for(int j = 1;j < pGridCtrl->GetRowCount();++j)//! 헤더 부분은 제외.
					{
						//! TYPE이 없으면 저장하지 않는다.
						if(pGridCtrl->GetItemText(j , 1).IsEmpty()) continue;

						CELOADTable::Record record;

						record.FieldValueMap.insert(make_pair(_T("C_CATEGORY") , __CATEGORIES__[i]));
						CGridCellCheck* pCellCheck = (CGridCellCheck*)pGridCtrl->GetCell(j,0);
						if(pCellCheck)
						{
							record.FieldValueMap.insert(make_pair(_T("C_CHECK") , pCellCheck->GetCheck() ? _T("YES") : _T("NO")));
						}else	record.FieldValueMap.insert(make_pair(_T("C_CHECK") , _T("NO")));

						record.FieldValueMap.insert(make_pair(_T("C_TYPE") , pGridCtrl->GetItemText(j , 1).operator LPCSTR()));
						record.FieldValueMap.insert(make_pair(_T("C_IO_TYPE") , pGridCtrl->GetItemText(j , 2).operator LPCSTR()));
						record.FieldValueMap.insert(make_pair(_T("C_DESC") , pGridCtrl->GetItemText(j , 3).operator LPCSTR()));

						pTable->Insert(record);
					}
				}
				else if(2 == i)
				{
					for(int j = 1;j < pGridCtrl->GetRowCount();++j)//! 헤더 부분은 제외.
					{
						//! TYPE이 없으면 저장하지 않는다.
						if(pGridCtrl->GetItemText(j , 1).IsEmpty()) continue;

						CELOADTable::Record record;

						record.FieldValueMap.insert(make_pair(_T("C_CATEGORY") , __CATEGORIES__[i]));
						CGridCellCheck* pCellCheck = (CGridCellCheck*)pGridCtrl->GetCell(j,0);
						if(pCellCheck)
						{
							record.FieldValueMap.insert(make_pair(_T("C_CHECK") , pCellCheck->GetCheck() ? _T("YES") : _T("NO")));
						}else	record.FieldValueMap.insert(make_pair(_T("C_CHECK") , _T("NO")));

						record.FieldValueMap.insert(make_pair(_T("C_TYPE") , pGridCtrl->GetItemText(j , 1).operator LPCSTR()));
						record.FieldValueMap.insert(make_pair(_T("C_IO_TYPE") , pGridCtrl->GetItemText(j , 2).operator LPCSTR()));
						record.FieldValueMap.insert(make_pair(_T("C_DESC") , pGridCtrl->GetItemText(j , 3).operator LPCSTR()));
						record.FieldValueMap.insert(make_pair(_T("C_NAMING_RULE_PREFIX") , pGridCtrl->GetItemText(j , 4).operator LPCSTR()));

						pTable->Insert(record);
					}
				}
			}

		}
		pTable->Save();

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-20 오후 8:18:45	

	@param	

	@return		
*/
void CControlTypeCreationDlg::OnBnClickedLcsButton()
{
	m_rCurCategory = __CATEGORIES__[0];
	{
		m_wndLCSTypeButton.SetState(TRUE);
		m_wndLCPTypeButton.SetState(FALSE);
		m_wndDCSTypeButton.SetState(FALSE);

		m_wndLCSTypeButton.SetBGColor(RGB(196,222,241), TRUE);
		m_wndLCPTypeButton.SetBGColor(RGB(255,255,255), TRUE);
		m_wndDCSTypeButton.SetBGColor(RGB(255,255,255), TRUE);

		m_wndLscOrLocalTypeGridCtrl.ShowWindow(TRUE);
		m_wndLcpIoTypeGridCtrl.ShowWindow(FALSE);
		m_wndDcsIoTypeGridCtrl.ShowWindow(FALSE);
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-04-20 오전 9:18:08	

	@param	

	@return		
*/
void CControlTypeCreationDlg::OnBnClickedLcpButton()
{
	m_rCurCategory = __CATEGORIES__[1];
	{
		m_wndLCSTypeButton.SetState(FALSE);
		m_wndLCPTypeButton.SetState(TRUE);
		m_wndDCSTypeButton.SetState(FALSE);
		
		m_wndLCPTypeButton.SetBGColor(RGB(196,222,241), TRUE);
		m_wndLCSTypeButton.SetBGColor(RGB(255,255,255), TRUE);
		m_wndDCSTypeButton.SetBGColor(RGB(255,255,255), TRUE);

		m_wndLscOrLocalTypeGridCtrl.ShowWindow(FALSE);
		m_wndLcpIoTypeGridCtrl.ShowWindow(TRUE);
		m_wndDcsIoTypeGridCtrl.ShowWindow(FALSE);
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-04-20 오전 9:18:33	

	@param	

	@return		
*/
void CControlTypeCreationDlg::OnBnClickedDcsButton()
{
	m_rCurCategory = __CATEGORIES__[2];
	{
		m_wndLCSTypeButton.SetState(FALSE);
		m_wndLCPTypeButton.SetState(FALSE);
		m_wndDCSTypeButton.SetState(TRUE);

		m_wndLCSTypeButton.SetBGColor(RGB(255,255,255), TRUE);
		m_wndLCPTypeButton.SetBGColor(RGB(255,255,255), TRUE);
		m_wndDCSTypeButton.SetBGColor(RGB(196,222,241), TRUE);

		m_wndLscOrLocalTypeGridCtrl.ShowWindow(FALSE);
		m_wndLcpIoTypeGridCtrl.ShowWindow(FALSE);
		m_wndDcsIoTypeGridCtrl.ShowWindow(TRUE);
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-04-22 오후 3:19:17	

	@param	

	@return		
*/
void CControlTypeCreationDlg::OnBnClickedWizback()
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
void CControlTypeCreationDlg::OnBnClickedWiznext()
{
	if(m_bModified)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		if(IDYES == AfxMessageBox(docData.GetStringOf(49).c_str() , MB_YESNO|MB_ICONWARNING ))
		{
			SaveData();
			m_bModified = false;
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
	@brief	

	@author BHK	

	@date 2009-04-22 오후 4:42:40	

	@param	

	@return		
*/
void CControlTypeCreationDlg::OnBnClickedCancel()
{
	if(m_bModified)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		if(IDYES == AfxMessageBox(docData.GetStringOf(48).c_str() , MB_YESNO|MB_ICONWARNING ))
		{
			SaveData();
			m_bModified = false;
			GetParent()->SendMessage(WM_COMMAND , IDOK);
		}
		else
		{
			GetParent()->SendMessage(WM_COMMAND , IDCANCEL);
		}
		
	}
	else
	{
		GetParent()->SendMessage(WM_COMMAND , IDCANCEL);
	}
}

/**
	@brief	다이얼로그 초기화

	@author KHS	

	@return		
**/
BOOL CControlTypeCreationDlg::Init(void)
{
	return TRUE;
}
