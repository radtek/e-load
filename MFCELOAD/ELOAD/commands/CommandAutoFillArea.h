#pragma once
#include "..\eventdata.h"

namespace COMMAND
{
class CCommandAutoFillArea :
	public CEventData
{
public:
	CCommandAutoFillArea(void);
	~CCommandAutoFillArea(void);

	int Execute(const bool& bSetOriginalValue =  false);
	static CEventData* CreateInstance()
	{
		return new CCommandAutoFillArea;
	}
};
};