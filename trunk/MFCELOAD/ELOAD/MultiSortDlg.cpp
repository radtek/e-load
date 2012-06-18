// MultiSortDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "MultiSortDlg.h"
#include "ELOADDoc.h"
#include "ELOADView.h"
#include "ELoadDocData.h"

// CMultiSortDlg dialog

IMPLEMENT_DYNAMIC(CMultiSortDlg, CDialog)

CMultiSortDlg::CMultiSortDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMultiSortDlg::IDD, pParent)
{
	m_MultiSortDirEntry.push_back(0);
	m_MultiSortDirEntry.push_back(0);
	m_MultiSortDirEntry.push_back(0);
}

CMultiSortDlg::~CMultiSortDlg()
{
}

void CMultiSortDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX , IDC_STATIC_TITLE , m_wndStaticTitle);

	DDX_Control(pDX , IDC_BUTTON_MULTI_SELECT_COND1 , m_wndMultiSelectCond1);
	DDX_Control(pDX , IDC_BUTTON_MULTI_SELECT_COND2 , m_wndMultiSelectCond2);
	DDX_Control(pDX , IDC_BUTTON_MULTI_SELECT_COND3 , m_wndMultiSelectCond3);
	DDX_Control(pDX , IDC_BUTTON_MULTI_SORT_DIR1 , m_wndMultiSortDir1);
	DDX_Control(pDX , IDC_BUTTON_MULTI_SORT_DIR2 , m_wndMultiSortDir2);
	DDX_Control(pDX , IDC_BUTTON_MULTI_SORT_DIR3 , m_wndMultiSortDir3);

	DDX_Control(pDX , IDC_BUTTON_MULTI_SELECT_SHOW  , m_wndMultiSelectShow);

	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMultiSortDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_MULTI_SELECT_COND1, &CMultiSortDlg::OnBnClickedButtonMultiSelectCond1)
	ON_BN_CLICKED(IDC_BUTTON_MULTI_SELECT_COND2, &CMultiSortDlg::OnBnClickedButtonMultiSelectCond2)
	ON_BN_CLICKED(IDC_BUTTON_MULTI_SELECT_COND3, &CMultiSortDlg::OnBnClickedButtonMultiSelectCond3)
	ON_BN_CLICKED(IDC_BUTTON_MULTI_SELECT_SHOW, &CMultiSortDlg::OnBnClickedButtonMultiSelectShow)
	ON_BN_CLICKED(IDC_BUTTON_MULTI_SORT_DIR1, &CMultiSortDlg::OnBnClickedButtonMultiSortDir1)
	ON_BN_CLICKED(IDC_BUTTON_MULTI_SORT_DIR2, &CMultiSortDlg::OnBnClickedButtonMultiSortDir2)
	ON_BN_CLICKED(IDC_BUTTON_MULTI_SORT_DIR3, &CMultiSortDlg::OnBnClickedButtonMultiSortDir3)
END_MESSAGE_MAP()


// CMultiSortDlg message handlers

BOOL CMultiSortDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_wndStaticTitle.SetColor( RGB(248, 248, 243) );
	m_wndStaticTitle.SetGradientColor( RGB(189, 189, 163) );
	m_wndStaticTitle.SetVerticalGradient();
	m_wndStaticTitle.SetTextColor(RGB(0,0,0));

	CELoadDocData & docData = CELoadDocData::GetInstance();
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	m_wndMultiSortDir1.SetTextHotColor(TextHotColor);
	m_wndMultiSortDir2.SetTextHotColor(TextHotColor);
	m_wndMultiSortDir3.SetTextHotColor(TextHotColor);

	m_wndMultiSortDir1.SetIcon(IDI_ASCENDING );
	m_wndMultiSortDir2.SetIcon(IDI_ASCENDING );
	m_wndMultiSortDir3.SetIcon(IDI_ASCENDING );

	m_MultiSortCellNoEntry.push_back(-1);
	m_MultiSortCellNoEntry.push_back(-1);
	m_MultiSortCellNoEntry.push_back(-1);

	///CMFCButton::SetButtonStyle(
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CMultiSortDlg::OnBnClickedButtonMultiSelectCond1()
{
	CString rString;
	m_wndMultiSelectCond1.GetWindowText(rString);
	if(!rString.IsEmpty())
	{
		m_wndMultiSelectCond1.SetWindowText("");
		m_MultiSortCellNoEntry[0] = -1;
	}
	else
	{
		CELOADView* pView = (CELOADView*)GetParent();
		if(pView)
		{
			int nCellNo = 0;
			m_wndMultiSelectCond1.SetWindowText(pView->GetCurSelColumnName(nCellNo));
			m_MultiSortCellNoEntry[0] = nCellNo;
		}
	}

}

void CMultiSortDlg::OnBnClickedButtonMultiSelectCond2()
{
	CString rString;
	m_wndMultiSelectCond2.GetWindowText(rString);
	if(!rString.IsEmpty())
	{
		m_wndMultiSelectCond2.SetWindowText("");
		m_MultiSortCellNoEntry[1] = -1;
	}
	else
	{
		CELOADView* pView = (CELOADView*)GetParent();
		if(pView)
		{
			int nCellNo = 0;
			m_wndMultiSelectCond2.SetWindowText(pView->GetCurSelColumnName(nCellNo));
			m_MultiSortCellNoEntry[1] = nCellNo;
		}
	}
}

void CMultiSortDlg::OnBnClickedButtonMultiSelectCond3()
{
	CString rString;
	m_wndMultiSelectCond3.GetWindowText(rString);
	if(!rString.IsEmpty())
	{
		m_wndMultiSelectCond3.SetWindowText("");
		m_MultiSortCellNoEntry[2] = -1;
	}
	else
	{
		CELOADView* pView = (CELOADView*)GetParent();
		if(pView)
		{
			int nCellNo = 0;
			m_wndMultiSelectCond3.SetWindowText(pView->GetCurSelColumnName(nCellNo));
			m_MultiSortCellNoEntry[2] = nCellNo;
		}
	}
}

void CMultiSortDlg::OnBnClickedButtonMultiSelectShow()
{
	CELOADView* pView = (CELOADView*)GetParent();
	if(pView) pView->OnMultiSort();
}

/**
	@brief	

	@author	BHK
*/
int CMultiSortDlg::GetMultiSortInfoEntry(vector<int>& MultiSortCellNoEntry , vector<int>& MultiSortDirEntry)
{
	MultiSortCellNoEntry.clear();
	MultiSortCellNoEntry.insert(MultiSortCellNoEntry.begin(),m_MultiSortCellNoEntry.begin() , m_MultiSortCellNoEntry.end());

	MultiSortDirEntry.clear();
	MultiSortDirEntry.insert(MultiSortDirEntry.begin(),m_MultiSortDirEntry.begin() , m_MultiSortDirEntry.end());

	return ERROR_SUCCESS;
}

void CMultiSortDlg::OnBnClickedButtonMultiSortDir1()
{
	m_MultiSortDirEntry[0] = (0 == m_MultiSortDirEntry[0]) ? 1 : 0;
	m_wndMultiSortDir1.SetIcon((0 == m_MultiSortDirEntry[0]) ? IDI_ASCENDING : IDI_DESCENDING );
}

void CMultiSortDlg::OnBnClickedButtonMultiSortDir2()
{
	m_MultiSortDirEntry[1] = (0 == m_MultiSortDirEntry[1]) ? 1 : 0;
	m_wndMultiSortDir2.SetIcon((0 == m_MultiSortDirEntry[1]) ? IDI_ASCENDING : IDI_DESCENDING );
}

void CMultiSortDlg::OnBnClickedButtonMultiSortDir3()
{
	m_MultiSortDirEntry[2] = (0 == m_MultiSortDirEntry[2]) ? 1 : 0;
	m_wndMultiSortDir3.SetIcon((0 == m_MultiSortDirEntry[2]) ? IDI_ASCENDING : IDI_DESCENDING );
}
