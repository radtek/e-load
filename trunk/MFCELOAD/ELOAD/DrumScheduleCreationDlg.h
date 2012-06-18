#pragma once
#include "TreeListCtrlEx.h"
#include "MFCButtonEx.h"
#include "afxwin.h"
#include "ListACtrl.h"
#include "ListCtrlEx.h"
#include "DrumItem.h"
// CDrumScheduleCreationDlg dialog

class CDrumScheduleCreationDlg : public CDialog
{
	DECLARE_DYNAMIC(CDrumScheduleCreationDlg)

	typedef struct tagCABLE_ITEM_STRUCTURE
	{
		string rCableVolt;
		string rSpec;
		string rCore;
		string rSize;
		string rCategory;
		string rVoltType;

		bool operator ==(const struct tagCABLE_ITEM_STRUCTURE& rhs)
		{
			return ((rhs.rCableVolt == rCableVolt) && 
			(rhs.rSpec == rSpec) && 
			(rhs.rCore == rCore) &&
			(rhs.rSize == rSize));
		}
	}CABLE_ITEM_STRUCTURE;

	typedef struct
	{
		CString rCableVolt;
		CString rCableSpec;
		CString rCableCore;
		bool bIsDeleted;
		CString rLoadID;				// Cable의 Load ID
		vector<CCableItem*> m_CableItemEntry;		// LOAD ID를 키값으로 갖는 Cable Entry
		double nTotalCableLength;
		double nExtraCableLength;
		
	}LOAD_CABLE_RECORD;
	
public:
	CDrumScheduleCreationDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDrumScheduleCreationDlg();

// Dialog Data
	enum { IDD = IDD_DRUM_SCHEDULE_CREATION };

	CTreeListCtrlEx	m_wndDrumTreeListCtrl;
	CListCtrlExtand	m_wndCableSpecListCtrl;

	BOOL m_bAppendDrumSchCreation;
	CMFCButtonEx	m_wndAddButton;
	CMFCButtonEx	m_wndCreationButton , m_wndCancelButton;
	CMFCButtonEx	m_wndSaveButton;
	CMFCButtonEx	m_wndRefreshButton;

	CFont		m_font;
	CComboBox	m_wndComBo1,m_wndComBo2;
	CEdit		m_wndEdit;
	CEdit		m_wndMinSpareLenEdit;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:	
	HICON				m_hIcon;
	///CImageList			m_imgList;
	CListCtrl*			m_pDragList;	//Which CWnd we are dragging FROM
	CTreeListCtrl*			m_pDropList;	//Which CWnd we are dropping ON
	CImageList*			m_pDragImage;	//For creating and managing the drag-image
	BOOL				m_bDragging;	//T during a drag operation
	int				m_nDragIndex;	//Index of selected item in the List we are dragging FROM
	int				m_nDropIndex;	//Index at which to drop item in the List we are dropping ON
	HTREEITEM			m_hDragItem;	//Pointer to window we are dropping on (will be cast to CListCtrl* type)
	HTREEITEM			m_hDropItem;	//Pointer to window we are dropping on (will be cast to CListCtrl* type)

private:
	int DisplayCableSpec(bool bLoadDB = true);
	int InitSchematicTreeList();
	int InitCableSpecList(void);
	static BOOL InitEditor(CWnd** pWnd, int nRow, int nColumn, CString& strSubItemText, DWORD_PTR dwItemData, void* pThis, BOOL bUpdate);
	static BOOL EndEditor(CWnd** pWnd, int nRow, int nColumn, CString& strSubItemText, DWORD_PTR dwItemData, void* pThis, BOOL bUpdate);
public:
	afx_msg void OnBnClickedStdCableLengthButton();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLvnBegindragCableSpecList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSaveButton();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCableDataDelete();

	bool IsExistCableLength(HTREEITEM &hItem, const CString& rCableLength);
	int OnBeginDrag(LPNMLISTVIEW pNMLV , CListCtrl& ListCtrl);
	void DropItemOnList(CListCtrl* pDragList , CTreeListCtrl* pDropList);
	int DisplaySchematicTreeListCtrl(void);
	BOOL CompareMatchCable(const CABLE_ITEM_STRUCTURE& record, vector<CABLE_ITEM_STRUCTURE> &CableItemEntry);
	afx_msg void OnBnClickedCreationButton();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	int DrumScheduleCreation(void);

	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	bool ComparePrefix(const string& rPrefix);
	afx_msg void OnBnClickedRefreshButton();
	bool IsTotalCableEntryEmpty(const vector<LOAD_CABLE_RECORD>& TotalCableEntry);
	bool IsFoundCableForDrumSchedule(CDrumItem* pDrumItem, vector<LOAD_CABLE_RECORD>& TotalCableEntry, const double& nDrumSize, double& nTotalLength, const CString& rMinSpare);
	CDrumItem* DrumScheduleOfConnectCable(CELOADTable::Record& record, LOAD_CABLE_RECORD& LoadCablRecord, int& nDrumCount, double& nTotalLength, const CString& rMinSpare, const double& nDrumSize);
	CDrumItem* DrumScheduleOfGroupCable(CDrumItem * pDrumItem, CELOADTable::Record& record, LOAD_CABLE_RECORD& LoadCablRecord, int& nDrumCount, double& nTotalLength, const CString& rMinSpare, const double& nDrumSize);
	int LoadData(void);

};
