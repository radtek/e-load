// CableSizingSettingPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "CableSizingSettingPropPage.h"
#include <util/excel9/ExcelAutomation.h>
#include "ELoadDocData.h"
#include "SQLFile.h"

#include "3PHMotorCableSizingTable.h"
#include "3PHFeederCableSizingTable.h"
#include "1PHFeederCableSizingTable.h"
#include "DCFeederCableSizingTable.h"

// CCableSizingSettingPropPage dialog

IMPLEMENT_DYNAMIC(CCableSizingSettingPropPage, CELOADPropPage)

CCableSizingSettingPropPage::CCableSizingSettingPropPage()
	: CELOADPropPage(CCableSizingSettingPropPage::IDD)
{

}

CCableSizingSettingPropPage::~CCableSizingSettingPropPage()
{
}

void CCableSizingSettingPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX , IDC_3PH_MOTOR  , m_wnd3PHMotorStatic);
	DDX_Control(pDX , IDC_3PH_FEEDER , m_wnd3PHFeederStatic);
	DDX_Control(pDX , IDC_1PH_FEEDER , m_wnd1PHFeederStatic);
	DDX_Control(pDX , IDC_DC_FEEDER  , m_wndDCFeederStatic);

        DDX_Control(pDX , IDC_BUTTON_3PH_MOTOR , m_wnd3PHMotorButton);
        DDX_Control(pDX , IDC_BUTTON_3PH_FEEDER, m_wnd3PHFeederButton);
        DDX_Control(pDX , IDC_BUTTON_1PH_FEEDER, m_wnd1PHFeederButton);
        DDX_Control(pDX , IDC_BUTTON_DC_FEEDER , m_wndDCFeederButton);

        CELOADPropPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCableSizingSettingPropPage, CELOADPropPage)
	ON_BN_CLICKED(IDC_BUTTON_3PH_MOTOR, &CCableSizingSettingPropPage::OnBnClickedButton3phMotor)
	ON_BN_CLICKED(IDC_BUTTON_3PH_FEEDER, &CCableSizingSettingPropPage::OnBnClickedButton3phFeeder)
	ON_BN_CLICKED(IDC_BUTTON_1PH_FEEDER, &CCableSizingSettingPropPage::OnBnClickedButton1phFeeder)
	ON_BN_CLICKED(IDC_BUTTON_DC_FEEDER, &CCableSizingSettingPropPage::OnBnClickedButtonDcFeeder)
END_MESSAGE_MAP()

/**
	@brief	select a excel file.

	@author	BHK
*/
static CString GetExcelFilePath()
{
	CString res;

	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY  ,"Excel Files(*.xls)|*.xls|All Files(*.*)|*.*||");
	if(IDOK == fileDlg.DoModal())
	{
		res = fileDlg.GetPathName();
		CString strFileTitle = fileDlg.GetFileTitle();
		CFile myFile;
		CFileException fileException;

		if(!myFile.Open(res, CFile::modeRead, &fileException))
		{
			TRACE("Can't open file %s, error = %u\n", res , fileException.m_cause);
		}
		myFile.Close();
	}

	return res;
}


// CCableSizingSettingPropPage message handlers
/**
	@brief	

	@author	BHK
*/
void CCableSizingSettingPropPage::OnBnClickedButton3phMotor()
{
	SetDlgItemText(IDC_EDIT_3PH_MOTOR , GetExcelFilePath());
	
	CString rString;
	GetDlgItemText(IDC_EDIT_3PH_MOTOR , rString);
	if(!rString.IsEmpty())
	{
		SetCableSizeExcelFile(_T("T_3PH_MOTOR_CABLE_SIZING") , rString);
	}
}

void CCableSizingSettingPropPage::OnBnClickedButton3phFeeder()
{
	SetDlgItemText(IDC_EDIT_3PH_FEEDER , GetExcelFilePath());

	CString rString;
	GetDlgItemText(IDC_EDIT_3PH_FEEDER , rString);
	if(!rString.IsEmpty())
	{
		SetCableSizeExcelFile(_T("T_3PH_FEEDER_CABLE_SIZING") , rString);
	}
}

void CCableSizingSettingPropPage::OnBnClickedButton1phFeeder()
{
	SetDlgItemText(IDC_EDIT_1PH_FEEDER , GetExcelFilePath());

	CString rString;
	GetDlgItemText(IDC_EDIT_1PH_FEEDER , rString);
	if(!rString.IsEmpty())
	{
		SetCableSizeExcelFile(_T("T_1PH_FEEDER_CABLE_SIZING") , rString);
	}
}

void CCableSizingSettingPropPage::OnBnClickedButtonDcFeeder()
{
	SetDlgItemText(IDC_EDIT_DC_FEEDER , GetExcelFilePath());

	CString rString;
	GetDlgItemText(IDC_EDIT_DC_FEEDER , rString);
	if(!rString.IsEmpty())
	{
		SetCableSizeExcelFile(_T("T_DC_FEEDER_CABLE_SIZING") , rString);
	}
}

/**
	@brief	excel 파일을 읽어서 database에 저장한다.

	@author	BHK
*/
int CCableSizingSettingPropPage::SetCableSizeExcelFile(const CString& rTableName, const CString& rExcelFilePath)
{
	CExcelAutomation xlApp;

	//! 엑셀 파일은 오픈한다.
	if(!xlApp.Open(rExcelFilePath))
	{
		AfxMessageBox("Can't Start Excel and get Application object.");
		return ERROR_BAD_ENVIRONMENT;
	}

	long lowerRow = 0 , lowerCol = 0 , upperRow = 0 , upperCol = 0;
	xlApp.GetUsedRange(lowerRow , lowerCol , upperRow , upperCol);
	
	if("T_3PH_MOTOR_CABLE_SIZING" == rTableName)
	{
		C3PHMotorCableSizingTable& table = C3PHMotorCableSizingTable::GetInstance();
		table.Clear();
		for(int nRow = 2; nRow <= upperRow;++nRow)
		{
			const CString rCellNo = xlApp.MakeExcelNoString(nRow,1);
			CString rCellValue;
			xlApp.GetCellValue(COleVariant(rCellNo) , rCellValue);
			
			C3PHMotorCableSizingTable::Record record;
			record.rKW = rCellValue.operator LPCSTR();
			table.Insert(record);
		}
		table.Save();
	}
	else if("T_3PH_FEEDER_CABLE_SIZING" == rTableName)
	{
		C3PHFeederCableSizingTable& table = C3PHFeederCableSizingTable::GetInstance();
		table.Clear();
		for(int nRow = 2; nRow <= upperRow;++nRow)
		{
			const CString rCellNo = xlApp.MakeExcelNoString(nRow,1);
			CString rCellValue;
			xlApp.GetCellValue(COleVariant(rCellNo) , rCellValue);

			C3PHFeederCableSizingTable::Record record;
			record.rKW = rCellValue.operator LPCSTR();
			table.Insert(record);
		}
		table.Save();
	}
	else if("T_1PH_FEEDER_CABLE_SIZING" == rTableName)
	{
		C1PHFeederCableSizingTable& table = C1PHFeederCableSizingTable::GetInstance();
		for(int nRow = 2; nRow <= upperRow;++nRow)
		{
			const CString rCellNo = xlApp.MakeExcelNoString(nRow,1);
			CString rCellValue;
			xlApp.GetCellValue(COleVariant(rCellNo) , rCellValue);

			C1PHFeederCableSizingTable::Record record;
			record.rKW = rCellValue.operator LPCSTR();
			table.Insert(record);
		}
		table.Save();
	}
	else if("T_DC_FEEDER_CABLE_SIZING" == rTableName)
	{
		CDCFeederCableSizingTable& table = CDCFeederCableSizingTable::GetInstance();
		table.Clear();
		for(int nRow = 2; nRow <= upperRow;++nRow)
		{
			const CString rCellNo = xlApp.MakeExcelNoString(nRow,1);
			CString rCellValue;
			xlApp.GetCellValue(COleVariant(rCellNo) , rCellValue);

			CDCFeederCableSizingTable::Record record;
			record.rKW = rCellValue.operator LPCSTR();
			table.Insert(record);
		}
		table.Save();
	}

	return ERROR_SUCCESS;
}

BOOL CCableSizingSettingPropPage::OnInitDialog()
{
        CELOADPropPage::OnInitDialog();

	{
		m_wnd3PHMotorStatic.SetColor(RGB(236,233,216));
		m_wnd3PHMotorStatic.SetGradientColor(RGB(236,233,216));
		C3PHMotorCableSizingTable& table = C3PHMotorCableSizingTable::GetInstance();
		table.Load();
		if(0 == table.GetRecordCount())
			m_wnd3PHMotorStatic.SetTextColor(RGB(0,0,0));
		else	m_wnd3PHMotorStatic.SetTextColor(RGB(255,0,0));
	}

	{
		m_wnd3PHFeederStatic.SetColor(RGB(236,233,216));
		m_wnd3PHFeederStatic.SetGradientColor(RGB(236,233,216));

		C3PHFeederCableSizingTable& table = C3PHFeederCableSizingTable::GetInstance();
		table.Load();
		if(0 == table.GetRecordCount())
			m_wnd3PHFeederStatic.SetTextColor(RGB(0,0,0));
		else	m_wnd3PHFeederStatic.SetTextColor(RGB(255,0,0));
	}

	{
		m_wnd1PHFeederStatic.SetColor(RGB(236,233,216));
		m_wnd1PHFeederStatic.SetGradientColor(RGB(236,233,216));

		C1PHFeederCableSizingTable& table = C1PHFeederCableSizingTable::GetInstance();
		table.Load();
		if(0 == table.GetRecordCount())
			m_wnd1PHFeederStatic.SetTextColor(RGB(0,0,0));
		else	m_wnd1PHFeederStatic.SetTextColor(RGB(255,0,0));
	}

	{
		m_wndDCFeederStatic.SetColor(RGB(236,233,216));
		m_wndDCFeederStatic.SetGradientColor(RGB(236,233,216));
		
		CDCFeederCableSizingTable& table = CDCFeederCableSizingTable::GetInstance();
		table.Load();
		if(0 == table.GetRecordCount())
			m_wndDCFeederStatic.SetTextColor(RGB(0,0,0));
		else	m_wndDCFeederStatic.SetTextColor(RGB(255,0,0));
	}

        return TRUE;  // return TRUE unless you set the focus to a control
        // EXCEPTION: OCX Property Pages should return FALSE
}
