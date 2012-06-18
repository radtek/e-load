// CableSizingDoc.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "CableSizingTableDoc.h"
#include "ELoadDocData.h"
#include "CompareRevisionDataDoc.h"

// CCableSizingTableDoc

IMPLEMENT_DYNCREATE(CCableSizingTableDoc, CDocument)

CCableSizingTableDoc::CCableSizingTableDoc()
{
}

BOOL CCableSizingTableDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CCableSizingTableDoc::~CCableSizingTableDoc()
{
}


BEGIN_MESSAGE_MAP(CCableSizingTableDoc, CDocument)
END_MESSAGE_MAP()


// CCableSizingTableDoc diagnostics

#ifdef _DEBUG
void CCableSizingTableDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CCableSizingTableDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CCableSizingTableDoc serialization

void CCableSizingTableDoc::Serialize(CArchive& ar)
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


// CCableSizingTableDoc commands

/**
	@brief	

	@author	KHS,BHK
*/
BOOL CCableSizingTableDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	CString rString(lpszPathName);

	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string rProjectPath = docData.GetProjectFolderPath();

	m_rCableSizingDataFileName = lpszPathName;
	if((_T("REV(") == rString.Left(4)) && (_T(")") == rString.Right(1)))
	{
		rString = rString.Right(rString.GetLength() - 4);
		rString = rString.Left(rString.GetLength() - 1);
		const CString rRevNo = rString;
		m_rFolderPath = rProjectPath + string(_T("Cable Sizing Table\\")) + rRevNo.operator LPCSTR();

		CELoadDocData& docData = CELoadDocData::GetInstance();
		const string rMDBFilePath = docData.GetProjectMDBFilePath();

		CADODB adoDB;
		const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;
		if(TRUE == adoDB.DBConnect(rConnectionString))
		{
			const CString rFormat(_T("SELECT * FROM T_PRODUCT_REVISION WHERE C_PRODUCT_NAME = 'Cable Sizing Table' AND C_REVISION_NO = %s"));

			CString rSQL;
			rSQL.Format(rFormat , rRevNo);

			adoDB.OpenQuery(rSQL.operator LPCTSTR());
			LONG lRecordCount = 0L;
			adoDB.GetRecordCount(&lRecordCount);
			if(1 == lRecordCount)
			{
				STRING_T strValue;
				adoDB.GetFieldValue(0 , _T("C_REVISION_NO") , &strValue);
				char *pData = NULL;
				long lSize = 0L;
				adoDB.GetFieldSize(0 , _T("C_DATA") , lSize);
				if(pData = (char*)calloc(1 , sizeof(char)*lSize))
				{
					LPSECURITY_ATTRIBUTES attr;
					attr = NULL;

					const CString rRevPath = CCompareRevisionDataDoc::GetRevPath(_T("Cable Sizing Table") , strValue.c_str());
					CreateFolderIfNeed(rRevPath.operator LPCSTR());

					adoDB.GetBinaryValue(0 , _T("C_DATA") , (LPVOID)pData , lSize);
					CFile f(rRevPath + _T("\\REV.ZIP") , CFile::modeCreate|CFile:: modeReadWrite);
					f.Write(pData , lSize);
					f.Close();

					CCompareRevisionDataDoc::UncompressRevFile(rRevPath + _T("\\REV.ZIP") , rRevPath);
				}

				if(pData)
				{
					free((void*)pData);
					pData = NULL;
				}
			}
		}
		else
		{
			AfxMessageBox(_T("Fail to connect database"));
		}
	}
	else if(!rString.IsEmpty())
	{
		m_rFolderPath = rProjectPath + _T("Cable Sizing Table");
		m_rFilePath = (rString.MakeUpper()).operator LPCSTR();
	}
	
	return TRUE;
}

/**
	@brief	Product Output Folder가 생성되지 않았다면 폴더를 생성한다.

	@author BHK	

	@date 2009-05-07 오후 1:51:14	

	@param	

	@return		
*/
int CCableSizingTableDoc::CreateFolderIfNeed(const string& rFolderPath)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string rInterestingPath = (rFolderPath.empty()) ? 
		(docData.GetProjectFolderPath() + _T("Cable Sizing Result")) : rFolderPath;
	
	if(FALSE == ::PathIsDirectory(rInterestingPath.c_str()))
	{
		CreateDirectory(rInterestingPath.c_str() , NULL);
	}

	return 0;
}