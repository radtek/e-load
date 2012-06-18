// SchematicTypeCreationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "SchematicTypeCreationDlg.h"
#include "ELoadDocData.h"
#include <util/ado/ADODB.h>
#include "GridCellEx.h"
#include "FillGridCellComboBox.h"
#include "Tables/SchematicControlTypeMapTable.h"
#include "Tables/ControlTypeTable.h"
#include "SchematicTypeInfoDlg.h"

using namespace SchematicTypeCreation;
/////////////////////////////////////////////////////////////////////////////
// CGridCellComboEx 
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
	CControlTypeTable* pControlTable = static_cast<CControlTypeTable*>(docData.GetTableOf(_T("T_CONTROL_TYPE")));
	if(pControlTable)
	{
		pControlTable->GetControlTypeList(m_Strings);
		for(int i = m_Strings.GetCount() - 1;i >= 0;--i)
		{
			vector<string>::iterator where = find(ExceptItemList.begin() , ExceptItemList.end() , m_Strings.GetAt(i).operator LPCSTR());
			if(where != ExceptItemList.end()) m_Strings.RemoveAt(i);
		}

		/*
		m_Strings.RemoveAll();
		for(vector<CELOADTable::Record>::iterator itr = pControlTable->m_RecordEntry.begin(); itr != pControlTable->m_RecordEntry.end();++itr)
		{
			const string rType = itr->FieldValueMap[_T("C_TYPE")].c_str();
			vector<string>::iterator where = find(ExceptItemList.begin() , ExceptItemList.end() , rType);
			if(where == ExceptItemList.end()) m_Strings.Add(rType.c_str());
		}
		*/
	}

	CGridCellCombo::Edit(nRow , nCol , rect , point , nID , nChar);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// CSchematicTypeCreationDlg dialog

IMPLEMENT_DYNAMIC(CSchematicTypeCreationDlg, CWizardPropertyPage)

CSchematicTypeCreationDlg::CSchematicTypeCreationDlg(const string& rInitSchematicType , CWnd* pParent /*=NULL*/)
	: CWizardPropertyPage(CSchematicTypeCreationDlg::IDD /*pParent*/) , m_rCurSchematicType(_T(""))
	, m_bDeleted(false)
{
	m_bModified = false;
	m_pSchematicTypeTableCopy = NULL;
	m_pSchematicControlTypeMapTableCopy = NULL;
	m_pControlCableTypeMapTableCopy = NULL;
	m_pSchemControlCableMapTableCopy     = NULL;

	m_bDragging = FALSE;
	m_rInitSchematicType = rInitSchematicType;
}

CSchematicTypeCreationDlg::~CSchematicTypeCreationDlg()
{
	try
	{
		if(m_pSchematicTypeTableCopy) SAFE_DELETE(m_pSchematicTypeTableCopy);
		if(m_pSchematicControlTypeMapTableCopy) SAFE_DELETE(m_pSchematicControlTypeMapTableCopy);
		if(m_pControlCableTypeMapTableCopy) SAFE_DELETE(m_pControlCableTypeMapTableCopy);
		if(m_pSchemControlCableMapTableCopy) SAFE_DELETE(m_pSchemControlCableMapTableCopy);
	}
	catch(...)
	{
	}
}

void CSchematicTypeCreationDlg::DoDataExchange(CDataExchange* pDX)
{
	CWizardPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, ID_SETTING_SAVE, m_wndSettingSaveButton);
	DDX_Control(pDX, IDCANCEL, m_wndCancelButton);

	DDX_Control(pDX, ID_WIZBACK, m_wndBackButton);
	DDX_Control(pDX, ID_WIZNEXT, m_wndNextButton);

	DDX_Control(pDX, IDC_SCHEMATIC_ADD_BUTTON, m_wndSchematicAddButton);
	DDX_Control(pDX, IDC_SCHEMATIC_DELETE_BUTTON, m_wndSchematicDeleteButton);
	DDX_Control(pDX, IDC_SCHEMATIC_MODIFY_BUTTON, m_wndSchematicModifyButton);

	DDX_Control(pDX, IDC_CONTROL_TYPE_LIST, m_wndLcsLocalTypeListCtrl);
	DDX_Control(pDX, IDC_SCHEMATIC_TYPE_EDIT, m_wndSchematicTypeEdit);
	DDX_Control(pDX, IDC_SCHEMATIC_DESC_EDIT, m_wndSchematicDescEdit);
	DDX_Control(pDX, IDC_CONTROL_TYPE_LIST2, m_wndLcpIoTypeListCtrl);
	DDX_Control(pDX, IDC_CONTROL_TYPE_LIST3, m_wndDcsIoTypeListCtrl);
}


BEGIN_MESSAGE_MAP(CSchematicTypeCreationDlg, CDialog)
	ON_BN_CLICKED(ID_SETTING_SAVE, &CSchematicTypeCreationDlg::OnBnClickedSettingSave)
	ON_BN_CLICKED(IDCANCEL, &CSchematicTypeCreationDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_SCHEMATIC_ADD_BUTTON, &CSchematicTypeCreationDlg::OnBnClickedSchematicAddButton)
	ON_BN_CLICKED(ID_WIZBACK, &CSchematicTypeCreationDlg::OnBnClickedWizback)
	ON_BN_CLICKED(ID_WIZNEXT, &CSchematicTypeCreationDlg::OnBnClickedWiznext)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY(LVN_BEGINDRAG, IDC_CONTROL_TYPE_LIST, &CSchematicTypeCreationDlg::OnLvnBegindragControlTypeList)
	
	ON_COMMAND(ID_ITEM_DELETE, &CSchematicTypeCreationDlg::OnItemDelete)
	ON_COMMAND(ID_ITEM_MODIFY, &CSchematicTypeCreationDlg::OnItemModify)
	ON_COMMAND(ID_CONTROLTYPEMENU_DELETE, &CSchematicTypeCreationDlg::OnControltypemenuDelete)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_CONTROL_TYPE_LIST2, &CSchematicTypeCreationDlg::OnLvnBegindragControlTypeList2)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_CONTROL_TYPE_LIST3, &CSchematicTypeCreationDlg::OnLvnBegindragControlTypeList3)
END_MESSAGE_MAP()


// CSchematicTypeCreationDlg message handlers
/**
	@brief	다이얼로그 초기화

	@author BHK	

	@date 2009-04-21 오전 9:41:04	

	@param	

	@return		
*/
BOOL CSchematicTypeCreationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_hIcon = AfxGetApp()->LoadIcon(IDI_GRABBER);

	CELOADApp* pApp = (CELOADApp*)AfxGetApp();

	//! CANCEL 명령어를 위해 복사본을 저장해 둔다.
	CELoadDocData & docData = CELoadDocData::GetInstance();
	{
		CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_TYPE"));
		if(pTable) m_pSchematicTypeTableCopy = pTable->Clone();
	}
	{
		CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_TYPE_MAP"));
		if(pTable) m_pSchematicControlTypeMapTableCopy = pTable->Clone();
	}
	{
		CELOADTable* pTable = docData.GetTableOf(_T("T_CONTROL_CABLE_CONTROL_TYPE_MAP"));
		if(pTable) m_pControlCableTypeMapTableCopy = pTable->Clone();
	}
	{
		CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_CABLE_MAP"));
		if(pTable) m_pSchemControlCableMapTableCopy = pTable->Clone();
	}

	//! Schematic type Tree List 해더생성
	if(ERROR_SUCCESS != InitSchematicTreeList())
	{
		AfxMessageBox("Fail Create TreeList Ctrl...");
		return FALSE;
	}

	m_wndSchematicTreeListCtrl.SetImageList(&(pApp->m_ELoadImages), LVSIL_NORMAL);
	m_wndLcsLocalTypeListCtrl.SetImageList(&(pApp->m_ELoadImages), LVSIL_SMALL);
	m_wndLcpIoTypeListCtrl.SetImageList(&(pApp->m_ELoadImages), LVSIL_SMALL);
	m_wndDcsIoTypeListCtrl.SetImageList(&(pApp->m_ELoadImages), LVSIL_SMALL);

	//! control type List 해더생성
	InitControlTypeList();

	//! BUTTON ICON SETTING
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));

	m_wndSettingSaveButton.SetTextHotColor(TextHotColor);
	m_wndCancelButton.SetTextHotColor(TextHotColor);
	m_wndSchematicAddButton.SetTextHotColor(TextHotColor);
	m_wndSchematicDeleteButton.SetTextHotColor(TextHotColor);
	
	m_wndSchematicAddButton.SetIcon(IDI_ADD);
	m_wndSchematicDeleteButton.SetIcon(IDI_DEL);

	//! BUTTON SETTING
	m_wndBackButton.SetTextHotColor(TextHotColor);
	m_wndNextButton.SetTextHotColor(TextHotColor);
	m_wndBackButton.SetIcon(IDI_PRE);
	m_wndNextButton.SetIcon(IDI_NEXT);

	m_wndSettingSaveButton.SetIcon(IDI_OK_18X18);
	m_wndCancelButton.SetIcon(IDI_CANCEL_18X18);

	m_hIcon = AfxGetApp()->LoadIcon(IDI_GRABBER);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	

	@author BHK	

	@date 2009-05-13 오후 8:15:32	

	@param	

	@return		
*/
void CSchematicTypeCreationDlg::OnBnClickedSettingSave()
{
	m_bModified = false;
	if(ERROR_SUCCESS == SaveData())
	{
		AfxMessageBox("SAVED");
	}
}

/**
	@brief	수정하기 전 상태로 되돌린다.

	@author BHK	

	@date 2009-04-24 오전 11:05:05	

	@param	

	@return		
*/
void CSchematicTypeCreationDlg::OnBnClickedCancel()
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
				if(m_pSchematicTypeTableCopy && pTable) pTable->Copy(m_pSchematicTypeTableCopy);
			}
			{
				CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_TYPE_MAP"));
				if(m_pSchematicControlTypeMapTableCopy && pTable) pTable->Copy(m_pSchematicControlTypeMapTableCopy);
			}
			{
				CELOADTable* pTable = docData.GetTableOf(_T("T_CONTROL_CABLE_CONTROL_TYPE_MAP"));
				if(m_pControlCableTypeMapTableCopy && pTable) pTable->Copy(m_pControlCableTypeMapTableCopy);
			}
			{
				CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_CABLE_MAP"));
				if(m_pSchemControlCableMapTableCopy && pTable) pTable->Copy(m_pSchemControlCableMapTableCopy);
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
	@brief	

	@author BHK	

	@date 2009-04-15 오후 4:22:47	

	@param	

	@return		
*/
BOOL CSchematicTypeCreationDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
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

				menu.AppendMenu(0 , ID_ITEM_MODIFY , _T("Modify"));
				menu.AppendMenu(0 , ID_ITEM_DELETE , _T("Delete"));
				menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, point.y, this);
			}
		}
		else if((WM_RBUTTONUP == ptlistinfo->code) && ptlistinfo->hItem && (NULL != m_wndSchematicTreeListCtrl.GetParentItem(ptlistinfo->hItem)))
		{
			//! this is control type tree item.
			const CString rItemText = m_wndSchematicTreeListCtrl.GetItemText(ptlistinfo->hItem , 0);
			CMenu menu;
			if(menu.CreatePopupMenu())
			{
				CPoint point;
				GetCursorPos(&point);
				///ScreenToClient(&point);

				menu.AppendMenu(0 , ID_CONTROLTYPEMENU_DELETE , _T("Delete"));
				menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, point.y, this);
			}
		}
	}

	return CWizardPropertyPage::OnNotify(wParam, lParam, pResult);
}


/**
	@brief	ControlType GridCtrl에 아이템을 보여준다.

	@author KHS	

	@return		
**/
int CSchematicTypeCreationDlg::DisplayControlTypeData()
{
	m_wndLcsLocalTypeListCtrl.DeleteAllItems();
	m_wndLcpIoTypeListCtrl.DeleteAllItems();
	m_wndDcsIoTypeListCtrl.DeleteAllItems();

	CELoadDocData& docData = CELoadDocData::GetInstance();
	CControlTypeTable* pTable = static_cast<CControlTypeTable*>(docData.GetTableOf(_T("T_CONTROL_TYPE")));
	if(pTable)
	{
		for(vector<CELOADTable::Record>::iterator jtr = pTable->m_RecordEntry.begin();jtr != pTable->m_RecordEntry.end();++jtr)
		{
			if(_T("YES") == jtr->FieldValueMap[_T("C_CHECK")])
			{
				const string rType = jtr->FieldValueMap[_T("C_TYPE")];
				const string rIoType = jtr->FieldValueMap[_T("C_IO_TYPE")];
				const string rDesc = jtr->FieldValueMap[_T("C_DESC")];
				const string rIoTag = jtr->FieldValueMap[_T("C_NAMING_RULE_PREFIX")];
				const string rCategory = jtr->FieldValueMap[_T("C_CATEGORY")];

				InsertItemIntoControlTypeListCtrl(rType, rIoType, rDesc, rIoTag, rCategory);
			}
		}
		return ERROR_SUCCESS;
	}
	
	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author BHK	

	@date 2009-06-26 오전 10:46:59	

	@param	

	@return		
*/
BOOL CSchematicTypeCreationDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE) return TRUE;
		if(pMsg->wParam == VK_RETURN)
		{
			if(pMsg->hwnd == m_wndSchematicDescEdit.GetSafeHwnd())
			{
				OnBnClickedSchematicAddButton();
			}
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

/**
	@brief	ADD

	@author KHS	

	@return		
**/
void CSchematicTypeCreationDlg::OnBnClickedSchematicAddButton()
{
	m_bModified = true;
	CString rSchematicType, rSchematicDesc;

	m_wndSchematicTypeEdit.GetWindowText(rSchematicType);
	m_wndSchematicDescEdit.GetWindowText(rSchematicDesc);
	
	if(rSchematicType.IsEmpty())
	{
		::MessageBox(NULL, "Schematic Type을 입력하세요!! ", "Warning!", MB_OK|MB_ICONWARNING );
		return;
	}

	if(IsExistItemText(rSchematicType))
	{
		::MessageBox(NULL, "The Schematic Type exists already!! ", "Warning!", MB_OK|MB_ICONWARNING );
		return;
	}
	HTREEITEM hItem = MakeRootItemInTreeList(rSchematicType, rSchematicDesc);
	if(hItem)
	{
		//! 레코드 추가
		CELoadDocData& docData = CELoadDocData::GetInstance();
		CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_TYPE"));
		if(pTable)
		{
			CELOADTable::Record record;

			record.FieldValueMap[_T("C_TYPE")] = rSchematicType.operator LPCSTR();
			record.FieldValueMap[_T("C_DESC")] = rSchematicDesc.operator LPCSTR();

			pTable->m_RecordEntry.push_back(record);
			
		}
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
	m_wndSchematicTypeEdit.SetWindowText("");
	m_wndSchematicDescEdit.SetWindowText("");
}

/**
	@brief	DELETE

	@author KHS	

	@return		
**/
void CSchematicTypeCreationDlg::OnItemDelete()
{
	m_bModified = true;

	CELoadDocData& docData = CELoadDocData::GetInstance();

	HTREEITEM hItem = m_wndSchematicTreeListCtrl.GetSelectedItem();
	if(hItem)
	{
		HTREEITEM hParent = m_wndSchematicTreeListCtrl.GetParentItem(hItem);
		if(NULL == hParent)
		{
			const CString rCurSchematicType = m_wndSchematicTreeListCtrl.GetItemText(hItem);
			CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_TYPE"));
			if(pTable)
			{
				{
					for(map<int,string>::iterator itr = m_SchematicTypeMap.begin();itr != m_SchematicTypeMap.end();++itr)
					{
						if(rCurSchematicType == itr->second.c_str())
						{
							m_SchematicTypeMap.erase(itr);
							break;
						}
					}
				}

				{
					for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin(); itr != pTable->m_RecordEntry.end();++itr)
					{
						const string rType = itr->FieldValueMap[_T("C_TYPE")];
						if(rCurSchematicType == rType.c_str())
						{
							pTable->m_RecordEntry.erase(itr);
							break;
						}
					}
				}
				//! T_SCHEMATIC_CONTROL_TYPE_MAP 테이블의 내용을 삭제한다.
				CELOADTable* pSchemControlMapTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_TYPE_MAP"));
				if(pSchemControlMapTable)
				{
					for(vector<CELOADTable::Record>::iterator itr = pSchemControlMapTable->m_RecordEntry.begin(); itr != pSchemControlMapTable->m_RecordEntry.end();)
					{
						const string rType = itr->FieldValueMap[_T("C_SCHEM_TYPE")];
						if(rCurSchematicType == rType.c_str())
						{
							itr = pSchemControlMapTable->m_RecordEntry.erase(itr);
						}
						else
						{
							++itr;
						}
					}
				}

				CELOADTable* pSchemCableMapTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_CABLE_MAP"));
				if(pSchemCableMapTable)
				{
					for(vector<CELOADTable::Record>::iterator itr = pSchemCableMapTable->m_RecordEntry.begin(); itr != pSchemCableMapTable->m_RecordEntry.end();)
					{
						const string rType = itr->FieldValueMap[_T("C_SCHEM_TYPE")];
						if(rCurSchematicType == rType.c_str())
						{
							itr = pSchemCableMapTable->m_RecordEntry.erase(itr);
						}
						else
						{
							++itr;
						}
					}
				}

				CELOADTable* pControlCableMapTable = docData.GetTableOf(_T("T_CONTROL_CABLE_CONTROL_TYPE_MAP"));
				if(pControlCableMapTable)
				{
					for(vector<CELOADTable::Record>::iterator itr = pControlCableMapTable->m_RecordEntry.begin(); itr != pControlCableMapTable->m_RecordEntry.end();)
					{
						const string rType = itr->FieldValueMap[_T("C_SCHEM_TYPE")];
						if(rCurSchematicType == rType.c_str())
						{
							itr = pControlCableMapTable->m_RecordEntry.erase(itr);
						}
						else
						{
							++itr;
						}
					}
				}

				//! Load에 Assigned된 Schematic Type을 제거
				/*list<CELoadItem*> LoadItemList;
				docData.GetELoadItemListOf(LoadItemList, CLoadItem::TypeString());
				for(list<CELoadItem*>::iterator itr = LoadItemList.begin(); itr != LoadItemList.end();++itr)
				{
					const string rSchematicType = (*itr)->prop()->GetValue(_T("Control Cable"), _T("Schematic Ty"));
					if(rSchematicType.c_str() == rCurSchematicType)
					{
						(*itr)->prop()->SetValue(_T("Control Cable"), _T("Schematic Ty"), "");
						(*itr)->prop()->SetOriginalValue(_T("Control Cable"), _T("Schematic Ty"), "");

						m_bDeleted = true;
					}
				}*/
			}
		}
		else
		{
			const CString rCurControlType = m_wndSchematicTreeListCtrl.GetItemText(hItem);
			const CString rCurSchematicType = m_wndSchematicTreeListCtrl.GetItemText(hParent);
			//! T_SCHEMATIC_CONTROL_TYPE_MAP 테이블의 내용을 삭제한다.
			CELOADTable* pMapTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_TYPE_MAP"));
			if(pMapTable)
			{
				for(vector<CELOADTable::Record>::iterator itr = pMapTable->m_RecordEntry.begin(); itr != pMapTable->m_RecordEntry.end();)
				{
					const string rCType = itr->FieldValueMap[_T("C_CONTROL_TYPE")];
					const string rSType = itr->FieldValueMap[_T("C_SCHEM_TYPE")];
					if(rCurControlType == rCType.c_str() && rCurSchematicType == rSType.c_str())
					{
						itr = pMapTable->m_RecordEntry.erase(itr);
					}
					else
					{
						++itr;
					}
				}
			}

			CELOADTable* pControlCableMapTable = docData.GetTableOf(_T("T_CONTROL_CABLE_CONTROL_TYPE_MAP"));
			if(pControlCableMapTable)
			{
				for(vector<CELOADTable::Record>::iterator itr = pControlCableMapTable->m_RecordEntry.begin(); itr != pControlCableMapTable->m_RecordEntry.end();)
				{
					const string rControlType = itr->FieldValueMap[_T("C_CONTROL_TYPE")];
					const string rSchematicType = itr->FieldValueMap[_T("C_SCHEM_TYPE")];
					if(rCurControlType == rControlType.c_str() && rCurSchematicType == rSchematicType.c_str())
					{
						itr = pControlCableMapTable->m_RecordEntry.erase(itr);
					}
					else
					{
						++itr;
					}
				}
			}
		}
		m_wndSchematicTreeListCtrl.DeleteItem(hItem);
	}
}


/**
	@brief	DB 데이타를 읽어서 화면에 출력한다.

	@author KHS	

	@return		
**/
int CSchematicTypeCreationDlg::DisplaySchematicTypeData(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_TYPE"));
	if(pTable)
	{
		//m_SchematicTypeMap.clear();
		for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin();itr != pTable->m_RecordEntry.end();++itr)
		{
			const CString rType = itr->FieldValueMap[_T("C_TYPE")].c_str();
			const CString rDesc = itr->FieldValueMap[_T("C_DESC")].c_str();

			MakeRootItemInTreeList(rType, rDesc);
			//m_SchematicTypeMap.insert(make_pair(m_wndSchematicTypeGridCtrl.GetRowCount() - 1,rType));
		}
		
		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	화면상의 데이타를 DB에 저장한다.

	@author KHS	

	@return		
**/
int CSchematicTypeCreationDlg::SaveData(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	{
		CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_TYPE"));
		if(pTable)
		{
			pTable->Save();
			if(m_pSchematicTypeTableCopy) m_pSchematicTypeTableCopy->Copy(pTable);
		}
	}
	{
		CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_TYPE_MAP"));
		if(pTable)
		{
			pTable->Save();
			if(m_pSchematicControlTypeMapTableCopy) m_pSchematicControlTypeMapTableCopy->Copy(pTable);
		}
	}

	{
		CELOADTable* pTable = docData.GetTableOf(_T("T_CONTROL_CABLE_CONTROL_TYPE_MAP"));
		if(pTable)
		{
			pTable->Save();
			if(m_pControlCableTypeMapTableCopy) m_pControlCableTypeMapTableCopy->Copy(pTable);
		}
	}
	{
		CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_CABLE_MAP"));
		if(pTable)
		{
			pTable->Save();
			if(m_pSchemControlCableMapTableCopy) m_pSchemControlCableMapTableCopy->Copy(pTable);
		}
	}
	/*if(m_bDeleted)
	{
		return docData.Save(CLoadItem::TableName(), CLoadItem::TypeString(),  AfxGetMainWnd());
		m_bDeleted = false;
	}*/
	return ERROR_SUCCESS;
	
}


/**
	@brief	SELECTED OF GRID CTRL COMBOBOX

	@author KHS	

	@return		
**/
int CSchematicTypeCreationDlg::ShowSelectControlTypeItem(const int& nRow)
{
	return ERROR_SUCCESS;
}

/**
	@brief	Schematic Type을 변경했을때, Schematic Type에 대한 설정한 Control Type 데이터를 저장한다.

	@author BHK	

	@date 2009-04-21 오전 10:03:26	

	@param	

	@return		
*/
int CSchematicTypeCreationDlg::OnChangeSchematicType(void)
{
	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-22 오후 3:19:17	

	@param	

	@return		
*/
void CSchematicTypeCreationDlg::OnBnClickedWizback()
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
void CSchematicTypeCreationDlg::OnBnClickedWiznext()
{
	if(m_bModified)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		if(IDYES == AfxMessageBox(docData.GetStringOf(49).c_str() , MB_YESNO|MB_ICONWARNING ))
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

	@author KHS	

	@return		
**/
int CSchematicTypeCreationDlg::InitSchematicTreeList(void)
{
	static const CString __HEADERS__[] = {_T("SCHEM TYPE") , _T("TYPE DESC")};

	CRect rect;
	CPoint point(0,0);
	GetDlgItem(IDC_MAIN_SCHEMATIC_STATIC)->GetWindowRect(&rect);
	ScreenToClient(&rect);

	unsigned uStyle	= TVS_HASBUTTONS|TVS_HASLINES/*|TVS_FULLROWSELECT|TVS_NONEVENHEIGHT|TVS_CHECKBOXES*/;
	unsigned uExStyle = TVS_EX_SUBSELECT|TVS_EX_FULLROWMARK;

	if(m_wndSchematicTreeListCtrl.Create(uStyle|WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL,rect,this, 123))
	{
		m_wndSchematicTreeListCtrl.InsertColumn(0, __HEADERS__[0], LVCFMT_LEFT, 170);
		m_wndSchematicTreeListCtrl.InsertColumn(1, __HEADERS__[1], LVCFMT_LEFT, 190);
		m_wndSchematicTreeListCtrl.SetExtendedStyle(uExStyle);

		return ERROR_SUCCESS;
	}
	

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	SCHEMATIC TREE LIST CTRL에 루트 아이템을 만든다. (SCHEMATIC ITEM)

	@author KHS	

	@return		
**/
HTREEITEM CSchematicTypeCreationDlg::MakeRootItemInTreeList(const CString& rType, const CString& rDesc)
{
	TV_INSERTSTRUCT tvStruct;

	tvStruct.hParent = NULL;
	tvStruct.hInsertAfter = TVI_SORT;
	tvStruct.item.iImage = 0;
	tvStruct.item.iSelectedImage = 0;
	tvStruct.item.pszText = (LPSTR)(LPCTSTR)rType;
	tvStruct.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

	HTREEITEM hParent = m_wndSchematicTreeListCtrl.InsertItem(&tvStruct);
	if(hParent)
	{
		m_wndSchematicTreeListCtrl.SetItemText(hParent, rDesc, 1);

		CELoadDocData& docData = CELoadDocData::GetInstance();
		CELOADTable* pMapTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_TYPE_MAP"));
		if(pMapTable)
		{
			for(vector<CELOADTable::Record>::iterator itr = pMapTable->m_RecordEntry.begin(); itr != pMapTable->m_RecordEntry.end();++itr)
			{
				const string rSchemType = itr->FieldValueMap[_T("C_SCHEM_TYPE")];
				const string rControlType = itr->FieldValueMap[_T("C_CONTROL_TYPE")];
				if(rSchemType.c_str() == rType)
				{
					tvStruct.hParent = hParent;
					tvStruct.hInsertAfter = TVI_SORT;
					tvStruct.item.iImage = 1;
					tvStruct.item.iSelectedImage = 1;
					tvStruct.item.pszText = (LPSTR)(LPCTSTR)rControlType.c_str();
					tvStruct.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
					m_wndSchematicTreeListCtrl.InsertItem(&tvStruct);
				}
			}
			m_wndSchematicTreeListCtrl.Expand(hParent, TVE_EXPAND);
		}
	}

	return hParent;
}

/**
	@brief	CONTROL TYPE LIST CTRL을 
	생성한다.

	@author KHS	

	@return		
**/
int CSchematicTypeCreationDlg::InitControlTypeList(void)
{
	static const CString __HEADERS__[] = {_T("TYPE") , _T("IO TYPE"), _T("DESC."), _T("IO TAG")};
	
	m_wndLcsLocalTypeListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES);

	m_wndLcsLocalTypeListCtrl.InsertColumn(0 , __HEADERS__[0]  , LVCFMT_LEFT , 100);
	m_wndLcsLocalTypeListCtrl.InsertColumn(1 , __HEADERS__[1]  , LVCFMT_LEFT , 100);
	m_wndLcsLocalTypeListCtrl.InsertColumn(2 , __HEADERS__[2]  , LVCFMT_LEFT , 200);
	m_wndLcsLocalTypeListCtrl.InsertColumn(3 , __HEADERS__[3]  , LVCFMT_LEFT , 50);

	m_wndLcpIoTypeListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES);

	m_wndLcpIoTypeListCtrl.InsertColumn(0 , __HEADERS__[0]  , LVCFMT_LEFT , 100);
	m_wndLcpIoTypeListCtrl.InsertColumn(1 , __HEADERS__[1]  , LVCFMT_LEFT , 100);
	m_wndLcpIoTypeListCtrl.InsertColumn(2 , __HEADERS__[2]  , LVCFMT_LEFT , 200);
	m_wndLcpIoTypeListCtrl.InsertColumn(3 , __HEADERS__[3]  , LVCFMT_LEFT , 50);

	m_wndDcsIoTypeListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES);

	m_wndDcsIoTypeListCtrl.InsertColumn(0 , __HEADERS__[0]  , LVCFMT_LEFT , 100);
	m_wndDcsIoTypeListCtrl.InsertColumn(1 , __HEADERS__[1]  , LVCFMT_LEFT , 100);
	m_wndDcsIoTypeListCtrl.InsertColumn(2 , __HEADERS__[2]  , LVCFMT_LEFT , 200);
	m_wndDcsIoTypeListCtrl.InsertColumn(3 , __HEADERS__[3]  , LVCFMT_LEFT , 50);


	return ERROR_SUCCESS;
}

/**
	@brief	ControlType List에 아이템을 추가한다.

	@author KHS	

	@return		
**/
int CSchematicTypeCreationDlg::InsertItemIntoControlTypeListCtrl(const string& rType, const string& rIoType, const string& rDesc, const string& rIoTag,const string& rCategory)
{
	if(rCategory == "LCS or LOCAL TYPE")
	{
		const int nItemCount = m_wndLcsLocalTypeListCtrl.GetItemCount();

		const int nItem = m_wndLcsLocalTypeListCtrl.InsertItem(nItemCount, rType.c_str(), 1);
		if(-1 != nItem)
		{
			m_wndLcsLocalTypeListCtrl.SetItemText(nItem, 1, rIoType.c_str());
			m_wndLcsLocalTypeListCtrl.SetItemText(nItem, 2, rDesc.c_str());
			m_wndLcsLocalTypeListCtrl.SetItemText(nItem, 3, rIoTag.c_str());
		}
	}
	else if(rCategory == "LCP IO TYPE")
	{
		const int nItemCount = m_wndLcpIoTypeListCtrl.GetItemCount();

		const int nItem = m_wndLcpIoTypeListCtrl.InsertItem(nItemCount, rType.c_str(), 1);
		if(-1 != nItem)
		{
			m_wndLcpIoTypeListCtrl.SetItemText(nItem, 1, rIoType.c_str());
			m_wndLcpIoTypeListCtrl.SetItemText(nItem, 2, rDesc.c_str());
			m_wndLcpIoTypeListCtrl.SetItemText(nItem, 3, rIoTag.c_str());
		}
	}
	else
	{
		const int nItemCount = m_wndDcsIoTypeListCtrl.GetItemCount();

		const int nItem = m_wndDcsIoTypeListCtrl.InsertItem(nItemCount, rType.c_str(), 1);
		if(-1 != nItem)
		{
			m_wndDcsIoTypeListCtrl.SetItemText(nItem, 1, rIoType.c_str());
			m_wndDcsIoTypeListCtrl.SetItemText(nItem, 2, rDesc.c_str());
			m_wndDcsIoTypeListCtrl.SetItemText(nItem, 3, rIoTag.c_str());
		}
	}
	
	

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author KHS

	@return		
**/
void CSchematicTypeCreationDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
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
	@brief	

	@author KHS

	@return
**/
void CSchematicTypeCreationDlg::OnMouseMove(UINT nFlags, CPoint point)
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
				// Highlight it
				///pTreeList->SetItemState(m_nDropIndex, LVIS_DROPHILITED, LVIS_DROPHILITED);
				pTreeList->SetItemState(m_hDropItem, LVIS_DROPHILITED, LVIS_DROPHILITED);
				// Redraw item
				//pList->RedrawItems(m_nDropIndex, m_nDropIndex);
				pTreeList->RedrawWindow();
			}else	SetCursor(LoadCursor(NULL, IDC_NO));
		}
		else if(pDropWnd->IsKindOf(RUNTIME_CLASS(CListCtrl)) && (pDropWnd->GetSafeHwnd() == m_wndLcsLocalTypeListCtrl.GetSafeHwnd()))
		{
			TRACE("m_nDropIndex is not 100\n");
			//Note that we can drop here
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			CListCtrl* pList = (CListCtrl*)pDropWnd;
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

/**
	@brief	

	@author KHS

	@return		
**/
void CSchematicTypeCreationDlg::OnLvnBegindragControlTypeList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if(NULL == pNMLV) return;

	OnBeginDrag(pNMLV , m_wndLcsLocalTypeListCtrl);

	*pResult = 0;
}

/**
	@brief	BEGIN DRAG

	@author KHS	

	@return		
**/
int CSchematicTypeCreationDlg::OnBeginDrag(LPNMLISTVIEW pNMLV, CListCtrl& ListCtrl)
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
void CSchematicTypeCreationDlg::DropItemOnList(CListCtrl* pDragList , CTreeListCtrl* pDropList)
{
	if((pDropList->GetSafeHwnd() == m_wndSchematicTreeListCtrl.GetSafeHwnd()) || 
		(pDropList->GetSafeHwnd() == m_wndLcsLocalTypeListCtrl.GetSafeHwnd()) )
	{
		HTREEITEM hParent = m_wndSchematicTreeListCtrl.GetParentItem(m_hDropItem);
		if(NULL == hParent&& m_hDropItem)
		{
			// Turn off hilight for previous drop target
			pDropList->SetItemState (m_hDropItem, 0, LVIS_DROPHILITED);
			// Redraw previous item
			//pDropList->RedrawItems (m_nDropIndex, m_nDropIndex);
			
			CString rControlType = pDragList->GetItemText(m_nDragIndex, 0);
			CString rSchematicType = pDropList->GetItemText(m_hDropItem);

			CString rString;
			CELoadDocData& docData = CELoadDocData::GetInstance();
			CELOADTable* pMapTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_TYPE_MAP"));
			if(pMapTable)
			{
				if(pDropList->GetSafeHwnd() == m_wndSchematicTreeListCtrl.GetSafeHwnd())
				{
					if(!CompareMatchType(rSchematicType, rControlType))
					{
						CELOADTable::Record record;

						record.FieldValueMap.insert(make_pair(_T("C_SCHEM_TYPE") , rSchematicType));
						record.FieldValueMap.insert(make_pair(_T("C_CONTROL_TYPE") , rControlType));

						pMapTable->m_RecordEntry.push_back(record);

						TV_INSERTSTRUCT tvStruct;

						tvStruct.hParent = m_hDropItem;
						tvStruct.hInsertAfter = TVI_SORT;
						tvStruct.item.iImage = 1;
						tvStruct.item.iSelectedImage = 1;
						tvStruct.item.pszText = (LPSTR)(LPCTSTR)rControlType;
						tvStruct.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

						m_wndSchematicTreeListCtrl.InsertItem(&tvStruct);

						m_bModified = true;

					}
					else
					{
						::MessageBox(NULL, "The control type exists already!! ", "Warning!", MB_OK|MB_ICONWARNING );
					}
					m_wndSchematicTreeListCtrl.UpdateWindow();
				}
			}
		}
		else
		{
			::MessageBox(NULL, "Schematic Type을 선택하세요!! ", "Warning!", MB_OK|MB_ICONWARNING );
		}
	}
}

/**
	@brief	SCHEMATIC TYPE 과 CONTROL TYPE이 같은게 있는지 비교한다.

	@author KHS	

	@return	 같은게 있으면 TRUE	
**/
BOOL CSchematicTypeCreationDlg::CompareMatchType(const CString& rSchematicType, const CString& rControlType)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pMapTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_TYPE_MAP"));
	if(pMapTable)
	{
		for(vector<CELOADTable::Record>::iterator itr = pMapTable->m_RecordEntry.begin(); itr != pMapTable->m_RecordEntry.end();++itr)
		{
			if(rSchematicType == itr->FieldValueMap[_T("C_SCHEM_TYPE")].c_str() && rControlType == itr->FieldValueMap[_T("C_CONTROL_TYPE")].c_str())
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

/**
	@brief	다이얼로그 박스 초기화

	@author KHS	

	@return		
**/
BOOL CSchematicTypeCreationDlg::Init(void)
{
	m_wndSchematicTreeListCtrl.DeleteAllItems();
	m_wndLcsLocalTypeListCtrl.DeleteAllItems();

	CELoadDocData& docData = CELoadDocData::GetInstance();
	{
		//! Schematic-Control Type 테이블의 아이템중 Schematic Type 테이블에서 찾을수 없는 아이템은 삭제한다.
		/*
		CELOADTable* pSchemTypeTable = docData.GetTableOf(_T("T_SCHEMATIC_TYPE"));
		CELOADTable* pSchemControlTypeMapTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_TYPE_MAP"));
		if(pSchemTypeTable && pSchemControlTypeMapTable)
		{
			for(vector<CELOADTable::Record>::iterator itr = pSchemControlTypeMapTable->m_RecordEntry.begin();itr != pSchemControlTypeMapTable->m_RecordEntry.end();)
			{
				vector<CELOADTable::Record>::iterator jtr;
				for(jtr = pSchemTypeTable->m_RecordEntry.begin();jtr != pSchemTypeTable->m_RecordEntry.end();++jtr)
				{
					if(itr->FieldValueMap[_T("C_SCHEM_TYPE")] == itr->FieldValueMap[_T("C_TYPE")]) break;
				}
				if(jtr == pSchemTypeTable->m_RecordEntry.end())
				{
					itr = pSchemControlTypeMapTable->m_RecordEntry.erase(itr);
				}else	++itr;
			}
		}
		*/
	}

	int nIndex = 0;
	DisplaySchematicTypeData();
	HTREEITEM hItem = m_wndSchematicTreeListCtrl.GetRootItem();
	while(hItem)
	{
		const string& rItemText = m_wndSchematicTreeListCtrl.GetItemText(hItem , 0);
		if(rItemText == m_rInitSchematicType)
		{
			m_wndSchematicTreeListCtrl.SelectItem(hItem);

			RECT itemRect , clientRect;
			m_wndSchematicTreeListCtrl.GetClientRect(&clientRect);
			m_wndSchematicTreeListCtrl.ListGetItemRect(0 , 0 , &itemRect, LVIR_BOUNDS);   // 한칸의 높이를 측정
			if((itemRect.bottom - itemRect.top) > (clientRect.bottom - clientRect.top))
			{
				//! 선택한 Schematic Type이 화면에 보이도록 스크롤바를 이동한다.
				m_wndSchematicTreeListCtrl.Select(hItem , TVGN_FIRSTVISIBLE);
			}
			
			break;
		}
		HTREEITEM hChild = m_wndSchematicTreeListCtrl.GetChildItem(hItem);
		while(hChild)
		{
			hChild = m_wndSchematicTreeListCtrl.GetNextSiblingItem(hChild);
			++nIndex;
		}
		hItem = m_wndSchematicTreeListCtrl.GetNextSiblingItem(hItem);
		++nIndex;
	}

	DisplayControlTypeData();

	return TRUE;
}

/**
	@brief	Schematic Type Info Dialog를 호출한다.

	@author BHK	

	@date 2009-05-13 오후 5:05:44	

	@param	

	@return		
*/
void CSchematicTypeCreationDlg::OnItemModify()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	HTREEITEM hItem = m_wndSchematicTreeListCtrl.GetSelectedItem();
	if(hItem)
	{
		HTREEITEM hParent = m_wndSchematicTreeListCtrl.GetParentItem(hItem);
		if(NULL == hParent)
		{
			const CString rCurSchematicType = m_wndSchematicTreeListCtrl.GetItemText(hItem);
			const CString rCurSchematicDesc = m_wndSchematicTreeListCtrl.GetItemText(hItem , 1);
			CSchematicTypeInfoDlg dlg(rCurSchematicType , rCurSchematicDesc);
			if((IDOK == dlg.DoModal()) /*&& (dlg.m_rOldSchemType != dlg.m_rSchemType)*/)
			{
				//! Schematic , Schematic-Control Type Map Table.을 수정해야 한다.
				ChangeSchemTypeInTable(dlg.m_rSchemType.operator LPCSTR() , dlg.m_rSchemDesc.operator LPCSTR() , rCurSchematicType.operator LPCSTR());
				CELoadDocData& docData = CELoadDocData::GetInstance();

				CELOADTable* pSchemTypeTable = docData.GetTableOf(_T("T_SCHEMATIC_TYPE"));
				if(pSchemTypeTable)
				{
					for(vector<CELOADTable::Record>::iterator itr = pSchemTypeTable->m_RecordEntry.begin(); itr != pSchemTypeTable->m_RecordEntry.end();++itr)
					{
						if((rCurSchematicType == itr->FieldValueMap[_T("C_TYPE")].c_str()))
						{
							itr->FieldValueMap[_T("C_TYPE")] = dlg.m_rSchemType.operator LPCSTR();
							itr->FieldValueMap[_T("C_DESC")] = dlg.m_rSchemDesc.operator LPCSTR();
							break;
						}
					}
				}

				CELOADTable* pSchemControlMapTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_TYPE_MAP"));
				if(pSchemControlMapTable)
				{
					for(vector<CELOADTable::Record>::iterator itr = pSchemControlMapTable->m_RecordEntry.begin(); itr != pSchemControlMapTable->m_RecordEntry.end();++itr)
					{
						if((rCurSchematicType == itr->FieldValueMap[_T("C_SCHEM_TYPE")].c_str()))
						{
							itr->FieldValueMap[_T("C_SCHEM_TYPE")] = dlg.m_rSchemType.operator LPCSTR();
						}
					}
				}

				CELOADTable* pSchemCableMapTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_CABLE_MAP"));
				if(pSchemCableMapTable)
				{
					for(vector<CELOADTable::Record>::iterator itr = pSchemCableMapTable->m_RecordEntry.begin(); itr != pSchemCableMapTable->m_RecordEntry.end();++itr)
					{
						if((rCurSchematicType == itr->FieldValueMap[_T("C_SCHEM_TYPE")].c_str()))
						{
							itr->FieldValueMap[_T("C_SCHEM_TYPE")] = dlg.m_rSchemType.operator LPCSTR();
						}
					}
				}

				CELOADTable* pControlCableMapTable = docData.GetTableOf(_T("T_CONTROL_CABLE_CONTROL_TYPE_MAP"));
				if(pControlCableMapTable)
				{
					for(vector<CELOADTable::Record>::iterator itr = pControlCableMapTable->m_RecordEntry.begin(); itr != pControlCableMapTable->m_RecordEntry.end();++itr)
					{
						if((rCurSchematicType == itr->FieldValueMap[_T("C_SCHEM_TYPE")].c_str()))
						{
							itr->FieldValueMap[_T("C_SCHEM_TYPE")] = dlg.m_rSchemType.operator LPCSTR();
						}
					}
				}

				m_wndSchematicTreeListCtrl.SetItemText(hItem , dlg.m_rSchemType , 0);
				m_wndSchematicTreeListCtrl.SetItemText(hItem , dlg.m_rSchemDesc , 1);

				ModifySchematicTypeInLoad(rCurSchematicType, dlg.m_rSchemType);

				m_bModified = true;
			}
		}
	}
}

/**
	@brief	TREE에 같은 이름의 SCHEMATIC TYPE 이 있는지 검사

	@author KHS	

	@date 2009-05-13 오후 5:06:21	

	@param	

	@return		
**/
BOOL CSchematicTypeCreationDlg::IsExistItemText(const CString& rItemText)
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
	@brief	모든 ELoad Table들에서 Schem Name을 변경한다.

	@author BHK	

	@date 2009-05-13 오후 7:03:13	

	@param	

	@return		
*/
int CSchematicTypeCreationDlg::ChangeSchemTypeInTable(const string& rNewSchemType , const string& rNewSchemDesc , const string& rOldSchemType)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	{
		CELOADTable* pTable = docData.GetTableOf(_T("T_CONTROL_CABLE_CONTROL_TYPE_MAP"));
		if(pTable)
		{
			for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin();itr != pTable->m_RecordEntry.end();++itr)
			{
				if(rOldSchemType == itr->FieldValueMap[_T("C_SCHEM_TYPE")].c_str())
				{
					itr->FieldValueMap[_T("C_SCHEM_TYPE")] = rNewSchemType;
				}
			}
		}
	}
	{
		CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_CABLE_MAP"));
		if(pTable)
		{
			for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin();itr != pTable->m_RecordEntry.end();++itr)
			{
				if(rOldSchemType == itr->FieldValueMap[_T("C_SCHEM_TYPE")].c_str())
				{
					itr->FieldValueMap[_T("C_SCHEM_TYPE")] = rNewSchemType;
				}
			}
		}
	}
	{
		CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_TYPE_MAP"));
		if(pTable)
		{
			for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin();itr != pTable->m_RecordEntry.end();++itr)
			{
				if(rOldSchemType == itr->FieldValueMap[_T("C_SCHEM_TYPE")].c_str())
				{
					itr->FieldValueMap[_T("C_SCHEM_TYPE")] = rNewSchemType;
				}
			}
		}
	}
	{
		CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_TYPE"));
		if(pTable)
		{
			for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin();itr != pTable->m_RecordEntry.end();++itr)
			{
				if(rOldSchemType == itr->FieldValueMap[_T("C_TYPE")].c_str())
				{
					itr->FieldValueMap[_T("C_TYPE")] = rNewSchemType;
					itr->FieldValueMap[_T("C_DESC")] = rNewSchemDesc;
				}
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	LOAD에 Schematic type 을 변경한다.

	@author KHS	

	@date 2009-05-19 오후 3:23:56	

	@param	

	@return		
**/
int CSchematicTypeCreationDlg::ModifySchematicTypeInLoad(const CString& rOldSchematicType, const CString& rNewSchematicType)
{
	ASSERT(rOldSchematicType && "rOldSchematicType is empty");

	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> LoadItemList;
	docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
	//if(pLoadItemList)
	{
		for(list<CELoadItem*>::iterator itr = LoadItemList.begin(); itr != LoadItemList.end();++itr)
		{
			if((*itr)->IsDeleted()) continue;

			if(rOldSchematicType == (*itr)->prop()->GetValue(_T("Control Cable") , _T("Schematic Ty")).c_str())
			{
				(*itr)->prop()->SetValue(_T("Control Cable") , _T("Schematic Ty"), rNewSchematicType.operator LPCSTR());
			}
		}
		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	Control type delete

	@author KHS	

	@date 2009-06-05 오전 11:13:43	

	@param	

	@return		
**/
void CSchematicTypeCreationDlg::OnControltypemenuDelete()
{
	// TODO: Add your command handler code here
	OnItemDelete();
}

void CSchematicTypeCreationDlg::OnLvnBegindragControlTypeList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if(NULL == pNMLV) return;

	OnBeginDrag(pNMLV , m_wndLcpIoTypeListCtrl);

	*pResult = 0;
}

void CSchematicTypeCreationDlg::OnLvnBegindragControlTypeList3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if(NULL == pNMLV) return;

	OnBeginDrag(pNMLV , m_wndDcsIoTypeListCtrl);

	*pResult = 0;
}
