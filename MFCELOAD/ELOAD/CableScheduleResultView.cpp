// CableScheduleResultView.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "CableScheduleResultView.h"
#include "ELoadDocData.h"
#include <Zip\inc\ZipArchive.h>
#include "pyfromc.h"
#include "SimpleAnimationDlg.h"
#include "CableScheduleResultDoc.h"

// CCableScheduleResultView

IMPLEMENT_DYNCREATE(CCableScheduleResultView, CFormView)

CCableScheduleResultView::CCableScheduleResultView()
	: CFormView(CCableScheduleResultView::IDD)
{

}

CCableScheduleResultView::~CCableScheduleResultView()
{
}

void CCableScheduleResultView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FPSPREAD_USER_CABLE_RESULT_VIEW, m_wndSpread);
}

BEGIN_MESSAGE_MAP(CCableScheduleResultView, CFormView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_SAVE, &CCableScheduleResultView::OnFileSave)
	ON_COMMAND(ID_FILE_PRINT, &CCableScheduleResultView::OnFilePrint)
END_MESSAGE_MAP()


// CCableScheduleResultView diagnostics

#ifdef _DEBUG
void CCableScheduleResultView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CCableScheduleResultView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CCableScheduleResultView message handlers

void CCableScheduleResultView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string rProjectPath = docData.GetProjectFolderPath();
	CCableScheduleResultDoc* pDoc = (CCableScheduleResultDoc*)GetDocument();
	const CString rFilePath = pDoc->m_rFolderPath + CString(_T("CableScheduleResult.txt"));
	if(::FileExist(rFilePath))
	{
		BeginWaitCursor();
		m_wndSpread.SetReDraw(FALSE);
		
		DisplayCableScheduleResult();
		AdjustSheetTextAlign();
		m_wndSpread.SetReDraw(TRUE);
		EndWaitCursor();
	}
	else
	{
		AfxMessageBox(_T("No Cable Schedule Product."));
	}
}

void CCableScheduleResultView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if(m_wndSpread.GetSafeHwnd())
	{
		m_wndSpread.SetWindowPos(NULL , 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

int CCableScheduleResultView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

/**
	@brief	Control cable data 를 Sheet에 뿌린다.

	@author KHS	

	@date 2009-06-09 오후 1:38:50	

	@param	

	@return		
**/
int CCableScheduleResultView::DisplayCableScheduleResult(void)
{
	m_wndSpread.SetSheetCount(1);
	m_wndSpread.SetMaxRows(0);
	
	CELoadDocData& docData = CELoadDocData::GetInstance();

	const COLORREF ItemIdColor   = docData.GetColorCode(_T("ITEM_ID_BACK_COLOR"));

	CCableScheduleResultDoc* pDoc = (CCableScheduleResultDoc*)GetDocument();
	CString rFilePath = pDoc->m_rFolderPath + CString(_T("CableScheduleResult.txt"));

	if(TRUE == m_wndSpread.LoadTextFile(rFilePath , "" , "\t" , "\n" , NULL , ""))
	{
		m_wndSpread.SetColHeaderRows(2);
		m_wndSpread.SetRowHeadersShow(FALSE);
		m_wndSpread.SetMaxCols(16);

		CCableScheduleResultView::CreateSheetHeader(m_wndSpread);

		const long nMaxRows = m_wndSpread.GetMaxRows();
		const long nMaxCols = m_wndSpread.GetMaxCols();
		for(int i = 1;i <= nMaxRows;++i)
		{
			m_wndSpread.SetRow(i);
			m_wndSpread.SetCol(2);
			m_wndSpread.SetBackColor(ItemIdColor);
			m_wndSpread.SetBackColorStyle(m_wndSpread.BackColorStyleUnderGrid);

			m_wndSpread.SetCol(8);
			m_wndSpread.SetBackColor(ItemIdColor);
			m_wndSpread.SetBackColorStyle(m_wndSpread.BackColorStyleUnderGrid);
		}
		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}


/**
	@brief	USER CABLE RESULT VIEW HEADER

	@author KHS	

	@date 2009-06-25 오후 1:52:57	

	@param	

	@return		
**/
int CCableScheduleResultView::CreateSheetHeader(CFpspread1& m_wndSpread)
{
	try
	{
		//! SET COLUMN WIDTH	
		m_wndSpread.put_ColWidth(2 , 17);	
		m_wndSpread.put_ColWidth(5 , 17);
		m_wndSpread.put_ColWidth(6 , 17);
		m_wndSpread.put_ColWidth(7 , 25);
		m_wndSpread.put_ColWidth(8 , 17);
		m_wndSpread.put_ColWidth(15 , 15);
		m_wndSpread.put_RowHeight(m_wndSpread.SpreadHeader + 1, 25);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(1);
		m_wndSpread.SetText(_T("AREA"));
		m_wndSpread.AddCellSpan(1, m_wndSpread.SpreadHeader, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(2);
		m_wndSpread.SetText(_T("LOAD ID"));
		m_wndSpread.AddCellSpan(2, m_wndSpread.SpreadHeader, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader );
		m_wndSpread.SetCol(3);
		m_wndSpread.SetText(_T("LOAD\nVOLT\n[KV]"));
		m_wndSpread.AddCellSpan(3, m_wndSpread.SpreadHeader, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader );
		m_wndSpread.SetCol(4);
		m_wndSpread.SetText(_T("LOAD\nRATING\n[KW]"));
		m_wndSpread.AddCellSpan(4, m_wndSpread.SpreadHeader, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(5);
		m_wndSpread.SetText(_T("FROM"));
		m_wndSpread.AddCellSpan(5, m_wndSpread.SpreadHeader, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(6);
		m_wndSpread.SetText(_T("TO"));
		m_wndSpread.AddCellSpan(6, m_wndSpread.SpreadHeader, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(7);
		m_wndSpread.SetText(_T("DESCRIPTION"));
		m_wndSpread.AddCellSpan(7, m_wndSpread.SpreadHeader, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(8);
		m_wndSpread.SetText(_T("CABLE DATA"));
		m_wndSpread.AddCellSpan(8, m_wndSpread.SpreadHeader, 7, 1);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(8);
		m_wndSpread.SetText(_T("CABLE ID"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(9);
		m_wndSpread.SetText(_T("VOLT"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(10);
		m_wndSpread.SetText(_T("TYPE"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(11);
		m_wndSpread.SetText(_T("CORE"));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(12);
		m_wndSpread.SetText(_T("SIZE"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(13);
		m_wndSpread.SetText(_T("LENGTH\n[m]"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(14);
		m_wndSpread.SetText(_T("CONDUIT\nSIZE"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(15);
		m_wndSpread.SetText(_T("DRUM\nNO."));
		m_wndSpread.AddCellSpan(15, m_wndSpread.SpreadHeader, 1, 2);
		
		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(16);
		m_wndSpread.SetText(_T("REMARK"));
		m_wndSpread.AddCellSpan(16, m_wndSpread.SpreadHeader, 1, 2);
	}
	catch(...)
	{
		return ERROR_BAD_ENVIRONMENT;
	}
	return ERROR_SUCCESS;
}

/**
	@brief	SHEET의 텍스트 정렬을 맞춘다.

	@author	KHS
*/
int CCableScheduleResultView::AdjustSheetTextAlign(void)
{
	static const int __COLS__[]={3,4,9,11,12,13,14};
	const int nColsCount = sizeof(__COLS__) / sizeof(__COLS__[0]);

	const long nMaxRows = m_wndSpread.GetMaxRows();
	for(int i = 1;i <= nMaxRows;++i)
	{
		m_wndSpread.put_RowHeight(i , 13);
		m_wndSpread.SetRow(i);

		for(int j = 0;j < nColsCount;++j)
		{
			m_wndSpread.SetCol(__COLS__[j]);
			if(11 == __COLS__[j])
				m_wndSpread.SetTypeHAlign(m_wndSpread.TypeHAlignCenter);
			else
				m_wndSpread.SetTypeHAlign(m_wndSpread.TypeHAlignRight);
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	SAVE

	@author KHS	

	@date 2009-06-26 오후 2:54:15	

	@param	

	@return		
**/
void CCableScheduleResultView::OnFileSave()
{
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp) pApp->OnSaveDataBase();
}

static UINT gfnPrintUserCableResult(LPVOID pVoid)
{
	CSimpleAnimationDlg& anidlg = CSimpleAnimationDlg::GetInstance();
	if(NULL == anidlg.GetSafeHwnd())
	{
		anidlg.DoModal();
	}

	return ERROR_SUCCESS;
}

/*
namespace CableScheduleResult
{
	bool SortByColumn(const string& lhs , const string& rhs)
	{
		vector<STRING> oResult1 , oResult2;
		CTokenizer<CIsFromString>::Tokenize(oResult1 , lhs , CIsFromString(_T("=")));
		CTokenizer<CIsFromString>::Tokenize(oResult2 , rhs , CIsFromString(_T("=")));

		if((2 == oResult1.size()) && (2 == oResult2.size())) return oResult1[1] < oResult2[1];
		return false;
	}
};
*/

/**
	@brief	Excel Report

	@author BHK	

	@date 2009-09-03 오후 5:03:35

	@param	

	@return		
**/
void CCableScheduleResultView::OnFilePrint()
{
	CCableScheduleResultDoc* pDoc = (CCableScheduleResultDoc*)GetDocument();
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const CString rFilePath = (pDoc->m_rFolderPath + _T("CableScheduleResult.txt"));
	
	ExportToExcel(rFilePath);

}

/**
	@brief	Excel Report

	@author BHK	

	@date 2009-10-27

	@param	

	@return		
**/
int CCableScheduleResultView::ExportToExcel(const CString& rFilePath)
{
	//CString rCableScheduleTemplate;
	//CELoadDocData& docData = CELoadDocData::GetInstance();
	//CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
	//if(NULL != pSettingTable)
	//{
	//	rCableScheduleTemplate = pSettingTable->m_RecordEntry[0].FieldValueMap[_T("C_CABLE_SCHEDULE_TEMPLATE")].c_str();
	//}

	//if(!rCableScheduleTemplate.IsEmpty())
	//{
	//	CWinThread* pThread = AfxBeginThread(gfnPrintUserCableResult , NULL , THREAD_PRIORITY_NORMAL,0);

	//	CELoadDocData& docData = CELoadDocData::GetInstance();
	//	const string rMDBFilePath = docData.GetProjectMDBFilePath();

	//	CADODB adoDB;
	//	const CString rConnectionString = CString("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=") + rMDBFilePath.c_str() + DB_PASSWORD;
	//	if(TRUE == adoDB.DBConnect(rConnectionString))
	//	{
	//		const CString rSQL(_T("SELECT * FROM T_EXCEL_REPORT_FORMAT WHERE C_TEMPLATE_NAME = '" + rCableScheduleTemplate +"'"));
	//		adoDB.OpenQuery(rSQL);
	//		LONG lRecordCount = 0L;
	//		adoDB.GetRecordCount(&lRecordCount);
	//		if(1 == lRecordCount)
	//		{
	//			const int nFieldCount = adoDB.GetFieldCount();
	//			CString rRecordName, rTemplateName, rStartRow, rExcelPath, rMappingData, rProjectNo, rProjectName, rClient, rDate;
	//			char *pData = NULL;
	//			char *pMapData = NULL;
	//			long lSize = 0L;

	//			adoDB.GetFieldValue(0, _T("C_RECORD_NAME"), &rRecordName);
	//			adoDB.GetFieldValue(0, _T("C_TEMPLATE_NAME"), &rTemplateName);
	//			adoDB.GetFieldValue(0, _T("C_START_ROW"), &rStartRow);
	//			adoDB.GetFieldValue(0, _T("C_EXCEL_FILE_PATH"), &rExcelPath);
	//			adoDB.GetFieldValue(0, _T("C_PROJECT_NO"), &rProjectNo);
	//			adoDB.GetFieldValue(0, _T("C_PROJECT_NAME"), &rProjectName);
	//			adoDB.GetFieldValue(0, _T("C_CLIENT"), &rClient);
	//			adoDB.GetFieldValue(0, _T("C_DATE"), &rDate);

	//			adoDB.GetFieldSize(0 , _T("C_EXCEL_FILE") , lSize);

	//			CString rFolderPath = docData.GetProjectFolderPath().c_str();
	//			rFolderPath += _T("Cable Schedule Result\\");
	//			LPSECURITY_ATTRIBUTES attr;
	//			attr = NULL;
	//			if(!FileExist(rFolderPath))
	//			{
	//				::CreateDirectory(rFolderPath , attr);
	//			}

	//			if(pData = (char*)calloc(1 , sizeof(char)*lSize))
	//			{
	//				adoDB.GetBinaryValue(0 , _T("C_EXCEL_FILE") , (LPVOID)pData , lSize);
	//				CFile f(rFolderPath + _T("\\ExcelTemplate.ZIP") , CFile::modeCreate|CFile:: modeReadWrite);
	//				f.Write(pData , lSize);
	//				f.Close();

	//				CCableScheduleResultView::UncompressExcelFile(rFolderPath + _T("\\ExcelTemplate.ZIP"), rFolderPath);
	//			}

	//			/*
	//			adoDB.GetFieldSize(0 , _T("C_MAPING_DATA") , lSize);
	//			if(pData = (char*)calloc(1 , sizeof(char)*lSize))
	//			{
	//				adoDB.GetBinaryValue(0 , _T("C_MAPING_DATA") , (LPVOID)pData , lSize);
	//				rMappingData = pData;
	//				vector<string> oResult;
	//				CTokenizer<CIsFromString>::Tokenize(oResult, rMappingData.operator LPCSTR(), CIsFromString(_T("|")));
	//				if(!oResult.empty())
	//				{
	//					WriteCableScheduleToTextFile(docData.GetProjectFolderPath() + _T("Working\\CableScheduleResult.txt") , oResult);
	//				}
	//			}
	//			*/

	//			//! 메모리 해제
	//			if(pData)
	//			{
	//				free((void*)pData);
	//				pData = NULL;
	//			}
	//			int at = rExcelPath.ReverseFind('\\');
	//			rExcelPath = rExcelPath.Right(rExcelPath.GetLength() - at);
	//			
	//			python.CallPyWriteDataFileToExcel(rFolderPath + rExcelPath, rStartRow, rProjectNo, rProjectName,
	//				rClient, rDate , rFilePath);

	//			if(NULL != pThread)
	//			{
	//				CSimpleAnimationDlg& anidlg = CSimpleAnimationDlg::GetInstance();
	//				if(anidlg.GetSafeHwnd()) anidlg.SendMessage(WM_COMMAND , IDOK);
	//			}
	//		}
	//	}
	//	else
	//	{
	//		AfxMessageBox(_T("Fail to connect database"));
	//	}
	//}else	AfxMessageBox(_T("First of all, Select Cable Schedule Template."));
	
	CWinThread* pThread = AfxBeginThread(gfnPrintUserCableResult , NULL , THREAD_PRIORITY_NORMAL,0);
	
	python.CallPyWriteCableScheduleResultToExcel();

	if(NULL != pThread)
	{
		CSimpleAnimationDlg& anidlg = CSimpleAnimationDlg::GetInstance();
		if(anidlg.GetSafeHwnd()) anidlg.SendMessage(WM_COMMAND , IDOK);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-09-03 오후 5:09:26	

	@param	

	@return		
*/
int CCableScheduleResultView::WriteCableScheduleToTextFile(const string& rFilePath , const vector<string>& MappingList)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	TABLES::CELOADTable* pTable = docData.GetTableOf(_T("T_CABLE_SCHEDULE_SETTING"));
	ofstream ofile(rFilePath.c_str());
	if(ofile.is_open() && pTable)
	{
		const int nTotalCount = pTable->m_RecordEntry.size();
		int nIndex = 0;

		for(vector<TABLES::CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin();itr != pTable->m_RecordEntry.end();++itr)
		{
			CBusItem* pBusItem = docData.FindBusItemByName(itr->FieldValueMap[_T("C_BUS_ID")]);
			if(NULL == pBusItem) continue;

			ofile << _T("\t") << pBusItem->GetName();
			ofile << _T(" - ") << pBusItem->prop()->GetValue(_T("General") , _T("Description")) << std::endl;

			list<CELoadItem*> CableItemList;
			pBusItem->GetCableList(CableItemList);

			///list<CELoadItem*> EqCableItemList;
			
			string rLoadID;
			::stable_sort(CableItemList.begin() , CableItemList.end() , CableScheduleResult::SortCableItemFunc);
			for(list<CELoadItem*>::iterator jtr = CableItemList.begin(); jtr != CableItemList.end();++jtr)
			{
				if((*jtr)->IsDeleted()) continue;
				
				CCableItem* pCableItem = static_cast<CCableItem*>(*jtr);
				if(!rLoadID.empty() && (rLoadID != pCableItem->prop()->GetValue(_T("General") , _T("Load Id"))))
					ofile << std::endl;
				
				rLoadID = pCableItem->prop()->GetValue(_T("General") , _T("Load Id"));
				WriteCableItemToTextFile(ofile , pCableItem , MappingList);
			}
			ofile << std::endl;
		}
		ofile.close();

		return ERROR_SUCCESS;
	}
	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author BHK	

	@date 2009-09-03 오후 5:14:31	

	@param	

	@return		
*/
int CCableScheduleResultView::WriteCableItemToTextFile(ofstream& ofile , CCableItem* pCableItem , const vector<string>& MappingList)
{
	assert(pCableItem && "pCableItem is NULL");

	if(pCableItem)
	{
		for(vector<string>::const_iterator itr = MappingList.begin(); itr != MappingList.end();++itr)
		{
			const string rValue = (*itr);
			vector<string> oResult1, oResult2;
			CTokenizer<CIsFromString>::Tokenize(oResult1, rValue.c_str(), CIsFromString("="));
			if(2 == oResult1.size())
			{
				if(oResult1[1].empty())
				{
					ofile << _T("\t");
					continue;
				}
				CString rColumn = oResult1[0].c_str();
				CTokenizer<CIsComma>::Tokenize(oResult2, oResult1[1], CIsComma());
				if(2 == oResult2.size())
				{
					const string rValue = pCableItem->prop()->GetValue(oResult2[0],oResult2[1]);

					ofile << rValue + _T("\t");
				}else	ofile << _T("\t");
			}else	ofile << _T("\t");
		}

		ofile << endl;

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author KHS	

	@date 2009-07-06 오후 1:53:34	

	@param	

	@return		
**/
int CCableScheduleResultView::UncompressExcelFile(const CString& rExcelPath, const CString& rFolderPath)
{
	CZipArchive zip;
	zip.Open(rExcelPath , CZipArchive::open);
	const int nEntries = zip.GetNoEntries();
	for(int i = 0;i < nEntries;++i)
	{
		zip.ExtractFile(i,rFolderPath);
	}
	zip.Close();

	return ERROR_SUCCESS;
}
