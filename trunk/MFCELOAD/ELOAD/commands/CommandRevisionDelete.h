#pragma once
#include "..\eventdata.h"

namespace COMMAND
{
class CCommandRevisionDelete :
	public CEventData
{
public:
	CCommandRevisionDelete(void);
	virtual ~CCommandRevisionDelete(void);

	int Execute(const string& rRevisionNo, const string& rProductName);
	static CEventData* CreateInstance()
	{
		return new CCommandRevisionDelete;
	}
};
};