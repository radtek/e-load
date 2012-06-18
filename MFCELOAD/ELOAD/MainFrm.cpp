
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "ELOAD.h"

#include "MainFrm.h"
#include "AssignLoadToBusDlg.h"
#include "NewProjectDlg.h"
#include "ELoadDocData.h"
#include "ExcelImportDlg.h"
#include "Tables/LoadSummaryRefTable.h"
#include "pyfromc.h"
#include "SimpleProgressBar.h"
#include "ELOADPropSheet.h"
#include "CableDataDlg.h"
#include "Tables/ProjectSettingTable.h"
#include "CableSizingStartUpDlg.h"
#include "LoadSummaryStartUpDlg.h"
#include "SimpleAnimationDlg.h"
#include "WorkStatusDlg.h"
#include "ConduitTableDlg.h"
#include "SelectProjectDlg.h"
#include "ELoadItemViewBar.h"
#include "VoltageSettingDlg.h"
#include "ToolShell.h"

#include "LoadSummaryPropPage.h"
#include "CableSizingSettingDlg.h"
#include "PowerCableCreationDlg.h"
#include "PowerCableCreationSettingDlg.h"
#include "PowerCableNamingRuleDlg.h"
#include "VirtualLoadSummaryDlg.h"
#include "ControlTypeCreationDlg.h"
#include "SchematicTypeCreationDlg.h"
#include "ControlCableCreationSettingDlg.h"
#include "MccbSizeSettingDlg.h"
#include "ControlCableSpecCreationDlg.h"
#include "WizardDlg.h"
#include "AssignSchematicTypeToLoadDlg.h"
#include "CableRouteTypeCreationDlg.h"
#include "CableRouteGroupCreationDlg.h"
#include "CableRouteAssignTOLoadDlg.h"
#include "CableTrayFillRateCalculationDlg.h"
#include "CableBMSummaryDlg.h"
#include "ProjectInfoDlg.h"
#include "CableScheduleSettingDlg.h"
#include "TemplateMappingDlg.h"
#include "DataResultDlg.h"
#include "DrumScheduleCreationDlg.h"
#include "CompareRevisionDataDoc.h"

#include "commands/CommandUndo.h"
#include <util/FileTools.h>
#include "commands/CommandCalculateLoadSummary.h"

#include <Zip\inc\ZipArchive.h>

#include "VersionNo.h"
#include "DataResultDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static CMainFrame* __MAINFRAME__ = NULL;
CMainFrame* ELOADGetMainWnd()
{
	return __MAINFRAME__;
}

// CMainFrame
/*
* 50 IDs have been reserved in resource.h.
* attention if you modify the following values
*/
IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_TOOLS_ASSIGNLOADTOBUS, &CMainFrame::OnToolsAssignloadtobus)
	ON_COMMAND(ID_ADMIN_CREATE_PROJECT, &CMainFrame::OnAdminCreateProject)
	ON_COMMAND(ID_VIEW_PROPERTIES_BAR, &CMainFrame::OnViewPropertiesBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROPERTIES_BAR, &CMainFrame::OnUpdateViewPropertiesBar)
	ON_COMMAND(ID_VIEW_ITEM_BAR, &CMainFrame::OnViewItemBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ITEM_BAR, OnUpdateLoadItemBar)
	ON_COMMAND(ID_VIEW_OUTPUT_BAR, &CMainFrame::OnViewOutputBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OUTPUT_BAR, &CMainFrame::OnUpdateViewOutputBar)
	ON_COMMAND(ID_VIEW_TOOLBAR, &CMainFrame::OnViewToolbar)
	ON_COMMAND(ID_TOOLS_PROJECT_SETTING, &CMainFrame::OnToolsProjectSetting)
	ON_COMMAND(ID_TOOLS_RELOAD_PYTHON, &CMainFrame::OnToolsReloadPython)
	ON_COMMAND(ID_CALC_LOAD_SUMMARY , &CMainFrame::OnCalcLoadSummary)
	ON_COMMAND(ID_CALC_CABLE_SIZE , &CMainFrame::OnCalcCableSize)
	ON_COMMAND(ID_EXCEL_IMPORT, CMainFrame::OnExcelImport)
	ON_COMMAND(ID_EXCEL_EXPORT, CMainFrame::OnExcelExport)
	ON_UPDATE_COMMAND_UI(ID_ADMIN_CREATE_PROJECT, &CMainFrame::OnUpdateAdminCreateProject)
	ON_WM_CLOSE()

	ON_COMMAND(ID_TOOLS_PROJECTSELECT, &CMainFrame::OnToolsProjectselect)
	ON_COMMAND(ID_LIBRARY_CABLE_DATA, &CMainFrame::OnLibraryCableData)
	ON_COMMAND(ID_LIBRARY_CONDUIT_TABLE, &CMainFrame::OnLibraryConduitTable)
	ON_COMMAND(ID_PROJECT_VOLTAGE, &CMainFrame::OnProjectVoltage)
	ON_COMMAND(ID_CABLE_POWERCABLECREATION, &CMainFrame::OnCablePowercablecreation)
	ON_COMMAND(ID_CABLE_POWERCABLECREATION_SETTING, &CMainFrame::OnCablePowercablecreationSetting)
	ON_COMMAND(ID_CABLE_POWERCABLENAMINGRULE, &CMainFrame::OnCablePowerCableNamingRule)
	ON_COMMAND(ID_LIBRARY_CABLE_SIZING_SETTING, &CMainFrame::OnLibraryCableSizingSetting)

	ON_COMMAND(ID_LOAD_VIRTUAL_LOAD_SUMMARY, &CMainFrame::OnLoadVirtualLoadSummary)
	ON_COMMAND(ID_CONTROL_TYPE_CREATION, &CMainFrame::OnControlTypeCreation)
	ON_COMMAND(ID_SCHEMATIC_TYPE_CREATION, &CMainFrame::OnSchematicTypeCreation)
	ON_COMMAND(ID_CONTROLCABLE_CREATION_SETTING, &CMainFrame::OnControlcableCreationSetting)
	ON_COMMAND(ID_MCCB_SIZE_SETTING, &CMainFrame::OnMccbSizeSetting)
	ON_COMMAND(ID_CONTROL_CABLE_SPEC_CREATION, &CMainFrame::OnControlCableSpecCreation)
	ON_COMMAND(ID_SCHEMATIC_TYPE_ASSIGN_TO_LOAD, &CMainFrame::OnSchematicTypeAssignToLoad)
	ON_COMMAND(ID_FILE_NEW, &CMainFrame::OnFileNew)
	ON_COMMAND(ID_CABLE_ROUTE_TYPE_CREATION, &CMainFrame::OnCableRouteTypeCreation)
	ON_COMMAND(ID_CABLE_ROUTE_GROUP_CREATION, &CMainFrame::OnCableRouteGroupCreation)
	ON_COMMAND(ID_CABLE_ROUTE_ASSIGNTOLOAD, &CMainFrame::OnCableRouteAssigntoload)
	ON_COMMAND(ID_CABLE_TRAY_FILLRATE_EXECUTION, &CMainFrame::OnCableTrayFillrateExecution)
	ON_COMMAND(ID_CONTROL_CABLE_CREATION, &CMainFrame::OnControlCableCreation)
	///ON_COMMAND(ID_CABLE_BM_SUMMARY, &CMainFrame::OnCableBmSummary)
	ON_COMMAND(ID_POWER_CABLE_VERIFICATION_EXECUTION, &CMainFrame::OnPowerCableVerificationExecution)
	ON_COMMAND(ID_PROJECT_INFORMATION, &CMainFrame::OnProjectInformation)
	ON_COMMAND(ID_TOOLS_MAKE_EXCEL_REPORT_FORMAT, &CMainFrame::OnToolsMakeExcelReportFormat)
	ON_COMMAND(ID_TOOLS_TEMPLATE_MAPPING, &CMainFrame::OnToolsTemplateMapping)
	ON_COMMAND(ID_CABLE_BM_SUMMARY , &CMainFrame::OnCableBMSummary)
	ON_COMMAND(ID_CABLE_SCHEDULE_SETTING, &CMainFrame::OnCableScheduleSetting)
	ON_COMMAND(ID_CABLE_SCHEDULE_CREATION , &CMainFrame::OnCableScheduleCreation)
	ON_COMMAND(ID_DRUM_SCHEDULE_CREATION , &CMainFrame::OnDrumScheduleCreation)
	ON_COMMAND(ID_USER_CABLE_IMPORT, &CMainFrame::OnUserCableImport)
	ON_COMMAND(ID_POWERCABLE_ERROR_LOG, &CMainFrame::OnPowercableErrorLog)
	ON_COMMAND(ID_CONTROLCABLE_ERROR_LOG, &CMainFrame::OnControlcableErrorLog)
	ON_COMMAND(ID_LOAD_SUMMARY_ERROR_LOG, &CMainFrame::OnLoadSummaryErrorLog)
	ON_COMMAND(ID_EXCEL_IMPORT_ERROR_LOG, &CMainFrame::OnExcelImportErrorLog)
	ON_COMMAND(ID_ACCELERATOR_HELP, &CMainFrame::OnAcceleratorHelp)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2005);

	__MAINFRAME__ = this;

	///m_pMenuCommandSet=	new MenuCommandSet();
}

CMainFrame::~CMainFrame()
{
	try
	{
		///SAFE_DELETE(m_pMenuCommandSet);
	}
	catch(...)
	{
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-03-30 오후 5:38:25	

	@param	

	@return		
**/
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Create the ribbon bar
	if (!m_wndRibbonBar.Create(this))
	{
		return -1;   //Failed to create ribbon bar
	}
	
	InitializeRibbon();

	// 보관된 값에 따라 비주얼 관리자 및 스타일을 설정합니다.
	OnApplicationLook(theApp.m_nAppLook);

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // 사용할 수 있는 다른 스타일...
	mdiTabParams.m_bActiveTabCloseButton = TRUE;      // FALSE로 설정하여 탭 영역 오른쪽에 닫기 단추를 배치합니다.
	mdiTabParams.m_bTabIcons = FALSE;    // TRUE로 설정하여 MDI 탭의 문서 아이콘을 활성화합니다.
	mdiTabParams.m_bAutoColor = TRUE;    // FALSE로 설정하여 MDI 탭의 자동 색 지정을 비활성화합니다.
	mdiTabParams.m_bDocumentMenu = TRUE; // 탭 영역의 오른쪽 가장자리에 문서 메뉴를 활성화합니다.
	EnableMDITabbedGroups(TRUE, mdiTabParams);
	
	// 메뉴 모음을 활성화해도 포커스가 이동하지 않게 합니다.
	CMFCPopupMenu::SetForceMenuFocus(FALSE);
	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	//------------------
	// Create ELoadItem view:
	//------------------
	if (!m_wndELoadItemView.Create (_T("Equipment Type"), this, CRect (0, 0, 230, 200),//
		TRUE, 
		ID_VIEW_CLASS,
		WS_CHILD | WS_CLIPSIBLINGS |WS_VISIBLE| WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Equipment Type Bar\n");
		return FALSE;      // fail to create
	}

	// TODO: 도구 모음 및 메뉴 모음을 도킹할 수 없게 하려면 이 다섯 줄을 삭제하십시오.
	m_wndELoadItemView.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndELoadItemView);

	// Visual Studio 2005 스타일 도킹 창 동작을 활성화합니다.
	CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 스타일 도킹 창 자동 숨김 동작을 활성화합니다.
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 메뉴 항목 이미지를 로드합니다(표준 도구 모음에 없음).
///	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// 도킹 창을 만듭니다.
	if (!CreateDockingWindows())
	{
		TRACE0("도킹 창을 만들지 못했습니다.\n");
		return -1;
	}
	m_wndPropertiesBar.ShowPane(FALSE , FALSE , FALSE);

	CDockablePane* pTabbedBar = NULL;
	m_wndPropertiesBar.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndPropertiesBar);
	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);
	//! 프로그램 시작시 Output 창을 보이지 않게 한다.
	m_wndOutput.ShowPane( FALSE , FALSE , TRUE );
	
	// 향상된 창 관리 대화 상자를 활성화합니다.
	///EnableWindowsDialog(ID_WINDOW_MANAGER, IDS_WINDOWS_MANAGER, TRUE);

	// 도구 모음 및 도킹 창 메뉴 바꾸기를 활성화합니다.
///	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// 빠른(<Alt> 키를 누른 채 끌기) 도구 모음 사용자 지정을 활성화합니다.
	///CMFCToolBar::EnableQuickCustomization();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	CELoadDocData& docData = CELoadDocData::GetInstance();
	///docData.SetModifiedFlag(false);
	
	const CString rProjectNo = docData.GetProjectNo().c_str();
	const CString rProjectName = docData.GetProjectName().c_str();
	CString rTitle;
	rTitle = _T("e-LOAD v") + CString(STRPRODUCTVER) + _T(" (") + rProjectNo + _T(", ") + rProjectName + _T(")");

	this->SetTitle(rTitle);

	return TRUE;
}

/**
	@brief	

	@author	BHK
*/
BOOL CMainFrame::CreateDockingWindows()
{
	// 속성 창을 만듭니다.
	CString strPropertiesWnd = _T("Properties");
	if (!m_wndPropertiesBar.Create(strPropertiesWnd, this, CRect(0, 0, 250, 200), TRUE, ID_VIEW_PROPERTIES, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("속성 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}

	// 출력 창을 만듭니다.
	if (!m_wndOutput.Create(_T("Output"), this, CRect(0, 0, 100, 100), TRUE, 0x1011/*ID_VIEW_OUTPUTWND*/, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("출력 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);

	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_BAR_HC : IDI_PROPERTIES_BAR), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndPropertiesBar.SetIcon(hPropertiesBarIcon, FALSE);

	UpdateMDITabbedBarsIcons();
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 메뉴를 검색합니다. */);
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CMDIFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}
/*
	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
*/
	return lres;

}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

/**
	@brief	

	@author	BHK
*/
void CMainFrame::OnViewItemBar()
{
	m_wndELoadItemView.ShowPane( m_wndELoadItemView.IsVisible() ? FALSE : TRUE , FALSE , TRUE );
}

void CMainFrame::OnUpdateLoadItemBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndELoadItemView.IsVisible());
}

/**
	@brief	

	@author BHK	

	@date 2009-07-22 오후 4:29:49	

	@param	

	@return		
*/
void CMainFrame::OnViewOutputBar()
{
	m_wndOutput.ShowPane( m_wndOutput.IsVisible() ? FALSE : TRUE , FALSE , TRUE );
}

/**
	@brief	

	@author BHK	

	@date 2009-07-22 오후 4:30:14	

	@param	

	@return		
*/
void CMainFrame::OnUpdateViewOutputBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndOutput.IsVisible());
}

void CMainFrame::OnUpdateViewPropertiesBar(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_wndPropertiesBar.IsVisible() );
}

/**
	@brief	

	@author	BHK
*/
void CMainFrame::OnViewPropertiesBar()
{
	m_wndPropertiesBar.ShowPane(m_wndPropertiesBar.IsVisible() ? FALSE : TRUE , FALSE , TRUE );
}


/**
	@brief	

	@author	BHK
*/
void CMainFrame::OnViewToolbar()
{
	// TODO: Add your command handler code here
	//m_wndToolBar.ShowPane( m_wndToolBar.IsVisible() ? FALSE : TRUE , FALSE , TRUE );
}

/**
	@brief	open project setting dialog.

	@author	BHK
*/
void CMainFrame::OnToolsProjectSetting()
{
	CLoadSummaryPropPage dlg;
	if(IDOK == dlg.DoModal())
	{
	}
}

/**
	@brief	reload python module.

	@author	BHK
*/
void CMainFrame::OnToolsReloadPython()
{
	python.ReloadPython();
}

/**
	@brief	LOAD를 BUS에 할당하는 DIALOG를 호출한다.
	OK이면 Class Tree의 BUS ITEM을 새롭게 생성한다.

	@author	BHK
*/
void CMainFrame::OnToolsAssignloadtobus()
{
	CAssignLoadToBusDlg dlg;
	if(IDOK == dlg.DoModal())
	{
		m_wndELoadItemView.UpdateShowLoadBusItem();
	}
}

/**
	@brief	NEW PROJECT DIALOG를 호출한다.

	@author	BHK
*/
void CMainFrame::OnAdminCreateProject()
{
	CNewProjectDlg dlg;
	if(IDOK == dlg.DoModal())
	{
	}
}

/**
	@brief  Import Excel File for load item

	@author	humkyung

	@date	????.??.??
*/
void CMainFrame::OnExcelImport()
{
	CExcelImportDlg dlg;
	if(IDOK == dlg.DoModal())
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();

		CString rMsg;
		const string rResultString = dlg.GetResultString();
		if(docData.m_nErrorCount > 0)
			rMsg.Format(_T("%s%d error(s)") , rResultString.c_str() , docData.m_nErrorCount);
		else	rMsg.Format(_T("%s") , rResultString.c_str());

		CDataResultDlg dlg(rMsg);
		dlg.DoModal();

		list<CELoadItem*> LoadItemList;
		docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
		//if(pLoadItemList)
		{
			vector<CLoadItem*> LoadItemEntry;
			for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
			{
				CLoadItem* pLoad = static_cast<CLoadItem*>(*itr);

				///!Import한 LOAD에 Bus ID의 Bus가 실제 존해하지 않으면 제거
				if(pLoad)
				{
					const string rBusID = pLoad->prop()->GetValue(_T("Panel"), _T("Bus ID"));
					if(NULL == docData.FindBusItemByName(rBusID))
					{
						pLoad->prop()->SetOriginalValue(_T("Panel"), _T("Bus ID"), _T(""));
						pLoad->prop()->SetValue(_T("Panel"), _T("Bus ID"), _T(""));
					}
					LoadItemEntry.push_back(pLoad);
				}
			}

			m_wndELoadItemView.UpdateShowLoadBusItem();
			m_wndELoadItemView.RemakeLoadListCtrl(_T("") , _T("") , LoadItemEntry);
			m_wndPropertiesBar.ShowPane(FALSE,FALSE,FALSE);
			CELOADApp* pApp = (CELOADApp*)AfxGetApp();
			if(pApp)
			{
				//! LOG를 화면에 출력
				CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
				if(pFrame)
				{
					//! Log 파일을 database에 저장한다.
					AppenderPtr appender = mylogger->getAppender(_T("ELoadAppender"));
					FileAppenderPtr fileappender = (FileAppenderPtr)appender;
					const string rLogFilePath = fileappender->getFile();
					pApp->PutLogFileToDatabase(rLogFilePath, _T("Excel Import"));

					pFrame->m_wndOutput.Navigate2(rLogFilePath.c_str());
					pFrame->m_wndELoadItemView.m_wndItemClassTreeCtrl.DisplayCableItemCount();
				}
			}
		}
	}
}

/**
	@brief	

	@author HumKyung.BAEK	

	@date 2010-03-16

	@param	

	@return		
*/
UINT CMainFrame::StatusExportThreadEntry(LPVOID pVoid)
{       
	return ((CMainFrame*)pVoid)->StatusExportThread();
}

/**
	@brief  export load data to excel file.

	@author	HumKyung.BAEK
*/
UINT CMainFrame::StatusExportThread()
{
	CWorkStatusDlg* pWorkStatusDlg = CWorkStatusDlg::GetInstance();
	try
	{
		InterlockedExchange((LONG*)(&(pWorkStatusDlg->m_bThreadRunning)) , TRUE);

		CExcelAutomation xlApp;
		
		//! 엑셀 파일을 오픈한다.
		const CString rTemplateExlFilePath = GetExecPath() + _T("Excel Format\\FORMAT_LOAD_IMPORT.xls");
		if(!xlApp.Open(rTemplateExlFilePath))
		{
			AfxMessageBox(_T("Can't Start Excel and get Application object."));
			return IDCANCEL;
		}
		///xlApp.SetActiveSheet( 1 );

		//! write load item data to excel
		CELoadDocData& docData = CELoadDocData::GetInstance();
		CADODB adoDB;
		string rProjectMDBFilePath = docData.GetProjectMDBFilePath();
		const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rProjectMDBFilePath + DB_PASSWORD;
		if(TRUE == adoDB.DBConnect(rConnectionString))
		{
			adoDB.OpenQuery(_T("SELECT * FROM T_EXCEL_MAPPING"));
			LONG lRecordCount = 0L;
			adoDB.GetRecordCount(&lRecordCount);
			if(lRecordCount > 0)
			{
				map<CString,CString> FieldValueMap;
				for(int i = 0;i < adoDB.GetFieldCount();++i)
				{
					STRING_T rFieldName , _rValue;
					rFieldName = adoDB.GetFieldName(i);
					adoDB.GetFieldValue(0 , rFieldName , &_rValue);
					CString rValue(_rValue.c_str());
					rValue.Trim();
					if(!rValue.IsEmpty()) FieldValueMap.insert(make_pair(rFieldName.c_str() , rValue));
				}

				int nRow = 4;	//! default value
				list<CELoadItem*> BusGroupItemList , LoadItemList;
				docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString() , _T("*"));
				docData.GetELoadItemListOf(BusGroupItemList , CBusGroupItem::TypeString() , _T("*"));
				for(list<CELoadItem*>::iterator itr = BusGroupItemList.begin();itr != BusGroupItemList.end();++itr)
				{
					CBusGroupItem* pBusGroupItem = static_cast<CBusGroupItem*>(*itr);
					for(vector<CBusItem*>::iterator jtr = pBusGroupItem->m_BusRefEntry.begin();jtr != pBusGroupItem->m_BusRefEntry.end();++jtr)
					{
						CItemSelectionSet SelectionSet;
						docData.GetLoadItemSetInBus(SelectionSet,(*jtr)->prop()->GetValue(_T("General") , _T("Bus ID")));
						for(int i = 0;i < SelectionSet.GetItemCount();++i)
						{
							CLoadItem* pLoadItem = static_cast<CLoadItem*>(SelectionSet.GetItemAt(i));

							if(_T("USER") == CString(pLoadItem->prop()->GetValue(_T("General") , _T("Created By")).c_str()).MakeUpper())
							{
								continue;
							}

							vector<string> oResult;
							for(map<CString,CString>::iterator ktr = FieldValueMap.begin();ktr != FieldValueMap.end();++ktr)
							{
								CTokenizer<CIsComma>::Tokenize(oResult , ktr->first.operator LPCSTR() , CIsComma());
								if(!ktr->second.IsEmpty() && (2 == oResult.size()))
								{
									string rValue = pLoadItem->prop()->GetValue(oResult[0],oResult[1]);
									if((CString("Type").MakeUpper() == oResult[0].c_str()) && (CString("Load Category").MakeUpper() == oResult[1].c_str()))
									{
										rValue = rValue.substr(0,1);
									}
									else if((CString("Type").MakeUpper() == oResult[0].c_str()) && (CString("Operation Mode").MakeUpper() == oResult[1].c_str()))
									{
										rValue = rValue.substr(0,1);
									}
									else if((CString("Characteristic").MakeUpper() == oResult[0].c_str()) && 
										((CString("PF(100% Load)").MakeUpper() == oResult[1].c_str()) || (CString("EFF(100% Load)").MakeUpper() == oResult[1].c_str())))
									{
										if(false == pLoadItem->prop()->GetCategoryByName(oResult[0])->IsLocked(oResult[1]))
										{
											rValue = _T("");
										}
									}

									CString rCellNo;
									rCellNo.Format(_T("%s%d") , ktr->second , nRow);
									xlApp.SetCellValue(rCellNo , CString(rValue.c_str()));
								}
							}
							++nRow;

							if(pWorkStatusDlg)
							{
								int progress = int((double)(nRow - 4) / (double)LoadItemList.size() * 100.f);
								pWorkStatusDlg->UpdateWorkStatus(CString("Export Load Data...") , progress);
							}
						}

						
						/*
						for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
						{	
						}
						*/
					}
				}
			}
		}

		xlApp.SaveAs(m_sExcelFilePath);
		xlApp.Close();

		InterlockedExchange((LONG*)(&(pWorkStatusDlg->m_bThreadRunning)) , FALSE);
		//////////////////////////////////////////////////////////////////////////
		/// close work status dialog
		pWorkStatusDlg->SendMessage(WM_COMMAND , IDOK );
	}
	catch(exception& e)
	{
		AfxMessageBox(e.what());

		InterlockedExchange((LONG*)(&(pWorkStatusDlg->m_bThreadRunning)) , FALSE);

		if(pWorkStatusDlg) pWorkStatusDlg->SendMessage(WM_COMMAND , IDCANCEL );

		return IDCANCEL;
	}

	return IDOK;
}

/**
	@brief  Export load data to Excel File

	@author	HumKyung.BAEK

	@date	2010.03.17
*/
void CMainFrame::OnExcelExport()
{
	CString strExtFile, strFilePath;
	CString strFilter =  _T("엑셀 문서(*.xls)|*.xls||");
	strExtFile = _T("*.xls"); 

	CFileDialog dlg(FALSE,strExtFile,strExtFile,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, strFilter);
	if(IDOK == dlg.DoModal())
	{
		m_sExcelFilePath = dlg.GetPathName();

		CWorkStatusDlg dlg;
		try
		{
			dlg.m_pThread = AfxBeginThread(StatusExportThreadEntry, this, THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
			if (NULL == dlg.m_pThread)
			{
				AfxMessageBox("Can't create thread!!!");
			}
			else
			{
				dlg.DoModal();
			}
		}
		catch(exception& e)
		{
			AfxMessageBox(e.what());
		}
	}
}

/**
	@brief	LoadSummary Thread Function.

	@author	BHK
*/
static UINT gfnCalcLoadSummary(LPVOID pVoid)
{
	CWorkStatusDlg* pDlg = (CWorkStatusDlg*)pVoid;

	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> LoadItemList;
	docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
	///if(pLoadItemList)
	{
		const size_t nCount = LoadItemList.size();
		int nIndex = 0;

		ELOAD_LOG4CXX_RESET(mylogger, _T("Load Summary Calculation Error Messages"));

		CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
		string* pValue = pSettingTable->GetFieldValue(_T("C_CUR_TABLE_FOR_LOAD_SUMMARY"));
		if((NULL == pValue) || pValue->empty())
		{
			ELOAD_LOG4CXX_ERROR(mylogger , _T("SYSTEM") , 23);
			if(pDlg) pDlg->SendMessage(WM_COMMAND , IDOK);
			return ERROR_BAD_ENVIRONMENT;
		}
		
		for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr,++nIndex)
		{
			CLoadItem* pLoadItem = static_cast<CLoadItem*>(*itr);
			const string rLoadID = pLoadItem->GetName();
			if(pLoadItem->IsDeleted()) continue;	//! 삭제된 아이템은 SKIP....

			const string rLoadCategory = pLoadItem->prop()->GetValue(_T("Type") , _T("Load Category"));
			
			double nRatingCapacity = 0.f;
			pLoadItem->CalcRatingCapacity(nRatingCapacity);

			//! MOTOR TYPE은 무조건 POLES 값이 존재해야 한다.
			const string rPoles = pLoadItem->prop()->GetValue(_T("Rating") , _T("Poles"));
			if((_T("MOTOR") == rLoadCategory) && rPoles.empty())
			{
				//! Load Summary 계산 결과를 클리어한다.
				pLoadItem->prop()->SetValue(_T("Load Summary") , _T("PF") , _T("") , true);
				pLoadItem->prop()->SetValue(_T("Load Summary") , _T("EFF") , _T("") , true);
				pLoadItem->prop()->SetValue(_T("Load Summary") , _T("KW") , _T("") , true);
				pLoadItem->prop()->SetValue(_T("Load Summary") , _T("KVAR") , _T("") , true);
				pLoadItem->prop()->SetValue(_T("Load Summary") , _T("KVA") , _T("") , true);
				ELOAD_LOG4CXX_ERROR(mylogger , rLoadID, 15);
				continue;
			}

			int nPoles = 0;
			try
			{
				nPoles = atoi(rPoles.c_str());
			}
			catch(...)
			{
				continue;
			}

			pLoadItem->CalcLoadSummary();

			int nProgress = int((double(nIndex) / double(nCount)) * 100.f);
			pDlg->UpdateWorkStatus(CString(_T("Load Summary")) , nProgress);
		}

		//! Load Summary를 엑셀 파일로 저장한다.
		python.CallPyWriteLoadSummaryToDataFile();

		if(pDlg) pDlg->SendMessage(WM_COMMAND , IDOK);

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	각 Load마다 Load Summary를 계산한다.

	@author	HumKyung

	@date	????.??.??

	@param	

	@return
*/
void CMainFrame::OnCalcLoadSummary()
{
	CLoadSummaryStartUpDlg dlg;
	if(IDOK == dlg.DoModal())
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		if(true == docData.IsModified())
		{
			if(IDYES == AfxMessageBox(_T("data is modified. calculate after saving?") , MB_YESNO|MB_ICONQUESTION))
			{
				CELOADApp* pApp = (CELOADApp*)AfxGetApp();
				if(pApp)
				{
					docData.Save();

					//! 저장 후 prop TEXT 상태 초기화
					vector<string> emptyEntry;
					pApp->UpdateAllViews(emptyEntry);
				}

			}
			else
			{
				COMMAND::CCommandUndo cmdUndo;
				cmdUndo.Execute();
			}
		}

		COMMAND::CCommandCalculateLoadSummary cmdCalculateLoadSummary;
		cmdCalculateLoadSummary.Execute();
		CDataResultDlg dlg(cmdCalculateLoadSummary.GetResultString().c_str());
		const int res = dlg.DoModal();

		CELOADApp* pApp = (CELOADApp*)AfxGetApp();
		if(pApp)
		{
			//! set modified flag.
			CELoadDocData& docData = CELoadDocData::GetInstance();
			list<CELoadItem*> LoadItemList;
			docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
			////if(pLoadItemList)
			{
				for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
				{
					CLoadItem* pLoad = static_cast<CLoadItem*>(*itr);
					if(pLoad->prop()->IsModified())
					{
						docData.SetModifiedFlag(true);
						break;
					}
				}
			
				//! LOAD SUMMARY와 LOAD TYPE의 모든 뷰들을 갱신한다.
				vector<string> DocTypeEntry;
				DocTypeEntry.push_back(_T("LOAD SUMMARY"));
				DocTypeEntry.push_back(CLoadItem::TypeString());
				pApp->UpdateAllViews(DocTypeEntry);

				//! property window에 load item이 표시되고 있으면 property window을 갱신한다.
				CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
				if(pFrame) pFrame->m_wndPropertiesBar.UpdateAllProperties(CLoadItem::TypeString());

				//! LOAD SUM 후 BUS CATEGORY쪽의 LOAD SUMMARY SHEET를 보여준다
				CELOADApp* pApp = (CELOADApp*)AfxGetApp();
				if(pApp)
				{
					//! 압축 파일을 만들기 위해 PDB,BUS 뷰를 모두 생성한다.
					//! 마지막 VIEW(BUS)만 보일것이다.
					pApp->OpenNewDocumentIfNeed(_T("LOAD SUMMARY") , _T("ALL PDB LOAD SUMMARY"));
					pApp->OpenNewDocumentIfNeed(_T("LOAD SUMMARY") , _T("ALL BUS LOAD SUMMARY"));
				}

				{
					AppenderPtr appender = mylogger->getAppender(_T("ELoadAppender"));
					FileAppenderPtr fileappender = (FileAppenderPtr)appender;
					const string rLogFilePath = fileappender->getFile();
					pApp->PutLogFileToDatabase(rLogFilePath, _T("Load Summary"));
					m_wndOutput.Navigate2(rLogFilePath.c_str());

					m_wndELoadItemView.ShowAllBUSLoadSummaryCallback(NULL , NULL , NULL , NULL);
					/*
					//! compare current status with last status.
					{
						CCompareRevisionDataDoc::COMPARE_ITEM CompareItem;
						CompareItem.rProductName    = _T("Load Summary-BUS");
						CompareItem.nLeftSideRevNo  = -1;
						CompareItem.nRightSideRevNo = -1;
						const BOOL res = pApp->OpenNewDocumentIfNeed(_T("REVISION") , LPCTSTR(&CompareItem));
					}
					*/
				}
			}
		}
	}
}

/**
	@brief	calculate cable length.

	@author	BHK
*/
static UINT gfncCalculateCableLength(LPVOID pVoid)
{
	CString* pDataName = (CString*)(pVoid);
	int res = python.CallPyCalculateCableLength(pDataName);

	CSimpleAnimationDlg& anidlg = CSimpleAnimationDlg::GetInstance();
	if(anidlg.GetSafeHwnd()) anidlg.SendMessage(WM_COMMAND , (ERROR_SUCCESS == res) ? IDOK : IDCANCEL);

	return res;
}

/**
	@brief	Cable Sizing Table을 계산한다.

	@author	HumKyung

	@date	????.??.??

	@param	

	@return	
*/
void CMainFrame::OnCalcCableSize()
{
	CCableSizingStartUpDlg dlg;
	if(IDOK == dlg.DoModal()) 
	{
		CSimpleAnimationDlg& anidlg = CSimpleAnimationDlg::GetInstance();
		if(NULL == anidlg.GetSafeHwnd())
		{
			anidlg.m_pThread = AfxBeginThread(gfncCalculateCableLength , &(dlg.m_rDataName) , THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);

			if(IDOK == anidlg.DoModal())
			{
				CELOADApp* pApp = (CELOADApp*)AfxGetApp();
				if(pApp) pApp->OpenNewDocumentIfNeed(_T("CABLE SIZING TABLE") , dlg.m_rDataName);
			}
		}
	}
}

/**
	@brief	

	@author	BHK
*/
void CMainFrame::OnUpdateAdminCreateProject(CCmdUI *pCmdUI)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	pCmdUI->Enable(_T("ADMIN") == docData.GetUserID());
}

/**
	@brief	application이 종료될 때.
	data가 수정되었다면 저장할것인지를 물어보고 YES면 저장하도록 한다.

	@author	BHK
*/
void CMainFrame::OnClose()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	if(docData.IsModified())
	{
		if(IDYES == AfxMessageBox("Data is modified.\nDo you want to save data?" , MB_YESNO|MB_ICONQUESTION))
		{
			docData.Save();
		}
	}

	CMDIFrameWndEx::OnClose();
}

/*
        @brief  
        
	@author HumKyung.BAEK
**/
static UINT gfnProjectSelect(LPVOID pVoid)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	docData.ClearTableOf(_T("*"));
	docData.ClearELoadItemListOf(_T("*"));
        docData.LoadFromDataBase();

        CSimpleAnimationDlg& anidlg = CSimpleAnimationDlg::GetInstance();
        anidlg.SendMessage(WM_COMMAND , IDOK);
	return ERROR_SUCCESS;
}

/*
        @brief  DOMODAL SELECT PROJECT DIALOG BOX
        
	@author KHS
**/
void CMainFrame::OnToolsProjectselect()
{
	CSelectProjectDlg Dlg;
	if(IDOK == Dlg.DoModal())
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();

		CString rCurProjectNo = docData.GetProjectNo().c_str();
		CString rDlgProjectNo = Dlg.m_rProjectNo;
		
		if(rCurProjectNo == rDlgProjectNo) return;

		CSimpleAnimationDlg& anidlg = CSimpleAnimationDlg::GetInstance();

		docData.SetProjectNo(Dlg.m_rProjectNo.operator LPCSTR());
		docData.SetProjectName(Dlg.m_rProjectName.operator LPCSTR());
		docData.SetModifiedFlag(false);

		CELOADApp* pApp = (CELOADApp*)AfxGetApp();
		anidlg.m_pThread = AfxBeginThread(gfnProjectSelect , pApp , THREAD_PRIORITY_NORMAL,0);
		if(anidlg.m_pThread)
		{
			anidlg.DoModal();

			//! LOAD BUS TREE INIT
			m_wndELoadItemView.UpdateShowLoadBusItem();

			//! LOAD LIST INIT
			vector<CLoadItem*> LoadItemEntry;
			list<CELoadItem*> LoadItemList;
			docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
			///if(pLoadItemList)
			{
				for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
				{
					CLoadItem* pLoad = static_cast<CLoadItem*>(*itr);
					if(pLoad) LoadItemEntry.push_back(pLoad);
				}
			}
			m_wndELoadItemView.RemakeLoadListCtrl(_T("") , _T("") , LoadItemEntry);

			//! BUS TREE INIT
			m_wndELoadItemView.RemakeBusTreeCtrl();
			m_wndPropertiesBar.ShowPane(FALSE,FALSE,FALSE);
		}
	}
	else return;
}

/**
	@brief	CableData Dialog를 오픈한다.

	@author	BHK
*/
void CMainFrame::OnLibraryCableData()
{
	CCableDataDlg dlg;
	if(IDOK == dlg.DoModal())
	{
		dlg.SaveData();
	}
}

void CMainFrame::OnLibraryConduitTable()
{
	CConduitTableDlg dlg;
	if(IDOK == dlg.DoModal())
	{
		///page.SaveData();
	}
}

void CMainFrame::OnProjectVoltage()
{
	CVoltageSettingDlg dlg;
	if(IDOK == dlg.DoModal())
	{
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-04-22 오전 11:03:57	

	@param	

	@return		
*/
void CMainFrame::OnCablePowercablecreationSetting()
{
	CPowerCableCreationSettingDlg dlg;
	if(IDOK == dlg.DoModal())
	{
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-04-22 오전 11:04:26	

	@param	

	@return		
*/
void CMainFrame::OnCablePowercablecreation()
{
	CPowerCableCreationDlg dlg;
	if(IDOK == dlg.DoModal())
	{
		m_wndELoadItemView.m_wndItemClassTreeCtrl.DisplayCableItemCount();
		m_wndELoadItemView.UpdateCableListCtrl(_T("Auto Power Cable"));
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-04-22 오전 11:04:41	

	@param	

	@return		
*/
void CMainFrame::OnCablePowerCableNamingRule()
{
	CPowerCableNamingRuleDlg dlg;
	if(IDOK == dlg.DoModal())
	{
	}
}

/**
	@brief

	@author	BHK
*/
void CMainFrame::OnLibraryCableSizingSetting()
{
	CCableSizingSettingDlg dlg;
	const int res = dlg.DoModal();
}

/**
	@brief	

	@author BHK	

	@date 2009-04-06 오후 7:00:25	

	@param	

	@return		
*/
void CMainFrame::OnLoadVirtualLoadSummary()
{
	CVirtualLoadSummaryDlg dlg;
	if(IDOK == dlg.DoModal())
	{
		m_wndELoadItemView.UpdateLoadListCtrl();
	}
}
/**
	@brief	CONTROL TYPE CREATION 

	@author KHS	

	@return		
**/
void CMainFrame::OnControlTypeCreation()
{
	CWizardDlg dlg(0);
	CControlTypeCreationDlg _dlg;
	CSchematicTypeCreationDlg _dlg2;
	CControlCableCreationSettingDlg _dlg3;
	dlg.AddPage(CControlTypeCreationDlg::IDD , &_dlg , _T("C1 - Control Type Creation"));
	dlg.AddPage(CSchematicTypeCreationDlg::IDD , &_dlg2 , _T("C2 - Schematic Type Creation"));
	dlg.AddPage(CControlCableCreationSettingDlg::IDD , &_dlg3 , _T("C3 - Control Cable Setting"));
	
	dlg.DoModal();
}
/**
	@brief	SCHEMATIC TYPE CREATION DIALOG BOX

	@author KHS	

	@return		
**/
void CMainFrame::OnSchematicTypeCreation()
{
	CWizardDlg dlg(1);
	CControlTypeCreationDlg _dlg;
	CSchematicTypeCreationDlg _dlg2;
	CControlCableCreationSettingDlg _dlg3;
	dlg.AddPage(CControlTypeCreationDlg::IDD , &_dlg , _T("C1 - Control Type Creation"));
	dlg.AddPage(CSchematicTypeCreationDlg::IDD , &_dlg2 , _T("C2 - Schematic Type Creation"));
	dlg.AddPage(CControlCableCreationSettingDlg::IDD , &_dlg3 , _T("C3 - Control Cable Setting"));
	
	dlg.DoModal();
}

/**
	@brief	CONTROL CABLE CREATION SETTING

	@author KHS	

	@return		
**/
void CMainFrame::OnControlcableCreationSetting()
{
	CWizardDlg dlg(2);
	CControlTypeCreationDlg _dlg;
	CSchematicTypeCreationDlg _dlg2;
	CControlCableCreationSettingDlg _dlg3;
	dlg.AddPage(CControlTypeCreationDlg::IDD , &_dlg , _T("C1 - Control Type Creation"));
	dlg.AddPage(CSchematicTypeCreationDlg::IDD , &_dlg2 , _T("C2 - Schematic Type Creation"));
	dlg.AddPage(CControlCableCreationSettingDlg::IDD , &_dlg3 , _T("C3 - Control Cable Setting"));
	
	dlg.DoModal();
}

void CMainFrame::OnMccbSizeSetting()
{
	CMccbSizeSettingDlg dlg;
	if(IDOK == dlg.DoModal())
	{
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-04-17 오후 5:41:44	

	@param	

	@return		
*/
void CMainFrame::OnControlCableSpecCreation()
{
	CControlCableSpecCreationDlg dlg;
	if(IDOK == dlg.DoModal())
	{
	}
}

#include "AssignSchematicTypeToLoadDlg.h"

/**
	@brief	

	@author BHK	

	@date 2009-04-21 오후 12:51:40	

	@param	

	@return		
*/
void CMainFrame::OnSchematicTypeAssignToLoad()
{
	CWizardDlg dlg(0);
	CAssignSchematicTypeToLoadDlg _dlg4;
	dlg.AddPage(CAssignSchematicTypeToLoadDlg::IDD , &_dlg4 , _T("C4 - Schematic Type Assign to Load"));
	
	dlg.DoModal();
}

/**
	@brief	리본 메뉴를 초기화

	@author KHS	

	@return		
**/

void CMainFrame::InitializeRibbon(void)
{
	BOOL bNameValid;

	CString strTemp;
	bNameValid = strTemp.LoadString(IDS_RIBBON_FILE);
	ASSERT(bNameValid);

	// 창 이미지를 로드합니다.
	m_PanelImages.SetImageSize(CSize(16, 16));
	m_PanelImages.Load(IDB_BUTTONS);

	// 초기화 주 단추입니다.
	m_MainButton.SetImage(IDB_MAIN);
	m_MainButton.SetText(_T("\n"));
	m_MainButton.SetToolTipText(strTemp);

	m_wndRibbonBar.SetApplicationButton(&m_MainButton, CSize (45, 45));
	{
		//! main panel

		CMFCRibbonMainPanel* pMainPanel = m_wndRibbonBar.AddMainCategory(strTemp, IDB_FILE_SMALL, IDB_FILE_LARGE);

		bNameValid = strTemp.LoadString(IDS_RIBBON_NEW);
		ASSERT(bNameValid);
		pMainPanel->Add(new CMFCRibbonButton(ID_FILE_NEW, strTemp, 0, 0));
		bNameValid = strTemp.LoadString(IDS_RIBBON_OPEN);
		ASSERT(bNameValid);
		pMainPanel->Add(new CMFCRibbonButton(ID_FILE_OPEN, strTemp, 1, 1));
		bNameValid = strTemp.LoadString(IDS_RIBBON_SAVE);
		ASSERT(bNameValid);
		pMainPanel->Add(new CMFCRibbonButton(ID_FILE_SAVE, strTemp, 2, 2));
		bNameValid = strTemp.LoadString(IDS_RIBBON_SAVEAS);
		ASSERT(bNameValid);
		pMainPanel->Add(new CMFCRibbonButton(ID_FILE_SAVE_AS, strTemp, 3, 3));

		bNameValid = strTemp.LoadString(IDS_RIBBON_PRINT);
		ASSERT(bNameValid);
		CMFCRibbonButton* pBtnPrint = new CMFCRibbonButton(ID_FILE_PRINT, strTemp, 17, 15);
		pBtnPrint->SetKeys(_T("p"), _T("w"));
		/*bNameValid = strTemp.LoadString(IDS_RIBBON_PRINT_LABEL);
		ASSERT(bNameValid);
		pBtnPrint->AddSubItem(new CMFCRibbonLabel(strTemp));
		bNameValid = strTemp.LoadString(IDS_RIBBON_PRINT_QUICK);
		ASSERT(bNameValid);
		pBtnPrint->AddSubItem(new CMFCRibbonButton(ID_FILE_PRINT_DIRECT, strTemp, 7, 7, TRUE));
		bNameValid = strTemp.LoadString(IDS_RIBBON_PRINT_PREVIEW);
		ASSERT(bNameValid);
		pBtnPrint->AddSubItem(new CMFCRibbonButton(ID_FILE_PRINT_PREVIEW, strTemp, 8, 8, TRUE));
		bNameValid = strTemp.LoadString(IDS_RIBBON_PRINT_SETUP);
		ASSERT(bNameValid);
		pBtnPrint->AddSubItem(new CMFCRibbonButton(ID_FILE_PRINT_SETUP, strTemp, 11, 11, TRUE));*/
		pMainPanel->Add(pBtnPrint);
		pMainPanel->Add(new CMFCRibbonSeparator(TRUE));

		bNameValid = strTemp.LoadString(IDS_RIBBON_CLOSE);
		ASSERT(bNameValid);
		pMainPanel->Add(new CMFCRibbonButton(ID_FILE_CLOSE, strTemp, 9, 9));

		bNameValid = strTemp.LoadString(IDS_RIBBON_RECENT_DOCS);
		ASSERT(bNameValid);
		pMainPanel->AddRecentFilesList(strTemp);

		bNameValid = strTemp.LoadString(IDS_RIBBON_EXIT);
		ASSERT(bNameValid);
		pMainPanel->AddToBottom(new CMFCRibbonMainPanelButton(ID_APP_EXIT, strTemp, 15));
	}

	//! 기존 메뉴 바의 메뉴들을 추가한다.
	AddMenuCategory();

	// "클립보드" 패널을 만듭니다.
	/*
	bNameValid = strTemp.LoadString(IDS_RIBBON_CLIPBOARD);
	ASSERT(bNameValid);
	CMFCRibbonPanel* pPanelClipboard = pCategoryHome->AddPanel(strTemp, m_PanelImages.ExtractIcon(27));

	bNameValid = strTemp.LoadString(IDS_RIBBON_PASTE);
	ASSERT(bNameValid);
	CMFCRibbonButton* pBtnPaste = new CMFCRibbonButton(ID_EDIT_PASTE, strTemp, 0, 0);
	pPanelClipboard->Add(pBtnPaste);

	bNameValid = strTemp.LoadString(IDS_RIBBON_CUT);
	ASSERT(bNameValid);
	pPanelClipboard->Add(new CMFCRibbonButton(ID_EDIT_CUT, strTemp, 1));
	bNameValid = strTemp.LoadString(IDS_RIBBON_COPY);
	ASSERT(bNameValid);
	pPanelClipboard->Add(new CMFCRibbonButton(ID_EDIT_COPY, strTemp, 2));
	bNameValid = strTemp.LoadString(IDS_RIBBON_SELECTALL);
	ASSERT(bNameValid);
	pPanelClipboard->Add(new CMFCRibbonButton(ID_EDIT_SELECT_ALL, strTemp, -1));
	pPanelClipboard->Add(new CMFCRibbonButton(ID_EDIT_UNDO, _T("실행취소"), 10));
	*/

	// "보기" 패널을 만들고 추가합니다.
	bNameValid = strTemp.LoadString(IDS_RIBBON_VIEW);
	ASSERT(bNameValid);
	
	// 탭의 오른쪽에 요소를 추가합니다.
	bNameValid = strTemp.LoadString(IDS_RIBBON_STYLE);
	ASSERT(bNameValid);
	CMFCRibbonButton* pVisualStyleButton = new CMFCRibbonButton(-1, strTemp, -1, -1);

	pVisualStyleButton->SetMenu(IDR_THEME_MENU, TRUE /* 기본값 없음 명령입니다. */, TRUE /* 오른쪽 맞춤입니다. */);

	bNameValid = strTemp.LoadString(IDS_RIBBON_STYLE_TIP);
	ASSERT(bNameValid);
	pVisualStyleButton->SetToolTipText(strTemp);
	bNameValid = strTemp.LoadString(IDS_RIBBON_STYLE_DESC);
	ASSERT(bNameValid);
	pVisualStyleButton->SetDescription(strTemp);
	m_wndRibbonBar.AddToTabs(pVisualStyleButton);

	// 빠른 실행 도구 모음 명령을 추가합니다.
	CList<UINT, UINT> lstQATCmds;

	lstQATCmds.AddTail(ID_FILE_NEW);
	lstQATCmds.AddTail(ID_FILE_OPEN);
	lstQATCmds.AddTail(ID_FILE_SAVE);
	///lstQATCmds.AddTail(ID_EDIT_UNDO);
	lstQATCmds.AddTail(ID_FILE_PRINT);
	lstQATCmds.AddTail(ID_CALC_LOAD_SUMMARY);
	lstQATCmds.AddTail(ID_CALC_CABLE_SIZE);
	
	m_wndRibbonBar.SetQuickAccessCommands(lstQATCmds);

	m_wndRibbonBar.AddToTabs(new CMFCRibbonButton(ID_APP_ABOUT, _T("\n"), m_PanelImages.ExtractIcon (0)));
}

#define	BM_SUMMARY_ICON			14
#define	HORIZONTAL_LIST_ICON		24
#define	VERTICAL_LIST_ICON		25
#define	SETTING_ICON			26
#define	LOAD_ASSIGN_ICON		27
#define	LS_VIRTUAL_ICON			28
#define	LS_MCCB_SIZE_ICON		29
#define	MF_CAPACITY_ICON		30
#define	LV_CABLE_SIZING_TABLE_ICON	31
#define	CABLE_SPEC_ICON			32
#define	CABLE_SCHEDULE_ICON		33
#define	DRUM_SCHEDULE_ICON		34

/**
	@brief	리본 메뉴바에 CATEGORY 를 추가한다.

	@author KHS	

	@return		
**/
int CMainFrame::AddMenuCategory(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	BOOL bNameValid;
	CString strTemp;
	CMFCRibbonCategory* pCategory = NULL;
	CMFCRibbonPanel* pClipboard = NULL;
	CMFCRibbonButton *pBtn = NULL;
	
	//! PROJECT 
	{
		bNameValid = strTemp.LoadString(IDS_PROJECT);
		ASSERT(bNameValid);
		pCategory = m_wndRibbonBar.AddCategory(strTemp, IDB_FILE_SMALL, IDB_FILE_LARGE);
		pClipboard = pCategory->AddPanel(docData.GetStringOf(42).c_str() , m_PanelImages.ExtractIcon(27));

		pBtn = new CMFCRibbonButton(ID_PROJECT_INFORMATION, docData.GetStringOf(43).c_str() , -1 , HORIZONTAL_LIST_ICON, TRUE);  
		pClipboard->Add(pBtn);
		pBtn = new CMFCRibbonButton(ID_PROJECT_VOLTAGE, docData.GetStringOf(44).c_str() , -1 , VERTICAL_LIST_ICON);
		pClipboard->Add(pBtn);
	}

	//! LOAD SUMMARY
	bNameValid = strTemp.LoadString(IDS_LOADSUMMARY);
	ASSERT(bNameValid);
	pCategory = m_wndRibbonBar.AddCategory(strTemp, IDB_FILE_SMALL, IDB_FILE_LARGE);

	{
		pClipboard = pCategory->AddPanel(docData.GetStringOf(1).c_str() , m_PanelImages.ExtractIcon(27));
		pBtn = new CMFCRibbonButton(ID_EXCEL_IMPORT, docData.GetStringOf(2).c_str() , -1,15);
		pClipboard->Add(pBtn);
		pBtn = new CMFCRibbonButton(ID_EXCEL_EXPORT, docData.GetStringOf(65).c_str() , -1,15);
		pClipboard->Add(pBtn);

		pClipboard->Add(new CMFCRibbonSeparator());
		pBtn = new CMFCRibbonButton(ID_EXCEL_IMPORT_ERROR_LOG, docData.GetStringOf(62).c_str(), -1,23);
		pClipboard->Add(pBtn);
	}

	CMFCRibbonPanel* pPannel = pCategory->AddPanel(docData.GetStringOf(3).c_str() , m_PanelImages.ExtractIcon(27));
	if(pPannel)
	{
		pBtn = new CMFCRibbonButton(ID_TOOLS_PROJECT_SETTING, docData.GetStringOf(4).c_str() , -1,SETTING_ICON, TRUE);
		pPannel->Add(pBtn);
		pBtn = new CMFCRibbonButton(ID_TOOLS_ASSIGNLOADTOBUS, docData.GetStringOf(5).c_str() , -1,LOAD_ASSIGN_ICON, TRUE);
		pPannel->Add(pBtn);
		pPannel->Add(new CMFCRibbonSeparator());
		pBtn = new CMFCRibbonButton(ID_CALC_LOAD_SUMMARY, docData.GetStringOf(6).c_str() , 18,16);
		pPannel->Add(pBtn);
		pPannel->Add(new CMFCRibbonSeparator());
		pBtn = new CMFCRibbonButton(ID_LOAD_VIRTUAL_LOAD_SUMMARY, docData.GetStringOf(8).c_str() , -1,LS_VIRTUAL_ICON, TRUE);
		pPannel->Add(pBtn);

		pPannel->Add(new CMFCRibbonSeparator());
		pBtn = new CMFCRibbonButton(ID_LOAD_SUMMARY_ERROR_LOG, docData.GetStringOf(61).c_str(), -1,23);
		pPannel->Add(pBtn);
	}

	CMFCRibbonPanel* pClipboard4 = pCategory->AddPanel(docData.GetStringOf(9).c_str() , m_PanelImages.ExtractIcon(27));
	if(pClipboard4)
	{
		pBtn = new CMFCRibbonButton(ID_MCCB_SIZE_SETTING, docData.GetStringOf(10).c_str() , -1,LS_MCCB_SIZE_ICON, TRUE);
		pClipboard4->Add(pBtn);
	}

	//! POWER CABLE
	bNameValid = strTemp.LoadString(IDS_POWERCABLE);
	ASSERT(bNameValid);
	pCategory = m_wndRibbonBar.AddCategory(strTemp, IDB_FILE_SMALL, IDB_FILE_LARGE);

	pPannel = pCategory->AddPanel(docData.GetStringOf(17).c_str(), m_PanelImages.ExtractIcon(27));
	if(pPannel)
	{
		pBtn = new CMFCRibbonButton(ID_LIBRARY_CABLE_SIZING_SETTING, docData.GetStringOf(18).c_str(), -1,MF_CAPACITY_ICON);  
		pPannel->Add(pBtn);
		pBtn = new CMFCRibbonButton(ID_CALC_CABLE_SIZE, docData.GetStringOf(19).c_str(), -1, LV_CABLE_SIZING_TABLE_ICON);
		pPannel->Add(pBtn);
	}

	pPannel = pCategory->AddPanel(docData.GetStringOf(20).c_str(), m_PanelImages.ExtractIcon(27));
	if(pPannel)
	{
		pBtn = new CMFCRibbonButton(ID_CABLE_POWERCABLENAMINGRULE, docData.GetStringOf(21).c_str(), -1, HORIZONTAL_LIST_ICON);
		pPannel->Add(pBtn);
		pBtn = new CMFCRibbonButton(ID_CABLE_POWERCABLECREATION_SETTING, docData.GetStringOf(22).c_str(), -1, VERTICAL_LIST_ICON);
		pPannel->Add(pBtn);
		pPannel->Add(new CMFCRibbonSeparator());
		pBtn = new CMFCRibbonButton(ID_CABLE_POWERCABLECREATION, docData.GetStringOf(23).c_str(), -1,19);
		pPannel->Add(pBtn);
		pPannel->Add(new CMFCRibbonSeparator());
		pBtn = new CMFCRibbonButton(ID_POWER_CABLE_VERIFICATION_EXECUTION, docData.GetStringOf(25).c_str(), -1,20);
		pPannel->Add(pBtn);

		pPannel->Add(new CMFCRibbonSeparator());
		pBtn = new CMFCRibbonButton(ID_POWERCABLE_ERROR_LOG, docData.GetStringOf(59).c_str(), -1,23);
		pPannel->Add(pBtn);
	}

	//! CONTROL CABLE
	bNameValid = strTemp.LoadString(IDS_CONTROLCABLE);
	ASSERT(bNameValid);
	pCategory = m_wndRibbonBar.AddCategory(strTemp, IDB_FILE_SMALL, IDB_FILE_LARGE);

	pClipboard = pCategory->AddPanel(docData.GetStringOf(26).c_str(), m_PanelImages.ExtractIcon(27));
	pBtn = new CMFCRibbonButton(ID_CONTROL_CABLE_SPEC_CREATION, docData.GetStringOf(27).c_str(),-1,CABLE_SPEC_ICON);  
	pClipboard->Add(pBtn);

	pPannel = pCategory->AddPanel(docData.GetStringOf(28).c_str(), m_PanelImages.ExtractIcon(27));
	if(pPannel)
	{
		pBtn = new CMFCRibbonButton(ID_CONTROL_TYPE_CREATION, docData.GetStringOf(29).c_str(), -1,HORIZONTAL_LIST_ICON);
		pPannel->Add(pBtn);
		pBtn = new CMFCRibbonButton(ID_SCHEMATIC_TYPE_CREATION, docData.GetStringOf(30).c_str(), -1,VERTICAL_LIST_ICON);
		pPannel->Add(pBtn);
		pBtn = new CMFCRibbonButton(ID_CONTROLCABLE_CREATION_SETTING, docData.GetStringOf(31).c_str(), -1,SETTING_ICON);
		pPannel->Add(pBtn);
		pPannel->Add(new CMFCRibbonSeparator());
		pBtn = new CMFCRibbonButton(ID_SCHEMATIC_TYPE_ASSIGN_TO_LOAD, docData.GetStringOf(32).c_str(), -1, LOAD_ASSIGN_ICON);
		pPannel->Add(pBtn);
		pPannel->Add(new CMFCRibbonSeparator());
		pBtn = new CMFCRibbonButton(ID_CONTROL_CABLE_CREATION, docData.GetStringOf(33).c_str(), -1,18);
		pPannel->Add(pBtn);
		pBtn = new CMFCRibbonButton(ID_CONTROL_CABLE_VERIFICATION , docData.GetStringOf(64).c_str(), -1,18);
		pPannel->Add(pBtn);

		pPannel->Add(new CMFCRibbonSeparator());
		pBtn = new CMFCRibbonButton(ID_CONTROLCABLE_ERROR_LOG, docData.GetStringOf(58).c_str(), -1, 23);
		pPannel->Add(pBtn);
	}

	//! CABLE/DRUM SCHEDULE
	bNameValid = strTemp.LoadString(IDS_CONTROLCABLE);
	ASSERT(bNameValid);
	pCategory = m_wndRibbonBar.AddCategory(docData.GetStringOf(53).c_str(), IDB_FILE_SMALL, IDB_FILE_LARGE);

	pPannel = pCategory->AddPanel(docData.GetStringOf(53).c_str(), m_PanelImages.ExtractIcon(27));
	if(pPannel)
	{
		pBtn = new CMFCRibbonButton(ID_USER_CABLE_IMPORT, docData.GetStringOf(54).c_str(), -1,15);
		pPannel->Add(pBtn);
		pBtn = new CMFCRibbonButton(ID_CABLE_SCHEDULE_SETTING, docData.GetStringOf(55).c_str(), -1,CABLE_SCHEDULE_ICON);
		pPannel->Add(pBtn);
		pPannel->Add(new CMFCRibbonSeparator());
		///pBtn = new CMFCRibbonButton(ID_CABLE_SCHEDULE_CREATION, docData.GetStringOf(56).c_str(), -1, 22);
		///pPannel->Add(pBtn);
		///pPannel->Add(new CMFCRibbonSeparator());
		pBtn = new CMFCRibbonButton(ID_DRUM_SCHEDULE_CREATION, docData.GetStringOf(63).c_str(), -1,DRUM_SCHEDULE_ICON);
		pPannel->Add(pBtn);
		pPannel->Add(new CMFCRibbonSeparator());
		pBtn = new CMFCRibbonButton(ID_CABLE_BM_SUMMARY, docData.GetStringOf(57).c_str(), -1,BM_SUMMARY_ICON);
		pPannel->Add(pBtn);
		
	}

	//! CABLE ROUTE
	/* disable below codes because it doesn't work - HumKyung.Baek
	bNameValid = strTemp.LoadString(IDS_CABLE);
	ASSERT(bNameValid);
	pCategory = m_wndRibbonBar.AddCategory(strTemp, IDB_FILE_SMALL, IDB_FILE_LARGE);

	pPannel = pCategory->AddPanel(docData.GetStringOf(11).c_str() , m_PanelImages.ExtractIcon(27));
	if(pPannel)
	{
		pBtn = new CMFCRibbonButton(ID_CABLE_ROUTE_TYPE_CREATION, docData.GetStringOf(12).c_str() , -1,HORIZONTAL_LIST_ICON);
		pPannel->Add(pBtn);
		pBtn = new CMFCRibbonButton(ID_CABLE_ROUTE_GROUP_CREATION, docData.GetStringOf(46).c_str() , -1,VERTICAL_LIST_ICON);  
		pPannel->Add(pBtn);

		pPannel->Add(new CMFCRibbonSeparator());

		pBtn = new CMFCRibbonButton(ID_CABLE_ROUTE_ASSIGNTOLOAD, docData.GetStringOf(47).c_str() , -1, LOAD_ASSIGN_ICON);
		pPannel->Add(pBtn);

		pPannel->Add(new CMFCRibbonSeparator());

		pBtn = new CMFCRibbonButton(ID_CABLE_TRAY_FILLRATE_EXECUTION, docData.GetStringOf(14).c_str() , -1,21);
		pPannel->Add(pBtn);
	}
	*/

	//! Tools
	bNameValid = strTemp.LoadString(IDS_TOOLS);
	ASSERT(bNameValid);
	pCategory = m_wndRibbonBar.AddCategory(strTemp , IDB_FILE_SMALL, IDB_FILE_LARGE);

	pClipboard = pCategory->AddPanel(docData.GetStringOf(34).c_str() , m_PanelImages.ExtractIcon(27));
	pBtn = new CMFCRibbonButton(ID_TOOLS_PROJECTSELECT , docData.GetStringOf(35).c_str(), 20,-1);
	pClipboard->Add(pBtn);
	pBtn = new CMFCRibbonButton(ID_TOOLS_MAKE_EXCEL_REPORT_FORMAT , docData.GetStringOf(52).c_str(), 20,-1);
	pClipboard->Add(pBtn);
	pBtn = new CMFCRibbonButton(ID_TOOLS_TEMPLATE_MAPPING , docData.GetStringOf(60).c_str(), 20,-1);
	pClipboard->Add(pBtn);

	CMFCRibbonPanel* pClipboard2 = pCategory->AddPanel(docData.GetStringOf(36).c_str(), m_PanelImages.ExtractIcon(27));
	pBtn = new CMFCRibbonButton(ID_TOOLS_RELOAD_PYTHON, docData.GetStringOf(37).c_str(), 20,-1);
	pClipboard2->Add(pBtn);

	//! Home pannel을 tool 로 이동
	bNameValid = strTemp.LoadString(IDS_HOME);
	ASSERT(bNameValid);
	CMFCRibbonPanel* pPanelView = pCategory->AddPanel(strTemp, m_PanelImages.ExtractIcon (7));

	bNameValid = strTemp.LoadString(IDS_RIBBON_STATUSBAR);
	ASSERT(bNameValid);
	CMFCRibbonButton* pBtnStatusBar = new CMFCRibbonCheckBox(ID_VIEW_STATUS_BAR, strTemp);
	pPanelView->Add(pBtnStatusBar);
	CMFCRibbonButton* pBtnPropertiesBar = new CMFCRibbonCheckBox(ID_VIEW_PROPERTIES_BAR, _T("속성 표시창"));
	pPanelView->Add(pBtnPropertiesBar);
	CMFCRibbonButton* pBtItemBar = new CMFCRibbonCheckBox(ID_VIEW_ITEM_BAR, _T("Equipment Type 표시창"));
	pPanelView->Add(pBtItemBar);
	pBtItemBar = new CMFCRibbonCheckBox(ID_VIEW_OUTPUT_BAR, _T("Output 표시창"));
	pPanelView->Add(pBtItemBar);



	//! Library
	bNameValid = strTemp.LoadString(IDS_LIBRARY);
	ASSERT(bNameValid);
	pCategory = m_wndRibbonBar.AddCategory(strTemp, IDB_FILE_SMALL, IDB_FILE_SMALL);
	pClipboard = pCategory->AddPanel(docData.GetStringOf(38).c_str(), m_PanelImages.ExtractIcon(27));

	pBtn = new CMFCRibbonButton(ID_LIBRARY_CABLE_DATA, docData.GetStringOf(39).c_str(), 20,-1);
	pClipboard->Add(pBtn);
	pBtn = new CMFCRibbonButton(ID_LIBRARY_CONDUIT_TABLE, docData.GetStringOf(40).c_str(),20,-1);
	pClipboard->Add(pBtn);

	//! Admin
	bNameValid = strTemp.LoadString(IDS_ADMIN);
	ASSERT(bNameValid);
	pCategory = m_wndRibbonBar.AddCategory(strTemp, IDB_FILE_SMALL, IDB_FILE_SMALL);
	pClipboard = pCategory->AddPanel(docData.GetStringOf(41).c_str(), m_PanelImages.ExtractIcon(27));
	
	bNameValid = strTemp.LoadString(IDS_NEW_PROJECT);
	ASSERT(bNameValid);
	pBtn = new CMFCRibbonButton(ID_ADMIN_CREATE_PROJECT, strTemp, 20,-1);
	pClipboard->Add(pBtn);

	return ERROR_SUCCESS;
}

void CMainFrame::OnFileNew()
{
	// TODO: Add your command handler code here
	OnAdminCreateProject();
}

void CMainFrame::OnCableRouteTypeCreation()
{
	// TODO: Add your command handler code here
	CWizardDlg dlg(0);
	CCableRouteTypeCreationDlg _dlg;
	CCableRouteGroupCreationDlg _dlg2;
	
	dlg.AddPage(CCableRouteTypeCreationDlg::IDD, &_dlg, "R1 - Route Type Creation");
	dlg.AddPage(CCableRouteGroupCreationDlg::IDD, &_dlg2, "R2 - Route Group Creation");

	dlg.DoModal();
}

void CMainFrame::OnCableRouteGroupCreation()
{
	CWizardDlg dlg(1);
	CCableRouteTypeCreationDlg _dlg;
	CCableRouteGroupCreationDlg _dlg2;
	
	dlg.AddPage(CCableRouteTypeCreationDlg::IDD, &_dlg, "R1 - Route Type Creation");
	dlg.AddPage(CCableRouteGroupCreationDlg::IDD, &_dlg2, "R2 - Route Group Creation");

	dlg.DoModal();
}

void CMainFrame::OnCableRouteAssigntoload()
{
	CCableRouteAssignTOLoadDlg dlg;
	if(IDOK == dlg.DoModal())
	{
	}
}

void CMainFrame::OnCableTrayFillrateExecution()
{
	CCableTrayFillRateCalculationDlg dlg;
	if(IDOK == dlg.DoModal())
	{
	}
}

#include "ControlCableCreationDlg.h"
#include "commands/CommandControlCableCreation.h"
/**
	@brief	CREATE CONTROL CABLE

	@author KHS	

	@return		
**/
void CMainFrame::OnControlCableCreation()
{
	CControlCableCreationDlg dlg;
	if((IDOK == dlg.DoModal()))
	{
		UpdateData(TRUE);
		if(!dlg.m_rDCSDesignLength.IsEmpty())
		{
			ELOAD_LOG4CXX_RESET(mylogger, _T("Auto Control Cable Error Messages"));
			COMMAND::CCommandControlCableCreation cmdControlCableCreation;
			cmdControlCableCreation.Execute();

			CELoadDocData& docData = CELoadDocData::GetInstance();
			//! modified flag를 setting한다.
			docData.SetModifiedFlag(true);

			CString rMsg;
			const string rResultString = cmdControlCableCreation.GetResultString();
			if(docData.m_nErrorCount > 0)
				rMsg.Format(_T("%s%d error(s)") , rResultString.c_str() , docData.m_nErrorCount);
			else	rMsg.Format(_T("%s") , rResultString.c_str());
			CDataResultDlg dlg(rMsg);
			dlg.DoModal();

			m_wndELoadItemView.UpdateCableListCtrl(_T("Auto Control Cable"));
			m_wndELoadItemView.m_wndItemClassTreeCtrl.DisplayCableItemCount();

			//! CONTROL CABLE RESULT VIEW
			CELOADApp* pApp = (CELOADApp*)AfxGetApp();
			if(pApp)
			{
				const string rProjectPath = docData.GetProjectFolderPath();
				CString rInterestingFilePath = rProjectPath.c_str() + CString("Control Cable Result\\");
				CFileTools::DeleteFiles(rInterestingFilePath , _T("*.*"));

				pApp->OpenNewDocumentIfNeed(_T("CONTROL CABLE") , _T("CREATE CONTROL CABLE"));
			}

			//! LOG 출력
			AppenderPtr appender = mylogger->getAppender("ELoadAppender");
			FileAppenderPtr fileappender = (FileAppenderPtr)appender;
			const string rLogFilePath = fileappender->getFile();
			pApp->PutLogFileToDatabase(rLogFilePath, "Auto Control Cable");

			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			if(pFrame) pFrame->m_wndOutput.Navigate2(rLogFilePath.c_str());
		}else	AfxMessageBox(_T("DCS IO TYPE의 DESIGN LENGTH 값을 입력하세요..."));
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-05-15 오후 11:39:31	

	@param	

	@return		
**/
void CMainFrame::OnCableBmSummary()
{
	CCableBMSummaryDlg dlg;
	if(IDOK == dlg.DoModal())
	{
	}
}

/**
	@brief	

	@author KHS	

	@date 2009-06-01 오후 12:58:12	

	@param	

	@return		
**/
int CMainFrame::DisplayArea(void)
{
	m_wndELoadItemView.m_wndItemClassTreeCtrl.DisplayArea();
	return ERROR_SUCCESS;
}

/**
	@brief	PowerCable Verification.

	@author BHK	

	@date 2009-06-15 오후 11:54:51	

	@param	

	@return		
**/
void CMainFrame::OnPowerCableVerificationExecution()
{
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp) pApp->OpenNewDocumentIfNeed(_T("POWER CABLE VERIFICATION") , NULL);
}

/**
	@brief	

	@author BHK	

	@date 2009-06-25 오후 2:21:10	

	@param	

	@return		
*/
void CMainFrame::OnProjectInformation()
{
	CProjectInfoDlg dlg;
	dlg.DoModal();
}

/**
	@brief	MAKE EXCEL REPORT FORMAT 

	@author KHS	

	@date 2009-07-01 오후 1:46:49	

	@param	

	@return		
**/
void CMainFrame::OnToolsMakeExcelReportFormat()
{
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp) pApp->OpenNewDocumentIfNeed(_T("EXCEL REPORT FORMAT") , _T("EXCEL REPORT FORMAT"));
}

/**
	@brief	

	@author BHK	

	@date 2009-07-30 오전 10:03:03	

	@param	

	@return		
*/
void CMainFrame::OnToolsTemplateMapping()
{
	CTemplateMappingDlg dlg;
	dlg.DoModal();
}

/**
	@brief	Cable BM Summary Dialog를 표시합니다.

	@author BHK	

	@date 2009-07-01 오후 5:18:57	

	@param	

	@return		
*/
void CMainFrame::OnCableBMSummary()
{
	CCableBMSummaryDlg dlg;
	if(IDOK == dlg.DoModal())
	{
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-07-02 오전 11:40:23	

	@param	

	@return		
*/
void CMainFrame::OnUserCableImport()
{
	CMainFrame::m_wndELoadItemView.OnUserCableImport();
}

/**
	@brief	Cable Schedule Setting Dialog를 Open한다.

	@author BHK	

	@date 2009-07-02 오후 12:53:56	

	@param	

	@return		
*/
void CMainFrame::OnCableScheduleSetting()
{
	CCableScheduleSettingDlg dlg;
	if(IDOK == dlg.DoModal())
	{
	}
}

/**
	@brief	Cable Schedule을 생성합니다.

	@author BHK	

	@date 2009-09-03 오전 11:45:33	

	@param	

	@return		
*/
void CMainFrame::OnCableScheduleCreation()
{
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp) pApp->OpenNewDocumentIfNeed(_T("Cable Schedule") , _T("Create Cable Schedule"));
}

/**
	@brief	Drum Schedule을 생성합니다.

	@author BHK	

	@date 2009-09-03 오후 7:12:51	

	@param	

	@return		
*/
void CMainFrame::OnDrumScheduleCreation()
{
	CDrumScheduleCreationDlg dlg;
	dlg.DoModal();
}

/**
	@brief	Power Cable Log를 보여준다.

	@author KHS	

	@date 2009-07-21 오전 11:38:51	

	@param	

	@return		
**/
void CMainFrame::OnPowercableErrorLog()
{
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp)
	{
		BeginWaitCursor();

		AppenderPtr appender = mylogger->getAppender("ELoadAppender");
		FileAppenderPtr fileappender = (FileAppenderPtr)appender;
		const string rLogFilePath = fileappender->getFile();

		pApp->GetLogFileToDatabase( rLogFilePath, "Auto Power Cable");

		EndWaitCursor();
	}
}

/**
	@brief	Control Cable Log를 보여준다.

	@author KHS	

	@date 2009-07-21 오전 11:43:24	

	@param	

	@return		
**/
void CMainFrame::OnControlcableErrorLog()
{
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp)
	{
		BeginWaitCursor();

		AppenderPtr appender = mylogger->getAppender("ELoadAppender");
		FileAppenderPtr fileappender = (FileAppenderPtr)appender;
		const string rLogFilePath = fileappender->getFile();

		pApp->GetLogFileToDatabase( rLogFilePath, "Auto Control Cable");

		EndWaitCursor();
	}
}

/**
	@brief	Load Summary Error Log를 보여준다.

	@author BHK	

	@date 2009-08-26 오후 4:14:28	

	@param	

	@return		
*/
void CMainFrame::OnLoadSummaryErrorLog()
{
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp)
	{
		BeginWaitCursor();

		AppenderPtr appender = mylogger->getAppender(_T("ELoadAppender"));
		FileAppenderPtr fileappender = (FileAppenderPtr)appender;
		const string rLogFilePath = fileappender->getFile();

		pApp->GetLogFileToDatabase( rLogFilePath, _T("Load Summary"));

		EndWaitCursor();
	}
}

/**
	@brief	Excel Import Error Log를 보여준다.

	@author BHK	

	@date 2009-08-27 오후 1:59:03

	@param	

	@return		
*/
void CMainFrame::OnExcelImportErrorLog()
{
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp)
	{
		BeginWaitCursor();

		AppenderPtr appender = mylogger->getAppender(_T("ELoadAppender"));
		FileAppenderPtr fileappender = (FileAppenderPtr)appender;
		const string rLogFilePath = fileappender->getFile();

		pApp->GetLogFileToDatabase( rLogFilePath, _T("Excel Import"));

		EndWaitCursor();
	}
}

/**
	@brief	open pdf manual

	@author HumKyung

	@date 2010-12-14

	@param	

	@return		
*/
void CMainFrame::OnAcceleratorHelp()
{
	CString sManualFilePath = GetExecPath() + _T("Manual\\eLOAD_메뉴얼.pdf");

	::ShellExecute(NULL , _T("OPEN") , sManualFilePath , NULL , NULL , SW_SHOWNORMAL);
}
