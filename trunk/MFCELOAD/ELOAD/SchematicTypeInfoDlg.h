#pragma once

// CSchematicTypeInfoDlg dialog
#include "MFCButtonEx.h"

class CSchematicTypeInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CSchematicTypeInfoDlg)

public:
	CSchematicTypeInfoDlg(const CString& rOldSchemType , const CString& rOldSchemDesc , CWnd* pParent = NULL);   // standard constructor
	virtual ~CSchematicTypeInfoDlg();

// Dialog Data
	enum { IDD = IDD_SCHEMATIC_TYPE_INFO };

	CString m_rOldSchemType , m_rOldSchemDesc;
	CString m_rSchemType , m_rSchemDesc;

	CMFCButtonEx m_wndOKButton , m_wndCancelButton;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
