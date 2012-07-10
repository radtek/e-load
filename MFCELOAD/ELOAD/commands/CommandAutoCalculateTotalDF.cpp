#include "StdAfx.h"
#include "..\LoadItem.h"
#include "CommandAutoCalculateTotalDF.h"

#include <sstream>

using namespace COMMAND;
CCommandAutoCalculateTotalDF::CCommandAutoCalculateTotalDF(void) : CEventData("" , CEventData::TYPE_NULL)
{
}

CCommandAutoCalculateTotalDF::~CCommandAutoCalculateTotalDF(void)
{
}

/**
	@brief	

	@author BHK	

	@date 2009-04-10 ¿ÀÈÄ 11:29:14	

	@param	

	@return		
**/
int COMMAND::CCommandAutoCalculateTotalDF::Execute(const bool& bSetOriginalValue)
{
	const int nItemCount = m_SelectionSet.GetItemCount();
	for(int i = 0;i < nItemCount;++i)
	{
		CELoadItem* pItem = m_SelectionSet.GetItemAt(i);
		if(pItem && pItem->IsKindOf(CLoadItem::TypeString()))
		{
			const string rAmpTempDF = pItem->prop()->GetValue(_T("Derating Factor") , _T("Amb Temp DF"));
			///const double nAmbTempDF = atof(.c_str());
			const double nOthersDF  = atof(pItem->prop()->GetValue(_T("Derating Factor") , _T("Others DF")).c_str());
			
			ostringstream oss;
			if(!rAmpTempDF.empty())
				oss << SAFE_ROUND(atof(rAmpTempDF.c_str())*nOthersDF,2);
			else	oss << SAFE_ROUND(nOthersDF,2);
			if(true == bSetOriginalValue) pItem->prop()->SetOriginalValue(_T("Derating Factor") , _T("Total DF") , oss.str());
			pItem->prop()->SetValue(_T("Derating Factor") , _T("Total DF") , oss.str());
		}
	}

	return ERROR_SUCCESS;
}
