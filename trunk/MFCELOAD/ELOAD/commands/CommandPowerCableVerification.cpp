#include "StdAfx.h"
#include "..\ELoadDocData.h"
#include "../CableSizingResultDoc.h"
#include "..\Tables/CableCreationSettingTable.h"
#include "CommandPowerCableVerification.h"
#include "..\WorkStatusDlg.h"
#include "..\pyfromc.h"
#include "..\ELoad.h"
#include "..\CableSizingResultDoc.h"

using namespace COMMAND;

CCommandPowerCableVerification::CCommandPowerCableVerification(void) : m_nConduitLength(0.f) , m_nRunningCalculatedVD(0.f)
{
	m_nConductors_1 = 0.f;
	m_nConductors_2 = 0.f;
	m_nConductors_Over2 = 0.f;

	m_nCheckingArea = BEFORE;

	m_nLoadPowerCableRadio = CCableSizingResultDoc::ALL_POWER_CABLE;
}

CCommandPowerCableVerification::~CCommandPowerCableVerification(void)
{
}

/**
@brief	LoadSummary Thread Function.

@author	BHK
*/
UINT CCommandPowerCableVerification::StatusThreadEntry(LPVOID pVoid)
{
	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
	if(pDlg)
	{
		CCommandPowerCableVerification* p = (CCommandPowerCableVerification*)pVoid;
		p->StatusThread();
		
		InterlockedExchange((LONG*)(&(pDlg->m_bThreadRunning)) , FALSE);
		if(pDlg) pDlg->SendMessage(WM_COMMAND , IDOK);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	power cable을 생성합니다.

	@author	BHK
*/
int COMMAND::CCommandPowerCableVerification::Execute(void)
{	
	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
	if(NULL == pDlg)
	{
		CWorkStatusDlg dlg;
		dlg.m_pThread = AfxBeginThread(CCommandPowerCableVerification::StatusThreadEntry , this , THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
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
int CCommandPowerCableVerification::CreatePowerCableInBus(double nMinSize , CCableCreationTable::Record &SettingRecord , vector<CBusItem*>& BusItemEntry)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();
	CString rTitle = _T("Calculate Cable Sizing Result...");
	for(vector<CBusItem*>::iterator itr = BusItemEntry.begin();itr != BusItemEntry.end();++itr)
	{
		const string rBusVolt = (*itr)->prop()->GetValue(_T("General") , _T("Bus Voltage"));
		const double nBusVolt = atof(rBusVolt.c_str());

		const string rBusID = (*itr)->GetName();
		CItemSelectionSet SelectionSet;
		docData.GetLoadItemSetInBus(SelectionSet , rBusID);
		for(int i = 0;i < SelectionSet.GetItemCount();++i)
		{
			CLoadItem* pLoadItem = static_cast<CLoadItem*>(SelectionSet.GetItemAt(i));
			
			//! Cable을 생성할 Load를 리스트에 넣는다.
			///CPowerCableVerificationDoc::m_LoadItemListCreateUnsizedPowerCable.push_back(pLoadItem);

			const string rLoadID = pLoadItem->prop()->GetValue(_T("GENERAL") , _T("ITEM ID"));
			if(ERROR_SUCCESS == python.CallPyCheckLoadPropForPowerCableCreation(rLoadID))
			{
				const string rLoadCategory        = CString(pLoadItem->prop()->GetValue(_T("Type") , _T("Load Category")).c_str()).MakeUpper();
				const string rCableVolt		  = pLoadItem->prop()->GetValue(_T("Power Cable Setting"),_T("Cable Voltage"));
				const string rCableCore		  = pLoadItem->prop()->GetValue(_T("Power Cable Setting") , _T("Cable Core"));
				const string rCableType		  = pLoadItem->prop()->GetValue(_T("Power Cable Setting") , _T("Cable Type"));
				const string rRatingCapacityAbove = pLoadItem->prop()->GetValue(_T("Power Cable Setting") , _T("Rating Capacity Above"));
				const string rInstallMethod	  = pLoadItem->prop()->GetValue(_T("Cable Route") , _T("Install"));
				const string rSF		  = pLoadItem->prop()->GetValue(_T("Voltage Drop") , _T("SF For FLC"));
				const string rTotalDF		  = pLoadItem->prop()->GetValue(_T("Derating Factor") , _T("Total DF"));
				const double nFLC = pLoadItem->GetFLCForPowerCableCreation();
				
				const double nSF  = atof(rSF.c_str());
				const double nLRC = 0.01 * atof(pLoadItem->prop()->GetValue(_T("Characteristic") , _T("LRC")).c_str());
				
				const string rType = rCableVolt + "KV " + rCableCore + " CU" + " " + rCableType + " (eLoad)";
				CString rCableSize, rCableAmp;
				CCableDataByType* pCableDataType = docData.m_ProjectCableDataTable.GetRecordListOf(rType);
				if(NULL == pCableDataType) 
				{
					ELOAD_LOG4CXX_ERROR(mylogger , rLoadID , 14);
					continue;
				}

				vector<CCableDataByType::Record*>* pRecordList = pCableDataType->GetRecordEntryOfHertz();
				if(pRecordList && !pRecordList->empty())
				{
					int nNoOfCable = 1;

					vector<CCableDataByType::Record*>::iterator ktr;
					if(ERROR_BAD_ENVIRONMENT == pLoadItem->FindMinPowerCableSize(nNoOfCable , nMinSize , pRecordList , ktr))
					{
						ELOAD_LOG4CXX_ERROR(mylogger , rLoadID , 18);
						continue;
					}

					CString rMotorStartApply = SelectionSet.GetItemAt(i)->prop()->GetValue("Voltage Drop" , "Motor Start Apply").c_str();
					const string rPFatStarting = SelectionSet.GetItemAt(i)->prop()->GetValue("Characteristic" , "PF at Starting");
					const bool bNeedCheckMotorStartApply = (_T("YES") == rMotorStartApply.MakeUpper() && !rPFatStarting.empty());
					const double nPFatStarting = atof(rPFatStarting.c_str());
					
					if(ktr != pRecordList->end())
					{
						const double nPF = CCommandPowerCableVerification::GetPowerFactor((CLoadItem*)(SelectionSet.GetItemAt(i)));
						const string rRunningVD = GetRunVoltageDrop(pLoadItem);
						if(rRunningVD.empty())
						{
							ELOAD_LOG4CXX_ERROR(mylogger , SelectionSet.GetItemAt(i)->GetName() , 13);
							continue;
						}
						const double nRunVD = atof(rRunningVD.c_str());
						
						//! 먼저 Cable Item을 생성한다.
						CCableItem* pCableItem = CreatePowerCableItem(pLoadItem , SettingRecord);
						if(pCableItem)
						{
							pLoadItem->AddCable(pCableItem);
							pCableItem->SetLoadItemPtr(pLoadItem);
						}

						m_nRunningCalculatedVD = -1;
						m_nCheckingArea = BEFORE;
						//! 1. RunningVD가 맞는 Cable Size를 선정한다.
						if(ERROR_SUCCESS == FindPowerCableSize(pCableItem , pRecordList , ktr , pLoadItem , SettingRecord , 
							nPF , (nFLC*nSF) , (nBusVolt*nRunVD*0.01*1000) , nMinSize , nNoOfCable , m_nRunningCalculatedVD))
						{
							if((ktr != pRecordList->end()) && bNeedCheckMotorStartApply)
							{
								//! Motor Starting쪽 확인이 필요할때... 계산 결과를 담은 리스트를 클리어한다.
								pCableItem->ClearCalculatedResult();
								
								double nStartingCalculatedVD = 0.f;
								const string StartVD  = SelectionSet.GetItemAt(i)->prop()->GetValue("Voltage Drop" , "Motor Start VD");
								const double nStartVD = atof(StartVD.c_str());
								//! 2. StartVD가 맞는 Cable Size를 선정한다.(RunningVD도 맞도록 한다.) 
								if(ERROR_SUCCESS == FindPowerCableSize(pCableItem , pRecordList , ktr , pLoadItem , 
									SettingRecord , nPFatStarting , (nFLC*nLRC) , (nBusVolt*nStartVD*0.01*1000) , nMinSize , nNoOfCable , nStartingCalculatedVD))
								{
									//! RUNNING쪽의 VD를 다시 계산한다.
									const string rR = (*ktr)->m_FieldMap[_T("C_R")];
									const string rX = (*ktr)->m_FieldMap[_T("C_X")];
									const string rID		= pLoadItem->GetName();
									const string rPhase		= pLoadItem->prop()->GetValue("Rating" , "Phase");
									///const string rCategory	= pLoadItem->prop()->GetValue("Type" , "Load Category");
									const string rL			= pLoadItem->prop()->GetValue("Cable Route"  , "Design Length");
									const int res = python.CallPyFindPowerCableSize(rID , rPhase , rL ,
										rR , rX , nPF , (nFLC*nSF) , (nBusVolt*nRunVD*0.01*1000) , nNoOfCable , m_nRunningCalculatedVD);

									SetPowerCableProp(pCableItem->prop() , pLoadItem , SettingRecord , nNoOfCable , *ktr , m_nRunningCalculatedVD , nStartingCalculatedVD);
									CreateSpaceHeaterPowerCableIfNeed(pLoadItem , SettingRecord , pCableItem , rBusID);

									if(ktr == pRecordList->end())
									{
										ktr = pRecordList->begin();
										++nNoOfCable;
									}
									m_nCheckingArea = AFTER;
									//! 이후의 몇번의 계산을 수행하여 그 결과를 리스트에 담는다.
									FindPowerCableSize(pCableItem , pRecordList , ktr , pLoadItem , SettingRecord , nPFatStarting , 
										(nFLC*nLRC) , (nBusVolt*1000*nStartVD*0.01) , nMinSize , nNoOfCable , nStartingCalculatedVD);
								}else	CCableItem::DeleteInstance(pCableItem);
							}
							else if((ktr != pRecordList->end()) && !bNeedCheckMotorStartApply)
							{
								SetPowerCableProp(pCableItem->prop() , pLoadItem , SettingRecord , nNoOfCable , 
									*ktr , m_nRunningCalculatedVD , 0.f);
								CreateSpaceHeaterPowerCableIfNeed(pLoadItem , SettingRecord , pCableItem , rBusID);

								double nRunningCalculatedVD = 0.f;
								if(ktr == pRecordList->end())
								{
									ktr = pRecordList->begin();
									++nNoOfCable;
								}
								m_nCheckingArea = AFTER;
								//! 이후의 몇번의 계산을 수행하여 그 결과를 리스트에 담는다.
								FindPowerCableSize(pCableItem , pRecordList , ktr , pLoadItem , SettingRecord , 
									nPF , (nFLC*nSF) , (nBusVolt*1000*nRunVD*0.01) , nMinSize , nNoOfCable , nRunningCalculatedVD);
							}
						}else	CCableItem::DeleteInstance(pCableItem);

						AdjustCableNumbering(pLoadItem , SettingRecord , pCableItem);
					}
					else
					{
						//!
					}
				}
			}

			int progress = int((double(m_nIndex) / double(m_nTotalCount))*100.);
			if(pDlg) pDlg->UpdateWorkStatus(rTitle , progress);
			++m_nIndex;
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	조건에 맞는 Power Cable Size를 구합니다.

	@author	BHK
*/
int CCommandPowerCableVerification::FindPowerCableSize(CCableItem* pCableItem , 
	vector<CCableDataByType::Record*>* pRecordList , vector<CCableDataByType::Record*>::iterator& ktr , 
	CLoadItem* pLoadItem , CCableCreationTable::Record &SettingRecord , const double& nPF , const double& I , const double& nMotorVD , const double& nMinSize , int& nMultiply , double& nCalculatedVD)
{
	assert(pCableItem && pRecordList && pLoadItem && "pCableItem or pRecordList or pLoadItem is NULL");

	bool bFound = false;
	if((NULL != pCableItem) && (NULL != pRecordList) && (ktr != pRecordList->end()) & (NULL != pLoadItem))
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		const string rBusId = pLoadItem->prop()->GetValue(_T("Panel") , _T("Bus ID"));
		CBusItem* pBus = docData.FindBusItemByName(rBusId);
		const string rBusVolt = pBus->prop()->GetValue(_T("General") , _T("Bus Voltage"));
		const double nBusVolt = atof(rBusVolt.c_str());
		
		const double nPF100 = CCommandPowerCableVerification::GetPowerFactor(pLoadItem);
		const double nFLC   = pLoadItem->GetFLCForPowerCableCreation();
		const double nSF    = atof(pLoadItem->prop()->GetValue("Voltage Drop", "SF For FLC").c_str());
		const double nRunningVD = atof(GetRunVoltageDrop(pLoadItem).c_str());

		ostringstream oss;
		oss << nMotorVD;
		const string MotorVD = oss.str();

		if(AFTER == m_nCheckingArea)
		{
			//! 다음 레코드로 이동.
			++ktr;
			if(ktr == pRecordList->end())
			{
				ktr = pRecordList->begin();
				++nMultiply;
			}

			//! TRY 갯수만큼 시도한다.
			for(int i = 0;(i < CCableItem::TRY_CALCULATED_RESULTS);)
			{
				const string rSize = (*ktr)->m_FieldMap[_T("C_SIZE")].c_str();
				const double nSize = atof(rSize.c_str());
				if(nSize >= nMinSize)
				{
					const string rR    = (*ktr)->m_FieldMap[_T("C_R")].c_str();
					const string rX    = (*ktr)->m_FieldMap[_T("C_X")].c_str();
					const string rID		= pLoadItem->GetName();
					const string rPhase		= pLoadItem->prop()->GetValue("Rating" , "Phase");
					///const string rCategory	= pLoadItem->prop()->GetValue("Type" , "Load Category");
					const string rL			= pLoadItem->prop()->GetValue("Cable Route"  , "Design Length");
					const int res = python.CallPyFindPowerCableSize(rID , rPhase , rL ,
						rR , rX , nPF , I , nMotorVD , nMultiply , nCalculatedVD);
					CELoadItemProp* pCalculationResultProp = pCableItem->GetPropForOverCalculatedResult();
					if(pCalculationResultProp)
					{
						if(-1 == m_nRunningCalculatedVD)
						{
							SetPowerCableProp(pCalculationResultProp , pLoadItem , SettingRecord , nMultiply , *ktr , nCalculatedVD , 0.);
						}
						else
						{
							SetPowerCableProp(pCalculationResultProp , pLoadItem , SettingRecord , nMultiply , *ktr , m_nRunningCalculatedVD , nCalculatedVD);
						}
					}
					++i;
				}

				++ktr;
				if(ktr == pRecordList->end())
				{
					ktr = pRecordList->begin();
					++nMultiply;
				}
			}

			return ERROR_SUCCESS;
		}
		else
		{
			while(false == bFound)
			{
				for(;ktr != pRecordList->end();++ktr)
				{
					const string rSize = (*ktr)->m_FieldMap[_T("C_SIZE")].c_str();
					const double nSize = atof(rSize.c_str());
					if(nSize < nMinSize) continue;

					const string rR    = (*ktr)->m_FieldMap[_T("C_R")].c_str();
					const string rX    = (*ktr)->m_FieldMap[_T("C_X")].c_str();

					double _nCalculatedVD = -1;
					int res[2] = {ERROR_SUCCESS , ERROR_SUCCESS};
					
					const string rID		= pLoadItem->GetName();
					const string rPhase		= pLoadItem->prop()->GetValue("Rating" , "Phase");
					///const string rCategory	= pLoadItem->prop()->GetValue("Type" , "Load Category");
					const string rL			= pLoadItem->prop()->GetValue("Cable Route"  , "Design Length");
					res[0] = python.CallPyFindPowerCableSize(rID , rPhase , rL ,
						rR , rX , nPF , I , nMotorVD , nMultiply , _nCalculatedVD);
					CELoadItemProp* pCalculationResultProp = pCableItem->GetPropForBelowCalculatedResult();
					if(pCalculationResultProp)
					{
						if(-1 == m_nRunningCalculatedVD)
						{
							SetPowerCableProp(pCalculationResultProp , pLoadItem , SettingRecord , nMultiply , *ktr , _nCalculatedVD , 0.);
						}
						else
						{
							//! RUNNING쪽의 VD를 다시 계산한다.(running쪽도 만족해야 한다.)
							const string rID		= pLoadItem->GetName();
							const string rPhase		= pLoadItem->prop()->GetValue("Rating" , "Phase");
							///const string rCategory	= pLoadItem->prop()->GetValue("Type" , "Load Category");
							const string rL			= pLoadItem->prop()->GetValue("Cable Route"  , "Design Length");
							res[1] = python.CallPyFindPowerCableSize(rID , rPhase , rL ,
								rR , rX , nPF100 , (nFLC*nSF) , (nBusVolt*nRunningVD*0.01*1000) , nMultiply , m_nRunningCalculatedVD);

							SetPowerCableProp(pCalculationResultProp , pLoadItem , SettingRecord , nMultiply , *ktr , m_nRunningCalculatedVD , _nCalculatedVD);
						}
					}

					if((ERROR_SUCCESS == res[0]) && (ERROR_SUCCESS == res[1]))
					{
						nCalculatedVD = _nCalculatedVD;
						bFound = true;
						break;
					}
				}
				if(true == bFound) break;
				
				ktr = pRecordList->begin();
				++nMultiply;
			}
		}
	}

	return (true == bFound) ? ERROR_SUCCESS : ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	Power Cable Item의 속성을 설정합니다.

	@author	BHK
*/
int CCommandPowerCableVerification::SetPowerCableProp( CELoadItemProp* pItemProp ,
	CLoadItem* pLoadItem , CCableCreationTable::Record &SettingRecord , const int& nNoOfCable , CCableDataByType::Record* pRecord ,
	const double& nRunningVD , const double& nStartingVD)
{
	assert(pItemProp && pLoadItem && pRecord && "pItemProp or pLoadItem or pRecord is NULL");

	if(pItemProp && pLoadItem && pRecord)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		const string rBusId = pLoadItem->prop()->GetValue(_T("Panel") , _T("Bus ID"));
		CBusItem* pBus = docData.FindBusItemByName(rBusId);
		const string rBusVolt = pBus->prop()->GetValue(_T("General") , _T("Bus Voltage"));
		const double nBusVolt = atof(rBusVolt.c_str());

		const string rVoltType            = SettingRecord.FieldValueMap[_T("C_VOLTAGE_TYPE")];
		const string rCableNamePrefix     = SettingRecord.FieldValueMap[_T("C_MAIN_POWER_CABLE_NAMING_PREFIX")];
		const string rCableNameSuffix     = SettingRecord.FieldValueMap[_T("C_MAIN_POWER_CABLE_NAMING_SUFFIX")];
		
		const string rLoadName = pLoadItem->GetName();
		const string rCableName= rCableNamePrefix + rLoadName + rCableNameSuffix;

		static const string GENERAL=_T("General");
		pItemProp->SetValue(GENERAL , _T("CABLE ID") , rCableName);
		pItemProp->SetOriginalValue(GENERAL , _T("CABLE ID") , rCableName);

		pItemProp->SetValue(GENERAL , _T("Load Id") , pLoadItem->GetName());
		pItemProp->SetOriginalValue(GENERAL , _T("Load Id") , pLoadItem->GetName());

		pItemProp->SetValue(GENERAL , _T("Cable Category") , _T("Power Cable"));
		pItemProp->SetOriginalValue(GENERAL , _T("Cable Category") , _T("Power Cable"));

		pItemProp->SetValue(GENERAL , _T("Volt Type") , rVoltType);
		pItemProp->SetOriginalValue(GENERAL , _T("Volt Type") , rVoltType);

		const string rCableVolt = pLoadItem->prop()->GetValue(_T("Power Cable Setting") , _T("Cable Voltage"));
		pItemProp->SetValue(GENERAL , _T("Cable Volt") , rCableVolt);
		pItemProp->SetOriginalValue(GENERAL , _T("Cable Volt") , rCableVolt);

		const string rCableType = pLoadItem->prop()->GetValue(_T("Power Cable Setting"), _T("Cable Type"));
		pItemProp->SetValue(GENERAL , _T("Cable Type"), rCableType);
		pItemProp->SetOriginalValue(GENERAL , _T("Cable Type"), rCableType);

		const string rCableSpec = pLoadItem->prop()->GetValue(_T("Power Cable Setting") , _T("Cable Spec"));
		pItemProp->SetValue(GENERAL , _T("Cable Spec") , rCableSpec);
		pItemProp->SetOriginalValue(GENERAL , _T("Cable Spec") , rCableSpec);

		const string rCableCore = pLoadItem->prop()->GetValue(_T("Power Cable Setting") , _T("Cable Core"));
		pItemProp->SetValue(GENERAL , _T("Core") , rCableCore);
		pItemProp->SetOriginalValue(GENERAL , _T("Core") , rCableCore);

		const string rCableSize= pRecord->m_FieldMap[_T("C_SIZE")];
		pItemProp->SetValue(GENERAL , _T("Size") , rCableSize);
		pItemProp->SetOriginalValue(GENERAL , _T("Size") , rCableSize);

		CString rString;
		rString.Format("%d" , nNoOfCable);
		pItemProp->SetValue(GENERAL , _T("No Of Cables/Phase") , rString.operator LPCSTR());
		pItemProp->SetOriginalValue(GENERAL , _T("No Of Cables/Phase") , rString.operator LPCSTR());

		stringstream oss;
		oss << SAFE_ROUND(nRunningVD,2);
		pItemProp->SetValue(GENERAL , _T("Running VD") , oss.str());
		pItemProp->SetOriginalValue(GENERAL , _T("Running VD") , oss.str());
		oss.str(""); oss << SAFE_ROUND((nRunningVD/(nBusVolt*1000)*100),2);
		pItemProp->SetValue(GENERAL , _T("Running VD%") , oss.str());
		pItemProp->SetOriginalValue(GENERAL , _T("Running VD%") , oss.str());
		oss.str(""); oss << GetRunVoltageDrop(pLoadItem);
		pItemProp->SetValue(GENERAL , _T("Allow Running VD%") , oss.str());
		pItemProp->SetOriginalValue(GENERAL , _T("Allow Running VD%") , oss.str());
		if(0. != nStartingVD)
		{
			oss.str(""); oss << SAFE_ROUND(nStartingVD,2);
			pItemProp->SetValue(GENERAL , _T("Starting VD") , oss.str());
			pItemProp->SetOriginalValue(GENERAL , _T("Starting VD") , oss.str());
			oss.str(""); oss << SAFE_ROUND((nStartingVD/(nBusVolt*1000)*100),2);
			pItemProp->SetValue(GENERAL , _T("Starting VD%") , oss.str());
			pItemProp->SetOriginalValue(GENERAL , _T("Starting VD%") , oss.str());
			oss.str(""); oss << pLoadItem->prop()->GetValue(_T("Voltage Drop") , _T("Motor Start VD"));
			pItemProp->SetValue(GENERAL , _T("Allow Starting VD%") , oss.str());
			pItemProp->SetOriginalValue(GENERAL , _T("Allow Starting VD%") , oss.str());
		}

		const string rDesignLength = pLoadItem->prop()->GetValue(_T("Cable Route") , _T("Design Length"));
		pItemProp->SetValue(GENERAL , _T("Length") , rDesignLength);
		pItemProp->SetOriginalValue(GENERAL , _T("Length") , rDesignLength);

		const string rInstall = pLoadItem->prop()->GetValue(_T("Cable Route") , _T("Install"));

		const string rAmpG = pRecord->m_FieldMap[_T("C_AMP_G")];
		const string rAmpA = pRecord->m_FieldMap[_T("C_AMP_A")];
		const string rR = pRecord->m_FieldMap[_T("C_R")];
		const string rX = pRecord->m_FieldMap[_T("C_X")];
		const string rCableWeight = pRecord->m_FieldMap[_T("C_CABLE_WEIGHT")];
		const string rDia = pRecord->m_FieldMap[_T("C_DIA")];
		const string rTotalDeratingFactor = SettingRecord.FieldValueMap[_T("C_TOTAL_DERATING_FACTOR")];

		//! CABLE SPEC
		static const string CABLE_SPEC = _T("Cable Spec");
		pItemProp->SetValue(CABLE_SPEC , _T("R") , rR);
		pItemProp->SetOriginalValue(CABLE_SPEC , _T("R") , rR);
		pItemProp->SetValue(CABLE_SPEC , _T("X") , rX);
		pItemProp->SetOriginalValue(CABLE_SPEC , _T("X") , rX);
		pItemProp->SetValue(CABLE_SPEC , _T("Weight") , rCableWeight);
		pItemProp->SetOriginalValue(CABLE_SPEC , _T("Weight") , rCableWeight);

		if(_T("U/G") == rInstall)
		{
			pItemProp->SetValue(CABLE_SPEC , _T("Nominal Current") , rAmpG);
			pItemProp->SetOriginalValue(CABLE_SPEC , _T("Nominal Current") , rAmpG);
		}
		else
		{
			pItemProp->SetValue(CABLE_SPEC , _T("Nominal Current") , rAmpA);
			pItemProp->SetOriginalValue(CABLE_SPEC , _T("Nominal Current") , rAmpA);
		}
		pItemProp->SetValue(CABLE_SPEC , _T("Derating Factor") , rTotalDeratingFactor);
		pItemProp->SetOriginalValue(CABLE_SPEC , _T("Derating Factor") , rTotalDeratingFactor);

		pItemProp->SetValue(CABLE_SPEC , _T("Cable OD") , rDia);
		pItemProp->SetOriginalValue(CABLE_SPEC , _T("Cable OD") , rDia);
		//!

		static const string CONDUIT = _T("Conduit");
		const double nCableSize = atof(rCableSize.c_str());
		for(vector<CCableLengthDataSheet::ConduitSizeData>::iterator itr = m_ConduitSizeList.begin();itr != m_ConduitSizeList.end();++itr)
		{
			if(nCableSize == itr->CableSize)
			{
				stringstream oss;
				oss << (itr->ConduitSize);

				pItemProp->SetValue(CONDUIT , _T("Conduit Size") , oss.str());
				pItemProp->SetOriginalValue(CONDUIT , _T("Conduit Size") , oss.str());
				break;
			}
		}
		
		oss.str("");  oss << m_nConduitLength;
		pItemProp->SetValue(CONDUIT , _T("Conduit Length") , oss.str());
		pItemProp->SetOriginalValue(CONDUIT , _T("Conduit Length") , oss.str());

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	

	@author	BHK
*/
int COMMAND::CCommandPowerCableVerification::ExecuteForEachRecord(vector<CBusItem*>& BusItemEntry , vector<CCableCreationTable::Record>& RecordList, double nMinSize)
{
	for(vector<CCableCreationTable::Record>::iterator itr = RecordList.begin();itr != RecordList.end();++itr)
	{
		map<string,string>::iterator where = itr->FieldValueMap.find(_T("C_BUS_VOLTAGE"));
		if(where != itr->FieldValueMap.end())
		{
			if(where->second.empty()) continue;

			vector<CBusItem*> SelectedBusItemEntry;
			for(vector<CBusItem*>::iterator jtr = BusItemEntry.begin();jtr != BusItemEntry.end();++jtr)
			{
				const string rValue = (*jtr)->prop()->GetValue(_T("General") , _T("Bus Voltage"));
				if(atof(where->second.c_str()) == atof(rValue.c_str())) SelectedBusItemEntry.push_back(*jtr);
			}

			if(!SelectedBusItemEntry.empty())
			{
				const string rCableVolt = itr->FieldValueMap[_T("C_CABLE_VOLTAGE")];
				const string rCableCore = itr->FieldValueMap[_T("C_CABLE_CORE")];
				const string rCableType = itr->FieldValueMap[_T("C_CABLE_TYPE")];
				const string rType = rCableVolt + "KV " + rCableCore + " CU" + " " + rCableType + " (eLoad)";
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
*/
CCableItem* COMMAND::CCommandPowerCableVerification::CreatePowerCableItem(CLoadItem* pLoadItem , CCableCreationTable::Record &SettingRecord)
{
	assert(pLoadItem && "pLoadItem is NULL");
	CCableItem* pCableItem = NULL;

	if(pLoadItem)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		pCableItem = static_cast<CCableItem*>(docData.CreateELoadItemOf(CCableItem::TypeString()));
	}

	return pCableItem;
}

/**
	@brief	

	@author	BHK
*/
int COMMAND::CCommandPowerCableVerification::StatusThread(void)
{
	//! Unsized Power Cable을 생성할 Load를 담을 List를 클리어한다.
	CCableSizingResultDoc::m_InterestingLoadItemList.clear();

	vector<CBusItem*> BusItemEntry;
	CELoadDocData& docData = CELoadDocData::GetInstance();
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
			m_nLoadPowerCableRadio = atoi(rValue.c_str());
		}
	}

	list<CELoadItem*> ItemList;
	docData.GetELoadItemListOf(ItemList , CLoadItem::TypeString());
	m_nTotalCount = count_if(ItemList.begin() , ItemList.end() , CELoadItem::IsNotDeleted);
	m_nIndex = 0;

	//!
	CHVCableCreationSettingTable& HVVoltageSettingTable = CHVCableCreationSettingTable::GetInstance();
	ExecuteForEachRecord(BusItemEntry , HVVoltageSettingTable.m_RecordEntry , nMinHvSize);
	
	CLV3PHCableCreationSettingTable& LV3PHVoltageSettingTable = CLV3PHCableCreationSettingTable::GetInstance();
	ExecuteForEachRecord(BusItemEntry , LV3PHVoltageSettingTable.m_RecordEntry , nMinLvSize);
	
	CLV1PHCableCreationSettingTable& LV1PHVoltageSettingTable = CLV1PHCableCreationSettingTable::GetInstance();
	ExecuteForEachRecord(BusItemEntry , LV1PHVoltageSettingTable.m_RecordEntry , nMinLvSize);
	
	CDCCableCreationSettingTable& DCVoltageSettingTable = CDCCableCreationSettingTable::GetInstance();
	ExecuteForEachRecord(BusItemEntry , DCVoltageSettingTable.m_RecordEntry , nMinLvSize);

	return ERROR_SUCCESS;
}
