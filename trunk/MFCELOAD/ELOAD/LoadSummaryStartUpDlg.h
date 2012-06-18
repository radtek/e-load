#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "MFCButtonEx.h"
#include <gui/FilterEdit/UIntRangeEdit.h>
#include <gui/gridctrl/GridCtrl.h>

// CLoadSummaryStartUpDlg dialog
class CLoadSummaryStartUpDlg : public CDialog
{
	DECLARE_DYNAMIC(CLoadSummaryStartUpDlg)

public:
	CLoadSummaryStartUpDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLoadSummaryStartUpDlg();

// Dialog Data
	enum { IDD = IDD_LOADSUMMARYSTARTUPDLG };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
        virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOK();
        afx_msg void OnBnClickedRun();
        afx_msg void OnBnClickedCancel();

	CGridCtrl       m_GridCtrl;
	CGridCtrl	m_SubBUSGridCtrl;
	CMFCTabCtrlEx	m_LoadSummaryTab;
	CEdit		m_wndDocNoEdit;

        CMFCButtonEx m_wndRunButton , m_wndCancelButton;
private:
        static BOOL __CHECKAPPLYALL__;
private:
	int FillGridCtrl(void);
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	int CreateTabCtrl(void);
	int CreateSubBUSGridCtrl(void);
	int FillPdbGridCtrl(void);
	int CreateBusGridCtrl(void);
	int SubBUSRun(void);
	afx_msg void OnBnClickedCheckApplyConnectedTransformer();
};
