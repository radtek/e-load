// DataResultDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "DataResultDlg.h"
#include "ELoadDocData.h"
#include "Tokenizer.h"
// CDataResultDlg dialog

IMPLEMENT_DYNAMIC(CDataResultDlg, CDialog)

CDataResultDlg::CDataResultDlg(const CString& rMsg, CWnd* pParent /*=NULL*/)
	: CDialog(CDataResultDlg::IDD, pParent), m_rMsg(rMsg)
{

}

CDataResultDlg::~CDataResultDlg()
{
}

void CDataResultDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_wndOkButton);
	DDX_Control(pDX, IDC_STATIC1, m_wnd1Static);
	DDX_Control(pDX, IDC_STATIC2, m_wnd2Static);
	DDX_Control(pDX, IDC_STATIC3, m_wnd3Static);
	DDX_Control(pDX, IDC_STATIC4, m_wnd4Static);
	DDX_Control(pDX, IDC_STATIC5, m_wnd5Static);
	DDX_Control(pDX, IDC_STATIC6, m_wnd6Static);
	DDX_Control(pDX, IDC_STATIC7, m_wnd7Static);
	DDX_Control(pDX, IDC_GROUP_STATIC, m_wndResultGroupStatic);
}


BEGIN_MESSAGE_MAP(CDataResultDlg, CDialog)
END_MESSAGE_MAP()


// CDataResultDlg message handlers

BOOL CDataResultDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CELoadDocData& docData = CELoadDocData::GetInstance();

	//! Button Setting
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	m_wndOkButton.SetTextHotColor(TextHotColor);
	m_wndOkButton.SetIcon(IDI_OK_18X18);

	const COLORREF GroupBoxTextColor = RGB(0,70,213);
	m_wndResultGroupStatic.SetTextColor(GroupBoxTextColor, FALSE);
	
	m_wnd5Static.SetFontBold(TRUE);
	m_wnd6Static.SetFontBold(TRUE);
	m_wnd7Static.SetFontBold(TRUE);
	m_wnd4Static.SetFontBold(TRUE);
	
	vector<string> oResult;
	CTokenizer<CIsFromString>::Tokenize(oResult, m_rMsg.operator LPCSTR(), CIsFromString("|"));
	if(7 == oResult.size())
	{
		m_wnd1Static.SetWindowText(oResult[0].c_str());
		m_wnd2Static.SetWindowText(oResult[2].c_str());
		m_wnd3Static.SetWindowText(oResult[4].c_str());
		m_wnd4Static.SetWindowText(oResult[6].c_str());

		m_wnd5Static.SetWindowText(oResult[1].c_str());
		m_wnd6Static.SetWindowText(oResult[3].c_str());
		m_wnd7Static.SetWindowText(oResult[5].c_str());
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
