#include "StdAfx.h"
#include "ELoad.h"
#include "UPSDCItem.h"
#include "ELoadDocData.h"
#include "pyfromc.h"

using namespace ITEMS;

IMPLEMENTS_ELOAD_ITEM(CUPSDCItem , CELoadItem , "UPSDC")

CUPSDCItem::CUPSDCItem(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CELoadItemProp* pItemProp = docData.m_ItemPropMap[CUPSDCItem::TypeString()];
	if(pItemProp)
	{
		m_pProp = pItemProp->Clone();
		m_pProp->SetOwner(this);

		//! UPS/DC의 기본값을 설정한다.
		prop()->SetOriginalValue(_T("General") , _T("SELECTED CAPACITY") , _T("UPS Capacity"));
		prop()->SetValue(_T("General") , _T("SELECTED CAPACITY") , _T("UPS Capacity"));

		prop()->SetOriginalValue(_T("From1") , _T("Operation Mode") , _T("CONTINUOUS"));
		prop()->SetValue(_T("From1") , _T("Operation Mode") , _T("CONTINUOUS"));

		prop()->SetOriginalValue(_T("From2") , _T("Operation Mode") , _T("CONTINUOUS"));
		prop()->SetValue(_T("From2") , _T("Operation Mode") , _T("CONTINUOUS"));

		prop()->SetOriginalValue(_T("General") , _T("PF") , _T("0.85"));
		prop()->SetValue(_T("General") , _T("PF") , _T("0.85"));
		prop()->SetOriginalValue(_T("General") , _T("Cable Length") , _T("80"));
		prop()->SetValue(_T("General") , _T("Cable Length") , _T("80"));
	}
	else
	{
		AfxMessageBox(_T("UPS/DC Property를 구할수 없습니다."));
	}
}

CUPSDCItem::~CUPSDCItem(void)
{
}

/**
	@brief

	@author	KHS
*/
int CUPSDCItem::SetName( const string& name)
{
	CELoadItemProp* pProp = prop();
	if(pProp) return pProp->SetValue("General" , "UPSDC ID" , name);

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief

	@author	KHS
*/
string CUPSDCItem::TableName()
{
	static const string __TABLE_NAME__ = _T("T_UPSDC_ITEM");

	return __TABLE_NAME__;
}

/**
	@brief

	@author	KHS
*/
string CUPSDCItem::GetName() const
{
	CELoadItemProp* pProp = prop();
	if(pProp) return pProp->GetValue("General" , "UPSDC ID");

	return _T("");
}

/**
	@brief	

	@author BHK	

	@date 2009-06-15 오후 1:20:56	

	@param	

	@return		
*/
int ITEMS::CUPSDCItem::GetLoadSummaryCalcData(double nFactors[3] , double nKW[3] , double nKVAR[3] , const string& rBusID)
{
	string rBHP , rPF , rKVA;
	const string rSelectedCapacity = prop()->GetValue(_T("General") , _T("SELECTED CAPACITY"));
	if(_T("UPS Capacity") == rSelectedCapacity)
	{
		rBHP = prop()->GetValue(rSelectedCapacity , _T("BHP"));
		rPF  = prop()->GetValue(_T("General") , _T("PF"));
		rKVA = prop()->GetValue(rSelectedCapacity , _T("KVA"));

		if(rBHP.empty() || (0. == atof(rBHP.c_str())))
		{
			rBHP = rKVA;
			prop()->SetValue(rSelectedCapacity , _T("BHP") , rBHP);
		}
	}
	else if(_T("DC Capacity") == rSelectedCapacity)
	{
		rBHP = prop()->GetValue(rSelectedCapacity , _T("AC Rating BHP"));
		rPF  = prop()->GetValue(_T("General") , _T("PF"));
		rKVA = prop()->GetValue(rSelectedCapacity , _T("AC Rating KVA"));

		if(rBHP.empty() || (0. == atof(rBHP.c_str())))
		{
			rBHP = rKVA;
			prop()->SetValue(rSelectedCapacity , _T("AC Rating BHP") , rBHP);
		}
	}

	string rOP;
	const string rFrom1BusID = prop()->GetValue(_T("From1") , _T("Bus ID"));
	const string rFrom2BusID = prop()->GetValue(_T("From2") , _T("Bus ID"));
	if(rFrom1BusID == rBusID)
	{
		rOP = prop()->GetValue(_T("From1") , _T("Operation Mode"));
	}
	else if(rFrom2BusID == rBusID)
	{
		rOP = prop()->GetValue(_T("From2") , _T("Operation Mode"));
	}
	
	const double nKVA = atof(rKVA.c_str());
	
	//! BHP값이 0이면 KVA값으로 대체한다.
	double nBHP = atof(rBHP.c_str());
	if(0.0 == nBHP) nBHP = nKVA;
	
	nFactors[0] = (0. != nKVA) ? (nBHP / nKVA) : 0.;	//! LF
	nFactors[1] = atof(rPF.c_str());			//! PF
	nFactors[2] = 1.f;					//! EFF

	//! 매개변수 초기화...
	memset(nKW   , 0 , sizeof(double)*3);
	memset(nKVAR , 0 , sizeof(double)*3);
	if(rOP == _T("CONTINUOUS"))
	{
		nKW[0]   = nBHP * atof(rPF.c_str());
		nKVAR[0] = sqrt((nBHP * nBHP) - (nKW[0]*nKW[0]));
	}
	else if(rOP == _T("INTERMITTENT"))
	{
		nKW[1]   = nBHP * atof(rPF.c_str());
		nKVAR[1] = sqrt((nBHP * nBHP) - (nKW[1]*nKW[1]));
	}
	else if(rOP == _T("STAND-BY"))
	{
		nKW[2]   = nBHP * atof(rPF.c_str());
		nKVAR[2] = sqrt((nBHP * nBHP) - (nKW[2]*nKW[2]));
	}

	//! 소수점 2,3째 자리로 맞춤.
	nFactors[0] = SAFE_ROUND(nFactors[0] , 2);	//! LF
	nFactors[1] = SAFE_ROUND(nFactors[1] , 3);	//! PF
	nFactors[2] = SAFE_ROUND(nFactors[2] , 3);	//! EFF

	return ERROR_SUCCESS;
}

/**
	@brief	UPS/DC에 속한 Cable들을 구한다.

	@author BHK	

	@date 2009-07-29 오전 10:40:49	

	@param	

	@return		
*/
int ITEMS::CUPSDCItem::GetCableList(list<CCableItem*>& CableItemList)
{
	const string rUPSDCID = GetName();

	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> _CableItemList;
	docData.GetELoadItemListOf(_CableItemList , CCableItem::TypeString());
	for(list<CELoadItem*>::iterator itr = _CableItemList.begin();itr != _CableItemList.end();++itr)
	{
		CCableItem* pCableItem = static_cast<CCableItem*>(*itr);
		const string rLoadID = (*itr)->prop()->GetValue(_T("General") , _T("Load Id"));
		if(rUPSDCID == rLoadID) CableItemList.push_back(pCableItem);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-08-11 오전 9:24:50	

	@param	

	@return		
*/
int ITEMS::CUPSDCItem::CreatePowerCable(TABLES::CCableCreationTable::Record &SettingRecord , const ParamForPowerCableCreation& param)
{
	m_param = param;
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		const string rMDBFilePath = docData.GetProjectMDBFilePath();

		const string rID = prop()->GetValue(_T("GENERAL") , _T("UPSDC ID"));
		const string rCableVolt		  = SettingRecord.FieldValueMap[_T("C_CABLE_VOLTAGE")];
		const string rCableCore		  = SettingRecord.FieldValueMap[_T("C_CABLE_CORE")];
		const string rCableType		  = SettingRecord.FieldValueMap[_T("C_CABLE_TYPE")];
		const string rSF		  = SettingRecord.FieldValueMap[_T("C_SF")];
		const double nSF  = atof(rSF.c_str());
		const string rTotalDF		  = SettingRecord.FieldValueMap[_T("C_TOTAL_DERATING_FACTOR")];
		const double nFLC = param.nFLC;
		
		const string rType = rCableVolt + _T("KV ") + rCableCore + _T(" CU") + _T(" ") + rCableType + _T(" (eLoad)");
		CString rCableSize, rCableAmp;

		//! Cable Data Table에서 Cable Type에 일치하는 Cable Data Record를 구한다.
		CCableDataByType* pCableDataType = docData.m_ProjectCableDataTable.GetRecordListOf(rType);
		if(NULL == pCableDataType) 
		{
			ELOAD_LOG4CXX_ERROR(mylogger , rID , 14);
			return ERROR_BAD_ENVIRONMENT;
		}

		vector<CCableDataByType::Record*>* pRecordList = pCableDataType->GetRecordEntryOfHertz();
		if(pRecordList && !pRecordList->empty())
		{
			int nNoOfCable = 1;
			double nMinSize = -1.f;

			vector<CCableDataByType::Record*>::iterator ktr;
			if(ERROR_BAD_ENVIRONMENT == FindMinPowerCableSize(SettingRecord , nNoOfCable , nMinSize , pRecordList , ktr))
			{
				ELOAD_LOG4CXX_ERROR(mylogger , rID , 18);
				return ERROR_BAD_ENVIRONMENT;
			}

			if(ktr != pRecordList->end())
			{
				const double nPF = GetPFForPowerCableCreation();
				const string rRunningVD = GetRunVoltageDrop(SettingRecord);
				if(rRunningVD.empty())
				{
					ELOAD_LOG4CXX_ERROR(mylogger , rID, 13);
					return ERROR_BAD_ENVIRONMENT;
				}
				const double nRunVD = atof(rRunningVD.c_str());

				//! 먼저 Cable Item을 생성한다.
				const string rFromBusID = prop()->GetValue(_T("From") , _T("Bus Id"));
				const string rTRID = prop()->GetValue(_T("General") , _T("TR ID"));
				CCableItem* pCableItem = CCommandPowerCableCreation::CreatePowerCableItem(rFromBusID , rTRID , rFromBusID , rTRID , SettingRecord);
				if(pCableItem)
				{
					InitializeCableItemProperties(pCableItem);

					///AddCable(pCableItem);
					///pCableItem->SetLoadItemPtr(this);
				}else	return ERROR_BAD_ENVIRONMENT;

				CCommandPowerCableCreation::m_nRunningCalculatedVD = -1;
				CCommandPowerCableCreation::m_nCheckingArea = CCommandPowerCableCreation::BEFORE;
				
				{
					m_param.nPF = nPF;
					m_param.I = (nFLC*nSF);
					m_param.nMotorVD = (m_param.nBusVolt*nRunVD*0.01*1000);
					m_param.nMinSize = nMinSize;
					m_param.rPhase   = m_param.rPhase;
					m_param.nFLC = nFLC;
				}
				//! 1. RunningVD가 맞는 Cable Size를 선정한다.
				if(ERROR_SUCCESS == FindPowerCableSize(pCableItem , pRecordList , ktr , SettingRecord , 
					nNoOfCable , CCommandPowerCableCreation::m_nRunningCalculatedVD , m_param))
				{
					if(ktr != pRecordList->end())	//! Size를 찾았음...
					{
						SetPowerCableProp(pCableItem->prop() , SettingRecord , nNoOfCable , 
							*ktr , CCommandPowerCableCreation::m_nRunningCalculatedVD , 0.f, m_param);

						double nRunningCalculatedVD = 0.f;
						if(ktr == pRecordList->end())
						{
							ktr = pRecordList->begin();
							++nNoOfCable;
						}
						CCommandPowerCableCreation::m_nCheckingArea = CCommandPowerCableCreation::AFTER;
						//! 이후의 몇번의 계산을 수행하여 그 결과를 리스트에 담는다.
						FindPowerCableSize(pCableItem , pRecordList , ktr , SettingRecord , 
							nNoOfCable , nRunningCalculatedVD , param);
					}
				}else	CCableItem::DeleteInstance(pCableItem);

				CCommandPowerCableCreation::AdjustCableNumbering(NULL , SettingRecord , pCableItem);
			}
			else
			{
				//!
			}

			return ERROR_SUCCESS;
		}
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	Mininum Cable Size를 구한다.

	@author BHK	

	@date 2009-08-11 오후 3:04:39	

	@param	

	@return		
*/
int ITEMS::CUPSDCItem::FindMinPowerCableSize(CCableCreationTable::Record &SettingRecord , int& nNoOfCable , const double& nMinSize , vector<CCableDataByType::Record*>* pRecordList , vector<CCableDataByType::Record*>::iterator& ktr)
{
	assert(pRecordList && "pRecordList is NULL");

	if(pRecordList)
	{
		bool bFound = false;
		while(false == bFound)
		{
			for(ktr = pRecordList->begin();ktr != pRecordList->end();++ktr)
			{
				const string rSize = (*ktr)->m_FieldMap[_T("C_SIZE")].c_str();
				const double nSize = atof(rSize.c_str());
				if((-1 != nMinSize) && (nSize < nMinSize)) continue;	//! min. size 적용

				const string rUGCableAmbTemp = (*ktr)->m_FieldMap[_T("C_AMP_G")].c_str();
				const string rAGCableAmbTemp = (*ktr)->m_FieldMap[_T("C_AMP_A")].c_str();
				/// 2009.08.11
				const string rInstall = SettingRecord.FieldValueMap[_T("C_INSTALL_METHOD")];
				const string rTotalDF = SettingRecord.FieldValueMap[_T("C_TOTAL_DERATING_FACTOR")];
				const double nFLC = m_param.nFLC;
				const string rSF = SettingRecord.FieldValueMap[_T("C_SF")];

				if(ERROR_SUCCESS == python.CallPyFindMinPowerCableSize(rInstall , rTotalDF , nFLC , rSF , 
					rUGCableAmbTemp , rAGCableAmbTemp , nNoOfCable))
				{
					bFound = true;	//! MIN. POWER CABLE SIZE를 찾았음.
					return ERROR_SUCCESS;
				}
			}
			++nNoOfCable;
		}

		return (ktr != pRecordList->end()) ? ERROR_SUCCESS : ERROR_BAD_ENVIRONMENT;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	조건에 맞는 Power Cable Size를 구합니다.

	@author BHK	

	@date 2009-08-11 오후 1:41:51	

	@param	

	@return		
*/
int ITEMS::CUPSDCItem::FindPowerCableSize(CCableItem* pCableItem , 
	vector<CCableDataByType::Record*>* pRecordList , vector<CCableDataByType::Record*>::iterator& ktr , 
	CCableCreationTable::Record &SettingRecord , int& nMultiply , double& nCalculatedVD , ParamForPowerCableCreation param)
{
	assert(pCableItem && pRecordList && "pCableItem or pRecordList is NULL");

	bool bFound = false;
	if((NULL != pCableItem) && (NULL != pRecordList) && (ktr != pRecordList->end()))
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		const string rBusId = prop()->GetValue(_T("From1") , _T("Bus Id"));
		CBusItem* pBus = docData.FindBusItemByName(rBusId);
		const string rBusVolt = pBus->prop()->GetValue(_T("From1") , _T("Bus Volt"));
		const double nBusVolt = atof(rBusVolt.c_str());
		
		const double nPF100 = GetPFForPowerCableCreation();
		const double nFLC   = m_param.nFLC;
		const double nSF    = atof(SettingRecord.FieldValueMap[_T("C_SF")].c_str());
		const double nRunningVD = atof(GetRunVoltageDrop(SettingRecord).c_str());

		ostringstream oss;
		oss << param.nMotorVD;
		const string MotorVD = oss.str();

		if(COMMAND::CCommandPowerCableCreation::AFTER == COMMAND::CCommandPowerCableCreation::m_nCheckingArea)
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
				//! Mininum Size보다 클 경우에는 작업을 한다.
				if((nSize >= param.nMinSize) && (-1 != param.nMinSize))
				{
					const string rR    = (*ktr)->m_FieldMap[_T("C_R")].c_str();
					const string rX    = (*ktr)->m_FieldMap[_T("C_X")].c_str();

					const string rID	= GetName();
					string rL		= prop()->GetValue(_T("General")  , _T("Cable Length"));

					if(rL.empty()) rL = _T("80");

					const int res = python.CallPyFindPowerCableSize(rID , param.rPhase , rL ,
						rR , rX , param.nPF , param.I , param.nMotorVD , nMultiply , nCalculatedVD);
					CELoadItemProp* pCalculationResultProp = pCableItem->GetPropForOverCalculatedResult();
					if(pCalculationResultProp)
					{
						if(-1 == COMMAND::CCommandPowerCableCreation::m_nRunningCalculatedVD)
						{
							SetPowerCableProp(pCalculationResultProp , SettingRecord , nMultiply , *ktr , nCalculatedVD , 0., param);
						}
						else
						{
							SetPowerCableProp(pCalculationResultProp , SettingRecord , nMultiply , *ktr , COMMAND::CCommandPowerCableCreation::m_nRunningCalculatedVD , nCalculatedVD, param);
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
					//! Mininum Size보다 작을 경우에는 SKIP
					if((-1 != param.nMinSize) && (nSize < param.nMinSize)) continue;
					
					const string rR    = (*ktr)->m_FieldMap[_T("C_R")].c_str();
					const string rX    = (*ktr)->m_FieldMap[_T("C_X")].c_str();

					double _nCalculatedVD = -1;
					int res[2] = {ERROR_SUCCESS , ERROR_SUCCESS};
					const string rID	= GetName();
					const string rL		= prop()->GetValue(_T("General") , _T("Cable Length"));
					res[0] = python.CallPyFindPowerCableSize(rID , param.rPhase , rL ,
						rR , rX , param.nPF , param.I , param.nMotorVD , nMultiply , _nCalculatedVD);
					CELoadItemProp* pCalculationResultProp = pCableItem->GetPropForBelowCalculatedResult();
					if(pCalculationResultProp)
					{
						if(-1 == COMMAND::CCommandPowerCableCreation::m_nRunningCalculatedVD)
						{
							SetPowerCableProp(pCalculationResultProp , SettingRecord , nMultiply , *ktr , _nCalculatedVD , 0., param);
						}
						else
						{
							//! RUNNING쪽의 VD를 다시 계산한다.(running쪽도 만족해야 한다.)
							res[1] = python.CallPyFindPowerCableSize(rID , param.rPhase , rL ,
								rR , rX , nPF100 , (nFLC*nSF) , (nBusVolt*nRunningVD*0.01*1000) , nMultiply , COMMAND::CCommandPowerCableCreation::m_nRunningCalculatedVD);

							SetPowerCableProp(pCalculationResultProp , SettingRecord , nMultiply , *ktr , COMMAND::CCommandPowerCableCreation::m_nRunningCalculatedVD , _nCalculatedVD, param);
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
	@brief	

	@author BHK	

	@date 2009-08-11 오전 10:59:49	

	@param	

	@return		
*/
double ITEMS::CUPSDCItem::GetPFForPowerCableCreation() const
{
	double nPF = atof(prop()->GetValue(_T("General") , _T("PF")).c_str());
	if(nPF < 0.f) nPF = 0.f;
	return nPF;
}

/**
	@brief	Power Cable 생성에 필요한 FLC값을 구한다.

	@author BHK	

	@date 2009-09-01 오후 7:27:31	

	@param	

	@return		
*/
double ITEMS::CUPSDCItem::GetFLCForPowerCableCreation(const int& at) const
{
	const string rSelected = prop()->GetValue(_T("General") , _T("SELECTED CAPACITY"));
	if(_T("UPS Capacity") == rSelected)
	{
		if(1 == at)
			return atof(prop()->GetValue(_T("UPS Capacity") , _T("FLC")).c_str());
		else if(2 == at)
		{
			const double nKVA = atof(prop()->GetValue(_T("UPS Capacity") , _T("KVA")).c_str());
			const double nLoadVolt = atof(prop()->GetValue(_T("UPS Capacity") , _T("Output Load Volt")).c_str());
			
			const string rToBusID = prop()->GetValue(_T("To") , _T("Bus ID"));
			CELoadDocData& docData = CELoadDocData::GetInstance();
			CBusItem* pBusItem = docData.FindBusItemByName(rToBusID);
			if(NULL != pBusItem)
			{
				const string rPhase = pBusItem->prop()->GetValue(_T("General") , _T("Phase"));
				if(_T("3") == rPhase)
				{
					return (nKVA / nLoadVolt / sqrt(3.f));
				}
				else
				{
					return (nKVA / nLoadVolt);
				}
			}
		}
	}
	else
	{
		if(1 == at)
			return atof(prop()->GetValue(_T("DC Capacity") , _T("AC Rating FLC")).c_str());
		else if(2 == at)
			return atof(prop()->GetValue(_T("DC Capacity") , _T("DC Rating FLC")).c_str());
	}

	return 0.f;
}

/**
	@brief	Power Cable 생성에 필요한 KVA값을 구한다.

	@author BHK	

	@date 2009-09-01 오후 7:56:34	

	@param	

	@return		
*/
double ITEMS::CUPSDCItem::GetLoadVoltForPowerCableCreation(const int& at) const
{
	if(0 == at)
	{
		return atof(prop()->GetValue(_T("From1") , _T("Load Volt")).c_str());
	}
	else if(1 == at)
	{
		return atof(prop()->GetValue(_T("From2") , _T("Load Volt")).c_str());
	}
	else if(2 == at)
	{
		return atof(prop()->GetValue(_T("To") , _T("Load Volt")).c_str());
	}

	/*
	const string rSelected = prop()->GetValue(_T("General") , _T("SELECTED CAPACITY"));
	if(_T("UPS Capacity") == rSelected)
	{
		return atof(prop()->GetValue(_T("UPS Capacity") , _T("Input Load Volt")).c_str());
	}
	else
	{
		return atof(prop()->GetValue(_T("DC Capacity") , _T("DC Rating Load Volt")).c_str());
	}
	*/

	return 0.f;
}

/**
	@brief	

	@author BHK	

	@date 2009-08-11 오전 11:02:11	

	@param	

	@return		
*/
string ITEMS::CUPSDCItem::GetRunVoltageDrop(TABLES::CCableCreationTable::Record &SettingRecord) const
{
	return SettingRecord.FieldValueMap[_T("C_FEEDER_RUNNING")];
}


/**
	@brief	Power Cable Item의 속성을 설정합니다.

	@author	BHK

	@date 2009-08-11 오후 2:19:26
*/
int ITEMS::CUPSDCItem::SetPowerCableProp( CELoadItemProp* pItemProp , CCableCreationTable::Record &SettingRecord , const int& nNoOfCable , 
					CCableDataByType::Record* pRecord , const double& nRunningVD , const double& nStartingVD, const ParamForPowerCableCreation& param)
{
	assert(pItemProp && pRecord && "pItemProp or pRecord is NULL");

	if(pItemProp && pRecord)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		
		const string rVoltType          = SettingRecord.FieldValueMap[_T("C_VOLTAGE_TYPE")];
		const string rCableNamePrefix   = SettingRecord.FieldValueMap[_T("C_MAIN_POWER_CABLE_NAMING_PREFIX")];
		const string rCableNameSuffix   = SettingRecord.FieldValueMap[_T("C_MAIN_POWER_CABLE_NAMING_SUFFIX")];
		const string rInstallMethod	= SettingRecord.FieldValueMap[_T("C_INSTALL_METHOD")];
		const double nBusVolt		= atof(SettingRecord.FieldValueMap[_T("C_BUS_VOLTAGE")].c_str());
		
		const string rID = GetName();
		const string rCableName= rCableNamePrefix + rID + rCableNameSuffix;
		static const string GENERAL=_T("General");
		pItemProp->SetValue(GENERAL , _T("CABLE ID") , rCableName);
		pItemProp->SetOriginalValue(GENERAL , _T("CABLE ID") , rCableName);
		pItemProp->SetValue(GENERAL , _T("Area") , param.rArea);
		pItemProp->SetOriginalValue(GENERAL , _T("Area") , param.rArea);

		pItemProp->SetValue(_T("General"), _T("Cable Category"), _T("Power Cable"));
		pItemProp->SetOriginalValue(_T("General"), _T("Cable Category"), _T("Power Cable"));
		pItemProp->SetValue(_T("General"), _T("SubCategory"), _T("Equipment"));
		pItemProp->SetOriginalValue(_T("General"), _T("SubCategory"), _T("Equipment"));

		pItemProp->SetValue(_T("General") , _T("Description") , rID + _T(" Power Cable"));
		pItemProp->SetOriginalValue(_T("General") , _T("Description") , rID + _T(" Power Cable"));

		pItemProp->SetValue(_T("General"), _T("Created By"), _T("Auto"));
		pItemProp->SetOriginalValue(_T("General"), _T("Created By"), _T("Auto"));

		pItemProp->SetValue(_T("General"), _T("From"), param.rFrom);
		pItemProp->SetOriginalValue(_T("General"), _T("From"), param.rFrom);
		pItemProp->SetValue(_T("General"), _T("Panel ID"), param.rPanelID);
		pItemProp->SetOriginalValue(_T("General"), _T("Panel ID"), param.rPanelID);
		pItemProp->SetValue(_T("General"), _T("To"), param.rTo);
		pItemProp->SetOriginalValue(_T("General"), _T("To"), param.rTo);
		pItemProp->SetValue(_T("General"), _T("Load Id"), param.rLoadID);
		pItemProp->SetOriginalValue(_T("General"), _T("Load Id"), param.rLoadID);

		//!Load Volt
		{
			ostringstream oss;
			oss << param.nLoadVolt;
			pItemProp->SetValue(_T("General"), _T("Load Volt"), oss.str());
			pItemProp->SetOriginalValue(_T("General"), _T("Load Volt"), oss.str());
		}

		//!SF
		const string rSF = SettingRecord.FieldValueMap[_T("C_SF")];
		pItemProp->SetValue(_T("General"), _T("SF"), rSF);
		pItemProp->SetOriginalValue(_T("General"), _T("SF"), rSF);

		//! FLC
		CString rFLC;
		rFLC.Format("%.2f",param.nFLC);
		pItemProp->SetValue(_T("General"), _T("FLC"), rFLC.operator LPCSTR());
		pItemProp->SetOriginalValue(_T("General"), _T("FLC"), rFLC.operator LPCSTR());

		//!Cable Length
		const string rCableLength = prop()->GetValue(_T("General"), _T("Cable Length"));
		pItemProp->SetValue(_T("General"), _T("Length"), rCableLength);
		pItemProp->SetOriginalValue(_T("General"), _T("Length"), rCableLength);

		//! SPEC 
		pItemProp->SetValue(GENERAL , _T("Cable Spec") , param.rSpec);
		pItemProp->SetOriginalValue(GENERAL , _T("Cable Spec") , param.rSpec);

		//! SIZE
		const string rCableSize= pRecord->m_FieldMap[_T("C_SIZE")];
		pItemProp->SetValue(GENERAL , _T("Size") , rCableSize);
		pItemProp->SetOriginalValue(GENERAL , _T("Size") , rCableSize);
		
		//! CORE
		const string rCableCore= pRecord->m_FieldMap[_T("C_CORE")];
		pItemProp->SetValue(GENERAL , _T("Core") , rCableCore);
		pItemProp->SetOriginalValue(GENERAL , _T("Core") , rCableCore);

		//!VOLT 
		const string rCableVolt= pRecord->m_FieldMap[_T("C_VOLT")];
		pItemProp->SetValue(GENERAL , _T("Cable Volt") , rCableVolt);
		pItemProp->SetOriginalValue(GENERAL , _T("Cable Volt") , rCableVolt);

		//!No og Cable
		CString rString;
		rString.Format("%d" , nNoOfCable);
		pItemProp->SetValue(GENERAL , _T("No Of Cables/Phase") , rString.operator LPCSTR());
		pItemProp->SetOriginalValue(GENERAL , _T("No Of Cables/Phase") , rString.operator LPCSTR());

		stringstream oss;
		oss << SAFE_ROUND(nRunningVD,2);
		pItemProp->SetValue(GENERAL , _T("Running VD") , oss.str());
		pItemProp->SetOriginalValue(GENERAL , _T("Running VD") , oss.str());
		if(0. != nStartingVD)
		{
			oss.str(""); oss << SAFE_ROUND(nStartingVD,2);
			pItemProp->SetValue(GENERAL , _T("Starting VD") , oss.str());
			pItemProp->SetOriginalValue(GENERAL , _T("Starting VD") , oss.str());
			oss.str(""); oss << prop()->GetValue(_T("Voltage Drop") , _T("Motor Start VD"));
			pItemProp->SetValue(GENERAL , _T("Allow Starting VD%") , oss.str());
			pItemProp->SetOriginalValue(GENERAL , _T("Allow Starting VD%") , oss.str());
		}

		//! 현재 Equipment는 Area 항목이 없음.
		///pItemProp->SetValue(GENERAL , _T("Area") , rArea);
		///pItemProp->SetOriginalValue(GENERAL , _T("Area") , rArea);

		pItemProp->SetValue(GENERAL , _T("Volt Type") , rVoltType);
		pItemProp->SetOriginalValue(GENERAL , _T("Volt Type") , rVoltType);

		oss.str(""); oss << SAFE_ROUND((nRunningVD/(nBusVolt*1000)*100),2);
		pItemProp->SetValue(GENERAL , _T("Running VD%") , oss.str());
		pItemProp->SetOriginalValue(GENERAL , _T("Running VD%") , oss.str());
		oss.str(""); oss << GetRunVoltageDrop(SettingRecord);
		pItemProp->SetValue(GENERAL , _T("Allow Running VD%") , oss.str());
		pItemProp->SetOriginalValue(GENERAL , _T("Allow Running VD%") , oss.str());
		/*
		if(0. != nStartingVD)
		{
			oss.str(""); oss << SAFE_ROUND(nStartingVD,2);
			pItemProp->SetValue(GENERAL , _T("Starting VD") , oss.str());
			pItemProp->SetOriginalValue(GENERAL , _T("Starting VD") , oss.str());
			oss.str(""); oss << SAFE_ROUND((nStartingVD/(nBusVolt*1000)*100),2);
			pItemProp->SetValue(GENERAL , _T("Starting VD%") , oss.str());
			pItemProp->SetOriginalValue(GENERAL , _T("Starting VD%") , oss.str());
			oss.str(""); oss << prop()->GetValue(_T("Voltage Drop") , _T("Motor Start VD"));
			pItemProp->SetValue(GENERAL , _T("Allow Starting VD%") , oss.str());
			pItemProp->SetOriginalValue(GENERAL , _T("Allow Starting VD%") , oss.str());
		}
		*/

		///const string rInstall = prop()->GetValue(_T("Cable Route") , _T("Install"));

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

		if(_T("U/G") == rInstallMethod)
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
		for(vector<CCableLengthDataSheet::ConduitSizeData>::iterator itr = COMMAND::CCommandPowerCableCreation::m_ConduitSizeList.begin();itr != COMMAND::CCommandPowerCableCreation::m_ConduitSizeList.end();++itr)
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
		
		oss.str("");  oss << COMMAND::CCommandPowerCableCreation::m_nConduitLength;
		pItemProp->SetValue(CONDUIT , _T("Conduit Length") , oss.str());
		pItemProp->SetOriginalValue(CONDUIT , _T("Conduit Length") , oss.str());
		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	initialize cable item's properties

	@author	BHK

	@date	2009.11.12

	@param	pointer of cable item

	@return ERROR_SUCCESS or ERROR_BAD_ENVIRONMENT
*/
int ITEMS::CUPSDCItem::InitializeCableItemProperties(CCableItem* pCableItem)
{
	assert(pCableItem && "pCableItem is NULL");

	if(pCableItem)
	{
		const string rPF = prop()->GetValue(_T("General"), _T("PF"));
		pCableItem->prop()->SetValue(_T("General") , _T("PF") , rPF);
		pCableItem->prop()->SetOriginalValue(_T("General") , _T("PF") , rPF);

		const string rSelected = prop()->GetValue(_T("General") , _T("SELECTED CAPACITY"));
		string rRating;
		if(_T("UPS Capacity") == rSelected)
		{
			rRating = prop()->GetValue(_T("UPS Capacity") , _T("KVA"));
		}
		else
		{
			rRating = prop()->GetValue(_T("DC Capacity") , _T("AC Rating KVA"));
		}
		pCableItem->prop()->SetValue(_T("General") , _T("Rating Capacity") , rRating);
		pCableItem->prop()->SetOriginalValue(_T("General") , _T("Rating Capacity") , rRating);

		const string rUnit(_T("KVA"));
		pCableItem->prop()->SetValue(_T("General") , _T("UNIT") , rUnit);
		pCableItem->prop()->SetOriginalValue(_T("General") , _T("UNIT") , rUnit);

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}
