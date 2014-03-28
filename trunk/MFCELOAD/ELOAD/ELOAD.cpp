
// ELOAD.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "ELOAD.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "CableSizingTableChildFrm.h"

#include "ELOADDoc.h"
#include "ELOADView.h"
#include <log4cxx/htmllayout.h>
#include <Zip\inc\ZipArchive.h>
#include "ELoadDocData.h"
#include "BusDoc.h"
#include "BusView.h"
#include "BusDiagramChildFrame.h"
#include "LoadSummaryDoc.h"
#include "LoadSummaryView.h"
#include "LoginDlg.h"
#include "SimpleProgressBar.h"
#include "CableSizingTableView.h"
#include "CableSizingTableDoc.h"
#include "CableSizingResultView.h"
#include "TransformerSizingResultDoc.h"
#include "TransformerSizingResultView.h"
#include "CompareRevisionDataDoc.h"
#include "CompareRevisionDataView.h"
#include "CableRouteCableListDoc.h"
#include "CableRouteCableListView.h"
#include "ControlCableResultDoc.h"
#include "ControlCableResultView.h"
#include "PowerCableVerificationDoc.h"
#include "PowerCableVerificationView.h"
#include "UserCableResultDoc.h"
#include "UserCableResultView.h"
#include "CableScheduleResultDoc.h"
#include "CableScheduleResultView.h"
#include "DrumScheduleResultDoc.h"
#include "DrumScheduleResultView.h"
#include "LoadListSldDoc.h"
#include "LoadListSldView.h"
//! 2010.12.23 - added by HumKyung
#include "BusToBusDoc.h"
#include "BusToBusView.h"

#include "ExcelReportFormatDoc.h"
#include "ExcelReportFormatView.h"

#include "CableSizingResultDoc.h"
#include "commands/CommandUndo.h"
#include "commands/CommandAutoCalculateStartingAmp.h"
#include "ELoadLayout.h"
#include "Splash/SplashScreenFx.h"
#include "VersionNo.h"

#include "pyfromc.h"
#include <util/Registry.h>
#include <locale>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef	int (* AutoUpFunc)(const char*);
typedef int (* CheckForUpdateFunc)(const char*);
// CELoadApp

const int RETURN_ERROR		= -1;
const int RETURN_OK		= 0;
const int RETURN_NEWVERSION	= 1;
const int RETURN_NONEWVERSION	= 2;
const int RETURN_SECURITYERROR	= 3;

// CELOADApp

BEGIN_MESSAGE_MAP(CELOADApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CELOADApp::OnAppAbout)
	// 표준 파일을 기초로 하는 문서 명령입니다.
	ON_COMMAND(ID_FILE_NEW, &CELOADApp::OnFileNew)
	ON_COMMAND(ID_FILE_SAVE, CELOADApp::OnSaveDataBase)
	ON_COMMAND(ID_EDIT_UNDO, &CELOADApp::OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CELOADApp::OnUpdateEditUndo)
        ON_COMMAND(ID_FILE_OPEN, &CELOADApp::OnFileOpen)
END_MESSAGE_MAP()

// CELOADApp 생성

CELOADApp::CELOADApp()
{
	m_bHiColorIcons = TRUE;

	//! ifstream으로 파일명이 한글로된 파일을 읽지 못하는 문제가 생긴다. 따라서 아래의 코드를 추가했음.
	setlocale(LC_ALL , _T("Korean"));
}

// 유일한 CELOADApp 개체입니다.

CELOADApp theApp;
log4cxx::LoggerPtr mylogger;

extern "C" __declspec(dllimport) int CheckForUpdate(const char* rIniFilePath);
extern "C" __declspec(dllimport) int AutoUp(const char* pszIniFilePath);

// CELOADApp 초기화
BOOL CELOADApp::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다. 
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// OLE 라이브러리를 초기화합니다.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	/// terminate if can't connect internet.
	/// 보안과 버젼을 확인하여 새로운 버젼이 나왔으면 다운로드하여 설치하도록 한다.
#ifdef	NDEBUG
#ifndef	NOCHECK
	const int res = AutoUpdate();
	if((RETURN_ERROR == res) || (RETURN_SECURITYERROR == res)) return FALSE;
#endif
#endif

	CLoginDlg dlg;
	if(IDOK == dlg.DoModal())
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		docData.SetUserID( dlg.m_rID.operator LPCSTR() );
		docData.SetProjectNo(dlg.m_rProjectNo.operator LPCSTR());
		docData.SetProjectName(dlg.GetProjectNameOf(dlg.m_rProjectNo).operator LPCSTR());
		docData.SetProjectDesc(dlg.GetProjectDescOf(dlg.m_rProjectNo).operator LPCSTR());
		docData.SetProjectClient(dlg.GetProjectClientOf(dlg.m_rProjectNo).operator LPCSTR());

		CString rServerFolderPath = dlg.GetServerFolderPath();
		docData.SetServerFolderPath( rServerFolderPath.operator LPCSTR() );
	}else	return FALSE;

	// SplashScreen
	CSplashScreenFx *pSplash = new CSplashScreenFx();
	if(pSplash)
	{
		pSplash->Create(GetDesktopWindow() ,CString(CString(STRPRODUCTVER) + _T(" Initializing...")),0,CSS_FADE | CSS_CENTERSCREEN | CSS_SHADOW);
		pSplash->SetBitmap(IDB_LOGO,12,41,80);
		pSplash->SetTextFont("Arial",80,CSS_TEXT_NORMAL);
		pSplash->SetTextRect(CRect(195,240,390,280));
		pSplash->CreateProgressBar(CRect(5,273,395,280), CSize(0, 9), PBS_SMOOTH);
		pSplash->SetTextColor(RGB(0,0,0));
		pSplash->SetTextFormat(DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
		pSplash->Show();	
		pSplash->SetActiveWindow();
	}

	python.startup();

	log4cxx::LayoutPtr layout(new CELoadLayout());

	// Create a new appender with the created layout
	// Here we create a 'appender' to output to a FILE, instead of the 'console'.
	// Here it outputs the logs to a file 'log.txt'
	TCHAR szBuf[MAX_PATH + 1] = {'\0',};
	GetModuleFileName(NULL , szBuf , MAX_PATH);
	string rExecFolder(szBuf);
	const string::size_type at = rExecFolder.rfind("\\");
	if(string::npos != at) rExecFolder = rExecFolder.substr(0 , at);
	if('\\' != rExecFolder[rExecFolder.length() - 1]) rExecFolder = rExecFolder + "\\";

	const CString rLogFilePath = string(rExecFolder + _T("temp\\log.html")).c_str();
	if(FileExist(rLogFilePath)) DeleteFile(rLogFilePath);
	log4cxx::FileAppenderPtr appender(new log4cxx::FileAppender(layout, rLogFilePath.operator LPCSTR() , true));
	appender->setName(_T("ELoadAppender"));

	// Create a new Logger with the name: 'ELoad'
	mylogger = Logger::getLogger( _T("ELoad") );

	// Set Level for the new logger created

	///mylogger->setLevel(log4cxx::Level::getInfo());

	// Add the appender to the 'logger' object created
	mylogger->addAppender(appender);
/*
	log4cxx::BasicConfigurator::configure();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
*/
	//! set timezone to ROK because of log4cxx-0.9.7 changes timezone.
	putenv( _T("TZ=ROK") );
	tzset();

	///install_crashrpt();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("ELOAD"));
	LoadStdProfileSettings(0);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.

	InitContextMenuManager();

	{
		// Load From Database
		CELoadDocData& docData = CELoadDocData::GetInstance();
		if(!docData.GetProjectMDBFilePath().empty())
			docData.LoadFromDataBase();
		else	CSplashScreenFx::GetInstance()->Hide();
	}

	InitKeyboardManager();

	GetContextMenuManager()->AddMenu (_T("Resource"), IDR_POPUP_RESOURCE);
	GetContextMenuManager()->AddMenu (_T("ResourceSub"), IDR_POPUP_RESOURCE_SUB);
	GetContextMenuManager()->AddMenu (_T("ResourceBus"), IDR_CONTEXT_BUS_MENU);
	GetContextMenuManager()->AddMenu (_T("CONTEXT_LOAD_MENU"), IDR_CONTEXT_LOAD_MENU);
	GetContextMenuManager()->AddMenu (_T("ResourceShowLoad"), IDR_CONTEXT_SHOWLOAD_MENU);
        GetContextMenuManager()->AddMenu (_T("GridCtrl"), IDR_GRID_CONTEXT_MENU);
        GetContextMenuManager()->AddMenu (_T("BUSCOPY"), IDR_BUS_COPY_MENU);
	GetContextMenuManager()->AddMenu (_T("TRANSFORMER"), IDR_CONTEXT_TRANSFORMER_MENU);
	GetContextMenuManager()->AddMenu (_T("CAPACITOR"), IDR_CONTEXT_CAPACITOR_MENU);
	GetContextMenuManager()->AddMenu (_T("GENERATOR"), IDR_CONTEXT_GENERATOR_MENU);
	GetContextMenuManager()->AddMenu (_T("UPSDC"), IDR_CONTEXT_UPSDC_MENU);
	GetContextMenuManager()->AddMenu (_T("CABLE"), IDR_CONTEXT_CABLE_MENU);
	GetContextMenuManager()->AddMenu (_T("CALC_LOADSUMMARY"), IDR_CONTEXT_CALC_LOADSUMMARY_MENU);
	GetContextMenuManager()->AddMenu (_T("TR_SIZING_RESULT"), IDR_CONTEXT_TR_SIZING_RESULT_MENU);
	GetContextMenuManager()->AddMenu (_T("CALC_CABLESIZING"), IDR_CONTEXT_CALC_CABLESIZING_MENU);
	GetContextMenuManager()->AddMenu (_T("CREATE_CABLESIZING_RESULT"), IDR_CONTEXT_CREATE_CABLE_SIZING_RESULT_MENU);
	GetContextMenuManager()->AddMenu (_T("CONTROL_TYPE_CREATION"), IDR_CONTROL_TYPE_CREATION_MENU);
	GetContextMenuManager()->AddMenu (_T("BUS_DIAGRAM_OPEN"), IDR_BUS_DIAGRAM_OPEN);
	GetContextMenuManager()->AddMenu (_T("SCHEMATIC TYPE"),IDR_CONTEXT_SCHEMATIC_MENU);
	GetContextMenuManager()->AddMenu (_T("CONTROL TYPE"),IDR_CONTEXT_CONTROLTYPE_MENU);
	GetContextMenuManager()->AddMenu (_T("USER CABLE"),IDR_CONTEXT_USER_CABLE_MENU);
	GetContextMenuManager()->AddMenu (_T("CONTROL CABLE RESULT"), IDR_CONTEXT_CONTROL_CABLE_RESULT_MENU);
	//! 2010.12.27 - added by HumKyung
	GetContextMenuManager()->AddMenu (_T("BUS TO BUS RELATION"), IDR_CONTEXT_BUS_TO_BUS_MENU);

	CreateELoadImageList();
		
	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	///test_generate_report();
	// 응용 프로그램의 문서 템플릿을 등록합니다. 문서 템플릿은
	//  문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.
	AddELoadDocTemplate();

	// 주 MDI 프레임 창을 만듭니다.
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	// 접미사가 있을 경우에만 DragAcceptFiles를 호출합니다.
	//  MDI 응용 프로그램에서는 m_pMainWnd를 설정한 후 바로 이러한 호출이 발생해야 합니다.

	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// 명령줄에 지정된 명령을 디스패치합니다.
	// 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// 주 창이 초기화되었으므로 이를 표시하고 업데이트합니다.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

/**
        @brief	BusGroup이름으로 BusGroup을 찾는다.

        @author	KHS
*/
CBusGroupItem* CELOADApp::FindBusGroup(const CString& rBusGroupName)
{
	CELoadDocData& docData    = CELoadDocData::GetInstance();
	list<CELoadItem*> BusGroupList;
	docData.GetELoadItemListOf(BusGroupList , CBusGroupItem::TypeString());
	if(!BusGroupList.empty())
	{
		for(list<CELoadItem*>::iterator itr = BusGroupList.begin(); itr != BusGroupList.end();++itr)
		{
			CBusGroupItem* pBusGroup = static_cast<CBusGroupItem*>(*itr);

			if(pBusGroup->IsDeleted()) continue;
			if(rBusGroupName == pBusGroup->GetName().c_str()) return pBusGroup;
		}
	}

	return NULL;
}

/**
	@brief	open new document after close all already opened documents.

	@author	HumKyung

	@date	????.??.??

	@param	strTarget
	@param	pExtraParam

	@return	BOOL
*/
BOOL CELOADApp::OpenNewDocumentIfNeed(const CString &strTarget , LPCTSTR pExtraParam)
{
	CString strDocName;
	if(CBusItem::TypeString() == strTarget)
	{
		CDocTemplate* pSelectedTemplate = NULL;
		POSITION pos = GetFirstDocTemplatePosition();
		while (pos != NULL)
		{
			pSelectedTemplate = (CDocTemplate*) GetNextDocTemplate(pos);
			pSelectedTemplate->GetDocString(strDocName, CDocTemplate::docName);
			if (strTarget == strDocName)
			{
				CString rExecPath = GetExecPath();
				if(_T("\\") != rExecPath.Right(1)) rExecPath += _T("\\");
				CString rBusDiagramFilePath = rExecPath + _T("temp\\BusDiagram.jpg");

				POSITION docPos = pSelectedTemplate->GetFirstDocPosition();
				if(NULL == docPos)
				{
					pSelectedTemplate->OpenDocumentFile(rBusDiagramFilePath);
				}
				else
				{
					CDocument *pDoc = pSelectedTemplate->GetNextDoc(docPos);
					pDoc->OnOpenDocument(rBusDiagramFilePath);
					pDoc->UpdateAllViews(NULL);
				}
			}
			else	//! 이외의 TEMPLATE는 닫는다.
			{
				POSITION docPos = pSelectedTemplate->GetFirstDocPosition();
				if(docPos != NULL)
				{
					pSelectedTemplate->CloseAllDocuments(TRUE);
				}
			}
		}
	}
	else if(CLoadItem::TypeString() == strTarget)
	{
		CDocTemplate* pSelectedTemplate;
		POSITION pos = GetFirstDocTemplatePosition();
		while (pos != NULL)
		{
			pSelectedTemplate = (CDocTemplate*) GetNextDocTemplate(pos);
			pSelectedTemplate->GetDocString(strDocName, CDocTemplate::docName);

			if (strDocName == strTarget)
			{
				POSITION docPos = pSelectedTemplate->GetFirstDocPosition();
				if(docPos == NULL)
				{
					pSelectedTemplate->OpenDocumentFile(pExtraParam);
				}
				else
				{
					CELOADDoc *pDoc = (CELOADDoc*)pSelectedTemplate->GetNextDoc(docPos);
					if(pDoc)
					{
						pDoc->OnNewDocument();//pExtraParam);
						pDoc->SetTitle(pExtraParam);
						pDoc->UpdateAllViews(NULL);
					}
				}
			}
			//! 이외의 TEMPLATE는 닫는다.
			else
			{
				POSITION docPos = pSelectedTemplate->GetFirstDocPosition();
				if(docPos != NULL)
				{
					pSelectedTemplate->CloseAllDocuments(TRUE);
				}
			}
		}
		return TRUE;
	}
	else if(_T("LOAD SUMMARY") == strTarget)
	{
		CDocTemplate* pSelectedTemplate = NULL;
		POSITION pos = GetFirstDocTemplatePosition();
		while (NULL != pos) 
		{
			pSelectedTemplate = (CDocTemplate*) GetNextDocTemplate(pos);
			pSelectedTemplate->GetDocString(strDocName, CDocTemplate::docName);
			if (strTarget == strDocName)
			{
				POSITION docPos = pSelectedTemplate->GetFirstDocPosition();
				if(NULL == docPos)
				{
					pSelectedTemplate->OpenDocumentFile(pExtraParam);
				}
				else
				{
					CLoadSummaryDoc *pDoc = (CLoadSummaryDoc*)pSelectedTemplate->GetNextDoc(docPos);
					if(pDoc)
					{
						pDoc->OnOpenDocument(pExtraParam);
						pDoc->SetTitle(pExtraParam);
						pDoc->UpdateAllViews(NULL);
					}
				}
			}
			else	//! 이외의 TEMPLATE는 닫는다.
			{
				POSITION docPos = pSelectedTemplate->GetFirstDocPosition();
				if(docPos != NULL)
				{
					pSelectedTemplate->CloseAllDocuments(TRUE);
				}
			}
		}
	}
	else if(_T("CABLE SIZING TABLE") == strTarget)
	{
		CDocTemplate* pSelectedTemplate = NULL;
		POSITION pos = GetFirstDocTemplatePosition();
		while (pos != NULL) 
		{
			pSelectedTemplate = (CDocTemplate*) GetNextDocTemplate(pos);
			pSelectedTemplate->GetDocString(strDocName, CDocTemplate::docName);
			if (strTarget == strDocName)
			{
				POSITION docPos = pSelectedTemplate->GetFirstDocPosition();
				if(NULL == docPos)
				{
					pSelectedTemplate->OpenDocumentFile(pExtraParam);
				}
				else
				{
					CCableSizingTableDoc* pDoc = (CCableSizingTableDoc*)pSelectedTemplate->GetNextDoc(docPos);
					if(pDoc)
					{
						pDoc->OnOpenDocument(pExtraParam);
						pDoc->SetTitle(pExtraParam);
						pDoc->UpdateAllViews(NULL);
					}

				}

			}
			else	//! 이외의 TEMPLATE는 닫는다.
			{
				POSITION docPos = pSelectedTemplate->GetFirstDocPosition();
				if(docPos != NULL)
				{
					pSelectedTemplate->CloseAllDocuments(TRUE);
				}
			}
		}
	}
	else if((_T("Auto Power Cable") == strTarget) || ("TRANSFORMER SIZING" == strTarget) || (_T("REVISION") == strTarget) || (_T("ROUTE CABLE LIST") == strTarget)|| (_T("CONTROL CABLE") == strTarget) || (_T("ROUTE CABLE LIST") == strTarget) || (_T("USER CABLE") == strTarget) || (_T("EXCEL REPORT FORMAT") == strTarget) || (_T("LOAD LIST SLD") == strTarget))
	{
		CDocTemplate* pSelectedTemplate = NULL;
		POSITION pos = GetFirstDocTemplatePosition();
		while (pos != NULL) 
		{
			pSelectedTemplate = (CDocTemplate*) GetNextDocTemplate(pos);
			pSelectedTemplate->GetDocString(strDocName, CDocTemplate::docName);
			if (strTarget == strDocName)
			{
				POSITION docPos = pSelectedTemplate->GetFirstDocPosition();
				if(NULL == docPos)
				{
					pSelectedTemplate->OpenDocumentFile(pExtraParam);
				}
				else
				{
					CELOADDoc* pDoc = (CELOADDoc*)pSelectedTemplate->GetNextDoc(docPos);
					if(pDoc)
					{
						pDoc->OnOpenDocument(pExtraParam);
						pDoc->SetTitle(pExtraParam);
						pDoc->UpdateAllViews(NULL);
					}
				}
			}
			else	//! 이외의 TEMPLATE는 닫는다.
			{
				POSITION docPos = pSelectedTemplate->GetFirstDocPosition();
				if(docPos != NULL)
				{
					pSelectedTemplate->CloseAllDocuments(TRUE);
				}
			}
		}
	}
	else if(_T("POWER CABLE VERIFICATION") == strTarget)
	{
		CDocTemplate* pSelectedTemplate = NULL;
		POSITION pos = GetFirstDocTemplatePosition();
		while (pos != NULL) 
		{
			pSelectedTemplate = (CDocTemplate*) GetNextDocTemplate(pos);
			pSelectedTemplate->GetDocString(strDocName, CDocTemplate::docName);
			if (strTarget == strDocName)
			{
				POSITION docPos = pSelectedTemplate->GetFirstDocPosition();
				if(NULL == docPos)
				{
					pSelectedTemplate->OpenDocumentFile(pExtraParam);
				}
				else
				{
					CELOADDoc* pDoc = (CELOADDoc*)pSelectedTemplate->GetNextDoc(docPos);
					if(pDoc)
					{
						pDoc->OnOpenDocument(pExtraParam);
						pDoc->SetTitle(pExtraParam);
						pDoc->UpdateAllViews(NULL);
					}
				}
			}
			else	//! 이외의 TEMPLATE는 닫는다.
			{
				POSITION docPos = pSelectedTemplate->GetFirstDocPosition();
				if(docPos != NULL)
				{
					pSelectedTemplate->CloseAllDocuments(TRUE);
				}
			}
		}
	}
	else
	{
		CDocTemplate* pSelectedTemplate = NULL;
		POSITION pos = GetFirstDocTemplatePosition();
		while (pos != NULL) 
		{
			pSelectedTemplate = (CDocTemplate*) GetNextDocTemplate(pos);
			pSelectedTemplate->GetDocString(strDocName, CDocTemplate::docName);
			if (strTarget == strDocName)
			{
				POSITION docPos = pSelectedTemplate->GetFirstDocPosition();
				if(NULL == docPos)
				{
					pSelectedTemplate->OpenDocumentFile(pExtraParam);
				}
				else
				{
					CELOADDoc* pDoc = (CELOADDoc*)pSelectedTemplate->GetNextDoc(docPos);
					if(pDoc)
					{
						pDoc->OnOpenDocument(pExtraParam);
						pDoc->SetTitle(pExtraParam);
						pDoc->UpdateAllViews(NULL);
					}
				}
			}
			else	//! 이외의 TEMPLATE는 닫는다.
			{
				POSITION docPos = pSelectedTemplate->GetFirstDocPosition();
				if(docPos != NULL)
				{
					pSelectedTemplate->CloseAllDocuments(TRUE);
				}
			}
		}
	}

	return FALSE;
}


/**
        @brief 
        @author	KHS
*/
int CELOADApp::OpenQuery(CADODB* pAdoDb, const STRING_T& rQuery)
{
	try
	{
		pAdoDb->OpenQuery(rQuery);
	}
	catch(_com_error & e)
	{
		CString rMsg;
		rMsg.Format("*** DOG CHECK [ADO DB] => <ExecuteQuery 오류> 설명[%s] !! ***\n", (LPCSTR)e.Description());
		AfxMessageBox(rMsg);
		return ERROR_BAD_ENVIRONMENT;
	}
	return ERROR_SUCCESS;
}
/**
        @brief 
        @author	KHS
*/
int CELOADApp::ExecuteQuery(CADODB* pAdoDb, const STRING_T& rQuery)
{
	try
	{
		pAdoDb->ExecuteQuery(rQuery);
	}
	catch(_com_error & e)
	{
		CString rMsg;
		rMsg.Format("*** DOG CHECK [ADO DB] => <ExecuteQuery 오류> 설명[%s] !! ***\n", (LPCSTR)e.Description());
		AfxMessageBox(rMsg);
		return ERROR_BAD_ENVIRONMENT;
	}
	return ERROR_SUCCESS;
}

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CELOADApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CELOADApp 사용자 지정 로드/저장 메서드

void CELOADApp::PreLoadState()
{
	CString strName;
///	bNameValid = strName.LoadString(IDS_EDIT_MENU);
///	ASSERT(bNameValid);
///	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
///	bNameValid = strName.LoadString(IDS_EXPLORER);
//	ASSERT(bNameValid);
///	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CELOADApp::LoadCustomState()
{
}

void CELOADApp::SaveCustomState()
{
}

// CELOADApp 메시지 처리기

/**
        @brief	Save DataBase
	
	@author	KHS
*/
void CELOADApp::OnSaveDataBase()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	docData.Save();
	
	//! 저장 후 prop TEXT 상태 초기화
	vector<string> emptyEntry;
	UpdateAllViews(emptyEntry);

	MessageBox(NULL,"SUCCESS !!", "SAVE", MB_ICONINFORMATION);
}


/**
	@brief	DocTypeEntry에 들어있는 DocType을 가진 모든 뷰들을 갱신한다.만일 DocTypeEntry이 비어 있으면 모든 뷰들을 갱신한다.

	@author	BHK
*/
int CELOADApp::UpdateAllViews(const vector<string>& DocTypeEntry)
{
	CString strDocName;
	CDocTemplate* pSelectedTemplate;
	POSITION pos = GetFirstDocTemplatePosition();
	while (pos != NULL) 
	{
		CString strDocName;
		pSelectedTemplate = (CDocTemplate*) GetNextDocTemplate(pos);
		pSelectedTemplate->GetDocString(strDocName, CDocTemplate::docName);
		
		vector<string>::const_iterator where = find(DocTypeEntry.begin(),DocTypeEntry.end(),strDocName.operator LPCSTR());

		if((DocTypeEntry.empty()) || (where != DocTypeEntry.end()))
		{
			POSITION docPos = pSelectedTemplate->GetFirstDocPosition();
			while(docPos != NULL)
			{
				CDocument* pDoc = (pSelectedTemplate->GetNextDoc(docPos));
				
				pDoc->SetModifiedFlag();
				pDoc->UpdateAllViews(NULL);
				pDoc->SetModifiedFlag(FALSE);
			}
		}
	}

        CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
        if(pFrame) pFrame->m_wndPropertiesBar.CommitModifiedValues();

	return FALSE;
}

void CELOADApp::OnFileNew()
{
        // TODO: Add your command handler code here
}

/**
        @brief  Document를 닫는다.

        @author KHS
*/
void CELOADApp::CloseDocument(const CString& rDocName)
{
	//! 뷰를 닫는다
	CDocTemplate* pSelectedTemplate;
	CString strDocName;
	POSITION docpos = GetFirstDocTemplatePosition();
	while (docpos != NULL) 
	{
		pSelectedTemplate = (CDocTemplate*) GetNextDocTemplate(docpos);
		pSelectedTemplate->GetDocString(strDocName, CDocTemplate::docName);
		//if (rDocName == strDocName)
		{
			pSelectedTemplate->CloseAllDocuments(TRUE);
		}
	}
}

/**
	@brief	UNDO COMMAND 수행

	@author	BHK
*/
void CELOADApp::OnEditUndo()
{
	COMMAND::CCommandUndo cmdUndo;
	if(ERROR_SUCCESS == cmdUndo.Execute())
	{
		vector<string> DocTypeEntry;
		DocTypeEntry.push_back(CLoadItem::TypeString());
		UpdateAllViews(DocTypeEntry);

		CMainFrame* pFrame = ELOADGetMainWnd();
		if(pFrame) pFrame->m_wndPropertiesBar.ShowPane(FALSE,FALSE,FALSE);
	}
}

/**
	@brief	

	@author	BHK
*/
void CELOADApp::OnUpdateEditUndo(CCmdUI *pCmdUI)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	pCmdUI->Enable(docData.IsModified());
}
/*
        @brief  PROJECT SELECT
        @author KHS
**/
void CELOADApp::OnFileOpen()
{
	// TODO: Add your command handler code here
	CMainFrame* pFrame = ELOADGetMainWnd();
	if(pFrame) pFrame->OnToolsProjectselect();
}

extern "C" __declspec(dllimport) CString GetMACaddress();

/**
	@brief	HTTP를 통해서 업데이트된 프로그램이 있는지 확인후 다운로드하고나서 설치하도록 합니다.이떄 ELOAD는 종료가 됩니다.

	@author	BHK
*/
int CELOADApp::AutoUpdate(void)
{
	const CString rMACaddress = GetMACaddress();

	CString rExecPath = GetExecPath();
	if("\\" != rExecPath.Right(1)) rExecPath += _T("\\");

	int res = RETURN_OK;
	HMODULE hModule = AfxLoadLibrary(rExecPath + _T("AutoUp.dll"));
	if(hModule)
	{
		const CString rAutoUpIniFilePath = rExecPath + _T("AutoUp.ini");
		WritePrivateProfileString(_T("SECURITY") , _T("CODE") , rMACaddress , rAutoUpIniFilePath);

		CheckForUpdateFunc pfCheckForUpdate = (CheckForUpdateFunc)GetProcAddress(hModule , _T("CheckForUpdate"));
		res = pfCheckForUpdate(rAutoUpIniFilePath);
		if(RETURN_ERROR == res)
		{
			AfxMessageBox(_T("Connection Error."));
		}
		else if(RETURN_SECURITYERROR == res)
		{
			CString rString;
			rString.Format(_T("Security Error. (%s)MAC address를 등록하세요.") , rMACaddress);
			AfxMessageBox(rString);
		}
		else if((RETURN_OK == res) || (RETURN_NEWVERSION == res))
		{
			if(IDYES == AfxMessageBox(_T("새로운 버전이 나왔습니다. 다운로드할까요?") , MB_YESNO|MB_ICONQUESTION))
			{
				AutoUpFunc pfAutoUp = (AutoUpFunc)GetProcAddress(hModule , _T("AutoUp"));
				pfAutoUp(rExecPath + _T("AutoUp.ini"));
			}
		}

		AfxFreeLibrary(hModule);
	}

	return res;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-14 오후 11:48:30	

	@param	

	@return		
**/
BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_STATIC_VERSION)->SetWindowText(_T("ELoad Version "STRPRODUCTVER));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	ELoad에서 사용할 image list를 생성한다.

	@author BHK	

	@date 2009-05-10 오전 8:18:02	

	@param	

	@return		
**/
int CELOADApp::CreateELoadImageList(void)
{
	const CString rImageFilePath = GetExecPath() + _T("images");

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_CLASS_VIEW_HC : IDB_CLASS_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap (uiBmpId))
	{
		TRACE(_T ("Can't load bitmap: %x\n"), uiBmpId);
		ASSERT (FALSE);
		return ERROR_BAD_ENVIRONMENT;
	}

	BITMAP bmpObj;
	bmp.GetBitmap (&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

        /// Class View Tree  이미지리스트
        m_ELoadImages.Create (16, bmpObj.bmHeight, ILC_MASK|ILC_COLORDDB, 0, 0);
        m_ELoadImages.Add (&bmp, RGB (255, 0, 0));
	
	const CString __IMAGE_FILE_NAMES__[] = {"bus_group.bmp" , "bus.bmp" , "load.bmp" , "motor.bmp" , "transformer.bmp" , "cable.bmp" , "revision.bmp" , "capacitor.bmp" , "generator.bmp" , "upsdc.bmp" , "area.bmp", "PDB.bmp"};
	for(int i = 0;i < SIZE_OF_ARRAY(__IMAGE_FILE_NAMES__);++i)
	{
		HBITMAP hbitmap = (HBITMAP)LoadImage(::AfxGetApp()->m_hInstance,
					 rImageFilePath + _T("\\") +  __IMAGE_FILE_NAMES__[i], IMAGE_BITMAP, 0 , 0 ,
		   LR_LOADFROMFILE|LR_CREATEDIBSECTION|LR_DEFAULTSIZE);
		CBitmap bitmap;
		bitmap.Attach(hbitmap);
		m_ELoadImages.Add(&bitmap , RGB(255,0,0));
		bitmap.DeleteObject();
		if (hbitmap) DeleteObject(hbitmap);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	LOG 파일을 데이터베이스에 저장합니다.

	@author BHK	

	@date 2009-07-01 오후 11:55:46	

	@param	

	@return		
**/
int CELOADApp::PutLogFileToDatabase(const string& rLogFilePath, const string& rLogType)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	///
	const string rMDBFilePath = docData.GetProjectMDBFilePath();
	//! 테이블이 없으면 생성합니다.
	{
		TABLES::CELOADTable table;
		table.CreateTable(rMDBFilePath , _T("T_PRODUCT_LOG"));
	}
	//!

	CADODB adoDB;
	const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rConnectionString))
	{
		//! 기존에 입력한 값이 있으면 삭제한다.
		adoDB.ExecuteQuery(_T("DELETE * FROM T_PRODUCT_LOG WHERE C_PRODUCT_NAME = '") + rLogType + _T("'"));

		CString strFormat = _T("INSERT INTO T_PRODUCT_LOG(C_PRODUCT_NAME,C_DATE) VALUES('%s','%s')");
		SYSTEMTIME st;
		GetLocalTime( &st );
		CString strTimeData , strSQL;
		strTimeData.Format(_T("%d년 %d월 %d일 - %d시 %d분") , st.wYear , st.wMonth , st.wDay , st.wHour , st.wMinute);
		strSQL.Format(strFormat , rLogType.c_str() , strTimeData);
		adoDB.ExecuteQuery(strSQL.operator LPCTSTR());

		adoDB.OpenQuery(_T("SELECT * FROM T_PRODUCT_LOG WHERE C_PRODUCT_NAME = '") + rLogType + _T("'"));
		LONG lRecordCount = 0L;
		adoDB.GetRecordCount(&lRecordCount);
		if(1 == lRecordCount)
		{
			//! 먼저 log.html 파일을 다른 파일로 복사한다.
			CString rLogFile = rLogFilePath.c_str();
			const int at = rLogFile.ReverseFind('\\');
			CString rNewLogFile = rLogFile.Left(at);
			if('\\' != rNewLogFile.Right(1)) rNewLogFile += '\\';
			rNewLogFile += CString(rLogType.c_str()) + _T(".html");
			CopyFile(rLogFilePath.c_str() , rNewLogFile , FALSE);
			//! LOG파일을 압축한다.
			
			CZipArchive zip;
			CString rZipFilePath(rLogFilePath.c_str());
			rZipFilePath.Replace(_T("html") , _T("zip"));
			zip.Open(rZipFilePath , CZipArchive::create );
			zip.AddNewFile(rNewLogFile , -1 , false);
			zip.Close();

			CFile f(rZipFilePath , CFile::modeRead);
			ULONG fSize = f.GetLength();
			char* pData = (char*)calloc(1 , sizeof(char)*fSize);
			if(pData)
			{
				f.Read(pData,fSize);
				const int nFieldCount = adoDB.GetFieldCount();
				for(int i = 0;i < nFieldCount;++i)
				{
					if(_T("C_DATA") == adoDB.GetFieldName(i))
					{
						adoDB.AppendChunk(0 , i , pData , fSize);
						free((void*)pData);
						break;
					}
				}
			}
		}
		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	DB 에서 Log 파일을 가져온다.

	@author KHS	

	@date 2009-07-21 오후 1:03:26	

	@param	

	@return		
**/
int CELOADApp::GetLogFileToDatabase(const string& rLogFilePath, const string& rLogType)
{
	CELoadDocData & docData = CELoadDocData::GetInstance();
	
	//! Temp Folder가 없으면 생성
	if(!FileExist(docData.GetProjectFolderPath().c_str() + CString("Temp")))
	{
		CreateDirectory(docData.GetProjectFolderPath().c_str() + CString("Temp"), NULL);
	}

	const string rMDBFilePath = docData.GetProjectMDBFilePath();

	CADODB adoDB;
	const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rConnectionString))
	{
		adoDB.OpenQuery(_T("SELECT * FROM T_PRODUCT_LOG WHERE C_PRODUCT_NAME='") + rLogType + _T("'"));
		
		LONG lRecordCount = 0L;
		char *pData = NULL;
		long lSize = 0L;
		
		STRING_T rProductName, rDate;
		adoDB.GetRecordCount(&lRecordCount);
		if(1 == lRecordCount)
		{
			adoDB.GetFieldValue(0, _T("C_PRODUCT_NAME"), &rProductName);
			adoDB.GetFieldValue(0, _T("C_DATE"), &rDate);

			CString rZipFilePath = docData.GetProjectFolderPath().c_str() + CString("Temp\\log.html");
			rZipFilePath.Replace(_T("html"), _T("zip"));
			adoDB.GetFieldSize(0 , _T("C_DATA") , lSize);
			if(pData = (char*)calloc(1 , sizeof(char)*lSize))
			{
				adoDB.GetBinaryValue(0 , _T("C_DATA") , (LPVOID)pData , lSize);
				CFile f(rZipFilePath , CFile::modeCreate|CFile:: modeReadWrite);
				f.Write(pData , lSize);
				f.Close();
				UncompressFile(rZipFilePath, docData.GetProjectFolderPath().c_str() + CString("Temp\\"));
			}
		
			CString rLogFilePath = docData.GetProjectFolderPath().c_str() + CString("Temp\\") + CString(rLogType.c_str()) + ".html";
			
			HWND hwnd = GetForegroundWindow();
			ShellExecute(hwnd, "open", rLogFilePath, "log.txt", NULL, SW_SHOWNORMAL);
			//!Log 파일을 실행한다.

			if(pData)
			{
				free((void*)pData);
				pData = NULL;
			}
		}
	}
	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author KHS	

	@date 2009-07-21 오후 1:17:50	

	@param	

	@return		
**/
int CELOADApp::UncompressFile(const CString& rFilePath, const CString& rFolderPath)
{
	CZipArchive zip;
	zip.Open(rFilePath , CZipArchive::open);
	const int nEntries = zip.GetNoEntries();
	for(int i = 0;i < nEntries;++i)
	{
		zip.ExtractFile(i,rFolderPath);
	}
	zip.Close();

	return ERROR_SUCCESS;
}

/**
	@brief	ELOAD용 Document Template을 등록합니다.

	@author HumKyung	

	@date 2009-09-03 오후 2:03:12	

	@param	

	@return		
*/
int CELOADApp::AddELoadDocTemplate(void)
{
	CMultiDocTemplate* pDocTemplate = NULL;
	pDocTemplate = new CMultiDocTemplate(/*IDR_ELoadTYPE*/IDS_STRING_LOAD_TYPE,
		RUNTIME_CLASS(CELOADDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CELOADView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(/*IDR_ELoadTYPE*/IDS_STRING_BUS_TYPE,
		RUNTIME_CLASS(CBusDoc),
		RUNTIME_CLASS(CBusDiagramChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CBusView));
	if (!pDocTemplate) return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(/*IDR_ELoadTYPE*/IDS_STRING_LOAD_SUMMARY_TYPE,
		RUNTIME_CLASS(CLoadSummaryDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CLoadSummaryView));
	if (!pDocTemplate) return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(/*IDR_ELoadTYPE*/IDS_STRING_CABLE_SIZING_TYPE,
		RUNTIME_CLASS(CCableSizingTableDoc),
		RUNTIME_CLASS(CCableSizingTableChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CCableSizingTableView));
	if (!pDocTemplate) return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(/*IDR_ELoadTYPE*/IDS_STRING_TRASNFORMER_SIZING_RESULT_TYPE,
		RUNTIME_CLASS(CTransformerSizingResultDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CTransformerSizingResultView));
	if (!pDocTemplate) return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(/*IDR_ELoadTYPE*/IDS_STRING_AUTO_POWER_CABLE_TYPE,
		RUNTIME_CLASS(CCableSizingResultDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CCableSizingResultView));
	if (!pDocTemplate) return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(/*IDR_ELoadTYPE*/IDS_STRING_POWER_CABLE_VERIFICATION_TYPE,
		RUNTIME_CLASS(CPowerCableVerificationDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CPowerCableVerificationView));
	if (!pDocTemplate) return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(/*IDR_ELoadTYPE*/IDS_COMPARE_REVISION_DATA_TYPE,
		RUNTIME_CLASS(CCompareRevisionDataDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CCompareRevisionDataView));
	if (!pDocTemplate) return FALSE;
	AddDocTemplate(pDocTemplate);

	//!CABLE ROUTE CABLE LIST
	pDocTemplate = new CMultiDocTemplate(/*IDR_ELoadTYPE*/IDS_STRING_CABLE_ROUTE_CABLE_LIST_TYPE,
		RUNTIME_CLASS(CCableRouteCableListDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CCableRouteCableListView));
	if (!pDocTemplate) return FALSE;
	AddDocTemplate(pDocTemplate);

	//!CONTROL CABLE RESULT VIEW
	pDocTemplate = new CMultiDocTemplate(/*IDR_ELoadTYPE*/IDS_STRING_CONTROL_CABLE_RESULT_TYPE,
		RUNTIME_CLASS(CControlCableResultDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CControlCableResultView));
	if (!pDocTemplate) return FALSE;
	AddDocTemplate(pDocTemplate);

	//!USER CABLE RESULT VIEW
	pDocTemplate = new CMultiDocTemplate(/*IDR_ELoadTYPE*/IDS_STRING_USER_CABLE_RESULT_TYPE,
		RUNTIME_CLASS(CUserCableResultDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CUserCableResultView));
	if (!pDocTemplate) return FALSE;
	AddDocTemplate(pDocTemplate);

	//!CABLE SCHEDULE RESULT VIEW
	pDocTemplate = new CMultiDocTemplate(IDS_STRING_CABLE_SCHEDULE_RESULT_TYPE,
		RUNTIME_CLASS(CCableScheduleResultDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CCableScheduleResultView));
	if (!pDocTemplate) return FALSE;
	AddDocTemplate(pDocTemplate);

	//!EXCEL REPORT FORMAT VIEW
	pDocTemplate = new CMultiDocTemplate(/*IDR_ELoadTYPE*/IDS_STRING_EXCEL_REPORT_FORMAT_RESULT_TYPE,
		RUNTIME_CLASS(CExcelReportFormatDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CExcelReportFormatView));
	if (!pDocTemplate) return FALSE;
	AddDocTemplate(pDocTemplate);

	//!DRUM SCHEDULE RESULT VIEW
	pDocTemplate = new CMultiDocTemplate(IDS_STRING_DRUM_SCHEDULE_RESULT_TYPE,
		RUNTIME_CLASS(CDrumScheduleResultDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CDrumScheduleResultView));
	if (!pDocTemplate) return FALSE;
	AddDocTemplate(pDocTemplate);

	//!LOAD LIST SLD RESULT VIEW
	pDocTemplate = new CMultiDocTemplate(IDS_STRING_LOAD_LIST_SLD_RESULT_TYPE,
		RUNTIME_CLASS(CLoadListSldDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CLoadListSldView));
	if (!pDocTemplate) return FALSE;
	AddDocTemplate(pDocTemplate);

	//! BUS TO BUS VIEW - 2010.12.23 - added by HumKyung
	pDocTemplate = new CMultiDocTemplate(IDS_BUS_TO_BUS_TYPE,
		RUNTIME_CLASS(CBusToBusDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CBusToBusView));
	if (!pDocTemplate) return FALSE;
	AddDocTemplate(pDocTemplate);

	return 0;
}

/**
	@brief	return BusToBusView which is already opened

	@author	HumKyung

	@date	2010.12.23

	@param

	@return	CBusToBusView*
*/
CBusToBusView* CELOADApp::GetBusToBusView(void)
{
	CString strDocName;

	CDocTemplate* pSelectedTemplate;
	POSITION pos = GetFirstDocTemplatePosition();
	while (pos != NULL)
	{
		pSelectedTemplate = (CDocTemplate*) GetNextDocTemplate(pos);
		pSelectedTemplate->GetDocString(strDocName, CDocTemplate::docName);

		if (_T("BUS_TO_BUS") == strDocName)
		{
			POSITION docPos = pSelectedTemplate->GetFirstDocPosition();
			if(docPos == NULL)
			{
				return NULL;
			}
			else
			{
				CDocument *pDoc = (CDocument*)pSelectedTemplate->GetNextDoc(docPos);
				if(pDoc)
				{
					POSITION viewPos = pDoc->GetFirstViewPosition();
					return (CBusToBusView*)(pDoc->GetNextView(viewPos));
				}
			}
		}
	}

	return NULL;
}
