// SchematicTypeInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "ControlCableTypeInfoDlg.h"
#include "ELoadDocData.h"



// CControlCableTypeInfoDlg dialog

IMPLEMENT_DYNAMIC(CControlCableTypeInfoDlg, CDialog)

CControlCableTypeInfoDlg::CControlCableTypeInfoDlg(const CString& rOldCableType , const CString& rOldPrefix , const CString& rOldSuffix, const CString& rOldDesc,  CWnd* pParent)
	: CDialog(CControlCableTypeInfoDlg::IDD, pParent) , m_rOldCableType(rOldCableType), m_rOldPrefix(rOldPrefix), m_rOldSuffix(rOldSuffix), m_rOldDesc(rOldDesc)
{

}

CControlCableTypeInfoDlg::~CControlCableTypeInfoDlg()
{
}

void CControlCableTypeInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX , IDOK , m_wndOKButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);

	DDX_Control(pDX, IDC_BEFORE_CABLE_TYPE_EDIT, m_wndBeforeCableTypeEdit);
	DDX_Control(pDX, IDC_AFTER_CONTROL_CABLE_SPEC_COMBO, m_wndAfterCableTypeCombo);

	DDX_Control(pDX, IDC_AFTER_CABLE_PREFIX_EDIT, m_wndAfterPrefixEdit);
	DDX_Control(pDX, IDC_BEFORE_CABLE_PREFIX_EDIT, m_wndBeforePrefixEdit);

	DDX_Control(pDX, IDC_AFTER_CABLE_SUFFIX_EDIT, m_wndAfterSuffixEdit);
	DDX_Control(pDX, IDC_BEFORE_CABLE_SUFFIX_EDIT, m_wndBeforeSuffixEdit);

	DDX_Control(pDX, IDC_AFTER_EDIT_CABLE_DESC, m_wndAfterDescEdit);
	DDX_Control(pDX, IDC_BEFORE_EDIT_CABLE_DESC, m_wndBeforeDescEdit);
}


BEGIN_MESSAGE_MAP(CControlCableTypeInfoDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CControlCableTypeInfoDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CControlCableTypeInfoDlg message handlers
/**
	@brief	

	@author KHS	

	@date 2009-05-13 오후 8:24:22	

	@param	

	@return		
**/
BOOL CControlCableTypeInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_wndBeforeCableTypeEdit.SetWindowText(m_rOldCableType);
	m_wndBeforePrefixEdit.SetWindowText(m_rOldPrefix);
	m_wndBeforeSuffixEdit.SetWindowText(m_rOldSuffix);
	m_wndBeforeDescEdit.SetWindowText(m_rOldDesc);
	

	m_wndAfterPrefixEdit.SetWindowText(m_rOldPrefix);
	m_wndAfterSuffixEdit.SetWindowText(m_rOldSuffix);
	m_wndAfterDescEdit.SetWindowText(m_rOldDesc);

	FillControlCableTypeComboBox();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	Cable Type ComboBox를 채운다.

	@author KHS	

	@date 2009-05-13 오후 8:26:53	

	@param	

	@return		
**/
int CControlCableTypeInfoDlg::FillControlCableTypeComboBox(void)
{
	m_wndAfterCableTypeCombo.ResetContent();

	const int nItemCount = m_wndAfterCableTypeCombo.GetCount();
	for(int i = nItemCount - 1;i >= 0;--i) m_wndAfterCableTypeCombo.DeleteString(i);

	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pTable = docData.GetTableOf(_T("T_CONTROL_CABLE_SETTING"));
	if(pTable)
	{
		for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin(); itr != pTable->m_RecordEntry.end();++itr)
		{
			const CString rCableType	= itr->FieldValueMap[_T("C_SPEC")].c_str();
			const CString rCableCore	= itr->FieldValueMap[_T("C_CORE")].c_str();
			const CString rCableSize	= itr->FieldValueMap[_T("C_SIZE")].c_str();
			const CString rControlCableSpec = rCableType + _T(" ") + rCableCore + _T("-") + rCableSize;
			
			m_wndAfterCableTypeCombo.AddString(rControlCableSpec);
		}

		int nItem = m_wndAfterCableTypeCombo.FindString(0, m_rOldCableType);
		if( -1 != nItem)
		{
			m_wndAfterCableTypeCombo.SetCurSel(nItem);
		}
		else
		{
			m_wndAfterCableTypeCombo.SetCurSel(0);
		}

		return ERROR_SUCCESS;
	}
	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author KHS	

	@date 2009-05-19 오후 3:06:38	

	@param	

	@return		
**/
void CControlCableTypeInfoDlg::OnBnClickedOk()
{
	int nSelItem = m_wndAfterCableTypeCombo.GetCurSel();
	if(nSelItem != -1)
	{
		m_wndAfterCableTypeCombo.GetLBText(nSelItem, m_rCableType);
	}
	m_wndAfterSuffixEdit.GetWindowText(m_rSuffix);
	m_wndAfterPrefixEdit.GetWindowText(m_rPrefix);
	m_wndAfterDescEdit.GetWindowText(m_rDesc);

	OnOK();
}

BOOL CControlCableTypeInfoDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_KEYDOWN) 
	{
		if(pMsg->wParam == VK_ESCAPE) return TRUE;
		if(pMsg->wParam == VK_RETURN) return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}
