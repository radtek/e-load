#pragma once
#include "Fpspread1.h"
#include "ELOADPropSheet.h"
#include "MFCButtonEx.h"

#include <list>
using namespace std;
// CConduitTableDlg dialog

class CConduitTableDlg : public CDialog
{
	DECLARE_DYNAMIC(CConduitTableDlg)
public:
	CConduitTableDlg();
	virtual ~CConduitTableDlg();

// Dialog Data
	enum { IDD = IDD_CONDUIT_TABLE_PAGE };
	CListCtrl m_wndConduitTableListCtrl;
	CString m_rTableDesc;
	CFpspread1 m_wndCONDSpread;
	CMFCButtonEx m_wndAddButton , m_wndDelButton;
	CMFCButtonEx m_wndCancelButton;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	int ImportConduitTableFromExcelFile(const CString& rFilePath);
public:
	afx_msg void OnBnClickedButtonAddConduitTable();
	virtual BOOL OnInitDialog();
	int LoadData(void);
private:
	int DisplayData(void);
	int DisplayConduitData(const string& rTableName);
public:
	afx_msg void OnLvnItemchangedListConduitTable(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonDeleteConduitTable();
	afx_msg void OnNMClickListConduitTable(NMHDR *pNMHDR, LRESULT *pResult);
};
