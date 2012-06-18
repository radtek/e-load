// SimpleProgressBar.cpp : implementation file
//

#include "stdafx.h"
#include "SimpleProgressBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	PROGRESS_WND_WIDTH	230
#define	PROGRESS_WND_HEIGHT	25

/////////////////////////////////////////////////////////////////////////////
// CSimpleProgressBar window
/////////////////////////////////////////////////////////////////////////////
// CSimpleProgressBar

static CSimpleProgressBar* __INSTANCE__ = NULL;
static bool g_bSimpleProgressBarWndClassRegistered = false;

CSimpleProgressBar::CSimpleProgressBar()
	: m_sStatusText( _T("Initializing ...") )
{
	VERIFY( RegisterSplashWndClass() );
	__initControls();

	__INSTANCE__ = this;
}

CSimpleProgressBar::CSimpleProgressBar(CWnd * pWndParent) : m_sStatusText( _T("Initializing ...") )
{
	VERIFY( Create( pWndParent) );
	__initControls();

	__INSTANCE__ = this;
}

CSimpleProgressBar::~CSimpleProgressBar()
{
	m_Font.DeleteObject();
	__INSTANCE__ = NULL;
}


BEGIN_MESSAGE_MAP(CSimpleProgressBar, CDialog)
	//{{AFX_MSG_MAP(CSimpleProgressBar)
	///ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_NCCALCSIZE()
	ON_WM_NCHITTEST()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	///ON_WM_CREATE()
END_MESSAGE_MAP()

void CSimpleProgressBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
        DDX_Control(pDX , IDC_PROGRESS , m_wndProgressCtrl);
}

/////////////////////////////////////////////////////////////////////////////
// CSimpleProgressBar message handlers

void CSimpleProgressBar::SetStatusText(LPCTSTR rStatusText , const int& nProgress )
{
	if(GetSafeHwnd() && m_wndProgressCtrl.GetSafeHwnd())
	{
		m_wndProgressCtrl.SetWindowText(rStatusText);
		m_wndProgressCtrl.SetPos(nProgress);
	}
}

bool CSimpleProgressBar::Create( CWnd * pWndParent )
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() == NULL );
	ASSERT( m_bitmap.GetSafeHandle() == NULL );

	/*
	if( !RegisterSplashWndClass() )
	{
		ASSERT( FALSE );
		return false;
	}
	*/

	CRect rcDesktop;
	VERIFY(
		::SystemParametersInfo(
			SPI_GETWORKAREA,
			0,
			PVOID(&rcDesktop),
			0
			)
		);
	
	if(!CDialog::Create(MAKEINTRESOURCE(IDD_SIMPLE_PROGRESSBAR) , pWndParent))
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
///		CDialog::SetWindowPos(
		::SetWindowPos(m_hWnd , (NULL != pWndParent) ? pWndParent->GetSafeHwnd() : HWND_TOP , rcWnd.left , rcWnd.top , rcWnd.Width() , rcWnd.Height() , SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE)
///		::SetWindowPos(
///			m_hWnd, HWND_TOP, rcWnd., 0, 0, 0,
//			SWP_NOSIZE|SWP_SHOWWINDOW
//			)
		);
	RedrawWindow();

	CSimpleProgressBar::PassMsgLoop( false );

	return true;
}

/**
	@brief	

	@author	BHK
*/
void CSimpleProgressBar::__initControls()
{
	VERIFY(m_Font.CreateFont(
		16,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"Arial"));
}

/*
BOOL CSimpleProgressBar::OnEraseBkgnd(CDC* pDC) 
{
	pDC;
	return TRUE;
}
*/

void CSimpleProgressBar::OnPaint() 
{
	CDialog::OnPaint();
}

void CSimpleProgressBar::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	bCalcValidRects;
	lpncsp;
}

LRESULT CSimpleProgressBar::OnNcHitTest(CPoint point) 
{
	point;
	return HTCLIENT;
}

void CSimpleProgressBar::OnClose() 
{
}

BOOL CSimpleProgressBar::PreCreateWindow(CREATESTRUCT& cs) 
{
	if( ( !RegisterSplashWndClass() )
		|| ( !CWnd::PreCreateWindow(cs) )
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}

	cs.lpszClass = SIMPLE_PROGRESSBAR_WNDCLASS;

	return TRUE;
}

bool CSimpleProgressBar::RegisterSplashWndClass()
{
	if( g_bSimpleProgressBarWndClassRegistered )
		return true;

WNDCLASS _wndClassInfo;
HINSTANCE hInst = AfxGetInstanceHandle();
	if( ! ::GetClassInfo(
			hInst,
			SIMPLE_PROGRESSBAR_WNDCLASS,
			&_wndClassInfo
			)
		)
	{
		// otherwise we need to register a new class
		_wndClassInfo.style =
				CS_GLOBALCLASS
				//|CS_DBLCLKS
				|CS_HREDRAW|CS_VREDRAW
				|CS_NOCLOSE
				|CS_SAVEBITS
				;
		_wndClassInfo.lpfnWndProc = ::DefWindowProc;
		_wndClassInfo.cbClsExtra = _wndClassInfo.cbWndExtra = 0;
		_wndClassInfo.hInstance = hInst;
		_wndClassInfo.hIcon = NULL;
		_wndClassInfo.hCursor =
				::LoadCursor(
					NULL, //hInst,
					IDC_WAIT
					)
				;
		ASSERT( _wndClassInfo.hCursor != NULL );
		_wndClassInfo.hbrBackground = NULL; 
		_wndClassInfo.lpszMenuName = NULL;
		_wndClassInfo.lpszClassName = SIMPLE_PROGRESSBAR_WNDCLASS;
		if( !::AfxRegisterClass( &_wndClassInfo ) )
		{
			ASSERT( FALSE );
			//AfxThrowResourceException();
			return false;
		}
	}

	g_bSimpleProgressBarWndClassRegistered = true;
	return true;
}

BOOL CSimpleProgressBar::DestroyWindow() 
{
	ShowWindow( SW_HIDE );
	CSimpleProgressBar::PassMsgLoop( false );
	
	return CDialog::DestroyWindow();
}

void CSimpleProgressBar::PassMsgLoop(
	bool bEnableOnIdleCalls
	)
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

/*
int CSimpleProgressBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect;
	GetClientRect(&rect);
	m_wndProgressCtrl.Create(WS_CHILD | WS_VISIBLE , rect , this , 0x100);

	return 0;
}
*/