#pragma once

#include "MFCButtonEx.h"

// CCompareRevisionDataDlg dialog

class CCompareRevisionDataDlg : public CDialog
{
	DECLARE_DYNAMIC(CCompareRevisionDataDlg)

public:
	CCompareRevisionDataDlg(const CString& rProductName , CWnd* pParent = NULL);   // standard constructor
	virtual ~CCompareRevisionDataDlg();

// Dialog Data
	enum { IDD = IDD_VIEW_REVISION };
	
	CString		m_rProductName;	//! 현재 작업 중인 PRODUCT NAME
	UINT m_nLeftSideRevNo , m_nRightSideRevNo;

	CMFCListCtrl m_wndRevisionListCtrl;
	CMFCListCtrl m_wndLeftProductListCtrl , m_wndRightProductListCtrl;
	CMFCButtonEx m_wndOKButton , m_wndCancelButton;
protected:
	HICON		m_hIcon;
	CImageList	m_imgList;
	CListCtrl*	m_pDragList;	//Which CWnd we are dragging FROM
	CListCtrl*	m_pDropList;	//Which CWnd we are dropping ON

	CImageList*	m_pDragImage;	//For creating and managing the drag-image
	BOOL		m_bDragging;	//T during a drag operation
	int		m_nDragIndex;	//Index of selected item in the List we are dragging FROM
	int		m_nDropIndex;	//Index at which to drop item in the List we are dropping ON
	HTREEITEM	m_hDragItem;	//Pointer to window we are dropping on (will be cast to CListCtrl* type)
	HTREEITEM	m_hDropItem;	//Pointer to window we are dropping on (will be cast to CListCtrl* type)

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	int CreateRevisionListCtrl(void);
	int DisplayRevisionData(void);
	int OnBeginDrag(LPNMLISTVIEW pNMLV, CListCtrl& ListCtrl);
	void DropItemOnList(CListCtrl* pDragList , CListCtrl* pDropList);
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnLvnBegindragListRevision(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
//	afx_msg void OnLvnItemchangedListLeftProduct(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnLvnItemchangedListRightProduct(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListLeftProduct(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListRightProduct(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
};
