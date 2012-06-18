#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "MFCButtonEx.h"

// CCableRouteAssignTOLoadDlg dialog

class CCableRouteAssignTOLoadDlg : public CDialog
{
	DECLARE_DYNAMIC(CCableRouteAssignTOLoadDlg)

public:
	CCableRouteAssignTOLoadDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCableRouteAssignTOLoadDlg();

// Dialog Data
	enum { IDD = IDD_CABLE_ROUTE_ASSIGN_TO_LOAD_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	map<CELoadItem*,string>		m_LoadRouteGroupMap;	//! LoadItem*와 해당 Route Group을 저장하는 컨테이너.
public:
	CMFCTextColorListCtrl		m_wndRouteGrListCtrl;
	CMFCTextColorListCtrl		m_wndAssignLoadListCtrl;
	CMFCTextColorListCtrl		m_wndNotAssignLoadListCtrl;
	
	CMFCButtonEx m_wndOkButton;
	CMFCButtonEx m_wndCancelButton;

	HICON				m_hIcon;
	///CImageList			m_imgList;
	CMFCTextColorListCtrl*		m_pDragList;	//Which CWnd we are dragging FROM
	CMFCTextColorListCtrl*		m_pDropList;	//Which CWnd we are dropping ON
	CImageList*			m_pDragImage;	//For creating and managing the drag-image
	BOOL				m_bDragging;	//T during a drag operation
	int				m_nDragIndex;	//Index of selected item in the List we are dragging FROM
	int				m_nDropIndex;	//Index at which to drop item in the List we are dropping ON
	HTREEITEM			m_hDragItem;	//Pointer to window we are dropping on (will be cast to CListCtrl* type)

	CComboBox	m_wndLoadAreaCombo;
	CComboBox	m_wndSortByBusCombo;
public:
	afx_msg void OnCbnSelchangeLoadAreaCombo();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLvnBegindragNotAssignList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnBegindragAssignLoadList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickRouteGroupList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	virtual BOOL OnInitDialog();

	int InitRouteGroupListCtrl(void);
	int InitAssignListCtrl(void);
	int InitNotAssignListCtrl(void);
	int DisplayRouteGroupData(void);
	int DisplayLoadHasRouteGroup(CMFCTextColorListCtrl& ListCtrl , const string& rRouteGroup, const string& rArea, const string rBusId);

	int InsertItemInRouteGroupListCtrl(const CString& rRouteGroup, const CString& rDesc, const CString& rDesignLength);

	BOOL IsExistRouteGroupItem(const CString& rRouteGroup);
	int CreateImageList(void);

	int OnBeginDrag(LPNMLISTVIEW pNMLV , CMFCTextColorListCtrl& ListCtrl);
	void DropItemOnList(CMFCTextColorListCtrl* pDragList , CMFCTextColorListCtrl* pDropList);
	void GetSelectedLoadItems(CMFCListCtrl& ListCtrl , vector<long>& LoadItemIndexEntry);
	string GetSelectedRouteGroupFromRouteGrListCtrl(void);
	int MoveToOriginalRouteGroup(void);
	afx_msg void OnCbnSelchangeLoadBusCombo();
};
