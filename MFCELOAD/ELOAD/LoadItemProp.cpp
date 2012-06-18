#include "StdAfx.h"
#include "LoadItemProp.h"
#include "ELoadItem.h"

using namespace ITEMS;

CLoadItemProp::CLoadItemProp(CELoadItem* pOwner) : CELoadItemProp(pOwner)
{
}

CLoadItemProp::~CLoadItemProp(void)
{
}

CELoadItemProp* CLoadItemProp::Clone()
{
	CLoadItemProp* res = new CLoadItemProp(NULL);
	if(res)
	{
		for(vector<CItemPropCategory*>::iterator itr = m_pItemPropCategory->begin();itr != m_pItemPropCategory->end();++itr)
		{
			CItemPropCategory* pCategory = (*itr)->Clone();
			if(pCategory) res->m_pItemPropCategory->push_back( pCategory );
		}
	}

	return res;
}

/**
	@brief	LoadItemProp을 생성합니다.

	@author	BHK
*/
CELoadItemProp* CLoadItemProp::CreateInstance()
{
	return (CELoadItemProp*)(new CLoadItemProp(NULL));
}