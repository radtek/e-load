// SelectTemplateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "TemplateMappingDlg.h"
#include "ELoadDocData.h"


// CTemplateMappingDlg dialog

IMPLEMENT_DYNAMIC(CTemplateMappingDlg, CDialog)

CTemplateMappingDlg::CTemplateMappingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTemplateMappingDlg::IDD, pParent)
{

}

CTemplateMappingDlg::~CTemplateMappingDlg()
{
}

void CTemplateMappingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	DDX_Control(pDX, IDC_COMBO_USER_CABLE_TEMPLATE , m_wndUserCableTemplateCombo);
	DDX_Control(pDX, IDC_COMBO_LOAD_LIST_TEMPLATE , m_wndLoadListTemplateCombo);
	DDX_Control(pDX, IDC_COMBO_CABLE_SCHEDULE_TEMPLATE , m_wndCableScheduleTemplateCombo);
	DDX_Control(pDX, IDC_COMBO_DRUM_SCHEDULE_TEMPLATE , m_wndDrumScheduleTemplateCombo);

	DDX_Control(pDX, IDOK, m_wndOkButton);
	DDX_Control(pDX, IDCANCEL, m_wndCancelButton);
}


BEGIN_MESSAGE_MAP(CTemplateMappingDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CTemplateMappingDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CTemplateMappingDlg message handlers

BOOL CTemplateMappingDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	return CDialog::PreTranslateMessage(pMsg);
}

/**
	@brief	

	@author BHK	

	@date 2009-07-30 오전 9:51:55	

	@param	

	@return		
*/
BOOL CTemplateMappingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CELoadDocData& docData = CELoadDocData::GetInstance();
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	m_wndOkButton.SetIcon(IDI_OK_18X18);
	m_wndOkButton.SetTextHotColor(TextHotColor);

	m_wndCancelButton.SetIcon(IDI_CANCEL_18X18);
	m_wndCancelButton.SetTextHotColor(TextHotColor);

	const string rMDBFilePath = docData.GetProjectMDBFilePath();

	CADODB adoDB;
	const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rConnectionString))
	{
		adoDB.OpenQuery(_T("SELECT * FROM T_EXCEL_REPORT_FORMAT"));
		LONG lRecordCount = 0L;
		STRING_T rValue;
		adoDB.GetRecordCount(&lRecordCount);
		for(int i=0;i< lRecordCount;++i)
		{
			adoDB.GetFieldValue(i, _T("C_TEMPLATE_NAME"), &rValue);
			if(-1 == m_wndUserCableTemplateCombo.FindString(-1, rValue.c_str()))
			{
				m_wndUserCableTemplateCombo.AddString(rValue.c_str());
			}
			if(-1 == m_wndLoadListTemplateCombo.FindString(-1, rValue.c_str()))
			{
				m_wndLoadListTemplateCombo.AddString(rValue.c_str());
			}
			if(-1 == m_wndCableScheduleTemplateCombo.FindString(-1, rValue.c_str()))
			{
				m_wndCableScheduleTemplateCombo.AddString(rValue.c_str());
			}
			if(-1 == m_wndDrumScheduleTemplateCombo.FindString(-1, rValue.c_str()))
			{
				m_wndDrumScheduleTemplateCombo.AddString(rValue.c_str());
			}
		}

		//! 
		CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
		if(pSettingTable)
		{
			m_rUserCableTemplate = pSettingTable->m_RecordEntry[0].FieldValueMap[_T("C_USER_CABLE_TEMPLATE")].c_str();
			{
				const int nItem = m_wndUserCableTemplateCombo.FindString(-1 , m_rUserCableTemplate);
				if(-1 != nItem) m_wndUserCableTemplateCombo.SetCurSel(nItem);
			}

			m_rLoadListTemplate = pSettingTable->m_RecordEntry[0].FieldValueMap[_T("C_LOAD_LIST_TEMPLATE")].c_str();
			{
				const int nItem = m_wndLoadListTemplateCombo.FindString(-1 , m_rLoadListTemplate);
				if(-1 != nItem) m_wndLoadListTemplateCombo.SetCurSel(nItem);
			}

			m_rCableScheduleTemplate = pSettingTable->m_RecordEntry[0].FieldValueMap[_T("C_CABLE_SCHEDULE_TEMPLATE")].c_str();
			{
				const int nItem = m_wndCableScheduleTemplateCombo.FindString(-1 , m_rCableScheduleTemplate);
				if(-1 != nItem) m_wndCableScheduleTemplateCombo.SetCurSel(nItem);
			}

			m_rDrumScheduleTemplate = pSettingTable->m_RecordEntry[0].FieldValueMap[_T("C_DRUM_SCHEDULE_TEMPLATE")].c_str();
			{
				const int nItem = m_wndDrumScheduleTemplateCombo.FindString(-1 , m_rDrumScheduleTemplate);
				if(-1 != nItem) m_wndDrumScheduleTemplateCombo.SetCurSel(nItem);
			}
		}

		return ERROR_SUCCESS;
	}
	return ERROR_BAD_ENVIRONMENT;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	설정한 Template 이름을 database에 저장한다.

	@author BHK	

	@date 2009-07-30 오전 9:41:22	

	@param	

	@return		
*/
void CTemplateMappingDlg::OnBnClickedOk()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
	if(NULL != pSettingTable)
	{
		{
			const int nItem = m_wndUserCableTemplateCombo.GetCurSel();
			if( -1 != nItem)
			{
				m_wndUserCableTemplateCombo.GetLBText(nItem, m_rUserCableTemplate);
				pSettingTable->m_RecordEntry[0].FieldValueMap[_T("C_USER_CABLE_TEMPLATE")] = m_rUserCableTemplate;
			}
		}
		{
			const int nItem = m_wndLoadListTemplateCombo.GetCurSel();
			if( -1 != nItem)
			{
				m_wndLoadListTemplateCombo.GetLBText(nItem, m_rLoadListTemplate);
				pSettingTable->m_RecordEntry[0].FieldValueMap[_T("C_LOAD_LIST_TEMPLATE")] = m_rLoadListTemplate;
			}
		}
		{
			const int nItem = m_wndCableScheduleTemplateCombo.GetCurSel();
			if( -1 != nItem)
			{
				m_wndCableScheduleTemplateCombo.GetLBText(nItem, m_rCableScheduleTemplate);
				pSettingTable->m_RecordEntry[0].FieldValueMap[_T("C_CABLE_SCHEDULE_TEMPLATE")] = m_rCableScheduleTemplate;
			}
		}
		{
			const int nItem = m_wndDrumScheduleTemplateCombo.GetCurSel();
			if( -1 != nItem)
			{
				m_wndDrumScheduleTemplateCombo.GetLBText(nItem, m_rDrumScheduleTemplate);
				pSettingTable->m_RecordEntry[0].FieldValueMap[_T("C_DRUM_SCHEDULE_TEMPLATE")] = m_rDrumScheduleTemplate;
			}
		}
		pSettingTable->Save();
	}

	OnOK();
}
