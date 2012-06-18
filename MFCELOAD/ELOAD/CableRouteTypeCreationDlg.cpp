// CableRouteTypeCreationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "CableRouteTypeCreationDlg.h"
#include "ELoadDocData.h"

// CCableRouteTypeCreationDlg dialog

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

/*
struct MatchRouteType : public binary_function<CELoadItem*, string, bool>
{
        bool operator()(const CELoadItem* pItem , const string& rhs) const
	{
		return (rhs == pItem->prop()->GetValue(_T("General"), _T("Route ID")));
		
	}
};

struct MatchRouteGroup : public binary_function<CELOADTable::Record, string, bool>
{
        bool operator()(const CELOADTable::Record lhs , const string& rhs) const
	{
		const map<string,string>::const_iterator where = lhs.FieldValueMap.find(_T("C_ROUTE_GROUP"));
		if(where != lhs.FieldValueMap.end())
		{
			return (rhs == where->second);
		}

		return false;
	}
};
*/

IMPLEMENT_DYNAMIC(CCableRouteTypeCreationDlg, CWizardPropertyPage)

CCableRouteTypeCreationDlg::CCableRouteTypeCreationDlg(CWnd* pParent /*=NULL*/)
	: CWizardPropertyPage(CCableRouteTypeCreationDlg::IDD/*, pParent*/)
{
	m_bDeleted =  false;
	m_bModified = false;
}

CCableRouteTypeCreationDlg::~CCableRouteTypeCreationDlg()
{
}

void CCableRouteTypeCreationDlg::DoDataExchange(CDataExchange* pDX)
{
	CWizardPropertyPage::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_ROUTE_TYPE_TREE, m_wndRouteTreeListCtrl);
	DDX_Control(pDX, IDC_ROUTE_TYPE_EDIT, m_wndRouteTypeEdit);
	DDX_Control(pDX, IDC_ROUTE_LENGTH_EDIT, m_wndRouteLengthEdit);
	DDX_Control(pDX, IDC_DIRECTION_COMBO, m_wndRouteDirectionCombo);
	DDX_Control(pDX, IDC_ADD_BUTTON, m_wndAddButton);
	DDX_Control(pDX, IDOK, m_wndOkButton);
	DDX_Control(pDX, IDCANCEL, m_wndCancelButton);
	DDX_Control(pDX, IDC_TD_COMBO, m_wndTdCombo);
	DDX_Control(pDX, IDC_DEFAULT_SIZE_COMBO, m_wndDefaultsizeCombo);
	DDX_Control(pDX, ID_WIZBACK, m_wndBackButton);
	DDX_Control(pDX, ID_WIZNEXT, m_wndNextButton);
}


BEGIN_MESSAGE_MAP(CCableRouteTypeCreationDlg, CDialog)
	ON_BN_CLICKED(IDC_ADD_BUTTON, &CCableRouteTypeCreationDlg::OnBnClickedAddButton)
	ON_BN_CLICKED(IDOK, &CCableRouteTypeCreationDlg::OnBnClickedOk)
	ON_NOTIFY(NM_CLICK, IDC_ROUTE_TYPE_TREE, &CCableRouteTypeCreationDlg::OnNMClickRouteTypeTree)
	ON_BN_CLICKED(ID_WIZBACK, &CCableRouteTypeCreationDlg::OnBnClickedWizback)
	ON_BN_CLICKED(ID_WIZNEXT, &CCableRouteTypeCreationDlg::OnBnClickedWiznext)
	ON_BN_CLICKED(IDCANCEL, &CCableRouteTypeCreationDlg::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_TD_COMBO, &CCableRouteTypeCreationDlg::OnCbnSelchangeTdCombo)
	ON_COMMAND(ID_CABLE_ROUTE_DELETE, &CCableRouteTypeCreationDlg::OnCableRouteDelete)
END_MESSAGE_MAP()


// CCableRouteTypeCreationDlg message handlers

BOOL CCableRouteTypeCreationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	//! 이미지 리슽 생성
	CreateImageList();
	
	if(ERROR_SUCCESS != CreateRouteTypeTreeListCtrl())
	{
		TRACE0("Failed to create TreeList Ctrl \n");
		return FALSE;
	}

	m_wndRouteTreeListCtrl.SetImageList(&m_imgList, LVSIL_NORMAL);
	m_wndRouteTreeListCtrl.SetDeleteEvent(true);

	if(ERROR_SUCCESS != DisplayRouteTreeCtrl())
	{
		return FALSE;
	}

	if(ERROR_SUCCESS != InitEditCtrl())
	{
		return FALSE;
	}

	CELoadDocData& docData = CELoadDocData::GetInstance();
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));

	//! BUTTON SETTING
	m_wndOkButton.SetTextHotColor(TextHotColor);
	m_wndCancelButton.SetTextHotColor(TextHotColor);
	m_wndAddButton.SetTextHotColor(TextHotColor);

	m_wndBackButton.SetTextHotColor(TextHotColor);
	m_wndNextButton.SetTextHotColor(TextHotColor);

	m_wndBackButton.SetIcon(IDI_PRE);
	m_wndNextButton.SetIcon(IDI_NEXT);

	m_wndOkButton.SetIcon(IDI_OK_18X18);
	m_wndCancelButton.SetIcon(IDI_CANCEL_18X18);
	m_wndAddButton.SetIcon(IDI_ADD);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	 ADD ITEM

	@author KHS	

	@return		
**/
void CCableRouteTypeCreationDlg::OnBnClickedAddButton()
{
	// TODO: Add your control notification handler code here
	CString rRouteType, rLength, rTD, rDirection, rDefaultSize;

	m_wndRouteTypeEdit.GetWindowText(rRouteType);
	if(rRouteType.IsEmpty())
	{
		::MessageBox(NULL, "Route Type을 입력하세요 !", "Warning", MB_OK|MB_ICONWARNING);
		return;
	}

	if(IsExistRouteTypeName(rRouteType))
	{
		::MessageBox(NULL, "Route Type이 이미 존재합니다!!", "Warning", MB_OK|MB_ICONINFORMATION );
		return;
	}

	int nItem = m_wndTdCombo.GetCurSel();
	if( -1 != nItem)
	{
		m_wndTdCombo.GetLBText(nItem, rTD);
	}

	m_wndRouteLengthEdit.GetWindowText(rLength);

	if(rLength.IsEmpty() && rTD != _T("Start"))
	{
		::MessageBox(NULL, "Route Length를 입력하세요 !", "Warning", MB_OK|MB_ICONWARNING);
		return;
	}
	
	nItem = m_wndRouteDirectionCombo.GetCurSel();
	if(-1 != nItem)
	{
		m_wndRouteDirectionCombo.GetLBText(nItem, rDirection);
	}
	
	if(CheckDirection(rDirection))
	{
		::MessageBox(NULL, "같은 Direction이 이미 존재합니다 !", "Warning", MB_OK|MB_ICONWARNING);
		return;
	}

	nItem = m_wndDefaultsizeCombo.GetCurSel();
	if(-1 != nItem)
	{
		m_wndDefaultsizeCombo.GetLBText(nItem, rDefaultSize);
	}
	
	HTREEITEM hSelItem = m_wndRouteTreeListCtrl.GetSelectedItem();
	if(hSelItem == NULL && rTD != _T("Start")) 
	{
		::MessageBox(NULL, "Select Point !!", "Warning", MB_OK|MB_ICONWARNING);
		return;
	}

	CELoadDocData& docData = CELoadDocData::GetInstance();
	CRouteItem* pRouteItem = static_cast<CRouteItem*>(docData.CreateELoadItemOf(CRouteItem::TypeString()));
	if(pRouteItem)
	{
		//! START POINT
		if(rTD == _T("Start"))
		{
			pRouteItem->prop()->SetValue(_T("General"),_T("Route ID"), rRouteType.operator LPCSTR());
			pRouteItem->prop()->SetOriginalValue(_T("General"),_T("Route ID"), rRouteType.operator LPCSTR());

			pRouteItem->prop()->SetValue(_T("General"),_T("Route Length"), _T(""));
			pRouteItem->prop()->SetOriginalValue(_T("General"),_T("Route Length"), _T(""));

			pRouteItem->prop()->SetValue(_T("General"),_T("TD"), rTD.operator LPCSTR());
			pRouteItem->prop()->SetOriginalValue(_T("General"),_T("TD"), rTD.operator LPCSTR());

			pRouteItem->prop()->SetValue(_T("General"),_T("Default Size"), _T(""));
			pRouteItem->prop()->SetOriginalValue(_T("General"),_T("Default Size"), _T(""));

			pRouteItem->prop()->SetValue(_T("General"),_T("Parent"), _T("ROOT"));
			pRouteItem->prop()->SetOriginalValue(_T("General"),_T("Parent"), _T("ROOT"));

			AddRootTrayItem(rRouteType, rTD);
		}
		else
		{
			const CString rParent = m_wndRouteTreeListCtrl.GetItemText(hSelItem);
			
			pRouteItem->prop()->SetValue(_T("General"),_T("Route ID"), rRouteType.operator LPCSTR());
			pRouteItem->prop()->SetOriginalValue(_T("General"),_T("Route ID"), rRouteType.operator LPCSTR());

			pRouteItem->prop()->SetValue(_T("General"),_T("Route Length"), rLength.operator LPCSTR());
			pRouteItem->prop()->SetOriginalValue(_T("General"),_T("Route Length"), rLength.operator LPCSTR());

			pRouteItem->prop()->SetValue(_T("General"),_T("TD"), rTD.operator LPCSTR());
			pRouteItem->prop()->SetOriginalValue(_T("General"),_T("TD"), rTD.operator LPCSTR());

			pRouteItem->prop()->SetValue(_T("General"),_T("Direction"), rDirection.operator LPCSTR());
			pRouteItem->prop()->SetOriginalValue(_T("General"),_T("Direction"), rDirection.operator LPCSTR());

			pRouteItem->prop()->SetValue(_T("General"),_T("Default Size"), rDefaultSize.operator LPCSTR());
			pRouteItem->prop()->SetOriginalValue(_T("General"),_T("Default Size"), rDefaultSize.operator LPCSTR());

			pRouteItem->prop()->SetValue(_T("General"),_T("Parent"), rParent.operator LPCSTR());
			pRouteItem->prop()->SetOriginalValue(_T("General"),_T("Parent"), rParent.operator LPCSTR());

			AddChildTrayItem(hSelItem, rRouteType, rLength, rTD, rDirection, rDefaultSize);
		}
		m_bModified = true;
	}
	ClearEdit();
}

/**
	@brief	EDIT CTRL 초기화

	@author KHS	

	@return		
**/
int CCableRouteTypeCreationDlg::InitEditCtrl(void)
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

		//! ROUTE TYPE
		m_wndRouteTypeEdit.SetRegEx(rTextRegEx.c_str());
		//m_wndRouteTypeEdit.CreateToolTip(this , rToolTip);
		m_wndRouteTypeEdit.SetWavyLineColour (WavyLineColor);
		m_wndRouteTypeEdit.SetBackgroundColourFocus (BackFocusColor);
		m_wndRouteTypeEdit.SetForegroundColourError (TextErrorColor);
		m_wndRouteTypeEdit.SetBackgroundColourError (BackErrorColor);

		//! ROUTE LENGTH
		m_wndRouteLengthEdit.SetRegEx(rNumberRegEx.c_str());
		//m_wndRouteLengthEdit.CreateToolTip(this , rToolTip);
		m_wndRouteLengthEdit.SetWavyLineColour (WavyLineColor);
		m_wndRouteLengthEdit.SetBackgroundColourFocus (BackFocusColor);
		m_wndRouteLengthEdit.SetForegroundColourError (TextErrorColor);
		m_wndRouteLengthEdit.SetBackgroundColourError (BackErrorColor);

		InitDirectionCombo();
		InitDefaultSizeCombo();
		InitTdCombo();
	}
	catch(...)
	{
		return ERROR_BAD_ENVIRONMENT;
	}

	return ERROR_SUCCESS;
}

/**
	@brief	DIRECTION COMBO BOX 초기화

	@author KHS	

	@return		
**/
int CCableRouteTypeCreationDlg::InitDirectionCombo(void)
{
	const CString __COMBO__[] = {_T("Straight"),_T("Left"),_T("Right")};

	for(int i =0;i < 3;i++)
	{
		m_wndRouteDirectionCombo.AddString(__COMBO__[i]);
	}

	m_wndRouteDirectionCombo.SetCurSel(2);

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author KHS	

	@return		
**/
HTREEITEM CCableRouteTypeCreationDlg::AddRootTrayItem(const CString& rRouteType, const CString& rTD)
{
	TV_INSERTSTRUCT TI;
	TI.hParent      = NULL;
	TI.hInsertAfter = TVI_SORT;
	TI.item.iImage  = 0;
	TI.item.iSelectedImage = 0;
	TI.item.mask    = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	TI.item.pszText = (LPSTR)(rRouteType.operator LPCSTR());

	HTREEITEM hItem = m_wndRouteTreeListCtrl.InsertItem(&TI);
	
	m_wndRouteTreeListCtrl.SetItemText(hItem, rTD, 1);

	m_wndRouteTreeListCtrl.Expand(hItem, TVE_EXPAND);

	return hItem;
}

/**
	@brief	

	@author KHS

	@return		
**/
HTREEITEM CCableRouteTypeCreationDlg::AddChildTrayItem(HTREEITEM hParent, const CString& rRouteType, const CString& rLength, const CString& rTD, const CString& rDirection, const CString& rDefaultSize)
{
	int nImage = 1;

	if( "Left" == rDirection )	nImage = 3;
	else if("Right" == rDirection)	nImage = 4;
	else				nImage = 5;
	
	TV_INSERTSTRUCT TI;
	TI.hParent      = hParent;
	TI.item.iImage  = nImage;
	TI.item.iSelectedImage = nImage;
	TI.item.mask    = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	TI.item.pszText = (LPSTR)(rRouteType.operator LPCSTR());

	HTREEITEM hItem = m_wndRouteTreeListCtrl.InsertItem(&TI);
	m_wndRouteTreeListCtrl.SetItemText(hItem, rTD,1);
	m_wndRouteTreeListCtrl.SetItemText(hItem, rLength, 2);
	m_wndRouteTreeListCtrl.SetItemText(hItem, rDirection, 3);
	m_wndRouteTreeListCtrl.SetItemText(hItem, rDefaultSize, 4);

	m_wndRouteTreeListCtrl.Expand(hItem, TVE_EXPAND);

	return hItem;
}

/**
	@brief	Route Type 데이타를 저장한다.

	@author KHS	

	@return		
**/
int CCableRouteTypeCreationDlg::SaveData(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	
	//! Route Item 
	if(ERROR_SUCCESS != docData.Save(CRouteItem::TableName(), CRouteItem::TypeString(), AfxGetMainWnd()))
	{
		return ERROR_BAD_ENVIRONMENT;
	}

	if(m_bDeleted)
	{
		//!변경된 LOAD ITEM PROP 을 DB 에 저장한다.
		if(ERROR_SUCCESS != docData.Save(CLoadItem::TableName(), CLoadItem::TypeString(), AfxGetMainWnd()))
		{
			return ERROR_BAD_ENVIRONMENT;
		}
	
		//!변경된 RouteGroup 저장
		CELOADTable* pTable = docData.GetTableOf(_T("T_ROUTE_GROUP"));
		if(pTable)
		{
			if(ERROR_SUCCESS != pTable->Save())
			{
				return ERROR_BAD_ENVIRONMENT;
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	Route Type Data를 Tree에 뿌린다.

	@author KHS	

	@return		
**/
int CCableRouteTypeCreationDlg::DisplayRouteTreeCtrl(void)
{
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
			if("ROOT" == (*jtr)->prop()->GetValue(_T("General"),_T("Parent")))
			{
				const string rRouteType = (*jtr)->prop()->GetValue(_T("General"),_T("Route ID"));
				//const string rLength = (*jtr)->prop()->GetValue(_T("General"),_T("Route Length"));
				const string rTD = (*jtr)->prop()->GetValue(_T("General"),_T("TD"));
				//const string rDefaultSize = (*jtr)->prop()->GetValue(_T("General"),_T("Default Size"));

				hRoot = AddRootTrayItem(rRouteType.c_str(), rTD.c_str());

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

					int res = FindNodeByString(hRoot, rParent.c_str());
					if(1 == res)
					{
						HTREEITEM hItem = m_wndRouteTreeListCtrl.GetSelectedItem();
						if(hItem)
						{
							AddChildTrayItem(hItem, rRouteType.c_str(), rLength.c_str(), rTD.c_str(), rDirection.c_str(), rDefaultSize.c_str());
						}
					}
				}
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	입력 창을 초기화

	@author KHS	

	@return		
**/
int CCableRouteTypeCreationDlg::ClearEdit(void)
{
	m_wndRouteTypeEdit.ClearText();
	m_wndRouteLengthEdit.ClearText();

	int nItem = m_wndDefaultsizeCombo.FindString(0, "900");
	if(-1 != nItem)
	{
		m_wndDefaultsizeCombo.SetCurSel(nItem);	
	}

	nItem  = m_wndTdCombo.FindString(0, "Tray");
	if(-1 != nItem)
	{
		m_wndTdCombo.SetCurSel(nItem);
	}

	m_wndRouteLengthEdit.EnableWindow(TRUE);
	m_wndRouteDirectionCombo.EnableWindow(TRUE);
	m_wndDefaultsizeCombo.EnableWindow(TRUE);

	return ERROR_SUCCESS;
}

/**
	@brief	이름으로 TREEITEM을 찾는다.

	@author KHS	

	@return		
**/
int CCableRouteTypeCreationDlg::FindNodeByString(HTREEITEM hItem, const CString& rName)
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
	@brief	SAVE DATA

	@author KHS	

	@return		
**/
void CCableRouteTypeCreationDlg::OnBnClickedOk()
{
	if(ERROR_SUCCESS == SaveData())
	{
		::MessageBox(NULL, "SUCCESS!!", "Saved", MB_OK|MB_ICONINFORMATION );
		m_bDeleted = false;
		m_bModified = false;
	}
}

/**
	@brief	삭제 시 TREE에서 CHILD Item을 찾아서 있으면 T_ROUTE 테이블에서 삭제한다.

	@author KHS	

	@return		
**/
BOOL CCableRouteTypeCreationDlg::DeleteSelectedItem(HTREEITEM hItem, const CString& rItemText)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	list<CELoadItem*> RouteItemList;
	docData.GetELoadItemListOf(RouteItemList , CRouteItem::TypeString());

	list<CELoadItem*> TrayItemList;
	docData.GetELoadItemListOf(TrayItemList , CTrayItem::TypeString());

	list<CELoadItem*> CableItemList;
	docData.GetELoadItemListOf(CableItemList , CCableItem::TypeString());
	//if(pRouteItemList)
	{
		for(list<CELoadItem*>::iterator itr = RouteItemList.begin(); itr != RouteItemList.end();++itr)
		{
			string rRouteItem;

			const string rRouteType = (*itr)->prop()->GetValue(_T("General"),_T("Route ID"));
			const string rLength = (*itr)->prop()->GetValue(_T("General"),_T("Route Length"));
			const string rTD = (*itr)->prop()->GetValue(_T("General"),_T("TD"));
			const string rDirection = (*itr)->prop()->GetValue(_T("General"),_T("Direction"));
			const string rDefaultSize = (*itr)->prop()->GetValue(_T("General"),_T("Default Size"));
			const string rParent = (*itr)->prop()->GetValue(_T("General"),_T("Parent"));

			if(rItemText == rRouteType.c_str())
			{
				//!ROUTE의 TRAY들도 모두 삭제
				//if(pTrayItemList)
				{
					for(list<CELoadItem*>::iterator jtr = TrayItemList.begin(); jtr != TrayItemList.end();++jtr)
					{
						if(rItemText == (*jtr)->prop()->GetValue(_T("General"),_T("Route ID")).c_str())
						{
							const string rTrayId = (*jtr)->prop()->GetValue(_T("General"), _T("Tray ID"));
							SAFE_DELETE(*jtr);
							TrayItemList.erase(jtr);
							jtr = TrayItemList.begin();

							//! CABLE ITEM의 TRAY LIST에서 삭제된 TRAY를 제거???해야하나??
							//if(pCableItemList)
							{
								for(list<CELoadItem*>::iterator ktr = CableItemList.begin(); ktr != CableItemList.end();++ktr)
								{
									string rTrayList = (*ktr)->prop()->GetValue(_T("Tray"), _T("Tray List"));
									if(!rTrayList.empty())
									{
										vector<string> oResult;
										CTokenizer<CIsComma>::Tokenize(oResult, rTrayList, CIsComma());
										if(!oResult.empty())
										{
											vector<string>::iterator where = find(oResult.begin(), oResult.end(), rTrayId);
											if(where != oResult.end())
											{
												oResult.erase(where);
												CString rValue;
												for(int i = 0;i < oResult.size();++i)
												{
													rValue += (CString(oResult[i].c_str()) + ",");
												}
												if(rValue.Right(1) == ',')	rValue = rValue.Left(rValue.GetLength() -1);
												(*ktr)->prop()->SetValue(_T("Tray"), _T("Tray List"), rValue.operator LPCSTR());
												(*ktr)->prop()->SetOriginalValue(_T("Tray"), _T("Tray List"), rValue.operator LPCSTR());
											}
										}
									}
								}
							}
						}
						
					}
				}
				(*itr)->SetDeleteFlag(true);
				//SAFE_DELETE(*itr);
				//pRouteItemList->erase(itr);

				HTREEITEM hChild = m_wndRouteTreeListCtrl.GetChildItem(hItem);
				if(hChild)
				{
					do 
					{
						CString rName = m_wndRouteTreeListCtrl.GetItemText(hChild);
						if(TRUE == DeleteSelectedItem(hChild, rName)) 
						{
							return TRUE;
						}
					} while((hChild = m_wndRouteTreeListCtrl.GetNextSiblingItem(hChild)) != NULL);
				}
				break;
			}
		}
	}
	return FALSE;
}

/**
	@brief	이미지 리스트 생성

	@author KHS	

	@return		
**/
int CCableRouteTypeCreationDlg::CreateImageList(void)
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
	@brief	TREE 에서 빈공간을 클릭시 SELECTED ITEM 해제

	@author KHS	

	@return		
**/
void CCableRouteTypeCreationDlg::OnNMClickRouteTypeTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	
	if(pTVDispInfo->item.hItem == NULL)
	{
		m_wndRouteTreeListCtrl.Select(NULL,TVGN_CARET);
	}

	*pResult = 0;
}

/**
	@brief ROUTE TYPE 중복체크

	@author KHS	

	@return		
**/
BOOL CCableRouteTypeCreationDlg::IsExistRouteTypeName(const CString& rRouteTypeName)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> RouteItemList;
	docData.GetELoadItemListOf(RouteItemList , CRouteItem::TypeString());
	//if(pRouteItemList)
	{
		list<CELoadItem*>::iterator where = find_if(RouteItemList.begin(), RouteItemList.end(), bind2nd(MatchRouteType(), rRouteTypeName.operator LPCSTR()));
		if(where != RouteItemList.end())
		{
			return TRUE;
		}
	}

	return FALSE;
}

/**
	@brief	

	@author KHS	

	@date 2009-05-14 오후 7:05:49	

	@param	

	@return		
**/
int CCableRouteTypeCreationDlg::InitTdCombo(void)
{
	const CString __COMBO__[] = {_T("Tray"),_T("Duct"),_T("Start")};

	for(int i =0;i < 3;i++)
	{
		m_wndTdCombo.AddString(__COMBO__[i]);
	}

	m_wndTdCombo.SetCurSel(2);

	return ERROR_SUCCESS;
}
/**
	@brief	

	@author KHS	

	@date 2009-05-14 오후 7:05:46	

	@param	

	@return		
**/
int CCableRouteTypeCreationDlg::InitDefaultSizeCombo(void)
{
	const CString __COMBO__[] = {_T("900"),_T("600"),_T("430"),_T("300")};

	for(int i =0;i < 4;i++)
	{
		m_wndDefaultsizeCombo.AddString(__COMBO__[i]);
	}

	m_wndDefaultsizeCombo.SetCurSel(3);

	return ERROR_SUCCESS;
}

/**
	@brief	WIZARD BACK

	@author KHS	

	@date 2009-05-15 오전 9:38:15	

	@param	

	@return		
**/
void CCableRouteTypeCreationDlg::OnBnClickedWizback()
{
	GetParent()->SendMessage(WM_COMMAND , ID_WIZBACK , 0);
}

/**
	@brief	WIAZRD NEXT

	@author KHS	

	@date 2009-05-15 오전 9:38:18	

	@param	

	@return		
**/
void CCableRouteTypeCreationDlg::OnBnClickedWiznext()
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
	@brief	WIZARD INIT

	@author KHS	

	@date 2009-05-15 오전 10:04:42	

	@param	

	@return		
**/
BOOL CCableRouteTypeCreationDlg::Init(void)
{
	/*m_wndRouteTreeListCtrl.DeleteAllItems();

	if(ERROR_SUCCESS != DisplayRouteTreeCtrl())
	{
		return FALSE;
	}*/
	return TRUE;
}

void CCableRouteTypeCreationDlg::OnBnClickedCancel()
{
	if(m_bModified)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		if(IDYES == AfxMessageBox(docData.GetStringOf(48).c_str() , MB_YESNO|MB_ICONWARNING ))
		{
			SaveData();
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
	@brief	

	@author KHS	

	@date 2009-05-15 오전 10:26:52	

	@param	

	@return		
**/
int CCableRouteTypeCreationDlg::CreateRouteTypeTreeListCtrl(void)
{
	static const CString __HEADERS__[] = {_T("Route Type"), _T("T/D"), _T("Route Length") , _T("Direction"),_T("Default Size")};

	CRect rect;
	CPoint point(0,0);
	GetDlgItem(IDC_ROUTE_TYPE_STATIC)->GetWindowRect(&rect);
	ScreenToClient(&rect);

	unsigned uStyle	= TVS_HASBUTTONS|TVS_HASLINES|TVS_FULLROWSELECT|TVS_NONEVENHEIGHT/*|TVS_CHECKBOXES*/;
	unsigned uExStyle = TVS_EX_SUBSELECT|TVS_EX_FULLROWMARK;

	if(m_wndRouteTreeListCtrl.Create(uStyle|WS_CHILD|WS_VISIBLE|WS_BORDER,rect,this, 123))
	{
		m_wndRouteTreeListCtrl.InsertColumn(0, __HEADERS__[0], LVCFMT_LEFT, 240);
		m_wndRouteTreeListCtrl.InsertColumn(1, __HEADERS__[1], LVCFMT_LEFT, 60);
		m_wndRouteTreeListCtrl.InsertColumn(2, __HEADERS__[2], LVCFMT_LEFT, 90);
		m_wndRouteTreeListCtrl.InsertColumn(3, __HEADERS__[3], LVCFMT_LEFT, 80);
		m_wndRouteTreeListCtrl.InsertColumn(4, __HEADERS__[4], LVCFMT_LEFT, 80);

		m_wndRouteTreeListCtrl.SetExtendedStyle(uExStyle);

		return ERROR_SUCCESS;
	}
	

	return ERROR_BAD_ENVIRONMENT;
}

BOOL CCableRouteTypeCreationDlg::PreTranslateMessage(MSG* pMsg)
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
	@brief	Route 에 같은 방향이 중복될 수 없음을 체크

	@author KHS	

	@date 2009-06-15 오전 10:05:06	

	@param	

	@return		
**/
BOOL CCableRouteTypeCreationDlg::CheckDirection(const CString& rDirection)
{
	HTREEITEM hSelItem = m_wndRouteTreeListCtrl.GetSelectedItem();
	if(hSelItem)
	{
		HTREEITEM hChild = m_wndRouteTreeListCtrl.GetChildItem(hSelItem);
		if(hChild)
		{
			do
			{
				CString rText = m_wndRouteTreeListCtrl.GetItemText(hChild, 3);
				if(rDirection == rText) return TRUE;
				
			}while((hChild = m_wndRouteTreeListCtrl.GetNextSiblingItem(hChild)) != NULL);
		}
	}

	return FALSE;
}

/**
	@brief	 TRAY / DUCT Combo Box Sel Change

	@author KHS	

	@date 2009-06-15 오후 1:41:23	

	@param	

	@return		
**/
void CCableRouteTypeCreationDlg::OnCbnSelchangeTdCombo()
{
	// TODO: Add your control notification handler code here
	const int nItem = m_wndTdCombo.GetCurSel();
	if(-1 != nItem)
	{
		CString rItemText;
		m_wndTdCombo.GetLBText(nItem, rItemText);
		if(rItemText == "Start")
		{
			m_wndRouteLengthEdit.EnableWindow(FALSE);
			m_wndRouteDirectionCombo.EnableWindow(FALSE);
			m_wndDefaultsizeCombo.EnableWindow(FALSE);

		}
		else
		{
			m_wndRouteLengthEdit.EnableWindow(TRUE);
			m_wndRouteDirectionCombo.EnableWindow(TRUE);
			m_wndDefaultsizeCombo.EnableWindow(TRUE);
		}
	}
}

/**
	@brief	

	@author KHS	

	@date 2009-07-14 오후 3:22:17	

	@param	

	@return		
**/
void CCableRouteTypeCreationDlg::OnCableRouteDelete()
{
	HTREEITEM hSelItem = m_wndRouteTreeListCtrl.GetSelectedItem();
	if(hSelItem)
	{
		CString rSelItemText = m_wndRouteTreeListCtrl.GetItemText(hSelItem);

		//!ROUTE_TYPE을 삭제
		DeleteSelectedItem(hSelItem, rSelItemText);
		m_wndRouteTreeListCtrl.DeleteItem(hSelItem);

		CELoadDocData& docData = CELoadDocData::GetInstance();
		{
			//! Route Type을 삭제하면 Group에 들어가있는 Route Type을 찾아서 해당 Group도 모두 지운다.
			CELOADTable* pTable = docData.GetTableOf(_T("T_ROUTE_GROUP"));
			list<CELoadItem*> LoadItemList;
			docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
			if(pTable)
			{
				vector<string> RouteGroupEntry;
				{
					for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin(); itr != pTable->m_RecordEntry.end();++itr)
					{
						if(rSelItemText == itr->FieldValueMap[_T("C_ROUTE_TYPE")].c_str())
						{
							const string rRouteGroup = itr->FieldValueMap[_T("C_ROUTE_GROUP")];
							RouteGroupEntry.push_back(rRouteGroup);

							//!ASSIGN 된 LOAD 들에서 삭제 된 ROUTE_GROUP을 ASSIGN 한놈들을 초기화
							//if(pLoadItemList)
							{
								for(list<CELoadItem*>::iterator jtr = LoadItemList.begin(); jtr != LoadItemList.end();++jtr)
								{
									if(rRouteGroup == (*jtr)->prop()->GetValue(_T("Cable Route"), _T("Route Group")).c_str())
									{
										(*jtr)->prop()->SetValue(_T("Cable Route"), _T("Route Group"), _T(""));
										(*jtr)->prop()->SetOriginalValue(_T("Cable Route"), _T("Route Group"), _T(""));
									}
								}
							}
							m_bDeleted = true;
						}	
					}
				}
				{
					for(vector<string>::iterator itr = RouteGroupEntry.begin(); itr != RouteGroupEntry.end();++itr)
					{
						for(vector<CELOADTable::Record>::iterator jtr = pTable->m_RecordEntry.begin(); jtr != pTable->m_RecordEntry.end();)
						{
							if((*itr) == jtr->FieldValueMap[_T("C_ROUTE_GROUP")])
							{
								jtr = pTable->m_RecordEntry.erase(jtr);
							}
							else
							{
								++jtr;
							}
						}
					}
				}
			}
		}
		m_bModified = true;
	}
	else
	{
		::MessageBox(NULL, "먼저 Route Item을 선택하세요", "Warning", MB_OK|MB_ICONWARNING);
		return;
	}
}
