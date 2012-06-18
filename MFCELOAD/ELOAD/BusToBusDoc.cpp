// BusToBusDoc.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "BusToBusDoc.h"

//////////////////////////////////////////////////////////////////////////////////////////
// CBusToBusDoc

IMPLEMENT_DYNCREATE(CBusToBusDoc, CDocument)

CBusToBusDoc::CBusToBusDoc()
{
}

BOOL CBusToBusDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CBusToBusDoc::~CBusToBusDoc()
{
}


BEGIN_MESSAGE_MAP(CBusToBusDoc, CDocument)
END_MESSAGE_MAP()

/**
	@brief	

	@author	HumKyung

	@date	2011.01.05

	@param	lpszPathName

	@return
*/
BOOL CBusToBusDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	return TRUE;
}
// CBusToBusDoc diagnostics

#ifdef _DEBUG
void CBusToBusDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CBusToBusDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CBusToBusDoc serialization

void CBusToBusDoc::Serialize(CArchive& ar)
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


// CBusToBusDoc commands
