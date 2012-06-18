#pragma once
#include "ELoadItem.h"

namespace ITEMS
{
class CGeneratorItem : public CELoadItem
{
public:
	CGeneratorItem(void);
	~CGeneratorItem(void);

	static string TableName();
	string GetName() const;
	int SetName( const string& name );

	DECLARE_ELOAD_ITEM(CGeneratorItem)
};
};
