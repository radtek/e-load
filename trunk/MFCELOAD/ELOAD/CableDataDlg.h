#pragma once
#include "EloadPropSheet.h"
#include "fpspread1.h"
#include "MFCButtonEx.h"

// CCableDataDlg dialog

class CCableDataDlg : public CDialog
{
	DECLARE_DYNAMIC(CCableDataDlg)

public:
	CCableDataDlg();
	virtual ~CCableDataDlg();

// Dialog Data
	enum { IDD = IDD_CABLE_DATA };
	CComboBox m_cboCableType;
	CComboBox m_cboHertz;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CListCtrl  m_wndCableDataListCtrl;
	CFpspread1 m_wndSpread;
        CMFCButtonEx m_wndImportButton, m_wndExportButton , m_wndDeleteButton;
	CMFCButtonEx m_wndCancelButton;
	CToolTipCtrl* m_pTT;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonImport();
private:
	int ImportCableSizingExcelFile(const CString& rFilePath);
        int ExportCableSizingExcelFile(const CString& rFilePath, const CString& rHertz, const CString& rCableType);
public:
	afx_msg void OnCbnSelchangeComboCableType();
	int SaveData(void);
        afx_msg void OnBnClickedButtonDelete();
        int SetScrollSetting(void);
        afx_msg void OnBnClickedButtonExport();
	int LoadData(void);
private:
	int DisplayData(void);
	int DisplayCableData(const string& rTableName, const bool bForce = false);
private:
	int FillCableTypeAndHertzComboBox(void);
public:
	afx_msg void OnLvnKeydownListCableData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListCableData(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnCbnSelchangeComboHertz();

	CString m_rUGBaseTemp;
	CString m_rAGBaseTemp;
	CString m_rMaxConductorTemp;
};
