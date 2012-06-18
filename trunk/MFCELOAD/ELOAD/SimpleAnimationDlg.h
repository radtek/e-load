#pragma once
#include "afxwin.h"
#include "PictureEx.h"

// CSimpleAnimationDlg dialog

class CSimpleAnimationDlg : public CDialog
{
	CSimpleAnimationDlg(CWnd* pParent = NULL);	//! standard constructor

	DECLARE_DYNAMIC(CSimpleAnimationDlg)
public:
	static CSimpleAnimationDlg& GetInstance();
	virtual ~CSimpleAnimationDlg();

// Dialog Data
	enum { IDD = IDD_SIMPLE_ANIMATION };
	CWinThread* m_pThread;

	bool Create(CWnd * pWndParent);
	volatile LONG m_bThreadRunning;
	static void PassMsgLoop(bool bEnableOnIdleCalls	);
protected:
	virtual BOOL DestroyWindow();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	CPictureEx m_Picture;
};
