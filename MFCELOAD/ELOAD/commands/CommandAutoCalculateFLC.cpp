#include "StdAfx.h"
#include "CommandAutoCalculateFLC.h"
#include "CommandAutoCalculateStartingAmp.h"
#include "..\pyfromc.h"

using namespace COMMAND;

CCommandAutoCalculateFLC::CCommandAutoCalculateFLC(void) : CEventData("" , CEventData::TYPE_NULL)
{
}

CCommandAutoCalculateFLC::~CCommandAutoCalculateFLC(void)
{
}

/**
	@brief	change "Starting Amp" after calculating FLC

	@author	BHK
*/
int CCommandAutoCalculateFLC::Execute(const bool& bSetOriginalValue)
{
	const int nItemCount = m_SelectionSet.GetItemCount();
	for(int i = 0;i < nItemCount;++i)
	{
		CELoadItem* pItem = m_SelectionSet.GetItemAt(i);
		if(pItem)
		{
			const string& rFLC = python.CallPyCalculateFLC(pItem->GetName());
			if(bSetOriginalValue) pItem->prop()->SetOriginalValue("Rating" , "FLC" , rFLC);
			pItem->prop()->SetValue("Rating" , "FLC" , rFLC);
		}
	}

	///
	CCommandAutoCalculateStartingAmp cmdAutoCalculateStaringAmp;
	cmdAutoCalculateStaringAmp.m_SelectionSet = m_SelectionSet;
	cmdAutoCalculateStaringAmp.Execute();

	return ERROR_SUCCESS;
}