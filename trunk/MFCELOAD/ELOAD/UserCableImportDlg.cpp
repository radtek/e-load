// UserCableImportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "UserCableImportDlg.h"
#include "ELoadDocData.h"

// CUserCableImportDlg dialog

IMPLEMENT_DYNAMIC(CUserCableImportDlg, CDialog)

CUserCableImportDlg::CUserCableImportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUserCableImportDlg::IDD, pParent)
	, m_nOption(0)
{

}

CUserCableImportDlg::~CUserCableImportDlg()
{
}

void CUserCableImportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX , IDC_DIR_OPEN , m_wndOpenButton);
	DDX_Control(pDX , IDOK , m_wndOKButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);
	DDX_Text(pDX , IDC_DIR_EDIT , m_rUserCableExcelPath);
	DDX_Radio(pDX, IDC_RADIO_NEW_LOADS, m_nOption);
}


BEGIN_MESSAGE_MAP(CUserCableImportDlg, CDialog)
	ON_BN_CLICKED(IDC_DIR_OPEN, &CUserCableImportDlg::OnBnClickedDirOpen)
	ON_BN_CLICKED(IDOK, &CUserCableImportDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO_UPDATE_LOADS, &CUserCableImportDlg::OnBnClickedRadioUpdateLoads)
	ON_BN_CLICKED(IDC_RADIO_NEW_LOADS, &CUserCableImportDlg::OnBnClickedRadioNewLoads)
END_MESSAGE_MAP()


// CUserCableImportDlg message handlers

/**
	@brief	

	@author BHK	

	@date 2009-07-01 오후 7:21:22	

	@param	

	@return		
*/
void CUserCableImportDlg::OnBnClickedDirOpen()
{
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY  ,"Excel Files(*.xls)|*.xls|All Files(*.*)|*.*||");
	if(IDOK == fileDlg.DoModal())
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
		m_rUserCableExcelPath = strFileName;
	}
	else return;

	UpdateData(FALSE);
}

/**
	@brief	

	@author BHK	

	@date 2009-07-01 오후 7:39:55	

	@param	

	@return		
*/
BOOL CUserCableImportDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CELoadDocData& docData = CELoadDocData::GetInstance();
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	m_wndOKButton.SetIcon(IDI_EXCEL_IMPORT);
	m_wndOKButton.SetTextHotColor(TextHotColor);

	m_wndCancelButton.SetIcon(IDI_CANCEL_18X18);
	m_wndCancelButton.SetTextHotColor(TextHotColor);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CUserCableImportDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

/**
	@brief	

	@author BHK	

	@date 2009-07-06 오후 1:38:17	

	@param	

	@return		
*/
void CUserCableImportDlg::OnBnClickedRadioUpdateLoads()
{
	m_nOption = 1;
	UpdateData(FALSE);
}

/**
	@brief	

	@author BHK	

	@date 2009-07-06 오후 1:38:31	

	@param	

	@return		
*/
void CUserCableImportDlg::OnBnClickedRadioNewLoads()
{
	m_nOption = 0;
	UpdateData(FALSE);
}
