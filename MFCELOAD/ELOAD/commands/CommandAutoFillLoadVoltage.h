#pragma once

#include "..\eventdata.h"

namespace COMMAND
{
class CCommandAutoFillLoadVoltage :
	public CEventData
{
public:
	CCommandAutoFillLoadVoltage(void);
	~CCommandAutoFillLoadVoltage(void);

	int Execute(const bool& bSetOriginalValue = false);
	static CEventData* CreateInstance()
	{
		return new CCommandAutoFillLoadVoltage;
	}
};
};
