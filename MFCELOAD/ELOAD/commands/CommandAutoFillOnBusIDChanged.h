#pragma once
#include "..\eventdata.h"

namespace COMMAND
{
class CCommandAutoFillOnBusIDChanged :
	public CEventData
{
public:
	CCommandAutoFillOnBusIDChanged(void);
	~CCommandAutoFillOnBusIDChanged(void);

	int Execute(const bool& bSetOriginalValue =  false);
	static CEventData* CreateInstance()
	{
		return new CCommandAutoFillOnBusIDChanged;
	}
public:
};
};
