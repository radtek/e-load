#pragma once
#include "MFCButtonEx.h"
#include "ControlCableGridCtrl.h"
#include "LabelStatic.h"
#include "afxwin.h"

// CControlCableSpecCreationDlg dialog

class CControlCableSpecCreationDlg : public CDialog
{
	DECLARE_DYNAMIC(CControlCableSpecCreationDlg)

public:
	CControlCableSpecCreationDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CControlCableSpecCreationDlg();

// Dialog Data
	enum { IDD = IDD_CONTROL_CABLE_SPEC_CREATION };
	CControlCableGridCtrl	m_wndMainCableGridCtrl;

	CLabelStatic m_ControlCableSettingStatic;

	CMFCButtonEx	m_wndMainCableDeleteButton;
	CMFCButtonEx	m_wndMainCableAddButton;
	CMFCButtonEx	m_wndCancelButton;
	CMFCButtonEx	m_wndOKButton;
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	int InsertItemInMainCableGridCtrl(const CString& rCableSpec ="", const CString& rCableCore ="", const CString& rCableSize = "", const CString& rCableOD= "", const CString& rCableWeight ="");
	int CreateMainCableGridCtrl(void);
	int DisplayMainCableData(void);
public:
	afx_msg void OnBnClickedMaincableDeleteButton();
	afx_msg void OnBnClickedMaincableAddButton();
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedOk();
};
