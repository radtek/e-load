#include "StdAfx.h"
#include <util/ado/ADODB.h>
#include "../ELoad.h"
#include "ConduitListTable.h"
#include "../ELoadDocData.h"
#include <util/ado/SQLFile.h>

#define	T_CONDUIT_LIST	"T_CONDUIT_LIST"

CConduitListTable::CConduitListTable(void)
{
}

CConduitListTable::~CConduitListTable(void)
{
}

CConduitListTable& CConduitListTable::GetInstance()
{
	static CConduitListTable __INSTANCE__;

	return __INSTANCE__;
}

/**
	@brief	T_CONDUIT_LIST 테이블을 읽는다.

	@author	HumKyung

	@date	????.??.??

	@param

	@return	int
*/
int TABLES::CConduitListTable::Load(void)
{
	static bool __ALREADY_LOADED__ = false;

	if(false == __ALREADY_LOADED__)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();

		string rCommonLibraryMDBFilePath = docData.GetCommonLibraryMDBFilePath();
		CADODB adoDB;
		const STRING_T rProvider = STRING_T(PROVIDER) + _T("Data Source=") + rCommonLibraryMDBFilePath + DB_PASSWORD;
		if(TRUE == adoDB.DBConnect(rProvider))
		{
			STRING_T rQuery(STRING_T(_T("SELECT * FROM ")) + T_CONDUIT_LIST);
			try
			{ 
				adoDB.OpenQuery(rQuery);

				LONG lRecordCount = 0;
				adoDB.GetRecordCount(&lRecordCount);
				for(int i = 0;i < lRecordCount;++i)
				{
					STRING_T rValue;

					TABLE_RECORD record;
					adoDB.GetFieldValue(i , _T("C_TABLE_NAME") , &rValue);
					record.rTableNo = rValue;
					adoDB.GetFieldValue(i , _T("C_TABLE_DESC") , &rValue);
					record.rTableDesc = rValue;

					m_TableRecordEntry.push_back(record);
				}

				__ALREADY_LOADED__ = true;
			}
			catch(_com_error &e)
			{
				CString rMsg;
				rMsg.Format("*** DOG CHECK [ADO DB] => <ExecuteQuery 오류> 설명[%s] !! ***\n", (LPCSTR)e.Description());
				ELOAD_LOG4CXX_WARN(mylogger , "" , 1);
			}
		}
		else
		{
			AfxMessageBox("FAIL TO CONNET DATABASE!");
			return ERROR_BAD_ENVIRONMENT;
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
int TABLES::CConduitListTable::AddTable(const string& rTableName , const string& rTableDesc)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	for(vector<TABLE_RECORD>::iterator itr = m_TableRecordEntry.begin();itr != m_TableRecordEntry.end();++itr)
	{
		CString lhs(rTableName.c_str());
		lhs.MakeUpper();
		CString rhs(itr->rTableNo.c_str());
		rhs.MakeUpper();
		if(lhs == rhs) return ERROR_SUCCESS;
	}

	STRING_T rCommonLibraryMDBFilePath = docData.GetCommonLibraryMDBFilePath();

	CADODB adoDB;
	const STRING_T rProvider = STRING_T(PROVIDER) + _T("Data Source=") + rCommonLibraryMDBFilePath + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rProvider))
	{
		if(FALSE == adoDB.OpenQuery(string("SELECT * FROM "T_CONDUIT_LIST).c_str()))
		{
			//! create table if not exist
			CString rExecPath = GetExecPath();
			if("\\" != rExecPath.Right(1)) rExecPath += "\\";
			CString rSqlFilePath = rExecPath + "Backup\\"T_CONDUIT_LIST".SQL";

			CSQLFile sqlFile(rSqlFilePath.operator LPCSTR());
			sqlFile.Parse();
			STRING_T rSql = sqlFile.GetCreateString();
			
			if(!rSql.empty()) adoDB.ExecuteQuery(rSql);
		}

		STRING_T rQuery = STRING_T(_T("INSERT INTO ")) + STRING_T(T_CONDUIT_LIST) + STRING_T("(C_TABLE_NAME,C_TABLE_DESC) VALUES ") +
			_T("('") + rTableName + _T("','") + rTableDesc + _T("')");
		if(FALSE == adoDB.ExecuteQuery(rQuery))
		{
			AfxMessageBox( _T("Fail to insert record") );
			return ERROR_BAD_ENVIRONMENT;
		}

		TABLE_RECORD record;
		record.rTableNo   = rTableName;
		record.rTableDesc = rTableDesc;
		m_TableRecordEntry.push_back(record);
	}
	else
	{
		AfxMessageBox("Fail to connect db.");
		return ERROR_BAD_ENVIRONMENT;
	}

	return ERROR_SUCCESS;
}

/**
	@brief	CONDUIT TABLE을 삭제하고 T_CONDUIT_LIST에서 TABLE NAME을 가진 레코드를 삭제한다.

	@author	BHK
*/
int TABLES::CConduitListTable::DeleteTable(const string& rTableName)
{
	CELoadDocData& docData    = CELoadDocData::GetInstance();
	//! MDB CONNECT
	string rCommonLibraryMDBFilePath = docData.GetCommonLibraryMDBFilePath();

	CADODB adoDB;
	const STRING_T rDBPath = STRING_T(PROVIDER) + _T("Data Source=") + rCommonLibraryMDBFilePath + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rDBPath))
	{
		try
		{
			adoDB.ExecuteQuery(("DROP TABLE [" + rTableName + "]").c_str());

			CString rSql(string("DELETE * FROM "T_CONDUIT_LIST" WHERE C_TABLE_NAME='" + rTableName + "'").c_str());
			try
			{ 
				adoDB.ExecuteQuery(rSql.operator LPCTSTR());
			}
			catch(_com_error &e)
			{
				CString rMsg;
				rMsg.Format("*** DOG CHECK [ADO DB] => <ExecuteQuery 오류> 설명[%s] !! ***\n", (LPCSTR)e.Description());
				ELOAD_LOG4CXX_WARN(mylogger , "" , 3);

				AfxMessageBox(rMsg);
			}

			for(vector<TABLE_RECORD>::iterator itr = m_TableRecordEntry.begin();itr != m_TableRecordEntry.end();++itr)
			{
				CString lhs(rTableName.c_str());
				lhs.MakeUpper();
				CString rhs(itr->rTableNo.c_str());
				rhs.MakeUpper();
				if(lhs == rhs)
				{
					m_TableRecordEntry.erase(itr);
					break;
				}
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