#pragma once

#include "eloaditem.h"
#include ".\Tables/CableCreationTable.h"
#include "commands/CommandPowerCableCreation.h"
#include <util/ado/ADODB.h>
#include "CableItem.h"
#include <vector>

namespace ITEMS
{
class CLoadItem :
	public CELoadItem
{
public:
	CLoadItem();
	virtual ~CLoadItem(void);

	string GetName() const;
	int SetName( const string& name );

	string GetBusId() const;
	int SetBusId( const string& rBusId );

	int SetValue( const string& rCategory , const string& rKey , const string& rValue );
	int Copy( CLoadItem* pCopied );
	static string TableName();

	int AddCable( CELoadItem* pItem )
	{
		assert(pItem && "pItem is NULL");

		if(pItem && (pItem->IsKindOf(CCableItem::TypeString())))
			m_CableRefEntry.push_back(pItem);

		return ERROR_SUCCESS;
	}

	int GetLoadSummaryCalcData(double nFactors[3] , double nKW[3] , double nKVAR[3]);
	int SetDeleteFlag(const bool& bDelete);
	vector<CELoadItem*> m_CableRefEntry;

	DECLARE_ELOAD_ITEM(CLoadItem)
	int ClearCableItems(const string& rCategory="",  const string& rCreatedBy = "");
	int CalcRatingCapacity(double& nRatingCapacity);
	int CalcLoadSummary();
	double GetRatingCapacityForPowerCableCreation();
	double GetFLCForPowerCableCreation();
	double GetPFForPowerCableCreation();
	double GetEFFForPowerCableCreation();
	int GetCableCountOf(const string& rCableCategory);
private:
	string m_rRemark;
	int FindPowerCableSize(CCableItem* pCableItem , vector<CCableDataByType::Record*>* pRecordList , vector<CCableDataByType::Record*>::iterator& ktr , 
		CCableCreationTable::Record &SettingRecord , const double& nPF , const double& I , const double& nMotorVD , const double& nMinSize , int& nMultiply , double& nCalculatedVD);
	int SetPowerCableProp( CELoadItemProp* pItemProp , CCableCreationTable::Record &SettingRecord , const int& nNoOfCable , 
		CCableDataByType::Record* pRecord , const double& nRunningVD , const double& nStartingVD);
public:
	int CreatePowerCable(double nMinSize, CCableCreationTable::Record& SettingRecord, const double& nBusVolt);
	string GetRunVoltageDrop(void);
	int FindMinPowerCableSize(int& nNoOfCable , const double& nMinSize , vector<CCableDataByType::Record*>* pRecordList , vector<CCableDataByType::Record*>::iterator& ktr);
	int GetCableOfType(vector<CCableItem*>& CableItemEntry, const string& rCableType, const string& rControlTo = _T(""));
	int InitializeCableItemProperties(CCableItem* pCableItem);
private:
};
};