#include "StdAfx.h"
#include "CommandUndo.h"
#include "..\ELoadDocData.h"

#include <vector>
using namespace std;

using namespace COMMAND;

CCommandUndo::CCommandUndo(void)
{
}

CCommandUndo::~CCommandUndo(void)
{
}

/**
	@brief	������ DATA�� ���� ���ͽ�Ų��.

	@author	BHK
*/
int CCommandUndo::Execute()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> LoadItemList;
	docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
	if(!LoadItemList.empty())
	{
		for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
		{
			CLoadItem* pLoad = static_cast<CLoadItem*>(*itr);
			pLoad->prop()->ResetModifiedValues();
		}
	}

	return ERROR_SUCCESS;
}