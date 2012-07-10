#pragma once

#include "ELOADTable.h"
#include <vector>
using namespace std;

namespace TABLES
{
	class CSchematicControlTypeMapTable : public CELOADTable
	{
	public:
		CSchematicControlTypeMapTable::CSchematicControlTypeMapTable();
		CSchematicControlTypeMapTable::~CSchematicControlTypeMapTable();

		int GetControlTypeListOf(vector<string>& ControlTypeList , const string& rSchematicType);
		////////////////////////////////////////////////////////////////////////////////////////////////////
		static const string GetTableName();
		static CELOADTable* CreateInstance()
		{
			return (new CSchematicControlTypeMapTable);
		}
	};
};
