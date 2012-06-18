#pragma once


// CControlCableCreationSettingDlg dialog
#include "MFCButtonEx.h"
#include <gui/gridctrl/GridCtrl.h>
#include <gui/FilterEdit/BoostBaseFilterEdit.h>
#include <gui/GridCtrl/GridCellCombo.h>
#include "afxwin.h"
#include "Tables/ELOADTable.h"
#include "ControlCableGridCtrl.h"
#include "TreeListCtrlEx.h"
#include "afxcmn.h"
#include "WizardPropertyPage.h"
#include "ELOAD.h"

namespace ControlCableCreationSetting
{
class CGridCellControlCableCreationCombo : public CGridCellCombo
{
	DECLARE_DYNCREATE(CGridCellControlCableCreationCombo)
public:
	CGridCellControlCableCreationCombo();

	// editing cells
public:
	virtual BOOL  Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
};

class CGridCellControlTypeCombo : public CGridCellCombo
{
	DECLARE_DYNCREATE(CGridCellControlTypeCombo)
public:
	CGridCellControlTypeCombo();

	// editing cells
public:
	virtual BOOL  Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
	string m_rSchematicType;
};
};

class CControlCableCreationSettingDlg : public CWizardPropertyPage
{
	DECLARE_DYNAMIC(CControlCableCreationSettingDlg)

	typedef struct
	{
		string rSchemType;
		string rCableType , rCableCore , rCableSize;
		string rPrefix , rSuffix;
		string rDesc;
	}ControlCableRecord;
public:
	CControlCableCreationSettingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CControlCableCreationSettingDlg();

// Dialog Data
	enum { IDD = IDD_CONTROL_CALBLE_CREATION_SETTING_DIALOG };

protected:
	string m_rCurSchematicType;	//! 현재 선택한 Schematic Type
	int m_nCurControlCableCreation;	//! 현재 선택한 Control Cable Creation
	map<int,TABLES::CELOADTable::Record> m_ControlCableDataMap;
	map<int,string> m_ControlTypeMap;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:

	CMFCButtonEx	m_wndBackButton;
	CMFCButtonEx	m_wndNextButton;

	CMFCButtonEx	m_wndCheckButton;
	CMFCButtonEx	m_wndSettingSaveButton;
	CMFCButtonEx	m_wndCancelButton;

	CMFCButtonEx	m_wndCableAddButton;
	CMFCButtonEx	m_wndExportButton;

	CTreeListCtrlEx		m_wndSchematicTreeListCtrl;
	CListCtrl		m_wndLcsLocalTypeListCtrl;
	CListCtrl		m_wndLcpIoTypeListCtrl;
	CListCtrl		m_wndDcsIoTypeListCtrl;
	
	HICON		m_hIcon;
	CImageList	m_imgList;
	CListCtrl*	m_pDragList;	//Which CWnd we are dragging FROM
	CTreeListCtrl*	m_pDropList;	//Which CWnd we are dropping ON

	CImageList*	m_pDragImage;	//For creating and managing the drag-image
	BOOL		m_bDragging;	//T during a drag operation
	int		m_nDragIndex;	//Index of selected item in the List we are dragging FROM
	int		m_nDropIndex;	//Index at which to drop item in the List we are dropping ON
	HTREEITEM	m_hDragItem;	//Pointer to window we are dropping on (will be cast to CListCtrl* type)
	HTREEITEM	m_hDropItem;	//Pointer to window we are dropping on (will be cast to CListCtrl* type)
	
	CComboBox	m_wndControlCableSpecCombo;

	CEdit		m_wndCablePrefixEdit;
	CEdit		m_wndCableSuffixEdit;
	///CEdit		m_wndCopySchemTypeNameEdit;

	int ShowSelectSubItem(CGridCtrl& GridCtrl);
	HTREEITEM InsertItemIntoSchematicListCtrl(const CString& rType= "", const CString& rDesc= "");
	int InsertItemIntoControlTypeListCtrl(const CString& rType = "", const CString& rIoType = "", const CString& rDesc ="", const CString& rIoTag = "");
	//int InsertItemInCableDataListCtrl(const CString& rType ="", const CString& rCore= "" , const CString& rSize= "" , const CString& rPrefix= "",const CString& rSuffix= "", const CString& rLoadId = "\"LOAD ID\"");
	HTREEITEM AddControlItemToSchemType(TABLES::CELOADTable::Record& record);
	
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	int DisplaySchematicTypeData(void);
	int SaveData(void);
	int SaveMainCableData(void);
private:
	int ShowSelectControlTypeItem(const int& nRow);
	//int DisplayControlCableData(const string& rSchematicType);
	int DisplayControlTypeData(const string& rSchematicType, TABLES::CELOADTable::Record& ControlCableRecord);
	int ClearControlCableData();
	int ClearControlTypeData();
private:
	TABLES::CELOADTable *m_pSchematicControlCableMapTableCopy /*, *m_pControlCableCreationSettingTableCopy */, *m_pControlCableControlTypeMapTableCopy, *m_pSchematicTypeCopy, *m_pSchematicControlTypeCopy;

	int CheckControlCableControlTypeMap(void);
	int OnChangeSchematicType(void);
	int OnChangeControlCableData(void);
public:
	int CreateImageList(void);
	int CreateControlTypeList(void);
	int CreateCableTypeList(void);
	int CreateSchematicTreeList();

	int FillComboItem(void);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//afx_msg void OnLvnBegindragCableDataList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSettingSave();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedCabledataAddButton();
	//afx_msg void OnBnClickedCabledataDeleteButton();
	afx_msg void OnBnClickedWizback();
	afx_msg void OnBnClickedWiznext();

	int OnBeginDrag(LPNMLISTVIEW pNMLV, CListCtrl& ListCtrl);
	void DropItemOnList(CListCtrl* pDragList , CTreeListCtrl* pDropList);
	BOOL CompareMatchType(const CString& rSchematicType, const CString& rCableType, const CString& rCableCore, const CString& rCableSize, const CString& rPrefix, const CString& rSuffix);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLvnBegindragControlTypeList(NMHDR *pNMHDR, LRESULT *pResult);
private:
	int DisplayControlTypeListOfSchematicType(const string& rText);
	int DisplayControlTypeTreeNodeOf(HTREEITEM hItem , const string& rSchemType , 
								  const string& rCableType , const string& rCableCore ,const string& rCableSize, 
								  const string& rCablePrefix , const string& rCableSuffix, const string& rDesc);
	int InsertControlTypeTreeNod(HTREEITEM hItem , const string& rControlType);
public:
	BOOL Init(void);
	afx_msg void OnBnClickedButtonDelete();
	BOOL CompareMatchCableType(TABLES::CELOADTable::Record record);
private:
	int GetControlCableRecordFrom(ControlCableRecord& record , HTREEITEM hItem);
	int RemoveControlCableControlTypeRecord(const string& rSchemType, const ControlCableRecord& record, const string& rControlType);
	int RemoveControlCableRecord(const string& rSchemType, const ControlCableRecord& record);
public:
	int IsExistItemText(const CString& rItemText);
	//afx_msg void OnBnClickedControlcableModifyButton();
	afx_msg void OnItemCopy();
	afx_msg void OnItemDelete();
	afx_msg void OnSchematicmenuDelete();
	afx_msg void OnSchematicmenuModify();
	afx_msg void OnControltypemenuDelete();
	
	afx_msg void OnLvnBegindragControlTypeList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnBegindragControlTypeList3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonCheck();
	int RemoveSchematicTypeRecord(const string& rSchematicType);
	bool m_bDeleted;
	
	afx_msg void OnBnClickedButtonExport();
	int WriteControlCableSettingToTxtFile(void);
};
