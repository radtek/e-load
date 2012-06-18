#pragma once

// CCableSizingTableDoc document

class CCableSizingTableDoc : public CDocument
{
	DECLARE_DYNCREATE(CCableSizingTableDoc)

public:
	CCableSizingTableDoc();
	virtual ~CCableSizingTableDoc();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	string m_rFolderPath , m_rFilePath;
	string m_rCableSizingDataFileName;
	string m_rCableSizingExcelFilePath;
protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
	int CreateFolderIfNeed(const string& rFolderPath);
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
};
