#pragma once
#include "afxwin.h"
#include "ColorButton.h"
#include "MFCButtonEx.h"

// CCableBMSummaryDlg dialog

class CCableBMSummaryDlg : public CDialog
{
	DECLARE_DYNAMIC(CCableBMSummaryDlg)

public:
	CCableBMSummaryDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCableBMSummaryDlg();

	typedef struct
	{
		string rCableVolt;
		string rSpec;
		string rCore;
		string rSize;
		string rLength;
		string rCableCategory;
		CString rCableVoltType;
		string area;
		string rUnit;

		map<string , double> AreaLengthMap;
	}CABLE_ITEM_STRUCT;

// Dialog Data
	enum { IDD = IDD_CABLE_BM_SUMMARY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	int CreateListCtrl(void);

	afx_msg void OnBnClickedHvPowercableButton();
	afx_msg void OnBnClickedLvPowercableButton();
	afx_msg void OnBnClickedControlCableButton();

public:
	CListCtrl	m_wndHVPowerCableListCtrl;
	CListCtrl	m_wndLVPowerCableListCtrl;
	CListCtrl	m_wndControlCableListCtrl;

	CColorButton	m_wndHVPowerCableButton;
	CColorButton	m_wndLVPowerCableButton;
	CColorButton	m_wndControlCableButton;

	CMFCButtonEx	m_wndOkButton;
	CMFCButtonEx	m_wndCancelButton;
	CMFCButtonEx	m_wndReportButton;

	CComboBox	m_wndAreaCombo;
	CComboBox	m_wndUnitCombo;

	CEdit		m_wndTotalLengthEdit;
	CEdit		m_wndHVPowerCableBmEdit;
	CEdit		m_wndLVPowerCableBmEdit;
	CEdit		m_wndControlCableBmEdit;
	CEdit		m_wndTotalBmEdit;

	int	m_nHvpowerCableTotalLength;
	int	m_nLvpowerCableTotalLength;
	int	m_nControlCableTotalLength;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedOk();
	
	int DisplayCableData(const string& rArea = "", const string& rUnit = "");
	afx_msg void OnCbnSelchangeUnitCombo();
	int FillComboBox(void);
	
	afx_msg void OnBnClickedReportButton();
	BOOL CompareMatchCable(CABLE_ITEM_STRUCT record, vector<CABLE_ITEM_STRUCT> &CableItemEntry);
	afx_msg void OnCbnSelchangeAreaCombo();
	
	int WriteCableBMDataInTextFile();
private:
	BOOL CompareMatchCableInMap(CABLE_ITEM_STRUCT record, map<string , vector<CABLE_ITEM_STRUCT> >& CableItemMap);
};
