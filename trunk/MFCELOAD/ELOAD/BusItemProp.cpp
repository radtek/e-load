#include "StdAfx.h"
#include "BusItemProp.h"
#include <fstream>
#include "Tokenizer.h"
#include "IsString.h"
#include "ItemSelectionSet.h"
#include "ELoadDocData.h"
#include "commands\CommandAutoCalculateFLC.h"
#include <vector>

using namespace std;
using namespace COMMAND;
using namespace ITEMS;

CBusItemProp::CBusItemProp(CELoadItem* pOwner) : CELoadItemProp(pOwner)
{
}

CBusItemProp::~CBusItemProp(void)
{
}

CELoadItemProp* CBusItemProp::CreateInstance()
{
	return (CELoadItemProp*)(new CBusItemProp(NULL));
}

/**
	@brief	

	@author	BHK
*/
CELoadItemProp* CBusItemProp::Clone()
{
	CBusItemProp* res = new CBusItemProp;
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
	@brief	property의 value를 설정한다.


	@author	BHK
*/
int CBusItemProp::SetValue( const string& rCategory , const string& rKey , const string& rValue )
{
	CString _rCategory(rCategory.c_str()) , _rKey(rKey.c_str());
	_rCategory.MakeUpper();
	_rKey.MakeUpper();

	if((_T("GENERAL") == _rCategory) && (_T("LOAD VOLTAGE") == _rKey))
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();

		CItemSelectionSet SelectionSet;
		docData.GetLoadItemSetInBus(SelectionSet , m_pOwner->GetName());
		for(int i = 0;i < SelectionSet.GetItemCount();++i)
		{
			SelectionSet.GetItemAt(i)->prop()->SetValue("Rating" , "Rated Voltage" , rValue);
		}

		CCommandAutoCalculateFLC cmdAutoCalculateFLC;
		cmdAutoCalculateFLC.m_SelectionSet = SelectionSet;
		cmdAutoCalculateFLC.Execute();
	}
	else if((_T("GENERAL") == _rCategory) && (_T("PHASE") == _rKey))
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();

		CItemSelectionSet SelectionSet;
		docData.GetLoadItemSetInBus(SelectionSet , m_pOwner->GetName());
		for(int i = 0;i < SelectionSet.GetItemCount();++i)
		{
			SelectionSet.GetItemAt(i)->prop()->SetValue("Rating" , "Phase" , rValue);
		}

		CCommandAutoCalculateFLC cmdAutoCalculateFLC;
		cmdAutoCalculateFLC.m_SelectionSet = SelectionSet;
		cmdAutoCalculateFLC.Execute();
	}

	CELoadItemProp::SetValue(rCategory , rKey , rValue);

	return ERROR_SUCCESS;
}