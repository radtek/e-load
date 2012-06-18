// ExcelImportDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ELoad.h"
#include "WorkStatusDlg.h"
#include "ExcelImportDlg.h"
#include <vector>
#include <Tokenizer.h>
#include <isstring.h>
#include <string>
#include "ExcelMapingGridCell.h"
#include "ELoadItem.h"
#include "ELoadDocData.h"
#include "pyfromc.h"
#include "MainFrm.h"
#include "enablebuddybutton.h"

using namespace std;

// CExcelImportDlg dialog
#define	READ_ONLY_COLOR	RGB(220, 220, 220)

IMPLEMENT_DYNAMIC(CExcelImportDlg, CDialog)

CExcelImportDlg::CExcelImportDlg(CWnd* pParent /*=NULL*/)
: CDialog(CExcelImportDlg::IDD, pParent)
, m_sStartRow(_T("4"))
, rFileName(_T(""))
, m_rFieldName(_T(""))
, IsDBOpen(FALSE)
{
	m_nCountOfImport = 0L;
	m_nCountOfImportFail = 0L;
}

CExcelImportDlg::~CExcelImportDlg()
{
}

void CExcelImportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX , IDC_DIR_EDIT , m_wndEditBrowseCtrl);
	DDX_Text(pDX, IDC_ROWCOUNT_EDIT, m_sStartRow);

	DDX_Control(pDX , IDOK , m_wndOKButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);
}


BEGIN_MESSAGE_MAP(CExcelImportDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CExcelImportDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// CExcelImportDlg message handlers

/**
	@brief  Excel을 읽어서 LOAD DATA를 로드한다..

	@author	KHS,BHK

	@parm	xlApp

	@return	int
*/
int CExcelImportDlg::ImportLoadDataFromExcel(CExcelAutomation& xlApp)
{
	m_nCountOfImport = 0;
	m_nCountOfImportFail = 0;

	CELoadDocData& docData = CELoadDocData::GetInstance();

	CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_UPDATE_LOADS);
	const bool bNewLoads = (pButton && pButton->GetCheck()) ? false : true;
	if(bNewLoads) docData.ClearELoadItemListOf(CLoadItem::TypeString());	//! 기존의 Load Item들을 삭제한다.

	CELoadItemProp* pLoadProp = docData.m_ItemPropMap[CLoadItem::TypeString()];

	CString rTitle = _T("Read Load Excel....");
	int progress = 0;
	CWorkStatusDlg::UpdateWorkStatus(rTitle , progress);

	//!  Get Excel Row Count, Column Count
	long nMinX = 0, nMinY = 0, nMaxX = 0, nMaxY = 0; 
	xlApp.GetUsedRange(nMinY, nMinX, nMaxY, nMaxX);
	const int nRowCount = nMaxY;

	COleVariant VOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	COleVariant VFalse((short)FALSE);
	
	m_nTryCount = 0, m_nSuccessCount = 0 , docData.m_nErrorCount = 0;
	const int iRowStart = atoi(m_sStartRow);
	
	vector<CLoadItem*> NeedPostProcess;
	map<pair<string,string> ,string> FieldValueMap;	//! 필드 이름과 값을 저장하는 Map
	//! Insert Item
	int nIndex = 0;
	for(int nRow = iRowStart; nRow <= nRowCount;++nRow)
	{
		//! 필드 이름과 값을 저장하는 Map을 초기화 시킨다.
		FieldValueMap.clear();
		//! read and store data.
		CString rField , rFieldNameSet , rTotalFieldValue;
		for(vector<EXCEL_MAPPING_RECORD>::iterator itr = m_FieldEntry.begin(); itr != m_FieldEntry.end();++itr)
		{
			const CString rFieldName = itr->rField.c_str();
			CString rTotalValue;
			for(int i =0;i < sizeof(itr->aryExlCell) / sizeof(itr->aryExlCell[0]);++i)
			{
				CString rCol = itr->aryExlCell[i].c_str();
				if(rCol.IsEmpty()) continue;
				CString rValue, rResult;
				if(isalpha((int)rCol[0]))
				{
					rResult.Format(_T("%s%d") , rCol, nRow);
					COleVariant CellNum = COleVariant(rResult);
					xlApp.GetCellValue(CellNum, rValue);
					if(_T("-") == rValue.Trim()) rValue = _T("");
				}
				else
				{
					rValue = rCol;
				}
				rTotalValue += rValue;
			}
			if(',' == rTotalValue.Right(1))
			{
				int at = rTotalValue.ReverseFind(',');
				rTotalValue = rTotalValue.Left(at);
			}
			vector<string> oResult;
			CTokenizer<CIsComma>::Tokenize( oResult , rFieldName.operator LPCSTR() , CIsComma());

			FieldValueMap.insert(make_pair(make_pair(oResult[0] , oResult[1]) , rTotalValue.operator LPCSTR()));
			rTotalFieldValue += rTotalValue;
		}

		//! break loop if all fields are blank.
		if(rTotalFieldValue.IsEmpty()) break;
		::OutputDebugString(rTotalFieldValue);
		++m_nTryCount;

		CLoadItem* pLoadItem = NULL;
		//! Update인 경우...
		if(false == bNewLoads)
		{
			//! Update인 경우에 기존에 같은 이름의 Load Item이 있는지 확인한다. 있으면 새롭게 기존의 것을 사용하도록 한다.
			const string rValue = FieldValueMap[make_pair(_T("General") , _T("ITEM ID"))];
			pLoadItem = docData.FindLoadItemByName(rValue);
		}
		if(NULL == pLoadItem)
		{
			map<pair<string,string> , string>::iterator where1 = FieldValueMap.find(make_pair(_T("General") , _T("ITEM ID")));
			if((where1 == FieldValueMap.end()) || where1->second.empty())
			{
				ELOAD_LOG4CXX_WARN(mylogger , _T("") , 49);
				++m_nCountOfImportFail;
				continue;
			}

			where1 = FieldValueMap.find(make_pair(_T("Load") , _T("BHP <1>")));
			map<pair<string,string> , string>::iterator where2 = FieldValueMap.find(make_pair(_T("Rating") , _T("Rating Capacity")));
			if((where1 != FieldValueMap.end()) && (where1 != FieldValueMap.end()))
			{
				const double nBHP = atof(where1->second.c_str());
				const double nRating = atof(where2->second.c_str());
				//! BHP 값이 RATING 보다 크면 에러
				if(nBHP > nRating)
				{
					const string rLoadID = FieldValueMap[make_pair(_T("General") , _T("ITEM ID"))];
					ELOAD_LOG4CXX_WARN(mylogger , rLoadID , 33);
					++m_nCountOfImportFail;
					continue;
				}
			}
			
			//! error check
			bool bError = false;
			for(map<pair<string,string> , string>::iterator jtr = FieldValueMap.begin();jtr != FieldValueMap.end();++jtr)
			{
				const string rCategory = jtr->first.first;
				const string rKey = jtr->first.second;
				const string rValue = jtr->second;

				const int nResult = LoadImportErrorCheck(rCategory, rKey, rValue);
				if(ERROR_SUCCESS != nResult)
				{
					const string rLoadID = FieldValueMap[make_pair(_T("General") , _T("ITEM ID"))];
					ELOAD_LOG4CXX_ERROR(mylogger , rLoadID , nResult);
					bError = true;
					break;
				}
			}
			if(bError)
			{
				++m_nCountOfImportFail;
				continue;
			}

			if(!bError)
			{
				//! NOTICE : LoadItem 생성과 함께 docData에 저장된다.
				if(pLoadItem = static_cast<CLoadItem*>(docData.CreateELoadItemOf(CLoadItem::TypeString())))
				{
					//! IMPORT된 ITEM의 갯수를 증가시킨다.
					++m_nCountOfImport;

					//! set load properties
					for(map<pair<string,string> , string>::iterator jtr = FieldValueMap.begin();jtr != FieldValueMap.end();++jtr)
					{
						const string rCategory = jtr->first.first;
						const string rKey = jtr->first.second;
						const string rValue = jtr->second;

						pLoadItem->prop()->SetValue( rCategory , rKey ,  rValue);
						pLoadItem->prop()->SetOriginalValue( rCategory , rKey ,  rValue);
					}
										
					NeedPostProcess.push_back(pLoadItem);
					++m_nSuccessCount;
				}
				else
				{
					const string rLoadID = FieldValueMap[make_pair(_T("General") , _T("ITEM ID"))];
					ELOAD_LOG4CXX_ERROR(mylogger , rLoadID , 29);
					++m_nCountOfImportFail;
				}
			}
		}
		else
		{
			const string rLoadID = FieldValueMap[make_pair(_T("General") , _T("ITEM ID"))];
			ELOAD_LOG4CXX_ERROR(mylogger , rLoadID , 31);
			++m_nCountOfImportFail;
		}

		double dProcess = double(nRow - iRowStart) / double(nRowCount);
		int process = (int)((dProcess) * 100.);
		CWorkStatusDlg::UpdateWorkStatus(rTitle , process);
	}

	//! 로드 데이터를 IMPORT하고 난뒤의 후속 작업을 수행한다.
	python.CallPyPostImportLoadDataFromExcel(NeedPostProcess);

	return ERROR_SUCCESS;
}

/**
	@brief  Excel에서 Load Item을 읽어 Database에 저장한다.

	@author	KHS
*/
UINT CExcelImportDlg::StatusThread()
{
	CWorkStatusDlg* pWorkStatusDlg = CWorkStatusDlg::GetInstance();
	try
	{
		InterlockedExchange((LONG*)(&(pWorkStatusDlg->m_bThreadRunning)) , TRUE);

		CExcelAutomation xlApp;
		
		//! LOG
		ELOAD_LOG4CXX_RESET(mylogger, _T("Excel Import Error Messages"));

		//! 엑셀 파일은 오픈한다.
		if(!xlApp.Open(m_sExcelFilePath))
		{
			AfxMessageBox(_T("Can't Start Excel and get Application object."));
			return IDCANCEL;
		}
		xlApp.SetActiveSheet( 1 ); // LOAD Sheet

		//! 엑셀을 읽어서 메모리로 부른다.
		ImportLoadDataFromExcel(xlApp);

		CELoadDocData& docData = CELoadDocData::GetInstance();
		STRING_T rProjectMDBFilePath = docData.GetProjectMDBFilePath();

		CADODB adoDB;
		const STRING_T rDBPath = STRING_T(PROVIDER) + _T("Data Source=") + rProjectMDBFilePath + DB_PASSWORD;
		if(TRUE == adoDB.DBConnect(rDBPath))	//! MDB에 연결한다.
		{
			//! T_LOAD_ITEM 테이블이 존재 하는지 검사한다.
			if(TableExist(&adoDB, CString(CLoadItem::TableName().c_str())))
			{
				//! 테이블이 존재하면 기존의 테이블을 삭제한다.
				if(!DropMdbTable(&adoDB, CString(CLoadItem::TableName().c_str()))) return IDCANCEL;
			}
			//!  T_LOAD_ITEM_REV 테이블이 존재 하는지 검사한다.
			const CString rRevTableName = CString(CLoadItem::TableName().c_str()) + "_REV";
			if(TableExist(&adoDB, rRevTableName))
			{
				//! 테이블이 존재하면 기존의 테이블을 삭제한다.
				if(!DropMdbTable(&adoDB, rRevTableName)) return IDCANCEL;
			}
			//! 테이블을 새로 만든다. 
			docData.CreateTableFromProp(adoDB , CLoadItem::TableName() , CLoadItem::TypeString());
			docData.CreateTableFromProp(adoDB , CLoadItem::TableName() + _T("_REV"), CELoadItem::TypeString());

			//! Load Item을  DATABASE에 저장한다.
			if(!SetDataToDatabase(&adoDB))  return IDCANCEL;
		}
		else
		{
			xlApp.Close();
			throw exception("not found MDB !");
		}

		xlApp.Close();

		adoDB.DBDisConnect();

		InterlockedExchange((LONG*)(&(pWorkStatusDlg->m_bThreadRunning)) , FALSE);
		//////////////////////////////////////////////////////////////////////////
		/// close work status dialog
		pWorkStatusDlg->SendMessage(WM_COMMAND , IDOK );
	}
	catch(exception& e)
	{
		AfxMessageBox(e.what());

		InterlockedExchange((LONG*)(&(pWorkStatusDlg->m_bThreadRunning)) , FALSE);

		if(pWorkStatusDlg) pWorkStatusDlg->SendMessage(WM_COMMAND , IDCANCEL );

		return IDCANCEL;
	}

	return IDOK;
}

/**
	@brief	

	@author BHK	

	@date 2009-08-26 오후 12:56:58	

	@param	

	@return		
*/
UINT CExcelImportDlg::StatusThreadEntry(LPVOID pVoid)
{       
	return ((CExcelImportDlg*)pVoid)->StatusThread();
}

/**
	@brief	Excel에서 Load Item을 import한다.

	@author	HumKyung

	@date	????.??.??

	@param

	@return
*/
void CExcelImportDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	m_wndEditBrowseCtrl.GetWindowText(m_sExcelFilePath);
	
	WriteExcelMappingInMDB();

	if(!CheckProgram())
	{
		//! 실행 취소
		return OnCancel();
	}

	CWorkStatusDlg dlg;
	try
	{
		this->ShowWindow(FALSE);

		dlg.m_pThread = AfxBeginThread(StatusThreadEntry, this, THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
		if (NULL == dlg.m_pThread)
		{
			AfxMessageBox("Can't create thread!!!");
		}
		else
		{
			//! Load관련 문서들을 모두 닫는다.
			theApp.CloseDocument(CLoadItem::TypeString());
			if(IDOK == dlg.DoModal())
			{
				/*CString rMsg;
				rMsg.Format("SUCCESS!! \n\nTotal %d items are Imported.\n Total %d items are Failed." , m_nCountOfImport, m_nCountOfImportFail);
				AfxMessageBox(rMsg);*/
				OnOK();
			}
			else
			{
				OnCancel();
			}
		}
		this->ShowWindow(TRUE);
	}
	catch(exception& e)
	{
		this->ShowWindow(TRUE);
		AfxMessageBox(e.what());
		return OnCancel();
	}
}
/**
	@brief  : 테이블 존재 여부를 가린다.
	
	@return : 테이블이 존재하면 True  없으면  false
	
	@author	KHS
*/
BOOL CExcelImportDlg::TableExist(CADODB* pAdoDb, const CString rTableName)
{
	assert(pAdoDb && "pAdoDb is NULL");

	if(pAdoDb)
	{
		const CString rExistTable("Select * From " +  rTableName);
		return pAdoDb->OpenQuery(rExistTable.operator LPCTSTR());
	}

	return FALSE;
}

/**
        @brief  return Count of Colume

        @author	KHS

	@date	????.??.??

	@param	xlApp
	@param	nStartColumn

	@return	int
*/
int CExcelImportDlg::GetColumnCount(CExcelAutomation& xlApp, const int& nStartColumn)
{
	int nColCount = 0;
	int nCol = nStartColumn;
	CString rValue1 = _T("");
	const int nRowStart = atoi(m_sStartRow);
	while(1)
	{
		CString rResult = xlApp.MakeExcelNoString( nRowStart , nCol);
		COleVariant CellNum1 = COleVariant(rResult);
		xlApp.GetCellValue(CellNum1, rValue1);
		if( "-" == rValue1 && nStartColumn != nColCount) break;
		nColCount++;
		nCol++;
	}
	return nColCount;
}

/**
	@brief  return Count of Row
	@author	KHS
*/
int CExcelImportDlg::GetRowCount(CExcelAutomation& xlApp, const int& nStartRow, const int& nCol)
{
	int nRowCount = 0;
	int nRow = nStartRow;
	CString rValue = _T("");
	while(1)
	{
		CString rResult = xlApp.MakeExcelNoString( nRow, nCol);
		COleVariant CellNum = COleVariant(rResult);
		xlApp.GetCellValue(CellNum, rValue);
		if( "-" == rValue && nStartRow != nRow ) break;
		nRowCount++;
		nRow++;
	}
	return nRowCount;
}

/**
        @brief  Drop Mdb Table
        @author	KHS
*/

bool CExcelImportDlg::DropMdbTable(CADODB* pAdoDb, const CString& rTableName)
{
        CString rQuery( _T("DROP TABLE ") + rTableName);
        try
        {
                pAdoDb->ExecuteQuery(rQuery.operator LPCTSTR());
        }
        catch(_com_error &e)
        {
                CString rMsg;
                rMsg.Format("*** DOG CHECK [ADO DB] => <ExecuteQuery 오류> 설명[%s] !! ***\n", (LPCSTR)e.Description());
                AfxMessageBox(rMsg);
                return false;
        }
        return true;
}

/**
	@brief  Check Program

	@author	KHS
*/
bool CExcelImportDlg::CheckProgram(void)
{
	this->ShowWindow(FALSE);
	if(m_sStartRow.IsEmpty())
	{
		AfxMessageBox(_T("Select Start Row Count"));
		this->ShowWindow(TRUE);
		return false;
	}
	if(m_sExcelFilePath.IsEmpty()) 
	{
		AfxMessageBox(_T("Select [*.xls] File, in first !"));
		this->ShowWindow(TRUE);
		return false;
	}

	//! New Loads 옵션일 경우에 진행 여부를 물어보도록 한다.
	CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_UPDATE_LOADS);
	const bool bNewLoads = (pButton && pButton->GetCheck()) ? false : true;
	if(bNewLoads)
	{
		if( IDYES != MessageBox("기존의 데이타베이스가 삭제 됩니다. 계속 진행하시겠습니까? ", "Warning!", MB_YESNO|MB_ICONWARNING ))
		{
			AfxMessageBox(_T(" Exit!! [EXCEL IMPORT]"));
			this->ShowWindow(TRUE);
			return false;
		}
	}

	this->ShowWindow(TRUE);
	return true;
}

/**
	@brief  LOAD DATA를 MDB 에 넣는다.

	@author	KHS
*/
bool CExcelImportDlg::SetDataToDatabase(CADODB* pAdoDb)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	//! 프로그레스바 초기화
	CString rTitle = _T("Save Data...");
	double dProcess = 0.;
	int process = (int)((dProcess) * 100.);
	CWorkStatusDlg::UpdateWorkStatus(rTitle , process);

	int nIndex = 0;
	list<CELoadItem*> LoadItemList;
	docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
	////if(pLoadItemList)
	{
		for(list<CELoadItem*>::iterator itr = LoadItemList.begin(); itr != LoadItemList.end();++itr)
		{
			CLoadItem* pLoadItem = static_cast<CLoadItem*>(*itr);
			if(pLoadItem)
			{
				CELoadItemProp* pProp = pLoadItem->prop();
				const size_t nCategoryCount = pProp->GetCategoryCount();
				CString rFieldNameSet, rField;

				//! C_ID에 m_rId를 넣는다
				const CString rId = pLoadItem->GetId().c_str();

				for(int i = 0 ;i < (int)nCategoryCount;++i)
				{
					CItemPropCategory* pItemCategory = pProp->GetCategoryAt(i);
					const string rCategoryName = pItemCategory->name();
					const int nItemCount = pItemCategory->GetItemCount();
					for(int j = 0; j < nItemCount;++j)
					{
						CItemPropCategory::ItemPropTag* pItemTag = pItemCategory->GetItemAt(j);

						const string rKey = pItemTag->rKey;
						const string rItr = rCategoryName + "," + rKey;
						const string rValue = pProp->GetValue(rCategoryName , rKey);

						CString rFieldValue = rValue.c_str();
						rField += (_T("'") + rFieldValue + _T("',"));
						string rTemp = _T("[") + rItr + _T("],");
						rFieldNameSet += rTemp.c_str();
					}
				}
				if( _T(",") == rField.Right(1))
				{
					int at = rField.ReverseFind(',');
					rField =rField.Left(at);
				}
				if(_T(",") == rFieldNameSet.Right(1))
				{
					int at = rFieldNameSet.ReverseFind(',');
					rFieldNameSet = rFieldNameSet.Left(at);
				}
				const CString rQuery = _T("INSERT INTO ") + CString(CLoadItem::TableName().c_str()) + _T(" ( ") +  rFieldNameSet + _T(", [C_ID] )") + _T(" VALUES (") + rField + _T(",'") + rId + _T("');");
				try
				{
					pAdoDb->ExecuteQuery(rQuery.operator LPCTSTR());
				}
				catch(_com_error &e)
				{
					CMainFrame* pMainFrm = ELOADGetMainWnd();
					if(pMainFrm)
					{
						CString rMsg;
						rMsg.Format(_T("*** DOG CHECK [ADO DB] => <ExecuteQuery 오류> 설명[%s] !! ***\n"), (LPCSTR)e.Description());

						pMainFrm->m_wndOutput.AppendLogMsg(rMsg.operator LPCSTR());
					}
				}
				pLoadItem->SetDataBaseItem();

			}
			process = int(float(nIndex++) / float(LoadItemList.size()) * 100.);
			CWorkStatusDlg::UpdateWorkStatus(rTitle , process);
		}
	}

	return true;
}

/**
        @ brief  테이블에 필수 필드를 생성한다.
        
	@author	KHS
*/
int CExcelImportDlg::AppendFieldInDataBase(CADODB* pAdoDb)
{
        //! T_LOAD_ITEM 테이블에 추가
        CString rQuery("ALTER TABLE " + CString(CLoadItem::TableName().c_str()) + " ADD [C_ID] VARCHAR(50), [C_REV] VARCHAR(50),[C_REV_DESC] VARCHAR(50),[C_DELETE] VARCHAR(50) default No");
        ExecuteQuery(pAdoDb, rQuery);

        //! T_LOAD_ITEM_REV 테이블에 추가
        rQuery = "ALTER TABLE " + CString(CLoadItem::TableName().c_str()) + "_REV ADD [C_ID] VARCHAR(50) ,[C_REV] VARCHAR(50),[C_REV_DESC] VARCHAR(50),[C_DELETE] VARCHAR(50) default No";
        ExecuteQuery(pAdoDb, rQuery);

        return ERROR_SUCCESS;
}
/**
        @brief  쿼리문을 실행한다.
        @author	KHS
*/
int CExcelImportDlg::OpenQuery(CADODB* pAdoDb, const CString& rQuery)
{
        try
        {
                pAdoDb->OpenQuery(rQuery.operator LPCTSTR());
        }
        catch(_com_error &e)
        {
                CString rMsg;
                rMsg.Format("*** DOG CHECK [ADO DB] => <ExecuteQuery 오류> 설명[%s] !! ***\n", (LPCSTR)e.Description());
                AfxMessageBox(rMsg);
                return ERROR_BAD_ENVIRONMENT;
        }
        return ERROR_SUCCESS;
}


/**
        @brief  쿼리문을 실행한다.
        @author	KHS
*/
int CExcelImportDlg::ExecuteQuery(CADODB* pAdoDb, const CString& rQuery)
{
        try
        {
                pAdoDb->ExecuteQuery(rQuery.operator LPCTSTR());
        }
        catch(_com_error &e)
        {
                CString rMsg;
                rMsg.Format("*** DOG CHECK [ADO DB] => <ExecuteQuery 오류> 설명[%s] !! ***\n", (LPCSTR)e.Description());
                AfxMessageBox(rMsg);
                return ERROR_BAD_ENVIRONMENT;
        }
        return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-06-03 오후 2:39:27	

	@param	

	@return		
*/
BOOL CExcelImportDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(AfxGetApp()->LoadIcon(IDI_EXCEL) , FALSE);

	/*
	{
		CWnd* pWndEdit = GetDlgItem(IDC_DIR_EDIT);
		CWnd* pWndButton = GetDlgItem(IDC_DIR_OPEN);
		VERIFY( ::EnableBuddyButton( pWndEdit->GetSafeHwnd() , pWndButton->GetSafeHwnd() , BBS_RIGHT ) );
	}
	*/

	CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_NEW_LOADS);
	if(pButton) pButton->SetCheck(TRUE);

	ReadField();
	CreateGridCtrl();

	m_wndEditBrowseCtrl.EnableFileBrowseButton();

	CELoadDocData& docData = CELoadDocData::GetInstance();
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	m_wndOKButton.SetIcon(IDI_EXCEL_IMPORT);
	m_wndOKButton.SetTextHotColor(TextHotColor);

	m_wndCancelButton.SetIcon(IDI_CANCEL_18X18);
	m_wndCancelButton.SetTextHotColor(TextHotColor);

	return TRUE;  
	// return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief  엑셀 맵핑할 GridCtrl을 만든다.

	@author	KHS
*/
int CExcelImportDlg::CreateGridCtrl()
{
	CRect rect(11 ,100 ,644, 175);

	CWnd* pWnd = GetDlgItem(IDC_STATIC_MAPPING);
	if(pWnd)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		rect.DeflateRect(10 , 20 , 10 , 10);
	}

	if(FALSE == m_Grid.Create(rect, this, 100))
	{
		AfxMessageBox("Can't create grid ctrl.");
		return FALSE;
	}
	try 
	{
		m_Grid.SetRowCount(4);
		m_Grid.SetColumnCount(12);
		m_Grid.SetFixedRowCount(1);
		m_Grid.SetFixedColumnCount(1);

		m_Grid.SetEditable(TRUE);
		m_Grid.EnableDragAndDrop(FALSE);
		m_Grid.SetRowResize(FALSE);
		m_Grid.SetColumnResize(FALSE);
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}
	///fill rows/cols with text
	try
	{
		int nColSize = 0;

		CELoadDocData& docData = CELoadDocData::GetInstance();
		list<CELoadItem*> LoadItemList;
		docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
		//if(pLoadItemList)
		{
			const size_t nSize = LoadItemList.size();	

			CELoadItemProp* pProp = docData.m_ItemPropMap[CLoadItem::TypeString()];
			if(pProp)
			{
				const size_t nCategorySize = pProp->GetCategoryCount();
				for(size_t i = 0; i < nCategorySize;++i)
				{
					CItemPropCategory* pCategory = pProp->GetCategoryAt(i);
					if(NULL == pCategory) continue;

					const int nItemSize = pCategory->GetItemCount();
					nColSize += nItemSize;
				}
				m_Grid.SetColumnCount( nColSize + 1);
			}
		}

		// 해더
		CELoadItemProp* pLoadProp = docData.m_ItemPropMap[CLoadItem::TypeString()];
		int nColumn = 0;
		const int nCategoryCount = pLoadProp->GetCategoryCount();
		for(int i = 0;i < nCategoryCount;++i)
		{
			CItemPropCategory* pItemCategory = pLoadProp->GetCategoryAt(i);
			const string rCategoryName = pItemCategory->name();
			const int nItemCount = pItemCategory->GetItemCount();
			for(int j = 0;j < nItemCount;++j)
			{
				CItemPropCategory::ItemPropTag* pItemTag = pItemCategory->GetItemAt(j);
				const string rKey = pItemTag->rKey;
				string rFieldSet = rKey;

				GV_ITEM Item;
				Item.mask = GVIF_TEXT|GVIF_FORMAT;
				Item.row = 0;
				Item.col = ++nColumn;
				Item.nFormat = DT_LEFT|DT_WORDBREAK;//DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
				Item.strText = rFieldSet.c_str();
				if(TRUE == m_Grid.SetItem(&Item))
				{
					//! header cell에 ItemPropTag 포인터를 저장한다.
					m_Grid.SetItemData(0 , Item.col , (LPARAM)pItemTag);
				}else	AfxMessageBox(rFieldSet.c_str());
			}
		}

		//!MDB 에 연결한다.
		CADODB adoDb;
		const STRING_T rProjectMDBFilePath = docData.GetProjectMDBFilePath();
		const STRING_T rDBPath = STRING_T(PROVIDER) + _T("Data Source=") + rProjectMDBFilePath + DB_PASSWORD;
		//! MDB 연결
		BOOL bDBCon = adoDb.DBConnect(rDBPath);
		if(bDBCon)
		{
			STRING_T rQuery("SELECT * FROM T_EXCEL_MAPPING");
			adoDb.OpenQuery(rQuery);
		}

		//! Grid Index 와 Excel  Combo를 만든다.
		for(int nRow = 0; nRow < m_Grid.GetRowCount();nRow++)
		{
			for(int nCol = 0;nCol < nColSize + 1;nCol++)
			{
				GV_ITEM Item;
				Item.mask = GVIF_TEXT|GVIF_FORMAT;
				Item.row = nRow;
				Item.col = nCol;
				if ((1 > nCol) && (0 < nRow)) 
				{
					Item.nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
					Item.strText.Format(_T("%d"),nRow);
					m_Grid.SetItem(&Item);
				}
				else if((nCol >0 ) &&( nRow > 0))
				{
					Item.nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
					CItemPropCategory::ItemPropTag* pTag = (CItemPropCategory::ItemPropTag *)m_Grid.GetItemData(0 , nCol);
					if(pTag)
					{
						CString rColumnText = m_Grid.GetItemText(0 , nCol);
						string rCategory = pTag->pParent->name();
						rCategory += ("," + rColumnText);

						STRING_T rItemValue(_T(""));
						//! MDB를 읽어서 데이타를 가져온다.
						if(bDBCon)
						{
							if(TRUE != adoDb.GetFieldValue(nRow -1 , rCategory.c_str() , &rItemValue))
							{
								rItemValue = "";
							}
						}
						Item.strText = rItemValue.c_str();
						if("LOCK" == rColumnText) 
						{
							CGridCell* pCell = (CGridCell*)m_Grid.GetCell(Item.row , Item.col);
							pCell->SetState(pCell->GetState() | GVIS_READONLY);
							pCell->SetBackClr(READ_ONLY_COLOR);
						}
						else
						{
							m_Grid.SetItem(&Item);
							m_Grid.SetCellType(Item.row, Item.col , RUNTIME_CLASS(CExcelMapingGridCell));
						}
					}
				}
				m_Grid.SetRowHeight(nRow , 24);
				m_Grid.SetColumnWidth(nCol , 64);
			}	
		}
		m_Grid.AutoSizeColumns();
		m_Grid.ShowScrollBar(GVL_VERT, TRUE);
	}
	catch (exception& e)
	{
		AfxMessageBox(e.what());
		return ERROR_BAD_ENVIRONMENT;
	}

	return ERROR_SUCCESS;
}

/**
        @brief  Excel Mapping Ini에 맵핑을 저장한다.
        @author	KHS
*/
int CExcelImportDlg::WriteExcelMappingInIni(const CString& rIniFilePath)
{
        CString rApp("EXCEL_MAPPING");
        for(int i = 1; i < m_Grid.GetColumnCount();i++)
        {
                CItemPropCategory::ItemPropTag* pTag = (CItemPropCategory::ItemPropTag *)m_Grid.GetItemData(0 , i);
                if(pTag)
                {
                        string rCategory = pTag->pParent->name();
                        CString rItemText = m_Grid.GetItemText(0 , i);
                        rCategory += ("," + rItemText);
                        CString rItemValue = m_Grid.GetItemText(1, i);

                        WritePrivateProfileString(rApp , rCategory.c_str() , rItemValue , rIniFilePath);
                }
        }
        return ERROR_SUCCESS;
}

/**
	@brief  Excel Mapping을 MDB 에 저장한다.

	@author KHS
*/
BOOL CExcelImportDlg::WriteExcelMappingInMDB(void)
{
	int i = 1;
	for(vector<EXCEL_MAPPING_RECORD>::iterator itr = m_FieldEntry.begin(); itr != m_FieldEntry.end();++itr)
	{
		CString rTotalValue;;
		for(int nRow =0; nRow < m_Grid.GetRowCount() - 1;nRow++)
		{
			CString rText = m_Grid.GetItemText(nRow + 1, i);
			itr->aryExlCell[nRow] = rText.operator LPCSTR();
		}
		i++;
	}

	CELoadDocData& docData = CELoadDocData::GetInstance();
	const STRING_T rProjectMDBFilePath = docData.GetProjectMDBFilePath();
	CADODB adoDb;
	const STRING_T rDBPath = STRING_T(PROVIDER) + _T("Data Source=") + rProjectMDBFilePath + DB_PASSWORD;
	if(TRUE == adoDb.DBConnect(rDBPath))
	{
		CString rQuery, rFieldSet, rCreateFieldSet;

		if(TRUE == TableExist(&adoDb, _T("T_EXCEL_MAPPING")))
		{
			//! 테이블이 존재하면 삭제한다.
			rQuery = _T("DROP TABLE T_EXCEL_MAPPING SET");
			adoDb.ExecuteQuery(rQuery.operator LPCTSTR());
		}
		//! 테이블 생성
		for(vector<EXCEL_MAPPING_RECORD>::iterator itr = m_FieldEntry.begin(); itr != m_FieldEntry.end();++itr)
		{
			CString rKey = itr->rField.c_str();
			rCreateFieldSet += ( "[" + rKey + "] VARCHAR(50),");
			rFieldSet += ( "[" + rKey + "],");
		}
		if("," == rCreateFieldSet.Right(1))
		{
			int at = rCreateFieldSet.ReverseFind(',');
			rCreateFieldSet = rCreateFieldSet.Left(at);
		}
		if("," == rFieldSet.Right(1))
		{
			int at = rFieldSet.ReverseFind(',');
			rFieldSet = rFieldSet.Left(at);
		}
		rQuery = _T("CREATE TABLE T_EXCEL_MAPPING (") + rCreateFieldSet + _T(" )");
		if(FALSE == adoDb.ExecuteQuery(rQuery.operator LPCTSTR()))
		{
			AfxMessageBox("Fail Create Table..");
			return FALSE;
		}
		for(int nRowCount =0; nRowCount < m_Grid.GetRowCount() - 1; nRowCount++)
		{
			CString rValueSet;
			for(vector<EXCEL_MAPPING_RECORD>::iterator itr = m_FieldEntry.begin(); itr != m_FieldEntry.end();itr++)
			{
				CString rKey = itr->rField.c_str();
				CString rValue = itr->aryExlCell[nRowCount].c_str();

				rValueSet += ( "'" + rValue + "',");
			}

			if("," == rValueSet.Right(1))
			{
				int at = rValueSet.ReverseFind(',');
				rValueSet = rValueSet.Left(at);
			}
			//! 테이블을 만들었으면 데이타를 삽입한다.
			rQuery = _T("INSERT INTO T_EXCEL_MAPPING ( ") + rFieldSet + _T(") VALUES (") + rValueSet + _T(")");
			if(FALSE == adoDb.OpenQuery(rQuery.operator LPCTSTR()))
			{
				AfxMessageBox("fail, insert data..");
			}
		}
	}
	else
	{
		AfxMessageBox("Not found MDB...");
		return FALSE;
	}

	return TRUE;
}

/**
	@brief  CATEGORY + KEY (필드)를 맵에 저장한다.

	@author KHS
*/
int CExcelImportDlg::ReadField(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELoadItemProp* pLoadProp = docData.m_ItemPropMap[CLoadItem::TypeString()];

	int nColumn = 0;
	const int nCategoryCount = pLoadProp->GetCategoryCount();

	EXCEL_MAPPING_RECORD record;

	for(int i = 0;i < nCategoryCount;++i)
	{
		CItemPropCategory* pItemCategory = pLoadProp->GetCategoryAt(i);
		const string rCategoryName = pItemCategory->name();
		const int nItemCount = pItemCategory->GetItemCount();
		for(int j = 0;j < nItemCount;++j)
		{
			CItemPropCategory::ItemPropTag* pItemTag = pItemCategory->GetItemAt(j);
			const string rKey = pItemTag->rKey;
			string rFieldName = rCategoryName + "," + rKey;
			record.rField = rFieldName;

			m_FieldEntry.push_back(record);
		}
	}
	return 0;
}

/**
        @brief  

        @author BHK
*/
BOOL CExcelImportDlg::PreTranslateMessage(MSG* pMsg)
{
        //! 리턴키나 ESCAPE KEY로 윈도우가 닫기는 것을 방지.
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE) return TRUE;
		if(pMsg->wParam == VK_RETURN) return TRUE;
	}

        return CDialog::PreTranslateMessage(pMsg);
}

/**
	@brief	Result String 

	@author KHS	

	@date 2009-07-23 오전 11:26:00	

	@param	

	@return		
**/
string CExcelImportDlg::GetResultString(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	ostringstream oss;

	oss << _T("TOTAL LOAD |") << m_nTryCount << _T(" (SETS) |");
	oss << _T("LOAD CREATION |") << m_nSuccessCount << _T(" (SETS) | NOT-CREATION LOAD |") << docData.m_nErrorCount << _T(" (SETS) |");

	return oss.str();
}

/**
	@brief	Load Item import 시 Error 사항을 체크 후 에러값을 리턴

	@author KHS	

	@date 2009-08-07 오후 3:01:26	

	@param	

	@return		
**/
int CExcelImportDlg::LoadImportErrorCheck(const string& rCategory, const string& rKey, const string& rValue)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	if((_T("General") == rCategory) && (_T("ITEM ID") == rKey))
	{
		CLoadItem* pLoadItem = docData.FindLoadItemByName(rValue);
		if(pLoadItem) return 31;
	}
	else if((_T("Load") == rCategory) &&(_T("BHP <1>") == rKey))
	{
		if((isalpha(rValue[0])) && (!isdigit(rValue[0]))) return 32;
	}
	else if((_T("Rating") == rCategory) &&(_T("Rating Capacity") == rKey))
	{
		if((isalpha(rValue[0])) && (!isdigit(rValue[0]))) return 34;
	}
	else if((_T("Type") == rCategory) && (_T("Operation Mode") == rKey))
	{
		CString _rValue(rValue.c_str());
		_rValue.MakeUpper();

		if((_rValue != "C") && (_rValue != "I") && (_rValue != "S")) return 35;
	}
	else if((_T("Type") == rCategory) && (_T("Load Category") == rKey))
	{
		CString _rValue(rValue.c_str());
		_rValue.MakeUpper();

		static CString LoadCategories[]={_T("M") , _T("I") , _T("L") , _T("W") , _T("C") , _T("O") , _T("S")};
		const int nSize = sizeof(LoadCategories)/sizeof(LoadCategories[0]);
		CString* p = find(LoadCategories , LoadCategories + nSize , _rValue);
		if(nSize == (p - LoadCategories)) return 36;
		///if((_rValue != "M") && (_rValue != "I") && (_rValue != "L") && (_rValue != "W") && (_rValue != "C") && (_rValue != "O")) return 36;
	}
	else if((_T("Rating") == rCategory) &&(_T("Poles") == rKey))
	{
		if((isalpha(rValue[0])) && (!isdigit(rValue[0]))) return 37;
	}
	else if((_T("Characteristic") == rCategory) &&(_T("PF(100% Load)") == rKey))
	{
		if(atof(rValue.c_str()) > 1.) return 38;
	}
	else if((_T("Characteristic") == rCategory) &&(_T("EFF(100% Load)") == rKey))
	{
		if(atof(rValue.c_str()) > 1.) return 39;
	}
	else if((_T("Cable Route") == rCategory) && (_T("Design Length") == rKey))
	{
		if((isalpha(rValue[0])) && (!isdigit(rValue[0]))) return 40;
	}

	return ERROR_SUCCESS;
}