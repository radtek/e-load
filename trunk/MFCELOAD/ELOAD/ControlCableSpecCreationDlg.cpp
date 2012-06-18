// ControlCableSpecCreationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "ControlCableSpecCreationDlg.h"
#include "ELoadDocData.h"
#include "GridCellEx.h"

// CControlCableSpecCreationDlg dialog

IMPLEMENT_DYNAMIC(CControlCableSpecCreationDlg, CDialog)

CControlCableSpecCreationDlg::CControlCableSpecCreationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CControlCableSpecCreationDlg::IDD, pParent)
{

}

CControlCableSpecCreationDlg::~CControlCableSpecCreationDlg()
{
}

void CControlCableSpecCreationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CONTROL_CABLE_SETTING_STATIC, m_ControlCableSettingStatic);

	DDX_Control(pDX, IDC_MAINCABLE_DELETE_BUTTON, m_wndMainCableDeleteButton);
	DDX_Control(pDX, IDC_MAINCABLE_ADD_BUTTON, m_wndMainCableAddButton);
	DDX_Control(pDX, IDCANCEL, m_wndCancelButton);
	DDX_Control(pDX, IDOK, m_wndOKButton);
}


BEGIN_MESSAGE_MAP(CControlCableSpecCreationDlg, CDialog)
	ON_BN_CLICKED(IDC_MAINCABLE_DELETE_BUTTON, &CControlCableSpecCreationDlg::OnBnClickedMaincableDeleteButton)
	ON_BN_CLICKED(IDC_MAINCABLE_ADD_BUTTON, &CControlCableSpecCreationDlg::OnBnClickedMaincableAddButton)
	ON_BN_CLICKED(IDOK, &CControlCableSpecCreationDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CControlCableSpecCreationDlg message handlers
/**
	@brief	

	@author BHK	

	@date 2009-04-17 오후 5:46:15	

	@param	

	@return		
*/
BOOL CControlCableSpecCreationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(ERROR_SUCCESS != CreateMainCableGridCtrl())
	{
		TRACE0("Failed to create Properies Grid \n");
		return FALSE;
	}
	DisplayMainCableData();

	CELoadDocData & docData = CELoadDocData::GetInstance();
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));

	m_wndMainCableDeleteButton.SetTextHotColor(TextHotColor);
	m_wndMainCableAddButton.SetTextHotColor(TextHotColor);
	m_wndCancelButton.SetTextHotColor(TextHotColor);
	m_wndOKButton.SetTextHotColor(TextHotColor);

	m_wndMainCableDeleteButton.SetIcon(IDI_DEL);
	m_wndMainCableAddButton.SetIcon(IDI_ADD);

	m_wndCancelButton.SetIcon(IDI_OK_18X18);
	m_wndOKButton.SetIcon(IDI_CANCEL_18X18);


	//! LABEL SETTING
	m_ControlCableSettingStatic.SetBkColor(DarkenColor(GetSysColor(COLOR_BTNFACE),0.15) , GetSysColor(COLOR_BTNFACE) , CLabelStatic::HorzGradient);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	CREATE 상단 메인 CABLE GRID CTRL

	@author KHS	

	@return		
**/
int CControlCableSpecCreationDlg::CreateMainCableGridCtrl(void)
{
	//m_wndMainCableGridCtrl
	static const CString __HEADERS__[] = {_T("CABLE SPEC") , _T("CORE"), _T("SIZE"), _T("Cable OD[MM]"), _T("Cable Weight[kg/km]")};

	CRect rect;
	GetDlgItem(IDC_CABLE_STATIC)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	if(m_wndMainCableGridCtrl.Create(rect , this , 104))
	{
		m_wndMainCableGridCtrl.SetColumnCount(sizeof(__HEADERS__) / sizeof(__HEADERS__[0]));
		m_wndMainCableGridCtrl.SetFixedRowCount(1);

		m_wndMainCableGridCtrl.SetEditable(TRUE);
		m_wndMainCableGridCtrl.SetRowResize(FALSE);
		m_wndMainCableGridCtrl.SetColumnResize(TRUE);
		
		// 해더
		const int nColumnCount = m_wndMainCableGridCtrl.GetColumnCount();
		for(int i = 0;i < nColumnCount;++i)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = 0;
			Item.col = i;
			Item.nFormat = DT_LEFT|DT_WORDBREAK | DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX;
			Item.strText = __HEADERS__[i];
			m_wndMainCableGridCtrl.SetItem(&Item);
		}

		m_wndMainCableGridCtrl.AutoSizeColumns();
		m_wndMainCableGridCtrl.AutoSizeRows();

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/** 
	@brief	MAIN CABLE DATA를 화면에 출력한다.

	@author KHS	

	@return		
**/
int CControlCableSpecCreationDlg::DisplayMainCableData(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pTable = docData.GetTableOf(_T("T_CONTROL_CABLE_SETTING"));
	if(pTable)
	{
		for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin(); itr != pTable->m_RecordEntry.end();++itr)
		{
			const CString rCableSpec	= itr->FieldValueMap[_T("C_SPEC")].c_str();
			const CString rCableCore	= itr->FieldValueMap[_T("C_CORE")].c_str();
			const CString rCableSize	= itr->FieldValueMap[_T("C_SIZE")].c_str();
			const CString rCableOD		= itr->FieldValueMap[_T("C_OD")].c_str();
			const CString rCableWeight	= itr->FieldValueMap[_T("C_WEIGHT")].c_str();

			InsertItemInMainCableGridCtrl(rCableSpec, rCableCore, rCableSize, rCableOD, rCableWeight);
		}

		///DisplaySchematicTypeData();

		return ERROR_SUCCESS;
	}
	return ERROR_BAD_ENVIRONMENT;

}


/**
	@brief	선택한 항목을 삭제합니다.

	@author BHK	

	@date 2009-04-17 오후 5:52:22	

	@param	

	@return		
*/
void CControlCableSpecCreationDlg::OnBnClickedMaincableDeleteButton()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pTable = docData.GetTableOf(_T("T_CONTROL_CABLE_SETTING"));
	if(pTable)
	{
		CCellRange range = m_wndMainCableGridCtrl.GetSelectedCellRange();
		for(int i = range.GetMaxRow();i >= range.GetMinRow();--i)	
		{
			m_wndMainCableGridCtrl.DeleteRow(i);
			
			//! 테이블 상의 레코드를 삭제한다.
			int nIndex = 0;
			for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin();itr != pTable->m_RecordEntry.end();++itr,++nIndex)
			{
				if(nIndex == (i - 1))	//! 그리드 컨트롤의 헤더 때문에 1을 빼준 값과 비교한다.
				{
					pTable->m_RecordEntry.erase(itr);
					break;
				}
			}
		}

		m_wndMainCableGridCtrl.RedrawWindow();
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-04-17 오후 6:41:46	

	@param	

	@return		
*/
void CControlCableSpecCreationDlg::OnBnClickedMaincableAddButton()
{
	InsertItemInMainCableGridCtrl();
	
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pTable = docData.GetTableOf(_T("T_CONTROL_CABLE_SETTING"));
	if(pTable)
	{
		CELOADTable::Record record;

		record.FieldValueMap[_T("C_SPEC")]	= "";
		record.FieldValueMap[_T("C_CORE")]	= "";
		record.FieldValueMap[_T("C_SIZE")]	= "";
		record.FieldValueMap[_T("C_OD")]	= "";
		record.FieldValueMap[_T("C_WEIGHT")]	= "";

		pTable->m_RecordEntry.push_back(record);
	}
}

/**
	@brief	INSERT ITEM IN MAIN CABLE GRID CTRL

	@author KHS	

	@return		
**/
int CControlCableSpecCreationDlg::InsertItemInMainCableGridCtrl(const CString& rCableSpec, const CString& rCableCore, const CString& rCableSize, const CString& rCableOD, const CString& rCableWeight)
{
	CELoadDocData &docData = CELoadDocData::GetInstance();
	const COLORREF ReadOnlyColor = docData.GetColorCode("READ_ONLY_BACK_COLOR");

	m_wndMainCableGridCtrl.SetRowCount(m_wndMainCableGridCtrl.GetRowCount() + 1);

	const int nRow = m_wndMainCableGridCtrl.GetRowCount() -1;
	const int nColCount = m_wndMainCableGridCtrl.GetColumnCount();
	
	for(int i = 0;i < nColCount;++i)
	{
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.row = nRow;
		Item.col = i;
		Item.nFormat = DT_LEFT|DT_WORDBREAK | DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX;
		if(0 == i)
			Item.strText = rCableSpec;
		else if(1 == i)
			Item.strText = rCableCore;
		else if(2 == i)
			Item.strText = rCableSize;
		else if(3 == i)
			Item.strText = rCableOD;
		else
			Item.strText = rCableWeight;

		m_wndMainCableGridCtrl.SetItem(&Item);
		m_wndMainCableGridCtrl.SetCellType(Item.row, Item.col , RUNTIME_CLASS(CGridCellEx));
	}
	m_wndMainCableGridCtrl.SetRowHeight(m_wndMainCableGridCtrl.GetRowCount() - 1, m_wndMainCableGridCtrl.GetRowHeight(0));
	m_wndMainCableGridCtrl.AutoSizeColumns();

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-17 오후 6:44:37	

	@param	

	@return		
*/
BOOL CControlCableSpecCreationDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR* pNMHDR = (NMHDR*)lParam;
	pNMHDR->hwndFrom;
	GV_DISPINFO* dispinfo = (GV_DISPINFO*)(lParam);
	
	CELoadDocData& docData = CELoadDocData::GetInstance();
	if(wParam == m_wndMainCableGridCtrl.GetDlgCtrlID())
	{
		if(dispinfo->hdr.code == GVN_ENDLABELEDIT)
		{
			const int nRow = dispinfo->item.row;
			const int nCol = dispinfo->item.col;

			const string rItemText = m_wndMainCableGridCtrl.GetItemText(nRow , nCol);

			CELOADTable* pTable = docData.GetTableOf(_T("T_CONTROL_CABLE_SETTING"));
			if(pTable)
			{
				if(0 == nCol)
					pTable->m_RecordEntry[nRow - 1].FieldValueMap[_T("C_SPEC")] = rItemText;
				else if(1 == nCol)
					pTable->m_RecordEntry[nRow - 1].FieldValueMap[_T("C_CORE")] = rItemText;
				else if(2 == nCol)
					pTable->m_RecordEntry[nRow - 1].FieldValueMap[_T("C_SIZE")] = rItemText;
				else if(3 == nCol)
					pTable->m_RecordEntry[nRow - 1].FieldValueMap[_T("C_OD")] = rItemText;
				else if(4 == nCol)
					pTable->m_RecordEntry[nRow - 1].FieldValueMap[_T("C_WEIGHT")] = rItemText;
			}
		}
	}

	return CDialog::OnNotify(wParam, lParam, pResult);
}

/**
	@brief	

	@author BHK	

	@date 2009-04-17 오후 6:45:56	

	@param	

	@return		
*/
BOOL CControlCableSpecCreationDlg::PreTranslateMessage(MSG* pMsg)
{
	if((m_wndMainCableGridCtrl.GetSafeHwnd() != pMsg->hwnd) && (FALSE == ::IsChild(m_wndMainCableGridCtrl.GetSafeHwnd() , pMsg->hwnd)))
	{
		if(pMsg->wParam == VK_ESCAPE) return TRUE;
		if(pMsg->wParam == VK_RETURN) return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

/**
	@brief	control cable data를 database에 저장합니다.

	@author KHS	

	@return		
**/
void CControlCableSpecCreationDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pTable = docData.GetTableOf(_T("T_CONTROL_CABLE_SETTING"));
	if(pTable) pTable->Save();

	OnOK();
}
