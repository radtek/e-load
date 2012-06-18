#pragma once

#include "Tables/CableDataTable.h"
#include "GridCtrlEx.h"
#include <gui/FilterEdit/BoostBaseFilterEdit.h>
#include "LabelStatic.h"
#include "XGroupBox.h"
#include "GroupCheck.h"
#include "afxwin.h"
#include <string>
#include <vector>
#include "LoadItem.h"
#include "CableLengthDataSheet.h"

#include "Tables/CableDataTable.h"
#include "Tables/ELOADTable.h"

using namespace TABLES;
// CPowerCableCreationSettingDlg dialog

class CPowerCableCreationSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CPowerCableCreationSettingDlg)

public:
	CPowerCableCreationSettingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPowerCableCreationSettingDlg();

	CComboBox	m_wndAGTempCombo;
	CComboBox	m_wndUGTempCombo;

	CXGroupBox	m_StaticPowerCableCreatinSetting;
	CXGroupBox	m_DeratingFactorGroupBox;
	CXGroupBox	m_ConduitSizingSettingGroupBox;
	CXGroupBox	m_MinPowerFactorGroupBox;
	CXGroupBox	m_PowerCableCreationSettingGroupBox;
	BOOL		m_bApplyByAmbientTempFactor;
	///BOOL		m_bApplyMinLVPowerCableSize , m_bApplyByFaultCurrentAtHVPowerCable;

	CMFCButton	m_wndCalcButton;

	CMFCButton	m_wndCloseButton;
	CMFCButton	m_wndSettingSaveButton;

// Dialog Data
	enum { IDD = IDD_POWER_CABLE_CREATION_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	int DisplayData(void);
	int LoadData(void);
public:
	afx_msg void OnBnClickedButtonCalc();
	//afx_msg void OnBnClickedButtonCableCreation();
	afx_msg void OnBnClickedButtonSettingSave();
	afx_msg void OnBnClickedButtonClose();

public:
	int SaveData(void);
	int InitEditBox(void);
private:
	CComboBox m_SelectCableDataTableComboBox;

	vector<string> m_CableTypeList;

	CStringArray	m_CableType;
	CStringArray	m_CableVoltage;
	CStringArray	m_CableCore;
public:
	static int UpdateLoadItems(void);
	int LoadCableDataTable(void);
	afx_msg void OnCbnSelchangeSelectCableDataCombo();
	afx_msg void OnBnClickedCheckApplyByAmbientTempFactor();
private:
	int FillComboBoxWithCableSettingData(void);
	int UpdateComboContents(void);
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
private:
	int CalcAmbientTempDeratingFactor(const int& nRow);
	int CalcTotalDeratingFactor(const int& nRow);
	int CheckItemAndReadOnly(void);
public:
//	afx_msg void OnBnClickedCheckApplyByAmbientTempFactor();

	CEdit m_HV1_BusVoltageEdit, m_HV2_BusVoltageEdit, m_HV3_BusVoltageEdit, 
		m_LV1_BusVoltageEdit, m_LV2_BusVoltageEdit, m_LV11_BusVoltageEdit,m_LV12_BusVoltageEdit,
		m_DC_BusVoltageEdit;

	CEdit m_HV1_LoadVoltageEdit,m_HV2_LoadVoltageEdit, m_HV3_LoadVoltageEdit, 
		m_LV1_LoadVoltageEdit, m_LV2_LoadVoltageEdit, m_LV11_LoadVoltageEdit, m_LV12_LoadVoltageEdit,
		m_DC_LoadVoltageEdit;

	CComboBox m_HV1_CableVoltageCombo,m_HV2_CableVoltageCombo,m_HV3_CableVoltageCombo,
		m_LV1_CableVoltageCombo,m_LV2_CableVoltageCombo,m_LV11_CableVoltageCombo,m_LV12_CableVoltageCombo,
		m_DC_CableVoltageCombo;

	CComboBox m_HV1_CableTypeCombo, m_HV2_CableTypeCombo, m_HV3_CableTypeCombo,
		m_LV1_CableTypeCombo, m_LV2_CableTypeCombo,m_LV11_CableTypeCombo, m_LV12_CableTypeCombo,
		m_DC_CableTypeCombo;

	CEdit m_HV1_CableScheduleSpecEdit,m_HV2_CableScheduleSpecEdit,m_HV3_CableScheduleSpecEdit,
		m_LV1_CableScheduleSpecEdit,m_LV2_CableScheduleSpecEdit,m_LV11_CableScheduleSpecEdit,m_LV12_CableScheduleSpecEdit,
		m_DC_CableScheduleSpecEdit;

	CComboBox m_HV1_CableCoreCombo,m_HV2_CableCoreCombo,m_HV3_CableCoreCombo,
		m_LV1_CableCoreCombo,m_LV2_CableCoreCombo,m_LV11_CableCoreCombo,m_LV12_CableCoreCombo,
		m_DC_CableCoreCombo;

	CEdit m_HV1_MotorRunningEdit,m_HV2_MotorRunningEdit,m_HV3_MotorRunningEdit,
		m_LV1_MotorRunningEdit,m_LV2_MotorRunningEdit;

	CEdit m_HV1_MotorStartingEdit,m_HV2_MotorStartingEdit,m_HV3_MotorStartingEdit,
		m_LV1_MotorStartingEdit,m_LV2_MotorStartingEdit;

	CButton m_HV1_ApplyMotorCheck,m_HV2_ApplyMotorCheck,m_HV3_ApplyMotorCheck,
		m_LV1_ApplyMotorCheck,m_LV2_ApplyMotorCheck;

	CEdit m_HV1_FeederRunningEdit,m_HV2_FeederRunningEdit,m_HV3_FeederRunningEdit,
		m_LV1_FeederRunningEdit,m_LV2_FeederRunningEdit,m_LV11_FeederRunningEdit,m_LV12_FeederRunningEdit,
		m_DC_FeederRunningEdit;

	CComboBox m_HV1_InstallMethodCombo,m_HV2_InstallMethodCombo,m_HV3_InstallMethodCombo,
		m_LV1_InstallMethodCombo,m_LV2_InstallMethodCombo,m_LV11_InstallMethodCombo,m_LV12_InstallMethodCombo,
		m_DC_InstallMethodCombo;

	CEdit m_HV1_AmbientTempEdit,m_HV2_AmbientTempEdit,m_HV3_AmbientTempEdit,
		m_LV1_AmbientTempEdit,m_LV2_AmbientTempEdit,m_LV11_AmbientTempEdit,m_LV12_AmbientTempEdit,
		m_DC_AmbientTempEdit;

	CEdit m_HV1_OtherDeratingFactorEdit,m_HV2_OtherDeratingFactorEdit,m_HV3_OtherDeratingFactorEdit,
		m_LV1_OtherDeratingFactorEdit,m_LV2_OtherDeratingFactorEdit,m_LV11_OtherDeratingFactorEdit,m_LV12_OtherDeratingFactorEdit,
		m_DC_OtherDeratingFactorEdit;

	CEdit m_HV1_TotalDeratingFactorEdit,m_HV2_TotalDeratingFactorEdit,m_HV3_TotalDeratingFactorEdit,
		m_LV1_TotalDeratingFactorEdit,m_LV2_TotalDeratingFactorEdit,m_LV11_TotalDeratingFactorEdit,m_LV12_TotalDeratingFactorEdit,
		m_DC_TotalDeratingFactorEdit;

	CEdit m_HV1_SfEdit,m_HV2_SfEdit,m_HV3_SfEdit,
		m_LV1_SfEdit,m_LV2_SfEdit,m_LV11_SfEdit,m_LV12_SfEdit,
		m_DC_SfEdit;

	CLabelStatic m_VoltageSettingTitleStatic;
	CLabelStatic m_CableSettingTitleStatic;
	CLabelStatic m_VoltageDropTitleStatic;
	CLabelStatic m_DeratingFactorTitleStatic;

	CStatic m_HV1_VoltageTypeStatic,m_HV2_VoltageTypeStatic,m_HV3_VoltageTypeStatic,
		m_LV1_VoltageTypeStatic,m_LV2_VoltageTypeStatic,m_LV11_VoltageTypeStatic,m_LV12_VoltageTypeStatic,
		m_DC_VoltageTypeStatic;

	//! CABLE CREATION ITEM
	CComboBox m_CableVoltageComboBox;
	CComboBox m_CableTypeComboBox;
	CComboBox m_CableCoreComboBox , m_CableSizeComboBox;
	CEdit m_CableScheduleSpecEdit;

	afx_msg void OnCbnSelchangeHv1CableVoltageCombo();
	afx_msg void OnCbnSelchangeHv2CableVoltageCombo();
	afx_msg void OnCbnSelchangeHv3CableVoltageCombo();
	afx_msg void OnCbnSelchangeLv1CableVoltageCombo();
	afx_msg void OnCbnSelchangeLv2CableVoltageCombo();
	afx_msg void OnCbnSelchangeLv11CableVoltageCombo();
	afx_msg void OnCbnSelchangeLv12CableVoltageCombo();
	afx_msg void OnCbnSelchangeDcCableVoltageCombo();
	afx_msg void OnCbnSelchangeHv1CableTypeCombo();
	afx_msg void OnCbnSelchangeHv2CableTypeCombo();
	afx_msg void OnCbnSelchangeHv3CableTypeCombo();
	afx_msg void OnCbnSelchangeLv1CableTypeCombo();
	afx_msg void OnCbnSelchangeLv2CableTypeCombo();
	afx_msg void OnCbnSelchangeLv11CableTypeCombo();
	afx_msg void OnCbnSelchangeLv12CableTypeCombo();
	afx_msg void OnCbnSelchangeDcCableTypeCombo();
	afx_msg void OnCbnSelendokHv1InstallMethodCombo();
	afx_msg void OnCbnSelendokHv2InstallMethodCombo();
	afx_msg void OnCbnSelendokHv3InstallMethodCombo();
	afx_msg void OnCbnSelendokLv1InstallMethodCombo();
	afx_msg void OnCbnSelendokLv2InstallMethodCombo();
	afx_msg void OnCbnSelendokLv11InstallMethodCombo();
	afx_msg void OnCbnSelendokLv12InstallMethodCombo();
	afx_msg void OnCbnSelendokDcInstallMethodCombo();
	afx_msg void OnCbnSelendokHv1CableVoltageCombo();
	afx_msg void OnCbnSelendokHv2CableVoltageCombo();
	afx_msg void OnCbnSelendokHv3CableVoltageCombo();
	afx_msg void OnCbnSelendokLv1CableVoltageCombo();
	afx_msg void OnCbnSelendokLv2CableVoltageCombo();
	afx_msg void OnCbnSelendokLv11CableVoltageCombo();
	afx_msg void OnCbnSelendokLv12CableVoltageCombo();
	afx_msg void OnCbnSelendokDcCableVoltageCombo();
	afx_msg void OnCbnSelendokHv1CableTypeCombo();
	afx_msg void OnCbnSelendokHv2CableTypeCombo();
	afx_msg void OnCbnSelendokHv3CableTypeCombo();
	afx_msg void OnCbnSelendokLv1CableTypeCombo();
	afx_msg void OnCbnSelendokLv2CableTypeCombo();
	afx_msg void OnCbnSelendokLv11CableTypeCombo();
	afx_msg void OnCbnSelendokLv12CableTypeCombo();
	afx_msg void OnCbnSelendokDcCableTypeCombo();
	afx_msg void OnCbnSelendokHv1CableCoreCombo();
	afx_msg void OnCbnSelendokHv2CableCoreCombo();
	afx_msg void OnCbnSelendokHv3CableCoreCombo();
	afx_msg void OnCbnSelendokLv1CableCoreCombo();
	afx_msg void OnCbnSelendokLv2CableCoreCombo();
	afx_msg void OnCbnSelendokLv11CableCoreCombo();
	afx_msg void OnCbnSelendokLv12CableCoreCombo();
	afx_msg void OnCbnSelendokDcCableCoreCombo();
public:
	afx_msg void OnEnChangeHv1AmbientTempEdit();
	afx_msg void OnEnChangeHv2AmbientTempEdit();
	afx_msg void OnEnChangeHv3AmbientTempEdit();
	afx_msg void OnEnChangeLv1AmbientTempEdit();
	afx_msg void OnEnChangeLv2AmbientTempEdit();
	afx_msg void OnEnChangeLv11AmbientTempEdit();
	afx_msg void OnEnChangeLv12AmbientTempEdit();
	afx_msg void OnEnChangeDcAmbientTempEdit();
	afx_msg void OnEnChangeHv1OtherEdit();
	afx_msg void OnEnChangeHv2OtherEdit();
	afx_msg void OnEnChangeHv3OtherEdit();
	afx_msg void OnEnChangeLv1OtherEdit();
	afx_msg void OnEnChangeLv2OtherEdit();
	afx_msg void OnEnChangeLv11OtherEdit();
	afx_msg void OnEnChangeLv12OtherEdit();
	afx_msg void OnEnChangeDcOtherEdit();
private:
	int FillApplyMinLVPowerCableSizeComboBox(void);
public:
	afx_msg void OnBnClickedCheckApplyMinimumLvPowerCableSize();
	afx_msg void OnBnClickedCheckApplyByFaultCurrentAtHvPowerCable();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnCbnSelchangeComboAgTemp();
	afx_msg void OnCbnSelchangeComboUgTemp();
	int SaveSelectCabledataTable();
	int SavePowerCableCreationSettingSettingData(void);
private:
	///int CreatePowerCable(void);
	//int CreatePowerCableInBus(const double& nMinSize , CELOADTable::Record &SettingRecord , vector<CBusItem*>& BusItemEntry);
	///int FindPowerCableSize(CCableItem* pCableItem , 
	///	vector<TABLES::CCableDataByType::Record*>* pRecordList , vector<TABLES::CCableDataByType::Record*>::iterator& ktr , 
	///	CLoadItem* pLoadItem , CELOADTable::Record &SettingRecord , const string& PF , const string& MotorVD , const double& nMinSize , int& nMultiply , const int& pos);
	///int SetPowerCableProp(CELoadItemProp* pItemProp , ITEMS::CLoadItem* pLoadItem , 
	///	CELOADTable::Record &SettingRecord , const int& nNoOfCable , CCableDataByType::Record* pRecord);
	///int CreatePowerCableItem(ITEMS::CLoadItem* pLoadItem , 
	///	const string& rVoltType ,const string& rCableNamePrefix, const string& rCableNameSuffix, const int& nNoOfCable , CCableDataByType::Record* pRecord);
	int DisplayCableCreationSettingData(void);

private:
	int DisplayHVCableData(void);
	int DisplayLV3PHCableData(void);
	int DisplayLV1PHCableData(void);
	int DisplayDCCableData(void);
public:
	afx_msg void OnCbnSelchangeCableVoltageCombo2();
	afx_msg void OnCbnSelchangeCableTypeCombo2();
	afx_msg void OnCbnSelchangeCableCoreCombo3();
	afx_msg void OnCbnSelchangeCableSizeCombo2();
};
