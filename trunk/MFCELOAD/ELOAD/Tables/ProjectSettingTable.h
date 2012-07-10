#pragma once
#include "ELOADTable.h"

namespace TABLES
{
class CProjectSettingTable : public CELOADTable
{
	
public:
	CProjectSettingTable(void);
	///static CProjectSettingTable& GetInstance(void);
	~CProjectSettingTable(void);
public:
	int Load(const string& rMDBFilePath,const string& rTableName);
	///int Save(void);
	string* GetFieldValue(const string& rFieldName);
	int SetFieldValue(const string& rFieldName , const string& rFieldValue);
	int AddField(const string& rFileName);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	static const string GetTableName();
	static CELOADTable* CreateInstance()
	{
		return (new CProjectSettingTable);
	}
public:
	string GetMDBFilePath(void);
	int Update(void);
	///int SyncToSqlFile(CADODB& adoDB);
};
};