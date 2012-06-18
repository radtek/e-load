// TransformerPropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "MainFrm.h"
#include "ELoadDocData.h"
#include "PropertiesWnd.h"
#include "TransformerPropDlg.h"
#include <gui/GridCtrl/GridCellCheck.h>

#include <math.h>
#include <sstream>
using namespace std;

int CTransformerPropDlg::IDEAL_WIDTH = 335;

// CTransformerPropDlg dialog

IMPLEMENT_DYNAMIC(CTransformerPropDlg, CDialog)

/**
*/
CTransformerPropDlg::CTransformerPropDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTransformerPropDlg::IDD, pParent)
{
}

CTransformerPropDlg::~CTransformerPropDlg()
{
}

void CTransformerPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//! 2011.02.07 added by HumKyung
	DDX_Control(pDX , IDC_BUTTON_TR_ADD , m_wndAddButton);
	DDX_Control(pDX , IDC_BUTTON_TR_DELETE , m_wndDeleteButton);
	DDX_Control(pDX , IDC_BUTTON_TR_SAVE , m_wndSaveButton);
}


BEGIN_MESSAGE_MAP(CTransformerPropDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CTransformerPropDlg::OnBnClickedOK)
	ON_BN_CLICKED(IDCANCEL, &CTransformerPropDlg::OnBnClickedCancel)
	ON_REGISTERED_MESSAGE(CEventData::SELECT_ITEM , OnItemSelected)
	ON_BN_CLICKED(IDC_RADIO_CABLE, &CTransformerPropDlg::OnBnClickedRadioCable)
	ON_BN_CLICKED(IDC_RADIO_BUS_DUCT, &CTransformerPropDlg::OnBnClickedRadioBusDuct)
	ON_CBN_SELENDOK(IDC_COMBO_FROM_BUS_ID, &CTransformerPropDlg::OnCbnSelendokComboFromBusId)
	ON_CBN_SELENDOK(IDC_COMBO_TO_BUS_ID, &CTransformerPropDlg::OnCbnSelendokComboToBusId)
	ON_CBN_SELENDOK(IDC_COMBO_FROM_CABLE_VOLT, &CTransformerPropDlg::OnCbnSelendokComboFromCableVolt)
	ON_CBN_SELENDOK(IDC_COMBO_FROM_CABLE_TYPE, &CTransformerPropDlg::OnCbnSelendokComboFromCableType)
	ON_CBN_SELENDOK(IDC_COMBO_TO_CABLE_VOLT, &CTransformerPropDlg::OnCbnSelendokComboToCableVolt)
	ON_CBN_SELENDOK(IDC_COMBO_TO_CABLE_TYPE, &CTransformerPropDlg::OnCbnSelendokComboToCableType)
	ON_CBN_SELENDOK(IDC_COMBO_FROM_CORE, &CTransformerPropDlg::OnCbnSelendokComboFromCore)
	ON_CBN_SELENDOK(IDC_COMBO_TO_CORE, &CTransformerPropDlg::OnCbnSelendokComboToCore)
	ON_EN_CHANGE(IDC_EDIT_SELECTED_CAPACITY, &CTransformerPropDlg::OnEnChangeEditSelectedCapacity)
	ON_EN_CHANGE(IDC_EDIT_FROM_SPEC, &CTransformerPropDlg::OnEnChangeEditFromSpec)
	ON_EN_CHANGE(IDC_EDIT_TO_SPEC, &CTransformerPropDlg::OnEnChangeEditToSpec)
	ON_EN_CHANGE(IDC_EDIT_TRANSFORMER_PF, &CTransformerPropDlg::OnEnChangeEditTransformerPf)
	ON_EN_CHANGE(IDC_EDIT_TRANSFORMER_LENGTH, &CTransformerPropDlg::OnEnChangeEditTransformerLength)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_TR_ADD, &CTransformerPropDlg::OnBnClickedButtonTrAdd)
	ON_BN_CLICKED(IDC_BUTTON_TR_SAVE, &CTransformerPropDlg::OnBnClickedButtonTrSave)
	ON_BN_CLICKED(IDC_BUTTON_TR_DELETE, &CTransformerPropDlg::OnBnClickedButtonTrDelete)
END_MESSAGE_MAP()

/**
	@brief	OnOK를 호출하지 않아 다이얼로그가 종료되는 것을 막는다.

	@author	HumKyung

	@date	????.??.??

	@param

	@return
*/
void CTransformerPropDlg::OnBnClickedOK()
{
}

/**
	@brief	OnCacel를 호출하지 않아 다이얼로그가 종료되는 것을 막는다.

	@author	BHK
*/
void CTransformerPropDlg::OnBnClickedCancel()
{
}

// CTransformerPropDlg message handlers

/**
	@brief	사용자가 Item을 선택했을 때...

	@author	BHK
*/
LRESULT CTransformerPropDlg::OnItemSelected(WPARAM wParam, LPARAM lParam)
{
	CEventData* pEventData = (CEventData*)lParam;
	if(pEventData) SetEventDataValues(pEventData);
		
	return 0;
}

/**
	@brief	

	@author	BHK
*/
int CTransformerPropDlg::SetEventDataValues( CEventData* pEventData )
{
	assert(pEventData && "pEventData is NULL");

	if(pEventData)
	{
		BeginWaitCursor();

		string rValue       = pEventData->m_SelectionSet.GetValue( _T("General") , _T("TR ID") );
		SetDlgItemText(IDC_EDIT_TRANSFORMER_ID , rValue.c_str());
		const string rTRPF = pEventData->m_SelectionSet.GetValue( _T("General") , _T("PF") );
		SetDlgItemText(IDC_EDIT_TRANSFORMER_PF , rTRPF.c_str());
		const string rTRLength = pEventData->m_SelectionSet.GetValue( _T("General") , _T("Cable Length") );
		SetDlgItemText(IDC_EDIT_TRANSFORMER_LENGTH , rTRLength.c_str());

		rValue = pEventData->m_SelectionSet.GetValue( _T("From") , _T("Bus Id") );
		GetDlgItem(IDC_COMBO_FROM_BUS_ID)->SetWindowText(rValue.c_str());
		rValue = pEventData->m_SelectionSet.GetValue( _T("From") , _T("Bus Volt") );
		SetDlgItemText(IDC_EDIT_FROM_BUS_VOLT , rValue.c_str());
		rValue = pEventData->m_SelectionSet.GetValue( _T("From") , _T("Load Volt") );
		SetDlgItemText(IDC_EDIT_FROM_LOAD_VOLT , rValue.c_str());
		
		///rValue = pEventData->m_SelectionSet.GetValue( _T("From") , _T("Cable Volt"));
		///SetDlgItemText(IDC_COMBO_FROM_CABLE_VOLT , rValue.c_str());
		rValue = pEventData->m_SelectionSet.GetValue( _T("From") , _T("Cable Type"));
		SetDlgItemText(IDC_COMBO_FROM_CABLE_TYPE , rValue.c_str());
		rValue = pEventData->m_SelectionSet.GetValue( _T("From") , _T("Cable Core"));
		SetDlgItemText(IDC_COMBO_FROM_CORE , rValue.c_str());
		rValue = pEventData->m_SelectionSet.GetValue( _T("From") , _T("Cable Spec"));
		SetDlgItemText(IDC_EDIT_FROM_SPEC , rValue.c_str());
		
		rValue = pEventData->m_SelectionSet.GetValue( _T("To") , _T("Bus Id") );
		GetDlgItem(IDC_COMBO_TO_BUS_ID)->SetWindowText(rValue.c_str());
		rValue = pEventData->m_SelectionSet.GetValue( _T("To") , _T("Bus Volt") );
		SetDlgItemText(IDC_EDIT_TO_BUS_VOLT , rValue.c_str());
		rValue = pEventData->m_SelectionSet.GetValue( _T("To") , _T("Load Volt") );
		SetDlgItemText(IDC_EDIT_TO_LOAD_VOLT , rValue.c_str());

		///rValue = pEventData->m_SelectionSet.GetValue( _T("To") , _T("Cable Volt"));
		///SetDlgItemText(IDC_COMBO_TO_CABLE_VOLT , rValue.c_str());
		rValue = pEventData->m_SelectionSet.GetValue( _T("To") , _T("Cable Type"));
		SetDlgItemText(IDC_COMBO_TO_CABLE_TYPE , rValue.c_str());
		rValue = pEventData->m_SelectionSet.GetValue( _T("To") , _T("Cable Core"));
		SetDlgItemText(IDC_COMBO_TO_CORE , rValue.c_str());
		rValue = pEventData->m_SelectionSet.GetValue( _T("To") , _T("Cable Spec"));
		SetDlgItemText(IDC_EDIT_TO_SPEC , rValue.c_str());
		
		vector<CBusItem*> BusItemEntry;
		CELoadDocData& docData = CELoadDocData::GetInstance();
		docData.GetAllBusItems(BusItemEntry);
		{
			CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_FROM_BUS_ID);
			if(pComboBox)
			{
				pComboBox->ResetContent();
				for(vector<CBusItem*>::iterator itr = BusItemEntry.begin();itr != BusItemEntry.end();++itr)
				{
					pComboBox->AddString((*itr)->GetName().c_str());
				}

				rValue = pEventData->m_SelectionSet.GetValue( _T("From") , _T("Bus Id"));
				const int nFound = pComboBox->FindString(-1 , rValue.c_str());
				if(-1 != nFound) pComboBox->SetCurSel(nFound);
				OnCbnSelendokComboFromBusId();
			}
		}

		{
			CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_TO_BUS_ID);
			if(pComboBox)
			{
				pComboBox->ResetContent();
				for(vector<CBusItem*>::iterator itr = BusItemEntry.begin();itr != BusItemEntry.end();++itr)
				{
					pComboBox->AddString((*itr)->GetName().c_str());
				}

				rValue = pEventData->m_SelectionSet.GetValue( _T("To") , _T("Bus Id"));
				const int nFound = pComboBox->FindString(-1 , rValue.c_str());
				if(-1 != nFound) pComboBox->SetCurSel(nFound);
				OnCbnSelendokComboToBusId();
			}
		}

		{
			CComboBox* pFromCableVoltComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_FROM_CABLE_VOLT);
			CComboBox* pToCableVoltComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_TO_CABLE_VOLT);
			CComboBox* pFromCableTypeComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_FROM_CABLE_TYPE);
			CComboBox* pToCableTypeComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_TO_CABLE_TYPE);
			CComboBox* pFromCableCoreComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_FROM_CORE);
			CComboBox* pToCableCoreComboBox   = (CComboBox*)GetDlgItem(IDC_COMBO_TO_CORE);
			
			if(pFromCableVoltComboBox) pFromCableVoltComboBox->ResetContent();
			if(pToCableVoltComboBox) pToCableVoltComboBox->ResetContent();
			if(pFromCableTypeComboBox) pFromCableTypeComboBox->ResetContent();
			if(pToCableTypeComboBox) pToCableTypeComboBox->ResetContent();
			if(pFromCableCoreComboBox) pFromCableCoreComboBox->ResetContent();
			if(pToCableCoreComboBox) pToCableCoreComboBox->ResetContent();

			CELoadDocData& docData = CELoadDocData::GetInstance();
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
								if(pFromCableVoltComboBox)
								{
									const int nFound = pFromCableVoltComboBox->FindString(-1 , rCableVolt);
									if(-1 == nFound)
									{
										if(pFromCableVoltComboBox) pFromCableVoltComboBox->AddString(rCableVolt);
										if(pToCableVoltComboBox) pToCableVoltComboBox->AddString(rCableVolt);
									}
								}
							}
						}
					}
				}

				const string rFromCableVolt = pEventData->m_SelectionSet.GetValue(_T("From") , _T("Cable Volt"));
				if(pFromCableVoltComboBox)
				{
					const int nFound = pFromCableVoltComboBox->FindString(-1 , rFromCableVolt.c_str());
					if(-1 != nFound)
					{
						if(pFromCableVoltComboBox) pFromCableVoltComboBox->SetCurSel(nFound);
						OnCbnSelendokComboFromCableVolt();
					}
				}
				
				const string rToCableVolt = pEventData->m_SelectionSet.GetValue(_T("To") , _T("Cable Volt"));
				if(pToCableVoltComboBox)
				{
					const int nFound = pToCableVoltComboBox->FindString(-1 , rToCableVolt.c_str());
					if(-1 != nFound)
					{
						if(pToCableVoltComboBox) pToCableVoltComboBox->SetCurSel(nFound);
						OnCbnSelendokComboToCableVolt();
					}
				}
			}
			else
			{
				CMainFrame* pMainFrm = ELOADGetMainWnd();
				if(pMainFrm) pMainFrm->m_wndOutput.AppendLogMsg(_T("CableData 테이블이 설정되지 않았습니다."));
			}
		}

		const string rFromCableSpec = pEventData->m_SelectionSet.GetValue(_T("From") , _T("Cable Spec"));
		SetDlgItemText(IDC_EDIT_FROM_SPEC , rFromCableSpec.c_str());
		const string rToCableSpec = pEventData->m_SelectionSet.GetValue(_T("To") , _T("Cable Spec"));
		SetDlgItemText(IDC_EDIT_TO_SPEC , rToCableSpec.c_str());
		/*
		{
			const string rConnection = pEventData->m_SelectionSet.GetValue(_T("To") , _T("Connection"));
			if(_T("BUS DUCT") == rConnection)
			{
				CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_BUS_DUCT);
				if(pButton) pButton->SetCheck(TRUE);
				pButton = (CButton*)GetDlgItem(IDC_RADIO_CABLE);
				if(pButton) pButton->SetCheck(FALSE);

				OnBnClickedRadioBusDuct();
			}
			else
			{
				CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_CABLE);
				if(pButton) pButton->SetCheck(TRUE);
				pButton = (CButton*)GetDlgItem(IDC_RADIO_BUS_DUCT);
				if(pButton) pButton->SetCheck(FALSE);

				OnBnClickedRadioCable();
			}
		}
		*/

		{
			const int res = SetCalculatedCapacityText();
			const string rSelectedCapacity = pEventData->m_SelectionSet.GetValue(_T("Capacity") , _T("Selected Capacity"));
			SetDlgItemText(IDC_EDIT_SELECTED_CAPACITY , rSelectedCapacity.c_str());
		}

		//! 2011.02.08 added by HumKyung
		CTransformerItem* pTrItem = static_cast<CTransformerItem*>(pEventData->m_SelectionSet.GetItemAt(0));
		DisplayBusNameListOfTr(pTrItem);
		//!

		EndWaitCursor();

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	

	@author	HumKyung

	@date	????.??.??

	@param

	@return
*/
void CTransformerPropDlg::OnBnClickedRadioCable()
{
	CWnd* pWnd = GetDlgItem(IDC_COMBO_TO_CABLE_VOLT);
	if(pWnd) pWnd->EnableWindow(TRUE);
	pWnd = GetDlgItem(IDC_COMBO_TO_CABLE_TYPE);
	if(pWnd) pWnd->EnableWindow(TRUE);
	pWnd = GetDlgItem(IDC_COMBO_TO_CORE);
	if(pWnd) pWnd->EnableWindow(TRUE);
	pWnd = GetDlgItem(IDC_EDIT_TO_SPEC);
	if(pWnd) pWnd->EnableWindow(TRUE);

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	pPropWnd->GetSelectionSet().SetValue(_T("To") , _T("Connection") , _T("CABLE"));

}

/**
	@brief	

	@author	BHK
*/
void CTransformerPropDlg::OnBnClickedRadioBusDuct()
{
	GetDlgItem(IDC_COMBO_TO_CABLE_VOLT)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_TO_CABLE_TYPE)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_TO_CORE)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_TO_SPEC)->EnableWindow(FALSE);

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	pPropWnd->GetSelectionSet().SetValue(_T("To") , _T("Connection") , _T("BUS DUCT"));
}

/**
	@brief	

	@author	HumKyung

	@date	????.??.??

	@param

	@return
*/
void CTransformerPropDlg::OnCbnSelendokComboFromBusId()
{
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_FROM_BUS_ID);
	if(pComboBox)
	{
		const int nCurSel = pComboBox->GetCurSel();
		if(-1 != nCurSel)
		{
			CString rString;
			pComboBox->GetLBText(nCurSel , rString);

			CELoadDocData& docData = CELoadDocData::GetInstance();
			CBusItem* pBusItem = docData.FindBusItemByName(rString.operator LPCSTR());
			if(pBusItem)
			{
				CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
				pPropWnd->GetSelectionSet().SetValue(_T("From") , _T("Bus Id") , rString.operator LPCSTR());

				const string rBusVolt = pBusItem->prop()->GetValue(_T("General") , _T("Bus Voltage"));
				const string rLoadVolt= pBusItem->prop()->GetValue(_T("General") , _T("Load Voltage"));
				SetDlgItemText(IDC_EDIT_FROM_BUS_VOLT , CString(rBusVolt.c_str()));
				SetDlgItemText(IDC_EDIT_FROM_LOAD_VOLT, CString(rLoadVolt.c_str()));
				pPropWnd->GetSelectionSet().SetValue(_T("From") , _T("Bus Volt") , rBusVolt.c_str());
				pPropWnd->GetSelectionSet().SetValue(_T("From") , _T("Load Volt"), rLoadVolt.c_str());
			}
		}
	}
}

/**
	@brief	called after selecting to bus

	@author	HumKyung

	@date	????.??.??

	@param

	@return
*/
void CTransformerPropDlg::OnCbnSelendokComboToBusId()
{
	//! 2011.02.11 added by HumKyung
	CString sFromBusId;
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_FROM_BUS_ID);
	if(pComboBox)
	{
		const int nCurSel = pComboBox->GetCurSel();
		if(-1 != nCurSel)
		{
			pComboBox->GetLBText(nCurSel , sFromBusId);
		}
	}
	//!

	pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_TO_BUS_ID);
	if(pComboBox)
	{
		const int nCurSel = pComboBox->GetCurSel();
		if(-1 != nCurSel)
		{
			CString rString;
			pComboBox->GetLBText(nCurSel , rString);
			if(sFromBusId != rString)
			{
				CELoadDocData& docData = CELoadDocData::GetInstance();
				CBusItem* pBusItem = docData.FindBusItemByName(rString.operator LPCSTR());
				if(pBusItem)
				{
					CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
					pPropWnd->GetSelectionSet().SetValue(_T("To") , _T("Bus Id") , rString.operator LPCSTR());

					const string rBusVolt = pBusItem->prop()->GetValue(_T("General") , _T("Bus Voltage"));
					const string rLoadVolt= pBusItem->prop()->GetValue(_T("General") , _T("Load Voltage"));
					SetDlgItemText(IDC_EDIT_TO_BUS_VOLT , CString(rBusVolt.c_str()));
					SetDlgItemText(IDC_EDIT_TO_LOAD_VOLT, CString(rLoadVolt.c_str()));
					pPropWnd->GetSelectionSet().SetValue(_T("To") , _T("Bus Volt") , rBusVolt.c_str());
					pPropWnd->GetSelectionSet().SetValue(_T("To") , _T("Load Volt"), rLoadVolt.c_str());

					SetCalculatedCapacityText();

					if(m_wndBusGrid.GetRowCount() <= 1)
					{
						m_wndBusGrid.SetRowCount(2);
					}

					//! set first row - 2011.02.24 added by HumKyung
					if(m_wndBusGrid.GetRowCount() > 1)
					{
						m_wndBusGrid.SetCellType(1 , 0 , RUNTIME_CLASS(CGridCellCheck));

						m_wndBusGrid.SetItemText(1 , 2 , rString);
						string sBusGroup;
						CBusItem* pBusItem = docData.FindBusItemByName(rString.operator LPCSTR());
						if(NULL != pBusItem) sBusGroup = pBusItem->prop()->GetValue(_T("General") , _T("Bus Group"));

						m_wndBusGrid.SetItemText(1 , 1 , sBusGroup.c_str());

						m_wndBusGrid.SetCellType(1 , 1 , RUNTIME_CLASS(CGridCellBusGroupCombo));
						{
							CGridCellBusGroupCombo* pCell = (CGridCellBusGroupCombo*)(m_wndBusGrid.GetCell(1 , 2));
							if(pCell) pCell->SetStyle(CBS_DROPDOWNLIST);
						}

						m_wndBusGrid.SetCellType(1 , 2 , RUNTIME_CLASS(CGridCellBusCombo));
						{
							CGridCellBusCombo* pCell = (CGridCellBusCombo*)(m_wndBusGrid.GetCell(1 , 2));
							if(pCell) pCell->SetStyle(CBS_DROPDOWNLIST);
						}
						
						m_wndBusGrid.RedrawRow(1);
						m_wndBusGrid.AutoSizeRows();
					}
				}
			}
		}
	}
}

/**
	@brief	CableVolt를 선택했을때 CableType ComboBox를 채운다.

	@author	HumKyung

	@date	????.??.??

	@param	

	@return	
*/
void CTransformerPropDlg::OnCbnSelendokComboFromCableVolt()
{
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_FROM_CABLE_VOLT);
	const int nCurSel = pComboBox->GetCurSel();
	if(-1 == nCurSel) return;
	CString rSelCableVolt;
	pComboBox->GetLBText(nCurSel , rSelCableVolt);
	
	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	pPropWnd->GetSelectionSet().SetValue(_T("From") , _T("Cable Volt") , rSelCableVolt.operator LPCSTR());

	CComboBox* pFromCableTypeComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_FROM_CABLE_TYPE);
	CComboBox* pFromCableCoreComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_FROM_CORE);

	//! Cable Type ComboBox를 채운다.
	pFromCableTypeComboBox->ResetContent();
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
						const int nFound = pFromCableTypeComboBox->FindString(-1 , rCableType);
						if(-1 == nFound) pFromCableTypeComboBox->AddString(rCableType);
					}
				}
			}
		}
	}

	///pFromCableCoreComboBox->ResetContent();
	const string rCableType = pPropWnd->GetSelectionSet().GetValue(_T("From") , _T("Cable Type"));
	int nFound = pFromCableTypeComboBox->FindString(-1 , rCableType.c_str());
	if(-1 != nFound) 
		pFromCableTypeComboBox->SetCurSel(nFound);
	else
	{
		pPropWnd->GetSelectionSet().SetValue(_T("From") , _T("Cable Type") , _T(""));
		pFromCableTypeComboBox->SetWindowText(_T(""));
	}

	OnCbnSelendokComboFromCableType();

	/*
	const string rCableCore = pPropWnd->GetSelectionSet().SetValue(_T("From") , _T("Cable Core"));
	nFound = pFromCableCoreComboBox->FindString(-1 , rCableCore.c_str());
	if(-1 != nFound) pFromCableCoreComboBox->SetCurSel(nFound);
	*/
}

/**
	@brief	CableType를 선택했을때 CableCore ComboBox를 채운다.

	@author	BHK
*/
void CTransformerPropDlg::OnCbnSelendokComboFromCableType()
{
	CComboBox* pCableVoltComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_FROM_CABLE_VOLT);
	const int nCableVoltCurSel = pCableVoltComboBox->GetCurSel();
	CComboBox* pCableTypeComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_FROM_CABLE_TYPE);
	const int nCableTypeCurSel = pCableTypeComboBox->GetCurSel();
	if((-1 == nCableVoltCurSel) || (-1 == nCableTypeCurSel)) return;
	CString rSelCableVolt , rSelCableType;
	pCableVoltComboBox->GetLBText(nCableVoltCurSel , rSelCableVolt);
	pCableTypeComboBox->GetLBText(nCableTypeCurSel , rSelCableType);
	
	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	pPropWnd->GetSelectionSet().SetValue(_T("From") , _T("Cable Type") , rSelCableType.operator LPCSTR());

	//! Cable Core ComboBox를 채운다.
	CComboBox* pCableCoreComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_FROM_CORE);
	pCableCoreComboBox->ResetContent();
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
						const int nFound = pCableCoreComboBox->FindString(-1 , rCableCore);
						if(-1 == nFound) pCableCoreComboBox->AddString(rCableCore);
					}
				}
			}
		}
	}

	const string rCableCore = pPropWnd->GetSelectionSet().GetValue(_T("From") , _T("Cable Core"));
	const int nFound = pCableCoreComboBox->FindString(-1 , rCableCore.c_str());
	if(-1 != nFound)
		pCableCoreComboBox->SetCurSel(nFound);
	else
	{
		pCableCoreComboBox->SetWindowText(_T(""));
		pPropWnd->GetSelectionSet().SetValue(_T("From") , _T("Cable Core") , _T(""));
	}

	///OnCbnSelendokComboFromCore();
}

/**
	@brief	CableVolt를 선택했을때 CableType ComboBox를 채운다.

	@author	BHK
*/
void CTransformerPropDlg::OnCbnSelendokComboToCableVolt()
{
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_TO_CABLE_VOLT);
	const int nCurSel = pComboBox->GetCurSel();
	if(-1 == nCurSel) return;
	CString rSelCableVolt;
	pComboBox->GetLBText(nCurSel , rSelCableVolt);
	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	pPropWnd->GetSelectionSet().SetValue(_T("To") , _T("Cable Volt") , rSelCableVolt.operator LPCSTR());

	CComboBox* pToCableTypeComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_TO_CABLE_TYPE);
	///CComboBox* pToCableCoreComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_TO_CORE);
	pToCableTypeComboBox->ResetContent();
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
						const int nFound = pToCableTypeComboBox->FindString(-1 , rCableType);
						if(-1 == nFound) pToCableTypeComboBox->AddString(rCableType);
					}
				}
			}
		}
	}

	///pToCableCoreComboBox->ResetContent();
	const string rCableType = pPropWnd->GetSelectionSet().GetValue(_T("To") , _T("Cable Type"));
	const int nFound = pToCableTypeComboBox->FindString(-1 , rCableType.c_str());
	if(-1 != nFound)
		pToCableTypeComboBox->SetCurSel(nFound);
	else
	{
		pToCableTypeComboBox->SetWindowText(_T(""));
		pPropWnd->GetSelectionSet().SetValue(_T("To") , _T("Cable Type") , _T(""));
	}

	OnCbnSelendokComboToCableType();
}

/**
	@brief	CableType를 선택했을때 CableCore ComboBox를 채운다.

	@author	BHK
*/
void CTransformerPropDlg::OnCbnSelendokComboToCableType()
{
	CComboBox* pCableVoltComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_TO_CABLE_VOLT);
	const int nCableVoltCurSel = pCableVoltComboBox->GetCurSel();
	CComboBox* pCableTypeComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_TO_CABLE_TYPE);
	const int nCableTypeCurSel = pCableTypeComboBox->GetCurSel();
	if((-1 == nCableVoltCurSel) || (-1 == nCableTypeCurSel)) return;
	CString rSelCableVolt , rSelCableType;
	pCableVoltComboBox->GetLBText(nCableVoltCurSel , rSelCableVolt);
	pCableTypeComboBox->GetLBText(nCableTypeCurSel , rSelCableType);
	
	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	pPropWnd->GetSelectionSet().SetValue(_T("To") , _T("Cable Type") , rSelCableType.operator LPCSTR());

	//! Cable Core ComboBox를 채운다.
	CComboBox* pCableCoreComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_TO_CORE);
	pCableCoreComboBox->ResetContent();
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
						const int nFound = pCableCoreComboBox->FindString(-1 , rCableCore);
						if(-1 == nFound) pCableCoreComboBox->AddString(rCableCore);
					}
				}
			}
		}
	}

	const string rCableCore = pPropWnd->GetSelectionSet().GetValue(_T("To") , _T("Cable Core"));
	const int nFound = pCableCoreComboBox->FindString(-1 , rCableCore.c_str());
	if(-1 != nFound)
		pCableCoreComboBox->SetCurSel(nFound);
	else
	{
		pPropWnd->GetSelectionSet().SetValue(_T("To") , _T("Cable Core") , _T(""));
		pCableCoreComboBox->SetWindowText(_T(""));
	}

	///OnCbnSelendokComboToCore();
}

/**
	@brief	

	@author	BHK
*/
void CTransformerPropDlg::OnCbnSelendokComboFromCore()
{
	CComboBox* pCableCoreComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_FROM_CORE);
	const int nCableCoreCurSel = pCableCoreComboBox->GetCurSel();
	if(-1 == nCableCoreCurSel) return;
	CString rSelCableCore;
	pCableCoreComboBox->GetLBText(nCableCoreCurSel , rSelCableCore);
	
	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	pPropWnd->GetSelectionSet().SetValue(_T("From") , _T("Cable Core") , rSelCableCore.operator LPCSTR());
}

/**
	@brief	

	@author	BHK
*/
void CTransformerPropDlg::OnCbnSelendokComboToCore()
{
	CComboBox* pCableCoreComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_TO_CORE);
	const int nCableCoreCurSel = pCableCoreComboBox->GetCurSel();
	if(-1 == nCableCoreCurSel) return;
	CString rSelCableCore;
	pCableCoreComboBox->GetLBText(nCableCoreCurSel , rSelCableCore);
	
	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	pPropWnd->GetSelectionSet().SetValue(_T("To") , _T("Cable Core") , rSelCableCore.operator LPCSTR());
}

/**
	@brief	

	@author	BHK
*/
void CTransformerPropDlg::OnEnChangeEditSelectedCapacity()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString rString;
	GetDlgItemText(IDC_EDIT_SELECTED_CAPACITY , rString);

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	pPropWnd->GetSelectionSet().SetValue(_T("Capacity") , _T("Selected Capacity") , rString.operator LPCSTR());
}

/**
	@brief	

	@author	BHK
*/
void CTransformerPropDlg::OnEnChangeEditFromSpec()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString rString;
	GetDlgItemText(IDC_EDIT_FROM_SPEC , rString);

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	pPropWnd->GetSelectionSet().SetValue(_T("From") , _T("Cable Spec") , rString.operator LPCSTR());
}

/**
	@brief	

	@author	BHK
*/
void CTransformerPropDlg::OnEnChangeEditToSpec()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString rString;
	GetDlgItemText(IDC_EDIT_TO_SPEC , rString);

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	pPropWnd->GetSelectionSet().SetValue(_T("To") , _T("Cable Spec") , rString.operator LPCSTR());
}

/**
	@brief	transformer의 capacity를 자동 계산하여 텍스트 출력한다.

	@author	BHK

	@date	?

	@param	

	@return
*/
int CTransformerPropDlg::SetCalculatedCapacityText()
{
	double capacity = 0.f;

	BeginWaitCursor();

	SetDlgItemText(IDC_EDIT_CALCULATED_CAPACITY , _T(""));
	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	if(pPropWnd)
	{
		CTransformerItem* pTrItem = (CTransformerItem*)pPropWnd->GetSelectionSet().GetItemAt(0);
		if(pTrItem)
		{
			capacity = pTrItem->CalculateCapacity();
			
			STRINGSTREAM_T oss;
			oss << capacity;
			SetDlgItemText(IDC_EDIT_CALCULATED_CAPACITY , oss.str().c_str());

			pTrItem->prop()->SetValue(_T("Capacity") , _T("Calculated Capacity") , oss.str());
		}
	}
	EndWaitCursor();

	return capacity;
}

struct IsMatch : public unary_function<string , bool>
{
	string m_str;
	IsMatch(const string& str) : m_str(str) {}

	bool operator()(const string& rhs) const
	{
		return (m_str == rhs);
	}
};

/**
	@brief	

	@author HumKyung	

	@date 2009-04-01 오후 7:09:45	

	@param	

	@return		
*/
BOOL CTransformerPropDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	if(pPropWnd)
	{
		const string rConnection = pPropWnd->GetSelectionSet().GetValue(_T("Capacity") , _T("Connection"));
		const int res = SetCalculatedCapacityText();
	}

	//! create bus grid control - 2011.02.07 added by HumKyung
	if(NULL == m_wndBusGrid.GetSafeHwnd())
	{
		CRect rect;
		rect.SetRectEmpty();
		if(TRUE == m_wndBusGrid.Create(rect , this , 0x101))
		{
			m_wndBusGrid.SetFixedRowCount(1);
			m_wndBusGrid.SetColumnCount(3);

			m_wndBusGrid.SetItemText(0 , 0 , _T(""));
			m_wndBusGrid.SetItemText(0 , 1 , _T("Bus Gr."));
			m_wndBusGrid.SetItemText(0 , 2 , _T("Bus"));

			CTransformerItem* pTrItem = (CTransformerItem*)pPropWnd->GetSelectionSet().GetItemAt(0);
			DisplayBusNameListOfTr(pTrItem);
		}
	}
	//!

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTransformerPropDlg::OnEnChangeEditTransformerPf()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString rString;
	GetDlgItemText(IDC_EDIT_TRANSFORMER_PF , rString);

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	pPropWnd->GetSelectionSet().SetValue(_T("General") , _T("PF") , rString.operator LPCSTR());
}

void CTransformerPropDlg::OnEnChangeEditTransformerLength()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString rString;
	GetDlgItemText(IDC_EDIT_TRANSFORMER_LENGTH , rString);

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	pPropWnd->GetSelectionSet().SetValue(_T("General") , _T("Cable Length") , rString.operator LPCSTR());
}

/**
	@brief

	@author	HumKyung

	@date	2011.02.07

	@param	nType
	@param	cx
	@param	cy

	@return
*/
void CTransformerPropDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CRect rectButton;

	CWnd* pSeperatorWnd = GetDlgItem(IDC_STATIC_SEPERATOR);

	if(pSeperatorWnd && m_wndSaveButton.GetSafeHwnd())
	{
		CRect rectSeperator;
		pSeperatorWnd->GetWindowRect(&rectSeperator);
		ScreenToClient(&rectSeperator);

		CRect rect;
		m_wndSaveButton.GetWindowRect(&rect);
		ScreenToClient(&rect);

		m_wndSaveButton.SetWindowPos(NULL , rectSeperator.right - rect.Width() , cy - rect.Height() - 5 , 0 , 0 , SWP_NOZORDER | SWP_NOSIZE);
		m_wndSaveButton.GetWindowRect(&rectButton);
		ScreenToClient(&rectButton);
	}

	if(m_wndDeleteButton.GetSafeHwnd())
	{
		CRect rect;
		m_wndDeleteButton.GetWindowRect(&rect);
		ScreenToClient(&rect);

		m_wndDeleteButton.SetWindowPos(NULL , rectButton.left - rect.Width() - 5 , cy - rect.Height() - 5 , 0 , 0 , SWP_NOZORDER | SWP_NOSIZE);
		m_wndDeleteButton.GetWindowRect(&rectButton);
		ScreenToClient(&rectButton);
	}

	if(m_wndAddButton.GetSafeHwnd())
	{
		CRect rect;
		m_wndAddButton.GetWindowRect(&rect);
		ScreenToClient(&rect);

		m_wndAddButton.SetWindowPos(NULL , rectButton.left - rect.Width() , cy - rect.Height() - 5 , 0 , 0 , SWP_NOZORDER | SWP_NOSIZE);
		m_wndAddButton.GetWindowRect(&rectButton);
		ScreenToClient(&rectButton);
	}

	if(pSeperatorWnd && m_wndBusGrid.GetSafeHwnd())
	{
		CRect rect;
		pSeperatorWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		m_wndBusGrid.SetWindowPos(NULL , rect.left , rect.bottom + 5 , rect.Width() , cy - rect.bottom - rectButton.Height() - 15 , SWP_NOZORDER);
	}

	//! 2011.02.08 added by HumKyung
	CWnd* pWnd = GetDlgItem(IDC_STATIC_TR_CAPACITY);
	if(pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		pWnd->SetWindowPos(NULL , 0 , 0 , cx - rect.left - 5 , cy - rect.top , SWP_NOZORDER | SWP_NOMOVE);
	}
}

/**
	@brief

	@author	HumKyung

	@date	2011.02.07

	@param	

	@return
*/
void CTransformerPropDlg::OnBnClickedButtonTrAdd()
{
	if(m_wndBusGrid.GetSafeHwnd())
	{
		m_wndBusGrid.SetRowCount( m_wndBusGrid.GetRowCount() + 1 );
		const int iRow = m_wndBusGrid.GetRowCount() - 1;
		m_wndBusGrid.SetCellType(iRow , 0 , RUNTIME_CLASS(CGridCellCheck));

		m_wndBusGrid.SetCellType(iRow , 1 , RUNTIME_CLASS(CGridCellBusGroupCombo));
		{
			CGridCellBusGroupCombo* pCell = (CGridCellBusGroupCombo*)(m_wndBusGrid.GetCell(iRow , 1));
			if(pCell) pCell->SetStyle(CBS_DROPDOWNLIST);
		}

		m_wndBusGrid.SetCellType(iRow , 2 , RUNTIME_CLASS(CGridCellBusCombo));
		{
			CGridCellBusCombo* pCell = (CGridCellBusCombo*)(m_wndBusGrid.GetCell(iRow , 2));
			if(pCell) pCell->SetStyle(CBS_DROPDOWNLIST);
		}

		m_wndBusGrid.AutoSizeRows();
	}
}

/**
	@brief	

	@author	HumKyung

	@date	2011.02.08

	@param	wParam
	@param	lParam
	@param	pResult

	@return	BOOL
*/
BOOL CTransformerPropDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if(wParam == m_wndBusGrid.GetDlgCtrlID())
	{
		NM_GRIDVIEW* pGridView = (NM_GRIDVIEW*)(lParam);
		if((GVN_ENDLABELEDIT == pGridView->hdr.code) && (1 == pGridView->iColumn))
		{
			CGridCellBase* pCell = m_wndBusGrid.GetCell(pGridView->iRow, pGridView->iColumn);
			if(pCell)
			{
				pCell->EndEdit();
				const CString sBusGroupName(pCell->GetText());

				CGridCellBusCombo* pCombo = (CGridCellBusCombo*)m_wndBusGrid.GetCell(pGridView->iRow, pGridView->iColumn + 1);
				if(pCombo)
				{
					pCombo->SetBusGroupName(sBusGroupName);
					m_wndBusGrid.RedrawCell(pGridView->iRow, pGridView->iColumn + 1);
				}
			}
			m_wndBusGrid.AutoSizeRow(pGridView->iRow);
		}
		return TRUE;
	}

	return CDialog::OnNotify(wParam, lParam, pResult);
}

/**
	@brief	save bus group and bus name to transformer item.

	@author	HumKyung

	@date	2011.02.08

	@return	BOOL
*/
void CTransformerPropDlg::OnBnClickedButtonTrSave()
{
	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	if(pPropWnd)
	{
		CTransformerItem* pTrItem = (CTransformerItem*)pPropWnd->GetSelectionSet().GetItemAt(0);
		if(pTrItem)
		{
			SetCapacityProperty(pTrItem);
						
			{
				CELoadDocData& docData = CELoadDocData::GetInstance();
				const string sProjectMDBFilePath = docData.GetProjectMDBFilePath();
				CADODB adoDB;
				const STRING_T sDBPath = STRING_T(PROVIDER) + _T("Data Source=") + sProjectMDBFilePath + DB_PASSWORD;
				if(TRUE == adoDB.DBConnect(sDBPath))
				{
					pTrItem->SaveData(adoDB , CTransformerItem::TableName());
				}
			}

			const int res = SetCalculatedCapacityText();

			//! OBSERVER를 통해 ITEM이 수정되었다는 것을 알린다. - 2011.02.11 added by HumKyung
			CEventData EventData(CTransformerItem::TypeString(), CEventData::MODIFY_ITEM_PROP);
			EventData.Add( pTrItem );
			pPropWnd->observer()->FireEvent(EventData);
			//!

			AfxMessageBox(_T("Save is done."));
		}
	}
}

/**
	@brief	display bus group and bus name list of transformer item.

	@author	HumKyung

	@date	2011.02.08

	@param	pTrItem

	@return	int
*/
int CTransformerPropDlg::DisplayBusNameListOfTr(CTransformerItem* pTrItem)
{
	assert(pTrItem && "pTrItem is NULL");

	if(pTrItem)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();

		const string sToBus = pTrItem->prop()->GetValue(_T("To") , _T("Bus Id"));
		const string sBuss = pTrItem->prop()->GetValue(_T("Capacity") , _T("Bus"));

		vector<string> oBusGroupList , oBusList;
		CTokenizer<CIsComma>::Tokenize(oBusList , sBuss , CIsComma());
		oBusList.erase(
			remove_if(oBusList.begin() , oBusList.end() , IsMatch(_T(""))) , oBusList.end());
///		if(!oBusList.empty()) oBusList.erase(oBusList.begin());
		
		//! remove to-bus from bus list and - 2011.02.18 added by HumKyung
		if(!sToBus.empty())
		{
			oBusList.erase(
				remove_if(oBusList.begin() , oBusList.end() , IsMatch(sToBus)) , oBusList.end());
			oBusList.insert(oBusList.begin() , sToBus);
		}

		m_wndBusGrid.SetRowCount( oBusList.size() + 1);
		const int iRowCount = m_wndBusGrid.GetRowCount();

		for(int i = 1;i < iRowCount;++i)
		{
			m_wndBusGrid.SetCellType(i , 0 , RUNTIME_CLASS(CGridCellCheck));
			
			m_wndBusGrid.SetItemText(i , 2 , oBusList[i-1].c_str());
			string sBusGroup;
			CBusItem* pBusItem = docData.FindBusItemByName(oBusList[i-1]);
			if(NULL != pBusItem) sBusGroup = pBusItem->prop()->GetValue(_T("General") , _T("Bus Group"));

			m_wndBusGrid.SetItemText(i , 1 , sBusGroup.c_str());

			m_wndBusGrid.SetCellType(i , 1 , RUNTIME_CLASS(CGridCellBusGroupCombo));
			{
				CGridCellBusGroupCombo* pCell = (CGridCellBusGroupCombo*)(m_wndBusGrid.GetCell(i , 2));
				if(pCell) pCell->SetStyle(CBS_DROPDOWNLIST);
			}

			m_wndBusGrid.SetCellType(i , 2 , RUNTIME_CLASS(CGridCellBusCombo));
			{
				CGridCellBusCombo* pCell = (CGridCellBusCombo*)(m_wndBusGrid.GetCell(i , 2));
				if(pCell) pCell->SetStyle(CBS_DROPDOWNLIST);
			}

			//! set first row's state read-only - 2011.02.24 added by HumKyung
			if(1 == i)
			{
				CGridCellBase* pCell = m_wndBusGrid.GetCell(i , 0);
				pCell->SetState(pCell->GetState() | GVIS_READONLY);
				pCell = m_wndBusGrid.GetCell(i , 1);
				pCell->SetState(pCell->GetState() | GVIS_READONLY);
				pCell = m_wndBusGrid.GetCell(i , 2);
				pCell->SetState(pCell->GetState() | GVIS_READONLY);
			}
		}
		m_wndBusGrid.AutoSizeRows();

		//! 2011.02.08 added by HumKyung
		m_wndBusGrid.SetColumnWidth(0 , 30);
		m_wndBusGrid.SetColumnWidth(1 , 130);
		m_wndBusGrid.SetColumnWidth(2 , 130);
		m_wndBusGrid.RedrawWindow();

		SetCapacityProperty(pTrItem);	//! 2011.02.18 added by HumKyung

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	delete all checked items

	@author	HumKyung

	@date	2011.02.08

	@return	void
*/
void CTransformerPropDlg::OnBnClickedButtonTrDelete()
{
	if(m_wndBusGrid.GetSafeHwnd())
	{
		if(IDYES == AfxMessageBox(_T("Check한 아이템을 삭제할까요?") , MB_YESNO|MB_ICONWARNING))
		{
			while(1)
			{
				bool bDeleted = false;
				const int iRowCount = m_wndBusGrid.GetRowCount();
				for(int i = iRowCount - 1;i > 0;--i)
				{
					CGridCellCheck* pCellCheck = (CGridCellCheck*)(m_wndBusGrid.GetCell(i , 0));
					if(pCellCheck)
					{
						if(pCellCheck->GetCheck())
						{
							m_wndBusGrid.DeleteRow(i);
							bDeleted = true;
							break;
						}
					}
				}

				if(false == bDeleted) break;
			}

			m_wndBusGrid.RedrawWindow();
		}
	}
}

/**
	@brief	set capacity property

	@author	HumKyung

	@date	2011.02.18

	@parm	

	@return	int
*/
int CTransformerPropDlg::SetCapacityProperty(CTransformerItem* pTrItem)
{
	assert(pTrItem && "pTrItem is NULL");

	if(pTrItem)
	{
		CString sBusGroupNames , sBusNames;
		const int iRowCount = m_wndBusGrid.GetRowCount();
		for(int i = 1;i < iRowCount;++i)
		{
			sBusGroupNames += m_wndBusGrid.GetItemText(i , 1) + _T(",");
			sBusNames += m_wndBusGrid.GetItemText(i , 2) + _T(",");
		}
		if(_T(",") == sBusGroupNames.Right(1)) sBusGroupNames = sBusGroupNames.Left(sBusGroupNames.GetLength() - 1);
		if(_T(",") == sBusNames.Right(1)) sBusNames = sBusNames.Left(sBusNames.GetLength() - 1);

		pTrItem->prop()->SetValue(_T("Capacity") , _T("Bus Group") , sBusGroupNames.operator LPCSTR());
		pTrItem->prop()->SetValue(_T("Capacity") , _T("Bus") , sBusNames.operator LPCSTR());
		
		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}
