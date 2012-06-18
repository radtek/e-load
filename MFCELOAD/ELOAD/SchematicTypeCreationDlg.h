#pragma once
#include "MFCButtonEx.h"
#include <gui/gridctrl/GridCtrl.h>
#include "afxwin.h"
#include "ControlCableGridCtrl.h"
#include "Tables/ELOADTable.h"
#include <gui/GridCtrl/GridCellCombo.h>
#include "TreeListCtrlEx.h"
#include "afxcmn.h"
#include "WizardPropertyPage.h"

// CSchematicTypeCreationDlg dialog
namespace SchematicTypeCreation
{
class CGridCellControlTypeCombo : public CGridCellCombo
{
	DECLARE_DYNCREATE(CGridCellControlTypeCombo)
public:
	CGridCellControlTypeCombo();

	// editing cells
public:
	virtual BOOL  Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
};
};

class CSchematicTypeCreationDlg : public CWizardPropertyPage
{
	DECLARE_DYNAMIC(CSchematicTypeCreationDlg)
	
	typedef struct
	{
		string rType , rDesc;
	}Schem;
public:
	CSchematicTypeCreationDlg(const string& rSchematicType = _T("") , CWnd* pParent = NULL);   // standard constructor
	virtual ~CSchematicTypeCreationDlg();

// Dialog Data
	enum { IDD = IDD_SCHEMATIC_TYPE_CREATION_DIALOG };

protected:
	string m_rCurSchematicType;	//! 현재 설정된 Schematic Type
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CMFCButtonEx	m_wndBackButton;
	CMFCButtonEx	m_wndNextButton;

	CMFCButtonEx	m_wndSettingSaveButton;
	CMFCButtonEx	m_wndCancelButton;

	CMFCButtonEx	m_wndSchematicAddButton;
	CMFCButtonEx	m_wndSchematicDeleteButton , m_wndSchematicModifyButton;

	CTreeListCtrlEx		m_wndSchematicTreeListCtrl;
	CListCtrl		m_wndLcsLocalTypeListCtrl;
	CListCtrl		m_wndLcpIoTypeListCtrl;
	CListCtrl		m_wndDcsIoTypeListCtrl;

	CEdit m_wndSchematicTypeEdit;
	CEdit m_wndSchematicDescEdit;
	
	HICON		m_hIcon;
	///CImageList	m_imgList;
	CListCtrl*	m_pDragList;	//Which CWnd we are dragging FROM
	CTreeListCtrl*	m_pDropList;	//Which CWnd we are dropping ON

	CImageList*	m_pDragImage;	//For creating and managing the drag-image
	BOOL		m_bDragging;	//T during a drag operation
	int		m_nDragIndex;	//Index of selected item in the List we are dragging FROM
	int		m_nDropIndex;	//Index at which to drop item in the List we are dropping ON
	HTREEITEM	m_hDragItem;	//Pointer to window we are dropping on (will be cast to CListCtrl* type)
	HTREEITEM	m_hDropItem;	//Pointer to window we are dropping on (will be cast to CListCtrl* type)

protected:
	TABLES::CELOADTable *m_pSchematicTypeTableCopy , *m_pSchematicControlTypeMapTableCopy;
	TABLES::CELOADTable *m_pControlCableTypeMapTableCopy , *m_pSchemControlCableMapTableCopy;

	list<Schem> m_SchemEntry;
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	map<int,string> m_SchematicTypeMap;
	map<int,string> m_ControlTypeMap;
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnBnClickedSettingSave();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedSchematicAddButton();
	afx_msg void OnItemModify();
	afx_msg void OnItemDelete();
	afx_msg void OnBnClickedWizback();
	afx_msg void OnBnClickedWiznext();
	
private:
	string m_rInitSchematicType;
	int OnChangeSchematicType(void);
public:
	int DisplayControlTypeData(void);
	int InsertItemIntoControlTypeListCtrl(const string& rType ="", const string& rIoType="", const string& rDesc="", const string& rIoTag="", const string& rCategory = "");
	int InitSchematicTreeList(void);
	HTREEITEM MakeRootItemInTreeList(const CString& rType, const CString& rDesc);
	int InitControlTypeList(void);
	int ShowSelectControlTypeItem(const int& nRow);
	int DisplaySchematicTypeData(void);
	int SaveData(void);

	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLvnBegindragControlTypeList(NMHDR *pNMHDR, LRESULT *pResult);
	int OnBeginDrag(LPNMLISTVIEW pNMLV, CListCtrl& ListCtrl);
	void DropItemOnList(CListCtrl* pDragList , CTreeListCtrl* pDropList);
	BOOL CompareMatchType(const CString& rSchematicType, const CString& rControlType);
	int CreateImageList(void);
	BOOL Init(void);
	BOOL IsExistItemText(const CString& rItemText);
private:
	int ChangeSchemTypeInTable(const string& rNewSchemType , const string& rNewSchemDesc , const string& rOldSchemType);
public:
	int ModifySchematicTypeInLoad(const CString& rOldSchematicType, const CString& rNewSchematicType);
	afx_msg void OnControltypemenuDelete();
	afx_msg void OnLvnBegindragControlTypeList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnBegindragControlTypeList3(NMHDR *pNMHDR, LRESULT *pResult);
	bool m_bDeleted;
};
