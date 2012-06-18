#pragma once
#include "eloaditem.h"
#include "BusItem.h"
#include <assert.h>
#include <string>
using namespace std;

namespace ITEMS
{
class CBusGroupItem :
	public CELoadItem
{
public:
	CBusGroupItem( );
	virtual ~CBusGroupItem(void);

	int AddBus( CBusItem* pBusItem )
	{
		assert(pBusItem && "BusItem is NULL");

		if(pBusItem)
		{
			vector<CBusItem*>::iterator where = find(m_BusRefEntry.begin() , m_BusRefEntry.end() , pBusItem);
			if(where == m_BusRefEntry.end()) m_BusRefEntry.push_back(pBusItem);
		}

		return ERROR_SUCCESS;
	}

	string GetName() const;
	int SetName( const string& name);
	string GetCategoryString();
	
	int GetOrderNo() const;
	int SortBusByOrderNo();

	int ClearAllBusItems(){
		m_BusRefEntry.clear();
		return ERROR_SUCCESS;
	}

	int RemoveBusItem( CBusItem* pBusItem );
	int Copy( CBusGroupItem* pCopied );
	static string TableName();

	DECLARE_ELOAD_ITEM(CBusGroupItem)
public:
	vector<CBusItem*> m_BusRefEntry;
	CBusItem* FindBusByName(const string& rBusName);
};
};