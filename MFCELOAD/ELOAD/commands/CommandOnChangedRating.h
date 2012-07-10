#pragma once
#include "..\eventdata.h"

namespace COMMAND
{
class CCommandOnChangedRating :
	public CEventData
{
public:
	CCommandOnChangedRating(void);
	virtual ~CCommandOnChangedRating(void);

	int Execute(const bool& bSetOriginalValue = false);
	static CEventData* CreateInstance()
	{
		return new CCommandOnChangedRating;
	}
};
};
