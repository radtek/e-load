#pragma once
#include "ELoadItem.h"

class CItemMemStrategy
{
	CItemMemStrategy(void);
	~CItemMemStrategy(void);
public:
	class CItemMemStrategyState
	{
	public:
		CItemMemStrategyState(){}
		virtual ~CItemMemStrategyState(){}

		virtual int OnCreate(ITEMS::CELoadItem* pItem){ return ERROR_SUCCESS; }
		virtual int OnDelete(ITEMS::CELoadItem* pItem){ return ERROR_SUCCESS; }
	};

	static CItemMemStrategy& GetInstance();

	int OnCreate(ITEMS::CELoadItem* pItem);
	int OnDelete(ITEMS::CELoadItem* pItem);

	CItemMemStrategyState* m_pStrategyState;
};
