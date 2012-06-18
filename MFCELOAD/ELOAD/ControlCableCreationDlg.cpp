// ControlCableCreationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "ControlCableCreationDlg.h"
#include "ELoadDocData.h"

// CControlCableCreationDlg dialog

IMPLEMENT_DYNAMIC(CControlCableCreationDlg, CDialog)

CControlCableCreationDlg::CControlCableCreationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CControlCableCreationDlg::IDD, pParent)
	, m_nLoadControlCableRadio(1)
{

}

CControlCableCreationDlg::~CControlCableCreationDlg()
{
}

void CControlCableCreationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX , IDC_EDIT_DCS_DESIGN_LENGTH , m_rDCSDesignLength);
	DDX_Control(pDX , IDOK , m_wndOKButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);
	DDX_Radio(pDX, IDC_RADIO_ALL_CONTROL_CABLE, m_nLoadControlCableRadio);
	DDV_MinMaxInt(pDX, m_nLoadControlCableRadio, 0, 1);
}


BEGIN_MESSAGE_MAP(CControlCableCreationDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CControlCableCreationDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CControlCableCreationDlg message handlers
/**
	@brief	

	@author BHK	

	@date 2009-06-09 오후 2:39:55	

	@param	

	@return		
*/
BOOL CControlCableCreationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CELoadDocData& docData = CELoadDocData::GetInstance();
	CProjectSettingTable* pSettingTable = (CProjectSettingTable*)docData.GetTableOf(CProjectSettingTable::GetTableName());
	if(pSettingTable)
	{
		m_nLoadControlCableRadio = atoi(pSettingTable->m_RecordEntry[0].FieldValueMap[_T("C_LOAD_CONTROL_CABLE_RADIO")].c_str());
		if(0 == m_nLoadControlCableRadio)
		{
			CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_ALL_CONTROL_CABLE);
			if(pButton) pButton->SetCheck(TRUE);
		}
		else
		{
			CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_UNSIZED_CONTROL_CABLE_ONLY);
			if(pButton) pButton->SetCheck(TRUE);
		}
		UpdateData(FALSE);

		const string rDesignLength = pSettingTable->m_RecordEntry[0].FieldValueMap[_T("C_DCS_IO_TYPE_DESIGN_LENGTH")];
		SetDlgItemText(IDC_EDIT_DCS_DESIGN_LENGTH , rDesignLength.c_str());
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	입력한 Design Length값을 저장하도록 한다.

	@author BHK	

	@date 2009-06-09 오후 2:44:17	

	@param	

	@return		
*/
void CControlCableCreationDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	CELoadDocData& docData = CELoadDocData::GetInstance();
	CProjectSettingTable* pSettingTable = (CProjectSettingTable*)docData.GetTableOf(CProjectSettingTable::GetTableName());
	if(pSettingTable)
	{
		string rControlCableRadio = _T("0");
		CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_ALL_CONTROL_CABLE);
		if(pButton && (TRUE == pButton->GetCheck()))
			rControlCableRadio = _T("0");
		else	rControlCableRadio = _T("1");
		map<string,string>::iterator where = pSettingTable->m_RecordEntry[0].FieldValueMap.find(_T("C_LOAD_CONTROL_CABLE_RADIO"));
		if(where != pSettingTable->m_RecordEntry[0].FieldValueMap.end()) where->second = rControlCableRadio;

		CString rString;
		GetDlgItemText(IDC_EDIT_DCS_DESIGN_LENGTH , rString);
		where = pSettingTable->m_RecordEntry[0].FieldValueMap.find(_T("C_DCS_IO_TYPE_DESIGN_LENGTH"));
		if(where != pSettingTable->m_RecordEntry[0].FieldValueMap.end()) where->second = rString.operator LPCSTR();

		pSettingTable->Save();
	}

	if(0 == m_nLoadControlCableRadio)
	{
		if(IDOK != AfxMessageBox(_T("All control cables will be deleted. Do you want to execute?") , MB_OKCANCEL| MB_ICONWARNING))
		{
			OnCancel();
			return;
		}
	}

	OnOK();
}
