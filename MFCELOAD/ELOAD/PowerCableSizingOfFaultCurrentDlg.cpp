// PowerCableSizingOfFaultCurrentDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "PowerCableSizingOfFaultCurrentDlg.h"
#include "Tables/CableCreationSettingTable.h"
#include "ELoadDocData.h"

#include <sstream>
// CPowerCableSizingOfFaultCurrentDlg dialog

int     CPowerCableSizingOfFaultCurrentDlg::__CABLE_CONDUCTOR__     = 0;
int     CPowerCableSizingOfFaultCurrentDlg::__INSULATION_MATERIAL__ = 1;
CString CPowerCableSizingOfFaultCurrentDlg::__POWER_CABLE_SIZING__  = _T("0");
CString CPowerCableSizingOfFaultCurrentDlg::__FAULT_CLEARING_TIME__ = _T("");
CString CPowerCableSizingOfFaultCurrentDlg::__FAULT_CURRENT__       = _T("");

IMPLEMENT_DYNAMIC(CPowerCableSizingOfFaultCurrentDlg, CDialog)

CPowerCableSizingOfFaultCurrentDlg::CPowerCableSizingOfFaultCurrentDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPowerCableSizingOfFaultCurrentDlg::IDD, pParent)
{
}

CPowerCableSizingOfFaultCurrentDlg::~CPowerCableSizingOfFaultCurrentDlg()
{
}

void CPowerCableSizingOfFaultCurrentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Radio(pDX , IDC_RADIO_CU , CPowerCableSizingOfFaultCurrentDlg::__CABLE_CONDUCTOR__);
	DDX_Radio(pDX , IDC_RADIO_PVC , CPowerCableSizingOfFaultCurrentDlg::__INSULATION_MATERIAL__);

	DDX_Text(pDX , IDC_EDIT_FAULT_CLEARING_TIME , CPowerCableSizingOfFaultCurrentDlg::__FAULT_CLEARING_TIME__);
	DDX_Text(pDX , IDC_EDIT_FAULT_CURRENT , CPowerCableSizingOfFaultCurrentDlg::__FAULT_CURRENT__);

	DDX_Control(pDX , IDC_EDIT_FAULT_CLEARING_TIME , m_wndFaultClearingTimeEdit);
	DDX_Control(pDX , IDC_EDIT_FAULT_CURRENT , m_wndFaultCurrentEdit);

	DDX_Control(pDX , IDC_BUTTON_CLEAR , m_wndClearButton);
	DDX_Control(pDX , IDOK , m_wndApplyButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);

	DDX_Text(pDX , IDC_EDIT_RESULT , CPowerCableSizingOfFaultCurrentDlg::__POWER_CABLE_SIZING__);
}


BEGIN_MESSAGE_MAP(CPowerCableSizingOfFaultCurrentDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CALC, &CPowerCableSizingOfFaultCurrentDlg::OnBnClickedButtonCalc)
	ON_BN_CLICKED(IDC_RADIO_CU, &CPowerCableSizingOfFaultCurrentDlg::OnBnClickedRadioCu)
	ON_BN_CLICKED(IDC_RADIO_AL, &CPowerCableSizingOfFaultCurrentDlg::OnBnClickedRadioAl)
	ON_BN_CLICKED(IDC_RADIO_PVC, &CPowerCableSizingOfFaultCurrentDlg::OnBnClickedRadioPvc)
	ON_BN_CLICKED(IDC_RADIO_XLPE, &CPowerCableSizingOfFaultCurrentDlg::OnBnClickedRadioXlpe)
	ON_EN_CHANGE(IDC_EDIT_FAULT_CLEARING_TIME, &CPowerCableSizingOfFaultCurrentDlg::OnEnChangeEditFaultClearingTime)
	ON_EN_CHANGE(IDC_EDIT_FAULT_CURRENT, &CPowerCableSizingOfFaultCurrentDlg::OnEnChangeEditFaultCurrent)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CPowerCableSizingOfFaultCurrentDlg::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDOK, &CPowerCableSizingOfFaultCurrentDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CPowerCableSizingOfFaultCurrentDlg message handlers

BOOL CPowerCableSizingOfFaultCurrentDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string regexp = docData.GetRegExp(_T("NUMBER"));
	if(!regexp.empty())
	{
		m_wndFaultClearingTimeEdit.SetRegEx(regexp.c_str());
		m_wndFaultCurrentEdit.SetRegEx(regexp.c_str());
	}
	
	static const CString rToolTip(_T("데이터 형식이 일치하지 않습니다."));
	const COLORREF WavyLineColor = docData.GetColorCode("FILTER_EDIT_WAVY_LINE_COLOR");
	const COLORREF BackFocusColor= docData.GetColorCode("FILTER_EDIT_BACK_FOCUS_COLOR");
	const COLORREF TextErrorColor= docData.GetColorCode("FILTER_EDIT_TEXT_ERROR_COLOR");
	const COLORREF BackErrorColor= docData.GetColorCode("FILTER_EDIT_BACK_ERROR_COLOR");

	m_wndFaultClearingTimeEdit.CreateToolTip(this , rToolTip);
	m_wndFaultClearingTimeEdit.SetWavyLineColour (WavyLineColor);
	m_wndFaultClearingTimeEdit.SetBackgroundColourFocus (BackFocusColor);
	m_wndFaultClearingTimeEdit.SetForegroundColourError (TextErrorColor);
	m_wndFaultClearingTimeEdit.SetBackgroundColourError (BackErrorColor);

	m_wndFaultCurrentEdit.CreateToolTip(this , rToolTip);
	m_wndFaultCurrentEdit.SetWavyLineColour (WavyLineColor);
	m_wndFaultCurrentEdit.SetBackgroundColourFocus (BackFocusColor);
	m_wndFaultCurrentEdit.SetForegroundColourError (TextErrorColor);
	m_wndFaultCurrentEdit.SetBackgroundColourError (BackErrorColor);

	HICON hOKIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_OK_18X18),
		IMAGE_ICON,
		18,18, 0 );
	m_wndApplyButton.SetIcon(hOKIcon);
		
	HICON hCancelIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_CANCEL_18X18),
		IMAGE_ICON,
		18,18, 0 );
	m_wndCancelButton.SetIcon(hCancelIcon);

	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	m_wndClearButton.SetTextHotColor(TextHotColor);
	m_wndApplyButton.SetTextHotColor(TextHotColor);
	m_wndCancelButton.SetTextHotColor(TextHotColor);

	LoadData();
	DisplayData();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPowerCableSizingOfFaultCurrentDlg::PreTranslateMessage(MSG* pMsg)
{
	//! 리턴키나 ESCAPE KEY로 윈도우가 닫기는 것을 방지.
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE) return TRUE;
		if(pMsg->wParam == VK_RETURN)
		{
			if(m_wndFaultClearingTimeEdit.GetSafeHwnd() == pMsg->hwnd)
			{
				m_wndFaultCurrentEdit.SetFocus();
			}
			

			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

/**
	@brief	

	@author	BHK
*/
void CPowerCableSizingOfFaultCurrentDlg::OnBnClickedButtonCalc()
{
	UpdateData(TRUE);

	CString rString;
	GetDlgItemText(IDC_EDIT_FAULT_CURRENT,rString);
	const double nFaultCurrent = atof(rString);
	GetDlgItemText(IDC_EDIT_FAULT_CLEARING_TIME,rString);
	const double nFaultClearingTime = atof(rString);

	double K = 0.;
	if((0 == CPowerCableSizingOfFaultCurrentDlg::__CABLE_CONDUCTOR__) && 
		(0 == CPowerCableSizingOfFaultCurrentDlg::__INSULATION_MATERIAL__))
	{
		K = 0.115;
	}
	else if((0 == CPowerCableSizingOfFaultCurrentDlg::__CABLE_CONDUCTOR__) && 
		(1 == CPowerCableSizingOfFaultCurrentDlg::__INSULATION_MATERIAL__))
	{
		K = 0.143;
	}
	else if((1 == CPowerCableSizingOfFaultCurrentDlg::__CABLE_CONDUCTOR__) && 
		(0 == CPowerCableSizingOfFaultCurrentDlg::__INSULATION_MATERIAL__))
	{
		K = 0.076;
	}
	else if((1 == CPowerCableSizingOfFaultCurrentDlg::__CABLE_CONDUCTOR__) && 
		(1 == CPowerCableSizingOfFaultCurrentDlg::__INSULATION_MATERIAL__))
	{
		K = 0.094;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	stringstream oss;
	oss << nFaultCurrent * sqrt(nFaultClearingTime) / K;
	CPowerCableSizingOfFaultCurrentDlg::__POWER_CABLE_SIZING__ = oss.str().c_str();

	UpdateData(FALSE);
}

void CPowerCableSizingOfFaultCurrentDlg::OnBnClickedRadioCu()
{
	OnBnClickedButtonCalc();
}

void CPowerCableSizingOfFaultCurrentDlg::OnBnClickedRadioAl()
{
	OnBnClickedButtonCalc();
}

void CPowerCableSizingOfFaultCurrentDlg::OnBnClickedRadioPvc()
{
	OnBnClickedButtonCalc();
}

void CPowerCableSizingOfFaultCurrentDlg::OnBnClickedRadioXlpe()
{
	OnBnClickedButtonCalc();
}

void CPowerCableSizingOfFaultCurrentDlg::OnEnChangeEditFaultClearingTime()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	OnBnClickedButtonCalc();
}

void CPowerCableSizingOfFaultCurrentDlg::OnEnChangeEditFaultCurrent()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	OnBnClickedButtonCalc();
}

/**
	@brief	

	@author	BHK
*/
void CPowerCableSizingOfFaultCurrentDlg::OnBnClickedButtonClear()
{
	CPowerCableSizingOfFaultCurrentDlg::__POWER_CABLE_SIZING__ = _T("");
	UpdateData(FALSE);
}

/**
	@brief	CABLE CREATION SETTING DATA 를 LOAD 한다.

	@author KHS	

	@return		
**/
int CPowerCableSizingOfFaultCurrentDlg::LoadData(void)
{
	CELoadDocData &docData = CELoadDocData::GetInstance();
	const string rMDBFilePath = docData.GetProjectMDBFilePath();

	CPowerCableCreationSettingTable* pPowerCableCreationSettingTable = static_cast<CPowerCableCreationSettingTable*>(docData.GetTableOf(CPowerCableCreationSettingTable::GetTableName()));

	return ERROR_SUCCESS;
}

/**
	@brief	LOAD 된 DATA를 화면에 출력한다.

	@author KHS	

	@return		
**/
int CPowerCableSizingOfFaultCurrentDlg::DisplayData(void)
{
	//! DISPLAY CABLE CREATION SETTING DATA 
	CELoadDocData &docData = CELoadDocData::GetInstance();
	CPowerCableCreationSettingTable* pPowerCableSettingTable = static_cast<CPowerCableCreationSettingTable*>(docData.GetTableOf(CPowerCableCreationSettingTable::GetTableName()));
	if(pPowerCableSettingTable)
	{
		int nRow = 1;
		for(vector<CCableCreationTable::Record>::iterator itr = pPowerCableSettingTable->m_RecordEntry.begin();itr != pPowerCableSettingTable->m_RecordEntry.end();++itr,++nRow)
		{
			if(1 == nRow)
			{
				m_wndFaultClearingTimeEdit.SetWindowText(itr->FieldValueMap["C_FAULT_CLEARING_TIME"].empty() ? _T("") : itr->FieldValueMap["C_FAULT_CLEARING_TIME"].c_str());
				m_wndFaultCurrentEdit.SetWindowText(itr->FieldValueMap["C_FAULT_CURRENT"].empty() ? _T("") : itr->FieldValueMap["C_FAULT_CURRENT"].c_str());

				if(!itr->FieldValueMap["C_CABLE_CONDUCTOR"].empty())
				{
					__CABLE_CONDUCTOR__ = atoi(itr->FieldValueMap["C_CABLE_CONDUCTOR"].c_str());
				}

				if(!itr->FieldValueMap["C_INSULATION_MATERIAL"].empty())
				{
					__INSULATION_MATERIAL__ = atoi(itr->FieldValueMap["C_INSULATION_MATERIAL"].c_str());
				}
			}
		}

		UpdateData(FALSE);

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	Data를 DB 에 저장한다.

	@author KHS	

	@return		
**/
int CPowerCableSizingOfFaultCurrentDlg::SaveData(void)
{
	UpdateData(TRUE);

	CELoadDocData& docData = CELoadDocData::GetInstance();
	CString rConductor, rInsulationMaterial, rFaultClearingTime, rFaultCurrent;
	CPowerCableCreationSettingTable* pPowerCableCreationSettingTable = static_cast<CPowerCableCreationSettingTable*>(docData.GetTableOf(CPowerCableCreationSettingTable::GetTableName()));
	if(pPowerCableCreationSettingTable)
	{
		int i = 1;
		for(vector<CCableCreationTable::Record>::iterator itr = pPowerCableCreationSettingTable->m_RecordEntry.begin();itr != pPowerCableCreationSettingTable->m_RecordEntry.end();++itr,++i)
		{
			if(i == 1)
			{
				rConductor.Format("%d", __CABLE_CONDUCTOR__);
				rInsulationMaterial.Format("%d", __INSULATION_MATERIAL__);

				m_wndFaultClearingTimeEdit.GetWindowText(rFaultClearingTime);
				m_wndFaultCurrentEdit.GetWindowText(rFaultCurrent);
			}

			itr->FieldValueMap["C_CABLE_CONDUCTOR"]		= rConductor.operator LPCSTR();
			itr->FieldValueMap["C_INSULATION_MATERIAL"]	= rInsulationMaterial.operator LPCSTR();
			itr->FieldValueMap["C_FAULT_CLEARING_TIME"]	= rFaultClearingTime.operator LPCSTR();
			itr->FieldValueMap["C_FAULT_CURRENT"]		= rFaultCurrent.operator LPCSTR();

			if(1 == i) break;
		}
		pPowerCableCreationSettingTable->Update();

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}
/**
	@brief	CLICK APPLY BUTTON

	@author KHS	

	@return		
**/
void CPowerCableSizingOfFaultCurrentDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	SaveData();

	OnOK();
}
