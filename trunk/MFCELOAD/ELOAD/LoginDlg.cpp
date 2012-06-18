// LoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELoad.h"
#include "LoginDlg.h"
#include "enablebuddybutton.h"
#include "ELoadDocData.h"
#include <gui/FolderDlg/FolderDlg.h>

static const string TABLE_NAME = _T("T_ELOAD_PROJECT");

// CLoginDlg dialog

IMPLEMENT_DYNCREATE(CLoginDlg, CDialog)

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoginDlg::IDD, pParent)
{
}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_STATIC_TITLE , m_wndStaticTitle);
	DDX_Control(pDX , IDC_STATIC_PROJECT , m_wndProjectNoStatic);
	DDX_Control(pDX , IDC_COMBO_PROJECT , m_wndProjectNoCombo);
	DDX_Text(pDX , IDC_COMBO_PROJECT , m_rServerFolderPath);
	DDX_Control(pDX , IDC_STATIC_ID , m_wndUserIDStatic);
	DDX_Control(pDX , IDC_EDIT_ID , m_wndUserID);
	DDX_Text(pDX , IDC_EDIT_ID , m_rID);
	DDX_Control(pDX , IDC_STATIC_PASSWORD , m_wndPasswordStatic);
	DDX_Control(pDX , IDC_EDIT_PASSWORD , m_wndPassword);
	DDX_Text(pDX , IDC_EDIT_FOLDER , m_rServerFolderPath);
	DDX_Control(pDX , IDC_STATIC_MSG , m_wndStaticMsg);

	DDX_Control(pDX , IDC_BUTTON_SELECT_SERVER , m_wndSelectServerButton);
	DDX_Control(pDX , IDOK , m_wndLoginButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);
}

/**
	@brief	

	@author	BHK
*/
BOOL CLoginDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	//! Set Visual Style
	CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));

	CFont* m_pBoldFont = new CFont;
	m_pBoldFont->CreateFont(20,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"Arial");

	m_wndStaticTitle.SetColor( RGB(0, 0, 0) );///GetSysColor(COLOR_BTNFACE) );
	m_wndStaticTitle.SetGradientColor( RGB(74, 84, 103) );
	m_wndStaticTitle.SetFont(m_pBoldFont);
	m_wndStaticTitle.SetVerticalGradient(TRUE);
	m_wndStaticTitle.SetTextAlign(DT_CENTER);
	m_wndStaticTitle.SetTextColor(RGB(255 , 255 , 255));

	m_wndProjectNoStatic.SetColor(RGB(245,245,245));
	m_wndProjectNoStatic.SetGradientColor(RGB(245,245,245));
	m_wndProjectNoStatic.SetTextColor(RGB(0, 0, 0));

	m_wndUserIDStatic.SetColor(RGB(245,245,245));
	m_wndUserIDStatic.SetGradientColor(RGB(245,245,245));
	m_wndUserIDStatic.SetTextColor(RGB(0, 0, 0));

	CString rRegEx="([a-zA-Z0-9])*";
	m_wndUserID.SetRegEx(rRegEx);
	m_wndUserID.CreateToolTip(this , "아이디로 사용할 수 없는 글자입니다.");
	m_wndUserID.SetBackgroundColourFocus (RGB (150, 200, 255));
	m_wndUserID.SetForegroundColourError (RGB (255, 255, 255));
	m_wndUserID.SetBackgroundColourError (RGB (255, 0, 0));
	m_wndUserID.SetWindowText(_T("ADMIN"));
	
	m_wndPasswordStatic.SetColor(RGB(245,245,245));
	m_wndPasswordStatic.SetGradientColor(RGB(245,245,245));
	m_wndPasswordStatic.SetTextColor(RGB(0, 0, 0));

	rRegEx = "([a-zA-Z0-9!@#$%^&*()_+])*";
	m_wndPassword.SetRegEx(rRegEx);
	CString rToolTip("암호로 사용할 수 없는 글자입니다.");
	m_wndPassword.CreateToolTip(this , rToolTip);
	m_wndPassword.SetBackgroundColourFocus (RGB (150, 200, 255));
	m_wndPassword.SetForegroundColourError (RGB (255, 255, 255));
	m_wndPassword.SetBackgroundColourError (RGB (255, 0, 0));
	m_wndPassword.SetWindowText(_T("XXXXXX"));

	CWnd* pWndFolder = GetDlgItem(IDC_EDIT_FOLDER);
	CWnd* pSelectServerButton = GetDlgItem(IDC_BUTTON_SELECT_SERVER);
	VERIFY( ::EnableBuddyButton( pWndFolder->GetSafeHwnd() , pSelectServerButton->GetSafeHwnd() , BBS_RIGHT ) );

	m_wndStaticMsg.SetGradientColor( RGB(255,0,0) );
	m_wndStaticMsg.SetColor( RGB(255,0,0) );
	m_wndStaticMsg.SetTextColor(RGB(0 , 0 , 0));

	CELoadDocData& docData = CELoadDocData::GetInstance();
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	m_wndLoginButton.SetTextHotColor(TextHotColor);
	m_wndCancelButton.SetTextHotColor(TextHotColor);
	
	HICON hOKIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_OK),
		IMAGE_ICON,
		16,16, 0 );
	m_wndLoginButton.SetIcon(hOKIcon);
	
	HICON hCancelIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_CANCEL),
		IMAGE_ICON,
		16,16, 0 );
	m_wndCancelButton.SetIcon(hCancelIcon);

	LoadAppSettingFile();

	SyncDatabaseStructure();
	ReadProjectNoFromELoadMDB();
	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_SERVER, &CLoginDlg::OnBnClickedButtonSelectServer)
	ON_BN_CLICKED(IDOK, &CLoginDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT_ID, &CLoginDlg::OnEnChangeEditId)
END_MESSAGE_MAP()

// CLoginDlg message handlers

/**	
	@brief	select server path

	@author	BHK

	@date	?

	@param	

	@return
*/
void CLoginDlg::OnBnClickedButtonSelectServer()
{
	UINT	uFlags	  = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
	CFolderDialog dlg(_T( "Select ELOAD Server Path" ), NULL , this , uFlags);
	if(IDOK == dlg.DoModal())
	{
		m_rServerFolderPath = dlg.GetFolderPath();
		CString rServerMDB = m_rServerFolderPath;
		if(_T("\\") != rServerMDB.Right(1)) rServerMDB += _T("\\");
		rServerMDB += _T("ELOAD.MDB");

		//! copy template mdb to ELOAD.MDB as server database if server database doesn't exist.
		if(FALSE == FileExist(rServerMDB))
		{
			CString rExecPath = GetExecPath();
			CString rTemplateMDB = (_T("\\") != rExecPath.Right(1)) ? (rExecPath + _T("\\")) : rExecPath;
			rTemplateMDB += _T("Backup\\ELOAD.MDB");
			::CopyFile(rTemplateMDB , rServerMDB , TRUE);
		}

		ReadProjectNoFromELoadMDB();

		UpdateData(FALSE);
	}
}

/**
	@brief	
	<li> 사용자가 입력한 값들을 ini 파일로 저장한다.

	@author	BHK

	@date	?

	@param

	@return
*/
void CLoginDlg::OnBnClickedOk()
{
	if(m_rServerFolderPath.IsEmpty())
	{
		DisplayMessage("먼저 서버 폴더를 설정하세요." , RGB(255,0,0) , RGB(0,0,0));
		return;
	}

	//! get Project No which is Selected.
	const int nSelItem = m_wndProjectNoCombo.GetCurSel();
	if(-1 != nSelItem)
		m_wndProjectNoCombo.GetLBText(nSelItem, m_rProjectNo);
	else	m_rProjectNo = _T("");

	string rServerMDB = m_rServerFolderPath;
	if('\\' != rServerMDB.at( rServerMDB.length() - 1)) rServerMDB += _T("\\");
	rServerMDB += _T("ELOAD.MDB");

	if(FALSE == FileExist(rServerMDB.c_str()))
	{
		CString rExecPath = GetExecPath();
		string rTemplateMDB = (_T("\\") != rExecPath.Right(1)) ? (rExecPath + _T("\\")).operator LPCSTR() : rExecPath.operator LPCSTR();
		rTemplateMDB += _T("Backup\\ELOAD.MDB");
		CopyFile(rTemplateMDB.c_str() ,rServerMDB.c_str() , TRUE);

		//! ADMIN만 로그인 할수 있다.
		if(_T("ADMIN") != m_rID)
		{
			DisplayMessage(_T("프로젝트가 없습니다. 사용자는 로그인할 수 없습니다.") , RGB(255,0,0) , RGB(0,0,0));
			
			return;
		}

		CString rPassword;
		GetDlgItemText(IDC_EDIT_PASSWORD , rPassword);
		if(ADMIN_PASSWORD != rPassword)
		{
			DisplayMessage(_T("관리자 암호가 틀립니다.") , RGB(255,0,0) , RGB(0,0,0));
			return;
		}
	}
	else
	{
		//! 검사를 통과하지 못하면 다이얼로그를 종료하지 않는다.
		//! ELOAD: id와 password를 검사한다.
		if((_T("USER") == m_rID) || (_T("ADMIN") == m_rID))
		{
			if(m_rProjectNo.IsEmpty())
			{
				DisplayMessage(_T("먼저 프로젝트를 설정하세요.") , RGB(255,0,0) , RGB(0,0,0));
				return;
			}

			if(_T("ADMIN") == m_rID)
			{
				CString rPassword;
				GetDlgItemText(IDC_EDIT_PASSWORD , rPassword);
				if(ADMIN_PASSWORD != rPassword)
				{
					DisplayMessage(_T("관리자 암호가 틀립니다.") , RGB(255,0,0) , RGB(0,0,0));
					return;
				}
			}
		}
		else
		{
			DisplayMessage(_T("유효하지 않는 ID입니다.") , RGB(255,0,0) , RGB(0,0,0));
			return;
		}
	}

	SaveAppSettingFile();

	CDialog::OnOK();
}

/**
	@brief	application setting file을 읽어 UI.에 뿌려준다.

	@author	BHK
*/
int CLoginDlg::LoadAppSettingFile(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const CString rAppSettingFilePath = docData.GetAppSettingFilePath();
	static const CString rApp("ELoad");
	TCHAR szBuf[MAX_PATH + 1] = {'\0'};

	if(GetPrivateProfileString(rApp , "Server" , "" , szBuf , MAX_PATH , rAppSettingFilePath))
	{
		m_rServerFolderPath = szBuf;
	}
	if(GetPrivateProfileString(rApp , "CurPrjName" , "" , szBuf , MAX_PATH , rAppSettingFilePath))
	{
		m_rProjectNo = szBuf;
	}
	if(GetPrivateProfileString(rApp , "CurID" , "" , szBuf , MAX_PATH , rAppSettingFilePath))
	{
		m_rID = szBuf;
		if("USER" == m_rID) m_wndPassword.EnableWindow(FALSE);
	}
	
	UpdateData(FALSE);

	return ERROR_SUCCESS;
}

/**
	@brief	ID가 수정되었을때...

	@author	HumKyung.BAEK
*/
void CLoginDlg::OnEnChangeEditId()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);

	//! 일단은 주석처리 함. 나중에 주석을 풀 예정
	/*
	if(_T("ADMIN") == m_rID)
	{
		m_wndPassword.EnableWindow(TRUE);
	}
	else if(_T("USER") == m_rID)
	{
		m_wndPassword.EnableWindow(FALSE);
	}
	*/
}

/**
	@brief	UI의 내용을 application setting file로 저장한다.

	@author	BHK
*/
int CLoginDlg::SaveAppSettingFile(void)
{
	UpdateData(TRUE);

	CELoadDocData& docData = CELoadDocData::GetInstance();
	const CString rAppSettingFilePath = docData.GetAppSettingFilePath();
	static const CString rApp("ELoad");
	
	WritePrivateProfileString(rApp , "Server" , m_rServerFolderPath , rAppSettingFilePath);
	WritePrivateProfileString(rApp , "CurPrjName" , m_rProjectNo , rAppSettingFilePath);
	WritePrivateProfileString(rApp , "CurID" , m_rID , rAppSettingFilePath);

	return ERROR_SUCCESS;
}

/**
	@brief	read project no from project setup database and add string to combo box.

	@author	BHK

	@return	
*/
int CLoginDlg::ReadProjectNoFromELoadMDB(void)
{
	CString str;

	if(m_rServerFolderPath.IsEmpty()) return ERROR_BAD_ENVIRONMENT;

	const STRING_T rMDBFilePath = m_rServerFolderPath.operator LPCTSTR() + STRING_T(_T("\\ELoad.mdb"));
	const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;

	CADODB adoDB;
	if(!adoDB.DBConnect(rConnectionString))
	{
		m_wndStaticMsg.SetGradientColor( RGB(255,0,0) );
		m_wndStaticMsg.SetColor( RGB(255,0,0) );///GetSysColor(COLOR_BTNFACE) );
		m_wndStaticMsg.SetWindowText("Fail to Connect ELoad DB.");
		
		return ERROR_BAD_ENVIRONMENT;
	}
	else
	{
		m_wndStaticMsg.SetGradientColor( GetSysColor(COLOR_BTNFACE) );
		m_wndStaticMsg.SetColor( GetSysColor(COLOR_BTNFACE) );
		m_wndStaticMsg.SetWindowText("Connect ELoad DB Success.");
	}

	if(!adoDB.OpenQuery("SELECT * FROM T_ELOAD_PROJECT"))
		return ERROR_BAD_ENVIRONMENT;

	CString tmp;
	LONG lRecordCount = 0;
	if(adoDB.GetRecordCount(&lRecordCount))
	{
		//! 기존에 들어 있는 내용을 지운다.
		const int nItemCount = m_wndProjectNoCombo.GetCount();
		for(int i = nItemCount - 1;i >= 0;--i) m_wndProjectNoCombo.DeleteString(i);

		STRING_T strValue;
		for(int i = 0;i < lRecordCount;++i)
		{
			adoDB.GetFieldValue(i , "C_PROJECT_NO" , &strValue);
			if(strValue.empty()) continue;

			if(-1 == m_wndProjectNoCombo.FindString(0 , strValue.c_str()))
			{
				m_wndProjectNoCombo.AddString(strValue.c_str());
				
				STRING_T rValue;
				adoDB.GetFieldValue(i , _T("C_PROJECT_NAME") , &rValue);
				m_ProjectInfoMap[strValue.c_str()].m_rProjectName = rValue.c_str();
				adoDB.GetFieldValue(i , _T("C_PROJECT_DESC") , &rValue);
				m_ProjectInfoMap[strValue.c_str()].m_rProjectDesc = rValue.c_str();
				adoDB.GetFieldValue(i , _T("C_PROJECT_CLIENT") , &rValue);
				m_ProjectInfoMap[strValue.c_str()].m_rProjectClient = rValue.c_str();
			}
		}
		
		const int nFound = m_wndProjectNoCombo.FindString(0 , m_rProjectNo);
		(-1 != nFound) ? m_wndProjectNoCombo.SetCurSel(nFound) : m_wndProjectNoCombo.SetCurSel(0);
		
		const int nItem = m_wndProjectNoCombo.GetCurSel();
		if(-1 != nItem) m_wndProjectNoCombo.GetLBText(nItem , m_rProjectNo);

		if(!adoDB.DBDisConnect())
		{
			m_wndStaticMsg.SetGradientColor( RGB(255,0,0) );
			m_wndStaticMsg.SetColor( RGB(255,0,0) );///GetSysColor(COLOR_BTNFACE) );
			m_wndStaticMsg.SetWindowText("Fail to Disconnect ELoad DB.");

			return ERROR_BAD_ENVIRONMENT;
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
int CLoginDlg::DisplayMessage(const string& rMsg , COLORREF bgColor , COLORREF fgColor)
{
	m_wndStaticMsg.SetGradientColor( bgColor );
	m_wndStaticMsg.SetColor( bgColor );
	m_wndStaticMsg.SetTextColor(fgColor);
	m_wndStaticMsg.SetWindowText( rMsg.c_str() );

	return ERROR_SUCCESS;
}

/**
	@brief	PROJECT NO에 해당하는 PROJECT NAME을 구한다.

	@author	BHK
*/
CString CLoginDlg::GetProjectNameOf(const CString& rProjectNo)
{
	map<CString,ProjectInfo>::iterator where = m_ProjectInfoMap.find(rProjectNo);
	if(where != m_ProjectInfoMap.end()) return (where->second.m_rProjectName.c_str());

	return CString("");
}

/**
	@brief	

	@author BHK	

	@date 2009-06-25 오후 2:44:24	

	@param	

	@return		
*/
CString CLoginDlg::GetProjectDescOf(const CString& rProjectNo)
{
	map<CString,ProjectInfo>::iterator where = m_ProjectInfoMap.find(rProjectNo);
	if(where != m_ProjectInfoMap.end()) return (where->second.m_rProjectDesc.c_str());

	return CString("");
}

/**
	@brief	

	@author BHK	

	@date 2009-06-25 오후 2:44:27	

	@param	

	@return		
*/
CString CLoginDlg::GetProjectClientOf(const CString& rProjectNo)
{
	map<CString,ProjectInfo>::iterator where = m_ProjectInfoMap.find(rProjectNo);
	if(where != m_ProjectInfoMap.end()) return (where->second.m_rProjectClient.c_str());

	return CString("");
}
/**
	@brief	MDB database 구조를 SQL파일에 기술한 내용대로 맞춘다.

	@author BHK	

	@date 2009-06-25 오후 1:42:36	

	@param	

	@return		
*/
int CLoginDlg::SyncDatabaseStructure(void)
{
	CADODB adoDB;
	const STRING_T rMDBFilePath = m_rServerFolderPath.operator LPCTSTR() + STRING_T(_T("\\ELoad.mdb"));
	const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rConnectionString))
	{
		CString rExecPath = GetExecPath();
		if("\\" != rExecPath.Right(1)) rExecPath += "\\";
		const CString rSqlFilePath = rExecPath + "Backup\\T_ELOAD_PROJECT.SQL";
		
		CSQLFile sqlFile(rSqlFilePath.operator LPCSTR());
		if(ERROR_SUCCESS != sqlFile.Parse()) return ERROR_BAD_ENVIRONMENT;

		if(FALSE == adoDB.OpenQuery(_T("SELECT * FROM ") + TABLE_NAME))
		{
			//! 테이블이 없으므로 테이블을 생성한다.	
			const string rSql = sqlFile.GetCreateString();
			if(!rSql.empty()) adoDB.ExecuteQuery(rSql.c_str());
		}
		else
		{
			vector<string> FieldNameEntry;

			//! database의 필드명을 미리 저장해 둔다.
			int nFieldCount = adoDB.GetFieldCount();
			for(int i = 0;i < nFieldCount;++i) FieldNameEntry.push_back(adoDB.GetFieldName(i));
			
			SyncToSqlFile(adoDB , sqlFile , FieldNameEntry);
		}

		return ERROR_SUCCESS;
	}
	else
	{
		AfxMessageBox(_T("Fail to connect database.h"));
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	SQL 파일과 형식을 일치시킨다.

	@author BHK	

	@date 2009-06-25 오후 1:47:01	

	@param	

	@return		
*/
int CLoginDlg::SyncToSqlFile(CADODB& adoDB , CSQLFile& sqlFile , vector<string>& FieldNameEntry)
{
	const int nFieldCount = sqlFile.GetColumnCount();
	for(int i = 0;i < nFieldCount;++i)
	{
		const string rFieldName = sqlFile.GetFieldNameAt(i);
		if(rFieldName.empty()) continue;

		vector<string>::iterator where = find(FieldNameEntry.begin() , FieldNameEntry.end() , rFieldName);
		if(where == FieldNameEntry.end())
		{
			//! 필드가 존재하지 않으면 테이블에 필드를 생성한다.
			adoDB.ExecuteQuery(string("ALTER TABLE " + TABLE_NAME + " ADD " + sqlFile.GetFieldDataAt(i)).c_str());
			FieldNameEntry.push_back(rFieldName);
		}
	}

	for(vector<string>::iterator itr = FieldNameEntry.begin();itr != FieldNameEntry.end();)
	{
		if(false == sqlFile.ExistColumnName(*itr))
		{
			//! 불필요한 필드는 테이블에서 삭제한다.
			adoDB.ExecuteQuery(string("ALTER TABLE " + TABLE_NAME + " DROP [" + *itr + "]").c_str());
			itr = FieldNameEntry.erase(itr);
		}else	itr++;
	}

	return ERROR_SUCCESS;
}