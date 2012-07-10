#include "StdAfx.h"
#include "../ELoad.h"
#include "../ELoadDocData.h"
#include "FLCTable.h"

using namespace TABLES;

#define	FLC_TABLE_NAME	"T_FLC"

CFLCTable::CFLCTable(void)
{
}

CFLCTable::~CFLCTable(void)
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
int TABLES::CFLCTable::Load(void)
{
	static bool __ALREADY_LOADED__ = false;

	if(false == __ALREADY_LOADED__)
	{
		CELoadDocData& docData    = CELoadDocData::GetInstance();
		//! MDB CONNECT
		STRING_T rProjectMDBFilePath = docData.GetProjectMDBFilePath();

		CADODB adoDB;
		const STRING_T rDBPath = STRING_T(PROVIDER) + _T("Data Source=") + rProjectMDBFilePath + DB_PASSWORD;
		if(TRUE == adoDB.DBConnect(rDBPath))
		{
			CString rQuery("SELECT * FROM "FLC_TABLE_NAME);
			try
			{ 
				adoDB.OpenQuery(rQuery.operator LPCTSTR());

				LONG lRecordCount = 0;
				adoDB.GetRecordCount(&lRecordCount);
				for(int i = 0;i < lRecordCount;i++)
				{
					CFLCTable::Record* pRecord = new CFLCTable::Record;
					if(pRecord)
					{
						STRING_T rValue;
						adoDB.GetFieldValue(i , "C_VALUE" , &rValue);
						pRecord->VALUE = rValue;

						Insert(pRecord);
					}
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
	@brief	

	@author	BHK
*/
int TABLES::CFLCTable::Save(void)
{
	CELoadDocData& docData    = CELoadDocData::GetInstance();
	//! MDB CONNECT
	STRING_T rProjectMDBFilePath = docData.GetProjectMDBFilePath();

	CADODB adoDB;
	const STRING_T rDBPath = STRING_T(PROVIDER) + _T("Data Source=") + rProjectMDBFilePath + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rDBPath))
	{
		if(FALSE == adoDB.OpenQuery("SELECT * FROM "FLC_TABLE_NAME))
		{
			CString rExecPath = GetExecPath();
			if("\\" != rExecPath.Right(1)) rExecPath += "\\";
			CString rSqlFilePath = rExecPath + "Backup\\"FLC_TABLE_NAME".SQL";

			//! synchronize table structure.
			CSQLFile sqlFile(rSqlFilePath.operator LPCSTR());
			sqlFile.Parse();
			string rSql = sqlFile.GetCreateString();
			if(!rSql.empty()) adoDB.ExecuteQuery(rSql.c_str());
			
			//! insert data into T_FLC table.
			for(vector<Record*>::iterator itr = m_RecordEntry.begin();itr != m_RecordEntry.end();++itr)
			{
				rSql = "INSERT INTO " + string(FLC_TABLE_NAME) + "(C_VALUE) VALUES('";
				rSql+= (*itr)->VALUE + "')";

				adoDB.ExecuteQuery(rSql.c_str());
			}

			return ERROR_SUCCESS;
		}
		else
		{
			adoDB.ExecuteQuery("DELETE * FROM "FLC_TABLE_NAME);

			string rSql;
			for(vector<Record*>::iterator itr = m_RecordEntry.begin();itr != m_RecordEntry.end();++itr)
			{
				rSql = "INSERT INTO " + string(FLC_TABLE_NAME) + "(C_VALUE) VALUES('";
				rSql+= (*itr)->VALUE + "')";

				adoDB.ExecuteQuery(rSql.c_str());
			}
		}
	}
	else
	{
		AfxMessageBox("Fail to connect project database... !");
		return ERROR_BAD_ENVIRONMENT;
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
int CFLCTable::Insert(CFLCTable::Record* pRecord)
{
	assert(pRecord && "pRecord is NULL");

	if(pRecord) m_RecordEntry.push_back(pRecord);
	
	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
int TABLES::CFLCTable::Clear(void)
{
	for(vector<Record*>::iterator itr = m_RecordEntry.begin();itr != m_RecordEntry.end();++itr)
	{
		SAFE_DELETE(*itr);
	}

	m_RecordEntry.clear();

	return ERROR_SUCCESS;
}