#pragma once

#include <vector>
using namespace std;

namespace TABLES
{
class CFLCTable
{
public:
	typedef struct
	{
		string VALUE;
	}Record;

	CFLCTable(void);
	~CFLCTable(void);

	int Load(void);
	int Save(void);
	int GetRecordCount() const { return m_RecordEntry.size(); }
	Record* GetRecordAt(const size_t& at) const
	{ 
		if(at < m_RecordEntry.size()) return m_RecordEntry[at];
		return NULL;
	}
	int Insert(CFLCTable::Record* pRecord);
	int Clear(void);
private:
	vector<Record*> m_RecordEntry;
};
};