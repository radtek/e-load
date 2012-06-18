#pragma once
#include "eloaditem.h"
#include <string>
using namespace std;

namespace ITEMS
{
class CBusGroupItem;
class CTransformerItem;
class CCableItem;
class CBusItem :
	public CELoadItem
{
public:
	CBusItem();
	virtual ~CBusItem(void);

	string GetBusGroupName() const;
	void SetBusGroupItemPtr( CBusGroupItem* pBusGroupItem );
	CBusGroupItem* GetBusGroupItemPtr() const;
	string GetCategoryString();

	string GetName() const;
	int SetName( const string& name);

	int Copy( CBusItem* pCopied );
	int GetOrderNo() const;
	static string TableName();
	int SetDeleteFlag(const bool& bDelete);
	bool valid() const;

	DECLARE_ELOAD_ITEM(CBusItem)
private:
	CBusGroupItem* m_pBusGroupItem;
public:
	int GetLoadSummaryCalcData(double nFactors[3] , double nKW[3] , double nKVAR[3] , vector<string> *pLoadItemEntry = NULL);
	int GetLoadSummaryResultData(double& nKW, double& nKVAR , vector<string> *pLoadItemEntry = NULL);
	int CalcLoadSummary();
	double GetPowerFactor(void);
	int GetConnectedTrItemEntry(vector<CTransformerItem*>& TrItemEntry);
	int GetCableList(list<CELoadItem*>& CableItemList);
};
};