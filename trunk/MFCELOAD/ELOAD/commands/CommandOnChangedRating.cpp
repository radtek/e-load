#include "StdAfx.h"
#include "CommandOnChangedRating.h"
#include "CommandAutoCalculateFLC.h"
#include "..\pyfromc.h"

using namespace COMMAND;

CCommandOnChangedRating::CCommandOnChangedRating(void) : CEventData("" , CEventData::TYPE_NULL)
{
}

CCommandOnChangedRating::~CCommandOnChangedRating(void)
{
}

/**
	@brief	called when rating capacity is changed.

	@author	BHK

	@date	2010.01.20
*/
int CCommandOnChangedRating::Execute(const bool& bSetOriginalValue)
{
	const int nItemCount = m_SelectionSet.GetItemCount();
	for(int i = 0;i < nItemCount;++i)
	{
		CELoadItem* pItem = m_SelectionSet.GetItemAt(i);
		if(pItem)
		{
			//! auto calculate flc
			const string& rFLC = python.CallPyCalculateFLC(pItem->GetName());
			if(bSetOriginalValue) pItem->prop()->SetOriginalValue("Rating" , "FLC" , rFLC);
			pItem->prop()->SetValue(_T("Rating") , "FLC" , rFLC);

			const double nRatingCapacity = atof(pItem->prop()->GetValue(_T("Rating") , _T("Rating Capacity")).c_str());
			const double nBHP = atof(pItem->prop()->GetValue(_T("Load") , _T("BHP <1>")).c_str());
			if(nRatingCapacity < nBHP)
			{
				//! clear BHP and LF value
				pItem->prop()->SetValue(_T("Load") , _T("BHP <1>") , _T(""));
				pItem->prop()->SetValue(_T("Characteristic") , _T("LF") , _T(""));
				if(bSetOriginalValue)
				{
					pItem->prop()->SetOriginalValue(_T("Load") , _T("BHP <1>") , _T(""));
					pItem->prop()->SetOriginalValue(_T("Characteristic") , _T("LF") , _T(""));
				}
			}

			///return ERROR_SUCCESS;
		}
	}

	return ERROR_SUCCESS;
}
