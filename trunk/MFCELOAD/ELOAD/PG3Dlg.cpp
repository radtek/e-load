// PG3Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "PG3Dlg.h"
#include "pyfromc.h"
#include "PropertiesWnd.h"

// CPG3Dlg dialog

IMPLEMENT_DYNAMIC(CPG3Dlg, CDialog)

CPG3Dlg::CPG3Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPG3Dlg::IDD, pParent)
{

}

CPG3Dlg::~CPG3Dlg()
{
}

void CPG3Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPG3Dlg, CDialog)
	ON_EN_CHANGE(IDC_EDIT_COSQL, &CPG3Dlg::OnEnChangeEditCosql)
	ON_EN_CHANGE(IDC_EDIT_K, &CPG3Dlg::OnEnChangeEditK)
	ON_EN_CHANGE(IDC_EDIT_COSQG, &CPG3Dlg::OnEnChangeEditCosqg)
	ON_EN_CHANGE(IDC_EDIT_W0, &CPG3Dlg::OnEnChangeEditW0)
	ON_EN_CHANGE(IDC_EDIT_QLMAX, &CPG3Dlg::OnEnChangeEditQlmax)
END_MESSAGE_MAP()

/**
	@brief	

	@author BHK	

	@date 2009-05-08 오후 8:32:59	

	@param	

	@return		
*/
int CPG3Dlg::CalculatePG3(void)
{
	CString rString;
	GetDlgItemText(IDC_EDIT_W0,rString);
	const double nW0 = atof(rString);
	GetDlgItemText(IDC_EDIT_QLMAX,rString);
	const double nQLMAX = atof(rString);
	GetDlgItemText(IDC_EDIT_COSQL,rString);
	const double nCOSQL = atof(rString);
	GetDlgItemText(IDC_EDIT_K,rString);
	const double nK = atof(rString);
	GetDlgItemText(IDC_EDIT_COSQG,rString);
	const double nCOSQG = atof(rString);
	if((0.f != nK) && (0.f != nCOSQG))
	{
		const string rPG3 = python.CallPyCalculatePG3(nW0 , nQLMAX , nCOSQL , nK , nCOSQG);
		SetDlgItemText(IDC_EDIT_PG3 , rPG3.c_str());
		GetParent()->GetParent()->SetDlgItemText(IDC_EDIT_PG3 , rPG3.c_str());
	}
	else
	{
		SetDlgItemText(IDC_EDIT_PG3 , _T(""));
		GetParent()->GetParent()->SetDlgItemText(IDC_EDIT_PG3 , _T(""));
	}

	return ERROR_SUCCESS;
}
// CPG3Dlg message handlers

void CPG3Dlg::OnEnChangeEditCosql()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString rString;
	GetDlgItemText(IDC_EDIT_COSQL,rString);
	CalculatePG3();

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)(GetParent()->GetParent()->GetParent());
	if(pPropWnd) pPropWnd->GetSelectionSet().SetValue(_T("PG3") , _T("CosQL") , rString.operator LPCSTR());
}

void CPG3Dlg::OnEnChangeEditK()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString rString;
	GetDlgItemText(IDC_EDIT_K,rString);
	CalculatePG3();

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)(GetParent()->GetParent()->GetParent());
	if(pPropWnd) pPropWnd->GetSelectionSet().SetValue(_T("PG3") , _T("K") , rString.operator LPCSTR());
}

void CPG3Dlg::OnEnChangeEditCosqg()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString rString;
	GetDlgItemText(IDC_EDIT_COSQG,rString);
	CalculatePG3();
	
	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)(GetParent()->GetParent()->GetParent());
	if(pPropWnd) pPropWnd->GetSelectionSet().SetValue(_T("PG3") , _T("COS θG") , rString.operator LPCSTR());
}

/**
	@brief	

	@author BHK	

	@date 2009-05-09 오전 11:49:42	

	@param	

	@return		
**/
int CPG3Dlg::SetEventDataValues(CEventData*  pEventData)
{
	assert(pEventData && "pEventData is NULL");

	if(pEventData)
	{
		string rValue = pEventData->m_SelectionSet.GetValue( _T("PG3") , _T("Wo") );
		SetDlgItemText(IDC_EDIT_W0 , rValue.c_str());

		rValue = pEventData->m_SelectionSet.GetValue( _T("PG3") , _T("Qmax") );
		SetDlgItemText(IDC_EDIT_QLMAX , rValue.c_str());

		rValue = pEventData->m_SelectionSet.GetValue( _T("PG3") , _T("CosQL") );
		SetDlgItemText(IDC_EDIT_COSQL , rValue.c_str());

		rValue = pEventData->m_SelectionSet.GetValue( _T("PG3") , _T("K") );
		SetDlgItemText(IDC_EDIT_K , rValue.c_str());

		rValue = pEventData->m_SelectionSet.GetValue( _T("PG3") , _T("COS θG") );
		SetDlgItemText(IDC_EDIT_COSQG , rValue.c_str());

		rValue = pEventData->m_SelectionSet.GetValue( _T("PG3") , _T("PG3") );
		SetDlgItemText(IDC_EDIT_PG3 , rValue.c_str());

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-09 오전 11:54:02	

	@param	

	@return		
**/
void CPG3Dlg::OnEnChangeEditW0()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString rString;
	GetDlgItemText(IDC_EDIT_W0,rString);

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)(GetParent()->GetParent()->GetParent());
	if(pPropWnd) pPropWnd->GetSelectionSet().SetValue(_T("PG3") , _T("Wo") , rString.operator LPCSTR());
}

/**
	@brief	

	@author BHK	

	@date 2009-06-22 오후 3:09:03	

	@param	

	@return		
*/
void CPG3Dlg::OnEnChangeEditQlmax()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString rString;
	GetDlgItemText(IDC_EDIT_QLMAX,rString);
	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)(GetParent()->GetParent()->GetParent());
	if(pPropWnd) pPropWnd->GetSelectionSet().SetValue( _T("PG3") , _T("Qmax") ,rString.operator LPCSTR());
}
