#pragma once

#include "LoadItem.h"
#include <sstream>
using namespace std;

// CCableSizingResultDoc document

class CCableSizingResultDoc : public CDocument
{
	DECLARE_DYNCREATE(CCableSizingResultDoc)

public:
	enum
	{
		ALL_POWER_CABLE = 0x00,
		UNSIZED_POWER_CABLE_ONLY = 0x01,
	};

	CCableSizingResultDoc();
	virtual ~CCableSizingResultDoc();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	CString m_rFolderPath;

	static int m_nLoadPowerCableCreation;
	static list<ITEMS::CELoadItem*> m_InterestingLoadItemList;
protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
	static int WriteCableResultDataInBus(ofstream& ofile , CBusItem* pBusItem);
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	int StatusWriteTextThread(void);
private:
	static UINT StatusWriteTextThreadEntry(LPVOID pVoid);
	int WriteCableSizeDataToTextFile(void);
	int CreateFolderIfNeed(const string& rFolderPath = _T(""));

};
