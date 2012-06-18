// CapacitorPropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "ELoadDocData.h"
#include "PropertiesWnd.h"
#include "CapacitorPropDlg.h"
#include <math.h>
#include <sstream>
#include "CapacitorItem.h"

using namespace std;

int CCapacitorPropDlg::IDEAL_WIDTH = 350;
// CCapacitorPropDlg dialog

IMPLEMENT_DYNAMIC(CCapacitorPropDlg, CDialog)

CCapacitorPropDlg::CCapacitorPropDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCapacitorPropDlg::IDD, pParent)
{

}

CCapacitorPropDlg::~CCapacitorPropDlg()
{
}

void CCapacitorPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CAPACITOR_ID, m_wndCapacitorIdEdit);
	DDX_Control(pDX, IDC_COMBO_FROM_BUS_ID, m_wndFromBusIdCombo);
	DDX_Control(pDX, IDC_EDIT_FROM_BUS_VOLT, m_wndFromBusVoltEdit);
	DDX_Control(pDX, IDC_EDIT_FROM_LOAD_VOLT, m_wndFromLoadVoltEdit);
	DDX_Control(pDX, IDC_COMBO_FROM_CABLE_VOLT, m_wndFromCableVoltCombo);
	DDX_Control(pDX, IDC_COMBO_FROM_CABLE_TYPE, m_wndFromCableTypeCombo);
	DDX_Control(pDX, IDC_COMBO_FROM_CORE, m_wndFromCableCoreCombo);
	DDX_Control(pDX, IDC_EDIT_FROM_SPEC, m_wndFromCableSpecEdit);
	DDX_Control(pDX, IDC_EDIT_TARGET_POWER_FACTOR, m_wndTargetPowerFactorEdit);
	DDX_Control(pDX, IDC_EDIT_CALCULATED_CAPACITY, m_wndCalculatedCapacityEdit);
	DDX_Control(pDX, IDC_EDIT_SELECTED_CAPACITY, m_wndSelectedCapacityEdit);
	DDX_Control(pDX, IDC_EDIT_FLC, m_wndFlcEdit);
	DDX_Control(pDX, IDC_EDIT_IMPROVED_POWER_FACTOR, m_wndImprovedPowerFactorEdit);

	DDX_Control(pDX , IDC_EDIT_TOTAL_KW , m_wndTotalKWEdit);
	DDX_Control(pDX , IDC_EDIT_TOTAL_KVA , m_wndTotalKVAEdit);
}


BEGIN_MESSAGE_MAP(CCapacitorPropDlg, CDialog)
	ON_REGISTERED_MESSAGE(CEventData::SELECT_ITEM , OnItemSelected)
	ON_CBN_SELCHANGE(IDC_COMBO_FROM_CABLE_VOLT, &CCapacitorPropDlg::OnCbnSelchangeComboFromCableVolt)
	ON_CBN_SELCHANGE(IDC_COMBO_FROM_CABLE_TYPE, &CCapacitorPropDlg::OnCbnSelchangeComboFromCableType)
	ON_CBN_SELCHANGE(IDC_COMBO_FROM_BUS_ID, &CCapacitorPropDlg::OnCbnSelchangeComboFromBusId)
	ON_CBN_SELCHANGE(IDC_COMBO_FROM_CORE, &CCapacitorPropDlg::OnCbnSelchangeComboFromCore)
	ON_EN_CHANGE(IDC_EDIT_TARGET_POWER_FACTOR, &CCapacitorPropDlg::OnEnChangeEditTargetPowerFactor)
	ON_EN_CHANGE(IDC_EDIT_SELECTED_CAPACITY, &CCapacitorPropDlg::OnEnChangeEditSelectedCapacity)
	ON_EN_CHANGE(IDC_EDIT_FROM_SPEC, &CCapacitorPropDlg::OnEnChangeEditFromSpec)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CONNECTED_LIST, &CCapacitorPropDlg::OnNMClickListConnectedList)
	ON_EN_CHANGE(IDC_EDIT_CAPACITOR_PF, &CCapacitorPropDlg::OnEnChangeEditCapacitorPf)
	ON_EN_CHANGE(IDC_EDIT_CAPACITOR_LENGTH, &CCapacitorPropDlg::OnEnChangeEditCapacitorLength)
	ON_EN_CHANGE(IDC_EDIT_TOTAL_KW, &CCapacitorPropDlg::OnEnChangeEditTotalKw)
	ON_EN_CHANGE(IDC_EDIT_TOTAL_KVA, &CCapacitorPropDlg::OnEnChangeEditTotalKva)
END_MESSAGE_MAP()


// CCapacitorPropDlg message handlers

BOOL CCapacitorPropDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	///m_wndConnectedBusListCtrl.SetExtendedStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);
	///m_wndConnectedBusListCtrl.InsertColumn(0 , _T("Bus ID") , LVCFMT_LEFT , 200);
	///m_wndConnectedBusListCtrl.InsertColumn(1 , _T("Power Factor") , LVCFMT_LEFT , 200);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	

	@author BHK	

	@date 2009-09-02 오전 10:22:53	

	@param	

	@return		
*/
BOOL CCapacitorPropDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE) return TRUE;
		if(pMsg->wParam == VK_RETURN) return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

/**
	@brief	

	@author KHS	

	@date 2009-05-08 오전 9:26:26	

	@param	

	@return		
**/
LRESULT CCapacitorPropDlg::OnItemSelected(WPARAM wParam, LPARAM lParam)
{
	CEventData* pEventData = (CEventData*)lParam;
	if(pEventData) SetEventDataValues(pEventData);
		
	return 0;
}

/**
	@brief	

	@author KHS	

	@date 2009-05-08 오전 9:26:21	

	@param	

	@return		
**/
int CCapacitorPropDlg::SetEventDataValues(CEventData* pEventData)
{
	assert(pEventData && "pEventData is NULL");

	if(pEventData)
	{
		BeginWaitCursor();

		const string rCapacitorId = pEventData->m_SelectionSet.GetValue( _T("General") , _T("CAPACITOR ID") );
		m_wndCapacitorIdEdit.SetWindowText(rCapacitorId.c_str());
		const string rCapacitorPF = pEventData->m_SelectionSet.GetValue( _T("General") , _T("PF") );
		SetDlgItemText(IDC_EDIT_CAPACITOR_PF , rCapacitorPF.c_str());
		const string rCapacitorLength = pEventData->m_SelectionSet.GetValue( _T("General") , _T("Cable Length"));
		SetDlgItemText(IDC_EDIT_CAPACITOR_LENGTH , rCapacitorLength.c_str());

		vector<CBusItem*> BusItemEntry;
		CELoadDocData& docData = CELoadDocData::GetInstance();
		docData.GetAllBusItems(BusItemEntry);
		
		m_wndFromBusIdCombo.ResetContent();
		string rBusId = pEventData->m_SelectionSet.GetValue( _T("From") , _T("Bus Id"));

		for(vector<CBusItem*>::iterator itr = BusItemEntry.begin();itr != BusItemEntry.end();++itr)
		{
			m_wndFromBusIdCombo.AddString((*itr)->GetName().c_str());
		}

		int nIndex = m_wndFromBusIdCombo.FindString(-1, rBusId.c_str());
		if(-1 != nIndex)
		{
			m_wndFromBusIdCombo.SetCurSel(nIndex);
			DisplayLoadSummaryResult();

			string rTargetPowerFactor = pEventData->m_SelectionSet.GetValue( _T("Capacity") , _T("Target Power Factor") );
			if(!rTargetPowerFactor.empty())
				m_wndTargetPowerFactorEdit.SetWindowText(rTargetPowerFactor.c_str());

			string rSelectedCapacity = pEventData->m_SelectionSet.GetValue( _T("Capacity") , _T("Selected Capacity") );
			if(!rSelectedCapacity.empty())
				m_wndSelectedCapacityEdit.SetWindowText(rSelectedCapacity.c_str());

		}

		string rBusVolt = pEventData->m_SelectionSet.GetValue( _T("From") , _T("Bus Volt"));
		m_wndFromBusVoltEdit.SetWindowText(rBusVolt.c_str());

		string rLoadVolt = pEventData->m_SelectionSet.GetValue( _T("From") , _T("Load Volt"));
		m_wndFromLoadVoltEdit.SetWindowText(rLoadVolt.c_str());

		m_wndFromCableVoltCombo.ResetContent();
		m_wndFromCableTypeCombo.ResetContent();
		m_wndFromCableCoreCombo.ResetContent();

		TABLES::CELOADTable* pTable = docData.GetTableOf(CProjectSettingTable::GetTableName());
		const CString rDataName = pTable->m_RecordEntry[0].FieldValueMap[_T("C_CUR_CABLE_SIZING_SETTING_TABLE_NAME")].c_str();

		CCableSizeSettingTable& CableSizeSettingTable = CCableSizeSettingTable::GetInstance();
		CableSizeSettingTable.Load();
		if(!CableSizeSettingTable.m_RecordMap[rDataName].m_rCableDataTable.empty())
		{
			docData.m_ProjectCableDataTable.Load(CableSizeSettingTable.m_RecordMap[rDataName].m_rCableDataTable);

			vector<string> CableTypeList;
			docData.m_ProjectCableDataTable.GetCableTypeKeyList(CableTypeList);
			for(vector<string>::iterator itr = CableTypeList.begin(); itr != CableTypeList.end();++itr)
			{	
				CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(*itr);
				if(pCableDataByType)
				{
					vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
					if(pRecordList)
					{
		
						for(vector<CCableDataByType::Record*>::iterator jtr = pRecordList->begin();jtr != pRecordList->end();++jtr)
						{
							const CString rCableVolt = (*jtr)->m_FieldMap[_T("C_VOLT")].c_str();
							const int nFound = m_wndFromCableVoltCombo.FindString(0 , rCableVolt);
							if(-1 == nFound)
							{
								m_wndFromCableVoltCombo.AddString(rCableVolt);
							}
						}
					}
				}
			}

			const string rCableVolt = pEventData->m_SelectionSet.GetValue(_T("From") , _T("Cable Volt"));
			nIndex = m_wndFromCableVoltCombo.FindString(-1 , rCableVolt.c_str());
			if(-1 != nIndex)
			{
				m_wndFromCableVoltCombo.SetCurSel(nIndex);
				OnCbnSelchangeComboFromCableVolt();
			}
		}
		else
		{
			///AfxMessageBox(_T("CableData 테이블이 설정되지 않았습니다."));
			::OutputDebugString(_T("CableData 테이블이 설정되지 않았습니다."));
		}

		string rCableSpec = pEventData->m_SelectionSet.GetValue( _T("From") , _T("Cable Spec"));
		m_wndFromCableSpecEdit.SetWindowText(rCableSpec.c_str());

		m_wndTargetPowerFactorEdit.SetWindowText(pEventData->m_SelectionSet.GetValue(_T("Capacity") , _T("Target Power Factor")).c_str());
		m_wndSelectedCapacityEdit.SetWindowText(pEventData->m_SelectionSet.GetValue(_T("Capacity") , _T("Selected Capacity")).c_str());

		EndWaitCursor();

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	CABLE VOLT COMBO SELECT CHANGE

	@author KHS	

	@date 2009-05-08 오전 10:35:44	

	@param	

	@return		
**/
void CCapacitorPropDlg::OnCbnSelchangeComboFromCableVolt()
{
	int nCurSel = m_wndFromCableVoltCombo.GetCurSel();
	if(-1 == nCurSel) return;
	CString rSelCableVolt;
	m_wndFromCableVoltCombo.GetLBText(nCurSel , rSelCableVolt);
	
	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	pPropWnd->GetSelectionSet().SetValue(_T("From") , _T("Cable Volt") , rSelCableVolt.operator LPCSTR());

	//! Cable Type ComboBox를 채운다.
	m_wndFromCableTypeCombo.ResetContent();

	CELoadDocData& docData = CELoadDocData::GetInstance();
	vector<string> CableTypeList;
	docData.m_ProjectCableDataTable.GetCableTypeKeyList(CableTypeList);
	for(vector<string>::iterator itr = CableTypeList.begin(); itr != CableTypeList.end();++itr)
	{	
		CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(*itr);
		if(pCableDataByType)
		{
			vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
			if(pRecordList)
			{
				for(vector<CCableDataByType::Record*>::iterator jtr = pRecordList->begin();jtr != pRecordList->end();++jtr)
				{
					const CString rCableVolt = (*jtr)->m_FieldMap[_T("C_VOLT")].c_str();
					if(rCableVolt == rSelCableVolt)
					{
						const CString rCableType = (*jtr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str();
						const int nFound = m_wndFromCableTypeCombo.FindString(-1 , rCableType);
						if(-1 == nFound) m_wndFromCableTypeCombo.AddString(rCableType);
					}
				}
			}
		}
	}

	const string rCableType = pPropWnd->GetSelectionSet().GetValue(_T("From") , _T("Cable Type"));
	int nFound = m_wndFromCableTypeCombo.FindString(-1 , rCableType.c_str());
	if(-1 != nFound) 
		m_wndFromCableTypeCombo.SetCurSel(nFound);
	else
	{
		pPropWnd->GetSelectionSet().SetValue(_T("From") , _T("Cable Type") , _T(""));
		m_wndFromCableTypeCombo.SetWindowText(_T(""));
	}

	OnCbnSelchangeComboFromCableType();
}

/**
	@brief	CABLE TYPE COMBO SELECT CHANGE

	@author KHS	

	@date 2009-05-08 오전 11:13:06	

	@param	

	@return		
**/
void CCapacitorPropDlg::OnCbnSelchangeComboFromCableType()
{
	const int nCableVoltCurSel = m_wndFromCableVoltCombo.GetCurSel();
	const int nCableTypeCurSel = m_wndFromCableTypeCombo.GetCurSel();

	if((-1 == nCableVoltCurSel) || (-1 == nCableTypeCurSel)) return;

	CString rSelCableVolt , rSelCableType;
	m_wndFromCableVoltCombo.GetLBText(nCableVoltCurSel , rSelCableVolt);
	m_wndFromCableTypeCombo.GetLBText(nCableTypeCurSel , rSelCableType);
	
	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	pPropWnd->GetSelectionSet().SetValue(_T("From") , _T("Cable Type") , rSelCableType.operator LPCSTR());

	//! Cable Core ComboBox를 채운다.
	m_wndFromCableCoreCombo.ResetContent();
	CELoadDocData& docData = CELoadDocData::GetInstance();
	vector<string> CableTypeList;
	docData.m_ProjectCableDataTable.GetCableTypeKeyList(CableTypeList);
	for(vector<string>::iterator itr = CableTypeList.begin(); itr != CableTypeList.end();++itr)
	{	
		CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(*itr);
		if(pCableDataByType)
		{
			vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
			if(pRecordList)
			{
				for(vector<CCableDataByType::Record*>::iterator jtr = pRecordList->begin();jtr != pRecordList->end();++jtr)
				{
					const CString rCableVolt = (*jtr)->m_FieldMap[_T("C_VOLT")].c_str();
					const CString rCableType = (*jtr)->m_FieldMap[_T("C_CABLE_TYPE")].c_str();
					if((rCableVolt == rSelCableVolt) && (rCableType == rSelCableType))
					{
						const CString rCableCore = (*jtr)->m_FieldMap[_T("C_CORE")].c_str();
						const int nFound = m_wndFromCableCoreCombo.FindString(-1 , rCableCore);
						if(-1 == nFound) m_wndFromCableCoreCombo.AddString(rCableCore);
					}
				}
			}
		}
	}

	const string rCableCore = pPropWnd->GetSelectionSet().GetValue(_T("From") , _T("Cable Core"));
	const int nFound = m_wndFromCableCoreCombo.FindString(-1 , rCableCore.c_str());
	if(-1 != nFound)
		m_wndFromCableCoreCombo.SetCurSel(nFound);
	else
	{
		m_wndFromCableCoreCombo.SetWindowText(_T(""));
		pPropWnd->GetSelectionSet().SetValue(_T("From") , _T("Cable Core") , _T(""));
	}
}

/**
	@brief	CABLE CORE COMBO SELECT CHANGE

	@author KHS	

	@date 2009-05-08 오전 11:18:19	

	@param	

	@return		
**/
void CCapacitorPropDlg::OnCbnSelchangeComboFromCore()
{
	
	const int nCableCoreCurSel = m_wndFromCableCoreCombo.GetCurSel();
	if(-1 == nCableCoreCurSel) return;
	CString rSelCableCore;
	m_wndFromCableCoreCombo.GetLBText(nCableCoreCurSel , rSelCableCore);
	
	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	pPropWnd->GetSelectionSet().SetValue(_T("From") , _T("Cable Core") , rSelCableCore.operator LPCSTR());
}

/**
	@brief	BUS ID COMBO SELECT CHANGE

	@author KHS	

	@date 2009-05-08 오전 11:16:17	

	@param	

	@return		
**/
void CCapacitorPropDlg::OnCbnSelchangeComboFromBusId()
{
	const int nCurSel = m_wndFromBusIdCombo.GetCurSel();
	if(-1 != nCurSel)
	{
		CString rString;
		m_wndFromBusIdCombo.GetLBText(nCurSel , rString);

		CELoadDocData& docData = CELoadDocData::GetInstance();
		CBusItem* pBusItem = docData.FindBusItemByName(rString.operator LPCSTR());
		if(pBusItem)
		{
			CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
			pPropWnd->GetSelectionSet().SetValue(_T("From") , _T("Bus Id") , rString.operator LPCSTR());

			const string rBusVolt = pBusItem->prop()->GetValue(_T("General") , _T("Bus Voltage"));
			const string rLoadVolt= pBusItem->prop()->GetValue(_T("General") , _T("Load Voltage"));

			m_wndFromBusVoltEdit.SetWindowText(rBusVolt.c_str());
			m_wndFromLoadVoltEdit.SetWindowText(rLoadVolt.c_str());

			pPropWnd->GetSelectionSet().SetValue(_T("From") , _T("Bus Volt") , rBusVolt.c_str());
			pPropWnd->GetSelectionSet().SetValue(_T("From") , _T("Load Volt"), rLoadVolt.c_str());
			
			DisplayLoadSummaryResult();
		}
	}
}

/**
	@brief	CHANGED TARGET POWER FACTOR EDIT

	@author KHS	

	@return		
**/
void CCapacitorPropDlg::OnEnChangeEditTargetPowerFactor()
{
	CString rTargetPowerFactor;
	m_wndTargetPowerFactorEdit.GetWindowText(rTargetPowerFactor);

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	pPropWnd->GetSelectionSet().SetValue(_T("Capacity") , _T("Target Power Factor") , rTargetPowerFactor.operator LPCSTR());
	
	double dTargetPowerFactor = SetCalculatedCapacityText();
}

/**
	@brief	CHANGED SELECTED CAPACITY EDIT

	@author KHS	

	@return		
**/
void CCapacitorPropDlg::OnEnChangeEditSelectedCapacity()
{
	CString rSelectedCapacity;
	m_wndSelectedCapacityEdit.GetWindowText(rSelectedCapacity);

	//if(rSelectedCapacity.IsEmpty()) return;

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	pPropWnd->GetSelectionSet().SetValue(_T("Capacity") , _T("Selected Capacity") , rSelectedCapacity.operator LPCSTR());

	SetSelectedCapacityText();
}

/**
	@brief	CHANGE CABLE SPEC EDIT

	@author KHS	

	@return		
**/
void CCapacitorPropDlg::OnEnChangeEditFromSpec()
{
	CString rCableSpec;
	m_wndFromCableSpecEdit.GetWindowText(rCableSpec);

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	pPropWnd->GetSelectionSet().SetValue(_T("From") , _T("Cable Spec") , rCableSpec.operator LPCSTR());
}

/**
	@brief	CALCULATED CAPACITY

	@author KHS	

	@return		
**/
int CCapacitorPropDlg::SetCalculatedCapacityText()
{
	double capacity = 0.f;

	BeginWaitCursor();

	m_wndCalculatedCapacityEdit.SetWindowText(_T(""));
	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	if(pPropWnd)
	{
		CCapacitorItem* pCapacitorItem = (CCapacitorItem*)pPropWnd->GetSelectionSet().GetItemAt(0);
		if(pCapacitorItem)
		{
			CString rTargetPowerFactor;
			m_wndTargetPowerFactorEdit.GetWindowText(rTargetPowerFactor);
			if(rTargetPowerFactor.IsEmpty()) return ERROR_BAD_ENVIRONMENT;

			double dTargetFowerFactor = atof(rTargetPowerFactor);
			capacity = pCapacitorItem->CalculateCapacity(dTargetFowerFactor);
			
			stringstream oss;
			oss << capacity;
			m_wndCalculatedCapacityEdit.SetWindowText(oss.str().c_str());

			pCapacitorItem->prop()->SetValue(_T("Capacity") , _T("Calculated Capacity") , oss.str());
		}
	}
	
	EndWaitCursor();

	return ERROR_SUCCESS;
}

/**
	@brief	DISPLAY 선택한 BUS에 연관된 BUS LIST를 표시한다.

	@author KHS	

	@date 2009-05-08 오후 1:51:53	

	@param	

	@return		
**/
int CCapacitorPropDlg::DisplayLoadSummaryResult()
{
	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	if(pPropWnd)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		const int nCurSel = m_wndFromBusIdCombo.GetCurSel();
		if(-1 != nCurSel)
		{
			CString rBusID;
			m_wndFromBusIdCombo.GetLBText(nCurSel , rBusID);
			CBusItem* pBusItem = docData.FindBusItemByName(rBusID.operator LPCSTR());
			if(pBusItem)
			{
				double nKW = 0.f , nKVAR = 0.f , nKVA = 0.f;

				CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
				if(pSettingTable)
				{
					string* pValue = pSettingTable->GetFieldValue(_T("C_APPLY_CONNECTED_TRANSFORMER"));
					docData.m_bApplyTrToLoadSummary = ((NULL != pValue) && (_T("YES") == (*pValue))) ? true : false;
				}

				CELoadDocData& docData = CELoadDocData::GetInstance();
				if(TRUE == docData.m_bApplyTrToLoadSummary)
				{
					pBusItem->GetLoadSummaryResultData(nKW , nKVAR);
					nKVA = SAFE_ROUND(sqrt(nKW*nKW + nKVAR*nKVAR) , 2);
					nKW = SAFE_ROUND(nKW , 2);
					nKVA= SAFE_ROUND(nKVA , 2);
					
					CString rString;
					rString.Format(_T("%.2lf") , nKW);
					m_wndTotalKWEdit.SetWindowText(rString);
					pPropWnd->GetSelectionSet().GetItemAt(0)->prop()->SetValue(_T("Capacity") , _T("Total KW") , rString.operator LPCSTR());
					
					rString.Format(_T("%.2lf") , nKVA);
					m_wndTotalKVAEdit.SetWindowText(rString);
					pPropWnd->GetSelectionSet().GetItemAt(0)->prop()->SetValue(_T("Capacity") , _T("Total KVA") , rString.operator LPCSTR());
				}
				else
				{
					CString rString = pPropWnd->GetSelectionSet().GetItemAt(0)->prop()->GetValue(_T("Capacity") , _T("Total KW")).c_str();
					nKW = atof(rString);
					nKW = SAFE_ROUND(nKW , 2);
					rString.Format(_T("%.2lf") , nKW);
					m_wndTotalKWEdit.SetWindowText(rString);
					
					rString = pPropWnd->GetSelectionSet().GetItemAt(0)->prop()->GetValue(_T("Capacity") , _T("Total KVA")).c_str();
					nKVA = atof(rString);
					nKVA = SAFE_ROUND(nKVA , 2);
					rString.Format(_T("%.2lf") , nKVA);
					m_wndTotalKVAEdit.SetWindowText(rString);
				}

				m_wndTotalKWEdit.EnableWindow(docData.m_bApplyTrToLoadSummary ? FALSE : TRUE);
				m_wndTotalKVAEdit.EnableWindow(docData.m_bApplyTrToLoadSummary ? FALSE : TRUE);

				const double nOriginalPowerFactor = SAFE_ROUND((nKW / nKVA) , 3);
				ostringstream oss;
				oss << nOriginalPowerFactor;
				SetDlgItemText(IDC_EDIT_ORIGINAL_POWER_FACTOR , oss.str().c_str());
				pPropWnd->GetSelectionSet().GetItemAt(0)->prop()->SetValue(_T("Capacity") , _T("Original Power Factor") , oss.str());
			}
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author KHS	

	@date 2009-05-08 오후 3:38:44	

	@param	

	@return		
**/
int CCapacitorPropDlg::SetSelectedCapacityText(void)
{
	BeginWaitCursor();
	
	double dFlc = 0.f, dImprovedPowerFactor = 0.f;
	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	if(pPropWnd)
	{
		CCapacitorItem* pCapacitorItem = (CCapacitorItem*)pPropWnd->GetSelectionSet().GetItemAt(0);
		if(pCapacitorItem)
		{
			CString rSelectedCapacity;
			m_wndSelectedCapacityEdit.GetWindowText(rSelectedCapacity);
			
			m_wndFlcEdit.SetWindowText("");
			m_wndImprovedPowerFactorEdit.SetWindowText("");

			if(rSelectedCapacity.IsEmpty()) return ERROR_BAD_ENVIRONMENT;

			const double dSelectedCapacity = atof(rSelectedCapacity);
			pCapacitorItem->SetSelectedCapacity(dFlc, dImprovedPowerFactor,dSelectedCapacity);
			
			stringstream ossFlc, ossImprovedPowerFactor;
			ossFlc << dFlc;
			ossImprovedPowerFactor << dImprovedPowerFactor;

			m_wndFlcEdit.SetWindowText(ossFlc.str().c_str());
			m_wndImprovedPowerFactorEdit.SetWindowText(ossImprovedPowerFactor.str().c_str());

			pCapacitorItem->prop()->SetValue(_T("Capacity") , _T("FLC") , ossFlc.str());
			pCapacitorItem->prop()->SetValue(_T("Capacity") , _T("Improved Power Factor") , ossImprovedPowerFactor.str());
		}
	}
	
	EndWaitCursor();

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-20 오전 12:01:55	

	@param	

	@return		
**/
void CCapacitorPropDlg::OnNMClickListConnectedList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	/*
	CPoint point;
	GetCursorPos(&point);
	m_wndConnectedBusListCtrl.ScreenToClient(&point);

	UINT uFlags;
	const int nItem = m_wndConnectedBusListCtrl.HitTest(point , &uFlags);
	if((-1 != nItem) && (uFlags & LVHT_ONITEMSTATEICON))	//! ICON부분을 클릭 했을때...
	{
		CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
		if(pPropWnd)
		{
			CCapacitorItem* pCapacitorItem = (CCapacitorItem*)pPropWnd->GetSelectionSet().GetItemAt(0);
			if(pCapacitorItem)
			{
				CString rValue;
				POSITION pos = m_wndConnectedBusListCtrl.GetFirstSelectedItemPosition();
				while(pos)
				{
					const int nSelItem = m_wndConnectedBusListCtrl.GetNextSelectedItem(pos);
					m_wndConnectedBusListCtrl.GetItemText(nSelItem , 0);
				}
				if(_T(",") == rValue.Right(1)) rValue = rValue.Left(rValue.GetLength() - 1);
				pCapacitorItem->prop()->SetValue(_T("Capacity") , _T("Connected ID List") , rValue.operator LPCSTR());
			}
		}
	}
	*/

	*pResult = 0;
}

void CCapacitorPropDlg::OnEnChangeEditCapacitorPf()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString rString;
	GetDlgItemText(IDC_EDIT_CAPACITOR_PF , rString);

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	pPropWnd->GetSelectionSet().SetValue(_T("General") , _T("PF") , rString.operator LPCSTR());
}

void CCapacitorPropDlg::OnEnChangeEditCapacitorLength()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString rString;
	GetDlgItemText(IDC_EDIT_CAPACITOR_LENGTH , rString);

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	pPropWnd->GetSelectionSet().SetValue(_T("General") , _T("Cable Length") , rString.operator LPCSTR());
}

/**
	@brief	

	@author BHK	

	@date 2009-09-02 오전 10:46:02	

	@param	

	@return		
*/
void CCapacitorPropDlg::OnEnChangeEditTotalKw()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString rTotalKW;
	m_wndTotalKWEdit.GetWindowText(rTotalKW);

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	if(pPropWnd) pPropWnd->GetSelectionSet().SetValue(_T("Capacity") , _T("Total KW") , rTotalKW.operator LPCSTR());
}

/**
	@brief	

	@author BHK	

	@date 2009-09-02 오전 10:47:33	

	@param	

	@return		
*/
void CCapacitorPropDlg::OnEnChangeEditTotalKva()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString rTotalKVA;
	m_wndTotalKVAEdit.GetWindowText(rTotalKVA);

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	if(pPropWnd) pPropWnd->GetSelectionSet().SetValue(_T("Capacity") , _T("Total KVA") , rTotalKVA.operator LPCSTR());
}
