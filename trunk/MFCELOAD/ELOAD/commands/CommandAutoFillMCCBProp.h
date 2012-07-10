#pragma once

#include "..\LoadItem.h"
#include "..\Tables\MCCBSizeAutoSelectionFunctionTable.h"

namespace COMMAND
{
class CCommandAutoFillMCCBProp :
	public CEventData
{

	typedef struct
	{
		string rBusVolt;
		string rPhase;
		string rVoltType;
	}BUSVOLT_VOLTTYPE_RECORD;

public:
	CCommandAutoFillMCCBProp(const BOOL& bAllLoads = TRUE);
	~CCommandAutoFillMCCBProp(void);

	int Execute(const bool& bSetOriginalValue = false);
	static CEventData* CreateInstance()
	{
		return (new CCommandAutoFillMCCBProp);
	}
private:
	int ExecuteOnLoadItem(const bool& bSetOriginalValue, ITEMS::CLoadItem* pLoadItem,TABLES::CMCCBSizeAutoSelectionFunctionTable* pTable);
	int ClearMCCBProp(const bool& bSetOriginalValue,CLoadItem* pLoadItem);

	//vector<string,string> m_BusVolt_VoltTypeMap;
	vector<BUSVOLT_VOLTTYPE_RECORD> m_BusVolt_VoltTypeEntry;
	BOOL m_bAllLoads;
};
};