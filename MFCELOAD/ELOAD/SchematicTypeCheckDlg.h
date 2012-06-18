#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "MFCButtonEx.h"

// CSchematicTypeCheckDlg dialog

class CSchematicTypeCheckDlg : public CDialog
{
	DECLARE_DYNAMIC(CSchematicTypeCheckDlg)

public:
	CSchematicTypeCheckDlg(const CString& rSchematicType , CWnd* pParent = NULL);   // standard constructor
	virtual ~CSchematicTypeCheckDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_SCHEMATIC_TYPE_CHECK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl		m_wndSchematicTypeListCtrl;
	CMFCButtonEx		m_wndOkButton;
	HICON		m_hIcon;

	virtual BOOL OnInitDialog();
	int DisplaySchematicType(void);

private:
	CString		m_rSchematicType;
};
