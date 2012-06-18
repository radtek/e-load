
// MainFrm.h : CMainFrame 클래스의 인터페이스
//

#pragma once

#define POSITION_X      400
#define POSITION_Y      100

#include "OutputWnd.h"
#include "PropertiesWnd.h"
#include "ELoadItemViewBar.h"
#include "ELoadMenuBar.h"

class MenuCommandSet;
class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// 특성입니다.
public:
        
// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	///int OnClickTreeItem(const CString& rTreeText);

	CMDIClientAreaWnd& GetClientAreaWnd()
	{
		return m_wndClientArea;
	}
// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	CPropertiesWnd    m_wndPropertiesBar;
	COutputWnd        m_wndOutput;
protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CMFCStatusBar     m_wndStatusBar;

	CMFCRibbonBar			m_wndRibbonBar;	// Ribbon bar for the application
	CMFCRibbonApplicationButton	m_MainButton;	// The application button for the ribbon
	CMFCToolBarImages		m_PanelImages;	// Ribbon Panel Icons
	
	CELoadItemViewBar		m_wndELoadItemView;

	CString				m_sExcelFilePath;
// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnToolsAssignloadtobus();
	afx_msg void OnAdminCreateProject();
	afx_msg void OnViewPropertiesBar();
	afx_msg void OnUpdateViewPropertiesBar(CCmdUI *pCmdUI);
	afx_msg void OnViewItemBar();
	afx_msg void OnViewOutputBar();
	afx_msg void OnUpdateViewOutputBar(CCmdUI *pCmdUI);
	afx_msg void OnViewToolbar();
	afx_msg void OnUpdateViewToolbar(CCmdUI *pCmdUI);
	afx_msg void OnToolsProjectSetting();
	afx_msg void OnToolsReloadPython();
	afx_msg void OnExcelImport();
	afx_msg void OnExcelExport();
	afx_msg void OnCalcLoadSummary();
	afx_msg void OnCalcCableSize();
	afx_msg void OnUpdateLoadItemBar(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);

	UINT StatusExportThread();
	static UINT StatusExportThreadEntry(LPVOID pVoid);
public:
	afx_msg void OnUpdateAdminCreateProject(CCmdUI *pCmdUI);
        afx_msg void OnToolsProjectselect();
	afx_msg void OnLibraryCableData();
	afx_msg void OnLibraryConduitTable();
        afx_msg void OnProjectVoltage();
	afx_msg void OnCablePowercablecreation();
	afx_msg void OnCablePowercablecreationSetting();
	afx_msg void OnCablePowerCableNamingRule();
	afx_msg void OnLibraryCableSizingSetting();
	afx_msg void OnLoadVirtualLoadSummary();
	afx_msg void OnControlTypeCreation();
	afx_msg void OnSchematicTypeCreation();
	afx_msg void OnControlcableCreationSetting();
	afx_msg void OnMccbSizeSetting();
	afx_msg void OnControlCableSpecCreation();
	afx_msg void OnSchematicTypeAssignToLoad();
	void InitializeRibbon(void);
	int AddMenuCategory(void);
	afx_msg void OnFileNew();
	afx_msg void OnCableRouteTypeCreation();
	afx_msg void OnCableRouteGroupCreation();
	afx_msg void OnCableRouteAssigntoload();
	afx_msg void OnCableTrayFillrateExecution();


	afx_msg void OnControlCableCreation();
	afx_msg void OnCableBmSummary();
	int DisplayArea(void);
	afx_msg void OnPowerCableVerificationExecution();

	afx_msg void OnProjectInformation();
	afx_msg void OnToolsMakeExcelReportFormat();
	afx_msg void OnToolsTemplateMapping();
	afx_msg void OnCableBMSummary();
	afx_msg void OnCableScheduleSetting();
	afx_msg void OnCableScheduleCreation();
	afx_msg void OnDrumScheduleCreation();
	afx_msg void OnUserCableImport();
	afx_msg void OnPowercableErrorLog();
	afx_msg void OnControlcableErrorLog();
	afx_msg void OnLoadSummaryErrorLog();
	afx_msg void OnExcelImportErrorLog();
	afx_msg void OnAcceleratorHelp();
};

//! 다른 쓰레드에서는 Afxxxx 계열의 함수가 제대로 작동을 하지 않으므로...
extern CMainFrame* ELOADGetMainWnd();

