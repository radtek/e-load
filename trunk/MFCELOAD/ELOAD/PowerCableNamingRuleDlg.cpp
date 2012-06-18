// PowerCableNamingRuleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "PowerCableNamingRuleDlg.h"
#include "GridCellEx.h"
#include "ELoadDocData.h"
#include "Tables/CableCreationSettingTable.h"
#include "FillGridCellComboBox.h"
#include <util/ado/SQLFile.h>

#define	READ_ONLY_COLOR	RGB(220, 220, 220)
#define	GRID_TEXT_COLOR	RGB(0, 0, 0)
#define RED_TEXT_COLOR	RGB(255, 0 , 0)


// CPowerCableNamingRuleDlg dialog

IMPLEMENT_DYNAMIC(CPowerCableNamingRuleDlg, CDialog)

CPowerCableNamingRuleDlg::CPowerCableNamingRuleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPowerCableNamingRuleDlg::IDD, pParent)
{
	m_nParallel = 0;
	m_nSingleCore = 1; 
}

CPowerCableNamingRuleDlg::~CPowerCableNamingRuleDlg()
{
}

void CPowerCableNamingRuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_wndOKButton);
	DDX_Control(pDX, IDCANCEL, m_wndCancelButton);

	//! VOLTAGE TYPE
	DDX_Control(pDX, IDC_HV1_VOLTAGE_TYPE_STATIC, m_HV1_VoltageTypeStatic);
	DDX_Control(pDX, IDC_HV2_VOLTAGE_TYPE_STATIC, m_HV2_VoltageTypeStatic);
	DDX_Control(pDX, IDC_HV3_VOLTAGE_TYPE_STATIC, m_HV3_VoltageTypeStatic);
	DDX_Control(pDX, IDC_LV1_VOLTAGE_TYPE_STATIC, m_LV1_VoltageTypeStatic);
	DDX_Control(pDX, IDC_LV2_VOLTAGE_TYPE_STATIC, m_LV2_VoltageTypeStatic);
	DDX_Control(pDX, IDC_LV11_VOLTAGE_TYPE_STATIC, m_LV11_VoltageTypeStatic);
	DDX_Control(pDX, IDC_LV12_VOLTAGE_TYPE_STATIC, m_LV12_VoltageTypeStatic);
	DDX_Control(pDX, IDC_DC_VOLTAGE_TYPE_STATIC, m_DC_VoltageTypeStatic);

	//! BUS VOLTAGE
	DDX_Control(pDX, IDC_HV1_BUS_VOLTAGE_EDIT2, m_HV1_BusVoltageEdit);
	DDX_Control(pDX, IDC_HV2_BUS_VOLTAGE_EDIT2, m_HV2_BusVoltageEdit);
	DDX_Control(pDX, IDC_HV3_BUS_VOLTAGE_EDIT2, m_HV3_BusVoltageEdit);
	DDX_Control(pDX, IDC_LV1_BUS_VOLTAGE_EDIT, m_LV1_BusVoltageEdit);
	DDX_Control(pDX, IDC_LV2_BUS_VOLTAGE_EDIT, m_LV2_BusVoltageEdit);
	DDX_Control(pDX, IDC_LV11_BUS_VOLTAGE_EDIT, m_LV11_BusVoltageEdit);
	DDX_Control(pDX, IDC_LV12_BUS_VOLTAGE_EDIT, m_LV12_BusVoltageEdit);
	DDX_Control(pDX, IDC_DC_BUS_VOLTAGE_EDIT, m_DC_BusVoltageEdit);

	//! LOAD VOLTAGE
	DDX_Control(pDX, IDC_HV1_LOAD_VOLTAGE_EDIT, m_HV1_LoadVoltageEdit);
	DDX_Control(pDX, IDC_HV2_LOAD_VOLTAGE_EDIT, m_HV2_LoadVoltageEdit);
	DDX_Control(pDX, IDC_HV3_LOAD_VOLTAGE_EDIT, m_HV3_LoadVoltageEdit);
	DDX_Control(pDX, IDC_LV1_LOAD_VOLTAGE_EDIT, m_LV1_LoadVoltageEdit);
	DDX_Control(pDX, IDC_LV2_LOAD_VOLTAGE_EDIT, m_LV2_LoadVoltageEdit);
	DDX_Control(pDX, IDC_LV11_LOAD_VOLTAGE_EDIT, m_LV11_LoadVoltageEdit);
	DDX_Control(pDX, IDC_LV12_LOAD_VOLTAGE_EDIT, m_LV12_LoadVoltageEdit);
	DDX_Control(pDX, IDC_DC_LOAD_VOLTAGE_EDIT, m_DC_LoadVoltageEdit);

	//!MAIN PREFIX
	DDX_Control(pDX, IDC_HV1_MAIN_PREFIX_EDIT, m_HV1_MainNamingPrefixEdit);
	DDX_Control(pDX, IDC_HV2_MAIN_PREFIX_EDIT, m_HV2_MainNamingPrefixEdit);
	DDX_Control(pDX, IDC_HV3_MAIN_PREFIX_EDIT, m_HV3_MainNamingPrefixEdit);
	DDX_Control(pDX, IDC_LV1_MAIN_PREFIX_EDIT, m_LV1_MainNamingPrefixEdit);
	DDX_Control(pDX, IDC_LV2_MAIN_PREFIX_EDIT, m_LV2_MainNamingPrefixEdit);
	DDX_Control(pDX, IDC_LV11_MAIN_PREFIX_EDIT, m_LV11_MainNamingPrefixEdit);
	DDX_Control(pDX, IDC_LV12_MAIN_PREFIX_EDIT, m_LV12_MainNamingPrefixEdit);
	DDX_Control(pDX, IDC_DC_MAIN_PREFIX_EDIT, m_DC_MainNamingPrefixEdit);

	//! MAIN SUFFIX
	DDX_Control(pDX, IDC_HV1_MAIN_SUFFIX_EDIT, m_HV1_MainNamingSuffixEdit);
	DDX_Control(pDX, IDC_HV2_MAIN_SUFFIX_EDIT, m_HV2_MainNamingSuffixEdit);
	DDX_Control(pDX, IDC_HV3_MAIN_SUFFIX_EDIT, m_HV3_MainNamingSuffixEdit);
	DDX_Control(pDX, IDC_LV1_MAIN_SUFFIX_EDIT, m_LV1_MainNamingSuffixEdit);
	DDX_Control(pDX, IDC_LV2_MAIN_SUFFIX_EDIT, m_LV2_MainNamingSuffixEdit);
	DDX_Control(pDX, IDC_LV11_MAIN_SUFFIX_EDIT, m_LV11_MainNamingSuffixEdit);
	DDX_Control(pDX, IDC_LV12_MAIN_SUFFIX_EDIT, m_LV12_MainNamingSuffixEdit);
	DDX_Control(pDX, IDC_DC_MAIN_SUFFIX_EDIT, m_DC_MainNamingSuffixEdit);

	//! SPACE HEADER PREFIX
	DDX_Control(pDX, IDC_HV1_SPACE_HEADER_PREFIX_EDIT, m_HV1_SpaceHeaderNamingPrefixEdit);
	DDX_Control(pDX, IDC_HV2_SPACE_HEADER_PREFIX_EDIT, m_HV2_SpaceHeaderNamingPrefixEdit);
	DDX_Control(pDX, IDC_HV3_SPACE_HEADER_PREFIX_EDIT, m_HV3_SpaceHeaderNamingPrefixEdit);
	DDX_Control(pDX, IDC_LV1_SPACE_HEADER_PREFIX_EDIT, m_LV1_SpaceHeaderNamingPrefixEdit);
	DDX_Control(pDX, IDC_LV2_SPACE_HEADER_PREFIX_EDIT, m_LV2_SpaceHeaderNamingPrefixEdit);

	//! SPACE HEADER SUFFIX
	DDX_Control(pDX, IDC_HV1_SPACE_HEADER_SUFFIX_EDIT, m_HV1_SpaceHeaderNamingSuffixEdit);
	DDX_Control(pDX, IDC_HV2_SPACE_HEADER_SUFFIX_EDIT, m_HV2_SpaceHeaderNamingSuffixEdit);
	DDX_Control(pDX, IDC_HV3_SPACE_HEADER_SUFFIX_EDIT, m_HV3_SpaceHeaderNamingSuffixEdit);
	DDX_Control(pDX, IDC_LV1_SPACE_HEADER_SUFFIX_EDIT, m_LV1_SpaceHeaderNamingSuffixEdit);
	DDX_Control(pDX, IDC_LV2_SPACE_HEADER_SUFFIX_EDIT, m_LV2_SpaceHeaderNamingSuffixEdit);
	DDX_Control(pDX, IDC_STATIC_NAMING_RULE_SETTING, m_StaticNamingRuleSetting);
	
	//! STATIC GROUP BOX
	DDX_Control(pDX, IDC_STATIC_SINGLE_CORE_NUMBERING, m_StaticSingleCoreCableNumbering);
	DDX_Control(pDX, IDC_STATIC_PARALLER_CABLE_NUMBERING, m_StaticParallerCableNumbering);

	//! MAIN NAING
	DDX_Control(pDX, IDC_HV1_MAIN_EX_STATIC, m_HV1_MainExStatic);
	DDX_Control(pDX, IDC_HV2_MAIN_EX_STATIC, m_HV2_MainExStatic);
	DDX_Control(pDX, IDC_HV3_MAIN_EX_STATIC, m_HV3_MainExStatic);
	DDX_Control(pDX, IDC_LV1_MAIN_EX_STATIC, m_LV1_MainExStatic);
	DDX_Control(pDX, IDC_LV2_MAIN_EX_STATIC, m_LV2_MainExStatic);
	DDX_Control(pDX, IDC_LV11_MAIN_EX_STATIC, m_LV11_MainExStatic);
	DDX_Control(pDX, IDC_LV12_MAIN_EX_STATIC, m_LV12_MainExStatic);
	DDX_Control(pDX, IDC_DC_MAIN_EX_STATIC, m_DC_MainExStatic);

	//! SPACE HEADER NAMING
	DDX_Control(pDX, IDC_HV1_SPACE_HEADER_EX_STATIC, m_HV1_SpaceHeaderExStatic);
	DDX_Control(pDX, IDC_HV2_SPACE_HEADER_EX_STATIC, m_HV2_SpaceHeaderExStatic);
	DDX_Control(pDX, IDC_HV3_SPACE_HEADER_EX_STATIC, m_HV3_SpaceHeaderExStatic);
	DDX_Control(pDX, IDC_LV1_SPACE_HEADER_EX_STATIC, m_LV1_SpaceHeaderExStatic);
	DDX_Control(pDX, IDC_LV2_SPACE_HEADER_EX_STATIC, m_LV2_SpaceHeaderExStatic);

	//! CABLE NAMING RULE SETTING
	DDX_Radio(pDX , IDC_PARALLEL_123_RADIO , m_nParallel);
	DDX_Radio(pDX , IDC_SINGLE_123_RADIO , m_nSingleCore);

}


BEGIN_MESSAGE_MAP(CPowerCableNamingRuleDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CPowerCableNamingRuleDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_HV1_MAIN_PREFIX_EDIT, &CPowerCableNamingRuleDlg::OnEnChangeHv1MainPrefixEdit)
	ON_EN_CHANGE(IDC_HV2_MAIN_PREFIX_EDIT, &CPowerCableNamingRuleDlg::OnEnChangeHv2MainPrefixEdit)
	ON_EN_CHANGE(IDC_HV3_MAIN_PREFIX_EDIT, &CPowerCableNamingRuleDlg::OnEnChangeHv3MainPrefixEdit)
	ON_EN_CHANGE(IDC_LV1_MAIN_PREFIX_EDIT, &CPowerCableNamingRuleDlg::OnEnChangeLv1MainPrefixEdit)
	ON_EN_CHANGE(IDC_LV2_MAIN_PREFIX_EDIT, &CPowerCableNamingRuleDlg::OnEnChangeLv2MainPrefixEdit)
	ON_EN_CHANGE(IDC_LV11_MAIN_PREFIX_EDIT, &CPowerCableNamingRuleDlg::OnEnChangeLv11MainPrefixEdit)
	ON_EN_CHANGE(IDC_LV12_MAIN_PREFIX_EDIT, &CPowerCableNamingRuleDlg::OnEnChangeLv12MainPrefixEdit)
	ON_EN_CHANGE(IDC_DC_MAIN_PREFIX_EDIT, &CPowerCableNamingRuleDlg::OnEnChangeDcMainPrefixEdit)
	ON_EN_CHANGE(IDC_HV1_MAIN_SUFFIX_EDIT, &CPowerCableNamingRuleDlg::OnEnChangeHv1MainSuffixEdit)
	ON_EN_CHANGE(IDC_HV2_MAIN_SUFFIX_EDIT, &CPowerCableNamingRuleDlg::OnEnChangeHv2MainSuffixEdit)
	ON_EN_CHANGE(IDC_HV3_MAIN_SUFFIX_EDIT, &CPowerCableNamingRuleDlg::OnEnChangeHv3MainSuffixEdit)
	ON_EN_CHANGE(IDC_LV1_MAIN_SUFFIX_EDIT, &CPowerCableNamingRuleDlg::OnEnChangeLv1MainSuffixEdit)
	ON_EN_CHANGE(IDC_LV2_MAIN_SUFFIX_EDIT, &CPowerCableNamingRuleDlg::OnEnChangeLv2MainSuffixEdit)
	ON_EN_CHANGE(IDC_LV11_MAIN_SUFFIX_EDIT, &CPowerCableNamingRuleDlg::OnEnChangeLv11MainSuffixEdit)
	ON_EN_CHANGE(IDC_LV12_MAIN_SUFFIX_EDIT, &CPowerCableNamingRuleDlg::OnEnChangeLv12MainSuffixEdit)
	ON_EN_CHANGE(IDC_DC_MAIN_SUFFIX_EDIT, &CPowerCableNamingRuleDlg::OnEnChangeDcMainSuffixEdit)
	ON_EN_CHANGE(IDC_HV1_SPACE_HEADER_PREFIX_EDIT, &CPowerCableNamingRuleDlg::OnEnChangeHv1SpaceHeaderPrefixEdit)
	ON_EN_CHANGE(IDC_HV2_SPACE_HEADER_PREFIX_EDIT, &CPowerCableNamingRuleDlg::OnEnChangeHv2SpaceHeaderPrefixEdit)
	ON_EN_CHANGE(IDC_HV3_SPACE_HEADER_PREFIX_EDIT, &CPowerCableNamingRuleDlg::OnEnChangeHv3SpaceHeaderPrefixEdit)
	ON_EN_CHANGE(IDC_LV1_SPACE_HEADER_PREFIX_EDIT, &CPowerCableNamingRuleDlg::OnEnChangeLv1SpaceHeaderPrefixEdit)
	ON_EN_CHANGE(IDC_LV2_SPACE_HEADER_PREFIX_EDIT, &CPowerCableNamingRuleDlg::OnEnChangeLv2SpaceHeaderPrefixEdit)
	ON_EN_CHANGE(IDC_HV1_SPACE_HEADER_SUFFIX_EDIT, &CPowerCableNamingRuleDlg::OnEnChangeHv1SpaceHeaderSuffixEdit)
	ON_EN_CHANGE(IDC_HV2_SPACE_HEADER_SUFFIX_EDIT, &CPowerCableNamingRuleDlg::OnEnChangeHv2SpaceHeaderSuffixEdit)
	ON_EN_CHANGE(IDC_HV3_SPACE_HEADER_SUFFIX_EDIT, &CPowerCableNamingRuleDlg::OnEnChangeHv3SpaceHeaderSuffixEdit)
	ON_EN_CHANGE(IDC_LV1_SPACE_HEADER_SUFFIX_EDIT, &CPowerCableNamingRuleDlg::OnEnChangeLv1SpaceHeaderSuffixEdit)
	ON_EN_CHANGE(IDC_LV2_SPACE_HEADER_SUFFIX_EDIT, &CPowerCableNamingRuleDlg::OnEnChangeLv2SpaceHeaderSuffixEdit)
END_MESSAGE_MAP()


// CPowerCableNamingRuleDlg message handlers

BOOL CPowerCableNamingRuleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	BeginWaitCursor();

	const COLORREF GroupBoxTextColor = RGB(0,70,213);
	m_StaticNamingRuleSetting.SetTextColor(GroupBoxTextColor, FALSE);
	m_StaticSingleCoreCableNumbering.SetTextColor(GroupBoxTextColor, FALSE);
	m_StaticParallerCableNumbering.SetTextColor(GroupBoxTextColor, FALSE);

	LoadData();
	SetExampleText();
	DisplayData();

	//! 아이템의  BusVoltage가 없으면 READ ONLY로 셋팅한다.
	CheckItemAndReadOnly();

	//! SET BUTTON ICON
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	m_wndOKButton.SetTextHotColor(TextHotColor);
	m_wndCancelButton.SetTextHotColor(TextHotColor);
	
	HICON hOKIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_OK_18X18),
		IMAGE_ICON,
		18,18, 0 );
	m_wndOKButton.SetIcon(hOKIcon);
	
	HICON hCancelIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_CANCEL_18X18),
		IMAGE_ICON,
		18,18, 0 );
	m_wndCancelButton.SetIcon(hCancelIcon);

	EndWaitCursor();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief  DB 데이타를 읽어온다.
	
	@author KHS
*/
int CPowerCableNamingRuleDlg::LoadData(void)
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
	
	CCableNamingRuleSettingTable& CableNamingRuleSettingTable = CCableNamingRuleSettingTable::GetInstance();
	CableNamingRuleSettingTable.Load(rMDBFilePath);

	return ERROR_SUCCESS;
}

/**
	@brief  SETTING 내용을 화면에 표시한다.
	
	@author KHS,BHK
*/
int CPowerCableNamingRuleDlg::DisplayData(void)
{
	CString rPrefix, rSuffix, rLoadId = "PM-9500";
	
	int nRow = 0;
	DisplayHVCableData();
	DisplayLV3PHCableData();
	DisplayLV1PHCableData();
	DisplayDCCableData();
	
	DisplayCableNamingRuleSetting();

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	KHS
*/
void CPowerCableNamingRuleDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	SaveData();
	OnOK();
}
/*
	@brief  SETTING DATA를 DB에 저장한다.

	@author KHS
**/
int CPowerCableNamingRuleDlg::SaveData(void)
{
	CString rVoltageType, rBusVoltage, rLoadVoltage, rMainNamingPrefix, rMainNamingSuffix, rSpaceHeaderNamingPrefix, rSpaceHeaderNamingSuffix;

	CHVCableCreationSettingTable& HVVoltageSettingTable = CHVCableCreationSettingTable::GetInstance();
	int i = 1;
	for(vector<CCableCreationTable::Record>::iterator itr = HVVoltageSettingTable.m_RecordEntry.begin();itr != HVVoltageSettingTable.m_RecordEntry.end();++itr,++i)
	{
		if(i == 1)
		{
			m_HV1_VoltageTypeStatic.GetWindowText(rVoltageType);
			m_HV1_BusVoltageEdit.GetWindowText(rBusVoltage);
			m_HV1_LoadVoltageEdit.GetWindowText(rLoadVoltage);
			m_HV1_MainNamingPrefixEdit.GetWindowText(rMainNamingPrefix);
			m_HV1_MainNamingSuffixEdit.GetWindowText(rMainNamingSuffix);
			m_HV1_SpaceHeaderNamingPrefixEdit.GetWindowText(rSpaceHeaderNamingPrefix);
			m_HV1_SpaceHeaderNamingSuffixEdit.GetWindowText(rSpaceHeaderNamingSuffix);
		}
		else if(i == 2)
		{
			m_HV2_VoltageTypeStatic.GetWindowText(rVoltageType);
			m_HV2_BusVoltageEdit.GetWindowText(rBusVoltage);
			m_HV2_LoadVoltageEdit.GetWindowText(rLoadVoltage);
			m_HV2_MainNamingPrefixEdit.GetWindowText(rMainNamingPrefix);
			m_HV2_MainNamingSuffixEdit.GetWindowText(rMainNamingSuffix);
			m_HV2_SpaceHeaderNamingPrefixEdit.GetWindowText(rSpaceHeaderNamingPrefix);
			m_HV2_SpaceHeaderNamingSuffixEdit.GetWindowText(rSpaceHeaderNamingSuffix);
		}
		else if(i == 3)
		{
			m_HV3_VoltageTypeStatic.GetWindowText(rVoltageType);
			m_HV3_BusVoltageEdit.GetWindowText(rBusVoltage);
			m_HV3_LoadVoltageEdit.GetWindowText(rLoadVoltage);
			m_HV3_MainNamingPrefixEdit.GetWindowText(rMainNamingPrefix);
			m_HV3_MainNamingSuffixEdit.GetWindowText(rMainNamingSuffix);
			m_HV3_SpaceHeaderNamingPrefixEdit.GetWindowText(rSpaceHeaderNamingPrefix);
			m_HV3_SpaceHeaderNamingSuffixEdit.GetWindowText(rSpaceHeaderNamingSuffix);
		}

		itr->FieldValueMap["C_VOLTAGE_TYPE"]				= rVoltageType.operator LPCSTR();
		itr->FieldValueMap["C_BUS_VOLTAGE"]				= rBusVoltage.operator LPCSTR();
		itr->FieldValueMap["C_LOAD_VOLTAGE"]				= rLoadVoltage.operator LPCSTR();
		itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_PREFIX"]		= rMainNamingPrefix.operator LPCSTR();
		itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_SUFFIX"]		= rMainNamingSuffix.operator LPCSTR();
		itr->FieldValueMap["C_SPACE_HEATER_POWER_CABLE_NAMING_PREFIX"]	= rSpaceHeaderNamingPrefix.operator LPCSTR();
		itr->FieldValueMap["C_SPACE_HEATER_POWER_CABLE_NAMING_SUFFIX"]	= rSpaceHeaderNamingSuffix.operator LPCSTR();

		if(3 == i) break;
	}
	HVVoltageSettingTable.Update();

	CLV3PHCableCreationSettingTable& LV3PHVoltageSettingTable = CLV3PHCableCreationSettingTable::GetInstance();
	i = 1;
	for(vector<CCableCreationTable::Record>::iterator itr = LV3PHVoltageSettingTable.m_RecordEntry.begin();itr != LV3PHVoltageSettingTable.m_RecordEntry.end();++itr,++i)
	{
		if(i == 1)
		{
			m_LV1_VoltageTypeStatic.GetWindowText(rVoltageType);
			m_LV1_BusVoltageEdit.GetWindowText(rBusVoltage);
			m_LV1_LoadVoltageEdit.GetWindowText(rLoadVoltage);
			m_LV1_MainNamingPrefixEdit.GetWindowText(rMainNamingPrefix);
			m_LV1_MainNamingSuffixEdit.GetWindowText(rMainNamingSuffix);
			m_LV1_SpaceHeaderNamingPrefixEdit.GetWindowText(rSpaceHeaderNamingPrefix);
			m_LV1_SpaceHeaderNamingSuffixEdit.GetWindowText(rSpaceHeaderNamingSuffix);
		}
		else if(i == 2)
		{
			m_LV2_VoltageTypeStatic.GetWindowText(rVoltageType);
			m_LV2_BusVoltageEdit.GetWindowText(rBusVoltage);
			m_LV2_LoadVoltageEdit.GetWindowText(rLoadVoltage);
			m_LV2_MainNamingPrefixEdit.GetWindowText(rMainNamingPrefix);
			m_LV2_MainNamingSuffixEdit.GetWindowText(rMainNamingSuffix);
			m_LV2_SpaceHeaderNamingPrefixEdit.GetWindowText(rSpaceHeaderNamingPrefix);
			m_LV2_SpaceHeaderNamingSuffixEdit.GetWindowText(rSpaceHeaderNamingSuffix);
		}

		itr->FieldValueMap["C_VOLTAGE_TYPE"]				= rVoltageType.operator LPCSTR();
		itr->FieldValueMap["C_BUS_VOLTAGE"]				= rBusVoltage.operator LPCSTR();
		itr->FieldValueMap["C_LOAD_VOLTAGE"]				= rLoadVoltage.operator LPCSTR();
		itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_PREFIX"]		= rMainNamingPrefix.operator LPCSTR();
		itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_SUFFIX"]		= rMainNamingSuffix.operator LPCSTR();
		itr->FieldValueMap["C_SPACE_HEATER_POWER_CABLE_NAMING_PREFIX"]	= rSpaceHeaderNamingPrefix.operator LPCSTR();
		itr->FieldValueMap["C_SPACE_HEATER_POWER_CABLE_NAMING_SUFFIX"]	= rSpaceHeaderNamingSuffix.operator LPCSTR();

		if(2 == i) break;
	}
	LV3PHVoltageSettingTable.Update();

	CLV1PHCableCreationSettingTable& LV1PHVoltageSettingTable = CLV1PHCableCreationSettingTable::GetInstance();
	i = 1;
	for(vector<CCableCreationTable::Record>::iterator itr = LV1PHVoltageSettingTable.m_RecordEntry.begin();itr != LV1PHVoltageSettingTable.m_RecordEntry.end();++itr,++i)
	{
		if(i == 1)
		{
			m_LV11_VoltageTypeStatic.GetWindowText(rVoltageType);
			m_LV11_BusVoltageEdit.GetWindowText(rBusVoltage);
			m_LV11_LoadVoltageEdit.GetWindowText(rLoadVoltage);
			m_LV11_MainNamingPrefixEdit.GetWindowText(rMainNamingPrefix);
			m_LV11_MainNamingSuffixEdit.GetWindowText(rMainNamingSuffix);
		}
		else if(i == 2)
		{
			m_LV12_VoltageTypeStatic.GetWindowText(rVoltageType);
			m_LV12_BusVoltageEdit.GetWindowText(rBusVoltage);
			m_LV12_LoadVoltageEdit.GetWindowText(rLoadVoltage);
			m_LV12_MainNamingPrefixEdit.GetWindowText(rMainNamingPrefix);
			m_LV12_MainNamingSuffixEdit.GetWindowText(rMainNamingSuffix);
		}

		itr->FieldValueMap["C_VOLTAGE_TYPE"]				= rVoltageType.operator LPCSTR();
		itr->FieldValueMap["C_BUS_VOLTAGE"]				= rBusVoltage.operator LPCSTR();
		itr->FieldValueMap["C_LOAD_VOLTAGE"]				= rLoadVoltage.operator LPCSTR();
		itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_PREFIX"]		= rMainNamingPrefix.operator LPCSTR();
		itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_SUFFIX"]		= rMainNamingSuffix.operator LPCSTR();

		if(2 == i) break;
	}
	LV1PHVoltageSettingTable.Update();

	CDCCableCreationSettingTable& DCVoltageSettingTable = CDCCableCreationSettingTable::GetInstance();
	i = 1;
	for(vector<CCableCreationTable::Record>::iterator itr = DCVoltageSettingTable.m_RecordEntry.begin();itr != DCVoltageSettingTable.m_RecordEntry.end();++itr,++i)
	{
		if(i == 1)
		{
			m_DC_VoltageTypeStatic.GetWindowText(rVoltageType);
			m_DC_BusVoltageEdit.GetWindowText(rBusVoltage);
			m_DC_LoadVoltageEdit.GetWindowText(rLoadVoltage);
			m_DC_MainNamingPrefixEdit.GetWindowText(rMainNamingPrefix);
			m_DC_MainNamingSuffixEdit.GetWindowText(rMainNamingSuffix);
		}

		itr->FieldValueMap["C_VOLTAGE_TYPE"]				= rVoltageType.operator LPCSTR();
		itr->FieldValueMap["C_BUS_VOLTAGE"]				= rBusVoltage.operator LPCSTR();
		itr->FieldValueMap["C_LOAD_VOLTAGE"]				= rLoadVoltage.operator LPCSTR();
		itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_PREFIX"]		= rMainNamingPrefix.operator LPCSTR();
		itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_SUFFIX"]		= rMainNamingSuffix.operator LPCSTR();

		if(1 == i) break;
	}
	DCVoltageSettingTable.Update();

	//! CABLE NAMING RULE SETTING 을 DB에 저장한다.
	SaveCableNamingRuleSettingData();

	return ERROR_SUCCESS;
}


BOOL CPowerCableNamingRuleDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{	
	return CDialog::OnNotify(wParam, lParam, pResult);
}
/**
	@brief  EXAMPLE TEXT SETTING
	@author KHS
*/
int CPowerCableNamingRuleDlg::SetExampleText(void)
{
	m_HV1_MainExStatic.SetTextColor(RED_TEXT_COLOR);
	m_HV2_MainExStatic.SetTextColor(RED_TEXT_COLOR);
	m_HV3_MainExStatic.SetTextColor(RED_TEXT_COLOR);
	m_LV1_MainExStatic.SetTextColor(RED_TEXT_COLOR);
	m_LV2_MainExStatic.SetTextColor(RED_TEXT_COLOR);
	m_LV11_MainExStatic.SetTextColor(RED_TEXT_COLOR);
	m_LV12_MainExStatic.SetTextColor(RED_TEXT_COLOR);
	m_DC_MainExStatic.SetTextColor(RED_TEXT_COLOR);

	m_HV1_SpaceHeaderExStatic.SetTextColor(RED_TEXT_COLOR);
	m_HV2_SpaceHeaderExStatic.SetTextColor(RED_TEXT_COLOR);
	m_HV3_SpaceHeaderExStatic.SetTextColor(RED_TEXT_COLOR);
	m_LV1_SpaceHeaderExStatic.SetTextColor(RED_TEXT_COLOR);
	m_LV2_SpaceHeaderExStatic.SetTextColor(RED_TEXT_COLOR);

	return ERROR_SUCCESS;
}

void CPowerCableNamingRuleDlg::OnEnChangeHv1MainPrefixEdit()
{
	// TODO:  Add your control notification handler code here
	onEditBoxEndEdit(&m_HV1_MainNamingPrefixEdit, &m_HV1_MainNamingSuffixEdit, &m_HV1_MainExStatic);
	
}

void CPowerCableNamingRuleDlg::OnEnChangeHv2MainPrefixEdit()
{
	// TODO:  Add your control notification handler code here
	onEditBoxEndEdit(&m_HV2_MainNamingPrefixEdit, &m_HV2_MainNamingSuffixEdit, &m_HV2_MainExStatic);
}

void CPowerCableNamingRuleDlg::OnEnChangeHv3MainPrefixEdit()
{
	// TODO:  Add your control notification handler code here
	onEditBoxEndEdit(&m_HV3_MainNamingPrefixEdit, &m_HV3_MainNamingSuffixEdit, &m_HV3_MainExStatic);
}

void CPowerCableNamingRuleDlg::OnEnChangeLv1MainPrefixEdit()
{
	// TODO:  Add your control notification handler code here
	onEditBoxEndEdit(&m_LV1_MainNamingPrefixEdit, &m_LV1_MainNamingSuffixEdit, &m_LV1_MainExStatic);
}

void CPowerCableNamingRuleDlg::OnEnChangeLv2MainPrefixEdit()
{
	// TODO:  Add your control notification handler code here
	onEditBoxEndEdit(&m_LV2_MainNamingPrefixEdit, &m_LV2_MainNamingSuffixEdit, &m_LV2_MainExStatic);
}

void CPowerCableNamingRuleDlg::OnEnChangeLv11MainPrefixEdit()
{
	// TODO:  Add your control notification handler code here
	onEditBoxEndEdit(&m_LV11_MainNamingPrefixEdit, &m_LV11_MainNamingSuffixEdit, &m_LV11_MainExStatic);
}

void CPowerCableNamingRuleDlg::OnEnChangeLv12MainPrefixEdit()
{
	// TODO:  Add your control notification handler code here
	onEditBoxEndEdit(&m_LV12_MainNamingPrefixEdit, &m_LV12_MainNamingSuffixEdit, &m_LV12_MainExStatic);
}

void CPowerCableNamingRuleDlg::OnEnChangeDcMainPrefixEdit()
{
	// TODO:  Add your control notification handler code here
	onEditBoxEndEdit(&m_DC_MainNamingPrefixEdit, &m_DC_MainNamingSuffixEdit, &m_DC_MainExStatic);
}

void CPowerCableNamingRuleDlg::OnEnChangeHv1MainSuffixEdit()
{
	// TODO:  Add your control notification handler code here
	onEditBoxEndEdit(&m_HV1_MainNamingPrefixEdit, &m_HV1_MainNamingSuffixEdit, &m_HV1_MainExStatic);
}

void CPowerCableNamingRuleDlg::OnEnChangeHv2MainSuffixEdit()
{
	// TODO:  Add your control notification handler code here
	onEditBoxEndEdit(&m_HV2_MainNamingPrefixEdit, &m_HV2_MainNamingSuffixEdit, &m_HV2_MainExStatic);
}

void CPowerCableNamingRuleDlg::OnEnChangeHv3MainSuffixEdit()
{
	// TODO:  Add your control notification handler code here
	onEditBoxEndEdit(&m_HV3_MainNamingPrefixEdit, &m_HV3_MainNamingSuffixEdit, &m_HV3_MainExStatic);
}

void CPowerCableNamingRuleDlg::OnEnChangeLv1MainSuffixEdit()
{
	// TODO:  Add your control notification handler code here
	onEditBoxEndEdit(&m_LV1_MainNamingPrefixEdit, &m_LV1_MainNamingSuffixEdit, &m_LV1_MainExStatic);
}

void CPowerCableNamingRuleDlg::OnEnChangeLv2MainSuffixEdit()
{
	// TODO:  Add your control notification handler code here
	onEditBoxEndEdit(&m_LV2_MainNamingPrefixEdit, &m_LV2_MainNamingSuffixEdit, &m_LV2_MainExStatic);
}

void CPowerCableNamingRuleDlg::OnEnChangeLv11MainSuffixEdit()
{
	// TODO:  Add your control notification handler code here
	onEditBoxEndEdit(&m_LV11_MainNamingPrefixEdit, &m_LV11_MainNamingSuffixEdit, &m_LV11_MainExStatic);
}

void CPowerCableNamingRuleDlg::OnEnChangeLv12MainSuffixEdit()
{
	// TODO:  Add your control notification handler code here
	onEditBoxEndEdit(&m_LV12_MainNamingPrefixEdit, &m_LV12_MainNamingSuffixEdit, &m_LV12_MainExStatic);
}

void CPowerCableNamingRuleDlg::OnEnChangeDcMainSuffixEdit()
{
	// TODO:  Add your control notification handler code here
	onEditBoxEndEdit(&m_DC_MainNamingPrefixEdit, &m_DC_MainNamingSuffixEdit, &m_DC_MainExStatic);
}

void CPowerCableNamingRuleDlg::OnEnChangeHv1SpaceHeaderPrefixEdit()
{
	// TODO:  Add your control notification handler code here
	onEditBoxEndEdit(&m_HV1_SpaceHeaderNamingPrefixEdit, &m_HV1_SpaceHeaderNamingSuffixEdit, &m_HV1_SpaceHeaderExStatic);
}

void CPowerCableNamingRuleDlg::OnEnChangeHv2SpaceHeaderPrefixEdit()
{
	// TODO:  Add your control notification handler code here
	onEditBoxEndEdit(&m_HV2_SpaceHeaderNamingPrefixEdit, &m_HV2_SpaceHeaderNamingSuffixEdit, &m_HV2_SpaceHeaderExStatic);
}

void CPowerCableNamingRuleDlg::OnEnChangeHv3SpaceHeaderPrefixEdit()
{
	// TODO:  Add your control notification handler code here
	onEditBoxEndEdit(&m_HV3_SpaceHeaderNamingPrefixEdit, &m_HV3_SpaceHeaderNamingSuffixEdit, &m_HV3_SpaceHeaderExStatic);
}

void CPowerCableNamingRuleDlg::OnEnChangeLv1SpaceHeaderPrefixEdit()
{
	// TODO:  Add your control notification handler code here
	onEditBoxEndEdit(&m_LV1_SpaceHeaderNamingPrefixEdit, &m_LV1_SpaceHeaderNamingSuffixEdit, &m_LV1_SpaceHeaderExStatic);
}

void CPowerCableNamingRuleDlg::OnEnChangeLv2SpaceHeaderPrefixEdit()
{

	// TODO:  Add your control notification handler code here
	onEditBoxEndEdit(&m_LV2_SpaceHeaderNamingPrefixEdit, &m_LV2_SpaceHeaderNamingSuffixEdit, &m_LV2_SpaceHeaderExStatic);
}

void CPowerCableNamingRuleDlg::OnEnChangeHv1SpaceHeaderSuffixEdit()
{
	// TODO:  Add your control notification handler code here
	onEditBoxEndEdit(&m_HV1_SpaceHeaderNamingPrefixEdit, &m_HV1_SpaceHeaderNamingSuffixEdit, &m_HV1_SpaceHeaderExStatic);
}

void CPowerCableNamingRuleDlg::OnEnChangeHv2SpaceHeaderSuffixEdit()
{
	// TODO:  Add your control notification handler code here
	onEditBoxEndEdit(&m_HV2_SpaceHeaderNamingPrefixEdit, &m_HV2_SpaceHeaderNamingSuffixEdit, &m_HV2_SpaceHeaderExStatic);
}

void CPowerCableNamingRuleDlg::OnEnChangeHv3SpaceHeaderSuffixEdit()
{
	// TODO:  Add your control notification handler code here
	onEditBoxEndEdit(&m_HV3_SpaceHeaderNamingPrefixEdit, &m_HV3_SpaceHeaderNamingSuffixEdit, &m_HV3_SpaceHeaderExStatic);
}

void CPowerCableNamingRuleDlg::OnEnChangeLv1SpaceHeaderSuffixEdit()
{
	// TODO:  Add your control notification handler code here
	onEditBoxEndEdit(&m_LV1_SpaceHeaderNamingPrefixEdit, &m_LV1_SpaceHeaderNamingSuffixEdit, &m_LV1_SpaceHeaderExStatic);
}

void CPowerCableNamingRuleDlg::OnEnChangeLv2SpaceHeaderSuffixEdit()
{
	// TODO:  Add your control notification handler code here
	onEditBoxEndEdit(&m_LV2_SpaceHeaderNamingPrefixEdit, &m_LV2_SpaceHeaderNamingSuffixEdit, &m_LV2_SpaceHeaderExStatic);
}

int CPowerCableNamingRuleDlg::onEditBoxEndEdit(CEdit* prefixEdit, CEdit* suffixEdit, CLabelStatic* resultStatic)
{
	CString rPrefix, rSuffix, rLoadId = "PM-9500A";
	prefixEdit->GetWindowText(rPrefix);
	suffixEdit->GetWindowText(rSuffix);
	resultStatic->SetWindowText("ex> " + rPrefix + rLoadId + rSuffix);
	resultStatic->RedrawWindow();

	SetExampleText();

	return ERROR_SUCCESS;
}

BOOL CPowerCableNamingRuleDlg::PreTranslateMessage(MSG* pMsg)
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
	@ brief  power cable Item의 Read Only Setting

	@ author KHS
*/
int CPowerCableNamingRuleDlg::CheckItemAndReadOnly(void)
{
	CString rHv1BusVoltage, rHv2BusVoltage, rHv3BusVoltage, rLv1BusVoltage, rLv2BusVoltage,
		rLv11BusVoltage, rLv12BusVoltage, rDCBusVoltage;

	m_HV1_BusVoltageEdit.GetWindowText(rHv1BusVoltage);
	if(rHv1BusVoltage.IsEmpty())
	{
		m_HV1_LoadVoltageEdit.EnableWindow(FALSE);
		GetDlgItem(IDC_HV1_VOLTAGE_TYPE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_HV1_PHASE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_HV1_POWER_CABLE_LOAD_ID)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_HV1_SPACE_HEATER_LOAD_ID)->EnableWindow(FALSE);
		GetDlgItem(IDC_HV1_MAIN_EX_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_HV1_SPACE_HEADER_EX_STATIC)->EnableWindow(FALSE);

		m_HV1_MainNamingPrefixEdit.EnableWindow(FALSE);
		m_HV1_MainNamingSuffixEdit.EnableWindow(FALSE);
		m_HV1_SpaceHeaderNamingPrefixEdit.EnableWindow(FALSE);
		m_HV1_SpaceHeaderNamingSuffixEdit.EnableWindow(FALSE);
		m_HV1_MainExStatic.EnableWindow(FALSE);
		m_HV1_SpaceHeaderExStatic.EnableWindow(FALSE);
	}
	m_HV2_BusVoltageEdit.GetWindowText(rHv2BusVoltage);
	if(rHv2BusVoltage.IsEmpty())
	{
		m_HV2_LoadVoltageEdit.EnableWindow(FALSE);
		GetDlgItem(IDC_HV2_VOLTAGE_TYPE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_HV2_PHASE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_HV2_POWER_CABLE_LOAD_ID)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_HV2_SPACE_HEATER_LOAD_ID)->EnableWindow(FALSE);
		GetDlgItem(IDC_HV2_MAIN_EX_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_HV2_SPACE_HEADER_EX_STATIC)->EnableWindow(FALSE);

		m_HV2_MainNamingPrefixEdit.EnableWindow(FALSE);
		m_HV2_MainNamingSuffixEdit.EnableWindow(FALSE);
		m_HV2_SpaceHeaderNamingPrefixEdit.EnableWindow(FALSE);
		m_HV2_SpaceHeaderNamingSuffixEdit.EnableWindow(FALSE);
		m_HV2_MainExStatic.EnableWindow(FALSE);
		m_HV2_SpaceHeaderExStatic.EnableWindow(FALSE);
	}
	m_HV3_BusVoltageEdit.GetWindowText(rHv3BusVoltage);
	if(rHv3BusVoltage.IsEmpty())
	{
		m_HV3_LoadVoltageEdit.EnableWindow(FALSE);
		GetDlgItem(IDC_HV3_VOLTAGE_TYPE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_HV3_PHASE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_HV3_POWER_CABLE_LOAD_ID)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_HV3_SPACE_HEATER_LOAD_ID)->EnableWindow(FALSE);
		GetDlgItem(IDC_HV3_MAIN_EX_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_HV3_SPACE_HEADER_EX_STATIC)->EnableWindow(FALSE);

		m_HV3_MainNamingPrefixEdit.EnableWindow(FALSE);
		m_HV3_MainNamingSuffixEdit.EnableWindow(FALSE);
		m_HV3_SpaceHeaderNamingPrefixEdit.EnableWindow(FALSE);
		m_HV3_SpaceHeaderNamingSuffixEdit.EnableWindow(FALSE);
		m_HV3_MainExStatic.EnableWindow(FALSE);
		m_HV3_SpaceHeaderExStatic.EnableWindow(FALSE);
	}
	m_LV1_BusVoltageEdit.GetWindowText(rLv1BusVoltage);
	if(rLv1BusVoltage.IsEmpty())
	{
		m_LV1_LoadVoltageEdit.EnableWindow(FALSE);
		GetDlgItem(IDC_LV1_VOLTAGE_TYPE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_LV1_3PH_PHASE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_LV1_POWER_CABLE_LOAD_ID)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_LV1_SPACE_HEATER_LOAD_ID)->EnableWindow(FALSE);
		GetDlgItem(IDC_LV1_MAIN_EX_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_LV1_SPACE_HEADER_EX_STATIC)->EnableWindow(FALSE);

		m_LV1_MainNamingPrefixEdit.EnableWindow(FALSE);
		m_LV1_MainNamingSuffixEdit.EnableWindow(FALSE);
		m_LV1_SpaceHeaderNamingPrefixEdit.EnableWindow(FALSE);
		m_LV1_SpaceHeaderNamingSuffixEdit.EnableWindow(FALSE);
		m_LV1_MainExStatic.EnableWindow(FALSE);
		m_LV1_SpaceHeaderExStatic.EnableWindow(FALSE);
	}
	m_LV2_BusVoltageEdit.GetWindowText(rLv2BusVoltage);
	if(rLv2BusVoltage.IsEmpty())
	{
		m_LV2_LoadVoltageEdit.EnableWindow(FALSE);
		GetDlgItem(IDC_LV2_VOLTAGE_TYPE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_LV2_3PH_PHASE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_LV2_POWER_CABLE_LOAD_ID)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_LV2_SPACE_HEATER_LOAD_ID)->EnableWindow(FALSE);
		GetDlgItem(IDC_LV2_MAIN_EX_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_LV2_SPACE_HEADER_EX_STATIC)->EnableWindow(FALSE);

		m_LV2_MainNamingPrefixEdit.EnableWindow(FALSE);
		m_LV2_MainNamingSuffixEdit.EnableWindow(FALSE);
		m_LV2_SpaceHeaderNamingPrefixEdit.EnableWindow(FALSE);
		m_LV2_SpaceHeaderNamingSuffixEdit.EnableWindow(FALSE);
		m_LV2_MainExStatic.EnableWindow(FALSE);
		m_LV2_SpaceHeaderExStatic.EnableWindow(FALSE);

	}
	m_LV11_BusVoltageEdit.GetWindowText(rLv11BusVoltage);
	if(rLv11BusVoltage.IsEmpty())
	{
		m_LV11_LoadVoltageEdit.EnableWindow(FALSE);
		GetDlgItem(IDC_LV11_VOLTAGE_TYPE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_LV11_1PH_PHASE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_LV11_POWER_CABLE_LOAD_ID)->EnableWindow(FALSE);
		GetDlgItem(IDC_LV11_MAIN_EX_STATIC)->EnableWindow(FALSE);
		
		m_LV11_MainNamingPrefixEdit.EnableWindow(FALSE);
		m_LV11_MainNamingSuffixEdit.EnableWindow(FALSE);

		m_LV11_MainExStatic.EnableWindow(FALSE);
	}
	m_LV12_BusVoltageEdit.GetWindowText(rLv12BusVoltage);
	if(rLv12BusVoltage.IsEmpty())
	{
		m_LV12_LoadVoltageEdit.EnableWindow(FALSE);
		GetDlgItem(IDC_LV12_VOLTAGE_TYPE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_LV12_1PH_PHASE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_LV12_POWER_CABLE_LOAD_ID)->EnableWindow(FALSE);
		GetDlgItem(IDC_LV12_MAIN_EX_STATIC)->EnableWindow(FALSE);

		m_LV12_MainNamingPrefixEdit.EnableWindow(FALSE);
		m_LV12_MainNamingSuffixEdit.EnableWindow(FALSE);
		m_LV12_MainExStatic.EnableWindow(FALSE);

	}
	m_DC_BusVoltageEdit.GetWindowText(rDCBusVoltage);
	if(rDCBusVoltage.IsEmpty())
	{
		m_DC_LoadVoltageEdit.EnableWindow(FALSE);
		GetDlgItem(IDC_DC_VOLTAGE_TYPE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_DC_PHASE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_DC_POWER_CABLE_LOAD_ID)->EnableWindow(FALSE);
		GetDlgItem(IDC_DC_MAIN_EX_STATIC)->EnableWindow(FALSE);

		m_DC_MainNamingPrefixEdit.EnableWindow(FALSE);
		m_DC_MainNamingSuffixEdit.EnableWindow(FALSE);
		m_DC_MainExStatic.EnableWindow(FALSE);
	}

	return ERROR_SUCCESS;
}
//}

/**
	@brief  CABLE NAMING RULE SETTING내용을 DISPLAY 한다.

	@author KHS
*/
int CPowerCableNamingRuleDlg::DisplayCableNamingRuleSetting(void)
{
	int nRow = 1;
	CCableNamingRuleSettingTable& CableNamingRuleSettingTable = CCableNamingRuleSettingTable::GetInstance();
	if(!CableNamingRuleSettingTable.m_RecordEntry.empty())
	{
		nRow = 1;
		for(vector<CCableCreationTable::Record>::iterator itr = CableNamingRuleSettingTable.m_RecordEntry.begin();itr != CableNamingRuleSettingTable.m_RecordEntry.end();++itr,++nRow)
		{
			
			if(1 == nRow)
			{
				//m_CableScheduleSpecEdit.SetWindowText(itr->FieldValueMap["C_SPACE_HEATER_CABLE_SCHEDULE_SPEC"].c_str());

				if("0" == itr->FieldValueMap["C_SINGLE_CORE_CABLE_NUMBERING"])		m_nSingleCore = 0;
				else if("1" == itr->FieldValueMap["C_SINGLE_CORE_CABLE_NUMBERING"])	m_nSingleCore = 1;
				else if("2" == itr->FieldValueMap["C_SINGLE_CORE_CABLE_NUMBERING"])	m_nSingleCore = 2;

				if("0" == itr->FieldValueMap["C_PARALLEL_CABLE_NUMBERING"])		m_nParallel = 0;
				else if("1" == itr->FieldValueMap["C_PARALLEL_CABLE_NUMBERING"])	m_nParallel = 1;

				UpdateData(FALSE);
			}
			
			if(1 ==nRow) break;
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief  CABLE NAMING RULE SETTING DATA를 저장한다.

	@author KHS
*/
int CPowerCableNamingRuleDlg::SaveCableNamingRuleSettingData(void)
{
	UpdateData(TRUE);
	CString rCableType, rCableCore, rCableVoltage, rCableScheduleSpec,rParallelCableNumberingRule,rSingleCoreCableNumbering;
	CString rCableSize;

	CCableNamingRuleSettingTable& CableNamingRuleSettingTable = CCableNamingRuleSettingTable::GetInstance();
	int i = 1;
	for(vector<CCableCreationTable::Record>::iterator itr = CableNamingRuleSettingTable.m_RecordEntry.begin();itr != CableNamingRuleSettingTable.m_RecordEntry.end();++itr,++i)
	{
		if(i == 1)
		{
			rParallelCableNumberingRule.Format("%d", m_nParallel);
			rSingleCoreCableNumbering.Format("%d", m_nSingleCore);
		}

		itr->FieldValueMap["C_PARALLEL_CABLE_NUMBERING"]         = rParallelCableNumberingRule.operator LPCSTR();
		itr->FieldValueMap["C_SINGLE_CORE_CABLE_NUMBERING"]      = rSingleCoreCableNumbering.operator LPCSTR();

		if(1 == i) break;
	}

	CableNamingRuleSettingTable.Update();

	return ERROR_SUCCESS;
}
/**
	@brief  Project Setting Table에서 CableDataTable 이름을 읽어와 CableDataTable을 로딩한다.

	@author	KHS
*/
int CPowerCableNamingRuleDlg::LoadCableDataTable(void)
{
	CCableDataListTable &CableDataListTable = CCableDataListTable::GetInstance();
	CableDataListTable.Load();

	CELoadDocData& docData = CELoadDocData::GetInstance();

	CString rProjectMDB = docData.GetProjectMDBFilePath().c_str();
	rProjectMDB.MakeUpper();
	if(FileExist(rProjectMDB))
	{
		CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
		string* pCableDataTableName = pSettingTable->GetFieldValue(_T("C_CABLEDATA_TABLE"));
		if(pCableDataTableName && !pCableDataTableName->empty())
		{
			//! Cable Data Table을 로딩한다.
			docData.m_ProjectCableDataTable.Load(*pCableDataTableName , (*pCableDataTableName) != docData.m_ProjectCableDataTable.GetTableName());
		}
		//! CABLE DATA COMBO BOX를 셋팅한다.
	}

	return ERROR_SUCCESS;
}

/**
	@brief	HV Cable Setting Data를 화면에 표시합니다.

	@author BHK	

	@date 2009-04-01 오전 10:34:31	

	@param	

	@return		
*/
int CPowerCableNamingRuleDlg::DisplayHVCableData(void)
{
	static const CString rLoadId = "PM-9500";
	CString rPrefix, rSuffix;

	int nRow = 0;
	CHVCableCreationSettingTable& HVVoltageSettingTable = CHVCableCreationSettingTable::GetInstance();
	if(!HVVoltageSettingTable.m_RecordEntry.empty())
	{
		nRow = 1;
		for(vector<CCableCreationTable::Record>::iterator itr = HVVoltageSettingTable.m_RecordEntry.begin();itr != HVVoltageSettingTable.m_RecordEntry.end();++itr,++nRow)
		{
			if(1 == nRow)
			{
				m_HV1_BusVoltageEdit.SetWindowText(itr->FieldValueMap["C_BUS_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_BUS_VOLTAGE"].c_str());
				m_HV1_LoadVoltageEdit.SetWindowText(itr->FieldValueMap["C_LOAD_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_LOAD_VOLTAGE"].c_str());
				m_HV1_MainNamingPrefixEdit.SetWindowText(itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_PREFIX"].empty() ? "" : itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_PREFIX"].c_str());
				m_HV1_MainNamingSuffixEdit.SetWindowText(itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_SUFFIX"].empty() ? "" : itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_SUFFIX"].c_str());
				m_HV1_SpaceHeaderNamingPrefixEdit.SetWindowText(itr->FieldValueMap["C_SPACE_HEATER_POWER_CABLE_NAMING_PREFIX"].empty() ? "" : itr->FieldValueMap["C_SPACE_HEATER_POWER_CABLE_NAMING_PREFIX"].c_str());
				m_HV1_SpaceHeaderNamingSuffixEdit.SetWindowText(itr->FieldValueMap["C_SPACE_HEATER_POWER_CABLE_NAMING_SUFFIX"].empty() ? "" : itr->FieldValueMap["C_SPACE_HEATER_POWER_CABLE_NAMING_SUFFIX"].c_str());
				m_HV1_MainNamingPrefixEdit.GetWindowText(rPrefix);
				m_HV1_MainNamingSuffixEdit.GetWindowText(rSuffix);
				m_HV1_MainExStatic.SetText("ex> " + rPrefix + rLoadId + rSuffix);
				m_HV1_SpaceHeaderNamingPrefixEdit.GetWindowText(rPrefix);
				m_HV1_SpaceHeaderNamingSuffixEdit.GetWindowText(rSuffix);
				m_HV1_SpaceHeaderExStatic.SetText("ex> " + rPrefix + rLoadId + rSuffix);
			}
			else if(2 == nRow)
			{
				m_HV2_BusVoltageEdit.SetWindowText(itr->FieldValueMap["C_BUS_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_BUS_VOLTAGE"].c_str());
				m_HV2_LoadVoltageEdit.SetWindowText(itr->FieldValueMap["C_LOAD_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_LOAD_VOLTAGE"].c_str());
				m_HV2_MainNamingPrefixEdit.SetWindowText(itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_PREFIX"].empty() ? "" : itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_PREFIX"].c_str());
				m_HV2_MainNamingSuffixEdit.SetWindowText(itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_SUFFIX"].empty() ? "" : itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_SUFFIX"].c_str());
				m_HV2_SpaceHeaderNamingPrefixEdit.SetWindowText(itr->FieldValueMap["C_SPACE_HEATER_POWER_CABLE_NAMING_PREFIX"].empty() ? "" : itr->FieldValueMap["C_SPACE_HEATER_POWER_CABLE_NAMING_PREFIX"].c_str());
				m_HV2_SpaceHeaderNamingSuffixEdit.SetWindowText(itr->FieldValueMap["C_SPACE_HEATER_POWER_CABLE_NAMING_SUFFIX"].empty() ? "" : itr->FieldValueMap["C_SPACE_HEATER_POWER_CABLE_NAMING_SUFFIX"].c_str());
				m_HV2_MainNamingPrefixEdit.GetWindowText(rPrefix);
				m_HV2_MainNamingSuffixEdit.GetWindowText(rSuffix);
				m_HV2_MainExStatic.SetText("ex> " + rPrefix + rLoadId + rSuffix);
				m_HV2_SpaceHeaderNamingPrefixEdit.GetWindowText(rPrefix);
				m_HV2_SpaceHeaderNamingSuffixEdit.GetWindowText(rSuffix);
				m_HV2_SpaceHeaderExStatic.SetText("ex> " + rPrefix + rLoadId + rSuffix);
			}
			else if(3 == nRow)
			{
				m_HV3_BusVoltageEdit.SetWindowText(itr->FieldValueMap["C_BUS_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_BUS_VOLTAGE"].c_str());
				m_HV3_LoadVoltageEdit.SetWindowText(itr->FieldValueMap["C_LOAD_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_LOAD_VOLTAGE"].c_str());
				m_HV3_MainNamingPrefixEdit.SetWindowText(itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_PREFIX"].empty() ? "" : itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_PREFIX"].c_str());
				m_HV3_MainNamingSuffixEdit.SetWindowText(itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_SUFFIX"].empty() ? "" : itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_SUFFIX"].c_str());
				m_HV3_SpaceHeaderNamingPrefixEdit.SetWindowText(itr->FieldValueMap["C_SPACE_HEATER_POWER_CABLE_NAMING_PREFIX"].empty() ? "" : itr->FieldValueMap["C_SPACE_HEATER_POWER_CABLE_NAMING_PREFIX"].c_str());
				m_HV3_SpaceHeaderNamingSuffixEdit.SetWindowText(itr->FieldValueMap["C_SPACE_HEATER_POWER_CABLE_NAMING_SUFFIX"].empty() ? "" : itr->FieldValueMap["C_SPACE_HEATER_POWER_CABLE_NAMING_SUFFIX"].c_str());
				m_HV3_MainNamingPrefixEdit.GetWindowText(rPrefix);
				m_HV3_MainNamingSuffixEdit.GetWindowText(rSuffix);
				m_HV3_MainExStatic.SetText("ex> " + rPrefix + rLoadId + rSuffix);
				m_HV3_SpaceHeaderNamingPrefixEdit.GetWindowText(rPrefix);
				m_HV3_SpaceHeaderNamingSuffixEdit.GetWindowText(rSuffix);
				m_HV3_SpaceHeaderExStatic.SetText("ex> " + rPrefix + rLoadId + rSuffix);
			}
			if(nRow == 3) break;
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	LV3PH Cable Setting Data를 화면에 표시합니다.

	@author BHK	

	@date 2009-04-01 오전 10:36:04	

	@param	

	@return		
*/
int CPowerCableNamingRuleDlg::DisplayLV3PHCableData(void)
{
	static const CString rLoadId = "PM-9500";
	CString rPrefix, rSuffix;

	int nRow = 0;
	CLV3PHCableCreationSettingTable& LV3PHVoltageSettingTable = CLV3PHCableCreationSettingTable::GetInstance();
	if(!LV3PHVoltageSettingTable.m_RecordEntry.empty())
	{
		nRow = 1;
		for(vector<CCableCreationTable::Record>::iterator itr = LV3PHVoltageSettingTable.m_RecordEntry.begin();itr != LV3PHVoltageSettingTable.m_RecordEntry.end();++itr,++nRow)
		{
			if(1 == nRow)
			{
				m_LV1_BusVoltageEdit.SetWindowText(itr->FieldValueMap["C_BUS_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_BUS_VOLTAGE"].c_str());
				m_LV1_LoadVoltageEdit.SetWindowText(itr->FieldValueMap["C_LOAD_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_LOAD_VOLTAGE"].c_str());
				m_LV1_MainNamingPrefixEdit.SetWindowText(itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_PREFIX"].empty() ? "" : itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_PREFIX"].c_str());
				m_LV1_MainNamingSuffixEdit.SetWindowText(itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_SUFFIX"].empty() ? "" : itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_SUFFIX"].c_str());
				m_LV1_SpaceHeaderNamingPrefixEdit.SetWindowText(itr->FieldValueMap["C_SPACE_HEATER_POWER_CABLE_NAMING_PREFIX"].empty() ? "" : itr->FieldValueMap["C_SPACE_HEATER_POWER_CABLE_NAMING_PREFIX"].c_str());
				m_LV1_SpaceHeaderNamingSuffixEdit.SetWindowText(itr->FieldValueMap["C_SPACE_HEATER_POWER_CABLE_NAMING_SUFFIX"].empty() ? "" : itr->FieldValueMap["C_SPACE_HEATER_POWER_CABLE_NAMING_SUFFIX"].c_str());
				m_LV1_MainNamingPrefixEdit.GetWindowText(rPrefix);
				m_LV1_MainNamingSuffixEdit.GetWindowText(rSuffix);
				m_LV1_MainExStatic.SetText("ex> " + rPrefix + rLoadId + rSuffix);
				m_LV1_SpaceHeaderNamingPrefixEdit.GetWindowText(rPrefix);
				m_LV1_SpaceHeaderNamingSuffixEdit.GetWindowText(rSuffix);
				m_LV1_SpaceHeaderExStatic.SetText("ex> " + rPrefix + rLoadId + rSuffix);
			}
			else if(2 == nRow)
			{
				m_LV2_BusVoltageEdit.SetWindowText(itr->FieldValueMap["C_BUS_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_BUS_VOLTAGE"].c_str());
				m_LV2_LoadVoltageEdit.SetWindowText(itr->FieldValueMap["C_LOAD_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_LOAD_VOLTAGE"].c_str());
				m_LV2_MainNamingPrefixEdit.SetWindowText(itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_PREFIX"].empty() ? "" : itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_PREFIX"].c_str());
				m_LV2_MainNamingSuffixEdit.SetWindowText(itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_SUFFIX"].empty() ? "" : itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_SUFFIX"].c_str());
				m_LV2_SpaceHeaderNamingPrefixEdit.SetWindowText(itr->FieldValueMap["C_SPACE_HEATER_POWER_CABLE_NAMING_PREFIX"].empty() ? "" : itr->FieldValueMap["C_SPACE_HEATER_POWER_CABLE_NAMING_PREFIX"].c_str());
				m_LV2_SpaceHeaderNamingSuffixEdit.SetWindowText(itr->FieldValueMap["C_SPACE_HEATER_POWER_CABLE_NAMING_SUFFIX"].empty() ? "" : itr->FieldValueMap["C_SPACE_HEATER_POWER_CABLE_NAMING_SUFFIX"].c_str());
				m_LV2_MainNamingPrefixEdit.GetWindowText(rPrefix);
				m_LV2_MainNamingSuffixEdit.GetWindowText(rSuffix);
				m_LV2_MainExStatic.SetText("ex> " + rPrefix + rLoadId + rSuffix);
				m_LV2_SpaceHeaderNamingPrefixEdit.GetWindowText(rPrefix);
				m_LV2_SpaceHeaderNamingSuffixEdit.GetWindowText(rSuffix);
				m_LV2_SpaceHeaderExStatic.SetText("ex> " + rPrefix + rLoadId + rSuffix);
			}
			if(2 == nRow) break;
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	LV1PH Cable Setting Data를 화면에 표시합니다.

	@author BHK	

	@date 2009-04-01 오전 10:37:20	

	@param	

	@return		
*/
int CPowerCableNamingRuleDlg::DisplayLV1PHCableData(void)
{
	static const CString rLoadId = "PM-9500";
	CString rPrefix, rSuffix;

	int nRow = 0;
	CLV1PHCableCreationSettingTable& LV1PHVoltageSettingTable = CLV1PHCableCreationSettingTable::GetInstance();

	if(!LV1PHVoltageSettingTable.m_RecordEntry.empty())
	{
		nRow = 1;
		for(vector<CCableCreationTable::Record>::iterator itr = LV1PHVoltageSettingTable.m_RecordEntry.begin();itr != LV1PHVoltageSettingTable.m_RecordEntry.end();++itr,++nRow)
		{
			if(1 == nRow)
			{
				m_LV11_BusVoltageEdit.SetWindowText(itr->FieldValueMap["C_BUS_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_BUS_VOLTAGE"].c_str());
				m_LV11_LoadVoltageEdit.SetWindowText(itr->FieldValueMap["C_LOAD_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_LOAD_VOLTAGE"].c_str());
				m_LV11_MainNamingPrefixEdit.SetWindowText(itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_PREFIX"].empty() ? "" : itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_PREFIX"].c_str());
				m_LV11_MainNamingSuffixEdit.SetWindowText(itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_SUFFIX"].empty() ? "" : itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_SUFFIX"].c_str());
				m_LV11_MainNamingPrefixEdit.GetWindowText(rPrefix);
				m_LV11_MainNamingSuffixEdit.GetWindowText(rSuffix);
				m_LV11_MainExStatic.SetText("ex> " + rPrefix + rLoadId + rSuffix);
			}
			else if(2 == nRow)
			{
				m_LV12_BusVoltageEdit.SetWindowText(itr->FieldValueMap["C_BUS_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_BUS_VOLTAGE"].c_str());
				m_LV12_LoadVoltageEdit.SetWindowText(itr->FieldValueMap["C_LOAD_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_LOAD_VOLTAGE"].c_str());
				m_LV12_MainNamingPrefixEdit.SetWindowText(itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_PREFIX"].empty() ? "" : itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_PREFIX"].c_str());
				m_LV12_MainNamingSuffixEdit.SetWindowText(itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_SUFFIX"].empty() ? "" : itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_SUFFIX"].c_str());
				m_LV12_MainNamingPrefixEdit.GetWindowText(rPrefix);
				m_LV12_MainNamingSuffixEdit.GetWindowText(rSuffix);
				m_LV12_MainExStatic.SetText("ex> " + rPrefix + rLoadId + rSuffix);
			}
			if(2 == nRow) break;
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	DC Cable Setting Data를 화면에 표시합니다.

	@author BHK	

	@date 2009-04-01 오전 10:38:36	

	@param	

	@return		
*/
int CPowerCableNamingRuleDlg::DisplayDCCableData(void)
{
	static const CString rLoadId = "PM-9500";
	CString rPrefix, rSuffix;

	int nRow = 0;
	CDCCableCreationSettingTable& DCVoltageSettingTable = CDCCableCreationSettingTable::GetInstance();
	if(!DCVoltageSettingTable.m_RecordEntry.empty())
	{
		nRow = 1;
		for(vector<CCableCreationTable::Record>::iterator itr = DCVoltageSettingTable.m_RecordEntry.begin();itr != DCVoltageSettingTable.m_RecordEntry.end();++itr,++nRow)
		{
			if(1 == nRow)
			{
				m_DC_BusVoltageEdit.SetWindowText(itr->FieldValueMap["C_BUS_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_BUS_VOLTAGE"].c_str());
				m_DC_LoadVoltageEdit.SetWindowText(itr->FieldValueMap["C_LOAD_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_LOAD_VOLTAGE"].c_str());
				m_DC_MainNamingPrefixEdit.SetWindowText(itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_PREFIX"].empty() ? "" : itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_PREFIX"].c_str());
				m_DC_MainNamingSuffixEdit.SetWindowText(itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_SUFFIX"].empty() ? "" : itr->FieldValueMap["C_MAIN_POWER_CABLE_NAMING_SUFFIX"].c_str());
				m_DC_MainNamingPrefixEdit.GetWindowText(rPrefix);
				m_DC_MainNamingSuffixEdit.GetWindowText(rSuffix);
				m_DC_MainExStatic.SetText("ex> " + rPrefix + rLoadId + rSuffix);
			}
			if(1 == nRow) break;
		}
	}

	return ERROR_SUCCESS;
}
