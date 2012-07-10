#pragma once

namespace COMMAND
{
class CCommandAutoCalculateVD :
	public CEventData
{
public:
	CCommandAutoCalculateVD(void);
	~CCommandAutoCalculateVD(void);

	int Execute(const bool& bSetOriginalValue);
	static CEventData* CreateInstance()
	{
		return new CCommandAutoCalculateVD;
	}
public:
	string m_rR , m_rX;
	string m_rCableDia , m_rCableWeight;
};
};