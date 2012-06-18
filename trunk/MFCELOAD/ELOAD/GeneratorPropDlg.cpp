// GeneratorPropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "MainFrm.h"
#include "GeneratorPropDlg.h"
#include "ELoadDocData.h"
#include "PropertiesWnd.h"

int CGeneratorPropDlg::IDEAL_WIDTH = 580;
// CGeneratorPropDlg dialog

IMPLEMENT_DYNAMIC(CGeneratorPropDlg, CDialog)

CGeneratorPropDlg::CGeneratorPropDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGeneratorPropDlg::IDD, pParent)
{

}

CGeneratorPropDlg::~CGeneratorPropDlg()
{
}

void CGeneratorPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGeneratorPropDlg, CDialog)
	ON_REGISTERED_MESSAGE(CEventData::SELECT_ITEM , OnItemSelected)
	ON_CBN_SELENDOK(IDC_COMBO_TO_BUS_ID, &CGeneratorPropDlg::OnCbnSelendokComboToBusId)
	ON_CBN_SELENDOK(IDC_COMBO_TO_CABLE_VOLT, &CGeneratorPropDlg::OnCbnSelendokComboToCableVolt)
	ON_CBN_SELENDOK(IDC_COMBO_TO_CABLE_TYPE, &CGeneratorPropDlg::OnCbnSelendokComboToCableType)
	ON_EN_CHANGE(IDC_EDIT_TO_SPEC, &CGeneratorPropDlg::OnEnChangeEditToSpec)
	ON_WM_SIZE()
	ON_EN_CHANGE(IDC_EDIT_PG2, &CGeneratorPropDlg::OnEnChangeEditPg2)
	ON_CBN_SELENDOK(IDC_COMBO_TO_CORE, &CGeneratorPropDlg::OnCbnSelendokComboToCore)
	ON_EN_CHANGE(IDC_EDIT_PG1, &CGeneratorPropDlg::OnEnChangeEditPg1)
	ON_EN_CHANGE(IDC_EDIT_PG3, &CGeneratorPropDlg::OnEnChangeEditPg3)
	ON_EN_CHANGE(IDC_EDIT_SELECTED_VALUE, &CGeneratorPropDlg::OnEnChangeEditSelectedValue)
END_MESSAGE_MAP()


// CGeneratorPropDlg message handlers

/**
	@brief	사용자가 Item을 선택했을 때...

	@author BHK	

	@date 2009-05-08 오전 11:38:40	

	@param	

	@return		
*/
LRESULT CGeneratorPropDlg::OnItemSelected(WPARAM wParam, LPARAM lParam)
{
	CEventData* pEventData = (CEventData*)lParam;
	if(pEventData) SetEventDataValues(pEventData);
		
	return 0;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-08 오전 11:34:11	

	@param	

	@return		
*/
int CGeneratorPropDlg::SetEventDataValues( CEventData* pEventData )
{
	assert(pEventData && "pEventData is NULL");

	if(pEventData && (pEventData->GetItemCount() > 0))
	{
		BeginWaitCursor();

		//! load list의 generator item을 설정한다.
		m_wndLoadList.m_pGeneratorItem = static_cast<CGeneratorItem*>(pEventData->GetItemAt(0));

		string rValue = pEventData->m_SelectionSet.GetValue( _T("General") , _T("GENERATOR ID") );
		SetDlgItemText(IDC_EDIT_GENERATOR_ID , rValue.c_str());
		rValue = pEventData->m_SelectionSet.GetValue( _T("General") , _T("Selected Value") );
		SetDlgItemText(IDC_EDIT_SELECTED_VALUE , rValue.c_str());

		vector<CBusItem*> BusItemEntry;
		CELoadDocData& docData = CELoadDocData::GetInstance();
		docData.GetAllBusItems(BusItemEntry);
		{
			CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_TO_BUS_ID);
			if(pComboBox)
			{
				pComboBox->ResetContent();
				for(vector<CBusItem*>::iterator itr = BusItemEntry.begin();itr != BusItemEntry.end();++itr)
				{
					if((*itr)->IsDeleted()) continue;
					pComboBox->AddString((*itr)->GetName().c_str());
				}

				rValue = pEventData->m_SelectionSet.GetValue( _T("To") , _T("Bus ID"));
				const int nFound = pComboBox->FindString(-1 , rValue.c_str());
				if(-1 != nFound)
				{
					pComboBox->SetCurSel(nFound);
					OnCbnSelendokComboToBusId();
				}
			}
		}

		rValue = pEventData->m_SelectionSet.GetValue( _T("To") , _T("Bus Volt") );
		SetDlgItemText(IDC_EDIT_TO_BUS_VOLT , rValue.c_str());

		rValue = pEventData->m_SelectionSet.GetValue( _T("To") , _T("Load Volt") );
		SetDlgItemText(IDC_EDIT_TO_LOAD_VOLT , rValue.c_str());

		{
			CComboBox* pToCableVoltComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_TO_CABLE_VOLT);
			CComboBox* pToCableTypeComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_TO_CABLE_TYPE);
			CComboBox* pToCableCoreComboBox   = (CComboBox*)GetDlgItem(IDC_COMBO_TO_CORE);
			
			pToCableVoltComboBox->ResetContent();
			pToCableTypeComboBox->ResetContent();
			pToCableCoreComboBox->ResetContent();

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
								const int nFound = pToCableVoltComboBox->FindString(-1 , rCableVolt);
								if(-1 == nFound)
								{
									pToCableVoltComboBox->AddString(rCableVolt);
								}
							}
						}
					}
				}
			
				const string rToCableVolt = pEventData->m_SelectionSet.GetValue(_T("To") , _T("Cable Volt"));
				const int nFound = pToCableVoltComboBox->FindString(-1 , rToCableVolt.c_str());
				if(-1 != nFound)
				{
					pToCableVoltComboBox->SetCurSel(nFound);
					OnCbnSelendokComboToCableVolt();
				}
			}
			else
			{
				///AfxMessageBox(_T("CableData 테이블이 설정되지 않았습니다."));
				::OutputDebugString(_T("CableData 테이블이 설정되지 않았습니다."));
			}
		}

		rValue = pEventData->m_SelectionSet.GetValue( _T("To") , _T("Cable Spec") );
		SetDlgItemText(IDC_EDIT_TO_SPEC , rValue.c_str());

		rValue = pEventData->m_SelectionSet.GetValue( _T("PG1") , _T("PG1") );
		SetDlgItemText(IDC_EDIT_PG1 , rValue.c_str());

		rValue = pEventData->m_SelectionSet.GetValue( _T("PG2") , _T("PG2") );
		SetDlgItemText(IDC_EDIT_PG2 , rValue.c_str());

		rValue = pEventData->m_SelectionSet.GetValue( _T("PG3") , _T("PG3") );
		SetDlgItemText(IDC_EDIT_PG3 , rValue.c_str());
		
		m_wndPG1.SetEventDataValues( pEventData );
		m_wndPG2.SetEventDataValues( pEventData );
		m_wndPG3.SetEventDataValues( pEventData );

		EndWaitCursor();

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	BUS선택 시 Bus Volt , Load Volt를 화면에 표시합니다.
	선택한 BUS에 속한 Load들을 표시합니다.

	@author BHK	

	@date 2009-05-08 오후 12:51:19	

	@param	

	@return		
*/
void CGeneratorPropDlg::OnCbnSelendokComboToBusId()
{
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_TO_BUS_ID);
	if(pComboBox)
	{
		const int nCurSel = pComboBox->GetCurSel();
		if(-1 != nCurSel)
		{
			CString rString;
			pComboBox->GetLBText(nCurSel , rString);

			CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
			if(pPropWnd)
			{
				const string rOldBusID = pPropWnd->GetSelectionSet().GetValue(_T("To") , _T("Bus Id"));
				pPropWnd->GetSelectionSet().SetValue(_T("To") , _T("Bus Id") , rString.operator LPCSTR());
				m_wndLoadList.DisplayLoadList(rString.operator LPCSTR() , (rString == rOldBusID.c_str()));

				CELoadDocData& docData = CELoadDocData::GetInstance();
				CBusItem* pBusItem = docData.FindBusItemByName(rString.operator LPCSTR());
				if(pBusItem)
				{
					const string rBusVolt = pBusItem->prop()->GetValue(_T("General") , _T("Bus Voltage"));
					const string rLoadVolt= pBusItem->prop()->GetValue(_T("General") , _T("Load Voltage"));
					SetDlgItemText(IDC_EDIT_TO_BUS_VOLT , CString(rBusVolt.c_str()));
					SetDlgItemText(IDC_EDIT_TO_LOAD_VOLT, CString(rLoadVolt.c_str()));
					pPropWnd->GetSelectionSet().SetValue(_T("To") , _T("Bus Volt") , rBusVolt.c_str());
					pPropWnd->GetSelectionSet().SetValue(_T("To") , _T("Load Volt"), rLoadVolt.c_str());

					CalcGeneratorProp(rString.operator LPCSTR());
				}
			}
		}
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-05-08 오후 1:00:30	

	@param	

	@return		
*/
void CGeneratorPropDlg::OnCbnSelendokComboToCableVolt()
{
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_TO_CABLE_VOLT);
	const int nCurSel = pComboBox->GetCurSel();
	if(-1 == nCurSel) return;
	CString rSelCableVolt;
	pComboBox->GetLBText(nCurSel , rSelCableVolt);
	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	pPropWnd->GetSelectionSet().SetValue(_T("To") , _T("Cable Volt") , rSelCableVolt.operator LPCSTR());

	CComboBox* pToCableTypeComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_TO_CABLE_TYPE);
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

	const string rCableType = pPropWnd->GetSelectionSet().GetValue(_T("To") , _T("Cable Type"));
	const int nFound = pToCableTypeComboBox->FindString(-1 , rCableType.c_str());
	if(-1 != nFound)
	{
		pToCableTypeComboBox->SetCurSel(nFound);
	}
	else
	{
		pToCableTypeComboBox->SetWindowText(_T(""));
		pPropWnd->GetSelectionSet().SetValue(_T("To") , _T("Cable Type") , _T(""));
	}

	OnCbnSelendokComboToCableType();	
}

/**
	@brief	

	@author BHK	

	@date 2009-05-08 오후 1:03:25	

	@param	

	@return		
*/
void CGeneratorPropDlg::OnCbnSelendokComboToCableType()
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
}

/**
	@brief	

	@author BHK	

	@date 2009-05-08 오후 1:04:56	

	@param	

	@return		
*/
void CGeneratorPropDlg::OnEnChangeEditToSpec()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString rString;
	GetDlgItemText(IDC_EDIT_TO_SPEC , rString);

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	if(pPropWnd) pPropWnd->GetSelectionSet().SetValue(_T("To") , _T("Cable Spec") , rString.operator LPCSTR());
}

/**
	@brief	

	@author BHK	

	@date 2009-05-08 오후 4:37:36	

	@param	

	@return		
*/
BOOL CGeneratorPropDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rect;
	rect.SetRectEmpty();
	if (!m_wndTab.Create(CMFCTabCtrl::STYLE_3D , rect , this, 0x110))
	{
		TRACE0("Failed to create Properies Grid \n");
		return FALSE;	// fail to create
	}
	m_wndTab.SetLocation(CMFCTabCtrl::LOCATION_TOP);
	m_wndTab.ShowWindow(SW_SHOW);

	if(TRUE == m_wndLoadList.Create(CLoadListDlg::IDD , &m_wndTab))
	{
		m_wndTab.AddTab(&m_wndLoadList , _T("Load List"));
	}

	if(TRUE == m_wndPG1.Create(CPG1Dlg::IDD , &m_wndTab))
	{
		m_wndTab.AddTab(&m_wndPG1 , _T("PG1"));
	}
	
	if(TRUE == m_wndPG2.Create(CPG2Dlg::IDD , &m_wndTab))
	{
		m_wndTab.AddTab(&m_wndPG2 , _T("PG2"));
	}

	if(TRUE == m_wndPG3.Create(CPG3Dlg::IDD , &m_wndTab))
	{
		m_wndTab.AddTab(&m_wndPG3 , _T("PG3"));
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	

	@author BHK	

	@date 2009-05-08 오후 4:46:07	

	@param	

	@return		
*/
void CGeneratorPropDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if(m_wndTab.GetSafeHwnd())
	{
		m_wndTab.SetWindowPos(&(CWnd::wndTop) , 0 , 280 , cx , cy - 280 , SWP_NOACTIVATE);
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-05-08 오후 8:31:33	

	@param	

	@return		
*/
void CGeneratorPropDlg::OnEnChangeEditPg2()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString rString;
	m_wndPG2.GetDlgItemText(IDC_EDIT_PLmax , rString);
	m_wndPG3.SetDlgItemText(IDC_EDIT_QLMAX , rString);

	GetDlgItemText(IDC_EDIT_PG2 , rString);
	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	if(pPropWnd) pPropWnd->GetSelectionSet().SetValue(_T("PG2") , _T("PG2") , rString.operator LPCSTR());
}

/**
	@brief	

	@author BHK	

	@date 2009-05-09 오전 11:09:12	

	@param	

	@return		
**/
void CGeneratorPropDlg::OnCbnSelendokComboToCore()
{
	CComboBox* pCableCoreComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_TO_CORE);
	if(pCableCoreComboBox)
	{
		const int nCableCoreCurSel = pCableCoreComboBox->GetCurSel();
		if(-1 != nCableCoreCurSel)
		{
			CString rCableCore;
			pCableCoreComboBox->GetLBText(nCableCoreCurSel , rCableCore);
			
			CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
			pPropWnd->GetSelectionSet().SetValue(_T("To") , _T("Cable Core") , rCableCore.operator LPCSTR());
		}
		else
		{
			pCableCoreComboBox->SetWindowText(_T(""));

			CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
			pPropWnd->GetSelectionSet().SetValue(_T("To") , _T("Cable Core") , _T(""));
		}
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-05-09 오전 11:19:15	

	@param	

	@return		
**/
void CGeneratorPropDlg::OnEnChangeEditPg1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString rString;
	GetDlgItemText(IDC_EDIT_PG1 , rString);

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	if(pPropWnd) pPropWnd->GetSelectionSet().SetValue(_T("PG1") , _T("PG1") , rString.operator LPCSTR());
}

/**
	@brief	

	@author BHK	

	@date 2009-05-09 오전 11:19:40	

	@param	

	@return		
**/
void CGeneratorPropDlg::OnEnChangeEditPg3()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString rString;
	GetDlgItemText(IDC_EDIT_PG3 , rString);

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	if(pPropWnd) pPropWnd->GetSelectionSet().SetValue(_T("PG3") , _T("PG3") , rString.operator LPCSTR());
}

/**
	@brief	

	@author BHK	

	@date 2009-05-27 오후 8:08:41	

	@param	

	@return		
*/
int CGeneratorPropDlg::CalcGeneratorProp(const string& rBusID)
{
	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();

	CELoadDocData& docData = CELoadDocData::GetInstance();
	CBusItem* pBusItem = docData.FindBusItemByName(rBusID);
	if(pBusItem && pPropWnd)
	{
		vector<string> LoadItemEntry;
		const string& rSelectedLoadList = pPropWnd->GetSelectionSet().GetValue(_T("General") , _T("Selected Load List"));
		if(!rSelectedLoadList.empty()) CTokenizer<CIsComma>::Tokenize(LoadItemEntry , rSelectedLoadList , CIsComma());
		
		double nKW = 0.f , nKVAR = 0.f;
		pBusItem->GetLoadSummaryResultData(nKW , nKVAR , &LoadItemEntry);
		ostringstream oss;
		oss << nKW;
		m_wndPG1.SetDlgItemText(IDC_EDIT_PG1_WL , oss.str().c_str());

		CItemSelectionSet SelectionSet;
		docData.GetLoadItemSetInBus(SelectionSet , rBusID);

		string rMaxKVAMotorID;
		double nMaxKVA = 0.f , nMaxKW = 0.f , nSumKW = 0.f;
		for(int i = 0;i < SelectionSet.GetItemCount();++i)
		{
			CLoadItem* pLoadItem = static_cast<CLoadItem*>(SelectionSet.GetItemAt(i));
			vector<string>::iterator where = find(LoadItemEntry.begin() , LoadItemEntry.end() , SelectionSet.GetItemAt(i)->GetName());
			if(where != LoadItemEntry.end())
			{
				const string rKVA = SelectionSet.GetItemAt(i)->prop()->GetValue(_T("Load Summary") , _T("KVA"));
				if(nMaxKVA < atof(rKVA.c_str()))
				{
					rMaxKVAMotorID = where->c_str();
					nMaxKVA = atof(rKVA.c_str());
				}

				double nRatingCapacity = 0.f;
				pLoadItem->CalcRatingCapacity(nRatingCapacity);
				if(nMaxKW < nRatingCapacity) nMaxKW = nRatingCapacity;
				nSumKW += nKW;
			}
		}
		oss.str("");
		oss << nMaxKVA;
		m_wndPG2.SetDlgItemText(IDC_EDIT_MAX_STARTING_CAPACITY , oss.str().c_str());
		m_wndPG2.DisplayPLmaxEquation(rMaxKVAMotorID);

		double nBusKW = 0.f , nBusKVAR = 0.f;
		pBusItem->GetLoadSummaryResultData(nBusKW , nBusKVAR);
		oss.str(""); oss << nBusKW;
		m_wndPG3.SetDlgItemText(IDC_EDIT_SUM_KW, oss.str().c_str());
		oss.str(""); oss << nMaxKW;
		m_wndPG3.SetDlgItemText(IDC_EDIT_MAX_KW, oss.str().c_str());
		oss.str(""); oss << (nBusKW - nMaxKW);
		m_wndPG3.SetDlgItemText(IDC_EDIT_W0, oss.str().c_str());

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-28 오전 12:22:56	

	@param	

	@return		
**/
void CGeneratorPropDlg::OnEnChangeEditSelectedValue()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString rString;
	GetDlgItemText(IDC_EDIT_SELECTED_VALUE , rString);

	CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent();
	if(pPropWnd) pPropWnd->GetSelectionSet().SetValue(_T("General") , _T("Selected Value") , rString.operator LPCSTR());
}
