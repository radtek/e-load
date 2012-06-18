#pragma once
#include "afxcmn.h"
#include <list>
#include <vector>

#include "ELoadPropSheet.h"
#include "fpspread1.h"
#include "MFCButtonEx.h"
#include <gui/FilterEdit/BoostBaseFilterEdit.h>
#include "afxcoll.h"
#include "afxwin.h"

using namespace std;

// CLoadSummaryPropPage dialog

class CLoadSummaryPropPage : public CDialog
{
	DECLARE_DYNAMIC(CLoadSummaryPropPage)

public:
        typedef struct
	{
		string rTableNo;
		string rTableDesc;
	}TABLE_RECORD;

	CLoadSummaryPropPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLoadSummaryPropPage();

// Dialog Data
	enum { IDD = IDD_LOAD_SUMMARY_SETTING };
	CMFCButtonEx m_wndAddButton , m_wndDelButton;
	CMFCButtonEx m_wndSaveButton , m_wndCancelButton;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonAddTable();
	afx_msg void OnBnClickedButtonDeleteTable();
public:
	CString m_rSelectExcelFileName;
	CString m_rTableDesc;
	CString m_rSelTable;
public:
	int CreateDbTableOfExcelFileName(const CString& rTableName, const CString& rFieldNameSet, CADODB* pAdoDb); 
	virtual BOOL OnInitDialog();
	CListCtrl m_wndLoadSummaryTableListCtrl;
private:
	CBoostBaseFilterEdit m_wndFilterEdit[15];

	list<TABLE_RECORD> m_TableRecordEntry;
public:
	int LoadData(void);
	int DisplayProjectRecord(void);
	int LoadDataFromExcel(CString& rTableName);
private:
	CString m_rSelExcelFilePath;
public:
	afx_msg void OnNMClickListTableForLoadSummary(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDBClickListTableForLoadSummary(NMHDR *pNMHDR, LRESULT *pResult);

	int SaveData(void);
	afx_msg void OnLvnKeydownListTableForLoadSummary(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSave();

};
