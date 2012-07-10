#include "StdAfx.h"
#include "../ELoad.h"
#include "..\ELoadDocData.h"
#include "CommandCalculateLoadSummary.h"
#include "../WorkStatusDlg.h"
#include "../Tables/ProjectSettingTable.h"
#include "../ELoadDocData.h"
#include "../pyfromc.h"
#include "../Tables/LoadSummaryRefTable.h"

#include <assert.h>
using namespace COMMAND;
using namespace TABLES;

CCommandCalculateLoadSummary::CCommandCalculateLoadSummary()
{
	m_nTryCount = 0;
	m_nSuccessCount = 0;
	m_nFailCount = 0;
}

CCommandCalculateLoadSummary::~CCommandCalculateLoadSummary(void)
{
}

/**
	@brief	

	@author BHK	

	@date 2009-08-26 오후 4:46:08	

	@param	

	@return		
*/
int CCommandCalculateLoadSummary::Execute()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CWorkStatusDlg wdlg;
	try
	{	
		m_nTryCount = m_nSuccessCount = m_nFailCount = 0;

		CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
		{
		}

		if(ERROR_SUCCESS != CLoadSummaryRefTable::Load())
		{
			AfxMessageBox(_T("LOAD SUMMARY용 REF TABLE을 설정하세요."));
			return ERROR_BAD_ENVIRONMENT;
		}

		wdlg.m_pThread = AfxBeginThread(StatusThreadEntry , this , THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
		if (NULL == wdlg.m_pThread)
		{
			AfxMessageBox(_T("Can't create thread!!!"));
			
			return ERROR_BAD_ENVIRONMENT;
		}
		else
		{
			wdlg.DoModal();
		}
	}
	catch(exception& e)
	{
		UNUSED_ALWAYS(e);
	}

	//! 로드 SUMMARY 계산을 하고 난뒤의 후속 작업을 수행한다.
	python.CallPyPostLoadSummary(docData.m_bApplyTrToLoadSummary);

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-08-26 오후 4:51:59	

	@param	

	@return		
*/
UINT COMMAND::CCommandCalculateLoadSummary::StatusThreadEntry(LPVOID pVoid)
{
	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
	if(pDlg)
	{
		CCommandCalculateLoadSummary* p = (CCommandCalculateLoadSummary*)pVoid;
		p->StatusThread();
		
		InterlockedExchange((LONG*)(&(pDlg->m_bThreadRunning)) , FALSE);
		if(pDlg)
		{
			pDlg->PostMessage(WM_COMMAND, IDOK);
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-08-26 오후 4:51:18	

	@param	

	@return		
*/
int COMMAND::CCommandCalculateLoadSummary::StatusThread(void)
{
	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();

	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> LoadItemList;
	docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
	///if(pLoadItemList)
	{
		const size_t nCount = LoadItemList.size();
		int nIndex = 0;

		ELOAD_LOG4CXX_RESET(mylogger, _T("Load Summary Calculation Error Messages"));

		CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
		string* pValue = pSettingTable->GetFieldValue(_T("C_CUR_TABLE_FOR_LOAD_SUMMARY"));
		if((NULL == pValue) || pValue->empty())
		{
			ELOAD_LOG4CXX_ERROR(mylogger , _T("SYSTEM") , 23);
			if(pDlg) pDlg->PostMessage(WM_COMMAND , IDOK);

			return ERROR_BAD_ENVIRONMENT;
		}

		for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr,++nIndex)
		{
			CLoadItem* pLoadItem = static_cast<CLoadItem*>(*itr);
			const string rLoadID = pLoadItem->GetName();
			if(pLoadItem->IsDeleted()) continue;	//! 삭제된 아이템은 SKIP....

			++m_nTryCount;
			const string rLoadCategory = pLoadItem->prop()->GetValue(_T("Type") , _T("Load Category"));

			double nRatingCapacity = 0.f;
			pLoadItem->CalcRatingCapacity(nRatingCapacity);

			//! MOTOR TYPE은 무조건 POLES 값이 존재해야 한다.
			const string rPoles = pLoadItem->prop()->GetValue(_T("Rating") , _T("Poles"));
			if((_T("MOTOR") == rLoadCategory) && rPoles.empty())
			{
				//! Load Summary 계산 결과를 클리어한다.
				pLoadItem->prop()->SetValue(_T("Load Summary") , _T("PF") , _T("") , true);
				pLoadItem->prop()->SetValue(_T("Load Summary") , _T("EFF") , _T("") , true);
				pLoadItem->prop()->SetValue(_T("Load Summary") , _T("KW") , _T("") , true);
				pLoadItem->prop()->SetValue(_T("Load Summary") , _T("KVAR") , _T("") , true);
				pLoadItem->prop()->SetValue(_T("Load Summary") , _T("KVA") , _T("") , true);
				ELOAD_LOG4CXX_ERROR(mylogger , rLoadID, 15);

				++m_nFailCount;
				continue;
			}

			int nPoles = 0;
			try
			{
				nPoles = atoi(rPoles.c_str());
			}
			catch(...)
			{
				++m_nFailCount;
				continue;
			}

			(ERROR_SUCCESS == pLoadItem->CalcLoadSummary()) ? ++m_nSuccessCount : ++m_nFailCount;

			int nProgress = int((double(nIndex) / double(nCount)) * 100.f);
			pDlg->UpdateWorkStatus(CString(_T("Load Summary")) , nProgress);
		}

		//! Load Summary를 DATA 파일로 저장한다.
		python.CallPyWriteLoadSummaryToDataFile();

		if(pDlg) pDlg->SendMessage(WM_COMMAND , IDOK);

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	return result of execution as string.

	@author BHK	

	@date 2009-08-26 16:42:50 

	@param	

	@return		
*/
string COMMAND::CCommandCalculateLoadSummary::GetResultString(void)
{
	ostringstream oss;
	{
		oss << _T("TOTAL LOAD |") << m_nTryCount << _T(" (SETS) |");
		oss << _T("Success Count |") << m_nSuccessCount << _T(" (SETS) | ");
		oss << _T("Error Count |") << m_nFailCount << _T(" (SETS) |");
	}

	return oss.str();
}
