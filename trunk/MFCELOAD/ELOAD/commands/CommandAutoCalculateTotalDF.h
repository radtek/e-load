#pragma once
#include "..\eventdata.h"

namespace COMMAND
{
class CCommandAutoCalculateTotalDF :
	public CEventData
{
public:
	CCommandAutoCalculateTotalDF(void);
	~CCommandAutoCalculateTotalDF(void);
	int Execute(const bool& bSetOriginalValue);

	static CEventData* CreateInstance()
	{
		return (new CCommandAutoCalculateTotalDF);
	}
};
};