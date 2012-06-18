// MccbSizeSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include <math.h>
#include "MccbSizeSettingDlg.h"
#include "ELoadDocData.h"
#include <util/ado/ADODB.h>
#include "Tables/CableCreationSettingTable.h"
#include "Tables/MCCBSizeAutoSelectionFunctionTable.h"
#include "commands/CommandAutoFillMCCBProp.h"
#include "afxwin.h"
#include "atltypes.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CReflectionStatic, CStatic)

BOOL CReflectionStatic::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	return GetParent()->GetParent()->SendMessage(WM_NOTIFY , wParam ,lParam);

	return CStatic::OnNotify(wParam,lParam , pResult);
}

static const string __VOLT_TYPES__[] = {"LV-1 (3PH)" , "LV-2 (3PH)" , "LV-11 (1PH)" , "LV-12 (1PH)" , "DC" };
static const CString __MOTOR_HEADERS__[] = {_T("       MCCB \n[AF]        [AT]") , _T("") , _T("   CAPACITY \n       [KW]") ,_T("") ,  _T("        FLC \n        [A]"), _T("") , _T("   FLC x SF \n        [A]") , _T("")};
static const CString __FEEDER_HEADERS__[] = {_T("       MCCB \n[AF]        [AT]") , _T("") , _T("   CAPACITY \n       [KVA]") ,_T("") ,  _T("        FLC \n        [A]"), _T("") , _T("   FLC x SF \n        [A]") , _T("")};

// CMccbSizeSettingDlg dialog

IMPLEMENT_DYNAMIC(CMccbSizeSettingDlg, CDialog)

CMccbSizeSettingDlg::CMccbSizeSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMccbSizeSettingDlg::IDD, pParent)
	, m_rCurVoltType(_T("LV-1 (3PH)"))
{
	m_bAllLoads = FALSE;
}

CMccbSizeSettingDlg::~CMccbSizeSettingDlg()
{
}

void CMccbSizeSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, ID_ADD_BUTTON, m_wndAddButton);
	DDX_Control(pDX, ID_SAVE_BUTTON, m_wndSaveButton);
	DDX_Control(pDX, IDC_BUTTON_DELETE, m_wndDeleteButton);
	DDX_Control(pDX, IDC_BUTTON_APPLY , m_wndApplyButton);
	DDX_Control(pDX, ID_APPLY_SAVE, m_wndApplyAllLoadsAndSaveButton);
	DDX_Control(pDX, IDCANCEL, m_wndCloseButton);

	DDX_Radio(pDX , IDC_ALL_LOAD_RADIO , m_bAllLoads);
}


BEGIN_MESSAGE_MAP(CMccbSizeSettingDlg, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CMccbSizeSettingDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_MCCB_SIZE_SETTING_CHECK, &CMccbSizeSettingDlg::OnBnClickedMccbSizeSettingCheck)
	ON_BN_CLICKED(ID_ADD_BUTTON, &CMccbSizeSettingDlg::OnBnClickedAddButton)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CMccbSizeSettingDlg::OnBnClickedButtonDelete)
	ON_BN_CLICKED(ID_SAVE_BUTTON, &CMccbSizeSettingDlg::OnBnClickedSaveButton)
	ON_BN_CLICKED(ID_APPLY_SAVE, &CMccbSizeSettingDlg::OnBnClickedApplySave)
	ON_BN_CLICKED(IDC_BUTTON_LV1, &CMccbSizeSettingDlg::OnBnClickedButtonLv1)
	ON_BN_CLICKED(IDC_BUTTON_LV2, &CMccbSizeSettingDlg::OnBnClickedButtonLv2)
	ON_BN_CLICKED(IDC_BUTTON_LV11, &CMccbSizeSettingDlg::OnBnClickedButtonLv11)
	ON_BN_CLICKED(IDC_BUTTON_LV12, &CMccbSizeSettingDlg::OnBnClickedButtonLv12)
	ON_BN_CLICKED(IDC_BUTTON_DC, &CMccbSizeSettingDlg::OnBnClickedButtonDc)
	ON_EN_CHANGE(IDC_EDIT_PF, &CMccbSizeSettingDlg::OnEnChangeEditPf)
	ON_EN_CHANGE(IDC_EDIT_SF, &CMccbSizeSettingDlg::OnEnChangeEditSf)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CMccbSizeSettingDlg::OnBnClickedButtonApply)
END_MESSAGE_MAP()


// CMccbSizeSettingDlg message handlers

BOOL CMccbSizeSettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_wndLV1Button.Attach(IDC_BUTTON_LV1 , this);
	m_wndLV2Button.Attach(IDC_BUTTON_LV2 , this);
	m_wndLV11Button.Attach(IDC_BUTTON_LV11 , this);
	m_wndLV12Button.Attach(IDC_BUTTON_LV12 , this);
	m_wndDCButton.Attach(IDC_BUTTON_DC , this);
	m_wndLV1Button.SetColor(RGB(0,0,0) , RGB(196,222,241));
	m_wndLV2Button.SetColor(RGB(0,0,0) , GetSysColor(COLOR_BTNFACE));
	m_wndLV11Button.SetColor(RGB(0,0,0) , GetSysColor(COLOR_BTNFACE));
	m_wndLV12Button.SetColor(RGB(0,0,0) , GetSysColor(COLOR_BTNFACE));
	m_wndDCButton.SetColor(RGB(0,0,0) , GetSysColor(COLOR_BTNFACE));

	CButton* pButton = (CButton*)(GetDlgItem(IDC_MCCB_SIZE_SETTING_CHECK));
	pButton->SetCheck(TRUE);

	DisplayVoltageSetting();

	CreateMccbTabCtrl();
	DisplayTableData();

	{
		m_wndLV1Button.SetFocus();
		CELoadDocData& docData = CELoadDocData::GetInstance();
		CELOADTable* pTable = docData.GetTableOf(CMCCBSizeAutoSelectionFunctionTable::GetTableName());
		if(pTable && (pTable->GetRecordCount() > 0))
		{
			SetDlgItemText(IDC_EDIT_PF , pTable->m_RecordEntry[0].FieldValueMap[_T("C_PF")].c_str());
			SetDlgItemText(IDC_EDIT_SF , pTable->m_RecordEntry[0].FieldValueMap[_T("C_SF")].c_str());
		}
	}
	
	//! BUTTON SETTING
	CELoadDocData & docData = CELoadDocData::GetInstance();
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));

	m_wndAddButton.SetTextHotColor(TextHotColor);
	m_wndSaveButton.SetTextHotColor(TextHotColor);
	m_wndDeleteButton.SetTextHotColor(TextHotColor);
	m_wndApplyAllLoadsAndSaveButton.SetTextHotColor(TextHotColor);
	m_wndCloseButton.SetTextHotColor(TextHotColor);

	m_wndApplyAllLoadsAndSaveButton.SetIcon(IDI_OK_18X18);
	m_wndCloseButton.SetIcon(IDI_CANCEL_18X18);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	

	@author BHK	

	@date 2009-04-20 오후 3:50:43	

	@param	

	@return		
*/
int CMccbSizeSettingDlg::DisplayVoltageSetting()
{
	CELoadDocData &docData = CELoadDocData::GetInstance();
	const string rMDBFilePath = docData.GetProjectMDBFilePath();
	{
		CLV3PHCableCreationSettingTable& LV3PHVoltageSettingTable = CLV3PHCableCreationSettingTable::GetInstance();
		LV3PHVoltageSettingTable.Load(rMDBFilePath);
		for(int i = 0;i < LV3PHVoltageSettingTable.GetRecordCount();++i)
		{
			const string rBusVolt = LV3PHVoltageSettingTable.m_RecordEntry[i].FieldValueMap[_T("C_BUS_VOLTAGE")];
			const string rLoadVolt= LV3PHVoltageSettingTable.m_RecordEntry[i].FieldValueMap[_T("C_LOAD_VOLTAGE")];
			if(rBusVolt.empty() || rLoadVolt.empty())
			{
				if(0 == i)
				{
					m_wndLV1Button.EnableWindow(FALSE);
					GetDlgItem(IDC_STATIC_LV1_BUS_VOLT)->EnableWindow(FALSE);
					GetDlgItem(IDC_STATIC_LV1_LOAD_VOLT)->EnableWindow(FALSE);
				}
				else if(1 == i)
				{
					m_wndLV2Button.EnableWindow(FALSE);
					GetDlgItem(IDC_STATIC_LV2_BUS_VOLT)->EnableWindow(FALSE);
					GetDlgItem(IDC_STATIC_LV2_LOAD_VOLT)->EnableWindow(FALSE);
				}
			}
			else
			{
				if(0 == i)
				{
					SetDlgItemText(IDC_STATIC_LV1_BUS_VOLT , rBusVolt.c_str());
					SetDlgItemText(IDC_STATIC_LV1_LOAD_VOLT , rLoadVolt.c_str());
				}
				else if(1 == i)
				{
					SetDlgItemText(IDC_STATIC_LV2_BUS_VOLT , rBusVolt.c_str());
					SetDlgItemText(IDC_STATIC_LV2_LOAD_VOLT , rLoadVolt.c_str());
				}
			}
		}
	}

	{
		CLV1PHCableCreationSettingTable& LV1PHVoltageSettingTable = CLV1PHCableCreationSettingTable::GetInstance();
		LV1PHVoltageSettingTable.Load(rMDBFilePath);

		for(int i = 0;i < LV1PHVoltageSettingTable.GetRecordCount();++i)
		{
			const string rBusVolt = LV1PHVoltageSettingTable.m_RecordEntry[i].FieldValueMap[_T("C_BUS_VOLTAGE")];
			const string rLoadVolt= LV1PHVoltageSettingTable.m_RecordEntry[i].FieldValueMap[_T("C_LOAD_VOLTAGE")];
			if(rBusVolt.empty() || rLoadVolt.empty())
			{
				if(0 == i)
				{
					m_wndLV11Button.EnableWindow(FALSE);
					GetDlgItem(IDC_STATIC_LV11_BUS_VOLT)->EnableWindow(FALSE);
					GetDlgItem(IDC_STATIC_LV11_LOAD_VOLT)->EnableWindow(FALSE);
				}
				else if(1 == i)
				{
					m_wndLV12Button.EnableWindow(FALSE);
					GetDlgItem(IDC_STATIC_LV12_BUS_VOLT)->EnableWindow(FALSE);
					GetDlgItem(IDC_STATIC_LV12_LOAD_VOLT)->EnableWindow(FALSE);
				}
			}
			else
			{
				if(0 == i)
				{
					SetDlgItemText(IDC_STATIC_LV11_BUS_VOLT , rBusVolt.c_str());
					SetDlgItemText(IDC_STATIC_LV11_LOAD_VOLT , rLoadVolt.c_str());
				}
				else if(1 == i)
				{
					SetDlgItemText(IDC_STATIC_LV12_BUS_VOLT , rBusVolt.c_str());
					SetDlgItemText(IDC_STATIC_LV12_LOAD_VOLT , rLoadVolt.c_str());
				}
			}
		}
	}

	{
		CDCCableCreationSettingTable& DCVoltageSettingTable = CDCCableCreationSettingTable::GetInstance();
		DCVoltageSettingTable.Load(rMDBFilePath);

		for(int i = 0;i < DCVoltageSettingTable.GetRecordCount();++i)
		{
			const string rBusVolt = DCVoltageSettingTable.m_RecordEntry[i].FieldValueMap[_T("C_BUS_VOLTAGE")];
			const string rLoadVolt= DCVoltageSettingTable.m_RecordEntry[i].FieldValueMap[_T("C_LOAD_VOLTAGE")];
			if(rBusVolt.empty() || rLoadVolt.empty())
			{
				if(0 == i)
				{
					m_wndDCButton.EnableWindow(FALSE);
					GetDlgItem(IDC_STATIC_DC_BUS_VOLT)->EnableWindow(FALSE);
					GetDlgItem(IDC_STATIC_DC_LOAD_VOLT)->EnableWindow(FALSE);
				}
			}
			else
			{
				if(0 == i)
				{
					SetDlgItemText(IDC_STATIC_DC_BUS_VOLT , rBusVolt.c_str());
					SetDlgItemText(IDC_STATIC_DC_LOAD_VOLT , rLoadVolt.c_str());
				}
			}
		}
	}

	return ERROR_SUCCESS;
}

BOOL CMccbSizeSettingDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		///if(pMsg->wParam == VK_ESCAPE) return TRUE;
		if((pMsg->wParam == VK_RETURN) && 
			((GetDlgItem(IDC_EDIT_PF)->GetSafeHwnd() == pMsg->hwnd) || (GetDlgItem(IDC_EDIT_SF)->GetSafeHwnd() == pMsg->hwnd))) return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

/**
	@brief	

	@author BHK	

	@date 2009-04-23 오전 11:30:37	

	@param	

	@return		
*/
BOOL CMccbSizeSettingDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR* pNMHDR = (NMHDR*)lParam;
	pNMHDR->hwndFrom;
	
	if((wParam == m_wndDCGridCtrl.GetDlgCtrlID()) || (wParam == m_wndLV1GridCtrl.GetDlgCtrlID()) || 
		(wParam == m_wndLV11GridCtrl.GetDlgCtrlID()) || (wParam == m_wndLV12GridCtrl.GetDlgCtrlID()) ||
		(wParam == m_wndDCMGridCtrl.GetDlgCtrlID()) || (wParam == m_wndLV1MGridCtrl.GetDlgCtrlID()) || 
		(wParam == m_wndLV11MGridCtrl.GetDlgCtrlID()) || (wParam == m_wndLV12MGridCtrl.GetDlgCtrlID()))
	{
		GV_DISPINFO* dispinfo = (GV_DISPINFO*)(lParam);
		if(dispinfo->hdr.code == GVN_ENDLABELEDIT)
		{
			const int nRow = dispinfo->item.row;
			const int nCol = dispinfo->item.col;

			CGridCtrl* pGridCtrl = NULL;
			if(wParam == m_wndDCGridCtrl.GetDlgCtrlID()) 
				pGridCtrl = &m_wndDCGridCtrl;
			else if(wParam == m_wndLV1GridCtrl.GetDlgCtrlID())
				pGridCtrl = &m_wndLV1GridCtrl;
			else if(wParam == m_wndLV11GridCtrl.GetDlgCtrlID())
				pGridCtrl = &m_wndLV11GridCtrl;
			else if(wParam == m_wndLV12GridCtrl.GetDlgCtrlID())
				pGridCtrl = &m_wndLV12GridCtrl;
			else if(wParam == m_wndDCMGridCtrl.GetDlgCtrlID()) 
				pGridCtrl = &m_wndDCMGridCtrl;
			else if(wParam == m_wndLV1MGridCtrl.GetDlgCtrlID())
				pGridCtrl = &m_wndLV1MGridCtrl;
			else if(wParam == m_wndLV11MGridCtrl.GetDlgCtrlID())
				pGridCtrl = &m_wndLV11MGridCtrl;
			else if(wParam == m_wndLV12MGridCtrl.GetDlgCtrlID())
				pGridCtrl = &m_wndLV12MGridCtrl;

			ChangeFLCAndSF(pGridCtrl , nRow);
		}
	}

	return CDialog::OnNotify(wParam, lParam, pResult);
}

/**
	@brief	CREATE MCCB TAB CTRL 

	@author KHS	

	@return		
**/
int CMccbSizeSettingDlg::CreateMccbTabCtrl(void)
{
	CRect rect;
	GetDlgItem(IDC_MCCB_SIZE_SETTING_STATIC)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	
	if (!m_wndTabCtrl.Create (CMFCTabCtrl::STYLE_3D_VS2005, rect , this, 0x110))
	{
		TRACE0("Failed to create Properies Tab \n");
		return ERROR_BAD_ENVIRONMENT;      // fail to create
	}

	m_wndTabCtrl.SetWindowPos(&(CWnd::wndTop) , 0,0,0,0,SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE);
	m_wndTabCtrl.SetLocation(CMFCTabCtrl::LOCATION_TOP);
	m_wndTabCtrl.SetActiveTabBoldFont(TRUE);
	m_wndTabCtrl.EnableAutoColor(TRUE);
	
	GetDlgItem(IDC_MCCB_SIZE_SETTING_STATIC)->GetWindowRect(&rect);
	m_wndTabCtrl.ScreenToClient(&rect);

	rect.top += 30;
	m_wndMotorStatic.Create(_T(""), NULL, rect, &m_wndTabCtrl);
	m_wndFeederStatic.Create(_T(""), NULL, rect, &m_wndTabCtrl);

	m_wndTabCtrl.AddTab(&m_wndMotorStatic, _T("MOTOR"));
	m_wndTabCtrl.AddTab(&m_wndFeederStatic, _T("FEEDER"));

	if(ERROR_SUCCESS == CreateLV1GridCtrl())
	{
		//m_wndTabCtrl.AddTab(&m_wndLV1MGridCtrl,_T("MOTOR"));
		//m_wndTabCtrl.AddTab(&m_wndLV1GridCtrl,_T("FEEDER"));
		//m_wndLV2GridCtrl.ShowWindow(SW_NORMAL);
	}
	if(ERROR_SUCCESS == CreateLV2GridCtrl())
	{
		//m_wndTabCtrl.AddTab(&m_wndLV2MGridCtrl,_T("MOTOR"));
		//m_wndTabCtrl.AddTab(&m_wndLV2GridCtrl,_T("FEEDER"));
		m_wndLV2GridCtrl.ShowWindow(SW_HIDE);
		m_wndLV2MGridCtrl.ShowWindow(SW_HIDE);
	}
	if(ERROR_SUCCESS == CreateLV11GridCtrl())
	{
		//m_wndTabCtrl.AddTab(&m_wndLV11MGridCtrl,_T("MOTOR"));
		//m_wndTabCtrl.AddTab(&m_wndLV11GridCtrl,_T("FEEDER"));
		m_wndLV11GridCtrl.ShowWindow(SW_HIDE);
		m_wndLV11MGridCtrl.ShowWindow(SW_HIDE);
	}
	if(ERROR_SUCCESS == CreateLV12GridCtrl())
	{
		//m_wndTabCtrl.AddTab(&m_wndLV12MGridCtrl,_T("MOTOR"));
		//m_wndTabCtrl.AddTab(&m_wndLV12GridCtrl,_T("FEEDER"));
		m_wndLV12GridCtrl.ShowWindow(SW_HIDE);
		m_wndLV12MGridCtrl.ShowWindow(SW_HIDE);
	}
	if(ERROR_SUCCESS == CreateDCGridCtrl())
	{
		//m_wndTabCtrl.AddTab(&m_wndDCMGridCtrl,_T("MOTOR"));
		//m_wndTabCtrl.AddTab(&m_wndDCGridCtrl,_T("FEEDER"));
		m_wndDCGridCtrl.ShowWindow(SW_HIDE);
		m_wndDCMGridCtrl.ShowWindow(SW_HIDE);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	Create LV1 VOLT TYPE Grid Ctrl

	@author KHS	

	@return		
**/
int CMccbSizeSettingDlg::CreateLV1GridCtrl(void)
{
	CRect rect;
	m_wndFeederStatic.GetWindowRect(&rect);
	m_wndFeederStatic.ScreenToClient(&rect);

	//! FEEDER
	if(m_wndLV1GridCtrl.Create(rect , &m_wndFeederStatic , 100))
	{
		m_wndLV1GridCtrl.SetColumnCount(sizeof(__FEEDER_HEADERS__) / sizeof(__FEEDER_HEADERS__[0]));
		m_wndLV1GridCtrl.SetFixedRowCount(1);

		m_wndLV1GridCtrl.SetEditable(TRUE);
		m_wndLV1GridCtrl.SetRowResize(FALSE);
		m_wndLV1GridCtrl.SetColumnResize(TRUE);
		m_wndLV1GridCtrl.EnableTitleTips(FALSE);

		// 해더
		const int nColumnCount = m_wndLV1GridCtrl.GetColumnCount();
		for(int i = 0;i < nColumnCount;++i)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = 0;
			Item.col = i;
			Item.nFormat = DT_LEFT|DT_WORDBREAK | DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX;
			Item.strText = __FEEDER_HEADERS__[i];
			m_wndLV1GridCtrl.SetItem(&Item);
		}

		m_wndLV1GridCtrl.AutoSizeColumns();
		m_wndLV1GridCtrl.AutoSizeRows();

		m_wndLV1GridCtrl.MergeCells(0, 0 , 0 , 1);
		m_wndLV1GridCtrl.MergeCells(0, 2 , 0 , 3);
		m_wndLV1GridCtrl.MergeCells(0, 4 , 0 , 5);
		m_wndLV1GridCtrl.MergeCells(0, 6 , 0 , 7);

	}
	
	//! MOTOR
	if(m_wndLV1MGridCtrl.Create(rect , &m_wndMotorStatic , 106))
	{
		m_wndLV1MGridCtrl.SetColumnCount(sizeof(__MOTOR_HEADERS__) / sizeof(__MOTOR_HEADERS__[0]));
		m_wndLV1MGridCtrl.SetFixedRowCount(1);

		m_wndLV1MGridCtrl.SetEditable(TRUE);
		m_wndLV1MGridCtrl.SetRowResize(FALSE);
		m_wndLV1MGridCtrl.SetColumnResize(TRUE);
		m_wndLV1MGridCtrl.EnableTitleTips(FALSE);

		// 해더
		const int nColumnCount = m_wndLV1MGridCtrl.GetColumnCount();
		for(int i = 0;i < nColumnCount;++i)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = 0;
			Item.col = i;
			Item.nFormat = DT_LEFT|DT_WORDBREAK | DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX;
			Item.strText = __MOTOR_HEADERS__[i];
			m_wndLV1MGridCtrl.SetItem(&Item);
		}

		m_wndLV1MGridCtrl.AutoSizeColumns();
		m_wndLV1MGridCtrl.AutoSizeRows();

		m_wndLV1MGridCtrl.MergeCells(0, 0 , 0 , 1);
		m_wndLV1MGridCtrl.MergeCells(0, 2 , 0 , 3);
		m_wndLV1MGridCtrl.MergeCells(0, 4 , 0 , 5);
		m_wndLV1MGridCtrl.MergeCells(0, 6 , 0 , 7);

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	Create 3Ph Feeder Grid Ctrl

	@author KHS	

	@return		
**/
int CMccbSizeSettingDlg::CreateLV2GridCtrl(void)
{
	CRect rect;
	m_wndFeederStatic.GetWindowRect(&rect);
	m_wndFeederStatic.ScreenToClient(&rect);
	//!FEEDER
	if(m_wndLV2GridCtrl.Create(rect , &m_wndFeederStatic , 101))
	{
		m_wndLV2GridCtrl.SetColumnCount(sizeof(__FEEDER_HEADERS__) / sizeof(__FEEDER_HEADERS__[0]));
		m_wndLV2GridCtrl.SetFixedRowCount(1);

		m_wndLV2GridCtrl.SetEditable(TRUE);
		m_wndLV2GridCtrl.SetRowResize(FALSE);
		m_wndLV2GridCtrl.SetColumnResize(TRUE);
		m_wndLV2GridCtrl.EnableTitleTips(FALSE);
		
		// 해더
		const int nColumnCount = m_wndLV2GridCtrl.GetColumnCount();
		for(int i = 0;i < nColumnCount;++i)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = 0;
			Item.col = i;
			Item.nFormat = DT_LEFT|DT_WORDBREAK | DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX;
			Item.strText = __FEEDER_HEADERS__[i];
			m_wndLV2GridCtrl.SetItem(&Item);
		}

		m_wndLV2GridCtrl.AutoSizeColumns();
		m_wndLV2GridCtrl.AutoSizeRows();

		m_wndLV2GridCtrl.MergeCells(0, 0 , 0 , 1);
		m_wndLV2GridCtrl.MergeCells(0, 2 , 0 , 3);
		m_wndLV2GridCtrl.MergeCells(0, 4 , 0 , 5);
		m_wndLV2GridCtrl.MergeCells(0, 6 , 0 , 7);
	}
	
	//! MOTOR
	if(m_wndLV2MGridCtrl.Create(rect , &m_wndMotorStatic , 107))
	{
		m_wndLV2MGridCtrl.SetColumnCount(sizeof(__MOTOR_HEADERS__) / sizeof(__MOTOR_HEADERS__[0]));
		m_wndLV2MGridCtrl.SetFixedRowCount(1);

		m_wndLV2MGridCtrl.SetEditable(TRUE);
		m_wndLV2MGridCtrl.SetRowResize(FALSE);
		m_wndLV2MGridCtrl.SetColumnResize(TRUE);
		m_wndLV2MGridCtrl.EnableTitleTips(FALSE);
		
		// 해더
		const int nColumnCount = m_wndLV2MGridCtrl.GetColumnCount();
		for(int i = 0;i < nColumnCount;++i)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = 0;
			Item.col = i;
			Item.nFormat = DT_LEFT|DT_WORDBREAK | DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX;
			Item.strText = __MOTOR_HEADERS__[i];
			m_wndLV2MGridCtrl.SetItem(&Item);
		}

		m_wndLV2MGridCtrl.AutoSizeColumns();
		m_wndLV2MGridCtrl.AutoSizeRows();

		m_wndLV2MGridCtrl.MergeCells(0, 0 , 0 , 1);
		m_wndLV2MGridCtrl.MergeCells(0, 2 , 0 , 3);
		m_wndLV2MGridCtrl.MergeCells(0, 4 , 0 , 5);
		m_wndLV2MGridCtrl.MergeCells(0, 6 , 0 , 7);

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	Create LV11 VOLT TYPE Grid Ctrl

	@author KHS	

	@return		
**/
int CMccbSizeSettingDlg::CreateLV11GridCtrl(void)
{
	CRect rect;
	m_wndFeederStatic.GetWindowRect(&rect);
	m_wndFeederStatic.ScreenToClient(&rect);

	//! FEEDER
	if(m_wndLV11GridCtrl.Create(rect , &m_wndFeederStatic , 102))
	{
		m_wndLV11GridCtrl.SetColumnCount(sizeof(__FEEDER_HEADERS__) / sizeof(__FEEDER_HEADERS__[0]));
		m_wndLV11GridCtrl.SetFixedRowCount(1);

		m_wndLV11GridCtrl.SetEditable(TRUE);
		m_wndLV11GridCtrl.SetRowResize(FALSE);
		m_wndLV11GridCtrl.SetColumnResize(TRUE);
		m_wndLV11GridCtrl.EnableTitleTips(FALSE);
		
		// 해더
		const int nColumnCount = m_wndLV11GridCtrl.GetColumnCount();
		for(int i = 0;i < nColumnCount;++i)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = 0;
			Item.col = i;
			Item.nFormat = DT_LEFT|DT_WORDBREAK | DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX;
			Item.strText = __FEEDER_HEADERS__[i];
			m_wndLV11GridCtrl.SetItem(&Item);
		}

		m_wndLV11GridCtrl.AutoSizeColumns();
		m_wndLV11GridCtrl.AutoSizeRows();

		m_wndLV11GridCtrl.MergeCells(0, 0 , 0 , 1);
		m_wndLV11GridCtrl.MergeCells(0, 2 , 0 , 3);
		m_wndLV11GridCtrl.MergeCells(0, 4 , 0 , 5);
		m_wndLV11GridCtrl.MergeCells(0, 6 , 0 , 7);
	}
	
	//! MOTOR
	if(m_wndLV11MGridCtrl.Create(rect , &m_wndMotorStatic , 108))
	{
		m_wndLV11MGridCtrl.SetColumnCount(sizeof(__MOTOR_HEADERS__) / sizeof(__MOTOR_HEADERS__[0]));
		m_wndLV11MGridCtrl.SetFixedRowCount(1);

		m_wndLV11MGridCtrl.SetEditable(TRUE);
		m_wndLV11MGridCtrl.SetRowResize(FALSE);
		m_wndLV11MGridCtrl.SetColumnResize(TRUE);
		m_wndLV11MGridCtrl.EnableTitleTips(FALSE);
		
		// 해더
		const int nColumnCount = m_wndLV11MGridCtrl.GetColumnCount();
		for(int i = 0;i < nColumnCount;++i)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = 0;
			Item.col = i;
			Item.nFormat = DT_LEFT|DT_WORDBREAK | DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX;
			Item.strText = __MOTOR_HEADERS__[i];
			m_wndLV11MGridCtrl.SetItem(&Item);
		}

		m_wndLV11MGridCtrl.AutoSizeColumns();
		m_wndLV11MGridCtrl.AutoSizeRows();

		m_wndLV11MGridCtrl.MergeCells(0, 0 , 0 , 1);
		m_wndLV11MGridCtrl.MergeCells(0, 2 , 0 , 3);
		m_wndLV11MGridCtrl.MergeCells(0, 4 , 0 , 5);
		m_wndLV11MGridCtrl.MergeCells(0, 6 , 0 , 7);

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	Create LV11 VOLT TYPE Grid Ctrl

	@author KHS	

	@return		
**/
int CMccbSizeSettingDlg::CreateLV12GridCtrl(void)
{
	CRect rect;
	m_wndFeederStatic.GetWindowRect(&rect);
	m_wndFeederStatic.ScreenToClient(&rect);

	//! FEEDER
	if(m_wndLV12GridCtrl.Create(rect , &m_wndFeederStatic , 103))
	{
		m_wndLV12GridCtrl.SetColumnCount(sizeof(__FEEDER_HEADERS__) / sizeof(__FEEDER_HEADERS__[0]));
		m_wndLV12GridCtrl.SetFixedRowCount(1);

		m_wndLV12GridCtrl.SetEditable(TRUE);
		m_wndLV12GridCtrl.SetRowResize(FALSE);
		m_wndLV12GridCtrl.SetColumnResize(TRUE);
		m_wndLV12GridCtrl.EnableTitleTips(FALSE);
		
		// 해더
		const int nColumnCount = m_wndLV12GridCtrl.GetColumnCount();
		for(int i = 0;i < nColumnCount;++i)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = 0;
			Item.col = i;
			Item.nFormat = DT_LEFT|DT_WORDBREAK | DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX;
			Item.strText = __FEEDER_HEADERS__[i];
			m_wndLV12GridCtrl.SetItem(&Item);
		}

		m_wndLV12GridCtrl.AutoSizeColumns();
		m_wndLV12GridCtrl.AutoSizeRows();

		m_wndLV12GridCtrl.MergeCells(0, 0 , 0 , 1);
		m_wndLV12GridCtrl.MergeCells(0, 2 , 0 , 3);
		m_wndLV12GridCtrl.MergeCells(0, 4 , 0 , 5);
		m_wndLV12GridCtrl.MergeCells(0, 6 , 0 , 7);
	}
	
	//! MOTOR
	if(m_wndLV12MGridCtrl.Create(rect , &m_wndMotorStatic , 109))
	{
		m_wndLV12MGridCtrl.SetColumnCount(sizeof(__MOTOR_HEADERS__) / sizeof(__MOTOR_HEADERS__[0]));
		m_wndLV12MGridCtrl.SetFixedRowCount(1);

		m_wndLV12MGridCtrl.SetEditable(TRUE);
		m_wndLV12MGridCtrl.SetRowResize(FALSE);
		m_wndLV12MGridCtrl.SetColumnResize(TRUE);
		m_wndLV12MGridCtrl.EnableTitleTips(FALSE);
		
		// 해더
		const int nColumnCount = m_wndLV12MGridCtrl.GetColumnCount();
		for(int i = 0;i < nColumnCount;++i)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = 0;
			Item.col = i;
			Item.nFormat = DT_LEFT|DT_WORDBREAK | DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX;
			Item.strText = __MOTOR_HEADERS__[i];
			m_wndLV12MGridCtrl.SetItem(&Item);
		}

		m_wndLV12MGridCtrl.AutoSizeColumns();
		m_wndLV12MGridCtrl.AutoSizeRows();

		m_wndLV12MGridCtrl.MergeCells(0, 0 , 0 , 1);
		m_wndLV12MGridCtrl.MergeCells(0, 2 , 0 , 3);
		m_wndLV12MGridCtrl.MergeCells(0, 4 , 0 , 5);
		m_wndLV12MGridCtrl.MergeCells(0, 6 , 0 , 7);

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	Create DC Feede rGrid Ctrl

	@author KHS	

	@return		
**/
int CMccbSizeSettingDlg::CreateDCGridCtrl(void)
{
	CRect rect;
	m_wndFeederStatic.GetWindowRect(&rect);
	m_wndFeederStatic.ScreenToClient(&rect);

	//! FEEDER
	if(m_wndDCGridCtrl.Create(rect , &m_wndFeederStatic , 104))
	{
		m_wndDCGridCtrl.SetColumnCount(sizeof(__FEEDER_HEADERS__) / sizeof(__FEEDER_HEADERS__[0]));
		m_wndDCGridCtrl.SetFixedRowCount(1);

		m_wndDCGridCtrl.SetEditable(TRUE);
		m_wndDCGridCtrl.SetRowResize(FALSE);
		m_wndDCGridCtrl.SetColumnResize(TRUE);
		m_wndDCGridCtrl.EnableTitleTips(FALSE);
		
		// 해더
		const int nColumnCount = m_wndDCGridCtrl.GetColumnCount();
		for(int i = 0;i < nColumnCount;++i)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = 0;
			Item.col = i;
			Item.nFormat = DT_LEFT|DT_WORDBREAK | DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX;
			Item.strText = __FEEDER_HEADERS__[i];
			m_wndDCGridCtrl.SetItem(&Item);
		}

		m_wndDCGridCtrl.AutoSizeColumns();
		m_wndDCGridCtrl.AutoSizeRows();

		m_wndDCGridCtrl.MergeCells(0, 0 , 0 , 1);
		m_wndDCGridCtrl.MergeCells(0, 2 , 0 , 3);
		m_wndDCGridCtrl.MergeCells(0, 4 , 0 , 5);
		m_wndDCGridCtrl.MergeCells(0, 6 , 0 , 7);
	}
	
	//! MOTOR
	if(m_wndDCMGridCtrl.Create(rect , &m_wndMotorStatic , 110))
	{
		m_wndDCMGridCtrl.SetColumnCount(sizeof(__MOTOR_HEADERS__) / sizeof(__MOTOR_HEADERS__[0]));
		m_wndDCMGridCtrl.SetFixedRowCount(1);

		m_wndDCMGridCtrl.SetEditable(TRUE);
		m_wndDCMGridCtrl.SetRowResize(FALSE);
		m_wndDCMGridCtrl.SetColumnResize(TRUE);
		m_wndDCMGridCtrl.EnableTitleTips(FALSE);
		
		// 해더
		const int nColumnCount = m_wndDCMGridCtrl.GetColumnCount();
		for(int i = 0;i < nColumnCount;++i)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = 0;
			Item.col = i;
			Item.nFormat = DT_LEFT|DT_WORDBREAK | DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX;
			Item.strText = __MOTOR_HEADERS__[i];
			m_wndDCMGridCtrl.SetItem(&Item);
		}

		m_wndDCMGridCtrl.AutoSizeColumns();
		m_wndDCMGridCtrl.AutoSizeRows();

		m_wndDCMGridCtrl.MergeCells(0, 0 , 0 , 1);
		m_wndDCMGridCtrl.MergeCells(0, 2 , 0 , 3);
		m_wndDCMGridCtrl.MergeCells(0, 4 , 0 , 5);
		m_wndDCMGridCtrl.MergeCells(0, 6 , 0 , 7);

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	INSERT ITEM IN GRID CTRL

	@author KHS	

	@return		
**/
int CMccbSizeSettingDlg::AddItemInGridCtrl(CGridCtrl& GridCtrl, const int& nRow, CELOADTable::Record& record)
{
	UpdateData();

	CELoadDocData& docData = CELoadDocData::GetInstance();
	const COLORREF ReadOnlyColor   = docData.GetColorCode("READ_ONLY_BACK_COLOR");
	const COLORREF ItemIDBackColor = docData.GetColorCode("ITEM_ID_BACK_COLOR");

	GridCtrl.SetRowCount( GridCtrl.GetRowCount() + 1);
	const int nColCount = GridCtrl.GetColumnCount();
	
	const string rMCCBAF = record.FieldValueMap[_T("C_MCCB_AF")];
	const string rMCCBAT = record.FieldValueMap[_T("C_MCCB_AT")];
	const string rMinCapacity = record.FieldValueMap[_T("C_MIN_CAPACITY")];
	const string rMaxCapacity = record.FieldValueMap[_T("C_MAX_CAPACITY")];
	const string rMinFLC = record.FieldValueMap[_T("C_MIN_FLC")];
	const string rMaxFLC = record.FieldValueMap[_T("C_MAX_FLC")];
	const string rMinFLC_SF = record.FieldValueMap[_T("C_MIN_FLC_SF")];
	const string rMaxFLC_SF = record.FieldValueMap[_T("C_MAX_FLC_SF")];
	for(int i = 0;i < nColCount;++i)
	{
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.row = GridCtrl.GetRowCount() - 1;
		Item.col = i;
		Item.nFormat = DT_RIGHT|DT_WORDBREAK | DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX;

		if(0 == i)
		{
			Item.strText = rMCCBAF.c_str();
		}
		else if(1 == i)
		{
			Item.strText = rMCCBAT.c_str();
		}
		else if(2 == i)
		{
			if(rMinCapacity.empty())
			{
				if(1 == Item.row)	//! 맨 첫번째 줄이면...
					Item.strText = _T("0");
				else
				{
					const CString rItemText = GridCtrl.GetItemText(Item.row - 1 , i + 1);
					const double nPrevMaxCapacity = atof(rItemText);

					vector<string> oResult;
					CTokenizer<CIsFromString>::Tokenize(oResult , rItemText.operator LPCSTR() , CIsFromString("."));
					double nMinCapacity = (2 == oResult.size()) ? 
						nPrevMaxCapacity + pow(10.f , -1*int(oResult[1].length())) : 
						nPrevMaxCapacity + 1.f;

					ostringstream oss;
					oss << nMinCapacity;
					Item.strText = oss.str().c_str();
					
				}
			}else	Item.strText = rMinCapacity.c_str();
			
			CGridCell* pCell = (CGridCell*)GridCtrl.GetCell(Item.row , Item.col);
			pCell->SetState(pCell->GetState() | GVIS_READONLY);
			pCell->SetBackClr(ReadOnlyColor);
		}
		else if(3 == i)
			Item.strText = rMaxCapacity.c_str();
		else if(4 == i)
		{
			Item.strText = rMinFLC.c_str();

			CGridCell* pCell = (CGridCell*)GridCtrl.GetCell(Item.row , Item.col);
			pCell->SetState(pCell->GetState() | GVIS_READONLY);
			pCell->SetBackClr(ItemIDBackColor);
		}
		else if(5 == i)
		{
			Item.strText = rMaxFLC.c_str();

			CGridCell* pCell = (CGridCell*)GridCtrl.GetCell(Item.row , Item.col);
			pCell->SetState(pCell->GetState() | GVIS_READONLY);
			pCell->SetBackClr(ItemIDBackColor);
		}
		else if(6 == i)
		{
			Item.strText = rMinFLC_SF.c_str();

			CGridCell* pCell = (CGridCell*)GridCtrl.GetCell(Item.row , Item.col);
			pCell->SetState(pCell->GetState() | GVIS_READONLY);
			pCell->SetBackClr(ItemIDBackColor);
		}
		else if(7 == i)
		{
			Item.strText = rMaxFLC_SF.c_str();

			CGridCell* pCell = (CGridCell*)GridCtrl.GetCell(Item.row , Item.col);
			pCell->SetState(pCell->GetState() | GVIS_READONLY);
			pCell->SetBackClr(ItemIDBackColor);
		}
		
		GridCtrl.SetItem(&Item);
		GridCtrl.SetColumnWidth( Item.col, 50);
	}
	GridCtrl.SetRowHeight(GridCtrl.GetRowCount() - 1, GridCtrl.GetRowHeight(0) * 0.7);

	GridCtrl.RedrawWindow();

	return ERROR_SUCCESS;
}

/**
	@brief	CANCEL

	@author KHS	

	@return		
**/
void CMccbSizeSettingDlg::OnBnClickedCancel()
{
	OnCancel();
}

/**
	@brief	disable all windows when check is setted.

	@author BHK	

	@date 2009-04-14 오전 11:37:29	

	@param	

	@return		
*/
void CMccbSizeSettingDlg::OnBnClickedMccbSizeSettingCheck()
{
	CButton* pButton = (CButton*)(GetDlgItem(IDC_MCCB_SIZE_SETTING_CHECK));
	if(pButton)
	{
		const BOOL bCheck = pButton->GetCheck();
		
		m_wndLV1GridCtrl.EnableWindow(bCheck);
		m_wndLV1GridCtrl.RedrawWindow();
		m_wndLV2GridCtrl.EnableWindow(bCheck);
		m_wndLV2GridCtrl.RedrawWindow();
		m_wndLV11GridCtrl.EnableWindow(bCheck);
		m_wndLV11GridCtrl.RedrawWindow();
		m_wndLV12GridCtrl.EnableWindow(bCheck);
		m_wndLV12GridCtrl.RedrawWindow();
		m_wndDCGridCtrl.EnableWindow(bCheck);
		m_wndDCGridCtrl.RedrawWindow();

		m_wndLV1MGridCtrl.EnableWindow(bCheck);
		m_wndLV1MGridCtrl.RedrawWindow();
		m_wndLV2MGridCtrl.EnableWindow(bCheck);
		m_wndLV2MGridCtrl.RedrawWindow();
		m_wndLV11MGridCtrl.EnableWindow(bCheck);
		m_wndLV11MGridCtrl.RedrawWindow();
		m_wndLV12MGridCtrl.EnableWindow(bCheck);
		m_wndLV12MGridCtrl.RedrawWindow();
		m_wndDCMGridCtrl.EnableWindow(bCheck);
		m_wndDCMGridCtrl.RedrawWindow();

		GetDlgItem(IDC_EDIT_PF)->EnableWindow(bCheck);
		GetDlgItem(IDC_EDIT_SF)->EnableWindow(bCheck);

		m_wndAddButton.EnableWindow(bCheck);
		m_wndDeleteButton.EnableWindow(bCheck);
		m_wndSaveButton.EnableWindow(bCheck);
		m_wndApplyAllLoadsAndSaveButton.EnableWindow(bCheck);
	}
}

/**
	@brief	맨 마지막에 새로운 ROW를 추가한다.

	@author BHK	

	@date 2009-04-14 오후 1:36:52	

	@param	

	@return		
*/
void CMccbSizeSettingDlg::OnBnClickedAddButton()
{
	const int nTab = m_wndTabCtrl.GetActiveTab();
	CGridCtrl* pGridCtrl = GetGridCtrlOf(m_rCurVoltType, nTab);
	if(pGridCtrl)
	{
		CELOADTable::Record record;
		
		record.FieldValueMap.insert(make_pair(_T("C_LOAD_TYPE"),(0 == nTab)? _T("MOTOR") : _T("FEEDER")));
		record.FieldValueMap.insert(make_pair(_T("C_VOLTAGE_TYPE"),m_rCurVoltType));
		record.FieldValueMap.insert(make_pair(_T("C_MCCB_AF"),_T("")));
		record.FieldValueMap.insert(make_pair(_T("C_MCCB_AT"),_T("")));
		record.FieldValueMap.insert(make_pair(_T("C_MIN_CAPACITY"),_T("")));
		record.FieldValueMap.insert(make_pair(_T("C_MAX_CAPACITY"),_T("")));
		record.FieldValueMap.insert(make_pair(_T("C_MIN_FLC"),_T("")));
		record.FieldValueMap.insert(make_pair(_T("C_MAX_FLC"),_T("")));
		record.FieldValueMap.insert(make_pair(_T("C_MIN_FLC_SF"),_T("")));
		record.FieldValueMap.insert(make_pair(_T("C_MAX_FLC_SF"),_T("")));
		
		CString rString;
		GetDlgItemText(IDC_EDIT_PF , rString);
		record.FieldValueMap.insert(make_pair(_T("C_PF"),rString.operator LPCSTR()));
		GetDlgItemText(IDC_EDIT_SF , rString);
		record.FieldValueMap.insert(make_pair(_T("C_SF"),rString.operator LPCSTR()));

		AddItemInGridCtrl(*pGridCtrl , pGridCtrl->GetRowCount() , record);
		
		pGridCtrl->SetFocus();
		pGridCtrl->SetSelectedRange(pGridCtrl->GetRowCount() - 1, 0,pGridCtrl->GetRowCount() - 1, 0);
		pGridCtrl->SetFocusCell(pGridCtrl->GetRowCount() - 1, 0);
	}
}

/**
	@brief	선택한 ROW들을 삭제한다.

	@author BHK	

	@date 2009-04-14 오후 1:46:50	

	@param	

	@return		
*/
void CMccbSizeSettingDlg::OnBnClickedButtonDelete()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pTable = docData.GetTableOf(CMCCBSizeAutoSelectionFunctionTable::GetTableName());
	
	const int nTab = m_wndTabCtrl.GetActiveTab();
	CGridCtrl* pGridCtrl = GetGridCtrlOf(m_rCurVoltType, nTab);
	if(pGridCtrl && pTable)
	{
		pGridCtrl->SetFocus();
		CCellRange range = pGridCtrl->GetSelectedCellRange();
		for(int i = range.GetMaxRow();i >= range.GetMinRow();--i)
		{
			const string rMCCBAF1 = pGridCtrl->GetItemText(i , 0).operator LPCSTR();
			const string rMCCBAT1 = pGridCtrl->GetItemText(i , 1).operator LPCSTR();
			const string rMinCapacity1 = pGridCtrl->GetItemText(i , 2).operator LPCSTR();
			const string rMaxCapacity1 = pGridCtrl->GetItemText(i , 3).operator LPCSTR();
			const string rMinFLC1 = pGridCtrl->GetItemText(i , 4).operator LPCSTR();
			const string rMaxFLC1 = pGridCtrl->GetItemText(i , 5).operator LPCSTR();
			const string rMinFLCSF1 = pGridCtrl->GetItemText(i , 6).operator LPCSTR();
			const string rMaxFLCSF1 = pGridCtrl->GetItemText(i , 7).operator LPCSTR();

			pGridCtrl->DeleteRow(i);

			for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin();itr != pTable->m_RecordEntry.end();++itr)
			{
				const string rVoltType = itr->FieldValueMap[_T("C_VOLTAGE_TYPE")];
				const string rMCCBAF2 = itr->FieldValueMap[_T("C_MCCB_AF")];
				const string rMCCBAT2 = itr->FieldValueMap[_T("C_MCCB_AT")];
				const string rMinCapacity2 = itr->FieldValueMap[_T("C_MIN_CAPACITY")];
				const string rMaxCapacity2 = itr->FieldValueMap[_T("C_MAX_CAPACITY")];
				const string rMinFLC2 = itr->FieldValueMap[_T("C_MIN_FLC")];
				const string rMaxFLC2 = itr->FieldValueMap[_T("C_MAX_FLC")];
				const string rMinFLCSF2 = itr->FieldValueMap[_T("C_MIN_FLC_SF")];
				const string rMaxFLCSF2 = itr->FieldValueMap[_T("C_MAX_FLC_SF")];
				if((m_rCurVoltType == rVoltType) && (rMCCBAF1 == rMCCBAF2) && (rMCCBAT1 == rMCCBAT2) && 
					(rMinCapacity1 == rMinCapacity2) && (rMaxCapacity1 == rMaxCapacity2) && 
					(rMinFLC1 == rMinFLC2) && (rMaxFLC1 == rMaxFLC2) && 
					(rMinFLCSF1 == rMinFLCSF2) && (rMaxFLCSF1 == rMaxFLCSF2))
				{
					pTable->m_RecordEntry.erase(itr);
					break;
				}
			}
		}
		pGridCtrl->RedrawWindow();
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-04-14 오후 2:15:59	

	@param	

	@return		
*/
void CMccbSizeSettingDlg::OnBnClickedSaveButton()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pTable = docData.GetTableOf(CMCCBSizeAutoSelectionFunctionTable::GetTableName());
	if(pTable)
	{
		CString rString;
		GetDlgItemText(IDC_EDIT_PF , rString);
		const string rPF = rString.operator LPCSTR();
		GetDlgItemText(IDC_EDIT_SF , rString);
		const string rSF = rString.operator LPCSTR();

		pTable->Clear();
		for(int n = 0; n < m_wndTabCtrl.GetTabsNum();++n)
		{
			for(int i = 0;i < SIZE_OF_ARRAY(__VOLT_TYPES__);++i)
			{
				CGridCtrl* pGridCtrl = GetGridCtrlOf(__VOLT_TYPES__[i], n);
				if(pGridCtrl)
				{
					for(int j = 1;j < pGridCtrl->GetRowCount();++j)	//! 헤더 부분은 제외.
					{
						CELOADTable::Record record;
						
						record.FieldValueMap.insert(make_pair(_T("C_LOAD_TYPE") , (n == 1) ? _T("FEEDER") : _T("MOTOR")));
						record.FieldValueMap.insert(make_pair(_T("C_VOLTAGE_TYPE") , __VOLT_TYPES__[i]));
						record.FieldValueMap.insert(make_pair(_T("C_MCCB_AF") , pGridCtrl->GetItemText(j , 0).operator LPCSTR()));
						record.FieldValueMap.insert(make_pair(_T("C_MCCB_AT") , pGridCtrl->GetItemText(j , 1).operator LPCSTR()));
						record.FieldValueMap.insert(make_pair(_T("C_MIN_CAPACITY") , pGridCtrl->GetItemText(j , 2).operator LPCSTR()));
						record.FieldValueMap.insert(make_pair(_T("C_MAX_CAPACITY") , pGridCtrl->GetItemText(j , 3).operator LPCSTR()));
						record.FieldValueMap.insert(make_pair(_T("C_MIN_FLC") , pGridCtrl->GetItemText(j , 4).operator LPCSTR()));
						record.FieldValueMap.insert(make_pair(_T("C_MAX_FLC") , pGridCtrl->GetItemText(j , 5).operator LPCSTR()));
						record.FieldValueMap.insert(make_pair(_T("C_MIN_FLC_SF") , pGridCtrl->GetItemText(j , 6).operator LPCSTR()));
						record.FieldValueMap.insert(make_pair(_T("C_MAX_FLC_SF") , pGridCtrl->GetItemText(j , 7).operator LPCSTR()));
						record.FieldValueMap.insert(make_pair(_T("C_PF") , rPF));
						record.FieldValueMap.insert(make_pair(_T("C_SF") , rSF));

						pTable->Insert(record);
					}
				}
			}
		}
		pTable->Save();
	}
}

/**
	@brief	테이블의 내용을 그리드에 표시한다.

	@author BHK	

	@date 2009-04-14 오후 3:21:06	

	@param	

	@return		
*/
int CMccbSizeSettingDlg::DisplayTableData(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pTable = docData.GetTableOf(CMCCBSizeAutoSelectionFunctionTable::GetTableName());
	if(pTable)
	{
		for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin();itr != pTable->m_RecordEntry.end();++itr)
		{
			const CString rLoadType = itr->FieldValueMap[_T("C_LOAD_TYPE")].c_str();
			const string rVoltType = itr->FieldValueMap[_T("C_VOLTAGE_TYPE")];
			CGridCtrl* pGridCtrl = GetGridCtrlOf(rVoltType, (_T("FEEDER") == rLoadType)? 1 : 0 );
			if(pGridCtrl)
			{
				const CString rMinCapacity = itr->FieldValueMap[_T("C_MIN_CAPACITY")].c_str();
				const CString rMaxCapacity = itr->FieldValueMap[_T("C_MAX_CAPACITY")].c_str();
				const CString rFlcFactor   = itr->FieldValueMap[_T("C_FLC_FACTOR")].c_str();
				const CString rAF          = itr->FieldValueMap[_T("C_MCCB_AF")].c_str();
				const CString rAT          = itr->FieldValueMap[_T("C_MCCB_AT")].c_str();
						
				AddItemInGridCtrl(*pGridCtrl , -1 , *itr);
			}
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	Table 저장.

	@author BHK	

	@date 2009-04-14 오후 4:21:07	

	@param	

	@return		
*/
void CMccbSizeSettingDlg::OnBnClickedApplySave()
{
	//! 테이블을 저장합니다.
	OnBnClickedSaveButton();

	AfxMessageBox("Saved!!",MB_OK|MB_ICONINFORMATION);
}

/**
	@brief	

	@author BHK	

	@date 2009-04-20 오후 2:35:44	

	@param	

	@return		
*/
CGridCtrl* CMccbSizeSettingDlg::GetGridCtrlOf(const string& rVoltType, const int& nTab)
{
	CGridCtrl* pGridCtrl = NULL;
	if(0 == nTab)
	{
		//! MOTOR

		if(_T("LV-1 (3PH)") == rVoltType)
			pGridCtrl = &m_wndLV1MGridCtrl;
		else if(_T("LV-2 (3PH)") == rVoltType)
			pGridCtrl = &m_wndLV2MGridCtrl;
		else if(_T("LV-11 (1PH)") == rVoltType)
			pGridCtrl = &m_wndLV11MGridCtrl;
		else if(_T("LV-12 (1PH)") == rVoltType)
			pGridCtrl = &m_wndLV12MGridCtrl;
		else if(_T("DC") == rVoltType)
			pGridCtrl = &m_wndDCMGridCtrl;
	}
	else if(1 == nTab)
	{
		//! FEEDER

		if(_T("LV-1 (3PH)") == rVoltType)
			pGridCtrl = &m_wndLV1GridCtrl;
		else if(_T("LV-2 (3PH)") == rVoltType)
			pGridCtrl = &m_wndLV2GridCtrl;
		else if(_T("LV-11 (1PH)") == rVoltType)
			pGridCtrl = &m_wndLV11GridCtrl;
		else if(_T("LV-12 (1PH)") == rVoltType)
			pGridCtrl = &m_wndLV12GridCtrl;
		else if(_T("DC") == rVoltType)
			pGridCtrl = &m_wndDCGridCtrl;
	}

	return pGridCtrl;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-20 오후 4:48:17	

	@param	

	@return		
*/
double CMccbSizeSettingDlg::GetLoadVoltOf(const string& rVoltType)
{
	CString rString;
	if(_T("LV-1 (3PH)") == rVoltType)
		GetDlgItemText(IDC_STATIC_LV1_LOAD_VOLT , rString);
	else if(_T("LV-2 (3PH)") == rVoltType)
		GetDlgItemText(IDC_STATIC_LV2_LOAD_VOLT , rString);
	else if(_T("LV-11 (1PH)") == rVoltType)
		GetDlgItemText(IDC_STATIC_LV11_LOAD_VOLT , rString);
	else if(_T("LV-12 (1PH)") == rVoltType)
		GetDlgItemText(IDC_STATIC_LV12_LOAD_VOLT , rString);
	else if(_T("DC") == rVoltType)
		GetDlgItemText(IDC_STATIC_DC_LOAD_VOLT , rString);

	return atof(rString);
}

/**
	@brief	

	@author BHK	

	@date 2009-04-20 오후 2:52:17	

	@param	

	@return		
*/
void CMccbSizeSettingDlg::OnBnClickedButtonLv1()
{
	const int nTab = m_wndTabCtrl.GetActiveTab();
	m_wndMotorStatic.ShowWindow((nTab == 0)?SW_SHOW : SW_HIDE);
	m_wndFeederStatic.ShowWindow((nTab == 1)?SW_SHOW : SW_HIDE);

	CString rString;
	m_wndLV1Button.GetWindowText(rString);
	m_rCurVoltType = rString.operator LPCSTR();
	m_wndLV1Button.SetState(TRUE);
	m_wndLV1GridCtrl.ShowWindow(SW_SHOW);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	m_wndLV2GridCtrl.ShowWindow(SW_HIDE);
	m_wndLV2Button.SetState(FALSE);
	m_wndLV2Button.SetBGColor(GetSysColor(COLOR_BTNFACE) ,TRUE);
	m_wndLV11GridCtrl.ShowWindow(SW_HIDE);
	m_wndLV11Button.SetState(FALSE);
	m_wndLV11Button.SetBGColor(GetSysColor(COLOR_BTNFACE) ,TRUE);
	m_wndLV12GridCtrl.ShowWindow(SW_HIDE);
	m_wndLV12Button.SetState(FALSE);
	m_wndLV12Button.SetBGColor(GetSysColor(COLOR_BTNFACE) ,TRUE);
	m_wndDCGridCtrl.ShowWindow(SW_HIDE);
	m_wndDCButton.SetState(FALSE);
	m_wndDCButton.SetBGColor(GetSysColor(COLOR_BTNFACE) ,TRUE);

	m_wndLV1MGridCtrl.ShowWindow(SW_SHOW);
	m_wndLV2MGridCtrl.ShowWindow(SW_HIDE);
	m_wndLV11MGridCtrl.ShowWindow(SW_HIDE);
	m_wndLV12MGridCtrl.ShowWindow(SW_HIDE);
	m_wndDCMGridCtrl.ShowWindow(SW_HIDE);
}

/**
	@brief	

	@author BHK	

	@date 2009-04-20 오후 2:56:59	

	@param	

	@return		
*/
void CMccbSizeSettingDlg::OnBnClickedButtonLv2()
{
	const int nTab = m_wndTabCtrl.GetActiveTab();
	m_wndMotorStatic.ShowWindow((nTab == 0)?SW_SHOW : SW_HIDE);
	m_wndFeederStatic.ShowWindow((nTab == 1)?SW_SHOW : SW_HIDE);

	CString rString;
	m_wndLV2Button.GetWindowText(rString);
	m_rCurVoltType = rString.operator LPCSTR();
	m_wndLV2Button.SetState(TRUE);
	m_wndLV2GridCtrl.ShowWindow(SW_SHOW);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	m_wndLV1GridCtrl.ShowWindow(SW_HIDE);
	m_wndLV1Button.SetState(FALSE);
	m_wndLV1Button.SetBGColor(GetSysColor(COLOR_BTNFACE) ,TRUE);
	m_wndLV11GridCtrl.ShowWindow(SW_HIDE);
	m_wndLV11Button.SetState(FALSE);
	m_wndLV11Button.SetBGColor(GetSysColor(COLOR_BTNFACE) ,TRUE);
	m_wndLV12GridCtrl.ShowWindow(SW_HIDE);
	m_wndLV12Button.SetState(FALSE);
	m_wndLV12Button.SetBGColor(GetSysColor(COLOR_BTNFACE) ,TRUE);
	m_wndDCGridCtrl.ShowWindow(SW_HIDE);
	m_wndDCButton.SetState(FALSE);
	m_wndDCButton.SetBGColor(GetSysColor(COLOR_BTNFACE) ,TRUE);

	m_wndLV1MGridCtrl.ShowWindow(SW_HIDE);
	m_wndLV2MGridCtrl.ShowWindow(SW_SHOW);
	m_wndLV11MGridCtrl.ShowWindow(SW_HIDE);
	m_wndLV12MGridCtrl.ShowWindow(SW_HIDE);
	m_wndDCMGridCtrl.ShowWindow(SW_HIDE);
}

/**
	@brief	

	@author BHK	

	@date 2009-04-20 오후 2:58:12	

	@param	

	@return		
*/
void CMccbSizeSettingDlg::OnBnClickedButtonLv11()
{
	const int nTab = m_wndTabCtrl.GetActiveTab();
	m_wndMotorStatic.ShowWindow((nTab == 0)?SW_SHOW : SW_HIDE);
	m_wndFeederStatic.ShowWindow((nTab == 1)?SW_SHOW : SW_HIDE);
	//m_wndTabCtrl.SetActiveTab(1);

	CString rString;
	m_wndLV11Button.GetWindowText(rString);
	m_rCurVoltType = rString.operator LPCSTR();
	m_wndLV11Button.SetState(TRUE);
	m_wndLV11GridCtrl.ShowWindow(SW_SHOW);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	m_wndLV1GridCtrl.ShowWindow(SW_HIDE);
	m_wndLV1Button.SetState(FALSE);
	m_wndLV1Button.SetBGColor(GetSysColor(COLOR_BTNFACE) ,TRUE);
	m_wndLV2GridCtrl.ShowWindow(SW_HIDE);
	m_wndLV2Button.SetState(FALSE);
	m_wndLV2Button.SetBGColor(GetSysColor(COLOR_BTNFACE) ,TRUE);
	m_wndLV12GridCtrl.ShowWindow(SW_HIDE);
	m_wndLV12Button.SetState(FALSE);
	m_wndLV12Button.SetBGColor(GetSysColor(COLOR_BTNFACE) ,TRUE);
	m_wndDCGridCtrl.ShowWindow(SW_HIDE);
	m_wndDCButton.SetState(FALSE);
	m_wndDCButton.SetBGColor(GetSysColor(COLOR_BTNFACE) ,TRUE);

	m_wndLV1MGridCtrl.ShowWindow(SW_HIDE);
	m_wndLV2MGridCtrl.ShowWindow(SW_HIDE);
	m_wndLV11MGridCtrl.ShowWindow(SW_HIDE);
	m_wndLV12MGridCtrl.ShowWindow(SW_HIDE);
	m_wndDCMGridCtrl.ShowWindow(SW_HIDE);
}

/**
	@brief	

	@author BHK	

	@date 2009-04-20 오후 2:58:17	

	@param	

	@return		
*/
void CMccbSizeSettingDlg::OnBnClickedButtonLv12()
{
	const int nTab = m_wndTabCtrl.GetActiveTab();
	m_wndMotorStatic.ShowWindow((nTab == 0)?SW_SHOW : SW_HIDE);
	m_wndFeederStatic.ShowWindow((nTab == 1)?SW_SHOW : SW_HIDE);
	//m_wndTabCtrl.SetActiveTab(1);

	CString rString;
	m_wndLV12Button.GetWindowText(rString);
	m_rCurVoltType = rString.operator LPCSTR();
	m_wndLV12Button.SetState(TRUE);
	m_wndLV12GridCtrl.ShowWindow(SW_SHOW);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	m_wndLV1GridCtrl.ShowWindow(SW_HIDE);
	m_wndLV1Button.SetState(FALSE);
	m_wndLV1Button.SetBGColor(GetSysColor(COLOR_BTNFACE) ,TRUE);
	m_wndLV2GridCtrl.ShowWindow(SW_HIDE);
	m_wndLV2Button.SetState(FALSE);
	m_wndLV2Button.SetBGColor(GetSysColor(COLOR_BTNFACE) ,TRUE);
	m_wndLV11GridCtrl.ShowWindow(SW_HIDE);
	m_wndLV11Button.SetState(FALSE);
	m_wndLV11Button.SetBGColor(GetSysColor(COLOR_BTNFACE) ,TRUE);
	m_wndDCGridCtrl.ShowWindow(SW_HIDE);
	m_wndDCButton.SetState(FALSE);
	m_wndDCButton.SetBGColor(GetSysColor(COLOR_BTNFACE) ,TRUE);

	m_wndLV1MGridCtrl.ShowWindow(SW_HIDE);
	m_wndLV2MGridCtrl.ShowWindow(SW_HIDE);
	m_wndLV11MGridCtrl.ShowWindow(SW_HIDE);
	m_wndLV12MGridCtrl.ShowWindow(SW_HIDE);
	m_wndDCMGridCtrl.ShowWindow(SW_HIDE);
}

/**
	@brief	

	@author BHK	

	@date 2009-04-20 오후 2:58:20	

	@param	

	@return		
*/
void CMccbSizeSettingDlg::OnBnClickedButtonDc()
{
	const int nTab = m_wndTabCtrl.GetActiveTab();
	m_wndMotorStatic.ShowWindow((nTab == 0)?SW_SHOW : SW_HIDE);
	m_wndFeederStatic.ShowWindow((nTab == 1)?SW_SHOW : SW_HIDE);

	CString rString;
	m_wndDCButton.GetWindowText(rString);
	m_rCurVoltType = rString.operator LPCSTR();
	m_wndDCButton.SetState(TRUE);
	m_wndDCGridCtrl.ShowWindow(SW_SHOW);
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	m_wndLV1GridCtrl.ShowWindow(SW_HIDE);
	m_wndLV1Button.SetState(FALSE);
	m_wndLV1Button.SetBGColor(GetSysColor(COLOR_BTNFACE) ,TRUE);
	m_wndLV2GridCtrl.ShowWindow(SW_HIDE);
	m_wndLV2Button.SetState(FALSE);
	m_wndLV2Button.SetBGColor(GetSysColor(COLOR_BTNFACE) ,TRUE);
	m_wndLV11GridCtrl.ShowWindow(SW_HIDE);
	m_wndLV11Button.SetState(FALSE);
	m_wndLV11Button.SetBGColor(GetSysColor(COLOR_BTNFACE) ,TRUE);
	m_wndLV12GridCtrl.ShowWindow(SW_HIDE);
	m_wndLV12Button.SetState(FALSE);
	m_wndLV12Button.SetBGColor(GetSysColor(COLOR_BTNFACE) ,TRUE);
	m_wndDCGridCtrl.ShowWindow(SW_SHOW);

	m_wndLV1MGridCtrl.ShowWindow(SW_HIDE);
	m_wndLV2MGridCtrl.ShowWindow(SW_HIDE);
	m_wndLV11MGridCtrl.ShowWindow(SW_HIDE);
	m_wndLV12MGridCtrl.ShowWindow(SW_HIDE);
	m_wndDCMGridCtrl.ShowWindow(SW_HIDE);
}

/**
	@brief	motor나 feeder에 따라 FLC를 계산합니다.

	@author BHK	

	@date 2009-04-20 오후 4:44:55	

	@param	

	@return		
*/
static double CalculateFLC(const double nRatingCapacity , const double nLoadVolt , const double nPF , const bool& bMotor)
{
	double nFLC = 0.f;
	if(true == bMotor)
	{
		nFLC = nRatingCapacity / (::sqrt(3.) * nLoadVolt * nPF);
	}
	else
	{
		nFLC = nRatingCapacity / (::sqrt(3.) * nLoadVolt);
	}
	nFLC = SAFE_ROUND(nFLC,2);

	return nFLC;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-20 오후 4:41:52	

	@param	

	@return		
*/
void CMccbSizeSettingDlg::OnEnChangeEditPf()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	ChangeFLCAndSF(NULL , -1);
}

/**
	@brief	

	@author BHK	

	@date 2009-04-20 오후 4:59:45	

	@param	

	@return		
*/
void CMccbSizeSettingDlg::OnEnChangeEditSf()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	ChangeFLCAndSF(NULL , -1);
}

/**
	@brief	FLC와 FLC*SF값을 표기한다.

	@author BHK	

	@date 2009-04-23 오전 11:36:10	

	@param	

	@return		
*/
int CMccbSizeSettingDlg::ChangeFLCAndSF(CGridCtrl* pGridCtrl , const int& nRow)
{
	CString rString;
	GetDlgItemText(IDC_EDIT_PF , rString);
	const double nPF = atof(rString);

	GetDlgItemText(IDC_EDIT_SF , rString);
	const double nSF = atof(rString);

	if(NULL == pGridCtrl)
	{
		for(int n = 0; n < m_wndTabCtrl.GetTabsNum();++n)
		{
			for(int j = 0;j < SIZE_OF_ARRAY(__VOLT_TYPES__);++j)
			{
				CGridCtrl* pGridCtrl = GetGridCtrlOf(__VOLT_TYPES__[j], n);
				if(pGridCtrl)
				{
					const DWORD wParam = pGridCtrl->GetDlgCtrlID();
					const bool bMotor =	(wParam == m_wndDCMGridCtrl.GetDlgCtrlID()) || (wParam == m_wndLV1MGridCtrl.GetDlgCtrlID()) || 
								(wParam == m_wndLV11MGridCtrl.GetDlgCtrlID()) || (wParam == m_wndLV12MGridCtrl.GetDlgCtrlID());

					const double nLoadVolt = GetLoadVoltOf(__VOLT_TYPES__[j]);
					for(int i = 1;i < pGridCtrl->GetRowCount();++i)
					{
						if(0 != nPF)
						{
							double nFLCxSF = 0.;
							double nFLC = CalculateFLC(atof(pGridCtrl->GetItemText(i , 2)),nLoadVolt,nPF , bMotor);
							{
								ostringstream oss;

								nFLC = SAFE_ROUND(nFLC, 2);
								oss << nFLC;
								pGridCtrl->SetItemText(i , 4,oss.str().c_str());

								oss.str("");
								nFLCxSF = SAFE_ROUND(nFLC * nSF, 2);
								oss << nFLCxSF;
								pGridCtrl->SetItemText(i , 6,oss.str().c_str());
							}
							nFLC = CalculateFLC(atof(pGridCtrl->GetItemText(i , 3)),nLoadVolt,nPF , bMotor);
							{
								ostringstream oss;

								nFLC = SAFE_ROUND(nFLC, 2);
								oss << nFLC;
								pGridCtrl->SetItemText(i , 5,oss.str().c_str());
								
								oss.str("");
								nFLCxSF = SAFE_ROUND(nFLC * nSF, 2);
								oss << nFLCxSF;
								pGridCtrl->SetItemText(i , 7,oss.str().c_str());
							}
						}
						else
						{
							pGridCtrl->SetItemText(i , 4 , _T(""));
							pGridCtrl->SetItemText(i , 5 , _T(""));
							pGridCtrl->SetItemText(i , 6 , _T(""));
							pGridCtrl->SetItemText(i , 7 , _T(""));
						}
						pGridCtrl->RedrawRow(i);
					}
				}
			}
		}
	}
	else if((nRow > 0) && (nRow < pGridCtrl->GetRowCount()))
	{
		const DWORD wParam = pGridCtrl->GetDlgCtrlID();
		const bool bMotor =	(wParam == m_wndDCMGridCtrl.GetDlgCtrlID()) || (wParam == m_wndLV1MGridCtrl.GetDlgCtrlID()) || 
					(wParam == m_wndLV11MGridCtrl.GetDlgCtrlID()) || (wParam == m_wndLV12MGridCtrl.GetDlgCtrlID());

		const double nLoadVolt = GetLoadVoltOf(m_rCurVoltType);
		double nFLCxSF = 0.;
		if(0 != nPF)
		{
			double nFLC = CalculateFLC(atof(pGridCtrl->GetItemText(nRow , 2)),nLoadVolt,nPF , bMotor);
			{
				ostringstream oss;
				nFLC = SAFE_ROUND(nFLC, 2);
				oss << nFLC;
				pGridCtrl->SetItemText(nRow , 4,oss.str().c_str());
				pGridCtrl->RedrawCell(nRow , 4);

				oss.str("");
				nFLCxSF = SAFE_ROUND(nFLC * nSF, 2);
				oss << nFLCxSF;
				pGridCtrl->SetItemText(nRow , 6,oss.str().c_str());
				pGridCtrl->RedrawCell(nRow , 6);
			}
			nFLC = CalculateFLC(atof(pGridCtrl->GetItemText(nRow , 3)),nLoadVolt,nPF , bMotor);
			{
				ostringstream oss;
				nFLC = SAFE_ROUND(nFLC, 2);
				oss << nFLC;
				pGridCtrl->SetItemText(nRow , 5,oss.str().c_str());
				pGridCtrl->RedrawCell(nRow , 5);

				oss.str("");
				nFLCxSF = SAFE_ROUND(nFLC * nSF, 2);
				oss << nFLCxSF;
				pGridCtrl->SetItemText(nRow , 7,oss.str().c_str());
				pGridCtrl->RedrawCell(nRow , 7);
			}
		}
		else
		{
			pGridCtrl->SetItemText(nRow , 4 , _T(""));
			pGridCtrl->RedrawCell(nRow , 4);
			pGridCtrl->SetItemText(nRow , 5 , _T(""));
			pGridCtrl->RedrawCell(nRow , 5);
			pGridCtrl->SetItemText(nRow , 6 , _T(""));
			pGridCtrl->RedrawCell(nRow , 6);
			pGridCtrl->SetItemText(nRow , 7 , _T(""));
			pGridCtrl->RedrawCell(nRow , 7);
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	Table 저장과 함께 선택한 BUS에 한하여 MCCB값을 설정한다.

	@author	BHK

	@date	2009.09.29

	@return
*/
void CMccbSizeSettingDlg::OnBnClickedButtonApply()
{
	UpdateData(TRUE);

	if(FALSE == m_bAllLoads)
	{
		if(IDNO == AfxMessageBox("모든 Load들의 Mccb 값을 변경합니다, 계속하시겠습니까?", MB_YESNO|MB_ICONWARNING)) return;
	}
	//! 테이블을 저장합니다.
	OnBnClickedSaveButton();

	///////////////////////////////////////////////////////////////////////////////////////////////////

	COMMAND::CCommandAutoFillMCCBProp cmdAutoFillMCCBProp;
	if(ERROR_SUCCESS == cmdAutoFillMCCBProp.Execute(m_bAllLoads))
	{
		AfxMessageBox(_T("Save is done."));
	}
}
