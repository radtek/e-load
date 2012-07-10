#include "StdAfx.h"
#include "CommandAutoFillOnBusIDChanged.h"
#include "..\ELoadDocData.h"
#include "..\MainFrm.h"
using namespace COMMAND;

CCommandAutoFillOnBusIDChanged::CCommandAutoFillOnBusIDChanged(void): CEventData("" , CEventData::TYPE_NULL)
{
}

CCommandAutoFillOnBusIDChanged::~CCommandAutoFillOnBusIDChanged(void)
{
}

/**
	@brief	LOCATION AREA 를 수정하거나 추가하면 Sort Area에 Area를 추가한다.

	@author KHS	

	@date 2009-06-01 오후 1:24:41	

	@param	

	@return		
**/
int CCommandAutoFillOnBusIDChanged::Execute(const bool& bSetOriginalValue)
{
	//! Load item 에서 BUS ID를 변경한다.
	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> LoadItemList , TrItemList , CapacitorItemList , GeneratorItemList ,UPSDCItemList;
	docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
	docData.GetELoadItemListOf(TrItemList , CTransformerItem::TypeString());
	docData.GetELoadItemListOf(CapacitorItemList , CCapacitorItem::TypeString());
	docData.GetELoadItemListOf(GeneratorItemList , CGeneratorItem::TypeString());
	docData.GetELoadItemListOf(UPSDCItemList , CUPSDCItem::TypeString());

	for(int i = 0;i < m_SelectionSet.GetItemCount();++i)
	{
		const string rOldBusID = m_SelectionSet.GetItemAt(i)->prop()->GetValue(_T("General") , _T("Bus ID"));

		for(list<CELoadItem*>::iterator itr = LoadItemList.begin(); itr != LoadItemList.end();++itr)
		{
			CLoadItem* pLoad = static_cast<CLoadItem*>(*itr);
			if( pLoad->GetBusId() == rOldBusID) pLoad->SetBusId(m_rNewValue);
		}

		for(list<CELoadItem*>::iterator itr = TrItemList.begin(); itr != TrItemList.end();++itr)
		{
			CTransformerItem* pTrItem = static_cast<CTransformerItem*>(*itr);
			if(pTrItem)
			{
				const string rFromBusID = pTrItem->prop()->GetValue(_T("From") , _T("Bus Id"));
				const string rToBusID = pTrItem->prop()->GetValue(_T("To") , _T("Bus Id"));

				if(rFromBusID == rOldBusID) pTrItem->prop()->SetValue(_T("From") , _T("Bus Id") , m_rNewValue);
				if(rToBusID == rOldBusID) pTrItem->prop()->SetValue(_T("To") , _T("Bus Id") , m_rNewValue);
			}
		}
		for(list<CELoadItem*>::iterator itr = CapacitorItemList.begin(); itr != CapacitorItemList.end();++itr)
		{
			CCapacitorItem* pCapacitorItem = static_cast<CCapacitorItem*>(*itr);
			if(pCapacitorItem)
			{
				const string rFromBusID = pCapacitorItem->prop()->GetValue(_T("From") , _T("Bus Id"));

				if(rFromBusID == rOldBusID) pCapacitorItem->prop()->SetValue(_T("From") , _T("Bus Id") , m_rNewValue);
			}
		}

		for(list<CELoadItem*>::iterator itr = GeneratorItemList.begin(); itr != GeneratorItemList.end();++itr)
		{
			CGeneratorItem* pGeneratorItem = static_cast<CGeneratorItem*>(*itr);
			if(pGeneratorItem)
			{
				const string rToBusID = pGeneratorItem->prop()->GetValue(_T("To") , _T("Bus ID"));

				if(rToBusID == rOldBusID) pGeneratorItem->prop()->SetValue(_T("To") , _T("Bus ID") , m_rNewValue);
			}
		}
	
		for(list<CELoadItem*>::iterator itr = UPSDCItemList.begin(); itr != UPSDCItemList.end();++itr)
		{
			CUPSDCItem* pUPSDCItem = static_cast<CUPSDCItem*>(*itr);
			if(pUPSDCItem)
			{
				const string rFrom1BusID = pUPSDCItem->prop()->GetValue(_T("From1") , _T("Bus ID"));
				const string rFrom2BusID = pUPSDCItem->prop()->GetValue(_T("From2") , _T("Bus ID"));
				const string rToBusID = pUPSDCItem->prop()->GetValue(_T("To") , _T("Bus ID"));

				if(rFrom1BusID == rOldBusID) pUPSDCItem->prop()->SetValue(_T("From1") , _T("Bus ID") , m_rNewValue);
				if(rFrom2BusID == rOldBusID) pUPSDCItem->prop()->SetValue(_T("From2") , _T("Bus ID") , m_rNewValue);
				if(rToBusID == rOldBusID) pUPSDCItem->prop()->SetValue(_T("To") , _T("Bus ID") , m_rNewValue);
			}
		}
	}

	return ERROR_SUCCESS;
}
