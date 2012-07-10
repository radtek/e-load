#include "StdAfx.h"
#include "..\ELoadDocData.h"
#include "CommandAutoFillMCCBProp.h"
#include "CommandAutoFillLoadVoltage.h"

using namespace COMMAND;
CCommandAutoFillLoadVoltage::CCommandAutoFillLoadVoltage(void) : CEventData("" , CEventData::TYPE_NULL)
{
}

CCommandAutoFillLoadVoltage::~CCommandAutoFillLoadVoltage(void)
{
}

/**
	@brief	

	@author BHK	

	@date 2009-04-14 ¿ÀÈÄ 7:42:55	

	@param	

	@return		
*/
int CCommandAutoFillLoadVoltage::Execute(const bool& bSetOriginalValue)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	for(int i = 0;i < m_SelectionSet.GetItemCount();++i)
	{
		CELoadItem* pItem = m_SelectionSet.GetItemAt(i);
		if(pItem && pItem->IsKindOf(CBusItem::TypeString()))
		{
			const string rBusID = pItem->prop()->GetValue(_T("General") , _T("Bus ID"));
			const string rLoadVolt = pItem->prop()->GetValue(_T("General") , _T("Load Voltage"));
			
			CItemSelectionSet SelectionSet;
			docData.GetLoadItemSetInBus(SelectionSet , rBusID);
			for(int j = 0;j < SelectionSet.GetItemCount();++j)
			{
				CELoadItem* pItem = SelectionSet.GetItemAt(j);
				if(pItem) pItem->prop()->SetValue(_T("Rating") , _T("Rated Voltage") , rLoadVolt);
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			CCommandAutoFillMCCBProp cmdAutoFillMCCBProp;
			cmdAutoFillMCCBProp.m_SelectionSet = SelectionSet;
			cmdAutoFillMCCBProp.Execute();
		}
	}

	return ERROR_SUCCESS;
}
