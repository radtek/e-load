
#pragma once

#include "ELOADTable.h"

using namespace std;

namespace TABLES
{
class CCableCreationTable : public CELOADTable
{
protected:
	CCableCreationTable(void);
public:
	~CCableCreationTable(void);

public:
	int Update(void);
	int Save(void);
};
};