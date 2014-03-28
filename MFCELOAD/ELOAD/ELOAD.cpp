
// ELOAD.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
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
	// ǥ�� ������ ���ʷ� �ϴ� ���� ����Դϴ�.
	ON_COMMAND(ID_FILE_NEW, &CELOADApp::OnFileNew)
	ON_COMMAND(ID_FILE_SAVE, CELOADApp::OnSaveDataBase)
	ON_COMMAND(ID_EDIT_UNDO, &CELOADApp::OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CELOADApp::OnUpdateEditUndo)
        ON_COMMAND(ID_FILE_OPEN, &CELOADApp::OnFileOpen)
END_MESSAGE_MAP()

// CELOADApp ����

CELOADApp::CELOADApp()
{
	m_bHiColorIcons = TRUE;

	//! ifstream���� ���ϸ��� �ѱ۷ε� ������ ���� ���ϴ� ������ �����. ���� �Ʒ��� �ڵ带 �߰�����.
	setlocale(LC_ALL , _T("Korean"));
}

// ������ CELOADApp ��ü�Դϴ�.

CELOADApp theApp;
log4cxx::LoggerPtr mylogger;

extern "C" __declspec(dllimport) int CheckForUpdate(const char* rIniFilePath);
extern "C" __declspec(dllimport) int AutoUp(const char* pszIniFilePath);

// CELOADApp �ʱ�ȭ
BOOL CELOADApp::InitInstance()
{
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�. 
	// InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
	// �� �׸��� �����Ͻʽÿ�.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// OLE ���̺귯���� �ʱ�ȭ�մϴ�.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	/// terminate if can't connect internet.
	/// ���Ȱ� ������ Ȯ���Ͽ� ���ο� ������ �������� �ٿ�ε��Ͽ� ��ġ�ϵ��� �Ѵ�.
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

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("ELOAD"));
	LoadStdProfileSettings(0);  // MRU�� �����Ͽ� ǥ�� INI ���� �ɼ��� �ε��մϴ�.

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
	// ���� ���α׷��� ���� ���ø��� ����մϴ�. ���� ���ø���
	//  ����, ������ â �� �� ������ ���� ������ �մϴ�.
	AddELoadDocTemplate();

	// �� MDI ������ â�� ����ϴ�.
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	// ���̻簡 ���� ��쿡�� DragAcceptFiles�� ȣ���մϴ�.
	//  MDI ���� ���α׷������� m_pMainWnd�� ������ �� �ٷ� �̷��� ȣ���� �߻��ؾ� �մϴ�.

	// ǥ�� �� ���, DDE, ���� ���⿡ ���� ������� ���� �м��մϴ�.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// ����ٿ� ������ ����� ����ġ�մϴ�.
	// ���� ���α׷��� /RegServer, /Register, /Unregserver �Ǵ� /Unregister�� ���۵� ��� FALSE�� ��ȯ�մϴ�.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// �� â�� �ʱ�ȭ�Ǿ����Ƿ� �̸� ǥ���ϰ� ������Ʈ�մϴ�.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

/**
        @brief	BusGroup�̸����� BusGroup�� ã�´�.

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
			else	//! �̿��� TEMPLATE�� �ݴ´�.
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
			//! �̿��� TEMPLATE�� �ݴ´�.
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
			else	//! �̿��� TEMPLATE�� �ݴ´�.
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
			else	//! �̿��� TEMPLATE�� �ݴ´�.
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
			else	//! �̿��� TEMPLATE�� �ݴ´�.
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
			else	//! �̿��� TEMPLATE�� �ݴ´�.
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
			else	//! �̿��� TEMPLATE�� �ݴ´�.
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
		rMsg.Format("*** DOG CHECK [ADO DB] => <ExecuteQuery ����> ����[%s] !! ***\n", (LPCSTR)e.Description());
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
		rMsg.Format("*** DOG CHECK [ADO DB] => <ExecuteQuery ����> ����[%s] !! ***\n", (LPCSTR)e.Description());
		AfxMessageBox(rMsg);
		return ERROR_BAD_ENVIRONMENT;
	}
	return ERROR_SUCCESS;
}

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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

// ��ȭ ���ڸ� �����ϱ� ���� ���� ���α׷� ����Դϴ�.
void CELOADApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CELOADApp ����� ���� �ε�/���� �޼���

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

// CELOADApp �޽��� ó����

/**
        @brief	Save DataBase
	
	@author	KHS
*/
void CELOADApp::OnSaveDataBase()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	docData.Save();
	
	//! ���� �� prop TEXT ���� �ʱ�ȭ
	vector<string> emptyEntry;
	UpdateAllViews(emptyEntry);

	MessageBox(NULL,"SUCCESS !!", "SAVE", MB_ICONINFORMATION);
}


/**
	@brief	DocTypeEntry�� ����ִ� DocType�� ���� ��� ����� �����Ѵ�.���� DocTypeEntry�� ��� ������ ��� ����� �����Ѵ�.

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
        @brief  Document�� �ݴ´�.

        @author KHS
*/
void CELOADApp::CloseDocument(const CString& rDocName)
{
	//! �並 �ݴ´�
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
	@brief	UNDO COMMAND ����

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
	@brief	HTTP�� ���ؼ� ������Ʈ�� ���α׷��� �ִ��� Ȯ���� �ٿ�ε��ϰ��� ��ġ�ϵ��� �մϴ�.�̋� ELOAD�� ���ᰡ �˴ϴ�.

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
			rString.Format(_T("Security Error. (%s)MAC address�� ����ϼ���.") , rMACaddress);
			AfxMessageBox(rString);
		}
		else if((RETURN_OK == res) || (RETURN_NEWVERSION == res))
		{
			if(IDYES == AfxMessageBox(_T("���ο� ������ ���Խ��ϴ�. �ٿ�ε��ұ��?") , MB_YESNO|MB_ICONQUESTION))
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

	@date 2009-04-14 ���� 11:48:30	

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
	@brief	ELoad���� ����� image list�� �����Ѵ�.

	@author BHK	

	@date 2009-05-10 ���� 8:18:02	

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

        /// Class View Tree  �̹�������Ʈ
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
	@brief	LOG ������ �����ͺ��̽��� �����մϴ�.

	@author BHK	

	@date 2009-07-01 ���� 11:55:46	

	@param	

	@return		
**/
int CELOADApp::PutLogFileToDatabase(const string& rLogFilePath, const string& rLogType)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	///
	const string rMDBFilePath = docData.GetProjectMDBFilePath();
	//! ���̺��� ������ �����մϴ�.
	{
		TABLES::CELOADTable table;
		table.CreateTable(rMDBFilePath , _T("T_PRODUCT_LOG"));
	}
	//!

	CADODB adoDB;
	const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rConnectionString))
	{
		//! ������ �Է��� ���� ������ �����Ѵ�.
		adoDB.ExecuteQuery(_T("DELETE * FROM T_PRODUCT_LOG WHERE C_PRODUCT_NAME = '") + rLogType + _T("'"));

		CString strFormat = _T("INSERT INTO T_PRODUCT_LOG(C_PRODUCT_NAME,C_DATE) VALUES('%s','%s')");
		SYSTEMTIME st;
		GetLocalTime( &st );
		CString strTimeData , strSQL;
		strTimeData.Format(_T("%d�� %d�� %d�� - %d�� %d��") , st.wYear , st.wMonth , st.wDay , st.wHour , st.wMinute);
		strSQL.Format(strFormat , rLogType.c_str() , strTimeData);
		adoDB.ExecuteQuery(strSQL.operator LPCTSTR());

		adoDB.OpenQuery(_T("SELECT * FROM T_PRODUCT_LOG WHERE C_PRODUCT_NAME = '") + rLogType + _T("'"));
		LONG lRecordCount = 0L;
		adoDB.GetRecordCount(&lRecordCount);
		if(1 == lRecordCount)
		{
			//! ���� log.html ������ �ٸ� ���Ϸ� �����Ѵ�.
			CString rLogFile = rLogFilePath.c_str();
			const int at = rLogFile.ReverseFind('\\');
			CString rNewLogFile = rLogFile.Left(at);
			if('\\' != rNewLogFile.Right(1)) rNewLogFile += '\\';
			rNewLogFile += CString(rLogType.c_str()) + _T(".html");
			CopyFile(rLogFilePath.c_str() , rNewLogFile , FALSE);
			//! LOG������ �����Ѵ�.
			
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
	@brief	DB ���� Log ������ �����´�.

	@author KHS	

	@date 2009-07-21 ���� 1:03:26	

	@param	

	@return		
**/
int CELOADApp::GetLogFileToDatabase(const string& rLogFilePath, const string& rLogType)
{
	CELoadDocData & docData = CELoadDocData::GetInstance();
	
	//! Temp Folder�� ������ ����
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
			//!Log ������ �����Ѵ�.

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

	@date 2009-07-21 ���� 1:17:50	

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
	@brief	ELOAD�� Document Template�� ����մϴ�.

	@author HumKyung	

	@date 2009-09-03 ���� 2:03:12	

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
