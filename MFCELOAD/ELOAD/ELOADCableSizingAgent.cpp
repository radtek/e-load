#include "StdAfx.h"
#include "ELoadDocData.h"
#include "ELOADCableSizingAgent.h"
#include "Tables/CableSizeSettingTable.h"

CELOADCableSizingAgent::CELOADCableSizingAgent(void)
{
}

CELOADCableSizingAgent::~CELOADCableSizingAgent(void)
{
}

/**
	@brief	load cable data table.

	@author	BHK
*/
int CELOADCableSizingAgent::LoadCableDataTableIfNeeded()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	TABLES::CELOADTable* pTable = docData.GetTableOf(CProjectSettingTable::GetTableName());
	const CString rDataName = pTable->m_RecordEntry[0].FieldValueMap[_T("C_CUR_CABLE_SIZING_SETTING_TABLE_NAME")].c_str();

	CCableSizeSettingTable& CableSizeSettingTable = CCableSizeSettingTable::GetInstance();
	CableSizeSettingTable.Load();

	if(docData.m_ProjectCableDataTable.GetTableName() != CableSizeSettingTable.m_RecordMap[rDataName].m_rCableDataTable)
	{
		docData.m_ProjectCableDataTable.Load(CableSizeSettingTable.m_RecordMap[rDataName].m_rCableDataTable, 
			docData.m_ProjectCableDataTable.GetTableName() != CableSizeSettingTable.m_RecordMap[rDataName].m_rCableDataTable);

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	현재 프로젝트 헤르쯔에 맞는 CableType 리스트를 구한다.

	@author	BHK
*/
int CELOADCableSizingAgent::GetCableTypeList(vector<string>& CableTypeList)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	
	LoadCableDataTableIfNeeded();

	vector<string> TempCableTypeList;
	docData.m_ProjectCableDataTable.GetCableTypeKeyList(TempCableTypeList);
	for(vector<string>::iterator itr = TempCableTypeList.begin();itr != TempCableTypeList.end();++itr)
	{
		CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(*itr);
		if(pCableDataByType)
		{
			vector<CCableDataByType::Record*>* pRecordOfHertz = pCableDataByType->GetRecordEntryOfHertz();
			if(NULL != pRecordOfHertz) CableTypeList.push_back(*itr);
		}
	}

	return ERROR_SUCCESS;
}
