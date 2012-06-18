// PowerCableCreationSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "pyfromc.h"

#include <util/ado/ADODB.h>
#include <gui/GridCtrl/GridCellCheck.h>
#include <gui/GridCtrl/GridCellCombo.h>
#include <gui/GridCtrl/GridCellNumeric.h>

#include "ELoadDocData.h"
#include "GridCellEx.h"
#include "Tables/CableCreationSettingTable.h"
#include "PowerCableSizingOfFaultCurrentDlg.h"
#include "CableItem.h"
#include <util/ado/SQLFile.h>
#include "commands/CommandPowerCableCreation.h"
#include "CableSizingResultView.h"
#include "ToolShell.h"
#include "PowerCableCreationSettingDlg.h"
#include "commands\CommandAutoCalculateTotalDF.h"

#define	READ_ONLY_COLOR	RGB(220, 220, 220)
#define	GRID_TEXT_COLOR	RGB(0, 0, 0)

// CPowerCableCreationSettingDlg dialog

IMPLEMENT_DYNAMIC(CPowerCableCreationSettingDlg, CDialog)

CPowerCableCreationSettingDlg::CPowerCableCreationSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPowerCableCreationSettingDlg::IDD, pParent)
{
	m_bApplyByAmbientTempFactor = TRUE;
	///m_bApplyMinLVPowerCableSize = TRUE;
	///m_bApplyByFaultCurrentAtHVPowerCable = TRUE;
}

CPowerCableCreationSettingDlg::~CPowerCableCreationSettingDlg()
{
}

void CPowerCableCreationSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//! CABLE CREATION ITEM
	DDX_Control(pDX, IDC_CABLE_VOLTAGE_COMBO2, m_CableVoltageComboBox);
	DDX_Control(pDX, IDC_CABLE_TYPE_COMBO2, m_CableTypeComboBox);
	DDX_Control(pDX, IDC_CABLE_CORE_COMBO3, m_CableCoreComboBox);
	DDX_Control(pDX, IDC_CABLE_SIZE_COMBO2, m_CableSizeComboBox);
	DDX_Control(pDX, IDC_CABLE_SCHEDULE_SPEC_EDIT2, m_CableScheduleSpecEdit);

	DDX_Control(pDX , IDC_STATIC_DERATING_FACTOR , m_DeratingFactorGroupBox);
	DDX_Control(pDX , IDC_STATIC_CONDUIT_SIZING_SETTING , m_ConduitSizingSettingGroupBox);
	DDX_Control(pDX , IDC_STATIC_MIN_POWER_FACTOR , m_MinPowerFactorGroupBox);

	DDX_Control(pDX , IDC_COMBO_AG_TEMP , m_wndAGTempCombo);
	DDX_Control(pDX , IDC_COMBO_UG_TEMP , m_wndUGTempCombo);

	DDX_Check(pDX , IDC_CHECK_APPLY_BY_AMBIENT_TEMP_FACTOR , m_bApplyByAmbientTempFactor);
	///DDX_Check(pDX , IDC_CHECK_APPLY_MINIMUM_LV_POWER_CABLE_SIZE , m_bApplyMinLVPowerCableSize);
	///DDX_Check(pDX , IDC_CHECK_APPLY_BY_FAULT_CURRENT_AT_HV_POWER_CABLE , m_bApplyByFaultCurrentAtHVPowerCable);

	DDX_Control(pDX , IDC_BUTTON_CALC , m_wndCalcButton);

	DDX_Control(pDX , IDC_BUTTON_SETTING_SAVE , m_wndSettingSaveButton);
	DDX_Control(pDX , IDC_BUTTON_CLOSE , m_wndCloseButton);
	DDX_Control(pDX, IDC_SELECT_CABLE_DATA_COMBO, m_SelectCableDataTableComboBox);

	//! BUS VOLTAGE
	DDX_Control(pDX, IDC_HV1_BUS_VOLTAGE_EDIT, m_HV1_BusVoltageEdit);
	DDX_Control(pDX, IDC_HV2_BUS_VOLTAGE_EDIT, m_HV2_BusVoltageEdit);
	DDX_Control(pDX, IDC_HV3_BUS_VOLTAGE_EDIT, m_HV3_BusVoltageEdit);
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

	//! CABLE VOLTAGE
	DDX_Control(pDX, IDC_HV1_CABLE_VOLTAGE_COMBO, m_HV1_CableVoltageCombo);
	DDX_Control(pDX, IDC_HV2_CABLE_VOLTAGE_COMBO, m_HV2_CableVoltageCombo);
	DDX_Control(pDX, IDC_HV3_CABLE_VOLTAGE_COMBO, m_HV3_CableVoltageCombo);
	DDX_Control(pDX, IDC_LV1_CABLE_VOLTAGE_COMBO, m_LV1_CableVoltageCombo);
	DDX_Control(pDX, IDC_LV2_CABLE_VOLTAGE_COMBO, m_LV2_CableVoltageCombo);
	DDX_Control(pDX, IDC_LV11_CABLE_VOLTAGE_COMBO, m_LV11_CableVoltageCombo);
	DDX_Control(pDX, IDC_LV12_CABLE_VOLTAGE_COMBO, m_LV12_CableVoltageCombo);
	DDX_Control(pDX, IDC_DC_CABLE_VOLTAGE_COMBO, m_DC_CableVoltageCombo);

	//! CABLE TYPE
	DDX_Control(pDX, IDC_HV1_CABLE_TYPE_COMBO, m_HV1_CableTypeCombo);
	DDX_Control(pDX, IDC_HV2_CABLE_TYPE_COMBO, m_HV2_CableTypeCombo);
	DDX_Control(pDX, IDC_HV3_CABLE_TYPE_COMBO, m_HV3_CableTypeCombo);
	DDX_Control(pDX, IDC_LV1_CABLE_TYPE_COMBO, m_LV1_CableTypeCombo);
	DDX_Control(pDX, IDC_LV2_CABLE_TYPE_COMBO, m_LV2_CableTypeCombo);
	DDX_Control(pDX, IDC_LV11_CABLE_TYPE_COMBO, m_LV11_CableTypeCombo);
	DDX_Control(pDX, IDC_LV12_CABLE_TYPE_COMBO, m_LV12_CableTypeCombo);
	DDX_Control(pDX, IDC_DC_CABLE_TYPE_COMBO, m_DC_CableTypeCombo);

	//! CABLE SCHEDULE SPEC
	DDX_Control(pDX, IDC_HV1_CABLE_SCHEDULE_SPEC_EDIT, m_HV1_CableScheduleSpecEdit);
	DDX_Control(pDX, IDC_HV2_CABLE_SCHEDULE_SPEC_EDIT, m_HV2_CableScheduleSpecEdit);
	DDX_Control(pDX, IDC_HV3_CABLE_SCHEDULE_SPEC_EDIT, m_HV3_CableScheduleSpecEdit);
	DDX_Control(pDX, IDC_LV1_CABLE_SCHEDULE_SPEC_EDIT, m_LV1_CableScheduleSpecEdit);
	DDX_Control(pDX, IDC_LV2_CABLE_SCHEDULE_SPEC_EDIT, m_LV2_CableScheduleSpecEdit);
	DDX_Control(pDX, IDC_LV11_CABLE_SCHEDULE_SPEC_EDIT, m_LV11_CableScheduleSpecEdit);
	DDX_Control(pDX, IDC_LV12_CABLE_SCHEDULE_SPEC_EDIT, m_LV12_CableScheduleSpecEdit);
	DDX_Control(pDX, IDC_DC_CABLE_SCHEDULE_SPEC_EDIT, m_DC_CableScheduleSpecEdit);

	//! CABLE CORE
	DDX_Control(pDX, IDC_HV1_CABLE_CORE_COMBO, m_HV1_CableCoreCombo);
	DDX_Control(pDX, IDC_HV2_CABLE_CORE_COMBO, m_HV2_CableCoreCombo);
	DDX_Control(pDX, IDC_HV3_CABLE_CORE_COMBO, m_HV3_CableCoreCombo);
	DDX_Control(pDX, IDC_LV1_CABLE_CORE_COMBO, m_LV1_CableCoreCombo);
	DDX_Control(pDX, IDC_LV2_CABLE_CORE_COMBO, m_LV2_CableCoreCombo);
	DDX_Control(pDX, IDC_LV11_CABLE_CORE_COMBO, m_LV11_CableCoreCombo);
	DDX_Control(pDX, IDC_LV12_CABLE_CORE_COMBO, m_LV12_CableCoreCombo);
	DDX_Control(pDX, IDC_DC_CABLE_CORE_COMBO, m_DC_CableCoreCombo);

	//! MOTOR RUNNING
	DDX_Control(pDX, IDC_HV1_MOTOR_RUNNING_EDIT, m_HV1_MotorRunningEdit);
	DDX_Control(pDX, IDC_HV2_MOTOR_RUNNING_EDIT, m_HV2_MotorRunningEdit);
	DDX_Control(pDX, IDC_HV3_MOTOR_RUNNING_EDIT, m_HV3_MotorRunningEdit);
	DDX_Control(pDX, IDC_LV1_MOTOR_RUNNING_EDIT, m_LV1_MotorRunningEdit);
	DDX_Control(pDX, IDC_LV2_MOTOR_RUNNING_EDIT, m_LV2_MotorRunningEdit);

	//! MOTOR STARTING
	DDX_Control(pDX, IDC_HV1_MOTOR_STARTING_EDIT, m_HV1_MotorStartingEdit);
	DDX_Control(pDX, IDC_HV2_MOTOR_STARTING_EDIT, m_HV2_MotorStartingEdit);
	DDX_Control(pDX, IDC_HV3_MOTOR_STARTING_EDIT, m_HV3_MotorStartingEdit);
	DDX_Control(pDX, IDC_LV1_MOTOR_STARTING_EDIT, m_LV1_MotorStartingEdit);
	DDX_Control(pDX, IDC_LV2_MOTOR_STARTING_EDIT, m_LV2_MotorStartingEdit);

	//! APPLY MOTOR STARTING
	DDX_Control(pDX, IDC_HV1_APPLY_MOTOR_STARTING_CHECK, m_HV1_ApplyMotorCheck);
	DDX_Control(pDX, IDC_HV2_APPLY_MOTOR_STARTING_CHECK, m_HV2_ApplyMotorCheck);
	DDX_Control(pDX, IDC_HV3_APPLY_MOTOR_STARTING_CHECK, m_HV3_ApplyMotorCheck);
	DDX_Control(pDX, IDC_LV1_APPLY_MOTOR_STARTING_CHECK, m_LV1_ApplyMotorCheck);
	DDX_Control(pDX, IDC_LV2_APPLY_MOTOR_STARTING_CHECK, m_LV2_ApplyMotorCheck);

	//! FEEFER RUNNING
	DDX_Control(pDX, IDC_HV1_FEEDER_RUNNING_EDIT, m_HV1_FeederRunningEdit);
	DDX_Control(pDX, IDC_HV2_FEEDER_RUNNING_EDIT, m_HV2_FeederRunningEdit);
	DDX_Control(pDX, IDC_HV3_FEEDER_RUNNING_EDIT, m_HV3_FeederRunningEdit);
	DDX_Control(pDX, IDC_LV1_FEEDER_RUNNING_EDIT, m_LV1_FeederRunningEdit);
	DDX_Control(pDX, IDC_LV2_FEEDER_RUNNING_EDIT, m_LV2_FeederRunningEdit);
	DDX_Control(pDX, IDC_LV11_FEEDER_RUNNING_EDIT, m_LV11_FeederRunningEdit);
	DDX_Control(pDX, IDC_LV12_FEEDER_RUNNING_EDIT, m_LV12_FeederRunningEdit);
	DDX_Control(pDX, IDC_DC_FEEDER_RUNNING_EDIT, m_DC_FeederRunningEdit);

	//! INSTALL METHOD
	DDX_Control(pDX, IDC_HV1_INSTALL_METHOD_COMBO, m_HV1_InstallMethodCombo);
	DDX_Control(pDX, IDC_HV2_INSTALL_METHOD_COMBO, m_HV2_InstallMethodCombo);
	DDX_Control(pDX, IDC_HV3_INSTALL_METHOD_COMBO, m_HV3_InstallMethodCombo);
	DDX_Control(pDX, IDC_LV1_INSTALL_METHOD_COMBO, m_LV1_InstallMethodCombo);
	DDX_Control(pDX, IDC_LV2_INSTALL_METHOD_COMBO, m_LV2_InstallMethodCombo);
	DDX_Control(pDX, IDC_LV11_INSTALL_METHOD_COMBO, m_LV11_InstallMethodCombo);
	DDX_Control(pDX, IDC_LV12_INSTALL_METHOD_COMBO, m_LV12_InstallMethodCombo);
	DDX_Control(pDX, IDC_DC_INSTALL_METHOD_COMBO, m_DC_InstallMethodCombo);

	//! AMBIENT TEMP DERATING FACTOR
	DDX_Control(pDX, IDC_HV1_AMBIENT_TEMP_EDIT, m_HV1_AmbientTempEdit);
	DDX_Control(pDX, IDC_HV2_AMBIENT_TEMP_EDIT, m_HV2_AmbientTempEdit);
	DDX_Control(pDX, IDC_HV3_AMBIENT_TEMP_EDIT, m_HV3_AmbientTempEdit);
	DDX_Control(pDX, IDC_LV1_AMBIENT_TEMP_EDIT, m_LV1_AmbientTempEdit);
	DDX_Control(pDX, IDC_LV2_AMBIENT_TEMP_EDIT, m_LV2_AmbientTempEdit);
	DDX_Control(pDX, IDC_LV11_AMBIENT_TEMP_EDIT, m_LV11_AmbientTempEdit);
	DDX_Control(pDX, IDC_LV12_AMBIENT_TEMP_EDIT, m_LV12_AmbientTempEdit);
	DDX_Control(pDX, IDC_DC_AMBIENT_TEMP_EDIT, m_DC_AmbientTempEdit);

	//!OTHER DERATING FACTOR
	DDX_Control(pDX, IDC_HV1_OTHER_EDIT, m_HV1_OtherDeratingFactorEdit);
	DDX_Control(pDX, IDC_HV2_OTHER_EDIT, m_HV2_OtherDeratingFactorEdit);
	DDX_Control(pDX, IDC_HV3_OTHER_EDIT, m_HV3_OtherDeratingFactorEdit);
	DDX_Control(pDX, IDC_LV1_OTHER_EDIT, m_LV1_OtherDeratingFactorEdit);
	DDX_Control(pDX, IDC_LV2_OTHER_EDIT, m_LV2_OtherDeratingFactorEdit);
	DDX_Control(pDX, IDC_LV11_OTHER_EDIT, m_LV11_OtherDeratingFactorEdit);
	DDX_Control(pDX, IDC_LV12_OTHER_EDIT, m_LV12_OtherDeratingFactorEdit);
	DDX_Control(pDX, IDC_DC_OTHER_EDIT, m_DC_OtherDeratingFactorEdit);

	//! TOTAL DERATING FACTOR
	DDX_Control(pDX, IDC_HV1_TOTAL_EDIT, m_HV1_TotalDeratingFactorEdit);
	DDX_Control(pDX, IDC_HV2_TOTAL_EDIT, m_HV2_TotalDeratingFactorEdit);
	DDX_Control(pDX, IDC_HV3_TOTAL_EDIT, m_HV3_TotalDeratingFactorEdit);
	DDX_Control(pDX, IDC_LV1_TOTAL_EDIT, m_LV1_TotalDeratingFactorEdit);
	DDX_Control(pDX, IDC_LV2_TOTAL_EDIT, m_LV2_TotalDeratingFactorEdit);
	DDX_Control(pDX, IDC_LV11_TOTAL_EDIT, m_LV11_TotalDeratingFactorEdit);
	DDX_Control(pDX, IDC_LV12_TOTAL_EDIT, m_LV12_TotalDeratingFactorEdit);
	DDX_Control(pDX, IDC_DC_TOTAL_EDIT, m_DC_TotalDeratingFactorEdit);

	//! SF
	DDX_Control(pDX, IDC_HV1_SF_EDIT, m_HV1_SfEdit);
	DDX_Control(pDX, IDC_HV2_SF_EDIT, m_HV2_SfEdit);
	DDX_Control(pDX, IDC_HV3_SF_EDIT, m_HV3_SfEdit);
	DDX_Control(pDX, IDC_LV1_SF_EDIT, m_LV1_SfEdit);
	DDX_Control(pDX, IDC_LV2_SF_EDIT, m_LV2_SfEdit);
	DDX_Control(pDX, IDC_LV11_SF_EDIT, m_LV11_SfEdit);
	DDX_Control(pDX, IDC_LV12_SF_EDIT, m_LV12_SfEdit);
	DDX_Control(pDX, IDC_DC_SF_EDIT, m_DC_SfEdit);

	//! TITLE
	DDX_Control(pDX, IDC_VOLTAGE_SETTING_TITLE_STATIC, m_VoltageSettingTitleStatic);
	DDX_Control(pDX, IDC_CABLE_SETTING_TITLE_STATIC, m_CableSettingTitleStatic);
	DDX_Control(pDX, IDC_VOLTAGE_DROP_TITLE_STATIC, m_VoltageDropTitleStatic);
	DDX_Control(pDX, IDC_DERATING_FACTOR_TITLE_STATIC, m_DeratingFactorTitleStatic);

	//!Voltage Type
	DDX_Control(pDX, IDC_HV1_VOLTAGE_TYPE_STATIC, m_HV1_VoltageTypeStatic);
	DDX_Control(pDX, IDC_HV2_VOLTAGE_TYPE_STATIC, m_HV2_VoltageTypeStatic);
	DDX_Control(pDX, IDC_HV3_VOLTAGE_TYPE_STATIC, m_HV3_VoltageTypeStatic);
	DDX_Control(pDX, IDC_LV1_VOLTAGE_TYPE_STATIC, m_LV1_VoltageTypeStatic);
	DDX_Control(pDX, IDC_LV2_VOLTAGE_TYPE_STATIC, m_LV2_VoltageTypeStatic);
	DDX_Control(pDX, IDC_LV11_VOLTAGE_TYPE_STATIC, m_LV11_VoltageTypeStatic);
	DDX_Control(pDX, IDC_LV12_VOLTAGE_TYPE_STATIC, m_LV12_VoltageTypeStatic);
	DDX_Control(pDX, IDC_DC_VOLTAGE_TYPE_STATIC, m_DC_VoltageTypeStatic);

}


BEGIN_MESSAGE_MAP(CPowerCableCreationSettingDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CALC, &CPowerCableCreationSettingDlg::OnBnClickedButtonCalc)
	ON_BN_CLICKED(IDC_BUTTON_SETTING_SAVE, &CPowerCableCreationSettingDlg::OnBnClickedButtonSettingSave)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CPowerCableCreationSettingDlg::OnBnClickedButtonClose)
	ON_CBN_SELCHANGE(IDC_SELECT_CABLE_DATA_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelchangeSelectCableDataCombo)
	ON_BN_CLICKED(IDC_CHECK_APPLY_BY_AMBIENT_TEMP_FACTOR, &CPowerCableCreationSettingDlg::OnBnClickedCheckApplyByAmbientTempFactor)
	ON_CBN_SELCHANGE(IDC_HV1_CABLE_VOLTAGE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelchangeHv1CableVoltageCombo)
	ON_CBN_SELCHANGE(IDC_HV2_CABLE_VOLTAGE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelchangeHv2CableVoltageCombo)
	ON_CBN_SELCHANGE(IDC_HV3_CABLE_VOLTAGE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelchangeHv3CableVoltageCombo)
	ON_CBN_SELCHANGE(IDC_LV1_CABLE_VOLTAGE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelchangeLv1CableVoltageCombo)
	ON_CBN_SELCHANGE(IDC_LV2_CABLE_VOLTAGE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelchangeLv2CableVoltageCombo)
	ON_CBN_SELCHANGE(IDC_LV11_CABLE_VOLTAGE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelchangeLv11CableVoltageCombo)
	ON_CBN_SELCHANGE(IDC_LV12_CABLE_VOLTAGE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelchangeLv12CableVoltageCombo)
	ON_CBN_SELCHANGE(IDC_DC_CABLE_VOLTAGE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelchangeDcCableVoltageCombo)
	ON_CBN_SELCHANGE(IDC_HV1_CABLE_TYPE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelchangeHv1CableTypeCombo)
	ON_CBN_SELCHANGE(IDC_HV2_CABLE_TYPE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelchangeHv2CableTypeCombo)
	ON_CBN_SELCHANGE(IDC_HV3_CABLE_TYPE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelchangeHv3CableTypeCombo)
	ON_CBN_SELCHANGE(IDC_LV1_CABLE_TYPE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelchangeLv1CableTypeCombo)
	ON_CBN_SELCHANGE(IDC_LV2_CABLE_TYPE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelchangeLv2CableTypeCombo)
	ON_CBN_SELCHANGE(IDC_LV11_CABLE_TYPE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelchangeLv11CableTypeCombo)
	ON_CBN_SELCHANGE(IDC_LV12_CABLE_TYPE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelchangeLv12CableTypeCombo)
	ON_CBN_SELCHANGE(IDC_DC_CABLE_TYPE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelchangeDcCableTypeCombo)
	ON_CBN_SELENDOK(IDC_HV1_INSTALL_METHOD_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokHv1InstallMethodCombo)
	ON_CBN_SELENDOK(IDC_HV2_INSTALL_METHOD_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokHv2InstallMethodCombo)
	ON_CBN_SELENDOK(IDC_HV3_INSTALL_METHOD_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokHv3InstallMethodCombo)
	ON_CBN_SELENDOK(IDC_LV1_INSTALL_METHOD_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokLv1InstallMethodCombo)
	ON_CBN_SELENDOK(IDC_LV2_INSTALL_METHOD_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokLv2InstallMethodCombo)
	ON_CBN_SELENDOK(IDC_LV11_INSTALL_METHOD_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokLv11InstallMethodCombo)
	ON_CBN_SELENDOK(IDC_LV12_INSTALL_METHOD_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokLv12InstallMethodCombo)
	ON_CBN_SELENDOK(IDC_DC_INSTALL_METHOD_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokDcInstallMethodCombo)
	ON_CBN_SELENDOK(IDC_HV1_CABLE_VOLTAGE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokHv1CableVoltageCombo)
	ON_CBN_SELENDOK(IDC_HV2_CABLE_VOLTAGE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokHv2CableVoltageCombo)
	ON_CBN_SELENDOK(IDC_HV3_CABLE_VOLTAGE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokHv3CableVoltageCombo)
	ON_CBN_SELENDOK(IDC_LV1_CABLE_VOLTAGE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokLv1CableVoltageCombo)
	ON_CBN_SELENDOK(IDC_LV2_CABLE_VOLTAGE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokLv2CableVoltageCombo)
	ON_CBN_SELENDOK(IDC_LV11_CABLE_VOLTAGE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokLv11CableVoltageCombo)
	ON_CBN_SELENDOK(IDC_LV12_CABLE_VOLTAGE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokLv12CableVoltageCombo)
	ON_CBN_SELENDOK(IDC_DC_CABLE_VOLTAGE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokDcCableVoltageCombo)
	ON_CBN_SELENDOK(IDC_HV1_CABLE_TYPE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokHv1CableTypeCombo)
	ON_CBN_SELENDOK(IDC_HV2_CABLE_TYPE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokHv2CableTypeCombo)
	ON_CBN_SELENDOK(IDC_HV3_CABLE_TYPE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokHv3CableTypeCombo)
	ON_CBN_SELENDOK(IDC_LV1_CABLE_TYPE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokLv1CableTypeCombo)
	ON_CBN_SELENDOK(IDC_LV2_CABLE_TYPE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokLv2CableTypeCombo)
	ON_CBN_SELENDOK(IDC_LV11_CABLE_TYPE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokLv11CableTypeCombo)
	ON_CBN_SELENDOK(IDC_LV12_CABLE_TYPE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokLv12CableTypeCombo)
	ON_CBN_SELENDOK(IDC_DC_CABLE_TYPE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokDcCableTypeCombo)
	ON_CBN_SELENDOK(IDC_HV1_CABLE_CORE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokHv1CableCoreCombo)
	ON_CBN_SELENDOK(IDC_HV2_CABLE_CORE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokHv2CableCoreCombo)
	ON_CBN_SELENDOK(IDC_HV3_CABLE_CORE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokHv3CableCoreCombo)
	ON_CBN_SELENDOK(IDC_LV1_CABLE_CORE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokLv1CableCoreCombo)
	ON_CBN_SELENDOK(IDC_LV2_CABLE_CORE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokLv2CableCoreCombo)
	ON_CBN_SELENDOK(IDC_LV11_CABLE_CORE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokLv11CableCoreCombo)
	ON_CBN_SELENDOK(IDC_LV12_CABLE_CORE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokLv12CableCoreCombo)
	ON_CBN_SELENDOK(IDC_DC_CABLE_CORE_COMBO, &CPowerCableCreationSettingDlg::OnCbnSelendokDcCableCoreCombo)
	ON_EN_CHANGE(IDC_HV1_AMBIENT_TEMP_EDIT, &CPowerCableCreationSettingDlg::OnEnChangeHv1AmbientTempEdit)
	ON_EN_CHANGE(IDC_HV2_AMBIENT_TEMP_EDIT, &CPowerCableCreationSettingDlg::OnEnChangeHv2AmbientTempEdit)
	ON_EN_CHANGE(IDC_HV3_AMBIENT_TEMP_EDIT, &CPowerCableCreationSettingDlg::OnEnChangeHv3AmbientTempEdit)
	ON_EN_CHANGE(IDC_LV1_AMBIENT_TEMP_EDIT, &CPowerCableCreationSettingDlg::OnEnChangeLv1AmbientTempEdit)
	ON_EN_CHANGE(IDC_LV2_AMBIENT_TEMP_EDIT, &CPowerCableCreationSettingDlg::OnEnChangeLv2AmbientTempEdit)
	ON_EN_CHANGE(IDC_LV11_AMBIENT_TEMP_EDIT, &CPowerCableCreationSettingDlg::OnEnChangeLv11AmbientTempEdit)
	ON_EN_CHANGE(IDC_LV12_AMBIENT_TEMP_EDIT, &CPowerCableCreationSettingDlg::OnEnChangeLv12AmbientTempEdit)
	ON_EN_CHANGE(IDC_DC_AMBIENT_TEMP_EDIT, &CPowerCableCreationSettingDlg::OnEnChangeDcAmbientTempEdit)
	ON_EN_CHANGE(IDC_HV1_OTHER_EDIT, &CPowerCableCreationSettingDlg::OnEnChangeHv1OtherEdit)
	ON_EN_CHANGE(IDC_HV2_OTHER_EDIT, &CPowerCableCreationSettingDlg::OnEnChangeHv2OtherEdit)
	ON_EN_CHANGE(IDC_HV3_OTHER_EDIT, &CPowerCableCreationSettingDlg::OnEnChangeHv3OtherEdit)
	ON_EN_CHANGE(IDC_LV1_OTHER_EDIT, &CPowerCableCreationSettingDlg::OnEnChangeLv1OtherEdit)
	ON_EN_CHANGE(IDC_LV2_OTHER_EDIT, &CPowerCableCreationSettingDlg::OnEnChangeLv2OtherEdit)
	ON_EN_CHANGE(IDC_LV11_OTHER_EDIT, &CPowerCableCreationSettingDlg::OnEnChangeLv11OtherEdit)
	ON_EN_CHANGE(IDC_LV12_OTHER_EDIT, &CPowerCableCreationSettingDlg::OnEnChangeLv12OtherEdit)
	ON_EN_CHANGE(IDC_DC_OTHER_EDIT, &CPowerCableCreationSettingDlg::OnEnChangeDcOtherEdit)
	ON_BN_CLICKED(IDC_CHECK_APPLY_MINIMUM_LV_POWER_CABLE_SIZE, &CPowerCableCreationSettingDlg::OnBnClickedCheckApplyMinimumLvPowerCableSize)
	ON_BN_CLICKED(IDC_CHECK_APPLY_BY_FAULT_CURRENT_AT_HV_POWER_CABLE, &CPowerCableCreationSettingDlg::OnBnClickedCheckApplyByFaultCurrentAtHvPowerCable)
	ON_CBN_SELCHANGE(IDC_COMBO_AG_TEMP, &CPowerCableCreationSettingDlg::OnCbnSelchangeComboAgTemp)
	ON_CBN_SELCHANGE(IDC_COMBO_UG_TEMP, &CPowerCableCreationSettingDlg::OnCbnSelchangeComboUgTemp)
	ON_CBN_SELCHANGE(IDC_CABLE_VOLTAGE_COMBO2, &CPowerCableCreationSettingDlg::OnCbnSelchangeCableVoltageCombo2)
	ON_CBN_SELCHANGE(IDC_CABLE_TYPE_COMBO2, &CPowerCableCreationSettingDlg::OnCbnSelchangeCableTypeCombo2)
	ON_CBN_SELCHANGE(IDC_CABLE_CORE_COMBO3, &CPowerCableCreationSettingDlg::OnCbnSelchangeCableCoreCombo3)
	ON_CBN_SELCHANGE(IDC_CABLE_SIZE_COMBO2, &CPowerCableCreationSettingDlg::OnCbnSelchangeCableSizeCombo2)
END_MESSAGE_MAP()


// CPowerCableCreationSettingDlg message handlers
/**
	@brief	

	@author BHK	

	@date 2009-03-27 오후 10:47:00	

	@param	

	@return		
**/
BOOL CPowerCableCreationSettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	BeginWaitCursor();

	InitEditBox();

	//!TITLE TEXT
	m_VoltageSettingTitleStatic.SetBkColor(DarkenColor(GetSysColor(COLOR_BTNFACE),0.15) , GetSysColor(COLOR_BTNFACE) , CLabelStatic::HorzGradient);
	m_CableSettingTitleStatic.SetBkColor(DarkenColor(GetSysColor(COLOR_BTNFACE),0.15) , GetSysColor(COLOR_BTNFACE) , CLabelStatic::HorzGradient);	
	m_VoltageDropTitleStatic.SetBkColor(DarkenColor(GetSysColor(COLOR_BTNFACE),0.15) , GetSysColor(COLOR_BTNFACE) , CLabelStatic::HorzGradient);	
	m_DeratingFactorTitleStatic.SetBkColor(DarkenColor(GetSysColor(COLOR_BTNFACE),0.15) , GetSysColor(COLOR_BTNFACE) , CLabelStatic::HorzGradient);	
	
	const COLORREF GroupBoxTextColor = RGB(0,70,213);
	m_DeratingFactorGroupBox.SetTextColor(GroupBoxTextColor, FALSE);
	m_ConduitSizingSettingGroupBox.SetTextColor(GroupBoxTextColor, FALSE);
	m_MinPowerFactorGroupBox.SetTextColor(GroupBoxTextColor, FALSE);
	m_PowerCableCreationSettingGroupBox.SetTextColor(GroupBoxTextColor, FALSE);
	m_StaticPowerCableCreatinSetting.SetTextColor(GroupBoxTextColor, FALSE);

	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_CONDUIT_TYPE);
	if(pComboBox)
	{
		CConduitListTable& ConduitListTable = CConduitListTable::GetInstance();
		ConduitListTable.Load();
		const int nRecordCount = ConduitListTable.GetRecordCount();
		for(int i = 0;i < nRecordCount;++i)
		{
			CConduitListTable::TABLE_RECORD* pRecord = ConduitListTable.GetRecordAt(i);
			if(pRecord) pComboBox->AddString(pRecord->rTableNo.c_str());
		}
	}

	if(ERROR_SUCCESS != LoadCableDataTable())
	{
		return FALSE;
	}

	LoadData();
	DisplayData();
	
	//! 아이템의  BusVoltage가 없으면 READ ONLY로 셋팅한다.
	CheckItemAndReadOnly();

	CELoadDocData& docData = CELoadDocData::GetInstance();
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	m_wndCalcButton.SetTextHotColor(TextHotColor);
	m_wndSettingSaveButton.SetTextHotColor(TextHotColor);
	m_wndCloseButton.SetTextHotColor(TextHotColor);
	
	HICON hCancelIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_CANCEL_18X18),
		IMAGE_ICON,
		18,18, 0 );
	m_wndCloseButton.SetIcon(hCancelIcon);

	hCancelIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_OK_18X18),
		IMAGE_ICON,
		18,18, 0 );
	m_wndSettingSaveButton.SetIcon(hCancelIcon);

	EndWaitCursor();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


/**
	@brief  SETTING 내용을 화면에 표시한다.
	
	@author BHK
*/
int CPowerCableCreationSettingDlg::DisplayData(void)
{
	//! INSTALL METHOD COMBO 에 미리 아이템을 넣어준다.
	static const CString InstallMethods[] = {_T("U/G"), _T("A/G")};
	for( int i = 0;i < sizeof(InstallMethods)/ sizeof(InstallMethods[0]);++i)
	{
		m_HV1_InstallMethodCombo.AddString(InstallMethods[i]);
		m_HV2_InstallMethodCombo.AddString(InstallMethods[i]);
		m_HV3_InstallMethodCombo.AddString(InstallMethods[i]);
		m_LV1_InstallMethodCombo.AddString(InstallMethods[i]);
		m_LV2_InstallMethodCombo.AddString(InstallMethods[i]);
		m_LV11_InstallMethodCombo.AddString(InstallMethods[i]);
		m_LV12_InstallMethodCombo.AddString(InstallMethods[i]);
		m_DC_InstallMethodCombo.AddString(InstallMethods[i]);
	}

	DisplayHVCableData();
	DisplayLV3PHCableData();
	DisplayLV1PHCableData();
	DisplayDCCableData();
	
	CELoadDocData& docData = CELoadDocData::GetInstance();
	//! DISPLAY CABLE CREATION SETTING DATA 
	CPowerCableCreationSettingTable* pPowerCableSettingTable = static_cast<CPowerCableCreationSettingTable*>(docData.GetTableOf(CPowerCableCreationSettingTable::GetTableName()));
	if(pPowerCableSettingTable)
	{
		int nRow = 1;
		for(vector<CCableCreationTable::Record>::iterator itr = pPowerCableSettingTable->m_RecordEntry.begin();itr != pPowerCableSettingTable->m_RecordEntry.end();++itr,++nRow)
		{
			if(1 == nRow)
			{
				m_bApplyByAmbientTempFactor = ("Yes" == itr->FieldValueMap["C_AMBIENT_TEMP_FACTOR"] ? TRUE : FALSE);

				CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_APPLY_MINIMUM_LV_POWER_CABLE_SIZE);
				("Yes" == itr->FieldValueMap["C_MIN_LV_CHECK"] ? pButton->SetCheck(TRUE) : pButton->SetCheck(FALSE));

				pButton = (CButton*)GetDlgItem(IDC_CHECK_APPLY_BY_FAULT_CURRENT_AT_HV_POWER_CABLE);
				("Yes" == itr->FieldValueMap["C_FAULT_CURRENT_AT_HV_CHECK"] ? pButton->SetCheck(TRUE) : pButton->SetCheck(FALSE));

				CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_CONDUIT_LENGTH);
				pEdit->SetWindowText(itr->FieldValueMap["C_CONDUIT_LENGTH"].empty() ? _T("") : itr->FieldValueMap["C_CONDUIT_LENGTH"].c_str());

				pEdit = (CEdit*)GetDlgItem(IDC_EDIT_POWER_CABLE_SIZING_OF_FAULT_CURRENT);
				pEdit->SetWindowText(itr->FieldValueMap["C_FAULT_CURRENT_AT_HV"].empty() ? _T("") : itr->FieldValueMap["C_FAULT_CURRENT_AT_HV"].c_str());

				pEdit = (CEdit*)GetDlgItem(IDC_EDIT_NUMBER_OF_CONDUCTORS_1);
				pEdit->SetWindowText(itr->FieldValueMap["C_NUMBER_OF_CONDUCTORS_1"].empty() ? _T("53") : itr->FieldValueMap["C_NUMBER_OF_CONDUCTORS_1"].c_str());

				pEdit = (CEdit*)GetDlgItem(IDC_EDIT_NUMBER_OF_CONDUCTORS_2);
				pEdit->SetWindowText(itr->FieldValueMap["C_NUMBER_OF_CONDUCTORS_2"].empty() ? _T("31") : itr->FieldValueMap["C_NUMBER_OF_CONDUCTORS_2"].c_str());

				pEdit = (CEdit*)GetDlgItem(IDC_EDIT_NUMBER_OF_CONDUCTORS_OVER_2);
				pEdit->SetWindowText(itr->FieldValueMap["C_NUMBER_OF_CONDUCTORS_OVER2"].empty() ? _T("40") : itr->FieldValueMap["C_NUMBER_OF_CONDUCTORS_OVER2"].c_str());

				SetDlgItemText(IDC_EDIT_RATING_CAPACITY_ABOVE , itr->FieldValueMap[_T("C_RATING_CAPACITY_ABOVE")].c_str());
				m_CableScheduleSpecEdit.SetWindowText(itr->FieldValueMap["C_SPACE_HEATER_CABLE_SCHEDULE_SPEC"].c_str());
			}
			//! CABLE SETTING COMBO BOX
			map<string,string>::iterator where = pPowerCableSettingTable->m_RecordEntry.front().FieldValueMap.find(_T("C_SPACE_HEATER_CABLE_VOLTAGE"));
			if(where != pPowerCableSettingTable->m_RecordEntry.front().FieldValueMap.end())
			{
				const int nFound = m_CableVoltageComboBox.FindString(-1 , where->second.c_str());
				if(-1 != nFound)
				{
					m_CableVoltageComboBox.SetCurSel(nFound);
					OnCbnSelchangeCableVoltageCombo2();
				}
			}
			else	OnCbnSelchangeCableVoltageCombo2();

			//!그룹 체크 박스의 VISIBLE 설정
			CButton* pApplyMinLvPowerCableSizeCheckButton = (CButton*)GetDlgItem(IDC_CHECK_APPLY_MINIMUM_LV_POWER_CABLE_SIZE);
			pApplyMinLvPowerCableSizeCheckButton->SetCheck(TRUE);
			if(FALSE == pApplyMinLvPowerCableSizeCheckButton->GetCheck())
			{
				CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_APPLY_MIN_LV_POWER_CABLE_SIZE);
				pComboBox->EnableWindow(FALSE);
			}
			else
			{
				CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_APPLY_MIN_LV_POWER_CABLE_SIZE);
				pComboBox->EnableWindow(TRUE);
			}

			CButton* pApplyByFaultCurrentAtHvPowerCableCheckButton = (CButton*)GetDlgItem(IDC_CHECK_APPLY_BY_FAULT_CURRENT_AT_HV_POWER_CABLE);
			pApplyByFaultCurrentAtHvPowerCableCheckButton->SetCheck(TRUE);
			if(FALSE == pApplyByFaultCurrentAtHvPowerCableCheckButton->GetCheck())
			{
				CButton* pButton = (CButton*)GetDlgItem(IDC_BUTTON_CALC);
				pButton->EnableWindow(FALSE);
			}
			else
			{
				CButton* pButton = (CButton*)GetDlgItem(IDC_BUTTON_CALC);
				pButton->EnableWindow(TRUE);
			}
		}
	}
	UpdateData(FALSE);

	UpdateComboContents();

	//! COMBO BOX를 채운다.
	for(int i = 0;i <= 60;i+=5)
	{
		CString rString;
		rString.Format("%d" , i);
		m_wndAGTempCombo.AddString(rString);
		m_wndUGTempCombo.AddString(rString);
	}
	m_wndAGTempCombo.SetCurSel(0);
	m_wndUGTempCombo.SetCurSel(0);

	FillApplyMinLVPowerCableSizeComboBox();

	//!COMBOBOX
	DisplayCableCreationSettingData();

	UpdateData(FALSE);
	
	return ERROR_SUCCESS;
}

/**
	@brief  DB 데이타를 읽어온다.
	
	@author BHK
*/
int CPowerCableCreationSettingDlg::LoadData(void)
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
	@brief	Power Cable Sizing

	@author	BHK
*/
void CPowerCableCreationSettingDlg::OnBnClickedButtonCalc()
{
	CPowerCableSizingOfFaultCurrentDlg dlg;
	if(IDOK == dlg.DoModal())
	{
		SetDlgItemText(IDC_EDIT_POWER_CABLE_SIZING_OF_FAULT_CURRENT , CPowerCableSizingOfFaultCurrentDlg::__POWER_CABLE_SIZING__);
	}
}

/**
	@brief	설정한 값들을 저장한다.

	@author	BHK,KHS
*/
void CPowerCableCreationSettingDlg::OnBnClickedButtonSettingSave()
{
	if(ERROR_SUCCESS == SaveData())
		AfxMessageBox("SUCCESS, SAVE!!");
}

void CPowerCableCreationSettingDlg::OnBnClickedButtonClose()
{
	OnCancel();
}


/**
	@brief	설정한 값들을 저장한다.

	@author KHS
*/
int CPowerCableCreationSettingDlg::SaveData(void)
{
	CString rVoltageType, rBusVoltage, rLoadVoltage, rCableVoltage, rCableType, rCableCore, rCableScheduleSpec, rMotorRunning, 
		rMotorStarting, rApplyMotorStarting , rFeederRunning, rInstallMethod, rAmbientTemp, rOtherDeratingFactor, rTotalDeratingFactor,rSF, rApplySpaceHeader;

	CHVCableCreationSettingTable& HVVoltageSettingTable = CHVCableCreationSettingTable::GetInstance();
	int i = 1;
	for(vector<CCableCreationTable::Record>::iterator itr = HVVoltageSettingTable.m_RecordEntry.begin();itr != HVVoltageSettingTable.m_RecordEntry.end();++itr,++i)
	{
		if(i == 1)
		{
			m_HV1_VoltageTypeStatic.GetWindowText(rVoltageType);
			m_HV1_BusVoltageEdit.GetWindowText(rBusVoltage);
			m_HV1_LoadVoltageEdit.GetWindowText(rLoadVoltage);
			m_HV1_CableVoltageCombo.GetWindowText(rCableVoltage);
			m_HV1_CableTypeCombo.GetWindowText(rCableType);
			m_HV1_CableScheduleSpecEdit.GetWindowText(rCableScheduleSpec);
			m_HV1_CableCoreCombo.GetWindowText(rCableCore);
			m_HV1_MotorRunningEdit.GetWindowText(rMotorRunning);
			m_HV1_MotorStartingEdit.GetWindowText(rMotorStarting);
			
			CButton* pButton = (CButton*)GetDlgItem(IDC_HV1_APPLY_MOTOR_STARTING_CHECK);
			rApplyMotorStarting = (TRUE == pButton->GetCheck()) ? _T("Yes") : _T("No");

			m_HV1_FeederRunningEdit.GetWindowText(rFeederRunning);
			m_HV1_InstallMethodCombo.GetWindowText(rInstallMethod);
			m_HV1_AmbientTempEdit.GetWindowText(rAmbientTemp);
			m_HV1_OtherDeratingFactorEdit.GetWindowText(rOtherDeratingFactor);
			m_HV1_TotalDeratingFactorEdit.GetWindowText(rTotalDeratingFactor);
			m_HV1_SfEdit.GetWindowText(rSF);


		}
		else if(i == 2)
		{
			m_HV2_VoltageTypeStatic.GetWindowText(rVoltageType);
			m_HV2_BusVoltageEdit.GetWindowText(rBusVoltage);
			m_HV2_LoadVoltageEdit.GetWindowText(rLoadVoltage);
			m_HV2_CableVoltageCombo.GetWindowText(rCableVoltage);
			m_HV2_CableTypeCombo.GetWindowText(rCableType);
			m_HV2_CableScheduleSpecEdit.GetWindowText(rCableScheduleSpec);
			m_HV2_CableCoreCombo.GetWindowText(rCableCore);
			m_HV2_MotorRunningEdit.GetWindowText(rMotorRunning);
			m_HV2_MotorStartingEdit.GetWindowText(rMotorStarting);

			CButton* pButton = (CButton*)GetDlgItem(IDC_HV2_APPLY_MOTOR_STARTING_CHECK);
			rApplyMotorStarting = (TRUE == pButton->GetCheck()) ? _T("Yes") : _T("No");

			m_HV2_FeederRunningEdit.GetWindowText(rFeederRunning);
			m_HV2_InstallMethodCombo.GetWindowText(rInstallMethod);
			m_HV2_AmbientTempEdit.GetWindowText(rAmbientTemp);
			m_HV2_OtherDeratingFactorEdit.GetWindowText(rOtherDeratingFactor);
			m_HV2_TotalDeratingFactorEdit.GetWindowText(rTotalDeratingFactor);
			m_HV2_SfEdit.GetWindowText(rSF);
		}
		else if(i == 3)
		{
			m_HV3_VoltageTypeStatic.GetWindowText(rVoltageType);
			m_HV3_BusVoltageEdit.GetWindowText(rBusVoltage);
			m_HV3_LoadVoltageEdit.GetWindowText(rLoadVoltage);
			m_HV3_CableVoltageCombo.GetWindowText(rCableVoltage);
			m_HV3_CableTypeCombo.GetWindowText(rCableType);
			m_HV3_CableScheduleSpecEdit.GetWindowText(rCableScheduleSpec);
			m_HV3_CableCoreCombo.GetWindowText(rCableCore);
			m_HV3_MotorRunningEdit.GetWindowText(rMotorRunning);
			m_HV3_MotorStartingEdit.GetWindowText(rMotorStarting);
			
			CButton* pButton = (CButton*)GetDlgItem(IDC_HV3_APPLY_MOTOR_STARTING_CHECK);
			rApplyMotorStarting = (TRUE == pButton->GetCheck()) ? _T("Yes") : _T("No");

			m_HV3_FeederRunningEdit.GetWindowText(rFeederRunning);
			m_HV3_InstallMethodCombo.GetWindowText(rInstallMethod);
			m_HV3_AmbientTempEdit.GetWindowText(rAmbientTemp);
			m_HV3_OtherDeratingFactorEdit.GetWindowText(rOtherDeratingFactor);
			m_HV3_TotalDeratingFactorEdit.GetWindowText(rTotalDeratingFactor);
			m_HV3_SfEdit.GetWindowText(rSF);
		}

		itr->FieldValueMap["C_VOLTAGE_TYPE"]			= rVoltageType.operator LPCSTR();
		itr->FieldValueMap["C_BUS_VOLTAGE"]			= rBusVoltage.operator LPCSTR();
		itr->FieldValueMap["C_LOAD_VOLTAGE"]			= rLoadVoltage.operator LPCSTR();
		itr->FieldValueMap["C_CABLE_VOLTAGE"]			= rCableVoltage.operator LPCSTR();
		itr->FieldValueMap["C_CABLE_TYPE"]			= rCableType.operator LPCSTR();
		itr->FieldValueMap["C_CABLE_SCHEDULE_SPEC"]		= rCableScheduleSpec.operator LPCSTR();
		itr->FieldValueMap["C_CABLE_CORE"]			= rCableCore.operator LPCSTR();
		itr->FieldValueMap["C_MOTOR_RUNNING"]			= rMotorRunning.operator LPCSTR();
		itr->FieldValueMap["C_MOTOR_STARTING"]			= rMotorStarting.operator LPCSTR();
		itr->FieldValueMap[_T("C_APPLY_MOTOR_STARTING")]	= rApplyMotorStarting.operator LPCSTR();
		itr->FieldValueMap["C_FEEDER_RUNNING"]			= rFeederRunning.operator LPCSTR();
		itr->FieldValueMap["C_INSTALL_METHOD"]			= rInstallMethod.operator LPCSTR();
		itr->FieldValueMap["C_AMBIENT_TEMP_DERATING_FACTOR"]	= rAmbientTemp.operator LPCSTR();
		itr->FieldValueMap["C_OTHERS_DERATING_FACTOR"]		= rOtherDeratingFactor.operator LPCSTR();
		itr->FieldValueMap["C_TOTAL_DERATING_FACTOR"]		= rTotalDeratingFactor.operator LPCSTR();
		itr->FieldValueMap["C_SF"]				= rSF.operator LPCSTR();
		
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
			m_LV1_CableVoltageCombo.GetWindowText(rCableVoltage);
			m_LV1_CableTypeCombo.GetWindowText(rCableType);
			m_LV1_CableScheduleSpecEdit.GetWindowText(rCableScheduleSpec);
			m_LV1_CableCoreCombo.GetWindowText(rCableCore);
			m_LV1_MotorRunningEdit.GetWindowText(rMotorRunning);
			m_LV1_MotorStartingEdit.GetWindowText(rMotorStarting);

			CButton* pButton = (CButton*)GetDlgItem(IDC_LV1_APPLY_MOTOR_STARTING_CHECK);
			rApplyMotorStarting = (TRUE == pButton->GetCheck()) ? _T("Yes") : _T("No");

			m_LV1_FeederRunningEdit.GetWindowText(rFeederRunning);
			m_LV1_InstallMethodCombo.GetWindowText(rInstallMethod);
			m_LV1_AmbientTempEdit.GetWindowText(rAmbientTemp);
			m_LV1_OtherDeratingFactorEdit.GetWindowText(rOtherDeratingFactor);
			m_LV1_TotalDeratingFactorEdit.GetWindowText(rTotalDeratingFactor);
			m_LV1_SfEdit.GetWindowText(rSF);
		}
		else if(i == 2)
		{
			m_LV2_VoltageTypeStatic.GetWindowText(rVoltageType);
			m_LV2_BusVoltageEdit.GetWindowText(rBusVoltage);
			m_LV2_LoadVoltageEdit.GetWindowText(rLoadVoltage);
			m_LV2_CableVoltageCombo.GetWindowText(rCableVoltage);
			m_LV2_CableTypeCombo.GetWindowText(rCableType);
			m_LV2_CableScheduleSpecEdit.GetWindowText(rCableScheduleSpec);
			m_LV2_CableCoreCombo.GetWindowText(rCableCore);
			m_LV2_MotorRunningEdit.GetWindowText(rMotorRunning);
			m_LV2_MotorStartingEdit.GetWindowText(rMotorStarting);

			CButton* pButton = (CButton*)GetDlgItem(IDC_LV2_APPLY_MOTOR_STARTING_CHECK);
			rApplyMotorStarting = (TRUE == pButton->GetCheck()) ? _T("Yes") : _T("No");

			m_LV2_FeederRunningEdit.GetWindowText(rFeederRunning);
			m_LV2_InstallMethodCombo.GetWindowText(rInstallMethod);
			m_LV2_AmbientTempEdit.GetWindowText(rAmbientTemp);
			m_LV2_OtherDeratingFactorEdit.GetWindowText(rOtherDeratingFactor);
			m_LV2_TotalDeratingFactorEdit.GetWindowText(rTotalDeratingFactor);
			m_LV2_SfEdit.GetWindowText(rSF);
		}

		itr->FieldValueMap["C_VOLTAGE_TYPE"]			= rVoltageType.operator LPCSTR();
		itr->FieldValueMap["C_BUS_VOLTAGE"]			= rBusVoltage.operator LPCSTR();
		itr->FieldValueMap["C_LOAD_VOLTAGE"]			= rLoadVoltage.operator LPCSTR();
		itr->FieldValueMap["C_CABLE_VOLTAGE"]			= rCableVoltage.operator LPCSTR();
		itr->FieldValueMap["C_CABLE_TYPE"]			= rCableType.operator LPCSTR();
		itr->FieldValueMap["C_CABLE_SCHEDULE_SPEC"]		= rCableScheduleSpec.operator LPCSTR();
		itr->FieldValueMap["C_CABLE_CORE"]			= rCableCore.operator LPCSTR();
		itr->FieldValueMap["C_MOTOR_RUNNING"]			= rMotorRunning.operator LPCSTR();
		itr->FieldValueMap["C_MOTOR_STARTING"]			= rMotorStarting.operator LPCSTR();
		itr->FieldValueMap[_T("C_APPLY_MOTOR_STARTING")]	= rApplyMotorStarting.operator LPCSTR();
		itr->FieldValueMap["C_FEEDER_RUNNING"]			= rFeederRunning.operator LPCSTR();
		itr->FieldValueMap["C_INSTALL_METHOD"]			= rInstallMethod.operator LPCSTR();
		itr->FieldValueMap["C_AMBIENT_TEMP_DERATING_FACTOR"]	= rAmbientTemp.operator LPCSTR();
		itr->FieldValueMap["C_OTHERS_DERATING_FACTOR"]		= rOtherDeratingFactor.operator LPCSTR();
		itr->FieldValueMap["C_TOTAL_DERATING_FACTOR"]		= rTotalDeratingFactor.operator LPCSTR();
		itr->FieldValueMap["C_SF"]				= rSF.operator LPCSTR();
		
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
			m_LV11_CableVoltageCombo.GetWindowText(rCableVoltage);
			m_LV11_CableTypeCombo.GetWindowText(rCableType);
			m_LV11_CableScheduleSpecEdit.GetWindowText(rCableScheduleSpec);
			m_LV11_CableCoreCombo.GetWindowText(rCableCore);
			m_LV11_FeederRunningEdit.GetWindowText(rFeederRunning);
			m_LV11_InstallMethodCombo.GetWindowText(rInstallMethod);
			m_LV11_AmbientTempEdit.GetWindowText(rAmbientTemp);
			m_LV11_OtherDeratingFactorEdit.GetWindowText(rOtherDeratingFactor);
			m_LV11_TotalDeratingFactorEdit.GetWindowText(rTotalDeratingFactor);
			m_LV11_SfEdit.GetWindowText(rSF);
		}
		else if(i == 2)
		{
			m_LV12_VoltageTypeStatic.GetWindowText(rVoltageType);
			m_LV12_BusVoltageEdit.GetWindowText(rBusVoltage);
			m_LV12_LoadVoltageEdit.GetWindowText(rLoadVoltage);
			m_LV12_CableVoltageCombo.GetWindowText(rCableVoltage);
			m_LV12_CableTypeCombo.GetWindowText(rCableType);
			m_LV12_CableScheduleSpecEdit.GetWindowText(rCableScheduleSpec);
			m_LV12_CableCoreCombo.GetWindowText(rCableCore);
			m_LV12_FeederRunningEdit.GetWindowText(rFeederRunning);
			m_LV12_InstallMethodCombo.GetWindowText(rInstallMethod);
			m_LV12_AmbientTempEdit.GetWindowText(rAmbientTemp);
			m_LV12_OtherDeratingFactorEdit.GetWindowText(rOtherDeratingFactor);
			m_LV12_TotalDeratingFactorEdit.GetWindowText(rTotalDeratingFactor);
			m_LV12_SfEdit.GetWindowText(rSF);
		}

		itr->FieldValueMap["C_VOLTAGE_TYPE"]			= rVoltageType.operator LPCSTR();
		itr->FieldValueMap["C_BUS_VOLTAGE"]			= rBusVoltage.operator LPCSTR();
		itr->FieldValueMap["C_LOAD_VOLTAGE"]			= rLoadVoltage.operator LPCSTR();
		itr->FieldValueMap["C_CABLE_VOLTAGE"]			= rCableVoltage.operator LPCSTR();
		itr->FieldValueMap["C_CABLE_TYPE"]			= rCableType.operator LPCSTR();
		itr->FieldValueMap["C_CABLE_SCHEDULE_SPEC"]		= rCableScheduleSpec.operator LPCSTR();
		itr->FieldValueMap["C_CABLE_CORE"]			= rCableCore.operator LPCSTR();
		itr->FieldValueMap["C_FEEDER_RUNNING"]			= rFeederRunning.operator LPCSTR();
		itr->FieldValueMap["C_INSTALL_METHOD"]			= rInstallMethod.operator LPCSTR();
		itr->FieldValueMap["C_AMBIENT_TEMP_DERATING_FACTOR"]	= rAmbientTemp.operator LPCSTR();
		itr->FieldValueMap["C_OTHERS_DERATING_FACTOR"]		= rOtherDeratingFactor.operator LPCSTR();
		itr->FieldValueMap["C_TOTAL_DERATING_FACTOR"]		= rTotalDeratingFactor.operator LPCSTR();
		itr->FieldValueMap["C_SF"]				= rSF.operator LPCSTR();

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
			m_DC_CableVoltageCombo.GetWindowText(rCableVoltage);
			m_DC_CableTypeCombo.GetWindowText(rCableType);
			m_DC_CableScheduleSpecEdit.GetWindowText(rCableScheduleSpec);
			m_DC_CableCoreCombo.GetWindowText(rCableCore);
			m_DC_FeederRunningEdit.GetWindowText(rFeederRunning);
			m_DC_InstallMethodCombo.GetWindowText(rInstallMethod);
			m_DC_AmbientTempEdit.GetWindowText(rAmbientTemp);
			m_DC_OtherDeratingFactorEdit.GetWindowText(rOtherDeratingFactor);
			m_DC_TotalDeratingFactorEdit.GetWindowText(rTotalDeratingFactor);
			m_DC_SfEdit.GetWindowText(rSF);
		}

		itr->FieldValueMap["C_VOLTAGE_TYPE"]			= rVoltageType.operator LPCSTR();
		itr->FieldValueMap["C_BUS_VOLTAGE"]			= rBusVoltage.operator LPCSTR();
		itr->FieldValueMap["C_LOAD_VOLTAGE"]			= rLoadVoltage.operator LPCSTR();
		itr->FieldValueMap["C_CABLE_VOLTAGE"]			= rCableVoltage.operator LPCSTR();
		itr->FieldValueMap["C_CABLE_TYPE"]			= rCableType.operator LPCSTR();
		itr->FieldValueMap["C_CABLE_SCHEDULE_SPEC"]		= rCableScheduleSpec.operator LPCSTR();
		itr->FieldValueMap["C_CABLE_CORE"]			= rCableCore.operator LPCSTR();
		itr->FieldValueMap["C_FEEDER_RUNNING"]			= rFeederRunning.operator LPCSTR();
		itr->FieldValueMap["C_INSTALL_METHOD"]			= rInstallMethod.operator LPCSTR();
		itr->FieldValueMap["C_AMBIENT_TEMP_DERATING_FACTOR"]	= rAmbientTemp.operator LPCSTR();
		itr->FieldValueMap["C_OTHERS_DERATING_FACTOR"]		= rOtherDeratingFactor.operator LPCSTR();
		itr->FieldValueMap["C_TOTAL_DERATING_FACTOR"]		= rTotalDeratingFactor.operator LPCSTR();
		itr->FieldValueMap["C_SF"]				= rSF.operator LPCSTR();

		if(1 == i) break;
	}
	DCVoltageSettingTable.Update();

	SavePowerCableCreationSettingSettingData();
	//!
	UpdateLoadItems();

	SaveSelectCabledataTable();

	return ERROR_SUCCESS;
}

/**
	@brief  Project Setting Table에서 CableDataTable 이름을 읽어와 CableDataTable을 로딩한다.

	@author	KHS
*/
int CPowerCableCreationSettingDlg::LoadCableDataTable(void)
{
	CCableDataListTable &CableDataListTable = CCableDataListTable::GetInstance();
	CableDataListTable.Load();

	const size_t nRecordCount = CableDataListTable.GetRecordCount();
	for(int i = 0;i < (int)nRecordCount;++i)
	{
		TABLES::CCableDataListTable::TABLE_RECORD* pRecord = CableDataListTable.GetRecordAt(i);
		if(pRecord)
		{
			m_SelectCableDataTableComboBox.AddString(pRecord->rTableNo.c_str());
		}
	}

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
			CString rTableName;
			int nItem = m_SelectCableDataTableComboBox.FindString(-1 , pCableDataTableName->c_str());
			if(-1 != nItem)
			{
				m_SelectCableDataTableComboBox.SetCurSel(nItem);
				//! Cable Data Table을 로딩한다.
				docData.m_ProjectCableDataTable.Load(*pCableDataTableName , (*pCableDataTableName) != docData.m_ProjectCableDataTable.GetTableName());
			}
		}

		FillComboBoxWithCableSettingData();
	}

	return ERROR_SUCCESS;
}


/**
	@brief	사용자가 Cable Data를 선택했을때... 다시 Cable Data를 로딩한다.

	@author	BHK
*/
void CPowerCableCreationSettingDlg::OnCbnSelchangeSelectCableDataCombo()
{
	BeginWaitCursor();
	const int nSel = m_SelectCableDataTableComboBox.GetCurSel();
	if(-1 != nSel)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();

		CString rString;
		m_SelectCableDataTableComboBox.GetLBText(nSel , rString);
		if(rString != CString(docData.m_ProjectCableDataTable.GetTableName().c_str()))
		{
			//! 현재 Cable Data Table과 다른 Table을 선택했을 때 Cable Data를 다시 로딩한다.
			if(ERROR_SUCCESS == docData.m_ProjectCableDataTable.Load(rString.operator LPCSTR() , rString != CString(docData.m_ProjectCableDataTable.GetTableName().c_str())))
			{
			}
			FillComboBoxWithCableSettingData();
			UpdateComboContents();
		}
	}
	EndWaitCursor();
}

/**
	@brief	Cable Data Table에서 콤보 박스에 CABLE VOLTAGE항목을 채운다.

	@author	BHK
*/
int CPowerCableCreationSettingDlg::FillComboBoxWithCableSettingData(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	m_CableVoltage.RemoveAll();
	m_CableTypeList.clear();

	docData.m_ProjectCableDataTable.GetCableTypeKeyList(m_CableTypeList);
	for(vector<string>::iterator jtr = m_CableTypeList.begin(); jtr != m_CableTypeList.end();++jtr)
	{	
		CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(*jtr);
		if(pCableDataByType)
		{
			vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
			if(pRecordList)
			{
				for(vector<CCableDataByType::Record*>::iterator ktr = pRecordList->begin();ktr != pRecordList->end();++ktr)
				{
					if(TRUE == docData.CompareCStringArray(m_CableVoltage, (*ktr)->m_FieldMap[_T("C_VOLT")].c_str()))
					{
						m_CableVoltage.Add((*ktr)->m_FieldMap[_T("C_VOLT")].c_str());
						m_HV1_CableVoltageCombo.AddString((*ktr)->m_FieldMap[_T("C_VOLT")].c_str());
						m_HV2_CableVoltageCombo.AddString((*ktr)->m_FieldMap[_T("C_VOLT")].c_str());
						m_HV3_CableVoltageCombo.AddString((*ktr)->m_FieldMap[_T("C_VOLT")].c_str());
						m_LV1_CableVoltageCombo.AddString((*ktr)->m_FieldMap[_T("C_VOLT")].c_str());
						m_LV2_CableVoltageCombo.AddString((*ktr)->m_FieldMap[_T("C_VOLT")].c_str());
						m_LV11_CableVoltageCombo.AddString((*ktr)->m_FieldMap[_T("C_VOLT")].c_str());
						m_LV12_CableVoltageCombo.AddString((*ktr)->m_FieldMap[_T("C_VOLT")].c_str());
						m_DC_CableVoltageCombo.AddString((*ktr)->m_FieldMap[_T("C_VOLT")].c_str());
						
						m_CableVoltageComboBox.AddString((*ktr)->m_FieldMap[_T("C_VOLT")].c_str());
					}
				}
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	콤보 박스에 새로운 값으로 채운다.

	@author	BHK
*/
int CPowerCableCreationSettingDlg::UpdateComboContents(void)
{

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
BOOL CPowerCableCreationSettingDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	return CDialog::OnNotify(wParam, lParam, pResult);
}

/**
	@brief	AMBIENT TEMP. DERATIONG FACTOR를 계산합니다.

	@param	nRow 계산을 수행할 줄

	@author	BHK
*/
int CPowerCableCreationSettingDlg::CalcAmbientTempDeratingFactor(const int& nRow)
{
	if(FALSE == m_bApplyByAmbientTempFactor) return ERROR_SUCCESS;

	CString rCableVolt , rCableType , rCableCore , rInstallMethod;
	if(0 == nRow)
	{
		int nCurSel = m_HV1_CableVoltageCombo.GetCurSel();
		if(-1 != nCurSel) m_HV1_CableVoltageCombo.GetLBText(nCurSel , rCableVolt);
		
		nCurSel = m_HV1_CableTypeCombo.GetCurSel();
		if(-1 != nCurSel) m_HV1_CableTypeCombo.GetLBText(nCurSel , rCableType);

		nCurSel = m_HV1_CableCoreCombo.GetCurSel();
		if(-1 != nCurSel) m_HV1_CableCoreCombo.GetLBText(nCurSel , rCableCore);

		nCurSel = m_HV1_InstallMethodCombo.GetCurSel();
		if(-1 != nCurSel) m_HV1_InstallMethodCombo.GetLBText(nCurSel , rInstallMethod);
	}
	else if(1 == nRow)
	{
		int nCurSel = m_HV2_CableVoltageCombo.GetCurSel();
		if(-1 != nCurSel) m_HV2_CableVoltageCombo.GetLBText(nCurSel , rCableVolt);
		
		nCurSel = m_HV2_CableTypeCombo.GetCurSel();
		if(-1 != nCurSel) m_HV2_CableTypeCombo.GetLBText(nCurSel , rCableType);

		nCurSel = m_HV2_CableCoreCombo.GetCurSel();
		if(-1 != nCurSel) m_HV2_CableCoreCombo.GetLBText(nCurSel , rCableCore);

		nCurSel = m_HV2_InstallMethodCombo.GetCurSel();
		if(-1 != nCurSel) m_HV2_InstallMethodCombo.GetLBText(nCurSel , rInstallMethod);
	}
	else if(2 == nRow)
	{
		int nCurSel = m_HV3_CableVoltageCombo.GetCurSel();
		if(-1 != nCurSel) m_HV3_CableVoltageCombo.GetLBText(nCurSel , rCableVolt);
		
		nCurSel = m_HV3_CableTypeCombo.GetCurSel();
		if(-1 != nCurSel) m_HV3_CableTypeCombo.GetLBText(nCurSel , rCableType);

		nCurSel = m_HV3_CableCoreCombo.GetCurSel();
		if(-1 != nCurSel) m_HV3_CableCoreCombo.GetLBText(nCurSel , rCableCore);

		nCurSel = m_HV3_InstallMethodCombo.GetCurSel();
		if(-1 != nCurSel) m_HV3_InstallMethodCombo.GetLBText(nCurSel , rInstallMethod);
	}
	else if(3 == nRow)
	{
		int nCurSel = m_LV1_CableVoltageCombo.GetCurSel();
		if(-1 != nCurSel) m_LV1_CableVoltageCombo.GetLBText(nCurSel , rCableVolt);
		
		nCurSel = m_LV1_CableTypeCombo.GetCurSel();
		if(-1 != nCurSel) m_LV1_CableTypeCombo.GetLBText(nCurSel , rCableType);

		nCurSel = m_LV1_CableCoreCombo.GetCurSel();
		if(-1 != nCurSel) m_LV1_CableCoreCombo.GetLBText(nCurSel , rCableCore);

		nCurSel = m_LV1_InstallMethodCombo.GetCurSel();
		if(-1 != nCurSel) m_LV1_InstallMethodCombo.GetLBText(nCurSel , rInstallMethod);
	}
	else if(4 == nRow)
	{
		int nCurSel = m_LV2_CableVoltageCombo.GetCurSel();
		if(-1 != nCurSel) m_LV2_CableVoltageCombo.GetLBText(nCurSel , rCableVolt);
		
		nCurSel = m_LV2_CableTypeCombo.GetCurSel();
		if(-1 != nCurSel) m_LV2_CableTypeCombo.GetLBText(nCurSel , rCableType);

		nCurSel = m_LV2_CableCoreCombo.GetCurSel();
		if(-1 != nCurSel) m_LV2_CableCoreCombo.GetLBText(nCurSel , rCableCore);

		nCurSel = m_LV2_InstallMethodCombo.GetCurSel();
		if(-1 != nCurSel) m_LV2_InstallMethodCombo.GetLBText(nCurSel , rInstallMethod);
	}
	else if(5 == nRow)
	{
		int nCurSel = m_LV11_CableVoltageCombo.GetCurSel();
		if(-1 != nCurSel) m_LV11_CableVoltageCombo.GetLBText(nCurSel , rCableVolt);
		
		nCurSel = m_LV11_CableTypeCombo.GetCurSel();
		if(-1 != nCurSel) m_LV11_CableTypeCombo.GetLBText(nCurSel , rCableType);

		nCurSel = m_LV11_CableCoreCombo.GetCurSel();
		if(-1 != nCurSel) m_LV11_CableCoreCombo.GetLBText(nCurSel , rCableCore);

		nCurSel = m_LV11_InstallMethodCombo.GetCurSel();
		if(-1 != nCurSel) m_LV11_InstallMethodCombo.GetLBText(nCurSel , rInstallMethod);
	}
	else if(6 == nRow)
	{
		int nCurSel = m_LV12_CableVoltageCombo.GetCurSel();
		if(-1 != nCurSel) m_LV12_CableVoltageCombo.GetLBText(nCurSel , rCableVolt);
		
		nCurSel = m_LV12_CableTypeCombo.GetCurSel();
		if(-1 != nCurSel) m_LV12_CableTypeCombo.GetLBText(nCurSel , rCableType);

		nCurSel = m_LV12_CableCoreCombo.GetCurSel();
		if(-1 != nCurSel) m_LV12_CableCoreCombo.GetLBText(nCurSel , rCableCore);

		nCurSel = m_LV12_InstallMethodCombo.GetCurSel();
		if(-1 != nCurSel) m_LV12_InstallMethodCombo.GetLBText(nCurSel , rInstallMethod);
	}
	else if(7 == nRow)
	{
		int nCurSel = m_DC_CableVoltageCombo.GetCurSel();
		if(-1 != nCurSel) m_DC_CableVoltageCombo.GetLBText(nCurSel , rCableVolt);
		
		nCurSel = m_DC_CableTypeCombo.GetCurSel();
		if(-1 != nCurSel) m_DC_CableTypeCombo.GetLBText(nCurSel , rCableType);

		nCurSel = m_DC_CableCoreCombo.GetCurSel();
		if(-1 != nCurSel) m_DC_CableCoreCombo.GetLBText(nCurSel , rCableCore);

		nCurSel = m_DC_InstallMethodCombo.GetCurSel();
		if(-1 != nCurSel) m_DC_InstallMethodCombo.GetLBText(nCurSel , rInstallMethod);
	}
		
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string rType = CString(rCableVolt + "KV " + rCableCore + " " + CString("CU") + " " + rCableType + " (" + CString("eLoad") + ")").operator LPCSTR();

	CCableDataByType* pCableDataType = docData.m_ProjectCableDataTable.GetRecordListOf(rType);
	if(pCableDataType)
	{
		vector<CCableDataByType::Record*>* pRecordList = pCableDataType->GetRecordEntryOfHertz();
		if(pRecordList && !pRecordList->empty())
		{
			const double nUGBaseTemp  = atof(pRecordList->front()->m_FieldMap[_T("C_UG_BASE_TEMP")].c_str());
			const double nAGBaseTemp  = atof(pRecordList->front()->m_FieldMap[_T("C_AG_BASE_TEMP")].c_str());
			const double nMaxCondTemp = atof(pRecordList->front()->m_FieldMap[_T("C_MAX_CONDUCTOR_TEMP")].c_str());
			double nTemperature = -1 , nBaseTemperature = -1;
			if(_T("A/G") == rInstallMethod)
			{
				const int nSel = m_wndAGTempCombo.GetCurSel();
				if(-1 != nSel)
				{
					CString rString;
					m_wndAGTempCombo.GetLBText(nSel , rString);
					nTemperature = atof(rString);
					nBaseTemperature = nAGBaseTemp;
				}
			}
			else if(_T("U/G") == rInstallMethod)
			{
				const int nSel = m_wndUGTempCombo.GetCurSel();
				if(-1 != nSel)
				{
					CString rString;
					m_wndUGTempCombo.GetLBText(nSel , rString);
					nTemperature = atof(rString);
					nBaseTemperature = nUGBaseTemp;
				}
			}

			if(-1 != nTemperature)
			{
				stringstream oss;
				oss.precision(2);
				oss.setf(ios_base:: fixed, ios_base:: floatfield); // 소수점 방식으로 표시됨
				const double dAmbientTemp = SAFE_ROUND(sqrt((nMaxCondTemp - nTemperature) / (nMaxCondTemp - nBaseTemperature)) , 2);
				oss << dAmbientTemp;
				if(0 == nRow)
				{
					m_HV1_AmbientTempEdit.SetWindowText(oss.str().c_str());
				}
				else if(1 == nRow)
				{
					m_HV2_AmbientTempEdit.SetWindowText(oss.str().c_str());
				}
				else if(2 == nRow)
				{
					m_HV3_AmbientTempEdit.SetWindowText(oss.str().c_str());
				}
				else if(3 == nRow)
				{
					m_LV1_AmbientTempEdit.SetWindowText(oss.str().c_str());
				}
				else if(4 == nRow)
				{
					m_LV2_AmbientTempEdit.SetWindowText(oss.str().c_str());
				}
				else if(5 == nRow)
				{
					m_LV11_AmbientTempEdit.SetWindowText(oss.str().c_str());
				}
				else if(6 == nRow)
				{
					m_LV12_AmbientTempEdit.SetWindowText(oss.str().c_str());
				}
				else if(7 == nRow)
				{
					m_DC_AmbientTempEdit.SetWindowText(oss.str().c_str());
				}
			}
			
			return ERROR_SUCCESS;
		}
	}
	
	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	TOTAL DERATIONG FACTOR를 계산합니다.

	@param	nRow 계산을 수행할 줄

	@author	BHK
*/
int CPowerCableCreationSettingDlg::CalcTotalDeratingFactor(const int& nRow)
{
	CString rAmbTempDeratingFactor , rOthersDerationgFactor;
	if(0 == nRow)
	{
		m_HV1_AmbientTempEdit.GetWindowText(rAmbTempDeratingFactor);
		m_HV1_OtherDeratingFactorEdit.GetWindowText(rOthersDerationgFactor);
	}
	else if(1 == nRow)
	{
		m_HV2_AmbientTempEdit.GetWindowText(rAmbTempDeratingFactor);
		m_HV2_OtherDeratingFactorEdit.GetWindowText(rOthersDerationgFactor);
	}
	else if(2 == nRow)
	{
		m_HV3_AmbientTempEdit.GetWindowText(rAmbTempDeratingFactor);
		m_HV3_OtherDeratingFactorEdit.GetWindowText(rOthersDerationgFactor);
	}
	else if(3 == nRow)
	{
		m_LV1_AmbientTempEdit.GetWindowText(rAmbTempDeratingFactor);
		m_LV1_OtherDeratingFactorEdit.GetWindowText(rOthersDerationgFactor);
	}
	else if(4 == nRow)
	{
		m_LV2_AmbientTempEdit.GetWindowText(rAmbTempDeratingFactor);
		m_LV2_OtherDeratingFactorEdit.GetWindowText(rOthersDerationgFactor);
	}
	else if(5 == nRow)
	{
		m_LV11_AmbientTempEdit.GetWindowText(rAmbTempDeratingFactor);
		m_LV11_OtherDeratingFactorEdit.GetWindowText(rOthersDerationgFactor);
	}
	else if(6 == nRow)
	{
		m_LV12_AmbientTempEdit.GetWindowText(rAmbTempDeratingFactor);
		m_LV12_OtherDeratingFactorEdit.GetWindowText(rOthersDerationgFactor);
	}
	else if(7 == nRow)
	{
		m_DC_AmbientTempEdit.GetWindowText(rAmbTempDeratingFactor);
		m_DC_OtherDeratingFactorEdit.GetWindowText(rOthersDerationgFactor);
	}
	
	//! Ambient Derating Factor값이 없을 때는 Other Derating Factor값만 사용한다.
	double nTotalDeratingFactor = 0;
	if(!rAmbTempDeratingFactor.IsEmpty())
		nTotalDeratingFactor = atof(rAmbTempDeratingFactor) * atof(rOthersDerationgFactor);
	else	nTotalDeratingFactor = atof(rOthersDerationgFactor);

	ostringstream oss;
	oss.precision(2);
	oss.setf(ios_base:: fixed, ios_base:: floatfield); // 소수점 방식으로 표시됨
	double dTotalDeratingFactor = SAFE_ROUND(nTotalDeratingFactor , 2);
	oss << dTotalDeratingFactor;

	if(0 == nRow)
	{
		m_HV1_TotalDeratingFactorEdit.SetWindowText(oss.str().c_str());
	}
	else if(1 == nRow)
	{
		m_HV2_TotalDeratingFactorEdit.SetWindowText(oss.str().c_str());
	}
	else if(2 == nRow)
	{
		m_HV3_TotalDeratingFactorEdit.SetWindowText(oss.str().c_str());
	}
	else if(3 == nRow)
	{
		m_LV1_TotalDeratingFactorEdit.SetWindowText(oss.str().c_str());
	}
	else if(4 == nRow)
	{
		m_LV2_TotalDeratingFactorEdit.SetWindowText(oss.str().c_str());
	}
	else if(5 == nRow)
	{
		m_LV11_TotalDeratingFactorEdit.SetWindowText(oss.str().c_str());
	}
	else if(6 == nRow)
	{
		m_LV12_TotalDeratingFactorEdit.SetWindowText(oss.str().c_str());
	}
	else if(7 == nRow)
	{
		m_DC_TotalDeratingFactorEdit.SetWindowText(oss.str().c_str());
	}
	
	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	

	@author	BHK
*/
void CPowerCableCreationSettingDlg::OnBnClickedCheckApplyByAmbientTempFactor()
{
	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_APPLY_BY_AMBIENT_TEMP_FACTOR);
	m_bApplyByAmbientTempFactor = pButton->GetCheck();

	if(m_bApplyByAmbientTempFactor)
	{
		GetDlgItem(IDC_COMBO_AG_TEMP)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_UG_TEMP)->EnableWindow(TRUE);
	}
	else
	{
		m_HV1_AmbientTempEdit.SetWindowText(_T(""));
		m_HV2_AmbientTempEdit.SetWindowText(_T(""));
		m_HV3_AmbientTempEdit.SetWindowText(_T(""));
		m_LV1_AmbientTempEdit.SetWindowText(_T(""));
		m_LV2_AmbientTempEdit.SetWindowText(_T(""));
		m_LV11_AmbientTempEdit.SetWindowText(_T(""));
		m_LV12_AmbientTempEdit.SetWindowText(_T(""));
		m_DC_AmbientTempEdit.SetWindowText(_T(""));

		GetDlgItem(IDC_COMBO_AG_TEMP)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_UG_TEMP)->EnableWindow(FALSE);
	}
}

int CPowerCableCreationSettingDlg::InitEditBox(void)
{
	m_HV1_BusVoltageEdit.SetReadOnly();
	m_HV2_BusVoltageEdit.SetReadOnly();
	m_HV3_BusVoltageEdit.SetReadOnly();
	m_LV1_BusVoltageEdit.SetReadOnly();
	m_LV2_BusVoltageEdit.SetReadOnly();
	m_LV11_BusVoltageEdit.SetReadOnly();
	m_LV12_BusVoltageEdit.SetReadOnly();
	m_DC_BusVoltageEdit.SetReadOnly();

	m_HV1_LoadVoltageEdit.SetReadOnly();
	m_HV2_LoadVoltageEdit.SetReadOnly();
	m_HV3_LoadVoltageEdit.SetReadOnly();
	m_LV1_LoadVoltageEdit.SetReadOnly();
	m_LV2_LoadVoltageEdit.SetReadOnly();
	m_LV11_LoadVoltageEdit.SetReadOnly();
	m_LV12_LoadVoltageEdit.SetReadOnly();
	m_DC_LoadVoltageEdit.SetReadOnly();

	return ERROR_SUCCESS;
}
/**
	@brief	SELECT CABLE VOLTAGE (HV1)
	@author	KHS
*/
void CPowerCableCreationSettingDlg::OnCbnSelchangeHv1CableVoltageCombo()
{
	// TODO: Add your control notification handler code here
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CStringArray CableTypes;
	m_HV1_CableTypeCombo.ResetContent();
	m_HV1_CableCoreCombo.ResetContent();

	for(vector<string>::iterator jtr = m_CableTypeList.begin(); jtr != m_CableTypeList.end();++jtr)
	{	
		CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(*jtr);
		if(pCableDataByType)
		{
			vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
			if(pRecordList)
			{
				for(vector<CCableDataByType::Record*>::iterator ktr = pRecordList->begin();ktr != pRecordList->end();++ktr)
				{
					CString rCableVoltage;
					int nItem = m_HV1_CableVoltageCombo.GetCurSel();
					m_HV1_CableVoltageCombo.GetLBText(nItem, rCableVoltage);
					if((*ktr)->m_FieldMap[_T("C_VOLT")].c_str() == rCableVoltage)
					{
						if(TRUE == docData.CompareCStringArray(CableTypes, (*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str()))
						{
							CableTypes.Add((*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str());
							m_HV1_CableTypeCombo.AddString((*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str());
						}
					}
				}
			}
		}
	}
}
/**
	@brief	SELECT CABLE VOLTAGE (HV2)
	@author	KHS
*/
void CPowerCableCreationSettingDlg::OnCbnSelchangeHv2CableVoltageCombo()
{
	// TODO: Add your control notification handler code here
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CStringArray CableTypes;
	m_HV2_CableTypeCombo.ResetContent();
	m_HV2_CableCoreCombo.ResetContent();

	for(vector<string>::iterator jtr = m_CableTypeList.begin(); jtr != m_CableTypeList.end();++jtr)
	{	
		CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(*jtr);
		if(pCableDataByType)
		{
			vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
			if(pRecordList)
			{
				for(vector<CCableDataByType::Record*>::iterator ktr = pRecordList->begin();ktr != pRecordList->end();++ktr)
				{
					CString rCableVoltage;
					int nItem = m_HV2_CableVoltageCombo.GetCurSel();
					m_HV2_CableVoltageCombo.GetLBText(nItem, rCableVoltage);
					if((*ktr)->m_FieldMap[_T("C_VOLT")].c_str() == rCableVoltage)
					{
						if(TRUE == docData.CompareCStringArray(CableTypes, (*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str()))
						{
							CableTypes.Add((*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str());
							m_HV2_CableTypeCombo.AddString((*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str());
						}
					}
				}
			}
		}
	}
}
/**
	@brief	SELECT CABLE VOLTAGE (HV3)
	@author	KHS
*/
void CPowerCableCreationSettingDlg::OnCbnSelchangeHv3CableVoltageCombo()
{
	// TODO: Add your control notification handler code here
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CStringArray CableTypes;
	m_HV3_CableTypeCombo.ResetContent();
	m_HV3_CableCoreCombo.ResetContent();

	for(vector<string>::iterator jtr = m_CableTypeList.begin(); jtr != m_CableTypeList.end();++jtr)
	{	
		CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(*jtr);
		if(pCableDataByType)
		{
			vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
			if(pRecordList)
			{
				for(vector<CCableDataByType::Record*>::iterator ktr = pRecordList->begin();ktr != pRecordList->end();++ktr)
				{
					CString rCableVoltage;
					int nItem = m_HV3_CableVoltageCombo.GetCurSel();
					m_HV3_CableVoltageCombo.GetLBText(nItem, rCableVoltage);
					if((*ktr)->m_FieldMap[_T("C_VOLT")].c_str() == rCableVoltage)
					{
						if(TRUE == docData.CompareCStringArray(CableTypes, (*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str()))
						{
							CableTypes.Add((*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str());
							m_HV3_CableTypeCombo.AddString((*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str());
						}
					}
				}
			}
		}
	}
}
/**
	@brief	SELECT CABLE VOLTAGE (LV1(3PH))
	@author	KHS
*/
void CPowerCableCreationSettingDlg::OnCbnSelchangeLv1CableVoltageCombo()
{
	// TODO: Add your control notification handler code here
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CStringArray CableTypes;
	m_LV1_CableTypeCombo.ResetContent();
	m_LV1_CableCoreCombo.ResetContent();

	for(vector<string>::iterator jtr = m_CableTypeList.begin(); jtr != m_CableTypeList.end();++jtr)
	{	
		CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(*jtr);
		if(pCableDataByType)
		{
			vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
			if(pRecordList)
			{
				for(vector<CCableDataByType::Record*>::iterator ktr = pRecordList->begin();ktr != pRecordList->end();++ktr)
				{
					CString rCableVoltage;
					int nItem = m_LV1_CableVoltageCombo.GetCurSel();
					m_LV1_CableVoltageCombo.GetLBText(nItem, rCableVoltage);
					if((*ktr)->m_FieldMap[_T("C_VOLT")].c_str() == rCableVoltage)
					{
						if(TRUE == docData.CompareCStringArray(CableTypes, (*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str()))
						{
							CableTypes.Add((*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str());
							m_LV1_CableTypeCombo.AddString((*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str());
						}
					}
				}
			}
		}
	}
}
/**
	@brief	SELECT CABLE VOLTAGE (LV2(3PH))
	@author	KHS
*/
void CPowerCableCreationSettingDlg::OnCbnSelchangeLv2CableVoltageCombo()
{
	// TODO: Add your control notification handler code here
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CStringArray CableTypes;
	m_LV2_CableTypeCombo.ResetContent();
	m_LV2_CableCoreCombo.ResetContent();

	for(vector<string>::iterator jtr = m_CableTypeList.begin(); jtr != m_CableTypeList.end();++jtr)
	{	
		CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(*jtr);
		if(pCableDataByType)
		{
			vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
			if(pRecordList)
			{
				for(vector<CCableDataByType::Record*>::iterator ktr = pRecordList->begin();ktr != pRecordList->end();++ktr)
				{
					CString rCableVoltage;
					int nItem = m_LV2_CableVoltageCombo.GetCurSel();
					m_LV2_CableVoltageCombo.GetLBText(nItem, rCableVoltage);
					if((*ktr)->m_FieldMap[_T("C_VOLT")].c_str() == rCableVoltage)
					{
						if(TRUE == docData.CompareCStringArray(CableTypes, (*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str()))
						{
							CableTypes.Add((*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str());
							m_LV2_CableTypeCombo.AddString((*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str());
						}
					}
				}
			}
		}
	}
}
/**
	@brief	SELECT CABLE VOLTAGE (LV11(1PH))
	@author	KHS
*/
void CPowerCableCreationSettingDlg::OnCbnSelchangeLv11CableVoltageCombo()
{
	// TODO: Add your control notification handler code here
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CStringArray CableTypes;
	m_LV11_CableTypeCombo.ResetContent();
	m_LV11_CableCoreCombo.ResetContent();

	for(vector<string>::iterator jtr = m_CableTypeList.begin(); jtr != m_CableTypeList.end();++jtr)
	{	
		CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(*jtr);
		if(pCableDataByType)
		{
			vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
			if(pRecordList)
			{
				for(vector<CCableDataByType::Record*>::iterator ktr = pRecordList->begin();ktr != pRecordList->end();++ktr)
				{
					CString rCableVoltage;
					int nItem = m_LV11_CableVoltageCombo.GetCurSel();
					m_LV11_CableVoltageCombo.GetLBText(nItem, rCableVoltage);
					if((*ktr)->m_FieldMap[_T("C_VOLT")].c_str() == rCableVoltage)
					{
						if(TRUE == docData.CompareCStringArray(CableTypes, (*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str()))
						{
							CableTypes.Add((*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str());
							m_LV11_CableTypeCombo.AddString((*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str());
						}
					}
				}
			}
		}
	}
}
/**
	@brief	SELECT CABLE VOLTAGE (LV12(1PH))
	@author	KHS
*/
void CPowerCableCreationSettingDlg::OnCbnSelchangeLv12CableVoltageCombo()
{
	// TODO: Add your control notification handler code here
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CStringArray CableTypes;
	m_LV12_CableTypeCombo.ResetContent();
	m_LV12_CableCoreCombo.ResetContent();

	for(vector<string>::iterator jtr = m_CableTypeList.begin(); jtr != m_CableTypeList.end();++jtr)
	{	
		CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(*jtr);
		if(pCableDataByType)
		{
			vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
			if(pRecordList)
			{
				for(vector<CCableDataByType::Record*>::iterator ktr = pRecordList->begin();ktr != pRecordList->end();++ktr)
				{
					CString rCableVoltage;
					int nItem = m_LV12_CableVoltageCombo.GetCurSel();
					m_LV12_CableVoltageCombo.GetLBText(nItem, rCableVoltage);
					if((*ktr)->m_FieldMap[_T("C_VOLT")].c_str() == rCableVoltage)
					{
						if(TRUE == docData.CompareCStringArray(CableTypes, (*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str()))
						{
							CableTypes.Add((*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str());
							m_LV12_CableTypeCombo.AddString((*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str());
						}
					}
				}
			}
		}
	}
}
/**
	@brief	SELECT CABLE VOLTAGE (DC)
	@author	KHS
*/
void CPowerCableCreationSettingDlg::OnCbnSelchangeDcCableVoltageCombo()
{
	// TODO: Add your control notification handler code here
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CStringArray CableTypes;
	m_DC_CableTypeCombo.ResetContent();
	m_DC_CableCoreCombo.ResetContent();

	for(vector<string>::iterator jtr = m_CableTypeList.begin(); jtr != m_CableTypeList.end();++jtr)
	{	
		CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(*jtr);
		if(pCableDataByType)
		{
			vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
			if(pRecordList)
			{
				for(vector<CCableDataByType::Record*>::iterator ktr = pRecordList->begin();ktr != pRecordList->end();++ktr)
				{
					CString rCableVoltage;
					int nItem = m_DC_CableVoltageCombo.GetCurSel();
					m_DC_CableVoltageCombo.GetLBText(nItem, rCableVoltage);
					if((*ktr)->m_FieldMap[_T("C_VOLT")].c_str() == rCableVoltage)
					{
						if(TRUE == docData.CompareCStringArray(CableTypes, (*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str()))
						{
							CableTypes.Add((*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str());
							m_DC_CableTypeCombo.AddString((*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str());
						}
					}
				}
			}
		}
	}
}
/**
	@brief	SELECT CABLE TYPE (HV1)
	@author	KHS
*/
void CPowerCableCreationSettingDlg::OnCbnSelchangeHv1CableTypeCombo()
{
	// TODO: Add your control notification handler code here
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CStringArray CableCores;
	m_HV1_CableCoreCombo.ResetContent();

	for(vector<string>::iterator jtr = m_CableTypeList.begin(); jtr != m_CableTypeList.end();++jtr)
	{	
		CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(*jtr);
		if(pCableDataByType)
		{
			vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
			if(pRecordList)
			{
				for(vector<CCableDataByType::Record*>::iterator ktr = pRecordList->begin();ktr != pRecordList->end();++ktr)
				{
					CString rCableVoltage, rCableType;
					int nCableVoltageItem = m_HV1_CableVoltageCombo.GetCurSel();
					int nCableTypeItem = m_HV1_CableTypeCombo.GetCurSel();
					m_HV1_CableVoltageCombo.GetLBText(nCableVoltageItem, rCableVoltage);
					m_HV1_CableTypeCombo.GetLBText(nCableTypeItem, rCableType);
					if((*ktr)->m_FieldMap[_T("C_VOLT")].c_str() == rCableVoltage && (*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str() == rCableType)
					{
						if(TRUE == docData.CompareCStringArray(CableCores, (*ktr)->m_FieldMap[_T("C_CORE")].c_str()))
						{
							CableCores.Add((*ktr)->m_FieldMap[_T("C_CORE")].c_str());
							m_HV1_CableCoreCombo.AddString((*ktr)->m_FieldMap[_T("C_CORE")].c_str());
						}
					}
				}
			}
		}
	}
}
/**
	@brief	SELECT CABLE TYPE (HV2)
	@author	KHS
*/
void CPowerCableCreationSettingDlg::OnCbnSelchangeHv2CableTypeCombo()
{
	// TODO: Add your control notification handler code here
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CStringArray CableCores;
	m_HV2_CableCoreCombo.ResetContent();

	for(vector<string>::iterator jtr = m_CableTypeList.begin(); jtr != m_CableTypeList.end();++jtr)
	{	
		CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(*jtr);
		if(pCableDataByType)
		{
			vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
			if(pRecordList)
			{
				for(vector<CCableDataByType::Record*>::iterator ktr = pRecordList->begin();ktr != pRecordList->end();++ktr)
				{
					CString rCableVoltage, rCableType;
					int nCableVoltageItem = m_HV2_CableVoltageCombo.GetCurSel();
					int nCableTypeItem = m_HV2_CableTypeCombo.GetCurSel();
					m_HV2_CableVoltageCombo.GetLBText(nCableVoltageItem, rCableVoltage);
					m_HV2_CableTypeCombo.GetLBText(nCableTypeItem, rCableType);
					if((*ktr)->m_FieldMap[_T("C_VOLT")].c_str() == rCableVoltage && (*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str() == rCableType)
					{
						if(TRUE == docData.CompareCStringArray(CableCores, (*ktr)->m_FieldMap[_T("C_CORE")].c_str()))
						{
							CableCores.Add((*ktr)->m_FieldMap[_T("C_CORE")].c_str());
							m_HV2_CableCoreCombo.AddString((*ktr)->m_FieldMap[_T("C_CORE")].c_str());
						}
					}
				}
			}
		}
	}
}
/**
	@brief	SELECT CABLE TYPE (HV3)
	@author	KHS
*/
void CPowerCableCreationSettingDlg::OnCbnSelchangeHv3CableTypeCombo()
{
	// TODO: Add your control notification handler code here
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CStringArray CableCores;
	m_HV3_CableCoreCombo.ResetContent();

	for(vector<string>::iterator jtr = m_CableTypeList.begin(); jtr != m_CableTypeList.end();++jtr)
	{	
		CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(*jtr);
		if(pCableDataByType)
		{
			vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
			if(pRecordList)
			{
				for(vector<CCableDataByType::Record*>::iterator ktr = pRecordList->begin();ktr != pRecordList->end();++ktr)
				{
					CString rCableVoltage, rCableType;
					int nCableVoltageItem = m_HV3_CableVoltageCombo.GetCurSel();
					int nCableTypeItem = m_HV3_CableTypeCombo.GetCurSel();
					m_HV3_CableVoltageCombo.GetLBText(nCableVoltageItem, rCableVoltage);
					m_HV3_CableTypeCombo.GetLBText(nCableTypeItem, rCableType);
					if((*ktr)->m_FieldMap[_T("C_VOLT")].c_str() == rCableVoltage && (*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str() == rCableType)
					{
						if(TRUE == docData.CompareCStringArray(CableCores, (*ktr)->m_FieldMap[_T("C_CORE")].c_str()))
						{
							CableCores.Add((*ktr)->m_FieldMap[_T("C_CORE")].c_str());
							m_HV3_CableCoreCombo.AddString((*ktr)->m_FieldMap[_T("C_CORE")].c_str());
						}
					}
				}
			}
		}
	}
}
/**
	@brief	SELECT CABLE TYPE (LV1)
	@author	KHS
*/
void CPowerCableCreationSettingDlg::OnCbnSelchangeLv1CableTypeCombo()
{
	// TODO: Add your control notification handler code here
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CStringArray CableCores;
	m_LV1_CableCoreCombo.ResetContent();

	for(vector<string>::iterator jtr = m_CableTypeList.begin(); jtr != m_CableTypeList.end();++jtr)
	{	
		CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(*jtr);
		if(pCableDataByType)
		{
			vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
			if(pRecordList)
			{
				for(vector<CCableDataByType::Record*>::iterator ktr = pRecordList->begin();ktr != pRecordList->end();++ktr)
				{
					CString rCableVoltage, rCableType;
					int nCableVoltageItem = m_LV1_CableVoltageCombo.GetCurSel();
					int nCableTypeItem = m_LV1_CableTypeCombo.GetCurSel();
					m_LV1_CableVoltageCombo.GetLBText(nCableVoltageItem, rCableVoltage);
					m_LV1_CableTypeCombo.GetLBText(nCableTypeItem, rCableType);
					if((*ktr)->m_FieldMap[_T("C_VOLT")].c_str() == rCableVoltage && (*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str() == rCableType)
					{
						if(TRUE == docData.CompareCStringArray(CableCores, (*ktr)->m_FieldMap[_T("C_CORE")].c_str()))
						{
							CableCores.Add((*ktr)->m_FieldMap[_T("C_CORE")].c_str());
							m_LV1_CableCoreCombo.AddString((*ktr)->m_FieldMap[_T("C_CORE")].c_str());
						}
					}
				}
			}
		}
	}
}
/**
	@brief	SELECT CABLE TYPE (LV2)
	@author	KHS
*/
void CPowerCableCreationSettingDlg::OnCbnSelchangeLv2CableTypeCombo()
{
	// TODO: Add your control notification handler code here
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CStringArray CableCores;
	m_LV2_CableCoreCombo.ResetContent();

	for(vector<string>::iterator jtr = m_CableTypeList.begin(); jtr != m_CableTypeList.end();++jtr)
	{	
		CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(*jtr);
		if(pCableDataByType)
		{
			vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
			if(pRecordList)
			{
				for(vector<CCableDataByType::Record*>::iterator ktr = pRecordList->begin();ktr != pRecordList->end();++ktr)
				{
					CString rCableVoltage, rCableType;
					int nCableVoltageItem = m_LV2_CableVoltageCombo.GetCurSel();
					int nCableTypeItem = m_LV2_CableTypeCombo.GetCurSel();
					m_LV2_CableVoltageCombo.GetLBText(nCableVoltageItem, rCableVoltage);
					m_LV2_CableTypeCombo.GetLBText(nCableTypeItem, rCableType);
					if((*ktr)->m_FieldMap[_T("C_VOLT")].c_str() == rCableVoltage && (*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str() == rCableType)
					{
						if(TRUE == docData.CompareCStringArray(CableCores, (*ktr)->m_FieldMap[_T("C_CORE")].c_str()))
						{
							CableCores.Add((*ktr)->m_FieldMap[_T("C_CORE")].c_str());
							m_LV2_CableCoreCombo.AddString((*ktr)->m_FieldMap[_T("C_CORE")].c_str());
						}
					}
				}
			}
		}
	}
}
/**
	@brief	SELECT CABLE TYPE (LV11)
	@author	KHS
*/
void CPowerCableCreationSettingDlg::OnCbnSelchangeLv11CableTypeCombo()
{
	// TODO: Add your control notification handler code here
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CStringArray CableCores;
	m_LV11_CableCoreCombo.ResetContent();

	for(vector<string>::iterator jtr = m_CableTypeList.begin(); jtr != m_CableTypeList.end();++jtr)
	{	
		CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(*jtr);
		if(pCableDataByType)
		{
			vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
			if(pRecordList)
			{
				for(vector<CCableDataByType::Record*>::iterator ktr = pRecordList->begin();ktr != pRecordList->end();++ktr)
				{
					CString rCableVoltage, rCableType;
					int nCableVoltageItem = m_LV11_CableVoltageCombo.GetCurSel();
					int nCableTypeItem = m_LV11_CableTypeCombo.GetCurSel();
					m_LV11_CableVoltageCombo.GetLBText(nCableVoltageItem, rCableVoltage);
					m_LV11_CableTypeCombo.GetLBText(nCableTypeItem, rCableType);
					if((*ktr)->m_FieldMap[_T("C_VOLT")].c_str() == rCableVoltage && (*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str() == rCableType)
					{
						if(TRUE == docData.CompareCStringArray(CableCores, (*ktr)->m_FieldMap[_T("C_CORE")].c_str()))
						{
							CableCores.Add((*ktr)->m_FieldMap[_T("C_CORE")].c_str());
							m_LV11_CableCoreCombo.AddString((*ktr)->m_FieldMap[_T("C_CORE")].c_str());
						}
					}
				}
			}
		}
	}
}
/**
	@brief	SELECT CABLE TYPE (LV12)
	@author	KHS
*/
void CPowerCableCreationSettingDlg::OnCbnSelchangeLv12CableTypeCombo()
{
	// TODO: Add your control notification handler code here
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CStringArray CableCores;
	m_LV12_CableCoreCombo.ResetContent();

	for(vector<string>::iterator jtr = m_CableTypeList.begin(); jtr != m_CableTypeList.end();++jtr)
	{	
		CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(*jtr);
		if(pCableDataByType)
		{
			vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
			if(pRecordList)
			{
				for(vector<CCableDataByType::Record*>::iterator ktr = pRecordList->begin();ktr != pRecordList->end();++ktr)
				{
					CString rCableVoltage, rCableType;
					int nCableVoltageItem = m_LV12_CableVoltageCombo.GetCurSel();
					int nCableTypeItem = m_LV12_CableTypeCombo.GetCurSel();
					m_LV12_CableVoltageCombo.GetLBText(nCableVoltageItem, rCableVoltage);
					m_LV12_CableTypeCombo.GetLBText(nCableTypeItem, rCableType);
					if((*ktr)->m_FieldMap[_T("C_VOLT")].c_str() == rCableVoltage && (*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str() == rCableType)
					{
						if(TRUE == docData.CompareCStringArray(CableCores, (*ktr)->m_FieldMap[_T("C_CORE")].c_str()))
						{
							CableCores.Add((*ktr)->m_FieldMap[_T("C_CORE")].c_str());
							m_LV12_CableCoreCombo.AddString((*ktr)->m_FieldMap[_T("C_CORE")].c_str());
						}
					}
				}
			}
		}
	}
}
/**
	@brief	SELECT CABLE TYPE (DC)
	@author	KHS
*/
void CPowerCableCreationSettingDlg::OnCbnSelchangeDcCableTypeCombo()
{
	// TODO: Add your control notification handler code here
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CStringArray CableCores;
	m_DC_CableCoreCombo.ResetContent();

	for(vector<string>::iterator jtr = m_CableTypeList.begin(); jtr != m_CableTypeList.end();++jtr)
	{	
		CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(*jtr);
		if(pCableDataByType)
		{
			vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
			if(pRecordList)
			{
				for(vector<CCableDataByType::Record*>::iterator ktr = pRecordList->begin();ktr != pRecordList->end();++ktr)
				{
					CString rCableVoltage, rCableType;
					int nCableVoltageItem = m_DC_CableVoltageCombo.GetCurSel();
					int nCableTypeItem = m_DC_CableTypeCombo.GetCurSel();
					m_DC_CableVoltageCombo.GetLBText(nCableVoltageItem, rCableVoltage);
					m_DC_CableTypeCombo.GetLBText(nCableTypeItem, rCableType);
					if((*ktr)->m_FieldMap[_T("C_VOLT")].c_str() == rCableVoltage && (*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str() == rCableType)
					{
						if(TRUE == docData.CompareCStringArray(CableCores, (*ktr)->m_FieldMap[_T("C_CORE")].c_str()))
						{
							CableCores.Add((*ktr)->m_FieldMap[_T("C_CORE")].c_str());
							m_DC_CableCoreCombo.AddString((*ktr)->m_FieldMap[_T("C_CORE")].c_str());
						}
					}
				}
			}
		}
	}
}

/**
	@brief	CalcAmbientTempDeratingFactor을 호출한다.

	@author	BHK
*/
void CPowerCableCreationSettingDlg::OnCbnSelendokHv1InstallMethodCombo()
{
	CalcAmbientTempDeratingFactor(0);
}

/**
	@brief	CalcAmbientTempDeratingFactor을 호출한다.

	@author	BHK
*/
void CPowerCableCreationSettingDlg::OnCbnSelendokHv2InstallMethodCombo()
{
	CalcAmbientTempDeratingFactor(1);
}

/**
	@brief	CalcAmbientTempDeratingFactor을 호출한다.

	@author	BHK
*/
void CPowerCableCreationSettingDlg::OnCbnSelendokHv3InstallMethodCombo()
{
	CalcAmbientTempDeratingFactor(2);
}

/**
	@brief	CalcAmbientTempDeratingFactor을 호출한다.

	@author	BHK
*/
void CPowerCableCreationSettingDlg::OnCbnSelendokLv1InstallMethodCombo()
{
	CalcAmbientTempDeratingFactor(3);
}

/**
	@brief	CalcAmbientTempDeratingFactor을 호출한다.

	@author	BHK
*/
void CPowerCableCreationSettingDlg::OnCbnSelendokLv2InstallMethodCombo()
{
	CalcAmbientTempDeratingFactor(4);
}

/**
	@brief	CalcAmbientTempDeratingFactor을 호출한다.

	@author	BHK
*/
void CPowerCableCreationSettingDlg::OnCbnSelendokLv11InstallMethodCombo()
{
	CalcAmbientTempDeratingFactor(5);
}

/**
	@brief	CalcAmbientTempDeratingFactor을 호출한다.

	@author	BHK
*/
void CPowerCableCreationSettingDlg::OnCbnSelendokLv12InstallMethodCombo()
{
	CalcAmbientTempDeratingFactor(6);
}

/**
	@brief	CalcAmbientTempDeratingFactor을 호출한다.

	@author	BHK
*/
void CPowerCableCreationSettingDlg::OnCbnSelendokDcInstallMethodCombo()
{
	CalcAmbientTempDeratingFactor(7);
}

/**
	@brief	

	@author	BHK
*/
void CPowerCableCreationSettingDlg::OnCbnSelendokHv1CableVoltageCombo()
{
	CalcAmbientTempDeratingFactor(0);
}

void CPowerCableCreationSettingDlg::OnCbnSelendokHv2CableVoltageCombo()
{
	CalcAmbientTempDeratingFactor(1);
}

void CPowerCableCreationSettingDlg::OnCbnSelendokHv3CableVoltageCombo()
{
	CalcAmbientTempDeratingFactor(2);
}

/**
	@brief	

	@author	BHK
*/
void CPowerCableCreationSettingDlg::OnCbnSelendokLv1CableVoltageCombo()
{
	CalcAmbientTempDeratingFactor(3);
	FillApplyMinLVPowerCableSizeComboBox();
}

void CPowerCableCreationSettingDlg::OnCbnSelendokLv2CableVoltageCombo()
{
	CalcAmbientTempDeratingFactor(4);
}

void CPowerCableCreationSettingDlg::OnCbnSelendokLv11CableVoltageCombo()
{
	CalcAmbientTempDeratingFactor(5);
}

void CPowerCableCreationSettingDlg::OnCbnSelendokLv12CableVoltageCombo()
{
	CalcAmbientTempDeratingFactor(6);
}

void CPowerCableCreationSettingDlg::OnCbnSelendokDcCableVoltageCombo()
{
	CalcAmbientTempDeratingFactor(7);
}

void CPowerCableCreationSettingDlg::OnCbnSelendokHv1CableTypeCombo()
{
	CalcAmbientTempDeratingFactor(0);
}

void CPowerCableCreationSettingDlg::OnCbnSelendokHv2CableTypeCombo()
{
	CalcAmbientTempDeratingFactor(1);
}

void CPowerCableCreationSettingDlg::OnCbnSelendokHv3CableTypeCombo()
{
	CalcAmbientTempDeratingFactor(2);
}

void CPowerCableCreationSettingDlg::OnCbnSelendokLv1CableTypeCombo()
{
	CalcAmbientTempDeratingFactor(3);
	FillApplyMinLVPowerCableSizeComboBox();
}

void CPowerCableCreationSettingDlg::OnCbnSelendokLv2CableTypeCombo()
{
	CalcAmbientTempDeratingFactor(4);
	FillApplyMinLVPowerCableSizeComboBox();
}

void CPowerCableCreationSettingDlg::OnCbnSelendokLv11CableTypeCombo()
{
	CalcAmbientTempDeratingFactor(5);
}

void CPowerCableCreationSettingDlg::OnCbnSelendokLv12CableTypeCombo()
{
	CalcAmbientTempDeratingFactor(6);
}

void CPowerCableCreationSettingDlg::OnCbnSelendokDcCableTypeCombo()
{
	CalcAmbientTempDeratingFactor(7);
}

void CPowerCableCreationSettingDlg::OnCbnSelendokHv1CableCoreCombo()
{
	CalcAmbientTempDeratingFactor(0);
}

void CPowerCableCreationSettingDlg::OnCbnSelendokHv2CableCoreCombo()
{
	CalcAmbientTempDeratingFactor(1);
}

void CPowerCableCreationSettingDlg::OnCbnSelendokHv3CableCoreCombo()
{
	CalcAmbientTempDeratingFactor(2);
}

void CPowerCableCreationSettingDlg::OnCbnSelendokLv1CableCoreCombo()
{
	CalcAmbientTempDeratingFactor(3);
	FillApplyMinLVPowerCableSizeComboBox();
}

void CPowerCableCreationSettingDlg::OnCbnSelendokLv2CableCoreCombo()
{
	CalcAmbientTempDeratingFactor(4);
	FillApplyMinLVPowerCableSizeComboBox();
}

void CPowerCableCreationSettingDlg::OnCbnSelendokLv11CableCoreCombo()
{
	CalcAmbientTempDeratingFactor(5);
}

void CPowerCableCreationSettingDlg::OnCbnSelendokLv12CableCoreCombo()
{
	CalcAmbientTempDeratingFactor(6);
}

void CPowerCableCreationSettingDlg::OnCbnSelendokDcCableCoreCombo()
{
	CalcAmbientTempDeratingFactor(7);
}

/**
	@brief	bus에 속한 모든 load item들에 대해서 cable에 관련한 속성을 설정한다.

	@author	BHK
*/
static int SetLoadPropInBus(vector<CBusItem*>& BusItemEntry , CCableCreationTable::Record& record)
{
	string rRatingCapacityAbove = _T("0");
	try
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		CPowerCableCreationSettingTable* pPowerCableCreationSettingTable = static_cast<CPowerCableCreationSettingTable*>(docData.GetTableOf(CPowerCableCreationSettingTable::GetTableName()));
		rRatingCapacityAbove = pPowerCableCreationSettingTable->m_RecordEntry[0].FieldValueMap[_T("C_RATING_CAPACITY_ABOVE")];
	}
	catch(...)
	{
	}

	const string rCableVolt = record.FieldValueMap[_T("C_CABLE_VOLTAGE")];
	const string rCableType = record.FieldValueMap[_T("C_CABLE_TYPE")];
	const string rCableSpec = record.FieldValueMap[_T("C_CABLE_SCHEDULE_SPEC")];
	const string rCableCore = record.FieldValueMap[_T("C_CABLE_CORE")];

	const string rMotorRunningVD     = record.FieldValueMap[_T("C_MOTOR_RUNNING")];
	const string rMotorStartingVD    = record.FieldValueMap[_T("C_MOTOR_STARTING")];
	const string rApplyMotorStarting = record.FieldValueMap[_T("C_APPLY_MOTOR_STARTING")];
	const string rFeederRunning      = record.FieldValueMap[_T("C_FEEDER_RUNNING")];
	const string rInstallMethod	 = record.FieldValueMap[_T("C_INSTALL_METHOD")];
	const string rSF		 = record.FieldValueMap[_T("C_SF")];

	const string rAmbTempDF = record.FieldValueMap[_T("C_AMBIENT_TEMP_DERATING_FACTOR")];
	const string rOthersDF  = record.FieldValueMap[_T("C_OTHERS_DERATING_FACTOR")];
	const string rTotalDF   = record.FieldValueMap[_T("C_TOTAL_DERATING_FACTOR")];

	CELoadDocData& docData = CELoadDocData::GetInstance();
	for(vector<CBusItem*>::iterator itr = BusItemEntry.begin();itr != BusItemEntry.end();++itr)
	{
		CItemSelectionSet SelectionSet;
		docData.GetLoadItemSetInBus(SelectionSet , (*itr)->GetName());
		for(int i = 0;i < SelectionSet.GetItemCount();++i)
		{
			SelectionSet.GetItemAt(i)->prop()->SetValue(_T("Power Cable Setting") , _T("Cable Voltage") , rCableVolt);
			SelectionSet.GetItemAt(i)->prop()->SetValue(_T("Power Cable Setting") , _T("Cable Type") , rCableType);
			SelectionSet.GetItemAt(i)->prop()->SetValue(_T("Power Cable Setting") , _T("Cable Spec") , rCableSpec);
			SelectionSet.GetItemAt(i)->prop()->SetValue(_T("Power Cable Setting") , _T("Cable Core") , rCableCore);
			SelectionSet.GetItemAt(i)->prop()->SetValue(_T("Power Cable Setting") , _T("Rating Capacity Above") , rRatingCapacityAbove);

			SelectionSet.GetItemAt(i)->prop()->SetValue(_T("Voltage Drop") , _T("Motor Run VD") , rMotorRunningVD);
			SelectionSet.GetItemAt(i)->prop()->SetValue(_T("Voltage Drop") , _T("Motor Start VD") , rMotorStartingVD);
			SelectionSet.GetItemAt(i)->prop()->SetValue(_T("Voltage Drop") , _T("Motor Start Apply") , rApplyMotorStarting);
			SelectionSet.GetItemAt(i)->prop()->SetValue(_T("Voltage Drop") , _T("Feeder Run VD") , rFeederRunning);
			SelectionSet.GetItemAt(i)->prop()->SetValue(_T("Voltage Drop") , _T("SF For FLC") , rSF);

			SelectionSet.GetItemAt(i)->prop()->SetValue(_T("Derating Factor") , _T("Amb Temp DF") , rAmbTempDF);
			SelectionSet.GetItemAt(i)->prop()->SetValue(_T("Derating Factor") , _T("Others DF") , rOthersDF);
			///SelectionSet.GetItemAt(i)->prop()->SetValue(_T("Derating Factor") , _T("Total DF") , rTotalDF);
			
			COMMAND::CCommandAutoCalculateTotalDF cmdAutoCalculateTotalDF;
			cmdAutoCalculateTotalDF.m_SelectionSet.Add(SelectionSet.GetItemAt(i));
			cmdAutoCalculateTotalDF.Execute(false);
			
			/*
			const double nAmbTempDF = atof(SelectionSet.GetItemAt(i)->prop()->GetValue(_T("Derating Factor") , _T("Amb Temp DF")).c_str());
			const double nOthersDF  = atof(SelectionSet.GetItemAt(i)->prop()->GetValue(_T("Derating Factor") , _T("Others DF")).c_str());
			ostringstream oss;
			oss << SAFE_ROUND(nAmbTempDF*nOthersDF,2);
			SelectionSet.GetItemAt(i)->prop()->SetValue(_T("Derating Factor") , _T("Total DF") , oss.str());
			*/

			SelectionSet.GetItemAt(i)->prop()->SetValue(_T("Cable Route") , _T("Install") , rInstallMethod);
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	설정된 정보로 Load Item을 갱신한다.

	@author	BHK
*/
int CPowerCableCreationSettingDlg::UpdateLoadItems(void)
{
	vector<CBusItem*> BusItemEntry;
	CELoadDocData& docData = CELoadDocData::GetInstance();
	docData.GetAllBusItems(BusItemEntry);

	CHVCableCreationSettingTable& HVVoltageSettingTable = CHVCableCreationSettingTable::GetInstance();
	for(vector<CCableCreationTable::Record>::iterator itr = HVVoltageSettingTable.m_RecordEntry.begin();itr != HVVoltageSettingTable.m_RecordEntry.end();++itr)
	{
		map<string,string>::iterator where = itr->FieldValueMap.find(_T("C_BUS_VOLTAGE"));
		if(where != itr->FieldValueMap.end())
		{
			vector<CBusItem*> SelectedBusItemEntry;
			for(vector<CBusItem*>::iterator jtr = BusItemEntry.begin();jtr != BusItemEntry.end();++jtr)
			{
				const string rValue = (*jtr)->prop()->GetValue(_T("General") , _T("Bus Voltage"));
				if(atof(where->second.c_str()) == atof(rValue.c_str())) SelectedBusItemEntry.push_back(*jtr);
			}

			SetLoadPropInBus(SelectedBusItemEntry , (*itr));
		}
	}

	CLV3PHCableCreationSettingTable& LV3PHVoltageSettingTable = CLV3PHCableCreationSettingTable::GetInstance();
	for(vector<CCableCreationTable::Record>::iterator itr = LV3PHVoltageSettingTable.m_RecordEntry.begin();itr != LV3PHVoltageSettingTable.m_RecordEntry.end();++itr)
	{
		map<string,string>::iterator where = itr->FieldValueMap.find(_T("C_BUS_VOLTAGE"));
		if(where != itr->FieldValueMap.end())
		{
			vector<CBusItem*> SelectedBusItemEntry;
			for(vector<CBusItem*>::iterator jtr = BusItemEntry.begin();jtr != BusItemEntry.end();++jtr)
			{
				const string rValue = (*jtr)->prop()->GetValue(_T("General") , _T("Bus Voltage"));
				if(atof(where->second.c_str()) == atof(rValue.c_str())) SelectedBusItemEntry.push_back(*jtr);
			}

			SetLoadPropInBus(SelectedBusItemEntry , *itr);
		}
	}

	CLV1PHCableCreationSettingTable& LV1PHVoltageSettingTable = CLV1PHCableCreationSettingTable::GetInstance();
	for(vector<CCableCreationTable::Record>::iterator itr = LV1PHVoltageSettingTable.m_RecordEntry.begin();itr != LV1PHVoltageSettingTable.m_RecordEntry.end();++itr)
	{
		map<string,string>::iterator where = itr->FieldValueMap.find(_T("C_BUS_VOLTAGE"));
		if(where != itr->FieldValueMap.end())
		{
			vector<CBusItem*> SelectedBusItemEntry;
			for(vector<CBusItem*>::iterator jtr = BusItemEntry.begin();jtr != BusItemEntry.end();++jtr)
			{
				const string rValue = (*jtr)->prop()->GetValue(_T("General") , _T("Bus Voltage"));
				if(atof(where->second.c_str()) == atof(rValue.c_str())) SelectedBusItemEntry.push_back(*jtr);
			}

			SetLoadPropInBus(SelectedBusItemEntry , *itr);
		}
	}

	CDCCableCreationSettingTable& DCVoltageSettingTable = CDCCableCreationSettingTable::GetInstance();
	for(vector<CCableCreationTable::Record>::iterator itr = DCVoltageSettingTable.m_RecordEntry.begin();itr != DCVoltageSettingTable.m_RecordEntry.end();++itr)
	{
		map<string,string>::iterator where = itr->FieldValueMap.find(_T("C_BUS_VOLTAGE"));
		if(where != itr->FieldValueMap.end())
		{
			vector<CBusItem*> SelectedBusItemEntry;
			for(vector<CBusItem*>::iterator jtr = BusItemEntry.begin();jtr != BusItemEntry.end();++jtr)
			{
				const string rValue = (*jtr)->prop()->GetValue(_T("General") , _T("Bus Voltage"));
				if(atof(where->second.c_str()) == atof(rValue.c_str())) SelectedBusItemEntry.push_back(*jtr);
			}

			SetLoadPropInBus(SelectedBusItemEntry , *itr);
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
void CPowerCableCreationSettingDlg::OnEnChangeHv1AmbientTempEdit()
{
	CalcTotalDeratingFactor(0);
}

void CPowerCableCreationSettingDlg::OnEnChangeHv2AmbientTempEdit()
{
	CalcTotalDeratingFactor(1);
}

void CPowerCableCreationSettingDlg::OnEnChangeHv3AmbientTempEdit()
{
	CalcTotalDeratingFactor(2);
}

void CPowerCableCreationSettingDlg::OnEnChangeLv1AmbientTempEdit()
{
	CalcTotalDeratingFactor(3);
}

void CPowerCableCreationSettingDlg::OnEnChangeLv2AmbientTempEdit()
{
	CalcTotalDeratingFactor(4);
}

void CPowerCableCreationSettingDlg::OnEnChangeLv11AmbientTempEdit()
{
	CalcTotalDeratingFactor(5);
}

void CPowerCableCreationSettingDlg::OnEnChangeLv12AmbientTempEdit()
{
	CalcTotalDeratingFactor(6);
}

void CPowerCableCreationSettingDlg::OnEnChangeDcAmbientTempEdit()
{
	CalcTotalDeratingFactor(7);
}

void CPowerCableCreationSettingDlg::OnEnChangeHv1OtherEdit()
{
	CalcTotalDeratingFactor(0);
}

void CPowerCableCreationSettingDlg::OnEnChangeHv2OtherEdit()
{
	CalcTotalDeratingFactor(1);
}

void CPowerCableCreationSettingDlg::OnEnChangeHv3OtherEdit()
{
	CalcTotalDeratingFactor(2);
}

void CPowerCableCreationSettingDlg::OnEnChangeLv1OtherEdit()
{
	CalcTotalDeratingFactor(3);
}

void CPowerCableCreationSettingDlg::OnEnChangeLv2OtherEdit()
{
	CalcTotalDeratingFactor(4);
}

void CPowerCableCreationSettingDlg::OnEnChangeLv11OtherEdit()
{
	CalcTotalDeratingFactor(5);
}

void CPowerCableCreationSettingDlg::OnEnChangeLv12OtherEdit()
{
	CalcTotalDeratingFactor(6);
}

void CPowerCableCreationSettingDlg::OnEnChangeDcOtherEdit()
{
	CalcTotalDeratingFactor(7);
}

/**
	@brief	APPLY MINIMUM LV POWER CABLE SIZE COMBO-BOX를 채운다.

	@author	BHK
*/
int CPowerCableCreationSettingDlg::FillApplyMinLVPowerCableSizeComboBox(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CString rCableVolt , rCableType , rCableCore;

	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_APPLY_MIN_LV_POWER_CABLE_SIZE);
	if(pComboBox) pComboBox->ResetContent();

	if(pComboBox)
	{
		m_LV1_CableVoltageCombo.GetWindowText(rCableVolt);
		m_LV1_CableTypeCombo.GetWindowText(rCableType);
		m_LV1_CableCoreCombo.GetWindowText(rCableCore);

		const string rType = CString(rCableVolt + "KV " + rCableCore + " " + CString("CU") + " " + rCableType + " (" + CString("eLoad") + ")").operator LPCSTR();

		const string rTableName = docData.m_ProjectCableDataTable.GetTableName();
		docData.m_ProjectCableDataTable.Load(rTableName);
		CCableDataByType* pCableDataType = docData.m_ProjectCableDataTable.GetRecordListOf(rType);
		if(pCableDataType)
		{
			vector<CCableDataByType::Record*>* pRecordList = pCableDataType->GetRecordEntryOfHertz();
			if(pRecordList && !pRecordList->empty())
			{
				for(vector<CCableDataByType::Record*>::iterator itr = pRecordList->begin();itr != pRecordList->end();++itr)
				{
					CString rCableSize  = (*itr)->m_FieldMap[_T("C_SIZE")].c_str();
					const int nFound = pComboBox->FindString(-1 , rCableSize);
					if(-1 == nFound) pComboBox->AddString(rCableSize);
				}
			}
		}
	}

	if(pComboBox)
	{
		m_LV2_CableVoltageCombo.GetWindowText(rCableVolt);
		m_LV2_CableTypeCombo.GetWindowText(rCableType);
		m_LV2_CableCoreCombo.GetWindowText(rCableCore);

		const string rType = CString(rCableVolt + "KV " + rCableCore + " " + CString("CU") + " " + rCableType + " (" + CString("eLoad") + ")").operator LPCSTR();

		CCableDataByType* pCableDataType = docData.m_ProjectCableDataTable.GetRecordListOf(rType);
		if(pCableDataType)
		{
			vector<CCableDataByType::Record*>* pRecordList = pCableDataType->GetRecordEntryOfHertz();
			if(pRecordList && !pRecordList->empty())
			{
				for(vector<CCableDataByType::Record*>::iterator itr = pRecordList->begin();itr != pRecordList->end();++itr)
				{
					CString rCableSize  = (*itr)->m_FieldMap[_T("C_SIZE")].c_str();
					const int nFound = pComboBox->FindString(-1 , rCableSize);
					if(-1 == nFound) pComboBox->AddString(rCableSize);
				}
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
void CPowerCableCreationSettingDlg::OnBnClickedCheckApplyMinimumLvPowerCableSize()
{
	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_APPLY_MINIMUM_LV_POWER_CABLE_SIZE);
	if(pButton)
	{
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_APPLY_MIN_LV_POWER_CABLE_SIZE);

		if(pComboBox) pComboBox->EnableWindow(pButton->GetCheck());
	}
}

/**
	@brief	

	@author	BHK
*/
void CPowerCableCreationSettingDlg::OnBnClickedCheckApplyByFaultCurrentAtHvPowerCable()
{
	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_APPLY_BY_FAULT_CURRENT_AT_HV_POWER_CABLE);
	if(pButton)
	{
		CButton* pButtonCalc = (CButton*)GetDlgItem(IDC_BUTTON_CALC);

		if(pButtonCalc) pButtonCalc->EnableWindow(pButton->GetCheck());
	}
}

BOOL CPowerCableCreationSettingDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE) return TRUE;
		if(pMsg->wParam == VK_RETURN) return TRUE;
	}
        return CDialog::PreTranslateMessage(pMsg);
}

void CPowerCableCreationSettingDlg::OnCbnSelchangeComboAgTemp()
{
	for(int i = 0;i < 8;i++)
	{
		CalcAmbientTempDeratingFactor(i);
	}
}

void CPowerCableCreationSettingDlg::OnCbnSelchangeComboUgTemp()
{
	for(int i = 0;i < 8;i++)
	{
		CalcAmbientTempDeratingFactor(i);
	}
}

/**
	@brief  현재 선택된 CABLE DATA TABLE을 POJECT SETTING TABLE에 저장한다.

	@author KHS,BHK
*/
int CPowerCableCreationSettingDlg::SaveSelectCabledataTable()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CString rProjectMDB = docData.GetProjectMDBFilePath().c_str();
	rProjectMDB.MakeUpper();
	if(FileExist(rProjectMDB))
	{
		CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
		///docData.m_ProjectSettingTable.Load(_T("") , CProjectSettingTable::GetTableName());
		string* pValue = pSettingTable->GetFieldValue(_T("C_CABLEDATA_TABLE"));
		if(NULL == pValue) pSettingTable->AddField(_T("C_CABLEDATA_TABLE"));

		CString rSelTableName;
		int nItem = m_SelectCableDataTableComboBox.GetCurSel();
		if(-1 != nItem)	m_SelectCableDataTableComboBox.GetLBText(nItem, rSelTableName);

		pSettingTable->SetFieldValue(_T("C_CABLEDATA_TABLE") , rSelTableName.operator LPCSTR());
		pSettingTable->Save();

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	POWER CABLE CREATION SETTING을 저장한다.

	@author KHS
*/
int CPowerCableCreationSettingDlg::SavePowerCableCreationSettingSettingData(void)
{
	UpdateData(TRUE);

	CString rAgTemp, rUgTemp, rApplyByAmbientTempFactor, rApplyMinLVPowerCableSize, rApplyMinLVPowerCableSizeCheck, rApplyByFaultCurrentAtHVPowerCableCheck,
		rLoadPowerCableCreationSetting, rEqBusPowerCableCreationSetting, rLoadAllPowerCable, rLoadUnsizedPowerCableOnly, rEqBusAllPowerCable,
		rEqBusUnsizedPowerCableOnly, rConduitLength, rPowerCableSizeOfFaultCurrent, rNumberOfConductors1,rNumberOfConductors2,
		rNumberOfConductorsOver2, rConduitType, rCableVoltage, rCableType, rCableCore, rCableSize, rCableScheduleSpec;

	CString rRatingCapacityAbove;
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CPowerCableCreationSettingTable* pPowerCableCreationSettingTable = static_cast<CPowerCableCreationSettingTable*>(docData.GetTableOf(CPowerCableCreationSettingTable::GetTableName()));
	if(pPowerCableCreationSettingTable)
	{
		int i = 1;
		for(vector<CCableCreationTable::Record>::iterator itr = pPowerCableCreationSettingTable->m_RecordEntry.begin();itr != pPowerCableCreationSettingTable->m_RecordEntry.end();++itr,++i)
		{
			if(i == 1)
			{
				int nCurSel = m_wndAGTempCombo.GetCurSel();
				if(-1 != nCurSel) m_wndAGTempCombo.GetLBText(nCurSel, rAgTemp);

				nCurSel = m_wndUGTempCombo.GetCurSel();
				if(-1 != nCurSel) m_wndUGTempCombo.GetLBText(nCurSel, rUgTemp);

				rApplyByAmbientTempFactor =  (TRUE == m_bApplyByAmbientTempFactor) ? _T("Yes") : _T("No");
				
				CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_APPLY_MINIMUM_LV_POWER_CABLE_SIZE);
				rApplyMinLVPowerCableSizeCheck = (TRUE == pButton->GetCheck()) ? _T("Yes") : _T("No");

				pButton = (CButton*)GetDlgItem(IDC_CHECK_APPLY_BY_FAULT_CURRENT_AT_HV_POWER_CABLE);
				rApplyByFaultCurrentAtHVPowerCableCheck = (TRUE == pButton->GetCheck()) ? _T("Yes") : _T("No");

				CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_CONDUIT_LENGTH);
				pEdit->GetWindowText(rConduitLength);

				pEdit = (CEdit*)GetDlgItem(IDC_EDIT_POWER_CABLE_SIZING_OF_FAULT_CURRENT);
				pEdit->GetWindowText(rPowerCableSizeOfFaultCurrent);

				pEdit = (CEdit*)GetDlgItem(IDC_EDIT_NUMBER_OF_CONDUCTORS_1);
				pEdit->GetWindowText(rNumberOfConductors1);

				pEdit = (CEdit*)GetDlgItem(IDC_EDIT_NUMBER_OF_CONDUCTORS_2);
				pEdit->GetWindowText(rNumberOfConductors2);

				pEdit = (CEdit*)GetDlgItem(IDC_EDIT_NUMBER_OF_CONDUCTORS_OVER_2);
				pEdit->GetWindowText(rNumberOfConductorsOver2);

				CComboBox *pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_CONDUIT_TYPE);
				nCurSel = pCombo->GetCurSel();
				if(-1 != nCurSel)pCombo->GetLBText(nCurSel, rConduitType);

				pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_APPLY_MIN_LV_POWER_CABLE_SIZE);
				nCurSel = pCombo->GetCurSel();
				if(-1 != nCurSel)pCombo->GetLBText(nCurSel, rApplyMinLVPowerCableSize);

				//! SPACE HEATER CAPACITY TYPE
				pEdit = (CEdit*)GetDlgItem(IDC_EDIT_RATING_CAPACITY_ABOVE);
				pEdit->GetWindowText(rRatingCapacityAbove);

				nCurSel = m_CableVoltageComboBox.GetCurSel();
				if(-1 != nCurSel) m_CableVoltageComboBox.GetLBText(nCurSel, rCableVoltage);

				nCurSel = m_CableTypeComboBox.GetCurSel();
				if( -1 != nCurSel) m_CableTypeComboBox.GetLBText(nCurSel, rCableType);

				nCurSel = m_CableCoreComboBox.GetCurSel();
				if( -1 != nCurSel) m_CableCoreComboBox.GetLBText(nCurSel, rCableCore);
				
				nCurSel = m_CableSizeComboBox.GetCurSel();
				if( -1 != nCurSel) m_CableSizeComboBox.GetLBText(nCurSel, rCableSize);
				
				m_CableScheduleSpecEdit.GetWindowText(rCableScheduleSpec);
			}

			itr->FieldValueMap["C_AMBIENT_TEMP_FACTOR"]		= rApplyByAmbientTempFactor.operator LPCSTR();
			itr->FieldValueMap["C_AG_TEMP"]				= rAgTemp.operator LPCSTR();
			itr->FieldValueMap["C_UG_TEMP"]				= rUgTemp.operator LPCSTR();
			itr->FieldValueMap["C_CONDUIT_LENGTH"]			= rConduitLength.operator LPCSTR();
			itr->FieldValueMap["C_CONDUIT_TYPE"]			= rConduitType.operator LPCSTR();
			itr->FieldValueMap["C_NUMBER_OF_CONDUCTORS_1"]		= rNumberOfConductors1.operator LPCSTR();
			itr->FieldValueMap["C_NUMBER_OF_CONDUCTORS_2"]		= rNumberOfConductors2.operator LPCSTR();
			itr->FieldValueMap["C_NUMBER_OF_CONDUCTORS_OVER2"]	= rNumberOfConductorsOver2.operator LPCSTR();
			itr->FieldValueMap["C_MIN_LV_CHECK"]			= rApplyMinLVPowerCableSizeCheck.operator LPCSTR();
			itr->FieldValueMap["C_MIN_LV"]				= rApplyMinLVPowerCableSize.operator LPCSTR();
			itr->FieldValueMap["C_FAULT_CURRENT_AT_HV_CHECK"]	= rApplyByFaultCurrentAtHVPowerCableCheck.operator LPCSTR();
			itr->FieldValueMap["C_FAULT_CURRENT_AT_HV"]		= rPowerCableSizeOfFaultCurrent.operator LPCSTR();
			
			//! SPACE HEATER CAPACITY TYPE
			itr->FieldValueMap["C_RATING_CAPACITY_ABOVE"]            = rRatingCapacityAbove.operator LPCSTR();
			itr->FieldValueMap["C_SPACE_HEATER_CABLE_TYPE"]          = rCableType.operator LPCSTR();
			itr->FieldValueMap["C_SPACE_HEATER_CABLE_VOLTAGE"]       = rCableVoltage.operator LPCSTR();
			itr->FieldValueMap["C_SPACE_HEATER_CABLE_CORE"]          = rCableCore.operator LPCSTR();
			itr->FieldValueMap["C_SPACE_HEATER_CABLE_SCHEDULE_SPEC"] = rCableScheduleSpec.operator LPCSTR();
			itr->FieldValueMap["C_SPACE_HEATER_CABLE_SIZE"]          = rCableSize.operator LPCSTR();
			
			if(1 == i) break;
		}
		pPowerCableCreationSettingTable->Update();

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	POWER CABLE CREATION SETTING 내용을 화면에 표시한다.
	
	@author BHK
*/
int CPowerCableCreationSettingDlg::DisplayCableCreationSettingData(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CPowerCableCreationSettingTable* pPowerCableCreationSettingTable = static_cast<CPowerCableCreationSettingTable*>(docData.GetTableOf(CPowerCableCreationSettingTable::GetTableName()));
	if(pPowerCableCreationSettingTable)
	{
		map<string,string>::iterator where = pPowerCableCreationSettingTable->m_RecordEntry.front().FieldValueMap.find(_T("C_CONDUIT_TYPE"));
		if(where != pPowerCableCreationSettingTable->m_RecordEntry.front().FieldValueMap.end())
		{
			CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_CONDUIT_TYPE);
			const int nFound = pComboBox->FindString(-1 , where->second.c_str());
			if(-1 != nFound) pComboBox->SetCurSel(nFound);
		}

		where = pPowerCableCreationSettingTable->m_RecordEntry.front().FieldValueMap.find(_T("C_AG_TEMP"));
		if(where != pPowerCableCreationSettingTable->m_RecordEntry.front().FieldValueMap.end())
		{
			const int nFound = m_wndAGTempCombo.FindString(-1 , where->second.c_str());
			if(-1 != nFound) m_wndAGTempCombo.SetCurSel(nFound);
		}

		where = pPowerCableCreationSettingTable->m_RecordEntry.front().FieldValueMap.find(_T("C_UG_TEMP"));
		if(where != pPowerCableCreationSettingTable->m_RecordEntry.front().FieldValueMap.end())
		{
			const int nFound = m_wndUGTempCombo.FindString(-1 , where->second.c_str());
			if(-1 != nFound) m_wndUGTempCombo.SetCurSel(nFound);
		}

		where = pPowerCableCreationSettingTable->m_RecordEntry.front().FieldValueMap.find(_T("C_MIN_LV"));
		if(where != pPowerCableCreationSettingTable->m_RecordEntry.front().FieldValueMap.end())
		{
			CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_APPLY_MIN_LV_POWER_CABLE_SIZE);
			const int nFound = pComboBox->FindString(-1 , where->second.c_str());
			if(-1 != nFound) pComboBox->SetCurSel(nFound);
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}
/**
	@ brief  power cable Item의 Read Only Setting
	@ author KHS
*/
int CPowerCableCreationSettingDlg::CheckItemAndReadOnly(void)
{
	CString rHv1BusVoltage, rHv2BusVoltage, rHv3BusVoltage, rLv1BusVoltage, rLv2BusVoltage,
		rLv11BusVoltage, rLv12BusVoltage, rDCBusVoltage;
	m_HV1_BusVoltageEdit.GetWindowText(rHv1BusVoltage);
	if(rHv1BusVoltage.IsEmpty())
	{
		GetDlgItem(IDC_HV1_VOLTAGE_TYPE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_HV1_PHASE_STATIC)->EnableWindow(FALSE);
		m_HV1_CableVoltageCombo.EnableWindow(FALSE);
		m_HV1_CableTypeCombo.EnableWindow(FALSE);
		m_HV1_CableScheduleSpecEdit.EnableWindow(FALSE);
		m_HV1_CableCoreCombo.EnableWindow(FALSE);
		m_HV1_MotorRunningEdit.EnableWindow(FALSE);
		m_HV1_MotorStartingEdit.EnableWindow(FALSE);
		m_HV1_ApplyMotorCheck.EnableWindow(FALSE);
		m_HV1_FeederRunningEdit.EnableWindow(FALSE);
		m_HV1_InstallMethodCombo.EnableWindow(FALSE);
		m_HV1_AmbientTempEdit.EnableWindow(FALSE);
		m_HV1_OtherDeratingFactorEdit.EnableWindow(FALSE);
		m_HV1_TotalDeratingFactorEdit.EnableWindow(FALSE);
		m_HV1_SfEdit.EnableWindow(FALSE);
	}
	m_HV2_BusVoltageEdit.GetWindowText(rHv2BusVoltage);
	if(rHv2BusVoltage.IsEmpty())
	{
		GetDlgItem(IDC_HV2_VOLTAGE_TYPE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_HV2_PHASE_STATIC)->EnableWindow(FALSE);
		m_HV2_CableVoltageCombo.EnableWindow(FALSE);
		m_HV2_CableTypeCombo.EnableWindow(FALSE);
		m_HV2_CableScheduleSpecEdit.EnableWindow(FALSE);
		m_HV2_CableCoreCombo.EnableWindow(FALSE);
		m_HV2_MotorRunningEdit.EnableWindow(FALSE);
		m_HV2_MotorStartingEdit.EnableWindow(FALSE);
		m_HV2_ApplyMotorCheck.EnableWindow(FALSE);
		m_HV2_FeederRunningEdit.EnableWindow(FALSE);
		m_HV2_InstallMethodCombo.EnableWindow(FALSE);
		m_HV2_AmbientTempEdit.EnableWindow(FALSE);
		m_HV2_OtherDeratingFactorEdit.EnableWindow(FALSE);
		m_HV2_TotalDeratingFactorEdit.EnableWindow(FALSE);
		m_HV2_SfEdit.EnableWindow(FALSE);
	}
	m_HV3_BusVoltageEdit.GetWindowText(rHv3BusVoltage);
	if(rHv3BusVoltage.IsEmpty())
	{
		GetDlgItem(IDC_HV3_VOLTAGE_TYPE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_HV3_PHASE_STATIC)->EnableWindow(FALSE);
		m_HV3_CableVoltageCombo.EnableWindow(FALSE);
		m_HV3_CableTypeCombo.EnableWindow(FALSE);
		m_HV3_CableScheduleSpecEdit.EnableWindow(FALSE);
		m_HV3_CableCoreCombo.EnableWindow(FALSE);
		m_HV3_MotorRunningEdit.EnableWindow(FALSE);
		m_HV3_MotorStartingEdit.EnableWindow(FALSE);
		m_HV3_ApplyMotorCheck.EnableWindow(FALSE);
		m_HV3_FeederRunningEdit.EnableWindow(FALSE);
		m_HV3_InstallMethodCombo.EnableWindow(FALSE);
		m_HV3_AmbientTempEdit.EnableWindow(FALSE);
		m_HV3_OtherDeratingFactorEdit.EnableWindow(FALSE);
		m_HV3_TotalDeratingFactorEdit.EnableWindow(FALSE);
		m_HV3_SfEdit.EnableWindow(FALSE);
	}
	m_LV1_BusVoltageEdit.GetWindowText(rLv1BusVoltage);
	if(rLv1BusVoltage.IsEmpty())
	{
		GetDlgItem(IDC_LV1_VOLTAGE_TYPE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_LV1_3PH_PHASE_STATIC)->EnableWindow(FALSE);
		m_LV1_CableVoltageCombo.EnableWindow(FALSE);
		m_LV1_CableTypeCombo.EnableWindow(FALSE);
		m_LV1_CableScheduleSpecEdit.EnableWindow(FALSE);
		m_LV1_CableCoreCombo.EnableWindow(FALSE);
		m_LV1_MotorRunningEdit.EnableWindow(FALSE);
		m_LV1_MotorStartingEdit.EnableWindow(FALSE);
		m_LV1_ApplyMotorCheck.EnableWindow(FALSE);
		m_LV1_FeederRunningEdit.EnableWindow(FALSE);
		m_LV1_InstallMethodCombo.EnableWindow(FALSE);
		m_LV1_AmbientTempEdit.EnableWindow(FALSE);
		m_LV1_OtherDeratingFactorEdit.EnableWindow(FALSE);
		m_LV1_TotalDeratingFactorEdit.EnableWindow(FALSE);
		m_LV1_SfEdit.EnableWindow(FALSE);
	}
	m_LV2_BusVoltageEdit.GetWindowText(rLv2BusVoltage);
	if(rLv2BusVoltage.IsEmpty())
	{
		GetDlgItem(IDC_LV2_VOLTAGE_TYPE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_LV2_3PH_PHASE_STATIC)->EnableWindow(FALSE);
		m_LV2_CableVoltageCombo.EnableWindow(FALSE);
		m_LV2_CableTypeCombo.EnableWindow(FALSE);
		m_LV2_CableScheduleSpecEdit.EnableWindow(FALSE);
		m_LV2_CableCoreCombo.EnableWindow(FALSE);
		m_LV2_MotorRunningEdit.EnableWindow(FALSE);
		m_LV2_MotorStartingEdit.EnableWindow(FALSE);
		m_LV2_ApplyMotorCheck.EnableWindow(FALSE);
		m_LV2_FeederRunningEdit.EnableWindow(FALSE);
		m_LV2_InstallMethodCombo.EnableWindow(FALSE);
		m_LV2_AmbientTempEdit.EnableWindow(FALSE);
		m_LV2_OtherDeratingFactorEdit.EnableWindow(FALSE);
		m_LV2_TotalDeratingFactorEdit.EnableWindow(FALSE);
		m_LV2_SfEdit.EnableWindow(FALSE);
	}
	m_LV11_BusVoltageEdit.GetWindowText(rLv11BusVoltage);
	if(rLv11BusVoltage.IsEmpty())
	{
		GetDlgItem(IDC_LV11_VOLTAGE_TYPE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_LV11_1PH_PHASE_STATIC)->EnableWindow(FALSE);
		m_LV11_CableVoltageCombo.EnableWindow(FALSE);
		m_LV11_CableTypeCombo.EnableWindow(FALSE);
		m_LV11_CableScheduleSpecEdit.EnableWindow(FALSE);
		m_LV11_CableCoreCombo.EnableWindow(FALSE);
		m_LV11_FeederRunningEdit.EnableWindow(FALSE);
		m_LV11_InstallMethodCombo.EnableWindow(FALSE);
		m_LV11_AmbientTempEdit.EnableWindow(FALSE);
		m_LV11_OtherDeratingFactorEdit.EnableWindow(FALSE);
		m_LV11_TotalDeratingFactorEdit.EnableWindow(FALSE);
		m_LV11_SfEdit.EnableWindow(FALSE);
	}
	m_LV12_BusVoltageEdit.GetWindowText(rLv12BusVoltage);
	if(rLv12BusVoltage.IsEmpty())
	{
		GetDlgItem(IDC_LV12_VOLTAGE_TYPE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_LV12_1PH_PHASE_STATIC)->EnableWindow(FALSE);
		m_LV12_CableVoltageCombo.EnableWindow(FALSE);
		m_LV12_CableTypeCombo.EnableWindow(FALSE);
		m_LV12_CableScheduleSpecEdit.EnableWindow(FALSE);
		m_LV12_CableCoreCombo.EnableWindow(FALSE);
		m_LV12_FeederRunningEdit.EnableWindow(FALSE);
		m_LV12_InstallMethodCombo.EnableWindow(FALSE);
		m_LV12_AmbientTempEdit.EnableWindow(FALSE);
		m_LV12_OtherDeratingFactorEdit.EnableWindow(FALSE);
		m_LV12_TotalDeratingFactorEdit.EnableWindow(FALSE);
		m_LV12_SfEdit.EnableWindow(FALSE);
	}
	m_DC_BusVoltageEdit.GetWindowText(rDCBusVoltage);
	if(rDCBusVoltage.IsEmpty())
	{
		GetDlgItem(IDC_DC_VOLTAGE_TYPE_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_DC_PHASE_STATIC)->EnableWindow(FALSE);
		m_DC_CableVoltageCombo.EnableWindow(FALSE);
		m_DC_CableTypeCombo.EnableWindow(FALSE);
		m_DC_CableScheduleSpecEdit.EnableWindow(FALSE);
		m_DC_CableCoreCombo.EnableWindow(FALSE);
		m_DC_FeederRunningEdit.EnableWindow(FALSE);
		m_DC_InstallMethodCombo.EnableWindow(FALSE);
		m_DC_AmbientTempEdit.EnableWindow(FALSE);
		m_DC_OtherDeratingFactorEdit.EnableWindow(FALSE);
		m_DC_TotalDeratingFactorEdit.EnableWindow(FALSE);
		m_DC_SfEdit.EnableWindow(FALSE);
	}
	return ERROR_SUCCESS;
}

/**
	@brief	HV Cable Setting Data를 화면에 표시합니다.

	@author BHK	

	@date 2009-04-01 오전 10:06:02	

	@param	

	@return		
*/
int CPowerCableCreationSettingDlg::DisplayHVCableData(void)
{
	CHVCableCreationSettingTable& HVVoltageSettingTable = CHVCableCreationSettingTable::GetInstance();
	int nRow = 1;
	for(vector<CCableCreationTable::Record>::iterator itr = HVVoltageSettingTable.m_RecordEntry.begin();itr != HVVoltageSettingTable.m_RecordEntry.end();++itr,++nRow)
	{
		if(1 == nRow )
		{
			m_HV1_BusVoltageEdit.SetWindowText(itr->FieldValueMap["C_BUS_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_BUS_VOLTAGE"].c_str());
			m_HV1_LoadVoltageEdit.SetWindowText(itr->FieldValueMap["C_LOAD_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_LOAD_VOLTAGE"].c_str());

			int nSelItem = m_HV1_CableVoltageCombo.FindString(-1, itr->FieldValueMap["C_CABLE_VOLTAGE"].c_str());
			if(-1 != nSelItem)
			{
				m_HV1_CableVoltageCombo.SetCurSel(nSelItem);
				OnCbnSelchangeHv1CableVoltageCombo();
			}
			nSelItem =  m_HV1_CableTypeCombo.FindString(-1 , itr->FieldValueMap["C_CABLE_TYPE"].c_str());
			if( -1 != nSelItem) 
			{
				m_HV1_CableTypeCombo.SetCurSel(nSelItem);
				OnCbnSelchangeHv1CableTypeCombo();
			}
			nSelItem = m_HV1_CableCoreCombo.FindString(-1 , itr->FieldValueMap["C_CABLE_CORE"].c_str());
			if( -1 != nSelItem)
			{
				m_HV1_CableCoreCombo.SetCurSel(nSelItem);
			}

			m_HV1_CableScheduleSpecEdit.SetWindowText(itr->FieldValueMap["C_CABLE_SCHEDULE_SPEC"].empty() ? "" : itr->FieldValueMap["C_CABLE_SCHEDULE_SPEC"].c_str());
			m_HV1_MotorRunningEdit.SetWindowText(itr->FieldValueMap["C_MOTOR_RUNNING"].empty() ? "" : itr->FieldValueMap["C_MOTOR_RUNNING"].c_str());
			m_HV1_MotorStartingEdit.SetWindowText(itr->FieldValueMap["C_MOTOR_STARTING"].empty() ? "" : itr->FieldValueMap["C_MOTOR_STARTING"].c_str());

			CButton* pButton = (CButton*)GetDlgItem(IDC_HV1_APPLY_MOTOR_STARTING_CHECK);
			(_T("Yes") == itr->FieldValueMap[_T("C_APPLY_MOTOR_STARTING")]) ? pButton->SetCheck(TRUE) : pButton->SetCheck(FALSE);

			m_HV1_FeederRunningEdit.SetWindowText(itr->FieldValueMap["C_FEEDER_RUNNING"].empty() ? "" : itr->FieldValueMap["C_FEEDER_RUNNING"].c_str());
			nSelItem = m_HV1_InstallMethodCombo.FindString(-1, itr->FieldValueMap["C_INSTALL_METHOD"].c_str());
			if(-1 != nSelItem) m_HV1_InstallMethodCombo.SetCurSel(nSelItem);
			else m_HV1_InstallMethodCombo.SetWindowText(itr->FieldValueMap["C_INSTALL_METHOD"].empty() ? "" : itr->FieldValueMap["C_INSTALL_METHOD"].c_str());
			m_HV1_AmbientTempEdit.SetWindowText(itr->FieldValueMap["C_AMBIENT_TEMP_DERATING_FACTOR"].empty() ? "" : itr->FieldValueMap["C_AMBIENT_TEMP_DERATING_FACTOR"].c_str());
			m_HV1_OtherDeratingFactorEdit.SetWindowText(itr->FieldValueMap["C_OTHERS_DERATING_FACTOR"].empty() ? "" : itr->FieldValueMap["C_OTHERS_DERATING_FACTOR"].c_str());
			m_HV1_TotalDeratingFactorEdit.SetWindowText(itr->FieldValueMap["C_TOTAL_DERATING_FACTOR"].empty() ? "" : itr->FieldValueMap["C_TOTAL_DERATING_FACTOR"].c_str());
			m_HV1_SfEdit.SetWindowText(itr->FieldValueMap["C_SF"].empty() ? "" : itr->FieldValueMap["C_SF"].c_str());
		}
		else if( 2 == nRow)
		{
			m_HV2_BusVoltageEdit.SetWindowText(itr->FieldValueMap["C_BUS_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_BUS_VOLTAGE"].c_str());
			m_HV2_LoadVoltageEdit.SetWindowText(itr->FieldValueMap["C_LOAD_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_LOAD_VOLTAGE"].c_str());

			int nSelItem = m_HV2_CableVoltageCombo.FindString(-1, itr->FieldValueMap["C_CABLE_VOLTAGE"].c_str());
			if(-1 != nSelItem)
			{
				m_HV2_CableVoltageCombo.SetCurSel(nSelItem);
				OnCbnSelchangeHv2CableVoltageCombo();
			}
			nSelItem =  m_HV2_CableTypeCombo.FindString(-1 , itr->FieldValueMap["C_CABLE_TYPE"].c_str());
			if( -1 != nSelItem) 
			{
				m_HV2_CableTypeCombo.SetCurSel(nSelItem);
				OnCbnSelchangeHv2CableTypeCombo();
			}
			nSelItem = m_HV2_CableCoreCombo.FindString(-1 , itr->FieldValueMap["C_CABLE_CORE"].c_str());
			if( -1 != nSelItem)
			{
				m_HV2_CableCoreCombo.SetCurSel(nSelItem);
			}
			m_HV2_CableScheduleSpecEdit.SetWindowText(itr->FieldValueMap["C_CABLE_SCHEDULE_SPEC"].empty() ? "" : itr->FieldValueMap["C_CABLE_SCHEDULE_SPEC"].c_str());
			m_HV2_MotorRunningEdit.SetWindowText(itr->FieldValueMap["C_MOTOR_RUNNING"].empty() ? "" : itr->FieldValueMap["C_MOTOR_RUNNING"].c_str());
			m_HV2_MotorStartingEdit.SetWindowText(itr->FieldValueMap["C_MOTOR_STARTING"].empty() ? "" : itr->FieldValueMap["C_MOTOR_STARTING"].c_str());

			CButton* pButton = (CButton*)GetDlgItem(IDC_HV2_APPLY_MOTOR_STARTING_CHECK);
			(_T("Yes") == itr->FieldValueMap[_T("C_APPLY_MOTOR_STARTING")]) ? pButton->SetCheck(TRUE) : pButton->SetCheck(FALSE);

			m_HV2_FeederRunningEdit.SetWindowText(itr->FieldValueMap["C_FEEDER_RUNNING"].empty() ? "" : itr->FieldValueMap["C_FEEDER_RUNNING"].c_str());

			nSelItem = m_HV2_InstallMethodCombo.FindString(-1, itr->FieldValueMap["C_INSTALL_METHOD"].c_str());
			if(-1 != nSelItem) m_HV2_InstallMethodCombo.SetCurSel(nSelItem);
			else m_HV2_InstallMethodCombo.SetWindowText(itr->FieldValueMap["C_INSTALL_METHOD"].empty() ? "" : itr->FieldValueMap["C_INSTALL_METHOD"].c_str());
			m_HV2_AmbientTempEdit.SetWindowText(itr->FieldValueMap["C_AMBIENT_TEMP_DERATING_FACTOR"].empty() ? "" : itr->FieldValueMap["C_AMBIENT_TEMP_DERATING_FACTOR"].c_str());
			m_HV2_OtherDeratingFactorEdit.SetWindowText(itr->FieldValueMap["C_OTHERS_DERATING_FACTOR"].empty() ? "" : itr->FieldValueMap["C_OTHERS_DERATING_FACTOR"].c_str());
			m_HV2_TotalDeratingFactorEdit.SetWindowText(itr->FieldValueMap["C_TOTAL_DERATING_FACTOR"].empty() ? "" : itr->FieldValueMap["C_TOTAL_DERATING_FACTOR"].c_str());
			m_HV2_SfEdit.SetWindowText(itr->FieldValueMap["C_SF"].empty() ? "" : itr->FieldValueMap["C_SF"].c_str());
		}
		else if(3 == nRow)
		{
			m_HV3_BusVoltageEdit.SetWindowText(itr->FieldValueMap["C_BUS_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_BUS_VOLTAGE"].c_str());
			m_HV3_LoadVoltageEdit.SetWindowText(itr->FieldValueMap["C_LOAD_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_LOAD_VOLTAGE"].c_str());

			int nSelItem = m_HV3_CableVoltageCombo.FindString(-1, itr->FieldValueMap["C_CABLE_VOLTAGE"].c_str());
			if(-1 != nSelItem)
			{
				m_HV3_CableVoltageCombo.SetCurSel(nSelItem);
				OnCbnSelchangeHv3CableVoltageCombo();
			}
			nSelItem =  m_HV3_CableTypeCombo.FindString(-1 , itr->FieldValueMap["C_CABLE_TYPE"].c_str());
			if( -1 != nSelItem) 
			{
				m_HV3_CableTypeCombo.SetCurSel(nSelItem);
				OnCbnSelchangeHv3CableTypeCombo();
			}
			nSelItem = m_HV3_CableCoreCombo.FindString(-1 , itr->FieldValueMap["C_CABLE_CORE"].c_str());
			if( -1 != nSelItem)
			{
				m_HV3_CableCoreCombo.SetCurSel(nSelItem);
			}

			m_HV3_CableScheduleSpecEdit.SetWindowText(itr->FieldValueMap["C_CABLE_SCHEDULE_SPEC"].empty() ? "" : itr->FieldValueMap["C_CABLE_SCHEDULE_SPEC"].c_str());
			m_HV3_MotorRunningEdit.SetWindowText(itr->FieldValueMap["C_MOTOR_RUNNING"].empty() ? "" : itr->FieldValueMap["C_MOTOR_RUNNING"].c_str());
			m_HV3_MotorStartingEdit.SetWindowText(itr->FieldValueMap["C_MOTOR_STARTING"].empty() ? "" : itr->FieldValueMap["C_MOTOR_STARTING"].c_str());

			CButton* pButton = (CButton*)GetDlgItem(IDC_HV3_APPLY_MOTOR_STARTING_CHECK);
			(_T("Yes") == itr->FieldValueMap[_T("C_APPLY_MOTOR_STARTING")]) ? pButton->SetCheck(TRUE) : pButton->SetCheck(FALSE);

			m_HV3_FeederRunningEdit.SetWindowText(itr->FieldValueMap["C_FEEDER_RUNNING"].empty() ? "" : itr->FieldValueMap["C_FEEDER_RUNNING"].c_str());

			nSelItem = m_HV3_InstallMethodCombo.FindString(-1, itr->FieldValueMap["C_INSTALL_METHOD"].c_str());
			if(-1 != nSelItem) m_HV3_InstallMethodCombo.SetCurSel(nSelItem);
			else m_HV3_InstallMethodCombo.SetWindowText(itr->FieldValueMap["C_INSTALL_METHOD"].empty() ? "" : itr->FieldValueMap["C_INSTALL_METHOD"].c_str());
			m_HV3_AmbientTempEdit.SetWindowText(itr->FieldValueMap["C_AMBIENT_TEMP_DERATING_FACTOR"].empty() ? "" : itr->FieldValueMap["C_AMBIENT_TEMP_DERATING_FACTOR"].c_str());
			m_HV3_OtherDeratingFactorEdit.SetWindowText(itr->FieldValueMap["C_OTHERS_DERATING_FACTOR"].empty() ? "" : itr->FieldValueMap["C_OTHERS_DERATING_FACTOR"].c_str());
			m_HV3_TotalDeratingFactorEdit.SetWindowText(itr->FieldValueMap["C_TOTAL_DERATING_FACTOR"].empty() ? "" : itr->FieldValueMap["C_TOTAL_DERATING_FACTOR"].c_str());
			m_HV3_SfEdit.SetWindowText(itr->FieldValueMap["C_SF"].empty() ? "" : itr->FieldValueMap["C_SF"].c_str());
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	LV3PH Cable Setting Data를 화면에 표시합니다.

	@author BHK	

	@date 2009-04-01 오전 10:07:35	

	@param	

	@return		
*/
int CPowerCableCreationSettingDlg::DisplayLV3PHCableData(void)
{
	CLV3PHCableCreationSettingTable& LV3PHVoltageSettingTable = CLV3PHCableCreationSettingTable::GetInstance();

	int nRow = 1;
	for(vector<CCableCreationTable::Record>::iterator itr = LV3PHVoltageSettingTable.m_RecordEntry.begin();itr != LV3PHVoltageSettingTable.m_RecordEntry.end();++itr,++nRow)
	{
		if(1 == nRow)
		{
			m_LV1_BusVoltageEdit.SetWindowText(itr->FieldValueMap["C_BUS_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_BUS_VOLTAGE"].c_str());
			m_LV1_LoadVoltageEdit.SetWindowText(itr->FieldValueMap["C_LOAD_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_LOAD_VOLTAGE"].c_str());

			int nSelItem = m_LV1_CableVoltageCombo.FindString(-1, itr->FieldValueMap["C_CABLE_VOLTAGE"].c_str());
			if(-1 != nSelItem)
			{
				m_LV1_CableVoltageCombo.SetCurSel(nSelItem);
				OnCbnSelchangeLv1CableVoltageCombo();
			}
			nSelItem =  m_LV1_CableTypeCombo.FindString(-1 , itr->FieldValueMap["C_CABLE_TYPE"].c_str());
			if( -1 != nSelItem) 
			{
				m_LV1_CableTypeCombo.SetCurSel(nSelItem);
				OnCbnSelchangeLv1CableTypeCombo();
			}
			nSelItem = m_LV1_CableCoreCombo.FindString(-1 , itr->FieldValueMap["C_CABLE_CORE"].c_str());
			if( -1 != nSelItem)
			{
				m_LV1_CableCoreCombo.SetCurSel(nSelItem);
			}
			m_LV1_CableScheduleSpecEdit.SetWindowText(itr->FieldValueMap["C_CABLE_SCHEDULE_SPEC"].empty() ? "" : itr->FieldValueMap["C_CABLE_SCHEDULE_SPEC"].c_str());

			m_LV1_MotorRunningEdit.SetWindowText(itr->FieldValueMap["C_MOTOR_RUNNING"].empty() ? "" : itr->FieldValueMap["C_MOTOR_RUNNING"].c_str());
			m_LV1_MotorStartingEdit.SetWindowText(itr->FieldValueMap["C_MOTOR_STARTING"].empty() ? "" : itr->FieldValueMap["C_MOTOR_STARTING"].c_str());

			CButton* pButton = (CButton*)GetDlgItem(IDC_LV1_APPLY_MOTOR_STARTING_CHECK);
			(_T("Yes") == itr->FieldValueMap[_T("C_APPLY_MOTOR_STARTING")]) ? pButton->SetCheck(TRUE) : pButton->SetCheck(FALSE);

			m_LV1_FeederRunningEdit.SetWindowText(itr->FieldValueMap["C_FEEDER_RUNNING"].empty() ? "" : itr->FieldValueMap["C_FEEDER_RUNNING"].c_str());

			nSelItem = m_LV1_InstallMethodCombo.FindString(-1, itr->FieldValueMap["C_INSTALL_METHOD"].c_str());
			if(-1 != nSelItem) m_LV1_InstallMethodCombo.SetCurSel(nSelItem);
			else m_LV1_InstallMethodCombo.SetWindowText(itr->FieldValueMap["C_INSTALL_METHOD"].empty() ? "" : itr->FieldValueMap["C_INSTALL_METHOD"].c_str());
			m_LV1_AmbientTempEdit.SetWindowText(itr->FieldValueMap["C_AMBIENT_TEMP_DERATING_FACTOR"].empty() ? "" : itr->FieldValueMap["C_AMBIENT_TEMP_DERATING_FACTOR"].c_str());
			m_LV1_OtherDeratingFactorEdit.SetWindowText(itr->FieldValueMap["C_OTHERS_DERATING_FACTOR"].empty() ? "" : itr->FieldValueMap["C_OTHERS_DERATING_FACTOR"].c_str());
			m_LV1_TotalDeratingFactorEdit.SetWindowText(itr->FieldValueMap["C_TOTAL_DERATING_FACTOR"].empty() ? "" : itr->FieldValueMap["C_TOTAL_DERATING_FACTOR"].c_str());
			m_LV1_SfEdit.SetWindowText(itr->FieldValueMap["C_SF"].empty() ? "" : itr->FieldValueMap["C_SF"].c_str());
		}
		else if(2 == nRow)
		{
			m_LV2_BusVoltageEdit.SetWindowText(itr->FieldValueMap["C_BUS_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_BUS_VOLTAGE"].c_str());
			m_LV2_LoadVoltageEdit.SetWindowText(itr->FieldValueMap["C_LOAD_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_LOAD_VOLTAGE"].c_str());

			int nSelItem = m_LV2_CableVoltageCombo.FindString(-1, itr->FieldValueMap["C_CABLE_VOLTAGE"].c_str());
			if(-1 != nSelItem)
			{
				m_LV2_CableVoltageCombo.SetCurSel(nSelItem);
				OnCbnSelchangeLv2CableVoltageCombo();
			}
			nSelItem =  m_LV2_CableTypeCombo.FindString(-1 , itr->FieldValueMap["C_CABLE_TYPE"].c_str());
			if( -1 != nSelItem) 
			{
				m_LV2_CableTypeCombo.SetCurSel(nSelItem);
				OnCbnSelchangeLv2CableTypeCombo();
			}
			nSelItem = m_LV2_CableCoreCombo.FindString(-1 , itr->FieldValueMap["C_CABLE_CORE"].c_str());
			if( -1 != nSelItem)
			{
				m_LV2_CableCoreCombo.SetCurSel(nSelItem);
			}
			m_LV2_CableScheduleSpecEdit.SetWindowText(itr->FieldValueMap["C_CABLE_SCHEDULE_SPEC"].empty() ? "" : itr->FieldValueMap["C_CABLE_SCHEDULE_SPEC"].c_str());
			m_LV2_MotorRunningEdit.SetWindowText(itr->FieldValueMap["C_MOTOR_RUNNING"].empty() ? "" : itr->FieldValueMap["C_MOTOR_RUNNING"].c_str());
			m_LV2_MotorStartingEdit.SetWindowText(itr->FieldValueMap["C_MOTOR_STARTING"].empty() ? "" : itr->FieldValueMap["C_MOTOR_STARTING"].c_str());

			CButton* pButton = (CButton*)GetDlgItem(IDC_LV2_APPLY_MOTOR_STARTING_CHECK);
			(_T("Yes") == itr->FieldValueMap[_T("C_APPLY_MOTOR_STARTING")]) ? pButton->SetCheck(TRUE) : pButton->SetCheck(FALSE);

			m_LV2_FeederRunningEdit.SetWindowText(itr->FieldValueMap["C_FEEDER_RUNNING"].empty() ? "" : itr->FieldValueMap["C_FEEDER_RUNNING"].c_str());
			nSelItem = m_LV2_InstallMethodCombo.FindString(-1, itr->FieldValueMap["C_INSTALL_METHOD"].c_str());
			if(-1 != nSelItem) m_LV2_InstallMethodCombo.SetCurSel(nSelItem);
			else m_LV2_InstallMethodCombo.SetWindowText(itr->FieldValueMap["C_INSTALL_METHOD"].empty() ? "" : itr->FieldValueMap["C_INSTALL_METHOD"].c_str());
			m_LV2_AmbientTempEdit.SetWindowText(itr->FieldValueMap["C_AMBIENT_TEMP_DERATING_FACTOR"].empty() ? "" : itr->FieldValueMap["C_AMBIENT_TEMP_DERATING_FACTOR"].c_str());
			m_LV2_OtherDeratingFactorEdit.SetWindowText(itr->FieldValueMap["C_OTHERS_DERATING_FACTOR"].empty() ? "" : itr->FieldValueMap["C_OTHERS_DERATING_FACTOR"].c_str());
			m_LV2_TotalDeratingFactorEdit.SetWindowText(itr->FieldValueMap["C_TOTAL_DERATING_FACTOR"].empty() ? "" : itr->FieldValueMap["C_TOTAL_DERATING_FACTOR"].c_str());
			m_LV2_SfEdit.SetWindowText(itr->FieldValueMap["C_SF"].empty() ? "" : itr->FieldValueMap["C_SF"].c_str());
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	LV1PH Cable Setting Data를 화면에 표시합니다.

	@author BHK	

	@date 2009-04-01 오전 10:11:35	

	@param	

	@return		
*/
int CPowerCableCreationSettingDlg::DisplayLV1PHCableData(void)
{
	CLV1PHCableCreationSettingTable& LV1PHVoltageSettingTable = CLV1PHCableCreationSettingTable::GetInstance();
	int nRow = 1;
	for(vector<CCableCreationTable::Record>::iterator itr = LV1PHVoltageSettingTable.m_RecordEntry.begin();itr != LV1PHVoltageSettingTable.m_RecordEntry.end();++itr,++nRow)
	{
		if(1 == nRow)
		{
			m_LV11_BusVoltageEdit.SetWindowText(itr->FieldValueMap["C_BUS_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_BUS_VOLTAGE"].c_str());
			m_LV11_LoadVoltageEdit.SetWindowText(itr->FieldValueMap["C_LOAD_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_LOAD_VOLTAGE"].c_str());

			int nSelItem = m_LV11_CableVoltageCombo.FindString(-1, itr->FieldValueMap["C_CABLE_VOLTAGE"].c_str());
			if(-1 != nSelItem)
			{
				m_LV11_CableVoltageCombo.SetCurSel(nSelItem);
				OnCbnSelchangeLv11CableVoltageCombo();
			}
			nSelItem =  m_LV11_CableTypeCombo.FindString(-1 , itr->FieldValueMap["C_CABLE_TYPE"].c_str());
			if( -1 != nSelItem) 
			{
				m_LV11_CableTypeCombo.SetCurSel(nSelItem);
				OnCbnSelchangeLv11CableTypeCombo();
			}
			nSelItem = m_LV11_CableCoreCombo.FindString(-1 , itr->FieldValueMap["C_CABLE_CORE"].c_str());
			if( -1 != nSelItem)
			{
				m_LV11_CableCoreCombo.SetCurSel(nSelItem);
			}
			m_LV11_CableScheduleSpecEdit.SetWindowText(itr->FieldValueMap["C_CABLE_SCHEDULE_SPEC"].empty() ? "" : itr->FieldValueMap["C_CABLE_SCHEDULE_SPEC"].c_str());
			m_LV11_FeederRunningEdit.SetWindowText(itr->FieldValueMap["C_FEEDER_RUNNING"].empty() ? "" : itr->FieldValueMap["C_FEEDER_RUNNING"].c_str());
			nSelItem = m_LV11_InstallMethodCombo.FindString(-1, itr->FieldValueMap["C_INSTALL_METHOD"].c_str());
			if(-1 != nSelItem) m_LV11_InstallMethodCombo.SetCurSel(nSelItem);
			else m_LV11_InstallMethodCombo.SetWindowText(itr->FieldValueMap["C_INSTALL_METHOD"].empty() ? "" : itr->FieldValueMap["C_INSTALL_METHOD"].c_str());
			m_LV11_AmbientTempEdit.SetWindowText(itr->FieldValueMap["C_AMBIENT_TEMP_DERATING_FACTOR"].empty() ? "" : itr->FieldValueMap["C_AMBIENT_TEMP_DERATING_FACTOR"].c_str());
			m_LV11_OtherDeratingFactorEdit.SetWindowText(itr->FieldValueMap["C_OTHERS_DERATING_FACTOR"].empty() ? "" : itr->FieldValueMap["C_OTHERS_DERATING_FACTOR"].c_str());
			m_LV11_TotalDeratingFactorEdit.SetWindowText(itr->FieldValueMap["C_TOTAL_DERATING_FACTOR"].empty() ? "" : itr->FieldValueMap["C_TOTAL_DERATING_FACTOR"].c_str());
			m_LV11_SfEdit.SetWindowText(itr->FieldValueMap["C_SF"].empty() ? "" : itr->FieldValueMap["C_SF"].c_str());
		}
		else if(2 == nRow)
		{
			m_LV12_BusVoltageEdit.SetWindowText(itr->FieldValueMap["C_BUS_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_BUS_VOLTAGE"].c_str());
			m_LV12_LoadVoltageEdit.SetWindowText(itr->FieldValueMap["C_LOAD_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_LOAD_VOLTAGE"].c_str());

			int nSelItem = m_LV12_CableVoltageCombo.FindString(-1, itr->FieldValueMap["C_CABLE_VOLTAGE"].c_str());
			if(-1 != nSelItem)
			{
				m_LV12_CableVoltageCombo.SetCurSel(nSelItem);
				OnCbnSelchangeLv12CableVoltageCombo();
			}
			nSelItem =  m_LV12_CableTypeCombo.FindString(-1 , itr->FieldValueMap["C_CABLE_TYPE"].c_str());
			if( -1 != nSelItem) 
			{
				m_LV12_CableTypeCombo.SetCurSel(nSelItem);
				OnCbnSelchangeLv12CableTypeCombo();
			}
			nSelItem = m_LV12_CableCoreCombo.FindString(-1 , itr->FieldValueMap["C_CABLE_CORE"].c_str());
			if( -1 != nSelItem)
			{
				m_LV12_CableCoreCombo.SetCurSel(nSelItem);
			}
			m_LV12_CableScheduleSpecEdit.SetWindowText(itr->FieldValueMap["C_CABLE_SCHEDULE_SPEC"].empty() ? "" : itr->FieldValueMap["C_CABLE_SCHEDULE_SPEC"].c_str());
			m_LV12_FeederRunningEdit.SetWindowText(itr->FieldValueMap["C_FEEDER_RUNNING"].empty() ? "" : itr->FieldValueMap["C_FEEDER_RUNNING"].c_str());
			nSelItem = m_LV12_InstallMethodCombo.FindString(-1, itr->FieldValueMap["C_INSTALL_METHOD"].c_str());
			if(-1 != nSelItem) m_LV12_InstallMethodCombo.SetCurSel(nSelItem);
			else m_LV12_InstallMethodCombo.SetWindowText(itr->FieldValueMap["C_INSTALL_METHOD"].empty() ? "" : itr->FieldValueMap["C_INSTALL_METHOD"].c_str());
			m_LV12_AmbientTempEdit.SetWindowText(itr->FieldValueMap["C_AMBIENT_TEMP_DERATING_FACTOR"].empty() ? "" : itr->FieldValueMap["C_AMBIENT_TEMP_DERATING_FACTOR"].c_str());
			m_LV12_OtherDeratingFactorEdit.SetWindowText(itr->FieldValueMap["C_OTHERS_DERATING_FACTOR"].empty() ? "" : itr->FieldValueMap["C_OTHERS_DERATING_FACTOR"].c_str());
			m_LV12_TotalDeratingFactorEdit.SetWindowText(itr->FieldValueMap["C_TOTAL_DERATING_FACTOR"].empty() ? "" : itr->FieldValueMap["C_TOTAL_DERATING_FACTOR"].c_str());
			m_LV12_SfEdit.SetWindowText(itr->FieldValueMap["C_SF"].empty() ? "" : itr->FieldValueMap["C_SF"].c_str());
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	DC Cable Setting Data를 화면에 표시합니다.

	@author BHK	

	@date 2009-04-01 오전 10:13:05	

	@param	

	@return		
*/
int CPowerCableCreationSettingDlg::DisplayDCCableData(void)
{
	CDCCableCreationSettingTable& DCVoltageSettingTable = CDCCableCreationSettingTable::GetInstance();
	int nRow = 1;
	for(vector<CCableCreationTable::Record>::iterator itr = DCVoltageSettingTable.m_RecordEntry.begin();itr != DCVoltageSettingTable.m_RecordEntry.end();++itr,++nRow)
	{
		if(1 == nRow)
		{
			m_DC_BusVoltageEdit.SetWindowText(itr->FieldValueMap["C_BUS_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_BUS_VOLTAGE"].c_str());
			m_DC_LoadVoltageEdit.SetWindowText(itr->FieldValueMap["C_LOAD_VOLTAGE"].empty() ? "" : itr->FieldValueMap["C_LOAD_VOLTAGE"].c_str());

			int nSelItem = m_DC_CableVoltageCombo.FindString(-1, itr->FieldValueMap["C_CABLE_VOLTAGE"].c_str());
			if(-1 != nSelItem)
			{
				m_DC_CableVoltageCombo.SetCurSel(nSelItem);
				OnCbnSelchangeDcCableVoltageCombo();
			}
			nSelItem =  m_DC_CableTypeCombo.FindString(-1 , itr->FieldValueMap["C_CABLE_TYPE"].c_str());
			if( -1 != nSelItem) 
			{
				m_DC_CableTypeCombo.SetCurSel(nSelItem);
				OnCbnSelchangeDcCableTypeCombo();
			}
			nSelItem = m_DC_CableCoreCombo.FindString(-1 , itr->FieldValueMap["C_CABLE_CORE"].c_str());
			if( -1 != nSelItem)
			{
				m_DC_CableCoreCombo.SetCurSel(nSelItem);
			}
			m_DC_CableScheduleSpecEdit.SetWindowText(itr->FieldValueMap["C_CABLE_SCHEDULE_SPEC"].empty() ? "" : itr->FieldValueMap["C_CABLE_SCHEDULE_SPEC"].c_str());
			m_DC_FeederRunningEdit.SetWindowText(itr->FieldValueMap["C_FEEDER_RUNNING"].empty() ? "" : itr->FieldValueMap["C_FEEDER_RUNNING"].c_str());
			nSelItem = m_DC_InstallMethodCombo.FindString(-1, itr->FieldValueMap["C_INSTALL_METHOD"].c_str());
			if(-1 != nSelItem) m_DC_InstallMethodCombo.SetCurSel(nSelItem);
			else m_DC_InstallMethodCombo.SetWindowText(itr->FieldValueMap["C_INSTALL_METHOD"].empty() ? "" : itr->FieldValueMap["C_INSTALL_METHOD"].c_str());
			m_DC_AmbientTempEdit.SetWindowText(itr->FieldValueMap["C_AMBIENT_TEMP_DERATING_FACTOR"].empty() ? "" : itr->FieldValueMap["C_AMBIENT_TEMP_DERATING_FACTOR"].c_str());
			m_DC_OtherDeratingFactorEdit.SetWindowText(itr->FieldValueMap["C_OTHERS_DERATING_FACTOR"].empty() ? "" : itr->FieldValueMap["C_OTHERS_DERATING_FACTOR"].c_str());
			m_DC_TotalDeratingFactorEdit.SetWindowText(itr->FieldValueMap["C_TOTAL_DERATING_FACTOR"].empty() ? "" : itr->FieldValueMap["C_TOTAL_DERATING_FACTOR"].c_str());
			m_DC_SfEdit.SetWindowText(itr->FieldValueMap["C_SF"].empty() ? "" : itr->FieldValueMap["C_SF"].c_str());
		}
	}

	return ERROR_SUCCESS;
}
/**
	@brief	CABLE VOLTAGE COMBO BOX CHANGE

	@author KHS	

	@date 2009-04-06 오후 8:13:58	

	@param	

	@return		
**/
void CPowerCableCreationSettingDlg::OnCbnSelchangeCableVoltageCombo2()
{
	// TODO: Add your control notification handler code here
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CString rCableVoltage;
	int nItem = m_CableVoltageComboBox.GetCurSel();
	m_CableVoltageComboBox.GetLBText(nItem, rCableVoltage);

	m_CableTypeComboBox.ResetContent();
	for(vector<string>::iterator jtr = m_CableTypeList.begin(); jtr != m_CableTypeList.end();++jtr)
	{	
		CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(*jtr);
		if(pCableDataByType)
		{
			vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
			if(pRecordList)
			{
				for(vector<CCableDataByType::Record*>::iterator ktr = pRecordList->begin();ktr != pRecordList->end();++ktr)
				{
					if((*ktr)->m_FieldMap[_T("C_VOLT")].c_str() == rCableVoltage)
					{
						const CString rCableType = (*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str();
						if(-1 == m_CableTypeComboBox.FindString(-1 , rCableType))
							m_CableTypeComboBox.AddString(rCableType);
					}
				}
			}
		}
	}

	CPowerCableCreationSettingTable* pPowerCableSettingTable = static_cast<CPowerCableCreationSettingTable*>(docData.GetTableOf(CPowerCableCreationSettingTable::GetTableName()));
	if(pPowerCableSettingTable)
	{
		if(1 == pPowerCableSettingTable->m_RecordEntry.size())
		{
			const string rCableType = pPowerCableSettingTable->m_RecordEntry[0].FieldValueMap[_T("C_SPACE_HEATER_CABLE_TYPE")];
			const int nFound = m_CableTypeComboBox.FindString(-1 , rCableType.c_str());
			if(-1 != nFound)
				m_CableTypeComboBox.SetCurSel(nFound);
			else
			{
				m_CableTypeComboBox.SetWindowText(_T(""));
				pPowerCableSettingTable->m_RecordEntry[0].FieldValueMap[_T("C_SPACE_HEATER_CABLE_TYPE")] = _T("");
			}
		}
	}

	OnCbnSelchangeCableTypeCombo2();
}
/**
	@brief	CABLE TYPE COMBO BOX CHANGE

	@author KHS	

	@date 2009-04-06 오후 8:13:58	

	@param	

	@return		
**/
void CPowerCableCreationSettingDlg::OnCbnSelchangeCableTypeCombo2()
{
	// TODO: Add your control notification handler code here
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CString rCableVoltage, rCableType;
	int nCableVoltageItem = m_CableVoltageComboBox.GetCurSel();
	int nCableTypeItem = m_CableTypeComboBox.GetCurSel();
	if(-1 != nCableVoltageItem)m_CableVoltageComboBox.GetLBText(nCableVoltageItem, rCableVoltage);
	else return;
	if(-1 != nCableTypeItem)m_CableTypeComboBox.GetLBText(nCableTypeItem, rCableType);
	else return;

	m_CableCoreComboBox.ResetContent();
	for(vector<string>::iterator jtr = m_CableTypeList.begin(); jtr != m_CableTypeList.end();++jtr)
	{	
		CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(*jtr);
		if(pCableDataByType)
		{
			vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
			if(pRecordList)
			{
				for(vector<CCableDataByType::Record*>::iterator ktr = pRecordList->begin();ktr != pRecordList->end();++ktr)
				{
					if((*ktr)->m_FieldMap[_T("C_VOLT")].c_str() == rCableVoltage && (*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str() == rCableType)
					{
						const CString rCableCore = (*ktr)->m_FieldMap[_T("C_CORE")].c_str();
						if(-1 == m_CableCoreComboBox.FindString(-1 , rCableCore))
							m_CableCoreComboBox.AddString(rCableCore);
					}
				}
			}
		}
	}

	CPowerCableCreationSettingTable* pPowerCableSettingTable = static_cast<CPowerCableCreationSettingTable*>(docData.GetTableOf(CPowerCableCreationSettingTable::GetTableName()));
	if(pPowerCableSettingTable)
	{
		if(1 == pPowerCableSettingTable->m_RecordEntry.size())
		{
			const string rCableCore = pPowerCableSettingTable->m_RecordEntry[0].FieldValueMap[_T("C_SPACE_HEATER_CABLE_CORE")];
			const int nFound = m_CableCoreComboBox.FindString(-1 , rCableCore.c_str());
			if(-1 != nFound)
				m_CableCoreComboBox.SetCurSel(nFound);
			else
			{
				m_CableCoreComboBox.SetWindowText(_T(""));
				pPowerCableSettingTable->m_RecordEntry[0].FieldValueMap[_T("C_SPACE_HEATER_CABLE_CORE")] = _T("");
			}
		}
	}

	OnCbnSelchangeCableCoreCombo3();
}
/**
	@brief	CABLE CORE COMBO BOX CHANGE

	@author KHS	

	@date 2009-04-06 오후 8:13:58	

	@param	

	@return		
**/
void CPowerCableCreationSettingDlg::OnCbnSelchangeCableCoreCombo3()
{
	// TODO: Add your control notification handler code here
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CString rCableVoltage, rCableType , rCableCore;
	int nSel = m_CableVoltageComboBox.GetCurSel();
	if(-1 != nSel) m_CableVoltageComboBox.GetLBText(nSel, rCableVoltage);
	nSel = m_CableTypeComboBox.GetCurSel();
	if(-1 != nSel) m_CableTypeComboBox.GetLBText(nSel, rCableType);
	nSel = m_CableCoreComboBox.GetCurSel();
	if(-1 != nSel) m_CableCoreComboBox.GetLBText(nSel , rCableCore);

	m_CableSizeComboBox.ResetContent();
	for(vector<string>::iterator jtr = m_CableTypeList.begin(); jtr != m_CableTypeList.end();++jtr)
	{	
		CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(*jtr);
		if(pCableDataByType)
		{
			vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
			if(pRecordList)
			{
				for(vector<CCableDataByType::Record*>::iterator ktr = pRecordList->begin();ktr != pRecordList->end();++ktr)
				{
					if(((*ktr)->m_FieldMap[_T("C_VOLT")].c_str() == rCableVoltage) && ((*ktr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str() == rCableType) && 
						((*ktr)->m_FieldMap[_T("C_CORE")].c_str() == rCableCore))
					{
						const CString rCableSize = (*ktr)->m_FieldMap[_T("C_SIZE")].c_str();
						if(-1 == m_CableSizeComboBox.FindString(-1 , rCableSize))
							m_CableSizeComboBox.AddString(rCableSize);
					}
				}
			}
		}
	}

	CPowerCableCreationSettingTable* pPowerCableSettingTable = static_cast<CPowerCableCreationSettingTable*>(docData.GetTableOf(CPowerCableCreationSettingTable::GetTableName()));
	if(pPowerCableSettingTable)
	{
		if(1 == pPowerCableSettingTable->m_RecordEntry.size())
		{
			const string rCableSize = pPowerCableSettingTable->m_RecordEntry[0].FieldValueMap[_T("C_SPACE_HEATER_CABLE_SIZE")];
			const int nFound = m_CableSizeComboBox.FindString(-1 , rCableSize.c_str());
			if(-1 != nFound)
				m_CableSizeComboBox.SetCurSel(nFound);
			else
			{
				m_CableSizeComboBox.SetCurSel(-1);
				pPowerCableSettingTable->m_RecordEntry[0].FieldValueMap[_T("C_SPACE_HEATER_CABLE_SIZE")] = _T("");
			}
		}
	}

	OnCbnSelchangeCableSizeCombo2();
}
/**
	@brief	CABLE SIZE COMBO BOX CHANGE

	@author KHS	

	@date 2009-04-06 오후 8:13:58	

	@param	

	@return		
**/
void CPowerCableCreationSettingDlg::OnCbnSelchangeCableSizeCombo2()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CPowerCableCreationSettingTable* pPowerCableSettingTable = static_cast<CPowerCableCreationSettingTable*>(docData.GetTableOf(CPowerCableCreationSettingTable::GetTableName()));
	if(pPowerCableSettingTable)
	{
		if(1 == pPowerCableSettingTable->m_RecordEntry.size())
		{
			CString rCableSize;
			const int nSel = m_CableSizeComboBox.GetCurSel();
			if(-1 != nSel)
			{
				m_CableSizeComboBox.GetLBText(nSel , rCableSize);
				pPowerCableSettingTable->m_RecordEntry[0].FieldValueMap[_T("C_SPACE_HEATER_CABLE_SIZE")] = rCableSize.operator LPCSTR();
			}
			else
			{
				pPowerCableSettingTable->m_RecordEntry[0].FieldValueMap[_T("C_SPACE_HEATER_CABLE_SIZE")] = _T("");
			}
		}
	}
}
