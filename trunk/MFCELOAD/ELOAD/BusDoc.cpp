// BusDoc.cpp : implementation file
//

#include "stdafx.h"
#include "ELoad.h"
#include "BusDoc.h"
#include "pyfromc.h"

// CBusDoc

IMPLEMENT_DYNCREATE(CBusDoc, CDocument)

CBusDoc::CBusDoc() : m_pCxImage(NULL) , m_pThread(NULL)
{
}

BOOL CBusDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CBusDoc::~CBusDoc()
{
	try
	{
		// stop the elaborations
		if (m_pCxImage) m_pCxImage->SetEscape(1);
		// free objects
		SAFE_DELETE(m_pCxImage);

		if(NULL != m_pThread)
		{
			// we gave the thread a chance to quit. Since the thread didn't
			// listen to us we have to kill it.
			TerminateThread(m_pThread->m_hThread, (DWORD)-1);
		}
	}
	catch(...)
	{
	}
}


BEGIN_MESSAGE_MAP(CBusDoc, CDocument)
END_MESSAGE_MAP()


// CBusDoc diagnostics

#ifdef _DEBUG
void CBusDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CBusDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CBusDoc serialization

void CBusDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}
#endif


// CBusDoc commands

BOOL CBusDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: Add your specialized code here and/or call the base class
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();

	return TRUE;
	///pApp->Save
	///return CDocument::OnSaveDocument(lpszPathName);
}

typedef struct THREADPARAM
{
	CBusDoc* pBusDoc;
	CString  rPathName;
}THREADPARAM;

static UINT gfncGenerateBusDiagram(LPVOID pVoid)
{
	THREADPARAM* pParam = (THREADPARAM*) pVoid;
	if(pParam)
	{
		//! BusDiagram을 생성을 시도한다.
		///python.CallPyGenerateBusDiagram();
		pParam->pBusDoc->ViewBusDiagram(pParam->rPathName);

		///WaitForSingleObject(pParam->pBusDoc->m_pThread->m_hThread, INFINITE);
		pParam->pBusDoc->m_pThread = NULL;

		SAFE_DELETE(pParam);
	}
	
	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	HumKyung

	@date	????.??.??

	@param	lpszPathName

	@return
*/
BOOL CBusDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if(TRUE == FileExist(lpszPathName))
	{
		python.CallPyGenerateBusDiagram();
		m_pCxImage = new CxImage(lpszPathName, CXIMAGE_FORMAT_JPG);

		if (!m_pCxImage->IsValid())
		{
			CString s = m_pCxImage->GetLastError();
			AfxMessageBox(s);
			delete m_pCxImage;
			m_pCxImage = NULL;
			return FALSE;
		}
	}
	else
	{
		THREADPARAM param;
		param.pBusDoc   = this;
		param.rPathName = lpszPathName;

		//! BusDiagram을 생성을 시도한다.
		python.CallPyGenerateBusDiagram();
		ViewBusDiagram(lpszPathName);
		
		/*
		CWinThread* pThread = AfxBeginThread(gfncGenerateBusDiagram , &param , THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
		if(pThread)
		{
			pThread->ResumeThread();
		}
		*/
	}

	return TRUE;
}

/**
	@brief	generate bus diagram

	@author	BHK
*/
int CBusDoc::ViewBusDiagram(LPCTSTR lpszPathName)
{	
	SAFE_DELETE(m_pCxImage);
	m_pCxImage = new CxImage(lpszPathName, CXIMAGE_FORMAT_JPG);

	if (!m_pCxImage->IsValid())
	{
		CString s = m_pCxImage->GetLastError();
		AfxMessageBox(s);
		delete m_pCxImage;
		m_pCxImage = NULL;
		return ERROR_BAD_ENVIRONMENT;
	}

	UpdateAllViews(NULL);

	return ERROR_SUCCESS;
}

/**
	@breif	generate bus diagram.

	@author	BHK

	@date	2009.12.07

	@param

	@return
*/
int CBusDoc::GenerateBusDiagram(LPCTSTR lpszPathName)
{
	static UINT __WORKING__ = false;

	if(NULL == m_pThread)
	{
		THREADPARAM* pParam = new THREADPARAM;
		pParam->pBusDoc   = this;
		pParam->rPathName = lpszPathName;

		python.CallPyGenerateBusDiagram();
		m_pThread = AfxBeginThread(gfncGenerateBusDiagram , pParam , THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
		if(m_pThread)
		{
			m_pThread->ResumeThread();
		}
	}///else	AfxMessageBox(_T("이미 작업 중입니다."));

	return 0;
}
