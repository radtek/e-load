#include "StdAfx.h"
#include "splashscreenfx.h"
#include "../resource.h"

IMPLEMENT_DYNAMIC(CSplashScreenFx, CSplashScreenEx)

static CSplashScreenFx* __INSTANCE__ = NULL;

CSplashScreenFx::CSplashScreenFx(void) : m_pThread(NULL) , m_bThreadRunning(FALSE)
{
	__INSTANCE__ = this;
}

CSplashScreenFx::~CSplashScreenFx(void)
{
	///if(m_pProgress) SAFE_DELETE(m_pProgress);
	__INSTANCE__ = NULL;
}

CSplashScreenFx* CSplashScreenFx::GetInstance()
{
	return __INSTANCE__;
}

BEGIN_MESSAGE_MAP(CSplashScreenFx, CSplashScreenEx)
END_MESSAGE_MAP()


void CSplashScreenFx::WaitUntilUserConfirm(void)
{
	MSG message;
	while(1)
	{
		if(::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
			if(message.message == WM_LBUTTONDOWN)
				break;
			if(message.message == WM_KEYDOWN)
				break;
		}
	}
}

void CSplashScreenFx::CreateProgressBar(CRect& rcProgress, CSize& sizeProgress, DWORD dwStyle)
{
	AFX_MANAGE_STATE( ::AfxGetAppModuleState() );
	m_ctrlProgress.DestroyWindow();
	m_ctrlProgress.Create( WS_VISIBLE | WS_CHILD | dwStyle, rcProgress, this, 1);
	m_ctrlProgress.SetShowText(FALSE);

	/*
	CWnd*    pProgress = NULL;
	CBitmap* pBitmap   = NULL;

	pProgress = &m_ctrlProgress;

	pBitmap = new CBitmap();
	pBitmap->LoadBitmap(IDB_BMP_PHOTON);

	m_pProgress = new CTextProgressCtrl();/// (pProgress,	sizeProgress.cy, pBitmap);

	delete pBitmap;
	*/
}

void CSplashScreenFx::ProgressStep(void)
{
	if(m_ctrlProgress.GetSafeHwnd()) m_ctrlProgress.StepIt();
}
