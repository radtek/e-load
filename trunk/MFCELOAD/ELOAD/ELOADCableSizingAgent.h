#pragma once

#include <string>
#include <vector>
using namespace std;

class CELOADCableSizingAgent
{
public:
	CELOADCableSizingAgent(void);
	~CELOADCableSizingAgent(void);

	int GetCableTypeList(vector<string>& CableTypeList);
	int LoadCableDataTableIfNeeded();
};
