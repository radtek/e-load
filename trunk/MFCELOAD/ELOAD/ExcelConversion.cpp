// ExcelConversion.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "ExcelConversion.h"


// CExcelConversion dialog

IMPLEMENT_DYNAMIC(CExcelConversion, CDialog)

CExcelConversion::CExcelConversion(CWnd* pParent /*=NULL*/)
	: CDialog(CExcelConversion::IDD, pParent)
        , m_rExcelFileName(_T(""))
        , m_ExcelFilePath(_T(""))
{

}

CExcelConversion::~CExcelConversion()
{
}

void CExcelConversion::DoDataExchange(CDataExchange* pDX)
{
        CDialog::DoDataExchange(pDX);
        DDX_Control(pDX, IDC_SPREAD1, m_wndSpread1);
        DDX_Control(pDX, IDC_SPREAD2, m_wndSpread2);
        DDX_Text(pDX, IDC_EDIT_EXCEL_PATH, m_ExcelFilePath);
}


BEGIN_MESSAGE_MAP(CExcelConversion, CDialog)
        ON_BN_CLICKED(IDC_EXCEL_OPEN, &CExcelConversion::OnBnClickedExcelOpen)
        ON_BN_CLICKED(ID_EXCEL_IMPORT, &CExcelConversion::OnBnClickedExcelImport)
END_MESSAGE_MAP()


// CExcelConversion message handlers

BOOL CExcelConversion::OnInitDialog()
{
        CDialog::OnInitDialog();

        // TODO:  Add extra initialization here
        m_wndSpread1.SetAllowDragDrop(TRUE);
        ///m_wndSpread1.set

        return TRUE;  // return TRUE unless you set the focus to a control
        // EXCEPTION: OCX Property Pages should return FALSE
}

void CExcelConversion::OnBnClickedExcelOpen()
{
        // TODO: Add your control notification handler code here
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

		m_ExcelFilePath = strFileName;
		m_rExcelFileName = strFileTitle;
		
	}
	else return;

        ExcelOpen();

	UpdateData(FALSE);
}
/**
        @brief  Excel파일을  SPREAD 로 OPEN 한다
        @author KHS
*/
int CExcelConversion::ExcelOpen(void)
{
        if(FALSE == m_wndSpread1.ImportExcelBook((LPCSTR)m_ExcelFilePath, ""))
        {
                return ERROR_BAD_ENVIRONMENT;
        }

        return ERROR_SUCCESS;
}

void CExcelConversion::OnBnClickedExcelImport()
{
        // TODO: Add your control notification handler code here
        AfxMessageBox("Excel Import");
}
BEGIN_EVENTSINK_MAP(CExcelConversion, CDialog)
        ON_EVENT(CExcelConversion, IDC_SPREAD1, 37, CExcelConversion::OLEGiveFeedbackSpread1, VTS_PI4 VTS_PBOOL)
END_EVENTSINK_MAP()

void CExcelConversion::OLEGiveFeedbackSpread1(long* Effect, BOOL* DefaultCursors)
{
        // TODO: Add your message handler code here
}
