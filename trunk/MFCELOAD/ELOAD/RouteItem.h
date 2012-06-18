#pragma once
#include "ELoaditem.h"
#include "CableItem.h"
#include "TrayItem.h"

namespace ITEMS
{
class CRouteItem : public CELoadItem
{
public:
	CRouteItem();
	~CRouteItem(void);

private:

	DECLARE_ELOAD_ITEM(CRouteItem)

public:
	static string TableName(void);
	string GetName(void) const;
	int SetName(const string& rRouteName);
	int AddHVPowerCable(CCableItem* pCableItem);
	int AddLVPowerCable(CCableItem* pCableItem);
	int AddControlCable(CCableItem* pCableItem);
	int AddTrayItem(CTrayItem* pTrayItem);
	
	int RemoveHVPowerCableItem(CCableItem* pCableItem);
	int RemoveLVPowerCableItem(CCableItem* pCableItem);
	int RemoveControlCableItem(CCableItem* pCableItem);

	vector<CTrayItem*> m_TrayItemEntry;

	vector<CCableItem*> m_HV_PowerCableEntry;
	vector<CCableItem*> m_LV_PowerCableEntry;
	vector<CCableItem*> m_ControlCableEntry;

	//BOOL IsExistCableItem(CCableItem * pCableItem);

	bool IsSummary(void) const;
	int SetSummaryFlag(const bool& bSummary);
private:
	bool m_bSummary;
public:
	int SortHVPowerCableByLength(void);
	int SortLVPowerCableByLength(void);
	int SortControlCableByLength(void);
};
};
