#pragma once

///#include "BusToBusEditor.h"
///#include "BusToBusEditor.h"

// CBusToBusDoc document

class CBusToBusDoc : public CDocument
{
	DECLARE_DYNCREATE(CBusToBusDoc)

public:
	CBusToBusDoc();
	virtual ~CBusToBusDoc();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
protected:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

	DECLARE_MESSAGE_MAP()
};
