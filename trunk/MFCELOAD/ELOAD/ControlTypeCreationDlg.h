#pragma once
#include "MFCButtonEx.h"
#include <gui/gridctrl/GridCtrl.h>
#include "afxwin.h"
#include "atltypes.h"
#include <gui/FilterEdit/BoostBaseFilterEdit.h>
#include "Tables/ELOADTable.h"
#include "GridCtrlEx.h"
#include "ColorButton.h"
#include "WizardPropertyPage.h"

// CControlTypeCreationDlg dialog

class CControlTypeCreationDlg : public CWizardPropertyPage
{
	DECLARE_DYNAMIC(CControlTypeCreationDlg)

public:
	CControlTypeCreationDlg(const string& rInitCategory = _T("LCS or LOCAL TYPE") , CWnd* pParent = NULL);   // standard constructor
	virtual ~CControlTypeCreationDlg();

// Dialog Data
	enum { IDD = IDD_CONTROL_TYPE_CREATION_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CGridCtrl	m_wndLscOrLocalTypeGridCtrl;
	CGridCtrl	m_wndLcpIoTypeGridCtrl;
	CGridCtrl	m_wndDcsIoTypeGridCtrl;

	CMFCTabCtrl	m_wndControlTypeCreationTabCtrl;

	CMFCButtonEx	m_wndCancelButton;
	CMFCButtonEx	m_wndOkButton;
	CMFCButtonEx	m_wndAddButton;
	CMFCButtonEx	m_wndDeleteButton;
	CMFCButtonEx	m_wndBackButton;
	CMFCButtonEx	m_wndNextButton;

	CColorButton	m_wndLCSTypeButton;
	CColorButton	m_wndLCPTypeButton;
	CColorButton	m_wndDCSTypeButton;

public:
	int CreateTabCtrl(void);
	int CreateLscOrLocalTypeGridCtrl(void);
	int CreateLcpIoTypeGridCtrl(void);
	int CreateDcsIoTypeGridCtrl(void);
	int InsertItemInLscOrLocalTypeGridCtrl(const CString& rLscType = "", const CString& rLscDesc = "");
	int InsertItemInLcpIoTypeGridCtrl(const CString& rLcpIoType = "", const CString& rLcpIoDesc ="");
	int InsertItemDcsIoTypeGridCtrl(const CString& rDcsIoType= "", const CString& rIoType = "", const CString& rDcsIoDesc="", const CString& rPrefix="", const CString& rLoadId ="\"LOAD ID\"");
private:
	int m_nPageIndex;
	string m_rCurCategory;
	CGridCtrl* GetGridCtrlOf(const string& rCategory);
	int InsertItemIntoGridCtrl(CGridCtrl* pGridCtrl , TABLES::CELOADTable::Record& record);
public:
	virtual BOOL OnInitDialog();
	void OnControltypecreationInsert();
	void OnControltypecreationDelete();
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedFromMaster();
	int DisplayData(void);
	int SaveData(void);
	afx_msg void OnBnDropDownLcsButton(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedLcsButton();
	afx_msg void OnBnClickedLcpButton();
	afx_msg void OnBnClickedDcsButton();
	afx_msg void OnBnClickedWizback();
	afx_msg void OnBnClickedWiznext();
	afx_msg void OnBnClickedCancel();

	BOOL Init(void);
};
