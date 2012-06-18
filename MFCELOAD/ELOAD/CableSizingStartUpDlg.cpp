// CableSizingStartUpDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include <util/excel9/ExcelAutomation.h>
#include "CableSizingStartUpDlg.h"
#include "ELoadDocData.h"
#include "Tables/ConduitTable.h"
#include "enablebuddybutton.h"

#include "Tables/3PHMotorCableSizingTable.h"
#include "Tables/3PHFeederCableSizingTable.h"
#include "Tables/1PHFeederCableSizingTable.h"
#include "Tables/DCFeederCableSizingTable.h"
#include "ELOADCableSizingAgent.h"

using namespace TABLES;

// CCableSizingStartUpDlg dialog

IMPLEMENT_DYNAMIC(CCableSizingStartUpDlg, CDialog)

CCableSizingStartUpDlg::CCableSizingStartUpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCableSizingStartUpDlg::IDD, pParent)
{
}

CCableSizingStartUpDlg::~CCableSizingStartUpDlg()
{
	try
	{
		///m_font.DeleteObject();
	}
	catch(...)
	{
	}
}

void CCableSizingStartUpDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX , IDC_COMBO_DATA_NAME , m_wndDataNameCombo);
	DDX_Text(pDX , IDC_COMBO_DATA_NAME , m_rDataName);

	DDX_Text(pDX , IDC_COMBO_CABLE_DATA_TABLE , m_rCableDataTable);

	DDX_Text(pDX , IDC_COMBO_MOTOR_TYPE , m_rMotorType);

	DDX_Text(pDX , IDC_EDIT_BUS_MOTOR_VOLTAGE1 , m_rBusMotorVoltage[0]);
	DDX_Text(pDX , IDC_EDIT_BUS_MOTOR_VOLTAGE2 , m_rBusMotorVoltage[1]);
	DDX_Text(pDX , IDC_EDIT_HERTZ , m_rHertz);

	DDX_Text(pDX , IDC_EDIT_DERATING_FACTOR , m_rDeratingFactor);

	DDX_Text(pDX , IDC_EDIT_VOLTAGE_DROP1 , m_rVoltageDrop[0]);
	DDX_Text(pDX , IDC_EDIT_VOLTAGE_DROP2 , m_rVoltageDrop[1]);

	DDX_Text(pDX , IDC_EDIT_STARTING_VOLTAGE_DROP1 , m_rStartingVoltageDrop[0]);
	DDX_Text(pDX , IDC_EDIT_STARTING_VOLTAGE_DROP2 , m_rStartingVoltageDrop[1]);

	DDX_Text(pDX , IDC_EDIT_STARTING_POWER_FACTOR , m_rStartingPowerFactor);

	DDX_Text(pDX , IDC_COMBO_INSULATION_METHOD , m_rInsulationMethod);

	DDX_Text(pDX , IDC_COMBO_CABLE_TYPE , m_rCableType);
	
	DDX_Control(pDX , IDC_EDIT_PF_FOR_FEEDER , m_wndPFForFeederEdit);
	DDX_Text(pDX , IDC_EDIT_PF_FOR_FEEDER , m_rPFForFeeder);
	DDX_Text(pDX , IDC_COMBO_BUS , m_rSelectBus);
	DDX_Control(pDX , IDC_LIST_CONDUIT_TABLE , m_wndConduitTableListCtrl);

	DDX_Control(pDX , IDOK , m_wndOKButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);

	//! 2011.02.15 added by HumKyung
	DDX_Control(pDX , IDC_STATIC_SELECT_LOAD_TYPE , m_wndSelectLoadTypeButton);

	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCableSizingStartUpDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CCableSizingStartUpDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_COND, &CCableSizingStartUpDlg::OnBnClickedButtonSelectCond)
	ON_CBN_SELCHANGE(IDC_COMBO_BUS, &CCableSizingStartUpDlg::OnCbnSelchangeComboBus)
	ON_EN_CHANGE(IDC_EDIT_VOLTAGE_DROP1, &CCableSizingStartUpDlg::OnEnChangeEditVoltageDrop1)
	ON_EN_CHANGE(IDC_EDIT_STARTING_VOLTAGE_DROP1, &CCableSizingStartUpDlg::OnEnChangeEditStartingVoltageDrop1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CONDUIT_TABLE, &CCableSizingStartUpDlg::OnLvnItemchangedListConduitTable)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CONDUIT_TABLE, &CCableSizingStartUpDlg::OnNMClickListConduitTable)
	ON_CBN_SELENDOK(IDC_COMBO_CABLE_DATA_TABLE, &CCableSizingStartUpDlg::OnCbnSelendokComboCableDataTable)
	ON_CBN_SELENDOK(IDC_COMBO_MOTOR_TYPE, &CCableSizingStartUpDlg::OnCbnSelendokComboMotorType)
	ON_CBN_SELCHANGE(IDC_COMBO_MOTOR_TYPE, &CCableSizingStartUpDlg::OnCbnSelchangeComboMotorType)
	ON_CBN_SELCHANGE(IDC_COMBO_CABLE_DATA_TABLE, &CCableSizingStartUpDlg::OnCbnSelchangeComboCableDataTable)
	ON_CBN_SELENDOK(IDC_COMBO_DATA_NAME, &CCableSizingStartUpDlg::OnCbnSelendokComboDataName)

	ON_UPDATE_COMMAND_UI(IDOK , &CCableSizingStartUpDlg::OnUpdateOk)	//! 2011.02.15 added by HumKyung
END_MESSAGE_MAP()


// CCableSizingStartUpDlg message handlers

/**
	@brief

	@author	HumKyung

	@date	2011.02.15

	@param

	@return
*/
void CCableSizingStartUpDlg::OnUpdateOk(CCmdUI* pCmdUI)
{
	UpdateData(TRUE);

	if(m_rDataName.IsEmpty())
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	CString sLoadType;
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_MOTOR_TYPE);
	if(pCombo)
	{
		const int iCurSel = pCombo->GetCurSel();
		if(-1 != iCurSel)
		{
			pCombo->GetLBText(iCurSel , sLoadType);
		}
	}

	if(_T("3PH MOTOR") != sLoadType)
	{
		m_rPFForFeeder.Trim();
		pCmdUI->Enable(!m_rPFForFeeder.IsEmpty());
	}
	else
	{
		m_rStartingPowerFactor.Trim();
		m_rStartingVoltageDrop[0].Trim();
		pCmdUI->Enable(!m_rStartingPowerFactor.IsEmpty() && !m_rStartingVoltageDrop[0].IsEmpty());
	}
}

/**
	@brief	사용자가 설정한 값들을 저장한다.

	@author	HumKyung

	@date	????.??.??

	@param

	@return
*/
void CCableSizingStartUpDlg::OnBnClickedOk()
{
	UpdateData();

	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_DATA_NAME);
	pCombo->GetWindowText(m_rDataName);
	if(m_rDataName.IsEmpty())
	{
		AfxMessageBox(_T("DATA NAME이 비어 있습니다. DATA NAME을 입력해 주세요."));
		m_wndDataNameCombo.SetFocus();
		return;
	}
	SaveData(m_rDataName);

	OnOK();
}

/**
	@brief	DIALOG 초기화 부분.

	@author	HumKyung

	@date	????.??.??

	@param

	@return	BOOL
*/
BOOL CCableSizingStartUpDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	BeginWaitCursor();
	//! DialogBox Image
	HICON hIcon = AfxGetApp()->LoadIcon(IDI_CABLE_SIZING);
	SetIcon(hIcon, FALSE);

	CELoadDocData& docData = CELoadDocData::GetInstance();
	{
		CComboBox* pDataNameCombo = (CComboBox*)GetDlgItem(IDC_COMBO_DATA_NAME);
		CCableSizeSettingTable& CableSizeSettingTable = CCableSizeSettingTable::GetInstance();
		CableSizeSettingTable.Load();
		for(map<CString , CCableSizeSettingTable::Record>::iterator itr = CableSizeSettingTable.m_RecordMap.begin();itr != CableSizeSettingTable.m_RecordMap.end();++itr)
		{
			pDataNameCombo->AddString(itr->first);
		}

		vector<CBusItem*> BusItemEntry;
		list<CELoadItem*> BusGroupList;
		docData.GetELoadItemListOf(BusGroupList , CBusGroupItem::TypeString());
		///if(pBusGroupList)
		{
			for(list<CELoadItem*>::iterator itr = BusGroupList.begin();itr != BusGroupList.end();++itr)
			{
				CBusGroupItem* pBusGroup = static_cast<CBusGroupItem*>(*itr);

				if(pBusGroup->IsDeleted()) continue;
				BusItemEntry.insert(BusItemEntry.end() , pBusGroup->m_BusRefEntry.begin() , pBusGroup->m_BusRefEntry.end());
			}
		}
		
		{
			CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_BUS);
			for(vector<CBusItem*>::iterator itr = BusItemEntry.begin();itr != BusItemEntry.end();++itr)
			{
				if((*itr)->IsDeleted()) continue;
				pCombo->AddString((*itr)->GetName().c_str());
			}
		}
	}

	static const CString rToolTip(_T("데이터 형식이 일치하지 않습니다."));
	const COLORREF WavyLineColor = docData.GetColorCode("FILTER_EDIT_WAVY_LINE_COLOR");
	const COLORREF BackFocusColor= docData.GetColorCode("FILTER_EDIT_BACK_FOCUS_COLOR");
	const COLORREF TextErrorColor= docData.GetColorCode("FILTER_EDIT_TEXT_ERROR_COLOR");
	const COLORREF BackErrorColor= docData.GetColorCode("FILTER_EDIT_BACK_ERROR_COLOR");

	const string rNumberRegEx = docData.GetRegExp("NUMBER");
	m_wndPFForFeederEdit.SetRegEx(rNumberRegEx.c_str());
	m_wndPFForFeederEdit.CreateToolTip(this , rToolTip);
	m_wndPFForFeederEdit.SetWavyLineColour (WavyLineColor);
	m_wndPFForFeederEdit.SetBackgroundColourFocus (BackFocusColor);
	m_wndPFForFeederEdit.SetForegroundColourError (TextErrorColor);
	m_wndPFForFeederEdit.SetBackgroundColourError (BackErrorColor);

	if(ERROR_SUCCESS != LoadData()) return FALSE;

	FillCableTypeComboBox();

	CBitmap bmp;
	if (!bmp.LoadBitmap (IDB_CHECK))
	{
		TRACE(_T ("Can't load bitmap: %x\n"), IDB_CHECK);
		ASSERT (FALSE);
		return FALSE;
	}

	BITMAP bmpObj;
	bmp.GetBitmap (&bmpObj);

	const UINT nFlags = ILC_MASK | ILC_COLOR16;

	/// Class View Tree  이미지리스트
	m_imgList.Create (16, bmpObj.bmHeight, nFlags, 0, 0);
	m_imgList.Add (&bmp, RGB (255, 255, 255));

	m_wndConduitTableListCtrl.SetImageList(&m_imgList , LVSIL_SMALL);
	m_wndConduitTableListCtrl.InsertColumn(0 , _T("Table Name")  , LVCFMT_LEFT , 200);
	m_wndConduitTableListCtrl.InsertColumn(1 , _T("Table Desc.") , LVCFMT_LEFT , 150);
	m_wndConduitTableListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
	DisplayConduitData();

	UpdateData(FALSE);

	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	m_wndOKButton.SetTextHotColor(TextHotColor);
	m_wndCancelButton.SetTextHotColor(TextHotColor);

	m_wndOKButton.SetIcon(IDI_OK_18X18);
	m_wndCancelButton.SetIcon(IDI_CANCEL_18X18);

	OnCbnSelendokComboMotorType();
	OnCbnSelchangeComboMotorType();

	//! 2011.02.15 added by HumKyung
	m_wndSelectLoadTypeButton.SetFontBold(TRUE);
	m_wndSelectLoadTypeButton.SetTextColor(RGB(255,0,0));

	m_font.CreateFont(-11,0,0,0,400,FALSE,FALSE,FALSE,HANGUL_CHARSET,3,2,1,VARIABLE_PITCH | FF_MODERN, _T("Arial")); 
        LOGFONT logfont;
	m_font.GetLogFont(&logfont);
	GetDlgItem(IDC_COMBO_MOTOR_TYPE)->SetFont(&m_font);
	//!
	
	EndWaitCursor();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	

	@author	HumKyung

	@date	????.??.??

	@param

	@return	int
*/
int CCableSizingStartUpDlg::LoadData(void)
{
	CELoadDocData& docData    = CELoadDocData::GetInstance();
	CCableSizeSettingTable& CableSizeSettingTable = CCableSizeSettingTable::GetInstance();
	CableSizeSettingTable.Load();
	
	if(!CableSizeSettingTable.m_RecordMap.empty())
	{
		TABLES::CELOADTable* pTable = docData.GetTableOf(CProjectSettingTable::GetTableName());
		m_rDataName = pTable->m_RecordEntry[0].FieldValueMap[_T("C_CUR_CABLE_SIZING_SETTING_TABLE_NAME")].c_str();

		m_rBusMotorVoltage[0]     = CableSizeSettingTable.m_RecordMap[m_rDataName].m_rBusMotorVoltage[0].c_str();
		SetDlgItemText(IDC_EDIT_BUS_MOTOR_VOLTAGE1 , m_rBusMotorVoltage[0]);
		m_rBusMotorVoltage[1]     = CableSizeSettingTable.m_RecordMap[m_rDataName].m_rBusMotorVoltage[1].c_str();
		SetDlgItemText(IDC_EDIT_BUS_MOTOR_VOLTAGE2 , m_rBusMotorVoltage[1]);
		CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
		string* pValue = pSettingTable->GetFieldValue(_T("C_HERTZ"));
		m_rHertz = ((NULL != pValue) && !pValue->empty()) ? pValue->c_str() : _T("50");
		m_rDeratingFactor         = CableSizeSettingTable.m_RecordMap[m_rDataName].m_rDeratingFactor.c_str();
		SetDlgItemText(IDC_EDIT_DERATING_FACTOR , m_rDeratingFactor);
		m_rVoltageDrop[0]         = CableSizeSettingTable.m_RecordMap[m_rDataName].m_rVoltageDrop[0].c_str();
		SetDlgItemText(IDC_EDIT_VOLTAGE_DROP1 , m_rVoltageDrop[0]);
		m_rVoltageDrop[1]         = CableSizeSettingTable.m_RecordMap[m_rDataName].m_rVoltageDrop[1].c_str();
		m_rStartingVoltageDrop[0] = CableSizeSettingTable.m_RecordMap[m_rDataName].m_rStartingVoltageDrop[0].c_str();
		SetDlgItemText(IDC_EDIT_STARTING_VOLTAGE_DROP1 , m_rStartingVoltageDrop[0]);
		m_rStartingVoltageDrop[1] = CableSizeSettingTable.m_RecordMap[m_rDataName].m_rStartingVoltageDrop[1].c_str();
		m_rStartingPowerFactor    = CableSizeSettingTable.m_RecordMap[m_rDataName].m_rStartingPowerFactor.c_str();
		SetDlgItemText(IDC_EDIT_STARTING_POWER_FACTOR , m_rStartingPowerFactor);
		m_rInsulationMethod       = CableSizeSettingTable.m_RecordMap[m_rDataName].m_rInsulationMethod.c_str();
		m_rCableType              = CableSizeSettingTable.m_RecordMap[m_rDataName].m_rCableType.c_str();
		m_rCableDataTable	  = CableSizeSettingTable.m_RecordMap[m_rDataName].m_rCableDataTable.c_str();
		m_rConduitTable		  = CableSizeSettingTable.m_RecordMap[m_rDataName].m_rConduitTable.c_str();

		m_rMotorType		  = CableSizeSettingTable.GetMotorType(m_rDataName);
		m_rPFForFeeder		  = CableSizeSettingTable.GetPFForFeederValue(m_rDataName);
		m_wndPFForFeederEdit.SetWindowText(m_rPFForFeeder);
		m_rSelectBus		  = CableSizeSettingTable.m_RecordMap[m_rDataName].m_rSelectBus.c_str();

		const int nFound = m_wndDataNameCombo.FindString(-1 , m_rDataName);
		(-1 != nFound) ? m_wndDataNameCombo.SetCurSel(nFound) : m_wndDataNameCombo.SetCurSel(0);
	}
	if(m_rConduitTable.IsEmpty())
	{
		m_rConduitTable = _T("COND_IEC");	//! default value - 2011.02.15 added by HumKyung
		
		//! set default value - 2011.02.24 added by HumKyung
		map<CString , CCableSizeSettingTable::Record>::iterator where = CCableSizeSettingTable::GetInstance().m_RecordMap.find(m_rDataName);
		if(where != CCableSizeSettingTable::GetInstance().m_RecordMap.end())
		{
			CCableSizeSettingTable::GetInstance().m_RecordMap[m_rDataName].m_rConduitTable = m_rConduitTable;
		}
	}

	{
		CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_HERTZ);
		if(pEdit) pEdit->SetWindowText( m_rHertz );
	}

	{
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_CABLE_DATA_TABLE);
		if(pComboBox)
		{
			const int nFound = pComboBox->FindString(-1,m_rCableDataTable);
			if(-1 != nFound)
				pComboBox->SetCurSel(nFound);
			else	pComboBox->SetCurSel(0);
		}
	}

	{
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_MOTOR_TYPE);
		if(pComboBox)
		{
			const int nFound = pComboBox->FindString(-1,m_rMotorType);
			if(-1 != nFound)
				pComboBox->SetCurSel(nFound);
			else	pComboBox->SetCurSel(0);

			//! 2011.02.24 added by HumKyung
			OnCbnSelendokComboMotorType();
			OnCbnSelchangeComboMotorType();
			//!
		}
	}

	{
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_CABLE_TYPE);
		if(pComboBox)
		{
			const int nFound = pComboBox->FindString(-1,m_rCableType);
			if(-1 != nFound)
				pComboBox->SetCurSel(nFound);
			else	pComboBox->SetCurSel(0);
		}
	}

	{
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_INSULATION_METHOD);
		if(pComboBox)
		{
			const int nFound = pComboBox->FindString(-1,m_rInsulationMethod);
			if(-1 != nFound)
				pComboBox->SetCurSel(nFound);
			else	pComboBox->SetCurSel(0);
		}
	}

	{
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_BUS);
		if(pComboBox)
		{
			const int nFound = pComboBox->FindString(-1 , m_rSelectBus);
			if(-1 != nFound)
				pComboBox->SetCurSel(nFound);
			else	pComboBox->SetCurSel(0);
		}
	}

	CConduitListTable::GetInstance().Load();
	CCableDataListTable::GetInstance().Load();

	return ERROR_SUCCESS;
}

/**
	@brief	사용자가 설정한 값을 저장한다.

	@author	HumKyung

	@date	????.??.??

	@param	rDataName

	@return	int
*/
int CCableSizingStartUpDlg::SaveData(const CString& rDataName)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	
	TABLES::CELOADTable* pTable = docData.GetTableOf(CProjectSettingTable::GetTableName());
	if(pTable)
	{
		pTable->m_RecordEntry[0].FieldValueMap[_T("C_CUR_CABLE_SIZING_SETTING_TABLE_NAME")] = rDataName.operator LPCTSTR();
		pTable->Save();
	}

	CCableSizeSettingTable& CableSizeSettingTable = CCableSizeSettingTable::GetInstance();
	{
		map<CString , CCableSizeSettingTable::Record>::iterator where = CableSizeSettingTable.m_RecordMap.find(rDataName);
		if(where == CableSizeSettingTable.m_RecordMap.end())
		{
			CCableSizeSettingTable::Record record;
			CableSizeSettingTable.m_RecordMap.insert(make_pair(rDataName , record));
		}

		CableSizeSettingTable.m_RecordMap[rDataName].m_rBusMotorVoltage[0]     = m_rBusMotorVoltage[0];
		CableSizeSettingTable.m_RecordMap[rDataName].m_rBusMotorVoltage[1]     = m_rBusMotorVoltage[1];
		CableSizeSettingTable.m_RecordMap[rDataName].m_rDeratingFactor         = m_rDeratingFactor;
		CableSizeSettingTable.m_RecordMap[rDataName].m_rVoltageDrop[0]         = m_rVoltageDrop[0];
		CableSizeSettingTable.m_RecordMap[rDataName].m_rVoltageDrop[1]         = m_rVoltageDrop[1];
		CableSizeSettingTable.m_RecordMap[rDataName].m_rStartingVoltageDrop[0] = m_rStartingVoltageDrop[0];
		CableSizeSettingTable.m_RecordMap[rDataName].m_rStartingVoltageDrop[1] = m_rStartingVoltageDrop[1];
		CableSizeSettingTable.m_RecordMap[rDataName].m_rStartingPowerFactor    = m_rStartingPowerFactor;
		CableSizeSettingTable.m_RecordMap[rDataName].m_rInsulationMethod       = m_rInsulationMethod;
		CableSizeSettingTable.m_RecordMap[rDataName].m_rCableType              = m_rCableType;
		CableSizeSettingTable.SetMotorType(rDataName , m_rMotorType);
		CableSizeSettingTable.SetPFForFeederValue(rDataName , m_rPFForFeeder);

		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_BUS);
		int nSelItem = pCombo->GetCurSel();
		pCombo->GetLBText(nSelItem, CString(CableSizeSettingTable.m_RecordMap[rDataName].m_rSelectBus.c_str()));
	}

	if(m_wndConduitTableListCtrl.GetItemCount() > 0)
	{
		POSITION pos = m_wndConduitTableListCtrl.GetFirstSelectedItemPosition();
		if(NULL != pos)
		{
			const int nSelItem = m_wndConduitTableListCtrl.GetNextSelectedItem(pos);
			const CString rTableName = m_wndConduitTableListCtrl.GetItemText(nSelItem , 0);
			const CString rTableDesc = m_wndConduitTableListCtrl.GetItemText(nSelItem , 1);
			docData.m_ConduitTable.Load(rTableName.operator LPCSTR());

			CableSizeSettingTable.m_RecordMap[rDataName].m_rConduitTable = rTableName;
			CableSizeSettingTable.m_RecordMap[rDataName].m_rConduitDesc = rTableDesc;
		}
	}

	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_CABLE_DATA_TABLE);
	if(pComboBox)
	{
		const int nSel = pComboBox->GetCurSel();
		if(-1 != nSel)
		{
			CString rTableName;
			pComboBox->GetLBText(nSel , rTableName);
			docData.m_ProjectCableDataTable.Load(rTableName.operator LPCSTR());
			CableSizeSettingTable.m_RecordMap[rDataName].m_rCableDataTable = rTableName;
		}
	}
	
	CableSizeSettingTable.Save();
	docData.m_FLCTable.Save();

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
void CCableSizingStartUpDlg::OnBnClickedButtonSelectCond()
{
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY  ,"Excel Files(*.xls)|*.xls|All Files(*.*)|*.*||");
	if(IDOK == fileDlg.DoModal())
	{
		CString strFileName  = fileDlg.GetPathName();
		CString strFileTitle = fileDlg.GetFileTitle();
		CFileException fileException;

		ImportConduitExcelFile(strFileName);
		GetDlgItem(IDC_EDIT_COND)->SetWindowText(strFileName);
		DisplayConduitData();

	}
}

/**
	@brief	

	@author	BHK
*/
int CCableSizingStartUpDlg::ImportConduitExcelFile(const CString& rFilePath)
{
	CExcelAutomation xlApp;

	//! 엑셀 파일은 오픈한다.
	if(!xlApp.Open(rFilePath))
	{
		AfxMessageBox("Can't Start Excel and get Application object.");

		return ERROR_BAD_ENVIRONMENT;
	}

	CELoadDocData& docData = CELoadDocData::GetInstance();
	docData.m_ConduitTable.Clear();

	long lowerRow = 0 , lowerCol = 0 , upperRow = 0 , upperCol = 0;
	xlApp.GetUsedRange(lowerRow , lowerCol , upperRow , upperCol);
	for(int i = 2 ;i  <= upperRow;++i)
	{
		CConduitTable::Record* pRecord = new CConduitTable::Record;
		if(pRecord)
		{
			for(int j = lowerCol;j <= upperCol;++j)
			{
				const CString rCellNo = xlApp.MakeExcelNoString(i,j);
				CString rCellValue;
				xlApp.GetCellValue(COleVariant(rCellNo) , rCellValue);

				switch(j)
				{
					case 1:
						pRecord->CONDUIT = rCellValue;
						break;
					case 2:
						pRecord->DIA = rCellValue;
						break;
				}
			}

			docData.m_ConduitTable.Insert(pRecord);
		}
	}

	return 0;
}

/**
	@brief	conduit data를 표기한다.

	@author	HumKyung

	@date	????.??.??

	@param

	@return	
*/
int CCableSizingStartUpDlg::DisplayConduitData(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	m_wndConduitTableListCtrl.DeleteAllItems();	//! 기존의 항목을 삭제한다.

	CConduitListTable& ConduitListTable = CConduitListTable::GetInstance();

	int nIndex = 0;
	const size_t nRecordCount = ConduitListTable.GetRecordCount();
	for(int i = 0;i < (int)nRecordCount;++i)
	{
		TABLES::CConduitListTable::TABLE_RECORD* pRecord = ConduitListTable.GetRecordAt(i);
		if(pRecord)
		{
			const int nItem = m_wndConduitTableListCtrl.InsertItem(i , pRecord->rTableNo.c_str() , 0);
			if(-1 != nItem)
			{
				m_wndConduitTableListCtrl.SetItemText(nItem , 1 , pRecord->rTableDesc.c_str());
				if(m_rConduitTable == pRecord->rTableNo.c_str())
				{
					m_wndConduitTableListCtrl.SetItem(nItem , 0 , LVIF_IMAGE , NULL , 1 , -1,0,0);
				}
			}
		}
	}

	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		TABLES::CELOADTable* pTable = docData.GetTableOf(CProjectSettingTable::GetTableName());
		const CString rDataName = pTable->m_RecordEntry[0].FieldValueMap[_T("C_CUR_CABLE_SIZING_SETTING_TABLE_NAME")].c_str();

		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_CABLE_DATA_TABLE);
		if(pComboBox)
		{
			pComboBox->ResetContent(); //! 기존의 항목을 삭제한다.

			CCableDataListTable& CableDataListTable = CCableDataListTable::GetInstance();
			nIndex = 0;
			const size_t nRecordCount = CableDataListTable.GetRecordCount();
			for(int i = 0;i < (int)nRecordCount;++i)
			{
				TABLES::CCableDataListTable::TABLE_RECORD* pRecord = CableDataListTable.GetRecordAt(i);
				if(pRecord) pComboBox->AddString(pRecord->rTableNo.c_str());
			}

			int nFound = pComboBox->FindString(0,
				CCableSizeSettingTable::GetInstance().GetCableDataTableName(rDataName));
			if(-1 != nFound) pComboBox->SetCurSel(nFound);
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
int CCableSizingStartUpDlg::ImportFLCExcelFile(const CString& rFilePath)
{
	CExcelAutomation xlApp;

	//! 엑셀 파일은 오픈한다.
	if(!xlApp.Open(rFilePath))
	{
		AfxMessageBox("Can't Start Excel and get Application object.");

		return ERROR_BAD_ENVIRONMENT;
	}

	CELoadDocData& docData = CELoadDocData::GetInstance();
	docData.m_FLCTable.Clear();

	long lowerRow = 0 , lowerCol = 0 , upperRow = 0 , upperCol = 0;
	xlApp.GetUsedRange(lowerRow , lowerCol , upperRow , upperCol);
	for(int i = 2 ;i  <= upperRow;++i)
	{
		CFLCTable::Record* pRecord = new CFLCTable::Record;
		if(pRecord)
		{
			for(int j = lowerCol;j <= upperCol;++j)
			{
				const CString rCellNo = xlApp.MakeExcelNoString(i,j);
				CString rCellValue;
				xlApp.GetCellValue(COleVariant(rCellNo) , rCellValue);

				switch(j)
				{
					case 1:
						pRecord->VALUE = rCellValue;
						break;
				}
			}

			docData.m_FLCTable.Insert(pRecord);
		}
	}
        return 0;
}

/**
	@brief	다른 BUS를 선택했을 때

	@author	BHK
*/
void CCableSizingStartUpDlg::OnCbnSelchangeComboBus()
{
	UpdateData();

	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_BUS);
	if(pCombo)
	{
		const int nSel = pCombo->GetCurSel();
		if(-1 != nSel)
		{
			CString rString;
			pCombo->GetLBText(nSel , rString);

			CELoadDocData& docData = CELoadDocData::GetInstance();
			CBusItem* pBusItem = docData.FindBusItemByName(rString.operator LPCSTR());
			if(NULL != pBusItem)
			{
				{
					const double nVoltage = atof(pBusItem->prop()->GetValue("General", "Bus Voltage").c_str());
					stringstream oss;
					oss << nVoltage * 1000.f;	//! KV --> V
					m_rBusMotorVoltage[0] = oss.str().c_str();	
				}
				{
					const double nVoltage = atof(pBusItem->prop()->GetValue("General", "Load Voltage").c_str());
					stringstream oss;
					oss << nVoltage * 1000.f;	//! KV --> V
					m_rBusMotorVoltage[1] = oss.str().c_str();	
				}
				
				OnEnChangeEditVoltageDrop1();
				OnEnChangeEditStartingVoltageDrop1();

				UpdateData(FALSE);
			}
		}
	}
}

/*
	@brief	

	@author	HumKyung

	@date	????.??.??

	@param	pMsg

	@return	BOOL
*/
BOOL CCableSizingStartUpDlg::PreTranslateMessage(MSG* pMsg)
{
	//! 리턴키나 ESCAPE KEY로 윈도우가 닫기는 것을 방지.
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE) return TRUE;
		if(pMsg->wParam == VK_RETURN) return TRUE;
	}

	UpdateDialogControls(this, TRUE);

	return CDialog::PreTranslateMessage(pMsg);
}

/**
	@brief	

	@author	BHK
*/
void CCableSizingStartUpDlg::OnEnChangeEditVoltageDrop1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_VOLTAGE_DROP1);
	if(pEdit)
	{
		const double nVoltage = atof(m_rBusMotorVoltage[0].operator LPCSTR());

		CString rString;
		pEdit->GetWindowText(rString);
		const double nVoltageDrop = atof(rString.operator LPCSTR());
		stringstream oss;
		oss << nVoltage * nVoltageDrop * 0.01;

		m_rVoltageDrop[1] = oss.str().c_str();

		GetDlgItem(IDC_EDIT_VOLTAGE_DROP2)->SetWindowText(m_rVoltageDrop[1]);
	}
}

void CCableSizingStartUpDlg::OnEnChangeEditStartingVoltageDrop1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_STARTING_VOLTAGE_DROP1);
	if(pEdit)
	{
		const double nVoltage = atof(m_rBusMotorVoltage[0].operator LPCSTR());

		CString rString;
		pEdit->GetWindowText(rString);
		const double nVoltageDrop = atof(rString.operator LPCSTR());
		stringstream oss;
		oss << nVoltage * nVoltageDrop * 0.01;

		m_rStartingVoltageDrop[1] = oss.str().c_str();

		GetDlgItem(IDC_EDIT_STARTING_VOLTAGE_DROP2)->SetWindowText(m_rStartingVoltageDrop[1]);
	}
}

void CCableSizingStartUpDlg::OnLvnItemchangedListConduitTable(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CCableSizingStartUpDlg::OnNMClickListConduitTable(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	if(-1 != pNMItemActivate->iItem)
	{
		m_wndConduitTableListCtrl.SetItemState(pNMItemActivate->iItem,LVIS_SELECTED ,LVIS_SELECTED);

		{
			m_wndConduitTableListCtrl.SetItem(pNMItemActivate->iItem,0,LVIF_IMAGE , NULL , 1 ,-1,0,0); 
		}

		const int nCount = m_wndConduitTableListCtrl.GetItemCount();
		for(int i = 0; i < nCount; ++i)
		{
			if(i == pNMItemActivate->iItem) continue;

			m_wndConduitTableListCtrl.SetItem(i,0,LVIF_IMAGE , NULL , 0 ,-1,0,0); 
		}
	}

	*pResult = 0;
}

/**
	@brief	when cable data table is selected

	@author	HumKyung

	@date	????.??.??

	@param	

	@return	
*/
void CCableSizingStartUpDlg::OnCbnSelendokComboCableDataTable()
{
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_CABLE_DATA_TABLE);
	if(pComboBox)
	{
		const int nSel = pComboBox->GetCurSel();
		if(-1 != nSel)
		{
			CELoadDocData& docData = CELoadDocData::GetInstance();
			TABLES::CELOADTable* pTable = docData.GetTableOf(CProjectSettingTable::GetTableName());
			const CString rDataName = pTable->m_RecordEntry[0].FieldValueMap[_T("C_CUR_CABLE_SIZING_SETTING_TABLE_NAME")].c_str();
			
			CString rTableName;
			pComboBox->GetLBText(nSel , rTableName);

			//! CableSetting의 CableDataTable이름을 사용자가 정한 값으로 설정한다.
			CCableSizeSettingTable::GetInstance().m_RecordMap[rDataName].m_rCableDataTable = rTableName;
			
			CELOADCableSizingAgent agent;
			if(ERROR_SUCCESS == agent.LoadCableDataTableIfNeeded())
			{
				FillCableTypeComboBox();
			}
		}
	}
}

/**
	@brief	when motor type is selected

	@author	HumKyung

	@date	????.??.??

	@param

	@return
*/
void CCableSizingStartUpDlg::OnCbnSelendokComboMotorType()
{
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_MOTOR_TYPE);
	if(pComboBox)
	{
		const int nSel = pComboBox->GetCurSel();
		if(-1 != nSel)
		{
			CString rLBText;
			pComboBox->GetLBText(nSel , rLBText);

			CWnd* pWnd = (CWnd*)(GetDlgItem(IDC_EDIT_PF_FOR_FEEDER));
			if(pWnd) pWnd->EnableWindow(-1 != rLBText.Find("FEEDER"));

			//! 3PH MOTOR제외한 나머지는 STARTING VD, STARTING PF 비활성화 - 2011.02.15 added by Humkyung
			pWnd = (CWnd*)(GetDlgItem(IDC_EDIT_STARTING_VOLTAGE_DROP1));
			if(pWnd) pWnd->EnableWindow(-1 != rLBText.Find("3PH MOTOR"));
			pWnd = (CWnd*)(GetDlgItem(IDC_EDIT_STARTING_POWER_FACTOR));
			if(pWnd) pWnd->EnableWindow(-1 != rLBText.Find("3PH MOTOR"));
			//!

			CString rString;
			rString.Format("POWER && GROUNDING CABLE(FOR %s)" , rLBText);
			SetDlgItemText(IDC_STATIC_PROP , rString);
		}
	}
}

/**
	@brief

	@author	HumKyung

	@date	????.??.??

	@param	

	@return	
*/
void CCableSizingStartUpDlg::OnCbnSelchangeComboMotorType()
{
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_MOTOR_TYPE);
	const int nSel = pCombo->GetCurSel();
	if(-1 != nSel) pCombo->GetLBText(nSel , m_rMotorType);
	if(_T("3PH MOTOR") == m_rMotorType)
	{
		C3PHMotorCableSizingTable& table = C3PHMotorCableSizingTable::GetInstance();
		table.Load();
		if(0 == table.GetRecordCount())
		{
			AfxMessageBox(m_rMotorType + " TABLE을 로딩할 수 없습니다.\n 세팅에서 설정해 주세요.");
			m_wndOKButton.EnableWindow(FALSE);
		}else	m_wndOKButton.EnableWindow(TRUE);
	}
	else if(_T("3PH FEEDER") == m_rMotorType)
	{
		C3PHFeederCableSizingTable& table = C3PHFeederCableSizingTable::GetInstance();
		table.Load();
		if(0 == table.GetRecordCount())
		{
			AfxMessageBox(m_rMotorType + " TABLE을 로딩할 수 없습니다.\n 세팅에서 설정해 주세요.");
			m_wndOKButton.EnableWindow(FALSE);
		}else	m_wndOKButton.EnableWindow(TRUE);
	}
	else if(_T("1PH FEEDER") == m_rMotorType)
	{
		C1PHFeederCableSizingTable& table = C1PHFeederCableSizingTable::GetInstance();
		table.Load();
		if(0 == table.GetRecordCount())
		{
			AfxMessageBox(m_rMotorType + " TABLE을 로딩할 수 없습니다.\n 세팅에서 설정해 주세요.");
			m_wndOKButton.EnableWindow(FALSE);
		}else	m_wndOKButton.EnableWindow(TRUE);
	}
	else if(_T("DC FEEDER") == m_rMotorType)
	{
		CDCFeederCableSizingTable& table = CDCFeederCableSizingTable::GetInstance();
		table.Load();
		if(0 == table.GetRecordCount())
		{
			AfxMessageBox(m_rMotorType + " TABLE을 로딩할 수 없습니다.\n 세팅에서 설정해 주세요.");
			m_wndOKButton.EnableWindow(FALSE);
		}else	m_wndOKButton.EnableWindow(TRUE);
	}else	m_wndOKButton.EnableWindow(FALSE);
}

/**
	@brief	cable type combobox를 채웁니다.

	@author	BHK
*/
int CCableSizingStartUpDlg::FillCableTypeComboBox(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	vector<string> CableTypeList;
	CELOADCableSizingAgent agent;
	agent.GetCableTypeList(CableTypeList);

	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_CABLE_TYPE);
	if(pComboBox)
	{
		pComboBox->ResetContent();
		for(vector<string>::iterator itr = CableTypeList.begin();itr != CableTypeList.end();++itr)
		{
			pComboBox->AddString(itr->c_str());
		}
		
		const int nFound = pComboBox->FindString(0 , m_rCableType);
		if(-1 != nFound) pComboBox->SetCurSel(nFound);
	}

	return ERROR_SUCCESS;
}

void CCableSizingStartUpDlg::OnCbnSelchangeComboCableDataTable()
{
	// TODO: Add your control notification handler code here
}

/**
	@brief	when data name selection is done.

	@author	HumKyung

	@date	2009.10.05

	@param

	@return
*/
void CCableSizingStartUpDlg::OnCbnSelendokComboDataName()
{
	const int nSel = m_wndDataNameCombo.GetCurSel();
	if(-1 != nSel)
	{
		m_wndDataNameCombo.GetLBText(nSel , m_rDataName);
		
		CELoadDocData& docData = CELoadDocData::GetInstance();
		TABLES::CELOADTable* pTable = docData.GetTableOf(CProjectSettingTable::GetTableName());
		if(pTable)
		{
			pTable->m_RecordEntry[0].FieldValueMap[_T("C_CUR_CABLE_SIZING_SETTING_TABLE_NAME")] = m_rDataName;
		}

		LoadData();
		DisplayConduitData();

		UpdateData(FALSE);
	}
}
