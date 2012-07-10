#include "StdAfx.h"
#include "ControlTypeTable.h"

using namespace TABLES;
CControlTypeTable::CControlTypeTable()
{
	m_rTableName = _T("T_CONTROL_TYPE");
}

CControlTypeTable::~CControlTypeTable()
{
}

/**
	@brief	

	@author BHK	

	@date 2009-04-14 오후 4:57:13	

	@param	

	@return		
*/
const string CControlTypeTable::GetTableName()
{
	static const string __TABLE_NAME__ = _T("T_CONTROL_TYPE");

	return __TABLE_NAME__;
}

/**
	@brief	control type을 구한다.

	@author BHK	

	@date 2009-04-16 오후 2:03:45	

	@param	

	@return		
*/
int CControlTypeTable::GetControlTypeList(CStringArray& options)
{
	options.RemoveAll();
	for(vector<CELOADTable::Record>::iterator itr = m_RecordEntry.begin(); itr != m_RecordEntry.end();++itr)
	{
		if(_T("YES") == itr->FieldValueMap[_T("C_CHECK")])
		{
			const CString rType = itr->FieldValueMap[_T("C_TYPE")].c_str();
			options.Add(rType);
		}
	}
	
	return ERROR_SUCCESS;
}