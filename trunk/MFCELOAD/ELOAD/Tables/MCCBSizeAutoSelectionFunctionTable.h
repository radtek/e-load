#pragma once

#include "ELOADTable.h"
#include "../LoadItem.h"

namespace TABLES
{
class CMCCBSizeAutoSelectionFunctionTable :
	public CELOADTable
{
public:
	CMCCBSizeAutoSelectionFunctionTable(void);
	virtual ~CMCCBSizeAutoSelectionFunctionTable(void);

	int GetRecordMeetsVoltage(CELOADTable::Record& record , ITEMS::CLoadItem* pLoadItem , const string& rVoltType);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	static const string GetTableName();
	static CELOADTable* CreateInstance()
	{
		return (new CMCCBSizeAutoSelectionFunctionTable);
	}
};
};
