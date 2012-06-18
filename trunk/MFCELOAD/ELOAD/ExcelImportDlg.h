#pragma once
#include "afxwin.h"
#include <util/ado/ADODB.h>
#include <util/excel9/ExcelAutomation.h>
#include <gui/gridctrl/GridCtrl.h>
#include "GridCtrlEx.h"
#include <vector>
#include <string>
#include <list>
#include "MFCButtonEx.h"

using namespace std;

class CMyBrowseEdit : public CMFCEditBrowseCtrl
{
	virtual void OnBrowse()
	{
		CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY , _T("Excel Files(*.xls,*.xlsx)|*.xls;*.xlsx|All Files(*.*)|*.*||"));
		if(IDOK == fileDlg.DoModal())
		{
			CString strPathName = fileDlg.GetPathName();
			CString strFileTitle = fileDlg.GetFileTitle();
			CFile myFile;
			CFileException fileException;

			if(!myFile.Open(strPathName, CFile::modeRead, &fileException))
			{
				TRACE(_T("Can't open file %s, error = %u\n") , strPathName , fileException.m_cause);
			}
			myFile.Close();

			m_sFilePath = strPathName;
			m_sFileName = strFileTitle;
		}else return;

		SetWindowText(m_sFilePath);
	}
public:
	CString m_sFilePath , m_sFileName;
};

class CExcelImportDlg : public CDialog
{
	DECLARE_DYNAMIC(CExcelImportDlg)

public:
	CExcelImportDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CExcelImportDlg();

// Dialog Data
	enum { IDD = IDD_EXCEL_IMPORT_DIALOG };

	CMyBrowseEdit m_wndEditBrowseCtrl;
	CMFCButtonEx m_wndOKButton;
	CMFCButtonEx m_wndCancelButton;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        DECLARE_MESSAGE_MAP()
public:
	CString m_sExcelFilePath;
	CString m_sStartRow;
	afx_msg void OnBnClickedOk();
public:
        CGridCtrlEx m_Grid;

        typedef struct
	{
		string rField;
		string aryExlCell[3];
	}EXCEL_MAPPING_RECORD;

private:
	CString rFileName;
	CString m_rFieldName;
	BOOL IsDBOpen;
	LONG m_nCountOfImport;
	LONG m_nCountOfImportFail;
public:
	BOOL TableExist(CADODB* pAdoDb, const CString rTableName);
	int GetColumnCount(CExcelAutomation& xlAutomation, const int& nStartColumn);
	static int GetRowCount(CExcelAutomation& xlApp, const int& nStartRow, const int& nCol);
	bool DropMdbTable(CADODB* pAdoDb, const CString& rTableName);
	bool CheckProgram(void);
	CString m_rFieldSet;
	bool SetDataToDatabase(CADODB* pAdoDb);
private:
	static UINT StatusThreadEntry(LPVOID pVoid);
	UINT StatusThread();
	int CreateGridCtrl();

	int	m_nTryCount , m_nSuccessCount;/// , m_nFailCount;

public:
	int AppendFieldInDataBase(CADODB* pAdoDb);
	int ExecuteQuery(CADODB* pAdoDb, const CString& rQuery);
	virtual BOOL OnInitDialog();
	int WriteExcelMappingInIni(const CString& rIniFilePath);
	int ImportLoadDataFromExcel(CExcelAutomation& xlAutomation);

	vector<EXCEL_MAPPING_RECORD> m_FieldEntry;
	int OpenQuery(CADODB* pAdoDb, const CString& rQuery);
private:
	BOOL WriteExcelMappingInMDB(void);
	int ReadField(void);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	string GetResultString(void);
	int LoadImportErrorCheck(const string& rCategory, const string& rKey, const string& rValue);
};
