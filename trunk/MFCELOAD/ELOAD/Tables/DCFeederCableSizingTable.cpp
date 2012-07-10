#include "StdAfx.h"
#include "../ELoad.h"
#include <util/ado/SQLFile.h>
#include "DCFeederCableSizingTable.h"
#include "../ELoadDocData.h"

using namespace TABLES;

#define	T_DC_FEEDER_CABLE_SIZING	"T_DC_FEEDER_CABLE_SIZING"

CDCFeederCableSizingTable::CDCFeederCableSizingTable(void)
{
}

CDCFeederCableSizingTable& CDCFeederCableSizingTable::GetInstance(void)
{
	static CDCFeederCableSizingTable __INSTANCE__;

	return __INSTANCE__;
}

CDCFeederCableSizingTable::~CDCFeederCableSizingTable(void)
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
int CDCFeederCableSizingTable::Insert(CDCFeederCableSizingTable::Record record)
{
	m_RecordEntry.push_back(record);
	
	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
int TABLES::CDCFeederCableSizingTable::Clear(void)
{
	m_RecordEntry.clear();

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
int TABLES::CDCFeederCableSizingTable::Load()
{
	static bool __ALREADY_LOADED__ = false;

	if(false == __ALREADY_LOADED__)
	{
		CELoadDocData& docData    = CELoadDocData::GetInstance();
		//! MDB CONNECT
		const string rLibraryMDBFilePath = docData.GetCommonLibraryMDBFilePath();

		CADODB adoDB;
		const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rLibraryMDBFilePath.c_str() + DB_PASSWORD;
		if(TRUE == adoDB.DBConnect(rConnectionString))
		{
			CString rQuery(string("SELECT * FROM "T_DC_FEEDER_CABLE_SIZING).c_str());
			try
			{ 
				adoDB.OpenQuery(rQuery.operator LPCTSTR());

				LONG lRecordCount = 0;
				adoDB.GetRecordCount(&lRecordCount);
				for(int i = 0;i < lRecordCount;i++)
				{
					CDCFeederCableSizingTable::Record record;

					STRING_T rValue;
					adoDB.GetFieldValue(i , "C_KW" , &rValue);
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
int TABLES::CDCFeederCableSizingTable::Save()
{
	CELoadDocData& docData    = CELoadDocData::GetInstance();
	//! MDB CONNECT
	const string rLibraryMDBFilePath = docData.GetCommonLibraryMDBFilePath();

	CADODB adoDB;
	const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rLibraryMDBFilePath + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rConnectionString))
	{
		if(FALSE == adoDB.OpenQuery("SELECT * FROM "T_DC_FEEDER_CABLE_SIZING))
		{
			CString rExecPath = GetExecPath();
			if("\\" != rExecPath.Right(1)) rExecPath += "\\";
			CString rSqlFilePath = rExecPath + "Backup\\"T_DC_FEEDER_CABLE_SIZING".SQL";

			CSQLFile sqlFile(rSqlFilePath.operator LPCSTR());
			sqlFile.Parse();
			const string rSql = sqlFile.GetCreateString();
			if(!rSql.empty()) adoDB.ExecuteQuery(rSql.c_str());
		}
		else	adoDB.ExecuteQuery("DELETE * FROM "T_DC_FEEDER_CABLE_SIZING);

		for(vector<Record>::iterator itr = m_RecordEntry.begin();itr != m_RecordEntry.end();++itr)
		{
			string rSql = "INSERT INTO "T_DC_FEEDER_CABLE_SIZING"(C_KW) VALUES('";
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
