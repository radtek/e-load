// SchematicTypeInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "ELoadDocData.h"
#include "SchematicTypeInfoDlg.h"


// CSchematicTypeInfoDlg dialog

IMPLEMENT_DYNAMIC(CSchematicTypeInfoDlg, CDialog)

CSchematicTypeInfoDlg::CSchematicTypeInfoDlg(const CString& rOldSchemType , const CString& rOldSchemDesc , CWnd* pParent /*=NULL*/)
	: CDialog(CSchematicTypeInfoDlg::IDD, pParent) , m_rOldSchemType(rOldSchemType) , m_rOldSchemDesc(rOldSchemDesc)
{
	m_rSchemType = m_rOldSchemType;
	m_rSchemDesc = m_rOldSchemDesc;
}

CSchematicTypeInfoDlg::~CSchematicTypeInfoDlg()
{
}

void CSchematicTypeInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX , IDC_SCHEMATIC_TYPE_EDIT , m_rSchemType);
	DDX_Text(pDX , IDC_SCHEMATIC_DESC_EDIT , m_rSchemDesc);
	
	DDX_Control(pDX , IDOK , m_wndOKButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);
}


BEGIN_MESSAGE_MAP(CSchematicTypeInfoDlg, CDialog)
END_MESSAGE_MAP()


// CSchematicTypeInfoDlg message handlers
/**
	@brief	

	@author BHK	

	@date 2009-05-13 ¿ÀÈÄ 5:13:52	

	@param	

	@return		
*/
BOOL CSchematicTypeInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetDlgItemText(IDC_OLD_SCHEMATIC_TYPE_EDIT , m_rOldSchemType);
	SetDlgItemText(IDC_OLD_SCHEMATIC_DESC_EDIT , m_rOldSchemDesc);

	CELoadDocData& docData = CELoadDocData::GetInstance();
	//! BUTTON ICON SETTING
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));

	m_wndOKButton.SetIcon(IDI_OK_18X18);
	m_wndCancelButton.SetIcon(IDI_CANCEL_18X18);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
