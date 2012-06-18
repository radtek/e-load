#pragma once


#include <gui/ResizeWnd.h>
#include <gui/EasySize.h>

#include <map>
using namespace std;
// CVirtualLoadSummaryDlg dialog

class CVirtualLoadSummaryDlg : public CDialog
{
	DECLARE_DYNAMIC(CVirtualLoadSummaryDlg)

public:
	DECLARE_EASYSIZE;
	CVirtualLoadSummaryDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVirtualLoadSummaryDlg();

// Dialog Data
	enum { IDD = IDD_VIRTUAL_LOAD_SUMMARY };
	CResizeWnd m_wndResize;
	CMFCTextColorListCtrl m_wndBusListCtrl;
	CMFCTextColorListCtrl m_wndLoadListCtrl;
	CMFCTextColorListCtrl m_wndTransformerListCtrl;
	CMFCButton m_wndMoveToOriginalBus;
	CMFCButton m_wndOKButton , m_wndCancelButton;

	CFont m_NewHeaderFont;
protected:
	HICON m_hIcon;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnVertResize();
	DECLARE_MESSAGE_MAP()

	int		m_nCurBusSel;
	CListCtrl*	m_pDragList;	//Which ListCtrl we are dragging FROM
	CListCtrl*	m_pDropList;	//Which ListCtrl we are dropping ON
	CImageList*	m_pDragImage;	//For creating and managing the drag-image
	BOOL		m_bDragging;	//T during a drag operation
	int		m_nDragIndex;	//Index of selected item in the List we are dragging FROM
	int		m_nDropIndex;	//Index at which to drop item in the List we are dropping ON
	CWnd*		m_pDropWnd;	//Pointer to window we are dropping on (will be cast to CListCtrl* type)

	CImageList	m_Images;
public:
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMClickListBus(NMHDR *pNMHDR, LRESULT *pResult);
private:
	map<CELoadItem*,string> m_LoadBusIDMap;	//! LoadItem*와 해당 BUS 이름을 저장하는 컨테이너.
	int DisplayLoadInBus(const string& rBusId);
	void DropItemOnList(CListCtrl *pDragList, CListCtrl *pDropList);
	void GetSelectedLineDataRecord(vector<long>& LoadItemIndexEntry);
public:
	afx_msg void OnLvnBegindragListLoad(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
private:
	int DisplayBusItem(const bool& bUpdate = false);
	int DisplayTransformerItem(const bool& bUpdate = false);
public:
	afx_msg void OnBnClickedButtonMoveToOriginalBus();
	int MoveToOriginalBus(void);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
private:
	int DetermineLayout(int cx, int cy);
	static int CX , CY;
};
