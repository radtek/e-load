#include "StdAfx.h"
#include "../ELoad.h"
#include <util/ado/SQLFile.h>
#include "1PHFeederCableSizingTable.h"
#include "../ELoadDocData.h"

using namespace TABLES;

#define	T_1PH_FEEDER_CABLE_SIZING	_T("T_1PH_FEEDER_CABLE_SIZING")

C1PHFeederCableSizingTable::C1PHFeederCableSizingTable(void)
{
}

C1PHFeederCableSizingTable& C1PHFeederCableSizingTable::GetInstance(void)
{
	static C1PHFeederCableSizingTable __INSTANCE__;

	return __INSTANCE__;
}

C1PHFeederCableSizingTable::~C1PHFeederCableSizingTable(void)
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
int C1PHFeederCableSizingTable::Insert(C1PHFeederCableSizingTable::Record record)
{
	m_RecordEntry.push_back(record);
	
	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
int TABLES::C1PHFeederCableSizingTable::Clear(void)
{
	m_RecordEntry.clear();

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
int TABLES::C1PHFeederCableSizingTable::Load()
{
	static bool __ALREADY_LOADED__ = false;
	
	if(false == __ALREADY_LOADED__)
	{
		CELoadDocData& docData    = CELoadDocData::GetInstance();
		//! MDB CONNECT
		const STRING_T rLibraryMDBFilePath = docData.GetCommonLibraryMDBFilePath();

		CADODB adoDB;
		const STRING_T rConnectionString = STRING_T(_T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=")) + rLibraryMDBFilePath + DB_PASSWORD;
		if(TRUE == adoDB.DBConnect(rConnectionString))
		{
			STRING_T rQuery(_T("SELECT * FROM "T_1PH_FEEDER_CABLE_SIZING));
			try
			{ 
				adoDB.OpenQuery(rQuery);

				LONG lRecordCount = 0;
				adoDB.GetRecordCount(&lRecordCount);
				for(int i = 0;i < lRecordCount;++i)
				{
					C1PHFeederCableSizingTable::Record record;

					STRING_T rValue;
					adoDB.GetFieldValue(i , _T("C_KW") , &rValue);
					record.rKW = rValue;

					Insert(record);
				}

				__ALREADY_LOADED__ = true;
			}
			catch(_com_error &e)
			{
				CString rMsg;
				rMsg.Format("*** DOG CHECK [ADO DB] => <ExecuteQuery 오류> 설명[%s] !! ***\n", (LPCSTR)e.Description());
				ELOAD_LOG4CXX_WARN(mylogger , "" , 1);

				return ERROR_BAD_ENVIRONMENT;
			}
		}
		else
		{
			AfxMessageBox("Fail to connect project database... !");
			return ERROR_BAD_ENVIRONMENT;
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	CONDUIT 테이블을 저장한다.

	@author	BHK
*/
int TABLES::C1PHFeederCableSizingTable::Save()
{
	CELoadDocData& docData    = CELoadDocData::GetInstance();
	//! MDB CONNECT
	const STRING_T rLibraryMDBFilePath = docData.GetCommonLibraryMDBFilePath();

	CADODB adoDB;
	const STRING_T rConnectionString = STRING_T(PROVIDER) + STRING_T(_T("Data Source=")) + rLibraryMDBFilePath + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rConnectionString))
	{
		if(FALSE == adoDB.OpenQuery("SELECT * FROM "T_1PH_FEEDER_CABLE_SIZING))
		{
			CString rExecPath = GetExecPath();
			if("\\" != rExecPath.Right(1)) rExecPath += _T("\\");
			CString rSqlFilePath = rExecPath + "Backup\\"T_1PH_FEEDER_CABLE_SIZING".SQL";

			CSQLFile sqlFile(rSqlFilePath.operator LPCSTR());
			sqlFile.Parse();
			const string rSql = sqlFile.GetCreateString();
			if(!rSql.empty()) adoDB.ExecuteQuery(rSql.c_str());
		}
		else	adoDB.ExecuteQuery(_T("DELETE * FROM ")T_1PH_FEEDER_CABLE_SIZING);

		for(vector<Record>::iterator itr = m_RecordEntry.begin();itr != m_RecordEntry.end();++itr)
		{
			string rSql = _T("INSERT INTO ")T_1PH_FEEDER_CABLE_SIZING"(C_KW) VALUES('";
			rSql+= (itr)->rKW + "')";

			adoDB.ExecuteQuery(rSql.c_str());
		}

		return ERROR_SUCCESS;
	}
	else
	{
		AfxMessageBox("Fail to connect project database... !");
		return ERROR_BAD_ENVIRONMENT;
	}

	return ERROR_SUCCESS;
}
