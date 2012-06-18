#pragma once
#include "eloaditem.h"
#include <vector>
using namespace std;

namespace ITEMS
{
class CBusGroupItemProp :
	public CELoadItemProp
{
public:
	CBusGroupItemProp(CELoadItem* pOwner);	
	virtual ~CBusGroupItemProp(void);

        int SetValue( const string& rCategory , const string& rKey , const string& rValue );
        CELoadItemProp* Clone();
private:

	
public:
        static CELoadItemProp* CreateInstance(void);
};
};