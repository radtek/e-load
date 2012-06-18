// ProjectInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "ProjectInfoDlg.h"
#include "ELoadDocData.h"

// CProjectInfoDlg dialog

IMPLEMENT_DYNAMIC(CProjectInfoDlg, CDialog)

CProjectInfoDlg::CProjectInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProjectInfoDlg::IDD, pParent)
{

}

CProjectInfoDlg::~CProjectInfoDlg()
{
}

void CProjectInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX , IDOK , m_wndOKButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);
}


BEGIN_MESSAGE_MAP(CProjectInfoDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CProjectInfoDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CProjectInfoDlg message handlers

BOOL CProjectInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CELoadDocData& docData = CELoadDocData::GetInstance();

	static const CString rToolTip(_T("데이터 형식이 일치하지 않습니다."));
	const COLORREF WavyLineColor = docData.GetColorCode("FILTER_EDIT_WAVY_LINE_COLOR");
	const COLORREF BackFocusColor= docData.GetColorCode("FILTER_EDIT_BACK_FOCUS_COLOR");
	const COLORREF TextErrorColor= docData.GetColorCode("FILTER_EDIT_TEXT_ERROR_COLOR");
	const COLORREF BackErrorColor= docData.GetColorCode("FILTER_EDIT_BACK_ERROR_COLOR");
	
	LoadProjectRecordFromDatabase();

	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	m_wndOKButton.SetTextHotColor(TextHotColor);
	m_wndCancelButton.SetTextHotColor(TextHotColor);

	m_wndOKButton.SetIcon(IDI_OK_18X18);
	m_wndCancelButton.SetIcon(IDI_CANCEL_18X18);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	등록된 PROJECT RECORD를 DB에서 읽는다.

	@author BHK	

	@date 2009-06-25 오후 1:12:01	

	@param	

	@return		
*/
int CProjectInfoDlg::LoadProjectRecordFromDatabase(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	string rServerPath = docData.GetServerFolderPath();
	if('\\' != rServerPath.at(rServerPath.length() - 1)) rServerPath += "\\";
	const string rServerMDB = rServerPath + _T("ELOAD.MDB");

	///
	CADODB adoDB;
	const STRING_T rProvider = STRING_T(PROVIDER) + _T("Data Source=") + rServerMDB + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rProvider))
	{
		CString rQuery("SELECT * FROM T_ELOAD_PROJECT WHERE C_PROJECT_NO='" + CString(docData.GetProjectNo().c_str()) + _T("'"));
		try
		{
			adoDB.OpenQuery(rQuery.operator LPCTSTR());

			LONG lRecordCount = 0;
			adoDB.GetRecordCount(&lRecordCount);
			if(1 == lRecordCount)
			{
				STRING_T rValue;

				adoDB.GetFieldValue(0 , _T("C_PROJECT_NO") , &rValue);
				SetDlgItemText(IDC_EDIT_NO , rValue.c_str());
				adoDB.GetFieldValue(0 , _T("C_PROJECT_NAME") , &rValue);
				SetDlgItemText(IDC_EDIT_NAME , rValue.c_str());
				adoDB.GetFieldValue(0 , _T("C_PROJECT_DESC") , &rValue);
				SetDlgItemText(IDC_EDIT_DESC , rValue.c_str());
				adoDB.GetFieldValue(0 , _T("C_PROJECT_CLIENT") , &rValue);
				SetDlgItemText(IDC_EDIT_CLIENT , rValue.c_str());
			}
		}
		catch(...)
		{
		}
	}
	else
	{
		AfxMessageBox("Not Find DB... !");
		return ERROR_BAD_ENVIRONMENT;
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-06-25 오후 2:01:59	

	@param	

	@return		
*/
void CProjectInfoDlg::OnBnClickedOk()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	string rServerPath = docData.GetServerFolderPath();
	if('\\' != rServerPath.at(rServerPath.length() - 1)) rServerPath += "\\";
	const string rServerMDB = rServerPath + _T("ELOAD.MDB");

	///
	CADODB adoDB;
	const STRING_T rProvider = STRING_T(PROVIDER) + _T("Data Source=") + rServerMDB + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rProvider))
	{
		CString rFormat("UPDATE T_ELOAD_PROJECT SET [C_PROJECT_NAME]='%s',[C_PROJECT_DESC]='%s',[C_PROJECT_CLIENT]='%s' WHERE C_PROJECT_NO='" + CString(docData.GetProjectNo().c_str()) + _T("'"));
		CString rName , rDesc , rClient;
		GetDlgItemText(IDC_EDIT_NAME , rName);
		GetDlgItemText(IDC_EDIT_DESC , rDesc);
		GetDlgItemText(IDC_EDIT_CLIENT , rClient);
		CString rSQL;
		rSQL.Format(rFormat , rName , rDesc , rClient);
		try
		{ 
			adoDB.ExecuteQuery(rSQL.operator LPCTSTR());
			docData.SetProjectName(rName.operator LPCSTR());
			docData.SetProjectDesc(rDesc.operator LPCSTR());
			docData.SetProjectClient(rClient.operator LPCSTR());
		}
		catch(...)
		{
		}
	}
	else
	{
		AfxMessageBox("Not Find DB... !");
		return;
	}

	OnOK();
}
