// VoltageSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "VoltageSettingDlg.h"
#include <util/ado/ADODB.h>
#include "ELoadDocData.h"
#include "GridCellEx.h"

#include "Tables/CableCreationSettingTable.h"

using namespace TABLES;

// CVoltageSettingDlg dialog
#define	READ_ONLY_COLOR	RGB(220, 220, 220)
#define	GRID_TEXT_COLOR	RGB(0, 0, 0)

IMPLEMENT_DYNAMIC(CVoltageSettingDlg, CDialog)

CVoltageSettingDlg::CVoltageSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVoltageSettingDlg::IDD, pParent)
{

}

CVoltageSettingDlg::~CVoltageSettingDlg()
{
}

void CVoltageSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	//DDX_GridControl(pDX , IDC_HV_GRID , m_HVVoltageSettingGrid);
	CDialog::DoDataExchange(pDX);

	//! BUS
	DDX_Control(pDX, IDC_SAVE_BUTTON, m_wndOKButton);
	DDX_Control(pDX, IDCANCEL, m_wndCancelButton);
	DDX_Control(pDX, IDC_HV1_BUS_VOLTAGE_EDIT, m_HV1_BusVoltage);
	DDX_Control(pDX, IDC_HV2_BUS_VOLTAGE_EDIT, m_HV2_BusVoltage);
	DDX_Control(pDX, IDC_HV3_BUS_VOLTAGE_EDIT, m_HV3_BusVoltage);
	DDX_Control(pDX, IDC_LV1_3PH_BUS_VOLTAGE_EDIT, m_LV1_3PH_BusVoltage);
	DDX_Control(pDX, IDC_LV2_3PH_BUS_VOLTAGE_EDIT, m_LV2_3PH_BusVoltage);
	DDX_Control(pDX, IDC_LV11_1PH_BUS_VOLTAGE_EDIT, m_LV11_1PH_BusVoltage);
	DDX_Control(pDX, IDC_LV12_1PH_BUS_VOLTAGE_EDIT, m_LV12_1PH_BusVoltage);
	DDX_Control(pDX, IDC_DC_BUS_VOLTAGE_EDIT, m_DC_BusVoltage);

	//! LOAD
	DDX_Control(pDX, IDC_HV1_LOAD_VOLTAGE_EDIT, m_HV1_LoadVoltage);
	DDX_Control(pDX, IDC_HV2_LOAD_VOLTAGE_EDIT, m_HV2_LoadVoltage);
	DDX_Control(pDX, IDC_HV3_LOAD_VOLTAGE_EDIT, m_HV3_LoadVoltage);
	DDX_Control(pDX, IDC_LV1_3PH_LOAD_VOLTAGE_EDIT, m_LV1_3PH_LoadVoltage);
	DDX_Control(pDX, IDC_LV2_3PH_LOAD_VOLTAGE_EDIT, m_LV2_3PH_LoadVoltage);
	DDX_Control(pDX, IDC_LV11_1PH_LOAD_VOLTAGE_EDIT, m_LV11_1PH_LoadVoltage);
	DDX_Control(pDX, IDC_LV12_1PH_LOAD_VOLTAGE_EDIT, m_LV12_1PH_LoadVoltage);
	DDX_Control(pDX, IDC_DC_LOAD_VOLTAGE_EDIT, m_DC_LoadVoltage);

	//! VOLTAGE TYPE
	DDX_Control(pDX, IDC_HV1_STATIC, m_HV1_STATIC);
	DDX_Control(pDX, IDC_HV2_STATIC, m_HV2_STATIC);
	DDX_Control(pDX, IDC_HV3_STATIC, m_HV3_STATIC);
	DDX_Control(pDX, IDC_LV1_3PH_STATIC, m_LV1_3PH_STATIC);
	DDX_Control(pDX, IDC_LV2_3PH_STATIC, m_LV2_3PH_STATIC);
	DDX_Control(pDX, IDC_LV11_1PH_STATIC, m_LV11_1PH_STATIC);
	DDX_Control(pDX, IDC_LV12_1PH_STATIC, m_LV12_1PH_STATIC);
	DDX_Control(pDX, IDC_DC_STATIC, m_DC_STATIC);

	//! PHASE
	DDX_Control(pDX, IDC_HV1_PHASE_STATIC, m_HV1_PHASE_STATIC);
	DDX_Control(pDX, IDC_HV2_PHASE_STATIC, m_HV2_PHASE_STATIC);
	DDX_Control(pDX, IDC_HV3_PHASE_STATIC, m_HV3_PHASE_STATIC);
	DDX_Control(pDX, IDC_LV1_3PH_PHASE_STATIC, m_LV1_3PH_PHASE_STATIC);
	DDX_Control(pDX, IDC_LV2_3PH_PHASE_STATIC, m_LV2_3PH_PHASE_STATIC);
	DDX_Control(pDX, IDC_LV11_1PH_PHASE_STATIC, m_LV11_1PH_PHASE_STATIC);
	DDX_Control(pDX, IDC_LV12_1PH_PHASE_STATIC, m_LV12_1PH_PHASE_STATIC);
	DDX_Control(pDX, IDC_DC_PHASE_STATIC, m_DC_PHASE_STATIC);
}

BEGIN_MESSAGE_MAP(CVoltageSettingDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CVoltageSettingDlg::OnBnClickedOK)
	ON_BN_CLICKED(IDC_SAVE_BUTTON, &CVoltageSettingDlg::OnBnClickedSave)
	ON_WM_KEYDOWN()
	ON_EN_CHANGE(IDC_HV1_BUS_VOLTAGE_EDIT, &CVoltageSettingDlg::OnEnChangeHv1BusVoltageEdit)
	ON_EN_CHANGE(IDC_HV2_BUS_VOLTAGE_EDIT, &CVoltageSettingDlg::OnEnChangeHv2BusVoltageEdit)
	ON_EN_CHANGE(IDC_HV3_BUS_VOLTAGE_EDIT, &CVoltageSettingDlg::OnEnChangeHv3BusVoltageEdit)
	ON_EN_CHANGE(IDC_LV1_3PH_BUS_VOLTAGE_EDIT, &CVoltageSettingDlg::OnEnChangeLv13phBusVoltageEdit)
	ON_EN_CHANGE(IDC_LV2_3PH_BUS_VOLTAGE_EDIT, &CVoltageSettingDlg::OnEnChangeLv23phBusVoltageEdit)
	ON_EN_CHANGE(IDC_LV11_1PH_BUS_VOLTAGE_EDIT, &CVoltageSettingDlg::OnEnChangeLv111phBusVoltageEdit)
	ON_EN_CHANGE(IDC_LV12_1PH_BUS_VOLTAGE_EDIT, &CVoltageSettingDlg::OnEnChangeLv121phBusVoltageEdit)
	ON_EN_CHANGE(IDC_DC_BUS_VOLTAGE_EDIT, &CVoltageSettingDlg::OnEnChangeDcBusVoltageEdit)
END_MESSAGE_MAP()


// CVoltageSettingDlg message handlers

BOOL CVoltageSettingDlg::OnInitDialog()
{
        CDialog::OnInitDialog();

        // TODO:  Add extra initialization here

	//! SET BUTTON ICON
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	
	///HICON hOKIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), 
	///	MAKEINTRESOURCE(IDI_OK_18X18),
	///	IMAGE_ICON,
	///	18,18, 0 );
	m_wndOKButton.SetIcon(IDI_OK_18X18);
	m_wndOKButton.SetTextHotColor(TextHotColor);

	///HICON hCancelIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), 
	///	MAKEINTRESOURCE(IDI_CANCEL_18X18),
	///	IMAGE_ICON,
	///	18,18, 0 );
	m_wndCancelButton.SetIcon(IDI_CANCEL_18X18);
	m_wndCancelButton.SetTextHotColor(TextHotColor);

	InitEditBox();

	LoadData();
	DisplayData();
        
        return TRUE;  // return TRUE unless you set the focus to a control
        // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CVoltageSettingDlg::PreTranslateMessage(MSG* pMsg)
{
        // TODO: Add your specialized code here and/or call the base class
        if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE) return TRUE;
		if(pMsg->wParam == VK_RETURN) return TRUE;
	}
        return CDialog::PreTranslateMessage(pMsg);
}


/**
	@brief	

	@author	BHK
*/
void CVoltageSettingDlg::OnBnClickedOK()
{
	// TODO: Add your control notification handler code here
	///OnOK();
}

/**
	@brief	데이터를 저장하고 다이얼로그를 종료한다.

	@author	BHK
*/
void CVoltageSettingDlg::OnBnClickedSave()
{
	// TODO: Add your control notification handler code here
	SaveData();
	OnOK();
}

/**
	@brief  데이타를 DB 에 저장한다.

	@author KHS
*/
int CVoltageSettingDlg::SaveData(void)
{
	//static const string __COLUMNS__[] = {_T("C_VOLTAGE_TYPE") , _T("C_BUS_VOLTAGE") , _T("C_PHASE") , _T("C_LOAD_VOLTAGE")};
	CString rBusVoltage, rLoadVoltage, rVoltageType, rPhase;

	CHVCableCreationSettingTable& HVVoltageSettingTable = CHVCableCreationSettingTable::GetInstance();
	int i = 1;
	for(vector<CCableCreationTable::Record>::iterator itr = HVVoltageSettingTable.m_RecordEntry.begin();itr != HVVoltageSettingTable.m_RecordEntry.end();++itr,++i)
	{
		if(1 == i)
		{
			m_HV1_LoadVoltage.GetWindowText( rLoadVoltage);
			m_HV1_BusVoltage.GetWindowText( rBusVoltage);
			m_HV1_STATIC.GetWindowText( rVoltageType);
			m_HV1_PHASE_STATIC.GetWindowText(rPhase);
		}
		else if(2 == i)
		{
			m_HV2_LoadVoltage.GetWindowText( rLoadVoltage);
			m_HV2_BusVoltage.GetWindowText( rBusVoltage);
			m_HV2_STATIC.GetWindowText( rVoltageType);
			m_HV2_PHASE_STATIC.GetWindowText(rPhase);
		}
		else if(3 == i)
		{
			m_HV3_LoadVoltage.GetWindowText( rLoadVoltage);
			m_HV3_BusVoltage.GetWindowText( rBusVoltage);
			m_HV3_STATIC.GetWindowText( rVoltageType);
			m_HV3_PHASE_STATIC.GetWindowText(rPhase);
		}
		itr->FieldValueMap["C_BUS_VOLTAGE"]	= rBusVoltage.operator LPCSTR();
		itr->FieldValueMap["C_LOAD_VOLTAGE"]	= rLoadVoltage.operator LPCSTR();
		itr->FieldValueMap["C_VOLTAGE_TYPE"]	= rVoltageType.operator LPCSTR();
		itr->FieldValueMap["C_PHASE"]		= rPhase.operator LPCSTR();
		
		if(3 == i) break;
	}
	HVVoltageSettingTable.Update();

	CLV3PHCableCreationSettingTable& LV3PHVoltageSettingTable = CLV3PHCableCreationSettingTable::GetInstance();
	i = 1;
	for(vector<CCableCreationTable::Record>::iterator itr = LV3PHVoltageSettingTable.m_RecordEntry.begin();itr != LV3PHVoltageSettingTable.m_RecordEntry.end();++itr,++i)
	{
		if(1 == i)
		{
		m_LV1_3PH_LoadVoltage.GetWindowText( rLoadVoltage);
		m_LV1_3PH_BusVoltage.GetWindowText( rBusVoltage);
		m_LV1_3PH_STATIC.GetWindowText( rVoltageType);
		m_LV1_3PH_PHASE_STATIC.GetWindowText(rPhase);

		}
		else if(2 == i)
		{
			m_LV2_3PH_LoadVoltage.GetWindowText( rLoadVoltage);
			m_LV2_3PH_BusVoltage.GetWindowText( rBusVoltage);
			m_LV2_3PH_STATIC.GetWindowText( rVoltageType);
			m_LV2_3PH_PHASE_STATIC.GetWindowText(rPhase);
		}

		itr->FieldValueMap["C_BUS_VOLTAGE"]	= rBusVoltage.operator LPCSTR();
		itr->FieldValueMap["C_LOAD_VOLTAGE"]	= rLoadVoltage.operator LPCSTR();
		itr->FieldValueMap["C_VOLTAGE_TYPE"]	= rVoltageType.operator LPCSTR();
		itr->FieldValueMap["C_PHASE"]		= rPhase.operator LPCSTR();

		if(2 == i) break;

	}
	LV3PHVoltageSettingTable.Update();

	CLV1PHCableCreationSettingTable& LV1PHVoltageSettingTable = CLV1PHCableCreationSettingTable::GetInstance();
	i = 1;
	for(vector<CCableCreationTable::Record>::iterator itr = LV1PHVoltageSettingTable.m_RecordEntry.begin();itr != LV1PHVoltageSettingTable.m_RecordEntry.end();++itr,++i)
	{
		if(1 == i)
		{
			m_LV11_1PH_LoadVoltage.GetWindowText( rLoadVoltage);
			m_LV11_1PH_BusVoltage.GetWindowText( rBusVoltage);
			m_LV11_1PH_STATIC.GetWindowText( rVoltageType);
			m_LV11_1PH_PHASE_STATIC.GetWindowText(rPhase);
		}
		else if(2 == i)
		{
			m_LV12_1PH_LoadVoltage.GetWindowText( rLoadVoltage);
			m_LV12_1PH_BusVoltage.GetWindowText( rBusVoltage);
			m_LV12_1PH_STATIC.GetWindowText( rVoltageType);
			m_LV12_1PH_PHASE_STATIC.GetWindowText(rPhase);
		}

		itr->FieldValueMap["C_BUS_VOLTAGE"]	= rBusVoltage.operator LPCSTR();
		itr->FieldValueMap["C_LOAD_VOLTAGE"]	= rLoadVoltage.operator LPCSTR();
		itr->FieldValueMap["C_VOLTAGE_TYPE"]	= rVoltageType.operator LPCSTR();
		itr->FieldValueMap["C_PHASE"]		= rPhase.operator LPCSTR();

		if(2 == i) break;
	}
	LV1PHVoltageSettingTable.Update();

	CDCCableCreationSettingTable& DCVoltageSettingTable = CDCCableCreationSettingTable::GetInstance();
	i = 1;
	for(vector<CCableCreationTable::Record>::iterator itr = DCVoltageSettingTable.m_RecordEntry.begin();itr != DCVoltageSettingTable.m_RecordEntry.end();++itr,++i)
	{
		if(1 == i)
		{
			m_DC_LoadVoltage.GetWindowText( rLoadVoltage);
			m_DC_BusVoltage.GetWindowText( rBusVoltage);
			m_DC_STATIC.GetWindowText( rVoltageType);
			m_DC_PHASE_STATIC.GetWindowText(rPhase);
		}

		itr->FieldValueMap["C_BUS_VOLTAGE"] = rBusVoltage.operator LPCSTR();
		itr->FieldValueMap["C_LOAD_VOLTAGE"] = rLoadVoltage.operator LPCSTR();
		itr->FieldValueMap["C_VOLTAGE_TYPE"] = rVoltageType.operator LPCSTR();
		itr->FieldValueMap["C_PHASE"] = rPhase.operator LPCSTR();

		if(1 == i) break;
	}
	DCVoltageSettingTable.Update();

	
	return ERROR_SUCCESS;
}
/**
	@brief  DB 데이타를 읽어온다.
	
	@author KHS
*/
int CVoltageSettingDlg::LoadData(void)
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
	
	return ERROR_SUCCESS;
}

/**
	@brief  테이블의 내용을 EDIT BOX에 뿌린다.
	
	@author KHS
*/
int CVoltageSettingDlg::DisplayData(void)
{
	int nRow = 1;
	CHVCableCreationSettingTable& HVVoltageSettingTable = CHVCableCreationSettingTable::GetInstance();
	for(vector<CCableCreationTable::Record>::iterator itr = HVVoltageSettingTable.m_RecordEntry.begin(); itr !=  HVVoltageSettingTable.m_RecordEntry.end();++itr,++nRow)
	{
		if(1 == nRow)
		{
			m_HV1_BusVoltage.SetWindowText(itr->FieldValueMap["C_BUS_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_BUS_VOLTAGE"].c_str());
			m_HV1_LoadVoltage.SetWindowText(itr->FieldValueMap["C_LOAD_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_LOAD_VOLTAGE"].c_str());
		}
		else if(2 == nRow)
		{
			m_HV2_BusVoltage.SetWindowText(itr->FieldValueMap["C_BUS_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_BUS_VOLTAGE"].c_str());
			m_HV2_LoadVoltage.SetWindowTextA(itr->FieldValueMap["C_LOAD_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_LOAD_VOLTAGE"].c_str());
		}
		else if(3 == nRow)
		{
			m_HV3_BusVoltage.SetWindowText(itr->FieldValueMap["C_BUS_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_BUS_VOLTAGE"].c_str());
			m_HV3_LoadVoltage.SetWindowText(itr->FieldValueMap["C_LOAD_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_LOAD_VOLTAGE"].c_str());
		}

	}
	nRow = 1;
	CLV3PHCableCreationSettingTable& LV3PHVoltageSettingTable = CLV3PHCableCreationSettingTable::GetInstance();
	for(vector<CCableCreationTable::Record>::iterator itr = LV3PHVoltageSettingTable.m_RecordEntry.begin(); itr !=  LV3PHVoltageSettingTable.m_RecordEntry.end();++itr,++nRow)
	{
		if(1 == nRow)
		{
			m_LV1_3PH_BusVoltage.SetWindowText(itr->FieldValueMap["C_BUS_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_BUS_VOLTAGE"].c_str());
			m_LV1_3PH_LoadVoltage.SetWindowText(itr->FieldValueMap["C_LOAD_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_LOAD_VOLTAGE"].c_str());
		}
		else if(2 == nRow)	
		{
			m_LV2_3PH_BusVoltage.SetWindowText(itr->FieldValueMap["C_BUS_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_BUS_VOLTAGE"].c_str());
			m_LV2_3PH_LoadVoltage.SetWindowText(itr->FieldValueMap["C_LOAD_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_LOAD_VOLTAGE"].c_str());
		}
	}

	nRow = 1;
	CLV1PHCableCreationSettingTable& LV1PHVoltageSettingTable = CLV1PHCableCreationSettingTable::GetInstance();
	for(vector<CCableCreationTable::Record>::iterator itr = LV1PHVoltageSettingTable.m_RecordEntry.begin(); itr != LV1PHVoltageSettingTable.m_RecordEntry.end();++itr,++nRow)
	{
		if(1 == nRow)
		{
			m_LV11_1PH_BusVoltage.SetWindowText(itr->FieldValueMap["C_BUS_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_BUS_VOLTAGE"].c_str());
			m_LV11_1PH_LoadVoltage.SetWindowText(itr->FieldValueMap["C_LOAD_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_LOAD_VOLTAGE"].c_str());
		}
		else if(2 == nRow)
		{
			m_LV12_1PH_BusVoltage.SetWindowText(itr->FieldValueMap["C_BUS_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_BUS_VOLTAGE"].c_str());
			m_LV12_1PH_LoadVoltage.SetWindowText(itr->FieldValueMap["C_LOAD_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_LOAD_VOLTAGE"].c_str());
		}
	}

	nRow = 1;
	CDCCableCreationSettingTable& DCVoltageSettingTable = CDCCableCreationSettingTable::GetInstance();
	{	
		for(vector<CCableCreationTable::Record>::iterator itr = DCVoltageSettingTable.m_RecordEntry.begin(); itr != DCVoltageSettingTable.m_RecordEntry.end();++itr, ++nRow)
		{
			if(1 == nRow)
			{
				m_DC_BusVoltage.SetWindowText(itr->FieldValueMap["C_BUS_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_BUS_VOLTAGE"].c_str());
				m_DC_LoadVoltage.SetWindowText(itr->FieldValueMap["C_LOAD_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_LOAD_VOLTAGE"].c_str());
			}
		}			
	}				


	return ERROR_SUCCESS;
}

/**
	@brief  EDIT BOX MODE SETTING
	@author KHS
*/
int CVoltageSettingDlg::InitEditBox(void)
{
	CString rRegEx="([0-9.])*";
	m_HV1_BusVoltage.SetRegEx(rRegEx);
	m_HV1_BusVoltage.CreateToolTip(this , "사용할 수 없는 글자입니다.");
	m_HV1_BusVoltage.SetBackgroundColourFocus (RGB (150, 200, 255));
	m_HV1_BusVoltage.SetForegroundColourError (RGB (255, 255, 255));
	m_HV1_BusVoltage.SetBackgroundColourError (RGB (255, 0, 0));

	m_HV2_BusVoltage.SetRegEx(rRegEx);
	m_HV2_BusVoltage.CreateToolTip(this , "사용할 수 없는 글자입니다.");
	m_HV2_BusVoltage.SetBackgroundColourFocus (RGB (150, 200, 255));
	m_HV2_BusVoltage.SetForegroundColourError (RGB (255, 255, 255));
	m_HV2_BusVoltage.SetBackgroundColourError (RGB (255, 0, 0));

	m_HV3_BusVoltage.SetRegEx(rRegEx);
	m_HV3_BusVoltage.CreateToolTip(this , "사용할 수 없는 글자입니다.");
	m_HV3_BusVoltage.SetBackgroundColourFocus (RGB (150, 200, 255));
	m_HV3_BusVoltage.SetForegroundColourError (RGB (255, 255, 255));
	m_HV3_BusVoltage.SetBackgroundColourError (RGB (255, 0, 0));

	m_LV1_3PH_BusVoltage.SetRegEx(rRegEx);
	m_LV1_3PH_BusVoltage.CreateToolTip(this , "사용할 수 없는 글자입니다.");
	m_LV1_3PH_BusVoltage.SetBackgroundColourFocus (RGB (150, 200, 255));
	m_LV1_3PH_BusVoltage.SetForegroundColourError (RGB (255, 255, 255));
	m_LV1_3PH_BusVoltage.SetBackgroundColourError (RGB (255, 0, 0));

	m_LV2_3PH_BusVoltage.SetRegEx(rRegEx);
	m_LV2_3PH_BusVoltage.CreateToolTip(this , "사용할 수 없는 글자입니다.");
	m_LV2_3PH_BusVoltage.SetBackgroundColourFocus (RGB (150, 200, 255));
	m_LV2_3PH_BusVoltage.SetForegroundColourError (RGB (255, 255, 255));
	m_LV2_3PH_BusVoltage.SetBackgroundColourError (RGB (255, 0, 0));

	m_LV11_1PH_BusVoltage.SetRegEx(rRegEx);
	m_LV11_1PH_BusVoltage.CreateToolTip(this , "사용할 수 없는 글자입니다.");
	m_LV11_1PH_BusVoltage.SetBackgroundColourFocus (RGB (150, 200, 255));
	m_LV11_1PH_BusVoltage.SetForegroundColourError (RGB (255, 255, 255));
	m_LV11_1PH_BusVoltage.SetBackgroundColourError (RGB (255, 0, 0));

	m_LV12_1PH_BusVoltage.SetRegEx(rRegEx);
	m_LV12_1PH_BusVoltage.CreateToolTip(this , "사용할 수 없는 글자입니다.");
	m_LV12_1PH_BusVoltage.SetBackgroundColourFocus (RGB (150, 200, 255));
	m_LV12_1PH_BusVoltage.SetForegroundColourError (RGB (255, 255, 255));
	m_LV12_1PH_BusVoltage.SetBackgroundColourError (RGB (255, 0, 0));
	
	m_DC_BusVoltage.SetRegEx(rRegEx);
	m_DC_BusVoltage.CreateToolTip(this , "사용할 수 없는 글자입니다.");
	m_DC_BusVoltage.SetBackgroundColourFocus (RGB (150, 200, 255));
	m_DC_BusVoltage.SetForegroundColourError (RGB (255, 255, 255));
	m_DC_BusVoltage.SetBackgroundColourError (RGB (255, 0, 0));
	
	m_HV1_LoadVoltage.SetRegEx(rRegEx);
	m_HV1_LoadVoltage.CreateToolTip(this , "사용할 수 없는 글자입니다.");
	m_HV1_LoadVoltage.SetBackgroundColourFocus (RGB (150, 200, 255));
	m_HV1_LoadVoltage.SetForegroundColourError (RGB (255, 255, 255));
	m_HV1_LoadVoltage.SetBackgroundColourError (RGB (255, 0, 0));

	m_HV2_LoadVoltage.SetRegEx(rRegEx);
	m_HV2_LoadVoltage.CreateToolTip(this , "사용할 수 없는 글자입니다.");
	m_HV2_LoadVoltage.SetBackgroundColourFocus (RGB (150, 200, 255));
	m_HV2_LoadVoltage.SetForegroundColourError (RGB (255, 255, 255));
	m_HV2_LoadVoltage.SetBackgroundColourError (RGB (255, 0, 0));

	m_HV3_LoadVoltage.SetRegEx(rRegEx);
	m_HV3_LoadVoltage.CreateToolTip(this , "사용할 수 없는 글자입니다.");
	m_HV3_LoadVoltage.SetBackgroundColourFocus (RGB (150, 200, 255));
	m_HV3_LoadVoltage.SetForegroundColourError (RGB (255, 255, 255));
	m_HV3_LoadVoltage.SetBackgroundColourError (RGB (255, 0, 0));

	m_LV1_3PH_LoadVoltage.SetRegEx(rRegEx);
	m_LV1_3PH_LoadVoltage.CreateToolTip(this , "사용할 수 없는 글자입니다.");
	m_LV1_3PH_LoadVoltage.SetBackgroundColourFocus (RGB (150, 200, 255));
	m_LV1_3PH_LoadVoltage.SetForegroundColourError (RGB (255, 255, 255));
	m_LV1_3PH_LoadVoltage.SetBackgroundColourError (RGB (255, 0, 0));
	
	m_LV2_3PH_LoadVoltage.SetRegEx(rRegEx);
	m_LV2_3PH_LoadVoltage.CreateToolTip(this , "사용할 수 없는 글자입니다.");
	m_LV2_3PH_LoadVoltage.SetBackgroundColourFocus (RGB (150, 200, 255));
	m_LV2_3PH_LoadVoltage.SetForegroundColourError (RGB (255, 255, 255));
	m_LV2_3PH_LoadVoltage.SetBackgroundColourError (RGB (255, 0, 0));

	m_LV11_1PH_LoadVoltage.SetRegEx(rRegEx);
	m_LV11_1PH_LoadVoltage.CreateToolTip(this , "사용할 수 없는 글자입니다.");
	m_LV11_1PH_LoadVoltage.SetBackgroundColourFocus (RGB (150, 200, 255));
	m_LV11_1PH_LoadVoltage.SetForegroundColourError (RGB (255, 255, 255));
	m_LV11_1PH_LoadVoltage.SetBackgroundColourError (RGB (255, 0, 0));

	m_LV12_1PH_LoadVoltage.SetRegEx(rRegEx);
	m_LV12_1PH_LoadVoltage.CreateToolTip(this , "사용할 수 없는 글자입니다.");
	m_LV12_1PH_LoadVoltage.SetBackgroundColourFocus (RGB (150, 200, 255));
	m_LV12_1PH_LoadVoltage.SetForegroundColourError (RGB (255, 255, 255));
	m_LV12_1PH_LoadVoltage.SetBackgroundColourError (RGB (255, 0, 0));

	m_DC_LoadVoltage.SetRegEx(rRegEx);
	m_DC_LoadVoltage.CreateToolTip(this , "사용할 수 없는 글자입니다.");
	m_DC_LoadVoltage.SetBackgroundColourFocus (RGB (150, 200, 255));
	m_DC_LoadVoltage.SetForegroundColourError (RGB (255, 255, 255));
	m_DC_LoadVoltage.SetBackgroundColourError (RGB (255, 0, 0));

	return ERROR_SUCCESS;
}

/**
	@brief	BUS VOLTAGE EDIT들의 BUS VOLTAGE를 중복 체크한다.

	@author KHS	

	@return 중복된 BUS VOLTAGE가 없으면 TRUE 리턴
**/
BOOL CVoltageSettingDlg::CheckBusVoltage(const CString& rCompareBusVoltage, const int& nIndex)
{
	CString rBusVoltage;

	if(1 == nIndex || 2 == nIndex || 3 == nIndex)
	{
		m_HV1_BusVoltage.GetWindowText(rBusVoltage);
		if(!rBusVoltage.IsEmpty() && 1 != nIndex && rBusVoltage == rCompareBusVoltage) return FALSE;

		m_HV2_BusVoltage.GetWindowText(rBusVoltage);
		if(!rBusVoltage.IsEmpty() && 2 != nIndex && rBusVoltage == rCompareBusVoltage) return FALSE;

		m_HV3_BusVoltage.GetWindowText(rBusVoltage);
		if(!rBusVoltage.IsEmpty() && 3 != nIndex && rBusVoltage == rCompareBusVoltage) return FALSE;

	}
	else if(4 == nIndex || 5 == nIndex)
	{
		m_LV1_3PH_BusVoltage.GetWindowText(rBusVoltage);
		if(!rBusVoltage.IsEmpty() && 4 != nIndex && rBusVoltage == rCompareBusVoltage) return FALSE;

		m_LV2_3PH_BusVoltage.GetWindowText(rBusVoltage);
		if(!rBusVoltage.IsEmpty() && 5 != nIndex && rBusVoltage == rCompareBusVoltage) return FALSE;
	}
	else if(6 == nIndex || 7 == nIndex)
	{
		m_LV11_1PH_BusVoltage.GetWindowText(rBusVoltage);
		if(!rBusVoltage.IsEmpty() && 6 != nIndex && rBusVoltage == rCompareBusVoltage) return FALSE;

		m_LV12_1PH_BusVoltage.GetWindowText(rBusVoltage);
		if(!rBusVoltage.IsEmpty() && 7 != nIndex && rBusVoltage == rCompareBusVoltage) return FALSE;
	}

	return TRUE;
}

/**
	@brief	HV1 BUS VOLTAGE EDIT

	@author KHS		
**/
void CVoltageSettingDlg::OnEnChangeHv1BusVoltageEdit()
{
	CString rBusVoltage;
	m_HV1_BusVoltage.GetWindowText(rBusVoltage);
	if(!CheckBusVoltage(rBusVoltage, 1) && !rBusVoltage.IsEmpty())
	{
		::MessageBox(NULL, "The bus voltage exists already !","BUS VOLTAGE CHECK" , MB_OK|MB_ICONWARNING);
		m_HV1_BusVoltage.SetWindowText("");
	}
}

/**
	@brief	HV2 BUS VOLTAGE EDIT

	@author KHS		
**/
void CVoltageSettingDlg::OnEnChangeHv2BusVoltageEdit()
{
	// TODO:  Add your control notification handler code here
	CString rBusVoltage;
	m_HV2_BusVoltage.GetWindowText(rBusVoltage);
	if(!CheckBusVoltage(rBusVoltage, 2) && !rBusVoltage.IsEmpty())
	{
		::MessageBox(NULL, "The bus voltage exists already !","BUS VOLTAGE CHECK" , MB_OK|MB_ICONWARNING);
		m_HV2_BusVoltage.SetWindowText("");
	}
}

/**
	@brief	HV3 BUS VOLTAGE EDIT

	@author KHS		
**/
void CVoltageSettingDlg::OnEnChangeHv3BusVoltageEdit()
{
	// TODO:  Add your control notification handler code here
	CString rBusVoltage;
	m_HV3_BusVoltage.GetWindowText(rBusVoltage);
	if(!CheckBusVoltage(rBusVoltage, 3) && !rBusVoltage.IsEmpty())
	{
		::MessageBox(NULL, "The bus voltage exists already !","BUS VOLTAGE CHECK" , MB_OK|MB_ICONWARNING);
		m_HV3_BusVoltage.SetWindowText("");
	}
}
/**
	@brief	LV1 BUS VOLTAGE EDIT

	@author KHS		
**/
void CVoltageSettingDlg::OnEnChangeLv13phBusVoltageEdit()
{
	// TODO:  Add your control notification handler code here
	CString rBusVoltage;
	m_LV1_3PH_BusVoltage.GetWindowText(rBusVoltage);
	if(!CheckBusVoltage(rBusVoltage, 4) && !rBusVoltage.IsEmpty())
	{
		::MessageBox(NULL, "The bus voltage exists already !","BUS VOLTAGE CHECK" , MB_OK|MB_ICONWARNING);
		m_LV1_3PH_BusVoltage.SetWindowText("");
	}
}

/**
	@brief	LV2 BUS VOLTAGE EDIT

	@author KHS		
**/
void CVoltageSettingDlg::OnEnChangeLv23phBusVoltageEdit()
{
	// TODO:  Add your control notification handler code here
	CString rBusVoltage;
	m_LV2_3PH_BusVoltage.GetWindowText(rBusVoltage);
	if(!CheckBusVoltage(rBusVoltage, 5) && !rBusVoltage.IsEmpty())
	{
		::MessageBox(NULL, "The bus voltage exists already !","BUS VOLTAGE CHECK" , MB_OK|MB_ICONWARNING);
		m_LV2_3PH_BusVoltage.SetWindowText("");
	}
}
/**
	@brief	LV11 BUS VOLTAGE EDIT

	@author KHS		
**/
void CVoltageSettingDlg::OnEnChangeLv111phBusVoltageEdit()
{
	// TODO:  Add your control notification handler code here
	CString rBusVoltage;
	m_LV11_1PH_BusVoltage.GetWindowText(rBusVoltage);
	if(!CheckBusVoltage(rBusVoltage, 6) && !rBusVoltage.IsEmpty())
	{
		::MessageBox(NULL, "The bus voltage exists already !","BUS VOLTAGE CHECK" , MB_OK|MB_ICONWARNING);
		m_LV11_1PH_BusVoltage.SetWindowText("");
	}
}
/**
	@brief	LV12 BUS VOLTAGE EDIT

	@author KHS		
**/
void CVoltageSettingDlg::OnEnChangeLv121phBusVoltageEdit()
{
	// TODO:  Add your control notification handler code here
	CString rBusVoltage;
	m_LV12_1PH_BusVoltage.GetWindowText(rBusVoltage);
	if(!CheckBusVoltage(rBusVoltage, 7) && !rBusVoltage.IsEmpty())
	{
		::MessageBox(NULL, "The bus voltage exists already !","BUS VOLTAGE CHECK" , MB_OK|MB_ICONWARNING);
		m_LV12_1PH_BusVoltage.SetWindowText("");
	}
}
/**
	@brief	DC BUS VOLTAGE EDIT

	@author KHS		
**/
void CVoltageSettingDlg::OnEnChangeDcBusVoltageEdit()
{
	// TODO:  Add your control notification handler code here
	CString rBusVoltage;
	m_DC_BusVoltage.GetWindowText(rBusVoltage);
	if(!CheckBusVoltage(rBusVoltage, 8) && !rBusVoltage.IsEmpty())
	{
		::MessageBox(NULL, "The bus voltage exists already !","BUS VOLTAGE CHECK" , MB_OK|MB_ICONWARNING);
		m_DC_BusVoltage.SetWindowText("");
	}
}