#pragma once

#include "MFCListCtrlEx.h"
#include "LoadItem.h"
#include "MFCButtonEx.h"
#include <gui/ResizeWnd.h>
#include <gui/EasySize.h>

#ifdef _WIN32_WCE
#error "CDHtmlDialog is not supported for Windows CE."
#endif 

class CSortMFCListCtrl : public CMFCListCtrl
{
public:
	CSortMFCListCtrl() : CMFCListCtrl(){}
	virtual ~CSortMFCListCtrl(){}

	int OnCompareItems (LPARAM lParam1, LPARAM lParam2, int iColumn)
	{
		CELoadItem* pItem1 = (CELoadItem*)lParam1;
		CELoadItem* pItem2 = (CELoadItem*)lParam2;

		if((0 == iColumn) && pItem1 && pItem2)
		{
			CString rString1 = pItem1->GetName().c_str();
			rString1 = rString1.MakeUpper();
			CString rString2 = pItem2->GetName().c_str();
			rString2 = rString2.MakeUpper();

			return strcmp(rString1.operator LPCSTR() , rString2.operator LPCSTR());
		}

		return 0;

	}
};

// CAssignLoadToBusDlg dialog
class CAssignLoadToBusDlg : public CDialog
{
	DECLARE_DYNCREATE(CAssignLoadToBusDlg)

public:
	DECLARE_EASYSIZE;
	CAssignLoadToBusDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAssignLoadToBusDlg();
// Overrides

// Dialog Data
	enum { IDD = IDD_ASSIGN_LOAD_TO_BUS};
	
	CResizeWnd m_wndResize;
	CMFCTextColorListCtrl m_wndBusListCtrl;
	CMFCTextColorListCtrl m_wndLoadListCtrl;
	CMFCTextColorListCtrl m_wndNotAssignedLoadListCtrl;
	CComboBox m_wndAreaCombo;
	CMFCButtonEx m_wndMoveToOriginalBus;
	CMFCButtonEx m_wndOKButton , m_wndCancelButton;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBegindragListRight(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnVertResize();
	DECLARE_MESSAGE_MAP()

	int DetermineLayout(int cx, int cy);
private:
	int AddLoadItemToListCtrl(CLoadItem* pLoadItem);
	int AddLoadItemToBus(HTREEITEM hSelItem , vector<CLoadItem*>& LoadItemEntry);
	void DropItemOnList(CMFCTextColorListCtrl* pDragWnd , CMFCTextColorListCtrl* pDropWnd);

	typedef struct
	{
		string rBusID , rLoadVolt , rPhase;
	}BusInfo;

	map<CELoadItem*,BusInfo> m_LoadBusInfoMap;	//! LoadItem*와 해당 BUS 정보를 저장하는 컨테이너.
	HICON		m_hIcon;
	///CImageList	m_imgList;
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
	int DisplayBusItem(const bool& bUpdate);
	int DisplayLoadInBus(CMFCTextColorListCtrl& ListCtrl , const string& rBusID , const string& rInterestingArea = _T("*"));
	void GetSelectedLineDataRecord(CMFCListCtrl& ListCtrl , vector<long>& LoadItemIndexEntry);
public:
	afx_msg void OnNMClickListBus(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLvnBegindragListNotAssignedLoad(NMHDR *pNMHDR, LRESULT *pResult);
private:
	int OnBeginDrag(LPNMLISTVIEW pNMLV , CMFCTextColorListCtrl& ListCtrl);
	int MoveToOriginalBus(void);
public:
	afx_msg void OnBnClickedButtonMoveToOriginalBus();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnCbnSelendokComboArea();
};
