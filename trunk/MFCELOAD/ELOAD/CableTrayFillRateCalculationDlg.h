#pragma once

#include "LabelStatic.h"
#include "afxwin.h"
#include "MFCButtonEx.h"
#include <gui/FilterEdit/BoostBaseFilterEdit.h>
#include "RouteItem.h"
#include "CableItem.h"

// CCableTrayFillRateCalculationDlg dialog

class CCableTrayFillRateCalculationDlg : public CDialog
{
	DECLARE_DYNAMIC(CCableTrayFillRateCalculationDlg)

public:
	CCableTrayFillRateCalculationDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCableTrayFillRateCalculationDlg();

// Dialog Data
	enum { IDD = IDD_CABLE_TRAY_FILL_RATE_CALCULATION_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CLabelStatic	m_wndCableTraySystemStatic;
	CLabelStatic	m_wndDuctBankSystemStatic;
	CLabelStatic	m_wndCableTrayDuctFillingStatic;

	CMFCButtonEx	m_wndCalcButton;
	CMFCButtonEx	m_wndSaveButton;
	CMFCButtonEx	m_wndCancelButton;

	int		m_nCableTraySystem;
	int		m_nDuctBankSystemRadio;
	int		m_nCableTrayDuctFillingRadio;


	CComboBox		m_wndControlCableTrayLayerCombo;
	CComboBox		m_wndControlCableLayerCombo;

	CBoostBaseFilterEdit	m_wndDuctBankFillRatePercentageEdit;
public:
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedCalcButton();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedCancel();
	
	int InitEditBox(void);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	int SaveData(void);
	int DisplayData(void);
	int InitComboBox(void);
	int CalcCableTrayFillRate(void);
	int CreateTrayItem(vector<CCableItem*> CableItemEntry, const string& rCategory, const int& nLayer, int& nTrayCount, CRouteItem *pRouteItem);
	int DeleteAllTrayListInCableItem(void);
	int DeleteAllTrayItem(void);
	int DeleteCableItemInRoute(void);

	bool IsLastRouteType(const string& rRouteGroup, const string& rLastRouteType);
	CRouteItem* FindEndRouteItemOfRouteGroup(const string& rRouteGroup);
	int FillRouteCable(const string& rRouteType);

	CRouteItem* GetDirectionChildRouteItem(const string& rRouteType, const string& rRouteDirection);
	int TxtFileExport(void);

};
