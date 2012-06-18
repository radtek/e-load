#pragma once
#include "ELoadItem.h"
#include "CableItem.h"
namespace ITEMS
{
class CRouteItem;
class CTrayItem : public CELoadItem
{
public:
	CTrayItem(void);
	~CTrayItem(void);
private:
	DECLARE_ELOAD_ITEM(CTrayItem)

	CRouteItem* m_pRouteItem;
public:
	static string TableName(void);
	string GetName(void);
	int SetName(const string& rName);
	void SetRouteItemPtr(CRouteItem* pRouteItem);
	CRouteItem* GetRouteItemPtr(void) const;
	string GetRouteName(void);

	vector<CCableItem*> m_CableEntry;
	int AddCableItem(CCableItem* pCableItem);
	int RemoveCableItem(CCableItem* pCableItem);
};
};
