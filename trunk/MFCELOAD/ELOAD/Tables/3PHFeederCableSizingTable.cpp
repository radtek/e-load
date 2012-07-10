#include "StdAfx.h"
#include "../ELoad.h"
#include <util/ado/SQLFile.h>
#include "3PHFeederCableSizingTable.h"
#include "../ELoadDocData.h"

using namespace TABLES;

#define	T_3PH_FEEDER_CABLE_SIZING	_T("T_3PH_FEEDER_CABLE_SIZING")

C3PHFeederCableSizingTable::C3PHFeederCableSizingTable(void)
{
}

C3PHFeederCableSizingTable& C3PHFeederCableSizingTable::GetInstance(void)
{
	static C3PHFeederCableSizingTable __INSTANCE__;

	return __INSTANCE__;
}

C3PHFeederCableSizingTable::~C3PHFeederCableSizingTable(void)
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
int C3PHFeederCableSizingTable::Insert(C3PHFeederCableSizingTable::Record record)
{
	m_RecordEntry.push_back(record);
	
	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
int TABLES::C3PHFeederCableSizingTable::Clear(void)
{
	m_RecordEntry.clear();

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
int TABLES::C3PHFeederCableSizingTable::Load()
{
        static bool __ALREADY_LOADED__ = false;
	
	if(false == __ALREADY_LOADED__)
	{
		CELoadDocData& docData    = CELoadDocData::GetInstance();
		//! MDB CONNECT
		const STRING_T rLibraryMDBFilePath = docData.GetCommonLibraryMDBFilePath();

		CADODB adoDB;
		const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rLibraryMDBFilePath.c_str() + DB_PASSWORD;
		if(TRUE == adoDB.DBConnect(rConnectionString))
		{
			STRING_T rQuery(_T("SELECT * FROM ")T_3PH_FEEDER_CABLE_SIZING);
			try
			{ 
				adoDB.OpenQuery(rQuery);

				LONG lRecordCount = 0;
				adoDB.GetRecordCount(&lRecordCount);
				for(int i = 0;i < lRecordCount;i++)
				{
					C3PHFeederCableSizingTable::Record record;

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
				rMsg.Format("*** DOG CHECK [ADO DB] => <ExecuteQuery ����> ����[%s] !! ***\n", (LPCSTR)e.Description());
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
	@brief	CONDUIT ���̺��� �����Ѵ�.

	@author	BHK
*/
int TABLES::C3PHFeederCableSizingTable::Save()
{
	CELoadDocData& docData    = CELoadDocData::GetInstance();
	//! MDB CONNECT
	const string rLibraryMDBFilePath = docData.GetCommonLibraryMDBFilePath();

	CADODB adoDB;
	const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rLibraryMDBFilePath.c_str() + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rConnectionString))
	{
		if(FALSE == adoDB.OpenQuery("SELECT * FROM "T_3PH_FEEDER_CABLE_SIZING))
		{
			CString rExecPath = GetExecPath();
			if("\\" != rExecPath.Right(1)) rExecPath += "\\";
			CString rSqlFilePath = rExecPath + "Backup\\"T_3PH_FEEDER_CABLE_SIZING".SQL";

			CSQLFile sqlFile(rSqlFilePath.operator LPCSTR());
			sqlFile.Parse();
			const string rSql = sqlFile.GetCreateString();
			if(!rSql.empty()) adoDB.ExecuteQuery(rSql.c_str());
		}
		else	adoDB.ExecuteQuery("DELETE * FROM "T_3PH_FEEDER_CABLE_SIZING);

		for(vector<Record>::iterator itr = m_RecordEntry.begin();itr != m_RecordEntry.end();++itr)
		{
			string rSql = "INSERT INTO "T_3PH_FEEDER_CABLE_SIZING"(C_KW) VALUES('";
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