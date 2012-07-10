#pragma once
#include "..\eventdata.h"

namespace COMMAND
{
class CCommandUndo
{
public:
	CCommandUndo(void);
	~CCommandUndo(void);

	int Execute();
};
};