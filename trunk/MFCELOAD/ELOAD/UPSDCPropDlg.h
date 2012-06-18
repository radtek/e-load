#pragma once
#include "EventData.h"

// CUPSDCPropDlg dialog

class CUPSDCPropDlg : public CDialog
{
	DECLARE_DYNAMIC(CUPSDCPropDlg)

public:
	CUPSDCPropDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CUPSDCPropDlg();

// Dialog Data
	enum { IDD = IDD_UPSDC_PROP };
	
	CComboBox m_wndFrom1BusIdCombo , m_wndFrom2BusIdCombo , m_wndToBusIdCombo;
	CComboBox m_wndFrom2CableVoltCombo , m_wndFrom2CableTypeCombo , m_wndFrom2CableCoreCombo;
	CEdit m_wndFrom2CableSpecEdit;
	static int IDEAL_WIDTH;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnItemSelected(WPARAM wParam, LPARAM lParam);
private:
	int SetEventDataValues(CEventData* pEventData);
public:
	afx_msg void OnCbnSelendokComboFrom2CableVolt();
	afx_msg void OnCbnSelendokComboFrom2CableType();
	afx_msg void OnCbnSelendokComboFrom1BusId();
	afx_msg void OnCbnSelendokComboFrom2BusId();
	afx_msg void OnCbnSelendokComboToBusId();
	afx_msg void OnEnChangeEditFrom2Spec();
	afx_msg void OnCbnSelendokComboFrom2CableCore();
private:
	int FillLoadVolt(void);
	int CalculateFLCOfUPSCapacity();
	int CalculateKVAAndFLCOfDCCapacity();
public:
	afx_msg void OnEnChangeEditInputKva();
	afx_msg void OnEnChangeEditInputPf();
	afx_msg void OnEnChangeEditAcRatingPf();
	afx_msg void OnEnChangeEditDcRatingFlc();
	afx_msg void OnBnClickedRadioUpsCapacity();
	afx_msg void OnBnClickedRadioDcCapacity();
	afx_msg void OnCbnSelendokComboOpMode1();
	afx_msg void OnCbnSelendokComboOpMode2();
	afx_msg void OnEnChangeEditInputBhp();
	afx_msg void OnEnChangeEditAcRatingBhp();
	afx_msg void OnEnChangeEditUpsdcPf();
	afx_msg void OnEnChangeEditUpsdcLength();
};
