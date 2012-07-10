#pragma once

#include <vector>
using namespace std;

namespace TABLES
{
class C1PHFeederCableSizingTable
{
	C1PHFeederCableSizingTable(void);
public:
	typedef struct
	{
		string rKW;
	}Record;

	const size_t GetRecordCount() const
	{
		return m_RecordEntry.size();
	}

	Record* GetRecordAt(const size_t& at)
	{
		if(at < m_RecordEntry.size()) return &(m_RecordEntry.at(at));

		return NULL;
	}

	static C1PHFeederCableSizingTable& GetInstance(void);
	~C1PHFeederCableSizingTable(void);
public:
	int Load(void);
	int Save();
	int Insert(C1PHFeederCableSizingTable::Record record);
	int Clear(void);
private:
	vector<Record> m_RecordEntry;
};
};
