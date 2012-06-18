#pragma once

#include "MFCListCtrlEx.h"
#include "LoadItem.h"
#include "MFCButtonEx.h"
#include <gui/ResizeWnd.h>
#include <gui/EasySize.h>
#include "WizardPropertyPage.h"

#ifdef _WIN32_WCE
#error "CDHtmlDialog is not supported for Windows CE."
#endif 

// CAssignSchematicTypeToLoadDlg dialog
class CAssignSchematicTypeToLoadDlg : public CWizardPropertyPage
{
	DECLARE_DYNCREATE(CAssignSchematicTypeToLoadDlg)

public:
	DECLARE_EASYSIZE;
	CAssignSchematicTypeToLoadDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAssignSchematicTypeToLoadDlg();
// Overrides

// Dialog Data
	enum { IDD = IDD_ASSIGN_SCHEMATIC_TYPE_TO_LOAD};
	
	CMFCTextColorListCtrl m_wndLoadListCtrl;
	CMFCTextColorListCtrl m_wndSchematicTypeListCtrl;
	CMFCTextColorListCtrl m_wndNotAssignedLoadListCtrl;
	
	CComboBox       m_wndBusCombo;
	CMFCButtonEx	m_wndBackButton;
	CMFCButtonEx	m_wndNextButton;
	CMFCButtonEx	m_wndClearButton;

	CEdit		m_wndTotalLoadCountEdit;

	CMFCButtonEx m_wndMoveToOriginalSchematicType;
	CMFCButtonEx m_wndOKButton , m_wndCancelButton;

	BOOL	m_bPdbIncludeCheck;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBegindragListRight(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnVertResize();
	afx_msg void OnBnClickedWizback();
	afx_msg void OnBnClickedWiznext();
	DECLARE_MESSAGE_MAP()

	int DetermineLayout(int cx, int cy);
private:
	int AddLoadItemToListCtrl(CLoadItem* pLoadItem);
	int AddLoadItemToBus(HTREEITEM hSelItem , vector<CLoadItem*>& LoadItemEntry);
	void DropItemOnList(CMFCTextColorListCtrl* pDragWnd , CMFCTextColorListCtrl* pDropWnd);

	map<CELoadItem*,string> m_LoadSchematicTypeMap;	//! LoadItem*와 해당 Schematic Type을 저장하는 컨테이너.
	HICON		m_hIcon;
	CMFCTextColorListCtrl*	m_pDragList;	//Which CWnd we are dragging FROM
	CMFCTextColorListCtrl*	m_pDropList;	//Which CWnd we are dropping ON
	CImageList*	m_pDragImage;	//For creating and managing the drag-image
	BOOL		m_bDragging;	//T during a drag operation
	int		m_nDragIndex;	//Index of selected item in the List we are dragging FROM
	int		m_nDropIndex;	//Index at which to drop item in the List we are dropping ON
	HTREEITEM	m_hDragItem;	//Pointer to window we are dropping on (will be cast to CListCtrl* type)
public:
	afx_msg void OnBnClickedOk();
private:
	int CreateImageList(void);
	int DisplaySchematicType();
	int DisplayLoadHasSchematicType(CMFCTextColorListCtrl& ListCtrl , const string& rSchematicType , const string& rInterestingBusID = _T("*"));
	void GetSelectedLoadItems(CMFCListCtrl& ListCtrl , vector<long>& LoadItemIndexEntry);
public:
	afx_msg void OnNMClickListSchematicType(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLvnBegindragListNotAssignedLoad(NMHDR *pNMHDR, LRESULT *pResult);
private:
	int OnBeginDrag(LPNMLISTVIEW pNMLV , CMFCTextColorListCtrl& ListCtrl);
	int MoveToOriginalSchematicType(void);
public:
	afx_msg void OnBnClickedButtonMoveToOriginalSchematicType();
	afx_msg void OnBnClickedCancel();
private:
	string GetSelectedSchematicTypeFromSchematicListCtrl(void);
public:
	BOOL Init(void);
	afx_msg void OnLvnColumnclickListNotAssignedLoad(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelendokComboBus();
	afx_msg void OnBnClickedPdbIncludeCheck();
	afx_msg void OnBnClickedSchematicClearButton();
	int SaveData(void);
	int LoadData(void);
private:
	int DisplayBusInComboBox(void);
};
