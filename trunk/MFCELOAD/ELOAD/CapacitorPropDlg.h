#pragma once
#include "afxwin.h"
#include "LabelStatic.h"
#include "EventData.h"

// CCapacitorPropDlg dialog

class CCapacitorPropDlg : public CDialog
{
	DECLARE_DYNAMIC(CCapacitorPropDlg)

public:
	CCapacitorPropDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCapacitorPropDlg();

// Dialog Data
	enum { IDD = IDD_CAPACITOR_PROP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CEdit m_wndCapacitorIdEdit;
	CEdit m_wndFromBusVoltEdit;
	CEdit m_wndFromLoadVoltEdit;
	CEdit m_wndFromCableSpecEdit;
	CEdit m_wndTargetPowerFactorEdit;
	CEdit m_wndCalculatedCapacityEdit;
	CEdit m_wndSelectedCapacityEdit;
	CEdit m_wndFlcEdit;
	CEdit m_wndImprovedPowerFactorEdit;

	CComboBox m_wndFromBusIdCombo;
	CComboBox m_wndFromCableVoltCombo;
	CComboBox m_wndFromCableTypeCombo;
	CComboBox m_wndFromCableCoreCombo;

	CEdit m_wndTotalKWEdit , m_wndTotalKVAEdit;

	static int IDEAL_WIDTH;
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg LRESULT OnItemSelected(WPARAM wParam, LPARAM lParam);

private:
	int SetEventDataValues(CEventData* pEventData);
	int SetCalculatedCapacityText();
	int DisplayLoadSummaryResult();
public:
	afx_msg void OnCbnSelchangeComboFromCableVolt();
	afx_msg void OnCbnSelchangeComboFromCableType();
	afx_msg void OnCbnSelchangeComboFromBusId();
	afx_msg void OnCbnSelchangeComboFromCore();
	afx_msg void OnEnChangeEditTargetPowerFactor();
	afx_msg void OnEnChangeEditSelectedCapacity();
	afx_msg void OnEnChangeEditFromSpec();

	int SetSelectedCapacityText(void);
	afx_msg void OnNMClickListConnectedList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditCapacitorPf();
	afx_msg void OnEnChangeEditCapacitorLength();
	afx_msg void OnEnChangeEditTotalKw();
	afx_msg void OnEnChangeEditTotalKva();
};
