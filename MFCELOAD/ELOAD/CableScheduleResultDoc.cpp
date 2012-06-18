// CableScheduleResultDoc.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "CableScheduleResultDoc.h"
#include "ELoadDocData.h"
#include "WorkStatusDlg.h"
#include "CompareRevisionDataDoc.h"
#include <Zip\inc\ZipArchive.h>

// CCableScheduleResultDoc

IMPLEMENT_DYNCREATE(CCableScheduleResultDoc, CDocument)

CCableScheduleResultDoc::CCableScheduleResultDoc()
{
}

BOOL CCableScheduleResultDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CCableScheduleResultDoc::~CCableScheduleResultDoc()
{
}


BEGIN_MESSAGE_MAP(CCableScheduleResultDoc, CDocument)
END_MESSAGE_MAP()


bool CableScheduleResult::SortCableItemFunc(CELoadItem* lhs , CELoadItem* rhs)
{
	CCableItem* pLhsCableItem = static_cast<CCableItem*>(lhs);
	CCableItem* pRhsCableItem = static_cast<CCableItem*>(rhs);

	const string rLhsLoadId = lhs->prop()->GetValue(_T("General") , _T("Load Id"));
	const string rRhsLoadId = rhs->prop()->GetValue(_T("General") , _T("Load Id"));

	int nLhs = 0 , nRhs = 0;
	if(_T("User") == lhs->prop()->GetValue(_T("General") , _T("Created By")))
		nLhs = 1;
	else
	{
		if(_T("Control Cable") == lhs->prop()->GetValue(_T("General") , _T("Cable Category")))
			nLhs = 2;
		else	nLhs = 3;
	}
	if(_T("User") == rhs->prop()->GetValue(_T("General") , _T("Created By")))
		nRhs = 1;
	else
	{
		if(_T("Control Cable") == rhs->prop()->GetValue(_T("General") , _T("Cable Category")))
			nRhs = 2;
		else	nRhs = 3;
	}

	if(rLhsLoadId == rRhsLoadId)
	{
		if((pLhsCableItem->IsEquipmentCable() && pRhsCableItem->IsEquipmentCable()) ||
			(!pLhsCableItem->IsEquipmentCable() && !pRhsCableItem->IsEquipmentCable()))
		{
			return (nLhs > nRhs);
		}
		else
			return pLhsCableItem->IsEquipmentCable();
	}else	return (rLhsLoadId < rRhsLoadId);

	return false;
}

// CCableScheduleResultDoc diagnostics

#ifdef _DEBUG
void CCableScheduleResultDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CCableScheduleResultDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CCableScheduleResultDoc serialization

void CCableScheduleResultDoc::Serialize(CArchive& ar)
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


// CCableScheduleResultDoc commands

BOOL CCableScheduleResultDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	/*if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;*/
	const CString rParam(CString(lpszPathName).MakeUpper());

	CreateFolderIfNeed();

	CELoadDocData& docData = CELoadDocData::GetInstance();
	
	vector<string> oResult;
	CTokenizer<CIsFromString>::Tokenize(oResult , rParam.operator LPCSTR() , CIsFromString(_T("|")));
	if(1 == oResult.size())
	{
		m_rFolderPath = (docData.GetProjectFolderPath() + _T("Cable Schedule Result\\")).c_str();
		if(-1 != rParam.Find(_T("CREATE"))) WriteCableScheduleDataInText();
	}
	else if(2 == oResult.size())
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		const CString strProjectPath = docData.GetProjectFolderPath().c_str();
		::DeleteFile(strProjectPath + _T("Working\\CableScheduleResult.txt"));

		const string rMDBFilePath = docData.GetProjectMDBFilePath();

		CADODB adoDB;
		const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;
		if(TRUE == adoDB.DBConnect(rConnectionString))
		{
			const STRING_T rSql(_T("SELECT * FROM T_PRODUCT_REVISION WHERE C_PRODUCT_NAME = 'Cable Schedule' AND C_REVISION_NO = ") + oResult[1]);

			adoDB.OpenQuery(rSql);
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
					const CString rRevPath = CCompareRevisionDataDoc::GetRevPath(_T("Cable Schedule") , strValue.c_str());
					if(FALSE == ::PathIsDirectory(rRevPath)) CreateDirectory(rRevPath , NULL);

					adoDB.GetBinaryValue(0 , _T("C_DATA") , (LPVOID)pData , lSize);
					CFile f(rRevPath + _T("\\REV.ZIP") , CFile::modeCreate|CFile:: modeReadWrite);
					f.Write(pData , lSize);
					f.Close();

					CCompareRevisionDataDoc::UncompressRevFile(rRevPath + _T("\\REV.ZIP") , rRevPath);
					m_rFolderPath = rRevPath + _T("\\");

					/*
					LPSECURITY_ATTRIBUTES attr;
					attr = NULL;

					adoDB.GetBinaryValue(0 , _T("C_DATA") , (LPVOID)pData , lSize);
					CFile f(strProjectPath + _T("Cable Schedule Result\\Cable Schedule Result.ZIP") , CFile::modeCreate|CFile:: modeReadWrite);
					f.Write(pData , lSize);
					f.Close();

					CCompareRevisionDataDoc::UncompressRevFile(strProjectPath + _T("Cable Schedule Result\\Cable Schedule Result.ZIP") , strProjectPath + _T("Working"));
					*/
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

	return TRUE;
}

/**
	@brief	WRITE CABLE Schedule DATA to TEXT FILE

	@author BHK

	@date	2009-09-03 오후 4:02:46

	@param	

	@return		
**/
int CCableScheduleResultDoc::WriteCableScheduleDataInText(void)
{
	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
	if(NULL == pDlg)
	{
		CWorkStatusDlg dlg;
		dlg.m_pThread = AfxBeginThread(CCableScheduleResultDoc::StatusWriteTextThreadEntry , this , THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
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

	@author BHK	

	@date 2009-09-03 오후 4:51:48	

	@param	

	@return		
*/
int CCableScheduleResultDoc::StatusWriteTextThread()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	double progress = 0.;
	int nIndex = 0;

	const string rProjectPath = docData.GetProjectFolderPath();
	CString rFilePath = m_rFolderPath + CString(_T("CableScheduleResult.txt"));

	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();

	CString rTitle = _T("Write Cable Schedule...");

	TABLES::CELOADTable* pTable = docData.GetTableOf(_T("T_CABLE_SCHEDULE_SETTING"));
	ofstream ofile(rFilePath);
	if(ofile.is_open() && pTable)
	{
		if(pTable->m_RecordEntry.empty())
		{
			AfxMessageBox(_T("Cable Schedule Setting Table is unset."));
			return ERROR_SUCCESS;
		}

		const int nTotalCount = pTable->m_RecordEntry.size();
		int nIndex = 0;

		for(vector<TABLES::CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin();itr != pTable->m_RecordEntry.end();++itr)
		{
			CBusItem* pBusItem = docData.FindBusItemByName(itr->FieldValueMap[_T("C_BUS_ID")]);
			if(NULL == pBusItem) continue;

			list<CELoadItem*> CableItemList;
			pBusItem->GetCableList(CableItemList);
			
			string rLoadID;
			ofile << pBusItem->GetName() << _T(" - ");
			ofile << pBusItem->prop()->GetValue(_T("General") , _T("Description")) << std::endl;
			::stable_sort(CableItemList.begin() , CableItemList.end() , CableScheduleResult::SortCableItemFunc);
			for(list<CELoadItem*>::iterator jtr = CableItemList.begin(); jtr != CableItemList.end();++jtr)
			{
				if((*jtr)->IsDeleted()) continue;
				CCableItem* pCableItem = static_cast<CCableItem*>(*jtr);
				if(!rLoadID.empty() && (rLoadID != pCableItem->prop()->GetValue(_T("General") , _T("Load Id"))))
					ofile << std::endl;

				rLoadID = pCableItem->prop()->GetValue(_T("General") , _T("Load Id"));
				WriteCableItemToTextFile(ofile , *jtr);
			}
			ofile << std::endl;

			int progress = int((double(nIndex) / double(nTotalCount))*100.);
			if(pDlg) pDlg->UpdateWorkStatus(rTitle , progress);
			++nIndex;
		}
		ofile.close();

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author BHK	

	@date 2009-09-03 오후 4:50:51	

	@param	

	@return		
*/
UINT CCableScheduleResultDoc::StatusWriteTextThreadEntry(LPVOID pVoid)
{
	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
	if(pDlg)
	{
		CCableScheduleResultDoc* p = (CCableScheduleResultDoc*)pVoid;
		if(p) p->StatusWriteTextThread();

		InterlockedExchange((LONG*)(&(pDlg->m_bThreadRunning)) , FALSE);
		if(pDlg) pDlg->PostMessage(WM_COMMAND , IDOK);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-09-03 오후 4:08:52	

	@param	

	@return		
*/
int CCableScheduleResultDoc::WriteCableItemToTextFile(ofstream& ofile, CELoadItem* pCableItem)
{
	assert(pCableItem && "pCableItem is NULL");

	CELoadDocData& docData = CELoadDocData::GetInstance();
	if(pCableItem)
	{
		const string  rCreatedBy = pCableItem->prop()->GetValue(_T("General") , _T("Created By"));
		///if(_T("User") ==  rCreatedBy)
		{
			const string rArea = pCableItem->prop()->GetValue(_T("General") ,_T("Area"));
			//const string rPanelId = pCableItem->prop()->GetValue(_T("General"), _T("Panel ID"));
			const string rLoadId = pCableItem->prop()->GetValue(_T("General") ,_T("Load Id"));
			//string rCableCategory = pCableItem->prop()->GetValue(_T("General") ,_T("Cable Category"));
			const string rFrom = pCableItem->prop()->GetValue(_T("General") ,_T("From"));
			const string rTo = pCableItem->prop()->GetValue(_T("General") ,_T("To"));
			const string rCableId = pCableItem->prop()->GetValue(_T("General") ,_T("CABLE ID"));
			const string rDesc = pCableItem->prop()->GetValue(_T("General") ,_T("Description"));
			const string rVolt = pCableItem->prop()->GetValue(_T("General") ,_T("Cable Volt"));
			const string rType = pCableItem->prop()->GetValue(_T("General") ,_T("Cable Spec"));
			const string rSpec = pCableItem->prop()->GetValue(_T("General") ,_T("Cable Spec"));
			const string rCore = pCableItem->prop()->GetValue(_T("General") ,_T("Core"));
			const string rSize = pCableItem->prop()->GetValue(_T("General") ,_T("Size"));
			const string rLength = pCableItem->prop()->GetValue(_T("General") ,_T("Length"));
			const string rConduitSize = pCableItem->prop()->GetValue(_T("Conduit") ,_T("Conduit Size"));
			const string rDrumNo = pCableItem->prop()->GetValue(_T("General") ,_T("Drum Id"));

			string rRatingCapacity, rLoadVolt;

			CLoadItem* pLoadItem = docData.FindLoadItemByName(rLoadId);
			if(pLoadItem)
			{
				rLoadVolt = pLoadItem->prop()->GetValue(_T("Rating") ,_T("Rated Voltage"));
				rRatingCapacity = pLoadItem->prop()->GetValue(_T("Rating"), _T("Rating Capacity"));
			}

			ofile << rArea + "\t" << rLoadId + "\t"<< rLoadVolt + "\t" << rRatingCapacity + "\t" << rFrom + "\t" << rTo + "\t" << rDesc + "\t" << rCableId + "\t" << rVolt + "\t" << rSpec + "\t" << rCore + "\t" << rSize + "\t" << rLength+ "\t" << rConduitSize + "\t" << rDrumNo + "\t" <<endl;
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	Product Output Folder가 생성되지 않았다면 폴더를 생성한다.

	@author BHK	

	@date 2009-09-09 오전 10:58:32

	@param	

	@return		
*/
int CCableScheduleResultDoc::CreateFolderIfNeed(const string& rFolderPath)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string rInterestingPath = (rFolderPath.empty()) ? 
		(docData.GetProjectFolderPath() + _T("Cable Schedule Result")) : rFolderPath;

	if(FALSE == ::PathIsDirectory(rInterestingPath.c_str()))
	{
		CreateDirectory(rInterestingPath.c_str() , NULL);
	}

	return 0;
}