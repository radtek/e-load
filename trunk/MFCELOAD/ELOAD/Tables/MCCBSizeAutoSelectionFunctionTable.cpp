#include "StdAfx.h"
#include "MCCBSizeAutoSelectionFunctionTable.h"

using namespace TABLES;
CMCCBSizeAutoSelectionFunctionTable::CMCCBSizeAutoSelectionFunctionTable(void)
{
}

CMCCBSizeAutoSelectionFunctionTable::~CMCCBSizeAutoSelectionFunctionTable(void)
{
}

/**
	@brief	

	@author BHK	

	@date 2009-04-14 오후 4:57:13	

	@param	

	@return		
*/
const string CMCCBSizeAutoSelectionFunctionTable::GetTableName()
{
	static const string __TABLE_NAME__ = _T("T_MCCB_SIZE_AUTO_SELECTION_FUNCTION");

	return __TABLE_NAME__;
}

/**
	@brief	Rating Capacity가 MIN CAPACITY와 MAX CAPACITY 안에 속하는지를 검사한다.
	만일 Rating Capacity가 비어 있으면 ERROR_BAD_ENVIRONMENT를 리턴한다.

	@author BHK	

	@date 2009-04-14 오후 4:58:17	

	@param	

	@return		
*/
int TABLES::CMCCBSizeAutoSelectionFunctionTable::GetRecordMeetsVoltage(CELOADTable::Record& record , ITEMS::CLoadItem* pLoadItem , const string& rVoltType)
{
	assert(pLoadItem && "pLoadItem is NULL");

	if(pLoadItem)
	{
		const string rRatingCapacity = pLoadItem->prop()->GetValue(_T("Rating") , _T("Rating Capacity"));
		if(rRatingCapacity.empty()) return ERROR_BAD_ENVIRONMENT;
		
		const string rLoadCategory = pLoadItem->prop()->GetValue(_T("Type") , _T("Load Category"));

		const double nRatingCapacity = atof(rRatingCapacity.c_str());
		for(vector<Record>::iterator itr = m_RecordEntry.begin();itr != m_RecordEntry.end();++itr)
		{
			if((_T("MOTOR") == rLoadCategory) && (_T("MOTOR")  != itr->FieldValueMap[_T("C_LOAD_TYPE")])) continue;
			if((_T("MOTOR") != rLoadCategory) && (_T("FEEDER") != itr->FieldValueMap[_T("C_LOAD_TYPE")])) continue;

			if(rVoltType == itr->FieldValueMap[_T("C_VOLTAGE_TYPE")])
			{
				const double nMinCapacity = atof(itr->FieldValueMap[_T("C_MIN_CAPACITY")].c_str());
				const double nMaxCapacity = atof(itr->FieldValueMap[_T("C_MAX_CAPACITY")].c_str());

				if((nMinCapacity - nRatingCapacity)*(nMaxCapacity - nRatingCapacity) <= 0)
				{
					record = (*itr);
					return ERROR_SUCCESS;
				}
			}
		}

		return ERROR_BAD_ENVIRONMENT;
	}

	return ERROR_BAD_ENVIRONMENT;
}
