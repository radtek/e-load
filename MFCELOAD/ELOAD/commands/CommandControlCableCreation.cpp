#include "StdAfx.h"
#include "..\ELoad.h"
#include "../ELoadDocData.h"
#include "..\WorkStatusDlg.h"
#include "CommandControlCableCreation.h"

using namespace COMMAND;

bool MatchControlCableData(CELOADTable::Record lhs , CELOADTable::Record rhs)
{
	const string rSpec1  = lhs.FieldValueMap[_T("C_SPEC")];
	const string rCore1  = lhs.FieldValueMap[_T("C_CORE")];
	const string rSize1  = lhs.FieldValueMap[_T("C_SIZE")];

	const string rSpec2  = rhs.FieldValueMap[_T("C_SPEC")];
	const string rCore2  = rhs.FieldValueMap[_T("C_CORE")];
	const string rSize2  = rhs.FieldValueMap[_T("C_SIZE")];

	return ((rSpec1 == rSpec2) && (rCore1 == rCore2) && (rSize1 == rSize2));
}

CCommandControlCableCreation::CCommandControlCableCreation() : m_nDCSDesignLength(0.f)
{
}

CCommandControlCableCreation::~CCommandControlCableCreation(void)
{
}

/**
	@brief	

	@author BHK	

	@date 2009-04-29 오후 2:12:14	

	@param	

	@return		
*/
int CCommandControlCableCreation::Execute(const bool& bExecuteSelectedLoad)
{
	m_bExecuteSelectedLoad = bExecuteSelectedLoad;
	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
	if(NULL == pDlg)
	{
		CWorkStatusDlg dlg;
		dlg.m_pThread = AfxBeginThread(CCommandControlCableCreation::StatusThreadEntry , this , THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
		if(NULL != dlg.m_pThread)
		{
			InterlockedExchange((LONG*)(&(dlg.m_bThreadRunning)) , TRUE);
			dlg.DoModal();	

			return ERROR_SUCCESS;
		}
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-29 오후 2:15:16	

	@param	

	@return		
*/
UINT CCommandControlCableCreation::StatusThreadEntry(LPVOID pVoid)
{
	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
	if(pDlg)
	{
		CCommandControlCableCreation* p = (CCommandControlCableCreation*)pVoid;
		p->StatusThread();

		InterlockedExchange((LONG*)(&(pDlg->m_bThreadRunning)) , FALSE);
		if(pDlg) pDlg->PostMessage(WM_COMMAND , IDOK);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	옵션에 따라 필요한 Load Item을 구한다.

	@author BHK	

	@date 2009-07-09 오후 2:32:55	

	@param	

	@return		
*/
int CCommandControlCableCreation::GetInterestingLoadItemListAndOption()
{
	m_nDCSDesignLength = 0.f;

	CELoadDocData& docData = CELoadDocData::GetInstance();
	m_nLoadControlCableRadio = CControlCableResultDoc::ALL_CONTROL_CABLE;
	CProjectSettingTable* pSettingTable = (CProjectSettingTable*)docData.GetTableOf(CProjectSettingTable::GetTableName());
	if(pSettingTable)
	{
		string rControlCableRadio = _T("0");
		map<string,string>::iterator where = pSettingTable->m_RecordEntry[0].FieldValueMap.find(_T("C_LOAD_CONTROL_CABLE_RADIO"));
		if(where != pSettingTable->m_RecordEntry[0].FieldValueMap.end()) m_nLoadControlCableRadio = atoi(where->second.c_str());

		where = pSettingTable->m_RecordEntry[0].FieldValueMap.find(_T("C_DCS_IO_TYPE_DESIGN_LENGTH"));
		if(where != pSettingTable->m_RecordEntry[0].FieldValueMap.end()) m_nDCSDesignLength = atof(where->second.c_str());
	}

	if(false == m_bExecuteSelectedLoad)
	{
		if(CControlCableResultDoc::ALL_CONTROL_CABLE == m_nLoadControlCableRadio)
		{
			docData.GetELoadItemListOf(m_InterestingItemList , CLoadItem::TypeString());
		}
		else if(CControlCableResultDoc::UNSIZED_CONTROL_CABLE_ONLY == m_nLoadControlCableRadio)
		{
			list<CELoadItem*> LoadItemList;
			docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
			for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
			{
				CLoadItem* pLoadItem = static_cast<CLoadItem*>(*itr);
				const int nCableCount = pLoadItem->GetCableCountOf(_T("Control Cable"));
				if(0 == nCableCount) m_InterestingItemList.push_back(*itr);
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK,KHS
*/
int CCommandControlCableCreation::StatusThread(void)
{
	double progress = 0.;
	int nIndex = 0;

	CELoadDocData& docData = CELoadDocData::GetInstance();
	GetInterestingLoadItemListAndOption();

	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();

	CString rTitle = _T("Create Control Cable...");
	
	CELOADTable* pMapTable = docData.GetTableOf(_T("T_SCHEMATIC_CONTROL_CABLE_MAP"));
	if(pMapTable)
	{
		const int nTotalCount = count_if(m_InterestingItemList.begin() , m_InterestingItemList.end() , CELoadItem::IsNotDeleted);
		m_nTryCount = 0;m_nSuccessCount = 0;m_nFailCount = 0;
		for(list<CELoadItem*>::iterator itr = m_InterestingItemList.begin(); itr != m_InterestingItemList.end();++itr)
		{
			CLoadItem* pLoadItem = static_cast<CLoadItem*>(*itr);
			if(pLoadItem->IsDeleted()) continue;
			
			//! UNSIZED_CONTROL_CABLE_ONLY일 경우에는 Control Cable이 하나도 생성되지 않는 Load에 대해서만 Cable을 생성한다.
			///if((CControlCableResultDoc::UNSIZED_CONTROL_CABLE_ONLY == m_nLoadControlCableRadio) && (0 != pLoadItem->GetCableCountOf(_T("Control Cable")))) continue;
			
			//! load에 속한 control cable을 삭제한다.
			///if(CControlCableResultDoc::ALL_CONTROL_CABLE == m_nLoadControlCableRadio) pLoadItem->ClearCableItems(_T("Control Cable"), _T("Auto"));
			pLoadItem->ClearCableItems(_T("Control Cable"), _T("Auto"));
			
			//! Cable을 생성할 Load를 리스트에 넣는다.
			CControlCableResultDoc::m_InterestingLoadItemList.push_back(pLoadItem);

			++m_nTryCount;
			const string rName = pLoadItem->GetName();
			const string rSchematicType   = pLoadItem->prop()->GetValue(_T("Control Cable") , _T("Schematic Ty"));
			if(!rSchematicType.empty())
			{
				for(vector<CELOADTable::Record>::iterator itr = pMapTable->m_RecordEntry.begin(); itr != pMapTable->m_RecordEntry.end();++itr)
				{
					if(rSchematicType == itr->FieldValueMap[_T("C_SCHEM_TYPE")])
					{
						CCableItem* pCableItem = static_cast<CCableItem*>(docData.CreateELoadItemOf(CCableItem::TypeString()));
						if(pCableItem)
						{
							SetCableProp(pCableItem , pLoadItem , *itr);
							pLoadItem->m_CableRefEntry.push_back(pCableItem);
						}
					}
				}
				
				//! CONTROL CABLE이 없으면 ERROR
				/*
				if(0 == pLoadItem->GetCableCountOf("Control Cable"))
				{
					++m_nFailCount;
					ELOAD_LOG4CXX_WARN(mylogger , rName , 26);
				}
				else
				{
				*/
					++m_nSuccessCount;
				///}
			}
			else
			{
				++m_nFailCount;
				ELOAD_LOG4CXX_WARN(mylogger , rName , 25);
			}
			int progress = int((double(nIndex) / double(nTotalCount))*100.);
			if(pDlg) pDlg->UpdateWorkStatus(rTitle , progress);
			++nIndex;
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	Schematic Type이 

	@author BHK	

	@date 2009-04-29 오후 2:50:44	

	@param	

	@return		
*/
string CCommandControlCableCreation::GetControlTypeOf(CELOADTable::Record& record)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pTable = docData.GetTableOf(_T("T_CONTROL_CABLE_CONTROL_TYPE_MAP"));
	if(pTable)
	{
		for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin();itr != pTable->m_RecordEntry.end();++itr)
		{
			if(	(record.FieldValueMap[_T("C_SCHEM_TYPE")] == itr->FieldValueMap[_T("C_SCHEM_TYPE")]) && 
				(record.FieldValueMap[_T("C_CABLE_TYPE")] == itr->FieldValueMap[_T("C_CABLE_TYPE")]) &&
				(record.FieldValueMap[_T("C_CORE")] == itr->FieldValueMap[_T("C_CORE")]) &&
				(record.FieldValueMap[_T("C_SIZE")] == itr->FieldValueMap[_T("C_SIZE")]) &&
				(record.FieldValueMap[_T("C_NAMING_RULE_PREFIX")] == itr->FieldValueMap[_T("C_NAMING_RULE_PREFIX")]) &&
				(record.FieldValueMap[_T("C_NAMING_RULE_SUFFIX")] == itr->FieldValueMap[_T("C_NAMING_RULE_SUFFIX")]))
			{
				const string rControlType = itr->FieldValueMap[_T("C_CONTROL_TYPE")];
				CELOADTable* pControlTypeTable = docData.GetTableOf(_T("T_CONTROL_TYPE"));
				if(pControlTypeTable)
				{
					for(vector<CELOADTable::Record>::iterator jtr = pControlTypeTable->m_RecordEntry.begin();jtr != pControlTypeTable->m_RecordEntry.end();++jtr)
					{
						if(rControlType == jtr->FieldValueMap[_T("C_TYPE")])
						{
							return (jtr->FieldValueMap[_T("C_CATEGORY")]);
						}
					}
				}
			}
		}
	}

	return _T("");
}

//! Cable Item을 LOAD,CABLE 순에 의거하여 SORTING한다.
static bool SortCableItemFunc(CELoadItem* lhs , CELoadItem* rhs)
{
	CCableItem* _lhs = static_cast<CCableItem*>(lhs);
	CCableItem* _rhs = static_cast<CCableItem*>(rhs);

	CLoadItem* pLhsLoadItem = _lhs->GetLoadItemPtr();
	CLoadItem* pRhsLoadItem = _rhs->GetLoadItemPtr();
	if(pLhsLoadItem && pRhsLoadItem)
	{
		const string rLhsLoadId = pLhsLoadItem->GetName();
		const string rRhsLoadId = pRhsLoadItem->GetName();

		const string rLhsCableId = lhs->GetName();
		const string rRhsCableId = rhs->GetName();

		if(rLhsLoadId == rRhsLoadId)
		{
			return (rLhsCableId < rRhsCableId);
		}else	return (rLhsLoadId < rRhsLoadId);
	}

	return false;
}

/**
	@brief	bus 별 Load Id 가 중복되지 않도록 체크

	@author KHS	

	@date 2009-06-16 오후 1:01:38	

	@param	

	@return		
**/
BOOL CCommandControlCableCreation::IsExistLoadId(const CStringArray& LoadArray, const CString& rLoadId)
{
	if(!LoadArray.IsEmpty())
	{
		for(int i=0; i < LoadArray.GetCount();i++)
		{
			const CString rArrayLoadId = LoadArray[i];
			if(rArrayLoadId == rLoadId)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

/**
	@brief	Control Cable의 속성을 반영한다.

	@author BHK	

	@date 2009-07-03 오후 1:30:44	

	@param	

	@return		
*/
int COMMAND::CCommandControlCableCreation::SetCableProp(CCableItem* pCableItem, CLoadItem* pLoadItem , CELOADTable::Record& record)
{
	assert(pCableItem && pLoadItem && "pCableItem or pLoadItem is NULL");

	if(pCableItem && pLoadItem)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();

		CELOADTable* pTable = docData.GetTableOf(_T("T_CONTROL_CABLE_SETTING"));

		const string rBusID  = pLoadItem->prop()->GetValue(_T("Panel") , _T("Bus ID"));
		const string rLoadID = pLoadItem->prop()->GetValue(_T("General") , _T("ITEM ID"));
		const string rArea = pLoadItem->prop()->GetValue(_T("Location") , _T("Area"));

		const string rPrefix    = record.FieldValueMap[_T("C_NAMING_RULE_PREFIX")];
		const string rSuffix    = record.FieldValueMap[_T("C_NAMING_RULE_SUFFIX")];
		pCableItem->prop()->SetValue(_T("General") , _T("CABLE ID") , rPrefix + pLoadItem->GetName() + rSuffix);
		pCableItem->prop()->SetOriginalValue(_T("General") , _T("CABLE ID") , rPrefix + pLoadItem->GetName() + rSuffix);

		pCableItem->prop()->SetValue(_T("General") , _T("Area") , rArea);
		pCableItem->prop()->SetOriginalValue(_T("General") , _T("Area") , rArea);

		pCableItem->prop()->SetValue(_T("General") , _T("Created By"), _T("Auto"));
		pCableItem->prop()->SetOriginalValue(_T("General") , _T("Created By"), _T("Auto"));

		pCableItem->prop()->SetValue(_T("General") , _T("Load Id") , pLoadItem->GetName());
		pCableItem->prop()->SetOriginalValue(_T("General") , _T("Load Id") , pLoadItem->GetName());
		pCableItem->SetLoadItemPtr(pLoadItem);

		pCableItem->prop()->SetValue(_T("General") , _T("Cable Category") , _T("Control Cable"));
		pCableItem->prop()->SetOriginalValue(_T("General") , _T("Cable Category") , _T("Control Cable"));

		const string rCableType = record.FieldValueMap[_T("C_CABLE_TYPE")];
		pCableItem->prop()->SetValue(_T("General") , _T("Cable Spec") , rCableType);
		pCableItem->prop()->SetOriginalValue(_T("General") , _T("Cable Spec") , rCableType);

		const string rCableCore = record.FieldValueMap[_T("C_CORE")];
		pCableItem->prop()->SetValue(_T("General") , _T("Core") , rCableCore);
		pCableItem->prop()->SetOriginalValue(_T("General") , _T("Core") , rCableCore);

		const string rCableSize = record.FieldValueMap[_T("C_SIZE")];
		pCableItem->prop()->SetValue(_T("General") , _T("Size") , rCableSize);
		pCableItem->prop()->SetOriginalValue(_T("General") , _T("Size") , rCableSize);

		const string rCableDesc = record.FieldValueMap[_T("C_DESC")];
		pCableItem->prop()->SetValue(_T("General") , _T("Description") , rCableDesc);
		pCableItem->prop()->SetOriginalValue(_T("General") , _T("Description") , rCableDesc);

		pCableItem->prop()->SetValue(_T("General") , _T("From") , rBusID);
		pCableItem->prop()->SetOriginalValue(_T("General") , _T("From") , rBusID);
		pCableItem->prop()->SetValue(_T("General") , _T("Panel ID") , rBusID);
		pCableItem->prop()->SetOriginalValue(_T("General") , _T("Panel ID") , rBusID);

		//! 2009.11.12
		const string rLoadVolt = pLoadItem->prop()->GetValue(_T("Rating"), _T("Rated Voltage"));
		pCableItem->prop()->SetValue(_T("General") , _T("Load Volt") , rLoadVolt);
		pCableItem->prop()->SetOriginalValue(_T("General") , _T("Load Volt") , rLoadVolt);

		const string rRatingCapacity = pLoadItem->prop()->GetValue(_T("Rating") , _T("Rating Capacity"));
		pCableItem->prop()->SetValue(_T("General") , _T("Rating Capacity") , rRatingCapacity);
		pCableItem->prop()->SetOriginalValue(_T("General") , _T("Rating Capacity") , rRatingCapacity);

		const string rRatingCapacityUnit = pLoadItem->prop()->GetUnitStr(_T("Rating") , _T("Rating Capacity"));
		pCableItem->prop()->SetValue(_T("General") , _T("Rating Capacity Unit") , rRatingCapacityUnit);
		pCableItem->prop()->SetOriginalValue(_T("General") , _T("Rating Capacity Unit") , rRatingCapacityUnit);

		const string rSchematicTyp = pLoadItem->prop()->GetValue(_T("Control Cable") , _T("Schematic Ty"));
		pCableItem->prop()->SetValue(_T("General") , _T("Schematic Ty") , rSchematicTyp);
		pCableItem->prop()->SetOriginalValue(_T("General") , _T("Schematic Ty") , rSchematicTyp);
		//!

		const string rControlType = GetControlTypeOf(record);
		//! DCS IO TYPE을 가지면 DCS LENGTH를 그렇지 않으면 LOAD쪽의 DESIGN LENGTH를 채워 넣는다.
		if(_T("DCS IO TYPE") == rControlType)
		{
			pCableItem->prop()->SetValue(_T("General") , _T("To") , _T("DCS"));
			pCableItem->prop()->SetOriginalValue(_T("General") , _T("To") , _T("DCS"));

			stringstream oss;
			oss << m_nDCSDesignLength;
			pCableItem->prop()->SetValue(_T("General") , _T("Length") , oss.str());
			pCableItem->prop()->SetOriginalValue(_T("General") , _T("Length") , oss.str());
		}
		else
		{
			if(_T("LCP IO TYPE") == CString(rControlType.c_str()).MakeUpper())
			{
				pCableItem->prop()->SetValue(_T("General") , _T("To") , _T("LCP"));
				pCableItem->prop()->SetOriginalValue(_T("General") , _T("To") , _T("LCP"));
			}
			else
			{
				pCableItem->prop()->SetValue(_T("General") , _T("To") , _T("LCS"));
				pCableItem->prop()->SetOriginalValue(_T("General") , _T("To") , _T("LCS"));
			}

			const string rDesignLength = pLoadItem->prop()->GetValue(_T("Cable Route") , _T("Design Length"));
			pCableItem->prop()->SetValue(_T("General") , _T("Length") , rDesignLength);
			pCableItem->prop()->SetOriginalValue(_T("General") , _T("Length") , rDesignLength);
		}

		//! OD , WEIGHT 추가
		if(pTable)
		{
			CELOADTable::Record data;
			data.FieldValueMap[_T("C_SPEC")] = rCableType;
			data.FieldValueMap[_T("C_CORE")] = rCableCore;
			data.FieldValueMap[_T("C_SIZE")] = rCableSize;

			for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin(); itr != pTable->m_RecordEntry.end();++itr)
			{
				if(MatchControlCableData(data, *itr))
				{
					const string rOd = itr->FieldValueMap[_T("C_OD")];

					pCableItem->prop()->SetValue(_T("Cable Spec") , _T("Cable OD") , rOd);
					pCableItem->prop()->SetOriginalValue(_T("Cable Spec") , _T("Cable OD") , rOd);

					const string rWeight = itr->FieldValueMap[_T("C_WEIGHT")];

					pCableItem->prop()->SetValue(_T("Cable Spec") , _T("Weight") , rWeight);
					pCableItem->prop()->SetOriginalValue(_T("Cable Spec") , _T("Weight") , rWeight);
				}
			}
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	return result of execution as string.

	@author BHK	

	@date 2009-07-09 오전 9:43:31	

	@param	

	@return		
*/
string COMMAND::CCommandControlCableCreation::GetResultString(void)
{
	ostringstream oss;
	/*if(CControlCableResultDoc::UNSIZED_CONTROL_CABLE_ONLY == m_nLoadControlCableRadio)
	{
		oss << _T("UN-SIZED TOTAL LOAD (") << m_nTryCount << _T(" SETS)") << std::endl;
		oss << _T("--------------------------------------------------------------------------------------") << std::endl;
		oss << _T("CABLE CREATION LOAD (") << m_nSuccessCount << _T(" SETS)  | NOT-CREATION LOAD (") << m_nFailCount << _T(" SETS)") << std::endl;
		oss << _T("--------------------------------------------------------------------------------------");
	}
	else
	{
		oss << _T("TOTAL LOAD (") << m_nTryCount << _T(" SETS)") << std::endl;
		oss << _T("--------------------------------------------------------------------------------------") << std::endl;
		oss << _T("CABLE CREATION LOAD (") << m_nSuccessCount << _T(" SETS)  | NOT-CREATION LOAD (") << m_nFailCount << _T(" SETS)") << std::endl;
		oss << _T("--------------------------------------------------------------------------------------");
	}*/
	if(CControlCableResultDoc::UNSIZED_CONTROL_CABLE_ONLY == m_nLoadControlCableRadio)
	{
		oss << _T(" UN-SIZED TOTAL LOAD |") << m_nTryCount << _T(" (SETS) |");
		oss << _T(" CABLE CREATION LOAD |") << m_nSuccessCount << _T(" (SETS) | NOT-CREATION LOAD |") << m_nFailCount << _T(" (SETS) |");
	}
	else
	{
		oss << _T("TOTAL LOAD |") << m_nTryCount << _T(" (SETS) |");
		oss << _T("CABLE CREATION LOAD |") << m_nSuccessCount << _T(" (SETS) | NOT-CREATION LOAD |") << m_nFailCount << _T(" (SETS) |");
	}


	return oss.str();
}