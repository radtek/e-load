#pragma once

#include "MFCButtonEx.h"
#include <vector>
#include "afxwin.h"
#include "WizardPropertyPage.h"

using namespace std;

// CWizardDlg dialog

class CWizardDlg : public CDialog
{
	DECLARE_DYNAMIC(CWizardDlg)
	
	typedef struct
	{
		UINT nIDTemplate;
		CWizardPropertyPage* pPage;
		string rHelperString;
	}PageInfo;
public:
	CWizardDlg(const int& nStartPage , CWnd* pParent = NULL);   // standard constructor
	virtual ~CWizardDlg();

// Dialog Data
	enum { IDD = IDD_WIZARD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int AddPage(UINT nIDTemplate , CWizardPropertyPage* pPage , const string& rHelperString);
private:
	CWizardPropertyPage* m_pCurPage;
	int m_nCurPage;
	vector<PageInfo> m_PageList;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedWiznext();
	afx_msg void OnBnClickedWizback();
private:
	HICON m_hIcon;
	int SetCurPage(void);
};
