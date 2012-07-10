
#include "StdAfx.h"
#include <assert.h>
#include <util/ado/SQLFile.h>
#include "CableCreationTable.h"
#include <vector>
#include <map>
#include <util/ado/ADODB.h>

using namespace TABLES;

CCableCreationTable::CCableCreationTable(void) : CELOADTable()
{
}

CCableCreationTable::~CCableCreationTable(void)
{
	try
	{
	}
	catch(...)
	{
	}
}

/**
	@brief	DATA BASE를 업데이트 한다.

	@author	KHS
*/
int TABLES::CCableCreationTable::Update(void)
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

					for(map<STRING_T,STRING_T>::iterator jtr = itr->FieldValueMap.begin();jtr != itr->FieldValueMap.end();++jtr)
					{
						STRING_T rField = jtr->first;
						STRING_T rValue = jtr->second;

						if( _T("C_STATE") == rField) continue;

						STRING_T rSql = _T("UPDATE ")  + m_rTableName + _T(" SET [") + rField + _T("]='") + rValue + _T("' WHERE C_VOLTAGE_TYPE='") + STRING_T(rVoltageValue.operator LPCTSTR()) + _T("'");
						adoDB.ExecuteQuery(rSql);
					}
				}
				else	//! 레코드가 하나 일 때 (C_VOLTAGE_TYPE 필드가 없을때)
				{
					for(map<string,string>::iterator jtr = itr->FieldValueMap.begin();jtr != itr->FieldValueMap.end();++jtr)
					{
						STRING_T rField = jtr->first;
						STRING_T rValue = jtr->second;
						
						if( _T("C_STATE") == rField) continue;

						STRING_T rSql = _T("UPDATE ")  + m_rTableName + _T(" SET [") + rField + _T("]='") + rValue + _T("'");
						adoDB.ExecuteQuery(rSql);
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
	@brief	SAVE

	@author	BHK
*/
int TABLES::CCableCreationTable::Save()
{
	if(!m_rMDBFilePath.empty() && !m_rTableName.empty())
	{
		CADODB adoDB;
		const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + m_rMDBFilePath + DB_PASSWORD;
		if(TRUE == adoDB.DBConnect(rConnectionString))
		{
			//! 테이블의 내용을 모두 지운다.
			adoDB.ExecuteQuery(STRING_T(_T("DELETE * FROM ")) + m_rTableName);

			for(vector<Record>::iterator itr = m_RecordEntry.begin();itr != m_RecordEntry.end();++itr)
			{
				CString rFields , rValues;
				for(map<string,string>::iterator jtr = itr->FieldValueMap.begin();jtr != itr->FieldValueMap.end();++jtr)
				{
					if(_T("C_STATE") == jtr->first) continue;

					rFields += string(jtr->first  + _T(",")).c_str();
					rValues += string("'" + jtr->second + _T("',")).c_str();
				}
				if("," == rFields.Right(1)) rFields = rFields.Left(rFields.GetLength() - 1);
				if("," == rValues.Right(1)) rValues = rValues.Left(rValues.GetLength() - 1);

				const STRING_T rSql = _T("INSERT INTO ") + m_rTableName + _T("(") + STRING_T(rFields.operator LPCTSTR()) + _T(") VALUES(") + 
					STRING_T(rValues.operator LPCTSTR()) + _T(")");
				adoDB.ExecuteQuery(rSql);
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