// WizardDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "WizardDlg.h"
#include "ELoadDocData.h"

// CWizardDlg dialog

IMPLEMENT_DYNAMIC(CWizardDlg, CDialog)

CWizardDlg::CWizardDlg(const int& nStartPage , CWnd* pParent /*=NULL*/)
	: CDialog(CWizardDlg::IDD, pParent) , m_nCurPage(0)
{
	m_nCurPage = nStartPage;
	m_pCurPage = NULL;
}

CWizardDlg::~CWizardDlg()
{
	//! GDI OBJECT 삭제.
	if(m_hIcon) ::DeleteObject(m_hIcon);
}

void CWizardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWizardDlg, CDialog)
	ON_BN_CLICKED(ID_WIZNEXT, &CWizardDlg::OnBnClickedWiznext)
	ON_BN_CLICKED(ID_WIZBACK, &CWizardDlg::OnBnClickedWizback)
END_MESSAGE_MAP()


// CWizardDlg message handlers
/**
	@brief	

	@author BHK	

	@date 2009-04-21 오후 8:27:26	

	@param	

	@return		
*/
int CWizardDlg::AddPage(UINT nIDTemplate , CWizardPropertyPage* pPage , const string& rHelperString)
{
	CWizardDlg::PageInfo page;
	page.nIDTemplate = nIDTemplate;
	page.pPage = pPage;
	page.rHelperString = rHelperString;

	m_PageList.push_back(page);

	return 0;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-21 오후 8:29:02	

	@param	

	@return		
*/
BOOL CWizardDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	if(m_nCurPage < int(m_PageList.size()))
	{
		if(m_PageList[m_nCurPage].pPage->Create(m_PageList[m_nCurPage].nIDTemplate , this))
		{
			SetCurPage();
		}
	}
	
	//! ICON
	HICON hIcon = AfxGetApp()->LoadIcon(IDI_WIZARDLARGE);
	SetIcon(hIcon, FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	

	@author BHK	

	@date 2009-04-21 오후 9:01:30	

	@param	

	@return		
*/
void CWizardDlg::OnBnClickedWiznext()
{
	if((m_nCurPage + 1) < m_PageList.size())
	{
		++m_nCurPage;
		if(FALSE == ::IsWindow(m_PageList[m_nCurPage].pPage->GetSafeHwnd()))
		{
			m_PageList[m_nCurPage].pPage->Create(m_PageList[m_nCurPage].nIDTemplate , this);
		}
		SetCurPage();
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-04-21 오후 11:42:56	

	@param	

	@return		
**/
void CWizardDlg::OnBnClickedWizback()
{
	if(m_nCurPage > 0)
	{
		--m_nCurPage;
		if(FALSE == ::IsWindow(m_PageList[m_nCurPage].pPage->GetSafeHwnd()))
		{
			m_PageList[m_nCurPage].pPage->Create(m_PageList[m_nCurPage].nIDTemplate , this);
		}
		SetCurPage();
	}
}

/**
	@brief	wizard dialog의 크기를 현재 page에 맞게 조정합니다.

	@author BHK	

	@date 2009-04-21 오후 11:53:48	

	@param	

	@return		
**/
int CWizardDlg::SetCurPage(void)
{
	if(::IsWindow(m_PageList[m_nCurPage].pPage->GetSafeHwnd()))
	{
		CRect rect , rectClient;
		GetClientRect(&rectClient);
		GetWindowRect(&rect);
		const int nPaddWidth = rect.Width() - rectClient.Width();
		const int nPaddHeight= rect.Height()- rectClient.Height();
		rect.SetRectEmpty();

		m_PageList[m_nCurPage].pPage->ModifyStyleEx(WS_POPUP , WS_CHILD);
		m_PageList[m_nCurPage].pPage->SetWindowPos(NULL , rect.left , rect.bottom , 0 , 0 , SWP_NOSIZE | SWP_NOZORDER);

		CRect _rect;
		m_PageList[m_nCurPage].pPage->GetWindowRect(&_rect);
		ScreenToClient(&_rect);

		if(m_pCurPage) m_pCurPage->ShowWindow(SW_HIDE);
		SetWindowPos(NULL , 0 , 0 , nPaddWidth + _rect.right , nPaddHeight + _rect.bottom , SWP_NOMOVE | SWP_NOZORDER);
		m_PageList[m_nCurPage].pPage->ShowWindow(SW_SHOW);
		m_pCurPage = m_PageList[m_nCurPage].pPage;

		CWnd* pBackWnd = m_PageList[m_nCurPage].pPage->GetDlgItem(ID_WIZBACK);
		if(pBackWnd)
		{
			(0 == m_nCurPage) ? pBackWnd->EnableWindow(FALSE) : pBackWnd->EnableWindow(TRUE);
		}
		CWnd* pNextWnd = m_PageList[m_nCurPage].pPage->GetDlgItem(ID_WIZNEXT);
		if(pNextWnd)
		{
			((m_nCurPage + 1) < m_PageList.size()) ? pNextWnd->EnableWindow(TRUE) : pNextWnd->EnableWindow(FALSE);
		}

		if(::IsWindow(m_PageList[m_nCurPage].pPage->GetSafeHwnd()))
		{
			this->SetWindowText(m_PageList[m_nCurPage].rHelperString.c_str());
			m_PageList[m_nCurPage].pPage->Init();
		}
		
		return ERROR_SUCCESS;
	}

	RedrawWindow();

	return ERROR_BAD_ENVIRONMENT;
}
