// LoadListSldDoc.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "LoadListSldDoc.h"
#include "LoadListSldView.h"
#include "ELoadDocData.h"
#include "LoadItem.h"
#include "CompareRevisionDataDoc.h"
#include "WorkStatusDlg.h"
#include <sstream>

IMPLEMENT_DYNCREATE(CLoadListSldDoc, CDocument)

CLoadListSldDoc::CLoadListSldDoc()
{
}

BOOL CLoadListSldDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CLoadListSldDoc::~CLoadListSldDoc()
{
}


BEGIN_MESSAGE_MAP(CLoadListSldDoc, CDocument)
END_MESSAGE_MAP()


// CLoadListSldDoc diagnostics

#ifdef _DEBUG
void CLoadListSldDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CLoadListSldDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CLoadListSldDoc serialization

void CLoadListSldDoc::Serialize(CArchive& ar)
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


// CLoadListSldDoc commands

BOOL CLoadListSldDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	CreateFolderIfNeed();

	CELoadDocData& docData = CELoadDocData::GetInstance();
	vector<string> oResult;
	CTokenizer<CIsFromString>::Tokenize(oResult , lpszPathName , CIsFromString(_T("|")));
	if(1 == oResult.size())
	{
		m_rFolderPath = (docData.GetProjectFolderPath() + _T("Load List Sld Result\\")).c_str();
		WriteLoadListSldDataInText();
	}
	else if(2 == oResult.size())
	{
		/*CELoadDocData& docData = CELoadDocData::GetInstance();
		const CString strProjectPath = docData.GetProjectFolderPath().c_str();
		::DeleteFile(strProjectPath + _T("Working\\Load List Sld Result.txt"));

		const string rMDBFilePath = docData.GetProjectMDBFilePath();

		CADODB adoDB;
		CString rConnectionString = CString("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=") + rMDBFilePath.c_str() + DB_PASSWORD;
		if(TRUE == adoDB.DBConnect(rConnectionString))
		{
			const CString rSql(_T("SELECT * FROM T_PRODUCT_REVISION WHERE C_PRODUCT_NAME = 'Load List Sld' AND C_REVISION_NO = ") + CString(oResult[1].c_str()));

			adoDB.OpenQuery(rSql);
			LONG lRecordCount = 0L;
			adoDB.GetRecordCount(&lRecordCount);
			if(1 == lRecordCount)
			{
				CString strValue;
				adoDB.GetFieldValue(0 , _T("C_REVISION_NO") , &strValue);
				char *pData = NULL;
				long lSize = 0L;
				adoDB.GetFieldSize(0 , _T("C_DATA") , lSize);
				if(pData = (char*)calloc(1 , sizeof(char)*lSize))
				{
					const CString rRevPath = CCompareRevisionDataDoc::GetRevPath(_T("Load List Sld") , strValue);
					if(FALSE == ::PathIsDirectory(rRevPath)) CreateDirectory(rRevPath , NULL);

					adoDB.GetBinaryValue(0 , _T("C_DATA") , (LPVOID)pData , lSize);
					CFile f(rRevPath + _T("\\REV.ZIP") , CFile::modeCreate|CFile:: modeReadWrite);
					f.Write(pData , lSize);
					f.Close();

					CCompareRevisionDataDoc::UncompressRevFile(rRevPath + _T("\\REV.ZIP") , rRevPath);
					m_rFolderPath = rRevPath + _T("\\");
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
		}*/
	}

	return TRUE;
}

/**
	@brief	

	@author KHS	

	@date 2009-11-06 오후 3:11:24	

	@param	

	@return		
**/
int CLoadListSldDoc::CreateFolderIfNeed(const string& rFolderPath)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string rInterestingPath = (rFolderPath.empty()) ? 
		(docData.GetProjectFolderPath() + _T("Load List Sld Result")) : rFolderPath;

	if(FALSE == ::PathIsDirectory(rInterestingPath.c_str()))
	{
		CreateDirectory(rInterestingPath.c_str() , NULL);
	}

	return 0;
}


/**
	@brief	

	@author KHS	

	@date 2009-11-06 오후 3:11:29	

	@param	

	@return		
**/
int CLoadListSldDoc::WriteLoadListSldDataInText(void)
{
	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
	if(NULL == pDlg)
	{
		CWorkStatusDlg dlg;
		dlg.m_pThread = AfxBeginThread(CLoadListSldDoc::StatusWriteTextThreadEntry , this , THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
		if(NULL != dlg.m_pThread)
		{
			InterlockedExchange((LONG*)(&(dlg.m_bThreadRunning)) , TRUE);
			dlg.DoModal();	

			return ERROR_SUCCESS;
		}
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author KHS	

	@date 2009-11-06 오후 3:11:33	

	@param	

	@return		
**/
UINT CLoadListSldDoc::StatusWriteTextThreadEntry(LPVOID pVoid)
{
	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
	if(pDlg)
	{
		CLoadListSldDoc* p = (CLoadListSldDoc*)pVoid;
		if(p) p->StatusWriteTextThread();

		InterlockedExchange((LONG*)(&(pDlg->m_bThreadRunning)) , FALSE);
		if(pDlg) pDlg->PostMessage(WM_COMMAND , IDOK);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author KHS	

	@date 2009-09-28 오후 4:25:39	

	@param	

	@return		
**/
int CLoadListSldDoc::StatusWriteTextThread()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	double progress = 0.;
	int nIndex = 0;

	const string rProjectPath = docData.GetProjectFolderPath();
	CString rFilePath = rProjectPath.c_str() + CString(_T("Load List Sld Result\\%s.txt"));

	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();

	CString rTitle = _T("Write Load List Sld...");
	
	list<CELoadItem*> BusItemList;
	docData.GetELoadItemListOf(BusItemList, CBusItem::TypeString());
	
	const int nTotalCount = BusItemList.size();

	for(list<CELoadItem*>::iterator itr = BusItemList.begin(); itr != BusItemList.end();++itr)
	{
		const string rBusID = (*itr)->GetName();
		CItemSelectionSet SelectionSet;
		docData.GetLoadItemSetInBus(SelectionSet, rBusID);
		
		CString rBusText;
		rBusText.Format(rFilePath, rBusID.c_str());
		ofstream ofile(rBusText);
		if(ofile.is_open())
		{
			int nCount = 1;
			for(int i = 0;i < SelectionSet.GetItemCount();++i)
			{
				CLoadItem* pLoadItem = static_cast<CLoadItem*>(SelectionSet.GetItemAt(i));
				if(pLoadItem)
				{
					CELoadItemProp *pItemProp = pLoadItem->prop();
					if(pItemProp)
					{
						const string rLoadID = pItemProp->GetValue(_T("General"),_T("ITEM ID"));
						const string rDesc = pItemProp->GetValue(_T("General"),_T("Description"));
						const string rRating = pItemProp->GetValue(_T("Rating"),_T("Rating Capacity"));
						CString rDuty = pItemProp->GetValue(_T("Type"),_T("Operation Mode")).c_str();
						const string rAF = pItemProp->GetValue(_T("MCCB"),_T("AF"));
						const string rAT = pItemProp->GetValue(_T("MCCB"),_T("AT"));

						const string rSchematicType = pItemProp->GetValue(_T("Control Cable"),_T("Schematic Ty"));
						
						if(!rDuty.IsEmpty())
						{
							rDuty = rDuty.Left(1);
						}

						CString rIndex;
						rIndex.Format("%d", nCount);
						ofile <<  rIndex + _T("\t") << rLoadID + _T("\t") << rDesc +_T("\t") << rRating +_T("\t") << rDuty +_T("\t") << _T("MCCB\t") << rAF + _T("/") + rAT +_T("\t");
						
						vector<CCableItem*> PowerCableItemEntry, LCSControlCableItemEntry, NOTLCSControlCableItemEntry;
						pLoadItem->GetCableOfType(PowerCableItemEntry, _T("Power Cable"));
						pLoadItem->GetCableOfType(NOTLCSControlCableItemEntry, _T("Control Cable"));
						pLoadItem->GetCableOfType(LCSControlCableItemEntry, _T("Control Cable"), _T("LCS"));

						//! Power Cable
						CString rPowerCableValue, rLCSControlCableValue, rNotLCSControlCableValue;
						{
							for(vector<CCableItem*>::iterator jtr = PowerCableItemEntry.begin(); jtr != PowerCableItemEntry.end();++jtr)
							{
								const string rCableCore  = (*jtr)->prop()->GetValue(_T("General"),_T("Core")); 
								const string rCableSize = (*jtr)->prop()->GetValue(_T("General"),_T("Size"));

								rPowerCableValue += (CString(rCableCore.c_str()) + _T("-") + CString(rCableSize.c_str()) + _T(","));
							}

							if(_T(",") == rPowerCableValue.Right(1))
							{
								const int at = rPowerCableValue.ReverseFind(',');
								rPowerCableValue = rPowerCableValue.Left(at);
							}
							ofile << rPowerCableValue + _T("\t");
						}

						//! LCS Control Cable
						{
							for(vector<CCableItem*>::iterator ktr = LCSControlCableItemEntry.begin(); ktr != LCSControlCableItemEntry.end();++ktr)
							{
								const string rCableCore  = (*ktr)->prop()->GetValue(_T("General"),_T("Core")); 
								const string rCableSize = (*ktr)->prop()->GetValue(_T("General"),_T("Size"));

								rLCSControlCableValue += (CString(rCableCore.c_str()) + _T("-") + CString(rCableSize.c_str()) + _T(","));
							}

							if(_T(",") == rLCSControlCableValue.Right(1))
							{
								const int at = rLCSControlCableValue.ReverseFind(',');
								rLCSControlCableValue = rLCSControlCableValue.Left(at);
							}

							ofile << rLCSControlCableValue + _T("\t");
						}

						//!NOT LCS Control Cable
						{
							for(vector<CCableItem*>::iterator ktr = NOTLCSControlCableItemEntry.begin(); ktr != NOTLCSControlCableItemEntry.end();++ktr)
							{
								const string rCableCore  = (*ktr)->prop()->GetValue(_T("General"),_T("Core")); 
								const string rCableSize = (*ktr)->prop()->GetValue(_T("General"),_T("Size"));

								rNotLCSControlCableValue += (CString(rCableCore.c_str()) + _T("-") + CString(rCableSize.c_str()) + _T(","));
							}

							if(_T(",") == rNotLCSControlCableValue.Right(1))
							{
								const int at = rNotLCSControlCableValue.ReverseFind(',');
								rNotLCSControlCableValue = rNotLCSControlCableValue.Left(at);
							}

							ofile << rNotLCSControlCableValue + _T("\t");
						}

						ofile << rSchematicType + _T("\t") << endl;
						++nCount;
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

