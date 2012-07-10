#include "StdAfx.h"
#include "..\ELoadDocData.h"
#include "..\pyfromc.h"
#include "CommandPowerCableCreation.h"
#include "CommandAutoCalculateVD.h"

using namespace COMMAND;

CCommandAutoCalculateVD::CCommandAutoCalculateVD(void) : CEventData("" , CEventData::TYPE_NULL)
{
}

CCommandAutoCalculateVD::~CCommandAutoCalculateVD(void)
{
}

/**
	@brief	voltage drop을 계산한다.

	@author BHK	

	@date 2009-04-13 오후 1:44:52	

	@param	

	@return		
*/
int CCommandAutoCalculateVD::Execute(const bool& bSetOriginalValue)
{
	const int nItemCount = m_SelectionSet.GetItemCount();
	for(int i = 0;i < nItemCount;++i)
	{
		CELoadItem* pItem = m_SelectionSet.GetItemAt(i);
		if(pItem)
		{
			const string rCableVolt = pItem->prop()->GetValue(_T("General") , _T("Cable Volt"));
			const string rCableType = pItem->prop()->GetValue(_T("General") , _T("Cable Type"));
			const string rCableCore = pItem->prop()->GetValue(_T("General") , _T("Core"));

			const string rType = (rCableVolt + _T("KV ") + rCableCore + _T(" CU ") + rCableType + _T(" (eLoad)"));

			CELoadDocData& docData = CELoadDocData::GetInstance();
			TABLES::CELOADTable* pTable = docData.GetTableOf(CProjectSettingTable::GetTableName());
			const CString rDataName = pTable->m_RecordEntry[0].FieldValueMap[_T("C_CUR_CABLE_SIZING_SETTING_TABLE_NAME")].c_str();

			CCableSizeSettingTable& CableSizeSettingTable = CCableSizeSettingTable::GetInstance();
			CableSizeSettingTable.Load();

			docData.m_ProjectCableDataTable.Load(CableSizeSettingTable.m_RecordMap[rDataName].m_rCableDataTable);
			TABLES::CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(rType);
			if(pCableDataByType)
			{
				const string rCableSize = pItem->prop()->GetValue(_T("General") , _T("Size"));
				vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
				for(vector<CCableDataByType::Record*>::iterator itr = pRecordList->begin();itr != pRecordList->end();++itr)
				{
					if(rCableSize == (*itr)->m_FieldMap[_T("C_SIZE")])
					{
						const string rLoadID = pItem->prop()->GetValue(_T("General") , _T("Load Id"));
						CLoadItem* pLoadItem = docData.FindLoadItemByName(rLoadID);
						if(pLoadItem)
						{
							const string rBusID = pLoadItem->prop()->GetValue(_T("Panel") , _T("Bus ID"));
							const double nPF    = CCommandPowerCableCreation::GetPowerFactor(pLoadItem);
							const double nFLC   = pLoadItem->GetFLCForPowerCableCreation();///SAFE_ROUND(atof(pLoadItem->prop()->GetValue(_T("Rating") , _T("FLC")).c_str()) , 3);
							const double nSF    = atof(pLoadItem->prop()->GetValue(_T("Voltage Drop") , _T("SF For FLC")).c_str());

							CBusItem* pBusItem = docData.FindBusItemByName(rBusID);
							const double nBusVolt = atof(pBusItem->prop()->GetValue(_T("General") , _T("Bus Voltage")).c_str());
							const double nRunVoltageDrop = atof(CCommandPowerCableCreation::GetRunVoltageDrop(pLoadItem).c_str());

							m_rR = (*itr)->m_FieldMap[_T("C_R")];
							pItem->prop()->SetValue(_T("Cable Spec") , _T("R"),m_rR);
							m_rX = (*itr)->m_FieldMap[_T("C_X")];
							pItem->prop()->SetValue(_T("Cable Spec") , _T("X"),m_rX);
							m_rCableDia = (*itr)->m_FieldMap[_T("C_DIA")];
							pItem->prop()->SetValue(_T("Cable Spec") , _T("Cable OD"),m_rCableDia);
							m_rCableWeight = (*itr)->m_FieldMap[_T("C_CABLE_WEIGHT")];
							pItem->prop()->SetValue(_T("Cable Spec") , _T("Weight"),m_rCableWeight);

							const int nMultiply = atoi(pItem->prop()->GetValue(_T("General") , _T("No Of Cables/Phase")).c_str());
							const string rRunningVD = pItem->prop()->GetValue(_T("General") , _T("Running VD"));
							if(!rRunningVD.empty())
							{
								double nRunningVD= 0.f;
								const string rID		= pLoadItem->GetName();
								const string rPhase		= pLoadItem->prop()->GetValue("Rating" , "Phase");
								///const string rCategory	= pLoadItem->prop()->GetValue("Type" , "Load Category");
								const string rL			= pLoadItem->prop()->GetValue("Cable Route"  , "Design Length");
								python.CallPyFindPowerCableSize(rID , rPhase , rL ,
									m_rR , m_rX , nPF , (nFLC*nSF) , (nBusVolt*nRunVoltageDrop*0.01*1000) , nMultiply , nRunningVD);
								ostringstream oss;
								oss << SAFE_ROUND(nRunningVD,2);
								pItem->prop()->SetValue(_T("General") , _T("Running VD"),oss.str());
								oss.str(""); oss << SAFE_ROUND((nRunningVD/(nBusVolt*1000)*100),2);
								pItem->prop()->SetValue(_T("General") , _T("Running VD%"),oss.str());
							}

							const string rStartingVD = pItem->prop()->GetValue(_T("General") , _T("Starting VD"));
							if(!rStartingVD.empty())
							{
								double nStartingVD= 0.f;
								const double nPFatStarting = atof(pLoadItem->prop()->GetValue(_T("Characteristic") , _T("PF at Starting")).c_str());
								const double nLRC = 0.01*atof(pLoadItem->prop()->GetValue(_T("Characteristic") , _T("LRC")).c_str());
								const double nStartVoltageDrop = atof(pLoadItem->prop()->GetValue(_T("Voltage Drop") , _T("Motor Start VD")).c_str());

								const string rID		= pLoadItem->GetName();
								const string rPhase		= pLoadItem->prop()->GetValue("Rating" , "Phase");
								///const string rCategory	= pLoadItem->prop()->GetValue("Type" , "Load Category");
								const string rL			= pLoadItem->prop()->GetValue("Cable Route"  , "Design Length");
								python.CallPyFindPowerCableSize(rID , rPhase , rL , 
									m_rR , m_rX , nPFatStarting , (nFLC*nLRC) , (nBusVolt*nStartVoltageDrop*0.01*1000) , nMultiply , nStartingVD);

								ostringstream oss;
								oss << SAFE_ROUND(nStartingVD,2);
								pItem->prop()->SetValue(_T("General") , _T("Starting VD") , oss.str());
								oss.str(""); oss << SAFE_ROUND((nStartingVD/(nBusVolt*1000)*100),2);
								pItem->prop()->SetValue(_T("General") , _T("Starting VD%") , oss.str());
							}
						}

						break;
					}
				}
			}
			else
			{
				AfxMessageBox(_T("Cable Data Table에서 알맞은 데이터를 찾을수 없습니다."));
			}
		}
	}

	return ERROR_SUCCESS;
}