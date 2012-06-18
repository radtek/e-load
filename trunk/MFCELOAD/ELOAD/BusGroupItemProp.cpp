#include "StdAfx.h"
#include "BusGroupItemProp.h"
#include "Tokenizer.h"
#include "IsString.h"
#include <fstream>
#include <string>
#include "BusGroupItem.h"

using namespace std;
using namespace ITEMS;

CBusGroupItemProp::CBusGroupItemProp(CELoadItem* pOwner) : CELoadItemProp(pOwner)
{
}

CBusGroupItemProp::~CBusGroupItemProp(void)
{
}

/**
        @brief  

        @author BHK
*/
int CBusGroupItemProp::SetValue( const string& rCategory , const string& rKey , const string& rValue )
{
        CELoadItemProp::SetValue(rCategory , rKey , rValue);
        
        if(("GENERAL" == rCategory) && ("BUS GROUP ID" == rKey))
        {
                CBusGroupItem* pBusGroupItem = static_cast<CBusGroupItem*>(m_pOwner);
                for(vector<CBusItem*>::iterator itr = pBusGroupItem->m_BusRefEntry.begin();itr != pBusGroupItem->m_BusRefEntry.end();++itr)
                {
                        CELoadItemProp* pProp = (*itr)->prop();
                        pProp->SetValue("GENERAL" , "BUS GROUP" , rValue);
                }
        }
        
	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
CELoadItemProp* CBusGroupItemProp::Clone()
{
	CBusGroupItemProp* res = new CBusGroupItemProp(NULL);
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

CELoadItemProp* CBusGroupItemProp::CreateInstance(void)
{
        return new CBusGroupItemProp(NULL);
}
