#pragma once
#include "ELoadItem.h"
#include "./commands/CommandPowerCableCreation.h"

namespace ITEMS
{
class CUPSDCItem : public CELoadItem
{
public:
	CUPSDCItem(void);
	~CUPSDCItem(void);

	static string TableName();
	string GetName() const;
	int SetName( const string& name );
	int GetLoadSummaryCalcData(double nFactors[3] , double nKW[3] , double nKVAR[3] , const string& rBusID);
	int GetCableList(list<CCableItem*>& CableItemList);

	DECLARE_ELOAD_ITEM(CUPSDCItem)

	double GetLoadVoltForPowerCableCreation(const int& at) const;
	double GetFLCForPowerCableCreation(const int& at) const;
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
