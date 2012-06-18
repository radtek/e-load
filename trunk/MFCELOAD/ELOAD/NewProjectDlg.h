#pragma once

#include "MFCButtonEx.h"
#include <gui/ListCtrlEx.h.>
#include <gui/FilterEdit/BoostBaseFilterEdit.h>

#include <list>
using namespace std;
// CNewProjectDlg dialog

class CNewProjectDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNewProjectDlg)

public:
	typedef struct
	{
		string rProjectNo;
		string rProjectName;
		string rProjectDesc;
		string rProjectHertz;
	}PROJECT_RECORD;

	CNewProjectDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewProjectDlg();

// Dialog Data
	enum { IDD = IDD_NEW_PROJECT };
	CString m_rProjectNo , m_rProjectName , m_rProjectDesc;
	CString m_rProjectHertz;

	CBoostBaseFilterEdit m_wndProjectNoEdit;
	CMFCButtonEx m_wndNewProjectButton;
	CMFCButtonEx m_wndDeleteProjectButton;
	CMFCButtonEx m_wndOKButton;
	CMFCButtonEx m_wndCancelButton;

	CMFCListCtrl m_wndProjectListCtrl;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonNewProject();
private:
	int LoadProjectRecordFromDatabase(void);
	int CreateProject(void);

	list<PROJECT_RECORD> m_ProjectRecordEntry;
	int DisplayProjectRecord(void);
public:
	afx_msg void OnBnClickedButtonDeleteProject();
        afx_msg void OnBnClickedOk();
private:
	int RemakeProjectSettingTable(const CString& rProjectNo);
public:
	afx_msg void OnLvnItemchangedListProject(NMHDR *pNMHDR, LRESULT *pResult);
	int CurProjectUpdate(void);
};
