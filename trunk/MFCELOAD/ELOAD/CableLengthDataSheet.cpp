#include "StdAfx.h"
#include "ELoadDocData.h"
#include "Tables/LoadSummaryRefTable.h"
#include "CableLengthDataSheet.h"

#include "Tables/3PHMotorCableSizingTable.h"
#include "Tables/3PHFeederCableSizingTable.h"
#include "Tables/1PHFeederCableSizingTable.h"
#include "Tables/DCFeederCableSizingTable.h"

CCableLengthDataSheet::CCableLengthDataSheet(void) : m_nRowCount(0) , m_nColCount(0)
{
	/*
	CELoadDocData& docData = CELoadDocData::GetInstance();
	TABLES::CELOADTable* pTable = docData.GetTableOf(CProjectSettingTable::GetTableName());
	m_rName = pTable->m_RecordEntry[0].FieldValueMap[_T("C_CUR_CABLE_SIZING_SETTING_TABLE_NAME")].c_str();
	*/

}

CCableLengthDataSheet::~CCableLengthDataSheet(void)
{
}

/**
	@brief	cable length 계산 작업 준비

	@author	HumKyung

	@date	????.??.??

	@param

	@return	int
*/
int CCableLengthDataSheet::PrepareCableLengthCalculation(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	TABLES::CELOADTable* pTable = docData.GetTableOf(CProjectSettingTable::GetTableName());
	const CString rDataName = pTable->m_RecordEntry[0].FieldValueMap[_T("C_CUR_CABLE_SIZING_SETTING_TABLE_NAME")].c_str();

	CCableSizeSettingTable& CableSizeSettingTable = CCableSizeSettingTable::GetInstance();
	const CString rMotorType = CableSizeSettingTable.GetMotorType(rDataName);
	
	m_RatingCapacityEntry.clear();
	if(_T("3PH MOTOR") == rMotorType)
	{
		C3PHMotorCableSizingTable& table = C3PHMotorCableSizingTable::GetInstance();
		const int nRecordCount = table.GetRecordCount();
		for(int i = 0;i < nRecordCount;++i)
		{
			C3PHMotorCableSizingTable::Record* pRecord = table.GetRecordAt(i);
			m_RatingCapacityEntry.push_back(atof(pRecord->sKW.c_str()));
		}
	}
	else if(_T("3PH FEEDER") == rMotorType)
	{
		C3PHFeederCableSizingTable& table = C3PHFeederCableSizingTable::GetInstance();
		const int nRecordCount = table.GetRecordCount();
		for(int i = 0;i < nRecordCount;++i)
		{
			C3PHFeederCableSizingTable::Record* pRecord = table.GetRecordAt(i);
			m_RatingCapacityEntry.push_back(atof(pRecord->rKW.c_str()));
		}
	}
	else if(_T("1PH FEEDER") == rMotorType)
	{
		C1PHFeederCableSizingTable& table = C1PHFeederCableSizingTable::GetInstance();
		const int nRecordCount = table.GetRecordCount();
		for(int i = 0;i < nRecordCount;++i)
		{
			C1PHFeederCableSizingTable::Record* pRecord = table.GetRecordAt(i);
			m_RatingCapacityEntry.push_back(atof(pRecord->rKW.c_str()));
		}
	}
	else if(_T("DC FEEDER") == rMotorType)
	{
		CDCFeederCableSizingTable& table = CDCFeederCableSizingTable::GetInstance();
		const int nRecordCount = table.GetRecordCount();
		for(int i = 0;i < nRecordCount;++i)
		{
			CDCFeederCableSizingTable::Record* pRecord = table.GetRecordAt(i);
			m_RatingCapacityEntry.push_back(atof(pRecord->rKW.c_str()));
		}
	}
	else	return ERROR_BAD_ENVIRONMENT;

	TABLES::CLoadSummaryRefTable& refTable = TABLES::CLoadSummaryRefTable::GetInstance();
	refTable.Load();

	CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
	string* pValue = pSettingTable->GetFieldValue(_T("C_HERTZ"));
	const int nHertz = ((NULL != pValue) && !pValue->empty()) ? atoi(pValue->c_str()) : 50;	//! 50 is default value
	const int nPole=4;
	const double nVolt = atof(CableSizeSettingTable.GetBusMotorVoltage2(rDataName)) * 0.001;	//! KV --> V

	m_PFEntry.clear();
	m_FLCEntry.clear();
	for(vector<double>::iterator itr = m_RatingCapacityEntry.begin();itr != m_RatingCapacityEntry.end();++itr)
	{
		double PF100 = 0 , PF75 = 0 , PF50 = 0 , EFF100 = 0 , EFF75 = 0 , EFF50 = 0;
		double PF = 0. , EFF = 0. , LF = 1.;
		//! FEEDER가 아닐 경우에는 4번째 POLE인 PF 값을 구한다. (HZ = 50)
		if(-1 != CableSizeSettingTable.GetMotorType(rDataName).Find("FEEDER"))
		{
			PF = atof(CableSizeSettingTable.GetPFForFeederValue(rDataName).operator LPCSTR());
		}
		else
		{
			//! LF가 100%인 값을 구한다.
			LoadSearchResult res;
			refTable.FindValues(res , *itr , nHertz , nVolt , nPole , LF);
			PF = res.PF;
		}

		//! 소수점 2째자리까지 표현.
		long nTemp = long((PF * 100.) + 0.5);
		PF = (double)nTemp * 0.01;

		m_PFEntry.push_back(PF);
		m_FLCEntry.push_back(0);			//! 기본값으로 채워 넣는다.
	}

	if(ERROR_SUCCESS != CCableLengthDataSheet::CreateConduitSizeList(CableSizeSettingTable.GetCableType(rDataName).operator LPCSTR() , 
		CableSizeSettingTable.GetConduitTableName(rDataName).operator LPCSTR() , 0.4 , m_ConduitSizeList))
	{
		///AfxMessageBox(_T("Can't create conduit size list"));
		return ERROR_BAD_ENVIRONMENT;
	}
	
	m_nRowCount = m_RatingCapacityEntry.size();
	m_nColCount = m_ConduitSizeList.size();

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
int CCableLengthDataSheet::SetValue(const int& nRow, const int& nCol, const string& rValue)
{
	bool bFound = false;
	for(vector<CCableLengthDataSheet::VALUE>::iterator itr = m_ValueEntry.begin();itr != m_ValueEntry.end();++itr)
	{
		if((nRow == itr->row) && (nCol == itr->col))
		{
			itr->value = rValue;
			bFound = true;
			break;
		}
	}

	if(false == bFound)
	{
		CCableLengthDataSheet::VALUE value;
		value.row   = nRow;
		value.col   = nCol;
		value.value = rValue;

		m_ValueEntry.push_back(value);
	}

	if(m_nRowCount < nRow + 1) m_nRowCount = nRow + 1;
	if(m_nColCount < nCol + 1) m_nColCount = nCol + 1;

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
string CCableLengthDataSheet::GetValue(const int& nRow, const int& nCol)
{
	for(vector<CCableLengthDataSheet::VALUE>::iterator itr = m_ValueEntry.begin();itr != m_ValueEntry.end();++itr)
	{
		if((nRow == itr->row) && (nCol == itr->col)) return itr->value;
	}

	return string();
}

/**
	@brief	

	@author	BHK
*/
int CCableLengthDataSheet::GetRowCount(void) const
{
	return m_nRowCount;
}

/**
	@brief	

	@author	BHK
*/
int CCableLengthDataSheet::GetColCount(void) const
{
	return m_nColCount;
}

/**
	@brief	ConduitSizeList를 생성합니다.

	@author	HumKyung

	@date	????.??.??

	@param	rCableType
	@param	rCondiutTableName
	@param	nPercent
	@param	rhs

	@return	int
*/
int CCableLengthDataSheet::CreateConduitSizeList(const string& rCableType , const string& rConduitTableName , const double& nPercent , CCableLengthDataSheet::ConduitSizeList& rhs)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(rCableType);
	if(pCableDataByType && !rConduitTableName.empty())
	{
		rhs.clear();

		docData.m_ConduitTable.Load(rConduitTableName);
		const int nConduitCount = docData.m_ConduitTable.GetRecordCount();
		if(nConduitCount > 0)
		{
			vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
			for(vector<CCableDataByType::Record*>::iterator itr = pRecordList->begin();itr != pRecordList->end();++itr)
			{
				const double nCableRadius = atof((*itr)->m_FieldMap[_T("C_DIA")].c_str()) * 0.5;
				const double nCableSize   = atof((*itr)->m_FieldMap[_T("C_SIZE")].c_str());
				const double nCableArea   = nCableRadius * nCableRadius * 3.14;
				int i = nConduitCount - 1;
				for(i = nConduitCount - 1;i >= 0;--i)
				{
					CConduitTable::Record* pRecord = docData.m_ConduitTable.GetRecordAt(i);
					if(pRecord)
					{
						const double nConduitRadius = atof(pRecord->CONDUIT.c_str()) * 0.5;
						const double nConduitArea   = nConduitRadius * nConduitRadius * 3.14 * nPercent;
						if(nCableArea > nConduitArea) break;
					}
				}

				int nConduitIndex = (i + 1 > nConduitCount - 1) ? nConduitCount - 1 : i + 1;

				CCableLengthDataSheet::ConduitSizeData ConduitSizeData;
				ConduitSizeData.CableSize   = nCableSize;
				ConduitSizeData.ConduitSize = atof(docData.m_ConduitTable.GetRecordAt(nConduitIndex)->CONDUIT.c_str());
				rhs.push_back( ConduitSizeData );
			}
		}

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}
