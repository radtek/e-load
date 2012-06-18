#pragma once

// CLoadListSldDoc document

class CLoadListSldDoc : public CDocument
{
	DECLARE_DYNCREATE(CLoadListSldDoc)

public:
	CLoadListSldDoc();
	virtual ~CLoadListSldDoc();
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
	CString m_rFolderPath;
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	int CreateFolderIfNeed(const string& rFolderPath = _T(""));
	int WriteLoadListSldDataInText(void);
	static UINT StatusWriteTextThreadEntry(LPVOID pVoid);
	int StatusWriteTextThread();
};
