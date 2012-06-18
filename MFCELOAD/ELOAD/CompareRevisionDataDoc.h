#pragma once

// CCompareRevisionDataDoc document
#include "LoadSummaryDoc.h"

class CCompareRevisionDataDoc : public CDocument
{
	DECLARE_DYNCREATE(CCompareRevisionDataDoc)

public:
	typedef struct
	{
		CString rProductName;
		int nLeftSideRevNo , nRightSideRevNo;
	}COMPARE_ITEM;

	typedef struct
	{
		string key;
		vector<string> cols;
	}CompareRevisionRowData;
	typedef vector<CompareRevisionRowData> CompareRevisionDataSheet;

	CCompareRevisionDataDoc();
	virtual ~CCompareRevisionDataDoc();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	map<string , CompareRevisionDataSheet*> m_CompareRevisionDataSheetMap;
protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	static CString GetRevPath(const CString& rProductName , const CString& rRevNo);
	static int UncompressRevFile(const CString& rRevFilePath, const CString& rFolder);

	CString m_rProductName;
	int m_nLeftSideRevNo , m_nRightSideRevNo;
	vector<string> m_LeftSideItemEntry , m_RightSideItemEntry;
private:
public:
	int CompareRevisionData(vector<int>& CompareResult, const CompareRevisionDataSheet& lhs, CompareRevisionRowData& rhs);
	CString GetLeftSideFolderPath(void);
	CString GetRightSideFolderPath(void);
};
