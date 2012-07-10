#pragma once

#include "..\eventdata.h"

namespace COMMAND
{
class CCommandAutoChangeLoadVoltOfBus :
	public CEventData
{
public:
	CCommandAutoChangeLoadVoltOfBus(void);
	~CCommandAutoChangeLoadVoltOfBus(void);

	int Execute(const bool& bSetOriginalValue = false);
	static CEventData* CreateInstance()
	{
		return new CCommandAutoChangeLoadVoltOfBus;
	}
};
};
