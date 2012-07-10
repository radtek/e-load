#include "StdAfx.h"
#include "..\ELoadDocData.h"
#include "CommandAutoFillMCCBProp.h"
#include "CommandAutoChangeLoadVoltOfBus.h"
#include "../Tables/CableCreationSettingTable.h"
#include "CommandAutoFillLoadVoltage.h"
#include "CommandAutoFillPhase.h"

using namespace COMMAND;
CCommandAutoChangeLoadVoltOfBus::CCommandAutoChangeLoadVoltOfBus(void) : CEventData("" , CEventData::TYPE_NULL)
{
}

CCommandAutoChangeLoadVoltOfBus::~CCommandAutoChangeLoadVoltOfBus(void)
{
}

struct MatchBusVoltage : binary_function<CCableCreationTable::Record , string , bool>
{
	bool operator()(const CCableCreationTable::Record& lhs , const string& rhs) const
	{
		map<string,string>::const_iterator where = lhs.FieldValueMap.find(_T("C_BUS_VOLTAGE"));
		return ((where != lhs.FieldValueMap.end()) && (where->second == rhs));
	}
};

/**
	@brief	

	@author BHK	

	@date 2009-04-14 ¿ÀÈÄ 7:42:55	

	@param	

	@return		
*/
int CCommandAutoChangeLoadVoltOfBus::Execute(const bool& bSetOriginalValue)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	for(int i = 0;i < m_SelectionSet.GetItemCount();++i)
	{
		CELoadItem* pItem = m_SelectionSet.GetItemAt(i);
		if(pItem && (pItem->IsKindOf(CBusItem::TypeString())) || (pItem->IsKindOf(CBusGroupItem::TypeString())))
		{
			const string rBusID = pItem->prop()->GetValue(_T("General"), _T("Bus ID"));
			const string rBusVolt = pItem->prop()->GetValue(_T("General") , _T("Bus Voltage"));
			{
				CHVCableCreationSettingTable& HVVoltageSettingTable = CHVCableCreationSettingTable::GetInstance();
				vector<CCableCreationTable::Record>::iterator where = find_if(HVVoltageSettingTable.m_RecordEntry.begin() , HVVoltageSettingTable.m_RecordEntry.end() , bind2nd(MatchBusVoltage() , rBusVolt));
				if(where != HVVoltageSettingTable.m_RecordEntry.end())
				{
					const string rLoadVolt = where->FieldValueMap[_T("C_LOAD_VOLTAGE")];
					pItem->prop()->SetValue(_T("General") , _T("Load Voltage") , rLoadVolt);

					const string rPhase = where->FieldValueMap[_T("C_PHASE")];
					pItem->prop()->SetValue(_T("General"), _T("Phase"), rPhase);
					continue;
				}
			}

			{
				CLV3PHCableCreationSettingTable& LV3PHVoltageSettingTable = CLV3PHCableCreationSettingTable::GetInstance();
				vector<CCableCreationTable::Record>::iterator where = find_if(LV3PHVoltageSettingTable.m_RecordEntry.begin() , LV3PHVoltageSettingTable.m_RecordEntry.end() , bind2nd(MatchBusVoltage() , rBusVolt));
				if(where != LV3PHVoltageSettingTable.m_RecordEntry.end())
				{
					const string rLoadVolt = where->FieldValueMap[_T("C_LOAD_VOLTAGE")];
					pItem->prop()->SetValue(_T("General") , _T("Load Voltage") , rLoadVolt);

					const string rPhase = where->FieldValueMap[_T("C_PHASE")];
					pItem->prop()->SetValue(_T("General"), _T("Phase"), rPhase);
					continue;
				}
			}

			{
				CLV1PHCableCreationSettingTable& LV1PHVoltageSettingTable = CLV1PHCableCreationSettingTable::GetInstance();
				vector<CCableCreationTable::Record>::iterator where = find_if(LV1PHVoltageSettingTable.m_RecordEntry.begin() , LV1PHVoltageSettingTable.m_RecordEntry.end() , bind2nd(MatchBusVoltage() , rBusVolt));
				if(where != LV1PHVoltageSettingTable.m_RecordEntry.end())
				{
					const string rLoadVolt = where->FieldValueMap[_T("C_LOAD_VOLTAGE")];
					pItem->prop()->SetValue(_T("General") , _T("Load Voltage") , rLoadVolt);

					const string rPhase = where->FieldValueMap[_T("C_PHASE")];
					pItem->prop()->SetValue(_T("General"), _T("Phase"), rPhase);
					continue;
				}
			}

			{
				CDCCableCreationSettingTable& DCVoltageSettingTable = CDCCableCreationSettingTable::GetInstance();
				vector<CCableCreationTable::Record>::iterator where = find_if(DCVoltageSettingTable.m_RecordEntry.begin() , DCVoltageSettingTable.m_RecordEntry.end() , bind2nd(MatchBusVoltage() , rBusVolt));
				if(where != DCVoltageSettingTable.m_RecordEntry.end())
				{
					const string rLoadVolt = where->FieldValueMap[_T("C_LOAD_VOLTAGE")];
					pItem->prop()->SetValue(_T("General") , _T("Load Voltage") , rLoadVolt);

					const string rPhase = where->FieldValueMap[_T("C_PHASE")];
					pItem->prop()->SetValue(_T("General"), _T("Phase"), rPhase);
					continue;
				}
			}
		}
	}

	///
	CCommandAutoFillLoadVoltage cmdAutoFillLoadVoltage;
	cmdAutoFillLoadVoltage.m_SelectionSet = m_SelectionSet;
	cmdAutoFillLoadVoltage.Execute();

	CCommandAutoFillPhase cmdAutoFillPhase;
	cmdAutoFillPhase.m_SelectionSet = m_SelectionSet;
	cmdAutoFillPhase.Execute();

	return ERROR_SUCCESS;
}
