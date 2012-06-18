#pragma once


// CDrumScheduleResultDoc document

#include "WorkStatusDlg.h"

class CDrumScheduleResultDoc : public CDocument
{
	DECLARE_DYNCREATE(CDrumScheduleResultDoc)

public:
	CDrumScheduleResultDoc();
	virtual ~CDrumScheduleResultDoc();
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
	int CreateFolderIfNeed(const string& rFolderPath = "");
	int WriteDrumScheduleDataInText(void);
	static UINT StatusWriteTextThreadEntry(LPVOID pVoid);
	int StatusWriteTextThread();

	CString m_rFolderPath;
	int WriteTotalDrumScheduleResult(CWorkStatusDlg *pDlg);
	int WriteTotalDrumCableResult(CWorkStatusDlg *pDlg);
private:
	CString m_rCurDrum;
};
