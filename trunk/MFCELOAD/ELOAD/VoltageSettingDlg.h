#pragma once


#include "afxwin.h"
#include <gui/FilterEdit/BoostBaseFilterEdit.h>
#include "MFCButtonEx.h"

// CVoltageSettingDlg dialog

class CVoltageSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CVoltageSettingDlg)

public:
	CVoltageSettingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVoltageSettingDlg();

// Dialog Data
	enum { IDD = IDD_VAOLTAGE_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
        virtual BOOL OnInitDialog();

private:
	int DisplayData();
public:
        //int CreateVoltageSettingGridCtrl(void);
        virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedOK();
	afx_msg void OnBnClickedSave();

	int SaveData(void);
	int LoadData(void);

	CMFCButtonEx m_wndOKButton;
	CMFCButtonEx m_wndCancelButton;

	int InitEditBox(void);
private:
	CBoostBaseFilterEdit m_HV1_BusVoltage;
	CBoostBaseFilterEdit m_HV2_BusVoltage;
	CBoostBaseFilterEdit m_HV3_BusVoltage;
	CBoostBaseFilterEdit m_LV1_3PH_BusVoltage;
	CBoostBaseFilterEdit m_LV2_3PH_BusVoltage;
	CBoostBaseFilterEdit m_LV11_1PH_BusVoltage;
	CBoostBaseFilterEdit m_LV12_1PH_BusVoltage;
	CBoostBaseFilterEdit m_DC_BusVoltage;

	CBoostBaseFilterEdit m_HV1_LoadVoltage;
	CBoostBaseFilterEdit m_HV2_LoadVoltage;
	CBoostBaseFilterEdit m_HV3_LoadVoltage;
	CBoostBaseFilterEdit m_LV1_3PH_LoadVoltage;
	CBoostBaseFilterEdit m_LV2_3PH_LoadVoltage;
	CBoostBaseFilterEdit m_LV11_1PH_LoadVoltage;
	CBoostBaseFilterEdit m_LV12_1PH_LoadVoltage;
	CBoostBaseFilterEdit m_DC_LoadVoltage;

	CStatic m_HV1_STATIC;
	CStatic m_HV2_STATIC;
	CStatic m_HV3_STATIC;
	CStatic m_LV1_3PH_STATIC;
	CStatic m_LV2_3PH_STATIC;
	CStatic m_LV11_1PH_STATIC;
	CStatic m_LV12_1PH_STATIC;
	CStatic m_DC_STATIC;

	CStatic m_HV1_PHASE_STATIC;
	CStatic m_HV2_PHASE_STATIC;
	CStatic m_HV3_PHASE_STATIC;
	CStatic m_LV1_3PH_PHASE_STATIC;
	CStatic m_LV2_3PH_PHASE_STATIC;
	CStatic m_LV11_1PH_PHASE_STATIC;
	CStatic m_LV12_1PH_PHASE_STATIC;
	CStatic m_DC_PHASE_STATIC;
public:
	afx_msg void OnEnChangeHv1BusVoltageEdit();
	BOOL CheckBusVoltage(const CString& rCompareBusVoltage, const int& nIndex);
	afx_msg void OnEnChangeHv2BusVoltageEdit();
	afx_msg void OnEnChangeHv3BusVoltageEdit();
	afx_msg void OnEnChangeLv13phBusVoltageEdit();
	afx_msg void OnEnChangeLv23phBusVoltageEdit();
	afx_msg void OnEnChangeLv111phBusVoltageEdit();
	afx_msg void OnEnChangeLv121phBusVoltageEdit();
	afx_msg void OnEnChangeDcBusVoltageEdit();
	afx_msg void OnStnClickedHv1Static2();
};
