#pragma once

#include "ELOADTable.h"
#include <vector>
using namespace std;

namespace TABLES
{
	class CControlTypeTable : public CELOADTable
	{
	public:
		CControlTypeTable::CControlTypeTable();
		CControlTypeTable::~CControlTypeTable();

		int GetControlTypeList(CStringArray& options);
		////////////////////////////////////////////////////////////////////////////////////////////////////
		static const string GetTableName();
		static CELOADTable* CreateInstance()
		{
			return (new CControlTypeTable);
		}
	};
};
