// CableTrayFillRateCalculationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "CableTrayFillRateCalculationDlg.h"
#include "ELoadDocData.h"
#include "TrayItem.h"

#define HV_POWER_CABLE "HV POWER CABLE"
#define LV_POWER_CABLE "LV POWER CABLE"
#define CONTROL_CABLE  "CONTROL CABLE"

// CCableTrayFillRateCalculationDlg dialog
/*
struct MatchRouteType : public binary_function<CELoadItem*, string, bool>
{
        bool operator()(const CELoadItem* pItem , const string& rhs) const
	{
		return (rhs == pItem->prop()->GetValue(_T("General"), _T("Route ID")));
		
	}
};
*/

struct MatchRouteTypeInRouteGr : public binary_function<CELOADTable::Record, string, bool>
{
        bool operator()(const CELOADTable::Record lhs , const string& rhs) const
	{
		const map<string,string>::const_iterator where = lhs.FieldValueMap.find(_T("C_ROUTE_TYPE"));
		if(where != lhs.FieldValueMap.end())
		{
			return (rhs == where->second);
		}

		return false;
	}
};


struct MatchParent : public binary_function<CELoadItem*, string, bool>
{
        bool operator()(const CELoadItem* pItem , const string& rhs) const
	{
		return (rhs == pItem->prop()->GetValue(_T("General"), _T("Parent")));
		
	}
};

IMPLEMENT_DYNAMIC(CCableTrayFillRateCalculationDlg, CDialog)

CCableTrayFillRateCalculationDlg::CCableTrayFillRateCalculationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCableTrayFillRateCalculationDlg::IDD, pParent)
	, m_nCableTraySystem(0)
	, m_nDuctBankSystemRadio(0)
	, m_nCableTrayDuctFillingRadio(0)
{

}

CCableTrayFillRateCalculationDlg::~CCableTrayFillRateCalculationDlg()
{
}

void CCableTrayFillRateCalculationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CABLETRAYSYSTEM_STATIC, m_wndCableTraySystemStatic);
	DDX_Control(pDX, IDC_DUCTBANKSYSTEM_STATIC, m_wndDuctBankSystemStatic);
	DDX_Control(pDX, IDC_CABLETRAY_DUCTFILLING_STATIC, m_wndCableTrayDuctFillingStatic);
	DDX_Control(pDX, IDC_CALC_BUTTON, m_wndCalcButton);
	DDX_Control(pDX, ID_SAVE, m_wndSaveButton);
	DDX_Control(pDX, IDCANCEL, m_wndCancelButton);
	DDX_Radio(pDX, IDC_CABLE_TRAY_SYSTEM_OPTION_1_RADIO, m_nCableTraySystem);
	DDV_MinMaxInt(pDX, m_nCableTraySystem, 0, 2);
	DDX_Radio(pDX, IDC_DUCT_BANK_SYSTEM_OPTION_1_RADIO, m_nDuctBankSystemRadio);
	DDV_MinMaxInt(pDX, m_nDuctBankSystemRadio, 0, 1);
	DDX_Radio(pDX, IDC_CABLETRAY_DUCTFILLING_OPTION_1_RADIO, m_nCableTrayDuctFillingRadio);
	DDV_MinMaxInt(pDX, m_nCableTrayDuctFillingRadio, 0, 1);
	DDX_Control(pDX, IDC_CONTORL_CABLE_TRAY_COMBO, m_wndControlCableTrayLayerCombo);
	DDX_Control(pDX, IDC_CONTROL_CABLE_COMBO, m_wndControlCableLayerCombo);
	DDX_Control(pDX, IDC_DUCTBANKFILLRATE_PERCENTAGE_EDIT, m_wndDuctBankFillRatePercentageEdit);
}


BEGIN_MESSAGE_MAP(CCableTrayFillRateCalculationDlg, CDialog)
	ON_BN_CLICKED(IDC_CALC_BUTTON, &CCableTrayFillRateCalculationDlg::OnBnClickedCalcButton)
	ON_BN_CLICKED(ID_SAVE, &CCableTrayFillRateCalculationDlg::OnBnClickedSave)
	ON_BN_CLICKED(IDCANCEL, &CCableTrayFillRateCalculationDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CCableTrayFillRateCalculationDlg message handlers

BOOL CCableTrayFillRateCalculationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	InitEditBox();
	InitComboBox();

	DisplayData();

	//! LABEL SETTING
	m_wndCableTraySystemStatic.SetBkColor(DarkenColor(GetSysColor(COLOR_BTNFACE),0.15) , GetSysColor(COLOR_BTNFACE) , CLabelStatic::HorzGradient);
	m_wndDuctBankSystemStatic.SetBkColor(DarkenColor(GetSysColor(COLOR_BTNFACE),0.15) , GetSysColor(COLOR_BTNFACE) , CLabelStatic::HorzGradient);
	m_wndCableTrayDuctFillingStatic.SetBkColor(DarkenColor(GetSysColor(COLOR_BTNFACE),0.15) , GetSysColor(COLOR_BTNFACE) , CLabelStatic::HorzGradient);

	//!Button Setting
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));

	m_wndSaveButton.SetTextHotColor(TextHotColor);
	m_wndCalcButton.SetTextHotColor(TextHotColor);
	m_wndCancelButton.SetTextHotColor(TextHotColor);

	m_wndSaveButton.SetIcon(IDI_OK_18X18);
	//m_wndCalcButton.SetIcon(IDI_CALC);
	m_wndCancelButton.SetIcon(IDI_CANCEL_18X18);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	CABLE TRAY CALCULATION

	@author KHS	

	@return		
**/
void CCableTrayFillRateCalculationDlg::OnBnClickedCalcButton()
{
	BeginWaitCursor();
	if(ERROR_SUCCESS == CalcCableTrayFillRate())
	{
		//!CABLE SIZING RESULT VIEW를 생성
		CELOADApp* pApp = (CELOADApp*)AfxGetApp();
		if(pApp)
		{
			pApp->OpenNewDocumentIfNeed(_T("ROUTE CABLE LIST") , _T("ROUTE CABLE LIST"));

		}
		::MessageBox(NULL, "SUCCESS !", "CALCAULATION", MB_OK|MB_ICONINFORMATION);
		EndWaitCursor();
		OnOK();
		return;
	}
	EndWaitCursor();
}

/**
	@brief	SAVE 

	@author KHS	

	@return		
**/
void CCableTrayFillRateCalculationDlg::OnBnClickedSave()
{
	// TODO: Add your control notification handler code here

	if(ERROR_SUCCESS == SaveData())
	{
		CELOADApp* pApp = (CELOADApp*)AfxGetApp();
		if(pApp) pApp->OnSaveDataBase();
	}
}


/**
	@brief	CANCEL

	@author KHS	

	@return		
**/
void CCableTrayFillRateCalculationDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

/**
	@brief	INIT EDIT BOX 

	@author KHS	

	@return		
**/
int CCableTrayFillRateCalculationDlg::InitEditBox(void)
{
	try
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();

		static const CString rToolTip(_T("데이터 형식이 일치하지 않습니다."));
		const COLORREF WavyLineColor = docData.GetColorCode("FILTER_EDIT_WAVY_LINE_COLOR");
		const COLORREF BackFocusColor= docData.GetColorCode("FILTER_EDIT_BACK_FOCUS_COLOR");
		const COLORREF TextErrorColor= docData.GetColorCode("FILTER_EDIT_TEXT_ERROR_COLOR");
		const COLORREF BackErrorColor= docData.GetColorCode("FILTER_EDIT_BACK_ERROR_COLOR");

		const string rNumberRegEx = docData.GetRegExp("NUMBER");

		//! ROUTE GROUP
		m_wndDuctBankFillRatePercentageEdit.SetRegEx(rNumberRegEx.c_str());
		m_wndDuctBankFillRatePercentageEdit.CreateToolTip(this , rToolTip);
		m_wndDuctBankFillRatePercentageEdit.SetWavyLineColour (WavyLineColor);
		m_wndDuctBankFillRatePercentageEdit.SetBackgroundColourFocus (BackFocusColor);
		m_wndDuctBankFillRatePercentageEdit.SetForegroundColourError (TextErrorColor);
		m_wndDuctBankFillRatePercentageEdit.SetBackgroundColourError (BackErrorColor);

	}
	catch(...)
	{
		return ERROR_BAD_ENVIRONMENT;
	}
	return ERROR_SUCCESS;
}

/**
	@brief	

	@author KHS	

	@return		
**/
BOOL CCableTrayFillRateCalculationDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE) return TRUE;
		if(pMsg->wParam == VK_RETURN) return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

/**
	@brief	SAVE DATA

	@author KHS	

	@date 2009-05-07 오후 4:35:49	

	@param	

	@return		
**/
int CCableTrayFillRateCalculationDlg::SaveData(void)
{
	UpdateData(TRUE);

	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pTable = docData.GetTableOf(_T("T_CABLETRAYFILLRATE_SETTING"));
	if(pTable)
	{
		CString rCableTraySystem, rDuctBankSystem, rCableTrayDuctFilling, rControlCableTrayLayer, rControlCableLayer, rDuctBankFillRatePercentage;

		rCableTraySystem.Format("%d", m_nCableTraySystem);
		rDuctBankSystem.Format("%d", m_nDuctBankSystemRadio);
		rCableTrayDuctFilling.Format("%d", m_nCableTrayDuctFillingRadio);

		int nItem = m_wndControlCableTrayLayerCombo.GetCurSel();
		if(-1 != nItem)
		{
			m_wndControlCableTrayLayerCombo.GetLBText(nItem, rControlCableTrayLayer);
		}

		nItem = m_wndControlCableLayerCombo.GetCurSel();
		if( -1 != nItem)
		{
			m_wndControlCableLayerCombo.GetLBText(nItem, rControlCableLayer);
		}

		m_wndDuctBankFillRatePercentageEdit.GetWindowText(rDuctBankFillRatePercentage);

		if(pTable->m_RecordEntry.empty())
		{
			CELOADTable::Record record;
			{
				record.FieldValueMap[_T("C_CABLE_TRAY_SYSTEM")]		= rCableTraySystem.operator LPCSTR();
				record.FieldValueMap[_T("C_CONTROL_CABLE_TRAY_LAYER")]	= rControlCableTrayLayer.operator LPCSTR();
				record.FieldValueMap[_T("C_CONTROL_CABLE_LAYER")]		= rControlCableLayer.operator LPCSTR();
				record.FieldValueMap[_T("C_DUCT_BANK_SYSTEM")]		= rDuctBankSystem.operator LPCSTR();
				record.FieldValueMap[_T("C_DUCT_BANK_FILL_RATE_PERCENTAGE")] = rDuctBankFillRatePercentage.operator LPCSTR();
				record.FieldValueMap[_T("C_CABLE_TRAY_DUCT_FILLING")]	= rCableTrayDuctFilling.operator LPCSTR();
			}
			pTable->m_RecordEntry.push_back(record);

			pTable->Save();
		}
		else
		{
			vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin();
			{
				itr->FieldValueMap[_T("C_CABLE_TRAY_SYSTEM")]		= rCableTraySystem.operator LPCSTR();
				itr->FieldValueMap[_T("C_CONTROL_CABLE_TRAY_LAYER")]	= rControlCableTrayLayer.operator LPCSTR();
				itr->FieldValueMap[_T("C_CONTROL_CABLE_LAYER")]		= rControlCableLayer.operator LPCSTR();
				itr->FieldValueMap[_T("C_DUCT_BANK_SYSTEM")]		= rDuctBankSystem.operator LPCSTR();
				itr->FieldValueMap[_T("C_DUCT_BANK_FILL_RATE_PERCENTAGE")] = rDuctBankFillRatePercentage.operator LPCSTR();
				itr->FieldValueMap[_T("C_CABLE_TRAY_DUCT_FILLING")]	= rCableTrayDuctFilling.operator LPCSTR();
			}
			pTable->Save();
		}
		
		//!LOAD, CABLE, TRAY, ROUTE
		/*if(ERROR_SUCCESS != docData.Save(CLoadItem::TableName(), CLoadItem::TypeString(), AfxGetMainWnd()))
		{
			return ERROR_BAD_ENVIRONMENT;
		}
		if(ERROR_SUCCESS != docData.Save(CTrayItem::TableName(), CTrayItem::TypeString(), AfxGetMainWnd()))
		{
			return ERROR_BAD_ENVIRONMENT;
		}
		if(ERROR_SUCCESS != docData.Save(CRouteItem::TableName(), CRouteItem::TypeString(), AfxGetMainWnd()))
		{
			return ERROR_BAD_ENVIRONMENT;
		}
		if(ERROR_SUCCESS != docData.Save(CCableItem::TableName(), CCableItem::TypeString(), AfxGetMainWnd()))
		{
			return ERROR_BAD_ENVIRONMENT;
		}*/

	}
	return ERROR_SUCCESS;
}

/**
	@brief	DISPLAY CABLE TRAY FILL RATE DATA

	@author KHS	

	@return		
**/
int CCableTrayFillRateCalculationDlg::DisplayData(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pTable = docData.GetTableOf(_T("T_CABLETRAYFILLRATE_SETTING"));
	if(pTable)
	{
		CString rCableTraySystem, rDuctBankSystem, rCableTrayDuctFilling, rControlCableTrayLayer, rControlCableLayer, rDuctBankFillRatePercentage;
		if(!pTable->m_RecordEntry.empty())
		{
			vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin();
			{
				const CString rCableTraySystem = itr->FieldValueMap[_T("C_CABLE_TRAY_SYSTEM")].c_str();
				const CString rControlCableTrayLayer = itr->FieldValueMap[_T("C_CONTROL_CABLE_TRAY_LAYER")].c_str();
				const CString rControlCableLayer = itr->FieldValueMap[_T("C_CONTROL_CABLE_LAYER")].c_str();
				const CString rDuctBankSystem = itr->FieldValueMap[_T("C_DUCT_BANK_SYSTEM")].c_str();
				const CString rDuctBankFillRatePercentage = itr->FieldValueMap[_T("C_DUCT_BANK_FILL_RATE_PERCENTAGE")].c_str();
				const CString rCableTrayDuctFilling = itr->FieldValueMap[_T("C_CABLE_TRAY_DUCT_FILLING")].c_str();
				
				m_nCableTraySystem = atoi(rCableTraySystem);
				m_nDuctBankSystemRadio = atoi(rDuctBankSystem);
				m_nCableTrayDuctFillingRadio = atoi(rCableTrayDuctFilling);

				int nItem = m_wndControlCableTrayLayerCombo.FindString(-1, rControlCableTrayLayer);
				if(-1 != nItem)
				{
					m_wndControlCableTrayLayerCombo.SetCurSel(nItem);
				}

				nItem = m_wndControlCableLayerCombo.FindString(-1, rControlCableLayer);
				if(-1 != nItem)
				{
					m_wndControlCableLayerCombo.SetCurSel(nItem);
				}

				m_wndDuctBankFillRatePercentageEdit.SetWindowText(rDuctBankFillRatePercentage);
			}
		}

		UpdateData(FALSE);

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	ComboBox INIT

	@author KHS	

	@date 2009-05-15 오후 3:05:33	

	@param	

	@return		
**/
int CCableTrayFillRateCalculationDlg::InitComboBox(void)
{
	const CString __COMBO__[] = {_T("3"), _T("2"), _T("1")};
	
	const int nCount = sizeof(__COMBO__) / sizeof( __COMBO__[0]);
	for(int i =0; i < nCount;i++)
	{
		m_wndControlCableTrayLayerCombo.AddString(__COMBO__[i]);
		m_wndControlCableLayerCombo.AddString(__COMBO__[i]);
	}
	
	m_wndControlCableTrayLayerCombo.SetCurSel(0);
	m_wndControlCableLayerCombo.SetCurSel(0);

	return ERROR_SUCCESS;
}

/**
	@brief	CALCULATION CABLE TRAY FILL RATE

	@author KHS	

	@date 2009-05-18 오후 3:31:58	

	@param	

	@return		
**/
int CCableTrayFillRateCalculationDlg::CalcCableTrayFillRate(void)
{
	//!새로 TRAY 생성하기 전에 CABLE에 TRAY LIST를 초기화 한다.
	DeleteAllTrayListInCableItem();
	
	//!새로 TRAY 생성하기 전에 기존의 TRAY를 모두 제거
	DeleteAllTrayItem();
	
	//! 기존 Route 에 쌓인 Cable을 삭제, Tray도 삭제
	DeleteCableItemInRoute();

	UpdateData(TRUE);
	
	int nHVPowerCableLayer = 1;
	int nLVPowerCableLayer = 1;
	int nControlCableLayer = 1;
	
	CString rLayer;
	if(1 == m_nCableTraySystem)
	{
		int nItem = m_wndControlCableTrayLayerCombo.GetCurSel();
		if(nItem != -1)
		{
			m_wndControlCableTrayLayerCombo.GetLBText(nItem, rLayer);
			nControlCableLayer = atoi(rLayer);
		}
	}
	else if(2 == m_nCableTraySystem)
	{
		int nItem = m_wndControlCableLayerCombo.GetCurSel();
		if(nItem != -1)
		{
			m_wndControlCableLayerCombo.GetLBText(nItem, rLayer);
			nControlCableLayer = atoi(rLayer);
		}
	}

	CELoadDocData& docData = CELoadDocData::GetInstance();

	//vector<CRouteItem*> Last_Route_entry;
	list<CELoadItem*> LoadItemList;
	docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());

	list<CELoadItem*> CableItemList;
	docData.GetELoadItemListOf(CableItemList , CCableItem::TypeString());

	list<CELoadItem*> RouteItemList;
	docData.GetELoadItemListOf(RouteItemList , CRouteItem::TypeString());

	CELOADTable* pTable = docData.GetTableOf(_T("T_ROUTE_GROUP"));
	if(pTable)
	{
		for(list<CELoadItem*>::iterator itr = LoadItemList.begin(); itr != LoadItemList.end();++itr)
		{
			const string rLoadId = (*itr)->GetName();
			const string rLoadRouteGroup = (*itr)->prop()->GetValue(_T("Cable Route"), _T("Route Group"));
			if(!rLoadRouteGroup.empty())
			{
				//! Get EndRouteItem of RouteGroup
				CRouteItem* pEndRouteItem = FindEndRouteItemOfRouteGroup(rLoadRouteGroup);
				if(pEndRouteItem)
				{
					/*vector<CRouteItem*>::iterator where = find(Last_Route_entry.begin(),  Last_Route_entry.end(), pEndRouteItem);
					if(where == Last_Route_entry.end()) Last_Route_entry.push_back(pEndRouteItem);*/

					for(list<CELoadItem*>::iterator jtr = CableItemList.begin(); jtr!= CableItemList.end();++jtr)
					{
						const string rCableLoadId = (*jtr)->prop()->GetValue(_T("General"), _T("Load Id"));
						if(rLoadId == rCableLoadId)
						{
							CCableItem * pCableItem = static_cast<CCableItem*>(*jtr);
							if(pCableItem)
							{
								const string  rCableCategory = pCableItem->prop()->GetValue(_T("General"), _T("Cable Category"));
								if(_T("Power Cable") == rCableCategory)
								{
									const CString  rCableVoltType = pCableItem->prop()->GetValue(_T("General"), _T("Volt Type")).c_str();
									if(rCableVoltType.Find("HV") != -1)
									{
										pEndRouteItem->AddHVPowerCable(pCableItem);
									}
									else if(rCableVoltType.Find("LV") != -1)
									{
										pEndRouteItem->AddLVPowerCable(pCableItem);
									}
								}
								else if("Control Cable" == rCableCategory)
								{
									//!DCS TYPE은 제거
									if(_T("DCS") == pCableItem->prop()->GetValue(_T("General") , _T("To")))
									{
										continue;
									}
									pEndRouteItem->AddControlCable(pCableItem);
								}
							}
						}
					}
					//!말단 Route Summary
					pEndRouteItem->SetSummaryFlag(true);

					//!Cable 별로 정렬
					pEndRouteItem->SortHVPowerCableByLength();
					pEndRouteItem->SortLVPowerCableByLength();
					pEndRouteItem->SortControlCableByLength();
				}
			}
		}
		
		//! ROOT 부터 Cable을 쌓는다.
		for(list<CELoadItem*>::iterator itr = RouteItemList.begin(); itr != RouteItemList.end();++itr)
		{
			CRouteItem * pRouteItem = static_cast<CRouteItem*>(*itr);
			if(pRouteItem)
			{
				if(!pRouteItem->IsSummary())
				{
					const string rParent = pRouteItem->prop()->GetValue(_T("General"), _T("Parent"));
					if(rParent == "ROOT")
					{
						const string rRouteType = pRouteItem->GetName();
						FillRouteCable(rRouteType);

					}
				}
			}
		}


		//!TRAY 생성
		int nTrayCount = 0;
		int nTraySize = 900;
		for(list<CELoadItem*>::iterator itr = RouteItemList.begin(); itr != RouteItemList.end();++itr)
		{
			CRouteItem *pRouteItem = static_cast<CRouteItem*>(*itr);
			if(pRouteItem)
			{
				if(!pRouteItem->m_HV_PowerCableEntry.empty())
				{
					//! HV POWER CABLE
					CreateTrayItem(pRouteItem->m_HV_PowerCableEntry, HV_POWER_CABLE , nHVPowerCableLayer, nTrayCount, pRouteItem);
				}
				
				if(!pRouteItem->m_LV_PowerCableEntry.empty())
				{
					//!LV POWER CABLE
					CreateTrayItem(pRouteItem->m_LV_PowerCableEntry, LV_POWER_CABLE , nLVPowerCableLayer, nTrayCount, pRouteItem);
				}
				
				if(!pRouteItem->m_ControlCableEntry.empty())
				{
					//! CONTROL CABLE
					CreateTrayItem(pRouteItem->m_ControlCableEntry, CONTROL_CABLE, nControlCableLayer, nTrayCount, pRouteItem);
				}
			}
		}
		TxtFileExport();
		

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}


/**
	@brief	CATEGORY별 CABLEITEM으로 TRAY 생성

	@author KHS	

	@date 2009-05-20 오후 1:07:56	

	@param	

	@return		
**/
int CCableTrayFillRateCalculationDlg::CreateTrayItem(vector<CCableItem*> CableItemEntry, const string& rCategory, const int& nLayer, int& nTrayCount, CRouteItem *pRouteItem)
{
	ASSERT(pRouteItem && "pRouteItem is NULL");
	
	const string rRouteName = pRouteItem->GetName();
	CTrayItem *pTrayItem = NULL;
	double nTotalCableSize = 0.f;
	
	CELoadDocData& docData = CELoadDocData::GetInstance();
	for(vector<CCableItem*>::iterator jtr = CableItemEntry.begin(); jtr != CableItemEntry.end();++jtr)
	{
		CCableItem *pCableItem = static_cast<CCableItem*>(*jtr);
		if(pCableItem)
		{
			CString rCableSize = pCableItem->prop()->GetValue(_T("General"), _T("Size")).c_str();
			
			double nCableSize = atof(rCableSize);

			if((nTotalCableSize == 0.f) || ((nTotalCableSize + nCableSize) > (900 * nLayer)))
			{

				CELoadItem * pItem = static_cast<CELoadItem*>(docData.CreateELoadItemOf(CTrayItem::TypeString()));
				if(pItem)
				{
					pTrayItem = (CTrayItem*)(pItem);

					nTrayCount++;
					CString rTrayName;
					rTrayName.Format("TRAY%d", nTrayCount);
					pTrayItem->prop()->SetValue(_T("General"), _T("Tray ID"), rTrayName.operator LPCSTR());
					pTrayItem->prop()->SetValue(_T("General"), _T("Route ID"), rRouteName);

					pTrayItem->SetRouteItemPtr(pRouteItem);
					pRouteItem->AddTrayItem(pTrayItem);

					nTotalCableSize = 0.f;
				}
			}
			nTotalCableSize += nCableSize;

			CString rSize;
			rSize.Format("%.2f", nTotalCableSize);
			pTrayItem->prop()->SetValue(_T("General"), _T("Size"), rSize.operator LPCSTR());
			pTrayItem->AddCableItem(pCableItem);

			string rTrayList = pCableItem->prop()->GetValue(_T("Tray"), _T("Tray List"));

			if(!rTrayList.empty()) rTrayList+= ",";

			rTrayList += pTrayItem->GetName();
			(*jtr)->prop()->SetValue(_T("Tray"), _T("Tray List"), rTrayList);
			(*jtr)->prop()->SetOriginalValue(_T("Tray"), _T("Tray List"), rTrayList);
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	Tray 생성하기 전에 기존의 CABLE의 TRAY들을 제거

	@author KHS	

	@date 2009-05-25 오후 1:20:25	

	@param	

	@return		
**/
int CCableTrayFillRateCalculationDlg::DeleteAllTrayListInCableItem(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	list<CELoadItem*> CableItemList;
	docData.GetELoadItemListOf(CableItemList , CCableItem::TypeString());
	//if(pCableItemList)
	{
		for(list<CELoadItem*>::iterator itr = CableItemList.begin(); itr != CableItemList.end();++itr)
		{
			const string rTrayList = (*itr)->prop()->GetValue(_T("Tray"), _T("Tray List"));
			if(rTrayList.empty()) continue;

			(*itr)->prop()->SetValue(_T("Tray"), _T("Tray List"), _T(""));
			(*itr)->prop()->SetOriginalValue(_T("Tray"), _T("Tray List"), _T(""));
		}
		
		return ERROR_SUCCESS;
	}
	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	새로운 TRAY를 생성하기 전에 기존의 TRAY를 모두 삭제

	@author KHS	

	@date 2009-05-25 오후 1:27:55	

	@param	

	@return		
**/
int CCableTrayFillRateCalculationDlg::DeleteAllTrayItem(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*>  TrayItemList;
	docData.GetELoadItemListOf(TrayItemList , CTrayItem::TypeString());
	//if(pTrayItemList)
	{
		for(list<CELoadItem*>::iterator itr = TrayItemList.begin(); itr != TrayItemList.end();++itr)
		{
			(*itr)->SetDeleteFlag(true);
		}

		return ERROR_SUCCESS;
	}
	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	Route에 쌓인 모든 Cable Item을 제거

	@author KHS	

	@date 2009-05-26 오전 11:07:19	

	@param	

	@return		
**/
int CCableTrayFillRateCalculationDlg::DeleteCableItemInRoute(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> RouteItemList;
	docData.GetELoadItemListOf(RouteItemList , CRouteItem::TypeString());
	//if(pRouteItemList)
	{
		for(list<CELoadItem*>::iterator itr = RouteItemList.begin(); itr != RouteItemList.end();++itr)
		{
			CRouteItem *pRouteItem = static_cast<CRouteItem*>(*itr);
			if(pRouteItem)
			{
				pRouteItem->m_HV_PowerCableEntry.clear();
				pRouteItem->m_LV_PowerCableEntry.clear();
				pRouteItem->m_ControlCableEntry.clear();

				pRouteItem->m_TrayItemEntry.clear();
				pRouteItem->SetSummaryFlag(false);
			}
		}
		return ERROR_SUCCESS;
	}
	return ERROR_BAD_ENVIRONMENT;;
}



/**
	@brief RouteItem이 Gruop의 말단인지 판별

	@author KHS	

	@date 2009-05-28 오전 10:13:01	

	@param	

	@return		
**/
bool CCableTrayFillRateCalculationDlg::IsLastRouteType(const string& rRouteGroup, const string& rLastRouteType)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CELOADTable* pTable = docData.GetTableOf(_T("T_ROUTE_GROUP"));
	if(pTable)
	{
		for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin();itr != pTable->m_RecordEntry.end();++itr)
		{
			if((rRouteGroup == itr->FieldValueMap[_T("C_ROUTE_GROUP")]) && (rLastRouteType == itr->FieldValueMap[_T("C_ROUTE_TYPE")]))
			{
				return true;
			}
		}
	}

	return false;
}

/**
	@brief	Route Group의 말단 Route를 찾는다.

	@author KHS	

	@date 2009-05-28 오후 12:11:12	

	@param	

	@return	CRouteItem
**/
CRouteItem* CCableTrayFillRateCalculationDlg::FindEndRouteItemOfRouteGroup(const string& rRouteGroup)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> RouteItemList;
	docData.GetELoadItemListOf(RouteItemList , CRouteItem::TypeString());
	CELOADTable* pTable = docData.GetTableOf(_T("T_ROUTE_GROUP"));
	if(pTable)
	{
		vector<CRouteItem*> SelectedRouteItemEntry;
		for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin(); itr != pTable->m_RecordEntry.end();++itr)
		{
			const string rFindRouteGroup = itr->FieldValueMap[_T("C_ROUTE_GROUP")];
			if(rFindRouteGroup == rRouteGroup)
			{
				const string rRouteType = itr->FieldValueMap[_T("C_ROUTE_TYPE")];

				CRouteItem *pRouteItem = docData.FindRouteItemByName(rRouteType);
				if(pRouteItem)
				{
					SelectedRouteItemEntry.push_back(pRouteItem);
				}
			}
		}

		for(vector<CRouteItem*>::iterator itr = SelectedRouteItemEntry.begin(); itr != SelectedRouteItemEntry.end();++itr)
		{
			const string rRouteType = (*itr)->GetName();
			vector<CRouteItem*>::iterator where = find_if(SelectedRouteItemEntry.begin(), SelectedRouteItemEntry.end(), bind2nd(MatchParent(), rRouteType));
			if(where == SelectedRouteItemEntry.end())
			{
				return (*itr);
			}
		}
	}

	return NULL;
}


/**
	@brief	Route type 에 Cable을 쌓는다.

	@author KHS	

	@date 2009-05-28 오후 5:39:06	

	@param	

	@return		
**/
int CCableTrayFillRateCalculationDlg::FillRouteCable(const string & rRouteType)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> RouteItemList;
	docData.GetELoadItemListOf(RouteItemList , CRouteItem::TypeString());
	///if(pRouteItemList)
	{
		CRouteItem *pRouteItem = docData.FindRouteItemByName(rRouteType);
		if(pRouteItem)
		{
			//!왼쪽에 연결된 Route를 찾는다.
			CRouteItem *pLeftRouteItem = GetDirectionChildRouteItem(rRouteType, "Left");
			if(pLeftRouteItem)
			{
				const string rLeftRouteType = pLeftRouteItem->GetName();
				list<CELoadItem*>::iterator where = find_if(RouteItemList.begin(), RouteItemList.end(), bind2nd(MatchParent(), rLeftRouteType));
				if(where != RouteItemList.end())
				{
					FillRouteCable(rLeftRouteType);
				}
				pRouteItem->m_HV_PowerCableEntry.insert(pRouteItem->m_HV_PowerCableEntry.end(), pLeftRouteItem->m_HV_PowerCableEntry.begin(),pLeftRouteItem->m_HV_PowerCableEntry.end());
				pRouteItem->m_LV_PowerCableEntry.insert(pRouteItem->m_LV_PowerCableEntry.end(), pLeftRouteItem->m_LV_PowerCableEntry.begin(),pLeftRouteItem->m_LV_PowerCableEntry.end());
				pRouteItem->m_ControlCableEntry.insert(pRouteItem->m_ControlCableEntry.end(), pLeftRouteItem->m_ControlCableEntry.begin(),pLeftRouteItem->m_ControlCableEntry.end());
			}
			
			//!직선방향에 연결된 Route를 찾는다.
			CRouteItem *pStraightRouteItem = GetDirectionChildRouteItem(rRouteType , "Straight");
			if(pStraightRouteItem)
			{
				const string rStraightRouteType = pStraightRouteItem->GetName();
				list<CELoadItem*>::iterator where = find_if(RouteItemList.begin(), RouteItemList.end(), bind2nd(MatchParent(), rStraightRouteType));
				if(where != RouteItemList.end())
				{
					FillRouteCable(rStraightRouteType);
				}
				pRouteItem->m_HV_PowerCableEntry.insert(pRouteItem->m_HV_PowerCableEntry.end(), pStraightRouteItem->m_HV_PowerCableEntry.begin(),pStraightRouteItem->m_HV_PowerCableEntry.end());
				pRouteItem->m_LV_PowerCableEntry.insert(pRouteItem->m_LV_PowerCableEntry.end(), pStraightRouteItem->m_LV_PowerCableEntry.begin(),pStraightRouteItem->m_LV_PowerCableEntry.end());
				pRouteItem->m_ControlCableEntry.insert(pRouteItem->m_ControlCableEntry.end(), pStraightRouteItem->m_ControlCableEntry.begin(),pStraightRouteItem->m_ControlCableEntry.end());
			}
			//!오른쪽에 연결된 Route를 찾는다.
			CRouteItem *pRightRouteItem = GetDirectionChildRouteItem(rRouteType, "Right");
			if(pRightRouteItem)
			{
				const string rRightRouteType = pRightRouteItem->GetName();
				list<CELoadItem*>::iterator where = find_if(RouteItemList.begin(), RouteItemList.end(), bind2nd(MatchParent(), rRightRouteType));
				if(where != RouteItemList.end())
				{
					FillRouteCable(rRightRouteType);
				}
				pRouteItem->m_HV_PowerCableEntry.insert(pRouteItem->m_HV_PowerCableEntry.end(), pRightRouteItem->m_HV_PowerCableEntry.begin(),pRightRouteItem->m_HV_PowerCableEntry.end());
				pRouteItem->m_LV_PowerCableEntry.insert(pRouteItem->m_LV_PowerCableEntry.end(), pRightRouteItem->m_LV_PowerCableEntry.begin(),pRightRouteItem->m_LV_PowerCableEntry.end());
				pRouteItem->m_ControlCableEntry.insert(pRouteItem->m_ControlCableEntry.end(), pRightRouteItem->m_ControlCableEntry.begin(),pRightRouteItem->m_ControlCableEntry.end());
			}
			
			//!SummaryFlag TRUE
			pRouteItem->SetSummaryFlag(true);
		}
		return ERROR_SUCCESS;
	}
	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	RouteType의 방향의 자식 Route를 찾는다.

	@author KHS	

	@date 2009-05-28 오후 6:56:25	

	@param	

	@return		
**/
CRouteItem* CCableTrayFillRateCalculationDlg::GetDirectionChildRouteItem(const string& rRouteType, const string& rRouteDirection)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> RouteItemList;
	docData.GetELoadItemListOf(RouteItemList , CRouteItem::TypeString());

	for(list<CELoadItem*>::iterator itr = RouteItemList.begin(); itr != RouteItemList.end();++itr)
	{
		const string rParent = (*itr)->prop()->GetValue(_T("General"), _T("Parent"));
		if(rParent == rRouteType)
		{
			const string rDirection = (*itr)->prop()->GetValue(_T("General"), _T("Direction"));
			if(rRouteDirection == rDirection)
			{
				CRouteItem *pRouteItem = static_cast<CRouteItem*>(*itr);
				if(pRouteItem)
				{
					return pRouteItem;
				}
			}
		}
	}

	return NULL;
}


/**
	@brief	TEST용으로 Route를 출력한다.

	@author KHS	

	@date 2009-06-01 오후 1:56:03	

	@param	

	@return		
**/
int CCableTrayFillRateCalculationDlg::TxtFileExport(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string rFilePath = docData.GetProjectFolderPath();
	ofstream ofile(rFilePath.c_str() + CString(_T("Working\\RouteTray_Output.txt")));
	if(ofile.is_open())
	{
		list<CELoadItem*> RouteItemList;
		docData.GetELoadItemListOf(RouteItemList , CRouteItem::TypeString());
		//if(pRouteItemList)
		{
			for(list<CELoadItem*>::iterator itr = RouteItemList.begin(); itr != RouteItemList.end();++itr)
			{
				const string rRouteName = (*itr)->GetName();
				CRouteItem* pRouteItem = static_cast<CRouteItem*>(*itr);
				if(pRouteItem)
				{
					string rRouteTray;
					for(vector<CTrayItem*>::iterator ktr = pRouteItem->m_TrayItemEntry.begin(); ktr != pRouteItem->m_TrayItemEntry.end();++ktr)
					{
						rRouteTray += (*ktr)->GetName() + ",";
					}

					double nRatingCapacity = 0.f;
					for(vector<CCableItem*>::iterator jtr = pRouteItem->m_HV_PowerCableEntry.begin(); jtr != pRouteItem->m_HV_PowerCableEntry.end();++jtr)
					{
						const string rHVCableName = (*jtr)->GetName();
						const string rLoadId = (*jtr)->prop()->GetValue(_T("General"), _T("Load Id"));
						const string rTrayList = (*jtr)->prop()->GetValue(_T("Tray"), _T("Tray List"));

						CLoadItem* pLoadItem = docData.FindLoadItemByName(rLoadId);
						if(pLoadItem)
						{
							pLoadItem->CalcRatingCapacity(nRatingCapacity);
						}

						ofile << "Route Name : " << rRouteName << " Load ID :" << rLoadId << " Rating :"<< nRatingCapacity << " CABLE TYPE : HV POWER CABLE"<< " Cable Name : " << rHVCableName << " Cable Tray List : " << rTrayList << " Route TrayList : " << rRouteTray << endl;
					}

					for(vector<CCableItem*>::iterator jtr = pRouteItem->m_LV_PowerCableEntry.begin(); jtr != pRouteItem->m_LV_PowerCableEntry.end();++jtr)
					{
						const string rLVCableName = (*jtr)->GetName();
						const string rLoadId = (*jtr)->prop()->GetValue(_T("General"), _T("Load Id"));
						const string rTrayList = (*jtr)->prop()->GetValue(_T("Tray"), _T("Tray List"));

						CLoadItem* pLoadItem = docData.FindLoadItemByName(rLoadId);
						if(pLoadItem)
						{
							pLoadItem->CalcRatingCapacity(nRatingCapacity);
						}
						ofile << "Route Name : " <<rRouteName << " Load ID :" << rLoadId << " Rating :"<< nRatingCapacity << " CABLE TYPE : LV POWER CABLE"<< " Cable Name :" << rLVCableName << " Cable Tray List : " << rTrayList << " Route TrayList : " << rRouteTray << endl;
					}

					for(vector<CCableItem*>::iterator jtr = pRouteItem->m_ControlCableEntry.begin(); jtr != pRouteItem->m_ControlCableEntry.end();++jtr)
					{
						const string rControlCableName = (*jtr)->GetName();
						const string rLoadId = (*jtr)->prop()->GetValue(_T("General"), _T("Load Id"));
						const string rTrayList = (*jtr)->prop()->GetValue(_T("Tray"), _T("Tray List"));

						CLoadItem* pLoadItem = docData.FindLoadItemByName(rLoadId);
						if(pLoadItem)
						{
							pLoadItem->CalcRatingCapacity(nRatingCapacity);
						}

						ofile << "Route Name : " <<rRouteName << " Load ID :" << rLoadId << " Rating :"<< nRatingCapacity << " CABLE TYPE : CONTROL CABLE"<< " Cable Name :" << rControlCableName << " Cable Tray List : " << rTrayList << " Route TrayList : " << rRouteTray << endl;
					}
				}
			}
		}
	}
	ofile.close();

	return ERROR_SUCCESS;
}

