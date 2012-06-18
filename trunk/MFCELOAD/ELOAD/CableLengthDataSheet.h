#pragma once

#include <vector>
using namespace std;

class CCableLengthDataSheet
{
	typedef struct
	{
		int row , col;
		string value;
	}VALUE;
public:
	typedef struct
	{
		double CableSize , ConduitSize;
	}ConduitSizeData;

	typedef vector<ConduitSizeData> ConduitSizeList;

	CCableLengthDataSheet(void);
	~CCableLengthDataSheet(void);
public:
	///CString m_rName;
	vector<double> m_RatingCapacityEntry , m_FLCEntry , m_PFEntry;
	ConduitSizeList m_ConduitSizeList;
	int PrepareCableLengthCalculation(void);
private:
	int m_nRowCount , m_nColCount;
	vector<VALUE> m_ValueEntry;
public:
	int SetValue(const int& nRow, const int& nCol, const string& rValue);
	string GetValue(const int& nRow, const int& nCol);
	int GetRowCount(void) const;
	int GetColCount(void) const;
	static int CreateConduitSizeList(const string& rCableType , const string& rConduitTableName , const double& nPercent , CCableLengthDataSheet::ConduitSizeList& rhs);
};
