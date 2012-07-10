#pragma once
#include "..\eventdata.h"

namespace COMMAND
{
class CCommandAutoCalculateStartingAmp :
	public CEventData
{
public:
	CCommandAutoCalculateStartingAmp(void);
	virtual ~CCommandAutoCalculateStartingAmp(void);
	int Execute(const bool& bSetOriginalValue = false);

	static CEventData* CreateInstance()
	{
		return (new CCommandAutoCalculateStartingAmp);
	}
};
};