#include "StdAfx.h"
#include "SchematicControlTypeMapTable.h"

using namespace TABLES;
CSchematicControlTypeMapTable::CSchematicControlTypeMapTable()
{
	m_rTableName = _T("T_SCHEMATIC_CONTROL_TYPE_MAP");
}

CSchematicControlTypeMapTable::~CSchematicControlTypeMapTable()
{
}

/**
	@brief	

	@author BHK	

	@date 2009-04-14 오후 4:57:13	

	@param	

	@return		
*/
const string CSchematicControlTypeMapTable::GetTableName()
{
	static const string __TABLE_NAME__ = _T("T_SCHEMATIC_CONTROL_TYPE_MAP");

	return __TABLE_NAME__;
}

/**
	@brief	control type을 구한다.

	@author BHK	

	@date 2009-04-16 오후 2:03:45	

	@param	

	@return		
*/
int CSchematicControlTypeMapTable::GetControlTypeListOf(vector<string>& ControlTypeList , const string& rSchematicType)
{
	ControlTypeList.clear();
	for(vector<CELOADTable::Record>::iterator itr = m_RecordEntry.begin(); itr != m_RecordEntry.end();++itr)
	{
		const string _rSchematicType = itr->FieldValueMap[_T("C_SCHEM_TYPE")];
		if(_rSchematicType == rSchematicType)
		{
			const string rControlType = itr->FieldValueMap[_T("C_CONTROL_TYPE")];
			ControlTypeList.push_back(rControlType);
		}
	}
	
	return ERROR_SUCCESS;
}
