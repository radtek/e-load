#pragma once

#include "..\eventdata.h"

namespace COMMAND
{
class CCommandAutoFillPhase :
	public CEventData
{
public:
	CCommandAutoFillPhase(void);
	~CCommandAutoFillPhase(void);

	int Execute(const bool& bSetOriginalValue = false);
	static CEventData* CreateInstance()
	{
		return new CCommandAutoFillPhase;
	}
};
};
