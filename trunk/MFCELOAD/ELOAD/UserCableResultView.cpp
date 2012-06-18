// UserCableResultView.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "UserCableResultView.h"
#include "ELoadDocData.h"
#include <Zip\inc\ZipArchive.h>
#include "pyfromc.h"
#include "SimpleAnimationDlg.h"
// CUserCableResultView

IMPLEMENT_DYNCREATE(CUserCableResultView, CFormView)

CUserCableResultView::CUserCableResultView()
	: CFormView(CUserCableResultView::IDD)
{

}

CUserCableResultView::~CUserCableResultView()
{
}

void CUserCableResultView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FPSPREAD_USER_CABLE_RESULT_VIEW, m_wndSpread);
}

BEGIN_MESSAGE_MAP(CUserCableResultView, CFormView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_SAVE, &CUserCableResultView::OnFileSave)
	ON_COMMAND(ID_FILE_PRINT, &CUserCableResultView::OnFilePrint)
END_MESSAGE_MAP()


// CUserCableResultView diagnostics

#ifdef _DEBUG
void CUserCableResultView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CUserCableResultView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CUserCableResultView message handlers

void CUserCableResultView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	BeginWaitCursor();
	m_wndSpread.SetReDraw(FALSE);
	DisplayControlCableResult();
	AdjustSheetTextAlign();
	m_wndSpread.SetReDraw(TRUE);
	EndWaitCursor();
}

void CUserCableResultView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if(m_wndSpread.GetSafeHwnd())
	{
		m_wndSpread.SetWindowPos(NULL , 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

int CUserCableResultView::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
int CUserCableResultView::DisplayControlCableResult(void)
{
	m_wndSpread.SetSheetCount(1);
	m_wndSpread.SetMaxRows(0);
	
	CELoadDocData& docData = CELoadDocData::GetInstance();

	const COLORREF ItemIdColor   = docData.GetColorCode(_T("ITEM_ID_BACK_COLOR"));

	const string rProjectPath = docData.GetProjectFolderPath();
	CString rFilePath = rProjectPath.c_str() + CString("Working\\USER_CABLE_RESULT.TXT");

	if(TRUE == m_wndSpread.LoadTextFile(rFilePath , "" , "\t" , "\n" , NULL , ""))
	{
		m_wndSpread.SetColHeaderRows(2);
		m_wndSpread.SetRowHeadersShow(FALSE);
		m_wndSpread.SetMaxCols(16);

		CreateSheetHeader();

		const long nMaxRows = m_wndSpread.GetMaxRows();
		const long nMaxCols = m_wndSpread.GetMaxCols();
		for(int i = 1;i <= nMaxRows;++i)
		{
			m_wndSpread.SetRow(i);
			m_wndSpread.SetCol(2);
			m_wndSpread.SetBackColor(ItemIdColor);
			m_wndSpread.SetBackColorStyle(m_wndSpread.BackColorStyleUnderGrid);
			
			m_wndSpread.SetCol(3);
			m_wndSpread.SetBackColor(ItemIdColor);
			m_wndSpread.SetBackColorStyle(m_wndSpread.BackColorStyleUnderGrid);

			m_wndSpread.SetCol(7);
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
int CUserCableResultView::CreateSheetHeader(void)
{
	try
	{
		//! SET COLUMN WIDTH	
		m_wndSpread.put_ColWidth(2 , 17);	
		m_wndSpread.put_ColWidth(3 , 17);
		m_wndSpread.put_ColWidth(5 , 17);
		m_wndSpread.put_ColWidth(6 , 25);
		m_wndSpread.put_ColWidth(7 , 17);
		m_wndSpread.put_ColWidth(4 , 17);
		m_wndSpread.put_RowHeight(m_wndSpread.SpreadHeader + 1, 25);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(1);
		m_wndSpread.SetText(_T("AREA"));
		m_wndSpread.AddCellSpan(1, m_wndSpread.SpreadHeader, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(2);
		m_wndSpread.SetText(_T("Panel ID"));
		m_wndSpread.AddCellSpan(2, m_wndSpread.SpreadHeader, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(3);
		m_wndSpread.SetText(_T("LOAD DATA"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(3);
		m_wndSpread.SetText(_T("ITEM NO."));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(4);
		m_wndSpread.SetText(_T("FROM"));
		m_wndSpread.AddCellSpan(4, m_wndSpread.SpreadHeader, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(5);
		m_wndSpread.SetText(_T("TO"));
		m_wndSpread.AddCellSpan(5, m_wndSpread.SpreadHeader, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(6);
		m_wndSpread.SetText(_T("DESCRIPTION"));
		m_wndSpread.AddCellSpan(6, m_wndSpread.SpreadHeader, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(7);
		m_wndSpread.SetText(_T("CABLE DATA"));
		m_wndSpread.AddCellSpan(7, m_wndSpread.SpreadHeader, 9, 1);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(7);
		m_wndSpread.SetText(_T("CABLE ID"));
		
		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(8);
		m_wndSpread.SetText(_T("TYPE \n (P/C)"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(9);
		m_wndSpread.SetText(_T("VOLT \n [KV]"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(10);
		m_wndSpread.SetText(_T("SPEC"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(11);
		m_wndSpread.SetText(_T("CORE"));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(12);
		m_wndSpread.SetText(_T("SIZE"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(13);
		m_wndSpread.SetText(_T("LENGTH"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(14);
		m_wndSpread.SetText(_T("OD"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(15);
		m_wndSpread.SetText(_T("WEIGHT"));
		
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
int CUserCableResultView::AdjustSheetTextAlign(void)
{
	static const int __COLS__[]={8,9,11,12,13,14,15};
	const int nColsCount = sizeof(__COLS__) / sizeof(__COLS__[0]);

	const long nMaxRows = m_wndSpread.GetMaxRows();
	for(int i = 1;i <= nMaxRows;++i)
	{
		m_wndSpread.put_RowHeight(i , 13);
		m_wndSpread.SetRow(i);

		for(int j = 0;j < nColsCount;++j)
		{
			m_wndSpread.SetCol(__COLS__[j]);
			if(8 == __COLS__[j] || 11 == __COLS__[j])
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
void CUserCableResultView::OnFileSave()
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

/**
	@brief	Excel Report

	@author KHS	

	@date 2009-07-06 오후 1:01:29	

	@param	

	@return		
**/
void CUserCableResultView::OnFilePrint()
{
	CString rUserCableTemplate;
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
	if(NULL != pSettingTable)
	{
		rUserCableTemplate = pSettingTable->m_RecordEntry[0].FieldValueMap[_T("C_USER_CABLE_TEMPLATE")].c_str();
	}

	if(!rUserCableTemplate.IsEmpty())
	{
		CWinThread* pThread = AfxBeginThread(gfnPrintUserCableResult , NULL , THREAD_PRIORITY_NORMAL,0);

		CELoadDocData& docData = CELoadDocData::GetInstance();
		const string rMDBFilePath = docData.GetProjectMDBFilePath();

		CADODB adoDB;
		const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;
		if(TRUE == adoDB.DBConnect(rConnectionString))
		{
			const CString rSQL(_T("SELECT * FROM T_EXCEL_REPORT_FORMAT WHERE C_TEMPLATE_NAME = '" + rUserCableTemplate +"'"));
			adoDB.OpenQuery(rSQL.operator LPCTSTR());
			LONG lRecordCount = 0L;
			adoDB.GetRecordCount(&lRecordCount);
			if(1 == lRecordCount)
			{
				const int nFieldCount = adoDB.GetFieldCount();
				STRING_T rRecordName, rTemplateName, rStartRow, _sExcelPath, rMappingData, rProjectNo, rProjectName, rClient, rDate;
				CString rExcelPath;
				char *pData = NULL;
				char *pMapData = NULL;
				long lSize = 0L;

				adoDB.GetFieldValue(0, _T("C_RECORD_NAME"), &rRecordName);
				adoDB.GetFieldValue(0, _T("C_TEMPLATE_NAME"), &rTemplateName);
				adoDB.GetFieldValue(0, _T("C_START_ROW"), &rStartRow);
				adoDB.GetFieldValue(0, _T("C_EXCEL_FILE_PATH"), &_sExcelPath);
				rExcelPath = _sExcelPath.c_str();
				adoDB.GetFieldValue(0, _T("C_PROJECT_NO"), &rProjectNo);
				adoDB.GetFieldValue(0, _T("C_PROJECT_NAME"), &rProjectName);
				adoDB.GetFieldValue(0, _T("C_CLIENT"), &rClient);
				adoDB.GetFieldValue(0, _T("C_DATE"), &rDate);

				adoDB.GetFieldSize(0 , _T("C_EXCEL_FILE") , lSize);

				CString rFolderPath = docData.GetProjectFolderPath().c_str();
				rFolderPath += _T("User Cable Result\\");
				LPSECURITY_ATTRIBUTES attr;
				attr = NULL;
				if(!FileExist(rFolderPath))
				{
					::CreateDirectory(rFolderPath , attr);
				}

				if(pData = (char*)calloc(1 , sizeof(char)*lSize))
				{
					adoDB.GetBinaryValue(0 , _T("C_EXCEL_FILE") , (LPVOID)pData , lSize);
					CFile f(rFolderPath + _T("\\UserCableResult.ZIP") , CFile::modeCreate|CFile:: modeReadWrite);
					f.Write(pData , lSize);
					f.Close();
					UncompressExcelFile(rFolderPath + _T("\\UserCableResult.ZIP"), rFolderPath);
				}

				adoDB.GetFieldSize(0 , _T("C_MAPING_DATA") , lSize);
				if(pData = (char*)calloc(1 , sizeof(char)*lSize))
				{
					adoDB.GetBinaryValue(0 , _T("C_MAPING_DATA") , (LPVOID)pData , lSize);
					rMappingData = pData;
					vector<string> oResult;
					CTokenizer<CIsFromString>::Tokenize(oResult, rMappingData , CIsFromString( _T("|") ));
					if(!oResult.empty())
					{
						list <CELoadItem*> CableItemList;
						docData.GetELoadItemListOf(CableItemList, CCableItem::TypeString());

						ofstream ofile(rFolderPath + _T("\\UserCableResult.txt"));
						if( ofile.is_open())
						{
							for(list<CELoadItem*>::iterator itr = CableItemList.begin(); itr != CableItemList.end();++itr)
							{
								CCableItem *pCableItem = static_cast<CCableItem*>(*itr);
								if(pCableItem)
								{
									//const string rCableCategory = pCableItem->prop()->GetValue(_T("General"), T_("Cable Category"));
									const string rCreatedBy = pCableItem->prop()->GetValue(_T("General"), _T("Created By"));
									if(_T("User") == rCreatedBy)
									{
										for(int i =0; i < int(oResult.size());++i)
										{
											string rValue = oResult[i];
											vector<string> oResult1, oResult2;
											CTokenizer<CIsFromString>::Tokenize(oResult1, rValue.c_str(), CIsFromString("="));
											if(oResult1.size() == 2)
											{
												if(oResult1[1].empty())
												{
													ofile << "\t";
													continue;
												}
												CString rColumn = oResult1[0].c_str();
												CTokenizer<CIsComma>::Tokenize(oResult2, oResult1[1], CIsComma());
												if(oResult2.size() == 2)
												{
													const string rValue = pCableItem->prop()->GetValue(oResult2[0],oResult2[1]);

													ofile << rValue + "\t";
												}
											}
										}

										ofile << endl;
									}
								}
							}
							ofile.close();
						}
					}
				}

				//! 메모리 해제
				if(pData)
				{
					free((void*)pData);
					pData = NULL;
				}
				int at = rExcelPath.ReverseFind('\\');
				rExcelPath = rExcelPath.Right(rExcelPath.GetLength() - at);
				python.CallPyWriteDataFileToExcel(rFolderPath + rExcelPath, rStartRow.c_str() , rProjectNo.c_str() , rProjectName.c_str(),
					rClient.c_str() , rDate.c_str() , rFolderPath + _T("\\UserCableResult.txt"));

				if(NULL != pThread)
				{
					CSimpleAnimationDlg& anidlg = CSimpleAnimationDlg::GetInstance();
					if(anidlg.GetSafeHwnd()) anidlg.SendMessage(WM_COMMAND , IDOK);
				}
			}
			else
			{
				if(NULL != pThread)
				{
					CSimpleAnimationDlg& anidlg = CSimpleAnimationDlg::GetInstance();
					if(anidlg.GetSafeHwnd()) anidlg.SendMessage(WM_COMMAND , IDOK);
				}

				AfxMessageBox("User Cable Template이 존재 하지 않습니다.");
			}
		}
		else
		{
			AfxMessageBox(_T("Fail to connect database"));
		}
	}else	AfxMessageBox(_T("First of all, Select User Cable Template."));
}

/**
	@brief	

	@author KHS	

	@date 2009-07-06 오후 1:53:34	

	@param	

	@return		
**/
int CUserCableResultView::UncompressExcelFile(const CString& rExcelPath, const CString& rFolderPath)
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
