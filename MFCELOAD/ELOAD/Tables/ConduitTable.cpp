#include "StdAfx.h"
#include "../ELoad.h"
#include "../ELoadDocData.h"
#include "ConduitTable.h"
#include <util/ado/SQLFile.h>

using namespace TABLES;

#define	CONDUIT_TABLE_NAME	"T_CONDUIT"
#define	T_CONDUIT_LIST		"T_CONDUIT_LIST"

CConduitTable::CConduitTable(void)
{
}

CConduitTable::~CConduitTable(void)
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
int CConduitTable::Insert(CConduitTable::Record* pRecord)
{
	assert(pRecord && "pRecord is NULL");

	if(pRecord) m_RecordEntry.push_back(pRecord);
	
	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
int TABLES::CConduitTable::Clear(void)
{
	for(vector<Record*>::iterator itr = m_RecordEntry.begin();itr != m_RecordEntry.end();++itr)
	{
		SAFE_DELETE(*itr);
	}

	m_RecordEntry.clear();

	return ERROR_SUCCESS;
}

/**
	@brief	load condiut table which's name is rTableName

	@author	HumKyung

	@date	????.??.??

	@param	rTableName

	@return	int
*/
int TABLES::CConduitTable::Load(const string& rTableName)
{
	{
		CELoadDocData& docData    = CELoadDocData::GetInstance();
		//! MDB CONNECT
		string rCommonLibraryMDBFilePath = docData.GetCommonLibraryMDBFilePath();

		CADODB adoDB;
		const STRING_T rDBPath = STRING_T(PROVIDER) + _T("Data Source=") + rCommonLibraryMDBFilePath + DB_PASSWORD;
		if(TRUE == adoDB.DBConnect(rDBPath))
		{
			STRING_T rQuery(_T("SELECT * FROM ") + rTableName);
			try
			{ 
				adoDB.OpenQuery(rQuery);

				LONG lRecordCount = 0;
				adoDB.GetRecordCount(&lRecordCount);
				for(int i = 0;i < lRecordCount;i++)
				{
					CConduitTable::Record* pRecord = new CConduitTable::Record;
					if(pRecord)
					{
						STRING_T rValue;
						adoDB.GetFieldValue(i , "C_CONDUIT" , &rValue);
						pRecord->CONDUIT = rValue;
						adoDB.GetFieldValue(i , "C_DIA" , &rValue);
						pRecord->DIA = rValue;

						Insert(pRecord);
					}
				}
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
int TABLES::CConduitTable::Save(const string& rTableName)
{
	CELoadDocData& docData    = CELoadDocData::GetInstance();
	//! MDB CONNECT
	string rCommonLibraryMDBFilePath = docData.GetCommonLibraryMDBFilePath();

	CADODB adoDB;
	const STRING_T rDBPath = STRING_T(PROVIDER) + _T("Data Source=") + rCommonLibraryMDBFilePath + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rDBPath))
	{
		if(FALSE == adoDB.OpenQuery(("SELECT * FROM " + rTableName).c_str()))
		{
			CString rExecPath = GetExecPath();
			if("\\" != rExecPath.Right(1)) rExecPath += "\\";
			CString rSqlFilePath = rExecPath + "Backup\\T_CONDUIT.SQL";

			CSQLFile sqlFile(rSqlFilePath.operator LPCSTR());
			sqlFile.Parse();
			string rSql = sqlFile.GetCreateString();
			if(!rSql.empty())
			{
				CString sql(rSql.c_str());
				sql.Replace("?" , rTableName.c_str());
				adoDB.ExecuteQuery(sql.operator LPCTSTR());
			}

			for(vector<Record*>::iterator itr = m_RecordEntry.begin();itr != m_RecordEntry.end();++itr)
			{
				rSql = "INSERT INTO " + rTableName + "(C_CONDUIT,C_DIA) VALUES('";
				rSql+= (*itr)->CONDUIT + "','" + (*itr)->DIA + "')";

				adoDB.ExecuteQuery(rSql.c_str());
			}

			return ERROR_SUCCESS;
		}
		else
		{
			adoDB.ExecuteQuery(("DELETE * FROM " + rTableName).c_str());

			string rSql;
			for(vector<Record*>::iterator itr = m_RecordEntry.begin();itr != m_RecordEntry.end();++itr)
			{
				rSql = "INSERT INTO " + rTableName + "(C_CONDUIT,C_DIA) VALUES('";
				rSql+= (*itr)->CONDUIT + "','" + (*itr)->DIA + "')";

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
	@brief	CONDUIT TABLE을 삭제하고 T_CONDUIT_LIST에서 TABLE NAME을 가진 레코드를 삭제한다.

	@author	BHK
*/
/*
int TABLES::CConduitTable::Delete(const string& rTableName)
{
	CELoadDocData& docData    = CELoadDocData::GetInstance();
	//! MDB CONNECT
	string rCommonLibraryMDBFilePath = docData.GetCommonLibraryMDBFilePath();

	CADODB adoDB;
	CString rDBPath = CString("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=") + rCommonLibraryMDBFilePath.c_str();

	if(TRUE == adoDB.DBConnect(rDBPath))
	{
		try
		{
			adoDB.ExecuteQuery(("DROP TABLE " + rTableName).c_str());

			CString rSql(string("DELETE * FROM "T_CONDUIT_LIST" WHERE C_TABLE_NAME='" + rTableName + "'").c_str());
			try
			{ 
				adoDB.ExecuteQuery(rSql);

			}
			catch(_com_error &e)
			{
				CString rMsg;
				rMsg.Format("*** DOG CHECK [ADO DB] => <ExecuteQuery 오류> 설명[%s] !! ***\n", (LPCSTR)e.Description());
				ELOAD_LOG4CXX_WARN(mylogger , rMsg);

				AfxMessageBox(rMsg);
			}
		}
		catch(_com_error & e)
		{
			CString rMsg;
			rMsg.Format("*** DOG CHECK [ADO DB] => <ExecuteQuery 오류> 설명[%s] !! ***\n", (LPCSTR)e.Description());
			AfxMessageBox(rMsg);
			
			return ERROR_BAD_ENVIRONMENT;
		}
	}
	else
	{
		AfxMessageBox("Fail to connect common library database... !");
		return ERROR_BAD_ENVIRONMENT;
	}

	return ERROR_SUCCESS;
}
*/