#pragma once

// CCableScheduleResultDoc document

class CCableScheduleResultDoc : public CDocument
{
	DECLARE_DYNCREATE(CCableScheduleResultDoc)

public:
	CCableScheduleResultDoc();
	virtual ~CCableScheduleResultDoc();
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

	int WriteCableScheduleDataInText(void);
public:
	static UINT StatusWriteTextThreadEntry(LPVOID pVoid);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

	CString m_rFolderPath;
private:
	int StatusWriteTextThread();
	int WriteCableItemToTextFile(ofstream& ofile, CELoadItem* pCableItem);
	int CreateFolderIfNeed(const string& rFolderPath = _T(""));
};

namespace CableScheduleResult
{
	bool SortCableItemFunc(CELoadItem* lhs , CELoadItem* rhs);
};
