#pragma once

#include <gui/gridctrl/GridCtrl.h>
#include "ELoadPropSheet.h"
#include "MFCButtonEx.h"
#include <gui/GradientStatic.h>
#include "afxwin.h"

// CCableSizingSettingDlg dialog

class CCableSizingSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CCableSizingSettingDlg)

public:
	CCableSizingSettingDlg(CWnd* pParent = NULL);
	virtual ~CCableSizingSettingDlg();

// Dialog Data
	enum { IDD = IDD_CABLE_SIZING_SETTING };
	CMFCTabCtrlEx m_wndTabCtrl;
	
	CGridCtrl m_wnd3PHMotorGridCtrl;
	CGridCtrl m_wnd3PHFeederGridCtrl;
	CGridCtrl m_wnd1PHFeederGridCtrl;
	CGridCtrl m_wndDCFeederGridCtrl;

	CMFCButtonEx m_wndAddButton;
	CMFCButtonEx m_wndDeleteButton;
	CMFCButtonEx m_wndImportExcelButton;

	CMFCButtonEx m_wndOKButton;
	CMFCButtonEx m_wndCancelButton;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	int SetCableSizeExcelFile(const int& nIndex , const CString& rExcelFilePath);
public:
        virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
private:
	int BuildTabCtrl(void);
public:
	afx_msg void OnBnClickedButtonDelete();
private:
	int DisplayTableData(const int& nIndex);
public:
	afx_msg void OnBnClickedButtonImportFromExcel();
	afx_msg void OnBnClickedButtonAdd();
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};
