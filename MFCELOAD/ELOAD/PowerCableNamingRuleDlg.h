#pragma once

#include "GridCtrlEx.h"
#include "LabelStatic.h"
#include "afxwin.h"
#include "XGroupBox.h"

// CPowerCableNamingRuleDlg dialog

class CPowerCableNamingRuleDlg : public CDialog
{
	DECLARE_DYNAMIC(CPowerCableNamingRuleDlg)

public:
	CPowerCableNamingRuleDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPowerCableNamingRuleDlg();

// Dialog Data
	enum { IDD = IDD_POWER_CABLE_NAMING_RULE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	int CreatePowerCableNamingRuleGridCtrl(void);
	int LoadData(void);
	int DisplayData(void);

	afx_msg void OnBnClickedOk();
	int SaveData(void);
private:
	CMFCButton m_wndOKButton;
	CMFCButton m_wndCancelButton;

	/*CComboBox m_CableVoltageComboBox;
	CComboBox m_CableTypeComboBox;
	CComboBox m_CableCoreComboBox , m_CableSizeComboBox;
	CEdit m_CableScheduleSpecEdit;*/

	vector<string> m_CableTypeList;
	
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	CStatic m_HV1_VoltageTypeStatic,m_HV2_VoltageTypeStatic,m_HV3_VoltageTypeStatic,
		m_LV1_VoltageTypeStatic,m_LV2_VoltageTypeStatic,m_LV11_VoltageTypeStatic,m_LV12_VoltageTypeStatic,
		m_DC_VoltageTypeStatic;

	CEdit m_HV1_BusVoltageEdit,m_HV2_BusVoltageEdit,m_HV3_BusVoltageEdit,
		m_LV1_BusVoltageEdit,m_LV2_BusVoltageEdit,m_LV11_BusVoltageEdit,m_LV12_BusVoltageEdit,
		m_DC_BusVoltageEdit;

	CEdit m_HV1_LoadVoltageEdit,m_HV2_LoadVoltageEdit,m_HV3_LoadVoltageEdit,
		m_LV1_LoadVoltageEdit,m_LV2_LoadVoltageEdit,m_LV11_LoadVoltageEdit,m_LV12_LoadVoltageEdit,
		m_DC_LoadVoltageEdit;

	CEdit m_HV1_MainNamingPrefixEdit,m_HV2_MainNamingPrefixEdit,m_HV3_MainNamingPrefixEdit,
		m_LV1_MainNamingPrefixEdit,m_LV2_MainNamingPrefixEdit,m_LV11_MainNamingPrefixEdit,m_LV12_MainNamingPrefixEdit,
		m_DC_MainNamingPrefixEdit;

	CEdit m_HV1_MainNamingSuffixEdit,m_HV2_MainNamingSuffixEdit,m_HV3_MainNamingSuffixEdit,
		m_LV1_MainNamingSuffixEdit,m_LV2_MainNamingSuffixEdit,m_LV11_MainNamingSuffixEdit,m_LV12_MainNamingSuffixEdit,
		m_DC_MainNamingSuffixEdit;

	CEdit m_HV1_SpaceHeaderNamingPrefixEdit,m_HV2_SpaceHeaderNamingPrefixEdit,m_HV3_SpaceHeaderNamingPrefixEdit,
		m_LV1_SpaceHeaderNamingPrefixEdit,m_LV2_SpaceHeaderNamingPrefixEdit;

	CEdit m_HV1_SpaceHeaderNamingSuffixEdit,m_HV2_SpaceHeaderNamingSuffixEdit,m_HV3_SpaceHeaderNamingSuffixEdit,
		m_LV1_SpaceHeaderNamingSuffixEdit,m_LV2_SpaceHeaderNamingSuffixEdit;

	//! GROUP BOX
	CXGroupBox m_StaticNamingRuleSetting;
	//CXGroupBox m_StaticSpaceHeaderCableSetting;
	CXGroupBox m_StaticSingleCoreCableNumbering;
	CXGroupBox m_StaticParallerCableNumbering;

	//! EXAMPLE
	CLabelStatic m_HV1_MainExStatic,m_HV2_MainExStatic,m_HV3_MainExStatic,
		m_LV1_MainExStatic,m_LV2_MainExStatic,m_LV11_MainExStatic,m_LV12_MainExStatic,
		m_DC_MainExStatic;

	CLabelStatic m_HV1_SpaceHeaderExStatic,m_HV2_SpaceHeaderExStatic,m_HV3_SpaceHeaderExStatic,
		m_LV1_SpaceHeaderExStatic,m_LV2_SpaceHeaderExStatic;

	int SetExampleText(void);
	afx_msg void OnEnChangeHv1MainPrefixEdit();
	afx_msg void OnEnChangeHv2MainPrefixEdit();
	afx_msg void OnEnChangeHv3MainPrefixEdit();
	afx_msg void OnEnChangeLv1MainPrefixEdit();
	afx_msg void OnEnChangeLv2MainPrefixEdit();
	afx_msg void OnEnChangeLv11MainPrefixEdit();
	afx_msg void OnEnChangeLv12MainPrefixEdit();
	afx_msg void OnEnChangeDcMainPrefixEdit();
	afx_msg void OnEnChangeHv1MainSuffixEdit();
	afx_msg void OnEnChangeHv2MainSuffixEdit();
	afx_msg void OnEnChangeHv3MainSuffixEdit();
	afx_msg void OnEnChangeLv1MainSuffixEdit();
	afx_msg void OnEnChangeLv2MainSuffixEdit();
	afx_msg void OnEnChangeLv11MainSuffixEdit();
	afx_msg void OnEnChangeLv12MainSuffixEdit();
	afx_msg void OnEnChangeDcMainSuffixEdit();
	afx_msg void OnEnChangeHv1SpaceHeaderPrefixEdit();
	afx_msg void OnEnChangeHv2SpaceHeaderPrefixEdit();
	afx_msg void OnEnChangeHv3SpaceHeaderPrefixEdit();
	afx_msg void OnEnChangeLv1SpaceHeaderPrefixEdit();
	afx_msg void OnEnChangeLv2SpaceHeaderPrefixEdit();
	afx_msg void OnEnChangeHv1SpaceHeaderSuffixEdit();
	afx_msg void OnEnChangeHv2SpaceHeaderSuffixEdit();
	afx_msg void OnEnChangeHv3SpaceHeaderSuffixEdit();
	afx_msg void OnEnChangeLv1SpaceHeaderSuffixEdit();
	afx_msg void OnEnChangeLv2SpaceHeaderSuffixEdit();
	
	int onEditBoxEndEdit(CEdit* prefixEdit, CEdit* suffixEdit, CLabelStatic* resultStatic);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	int CheckItemAndReadOnly(void);
	int DisplayCableNamingRuleSetting(void);
	int SaveCableNamingRuleSettingData(void);
	int LoadCableDataTable(void);

	//int DisplayCableDataCombo(void);
	//afx_msg void OnCbnSelchangeCableVoltageCombo();
	//afx_msg void OnCbnSelchangeCableTypeCombo();

	//! RADIO BUTTON
	int m_nParallel;
	int m_nSingleCore;

	//afx_msg void OnCbnSelendokCableSizeCombo();
	//afx_msg void OnCbnSelendokCableCoreCombo();
private:
	int DisplayHVCableData(void);
	int DisplayLV3PHCableData(void);
	int DisplayLV1PHCableData(void);
	int DisplayDCCableData(void);
};
