#pragma once

#include "LoadItem.h"
#include <sstream>
#include <map>
using namespace std;

// CPowerCableVerificationDoc document

class CPowerCableVerificationDoc : public CDocument
{
	DECLARE_DYNCREATE(CPowerCableVerificationDoc)

public:
	enum
	{
		ALL_POWER_CABLE = 0x00,
		UNSIZED_POWER_CABLE_ONLY = 0x01
	};

	CPowerCableVerificationDoc();
	virtual ~CPowerCableVerificationDoc();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	string m_rFilePath;

	static int m_nLoadPowerCableCreation;
	static list<ITEMS::CLoadItem*> m_LoadItemListCreateUnsizedPowerCable;
protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
	static int SaveCableResultDataInBus(ofstream& ofile , CBusItem* pBusItem);
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	static int SaveCableSizeResultData(void);

	map<string , list<CELoadItem*>*> m_LoadCableMap;
private:
	int CreateFolderIfNeed(const string& rFolderPath = _T(""));
};

////////////////////////////////////////////////////////////////////////////////////////////////////
class CPowerCableVerificationMemStrategyState : public CItemMemStrategy::CItemMemStrategyState
{
	CPowerCableVerificationMemStrategyState(CPowerCableVerificationDoc* pDoc) : m_pDoc(pDoc)
	{
		CItemMemStrategy& strategy = CItemMemStrategy::GetInstance();
		strategy.m_pStrategyState = this;
	}
	~CPowerCableVerificationMemStrategyState()
	{
		CItemMemStrategy& strategy = CItemMemStrategy::GetInstance();
		strategy.m_pStrategyState = NULL;
	}

	int OnCreate(ITEMS::CELoadItem* pItem);
	int OnDelete(ITEMS::CELoadItem* pItem);
private:
	CPowerCableVerificationDoc* m_pDoc;
};
