#include "StdAfx.h"
#include "..\ELoadDocData.h"
#include "../Tables/CableCreationSettingTable.h"
#include "..\Tables/MCCBSizeAutoSelectionFunctionTable.h"
#include "CommandAutoFillMCCBProp.h"

using namespace COMMAND;
CCommandAutoFillMCCBProp::CCommandAutoFillMCCBProp(const BOOL& bAllLoads) : CEventData("" , CEventData::TYPE_NULL)
{
	m_bAllLoads = bAllLoads;
}

CCommandAutoFillMCCBProp::~CCommandAutoFillMCCBProp(void)
{
}

/**
	@brief	Load의 MCCB 항목들을 채운다.

	@author BHK	

	@date 2009-04-14 오후 4:37:41	

	@param	

	@return		
*/
int CCommandAutoFillMCCBProp::Execute(const bool& bSetOriginalValue)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CMCCBSizeAutoSelectionFunctionTable* pTable = (CMCCBSizeAutoSelectionFunctionTable*)docData.GetTableOf(CMCCBSizeAutoSelectionFunctionTable::GetTableName());
	if(pTable)
	{
		//m_BusVolt_VoltTypeMap.clear();
		m_BusVolt_VoltTypeEntry.clear();
		//! bus voltage와 voltage type간의 map을 생성한다.
		CLV3PHCableCreationSettingTable& LV3PHVoltageSettingTable = CLV3PHCableCreationSettingTable::GetInstance();
		{
			for(vector<CELOADTable::Record>::iterator itr = LV3PHVoltageSettingTable.m_RecordEntry.begin();itr != LV3PHVoltageSettingTable.m_RecordEntry.end();++itr)
			{
				if(itr->FieldValueMap[_T("C_BUS_VOLTAGE")].empty()) continue;
				
				BUSVOLT_VOLTTYPE_RECORD record;
				record.rBusVolt = itr->FieldValueMap[_T("C_BUS_VOLTAGE")];
				record.rVoltType = itr->FieldValueMap[_T("C_VOLTAGE_TYPE")];
				record.rPhase = _T("3");
				m_BusVolt_VoltTypeEntry.push_back(record);
				//m_BusVolt_VoltTypeMap[itr->FieldValueMap[_T("C_BUS_VOLTAGE")]] = itr->FieldValueMap[_T("C_VOLTAGE_TYPE")];
			}
		}
		CLV1PHCableCreationSettingTable& LV1PHVoltageSettingTable = CLV1PHCableCreationSettingTable::GetInstance();
		{
			for(vector<CELOADTable::Record>::iterator itr = LV1PHVoltageSettingTable.m_RecordEntry.begin();itr != LV1PHVoltageSettingTable.m_RecordEntry.end();++itr)
			{
				if(itr->FieldValueMap[_T("C_BUS_VOLTAGE")].empty()) continue;

				BUSVOLT_VOLTTYPE_RECORD record;
				record.rBusVolt = itr->FieldValueMap[_T("C_BUS_VOLTAGE")];
				record.rVoltType = itr->FieldValueMap[_T("C_VOLTAGE_TYPE")];
				record.rPhase = _T("1");
				m_BusVolt_VoltTypeEntry.push_back(record);
				//m_BusVolt_VoltTypeMap[itr->FieldValueMap[_T("C_BUS_VOLTAGE")]] = itr->FieldValueMap[_T("C_VOLTAGE_TYPE")];
			}
		}
		CDCCableCreationSettingTable& DCVoltageSettingTable = CDCCableCreationSettingTable::GetInstance();
		{
			for(vector<CELOADTable::Record>::iterator itr = DCVoltageSettingTable.m_RecordEntry.begin();itr != DCVoltageSettingTable.m_RecordEntry.end();++itr)
			{
				if(itr->FieldValueMap[_T("C_BUS_VOLTAGE")].empty()) continue;

				BUSVOLT_VOLTTYPE_RECORD record;
				record.rBusVolt = itr->FieldValueMap[_T("C_BUS_VOLTAGE")];
				record.rVoltType = itr->FieldValueMap[_T("C_VOLTAGE_TYPE")];
				record.rPhase = _T("DC");
				m_BusVolt_VoltTypeEntry.push_back(record);
				//m_BusVolt_VoltTypeMap[itr->FieldValueMap[_T("C_BUS_VOLTAGE")]] = itr->FieldValueMap[_T("C_VOLTAGE_TYPE")];
			}
		}

		if(0 == m_SelectionSet.GetItemCount())
		{
			list<CELoadItem*> BusItemList;
			docData.GetELoadItemListOf(BusItemList , CBusItem::TypeString(), _T("*"));
			if(!BusItemList.empty())
			{
				for(list<CELoadItem*>::iterator itr = BusItemList.begin();itr != BusItemList.end();++itr)
				{
					if((*itr)->IsDeleted()) continue;

					CItemSelectionSet SelectionSet;
					const string rBusID = (*itr)->prop()->GetValue(_T("General") , _T("Bus ID"));
					docData.GetLoadItemSetInBus(SelectionSet , rBusID);
					for(int i = 0;i < SelectionSet.GetItemCount();++i)
					{
						CLoadItem* pLoadItem = static_cast<CLoadItem*>(SelectionSet.GetItemAt(i));
						if(pLoadItem)
						{
							if(FALSE == m_bAllLoads)
							{
								const string rMCCB_AF = pLoadItem->prop()->GetValue(_T("MCCB") , _T("AF"));
								const string rMCCB_AT = pLoadItem->prop()->GetValue(_T("MCCB") , _T("AT"));
								if(!rMCCB_AF.empty() && !rMCCB_AT.empty()) continue;
							}
							ExecuteOnLoadItem(bSetOriginalValue , pLoadItem , pTable);
						}
					}
				}

				return ERROR_SUCCESS;
			}
		}
		else
		{
			for(int i = 0;i < m_SelectionSet.GetItemCount();++i)
			{
				CLoadItem* pLoadItem = static_cast<CLoadItem*>(m_SelectionSet.GetItemAt(i));
				if(pLoadItem)
				{
					if(FALSE == m_bAllLoads)
					{
						const string rMCCB_AF = pLoadItem->prop()->GetValue(_T("MCCB") , _T("AF"));
						const string rMCCB_AT = pLoadItem->prop()->GetValue(_T("MCCB") , _T("AT"));
						if(!rMCCB_AF.empty() && !rMCCB_AT.empty()) continue;
					}
					ExecuteOnLoadItem(bSetOriginalValue , pLoadItem , pTable);
				}
			}
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-14 오후 7:02:33	

	@param	

	@return		
*/
int COMMAND::CCommandAutoFillMCCBProp::ExecuteOnLoadItem(const bool& bSetOriginalValue, ITEMS::CLoadItem* pLoadItem,TABLES::CMCCBSizeAutoSelectionFunctionTable* pTable)
{
	assert(pLoadItem && pTable && "pLoadItem or pTable is NULL");

	if(pLoadItem && pTable)
	{
		if(pLoadItem->GetName() == _T("FA-201"))
		{
			string rLoadid = "1";
		}
		CELoadDocData& docData = CELoadDocData::GetInstance();

		const string rBusID = pLoadItem->GetBusId();
		CBusItem* pBusItem = docData.FindBusItemByName(rBusID);
		if(NULL == pBusItem)
		{
			ClearMCCBProp(bSetOriginalValue,pLoadItem);
			return ERROR_BAD_ENVIRONMENT;
		}
		const string rBusVolt = pBusItem->prop()->GetValue(_T("General") , _T("Bus Voltage"));
		const string rPhase = pBusItem->prop()->GetValue(_T("General") , _T("Phase"));
		/*map<string,string>::iterator where = m_BusVolt_VoltTypeMap.find(rBusVolt);
		if(where == m_BusVolt_VoltTypeMap.end())
		{
			ClearMCCBProp(bSetOriginalValue,pLoadItem);
			return ERROR_BAD_ENVIRONMENT;
		}*/
		
		string rVoltTypeTemp;
		bool bFindBusVolt = false;
		for(vector<BUSVOLT_VOLTTYPE_RECORD>::iterator itr = m_BusVolt_VoltTypeEntry.begin(); itr != m_BusVolt_VoltTypeEntry.end();++itr)
		{
			if((itr->rBusVolt == rBusVolt) && (itr->rPhase == rPhase))
			{
				bFindBusVolt = true;
				rVoltTypeTemp = itr->rVoltType;
			}
		}
		if(!bFindBusVolt)
		{
			ClearMCCBProp(bSetOriginalValue,pLoadItem);
			return ERROR_BAD_ENVIRONMENT;
		}

		const string rVoltType = rVoltTypeTemp;

		{
			CELOADTable::Record record;
			if(ERROR_SUCCESS == pTable->GetRecordMeetsVoltage(record , pLoadItem , rVoltType))
			{
				pLoadItem->prop()->SetValue(_T("MCCB") , _T("AF") , record.FieldValueMap[_T("C_MCCB_AF")]);
				pLoadItem->prop()->SetValue(_T("MCCB") , _T("AT") , record.FieldValueMap[_T("C_MCCB_AT")]);
				if(bSetOriginalValue)
				{
					pLoadItem->prop()->SetOriginalValue(_T("MCCB") , _T("AF") , record.FieldValueMap[_T("C_MCCB_AF")]);
					pLoadItem->prop()->SetOriginalValue(_T("MCCB") , _T("AT") , record.FieldValueMap[_T("C_MCCB_AT")]);
				}
			}else	ClearMCCBProp(bSetOriginalValue,pLoadItem);
		}
		
		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-14 오후 7:08:30	

	@param	

	@return		
*/
int COMMAND::CCommandAutoFillMCCBProp::ClearMCCBProp(const bool& bSetOriginalValue,CLoadItem* pLoadItem)
{
	assert(pLoadItem && "pLoadItem is NULL");

	if(pLoadItem)
	{
		pLoadItem->prop()->SetValue(_T("MCCB") , _T("AF") , _T(""));
		pLoadItem->prop()->SetValue(_T("MCCB") , _T("AT") , _T(""));
		if(bSetOriginalValue)
		{
			pLoadItem->prop()->SetOriginalValue(_T("MCCB") , _T("AF") , _T(""));
			pLoadItem->prop()->SetOriginalValue(_T("MCCB") , _T("AT") , _T(""));
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}
