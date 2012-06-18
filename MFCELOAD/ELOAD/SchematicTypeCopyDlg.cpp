// SchematicTypeCopyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "SchematicTypeCopyDlg.h"


// CSchematicTypeCopyDlg dialog

IMPLEMENT_DYNAMIC(CSchematicTypeCopyDlg, CDialog)

CSchematicTypeCopyDlg::CSchematicTypeCopyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSchematicTypeCopyDlg::IDD, pParent)
{

}

CSchematicTypeCopyDlg::~CSchematicTypeCopyDlg()
{
}

void CSchematicTypeCopyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX , IDC_EDIT_OLD_SCHEM_TYPE_NAME , m_rOldSchemTypeName);
	DDX_Text(pDX , IDC_EDIT_NEW_SCHEM_TYPE_NAME , m_rNewSchemTypeName);
	DDX_Text(pDX , IDC_EDIT_OLD_SCHEM_TYPE_DESC , m_rOldSchemTypeDesc);
	DDX_Text(pDX , IDC_EDIT_NEW_SCHEM_TYPE_DESC , m_rNewSchemTypeDesc);

	DDX_Control(pDX , IDOK , m_wndOKButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);
}


BEGIN_MESSAGE_MAP(CSchematicTypeCopyDlg, CDialog)
END_MESSAGE_MAP()


// CSchematicTypeCopyDlg message handlers
