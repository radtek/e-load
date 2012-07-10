#include "StdAfx.h"
#include "CableCreationSettingTable.h"
#include "../ELoadDocData.h"
#include <util/ado/SQLFile.h>

using namespace TABLES;
CHVCableCreationSettingTable::CHVCableCreationSettingTable()
{
	m_rTableName = _T("T_HV_CABLE_CREATION_SETTING");
	m_bAlreadyLoaded = false;
}

CHVCableCreationSettingTable& CHVCableCreationSettingTable::GetInstance()
{
	static CHVCableCreationSettingTable __INSTANCE__;

	return __INSTANCE__;
}

CHVCableCreationSettingTable::~CHVCableCreationSettingTable()
{
}

/**
	@brief	

	@author	BHK
*/
int CHVCableCreationSettingTable::Load(const string& rMDBFilePath)
{
	if(false == m_bAlreadyLoaded)
	{
		if(ERROR_SUCCESS == CCableCreationTable::Load(rMDBFilePath , m_rTableName)) m_bAlreadyLoaded = true;
		
		//! 기본적으로 3개의 RECORD를 가지도록 한다.
		if(m_RecordEntry.empty() || (m_RecordEntry.size() < 3))
		{
			static const string __VOLT_TYPES__[3] = {_T("HV-1") , _T("HV-2") , _T("HV-3")};

			CString rExecPath = GetExecPath();
			if("\\" != rExecPath.Right(1)) rExecPath += "\\";
			CString rSqlFilePath = rExecPath + "Backup\\" + CString(m_rTableName.c_str()) + ".SQL";

			CSQLFile sqlFile(rSqlFilePath.operator LPCSTR());
			if(ERROR_SUCCESS != sqlFile.Parse()) return ERROR_BAD_ENVIRONMENT;
			const int nFieldCount = sqlFile.GetColumnCount();
			
			//! 생성된 레코드로 DB에 레코드를 생성한다.
			CADODB adoDB;
			const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + m_rMDBFilePath.c_str() + DB_PASSWORD;
			if(TRUE == adoDB.DBConnect(rConnectionString))
			{
				for(int i = m_RecordEntry.size();i < 3;++i)
				{
					CCableCreationTable::Record record;
					for(int j = 0;j < nFieldCount;++j)
					{
						const string rFieldName = sqlFile.GetFieldNameAt(j);
						record.FieldValueMap[rFieldName.c_str()] = _T("");

					}
					record.FieldValueMap[_T("C_VOLTAGE_TYPE")] = __VOLT_TYPES__[i];
					m_RecordEntry.push_back(record);

					STRING_T rValues, rFields;
					for(map<STRING_T,STRING_T>::iterator itr = record.FieldValueMap.begin();itr != record.FieldValueMap.end();++itr)
					{
						STRING_T rField = itr->first;
						STRING_T rValue = itr->second;

						rFields += _T("[") + rField + _T("],");
						rValues += _T("'") + rValue + _T("',");
					}

					if( _T(",") == rFields.substr(rFields.length() - 1 , 1)) rFields = rFields.substr(0 , rFields.length() - 1);
					if( _T(",") == rValues.substr(rValues.length() - 1 , 1)) rValues = rValues.substr(0 , rValues.length() - 1);
					STRING_T rSql = _T("INSERT INTO ")  + m_rTableName + _T("(") + rFields + _T(") VALUES (") + rValues + _T(")");
					adoDB.ExecuteQuery(rSql);
				}
			}
			else
			{
				AfxMessageBox(_T("Can't Connet DataBase..."));
				return ERROR_BAD_ENVIRONMENT;
			}
		}
	}

	return ERROR_SUCCESS;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
CLV3PHCableCreationSettingTable::CLV3PHCableCreationSettingTable()
{
	m_rTableName = _T("T_LV_3PH_CABLE_CREATION_SETTING");
	m_bAlreadyLoaded = false;
}

CLV3PHCableCreationSettingTable& CLV3PHCableCreationSettingTable::GetInstance()
{
	static CLV3PHCableCreationSettingTable __INSTANCE__;

	return __INSTANCE__;
}

CLV3PHCableCreationSettingTable::~CLV3PHCableCreationSettingTable()
{
}

/**
	@brief	

	@author	BHK
*/
int CLV3PHCableCreationSettingTable::Load(const string& rMDBFilePath)
{
	if(false == m_bAlreadyLoaded)
	{
		if(ERROR_SUCCESS == CCableCreationTable::Load(rMDBFilePath,m_rTableName)) m_bAlreadyLoaded = true;
		//! 기본적으로 2개의 RECORD를 가지도록 한다.
		if(m_RecordEntry.empty() || (m_RecordEntry.size() < 2))
		{
			static const string __VOLT_TYPES__[2] = {_T("LV-1 (3PH)") , _T("LV-2 (3PH)")};

			CString rExecPath = GetExecPath();
			if("\\" != rExecPath.Right(1)) rExecPath += "\\";
			CString rSqlFilePath = rExecPath + "Backup\\" + CString(m_rTableName.c_str()) + ".SQL";

			CSQLFile sqlFile(rSqlFilePath.operator LPCSTR());
			if(ERROR_SUCCESS != sqlFile.Parse()) return ERROR_BAD_ENVIRONMENT;
			const int nFieldCount = sqlFile.GetColumnCount();
			
			//! 생성된 레코드로 DB에 레코드를 생성한다.
			CADODB adoDB;
			const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + m_rMDBFilePath.c_str()  + DB_PASSWORD;
			if(TRUE == adoDB.DBConnect(rConnectionString))
			{
				for(int i = m_RecordEntry.size();i < 2;++i)
				{
					CCableCreationTable::Record record;
					for(int j = 0;j < nFieldCount;++j)
					{
						const string rFieldName = sqlFile.GetFieldNameAt(j);
						record.FieldValueMap[rFieldName.c_str()] = _T("");

					}
					record.FieldValueMap[_T("C_VOLTAGE_TYPE")] = __VOLT_TYPES__[i];
					m_RecordEntry.push_back(record);

					STRING_T rValues, rFields;
					for(map<string,string>::iterator itr = record.FieldValueMap.begin();itr != record.FieldValueMap.end();++itr)
					{
						CString rField = itr->first.c_str();
						CString rValue = itr->second.c_str();

						rFields += _T("[") + rField + _T("],");
						rValues += _T("'") + rValue + _T("',");
					}

					if( _T(",") == rFields.substr(rFields.length() - 1, 1)) rFields = rFields.substr(0 , rFields.length() - 1);
					if( _T(",") == rValues.substr(rValues.length() - 1, 1)) rValues = rValues.substr(0 , rValues.length() - 1);
					STRING_T rSql = _T("INSERT INTO ")  + m_rTableName + _T("(") + rFields + _T(") VALUES (") + rValues + _T(")");
					adoDB.ExecuteQuery(rSql);
				}
			}
			else
			{
				AfxMessageBox("Not Connet DB...");
				return ERROR_BAD_ENVIRONMENT;
			}
		}
	}

	return ERROR_SUCCESS;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
CLV1PHCableCreationSettingTable::CLV1PHCableCreationSettingTable()
{
	m_rTableName = _T("T_LV_1PH_CABLE_CREATION_SETTING");
	m_bAlreadyLoaded = false;
}

CLV1PHCableCreationSettingTable& CLV1PHCableCreationSettingTable::GetInstance()
{
	static CLV1PHCableCreationSettingTable __INSTANCE__;

	return __INSTANCE__;
}

CLV1PHCableCreationSettingTable::~CLV1PHCableCreationSettingTable()
{
}

/**
	@brief	

	@author	BHK
*/
int CLV1PHCableCreationSettingTable::Load(const string& rMDBFilePath)
{
	if(false == m_bAlreadyLoaded)
	{
		if(ERROR_SUCCESS == CCableCreationTable::Load(rMDBFilePath,m_rTableName)) m_bAlreadyLoaded = true;
		//! 기본적으로 2개의 RECORD를 가지도록 한다.
		if(m_RecordEntry.empty() || (m_RecordEntry.size() < 2))
		{
			static const string __VOLT_TYPES__[2] = {_T("LV-11 (1PH)") , _T("LV-12 (1PH)")};

			CString rExecPath = GetExecPath();
			if("\\" != rExecPath.Right(1)) rExecPath += "\\";
			CString rSqlFilePath = rExecPath + "Backup\\" + CString(m_rTableName.c_str()) + ".SQL";

			CSQLFile sqlFile(rSqlFilePath.operator LPCSTR());
			if(ERROR_SUCCESS != sqlFile.Parse()) return ERROR_BAD_ENVIRONMENT;
			const int nFieldCount = sqlFile.GetColumnCount();
			
			//! 생성된레코드로 DB에 레코드를 생성한다.
			CADODB adoDB;
			const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + m_rMDBFilePath.c_str()  + DB_PASSWORD;
			if(TRUE == adoDB.DBConnect(rConnectionString))
			{
				for(int i = m_RecordEntry.size();i < 2;++i)
				{
					CCableCreationTable::Record record;
					for(int j = 0;j < nFieldCount;++j)
					{
						const string rFieldName = sqlFile.GetFieldNameAt(j);
						record.FieldValueMap[rFieldName.c_str()] = _T("");
					}
					record.FieldValueMap[_T("C_VOLTAGE_TYPE")] = __VOLT_TYPES__[i];
					m_RecordEntry.push_back(record);

					STRING_T rValues, rFields;
					for(map<string,string>::iterator itr = record.FieldValueMap.begin();itr != record.FieldValueMap.end();++itr)
					{
						CString rField = itr->first.c_str();
						CString rValue = itr->second.c_str();

						rFields += _T("[") + rField + _T("],");
						rValues += _T("'") + rValue + _T("',");
					}

					if( _T(",") == rFields.substr(rFields.length() - 1, 1)) rFields = rFields.substr(0 , rFields.length() - 1);
					if( _T(",") == rValues.substr(rValues.length() - 1, 1)) rValues = rValues.substr(0 , rValues.length() - 1);
					STRING_T rSql = _T("INSERT INTO ")  + m_rTableName + _T("(") + rFields + _T(") VALUES (") + rValues + _T(")");
					adoDB.ExecuteQuery(rSql);
				}
			}
			else
			{
				AfxMessageBox("Not Connet DB...");
				return ERROR_BAD_ENVIRONMENT;
			}
		}
	}

	return ERROR_SUCCESS;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
CDCCableCreationSettingTable::CDCCableCreationSettingTable()
{
	m_rTableName = _T("T_DC_CABLE_CREATION_SETTING");
	m_bAlreadyLoaded = false;
}

CDCCableCreationSettingTable& CDCCableCreationSettingTable::GetInstance()
{
	static CDCCableCreationSettingTable __INSTANCE__;

	return __INSTANCE__;
}

CDCCableCreationSettingTable::~CDCCableCreationSettingTable()
{
}

/**
	@brief	

	@author	BHK
*/
int CDCCableCreationSettingTable::Load(const string& rMDBFilePath)
{
	if(false == m_bAlreadyLoaded)
	{
		if(ERROR_SUCCESS == CCableCreationTable::Load(rMDBFilePath,m_rTableName)) m_bAlreadyLoaded = true;
		//! 기본적으로 하나의 RECORD를 가지도록 한다.
		if(m_RecordEntry.empty())
		{
			static const string __VOLT_TYPES__[3] = {_T("DC")};

			CString rExecPath = GetExecPath();
			if("\\" != rExecPath.Right(1)) rExecPath += "\\";
			CString rSqlFilePath = rExecPath + "Backup\\" + CString(m_rTableName.c_str()) + ".SQL";

			CSQLFile sqlFile(rSqlFilePath.operator LPCSTR());
			if(ERROR_SUCCESS != sqlFile.Parse()) return ERROR_BAD_ENVIRONMENT;

			const int nFieldCount = sqlFile.GetColumnCount();

			CCableCreationTable::Record record;
			for(int i = 0;i < nFieldCount;++i)
			{
				const string rFieldName = sqlFile.GetFieldNameAt(i);
				record.FieldValueMap[rFieldName.c_str()] = _T("");
			}
			record.FieldValueMap[_T("C_VOLTAGE_TYPE")] = __VOLT_TYPES__[0];
			m_RecordEntry.push_back(record);
			
			//! 생성된 하나의 레코드로 DB에 레코드를 생성한다.
			CADODB adoDB;
			const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + m_rMDBFilePath.c_str()  + DB_PASSWORD;
			if(TRUE == adoDB.DBConnect(rConnectionString))
			{
				STRING_T rValues, rFields;
				for(map<string,string>::iterator itr = m_RecordEntry.front().FieldValueMap.begin();itr != m_RecordEntry.front().FieldValueMap.end();++itr)
				{
					CString rField = itr->first.c_str();
					CString rValue = itr->second.c_str();

					rFields += _T("[") + rField + _T("],");
					rValues += _T("'") + rValue + _T("',");
				}

				if( _T(",") == rFields.substr(rFields.length() - 1 , 1)) rFields = rFields.substr(0 , rFields.length() - 1);
				if( _T(",") == rValues.substr(rValues.length() - 1 , 1)) rValues = rValues.substr(0 , rValues.length() - 1);
				STRING_T rSql = _T("INSERT INTO ")  + m_rTableName + _T("(") + rFields + _T(") VALUES (") + rValues + _T(")");
				adoDB.ExecuteQuery(rSql);
			}
			else
			{
				AfxMessageBox("Not Connet DB...");
				return ERROR_BAD_ENVIRONMENT;
			}
		}
	}

	return ERROR_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
CPowerCableCreationSettingTable::CPowerCableCreationSettingTable()
{
	m_rTableName = CPowerCableCreationSettingTable::GetTableName();
	m_bAlreadyLoaded = false;
}

/*
CPowerCableCreationSettingTable& CPowerCableCreationSettingTable::GetInstance()
{
	static CPowerCableCreationSettingTable __INSTANCE__;

	return __INSTANCE__;
}
*/

CPowerCableCreationSettingTable::~CPowerCableCreationSettingTable()
{
}

/**
	@brief	

	@author	KHS
*/
int CPowerCableCreationSettingTable::Load(const string& rMDBFilePath,const string& rTableName)
{
	if(false == m_bAlreadyLoaded)
	{
		if(ERROR_SUCCESS == CCableCreationTable::Load(rMDBFilePath,rTableName)) m_bAlreadyLoaded = true;
		
		//! 기본적으로 하나의 RECORD를 가지도록 한다.
		if(m_RecordEntry.empty())
		{
			CString rExecPath = GetExecPath();
			if("\\" != rExecPath.Right(1)) rExecPath += "\\";
			CString rSqlFilePath = rExecPath + "Backup\\" + CString(rTableName.c_str()) + ".SQL";

			CSQLFile sqlFile(rSqlFilePath.operator LPCSTR());
			if(ERROR_SUCCESS != sqlFile.Parse()) return ERROR_BAD_ENVIRONMENT;

			const int nFieldCount = sqlFile.GetColumnCount();

			CCableCreationTable::Record record;
			for(int i = 0;i < nFieldCount;++i)
			{
				const string rFieldName = sqlFile.GetFieldNameAt(i);
				record.FieldValueMap[rFieldName.c_str()] = _T("");
				
			}
			m_RecordEntry.push_back(record);
			
			//! 생성된 하나의 레코드로 DB에 레코드를 생성한다.
			CADODB adoDB;
			const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + m_rMDBFilePath.c_str() + DB_PASSWORD;
			if(TRUE == adoDB.DBConnect(rConnectionString))
			{
				CString rValues, rFields;
				for(map<string,string>::iterator itr = m_RecordEntry.front().FieldValueMap.begin();itr != m_RecordEntry.front().FieldValueMap.end();++itr)
				{
					if(_T("C_STATE") == itr->first) continue;
					CString rField = itr->first.c_str();
					CString rValue = itr->second.c_str();

					rFields += "[" + rField + "],";
					rValues += "'" + rValue + "',";
					break;
				}

				if( _T(",") == rFields.Right(1)) rFields = rFields.Left(rFields.GetLength()-1);
				if( _T(",") == rValues.Right(1)) rValues = rValues.Left(rValues.GetLength() - 1);
				const STRING_T rSql = _T("INSERT INTO ")  + m_rTableName + _T("(") + STRING_T(rFields.operator LPCTSTR()) + _T(") VALUES (") + 
					STRING_T(rValues.operator LPCTSTR()) + _T(")");
				adoDB.ExecuteQuery(rSql);
			}
			else
			{
				AfxMessageBox("Not Connet DB...");
				return ERROR_BAD_ENVIRONMENT;
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-20 오후 12:47:29	

	@param	

	@return		
*/
const string CPowerCableCreationSettingTable::GetTableName()
{
	static const string __TABLE_NAME__ = _T("T_POWER_CABLE_CREATION_SETTING");

	return __TABLE_NAME__;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
CCableNamingRuleSettingTable::CCableNamingRuleSettingTable()
{
	m_rTableName = _T("T_CABLE_NAMING_RULE_SETTING");
	m_bAlreadyLoaded = false;
}

CCableNamingRuleSettingTable& CCableNamingRuleSettingTable::GetInstance()
{
	static CCableNamingRuleSettingTable __INSTANCE__;

	return __INSTANCE__;
}

CCableNamingRuleSettingTable::~CCableNamingRuleSettingTable()
{
}

/**
	@brief	

	@author	KHS
*/
int CCableNamingRuleSettingTable::Load(const string& rMDBFilePath)
{
	if(false == m_bAlreadyLoaded)
	{
		if(ERROR_SUCCESS == CCableCreationTable::Load(rMDBFilePath,m_rTableName)) m_bAlreadyLoaded = true;
		
		//! 기본적으로 하나의 RECORD를 가지도록 한다.
		if(m_RecordEntry.empty())
		{
			CString rExecPath = GetExecPath();
			if("\\" != rExecPath.Right(1)) rExecPath += "\\";
			CString rSqlFilePath = rExecPath + "Backup\\" + CString(m_rTableName.c_str()) + ".SQL";

			CSQLFile sqlFile(rSqlFilePath.operator LPCSTR());
			if(ERROR_SUCCESS != sqlFile.Parse()) return ERROR_BAD_ENVIRONMENT;

			const int nFieldCount = sqlFile.GetColumnCount();

			CCableCreationTable::Record record;
			for(int i = 0;i < nFieldCount;++i)
			{
				const string rFieldName = sqlFile.GetFieldNameAt(i);
				
				record.FieldValueMap[rFieldName.c_str()] = _T("");
				
			}

			m_RecordEntry.push_back(record);
			
			//! 생성된 하나의 레코드로 DB에 레코드를 생성한다.
			CADODB adoDB;
			const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + m_rMDBFilePath + DB_PASSWORD;
			if(TRUE == adoDB.DBConnect(rConnectionString))
			{
				CString rValues, rFields;
				for(map<string,string>::iterator itr = m_RecordEntry.front().FieldValueMap.begin();itr != m_RecordEntry.front().FieldValueMap.end();++itr)
				{
					CString rField = itr->first.c_str();
					CString rValue = itr->second.c_str();

					rFields += "[" + rField + "],";
					rValues += "'" + rValue + "',";
				}

				if( _T(",") == rFields.Right(1)) rFields = rFields.Left(rFields.GetLength() - 1);
				if( _T(",") == rValues.Right(1)) rValues = rValues.Left(rValues.GetLength() - 1);
				STRING_T rSql = _T("INSERT INTO ")  + m_rTableName + _T("(") + STRING_T(rFields.operator LPCTSTR()) + _T(") VALUES (") + 
					STRING_T(rValues.operator LPCTSTR()) + _T(")");
				adoDB.ExecuteQuery(rSql);
			}
			else
			{
				AfxMessageBox("Not Connet DB...");
				return ERROR_BAD_ENVIRONMENT;
			}
		}
	}

	return ERROR_SUCCESS;
}