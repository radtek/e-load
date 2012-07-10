#pragma once

#include <util/ado/SQLFile.h>

#include <vector>
using namespace std;

namespace TABLES
{
class CCableDataByType
{
public:
	typedef struct
	{
		map<string , string> m_FieldMap;
		
		string GetCableType();
	}Record;

	~CCableDataByType();
	int Clear();
	int Load(CADODB& adoDB, const int& nRecordNo , vector<STRING_T>& FieldNameEntry , CSQLFile& sqlFile);
	int Save(CADODB& adoDB , const string& rTableName);
	vector<CCableDataByType::Record*>* GetRecordEntryOfHertz(const int& nHertz=-1);
	int Insert(CCableDataByType::Record* pRecord);
	int GetHertzList(vector<int>& HertzEntry);
private:
	map<int , vector<Record*>* > m_RecordMap;
};

class CCableDataTable
{
public:
	CCableDataTable(void);
	~CCableDataTable(void);
private:
	map<string , CCableDataByType* > m_CableDataByTypeMap;
public:
	string GetTableName() const { return m_rTableName; }

	CCableDataByType* GetRecordListOf(const string& rId);
	int GetCableCoreList(const int nHertz , const string& rVolt , const string& rType , vector<string>& CableCoreList);
	int GetCableTypeKeyList(vector<string>& CableTypeKeyList);
	int GetCableTypeKeyListOfHertz(vector<string>& CableTypeKeyList, const int& nHertz);
	int Insert(CCableDataByType::Record* pRecord);
        ///int DeleteRecord(CCableDataTable::Record* pRecord);
	int Clear(void);
	int Save(const string& rTableName);
	int Load(const string& rTableName , const bool& bForce = false);
	static string CreateCableId(string rVolt , string rCore , string rConductor , string rCableType , string rMPR);
private:
	int SyncToSQLFile(CADODB& adoDB , const string& rTableName , const vector<STRING_T>& WillBeDeletedFieldNameEntry , const vector<STRING_T>& ShouldBeAddedFieldNameEntry);

	bool m_bLoaded;
	string m_rTableName;
};
};