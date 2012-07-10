#pragma once
#include "..\eventdata.h"

namespace COMMAND
{
class CCommandOnChangedUPSKVA :
	public CEventData
{
public:
	CCommandOnChangedUPSKVA(void);
	virtual ~CCommandOnChangedUPSKVA(void);

	int Execute(const bool& bSetOriginalValue = false);
	static CEventData* CreateInstance()
	{
		return new CCommandOnChangedUPSKVA;
	}
};

class CCommandOnChangedDCKVA :
	public CEventData
{
public:
	CCommandOnChangedDCKVA(void);
	virtual ~CCommandOnChangedDCKVA(void);

	int Execute(const bool& bSetOriginalValue = false);
	static CEventData* CreateInstance()
	{
		return new CCommandOnChangedDCKVA;
	}
};
};
