// PG1Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "PG1Dlg.h"
#include "pyfromc.h"
#include "PropertiesWnd.h"

// CPG1Dlg dialog

IMPLEMENT_DYNAMIC(CPG1Dlg, CDialog)

CPG1Dlg::CPG1Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPG1Dlg::IDD, pParent)
{

}

CPG1Dlg::~CPG1Dlg()
{
}

void CPG1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPG1Dlg, CDialog)
	ON_EN_CHANGE(IDC_EDIT_PG1_WL, &CPG1Dlg::OnEnChangeEditPg1Wl)
	ON_EN_CHANGE(IDC_EDIT_PG1_L, &CPG1Dlg::OnEnChangeEditPg1L)
	ON_EN_CHANGE(IDC_EDIT_PG1_COS, &CPG1Dlg::OnEnChangeEditPg1Cos)
END_MESSAGE_MAP()


// CPG1Dlg message handlers
/**
	@brief	

	@author BHK	

	@date 2009-05-08 오후 2:45:51	

	@param	

	@return		
*/
int CPG1Dlg::CalculatePG1(void)
{
	CString rString;
	GetDlgItemText(IDC_EDIT_PG1_WL,rString);
	const double nWL = atof(rString);
	GetDlgItemText(IDC_EDIT_PG1_L,rString);
	const double nL = atof(rString);
	GetDlgItemText(IDC_EDIT_PG1_COS,rString);
	const double nCOS = atof(rString);
	if(0.f != nCOS)
	{
		const string rPG1 = python.CallPyCalculatePG1(nWL , nL , nCOS);
		SetDlgItemText(ID_EDIT_PG1 , rPG1.c_str());
		GetParent()->GetParent()->SetDlgItemText(IDC_EDIT_PG1 , rPG1.c_str());
	}
	else
	{
		SetDlgItemText(ID_EDIT_PG1 , _T(""));
		GetParent()->GetParent()->SetDlgItemText(IDC_EDIT_PG1 , _T(""));
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-08 오후 5:16:30	

	@param	

	@return		
*/
void CPG1Dlg::OnEnChangeEditPg1Wl()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString rString;
	GetDlgItemText(IDC_EDIT_PG1_WL,rString);
	CalculatePG1();

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)(GetParent()->GetParent()->GetParent());
	if(pPropWnd) pPropWnd->GetSelectionSet().SetValue(_T("PG1") , _T("WL") , rString.operator LPCSTR());
}

/**
	@brief	

	@author BHK	

	@date 2009-05-08 오후 5:16:52	

	@param	

	@return		
*/
void CPG1Dlg::OnEnChangeEditPg1L()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString rString;
	GetDlgItemText(IDC_EDIT_PG1_L,rString);
	CalculatePG1();

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)(GetParent()->GetParent()->GetParent());
	if(pPropWnd) pPropWnd->GetSelectionSet().SetValue(_T("PG1") , _T("L") , rString.operator LPCSTR());
}

/**
	@brief	

	@author BHK	

	@date 2009-05-08 오후 5:17:09	

	@param	

	@return		
*/
void CPG1Dlg::OnEnChangeEditPg1Cos()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString rString;
	GetDlgItemText(IDC_EDIT_PG1_COS,rString);
	CalculatePG1();

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)(GetParent()->GetParent()->GetParent());
	if(pPropWnd) pPropWnd->GetSelectionSet().SetValue(_T("PG1") , _T("COS θG") , rString.operator LPCSTR());
}

/**
	@brief	

	@author BHK	

	@date 2009-05-09 오전 11:41:25	

	@param	

	@return		
**/
int CPG1Dlg::SetEventDataValues(CEventData*  pEventData)
{
	assert(pEventData && "pEventData is NULL");

	if(pEventData)
	{
		string rValue = pEventData->m_SelectionSet.GetValue( _T("PG1") , _T("WL") );
		SetDlgItemText(IDC_EDIT_PG1_WL , rValue.c_str());

		rValue = pEventData->m_SelectionSet.GetValue( _T("PG1") , _T("L") );
		SetDlgItemText(IDC_EDIT_PG1_L , rValue.c_str());

		rValue = pEventData->m_SelectionSet.GetValue( _T("PG1") , _T("COS θG") );
		SetDlgItemText(IDC_EDIT_PG1_COS , rValue.c_str());

		rValue = pEventData->m_SelectionSet.GetValue( _T("PG1") , _T("PG1") );
		SetDlgItemText(IDC_EDIT_PG1 , rValue.c_str());

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}
