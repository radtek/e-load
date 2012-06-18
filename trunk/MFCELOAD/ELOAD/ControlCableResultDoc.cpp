// ControlCableResultDoc.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "ControlCableResultDoc.h"
#include "ELoadDocData.h"
#include "WorkStatusDlg.h"
#include "ELoadDocData.h"
#include "CompareRevisionDataDoc.h"
#include <util/FileTools.h>

list<ITEMS::CELoadItem*> CControlCableResultDoc::m_InterestingLoadItemList;
// CControlCableResultDoc

IMPLEMENT_DYNCREATE(CControlCableResultDoc, CDocument)

CControlCableResultDoc::CControlCableResultDoc()
{
}

BOOL CControlCableResultDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CControlCableResultDoc::~CControlCableResultDoc()
{
}


BEGIN_MESSAGE_MAP(CControlCableResultDoc, CDocument)
END_MESSAGE_MAP()


// CControlCableResultDoc diagnostics

#ifdef _DEBUG
void CControlCableResultDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CControlCableResultDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CControlCableResultDoc serialization

void CControlCableResultDoc::Serialize(CArchive& ar)
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


// CControlCableResultDoc commands

BOOL CControlCableResultDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	//if (!CDocument::OnOpenDocument(lpszPathName))
	//	return FALSE;
	
	const CString rString(CString(lpszPathName).MakeUpper());
	
	if((_T("CREATE CONTROL CABLE") == rString) || (_T("SHOW CONTROL CABLE") == rString))
	{
		//! CONTROL CABLE DATA를 TEXT 파일에 쓴다.
		WriteControlCableDataInTextFile();
		
		CELoadDocData& docData = CELoadDocData::GetInstance();
		const string rProjectPath = docData.GetProjectFolderPath();
		CString rInterestingFilePath = rProjectPath.c_str() + CString("Control Cable Result\\");
		m_rRevFolderPath = rInterestingFilePath;
	}
	else if(0 == rString.Find(_T("REV CONTROL CABLE RESULT")))
	{
		vector<string> oResult;
		CTokenizer<CIsFromString>::Tokenize(oResult , rString.operator LPCSTR() , CIsFromString("|"));
		if(2 == oResult.size())
		{
			const string rRevNo= oResult[1];

			CELoadDocData& docData = CELoadDocData::GetInstance();
			const string rMDBFilePath = docData.GetProjectMDBFilePath();

			CADODB adoDB;
			const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;
			if(TRUE == adoDB.DBConnect(rConnectionString))
			{
				const CString rFormat(_T("SELECT * FROM T_PRODUCT_REVISION WHERE C_PRODUCT_NAME = 'Auto Control Cable' AND C_REVISION_NO = %s"));

				CString rSQL;
				rSQL.Format(rFormat , rRevNo.c_str());

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

						const CString rRevPath = CCompareRevisionDataDoc::GetRevPath(_T("Auto Control Cable") , strValue.c_str());
						CreateFolderIfNeed(rRevPath.operator LPCSTR());

						adoDB.GetBinaryValue(0 , _T("C_DATA") , (LPVOID)pData , lSize);
						CFile f(rRevPath + _T("\\REV.ZIP") , CFile::modeCreate|CFile:: modeReadWrite);
						f.Write(pData , lSize);
						f.Close();

						CCompareRevisionDataDoc::UncompressRevFile(rRevPath + _T("\\REV.ZIP") , rRevPath);
						m_rRevFolderPath = rRevPath  + _T("\\");
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
	}

	return TRUE;
}

/**
	@brief	

	@author KHS	

	@date 2009-08-20 오전 10:55:32	

	@param	

	@return		
**/
int CControlCableResultDoc::CreateFolderIfNeed(const string& rFolderPath)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string rInterestingPath = (rFolderPath.empty()) ? 
		(docData.GetProjectFolderPath() + _T("Control Cable Result")) : rFolderPath;

	if(FALSE == ::PathIsDirectory(rInterestingPath.c_str()))
	{
		CreateDirectory(rInterestingPath.c_str() , NULL);
	}

	return 0;
}

/**
	@brief	

	@author KHS	

	@date 2009-06-10 오전 9:21:09	

	@param	

	@return		
**/
UINT CControlCableResultDoc::StatusWriteTextThreadEntry(LPVOID pVoid)
{
	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
	if(pDlg)
	{
		CControlCableResultDoc* p = (CControlCableResultDoc*)pVoid;
		if(p) p->StatusWriteTextThread();

		InterlockedExchange((LONG*)(&(pDlg->m_bThreadRunning)) , FALSE);
		if(pDlg) pDlg->PostMessage(WM_COMMAND , IDOK);
	}

	return ERROR_SUCCESS;
}

//! Cable Item을 LOAD,CABLE 순에 의거하여 SORTING한다.
static bool SortCableItemFunc(CELoadItem* lhs , CELoadItem* rhs)
{
	CCableItem* _lhs = static_cast<CCableItem*>(lhs);
	CCableItem* _rhs = static_cast<CCableItem*>(rhs);

	CLoadItem* pLhsLoadItem = _lhs->GetLoadItemPtr();
	CLoadItem* pRhsLoadItem = _rhs->GetLoadItemPtr();
	if(pLhsLoadItem && pRhsLoadItem)
	{
		const string rLhsLoadId = pLhsLoadItem->GetName();
		const string rRhsLoadId = pRhsLoadItem->GetName();

		const string rLhsCableId = lhs->GetName();
		const string rRhsCableId = rhs->GetName();

		if(rLhsLoadId == rRhsLoadId)
		{
			return (rLhsCableId < rRhsCableId);
		}else	return (rLhsLoadId < rRhsLoadId);
	}

	return false;
}

/**
	@brief	Control cable Data를 TEXT 파일에 쓴다.

	@author KHS	

	@date 2009-06-10 오전 9:17:50	

	@param	

	@return		
**/
int CControlCableResultDoc::StatusWriteTextThread()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	double progress = 0.;
	int nIndex = 0;

	const string rProjectPath = docData.GetProjectFolderPath();
	CString rInterestingFilePath = rProjectPath.c_str() + CString("Control Cable Result\\");
	
	///m_rRevFolderPath = rInterestingFilePath;

	//! 폴더가 존재하지 않으면 폴더 생성.
	CFileFind dirFind;
	if(0 == dirFind.FindFile(rInterestingFilePath.Left(rInterestingFilePath.GetLength() - 1)))
	{
		CreateDirectory(rInterestingFilePath , NULL);
	}

	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();

	CString rTitle = _T("Write Control Cable...");

	list<CELoadItem*> BusItemList;
	docData.GetELoadItemListOf(BusItemList , CBusItem::TypeString());
	
	const int nTotalCount = count_if(BusItemList.begin() , BusItemList.end() , CELoadItem::IsNotDeleted);
	for(list<CELoadItem*>::iterator itr = BusItemList.begin(); itr != BusItemList.end();++itr)
	{
		if((*itr)->IsDeleted()) continue;

		const string rBusID = (*itr)->GetName();
		CBusItem *pBusItem = static_cast<CBusItem*>(*itr);
		//! if(_T("INCOMING") == pBusItem->GetBusGroupName()) continue;
		
		CString rFirstLoad, rSecondLoad;
		CItemSelectionSet SelectionSet;
		docData.GetLoadItemSetInBus(SelectionSet , rBusID);

		int nNo = 0;
		ofstream ofile(rInterestingFilePath + rBusID.c_str() + _T(".BUS"));
		if(ofile.is_open())
		{
			for(int i = 0;i < SelectionSet.GetItemCount();++i)
			{
				list<CELoadItem*>::iterator where = find(CControlCableResultDoc::m_InterestingLoadItemList.begin() , CControlCableResultDoc::m_InterestingLoadItemList.end() , SelectionSet.GetItemAt(i));
				if(where == CControlCableResultDoc::m_InterestingLoadItemList.end()) continue;
				CLoadItem* pLoadItem = static_cast<CLoadItem*>(SelectionSet.GetItemAt(i));
				///if((*jtr)->IsDeleted()) continue;

				const string rLoadId = pLoadItem->GetName();
				const string rLosdBusId = pLoadItem->prop()->GetValue(_T("Panel"), _T("Bus ID"));
				///if(rLosdBusId == rBusID)
				{
					///const string rVolt   = pLoadItem->prop()->GetValue(_T("Rating"), _T("Rated Voltage"));
					///const string rRating = pLoadItem->prop()->GetValue(_T("Rating"), _T("Rating Capacity"));
					///const string rUnit = pLoadItem->prop()->GetUnitStr(_T("Rating"), _T("Rating Capacity"));
					///const string rSchematicType = pLoadItem->prop()->GetValue(_T("Control Cable"), _T("Schematic Ty"));

					list<CELoadItem*> CableItemList;
					CableItemList.insert(CableItemList.begin() , pLoadItem->m_CableRefEntry.begin() , pLoadItem->m_CableRefEntry.end());
					///docData.GetELoadItemListOf(CableItemList , CCableItem::TypeString());
					stable_sort(CableItemList.begin() , CableItemList.end() , SortCableItemFunc);

					for(list<CELoadItem*>::iterator ktr = CableItemList.begin(); ktr != CableItemList.end();++ktr)
					{
						if((*ktr)->IsDeleted()) continue;

						const string rCableId = (*ktr)->GetName();
						const string rCategory = (*ktr)->prop()->GetValue(_T("General"), _T("Cable Category"));
						const string rCableLoadId = (*ktr)->prop()->GetValue(_T("General"), _T("Load Id"));
						if((rCategory == _T("Control Cable")) && (rLoadId == rCableLoadId))
						{
							const string rVolt   = (*ktr)->prop()->GetValue(_T("General"), _T("Load Volt"));
							const string rRating = (*ktr)->prop()->GetValue(_T("General"), _T("Rating Capacity"));
							const string rUnit = (*ktr)->prop()->GetValue(_T("General"), _T("Rating Capacity Unit"));
							const string rSchematicType = (*ktr)->prop()->GetValue(_T("General"), _T("Schematic Ty"));

							const string rDesc = (*ktr)->prop()->GetValue(_T("General") ,_T("Description"));
							const string rType = (*ktr)->prop()->GetValue(_T("General") ,_T("Cable Spec"));
							const string rCore = (*ktr)->prop()->GetValue(_T("General") ,_T("Core"));
							const string rSize = (*ktr)->prop()->GetValue(_T("General") ,_T("Size"));
							const string rLength = (*ktr)->prop()->GetValue(_T("General") ,_T("Length"));

							rFirstLoad = rLoadId.c_str();

							if(!rSecondLoad.IsEmpty() && rFirstLoad != rSecondLoad)
							{
								ofile << "\n";
							}

							rSecondLoad = rFirstLoad;

							++nNo;
							CString rNo;
							rNo.Format("%d", nNo);
							ofile << "\t" + rNo + "\t" << rLoadId + "\t" << rVolt + "\t" << rRating + "\t" << rUnit + "\t" << rSchematicType + "\t";
							ofile << rCableId + "\t" << rDesc + "\t" << rType + "\t" << rCore + "\t" << rSize + "\t" << rLength+ "\t" << endl;

						}
					}
				}
			}
			ofile.close();
		}
		int progress = int((double(nIndex) / double(nTotalCount))*100.);
		if(pDlg) pDlg->UpdateWorkStatus(rTitle , progress);
		++nIndex;
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author KHS	

	@date 2009-06-09 오후 5:33:31	

	@param	

	@return		
**/

int CControlCableResultDoc::WriteControlCableDataInTextFile(void)
{
	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
	if(NULL == pDlg)
	{
		CWorkStatusDlg dlg;
		dlg.m_pThread = AfxBeginThread(CControlCableResultDoc::StatusWriteTextThreadEntry , this , THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
		if(NULL != dlg.m_pThread)
		{
			InterlockedExchange((LONG*)(&(dlg.m_bThreadRunning)) , TRUE);
			dlg.DoModal();	

			return ERROR_SUCCESS;
		}
	}

	return ERROR_BAD_ENVIRONMENT;
}
