#pragma once
#include "eloaditem.h"
#include "./commands/CommandPowerCableCreation.h"

using namespace COMMAND;

namespace ITEMS
{
class CTransformerItem : public CELoadItem
{
public:
	static string TableName();

	string GetName() const;
	int SetName( const string& name );

	CTransformerItem(void);
	~CTransformerItem(void);

	DECLARE_ELOAD_ITEM(CTransformerItem)
	double CalculateCapacity(void);
	int GetConnectedBusItemEntry(vector<CBusItem*>& BusItemEntry);
	int GetLoadSummaryCalcData(double nFactors[3] , double nKW[3] , double nKVAR[3]);
	int GetCableList(list<CCableItem*>& CableItemList);

	int CreatePowerCable(TABLES::CCableCreationTable::Record &SettingRecord , const ParamForPowerCableCreation& param);
private:
	int FindMinPowerCableSize(CCableCreationTable::Record &SettingRecord , int& nNoOfCable , const double& nMinSize , vector<CCableDataByType::Record*>* pRecordList , vector<CCableDataByType::Record*>::iterator& ktr);
	int FindPowerCableSize(CCableItem* pCableItem , 
		vector<CCableDataByType::Record*>* pRecordList , vector<CCableDataByType::Record*>::iterator& ktr , 
		CCableCreationTable::Record &SettingRecord , int& nMultiply , double& nCalculatedVD , ParamForPowerCableCreation param);
	double GetPFForPowerCableCreation() const;
	string GetRunVoltageDrop(TABLES::CCableCreationTable::Record &SettingRecord) const;
	int SetPowerCableProp( CELoadItemProp* pItemProp , CCableCreationTable::Record &SettingRecord , const int& nNoOfCable , 
				CCableDataByType::Record* pRecord , const double& nRunningVD , const double& nStartingVD, const ParamForPowerCableCreation& param);
private:
	ParamForPowerCableCreation m_param;
	int InitializeCableItemProperties(CCableItem* pCableItem);
};
};