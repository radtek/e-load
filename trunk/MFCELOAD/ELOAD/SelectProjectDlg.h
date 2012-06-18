#pragma once


// CSelectProjectDlg dialog
#include <gui/ListCtrlEx.h.>
#include <string>
#include "MFCButtonEx.h"

#include <list>
#include "afxcmn.h"
#include "afxwin.h"
using namespace std;

class CSelectProjectDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectProjectDlg)

public:
        typedef struct
	{
		string rProjectNo;
		string rProjectName;
		string rProjectDesc;
	}PROJECT_RECORD;

	CSelectProjectDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectProjectDlg();

        CString m_rProjectNo , m_rProjectName , m_rProjectDesc;

// Dialog Data
	enum { IDD = IDD_SELECT_PROJECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
        virtual BOOL OnInitDialog();
private:
        list<PROJECT_RECORD> m_ProjectRecordEntry;

        int LoadProjectRecordFromDatabase(void);
        int DisplayProjectRecord(void);
public:
        CMFCListCtrl m_wndSelectProjectList;

        CMFCButtonEx m_wndOKButton;
        CMFCButtonEx m_wndCancelButton;

        afx_msg void OnNMClickSelectProject(NMHDR *pNMHDR, LRESULT *pResult);
        CString m_rSelectedProjectEdit;
        afx_msg void OnNMDblclkSelectProject(NMHDR *pNMHDR, LRESULT *pResult);
        afx_msg void OnLvnKeydownSelectProject(NMHDR *pNMHDR, LRESULT *pResult);
};
