#pragma once

#include <vector>
using namespace std;

namespace TABLES
{
class CDCFeederCableSizingTable
{
	CDCFeederCableSizingTable(void);
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

	static CDCFeederCableSizingTable& GetInstance(void);
	~CDCFeederCableSizingTable(void);
public:
	int Load(void);
	int Save();
	int Insert(CDCFeederCableSizingTable::Record record);
	int Clear(void);
private:
	vector<Record> m_RecordEntry;
};
};
