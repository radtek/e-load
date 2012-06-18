// InitPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "WizardPropertyPage.h"


// CWizardPropertyPage dialog

IMPLEMENT_DYNAMIC(CWizardPropertyPage, CPropertyPage)

CWizardPropertyPage::CWizardPropertyPage(UINT nIDTemplate)
	: CPropertyPage(nIDTemplate) , m_bModified(false)
{

}

CWizardPropertyPage::~CWizardPropertyPage()
{
}

void CWizardPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWizardPropertyPage, CPropertyPage)
END_MESSAGE_MAP()


// CWizardPropertyPage message handlers
