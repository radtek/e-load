// ShowRemarkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "ShowRemarkDlg.h"
#include "MainFrm.h"


// CShowRemarkDlg dialog

IMPLEMENT_DYNAMIC(CShowRemarkDlg, CDialog)

CShowRemarkDlg::CShowRemarkDlg(ITEMS::CLoadItem* pLoadItem , CWnd* pParent /*=NULL*/)
	: CDialog(CShowRemarkDlg::IDD, pParent) , m_pLoadItem(pLoadItem)
{
}

CShowRemarkDlg::~CShowRemarkDlg()
{
}

void CShowRemarkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX , IDC_LIST_REMARK , m_wndRemarkListCtrl);
	DDX_Control(pDX , IDC_BUTTON_ADD , m_wndAddButton);
	DDX_Control(pDX , IDOK , m_wndOKButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);
}


BEGIN_MESSAGE_MAP(CShowRemarkDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CShowRemarkDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDOK, &CShowRemarkDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CShowRemarkDlg message handlers

/**
	@brief	

	@author BHK	

	@date 2009-05-19 오후 2:17:16	

	@param	

	@return		
*/
BOOL CShowRemarkDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_wndRemarkListCtrl.InsertColumn(0 , _T("Date") , LVCFMT_LEFT , 300);
	m_wndRemarkListCtrl.InsertColumn(1 , _T("Remark") , LVCFMT_LEFT , 500);

	if(m_pLoadItem)
	{
		const string rRemark = m_pLoadItem->prop()->GetValue(_T("Remark") , _T("REMARK"));
		
		int i = 0;
		vector<string> oResult;
		CTokenizer<CIsFromString>::Tokenize(oResult , rRemark , CIsFromString("\n"));
		for(vector<string>::iterator itr = oResult.begin();itr != oResult.end();++itr)
		{
			if(_T("") == (*itr)) continue;

			vector<string> oRemark;
			CTokenizer<CIsFromString>::Tokenize(oRemark , (*itr) , CIsFromString("\t"));
			if(oRemark.size() > 0)
			{
				const int nItem = m_wndRemarkListCtrl.InsertItem(i++ , oRemark[0].c_str());
				if((-1 != nItem) && (oRemark.size() > 1))
				{
					m_wndRemarkListCtrl.SetItemText(nItem , 1 , oRemark[1].c_str());
				}
			}
		}
	}

	m_wndOKButton.SetIcon(IDI_OK_18X18);
	m_wndCancelButton.SetIcon(IDI_CANCEL_18X18);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	

	@author BHK	

	@date 2009-05-19 오후 2:32:23	

	@param	

	@return		
*/
void CShowRemarkDlg::OnBnClickedButtonAdd()
{
	CString rString;
	GetDlgItemText(IDC_EDIT_REMARK , rString);

	if(!rString.IsEmpty())
	{
		CTime CurTime = CTime::GetCurrentTime();
		CString strTimeData = CurTime.Format("%Y년 %m월 %d일");
		const int nItem = m_wndRemarkListCtrl.InsertItem(0 , strTimeData);
		if(-1 != nItem) m_wndRemarkListCtrl.SetItemText(nItem , 1 , rString);
	}else	AfxMessageBox(_T("Remark를 입력하세요."));
}

/**
	@brief	

	@author BHK	

	@date 2009-05-19 오후 2:35:49	

	@param	

	@return		
*/
void CShowRemarkDlg::OnBnClickedOk()
{
	if(m_pLoadItem)
	{
		CString rRemark;
		for(int i = 0;i < m_wndRemarkListCtrl.GetItemCount();++i)
		{
			rRemark += m_wndRemarkListCtrl.GetItemText(i , 0) + _T("\t");
			rRemark += m_wndRemarkListCtrl.GetItemText(i , 1) + _T("\n");
		}
		if(_T("\n") == rRemark.Right(1)) rRemark = rRemark.Left(rRemark.GetLength() - 1);
		
		m_pLoadItem->prop()->SetValue(_T("Remark") , _T("REMARK") , rRemark.operator LPCSTR());
	}

	OnOK();
}
