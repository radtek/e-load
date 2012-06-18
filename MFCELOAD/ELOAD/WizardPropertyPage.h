#pragma once


// CWizardPropertyPage dialog

class CWizardPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CWizardPropertyPage)

public:
	CWizardPropertyPage(UINT nIDTemplate);
	virtual ~CWizardPropertyPage();

// Dialog Data
	//enum { IDD = IDD_INITPROPERTYPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	bool m_bModified;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Init(){ return Init();};
};
