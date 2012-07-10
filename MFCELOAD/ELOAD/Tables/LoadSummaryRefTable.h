#pragma once
#include <map>
using namespace std;

namespace TABLES
{

typedef struct
{
	double PF100 , PF75 , PF50;
	double EFF100 , EFF75 , EFF50;
	double PF , EFF;
	double LRC , PFlr;
}LoadSearchResult;

class CDataGroupByPole
{
public:
        CDataGroupByPole();
        ~CDataGroupByPole();
        int FindValues(LoadSearchResult& res , const double& nRatingCapacity , const double& nLF);
        typedef struct
        {
                double m_nKW;	//! rating capacity
                double m_nVOLT, m_nLRC, m_nPFlr;
                int m_nPOLE, m_nHZ;
                double m_nPF100, m_nPF75, m_nPF50;
                double m_nEFF100, m_nEFF75, m_nEFF50;

                int GetValues(LoadSearchResult& res , const double& nLF)
                {
			res.PF100  = m_nPF100,  res.PF75  = m_nPF75  , res.PF50  = m_nPF50;
			res.EFF100 = m_nEFF100, res.EFF75 = m_nEFF75 , res.EFF50 = m_nEFF50;
			res.LRC    = m_nLRC , res.PFlr = m_nPFlr;
			if(nLF == 1.0)
                        {	
				res.PF = ((0. == m_nPF75) && (0. == m_nPF50))  ?  m_nPF100  : m_nPF100;
                                res.EFF= ((0. == m_nEFF75) && (0. == m_nEFF50)) ? m_nEFF100 : m_nEFF100;
                                return ERROR_SUCCESS;
                        }
                        else if(nLF == 0.75)
                        {
                                res.PF = ((0. == m_nPF75) && (0. == m_nPF50)) ? m_nPF100 : m_nPF75;
                                res.EFF= ((0. == m_nEFF75) && (0. == m_nEFF50)) ? m_nEFF100 : m_nEFF75;
                                return ERROR_SUCCESS;
                        }
                        else if(nLF == 0.5)
                        {
                                res.PF = ((0. == m_nPF75) && (0. == m_nPF50)) ? m_nPF100 : m_nPF50;
                                res.EFF= ((0. == m_nEFF75) && (0. == m_nEFF50)) ? m_nEFF100 : m_nEFF50;
                                return ERROR_SUCCESS;
                        }
			else if((0.0 <= nLF) && (nLF < 0.5))
                        {
                                const double prev = 0.0;
                                const double next = 0.5;
                                const double dist = next - prev;
                                const double factor = (nLF - prev) / dist;
                                res.PF = ((0. == m_nPF75) && (0. == m_nPF50)) ? m_nPF100 : (0.0 + (m_nPF50 - 0.0) * factor); 
                                res.EFF= ((0. == m_nEFF75) && (0. == m_nEFF50)) ? m_nEFF100 : (0.0 + (m_nEFF50 - 0.0) * factor);
                                return ERROR_SUCCESS;
                        }
                        else if( (0.5 < nLF) && (nLF < 0.75))
                        {
                                const double prev = 0.5;
                                const double next = 0.75;
                                const double dist = next - prev;
                                const double factor = (nLF - prev) / dist;
                                res.PF = ((0. == m_nPF75) && (0. == m_nPF50)) ? m_nPF100 : (m_nPF50 + (m_nPF75 - m_nPF50) * factor); 
                                res.EFF= ((0. == m_nEFF75) && (0. == m_nEFF50)) ? m_nEFF100 : (m_nEFF50 + (m_nEFF75 - m_nEFF50) * factor);
                                return ERROR_SUCCESS;
                        }
                        else if((0.75 < nLF) && (nLF < 1.0))
                        {
                                const double prev = 0.75;
                                const double next = 1.0;
                                const double dist = next - prev;
                                const double factor = (nLF - prev) / dist;
                                res.PF = ((0. == m_nPF75) && (0. == m_nPF50)) ? m_nPF100 : (m_nPF75 + (m_nPF100 - m_nPF75) * factor); 
                                res.EFF= ((0. == m_nEFF75) && (0. == m_nEFF50)) ? m_nEFF100 : (m_nEFF75 + (m_nEFF100 - m_nEFF75) * factor);
                                return ERROR_SUCCESS;
                        }
                        else
                        {
                                res.PF = 0.0;
                                res.EFF = 0.0;
                        }

                        return ERROR_INVALID_PARAMETER;
                }

        }Record;

        int Load(CADODB& adoDB, int& i);
        int Sort();
        int Clear();
private:
        vector<Record*> m_RefRecordEntry;

public:
        int GetRatingCapacityList(vector<double>& RatingCapacityEntry);
};

class CDataGroupByVolt
{

public:
        CDataGroupByVolt();
        ~CDataGroupByVolt();
        int FindValues(LoadSearchResult& res , const double& nRatingCapacity , const int& nPoles, const double& nLF);
	int GetRatingCapacityList(vector<double>& RatingCapacityEntry , const int& nPoles);
        int Load(CADODB& adoDB,int& i);
        int Sort();
        int Clear();
private:
        map<int , CDataGroupByPole*> m_DataGroupByPoleEntry;
};

class CDataGroupByHertz
{

public:
        CDataGroupByHertz();
        ~CDataGroupByHertz();
        int FindValues(LoadSearchResult& res , const double& nRatingCapacity , const double& nVolt , const int& nPoles, const double& nLF);
        int Load(CADODB& adoDB,int& i);
        int Sort();
        int Clear();
        int GetRatingCapacityList(vector<double>& RatingCapacityEntry , const double& nVolt , const int& nPoles);
private:
        map<double , CDataGroupByVolt*> m_DataGroupByVoltEntry;
};

class CLoadSummaryRefTable
{
	CLoadSummaryRefTable();
public:
	~CLoadSummaryRefTable();
	static CLoadSummaryRefTable& GetInstance();
	string GetTableName() const
	{
		return m_rTableName;
	}
private:
        int LoadDataFromDatabase(const string& rTableName);
        
        string m_rTableName;
private:
        map<int , CDataGroupByHertz*> m_DataGroupByHertzEntry;
public:
        int Clear(void);
        static int Load(const bool& bForce = false);
	int FindValues(LoadSearchResult& res , const double& nRatingCapacity , const int& nHz, const double& nVolt , const int& nPoles , const double& nLF);
};
};
