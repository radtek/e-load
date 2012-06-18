// NewProjectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELoad.h"
#include "NewProjectDlg.h"
#include "ELoadDocData.h"
#include "Tables/ProjectSettingTable.h"

// CNewProjectDlg dialog

IMPLEMENT_DYNAMIC(CNewProjectDlg, CDialogEx)

CNewProjectDlg::CNewProjectDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CNewProjectDlg::IDD, pParent)
{

}

CNewProjectDlg::~CNewProjectDlg()
{
}

void CNewProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX , IDC_EDIT_NO , m_wndProjectNoEdit);

	DDX_Text(pDX , IDC_EDIT_NO , m_rProjectNo);
	DDX_Text(pDX , IDC_EDIT_NAME , m_rProjectName);
	DDX_Text(pDX , IDC_EDIT_DESC , m_rProjectDesc);
	DDX_Text(pDX , IDC_COMBO_HERTZ , m_rProjectHertz);

	DDX_Control(pDX , IDC_BUTTON_NEW_PROJECT , m_wndNewProjectButton);
	DDX_Control(pDX , IDC_BUTTON_DELETE_PROJECT , m_wndDeleteProjectButton);

	DDX_Control(pDX , IDC_LIST_PROJECT , m_wndProjectListCtrl);

	DDX_Control(pDX , IDOK , m_wndOKButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);
}


BEGIN_MESSAGE_MAP(CNewProjectDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_NEW_PROJECT, &CNewProjectDlg::OnBnClickedButtonNewProject)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_PROJECT, &CNewProjectDlg::OnBnClickedButtonDeleteProject)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PROJECT, &CNewProjectDlg::OnLvnItemchangedListProject)
END_MESSAGE_MAP()


// CNewProjectDlg message handlers

/**
	@brief	dialog 초기화 부분.

	@author	BHK
*/
BOOL CNewProjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CELoadDocData& docData = CELoadDocData::GetInstance();

	static const CString rToolTip(_T("데이터 형식이 일치하지 않습니다."));
	const COLORREF WavyLineColor = docData.GetColorCode("FILTER_EDIT_WAVY_LINE_COLOR");
	const COLORREF BackFocusColor= docData.GetColorCode("FILTER_EDIT_BACK_FOCUS_COLOR");
	const COLORREF TextErrorColor= docData.GetColorCode("FILTER_EDIT_TEXT_ERROR_COLOR");
	const COLORREF BackErrorColor= docData.GetColorCode("FILTER_EDIT_BACK_ERROR_COLOR");

	const string rFileNameRegEx = docData.GetRegExp("FILENAME");
	m_wndProjectNoEdit.SetRegEx(rFileNameRegEx.c_str());
	m_wndProjectNoEdit.CreateToolTip(this , rToolTip);
	m_wndProjectNoEdit.SetWavyLineColour (WavyLineColor);
	m_wndProjectNoEdit.SetBackgroundColourFocus (BackFocusColor);
	m_wndProjectNoEdit.SetForegroundColourError (TextErrorColor);
	m_wndProjectNoEdit.SetBackgroundColourError (BackErrorColor);

	m_wndProjectListCtrl.InsertColumn(0 , _T("Project No.")    , LVCFMT_LEFT , 90);
	m_wndProjectListCtrl.InsertColumn(1 , _T("Project Name.")  , LVCFMT_LEFT , 120);
	m_wndProjectListCtrl.InsertColumn(2 , _T("Project Desc.")  , LVCFMT_LEFT , 150);
	m_wndProjectListCtrl.InsertColumn(3 , _T("Hertz") , LVCFMT_CENTER , 50);
        m_wndProjectListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0,
                LVS_EX_FULLROWSELECT);

	//! default valule is 60hz
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_HERTZ);
	if(pComboBox) pComboBox->SetCurSel(1);
	
	LoadProjectRecordFromDatabase();

	DisplayProjectRecord();

	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	m_wndOKButton.SetTextHotColor(TextHotColor);
	m_wndCancelButton.SetTextHotColor(TextHotColor);

	m_wndOKButton.SetIcon(IDI_OK_18X18);
	m_wndCancelButton.SetIcon(IDI_CANCEL_18X18);

///	EnableVisualManagerStyle (TRUE, TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	새로운 프로젝트를 생성한다.

	@author	BHK
*/
void CNewProjectDlg::OnBnClickedButtonNewProject()
{
	UpdateData(TRUE);

	if(m_rProjectNo.IsEmpty())
	{
		CString rString;
		rString.LoadString(IDS_NO_PROJECT_NO);
		AfxMessageBox(rString);
		return;
	}

	CELoadDocData& docData = CELoadDocData::GetInstance();
	//! 기존의 PROJECT NO와 같은지 비교한다.
	for(list<PROJECT_RECORD>::iterator itr = m_ProjectRecordEntry.begin();itr != m_ProjectRecordEntry.end();++itr)
	{
		CString rString(itr->rProjectNo.c_str());
                CString rProjectNo = m_rProjectNo;
		if(rString.MakeUpper() == rProjectNo.MakeUpper())
		{
			if(ERROR_SUCCESS == RemakeProjectSettingTable(rProjectNo)) 
			{
				//!프로젝트 셋팅을 UPTATE 한다.
				if(ERROR_SUCCESS == CurProjectUpdate())
				{
					DisplayProjectRecord();
					AfxMessageBox(_T("PROJECT를 갱신했습니다."));
				}
			}
			else
			{
				rString.LoadString(IDS_EXIST_PROJECT_NO);
				AfxMessageBox(rString);
			}

			return;
		}
	}

	//! 새로운 PROJECT를 등록한다.
	if(ERROR_SUCCESS == CreateProject())
	{
		//! ELOAD: 새로 생성된 PROJECT 정보를 ProjectRecordEntry에 추가한다.
		PROJECT_RECORD record;
		record.rProjectNo   = m_rProjectNo;
		record.rProjectName = m_rProjectName;
		record.rProjectDesc = m_rProjectDesc;
		record.rProjectHertz= m_rProjectHertz;
		m_ProjectRecordEntry.push_back( record );
		DisplayProjectRecord();

		AfxMessageBox(_T("Project creation is OK."));
	}
}

/**
	@brief	등록된 PROJECT RECORD를 DB에서 읽는다.

	@author	BHK
*/
int CNewProjectDlg::LoadProjectRecordFromDatabase(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	string rServerPath = docData.GetServerFolderPath();
	if('\\' != rServerPath.at(rServerPath.length() - 1)) rServerPath += _T("\\");
	const string rServerMDB = rServerPath + _T("ELOAD.MDB");

	///
	CADODB adoDB;
	const STRING_T rProvider = STRING_T(PROVIDER) + _T("Data Source=") + rServerMDB + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rProvider))
	{
		CString rQuery(_T("SELECT * FROM T_ELOAD_PROJECT"));
		try
		{ 
			adoDB.OpenQuery(rQuery.operator LPCTSTR());

			LONG lRecordCount = 0;
			adoDB.GetRecordCount(&lRecordCount);
			for(int i = 0;i < lRecordCount;++i)
			{
				STRING_T rValue;

				PROJECT_RECORD record;
				adoDB.GetFieldValue(i , _T("C_PROJECT_NO") , &rValue);
				record.rProjectNo   = rValue;
				adoDB.GetFieldValue(i , _T("C_PROJECT_NAME") , &rValue);
				record.rProjectName = rValue;
				adoDB.GetFieldValue(i , _T("C_PROJECT_DESC") , &rValue);
				record.rProjectDesc = rValue;

				const string rProjectMDB = rServerPath + record.rProjectNo + "\\" + record.rProjectNo + ".MDB";
				CProjectSettingTable table;
				if(ERROR_SUCCESS == table.Load(rProjectMDB,CProjectSettingTable::GetTableName()))
				{
					string* pValue = table.GetFieldValue(_T("C_HERTZ"));
					record.rProjectHertz = (NULL != pValue) ? pValue->c_str() : _T("50");
				}else	record.rProjectHertz = _T("-");
				///

				m_ProjectRecordEntry.push_back(record);
			}
		}
		catch(...)
		{
		}
	}
	else
	{
		AfxMessageBox(_T("Not Find DB... !"));
		return ERROR_BAD_ENVIRONMENT;
	}

	return ERROR_SUCCESS;
}

/**
	@brief	TEMPLATE DATABASE를 복사하고 하위 폴드를 생성한다.

	@author	BHK

	@date	?
*/
int CNewProjectDlg::CreateProject(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	//! ELOAD: ELOAD MDB에 PROJECT 정보를 INSERT한다.
	STRING_T rServerMDB = docData.GetServerMDBFilePath();
	CADODB adoDB;
	const STRING_T rProvider = STRING_T(PROVIDER) + _T("Data Source=") + rServerMDB + DB_PASSWORD; 
	if(TRUE == adoDB.DBConnect(rProvider))
	{
		CString rQuery = CString(_T("INSERT INTO T_ELOAD_PROJECT(C_PROJECT_NO,C_PROJECT_NAME,C_PROJECT_DESC) VALUES ")) +
			 _T("('") + m_rProjectNo + _T("','") + m_rProjectName + _T("','") + m_rProjectDesc + _T("')");
		if(FALSE == adoDB.ExecuteQuery(rQuery.operator LPCTSTR()))
		{
			AfxMessageBox(_T("Fail to insert record"));
			return ERROR_BAD_ENVIRONMENT;
		}
	}
	else
	{
		AfxMessageBox(_T("Fail to connect db."));
		return ERROR_BAD_ENVIRONMENT;
	}

	//! ELOAD: TEMPLATE MDB를 서버 경로에 Project No 이름으로 복사한다.
	CString rExecPath = GetExecPath();
	if(_T("\\") != rExecPath.Right(1)) rExecPath += _T("\\");

	string rProjectFolder = docData.GetServerFolderPath();
	if('\\' != rProjectFolder.at(rProjectFolder.length() - 1)) rProjectFolder += "\\";
	rProjectFolder += m_rProjectNo;
	if(TRUE == CreateDirectory(rProjectFolder.c_str() , NULL))
	{
		string rProjectMDBFilePath;
		rProjectMDBFilePath = rProjectFolder + _T("\\") + string(m_rProjectNo.operator LPCSTR()) + _T(".MDB");
		if(FALSE == CopyFile(rExecPath + _T("Backup\\TEMPLATE.MDB") , rProjectMDBFilePath.c_str() , TRUE))
		{
			return ERROR_FILE_NOT_FOUND;
		}
		{
			CADODB adoProjectDB;
			const STRING_T rProvider = STRING_T(PROVIDER) + _T("Data Source=") + rProjectMDBFilePath + DB_PASSWORD; 
			if(TRUE == adoProjectDB.DBConnect(rProvider))
			{
				const CString rSqlFilePath = GetExecPath() + _T("\\Backup\\T_PROJECT_SETTING.SQL");
				CSQLFile sqlFile(rSqlFilePath.operator LPCSTR());
				if(ERROR_SUCCESS == sqlFile.Parse())
				{
					sqlFile.SyncToTable(adoProjectDB , _T("T_PROJECT_SETTING"));

					const CString rSql = _T("INSERT INTO T_PROJECT_SETTING(C_HERTZ) VALUES('") + m_rProjectHertz + _T("')");
					adoProjectDB.ExecuteQuery(rSql.operator LPCTSTR());
				}
			}
		}

		CString rCommonLibraryFilePath = CString(rProjectFolder.c_str()) + _T("\\COMMON_LIBRARY.MDB");
		if(FALSE == CopyFile(rExecPath + _T("Backup\\COMMON_LIBRARY.MDB") , rCommonLibraryFilePath , TRUE))
		{
			return ERROR_FILE_NOT_FOUND;
		}
		
		CString rWorkingFolderPath = rProjectFolder.c_str() + CString(_T("\\Working"));
		if(!FileExist(rWorkingFolderPath))
		{
			CreateDirectory(rWorkingFolderPath, NULL);
		}

		CString rControlCableResultFolderPath = rProjectFolder.c_str() + CString("\\Control Cable Result");
		if(!FileExist(rControlCableResultFolderPath))
		{
			CreateDirectory(rControlCableResultFolderPath, NULL);
		}

		CString rUserCableResultFolderPath = rProjectFolder.c_str() + CString("\\User Cable Result");
		if(!FileExist(rUserCableResultFolderPath))
		{
			CreateDirectory(rUserCableResultFolderPath, NULL);
		}

		CString rTempFolderPath = rProjectFolder.c_str() + CString("\\Temp");
		if(!FileExist(rTempFolderPath))
		{
			CreateDirectory(rTempFolderPath, NULL);
		}

		return ERROR_SUCCESS;
	}

	return ERROR_CANNOT_MAKE;
}

/**
	@brief	PROJECT RECORD를 리스트에 표기한다.

	@author
*/
int CNewProjectDlg::DisplayProjectRecord(void)
{
	m_wndProjectListCtrl.DeleteAllItems();	//! 기존의 항목을 삭제한다.

	int nIndex = 0;
	for(list<PROJECT_RECORD>::iterator itr = m_ProjectRecordEntry.begin();itr != m_ProjectRecordEntry.end();++itr)
	{
		const int nItem = m_wndProjectListCtrl.InsertItem(nIndex++ , itr->rProjectNo.c_str());
		if(-1 != nItem)
		{
			m_wndProjectListCtrl.SetItemText(nItem , 1 , itr->rProjectName.c_str());
			m_wndProjectListCtrl.SetItemText(nItem , 2 , itr->rProjectDesc.c_str());
			m_wndProjectListCtrl.SetItemText(nItem , 3 , itr->rProjectHertz.c_str());
		}
	}

	return 0;
}

/**
	@brief	사용자가 선택한 PROJECT들을 삭제한다.

	@author	KHS
*/
void CNewProjectDlg::OnBnClickedButtonDeleteProject()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	string rServerMDB = docData.GetServerMDBFilePath();
	CADODB adoDB;
	const STRING_T rProvider = STRING_T(PROVIDER) + _T("Data Source=") + rServerMDB + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rProvider))
	{
		for (int nItem = m_wndProjectListCtrl.GetItemCount() - 1 ; nItem >= 0 ; nItem--)
                {
                        UINT uState = m_wndProjectListCtrl.GetItemState(nItem, LVIS_SELECTED);
                        if ((uState & LVIS_SELECTED) == LVIS_SELECTED)
                        {
                                CString rItemText = m_wndProjectListCtrl.GetItemText(nItem, 0);
                                CString rQuery = _T("DELETE FROM T_ELOAD_PROJECT WHERE C_PROJECT_NO='");
                                rQuery += (rItemText + "'" );

                                if(FALSE == adoDB.ExecuteQuery(rQuery.operator LPCTSTR()))
                                {
                                        AfxMessageBox("Fail to delete record");
                                        return ;
                                }

                                ///DeleteProjectMdb();

                                m_wndProjectListCtrl.DeleteItem(nItem);

                                for(list<PROJECT_RECORD>::iterator itr = m_ProjectRecordEntry.begin();itr != m_ProjectRecordEntry.end();++itr)
                                {
                                        CString rString = itr->rProjectNo.c_str();
                                        CString rProjectNo = rItemText;
                                        if(rProjectNo.MakeUpper() == rString.MakeUpper())
                                        {
                                                //! rename project folder name
						CString rServFolderPath = docData.GetServerFolderPath().c_str();
						if(_T("\\") == rServFolderPath.Right(1)) rServFolderPath = rServFolderPath.Left(rServFolderPath.GetLength() - 1);
						CString rOldName = rServFolderPath + _T("\\") + CString(itr->rProjectNo.c_str()) , 
							rNewName = rServFolderPath + _T("\\") + CString((_T("DELETED - ") + itr->rProjectNo).c_str());

						rename(rOldName , rNewName);

						m_ProjectRecordEntry.erase(itr);
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

struct MatchProjectNo : public binary_function<CNewProjectDlg::PROJECT_RECORD,string,bool>
{
        bool operator()(const CNewProjectDlg::PROJECT_RECORD lhs , const string& rhs) const
	{
		return (lhs.rProjectNo == rhs);
	}
};
/**
	@brief	T_PROJECT_SETTING 테이블을 재건한다.

	@author	BHK
*/
int CNewProjectDlg::RemakeProjectSettingTable(const CString& rProjectNo)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	string rServerFolderPath = docData.GetServerFolderPath();
	if('\\' != rServerFolderPath[rServerFolderPath.length() - 1]) rServerFolderPath += "\\";

	CString rProjectMDB = CString(rServerFolderPath.c_str()) + rProjectNo + "\\" + rProjectNo + ".MDB";
	rProjectMDB.MakeUpper();
	if(FileExist(rProjectMDB))
	{
		CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
		///docData.m_ProjectSettingTable.Load(_T("") , CProjectSettingTable::GetTableName());

		CString rCurProjectMDBFilePath = pSettingTable->GetMDBFilePath().c_str();
		rCurProjectMDBFilePath.MakeUpper();
		if(rCurProjectMDBFilePath != rProjectMDB)
		{
			CProjectSettingTable table;
			if(ERROR_SUCCESS == table.Load(rProjectMDB.operator LPCSTR(),CProjectSettingTable::GetTableName()))
			{
				string* pValue = table.GetFieldValue(_T("C_HERTZ"));
				if(NULL == pValue) table.AddField(_T("C_HERTZ"));

				table.SetFieldValue(_T("C_HERTZ") , m_rProjectHertz.operator LPCSTR());
				table.Save();

				list<CNewProjectDlg::PROJECT_RECORD>::iterator where = find_if(m_ProjectRecordEntry.begin(), m_ProjectRecordEntry.end(), bind2nd(MatchProjectNo(),m_rProjectNo));
				if(where != m_ProjectRecordEntry.end())
				{
					where->rProjectName = m_rProjectName;
					where->rProjectDesc = m_rProjectDesc;
					where->rProjectHertz = m_rProjectHertz;
				}

				return ERROR_SUCCESS;
			}
			else
			{
				AfxMessageBox("Fail to connect project db.");
				return ERROR_FILE_NOT_FOUND;
			}
		}
		else
		{
			///if(ERROR_SUCCESS == docData.m_ProjectSettingTable.Load(_T("") , CProjectSettingTable::GetTableName()))
			{
				string* pValue = pSettingTable->GetFieldValue(_T("C_HERTZ"));
				if(NULL == pValue) pSettingTable->AddField(_T("C_HERTZ"));

				pSettingTable->SetFieldValue(_T("C_HERTZ") , m_rProjectHertz.operator LPCSTR());
				pSettingTable->Save();

				list<CNewProjectDlg::PROJECT_RECORD>::iterator where = find_if(m_ProjectRecordEntry.begin(), m_ProjectRecordEntry.end(), bind2nd(MatchProjectNo(),m_rProjectNo));
				if(where != m_ProjectRecordEntry.end())
				{
					where->rProjectName = m_rProjectName;
					where->rProjectDesc = m_rProjectDesc;
					where->rProjectHertz = m_rProjectHertz;
				}

				return ERROR_SUCCESS;
			}
			/*
			else
			{
				AfxMessageBox("Fail to connect project db.");
				return ERROR_FILE_NOT_FOUND;
			}
			*/
		}
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author	BHK
*/
void CNewProjectDlg::OnLvnItemchangedListProject(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	
	m_rProjectNo   = m_wndProjectListCtrl.GetItemText(pNMLV->iItem , 0);
	m_rProjectName = m_wndProjectListCtrl.GetItemText(pNMLV->iItem , 1);
	m_rProjectDesc = m_wndProjectListCtrl.GetItemText(pNMLV->iItem , 2);
	m_rProjectHertz= m_wndProjectListCtrl.GetItemText(pNMLV->iItem , 3);
	UpdateData(FALSE);
	
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_HERTZ);
	if(pComboBox)
	{
		const int nFound = pComboBox->FindString(-1 , m_rProjectHertz);
		if(-1 != nFound) pComboBox->SetCurSel(nFound);
	}

	*pResult = 0;
}

/*
	@brief  프로젝트 속성 변경 후 ELOAD  MDB 업데이트

	@author KHS
**/
int CNewProjectDlg::CurProjectUpdate(void)
{
	CELoadDocData &docData = CELoadDocData::GetInstance();
	STRING_T rServerMDB = docData.GetServerMDBFilePath();
	const STRING_T rProvider = STRING_T(PROVIDER) + _T("Data Source=") + rServerMDB + DB_PASSWORD;
	CADODB adoDb;
	CString rQuery;
	if(TRUE == adoDb.DBConnect(rProvider))
	{
		for(list<CNewProjectDlg::PROJECT_RECORD>::iterator itr= m_ProjectRecordEntry.begin(); itr != m_ProjectRecordEntry.end();++itr)
		{	
			rQuery = "UPDATE T_ELOAD_PROJECT SET [C_PROJECT_NAME]='" + CString(itr->rProjectName.c_str()) +"' WHERE [C_PROJECT_NO]='" + CString(itr->rProjectNo.c_str()) +"'";
			if(FALSE == adoDb.OpenQuery(rQuery.operator LPCTSTR()))
			{
				AfxMessageBox("fail DB update...");
				return ERROR_BAD_ENVIRONMENT;
			}
			rQuery = "UPDATE T_ELOAD_PROJECT SET [C_PROJECT_DESC]='" + CString(itr->rProjectDesc.c_str()) +"' WHERE [C_PROJECT_NO]='" + CString(itr->rProjectNo.c_str()) +"'";
			if(FALSE == adoDb.OpenQuery(rQuery.operator LPCTSTR()))
			{
				AfxMessageBox("fail DB update...");
				return ERROR_BAD_ENVIRONMENT;
			}
			rQuery = "UPDATE T_ELOAD_PROJECT SET [C_PROJECT_HERTZ]='" + CString(itr->rProjectHertz.c_str()) +"' WHERE [C_PROJECT_NO]='" + CString(itr->rProjectNo.c_str()) +"'";
			if(FALSE == adoDb.OpenQuery(rQuery.operator LPCTSTR()))
			{
				AfxMessageBox( _T("fail DB update...") );
				return ERROR_BAD_ENVIRONMENT;
			}
		}
	}
	else
	{
		AfxMessageBox("Not Connect DB...");
		return ERROR_BAD_ENVIRONMENT;
	}
	return ERROR_SUCCESS;
}
