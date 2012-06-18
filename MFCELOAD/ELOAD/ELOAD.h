
// ELOAD.h : ELOAD ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include <util/ado/ADODB.h>
#include "BusGroupItem.h"
#include "MFCPropEx.h"
#include "resource.h"       // �� ��ȣ�Դϴ�.
#include "BusToBusView.h"

using namespace ITEMS;
// CELOADApp:
// �� Ŭ������ ������ ���ؼ��� ELOAD.cpp�� �����Ͻʽÿ�.
//

class CELOADApp : public CWinAppEx
{
public:
	CELOADApp();

	// Override from CBCGPWorkspace
	BOOL LoadState (LPCTSTR lpszSectionName = NULL, CFrameImpl* pFrameImpl = NULL)
	{
		//! ������ State�� �ҷ� ���� ���� ���´�.
		return TRUE;
	}

	BOOL SaveState(LPCTSTR lpszSectionName = NULL, CFrameImpl* pFrameImpl = NULL)
	{
		return TRUE;
	}

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.
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