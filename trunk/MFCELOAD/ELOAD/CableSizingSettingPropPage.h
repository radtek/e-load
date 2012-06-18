#pragma once
#include "ELoadPropSheet.h"
#include <gui/XButtonXP/XButtonXP.h>
#include <gui/GradientStatic.h>
#include "afxwin.h"

// CCableSizingSettingPropPage dialog

class CCableSizingSettingPropPage : public CELOADPropPage
{
	DECLARE_DYNAMIC(CCableSizingSettingPropPage)

public:
	CCableSizingSettingPropPage();
	virtual ~CCableSizingSettingPropPage();

// Dialog Data
	enum { IDD = IDD_CABLE_SIZING_SETTING };

	CGradientStatic m_wnd3PHMotorStatic;
	CGradientStatic m_wnd3PHFeederStatic;
	CGradientStatic m_wnd1PHFeederStatic;
	CGradientStatic m_wndDCFeederStatic;

	CXButtonXP m_wnd3PHMotorButton;
	CXButtonXP m_wnd3PHFeederButton;
	CXButtonXP m_wnd1PHFeederButton;
	CXButtonXP m_wndDCFeederButton;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton3phMotor();
	afx_msg void OnBnClickedButton3phFeeder();
	afx_msg void OnBnClickedButton1phFeeder();
	afx_msg void OnBnClickedButtonDcFeeder();
private:
	int SetCableSizeExcelFile(const CString& rTableName, const CString& rExcelFilePath);
public:
        virtual BOOL OnInitDialog();
};
