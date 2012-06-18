#pragma once
#include "eloaditemprop.h"

namespace ITEMS
{
class CLoadItemProp :
	public CELoadItemProp
{
public:
	CLoadItemProp(CELoadItem* pOwner);
	virtual ~CLoadItemProp(void);

        CELoadItemProp* Clone();
	static CELoadItemProp* CreateInstance(void);
};
};