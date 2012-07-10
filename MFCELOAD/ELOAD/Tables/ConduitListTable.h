#pragma once

#include <vector>
using namespace std;

namespace TABLES
{
class CConduitListTable
{
	CConduitListTable(void);
public:
	typedef struct
	{
		string rTableNo;
		string rTableDesc;
	}TABLE_RECORD;

	const size_t GetRecordCount() const
	{
		return m_TableRecordEntry.size();
	}

	TABLE_RECORD* GetRecordAt(const size_t& at)
	{
		if(at < m_TableRecordEntry.size()) return &(m_TableRecordEntry.at(at));

		return NULL;
	}

	static CConduitListTable& GetInstance(void);
	~CConduitListTable(void);
public:
	int AddTable(const string& rTableName , const string& rTableDesc);
	int DeleteTable(const string& rTableName);
	int Load(void);
private:
	vector<TABLE_RECORD> m_TableRecordEntry;

};
};