#pragma once

#include <util/ado/SQLFile.h>

#include <vector>
#include <map>
#include <util/ado/ADODB.h>

using namespace std;

namespace TABLES
{
class CELOADTable
{
public:
	CELOADTable(void);
	class Record
	{
	public:
		Record()
		{
			FieldValueMap.insert(make_pair(_T("C_STATE") , _T("0")));	//! C_STATE 亲格阑 固府 积己.
		}
		~Record(){}

		map<string,string> FieldValueMap;

		int SetDeleteFlag()
		{
			FieldValueMap[_T("C_STATE")] = _T("-1");
		}
		bool IsDataBaseRecord() const
		{
			map<string,string>::const_iterator where = FieldValueMap.find(_T("C_STATE"));
			if(where != FieldValueMap.end()) return (_T("1") == where->second);

			return false;
		}
		bool IsDeletedRecord() const
		{
			map<string,string>::const_iterator where = FieldValueMap.find(_T("C_STATE"));
			if(where != FieldValueMap.end()) return (_T("-1") == where->second);

			return false;
		}
	};

	~CELOADTable(void);

	virtual int SyncToSqlFile(CADODB& adoDB , CSQLFile& sqlFile);
	virtual int Load(const string& rMDBFilePath,const string& rTableName);
	virtual int Save();
protected:
	string m_rMDBFilePath , m_rTableName;

	vector<string> m_FieldNameEntry;
public:
	CELOADTable* Copy(CELOADTable* pTable);
	CELOADTable* Clone();
	vector<Record> m_RecordEntry;
	int Update(void);
	int GetValue(const int& nRecordNo , const string& rFieldName , string& rValue);
	int SetValue(const int& nRecordNo, const string& rFieldName, const string& rFieldValue);
	int GetRecordCount(void);
	int Insert(CELOADTable::Record record);
	int Clear(void);
	int CreateTable(const string& rMDBFilePath, const string& rTableName);
};
};