#pragma once
#include "ximajpg.h"

// CBusDoc document

class CBusDoc : public CDocument
{
	DECLARE_DYNCREATE(CBusDoc)

public:
	CBusDoc();
	virtual ~CBusDoc();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	CxImage *m_pCxImage;				//main image
	CWinThread* m_pThread;
protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
public:
	inline CxImage *GetImage() { return m_pCxImage; }
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	int ViewBusDiagram(LPCTSTR lpszPathName);
	int GenerateBusDiagram(LPCTSTR lpszPathName);
};
