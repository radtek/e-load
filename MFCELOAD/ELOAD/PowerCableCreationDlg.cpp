// PowerCableCreationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "PowerCableCreationDlg.h"
#include "pyfromc.h"

#include "ELoadDocData.h"
#include "Tables/CableCreationSettingTable.h"
#include "CableItem.h"
#include "commands/CommandPowerCableCreation.h"
#include "CableSizingResultView.h"
#include "OutputWnd.h"
#include "Tables/CableDataListTable.h"
#include "PowerCableCreationSettingDlg.h"
#include "MainFrm.h"
#include "commands/CommandUndo.h"
#include "DataResultDlg.h"
#include <util/FileTools.h>

#define	READ_ONLY_COLOR	RGB(220, 220, 220)
#define	GRID_TEXT_COLOR	RGB(0, 0, 0)

// CPowerCableCreationSettingDlg dialog

IMPLEMENT_DYNAMIC(CPowerCableCreationDlg, CDialog)

CPowerCableCreationDlg::CPowerCableCreationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPowerCableCreationDlg::IDD, pParent)
{
	m_nLoadCableCreationRadio = 1;
	m_nBusCableCreationRadio = 1;
}

CPowerCableCreationDlg::~CPowerCableCreationDlg()
{
}

void CPowerCableCreationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, ID_POWER_CABLE_CREATION, m_wndCableCreationButton);
	DDX_Control(pDX, IDCANCEL, m_wndCancelButton);
	DDX_Radio(pDX, IDC_RADIO_LOAD_ALL_POWER_CABLE, m_nLoadCableCreationRadio);
	DDX_Radio(pDX, IDC_RADIO_BUS_ALL_POWER_CABLE, m_nBusCableCreationRadio);
	DDX_Control(pDX, IDC_CHECK_LOAD_POWER_CABLE_CREATION, m_wndLoadCableCreationCheckButton);
	DDX_Control(pDX, IDC_CHECK_EQ_BUS_POWER_CABLE_CREATION, m_wndBusCableCreationCheckButton);
	DDX_Control(pDX, IDC_STATIC_LOAD_POWER_CABLE_CREATION, m_StaticLoadPowerCableCreation);
	DDX_Control(pDX, IDC_STATIC_EQ_BUS_POWER_CABLE_CREATION, m_StaticBusPowerCableCreation);
}


BEGIN_MESSAGE_MAP(CPowerCableCreationDlg, CDialog)
	ON_BN_CLICKED(ID_POWER_CABLE_CREATION, &CPowerCableCreationDlg::OnBnClickedPowerCableCreation)
	ON_BN_CLICKED(IDCANCEL, &CPowerCableCreationDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK_EQ_BUS_POWER_CABLE_CREATION, &CPowerCableCreationDlg::OnBnClickedCheckEqBusPowerCableCreation)
	ON_BN_CLICKED(IDC_CHECK_LOAD_POWER_CABLE_CREATION, &CPowerCableCreationDlg::OnBnClickedCheckLoadPowerCableCreation)
END_MESSAGE_MAP()


BOOL CPowerCableCreationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_wndLoadCableCreationCheckButton.SetGroupbox(IDC_STATIC_LOAD_POWER_CABLE_CREATION);
	m_wndLoadCableCreationCheckButton.SetCheck(TRUE);
	m_wndBusCableCreationCheckButton.SetGroupbox(IDC_STATIC_EQ_BUS_POWER_CABLE_CREATION);
	m_wndBusCableCreationCheckButton.SetCheck(FALSE);
	
	CELoadDocData& docData = CELoadDocData::GetInstance();

	//! Button Setting
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	m_wndCableCreationButton.SetTextHotColor(TextHotColor);
	m_wndCancelButton.SetTextHotColor(TextHotColor);

	HICON hOKIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_OK_18X18),
		IMAGE_ICON,
		18,18, 0 );
	m_wndCableCreationButton.SetIcon(hOKIcon);
	
	HICON hCancelIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_CANCEL_18X18),
		IMAGE_ICON,
		18,18, 0 );
	m_wndCancelButton.SetIcon(hCancelIcon);
	
	BeginWaitCursor();

	if(ERROR_SUCCESS != LoadCableDataTable())
	{
		return FALSE;
	}

	LoadData();
	DisplayData();

	EndWaitCursor();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	데이타를 불러온다.

	@author KHS	

	@return		
**/
int CPowerCableCreationDlg::LoadData(void)
{
	CELoadDocData &docData = CELoadDocData::GetInstance();
	const string rMDBFilePath = docData.GetProjectMDBFilePath();
	
	CHVCableCreationSettingTable& HVVoltageSettingTable = CHVCableCreationSettingTable::GetInstance();
	HVVoltageSettingTable.Load(rMDBFilePath);

	CLV3PHCableCreationSettingTable& LV3PHVoltageSettingTable = CLV3PHCableCreationSettingTable::GetInstance();
	LV3PHVoltageSettingTable.Load(rMDBFilePath);

	CLV1PHCableCreationSettingTable& LV1PHVoltageSettingTable = CLV1PHCableCreationSettingTable::GetInstance();
	LV1PHVoltageSettingTable.Load(rMDBFilePath);

	CDCCableCreationSettingTable& DCVoltageSettingTable = CDCCableCreationSettingTable::GetInstance();
	DCVoltageSettingTable.Load(rMDBFilePath);

	CPowerCableCreationSettingTable* pPowerCableCreationSettingTable = static_cast<CPowerCableCreationSettingTable*>(docData.GetTableOf(CPowerCableCreationSettingTable::GetTableName()));
	
	return ERROR_SUCCESS;
}

/**
	@brief	데이타를 저장한다.

	@author KHS	

	@return		
**/
int CPowerCableCreationDlg::SaveData(void)
{
	UpdateData(TRUE);

	CELoadDocData& docData = CELoadDocData::GetInstance();
	CString rBusCable, rBusRadio, rLoadCable, rLoadRadio;
	CPowerCableCreationSettingTable* pPowerCableCreationSettingTable = static_cast<CPowerCableCreationSettingTable*>(docData.GetTableOf(CPowerCableCreationSettingTable::GetTableName()));
	if(pPowerCableCreationSettingTable)
	{
		int i = 1;
		for(vector<CCableCreationTable::Record>::iterator itr = pPowerCableCreationSettingTable->m_RecordEntry.begin();itr != pPowerCableCreationSettingTable->m_RecordEntry.end();++itr,++i)
		{
			if(i == 1)
			{
				CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_LOAD_POWER_CABLE_CREATION);
				rLoadCable = (TRUE == pButton->GetCheck()) ? _T("Yes") : _T("No");

				pButton = (CButton*)GetDlgItem(IDC_CHECK_EQ_BUS_POWER_CABLE_CREATION);
				rBusCable = (TRUE == pButton->GetCheck()) ? _T("Yes") : _T("No");

				//! LOAD
				rLoadRadio.Format(_T("%d") , m_nLoadCableCreationRadio);

				//! BUS
				rBusRadio.Format(_T("%d") , m_nBusCableCreationRadio);
			}

			itr->FieldValueMap["C_LOAD_POWER_CABLE_CREATION"]	= rLoadCable.operator LPCSTR();
			itr->FieldValueMap["C_LOAD_POWER_CABLE_RADIO"]		= rLoadRadio.operator LPCSTR();
			itr->FieldValueMap["C_EQ_BUS_POWER_CABLE_CREATION"]	= rBusCable.operator LPCSTR();
			itr->FieldValueMap["C_BUS_POWER_CABLE_RADIO"]		= rBusRadio.operator LPCSTR();

			if(1 == i) break;
		}
		pPowerCableCreationSettingTable->Update();
	}

	CPowerCableCreationSettingDlg::UpdateLoadItems();

	return ERROR_SUCCESS;
}

/**
	@brief	불러온 데이타를 화면에 출력한다.

	@author KHS 

	@return		
**/
int CPowerCableCreationDlg::DisplayData(void)
{
	//! DISPLAY CABLE CREATION SETTING DATA 
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CPowerCableCreationSettingTable* pPowerCableSettingTable = static_cast<CPowerCableCreationSettingTable*>(docData.GetTableOf(CPowerCableCreationSettingTable::GetTableName()));
	if(pPowerCableSettingTable)
	{
		int nRow = 1;
		for(vector<CCableCreationTable::Record>::iterator itr = pPowerCableSettingTable->m_RecordEntry.begin();itr != pPowerCableSettingTable->m_RecordEntry.end();++itr,++nRow)
		{
			if(1 == nRow)
			{
				const CString rLoadPowerCableCreation = CString(itr->FieldValueMap["C_LOAD_POWER_CABLE_CREATION"].c_str()).MakeUpper();
				CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_LOAD_POWER_CABLE_CREATION);
				(_T("YES") == rLoadPowerCableCreation ? pButton->SetCheck(TRUE) : pButton->SetCheck(FALSE));

				const CString rEqBusPowerCableCreation = CString(itr->FieldValueMap["C_EQ_BUS_POWER_CABLE_CREATION"].c_str()).MakeUpper();
				pButton = (CButton*)GetDlgItem(IDC_CHECK_EQ_BUS_POWER_CABLE_CREATION);
				(_T("YES") == rEqBusPowerCableCreation ? pButton->SetCheck(TRUE) : pButton->SetCheck(FALSE));
				
				if(!itr->FieldValueMap["C_LOAD_POWER_CABLE_RADIO"].empty())
				{
					m_nLoadCableCreationRadio = atoi(itr->FieldValueMap["C_LOAD_POWER_CABLE_RADIO"].c_str());
				}

				if(!itr->FieldValueMap["C_BUS_POWER_CABLE_RADIO"].empty())
				{
					m_nBusCableCreationRadio = atoi(itr->FieldValueMap["C_BUS_POWER_CABLE_RADIO"].c_str());
				}
			}

			//!그룹 체크 박스의 VISIBLE 설정
			CButton* pLoadCreationCheckButton = (CButton*)GetDlgItem(IDC_CHECK_LOAD_POWER_CABLE_CREATION);
			if(FALSE == pLoadCreationCheckButton->GetCheck())
			{
				CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_LOAD_ALL_POWER_CABLE);
				pButton->EnableWindow(FALSE);
				pButton = (CButton*)GetDlgItem(IDC_RADIO_LOAD_UNSIZED_POWER_CABLE_ONLY);
				pButton->EnableWindow(FALSE);
			}
			CButton* pBusCreationCheckButton = (CButton*)GetDlgItem(IDC_CHECK_EQ_BUS_POWER_CABLE_CREATION);
			if(FALSE == pBusCreationCheckButton->GetCheck())
			{
				CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_BUS_ALL_POWER_CABLE);
				pButton->EnableWindow(FALSE);
				pButton = (CButton*)GetDlgItem(IDC_RADIO_BUS_UNSIZED_POWER_CABLE_ONLY);
				pButton->EnableWindow(FALSE);
				CWnd* pWnd = GetDlgItem(IDC_STATIC_AREA);
				if(pWnd) pWnd->EnableWindow(FALSE);
				pWnd = GetDlgItem(IDC_EDIT_AREA);
				if(pWnd) pWnd->EnableWindow(FALSE);
			}
			else
			{
				CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_BUS_ALL_POWER_CABLE);
				pButton->EnableWindow(TRUE);
				pButton = (CButton*)GetDlgItem(IDC_RADIO_BUS_UNSIZED_POWER_CABLE_ONLY);
				pButton->EnableWindow(TRUE);
				CWnd* pWnd = GetDlgItem(IDC_STATIC_AREA);
				if(pWnd) pWnd->EnableWindow(TRUE);
				pWnd = GetDlgItem(IDC_EDIT_AREA);
				if(pWnd) pWnd->EnableWindow(TRUE);
			}
		}
	}
	
	UpdateData(FALSE);

	return ERROR_SUCCESS;
}

/**
	@brief  Project Setting Table에서 CableDataTable 이름을 읽어와 CableDataTable을 로딩한다.

	@author	KHS
*/
int CPowerCableCreationDlg::LoadCableDataTable(void)
{
	CCableDataListTable &CableDataListTable = CCableDataListTable::GetInstance();
	CableDataListTable.Load();

	CELoadDocData& docData = CELoadDocData::GetInstance();

	CString rProjectMDB = docData.GetProjectMDBFilePath().c_str();
	rProjectMDB.MakeUpper();
	if(FileExist(rProjectMDB))
	{
		CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
		///docData.m_ProjectSettingTable.Load(_T("") , CProjectSettingTable::GetTableName());
		string* pCableDataTableName = pSettingTable->GetFieldValue(_T("C_CABLEDATA_TABLE"));
		if(pCableDataTableName && !pCableDataTableName->empty())
		{
			docData.m_ProjectCableDataTable.Load(*pCableDataTableName , (*pCableDataTableName) != docData.m_ProjectCableDataTable.GetTableName());
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	POWER CABLE CREATION

	@author KHS

	@return		
**/
void CPowerCableCreationDlg::OnBnClickedPowerCableCreation()
{
	SaveData();	//! 우선 세팅을 저장한다.
	CELoadDocData& docData = CELoadDocData::GetInstance();
	if(true == docData.IsModified())
	{
		if(IDYES == AfxMessageBox(_T("데이터가 변경되었습니다. 저장 후 계산할까요?") , MB_YESNO|MB_ICONQUESTION))
		{
			CELOADApp* pApp = (CELOADApp*)AfxGetApp();
			if(pApp)
			{
				docData.Save();

				//! 저장 후 prop TEXT 상태 초기화
				vector<string> emptyEntry;
				pApp->UpdateAllViews(emptyEntry);
			}

		}
		else
		{
			COMMAND::CCommandUndo cmdUndo;
			cmdUndo.Execute();
		}
	}
	
	//! All Power Cable일때는 기존의 Cable이 삭제되므로 사용자들이 실행 여부를 확인하도록 한다.
	CButton* pLoadButton = (CButton*)GetDlgItem(IDC_CHECK_LOAD_POWER_CABLE_CREATION);
	CButton* pBusButton  = (CButton*)GetDlgItem(IDC_CHECK_EQ_BUS_POWER_CABLE_CREATION);
	if(((TRUE == pLoadButton->GetCheck()) && (0 == m_nLoadCableCreationRadio)) || 
		((TRUE == pBusButton->GetCheck()) && (0 == m_nBusCableCreationRadio)))
	{
		if(IDOK != AfxMessageBox(_T("All cables will be deleted. Do you want to execute?") , MB_OKCANCEL| MB_ICONWARNING))
		{
			return;
		}
	}
	
	{
		BeginWaitCursor();
		ELOAD_LOG4CXX_RESET(mylogger, _T("Auto Power Cable Error Messages"));
		CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_LOAD_POWER_CABLE_CREATION);
		CString rAreaName;
		GetDlgItemText(IDC_EDIT_AREA , rAreaName);
		COMMAND::CCommandPowerCableCreation cmdPowerCableCreation(
			(pButton && (TRUE == pButton->GetCheck())) ? 
			COMMAND::CCommandPowerCableCreation::LOAD : COMMAND::CCommandPowerCableCreation::EQ,
			rAreaName.operator LPCSTR()
			);
		if(ERROR_SUCCESS == cmdPowerCableCreation.Execute())
		{
			docData.SetModifiedFlag(true);
		}
		EndWaitCursor();

		///
		CString rMsg;
		const string rResultString = cmdPowerCableCreation.GetResultString();
		if(docData.m_nErrorCount > 0)
			rMsg.Format(_T("%s%d error(s)") , rResultString.c_str() , docData.m_nErrorCount);
		else	rMsg.Format(_T("%s") , rResultString.c_str());
		CDataResultDlg dlg(rMsg);
		dlg.DoModal();

		
		//!CABLE SIZING RESULT VIEW를 생성
		CELOADApp* pApp = (CELOADApp*)AfxGetApp();
		if(pApp)
		{
			const string rInterestingFilePath = docData.GetProjectFolderPath() + _T("Cable Sizing Result\\");
			CFileTools::DeleteFiles(CString(rInterestingFilePath.c_str()) , _T("*.*"));

			pApp->OpenNewDocumentIfNeed(_T("Auto Power Cable") , _T("Create Auto Power Cable"));

			AppenderPtr appender = mylogger->getAppender(_T("ELoadAppender"));
			FileAppenderPtr fileappender = (FileAppenderPtr)appender;
			const string rLogFilePath = fileappender->getFile();
			pApp->PutLogFileToDatabase(rLogFilePath, _T("Auto Power Cable"));
			
			//! LOG 출력
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			if(pFrame) pFrame->m_wndOutput.Navigate2(rLogFilePath.c_str());
		}

		OnOK();
		
	}///else	AfxMessageBox(_T("LOAD POWER CABLE 종류를 선택해 주세요."));
}

/**
	@brief	

	@author BHK	

	@date 2009-07-01 오후 11:37:18	

	@param	

	@return		
**/
void CPowerCableCreationDlg::OnBnClickedCancel()
{
	OnCancel();
}

/**
	@brief	

	@author KHS	

	@date 2009-07-24 오후 3:37:17	

	@param	

	@return		
**/
void CPowerCableCreationDlg::OnBnClickedCheckEqBusPowerCableCreation()
{
	m_wndLoadCableCreationCheckButton.SetCheck(FALSE);
}

/**
	@brief	

	@author KHS	

	@date 2009-07-24 오후 3:37:19	

	@param	

	@return		
**/
void CPowerCableCreationDlg::OnBnClickedCheckLoadPowerCableCreation()
{
	m_wndBusCableCreationCheckButton.SetCheck(FALSE);
}
