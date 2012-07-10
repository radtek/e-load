#include "StdAfx.h"
#include <math.h>
#include "../ELoad.h"
#include <util/ado/ADODB.h>
#include "../ELoadDocData.h"
#include "ProjectSettingTable.h"
#include "LoadSummaryRefTable.h"
#include <sstream>

using namespace TABLES;

CDataGroupByPole::CDataGroupByPole()
{
}

CDataGroupByPole::~CDataGroupByPole()
{
        try
        {
                Clear();
        }
        catch(...)
        {
        }
}

/**
	@brief	POLE단위로 구분된 레코드를 읽습니다.

	@author	BHK
*/
int CDataGroupByPole::Load(CADODB& adoDB, int& i)
{
        CDataGroupByPole::Record* pRecord = new CDataGroupByPole::Record;
        if(pRecord)
        {
                STRING_T rValue;

                adoDB.GetFieldValue(i,"KW",&rValue);      pRecord->m_nKW = atof(rValue.c_str());
                adoDB.GetFieldValue(i,"HZ",&rValue);      pRecord->m_nHZ = atoi(rValue.c_str());
                adoDB.GetFieldValue(i,"VOLT",&rValue);    pRecord->m_nVOLT = atof(rValue.c_str());
                adoDB.GetFieldValue(i,"POLE",&rValue);    pRecord->m_nPOLE = atoi(rValue.c_str());
                adoDB.GetFieldValue(i,"LRC",&rValue);     pRecord->m_nLRC = atof(rValue.c_str());
                adoDB.GetFieldValue(i,"PFlr",&rValue);    pRecord->m_nPFlr = atof(rValue.c_str());
                adoDB.GetFieldValue(i,"PF100",&rValue);   pRecord->m_nPF100 = atof(rValue.c_str());
                adoDB.GetFieldValue(i,"PF75",&rValue);    pRecord->m_nPF75 = atof(rValue.c_str());
                adoDB.GetFieldValue(i,"PF50",&rValue);    pRecord->m_nPF50 = atof(rValue.c_str());
                adoDB.GetFieldValue(i,"EFF100",&rValue);  pRecord->m_nEFF100 = atof(rValue.c_str());
                adoDB.GetFieldValue(i,"EFF75",&rValue);   pRecord->m_nEFF75 = atof(rValue.c_str());
                adoDB.GetFieldValue(i,"EFF50",&rValue);   pRecord->m_nEFF50 = atof(rValue.c_str());

                m_RefRecordEntry.push_back(pRecord);

                return ERROR_SUCCESS;
        }

        return ERROR_BAD_ENVIRONMENT;
}


static bool SortByRatingCapacity(CDataGroupByPole::Record* lhs , CDataGroupByPole::Record* rhs)
{
	return (lhs->m_nKW < rhs->m_nKW);
}

int CDataGroupByPole::Sort()
{
        stable_sort(m_RefRecordEntry.begin() , m_RefRecordEntry.end() , SortByRatingCapacity);

        return ERROR_SUCCESS;
}

int CDataGroupByPole::Clear()
{
        for(vector<Record*>::iterator itr = m_RefRecordEntry.begin();itr != m_RefRecordEntry.end();++itr)
	{
		delete (*itr);
		(*itr) = NULL;
	}
	m_RefRecordEntry.clear();

        return ERROR_SUCCESS;
}

int CDataGroupByPole::GetRatingCapacityList(vector<double>& RatingCapacityEntry)
{
        RatingCapacityEntry.clear();

	for(vector<Record*>::iterator itr = m_RefRecordEntry.begin();itr != m_RefRecordEntry.end();++itr)
	{
		RatingCapacityEntry.push_back((*itr)->m_nKW);
	}
        return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK,KHS
*/
int CDataGroupByPole::FindValues(LoadSearchResult& res , const double& nRatingCapacity , const double& nLF)
{
	for(vector<Record*>::iterator itr = m_RefRecordEntry.begin();itr != m_RefRecordEntry.end();++itr)
	{
		if((nRatingCapacity == (*itr)->m_nKW))
		{
			return (*itr)->GetValues(res , nLF);
		}
		else if((*itr)->m_nKW > nRatingCapacity)
		{
			if(itr > m_RefRecordEntry.begin())
			{
				const double KWprev   = (*(itr-1))->m_nKW;
				const double KWnext   = (*itr)->m_nKW;
				const double dist   = KWnext - KWprev;
				const double factor = (nRatingCapacity - KWprev) / dist;

				double PFprev = 0,PFnext = 0,EFFprev = 0,EFFnext = 0;

				CDataGroupByPole::Record record;
				PFprev = (*(itr-1))->m_nPF100;  PFnext  = (*itr)->m_nPF100;
				EFFprev= (*(itr-1))->m_nEFF100; EFFnext = (*itr)->m_nEFF100;
				record.m_nPF100 = PFprev  + (PFnext -PFprev )*factor;
				record.m_nEFF100= EFFprev + (EFFnext-EFFprev)*factor;

				PFprev = (*(itr-1))->m_nPF75;  PFnext  = (*itr)->m_nPF75;
				EFFprev= (*(itr-1))->m_nEFF75; EFFnext = (*itr)->m_nEFF75;
				record.m_nPF75 = PFprev  + (PFnext -PFprev )*factor;
				record.m_nEFF75= EFFprev + (EFFnext-EFFprev)*factor;

				PFprev = (*(itr-1))->m_nPF50;  PFnext  = (*itr)->m_nPF50;
				EFFprev= (*(itr-1))->m_nEFF50; EFFnext = (*itr)->m_nEFF50;
				record.m_nPF50 = PFprev  + (PFnext -PFprev )*factor;
				record.m_nEFF50= EFFprev + (EFFnext-EFFprev)*factor;

				double prev = (*(itr-1))->m_nLRC;  double next = (*itr)->m_nLRC;
				record.m_nLRC = prev + (next - prev)*factor;

				prev = (*(itr-1))->m_nPFlr;  next = (*itr)->m_nPFlr;
				record.m_nPFlr= prev + (next - prev)*factor;

				record.GetValues(res , nLF);

				return ERROR_SUCCESS;
			}
			else
			{
				//! 최소값보다 작을 경우에 최소값을 취한다.
				m_RefRecordEntry.front()->GetValues(res , nLF);
				return ERROR_SUCCESS;
			}
		}
	}

	//! RATING CAPACITY가 테이블에 있는 값보다 클때는 제일 큰 값의 것으로 PF,EFF를 설정한다.
	if(!m_RefRecordEntry.empty() && (nRatingCapacity > m_RefRecordEntry.back()->m_nKW))
	{
		m_RefRecordEntry.back()->GetValues(res , nLF);
		return ERROR_SUCCESS;
	}
	else if(!m_RefRecordEntry.empty() && (nRatingCapacity < m_RefRecordEntry.front()->m_nKW))
	{
		m_RefRecordEntry.front()->GetValues(res , nLF);
		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDataGroupByVolt::CDataGroupByVolt()
{
}

CDataGroupByVolt::~CDataGroupByVolt()
{
	try
	{
		Clear();
	}
	catch(...)
	{
	}
}

/**
	@brief	

	@author	BHK
*/
int CDataGroupByVolt::Load(CADODB& adoDB, int& i)
{
	STRING_T rValue;
	adoDB.GetFieldValue(i , _T("POLE") , &rValue);
	const int nPole = atoi(rValue.c_str());

	map<int , CDataGroupByPole*>::iterator where = m_DataGroupByPoleEntry.find(nPole);
	if(where == m_DataGroupByPoleEntry.end())
	{
		m_DataGroupByPoleEntry[nPole] = new CDataGroupByPole;
	}

	m_DataGroupByPoleEntry[nPole]->Load(adoDB, i);

	return ERROR_SUCCESS;
}

int CDataGroupByVolt::Sort()
{
	for(map<int , CDataGroupByPole*>::iterator itr = m_DataGroupByPoleEntry.begin();itr != m_DataGroupByPoleEntry.end();++itr)
	{
		itr->second->Sort();
	}

	return ERROR_SUCCESS;
}

int CDataGroupByVolt::Clear()
{
	for(map<int , CDataGroupByPole*>::iterator itr = m_DataGroupByPoleEntry.begin();itr != m_DataGroupByPoleEntry.end();++itr)
	{
		SAFE_DELETE(itr->second);
	}
	m_DataGroupByPoleEntry.clear();
	return ERROR_SUCCESS;
}

/**
	@brief	pole , lf , rating capacity로 값을 구한다.

	@author	BHK
*/
int CDataGroupByVolt::FindValues(LoadSearchResult& res , const double& nRatingCapacity , const int& nPoles, const double& nLF)
{
	map<int , CDataGroupByPole*>::iterator where = m_DataGroupByPoleEntry.find(nPoles);
	if(where != m_DataGroupByPoleEntry.end())
	{
		return where->second->FindValues(res , nRatingCapacity , nLF);
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	

	@author	BHK
*/
int CDataGroupByVolt::GetRatingCapacityList(vector<double>& RatingCapacityEntry , const int& nPoles)
{
	map<int , CDataGroupByPole*>::iterator where = m_DataGroupByPoleEntry.find(nPoles);
	if(where != m_DataGroupByPoleEntry.end())
	{
		where->second->GetRatingCapacityList(RatingCapacityEntry);
	}
        
        return ERROR_SUCCESS;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDataGroupByHertz::CDataGroupByHertz()
{
}

CDataGroupByHertz::~CDataGroupByHertz()
{
        try
        {
                Clear();
        }
        catch(...)
        {
        }
}

/**
	@brief	

	@author	BHK
*/
int CDataGroupByHertz::Load(CADODB& adoDB, int& i)
{
	STRING_T rValue;
	adoDB.GetFieldValue(i , _T("VOLT"),&rValue);
	const double nVolt = atof(rValue.c_str());

	map<double , CDataGroupByVolt*>::iterator where = m_DataGroupByVoltEntry.find(nVolt);
	if(where == m_DataGroupByVoltEntry.end())
	{
		m_DataGroupByVoltEntry[nVolt] = new CDataGroupByVolt;
	}

	m_DataGroupByVoltEntry[nVolt]->Load(adoDB, i);

	return ERROR_SUCCESS;
}

int CDataGroupByHertz::Sort()
{
	for(map<double , CDataGroupByVolt*>::iterator itr = m_DataGroupByVoltEntry.begin();itr != m_DataGroupByVoltEntry.end();++itr)
	{
		itr->second->Sort();
	}

	return ERROR_SUCCESS;
}

int CDataGroupByHertz::Clear()
{
	for(map<double , CDataGroupByVolt*>::iterator itr = m_DataGroupByVoltEntry.begin();itr != m_DataGroupByVoltEntry.end();++itr)
	{
		SAFE_DELETE(itr->second);
	}
	m_DataGroupByVoltEntry.clear();

	return ERROR_SUCCESS;
}

int CDataGroupByHertz::GetRatingCapacityList(vector<double>& RatingCapacityEntry , const double& nVolt , const int& nPoles)
{
	double nMin = -1;
	map<double , CDataGroupByVolt*>::iterator where = m_DataGroupByVoltEntry.end();
	for(map<double , CDataGroupByVolt*>::iterator itr = m_DataGroupByVoltEntry.begin();itr != m_DataGroupByVoltEntry.end();++itr)
	{
		if(-1 == nMin)
		{
			nMin = fabs(itr->first - nVolt);
			where = itr;
		}
		else if(fabs(itr->first - nVolt) < nMin)
		{
			nMin = fabs(itr->first - nVolt);
			where = itr;
		}
	}

	if(where != m_DataGroupByVoltEntry.end())
	{
		where->second->GetRatingCapacityList(RatingCapacityEntry , nPoles);
	}

	return ERROR_SUCCESS;
}
/**
	@brief	volt , pole , lf , rating capacity로 값을 구한다.
		가장 가까운 VOLTAGE를 가진 항목에서 값을 구한다.

	@author	BHK
*/
int CDataGroupByHertz::FindValues(LoadSearchResult& res , const double& nRatingCapacity , const double& nVolt , const int& nPoles, const double& nLF)
{
	double nMin = -1;
	map<double , CDataGroupByVolt*>::iterator where = m_DataGroupByVoltEntry.end();
	for(map<double , CDataGroupByVolt*>::iterator itr = m_DataGroupByVoltEntry.begin();itr != m_DataGroupByVoltEntry.end();++itr)
	{
		if(-1 == nMin)
		{
			nMin = fabs(itr->first - nVolt);
			where = itr;
		}
		else if(fabs(itr->first - nVolt) < nMin)
		{
			nMin = fabs(itr->first - nVolt);
			where = itr;
		}
	}

	///map<double , CDataGroupByVolt*>::iterator where = m_DataGroupByVoltEntry.find(nVolt);
	if(where != m_DataGroupByVoltEntry.end())
	{
		return where->second->FindValues(res , nRatingCapacity , nPoles , nLF);
	}

	return ERROR_INVALID_PARAMETER;
}

CLoadSummaryRefTable::CLoadSummaryRefTable()
{
}

CLoadSummaryRefTable::~CLoadSummaryRefTable()
{
	try
	{
		Clear();
	}
	catch(...)
	{
	}
}

/**
	@brief	

	@author	BHK
*/
CLoadSummaryRefTable& CLoadSummaryRefTable::GetInstance()
{
	static CLoadSummaryRefTable __INSTANCE__;

	return __INSTANCE__;
}


/**
	@brief	LOAD SUMMARY REFERENCE DATABASE에서 DATA를 읽는다.

	@author	BHK
*/
int CLoadSummaryRefTable::LoadDataFromDatabase( const string& rTableName )
{
	Clear();	

	CELoadDocData& docData = CELoadDocData::GetInstance();
	string rProjectMDBFilePath = docData.GetProjectMDBFilePath();

	CADODB adoDB;
	const STRING_T rDBPath = STRING_T(PROVIDER) + _T("Data Source=") + rProjectMDBFilePath + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rDBPath))
	{
		Clear();

		STRING_T rQuery(_T("SELECT * FROM ") + rTableName);
		try
		{ 
			adoDB.OpenQuery(rQuery);

			LONG lRecordCount = 0;
			adoDB.GetRecordCount(&lRecordCount);
			for(int i = 0;i < lRecordCount;i++)
			{
				STRING_T rValue;
				adoDB.GetFieldValue(i , _T("HZ") , &rValue);
				const int nHz = atoi(rValue.c_str());
				map<int , CDataGroupByHertz*>::iterator where = m_DataGroupByHertzEntry.find(nHz);
				if(where == m_DataGroupByHertzEntry.end())
				{
					m_DataGroupByHertzEntry[nHz] = new CDataGroupByHertz;
				}

				m_DataGroupByHertzEntry[nHz]->Load(adoDB, i);
			}
			//! 데이타 베이스를 LOAD 후 정렬한다.
			for(map<int , CDataGroupByHertz*>::iterator itr = m_DataGroupByHertzEntry.begin();itr != m_DataGroupByHertzEntry.end();++itr)
			{
				itr->second->Sort();
			}
		}
		catch(_com_error &e)
		{
			CString rMsg;
			rMsg.Format("*** DOG CHECK [ADO DB] => <ExecuteQuery 오류> 설명[%s] !! ***\n", (LPCSTR)e.Description());
			ELOAD_LOG4CXX_WARN(mylogger , "" , 1);

			return ERROR_BAD_ENVIRONMENT;
		}

		m_rTableName = rTableName;
	}
	else
	{
		AfxMessageBox("Fail to connect database... !");
		return ERROR_FILE_NOT_FOUND;
	}

	return ERROR_SUCCESS;
}

/**
	@brief	record를 삭제하고 Table Name을 클리어한다.

	@author	BHK
*/
int CLoadSummaryRefTable::Clear(void)
{
        for(map<int , CDataGroupByHertz*>::iterator itr =  m_DataGroupByHertzEntry.begin(); itr !=m_DataGroupByHertzEntry.end();++itr)
        {
                SAFE_DELETE(itr->second);
        }
        m_DataGroupByHertzEntry.clear();
	m_rTableName = _T("");	//! 테이블 이름을 클리어

	return ERROR_SUCCESS;
}

/**
	@brief	load summary table에서 hertz , volt , pole , rating capacity , lf로 값을 구한다.

	@author	BHK
*/
int CLoadSummaryRefTable::FindValues(LoadSearchResult& res , const double& nRatingCapacity , const int& nHz, const double& nVolt , const int& nPoles , const double& nLF)
{
	map<int , CDataGroupByHertz*>::iterator where = m_DataGroupByHertzEntry.find(nHz);
	if(where != m_DataGroupByHertzEntry.end())
	{
		return where->second->FindValues(res , nRatingCapacity , nVolt , nPoles, nLF);
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	PROJECT관련 TABLE을 읽습니다.

	@author	BHK
*/
int CLoadSummaryRefTable::Load(const bool& bForce)
{
	static bool __ALREADY_LOADED__ = false;

	if((false == __ALREADY_LOADED__) || (true == bForce))
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
		///docData.m_ProjectSettingTable.Load(_T("") , CProjectSettingTable::GetTableName());
		string* pValue = pSettingTable->GetFieldValue(_T("C_CUR_TABLE_FOR_LOAD_SUMMARY"));
		if(pValue)
		{
			CLoadSummaryRefTable& refTable = CLoadSummaryRefTable::GetInstance();
			int res = refTable.LoadDataFromDatabase(*pValue);
			if(ERROR_SUCCESS == res) __ALREADY_LOADED__ = true;
			
			return res;
		}

		return ERROR_BAD_ENVIRONMENT;
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
/*
int CLoadSummaryRefTable::GetRatingCapacityList(vector<double>& RatingCapacityEntry , const int& nHz, const double& nVolt , const int& nPoles)
{
	RatingCapacityEntry.clear();
	map<int , CDataGroupByHertz*>::iterator where = m_DataGroupByHertzEntry.find(nHz);
	if(where != m_DataGroupByHertzEntry.end())
	{
		where->second->GetRatingCapacityList(RatingCapacityEntry , nVolt , nPoles);
	}

	return ERROR_SUCCESS;
}
*/