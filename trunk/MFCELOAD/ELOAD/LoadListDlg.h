#pragma once

#include "GeneratorItem.h"

// CLoadListDlg dialog

class CLoadListDlg : public CDialog
{
	DECLARE_DYNAMIC(CLoadListDlg)

public:
	CLoadListDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLoadListDlg();

// Dialog Data
	enum { IDD = IDD_LOAD_LIST };
	CMFCListCtrl m_wndLoadListCtrl;

	ITEMS::CGeneratorItem* m_pGeneratorItem;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int DisplayLoadList(const string& rBusID , const bool& bDisplayOnlySelectedLoad = true);
	virtual BOOL OnInitDialog();
	afx_msg void OnNMClickListLoad(NMHDR *pNMHDR, LRESULT *pResult);
};
