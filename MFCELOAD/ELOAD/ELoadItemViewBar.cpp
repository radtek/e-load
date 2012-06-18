//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a sample for BCGControlBar Library Professional Edition
// Copyright (C) 1998-2008 BCGSoft Ltd.
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
//*******************************************************************************
//
// ELoadItemViewBar.cpp: implementation of the CELoadItemViewBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <Assert.h>
#include "ELoad.h"
#include "MainFrm.h"
#include "ELoadItemViewBar.h"
#include <fstream>
#include <string>
#include <vector>
#include "isstring.h"
#include "Tokenizer.h"
#include "BusGroupItem.h"
#include "CapacitorItem.h"
#include "GeneratorItem.h"
#include "UPSDCItem.h"
#include "ChildFrm.h"
#include "ELoadDocData.h"
#include "ELoadDoc.h"
#include "SaveProductToRevisionDlg.h"
#include "CompareRevisionDataDlg.h"
#include "CompareRevisionDataDoc.h"
#include "ExcelImportDlg.h"
#include "WorkStatusDlg.h"
#include "CableBMSummaryDlg.h"
#include "CableSizingResultDoc.h"
#include "Tables/CableCreationSettingTable.h"
#include "CableScheduleSettingDlg.h"
#include "UserCableImportDlg.h"
#include "ControlCableResultDoc.h"
#include "./commands/CommandPowerCableCreation.h"
#include "./commands/CommandControlCableCreation.h"

#include "DataResultDlg.h"

using namespace TABLES;
using namespace std;

BEGIN_ELOAD_CODE_MAP(CELoadItemViewBar)
	ELOADCODEFUNC("SHOW AREA",CELoadItemViewBar::ShowAreaCallback),
	ELOADCODEFUNC("SHOW BUS",CELoadItemViewBar::ShowBusCallback),
	ELOADCODEFUNC("SHOW PDB",CELoadItemViewBar::ShowPDBCallback),
	ELOADCODEFUNC("SHOW LOAD",CELoadItemViewBar::ShowLoadCallback),
	ELOADCODEFUNC("SHOW NOT ASSIGNED LOAD",CELoadItemViewBar::ShowNotAssignedLoadCallback),
	ELOADCODEFUNC("SHOW LOAD IN BUS",CELoadItemViewBar::ShowLoadInBusCallback),
	ELOADCODEFUNC("SHOW LOAD SUMMARY",CELoadItemViewBar::ShowLoadSummaryCallback),
	ELOADCODEFUNC("SHOW ALL BUS LOAD SUMMARY",CELoadItemViewBar::ShowAllBUSLoadSummaryCallback),
	ELOADCODEFUNC("SHOW ALL REV BUS LOAD SUMMARY",CELoadItemViewBar::ShowAllRevBUSLoadSummaryCallback),
	ELOADCODEFUNC("SHOW ALL PDB LOAD SUMMARY",CELoadItemViewBar::ShowAllPDBLoadSummaryCallback),
	ELOADCODEFUNC("SHOW ALL REV PDB LOAD SUMMARY",CELoadItemViewBar::ShowAllRevPDBLoadSummaryCallback),
	ELOADCODEFUNC("SHOW LOAD SUMMARY IN BUS",CELoadItemViewBar::ShowLoadSummaryInBusCallback),
	ELOADCODEFUNC("SHOW CABLE SIZING TABLE" , CELoadItemViewBar::ShowCableSizingTableCallback),
	ELOADCODEFUNC("SHOW SCHEMATIC" , CELoadItemViewBar::ShowSchematicCallback),
	ELOADCODEFUNC("SHOW IO" , CELoadItemViewBar::ShowIOCallback),
	ELOADCODEFUNC("SHOW LCS" , CELoadItemViewBar::ShowLCSCallback),
	ELOADCODEFUNC("SHOW POWER CABLE",CELoadItemViewBar::ShowPowerCableCallback),
	ELOADCODEFUNC("SHOW CONTROL CABLE",CELoadItemViewBar::ShowControlCableCallback),
	ELOADCODEFUNC("SHOW USER CABLE",CELoadItemViewBar::ShowUserCableCallback),
	ELOADCODEFUNC("SHOW TRANSFORMER",CELoadItemViewBar::ShowItemCallback),
	ELOADCODEFUNC("SHOW TRANSFORMER SIZING",CELoadItemViewBar::ShowTransformerSizingResultCallback),
	ELOADCODEFUNC("SHOW REV TRANSFORMER SIZING",CELoadItemViewBar::ShowRevTransformerSizingResultCallback),
	ELOADCODEFUNC("SHOW REV CABLE SIZING RESULT",CELoadItemViewBar::ShowRevCableSizingResultCallback),
	ELOADCODEFUNC("SHOW CAPACITOR",CELoadItemViewBar::ShowItemCallback),
	ELOADCODEFUNC("SHOW GENERATOR",CELoadItemViewBar::ShowItemCallback),
	ELOADCODEFUNC("SHOW UPSDC",CELoadItemViewBar::ShowItemCallback),
	ELOADCODEFUNC("SHOW CABLE ROUTE",CELoadItemViewBar::ShowCableRouteResultCallback),
	ELOADCODEFUNC("SHOW REV CONTROL CABLE RESULT",CELoadItemViewBar::ShowRevControlCableResultCallback),
	ELOADCODEFUNC("SHOW CABLE SCHEDULE",CELoadItemViewBar::ShowCableScheduleCallback),
	ELOADCODEFUNC("SHOW REV CABLE SCHEDULE",CELoadItemViewBar::ShowCableScheduleCallback),
	ELOADCODEFUNC("SHOW DRUM SCHEDULE",CELoadItemViewBar::ShowDrumScheduleCallback),
	ELOADCODEFUNC("SHOW REV DRUM SCHEDULE",CELoadItemViewBar::ShowDrumScheduleCallback),
	ELOADCODEFUNC("SHOW LOAD LIST-SLD",CELoadItemViewBar::ShowLoadListSldCallback),
	ELOADCODEFUNC("SHOW BUS TO BUS",CELoadItemViewBar::ShowBusToBusCallback),
END_ELOAD_CODE_MAP()

ELOADCODEIMPLEMENT(CELoadItemViewBar)

typedef struct
{
	CELoadItemViewBar* pThis;
	CString rExcelFilePath;
	int nOption;
}UserCableExcelImportParam;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CELoadItemViewBar::CELoadItemViewBar()
{
	m_pObserver = new CObserver(this , &CELoadItemViewBar::OnEventFromObserver);
}

CELoadItemViewBar::~CELoadItemViewBar()
{
	try
	{
		if(m_pObserver) SAFE_DELETE(m_pObserver);
	}
	catch(...)
	{
	}
}

BEGIN_MESSAGE_MAP(CELoadItemViewBar, CDockablePane)
	//{{AFX_MSG_MAP(CELoadItemViewBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_LOAD_LIST, &CELoadItemViewBar::OnLoadList)
	ON_COMMAND(ID_RESOURCESUB_RESOURCESUB1, &CELoadItemViewBar::OnResourcesubResourcesub1)
	ON_COMMAND(ID_RESOURCESUB_RESOURCESUB2, &CELoadItemViewBar::OnResourcesubResourcesub2)
	ON_COMMAND(ID_ITEM_SHOW, &CELoadItemViewBar::OnItemShow)
	ON_NOTIFY(TVN_SELCHANGED, CELoadItemViewBar::BUSTREE_ID , OnSelchangedBusTree)
        ON_COMMAND(ID_ITEM_SHOWALL, &CELoadItemViewBar::OnItemShowall)
	ON_COMMAND(ID_CALC_LOAD_SUMMARY, &CELoadItemViewBar::OnCalcLoadSummary)
	ON_COMMAND(ID_CALC_CABLE_SIZE , &CELoadItemViewBar::OnCalcCableSize)
        ON_COMMAND(ID_ITEM_COPY, &CELoadItemViewBar::OnItemCopy)
        ON_COMMAND(ID_ITEM_DELETE, &CELoadItemViewBar::OnItemDelete)
	ON_COMMAND(ID_ITEM_CREATE_POWER_CABLE_OF_SELECTED_LOAD, &CELoadItemViewBar::OnItemCreatePowerCableOfSelectedLoad)
	ON_COMMAND(ID_ITEM_CREATE_CONTROL_CABLE_OF_SELECTED_LOAD, &CELoadItemViewBar::OnItemCreateControlCableOfSelectedLoad)

        ON_COMMAND(ID_EDIT_CUT, &CELoadItemViewBar::OnEditCut)
        ON_COMMAND(ID_EDIT_PASTE, &CELoadItemViewBar::OnEditPaste)
        ON_COMMAND(ID_EDIT_CLEAR, &CELoadItemViewBar::OnEditClear)
	ON_COMMAND(ID_ITEM_SHOW_LIST, &CELoadItemViewBar::OnItemShowList)
        ON_COMMAND(ID_CREATE_LOAD, &CELoadItemViewBar::OnCreateLoad)
	ON_COMMAND(ID_CREATE_TRANSFORMER, &CELoadItemViewBar::OnCreateTransformer)
	ON_COMMAND(ID_COPY_TRANSFORMER, &CELoadItemViewBar::OnCopyTransformer)
	ON_COMMAND(ID_DELETE_TRANSFORMER, &CELoadItemViewBar::OnDeleteTransformer)

	ON_COMMAND(ID_CREATE_CABLE, &CELoadItemViewBar::OnCreateCable)
	ON_COMMAND(ID_COPY_CABLE, &CELoadItemViewBar::OnCopyCable)
	ON_COMMAND(ID_DELETE_CABLE, &CELoadItemViewBar::OnDeleteCable)
	ON_COMMAND(ID_DELETE_ALL_CABLE, &CELoadItemViewBar::OnDeleteAllCable)

	ON_COMMAND(ID_CABLESIZINGRESULT_SHOWCABLESIZINGRESULT, &CELoadItemViewBar::OnCablesizingresultShowcablesizingresult)
	ON_COMMAND(ID_DELETE_ITEM, &CELoadItemViewBar::OnDeleteCableSizingTableFile)
	ON_COMMAND(ID_SAVE_PRODUCT_TO_REVISION, &CELoadItemViewBar::OnSaveProductToRevision)
	ON_COMMAND(ID_VIEW_REVISION, &CELoadItemViewBar::OnViewRevision)
	
	ON_COMMAND(ID_CAPACITORCONTEXTMENU_NEWCAPACITOR, &CELoadItemViewBar::OnNewcapacitor)
	ON_COMMAND(ID_CAPACITORCONTEXTMENU_COPY, &CELoadItemViewBar::OnCapacitorCopy)
	ON_COMMAND(ID_CAPACITORCONTEXTMENU_DELETE, &CELoadItemViewBar::OnCapacitorDelete)

	ON_COMMAND(ID_GENERATORCONTEXTMENU_NEWGENERATOR, &CELoadItemViewBar::OnNewGenerator)
	ON_COMMAND(ID_GENERATORCONTEXTMENU_COPY, &CELoadItemViewBar::OnGeneratorCopy)
	ON_COMMAND(ID_GENERATORCONTEXTMENU_DELETE, &CELoadItemViewBar::OnGeneratorDelete)

	ON_COMMAND(ID_UPSDCCONTEXTMENU_NEWUPSDC, &CELoadItemViewBar::OnNewUPSDC)
	ON_COMMAND(ID_UPSDCCONTEXTMENU_COPY, &CELoadItemViewBar::OnUPSDCCopy)
	ON_COMMAND(ID_UPSDCCONTEXTMENU_DELETE, &CELoadItemViewBar::OnUPSDCDelete)
	ON_COMMAND(ID_USER_CABLE_IMPORT, &CELoadItemViewBar::OnUserCableImport)
	ON_COMMAND(ID_REV_ITEM_DELETE, &CELoadItemViewBar::OnRevItemDelete)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CELoadItemViewBar message handlers

/**
        @brief  view bar가 생성될때...

        @author BHK
*/
int CELoadItemViewBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_wndSplitter.CreateStatic (this, 2, 1);

	CRect rectDummy;
	rectDummy.SetRectEmpty ();

	// Create views:
	const DWORD dwViewStyle =	TVS_SHOWSELALWAYS | WS_CHILD | WS_VISIBLE | TVS_HASLINES | 
					TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS | 
					WS_CLIPCHILDREN;
	
	//! 폰트 설정 - default - (DEFAULT_GUI_FONT)
	if (!m_wndItemClassTreeCtrl.Create (dwViewStyle, rectDummy, &m_wndSplitter, m_wndSplitter.IdFromRowCol (0, 0)))
	{
		TRACE0("Failed to create Class View\n");
		return -1;      // fail to create
	}
	
	const DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_VSCROLL | LVS_REPORT | LVS_EDITLABELS | LVS_SHOWSELALWAYS;
	m_wndLoadListCtrl.SetColumnTitleString(_T("Load Item"));
	if(!m_wndLoadListCtrl.Create (dwStyle, rectDummy, &m_wndSplitter, m_wndSplitter.IdFromRowCol (1, 0)))
	{
		TRACE0("Failed to create load list ctrl\n");
		return -1;      // fail to create
	}
	m_wndLoadListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
	m_wndLoadListCtrl.SetSortColumn(0);	//! 오름 차순으로 정렬한다.
	m_ActionWndMap[&m_wndLoadListCtrl].push_back(_T("SHOW LOAD"));
	m_ActionWndMap[&m_wndLoadListCtrl].push_back(_T("SHOW LOAD IN BUS"));
	m_wndLoadListCtrl.ShowWindow(SW_HIDE);

	m_wndSchematicListCtrl.SetColumnTitleString(_T("Schematic Item"));
	if(!m_wndSchematicListCtrl.Create (dwStyle, rectDummy, &m_wndSplitter, CELoadItemViewBar::SCHEMATIC_ID))
	{
		TRACE0("Failed to create schematic list ctrl\n");
		return -1;      // fail to create
	}
	m_wndSchematicListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
	m_wndSchematicListCtrl.SetSortColumn(0);	//! 오름 차순으로 정렬한다.
	m_ActionWndMap[&m_wndSchematicListCtrl].push_back(_T("SHOW SCHEMATIC"));
	m_wndSchematicListCtrl.ShowWindow(SW_HIDE);

	m_wndIOListCtrl.SetColumnTitleString(_T("IO Item"));
	if(!m_wndIOListCtrl.Create (dwStyle, rectDummy, &m_wndSplitter, CELoadItemViewBar::IO_ID))
	{
		TRACE0("Failed to create io list ctrl\n");
		return -1;      // fail to create
	}
	m_wndIOListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
	m_wndIOListCtrl.SetSortColumn(0);	//! 오름 차순으로 정렬한다.
	m_ActionWndMap[&m_wndIOListCtrl].push_back(_T("SHOW IO"));
	m_wndIOListCtrl.ShowWindow(SW_HIDE);

	m_wndLCSListCtrl.SetColumnTitleString(_T("LCS Item"));
	if(!m_wndLCSListCtrl.Create (dwStyle, rectDummy, &m_wndSplitter, CELoadItemViewBar::LCS_ID))
	{
		TRACE0("Failed to create lcs list ctrl\n");
		return -1;      // fail to create
	}
	m_wndLCSListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
	m_wndLCSListCtrl.SetSortColumn(0);	//! 오름 차순으로 정렬한다.
	m_ActionWndMap[&m_wndLCSListCtrl].push_back(_T("SHOW LCS"));
	m_wndLCSListCtrl.ShowWindow(SW_HIDE);

	CreateCableListCtrl(dwStyle);

	m_wndTransformerListCtrl.SetColumnTitleString(_T("Transformer Item"));
	if(!m_wndTransformerListCtrl.Create (dwStyle, rectDummy, &m_wndSplitter, CELoadItemViewBar::TRANSFORMER_ID))
	{
		TRACE0("Failed to create transformer list ctrl\n");
		return -1;      // fail to create
	}
	m_wndTransformerListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
	m_wndTransformerListCtrl.SetSortColumn(0);	//! 오름 차순으로 정렬한다.
	m_ActionWndMap[&m_wndTransformerListCtrl].push_back(_T("SHOW TRANSFORMER"));
	m_wndTransformerListCtrl.ShowWindow(SW_HIDE);

	m_wndCapacitorListCtrl.SetColumnTitleString(_T("Capacitor Item"));
	if(!m_wndCapacitorListCtrl.Create (dwStyle, rectDummy, &m_wndSplitter, CELoadItemViewBar::CAPACITOR_ID))
	{
		TRACE0("Failed to create capacitor list ctrl\n");
		return -1;      // fail to create
	}
	m_wndCapacitorListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
	m_wndCapacitorListCtrl.SetSortColumn(0);	//! 오름 차순으로 정렬한다.
	m_ActionWndMap[&m_wndCapacitorListCtrl].push_back(_T("SHOW CAPACITOR"));
	m_wndCapacitorListCtrl.ShowWindow(SW_HIDE);

	m_wndGeneratorListCtrl.SetColumnTitleString(_T("Generator Item"));
	if(!m_wndGeneratorListCtrl.Create (dwStyle, rectDummy, &m_wndSplitter, CELoadItemViewBar::GENERATOR_ID))
	{
		TRACE0("Failed to create generator list ctrl\n");
		return -1;      // fail to create
	}
	m_wndGeneratorListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
	m_wndGeneratorListCtrl.SetSortColumn(0);	//! 오름 차순으로 정렬한다.
	m_ActionWndMap[&m_wndGeneratorListCtrl].push_back(_T("SHOW GENERATOR"));
	m_wndGeneratorListCtrl.ShowWindow(SW_HIDE);

	m_wndUPSDCListCtrl.SetColumnTitleString(_T("UPS/DC Item"));
	if(!m_wndUPSDCListCtrl.Create (dwStyle, rectDummy, &m_wndSplitter, CELoadItemViewBar::GENERATOR_ID))
	{
		TRACE0("Failed to create UPS/DC list ctrl\n");
		return -1;      // fail to create
	}
	m_wndUPSDCListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
	m_wndUPSDCListCtrl.SetSortColumn(0);	//! 오름 차순으로 정렬한다.
	m_ActionWndMap[&m_wndUPSDCListCtrl].push_back(_T("SHOW UPSDC"));
	m_wndUPSDCListCtrl.ShowWindow(SW_HIDE);

	if(!m_wndEmpty.Create(NULL , "" , WS_CHILD |WS_CLIPSIBLINGS , rectDummy , &m_wndSplitter , CELoadItemViewBar::EMPTY_ID))
	{
		TRACE0("Failed to create Child View\n");
		return -1;
	}
	m_ActionWndMap[&m_wndEmpty].push_back(_T("*"));

	///// Bus일때만 Tree
	const DWORD dwTreeStyle = WS_CHILD | WS_VISIBLE | TVS_SHOWSELALWAYS | TVS_EDITLABELS | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES;
	if(!m_wndBusTreeView.Create(dwTreeStyle, rectDummy, &m_wndSplitter, CELoadItemViewBar::BUSTREE_ID))
	{
		TRACE0("Failed to create Child View\n");
		return -1;
	}
	m_ActionWndMap[&m_wndBusTreeView].push_back(_T("SHOW BUS"));

	CFont font;
	font.CreateFont(-10,0,0,0,400,FALSE,FALSE,FALSE,HANGUL_CHARSET,3,2,1,VARIABLE_PITCH | FF_MODERN,"Arial"); 

	m_wndItemClassTreeCtrl.SetFont(CFont::FromHandle ((HFONT) GetStockObject (ANSI_VAR_FONT)));
	for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
	{
		itr->first->SetFont(CFont::FromHandle ((HFONT) GetStockObject (ANSI_VAR_FONT)));
	}

	m_wndBusTreeView.ShowWindow(SW_NORMAL);
	m_wndLoadListCtrl.ShowWindow(SW_HIDE);
	m_wndTransformerListCtrl.ShowWindow(SW_HIDE);
	m_wndCapacitorListCtrl.ShowWindow(SW_HIDE);

	m_wndItemClassTreeCtrl.FillClassView ();

	///! Bus Edit tree에 Bus Group과 Bus Item들을 생성한다.
	OnCreateBusTreeCtrl();

	m_wndItemClassTreeCtrl.Select(m_wndItemClassTreeCtrl.GetRootItem() , TVGN_CARET);

	OnChangeVisualStyle ();

	return 0;
}

void CELoadItemViewBar::OnSize(UINT nType, int cx, int cy) 
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout ();
}

/**
	@brief	called when right mouse button clicked

	@author	BHK

	@date	?

	@param

	@return
*/
void CELoadItemViewBar::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CTreeCtrl* pTreeWnd = (CTreeCtrl*)m_wndSplitter.GetPane(0,0);
	CListCtrl* pListWnd = (CListCtrl*)m_wndSplitter.GetPane(1,0);

	CPoint clientPoint = point;
	ScreenToClient(&clientPoint);

	CRect rect[2];
	pTreeWnd->GetWindowRect(&rect[0]);
	pListWnd->GetWindowRect(&rect[1]);
	ScreenToClient(&rect[0]);
	ScreenToClient(&rect[1]);
	
	//! clicked on tree window
	if(rect[0].PtInRect(clientPoint))
	{
		//! Click Tree Ctrl
		if (point != CPoint (-1, -1))
		{ 
			//---------------------
			// Select clicked item:
			//---------------------
			CPoint ptTree = point;
			pTreeWnd->ScreenToClient (&ptTree);

			HTREEITEM hTreeItem = pTreeWnd->HitTest (ptTree);
			if (hTreeItem != NULL)
			{
				pTreeWnd->SelectItem(hTreeItem);
				m_rCurrentItemText = pTreeWnd->GetItemText(hTreeItem);

				CClassTreeWnd::ELOAD_ITEM_FILE_STRUCT* pItemFileStruct = 
					(CClassTreeWnd::ELOAD_ITEM_FILE_STRUCT*)pTreeWnd->GetItemData(hTreeItem);
				pTreeWnd->SelectItem (hTreeItem);
				if(NULL == pItemFileStruct)
				{
					TVITEM item;
					item.hItem = hTreeItem;
					item.mask  = TVIF_IMAGE;
					pTreeWnd->GetItem(&item);

					HTREEITEM hParentItem = pTreeWnd->GetParentItem(hTreeItem);
					if(hParentItem && (_T("Cable Sizing Table") == pTreeWnd->GetItemText(hParentItem)))
					{
						CMenu menu;
						if(menu.CreatePopupMenu())
						{
							menu.AppendMenu(0 , ID_REV_ITEM_DELETE , _T("Delete"));
							menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, point.y, this);
						}
					}
					else if(REVISION_ICON == item.iImage)
					{
						CMenu menu;
						if(menu.CreatePopupMenu())
						{
							menu.AppendMenu(0 , ID_REV_ITEM_DELETE , _T("Delete"));
							menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, point.y, this);
						}
					}
					return;
				}

				if(_T("SHOW LOAD") == pItemFileStruct->rCommand)
				{
					pTreeWnd->SetFocus ();

					theApp.GetContextMenuManager ()->ShowPopupMenu (IDR_CONTEXT_SHOWLOAD_MENU,
						point.x, point.y, this, TRUE);//IDR_POPUP_SORT//
				}
				else if((_T("SHOW ALL BUS LOAD SUMMARY") == pItemFileStruct->rCommand) || (_T("SHOW ALL PDB LOAD SUMMARY") == pItemFileStruct->rCommand))
				{
					pTreeWnd->SetFocus ();

					theApp.GetContextMenuManager ()->ShowPopupMenu (IDR_CONTEXT_CALC_LOADSUMMARY_MENU,
						point.x, point.y, this, TRUE);//IDR_POPUP_SORT//
				}
				else if(_T("SHOW CABLE SIZING TABLE") == pItemFileStruct->rCommand)
				{
					pTreeWnd->SetFocus ();

					theApp.GetContextMenuManager ()->ShowPopupMenu (IDR_CONTEXT_CALC_CABLESIZING_MENU,
						point.x, point.y, this, TRUE);//IDR_POPUP_SORT//
				}
				else if(_T("SHOW TRANSFORMER SIZING") == pItemFileStruct->rCommand)
				{
					pTreeWnd->SetFocus ();

					theApp.GetContextMenuManager ()->ShowPopupMenu (IDR_CONTEXT_TR_SIZING_RESULT_MENU,
						point.x, point.y, this, TRUE);//IDR_POPUP_SORT//
				}
				else if(_T("SHOW POWER CABLE") == pItemFileStruct->rCommand)
				{
					pTreeWnd->SetFocus();

					theApp.GetContextMenuManager()->ShowPopupMenu(IDR_CONTEXT_CREATE_CABLE_SIZING_RESULT_MENU,
						point.x, point.y, this, TRUE);//IDR_POPUP_SORT//
				}
				else if(_T("SHOW CONTROL CABLE") == pItemFileStruct->rCommand)
				{
					pTreeWnd->SetFocus();

					theApp.GetContextMenuManager()->ShowPopupMenu(IDR_CONTEXT_CONTROL_CABLE_RESULT_MENU,
						point.x, point.y, this, TRUE);//IDR_POPUP_SORT//
				}
				else if(_T("SHOW USER CABLE") == pItemFileStruct->rCommand)
				{
					pTreeWnd->SetFocus();

					theApp.GetContextMenuManager()->ShowPopupMenu(IDR_CONTEXT_USER_CABLE_MENU,
						point.x, point.y, this, TRUE);//IDR_POPUP_SORT//
				}
				else if((_T("SHOW CABLE SCHEDULE") == pItemFileStruct->rCommand) || 
					(_T("SHOW DRUM SCHEDULE") == pItemFileStruct->rCommand) || 
					(_T("SHOW CABLE ROUTE") == pItemFileStruct->rCommand))
				{
					pTreeWnd->SetFocus();

					theApp.GetContextMenuManager()->ShowPopupMenu(IDR_CONTEXT_CONTROL_CABLE_RESULT_MENU,
						point.x, point.y, this, TRUE);//IDR_POPUP_SORT//
				}
				/*
				else
				{
					pTreeWnd->SetFocus ();
				}
				*/
			}
		}
		return;
	}
        ///! action window List
	else if(rect[1].PtInRect(clientPoint))
	{
		CPoint ptList = point;
		pListWnd->ScreenToClient (&ptList);

		if(m_wndTransformerListCtrl.IsWindowVisible())
		{
			theApp.GetContextMenuManager ()->ShowPopupMenu (IDR_CONTEXT_TRANSFORMER_MENU,
				point.x, point.y, this, TRUE);
		}
		else if(m_wndPowerCableListCtrl.IsWindowVisible() || m_wndControlCableListCtrl.IsWindowVisible() || m_wndUserCableListCtrl.IsWindowVisible())
		{
			theApp.GetContextMenuManager ()->ShowPopupMenu (IDR_CONTEXT_CABLE_MENU,
				point.x, point.y, this, TRUE);
		}
		else if(m_wndCapacitorListCtrl.IsWindowVisible())
		{
			theApp.GetContextMenuManager ()->ShowPopupMenu (IDR_CONTEXT_CAPACITOR_MENU,
				point.x, point.y, this, TRUE);
		}
		else if(m_wndGeneratorListCtrl.IsWindowVisible())
		{
			theApp.GetContextMenuManager ()->ShowPopupMenu (IDR_CONTEXT_GENERATOR_MENU,
				point.x, point.y, this, TRUE);
		}
		else if(m_wndUPSDCListCtrl.IsWindowVisible())
		{
			theApp.GetContextMenuManager ()->ShowPopupMenu (IDR_CONTEXT_UPSDC_MENU,
				point.x, point.y, this, TRUE);
		}
		else
		{
			int res = pListWnd->HitTest(ptList);
			if( -1 != res )
			{
				if(m_wndLoadListCtrl.IsWindowVisible())
				{
					theApp.GetContextMenuManager ()->ShowPopupMenu (IDR_CONTEXT_LOAD_MENU,
						point.x, point.y, this, TRUE);//IDR_POPUP_SORT//
				}
				///! BUS TREE 가 이리로도 온다...
				else if(m_wndBusTreeView.IsWindowVisible())
				{
					HTREEITEM hItem = m_wndBusTreeView.GetSelectedItem();
					///! Parents Tree
					if(NULL == m_wndBusTreeView.GetParentItem(hItem))
					{
						theApp.GetContextMenuManager()->ShowPopupMenu(IDR_CONTEXT_BUS_MENU,
							point.x, point.y, this, TRUE);
					}
				}
			}
			///! BUS TREE CTRL
			else
			{
				if(m_wndLoadListCtrl.IsWindowVisible())
				{
					if(INCOMING != m_wndLoadListCtrl.GetBusID())
					{
						CMenu menu , *pMenu = NULL;
						menu.LoadMenu(IDR_CONTEXT_LOAD_MENU);
						pMenu = menu.GetSubMenu(0);
						if(pMenu)
						{
							pMenu->DeleteMenu(11 , MF_BYPOSITION);
							pMenu->DeleteMenu(10 , MF_BYPOSITION);
							pMenu->DeleteMenu(9 , MF_BYPOSITION);
							pMenu->DeleteMenu(8 , MF_BYPOSITION);
							pMenu->DeleteMenu(7 , MF_BYPOSITION);
							pMenu->DeleteMenu(6 , MF_BYPOSITION);
							pMenu->DeleteMenu(4 , MF_BYPOSITION);
							pMenu->DeleteMenu(3 , MF_BYPOSITION);
							pMenu->DeleteMenu(1 , MF_BYPOSITION);
							pMenu->DeleteMenu(0 , MF_BYPOSITION);

							pMenu->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, point.y, this);
						}
					}
				}
				else if(m_wndBusTreeView.IsWindowVisible())
				{
					HTREEITEM hItem = m_wndBusTreeView.GetSelectedItem();
					///! BUS_GROUP
					if(NULL == m_wndBusTreeView.GetParentItem(hItem))
					{
						theApp.GetContextMenuManager()->ShowPopupMenu(IDR_CONTEXT_BUS_MENU,
							point.x, point.y, this, TRUE);
					}
					//! BUS
					else
					{
						theApp.GetContextMenuManager()->ShowPopupMenu(IDR_BUS_COPY_MENU,
							point.x, point.y, this, TRUE);
					}
				}
			}
		}
		return;
	}
	else	
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}
}

/**
	@brief	

	@author	BHK
*/
void CELoadItemViewBar::AdjustLayout ()
{
	if (GetSafeHwnd () == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect (rectClient);

	CRect rectPrev;
	m_wndSplitter.GetClientRect (rectPrev);

	m_wndSplitter.SetWindowPos (NULL, rectClient.left, rectClient.top , 
								rectClient.Width (), rectClient.Height () ,
								SWP_NOACTIVATE | SWP_NOZORDER);
	if (rectPrev.Height () != rectClient.Height () )
	{
		m_wndSplitter.SetRowInfo (0, 400,0);//rectClient.Height () * 1 / 3, 0);
	}
}

BOOL CELoadItemViewBar::PreTranslateMessage(MSG* pMsg) 
{
	return CDockablePane::PreTranslateMessage(pMsg);
}


void CELoadItemViewBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rectTree;
	m_wndItemClassTreeCtrl.GetWindowRect (rectTree);
	ScreenToClient (rectTree);

	rectTree.InflateRect (1, 1);
	dc.Draw3dRect (rectTree, ::GetSysColor (COLOR_3DSHADOW), ::GetSysColor (COLOR_3DSHADOW));
}

void CELoadItemViewBar::OnSetFocus(CWnd* pOldWnd) 
{
	CDockablePane::OnSetFocus(pOldWnd);
	
	m_wndItemClassTreeCtrl.SetFocus ();
}

/**
	@brief	

	@author	BHK
*/
void CELoadItemViewBar::OnChangeVisualStyle ()
{
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp)
	{
		m_wndItemClassTreeCtrl.SetImageList (&(pApp->m_ELoadImages), TVSIL_NORMAL);
		for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
		{
			if(itr->first->IsKindOf(RUNTIME_CLASS(CMFCListCtrl)))
				(static_cast<CMFCListCtrl*>(itr->first))->SetImageList (&(pApp->m_ELoadImages), LVSIL_SMALL);
			else if(itr->first->IsKindOf(RUNTIME_CLASS(CTreeCtrl)))
				(static_cast<CTreeCtrl*>(itr->first))->SetImageList (&(pApp->m_ELoadImages), TVSIL_NORMAL);
		}
	}
}

void CELoadItemViewBar::OnLoadList()
{
	// TODO: Add your command handler code here
	AfxMessageBox (_T("Load List"));
}

void CELoadItemViewBar::OnResourcesubResourcesub1()
{
	// TODO: Add your command handler code here
	AfxMessageBox (_T("Load List(BUS)"));
}

void CELoadItemViewBar::OnResourcesubResourcesub2()
{
	// TODO: Add your command handler code here
	AfxMessageBox (_T("Load List2"));
}

/**
	@brief  Read 'Load.Item' file and then create tree.

        @author	BHK
*/
/*
void CELoadItemViewBar::CreateObjTreeNode(CTreeCtrl* pTreeCtrl , HTREEITEM hParent, ifstream& ifile)
{
	HTREEITEM hItem = NULL;
	while(!ifile.eof() && ifile.good())
	{
		string aLine;
		getline(ifile, aLine);
		

		if(aLine.empty() || "*" == aLine || "[" == aLine) continue;

		CString rStr(aLine.c_str());
		rStr.Trim();
		if(("}" == rStr) || ("]" == rStr)) break;

		if("{" == rStr) 
		{
			CreateObjTreeNode(pTreeCtrl, hItem, ifile);
			m_wndItemClassTreeCtrl.SetItemState (hParent, TVIS_BOLD, TVIS_BOLD);
			m_wndItemClassTreeCtrl.Expand(hParent , TVE_EXPAND);
		}
		else if("" != rStr)
		{
			vector<string> oResult;
			CTokenizer<CIsFromString>::Tokenize(oResult , rStr.operator LPCSTR() , CIsFromString(","));
			CString rItemImage("0") , rSelectedImage("0") , rItemText("0") , rCommand("SHOW NONE");
			if(4 == oResult.size())
			{
				ELOAD_ITEM_FILE_STRUCT* pItemFileStruct = new ELOAD_ITEM_FILE_STRUCT;
				if(pItemFileStruct)
				{
					pItemFileStruct->rItemImage = oResult[1].c_str();
					pItemFileStruct->rSelectedImage = oResult[2].c_str();
					pItemFileStruct->rItemText = oResult[0].c_str();
					pItemFileStruct->rCommand  = oResult[3].c_str();
					pItemFileStruct->rItemImage.Trim();
					pItemFileStruct->rSelectedImage.Trim();
					pItemFileStruct->rItemText.Trim();
					pItemFileStruct->rCommand.Trim();

					m_ELoadItemFileStructEntry.push_back(pItemFileStruct);

					TV_INSERTSTRUCT TI;
					TI.hParent     = hParent;
					TI.item.iImage = atoi(pItemFileStruct->rItemImage);
					TI.item.iSelectedImage = atoi(pItemFileStruct->rSelectedImage);
					TI.item.mask   = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
					TI.item.pszText = const_cast<char*>(pItemFileStruct->rItemText.operator LPCTSTR());
					hItem = pTreeCtrl->InsertItem(&TI);
					if(NULL != hItem) pTreeCtrl->SetItemData(hItem , DWORD_PTR(pItemFileStruct));
				}
			}
		}
	}
}
*/
/**
	@brief	 Event When Click Tree Item

	@author	KHS,BHK
*/
int CELoadItemViewBar::OnClickTreeItem(const CString&  rCommand , const CString& rTypeStr , const CString& rHeaderText)
{
	ELoadCodeProc((char*)(rCommand.operator LPCSTR()) , rTypeStr , rHeaderText);
	
	return ERROR_SUCCESS;
}

/**
	@brief	called when Splitter window layout changed

	@author	BHK
*/
int CELoadItemViewBar::OnChangeLayoutSplitterWnd(void)
{
	CRect rect;
	m_wndSplitter.GetPane(1,0)->GetWindowRect(&rect);
	m_wndSplitter.ScreenToClient(&rect);
	
	for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
	{
		itr->first->SetWindowPos(NULL , rect.left , rect.top , rect.Width() , rect.Height() , SWP_NOZORDER | SWP_NOACTIVATE);
		itr->first->RedrawWindow();
	}

	return ERROR_SUCCESS;
}


/**
	@ brief  Add Bus Group in Bus
        @author	KHS
*/
HTREEITEM CELoadItemViewBar::AddBusGroupTreeCtrlItem( CBusGroupItem* pBusGroupItem )
{
	assert(pBusGroupItem && "pBusGroupItem is NULL");

	if(pBusGroupItem) return m_wndBusTreeView.AddBusGroupTreeCtrlItem(pBusGroupItem);

	return NULL;
}

/**
	@brief  Find  Bus Group Item for Add Bus

        @author	KHS
*/
HTREEITEM CELoadItemViewBar::FindBusGroupItem( CBusGroupItem* pBusGroupItem , HTREEITEM hItem )
{
	assert(pBusGroupItem && "pBusGroupItem is NULL");
	assert(hItem && "hItem is NULL");

	if(pBusGroupItem && hItem )
	{
		HTREEITEM hSiblingItem = hItem;
		while(hSiblingItem)
		{
			CBusGroupItem* ptr = (CBusGroupItem*)m_wndBusTreeView.GetItemData(hSiblingItem);
			if(ptr == pBusGroupItem) return hSiblingItem;

			HTREEITEM hChild = m_wndBusTreeView.GetChildItem(hSiblingItem);
			if(NULL != hChild)
			{
				CBusGroupItem* ptr = (CBusGroupItem*)m_wndBusTreeView.GetItemData(hSiblingItem);
				if((NULL != ptr) && (pBusGroupItem == ptr)) return hSiblingItem;

				HTREEITEM hBusGroupItem = FindBusGroupItem( pBusGroupItem , hChild );
				if(NULL != hBusGroupItem) return hBusGroupItem;
			}
			hSiblingItem = m_wndBusTreeView.GetNextSiblingItem(hSiblingItem);
		}
	}

	return NULL;
}
/**
	@ brief  Add Bus in BusGroup

        @author	KHS
*/
int CELoadItemViewBar::AddBusTreeCtrlItem( CBusItem* pBusItem )
{
	assert(pBusItem && "pBusItem is NULL");
	if(pBusItem)
	{
		m_wndBusTreeView.AddBusTreeCtrlItem(pBusItem);
		m_wndItemClassTreeCtrl.AddTreeNodeOf(pBusItem);
	}
        
	return ERROR_SUCCESS;
}

/**
        @brief  UPDATE BUS TREE

        @author	KHS
*/
int CELoadItemViewBar::UpdateBusTreeView(HTREEITEM hItem , CELoadItem* pItem )
{
	m_wndBusTreeView.UpdateBusTreeView(hItem , pItem );
	
	return ERROR_SUCCESS;
}

/**
        @brief  UPDATE LOAD LIST CONTROL

        @author	HumKyung.BAEK
*/
int CELoadItemViewBar::UpdateLoadItem(CELoadItem* pItem )
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	LVFINDINFO finfo;
	finfo.flags  = LVFI_PARAM;
	finfo.lParam = (LPARAM)pItem;

	const int nItem = m_wndLoadListCtrl.FindItem(&finfo , -1);
	if(-1 != nItem)
	{		
		LVITEM lvi;
		lvi.iItem    = nItem;
		lvi.iSubItem = 0;
		lvi.mask     = LVIF_IMAGE;
		m_wndLoadListCtrl.GetItem(&lvi);

		const int nIconNo = docData.GetIconNo(pItem);
		lvi.iImage = nIconNo;
		m_wndLoadListCtrl.SetItem(&lvi);
	}

	return ERROR_SUCCESS;
}

/**
        @brief  On event from Observer

        @author	HumKyung.BAEK
*/
int CELoadItemViewBar::OnEventFromObserver(CObserver* pObserver , CEventData* pEventData)
{
	if(CEventData::MODIFY_ITEM_PROP == pEventData->m_eEventType)
	{
		if(CLoadItem::TypeString() == pEventData->GetItemCategory())
		{
			const int nItemCount = pEventData->GetItemCount();
			for(int i = 0;i < nItemCount;++i)
			{
				((CELoadItemViewBar*)(pObserver->owner()))->UpdateLoadItem( pEventData->GetItemAt(i) );
			}
		}
		else if(CBusItem::TypeString() == pEventData->GetItemCategory())
		{
			const int nItemCount = pEventData->GetItemCount();
			for(int i = 0;i < nItemCount;++i)
			{
				((CELoadItemViewBar*)(pObserver->owner()))->m_wndItemClassTreeCtrl.ModifyBusItem( (CBusItem*)(pEventData->GetItemAt(i)) );
			}
		}
	}
	else if(CEventData::ADD_ITEM == pEventData->m_eEventType)
	{
		const int nItemCount = pEventData->m_SelectionSet.GetItemCount();
		for(int i = 0;i < nItemCount;++i)
		{
			((CELoadItemViewBar*)(pObserver->owner()))->m_wndItemClassTreeCtrl.AddTreeNodeOf((pEventData->GetItemAt(i)));
		}
	}
	else if(CEventData::DELETE_ITEM == pEventData->m_eEventType)
	{
		const int nItemCount = pEventData->m_SelectionSet.GetItemCount();
		for(int i = 0;i < nItemCount;++i)
		{
			((CELoadItemViewBar*)(pObserver->owner()))->m_wndItemClassTreeCtrl.DeleteTreeNodeOf((pEventData->GetItemAt(i)));
		}
	}
	else if(CEventData::REORDER_ITEM == pEventData->m_eEventType)
	{
		const int nItemCount = pEventData->m_SelectionSet.GetItemCount();
		for(int i = 0;i < nItemCount;++i)
		{
			if(pEventData->GetItemAt(i)->IsKindOf(CBusGroupItem::TypeString()))
			{
				CBusGroupItem* pBusGroupItem = static_cast<CBusGroupItem*>(pEventData->GetItemAt(i));
				for(vector<CBusItem*>::iterator itr = pBusGroupItem->m_BusRefEntry.begin();itr != pBusGroupItem->m_BusRefEntry.end();++itr)
				{
					((CELoadItemViewBar*)(pObserver->owner()))->m_wndItemClassTreeCtrl.DeleteTreeNodeOf(*itr);
					((CELoadItemViewBar*)(pObserver->owner()))->m_wndItemClassTreeCtrl.AddTreeNodeOf(*itr);
				}
			}
			else
			{
				((CELoadItemViewBar*)(pObserver->owner()))->m_wndItemClassTreeCtrl.DeleteTreeNodeOf((pEventData->GetItemAt(i)));
				((CELoadItemViewBar*)(pObserver->owner()))->m_wndItemClassTreeCtrl.AddTreeNodeOf((pEventData->GetItemAt(i)));
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	SHow Select Item in only propety view bar

	@author	KHS
*/
void CELoadItemViewBar::OnItemShow()
{
	if(m_wndLoadListCtrl.IsWindowVisible())
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		docData.m_SelectedItemEntry.clear();

		CEventData EventData("" , CEventData::SELECT_ITEM);

		string rItemCategoryString;
		POSITION pos = m_wndLoadListCtrl.GetFirstSelectedItemPosition();
		while(pos)
		{
			const int nItem   = m_wndLoadListCtrl.GetNextSelectedItem(pos);
			CELoadItem* pItem = (CELoadItem*)m_wndLoadListCtrl.GetItemData( nItem );
			if(pItem)
			{
				rItemCategoryString = pItem->GetTypeString();
				EventData.m_SelectionSet.Add( pItem );

				docData.m_SelectedItemEntry.push_back(pItem);
			}
		}
		EventData.SetItemCategory( rItemCategoryString );

		m_pObserver->FireEvent(EventData);

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if(!pFrame->m_wndPropertiesBar.IsVisible())
		{
			pFrame->m_wndPropertiesBar.ShowPane(TRUE , FALSE , TRUE);
		}

		//! 뷰를 닫는다
		/*CELOADApp* pApp = (CELOADApp*)AfxGetApp();
		if(pApp) pApp->CloseDocument(CLoadItem::TypeString());*/
	}		
}

/**
        @brief  SelChange BUS TREE

        @author	KHS
*/
void CELoadItemViewBar::OnSelchangedBusTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	HTREEITEM hItem= pNMTreeView->itemNew.hItem;

	*pResult = 0;
}

/**
        @brief  SHow All Item in GridCtrl
        @author	KHS
*/
void CELoadItemViewBar::OnItemShowall()
{
	// TODO: Add your command handler code here
	if(m_wndLoadListCtrl.IsWindowVisible())
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		docData.m_SelectedItemEntry.clear();

		CEventData EventData("" , CEventData::SELECT_ITEM);

		string rItemCategoryString;
		const int nItemCount = m_wndLoadListCtrl.GetItemCount();
		for(int i = 0;i < nItemCount;i++)
		{
			const int nItem = i;
			CELoadItem* pItem = (CELoadItem*)m_wndLoadListCtrl.GetItemData( nItem );
			if(pItem)
			{
				rItemCategoryString = pItem->GetTypeString();
				EventData.m_SelectionSet.Add( pItem );

				docData.m_SelectedItemEntry.push_back(pItem);
			}
		}	
		EventData.SetItemCategory( rItemCategoryString );

		//! 속성창이 열려 있으면 닫는다.
		/*CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if(pFrame->m_wndPropertiesBar.IsVisible())
		{
		pFrame->m_wndPropertiesBar.ShowPane(FALSE , FALSE , FALSE);
		}*/

		CELOADApp* pApp = (CELOADApp*)AfxGetApp();
		BOOL res = pApp->OpenNewDocumentIfNeed(CLoadItem::TypeString());
		//m_pObserver->FireEvent(EventData);
	}		
}

/**
        @brief  

        @author	BHK
*/
void CELoadItemViewBar::OnCalcLoadSummary()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if(pFrame)
	{
		pFrame->SendMessage(WM_COMMAND , ID_CALC_LOAD_SUMMARY , 0);
	}
}

/**
        @brief  

        @author	BHK
*/
void CELoadItemViewBar::OnCalcCableSize()
{
        CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if(pFrame)
	{
		pFrame->SendMessage(WM_COMMAND , ID_CALC_CABLE_SIZE , 0);
	}
}

/**
        @brief	LOAD ITEM Copy for Create Item in Preview List

        @author KHS
*/
void CELoadItemViewBar::OnItemCopy()
{
	if(m_wndLoadListCtrl.IsWindowVisible())
	{
		m_wndLoadListCtrl.OnItemCopy();
	}
}

/**
        @brief display Load Item Count

	@author	KHS
*/
void CELoadItemViewBar::DisplayItemCount(void)
{
	TCHAR szBuf[MAX_PATH] = {'\0',};

	//! COLUMN의 텍스트를 읽어온다.
	LVCOLUMN lvcolumn;
	lvcolumn.mask = LVCF_TEXT;
	lvcolumn.cchTextMax = MAX_PATH;
	lvcolumn.pszText= szBuf;
	m_wndLoadListCtrl.GetColumn(0 , &lvcolumn);
	CString rString = lvcolumn.pszText;
	const int at = rString.Find(_T("("));
	if(-1 != at) rString = rString.Left(at);

	CString rItemText;
	rItemText.Format(_T("%s(%d)") , rString.operator LPCSTR() , m_wndLoadListCtrl.GetItemCount());
	
	lvcolumn.mask = LVCF_TEXT;
	lvcolumn.pszText = LPSTR(rItemText.operator LPCSTR());
	m_wndLoadListCtrl.SetColumn(0 , &lvcolumn);
}

/**
	@brief	항상 존재해야 하는 BusGroup(INCOMING,HV,LV,SubBUS)을 생성한다.

	@author HumKyung	

	@date 2009-04-04 오전 1:21:47	

	@param	

	@return		
**/
void CELoadItemViewBar::CreateSystemBusGroup()
{
        CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> BusGroupItemList;
	docData.GetELoadItemListOf(BusGroupItemList , CBusGroupItem::TypeString());
	//if(pItemList)
	{
		HTREEITEM hFirstBusGroupTreeItem = NULL;
		for(list<string>::iterator itr = docData.m_lstSystemBusGroup.begin();itr != docData.m_lstSystemBusGroup.end();++itr)
		{
			//! system bus tree item
			HTREEITEM hBusTreeItem = NULL;
			list<CELoadItem*>::iterator where = find_if(BusGroupItemList.begin() , BusGroupItemList.end() , bind2nd(ITEMS::MatchItemName() , *itr));
			if(where == BusGroupItemList.end())
			{
				CBusGroupItem* pNewBusGroupItem = static_cast<CBusGroupItem*>(docData.CreateELoadItemOf(CBusGroupItem::TypeString()));
				if(pNewBusGroupItem)
				{
					CELoadItemProp* pItemProp = pNewBusGroupItem->prop();
					pItemProp->SetOriginalValue(_T("GENERAL") , _T("BUS GROUP ID") ,*itr);
					pItemProp->SetValue(_T("GENERAL") , _T("BUS GROUP ID") , *itr);

					hBusTreeItem = AddBusGroupTreeCtrlItem( pNewBusGroupItem );
				} 
			}else	(hBusTreeItem = AddBusGroupTreeCtrlItem( static_cast<CBusGroupItem*>(*where) ));

			if(NULL == hFirstBusGroupTreeItem) hFirstBusGroupTreeItem = hBusTreeItem;
		}

		if(hFirstBusGroupTreeItem) m_wndBusTreeView.SelectItem(hFirstBusGroupTreeItem);
	}
}

void CELoadItemViewBar::OnItemDelete()
{
        m_wndLoadListCtrl.OnItemDelete();
}

/**
	@brief	선택한 LOAD에 대해서 POWER CABLE을 생성한다.

	@author BHK	

	@date 2009-07-09 오후 12:56:42	

	@param	

	@return		
*/
void CELoadItemViewBar::OnItemCreatePowerCableOfSelectedLoad()
{
	COMMAND::CCommandPowerCableCreation cmdPowerCableCreation;
	POSITION pos = m_wndLoadListCtrl.GetFirstSelectedItemPosition();
	while(pos)
	{
		const int nItem = m_wndLoadListCtrl.GetNextSelectedItem(pos);
		CELoadItem* pItem = (CELoadItem*)m_wndLoadListCtrl.GetItemData(nItem);
		cmdPowerCableCreation.m_InterestingItemList.push_back(pItem);
	}
	ELOAD_LOG4CXX_RESET(mylogger , _T("Auto Power Cable Error Messages"));
	if(ERROR_SUCCESS == cmdPowerCableCreation.Execute(true))
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		docData.SetModifiedFlag(true);

		///
		CString rMsg;
		const string rResultString = cmdPowerCableCreation.GetResultString();
		if(docData.m_nErrorCount > 0)
			rMsg.Format("%s%d error(s)" , rResultString.c_str() , docData.m_nErrorCount);
		else	rMsg.Format("%s" , rResultString.c_str());

		CDataResultDlg dlg(rMsg);
		dlg.DoModal();

		//!CABLE SIZING RESULT VIEW를 생성
		CELOADApp* pApp = (CELOADApp*)AfxGetApp();
		if(pApp)
		{
			CCableSizingResultDoc::m_nLoadPowerCableCreation = CCableSizingResultDoc::UNSIZED_POWER_CABLE_ONLY;
			pApp->OpenNewDocumentIfNeed(_T("Auto Power Cable") , _T("Create Auto Power Cable"));

			AppenderPtr appender = mylogger->getAppender(_T("ELoadAppender"));
			FileAppenderPtr fileappender = (FileAppenderPtr)appender;
			const string rLogFilePath = fileappender->getFile();
			pApp->PutLogFileToDatabase(rLogFilePath, _T("Auto Power Cable"));

			//! LOG 출력
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			if(pFrame)
			{
				pFrame->m_wndOutput.Navigate2(rLogFilePath.c_str());

				///pFrame->m_wndELoadItemView.m_wndItemClassTreeCtrl.DisplayCableItemCount();
			}
		}
	}
}

/**
	@brief	선택한 LOAD에 대해서 CONTROL CABLE을 생성한다.

	@author BHK	

	@date 2009-07-09 오후 12:57:06	

	@param	

	@return		
*/
void CELoadItemViewBar::OnItemCreateControlCableOfSelectedLoad()
{
	COMMAND::CCommandControlCableCreation cmdControlCableCreation;
	POSITION pos = m_wndLoadListCtrl.GetFirstSelectedItemPosition();
	while(pos)
	{
		const int nItem = m_wndLoadListCtrl.GetNextSelectedItem(pos);
		CELoadItem* pItem = (CELoadItem*)m_wndLoadListCtrl.GetItemData(nItem);
		cmdControlCableCreation.m_InterestingItemList.push_back(pItem);
	}

	ELOAD_LOG4CXX_RESET(mylogger ,_T("Auto Control Cable Error Messages"));
	if(ERROR_SUCCESS == cmdControlCableCreation.Execute(true))
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();

		//! modified flag를 setting한다.
		docData.SetModifiedFlag(true);

		CString rMsg;
		const string rResultString = cmdControlCableCreation.GetResultString();
		if(docData.m_nErrorCount > 0)
			rMsg.Format("%s%d error(s)" , rResultString.c_str() , docData.m_nErrorCount);
		else	rMsg.Format("%s" , rResultString.c_str());
		//AfxMessageBox(rMsg);
		CDataResultDlg dlg(rMsg);
		dlg.DoModal();

		//! CONTROL CABLE RESULT VIEW
		CELOADApp* pApp = (CELOADApp*)AfxGetApp();
		if(pApp)
		{
			pApp->OpenNewDocumentIfNeed(_T("CONTROL CABLE") , _T("CREATE CONTROL CABLE"));

			//! LOG 출력
			AppenderPtr appender = mylogger->getAppender("ELoadAppender");
			FileAppenderPtr fileappender = (FileAppenderPtr)appender;
			const string rLogFilePath = fileappender->getFile();
			pApp->PutLogFileToDatabase(rLogFilePath, "Auto Control Cable");

			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			if(pFrame)
			{
				pFrame->m_wndOutput.Navigate2(rLogFilePath.c_str());

				///pFrame->m_wndELoadItemView.m_wndItemClassTreeCtrl.DisplayCableItemCount();
			}
		}
	}
}

void CELoadItemViewBar::OnEditCut()
{
}

void CELoadItemViewBar::OnEditPaste()
{

}

void CELoadItemViewBar::OnEditClear()
{
}

/**
	@brief	SHow Select Item in only propety sheet
	
	@author	BHK
*/
void CELoadItemViewBar::OnItemShowList()
{
	if(m_wndLoadListCtrl.IsWindowVisible())
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		docData.m_SelectedItemEntry.clear();

		CEventData EventData("" , CEventData::SELECT_ITEM);

		string rItemCategoryString;
		POSITION pos = m_wndLoadListCtrl.GetFirstSelectedItemPosition();
		while(pos)
		{
			const int nItem   = m_wndLoadListCtrl.GetNextSelectedItem(pos);
			CELoadItem* pItem = (CELoadItem*)m_wndLoadListCtrl.GetItemData( nItem );
			if(pItem)
			{
				rItemCategoryString = pItem->GetTypeString();
				EventData.m_SelectionSet.Add( pItem );
				docData.m_SelectedItemEntry.push_back(pItem);
			}
		}

		EventData.SetItemCategory( rItemCategoryString );
		CELOADApp* pApp = (CELOADApp*)AfxGetApp();
		BOOL res = pApp->OpenNewDocumentIfNeed(CLoadItem::TypeString());

		//! 속성창이 열려있으면 닫는다.
		/*CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if(pFrame->m_wndPropertiesBar.IsVisible())
		{
		pFrame->m_wndPropertiesBar.ShowPane(FALSE , FALSE , FALSE);
		}*/
	}
}

/**
	@brief	load item을 생성합니다.

	@author	BHK
*/
void CELoadItemViewBar::OnCreateLoad()
{
	if(m_wndLoadListCtrl.IsWindowVisible()) 
		m_wndLoadListCtrl.CreateItem(CLoadItem::TypeString());
}

/**
	@brief	Area에 속하는 Load들을 뿌려준다.

	@author BHK	

	@date 2009-05-28 오후 7:00:35	

	@param	

	@return		
*/
int CELoadItemViewBar::ShowAreaCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	if((pCommandStr != m_wndLoadListCtrl.GetCommandString()) || (pParam != m_wndLoadListCtrl.GetArea()))
	{
		m_wndLoadListCtrl.SetCommandString(pCommandStr);

		//! 한번만 LOAD ENTRY에 있는 데이타를 리스트에 컨트롤에 뿌려준다.
		list<CELoadItem*> LoadItemList;
		docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
		
		vector<CLoadItem*> LoadItemEntry;
		//if(pLoadItemList)
		{	
			for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
			{
				if((*itr)->IsDeleted()) continue;

				CLoadItem* pLoad = static_cast<CLoadItem*>(*itr);
				const string rArea = pLoad->prop()->GetValue(_T("Location") , _T("Area"));
				if((_T("") == pParam) || ((string(_T("NONE")) == pParam) && rArea.empty()) || (rArea == pParam))
					LoadItemEntry.push_back(pLoad);
			}
		}
		RemakeLoadListCtrl(_T("") , ((_T("") == pParam) || (string(_T("NONE")) == pParam)) ? _T("") : pParam , LoadItemEntry);

		m_wndLoadListCtrl.ShowWindow(SW_NORMAL);
		for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
		{
			if(itr->first != &m_wndLoadListCtrl) itr->first->ShowWindow(SW_HIDE);
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	callback function "SHOW BUS"

	@author	HumKyung

	@date	????.??.??

	@return
*/
int CELoadItemViewBar::ShowBusCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	m_wndBusTreeView.SetActionString(_T("SHOW BUS"));
	m_wndBusTreeView.ShowWindow(SW_NORMAL);
	for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
	{
		if(itr->first != &m_wndBusTreeView) itr->first->ShowWindow(SW_HIDE);
	}

	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	CString strDocName;
	CDocTemplate* pSelectedTemplate = NULL;
	POSITION pos = pApp->GetFirstDocTemplatePosition();
	while (pos != NULL) 
	{
		pSelectedTemplate = (CDocTemplate*) pApp->GetNextDocTemplate(pos);
		pSelectedTemplate->GetDocString(strDocName, CDocTemplate::docName);
		if (strDocName == "BUS")
		{
			POSITION docPos = pSelectedTemplate->GetFirstDocPosition();
			/*if(NULL == docPos)*/
				pApp->OpenNewDocumentIfNeed(_T("BUS"));
			/*else
			{
				CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
				CMFCTabCtrl& TabWnd = pFrame->GetMDITabs();

				CDocument* pDoc = pSelectedTemplate->GetNextDoc(docPos);
				POSITION viewPos = pDoc->GetFirstViewPosition();
				CView* pView = pDoc->GetNextView(viewPos);

				const int nTab = TabWnd.GetTabFromHwnd(pView->GetParent()->GetSafeHwnd());
				TabWnd.SetActiveTab(nTab);
			}*/
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	PDB Bus group에 속한 모든 Load들을 보여준다.

	@author BHK	

	@date 2009-04-05 오후 4:32:14	

	@param	

	@return		
**/
int CELoadItemViewBar::ShowPDBCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	if( pCommandStr != m_wndLoadListCtrl.GetCommandString() )
	{
		m_wndLoadListCtrl.SetCommandString(pCommandStr);

		//! 한번만 LOAD ENTRY에 있는 데이타를 리스트에 컨트롤에 뿌려준다.
		list<CELoadItem*> LoadItemList;
		docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());

		vector<CLoadItem*> LoadItemEntry;
		{	
			for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
			{
				CLoadItem* pLoad = static_cast<CLoadItem*>(*itr);
				const string rBusId = pLoad->GetBusId();
				CBusItem* pBusItem  = docData.FindBusItemByName(rBusId);
				if(pBusItem)
				{
					CBusGroupItem* pBusGroupItem = pBusItem->GetBusGroupItemPtr();
					if(pBusGroupItem)
					{
						if(SUBBUS == pBusGroupItem->GetName()) LoadItemEntry.push_back(pLoad);
					}
				}
			}
		}
		RemakeLoadListCtrl(_T("") , _T("") , LoadItemEntry);

		m_wndLoadListCtrl.ShowWindow(SW_NORMAL);
		for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
		{
			if(itr->first != &m_wndLoadListCtrl) itr->first->ShowWindow(SW_HIDE);
		}
	}

	return ERROR_SUCCESS;
}

/**
	@biref	

	@author	BHK
*/
int CELoadItemViewBar::ShowLoadCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	///if(!m_wndLoadListCtrl.IsWindowVisible() && (pCommandStr != m_wndLoadListCtrl.GetCommandString()))
	{
		m_wndLoadListCtrl.SetCommandString(pCommandStr);

		//! 한번만 LOAD ENTRY에 있는 데이타를 리스트에 컨트롤에 뿌려준다.
		list<CELoadItem*> LoadItemList;
		docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());

		vector<CLoadItem*> LoadItemEntry;
		//if(pLoadItemList)
		{	
			for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
			{
				if((*itr)->IsDeleted()) continue;

				CLoadItem* pLoad = static_cast<CLoadItem*>(*itr);
				const string rBusId = pLoad->GetBusId();

				if(rBusId.empty()) continue;

				CBusItem* pBusItem  = docData.FindBusItemByName(rBusId);
				if(pBusItem)
				{
					CBusGroupItem* pBusGroupItem = pBusItem->GetBusGroupItemPtr();
					if(pBusGroupItem)
					{
						if(SUBBUS != pBusGroupItem->GetName()) LoadItemEntry.push_back(pLoad);
					}
					else
					{
						//! USER BUS에 Assigned된 AUTO LOAD
						LoadItemEntry.push_back(pLoad);
					}
				}else	LoadItemEntry.push_back(pLoad);
			}
		}
		RemakeLoadListCtrl(_T("") , _T("") , LoadItemEntry);

		m_wndLoadListCtrl.ShowWindow(SW_NORMAL);
		for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
		{
			if(itr->first != &m_wndLoadListCtrl) itr->first->ShowWindow(SW_HIDE);
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-06-30 오전 11:28:36	

	@param	

	@return		
*/
int CELoadItemViewBar::ShowCableScheduleCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp)
	{
		CString rParam;
		rParam = CString(pParam).IsEmpty() ? 
			CString(_T("Show Cable Schedule")) : CString(_T("Cable Schedule|")) + pParam;
		pApp->OpenNewDocumentIfNeed(_T("Cable Schedule") , rParam);
	}
	
	return ERROR_SUCCESS;
}

/**
	@brief	

	@author KHS	

	@date 2009-09-29 오후 3:47:39	

	@param	

	@return		
**/
int CELoadItemViewBar::ShowDrumScheduleCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp) pApp->OpenNewDocumentIfNeed(_T("Drum Schedule") , 
		CString(pParam).IsEmpty() ? CString(_T("Drum Schedule")) : CString(_T("Drum Schedule|")) + pParam);
	
	return ERROR_SUCCESS;
}

/**
	@brief	

	@author KHS	

	@date 2009-11-06 오후 2:38:16	

	@param	

	@return		
**/
int CELoadItemViewBar::ShowLoadListSldCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	BeginWaitCursor();

	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp) pApp->OpenNewDocumentIfNeed(_T("LOAD LIST SLD") , 
		CString(pParam).IsEmpty() ? CString(_T("LOAD LIST SLD")) : CString(_T("LOAD LIST SLD|")) + pParam);
	
	EndWaitCursor();

	return ERROR_SUCCESS;
}
/**
	@brief	BUS에 할당되지 않은 LOAD들을 표기한다.

	@author BHK	

	@date 2009-06-18 오후 1:03:50	

	@param	

	@return		
*/
int CELoadItemViewBar::ShowNotAssignedLoadCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	if( pCommandStr != m_wndLoadListCtrl.GetCommandString() )
	{
		m_wndLoadListCtrl.SetCommandString(pCommandStr);

		//! 한번만 LOAD ENTRY에 있는 데이타를 리스트에 컨트롤에 뿌려준다.
		list<CELoadItem*> LoadItemList;
		docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());

		vector<CLoadItem*> LoadItemEntry;
		{	
			for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
			{
				CLoadItem* pLoad = static_cast<CLoadItem*>(*itr);
				const string rBusId = pLoad->GetBusId();
				CBusItem* pBusItem  = docData.FindBusItemByName(rBusId);
				if(NULL == pBusItem) LoadItemEntry.push_back(pLoad);
			}
		}
		RemakeLoadListCtrl(_T("") , _T("") , LoadItemEntry);

		m_wndLoadListCtrl.ShowWindow(SW_NORMAL);
		for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
		{
			if(itr->first != &m_wndLoadListCtrl) itr->first->ShowWindow(SW_HIDE);
		}
	}

	return ERROR_SUCCESS;
}

/**
	@biref	선택한 BUS에 속한 LOAD들을 보여준다.

	@author	BHK
*/
int CELoadItemViewBar::ShowLoadInBusCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	assert(pParam && "pParam is NULL");

	if(pParam)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		if((pCommandStr != m_wndLoadListCtrl.GetCommandString()) || (pParam != m_wndLoadListCtrl.GetBusID()))
		{
			m_wndLoadListCtrl.SetCommandString(pCommandStr);
			list<CELoadItem*> LoadItemList;
			docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
			{
				vector<CLoadItem*> LoadItemEntry;
				for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
				{
					CLoadItem* pLoad = static_cast<CLoadItem*>(*itr);
					if(pLoad)
					{
						const string rBusId = pLoad->prop()->GetValue(_T("Panel") , _T("Bus ID"));
						if(rBusId == pParam) LoadItemEntry.push_back(pLoad);
					}
				}
				RemakeLoadListCtrl(pParam , _T("") , LoadItemEntry);

				m_wndLoadListCtrl.ShowWindow(SW_NORMAL);
				for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
				{
					if(itr->first != &m_wndLoadListCtrl) itr->first->ShowWindow(SW_HIDE);
				}
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	callback function "SHOW LOAD SUMMARY"

	@author	BHK
*/
int CELoadItemViewBar::ShowLoadSummaryCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp) pApp->OpenNewDocumentIfNeed(_T("LOAD SUMMARY") , _T("ONLY LOAD SUMMARY"));
	
	m_wndEmpty.ShowWindow(SW_NORMAL);
	for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
	{
		if(itr->first != &m_wndEmpty) itr->first->ShowWindow(SW_HIDE);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	callback function "SHOW ALL BUS LOAD SUMMARY"

	@author	BHK

	@date	?
*/
int CELoadItemViewBar::ShowAllBUSLoadSummaryCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	BeginWaitCursor();

	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp)
	{
		LONG lRecordCount = 0L;
		{
			CELoadDocData& docData = CELoadDocData::GetInstance();
			const string rMDBFilePath = docData.GetProjectMDBFilePath();

			CADODB adoDB;
			const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;
			if(TRUE == adoDB.DBConnect(rConnectionString))
			{
				const CString rSql = (_T("SELECT * FROM T_PRODUCT_REVISION WHERE C_PRODUCT_NAME = 'Load Summary-BUS' ORDER BY C_REVISION_NO DESC"));

				adoDB.OpenQuery(rSql.operator LPCTSTR());
				adoDB.GetRecordCount(&lRecordCount);
			}
		}

		if(lRecordCount > 0)
		{
			//! compare current status with last status.
			CCompareRevisionDataDoc::COMPARE_ITEM CompareItem;
			CompareItem.rProductName    = _T("Load Summary-BUS");
			CompareItem.nLeftSideRevNo  = -1;
			CompareItem.nRightSideRevNo = -1;
			const BOOL res = pApp->OpenNewDocumentIfNeed(_T("REVISION") , LPCTSTR(&CompareItem));
		}
		else
		{
			pApp->OpenNewDocumentIfNeed(_T("LOAD SUMMARY") , _T("ALL BUS LOAD SUMMARY"));
		}
	}

	m_wndEmpty.ShowWindow(SW_SHOW);
	for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
	{
		if(itr->first != &m_wndEmpty) itr->first->ShowWindow(SW_HIDE);
	}
	
	EndWaitCursor();

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-06 오후 7:23:07	

	@param	

	@return		
*/
int CELoadItemViewBar::ShowAllRevBUSLoadSummaryCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp) pApp->OpenNewDocumentIfNeed(_T("LOAD SUMMARY") , _T("ALL REV BUS LOAD SUMMARY|") + CString(pParam));

	m_wndEmpty.ShowWindow(SW_SHOW);
	for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
	{
		if(itr->first != &m_wndEmpty) itr->first->ShowWindow(SW_HIDE);
	}
	
	return ERROR_SUCCESS;
}

/**
	@brief	callback function "SHOW ALL PDB LOAD SUMMARY"

	@author	BHK

	@date	?

	@param

	@return
*/
int CELoadItemViewBar::ShowAllPDBLoadSummaryCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	BeginWaitCursor();

	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp)
	{
		LONG lRecordCount = 0L;
		{
			CELoadDocData& docData = CELoadDocData::GetInstance();
			const string rMDBFilePath = docData.GetProjectMDBFilePath();

			CADODB adoDB;
			const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;
			if(TRUE == adoDB.DBConnect(rConnectionString))
			{
				const CString rSql = (_T("SELECT * FROM T_PRODUCT_REVISION WHERE C_PRODUCT_NAME = 'Load Summary-SubBUS' ORDER BY C_REVISION_NO DESC"));

				adoDB.OpenQuery(rSql.operator LPCTSTR());
				adoDB.GetRecordCount(&lRecordCount);
			}
		}

		if(lRecordCount > 0)
		{
			CCompareRevisionDataDoc::COMPARE_ITEM CompareItem;
			CompareItem.rProductName    = _T("Load Summary-SubBUS");
			CompareItem.nLeftSideRevNo  = -1;
			CompareItem.nRightSideRevNo = -1;
			const BOOL res = pApp->OpenNewDocumentIfNeed(_T("REVISION") , LPCTSTR(&CompareItem));
		}
		else
		{
			pApp->OpenNewDocumentIfNeed(_T("LOAD SUMMARY") , _T("ALL PDB LOAD SUMMARY"));
		}
	}

	m_wndEmpty.ShowWindow(SW_SHOW);
	for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
	{
		if(itr->first != &m_wndEmpty) itr->first->ShowWindow(SW_HIDE);
	}
	
	EndWaitCursor();

	return ERROR_SUCCESS;
}

/**
	@brief	callback function "SHOW ALL REV PDB LOAD SUMMARY"

	@author BHK	

	@date 2009-05-06 오후 8:18:32	

	@param	

	@return		
*/
int CELoadItemViewBar::ShowAllRevPDBLoadSummaryCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp) pApp->OpenNewDocumentIfNeed(_T("LOAD SUMMARY") , _T("ALL REV PDB LOAD SUMMARY|") + CString(pParam));

	m_wndEmpty.ShowWindow(SW_SHOW);
	for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
	{
		if(itr->first != &m_wndEmpty) itr->first->ShowWindow(SW_HIDE);
	}
	
	return ERROR_SUCCESS;
}

/**
	@brief	callback function "SHOW LOAD SUMMARY IN BUS"

	@author	BHK
*/
int CELoadItemViewBar::ShowLoadSummaryInBusCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	m_wndBusTreeView.SetActionString(_T("SHOW LOAD SUMMARY IN BUS"));
	
	m_wndBusTreeView.ShowWindow(SW_NORMAL);
	for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
	{
		if(itr->first != &m_wndBusTreeView) itr->first->ShowWindow(SW_HIDE);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	callback function "SHOW CABLE ROUTE"

	@author KHS	

	@date 2009-06-16 오후 3:03:39	

	@param	

	@return		
**/
int CELoadItemViewBar::ShowCableRouteResultCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp) pApp->OpenNewDocumentIfNeed(_T("ROUTE CABLE LIST") , _T("ROUTE CABLE LIST") + CString(pParam));

	m_wndEmpty.ShowWindow(SW_SHOW);
	for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
	{
		if(itr->first != &m_wndEmpty) itr->first->ShowWindow(SW_HIDE);
	}
	
	return ERROR_SUCCESS;
}

/**
	@brief	callback function "SHOW CABLE SIZING TABLE"

	@author	BHK
*/
int CELoadItemViewBar::ShowCableSizingTableCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	BeginWaitCursor();

	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(_T("") != pParam)
	{
		if(pApp) pApp->OpenNewDocumentIfNeed(_T("CABLE SIZING TABLE") , pParam);
	}
	else
	{
		if(pApp) pApp->OpenNewDocumentIfNeed(_T("CABLE SIZING TABLE") , _T("CABLE SIZING TABLE"));
	}

	m_wndEmpty.ShowWindow(SW_SHOW);
	for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
	{
		if(itr->first != &m_wndEmpty) itr->first->ShowWindow(SW_HIDE);
	}
	
	EndWaitCursor();

	return ERROR_SUCCESS;
}

/**
	@brief	SHOW SCHEMATIC

	@author BHK	

	@date 2009-05-12 오후 3:58:29	

	@param	

	@return		
*/
int CELoadItemViewBar::ShowSchematicCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	m_wndSchematicListCtrl.SetRedraw(FALSE);
	CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_TYPE"));
	if(pTable)
	{
		const int nRecordCount = pTable->m_RecordEntry.size();
		
		const int nItemCount = m_wndSchematicListCtrl.GetItemCount();
		if((0 == nItemCount) || (nItemCount != nRecordCount))
		{
			m_wndSchematicListCtrl.DeleteAllItems();

			int nIndex = 0;
			for(vector<TABLES::CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin();itr != pTable->m_RecordEntry.end();++itr)
			{
				const int nItem = m_wndSchematicListCtrl.InsertItem(nIndex++ , itr->FieldValueMap[_T("C_TYPE")].c_str());
			}
		}
		m_wndSchematicListCtrl.SetRedraw(TRUE);

		m_wndSchematicListCtrl.ShowWindow(SW_NORMAL);
		for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
		{
			if(itr->first != &m_wndSchematicListCtrl) itr->first->ShowWindow(SW_HIDE);
		}
	}
	
	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-12 오후 3:58:41	

	@param	

	@return		
*/
int CELoadItemViewBar::ShowIOCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	m_wndIOListCtrl.SetRedraw(FALSE);
	CELOADTable* pTable = docData.GetTableOf(_T("T_CONTROL_TYPE"));
	if(pTable)
	{
		m_wndIOListCtrl.DeleteAllItems();
		int nIndex = 0;
		for(vector<TABLES::CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin();itr != pTable->m_RecordEntry.end();++itr)
		{
			const string rCategory = itr->FieldValueMap[_T("C_CATEGORY")];
			if(string::npos != rCategory.find(_T("IO")))
			{
				const int nItem = m_wndIOListCtrl.InsertItem(nIndex++ , itr->FieldValueMap[_T("C_TYPE")].c_str());
			}
		}
		m_wndIOListCtrl.SetRedraw(TRUE);

		m_wndIOListCtrl.ShowWindow(SW_NORMAL);
		for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
		{
			if(itr->first != &m_wndIOListCtrl) itr->first->ShowWindow(SW_HIDE);
		}
	}
	
	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-12 오후 3:58:53	

	@param	

	@return		
*/
int CELoadItemViewBar::ShowLCSCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	m_wndLCSListCtrl.SetRedraw(FALSE);
	CELOADTable* pTable = docData.GetTableOf(_T("T_CONTROL_TYPE"));
	if(pTable)
	{
		m_wndLCSListCtrl.DeleteAllItems();
		int nIndex = 0;
		for(vector<TABLES::CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin();itr != pTable->m_RecordEntry.end();++itr)
		{
			const string rCategory = itr->FieldValueMap[_T("C_CATEGORY")];
			if(string::npos != rCategory.find(_T("LCS")))
			{
				const int nItem = m_wndLCSListCtrl.InsertItem(nIndex++ , itr->FieldValueMap[_T("C_TYPE")].c_str());
			}
		}
		m_wndLCSListCtrl.SetRedraw(TRUE);

		m_wndLCSListCtrl.ShowWindow(SW_NORMAL);
		for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
		{
			if(itr->first != &m_wndLCSListCtrl) itr->first->ShowWindow(SW_HIDE);
		}
	}
	
	return ERROR_SUCCESS;
}
/**
	@brief	callback function "SHOW POWER CABLE"

	@author	BHK

	@date	?
*/
int CELoadItemViewBar::ShowPowerCableCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	assert(pParam && "pParam is NULL");
	
	CELoadDocData& docData = CELoadDocData::GetInstance();

	m_wndPowerCableListCtrl.SetRedraw(FALSE);
	list<CELoadItem*> CableList;
	docData.GetELoadItemListOf(CableList , CCableItem::TypeString());
	const int nCableCount = int(count_if(CableList.begin() , CableList.end() , CELoadItem::IsNotDeleted));
	
	const int nItemCount = m_wndPowerCableListCtrl.GetItemCount();
	if((0 == nItemCount) || (nItemCount != nCableCount))
	{
		list<CELoadItem*> CableItemList;
		for(list<CELoadItem*>::iterator itr = CableList.begin();itr != CableList.end();++itr)
		{
			CString rCableCategory = (*itr)->prop()->GetValue(_T("General") , _T("Cable Category")).c_str();
			CString rCreateBy = (*itr)->prop()->GetValue(_T("General") , _T("Created By")).c_str();
			if(_T("POWER CABLE") == rCableCategory.MakeUpper() && _T("AUTO") == rCreateBy.MakeUpper()) CableItemList.push_back(*itr);
		}

		m_wndPowerCableListCtrl.RemakeListCtrl(_T("Power Cable Item") , &CableItemList);
	}
	m_wndPowerCableListCtrl.SetRedraw(TRUE);

	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp)
	{
		CCableSizingResultDoc::m_InterestingLoadItemList.clear();
		//! 모든 Auto POWER CABLE을 표시하도록 한다.
		list<CELoadItem*> InerestingItemList;
		docData.GetELoadItemListOf(InerestingItemList , CLoadItem::TypeString());
		CCableSizingResultDoc::m_InterestingLoadItemList.insert(CCableSizingResultDoc::m_InterestingLoadItemList.end() , InerestingItemList.begin() , InerestingItemList.end());

		docData.GetELoadItemListOf(InerestingItemList , CTransformerItem::TypeString());
		CCableSizingResultDoc::m_InterestingLoadItemList.insert(CCableSizingResultDoc::m_InterestingLoadItemList.end() , InerestingItemList.begin() , InerestingItemList.end());

		docData.GetELoadItemListOf(InerestingItemList , CCapacitorItem::TypeString());
		CCableSizingResultDoc::m_InterestingLoadItemList.insert(CCableSizingResultDoc::m_InterestingLoadItemList.end() , InerestingItemList.begin() , InerestingItemList.end());

		docData.GetELoadItemListOf(InerestingItemList , CUPSDCItem::TypeString());
		CCableSizingResultDoc::m_InterestingLoadItemList.insert(CCableSizingResultDoc::m_InterestingLoadItemList.end() , InerestingItemList.begin() , InerestingItemList.end());

		///pApp->OpenNewDocumentIfNeed(_T("Auto Power Cable") , _T("Show Auto Power Cable"));
	}

	const int res = OpenDocumentDependOn(_T("Auto Power Cable") , _T("Auto Power Cable") , _T("Show Auto Power Cable"));
	if(ERROR_SUCCESS == res)
	{
		m_wndPowerCableListCtrl.ShowWindow(SW_NORMAL);
		for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
		{
			if(itr->first != &m_wndPowerCableListCtrl) itr->first->ShowWindow(SW_HIDE);
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	callback function "SHOW CONTROL CABLE"

	@author	BHK
*/
int CELoadItemViewBar::ShowControlCableCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	assert(pParam && "pParam is NULL");
	
	CELoadDocData& docData = CELoadDocData::GetInstance();

	m_wndControlCableListCtrl.SetRedraw(FALSE);
	list<CELoadItem*> CableList;
	docData.GetELoadItemListOf(CableList , CCableItem::TypeString());
	const int nCableCount = int(count_if(CableList.begin() , CableList.end() , CELoadItem::IsNotDeleted));

	const int nItemCount = m_wndControlCableListCtrl.GetItemCount();
	if((0 == nItemCount) || (nItemCount != nCableCount))
	{
		list<CELoadItem*> CableItemList;
		for(list<CELoadItem*>::iterator itr = CableList.begin();itr != CableList.end();++itr)
		{
			CString rCableCategory = (*itr)->prop()->GetValue(_T("General") , _T("Cable Category")).c_str();
			CString rCreateBy = (*itr)->prop()->GetValue(_T("General") , _T("Created By")).c_str();
			if(_T("CONTROL CABLE") == rCableCategory.MakeUpper() && _T("AUTO") == rCreateBy.MakeUpper()) CableItemList.push_back(*itr);
		}

		m_wndControlCableListCtrl.RemakeListCtrl(_T("Control Cable Item") , &CableItemList);
	}
	m_wndControlCableListCtrl.SetRedraw(TRUE);
	
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp)
	{
		docData.GetELoadItemListOf(CControlCableResultDoc::m_InterestingLoadItemList , CLoadItem::TypeString());
		pApp->OpenNewDocumentIfNeed(_T("CONTROL CABLE") , _T("SHOW CONTROL CABLE"));
	}

	m_wndControlCableListCtrl.ShowWindow(SW_NORMAL);
	for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
	{
		if(itr->first != &m_wndControlCableListCtrl) itr->first->ShowWindow(SW_HIDE);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	callback function "SHOW USER CABLE"

	@author	BHK
*/
int CELoadItemViewBar::ShowUserCableCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	assert(pParam && "pParam is NULL");
	
	CELoadDocData& docData = CELoadDocData::GetInstance();

	list<CELoadItem*> CableList;
	docData.GetELoadItemListOf(CableList , CCableItem::TypeString(), _T("User"));

	list<CELoadItem*> CableItemList;
	for(list<CELoadItem*>::iterator itr = CableList.begin();itr != CableList.end();++itr)
	{
		CString rCereatedBy = (*itr)->prop()->GetValue(_T("General") , _T("Created By")).c_str();
		if(_T("USER") == rCereatedBy.MakeUpper()) CableItemList.push_back(*itr);
	}

	const int nItemCount = m_wndUserCableListCtrl.GetItemCount();
	if(nItemCount != CableItemList.size())
	{
		m_wndControlCableListCtrl.SetRedraw(FALSE);
		m_wndUserCableListCtrl.RemakeListCtrl(_T("User Cable Item") , &CableItemList);
		m_wndUserCableListCtrl.SetRedraw(TRUE);
	}
		
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp) pApp->OpenNewDocumentIfNeed(_T("USER CABLE") , _T("USER CABLE"));

	m_wndUserCableListCtrl.ShowWindow(SW_NORMAL);
	for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
	{
		if(itr->first != &m_wndUserCableListCtrl) itr->first->ShowWindow(SW_HIDE);
	}

	return ERROR_SUCCESS;
}

#include "TransformerSizingResultDoc.h"

/**
	@brief	callback function "SHOW TRANSFORMER SIZING RULE"

	@author BHK	

	@date 2009-03-27 오후 6:51:01	

	@param	

	@return		
**/
int CELoadItemViewBar::ShowTransformerSizingResultCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	BeginWaitCursor();

	//! save current transformer datas
	CTransformerSizingResultDoc::SaveTransformerSizingResultData();

	const int res = OpenDocumentDependOn(_T("Transformer Sizing") , _T("TRANSFORMER SIZING") , _T("TRANSFORMER SIZING"));
	if(ERROR_SUCCESS == res)
	{
		m_wndEmpty.ShowWindow(SW_SHOW);
		for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
		{
			if(itr->first != &m_wndEmpty) itr->first->ShowWindow(SW_HIDE);
		}
	}
	
	EndWaitCursor();

	return ERROR_SUCCESS;
}


/**
	@brief	

	@author BHK	

	@date 2009-05-07 오전 11:45:08	

	@param	

	@return		
*/
int CELoadItemViewBar::ShowRevTransformerSizingResultCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp) pApp->OpenNewDocumentIfNeed(_T("TRANSFORMER SIZING") , _T("REV TRANSFORMER SIZING|") + CString(pParam));

	m_wndEmpty.ShowWindow(SW_SHOW);
	for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
	{
		if(itr->first != &m_wndEmpty) itr->first->ShowWindow(SW_HIDE);
	}
	
	return ERROR_SUCCESS;
}

/**
	@brief	callback function "SHOW CABLE SIZING RULE"

	@author	BHK
*/
int CELoadItemViewBar::ShowCableSizingResultCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp)
	{
		//! 모든 POWER CABLE을 표시하도록 한다.
		CCableSizingResultDoc::m_nLoadPowerCableCreation = CCableSizingResultDoc::ALL_POWER_CABLE;
		pApp->OpenNewDocumentIfNeed(_T("Auto Power Cable") , _T("Show Auto Power Cable"));
	}

	m_wndEmpty.ShowWindow(SW_SHOW);
	for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
	{
		if(itr->first != &m_wndEmpty) itr->first->ShowWindow(SW_HIDE);
	}
	
	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-07 오후 2:09:55	

	@param	

	@return		
*/
int CELoadItemViewBar::ShowRevCableSizingResultCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp) pApp->OpenNewDocumentIfNeed(_T("Auto Power Cable") , _T("REV CABLE SIZING RESULT|") + CString(pParam));

	m_wndEmpty.ShowWindow(SW_SHOW);
	for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
	{
		if(itr->first != &m_wndEmpty) itr->first->ShowWindow(SW_HIDE);
	}
	
	return ERROR_SUCCESS;
}

/**
	@brief	REVISION CONTROL CABLE RESULT

	@author KHS	

	@date 2009-08-20 오전 10:07:35	

	@param	

	@return		
**/
int CELoadItemViewBar::ShowRevControlCableResultCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp) pApp->OpenNewDocumentIfNeed(_T("CONTROL CABLE") , _T("REV CONTROL CABLE RESULT|") + CString(pParam));

	m_wndEmpty.ShowWindow(SW_SHOW);
	for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
	{
		if(itr->first != &m_wndEmpty) itr->first->ShowWindow(SW_HIDE);
	}
	
	return ERROR_SUCCESS;
}
/**
	@brief	LoadListCtrl를 새롭게 생성한다.

	@author	BHK
*/
int CELoadItemViewBar::RemakeLoadListCtrl(const string& rBusID , const string& rArea , vector<CLoadItem*>& LoadItemEntry)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	const int nColumnWidth = m_wndLoadListCtrl.GetColumnWidth(0);
	
	m_wndLoadListCtrl.DeleteAllItems();
	///m_wndLoadListCtrl.DeleteColumn(1);
	m_wndLoadListCtrl.DeleteColumn(0);

	CString rText = _T("Load Item");
	m_wndLoadListCtrl.InsertColumn(0, rText, LVCFMT_LEFT, nColumnWidth);
	///m_wndLoadListCtrl.InsertColumn(1, _T("All Count : "), LVCFMT_LEFT, 100);
	m_wndLoadListCtrl.SetBusID(rBusID);
	m_wndLoadListCtrl.SetArea(rArea);

	int nItem = 0;
	for(vector<CLoadItem*>::iterator itr = LoadItemEntry.begin();itr != LoadItemEntry.end();++itr)
	{
		//! 삭제한 아이템은 보여주지 않는다.
		if((*itr)->IsDeleted()) continue;

		const string rName         = (*itr)->GetName();
		
		int nImage = docData.GetIconNo(*itr);
		int item = m_wndLoadListCtrl.InsertItem(nItem++ , rName.c_str() , nImage);
		m_wndLoadListCtrl.SetItemData( item , (DWORD_PTR)(*itr) );
	}

	m_wndLoadListCtrl.Sort(0 , m_wndLoadListCtrl.GetHeaderCtrl().IsAscending());
	DisplayItemCount();

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-06-16 오전 10:59:33	

	@param	

	@return		
*/
int CELoadItemViewBar::UpdateShowLoadBusItem()
{
	BeginWaitCursor();
	m_wndItemClassTreeCtrl.SetRedraw(FALSE);
	m_wndItemClassTreeCtrl.UpdateShowLoadBusItem();
	m_wndItemClassTreeCtrl.SetRedraw(TRUE);
	EndWaitCursor();

	return ERROR_SUCCESS;
}

/**
	@brief	BusGroup에서 INCOMMING,SubBUS는 없을시 무조건 생성하도록 한다.

	@author BHK	

	@date 2009-04-04 오전 1:18:29	

	@param	

	@return		
**/
int CELoadItemViewBar::OnCreateBusTreeCtrl(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> BusGroupList;
	docData.GetELoadItemListOf(BusGroupList , CBusGroupItem::TypeString());
        {
		//! 꼭 필요한 BUS GROUP들을 생성한다.
		CreateSystemBusGroup();
		
		for(list<CELoadItem*>::iterator itr = BusGroupList.begin(); itr != BusGroupList.end();++itr)
                {
			CBusGroupItem* pBusGroup = static_cast<CBusGroupItem*>(*itr);
			if(pBusGroup->IsDeleted()) continue;
			const string rBusGroupID = pBusGroup->GetName();
			
			list<string>::iterator where = find(docData.m_lstSystemBusGroup.begin() , docData.m_lstSystemBusGroup.end() , rBusGroupID);
			if((NOT_ASSIGNED != rBusGroupID) && (where == docData.m_lstSystemBusGroup.end())) AddBusGroupTreeCtrlItem(pBusGroup);

			for(vector<CBusItem*>::iterator jtr = pBusGroup->m_BusRefEntry.begin();jtr!= pBusGroup->m_BusRefEntry.end();++jtr)
			{
				if((*jtr)->IsDeleted()) continue;

				(*jtr)->SetBusGroupItemPtr(pBusGroup);
				AddBusTreeCtrlItem(*jtr);
			}
		}

		//! BUSGROUP에 한하여 TREE에 나타나는 순서대로 Order No를 다시 매긴다.
		int nOrderNo = 1;
		HTREEITEM hItem = m_wndBusTreeView.GetRootItem();
		while(hItem)
		{
			const CString rItemText = m_wndBusTreeView.GetItemText(hItem);
			
			CBusGroupItem* pBusGroupItem = docData.FindBusGroupItemByName(rItemText.operator LPCSTR());
			if(pBusGroupItem)
			{
				ostringstream oss;
				oss << nOrderNo;
				pBusGroupItem->prop()->SetOriginalValue(_T("General") , _T("Order No") , oss.str());
				pBusGroupItem->prop()->SetValue(_T("General") , _T("Order No") , oss.str());
			}
			hItem = m_wndBusTreeView.GetNextSiblingItem(hItem);
			++nOrderNo;
		}
		docData.SortBusGroupByOrderNo();
        }
        
	return ERROR_SUCCESS;
}
/**
	@brief	BusTreeCtrl를 새롭게 생성한다.

	@author	BHK
*/
int CELoadItemViewBar::RemakeBusTreeCtrl(void/*vector<CBusGroupItem*>& BusGroupItemEntry*/)
{
	m_wndBusTreeView.DeleteAllItems();

	OnCreateBusTreeCtrl();

        return ERROR_SUCCESS;
}

struct MatchItemName : binary_function<CELoadItem*,string,bool>
{
	bool operator()(CELoadItem* lhs , const string& rhs) const
	{
		return (rhs == lhs->GetName());
	}
};

/**
	@brief	

	@author	BHK
*/
void CELoadItemViewBar::OnCreateTransformer()
{
	m_wndTransformerListCtrl.CreateItem(CTransformerItem::TypeString());
}

void CELoadItemViewBar::OnCopyTransformer()
{
	if(m_wndTransformerListCtrl.IsWindowVisible())
        {
                m_wndTransformerListCtrl.OnItemCopy();
        }
}

void CELoadItemViewBar::OnDeleteTransformer()
{
	if(m_wndTransformerListCtrl.IsWindowVisible())
        {
		m_wndTransformerListCtrl.OnItemDelete();
	}
}

/**
	@brief	

	@author	BHK
*/
void CELoadItemViewBar::OnCreateCable()
{
	if(m_wndPowerCableListCtrl.IsWindowVisible())
		m_wndPowerCableListCtrl.CreateItem(CCableItem::TypeString());
	else if(m_wndControlCableListCtrl.IsWindowVisible())
		m_wndControlCableListCtrl.CreateItem(CCableItem::TypeString());
}

/**
	@brief	

	@author BHK	

	@date 2009-07-02 오후 2:22:09	

	@param	

	@return		
*/
void CELoadItemViewBar::OnCopyCable()
{
	if(m_wndPowerCableListCtrl.IsWindowVisible())
	{
		m_wndPowerCableListCtrl.OnItemCopy();
	}
	else if(m_wndControlCableListCtrl.IsWindowVisible())
	{
		m_wndControlCableListCtrl.OnItemCopy();
	}
	else if(m_wndUserCableListCtrl.IsWindowVisible())
	{
		m_wndUserCableListCtrl.OnItemCopy();
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-07-02 오후 1:14:19	

	@param	

	@return		
*/
void CELoadItemViewBar::OnDeleteCable()
{
	BeginWaitCursor();
	if(m_wndPowerCableListCtrl.IsWindowVisible())
	{
		m_wndPowerCableListCtrl.OnItemDelete();
	}
	else if(m_wndControlCableListCtrl.IsWindowVisible())
	{
		m_wndControlCableListCtrl.OnItemDelete();
	}
	else if(m_wndUserCableListCtrl.IsWindowVisible())
	{
		m_wndUserCableListCtrl.OnItemDelete();
	}
	EndWaitCursor();
}

/**
	@brief	List에 있는 모든 Cable Item을 삭제합니다.

	@author BHK	

	@date 2009-07-02 오후 1:14:27	

	@param	

	@return		
*/
void CELoadItemViewBar::OnDeleteAllCable()
{
	BeginWaitCursor();
	if(m_wndPowerCableListCtrl.IsWindowVisible())
	{
		m_wndPowerCableListCtrl.OnItemDeleteAll();
		
		CString rString;
		rString.LoadString(IDS_STRING_CABLE_SIZING_RESULT_TYPE);
		CELOADApp* pApp = (CELOADApp*)AfxGetApp();
		if(pApp) pApp->CloseDocument(rString);
	}
	else if(m_wndControlCableListCtrl.IsWindowVisible())
	{
		m_wndControlCableListCtrl.OnItemDeleteAll();

		CString rString;
		rString.LoadString(IDS_STRING_CONTROL_CABLE_RESULT_TYPE);
		CELOADApp* pApp = (CELOADApp*)AfxGetApp();
		if(pApp) pApp->CloseDocument(rString);
	}
	else if(m_wndUserCableListCtrl.IsWindowVisible())
	{
		m_wndUserCableListCtrl.OnItemDeleteAll();

		CString rString;
		rString.LoadString(IDS_STRING_USER_CABLE_RESULT_TYPE);
		CELOADApp* pApp = (CELOADApp*)AfxGetApp();
		if(pApp) pApp->CloseDocument(rString);
	}
	EndWaitCursor();
}

/**
	@brief  POWER CABLE CREATION DIALOG BOX를 띄운다.
	@author KHS
*/
void CELoadItemViewBar::OnCablesizingresultShowcablesizingresult()
{
	// TODO: Add your command handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if(pFrame)
	{
		pFrame->SendMessage(WM_COMMAND , ID_CABLE_POWERCABLECREATION , 0);
	}
}

/**
	@brief	just call DeleteCableSizingResultTableFile 

	@author BHK	

	@date 2009-04-10 오후 3:18:01	

	@param	

	@return		
*/
void CELoadItemViewBar::OnDeleteCableSizingTableFile()
{
	HTREEITEM hSelItem = m_wndItemClassTreeCtrl.GetSelectedItem();
	if(hSelItem) m_wndItemClassTreeCtrl.DeleteCableSizingTableFile(hSelItem);
}

/**
	@brief	현재 생성된 PRODUCT들을 database에 저장하기 위해 다이얼로그를 호출합니다.

	@author humkyung	

	@date 2009-04-27 오전 11:49:53	

	@param	

	@return		
*/
void CELoadItemViewBar::OnSaveProductToRevision()
{
	if(m_rCurrentItemText.Find(_T("(")) != -1)
	{
		const int at = m_rCurrentItemText.ReverseFind('(');
		m_rCurrentItemText  = m_rCurrentItemText.Left(at - 1);
	}
	CString sCurrentItemText = m_rCurrentItemText.MakeUpper();

	CSaveProductToRevisionDlg dlg(sCurrentItemText);
	if(IDOK == dlg.DoModal())
	{
		m_wndItemClassTreeCtrl.RemakeRevDataTreeNodeOf(sCurrentItemText.operator LPCSTR());
		//! construct other bus or pdb revision tree when save bus or pdb revision.
		if(_T("LOAD SUMMARY-BUS" == sCurrentItemText))
		{
			m_wndItemClassTreeCtrl.RemakeRevDataTreeNodeOf(_T("Load Summary-SubBUS"));
		}
		else if(_T("LOAD SUMMARY-SUBBUS" == sCurrentItemText))
		{
			m_wndItemClassTreeCtrl.RemakeRevDataTreeNodeOf(_T("Load Summary-BUS"));
		}
	}
}

/**
	@brief	open compare revision data document.

	@author BHK	

	@date 2009-04-27 오후 11:28:48

	@param	

	@return	
**/
void CELoadItemViewBar::OnViewRevision()
{
	if(m_rCurrentItemText.Find("(") != -1)
	{
		const int at = m_rCurrentItemText.ReverseFind('(');
		m_rCurrentItemText  = m_rCurrentItemText.Left(at - 1);
	}
	CCompareRevisionDataDlg dlg(m_rCurrentItemText);
	if(IDOK == dlg.DoModal())
	{
		CCompareRevisionDataDoc::COMPARE_ITEM CompareItem;
		CompareItem.rProductName    = dlg.m_rProductName;
		CompareItem.nLeftSideRevNo  = dlg.m_nLeftSideRevNo;
		CompareItem.nRightSideRevNo = dlg.m_nRightSideRevNo;

		CELOADApp* pApp = (CELOADApp*)AfxGetApp();
		BOOL res = pApp->OpenNewDocumentIfNeed(_T("REVISION") , LPCTSTR(&CompareItem));
	}
}

/**
	@brief	현재 활성화된 LoadListCtrl을 갱신한다.

	@author BHK	

	@date 2009-04-07 오후 4:58:16	

	@param	

	@return		
*/
int CELoadItemViewBar::UpdateLoadListCtrl(void)
{
	if(m_wndLoadListCtrl.IsWindowVisible())
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		const string rInterestingBusID = m_wndLoadListCtrl.GetBusID();
		const string rInterestingArea  = m_wndLoadListCtrl.GetArea();
		list<CELoadItem*> LoadItemList;
		docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
		///if(pItemList)
		{
			vector<CLoadItem*> LoadItemEntry;
			for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
			{
				CLoadItem* pLoadItem = static_cast<CLoadItem*>(*itr);
				const string rBusID = pLoadItem->GetBusId();
				const string rArea  = pLoadItem->prop()->GetValue(_T("Location") , _T("Area"));
				if((!rInterestingBusID.empty() && (rInterestingBusID == rBusID)) || (!rInterestingArea.empty() && (rInterestingArea == rArea)))
					LoadItemEntry.push_back(pLoadItem);
			}

			RemakeLoadListCtrl(rInterestingBusID , rInterestingArea , LoadItemEntry);
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-07-03 오전 11:18:12	

	@param	

	@return		
*/
int CELoadItemViewBar::UpdateCableListCtrl(const string& rCableType)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	
	string rHeaderText;
	CMFCCableListCtrlEx* pListCtrl = NULL;
	if(_T("Auto Power Cable") == rCableType)
	{
		pListCtrl = &m_wndPowerCableListCtrl;
		rHeaderText = _T("Power Cable Item");
	}
	else if(_T("Auto Control Cable") == rCableType)
	{
		pListCtrl = &m_wndControlCableListCtrl;
		rHeaderText = _T("Control Cable Item");
	}
	else if(_T("User Cable") == rCableType)
	{
		pListCtrl = &m_wndUserCableListCtrl;
		rHeaderText = _T("User Cable Item");
	}

	if(pListCtrl)
	{
		pListCtrl->SetRedraw(FALSE);
		list<CELoadItem*> CableList;
		docData.GetELoadItemListOf(CableList , CCableItem::TypeString() , _T("*"));
		const int nCableCount = int(count_if(CableList.begin() , CableList.end() , CELoadItem::IsNotDeleted));

		const int nItemCount = pListCtrl->GetItemCount();
		if((0 == nItemCount) || (nItemCount != nCableCount))
		{
			list<CELoadItem*> CableItemList;
			for(list<CELoadItem*>::iterator itr = CableList.begin();itr != CableList.end();++itr)
			{
				CString rCableCategory = (*itr)->prop()->GetValue(_T("General") , _T("Cable Category")).c_str();
				CString rCreateBy = (*itr)->prop()->GetValue(_T("General") , _T("Created By")).c_str();
				if(_T("Auto Power Cable") == rCableType)
				{
					if(_T("POWER CABLE") == rCableCategory.MakeUpper() && _T("AUTO") == rCreateBy.MakeUpper()) CableItemList.push_back(*itr);
				}
				else if(_T("Auto Control Cable") == rCableType)
				{
					if(_T("CONTROL CABLE") == rCableCategory.MakeUpper() && _T("AUTO") == rCreateBy.MakeUpper()) CableItemList.push_back(*itr);
				}
				else if(_T("User Cable") == rCableType)
				{
					if(_T("USER") == rCreateBy.MakeUpper()) CableItemList.push_back(*itr);
				}
			}
			pListCtrl->RemakeListCtrl(rHeaderText , &CableItemList);
		}
		pListCtrl->SetRedraw(TRUE);

		pListCtrl->ShowWindow(SW_NORMAL);
		for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
		{
			if(itr->first != pListCtrl) itr->first->ShowWindow(SW_HIDE);
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	Item Tree Node를 클릭 했을때...

	@author HumKyung	

	@date 2009-06-03 오후 4:21:50	

	@param	

	@return		
*/
int CELoadItemViewBar::ShowItemCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	assert(pCommandStr && "pCommandStr is NULL");
	assert(pParam && "pParam is NULL");
	if(pCommandStr && pParam)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();

		CMFCListCtrlEx* pInterestingListCtrl = (CMFCListCtrlEx*)FindWndByCommandStr(pCommandStr);
		if(pInterestingListCtrl)
		{
			pInterestingListCtrl->SetRedraw(FALSE);
			list<CELoadItem*> ELoadItemList;
			docData.GetELoadItemListOf(ELoadItemList , pTypeStr);
			const int nELoadItemCount = int(count_if(ELoadItemList.begin() , ELoadItemList.end() , CELoadItem::IsNotDeleted));

			const int nItemCount = pInterestingListCtrl->GetItemCount();
			if((0 == nItemCount) || (nItemCount != nELoadItemCount))
			{
				pInterestingListCtrl->RemakeListCtrl(pHeaderText , &ELoadItemList);
			}
			pInterestingListCtrl->SetRedraw(TRUE);

			pInterestingListCtrl->ShowWindow(SW_NORMAL);
			for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
			{
				if(itr->first != pInterestingListCtrl) itr->first->ShowWindow(SW_HIDE);
			}
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	NEW CAPACITOR

	@author KHS	

	@date 2009-05-07 오후 8:34:21	

	@param	

	@return		
**/
void CELoadItemViewBar::OnNewcapacitor()
{
	m_wndCapacitorListCtrl.CreateItem(CCapacitorItem::TypeString());
}

/**
	@brief	CAPACITOR COPY

	@author KHS	

	@date 2009-05-07 오후 8:34:23	

	@param	

	@return		
**/
void CELoadItemViewBar::OnCapacitorCopy()
{
	if(m_wndCapacitorListCtrl.IsWindowVisible())
	{
		m_wndCapacitorListCtrl.OnItemCopy();
	}
}

/**
	@brief	CAPACITOR DELETE

	@author KHS	

	@date 2009-05-07 오후 8:34:27	

	@param	

	@return		
**/
void CELoadItemViewBar::OnCapacitorDelete()
{
	if(m_wndCapacitorListCtrl.IsWindowVisible())
	{
		m_wndCapacitorListCtrl.OnItemDelete();
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-05-08 오전 9:31:07	

	@param	

	@return		
*/
void CELoadItemViewBar::OnNewGenerator()
{
	if(m_wndGeneratorListCtrl.IsWindowVisible()) m_wndGeneratorListCtrl.CreateItem(CGeneratorItem::TypeString());
}

/**
	@brief	

	@author BHK	

	@date 2009-05-08 오전 9:31:56	

	@param	

	@return		
*/
void CELoadItemViewBar::OnGeneratorCopy()
{
	if(m_wndGeneratorListCtrl.IsWindowVisible()) m_wndGeneratorListCtrl.OnItemCopy();
}

/**
	@brief	

	@author BHK	

	@date 2009-05-08 오전 9:31:52	

	@param	

	@return		
*/
void CELoadItemViewBar::OnGeneratorDelete()
{
	if(m_wndGeneratorListCtrl.IsWindowVisible()) m_wndGeneratorListCtrl.OnItemDelete();
}

/**
	@brief	

	@author BHK	

	@date 2009-06-03 오후 6:46:03	

	@param	

	@return		
*/
void CELoadItemViewBar::OnNewUPSDC()
{
	if(m_wndUPSDCListCtrl.IsWindowVisible()) m_wndUPSDCListCtrl.CreateItem(CUPSDCItem::TypeString());
}

/**
	@brief	

	@author BHK	

	@date 2009-06-03 오후 6:46:15	

	@param	

	@return		
*/
void CELoadItemViewBar::OnUPSDCCopy()
{
	if(m_wndUPSDCListCtrl.IsWindowVisible()) m_wndUPSDCListCtrl.OnItemCopy();
}

/**
	@brief	

	@author BHK	

	@date 2009-06-03 오후 6:46:25	

	@param	

	@return		
*/
void CELoadItemViewBar::OnUPSDCDelete()
{
	if(m_wndUPSDCListCtrl.IsWindowVisible()) m_wndUPSDCListCtrl.OnItemDelete();
}
/**
	@brief	

	@author BHK	

	@date 2009-06-03 오후 4:41:05	

	@param	

	@return		
*/
CWnd* CELoadItemViewBar::FindWndByCommandStr(const string& rCommandStr)
{
	CWnd* pInterestingWnd = NULL;
	for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
	{
		vector<string>::iterator where = find(itr->second.begin() , itr->second.end() , rCommandStr);
		if(where != itr->second.end()) return (itr->first);
	}

	return NULL;
}

/**
	@brief	Cable용 List Control을 생성한다.

	@author BHK	

	@date 2009-06-19 오후 5:15:45	

	@param	

	@return		
*/
int CELoadItemViewBar::CreateCableListCtrl(const DWORD& dwStyle)
{
	CRect rectDummy;
	rectDummy.SetRectEmpty();

	m_wndPowerCableListCtrl.SetColumnTitleString(_T("Power Cable Item"));
	if(!m_wndPowerCableListCtrl.Create (dwStyle, rectDummy, &m_wndSplitter, CELoadItemViewBar::POWER_CABLE_ID))
	{
		TRACE0("Failed to create transformer list ctrl\n");
		return -1;      // fail to create
	}
	m_wndPowerCableListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
	m_wndPowerCableListCtrl.SetSortColumn(1);
	m_ActionWndMap[&m_wndPowerCableListCtrl].push_back(_T("SHOW POWER CABLE"));
	m_wndPowerCableListCtrl.ShowWindow(SW_HIDE);

	m_wndControlCableListCtrl.SetColumnTitleString(_T("Control Cable Item"));
	if(!m_wndControlCableListCtrl.Create (dwStyle, rectDummy, &m_wndSplitter, CELoadItemViewBar::CONTROL_CABLE_ID))
	{
		TRACE0("Failed to create control cable list ctrl\n");
		return -1;      // fail to create
	}
	m_wndControlCableListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
	m_wndControlCableListCtrl.SetSortColumn(1);
	m_ActionWndMap[&m_wndControlCableListCtrl].push_back(_T("SHOW CONTROL CABLE"));
	m_wndControlCableListCtrl.ShowWindow(SW_HIDE);

	m_wndUserCableListCtrl.SetColumnTitleString(_T("User Cable Item"));
	if(!m_wndUserCableListCtrl.Create (dwStyle, rectDummy, &m_wndSplitter, CELoadItemViewBar::USER_CABLE_ID))
	{
		TRACE0("Failed to create user cable list ctrl\n");
		return -1;      // fail to create
	}
	m_wndUserCableListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
	m_wndUserCableListCtrl.SetSortColumn(1);
	m_ActionWndMap[&m_wndUserCableListCtrl].push_back(_T("SHOW USER CABLE"));
	m_wndUserCableListCtrl.ShowWindow(SW_HIDE);

	return ERROR_SUCCESS;
}

/**
	@brief	USER CABLE IMPORT , import후 생성된 user cable을 리스트에 표기한다.

	@author KHS	

	@date 2009-06-19 오후 5:16:58	

	@param	

	@return		
**/
void CELoadItemViewBar::OnUserCableImport()
{
	CUserCableImportDlg dlg;
	if(IDOK == dlg.DoModal())
	{
		UpdateData(TRUE);
		
		CELoadDocData& docData = CELoadDocData::GetInstance();

		UserCableExcelImportExecute(dlg.m_rUserCableExcelPath, dlg.m_nOption);

		CString rMsg;
		const string rResultString = GetUserCableImportResultString();
		if(docData.m_nErrorCount > 0)
			rMsg.Format("%s%d error(s)" , rResultString.c_str() , docData.m_nErrorCount);
		else	rMsg.Format("%s" , rResultString.c_str());
		
		CDataResultDlg dlg(rMsg);
		dlg.DoModal();

		m_wndItemClassTreeCtrl.DisplayCableItemCount();

		//! user cable을 표기한다.
		UpdateCableListCtrl(_T("User Cable"));

		docData.SetModifiedFlag(true);

		//! CONTROL CABLE RESULT VIEW
		CELOADApp* pApp = (CELOADApp*)AfxGetApp();
		if(pApp) 
		{
			pApp->OpenNewDocumentIfNeed(_T("USER CABLE") , _T("USER CABLE"));

			AppenderPtr appender = mylogger->getAppender("ELoadAppender");
			FileAppenderPtr fileappender = (FileAppenderPtr)appender;
			const string rLogFilePath = fileappender->getFile();
			pApp->PutLogFileToDatabase(rLogFilePath, "User Cable");

			//! LOG 출력
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			if(pFrame) pFrame->m_wndOutput.Navigate2(rLogFilePath.c_str());
		}
	}
}

struct MatchLoadVolt : public binary_function<CCableCreationTable::Record, string, bool>
{
        bool operator()(const CCableCreationTable::Record lhs , const string& rhs) const
	{
		const map<string, string>::const_iterator where = lhs.FieldValueMap.find(_T("C_LOAD_VOLTAGE"));
		if(where != lhs.FieldValueMap.end())
		{
			return (rhs == where->second);
		}

		return false;
	}
};

struct MatchCableVolt : public binary_function<CCableCreationTable::Record, string, bool>
{
        bool operator()(const CCableCreationTable::Record lhs , const string& rhs) const
	{
		const map<string, string>::const_iterator where = lhs.FieldValueMap.find(_T("C_CABLE_VOLTAGE"));
		if(where != lhs.FieldValueMap.end())
		{
			return (rhs == where->second);
		}

		return false;
	}
};

bool MatchControlCableSetting(CELOADTable::Record lhs , CELOADTable::Record rhs)
{
		const string rSpec1  = lhs.FieldValueMap[_T("C_SPEC")];
		const string rCore1  = lhs.FieldValueMap[_T("C_CORE")];
		const string rSize1  = lhs.FieldValueMap[_T("C_SIZE")];

		const string rSpec2  = rhs.FieldValueMap[_T("C_SPEC")];
		const string rCore2  = rhs.FieldValueMap[_T("C_CORE")];
		const string rSize2  = rhs.FieldValueMap[_T("C_SIZE")];
		
		return ((rSpec1 == rSpec2) && (rCore1 == rCore2) && (rSize1 == rSize2));
}

/**
	@brief	USER CABLE EXCEL IMPORT

	@author KHS	

	@date 2009-06-19 오후 5:27:52	

	@param	

	@return		
**/
int CELoadItemViewBar::UserCableExcelImport(const CString& rExcelFilePath, const int& nOption)
{
	CELoadDocData &docData = CELoadDocData::GetInstance();
	const string rMDBFilePath = docData.GetProjectMDBFilePath();

	CHVCableCreationSettingTable& HVVoltageSettingTable = CHVCableCreationSettingTable::GetInstance();
	HVVoltageSettingTable.Load(rMDBFilePath);

	CLV3PHCableCreationSettingTable& LV3PHVoltageSettingTable = CLV3PHCableCreationSettingTable::GetInstance();
	LV3PHVoltageSettingTable.Load(rMDBFilePath);

	CLV1PHCableCreationSettingTable& LV1PHVoltageSettingTable = CLV1PHCableCreationSettingTable::GetInstance();
	LV1PHVoltageSettingTable.Load(rMDBFilePath);

	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
	CString rTitle = _T("Import User Cable Data...");
	
	//! LOG
	ELOAD_LOG4CXX_RESET(mylogger, _T("User Cable Error Messages"));

	//!Option 0 일때 모든 User Cable을 삭제
	if(0 == nOption) DeleteAllUserCable();

	CExcelAutomation xlApp;

	try
	{
		//! 엑셀 파일은 오픈한다.
		if(!xlApp.Open(rExcelFilePath))
		{
			AfxMessageBox("Can't Start Excel and get Application object.");
			return IDCANCEL;
		}
		xlApp.SetActiveSheet( 1 ); // LOAD Sheet

		const int nRowStart = 6;
		const int nColStart = 8;

		///  Get Excel Row Count, Column Count
		int nRowCount = CExcelImportDlg::GetRowCount(xlApp, nRowStart, 7);

		COleVariant VOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
		COleVariant VFalse((short)FALSE);

		CString rResult;
		int nRow = nRowStart;

		int nIndex = 0;

		const int __INDEX__[] = {2,3,7,8,9,10,11,12,13,14,15,16,1,4,5,6};
		const int nColCount = sizeof(__INDEX__)/ sizeof(__INDEX__[0]);
		
	
		CELOADTable* pTable = docData.GetTableOf(_T("T_CONTROL_CABLE_SETTING"));

		m_nTryCount = 0;m_nSuccessCount = 0;m_nFailCount = 0;

		const int nTotalCount = nRowCount;

		for(int nRow = nRowStart; nRow < nRowStart + nRowCount;nRow++)
		{

			CCableItem* pCableItem = static_cast<CCableItem*>(docData.CreateELoadItemOf(CCableItem::TypeString()));
			if(pCableItem)
			{
				++m_nTryCount;
				pCableItem->prop()->SetValue(_T("General"), _T("Created By"), "User");
				pCableItem->prop()->SetOriginalValue(_T("General"), _T("Created By"), "User");
				
				CBusItem* pBusItem = NULL;
				CLoadItem* pLoadItem = NULL;
				CString rSpec, rCore, rSize, rOd, rWeight;
				for(int nCol = 0; nCol < nColCount;nCol++)
				{
					char cCol = 'A';
					cCol = char(int(cCol) + (__INDEX__[nCol] - 1));
					rResult.Format(_T("%c%d") , cCol, nRow);
					COleVariant CellNum = COleVariant(rResult);
					CString rValue;
					xlApp.GetCellValue(CellNum, rValue);
					if(_T("-") == rValue.Trim()) rValue = _T("");

					if(1 == __INDEX__[nCol])
					{
						pCableItem->prop()->SetValue(_T("General"), _T("Area"), rValue.operator LPCSTR());
						pCableItem->prop()->SetOriginalValue(_T("General"), _T("Area"), rValue.operator LPCSTR());
					}
					else if(2 == __INDEX__[nCol])
					{
						//! BUS ID
						pCableItem->prop()->SetValue(_T("General"), _T("Panel ID"), rValue.operator LPCSTR());
						pCableItem->prop()->SetOriginalValue(_T("General"), _T("Panel ID"), rValue.operator LPCSTR());
						{
							//! BUS가 존재 하지 않으면 생성
							pBusItem = docData.FindBusItemByName(rValue.operator LPCSTR());
							if(NULL == pBusItem)
							{
								pBusItem = static_cast<CBusItem*>(docData.CreateELoadItemOf(CBusItem::TypeString()));
								pBusItem->prop()->SetValue(_T("General"), _T("Bus ID"), rValue.operator LPCSTR());
								pBusItem->prop()->SetOriginalValue(_T("General"), _T("Bus ID"), rValue.operator LPCSTR());

								pBusItem->prop()->SetValue(_T("General"), _T("Created By"), _T("User"));
								pBusItem->prop()->SetOriginalValue(_T("General"), _T("Created By"), _T("User"));
							}
						}
					}
					else if(3 == __INDEX__[nCol])
					{
						//! LOAD ID
						pCableItem->prop()->SetValue(_T("General"), _T("Load Id"), rValue.operator LPCSTR());
						pCableItem->prop()->SetOriginalValue(_T("General"), _T("Load Id"), rValue.operator LPCSTR());

						if(docData.IsExistItemName(CLoadItem::TypeString(), rValue.operator LPCSTR()))
						{
							pLoadItem = docData.FindLoadItemByName(rValue.operator LPCSTR());
						}
						else
						{
							//! LOAD 가 존재 하지 않으므로 생성
							pLoadItem = static_cast<CLoadItem*>(docData.CreateELoadItemOf(CLoadItem::TypeString()));
							if(pLoadItem)
							{
								pLoadItem->prop()->SetValue(_T("General"), _T("ITEM ID"), rValue.operator LPCSTR());
								pLoadItem->prop()->SetOriginalValue(_T("General"), _T("ITEM ID"), rValue.operator LPCSTR());

								pLoadItem->prop()->SetValue(_T("General"), _T("Created By"), _T("User"));
								pLoadItem->prop()->SetOriginalValue(_T("General"), _T("Created By"), _T("User"));

								if(pBusItem)
								{
									const string rBusId = pBusItem->GetName();
									pLoadItem->prop()->SetValue(_T("Panel"), _T("Bus ID"), rBusId);
									pLoadItem->prop()->SetOriginalValue(_T("Panel"), _T("Bus ID"), rBusId);
								}
							}
						}
						
						if(pLoadItem)
						{
							pLoadItem->AddCable(pCableItem);
							pCableItem->SetLoadItemPtr(pLoadItem);
						}
					}
					else if(4 == __INDEX__[nCol])
					{
						pCableItem->prop()->SetValue(_T("General"), _T("From"), rValue.operator LPCSTR());
						pCableItem->prop()->SetOriginalValue(_T("General"), _T("From"), rValue.operator LPCSTR());
					}
					else if(5 == __INDEX__[nCol])
					{
						pCableItem->prop()->SetValue(_T("General"), _T("To"), rValue.operator LPCSTR());
						pCableItem->prop()->SetOriginalValue(_T("General"), _T("To"), rValue.operator LPCSTR());
					}
					else if(6 == __INDEX__[nCol])
					{
						pCableItem->prop()->SetValue(_T("General"), _T("Description"), rValue.operator LPCSTR());
						pCableItem->prop()->SetOriginalValue(_T("General"), _T("Description"), rValue.operator LPCSTR());
					}
					else if(7 == __INDEX__[nCol])
					{
						//!Cable ID
						if(1 == nOption && docData.IsExistItemName(CCableItem::TypeString(), rValue.operator LPCSTR()))
						{
							const string rLoadId  = pLoadItem->GetName();
							pCableItem->SetDeleteFlag(true);
							ELOAD_LOG4CXX_ERROR(mylogger , rLoadId , 27);
							++m_nFailCount;
							break;
						}

						++m_nSuccessCount;

						pCableItem->prop()->SetValue(_T("General"), _T("CABLE ID"), rValue.operator LPCSTR());
						pCableItem->prop()->SetOriginalValue(_T("General"), _T("CABLE ID"), rValue.operator LPCSTR());
					}
					else if(8 == __INDEX__[nCol])
					{
						if("P" == rValue)
						{
							pCableItem->prop()->SetValue(_T("General"), _T("Cable Category"), "Power Cable");
							pCableItem->prop()->SetOriginalValue(_T("General"), _T("Cable Category"), "Power Cable");
						}
						else if("C" == rValue)
						{
							pCableItem->prop()->SetValue(_T("General"), _T("Cable Category"), "Control Cable");
							pCableItem->prop()->SetOriginalValue(_T("General"), _T("Cable Category"), "Control Cable");
						}
						else
						{
							const string rLoadId  = pLoadItem->GetName();
							pCableItem->SetDeleteFlag(true);
							ELOAD_LOG4CXX_ERROR(mylogger , rLoadId , 41);
							++m_nFailCount;
							break;
						}
					}
					else if(9 == __INDEX__[nCol])
					{
						pCableItem->prop()->SetValue(_T("General"), _T("Cable Volt"), rValue.operator LPCSTR());
						pCableItem->prop()->SetOriginalValue(_T("General"), _T("Cable Volt"), rValue.operator LPCSTR());
						
						string rVoltType;
						const string rCableVolt = pCableItem->prop()->GetValue(_T("General"), _T("Cable Volt"));
						{
							vector<CCableCreationTable::Record>::iterator where = find_if(HVVoltageSettingTable.m_RecordEntry.begin(), HVVoltageSettingTable.m_RecordEntry.end(),bind2nd(MatchCableVolt(), rCableVolt));
							if(where != HVVoltageSettingTable.m_RecordEntry.end())
							{
								rVoltType = where->FieldValueMap[_T("C_VOLTAGE_TYPE")];
							}
						}

						{
							vector<CCableCreationTable::Record>::iterator where = find_if(LV3PHVoltageSettingTable.m_RecordEntry.begin(), LV3PHVoltageSettingTable.m_RecordEntry.end(),bind2nd(MatchCableVolt(), rCableVolt));
							if(where != LV3PHVoltageSettingTable.m_RecordEntry.end())
							{
								rVoltType = where->FieldValueMap[_T("C_VOLTAGE_TYPE")];
							}
						}

						{
							vector<CCableCreationTable::Record>::iterator where = find_if(LV1PHVoltageSettingTable.m_RecordEntry.begin(), LV1PHVoltageSettingTable.m_RecordEntry.end(),bind2nd(MatchCableVolt(), rCableVolt));
							if(where != LV1PHVoltageSettingTable.m_RecordEntry.end())
							{
								rVoltType = where->FieldValueMap[_T("C_VOLTAGE_TYPE")];
							}
						}
						if(rVoltType.empty())
						{
							const string rLoaId = pLoadItem->GetName();
							ELOAD_LOG4CXX_ERROR(mylogger , rLoaId , 28);
						}
						
						pCableItem->prop()->SetValue(_T("General"), _T("Volt Type"), rVoltType);
						pCableItem->prop()->SetOriginalValue(_T("General"), _T("Volt Type"), rVoltType);
							
					}
					else if(10 == __INDEX__[nCol])
					{
						pCableItem->prop()->SetValue(_T("General"), _T("Cable Spec"), rValue.operator LPCSTR());
						pCableItem->prop()->SetOriginalValue(_T("General"), _T("Cable Spec"), rValue.operator LPCSTR());
						rSpec = rValue;
					}
					else if(11 == __INDEX__[nCol])
					{
						pCableItem->prop()->SetValue(_T("General"), _T("Core"), rValue.operator LPCSTR());
						pCableItem->prop()->SetOriginalValue(_T("General"), _T("Core"), rValue.operator LPCSTR());
						rCore = rValue;
					}
					else if(12 == __INDEX__[nCol])
					{
						if((isalpha(rValue[0])) && (!isdigit(rValue[0])))
						{
							const string rLoadId  = pLoadItem->GetName();
							pCableItem->SetDeleteFlag(true);
							ELOAD_LOG4CXX_ERROR(mylogger , rLoadId , 40);
							++m_nFailCount;
							break;
						}

						pCableItem->prop()->SetValue(_T("General"), _T("Size"), rValue.operator LPCSTR());
						pCableItem->prop()->SetOriginalValue(_T("General"), _T("Size"), rValue.operator LPCSTR());
						rSize = rValue;
					}
					else if(13 == __INDEX__[nCol])
					{
						if((isalpha(rValue[0])) && (!isdigit(rValue[0])))
						{
							const string rLoadId  = pLoadItem->GetName();
							pCableItem->SetDeleteFlag(true);
							ELOAD_LOG4CXX_ERROR(mylogger , rLoadId , 43);
							++m_nFailCount;
							break;
						}
					
						pCableItem->prop()->SetValue(_T("General"), _T("Length"), rValue.operator LPCSTR());
						pCableItem->prop()->SetOriginalValue(_T("General"), _T("Length"), rValue.operator LPCSTR());
					}
					else if(14 == __INDEX__[nCol])
					{
						if(pTable)
						{
							CELOADTable::Record data;
							data.FieldValueMap[_T("C_SPEC")] = rSpec.operator LPCSTR();
							data.FieldValueMap[_T("C_CORE")] = rCore.operator LPCSTR();
							data.FieldValueMap[_T("C_SIZE")] = rSize.operator LPCSTR();
							for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin(); itr != pTable->m_RecordEntry.end();++itr)
							{
								if(MatchControlCableSetting(data, *itr))
								{
									rOd = itr->FieldValueMap[_T("C_OD")].c_str();

									if((isalpha(rOd[0])) && (!isdigit(rOd[0])))
									{
										const string rLoadId  = pLoadItem->GetName();
										pCableItem->SetDeleteFlag(true);
										ELOAD_LOG4CXX_ERROR(mylogger , rLoadId , 44);
										++m_nFailCount;
										break;
									}

									pCableItem->prop()->SetValue(_T("Cable Spec"), _T("Cable OD"), rOd.operator LPCSTR());
									pCableItem->prop()->SetOriginalValue(_T("Cable Spec"), _T("Cable OD"), rOd.operator LPCSTR());
								}
							}
						}
					}
					else if(15 == __INDEX__[nCol])
					{
						if(pTable)
						{
							CELOADTable::Record data;
							data.FieldValueMap[_T("C_SPEC")] = rSpec.operator LPCSTR();
							data.FieldValueMap[_T("C_CORE")] = rCore.operator LPCSTR();
							data.FieldValueMap[_T("C_SIZE")] = rSize.operator LPCSTR();
							for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin(); itr != pTable->m_RecordEntry.end();++itr)
							{
								if(MatchControlCableSetting(data, *itr))
								{
									rWeight = itr->FieldValueMap[_T("C_WEIGHT")].c_str();

									if((isalpha(rWeight[0])) && (!isdigit(rWeight[0])))
									{
										const string rLoadId  = pLoadItem->GetName();
										pCableItem->SetDeleteFlag(true);
										ELOAD_LOG4CXX_ERROR(mylogger , rLoadId , 45);
										++m_nFailCount;
										break;
									}

									pCableItem->prop()->SetValue(_T("Cable Spec"), _T("Weight"), rWeight.operator LPCSTR());
									pCableItem->prop()->SetOriginalValue(_T("Cable Spec"), _T("Weight"), rWeight.operator LPCSTR());
								}
							}
						}
					}
					else if(16 == __INDEX__[nCol]) //! REMARK
					{
					}
				}
			}
			int progress = int((double(nIndex) / double(nRowCount))*100.);
			if(pDlg) pDlg->UpdateWorkStatus(rTitle , progress);
			++nIndex;
		}

	}
	catch(...)
	{
		xlApp.Close();
		return ERROR_BAD_ENVIRONMENT;
	}
	xlApp.Close();

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author KHS	

	@date 2009-06-25 오후 3:34:41	

	@param	

	@return		
**/
int CELoadItemViewBar::UserCableExcelImportExecute(const CString& rExcelFilePath, const int& nOption)
{
	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
	if(NULL == pDlg)
	{
		UserCableExcelImportParam param;
		param.pThis = this;
		param.rExcelFilePath = rExcelFilePath;
		param.nOption = nOption;

		CWorkStatusDlg dlg;
		dlg.m_pThread = AfxBeginThread(CELoadItemViewBar::StatusUserCableExcelImportThreadEntry , &param , THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
		if(NULL != dlg.m_pThread)
		{
			InterlockedExchange((LONG*)(&(dlg.m_bThreadRunning)) , TRUE);
			dlg.DoModal();	

			return ERROR_SUCCESS;
		}
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author KHS	

	@date 2009-06-25 오후 3:37:07	

	@param	

	@return		
**/
UINT CELoadItemViewBar::StatusUserCableExcelImportThreadEntry(LPVOID pVoid)
{
	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
	if(pDlg)
	{
		UserCableExcelImportParam* p = (UserCableExcelImportParam*)pVoid;
		p->pThis->UserCableExcelImport(p->rExcelFilePath, p->nOption);

		InterlockedExchange((LONG*)(&(pDlg->m_bThreadRunning)) , FALSE);
		if(pDlg) pDlg->PostMessage(WM_COMMAND , IDOK);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	모든 User Cable을 삭제

	@author KHS	

	@date 2009-07-02 오전 10:05:32	

	@param	

	@return		
**/
int CELoadItemViewBar::DeleteAllUserCable(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	
	list<CELoadItem*> CableItemList;

	docData.GetELoadItemListOf( CableItemList, CCableItem::TypeString() ,_T("User"));
	for(list<CELoadItem*>::iterator itr = CableItemList.begin(); itr != CableItemList.end();++itr)
	{
		CCableItem* pCableItem = static_cast<CCableItem*>(*itr);
		if(pCableItem)
		{
			const string rCreateBy = pCableItem->prop()->GetValue(_T("General"),  _T("Created By"));
			if("User" == rCreateBy) pCableItem->SetDeleteFlag(true);
		}
	}
	return ERROR_SUCCESS;
}


/**
	@brief	REVISION ITEM DELETE

	@author KHS	

	@date 2009-07-13 오전 11:21:33	

	@param	

	@return		
**/
void CELoadItemViewBar::OnRevItemDelete()
{
	m_wndItemClassTreeCtrl.OnRevItemDelete();
}

/**
	@brief	

	@author KHS	

	@date 2009-07-23 오전 10:27:55	

	@param	

	@return		
**/
string CELoadItemViewBar::GetUserCableImportResultString(void)
{
	ostringstream oss;

	oss << _T("TOTAL USER CABLE |") << m_nTryCount << _T(" (SETS) |");
	oss << _T("USER CABLE CREATION |") << m_nSuccessCount << _T(" (SETS) | NOT-CREATION USER CABLE |") << m_nFailCount << _T(" (SETS) |");

	return oss.str();
}

/**
	@brief	open random document depend on revision.

	@author BHK

	@date 2009-12-29

	@param	

	@return
**/
int CELoadItemViewBar::OpenDocumentDependOn(const CString& rProductName , const CString& rTarget , const CString& rExtraParam)
{
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp)
	{
		LONG lRecordCount = 0L;
		{
			CELoadDocData& docData = CELoadDocData::GetInstance();
			const string rMDBFilePath = docData.GetProjectMDBFilePath();

			CADODB adoDB;
			const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;
			if(TRUE == adoDB.DBConnect(rConnectionString))
			{
				CString rSqlFormat = (_T("SELECT * FROM T_PRODUCT_REVISION WHERE C_PRODUCT_NAME = '%s' ORDER BY C_REVISION_NO DESC")) , rSql;
				rSql.Format(rSqlFormat , rProductName);

				adoDB.OpenQuery(rSql.operator LPCTSTR());
				adoDB.GetRecordCount(&lRecordCount);
			}
		}

		if(lRecordCount > 0)
		{
			CCompareRevisionDataDoc::COMPARE_ITEM CompareItem;
			CompareItem.rProductName    = rProductName;
			CompareItem.nLeftSideRevNo  = -1;
			CompareItem.nRightSideRevNo = -1;
			const BOOL res = pApp->OpenNewDocumentIfNeed(_T("REVISION") , LPCTSTR(&CompareItem));
		}
		else
		{
			pApp->OpenNewDocumentIfNeed(rTarget , rExtraParam);
		}
		
		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	callback function "SHOW BUS TO BUS"

	@author	HumKyung

	@date	2010.12.23

	@param

	@return
*/
int CELoadItemViewBar::ShowBusToBusCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText)
{
	m_wndBusTreeView.SetActionString(_T("SHOW BUS TO BUS"));
	m_wndBusTreeView.ShowWindow(SW_NORMAL);
	for(map<CWnd*,vector<string> >::iterator itr = m_ActionWndMap.begin();itr != m_ActionWndMap.end();++itr)
	{
		if(itr->first != &m_wndBusTreeView) itr->first->ShowWindow(SW_HIDE);
	}

	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	CString strDocName;
	CDocTemplate* pSelectedTemplate = NULL;
	POSITION pos = pApp->GetFirstDocTemplatePosition();
	while (pos != NULL) 
	{
		pSelectedTemplate = (CDocTemplate*) pApp->GetNextDocTemplate(pos);
		pSelectedTemplate->GetDocString(strDocName, CDocTemplate::docName);
		if (strDocName == _T("BUS_TO_BUS"))
		{
			POSITION docPos = pSelectedTemplate->GetFirstDocPosition();
			pApp->OpenNewDocumentIfNeed(_T("BUS_TO_BUS"));
		}
	}

	return ERROR_SUCCESS;
}