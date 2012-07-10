#pragma once

#include <vector>
using namespace std;

namespace TABLES
{
class C3PHFeederCableSizingTable
{
	C3PHFeederCableSizingTable(void);
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

	static C3PHFeederCableSizingTable& GetInstance(void);
	~C3PHFeederCableSizingTable(void);
public:
	int Load(void);
	int Save();
	int Insert(C3PHFeederCableSizingTable::Record record);
	int Clear(void);
private:
	vector<Record> m_RecordEntry;
};
};