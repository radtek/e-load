#include "StdAfx.h"
#include "ELoad.h"
#include "LoadItem.h"
#include "LoadItemProp.h"
#include <util/ado/ADODB.h>
#include "ELoadDocData.h"
#include "commands/CommandAutoCalculateFLC.h"
#include "commands/CommandPowerCableCreation.h"
#include "pyfromc.h"

using namespace COMMAND;
using namespace ITEMS;

IMPLEMENTS_ELOAD_ITEM(CLoadItem , CELoadItem , "LOAD")

CLoadItem::CLoadItem() : CELoadItem()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELoadItemProp* pItemProp = docData.m_ItemPropMap[CLoadItem::TypeString()];
	if(pItemProp)
	{
		m_pProp = pItemProp->Clone();
		m_pProp->SetOwner(this);

		//! Load의 기본값을 설정한다.
		prop()->SetOriginalValue(_T("Type") , _T("Load Category") , _T("MOTOR"));
		prop()->SetValue(_T("Type") , _T("Load Category") , _T("MOTOR"));

		prop()->SetOriginalValue(_T("Characteristic") , _T("PF LOCK") , _T("NO"));
		prop()->SetValue(_T("Characteristic") , _T("PF LOCK") , _T("NO"));

		prop()->SetOriginalValue(_T("Characteristic") , _T("EFF LOCK") , _T("NO"));
		prop()->SetValue(_T("Characteristic") , _T("EFF LOCK") , _T("NO"));

		prop()->SetOriginalValue(_T("Circuit Breaker") , _T("LOCK") , _T("NO"));
		prop()->SetValue(_T("Circuit Breaker") , _T("LOCK") , _T("NO"));

		//! 추가
		prop()->SetOriginalValue(_T("Type") , _T("Operation Mode") , _T("CONTINUOUS"));
		prop()->SetValue(_T("Type") , _T("Operation Mode") , _T("CONTINUOUS"));

		prop()->SetOriginalValue(_T("Rating") , _T("Poles") , _T("4"));
		prop()->SetValue(_T("Rating") , _T("Poles") , _T("4"));
	}
}

CLoadItem::~CLoadItem(void)
{
	try
	{
		if(m_pProp) SAFE_DELETE(m_pProp);

		//! CableItem Entry 클리어.
		ClearCableItems();
	}
	catch(...)
	{
	}
}

/**
	@brief	load item의 이름을 리턴한다.

	@author	BHK
*/
string CLoadItem::GetName() const
{
	CELoadItemProp* pProp = prop();
	if(pProp) return pProp->GetValue("GENERAL" , "ITEM ID");
        
        return string("");
}

/**
	@brief	set load name

	@author	BHK
*/
int CLoadItem::SetName( const string& name )
{
	static const string _CATEGORY_("GENERAL");
	static const string _KEY_("ITEM ID");

	CELoadItemProp* pProp = prop();
	if(pProp)
	{
		pProp->SetOriginalValue(_CATEGORY_ , _KEY_ , name);
		int res = pProp->SetValue(_CATEGORY_ , _KEY_ , name);
		return res;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	BUS ID를 구한다.

	@author	BHK
*/
string CLoadItem::GetBusId() const
{
        CItemPropCategory* pCategory = prop()->GetCategoryByName("Panel");
        if(pCategory) return pCategory->GetValue("Bus ID");
	
	return _T("");
}

/**
	@brief	BUS ID를 설정한다.

	@param	rBusId = BUS ITEM NAME

	@author	BHK
*/
int CLoadItem::SetBusId( const string& rBusId )
{
        CELoadItemProp* pProp = prop();
	if(pProp)
        {
                pProp->SetValue("Panel" , "Bus ID" , rBusId);

		CELoadDocData& docData = CELoadDocData::GetInstance();
		CBusItem* pBusItem = docData.FindBusItemByName(rBusId);
		if(pBusItem)
		{
			const string rRatedVoltage = pBusItem->prop()->GetValue("General" , "Load Voltage");
			const string rPhase        = pBusItem->prop()->GetValue("General" , "Phase");

			pProp->SetValue("Rating" , "Rated Voltage" , rRatedVoltage);
			pProp->SetValue("Rating" , "Phase" , rPhase);

			///
			CCommandAutoCalculateFLC cmdAutoCalculateFLC;
			cmdAutoCalculateFLC.m_SelectionSet.Add(this);
			cmdAutoCalculateFLC.Execute();
		}

        	return ERROR_SUCCESS;
        }

        return ERROR_BAD_ENVIRONMENT;
}

int CLoadItem::SetValue( const string& rCategory , const string& rKey , const string& rValue )
{
	/*for(vector<CItemPropCategory*>::const_iterator itr = m_pItemPropCategory->begin();itr != m_pItemPropCategory->end();++itr)
	{
		if(rCategory == (*itr)->name())
		{
			return (*itr)->SetValue( rKey , rValue );
		}
	}
        */
	return ERROR_INVALID_PARAMETER;
}

int CLoadItem::Copy( CLoadItem* pCopied )
{
        if(pCopied)
        {
		CELoadItem::Copy(pCopied);
                return ERROR_SUCCESS;
        }

        return ERROR_INVALID_PARAMETER;
}

/**
	@brief

	@author	HumKyung Baek
*/
string CLoadItem::TableName()
{
	static const string __TABLE_NAME__ = _T("T_LOAD_ITEM");

	return __TABLE_NAME__;
}

/**
	@brief	Load Summary 계산시 나온 값들을 구한다.

	@author HumKyung

	@date 2009-05-15 오후 3:13:37	

	@param	

	@return		
*/
int ITEMS::CLoadItem::GetLoadSummaryCalcData(double nFactors[3] , double nKW[3] , double nKVAR[3])
{
	//! SubBUS로 연결되어 있으면 PDB쪽의 값을 구해온다.
	const string rLoadCategory   = prop()->GetValue(_T("Type") , _T("Load Category"));
	const string rConnectedBusID = prop()->GetValue(_T("Panel") , _T("Connect Bus"));
	if((0 == stricmp(SUBBUS , rLoadCategory.c_str())) && !rConnectedBusID.empty())
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		CBusItem* pBusItem = docData.FindBusItemByName(rConnectedBusID);
		if(pBusItem)
		{
			double nBusFactors[3] = {0.f , 0.f , 0.f};
			double nBusKW[3] = {0.f , 0.f , 0.f} , nBusKVAR[3] = {0.f , 0.f , 0.f};
			pBusItem->GetLoadSummaryCalcData(nBusFactors , nBusKW , nBusKVAR);

			nFactors[0] = nBusFactors[0];
			nFactors[1] = nBusFactors[1];
			nFactors[2] = nBusFactors[2];
			nKW[0] += nBusKW[0]; nKW[1] += nBusKW[1]; nKW[2] += nBusKW[2];
			nKVAR[0] += nBusKVAR[0]; nKVAR[1] += nBusKVAR[1]; nKVAR[2] += nBusKVAR[2];
		}
	}
	else
	{
		nFactors[0] = atof(prop()->GetValue(_T("Characteristic") , _T("LF")).c_str());
		nFactors[0] = SAFE_ROUND(nFactors[0] , 2);
		nFactors[1] = atof(prop()->GetValue(_T("Load Summary") , _T("PF")).c_str());
		nFactors[1] = SAFE_ROUND(nFactors[1] , 3);
		nFactors[2] = atof(prop()->GetValue(_T("Load Summary") , _T("EFF")).c_str());
		nFactors[2] = SAFE_ROUND(nFactors[2] , 3);

		const string rOperMode = prop()->GetValue(_T("Type") , _T("Operation Mode"));
		if(_T("CONTINUOUS") == rOperMode)
		{
			nKW[0] += atof(prop()->GetValue(_T("Load Summary") , _T("KW")).c_str());
			nKVAR[0] += atof(prop()->GetValue(_T("Load Summary") , _T("KVAR")).c_str());
		}
		else if(_T("INTERMITTENT") == rOperMode)
		{
			nKW[1] += atof(prop()->GetValue(_T("Load Summary") , _T("KW")).c_str());
			nKVAR[1] += atof(prop()->GetValue(_T("Load Summary") , _T("KVAR")).c_str());
		}
		else if(_T("STAND-BY") == rOperMode)
		{
			nKW[2] += atof(prop()->GetValue(_T("Load Summary") , _T("KW")).c_str());
			nKVAR[2] += atof(prop()->GetValue(_T("Load Summary") , _T("KVAR")).c_str());
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	Cable Item들을 제거합니다.

	@author	HumKyung

	@date	????.??.??

	@param	rCategory
	@param	rCreatedBy

	@return	int
*/
int ITEMS::CLoadItem::ClearCableItems(const string& rCategory, const string& rCreatedBy)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	vector<CELoadItem*> CopiedCableRefEntry;
	CopiedCableRefEntry.insert(CopiedCableRefEntry.begin() , m_CableRefEntry.begin() , m_CableRefEntry.end());
	for(vector<CELoadItem*>::iterator itr = CopiedCableRefEntry.begin();itr != CopiedCableRefEntry.end();)
	{
		const string rCableCategory = (*itr)->prop()->GetValue(_T("General"),_T("Cable Category"));
		const string  rCableCreatedBy = (*itr)->prop()->GetValue(_T("General") , _T("Created By"));
		if((rCategory.empty() || (rCableCategory == rCategory)) && (rCreatedBy.empty() || (rCableCreatedBy == rCreatedBy )))
		{
			list<CELoadItem*>* pCableItemList = docData.GetELoadItemListPtrOf((*itr)->GetTypeString());
			if(pCableItemList)
			{
				list<CELoadItem*>::iterator where = find(pCableItemList->begin() , pCableItemList->end() , *itr);
				if(where != pCableItemList->end())
				{
					SAFE_DELETE(*where);
					pCableItemList->erase(where);	//! erase가 호출되면 iterator가 무효화가 된다.
					itr = CopiedCableRefEntry.erase(itr);
					continue;
				}
			}
		}
		++itr;
	}
	
	assert(m_CableRefEntry.size() == CopiedCableRefEntry.size());
	/*
	if(rCategory.empty())
	{
		assert(m_CableRefEntry.empty());

		m_CableRefEntry.clear();
	}
	*/
	return ERROR_SUCCESS;
}

/**
	@brief	delete flag를 설정합니다. 가지고 있는 cable item에 대해서도 적용시킵니다.

	@author	HumKyung Baek
*/
int ITEMS::CLoadItem::SetDeleteFlag(const bool& bDelete)
{
	CELoadItem::SetDeleteFlag(bDelete);

	for(vector<CELoadItem*>::iterator itr = m_CableRefEntry.begin();itr != m_CableRefEntry.end();++itr)
	{
		(*itr)->SetDeleteFlag(bDelete);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	load rating capacity를 구한다.

	@author HumKyung Baek	

	@date 2009-04-06 오후 3:16:04	

	@param	

	@return		
*/
int ITEMS::CLoadItem::CalcRatingCapacity(double& nRatingCapacity)
{
	nRatingCapacity = atof(prop()->GetValue(_T("Rating") , _T("Rating Capacity")).c_str());
	
	return ERROR_SUCCESS;
}


/**
	@brief	LoadSummary를 계산한다. 실패시 LoadSummary 계산 결과를 담은 항목을 클리어한다.

	@author HumKyung Baek	

	@date 2009-04-06 오후 3:16:04	

	@param	

	@return	ERROR_SUCCESS or ERROR_BAD_ENVIRONMENT
*/
int ITEMS::CLoadItem::CalcLoadSummary()
{
	CBusItem* pBusItem = NULL;
	
	const string rLoadID = GetName();

	const string rConnectedBusID = prop()->GetValue(_T("Panel") , _T("Connect Bus"));
	if(!rConnectedBusID.empty())
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		pBusItem = docData.FindBusItemByName(rConnectedBusID);
		if(pBusItem) pBusItem->CalcLoadSummary();
	}

	vector<double> result;
	if(ERROR_SUCCESS != python.CallPyLoadSummary(rLoadID , result))
	{
		prop()->SetValue(_T("Load Summary") , _T("PF") , _T("") , true);
		prop()->SetValue(_T("Load Summary") , _T("EFF") , _T("") , true);
		prop()->SetValue(_T("Load Summary") , _T("KW") , _T("") , true);
		prop()->SetValue(_T("Load Summary") , _T("KVAR") , _T("") , true);
		prop()->SetValue(_T("Load Summary") , _T("KVA") , _T("") , true);

		return ERROR_BAD_ENVIRONMENT;
	}

	return ERROR_SUCCESS;
}

/**
	@brief	Cable계산용 용량을 구한다.

	@author HumKyung Baek	

	@date 2009-04-24 오후 2:36:50	

	@param	

	@return		
*/
double ITEMS::CLoadItem::GetRatingCapacityForPowerCableCreation()
{
	double nRatingCapacity = 0.f;

	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string rLoadCategory = CString(prop()->GetValue(_T("Type") , _T("Load Category")).c_str()).MakeUpper();
	const string rUse          = CString(prop()->GetValue(_T("Panel"), _T("Real KVA For Power Cable")).c_str()).MakeUpper();
	const string rConnectedBusID = prop()->GetValue(_T("Panel") , _T("Connect Bus"));
	if((SUBBUS == rLoadCategory) && (_T("YES") == rUse) && !rConnectedBusID.empty())
	{
		double nKW = 0.f , nKVAR = 0.f;
		CBusItem* pBusItem = docData.FindBusItemByName(rConnectedBusID);
		if(pBusItem) pBusItem->GetLoadSummaryResultData(nKW , nKVAR);
		nRatingCapacity = SAFE_ROUND(sqrt(nKW*nKW + nKVAR*nKVAR) , 2);
	}
	else
	{
		nRatingCapacity = atof(prop()->GetValue(_T("Rating") , _T("Rating Capacity")).c_str());
	}
	
	return nRatingCapacity;
}

/**
	@brief	SubBUS이고 LoadSummary의 계산 결과를 사용할 경우와 그렇지 않는 경우에 있어 그 값을 리턴합니다.

	@author HumKyung Baek	

	@date 2009-04-17 오후 4:11:02	

	@param	

	@return		
*/
double ITEMS::CLoadItem::GetFLCForPowerCableCreation()
{
	double nFLC = 0.f;

	const string rLoadCategory = CString(prop()->GetValue(_T("Type") , _T("Load Category")).c_str()).MakeUpper();
	const string rUse          = CString(prop()->GetValue(_T("Panel"), _T("Real KVA For Power Cable")).c_str()).MakeUpper();

	if((SUBBUS == rLoadCategory) && (_T("YES") == rUse))
	{
		const string rConnectedBusID = prop()->GetValue(_T("Panel") , _T("Connect Bus"));
		CELoadDocData& docData = docData.GetInstance();
		CBusItem* pBusItem = docData.FindBusItemByName(rConnectedBusID);
		if(pBusItem)
		{
			double nKW = 0.f , nKVAR = 0.f;
			pBusItem->GetLoadSummaryResultData(nKW , nKVAR);
			const double nKVA = SAFE_ROUND(sqrt(nKW*nKW + nKVAR*nKVAR) , 2);

			const string rPhase = prop()->GetValue(_T("Rating") , _T("Phase"));
			const double nRatedVoltage = atof(prop()->GetValue(_T("Rating") , _T("Rated Voltage")).c_str());

			//! 계산식에서 KVA 경우일때의 계산식을 사용함.
			if(_T("3") == rPhase)
			{
				nFLC = (nKVA / (sqrt(3.)*nRatedVoltage));
				nFLC = SAFE_ROUND(nFLC,2);
			}
			else if(_T("1") == rPhase)
			{
				nFLC = (nKVA / (nRatedVoltage));
				nFLC = SAFE_ROUND(nFLC,2);
			}
			else if(_T("DC") == rPhase)
			{
				nFLC = (nKVA / (nRatedVoltage));
				nFLC = SAFE_ROUND(nFLC,2);
			}
		}
	}
	else
	{
		nFLC = atof(prop()->GetValue(_T("Rating") , _T("FLC")).c_str());
	}

	return nFLC;
}

/**
	@brief	SubBUS일 경우에는 LoadSummary 계산 결과인 PF값을 리턴합니다.

	@author HumKyung Baek	

	@date 2009-04-17 오후 4:11:02	

	@param	

	@return		
*/
double ITEMS::CLoadItem::GetPFForPowerCableCreation()
{
	double nPowerFactor = 0.f;

	const string rLoadCategory = CString(prop()->GetValue(_T("Type") , _T("Load Category")).c_str()).MakeUpper();
	const string rUse          = CString(prop()->GetValue(_T("Panel"), _T("Real KVA For Power Cable")).c_str()).MakeUpper();

	if((SUBBUS == rLoadCategory) && (_T("YES") == rUse))
	{
		///const string rConnectedBusID = prop()->GetValue(_T("Panel") , _T("Connect Bus"));
		double nFactors[3] = {0.f,0.f,0.f} , nKW[3] = {0.f,0.f,0.f} , nKVAR[3] = {0.f,0.f,0.f};
		GetLoadSummaryCalcData(nFactors , nKW , nKVAR);
		nPowerFactor = nFactors[1];
	}
	else
	{
		nPowerFactor += atof(prop()->GetValue(_T("Characteristic") , _T("PF(100% Load)")).c_str());
		//! PF가 0일 경우에 에러를 방지하기 위해 1로 설정한다.
		nPowerFactor = (0.f == nPowerFactor) ? 1.f : nPowerFactor;
	}

	//! PF값이 0일 경우에는 에러가 발생하지 않도록 1을 리턴하도록 합니다.
	if(0.f == nPowerFactor) nPowerFactor = 1.f;
	nPowerFactor = SAFE_ROUND(nPowerFactor , 3);

	return nPowerFactor;
}

/**
	@brief	SubBUS일 경우에는 LoadSummary 계산 결과인 EFF값을 리턴합니다.

	@author HumKyung Baek	

	@date 2009-04-17 오후 4:11:02	

	@param	

	@return		
*/
double ITEMS::CLoadItem::GetEFFForPowerCableCreation()
{
	double nEFF100 = 0.f;

	const string rLoadCategory = CString(prop()->GetValue(_T("Type") , _T("Load Category")).c_str()).MakeUpper();
	const string rUse          = CString(prop()->GetValue(_T("Panel"), _T("Real KVA For Power Cable")).c_str()).MakeUpper();

	if((SUBBUS == rLoadCategory) && (_T("YES") == rUse))
	{
		double nFactors[3] = {0.f,0.f,0.f} , nKW[3] = {0.f,0.f,0.f} , nKVAR[3] = {0.f,0.f,0.f};
		GetLoadSummaryCalcData(nFactors , nKW , nKVAR);
		nEFF100 = nFactors[2];
	}
	else
	{
		nEFF100 += atof(prop()->GetValue(_T("Characteristic") , _T("EFF(100% Load)")).c_str());
		//! EFF값이 0일 경우에 에러를 방지하기 위해 1.로 설정한다.
		nEFF100 = (0.f == nEFF100) ? 1.f : nEFF100;
	}

	nEFF100 = SAFE_ROUND(nEFF100 , 3);

	return nEFF100;
}

struct MatchCableCategory : unary_function<ITEMS::CELoadItem*,bool>
{
	MatchCableCategory(const string& rInterestingCategory) : _rCategory(rInterestingCategory)
	{
	}
	bool operator()(ITEMS::CELoadItem* pCableItem) const
	{
		const string rValue = pCableItem->prop()->GetValue(_T("General") , _T("Cable Category"));
		return (_rCategory == rValue);

	}
private:
	const string _rCategory;
};

/**
	@brief	load에 속한 매개 변수로 넘겨준 값과 같은 category를 가진 cable을 갯수를 구한다.

	@author HumKyung Baek	

	@date 2009-06-10 오전 10:46:54	

	@param	

	@return		
*/
int CLoadItem::GetCableCountOf(const string& rCableCategory)
{
	MatchCableCategory _match(rCableCategory);
	return count_if(m_CableRefEntry.begin() , m_CableRefEntry.end() , _match);
}

/**
	@brief	Power Cable을 생성한다.

	@author HumKyung Baek	

	@date 2009-08-10 오후 5:42:21	

	@param	

	@return		
*/
int ITEMS::CLoadItem::CreatePowerCable(double nMinSize, CCableCreationTable::Record& SettingRecord, const double& nBusVolt)
{
	///assert(pPowerCableCreationCmd && "pPowerCableCreationCmd is NULL");

	///if(pPowerCableCreationCmd)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();

		const string rLoadID = prop()->GetValue(_T("GENERAL") , _T("ITEM ID"));
		if(ERROR_SUCCESS == python.CallPyCheckLoadPropForPowerCableCreation(rLoadID))
		{
			const string rLoadCategory        = CString(prop()->GetValue(_T("Type") , _T("Load Category")).c_str()).MakeUpper();
			const string rCableVolt		  = prop()->GetValue(_T("Power Cable Setting"),_T("Cable Voltage"));
			const string rCableCore		  = prop()->GetValue(_T("Power Cable Setting") , _T("Cable Core"));
			const string rCableType		  = prop()->GetValue(_T("Power Cable Setting") , _T("Cable Type"));
			const string rRatingCapacityAbove = prop()->GetValue(_T("Power Cable Setting") , _T("Rating Capacity Above"));
			const string rInstallMethod	  = prop()->GetValue(_T("Cable Route") , _T("Install"));
			const string rSF		  = prop()->GetValue(_T("Voltage Drop") , _T("SF For FLC"));
			const string rTotalDF		  = prop()->GetValue(_T("Derating Factor") , _T("Total DF"));
			const double nFLC = GetFLCForPowerCableCreation();

			const double nSF  = atof(rSF.c_str());
			const double nLRC = 0.01 * atof(prop()->GetValue(_T("Characteristic") , _T("LRC")).c_str());

			const string rType = rCableVolt + _T("KV ") + rCableCore + _T(" CU") + _T(" ") + rCableType + _T(" (eLoad)");
			CString rCableSize, rCableAmp;

			//! Cable Data Table에서 Cable Type에 일치하는 Cable Data Record를 구한다.
			CCableDataByType* pCableDataType = docData.m_ProjectCableDataTable.GetRecordListOf(rType);
			if(NULL == pCableDataType) 
			{
				ELOAD_LOG4CXX_ERROR(mylogger , rLoadID , 14);
				return ERROR_BAD_ENVIRONMENT;
			}

			vector<CCableDataByType::Record*>* pRecordList = pCableDataType->GetRecordEntryOfHertz();
			if(pRecordList && !pRecordList->empty())
			{
				int nNoOfCable = 1;

				//! Mininum Cable Size를 구한다.
				vector<CCableDataByType::Record*>::iterator ktr;
				if(ERROR_BAD_ENVIRONMENT == FindMinPowerCableSize(nNoOfCable , nMinSize , pRecordList , ktr))
				{
					ELOAD_LOG4CXX_ERROR(mylogger , rLoadID , 18);
					return ERROR_BAD_ENVIRONMENT;
				}

				CString rMotorStartApply = prop()->GetValue("Voltage Drop" , "Motor Start Apply").c_str();
				const string rPFatStarting = prop()->GetValue("Characteristic" , "PF at Starting");
				const bool bNeedCheckMotorStartApply = (_T("YES") == rMotorStartApply.MakeUpper() && !rPFatStarting.empty());
				const double nPFatStarting = atof(rPFatStarting.c_str());

				if(ktr != pRecordList->end())
				{
					const double nPF = GetPFForPowerCableCreation();
					const string rRunningVD = GetRunVoltageDrop();
					if(rRunningVD.empty())
					{
						ELOAD_LOG4CXX_ERROR(mylogger , rLoadID, 13);
						return ERROR_BAD_ENVIRONMENT;
					}
					const double nRunVD = atof(rRunningVD.c_str());

					//! 먼저 Cable Item을 생성한다.
					const string rBusID  = prop()->GetValue(_T("Panel") , _T("Bus ID"));
					const string rLoadID = prop()->GetValue(_T("General") , _T("ITEM ID"));
					CCableItem* pCableItem = CCommandPowerCableCreation::CreatePowerCableItem(rBusID , rLoadID , rBusID , rLoadID , SettingRecord);
					if(pCableItem)
					{
						//! initialize cable properties.
						InitializeCableItemProperties(pCableItem);

						AddCable(pCableItem);
						pCableItem->SetLoadItemPtr(this);
					}else	return ERROR_BAD_ENVIRONMENT;

					CCommandPowerCableCreation::m_nRunningCalculatedVD = -1;
					CCommandPowerCableCreation::m_nCheckingArea = CCommandPowerCableCreation::BEFORE;
					//! 1. RunningVD가 맞는 Cable Size를 선정한다.
					if(ERROR_SUCCESS == FindPowerCableSize(pCableItem , pRecordList , ktr , SettingRecord , 
						nPF , (nFLC*nSF) , (nBusVolt*nRunVD*0.01*1000) , nMinSize , nNoOfCable , CCommandPowerCableCreation::m_nRunningCalculatedVD))
					{
						if((ktr != pRecordList->end()) && bNeedCheckMotorStartApply)
						{
							//! Motor Starting쪽 확인이 필요할때... 계산 결과를 담은 리스트를 클리어한다.
							pCableItem->ClearCalculatedResult();

							double nStartingCalculatedVD = 0.f;
							const string StartVD  = prop()->GetValue("Voltage Drop" , "Motor Start VD");
							const double nStartVD = atof(StartVD.c_str());
							//! 2. StartVD가 맞는 Cable Size를 선정한다.(RunningVD도 맞도록 한다.) 
							if(ERROR_SUCCESS == FindPowerCableSize(pCableItem , pRecordList , ktr , 
								SettingRecord , nPFatStarting , (nFLC*nLRC) , (nBusVolt*nStartVD*0.01*1000) , nMinSize , nNoOfCable , nStartingCalculatedVD))
							{
								//! RUNNING쪽의 VD를 다시 계산한다.
								const string rR = (*ktr)->m_FieldMap[_T("C_R")];
								const string rX = (*ktr)->m_FieldMap[_T("C_X")];
								const string rID	= GetName();
								const string rPhase	= prop()->GetValue("Rating" , "Phase");
								///const string rCategory	= prop()->GetValue("Type" , "Load Category");
								const string rL		= prop()->GetValue("Cable Route"  , "Design Length");
								const int res = python.CallPyFindPowerCableSize(rID , rPhase , rL ,
									rR , rX , nPF , (nFLC*nSF) , (nBusVolt*nRunVD*0.01*1000) , nNoOfCable , CCommandPowerCableCreation::m_nRunningCalculatedVD);

								SetPowerCableProp(pCableItem->prop() , SettingRecord , nNoOfCable , *ktr , CCommandPowerCableCreation::m_nRunningCalculatedVD , nStartingCalculatedVD);
								CCommandPowerCableCreation::CreateSpaceHeaterPowerCableIfNeed(this , SettingRecord , pCableItem , rBusID);

								if(ktr == pRecordList->end())
								{
									ktr = pRecordList->begin();
									++nNoOfCable;
								}
								CCommandPowerCableCreation::m_nCheckingArea = CCommandPowerCableCreation::AFTER;
								//! 이후의 몇번의 계산을 수행하여 그 결과를 리스트에 담는다.
								FindPowerCableSize(pCableItem , pRecordList , ktr , SettingRecord , nPFatStarting , 
									(nFLC*nLRC) , (nBusVolt*1000*nStartVD*0.01) , nMinSize , nNoOfCable , nStartingCalculatedVD);
							}else	CCableItem::DeleteInstance(pCableItem);
						}
						else if((ktr != pRecordList->end()) && !bNeedCheckMotorStartApply)
						{
							SetPowerCableProp(pCableItem->prop() , SettingRecord , nNoOfCable , 
								*ktr , CCommandPowerCableCreation::m_nRunningCalculatedVD , 0.f);
							CCommandPowerCableCreation::CreateSpaceHeaterPowerCableIfNeed(this , SettingRecord , pCableItem , rBusID);

							double nRunningCalculatedVD = 0.f;
							if(ktr == pRecordList->end())
							{
								ktr = pRecordList->begin();
								++nNoOfCable;
							}
							CCommandPowerCableCreation::m_nCheckingArea = CCommandPowerCableCreation::AFTER;
							//! 이후의 몇번의 계산을 수행하여 그 결과를 리스트에 담는다.
							FindPowerCableSize(pCableItem , pRecordList , ktr , SettingRecord , 
								nPF , (nFLC*nSF) , (nBusVolt*1000*nRunVD*0.01) , nMinSize , nNoOfCable , nRunningCalculatedVD);
						}
					}else	CCableItem::DeleteInstance(pCableItem);

					CCommandPowerCableCreation::AdjustCableNumbering(this , SettingRecord , pCableItem);
				}
				else
				{
					//!
				}

				return ERROR_SUCCESS;
			}
		}
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author HumKyung Baek	

	@date 2009-08-10 오후 5:48:20	

	@param	

	@return		
*/
string ITEMS::CLoadItem::GetRunVoltageDrop(void)
{
	//assert(pLoadItem && "pLoadItem is NULL");
	string rRunningVD;

	const string rLoadCategory = CString(prop()->GetValue(_T("Type") , _T("Load Category")).c_str()).MakeUpper();

	rRunningVD = (_T("MOTOR") == rLoadCategory) ? 
		prop()->GetValue(_T("Voltage Drop") , _T("Motor Run VD")) : 
		prop()->GetValue(_T("Voltage Drop") , _T("Feeder Run VD"));

	return rRunningVD;
}

/**
	@brief	Mininum Cable Size를 구한다.

	@author HumKyung Baek	

	@date 2009-08-11 오전 10:37:33	

	@param	

	@return		
*/
int ITEMS::CLoadItem::FindMinPowerCableSize(int& nNoOfCable , const double& nMinSize , vector<CCableDataByType::Record*>* pRecordList , vector<CCableDataByType::Record*>::iterator& ktr)
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
				if(nSize < nMinSize) continue;	//! min. size 적용

				const string rUGCableAmbTemp = (*ktr)->m_FieldMap[_T("C_AMP_G")].c_str();
				const string rAGCableAmbTemp = (*ktr)->m_FieldMap[_T("C_AMP_A")].c_str();
				/// 2009.08.11
				const string rInstall = prop()->GetValue(_T("Cable Route") , _T("Install"));
				const string rTotalDF = prop()->GetValue(_T("Derating Factor") , _T("Total DF"));
				const double nFLC = GetFLCForPowerCableCreation();
				const string rSF = prop()->GetValue(_T("Voltage Drop") , _T("SF For FLC"));

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

	@author	HumKyung Baek
*/
int ITEMS::CLoadItem::FindPowerCableSize(CCableItem* pCableItem , 
	vector<CCableDataByType::Record*>* pRecordList , vector<CCableDataByType::Record*>::iterator& ktr , 
	CCableCreationTable::Record &SettingRecord , const double& nPF , const double& I , const double& nMotorVD , const double& nMinSize , int& nMultiply , double& nCalculatedVD)
{
	assert(pCableItem && pRecordList && "pCableItem or pRecordList is NULL");

	bool bFound = false;
	if((NULL != pCableItem) && (NULL != pRecordList) && (ktr != pRecordList->end()))
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		const string rBusId = prop()->GetValue(_T("Panel") , _T("Bus ID"));
		CBusItem* pBus = docData.FindBusItemByName(rBusId);
		const string rBusVolt = pBus->prop()->GetValue(_T("General") , _T("Bus Voltage"));
		const double nBusVolt = atof(rBusVolt.c_str());
		
		const double nPF100 = atof(pCableItem->prop()->GetValue(_T("General") , _T("PF")).c_str());///GetPFForPowerCableCreation();
		const double nFLC   = atof(pCableItem->prop()->GetValue(_T("General") , _T("FLC")).c_str());///GetFLCForPowerCableCreation();
		const double nSF    = atof(prop()->GetValue("Voltage Drop", "SF For FLC").c_str());
		const double nRunningVD = atof(GetRunVoltageDrop().c_str());

		ostringstream oss;
		oss << nMotorVD;
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
				if(nSize >= nMinSize)
				{
					const string rR    = (*ktr)->m_FieldMap[_T("C_R")].c_str();
					const string rX    = (*ktr)->m_FieldMap[_T("C_X")].c_str();

					const string rID		= GetName();
					const string rPhase		= prop()->GetValue("Rating" , "Phase");
					///const string rCategory	= prop()->GetValue("Type" , "Load Category");
					const string rL			= prop()->GetValue("Cable Route"  , "Design Length");
					const int res = python.CallPyFindPowerCableSize(rID , rPhase , rL ,
						rR , rX , nPF , I , nMotorVD , nMultiply , nCalculatedVD);
					CELoadItemProp* pCalculationResultProp = pCableItem->GetPropForOverCalculatedResult();
					if(pCalculationResultProp)
					{
						if(-1 == COMMAND::CCommandPowerCableCreation::m_nRunningCalculatedVD)
						{
							SetPowerCableProp(pCalculationResultProp , SettingRecord , nMultiply , *ktr , nCalculatedVD , 0.);
						}
						else
						{
							SetPowerCableProp(pCalculationResultProp , SettingRecord , nMultiply , *ktr , COMMAND::CCommandPowerCableCreation::m_nRunningCalculatedVD , nCalculatedVD);
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
					const string rID	= GetName();
					const string rPhase	= prop()->GetValue("Rating" , "Phase");
					///const string rCategory	= prop()->GetValue("Type" , "Load Category");
					const string rL		= prop()->GetValue("Cable Route"  , "Design Length");
					res[0] = python.CallPyFindPowerCableSize(rID , rPhase , rL ,
						rR , rX , nPF , I , nMotorVD , nMultiply , _nCalculatedVD);
					CELoadItemProp* pCalculationResultProp = pCableItem->GetPropForBelowCalculatedResult();
					if(pCalculationResultProp)
					{
						if(-1 == COMMAND::CCommandPowerCableCreation::m_nRunningCalculatedVD)
						{
							SetPowerCableProp(pCalculationResultProp , SettingRecord , nMultiply , *ktr , _nCalculatedVD , 0.);
						}
						else
						{
							//! RUNNING쪽의 VD를 다시 계산한다.(running쪽도 만족해야 한다.)
							res[1] = python.CallPyFindPowerCableSize(rID , rPhase , rL ,
								rR , rX , nPF100 , (nFLC*nSF) , (nBusVolt*nRunningVD*0.01*1000) , nMultiply , COMMAND::CCommandPowerCableCreation::m_nRunningCalculatedVD);

							SetPowerCableProp(pCalculationResultProp , SettingRecord , nMultiply , *ktr , COMMAND::CCommandPowerCableCreation::m_nRunningCalculatedVD , _nCalculatedVD);
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
int ITEMS::CLoadItem::SetPowerCableProp( CELoadItemProp* pItemProp , CCableCreationTable::Record &SettingRecord , const int& nNoOfCable , 
					CCableDataByType::Record* pRecord , const double& nRunningVD , const double& nStartingVD)
{
	assert(pItemProp && pRecord && "pItemProp or pRecord is NULL");

	if(pItemProp && pRecord)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		const string rBusId = prop()->GetValue(_T("Panel") , _T("Bus ID"));
		CBusItem* pBus = docData.FindBusItemByName(rBusId);
		const string rBusVolt = pBus->prop()->GetValue(_T("General") , _T("Bus Voltage"));
		const double nBusVolt = atof(rBusVolt.c_str());
		const string rArea = prop()->GetValue(_T("Location") , _T("Area"));

		const string rVoltType            = SettingRecord.FieldValueMap[_T("C_VOLTAGE_TYPE")];
		const string rCableNamePrefix     = SettingRecord.FieldValueMap[_T("C_MAIN_POWER_CABLE_NAMING_PREFIX")];
		const string rCableNameSuffix     = SettingRecord.FieldValueMap[_T("C_MAIN_POWER_CABLE_NAMING_SUFFIX")];
		
		const string rLoadName = GetName();
		const string rCableName= rCableNamePrefix + rLoadName + rCableNameSuffix;

		static const string GENERAL=_T("General");
		pItemProp->SetValue(GENERAL , _T("CABLE ID") , rCableName);
		pItemProp->SetOriginalValue(GENERAL , _T("CABLE ID") , rCableName);

		pItemProp->SetValue(GENERAL , _T("Area") , rArea);
		pItemProp->SetOriginalValue(GENERAL , _T("Area") , rArea);

		pItemProp->SetValue(GENERAL , _T("Load Id") , GetName());
		pItemProp->SetOriginalValue(GENERAL , _T("Load Id") , GetName());

		pItemProp->SetValue(GENERAL , _T("Cable Category") , _T("Power Cable"));
		pItemProp->SetOriginalValue(GENERAL , _T("Cable Category") , _T("Power Cable"));

		pItemProp->SetValue(_T("General") , _T("Description") , rLoadName + _T(" Power Cable"));
		pItemProp->SetOriginalValue(_T("General") , _T("Description") , rLoadName + _T(" Power Cable"));

		pItemProp->SetValue(GENERAL , _T("Volt Type") , rVoltType);
		pItemProp->SetOriginalValue(GENERAL , _T("Volt Type") , rVoltType);

		const string rCableVolt = prop()->GetValue(_T("Power Cable Setting") , _T("Cable Voltage"));
		pItemProp->SetValue(GENERAL , _T("Cable Volt") , rCableVolt);
		pItemProp->SetOriginalValue(GENERAL , _T("Cable Volt") , rCableVolt);

		const string rCableType = prop()->GetValue(_T("Power Cable Setting"), _T("Cable Type"));
		pItemProp->SetValue(GENERAL , _T("Cable Type"), rCableType);
		pItemProp->SetOriginalValue(GENERAL , _T("Cable Type"), rCableType);

		const string rCableSpec = prop()->GetValue(_T("Power Cable Setting") , _T("Cable Spec"));
		pItemProp->SetValue(GENERAL , _T("Cable Spec") , rCableSpec);
		pItemProp->SetOriginalValue(GENERAL , _T("Cable Spec") , rCableSpec);

		const string rCableCore = prop()->GetValue(_T("Power Cable Setting") , _T("Cable Core"));
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
		oss.str(""); oss << GetRunVoltageDrop();
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
			oss.str(""); oss << prop()->GetValue(_T("Voltage Drop") , _T("Motor Start VD"));
			pItemProp->SetValue(GENERAL , _T("Allow Starting VD%") , oss.str());
			pItemProp->SetOriginalValue(GENERAL , _T("Allow Starting VD%") , oss.str());
		}

		const string rDesignLength = prop()->GetValue(_T("Cable Route") , _T("Design Length"));
		pItemProp->SetValue(GENERAL , _T("Length") , rDesignLength);
		pItemProp->SetOriginalValue(GENERAL , _T("Length") , rDesignLength);

		///const string rLoadVolt = prop()->GetValue(_T("Rating") , _T("Rated Voltage"));
		///pItemProp->SetValue(GENERAL , _T("Load Volt") , rLoadVolt);
		///pItemProp->SetOriginalValue(GENERAL , _T("Load Volt") , rLoadVolt);

		///const string rSF = prop()->GetValue(_T("Voltage Drop") , _T("SF For FLC"));
		///pItemProp->SetValue(GENERAL , _T("SF") , oss.str());
		///pItemProp->SetOriginalValue(GENERAL , _T("SF") , oss.str());
		//!<-- General properties

		const string rInstall = prop()->GetValue(_T("Cable Route") , _T("Install"));

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
	@brief	

	@author KHS	

	@date 2009-11-09 오전 11:43:47	

	@param	

	@return		
**/
int ITEMS::CLoadItem::GetCableOfType(vector<CCableItem*>& CableItemEntry, const string& rCableType, const string& rControlTo)
{
	CableItemEntry.clear();

	for(vector<CELoadItem*>::iterator itr = m_CableRefEntry.begin(); itr != m_CableRefEntry.end();++itr)
	{
		CCableItem* pCableItem = static_cast<CCableItem*>(*itr);
		if(pCableItem)
		{
			if(rCableType == (*itr)->prop()->GetValue(_T("General"),_T("Cable Category")))
			{
				if(rCableType == _T("Control Cable") && !rControlTo.empty())
				{
					if( rControlTo == (*itr)->prop()->GetValue(_T("General"),_T("To")))
					{
						CableItemEntry.push_back(pCableItem);
					}
				}
				else if(rCableType == _T("Control Cable") && rControlTo.empty())
				{
					if(_T("LCS") != (*itr)->prop()->GetValue(_T("General"),_T("To")))
					{
						CableItemEntry.push_back(pCableItem);
					}
				}
				else
				{
					CableItemEntry.push_back(pCableItem);
				}
			}
		}
	}
	return ERROR_SUCCESS;
}

/**
	@brief	initialize cable item properties

	@author BHK	

	@date 2009-11-12

	@param	

	@return		
**/
int ITEMS::CLoadItem::InitializeCableItemProperties(CCableItem* pCableItem)
{
	assert(pCableItem && "pCableItem is NULL");

	if(pCableItem)
	{
		const string rLoadVoltage = prop()->GetValue(_T("Rating") , _T("Rated Voltage"));
		pCableItem->prop()->SetValue(_T("General") , _T("Load Volt") , rLoadVoltage);
		pCableItem->prop()->SetOriginalValue(_T("General") , _T("Load Volt") , rLoadVoltage);

		const double nRatingCapacity = GetRatingCapacityForPowerCableCreation();
		ostringstream oss;
		if(0.f != nRatingCapacity)
			oss << nRatingCapacity;
		else	oss << _T("");
		pCableItem->prop()->SetValue(_T("General") , _T("Rating Capacity") , oss.str());
		pCableItem->prop()->SetOriginalValue(_T("General") , _T("Rating Capacity") , oss.str());

		const string rLoadCategory = CString(prop()->GetValue(_T("Type") , _T("Load Category")).c_str()).MakeUpper();
		const string rUse = CString(prop()->GetValue(_T("Panel"), _T("Real KVA For Power Cable")).c_str()).MakeUpper();
		const string rConnectedBusID = prop()->GetValue(_T("Panel") , _T("Connect Bus"));
		//! 연결된 PDB BUS에서 Rating을 가져올 경우 Rating의 단위는 [KVA]이다.
		if((SUBBUS == rLoadCategory) && (_T("YES") == rUse) && !rConnectedBusID.empty())
		{
			pCableItem->prop()->SetValue(_T("General") , _T("UNIT") , _T("KVA"));
			pCableItem->prop()->SetOriginalValue(_T("General") , _T("UNIT") , _T("KVA"));
		}
		else
		{
			const string rUnit = prop()->GetUnitStr(_T("Rating") , _T("Rating Capacity"));
			pCableItem->prop()->SetValue(_T("General") , _T("UNIT") , rUnit);
			pCableItem->prop()->SetOriginalValue(_T("General") , _T("UNIT") , rUnit);
		}

		const string rSF = prop()->GetValue(_T("Voltage Drop") , _T("SF For FLC"));
		pCableItem->prop()->SetValue(_T("General") , _T("SF") , rSF);
		pCableItem->prop()->SetOriginalValue(_T("General") , _T("SF") , rSF);

		const double nFLC = GetFLCForPowerCableCreation();
		oss.str(""); oss << nFLC;
		pCableItem->prop()->SetValue(_T("General") , _T("FLC") , oss.str());
		pCableItem->prop()->SetOriginalValue(_T("General") , _T("FLC") , oss.str());

		const double nPF = GetPFForPowerCableCreation();
		oss.str(""); oss << nPF;
		pCableItem->prop()->SetValue(_T("General") , _T("PF") , oss.str());
		pCableItem->prop()->SetOriginalValue(_T("General") , _T("PF") , oss.str());

		const double nEFF = GetEFFForPowerCableCreation();
		oss.str(""); oss << nEFF;
		pCableItem->prop()->SetValue(_T("General") , _T("EFF") , oss.str());
		pCableItem->prop()->SetOriginalValue(_T("General") , _T("EFF") , oss.str());

		const string rStartingAmp = prop()->GetValue(_T("Rating") , _T("Starting Amp"));
		pCableItem->prop()->SetValue(_T("General") , _T("Staring Amp") , rStartingAmp);
		pCableItem->prop()->SetOriginalValue(_T("General") , _T("Staring Amp") , rStartingAmp);

		const string rStartingPF = prop()->GetValue(_T("Characteristic") , _T("PF at Starting"));
		pCableItem->prop()->SetValue(_T("General") , _T("PF as Staring") , rStartingPF);
		pCableItem->prop()->SetOriginalValue(_T("General") , _T("PF as Staring") , rStartingPF);

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}