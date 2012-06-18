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
// ClassViewBar1.h: interface for the CELoadItemViewBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELOADITEMVIEWBAR1_H__FFAF44CB_B481_4420_8FDF_AF44ABE73D6C__INCLUDED_)
#define AFX_ELOADITEMVIEWBAR1_H__FFAF44CB_B481_4420_8FDF_AF44ABE73D6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ClassTreeWnd.h"
#include "EloadSplitterWnd.h"
#include "BusGroupItem.h"
#include "BusItem.h"
#include "BusGroupItemProp.h"
#include "BusItemProp.h"
#include "Observer.h"
#include "ELoad.h"
#include "LoadItem.h"

#include <vector>
#include <string>
#include <fstream>
#include <gui/ListCtrlEx.h>
#include "BusEditTreeCtrl.h"
#include "MFCListCtrlEx.h"
#include "EmptyWnd.h"

using namespace std;

#define	DECLARE_ELOAD_CODE_MAP(classname) \
public: \
	struct ELoadCodeMapEntry {char* pCommandStr; int (classname::* func)(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText); };\
	virtual void ELoadCodeProc(char* pCommandStr , const char* pTypeStr , const char* pHeaderText);\
private:

#define	BEGIN_ELOAD_CODE_MAP(classname) \
	static const classname::ELoadCodeMapEntry _ELoadCodeMap[]={
#define	END_ELOAD_CODE_MAP()	{"",0}};

#define	ELOADCODEFUNC(code,func)	{code,&func}

#define	ELOADCODEIMPLEMENT(classname) \
void classname::ELoadCodeProc(char* pCommandStr , const char* pTypeStr , const char* pHeaderText){\
	const int nSiz=sizeof(_ELoadCodeMap)/sizeof(classname::ELoadCodeMapEntry);\
	for(int i=0;i < nSiz;i++){\
		vector<string> oResult;\
		CTokenizer<CIsFromString>::Tokenize(oResult , pCommandStr , CIsFromString("|"));\
		if(0 == strcmp(oResult[0].c_str() ,_ELoadCodeMap[i].pCommandStr)){\
			(this->* _ELoadCodeMap[i].func)(pCommandStr , (2 == oResult.size()) ? oResult[1].c_str() : "" , pTypeStr , pHeaderText);\
			return;\
		}\
	}\
}\

class CELoadItemViewBar : public CDockablePane  
{
	enum
	{
		BUSTREE_ID		= 1234,
		POWER_CABLE_ID		= 1235,
		CONTROL_CABLE_ID	= 1236,
		USER_CABLE_ID		= 1237,
		TRANSFORMER_ID		= 1238,
		CAPACITOR_ID		= 1239,
		GENERATOR_ID		= 1240,
		UPSDC_ID		= 1241,
		SCHEMATIC_ID		= 1242,
		IO_ID			= 1243,
		LCS_ID			= 1244,
		EMPTY_ID		= 102
	};
public:
	CELoadItemViewBar();
	virtual ~CELoadItemViewBar();

	void AdjustLayout ();
	void OnChangeVisualStyle ();

	CClassTreeWnd			m_wndItemClassTreeCtrl;
protected:
	CEloadSplitterWnd		m_wndSplitter;
	CMFCListCtrlEx			m_wndLoadListCtrl;
	CMFCTransformerListCtrlEx	m_wndTransformerListCtrl;
	CMFCListCtrlEx			m_wndCapacitorListCtrl , m_wndGeneratorListCtrl , m_wndUPSDCListCtrl;
	CMFCListCtrlEx			m_wndSchematicListCtrl , m_wndIOListCtrl , m_wndLCSListCtrl;
	CMFCCableListCtrlEx		m_wndPowerCableListCtrl , m_wndControlCableListCtrl , m_wndUserCableListCtrl;
	CEmptyWnd			m_wndEmpty;

	CBusEditTreeCtrl	m_wndBusTreeView;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CELoadItemViewBar)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CELoadItemViewBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSelchangedBusTree(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLoadList();
	afx_msg void OnResourcesubResourcesub1();
	afx_msg void OnResourcesubResourcesub2();
	int OnClickTreeItem(const CString&  rCommand , const CString& rTypeStr = _T("NONE") , const CString& rHeaderText = _T("NONE"));
	int OnChangeLayoutSplitterWnd(void);
private:
	CString		m_rCurrentItemText;	//! 현재 선택된 TREE ITEM TEXT
	CObserver*	m_pObserver;
	map<CWnd*,vector<string> > m_ActionWndMap;
private:
	HTREEITEM FindBusGroupItem( CBusGroupItem* pBusGroupItem , HTREEITEM hItem );
	HTREEITEM AddBusGroupTreeCtrlItem( CBusGroupItem* pBusGrouPItem );
public:
        int AddBusTreeCtrlItem( CBusItem* pBusItem );
public:
	int UpdateBusTreeView(HTREEITEM hItem , CELoadItem* pItem);
	int UpdateLoadItem(CELoadItem* pItem );
	int UpdateLoadListCtrl(void);
	int UpdateCableListCtrl(const string& rCableType);
	static int OnEventFromObserver(CObserver* pObserver , CEventData* pEventData);
	afx_msg void OnItemShow();
        afx_msg void OnItemShowall();
	afx_msg void OnCalcLoadSummary();
	afx_msg void OnCalcCableSize();
        afx_msg void OnItemCopy();
        afx_msg void OnItemDelete();
	afx_msg void OnItemCreatePowerCableOfSelectedLoad();
	afx_msg void OnItemCreateControlCableOfSelectedLoad();
        void DisplayItemCount(void);
private:
        void CreateSystemBusGroup();
public:
        afx_msg void OnEditCut();
        afx_msg void OnEditPaste();
        afx_msg void OnEditClear();
	afx_msg void OnItemShowList();
        afx_msg void OnCreateLoad();

        int UpdateShowLoadBusItem();
	int ShowAreaCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	int ShowBusCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	int ShowPDBCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	int ShowLoadCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	int ShowNotAssignedLoadCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	int ShowLoadInBusCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	int ShowLoadSummaryCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	int ShowAllBUSLoadSummaryCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	int ShowAllRevBUSLoadSummaryCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	int ShowAllPDBLoadSummaryCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	int ShowAllRevPDBLoadSummaryCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	int ShowLoadSummaryInBusCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	int ShowCableSizingTableCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	int ShowSchematicCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	int ShowIOCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	int ShowLCSCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	int ShowPowerCableCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	int ShowControlCableCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	int ShowUserCableCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	int ShowTransformerSizingResultCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	int ShowRevTransformerSizingResultCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	int ShowCableSizingResultCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	int ShowRevCableSizingResultCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	int ShowItemCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	int ShowCableRouteResultCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	int ShowCableScheduleCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	int ShowDrumScheduleCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	int ShowRevControlCableResultCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	int ShowLoadListSldCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	//! 2010.12.23 - added by HumKyung
	int ShowBusToBusCallback(const char* pCommandStr , const char* pParam , const char* pTypeStr , const char* pHeaderText);
	
	int OnCreateBusTreeCtrl(void);
	int RemakeBusTreeCtrl(void);
	int RemakeLoadListCtrl(const string& rBusID , const string& rArea , vector<CLoadItem*>& LoadItemEntry);
	DECLARE_ELOAD_CODE_MAP(CELoadItemViewBar)
 
	afx_msg void OnCreateTransformer();
	afx_msg void OnCopyTransformer();
	afx_msg void OnDeleteTransformer();

	afx_msg void OnCreateCable();
	afx_msg void OnCopyCable();
	afx_msg void OnDeleteCable();
	afx_msg void OnDeleteAllCable();

	afx_msg void OnCablesizingresultShowcablesizingresult();
	afx_msg void OnDeleteCableSizingTableFile();
	afx_msg void OnSaveProductToRevision();
	afx_msg void OnViewRevision();
	afx_msg void OnNewcapacitor();
	afx_msg void OnCapacitorCopy();
	afx_msg void OnCapacitorDelete();

	afx_msg void OnNewGenerator();
	afx_msg void OnGeneratorCopy();
	afx_msg void OnGeneratorDelete();

	afx_msg void OnNewUPSDC();
	afx_msg void OnUPSDCCopy();
	afx_msg void OnUPSDCDelete();

	int UpdateCableTreeNodeForCount(void);
	int UserCableExcelImport(const CString& rExcelFilePath, const int& nOption);
private:
	CWnd* FindWndByCommandStr(const string& rCommandStr);
	int CreateCableListCtrl(const DWORD& dwStyle);
public:
	afx_msg void OnUserCableImport();
	int UserCableExcelImportExecute(const CString& rExcelFilePath, const int& nOption);
	static UINT StatusUserCableExcelImportThreadEntry(LPVOID pVoid);
	int DeleteAllUserCable(void);
	afx_msg void OnRevItemDelete();

	int	m_nTryCount , m_nSuccessCount , m_nFailCount;
	string GetUserCableImportResultString(void);
private:
	int OpenDocumentDependOn(const CString& rProductName , const CString& rTarget , const CString& rExtraParam);
};

#endif // !defined(AFX_ELOADITEMVIEWBAR1_H__FFAF44CB_B481_4420_8FDF_AF44ABE73D6C__INCLUDED_)
