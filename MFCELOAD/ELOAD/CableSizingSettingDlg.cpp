// CableSizingSettingPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "CableSizingSettingDlg.h"
#include <util/excel9/ExcelAutomation.h>
#include "ELoadDocData.h"
#include <util/ado/SQLFile.h>

#include "Tables/3PHMotorCableSizingTable.h"
#include "Tables/3PHFeederCableSizingTable.h"
#include "Tables/1PHFeederCableSizingTable.h"
#include "Tables/DCFeederCableSizingTable.h"

// CCableSizingSettingDlg dialog

IMPLEMENT_DYNAMIC(CCableSizingSettingDlg, CDialog)

CCableSizingSettingDlg::CCableSizingSettingDlg(CWnd* pParent)
	: CDialog(CCableSizingSettingDlg::IDD, pParent)
{

}

CCableSizingSettingDlg::~CCableSizingSettingDlg()
{
}

void CCableSizingSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	DDX_Control(pDX , IDC_BUTTON_ADD               , m_wndAddButton);
	DDX_Control(pDX , IDC_BUTTON_DELETE            , m_wndDeleteButton);
	DDX_Control(pDX , IDC_BUTTON_IMPORT_FROM_EXCEL , m_wndImportExcelButton);

	DDX_Control(pDX, IDOK, m_wndOKButton);
	DDX_Control(pDX, IDCANCEL, m_wndCancelButton);
}


BEGIN_MESSAGE_MAP(CCableSizingSettingDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CCableSizingSettingDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CCableSizingSettingDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CCableSizingSettingDlg::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT_FROM_EXCEL, &CCableSizingSettingDlg::OnBnClickedButtonImportFromExcel)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CCableSizingSettingDlg::OnBnClickedButtonAdd)
END_MESSAGE_MAP()

/**
	@brief	select a excel file.

	@author	BHK
*/
static CString GetExcelFilePath()
{
	CString res;

	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY  ,"Excel Files(*.xls)|*.xls|All Files(*.*)|*.*||");
	if(IDOK == fileDlg.DoModal())
	{
		res = fileDlg.GetPathName();
		CString strFileTitle = fileDlg.GetFileTitle();
		CFile myFile;
		CFileException fileException;

		if(!myFile.Open(res, CFile::modeRead, &fileException))
		{
			TRACE("Can't open file %s, error = %u\n", res , fileException.m_cause);
		}
		myFile.Close();
	}
	else return "";

	return res;
}


// CCableSizingSettingDlg message handlers
/**
	@brief	excel 파일을 읽어서 database에 저장한다.

	@author	BHK
*/
int CCableSizingSettingDlg::SetCableSizeExcelFile(const int& nIndex, const CString& rExcelFilePath)
{
	if(rExcelFilePath.IsEmpty()) return ERROR_BAD_ENVIRONMENT;

	static const CString _TABLE_NAMES__[4] = 
	{
		"T_3PH_MOTOR_CABLE_SIZING" , "T_3PH_FEEDER_CABLE_SIZING" , "T_1PH_FEEDER_CABLE_SIZING" , "T_DC_FEEDER_CABLE_SIZING"
	};

	CExcelAutomation xlApp;

	//! 엑셀 파일은 오픈한다.
	if(!xlApp.Open(rExcelFilePath))
	{
		AfxMessageBox("Can't Start Excel and get Application object.");
		return ERROR_BAD_ENVIRONMENT;
	}

	long lowerRow = 0 , lowerCol = 0 , upperRow = 0 , upperCol = 0;
	xlApp.GetUsedRange(lowerRow , lowerCol , upperRow , upperCol);
	
	if(0 == nIndex)
	{
		C3PHMotorCableSizingTable& table = C3PHMotorCableSizingTable::GetInstance();
		table.Clear();
		for(int nRow = 2; nRow <= upperRow;++nRow)
		{
			const CString rCellNo = xlApp.MakeExcelNoString(nRow,1);
			CString rCellValue;
			xlApp.GetCellValue(COleVariant(rCellNo) , rCellValue);
			
			C3PHMotorCableSizingTable::Record record;
			record.sKW = rCellValue.operator LPCSTR();
			table.Insert(record);
		}
		table.Save();
	}
	else if(1 == nIndex)
	{
		C3PHFeederCableSizingTable& table = C3PHFeederCableSizingTable::GetInstance();
		table.Clear();
		for(int nRow = 2; nRow <= upperRow;++nRow)
		{
			const CString rCellNo = xlApp.MakeExcelNoString(nRow,1);
			CString rCellValue;
			xlApp.GetCellValue(COleVariant(rCellNo) , rCellValue);

			C3PHFeederCableSizingTable::Record record;
			record.rKW = rCellValue.operator LPCSTR();
			table.Insert(record);
		}
		table.Save();
	}
	else if(2 == nIndex)
	{
		C1PHFeederCableSizingTable& table = C1PHFeederCableSizingTable::GetInstance();
		for(int nRow = 2; nRow <= upperRow;++nRow)
		{
			const CString rCellNo = xlApp.MakeExcelNoString(nRow,1);
			CString rCellValue;
			xlApp.GetCellValue(COleVariant(rCellNo) , rCellValue);

			C1PHFeederCableSizingTable::Record record;
			record.rKW = rCellValue.operator LPCSTR();
			table.Insert(record);
		}
		table.Save();
	}
	else if(3 == nIndex)
	{
		CDCFeederCableSizingTable& table = CDCFeederCableSizingTable::GetInstance();
		table.Clear();
		for(int nRow = 2; nRow <= upperRow;++nRow)
		{
			const CString rCellNo = xlApp.MakeExcelNoString(nRow,1);
			CString rCellValue;
			xlApp.GetCellValue(COleVariant(rCellNo) , rCellValue);

			CDCFeederCableSizingTable::Record record;
			record.rKW = rCellValue.operator LPCSTR();
			table.Insert(record);
		}
		table.Save();
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-13 오후 5:18:43	

	@param	

	@return		
*/
BOOL CCableSizingSettingDlg::OnInitDialog()
{
        CDialog::OnInitDialog();

	BuildTabCtrl();

	DisplayTableData(0);
	DisplayTableData(1);
	DisplayTableData(2);
	DisplayTableData(3);

	//! BUtton Setting
	CELoadDocData & docData = CELoadDocData::GetInstance();
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	m_wndOKButton.SetTextHotColor(TextHotColor);;
	m_wndCancelButton.SetTextHotColor(TextHotColor);
	
	m_wndOKButton.SetIcon(IDI_OK_18X18);
	m_wndCancelButton.SetIcon(IDI_CANCEL_18X18);

	/*m_wnd3PHMotorGridCtrl.SetCompareFunction(CGridCtrl::pfnCellNumericCompare);
	m_wnd3PHFeederGridCtrl.SetCompareFunction(CGridCtrl::pfnCellNumericCompare);
	m_wnd1PHFeederGridCtrl.SetCompareFunction(CGridCtrl::pfnCellNumericCompare);
	m_wndDCFeederGridCtrl.SetCompareFunction(CGridCtrl::pfnCellNumericCompare);*/


        return TRUE;  // return TRUE unless you set the focus to a control
        // EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	그리드에 있는 내용을 테이블에 저장한다.

	@author	BHK
*/
void CCableSizingSettingDlg::OnBnClickedOk()
{
	{
		C3PHMotorCableSizingTable& table = C3PHMotorCableSizingTable::GetInstance();
		table.Clear();
		for(int i = 1;i < m_wnd3PHMotorGridCtrl.GetRowCount();++i)
		{
			TABLES::C3PHMotorCableSizingTable::Record record;
			record.sKW = m_wnd3PHMotorGridCtrl.GetItemText(i,0).operator LPCSTR();
			table.Insert(record);
		}
		table.Save();
	}

	{
		C3PHFeederCableSizingTable& table = C3PHFeederCableSizingTable::GetInstance();
		table.Clear();
		for(int i = 1;i < m_wnd3PHFeederGridCtrl.GetRowCount();++i)
		{
			TABLES::C3PHFeederCableSizingTable::Record record;
			record.rKW = m_wnd3PHFeederGridCtrl.GetItemText(i,0).operator LPCSTR();
			table.Insert(record);
		}
		table.Save();
	}

	{
		C1PHFeederCableSizingTable& table = C1PHFeederCableSizingTable::GetInstance();
		table.Clear();
		for(int i = 1;i < m_wnd1PHFeederGridCtrl.GetRowCount();++i)
		{
			TABLES::C1PHFeederCableSizingTable::Record record;
			record.rKW = m_wnd1PHFeederGridCtrl.GetItemText(i,0).operator LPCSTR();
			table.Insert(record);
		}
		table.Save();
	}

	{
		CDCFeederCableSizingTable& table = CDCFeederCableSizingTable::GetInstance();
		table.Clear();
		for(int i = 1;i < m_wndDCFeederGridCtrl.GetRowCount();++i)
		{
			TABLES::CDCFeederCableSizingTable::Record record;
			record.rKW = m_wndDCFeederGridCtrl.GetItemText(i,0).operator LPCSTR();
			table.Insert(record);
		}
		table.Save();
	}
	::MessageBox(NULL , _T("Success!!") , _T("SAVE") , MB_OK|MB_ICONINFORMATION);
}

/**
	@brief	

	@author	BHK
*/
void CCableSizingSettingDlg::OnBnClickedCancel()
{
	OnCancel();
}

/**
	@brief	TabCtrl과 자식 윈도우들을 생성합니다.

	@author BHK	

	@date 2009-04-13 오후 5:16:27	

	@param	

	@return		
*/
int CCableSizingSettingDlg::BuildTabCtrl(void)
{
	CRect rect;
	GetDlgItem(IDC_STATIC_TAB_CTRL)->GetWindowRect(&rect);
	ScreenToClient(&rect);

	if (!m_wndTabCtrl.Create (CMFCTabCtrl::STYLE_3D_VS2005, rect , this , 0x100))
	{
		TRACE0("Failed to create Properies Grid \n");
		return ERROR_BAD_ENVIRONMENT;      // fail to create
	}
	m_wndTabCtrl.SetLocation(CMFCTabCtrl::LOCATION_TOP);
	m_wndTabCtrl.SetActiveTabBoldFont(TRUE);
	m_wndTabCtrl.EnableAutoColor(TRUE);

	if(TRUE == m_wnd3PHMotorGridCtrl.Create(rect , &m_wndTabCtrl , 0x101))
	{
		m_wnd3PHMotorGridCtrl.SetFixedRowCount(1);
		m_wnd3PHMotorGridCtrl.SetColumnCount(2);

		m_wnd3PHMotorGridCtrl.SetItemText(0,0,_T("CAPACITY"));
		m_wnd3PHMotorGridCtrl.SetItemText(0,1,_T("UNIT"));
		m_wndTabCtrl.AddTab(&m_wnd3PHMotorGridCtrl,_T("3PH MOTOR"));
	}
	if(TRUE == m_wnd3PHFeederGridCtrl.Create(rect , &m_wndTabCtrl , 0x102))
	{
		m_wnd3PHFeederGridCtrl.SetFixedRowCount(1);
		m_wnd3PHFeederGridCtrl.SetColumnCount(2);

		m_wnd3PHFeederGridCtrl.SetItemText(0,0,_T("CAPACITY"));
		m_wnd3PHFeederGridCtrl.SetItemText(0,1,_T("UNIT"));
		m_wndTabCtrl.AddTab(&m_wnd3PHFeederGridCtrl,_T("3PH FEEDER"));
	}
	if(TRUE == m_wnd1PHFeederGridCtrl.Create(rect , &m_wndTabCtrl , 0x103))
	{
		m_wnd1PHFeederGridCtrl.SetFixedRowCount(1);
		m_wnd1PHFeederGridCtrl.SetColumnCount(2);

		m_wnd1PHFeederGridCtrl.SetItemText(0,0,_T("CAPACITY"));
		m_wnd1PHFeederGridCtrl.SetItemText(0,1,_T("UNIT"));
		m_wndTabCtrl.AddTab(&m_wnd1PHFeederGridCtrl,_T("1PH FEEDER"));
	}
	if(TRUE == m_wndDCFeederGridCtrl.Create(rect , &m_wndTabCtrl , 0x104))
	{
		m_wndDCFeederGridCtrl.SetFixedRowCount(1);
		m_wndDCFeederGridCtrl.SetColumnCount(2);

		m_wndDCFeederGridCtrl.SetItemText(0,0,_T("CAPACITY"));
		m_wndDCFeederGridCtrl.SetItemText(0,1,_T("UNIT"));
		m_wndTabCtrl.AddTab(&m_wndDCFeederGridCtrl,_T("DC FEEDER"));
	}

	return 0;
}

/**
	@brief	선택한 내용을  GridCtrl에서 삭제한다.

	@author BHK	

	@date 2009-04-13 오후 5:38:44	

	@param	

	@return		
*/
void CCableSizingSettingDlg::OnBnClickedButtonDelete()
{
	const int nActiveTab = m_wndTabCtrl.GetActiveTab();
	CGridCtrl* pGridCtrl = (CGridCtrl*)m_wndTabCtrl.GetActiveWnd();
	if(pGridCtrl)
	{
		pGridCtrl->SetFocus();
		CCellRange range = pGridCtrl->GetSelectedCellRange();
		for(int i = range.GetMaxRow();i >= range.GetMinRow();--i) pGridCtrl->DeleteRow(i);
		pGridCtrl->RedrawWindow();
		///pGridCtrl->SetRowCount(pGridCtrl->GetRowCount() - (range.GetMaxRow() - range.GetMinRow() + 1));
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-04-13 오후 5:40:41	

	@param	

	@return		
*/
int CCableSizingSettingDlg::DisplayTableData(const int& nIndex)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const COLORREF ReadOnlyColor = docData.GetColorCode(_T("READ_ONLY_BACK_COLOR"));

	if(0 == nIndex)
	{
		C3PHMotorCableSizingTable& table = C3PHMotorCableSizingTable::GetInstance();
		table.Load();
		m_wnd3PHMotorGridCtrl.SetRowCount(table.GetRecordCount() + 1);
		for(int i = 0;i < (int)table.GetRecordCount();++i)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = i+1;
			Item.col = 0;
			Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
			Item.strText = table.GetRecordAt(i)->sKW.c_str();
			m_wnd3PHMotorGridCtrl.SetItem(&Item);
			
			m_wnd3PHMotorGridCtrl.SetItemText(i + 1,1,_T("KW"));
			CGridCell* pCell = (CGridCell*)m_wnd3PHMotorGridCtrl.GetCell(i+1,1);
			if(pCell)
			{
				pCell->SetState(pCell->GetState() | GVIS_READONLY);
				pCell->SetBackClr(ReadOnlyColor);
			}
		}
		m_wnd3PHMotorGridCtrl.AutoSizeRows();
	}
	else if(1 == nIndex)
	{
		C3PHFeederCableSizingTable& table = C3PHFeederCableSizingTable::GetInstance();
		table.Load();
		m_wnd3PHFeederGridCtrl.SetRowCount(table.GetRecordCount() + 1);
		for(int i = 0;i < (int)table.GetRecordCount();++i)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = i+1;
			Item.col = 0;
			Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
			Item.strText = table.GetRecordAt(i)->rKW.c_str();
			m_wnd3PHFeederGridCtrl.SetItem(&Item);

			m_wnd3PHFeederGridCtrl.SetItemText(i + 1,1,_T("KVA"));
			CGridCell* pCell = (CGridCell*)m_wnd3PHFeederGridCtrl.GetCell(i+1,1);
			if(pCell)
			{
				pCell->SetState(pCell->GetState() | GVIS_READONLY);
				pCell->SetBackClr(ReadOnlyColor);
			}
		}
		m_wnd3PHFeederGridCtrl.AutoSizeRows();
	}
	else if(2 == nIndex)
	{
		C1PHFeederCableSizingTable& table = C1PHFeederCableSizingTable::GetInstance();
		table.Load();
		m_wnd1PHFeederGridCtrl.SetRowCount(table.GetRecordCount() + 1);
		for(int i = 0;i < (int)table.GetRecordCount();++i)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = i+1;
			Item.col = 0;
			Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
			Item.strText = table.GetRecordAt(i)->rKW.c_str();
			m_wnd1PHFeederGridCtrl.SetItem(&Item);

			m_wnd1PHFeederGridCtrl.SetItemText(i + 1,1,_T("KVA"));
			CGridCell* pCell = (CGridCell*)m_wnd1PHFeederGridCtrl.GetCell(i+1,1);
			if(pCell)
			{
				pCell->SetState(pCell->GetState() | GVIS_READONLY);
				pCell->SetBackClr(ReadOnlyColor);
			}
		}
		m_wnd1PHFeederGridCtrl.AutoSizeRows();
	}
	else if(3 == nIndex)
	{
		CDCFeederCableSizingTable& table = CDCFeederCableSizingTable::GetInstance();
		table.Load();
		m_wndDCFeederGridCtrl.SetRowCount(table.GetRecordCount() + 1);
		for(int i = 0;i < (int)table.GetRecordCount();++i)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = i+1;
			Item.col = 0;
			Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
			Item.strText = table.GetRecordAt(i)->rKW.c_str();
			m_wndDCFeederGridCtrl.SetItem(&Item);

			m_wndDCFeederGridCtrl.SetItemText(i + 1,1,_T("KVA"));
			CGridCell* pCell = (CGridCell*)m_wndDCFeederGridCtrl.GetCell(i+1,1);
			if(pCell)
			{
				pCell->SetState(pCell->GetState() | GVIS_READONLY);
				pCell->SetBackClr(ReadOnlyColor);
			}
		}
		m_wndDCFeederGridCtrl.AutoSizeRows();
	}

	return ERROR_SUCCESS;
}

/**
	@brief	excel의 내용을 import하여 table에 저장한다.

	@author BHK	

	@date 2009-04-13 오후 5:53:34	

	@param	

	@return		
*/
void CCableSizingSettingDlg::OnBnClickedButtonImportFromExcel()
{
	const int nActiveTab = m_wndTabCtrl.GetActiveTab();
	SetCableSizeExcelFile(nActiveTab , GetExcelFilePath());
}

/**
	@brief	새로운 CELL을 하나 추가한다.

	@author BHK	

	@date 2009-04-13 오후 7:05:19	

	@param	

	@return		
*/
void CCableSizingSettingDlg::OnBnClickedButtonAdd()
{
	const int nActiveTab = m_wndTabCtrl.GetActiveTab();
	CGridCtrl* pGridCtrl = (CGridCtrl*)m_wndTabCtrl.GetActiveWnd();
	if(pGridCtrl)
	{
		pGridCtrl->SetFocus();
		pGridCtrl->SetRowCount(pGridCtrl->GetRowCount() + 1);
		{
			CELoadDocData& docData = CELoadDocData::GetInstance();
			const COLORREF ReadOnlyColor = docData.GetColorCode("READ_ONLY_BACK_COLOR");

			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = pGridCtrl->GetRowCount() - 1;
			Item.col = 0;
			Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
			Item.strText = _T("");
			pGridCtrl->SetItem(&Item);

			pGridCtrl->SetItemText(Item.row , 1 , (0 == nActiveTab) ? _T("KW") : _T("KVA"));
			CGridCell* pCell = (CGridCell*)pGridCtrl->GetCell(Item.row,1);
			if(pCell)
			{
				pCell->SetState(pCell->GetState() | GVIS_READONLY);
				pCell->SetBackClr(ReadOnlyColor);
			}
		}

		pGridCtrl->SetFocusCell(pGridCtrl->GetRowCount() - 1,0);
		pGridCtrl->SetSelectedRange(pGridCtrl->GetRowCount() - 1,0,pGridCtrl->GetRowCount() - 1,0);
		pGridCtrl->RedrawWindow();
	}
}

int CALLBACK fnCellNumericCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CGridCellBase* pCell1 = (CGridCellBase*) lParam1;
	CGridCellBase* pCell2 = (CGridCellBase*) lParam2;
	if (!pCell1 || !pCell2) return 0;

	double nValue1 = atof(pCell1->GetText());
	double nValue2 = atof(pCell2->GetText());

	if (nValue1 < nValue2)
		return -1;
	else if (nValue1 == nValue2)
		return 0;
	else
		return 1;
}



/**
	@brief	

	@author BHK	

	@date 2009-04-13 오후 7:14:40	

	@param	

	@return		
*/
BOOL CCableSizingSettingDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if(wParam == m_wnd3PHMotorGridCtrl.GetDlgCtrlID())
	{
		NM_GRIDVIEW* pGridView = (NM_GRIDVIEW*)(lParam);

		if((GVN_ENDLABELEDIT == pGridView->hdr.code) && (0 == pGridView->iColumn))
		{
			CGridCellBase* pCell = m_wnd3PHMotorGridCtrl.GetCell(pGridView->iRow, pGridView->iColumn);
			if(pCell) pCell->EndEdit();

			m_wnd3PHMotorGridCtrl.SortItems(fnCellNumericCompare , 0, TRUE, NULL);
			m_wnd3PHMotorGridCtrl.RedrawWindow();
		}
		return TRUE;
	}
	else if(wParam == m_wnd3PHFeederGridCtrl.GetDlgCtrlID())
	{
		NM_GRIDVIEW* pGridView = (NM_GRIDVIEW*)(lParam);

		if((GVN_ENDLABELEDIT == pGridView->hdr.code) && (0 == pGridView->iColumn))
		{
			m_wnd3PHFeederGridCtrl.SortItems(fnCellNumericCompare , 0, TRUE, NULL);
			m_wnd3PHFeederGridCtrl.RedrawWindow();
		}
	}
	else if(wParam == m_wnd1PHFeederGridCtrl.GetDlgCtrlID())
	{
		NM_GRIDVIEW* pGridView = (NM_GRIDVIEW*)(lParam);

		if((GVN_ENDLABELEDIT == pGridView->hdr.code) && (0 == pGridView->iColumn))
		{
			m_wnd1PHFeederGridCtrl.SortItems(fnCellNumericCompare , 0, TRUE, NULL);
			m_wnd1PHFeederGridCtrl.RedrawWindow();
		}
	}
	else if(wParam == m_wndDCFeederGridCtrl.GetDlgCtrlID())
	{
		NM_GRIDVIEW* pGridView = (NM_GRIDVIEW*)(lParam);

		if((GVN_ENDLABELEDIT == pGridView->hdr.code) && (0 == pGridView->iColumn))
		{
			m_wndDCFeederGridCtrl.SortItems(fnCellNumericCompare , 0, TRUE, NULL);
			m_wndDCFeederGridCtrl.RedrawWindow();
		}
	}

	return CDialog::OnNotify(wParam, lParam, pResult);
}
