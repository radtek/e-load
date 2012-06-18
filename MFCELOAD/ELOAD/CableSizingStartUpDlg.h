#pragma once
#include "Fpspread1.h"
#include "MFCButtonEx.h"
#include <gui/FilterEdit/BoostBaseFilterEdit.h>
#include "Tables/ConduitListTable.h"
#include "Tables/CableDataListTable.h"
#include "LabelStatic.h"

// CCableSizingStartUpDlg dialog

class CCableSizingStartUpDlg : public CDialog
{
	DECLARE_DYNAMIC(CCableSizingStartUpDlg)

public:
	CCableSizingStartUpDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCableSizingStartUpDlg();

// Dialog Data
	enum { IDD = IDD_CABLE_SIZING_START_UP };
	CString m_rBusMotorVoltage[2];
	CString m_rHertz;
	CString m_rDeratingFactor;
	CString m_rVoltageDrop[2];
	CString m_rStartingVoltageDrop[2];
	CString m_rStartingPowerFactor;
	CString m_rInsulationMethod;
	CString m_rCableType;
	CString m_rCableDataTable , m_rConduitTable;
	CString m_rMotorType , m_rPFForFeeder;
	CString m_rSelectBus;

	CImageList m_imgList;

	CComboBox m_wndDataNameCombo;
	CString m_rDataName;

	CBoostBaseFilterEdit m_wndPFForFeederEdit;
	CMFCButtonEx m_wndSelectCondButton;
	CListCtrl  m_wndConduitTableListCtrl;

	CMFCButtonEx m_wndOKButton , m_wndCancelButton;
	CLabelStatic m_wndSelectLoadTypeButton;	//! 2011.02.15 added by Humkyung
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnUpdateOk(CCmdUI* pCmdUI);	//! 2011.02.15 added by HumKyung
	virtual BOOL OnInitDialog();
private:
	CFont m_font;	//! 2011.02.15 added by HumKyung

	int LoadData(void);
	int SaveData(const CString& rDataName);
public:
	afx_msg void OnBnClickedButtonSelectCond();
private:
	int ImportFLCExcelFile(const CString& rFilePath);
	int ImportConduitExcelFile(const CString& rFilePath);
	int DisplayConduitData(void);
public:
	afx_msg void OnCbnSelchangeComboBus();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnEnChangeEditVoltageDrop1();
	afx_msg void OnEnChangeEditStartingVoltageDrop1();
	afx_msg void OnLvnItemchangedListConduitTable(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListConduitTable(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelendokComboCableDataTable();
	afx_msg void OnCbnSelendokComboMotorType();
	afx_msg void OnCbnSelchangeComboMotorType();
private:
	// fill
	int FillCableTypeComboBox(void);
public:
	afx_msg void OnCbnSelchangeComboCableDataTable();
	afx_msg void OnCbnSelendokComboDataName();
};
