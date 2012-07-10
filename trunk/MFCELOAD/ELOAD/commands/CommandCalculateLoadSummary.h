#pragma once

namespace COMMAND
{
class CCommandCalculateLoadSummary
{
protected:
public:
	int Execute();

	CCommandCalculateLoadSummary();
	~CCommandCalculateLoadSummary(void);
public:
	string GetResultString(void);
	static UINT StatusThreadEntry(LPVOID pVoid);
private:
	int StatusThread(void);

	int m_nTryCount , m_nSuccessCount , m_nFailCount;
};
};
