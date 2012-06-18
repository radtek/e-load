// ELOADPropSheet.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "ELOADPropSheet.h"
#include "ELoadDocData.h"


// CELOADPropSheet

IMPLEMENT_DYNAMIC(CELOADPropSheet, CMFCPropertySheet)

CELOADPropSheet::CELOADPropSheet()
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW | PSH_USEHICON;
	m_psh.dwFlags &= ~PSH_HASHELP;

	SetLook( CMFCPropertySheet::PropSheetLook_OneNoteTabs );
}

CELOADPropSheet::~CELOADPropSheet()
{
}


BEGIN_MESSAGE_MAP(CELOADPropSheet, CMFCPropertySheet)
END_MESSAGE_MAP()



// CELOADPropSheet message handlers



BOOL CELOADPropSheet::OnInitDialog()
{
	BOOL bResult = CMFCPropertySheet::OnInitDialog();
        
        SetWindowText("Project Setting");

        return bResult;
}

/**
	@brief	

	@author	BHK
*/
BOOL CELOADPropSheet::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// crack message parameters
	UINT nID  = LOWORD(wParam);
	///int nCode = HIWORD(wParam);

	// set m_nModalResult to ID of button, whenever button is clicked
	///if (hWndCtrl != NULL && nCode == BN_CLICKED)
	{
		if (nID == IDOK)
		{
			const int nCount = GetPageCount();
			for(int i = 0;i < nCount;++i)
			{
				CELOADPropPage* pPage = (CELOADPropPage*)GetPage(i);
				if(pPage && pPage->GetSafeHwnd())
				{
                                        //! LoadSummaryPropPage 시트 내용의 데이타를 넘겨준다

                                        //! 저장한다.
                                        pPage->SaveData();
				}
			}
		}
		else if (nID == ID_APPLY_NOW)
		{
		}
		else if (nID == IDCANCEL)
		{
		}
	}


	return CMFCPropertySheet::OnCommand(wParam, lParam);
}


IMPLEMENT_DYNAMIC(CELOADPropPage, CMFCPropertyPage)

CELOADPropPage::CELOADPropPage(UINT nIDTemplate , UINT nIDCaption)
	: CMFCPropertyPage(nIDTemplate)
{

}

CELOADPropPage::~CELOADPropPage()
{
}

void CELOADPropPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CELOADPropPage, CMFCPropertyPage)
END_MESSAGE_MAP()

/**
	@brief	

	@author	BHK
*/
BOOL CELOADPropSheet::PreTranslateMessage(MSG* pMsg)
{
	return CMFCPropertySheet::PreTranslateMessage(pMsg);
}

BOOL CELOADPropPage::PreTranslateMessage(MSG* pMsg)
{
	//! 리턴키나 ESCAPE KEY로 윈도우가 닫기는 것을 방지.
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE) return TRUE;
		if(pMsg->wParam == VK_RETURN) return TRUE;
	}

	return CMFCPropertyPage::PreTranslateMessage(pMsg);
}
