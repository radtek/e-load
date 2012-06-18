// ExcelReportFormatView.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "ExcelReportFormatView.h"
#include "ELoadDocData.h"
#include <Zip\inc\ZipArchive.h>

// CExcelReportFormatView

IMPLEMENT_DYNCREATE(CExcelReportFormatView, CFormView)

CExcelReportFormatView::CExcelReportFormatView()
	: CFormView(CExcelReportFormatView::IDD)
	, m_rExcelFileName(_T(""))
{

}

CExcelReportFormatView::~CExcelReportFormatView()
{
}
 
void CExcelReportFormatView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SELECT_TABLE_COMBO, m_wndSelectTableCombo);
	DDX_Control(pDX, IDC_SAVE_BUTTON, m_wndSaveButton);
	DDX_Control(pDX, IDC_FPSPREAD_EXCEL_REPORT_FORMAT_VIEW, m_wndSpread);
	DDX_Control(pDX, IDC_EXCEL_IMPORT_BUTTON, m_wndExcelImportButton);
	DDX_Control(pDX, IDC_SELECT_EXCEL_FILE_NAME_EDIT, m_wndExcelNameEdit);
	DDX_Control(pDX, IDC_FPSPREAD_EXCEL_REPORT_FORMAT_EDIT_VIEW, m_wndEditSpread);
	DDX_Control(pDX, IDC_EDIT_START_ROW, m_wndStartRowEdit);
	DDX_Control(pDX, IDC_TEMPLATE_NAME_EDIT, m_wndTemplateNameEdit);
	DDX_Control(pDX, IDC_TEMPLATE_NAME_STATIC, m_wndTemplateNameStatic);
	DDX_Control(pDX, IDC_TEMPLATE_NAME_COMBO, m_wndTemplateNameCombo);
	DDX_Control(pDX, IDC_EDIT_PROJECT_NO, m_wndProjectNoEdit);
	DDX_Control(pDX, IDC_EDIT_PROJECT_NAME, m_wndProjectNameEdit);
	DDX_Control(pDX, IDC_EDIT_CLIENT, m_wndClientEdit);
	DDX_Control(pDX, IDC_EDIT_DATE, m_wndDateEdit);
}

BEGIN_MESSAGE_MAP(CExcelReportFormatView, CFormView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_EXCEL_IMPORT_BUTTON, &CExcelReportFormatView::OnBnClickedExcelImportButton)
	ON_CBN_SELCHANGE(IDC_SELECT_TABLE_COMBO, &CExcelReportFormatView::OnCbnSelchangeSelectTableCombo)
	ON_BN_CLICKED(IDC_SAVE_BUTTON, &CExcelReportFormatView::OnBnClickedSaveButton)
	ON_CBN_SELCHANGE(IDC_TEMPLATE_NAME_COMBO, &CExcelReportFormatView::OnCbnSelchangeTemplateNameCombo)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CExcelReportFormatView, CFormView)
	ON_EVENT(CExcelReportFormatView, IDC_FPSPREAD_EXCEL_REPORT_FORMAT_EDIT_VIEW, 31, CExcelReportFormatView::ComboSelChangeFpspread1, VTS_I4 VTS_I4)
END_EVENTSINK_MAP()

/**
	@brief	콤보 박스에 선택한 내용일 변경되었을 때...

	@author BHK	

	@date 2009-07-22 오후 3:18:26	

	@param	

	@return		
*/
void CExcelReportFormatView::ComboSelChangeFpspread1(long Col, long Row)
{
	CELoadDocData &docData = CELoadDocData::GetInstance();

	CString rTableName;
	const int nSelItem = m_wndSelectTableCombo.GetCurSel();
	if(-1 != nSelItem)
	{
		m_wndSelectTableCombo.GetLBText(nSelItem, rTableName);	
	}
	else
	{
		AfxMessageBox("Select table, in first !!");
		return;
	}

	m_wndEditSpread.SetRow(Row);
	m_wndEditSpread.SetCol(Col);
	const CString rFieldName = m_wndEditSpread.GetText();

	if(rFieldName.IsEmpty())
	{
		AfxMessageBox("Select Field, in first !!");
		return;
	}
	

	const STRING_T rMDBFilePath = docData.GetProjectMDBFilePath();

	CADODB adoDB;
	const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rConnectionString))
	{
		adoDB.OpenQuery(CString("SELECT * FROM T_"+ rTableName + "_ITEM").operator LPCTSTR());
		
		LONG lRecordCount = 0L;
		STRING_T rValue;
		adoDB.GetRecordCount(&lRecordCount);
		if(0 < lRecordCount)
		{
			for(int i = 0; i < lRecordCount;i++)
			{
				if(9 < i ) break;

				adoDB.GetFieldValue(i, rFieldName.operator LPCTSTR() , &rValue);
				m_wndEditSpread.SetCol(Col);
				m_wndEditSpread.SetRow(Row + i + 1);
				m_wndEditSpread.SetText(rValue.c_str());
			}
		}
		else
		{
			return;
		}
	}
	else
	{
		AfxMessageBox("Fail to connect project database... !");
		return;
	}

}

// CExcelReportFormatView diagnostics

#ifdef _DEBUG
void CExcelReportFormatView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CExcelReportFormatView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CExcelReportFormatView message handlers

/**
	@brief	

	@author BHK	

	@date 2009-07-30 오전 10:23:12	

	@param	

	@return		
*/
void CExcelReportFormatView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if(m_wndSpread.GetSafeHwnd())
	{
		CRect rect , rectButton;
		m_wndSpread.GetWindowRect(&rect);
		ScreenToClient(&rect);

		if(m_wndSaveButton.GetSafeHwnd())
		{
			m_wndSaveButton.GetWindowRect(&rectButton);
			ScreenToClient(&rectButton);
		}

		m_wndSpread.SetWindowPos(NULL , 0, rect.top , cx , cy - rect.top - rectButton.Height() - 5 , SWP_NOACTIVATE | SWP_NOZORDER);
	}

	
	if(m_wndEditSpread.GetSafeHwnd())
	{
		CRect rect , rectButton;
		m_wndEditSpread.GetWindowRect(&rect);
		ScreenToClient(&rect);

		if(m_wndSpread.GetSafeHwnd())
		{
			m_wndSpread.GetWindowRect(&rectButton);
			ScreenToClient(&rectButton);
		}

		m_wndEditSpread.SetWindowPos(NULL , 0, rect.top , cx , cy - rect.top - rectButton.Height() - 50, SWP_NOACTIVATE | SWP_NOZORDER);
	}

	if(m_wndSaveButton.GetSafeHwnd())
	{
		CRect rect;
		m_wndSaveButton.GetWindowRect(&rect);
		ScreenToClient(&rect);

		m_wndSaveButton.SetWindowPos(NULL , cx - rect.Width() , cy - rect.Height() , 0 , 0 , SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
	}


	if(m_wndTemplateNameEdit.GetSafeHwnd())
	{
		CRect rect, buttonrect;
		m_wndTemplateNameEdit.GetWindowRect(&rect);
		m_wndSaveButton.GetWindowRect(&buttonrect);
		ScreenToClient(&rect);
		ScreenToClient(&buttonrect);

		m_wndTemplateNameEdit.SetWindowPos(NULL , cx - rect.Width() - buttonrect.Width() -5, cy - rect.Height() - 5 , 0 , 0 , SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
	}

	if(m_wndTemplateNameStatic.GetSafeHwnd())
	{
		CRect rect, buttonrect, editrect;
		m_wndTemplateNameStatic.GetWindowRect(&rect);
		m_wndTemplateNameEdit.GetWindowRect(&editrect);
		m_wndSaveButton.GetWindowRect(&buttonrect);
		ScreenToClient(&rect);
		ScreenToClient(&editrect);
		ScreenToClient(&buttonrect);

		m_wndTemplateNameStatic.SetWindowPos(NULL , cx - rect.Width()- editrect.Width() - buttonrect.Width() -10, cy - rect.Height() - 5 , 0 , 0 , SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
	}
}

int CExcelReportFormatView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}


void CExcelReportFormatView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	//! BUTTON SETTING
	CELoadDocData& docData = CELoadDocData::GetInstance();

	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	m_wndSaveButton.SetIcon(IDI_OK_18X18);
	m_wndSaveButton.SetTextHotColor(TextHotColor);

	m_wndExcelImportButton.SetTextHotColor(TextHotColor);

	m_wndSpread.SetMaxRows(28);
	m_wndSpread.SetMaxCols(21);
	
	LoadTemplate();

	FillTableNameCombo();

	DisplayEditSpread();


}

/**
	@brief	LOAD EXCEL FILE

	@author KHS	

	@date 2009-07-01 오후 1:25:00	

	@param	

	@return		
**/
void CExcelReportFormatView::OnBnClickedExcelImportButton()
{
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY  ,"Excel Files(*.xls)|*.xls|All Files(*.*)|*.*||");
	if(fileDlg.DoModal() == IDOK)
	{
		CString strFileName = fileDlg.GetPathName();
		CString strFileTitle = fileDlg.GetFileTitle();
		CFile myFile;
		CFileException fileException;

		if(!myFile.Open(strFileName, CFile::modeRead, &fileException))
		{
			TRACE("Can't open file %s, error = %u\n", strFileName,fileException.m_cause);
		}
		myFile.Close();

		if(!m_wndSpread.ImportExcelBook(strFileName, ""))
		{
			::MessageBox(NULL, "Fail, Load Excel File", "Warning", MB_OK|MB_ICONWARNING);
			return;
		}
		m_rExcelFileName = (strFileTitle + ".xls");
		m_wndExcelNameEdit.SetWindowText(strFileName);
		
		/*const int nCount = m_wndEditSpread.GetMaxCols();
		for(int i = 0;i < nCount;++i)
		{
			const double nWidth =  m_wndSpread.get_ColWidth(i);
			m_wndEditSpread.put_ColWidth(i, nWidth);
		}*/
	}
	else return;

}

/**
	@brief	FILL TABLE NAME COMBO BOX

	@author KHS	

	@date 2009-07-01 오후 2:30:24	

	@param	

	@return		
**/
int CExcelReportFormatView::FillTableNameCombo(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	for(map<string , CELoadItemProp* >::iterator itr = docData.m_ItemPropMap.begin();itr != docData.m_ItemPropMap.end();++itr)
	{
		m_wndSelectTableCombo.AddString(itr->first.c_str());
	}

	return 0;
}

/**
	@brief	

	@author BHK	

	@date 2009-07-01 오후 4:56:11	

	@param	

	@return		
*/
BOOL CExcelReportFormatView::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)   // 키보드가 눌렸을때 처리
	{
		if(pMsg->hwnd == GetDlgItem(IDC_EDIT_START_ROW)->GetSafeHwnd())
		{
			switch (pMsg->wParam)
			{ 
				case VK_RETURN:
				{
					AfxMessageBox(_T("DDD"));
				}
			}
		}
	}

	return CFormView::PreTranslateMessage(pMsg);
}

/**
	@brief	

	@author KHS	

	@date 2009-07-02 오후 5:07:20	

	@param	

	@return		
**/
int CExcelReportFormatView::DisplayEditSpread(void)
{
	m_wndEditSpread.SetMaxRows(10);
	m_wndEditSpread.SetMaxCols(30);
	m_wndEditSpread.SetScrollBars(1);

	for(int nRow = 1; nRow <= 10;++nRow)
	{
		m_wndEditSpread.SetRow(nRow);
		m_wndEditSpread.put_RowHeight(nRow, 18);
		for(int nCol = 1; nCol <= 30;++nCol)
		{
			m_wndEditSpread.SetCol(nCol);
			if(1 == nRow)
			{
				m_wndEditSpread.SetCellType(m_wndEditSpread.CellTypeComboBox);
			}
		}
	}


	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	Table Name ComboBox Selchange

	@author KHS	

	@date 2009-07-02 오후 5:40:31	

	@param	

	@return		
**/
void CExcelReportFormatView::OnCbnSelchangeSelectTableCombo()
{
	CString rTableName;
	const int nSelItem = m_wndSelectTableCombo.GetCurSel();
	if(-1 != nSelItem)
	{
		m_wndSelectTableCombo.GetLBText(nSelItem, rTableName);	
	}

	m_wndEditSpread.SetCol(1);
	m_wndEditSpread.SetRow(1);

	const int nColCount = m_wndEditSpread.GetMaxCols();
	const int nRowCount = m_wndEditSpread.GetMaxRows();
	for(int i =1; i <= nColCount;i++)
	{
		m_wndEditSpread.SetCol(i);
		InsertItemInSpreadCombo(i, rTableName);
	}

	for(int nRow = 2; nRow <= nRowCount;nRow++)
	{
		m_wndEditSpread.SetRow(nRow);
		for(int nCol = 1; nCol < nColCount;++nCol)
		{
			m_wndEditSpread.SetCol(nCol);
			m_wndEditSpread.SetText("");
		}
	}
	
}

/**
	@brief	Column에 필드를 채운다.

	@author KHS	

	@date 2009-07-03 오후 12:57:24	

	@param	

	@return		
**/
int CExcelReportFormatView::InsertItemInSpreadCombo(const int& nCol, const CString& rTableName)
{
	m_wndEditSpread.SetAction(m_wndEditSpread.ActionComboClear);
	
	m_wndEditSpread.SetTypeComboBoxString(_T(" "));

	CELoadDocData& docData = CELoadDocData::GetInstance();

	for(map<string , CELoadItemProp* >::iterator itr = docData.m_ItemPropMap.begin();itr != docData.m_ItemPropMap.end();++itr)
	{
		if(rTableName == itr->first.c_str())
		{
			const size_t nCount = itr->second->GetCategoryCount();
			for(int i = (nCount -1);i >= 0;--i)
			{
				CItemPropCategory* pItemCategory = itr->second->GetCategoryAt(i);
				const string rCategory = pItemCategory->name();
				const size_t nItemCount = pItemCategory->GetItemCount();
				for(int j = (nItemCount - 1);j >= 0;--j)
				{
					const string rKey = pItemCategory->GetItemNameAt(j);
					m_wndEditSpread.SetTypeComboBoxString(CString(rCategory.c_str()) + "," + rKey.c_str());
					m_wndEditSpread.SetTypeComboBoxWidth(170);
					m_wndEditSpread.put_ColWidth(nCol, 22);
				}
			}
		}
	}
	return ERROR_SUCCESS;
}

/**
	@brief	SAVE

	@author KHS	

	@date 2009-07-03 오후 1:33:48	

	@param	

	@return		
**/
void CExcelReportFormatView::OnBnClickedSaveButton()
{
	CString rExcelPath, rStartRow, rRecordName, rTemplateName;
	m_wndExcelNameEdit.GetWindowText(rExcelPath);
	m_wndStartRowEdit.GetWindowText(rStartRow);
	m_wndTemplateNameEdit.GetWindowText(rTemplateName);
	if(rExcelPath.IsEmpty())
	{
		::MessageBox(NULL, "Select Excel file...!!", "Warning", MB_OK|MB_ICONWARNING);
		return;
	}

	int nSelItem = m_wndSelectTableCombo.GetCurSel();
	if(-1 != nSelItem)
	{
		m_wndSelectTableCombo.GetLBText(nSelItem, rRecordName);
	}
	else
	{
		::MessageBox(NULL, "Select Table and Mapping Table..!!", "Warning", MB_OK|MB_ICONWARNING);
		return;
	}

	if(rStartRow.IsEmpty())
	{
		::MessageBox(NULL, "Input Start Row...!!", "Warning", MB_OK|MB_ICONWARNING);
		return;
	}

	if(rTemplateName.IsEmpty())
	{
		::MessageBox(NULL, "Input Template Name...!!", "Warning", MB_OK|MB_ICONWARNING);
		return;
	}

	CString rMapingData;
	const int nColCount = m_wndEditSpread.GetMaxCols();
	m_wndEditSpread.SetRow(1);
	for(int i = 1;i <= nColCount;++i)
	{
		m_wndEditSpread.SetCol(i);
		CString rValue = m_wndEditSpread.GetText();

		//if(rValue.IsEmpty()) continue;
		
		m_wndEditSpread.SetCol(m_wndEditSpread.SpreadHeader);
		//CString rHeader = m_wndEditSpread.GetText();
		char cCol = 'A';
		cCol = char(int(cCol) + i - 1);
		CString rHeader;
		rHeader.Format(_T("%c=") , cCol);
		rMapingData += (rHeader + rValue + '|');
	}

	if(rMapingData.Right(1) == '|')
	{
		rMapingData = rMapingData.Left(rMapingData.GetLength() - 1);
	}
	
	if(ERROR_SUCCESS == SaveData(rTemplateName, rRecordName, rStartRow, rExcelPath, rMapingData))
	{
		::MessageBox(NULL, "SUCEESS !", "SAVE", MB_OK|MB_ICONWARNING);	
	}
}	
/**
	@brief	SAVE DATA IN DB

	@author KHS	

	@date 2009-07-03 오후 4:07:00	

	@param	

	@return		
**/
int CExcelReportFormatView::SaveData(const CString& rTemplateName, const CString& rRecordName, const CString& rStartRow, const CString& rExcelPath, const CString& rMapingData)
{
	CString rProjectName, rProjectNo, rClient, rDate;

	m_wndProjectNoEdit.GetWindowText(rProjectNo);
	m_wndProjectNameEdit.GetWindowText(rProjectName);
	m_wndClientEdit.GetWindowText(rClient);
	m_wndDateEdit.GetWindowText(rDate);

	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string rMDBFilePath = docData.GetProjectMDBFilePath();
	
	//! 테이블이 없으면 생성합니다.
	TABLES::CELOADTable table;
	if(ERROR_SUCCESS != table.CreateTable(rMDBFilePath , "T_EXCEL_REPORT_FORMAT"))
	{
		::MessageBox(NULL, "fail , Create Table...!!", "Warning", MB_OK|MB_ICONWARNING);
		return ERROR_BAD_ENVIRONMENT;
	}

	CADODB adoDB;
	const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rConnectionString))
	{
		//! 기존에 입력한 값이 있으면 삭제한다.
		CString sSql = _T("DELETE * FROM T_EXCEL_REPORT_FORMAT WHERE C_TEMPLATE_NAME = '") + rTemplateName + _T("'");
		if(!adoDB.ExecuteQuery(sSql.operator LPCTSTR()))
		{
			::MessageBox(NULL, "fail, delete Table...!!", "Warning", MB_OK|MB_ICONWARNING);
			return ERROR_BAD_ENVIRONMENT;
		}

		CString strFormat = _T("INSERT INTO T_EXCEL_REPORT_FORMAT(C_RECORD_NAME,C_TEMPLATE_NAME,C_START_ROW,C_EXCEL_FILE_PATH,C_PROJECT_NO,C_PROJECT_NAME,C_CLIENT,C_DATE) VALUES('%s','%s','%s','%s','%s','%s','%s','%s')");
		CString strSQL;
		strSQL.Format(strFormat , rRecordName, rTemplateName, rStartRow, m_rExcelFileName, rProjectNo, rProjectName, rClient, rDate);
		adoDB.ExecuteQuery(strSQL.operator LPCTSTR());

		sSql = _T("SELECT * FROM T_EXCEL_REPORT_FORMAT WHERE C_TEMPLATE_NAME = '") + rTemplateName + _T("'");
		adoDB.OpenQuery(sSql.operator LPCTSTR());
		LONG lRecordCount = 0L;
		adoDB.GetRecordCount(&lRecordCount);
		if(1 == lRecordCount)
		{
			//!Temp 폴더가 없으면 생성
			if(!FileExist(docData.GetProjectFolderPath().c_str() + CString("Temp\\")))
			{
				CreateDirectory(docData.GetProjectFolderPath().c_str() + CString("Temp\\"), NULL);
			}

			//! LOG파일을 압축한다.
			CZipArchive zip;
			/*CString rZipFilePath(rExcelPath);*/
			CString rZipFilePath(docData.GetProjectFolderPath().c_str() + CString("Temp\\") + m_rExcelFileName);
			rZipFilePath.Replace(_T("xls") , _T("zip"));
			
			
			//!이전에 Zip 압축 파일이 존재하면 삭제한다.
			if(FileExist(rZipFilePath))
			{
				DeleteFile(rZipFilePath);
			}
			zip.Open(rZipFilePath , CZipArchive::create );
			zip.AddNewFile(rExcelPath , -1 , false);
			zip.Close();

			CFile f(rZipFilePath , CFile::modeRead);
			ULONG fSize = f.GetLength();
			char* pData = (char*)calloc(1 , sizeof(char)*fSize);
			if(pData)
			{
				f.Read(pData,fSize);
				const int nFieldCount = adoDB.GetFieldCount();
				for(int i = 0;i < nFieldCount;++i)
				{
					if(_T("C_EXCEL_FILE") == adoDB.GetFieldName(i))
					{
						adoDB.AppendChunk(0 , i , pData , fSize);
						free((void*)pData);
					}
					else if(_T("C_MAPING_DATA") == adoDB.GetFieldName(i))
					{
						adoDB.AppendChunk(0, i , (LPVOID)(rMapingData.operator LPCSTR()) , rMapingData.GetLength());
					}
				}
			}
		}
		return ERROR_SUCCESS;
	}
	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	이전에 작업한 TEMPLATE 이 있으면 불러온다.

	@author KHS	

	@date 2009-07-07 오전 10:15:22	

	@param	

	@return		
**/
int CExcelReportFormatView::LoadTemplate(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const STRING_T rMDBFilePath = docData.GetProjectMDBFilePath();

	CADODB adoDB;
	const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rConnectionString))
	{
		adoDB.OpenQuery(_T("SELECT * FROM T_EXCEL_REPORT_FORMAT"));
		LONG lRecordCount = 0L;
		STRING_T rValue;
		adoDB.GetRecordCount(&lRecordCount);
		for(int i=0;i< lRecordCount;++i)
		{
			adoDB.GetFieldValue(i, _T("C_TEMPLATE_NAME"), &rValue);
			if(-1 == m_wndTemplateNameCombo.FindString(-1, rValue.c_str()))
			{
				m_wndTemplateNameCombo.AddString(rValue.c_str());
			}
			
		}
		return ERROR_SUCCESS;
	}
	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	Template Change

	@author KHS	

	@date 2009-07-07 오전 10:29:36	

	@param	

	@return		
**/
void CExcelReportFormatView::OnCbnSelchangeTemplateNameCombo()
{
	CELoadDocData & docData = CELoadDocData::GetInstance();
	CString rTemplateName;
	const int nItem = m_wndTemplateNameCombo.GetCurSel();
	if(-1 != nItem)
	{
		m_wndTemplateNameCombo.GetLBText(nItem, rTemplateName);
	}

	if(rTemplateName.IsEmpty()) return;

	const string rMDBFilePath = docData.GetProjectMDBFilePath();

	CADODB adoDB;
	const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rConnectionString))
	{
		CString sSql = _T("SELECT * FROM T_EXCEL_REPORT_FORMAT WHERE C_TEMPLATE_NAME='") + rTemplateName + _T("'");
		adoDB.OpenQuery(sSql.operator LPCTSTR());
		
		LONG lRecordCount = 0L;
		STRING_T rRecordName, rTemplateName, rStartRow, rExcelFileName, rProjectNo, rProjectName, rClient, rDate;
		char *pData = NULL;
		long lSize = 0L;
		
		adoDB.GetRecordCount(&lRecordCount);
		if(1 == lRecordCount)
		{
			adoDB.GetFieldValue(0, _T("C_RECORD_NAME"), &rRecordName);
			adoDB.GetFieldValue(0, _T("C_TEMPLATE_NAME"), &rTemplateName);
			adoDB.GetFieldValue(0, _T("C_START_ROW"), &rStartRow);
			adoDB.GetFieldValue(0, _T("C_EXCEL_FILE_PATH"), &rExcelFileName);
			adoDB.GetFieldValue(0, _T("C_PROJECT_NO"), &rProjectNo);
			adoDB.GetFieldValue(0, _T("C_PROJECT_NAME"), &rProjectName);
			adoDB.GetFieldValue(0, _T("C_CLIENT"), &rClient);
			adoDB.GetFieldValue(0, _T("C_DATE"), &rDate);
			
			m_rExcelFileName = rExcelFileName.c_str();

			CString rZipFilePath = docData.GetProjectFolderPath().c_str() + CString(_T("Temp\\")) + rExcelFileName.c_str();
			rZipFilePath.Replace(_T("xls"), _T("zip"));
			adoDB.GetFieldSize(0 , _T("C_EXCEL_FILE") , lSize);
			if(pData = (char*)calloc(1 , sizeof(char)*lSize))
			{
				adoDB.GetBinaryValue(0 , _T("C_EXCEL_FILE") , (LPVOID)pData , lSize);
				CFile f(rZipFilePath , CFile::modeCreate|CFile:: modeReadWrite);
				f.Write(pData , lSize);
				f.Close();
				UncompressExcelFile(rZipFilePath, docData.GetProjectFolderPath().c_str() + CString("Temp\\"));
			}
			int nItem = m_wndSelectTableCombo.FindString(-1, rRecordName.c_str());
			if(-1 != nItem)
			{
				m_wndSelectTableCombo.SetCurSel(nItem);
				OnCbnSelchangeSelectTableCombo();
			}

			m_wndTemplateNameEdit.SetWindowText(rTemplateName.c_str());
			m_wndStartRowEdit.SetWindowText(rStartRow.c_str());

			CString rExcelPath = docData.GetProjectFolderPath().c_str() + CString("Temp\\") + rExcelFileName.c_str();

			m_wndExcelNameEdit.SetWindowText(rExcelPath);
			m_wndProjectNoEdit.SetWindowText(rProjectNo.c_str());
			m_wndProjectNameEdit.SetWindowText(rProjectName.c_str());
			m_wndClientEdit.SetWindowText(rClient.c_str());
			m_wndDateEdit.SetWindowText(rDate.c_str());

			if(!m_wndSpread.ImportExcelBook(rExcelPath, ""))
			{
				::MessageBox(NULL, "Fail, Load Excel File", "Warning", MB_OK|MB_ICONWARNING);
				return;
			}
			adoDB.GetFieldSize(0 , _T("C_MAPING_DATA") , lSize);
			if(pData = (char*)calloc(1 , sizeof(char)*lSize))
			{
				adoDB.GetBinaryValue(0 , _T("C_MAPING_DATA") , (LPVOID)pData , lSize);
			}

			vector<string> oResult, oResult1;
			CTokenizer<CIsFromString>::Tokenize(oResult, pData, CIsFromString("|"));
			if(!oResult.empty())
			{
				m_wndEditSpread.SetRow(1);
				for(int i =0; i < oResult.size();++i)
				{
					CTokenizer<CIsFromString>::Tokenize(oResult1, oResult[i], CIsFromString("="));

					m_wndEditSpread.SetCol(i + 1);
					int nFindItem = FindStringInSpread(oResult1[1].c_str());
					if(-1 != nFindItem)
					{
						m_wndEditSpread.SetTypeComboBoxCurSel(nFindItem);
					}
				}
			}
			if(pData)
			{
				free((void*)pData);
				pData = NULL;
			}
		}
	}
}
/**
	@brief	

	@author KHS	

	@date 2009-07-06 오후 1:53:34	

	@param	

	@return		
**/
int CExcelReportFormatView::UncompressExcelFile(const CString& rExcelPath, const CString& rFolderPath)
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

/**
	@brief	SPREAD COMBO BOX 에서 TEXT를 찾아 해당 인댁스 리턴

	@author KHS	

	@date 2009-07-07 오후 1:34:44	

	@param	

	@return		
**/
int CExcelReportFormatView::FindStringInSpread(const CString& rText)
{
	int nCount = m_wndEditSpread.GetTypeComboBoxCount();
	
	for(int i = 1; i <= nCount;++i)
	{
		m_wndEditSpread.SetTypeComboBoxIndex(i-1);
		CString rString = m_wndEditSpread.GetTypeComboBoxString();
		if(rText == rString) 
		{
			return (i-1);
		}
	}
	return -1;
}
