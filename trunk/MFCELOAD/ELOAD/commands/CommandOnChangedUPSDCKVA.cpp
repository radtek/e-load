#include "StdAfx.h"
#include "CommandOnChangedUPSDCKVA.h"
#include "..\pyfromc.h"

using namespace COMMAND;

CCommandOnChangedUPSKVA::CCommandOnChangedUPSKVA(void) : CEventData("" , CEventData::TYPE_NULL)
{
}

CCommandOnChangedUPSKVA::~CCommandOnChangedUPSKVA(void)
{
}

/**
	@brief	called when KVA of ups capacity is changed.

	@author	HumKyung.BAEK

	@date	2010.02.11
*/
int CCommandOnChangedUPSKVA::Execute(const bool& bSetOriginalValue)
{
	const int nItemCount = m_SelectionSet.GetItemCount();
	for(int i = 0;i < nItemCount;++i)
	{
		CELoadItem* pItem = m_SelectionSet.GetItemAt(i);
		if(pItem)
		{
			const double nBHP = atof(pItem->prop()->GetValue(_T("UPS Capacity") , _T("BHP")).c_str());
			if(0.f == nBHP)
			{
				const string rKVA = pItem->prop()->GetValue(_T("UPS Capacity") , _T("KVA"));

				pItem->prop()->SetValue(_T("UPS Capacity") , _T("BHP") , rKVA);
				if(bSetOriginalValue)
				{
					pItem->prop()->SetOriginalValue(_T("UPS Capacity") , _T("BHP") , rKVA);
				}
			}
		}
	}

	return ERROR_SUCCESS;
}


CCommandOnChangedDCKVA::CCommandOnChangedDCKVA(void) : CEventData("" , CEventData::TYPE_NULL)
{
}

CCommandOnChangedDCKVA::~CCommandOnChangedDCKVA(void)
{
}

/**
	@brief	called when KVA of dc capacity is changed.

	@author	BHK

	@date	2010.02.11
*/
int CCommandOnChangedDCKVA::Execute(const bool& bSetOriginalValue)
{
	const int nItemCount = m_SelectionSet.GetItemCount();
	for(int i = 0;i < nItemCount;++i)
	{
		CELoadItem* pItem = m_SelectionSet.GetItemAt(i);
		if(pItem)
		{
			const double nBHP = atof(pItem->prop()->GetValue(_T("DC Capacity") , _T("AC Rating BHP")).c_str());
			if(0.f == nBHP)
			{
				const string rKVA = pItem->prop()->GetValue(_T("DC Capacity") , _T("AC Rating KVA"));

				pItem->prop()->SetValue(_T("DC Capacity") , _T("AC Rating BHP") , rKVA);
				if(bSetOriginalValue)
				{
					pItem->prop()->SetOriginalValue(_T("DC Capacity") , _T("AC Rating BHP") , rKVA);
				}
			}
		}
	}

	return ERROR_SUCCESS;
}
