#pragma once

#include <gui/GradientStatic.h>
#include "MFCButtonEx.h"

// CMultiSortDlg dialog

class CMultiSortDlg : public CDialog
{
	DECLARE_DYNAMIC(CMultiSortDlg)

public:
	CMultiSortDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMultiSortDlg();

// Dialog Data
	enum { IDD = IDD_MULTI_SORT };
	CGradientStatic m_wndStaticTitle;

	CMFCButtonEx m_wndMultiSelectCond1,m_wndMultiSelectCond2,m_wndMultiSelectCond3;
	CMFCButtonEx m_wndMultiSortDir1,m_wndMultiSortDir2,m_wndMultiSortDir3;
	CMFCButtonEx m_wndMultiSelectShow;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonMultiSelectCond1();
	afx_msg void OnBnClickedButtonMultiSelectCond2();
	afx_msg void OnBnClickedButtonMultiSelectCond3();
	afx_msg void OnBnClickedButtonMultiSelectShow();
private:
	vector<int> m_MultiSortDirEntry;
	vector<int> m_MultiSortCellNoEntry;
public:
	int GetMultiSortInfoEntry(vector<int>& MultiSortCellNoEntry , vector<int>& MultiSortDirEntry);
	afx_msg void OnBnClickedButtonMultiSortDir1();
	afx_msg void OnBnClickedButtonMultiSortDir2();
	afx_msg void OnBnClickedButtonMultiSortDir3();
};
