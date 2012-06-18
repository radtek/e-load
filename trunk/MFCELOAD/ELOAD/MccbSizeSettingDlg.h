#pragma once

#include "ControlCableGridCtrl.h"
#include "ColorButton.h"
#include "MFCButtonEx.h"
#include "Tables/ELOADTable.h"
#include "afxwin.h"

// CMccbSizeSettingDlg dialog

class CReflectionStatic : public CStatic
{
	DECLARE_DYNAMIC(CReflectionStatic)
public:
	CReflectionStatic(CWnd* pParent = NULL){}
	virtual ~CReflectionStatic(){}

protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};

class CMccbSizeSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CMccbSizeSettingDlg)

public:
	CMccbSizeSettingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMccbSizeSettingDlg();

// Dialog Data
	enum { IDD = IDD_MCCB_SIZE_SETTING_DIALOG };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	CGridCtrl	m_wndLV1GridCtrl, m_wndLV1MGridCtrl;
	CGridCtrl	m_wndLV2GridCtrl, m_wndLV2MGridCtrl;
	CGridCtrl	m_wndLV11GridCtrl, m_wndLV11MGridCtrl;
	CGridCtrl	m_wndLV12GridCtrl, m_wndLV12MGridCtrl;
	CGridCtrl	m_wndDCGridCtrl, m_wndDCMGridCtrl;

	CReflectionStatic	m_wndMotorStatic, m_wndFeederStatic;
	
	CMFCTabCtrlEx	m_wndTabCtrl;

	CColorButton m_wndLV1Button , m_wndLV2Button;
	CColorButton m_wndLV11Button , m_wndLV12Button;
	CColorButton m_wndDCButton;

	CMFCButtonEx m_wndAddButton;
	CMFCButtonEx m_wndSaveButton;
	CMFCButtonEx m_wndDeleteButton;
	CMFCButtonEx m_wndApplyButton;
	CMFCButtonEx m_wndApplyAllLoadsAndSaveButton;
	CMFCButtonEx m_wndCloseButton;

	BOOL m_bAllLoads;

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	int CreateMccbTabCtrl(void);
	afx_msg void OnBnClickedCancel();
	int AddItemInGridCtrl(CGridCtrl& GridCtrl, const int& nRow, TABLES::CELOADTable::Record& record);
	int InsertItemInApplyGridCtrl(CControlCableGridCtrl& GridCtrl, const int& nRow , const CString& rCheck, const CString& rBusId, const CString& rBusVolt, const CString& rLoadVolt);
	int CreateApplyBusGridCtrl(void);
	int CreateApplyPdbGridCtrl(void);
	
	afx_msg void OnBnClickedMccbSizeSettingCheck();
	afx_msg void OnBnClickedAddButton();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedSaveButton();
private:
	string m_rCurVoltType;

	int CreateDCGridCtrl(void);
	int CreateLV12GridCtrl(void);
	int CreateLV11GridCtrl(void);
	int CreateLV2GridCtrl(void);
	int CreateLV1GridCtrl(void);
	int DisplayTableData(void);
private:
	int DisplayVoltageSetting();
	//int DisplayBusData(void);
public:
	afx_msg void OnBnClickedApplySave();
private:
	double GetLoadVoltOf(const string& rVoltType);
	CGridCtrl* GetGridCtrlOf(const string& rVoltType, const int& nTab);
public:
	afx_msg void OnBnClickedButtonLv1();
	afx_msg void OnBnClickedButtonLv2();
	afx_msg void OnBnClickedButtonLv11();
	afx_msg void OnBnClickedButtonLv12();
	afx_msg void OnBnClickedButtonDc();
	afx_msg void OnStnClickedStaticDcLoadVolt();
	afx_msg void OnEnChangeEditPf();
	afx_msg void OnEnChangeEditSf();
private:
	int ChangeFLCAndSF(CGridCtrl* pGridCtrl , const int& nRow);
public:
	afx_msg void OnBnClickedButtonApply();
};
