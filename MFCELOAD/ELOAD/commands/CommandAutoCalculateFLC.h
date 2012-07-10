#pragma once
#include "..\eventdata.h"

namespace COMMAND
{
class CCommandAutoCalculateFLC :
	public CEventData
{
public:
	CCommandAutoCalculateFLC(void);
	virtual ~CCommandAutoCalculateFLC(void);

	int Execute(const bool& bSetOriginalValue = false);
	static CEventData* CreateInstance()
	{
		return new CCommandAutoCalculateFLC;
	}
};
};