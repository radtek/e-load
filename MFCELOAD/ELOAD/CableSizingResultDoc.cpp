// CableSizingResultDoc.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "ELoadDocData.h"
#include "CableSizingResultDoc.h"
#include "CompareRevisionDataDoc.h"
#include "WorkStatusDlg.h"
#include <util/FileTools.h>

int CCableSizingResultDoc::m_nLoadPowerCableCreation = CCableSizingResultDoc::ALL_POWER_CABLE;
list<CELoadItem*> CCableSizingResultDoc::m_InterestingLoadItemList;

// CCableSizingResultDoc

IMPLEMENT_DYNCREATE(CCableSizingResultDoc, CDocument)

CCableSizingResultDoc::CCableSizingResultDoc()
{
}

BOOL CCableSizingResultDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CCableSizingResultDoc::~CCableSizingResultDoc()
{
}


BEGIN_MESSAGE_MAP(CCableSizingResultDoc, CDocument)
END_MESSAGE_MAP()


// CCableSizingResultDoc diagnostics

#ifdef _DEBUG
void CCableSizingResultDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CCableSizingResultDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CCableSizingResultDoc serialization

void CCableSizingResultDoc::Serialize(CArchive& ar)
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

/**
	@brief	

	@author BHK	

	@date 2009-09-03 오후 5:38:17	

	@param	

	@return		
*/
int CCableSizingResultDoc::WriteCableSizeDataToTextFile(void)
{
	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
	if(NULL == pDlg)
	{
		CWorkStatusDlg dlg;
		dlg.m_pThread = AfxBeginThread(CCableSizingResultDoc::StatusWriteTextThreadEntry , this , THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
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

	@date 2009-06-10 오전 9:21:09	

	@param	

	@return		
**/
UINT CCableSizingResultDoc::StatusWriteTextThreadEntry(LPVOID pVoid)
{
	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
	if(pDlg)
	{
		CCableSizingResultDoc* p = (CCableSizingResultDoc*)pVoid;
		if(p) p->StatusWriteTextThread();

		InterlockedExchange((LONG*)(&(pDlg->m_bThreadRunning)) , FALSE);
		if(pDlg) pDlg->PostMessage(WM_COMMAND , IDOK);
	}

	return ERROR_SUCCESS;
}

// CCableSizingResultDoc commands
/**
	@brief	

	@author BHK	

	@date 2009-05-07 오후 1:30:35	

	@param	

	@return		
*/
BOOL CCableSizingResultDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	/*if (!CDocument::OnOpenDocument(lpszPathName))
	return FALSE;*/
	const CString rParam(CString(lpszPathName).MakeUpper());

	if((_T("CREATE AUTO POWER CABLE") == rParam) || (_T("SHOW AUTO POWER CABLE") == rParam))
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		m_rFolderPath = docData.GetProjectFolderPath().c_str() + CString(_T("Cable Sizing Result\\"));

		CreateFolderIfNeed();
		WriteCableSizeDataToTextFile();
	}
	else if(0 == rParam.Find(_T("REV CABLE SIZING RESULT")))
	{
		vector<string> oResult;
		CTokenizer<CIsFromString>::Tokenize(oResult , rParam.operator LPCSTR() , CIsFromString("|"));
		if(2 == oResult.size())
		{
			const string rRevNo= oResult[1];

			CELoadDocData& docData = CELoadDocData::GetInstance();
			const string rMDBFilePath = docData.GetProjectMDBFilePath();

			CADODB adoDB;
			const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;
			if(TRUE == adoDB.DBConnect(rConnectionString))
			{
				const CString rFormat(_T("SELECT * FROM T_PRODUCT_REVISION WHERE C_PRODUCT_NAME = 'Auto Power Cable' AND C_REVISION_NO = %s"));

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

						const CString rRevPath = CCompareRevisionDataDoc::GetRevPath(_T("Auto Power Cable") , strValue.c_str());
						CreateFolderIfNeed(rRevPath.operator LPCSTR());

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
	}

	return TRUE;
}

/**
	@brief	CableSizeResultData를 파일로 저장합니다.

	@author BHK	

	@date 2009-05-07 오후 1:31:06	

	@param	

	@return		
*/
int CCableSizingResultDoc::StatusWriteTextThread(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string rInterestingFilePath = docData.GetProjectFolderPath() + _T("Cable Sizing Result\\");
		
	list<CELoadItem*> BusGroupItemList;
	docData.GetELoadItemListOf(BusGroupItemList , CBusGroupItem::TypeString());

	if(!BusGroupItemList.empty())
	{
		CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
		CString rTitle = _T("Write Cable Size...");
		
		int nTotalCount = 0 , nIndex = 0;
		for(list<CELoadItem*>::iterator itr = BusGroupItemList.begin();itr != BusGroupItemList.end();++itr)
		{
			CBusGroupItem* pBusGroupItem = static_cast<CBusGroupItem*>(*itr);
			if(pBusGroupItem) nTotalCount += pBusGroupItem->m_BusRefEntry.size();
		}
		
		for(list<CELoadItem*>::iterator itr = BusGroupItemList.begin();itr != BusGroupItemList.end();++itr)
		{
			CBusGroupItem* pBusGroupItem = static_cast<CBusGroupItem*>(*itr);
			const string rBusGroupName = pBusGroupItem->GetName();

			for(vector<CBusItem*>::iterator jtr = pBusGroupItem->m_BusRefEntry.begin();jtr != pBusGroupItem->m_BusRefEntry.end();++jtr,++nIndex)
			{
				if((*jtr)->IsDeleted()) continue;

				const string rBusID = (*jtr)->GetName();
				ofstream ofile(CString(rInterestingFilePath.c_str()) + rBusID.c_str() +  _T(".BUS"));
				if(ofile.is_open())
				{
					WriteCableResultDataInBus(ofile , *jtr);
					ofile.close();
				}

				int progress = int((double(nIndex) / double(nTotalCount))*100.);
				if(pDlg) pDlg->UpdateWorkStatus(rTitle , progress);
			}
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
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
	@brief	BUS에 들어 있는 CABLE DATA를 표기합니다.

	@author BHK	

	@date 2009-04-22 오후 7:07:03	

	@param	

	@return		
*/
int CCableSizingResultDoc::WriteCableResultDataInBus(ofstream& ofile , CBusItem* pBusItem)
{
	assert(pBusItem && "pBusItem is NULL");

	if(pBusItem)
	{
		int nNo = 0;
		CELoadDocData& docData = CELoadDocData::GetInstance();
		const string rBusID = pBusItem->GetName();

		//! Equipment의 Cable을 담은 MAP
		map<CCableItem*, CTransformerItem* > EQ_TRANSFORMER_MAP;
		map<CCableItem*, CCapacitorItem*> EQ_CAPACITOR_MAP;
		map<CCableItem*, CUPSDCItem*> EQ_UPSDC_MAP;

		//!  표시해야할 Cable Item을 담은 List
		list<CELoadItem*> CableItemList;

		for(list<CELoadItem*>::iterator itr = CCableSizingResultDoc::m_InterestingLoadItemList.begin();itr != CCableSizingResultDoc::m_InterestingLoadItemList.end();++itr)
		{
			if((*itr)->IsKindOf(CLoadItem::TypeString()))
			{
				CLoadItem* pLoadItem = static_cast<CLoadItem*>(*itr);
				if(rBusID == (*itr)->prop()->GetValue(_T("Panel") , _T("Bus ID")))
				{
					CableItemList.insert(CableItemList.end() , pLoadItem->m_CableRefEntry.begin() , pLoadItem->m_CableRefEntry.end());
				}
			}
			else if((*itr)->IsKindOf(CTransformerItem::TypeString()))
			{
				CTransformerItem* pTrItem = static_cast<CTransformerItem*>(*itr);
				list<CCableItem*> _CableItemList;
				pTrItem->GetCableList(_CableItemList);

				for(list<CCableItem*>::iterator jtr = _CableItemList.begin();jtr != _CableItemList.end();++jtr)
				{
					if(rBusID == (*jtr)->prop()->GetValue(_T("General") , _T("Panel ID")))
					{
						CableItemList.push_back(*jtr);
						EQ_TRANSFORMER_MAP.insert(::make_pair((*jtr), pTrItem));
					}
				}
			}
			else if((*itr)->IsKindOf(CCapacitorItem::TypeString()))
			{
				CCapacitorItem* pCapacitorItem = static_cast<CCapacitorItem*>(*itr);
				list<CCableItem*> _CableItemList;
				pCapacitorItem->GetCableList(_CableItemList);

				for(list<CCableItem*>::iterator jtr = _CableItemList.begin();jtr != _CableItemList.end();++jtr)
				{
					if(rBusID == (*jtr)->prop()->GetValue(_T("General") , _T("Panel ID")))
					{
						CableItemList.push_back(*jtr);
						EQ_CAPACITOR_MAP.insert(::make_pair((*jtr), pCapacitorItem));
					}
				}
			}
			else if((*itr)->IsKindOf(CUPSDCItem::TypeString()))
			{
				CUPSDCItem* pUPSDCItem = static_cast<CUPSDCItem*>(*itr);
				list<CCableItem*> _CableItemList;
				pUPSDCItem->GetCableList(_CableItemList);

				vector<CCableItem*> TempCableItemEntry;

				for(list<CCableItem*>::iterator jtr = _CableItemList.begin();jtr != _CableItemList.end();++jtr)
				{
					if(rBusID == (*jtr)->prop()->GetValue(_T("General") , _T("Panel ID")))
					{
						CableItemList.push_back(*jtr);
						EQ_UPSDC_MAP.insert(::make_pair((*jtr), pUPSDCItem));
					}
				}
			}
		}

		//! Cable Item을 LOAD,CABLE 순에 의거하여 SORTING한다.
		stable_sort(CableItemList.begin() , CableItemList.end() , SortCableItemFunc);

		for(list<CELoadItem*>::iterator itr = CableItemList.begin();itr != CableItemList.end();++itr)
		{
			if((*itr)->IsDeleted()) continue;

			CCableItem* pCableItem = static_cast<CCableItem*>(*itr);
			if(NULL == pCableItem) continue;

			const string rCableCategory = pCableItem->prop()->GetValue(_T("General") , _T("Cable Category"));
			const string rCreateBy = pCableItem->prop()->GetValue(_T("General") , _T("Created By"));

			if(_T("Control Cable") == rCableCategory || _T("User") == rCreateBy) continue;

			CELoadItemProp * pCableItemProp = pCableItem->prop();
			/*
			//! Space Heater는 표기하지 않는다.
			const string rSubCategory = pCableItem->prop()->GetValue(_T("General") , _T("SubCategory"));
			if(_T("Space Heater") == rSubCategory) continue;
			*/

			//! get load item
			CLoadItem* pLoadItem = pCableItem->GetLoadItemPtr();
			///if(pLoadItem)
			{
				++nNo;

				const string rBusID = pBusItem->GetName();
				ofile << /*rBusID.c_str() <<*/ "\t";

				///const string rBusVolt = (pBusItem) ? pBusItem->prop()->GetValue(_T("General") , _T("Bus Voltage")) : _T("");
				CString rNo;
				rNo.Format(_T("%d") , nNo);
				ofile << rNo << _T("\t");

				const string rLoadID = pCableItem->prop()->GetValue(_T("General") , _T("Load Id"));
				ofile << rLoadID.c_str() << "\t";

				const string rLoadVoltage = pCableItem->prop()->GetValue(_T("General") , _T("Load Volt"));
				ofile << rLoadVoltage.c_str() << "\t";

				//! LOAD ITEM
				if(pLoadItem)
				{
					///const string rLoadId = pLoadItem->GetName();
					CELoadItemProp* pLoadItemProp = pLoadItem->prop();

					const string rRating = pCableItem->prop()->GetValue(_T("General") , _T("Rating Capacity"));///pLoadItem->GetRatingCapacityForPowerCableCreation();
					ostringstream oss;
					///oss << nRatingCapacity;
					///const string rRating = (0.f != nRatingCapacity) ? oss.str() : _T("");
					ofile << rRating.c_str() << _T("\t");

					const string rUnit = pCableItem->prop()->GetValue(_T("General") , _T("UNIT"));
					ofile << rUnit.c_str() << _T("\t");

					const double nPF = atof(pCableItem->prop()->GetValue(_T("General") , _T("PF")).c_str());///pLoadItem->GetPFForPowerCableCreation();
					oss.str(""); oss << nPF;
					ofile << oss.str().c_str() << _T("\t");

					const double nEFF = atof(pCableItem->prop()->GetValue(_T("General") , _T("EFF")).c_str());///pLoadItem->GetEFFForPowerCableCreation();
					oss.str(""); oss << nEFF;
					ofile << oss.str().c_str() << "\t";

					///const double nFLC = pLoadItem->GetFLCForPowerCableCreation();
					///oss.str(""); oss << nFLC;
					const string rFLC = pCableItem->prop()->GetValue(_T("General") , _T("FLC"));
					const double nFLC = atof(rFLC.c_str());
					ofile << oss.str().c_str() << "\t";

					//const string rSF = pLoadItemProp->GetValue(_T("Voltage Drop") , _T("SF For FLC"));
					const string rSF = pCableItem->prop()->GetValue(_T("General") , _T("SF"));
					ofile << rSF.c_str() << "\t";

					double nFLCxSF = nFLC * atof(rSF.c_str());
					nFLCxSF = SAFE_ROUND(nFLCxSF,2);
					CString rResult;
					rResult.Format("%.2f", nFLCxSF);
					const string rFLCxSF = rResult.operator LPCSTR();
					ofile << rFLCxSF.c_str() << "\t";

					const string rStartingAmp = pCableItem->prop()->GetValue(_T("General") , _T("Staring Amp"));///pLoadItemProp->GetValue(_T("Rating") , _T("Starting Amp"));
					ofile << rStartingAmp.c_str() << "\t";

					const string rStartingPF = pCableItem->prop()->GetValue(_T("General") , _T("PF as Staring"));///pLoadItemProp->GetValue(_T("Characteristic") , _T("PF at Starting"));
					ofile << rStartingPF.c_str() << "\t";
				}
				else
				{
					const string rEqID = pCableItem->prop()->GetValue(_T("General") , _T("Load Id"));
					ofile << rEqID.c_str() << "\t";

					ITEMS::CTransformerItem *pTrItem = EQ_TRANSFORMER_MAP[pCableItem];
					ITEMS::CCapacitorItem *pCapacitorItem = EQ_CAPACITOR_MAP[pCableItem];
					ITEMS::CUPSDCItem *pUpsdcItem = EQ_UPSDC_MAP[pCableItem];

					string rPF, rSF , rLoadVolt(_T(" ")) , rRating(_T(" ")) , rUnit(_T(" "));
					CString rFLC, rFLCforSF;
					///if(pTrItem)
					{
						rFLC = pCableItem->prop()->GetValue(_T("General"), _T("FLC")).c_str();
						///rPF = pTrItem->prop()->GetValue(_T("General") , _T("PF"));
						rPF = pCableItem->prop()->GetValue(_T("General") , _T("PF"));
						rSF = pCableItem->prop()->GetValue(_T("General"), _T("SF"));
						rLoadVolt = pCableItem->prop()->GetValue(_T("General") , _T("Load Volt"));
						///rRating   = pTrItem->prop()->GetValue(_T("Capacity") , _T("Selected Capacity"));
						rRating   = pCableItem->prop()->GetValue(_T("General") , _T("Rating Capacity"));
						///rUnit     = _T("KVA");
						rUnit = pCableItem->prop()->GetValue(_T("General") , _T("UNIT"));
					}
					/*
					else if(pCapacitorItem)
					{
						rFLC = pCableItem->prop()->GetValue(_T("General"), _T("FLC")).c_str();
						///rPF = pCapacitorItem->prop()->GetValue(_T("General"), _T("PF"));
						rPF = pCableItem->prop()->GetValue(_T("General") , _T("PF"));
						rSF = pCableItem->prop()->GetValue(_T("General"), _T("SF"));
						rLoadVolt = pCableItem->prop()->GetValue(_T("General") , _T("Load Volt"));
						///rRating   = pCapacitorItem->prop()->GetValue(_T("Capacity") , _T("Selected Capacity"));
						rRating   = pCableItem->prop()->GetValue(_T("General") , _T("Rating Capacitor"));
						///rUnit     = _T("KVA");
						rUnit = pCableItem->prop()->GetValue(_T("General") , _T("UNIT"));
					}
					else if(pUpsdcItem)
					{
						rFLC = pCableItem->prop()->GetValue(_T("General"), _T("FLC")).c_str();
						///rPF = pUpsdcItem->prop()->GetValue(_T("General"), _T("PF"));
						rPF = pCableItem->prop()->GetValue(_T("General") , _T("PF"));
						rSF = pCableItem->prop()->GetValue(_T("General"), _T("SF"));
						rLoadVolt = pCableItem->prop()->GetValue(_T("General") , _T("Load Volt"));
						
						///const string rSelected = pUpsdcItem->prop()->GetValue(_T("General") , _T("SELECTED CAPACITY"));
						///if(_T("UPS Capacity") == rSelected)
						///{
						///	rRating = pUpsdcItem->prop()->GetValue(_T("UPS Capacity") , _T("KVA"));
						///}
						///else
						///{
						///	rRating = pUpsdcItem->prop()->GetValue(_T("DC Capacity") , _T("AC Rating KVA"));
						///}
						rRating   = pCableItem->prop()->GetValue(_T("General") , _T("Rating Capacitor"));
						///rUnit     = _T("KVA");
						rUnit = pCableItem->prop()->GetValue(_T("General") , _T("UNIT"));
					}
					*/
					const double nFLC = atof(rFLC);
					rFLC.Format("%.2f", nFLC);
					rFLCforSF.Format("%.2f", atof(rSF.c_str()) * nFLC);

					ofile << rLoadVolt << _T("\t") << rRating << _T("\t") << rUnit << _T("\t");	//! Volt , Rating , UNIT
					ofile << rPF + _T(" \t") << _T(" \t") << rFLC + _T(" \t") << rSF + _T(" \t") << rFLCforSF + _T(" \t") << _T(" \t") << _T(" \t");
				}

				//! write CABLE ITEM's properties
				const string rCableId = pCableItemProp->GetValue(_T("General") , _T("CABLE ID"));
				ofile << rCableId.c_str() << "\t";

				const string rCableVoltage = pCableItemProp->GetValue(_T("General") , _T("Cable Volt"));
				ofile << rCableVoltage.c_str() << "\t";

				const string rCableSpec = pCableItemProp->GetValue(_T("General") , _T("Cable Spec")); 
				ofile << rCableSpec.c_str() << _T("\t");

				const string rSelectedCable = pCableItemProp->GetValue(_T("General") , _T("Core"));
				ofile << rSelectedCable.c_str() << "\t";

				const string rCalculateCable = pCableItemProp->GetValue(_T("General") , _T("Size"));
				ofile << rCalculateCable.c_str() << "\t";

				const string rCableLegth = pCableItemProp->GetValue(_T("General") , _T("Length"));
				ofile << rCableLegth.c_str() << "\t";

				const string rR = pCableItemProp->GetValue(_T("Cable Spec") , _T("R"));
				ofile << rR.c_str() << "\t";

				const string rX = pCableItemProp->GetValue(_T("Cable Spec") , _T("X"));
				ofile << rX.c_str() << "\t";

				const string rCableAmp = pCableItemProp->GetValue(_T("Cable Spec") , _T("Nominal Current"));
				ofile << rCableAmp.c_str() << "\t";

				const string rDeratingFactor = pCableItemProp->GetValue(_T("Cable Spec") , _T("Derating Factor"));
				ofile << rDeratingFactor.c_str() << "\t";

				const string rVdAtRunning = pCableItem->prop()->GetValue(_T("General") , _T("Running VD"));
				ofile << rVdAtRunning.c_str() << "\t";

				string rValue = pCableItem->prop()->GetValue(_T("General") , _T("Running VD%"));
				ofile << ((rValue.empty() || _T("0") == rValue) ? _T("") : rValue + _T(" %")) << "\t";

				rValue	= pCableItem->prop()->GetValue(_T("General") , _T("Allow Running VD%"));
				ofile << ((rValue.empty() || _T("0") == rValue) ? _T("") : rValue + _T(" %")) << "\t";

				rValue	= pCableItem->prop()->GetValue(_T("General") , _T("Starting VD"));
				ofile << rValue.c_str() << "\t";

				rValue	= pCableItem->prop()->GetValue(_T("General") , _T("Starting VD%"));
				ofile << ((rValue.empty() || _T("0") == rValue) ? _T("") : rValue + _T(" %")) << "\t";

				rValue	= pCableItem->prop()->GetValue(_T("General") , _T("Allow Starting VD%"));
				ofile << ((rValue.empty() || _T("0") == rValue) ? _T("") : rValue + _T(" %")) << "\t";

				if(pLoadItem)
				{
					const string rRemark = pLoadItem->prop()->GetValue(_T("Remark") , _T("REMARK"));
					ofile << rRemark.c_str() << "\t" << std::endl;
				}
				else
				{
					ofile << _T(" \t") << std::endl;
				}
			}
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	Product Output Folder가 생성되지 않았다면 폴더를 생성한다.

	@author BHK	

	@date 2009-05-07 오후 1:51:14	

	@param	

	@return		
*/
int CCableSizingResultDoc::CreateFolderIfNeed(const string& rFolderPath)
{
	/*
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string rInterestingPath = (rFolderPath.empty()) ? 
		(docData.GetProjectFolderPath() + _T("Cable Sizing Result")) : rFolderPath;

	if(FALSE == ::PathIsDirectory(rInterestingPath.c_str()))
	{
		CreateDirectory(rInterestingPath.c_str() , NULL);
	}
	*/
	if(FALSE == ::PathIsDirectory(m_rFolderPath))
	{
		CreateDirectory(m_rFolderPath , NULL);
	}

	return ERROR_SUCCESS;
}
