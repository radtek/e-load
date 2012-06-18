#pragma once


// CSchematicTypeCopyDlg dialog

class CSchematicTypeCopyDlg : public CDialog
{
	DECLARE_DYNAMIC(CSchematicTypeCopyDlg)

public:
	CSchematicTypeCopyDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSchematicTypeCopyDlg();

// Dialog Data
	enum { IDD = IDD_SCHEM_TYPE_COPY };
	
	CString m_rOldSchemTypeName , m_rNewSchemTypeName;
	CString m_rOldSchemTypeDesc , m_rNewSchemTypeDesc;

	CMFCButton m_wndOKButton , m_wndCancelButton;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
