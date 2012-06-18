// CableBMSummaryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "CableBMSummaryDlg.h"
#include "ELoadDocData.h"
#include <vector>
#include "pyfromc.h"
#include "SimpleAnimationDlg.h"

// CCableBMSummaryDlg dialog
int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CListCtrl *pListCtrl = (CListCtrl*)lParamSort;

	LVFINDINFO info1, info2;
	
	info1.flags = LVFI_PARAM;
	info1.lParam = lParam1;

	info2.flags = LVFI_PARAM;
	info2.lParam = lParam2;

	int iRow1 = pListCtrl->FindItem(&info1, -1);
	int iRow2 = pListCtrl->FindItem(&info2, -1);
	
	CString strItem1, strItem2;
	int res = 0;
	for(int i = 0; i < 5;i++)
	{
		strItem1 = pListCtrl->GetItemText(iRow1, i);
		strItem2 = pListCtrl->GetItemText(iRow2, i);
		if(3 <= i)
		{
			if(atoi(strItem1) > atoi(strItem2)) return 1;
			else if(atoi(strItem1) < atoi(strItem2)) return -1;
			else continue;
		}
		else if(2 == i )
		{
			strItem1.Replace("C", "");
			strItem2.Replace("C", "");
			if(atoi(strItem1) > atoi(strItem2)) return 1;
			else if(atoi(strItem1) < atoi(strItem2)) return -1;
			else continue;

		}
		else if(0 == i)
		{
			vector<string> oResult1, oResult2;
			CTokenizer<CIsFromString>::Tokenize(oResult1, strItem1.operator LPCSTR(), CIsFromString("/"));
			CTokenizer<CIsFromString>::Tokenize(oResult2, strItem2.operator LPCSTR(), CIsFromString("/"));
			
			if(atof(oResult1[0].c_str()) > atof(oResult2[0].c_str())) return 1;
			else if(atof(oResult1[0].c_str()) < atof(oResult2[0].c_str())) return -1;
		}
		else
		{
			res = strcmp(strItem1, strItem2);
			if(0 != res) return res;
		}
	}

	return res;
}

IMPLEMENT_DYNAMIC(CCableBMSummaryDlg, CDialog)

CCableBMSummaryDlg::CCableBMSummaryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCableBMSummaryDlg::IDD, pParent)
{
	m_nHvpowerCableTotalLength = 0;
	m_nLvpowerCableTotalLength = 0;
	m_nControlCableTotalLength = 0;
	
}

CCableBMSummaryDlg::~CCableBMSummaryDlg()
{
}

void CCableBMSummaryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_wndOkButton);
	DDX_Control(pDX, IDCANCEL, m_wndCancelButton);
	DDX_Control(pDX, IDC_AREA_COMBO, m_wndAreaCombo);
	DDX_Control(pDX, IDC_UNIT_COMBO, m_wndUnitCombo);
	DDX_Control(pDX, IDC_DETATIL_TOTAL_LENGTH_EDIT2, m_wndTotalLengthEdit);
	DDX_Control(pDX, IDC_HV_POWER_CABLE_BM_EDIT, m_wndHVPowerCableBmEdit);
	DDX_Control(pDX, IDC_LV_POWER_CABLE_BM_EDIT, m_wndLVPowerCableBmEdit);
	DDX_Control(pDX, IDC_CONOTROL_CABLE_BM_EDIT, m_wndControlCableBmEdit);
	DDX_Control(pDX, IDC_BM_TOTAL_EDIT, m_wndTotalBmEdit);
	DDX_Control(pDX, IDC_REPORT_BUTTON, m_wndReportButton);
}


BEGIN_MESSAGE_MAP(CCableBMSummaryDlg, CDialog)
	ON_BN_CLICKED(IDC_HV_POWERCABLE_BUTTON, &CCableBMSummaryDlg::OnBnClickedHvPowercableButton)
	ON_BN_CLICKED(IDC_LV_POWERCABLE_BUTTON, &CCableBMSummaryDlg::OnBnClickedLvPowercableButton)
	ON_BN_CLICKED(IDC_CONTROL_CABLE_BUTTON, &CCableBMSummaryDlg::OnBnClickedControlCableButton)
	ON_BN_CLICKED(IDOK, &CCableBMSummaryDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_UNIT_COMBO, &CCableBMSummaryDlg::OnCbnSelchangeUnitCombo)
	ON_BN_CLICKED(IDC_REPORT_BUTTON, &CCableBMSummaryDlg::OnBnClickedReportButton)
	ON_CBN_SELCHANGE(IDC_AREA_COMBO, &CCableBMSummaryDlg::OnCbnSelchangeAreaCombo)
END_MESSAGE_MAP()


// CCableBMSummaryDlg message handlers

BOOL CCableBMSummaryDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	//!BUTTON SETTING
	CELoadDocData & docData = CELoadDocData::GetInstance();
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	m_wndOkButton.SetTextHotColor(TextHotColor);
	m_wndCancelButton.SetTextHotColor(TextHotColor);
	m_wndReportButton.SetTextHotColor(TextHotColor);

	m_wndOkButton.SetIcon(IDI_OK_18X18);
	m_wndCancelButton.SetIcon(IDI_CANCEL_18X18);
	m_wndReportButton.SetIcon(IDI_EXCEL);

	m_wndHVPowerCableButton.Attach(IDC_HV_POWERCABLE_BUTTON, this);
	m_wndLVPowerCableButton.Attach(IDC_LV_POWERCABLE_BUTTON, this);
	m_wndControlCableButton.Attach(IDC_CONTROL_CABLE_BUTTON, this);

	m_wndHVPowerCableButton.SetColor(RGB(0,0,0), RGB(196,222,241));
	m_wndLVPowerCableButton.SetColor(RGB(0,0,0), RGB(255,255,255));
	m_wndControlCableButton.SetColor(RGB(0,0,0), RGB(255,255,255));

	BeginWaitCursor();
	//! CREATE LIST CTRL
	if(ERROR_SUCCESS != CreateListCtrl())
	{
		AfxMessageBox("Fail, Create List Ctrl...!", MB_ICONWARNING|MB_OK);
		return FALSE;
	}
	
	m_wndAreaCombo.AddString("*");
	m_wndUnitCombo.AddString("*");
	
	FillComboBox();

	DisplayCableData("*", "*");

	const int nTotalLength = m_nControlCableTotalLength + m_nLvpowerCableTotalLength + m_nHvpowerCableTotalLength;
	CString rValue;
	rValue.Format("%d", nTotalLength);
	m_wndTotalBmEdit.SetWindowText(rValue);
	
	rValue.Format("%d", m_nHvpowerCableTotalLength);
	m_wndHVPowerCableBmEdit.SetWindowText(rValue);

	rValue.Format("%d", m_nLvpowerCableTotalLength);
	m_wndLVPowerCableBmEdit.SetWindowText(rValue);

	rValue.Format("%d", m_nControlCableTotalLength);
	m_wndControlCableBmEdit.SetWindowText(rValue);

	m_wndHVPowerCableButton.SetFocus();
	OnBnClickedHvPowercableButton();

	EndWaitCursor();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	CREATE LIST CTRL

	@author KHS	

	@return		
**/
int CCableBMSummaryDlg::CreateListCtrl(void)
{
	const CString __HEADERS__[] = {_T("VOLTAGE"), _T("SPEC."), _T("CORE"),_T("SIZE"), _T("LENGTH [m]")};

	CRect rect;
	GetDlgItem(IDC_LIST_STATIC)->GetWindowRect(&rect);
	ScreenToClient(&rect);

	const DWORD dwStyle = WS_CHILD | WS_BORDER| WS_VISIBLE | WS_CLIPSIBLINGS | WS_VSCROLL | LVS_REPORT | LVS_EDITLABELS | LVS_SHOWSELALWAYS;

	if(!m_wndHVPowerCableListCtrl.Create(dwStyle, rect, this, 1))
	{
		return ERROR_BAD_ENVIRONMENT;
	}
	
	if(!m_wndLVPowerCableListCtrl.Create(dwStyle, rect, this, 2))
	{
		return ERROR_BAD_ENVIRONMENT;
	}

	if(!m_wndControlCableListCtrl.Create(dwStyle, rect, this , 3))
	{
		return ERROR_BAD_ENVIRONMENT;
	}

	const int nColCount = sizeof(__HEADERS__) / sizeof(__HEADERS__[0]);


	{
		m_wndHVPowerCableListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
		m_wndHVPowerCableListCtrl.InsertColumn(0, __HEADERS__[0], LVCFMT_LEFT, 80);
		m_wndHVPowerCableListCtrl.InsertColumn(1, __HEADERS__[1], LVCFMT_LEFT, 70);
		m_wndHVPowerCableListCtrl.InsertColumn(2, __HEADERS__[2], LVCFMT_LEFT, 50);
		m_wndHVPowerCableListCtrl.InsertColumn(3, __HEADERS__[3], LVCFMT_RIGHT, 50);
		m_wndHVPowerCableListCtrl.InsertColumn(4, __HEADERS__[4], LVCFMT_RIGHT, 90);
		
		m_wndLVPowerCableListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
		m_wndLVPowerCableListCtrl.InsertColumn(0, __HEADERS__[0], LVCFMT_LEFT, 80);
		m_wndLVPowerCableListCtrl.InsertColumn(1, __HEADERS__[1], LVCFMT_LEFT, 70);
		m_wndLVPowerCableListCtrl.InsertColumn(2, __HEADERS__[2], LVCFMT_LEFT, 50);
		m_wndLVPowerCableListCtrl.InsertColumn(3, __HEADERS__[3], LVCFMT_RIGHT, 50);
		m_wndLVPowerCableListCtrl.InsertColumn(4, __HEADERS__[4], LVCFMT_RIGHT, 90);

		m_wndControlCableListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
		m_wndControlCableListCtrl.InsertColumn(0, __HEADERS__[0], LVCFMT_LEFT, 80);
		m_wndControlCableListCtrl.InsertColumn(1, __HEADERS__[1], LVCFMT_LEFT, 70);
		m_wndControlCableListCtrl.InsertColumn(2, __HEADERS__[2], LVCFMT_LEFT, 50);
		m_wndControlCableListCtrl.InsertColumn(3, __HEADERS__[3], LVCFMT_RIGHT, 50);
		m_wndControlCableListCtrl.InsertColumn(4, __HEADERS__[4], LVCFMT_RIGHT, 90);
	}
	
	m_wndHVPowerCableListCtrl.SetWindowPos(&(CWnd::wndTop),rect.left, rect.top, rect.Width(), rect.Height(),SWP_NOACTIVATE);
	m_wndLVPowerCableListCtrl.SetWindowPos(&(CWnd::wndTop),rect.left, rect.top, rect.Width(), rect.Height(),SWP_NOACTIVATE);
	m_wndControlCableListCtrl.SetWindowPos(&(CWnd::wndTop),rect.left, rect.top, rect.Width(), rect.Height(),SWP_NOACTIVATE);


	return ERROR_SUCCESS;
}

/**
	@brief	CLICK HV POWER CABLE BUTTON

	@author KHS	

	@date 2009-05-18 오후 1:50:26	

	@param	

	@return		
**/
void CCableBMSummaryDlg::OnBnClickedHvPowercableButton()
{
	m_wndHVPowerCableButton.SetState(TRUE);
	m_wndLVPowerCableButton.SetState(FALSE);
	m_wndControlCableButton.SetState(FALSE);
	
	m_wndHVPowerCableButton.SetBGColor(RGB(196,222,241), TRUE);
	m_wndLVPowerCableButton.SetBGColor(RGB(255,255,255), TRUE);
	m_wndControlCableButton.SetBGColor(RGB(255,255,255), TRUE);

	m_wndHVPowerCableListCtrl.ShowWindow(TRUE);
	m_wndLVPowerCableListCtrl.ShowWindow(FALSE);
	m_wndControlCableListCtrl.ShowWindow(FALSE);
	
	CString rTotalLength;
	rTotalLength.Format("%d", m_nHvpowerCableTotalLength);
	m_wndTotalLengthEdit.SetWindowText(rTotalLength);
	
}

/**
	@brief	CLICK LV POWER CABLE BUTTON

	@author KHS	

	@date 2009-05-18 오후 1:50:40	

	@param	

	@return		
**/
void CCableBMSummaryDlg::OnBnClickedLvPowercableButton()
{
	m_wndHVPowerCableButton.SetState(FALSE);
	m_wndLVPowerCableButton.SetState(TRUE);
	m_wndControlCableButton.SetState(FALSE);
	
	m_wndHVPowerCableButton.SetBGColor(RGB(255,255,255), TRUE);
	m_wndLVPowerCableButton.SetBGColor(RGB(196,222,241), TRUE);
	m_wndControlCableButton.SetBGColor(RGB(255,255,255), TRUE);

	m_wndHVPowerCableListCtrl.ShowWindow(FALSE);
	m_wndLVPowerCableListCtrl.ShowWindow(TRUE);
	m_wndControlCableListCtrl.ShowWindow(FALSE);
	
	CString rTotalLength;
	rTotalLength.Format("%d", m_nLvpowerCableTotalLength);
	m_wndTotalLengthEdit.SetWindowText(rTotalLength);
}	


/**
	@brief	CLICK CONTROL CABLE BUTTON

	@author KHS	

	@date 2009-05-18 오후 1:51:43	

	@param	

	@return		
**/
void CCableBMSummaryDlg::OnBnClickedControlCableButton()
{
	m_wndHVPowerCableButton.SetState(FALSE);
	m_wndLVPowerCableButton.SetState(FALSE);
	m_wndControlCableButton.SetState(TRUE);
	
	m_wndHVPowerCableButton.SetBGColor(RGB(255,255,255), TRUE);
	m_wndLVPowerCableButton.SetBGColor(RGB(255,255,255), TRUE);
	m_wndControlCableButton.SetBGColor(RGB(196,222,241), TRUE);

	m_wndHVPowerCableListCtrl.ShowWindow(FALSE);
	m_wndLVPowerCableListCtrl.ShowWindow(FALSE);
	m_wndControlCableListCtrl.ShowWindow(TRUE);

	CString rTotalLength;
	rTotalLength.Format(_T("%d") , m_nControlCableTotalLength);
	m_wndTotalLengthEdit.SetWindowText(rTotalLength);
}

BOOL CCableBMSummaryDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE) return TRUE;
		if(pMsg->wParam == VK_RETURN) return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CCableBMSummaryDlg::OnBnClickedOk()
{
	OnOK();
}


/**
	@brief	Cable Data Display

	@author KHS	

	@date 2009-06-18 오후 1:31:57	

	@param	

	@return		
**/
int CCableBMSummaryDlg::DisplayCableData(const string& rArea, const string& rUnit)
{
	m_wndHVPowerCableListCtrl.DeleteAllItems();
	m_wndLVPowerCableListCtrl.DeleteAllItems();
	m_wndControlCableListCtrl.DeleteAllItems();

	m_nHvpowerCableTotalLength = 0;
	m_nLvpowerCableTotalLength = 0;
	m_nControlCableTotalLength = 0;

	CELoadDocData& docData = CELoadDocData::GetInstance();
	
	list<CELoadItem*> CableItemList;
	docData.GetELoadItemListOf(CableItemList, CCableItem::TypeString(), "*");

	vector<CABLE_ITEM_STRUCT> CableItemEntry;
	for(list<CELoadItem*>::iterator itr = CableItemList.begin(); itr != CableItemList.end();++itr)
	{
		CCableItem *pCableItem = static_cast<CCableItem*>(*itr);
		if(pCableItem)
		{
			CABLE_ITEM_STRUCT record;

			record.rCableVolt = pCableItem->prop()->GetValue(_T("General"), _T("Cable Volt"));
			record.rSpec = pCableItem->prop()->GetValue(_T("General"), _T("Cable Spec"));
			record.rCore = pCableItem->prop()->GetValue(_T("General"), _T("Core"));
			record.rSize = pCableItem->prop()->GetValue(_T("General"), _T("Size"));
			record.rLength = pCableItem->prop()->GetValue(_T("General"), _T("Length"));	
			record.rUnit = pCableItem->prop()->GetValue(_T("General"), _T("Unit"));
			record.area = pCableItem->prop()->GetValue(_T("General"), _T("Area"));
			record.rCableCategory = pCableItem->prop()->GetValue(_T("General"), _T("Cable Category"));
			record.rCableVoltType = pCableItem->prop()->GetValue(_T("General"), _T("Volt Type")).c_str();

			//! area와 unit를 검사한다.
			if(rUnit != "*" && record.rUnit != rUnit) continue;
			if(rArea != "*" && record.area != rArea) continue;

			if(!CompareMatchCable(record, CableItemEntry)) CableItemEntry.push_back(record);
		}
	}
	for(vector<CABLE_ITEM_STRUCT>::iterator jtr = CableItemEntry.begin(); jtr != CableItemEntry.end();++jtr)
	{
		if(_T("Power Cable") == jtr->rCableCategory)
		{
			if(jtr->rCableVoltType.Find(_T("HV")) != -1)
			{
				//!HV POWER CABLE
				int nIndex = m_wndHVPowerCableListCtrl.GetItemCount();
				int nItem = m_wndHVPowerCableListCtrl.InsertItem(nIndex, jtr->rCableVolt.c_str());
				if(-1 != nItem)
				{
					m_wndHVPowerCableListCtrl.SetItemText(nItem, 1, jtr->rSpec.c_str());
					m_wndHVPowerCableListCtrl.SetItemText(nItem, 2, jtr->rCore.c_str());
					m_wndHVPowerCableListCtrl.SetItemText(nItem, 3, jtr->rSize.c_str());
					m_wndHVPowerCableListCtrl.SetItemText(nItem, 4, jtr->rLength.c_str());
					m_wndHVPowerCableListCtrl.SetItemData(nItem, (DWORD)nItem);

					m_nHvpowerCableTotalLength += atoi(jtr->rLength.c_str());
				}
			}
			else if((-1 != jtr->rCableVoltType.Find(_T("LV"))) || (-1 != jtr->rCableVoltType.Find(_T("DC"))))
			{
				//! LV POWER CABLE
				int nIndex = m_wndLVPowerCableListCtrl.GetItemCount();
				int nItem = m_wndLVPowerCableListCtrl.InsertItem(nIndex, jtr->rCableVolt.c_str());
				if(-1 != nItem)
				{
					m_wndLVPowerCableListCtrl.SetItemText(nItem, 1, jtr->rSpec.c_str());
					m_wndLVPowerCableListCtrl.SetItemText(nItem, 2, jtr->rCore.c_str());
					m_wndLVPowerCableListCtrl.SetItemText(nItem, 3, jtr->rSize.c_str());
					m_wndLVPowerCableListCtrl.SetItemText(nItem, 4, jtr->rLength.c_str());
					m_wndLVPowerCableListCtrl.SetItemData(nItem, (DWORD)nItem);

					m_nLvpowerCableTotalLength += atoi(jtr->rLength.c_str());
				}
			}
		}
		else if(_T("Control Cable") == jtr->rCableCategory)
		{
			//CONTROL CABLE
			int nIndex = m_wndControlCableListCtrl.GetItemCount();
			int nItem = m_wndControlCableListCtrl.InsertItem(nIndex, jtr->rCableVolt.c_str());
			if(-1 != nItem)
			{
				m_wndControlCableListCtrl.SetItemText(nItem, 1, jtr->rSpec.c_str());
				m_wndControlCableListCtrl.SetItemText(nItem, 2, jtr->rCore.c_str());
				m_wndControlCableListCtrl.SetItemText(nItem, 3, jtr->rSize.c_str());
				m_wndControlCableListCtrl.SetItemText(nItem, 4, jtr->rLength.c_str());
				m_wndControlCableListCtrl.SetItemData(nItem, (DWORD)nItem);
				m_nControlCableTotalLength += atoi(jtr->rLength.c_str());
			}
		}
	}
	m_wndHVPowerCableListCtrl.SortItems(CompareFunc, (LPARAM)&m_wndHVPowerCableListCtrl);
	m_wndLVPowerCableListCtrl.SortItems(CompareFunc, (LPARAM)&m_wndLVPowerCableListCtrl);
	m_wndControlCableListCtrl.SortItems(CompareFunc, (LPARAM)&m_wndControlCableListCtrl);

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	Area Combo Selchange

	@author KHS	

	@date 2009-06-18 오후 4:08:19	

	@param	

	@return		
**/
void CCableBMSummaryDlg::OnCbnSelchangeAreaCombo()
{
	BeginWaitCursor();

	CString rArea, rUnit;
	int nItem = m_wndAreaCombo.GetCurSel();
	if(-1 != nItem)
	{
		m_wndAreaCombo.GetLBText(nItem, rArea);
	}

	nItem = m_wndUnitCombo.GetCurSel();
	if(-1 != nItem)
	{
		m_wndUnitCombo.GetLBText(nItem, rUnit);
	}

	DisplayCableData(rArea.operator LPCSTR() , rUnit.operator LPCSTR());

	if(12 == m_wndHVPowerCableButton.GetState())
	{
		OnBnClickedHvPowercableButton();
	}
	else if(12 == m_wndLVPowerCableButton.GetState())
	{
		OnBnClickedLvPowercableButton();
	}
	else if(12 == m_wndControlCableButton.GetState())
	{
		OnBnClickedControlCableButton();
	}

	EndWaitCursor();
}

/**
	@brief	Unit Combo Selchange

	@author KHS	

	@date 2009-06-18 오후 4:08:22	

	@param	

	@return		
**/
void CCableBMSummaryDlg::OnCbnSelchangeUnitCombo()
{
	BeginWaitCursor();

	CString rArea, rUnit;
	int nItem = m_wndAreaCombo.GetCurSel();
	if(-1 != nItem)
	{
		m_wndAreaCombo.GetLBText(nItem, rArea);
	}

	nItem = m_wndUnitCombo.GetCurSel();
	if(-1 != nItem)
	{
		m_wndUnitCombo.GetLBText(nItem, rUnit);
	}

	DisplayCableData(rArea.operator LPCSTR() , rUnit.operator LPCSTR());
	
	if(12 == m_wndHVPowerCableButton.GetState())
	{
		OnBnClickedHvPowercableButton();
	}
	else if(12 == m_wndLVPowerCableButton.GetState())
	{
		OnBnClickedLvPowercableButton();
	}
	else if(12 == m_wndControlCableButton.GetState())
	{
		OnBnClickedControlCableButton();
	}

	EndWaitCursor();
}


/**
	@brief	Fill Area, Unit Combo Box

	@author KHS	

	@date 2009-06-18 오후 4:39:33	

	@param	

	@return		
**/
int CCableBMSummaryDlg::FillComboBox(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	
	list<CELoadItem*> CableItemList;
	docData.GetELoadItemListOf(CableItemList, CCableItem::TypeString(), "*");
	for(list<CELoadItem*>::iterator itr = CableItemList.begin(); itr != CableItemList.end();++itr)
	{
		CCableItem *pCableItem = static_cast<CCableItem*>(*itr);
		if(pCableItem)
		{
			const string rLoadId = pCableItem->prop()->GetValue(_T("General"), _T("Load Id"));

			string rCableUnit, rCableArea;
			/*CLoadItem* pLoadItem = docData.FindLoadItemByName(rLoadId);
			if(pLoadItem)*/
			{
				rCableUnit = pCableItem->prop()->GetValue(_T("General"), _T("Unit"));
				rCableArea = pCableItem->prop()->GetValue(_T("General"), _T("Area"));
			}
			
			//! Fill Area combo from cable Area
			int nItem = m_wndAreaCombo.FindString(0, rCableArea.c_str());
			if(-1 == nItem && !rCableArea.empty())
			{
				m_wndAreaCombo.AddString(rCableArea.c_str());
			}

			nItem = m_wndAreaCombo.FindString(0, "*");
			if(-1 != nItem)
			{
				m_wndAreaCombo.SetCurSel(nItem);
			}

			//! Fill unit combo from cable Unit
			nItem = m_wndUnitCombo.FindString(0, rCableUnit.c_str());
			if(-1 == nItem && !rCableUnit.empty())
			{
				m_wndUnitCombo.AddString(rCableUnit.c_str());
			}

			nItem = m_wndUnitCombo.FindString(0, "*");
			if(-1 != nItem)
			{
				m_wndUnitCombo.SetCurSel(nItem);
			}
		}
	}

	return ERROR_SUCCESS;
}

static UINT gfnPrintCableBmSummary(LPVOID pVoid)
{
	CSimpleAnimationDlg& anidlg = CSimpleAnimationDlg::GetInstance();
	if(NULL == anidlg.GetSafeHwnd())
	{
		anidlg.DoModal();
	}

	return ERROR_SUCCESS;
}

/**
	@brief	Cable BM Summary Excel Report

	@author KHS	

	@date 2009-06-22 오후 3:45:34	

	@param	

	@return		
**/
void CCableBMSummaryDlg::OnBnClickedReportButton()
{
	CWinThread* pThread = AfxBeginThread(gfnPrintCableBmSummary , NULL , THREAD_PRIORITY_NORMAL,0);
	
	WriteCableBMDataInTextFile();
	python.CallPyWriteCableBMSummaryToExcel();
	
	if(NULL != pThread)
	{
		CSimpleAnimationDlg& anidlg = CSimpleAnimationDlg::GetInstance();
		if(anidlg.GetSafeHwnd()) anidlg.SendMessage(WM_COMMAND , IDOK);
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-07-28 오후 3:23:45	

	@param	

	@return		
*/
struct FindMatchCableItem : binary_function<CCableBMSummaryDlg::CABLE_ITEM_STRUCT , CCableBMSummaryDlg::CABLE_ITEM_STRUCT , bool>
{
	bool operator()(const CCableBMSummaryDlg::CABLE_ITEM_STRUCT& lhs , const CCableBMSummaryDlg::CABLE_ITEM_STRUCT& rhs) const
	{
		const bool res = ((lhs.rCableVolt ==  rhs.rCableVolt) && (lhs.rSpec == rhs.rSpec) && (lhs.rCore == rhs.rCore) &&
		(lhs.rSize == rhs.rSize) && (lhs.rCableCategory == rhs.rCableCategory));

		return res;
	}
};

/**
	@brief	같은 Cable Item이 있는지 검사

	@author KHS	

	@date 2009-06-22 오후 4:20:32	

	@param	

	@return		
**/
BOOL CCableBMSummaryDlg::CompareMatchCable(CABLE_ITEM_STRUCT record, vector<CABLE_ITEM_STRUCT> &CableItemEntry)
{
	vector<CABLE_ITEM_STRUCT>::iterator where = find_if(CableItemEntry.begin() , CableItemEntry.end() , bind2nd(FindMatchCableItem() , record));
	if(where != CableItemEntry.end())
	{
		if(_T("Power Cable") == where->rCableCategory)
		{
			if((where->rCableVoltType.Find("LV") != record.rCableVoltType.Find("LV")) && (where->rCableVoltType.Find("HV") != record.rCableVoltType.Find("HV")))
			{
				return FALSE;///continue;
			}
		}
		const int nLength = atoi(record.rLength.c_str());
		
		map<string , double>::iterator at = where->AreaLengthMap.find(record.area);
		if(at == where->AreaLengthMap.end()) 
			where->AreaLengthMap.insert(make_pair(record.area , nLength));
		else	where->AreaLengthMap[record.area] += nLength;

		int nTotalLength = atoi(where->rLength.c_str());
		nTotalLength += nLength;
		CString rLength;
		rLength.Format("%d" ,nTotalLength);
		where->rLength = rLength.operator LPCSTR();

		return TRUE;
	}

	/*
	for(vector<CABLE_ITEM_STRUCT>::iterator itr = CableItemEntry.begin(); itr != CableItemEntry.end();++itr)
	{
		if((itr->rCableVolt ==  record.rCableVolt) &&
		(itr->rSpec ==  record.rSpec) &&
		(itr->rCore ==  record.rCore) &&
		(itr->rSize ==  record.rSize) &&
		(itr->rCableCategory == record.rCableCategory))
		{
			if(_T("Power Cable") == itr->rCableCategory)
			{
				if((itr->rCableVoltType.Find("LV") != record.rCableVoltType.Find("LV")) && (itr->rCableVoltType.Find("HV") != record.rCableVoltType.Find("HV")))
				{
					continue;
				}
			}
			const int nLength = atoi(record.rLength.c_str());
			int nTotalLength = atoi(itr->rLength.c_str());
			nTotalLength += nLength;
			CString rLength;
			rLength.Format("%d" ,nTotalLength);
			itr->rLength = rLength.operator LPCSTR();

			return TRUE;
		}
	}
	*/

	return FALSE;
}

BOOL CCableBMSummaryDlg::CompareMatchCableInMap(CABLE_ITEM_STRUCT record, map<string , vector<CABLE_ITEM_STRUCT> >& CableItemMap)
{
	map<string , vector<CABLE_ITEM_STRUCT> >::iterator at = CableItemMap.find(record.area);
	if(at != CableItemMap.end())
	///for(map<string , vector<CABLE_ITEM_STRUCT>>::iterator itr = CableItemMap.begin();itr != CableItemMap.end();++itr)
	{
		vector<CABLE_ITEM_STRUCT>::iterator where = find_if(at->second.begin() , at->second.end() , bind2nd(FindMatchCableItem() , record));
		if(where != at->second.end())
		{
			if(_T("Power Cable") == where->rCableCategory)
			{
				if((where->rCableVoltType.Find("LV") != record.rCableVoltType.Find("LV")) && (where->rCableVoltType.Find("HV") != record.rCableVoltType.Find("HV")))
				{
					return FALSE;///continue;
				}
			}
			const int nLength = atoi(record.rLength.c_str());
			int nTotalLength = atoi(where->rLength.c_str());
			nTotalLength += nLength;
			CString rLength;
			rLength.Format("%d" ,nTotalLength);
			where->rLength = rLength.operator LPCSTR();

			return TRUE;
		}
	}
	return FALSE;
}


bool fnCompareCable(CCableBMSummaryDlg::CABLE_ITEM_STRUCT lhs , CCableBMSummaryDlg::CABLE_ITEM_STRUCT rhs )
{
	
	CString strItem1, strItem2;
	int res = 0;

	strItem1 = lhs.rCableVolt.c_str();
	strItem2 = rhs.rCableVolt.c_str();
	vector<string> oResult1, oResult2;
	CTokenizer<CIsFromString>::Tokenize(oResult1, strItem1.operator LPCSTR(), CIsFromString("/"));
	CTokenizer<CIsFromString>::Tokenize(oResult2, strItem2.operator LPCSTR(), CIsFromString("/"));
	/*res = strcmp(oResult1[0].c_str(), oResult2[0].c_str());
	if(0 != res) return res < 0 ? true : false;*/
	if(atof(oResult1[0].c_str()) < atof(oResult2[0].c_str())) return true;
	else if(atof(oResult1[0].c_str()) > atof(oResult2[0].c_str())) return false;

	strItem1 = lhs.rSpec.c_str();
	strItem2 = rhs.rSpec.c_str();
	res = strcmp(strItem1, strItem2);
	if(0 != res) return res < 0 ? true : false;

	strItem1 = lhs.rCore.c_str();
	strItem2 = rhs.rCore.c_str();
	strItem1.Replace("C", "");
	strItem2.Replace("C", "");
	if(atoi(strItem1) < atoi(strItem2)) return true;
	else if(atoi(strItem1) > atoi(strItem2)) return false;

	strItem1 = lhs.rSize.c_str();
	strItem2 = rhs.rSize.c_str();
	if(atoi(strItem1) < atoi(strItem2)) return true;
	else if(atoi(strItem1) > atoi(strItem2)) return false;

	strItem1 = lhs.rLength.c_str();
	strItem2 = rhs.rLength.c_str();
	if(atoi(strItem1) < atoi(strItem2)) return true;
	else if(atoi(strItem1) > atoi(strItem2)) return false;

	return false;
}

/**
	@brief	Control cable Data를 TEXT 파일에 쓴다.

	@author KHS	

	@date 2009-06-10 오전 9:17:50	

	@param	

	@return		
**/
int CCableBMSummaryDlg::WriteCableBMDataInTextFile()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	int nIndex = 0;

	const string rProjectPath = docData.GetProjectFolderPath();
	CString rFilePath = rProjectPath.c_str() + CString(_T("Working\\"));
	
	//! 폴더가 존재하지 않으면 폴더 생성.
	CFileFind dirFind;
	if(0 == dirFind.FindFile(rFilePath.Left(rFilePath.GetLength() - 1)))
	{
		CreateDirectory(rFilePath , NULL);
	}
	
	CString rHvPowerCableAllTotal, rLvPowerCableAllTotal, rControlCableAllTotal, rAllTotal;

	m_wndHVPowerCableBmEdit.GetWindowText(rHvPowerCableAllTotal);
	m_wndLVPowerCableBmEdit.GetWindowText(rLvPowerCableAllTotal);
	m_wndControlCableBmEdit.GetWindowText(rControlCableAllTotal);
	m_wndTotalBmEdit.GetWindowText(rAllTotal);

	int nHVPowerCableTotal = 0, nLVPowerCableTotal = 0, nControlCableTotal = 0;

	list<CELoadItem*> CableItemList;
	docData.GetELoadItemListOf(CableItemList , CCableItem::TypeString() ,_T("*"));

	map<string , double> AreaLengthMap;
	vector<CABLE_ITEM_STRUCT> CableItemEntry;
	for(list<CELoadItem*>::iterator itr = CableItemList.begin(); itr != CableItemList.end();++itr)
	{
		CCableItem *pCableItem = static_cast<CCableItem*>(*itr);
		if(pCableItem)
		{
			CABLE_ITEM_STRUCT record;
			record.rCableVolt = pCableItem->prop()->GetValue(_T("General"), _T("Cable Volt"));
			record.rSpec = pCableItem->prop()->GetValue(_T("General"), _T("Cable Spec"));
			record.rCore = pCableItem->prop()->GetValue(_T("General"), _T("Core"));
			record.rSize = pCableItem->prop()->GetValue(_T("General"), _T("Size"));
			record.rLength = pCableItem->prop()->GetValue(_T("General"), _T("Length"));
			record.rCableVoltType = pCableItem->prop()->GetValue(_T("General"), _T("Volt Type")).c_str();
			record.area  = pCableItem->prop()->GetValue(_T("General"), _T("Area"));
			record.rUnit = pCableItem->prop()->GetValue(_T("General"), _T("Unit"));
			record.rCableCategory = pCableItem->prop()->GetValue(_T("General"), _T("Cable Category"));

			record.AreaLengthMap.insert(make_pair(record.area , atof(record.rLength.c_str())));

			map<string,double>::iterator where = AreaLengthMap.find(record.area);
			if(where != AreaLengthMap.end())
				AreaLengthMap[record.area] += atof(record.rLength.c_str());
			else	AreaLengthMap.insert(make_pair(record.area , atof(record.rLength.c_str())));

			if(!CompareMatchCable(record, CableItemEntry)) CableItemEntry.push_back(record);
		}
	}
	
	//!Cable Sorting
	::stable_sort(CableItemEntry.begin(), CableItemEntry.end(), fnCompareCable);

	CString rAreaNames;
	{
		ostringstream oss;
		for(map<string,double>::iterator itr = AreaLengthMap.begin();itr != AreaLengthMap.end();++itr)
		{
			oss << itr->first.c_str() << _T("\t");
		}
		rAreaNames = oss.str().c_str();
		rAreaNames.TrimRight();
	}

	//!Control Cable
	{
		ofstream ofile(rFilePath + _T("CABLE_BM_SUMMARY_CONTROL_CABLE.txt"));
		if(ofile.is_open())
		{
			
			ofile << rAreaNames.operator LPCSTR() << std::endl;
			for(vector<CABLE_ITEM_STRUCT>::iterator itr = CableItemEntry.begin();itr != CableItemEntry.end();++itr)
			{
				if(_T("Control Cable") == itr->rCableCategory)
				{
					ofile << itr->rCableVolt << "\t" << itr->rSpec << "\t" << itr->rCore << "\t" << itr->rSize << "\t" << itr->rLength << _T("\t");
					
					for(map<string,double>::iterator ktr = AreaLengthMap.begin();ktr != AreaLengthMap.end();++ktr)
					{
						if(itr->AreaLengthMap.end() == itr->AreaLengthMap.find(ktr->first)) itr->AreaLengthMap.insert(make_pair(ktr->first , 0));
					}

					ostringstream oss;
					for(map<string , double>::iterator jtr = itr->AreaLengthMap.begin();jtr != itr->AreaLengthMap.end();++jtr)
					{
						oss << jtr->second << _T("\t");
					}
					CString rString(oss.str().c_str());
					rString.TrimRight();
					ofile << rString.operator LPCSTR() << std::endl;
				}
			}
			ofile.close();
		}
	}

	//!LV power Cable
	{
		ofstream ofile(rFilePath + _T("CABLE_BM_SUMMARY_LV_POWER_CABLE.txt"));
		if(ofile.is_open())
		{
			ofile << rAreaNames.operator LPCSTR() << std::endl;
			for(vector<CABLE_ITEM_STRUCT>::iterator itr = CableItemEntry.begin();itr != CableItemEntry.end();++itr)
			{
				if((_T("Power Cable") == itr->rCableCategory) && 
					((-1 != itr->rCableVoltType.Find(_T("LV"))) || (-1 != itr->rCableVoltType.Find(_T("DC")))))
				{
					ofile << itr->rCableVolt << "\t" << itr->rSpec << "\t" << itr->rCore << "\t" << itr->rSize << "\t" << itr->rLength << _T("\t");
					for(map<string,double>::iterator ktr = AreaLengthMap.begin();ktr != AreaLengthMap.end();++ktr)
					{
						if(itr->AreaLengthMap.end() == itr->AreaLengthMap.find(ktr->first)) itr->AreaLengthMap.insert(make_pair(ktr->first , 0));
					}

					ostringstream oss;
					for(map<string , double>::iterator jtr = itr->AreaLengthMap.begin();jtr != itr->AreaLengthMap.end();++jtr)
					{
						oss << jtr->second << _T("\t");
					}
					CString rString(oss.str().c_str());
					rString.TrimRight();
					ofile << rString.operator LPCSTR() << std::endl;
				}
			}
			ofile.close();
		}
	}

	//!HV Power Cable
	{
		ofstream ofile(rFilePath + _T("CABLE_BM_SUMMARY_HV_POWER_CABLE.txt"));
		if(ofile.is_open())
		{
			ofile << rAreaNames.operator LPCSTR() << std::endl;
			for(vector<CABLE_ITEM_STRUCT>::iterator itr = CableItemEntry.begin();itr != CableItemEntry.end();++itr)
			{
				if((_T("Power Cable") == itr->rCableCategory) && (-1 != itr->rCableVoltType.Find(_T("HV"))))
				{
					ofile << itr->rCableVolt << "\t" << itr->rSpec << "\t" << itr->rCore << "\t" << itr->rSize << "\t" << itr->rLength << _T("\t");
					for(map<string,double>::iterator ktr = AreaLengthMap.begin();ktr != AreaLengthMap.end();++ktr)
					{
						if(itr->AreaLengthMap.end() == itr->AreaLengthMap.find(ktr->first)) itr->AreaLengthMap.insert(make_pair(ktr->first , 0));
					}

					ostringstream oss;
					for(map<string , double>::iterator jtr = itr->AreaLengthMap.begin();jtr != itr->AreaLengthMap.end();++jtr)
					{
						oss << jtr->second << _T("\t");
					}
					CString rString(oss.str().c_str());
					rString.TrimRight();
					ofile << rString.operator LPCSTR() << std::endl;
				}
			}

			ofile.close();
		}
	}

	return ERROR_SUCCESS;
}
