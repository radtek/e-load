#pragma once

// CExcelReportFormatDoc document

class CExcelReportFormatDoc : public CDocument
{
	DECLARE_DYNCREATE(CExcelReportFormatDoc)

public:
	CExcelReportFormatDoc();
	virtual ~CExcelReportFormatDoc();
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

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
};
