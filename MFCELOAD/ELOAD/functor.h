#pragma once
#include "ELoadItem.h"
#include "Tables/ELOADTable.h"
#include "RouteItem.h"
using namespace ITEMS;
using namespace TABLES;

struct MatchRouteGroup : public binary_function<CELOADTable::Record, string, bool>
{
        bool operator()(const CELOADTable::Record lhs , const string& rhs) const
	{
		const map<string,string>::const_iterator where = lhs.FieldValueMap.find(_T("C_ROUTE_GROUP"));
		if(where != lhs.FieldValueMap.end())
		{
			return (rhs == where->second);
		}

		return false;
	}
};

struct MatchRouteType : public binary_function<CELoadItem*, string, bool>
{
        bool operator()(const CELoadItem* pItem , const string& rhs) const
	{
		return (rhs == pItem->prop()->GetValue(_T("General"), _T("Route ID")));
		
	}
};

struct MatchSummary : public binary_function<CELoadItem*, bool, bool>
{
        bool operator()(CELoadItem* pItem , const bool& rhs) const
	{
		const CRouteItem* pRouteItem = static_cast<CRouteItem*>(pItem);
		if(pRouteItem)
		{
			return (rhs == pRouteItem->IsSummary());
		}
		return false;
	}
};

struct MatchType : public binary_function<CELOADTable::Record, string, bool>
{
        bool operator()(const CELOADTable::Record lhs , const string& rhs) const
	{
		const map<string,string>::const_iterator where = lhs.FieldValueMap.find(_T("C_TYPE"));
		if(where != lhs.FieldValueMap.end())
		{
			return (rhs == where->second);
		}

		return false;
	}
};
