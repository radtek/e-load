#pragma once
#include "../StdAfx.h"
#include "splashscreenex.h"
//#include "afxcmn.h"
#include <gui/TextProgressCtrl.h>

class CSplashScreenFx :	public CSplashScreenEx
{
	DECLARE_DYNAMIC(CSplashScreenFx)

public:
	CSplashScreenFx(void);
	~CSplashScreenFx(void);

	static CSplashScreenFx* GetInstance();
	volatile LONG m_bThreadRunning;
	CWinThread* m_pThread;
protected:
	DECLARE_MESSAGE_MAP()
public:
	void WaitUntilUserConfirm(void);
	CTextProgressCtrl m_ctrlProgress;
	void CreateProgressBar(CRect& rcProgress, CSize& sizeProgress, DWORD dwStyle);
	void ProgressStep(void);
};
