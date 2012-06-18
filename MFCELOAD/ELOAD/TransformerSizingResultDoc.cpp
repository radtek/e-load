// TransformerSizingResultDoc.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "CompareRevisionDataDoc.h"
#include "TransformerSizingResultDoc.h"
#include "ELoadDocData.h"

// CTransformerSizingResultDoc

IMPLEMENT_DYNCREATE(CTransformerSizingResultDoc, CDocument)

CTransformerSizingResultDoc::CTransformerSizingResultDoc()
{
}

BOOL CTransformerSizingResultDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CTransformerSizingResultDoc::~CTransformerSizingResultDoc()
{
}


BEGIN_MESSAGE_MAP(CTransformerSizingResultDoc, CDocument)
END_MESSAGE_MAP()


// CTransformerSizingResultDoc diagnostics

#ifdef _DEBUG
void CTransformerSizingResultDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CTransformerSizingResultDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CTransformerSizingResultDoc serialization

void CTransformerSizingResultDoc::Serialize(CArchive& ar)
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

// CTransformerSizingResultDoc commands

BOOL CTransformerSizingResultDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	CString rString(lpszPathName);

	CreateFolderIfNeed();
	if(0 == rString.Find(_T("REV TRANSFORMER SIZING")))
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
				const CString rFormat(_T("SELECT * FROM T_PRODUCT_REVISION WHERE C_PRODUCT_NAME = 'Transformer Sizing' AND C_REVISION_NO = %s"));
				
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
						
						const CString rRevPath = CCompareRevisionDataDoc::GetRevPath(_T("Transformer Sizing") , strValue.c_str());
						::CreateDirectory(rRevPath , attr);

						adoDB.GetBinaryValue(0 , _T("C_DATA") , (LPVOID)pData , lSize);
						CFile f(rRevPath + _T("\\REV.ZIP") , CFile::modeCreate|CFile:: modeReadWrite);
						f.Write(pData , lSize);
						f.Close();

						CCompareRevisionDataDoc::UncompressRevFile(rRevPath + _T("\\REV.ZIP") , rRevPath);
						m_rFilePath = rRevPath + _T("\\Transformer Sizing.txt");
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
	else if(_T("TRANSFORMER SIZING") == CString(lpszPathName))
	{
		if(ERROR_SUCCESS == SaveTransformerSizingResultData())
		{
			CELoadDocData& docData = CELoadDocData::GetInstance();
			m_rFilePath = docData.GetProjectFolderPath() + _T("Transformer Sizing\\Transformer Sizing.txt");
		}
	}

	return TRUE;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-07 오전 10:26:47	

	@param	

	@return		
*/
int CTransformerSizingResultDoc::CreateFolderIfNeed(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string rInterestingPath = docData.GetProjectFolderPath() + _T("Transformer Sizing");
	
	if(FALSE == ::PathIsDirectory(rInterestingPath.c_str()))
	{
		CreateDirectory(rInterestingPath.c_str() , NULL);
	}

	return 0;
}

/**
	@brief	현재 Transformer Sizing Data를 파일로 저장합니다.

	@author HumKyung

	@date 2009-05-07 오전 11:23:13	

	@param	

	@return	int
*/
int CTransformerSizingResultDoc::SaveTransformerSizingResultData(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const STRING_T rInterestingFilePath = docData.GetProjectFolderPath() + _T("Transformer Sizing\\Transformer Sizing.txt");
	OFSTREAM_T ofile(rInterestingFilePath.c_str());
	if(ofile.is_open())
	{
		list<CELoadItem*> TransformerItemList;
		docData.GetELoadItemListOf(TransformerItemList , CTransformerItem::TypeString());
		if(!TransformerItemList.empty())
		{
			::stable_sort(TransformerItemList.begin() , TransformerItemList.end() , SortByName);
			for(list<CELoadItem*>::iterator itr = TransformerItemList.begin();itr != TransformerItemList.end();++itr)
			{
				if((*itr)->IsDeleted()) continue;	//! 삭제된 아이템은 화면에 표시하지 않는다.
				//! skip if transformer name start with 'DEL ' - 2011.02.12 added by HumKyung
				string::size_type at = (*itr)->GetName().find(_T("DEL "));
				if(0 == at) continue;
				//!

				CTransformerItem* pTrItem = static_cast<CTransformerItem*>(*itr);

				RowData rowdata;

				//! BUS CONNECTION
				CELoadItemProp *pTransformerProp = (*itr)->prop();
				rowdata.rTRID = pTransformerProp->GetValue(_T("General") , _T("TR ID"));

				rowdata.rPriBusID = pTransformerProp->GetValue(_T("From") , _T("Bus Id"));

				rowdata.rPriBusVolt = pTransformerProp->GetValue(_T("From") , _T("Bus Volt"));

				//! TR CAPACITY - calcualte capacity of tranformer item - 2011.07.15
				CString rTemp;
				const double dCapacity = pTrItem->CalculateCapacity();
				/*CString rTemp;
				const string rCalcTrCapacity = pTransformerProp->GetValue(_T("Capacity") , _T("Calculated Capacity"));*/
				///rTemp.Format(_T("%.2f") , dCapacity);///SAFE_ROUND(atof(rCalcTrCapacity.c_str()),2));
				{
					OSTRINGSTREAM_T oss;
					oss << dCapacity;
					rowdata.rCalcualtedCapacity = oss.str();///rTemp.operator LPCTSTR();
				}

				const string rSelTrCapacity = pTransformerProp->GetValue(_T("Capacity") , _T("Selected Capacity"));
				rTemp.Format(_T("%.2f") , SAFE_ROUND(atof(rSelTrCapacity.c_str()),2));
				rowdata.rSelectedCapacity = rTemp.operator LPCTSTR();

				if(!rSelTrCapacity.empty()/* && !rCalcTrCapacity.empty()*/)
				{
					OSTRINGSTREAM_T oss;
					oss << (SAFE_ROUND(atof(rSelTrCapacity.c_str()) / dCapacity , 2)) * 100;
					rowdata.rSpare = oss.str();
				}

				const STRING_T rFindBusId= pTransformerProp->GetValue(_T("To") , _T("Bus Id"));
				CBusItem *pBusItem = docData.FindBusItemByName(rFindBusId);
				if(pBusItem)
				{
					//! Bus Group , Bus concept is gone.
					//! get bus list from transformer - 2011.02.12 added by HumKyung
					vector<CBusItem*> BusItemEntry;
					pTrItem->GetConnectedBusItemEntry(BusItemEntry);
					for(vector<CBusItem*>::iterator jtr = BusItemEntry.begin();jtr != BusItemEntry.end();++jtr)
					{
						if((*jtr)->IsDeleted()) continue;

						CELoadItemProp *pBusProp = (*jtr)->prop();
						rowdata.rSecBusID = pBusProp->GetValue("General", "Bus ID");
						rowdata.rSecBusVolt = pBusProp->GetValue("General", "Bus Voltage");

						double nKW = 0.f , nKVAR = 0.f;
						(*jtr)->GetLoadSummaryResultData(nKW , nKVAR);
						const double nKVA = sqrt(nKW*nKW + nKVAR*nKVAR);

						CString rTemp[3];
						rTemp[0].Format(_T("%.2f"), SAFE_ROUND(nKW,2));
						rTemp[1].Format(_T("%.2f"), SAFE_ROUND(nKVAR,2));
						rTemp[2].Format(_T("%.2f") , SAFE_ROUND(nKVA,2));
						rowdata.rKW   = rTemp[0].operator LPCTSTR();
						rowdata.rKVAR = rTemp[1].operator LPCTSTR();
						rowdata.rKVA  = rTemp[2].operator LPCTSTR();

						rowdata.Write(ofile);
						rowdata.Reset();
					}

					ofile << std::endl;
				}
			}
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}
