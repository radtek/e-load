#pragma once

#include "LoadItem.h"
// CControlCableResultDoc document

class CControlCableResultDoc : public CDocument
{
	DECLARE_DYNCREATE(CControlCableResultDoc)

public:
	enum
	{
		ALL_CONTROL_CABLE           = 0x00,
		UNSIZED_CONTROL_CABLE_ONLY  = 0x01,
		SELECTED_CONTROL_CABLE_ONLY = 0x02
	};

	CControlCableResultDoc();
	virtual ~CControlCableResultDoc();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	static list<ITEMS::CELoadItem*> m_InterestingLoadItemList;
protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	static UINT StatusWriteTextThreadEntry(LPVOID pVoid);
	int StatusWriteTextThread();
	int WriteControlCableDataInTextFile(void);
	int CreateFolderIfNeed(const string& rFolderPath);

	CString m_rRevFolderPath;

};
