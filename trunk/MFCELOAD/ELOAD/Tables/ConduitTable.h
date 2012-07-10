#pragma once

#include <string>
using namespace std;

namespace TABLES
{
	class CConduitTable
	{
	public:
		typedef struct
		{
			string CONDUIT , DIA;
		}Record;

		CConduitTable(void);
		~CConduitTable(void);
	private:
		vector<Record*> m_RecordEntry;
	public:
		int GetRecordCount() const { return m_RecordEntry.size(); }
		Record* GetRecordAt(const size_t& at) const
		{ 
			if(at < m_RecordEntry.size()) return m_RecordEntry[at];
			return NULL;
		}
		int Insert(CConduitTable::Record* pRecord);
		int Clear(void);
		int Load(const string& rTableName);
		int Save(const string& rTableName);
		///static int Delete(const string& rTableName);
	};
};
