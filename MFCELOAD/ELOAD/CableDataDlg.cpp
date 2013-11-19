// CableSizingPropPage.cpp : implementation file
//

//#ifdef _DEBUG
//#undef _DEBUG
//#include <python.h>
//#define _DEBUG
//#else
//#include <python.h>
//#endif

#include "stdafx.h"
#include "ELOAD.h"
#include "CableDataDlg.h"
#include <istools/inc/util/excel9/ExcelAutomation.h>
#include "ELoadDocData.h"
#include "Tables/CableDataTable.h"

#include "PythonCall.h"
#include "SimpleAnimationDlg.h"

#include <set>

// CCableDataDlg dialog

IMPLEMENT_DYNAMIC(CCableDataDlg, CDialog)

CCableDataDlg::CCableDataDlg()
	: CDialog(CCableDataDlg::IDD)
	, m_rUGBaseTemp(_T(""))
	, m_rAGBaseTemp(_T(""))
	, m_rMaxConductorTemp(_T(""))
{
	m_pTT = NULL;
}

CCableDataDlg::~CCableDataDlg()
{
	try
	{
		if(m_pTT) SAFE_DELETE(m_pTT);
	}
	catch(...)
	{
	}
}

void CCableDataDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX , IDC_LIST_CABLE_DATA , m_wndCableDataListCtrl);

	DDX_Control(pDX , IDC_COMBO_CABLE_TYPE , m_cboCableType);
	DDX_Control(pDX , IDC_COMBO_HERTZ , m_cboHertz);
	DDX_Control(pDX , IDC_FPSPREAD1 , m_wndSpread);
	DDX_Control(pDX , IDC_BUTTON_IMPORT , m_wndImportButton);
	DDX_Control(pDX , IDC_BUTTON_EXPORT , m_wndExportButton);
	DDX_Control(pDX , IDC_BUTTON_DELETE , m_wndDeleteButton);

	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);

	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_UG_TEMP_EDIT, m_rUGBaseTemp);
	DDX_Text(pDX, IDC_AG_TEMP_EDIT, m_rAGBaseTemp);
	DDX_Text(pDX, IDC_MAX_TEMP_EDIT, m_rMaxConductorTemp);
}


BEGIN_MESSAGE_MAP(CCableDataDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT, &CCableDataDlg::OnBnClickedButtonImport)
	ON_CBN_SELCHANGE(IDC_COMBO_CABLE_TYPE, &CCableDataDlg::OnCbnSelchangeComboCableType)
        ON_BN_CLICKED(IDC_BUTTON_DELETE, &CCableDataDlg::OnBnClickedButtonDelete)
        ON_BN_CLICKED(IDC_BUTTON_EXPORT, &CCableDataDlg::OnBnClickedButtonExport)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_CABLE_DATA, &CCableDataDlg::OnLvnKeydownListCableData)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CABLE_DATA, &CCableDataDlg::OnLvnItemchangedListCableData)
	ON_CBN_SELCHANGE(IDC_COMBO_HERTZ, &CCableDataDlg::OnCbnSelchangeComboHertz)
END_MESSAGE_MAP()


// CCableDataDlg message handlers

BOOL CCableDataDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_wndCableDataListCtrl.InsertColumn(0 , _T("Table Name")  , LVCFMT_LEFT , 200);
	m_wndCableDataListCtrl.InsertColumn(1 , _T("Table Desc.") , LVCFMT_LEFT , 300);
        m_wndCableDataListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

	m_pTT = new CToolTipCtrl;
	if(m_pTT)
	{
		m_pTT->Create(this , TTS_BALLOON|WS_POPUP|WS_EX_TOOLWINDOW);
		m_pTT->AddTool(&m_wndCableDataListCtrl , _T("DELETE키로 테이블을 삭제할수 있습니다."));
	}

        //! LOAD ICON
	CELoadDocData &docData = CELoadDocData::GetInstance();
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	m_wndImportButton.SetTextHotColor(TextHotColor);
	m_wndExportButton.SetTextHotColor(TextHotColor);
	m_wndDeleteButton.SetTextHotColor(TextHotColor);

	m_wndImportButton.SetIcon(IDI_EXCEL_IMPORT);
        m_wndExportButton.SetIcon(IDI_EXCEL_EXPORT);
        m_wndDeleteButton.SetIcon(IDI_EXCEL_DELETE);

	m_wndCancelButton.SetIcon(IDI_CANCEL_18X18);

	LoadData();
	DisplayData();

	m_wndSpread.SetMaxRows(1);
		
	m_wndSpread.SetRowHeadersShow(FALSE);
	m_wndSpread.SetMaxCols(8);

	m_wndSpread.SetColHeaderRows(2);
	m_wndSpread.SetRowHeaderCols(0);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
	m_wndSpread.SetCol(1);
	m_wndSpread.SetText(COleVariant("SIZE"));
	m_wndSpread.AddCellSpan(1,0,1,2);
	
	m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
	m_wndSpread.SetCol(2);
	m_wndSpread.SetText(COleVariant("OVERALL\nDIAMETER"));
	m_wndSpread.AddCellSpan(2,0,1,2);

	m_wndSpread.SetCol(3);
	m_wndSpread.SetText(COleVariant("IN BURIAL\nAMP."));
	m_wndSpread.AddCellSpan(3,0,1,2);

	m_wndSpread.SetCol(4);
	m_wndSpread.SetText(COleVariant("IN AIR\nAMP."));
	m_wndSpread.AddCellSpan(4,0,1,2);

	m_wndSpread.SetCol(5);
	m_wndSpread.SetText(COleVariant("R"));
	m_wndSpread.AddCellSpan(5,0,1,2);

	m_wndSpread.SetCol(6);
	m_wndSpread.SetText(COleVariant("L"));
	m_wndSpread.AddCellSpan(6,0,1,2);

	m_wndSpread.SetCol(7);
	m_wndSpread.SetText(COleVariant("CABLE\n  OD"));
	m_wndSpread.AddCellSpan(7,0,1,2);

	m_wndSpread.SetCol(8);
	m_wndSpread.SetText(COleVariant("CABLE\nWEIGHT"));
	m_wndSpread.AddCellSpan(8,0,1,2);

	m_wndSpread.SetRow(1);
	m_wndSpread.SetCol(1);
	m_wndSpread.SetText(COleVariant("SQMM"));
	m_wndSpread.SetCellType(CFpspread1::CellTypeStaticText);
	m_wndSpread.SetForeColor(RGB(0,0,255));
	m_wndSpread.SetBackColor(RGB(177,177,177));
	m_wndSpread.SetTypeHAlign(CFpspread1::TypeHAlignCenter);

	m_wndSpread.SetCol(2);
	m_wndSpread.SetText(COleVariant("MM"));
	m_wndSpread.SetCellType(CFpspread1::CellTypeStaticText);
	m_wndSpread.SetForeColor(RGB(0,0,255));
	m_wndSpread.SetBackColor(RGB(177,177,177));
	m_wndSpread.SetTypeHAlign(CFpspread1::TypeHAlignCenter);

	m_wndSpread.SetCol(3);
	m_wndSpread.SetText(COleVariant("(A)"));
	m_wndSpread.SetCellType(CFpspread1::CellTypeStaticText);
	m_wndSpread.SetForeColor(RGB(0,0,255));
	m_wndSpread.SetBackColor(RGB(177,177,177));
	m_wndSpread.SetTypeHAlign(CFpspread1::TypeHAlignCenter);

	m_wndSpread.SetCol(4);
	m_wndSpread.SetText(COleVariant("(A)"));
	m_wndSpread.SetCellType(CFpspread1::CellTypeStaticText);
	m_wndSpread.SetForeColor(RGB(0,0,255));
	m_wndSpread.SetBackColor(RGB(177,177,177));
	m_wndSpread.SetTypeHAlign(CFpspread1::TypeHAlignCenter);

	m_wndSpread.SetCol(5);
	m_wndSpread.SetText(COleVariant("(Ohm/km)"));
	m_wndSpread.SetCellType(CFpspread1::CellTypeStaticText);
	m_wndSpread.SetForeColor(RGB(0,0,255));
	m_wndSpread.SetBackColor(RGB(177,177,177));
	m_wndSpread.SetTypeHAlign(CFpspread1::TypeHAlignCenter);

	m_wndSpread.SetCol(6);
	m_wndSpread.SetText(COleVariant("(Ohm/km)"));
	m_wndSpread.SetCellType(CFpspread1::CellTypeStaticText);
	m_wndSpread.SetForeColor(RGB(0,0,255));
	m_wndSpread.SetBackColor(RGB(177,177,177));
	m_wndSpread.SetTypeHAlign(CFpspread1::TypeHAlignCenter);

	m_wndSpread.SetCol(7);
	m_wndSpread.SetText(COleVariant("[mm]"));
	m_wndSpread.SetCellType(CFpspread1::CellTypeStaticText);
	m_wndSpread.SetForeColor(RGB(0,0,255));
	m_wndSpread.SetBackColor(RGB(177,177,177));
	m_wndSpread.SetTypeHAlign(CFpspread1::TypeHAlignCenter);

	m_wndSpread.SetCol(8);
	m_wndSpread.SetText(COleVariant("[kg/km]"));
	m_wndSpread.SetCellType(CFpspread1::CellTypeStaticText);
	m_wndSpread.SetForeColor(RGB(0,0,255));
	m_wndSpread.SetBackColor(RGB(177,177,177));
	m_wndSpread.SetTypeHAlign(CFpspread1::TypeHAlignCenter);

	if(m_wndCableDataListCtrl.GetItemCount() > 0)
	{
		UINT flag = LVIS_SELECTED | LVIS_FOCUSED;
		m_wndCableDataListCtrl.SetItemState(0 , flag , flag);

		const CString rTableName = m_wndCableDataListCtrl.GetItemText(0,0);
		DisplayCableData(rTableName.operator LPCSTR());
	}

	m_cboCableType.SetCurSel(0);
	OnCbnSelchangeComboCableType();
        SetScrollSetting();

	m_wndCableDataListCtrl.SetFocus();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	CABLE DATA EXCEL FILE을 로딩합니다.

	@author	HumKyung

	@date	????.??.??

	@param

	@return
*/
void CCableDataDlg::OnBnClickedButtonImport()
{
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY  ,"Excel Files(*.xls)|*.xls|All Files(*.*)|*.*||");
	if(IDOK == fileDlg.DoModal())
	{
		CString rFileName    = fileDlg.GetFileName();
		CString strFileName  = fileDlg.GetPathName();
		CString strFileTitle = fileDlg.GetFileTitle();
		CFileException fileException;

		///CFile myFile;
		///if(!myFile.Open(strFileName, CFile::modeRead, &fileException))
		///{
		///	TRACE("Can't open file %s, error = %u\n", strFileName,fileException.m_cause);
		///}
		///myFile.Close();

		if(ERROR_SUCCESS == ImportCableSizingExcelFile(strFileName))
		{
			CELoadDocData& docData = CELoadDocData::GetInstance();

			const int nFound = rFileName.ReverseFind('.');
			if(-1 != nFound) rFileName = rFileName.Left(nFound);
			rFileName.MakeUpper();
			docData.m_ProjectCableDataTable.Save(rFileName.operator LPCSTR());
			CCableDataListTable::GetInstance().AddTable(rFileName.operator LPCSTR() , "");
			DisplayData();
			
			UINT flag = LVIS_SELECTED | LVIS_FOCUSED;
			m_wndCableDataListCtrl.SetItemState(m_wndCableDataListCtrl.GetItemCount() - 1 , flag , flag);
			m_wndCableDataListCtrl.SetFocus();
						
			FillCableTypeAndHertzComboBox();

			///m_cboCableType.SetCurSel(0);
			OnCbnSelchangeComboCableType();
		}
        }
}

/**
	@brief	import Cable Sizing Data Excel.

	@author	HumKyung

	@date	????.??.??

	@param	rFilePath	excel file path

	@return	int
*/
int CCableDataDlg::ImportCableSizingExcelFile(const CString& rFilePath)
{
	BeginWaitCursor();

	CExcelAutomation xlApp;

	//! 프로그레스바 초기화
	CString rTitle = "Excel Open...";
	double dProcess = 0.;
	int process = (int)((dProcess) * 100.);

	//! 엑셀 파일은 오픈한다.
	if(!xlApp.Open(rFilePath))
	{
		AfxMessageBox("Can't Start Excel and get Application object.");
		
		return ERROR_BAD_ENVIRONMENT;
	}

	CELoadDocData& docData = CELoadDocData::GetInstance();
        
        docData.m_ProjectCableDataTable.Clear();

	long lowerRow = 0 , lowerCol = 0 , upperRow = 0 , upperCol = 0;
	xlApp.GetUsedRange(lowerRow , lowerCol , upperRow , upperCol);
	for(int i = 2 ;i  <= upperRow;++i)
	{
                CCableDataByType::Record* pRecord = new CCableDataByType::Record;
		if(pRecord)
		{
			for(int j = lowerCol;j <= upperCol;++j)
			{
				const CString rCellNo = xlApp.MakeExcelNoString(i,j);
				CString rCellValue;
				xlApp.GetCellValue(COleVariant(rCellNo) , rCellValue);

				switch(j)
				{
					case 1:
						pRecord->m_FieldMap[_T("C_HERTZ")] = rCellValue.operator LPCSTR();
						break;
					case 2:
						pRecord->m_FieldMap[_T("C_CONDUCTOR")] = rCellValue.operator LPCSTR();
						break;
					case 3:
						pRecord->m_FieldMap[_T("C_VOLT")] = rCellValue.operator LPCSTR();
						break;
					case 4:
						pRecord->m_FieldMap[_T("C_CABLE_TYPE")] = rCellValue.operator LPCSTR();
						break;
					case 5:
						pRecord->m_FieldMap[_T("C_CORE")] = rCellValue.operator LPCSTR();
						break;
					case 6:
						pRecord->m_FieldMap[_T("C_MPR")] = rCellValue.operator LPCSTR();
						break;
					case 7:
						pRecord->m_FieldMap[_T("C_UG_BASE_TEMP")] = rCellValue.operator LPCSTR();
						break;
					case 8: 
						pRecord->m_FieldMap[_T("C_AG_BASE_TEMP")] = rCellValue.operator LPCSTR();
						break;
					case 9:
						pRecord->m_FieldMap[_T("C_MAX_CONDUCTOR_TEMP")] = rCellValue.operator LPCSTR();
						break;
					case 10:
						pRecord->m_FieldMap[_T("C_SIZE")] = rCellValue.operator LPCSTR();
						break;
					case 11:
						pRecord->m_FieldMap[_T("C_AMP_G")] = rCellValue.operator LPCSTR();
						break;
					case 12:
						pRecord->m_FieldMap[_T("C_AMP_A")] = rCellValue.operator LPCSTR();
						break;
					case 13:
						pRecord->m_FieldMap[_T("C_R")] = rCellValue.operator LPCSTR();
						break;
					case 14:
						pRecord->m_FieldMap[_T("C_X")] = rCellValue.operator LPCSTR();
						break;
					case 15:
						pRecord->m_FieldMap[_T("C_DIA")] = rCellValue.operator LPCSTR();
						break;
					case 16:
						pRecord->m_FieldMap[_T("C_CABLE_WEIGHT")] = rCellValue.operator LPCSTR();
						break;
				}
			}
			docData.m_ProjectCableDataTable.Insert(pRecord);
		}
	}

	EndWaitCursor();
	
/*	CELoadDocData& docData = CELoadDocData::GetInstance();
	string rProjectMDBFilePath = docData.GetProjectMDBFilePath();

	CADODB adoDB;
	CString rDBPath = CString("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=") + rProjectMDBFilePath.c_str();
	//! MDB에 연결한다.

	if(TRUE == adoDB.DBConnect(rDBPath))
	{
		//dProcess++;
		rTitle = "DB Connect....";
		dProcess = 0.35;
		process = (int)((dProcess) * 100.);
		CWorkStatusDlg::UpdateWorkStatus(rTitle , process);

		//! T_LOAD_ITEM 테이블이 존재 하는지 검사한다.
		const CString rTableName = "T_LOAD_ITEM";
		if(TableExist(&adoDB, rTableName))
		{
			//! 테이블이 존재하면 기존의 테이블을 삭제한다.
			if(!DropMdbTable(&adoDB, rTableName)) return IDCANCEL;
		}
		//!  T_LOAD_ITEM_REV 테이블이 존재 하는지 검사한다.
		const CString rRevTableName = "T_LOAD_ITEM_REV";
		if(TableExist(&adoDB, rRevTableName))
		{
			//! 테이블이 존재하면 기존의 테이블을 삭제한다.
			if(!DropMdbTable(&adoDB, rRevTableName)) return IDCANCEL;
		}
		//! 테이블을 새로 만든다. INI 파일을 읽어서 필드를 만든다.
		if(!CreateMdbTable(&adoDB, rDBPath)) return IDCANCEL;

		//! 필수 필드 추가
		if(ERROR_SUCCESS != AppendFieldInDataBase(&adoDB))return IDCANCEL;

		//! Load Item을  DATABASE 에 뿌려준다.
		if(!SetDataToDatabase(&adoDB))  return IDCANCEL;
	}
	else
	{
		xlAutomation.Close();
		throw exception("not found MDB !");
	}
*/
	xlApp.Close();

        return ERROR_SUCCESS;
}

/**
	@brief	보고자하는 Cable Type을 변경했을 때...

	@author	BHK
*/
void CCableDataDlg::OnCbnSelchangeComboCableType()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	const int nSel = m_cboCableType.GetCurSel();
	if(-1 != nSel)
	{
		CString rLBText;
		m_cboCableType.GetLBText(nSel , rLBText);

		CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(rLBText.operator LPCSTR());

		int nHertz = 50;
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_HERTZ);
		if(pComboBox)
		{
			const int nCurSel = pComboBox->GetCurSel();
			if(-1 != nCurSel)
			{
				CString rString;
				pComboBox->GetLBText(nCurSel , rString);
				nHertz = atoi(rString);
			}
		}

		vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz( nHertz );
		if(pRecordList)
		{
			m_wndSpread.SetMaxRows(pRecordList->size() + 1);

			int nRow = 2;
			for(vector<CCableDataByType::Record*>::iterator itr = pRecordList->begin();itr != pRecordList->end();++itr)
			{
				if(itr == pRecordList->begin())
				{
					m_rUGBaseTemp = (*itr)->m_FieldMap[_T("C_UG_BASE_TEMP")].c_str();
					m_rAGBaseTemp = (*itr)->m_FieldMap[_T("C_AG_BASE_TEMP")].c_str();
					m_rMaxConductorTemp = (*itr)->m_FieldMap[_T("C_MAX_CONDUCTOR_TEMP")].c_str();
				}
				m_wndSpread.put_RowHeight(nRow , 13);

				m_wndSpread.SetRow(nRow) , m_wndSpread.SetCol(1);
				m_wndSpread.SetTypeHAlign(CFpspread1::TypeHAlignRight);
				m_wndSpread.SetText(1 , nRow , COleVariant((*itr)->m_FieldMap[_T("C_SIZE")].c_str()));

				m_wndSpread.SetRow(nRow) , m_wndSpread.SetCol(2);
				m_wndSpread.SetTypeHAlign(CFpspread1::TypeHAlignRight);
				m_wndSpread.SetText(2 , nRow , COleVariant((*itr)->m_FieldMap[_T("C_DIA")].c_str()));

				m_wndSpread.SetRow(nRow) , m_wndSpread.SetCol(3);
				m_wndSpread.SetTypeHAlign(CFpspread1::TypeHAlignRight);
				m_wndSpread.SetText(3 , nRow , COleVariant((*itr)->m_FieldMap[_T("C_AMP_G")].c_str()));

				m_wndSpread.SetRow(nRow) , m_wndSpread.SetCol(4);
				m_wndSpread.SetTypeHAlign(CFpspread1::TypeHAlignRight);
				m_wndSpread.SetText(4 , nRow , COleVariant((*itr)->m_FieldMap[_T("C_AMP_A")].c_str()));

				m_wndSpread.SetRow(nRow) , m_wndSpread.SetCol(5);
				m_wndSpread.SetTypeHAlign(CFpspread1::TypeHAlignRight);
				m_wndSpread.SetText(5 , nRow , COleVariant((*itr)->m_FieldMap[_T("C_R")].c_str()));

				m_wndSpread.SetRow(nRow) , m_wndSpread.SetCol(6);
				m_wndSpread.SetTypeHAlign(CFpspread1::TypeHAlignRight);
				m_wndSpread.SetText(6 , nRow , COleVariant((*itr)->m_FieldMap[_T("C_X")].c_str()));

				m_wndSpread.SetRow(nRow) , m_wndSpread.SetCol(7);
				m_wndSpread.SetTypeHAlign(CFpspread1::TypeHAlignRight);
				m_wndSpread.SetText(7 , nRow , COleVariant((*itr)->m_FieldMap[_T("C_DIA")].c_str()));

				m_wndSpread.SetRow(nRow) , m_wndSpread.SetCol(8);
				m_wndSpread.SetTypeHAlign(CFpspread1::TypeHAlignRight);
				m_wndSpread.SetText(8 , nRow , COleVariant((*itr)->m_FieldMap[_T("C_CABLE_WEIGHT")].c_str()));

				++nRow;
			}
		}
		else	m_wndSpread.SetMaxRows(1); 
	}

	UpdateData(FALSE);
}

/**
	@brief	

	@author	BHK
*/
int CCableDataDlg::SaveData(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	///docData.m_ProjectCableDataTable.Save();

	return ERROR_SUCCESS;
}
/*
        @brief  DELETE

        @author KHS
**/
void CCableDataDlg::OnBnClickedButtonDelete()
{
        // TODO: Add your control notification handler code here
        CELoadDocData& docData = CELoadDocData::GetInstance();
	
	POSITION pos = m_wndCableDataListCtrl.GetFirstSelectedItemPosition();
	if(NULL != pos)
	{
		const int nSelItem = m_wndCableDataListCtrl.GetNextSelectedItem(pos);
		if(-1 != nSelItem)
		{
			const CString rTableName = m_wndCableDataListCtrl.GetItemText(nSelItem ,0);
			CCableDataListTable::GetInstance().DeleteTable(rTableName.operator LPCSTR());

			DisplayData();
			const int nItemCount = m_wndCableDataListCtrl.GetItemCount();
			if(nItemCount > 0)
			{
				UINT flag = LVIS_SELECTED | LVIS_FOCUSED;
				m_wndCableDataListCtrl.SetItemState(0 , flag , flag);

				CString rTableName = m_wndCableDataListCtrl.GetItemText(0 , 0);
				DisplayCableData(rTableName.operator LPCSTR());

				m_wndCableDataListCtrl.SetFocus();
			}
			else
			{
				m_wndSpread.SetMaxRows(1);

				m_wndSpread.SetRowHeadersShow(FALSE);
				m_wndSpread.SetMaxCols(7);

				m_wndSpread.SetColHeaderRows(2);
				m_wndSpread.SetRowHeaderCols(0);

				m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
				m_wndSpread.SetCol(1);
				m_wndSpread.SetText(COleVariant("SIZE"));
				m_wndSpread.AddCellSpan(1,0,1,2);

				m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
				m_wndSpread.SetCol(2);
				m_wndSpread.SetText(COleVariant("OVERALL\nDIAMETER"));
				m_wndSpread.AddCellSpan(2,0,1,2);

				m_wndSpread.SetCol(3);
				m_wndSpread.SetText(COleVariant("IN BURIAL\nAMP."));
				m_wndSpread.AddCellSpan(3,0,1,2);

				m_wndSpread.SetCol(4);
				m_wndSpread.SetText(COleVariant("IN AIR\nAMP."));
				m_wndSpread.AddCellSpan(4,0,1,2);

				m_wndSpread.SetCol(5);
				m_wndSpread.SetText(COleVariant("R"));
				m_wndSpread.AddCellSpan(5,0,1,2);

				m_wndSpread.SetCol(6);
				m_wndSpread.SetText(COleVariant("L\n60[Hz]"));
				m_wndSpread.AddCellSpan(6,0,1,2);

				m_wndSpread.SetCol(7);
				m_wndSpread.SetText(COleVariant("L\n50[Hz]"));
				m_wndSpread.AddCellSpan(7,0,1,2);

				m_wndSpread.SetRow(1);
				m_wndSpread.SetCol(1);
				m_wndSpread.SetText(COleVariant("SQMM"));
				m_wndSpread.SetCellType(CFpspread1::CellTypeStaticText);
				m_wndSpread.SetForeColor(RGB(0,0,255));
				m_wndSpread.SetBackColor(RGB(177,177,177));
				m_wndSpread.SetTypeHAlign(CFpspread1::TypeHAlignCenter);

				m_wndSpread.SetCol(2);
				m_wndSpread.SetText(COleVariant("MM"));
				m_wndSpread.SetCellType(CFpspread1::CellTypeStaticText);
				m_wndSpread.SetForeColor(RGB(0,0,255));
				m_wndSpread.SetBackColor(RGB(177,177,177));
				m_wndSpread.SetTypeHAlign(CFpspread1::TypeHAlignCenter);

				m_wndSpread.SetCol(3);
				m_wndSpread.SetText(COleVariant("(A)"));
				m_wndSpread.SetCellType(CFpspread1::CellTypeStaticText);
				m_wndSpread.SetForeColor(RGB(0,0,255));
				m_wndSpread.SetBackColor(RGB(177,177,177));
				m_wndSpread.SetTypeHAlign(CFpspread1::TypeHAlignCenter);

				m_wndSpread.SetCol(4);
				m_wndSpread.SetText(COleVariant("(A)"));
				m_wndSpread.SetCellType(CFpspread1::CellTypeStaticText);
				m_wndSpread.SetForeColor(RGB(0,0,255));
				m_wndSpread.SetBackColor(RGB(177,177,177));
				m_wndSpread.SetTypeHAlign(CFpspread1::TypeHAlignCenter);

				m_wndSpread.SetCol(5);
				m_wndSpread.SetText(COleVariant("(Ohm/km)"));
				m_wndSpread.SetCellType(CFpspread1::CellTypeStaticText);
				m_wndSpread.SetForeColor(RGB(0,0,255));
				m_wndSpread.SetBackColor(RGB(177,177,177));
				m_wndSpread.SetTypeHAlign(CFpspread1::TypeHAlignCenter);

				m_wndSpread.SetCol(6);
				m_wndSpread.SetText(COleVariant("(Ohm/km)"));
				m_wndSpread.SetCellType(CFpspread1::CellTypeStaticText);
				m_wndSpread.SetForeColor(RGB(0,0,255));
				m_wndSpread.SetBackColor(RGB(177,177,177));
				m_wndSpread.SetTypeHAlign(CFpspread1::TypeHAlignCenter);

				m_wndSpread.SetCol(8);
				m_wndSpread.SetText(COleVariant("(Ohm/km)"));
				m_wndSpread.SetCellType(CFpspread1::CellTypeStaticText);
				m_wndSpread.SetForeColor(RGB(0,0,255));
				m_wndSpread.SetBackColor(RGB(177,177,177));
				m_wndSpread.SetTypeHAlign(CFpspread1::TypeHAlignCenter);
			}
		}
	}
}

int CCableDataDlg::SetScrollSetting(void)
{
        //! 엑셀 임포트 후 스크롤 SETTING  0 : 둘다 없음, 1 : 수평 , 2 : 수직 , 3 : 둘다 있음
        long lRow = m_wndSpread.GetMaxRows();
        long lCol = m_wndSpread.GetMaxCols();
        if(lRow < 17 && lCol < 10)
        {
                m_wndSpread.SetScrollBars(0);
        }
        else if(lRow >= 17 && lCol >= 10)
        {
                m_wndSpread.SetScrollBars(3);
        }
        else if(lRow >= 17 && lCol < 10)
        {
                m_wndSpread.SetScrollBars(2);
        }
        else if(lRow < 17 && lCol >= 10)
        {
                m_wndSpread.SetScrollBars(1);
        }
        return 0;
}

/**
	@brief	

	@author	BHK
*/
void CCableDataDlg::OnBnClickedButtonExport()
{
        // TODO: Add your control notification handler code here
        CFileDialog fileDlg(FALSE, NULL, NULL, OFN_OVERWRITEPROMPT  ,"Excel Files(*.xls)|*.xls|All Files(*.*)|*.*||");
	if(IDOK == fileDlg.DoModal())
	{
		CString strFileName  = fileDlg.GetPathName();
		CString strFileTitle = fileDlg.GetFileTitle();
		CFileException fileException;

		///CFile myFile;
		///if(!myFile.Open(strFileName, CFile::modeRead, &fileException))
		///{
		///	TRACE("Can't open file %s, error = %u\n", strFileName,fileException.m_cause);
		///}
		///myFile.Close();

		if(strFileName.Right(4).Find(".xls") == -1) strFileName += _T(".xls");
		
		CString rHertz, rCableType;

		int nSelItem = m_cboCableType.GetCurSel();
		if(-1 != nSelItem)
		{
			m_cboCableType.GetLBText(nSelItem, rCableType);
		}
		nSelItem = m_cboHertz.GetCurSel();
		if(-1 != nSelItem)
		{
			m_cboHertz.GetLBText(nSelItem, rHertz);
		}

		ExportCableSizingExcelFile(strFileName, rHertz, rCableType);
	}
	else
		return;

}

typedef struct tagParam
{
	CSimpleAnimationDlg* pDlg;
	CString rFilePath;
}Param;

static UINT gfncExportCableSizingExcelFile(LPVOID pVoid)
{
	CSimpleAnimationDlg& anidlg = CSimpleAnimationDlg::GetInstance();

	if(NULL == anidlg.GetSafeHwnd())
	{
		anidlg.DoModal();
	}
	
	return ERROR_SUCCESS;
}

int CCableDataDlg::ExportCableSizingExcelFile(const CString& rFilePath, const CString& rHertz, const CString& rCableType)
{
	CWinThread* pThread = AfxBeginThread(gfncExportCableSizingExcelFile , NULL , THREAD_PRIORITY_NORMAL,0);

	python.CallPyExportCableSizingExcelFile(rFilePath, rHertz, rCableType);

	if(pThread)
	{
		CSimpleAnimationDlg& anidlg = CSimpleAnimationDlg::GetInstance();
		if(anidlg.GetSafeHwnd()) anidlg.SendMessage(WM_COMMAND , IDOK);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
int CCableDataDlg::LoadData(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	TABLES::CELOADTable* pTable = docData.GetTableOf(CProjectSettingTable::GetTableName());
	if(pTable)
	{
		const string rCableSizingSettingTableName = pTable->m_RecordEntry[0].FieldValueMap[_T("C_CUR_CABLE_SIZING_SETTING_TABLE_NAME")];

		CCableDataListTable::GetInstance().Load();
		CCableSizeSettingTable& CableSizeSettingTable = CCableSizeSettingTable::GetInstance();
		CableSizeSettingTable.Load();
		
		if(!CableSizeSettingTable.GetCableDataTableName(rCableSizingSettingTableName.c_str()).IsEmpty())
		{
			docData.m_ProjectCableDataTable.Load(CableSizeSettingTable.GetCableDataTableName(rCableSizingSettingTableName.c_str()).operator LPCSTR());
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief

	@author	BHK
*/
int CCableDataDlg::DisplayData(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	m_wndCableDataListCtrl.DeleteAllItems();	//! 기존의 항목을 삭제한다.

	CCableDataListTable& CableDataListTable = CCableDataListTable::GetInstance();
	const size_t nRecordCount = CableDataListTable.GetRecordCount();
	for(int i = 0;i < (int)nRecordCount;++i)
	{
		TABLES::CCableDataListTable::TABLE_RECORD* pRecord = CableDataListTable.GetRecordAt(i);
		if(pRecord)
		{
			const int nItem = m_wndCableDataListCtrl.InsertItem(i , pRecord->rTableNo.c_str());
			if(-1 != nItem) m_wndCableDataListCtrl.SetItemText(nItem , 1 , pRecord->rTableDesc.c_str());
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	CABLE TABLE을 표기합니다.

	@author	BHK
*/
int CCableDataDlg::DisplayCableData(const string& rTableName , const bool bForce)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	if(ERROR_SUCCESS == docData.m_ProjectCableDataTable.Load(rTableName , bForce))
	{
		TABLES::CELOADTable* pTable = docData.GetTableOf(CProjectSettingTable::GetTableName());
		if(pTable)
		{
			const string rCableSizingSettingTableName = pTable->m_RecordEntry[0].FieldValueMap[_T("C_CUR_CABLE_SIZING_SETTING_TABLE_NAME")];

			CCableSizeSettingTable& CableSizeSettingTable = CCableSizeSettingTable::GetInstance();
			CableSizeSettingTable.SetCableDataTableName(rCableSizingSettingTableName.c_str() , rTableName.c_str());
			CableSizeSettingTable.Save();

			FillCableTypeAndHertzComboBox();

			OnCbnSelchangeComboCableType();

			return ERROR_SUCCESS;
		}
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author	BHK
*/
int CCableDataDlg::FillCableTypeAndHertzComboBox(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	vector<string> CableTypeList;
	docData.m_ProjectCableDataTable.GetCableTypeKeyList(CableTypeList);	

	{
		m_cboHertz.ResetContent();

		set<int> HertzSet;
		m_cboHertz.ResetContent();
		for(vector<string>::iterator itr = CableTypeList.begin();itr != CableTypeList.end();++itr)
		{
			CCableDataByType* pCableDataType = docData.m_ProjectCableDataTable.GetRecordListOf(*itr);
			if(pCableDataType)
			{
				vector<int> HertzEntry;
				pCableDataType->GetHertzList(HertzEntry);
				for(vector<int>::iterator jtr = HertzEntry.begin();jtr != HertzEntry.end();++jtr) HertzSet.insert(*jtr);
			}
		}

		for(set<int>::iterator jtr = HertzSet.begin();jtr != HertzSet.end();++jtr)
		{
			CString rString;
			rString.Format("%d" , *jtr);
			m_cboHertz.AddString(rString);
		}

		m_cboHertz.SetCurSel(0);
		OnCbnSelchangeComboHertz();
	}

	/*m_cboCableType.ResetContent();

	for(vector<string>::iterator itr = CableTypeList.begin();itr != CableTypeList.end();++itr)
	{
		m_cboCableType.AddString(itr->c_str());
	}
	m_cboCableType.SetCurSel(0);*/

	return ERROR_SUCCESS;
}

/**
	@brief	VK_DELETE키를 눌렀을때 선택된 TABLE을 삭제하도록 한다.

	@author	BHK
*/
void CCableDataDlg::OnLvnKeydownListCableData(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	
	POSITION pos = m_wndCableDataListCtrl.GetFirstSelectedItemPosition();
	if(NULL != pos)
	{
		const int nSelItem = m_wndCableDataListCtrl.GetNextSelectedItem(pos);
		if(-1 != nSelItem)
		{
			const CString rTableName = m_wndCableDataListCtrl.GetItemText(nSelItem , 0);
			
			if(IDYES == AfxMessageBox("데이터베이스에서 테이블이 삭제됩니다. 계속 하시겠습니까?" , MB_YESNO))
			{
				CCableDataListTable& CableDataListTable = CCableDataListTable::GetInstance();
				if(ERROR_SUCCESS == CableDataListTable.DeleteTable(rTableName.operator LPCSTR()))
				{
					m_wndCableDataListCtrl.DeleteItem(nSelItem);
					if(m_wndCableDataListCtrl.GetItemCount() > 0)
					{
						m_wndCableDataListCtrl.SetItemState(0 , 0, LVIS_SELECTED);
						m_wndCableDataListCtrl.SetSelectionMark(0);
						const CString rNewTableName = m_wndCableDataListCtrl.GetItemText(0 , 0);

						BeginWaitCursor();
						DisplayCableData(rNewTableName.operator LPCSTR() , rNewTableName != rTableName);
						EndWaitCursor();
					}
					else
					{
						CELoadDocData& docData = CELoadDocData::GetInstance();
						docData.m_ProjectCableDataTable.Clear();
					}
				}
			}
		}
	}
	
	*pResult = 0;
}

/**
	@brief	

	@author	BHK
*/
void CCableDataDlg::OnLvnItemchangedListCableData(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	CELoadDocData& docData  = CELoadDocData::GetInstance();
	const string rTableName = docData.m_ProjectCableDataTable.GetTableName();

	POSITION pos = m_wndCableDataListCtrl.GetFirstSelectedItemPosition();
	if(NULL != pos)
	{
		const int nSelItem = m_wndCableDataListCtrl.GetNextSelectedItem(pos);
		if(-1 != nSelItem)
		{
			const CString rNewTableName = m_wndCableDataListCtrl.GetItemText(nSelItem , 0);
			if(rNewTableName != CString(rTableName.c_str()))
			{
				BeginWaitCursor();

				DisplayCableData(rNewTableName.operator LPCSTR() , rNewTableName != CString(rTableName.c_str()));

				EndWaitCursor();
			}
		}
	}

	*pResult = 0;
}

/**
	@brief	

	@author	BHK
*/
BOOL CCableDataDlg::PreTranslateMessage(MSG* pMsg)
{
	if(m_pTT) m_pTT->RelayEvent((LPMSG)pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}

/**
	@brief	HERTZ COMBO BOX SELCHANGE

	@author KHS	

	@date 2009-07-10 오후 4:00:45	

	@param	

	@return		
**/
void CCableDataDlg::OnCbnSelchangeComboHertz()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	
	CString rHertz;

	const int nItem = m_cboHertz.GetCurSel();
	if(-1 != nItem)
	{
		m_cboHertz.GetLBText(nItem, rHertz);
		
		const int nHertz = atoi(rHertz);
		vector<string> CableTypeList;
		docData.m_ProjectCableDataTable.GetCableTypeKeyListOfHertz(CableTypeList, nHertz);	

		m_cboCableType.ResetContent();

		for(vector<string>::iterator itr = CableTypeList.begin();itr != CableTypeList.end();++itr)
		{
			m_cboCableType.AddString(itr->c_str());
		}
		m_cboCableType.SetCurSel(0);
		OnCbnSelchangeComboCableType();
	}
}
