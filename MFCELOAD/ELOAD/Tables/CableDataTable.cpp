#include "StdAfx.h"
#include "../ELOAD.h"
#include "../ELoadDocData.h"
#include <util/ado/ADODB.h>
#include "CableDataTable.h"
#include <util/ado/SQLFile.h>

using namespace TABLES;

CCableDataByType::~CCableDataByType()
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
int CCableDataByType::Clear()
{
	for(map<int , vector<Record*>* >::iterator itr = m_RecordMap.begin();itr != m_RecordMap.end();++itr)
	{
		for(vector<Record*>::iterator jtr = itr->second->begin();jtr != itr->second->end();++jtr)
		{
			SAFE_DELETE(*jtr);
		}
		itr->second->clear();

		SAFE_DELETE(itr->second);
	}

	m_RecordMap.clear();

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
int CCableDataByType::Load(CADODB& adoDB, const int& nRecordNo , vector<STRING_T>& FieldNameEntry , CSQLFile& sqlFile)
{
	Record* pRecord = new Record;
	if(pRecord)
	{
		const int nFieldCount = sqlFile.GetColumnCount();
		for(int i = 0;i < nFieldCount;++i)
		{
			STRING_T rValue;
			const STRING_T rFieldName = sqlFile.GetFieldNameAt(i);
			vector<STRING_T>::iterator where = find(FieldNameEntry.begin() , FieldNameEntry.end() , rFieldName);
			if(where != FieldNameEntry.end()) adoDB.GetFieldValue(nRecordNo , rFieldName.c_str() , &rValue);
			pRecord->m_FieldMap[rFieldName] = rValue;
		}

		map<string , string>::iterator where = pRecord->m_FieldMap.find(_T("C_HERTZ"));
		if(where != pRecord->m_FieldMap.end())
		{
			const int nHertz = atoi(where->second.c_str());
			map<int , vector<Record*>* >::iterator at = m_RecordMap.find(nHertz);
			if(at == m_RecordMap.end())
			{
				m_RecordMap[nHertz] = new vector<Record*>;
			}
			m_RecordMap[nHertz]->push_back(pRecord);
		}
		else
		{
			ELOAD_LOG4CXX_WARN(mylogger, "" , 4);
			delete pRecord;
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

int CCableDataByType::Save(CADODB& adoDB ,  const string& rTableName)
{
	for(map<int , vector<Record*>* >::iterator itr = m_RecordMap.begin();itr != m_RecordMap.end();++itr)
	{
		for(vector<Record*>::iterator jtr = itr->second->begin();jtr != itr->second->end();++jtr)
		{
			string rFields , rValues;
			for(map<string,string>::iterator ktr = (*jtr)->m_FieldMap.begin();ktr != (*jtr)->m_FieldMap.end();++ktr)
			{
				rFields += "[" + ktr->first + "],";
				rValues += "'" + ktr->second + "',";
			}
			if(',' == rFields[rFields.length() - 1]) rFields = rFields.substr(0 , rFields.length() - 1);
			if(',' == rValues[rValues.length() - 1]) rValues = rValues.substr(0 , rValues.length() - 1);

			string rSql = "INSERT INTO [" + rTableName + "](" + rFields + ") VALUES(";
			rSql += rValues +  ")";

			adoDB.ExecuteQuery(rSql.c_str());
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	hertz에 해당하는 레코드들을 리턴한다.

	@param	nHertz 헤르쯔 값(만일 -1일 경우에는 PROJECT HERTZ값에 해당하는 레코드들을 리턴한다.)

	@author	BHK
*/
vector<CCableDataByType::Record*>* CCableDataByType::GetRecordEntryOfHertz(const int& nHertz)
{
	int hertz = nHertz;
	if(-1 == hertz)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
		string* pValue = pSettingTable->GetFieldValue(_T("C_HERTZ"));
		//! default hertz is 50
		hertz = ((NULL != pValue) && !pValue->empty()) ? atoi(pValue->c_str()) : 50;
	}

	map<int , vector<Record*>* >::iterator where = m_RecordMap.find(hertz);
	if(where != m_RecordMap.end()) return (where->second);

	return NULL;
}

/**
	@brief	

	@author	BHK
*/
int CCableDataByType::Insert(CCableDataByType::Record* pRecord)
{
	assert(pRecord && "pRecord is NULL");

	if(pRecord)
	{
		const string rHertz = pRecord->m_FieldMap[_T("C_HERTZ")];
		const int nHertz = atoi(rHertz.c_str());
		map<int , vector<Record*>* >::iterator where = m_RecordMap.find(nHertz);
		if(where == m_RecordMap.end())
		{
			m_RecordMap[nHertz] = new vector<Record*>;
		}
		m_RecordMap[nHertz]->push_back(pRecord);

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	

	@author	BHK
*/
int CCableDataByType::GetHertzList(vector<int>& HertzEntry)
{
	HertzEntry.clear();
	for(map<int , vector<Record*>* >::iterator itr = m_RecordMap.begin();itr != m_RecordMap.end();++itr)
	{
		HertzEntry.push_back(itr->first);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	Cable Type을 구한다.

	@author	BHK
*/
string CCableDataByType::Record::GetCableType()
{
	string rVolt = m_FieldMap[_T("C_VOLT")];
	string rCore = m_FieldMap[_T("C_CORE")];
	string rConductor = m_FieldMap[_T("C_CONDUCTOR")];
	string rCableType = m_FieldMap[_T("C_CABLE_TYPE")];
	string rMPR = m_FieldMap[_T("C_MPR")];

	return (rVolt + "KV " + rCore + " " + rConductor + " " + rCableType + " (" + rMPR + ")");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
CCableDataTable::CCableDataTable(void) : m_bLoaded(false)
{
}

CCableDataTable::~CCableDataTable(void)
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
int CCableDataTable::Insert(CCableDataByType::Record* pRecord)
{
	assert(pRecord && "pRecord is NULL");

	if(pRecord)
	{
		const string rCableType = pRecord->GetCableType();

		map<string , CCableDataByType* >::iterator where = m_CableDataByTypeMap.find(rCableType);
		if(where != m_CableDataByTypeMap.end())
		{
			where->second->Insert(pRecord);
		}
		else
		{
			CCableDataByType* pCableDataByType = new CCableDataByType;
			if(pCableDataByType)
			{
				pCableDataByType->Insert(pRecord);
				m_CableDataByTypeMap[rCableType] = pCableDataByType;
			}
		}

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	

	@author	BHK
*/
int CCableDataTable::Clear(void)
{
	for(map<string , CCableDataByType* >::iterator itr = m_CableDataByTypeMap.begin();itr != m_CableDataByTypeMap.end();++itr)
	{
		SAFE_DELETE(itr->second);
	}
	m_CableDataByTypeMap.clear();

	return ERROR_SUCCESS;
}


/**
	@brief	

	@author	BHK
*/
int CCableDataTable::GetCableTypeKeyList(vector<string>& CableTypeKeyList)
{
	CableTypeKeyList.clear();
	for(map<string , CCableDataByType* >::iterator itr = m_CableDataByTypeMap.begin();itr != m_CableDataByTypeMap.end();++itr)
	{
		CableTypeKeyList.push_back(itr->first);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	KHS
*/
int CCableDataTable::GetCableTypeKeyListOfHertz(vector<string>& CableTypeKeyList, const int& nHertz)
{
	CableTypeKeyList.clear();
	for(map<string , CCableDataByType* >::iterator itr = m_CableDataByTypeMap.begin();itr != m_CableDataByTypeMap.end();++itr)
	{
		vector<CCableDataByType::Record*>* pRecord = itr->second->GetRecordEntryOfHertz(nHertz);
		if(pRecord)
		{
			CableTypeKeyList.push_back(itr->first);
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	hertz , volt , cable type을 가지고 core를 구한다.

	@author BHK	

	@date 2009-04-15 오전 11:28:44	

	@param	

	@return		
*/
int CCableDataTable::GetCableCoreList(const int nHertz , const string& rVolt , const string& rType , vector<string>& CableCoreList)
{
	CableCoreList.clear();
	for(map<string , CCableDataByType* >::iterator itr = m_CableDataByTypeMap.begin();itr != m_CableDataByTypeMap.end();++itr)
	{
		vector<CCableDataByType::Record*>* pRecord = itr->second->GetRecordEntryOfHertz(nHertz);
		if(pRecord)
		{
			for(vector<CCableDataByType::Record*>::iterator jtr = pRecord->begin();jtr != pRecord->end();++jtr)
			{
				if((rVolt == (*jtr)->m_FieldMap[_T("C_VOLT")]) && (rType == (*jtr)->m_FieldMap[_T("C_CABLE_TYPE")]))
				{
					const string rCore = (*jtr)->m_FieldMap[_T("C_CORE")];
					vector<string>::iterator where = find(CableCoreList.begin() , CableCoreList.end() , rCore);
					if(where == CableCoreList.end()) CableCoreList.push_back(rCore);
				}
			}
		}
	}
	///::stable_sort(CableCoreList.begin() , CableCoreList.end() , less<string>());

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
CCableDataByType* CCableDataTable::GetRecordListOf(const string& rId)
{
	map<string , CCableDataByType* >::iterator where = m_CableDataByTypeMap.find(rId);
	if(where != m_CableDataByTypeMap.end()) return where->second;
	
	return NULL;
}

/**
	@brief	ITEM 테이블의 내용을 ITEM PROP과 일치시킨다.

	@param	rTableName 재구성할 테이블 이름.

	@author	BHK
*/
int CCableDataTable::SyncToSQLFile(CADODB& adoDB , const string& rTableName , const vector<STRING_T>& WillBeDeletedFieldNameEntry , const vector<STRING_T>& ShouldBeAddedFieldNameEntry)
{
	//! 필드 추가 부분
	for(vector<STRING_T>::const_iterator itr  = ShouldBeAddedFieldNameEntry.begin();itr != ShouldBeAddedFieldNameEntry.end();++itr)
	{
		adoDB.ExecuteQuery(_T("ALTER TABLE [") + rTableName + STRING_T(_T("] ADD ")) + (*itr));
	}
        
	//! 필드 삭제 부분
	for(vector<STRING_T>::const_iterator itr  = WillBeDeletedFieldNameEntry.begin();itr != WillBeDeletedFieldNameEntry.end();++itr)
	{
		//! DO DOT DELETE INDEX COLUMN.
		if(_T("C_IDX") == (*itr)) continue;

		adoDB.ExecuteQuery(_T("ALTER TABLE [") + rTableName + _T("] DROP [") + (*itr) + STRING_T(_T("]")));
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	HumKyung

	@date	????.??.??

	@param	rTableName

	@return	int
*/
int CCableDataTable::Save(const string& rTableName)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	string rCommonLibraryMDBFilePath = docData.GetCommonLibraryMDBFilePath();

	CADODB adoDB;
	const STRING_T rDBPath = STRING_T(PROVIDER) + _T("Data Source=") + rCommonLibraryMDBFilePath + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rDBPath))
	{
		string rSql("SELECT * FROM [" + rTableName + "]");
		if(FALSE == adoDB.OpenQuery(rSql.c_str()))
		{
			//! create table if not exist
			CString rExecPath = GetExecPath();
			if(_T("\\") != rExecPath.Right(1)) rExecPath += _T("\\");
			CString rSqlFilePath = rExecPath + _T("Backup\\T_CABLE_DATA.SQL");

			CSQLFile sqlFile(rSqlFilePath.operator LPCSTR());
			sqlFile.Parse();
			CString rSql = sqlFile.GetCreateString().c_str();
			
			if(!rSql.IsEmpty())
			{
				rSql.Replace("?" , CString(string("[" + rTableName + "]").c_str()));
				adoDB.ExecuteQuery(rSql.operator LPCTSTR());
			}
		}
		
		//! 테이블의 모든 내용을 삭제한다.
		adoDB.ExecuteQuery(string("DELETE * FROM [" + rTableName + "]").c_str());
		
		for(map<string , CCableDataByType* >::iterator itr = m_CableDataByTypeMap.begin();itr != m_CableDataByTypeMap.end();++itr)
		{
			itr->second->Save(adoDB , rTableName);
		}

		m_rTableName = rTableName;
	}        

	return ERROR_SUCCESS;
}

/**
	@brief	Cable Data Table을 로딩합니다.

	@param	bForce 값이 true이면 이미 로딩되었더라도 다시 로딩하게 합니다.

	@author	BHK
*/
int CCableDataTable::Load(const string& rTableName , const bool& bForce)
{
	if((false == m_bLoaded) || (true == bForce))
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		const string rCommonLibraryMDBFilePath = docData.GetCommonLibraryMDBFilePath();

		CADODB adoDB;
		const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rCommonLibraryMDBFilePath + DB_PASSWORD;
		if(TRUE == adoDB.DBConnect(rConnectionString))
		{
			CString rExecPath = GetExecPath();
			if("\\" != rExecPath.Right(1)) rExecPath += "\\";
			CString rSqlFilePath = rExecPath + _T("Backup\\T_CABLE_DATA.SQL");

			CSQLFile sqlFile(rSqlFilePath.operator LPCSTR());
			if(ERROR_SUCCESS != sqlFile.Parse()) return ERROR_BAD_ENVIRONMENT;

			Clear();	//! 기존의 데이터를 삭제한다.

			string rSql("SELECT * FROM [" + rTableName + "]");
			if(FALSE == adoDB.OpenQuery(rSql.c_str()))
			{
				//! 테이블이 없으므로 테이블을 생성한다.	
				CString rSql = CString(sqlFile.GetCreateString().c_str());
				if(!rSql.IsEmpty())
				{
					rSql.Replace("?" , CString(string("[" + rTableName + "]").c_str()));
					adoDB.ExecuteQuery(rSql.operator LPCTSTR());
				}
			}
			else
			{
				//! database의 필드명을 미리 저장해 둔다.
				vector<STRING_T> FieldNameEntry , ShouldBeAddedFieldNameEntry;
				int nFieldCount = adoDB.GetFieldCount();
				for(int i = 0;i < nFieldCount;++i) FieldNameEntry.push_back(adoDB.GetFieldName(i));

				LONG lRecordCount = 0L;
				adoDB.GetRecordCount(&lRecordCount);
				for(int i = 0;i < lRecordCount;++i)
				{
					STRING_T rVolt , rCore , rConductor , rCableType , rMPR;
					adoDB.GetFieldValue(i , _T("C_VOLT") , &rVolt);
					adoDB.GetFieldValue(i , _T("C_CORE") , &rCore);
					adoDB.GetFieldValue(i , _T("C_CONDUCTOR") , &rConductor);
					adoDB.GetFieldValue(i , _T("C_CABLE_TYPE") , &rCableType);
					adoDB.GetFieldValue(i , _T("C_MPR") , &rMPR);

					const STRING_T rType = CCableDataTable::CreateCableId(rVolt , rCore , rConductor , rCableType , rMPR);
					map<STRING_T , CCableDataByType* >::iterator where = m_CableDataByTypeMap.find(rType);
					if(where == m_CableDataByTypeMap.end())
					{
						m_CableDataByTypeMap[rType] = new CCableDataByType;
					}
					m_CableDataByTypeMap[rType]->Load(adoDB , i , FieldNameEntry , sqlFile);
				}

				m_rTableName = rTableName;
				m_bLoaded = true;

				{
					nFieldCount = sqlFile.GetColumnCount();
					for(int i = 0;i < nFieldCount;++i)
					{
						//! 테이블의 필드명과 SQL FILE에서의 필드명을 확인하는 루틴.
						const STRING_T rFieldName = sqlFile.GetFieldNameAt(i);
						STRING_T rString(rFieldName);
						IsString::ToUpper(rString);
						vector<STRING_T>::iterator where = find(FieldNameEntry.begin(),FieldNameEntry.end() , rString);
						if(where != FieldNameEntry.end())
						{
							FieldNameEntry.erase(where);
						}else	ShouldBeAddedFieldNameEntry.push_back(sqlFile.GetFieldDataAt(i).c_str());
						//!
					}

					//! 테이블을 SQL FILE에 있는 내용대로 맞춘다.
					SyncToSQLFile(adoDB , rTableName , FieldNameEntry , ShouldBeAddedFieldNameEntry);
				}

				return ERROR_SUCCESS;
			}
		} 
	}
	else if(true == m_bLoaded) return ERROR_SUCCESS;

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	Cable Id를 생성한다..

	@author	BHK
*/
string CCableDataTable::CreateCableId(string rVolt , string rCore , string rConductor , string rCableType , string rMPR)
{
	const string rId = rVolt + _T("KV ") + rCore + _T(" ") + rConductor + _T(" ") + rCableType + _T(" (") + rMPR + _T(")");

	return rId;
}