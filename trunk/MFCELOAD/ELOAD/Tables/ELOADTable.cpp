#include "StdAfx.h"
#include <assert.h>
#include <util/ado/SQLFile.h>
#include "ELOADTable.h"

using namespace TABLES;

CELOADTable::CELOADTable(void)
{
}

CELOADTable::~CELOADTable(void)
{
	try
	{
	}
	catch(...)
	{
	}
}

/**
	@brief	SQL 파일과 형식을 일치시킨다.

	@author	BHK
*/
int TABLES::CELOADTable::SyncToSqlFile(CADODB& adoDB , CSQLFile& sqlFile)
{
	if(!m_rTableName.empty())
	{
		bool bFoundStateField = false;
		const int nFieldCount = sqlFile.GetColumnCount();
		for(int i = 0;i < nFieldCount;++i)
		{
			string rFieldName = sqlFile.GetFieldNameAt(i);
			if(rFieldName.empty()) continue;

			vector<string>::iterator where = find(m_FieldNameEntry.begin() , m_FieldNameEntry.end() , rFieldName);
			if(where == m_FieldNameEntry.end())
			{
				//! 필드가 존재하지 않으면 테이블에 필드를 생성한다.
				adoDB.ExecuteQuery(string(_T("ALTER TABLE ") + m_rTableName + _T(" ADD ") + sqlFile.GetFieldDataAt(i)).c_str());
				m_FieldNameEntry.push_back(rFieldName);
			}
		}
		vector<string>::iterator where = find(m_FieldNameEntry.begin() , m_FieldNameEntry.end() , _T("C_STATE"));
		if(where == m_FieldNameEntry.end())
		{
			//! 필드가 존재하지 않으면 테이블에 필드를 생성한다.
			adoDB.ExecuteQuery(_T("ALTER TABLE ") + m_rTableName + _T(" ADD C_STATE VARCHAR(8)"));
			m_FieldNameEntry.push_back(_T("C_STATE"));
		}

		for(vector<STRING_T>::iterator itr = m_FieldNameEntry.begin();itr != m_FieldNameEntry.end();)
		{
			if((_T("C_STATE") != (*itr)) && (false == sqlFile.ExistColumnName(*itr)))
			{
				//! 불필요한 필드는 테이블에서 삭제한다.
				adoDB.ExecuteQuery(string(_T("ALTER TABLE ") + m_rTableName + _T(" DROP [") + *itr + _T("]")).c_str());
				itr = m_FieldNameEntry.erase(itr);
			}else	itr++;
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	테이블을 생성합니다.

	@author BHK	

	@date 2009-04-28 오전 9:36:32	

	@param	

	@return		
*/
int TABLES::CELOADTable::CreateTable(const string& rMDBFilePath,const string& rTableName)
{
	m_FieldNameEntry.clear();
	m_RecordEntry.clear();

	CADODB adoDB;
	const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rConnectionString))
	{
		m_rTableName = rTableName;
		CString rExecPath = GetExecPath();
		if("\\" != rExecPath.Right(1)) rExecPath += "\\";
		CString rSqlFilePath = rExecPath + "Backup\\" + CString(m_rTableName.c_str()) + ".SQL";
		
		CSQLFile sqlFile(rSqlFilePath.operator LPCSTR());
		if(ERROR_SUCCESS != sqlFile.Parse()) return ERROR_BAD_ENVIRONMENT;

		m_rMDBFilePath = rMDBFilePath;
		if(FALSE == adoDB.OpenQuery(_T("SELECT * FROM ") + m_rTableName))
		{
			//! 테이블이 없으므로 테이블을 생성한다.	
			const string rSql = sqlFile.GetCreateString();
			if(!rSql.empty()) adoDB.ExecuteQuery(rSql.c_str());
		}
		else
		{
			//! database의 필드명을 미리 저장해 둔다.
			int nFieldCount = adoDB.GetFieldCount();
			for(int i = 0;i < nFieldCount;++i) m_FieldNameEntry.push_back(adoDB.GetFieldName(i));
			
			SyncToSqlFile(adoDB , sqlFile);
		}

		return ERROR_SUCCESS;
	}
	else
	{
		AfxMessageBox(_T("Fail to connect database.h"));
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	load data from database and then sync database field to sqlfile.

	@author	BHK

	@date	?

	@param
	@param

	@return
*/
int TABLES::CELOADTable::Load(const string& rMDBFilePath,const string& rTableName)
{
	m_FieldNameEntry.clear();
	m_RecordEntry.clear();

	CADODB adoDB;
	const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rConnectionString))
	{
		m_rTableName = rTableName;
		CString rExecPath = GetExecPath();
		if(_T("\\") != rExecPath.Right(1)) rExecPath += _T("\\");
		CString rSqlFilePath = rExecPath + _T("Backup\\") + CString(m_rTableName.c_str()) + _T(".SQL");
		
		CSQLFile sqlFile(rSqlFilePath.operator LPCSTR());
		if(ERROR_SUCCESS != sqlFile.Parse()) return ERROR_BAD_ENVIRONMENT;

		m_rMDBFilePath = rMDBFilePath;
		if(FALSE == adoDB.OpenQuery(_T("SELECT * FROM ") + m_rTableName))
		{
			//! 테이블이 없으므로 테이블을 생성한다.	
			const string rSql = sqlFile.GetCreateString();
			if(!rSql.empty()) adoDB.ExecuteQuery(rSql.c_str());
		}
		else
		{
			//! database의 필드명을 미리 저장해 둔다.
			int nFieldCount = adoDB.GetFieldCount();
			for(int i = 0;i < nFieldCount;++i) m_FieldNameEntry.push_back(adoDB.GetFieldName(i));
			
			nFieldCount = sqlFile.GetColumnCount();

			LONG lRecordCount = 0L;
			adoDB.GetRecordCount(&lRecordCount);
			for(long lRecord = 0;lRecord < lRecordCount;++lRecord)
			{
				CELOADTable::Record record;
				for(int i = 0;i < nFieldCount;++i)
				{
					const string rFieldName = sqlFile.GetFieldNameAt(i);
					vector<string>::iterator where = find(m_FieldNameEntry.begin() , m_FieldNameEntry.end() , rFieldName);

					STRING_T rValue;
					if(where != m_FieldNameEntry.end())
					{
						adoDB.GetFieldValue(lRecord , rFieldName.c_str() , &rValue);
						record.FieldValueMap.insert(make_pair(rFieldName.c_str() , rValue));
					}else	record.FieldValueMap.insert(make_pair(rFieldName.c_str() , _T("")));
				}
				
				if(record.FieldValueMap.end() == record.FieldValueMap.find(_T("C_STATE")))
				{
					record.FieldValueMap.insert(make_pair(_T("C_STATE"),_T("1")));	//! set C_STATE value to "1" to indicate it is database side.
				}

				m_RecordEntry.push_back(record);
			}

			SyncToSqlFile(adoDB , sqlFile);
		}

		return ERROR_SUCCESS;
	}
	else
	{
		AfxMessageBox(_T("Fail to connect project database."));
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author	BHK
*/
int TABLES::CELOADTable::Save(void)
{
	if(!m_rMDBFilePath.empty() && !m_rTableName.empty())
	{
		CADODB adoDB;
		const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + m_rMDBFilePath + DB_PASSWORD;
		if(TRUE == adoDB.DBConnect(rConnectionString))
		{
			//! 테이블의 내용을 모두 지운다.
			adoDB.ExecuteQuery(_T("DELETE * FROM ") + m_rTableName);

			for(vector<Record>::iterator itr = m_RecordEntry.begin();itr != m_RecordEntry.end();++itr)
			{
				CString rFields , rValues;
				for(map<string,string>::iterator jtr = itr->FieldValueMap.begin();jtr != itr->FieldValueMap.end();++jtr)
				{
					if("C_STATE" == jtr->first)
					{
						jtr->second = _T("1");
						continue;
					}
					//if(jtr->second.empty()) continue;

					rFields += string(jtr->first  + _T(",")).c_str();
					rValues += string("'" + jtr->second + _T("',")).c_str();
				}
				if("," == rFields.Right(1)) rFields = rFields.Left(rFields.GetLength() - 1);
				if("," == rValues.Right(1)) rValues = rValues.Left(rValues.GetLength() - 1);

				const CString rSql = "INSERT INTO " + CString(m_rTableName.c_str()) + "(" + rFields + ") VALUES(" + rValues + ")";
				adoDB.ExecuteQuery(rSql.operator LPCTSTR());
			}

			return ERROR_SUCCESS;
		}
		else
		{
			AfxMessageBox("FAIL TO CONNET DATABASE !");
			return ERROR_BAD_ENVIRONMENT;
		}
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	DATA BASE를 업데이트 한다.

	@author	KHS
*/
int TABLES::CELOADTable::Update(void)
{
	if(!m_rMDBFilePath.empty() && !m_rTableName.empty())
	{
		CADODB adoDB;
		const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + m_rMDBFilePath + DB_PASSWORD;
		if(TRUE == adoDB.DBConnect(rConnectionString))
		{
			for(vector<Record>::iterator itr = m_RecordEntry.begin();itr != m_RecordEntry.end();++itr)
			{
				map<string,string>::iterator where = itr->FieldValueMap.find(_T("C_VOLTAGE_TYPE"));
				if(where != itr->FieldValueMap.end())
				{
					CString rVoltageValue = where->second.c_str();

					for(map<string,string>::iterator jtr = itr->FieldValueMap.begin();jtr != itr->FieldValueMap.end();++jtr)
					{
						CString rField = jtr->first.c_str();
						CString rValue = jtr->second.c_str();

						CString rSql = "UPDATE "  + CString(m_rTableName.c_str()) + " SET [" + rField + "]='" + rValue +"' WHERE C_VOLTAGE_TYPE='" + rVoltageValue + "'";
						adoDB.ExecuteQuery(rSql.operator LPCTSTR());
					}
				}
				else	//! 레코드가 하나 일 때 (C_VOLTAGE_TYPE 필드가 없을때)
				{
					for(map<string,string>::iterator jtr = itr->FieldValueMap.begin();jtr != itr->FieldValueMap.end();++jtr)
					{
						if("C_STATE" == jtr->first)
						{
							jtr->second = _T("1");
							continue;
						}
						CString rField = jtr->first.c_str();
						CString rValue = jtr->second.c_str();

						CString rSql = "UPDATE "  + CString(m_rTableName.c_str()) + " SET [" + rField + "]='" + rValue +"'";
						adoDB.ExecuteQuery(rSql.operator LPCTSTR());
					}
				}
			}

			return ERROR_SUCCESS;
		}
		else
		{
			AfxMessageBox("FAIL TO CONNET DATABASE !");
			return ERROR_BAD_ENVIRONMENT;
		}
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author	BHK
*/
int TABLES::CELOADTable::GetValue(const int& nRecordNo , const string& rFieldName , string& rValue)
{
	assert((nRecordNo < (int)(m_RecordEntry.size())) && "range error");

	if(nRecordNo < (int)(m_RecordEntry.size()))
	{
		map<string,string>::iterator where = m_RecordEntry[nRecordNo].FieldValueMap.find(rFieldName);
		if(where != m_RecordEntry[nRecordNo].FieldValueMap.end())
		{
			rValue = (where->second);
			return ERROR_SUCCESS;
		}
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author	BHK
*/
int TABLES::CELOADTable::SetValue(const int& nRecordNo, const string& rFieldName, const string& rFieldValue)
{
	assert(nRecordNo < int(m_RecordEntry.size()) && "range error");

	if(nRecordNo < int(m_RecordEntry.size()))
	{
		map<string,string>::iterator where = m_RecordEntry[nRecordNo].FieldValueMap.find(rFieldName);
		if(where != m_RecordEntry[nRecordNo].FieldValueMap.end())
		{
			(where->second) = rFieldValue;
			return ERROR_SUCCESS;
		}
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author	BHK
*/
int TABLES::CELOADTable::GetRecordCount(void)
{
	return (int)(m_RecordEntry.size());
}

/**
	@brief	

	@author BHK	

	@date 2009-04-14 오후 3:01:33	

	@param	

	@return		
*/
int TABLES::CELOADTable::Insert(CELOADTable::Record record)
{
	m_RecordEntry.push_back(record);
	
	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-14 오후 2:56:49	

	@param	

	@return		
*/
int TABLES::CELOADTable::Clear(void)
{
	m_RecordEntry.clear();

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-24 오전 11:09:03	

	@param	

	@return		
*/
CELOADTable* TABLES::CELOADTable::Clone()
{
	CELOADTable* pTable = new CELOADTable();
	if(pTable)
	{
		pTable->m_rMDBFilePath = m_rMDBFilePath;
		pTable->m_rTableName   = m_rTableName;

		pTable->m_FieldNameEntry.insert(pTable->m_FieldNameEntry.begin() , m_FieldNameEntry.begin() , m_FieldNameEntry.end());
		pTable->m_RecordEntry.insert(pTable->m_RecordEntry.begin() , m_RecordEntry.begin() , m_RecordEntry.end());
	}

	return pTable;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-24 오전 11:13:02	

	@param	

	@return		
*/
CELOADTable* TABLES::CELOADTable::Copy(CELOADTable* pTable)
{
	assert(pTable && "pTable is NULL");

	if(pTable)
	{
		m_rMDBFilePath = pTable->m_rMDBFilePath;
		m_rTableName   = pTable->m_rTableName;

		m_FieldNameEntry.clear();
		m_FieldNameEntry.insert(m_FieldNameEntry.begin() , pTable->m_FieldNameEntry.begin() , pTable->m_FieldNameEntry.end());
		m_RecordEntry.clear();
		m_RecordEntry.insert(m_RecordEntry.begin() , pTable->m_RecordEntry.begin() , pTable->m_RecordEntry.end());
	}

	return pTable;
}