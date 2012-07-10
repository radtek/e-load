#pragma once

#include "..\Tables/CableCreationTable.h"
#include "..\CableLengthDataSheet.h"
#include "CommandPowerCableCreation.h"

namespace COMMAND
{
class CCommandPowerCableVerification : public CCommandPowerCableCreation
{
public:
	CCommandPowerCableVerification(void);
	~CCommandPowerCableVerification(void);
	int Execute(void);
	static UINT StatusThreadEntry(LPVOID pVoid);
private:
	int CreatePowerCableInBus(double nMinSize , CCableCreationTable::Record &SettingRecord , vector<CBusItem*>& BusItemEntry);
	int FindPowerCableSize(CCableItem* pCableItem , 
		vector<CCableDataByType::Record*>* pRecordList , vector<CCableDataByType::Record*>::iterator& ktr , 
		CLoadItem* pLoadItem , CCableCreationTable::Record &SettingRecord , const double& nPF , const double& I , const double& nMotorVD , const double& nMinSize , 
		int& nMultiply , double& nCalculatedVD);
	int SetPowerCableProp( CELoadItemProp* pItemProp ,
		CLoadItem* pLoadItem , CCableCreationTable::Record &SettingRecord , const int& nNoOfCable , CCableDataByType::Record* pRecord ,
		const double& nRunningVD , const double& nStartingVD);

	string m_rConduitTableName;
	double m_nConduitLength;
	double m_nConductors_1 , m_nConductors_2 , m_nConductors_Over2;
	int    m_nCheckingArea;	//! 현재 확인하고 있는 부분(CHECKING_RUNNING 혹은 CHECKING_STARTING 둘중 하나가 됨)
	int    m_nTotalCount , m_nIndex;
	double m_nRunningCalculatedVD;
	int    m_nLoadPowerCableRadio;

	CCableLengthDataSheet::ConduitSizeList m_ConduitSizeList;
	int ExecuteForEachRecord(vector<CBusItem*>& BusItemEntry , vector<CCableCreationTable::Record>& RecordList, double nMinSize);
	CCableItem* CreatePowerCableItem(CLoadItem* pLoadItem , CCableCreationTable::Record &SettingRecord);
	int StatusThread(void);
};
};
