// SimpleAnimationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "SimpleAnimationDlg.h"


// CSimpleAnimationDlg dialog

IMPLEMENT_DYNAMIC(CSimpleAnimationDlg, CDialog)

CSimpleAnimationDlg::CSimpleAnimationDlg(CWnd* pParent /*=NULL*/)
: CDialog(CSimpleAnimationDlg::IDD, pParent) , m_pThread(NULL) , m_bThreadRunning(FALSE)
{

}

/**
@brief	

@author	BHK
*/
CSimpleAnimationDlg& CSimpleAnimationDlg::GetInstance()
{
	static CSimpleAnimationDlg __INSTANCE__;

	return __INSTANCE__;
}

CSimpleAnimationDlg::~CSimpleAnimationDlg()
{
	try
	{
		if(NULL != m_pThread)
		{
			/*delete m_pThread;
			m_pThread = NULL;*/
			TerminateThread(m_pThread->m_hThread, (DWORD)-1);
		}
	}
	catch(...)
	{
		AfxMessageBox("Thread error ");
	}
}

void CSimpleAnimationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_MYPICTURE, m_Picture);
}


BEGIN_MESSAGE_MAP(CSimpleAnimationDlg, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CSimpleAnimationDlg message handlers
bool CSimpleAnimationDlg::Create( CWnd * pWndParent )
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() == NULL );

	CRect rcDesktop;
	VERIFY(
		::SystemParametersInfo(
		SPI_GETWORKAREA,
		0,
		PVOID(&rcDesktop),
		0
		)
		);

	if(!CDialog::Create(MAKEINTRESOURCE(IDD_SIMPLE_ANIMATION) , pWndParent))
	{
		ASSERT( FALSE );
		return false;
	}

	if(NULL != pWndParent) pWndParent->GetWindowRect(&rcDesktop);
	CRect rcWnd( 0, 0, 0, 0);
	GetWindowRect(&rcWnd);
	rcWnd.OffsetRect(
		( rcDesktop.Width() -  rcWnd.Width() ) * 0.5,
		( rcDesktop.Height() - rcWnd.Height() ) * 0.5
		);

	VERIFY(
		::SetWindowPos(m_hWnd , (NULL != pWndParent) ? pWndParent->GetSafeHwnd() : HWND_TOP , rcWnd.left , rcWnd.top , rcWnd.Width() , rcWnd.Height() , SWP_NOSIZE | SWP_SHOWWINDOW)
		);

	///CSimpleAnimationDlg::PassMsgLoop( false );

	return true;
}

BOOL CSimpleAnimationDlg::DestroyWindow() 
{
	ShowWindow( SW_HIDE );
	///CSimpleAnimationDlg::PassMsgLoop( false );

	return CDialog::DestroyWindow();
}

void CSimpleAnimationDlg::PassMsgLoop(bool bEnableOnIdleCalls)
{
	//__PROF_UIS_MANAGE_STATE;
	AFX_MANAGE_STATE( ::AfxGetAppModuleState() );

	MSG msg;
	// Process all the messages in the message queue
	while( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
	{
		if( !AfxGetThread()->PumpMessage() )
		{
			PostQuitMessage(0);
			return;
		} // if( !AfxGetThread()->PumpMessage() )
	} // while( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
	if( bEnableOnIdleCalls )
	{
		for(	LONG nIdleCounter = 0L;
			::AfxGetThread()->OnIdle(nIdleCounter);
			nIdleCounter ++
			);
	}
}
BOOL CSimpleAnimationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// animate control functions...
	if(m_Picture.Load(MAKEINTRESOURCE( IDR_GIF1 ) , _T("GIF")))
	{
		m_Picture.Draw();
	}

	///::Sleep(100);

	if(NULL != m_pThread)
	{
		m_pThread->m_bAutoDelete = FALSE;
		m_pThread->ResumeThread();
	}
	else
	{
		return FALSE;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSimpleAnimationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		///dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

void CSimpleAnimationDlg::OnOK()
{
	if (m_bThreadRunning)
	{
		WaitForSingleObject(m_pThread->m_hThread, INFINITE);
		if(m_bThreadRunning)
		{
			// we gave the thread a chance to quit. Since the thread didn't
			// listen to us we have to kill it.
			TerminateThread(m_pThread->m_hThread, (DWORD)-1);
			InterlockedExchange((LONG*)(&m_bThreadRunning) , FALSE);
		}
	}

	CDialog::OnOK();
}

void CSimpleAnimationDlg::OnCancel()
{
	if (m_bThreadRunning)
	{
		WaitForSingleObject(m_pThread->m_hThread, INFINITE);
		if(m_bThreadRunning)
		{
			// we gave the thread a chance to quit. Since the thread didn't
			// listen to us we have to kill it.
			TerminateThread(m_pThread->m_hThread, (DWORD)-1);
			InterlockedExchange((LONG*)(&m_bThreadRunning) , FALSE);
		}
	}

	CDialog::OnCancel();
}
