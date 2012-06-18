#pragma once

#include <gui/EasySize.h>
#include <gui/TreeCtrlEx.h>
#include "LoadItem.h"
#include "ELoadPropList.h"

// CShowCableDlg dialog

class CShowCableDlg : public CDialog
{
	DECLARE_DYNAMIC(CShowCableDlg)

public:
	DECLARE_EASYSIZE;
	CShowCableDlg(ITEMS::CLoadItem* pLoadItem , CWnd* pParent = NULL);   // standard constructor
	virtual ~CShowCableDlg();

// Dialog Data
	enum { IDD = IDD_SHOW_CABLE };

	CTreeCtrlEx    m_wndCableTreeCtrl;
	CMFCListCtrl   m_wndCalculatedResultList;
	CELoadPropList m_wndCablePropList;
	CMFCButton m_wndApplyButton , m_wndCancel;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg LRESULT OnPropertyChanged(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()
private:
	ITEMS::CLoadItem*  m_pLoadItem;
	ITEMS::CCableItem* m_pCableItem;
	///CImageList m_imgList;
	map<string,HTREEITEM> m_CableCategoryItemMap;
	HICON m_hIcon;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
private:
	int DisplayData(void);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTvnSelchangedTreeCable(NMHDR *pNMHDR, LRESULT *pResult);
private:
	int DisplayCalculatedResultList(CCableItem* pCableItem);
	CELoadItemProp* m_pCloneProp;
public:
	afx_msg void OnBnClickedButtonRestModifiedValues();
	afx_msg void OnBnClickedButtonCommitModifiedValues();
	afx_msg void OnBnClickedCancel();
};
