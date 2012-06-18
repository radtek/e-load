// PG2Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "PG2Dlg.h"
#include "ELoadDocData.h"
#include "pyfromc.h"
#include "PropertiesWnd.h"

// CPG2Dlg dialog

IMPLEMENT_DYNAMIC(CPG2Dlg, CDialog)

CPG2Dlg::CPG2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPG2Dlg::IDD, pParent) , m_nRadio(0)
{

}

CPG2Dlg::~CPG2Dlg()
{
}

void CPG2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	DDX_Control(pDX , IDC_BUTTON_SAVE , m_wndSave);
}


BEGIN_MESSAGE_MAP(CPG2Dlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CPG2Dlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_RADIO1, &CPG2Dlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CPG2Dlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &CPG2Dlg::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO4, &CPG2Dlg::OnBnClickedRadio4)
	ON_BN_CLICKED(IDC_RADIO5, &CPG2Dlg::OnBnClickedRadio5)
	ON_BN_CLICKED(IDC_RADIO6, &CPG2Dlg::OnBnClickedRadio6)
	ON_BN_CLICKED(IDC_RADIO7, &CPG2Dlg::OnBnClickedRadio7)
	ON_BN_CLICKED(IDC_RADIO8, &CPG2Dlg::OnBnClickedRadio8)
	ON_EN_CHANGE(IDC_EDIT_MAX_STARTING_CAPACITY, &CPG2Dlg::OnEnChangeEditMaxStartingCapacity)
	ON_EN_CHANGE(IDC_EDIT_COEFF, &CPG2Dlg::OnEnChangeEditCoeff)
	ON_EN_CHANGE(IDC_EDIT_PLmax, &CPG2Dlg::OnEnChangeEditPlmax)
	ON_EN_CHANGE(IDC_EDIT_V, &CPG2Dlg::OnEnChangeEditV)
	ON_EN_CHANGE(IDC_EDIT_Xd1, &CPG2Dlg::OnEnChangeEditXd1)
	ON_EN_CHANGE(IDC_EDIT_Xd2, &CPG2Dlg::OnEnChangeEditXd2)
	ON_EN_CHANGE(IDC_EDIT_MOTOR_MAX_STARTING_CAPACITY, &CPG2Dlg::OnEnChangeEditMotorMaxStartingCapacity)
END_MESSAGE_MAP()

/**
	@brief	

	@author BHK	

	@date 2009-05-08 오후 8:12:59	

	@param	

	@return		
*/
int CPG2Dlg::CalculatePG2(void)
{
	CString rString;
	GetDlgItemText(IDC_EDIT_PLmax,rString);
	const double nPLmax = atof(rString);
	GetDlgItemText(IDC_EDIT_V,rString);
	const double nV = atof(rString);
	GetDlgItemText(IDC_EDIT_Xd1,rString);
	const double nXd1 = atof(rString);
	GetDlgItemText(IDC_EDIT_Xd2,rString);
	const double nXd2 = atof(rString);
	if(0.f != nV)
	{
		const string rPG2 = python.CallPyCalculatePG2(nPLmax , nV , nXd1 , nXd2);
		SetDlgItemText(IDC_EDIT_PG2 , rPG2.c_str());
		GetParent()->GetParent()->SetDlgItemText(IDC_EDIT_PG2 , rPG2.c_str());
	}
	else
	{
		SetDlgItemText(IDC_EDIT_PG2 , _T(""));
		GetParent()->GetParent()->SetDlgItemText(IDC_EDIT_PG2 , _T(""));
	}

	return ERROR_SUCCESS;
}

// CPG2Dlg message handlers

/**
	@brief	

	@author BHK	

	@date 2009-05-08 오후 6:53:58	

	@param	

	@return		
*/
BOOL CPG2Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CELoadDocData& docData = CELoadDocData::GetInstance();
	TABLES::CELOADTable* pTable = docData.GetTableOf(CProjectSettingTable::GetTableName());
	if(pTable && !pTable->m_RecordEntry.empty())
	{
		string rValue;
		rValue = pTable->m_RecordEntry[0].FieldValueMap[_T("C_DIRECT_ON_LINE")];
		if(rValue.empty()) rValue = _T("1.0");
		SetDlgItemText(IDC_EDIT_DIRECT_ON_LINE , rValue.c_str());

		rValue = pTable->m_RecordEntry[0].FieldValueMap[_T("C_AUTO_TRANSFORMER_50_TAP")];
		if(rValue.empty()) rValue = _T("0.25");
		SetDlgItemText(IDC_EDIT_AUTO_TRANSFORMER_50_TAP , rValue.c_str());

		rValue = pTable->m_RecordEntry[0].FieldValueMap[_T("C_AUTO_TRANSFORMER_65_TAP")];
		if(rValue.empty()) rValue = _T("0.42");
		SetDlgItemText(IDC_EDIT_AUTO_TRANSFORMER_65_TAP , rValue.c_str());

		rValue = pTable->m_RecordEntry[0].FieldValueMap[_T("C_AUTO_TRANSFORMER_80_TAP")];
		if(rValue.empty()) rValue = _T("0.64");
		SetDlgItemText(IDC_EDIT_AUTO_TRANSFORMER_80_TAP , rValue.c_str());

		rValue = pTable->m_RecordEntry[0].FieldValueMap[_T("C_REACTOR_50_TAP")];
		if(rValue.empty()) rValue = _T("0.5");
		SetDlgItemText(IDC_EDIT_REACTOR_50_TAP , rValue.c_str());

		rValue = pTable->m_RecordEntry[0].FieldValueMap[_T("C_REACTOR_65_TAP")];
		if(rValue.empty()) rValue = _T("0.65");
		SetDlgItemText(IDC_EDIT_REACTOR_65_TAP , rValue.c_str());

		rValue = pTable->m_RecordEntry[0].FieldValueMap[_T("C_REACTOR_80_TAP")];
		if(rValue.empty()) rValue = _T("0.8");
		SetDlgItemText(IDC_EDIT_REACTOR_80_TAP , rValue.c_str());

		rValue = pTable->m_RecordEntry[0].FieldValueMap[_T("C_STAR_DELTA")];
		if(rValue.empty()) rValue = _T("0.33");
		SetDlgItemText(IDC_EDIT_STAR_DELTA , rValue.c_str());
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	Project setting 테이블을 저장합니다.

	@author BHK	

	@date 2009-05-08 오후 7:02:45	

	@param	

	@return		
*/
void CPG2Dlg::OnBnClickedButtonSave()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	TABLES::CELOADTable* pTable = docData.GetTableOf(CProjectSettingTable::GetTableName());
	if(pTable)
	{
		CString rValue;
		GetDlgItemText(IDC_EDIT_DIRECT_ON_LINE , rValue);
		pTable->m_RecordEntry[0].FieldValueMap[_T("C_DIRECT_ON_LINE")] = rValue.operator LPCSTR();

		GetDlgItemText(IDC_EDIT_AUTO_TRANSFORMER_50_TAP , rValue);
		pTable->m_RecordEntry[0].FieldValueMap[_T("C_AUTO_TRANSFORMER_50_TAP")] = rValue.operator LPCSTR();

		GetDlgItemText(IDC_EDIT_AUTO_TRANSFORMER_65_TAP , rValue);
		pTable->m_RecordEntry[0].FieldValueMap[_T("C_AUTO_TRANSFORMER_65_TAP")] = rValue.operator LPCSTR();

		GetDlgItemText(IDC_EDIT_AUTO_TRANSFORMER_80_TAP , rValue);
		pTable->m_RecordEntry[0].FieldValueMap[_T("C_AUTO_TRANSFORMER_80_TAP")] = rValue.operator LPCSTR();

		GetDlgItemText(IDC_EDIT_REACTOR_50_TAP , rValue);
		pTable->m_RecordEntry[0].FieldValueMap[_T("C_REACTOR_50_TAP")] = rValue.operator LPCSTR();

		GetDlgItemText(IDC_EDIT_REACTOR_65_TAP , rValue);
		pTable->m_RecordEntry[0].FieldValueMap[_T("C_REACTOR_65_TAP")] = rValue.operator LPCSTR();

		GetDlgItemText(IDC_EDIT_REACTOR_80_TAP , rValue);
		pTable->m_RecordEntry[0].FieldValueMap[_T("C_REACTOR_80_TAP")] = rValue.operator LPCSTR();

		GetDlgItemText(IDC_EDIT_STAR_DELTA , rValue);
		pTable->m_RecordEntry[0].FieldValueMap[_T("C_STAR_DELTA")] = rValue.operator LPCSTR();

		pTable->Save();
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-05-08 오후 7:49:26	

	@param	

	@return		
*/
void CPG2Dlg::OnBnClickedRadio1()
{
	CString rValue;
	GetDlgItemText(IDC_EDIT_DIRECT_ON_LINE , rValue);
	SetDlgItemText(IDC_EDIT_COEFF , rValue);
}

void CPG2Dlg::OnBnClickedRadio2()
{
	CString rValue;
	GetDlgItemText(IDC_EDIT_AUTO_TRANSFORMER_50_TAP , rValue);
	SetDlgItemText(IDC_EDIT_COEFF , rValue);
}

void CPG2Dlg::OnBnClickedRadio3()
{
	CString rValue;
	GetDlgItemText(IDC_EDIT_AUTO_TRANSFORMER_65_TAP , rValue);
	SetDlgItemText(IDC_EDIT_COEFF , rValue);
}

void CPG2Dlg::OnBnClickedRadio4()
{
	CString rValue;
	GetDlgItemText(IDC_EDIT_AUTO_TRANSFORMER_80_TAP , rValue);
	SetDlgItemText(IDC_EDIT_COEFF , rValue);
}

void CPG2Dlg::OnBnClickedRadio5()
{
	CString rValue;
	GetDlgItemText(IDC_EDIT_REACTOR_50_TAP , rValue);
	SetDlgItemText(IDC_EDIT_COEFF , rValue);
}

void CPG2Dlg::OnBnClickedRadio6()
{
	CString rValue;
	GetDlgItemText(IDC_EDIT_REACTOR_65_TAP , rValue);
	SetDlgItemText(IDC_EDIT_COEFF , rValue);
}

void CPG2Dlg::OnBnClickedRadio7()
{
	CString rValue;
	GetDlgItemText(IDC_EDIT_REACTOR_80_TAP , rValue);
	SetDlgItemText(IDC_EDIT_COEFF , rValue);
}

void CPG2Dlg::OnBnClickedRadio8()
{
	CString rValue;
	GetDlgItemText(IDC_EDIT_STAR_DELTA , rValue);
	SetDlgItemText(IDC_EDIT_COEFF , rValue);
}

void CPG2Dlg::OnEnChangeEditMaxStartingCapacity()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString rLeftValue , rRightValue;
	GetDlgItemText(IDC_EDIT_MAX_STARTING_CAPACITY , rLeftValue);
	GetDlgItemText(IDC_EDIT_COEFF , rRightValue);
	
	const double nValue = SAFE_ROUND(atof(rLeftValue) * atof(rRightValue) , 2);
	ostringstream oss;
	oss << nValue;
	SetDlgItemText(IDC_EDIT_PLmax , oss.str().c_str());
}

/**
	@brief	

	@author BHK	

	@date 2009-05-08 오후 8:07:35	

	@param	

	@return		
*/
void CPG2Dlg::OnEnChangeEditCoeff()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	//! Project setting 테이블을 저장합니다.
	OnBnClickedButtonSave();

	CString rLeftValue , rRightValue , rValue;
	GetDlgItemText(IDC_EDIT_MAX_STARTING_CAPACITY , rLeftValue);
	GetDlgItemText(IDC_EDIT_COEFF , rRightValue);
	
	const double nValue = SAFE_ROUND(atof(rLeftValue) * atof(rRightValue) , 2);
	ostringstream oss;
	oss << nValue;
	SetDlgItemText(IDC_EDIT_PLmax , oss.str().c_str());

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)(GetParent()->GetParent()->GetParent());
	if(pPropWnd) pPropWnd->GetSelectionSet().SetValue(_T("PG2") , _T("coefficient") , rRightValue.operator LPCSTR());
}

void CPG2Dlg::OnEnChangeEditPlmax()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString rString;
	GetDlgItemText(IDC_EDIT_PLmax , rString);
	CalculatePG2();

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)(GetParent()->GetParent()->GetParent());
	if(pPropWnd) pPropWnd->GetSelectionSet().SetValue(_T("PG2") , _T("PLmax") , rString.operator LPCSTR());
}

void CPG2Dlg::OnEnChangeEditV()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	CString rString;
	GetDlgItemText(IDC_EDIT_V , rString);
	CalculatePG2();

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)(GetParent()->GetParent()->GetParent());
	if(pPropWnd) pPropWnd->GetSelectionSet().SetValue(_T("PG2") , _T("V") , rString.operator LPCSTR());
}

void CPG2Dlg::OnEnChangeEditXd1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString rString;
	GetDlgItemText(IDC_EDIT_Xd1 , rString);
	CalculatePG2();

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)(GetParent()->GetParent()->GetParent());
	if(pPropWnd) pPropWnd->GetSelectionSet().SetValue(_T("PG2") , _T("Xd1") , rString.operator LPCSTR());
}

void CPG2Dlg::OnEnChangeEditXd2()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	CString rString;
	GetDlgItemText(IDC_EDIT_Xd2, rString);
	CalculatePG2();

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)(GetParent()->GetParent()->GetParent());
	if(pPropWnd) pPropWnd->GetSelectionSet().SetValue(_T("PG2") , _T("Xd2") , rString.operator LPCSTR());
}

/**
	@brief	

	@author BHK	

	@date 2009-05-09 오전 11:48:59	

	@param	

	@return		
**/
int CPG2Dlg::SetEventDataValues(CEventData*  pEventData)
{
	assert(pEventData && "pEventData is NULL");
	static const UINT __IDS__[] = 
	{
		IDC_EDIT_DIRECT_ON_LINE , IDC_EDIT_AUTO_TRANSFORMER_50_TAP , IDC_EDIT_AUTO_TRANSFORMER_65_TAP , IDC_EDIT_AUTO_TRANSFORMER_80_TAP ,
		IDC_EDIT_REACTOR_50_TAP , IDC_EDIT_REACTOR_65_TAP , IDC_EDIT_REACTOR_80_TAP , IDC_EDIT_STAR_DELTA
	};
	static const UINT __RADIOS__[] = 
	{
		IDC_RADIO1 , IDC_RADIO2 , IDC_RADIO3 , IDC_RADIO4, IDC_RADIO5 , IDC_RADIO6 , IDC_RADIO7, IDC_RADIO8
	};

	if(pEventData)
	{
		string rValue = pEventData->m_SelectionSet.GetValue( _T("PG2") , _T("coefficient") );
		SetDlgItemText(IDC_EDIT_COEFF , rValue.c_str());

		CString rString;
		for(int i = 0;i < SIZE_OF_ARRAY(__IDS__);++i)
		{
			GetDlgItemText(__IDS__[i] , rString);
			if(rString == rValue.c_str())
			{
				CButton* pButton = (CButton*)GetDlgItem(__RADIOS__[i]);
				if(pButton) pButton->SetCheck(TRUE);
				m_nRadio = i;
				break;
			}
		}

		rValue = pEventData->m_SelectionSet.GetValue( _T("PG2") , _T("V") );
		SetDlgItemText(IDC_EDIT_V , rValue.c_str());

		rValue = pEventData->m_SelectionSet.GetValue( _T("PG2") , _T("Xd1") );
		SetDlgItemText(IDC_EDIT_Xd1 , rValue.c_str());

		rValue = pEventData->m_SelectionSet.GetValue( _T("PG2") , _T("Xd2") );
		SetDlgItemText(IDC_EDIT_Xd2 , rValue.c_str());

		rValue = pEventData->m_SelectionSet.GetValue( _T("PG2") , _T("PG2") );
		SetDlgItemText(IDC_EDIT_PG2 , rValue.c_str());

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author BHK	

	@date 2009-06-22 오후 7:13:43	

	@param	

	@return		
*/
int CPG2Dlg::DisplayPLmaxEquation(const string& rLoadID)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CLoadItem* pLoadItem = docData.FindLoadItemByName(rLoadID);
	if(pLoadItem)
	{
		const string rLoadID = pLoadItem->prop()->GetValue(_T("General") , _T("ITEM ID"));
		SetDlgItemText(IDC_EDIT_MOTOR_ID , rLoadID.c_str());
		
		double nRatingCapacity = 0.f;
		pLoadItem->CalcRatingCapacity(nRatingCapacity);
		ostringstream oss;
		oss << nRatingCapacity;
		SetDlgItemText(IDC_EDIT_MOTOR_RATING_CAPACITY , oss.str().c_str());
		
		//! display LRC value divied by 100.
		const string rLRC = pLoadItem->prop()->GetValue(_T("Characteristic") , _T("LRC"));
		const double nLRC = atof(rLRC.c_str()) * 0.01;
		oss.str("");
		oss << nLRC;
		SetDlgItemText(IDC_EDIT_MOTOR_LRC , oss.str().c_str());

		const string rPF = pLoadItem->prop()->GetValue(_T("Load Summary") , _T("PF"));
		SetDlgItemText(IDC_EDIT_MOTOR_PF , rPF.c_str());

		const double nPLmax = SAFE_ROUND((nRatingCapacity * nLRC) / atof(rPF.c_str()) , 2);
		oss.str("");
		oss << nPLmax;
		SetDlgItemText(IDC_EDIT_MOTOR_MAX_STARTING_CAPACITY , oss.str().c_str());
	}

	return 0;
}

/**
	@brief	

	@author BHK	

	@date 2009-06-23 오후 9:59:54	

	@param	

	@return		
**/
void CPG2Dlg::OnEnChangeEditMotorMaxStartingCapacity()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString rString;
	GetDlgItemText(IDC_EDIT_MOTOR_MAX_STARTING_CAPACITY , rString);
	SetDlgItemText(IDC_EDIT_MAX_STARTING_CAPACITY , rString);
}
