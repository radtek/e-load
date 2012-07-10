#pragma once

#include <list>
using namespace std;

namespace COMMAND
{
class CCommandCircleBusConnectionCheck
{
public:
	class _BusNode;
	struct _link
	{
		_BusNode* _pFrom;
		_BusNode* _pTo;
	};

	class _BusNode
	{
	public:
		string rName;
		list<_link> _linkEntry;
	};

	CCommandCircleBusConnectionCheck(void);
	~CCommandCircleBusConnectionCheck(void);

	int Execute();
private:
	int Prepare(list<_BusNode*>& nodeEntry);
};
};