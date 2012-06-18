#pragma once
#include "eloaditem.h"
#include <string>

using namespace std;

namespace ITEMS
{
class CBusItemProp :
	public CELoadItemProp
{
public:
	CBusItemProp(CELoadItem* pOwner = NULL);
	virtual ~CBusItemProp(void);
	virtual int SetValue( const string& rCategory , const string& rKey , const string& rValue );
	static CELoadItemProp* CreateInstance(void);
private:
	
public:
        CELoadItemProp* Clone();	
};
};