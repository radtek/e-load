// ControlCableCreationSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ControlCableCreationSettingDlg.h"
#include "ELoadDocData.h"
#include <util/ado/ADODB.h>
#include <gui/GridCtrl/GridCellCombo.h>
#include "FillGridCellComboBox.h"
#include "GridCellEx.h"
#include "SchematicTypeCreationDlg.h"

#include "Tables/ControlTypeTable.h"
#include "Tables/SchematicControlTypeMapTable.h"
#include "ControlCableTypeInfoDlg.h"
#include "SchematicTypeCopyDlg.h"
#include "LoadItemProp.h"
#include "SchematicTypeCheckDlg.h"
#include "SimpleAnimationDlg.h"
#include "pyfromc.h"

struct UsedControlType : public binary_function<CELOADTable::Record,pair<string,string>,bool>
{
	bool operator()(const CELOADTable::Record& record , const pair<string,string>& rSchemControlTypePair) const
	{
		int nMatchCount = 0;
		for(map<string,string>::const_iterator itr = record.FieldValueMap.begin() ; itr != record.FieldValueMap.end();++itr)
		{
			if((_T("C_SCHEM_TYPE") == itr->first) && (rSchemControlTypePair.first == itr->second))
				++nMatchCount; 
			else if((_T("C_CONTROL_TYPE") == itr->first) && (rSchemControlTypePair.second == itr->second))
				++nMatchCount;
		}

		return (2 == nMatchCount);
	}
};

//bool MatchControlCableData(CELOADTable::Record lhs , CELOADTable::Record rhs)
//{
//	const string rSchemType1  = lhs.FieldValueMap[_T("C_SCHEM_TYPE")];
//	const string rSpec1  = lhs.FieldValueMap[_T("C_SPEC")];
//	const string rCore1  = lhs.FieldValueMap[_T("C_CORE")];
//	const string rSize1  = lhs.FieldValueMap[_T("C_SIZE")];
//	const string rOD1    = lhs.FieldValueMap[_T("C_OD")];
//	const string rWeight1= lhs.FieldValueMap[_T("C_WEIGHT")];
//	const string rPrefix1= lhs.FieldValueMap[_T("C_NAMING_RULE_PREFIX")];
//	const string rSuffix1= lhs.FieldValueMap[_T("C_NAMING_RULE_SUFFIX")];
//
//	const string rSchemType2  = rhs.FieldValueMap[_T("C_SCHEM_TYPE")];
//	const string rSpec2  = rhs.FieldValueMap[_T("C_SPEC")];
//	const string rCore2  = rhs.FieldValueMap[_T("C_CORE")];
//	const string rSize2  = rhs.FieldValueMap[_T("C_SIZE")];
//	const string rOD2    = rhs.FieldValueMap[_T("C_OD")];
//	const string rWeight2= rhs.FieldValueMap[_T("C_WEIGHT")];
//	const string rPrefix2= rhs.FieldValueMap[_T("C_NAMING_RULE_PREFIX")];
//	const string rSuffix2= rhs.FieldValueMap[_T("C_NAMING_RULE_SUFFIX")];
//
//	return ((rSchemType1 == rSchemType2) && (rSpec1 == rSpec2) && (rCore1 == rCore2) && (rSize1 == rSize2) && (rOD1 == rOD2) && 
//		(rWeight1 == rWeight2) && (rPrefix1 == rPrefix2) && (rSuffix1 == rSuffix2));
//}
/*
struct MatchType : public binary_function<CELOADTable::Record, string, bool>
{
        bool operator()(const CELOADTable::Record lhs , const string& rhs) const
	{
		const map<string,string>::const_iterator where = lhs.FieldValueMap.find(_T("C_TYPE"));
		if(where != lhs.FieldValueMap.end())
		{
			return (rhs == where->second);
		}

		return false;
	}
};
*/
struct MatchSchemType : public binary_function<CELOADTable::Record, string, bool>
{
        bool operator()(const CELOADTable::Record lhs , const string& rhs) const
	{
		const map<string,string>::const_iterator where = lhs.FieldValueMap.find(_T("C_SCHEM_TYPE"));
		if(where != lhs.FieldValueMap.end())
		{
			return (rhs == where->second);
		}

		return false;
	}
};


using namespace ControlCableCreationSetting;
/////////////////////////////////////////////////////////////////////////////
// CGridCellComboEx 
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CGridCellControlCableCreationCombo, CGridCellCombo)

CGridCellControlCableCreationCombo::CGridCellControlCableCreationCombo() : CGridCellCombo()
{
}

// Create a control to do the editing
BOOL CGridCellControlCableCreationCombo::Edit(int nRow, int nCol, CRect rect, CPoint point , UINT nID, UINT nChar)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pControlTable = docData.GetTableOf(_T("T_CONTROL_CABLE_SETTING"));
	if(pControlTable)
	{
		m_Strings.RemoveAll();
		for(vector<CELOADTable::Record>::iterator itr = pControlTable->m_RecordEntry.begin(); itr != pControlTable->m_RecordEntry.end();++itr)
		{
			string rFieldValue;
			if(0 == nCol)
				rFieldValue = itr->FieldValueMap[_T("C_SPEC")].c_str();
			else if(1 == nCol)
				rFieldValue = itr->FieldValueMap[_T("C_CORE")].c_str();
			else if(2 == nCol)
				rFieldValue = itr->FieldValueMap[_T("C_SIZE")].c_str();
			m_Strings.Add(rFieldValue.c_str());
		}
	}

	CGridCellCombo::Edit(nRow , nCol , rect , point , nID , nChar);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGridCellControlTypeCombo 
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CGridCellControlTypeCombo, CGridCellCombo)

CGridCellControlTypeCombo::CGridCellControlTypeCombo() : CGridCellCombo()
{
}

// Create a control to do the editing
BOOL CGridCellControlTypeCombo::Edit(int nRow, int nCol, CRect rect, CPoint point , UINT nID, UINT nChar)
{
	vector<string> ExceptItemList;
	for(int i = 1;i < GetGrid()->GetRowCount();++i)
	{
		if(i == nRow) continue;
		ExceptItemList.push_back(GetGrid()->GetItemText(i , 0).operator LPCSTR());
	}

	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pMapTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_TYPE_MAP"));
	if(pMapTable)
	{
		m_Strings.RemoveAll();
		for(vector<CELOADTable::Record>::iterator itr = pMapTable->m_RecordEntry.begin(); itr != pMapTable->m_RecordEntry.end();++itr)
		{
			if(m_rSchematicType == itr->FieldValueMap[_T("C_SCHEM_TYPE")])
			{
				const string rControlType = itr->FieldValueMap[_T("C_CONTROL_TYPE")];
				vector<string>::iterator where = find(ExceptItemList.begin() , ExceptItemList.end() , rControlType);
				if(where == ExceptItemList.end()) m_Strings.Add(rControlType.c_str());
			}
		}
	}

	CGridCellCombo::Edit(nRow , nCol , rect , point , nID , nChar);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////

// CControlCableCreationSettingDlg dialog

IMPLEMENT_DYNAMIC(CControlCableCreationSettingDlg, CWizardPropertyPage)

CControlCableCreationSettingDlg::CControlCableCreationSettingDlg(CWnd* pParent /*=NULL*/)
	: CWizardPropertyPage(CControlCableCreationSettingDlg::IDD/*, pParent*/)
	, m_bDeleted(false)
{
	m_bDragging = FALSE;
	m_pSchematicControlCableMapTableCopy = NULL;
	///m_pControlCableCreationSettingTableCopy = NULL;
	m_pControlCableControlTypeMapTableCopy = NULL;
	m_pSchematicControlTypeCopy = NULL;
	m_pSchematicTypeCopy = NULL;
}

CControlCableCreationSettingDlg::~CControlCableCreationSettingDlg()
{
	try
	{
		if(m_pSchematicControlCableMapTableCopy) SAFE_DELETE(m_pSchematicControlCableMapTableCopy);
		//if(m_pControlCableCreationSettingTableCopy) SAFE_DELETE(m_pControlCableCreationSettingTableCopy);
		if(m_pControlCableControlTypeMapTableCopy) SAFE_DELETE(m_pControlCableControlTypeMapTableCopy);
		if(m_pSchematicTypeCopy) SAFE_DELETE(m_pSchematicTypeCopy);
		if(m_pSchematicControlTypeCopy) SAFE_DELETE(m_pSchematicControlTypeCopy);
	}
	catch(...)
	{
	}
}

void CControlCableCreationSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CWizardPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX , IDC_BUTTON_CHECK , m_wndCheckButton);
	DDX_Control(pDX, ID_SETTING_SAVE, m_wndSettingSaveButton);
	DDX_Control(pDX, IDCANCEL, m_wndCancelButton);
	DDX_Control(pDX, IDC_CABLEDATA_ADD_BUTTON, m_wndCableAddButton);
	DDX_Control(pDX, ID_WIZBACK, m_wndBackButton);
	DDX_Control(pDX, ID_WIZNEXT, m_wndNextButton);

	DDX_Control(pDX, IDC_CONTROL_TYPE_LIST, m_wndLcsLocalTypeListCtrl);
	DDX_Control(pDX, IDC_CONTROL_CABLE_SPEC_COMBO, m_wndControlCableSpecCombo);
	DDX_Control(pDX, IDC_CABLE_PREFIX_EDIT, m_wndCablePrefixEdit);
	DDX_Control(pDX, IDC_CABLE_SUFFIX_EDIT, m_wndCableSuffixEdit);
	DDX_Control(pDX, IDC_CONTROL_TYPE_LIST2, m_wndLcpIoTypeListCtrl);
	DDX_Control(pDX, IDC_CONTROL_TYPE_LIST3, m_wndDcsIoTypeListCtrl);
	DDX_Control(pDX, IDC_BUTTON_EXPORT, m_wndExportButton);
}


BEGIN_MESSAGE_MAP(CControlCableCreationSettingDlg, CPropertyPage)
	ON_BN_CLICKED(ID_SETTING_SAVE, &CControlCableCreationSettingDlg::OnBnClickedSettingSave)
	ON_BN_CLICKED(IDCANCEL, &CControlCableCreationSettingDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CABLEDATA_ADD_BUTTON, &CControlCableCreationSettingDlg::OnBnClickedCabledataAddButton)
	ON_BN_CLICKED(ID_WIZBACK, &CControlCableCreationSettingDlg::OnBnClickedWizback)
	ON_BN_CLICKED(ID_WIZNEXT, &CControlCableCreationSettingDlg::OnBnClickedWiznext)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY(LVN_BEGINDRAG, IDC_CONTROL_TYPE_LIST, &CControlCableCreationSettingDlg::OnLvnBegindragControlTypeList)
	ON_COMMAND(ID_ITEM_COPY, &CControlCableCreationSettingDlg::OnItemCopy)
	ON_COMMAND(ID_ITEM_DELETE, &CControlCableCreationSettingDlg::OnItemDelete)
	ON_COMMAND(ID_SCHEMATICMENU_DELETE, &CControlCableCreationSettingDlg::OnSchematicmenuDelete)
	ON_COMMAND(ID_SCHEMATICMENU_MODIFY, &CControlCableCreationSettingDlg::OnSchematicmenuModify)
	ON_COMMAND(ID_CONTROLTYPEMENU_DELETE, &CControlCableCreationSettingDlg::OnControltypemenuDelete)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_CONTROL_TYPE_LIST2, &CControlCableCreationSettingDlg::OnLvnBegindragControlTypeList2)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_CONTROL_TYPE_LIST3, &CControlCableCreationSettingDlg::OnLvnBegindragControlTypeList3)
	ON_BN_CLICKED(IDC_BUTTON_CHECK, &CControlCableCreationSettingDlg::OnBnClickedButtonCheck)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, &CControlCableCreationSettingDlg::OnBnClickedButtonExport)
END_MESSAGE_MAP()


// CControlCableCreationSettingDlg message handlers

BOOL CControlCableCreationSettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CELoadDocData & docData = CELoadDocData::GetInstance();
	{
		CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_TYPE"));
		if(pTable) m_pSchematicTypeCopy = pTable->Clone();
	}

	{
		CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_TYPE_MAP"));
		if(pTable) m_pSchematicControlTypeCopy = pTable->Clone();
	}

	{
		CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_CABLE_MAP"));
		if(pTable) m_pSchematicControlCableMapTableCopy = pTable->Clone();
	}
	
	{
		CELOADTable* pTable = docData.GetTableOf(_T("T_CONTROL_CABLE_CONTROL_TYPE_MAP"));
		if(pTable) m_pControlCableControlTypeMapTableCopy = pTable->Clone();
	}

	//! CREATE TREE LIST CTRL
	if(ERROR_SUCCESS != CreateSchematicTreeList())
	{
		TRACE0("Failed to create Properies Grid \n");
		return FALSE;
	}

	m_hIcon = AfxGetApp()->LoadIcon(IDI_GRABBER);

	CreateImageList();

	m_wndSchematicTreeListCtrl.SetImageList(&m_imgList, LVSIL_NORMAL);
	m_wndLcsLocalTypeListCtrl.SetImageList(&m_imgList, LVSIL_SMALL);
	m_wndLcpIoTypeListCtrl.SetImageList(&m_imgList, LVSIL_SMALL);
	m_wndDcsIoTypeListCtrl.SetImageList(&m_imgList, LVSIL_SMALL);

	//DisplaySchematicTypeData();

	//! control type List 해더생성
	if(ERROR_SUCCESS != CreateControlTypeList())
	{
		TRACE0("Failed to create Control List \n");
		return FALSE;
	}

	//! BUTTON SETTING

	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	m_wndSettingSaveButton.SetTextHotColor(TextHotColor);
	m_wndCancelButton.SetTextHotColor(TextHotColor);
	m_wndExportButton.SetTextHotColor(TextHotColor);

	m_wndCableAddButton.SetTextHotColor(TextHotColor);
	m_wndCableAddButton.SetIcon(IDI_ADD);
	m_wndExportButton.SetIcon(IDI_EXCEL);

	m_wndBackButton.SetTextHotColor(TextHotColor);
	m_wndNextButton.SetTextHotColor(TextHotColor);
	m_wndBackButton.SetIcon(IDI_PRE);
	m_wndNextButton.SetIcon(IDI_NEXT);

        m_wndSettingSaveButton.SetIcon(IDI_OK_18X18);
        m_wndCancelButton.SetIcon(IDI_CANCEL_18X18);

	return Init();  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	

	@author BHK	

	@date 2009-04-15 오후 11:34:44	

	@param	

	@return		
**/
BOOL CControlCableCreationSettingDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if(wParam == m_wndSchematicTreeListCtrl.GetDlgCtrlID())
	{
		struct TREELISTINFO *ptlistinfo = (struct TREELISTINFO *)lParam;
		if((WM_RBUTTONUP == ptlistinfo->code) && ptlistinfo->hItem && (NULL == m_wndSchematicTreeListCtrl.GetParentItem(ptlistinfo->hItem)))
		{
			//! this is schematic type tree item.
			const CString rItemText = m_wndSchematicTreeListCtrl.GetItemText(ptlistinfo->hItem , 0);
			CMenu menu;
			if(menu.CreatePopupMenu())
			{
				CPoint point;
				GetCursorPos(&point);
				///ScreenToClient(&point);

				menu.AppendMenu(0 , ID_ITEM_COPY , _T("Copy"));
				menu.AppendMenu(0 , ID_ITEM_DELETE , _T("Delete"));
				menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, point.y, this);
			}
		}
		else if((WM_RBUTTONUP == ptlistinfo->code) && ptlistinfo->hItem && (NULL != m_wndSchematicTreeListCtrl.GetParentItem(ptlistinfo->hItem))&& (NULL != m_wndSchematicTreeListCtrl.GetParentItem(ptlistinfo->hItem)))
		{
			HTREEITEM hParent = m_wndSchematicTreeListCtrl.GetParentItem(ptlistinfo->hItem);
			if(NULL == m_wndSchematicTreeListCtrl.GetParentItem(hParent))
			{
				//control cable type
				const CString rItemText = m_wndSchematicTreeListCtrl.GetItemText(ptlistinfo->hItem , 0);
				CMenu menu;
				if(menu.CreatePopupMenu())
				{
					CPoint point;
					GetCursorPos(&point);

					menu.AppendMenu(0 , ID_SCHEMATICMENU_MODIFY , _T("Modify"));
					menu.AppendMenu(0 , ID_SCHEMATICMENU_DELETE , _T("Delete"));
					menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, point.y, this);
				}
			}
			else
			{
				//contorl type
				const CString rItemText = m_wndSchematicTreeListCtrl.GetItemText(ptlistinfo->hItem , 0);
				CMenu menu;
				if(menu.CreatePopupMenu())
				{
					CPoint point;
					GetCursorPos(&point);

					menu.AppendMenu(0 , ID_CONTROLTYPEMENU_DELETE , _T("Delete"));
					menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, point.y, this);
				}
			}
		}
	}

	return CPropertyPage::OnNotify(wParam, lParam, pResult);
}

/**
	@brief	

	@author BHK	

	@date 2009-06-26 오전 10:53:08	

	@param	

	@return		
*/
BOOL CControlCableCreationSettingDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN) 
	{
		if(pMsg->wParam == VK_ESCAPE) return TRUE;
		if(pMsg->wParam == VK_RETURN)
		{
			if(pMsg->hwnd == GetDlgItem(IDC_EDIT_CABLE_DESC)->GetSafeHwnd())
			{
				OnBnClickedCabledataAddButton();
			}

			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}


static int CALLBACK 
MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	// lParamSort contains a pointer to the tree control.
	// The lParam of an item is just its handle, 
	// as specified with <A HREF="_mfc_ctreectrl.3a3a.setitemdata.htm">SetItemData</A>
	CTreeListCtrlEx* pmyTreeCtrl = (CTreeListCtrlEx*)lParamSort;

	CString rText = pmyTreeCtrl->GetItemText((HTREEITEM) lParam1);

	CString strLItem1 = pmyTreeCtrl->GetItemText((HTREEITEM) lParam1, 1);
	CString strLItem2 = pmyTreeCtrl->GetItemText((HTREEITEM) lParam1, 2);
	CString lItem = strLItem1 + strLItem2;

	CString strRItem1 = pmyTreeCtrl->GetItemText((HTREEITEM) lParam2, 1);
	CString strRItem2 = pmyTreeCtrl->GetItemText((HTREEITEM) lParam2, 2);
	CString rItem = strRItem1 + strRItem2;

	//return strcmp(lItem, rItem);
	return lItem.Compare(rItem);
}



/**
	@brief	INSERT ITEM IN SCHEMATIC LIST CTRL

	@author KHS	

	@return		
**/
HTREEITEM CControlCableCreationSettingDlg::InsertItemIntoSchematicListCtrl(const CString& rType, const CString& rDesc)
{
	//! Schematic Type을 뿌려준다.
	TV_INSERTSTRUCT tvStruct;

	tvStruct.hParent = NULL;
	tvStruct.hInsertAfter = TVI_SORT;
	tvStruct.item.iImage = 0;
	tvStruct.item.iSelectedImage = 0;
	tvStruct.item.pszText = (LPSTR)(LPCTSTR)rType;
	tvStruct.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

	HTREEITEM hParentItem = m_wndSchematicTreeListCtrl.InsertItem(&tvStruct);
	if(hParentItem)
	{
		m_wndSchematicTreeListCtrl.SetItemText(hParentItem , rDesc , 3);
	}
	
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pMapTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_CABLE_MAP"));
	if(pMapTable)
	{
		for(vector<CELOADTable::Record>::iterator itr = pMapTable->m_RecordEntry.begin(); itr != pMapTable->m_RecordEntry.end();++itr)
		{
			const string rSchemType = itr->FieldValueMap[_T("C_SCHEM_TYPE")];
			const string rCableType = itr->FieldValueMap[_T("C_CABLE_TYPE")];
			const string rCableCore = itr->FieldValueMap[_T("C_CORE")];
			const string rCableSize = itr->FieldValueMap[_T("C_SIZE")];
			const string rPrefix = itr->FieldValueMap[_T("C_NAMING_RULE_PREFIX")];
			const string rSuffix = itr->FieldValueMap[_T("C_NAMING_RULE_SUFFIX")];
			const string rDesc = itr->FieldValueMap[_T("C_DESC")];
			if(rSchemType.c_str() == rType)
			{
				const string rCableData = rCableType+ " " + rCableCore + "-" + rCableSize;
				
				tvStruct.hParent = hParentItem;
				//tvStruct.hInsertAfter = TVI_SORT;
				tvStruct.item.iImage = 3;
				tvStruct.item.iSelectedImage = 3;
				tvStruct.item.pszText = (LPSTR)(LPCTSTR)rCableData.c_str();
				tvStruct.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

				HTREEITEM hItem = m_wndSchematicTreeListCtrl.InsertItem(&tvStruct);
				if(hItem)
				{
					m_wndSchematicTreeListCtrl.SetItemData(hItem , (DWORD)hItem);
					m_wndSchematicTreeListCtrl.SetItemText(hItem, rPrefix.c_str(), 1);
					m_wndSchematicTreeListCtrl.SetItemText(hItem, rSuffix.c_str(), 2);
					m_wndSchematicTreeListCtrl.SetItemText(hItem, rDesc.c_str(), 3);
				
					TVSORTCB tvs;

					tvs.hParent = hParentItem;
					tvs.lpfnCompare = MyCompareProc;
					tvs.lParam = (LPARAM)&m_wndSchematicTreeListCtrl;

					m_wndSchematicTreeListCtrl.SortChildrenCB(&tvs);

					DisplayControlTypeTreeNodeOf(hItem , rSchemType , rCableType , rCableCore , rCableSize , rPrefix , rSuffix, rDesc);
					m_wndSchematicTreeListCtrl.Expand(hItem, TVE_EXPAND);
				}
			}
		}
		m_wndSchematicTreeListCtrl.Expand(hParentItem, TVE_EXPAND);
	}
	return hParentItem;
}


/**
	@brief	

	@author BHK	

	@date 2009-05-11 오후 12:35:53	

	@param	

	@return		
*/
HTREEITEM CControlCableCreationSettingDlg::AddControlItemToSchemType(CELOADTable::Record& record)
{
	HTREEITEM hItem = m_wndSchematicTreeListCtrl.GetSelectedItem();
	if(NULL != hItem)
	{
		const CString rCableType = record.FieldValueMap[_T("C_CABLE_TYPE")].c_str();
		const CString rCableCore = record.FieldValueMap[_T("C_CORE")].c_str();
		const CString rCableSize = record.FieldValueMap[_T("C_SIZE")].c_str();
		const CString rCableData = rCableType + " " + rCableCore + "-" + rCableSize;

		TV_INSERTSTRUCT tvStruct;

		tvStruct.hParent = hItem;
		//tvStruct.hInsertAfter = TVI_SORT;
		tvStruct.item.iImage = 3;
		tvStruct.item.iSelectedImage = 3;
		tvStruct.item.pszText = (LPSTR)(LPCTSTR)rCableData;
		tvStruct.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

		HTREEITEM hNewItem = m_wndSchematicTreeListCtrl.InsertItem(&tvStruct);
		if(NULL != hNewItem)
		{
			const CString rPrefix = record.FieldValueMap[_T("C_NAMING_RULE_PREFIX")].c_str();
			const CString rSuffix = record.FieldValueMap[_T("C_NAMING_RULE_SUFFIX")].c_str();
			const CString rDesc   = record.FieldValueMap[_T("C_DESC")].c_str();
			
			m_wndSchematicTreeListCtrl.SetItemData(hNewItem , (DWORD)hNewItem);
			m_wndSchematicTreeListCtrl.SetItemText(hNewItem , rPrefix , 1);
			m_wndSchematicTreeListCtrl.SetItemText(hNewItem , rSuffix , 2);
			m_wndSchematicTreeListCtrl.SetItemText(hNewItem , rDesc , 3);
			//m_wndSchematicTreeListCtrl.SortChildren(hItem, 1);

			TVSORTCB tvs;

			tvs.hParent = hItem;
			tvs.lpfnCompare = MyCompareProc;
			tvs.lParam = (LPARAM)&m_wndSchematicTreeListCtrl;

			m_wndSchematicTreeListCtrl.SortChildrenCB(&tvs);
		}

		return hNewItem;
	}

	return NULL;
}

/**
	@brief	SAVE

	@author KHS	

	@return		
**/
void CControlCableCreationSettingDlg::OnBnClickedSettingSave()
{
	m_bModified = false;

	CELoadDocData& docData = CELoadDocData::GetInstance();
	///OnChangeSchematicType();	//! 현재 상태를 저장하기 위해 호출한다.

	if(ERROR_SUCCESS == SaveData())
		AfxMessageBox(_T("SAVED"));
	else	AfxMessageBox(_T("FAIL TO SAVE"));
}

/**
	@brief	

	@author BHK	

	@date 2009-05-13 오전 10:35:56	

	@param	

	@return		
*/
void CControlCableCreationSettingDlg::OnBnClickedCancel()
{
	if(m_bModified)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		if(IDYES == AfxMessageBox(docData.GetStringOf(48).c_str() , MB_YESNO|MB_ICONWARNING ))
		{
			SaveData();
			m_bModified = false;
			GetParent()->SendMessage(WM_COMMAND , IDOK);
		}
		else
		{
			{
				CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_TYPE"));
				if(m_pSchematicTypeCopy && pTable) pTable->Copy(m_pSchematicTypeCopy);
			}

			{
				CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_TYPE_MAP"));
				if(m_pSchematicControlTypeCopy && pTable) pTable->Copy(m_pSchematicControlTypeCopy);
			}

			{
				CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_CABLE_MAP"));
				if(m_pSchematicControlCableMapTableCopy && pTable) pTable->Copy(m_pSchematicControlCableMapTableCopy);
			}

			{
				CELOADTable* pTable = docData.GetTableOf(_T("T_CONTROL_CABLE_CONTROL_TYPE_MAP"));
				if(m_pControlCableControlTypeMapTableCopy && pTable) pTable->Copy(m_pControlCableControlTypeMapTableCopy);
			}
			GetParent()->SendMessage(WM_COMMAND , IDCANCEL);
		}
	}
	else
	{
		GetParent()->SendMessage(WM_COMMAND , IDCANCEL);
	}
}


/**
	@brief	ADD

	@author KHS	

	@return		
**/
void CControlCableCreationSettingDlg::OnBnClickedCabledataAddButton()
{
	m_bModified = true;

	CString rCableType, rCableCore, rCableSize, rPrefix, rSuffix , rDesc;

	const int nSel = m_wndControlCableSpecCombo.GetCurSel();
	HTREEITEM hItem = m_wndSchematicTreeListCtrl.GetSelectedItem();
	HTREEITEM hParent = m_wndSchematicTreeListCtrl.GetParentItem(hItem);
	if(-1 != nSel)
	{
		if(NULL== hParent && hItem)
		{
			const CString rSchematicType = m_wndSchematicTreeListCtrl.GetItemText(hItem);

			CELoadDocData& docData = CELoadDocData::GetInstance();
			CELOADTable* pTable = docData.GetTableOf(_T("T_CONTROL_CABLE_SETTING"));
			if(pTable && (nSel < pTable->m_RecordEntry.size()))
			{
				rCableType = pTable->m_RecordEntry[nSel].FieldValueMap[_T("C_SPEC")].c_str();
				rCableCore = pTable->m_RecordEntry[nSel].FieldValueMap[_T("C_CORE")].c_str();
				rCableSize = pTable->m_RecordEntry[nSel].FieldValueMap[_T("C_SIZE")].c_str();
			}

			m_wndCablePrefixEdit.GetWindowText(rPrefix);
			m_wndCableSuffixEdit.GetWindowText(rSuffix);
			GetDlgItemText(IDC_EDIT_CABLE_DESC , rDesc);

			pTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_CABLE_MAP"));
			if(pTable)
			{
				CELOADTable::Record record;

				record.FieldValueMap[_T("C_SCHEM_TYPE")]		= rSchematicType;
				record.FieldValueMap[_T("C_CABLE_TYPE")]		= rCableType;
				record.FieldValueMap[_T("C_CORE")]			= rCableCore;
				record.FieldValueMap[_T("C_SIZE")]			= rCableSize;
				record.FieldValueMap[_T("C_NAMING_RULE_PREFIX")]	= rPrefix;
				record.FieldValueMap[_T("C_NAMING_RULE_SUFFIX")]	= rSuffix;
				record.FieldValueMap[_T("C_DESC")]			= rDesc;

				//! 중복체크
				if(!CompareMatchCableType(record))
				{
					pTable->m_RecordEntry.push_back(record);

					HTREEITEM hCableItem = AddControlItemToSchemType(record);
					
					//!SCROL 이동
					//! 해당 SCHEMATIC TYPE 아이템으로 스크롤 바 이동
					m_wndSchematicTreeListCtrl.Select(hItem, TVGN_FIRSTVISIBLE);

					RECT itemRect , clientRect;
					m_wndSchematicTreeListCtrl.GetClientRect(&clientRect);
					const int nRow = m_wndSchematicTreeListCtrl.GetRowOfItem(hItem);
					m_wndSchematicTreeListCtrl.ListGetItemRect(0 , 0 , &itemRect, LVIR_BOUNDS);   // 한칸의 높이를 측정
					//if((nRow * (itemRect.bottom - itemRect.top)) > (clientRect.bottom - clientRect.top))
					{
						//! 선택한 Schematic Type이 화면에 보이도록 스크롤바를 이동한다.
						m_wndSchematicTreeListCtrl.SelectItem(hItem);
					}

					m_wndCablePrefixEdit.SetWindowText("");
					m_wndCableSuffixEdit.SetWindowText("");
					SetDlgItemText(IDC_EDIT_CABLE_DESC , "");
				}
				else
				{
					::MessageBox(NULL, "Cable Data가 이미 존재합니다!! ", "Warning!", MB_OK|MB_ICONWARNING );
					return;
				}
			}
		}
		else
		{
			::MessageBox(NULL, "Schematic Type을 선택하세요!! ", "Warning!", MB_OK|MB_ICONWARNING );
			return;
		}
	}
}


/**
	@brief	데이타를 화면에 출력한다.

	@author KHS	

	@return		
**/
int CControlCableCreationSettingDlg::DisplaySchematicTypeData(void)
{
	//! SCHEMATIC TYPE을 가져와 뿌려준다.
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pSchematicTable = docData.GetTableOf(_T("T_SCHEMATIC_TYPE"));
	if(pSchematicTable)
	{
		for(vector<CELOADTable::Record>::iterator jtr = pSchematicTable->m_RecordEntry.begin(); jtr != pSchematicTable->m_RecordEntry.end();++jtr)
		{
			const CString rType = jtr->FieldValueMap[_T("C_TYPE")].c_str();
			const CString rDesc = jtr->FieldValueMap[_T("C_DESC")].c_str();

			InsertItemIntoSchematicListCtrl(rType, rDesc);
		}
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	화면 데이타를 저장한다.

	@author KHS	

	@return		
**/
int CControlCableCreationSettingDlg::SaveData(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	{
		CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_TYPE"));
		if(pTable)
		{
			pTable->Save();
			if(m_pSchematicTypeCopy) m_pSchematicTypeCopy->Copy(pTable);
		}
		else	return ERROR_BAD_ENVIRONMENT;
	}

	{
		CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_TYPE_MAP"));
		if(pTable)
		{
			pTable->Save();
			if(m_pSchematicControlTypeCopy) m_pSchematicControlTypeCopy->Copy(pTable);
		}
		else	return ERROR_BAD_ENVIRONMENT;
	}

	{
		CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_CABLE_MAP"));
		if(pTable) 
		{
			pTable->Save();
			if(m_pSchematicControlCableMapTableCopy) m_pSchematicControlCableMapTableCopy->Copy(pTable);
		}
		else	return ERROR_BAD_ENVIRONMENT;
	}

	{
		CELOADTable* pTable = docData.GetTableOf(_T("T_CONTROL_CABLE_CONTROL_TYPE_MAP"));
		if(pTable)
		{
			pTable->Save();
			if(m_pControlCableControlTypeMapTableCopy) m_pControlCableControlTypeMapTableCopy->Copy(pTable);
		}
		else	return ERROR_BAD_ENVIRONMENT;
	}

	/*if(m_bDeleted)
	{
		return docData.Save(CLoadItem::TableName(), CLoadItem::TypeString(),  AfxGetMainWnd());
		m_bDeleted = false;
	}*/

	return ERROR_SUCCESS;
}


/**
	@brief	

	@author BHK	

	@date 2009-04-16 오전 11:28:05	

	@param	

	@return		
*/
int CControlCableCreationSettingDlg::CheckControlCableControlTypeMap(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	
	//! 실제로 사용된 Control Type을 구한다.
	map<string,list<string> > UsedSchematicControlTypeMap;
	{
		CELOADTable* pTable = docData.GetTableOf(_T("T_CONTROL_CABLE_CONTROL_TYPE_MAP"));
		if(pTable)
		{
			for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin();itr != pTable->m_RecordEntry.end();++itr)
			{
				const string rSchemType   = itr->FieldValueMap[_T("C_SCHEM_TYPE")];
				const string rControlType = itr->FieldValueMap[_T("C_CONTROL_TYPE")];
				UsedSchematicControlTypeMap[rSchemType].push_back(rControlType);
			}
		}
	}

	map<string,list<string> > SchematicControlTypeMap2;
	{
		CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_TYPE_MAP"));
		if(pTable)
		{
			for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin();itr != pTable->m_RecordEntry.end();++itr)
			{
				const string rSchemType   = itr->FieldValueMap[_T("C_SCHEM_TYPE")];
				const string rControlType = itr->FieldValueMap[_T("C_CONTROL_TYPE")];
				SchematicControlTypeMap2[rSchemType].push_back(rControlType);
			}
		}
	}

	set<string> errors;
	//! Schematic Control Type Map에서 할당한 Control Type이 실제 사용되었는지 여부를 확인하여 사용되지 않았을 경우에 메세지를 나타낸다.
	for(map<string,list<string> >::iterator itr = SchematicControlTypeMap2.begin();itr != SchematicControlTypeMap2.end();++itr)
	{
		map<string,list<string> >::iterator where = UsedSchematicControlTypeMap.find(itr->first);
		if(where != UsedSchematicControlTypeMap.end())
		{
			for(list<string>::iterator jtr = itr->second.begin();jtr != itr->second.end();++jtr)
			{
				list<string>::iterator at = find(where->second.begin() , where->second.end() , *jtr);
				//! Control Type이 사용되지 않았을 때...
				if(at == where->second.end()) errors.insert(itr->first);
			}
		}else	errors.insert(itr->first);
	}

	//! 사용되지 않은 Control Type을 표기한다.
	if(!errors.empty())
	{
		CString msg;
		for(set<string>::iterator itr = errors.begin();itr != errors.end();++itr)
		{
			if(!itr->empty()) msg += itr->c_str() + CString(_T("\n"));
		}

		if(msg.IsEmpty())
		{
			::MessageBox(NULL, "OK !", "Information", MB_OK|MB_ICONINFORMATION);
		}
		else
		{
			CSchematicTypeCheckDlg dlg(msg);
			if(IDOK == dlg.DoModal())
			{
			}
		}

		///return ERROR_BAD_ENVIRONMENT;
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-22 오후 3:19:17	

	@param	

	@return		
*/
void CControlCableCreationSettingDlg::OnBnClickedWizback()
{
	if(m_bModified)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		if(IDYES == AfxMessageBox(docData.GetStringOf(50).c_str() , MB_YESNO|MB_ICONWARNING ))
		{
			SaveData();
			m_bModified = false;
			GetParent()->SendMessage(WM_COMMAND , ID_WIZBACK , 0);
		}
		else
		{
			GetParent()->SendMessage(WM_COMMAND , ID_WIZBACK , 0);
		}
		
	}
	else
	{
		GetParent()->SendMessage(WM_COMMAND , ID_WIZBACK , 0);
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-04-22 오후 3:20:24	

	@param	

	@return		
*/
void CControlCableCreationSettingDlg::OnBnClickedWiznext()
{
	if(m_bModified)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		if(IDYES == AfxMessageBox(docData.GetStringOf(48).c_str() , MB_YESNO|MB_ICONWARNING ))
		{
			SaveData();
			m_bModified = false;
			GetParent()->SendMessage(WM_COMMAND , ID_WIZNEXT, 0);
		}
		else
		{
			GetParent()->SendMessage(WM_COMMAND , ID_WIZNEXT, 0);
		}
		
	}
	else
	{
		GetParent()->SendMessage(WM_COMMAND , ID_WIZNEXT, 0);
	}

}

/**
	@brief	SCHEMATIC TREE LIST CTRL를 생성한다.

	@author BHK	

	@date 2009-04-23 오후 9:06:08	

	@param	

	@return		
*/
int CControlCableCreationSettingDlg::CreateSchematicTreeList(void)
{
	static const CString __HEADERS__[] = {_T("TYPE"), _T("PREFIX") , _T("SUFFIX") , _T("DESC")};

	CRect rect;
	CPoint point(0,0);
	GetDlgItem(IDC_MAIN_SCHEMATIC_STATIC)->GetWindowRect(&rect);
	ScreenToClient(&rect);

	unsigned uStyle	= TVS_HASBUTTONS|TVS_HASLINES|TVS_FULLROWSELECT|TVS_NONEVENHEIGHT/*|TVS_CHECKBOXES*/;
	unsigned uExStyle = TVS_EX_SUBSELECT|TVS_EX_FULLROWMARK;

	if(m_wndSchematicTreeListCtrl.Create(uStyle|WS_CHILD|WS_VISIBLE|WS_BORDER,rect,this, 123))
	{
		m_wndSchematicTreeListCtrl.InsertColumn(0, __HEADERS__[0], LVCFMT_LEFT, 180);
		m_wndSchematicTreeListCtrl.InsertColumn(1, __HEADERS__[1], LVCFMT_LEFT, 60);
		m_wndSchematicTreeListCtrl.InsertColumn(2, __HEADERS__[2], LVCFMT_LEFT, 60);
		m_wndSchematicTreeListCtrl.InsertColumn(3, __HEADERS__[3], LVCFMT_LEFT, 200);

		m_wndSchematicTreeListCtrl.SetExtendedStyle(uExStyle);

		return ERROR_SUCCESS;
	}
	

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	이미지 리스트 생성

	@author KHS	

	@return		
**/
int CControlCableCreationSettingDlg::CreateImageList(void)
{
	UINT uiBmpId = IDB_CLASS_VIEW_HC;

	CBitmap bmp;
	if (!bmp.LoadBitmap (uiBmpId))
	{
		TRACE(_T ("Can't load bitmap: %x\n"), uiBmpId);
		ASSERT (FALSE);
		return FALSE;
	}
	BITMAP bmpObj;
	bmp.GetBitmap (&bmpObj);

	UINT nFlags = ILC_MASK | ILC_COLOR24;

	m_imgList.Create (16, bmpObj.bmHeight, nFlags, 0, 0);
	m_imgList.Add (&bmp, RGB (255, 0, 0));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_CONTROLCABLE));
	

	return ERROR_SUCCESS;
}

/**
	@brief	CONTROL TYPE LIST 생성

	@author KHS	

	@return		
**/
int CControlCableCreationSettingDlg::CreateControlTypeList(void)
{
	static const CString __HEADERS__[] = {_T("TYPE") , _T("IO TYPE"), _T("DESC."), _T("PREFIX"), _T("CATEGORY")};
	
	m_wndLcsLocalTypeListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES);

	m_wndLcsLocalTypeListCtrl.InsertColumn(0 , __HEADERS__[0]  , LVCFMT_LEFT , 90);
	m_wndLcsLocalTypeListCtrl.InsertColumn(1 , __HEADERS__[1]  , LVCFMT_LEFT , 60);
	m_wndLcsLocalTypeListCtrl.InsertColumn(2 , __HEADERS__[2]  , LVCFMT_LEFT , 230);
	m_wndLcsLocalTypeListCtrl.InsertColumn(3 , __HEADERS__[3]  , LVCFMT_LEFT , 60);
	m_wndLcsLocalTypeListCtrl.InsertColumn(4 , __HEADERS__[4]  , LVCFMT_LEFT , 100);

	m_wndLcpIoTypeListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES);

	m_wndLcpIoTypeListCtrl.InsertColumn(0 , __HEADERS__[0]  , LVCFMT_LEFT , 90);
	m_wndLcpIoTypeListCtrl.InsertColumn(1 , __HEADERS__[1]  , LVCFMT_LEFT , 60);
	m_wndLcpIoTypeListCtrl.InsertColumn(2 , __HEADERS__[2]  , LVCFMT_LEFT , 230);
	m_wndLcpIoTypeListCtrl.InsertColumn(3 , __HEADERS__[3]  , LVCFMT_LEFT , 60);
	m_wndLcpIoTypeListCtrl.InsertColumn(4 , __HEADERS__[4]  , LVCFMT_LEFT , 100);

	m_wndDcsIoTypeListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES);

	m_wndDcsIoTypeListCtrl.InsertColumn(0 , __HEADERS__[0]  , LVCFMT_LEFT , 90);
	m_wndDcsIoTypeListCtrl.InsertColumn(1 , __HEADERS__[1]  , LVCFMT_LEFT , 60);
	m_wndDcsIoTypeListCtrl.InsertColumn(2 , __HEADERS__[2]  , LVCFMT_LEFT , 230);
	m_wndDcsIoTypeListCtrl.InsertColumn(3 , __HEADERS__[3]  , LVCFMT_LEFT , 60);
	m_wndDcsIoTypeListCtrl.InsertColumn(4 , __HEADERS__[4]  , LVCFMT_LEFT , 100);

	return ERROR_SUCCESS;
}


/**
	@brief	콤보 박스를 케이블 데이타로 채운다.

	@author KHS	

	@return		
**/
int CControlCableCreationSettingDlg::FillComboItem(void)
{
	const int nItemCount = m_wndControlCableSpecCombo.GetCount();
	for(int i = nItemCount - 1;i >= 0;--i) m_wndControlCableSpecCombo.DeleteString(i);

	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pTable = docData.GetTableOf(_T("T_CONTROL_CABLE_SETTING"));
	if(pTable)
	{
		for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin(); itr != pTable->m_RecordEntry.end();++itr)
		{
			const CString rCableType	= itr->FieldValueMap[_T("C_SPEC")].c_str();
			const CString rCableCore	= itr->FieldValueMap[_T("C_CORE")].c_str();
			const CString rCableSize	= itr->FieldValueMap[_T("C_SIZE")].c_str();
			const CString rControlCableSpec = rCableType + _T(" ") + rCableCore + _T("-") + rCableSize;
			
			m_wndControlCableSpecCombo.AddString(rControlCableSpec);
		}
		m_wndControlCableSpecCombo.SetCurSel(0);

		return ERROR_SUCCESS;
	}
	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	MOUSE MOVE

	@author KHS	

	@return		
**/
void CControlCableCreationSettingDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bDragging)
	{
		//// Move the drag image
		CPoint pt(point);	//get our current mouse coordinates
		ClientToScreen(&pt); //convert to screen coordinates
		m_pDragImage->DragMove(pt); //move the drag image to those coordinates
		// Unlock window updates (this allows the dragging image to be shown smoothly)
		m_pDragImage->DragShowNolock(false);

		//// Get the CWnd pointer of the window that is under the mouse cursor
		CWnd* pDropWnd = WindowFromPoint (pt);
		ASSERT(pDropWnd); //make sure we have a window

		//// If we drag outside current window we need to adjust the highlights displayed
		if (pDropWnd != m_pDropList)
		{
			if (m_nDropIndex != -1) //If we drag over the CListCtrl header, turn off the hover highlight
			{
				TRACE("m_nDropIndex is -1\n");
				CTreeListCtrl* pList = (CTreeListCtrl*)m_pDropList;
				m_nDropIndex = -1;
			}
			else //If we drag out of the CListCtrl altogether
			{
				TRACE("m_nDropIndex is not -1\n");
			}
		}

		// Save current window pointer as the CListCtrl we are dropping onto
		m_pDropList = (CTreeListCtrl*)pDropWnd;

		// Convert from screen coordinates to drop target client coordinates
		pDropWnd->ScreenToClient(&pt);

		//If we are hovering over a CListCtrl we need to adjust the highlights
		if(pDropWnd->IsKindOf(RUNTIME_CLASS(CTreeListCtrl)) && (pDropWnd->GetSafeHwnd() == m_wndSchematicTreeListCtrl.GetSafeHwnd()))
		{
			TRACE("m_nDropIndex is not 100\n");
			//Note that we can drop here
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			UINT uFlags;
			CTreeListCtrl* pTreeList = (CTreeListCtrl*)pDropWnd;

			// Turn off hilight for previous drop target
			pTreeList->SetItemState(m_hDropItem , 0 , LVIS_DROPHILITED);
			// Redraw previous item
			//pTreeList->RedrawItems (m_nDropIndex, m_nDropIndex);
			///pTreeList->RedrawWindow();

			// Get the item that is below cursor
			m_hDropItem = ((CTreeListCtrl*)pDropWnd)->HitTest(pt, &uFlags);
			if(NULL != m_hDropItem)
			{
				if(m_pDragList && (m_pDragList->GetSafeHwnd() == m_wndLcsLocalTypeListCtrl.GetSafeHwnd()))
				{
					TRACE("m_nDropIndex is not 100\n");
					HTREEITEM hParentItem = ((CTreeListCtrl*)pDropWnd)->GetParentItem(m_hDropItem);
					if(NULL != hParentItem)
					{
						//Note that we can drop here
						SetCursor(LoadCursor(NULL, IDC_ARROW));
						CListCtrl* pList = (CListCtrl*)pDropWnd;
					}
					else
					{
						m_pDropList->SetItemState (m_hDropItem, 0, LVIS_DROPHILITED);
						SetCursor(LoadCursor(NULL, IDC_NO));
						return;
					}
				}

				// Highlight it
				///pTreeList->SetItemState(m_nDropIndex, LVIS_DROPHILITED, LVIS_DROPHILITED);
				pTreeList->SetItemState(m_hDropItem, LVIS_DROPHILITED, LVIS_DROPHILITED);
				// Redraw item
				//pList->RedrawItems(m_nDropIndex, m_nDropIndex);
				pTreeList->RedrawWindow();
			}else	SetCursor(LoadCursor(NULL, IDC_NO));
		}
		else
		{
			//If we are not hovering over a CListCtrl, change the cursor
			// to note that we cannot drop here
			SetCursor(LoadCursor(NULL, IDC_NO));
		}
		// Lock window updates
		m_pDragImage->DragShowNolock(true);
	}

	CPropertyPage::OnMouseMove(nFlags, point);
}

void CControlCableCreationSettingDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	// TODO: Add your message handler code here and/or call default
	if (m_bDragging)
	{
		// Release mouse capture, so that other controls can get control/messages
		ReleaseCapture ();
		
		// Note that we are NOT in a drag operation
		m_bDragging = FALSE;
		
		// End dragging image
		m_pDragImage->DragLeave (GetDesktopWindow ());
		m_pDragImage->EndDrag ();
		delete m_pDragImage; //must delete it because it was created at the beginning of the drag
		
		CPoint pt (point); //Get current mouse coordinates
		ClientToScreen (&pt); //Convert to screen coordinates
		// Get the CWnd pointer of the window that is under the mouse cursor
		CWnd* pDropWnd = WindowFromPoint (pt);
		ASSERT (pDropWnd); //make sure we have a window pointer
		// If window is CListCtrl, we perform the drop
		if (pDropWnd->IsKindOf (RUNTIME_CLASS (CTreeListCtrl)))
		{
			m_pDropList = (CTreeListCtrl*)pDropWnd; //Set pointer to the list we are dropping on
			DropItemOnList(m_pDragList, m_pDropList); //Call routine to perform the actual drop
		}
	}

	CPropertyPage::OnLButtonUp(nFlags, point);
}



/**
	@brief	BEGIN DRAG FROM CABLE DATA LIST

	@author KHS	

	@return		
**/
int CControlCableCreationSettingDlg::OnBeginDrag(LPNMLISTVIEW pNMLV, CListCtrl& ListCtrl)
{
	//// Save the index of the item being dragged in m_nDragIndex
	////  This will be used later for retrieving the info dragged
	m_nDragIndex = pNMLV->iItem;

	//// Create a drag image
	POINT pt;
	int nOffset = -10; //offset in pixels for drag image (positive is up and to the left; neg is down and to the right)
	if((pNMLV->hdr.hwndFrom == ListCtrl.GetSafeHwnd()) && (ListCtrl.GetSelectedCount() > 0)) //! more than one item is selected
	{
		pt.x = nOffset;
		pt.y = nOffset;

		m_pDragImage = ListCtrl.CreateDragImage(m_nDragIndex, &pt);
		ASSERT(m_pDragImage); //make sure it was created
		//We will call delete later (in LButtonUp) to clean this up

		CBitmap bitmap;
		if(ListCtrl.GetSelectedCount() > 1) //more than 1 item in list is selected
			//bitmap.LoadBitmap(IDB_BITMAP_MULTI);
			bitmap.LoadBitmap(IDB_BITMAP_MULTI_BOXES);
		else	bitmap.LoadBitmap(IDB_BITMAP_BOX);
		m_pDragImage->Replace(0, &bitmap, &bitmap);

		//// Change the cursor to the drag image
		////	(still must perform DragMove() in OnMouseMove() to show it moving)
		m_pDragImage->BeginDrag(0, CPoint(nOffset, nOffset - 4));
		m_pDragImage->DragEnter(GetDesktopWindow(), pNMLV->ptAction);

		//// Set dragging flag and others
		m_bDragging = TRUE;	//we are in a drag and drop operation
		m_nDropIndex = -1;	//we don't have a drop index yet
		m_pDragList = &ListCtrl; //make note of which list we are dragging from
		m_pDropList = &m_wndSchematicTreeListCtrl; //at present the drag list is the drop list

		//// Capture all mouse messages
		SetCapture ();
	}

	return ERROR_SUCCESS;
}

/**
	@brief	DROP ITEM IN SCHEMATIC TREELIST CTRL

	@author KHS	

	@return		
**/
void CControlCableCreationSettingDlg::DropItemOnList(CListCtrl* pDragList , CTreeListCtrl* pDropList)
{
	if(	(pDropList->GetSafeHwnd() == m_wndSchematicTreeListCtrl.GetSafeHwnd()))
	{
		// Turn off hilight for previous drop target
		pDropList->SetItemState (m_hDropItem, 0, LVIS_DROPHILITED);
		// Redraw previous item
		//pDropList->RedrawItems (m_nDropIndex, m_nDropIndex);

		CString rSelSchematicType;
		HTREEITEM hSelItem = pDropList->GetSelectedItem();//m_hDropItem
		HTREEITEM hSelParent = pDropList->GetParentItem(hSelItem);
		HTREEITEM hSelGrandParent = pDropList->GetParentItem(hSelParent);
		if(hSelParent && hSelGrandParent == NULL)
		{
			rSelSchematicType = pDropList->GetItemText(hSelParent);
		}
		else if(hSelGrandParent)
		{
			rSelSchematicType = pDropList->GetItemText(hSelGrandParent);
		}
		else
		{
			rSelSchematicType = pDropList->GetItemText(hSelItem);
		}

		HTREEITEM hParent = m_wndSchematicTreeListCtrl.GetParentItem(m_hDropItem);
		
		if(m_pDragList && (m_pDragList->GetSafeHwnd() == m_wndLcsLocalTypeListCtrl.GetSafeHwnd() || m_pDragList->GetSafeHwnd() == m_wndLcpIoTypeListCtrl.GetSafeHwnd() || m_wndDcsIoTypeListCtrl))
		{
			//! CONTROL DATA에 CONTROL TYPE을 할당할 때.
			HTREEITEM hGrandParent = m_wndSchematicTreeListCtrl.GetParentItem(hParent);
			HTREEITEM hChild = m_wndSchematicTreeListCtrl.GetChildItem(hSelItem);
			
			if(/*(NULL == hGrandParent) &&*/ (m_pDragList))// CABLE
			{
				HTREEITEM hCableItem = m_wndSchematicTreeListCtrl.GetChildItem(hParent);
				if(hCableItem == NULL)
				{
					::MessageBox(NULL, "Cable Data Type이 없습니다!! ", "Warning!", MB_OK|MB_ICONWARNING );
					return;
				}
				CString rSchematicType;
				if(hGrandParent)
				{
					rSchematicType = m_wndSchematicTreeListCtrl.GetItemText(hGrandParent);
				}
				else
				{
					rSchematicType = m_wndSchematicTreeListCtrl.GetItemText(hParent);
				}
				if(rSchematicType != rSelSchematicType) 
				{
					::MessageBox(NULL, "선택한 Control Type의 Schematic Type이 다릅니다.!! ", "Warning!", MB_OK|MB_ICONWARNING );
					return;
				}

				CString rCableData;
				HTREEITEM hDropItemParent = m_wndSchematicTreeListCtrl.GetParentItem(m_hDropItem);
				if(hGrandParent)
				{
					rCableData = m_wndSchematicTreeListCtrl.GetItemText(hDropItemParent , 0);
				}
				else
				{
					rCableData = m_wndSchematicTreeListCtrl.GetItemText(m_hDropItem , 0);
				}
				vector<string> oResult;
				CTokenizer<CIsSpace>::Tokenize(oResult , rCableData.operator LPCSTR() , CIsSpace());
				if(oResult.size() == 2)
				{
					const CString rCableType = oResult[0].c_str();
					const string rCableCoreSize = oResult[1];
					CTokenizer<CIsFromString>::Tokenize(oResult ,rCableCoreSize , CIsFromString("-"));
					const CString rCableCore = oResult[0].c_str();
					const CString rCableSize = oResult[1].c_str();
					const CString rCablePrefix = m_wndSchematicTreeListCtrl.GetItemText((hGrandParent)?hDropItemParent:m_hDropItem , 1);
					const CString rCableSuffix = m_wndSchematicTreeListCtrl.GetItemText((hGrandParent)?hDropItemParent:m_hDropItem  , 2);
					const CString rDesc = m_wndSchematicTreeListCtrl.GetItemText((hGrandParent)?hDropItemParent:m_hDropItem  , 3);

					const CString rControlType = m_pDragList->GetItemText(m_nDragIndex , 0);

					CELoadDocData& docData = CELoadDocData::GetInstance();

					HTREEITEM hDropChild = m_wndSchematicTreeListCtrl.GetChildItem(m_hDropItem);
					if(hGrandParent) hDropChild = m_hDropItem;
			
					if(hDropChild)
					{
						const CString rChildControl = m_pDropList->GetItemText(hDropChild);
						const CString rDragCategory = m_pDragList->GetItemText(m_nDragIndex, 4);

						CELOADTable* pControlTypeTable = docData.GetTableOf(_T("T_CONTROL_TYPE"));
						if(pControlTypeTable)
						{
							vector<CELOADTable::Record>::iterator where =find_if(pControlTypeTable->m_RecordEntry.begin(), pControlTypeTable->m_RecordEntry.end(), bind2nd(MatchType(), rChildControl));
							if(where != pControlTypeTable->m_RecordEntry.end())
							{
								CString rPreItemCategory = where->FieldValueMap[_T("C_CATEGORY")].c_str();
								if((rPreItemCategory == "DCS IO TYPE" || rDragCategory == "DCS IO TYPE") && (rPreItemCategory != rDragCategory))
								{
									::MessageBox(NULL, "선택한 Control Type의 Category가 다릅니다.!! ", "Warning!", MB_OK|MB_ICONWARNING );
									return;
								}
							}
						}
					}

					CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_TYPE_MAP"));
					if(pTable)
					{
						vector<CELOADTable::Record>::iterator where = find_if(pTable->m_RecordEntry.begin(),pTable->m_RecordEntry.end(),bind2nd(UsedControlType() , make_pair(rSchematicType,rControlType)));
						if(where != pTable->m_RecordEntry.end())
						{
							CELOADTable* pMapTable = docData.GetTableOf(_T("T_CONTROL_CABLE_CONTROL_TYPE_MAP"));
							if(pMapTable)
							{
								CELOADTable::Record record;

								record.FieldValueMap.insert(make_pair(_T("C_SCHEM_TYPE") , rSchematicType));
								record.FieldValueMap.insert(make_pair(_T("C_CABLE_TYPE") , rCableType));
								record.FieldValueMap.insert(make_pair(_T("C_CORE") , rCableCore));
								record.FieldValueMap.insert(make_pair(_T("C_SIZE") , rCableSize));
								record.FieldValueMap.insert(make_pair(_T("C_NAMING_RULE_PREFIX") , rCablePrefix));
								record.FieldValueMap.insert(make_pair(_T("C_NAMING_RULE_SUFFIX") , rCableSuffix));
								record.FieldValueMap.insert(make_pair(_T("C_CONTROL_TYPE") , rControlType));
								record.FieldValueMap.insert(make_pair(_T("C_DESC") , rDesc));

								//! CATEGORY 같은지 확인
								//vector<CELOADTable::Record>::iterator where = find_if(pMapTable->m_RecordEntry.begin()
								pMapTable->m_RecordEntry.push_back(record);

								m_pDragList->DeleteItem(m_nDragIndex);
								
								if(hGrandParent) m_hDropItem = hDropItemParent;
								InsertControlTypeTreeNod(m_hDropItem , rControlType.operator LPCSTR());
								m_wndSchematicTreeListCtrl.Expand(m_hDropItem, TVE_EXPAND);

							
								m_bModified = true;
							}
						}
						else
						{
							::MessageBox(NULL, "Schematic Type을 확인하세요.!! ", "Warning!", MB_OK|MB_ICONWARNING );
						}
					}
				}
			}
		}
	}
	
}

/**
	@brief	T_SCHEMATIC_CONTROL_CABLE_MAP 중복체크

	@author KHS	

	@return		
**/
BOOL CControlCableCreationSettingDlg::CompareMatchType(const CString& rSchematicType, const CString& rCableType, const CString& rCableCore, const CString& rCableSize, const CString& rPrefix, const CString& rSuffix)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pMapTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_CABLE_MAP"));
	if(pMapTable)
	{
		for(vector<CELOADTable::Record>::iterator itr = pMapTable->m_RecordEntry.begin(); itr != pMapTable->m_RecordEntry.end();++itr)
		{
			if(rSchematicType == itr->FieldValueMap[_T("C_SCHEM_TYPE")].c_str() && rCableType == itr->FieldValueMap[_T("C_CABLE_TYPE")].c_str() &&
				rCableCore == itr->FieldValueMap[_T("C_CORE")].c_str() && rCableSize == itr->FieldValueMap[_T("C_SIZE")].c_str() &&
				rPrefix == itr->FieldValueMap[_T("C_NAMING_RULE_PREFIX")].c_str() && rSuffix == itr->FieldValueMap[_T("C_NAMING_RULE_SUFFIX")].c_str())

			{
				return TRUE;
			}
		}
	}

	return FALSE;
}
/**
	@brief	SCHEMATIC TREELIST CTRL CLICK

	@author KHS	

	@return		
**/
void CControlCableCreationSettingDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_wndSchematicTreeListCtrl.GetSelectedItem();
	CString rText;
	CPoint ptTree = point;
	HTREEITEM hTreeItem = m_wndSchematicTreeListCtrl.GetSelectedItem();
	if(hTreeItem != NULL)
	{
		HTREEITEM hParent = m_wndSchematicTreeListCtrl.GetParentItem(hTreeItem);
		HTREEITEM hGrandParent = m_wndSchematicTreeListCtrl.GetParentItem(hParent);
		m_wndSchematicTreeListCtrl.SetFocus();
		m_wndSchematicTreeListCtrl.SelectItem (hTreeItem);

		if(NULL == hParent)
		{
			rText = m_wndSchematicTreeListCtrl.GetItemText(hTreeItem);

			DisplayControlTypeListOfSchematicType(rText.operator LPCSTR());
		}
		else if(hParent && hGrandParent == NULL)
		{
			rText = m_wndSchematicTreeListCtrl.GetItemText(hParent);

			DisplayControlTypeListOfSchematicType(rText.operator LPCSTR());
		}
		else if(hGrandParent)
		{
			rText = m_wndSchematicTreeListCtrl.GetItemText(hGrandParent);

			DisplayControlTypeListOfSchematicType(rText.operator LPCSTR());
		}
	}

	CPropertyPage::OnLButtonDown(nFlags, point);
}

/**
	@brief	CONTROL LIST DRAG

	@author KHS	

	@return		
**/
void CControlCableCreationSettingDlg::OnLvnBegindragControlTypeList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if(NULL == pNMLV) return;

	OnBeginDrag(pNMLV , m_wndLcsLocalTypeListCtrl);

	*pResult = 0;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-26 오전 10:26:35	

	@param	

	@return		
**/
int CControlCableCreationSettingDlg::DisplayControlTypeListOfSchematicType(const string& rSchemType)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	
	CELOADTable* pMapTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_TYPE_MAP"));
	if(pMapTable)
	{
		m_wndLcsLocalTypeListCtrl.DeleteAllItems();
		m_wndLcpIoTypeListCtrl.DeleteAllItems();
		m_wndDcsIoTypeListCtrl.DeleteAllItems();

		for(vector<CELOADTable::Record>::iterator itr = pMapTable->m_RecordEntry.begin(); itr != pMapTable->m_RecordEntry.end();)
		{
			if(rSchemType == itr->FieldValueMap[_T("C_SCHEM_TYPE")])
			{
				const string rControlType = itr->FieldValueMap[_T("C_CONTROL_TYPE")];

				CELOADTable* pCableControlTypeMapTable = docData.GetTableOf(_T("T_CONTROL_CABLE_CONTROL_TYPE_MAP"));
				if(pCableControlTypeMapTable)
				{
					//! SCHEMATIC TYPE에 CONTROL TYPE이 이미 사용되었다면 리스트에 뿌리지 않는다.
					vector<CELOADTable::Record>::iterator where = find_if(pCableControlTypeMapTable->m_RecordEntry.begin() , pCableControlTypeMapTable->m_RecordEntry.end() , bind2nd(UsedControlType() , make_pair(rSchemType,rControlType)));
					if(where != pCableControlTypeMapTable->m_RecordEntry.end())
					{
						CELOADTable* pSchemControlCableMapTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_CABLE_MAP"));
						if(pSchemControlCableMapTable)
						{
							vector<CELOADTable::Record>::iterator jtr;
							for(jtr = pSchemControlCableMapTable->m_RecordEntry.begin();jtr != pSchemControlCableMapTable->m_RecordEntry.end();++jtr)
							{
								if((where->FieldValueMap[_T("C_SCHEM_TYPE")] == jtr->FieldValueMap[_T("C_SCHEM_TYPE")]) && 
								(where->FieldValueMap[_T("C_CABLE_TYPE")] == jtr->FieldValueMap[_T("C_CABLE_TYPE")]) && 
								(where->FieldValueMap[_T("C_CORE")] == jtr->FieldValueMap[_T("C_CORE")]) &&
								(where->FieldValueMap[_T("C_SIZE")] == jtr->FieldValueMap[_T("C_SIZE")]) && 
								(where->FieldValueMap[_T("C_NAMING_RULE_PREFIX")] == jtr->FieldValueMap[_T("C_NAMING_RULE_PREFIX")]) &&
								(where->FieldValueMap[_T("C_NAMING_RULE_SUFFIX")] == jtr->FieldValueMap[_T("C_NAMING_RULE_SUFFIX")]) && 
								(where->FieldValueMap[_T("C_DESC")] == jtr->FieldValueMap[_T("C_DESC")]))
								{
									break;
								}
							}

							/*
							if(jtr != pSchemControlCableMapTable->m_RecordEntry.end())
							{
								++itr;
								continue;
							}
							*/
							//! T_SCHEMATIC_CONTROL_CABLE_MAP에 Control Data가 없으면 잘못된 자료이기 때문에 삭제하도록 한다.
							if(jtr == pSchemControlCableMapTable->m_RecordEntry.end())
							{
								pCableControlTypeMapTable->m_RecordEntry.erase(where);
							}
						}
						++itr;
						continue;
					}
				}

				CControlTypeTable* pControlTypeTable = static_cast<CControlTypeTable*>(docData.GetTableOf(_T("T_CONTROL_TYPE")));
				if(pControlTypeTable)
				{
					vector<CELOADTable::Record>::iterator where = find_if(pControlTypeTable->m_RecordEntry.begin(), pControlTypeTable->m_RecordEntry.end(), bind2nd(MatchType(), rControlType));
					if(where != pControlTypeTable->m_RecordEntry.end())
					{
						const CString rType   = where->FieldValueMap[_T("C_TYPE")].c_str();
						const CString rIoType = where->FieldValueMap[_T("C_IO_TYPE")].c_str();
						const CString rDesc   = where->FieldValueMap[_T("C_DESC")].c_str();
						const CString rIoTag  = where->FieldValueMap[_T("C_NAMING_RULE_PREFIX")].c_str();
						const CString rCheck  = where->FieldValueMap[_T("C_CHECK")].c_str();
						const CString rCategory  = where->FieldValueMap[_T("C_CATEGORY")].c_str();

						if(_T("YES") == rCheck)
						{
							if(rCategory == "LCS or LOCAL TYPE")
							{
								const int nItemCount = m_wndLcsLocalTypeListCtrl.GetItemCount() + 1;

								const int nItem = m_wndLcsLocalTypeListCtrl.InsertItem(nItemCount, rType, 1);
								if(-1 != nItem)
								{
									m_wndLcsLocalTypeListCtrl.SetItemText(nItem, 1, rIoType);
									m_wndLcsLocalTypeListCtrl.SetItemText(nItem, 2, rDesc);
									m_wndLcsLocalTypeListCtrl.SetItemText(nItem, 3, rIoTag);
									m_wndLcsLocalTypeListCtrl.SetItemText(nItem, 4, rCategory);
								}
							}
							else if(rCategory == "LCP IO TYPE")
							{
								const int nItemCount = m_wndLcpIoTypeListCtrl.GetItemCount();

								const int nItem = m_wndLcpIoTypeListCtrl.InsertItem(nItemCount, rType, 1);
								if(-1 != nItem)
								{
									m_wndLcpIoTypeListCtrl.SetItemText(nItem, 1, rIoType);
									m_wndLcpIoTypeListCtrl.SetItemText(nItem, 2, rDesc);
									m_wndLcpIoTypeListCtrl.SetItemText(nItem, 3, rIoTag);
									m_wndLcpIoTypeListCtrl.SetItemText(nItem, 4, rCategory);
								}
							}
							else
							{
								const int nItemCount = m_wndDcsIoTypeListCtrl.GetItemCount();

								const int nItem = m_wndDcsIoTypeListCtrl.InsertItem(nItemCount, rType, 1);
								if(-1 != nItem)
								{
									m_wndDcsIoTypeListCtrl.SetItemText(nItem, 1, rIoType);
									m_wndDcsIoTypeListCtrl.SetItemText(nItem, 2, rDesc);
									m_wndDcsIoTypeListCtrl.SetItemText(nItem, 3, rIoTag);
									m_wndDcsIoTypeListCtrl.SetItemText(nItem, 4, rCategory);
								}
							}
						}
						else
						{
							//! CONTROL TYPE TABLE에서 사용하도록 선택하지 않은 것은 MAP TABLE에서 삭제한다.
							itr = pMapTable->m_RecordEntry.erase(itr);
							continue;
						}
					}
					else
					{
						//! CONTROL TYPE TABLE에서 삭제된 것은 MAP TABLE에서도 삭제한다.
						itr = pMapTable->m_RecordEntry.erase(itr);
						continue;
					}
				}
			}
			++itr;
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-26 오전 10:33:21	

	@param	

	@return		
**/
int CControlCableCreationSettingDlg::DisplayControlTypeTreeNodeOf(HTREEITEM hItem,const string& rSchemType , 
								  const string& rCableType , const string& rCableCore ,const string& rCableSize, 
								  const string& rCablePrefix , const string& rCableSuffix, const string& rDesc)
{
	if(hItem)
	{
		//! 트리에 붙어 있던 CONTORL TYPE NODE들을 모두 삭제한다.
		HTREEITEM hChildItem = m_wndSchematicTreeListCtrl.GetChildItem(hItem);
		while(hChildItem)
		{
			HTREEITEM hNextChildItem = m_wndSchematicTreeListCtrl.GetNextSiblingItem(hChildItem);
			m_wndSchematicTreeListCtrl.DeleteItem(hChildItem);
			hChildItem = hNextChildItem;
		}

		CELoadDocData& docData = CELoadDocData::GetInstance();
		CELOADTable* pMapTable = docData.GetTableOf(_T("T_CONTROL_CABLE_CONTROL_TYPE_MAP"));
		if(pMapTable)
		{
			for(vector<CELOADTable::Record>::iterator itr = pMapTable->m_RecordEntry.begin();itr != pMapTable->m_RecordEntry.end();++itr)
			{
				if((rSchemType == itr->FieldValueMap[_T("C_SCHEM_TYPE")]) && (rCableType == itr->FieldValueMap[_T("C_CABLE_TYPE")]) &&
					(rCableCore == itr->FieldValueMap[_T("C_CORE")]) && (rCableSize == itr->FieldValueMap[_T("C_SIZE")]) &&
					(rCablePrefix == itr->FieldValueMap[_T("C_NAMING_RULE_PREFIX")]) && (rCableSuffix == itr->FieldValueMap[_T("C_NAMING_RULE_SUFFIX")]) &&
					(rDesc == itr->FieldValueMap[_T("C_DESC")]))
				{
					const string rControlType = itr->FieldValueMap[_T("C_CONTROL_TYPE")];
					
					InsertControlTypeTreeNod(hItem , rControlType);
				}
			}
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-26 오전 10:57:31	

	@param	

	@return		
**/
int CControlCableCreationSettingDlg::InsertControlTypeTreeNod(HTREEITEM hItem , const string& rControlType)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pControlTypeTable = docData.GetTableOf(_T("T_CONTROL_TYPE"));
	if(pControlTypeTable)
	{
		for(vector<CELOADTable::Record>::iterator jtr = pControlTypeTable->m_RecordEntry.begin();jtr != pControlTypeTable->m_RecordEntry.end();++jtr)
		{
			if(rControlType == jtr->FieldValueMap[_T("C_TYPE")])
			{
				TV_INSERTSTRUCT tvStruct;

				tvStruct.hParent = hItem;
				tvStruct.hInsertAfter = TVI_SORT;
				tvStruct.item.iImage = 1;
				tvStruct.item.iSelectedImage = 1;
				tvStruct.item.pszText = (LPSTR)(LPCTSTR)rControlType.c_str();
				tvStruct.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

				HTREEITEM hChildItem = m_wndSchematicTreeListCtrl.InsertItem(&tvStruct);
				if(hChildItem)
				{
					//m_wndSchematicTreeListCtrl.SetItemText(hChildItem, jtr->FieldValueMap[_T("C_IO_TYPE")].c_str(),1);
					//m_wndSchematicTreeListCtrl.SetItemText(hChildItem, jtr->FieldValueMap[_T("C_DESC")].c_str(), 2);
					//m_wndSchematicTreeListCtrl.SetItemText(hChildItem, jtr->FieldValueMap[_T("C_NAMING_RULE_PREFIX")].c_str(), 3);
					break;
				}
			}
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	다이얼로그 초기화

	@author KHS	

	@return		
**/
BOOL CControlCableCreationSettingDlg::Init(void)
{
	m_wndSchematicTreeListCtrl.DeleteAllItems();
	m_wndLcsLocalTypeListCtrl.DeleteAllItems();
	m_wndLcpIoTypeListCtrl.DeleteAllItems();
	m_wndDcsIoTypeListCtrl.DeleteAllItems();
	
	DisplaySchematicTypeData();

	//! COMBO BOX에 CABLE SPEC ITEM을 채운다.
	FillComboItem();
	//DisplayControlCableData();

	//DisplayControlTypeData();

	return TRUE;
}


/**
	@brief	T_CONTROL_CABLE_CREATION_SETTING 중복 체크

	@author KHS	

	@date 2009-04-29 오후 4:47:18	

	@param	

	@return		
**/
BOOL CControlCableCreationSettingDlg::CompareMatchCableType(CELOADTable::Record record)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_CABLE_MAP"));
	if(pTable)
	{
		for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin(); itr != pTable->m_RecordEntry.end();++itr)
		{
			if(record.FieldValueMap[_T("C_SCHEM_TYPE")] == itr->FieldValueMap[_T("C_SCHEM_TYPE")] && 
				record.FieldValueMap[_T("C_CABLE_TYPE")] == itr->FieldValueMap[_T("C_CABLE_TYPE")] &&
				record.FieldValueMap[_T("C_CORE")] == itr->FieldValueMap[_T("C_CORE")] &&
				record.FieldValueMap[_T("C_SIZE")] == itr->FieldValueMap[_T("C_SIZE")] &&
				record.FieldValueMap[_T("C_NAMING_RULE_PREFIX")] == itr->FieldValueMap[_T("C_NAMING_RULE_PREFIX")] &&
				record.FieldValueMap[_T("C_NAMING_RULE_SUFFIX")] == itr->FieldValueMap[_T("C_NAMING_RULE_SUFFIX")] /*&&
				record.FieldValueMap[_T("C_DESC")] == itr->FieldValueMap[_T("C_DESC")]*/
			)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-11 오전 10:56:20	

	@param	

	@return		
*/
int CControlCableCreationSettingDlg::GetControlCableRecordFrom(ControlCableRecord& record , HTREEITEM hItem)
{
	vector<string> oResult;
	// CONTROL CABLE
	const CString rControlCable = m_wndSchematicTreeListCtrl.GetItemText(hItem , 0);
	record.rPrefix = m_wndSchematicTreeListCtrl.GetItemText(hItem , 1);
	record.rSuffix = m_wndSchematicTreeListCtrl.GetItemText(hItem , 2);
	record.rDesc   = m_wndSchematicTreeListCtrl.GetItemText(hItem , 3);
	CTokenizer<CIsSpace>::Tokenize(oResult , rControlCable.operator LPCSTR() , CIsSpace());
	if(2 == oResult.size())
	{
		record.rCableType = oResult[0];
		const string rCableCoreSize = oResult[1];
		CTokenizer<CIsFromString>::Tokenize(oResult , rCableCoreSize , CIsFromString("-"));
		if(2 == oResult.size())
		{
			record.rCableCore = oResult[0];
			record.rCableSize = oResult[1];
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-11 오전 11:11:06	

	@param	

	@return		
*/
int CControlCableCreationSettingDlg::RemoveControlCableControlTypeRecord(const string& rSchemType, const ControlCableRecord& record, const string& rControlType)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pTable = docData.GetTableOf(_T("T_CONTROL_CABLE_CONTROL_TYPE_MAP"));
	if(pTable)
	{
		for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin();itr != pTable->m_RecordEntry.end();++itr)
		{
			if(rSchemType == itr->FieldValueMap[_T("C_SCHEM_TYPE")])
			{
				if(	(record.rCableType == itr->FieldValueMap[_T("C_CABLE_TYPE")]) && (record.rCableCore == itr->FieldValueMap[_T("C_CORE")]) && 
					(record.rCableSize == itr->FieldValueMap[_T("C_SIZE")]) && (record.rPrefix == itr->FieldValueMap[_T("C_NAMING_RULE_PREFIX")]) && 
					(record.rSuffix == itr->FieldValueMap[_T("C_NAMING_RULE_SUFFIX")]) && (record.rDesc == itr->FieldValueMap[_T("C_DESC")]))
				{
					if(rControlType == itr->FieldValueMap[_T("C_CONTROL_TYPE")])
					{
						pTable->m_RecordEntry.erase(itr);
						return ERROR_SUCCESS;
					}
				}
			}
		}
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-11 오전 11:24:52	

	@param	

	@return		
*/
int CControlCableCreationSettingDlg::RemoveControlCableRecord(const string& rSchemType, const ControlCableRecord& record)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	{
		CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_CABLE_MAP"));
		if(pTable)
		{
			for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin();itr != pTable->m_RecordEntry.end();++itr)
			{
				if(rSchemType == itr->FieldValueMap[_T("C_SCHEM_TYPE")])
				{
					if(	(record.rCableType == itr->FieldValueMap[_T("C_CABLE_TYPE")]) && (record.rCableCore == itr->FieldValueMap[_T("C_CORE")]) && 
						(record.rCableSize == itr->FieldValueMap[_T("C_SIZE")]) && (record.rPrefix == itr->FieldValueMap[_T("C_NAMING_RULE_PREFIX")]) && 
						(record.rSuffix == itr->FieldValueMap[_T("C_NAMING_RULE_SUFFIX")]) && (record.rDesc == itr->FieldValueMap[_T("C_DESC")]))
					{
						pTable->m_RecordEntry.erase(itr);
						break;
					}
				}
			}
		}
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	SCHEMATIC TYPE을 COPY 할때 이름 중복 체크

	@author KHS	

	@date 2009-05-13 오후 5:30:55	

	@param	

	@return		
**/
BOOL CControlCableCreationSettingDlg::IsExistItemText(const CString& rItemText)
{
	HTREEITEM hItem = m_wndSchematicTreeListCtrl.GetItemOfRow(0);
	if(hItem)
	{
		while(hItem)
		{
			CString rText = m_wndSchematicTreeListCtrl.GetItemText(hItem);
			if(rText == rItemText) return TRUE;
			hItem = m_wndSchematicTreeListCtrl.GetNextSiblingItem(hItem);
		}
	}
	
	return FALSE;
}

/**
	@brief	TREELIST 에서 SCHEMATIC TYPE을 COPY한다.

	@author BHK	

	@date 2009-05-28 오후 1:37:06	

	@param	

	@return		
*/
void CControlCableCreationSettingDlg::OnItemCopy()
{
	HTREEITEM hSelItem = m_wndSchematicTreeListCtrl.GetSelectedItem();
	if(NULL != hSelItem)
	{
		const CString rSchemTypeName = m_wndSchematicTreeListCtrl.GetItemText(hSelItem , 0);
		const CString rSchemTypeDesc = m_wndSchematicTreeListCtrl.GetItemText(hSelItem , 3);

		CSchematicTypeCopyDlg dlg;
		dlg.m_rOldSchemTypeName = rSchemTypeName;
		dlg.m_rOldSchemTypeDesc = rSchemTypeDesc;

		dlg.m_rNewSchemTypeName = rSchemTypeName;
		dlg.m_rNewSchemTypeDesc = rSchemTypeDesc;

		if(IDOK == dlg.DoModal())
		{
			const CString rCopySchematicTypeName = dlg.m_rNewSchemTypeName;
			const CString rCopySchematicTypeDesc = dlg.m_rNewSchemTypeDesc;
			if(rCopySchematicTypeName.IsEmpty())
			{
				::MessageBox(NULL, "생성할 Schematic type Name을 먼저 입력하세요!! ", "Warning!", MB_OK|MB_ICONWARNING );
				return;
			}

			//! 이름 중복 체크
			if(IsExistItemText(rCopySchematicTypeName))
			{
				::MessageBox(NULL, "The Schematic Type exists already!! ", "Warning!", MB_OK|MB_ICONWARNING );
				return;
			}

			if(hSelItem)
			{
				m_bModified = true;
				const CString rSelSchematicType = m_wndSchematicTreeListCtrl.GetItemText(hSelItem);
				const CString rSelSchematicDesc = m_wndSchematicTreeListCtrl.GetItemText(hSelItem, 3);

				CELoadDocData& docData = CELoadDocData::GetInstance();

				//! SCHEMATIC TYPE 추가
				{
					CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_TYPE"));
					if(pTable)
					{
						{
							CELOADTable::Record record;

							record.FieldValueMap[_T("C_TYPE")] = rCopySchematicTypeName.operator LPCSTR();
							record.FieldValueMap[_T("C_DESC")] = rCopySchematicTypeDesc.operator LPCSTR();

							pTable->m_RecordEntry.push_back(record);
						}

					}
				}
				{
					vector<CELOADTable::Record> SCHEMATICCONTROLMAP;
					CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_TYPE_MAP"));
					if(pTable)
					{
						for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin(); itr != pTable->m_RecordEntry.end();++itr)
						{
							if(rSelSchematicType == itr->FieldValueMap[_T("C_SCHEM_TYPE")].c_str())
							{
								CELOADTable::Record record;

								record.FieldValueMap[_T("C_SCHEM_TYPE")] = rCopySchematicTypeName.operator LPCSTR();
								record.FieldValueMap[_T("C_CONTROL_TYPE")] = itr->FieldValueMap[_T("C_CONTROL_TYPE")];

								SCHEMATICCONTROLMAP.push_back(record);
							}
						}
						for(vector<CELOADTable::Record>::iterator jtr = SCHEMATICCONTROLMAP.begin(); jtr != SCHEMATICCONTROLMAP.end();++jtr)
						{
							pTable->m_RecordEntry.push_back(*jtr);
						}
					}
				}
				{
					//! SCHEMATIC CABLE MAP 추가
					vector<CELOADTable::Record> SCHEMATICCABLEMAP;
					CELOADTable* pCableMapTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_CABLE_MAP"));
					if(pCableMapTable)
					{
						for(vector<CELOADTable::Record>::iterator itr = pCableMapTable->m_RecordEntry.begin(); itr != pCableMapTable->m_RecordEntry.end();++itr)
						{
							if(rSelSchematicType == itr->FieldValueMap[_T("C_SCHEM_TYPE")].c_str())
							{
								CELOADTable::Record record;

								record.FieldValueMap[_T("C_SCHEM_TYPE")] = rCopySchematicTypeName.operator LPCSTR();
								record.FieldValueMap[_T("C_CABLE_TYPE")] = itr->FieldValueMap[_T("C_CABLE_TYPE")];
								record.FieldValueMap[_T("C_CORE")]	 = itr->FieldValueMap[_T("C_CORE")];
								record.FieldValueMap[_T("C_SIZE")]	 = itr->FieldValueMap[_T("C_SIZE")];
								record.FieldValueMap[_T("C_NAMING_RULE_PREFIX")] = itr->FieldValueMap[_T("C_NAMING_RULE_PREFIX")];	
								record.FieldValueMap[_T("C_NAMING_RULE_SUFFIX")] =itr->FieldValueMap[_T("C_NAMING_RULE_SUFFIX")];
								record.FieldValueMap[_T("C_DESC")] =itr->FieldValueMap[_T("C_DESC")];

								SCHEMATICCABLEMAP.push_back(record);
							}
						}
						for(vector<CELOADTable::Record>::iterator jtr = SCHEMATICCABLEMAP.begin(); jtr != SCHEMATICCABLEMAP.end();++jtr)
						{
							pCableMapTable->m_RecordEntry.push_back(*jtr);
						}
					}
				}

				//! CABLE CONTROL MAP
				{
					vector<CELOADTable::Record> CABLECONTROLMAP;
					CELOADTable* pCableControlMapTable = docData.GetTableOf(_T("T_CONTROL_CABLE_CONTROL_TYPE_MAP"));
					if(pCableControlMapTable)
					{
						for(vector<CELOADTable::Record>::iterator itr = pCableControlMapTable->m_RecordEntry.begin();itr != pCableControlMapTable->m_RecordEntry.end();++itr)
						{
							//! SCHEMATIC & CONTROL MAP
							if(itr->FieldValueMap[_T("C_SCHEM_TYPE")].c_str() == rSelSchematicType)
							{
								CELOADTable::Record record;

								record.FieldValueMap[_T("C_SCHEM_TYPE")] = rCopySchematicTypeName.operator LPCSTR();
								record.FieldValueMap[_T("C_CONTROL_TYPE")] = itr->FieldValueMap[_T("C_CONTROL_TYPE")];
								record.FieldValueMap[_T("C_CABLE_TYPE")] = itr->FieldValueMap[_T("C_CABLE_TYPE")];
								record.FieldValueMap[_T("C_CORE")] = itr->FieldValueMap[_T("C_CORE")];
								record.FieldValueMap[_T("C_SIZE")]= itr->FieldValueMap[_T("C_SIZE")];
								record.FieldValueMap[_T("C_NAMING_RULE_PREFIX")]= itr->FieldValueMap[_T("C_NAMING_RULE_PREFIX")];
								record.FieldValueMap[_T("C_NAMING_RULE_SUFFIX")]= itr->FieldValueMap[_T("C_NAMING_RULE_SUFFIX")];
								record.FieldValueMap[_T("C_DESC")]= itr->FieldValueMap[_T("C_DESC")];

								CABLECONTROLMAP.push_back(record);
							}
						}
						for(vector<CELOADTable::Record>::iterator jtr = CABLECONTROLMAP.begin(); jtr != CABLECONTROLMAP.end();++jtr)
						{
							pCableControlMapTable->m_RecordEntry.push_back(*jtr);
						}
					}
				}
				HTREEITEM hItem = InsertItemIntoSchematicListCtrl(rCopySchematicTypeName, rCopySchematicTypeDesc);

				//! 해당 SCHEMATIC TYPE 아이템으로 스크롤 바 이동
				m_wndSchematicTreeListCtrl.SelectItem(hItem);

				RECT itemRect , clientRect;
				m_wndSchematicTreeListCtrl.GetClientRect(&clientRect);
				const int nRow = m_wndSchematicTreeListCtrl.GetRowOfItem(hItem);
				m_wndSchematicTreeListCtrl.ListGetItemRect(0 , 0 , &itemRect, LVIR_BOUNDS);   // 한칸의 높이를 측정
				if((nRow * (itemRect.bottom - itemRect.top)) > (clientRect.bottom - clientRect.top))
				{
					//! 선택한 Schematic Type이 화면에 보이도록 스크롤바를 이동한다.
					m_wndSchematicTreeListCtrl.Select(hItem , TVGN_FIRSTVISIBLE);
				}
				
			}
			else
			{
				::MessageBox(NULL, "Select first Schematic type!! ", "Warning!", MB_OK|MB_ICONWARNING );
				return;
			}
		}
	}
}

/**
	@brief	Schematic type delete

	@author KHS	

	@date 2009-06-09 오전 9:26:44	

	@param	

	@return		
**/
void CControlCableCreationSettingDlg::OnItemDelete()
{
	OnSchematicmenuDelete();
}
/**
	@brief	Item Delete

	@author KHS	

	@date 2009-06-05 오전 11:33:05	

	@param	

	@return		
**/
void CControlCableCreationSettingDlg::OnSchematicmenuDelete()
{
	m_bModified = true;

	HTREEITEM hSelItem = m_wndSchematicTreeListCtrl.GetSelectedItem();
	HTREEITEM hParent = m_wndSchematicTreeListCtrl.GetParentItem(hSelItem);
	HTREEITEM hChild = m_wndSchematicTreeListCtrl.GetChildItem(hSelItem);

	//! SCHEMATIC TYPE 
	if((hSelItem) && (NULL == hParent))
	{
		const string rSelSchematicType = m_wndSchematicTreeListCtrl.GetItemText(hSelItem);
		const string rSelSchematicDesc = m_wndSchematicTreeListCtrl.GetItemText(hSelItem, 3);

		CELoadDocData& docData = CELoadDocData::GetInstance();
		
		//!SChematic Type의 삭제
		while(hChild)
		{
			ControlCableRecord record;
			GetControlCableRecordFrom(record , hChild);
			HTREEITEM hGrandChild = m_wndSchematicTreeListCtrl.GetChildItem(hChild);
			//! Control Type 삭제...
			while(hGrandChild)
			{
				const string rControlType = m_wndSchematicTreeListCtrl.GetItemText(hGrandChild);
				HTREEITEM hNextGrandChild = m_wndSchematicTreeListCtrl.GetNextSiblingItem(hGrandChild);
				RemoveControlCableControlTypeRecord(rSelSchematicType , record , rControlType);
				m_wndSchematicTreeListCtrl.DeleteItem(hGrandChild);
				hGrandChild = hNextGrandChild;
			}

			//! Control Cable 삭제...
			HTREEITEM hSiblingItem = m_wndSchematicTreeListCtrl.GetNextSiblingItem(hChild);
			RemoveControlCableRecord(rSelSchematicType , record);
			m_wndSchematicTreeListCtrl.DeleteItem(hChild);
			hChild = hSiblingItem;
		}
		//!Schematic Type을 삭제
		m_wndSchematicTreeListCtrl.DeleteItem(hSelItem);
		RemoveSchematicTypeRecord(rSelSchematicType);
		//! CONTROL CABLE 과 Cable Data List를 다시 Display한다.
		DisplayControlTypeListOfSchematicType(rSelSchematicType);

		////! Load에 Assigned된 Schematic Type을 제거
		//list<CELoadItem*> LoadItemList;
		//docData.GetELoadItemListOf(LoadItemList, CLoadItem::TypeString());
		//for(list<CELoadItem*>::iterator itr = LoadItemList.begin(); itr != LoadItemList.end();++itr)
		//{
		//	const string rSchematicType = (*itr)->prop()->GetValue(_T("Control Cable"), _T("Schematic Ty"));
		//	if(rSchematicType == rSelSchematicType)
		//	{
		//		(*itr)->prop()->SetValue(_T("Control Cable"), _T("Schematic Ty"), "");
		//		(*itr)->prop()->SetOriginalValue(_T("Control Cable"), _T("Schematic Ty"), "");

		//		m_bDeleted = true;
		//	}
		//}

	}
	else if(hSelItem && (NULL != hParent) && (NULL == m_wndSchematicTreeListCtrl.GetParentItem(hParent)))
	{
		//! CONTROL CABLE 
		ControlCableRecord record;
		GetControlCableRecordFrom(record , hSelItem);
		///hParent = m_wndSchematicTreeListCtrl.GetParentItem(hParent);
		const string rSchemType = m_wndSchematicTreeListCtrl.GetItemText(hParent);

		HTREEITEM hChild = m_wndSchematicTreeListCtrl.GetChildItem(hSelItem);
		while(hChild)
		{
			HTREEITEM hNextSibling = m_wndSchematicTreeListCtrl.GetNextSiblingItem(hChild);
			const string rControlType = m_wndSchematicTreeListCtrl.GetItemText(hChild);
			RemoveControlCableControlTypeRecord(rSchemType , record , rControlType);
			m_wndSchematicTreeListCtrl.DeleteItem(hChild);

			hChild = hNextSibling;
		}

		RemoveControlCableRecord(rSchemType , record);
		m_wndSchematicTreeListCtrl.DeleteItem(hSelItem);

		DisplayControlTypeListOfSchematicType(rSchemType);
	}
	else if(hSelItem && (NULL != hParent))
	{
		//! CONTROL TYPE
		const string rControlType = m_wndSchematicTreeListCtrl.GetItemText(hSelItem);

		ControlCableRecord record;
		GetControlCableRecordFrom(record , hParent);
		hParent = m_wndSchematicTreeListCtrl.GetParentItem(hParent);
		const string rSchemType = m_wndSchematicTreeListCtrl.GetItemText(hParent);
		
		RemoveControlCableControlTypeRecord(rSchemType , record , rControlType);
		m_wndSchematicTreeListCtrl.DeleteItem(hSelItem);

		DisplayControlTypeListOfSchematicType(rSchemType);
	}
}

/**
	@brief	Control Cable Modify

	@author KHS	

	@date 2009-06-05 오전 11:33:08	

	@param	

	@return		
**/
void CControlCableCreationSettingDlg::OnSchematicmenuModify()
{
	// TODO: Add your control notification handler code here
	HTREEITEM hSelItem = m_wndSchematicTreeListCtrl.GetSelectedItem();
	HTREEITEM hParent = m_wndSchematicTreeListCtrl.GetParentItem(hSelItem);
	HTREEITEM hGrandParent = m_wndSchematicTreeListCtrl.GetParentItem(hParent);
	if((NULL == hSelItem) && (hParent) && (NULL == hGrandParent))
	{
		::MessageBox(NULL, "Control Cable Type을 선택하세요!! ", "Warning!", MB_OK|MB_ICONWARNING );
		return;
	}
	const CString rSelSchemType = m_wndSchematicTreeListCtrl.GetItemText(hParent);
	CString rCableData = m_wndSchematicTreeListCtrl.GetItemText(hSelItem, 0);
	vector<string>oResult;
	CTokenizer<CIsFromString>::Tokenize(oResult, rCableData.operator LPCSTR(), CIsFromString(" "));
	if(oResult.size() != 2) return;

	const CString rCableType = oResult[0].c_str();
	string rCableCoreSize = oResult[1];
	CTokenizer<CIsFromString>::Tokenize(oResult, rCableCoreSize, CIsFromString("-"));
	if(oResult.size() != 2 ) return;

	const CString rCableCore = oResult[0].c_str();
	const CString rCableSize = oResult[1].c_str();
	const CString rPrefix = m_wndSchematicTreeListCtrl.GetItemText(hSelItem, 1);
	const CString rSuffix =  m_wndSchematicTreeListCtrl.GetItemText(hSelItem, 2);
	const CString rDesc = m_wndSchematicTreeListCtrl.GetItemText(hSelItem, 3);
	
	CControlCableTypeInfoDlg dlg(rCableData, rPrefix, rSuffix, rDesc);
	if(IDOK == dlg.DoModal())
	{
		string rNewCableType, rNewCableCore, rNewCableSize;
		string rCableData = dlg.m_rCableType;

		CTokenizer<CIsFromString>::Tokenize(oResult, rCableData, CIsFromString(" "));
		if(2 == oResult.size())
		{
			rNewCableType = oResult[0];
			string rCableCoreSize = oResult[1];
			CTokenizer<CIsFromString>::Tokenize(oResult, rCableCoreSize, CIsFromString("-"));
			if(2 == oResult.size())
			{
				rNewCableCore = oResult[0];
				rNewCableSize = oResult[1];
			}
		}
		//! 중복 데이타 검사
		if(CompareMatchType(rSelSchemType, rNewCableType.c_str(), rNewCableCore.c_str(), rNewCableSize.c_str(), dlg.m_rPrefix, dlg.m_rSuffix))
		{
			::MessageBox(NULL, "Cable Data가 이미 존재합니다!! ", "Warning!", MB_OK|MB_ICONWARNING );
			return;
		}

		CELoadDocData& docData = CELoadDocData::GetInstance();
		CELOADTable* pSchemCableMapTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_CABLE_MAP"));
		if(pSchemCableMapTable)
		{
			for(vector<CELOADTable::Record>::iterator itr = pSchemCableMapTable->m_RecordEntry.begin(); itr != pSchemCableMapTable->m_RecordEntry.end();++itr)
			{
				if((rSelSchemType == itr->FieldValueMap[_T("C_SCHEM_TYPE")].c_str()) &&
					(rCableType == itr->FieldValueMap[_T("C_CABLE_TYPE")].c_str()) &&
					(rCableCore == itr->FieldValueMap[_T("C_CORE")].c_str()) &&
					(rCableSize == itr->FieldValueMap[_T("C_SIZE")].c_str()) &&
					(rPrefix == itr->FieldValueMap[_T("C_NAMING_RULE_PREFIX")].c_str()) &&
					(rSuffix == itr->FieldValueMap[_T("C_NAMING_RULE_SUFFIX")].c_str()) &&
					(rDesc == itr->FieldValueMap[_T("C_DESC")].c_str()))
				{
					itr->FieldValueMap[_T("C_CABLE_TYPE")] = rNewCableType;
					itr->FieldValueMap[_T("C_CORE")] = rNewCableCore;
					itr->FieldValueMap[_T("C_SIZE")] = rNewCableSize;
					itr->FieldValueMap[_T("C_NAMING_RULE_PREFIX")] = dlg.m_rPrefix.operator LPCSTR();
					itr->FieldValueMap[_T("C_NAMING_RULE_SUFFIX")] = dlg.m_rSuffix.operator LPCSTR();
					itr->FieldValueMap[_T("C_DESC")] = dlg.m_rDesc.operator LPCSTR();
				}
			}
		}

		CELOADTable* pControlCableMapTable = docData.GetTableOf(_T("T_CONTROL_CABLE_CONTROL_TYPE_MAP"));
		if(pControlCableMapTable)
		{
			for(vector<CELOADTable::Record>::iterator itr = pControlCableMapTable->m_RecordEntry.begin(); itr != pControlCableMapTable->m_RecordEntry.end();++itr)
			{
				if((rSelSchemType == itr->FieldValueMap[_T("C_SCHEM_TYPE")].c_str()) &&
					(rCableType == itr->FieldValueMap[_T("C_CABLE_TYPE")].c_str()) &&
					(rCableCore == itr->FieldValueMap[_T("C_CORE")].c_str()) &&
					(rCableSize == itr->FieldValueMap[_T("C_SIZE")].c_str()) &&
					(rPrefix == itr->FieldValueMap[_T("C_NAMING_RULE_PREFIX")].c_str()) &&
					(rSuffix == itr->FieldValueMap[_T("C_NAMING_RULE_SUFFIX")].c_str()) &&
					(rDesc == itr->FieldValueMap[_T("C_DESC")].c_str()))
				{
					itr->FieldValueMap[_T("C_CABLE_TYPE")] = rNewCableType;
					itr->FieldValueMap[_T("C_CORE")] = rNewCableCore;
					itr->FieldValueMap[_T("C_SIZE")] = rNewCableSize;
					itr->FieldValueMap[_T("C_NAMING_RULE_PREFIX")] = dlg.m_rPrefix.operator LPCSTR();
					itr->FieldValueMap[_T("C_NAMING_RULE_SUFFIX")] = dlg.m_rSuffix.operator LPCSTR();
					itr->FieldValueMap[_T("C_DESC")] = dlg.m_rDesc.operator LPCSTR();	
				}
			}
		}

		m_wndSchematicTreeListCtrl.SetItemText(hSelItem , dlg.m_rCableType , 0);
		m_wndSchematicTreeListCtrl.SetItemText(hSelItem , dlg.m_rPrefix , 1);
		m_wndSchematicTreeListCtrl.SetItemText(hSelItem , dlg.m_rSuffix , 2);
		m_wndSchematicTreeListCtrl.SetItemText(hSelItem , dlg.m_rDesc , 3);

		m_bModified = true;
	}
}

/**
	@brief	Control Type Delete

	@author KHS	

	@date 2009-06-05 오전 11:35:57	

	@param	

	@return		
**/
void CControlCableCreationSettingDlg::OnControltypemenuDelete()
{
	OnSchematicmenuDelete();
}
/**
	@brief	LCP IO TYPE LIST CTRL BEGIN DRAG

	@author KHS	

	@date 2009-06-08 오후 2:03:52	

	@param	

	@return		
**/
void CControlCableCreationSettingDlg::OnLvnBegindragControlTypeList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if(NULL == pNMLV) return;

	OnBeginDrag(pNMLV , m_wndLcpIoTypeListCtrl);

	*pResult = 0;
}
/**
	@brief	DCS IO TYPE LIST CTRL BEGIN DRAG

	@author KHS	

	@date 2009-06-08 오후 2:03:49	

	@param	

	@return		
**/
void CControlCableCreationSettingDlg::OnLvnBegindragControlTypeList3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if(NULL == pNMLV) return;

	OnBeginDrag(pNMLV , m_wndDcsIoTypeListCtrl);

	*pResult = 0;
}

/**
	@brief	Schematic Type의 모든 I/O Type이 할당되었는지 검사한다.

	@author BHK	

	@date 2009-06-09 오전 10:42:11	

	@param	

	@return		
*/
void CControlCableCreationSettingDlg::OnBnClickedButtonCheck()
{
	CheckControlCableControlTypeMap();
}

/**
	@brief	Schematic Type Record를 삭제

	@author KHS	

	@date 2009-06-09 오전 9:32:57	

	@param	

	@return		
**/
int CControlCableCreationSettingDlg::RemoveSchematicTypeRecord(const string& rSchematicType)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	{
		CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_TYPE"));
		if(pTable)
		{
			for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin(); itr != pTable->m_RecordEntry.end();++itr)
			{
				if(rSchematicType == itr->FieldValueMap[_T("C_TYPE")])
				{
					pTable->m_RecordEntry.erase(itr);
					break;
				}
			}
		}
	}
	{
		CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_CABLE_MAP"));
		if(pTable)
		{
			for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin(); itr != pTable->m_RecordEntry.end();)
			{
				if(rSchematicType == itr->FieldValueMap[_T("C_SCHEM_TYPE")])
				{
					itr = pTable->m_RecordEntry.erase(itr);
				}
				else
				{
					++itr;
				}
			}
		}
	}
	{
		CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_TYPE_MAP"));
		if(pTable)
		{
			for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin(); itr != pTable->m_RecordEntry.end();)
			{
				if(rSchematicType == itr->FieldValueMap[_T("C_SCHEM_TYPE")])
				{
					itr = pTable->m_RecordEntry.erase(itr);
				}
				else
				{
					++itr;
				}
			}
			return ERROR_SUCCESS;
		}
	}
	return ERROR_BAD_ENVIRONMENT;
}


static UINT gfnPrintControlCableSetting(LPVOID pVoid)
{
	CSimpleAnimationDlg& anidlg = CSimpleAnimationDlg::GetInstance();
	if(NULL == anidlg.GetSafeHwnd())
        {
                anidlg.DoModal();
        }

	return ERROR_SUCCESS;
}


/**
	@brief	Control cable Creation Setting 을 Excel로 출력한다.

	@author KHS	

	@date 2009-06-17 오후 4:49:23	

	@param	

	@return	
**/
void CControlCableCreationSettingDlg::OnBnClickedButtonExport()
{
	CWinThread* pThread = AfxBeginThread(gfnPrintControlCableSetting , NULL , THREAD_PRIORITY_NORMAL,0);
	
	WriteControlCableSettingToTxtFile();
	python.CallPyWriteControlCableSettingToExcel();
	
	if(NULL != pThread)
	{
		CSimpleAnimationDlg& anidlg = CSimpleAnimationDlg::GetInstance();
		if(anidlg.GetSafeHwnd()) anidlg.SendMessage(WM_COMMAND , IDOK);
	}
}

/**
	@brief	Control cable Creation Setting 을 TEXT로 출력한다.

	@author KHS	

	@date 2009-06-17 오후 5:15:03	

	@param	

	@return		
**/
int CControlCableCreationSettingDlg::WriteControlCableSettingToTxtFile(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string rInterestingFilePath = docData.GetProjectFolderPath() + _T("Working\\CONTROL_CABLE_SETTING.TXT");
	ofstream ofile(rInterestingFilePath.c_str());
	if(ofile.is_open())
	{
		//!SCHEMATIC TYPE
		HTREEITEM hItem = m_wndSchematicTreeListCtrl.GetItemOfRow(0);
		if(hItem)
		{
			do
			{
				CString rSchematicType = m_wndSchematicTreeListCtrl.GetItemText(hItem);
				CString rSchematicDesc = m_wndSchematicTreeListCtrl.GetItemText(hItem, 3);

				ofile << rSchematicType + "@\t";

				//!CONTROL CABLE
				HTREEITEM hCableItem = m_wndSchematicTreeListCtrl.GetChildItem(hItem);
				if(hCableItem)
				{
					int nCableIndex = 1;
					int nControlIndex = 0;
					do
					{
						CString rCableData =  m_wndSchematicTreeListCtrl.GetItemText(hCableItem);
						vector<string> oResult;
						CString rCableType, rCableCoreSize;
						CTokenizer<CIsFromString>::Tokenize(oResult, rCableData.operator LPCSTR(), CIsFromString(" "));
						if((!oResult.empty()) && (oResult.size() == 2))
						{
							rCableType = oResult[0].c_str();
							rCableCoreSize = oResult[1].c_str();
						}
						CString rPrefix = m_wndSchematicTreeListCtrl.GetItemText(hCableItem, 1);
						CString rSuffix  = m_wndSchematicTreeListCtrl.GetItemText(hCableItem, 2);
						CString rCableDesc = m_wndSchematicTreeListCtrl.GetItemText(hCableItem, 3);

						//! CONTROL TYPE
						HTREEITEM hControlTypeItem = m_wndSchematicTreeListCtrl.GetChildItem(hCableItem);

						if(1 == nCableIndex)
						{
							ofile << rCableType + "\t" << rCableCoreSize + "\t" << rPrefix + "\t" << rSuffix + "\t" << rCableDesc + "\t"  << endl; 
						}
						else if(2 == nCableIndex && 0 == nControlIndex)
						{
							ofile << rSchematicDesc + "@\t" << rCableType + "\t" << rCableCoreSize + "\t" << rPrefix + "\t" << rSuffix + "\t" << rCableDesc + "\t" << endl; 
						}
						else
						{
							ofile << "\t" << rCableType + "\t" << rCableCoreSize + "\t" << rPrefix + "\t" << rSuffix + "\t" << rCableDesc + "\t" << endl; 
						}
						++nCableIndex;

						if(hControlTypeItem)
						{
							do
							{
								++nControlIndex;

								CString rControlType = m_wndSchematicTreeListCtrl.GetItemText(hControlTypeItem);
								CString rControlDesc = m_wndSchematicTreeListCtrl.GetItemText(hControlTypeItem, 3);
								
								if(1 == nControlIndex &&  2 == nCableIndex )
								{
									ofile << rSchematicDesc + "@\t\t" << rControlType+ "\t" << "\t" << rControlDesc + "\t" << endl;
								}
								else
								{
									ofile << "\t\t" << rControlType+ "\t" << "\t" << rControlDesc + "\t" << endl;
								}

							}while((hControlTypeItem = m_wndSchematicTreeListCtrl.GetNextSiblingItem(hControlTypeItem)) != NULL);
						}
						
					}while((hCableItem = m_wndSchematicTreeListCtrl.GetNextSiblingItem(hCableItem)) != NULL);
				}
				else 
				{
					//!Cable이 없을때
					ofile << endl << rSchematicDesc + "@\t"<< endl;

				}

				ofile << endl;

			}while((hItem = m_wndSchematicTreeListCtrl.GetNextSiblingItem(hItem)) != NULL);
		}
		ofile.close();

		return ERROR_SUCCESS;
	}
	return ERROR_BAD_ENVIRONMENT;
}
