
// ELOAD.h : ELOAD 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include <util/ado/ADODB.h>
#include "BusGroupItem.h"
#include "MFCPropEx.h"
#include "resource.h"       // 주 기호입니다.
#include "BusToBusView.h"

using namespace ITEMS;
// CELOADApp:
// 이 클래스의 구현에 대해서는 ELOAD.cpp을 참조하십시오.
//

class CELOADApp : public CWinAppEx
{
public:
	CELOADApp();

	// Override from CBCGPWorkspace
	BOOL LoadState (LPCTSTR lpszSectionName = NULL, CFrameImpl* pFrameImpl = NULL)
	{
		//! 저장한 State를 불러 오는 것을 막는다.
		return TRUE;
	}

	BOOL SaveState(LPCTSTR lpszSectionName = NULL, CFrameImpl* pFrameImpl = NULL)
	{
		return TRUE;
	}

// 재정의입니다.
public:
	virtual BOOL InitInstance();

// 구현입니다.
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;
	CImageList m_ELoadImages;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();
	BOOL OpenNewDocumentIfNeed(const CString &strTarget , LPCTSTR pExtraParam=NULL);
	void OnSaveDataBase();

	CBusGroupItem* FindBusGroup(const CString& rBusGroupName);
	
	afx_msg void OnFileNew();
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
private:        	
	int OpenQuery(CADODB* pAdoDb, const STRING_T& rQuery);
	int ExecuteQuery(CADODB* pAdoDb, const STRING_T& rQuery);
public:
	int UpdateAllViews(const vector<string>& DocTypeEntry);
        void CloseDocument(const CString& rDocName);
	int PutLogFileToDatabase(const string& rLogFilePath, const string& rLogType);
	int GetLogFileToDatabase(const string& rLogFilePath, const string& rLogType);
	int UncompressFile(const CString& rFilePath, const CString& rFolderPath);
public:
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
        afx_msg void OnFileOpen();
private:
	int AutoUpdate(void);
	int CreateELoadImageList(void);
	int AddELoadDocTemplate(void);
public:
	CBusToBusView* GetBusToBusView(void);
};

extern CELOADApp theApp;
extern log4cxx::LoggerPtr       mylogger;