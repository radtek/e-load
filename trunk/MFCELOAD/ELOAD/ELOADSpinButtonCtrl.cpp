// ELOADSpinButtonCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "MFCPropEx.h"
#include "ELOADSpinButtonCtrl.h"


// CELOADSpinButtonCtrl

IMPLEMENT_DYNAMIC(CELOADSpinButtonCtrl, CMFCSpinButtonCtrl)

CELOADSpinButtonCtrl::CELOADSpinButtonCtrl() : m_pBuddy(NULL)
{

}

CELOADSpinButtonCtrl::~CELOADSpinButtonCtrl()
{
}


BEGIN_MESSAGE_MAP(CELOADSpinButtonCtrl, CMFCSpinButtonCtrl)
	ON_NOTIFY_REFLECT(UDN_DELTAPOS, OnDeltapos)
END_MESSAGE_MAP()

/**
	@brief	

	@author	BHK
*/
CMFCPropEx* CELOADSpinButtonCtrl::SetBuddy( CMFCPropEx* pBuddy )
{
	CMFCPropEx* pOldBuddy = m_pBuddy;
	m_pBuddy = pBuddy;

	return pOldBuddy;
}

/**
	@brief	

	@author	BHK
*/
CMFCPropEx* CELOADSpinButtonCtrl::GetBuddy( ) const
{
	return m_pBuddy;
}

// CELOADSpinButtonCtrl message handlers

void CELOADSpinButtonCtrl::OnDeltapos (NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN> (pNMHDR);
	
	int nPos = pNMUpDown->iPos + pNMUpDown->iDelta;
	if(nPos < 0) nPos = GetRange();
	if(nPos > GetRange()) nPos = 0;
	SetPos(nPos);

	CMFCPropEx* pPropEx = GetBuddy();
	if(pPropEx)
	{
		pPropEx->SetCurUnitPos(nPos);
	}

	*pResult = -1;
}
