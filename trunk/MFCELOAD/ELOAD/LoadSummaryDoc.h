#pragma once

// CLoadSummaryDoc document

class CLoadSummaryDoc : public CDocument
{
	DECLARE_DYNCREATE(CLoadSummaryDoc)
public:
	typedef struct
	{
		CString rFileTitle;
		CString rFilePath;
	}LOAD_SUMMARY_ITEM;

	CLoadSummaryDoc();
	virtual ~CLoadSummaryDoc();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	string m_rLoadSummaryType;
	string m_rLoadSummaryExcelFilePath;
	string m_rLoadSummaryRevNo;
	string m_rProductName;
	
	string m_rFolderPath;	//! file path contains *.bus files
protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
public:
	vector<LOAD_SUMMARY_ITEM> m_LoadSummaryItemEntry;

	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	string GetLoadSummaryExcelFilePath() const
	{
		return m_rLoadSummaryExcelFilePath;
	}
};
