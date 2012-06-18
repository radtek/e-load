// ViewRevisionDoc.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "ELoadDocData.h"
#include "CompareRevisionDataDoc.h"
#include <Zip\inc\ZipArchive.h>

// CCompareRevisionDataDoc

IMPLEMENT_DYNCREATE(CCompareRevisionDataDoc, CDocument)

CCompareRevisionDataDoc::CCompareRevisionDataDoc() : m_nLeftSideRevNo(0) , m_nRightSideRevNo(0)
{
}

BOOL CCompareRevisionDataDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CCompareRevisionDataDoc::~CCompareRevisionDataDoc()
{
	try
	{
		for(map<string,CompareRevisionDataSheet*>::iterator itr = m_CompareRevisionDataSheetMap.begin();itr != m_CompareRevisionDataSheetMap.end();++itr)
		{
			SAFE_DELETE(itr->second);
		}
	}
	catch(...)
	{
	}
}


BEGIN_MESSAGE_MAP(CCompareRevisionDataDoc, CDocument)
END_MESSAGE_MAP()


// CCompareRevisionDataDoc diagnostics

#ifdef _DEBUG
void CCompareRevisionDataDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CCompareRevisionDataDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CCompareRevisionDataDoc serialization

void CCompareRevisionDataDoc::Serialize(CArchive& ar)
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


// CCompareRevisionDataDoc commands

/**
	@brief	

	@author BHK	

	@date 2009-05-06 오전 10:55:09

	@param	

	@return	
*/
BOOL CCompareRevisionDataDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	CCompareRevisionDataDoc::COMPARE_ITEM* pCompareItem = (CCompareRevisionDataDoc::COMPARE_ITEM*)(lpszPathName);
	if(pCompareItem)
	{
		m_rProductName   = pCompareItem->rProductName;
		m_rProductName.MakeUpper();	//! 2011.02.18
		m_nLeftSideRevNo = pCompareItem->nLeftSideRevNo;
		m_nRightSideRevNo= pCompareItem->nRightSideRevNo;
		//! left side rev. no가 최근의 REV NO이어야 한다.
		if(m_nLeftSideRevNo < m_nRightSideRevNo) swap(m_nLeftSideRevNo , m_nRightSideRevNo);

		CELoadDocData& docData = CELoadDocData::GetInstance();
		const string rMDBFilePath = docData.GetProjectMDBFilePath();

		CADODB adoDB;
		const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;
		if(TRUE == adoDB.DBConnect(rConnectionString))
		{
			CString rFormat;
			if((-1 != m_nLeftSideRevNo) && (-1 != m_nRightSideRevNo))
			{
				rFormat = (_T("SELECT * FROM T_PRODUCT_REVISION WHERE C_PRODUCT_NAME = '%s' AND (C_REVISION_NO = %d OR C_REVISION_NO = %d)"));

				CString rSQL;
				rSQL.Format(rFormat , m_rProductName , m_nLeftSideRevNo , m_nRightSideRevNo);
				adoDB.OpenQuery(rSQL.operator LPCTSTR());
				LONG lRecordCount = 0L;
				adoDB.GetRecordCount(&lRecordCount);
				if(2 == lRecordCount)
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
						const CString rRevPath = GetRevPath(m_rProductName , strValue.c_str());
						CreateOrClearFolder(rRevPath);
						::CreateDirectory(rRevPath , attr);

						adoDB.GetBinaryValue(0 , _T("C_DATA") , (LPVOID)pData , lSize);
						CFile f(rRevPath + _T("\\REV.ZIP") , CFile::modeCreate|CFile:: modeReadWrite);
						f.Write(pData , lSize);
						f.Close();

						UncompressRevFile(rRevPath + _T("\\REV.ZIP") , rRevPath);
					}

					adoDB.GetFieldValue(1 , _T("C_REVISION_NO") , &strValue);
					adoDB.GetFieldSize(1 , _T("C_DATA") , lSize);
					if(pData = (char*)realloc(pData , sizeof(char)*lSize))
					{
						LPSECURITY_ATTRIBUTES attr;
						attr = NULL;
						const CString rRevPath = GetRevPath(m_rProductName , strValue.c_str());
						::RemoveDirectory(rRevPath);
						::CreateDirectory(rRevPath , attr);

						adoDB.GetBinaryValue(1 , _T("C_DATA") , (LPVOID)pData , lSize);
						CFile f(rRevPath + _T("\\REV.ZIP") , CFile::modeCreate|CFile:: modeReadWrite);
						f.Write(pData , lSize);
						f.Close();

						UncompressRevFile(rRevPath + _T("\\REV.ZIP") , rRevPath);
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
				//! Not indicate specific revision no's of left and right.
				if((_T("LOAD SUMMARY-BUS") == m_rProductName) || (_T("LOAD SUMMARY-SUBBUS") == m_rProductName))
				{
					CString rFolderPath = string(docData.GetProjectFolderPath() + _T("Load Summary Result")).c_str();
					CString rLeftFolderPath = GetLeftSideFolderPath();
					if(_T("\\") == rLeftFolderPath.Right(1)) rLeftFolderPath = rLeftFolderPath.Left(rLeftFolderPath.GetLength() - 1);
					CreateOrClearFolder(rLeftFolderPath);
					::CreateDirectory(rLeftFolderPath , NULL);
					if(_T("LOAD SUMMARY-BUS") == m_rProductName)
					{
						UncompressRevFile(rFolderPath + _T("\\BUS_LoadSummary.zip") , rLeftFolderPath);
					}
					else if(_T("LOAD SUMMARY-SUBBUS") == m_rProductName)
					{
						UncompressRevFile(rFolderPath + _T("\\SubBUS_LoadSummary.zip") , rLeftFolderPath);
					}
				}
				
				rFormat = (_T("SELECT * FROM T_PRODUCT_REVISION WHERE C_PRODUCT_NAME = '%s' ORDER BY C_REVISION_NO DESC"));

				CString rSQL;
				rSQL.Format(rFormat , m_rProductName);
				adoDB.OpenQuery(rSQL.operator LPCTSTR());
				LONG lRecordCount = 0L;
				adoDB.GetRecordCount(&lRecordCount);
				if(lRecordCount > 0)
				{
					STRING_T strValue;
					adoDB.GetFieldValue(0 , _T("C_REVISION_NO") , &strValue);
					//! assign C_REVISION_NO value to m_nRightSideRevNo.
					m_nRightSideRevNo = atoi(strValue.c_str());

					char *pData = NULL;
					long lSize = 0L;
					adoDB.GetFieldSize(0 , _T("C_DATA") , lSize);
					if((lSize > 0L) && (pData = (char*)calloc(1 , sizeof(char)*lSize)))
					{
						//! create a folder if not exist.
						LPSECURITY_ATTRIBUTES attr;
						attr = NULL;
						const CString rRevPath = GetRevPath(m_rProductName , strValue.c_str());
						CreateOrClearFolder(rRevPath);
						::CreateDirectory(rRevPath , attr);

						adoDB.GetBinaryValue(0 , _T("C_DATA") , (LPVOID)pData , lSize);
						CFile f(rRevPath + _T("\\REV.ZIP") , CFile::modeCreate|CFile:: modeReadWrite);
						f.Write(pData , lSize);
						f.Close();

						UncompressRevFile(rRevPath + _T("\\REV.ZIP") , rRevPath);

						if(pData)
						{
							free((void*)pData);
							pData = NULL;
						}
					}
					else
					{
						::MessageBox(NULL, _T("Revision Data가 존재하지 않습니다.") , _T("Warning") , MB_OK|MB_ICONWARNING);
						return FALSE;
					}
				}
				else
				{
					::MessageBox(NULL, "Revision Data Record가 존재하지 않습니다.", "Warning" , MB_OK|MB_ICONWARNING);
					return FALSE;
				}
			}
		}
		else
		{
			AfxMessageBox(_T("Fail to connect database"));
		}
	}

	return TRUE;
}

/**
	@brief	

	@author HumKyung	

	@date 2009-05-06 오전 11:42:44	

	@param	

	@return		
*/
CString CCompareRevisionDataDoc::GetRevPath(const CString& rProductName , const CString& rRevNo)
{
	CString rRevPath , _sProductName(rProductName);
	_sProductName.MakeUpper();

	CELoadDocData& docData = CELoadDocData::GetInstance();
	if(_T("LOAD SUMMARY-BUS") == _sProductName)
	{
		rRevPath = CString(docData.GetProjectFolderPath().c_str()) + _T("Load Summary Result\\BUS_") + rRevNo;
	}
	else if(_T("LOAD SUMMARY-SUBBUS") == _sProductName)
	{
		rRevPath = CString(docData.GetProjectFolderPath().c_str()) + _T("Load Summary Result\\SubBUS_") + rRevNo;
	}
	else if(_T("TRANSFORMER SIZING") == _sProductName)
	{
		if(_T("-1") == rRevNo)
			rRevPath = CString(docData.GetProjectFolderPath().c_str()) + _T("Transformer Sizing");
		else	rRevPath = CString(docData.GetProjectFolderPath().c_str()) + _T("Transformer Sizing\\") + rRevNo;
	}
	else if(_T("AUTO POWER CABLE") == _sProductName)
	{
		if(_T("-1") == rRevNo)
			rRevPath = CString(docData.GetProjectFolderPath().c_str()) + _T("Cable Sizing Result");
		else	rRevPath = CString(docData.GetProjectFolderPath().c_str()) + _T("Cable Sizing Result\\") + rRevNo;
	}
	else if(_T("AUTO CONTROL CABLE") == _sProductName)
	{
		if(_T("-1") == rRevNo)
			rRevPath = CString(docData.GetProjectFolderPath().c_str()) + _T("Control Cable Result");
		else	rRevPath = CString(docData.GetProjectFolderPath().c_str()) + _T("Control Cable Result\\") + rRevNo;
	}
	else if(_T("CABLE SIZING TABLE") == _sProductName)
	{
		rRevPath = CString(docData.GetProjectFolderPath().c_str()) + _T("Cable Sizing Table\\") + rRevNo;
	}
	else if(_T("CABLE SCHEDULE") == _sProductName)
	{
		rRevPath = CString(docData.GetProjectFolderPath().c_str()) + _T("Cable Schedule Result\\") + rRevNo;
	}
	else if(_T("DRUM SCHEDULE") == _sProductName)
	{
		rRevPath = CString(docData.GetProjectFolderPath().c_str()) + _T("Drum Schedule Result\\") + rRevNo;
	}
	
	return rRevPath;
}

/**
	@brief	주어진 REV. 파일을 해당 폴더에 압축을 푼다.

	@author BHK	

	@date 2009-05-06 오후 1:11:43	

	@param	

	@return		
*/
int CCompareRevisionDataDoc::UncompressRevFile(const CString& rRevFilePath, const CString& rFolder)
{
	CZipArchive zip;
	zip.Open(rRevFilePath , CZipArchive::open);
	const int nEntries = zip.GetNoEntries();
	for(int i = 0;i < nEntries;++i)
	{
		zip.ExtractFile(i,rFolder);
	}
	zip.Close();

	return ERROR_SUCCESS;
}

/**
	@brief	sheet에서 row data에 해당하는 부분을 찾아 2개의 row data를 비교하여 그 결과값을 리턴한다.

	@author BHK	

	@date 2009-05-21 오전 10:59:58	

	@param	CompareResult
	@param	lhs
	@param	rhs

	@return	if found matching line, matching되는 row, otherwise return -1.
*/
int CCompareRevisionDataDoc::CompareRevisionData(vector<int>& CompareResult, const CompareRevisionDataSheet& lhs, CompareRevisionRowData& rhs)
{
	CompareResult.clear();

	int nRow = 1;
	for(vector<CompareRevisionRowData>::const_iterator itr = lhs.begin();itr != lhs.end();++itr,++nRow)
	{
		if(itr->key == rhs.key)
		{
			int nIndex = 0;
			for(vector<string>::const_iterator jtr = itr->cols.begin();jtr != itr->cols.end();++jtr,++nIndex)
			{
				//! 동일한 값이면 0으로 한다.
				CompareResult.push_back((rhs.cols.size() > nIndex) ? ((*jtr) != rhs.cols[nIndex]) : 1);
			}

			return nRow;
		}
	}

	return -1;
}

/**
	@brief	

	@author	BHK

	@date	2009-10-20

	@param

	@return
*/
CString CCompareRevisionDataDoc::GetLeftSideFolderPath(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CString strFolderPath = docData.GetProjectFolderPath().c_str();

	if(-1 != m_nLeftSideRevNo)
	{
		CString rLeftSideRevNo;
		rLeftSideRevNo.Format(_T("%d") , m_nLeftSideRevNo);
		strFolderPath = CCompareRevisionDataDoc::GetRevPath(m_rProductName , rLeftSideRevNo) + _T("\\");
	}
	else
	{
		if((_T("LOAD SUMMARY-BUS") == m_rProductName) || (_T("LOAD SUMMARY-SUBBUS") == m_rProductName))
		{
			strFolderPath = string(docData.GetProjectFolderPath() + _T("Load Summary Result\\Current\\")).c_str();
		}
		else
		{
			CString rLeftSideRevNo;
			rLeftSideRevNo.Format(_T("%d") , m_nLeftSideRevNo);
			strFolderPath = CCompareRevisionDataDoc::GetRevPath(m_rProductName , rLeftSideRevNo) + _T("\\");
		}
	}

	return strFolderPath;
}

/**
	@brief	

	@author	BHK

	@date	2009-10-20

	@param

	@return
*/
CString CCompareRevisionDataDoc::GetRightSideFolderPath(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CString strFolderPath = docData.GetProjectFolderPath().c_str();

	if(-1 != m_nRightSideRevNo)
	{
		CString rRightSideRevNo;
		rRightSideRevNo.Format(_T("%d") , m_nRightSideRevNo);
		strFolderPath = CCompareRevisionDataDoc::GetRevPath(m_rProductName , rRightSideRevNo) + _T("\\");
	}
	else
	{
		if((_T("LOAD SUMMARY-BUS") == m_rProductName) || (_T("LOAD SUMMARY-SUBBUS") == m_rProductName))
		{
			strFolderPath = string(docData.GetProjectFolderPath() + _T("Load Summary Result\\Current\\")).c_str();
		}
		else
		{
			CString rLeftSideRevNo;
			rLeftSideRevNo.Format(_T("%d") , m_nLeftSideRevNo);
			strFolderPath = CCompareRevisionDataDoc::GetRevPath(m_rProductName , rLeftSideRevNo) + _T("\\");
		}
	}

	return strFolderPath;
}
