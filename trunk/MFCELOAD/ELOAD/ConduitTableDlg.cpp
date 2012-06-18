// ConduitTablePage.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "ConduitTableDlg.h"
#include <util/excel9/ExcelAutomation.h>
#include "ELoadDocData.h"

#define	T_CONDUIT_LIST	"T_CONDUIT_LIST"

// CConduitTableDlg dialog

IMPLEMENT_DYNAMIC(CConduitTableDlg, CDialog)

CConduitTableDlg::CConduitTableDlg()
	: CDialog(CConduitTableDlg::IDD)
{

}

CConduitTableDlg::~CConduitTableDlg()
{
}

void CConduitTableDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_LIST_CONDUIT_TABLE , m_wndConduitTableListCtrl);
	DDX_Control(pDX , IDC_FPSPREAD_CONDUIT_TABLE , m_wndCONDSpread);

	DDX_Control(pDX , IDC_BUTTON_ADD_CONDUIT_TABLE    , m_wndAddButton);
	DDX_Control(pDX , IDC_BUTTON_DELETE_CONDUIT_TABLE , m_wndDelButton);

	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);

	DDX_Text(pDX , IDC_EDIT_TABLE_DESC , m_rTableDesc);
}


BEGIN_MESSAGE_MAP(CConduitTableDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADD_CONDUIT_TABLE, &CConduitTableDlg::OnBnClickedButtonAddConduitTable)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CONDUIT_TABLE, &CConduitTableDlg::OnLvnItemchangedListConduitTable)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_CONDUIT_TABLE, &CConduitTableDlg::OnBnClickedButtonDeleteConduitTable)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CONDUIT_TABLE, &CConduitTableDlg::OnNMClickListConduitTable)
END_MESSAGE_MAP()


// CConduitTableDlg message handlers

void CConduitTableDlg::OnBnClickedButtonAddConduitTable()
{
	UpdateData(TRUE);

	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY  ,"Excel Files(*.xls)|*.xls|All Files(*.*)|*.*||");
	if(IDOK == fileDlg.DoModal())
	{
		CString rFileName        = fileDlg.GetFileName();
		CString strFilePathName  = fileDlg.GetPathName();
		CString strFileTitle     = fileDlg.GetFileTitle();
		CFileException fileException;

		if(ERROR_SUCCESS == ImportConduitTableFromExcelFile(strFilePathName))
		{
			CELoadDocData& docData = CELoadDocData::GetInstance();
			string rCommonLibraryMDBFilePath = docData.GetCommonLibraryMDBFilePath();

			const int nFound = rFileName.ReverseFind('.');
			if(-1 != nFound) rFileName = rFileName.Left(nFound);
			rFileName.MakeUpper();

			//! ELOAD: COMMON LIBRARY MDB에 TABLE 정보를 INSERT한다.
			CConduitListTable::GetInstance().AddTable(rFileName.operator LPCSTR() , m_rTableDesc.operator LPCSTR());
			DisplayData();

			const int nTableCount = m_wndConduitTableListCtrl.GetItemCount();
			if(nTableCount > 0)
			{
				UINT flag = LVIS_SELECTED | LVIS_FOCUSED;
				m_wndConduitTableListCtrl.SetItemState(nTableCount - 1 , flag, flag);

				const CString rTableName = m_wndConduitTableListCtrl.GetItemText(nTableCount - 1 , 0);
				DisplayConduitData(rTableName.operator LPCSTR());

				m_wndConduitTableListCtrl.SetFocus();
			}
		}
	}
}

/**
	@brief	CONDUIT테이블을 생성하여 데이터를 입력한다.

	@author	BHK
*/
int CConduitTableDlg::ImportConduitTableFromExcelFile(const CString& rFilePath)
{
	CExcelAutomation xlApp;

	//! 엑셀 파일은 오픈한다.
	if(!xlApp.Open(rFilePath))
	{
		AfxMessageBox("Can't Start Excel and get Application object.");

		return ERROR_BAD_ENVIRONMENT;
	}

	CELoadDocData& docData = CELoadDocData::GetInstance();
	CConduitTable ConduitTable;

	long lowerRow = 0 , lowerCol = 0 , upperRow = 0 , upperCol = 0;
	xlApp.GetUsedRange(lowerRow , lowerCol , upperRow , upperCol);
	for(int i = 2 ;i  <= upperRow;++i)
	{
		CConduitTable::Record* pRecord = new CConduitTable::Record;
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
						pRecord->CONDUIT = rCellValue;
						break;
					case 2:
						pRecord->DIA = rCellValue;
						break;
				}
			}

			ConduitTable.Insert(pRecord);
		}
	}

	CString rTableName = rFilePath;
	int nFound = rFilePath.ReverseFind('\\');
	if(-1 != nFound) rTableName = rFilePath.Right(rFilePath.GetLength() - nFound - 1);
	nFound = rTableName.ReverseFind('.');
	if(-1 != nFound) rTableName = rTableName.Left(nFound);

	return ConduitTable.Save(rTableName.operator LPCSTR());
}

/**
	@brief	DIALOG 초기화 부분

	@author	BHK
*/
BOOL CConduitTableDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_wndConduitTableListCtrl.InsertColumn(0 , _T("Table Name")  , LVCFMT_LEFT , 200);
	m_wndConduitTableListCtrl.InsertColumn(1 , _T("Table Desc.") , LVCFMT_LEFT , 300);
        m_wndConduitTableListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

	CELoadDocData & docData = CELoadDocData::GetInstance();
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	m_wndAddButton.SetTextHotColor(TextHotColor);
	m_wndDelButton.SetTextHotColor(TextHotColor);
	m_wndCancelButton.SetTextHotColor(TextHotColor);

	m_wndAddButton.SetIcon(IDI_ADD);
	m_wndDelButton.SetIcon(IDI_DEL);
	m_wndCancelButton.SetIcon(IDI_CANCEL_18X18);

	LoadData();

	m_wndCONDSpread.SetMaxRows(2);
	m_wndCONDSpread.put_RowHeight(1,13);
	m_wndCONDSpread.SetText(0,1,COleVariant("SIZE"));
	m_wndCONDSpread.put_RowHeight(2,13);
	m_wndCONDSpread.SetText(0,2,COleVariant("DIA."));
	m_wndCONDSpread.SetMaxCols(1);
	DisplayData();

	if(m_wndConduitTableListCtrl.GetItemCount() > 0)
	{
		UINT flag = LVIS_SELECTED | LVIS_FOCUSED;
		m_wndConduitTableListCtrl.SetItemState(0 , flag, flag);

		const CString rTableName = m_wndConduitTableListCtrl.GetItemText(0 , 0);
		DisplayConduitData(rTableName.operator LPCSTR());
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
        @brief  DATABASE 에서 데이타를 읽어 온다.
        
	@author	BHK
*/
int CConduitTableDlg::LoadData(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	string rCommonLibraryMDBFilePath= docData.GetCommonLibraryMDBFilePath();
	CConduitListTable::GetInstance().Load();
	/*
	///
	{
		CADODB adoDB;
		string rProvider = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + rCommonLibraryMDBFilePath;

		if(TRUE == adoDB.DBConnect(rProvider.c_str()))
		{
			CString rQuery("SELECT * FROM "T_CONDUIT_LIST);
			try
			{ 
				adoDB.OpenQuery(rQuery);

				LONG lRecordCount = 0;
				adoDB.GetRecordCount(&lRecordCount);
				for(int i = 0;i < lRecordCount;++i)
				{
					CString rValue;

					TABLE_RECORD record;
					adoDB.GetFieldValue(i , "C_TABLE_NAME" , &rValue);
					record.rTableNo = rValue.operator LPCSTR();
					adoDB.GetFieldValue(i , "C_TABLE_DESC" , &rValue);
					record.rTableDesc = rValue.operator LPCSTR();

					m_TableRecordEntry.push_back(record);
				}
			}
			catch(_com_error &e)
			{
				CString rMsg;
				rMsg.Format("*** DOG CHECK [ADO DB] => <ExecuteQuery 오류> 설명[%s] !! ***\n", (LPCSTR)e.Description());
				ELOAD_LOG4CXX_WARN(mylogger , rMsg);
			}
		}
		else
		{
			AfxMessageBox("FAIL TO CONNET DATABASE!");
			return ERROR_BAD_ENVIRONMENT;
		}
	}
	*/

	return ERROR_SUCCESS;
}

/**
        @brief  읽어온 데이타를 리스트에 뿌려준다.

        @author	BHK
*/
int CConduitTableDlg::DisplayData(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

        m_wndConduitTableListCtrl.DeleteAllItems();	//! 기존의 항목을 삭제한다.

	CConduitListTable& ConduitListTable = CConduitListTable::GetInstance();
	const size_t nRecordCount = ConduitListTable.GetRecordCount();
	for(int i = 0;i < (int)nRecordCount;++i)
	{
		TABLES::CConduitListTable::TABLE_RECORD* pRecord = ConduitListTable.GetRecordAt(i);
		if(pRecord)
		{
			const int nItem = m_wndConduitTableListCtrl.InsertItem(i , pRecord->rTableNo.c_str());
			if(-1 != nItem) m_wndConduitTableListCtrl.SetItemText(nItem , 1 , pRecord->rTableDesc.c_str());
		}
	}

        UpdateData(FALSE);

	return 0;
}

/**
	@brief	

	@author	BHK
*/
void CConduitTableDlg::OnLvnItemchangedListConduitTable(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	*pResult = 0;
}

/**
	@brief	선택한 CONDUIT TABLE을 삭제한다.

	@author	BHK
*/
void CConduitTableDlg::OnBnClickedButtonDeleteConduitTable()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	POSITION pos = m_wndConduitTableListCtrl.GetFirstSelectedItemPosition();
	if(NULL != pos)
	{
		const int nSelItem = m_wndConduitTableListCtrl.GetNextSelectedItem(pos);
		if(-1 != nSelItem)
		{
			CString rTableName = m_wndConduitTableListCtrl.GetItemText(nSelItem , 0);
			if(ERROR_SUCCESS == CConduitListTable::GetInstance().DeleteTable(rTableName.operator LPCSTR()))
			{
				DisplayData();
				
				const int nTableCount = m_wndConduitTableListCtrl.GetItemCount();
				if(nTableCount > 0)
				{
					UINT flag = LVIS_SELECTED | LVIS_FOCUSED;
					m_wndConduitTableListCtrl.SetItemState(nTableCount - 1 , flag, flag);

					const CString rTableName = m_wndConduitTableListCtrl.GetItemText(nTableCount - 1 , 0);
					DisplayConduitData(rTableName.operator LPCSTR());

					m_wndConduitTableListCtrl.SetFocus();
				}
				else
				{
					m_wndCONDSpread.SetMaxRows(2);
					m_wndCONDSpread.put_RowHeight(1,13);
					m_wndCONDSpread.SetText(0,1,COleVariant("SIZE"));
					m_wndCONDSpread.SetText(1,1,COleVariant(""));
					m_wndCONDSpread.put_RowHeight(2,13);
					m_wndCONDSpread.SetText(0,2,COleVariant("DIA."));
					m_wndCONDSpread.SetText(1,2,COleVariant(""));
					m_wndCONDSpread.SetMaxCols(1);
				}
			}
		}
	}
}

/**
	@brief	

	@author	BHK
*/
void CConduitTableDlg::OnNMClickListConduitTable(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	POSITION pos = m_wndConduitTableListCtrl.GetFirstSelectedItemPosition();
	if(NULL != pos)
	{
		const int nSelItem = m_wndConduitTableListCtrl.GetNextSelectedItem(pos);
		if(-1 != nSelItem)
		{
			CString rTableName = m_wndConduitTableListCtrl.GetItemText(nSelItem , 0);
			DisplayConduitData(rTableName.operator LPCSTR());
		}
	}

	*pResult = 0;
}

/**
	@brief	conduit data를 표기한다.

	@author	BHK
*/
int CConduitTableDlg::DisplayConduitData(const string& rTableName)
{
	CConduitTable conduit;
	conduit.Load(rTableName);

	const int nRecordCount = conduit.GetRecordCount();
	m_wndCONDSpread.SetMaxCols(nRecordCount);
	for(int i = 0;i < nRecordCount;++i)
	{
		CString rString;
		rString.Format("%d" , i + 1);
		m_wndCONDSpread.SetText(i + 1,0,COleVariant(rString));

		m_wndCONDSpread.SetCol(i + 1);
		CConduitTable::Record* pRecord = conduit.GetRecordAt(i);
		if(pRecord)
		{
			m_wndCONDSpread.SetRow(1);
			m_wndCONDSpread.SetTypeHAlign(CFpspread1::TypeHAlignRight);
			m_wndCONDSpread.SetText(i + 1,1,COleVariant(pRecord->CONDUIT.c_str()));
			m_wndCONDSpread.SetRow(2);
			m_wndCONDSpread.SetTypeHAlign(CFpspread1::TypeHAlignRight);
			m_wndCONDSpread.SetText(i + 1,2,COleVariant(pRecord->DIA.c_str()));
		}
	}
        //! 엑셀 임포트 후 스크롤 SETTING  0 : 둘다 없음, 1 : 수평 , 2 : 수직 , 3 : 둘다 있음
        long lRow = m_wndCONDSpread.GetMaxRows();
        long lCol = m_wndCONDSpread.GetMaxCols();
        if(lRow < 3 && lCol < 10)
        {
                m_wndCONDSpread.SetScrollBars(0);
        }
        else if(lRow >= 3 && lCol >= 10)
        {
                m_wndCONDSpread.SetScrollBars(3);
        }
        else if(lRow >= 3 && lCol < 10)
        {
                m_wndCONDSpread.SetScrollBars(2);
        }
        else if(lRow < 3 && lCol >= 10)
        {
                m_wndCONDSpread.SetScrollBars(1);
        }

	return ERROR_SUCCESS;
}