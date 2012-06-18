
#include "stdafx.h"

#include "PropertiesWnd.h"
#include "ELoadDocData.h"
#include "Resource.h"
#include "MainFrm.h"
#include "ELOAD.h"
#include "ELoadPropTabWnd.h"
#include "ELoadPropList.h"
#include "pyfromc.h"
#include "commands/CommandAutoCalculateFLC.h"
#include "TransformerPropDlg.h"
#include "CapacitorPropDlg.h"
#include "GeneratorPropDlg.h"
#include "UPSDCPropDlg.h"

using namespace COMMAND;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd() : m_pActivePropWnd(NULL)
{
	m_pObserver = new CObserver(this , &CPropertiesWnd::OnEventFromObserver);
}

CPropertiesWnd::~CPropertiesWnd()
{
	try
	{
		if(NULL != m_pObserver) delete m_pObserver;
		m_pObserver = NULL;

		for(map<CString , CWnd*>::iterator itr = m_PropWndEntry.begin();itr != m_PropWndEntry.end();++itr)
		{
			delete (itr->second);
		}
	}
	catch(...)
	{
	}
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	ON_COMMAND(ID_PROPERTIES2, OnProperties2)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateProperties2)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar 메시지 처리기

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd () == NULL)
	{
		return;
	}

	CRect rectClient,rectToolbar;
	GetClientRect (rectClient);

	m_wndToolBar.GetWindowRect (&rectToolbar);

	int cyCmb = rectToolbar.Size ().cy;
	//int cyTlb = m_wndToolBar.CalcFixedLayout (FALSE, TRUE).cy;
	/*
	m_wndObjectCombo.SetWindowPos (NULL,
	rectClient.left, 
	rectClient.top,
	rectClient.Width (),
	200,
	SWP_NOACTIVATE | SWP_NOZORDER);
	*/
	m_wndToolBar.SetWindowPos(NULL,
		rectClient.left, 
		rectClient.top , 
		rectClient.Width (),
		25,
		SWP_NOACTIVATE | SWP_NOZORDER);


	for(map<CString , CWnd*>::iterator itr = m_PropWndEntry.begin();itr != m_PropWndEntry.end();++itr)
	{
		itr->second->SetWindowPos (NULL,
			rectClient.left, 
			rectClient.top + cyCmb , 
			rectClient.Width (),
			rectClient.Height () - (cyCmb),
			SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	InitPropList("BUS_GROUP");

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* 잠금 */);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(IDB_PROPERTIES_HC , 0, 0, TRUE /* 잠금 */);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// 모든 명령은 부모 프레임이 아닌 이 컨트롤을 통해 라우팅됩니다.
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnExpandAllProperties()
{
	for(map<CString , CWnd*>::iterator itr = m_PropWndEntry.begin();itr != m_PropWndEntry.end();++itr)
	{
		if(itr->second->IsKindOf(RUNTIME_CLASS(CELoadPropList)))
		{
			CELoadPropList* pPropList = static_cast<CELoadPropList*>(itr->second);
			pPropList->SetAlphabeticMode(FALSE);
		}
                else if(itr->second->IsKindOf(RUNTIME_CLASS(CELoadPropTabWnd)))
                {
                        CELoadPropTabWnd* pPropTabWnd = static_cast<CELoadPropTabWnd*>(itr->second);
			pPropTabWnd->SetAlphabeticMode(FALSE);
                }
	}
}

void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* pCmdUI)
{
}

void CPropertiesWnd::OnSortProperties()
{
	for(map<CString , CWnd*>::iterator itr = m_PropWndEntry.begin();itr != m_PropWndEntry.end();++itr)
	{
		if(itr->second->IsKindOf(RUNTIME_CLASS(CELoadPropList)))
		{
			CELoadPropList* pPropList = static_cast<CELoadPropList*>(itr->second);
			pPropList->SetAlphabeticMode();
		}
                else if(itr->second->IsKindOf(RUNTIME_CLASS(CELoadPropTabWnd)))
                {
                        CELoadPropTabWnd* pPropTabWnd = static_cast<CELoadPropTabWnd*>(itr->second);
			pPropTabWnd->SetAlphabeticMode();
                }
	}
}

void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	if(NULL != m_pActivePropWnd)
        {
                if(m_pActivePropWnd->IsKindOf(RUNTIME_CLASS(CELoadPropList)))
		{
			CELoadPropList* pPropList = static_cast<CELoadPropList*>(m_pActivePropWnd);
			pCmdUI->SetCheck (!pPropList->IsAlphabeticMode ());
		}
                else if(m_pActivePropWnd->IsKindOf(RUNTIME_CLASS(CELoadPropTabWnd)))
                {
                        CELoadPropTabWnd* pPropTabWnd = static_cast<CELoadPropTabWnd*>(m_pActivePropWnd);
			pCmdUI->SetCheck (!pPropTabWnd->IsAlphabeticMode ());
                }
        }
}

void CPropertiesWnd::OnProperties1()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}

void CPropertiesWnd::OnProperties2()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}

/**
 * 	@brief	Item Name에 해당하는 Prop Group File Name을 구한다.
 *
 * 	@author	BHK
 */ 	
int CPropertiesWnd::FindPropGroupOf( CString& rPropGroupFilePath , const CString& rItemName )
{
        string rExecPath = GetExecPath().operator LPCSTR();
	rPropGroupFilePath = (rExecPath + "Setting\\" +string(rItemName.operator LPCSTR()) + ".PROP_GROUP").c_str();
	if(!FileExist(rPropGroupFilePath))
	{
		rPropGroupFilePath = "";
		return ERROR_FILE_NOT_FOUND;
	}

	return ERROR_SUCCESS;
}

/**
	@brief	item에 맞는 property window를 생성한다.

	@author	BHK
*/
void CPropertiesWnd::InitPropList(const CString& rTypeString)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	map<string,CELoadItemProp*>::iterator where = docData.m_ItemPropMap.find( rTypeString.operator LPCSTR() );
        if(where == docData.m_ItemPropMap.end()) return;

	UINT ID = 2;
	for(map<string,CELoadItemProp*>::iterator itr = docData.m_ItemPropMap.begin();itr != docData.m_ItemPropMap.end();++itr)
	{
		CString rPropGroupFilePath;
		if(ERROR_SUCCESS == FindPropGroupOf(rPropGroupFilePath , itr->first.c_str()))
		{
			CELoadPropTabWnd* pPropTabWnd = new CELoadPropTabWnd(rPropGroupFilePath);
			if(pPropTabWnd)
			{
				CRect rectDummy;
				rectDummy.SetRectEmpty();

				if (!pPropTabWnd->Create (CMFCTabCtrl::STYLE_3D , rectDummy, this, ID++))
				{
					TRACE0("Failed to create Properies Grid \n");
					continue;      // fail to create
				}
				pPropTabWnd->InitPropList( itr->second );
				m_PropWndEntry[itr->first.c_str()] = pPropTabWnd;
				pPropTabWnd->ShowWindow(SW_HIDE);
			}
		}
		else
		{
			if(itr->first == string(CTransformerItem::TypeString()))
			{
				CTransformerPropDlg* pPropDlg = new CTransformerPropDlg;
				if(pPropDlg)
				{
					CRect rectDummy;
					rectDummy.SetRectEmpty();
					if (!pPropDlg->Create (CTransformerPropDlg::IDD , this))
					{
						TRACE0("Failed to create Properies Grid \n");
						continue;      // fail to create
					}
					pPropDlg->ShowWindow(SW_HIDE);
					m_PropWndEntry[itr->first.c_str()] = pPropDlg;
				}
			}
			else if(itr->first == string(CCapacitorItem::TypeString()))
			{
				CCapacitorPropDlg* pPropDlg = new CCapacitorPropDlg;
				if(pPropDlg)
				{
					CRect rectDummy;
					rectDummy.SetRectEmpty();
					if (!pPropDlg->Create (CCapacitorPropDlg::IDD , this))
					{
						TRACE0("Failed to create Properies Grid \n");
						continue;      // fail to create
					}
					pPropDlg->ShowWindow(SW_HIDE);
					m_PropWndEntry[itr->first.c_str()] = pPropDlg;
				}
			}
			else if(itr->first == string(CGeneratorItem::TypeString()))
			{
				CGeneratorPropDlg* pPropDlg = new CGeneratorPropDlg;
				if(pPropDlg)
				{
					CRect rectDummy;
					rectDummy.SetRectEmpty();
					if (!pPropDlg->Create (CGeneratorPropDlg::IDD , this))
					{
						TRACE0("Failed to create Properies Grid \n");
						continue;      // fail to create
					}
					pPropDlg->ShowWindow(SW_HIDE);
					m_PropWndEntry[itr->first.c_str()] = pPropDlg;
				}
			}
			else if(itr->first == string(CUPSDCItem::TypeString()))
			{
				CUPSDCPropDlg* pPropDlg = new CUPSDCPropDlg;
				if(pPropDlg)
				{
					CRect rectDummy;
					rectDummy.SetRectEmpty();
					if (!pPropDlg->Create (CUPSDCPropDlg::IDD , this))
					{
						TRACE0("Failed to create Properies Grid \n");
						continue;      // fail to create
					}
					pPropDlg->ShowWindow(SW_HIDE);
					m_PropWndEntry[itr->first.c_str()] = pPropDlg;
				}
			}
			else
			{
				CELoadPropList* pPropList = new CELoadPropList;
				if(pPropList)
				{
					CRect rectDummy;
					rectDummy.SetRectEmpty();
					if (!pPropList->Create (/*WS_VISIBLE | */WS_CHILD, rectDummy, this, ID++))
					{
						TRACE0("Failed to create Properies Grid \n");
						continue;      // fail to create
					}
					pPropList->EnableHeaderCtrl (FALSE);
					pPropList->EnableDescriptionArea ();
					pPropList->SetVSDotNetLook ();
					pPropList->MarkModifiedProperties ();

					pPropList->InitPropList( itr->second );
					m_PropWndEntry[itr->first.c_str()] = pPropList;
				}
			}
		}
	}
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	if(NULL != m_pActivePropWnd) m_pActivePropWnd->SetFocus ();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::SetPropListFont()
{
	
}

/**
	@brief	PropertyViewbar의 값을 설정한다. 앞서 ITEM에 해당하는 PropertyWindow를 활성화 시킨다.

	@author	BHK
	
	@date	?
 */	
int CPropertiesWnd::SetPropValues( const CString& rPropName , CEventData* pEventData )
{
	static int PREV_WIDTH = -1;

	map<CString,CWnd*>::iterator where = m_PropWndEntry.find(rPropName);
	if(where != m_PropWndEntry.end())
	{
		CWnd* pWnd = where->second;
		if(pWnd)
		{
			CRect rect;
			GetWindowRect(&rect);

			if(m_pActivePropWnd) m_pActivePropWnd->ShowWindow(SW_HIDE);
			if(pWnd->IsKindOf(RUNTIME_CLASS(CTransformerPropDlg)))
			{
				//! 현재 active된 윈도우가 transformer dialog가 아닐때 width를 저장해 둔다.
				if(m_pActivePropWnd && !m_pActivePropWnd->IsKindOf(RUNTIME_CLASS(CTransformerPropDlg)))
					PREV_WIDTH = rect.Width();

				//! transformer dialog에 맞게 폭을 조정.
				rect.left  = rect.right - CTransformerPropDlg::IDEAL_WIDTH;
				if((NULL == m_pActivePropWnd) || (m_pActivePropWnd && !m_pActivePropWnd->IsKindOf(RUNTIME_CLASS(CTransformerPropDlg))))
				{
					SetWindowPos(NULL , 0 , 0 , CTransformerPropDlg::IDEAL_WIDTH , rect.Height() , SWP_NOZORDER | SWP_NOMOVE);
				}
			}
			else if(pWnd->IsKindOf(RUNTIME_CLASS(CCapacitorPropDlg)))
			{
				//! 현재 active된 윈도우가 capacitor dialog가 아닐때 width를 저장해 둔다.
				if(m_pActivePropWnd && !m_pActivePropWnd->IsKindOf(RUNTIME_CLASS(CCapacitorPropDlg)))
					PREV_WIDTH = rect.Width();

				//! capacitor dialog에 맞게 폭을 조정.
				rect.left  = rect.right - CCapacitorPropDlg::IDEAL_WIDTH;
				if((NULL == m_pActivePropWnd) || (m_pActivePropWnd && !m_pActivePropWnd->IsKindOf(RUNTIME_CLASS(CCapacitorPropDlg))))
				{
					SetWindowPos(NULL , 0 , 0 , CCapacitorPropDlg::IDEAL_WIDTH , rect.Height() , SWP_NOZORDER | SWP_NOMOVE);
				}
			}
			else if(pWnd->IsKindOf(RUNTIME_CLASS(CGeneratorPropDlg)))
			{
				//! 현재 active된 윈도우가 capacitor dialog가 아닐때 width를 저장해 둔다.
				if(m_pActivePropWnd && !m_pActivePropWnd->IsKindOf(RUNTIME_CLASS(CGeneratorPropDlg)))
					PREV_WIDTH = rect.Width();

				//! generator dialog에 맞게 폭을 조정.
				rect.left  = rect.right - CGeneratorPropDlg::IDEAL_WIDTH;
				if((NULL == m_pActivePropWnd) || (m_pActivePropWnd && !m_pActivePropWnd->IsKindOf(RUNTIME_CLASS(CGeneratorPropDlg))))
				{
					SetWindowPos(NULL , 0 , 0 , CGeneratorPropDlg::IDEAL_WIDTH , rect.Height() , SWP_NOZORDER | SWP_NOMOVE);
				}	
			}
			else if(pWnd->IsKindOf(RUNTIME_CLASS(CUPSDCPropDlg)))
			{
				//! 현재 active된 윈도우가 upsdc dialog가 아닐때 width를 저장해 둔다.
				if(m_pActivePropWnd && !m_pActivePropWnd->IsKindOf(RUNTIME_CLASS(CUPSDCPropDlg)))
					PREV_WIDTH = rect.Width();

				//! upsdc dialog에 맞게 폭을 조정.
				rect.left  = rect.right - CUPSDCPropDlg::IDEAL_WIDTH;
				if((NULL == m_pActivePropWnd) || (m_pActivePropWnd && !m_pActivePropWnd->IsKindOf(RUNTIME_CLASS(CUPSDCPropDlg))))
				{
					SetWindowPos(NULL , 0 , 0 , CUPSDCPropDlg::IDEAL_WIDTH , rect.Height() , SWP_NOZORDER | SWP_NOMOVE);
				}	
			}
			else
			{
				rect.left  = rect.right - CELoadPropList::IDEAL_WIDTH;
				if((NULL == m_pActivePropWnd) || (m_pActivePropWnd && !m_pActivePropWnd->IsKindOf(RUNTIME_CLASS(CELoadPropList))))
				{
					MoveWindow(rect);
				}
			}

			pWnd->SendMessage(CEventData::SELECT_ITEM , 0 , (LPARAM)pEventData);	//! 메세지를 통보한다.
			pWnd->ShowWindow(SW_SHOW);
			m_pActivePropWnd = pWnd;

			CMainFrame* pFrame = (CMainFrame*)GetParent();
			if(pFrame) pFrame->RecalcLayout();
		}

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
        @brief  PropertyViewbar의 값을 변경한다.

	@author	BHK
*/
int CPropertiesWnd::ChangeContents(const CString& rTreeText , CEventData* pEventData )
{
	CString rText(rTreeText);
	rText.MakeUpper();

	SetPropValues(rText , pEventData);
	
	return ERROR_SUCCESS;
}

/**
	@brief	called when a property is changed.

	@autor	BHK
*/
LRESULT CPropertiesWnd::OnPropertyChanged(WPARAM wp,  LPARAM lp)
{
	if(NULL == m_pActivePropWnd) return 0;

	if(wp == m_pActivePropWnd->GetDlgCtrlID())
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
				CPoint pt((int(rect.left + rect.right) * 0.5) , int(rect.bottom + rect.Height() * 0.5));
				CMFCPropEx* pHitTestProp = (CMFCPropEx*)pPropList->HitTest(pt);
				if(pHitTestProp)
				{
					if(pHitTestProp->IsGroup())
					{
						while(pHitTestProp->IsGroup())
						{                               
							CRect rect = pHitTestProp->GetRect();
							CPoint pt((int(rect.left + rect.right) * 0.5) , int(rect.bottom + rect.Height() * 0.5));
							pHitTestProp = (CMFCPropEx*)pPropList->HitTest(pt);
						}
					}
					pPropList->SetCurSel(pHitTestProp);
				}
			}
			
			CELoadDocData& docData = CELoadDocData::GetInstance();
			CEventData EventData(m_SelectionSet.GetTypeString() , CEventData::MODIFY_ITEM_PROP);
			EventData.m_SelectionSet = m_SelectionSet;
			int bExecuted = ERROR_BAD_ENVIRONMENT;
			if(m_pActivePropWnd)
			{
				//! 변경된 property에 대해 등록된 PRE EXECUTION COMMAND가 있는지 확인하고 COMMAND를 수행한다.
				BeginWaitCursor();
				const CString rCategory = pParent->GetName();
				const CString rKey = pProp->GetName();

				auto_ptr<CEventData> command(docData.CreatePreExecutionCommandOf(m_SelectionSet.GetTypeString() , rCategory.operator LPCSTR() , rKey.operator LPCSTR()));
				if(NULL != command.get())
				{
					command->m_rNewValue = rValue.operator LPCSTR();
					command->m_SelectionSet = m_SelectionSet;
					bExecuted |= command->Execute();
				}
				EndWaitCursor();
			}

			int res = m_SelectionSet.SetValue( (NULL != pParent) ? pParent->GetName() : "" , pProp->GetName() , rValue.operator LPCSTR());
			ModifyPropertyEnableIfNeed((NULL != pParent) ? pParent->GetName() : "" , pProp->GetName());
			
			if((ERROR_SUCCESS == res) && m_pActivePropWnd)
			{
				//! 변경된 property에 대해 등록된 COMMAND가 있는지 확인하고 COMMAND를 수행한다.
				BeginWaitCursor();
				const CString rCategory = pParent->GetName();
				const CString rKey = pProp->GetName();

				auto_ptr<CEventData> command(docData.CreateAutoCalculateCommandOf(m_SelectionSet.GetTypeString() , rCategory.operator LPCSTR() , rKey.operator LPCSTR()));
				if(NULL != command.get())
				{
					command->m_SelectionSet = m_SelectionSet;
					bExecuted |= command->Execute();
				}
				EndWaitCursor();
			}

			if(ERROR_SUCCESS == (ERROR_SUCCESS & bExecuted))
			{
				OnEventFromObserver(m_pObserver, &EventData);
				m_pActivePropWnd->RedrawWindow();
			}

			//! Property 창에 수정한 값을 뿌려준다.
			if(ERROR_SUCCESS == res)
			{
				///OnEventFromObserver(m_pObserver, &EventData);
				m_pActivePropWnd->RedrawWindow();

				//! Property가 변경되었다는 것을 통보한다.
				m_pObserver->FireEvent( EventData );
			}
			else if(ERROR_INVALID_ACCESS == res)
			{
				//! 변경할수 없으므로 변경된 내용을 원래대로 되돌리기 위해...
				OnEventFromObserver(m_pObserver, &EventData);
			}
		}
	}
	return 0;
}

/**
        @brief  PROPERTY 창의 Original Value를 현재의 값으로 설정한다.

        @author KHS
*/
void CPropertiesWnd::CommitModifiedValues(void)
{
        for(map<CString , CWnd*>::iterator itr = m_PropWndEntry.begin();itr != m_PropWndEntry.end();++itr)
        {
                //!BUS PROPERTY TEXT
                if(itr->second->IsKindOf(RUNTIME_CLASS(CELoadPropList)))
		{
			CELoadPropList* pPropList = static_cast<CELoadPropList*>(itr->second);
			pPropList->CommitModifiedValues();
		}
                //!LOAD PROPERTY TEXT
                else if(itr->second->IsKindOf(RUNTIME_CLASS(CELoadPropTabWnd)))
                {
                        CELoadPropTabWnd* pPropTabWnd = static_cast<CELoadPropTabWnd*>(itr->second);
			pPropTabWnd->CommitModifiedValues();
                }
	}
}

/**
	@brief	callback function

        @author BHK
*/
int CPropertiesWnd::OnEventFromObserver(CObserver* pObserver , CEventData* pEventData)
{
	if(pEventData)
	{
                if(CEventData::SELECT_ITEM == pEventData->m_eEventType)
		{
			((CPropertiesWnd*)(pObserver->owner()))->m_SelectionSet = pEventData->m_SelectionSet;
			((CPropertiesWnd*)(pObserver->owner()))->ChangeContents( pEventData->GetItemCategory().c_str() , pEventData );
		}
                else if(CEventData::MODIFY_ITEM_PROP == pEventData->m_eEventType)
                {
			((CPropertiesWnd*)(pObserver->owner()))->m_SelectionSet = pEventData->m_SelectionSet;
                        ((CPropertiesWnd*)(pObserver->owner()))->ChangeContents( pEventData->GetItemCategory().c_str() , pEventData );
                }
	}

	return 0;
}

/**
	@brief	

	@author	BHK
*/
void CPropertiesWnd::OnDestroy()
{
	for(map<CString , CWnd*>::iterator itr = m_PropWndEntry.begin();itr != m_PropWndEntry.end();++itr)
	{
		itr->second->DestroyWindow();
	}

	CDockablePane::OnDestroy();
}

/**
	@brief	called when value of lock item is changed.

	@author	BHK
*/
int CPropertiesWnd::SetItemLockProp(const string& rCategory , const string& rKey , const BOOL& bLock)
{
	m_SelectionSet.SetLock(rCategory , rKey , ((TRUE == bLock) ? true : false));
	
	return ERROR_SUCCESS;
}

/**
	@brief	모든 property의 내용을 갱신한다.

	@author	BHK
*/
int CPropertiesWnd::UpdateAllProperties(const string& rItemType)
{
	if(rItemType == m_SelectionSet.GetTypeString())
	{
		CEventData EventData(m_SelectionSet.GetTypeString() , CEventData::MODIFY_ITEM_PROP);
		EventData.m_SelectionSet = m_SelectionSet;
		OnEventFromObserver(m_pObserver, &EventData);
		m_pActivePropWnd->RedrawWindow();
	}

	return ERROR_SUCCESS;
}

/**
	@brief	Load Category의 값이 "SubBUS"일때 동적으로 GridProperty의 Enable을 바꾼다.

	@author BHK	

	@date 2009-04-17 오전 10:46:15	

	@param	

	@return		
*/
int CPropertiesWnd::ModifyPropertyEnableIfNeed(const string& rCategoryName , const string& rKeyName)
{
	if((CLoadItem::TypeString() == m_SelectionSet.GetTypeString()) && 
		(string("Type") ==  rCategoryName) && (string("Load Category") == rKeyName))
	{
		if(m_pActivePropWnd->IsKindOf(RUNTIME_CLASS(CELoadPropList)))
		{
			CELoadPropList* pPropListWnd = (CELoadPropList*)m_pActivePropWnd;
			pPropListWnd->ModifyPropertyEnableIfNeed(m_SelectionSet);
		}
		else if(m_pActivePropWnd->IsKindOf(RUNTIME_CLASS(CELoadPropTabWnd)))
		{
			CELoadPropTabWnd* pPropTabWnd = static_cast<CELoadPropTabWnd*>(m_pActivePropWnd);
			const int nTabsNum = pPropTabWnd->GetTabsNum();
			for(int i = 0;i < nTabsNum;++i)
			{
				CELoadPropList* pPropListWnd = (CELoadPropList*)pPropTabWnd->GetTabWnd(i);
				pPropListWnd->ModifyPropertyEnableIfNeed(m_SelectionSet);
			}
		}
		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}
