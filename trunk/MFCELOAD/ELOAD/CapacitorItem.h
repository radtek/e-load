#pragma once
#include "ELoadItem.h"
#include "Tables/CableCreationTable.h"
#include "commands/CommandPowerCableCreation.h"

namespace ITEMS
{
class CCapacitorItem : public CELoadItem
{
public:
	CCapacitorItem(void);
	~CCapacitorItem(void);

	static string TableName();
	string GetName() const;
	int SetName( const string& name );

	DECLARE_ELOAD_ITEM(CCapacitorItem)

	double CalculateCapacity(const double& dTargetPowerFactor);
	int SetSelectedCapacity(double& dFlc, double& dImprovedPowerFactor, const double& dSelectedCapacity);
	int GetCapacitorData(double& nKW, double& nPF);
	int GetConnectedBusItem(vector<CBusItem*>& ConnectedBusItemEntry);
	int GetCableList(list<CCableItem*>& CableItemList);
private:
	int FindConnectedBusItem(const string& rBusId, vector<CBusItem*>& ConnectedBusItemEntry);
	int FindMinPowerCableSize(TABLES::CCableCreationTable::Record &SettingRecord , int& nNoOfCable , const double& nMinSize , vector<TABLES::CCableDataByType::Record*>* pRecordList , vector<CCableDataByType::Record*>::iterator& ktr);
	double GetPFForPowerCableCreation() const;
	string GetRunVoltageDrop(TABLES::CCableCreationTable::Record &SettingRecord) const;
	int FindPowerCableSize(CCableItem* pCableItem , 
		vector<CCableDataByType::Record*>* pRecordList , vector<CCableDataByType::Record*>::iterator& ktr , 
		CCableCreationTable::Record &SettingRecord , int& nMultiply , double& nCalculatedVD , ParamForPowerCableCreation param);
	int SetPowerCableProp( CELoadItemProp* pItemProp , CCableCreationTable::Record &SettingRecord , const int& nNoOfCable , 
					CCableDataByType::Record* pRecord , const double& nRunningVD , const double& nStartingVD, const ParamForPowerCableCreation& param);
public:
	int CreatePowerCable(TABLES::CCableCreationTable::Record &SettingRecord , const ParamForPowerCableCreation& param);
private:
	ParamForPowerCableCreation m_param;
	int InitializeCableItemProperties(CCableItem* pCableItem);
};
};