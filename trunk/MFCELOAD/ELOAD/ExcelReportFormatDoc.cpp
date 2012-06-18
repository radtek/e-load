// ExcelReportFormatDoc.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "ExcelReportFormatDoc.h"


// CExcelReportFormatDoc

IMPLEMENT_DYNCREATE(CExcelReportFormatDoc, CDocument)

CExcelReportFormatDoc::CExcelReportFormatDoc()
{
}

BOOL CExcelReportFormatDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CExcelReportFormatDoc::~CExcelReportFormatDoc()
{
}


BEGIN_MESSAGE_MAP(CExcelReportFormatDoc, CDocument)
END_MESSAGE_MAP()


// CExcelReportFormatDoc diagnostics

#ifdef _DEBUG
void CExcelReportFormatDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CExcelReportFormatDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CExcelReportFormatDoc serialization

void CExcelReportFormatDoc::Serialize(CArchive& ar)
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


// CExcelReportFormatDoc commands

BOOL CExcelReportFormatDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	/*if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;*/

	// TODO:  Add your specialized creation code here

	return TRUE;
}
