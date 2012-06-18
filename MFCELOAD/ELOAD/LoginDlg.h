#pragma once
#include <gui/FilterEdit/BoostBaseFilterEdit.h>
#include <gui/GradientStatic.h>

// CLoginDlg dialog

class CLoginDlg : public CDialog
{
	DECLARE_DYNCREATE(CLoginDlg)

public:
	CLoginDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLoginDlg();
// Overrides

// Dialog Data
	enum { IDD = IDD_DIALOG_LOGIN };

	CGradientStatic		m_wndStaticTitle;
	CGradientStatic		m_wndProjectNoStatic;
	CComboBox		m_wndProjectNoCombo;
	CString			m_rProjectNo;
	CGradientStatic		m_wndUserIDStatic;
	CBoostBaseFilterEdit    m_wndUserID;
	CString			m_rID;
	CGradientStatic		m_wndPasswordStatic;
	CBoostBaseFilterEdit	m_wndPassword;
	CGradientStatic		m_wndStaticMsg;

	CMFCButton		m_wndSelectServerButton;
	CMFCButton		m_wndLoginButton;
	CMFCButton		m_wndCancelButton;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSelectServer();
private:
	CString m_rServerFolderPath;
	map<CString,ProjectInfo> m_ProjectInfoMap;
public:
	CString GetServerFolderPath() const { return m_rServerFolderPath; }
	afx_msg void OnBnClickedOk();
private:
	int LoadAppSettingFile(void);
public:
	afx_msg void OnEnChangeEditId();
private:
	int SaveAppSettingFile(void);
	int ReadProjectNoFromELoadMDB(void);
	int DisplayMessage(const string& rMsg , COLORREF bgColor , COLORREF fgColor);
	int SyncDatabaseStructure(void);
	int SyncToSqlFile(CADODB& adoDB , CSQLFile& sqlFile , vector<string>& FieldNameEntry);
public:
	CString GetProjectNameOf(const CString& rProjectNo);
	CString GetProjectDescOf(const CString& rProjectNo);
	CString GetProjectClientOf(const CString& rProjectNo);
};
