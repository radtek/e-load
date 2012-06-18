// LoadSummaryDoc.cpp : implementation file
//

#include "stdafx.h"
#include "ELoad.h"
#include "LoadSummaryDoc.h"
#include "ELoadDocData.h"
#include "CompareRevisionDataDoc.h"

// CLoadSummaryDoc

IMPLEMENT_DYNCREATE(CLoadSummaryDoc, CDocument)

CLoadSummaryDoc::CLoadSummaryDoc()
{
}

BOOL CLoadSummaryDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CLoadSummaryDoc::~CLoadSummaryDoc()
{
}


BEGIN_MESSAGE_MAP(CLoadSummaryDoc, CDocument)
END_MESSAGE_MAP()


// CLoadSummaryDoc diagnostics

#ifdef _DEBUG
void CLoadSummaryDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CLoadSummaryDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CLoadSummaryDoc serialization

void CLoadSummaryDoc::Serialize(CArchive& ar)
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


// CLoadSummaryDoc commands

/**
	@brief	

	@author BHK	

	@date 2009-04-08 ¿ÀÀü 10:16:04	

	@param	

	@return		
*/
BOOL CLoadSummaryDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	CString rString(lpszPathName);
	if((0 == rString.Find(_T("ALL REV BUS LOAD SUMMARY"))) || (0 == rString.Find(_T("ALL REV PDB LOAD SUMMARY")))) 
	{
		vector<string> oResult;
		CTokenizer<CIsFromString>::Tokenize(oResult , rString.operator LPCSTR() , CIsFromString("|"));
		if(2 == oResult.size())
		{
			m_rLoadSummaryType = oResult[0];
			m_rLoadSummaryRevNo= oResult[1];

			CELoadDocData& docData = CELoadDocData::GetInstance();
			const string rMDBFilePath = docData.GetProjectMDBFilePath();

			CADODB adoDB;
			const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;
			if(TRUE == adoDB.DBConnect(rConnectionString))
			{
				const CString rFormat(_T("SELECT * FROM T_PRODUCT_REVISION WHERE C_PRODUCT_NAME = '%s' AND C_REVISION_NO = %s"));
				
				CString rSQL;
				if(_T("ALL REV BUS LOAD SUMMARY") == m_rLoadSummaryType)
				{
					m_rProductName = _T("Load Summary-BUS");
					rSQL.Format(rFormat , m_rProductName.c_str() , m_rLoadSummaryRevNo.c_str());
				}
				else if(_T("ALL REV PDB LOAD SUMMARY") == m_rLoadSummaryType)
				{
					m_rProductName = _T("Load Summary-SubBUS");
					rSQL.Format(rFormat , m_rProductName.c_str() , m_rLoadSummaryRevNo.c_str());
				}

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
					if((lSize > 0) && (pData = (char*)calloc(1 , sizeof(char)*lSize)))
					{
						LPSECURITY_ATTRIBUTES attr;
						attr = NULL;
						
						const CString rRevPath = CCompareRevisionDataDoc::GetRevPath(m_rProductName.c_str() , strValue.c_str());
						::CreateDirectory(rRevPath , attr);

						adoDB.GetBinaryValue(0 , _T("C_DATA") , (LPVOID)pData , lSize);
						CFile f(rRevPath + _T("\\REV.ZIP") , CFile::modeCreate|CFile:: modeReadWrite);
						f.Write(pData , lSize);
						f.Close();

						CCompareRevisionDataDoc::UncompressRevFile(rRevPath + _T("\\REV.ZIP") , rRevPath);

						//! save folder path
						m_rFolderPath = rRevPath.operator LPCTSTR();

						if(pData)
						{
							free((void*)pData);
							pData = NULL;
						}
					}
					else
					{
						AfxMessageBox(_T("Has no revision contents."));
						return FALSE;
					}
				}
			}
			else
			{
				AfxMessageBox(_T("Fail to connect database"));
			}
		}
	}
	else
	{
		m_rLoadSummaryType = lpszPathName;
		
		CELoadDocData& docData = CELoadDocData::GetInstance();
		string rLoadSummaryExcelFilePath = docData.GetProjectFolderPath().c_str();
		if('\\' != rLoadSummaryExcelFilePath.at(rLoadSummaryExcelFilePath.length() - 1)) rLoadSummaryExcelFilePath += "\\";
		m_rLoadSummaryExcelFilePath = rLoadSummaryExcelFilePath + docData.GetProjectNo() + _T(".XLS");

		//! save folder path
		m_rFolderPath = docData.GetProjectFolderPath() + _T("Load Summary Result");
	}
	
	return TRUE;
}
