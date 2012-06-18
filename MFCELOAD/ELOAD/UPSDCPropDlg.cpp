// UPSDCPropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "UPSDCPropDlg.h"
#include "ELoadDocData.h"
#include "PropertiesWnd.h"
#include <math.h>
#include <sstream>
#include "UPSDCItem.h"
#include "Tables/CableCreationSettingTable.h"

int CUPSDCPropDlg::IDEAL_WIDTH = 550;
// CUPSDCPropDlg dialog

IMPLEMENT_DYNAMIC(CUPSDCPropDlg, CDialog)

CUPSDCPropDlg::CUPSDCPropDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUPSDCPropDlg::IDD, pParent)
{

}

CUPSDCPropDlg::~CUPSDCPropDlg()
{
}

void CUPSDCPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_COMBO_FROM1_BUS_ID , m_wndFrom1BusIdCombo);
	DDX_Control(pDX , IDC_COMBO_FROM2_BUS_ID , m_wndFrom2BusIdCombo);
	DDX_Control(pDX , IDC_COMBO_TO_BUS_ID , m_wndToBusIdCombo);

	DDX_Control(pDX , IDC_COMBO_FROM2_CABLE_VOLT , m_wndFrom2CableVoltCombo);
	DDX_Control(pDX , IDC_COMBO_FROM2_CABLE_TYPE , m_wndFrom2CableTypeCombo);
	DDX_Control(pDX , IDC_COMBO_FROM2_CABLE_CORE , m_wndFrom2CableCoreCombo);

	DDX_Control(pDX , IDC_EDIT_FROM2_SPEC , m_wndFrom2CableSpecEdit);
}


BEGIN_MESSAGE_MAP(CUPSDCPropDlg, CDialog)
	ON_REGISTERED_MESSAGE(CEventData::SELECT_ITEM , OnItemSelected)
	ON_CBN_SELENDOK(IDC_COMBO_FROM2_CABLE_VOLT, &CUPSDCPropDlg::OnCbnSelendokComboFrom2CableVolt)
	ON_CBN_SELENDOK(IDC_COMBO_FROM2_CABLE_TYPE, &CUPSDCPropDlg::OnCbnSelendokComboFrom2CableType)
	ON_CBN_SELENDOK(IDC_COMBO_FROM1_BUS_ID, &CUPSDCPropDlg::OnCbnSelendokComboFrom1BusId)
	ON_CBN_SELENDOK(IDC_COMBO_FROM2_BUS_ID, &CUPSDCPropDlg::OnCbnSelendokComboFrom2BusId)
	ON_CBN_SELENDOK(IDC_COMBO_TO_BUS_ID, &CUPSDCPropDlg::OnCbnSelendokComboToBusId)
	ON_EN_CHANGE(IDC_EDIT_FROM2_SPEC, &CUPSDCPropDlg::OnEnChangeEditFrom2Spec)
	ON_CBN_SELENDOK(IDC_COMBO_FROM2_CABLE_CORE, &CUPSDCPropDlg::OnCbnSelendokComboFrom2CableCore)
	ON_EN_CHANGE(IDC_EDIT_INPUT_KVA, &CUPSDCPropDlg::OnEnChangeEditInputKva)
	ON_EN_CHANGE(IDC_EDIT_INPUT_PF, &CUPSDCPropDlg::OnEnChangeEditInputPf)
	ON_EN_CHANGE(IDC_EDIT_AC_RATING_PF, &CUPSDCPropDlg::OnEnChangeEditAcRatingPf)
	ON_EN_CHANGE(IDC_EDIT_DC_RATING_FLC, &CUPSDCPropDlg::OnEnChangeEditDcRatingFlc)
	ON_BN_CLICKED(IDC_RADIO_UPS_CAPACITY, &CUPSDCPropDlg::OnBnClickedRadioUpsCapacity)
	ON_BN_CLICKED(IDC_RADIO_DC_CAPACITY, &CUPSDCPropDlg::OnBnClickedRadioDcCapacity)
	ON_CBN_SELENDOK(IDC_COMBO_OP_MODE1, &CUPSDCPropDlg::OnCbnSelendokComboOpMode1)
	ON_CBN_SELENDOK(IDC_COMBO_OP_MODE2, &CUPSDCPropDlg::OnCbnSelendokComboOpMode2)
	ON_EN_CHANGE(IDC_EDIT_INPUT_BHP, &CUPSDCPropDlg::OnEnChangeEditInputBhp)
	ON_EN_CHANGE(IDC_EDIT_AC_RATING_BHP, &CUPSDCPropDlg::OnEnChangeEditAcRatingBhp)
	ON_EN_CHANGE(IDC_EDIT_UPSDC_PF, &CUPSDCPropDlg::OnEnChangeEditUpsdcPf)
	ON_EN_CHANGE(IDC_EDIT_UPSDC_LENGTH, &CUPSDCPropDlg::OnEnChangeEditUpsdcLength)
END_MESSAGE_MAP()


// CUPSDCPropDlg message handlers
/**
	@brief	

	@author KHS	

	@date 2009-05-08 오전 9:26:26	

	@param	

	@return		
**/
LRESULT CUPSDCPropDlg::OnItemSelected(WPARAM wParam, LPARAM lParam)
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
int CUPSDCPropDlg::SetEventDataValues(CEventData* pEventData)
{
	assert(pEventData && "pEventData is NULL");

	if(pEventData)
	{
		BeginWaitCursor();

		const string rUPSDCID = pEventData->m_SelectionSet.GetValue( _T("General") , _T("UPSDC ID") );
		SetDlgItemText(IDC_EDIT_UPSDC_ID , rUPSDCID.c_str());
		const string rUPSDCPF = pEventData->m_SelectionSet.GetValue( _T("General") , _T("PF") );
		SetDlgItemText(IDC_EDIT_UPSDC_PF , rUPSDCPF.c_str());
		const string rUPSDCLength = pEventData->m_SelectionSet.GetValue( _T("General") , _T("Cable Length") );
		SetDlgItemText(IDC_EDIT_UPSDC_LENGTH , rUPSDCLength.c_str());
		
		CELoadDocData& docData = CELoadDocData::GetInstance();
		
		vector<CBusItem*> BusItemEntry;
		docData.GetAllBusItems(BusItemEntry);
		
		const string rBusID1 = pEventData->m_SelectionSet.GetValue( _T("From1") , _T("Bus ID"));
		m_wndFrom1BusIdCombo.ResetContent();
		for(vector<CBusItem*>::iterator itr = BusItemEntry.begin();itr != BusItemEntry.end();++itr)
		{
			m_wndFrom1BusIdCombo.AddString((*itr)->GetName().c_str());
		}

		m_wndFrom2BusIdCombo.ResetContent();
		for(vector<CBusItem*>::iterator itr = BusItemEntry.begin();itr != BusItemEntry.end();++itr)
		{
			m_wndFrom2BusIdCombo.AddString((*itr)->GetName().c_str());
		}

		m_wndToBusIdCombo.ResetContent();
		for(vector<CBusItem*>::iterator itr = BusItemEntry.begin();itr != BusItemEntry.end();++itr)
		{
			m_wndToBusIdCombo.AddString((*itr)->GetName().c_str());
		}
		
		{
			const string rBusID = pEventData->m_SelectionSet.GetValue( _T("From1") , _T("Bus ID"));
			const int nItem = m_wndFrom1BusIdCombo.FindString(-1 , rBusID.c_str());
			if(-1 != nItem) m_wndFrom1BusIdCombo.SetCurSel(nItem);
			OnCbnSelendokComboFrom1BusId();

			const string rBusVolt = pEventData->m_SelectionSet.GetValue( _T("From1") , _T("Bus Volt"));
			SetDlgItemText(IDC_EDIT_FROM1_BUS_VOLT ,  rBusVolt.c_str());

			const string rLoadVolt = pEventData->m_SelectionSet.GetValue( _T("From1") , _T("Load Volt"));
			SetDlgItemText(IDC_EDIT_FROM1_LOAD_VOLT ,  rLoadVolt.c_str());

			const string rOperMode = pEventData->m_SelectionSet.GetValue( _T("From1") , _T("Operation Mode"));
			CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_OP_MODE1);
			if(pComboBox)
			{
				const int nFound = pComboBox->FindString(-1 , rOperMode.c_str());
				if(-1 != nFound) pComboBox->SetCurSel(nFound);
			}
		}
		{
			const string rBusID = pEventData->m_SelectionSet.GetValue( _T("From2") , _T("Bus ID"));
			const int nItem = m_wndFrom2BusIdCombo.FindString(-1 , rBusID.c_str());
			if(-1 != nItem) m_wndFrom2BusIdCombo.SetCurSel(nItem);

			const string rBusVolt = pEventData->m_SelectionSet.GetValue( _T("From2") , _T("Bus Volt"));
			SetDlgItemText(IDC_EDIT_FROM2_BUS_VOLT ,  rBusVolt.c_str());

			const string rLoadVolt = pEventData->m_SelectionSet.GetValue( _T("From2") , _T("Load Volt"));
			SetDlgItemText(IDC_EDIT_FROM2_LOAD_VOLT ,  rLoadVolt.c_str());

			const string rOperMode = pEventData->m_SelectionSet.GetValue( _T("From2") , _T("Operation Mode"));
			CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_OP_MODE2);
			if(pComboBox)
			{
				const int nFound = pComboBox->FindString(-1 , rOperMode.c_str());
				if(-1 != nFound) pComboBox->SetCurSel(nFound);
			}

			const string rCableSpec = pEventData->m_SelectionSet.GetValue( _T("From2") , _T("Cable Spec"));
			SetDlgItemText(IDC_EDIT_FROM2_SPEC ,  rCableSpec.c_str());
		}
		{
			const string rBusID = pEventData->m_SelectionSet.GetValue( _T("To") , _T("Bus ID"));
			const int nItem = m_wndToBusIdCombo.FindString(-1 , rBusID.c_str());
			if(-1 != nItem) m_wndToBusIdCombo.SetCurSel(nItem);
			OnCbnSelendokComboToBusId();

			const string rBusVolt = pEventData->m_SelectionSet.GetValue( _T("To") , _T("Bus Volt"));
			SetDlgItemText(IDC_EDIT_TO_BUS_VOLT ,  rBusVolt.c_str());

			const string rLoadVolt = pEventData->m_SelectionSet.GetValue( _T("To") , _T("Load Volt"));
			SetDlgItemText(IDC_EDIT_TO_LOAD_VOLT ,  rLoadVolt.c_str());
		}
		
		m_wndFrom2CableVoltCombo.ResetContent();
		m_wndFrom2CableTypeCombo.ResetContent();
		m_wndFrom2CableCoreCombo.ResetContent();

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
							const int nFound = m_wndFrom2CableVoltCombo.FindString(0 , rCableVolt);
							if(-1 == nFound)
							{
								m_wndFrom2CableVoltCombo.AddString(rCableVolt);
							}
						}
					}
				}
			}

			const string rCableVolt = pEventData->m_SelectionSet.GetValue(_T("From2") , _T("Cable Volt"));
			int nIndex = m_wndFrom2CableVoltCombo.FindString(-1 , rCableVolt.c_str());
			if(-1 != nIndex)
			{
				m_wndFrom2CableVoltCombo.SetCurSel(nIndex);
				OnCbnSelendokComboFrom2CableVolt();
			}
		}
		else
		{
			///AfxMessageBox(_T("CableData 테이블이 설정되지 않았습니다."));
			::OutputDebugString(_T("CableData 테이블이 설정되지 않았습니다."));
		}

		const string rCableSpec = pEventData->m_SelectionSet.GetValue( _T("From2") , _T("Cable Spec"));
		m_wndFrom2CableSpecEdit.SetWindowText(rCableSpec.c_str());

		const string rSelectedCapacity = pEventData->m_SelectionSet.GetValue( _T("General") , _T("SELECTED CAPACITY"));
		

		if(rSelectedCapacity == _T("UPS Capacity"))
		{
			CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_UPS_CAPACITY);
			pButton->SetCheck(TRUE);
			pButton = (CButton*)GetDlgItem(IDC_RADIO_DC_CAPACITY);
			pButton->SetCheck(FALSE);

			OnBnClickedRadioUpsCapacity();
		}
		else
		{
			CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_UPS_CAPACITY);
			pButton->SetCheck(FALSE);
			pButton = (CButton*)GetDlgItem(IDC_RADIO_DC_CAPACITY);
			pButton->SetCheck(TRUE);

			OnBnClickedRadioDcCapacity();
		}
		{
			string rLoadVolt = pEventData->m_SelectionSet.GetValue( _T("UPS Capacity") , _T("Input Load Volt") );
			SetDlgItemText(IDC_EDIT_INPUT_LOAD_VOLT , rLoadVolt.c_str());

			string rKVA = pEventData->m_SelectionSet.GetValue( _T("UPS Capacity") , _T("KVA") );
			SetDlgItemText(IDC_EDIT_INPUT_KVA , rKVA.c_str());
			string rBHP = pEventData->m_SelectionSet.GetValue( _T("UPS Capacity") , _T("BHP") );
			SetDlgItemText(IDC_EDIT_INPUT_BHP , rBHP.c_str());
			string rFLC = pEventData->m_SelectionSet.GetValue( _T("UPS Capacity") , _T("FLC") );
			SetDlgItemText(IDC_EDIT_INPUT_FLC , rFLC.c_str());
			string rPF = pEventData->m_SelectionSet.GetValue( _T("UPS Capacity") , _T("PF") );
			SetDlgItemText(IDC_EDIT_INPUT_PF , rPF.c_str());

			rLoadVolt = pEventData->m_SelectionSet.GetValue( _T("UPS Capacity") , _T("Output Load Volt") );
			SetDlgItemText(IDC_EDIT_OUTPUT_LOAD_VOLT , rLoadVolt.c_str());
			
			rLoadVolt = pEventData->m_SelectionSet.GetValue( _T("DC Capacity") , _T("AC Rating Load Volt") );
			SetDlgItemText(IDC_EDIT_AC_RATING_LOAD_VOLT , rLoadVolt.c_str());

			rKVA = pEventData->m_SelectionSet.GetValue( _T("DC Capacity") , _T("AC Rating KVA") );
			SetDlgItemText(IDC_EDIT_AC_RATING_KVA , rKVA.c_str());
			rBHP = pEventData->m_SelectionSet.GetValue( _T("DC Capacity") , _T("AC Rating BHP") );
			SetDlgItemText(IDC_EDIT_AC_RATING_BHP , rBHP.c_str());
			rFLC = pEventData->m_SelectionSet.GetValue( _T("DC Capacity") , _T("AC Rating FLC") );
			SetDlgItemText(IDC_EDIT_AC_RATING_FLC , rFLC.c_str());
			rPF = pEventData->m_SelectionSet.GetValue( _T("DC Capacity") , _T("AC Rating PF") );
			SetDlgItemText(IDC_EDIT_AC_RATING_PF , rPF.c_str());

			rLoadVolt = pEventData->m_SelectionSet.GetValue( _T("DC Capacity") , _T("DC Rating Load Volt") );
			SetDlgItemText(IDC_EDIT_DC_RATING_LOAD_VOLT , rLoadVolt.c_str());

			rFLC = pEventData->m_SelectionSet.GetValue( _T("DC Capacity") , _T("DC Rating FLC") );
			SetDlgItemText(IDC_EDIT_DC_RATING_FLC , rFLC.c_str());
		}

		EndWaitCursor();

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	

	@author BHK	

	@date 2009-06-03 오후 11:51:25	

	@param	

	@return		
**/
void CUPSDCPropDlg::OnCbnSelendokComboFrom2CableVolt()
{
	int nCurSel = m_wndFrom2CableVoltCombo.GetCurSel();
	if(-1 == nCurSel) return;
	CString rSelCableVolt;
	m_wndFrom2CableVoltCombo.GetLBText(nCurSel , rSelCableVolt);
	
	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	pPropWnd->GetSelectionSet().SetValue(_T("From2") , _T("Cable Volt") , rSelCableVolt.operator LPCSTR());

	//! Cable Type ComboBox를 채운다.
	m_wndFrom2CableTypeCombo.ResetContent();

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
						const int nFound = m_wndFrom2CableTypeCombo.FindString(-1 , rCableType);
						if(-1 == nFound) m_wndFrom2CableTypeCombo.AddString(rCableType);
					}
				}
			}
		}
	}

	const string rCableType = pPropWnd->GetSelectionSet().GetValue(_T("From2") , _T("Cable Type"));
	int nFound = m_wndFrom2CableTypeCombo.FindString(-1 , rCableType.c_str());
	if(-1 != nFound) 
		m_wndFrom2CableTypeCombo.SetCurSel(nFound);
	else
	{
		pPropWnd->GetSelectionSet().SetValue(_T("From2") , _T("Cable Type") , _T(""));
		m_wndFrom2CableTypeCombo.SetWindowText(_T(""));
	}

	OnCbnSelendokComboFrom2CableType();
}

/**
	@brief	

	@author BHK	

	@date 2009-06-03 오후 11:53:07	

	@param	

	@return		
**/
void CUPSDCPropDlg::OnCbnSelendokComboFrom2CableType()
{
	const int nCableVoltCurSel = m_wndFrom2CableVoltCombo.GetCurSel();
	const int nCableTypeCurSel = m_wndFrom2CableTypeCombo.GetCurSel();

	if((-1 == nCableVoltCurSel) || (-1 == nCableTypeCurSel)) return;

	CString rSelCableVolt , rSelCableType;
	m_wndFrom2CableVoltCombo.GetLBText(nCableVoltCurSel , rSelCableVolt);
	m_wndFrom2CableTypeCombo.GetLBText(nCableTypeCurSel , rSelCableType);
	
	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	pPropWnd->GetSelectionSet().SetValue(_T("From2") , _T("Cable Type") , rSelCableType.operator LPCSTR());

	//! Cable Core ComboBox를 채운다.
	m_wndFrom2CableCoreCombo.ResetContent();
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
						const int nFound = m_wndFrom2CableCoreCombo.FindString(-1 , rCableCore);
						if(-1 == nFound) m_wndFrom2CableCoreCombo.AddString(rCableCore);
					}
				}
			}
		}
	}

	const string rCableCore = pPropWnd->GetSelectionSet().GetValue(_T("From2") , _T("Cable Core"));
	const int nFound = m_wndFrom2CableCoreCombo.FindString(-1 , rCableCore.c_str());
	if(-1 != nFound)
		m_wndFrom2CableCoreCombo.SetCurSel(nFound);
	else
	{
		m_wndFrom2CableCoreCombo.SetWindowText(_T(""));
		pPropWnd->GetSelectionSet().SetValue(_T("From2") , _T("Cable Core") , _T(""));
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-06-03 오후 11:58:47	

	@param	

	@return		
**/
void CUPSDCPropDlg::OnCbnSelendokComboFrom1BusId()
{
	const int nCurSel = m_wndFrom1BusIdCombo.GetCurSel();
	if(-1 != nCurSel)
	{
		CString rString;
		m_wndFrom1BusIdCombo.GetLBText(nCurSel , rString);

		CELoadDocData& docData = CELoadDocData::GetInstance();
		CBusItem* pBusItem = docData.FindBusItemByName(rString.operator LPCSTR());
		if(pBusItem)
		{
			CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
			pPropWnd->GetSelectionSet().SetValue(_T("From1") , _T("Bus ID") , rString.operator LPCSTR());

			const string rBusVolt = pBusItem->prop()->GetValue(_T("General") , _T("Bus Voltage"));
			const string rLoadVolt= pBusItem->prop()->GetValue(_T("General") , _T("Load Voltage"));

			SetDlgItemText(IDC_EDIT_FROM1_BUS_VOLT , rBusVolt.c_str());
			SetDlgItemText(IDC_EDIT_FROM1_LOAD_VOLT , rLoadVolt.c_str());

			pPropWnd->GetSelectionSet().SetValue(_T("From1") , _T("Bus Volt") , rBusVolt.c_str());
			pPropWnd->GetSelectionSet().SetValue(_T("From1") , _T("Load Volt"), rLoadVolt.c_str());

			/*CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_UPS_CAPACITY);
			if(pButton && pButton->GetCheck())*/
			{
				SetDlgItemText(IDC_EDIT_INPUT_LOAD_VOLT , rLoadVolt.c_str());
				//SetDlgItemText(IDC_EDIT_AC_RATING_LOAD_VOLT , _T(""));
				pPropWnd->GetSelectionSet().SetValue(_T("UPS Capacity") , _T("Input Load Volt"), rLoadVolt.c_str());
				//pPropWnd->GetSelectionSet().SetValue(_T("DC Capacity") , _T("AC Rating Load Volt"), _T(""));
			}
			/*else*/
			{
				SetDlgItemText(IDC_EDIT_AC_RATING_LOAD_VOLT , rLoadVolt.c_str());
				//SetDlgItemText(IDC_EDIT_INPUT_LOAD_VOLT , _T(""));
				pPropWnd->GetSelectionSet().SetValue(_T("DC Capacity") , _T("AC Rating Load Volt"), rLoadVolt.c_str());
				//pPropWnd->GetSelectionSet().SetValue(_T("UPS Capacity") , _T("Input Load Volt"), _T(""));
			}
		}
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-06-04 오전 12:34:24	

	@param	

	@return		
**/
void CUPSDCPropDlg::OnCbnSelendokComboFrom2BusId()
{
	const int nCurSel = m_wndFrom2BusIdCombo.GetCurSel();
	if(-1 != nCurSel)
	{
		CString rString;
		m_wndFrom2BusIdCombo.GetLBText(nCurSel , rString);

		CELoadDocData& docData = CELoadDocData::GetInstance();
		CBusItem* pBusItem = docData.FindBusItemByName(rString.operator LPCSTR());
		if(pBusItem)
		{
			CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
			pPropWnd->GetSelectionSet().SetValue(_T("From2") , _T("Bus ID") , rString.operator LPCSTR());

			const string rBusVolt = pBusItem->prop()->GetValue(_T("General") , _T("Bus Voltage"));
			const string rLoadVolt= pBusItem->prop()->GetValue(_T("General") , _T("Load Voltage"));

			SetDlgItemText(IDC_EDIT_FROM2_BUS_VOLT , rBusVolt.c_str());
			SetDlgItemText(IDC_EDIT_FROM2_LOAD_VOLT , rLoadVolt.c_str());

			pPropWnd->GetSelectionSet().SetValue(_T("From2") , _T("Bus Volt") , rBusVolt.c_str());
			pPropWnd->GetSelectionSet().SetValue(_T("From2") , _T("Load Volt"), rLoadVolt.c_str());
		}
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-06-04 오전 12:37:02	

	@param	

	@return		
**/
void CUPSDCPropDlg::OnCbnSelendokComboToBusId()
{
	const int nCurSel = m_wndToBusIdCombo.GetCurSel();
	if(-1 != nCurSel)
	{
		CString rString;
		m_wndToBusIdCombo.GetLBText(nCurSel , rString);

		CELoadDocData& docData = CELoadDocData::GetInstance();
		CBusItem* pBusItem = docData.FindBusItemByName(rString.operator LPCSTR());
		if(pBusItem)
		{
			CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
			pPropWnd->GetSelectionSet().SetValue(_T("To") , _T("Bus ID") , rString.operator LPCSTR());

			const string rBusVolt = pBusItem->prop()->GetValue(_T("General") , _T("Bus Voltage"));
			const string rLoadVolt= pBusItem->prop()->GetValue(_T("General") , _T("Load Voltage"));

			SetDlgItemText(IDC_EDIT_TO_BUS_VOLT , rBusVolt.c_str());
			SetDlgItemText(IDC_EDIT_TO_LOAD_VOLT , rLoadVolt.c_str());

			pPropWnd->GetSelectionSet().SetValue(_T("To") , _T("Bus Volt") , rBusVolt.c_str());
			pPropWnd->GetSelectionSet().SetValue(_T("To") , _T("Load Volt"), rLoadVolt.c_str());

			/*CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_UPS_CAPACITY);
			if(pButton && pButton->GetCheck())*/
			{
				SetDlgItemText(IDC_EDIT_OUTPUT_LOAD_VOLT , rLoadVolt.c_str());
				//SetDlgItemText(IDC_EDIT_DC_RATING_LOAD_VOLT , _T(""));
				pPropWnd->GetSelectionSet().SetValue(_T("UPS Capacity") , _T("Output Load Volt"), rLoadVolt.c_str());
				//pPropWnd->GetSelectionSet().SetValue(_T("DC Capacity") , _T("DC Rating Load Volt"), _T(""));
			}
			//else
			{
				SetDlgItemText(IDC_EDIT_DC_RATING_LOAD_VOLT , rLoadVolt.c_str());
				//SetDlgItemText(IDC_EDIT_OUTPUT_LOAD_VOLT , _T(""));
				pPropWnd->GetSelectionSet().SetValue(_T("DC Capacity") , _T("DC Rating Load Volt"), rLoadVolt.c_str());
				//pPropWnd->GetSelectionSet().SetValue(_T("UPS Capacity") , _T("Output Load Volt"), _T(""));
			}
		}
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-06-04 오전 12:53:42	

	@param	

	@return		
**/
void CUPSDCPropDlg::OnEnChangeEditFrom2Spec()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	if(pPropWnd)
	{
		CString rString;
		GetDlgItemText(IDC_EDIT_FROM2_SPEC , rString);

		pPropWnd->GetSelectionSet().SetValue(_T("From2") , _T("Cable Spec") , rString.operator LPCSTR());
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-06-04 오전 12:55:18	

	@param	

	@return		
**/
void CUPSDCPropDlg::OnCbnSelendokComboFrom2CableCore()
{
	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	if(pPropWnd)
	{
		const int nCableCoreCurSel = m_wndFrom2CableCoreCombo.GetCurSel();
		if(-1 != nCableCoreCurSel)
		{
			CString rSelCableCore;
			m_wndFrom2CableCoreCombo.GetLBText(nCableCoreCurSel , rSelCableCore);
			pPropWnd->GetSelectionSet().SetValue(_T("From2") , _T("Cable Core") , rSelCableCore.operator LPCSTR());
		}
	}
}

/**
	@brief	UPS Capacity의 FLC값을 계산한다.

	@author BHK	

	@date 2009-06-04 오전 9:16:53	

	@param	

	@return		
*/
int CUPSDCPropDlg::CalculateFLCOfUPSCapacity()
{
	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	if(pPropWnd)
	{
		CString rKVA;
		GetDlgItemText(IDC_EDIT_INPUT_KVA , rKVA);
		
		CString rLoadVolt;
		GetDlgItemText(IDC_EDIT_INPUT_LOAD_VOLT , rLoadVolt);
		if(!rLoadVolt.IsEmpty() && 0.f != atof(rLoadVolt))
		{
			const double nFLC = atof(rKVA) / atof(rLoadVolt) / sqrt(3.f);
			CString rFLC;
			rFLC.Format(_T("%.3lf") , nFLC);

			SetDlgItemText(IDC_EDIT_INPUT_FLC , rFLC);
			pPropWnd->GetSelectionSet().SetValue(_T("UPS Capacity") , _T("FLC") , rFLC.operator LPCSTR());
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-06-04 오전 9:22:34	

	@param	

	@return		
*/
void CUPSDCPropDlg::OnEnChangeEditInputKva()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	if(pPropWnd)
	{
		CString rKVA;
		GetDlgItemText(IDC_EDIT_INPUT_KVA , rKVA);
		pPropWnd->GetSelectionSet().SetValue(_T("UPS Capacity") , _T("KVA") , rKVA.operator LPCSTR());

		CalculateFLCOfUPSCapacity();
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-06-04 오전 9:23:37	

	@param	

	@return		
*/
void CUPSDCPropDlg::OnEnChangeEditInputPf()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	if(pPropWnd)
	{
		CString rPF;
		GetDlgItemText(IDC_EDIT_INPUT_PF , rPF);
		pPropWnd->GetSelectionSet().SetValue(_T("UPS Capacity") , _T("PF") , rPF.operator LPCSTR());
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-06-04 오전 9:24:53	

	@param	

	@return		
*/
int CUPSDCPropDlg::CalculateKVAAndFLCOfDCCapacity()
{
	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	if(pPropWnd)
	{
		CString rPF;
		GetDlgItemText(IDC_EDIT_AC_RATING_PF , rPF);
		
		CString rLoadVolt , rDCFLC;
		GetDlgItemText(IDC_EDIT_DC_RATING_LOAD_VOLT , rLoadVolt);
		GetDlgItemText(IDC_EDIT_DC_RATING_FLC , rDCFLC);
		{
			const double nKVA = (atof(rDCFLC) * atof(rLoadVolt));
			CString rKVA;
			rKVA.Format(_T("%.2lf") , nKVA);

			SetDlgItemText(IDC_EDIT_AC_RATING_KVA , rKVA);
			pPropWnd->GetSelectionSet().SetValue(_T("DC Capacity") , _T("AC Rating KVA") , rKVA.operator LPCSTR());
		}

		CString rKVA;
		GetDlgItemText(IDC_EDIT_AC_RATING_KVA , rKVA);
		GetDlgItemText(IDC_EDIT_AC_RATING_LOAD_VOLT , rLoadVolt);
		if(!rLoadVolt.IsEmpty() && (0.f != atof(rLoadVolt)))
		{
			const double nFLC = atof(rKVA) / atof(rLoadVolt) / sqrt(3.f);
			CString rFLC;
			rFLC.Format(_T("%.3lf") , nFLC);

			SetDlgItemText(IDC_EDIT_AC_RATING_FLC , rFLC);
			pPropWnd->GetSelectionSet().SetValue(_T("DC Capacity") , _T("AC Rating FLC") , rFLC.operator LPCSTR());
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-06-04 오전 9:24:16	

	@param	

	@return		
*/
void CUPSDCPropDlg::OnEnChangeEditAcRatingPf()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	if(pPropWnd)
	{
		CString rPF;
		GetDlgItemText(IDC_EDIT_AC_RATING_PF , rPF);
		pPropWnd->GetSelectionSet().SetValue(_T("DC Capacity") , _T("AC Rating PF") , rPF.operator LPCSTR());

		CalculateKVAAndFLCOfDCCapacity();
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-06-04 오전 9:31:20	

	@param	

	@return		
*/
void CUPSDCPropDlg::OnEnChangeEditDcRatingFlc()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	if(pPropWnd)
	{
		CString rFLC;
		GetDlgItemText(IDC_EDIT_DC_RATING_FLC , rFLC);
		pPropWnd->GetSelectionSet().SetValue(_T("DC Capacity") , _T("DC Rating FLC") , rFLC.operator LPCSTR());

		CalculateKVAAndFLCOfDCCapacity();
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-06-04 오후 7:22:52	

	@param	

	@return		
*/
void CUPSDCPropDlg::OnBnClickedRadioUpsCapacity()
{
	CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_UPS_CAPACITY);
	if(pButton && pButton->GetCheck())
	{
		GetDlgItem(IDC_EDIT_INPUT_KVA)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_INPUT_PF)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_INPUT_BHP)->EnableWindow(TRUE);

		GetDlgItem(IDC_EDIT_AC_RATING_PF)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_DC_RATING_FLC)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_AC_RATING_BHP)->EnableWindow(FALSE);

		CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
		if(pPropWnd) pPropWnd->GetSelectionSet().SetValue(_T("General") , _T("SELECTED CAPACITY") , _T("UPS Capacity"));
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-06-04 오후 7:25:21	

	@param	

	@return		
*/
void CUPSDCPropDlg::OnBnClickedRadioDcCapacity()
{
	CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_DC_CAPACITY);
	if(pButton && pButton->GetCheck())
	{
		GetDlgItem(IDC_EDIT_AC_RATING_PF)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_DC_RATING_FLC)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_AC_RATING_BHP)->EnableWindow(TRUE);

		GetDlgItem(IDC_EDIT_INPUT_KVA)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_INPUT_PF)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_INPUT_BHP)->EnableWindow(FALSE);

		CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
		if(pPropWnd) pPropWnd->GetSelectionSet().SetValue(_T("General") , _T("SELECTED CAPACITY") , _T("DC Capacity"));
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-06-08 오후 10:54:07	

	@param	

	@return		
**/
void CUPSDCPropDlg::OnCbnSelendokComboOpMode1()
{
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_OP_MODE1);
	if(pComboBox)
	{
		const int nSel = pComboBox->GetCurSel();
		if(-1 != nSel)
		{
			CString rString;
			pComboBox->GetLBText(nSel , rString);

			CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
			if(pPropWnd) pPropWnd->GetSelectionSet().SetValue(_T("From1") , _T("Operation Mode") , rString.operator LPCSTR());
		}
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-06-08 오후 10:56:42	

	@param	

	@return		
**/
void CUPSDCPropDlg::OnCbnSelendokComboOpMode2()
{
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_OP_MODE2);
	if(pComboBox)
	{
		const int nSel = pComboBox->GetCurSel();
		if(-1 != nSel)
		{
			CString rString;
			pComboBox->GetLBText(nSel , rString);

			CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
			if(pPropWnd) pPropWnd->GetSelectionSet().SetValue(_T("From2") , _T("Operation Mode") , rString.operator LPCSTR());
		}
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-06-15 오후 8:15:29	

	@param	

	@return		
*/
void CUPSDCPropDlg::OnEnChangeEditInputBhp()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	if(pPropWnd)
	{
		CString rBHP;
		GetDlgItemText(IDC_EDIT_INPUT_BHP , rBHP);
		
		/*
		if(rBHP.IsEmpty() || (0. == atof(rBHP.operator LPCSTR())))
		{
			//! set BHP as KVA value.
			rBHP = pPropWnd->GetSelectionSet().GetValue(_T("UPS Capacity") , _T("KVA")).c_str();
			if(!rBHP.IsEmpty()) SetDlgItemText(IDC_EDIT_INPUT_BHP , rBHP);
		}
		*/

		pPropWnd->GetSelectionSet().SetValue(_T("UPS Capacity") , _T("BHP") , rBHP.operator LPCSTR());
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-06-15 오후 8:17:20	

	@param	

	@return		
*/
void CUPSDCPropDlg::OnEnChangeEditAcRatingBhp()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	if(pPropWnd)
	{
		CString rBHP;
		GetDlgItemText(IDC_EDIT_AC_RATING_BHP , rBHP);
		/*
		if(rBHP.IsEmpty() || (0. == atof(rBHP.operator LPCSTR())))
		{
			//! set BHP as KVA value.
			rBHP = pPropWnd->GetSelectionSet().GetValue(_T("DC Capacity") , _T("AC Rating KVA")).c_str();
			if(!rBHP.IsEmpty()) SetDlgItemText(IDC_EDIT_AC_RATING_BHP , rBHP);
		}
		*/

		pPropWnd->GetSelectionSet().SetValue(_T("DC Capacity") , _T("AC Rating BHP") , rBHP.operator LPCSTR());
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-08-28 오후 3:33:17	

	@param	

	@return		
*/
void CUPSDCPropDlg::OnEnChangeEditUpsdcPf()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString rString;
	GetDlgItemText(IDC_EDIT_UPSDC_PF , rString);

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	pPropWnd->GetSelectionSet().SetValue(_T("General") , _T("PF") , rString.operator LPCSTR());
}

/**
	@brief	

	@author BHK	

	@date 2009-08-28 오후 3:33:33	

	@param	

	@return		
*/
void CUPSDCPropDlg::OnEnChangeEditUpsdcLength()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString rString;
	GetDlgItemText(IDC_EDIT_UPSDC_LENGTH , rString);

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	pPropWnd->GetSelectionSet().SetValue(_T("General") , _T("Cable Length") , rString.operator LPCSTR());
}
