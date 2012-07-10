#pragma once

#include <vector>
using namespace std;

namespace TABLES
{
class C3PHMotorCableSizingTable
{
	C3PHMotorCableSizingTable(void);
public:
	typedef struct
	{
		string sKW;	//! capacity
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

	static C3PHMotorCableSizingTable& GetInstance(void);
	~C3PHMotorCableSizingTable(void);
public:
	int Load(void);
	int Save();
	int Insert(C3PHMotorCableSizingTable::Record record);
	int Clear(void);
private:
	vector<Record> m_RecordEntry;
};
};