#include "StdAfx.h"
#include "..\ELoadDocData.h"
#include "CommandAutoFillMCCBProp.h"
#include "CommandAutoFillPhase.h"
#include "CommandAutoCalculateFLC.h"

using namespace COMMAND;
CCommandAutoFillPhase::CCommandAutoFillPhase(void) : CEventData("" , CEventData::TYPE_NULL)
{
}

CCommandAutoFillPhase::~CCommandAutoFillPhase(void)
{
}

/**
	@brief	fired on phase of bus is changed.

	@author BHK	

	@date 2009-04-14 ¿ÀÈÄ 7:42:55	

	@param	

	@return		
*/
int CCommandAutoFillPhase::Execute(const bool& bSetOriginalValue)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	for(int i = 0;i < m_SelectionSet.GetItemCount();++i)
	{
		CELoadItem* pItem = m_SelectionSet.GetItemAt(i);
		if(pItem && pItem->IsKindOf(CBusItem::TypeString()))
		{
			const string rBusID = pItem->prop()->GetValue(_T("General") , _T("Bus ID"));
			const string rPhase = pItem->prop()->GetValue(_T("General") , _T("Phase"));
			
			CItemSelectionSet SelectionSet;
			docData.GetLoadItemSetInBus(SelectionSet , rBusID);
			for(int j = 0;j < SelectionSet.GetItemCount();++j)
			{
				CELoadItem* pItem = SelectionSet.GetItemAt(j);
				if(pItem) pItem->prop()->SetValue(_T("Rating") , _T("Phase") , rPhase);
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			CCommandAutoFillMCCBProp cmdAutoFillMCCBProp;
			cmdAutoFillMCCBProp.m_SelectionSet = SelectionSet;
			cmdAutoFillMCCBProp.Execute();

			CCommandAutoCalculateFLC cmdAutoCalculateFLC;
			cmdAutoCalculateFLC.m_SelectionSet = SelectionSet;
			cmdAutoCalculateFLC.Execute();
		}
	}

	return ERROR_SUCCESS;
}
