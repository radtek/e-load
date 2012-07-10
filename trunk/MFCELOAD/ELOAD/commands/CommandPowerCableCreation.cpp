#include "StdAfx.h"
#include "..\ELoadDocData.h"
#include "..\Tables/CableCreationSettingTable.h"
#include "CommandPowerCableCreation.h"
#include "..\WorkStatusDlg.h"
#include "..\pyfromc.h"
#include "..\ELoad.h"
#include "..\CableSizingResultDoc.h"
#include <assert.h>
using namespace COMMAND;

///
int CCommandPowerCableCreation::m_nCheckingArea = BEFORE;
double CCommandPowerCableCreation::m_nRunningCalculatedVD = 0.f;
CCableLengthDataSheet::ConduitSizeList CCommandPowerCableCreation::m_ConduitSizeList;
double CCommandPowerCableCreation::m_nConduitLength;
///

struct MatchLoadId : public binary_function<ITEMS::CELoadItem*,string,bool>
{
	bool operator()(const ITEMS::CELoadItem* lhs , const string& rhs) const
	{
		const string rLoadId = lhs->prop()->GetValue(_T("General"), _T("Load Id"));
		return (rLoadId == rhs);
	}
};

CCommandPowerCableCreation::CCommandPowerCableCreation(const CCommandPowerCableCreation::POWER_CABLE_OWNER_TYPE nOwnerType , const string& rAreaNameForEquip) : 
 m_nOwnerType(nOwnerType) , m_rAreaNameForEquip(rAreaNameForEquip)
{
	m_nConductors_1 = 0.f;
	m_nConductors_2 = 0.f;
	m_nConductors_Over2 = 0.f;

	CCommandPowerCableCreation::m_nConduitLength = 0.f;
	CCommandPowerCableCreation::m_nRunningCalculatedVD = 0.f;
	CCommandPowerCableCreation::m_nCheckingArea = BEFORE;
}

CCommandPowerCableCreation::~CCommandPowerCableCreation(void)
{
}

/**
	@brief	LoadSummary Thread Function.

	@author	BHK
*/
UINT CCommandPowerCableCreation::StatusThreadEntry(LPVOID pVoid)
{
	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
	if(pDlg)
	{
		CCommandPowerCableCreation* p = (CCommandPowerCableCreation*)pVoid;
		if(p) p->StatusThread();
		
		InterlockedExchange((LONG*)(&(pDlg->m_bThreadRunning)) , FALSE);
		if(pDlg)
		{
			pDlg->PostMessage(WM_COMMAND, IDOK);
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	power cable을 생성합니다.

	@author	BHK

	@date	?

	@param

	@return
*/
int COMMAND::CCommandPowerCableCreation::Execute(const bool& bExecuteSelectedLoad)
{	
	//! 선택한 LOAD에 대해서 Power Cable을 생성하는지 여부.
	m_bExecuteSelectedLoad = bExecuteSelectedLoad;
	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
	if(NULL == pDlg)
	{
		CWorkStatusDlg dlg;
		dlg.m_pThread = AfxBeginThread(CCommandPowerCableCreation::StatusThreadEntry , this , THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
		if(NULL != dlg.m_pThread)
		{
			InterlockedExchange((LONG*)(&(dlg.m_bThreadRunning)) , TRUE);
			dlg.DoModal();
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	BUS에 속하는 LOAD에 대해서 POWER CABLE을 생성한다.

	@author	BHK
*/
int CCommandPowerCableCreation::CreatePowerCableInBus(double nMinSize , CCableCreationTable::Record &SettingRecord , vector<CBusItem*>& BusItemEntry)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
	CString rTitle = _T("Calculate Cable Sizing Result...");
	for(vector<CBusItem*>::iterator itr = BusItemEntry.begin();itr != BusItemEntry.end();++itr)
	{
		const string rBusVolt = (*itr)->prop()->GetValue(_T("General") , _T("Bus Voltage"));
		const double nBusVolt = atof(rBusVolt.c_str());

		CItemSelectionSet SelectionSet;
		docData.GetLoadItemSetInBus(SelectionSet , (*itr)->GetName());
		for(int i = 0;i < SelectionSet.GetItemCount();++i)
		{
			CLoadItem* pLoadItem = static_cast<CLoadItem*>(SelectionSet.GetItemAt(i));
			list<CELoadItem*>::iterator where = find(m_InterestingItemList.begin() , m_InterestingItemList.end() , pLoadItem);
			if(where == m_InterestingItemList.end()) continue;

			CCableSizingResultDoc::m_nLoadPowerCableCreation = m_nPowerCableRadio;
			
			//! load에 속한 Auto Power Cable을 삭제한다.
			pLoadItem->ClearCableItems(_T("Power Cable"), _T("Auto"));
		
			//! Cable을 생성한 Load를 리스트에 넣어 Result View에 표시한다.
			CCableSizingResultDoc::m_InterestingLoadItemList.push_back(pLoadItem);

			++m_nTryCount;
			const int res = pLoadItem->CreatePowerCable(nMinSize , SettingRecord , nBusVolt);
			(ERROR_SUCCESS == res)? ++m_nSuccessCount : ++m_nFailCount;

			int progress = int((double(m_nIndex) / double(m_nTotalCount))*100.);
			if(pDlg) pDlg->UpdateWorkStatus(rTitle , progress);
			++m_nIndex;
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
int COMMAND::CCommandPowerCableCreation::ExecuteForEachRecord(vector<CBusItem*>& BusItemEntry , vector<CCableCreationTable::Record>& RecordList, double nMinSize , const string& rPhase)
{
	for(vector<CCableCreationTable::Record>::iterator itr = RecordList.begin();itr != RecordList.end();++itr)
	{
		map<string,string>::iterator where = itr->FieldValueMap.find(_T("C_BUS_VOLTAGE"));
		if(where != itr->FieldValueMap.end())
		{
			if(where->second.empty()) continue;

			vector<CBusItem*> SelectedBusItemEntry;
			for(vector<CBusItem*>::iterator jtr = BusItemEntry.begin();jtr != BusItemEntry.end();)
			{
				const string rValue    = (*jtr)->prop()->GetValue(_T("General") , _T("Bus Voltage"));
				const string rBusPhase = (*jtr)->prop()->GetValue(_T("General") , _T("Phase"));
				if((atof(where->second.c_str()) == atof(rValue.c_str())) && (rPhase == rBusPhase))
				{
					SelectedBusItemEntry.push_back(*jtr);
					jtr = BusItemEntry.erase(jtr);
				}else	++jtr;
			}

			if(!SelectedBusItemEntry.empty())
			{
				const string rCableVolt = itr->FieldValueMap[_T("C_CABLE_VOLTAGE")];
				const string rCableCore = itr->FieldValueMap[_T("C_CABLE_CORE")];
				const string rCableType = itr->FieldValueMap[_T("C_CABLE_TYPE")];
				const string rType = rCableVolt + _T("KV ") + rCableCore + _T(" CU") + _T(" ") + rCableType + _T(" (eLoad)");
				double nPercent = 0;
				if(_T("1C") == rCableCore)
					nPercent = m_nConductors_1;
				else if(_T("2C") == rCableCore)
					nPercent = m_nConductors_2;
				else	nPercent = m_nConductors_Over2;

				//! 먼저 Conduit Size List를 생성한다.
				if(ERROR_SUCCESS != CCableLengthDataSheet::CreateConduitSizeList(rType,m_rConduitTableName,nPercent,m_ConduitSizeList))
				{
					AfxMessageBox("Can't create conduit size list");
					return ERROR_BAD_ENVIRONMENT;
				}


				string rVoltType;
				map<string,string>::iterator VoltTypeItr = itr->FieldValueMap.find(_T("C_VOLTAGE_TYPE"));
				if(VoltTypeItr != itr->FieldValueMap.end()) rVoltType = VoltTypeItr->second.c_str();

				CreatePowerCableInBus(nMinSize , (*itr) , SelectedBusItemEntry);
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	power cable을 생성합니다.

	@author	BHK

	@date	?

	@param

	@return	pointer of created cable item
*/
CCableItem* COMMAND::CCommandPowerCableCreation::CreatePowerCableItem(const string& rFromID , const string& rToID , 
								      const string& rPanelID , const string& rLoadID ,
									CCableCreationTable::Record &SettingRecord)
{
	///assert(pLoadItem && "pLoadItem is NULL");
	CCableItem* pCableItem = NULL;

	///if(pLoadItem)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		pCableItem = static_cast<CCableItem*>(docData.CreateELoadItemOf(CCableItem::TypeString()));
		if(pCableItem)
		{
			pCableItem->prop()->SetValue(_T("General") , _T("Created By"), _T("Auto"));
			pCableItem->prop()->SetOriginalValue(_T("General") , _T("Created By"), _T("Auto"));

			pCableItem->prop()->SetValue(_T("General") , _T("From"), rFromID);
			pCableItem->prop()->SetOriginalValue(_T("General") , _T("From"), rFromID);

			pCableItem->prop()->SetValue(_T("General") , _T("To"), rToID);
			pCableItem->prop()->SetOriginalValue(_T("General") , _T("To"), rToID);

			pCableItem->prop()->SetValue(_T("General") , _T("Panel ID"), rPanelID);
			pCableItem->prop()->SetOriginalValue(_T("General") , _T("Panel ID"), rPanelID);

			pCableItem->prop()->SetValue(_T("General") , _T("Load Id"), rLoadID);
			pCableItem->prop()->SetOriginalValue(_T("General") , _T("Load Id"), rLoadID);
		}
	}

	return pCableItem;
}

/**
	@brief	space heater power cable을 생성합니다.

	@author	BHK
*/
CCableItem* COMMAND::CCommandPowerCableCreation::CreateSpaceHeaterPowerCableItem(CLoadItem* pLoadItem , CCableCreationTable::Record &SettingRecord , const double& nLength , const string& rBusID)
{
	assert(pLoadItem && "pLoadItem is NULL");
	CCableItem* pSpaceHeaterPowerCableItem = NULL;

	CELoadDocData& docData = CELoadDocData::GetInstance();
	TABLES::CPowerCableCreationSettingTable* pPowerCableCreationSettingTable = static_cast<TABLES::CPowerCableCreationSettingTable*>(docData.GetTableOf(CPowerCableCreationSettingTable::GetTableName()));
	if(pLoadItem && pPowerCableCreationSettingTable)
	{
		//! Space Heater Power Cable을 생성합니다.
		pSpaceHeaterPowerCableItem = static_cast<CCableItem*>(docData.CreateELoadItemOf(CCableItem::TypeString()));
		if(NULL != pSpaceHeaterPowerCableItem)
		{
			pLoadItem->InitializeCableItemProperties(pSpaceHeaterPowerCableItem);

			string rSpaceHeaterCableVolt , rSpaceHeaterCableType , rSpaceHeaterCableSpec , rSpaceHeaterCableCore , rSpaceHeaterCableSize;
			if(1 == pPowerCableCreationSettingTable->GetRecordCount())
			{
				string rValue;
				pPowerCableCreationSettingTable->GetValue(0 , _T("C_SPACE_HEATER_CABLE_VOLTAGE") , rValue);
				rSpaceHeaterCableVolt = rValue.c_str();
				pPowerCableCreationSettingTable->GetValue(0 , _T("C_SPACE_HEATER_CABLE_TYPE") , rValue);
				rSpaceHeaterCableType = rValue.c_str();
				pPowerCableCreationSettingTable->GetValue(0 , _T("C_SPACE_HEATER_CABLE_SCHEDULE_SPEC") , rValue);
				rSpaceHeaterCableSpec = rValue.c_str();
				pPowerCableCreationSettingTable->GetValue(0 , _T("C_SPACE_HEATER_CABLE_CORE") , rValue);
				rSpaceHeaterCableCore = rValue.c_str();
				pPowerCableCreationSettingTable->GetValue(0 , _T("C_SPACE_HEATER_CABLE_SIZE") , rValue);
				rSpaceHeaterCableSize = rValue.c_str();
			}

			const string rLoadId = pLoadItem->GetName();
			const string rPrefix = SettingRecord.FieldValueMap[_T("C_SPACE_HEATER_POWER_CABLE_NAMING_PREFIX")];
			const string rSuffix = SettingRecord.FieldValueMap[_T("C_SPACE_HEATER_POWER_CABLE_NAMING_SUFFIX")];

			const string rCableId = rPrefix + rLoadId + rSuffix;
			pSpaceHeaterPowerCableItem->SetName(rCableId);
			pSpaceHeaterPowerCableItem->prop()->SetOriginalValue(_T("General") , _T("CABLE ID") , rCableId);

			const string rLoadID = pLoadItem->GetName();
			pSpaceHeaterPowerCableItem->prop()->SetValue(_T("General") , _T("From") , rBusID);
			pSpaceHeaterPowerCableItem->prop()->SetOriginalValue(_T("General") , _T("From") , rBusID);

			pSpaceHeaterPowerCableItem->prop()->SetValue(_T("General") , _T("To") , rLoadID);
			pSpaceHeaterPowerCableItem->prop()->SetOriginalValue(_T("General") , _T("To") , rLoadID);

			pSpaceHeaterPowerCableItem->prop()->SetValue(_T("General") , _T("Panel ID") , rBusID);
			pSpaceHeaterPowerCableItem->prop()->SetOriginalValue(_T("General") , _T("Panel ID") , rBusID);

			pSpaceHeaterPowerCableItem->prop()->SetValue(_T("General") , _T("Load Id") , rLoadID);
			pSpaceHeaterPowerCableItem->prop()->SetOriginalValue(_T("General") , _T("Load Id") , rLoadID);

			pSpaceHeaterPowerCableItem->prop()->SetValue(_T("General") , _T("Cable Category") , _T("Power Cable"));
			pSpaceHeaterPowerCableItem->prop()->SetOriginalValue(_T("General") , _T("Cable Category") , _T("Power Cable"));

			pSpaceHeaterPowerCableItem->prop()->SetValue(_T("General") , _T("Created By"), _T("Auto"));
			pSpaceHeaterPowerCableItem->prop()->SetOriginalValue(_T("General") , _T("Created By"), _T("Auto"));
			pSpaceHeaterPowerCableItem->prop()->SetValue(_T("General") , _T("SubCategory") , _T("Space Heater"));
			pSpaceHeaterPowerCableItem->prop()->SetOriginalValue(_T("General") , _T("SubCategory") , _T("Space Heater"));

			const string rVoltType = SettingRecord.FieldValueMap[_T("C_VOLTAGE_TYPE")];
			pSpaceHeaterPowerCableItem->prop()->SetValue(_T("General") , _T("Volt Type") , rVoltType);
			pSpaceHeaterPowerCableItem->prop()->SetOriginalValue(_T("General") , _T("Volt Type") , rVoltType);

			pSpaceHeaterPowerCableItem->prop()->SetValue(_T("General") , _T("Cable Volt") , rSpaceHeaterCableVolt);
			pSpaceHeaterPowerCableItem->prop()->SetOriginalValue(_T("General") , _T("Cable Volt") , rSpaceHeaterCableVolt);
			pSpaceHeaterPowerCableItem->prop()->SetValue(_T("General") , _T("Cable Type") , rSpaceHeaterCableType);
			pSpaceHeaterPowerCableItem->prop()->SetOriginalValue(_T("General") , _T("Cable Type") , rSpaceHeaterCableType);
			pSpaceHeaterPowerCableItem->prop()->SetValue(_T("General") , _T("Cable Spec") , rSpaceHeaterCableSpec);
			pSpaceHeaterPowerCableItem->prop()->SetOriginalValue(_T("General") , _T("Cable Spec") , rSpaceHeaterCableSpec);
			pSpaceHeaterPowerCableItem->prop()->SetValue(_T("General") , _T("Core") , rSpaceHeaterCableCore);
			pSpaceHeaterPowerCableItem->prop()->SetOriginalValue(_T("General") , _T("Core") , rSpaceHeaterCableCore);
			pSpaceHeaterPowerCableItem->prop()->SetValue(_T("General") , _T("Size") , rSpaceHeaterCableSize);
			pSpaceHeaterPowerCableItem->prop()->SetOriginalValue(_T("General") , _T("Size") , rSpaceHeaterCableSize);
			
			ostringstream oss;
			oss << nLength;
			pSpaceHeaterPowerCableItem->prop()->SetValue(_T("General") , _T("Length") , oss.str());
			pSpaceHeaterPowerCableItem->prop()->SetOriginalValue(_T("General") , _T("Length") , oss.str());

			pLoadItem->AddCable(pSpaceHeaterPowerCableItem);
			pSpaceHeaterPowerCableItem->SetLoadItemPtr(pLoadItem);
		}
	}

	return pSpaceHeaterPowerCableItem;
}

/**
	@brief	cable numbering을 적용합니다.

	@author	BHK
*/
int COMMAND::CCommandPowerCableCreation::AdjustCableNumbering(CELoadItem* pItem , CCableCreationTable::Record &SettingRecord , CCableItem* pCableItem)
{
	assert(/*pItem &&*/ pCableItem && "pCableItem is NULL");

	if(pCableItem)
	{
		vector<CCableItem*> CableItemEntry;
		CableItemEntry.push_back(pCableItem);

		CELoadDocData& docData = CELoadDocData::GetInstance();
		TABLES::CCableNamingRuleSettingTable& CableNamingRuleSettingTable = CCableNamingRuleSettingTable::GetInstance();
		CableNamingRuleSettingTable.Load(docData.GetProjectMDBFilePath());
		if(1 == CableNamingRuleSettingTable.GetRecordCount())
		{
			string rValue;
			CableNamingRuleSettingTable.GetValue(0 , _T("C_SINGLE_CORE_CABLE_NUMBERING") , rValue);
			const int nSingleCoreCableNumbering = atoi(rValue.c_str());
			const string rCableId = pCableItem->prop()->GetValue(_T("General") , _T("CABLE ID"));

			const string rPhase = SettingRecord.FieldValueMap[_T("C_PHASE")];
			string rCore  = pCableItem->prop()->GetValue(_T("General") , _T("Core"));
			rCore = rCore.substr(0 , rCore.length() - 1);
			if((_T("3") == rPhase) && (_T("1") == rCore))
			{
				for(int i = 0;i < 2;++i)
				{
					CCableItem* pNewCableItem = static_cast<CCableItem*>(pCableItem->Clone());
					if(NULL == pNewCableItem) continue;
					CableItemEntry.push_back(pNewCableItem);
					if(pItem && pItem->IsKindOf(CLoadItem::TypeString()))
					{
						CLoadItem* pLoadItem = static_cast<CLoadItem*>(pItem);
						pLoadItem->AddCable(pNewCableItem);
					}

					if(0 == nSingleCoreCableNumbering)
					{
						stringstream oss;
						oss << rCableId << "-" << (i + 2);
						pNewCableItem->prop()->SetValue(_T("General") , _T("CABLE ID") ,oss.str());
					}
					else if(1 == nSingleCoreCableNumbering)
					{
						stringstream oss;
						oss << rCableId << "-" << ('A' + i + 1);
						pNewCableItem->prop()->SetValue(_T("General") , _T("CABLE ID") , oss.str());
					}
					else if(2 == nSingleCoreCableNumbering)
					{
						stringstream oss;
						oss << rCableId << "-" << ('A' + i + 1);
						pNewCableItem->prop()->SetValue(_T("General") , _T("CABLE ID") , oss.str());
					}
				}

				if(0 == nSingleCoreCableNumbering)
				{
					stringstream oss;
					oss << rCableId << "-" << 1;
					pCableItem->prop()->SetValue(_T("General") , _T("CABLE ID") ,oss.str());
				}
				else if(1 == nSingleCoreCableNumbering)
				{
					stringstream oss;
					oss << rCableId << "-" << 'A';
					pCableItem->prop()->SetValue(_T("General") , _T("CABLE ID") , oss.str());
				}
				else if(2 == nSingleCoreCableNumbering)
				{
					stringstream oss;
					oss << rCableId << "-" << 'A';
					pCableItem->prop()->SetValue(_T("General") , _T("CABLE ID") , oss.str());
				}
			}
			else if(((_T("1") == rPhase) || (_T("DC") == rPhase)) && (_T("1") == rCore))
			{
				CCableItem* pNewCableItem = static_cast<CCableItem*>(pCableItem->Clone());
				if(pNewCableItem)
				{
					CableItemEntry.push_back(pNewCableItem);
					if(pItem && pItem->IsKindOf(CLoadItem::TypeString()))
					{
						CLoadItem* pLoadItem = static_cast<CLoadItem*>(pItem);
						pLoadItem->AddCable(pNewCableItem);
					}

					const string rCableId = pCableItem->prop()->GetValue(_T("General") , _T("CABLE ID"));
					if(0 == nSingleCoreCableNumbering)
					{
						stringstream oss;
						oss << rCableId << "-" << 2;
						pNewCableItem->prop()->SetOriginalValue(_T("General") , _T("CABLE ID") ,oss.str());
						pNewCableItem->prop()->SetValue(_T("General") , _T("CABLE ID") ,oss.str());
					}
					else if(1 == nSingleCoreCableNumbering)
					{
						stringstream oss;
						oss << rCableId << "-" << 'B';
						pNewCableItem->prop()->SetOriginalValue(_T("General") , _T("CABLE ID") ,oss.str());
						pNewCableItem->prop()->SetValue(_T("General") , _T("CABLE ID") , oss.str());
					}
					else if(2 == nSingleCoreCableNumbering)
					{
						stringstream oss;
						oss << rCableId << "-" << 'B';
						pNewCableItem->prop()->SetOriginalValue(_T("General") , _T("CABLE ID") ,oss.str());
						pNewCableItem->prop()->SetValue(_T("General") , _T("CABLE ID") , oss.str());
					}
				}

				if(0 == nSingleCoreCableNumbering)
				{
					stringstream oss;
					oss << rCableId << "-" << 1;
					pCableItem->prop()->SetOriginalValue(_T("General") , _T("CABLE ID") ,oss.str());
					pCableItem->prop()->SetValue(_T("General") , _T("CABLE ID") ,oss.str());
				}
				else if(1 == nSingleCoreCableNumbering)
				{
					stringstream oss;
					oss << rCableId << "-" << 'A';
					pCableItem->prop()->SetOriginalValue(_T("General") , _T("CABLE ID") ,oss.str());
					pCableItem->prop()->SetValue(_T("General") , _T("CABLE ID") , oss.str());
				}
				else if(2 == nSingleCoreCableNumbering)
				{
					stringstream oss;
					oss << rCableId << "-" << 'A';
					pCableItem->prop()->SetOriginalValue(_T("General") , _T("CABLE ID") ,oss.str());
					pCableItem->prop()->SetValue(_T("General") , _T("CABLE ID") , oss.str());
				}
			}

			const string rNoOfCable = pCableItem->prop()->GetValue(_T("General") , _T("No Of Cables/Phase"));
			const int nNoOfCable = atoi(rNoOfCable.c_str());

			if(nNoOfCable > 1)
			{
				for(vector<CCableItem*>::iterator itr = CableItemEntry.begin();itr != CableItemEntry.end();++itr)
				{
					const string rCableId = (*itr)->prop()->GetValue(_T("General") , _T("CABLE ID"));

					string rValue;
					CableNamingRuleSettingTable.GetValue(0 , _T("C_PARALLEL_CABLE_NUMBERING") , rValue);
					const int nParallelCableNumbering = atoi(rValue.c_str());
					for(int i = 0;i < nNoOfCable - 1;++i)
					{
						CCableItem* pNewCableItem = static_cast<CCableItem*>((*itr)->Clone());
						if(NULL == pNewCableItem) continue;
						if(pItem && pItem->IsKindOf(CLoadItem::TypeString()))
						{
							CLoadItem* pLoadItem = static_cast<CLoadItem*>(pItem);
							pLoadItem->AddCable(pNewCableItem);
						}

						if(0 == nParallelCableNumbering)
						{
							stringstream oss;
							oss << rCableId << "-" << (i + 2);
							pNewCableItem->prop()->SetOriginalValue(_T("General") , _T("CABLE ID") ,oss.str());
							pNewCableItem->prop()->SetValue(_T("General") , _T("CABLE ID") ,oss.str());
						}
						else if(1 == nParallelCableNumbering)
						{
							stringstream oss;
							oss << rCableId << "-" << ('A' + i + 1);
							pNewCableItem->prop()->SetOriginalValue(_T("General") , _T("CABLE ID") ,oss.str());
							pNewCableItem->prop()->SetValue(_T("General") , _T("CABLE ID") , oss.str());
						}
					}

					///(*itr)->prop()->SetValue(_T("General") , _T("No Of Cables/Phase") , _T("1"));
					///(*itr)->prop()->SetOriginalValue(_T("General") , _T("No Of Cables/Phase") , _T("1"));
					if(0 == nParallelCableNumbering)
					{
						stringstream oss;
						oss << rCableId << "-" << 1;
						(*itr)->prop()->SetValue(_T("General") , _T("CABLE ID") ,oss.str());
						(*itr)->prop()->SetOriginalValue(_T("General") , _T("CABLE ID") ,oss.str());
					}
					else if(1 == nParallelCableNumbering)
					{
						stringstream oss;
						oss << rCableId << "-" << 'A';
						(*itr)->prop()->SetValue(_T("General") , _T("CABLE ID") , oss.str());
						(*itr)->prop()->SetOriginalValue(_T("General") , _T("CABLE ID") ,oss.str());
					}
				}
			}
		}

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	옵션(ALL_POWER_CABLE/UNSIZED_POWER_CABLE)에 따라 필요한 Load Item을 구한다.

	@author BHK	

	@date 2009-07-09 오후 2:32:55	

	@param	

	@return		
*/
int CCommandPowerCableCreation::GetInterestingLoadItemListAndOption()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	m_nPowerCableRadio = CCableSizingResultDoc::ALL_POWER_CABLE;
	CPowerCableCreationSettingTable* pPowerCableCreationSettingTable = static_cast<CPowerCableCreationSettingTable*>(docData.GetTableOf(CPowerCableCreationSettingTable::GetTableName()));
	if(pPowerCableCreationSettingTable)
	{
		CString rLoadPowerCableCreation;
		map<string,string>::iterator where = pPowerCableCreationSettingTable->m_RecordEntry[0].FieldValueMap.find(_T("C_LOAD_POWER_CABLE_CREATION"));
		if(where != pPowerCableCreationSettingTable->m_RecordEntry[0].FieldValueMap.end()) rLoadPowerCableCreation = where->second.c_str();
		
		if(_T("YES") == rLoadPowerCableCreation.MakeUpper())
		{
			m_nOwnerType = LOAD;
			where = pPowerCableCreationSettingTable->m_RecordEntry[0].FieldValueMap.find(_T("C_LOAD_POWER_CABLE_RADIO"));
			if(where != pPowerCableCreationSettingTable->m_RecordEntry[0].FieldValueMap.end()) m_nPowerCableRadio = atoi(where->second.c_str());
		}

		CString rEqPowerCableCreation;
		where = pPowerCableCreationSettingTable->m_RecordEntry[0].FieldValueMap.find(_T("C_EQ_BUS_POWER_CABLE_CREATION"));
		if(where != pPowerCableCreationSettingTable->m_RecordEntry[0].FieldValueMap.end()) rEqPowerCableCreation = where->second.c_str();
		
		if(_T("YES") == rEqPowerCableCreation.MakeUpper())
		{
			m_nOwnerType = EQ;
			where = pPowerCableCreationSettingTable->m_RecordEntry[0].FieldValueMap.find(_T("C_BUS_POWER_CABLE_RADIO"));
			if(where != pPowerCableCreationSettingTable->m_RecordEntry[0].FieldValueMap.end()) m_nPowerCableRadio = atoi(where->second.c_str());
		}
	}

	if((false == m_bExecuteSelectedLoad) && (LOAD == m_nOwnerType))
	{
		if(CCableSizingResultDoc::ALL_POWER_CABLE == m_nPowerCableRadio)
		{
			docData.GetELoadItemListOf(m_InterestingItemList , CLoadItem::TypeString());
		}
		else if(CCableSizingResultDoc::UNSIZED_POWER_CABLE_ONLY == m_nPowerCableRadio)
		{
			list<CELoadItem*> LoadItemList;
			docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
			for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
			{
				CLoadItem* pLoadItem = static_cast<CLoadItem*>(*itr);
				const int nCableCount = pLoadItem->GetCableCountOf(_T("Power Cable"));
				if(0 == nCableCount) m_InterestingItemList.push_back(*itr);
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
int COMMAND::CCommandPowerCableCreation::StatusThread(void)
{
	//! Power Cable을 생성할 Item을 담을 List를 클리어한다.
	CCableSizingResultDoc::m_InterestingLoadItemList.clear();

	if(LOAD == m_nOwnerType)
	{
		//! LOAD에 대해서 Power Cable을 생성 시킴.
		vector<CBusItem*> BusItemEntry;
		CELoadDocData& docData = CELoadDocData::GetInstance();
		const string rMDBFilePath = docData.GetProjectMDBFilePath();
		docData.GetAllBusItems(BusItemEntry);

		if(BusItemEntry.empty()) return ERROR_BAD_ENVIRONMENT;

		double nMinLvSize = -1 , nMinHvSize = -1 , nConductors_1  = 0 , nConductors_2 = 0 , nConductors_Over2 = 0;
		string rConduitType;
		CPowerCableCreationSettingTable* pPowerCableCreationSettingTable = static_cast<CPowerCableCreationSettingTable*>(docData.GetTableOf(CPowerCableCreationSettingTable::GetTableName()));
		if(pPowerCableCreationSettingTable)
		{
			if(1 == pPowerCableCreationSettingTable->GetRecordCount())
			{
				string rValue;
				pPowerCableCreationSettingTable->GetValue(0 , _T("C_MIN_LV_CHECK") , rValue);
				CString rMinLvCheck = rValue.c_str();
				if(_T("YES") == rMinLvCheck.MakeUpper())
				{
					pPowerCableCreationSettingTable->GetValue(0 , _T("C_MIN_LV") , rValue);
					CString rMinLv = rValue.c_str();
					nMinLvSize = atof(rMinLv);
				}

				pPowerCableCreationSettingTable->GetValue(0 , _T("C_FAULT_CURRENT_AT_HV_CHECK") , rValue);
				CString rMinHvCheck = rValue.c_str();
				if(_T("YES") == rMinHvCheck.MakeUpper())
				{
					pPowerCableCreationSettingTable->GetValue(0 , _T("C_FAULT_CURRENT_AT_HV") , rValue);
					CString rMinHv = rValue.c_str();
					nMinHvSize = atof(rMinHv);
				}

				pPowerCableCreationSettingTable->GetValue(0 , _T("C_CONDUIT_LENGTH") , rValue);
				m_nConduitLength   = atof(rValue.c_str());
				pPowerCableCreationSettingTable->GetValue(0 , _T("C_CONDUIT_TYPE") , rValue);
				m_rConduitTableName= rValue.c_str();
				pPowerCableCreationSettingTable->GetValue(0 , _T("C_NUMBER_OF_CONDUCTORS_1") , rValue);
				m_nConductors_1    = (0.01 * atof(rValue.c_str()));
				pPowerCableCreationSettingTable->GetValue(0 , _T("C_NUMBER_OF_CONDUCTORS_2") , rValue);
				m_nConductors_2    = (0.01 * atof(rValue.c_str()));
				pPowerCableCreationSettingTable->GetValue(0 , _T("C_NUMBER_OF_CONDUCTORS_OVER2") , rValue);
				m_nConductors_Over2= (0.01 * atof(rValue.c_str()));

				pPowerCableCreationSettingTable->GetValue(0 , _T("C_LOAD_POWER_CABLE_RADIO") , rValue);
				m_nPowerCableRadio = atoi(rValue.c_str());
			}
		}

		GetInterestingLoadItemListAndOption();
		m_nTotalCount = count_if(m_InterestingItemList.begin() , m_InterestingItemList.end() , CELoadItem::IsNotDeleted);
		m_nIndex = 0;
		m_nTryCount = 0;m_nSuccessCount = 0;m_nFailCount = 0;

		//! Cable Data Table을 로딩한다.
		CString rProjectMDB = rMDBFilePath.c_str();
		rProjectMDB.MakeUpper();
		if(FileExist(rProjectMDB))
		{
			CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
			string* pCableDataTableName = pSettingTable->GetFieldValue(_T("C_CABLEDATA_TABLE"));
			if(pCableDataTableName && !pCableDataTableName->empty())
			{
				docData.m_ProjectCableDataTable.Load(*pCableDataTableName , (*pCableDataTableName) != docData.m_ProjectCableDataTable.GetTableName());
			}
		}

		//! 실제 RUNNING하는 부분(HV,LV3,LV1,DC순으로 RUNNING함)
		CHVCableCreationSettingTable& HVVoltageSettingTable = CHVCableCreationSettingTable::GetInstance();
		HVVoltageSettingTable.Load(rMDBFilePath);
		ExecuteForEachRecord(BusItemEntry , HVVoltageSettingTable.m_RecordEntry , nMinHvSize , _T("3"));
		
		CLV3PHCableCreationSettingTable& LV3PHVoltageSettingTable = CLV3PHCableCreationSettingTable::GetInstance();
		LV3PHVoltageSettingTable.Load(rMDBFilePath);
		ExecuteForEachRecord(BusItemEntry , LV3PHVoltageSettingTable.m_RecordEntry , nMinLvSize , _T("3"));
		
		CLV1PHCableCreationSettingTable& LV1PHVoltageSettingTable = CLV1PHCableCreationSettingTable::GetInstance();
		LV1PHVoltageSettingTable.Load(rMDBFilePath);
		ExecuteForEachRecord(BusItemEntry , LV1PHVoltageSettingTable.m_RecordEntry , nMinLvSize , _T("1"));
		
		CDCCableCreationSettingTable& DCVoltageSettingTable = CDCCableCreationSettingTable::GetInstance();
		DCVoltageSettingTable.Load(rMDBFilePath);
		ExecuteForEachRecord(BusItemEntry , DCVoltageSettingTable.m_RecordEntry , nMinLvSize , _T("DC"));

		//! can't find power cable setting for bus voltage.
		if(!BusItemEntry.empty())
		{
			for(vector<CBusItem*>::iterator itr = BusItemEntry.begin();itr != BusItemEntry.end();++itr)
			{
				CItemSelectionSet SelectionSet;
				docData.GetLoadItemSetInBus(SelectionSet , (*itr)->GetName());
				for(int i = 0;i < SelectionSet.GetItemCount();++i)
				{
					ELOAD_LOG4CXX_ERROR(mylogger , SelectionSet.GetItemAt(i)->GetName().c_str() , 48);
				}
			}
		}

		m_nTryCount = m_InterestingItemList.size();
		if(0 == m_nTryCount)
		{
			//! 시도 횟수는 전체 Load의 갯수와 같다...
			list<CELoadItem*> LoadItemList;
			docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
			m_nTryCount = LoadItemList.size();
		}
		m_nFailCount= m_nTryCount - m_nSuccessCount;
	}
	else if(EQ == m_nOwnerType)
	{
		//! Equipment에 대해서 Power Cable을 생성시킴.
		CELoadDocData& docData = CELoadDocData::GetInstance();
		const string rMDBFilePath = docData.GetProjectMDBFilePath();

		double nMinLvSize = -1;
		string rConduitType;
		CPowerCableCreationSettingTable* pPowerCableCreationSettingTable = static_cast<CPowerCableCreationSettingTable*>(docData.GetTableOf(CPowerCableCreationSettingTable::GetTableName()));
		if(pPowerCableCreationSettingTable)
		{
			if(1 == pPowerCableCreationSettingTable->GetRecordCount())
			{
				string rValue;
				pPowerCableCreationSettingTable->GetValue(0 , _T("C_MIN_LV_CHECK") , rValue);
				CString rMinLvCheck = rValue.c_str();
				if(_T("YES") == rMinLvCheck.MakeUpper())
				{
					pPowerCableCreationSettingTable->GetValue(0 , _T("C_MIN_LV") , rValue);
					CString rMinLv = rValue.c_str();
					nMinLvSize = atof(rMinLv);
				}
				pPowerCableCreationSettingTable->GetValue(0 , _T("C_EQ_BUS_POWER_CABLE_CREATION") , rValue);
				m_nPowerCableRadio = atoi(rValue.c_str());
			}
		}

		//! Cable Data Table을 로딩한다.
		CString rProjectMDB = rMDBFilePath.c_str();
		rProjectMDB.MakeUpper();
		if(FileExist(rProjectMDB))
		{
			CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
			string* pCableDataTableName = pSettingTable->GetFieldValue(_T("C_CABLEDATA_TABLE"));
			if(pCableDataTableName && !pCableDataTableName->empty())
			{
				docData.m_ProjectCableDataTable.Load(*pCableDataTableName , (*pCableDataTableName) != docData.m_ProjectCableDataTable.GetTableName());
			}
		}
		CLV3PHCableCreationSettingTable& LV3PHVoltageSettingTable = CLV3PHCableCreationSettingTable::GetInstance();

		m_nTryCount = 0;m_nSuccessCount = 0;m_nFailCount = 0;
		m_nIndex = 0;

		list <CELoadItem*> TransformerItemList;
		docData.GetELoadItemListOf(TransformerItemList, CTransformerItem::TypeString());
		list <CELoadItem*> CapacitorItemList;
		docData.GetELoadItemListOf(CapacitorItemList, CCapacitorItem::TypeString());
		list <CELoadItem*> UpsDcItemList;
		docData.GetELoadItemListOf(UpsDcItemList, CUPSDCItem::TypeString());

		m_nTotalCount = TransformerItemList.size() + CapacitorItemList.size() + UpsDcItemList.size();

		CreateCableForTransformer(LV3PHVoltageSettingTable.m_RecordEntry , nMinLvSize);
		CreateCableForCapacitor(LV3PHVoltageSettingTable.m_RecordEntry , nMinLvSize);
		CreateCableForUPSDC(LV3PHVoltageSettingTable.m_RecordEntry , nMinLvSize);
	}
	
	return ERROR_SUCCESS;
}

/**
	@brief	LoadCategory에 따라 Running Voltage Drop을 리턴합니다.

	@author BHK	

	@date 2009-04-02 오후 8:01:25	

	@param	

	@return		
*/
string COMMAND::CCommandPowerCableCreation::GetRunVoltageDrop(CLoadItem* pLoadItem)
{
	assert(pLoadItem && "pLoadItem is NULL");
	string rRunningVD;

	if(pLoadItem)
	{
		const string rLoadCategory = CString(pLoadItem->prop()->GetValue(_T("Type") , _T("Load Category")).c_str()).MakeUpper();

		rRunningVD = (_T("MOTOR") == rLoadCategory) ? 
			pLoadItem->prop()->GetValue(_T("Voltage Drop") , _T("Motor Run VD")) : 
			pLoadItem->prop()->GetValue(_T("Voltage Drop") , _T("Feeder Run VD"));
	}

	return rRunningVD;
}

/**
	@brief	PDB일 경우에는 LoadSummary 계산 결과인 PF값을 리턴합니다.단, 0일 경우에는 에러가 발생하지 않도록 1을 리턴하도록 합니다.

	@author BHK	

	@date 2009-04-17 오후 4:11:02	

	@param	

	@return		
*/
double COMMAND::CCommandPowerCableCreation::GetPowerFactor(CLoadItem* pLoadItem)
{
	assert(pLoadItem && "pLoadItem is NULL");
	double nPowerFactor = 0.f;

	if(pLoadItem) nPowerFactor = pLoadItem->GetPFForPowerCableCreation();

	//! PF값이 0일 경우에는 에러가 발생하지 않도록 1을 리턴하도록 합니다.
	if(0.f == nPowerFactor) nPowerFactor = 1.f;
	nPowerFactor = SAFE_ROUND(nPowerFactor , 3);

	return nPowerFactor;
}

/**
	@brief	Load Category가 MOTOR이고 , Rating Capacity가 설정한 값보다 클때 Space Heater Cable을 생성합니다.

	@author BHK	

	@date 2009-04-23 오후 1:54:29	

	@param	

	@return		
*/
int COMMAND::CCommandPowerCableCreation::CreateSpaceHeaterPowerCableIfNeed(CLoadItem* pLoadItem , CCableCreationTable::Record &SettingRecord , CCableItem* pCableItem , const string& rBusID)
{
	assert(pLoadItem && pCableItem && "pLoadItem or pCableItem is NULL");

	if(pLoadItem && pCableItem)
	{
		const string rLoadCategory = pLoadItem->prop()->GetValue(_T("Type") , _T("Load Category"));
		if(_T("MOTOR") == rLoadCategory)
		{
			const string rRatingCapacityAbove = pLoadItem->prop()->GetValue(_T("Power Cable Setting") , _T("Rating Capacity Above"));
			const string rRatingCapacity = pLoadItem->prop()->GetValue(_T("Rating") , _T("Rating Capacity"));
			if(!rRatingCapacity.empty() && (atof(rRatingCapacity.c_str()) >= atof(rRatingCapacityAbove.c_str())))
			{
				const string rLength = pCableItem->prop()->GetValue(_T("General") , _T("Length"));
				CreateSpaceHeaterPowerCableItem(pLoadItem , SettingRecord , atof(rLength.c_str()) , rBusID);
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
string COMMAND::CCommandPowerCableCreation::GetResultString(void)
{
	ostringstream oss;
	if(CCableSizingResultDoc::UNSIZED_POWER_CABLE_ONLY == m_nPowerCableRadio)
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

/**
	@brief	Transformer용 Power Cable을 생성한다.

	@author BHK	

	@date 2009-07-29 오전 9:20:50	

	@param	

	@return		
*/
int COMMAND::CCommandPowerCableCreation::CreateCableForTransformer(vector<CCableCreationTable::Record>& RecordList , double nMinSize)
{
	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
	CString rTitle = _T("Calculate Cable Sizing Result...");

	CELoadDocData& docData = CELoadDocData::GetInstance();

	//! TRANSFORMER
	list <CELoadItem*> TransformerItemList;
	docData.GetELoadItemListOf(TransformerItemList, CTransformerItem::TypeString());
	{
		for(list<CELoadItem*>::iterator itr = TransformerItemList.begin(); itr != TransformerItemList.end();++itr)
		{
			int progress = int((double(m_nIndex) / double(m_nTotalCount))*100.);
			if(pDlg) pDlg->UpdateWorkStatus(rTitle , progress);
			++m_nIndex;

			CTransformerItem* pTrItem = static_cast<CTransformerItem*>(*itr);
			if(pTrItem)
			{
				list<CCableItem*> CableItemList;
				pTrItem->GetCableList(CableItemList);

				//! UNSIZED POWER CABLE ONLY일때 EQ가 Cable을 가지고 있으면 생성을 시도하지 않는다.
				if(CCableSizingResultDoc::UNSIZED_POWER_CABLE_ONLY == m_nPowerCableRadio)
				{
					if(!CableItemList.empty()) continue;
				}
				else
				{
					//! ALL Power cable 일때 기존의 Cable을 삭제한다.
					if(!CableItemList.empty())
					{
						for(list<CCableItem*>::iterator jtr = CableItemList.begin(); jtr != CableItemList.end(); ++jtr)
						{
							if(*jtr)
							{
								CCableItem::DeleteInstance(*jtr);
							}
						}
					}
				}
				++m_nTryCount;

				double nFlc = 0.f;
				const string rTrID = pTrItem->GetName();
				const string rPF = pTrItem->prop()->GetValue(_T("General") , _T("PF"));
				const string rCableLength = pTrItem->prop()->GetValue(_T("General") , _T("Cable Length"));
				const string rFrom = pTrItem->prop()->GetValue(_T("From"), _T("Bus Id"));
				const string rTo = pTrItem->prop()->GetValue(_T("To"), _T("Bus Id"));
				const string rConnection = pTrItem->prop()->GetValue(_T("To"), _T("Connection"));

				const string rCableName1 = GetCableNameFor(rTrID , rFrom);
				const string rCableName2 = GetCableNameFor(rTrID , rTo);
				if(!rPF.empty() && !rCableLength.empty() && !rCableName1.empty() && ((_T("CABLE") != rConnection) || !rCableName2.empty()))
				{
					const string rFromBusVolt = pTrItem->prop()->GetValue(_T("From") , _T("Bus Volt")).c_str();
					const double nFromBusVolt = atof(rFromBusVolt.c_str());
					CCableCreationTable::Record record[2];
					string rPhase[2];
					if(ERROR_SUCCESS != GetPowerCableCreationSettingRecordForEqPowerCable(rFrom , record[0] , rPhase[0]))
					{
						ELOAD_LOG4CXX_ERROR(mylogger , rTrID , 46);
						++m_nFailCount;
						continue;
					}
					ParamForPowerCableCreation param[2];
					{
						param[0].rPhase   = rPhase[0];
						param[0].nBusVolt = nFromBusVolt;
						const string rSelectedKVA = pTrItem->prop()->GetValue(_T("Capacity"), _T("Selected Capacity"));
						param[0].nLoadVolt = atof(pTrItem->prop()->GetValue(_T("From"), _T("Load Volt")).c_str());
						param[0].nFLC = atof(rSelectedKVA.c_str()) / sqrt(3.f) / param[0].nLoadVolt;
						param[0].rSpec= pTrItem->prop()->GetValue(_T("From"), _T("Cable Spec"));
						param[0].rFrom = pTrItem->prop()->GetValue(_T("From"), _T("Bus Id"));
						param[0].rTo = pTrItem->GetName();
						param[0].rPanelID = pTrItem->prop()->GetValue(_T("From"), _T("Bus Id"));
						param[0].rLoadID = pTrItem->GetName();
						param[0].rArea = m_rAreaNameForEquip;
					}
					if(_T("CABLE") == rConnection)
					{
						const string rToBusID = pTrItem->prop()->GetValue(_T("To") , _T("Bus Id"));
						if(ERROR_SUCCESS != GetPowerCableCreationSettingRecordForEqPowerCable(rToBusID , record[1] , rPhase[1]))
						{
							
							ELOAD_LOG4CXX_ERROR(mylogger , rTrID , 46);
							++m_nFailCount;
							continue;
						}
						{
							param[1].rPhase = rPhase[1];
							param[1].nBusVolt = atof(pTrItem->prop()->GetValue(_T("To") , _T("Bus Volt")).c_str());
							const string rSelectedKVA = pTrItem->prop()->GetValue(_T("Capacity"), _T("Selected Capacity"));
							param[1].nLoadVolt = atof(pTrItem->prop()->GetValue(_T("To"), _T("Load Volt")).c_str());
							param[1].nFLC = atof(rSelectedKVA.c_str()) / sqrt(3.f) / param[1].nLoadVolt;
							param[1].rSpec= pTrItem->prop()->GetValue(_T("To"), _T("Cable Spec"));
							param[1].rFrom = pTrItem->GetName();
							param[1].rTo = pTrItem->prop()->GetValue(_T("To"), _T("Bus Id"));
							param[1].rPanelID = pTrItem->prop()->GetValue(_T("To"), _T("Bus Id"));
							param[1].rLoadID = pTrItem->GetName();
							param[1].rArea = m_rAreaNameForEquip;
						}
					}
					
					int res[2] = {ERROR_BAD_ENVIRONMENT , ERROR_BAD_ENVIRONMENT};

					res[0] = pTrItem->CreatePowerCable(record[0] , param[0]);

					//! (TO BUS OPTION이 CABLE일 경우만)
					if(_T("CABLE") == rConnection)
					{
						res[1] = pTrItem->CreatePowerCable(record[1] , param[1]);
						if((ERROR_SUCCESS == res[0]) && (ERROR_SUCCESS == res[1]))
						{
							++m_nSuccessCount;
							//! Cable을 생성한 Transformer를 리스트에 넣어 Result View에 표시한다.
							CCableSizingResultDoc::m_InterestingLoadItemList.push_back(pTrItem);
						}
						else	
						{
							//! Cable의 Load Id가 UPS DC의 ID와 같은 Cable은 삭제한다.
							list<CELoadItem*> CableItemList;
							docData.GetELoadItemListOf(CableItemList, CCableItem::TypeString());
							list<CELoadItem*>::iterator where = find_if(CableItemList.begin(), CableItemList.end(), bind2nd(MatchLoadId(), rTrID));
							if(where != CableItemList.end())
							{
								CCableItem* pCableItem = static_cast<CCableItem*>(*where);
								if(pCableItem)
								{
									CCableItem::DeleteInstance(pCableItem);
								}
							}
							++m_nFailCount;
						}
					}
					else
					{
						if(ERROR_SUCCESS == res[0])	
						{
							++m_nSuccessCount;
							//! Cable을 생성한 Transformer를 리스트에 넣어 Result View에 표시한다.
							CCableSizingResultDoc::m_InterestingLoadItemList.push_back(pTrItem);
						}
						else	++m_nFailCount;
						
					}
				}
				else
				{
					ELOAD_LOG4CXX_ERROR(mylogger , rTrID , 47);
					++m_nFailCount;
				}
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	Capacitor용 Cable을 생성한다.

	@author BHK	

	@date 2009-07-29 오전 9:25:34	

	@param	

	@return		
*/
int COMMAND::CCommandPowerCableCreation::CreateCableForCapacitor(vector<CCableCreationTable::Record>& RecordList , double nMinSize)
{
	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
	CString rTitle = _T("Calculate Cable Sizing Result...");

	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string rMDBFilePath = docData.GetProjectMDBFilePath();

	//! CAPACITOR
	list <CELoadItem*> CapacitorItemList;
	docData.GetELoadItemListOf(CapacitorItemList, CCapacitorItem::TypeString());
	{
		for(list<CELoadItem*>::iterator itr = CapacitorItemList.begin(); itr != CapacitorItemList.end();++itr)
		{
			int progress = int((double(m_nIndex) / double(m_nTotalCount))*100.);
			if(pDlg) pDlg->UpdateWorkStatus(rTitle , progress);
			++m_nIndex;

			CCapacitorItem* pCapacitorItem = static_cast<CCapacitorItem*>(*itr);
			if(pCapacitorItem)
			{
				list<CCableItem*> CableItemList;
				pCapacitorItem->GetCableList(CableItemList);

				//! UNSIZED POWER CABLE ONLY일때 EQ가 Cable을 가지고 있으면 생성을 시도하지 않는다.
				if(CCableSizingResultDoc::UNSIZED_POWER_CABLE_ONLY == m_nPowerCableRadio)
				{
					if(!CableItemList.empty()) continue;
				}
				else
				{
					//! ALL Power cable 일때 기존의 Cable을 삭제한다.
					if(!CableItemList.empty())
					{
						for(list<CCableItem*>::iterator jtr = CableItemList.begin(); jtr != CableItemList.end(); ++jtr)
						{
							if(*jtr)
							{
								CCableItem::DeleteInstance(*jtr);
							}
						}
					}
				}
				
				++m_nTryCount;
				
				const string rCapacitorID = pCapacitorItem->GetName();
				const string rFromBusID = pCapacitorItem->prop()->GetValue(_T("From") , _T("Bus Id"));
				//! ERROR CHECK
				if(pCapacitorItem->prop()->GetValue(_T("Capacity") , _T("Selected Capacity")).empty())
				{
					ELOAD_LOG4CXX_ERROR(mylogger , rCapacitorID , 50);
					++m_nFailCount;
					continue;
				}
				CCableCreationTable::Record record;
				string rPhase;
				if(ERROR_SUCCESS != GetPowerCableCreationSettingRecordForEqPowerCable(rFromBusID , record , rPhase))
				{
					ELOAD_LOG4CXX_ERROR(mylogger , rCapacitorID , 46);
					++m_nFailCount;
					continue;
				}

				double nFlc = 0.f;
				const string rFrom = pCapacitorItem->prop()->GetValue(_T("From"), _T("Bus Id"));
				const string rPF = pCapacitorItem->prop()->GetValue(_T("General"), _T("PF"));
				const string rCableLength = pCapacitorItem->prop()->GetValue(_T("General"), _T("Cable Length"));
				const double nBusVolt = atof(pCapacitorItem->prop()->GetValue(_T("From"), _T("Bus Volt")).c_str());
				const string rCableName = GetCableNameFor(rCapacitorID , rFrom);
				if(!rPF.empty() && !rCableLength.empty() && !rCableName.empty())
				{
					ParamForPowerCableCreation param;
					{
						param.rPhase = rPhase;
						param.nBusVolt = nBusVolt;
						param.nFLC = atof(pCapacitorItem->prop()->GetValue(_T("Capacity") , _T("FLC")).c_str());
						param.nLoadVolt = atof(pCapacitorItem->prop()->GetValue(_T("From"), _T("Load Volt")).c_str());
						param.rSpec= pCapacitorItem->prop()->GetValue(_T("From"), _T("Cable Spec"));
						param.rFrom = pCapacitorItem->prop()->GetValue(_T("From"), _T("Bus Id"));
						param.rTo = pCapacitorItem->GetName();
						param.rPanelID = pCapacitorItem->prop()->GetValue(_T("From"), _T("Bus Id"));
						param.rLoadID  = pCapacitorItem->GetName();
						param.rArea = m_rAreaNameForEquip;
					}
					const int res = pCapacitorItem->CreatePowerCable(record , param);
					if(ERROR_SUCCESS == res)
					{
						++m_nSuccessCount;
						//! Cable을 생성한 Capacitor를 리스트에 넣어 Result View에 표시한다.
						CCableSizingResultDoc::m_InterestingLoadItemList.push_back(pCapacitorItem);
					}else	++m_nFailCount;
				}
				else
				{
					ELOAD_LOG4CXX_ERROR(mylogger , rCapacitorID , 47);
					++m_nFailCount;
				}
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	UPS/DC/용 Cable을 생성한다.

	@author BHK	

	@date 2009-07-29 오전 9:27:41	

	@param	

	@return		
*/
int COMMAND::CCommandPowerCableCreation::CreateCableForUPSDC(vector<CCableCreationTable::Record>& RecordList , double nMinSize)
{
	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
	CString rTitle = _T("Calculate Cable Sizing Result...");

	CELoadDocData& docData = CELoadDocData::GetInstance();

	//! UPS/DC
	list <CELoadItem*> UpsDcItemList;
	docData.GetELoadItemListOf(UpsDcItemList, CUPSDCItem::TypeString());
	{
		for(list<CELoadItem*>::iterator itr = UpsDcItemList.begin(); itr != UpsDcItemList.end();++itr)
		{
			int progress = int((double(m_nIndex) / double(m_nTotalCount))*100.);
			if(pDlg) pDlg->UpdateWorkStatus(rTitle , progress);
			++m_nIndex;

			CUPSDCItem* pUpsDcItem = static_cast<CUPSDCItem*>(*itr);
			if(pUpsDcItem)
			{
				list<CCableItem*> CableItemList;
				pUpsDcItem->GetCableList(CableItemList);

				//! UNSIZED POWER CABLE ONLY일때 EQ가 Cable을 가지고 있으면 생성을 시도하지 않는다.
				if(CCableSizingResultDoc::UNSIZED_POWER_CABLE_ONLY == m_nPowerCableRadio)
				{
					if(!CableItemList.empty()) continue;
				}
				else
				{
					//! ALL Power cable 일때 기존의 Cable을 삭제한다.
					if(!CableItemList.empty())
					{
						for(list<CCableItem*>::iterator jtr = CableItemList.begin(); jtr != CableItemList.end(); ++jtr)
						{
							if(*jtr)
							{
								CCableItem::DeleteInstance(*jtr);
							}
						}
					}
				}
				++m_nTryCount;

				double nFlc = 0.f;
				const string rUpsDcID = pUpsDcItem->GetName();
				const string rFrom1 = pUpsDcItem->prop()->GetValue(_T("From1"), _T("Bus ID"));
				const string rFrom2 = pUpsDcItem->prop()->GetValue(_T("From2"), _T("Bus ID"));
				const string rPF = pUpsDcItem->prop()->GetValue(_T("General") , _T("PF"));
				const string rCableLength = pUpsDcItem->prop()->GetValue(_T("General") , _T("Cable Length"));
				
				const string rCableName1 = GetCableNameFor(rUpsDcID , rFrom1);
				const string rCableName2 = GetCableNameFor(rUpsDcID , rFrom2);
				if(!rPF.empty() && !rCableLength.empty() && !rCableName1.empty() && !rCableName2.empty())
				{
					const string rFromBusVolt1 = pUpsDcItem->prop()->GetValue(_T("From1") , _T("Bus Volt")).c_str();
					const double nFromBusVolt1 = atof(rFromBusVolt1.c_str());
					CCableCreationTable::Record record[3];
					string rPhase[3];
					if(ERROR_SUCCESS != GetPowerCableCreationSettingRecordForEqPowerCable(rFrom1 , record[0] , rPhase[0]))
					{
						ELOAD_LOG4CXX_ERROR(mylogger , rUpsDcID , 46);
						++m_nFailCount;
						continue;
					}
					ParamForPowerCableCreation param[2];
					{
						param[0].rPhase = rPhase[0];
						param[0].nBusVolt = nFromBusVolt1;
						//! 현재는 UPS Capacitiy인 경우에서만 FLC를 계산했음.
						///const string rKVA = pUpsDcItem->prop()->GetValue(_T("UPS Capacity"), _T("KVA"));
						param[0].nLoadVolt = pUpsDcItem->GetLoadVoltForPowerCableCreation(0);
						param[0].nFLC = pUpsDcItem->GetFLCForPowerCableCreation(1);
						param[0].rSpec = pUpsDcItem->prop()->GetValue(_T("From2"), _T("Cable Spec"));
						param[0].rFrom = pUpsDcItem->prop()->GetValue(_T("From1"), _T("Bus ID"));
						param[0].rTo = pUpsDcItem->GetName();
						param[0].rPanelID = pUpsDcItem->prop()->GetValue(_T("From1"), _T("Bus ID"));
						param[0].rLoadID = pUpsDcItem->GetName();
						param[0].rArea = m_rAreaNameForEquip;
					}

					const string rFrom2BusVolt = pUpsDcItem->prop()->GetValue(_T("From2") , _T("Bus Volt")).c_str();
					const double nFrom2BusVolt = atof(rFrom2BusVolt.c_str());
					if(ERROR_SUCCESS != GetPowerCableCreationSettingRecordForEqPowerCable(rFrom2 , record[1] , rPhase[1]))
					{
						ELOAD_LOG4CXX_ERROR(mylogger , rUpsDcID , 46);
						++m_nFailCount;
						continue;
					}
					{
						param[1].rPhase = rPhase[1];
						param[1].nBusVolt = nFrom2BusVolt;
						//! 현재는 UPS Capacitiy인 경우에서만 FLC를 계산했음.
						///const string rKVA = pUpsDcItem->prop()->GetValue(_T("UPS Capacity"), _T("KVA"));
						param[1].nLoadVolt = pUpsDcItem->GetLoadVoltForPowerCableCreation(1);
						param[1].nFLC = pUpsDcItem->GetFLCForPowerCableCreation(1);
						param[1].rSpec = pUpsDcItem->prop()->GetValue(_T("From2"), _T("Cable Spec"));
						param[1].rFrom = pUpsDcItem->prop()->GetValue(_T("From2"), _T("Bus ID"));
						param[1].rTo = pUpsDcItem->GetName();
						param[1].rPanelID = pUpsDcItem->prop()->GetValue(_T("From2"), _T("Bus ID"));
						param[1].rLoadID = pUpsDcItem->GetName();
						param[1].rArea = m_rAreaNameForEquip;
					}
					
					/*
					const string rToBusID = pUpsDcItem->prop()->GetValue(_T("To") , _T("Bus ID")).c_str();
					if(ERROR_SUCCESS != GetPowerCableCreationSettingRecordForEqPowerCable(rToBusID , record[2] , rPhase[2]))
					{
						ELOAD_LOG4CXX_ERROR(mylogger , rUpsDcID , 46);
						++m_nFailCount;
						continue;
					}
					{
						param[2].rPhase = rPhase[2];
						param[2].nBusVolt = atof(pUpsDcItem->prop()->GetValue(_T("To") , _T("Bus Volt")).c_str());
						//! 현재는 UPS Capacitiy인 경우에서만 FLC를 계산했음.
						///const string rSelectedCapacity = pUpsDcItem->prop()->GetValue(_T("UPS Capacity"), _T("KVA"));
						param[2].nLoadVolt = pUpsDcItem->GetLoadVoltForPowerCableCreation(2);
						param[2].nFLC = pUpsDcItem->GetFLCForPowerCableCreation(2);
						param[2].rSpec = pUpsDcItem->prop()->GetValue(_T("From2"), _T("Cable Spec"));
						param[2].rFrom = pUpsDcItem->GetName();
						param[2].rTo = pUpsDcItem->prop()->GetValue(_T("To"), _T("Bus ID"));
						param[2].rPanelID = pUpsDcItem->prop()->GetValue(_T("To"), _T("Bus ID"));
						param[2].rLoadID = pUpsDcItem->GetName();
						param[2].rArea = m_rAreaNameForEquip;
					}
					*/

					int res[2] = {ERROR_BAD_ENVIRONMENT , ERROR_BAD_ENVIRONMENT};
					res[0] = pUpsDcItem->CreatePowerCable(record[0] , param[0]);
					res[1] = pUpsDcItem->CreatePowerCable(record[1] , param[1]);
					///res[2] = pUpsDcItem->CreatePowerCable(record[2] , param[2]);
					
					if((ERROR_SUCCESS == res[0]) && (ERROR_SUCCESS == res[1]))
					{
						++m_nSuccessCount;
						//! Cable을 생성한 UPS/DC를 리스트에 넣어 Result View에 표시한다.
						CCableSizingResultDoc::m_InterestingLoadItemList.push_back(pUpsDcItem);
					}
					else
					{
						//! Cable의 Load Id가 UPS DC의 ID와 같은 Cable은 삭제한다.
						list<CELoadItem*> CableItemList;
						docData.GetELoadItemListOf(CableItemList, CCableItem::TypeString());
						list<CELoadItem*>::iterator where = find_if(CableItemList.begin(), CableItemList.end(), bind2nd(MatchLoadId(), rUpsDcID));
						if(where != CableItemList.end())
						{
							CCableItem* pCableItem = static_cast<CCableItem*>(*where);
							if(pCableItem)
							{
								CCableItem::DeleteInstance(pCableItem);
							}
						}
						++m_nFailCount;
					}
				}
				else
				{
					if(rPF.empty() || rCableLength.empty())
						ELOAD_LOG4CXX_ERROR(mylogger , rUpsDcID , 51)
					else	ELOAD_LOG4CXX_ERROR(mylogger , rUpsDcID , 47);
					++m_nFailCount;
				}
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	Equipment Type의 Cable Name을 구한다.

	@author BHK	

	@date 2009-07-29 오후 3:42:18	

	@param	

	@return		
*/
string COMMAND::CCommandPowerCableCreation::GetCableNameFor(const string& rEqID , const string& rBusID)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string rMDBFilePath = docData.GetProjectMDBFilePath();

	CBusItem* pBusItem = docData.FindBusItemByName(rBusID);
	if(NULL == pBusItem) return string();
	const double nBusVoltage = atof(pBusItem->prop()->GetValue(_T("General") , _T("Bus Voltage")).c_str());
	const string rBusPhase = pBusItem->prop()->GetValue(_T("General") , _T("Phase"));

	CHVCableCreationSettingTable& HVVoltageSettingTable = CHVCableCreationSettingTable::GetInstance();
	HVVoltageSettingTable.Load(rMDBFilePath);
	for(vector<CCableCreationTable::Record>::iterator itr = HVVoltageSettingTable.m_RecordEntry.begin();itr != HVVoltageSettingTable.m_RecordEntry.end();++itr)
	{
		map<string,string>::iterator where = itr->FieldValueMap.find(_T("C_BUS_VOLTAGE"));
		if(where != itr->FieldValueMap.end())
		{
			if(where->second.empty()) continue;

			if((atof(where->second.c_str()) == nBusVoltage) && (_T("3") == rBusPhase))
			{
				const string rCableNamePrefix     = itr->FieldValueMap[_T("C_MAIN_POWER_CABLE_NAMING_PREFIX")];
				const string rCableNameSuffix     = itr->FieldValueMap[_T("C_MAIN_POWER_CABLE_NAMING_SUFFIX")];

				return (rCableNamePrefix + rEqID + rCableNameSuffix);
			}
		}
	}
	
	CLV3PHCableCreationSettingTable& LV3PHVoltageSettingTable = CLV3PHCableCreationSettingTable::GetInstance();
	LV3PHVoltageSettingTable.Load(rMDBFilePath);
	for(vector<CCableCreationTable::Record>::iterator itr = LV3PHVoltageSettingTable.m_RecordEntry.begin();itr != LV3PHVoltageSettingTable.m_RecordEntry.end();++itr)
	{
		map<string,string>::iterator where = itr->FieldValueMap.find(_T("C_BUS_VOLTAGE"));
		if(where != itr->FieldValueMap.end())
		{
			if(where->second.empty()) continue;

			const string rBusPhase = pBusItem->prop()->GetValue(_T("General") , _T("Phase"));
			if((atof(where->second.c_str()) == nBusVoltage) && (_T("3") == rBusPhase))
			{
				const string rCableNamePrefix     = itr->FieldValueMap[_T("C_MAIN_POWER_CABLE_NAMING_PREFIX")];
				const string rCableNameSuffix     = itr->FieldValueMap[_T("C_MAIN_POWER_CABLE_NAMING_SUFFIX")];

				return (rCableNamePrefix + rEqID + rCableNameSuffix);
			}
		}
	}
	
	CLV1PHCableCreationSettingTable& LV1PHVoltageSettingTable = CLV1PHCableCreationSettingTable::GetInstance();
	LV1PHVoltageSettingTable.Load(rMDBFilePath);
	for(vector<CCableCreationTable::Record>::iterator itr = LV1PHVoltageSettingTable.m_RecordEntry.begin();itr != LV1PHVoltageSettingTable.m_RecordEntry.end();++itr)
	{
		map<string,string>::iterator where = itr->FieldValueMap.find(_T("C_BUS_VOLTAGE"));
		if(where != itr->FieldValueMap.end())
		{
			if(where->second.empty()) continue;

			const string rBusPhase = pBusItem->prop()->GetValue(_T("General") , _T("Phase"));
			if((atof(where->second.c_str()) == nBusVoltage) && (_T("1") == rBusPhase))
			{
				const string rCableNamePrefix = itr->FieldValueMap[_T("C_MAIN_POWER_CABLE_NAMING_PREFIX")];
				const string rCableNameSuffix = itr->FieldValueMap[_T("C_MAIN_POWER_CABLE_NAMING_SUFFIX")];

				return (rCableNamePrefix + rEqID + rCableNameSuffix);
			}
		}
	}
	
	CDCCableCreationSettingTable& DCVoltageSettingTable = CDCCableCreationSettingTable::GetInstance();
	DCVoltageSettingTable.Load(rMDBFilePath);
	for(vector<CCableCreationTable::Record>::iterator itr = DCVoltageSettingTable.m_RecordEntry.begin();itr != DCVoltageSettingTable.m_RecordEntry.end();++itr)
	{
		map<string,string>::iterator where = itr->FieldValueMap.find(_T("C_BUS_VOLTAGE"));
		if(where != itr->FieldValueMap.end())
		{
			if(where->second.empty()) continue;

			const string rBusPhase = pBusItem->prop()->GetValue(_T("General") , _T("Phase"));
			if((atof(where->second.c_str()) == nBusVoltage) && (_T("DC") == rBusPhase))
			{
				const string rCableNamePrefix     = itr->FieldValueMap[_T("C_MAIN_POWER_CABLE_NAMING_PREFIX")];
				const string rCableNameSuffix     = itr->FieldValueMap[_T("C_MAIN_POWER_CABLE_NAMING_SUFFIX")];

				return (rCableNamePrefix + rEqID + rCableNameSuffix);
			}
		}
	}

	return string();
}

/**
	@brief	

	@author BHK	

	@date 2009-08-11 오후 1:35:45	

	@param	

	@return		
*/
int COMMAND::CCommandPowerCableCreation::GetPowerCableCreationSettingRecordForEqPowerCable(const string& rBusID , CCableCreationTable::Record& record , string& rPhase)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string& rMDBFilePath = docData.GetProjectMDBFilePath();

	string rBusVolt;
	CBusItem* pBusItem = docData.FindBusItemByName(rBusID);
	if(pBusItem)
	{
		rBusVolt = pBusItem->prop()->GetValue(_T("General") , _T("Bus Voltage"));
		rPhase   = pBusItem->prop()->GetValue(_T("General") , _T("Phase"));
	}
	list<CCableCreationTable::Record> _3PHCableCreationRecordList , _1PHCableCreationRecordList , _DCCableCreationRecordList;
	{
		//! 실제 RUNNING하는 부분(HV,LV3,LV1,DC순으로 RUNNING함)
		CHVCableCreationSettingTable& HVVoltageSettingTable = CHVCableCreationSettingTable::GetInstance();
		HVVoltageSettingTable.Load(rMDBFilePath);
		_3PHCableCreationRecordList.insert(_3PHCableCreationRecordList.begin() , HVVoltageSettingTable.m_RecordEntry.begin() , HVVoltageSettingTable.m_RecordEntry.end());

		CLV3PHCableCreationSettingTable& LV3PHVoltageSettingTable = CLV3PHCableCreationSettingTable::GetInstance();
		LV3PHVoltageSettingTable.Load(rMDBFilePath);
		_3PHCableCreationRecordList.insert(_3PHCableCreationRecordList.end() , LV3PHVoltageSettingTable.m_RecordEntry.begin() , LV3PHVoltageSettingTable.m_RecordEntry.end());

		CLV1PHCableCreationSettingTable& LV1PHVoltageSettingTable = CLV1PHCableCreationSettingTable::GetInstance();
		LV1PHVoltageSettingTable.Load(rMDBFilePath);
		_1PHCableCreationRecordList.insert(_1PHCableCreationRecordList.end() , LV1PHVoltageSettingTable.m_RecordEntry.begin() , LV1PHVoltageSettingTable.m_RecordEntry.end());

		CDCCableCreationSettingTable& DCVoltageSettingTable = CDCCableCreationSettingTable::GetInstance();
		DCVoltageSettingTable.Load(rMDBFilePath);
		_DCCableCreationRecordList.insert(_DCCableCreationRecordList.end() , DCVoltageSettingTable.m_RecordEntry.begin() , DCVoltageSettingTable.m_RecordEntry.end());
	}

	//! Cable Creation Setting Table에서 Bus Volt가 일치하는 Record를 찾는다.
	for(list<CCableCreationTable::Record>::iterator itr = _3PHCableCreationRecordList.begin();itr != _3PHCableCreationRecordList.end();++itr)
	{
		map<string,string>::iterator where1 = itr->FieldValueMap.find(_T("C_BUS_VOLTAGE"));
		map<string,string>::iterator where2 = itr->FieldValueMap.find(_T("C_PHASE"));
		if((where1 != itr->FieldValueMap.end()) && (where2 != itr->FieldValueMap.end()))
		{
			if(where1->second.empty() || where2->second.empty()) continue;
			if((where1->second == rBusVolt) && (where2->second == rPhase))
			{
				record = (*itr);
				return ERROR_SUCCESS;
			}
		}
	}
	
	for(list<CCableCreationTable::Record>::iterator itr = _1PHCableCreationRecordList.begin();itr != _1PHCableCreationRecordList.end();++itr)
	{
		map<string,string>::iterator where1 = itr->FieldValueMap.find(_T("C_BUS_VOLTAGE"));
		map<string,string>::iterator where2 = itr->FieldValueMap.find(_T("C_PHASE"));
		if((where1 != itr->FieldValueMap.end()) && (where2 != itr->FieldValueMap.end()))
		{
			if(where1->second.empty() || where2->second.empty()) continue;
			if((where1->second == rBusVolt) && (where2->second == rPhase))
			{
				record = (*itr);
				return ERROR_SUCCESS;
			}
		}
	}
	
	for(list<CCableCreationTable::Record>::iterator itr = _DCCableCreationRecordList.begin();itr != _DCCableCreationRecordList.end();++itr)
	{
		map<string,string>::iterator where1 = itr->FieldValueMap.find(_T("C_BUS_VOLTAGE"));
		map<string,string>::iterator where2 = itr->FieldValueMap.find(_T("C_PHASE"));
		if((where1 != itr->FieldValueMap.end()) && (where2 != itr->FieldValueMap.end()))
		{
			if(where1->second.empty() || where2->second.empty()) continue;
			if((where1->second == rBusVolt) && (where2->second == rPhase))
			{
				record = (*itr);
				return ERROR_SUCCESS;
			}
		}
	} 

	return ERROR_BAD_ENVIRONMENT;
}
