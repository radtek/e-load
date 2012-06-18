// LoadSummaryPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "ELoad.h"
#include "Tables/LoadSummaryRefTable.h"
#include "LoadSummaryPropPage.h"
#include "Tables/ProjectSettingTable.h"
#include "ELoadDocData.h"
#include <util/excel9/ExcelAutomation.h>


static const string __FIELD_NAMES__[] = 
{
	"C_LTG_PF"     , "C_LTG_EFF" , 
	"C_WELDING_PF" , "C_WELDING_EFF" ,
	"C_INST_PF"    , "C_INST_EFF" ,
	"C_COMM_PF"    , "C_COMM_EFF" ,
	"C_OTHERS_PF"  , "C_OTHERS_EFF" ,
	"C_LTG_LF"     , "C_WELDING_LF" ,
	"C_INST_LF"    , "C_COMM_LF" ,
	"C_OTHERS_LF"
};

// CLoadSummaryPropPage dialog

IMPLEMENT_DYNAMIC(CLoadSummaryPropPage, CDialog)

CLoadSummaryPropPage::CLoadSummaryPropPage(CWnd* pParent /*=NULL*/)
: CDialog(CLoadSummaryPropPage::IDD, pParent)
, m_rSelectExcelFileName(_T(""))
,m_rTableDesc(_T(""))
,m_rSelTable(_T(""))
, m_rSelExcelFilePath(_T(""))
{
	//m_psp.dwFlags ^= PSP_HASHELP;
}

CLoadSummaryPropPage::~CLoadSummaryPropPage()
{
}

void CLoadSummaryPropPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TABLE_FOR_LOAD_SUMMARY, m_wndLoadSummaryTableListCtrl);
	DDX_Text(pDX, IDC_EDIT_SELECTED_TABLE_FOR_LOAD_SUMMARY, m_rSelTable);
	DDX_Text(pDX, IDC_EDIT_TABLE_DESC, m_rTableDesc);

	DDX_Control(pDX , IDC_EDIT_LTG_PF , m_wndFilterEdit[0]);
	DDX_Control(pDX , IDC_EDIT_LTG_EFF , m_wndFilterEdit[1]);
	DDX_Control(pDX , IDC_EDIT_WELDING_PF , m_wndFilterEdit[2]);
	DDX_Control(pDX , IDC_EDIT_WELDING_EFF , m_wndFilterEdit[3]);
	DDX_Control(pDX , IDC_EDIT_INST_PF , m_wndFilterEdit[4]);

	DDX_Control(pDX , IDC_EDIT_INST_EFF , m_wndFilterEdit[5]);
	DDX_Control(pDX , IDC_EDIT_COMM_PF , m_wndFilterEdit[6]);
	DDX_Control(pDX , IDC_EDIT_COMM_EFF , m_wndFilterEdit[7]);
	DDX_Control(pDX , IDC_EDIT_OTHERS_PF , m_wndFilterEdit[8]);
	DDX_Control(pDX , IDC_EDIT_OTHERS_EFF , m_wndFilterEdit[9]);

	DDX_Control(pDX , IDC_EDIT_LTG_LF , m_wndFilterEdit[10]);
	DDX_Control(pDX , IDC_EDIT_WELDING_LF , m_wndFilterEdit[11]);
	DDX_Control(pDX , IDC_EDIT_INST_LF , m_wndFilterEdit[12]);
	DDX_Control(pDX , IDC_EDIT_COMM_LF , m_wndFilterEdit[13]);
	DDX_Control(pDX , IDC_EDIT_OTHERS_LF , m_wndFilterEdit[14]);

	DDX_Control(pDX , IDC_BUTTON_ADD_TABLE    , m_wndAddButton);
	DDX_Control(pDX , IDC_BUTTON_DELETE_TABLE , m_wndDelButton);
	
	DDX_Control(pDX, ID_BUTTON_SAVE, m_wndSaveButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);
}


BEGIN_MESSAGE_MAP(CLoadSummaryPropPage, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADD_TABLE, &CLoadSummaryPropPage::OnBnClickedButtonAddTable)
        ON_BN_CLICKED(IDC_BUTTON_DELETE_TABLE, &CLoadSummaryPropPage::OnBnClickedButtonDeleteTable)
        ON_NOTIFY(NM_CLICK, IDC_LIST_TABLE_FOR_LOAD_SUMMARY, &CLoadSummaryPropPage::OnNMClickListTableForLoadSummary)
        ON_NOTIFY(NM_DBLCLK, IDC_LIST_TABLE_FOR_LOAD_SUMMARY, &CLoadSummaryPropPage::OnNMDBClickListTableForLoadSummary)
        ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_TABLE_FOR_LOAD_SUMMARY, &CLoadSummaryPropPage::OnLvnKeydownListTableForLoadSummary)
	ON_BN_CLICKED(ID_BUTTON_SAVE, &CLoadSummaryPropPage::OnBnClickedButtonSave)
END_MESSAGE_MAP()

struct MatchTableId : public binary_function<CLoadSummaryPropPage::TABLE_RECORD,string,bool>
{
        bool operator()(const CLoadSummaryPropPage::TABLE_RECORD lhs , const string& rhs) const
	{
                return (lhs.rTableNo == rhs);
	}
};

/**
	@biref TABLE로 IMPORT할 EXCEL 파일을 선택한다. 
	선택한 EXCEL파일을 읽어 DATABASE에 저장한다.

	@author	BHK, KHS
*/
void CLoadSummaryPropPage::OnBnClickedButtonAddTable()
{
        UpdateData(TRUE);

	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY  ,"Excel Files(*.xls)|*.xls|All Files(*.*)|*.*||");
	if(IDOK == fileDlg.DoModal())
	{
		CString strFileName = fileDlg.GetPathName();
		CString strFileTitle = fileDlg.GetFileTitle();

                m_rSelectExcelFileName = strFileTitle;
                m_rSelExcelFilePath = strFileName;

		CFile myFile;
		CFileException fileException;

		if(!myFile.Open(strFileName, CFile::modeRead, &fileException))
                {
                        TRACE("Can't open file %s, error = %u\n", strFileName,fileException.m_cause);
                }
                myFile.Close();

                BeginWaitCursor();

                m_rSelTable = strFileName;
		CString rTableName;
                if(ERROR_SUCCESS == LoadDataFromExcel(rTableName))
                {
				 //! 선택한 엑셀 파일 이름의 TABLE을 만들고 T_TABLE_FOR_LOAD_SUMMARY TABLE에 이름을 저장한다.
				 CELoadDocData& docData = CELoadDocData::GetInstance();
				 string rProjectMDBFilePath = docData.GetProjectMDBFilePath();

				 //! ELOAD: ELOAD MDB에 PROJECT 정보를 INSERT한다.
				 CADODB adoDB;
				 const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rProjectMDBFilePath + DB_PASSWORD;
				 if(TRUE == adoDB.DBConnect(rConnectionString))
				 {
                                {
                                        CString rQuery = "SELECT C_TABLE_NO_FOR_LOAD_SUMMARY='" + rTableName +  "' FROM T_TABLE_FOR_LOAD_SUMMARY";
                                        if(TRUE == adoDB.OpenQuery(rQuery.operator LPCTSTR()))
                                        {
                                                rQuery = "DELETE FROM T_TABLE_FOR_LOAD_SUMMARY WHERE C_TABLE_NO_FOR_LOAD_SUMMARY='" + rTableName + "'";
                                                adoDB.ExecuteQuery(rQuery.operator LPCTSTR());
                                        }

                                        rQuery = CString("INSERT INTO T_TABLE_FOR_LOAD_SUMMARY (C_TABLE_NO_FOR_LOAD_SUMMARY,C_TABLE_DESC_FOR_LOAD_SUMMARY) VALUES ") +
                                                "('" + rTableName + "','" + m_rTableDesc + "')";
                                        if(FALSE == adoDB.ExecuteQuery(rQuery.operator LPCTSTR()))
                                        {
                                                AfxMessageBox("Fail to insert record");
                                                return;
                                        }
                                        else
                                        {
                                                //! 추가할 테이블이 존재하면 삭제한다.
                                                string rTableNo = rTableName.operator LPCSTR();
                                                list<TABLE_RECORD>::iterator where = find_if(m_TableRecordEntry.begin(), m_TableRecordEntry.end(), bind2nd(MatchTableId(),rTableNo));
                                                if(where != m_TableRecordEntry.end())
                                                {
                                                        m_TableRecordEntry.erase(where);
                                                }

                                                TABLE_RECORD record;
                                                record.rTableNo   = rTableName;
                                                record.rTableDesc = m_rTableDesc;
                                                m_TableRecordEntry.push_back(record);
                                        }
                                }
                        }
                        else
                        {
                                AfxMessageBox("Fail to connect db.");
                                return;
                        }

                        DisplayProjectRecord();
                }

		EndWaitCursor();
        }
        //UpdateData(FALSE);
}

/**
        @brief  엑셀 파일 이름의 테이블을 만든다.

        @author	KHS
*/
int CLoadSummaryPropPage::CreateDbTableOfExcelFileName(const CString& rTableName , const CString& rFieldNameSet, CADODB* pAdoDb)
{
        //! 테이블이 존재하면 삭제한다.
	STRING_T rQuery =  _T("SELECT * FROM ") + rTableName;
	if(TRUE == pAdoDb->OpenQuery(rQuery))
	{
		rQuery ="DROP TABLE " + rTableName;
		if(TRUE == pAdoDb->ExecuteQuery(rQuery))
		{
			//! 현재 DATABASE의 REF TABLE이 삭제가 되면 CLoadSummaryRefTable의 데이터를 제거한다.
			CLoadSummaryRefTable& LoadSummaryRefTable = CLoadSummaryRefTable::GetInstance();
			if(CString(LoadSummaryRefTable.GetTableName().c_str()) == rTableName)
			{
				LoadSummaryRefTable.Clear();
			}
		}
	}

	rQuery = _T("CREATE TABLE ") + STRING_T(rTableName.operator LPCTSTR()) + "( ID IDENTITY PRIMARY KEY," + STRING_T(rFieldNameSet.operator LPCTSTR()) + _T(")");
	if(FALSE == pAdoDb->ExecuteQuery(rQuery))
	{
		AfxMessageBox( _T("Fail to create record") );
		return ERROR_BAD_ENVIRONMENT;
	}

	return ERROR_SUCCESS;
}

/**
	@brief	다이얼로그 초기화 부분.

	@author	BHK
*/
BOOL CLoadSummaryPropPage::OnInitDialog()
{
        CDialog::OnInitDialog();

        m_wndLoadSummaryTableListCtrl.InsertColumn(0 , _T("Table No.")   , LVCFMT_LEFT , 200);
	m_wndLoadSummaryTableListCtrl.InsertColumn(1 , _T("Table Desc.") , LVCFMT_LEFT , 300);
        m_wndLoadSummaryTableListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);

	CELoadDocData & docData = CELoadDocData::GetInstance();
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	m_wndAddButton.SetTextHotColor(TextHotColor);
	m_wndDelButton.SetTextHotColor(TextHotColor);
	m_wndSaveButton.SetTextHotColor(TextHotColor);

	m_wndAddButton.SetIcon(IDI_ADD);
	m_wndDelButton.SetIcon(IDI_DEL);
	m_wndSaveButton.SetIcon(IDI_OK_18X18);
	m_wndCancelButton.SetIcon(IDI_CANCEL_18X18);

	static const CString rToolTip(_T("데이터 형식이 일치하지 않습니다."));
	const COLORREF WavyLineColor = docData.GetColorCode("FILTER_EDIT_WAVY_LINE_COLOR");
	const COLORREF BackFocusColor= docData.GetColorCode("FILTER_EDIT_BACK_FOCUS_COLOR");
	const COLORREF TextErrorColor= docData.GetColorCode("FILTER_EDIT_TEXT_ERROR_COLOR");
	const COLORREF BackErrorColor= docData.GetColorCode("FILTER_EDIT_BACK_ERROR_COLOR");

	const string rNumberRegEx = docData.GetRegExp("NUMBER");
	const int nCount = sizeof(m_wndFilterEdit) / sizeof(m_wndFilterEdit[0]);
	for(int i = 0;i < nCount;++i)
	{
		m_wndFilterEdit[i].SetRegEx(rNumberRegEx.c_str());
		m_wndFilterEdit[i].CreateToolTip(this , rToolTip);
		m_wndFilterEdit[i].SetWavyLineColour (WavyLineColor);
		m_wndFilterEdit[i].SetBackgroundColourFocus (BackFocusColor);
		m_wndFilterEdit[i].SetForegroundColourError (TextErrorColor);
		m_wndFilterEdit[i].SetBackgroundColourError (BackErrorColor);
	}

	LoadData();

        DisplayProjectRecord();

        return TRUE;  // return TRUE unless you set the focus to a control
        // EXCEPTION: OCX Property Pages should return FALSE
}

/**
        @brief  DATABASE 에서 데이타를 읽어 온다.

        @author	KHS
*/
int CLoadSummaryPropPage::LoadData(void)
{
        CELoadDocData& docData = CELoadDocData::GetInstance();
	string rProjectMDBFilePath= docData.GetProjectMDBFilePath();

	///
	{
		CADODB adoDB;
		const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rProjectMDBFilePath + DB_PASSWORD;
		if(TRUE == adoDB.DBConnect(rConnectionString))
		{
			STRING_T rQuery("SELECT * FROM T_TABLE_FOR_LOAD_SUMMARY");
			try
			{ 
				adoDB.OpenQuery(rQuery);

				LONG lRecordCount = 0;
				adoDB.GetRecordCount(&lRecordCount);
				for(int i = 0;i < lRecordCount;++i)
				{
					STRING_T rValue;

					TABLE_RECORD record;
					adoDB.GetFieldValue(i , "C_TABLE_NO_FOR_LOAD_SUMMARY" , &rValue);
					record.rTableNo = rValue;
					adoDB.GetFieldValue(i , "C_TABLE_DESC_FOR_LOAD_SUMMARY" , &rValue);
					record.rTableDesc = rValue;

					m_TableRecordEntry.push_back(record);
				}
			}
			catch(_com_error &e)
			{
				CString rMsg;
				rMsg.Format("*** DOG CHECK [ADO DB] => <ExecuteQuery 오류> 설명[%s] !! ***\n", (LPCSTR)e.Description());
				ELOAD_LOG4CXX_WARN(mylogger , "" , 1);
			}
		}
		else
		{
			AfxMessageBox("FAIL TO CONNET DATABASE!");
			return ERROR_BAD_ENVIRONMENT;
		}
	}

	{
		CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
		///docData.m_ProjectSettingTable.Load(_T("") , CProjectSettingTable::GetTableName());
	
		{
			const size_t nCount = sizeof(__FIELD_NAMES__) / sizeof(__FIELD_NAMES__[0]);
			for(int i = 0;i < nCount;++i)
			{
				string* pValue = pSettingTable->GetFieldValue(__FIELD_NAMES__[i]);
				if(pValue)
				{
					switch(i)
					{
						case 0:
							SetDlgItemText(IDC_EDIT_LTG_PF , pValue->c_str());
							break;
						case 1:
							SetDlgItemText(IDC_EDIT_LTG_EFF , pValue->c_str());
							break;
						case 2:
							SetDlgItemText(IDC_EDIT_WELDING_PF , pValue->c_str());
							break;
						case 3:
							SetDlgItemText(IDC_EDIT_WELDING_EFF , pValue->c_str());
							break;
						case 4:
							SetDlgItemText(IDC_EDIT_INST_PF , pValue->c_str());
							break;
						case 5:
							SetDlgItemText(IDC_EDIT_INST_EFF , pValue->c_str());
							break;
						case 6:
							SetDlgItemText(IDC_EDIT_COMM_PF , pValue->c_str());
							break;
						case 7:
							SetDlgItemText(IDC_EDIT_COMM_EFF , pValue->c_str());
							break;
						case 8:
							SetDlgItemText(IDC_EDIT_OTHERS_PF , pValue->c_str());
							break;
						case 9:
							SetDlgItemText(IDC_EDIT_OTHERS_EFF , pValue->c_str());
							break;
						case 10:
							SetDlgItemText(IDC_EDIT_LTG_LF , pValue->c_str());
							break;
						case 11:
							SetDlgItemText(IDC_EDIT_WELDING_LF , pValue->c_str());
							break;
						case 12:
							SetDlgItemText(IDC_EDIT_INST_LF , pValue->c_str());
							break;
						case 13:
							SetDlgItemText(IDC_EDIT_COMM_LF , pValue->c_str());
							break;
						case 14:
							SetDlgItemText(IDC_EDIT_OTHERS_LF , pValue->c_str());
							break;
					}
				}
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
        @brief  읽어온 데이타를 리스트에 뿌려준다.
        @author	KHS
*/
int CLoadSummaryPropPage::DisplayProjectRecord(void)
{
        m_wndLoadSummaryTableListCtrl.DeleteAllItems();	//! 기존의 항목을 삭제한다.

        /*
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CString rDBPath = docData.GetModulePath();
        rDBPath += "LoadItem.mdb";
	///
	CADODB adoDB;
        string rProvider;
        rProvider = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + rDBPath;

        if(TRUE == adoDB.DBConnect(rProvider.c_str()))
        {
		CString rQuery = "SELECT * FROM T_PROJECT_SETTING";
                if(TRUE == adoDB.OpenQuery(rQuery))
                {
                        LONG lRecordCount;
                        adoDB.GetRecordCount(&lRecordCount);
                        CString rValue;
                        for(int i = 0;i < lRecordCount;i++)
                        {
                                adoDB.GetFieldValue(i , "C_CUR_TABLE_FOR_LOAD_SUMMARY" , &rValue);
                        }
                        m_rSelTable = rValue;
                }
                else
                {
                        AfxMessageBox("Fail to select record");
                        return -1;
                }
        }
        else
        {
                AfxMessageBox("Not Find DB... !");
                return -1;
	}
	*/

	CELoadDocData& docData = CELoadDocData::GetInstance();
	CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
	m_rSelTable = (*(pSettingTable->GetFieldValue(_T("C_CUR_TABLE_FOR_LOAD_SUMMARY")))).c_str();

	int nIndex = 0;
	for(list<TABLE_RECORD>::iterator itr = m_TableRecordEntry.begin();itr != m_TableRecordEntry.end();++itr)
	{
                const int nItem = m_wndLoadSummaryTableListCtrl.InsertItem(nIndex++ , itr->rTableNo.c_str());
                m_wndLoadSummaryTableListCtrl.SetItemText(nItem , 1 , itr->rTableDesc.c_str());
                if(m_rSelTable == itr->rTableNo.c_str())
                {
                        m_wndLoadSummaryTableListCtrl.SetCheck(nItem);
                        m_wndLoadSummaryTableListCtrl.SetItemState(nItem,LVIS_SELECTED ,LVIS_SELECTED);
                }
	}

        UpdateData(FALSE);

	return 0;
}

/**
        @brief  리스트와 DB 에서 테이블을 지운다.
        @author	KHS
*/
void CLoadSummaryPropPage::OnBnClickedButtonDeleteTable()
{
	// TODO: Add your control notification handler code here
	CELoadDocData& docData = CELoadDocData::GetInstance();
	string rProjectMDBFilePath = docData.GetProjectMDBFilePath();

	CString rDBPath = docData.GetModulePath();
	rDBPath += _T("LoadItem.mdb");

	CADODB adoDB;
	const STRING_T rProvider = STRING_T(PROVIDER) + _T("Data Source=") + rProjectMDBFilePath + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rProvider))
	{
                for (int nItem = m_wndLoadSummaryTableListCtrl.GetItemCount() - 1 ; nItem >= 0 ; nItem--)
                {
                        UINT uState = m_wndLoadSummaryTableListCtrl.GetItemState(nItem, LVIS_SELECTED);
                        if ((uState & LVIS_SELECTED) == LVIS_SELECTED)
                        {
                                CString rItemText = m_wndLoadSummaryTableListCtrl.GetItemText(nItem, 0);

                                CString rQuery = "DELETE FROM T_TABLE_FOR_LOAD_SUMMARY WHERE C_TABLE_NO_FOR_LOAD_SUMMARY='";
                                rQuery += (rItemText + "'" );

                                if(FALSE == adoDB.ExecuteQuery(rQuery.operator LPCTSTR()))
                                {
                                        AfxMessageBox("Fail to delete record");
                                        return ;
                                }
                                rQuery = "DROP TABLE " + rItemText;

                                if(FALSE == adoDB.ExecuteQuery(rQuery.operator LPCTSTR()))
                                {
                                        AfxMessageBox("Fail to drop record");
                                        return ;
                                }
                                m_wndLoadSummaryTableListCtrl.DeleteItem(nItem);

                                for(list<TABLE_RECORD>::iterator itr = m_TableRecordEntry.begin();itr != m_TableRecordEntry.end();++itr)
                                {
                                        CString rString = itr->rTableNo.c_str();
                                        CString rProjectNo = rItemText;
                                        if(rProjectNo.MakeUpper() == rString.MakeUpper())
                                        {
                                                m_TableRecordEntry.erase(itr);
                                                break;
                                        }
                                }
                        }
                }
        }
        else
        {
                AfxMessageBox("Fail to connect db.");
                return;
        }
}
/**
        @brief  엑셀을 읽어서 데이타를 DB 에 저장한다.

        @author	KHS
*/
int CLoadSummaryPropPage::LoadDataFromExcel(CString& rTableName)
{
        COleVariant VOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	COleVariant VFalse((short)FALSE);

        CExcelAutomation xlApp;

        if(!xlApp.Open(m_rSelExcelFilePath))
        {
                AfxMessageBox("Can't Start Excel and get Application object.");
                return ERROR_BAD_ENVIRONMENT;
        }

	   //! DB connect
	   CELoadDocData& docData = CELoadDocData::GetInstance();
	   string rProjectMDBFilePath = docData.GetProjectMDBFilePath();
	   CADODB adoDB;
	   const STRING_T rProvider = STRING_T(PROVIDER) + _T("Data Source=") + rProjectMDBFilePath + DB_PASSWORD;
	   if(TRUE == adoDB.DBConnect(rProvider))
	   {
		   long lowerRow = 0, lowerCol = 0, upperRow = 0, upperCol = 0;
		   xlApp.GetUsedRange(lowerRow , lowerCol , upperRow , upperCol);
		   upperCol = (upperCol > lowerCol + 11) ? lowerCol + 11 : upperCol;

		//! TABLE NAME을 설정한다.
		CString rCellNo = xlApp.MakeExcelNoString(2, 4) , rValue;
		xlApp.GetCellValue(COleVariant(rCellNo) , rValue);
		rTableName = rValue;

		CString rFieldNameSet,rFieldName;
		for(int nRow = 4; nRow <= upperRow; ++nRow)
		{
			CString rRowText;
			for(int nCol = lowerCol; nCol <= upperCol; ++nCol)
			{
				rCellNo = xlApp.MakeExcelNoString(nRow, nCol);
				COleVariant CellNum = COleVariant(rCellNo);
				//! 엑셀의 값을 읽어 온다.
				xlApp.GetCellValue(CellNum, rValue);
				if(4 == nRow)
				{
					rFieldNameSet += ( "["+ rValue + "]" + " VARCHAR(50)," );
					rFieldName += ( "["+ rValue + "]" + "," );
				}
				else
				{
					rRowText += ( "'" + rValue + "',");
				}
			}

			if(4 == nRow)
			{
				int at = rFieldNameSet.ReverseFind(',');
				rFieldNameSet = rFieldNameSet.Left(at);
				at = rFieldName.ReverseFind(',');
				rFieldName = rFieldName.Left(at);
				//!엑셀을 읽어 생성된 필드 이름의 필드를 가진 테이블 생성
				if(ERROR_SUCCESS != CreateDbTableOfExcelFileName(rTableName , rFieldNameSet, &adoDB))
				{
					return ERROR_BAD_ENVIRONMENT;
				}
			}
			else
			{
				int at = rRowText.ReverseFind(',');
				rRowText = rRowText.Left(at);
				//! 쿼리문을 날려 DB에 저장한다.
				CString rQuery = "INSERT INTO " + rTableName + "(" + rFieldName + " )VALUES (" + rRowText + ");";
				if(FALSE == adoDB.ExecuteQuery(rQuery.operator LPCTSTR()))
				{
					AfxMessageBox("Fail to insert record");
					return ERROR_BAD_ENVIRONMENT;
				}
			}
		}
	}
	else
	{
		AfxMessageBox("Fail to connect db.");
		return ERROR_BAD_ENVIRONMENT;
	}

	xlApp.Close();
	return ERROR_SUCCESS;
}


/**
        @brief 리스트 아이템을 선택했을 때 이벤트 (체크 박스관련)
        @author KHS
*/
void CLoadSummaryPropPage::OnNMClickListTableForLoadSummary(NMHDR *pNMHDR, LRESULT *pResult)
{
        //LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE>(pNMHDR);
        // TODO: Add your control notification handler code here
        NMLISTVIEW* pNMLV = (NMLISTVIEW*)pNMHDR;

        CPoint ptList= pNMLV->ptAction;

        LVHITTESTINFO htinfo;
        htinfo.pt    = ptList;
        htinfo.flags = LVHT_ONITEMICON;
        int nItem = m_wndLoadSummaryTableListCtrl.HitTest (&htinfo);
        if(-1 != nItem)
        {
                m_wndLoadSummaryTableListCtrl.SetItemState(nItem,LVIS_SELECTED ,LVIS_SELECTED);

                int nCount = m_wndLoadSummaryTableListCtrl.GetItemCount();	
                if(BST_UNCHECKED == m_wndLoadSummaryTableListCtrl.GetCheck(nItem))
                {
                        m_wndLoadSummaryTableListCtrl.SetCheck(nItem);
                        m_rSelTable = m_wndLoadSummaryTableListCtrl.GetItemText(nItem, 0);
                        UpdateData(FALSE);
                }
                for(int nItem = 0; nItem < nCount; nItem++)
                {
                        if( pNMLV->iItem == nItem) continue;
                        ListView_SetCheckState(m_wndLoadSummaryTableListCtrl.GetSafeHwnd(), nItem, FALSE);
                }
                *pResult = 1;
        }
        else
        {
                *pResult = 0;
        }
}
/**
        @brief 리스트 아이템을더블 클릭 했을 때 이벤트 (체크 박스관련)
        @author KHS
*/
void CLoadSummaryPropPage::OnNMDBClickListTableForLoadSummary(NMHDR *pNMHDR, LRESULT *pResult)
{
        NMLISTVIEW* pNMLV = (NMLISTVIEW*)pNMHDR;

        CPoint ptList= pNMLV->ptAction;

        LVHITTESTINFO htinfo;
        htinfo.pt    = ptList;
        htinfo.flags = LVHT_ONITEMICON;
        int nItem = m_wndLoadSummaryTableListCtrl.HitTest (&htinfo);
        if(-1 != nItem)
        {
                m_wndLoadSummaryTableListCtrl.SetItemState(nItem,LVIS_SELECTED ,LVIS_SELECTED);

                int nCount = m_wndLoadSummaryTableListCtrl.GetItemCount();	
                if(BST_UNCHECKED == m_wndLoadSummaryTableListCtrl.GetCheck(nItem))
                {
                        m_wndLoadSummaryTableListCtrl.SetCheck(nItem);
                        m_rSelTable = m_wndLoadSummaryTableListCtrl.GetItemText(nItem, 0);
                        UpdateData(FALSE);
                }
                for(int nItem = 0; nItem < nCount; nItem++)
                {
                        if( pNMLV->iItem == nItem) continue;
                        ListView_SetCheckState(m_wndLoadSummaryTableListCtrl.GetSafeHwnd(), nItem, FALSE);
                }
                *pResult = 1;
        }
        else
        {
                *pResult = 0;
        }
}

/**
	@brief	사용자가 설정한 값들을 저장한다.

	@author	BHK
*/
int CLoadSummaryPropPage::SaveData(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	if(!m_rSelTable.IsEmpty())
	{
		CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
		pSettingTable->SetFieldValue(_T("C_CUR_TABLE_FOR_LOAD_SUMMARY") , m_rSelTable.operator LPCSTR());
	}

	{
		const size_t nCount = sizeof(__FIELD_NAMES__) / sizeof(__FIELD_NAMES__[0]);
		for(int i = 0;i < nCount;++i)
		{
			CString rValue;
			switch(i)
			{
				case 0:
					GetDlgItemText(IDC_EDIT_LTG_PF , rValue);
					break;
				case 1:
					GetDlgItemText(IDC_EDIT_LTG_EFF , rValue);
					break;
				case 2:
					GetDlgItemText(IDC_EDIT_WELDING_PF , rValue);
					break;
				case 3:
					GetDlgItemText(IDC_EDIT_WELDING_EFF , rValue);
					break;
				case 4:
					GetDlgItemText(IDC_EDIT_INST_PF , rValue);
					break;
				case 5:
					GetDlgItemText(IDC_EDIT_INST_EFF , rValue);
					break;
				case 6:
					GetDlgItemText(IDC_EDIT_COMM_PF , rValue);
					break;
				case 7:
					GetDlgItemText(IDC_EDIT_COMM_EFF , rValue);
					break;
				case 8:
					GetDlgItemText(IDC_EDIT_OTHERS_PF , rValue);
					break;
				case 9:
					GetDlgItemText(IDC_EDIT_OTHERS_EFF , rValue);
					break;
				case 10:
					GetDlgItemText(IDC_EDIT_LTG_LF , rValue);
					break;
				case 11:
					GetDlgItemText(IDC_EDIT_WELDING_LF , rValue);
					break;
				case 12:
					GetDlgItemText(IDC_EDIT_INST_LF , rValue);
					break;
				case 13:
					GetDlgItemText(IDC_EDIT_COMM_LF , rValue);
					break;
				case 14:
					GetDlgItemText(IDC_EDIT_OTHERS_LF , rValue);
					break;
			}
			CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
			pSettingTable->SetFieldValue(__FIELD_NAMES__[i].c_str() , rValue.operator LPCSTR());
		}
	}

	CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
	pSettingTable->Save();

	//! PROJECT SETTING TABLE을 저장하고 난후에 호출해야 한다.
	CLoadSummaryRefTable& loadSummaryRefTable = CLoadSummaryRefTable::GetInstance();
	CLoadSummaryRefTable::Load(CString(loadSummaryRefTable.GetTableName().c_str()) != m_rSelTable);

	return ERROR_SUCCESS;
}
/**
	@brief	Project Setting KEYDOWN

	@author	KHS
*/
void CLoadSummaryPropPage::OnLvnKeydownListTableForLoadSummary(NMHDR *pNMHDR, LRESULT *pResult)
{
        LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
        // TODO: Add your control notification handler code here
        *pResult = 0;
         int nCurItem = m_wndLoadSummaryTableListCtrl.GetSelectionMark();
        switch (pLVKeyDow->wVKey)
        {
        case VK_UP:
                {
                        if(nCurItem - 1 != -1)
                        {
                                m_wndLoadSummaryTableListCtrl.SetCheck(nCurItem - 1);
                                m_wndLoadSummaryTableListCtrl.SetItemState(nCurItem - 1,LVIS_SELECTED ,LVIS_SELECTED);
                                m_rSelTable = m_wndLoadSummaryTableListCtrl.GetItemText(nCurItem - 1, 0);
                                UpdateData(FALSE);
                                for(int nItem = 0; nItem < m_wndLoadSummaryTableListCtrl.GetItemCount(); nItem++)
                                {
                                        if( nItem == nCurItem - 1) continue;
                                        ListView_SetCheckState(m_wndLoadSummaryTableListCtrl.GetSafeHwnd(), nItem, FALSE);
                                }
                        }
                }
                break;
        case VK_DOWN:
                {
                        if(nCurItem + 1 < m_wndLoadSummaryTableListCtrl.GetItemCount())
                        {
                                m_wndLoadSummaryTableListCtrl.SetCheck(nCurItem + 1);
                                m_wndLoadSummaryTableListCtrl.SetItemState(nCurItem + 1,LVIS_SELECTED ,LVIS_SELECTED);
                                m_rSelTable = m_wndLoadSummaryTableListCtrl.GetItemText(nCurItem + 1, 0);
                                UpdateData(FALSE);
                                for(int nItem = 0; nItem < m_wndLoadSummaryTableListCtrl.GetItemCount(); nItem++)
                                {
                                        if( nItem == nCurItem + 1) continue;
                                        ListView_SetCheckState(m_wndLoadSummaryTableListCtrl.GetSafeHwnd(), nItem, FALSE);
                                }
                        }
                }
                break;
        }
}

/**
        @brief  현재 프로젝트 셋팅을 선택한다.

        @author KHS 
*/
void CLoadSummaryPropPage::OnBnClickedButtonSave()
{
	SaveData();
	OnOK();
}
