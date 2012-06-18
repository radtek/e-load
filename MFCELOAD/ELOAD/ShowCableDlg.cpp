// ShowCableDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "ShowCableDlg.h"
#include "ELoadDocData.h"

#include "Tables/CableCreationSettingTable.h"
using namespace TABLES;

// CShowCableDlg dialog

IMPLEMENT_DYNAMIC(CShowCableDlg, CDialog)

CShowCableDlg::CShowCableDlg(ITEMS::CLoadItem* pLoadItem , CWnd* pParent /*=NULL*/)
	: CDialog(CShowCableDlg::IDD, pParent) , m_pLoadItem(pLoadItem) , m_pCableItem(NULL)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	m_pCloneProp = docData.m_ItemPropMap[CCableItem::TypeString()]->Clone();
}

CShowCableDlg::~CShowCableDlg()
{
	try
	{
		if(NULL != m_pCloneProp) SAFE_DELETE(m_pCloneProp);
	}
	catch(...)
	{
	}
}

void CShowCableDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX , IDC_TREE_CABLE , m_wndCableTreeCtrl);
	DDX_Control(pDX , IDC_LIST_CALCULATED_RESULT , m_wndCalculatedResultList);
	DDX_Control(pDX , IDC_BUTTON_COMMIT_MODIFIED_VALUES , m_wndApplyButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancel);

	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CShowCableDlg, CDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_CABLE, &CShowCableDlg::OnTvnSelchangedTreeCable)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
	///ON_BN_CLICKED(IDOK, &CShowCableDlg::OnBnClickedOk)
	///ON_BN_CLICKED(IDC_BUTTON_REST_MODIFIED_VALUES, &CShowCableDlg::OnBnClickedButtonRestModifiedValues)
	ON_BN_CLICKED(IDC_BUTTON_COMMIT_MODIFIED_VALUES, &CShowCableDlg::OnBnClickedButtonCommitModifiedValues)
	ON_BN_CLICKED(IDCANCEL, &CShowCableDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CShowCableDlg)
	EASYSIZE(IDC_STATIC_CABLE_PROPERTY,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_PROPERTY,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_BORDER,0)
	
	EASYSIZE(IDC_STATIC_CABLE_LIST,ES_BORDER,ES_BORDER, IDC_STATIC_PROPERTY,ES_BORDER,0)
	EASYSIZE(IDC_TREE_CABLE,ES_BORDER,ES_BORDER, ES_BORDER,ES_BORDER,0)
	
	EASYSIZE(IDC_STATIC_CALCULATED_RESULT,ES_BORDER,IDC_TREE_CABLE, ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_LIST_CALCULATED_RESULT,ES_BORDER,IDC_STATIC_CALCULATED_RESULT, ES_BORDER,IDC_STATIC_CALCULATED_RESULT,0)

	EASYSIZE(IDC_STATIC_REST_MODIFIED_VALUES,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
	EASYSIZE(IDC_BUTTON_REST_MODIFIED_VALUES,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)

	EASYSIZE(IDCANCEL,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP

// CShowCableDlg message handlers
int CShowCableDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// keep client area
	CRect rect;
	GetClientRect(&rect);
	// set resizable style
	ModifyStyle(DS_MODALFRAME, WS_POPUP | WS_THICKFRAME);
	// adjust size to reflect new style
	::AdjustWindowRectEx(&rect, GetStyle(),
		::IsMenu(GetMenu()->GetSafeHmenu()), GetExStyle());
	SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(), SWP_FRAMECHANGED|
		SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOREPOSITION);

	return 0;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-06 오후 6:51:13	

	@param	

	@return		
*/
BOOL CShowCableDlg::OnEraseBkgnd(CDC* pDC)
{
	BOOL res = CDialog::OnEraseBkgnd(pDC);

	if(m_hIcon)
	{
		CRect rect;
		GetClientRect(&rect);
		int res = ::DrawIconEx(pDC->GetSafeHdc(), rect.Width() - 16 , rect.Height() - 16 , m_hIcon , 16 , 16 , 0 , NULL , DI_NORMAL);
	}

	return res;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-13 오후 2:44:21	

	@param	

	@return		
*/
BOOL CShowCableDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	BeginWaitCursor();
	CDialog::SetIcon(AfxGetApp()->LoadIcon(IDI_CABLE) , FALSE);

	if(m_pLoadItem)
	{
		const string rLoadName = m_pLoadItem->prop()->GetValue(_T("General") , _T("ITEM ID"));
		SetDlgItemText(IDC_EDIT_LOAD_ID , rLoadName.c_str());
	}

	m_wndCalculatedResultList.InsertColumn(0 , _T("Size") , 0 , 50);
	m_wndCalculatedResultList.InsertColumn(1 , _T("Length"), 0 , 50);
	m_wndCalculatedResultList.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

	/// 이미지리스트
	/*
	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_CLASS_VIEW_HC : IDB_CLASS_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap (uiBmpId))
	{
		TRACE(_T ("Can't load bitmap: %x\n"), uiBmpId);
		ASSERT (FALSE);
		return FALSE;
	}

	BITMAP bmpObj;
	bmp.GetBitmap (&bmpObj);

	UINT nFlags = ILC_MASK;
	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_imgList.Create (16, bmpObj.bmHeight, nFlags, 0, 0);
        m_imgList.Add (&bmp, RGB (255, 0, 0));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_BUS_GROUP));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_BUS));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_LOAD));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_MOTOR));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_TRANSFORMER));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_CABLE));
	*/
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	m_wndCableTreeCtrl.SetImageList(&(pApp->m_ELoadImages) , TVSIL_NORMAL);

	CELoadDocData& docData = CELoadDocData::GetInstance();
	{
		const string rProjectMDBFilePath = docData.GetProjectMDBFilePath();

		CHVCableCreationSettingTable& HVCableCreationgSettingTable = CHVCableCreationSettingTable::GetInstance();
		HVCableCreationgSettingTable.Load(rProjectMDBFilePath);
		CLV3PHCableCreationSettingTable& LV3PHCableCreationgSettingTable = CLV3PHCableCreationSettingTable::GetInstance();
		LV3PHCableCreationgSettingTable.Load(rProjectMDBFilePath);
		CLV1PHCableCreationSettingTable& LV1PHCableCreationgSettingTable = CLV1PHCableCreationSettingTable::GetInstance();
		LV1PHCableCreationgSettingTable.Load(rProjectMDBFilePath);
		CDCCableCreationSettingTable& DCCableCreationgSettingTable = CDCCableCreationSettingTable::GetInstance();
		DCCableCreationgSettingTable.Load(rProjectMDBFilePath);
		DisplayData();
	}

	CRect rect;
	CWnd* pWnd = GetDlgItem(IDC_STATIC_PROPERTY);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
	if (!m_wndCablePropList.Create (WS_VISIBLE | WS_CHILD, rect, this, IDC_STATIC_PROPERTY))
	{
		TRACE0("Failed to create Properies Grid \n");
		return FALSE;      // fail to create
	}
	m_wndCablePropList.EnableHeaderCtrl (FALSE);
	m_wndCablePropList.EnableDescriptionArea ();
	m_wndCablePropList.SetVSDotNetLook ();
	m_wndCablePropList.MarkModifiedProperties ();
	
	map<string,CELoadItemProp*>::iterator where = docData.m_ItemPropMap.find(CCableItem::TypeString());
	if(where != docData.m_ItemPropMap.end()) m_wndCablePropList.InitPropList( where->second );
	
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	m_wndCancel.SetTextHotColor(TextHotColor);
	
	HICON hCancelIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_CANCEL_18X18),
		IMAGE_ICON,
		18,18, 0 );
	m_wndCancel.SetImage(hCancelIcon);

	m_hIcon = AfxGetApp()->LoadIcon(IDI_GRABBER);

	INIT_EASYSIZE;

	GetDlgItem(IDC_BUTTON_REST_MODIFIED_VALUES)->EnableWindow(FALSE);

	EndWaitCursor();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CShowCableDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	UPDATE_EASYSIZE;
	CWnd* pWnd = GetDlgItem(IDC_STATIC_PROPERTY);
	if(pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		m_wndCablePropList.SetWindowPos(NULL , rect.left , rect.top , rect.Width() , rect.Height() , SWP_NOZORDER | SWP_NOACTIVATE);
	}
}

/**
	@brief	생성된 cable item들을 tree에 뿌려준다.

	@author	BHK
*/
int CShowCableDlg::DisplayData(void)
{
	m_wndCableTreeCtrl.DeleteAllItems();
	m_CableCategoryItemMap.clear();

	if(NULL != m_pLoadItem)
	{
		static const CString __CABLE_CATEGORY__[2] = {_T("Power Cable") , _T("Control Cable")};
		for(int i = 0;i < SIZE_OF_ARRAY(__CABLE_CATEGORY__);++i)
		{
			HTREEITEM hItem = m_wndCableTreeCtrl.InsertItem(__CABLE_CATEGORY__[i]);
			m_CableCategoryItemMap.insert(make_pair(__CABLE_CATEGORY__[i].operator LPCSTR() , hItem));
		}

		int nIndex = 0;
		for(vector<CELoadItem*>::iterator itr = m_pLoadItem->m_CableRefEntry.begin();itr != m_pLoadItem->m_CableRefEntry.end();++itr)
		{
			if((*itr)->IsDeleted()) continue;

			const string rCableName     = (*itr)->prop()->GetValue(_T("General") , _T("CABLE ID"));
			const string rCableSize     = (*itr)->prop()->GetValue(_T("General") , _T("Size"));
			const string rVoltType      = (*itr)->prop()->GetValue(_T("General") , _T("Volt Type"));
			const string rCableCategory = (*itr)->prop()->GetValue(_T("General") , _T("Cable Category"));

			map<string,HTREEITEM>::iterator where = m_CableCategoryItemMap.find(rCableCategory);
			if(where != m_CableCategoryItemMap.end())
			{
				TV_INSERTSTRUCT TI;
				TI.hParent      = where->second;
				TI.item.iImage  = CABLE_ICON;
				TI.item.iSelectedImage = CABLE_ICON;
				TI.item.mask    = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
				TI.item.pszText = (LPSTR)( rCableName.c_str() );
				HTREEITEM hItem = m_wndCableTreeCtrl.InsertItem(&TI);
				if(hItem)
				{
					m_wndCableTreeCtrl.SetItemData(hItem , (DWORD_PTR)(*itr));
					m_wndCableTreeCtrl.Expand(where->second , TVE_EXPAND);
				}
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	Cable Item을 선택했을 때 Cable의 속성을 보여준다.

	@author	BHK
*/
void CShowCableDlg::OnTvnSelchangedTreeCable(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	const CString rItemText = m_wndCableTreeCtrl.GetItemText(pNMTreeView->itemNew.hItem);
	map<string,HTREEITEM>::iterator where = m_CableCategoryItemMap.find(rItemText.operator LPCSTR());
	if(where == m_CableCategoryItemMap.end())	//! this is cable item
	{
		CCableItem* pCableItem = (CCableItem*)m_wndCableTreeCtrl.GetItemData(pNMTreeView->itemNew.hItem);
		if(pCableItem)
		{
			m_pCableItem = pCableItem;

			DisplayCalculatedResultList(m_pCableItem);

			CEventData EventData(CCableItem::TypeString() , CEventData::SELECT_ITEM);
			EventData.Add(m_pCableItem);
			m_pCloneProp->Copy(m_pCableItem->prop());	//! 변경하기 전에 미리 저장한다.

			m_wndCablePropList.SendMessage(CEventData::SELECT_ITEM , 0 , (LPARAM)(&EventData));	//! 메세지를 통보한다.
			GetDlgItem(IDC_BUTTON_COMMIT_MODIFIED_VALUES)->EnableWindow(TRUE);
		}
	}

	*pResult = 0;
}

/**
	@brief	called when a property is changed.

	@autor	BHK
*/
LRESULT CShowCableDlg::OnPropertyChanged(WPARAM wp,  LPARAM lp)
{
	if((wp == m_wndCablePropList.GetDlgCtrlID()) && m_pLoadItem)
	{
		CMFCPropEx* pProp = (CMFCPropEx*)(lp);				// Name  =BUS ID  , Value = sds
		CMFCPropEx* pParent = (CMFCPropEx*)pProp->GetParent();          // Category
		CMFCPropertyGridCtrl* pPropList = (CMFCPropertyGridCtrl*)(pProp->GetPropertyGridCtrl());
		CString rValue = pProp->GetValue();
		if((NULL != pPropList) && (NULL != pParent))
		{
			if(pProp->IsKindOf(RUNTIME_CLASS(CMFCCheckBoxProp)))
			{
				CMFCCheckBoxProp* pCheckBoxProp = (CMFCCheckBoxProp*)pProp;
				pCheckBoxProp->OnValueChanged();
			}
			else
			{
				CRect rect = pProp->GetRect();
				CPoint pt(int((rect.left + rect.right) * 0.5) , int(rect.bottom + rect.Height() * 0.5));
				CMFCPropEx* pHitTestProp = (CMFCPropEx*)pPropList->HitTest(pt);
				if(pHitTestProp)
				{
					if(pHitTestProp->IsGroup())
					{
						while(pHitTestProp->IsGroup())
						{                               
							CRect rect = pHitTestProp->GetRect();
							CPoint pt(int((rect.left + rect.right) * 0.5) , int(rect.bottom + rect.Height() * 0.5));
							pHitTestProp = (CMFCPropEx*)pPropList->HitTest(pt);
						}
					}
					pPropList->SetCurSel(pHitTestProp);
				}
			}

			if(m_pCableItem)
			{
				m_pCableItem->prop()->SetValue( (NULL != pParent) ? pParent->GetName() : "" , pProp->GetName() , rValue.operator LPCSTR());

				//! 변경된 property에 대해 등록된 COMMAND가 있는지 확인하고 COMMAND를 수행한다.
				const CString rCategory = pParent->GetName();
				const CString rKey = pProp->GetName();

				CELoadDocData& docData = CELoadDocData::GetInstance();
				auto_ptr<CEventData> command(docData.CreateAutoCalculateCommandOf(CCableItem::TypeString() , rCategory.operator LPCSTR() , rKey.operator LPCSTR()));
				if(NULL != command.get())
				{
					BeginWaitCursor();
					command->m_SelectionSet.Add(m_pCableItem);
					command->Execute();

					CEventData EventData(CCableItem::TypeString() , CEventData::SELECT_ITEM);
					EventData.Add(m_pCableItem);
					m_wndCablePropList.SendMessage(CEventData::SELECT_ITEM , 0 , (LPARAM)(&EventData));	//! 메세지를 통보한다.
					EndWaitCursor();
				}
			}
		}
	}
	return 0;
}

/**
	@brief	

	@autor	BHK
*/
int CShowCableDlg::DisplayCalculatedResultList(CCableItem* pCableItem)
{
	assert(pCableItem && "pCableItem is NULL");

	if(pCableItem)
	{
		m_wndCalculatedResultList.DeleteAllItems();

		int nIndex = 0;
		list<CELoadItemProp*> CalculatedResultList;
		pCableItem->GetCalculatedResultList(CalculatedResultList);
		for(list<CELoadItemProp*>::iterator itr = CalculatedResultList.begin();itr != CalculatedResultList.end();++itr,++nIndex)
		{
			const string& rSize   = (*itr)->GetValue(_T("General") , _T("Size"));
			const int nItem = m_wndCalculatedResultList.InsertItem(nIndex , rSize.c_str());
			const string& rLength = (*itr)->GetValue(_T("General") , _T("Length"));
			m_wndCalculatedResultList.SetItemText(nItem , 1 , rLength.c_str());
		}
	}

	return 0;
}

/**
	@brief	수정하기 이전의 값으로 되돌린다.

	@author BHK	

	@date 2009-04-13 오후 2:32:09	

	@param	

	@return		
*/
void CShowCableDlg::OnBnClickedButtonRestModifiedValues()
{
	if(m_pCableItem)
	{
		m_pCableItem->prop()->Copy(m_pCloneProp);
		
		CEventData EventData(CCableItem::TypeString() , CEventData::SELECT_ITEM);
		EventData.Add(m_pCableItem);
		m_wndCablePropList.SendMessage(CEventData::SELECT_ITEM , 0 , (LPARAM)(&EventData));	//! 메세지를 통보한다.
	}
}

/**
	@brief	수정한 내용을 item property에 적용한다.

	@author BHK	

	@date 2009-04-15 오전 10:22:14	

	@param	

	@return		
*/
void CShowCableDlg::OnBnClickedButtonCommitModifiedValues()
{
	//! clone property에 item property내용을 저장한다.
	if(m_pCableItem && m_pCloneProp) 
	{
		//! Value값을 Original Value에 저장
		for(vector<CELoadItem*>::iterator itr = m_pLoadItem->m_CableRefEntry.begin();itr != m_pLoadItem->m_CableRefEntry.end();++itr)
		{
			if((*itr)->IsDeleted()) continue;

			const string rCableCategory = (*itr)->prop()->GetValue(_T("General") , _T("Cable Category"));

			map<string,HTREEITEM>::iterator where = m_CableCategoryItemMap.find(rCableCategory);
			if(where != m_CableCategoryItemMap.end())
			{
				(*itr)->prop()->SetOriginalValueFromNewValue((*itr)->prop());
			}
		}
		
		//! Property 복사본을 현재 상태로 저장
		m_pCloneProp->Copy(m_pCableItem->prop());

		//!CableItem Item을 선택
		CEventData EventData(CCableItem::TypeString() , CEventData::SELECT_ITEM);
		EventData.Add(m_pCableItem);

		m_wndCablePropList.SendMessage(CEventData::SELECT_ITEM , 0 , (LPARAM)(&EventData));	//! 메세지를 통보한다.
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-04-15 오전 11:08:06	

	@param	

	@return		
*/
void CShowCableDlg::OnBnClickedCancel()
{
	if(m_pCableItem && m_pCloneProp) m_pCableItem->prop()->Copy(m_pCloneProp);

	OnCancel();
}
