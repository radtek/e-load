#include "StdAfx.h"
#include "../ELoad.h"
#include "../ELoadDocData.h"
#include <util/ado/SQLFile.h>
#include "ProjectSettingTable.h"

using namespace TABLES;

static const string __TABLE_NAME__ = _T("T_PROJECT_SETTING");

CProjectSettingTable::CProjectSettingTable(void)
{
}

/**
	@brief	

	@author BHK	

	@date 2009-04-15 오후 3:14:19	

	@param	

	@return		
*/
const string CProjectSettingTable::GetTableName()
{
	return __TABLE_NAME__;
}

CProjectSettingTable::~CProjectSettingTable(void)
{
}

/**
	@brief	T_PROJECT_SETTING 테이블을 읽는다.

	@param	rMDBFilePath = 로딩할 MDB FILE PATH , 만일 ""이면 현재 PROJECT MDB FILE PATH가 된다.

	@author	BHK
*/
int CProjectSettingTable::Load(const string& rMDBFilePath,const string& rTableName)
{
	if(m_RecordEntry.empty())
	{
		string rProjectMDBFilePath = rMDBFilePath;
		if("" == rProjectMDBFilePath)
		{
			CELoadDocData& docData = CELoadDocData::GetInstance();
			rProjectMDBFilePath= docData.GetProjectMDBFilePath();
		}
		m_rMDBFilePath = rProjectMDBFilePath;
		
		m_rTableName = rTableName;
		CELOADTable::Load(m_rMDBFilePath , m_rTableName);

		//! 기본적으로 하나의 RECORD를 가지도록 한다.
		if(m_RecordEntry.empty())
		{
			CADODB adoDB;
			const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;
			if(TRUE == adoDB.DBConnect(rConnectionString))
			{	
				const CString rQuery = "INSERT INTO " + CString(rTableName.c_str()) + "(C_CUR_TABLE_FOR_LOAD_SUMMARY) VALUES ('')";
				if(FALSE == adoDB.ExecuteQuery(rQuery.operator LPCTSTR()))
				{
					AfxMessageBox(_T("Fail to insert into Project Setting Table"));
					return ERROR_BAD_ENVIRONMENT;
				}
				if(FALSE == adoDB.OpenQuery(_T("SELECT * FROM ") + rTableName))
				{
					CString rExecPath = GetExecPath();
					if("\\" != rExecPath.Right(1)) rExecPath += "\\";
					CString rSqlFilePath = rExecPath + "Backup\\" + CString(m_rTableName.c_str()) + ".SQL";

					CSQLFile sqlFile(rSqlFilePath.operator LPCSTR());
					if(ERROR_SUCCESS != sqlFile.Parse()) return ERROR_BAD_ENVIRONMENT;

					const int nFieldCount = sqlFile.GetColumnCount();

					CELOADTable::Record record;
					for(int i = 0;i < nFieldCount;++i)
					{
						const string rFieldName = sqlFile.GetFieldNameAt(i);
						record.FieldValueMap[rFieldName.c_str()] = _T("");
					}

					m_RecordEntry.push_back(record);
				}
				else
				{
					const int nFieldCount = adoDB.GetFieldCount();
					LONG lRecordCount = 0L;
					adoDB.GetRecordCount(&lRecordCount);
					for(long lRecord = 0;lRecord < lRecordCount;++lRecord)
					{
						CELOADTable::Record record;
						for(int i = 0;i < nFieldCount;++i)
						{
							STRING_T rValue;
							STRING_T rFieldName = adoDB.GetFieldName(i);
							adoDB.GetFieldValue(lRecord , rFieldName , &rValue);
							record.FieldValueMap.insert(make_pair(rFieldName,rValue));
						}
						m_RecordEntry.push_back(record);
					}
				}
			}
			else
			{
				AfxMessageBox(_T("Fail to connect database.h"));
				return ERROR_BAD_ENVIRONMENT;
			}

			//! INSERT 구문을 추가.
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	T_PROJECT_SETTING 테이블을 저장한다.

	@author	BHK
*/
/*
int CProjectSettingTable::Save(void)
{
	///CELoadDocData& docData     = CELoadDocData::GetInstance();
	string rProjectMDBFilePath = m_rMDBFilePath;///docData.GetProjectMDBFilePath();

        CADODB adoDB;
        string rProvider = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + rProjectMDBFilePath + DB_PASSWORD;

        if(TRUE == adoDB.DBConnect(rProvider.c_str()))
        {
		SyncToSqlFile(adoDB);
		for(map<string,string>::iterator itr = m_FieldMap.begin();itr != m_FieldMap.end();++itr)
		{
			if(string("ID") == itr->first) continue;	//! KEY는 업데이트하지 않는다.

			CString rQuery = "UPDATE " + CString(__TABLE_NAME__.c_str()) + " SET [" + CString(itr->first.c_str()) + "]='"+ CString(itr->second.c_str()) + "'";
			if(FALSE == adoDB.ExecuteQuery(rQuery))
			{
				CString rMsg;
				rMsg.Format("FAIL TO UPDATE FIELD(%s)", itr->first.c_str());
				ELOAD_LOG4CXX_WARN(mylogger , rMsg);
			}
		}
        }
	else
	{
                AfxMessageBox("FAIL TO CONNET DATABASE !");
                return ERROR_BAD_ENVIRONMENT;
	}

	return ERROR_SUCCESS;
}
*/

/**
	@brief	FIELD의 VALUE를 구한다.

	@author	BHK
*/
string* CProjectSettingTable::GetFieldValue(const string& rFieldName)
{
	if(!m_RecordEntry.empty())
	{
		map<string,string>::iterator where = m_RecordEntry.front().FieldValueMap.find(rFieldName);
		if(where != m_RecordEntry.front().FieldValueMap.end()) return &(where->second);
	}

	return NULL;
}

/**
	@brief	FIELD의 VALUE를 설정한다.

	@author	BHK
*/
int CProjectSettingTable::SetFieldValue(const string& rFieldName , const string& rFieldValue)
{
	if(m_RecordEntry.empty())
	{
		CELOADTable::Record record;

		m_RecordEntry.push_back(record);
	}

	if(!m_RecordEntry.empty())
	{
		map<string,string>::iterator where = m_RecordEntry.front().FieldValueMap.find(rFieldName);
		if(where != m_RecordEntry.front().FieldValueMap.end())
		{
			(where->second = rFieldValue);
			return ERROR_SUCCESS;
		}
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	TABLE에 필드를 추가한다.

	@author	BHK
*/
int TABLES::CProjectSettingTable::AddField(const string& rFieldName)
{
	if(m_RecordEntry.empty())
	{
		CELOADTable::Record record;

		m_RecordEntry.push_back(record);
	}

	if(!m_RecordEntry.empty())
	{
		map<string,string>::iterator where = m_RecordEntry.front().FieldValueMap.find(rFieldName);
		if(where == m_RecordEntry.front().FieldValueMap.end())
		{
			m_RecordEntry.front().FieldValueMap.insert(make_pair(rFieldName,""));
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
string TABLES::CProjectSettingTable::GetMDBFilePath(void)
{
	return m_rMDBFilePath;
}