// CableRouteGroupCreationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "CableRouteGroupCreationDlg.h"
#include "CableRouteTypeCreationDlg.h"
#include "ELoadDocData.h"

// CCableRouteGroupCreationDlg dialog

/*
struct MatchParent : public binary_function<CELOADTable::Record, string, bool>
{
        bool operator()(const CELOADTable::Record lhs , const string& rhs) const
	{
		const map<string,string>::const_iterator where = lhs.FieldValueMap.find(_T("C_PARENT"));
		if(where != lhs.FieldValueMap.end())
		{
			return (rhs == where->second);
		}

		return false;
	}
};
*/
IMPLEMENT_DYNAMIC(CCableRouteGroupCreationDlg, CWizardPropertyPage)

CCableRouteGroupCreationDlg::CCableRouteGroupCreationDlg(CWnd* pParent /*=NULL*/)
	: CWizardPropertyPage(CCableRouteGroupCreationDlg::IDD/*, pParent*/)
	, bDeleteRouteGr(false)
{
	m_bModified = false;
}

CCableRouteGroupCreationDlg::~CCableRouteGroupCreationDlg()
{
}

void CCableRouteGroupCreationDlg::DoDataExchange(CDataExchange* pDX)
{
	CWizardPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_ROUTE_GR_EDIT, m_wndRouteGrEdit);
	DDX_Control(pDX, IDC_ROUTE_GR_DESC_EDIT, m_wndRouteGrDescEdit);
	DDX_Control(pDX, IDC_ROUTE_GR_ADD_BUTTON, m_wndRouteGrAddButton);
	DDX_Control(pDX, ID_SAVE, m_wndSaveButton);
	DDX_Control(pDX, IDCANCEL, m_wndCancelButton);
	DDX_Control(pDX, IDC_START_EDIT, m_wndStartEdit);
	DDX_Control(pDX, IDC_END_EDIT, m_wndEndEdit);
	DDX_Control(pDX, IDC_START_BUTTON, m_wndStartButton);
	DDX_Control(pDX, IDC_END_BUTTON, m_wndEndButton);
	DDX_Control(pDX, ID_WIZBACK, m_wndBackButton);
	DDX_Control(pDX, ID_WIZNEXT, m_wndNextButton);
}


BEGIN_MESSAGE_MAP(CCableRouteGroupCreationDlg, CDialog)
	ON_BN_CLICKED(IDC_ROUTE_GR_ADD_BUTTON, &CCableRouteGroupCreationDlg::OnBnClickedRouteGrAddButton)
	ON_BN_CLICKED(ID_SAVE, &CCableRouteGroupCreationDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CCableRouteGroupCreationDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_START_BUTTON, &CCableRouteGroupCreationDlg::OnBnClickedStartButton)
	ON_BN_CLICKED(IDC_END_BUTTON, &CCableRouteGroupCreationDlg::OnBnClickedEndButton)
	ON_BN_CLICKED(ID_WIZBACK, &CCableRouteGroupCreationDlg::OnBnClickedWizback)
	ON_BN_CLICKED(ID_WIZNEXT, &CCableRouteGroupCreationDlg::OnBnClickedWiznext)
	ON_COMMAND(ID_CABLE_ROUTE_DELETE, &CCableRouteGroupCreationDlg::OnCableRouteDelete)
END_MESSAGE_MAP()


// CCableRouteGroupCreationDlg message handlers

BOOL CCableRouteGroupCreationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CreateImageList();
	
	if(ERROR_SUCCESS != CreateRouteTypeTreeListCtrl())
	{
		TRACE0("Failed to create TreeList Ctrl \n");
		return FALSE;
	}

	if(ERROR_SUCCESS != CreateRouteGroupTreeListCtrl())
	{
		TRACE0("Failed to create TreeList Ctrl \n");
		return FALSE;
	}

	m_wndRouteTreeListCtrl.SetImageList(&m_imgList, LVSIL_NORMAL);
	m_wndRouteGroupTreeListCtrl.SetImageList(&m_imgList, LVSIL_NORMAL);
	m_wndRouteGroupTreeListCtrl.SetDeleteEvent(true);

	if(ERROR_SUCCESS != InitEditCtrl())
	{
		return FALSE;
	}

	if(ERROR_SUCCESS != DisplayRouteTreeCtrl())
	{
		return FALSE;
	}

	if(ERROR_SUCCESS != DisplayRouteGroupTreeCTrl())
	{
		return FALSE;
	}

	//! BUTTON SETTING
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));

	m_wndRouteGrAddButton.SetTextHotColor(TextHotColor);
	m_wndSaveButton.SetTextHotColor(TextHotColor);
	m_wndCancelButton.SetTextHotColor(TextHotColor);
	m_wndStartButton.SetTextHotColor(TextHotColor);
	m_wndEndButton.SetTextHotColor(TextHotColor);
	
	m_wndBackButton.SetTextHotColor(TextHotColor);
	m_wndNextButton.SetTextHotColor(TextHotColor);
	
	m_wndBackButton.SetIcon(IDI_PRE);
	m_wndNextButton.SetIcon(IDI_NEXT);

	m_wndRouteGrAddButton.SetIcon(IDI_ADD);
	m_wndSaveButton.SetIcon(IDI_OK_18X18);
	m_wndCancelButton.SetIcon(IDI_CANCEL_18X18);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	ROUTE GROUP ROOT ITEM

	@author KHS	

	@return		
**/
HTREEITEM CCableRouteGroupCreationDlg::AddRootTrayItem(const CString& rRouteGroup, const CString& rDesc, const CString& rDesignLength, CTreeListCtrlEx& TreeCtrl)
{
	TV_INSERTSTRUCT TI;
	TI.hParent      = NULL;
	TI.item.iImage  = 0;
	TI.item.iSelectedImage = 0;
	TI.item.mask    = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	TI.item.pszText = (LPSTR)(rRouteGroup.operator LPCSTR());

	HTREEITEM hItem = TreeCtrl.InsertItem(&TI);

	TreeCtrl.SetItemText(hItem, rDesc, 1);
	TreeCtrl.SetItemText(hItem, rDesignLength, 2);

	TreeCtrl.Expand(hItem, TVE_EXPAND);

	return hItem;
}

/**
	@brief	ROUTE TYPE ROOT ITEM

	@author KHS	

	@return		
**/
HTREEITEM CCableRouteGroupCreationDlg::AddRootTrayItem(const CString& rRouteType, const CString& rLength, const CString& rTD, const CString& rDefaultSize, CTreeListCtrlEx& TreeCtrl)
{
	TV_INSERTSTRUCT TI;
	TI.hParent      = NULL;
	TI.item.iImage  = 0;
	TI.item.iSelectedImage = 0;
	TI.item.mask    = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	TI.item.pszText = (LPSTR)(rRouteType.operator LPCSTR());

	HTREEITEM hItem = TreeCtrl.InsertItem(&TI);

	TreeCtrl.SetItemText(hItem, rLength, 1);
	TreeCtrl.SetItemText(hItem, rTD, 2);
	TreeCtrl.SetItemText(hItem, rDefaultSize, 4);

	TreeCtrl.Expand(hItem, TVE_EXPAND);

	return hItem;
}

/**
	@brief	

	@author KHS

	@return		
**/
HTREEITEM CCableRouteGroupCreationDlg::AddChildTrayItem(HTREEITEM hParent, CTreeListCtrlEx& TreeCtrl, const CString&  rRouteType,const CString& rLength, const CString& rTD, const CString& rDirection,const CString& rDefaultSize)
{
	int nImage = 1;
	if(rDirection.IsEmpty())	nImage = 1;
	else if("Left" == rDirection)	nImage = 3;
	else if("Right" == rDirection)	nImage = 4;
	else				nImage = 5;

	TV_INSERTSTRUCT TI;
	TI.hParent      = hParent;
	TI.item.iImage  = nImage;
	TI.item.iSelectedImage = nImage;
	TI.item.mask    = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	TI.item.pszText = (LPSTR)(rRouteType.operator LPCSTR());

	HTREEITEM hItem = TreeCtrl.InsertItem(&TI);

	if(TreeCtrl.GetSafeHwnd() == m_wndRouteGroupTreeListCtrl.GetSafeHwnd())
	{
		TreeCtrl.SetItemText(hItem, rLength,2);
		TreeCtrl.SetItemText(hItem, rTD, 3);
		TreeCtrl.SetItemText(hItem, rDirection, 4);
		TreeCtrl.SetItemText(hItem, rDefaultSize, 5);
	}
	else
	{

		TreeCtrl.SetItemText(hItem, rLength,1);
		TreeCtrl.SetItemText(hItem, rTD, 2);
		TreeCtrl.SetItemText(hItem, rDirection, 3);
		TreeCtrl.SetItemText(hItem, rDefaultSize, 4);
	}

	TreeCtrl.Expand(hParent, TVE_EXPAND);

	return hItem;
}

/**
	@brief	ROUTE TYPE TREE DISPLAY

	@author KHS	

	@return		
**/
int CCableRouteGroupCreationDlg::DisplayRouteTreeCtrl(void)
{
	//m_wndRouteTreeListCtrl
	CString rRootName;
	HTREEITEM hRoot = NULL;
	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> RouteItemList;
	docData.GetELoadItemListOf(RouteItemList , CRouteItem::TypeString());
	//if(pRouteItemList)
	{
		//! 먼저 ROOT item을 찾아서 
		for(list<CELoadItem*>::iterator jtr = RouteItemList.begin(); jtr != RouteItemList.end();++jtr)
		{
			if((*jtr)->IsDeleted()) continue;
			if((*jtr)->prop()->GetValue(_T("General"),_T("Parent")) == "ROOT")
			{
				const string rRouteType = (*jtr)->prop()->GetValue(_T("General"),_T("Route ID"));
				const string rLength = (*jtr)->prop()->GetValue(_T("General"),_T("Route Length"));
				const string rTD = (*jtr)->prop()->GetValue(_T("General"),_T("TD"));
				const string rDefaultSize = (*jtr)->prop()->GetValue(_T("General"),_T("Default Size"));

				hRoot = AddRootTrayItem(rRouteType.c_str(), rLength.c_str(), rTD.c_str(), rDefaultSize.c_str(), m_wndRouteTreeListCtrl);

				//!루프를 돌면서 부모를 찾아서 
				for(list<CELoadItem*>::iterator itr = RouteItemList.begin(); itr != RouteItemList.end();++itr)
				{
					if((*itr)->IsDeleted()) continue;

					const string rRouteType = (*itr)->prop()->GetValue(_T("General"),_T("Route ID"));
					const string rLength = (*itr)->prop()->GetValue(_T("General"),_T("Route Length"));
					const string rTD = (*itr)->prop()->GetValue(_T("General"),_T("TD"));
					const string rDirection = (*itr)->prop()->GetValue(_T("General"),_T("Direction"));
					const string rDefaultSize = (*itr)->prop()->GetValue(_T("General"),_T("Default Size"));
					const string rParent = (*itr)->prop()->GetValue(_T("General"),_T("Parent"));

					if(rParent == "ROOT") continue;

					//const string rRouteItem = rRouteType + "-" + rLength + "-" + rTD + "-" + rDirection + "-" + rDefaultSize;

					int res = FindNodeByString(hRoot, rParent.c_str());
					if(1 == res)
					{
						HTREEITEM hItem = m_wndRouteTreeListCtrl.GetSelectedItem();
						if(hItem)
						{
							AddChildTrayItem(hItem, m_wndRouteTreeListCtrl, rRouteType.c_str() , rLength.c_str(), rTD.c_str(), rDirection.c_str(), rDefaultSize.c_str());
						}
					}
				}
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	DISPLAY ROUTE GROUP TREE

	@author KHS	

	@return		
**/
int CCableRouteGroupCreationDlg::DisplayRouteGroupTreeCTrl(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pGroupTable = docData.GetTableOf(_T("T_ROUTE_GROUP"));
	list<CELoadItem*> RouteItemList;
	docData.GetELoadItemListOf(RouteItemList , CRouteItem::TypeString());
	if(pGroupTable)
	{
		HTREEITEM hParent = NULL;
		for(vector<CELOADTable::Record>::iterator itr = pGroupTable->m_RecordEntry.begin(); itr != pGroupTable->m_RecordEntry.end();++itr)
		{
			const string rRouteGroup = itr->FieldValueMap[_T("C_ROUTE_GROUP")];
			const string rDesc = itr->FieldValueMap[_T("C_DESC")];
			const string rDesignLength = itr->FieldValueMap[_T("C_DESIGN_LENGTH")];
			const string rRouteType = itr->FieldValueMap[_T("C_ROUTE_TYPE")];

			HTREEITEM hRoot = m_wndRouteGroupTreeListCtrl.GetRootItem();
			
			//! 같은 GROUP의 레코드는 한번만 삽입
			if(FALSE == FindSiblingNodeByString(hRoot, rRouteGroup.c_str()))
			{
				hParent = AddRootTrayItem(rRouteGroup.c_str(), rDesc.c_str(), rDesignLength.c_str(), m_wndRouteGroupTreeListCtrl);
			}
			
			//if(pRouteItemList)
			{
				list<CELoadItem*>::iterator where = find_if(RouteItemList.begin(), RouteItemList.end(), bind2nd(MatchRouteType(), rRouteType));
				if(where != RouteItemList.end())
				{
					const string rRouteType = (*where)->prop()->GetValue(_T("General"),_T("Route ID"));
					const string rLength = (*where)->prop()->GetValue(_T("General"),_T("Route Length"));
					const string rTD = (*where)->prop()->GetValue(_T("General"),_T("TD"));
					const string rDirection = (*where)->prop()->GetValue(_T("General"),_T("Direction"));
					const string rDefaultSize = (*where)->prop()->GetValue(_T("General"),_T("Default Size"));
					const string rParent = (*where)->prop()->GetValue(_T("General"),_T("Parent"));

					AddChildTrayItem(hParent, m_wndRouteGroupTreeListCtrl, rRouteType.c_str(), rLength.c_str(), rTD.c_str(), rDirection.c_str(), rDefaultSize.c_str());
				}
			}
		}
	}
	return ERROR_SUCCESS;
}

/**
	@brief	ADD ITEM

	@author KHS	

	@return		
**/
void CCableRouteGroupCreationDlg::OnBnClickedRouteGrAddButton()
{
	CString rRouteGroup, rRouteGroupDesc, rDesignLength, rStartRoute, rEndRoute;
	CStringArray SelectedItems;
	int nDesignLength = 0;

	//! ROUTE GROUP 이름 중복 체크
	m_wndRouteGrEdit.GetWindowText(rRouteGroup);

	if(IsExistRouteTypeName(rRouteGroup))
	{
		ClearEdit();
		::MessageBox(NULL, "Route Group이 이미 존재합니다 !!", "Warning", MB_OK|MB_ICONWARNING);
		return;
	}
	
	m_wndRouteGrDescEdit.GetWindowText(rRouteGroupDesc);
	
	m_wndStartEdit.GetWindowText(rStartRoute);
	m_wndEndEdit.GetWindowText(rEndRoute);

	if(rStartRoute.IsEmpty() || rEndRoute.IsEmpty())
	{
		::MessageBox(NULL, "Start & End를 선택하세요...", "Warning", MB_OK | MB_ICONWARNING);
		return;
	}

	if(rRouteGroup.IsEmpty())
	{
		::MessageBox(NULL, "Route Group을 입력하세요...", "Warning", MB_OK | MB_ICONWARNING);
		return;
	}
	
	vector<string> oResult;
	CTokenizer<CIsFromString>::Tokenize(oResult, rStartRoute.operator LPCSTR(), CIsFromString("-"));
	if(oResult.empty()) return;

	CString rStartRouteType = oResult[0].c_str();
	
	CTokenizer<CIsFromString>::Tokenize(oResult, rEndRoute.operator LPCSTR(), CIsFromString("-"));
	if(oResult.empty()) return;
	
	CString rEndRouteType = oResult[0].c_str();

	//!START - END 경로 사이의 TREE 아이템을 모두 찾는다.
	HTREEITEM hItem = m_wndRouteTreeListCtrl.GetSelectedItem();
	CString rParent;
	int res = FindNodeByStringReverse(hItem, rEndRouteType);
	if(res == 1)
	{
		while(rStartRouteType != rParent)
		{
			HTREEITEM hSelItem = m_wndRouteTreeListCtrl.GetSelectedItem();
			CString rSelItem = m_wndRouteTreeListCtrl.GetItemText(hSelItem);
			SelectedItems.Add(rSelItem);
			HTREEITEM hParent = m_wndRouteTreeListCtrl.GetParentItem(hSelItem);
			rParent = m_wndRouteTreeListCtrl.GetItemText(hParent);
			m_wndRouteTreeListCtrl.Select(hParent,TVGN_CARET);
		}
		SelectedItems.Add(rParent);
	}
	
	//! ROUTE TYPE TABLE 에서 찾은 트리 아이템들을 찾는다.
	vector<CELOADTable::Record> FIND_ROUTE_TYPE_ENTRY;

	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> RouteItemList;
	docData.GetELoadItemListOf(RouteItemList , CRouteItem::TypeString());
	///if(pRouteItemList)
	{
		for(int i =0;i < SelectedItems.GetSize();i++)
		{
			CTokenizer<CIsFromString>::Tokenize(oResult, SelectedItems[i].operator LPCSTR(), CIsFromString("-"));
			if(0 < oResult.size())
			{
				list<CELoadItem*>::iterator where = find_if(RouteItemList.begin(), RouteItemList.end(), bind2nd(MatchRouteType(), oResult[0]));
				if(where != RouteItemList.end())
				{
					CELOADTable::Record record;
					
					record.FieldValueMap[_T("C_ROUTE_TYPE")] = (*where)->prop()->GetValue(_T("General"),_T("Route ID"));
					record.FieldValueMap[_T("C_LENGTH")] = (*where)->prop()->GetValue(_T("General"),_T("Route Length"));
					record.FieldValueMap[_T("C_TD")] = (*where)->prop()->GetValue(_T("General"),_T("TD"));
					record.FieldValueMap[_T("C_DIRECTION")] = (*where)->prop()->GetValue(_T("General"),_T("Direction"));
					record.FieldValueMap[_T("C_DEFAULT_SIZE")] = (*where)->prop()->GetValue(_T("General"),_T("Default Size"));
					record.FieldValueMap[_T("C_PARENT")] = (*where)->prop()->GetValue(_T("General"),_T("Parent"));
					
					FIND_ROUTE_TYPE_ENTRY.push_back(record);

					nDesignLength += atoi((*where)->prop()->GetValue(_T("General"),_T("Route Length")).c_str());
				}
			}
		}
	}
	rDesignLength.Format("%d", nDesignLength);

	//! ROUTE GROUP 아이템 추가
	HTREEITEM hParent = AddRootTrayItem(rRouteGroup,rRouteGroupDesc,rDesignLength, m_wndRouteGroupTreeListCtrl);

	CELOADTable* pGroupTable = docData.GetTableOf(_T("T_ROUTE_GROUP"));
	if(pGroupTable)
	{
		//! GROUP에 ROUTE TYPE을 추가
		for(vector<CELOADTable::Record>::reverse_iterator itr = FIND_ROUTE_TYPE_ENTRY.rbegin(); itr != FIND_ROUTE_TYPE_ENTRY.rend();++itr)
		{
			CELOADTable::Record record;

			record.FieldValueMap[_T("C_ROUTE_GROUP")] = rRouteGroup.operator LPCSTR();
			record.FieldValueMap[_T("C_DESC")] = rRouteGroupDesc.operator LPCSTR();
			record.FieldValueMap[_T("C_DESIGN_LENGTH")] = rDesignLength.operator LPCSTR();
			record.FieldValueMap[_T("C_ROUTE_TYPE")] = itr->FieldValueMap[_T("C_ROUTE_TYPE")];

			const string rRouteType = itr->FieldValueMap[_T("C_ROUTE_TYPE")];
			const string rLength = itr->FieldValueMap[_T("C_LENGTH")];
			const string rTD = itr->FieldValueMap[_T("C_TD")];
			const string rDirection = itr->FieldValueMap[_T("C_DIRECTION")];
			const string rDefaultSize  = itr->FieldValueMap[_T("C_DEFAULT_SIZE")];
			const string rParent = itr->FieldValueMap[_T("C_PARENT")];

			pGroupTable->m_RecordEntry.push_back(record);
			
			AddChildTrayItem(hParent, m_wndRouteGroupTreeListCtrl, rRouteType.c_str(), rLength.c_str(), rTD.c_str(), rDirection.c_str(), rDefaultSize.c_str());
		}
		m_bModified = true;
	}

	ClearEdit();
}



/**
	@brief	SAVE DATA

	@author KHS	

	@return		
**/
void CCableRouteGroupCreationDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if(ERROR_SUCCESS == SaveData())
	{
		::MessageBox(NULL, "SUCCESS!!", "Saved", MB_OK|MB_ICONINFORMATION );
		bDeleteRouteGr = false;
		m_bModified = false;
	}
}

void CCableRouteGroupCreationDlg::OnBnClickedCancel()
{
	if(m_bModified)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		if(IDYES == AfxMessageBox(docData.GetStringOf(48).c_str() , MB_YESNO|MB_ICONWARNING ))
		{
			SaveData();
			m_bModified = false;
			GetParent()->SendMessage(WM_COMMAND , IDCANCEL);
		}
		else
		{

			GetParent()->SendMessage(WM_COMMAND , IDCANCEL);
		}
	}
	else
	{
		GetParent()->SendMessage(WM_COMMAND , IDCANCEL);
	}
}

/**
	@brief	INIT EDIT CTRL

	@author KHS	

	@return		
**/
int CCableRouteGroupCreationDlg::InitEditCtrl(void)
{
	try
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();

		static const CString rToolTip(_T("데이터 형식이 일치하지 않습니다."));
		const COLORREF WavyLineColor = docData.GetColorCode("FILTER_EDIT_WAVY_LINE_COLOR");
		const COLORREF BackFocusColor= docData.GetColorCode("FILTER_EDIT_BACK_FOCUS_COLOR");
		const COLORREF TextErrorColor= docData.GetColorCode("FILTER_EDIT_TEXT_ERROR_COLOR");
		const COLORREF BackErrorColor= docData.GetColorCode("FILTER_EDIT_BACK_ERROR_COLOR");

		const string rTextRegEx = docData.GetRegExp("TEXT");
		const string rNumberRegEx = docData.GetRegExp("NUMBER");

		//! ROUTE GROUP
		m_wndRouteGrEdit.SetRegEx(rTextRegEx.c_str());
		m_wndRouteGrEdit.CreateToolTip(this , rToolTip);
		m_wndRouteGrEdit.SetWavyLineColour (WavyLineColor);
		m_wndRouteGrEdit.SetBackgroundColourFocus (BackFocusColor);
		m_wndRouteGrEdit.SetForegroundColourError (TextErrorColor);
		m_wndRouteGrEdit.SetBackgroundColourError (BackErrorColor);

		//! ROUTE GROUP DESC
		m_wndRouteGrDescEdit.SetRegEx(rTextRegEx.c_str());
		m_wndRouteGrDescEdit.CreateToolTip(this , rToolTip);
		m_wndRouteGrDescEdit.SetWavyLineColour (WavyLineColor);
		m_wndRouteGrDescEdit.SetBackgroundColourFocus (BackFocusColor);
		m_wndRouteGrDescEdit.SetForegroundColourError (TextErrorColor);
		m_wndRouteGrDescEdit.SetBackgroundColourError (BackErrorColor);

	}
	catch(...)
	{
		return ERROR_BAD_ENVIRONMENT;
	}

	return ERROR_SUCCESS;
}

/**
	@brief	CLEAR EDIT CTRL

	@author KHS	

	@return		
**/
int CCableRouteGroupCreationDlg::ClearEdit(void)
{
	m_wndRouteGrEdit.ClearText();
	m_wndRouteGrDescEdit.ClearText();
	m_wndStartEdit.SetWindowText("");
	m_wndEndEdit.SetWindowText("");

	return ERROR_SUCCESS;
}

/**
	@brief	이름으로 TREEITEM을 찾는다.

	@author KHS	

	@return		
**/
int CCableRouteGroupCreationDlg::FindNodeByString(HTREEITEM hItem, const CString& rName)
{
	if(m_wndRouteTreeListCtrl.GetItemText(hItem) == rName)
	{
		//! 검색하고자 하는 항목을 찾은 경우..
		m_wndRouteTreeListCtrl.Select(hItem, TVGN_CARET);
		return 1;
	} 
	else
	{
		HTREEITEM hChild = m_wndRouteTreeListCtrl.GetChildItem(hItem);
		if(hChild)
		{ 
			do 
			{
				if(1 == FindNodeByString(hChild, rName)) 
				{
					return 1;
				}
			} while((hChild = m_wndRouteTreeListCtrl.GetNextSiblingItem(hChild)) != NULL);
		}
	}

	return 0;
}

/**
	@brief	이름으로 TREEITEM을 찾는다.

	@author KHS	

	@return		
**/
int CCableRouteGroupCreationDlg::FindNodeByStringReverse(HTREEITEM hItem, const CString& rName)
{
	if(m_wndRouteTreeListCtrl.GetItemText(hItem) == rName)
	{
		//! 검색하고자 하는 항목을 찾은 경우..
		m_wndRouteTreeListCtrl.Select(hItem, TVGN_CARET);
		return 1;
	} 
	else
	{
		HTREEITEM hParent = m_wndRouteTreeListCtrl.GetParentItem(hItem);
		if(hParent)
		{ 
			//! 부모노드가 있는 경우, 부모노드를 탐색해야 한다.
			do 
			{
				if(1 == FindNodeByString(hParent, rName)) 
				{
					return 1;
				}
			} while((hParent = m_wndRouteTreeListCtrl.GetNextSiblingItem(hParent)) != NULL);
		}
	}

	return 0;
}
/**
	@brief	이미지 리스트 생성

	@author KHS	

	@return		
**/
int CCableRouteGroupCreationDlg::CreateImageList(void)
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
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_LEFT));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_RIGHT));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_STRAIGHT));

	return ERROR_SUCCESS;
}

/**
	@brief	Route GROUP 데이타를 저장한다.

	@author KHS	

	@return		
**/
int CCableRouteGroupCreationDlg::SaveData(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	
		CELOADTable* pTable = docData.GetTableOf(_T("T_ROUTE_GROUP"));
		if(pTable)
		{
			if(ERROR_SUCCESS != pTable->Save())
			{
				return ERROR_BAD_ENVIRONMENT;
			}
		}
		
		//!변경된 LOAD ITEM PROP 을 DB 에 저장한다.
		if(bDeleteRouteGr)
		{
			if(ERROR_SUCCESS != docData.Save(CLoadItem::TableName(), CLoadItem::TypeString(), AfxGetMainWnd()))
			{
				return ERROR_BAD_ENVIRONMENT;
			}
		}

		return ERROR_SUCCESS;
}

/**
	@brief	SELECT START

	@author KHS	

	@return		
**/
void CCableRouteGroupCreationDlg::OnBnClickedStartButton()
{
	// TODO: Add your control notification handler code here
	HTREEITEM hSelItem = m_wndRouteTreeListCtrl.GetSelectedItem();
	if(hSelItem)
	{
		const CString rRouteType = m_wndRouteTreeListCtrl.GetItemText(hSelItem,0);
		const CString rLength = m_wndRouteTreeListCtrl.GetItemText(hSelItem,1);
		const CString rTD = m_wndRouteTreeListCtrl.GetItemText(hSelItem,2);
		const CString rDirection = m_wndRouteTreeListCtrl.GetItemText(hSelItem,3);
		const CString rDefaultSize = m_wndRouteTreeListCtrl.GetItemText(hSelItem, 4);
		
		CString rRouteItem;
		HTREEITEM hParent = m_wndRouteTreeListCtrl.GetParentItem(hSelItem);
		if(hParent == NULL)
		{
			rRouteItem = rRouteType +"-" + rTD;
		}
		else
		{
			rRouteItem = rRouteType + "-" + rLength +"-" + rTD + "-" + rDirection + "-" + rDefaultSize;
		}

		m_wndStartEdit.SetWindowText(rRouteItem);
		m_wndRouteTreeListCtrl.Select(NULL, TVGN_CARET);
	}
	else
	{
		::MessageBox(NULL, "먼저 Start Route Type을 선택하세요..", "Warning", MB_OK | MB_ICONWARNING);
	}
}

/**
	@brief	SELECT END

	@author KHS	

	@return		
**/
void CCableRouteGroupCreationDlg::OnBnClickedEndButton()
{
	HTREEITEM hSelItem = m_wndRouteTreeListCtrl.GetSelectedItem();
	if(hSelItem)
	{
		CString rSelText = m_wndRouteTreeListCtrl.GetItemText(hSelItem);
		CString rStartText, rParent;
		m_wndStartEdit.GetWindowText(rStartText);
		
		if(rStartText.IsEmpty())
		{
			::MessageBox(NULL, "Start를 먼저 선택하세요", "Warning", MB_OK|MB_ICONWARNING);
			return;
		}
		
		vector<string> oResult;
		CTokenizer<CIsFromString>::Tokenize(oResult, rStartText.operator LPCSTR(), CIsFromString("-"));
		if(oResult.empty()) return;
		
		rStartText = oResult[0].c_str();

		HTREEITEM hParent = m_wndRouteTreeListCtrl.GetParentItem(hSelItem);
		while(rParent != rStartText)
		{
			if(hParent)
			{
				rParent = m_wndRouteTreeListCtrl.GetItemText(hParent);
				hParent = m_wndRouteTreeListCtrl.GetParentItem(hParent);
			}
			else
			{
				::MessageBox(NULL, "Route Type이 다릅니다.", "Warning", MB_OK| MB_ICONWARNING);
				return;
			}
		}
		CString rLength = m_wndRouteTreeListCtrl.GetItemText(hSelItem, 1);
		CString rTD = m_wndRouteTreeListCtrl.GetItemText(hSelItem, 2);
		CString rDirection = m_wndRouteTreeListCtrl.GetItemText(hSelItem, 3);
		CString rDefaultSize = m_wndRouteTreeListCtrl.GetItemText(hSelItem, 4);

		CString rRouteItem;
		rRouteItem = rSelText + "-" + rLength + "-" + rTD + "-" + rDirection + "-" + rDefaultSize;
		m_wndEndEdit.SetWindowText(rRouteItem);
	}
	else
	{
		::MessageBox(NULL, "먼저 End Route Type을 선택하세요..", "Warning", MB_OK | MB_ICONWARNING);
	}
}

/**
	@brief ROUTE TYPE 중복체크

	@author KHS	

	@return		
**/
BOOL CCableRouteGroupCreationDlg::IsExistRouteTypeName(const CString& rRouteGroup)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pTable = docData.GetTableOf(_T("T_ROUTE_GROUP"));
	if(pTable)
	{
		vector<CELOADTable::Record>::iterator where = find_if(pTable->m_RecordEntry.begin(), pTable->m_RecordEntry.end(), bind2nd(MatchRouteGroup(), rRouteGroup.operator LPCSTR()));
		if(where != pTable->m_RecordEntry.end())
		{
			return TRUE;
		}
	}
	return FALSE;
}

/**
	@brief	DISPLAY시에 GROUP의 레코드 중복 체크

	@author KHS	

	@return		
**/
BOOL CCableRouteGroupCreationDlg::FindSiblingNodeByString(HTREEITEM hItem, const CString& rItemText)
{
	if(hItem)
	{
		CString rText = m_wndRouteGroupTreeListCtrl.GetItemText(hItem);
		if(m_wndRouteGroupTreeListCtrl.GetItemText(hItem) == rItemText)
		{
			return TRUE;
		} 
		else
		{
			HTREEITEM hSiblingItem = m_wndRouteGroupTreeListCtrl.GetNextSiblingItem(hItem);
			if(hSiblingItem)
			{
				CString rSiblingText = m_wndRouteGroupTreeListCtrl.GetItemText(hSiblingItem);
				if(FindSiblingNodeByString(hSiblingItem, rItemText))
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

/**
	@brief	WIZARD BACK

	@author KHS	

	@date 2009-05-15 오전 9:41:00	

	@param	

	@return		
**/
void CCableRouteGroupCreationDlg::OnBnClickedWizback()
{
	if(m_bModified)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		if(IDYES == AfxMessageBox(docData.GetStringOf(48).c_str() , MB_YESNO|MB_ICONWARNING ))
		{
			SaveData();
			m_bModified = false;
			GetParent()->SendMessage(WM_COMMAND , ID_WIZBACK, 0);
		}
		else
		{
			GetParent()->SendMessage(WM_COMMAND , ID_WIZBACK, 0);
		}
		
	}
	else
	{
		GetParent()->SendMessage(WM_COMMAND , ID_WIZBACK, 0);
	}
}

/**
	@brief	WIZARD NEXT

	@author KHS	

	@date 2009-05-15 오전 9:41:02	

	@param	

	@return		
**/
void CCableRouteGroupCreationDlg::OnBnClickedWiznext()
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

BOOL CCableRouteGroupCreationDlg::Init(void)
{
	m_wndRouteTreeListCtrl.DeleteAllItems();
	m_wndRouteGroupTreeListCtrl.DeleteAllItems();

	if(ERROR_SUCCESS != DisplayRouteTreeCtrl())
	{
		return FALSE;
	}

	if(ERROR_SUCCESS != DisplayRouteGroupTreeCTrl())
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CCableRouteGroupCreationDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE) return TRUE;
		if(pMsg->wParam == VK_RETURN) return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

/**
	@brief	CREATE ROUTE TYPE TREE LIST CTRL

	@author KHS	

	@date 2009-05-15 오후 1:03:52	

	@param	

	@return		
**/
int CCableRouteGroupCreationDlg::CreateRouteTypeTreeListCtrl(void)
{
	static const CString __HEADERS__[] = {_T("Route Type"), _T("Route Length") , _T("T/D") , _T("Direction"),_T("Default Size")};

	CRect rect;
	CPoint point(0,0);
	GetDlgItem(IDC_ROUTE_TYPE_STATIC)->GetWindowRect(&rect);
	ScreenToClient(&rect);

	unsigned uStyle	= TVS_HASBUTTONS|TVS_HASLINES|TVS_FULLROWSELECT|TVS_NONEVENHEIGHT/*|TVS_CHECKBOXES*/;
	unsigned uExStyle = TVS_EX_SUBSELECT|TVS_EX_FULLROWMARK;

	if(m_wndRouteTreeListCtrl.Create(uStyle|WS_CHILD|WS_VISIBLE|WS_BORDER,rect,this, 123))
	{
		m_wndRouteTreeListCtrl.InsertColumn(0, __HEADERS__[0], LVCFMT_LEFT, 240);
		m_wndRouteTreeListCtrl.InsertColumn(1, __HEADERS__[1], LVCFMT_LEFT, 90);
		m_wndRouteTreeListCtrl.InsertColumn(2, __HEADERS__[2], LVCFMT_LEFT, 40);
		m_wndRouteTreeListCtrl.InsertColumn(3, __HEADERS__[3], LVCFMT_LEFT, 70);
		m_wndRouteTreeListCtrl.InsertColumn(4, __HEADERS__[4], LVCFMT_LEFT, 80);

		m_wndRouteTreeListCtrl.SetExtendedStyle(uExStyle);

		return ERROR_SUCCESS;
	}
	

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	CREATE ROUTE GROUP TREE LIST CTRL

	@author KHS	

	@date 2009-05-15 오후 1:03:52	

	@param	

	@return		
**/
int CCableRouteGroupCreationDlg::CreateRouteGroupTreeListCtrl(void)
{
	static const CString __HEADERS__[] = {_T("Route Gr."), _T("Desc"),_T("Length") , _T("T/D") , _T("Direction"),_T("Default Size")};

	CRect rect;
	CPoint point(0,0);
	GetDlgItem(IDC_ROUTE_GROUP_STATIC)->GetWindowRect(&rect);
	ScreenToClient(&rect);

	unsigned uStyle	= TVS_HASBUTTONS|TVS_HASLINES|TVS_FULLROWSELECT|TVS_NONEVENHEIGHT/*|TVS_CHECKBOXES*/;
	unsigned uExStyle = TVS_EX_SUBSELECT|TVS_EX_FULLROWMARK;

	if(m_wndRouteGroupTreeListCtrl.Create(uStyle|WS_CHILD|WS_VISIBLE|WS_BORDER,rect,this, 123))
	{
		m_wndRouteGroupTreeListCtrl.InsertColumn(0, __HEADERS__[0], LVCFMT_LEFT, 80);
		m_wndRouteGroupTreeListCtrl.InsertColumn(1, __HEADERS__[1], LVCFMT_LEFT, 90);
		m_wndRouteGroupTreeListCtrl.InsertColumn(2, __HEADERS__[2], LVCFMT_LEFT, 60);
		m_wndRouteGroupTreeListCtrl.InsertColumn(3, __HEADERS__[3], LVCFMT_LEFT, 40);
		m_wndRouteGroupTreeListCtrl.InsertColumn(4, __HEADERS__[4], LVCFMT_LEFT, 70);
		m_wndRouteGroupTreeListCtrl.InsertColumn(5, __HEADERS__[5], LVCFMT_LEFT, 80);

		m_wndRouteGroupTreeListCtrl.SetExtendedStyle(uExStyle);

		return ERROR_SUCCESS;
	}
	

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	Delete Menu

	@author KHS	

	@date 2009-07-16 오후 4:28:13	

	@param	

	@return		
**/
void CCableRouteGroupCreationDlg::OnCableRouteDelete()
{
	HTREEITEM hSelItem = m_wndRouteGroupTreeListCtrl.GetSelectedItem();
	if(hSelItem)
	{
		CString rRouteGroupItem;
		HTREEITEM hParent = m_wndRouteGroupTreeListCtrl.GetParentItem(hSelItem);
		if(NULL == hParent)
		{
			rRouteGroupItem = m_wndRouteGroupTreeListCtrl.GetItemText(hSelItem);
			m_wndRouteGroupTreeListCtrl.DeleteItem(hSelItem);
		}
		else
		{
			rRouteGroupItem = m_wndRouteGroupTreeListCtrl.GetItemText(hParent);
			m_wndRouteGroupTreeListCtrl.DeleteItem(hParent);
		}
		CELoadDocData& docData = CELoadDocData::GetInstance();
		CELOADTable* pTable = docData.GetTableOf(_T("T_ROUTE_GROUP"));
		list<CELoadItem*> LoadItemList;
		docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
		if(pTable)
		{
			//! 삭제 될 RECORD를 찾는다.
			vector<CELOADTable::Record> DELETE_ROUTE_GROUP_ENTRY;
			for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin(); itr != pTable->m_RecordEntry.end();++itr)
			{
				if(rRouteGroupItem == itr->FieldValueMap[_T("C_ROUTE_GROUP")].c_str())
				{
					DELETE_ROUTE_GROUP_ENTRY.push_back(*itr);

					//!ASSIGN 된 LOAD 들에서 삭제 된 ROUTE_GROUP을 ASSIGN 한놈들을 초기화
					//if(pLoadItemList)
					{
						for(list<CELoadItem*>::iterator jtr = LoadItemList.begin(); jtr != LoadItemList.end();++jtr)
						{
							if(rRouteGroupItem == (*jtr)->prop()->GetValue(_T("Cable Route"), _T("Route Group")).c_str())
							{
								(*jtr)->prop()->SetValue(_T("Cable Route"), _T("Route Group"), _T(""));
								(*jtr)->prop()->SetOriginalValue(_T("Cable Route"), _T("Route Group"), _T(""));
							}
						}
					}
				}
			}
			//!찾은 RECORD를 삭제한다.
			for(vector<CELOADTable::Record>::iterator jtr = DELETE_ROUTE_GROUP_ENTRY.begin(); jtr != DELETE_ROUTE_GROUP_ENTRY.end();++jtr)
			{
				vector<CELOADTable::Record>::iterator where = find_if(pTable->m_RecordEntry.begin(), pTable->m_RecordEntry.end(), bind2nd(MatchRouteGroup(), rRouteGroupItem.operator LPCSTR()));	
				if(where != pTable->m_RecordEntry.end())
				{
					pTable->m_RecordEntry.erase(where);
				}
			}
		}
		bDeleteRouteGr = true;
		m_bModified = true;
	}
	else
	{
		::MessageBox(NULL, "Route Group을 선택하세요 !!", "Warning" , MB_OK|MB_ICONWARNING);
	}
}
