// DrumScheduleResultDoc.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "DrumScheduleResultDoc.h"

#include "DrumItem.h"
#include "ELoadDocData.h"
#include "pyfromc.h"
#include "SimpleAnimationDlg.h"
#include "CompareRevisionDataDoc.h"
#include <Zip\inc\ZipArchive.h>

// CDrumScheduleResultDoc

IMPLEMENT_DYNCREATE(CDrumScheduleResultDoc, CDocument)

CDrumScheduleResultDoc::CDrumScheduleResultDoc()
{
	m_rCurDrum = _T("");
}

BOOL CDrumScheduleResultDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CDrumScheduleResultDoc::~CDrumScheduleResultDoc()
{
}


BEGIN_MESSAGE_MAP(CDrumScheduleResultDoc, CDocument)
END_MESSAGE_MAP()


// CDrumScheduleResultDoc diagnostics

#ifdef _DEBUG
void CDrumScheduleResultDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CDrumScheduleResultDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CDrumScheduleResultDoc serialization

void CDrumScheduleResultDoc::Serialize(CArchive& ar)
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


// CDrumScheduleResultDoc commands

/**
	@brief	

	@author KHS	

	@date 2009-09-28 오후 4:03:57	

	@param	

	@return		
**/
BOOL CDrumScheduleResultDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	/*if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;*/

	CreateFolderIfNeed();

	CELoadDocData& docData = CELoadDocData::GetInstance();
	vector<string> oResult;
	CTokenizer<CIsFromString>::Tokenize(oResult , lpszPathName , CIsFromString(_T("|")));
	if(1 == oResult.size())
	{
		m_rFolderPath = (docData.GetProjectFolderPath() + _T("Drum Schedule Result\\")).c_str();
		WriteDrumScheduleDataInText();
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
			const CString rSql(_T("SELECT * FROM T_PRODUCT_REVISION WHERE C_PRODUCT_NAME = 'Drum Schedule' AND C_REVISION_NO = ") + CString(oResult[1].c_str()));

			adoDB.OpenQuery(rSql.operator LPCTSTR());
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
					const CString rRevPath = CCompareRevisionDataDoc::GetRevPath(_T("Drum Schedule") , strValue.c_str());
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
		}
	}

	return TRUE;
}

/**
	@brief	

	@author KHS	

	@date 2009-09-28 오후 4:12:49	

	@param	

	@return		
**/
int CDrumScheduleResultDoc::CreateFolderIfNeed(const string& rFolderPath)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string rInterestingPath = (rFolderPath.empty()) ? 
		(docData.GetProjectFolderPath() + _T("Drum Schedule Result")) : rFolderPath;

	if(FALSE == ::PathIsDirectory(rInterestingPath.c_str()))
	{
		CreateDirectory(rInterestingPath.c_str() , NULL);
	}

	return 0;
}

/**
	@brief	

	@author KHS	

	@date 2009-09-28 오후 4:20:35	

	@param	

	@return		
**/
int CDrumScheduleResultDoc::WriteDrumScheduleDataInText(void)
{
	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
	if(NULL == pDlg)
	{
		CWorkStatusDlg dlg;
		dlg.m_pThread = AfxBeginThread(CDrumScheduleResultDoc::StatusWriteTextThreadEntry , this , THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
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

	@date 2009-09-28 오후 4:23:41	

	@param	

	@return		
**/
UINT CDrumScheduleResultDoc::StatusWriteTextThreadEntry(LPVOID pVoid)
{
	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
	if(pDlg)
	{
		CDrumScheduleResultDoc* p = (CDrumScheduleResultDoc*)pVoid;
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
int CDrumScheduleResultDoc::StatusWriteTextThread()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	double progress = 0.;
	int nIndex = 0;

	const string rProjectPath = docData.GetProjectFolderPath();
	CString rFilePath = rProjectPath.c_str() + CString(_T("Drum Schedule Result\\DrumScheduleResult.txt"));

	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();

	CString rTitle = _T("Write Drum Schedule...");
	
	list<CELoadItem*> DrumItemList;
	docData.GetELoadItemListOf(DrumItemList, CDrumItem::TypeString());

	ofstream ofile(rFilePath);
	if(ofile.is_open())
	{
		const int nTotalCount = DrumItemList.size();
		int nIndex = 0;

		for(list<CELoadItem*>::iterator itr = DrumItemList.begin();itr != DrumItemList.end();++itr)
		{
			CDrumItem* pDrumItem = static_cast<CDrumItem*>(*itr);
			if(NULL == pDrumItem) continue;
			
			const string rRevno = "";
			const string rDrumName = pDrumItem->GetName();
			const string rDrumLength = pDrumItem->prop()->GetValue(_T("General"), _T("Length"));
			const string rCableVolt = pDrumItem->prop()->GetValue(_T("General"), _T("Cable Volt"));
			const string rCableType = pDrumItem->prop()->GetValue(_T("General"), _T("Cable Spec"));
			const string rCableCore = pDrumItem->prop()->GetValue(_T("General"), _T("Core"));
			const string rCableSize = pDrumItem->prop()->GetValue(_T("General"), _T("Size"));
			const string rSpareLen = pDrumItem->prop()->GetValue(_T("General"), _T("Spare Length"));

			ofile << rRevno + "\t" << rDrumName + "\t" << rDrumLength + "\t";
			
			list<CDrumItem::CABLE_RECORD> CableRecordList;
			pDrumItem->GetCableRecordList(CableRecordList);

			for(list<CDrumItem::CABLE_RECORD>::iterator jtr = CableRecordList.begin();jtr != CableRecordList.end();++jtr)
			{
				if(jtr != CableRecordList.begin()) ofile << "\t\t\t";
				ofile << jtr->rArea + "\t" << jtr->rCableId + "\t" << jtr->rFrom + "\t" << jtr->rTo + "\t" << rCableVolt + "\t" << rCableType + "\t" << rCableCore + "\t" << rCableSize + "\t" << jtr->rLength << endl;
			}
			
			//!Spare
			ofile << "\t\t\t\tSpare\t\t\t\t\t\t\t" + rSpareLen << endl << endl;

			int progress = int((double(nIndex) / double(nTotalCount))*100.);
			if(pDlg) pDlg->UpdateWorkStatus(rTitle , progress);
			++nIndex;
		}
		ofile.close();
		
		return WriteTotalDrumScheduleResult(pDlg);
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	Total 파일 생성

	@author KHS	

	@date 2009-11-20 오전 10:59:25	

	@param	

	@return		
**/
int CDrumScheduleResultDoc::WriteTotalDrumScheduleResult(CWorkStatusDlg *pDlg)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	double progress = 0.;
	int nIndex = 0;

	const string rProjectPath = docData.GetProjectFolderPath();
	CString rFilePath = rProjectPath.c_str() + CString(_T("Drum Schedule Result\\Total_DrumScheduleResult.txt"));

	CString rTitle = _T("Write Total Drum Schedule...");
	
	list<CELoadItem*> DrumItemList;
	docData.GetELoadItemListOf(DrumItemList, CDrumItem::TypeString());

	ofstream ofile(rFilePath);
	if(ofile.is_open())
	{
		const int nTotalCount = DrumItemList.size();
		int nIndex = 0;

		for(list<CELoadItem*>::iterator itr = DrumItemList.begin();itr != DrumItemList.end();++itr)
		{
			CDrumItem* pDrumItem = static_cast<CDrumItem*>(*itr);
			if(NULL == pDrumItem) continue;
			
			const string rRevno = "";
			const CString rDrumName = pDrumItem->GetName().c_str();
			const string rDrumLength = pDrumItem->prop()->GetValue(_T("General"), _T("Length"));
			const string rCableVolt = pDrumItem->prop()->GetValue(_T("General"), _T("Cable Volt"));
			const string rCableType = pDrumItem->prop()->GetValue(_T("General"), _T("Cable Spec"));
			const string rCableCore = pDrumItem->prop()->GetValue(_T("General"), _T("Core"));
			const string rCableSize = pDrumItem->prop()->GetValue(_T("General"), _T("Size"));
			const string rSpareLen = pDrumItem->prop()->GetValue(_T("General"), _T("Spare Length"));
			
			const int nPos = rDrumName.ReverseFind('-');
			CString rDrumTemp;
			rDrumTemp = rDrumName.Left(nPos);
		
			//! Drum 별로 빈 한줄 추가
			if(!m_rCurDrum.IsEmpty() && m_rCurDrum != rDrumTemp) ofile << endl;
			
			m_rCurDrum = rDrumTemp;

			CString rIndex;
			rIndex.Format("%d", nIndex + 1);
			ofile << rRevno + "\t" << rIndex + "\t" << rDrumName + "\t" << rDrumLength + "\t" << rCableVolt + "\t" << rCableType + "\t" << rCableCore + "\t" << rCableSize << endl;

			int progress = int((double(nIndex) / double(nTotalCount))*100.);
			if(pDlg) pDlg->UpdateWorkStatus(rTitle , progress);
			++nIndex;
		}

		m_rCurDrum = _T("");

		ofile.close();

		return WriteTotalDrumCableResult(pDlg);
	}
	return ERROR_BAD_ENVIRONMENT;
}


/**
	@brief	Drum Cable Total

	@author KHS	

	@date 2009-11-27 오전 9:08:18	

	@param	

	@return		
**/
int CDrumScheduleResultDoc::WriteTotalDrumCableResult(CWorkStatusDlg *pDlg)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	double progress = 0.;
	int nIndex = 0;

	const string rProjectPath = docData.GetProjectFolderPath();
	CString rFilePath = rProjectPath.c_str() + CString(_T("Drum Schedule Result\\Total_Drum_Cable_Result.txt"));

	CString rTitle = _T("Write Total Drum Cable Result...");
	
	list<CELoadItem*> DrumItemList;
	docData.GetELoadItemListOf(DrumItemList, CDrumItem::TypeString());

	ofstream ofile(rFilePath);
	if(ofile.is_open())
	{
		const int nTotalCount = DrumItemList.size();
		int nIndex = 0, nCount = 0;

		int progress = int((double(nIndex) / double(nTotalCount))*100.);
		if(pDlg) pDlg->UpdateWorkStatus(rTitle , progress);

		double nTotalDrumLength = 0.;
		string rCableVolt, rCableType, rDrumLength, rCableCore, rCableSize;
		for(list<CELoadItem*>::iterator itr = DrumItemList.begin();itr != DrumItemList.end();++itr)
		{
			CDrumItem* pDrumItem = static_cast<CDrumItem*>(*itr);
			if(NULL == pDrumItem) continue;
			
			const string rRevno = "";
			const CString rDrumName = pDrumItem->GetName().c_str();
			rDrumLength = pDrumItem->prop()->GetValue(_T("General"), _T("Length"));
			
			const int nPos = rDrumName.ReverseFind('-');
			CString rDrumTemp;
			rDrumTemp = rDrumName.Left(nPos);
			
			//! Drum 별로 빈 한줄 추가
			if((!m_rCurDrum.IsEmpty() && m_rCurDrum != rDrumTemp )|| ((*itr) == DrumItemList.back()))
			{
				CString rIndex;
				rIndex.Format("%d", nCount + 1);
				
				if((*itr) == DrumItemList.back()) nTotalDrumLength += atof(rDrumLength.c_str());

				CString rTotalLength;
				rTotalLength.Format("%.f", nTotalDrumLength);

				ofile << rRevno + "\t" << rIndex + "\t" << rTotalLength + "\t" << rCableVolt + "\t" << rCableType + "\t" << rCableCore + "\t" << rCableSize << endl;

				nTotalDrumLength = 0.;
				nCount++;
			}

			nTotalDrumLength += atof(rDrumLength.c_str());

			rCableVolt = pDrumItem->prop()->GetValue(_T("General"), _T("Cable Volt"));
			rCableType = pDrumItem->prop()->GetValue(_T("General"), _T("Cable Spec"));
			rCableCore = pDrumItem->prop()->GetValue(_T("General"), _T("Core"));
			rCableSize = pDrumItem->prop()->GetValue(_T("General"), _T("Size"));

			m_rCurDrum = rDrumTemp;

			int progress = int((double(nIndex) / double(nTotalCount))*100.);
			if(pDlg) pDlg->UpdateWorkStatus(rTitle , progress);
			++nIndex;
		}
		ofile.close();
		m_rCurDrum = _T("");
	
	}

	///! TEST
	//ofstream ofile1(rProjectPath.c_str() + CString(_T("Drum Schedule Result\\TEST_DRUM_LIST.txt")));
	//if(ofile1.is_open())
	//{
	//	for(list<CELoadItem*>::iterator jtr = DrumItemList.begin(); jtr != DrumItemList.end();++jtr)
	//	{
	//		CDrumItem *pDrumItem = static_cast<CDrumItem*>(*jtr);

	//		//ofile1 << pDrumItem->GetName() << endl;
	//		list<CDrumItem::CABLE_RECORD> CableRecordList;
	//		pDrumItem->GetCableRecordList(CableRecordList);
	//		for(list<CDrumItem::CABLE_RECORD>::iterator ktr = CableRecordList.begin(); ktr != CableRecordList.end();++ktr)
	//		{
	//			ofile1 << ktr->rCableId + _T("\t") << ktr->rLength << endl;
	//		}
	//	}
	//	ofile1.close();
	//}

	return ERROR_SUCCESS;
	//return ERROR_BAD_ENVIRONMENT;
}
