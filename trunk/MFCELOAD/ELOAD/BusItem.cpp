#include "StdAfx.h"
#include "ELoad.h"
#include "BusItemProp.h"
#include "BusItem.h"
#include "ELoadDocData.h"
#include "ELoadItemProp.h"

using namespace ITEMS;

IMPLEMENTS_ELOAD_ITEM(CBusItem , CELoadItem , "BUS")

CBusItem::CBusItem( ) : CELoadItem() , m_pBusGroupItem(NULL)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELoadItemProp* pItemProp = docData.m_ItemPropMap[CBusItem::GetCategoryString()];
	m_pProp = pItemProp->Clone();
        m_pProp->SetOwner(this);
}

CBusItem::~CBusItem(void)
{
	try
	{
		if(NULL != m_pProp) SAFE_DELETE(m_pProp);
	}
	catch(...)
	{
	}
}

string CBusItem::GetName() const
{
	CELoadItemProp* pProp = prop();
	if(pProp) return pProp->GetValue(_T("GENERAL") , _T("BUS ID"));

	return string("");
}

/**
	@brief	BUS ITEM의 이름을 설정한다. BUS에 속해 있는 LOAD ITEM의 BUS ITEM 이름도 변경한다.

	@author BHK	

	@date 2009-05-27 오후 3:45:46	

	@param	

	@return		
*/
int CBusItem::SetName( const string& name )
{
	CELoadItemProp* pProp = prop();
	if(pProp)
	{
		pProp->SetValue(_T("GENERAL") , _T("BUS ID") , name);
	}

	return ERROR_BAD_ENVIRONMENT;
}

void CBusItem::SetBusGroupItemPtr( CBusGroupItem* pBusGroupItem )
{
	m_pBusGroupItem = pBusGroupItem;
}

CBusGroupItem* CBusItem::GetBusGroupItemPtr() const
{
	return m_pBusGroupItem;
}

string CBusItem::GetCategoryString()
{
	return CBusItem::TypeString();
}

/**
	@brief	

	@author BHK	

	@date 2009-04-03 오후 11:06:40	

	@param	

	@return		
**/
int CBusItem::SetDeleteFlag(const bool& bDelete)
{
	CELoadItem::SetDeleteFlag(bDelete);
	
	if(true == bDelete)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		const string rBusId = GetName();
		CItemSelectionSet SelectionSet;
		docData.GetLoadItemSetInBus(SelectionSet , rBusId);
		for(int i = 0;i < SelectionSet.GetItemCount();++i)
		{
			SelectionSet.GetItemAt(i)->prop()->SetOriginalValue(_T("Panel") , _T("Bus ID") , _T(""));
			SelectionSet.GetItemAt(i)->prop()->SetValue(_T("Panel") , _T("Bus ID") , _T(""));
		}
	}

	return ERROR_SUCCESS;
}

/**
        @brief  

        @author BHK
*/
int CBusItem::Copy( CBusItem* pCopied )
{
        if(pCopied)
        {
		CELoadItem::Copy(pCopied);
                m_pBusGroupItem = pCopied->m_pBusGroupItem;

                return ERROR_SUCCESS;
        }

        return ERROR_INVALID_PARAMETER;
}

/**
	@brief	Order No를 구한다.

	@author	BHK
*/
int CBusItem::GetOrderNo() const
{
	CItemPropCategory* pCategory = prop()->GetCategoryByName("General");
	if(pCategory)
	{
		const string rOrderNo = pCategory->GetValue("Order No");
		return atoi(rOrderNo.c_str());
	}

	return -1;
}

/**
	@brief	return Bus Group Name

	@author	BHK
*/
string CBusItem::GetBusGroupName() const
{
	return m_pProp->GetValue(_T("General") , _T("Bus Group"));
}

/**
	@brief

	@author	BHK
*/
string CBusItem::TableName()
{
	static const string __TABLE_NAME__ = _T("T_BUS_ITEM");

	return __TABLE_NAME__;
}

/**
	@brief	Load Summary 계산시 나온 값들을 구한다.
	pLoadItemEntry에 들어 있는 Load들에 한해서 Load Summary를 구한다.

	@author HumKyung Baek	

	@date 2009-05-15 오후 2:54:19	

	@param	

	@return		
*/
int ITEMS::CBusItem::GetLoadSummaryCalcData(double nFactors[3] , double nKW[3] , double nKVAR[3] , vector<string> *pLoadItemEntry)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	{
		int nTotalCount = 0;

		double nLoadFactorsSum[3] = {0.f, 0.f , 0.f};
		CItemSelectionSet SelectionSet;
		const string rBusID = GetName();
		
		docData.GetLoadItemSetInBus(SelectionSet , GetName());
		const int nItemCount = SelectionSet.GetItemCount();
		for(int i = 0;i < nItemCount;++i,++nTotalCount)
		{
			CLoadItem* pLoadItem = (CLoadItem*)SelectionSet.GetItemAt(i);
			if(pLoadItem)
			{
				if(pLoadItemEntry)
				{
					vector<string>::iterator where = find(pLoadItemEntry->begin() , pLoadItemEntry->end() , pLoadItem->GetName());
					if(where == pLoadItemEntry->end()) continue;
				}

				double nLoadFactors[3] = {0.f, 0.f , 0.f} , nLoadKW[3] = {0.f , 0.f , 0.f} , nLoadKVAR[3] = {0.f , 0.f , 0.f};
				pLoadItem->GetLoadSummaryCalcData(nLoadFactors , nLoadKW , nLoadKVAR);
				nLoadFactorsSum[0] += nLoadFactors[0];
				nLoadFactorsSum[1] += nLoadFactors[1];
				nLoadFactorsSum[2] += nLoadFactors[2];
				
				nKW[0]  += nLoadKW[0];
				nKVAR[0]+= nLoadKVAR[0];
				nKW[1]  += nLoadKW[1];
				nKVAR[1]+= nLoadKVAR[1];
				nKW[2]  += nLoadKW[2];
				nKVAR[2]+= nLoadKVAR[2];
			}
		}
		
		{
			list<CELoadItem*> ItemList;
			docData.GetELoadItemListOf(ItemList , CUPSDCItem::TypeString());
			for(list<CELoadItem*>::iterator itr = ItemList.begin();itr != ItemList.end();++itr)
			{
				const string rFrom1BusID = (*itr)->prop()->GetValue(_T("From1") , _T("Bus ID"));
				const string rFrom2BusID = (*itr)->prop()->GetValue(_T("From2") , _T("Bus ID"));
				if((rBusID == rFrom1BusID) || (rBusID == rFrom2BusID))
				{
					double nUPSDCFactors[3] = {0.f, 0.f , 0.f} , nUPSDCKW[3] = {0.f , 0.f , 0.f} , nUPSDCKVAR[3] = {0.f , 0.f , 0.f};

					CUPSDCItem* pUPSDCItem = static_cast<CUPSDCItem*>(*itr);
					pUPSDCItem->GetLoadSummaryCalcData(nUPSDCFactors , nUPSDCKW , nUPSDCKVAR , rBusID);
					nLoadFactorsSum[0] += nUPSDCFactors[0];
					nLoadFactorsSum[1] += nUPSDCFactors[1];
					nLoadFactorsSum[2] += nUPSDCFactors[2];

					nKW[0]  += nUPSDCKW[0];
					nKVAR[0]+= nUPSDCKVAR[0];
					nKW[1]  += nUPSDCKW[1];
					nKVAR[1]+= nUPSDCKVAR[1];
					nKW[2]  += nUPSDCKW[2];
					nKVAR[2]+= nUPSDCKVAR[2];

					++nTotalCount;	//! 계산에 사용한 Item 갯수 증가
				}
			}
		}

		if(docData.m_bApplyTrToLoadSummary)
		{
			//! 연결된 TR이 있으면 TR에서 LoadSummary Calc. Data를 구하여 합한다.
			vector<CTransformerItem*> TrItemEntry;
			GetConnectedTrItemEntry(TrItemEntry);
			for(vector<CTransformerItem*>::iterator itr = TrItemEntry.begin();itr != TrItemEntry.end();++itr)
			{
				if((*itr)->IsDeleted()) continue;

				double nTrFactors[3] = {0.f , 0.f , 0.f} , nTrKW[3] = {0.f , 0.f , 0.f} , nTrKVAR[3] = {0.f , 0.f , 0.f};
				(*itr)->GetLoadSummaryCalcData(nTrFactors , nTrKW , nTrKVAR);
				nLoadFactorsSum[0] += nTrFactors[0];
				nLoadFactorsSum[1] += nTrFactors[1];
				nLoadFactorsSum[2] += nTrFactors[2];
	
				nKW[0]  += nTrKW[0];
				nKVAR[0]+= nTrKVAR[0];
				nKW[1]  += nTrKW[1];
				nKVAR[1]+= nTrKVAR[1];
				nKW[2]  += nTrKW[2];
				nKVAR[2]+= nTrKVAR[2];
				
				++nTotalCount;	//! 계산에 사용한 Item 갯수 증가
			}
		}

		if(nTotalCount > 0)
		{
			nFactors[0] = nLoadFactorsSum[0] / nTotalCount;
			nFactors[1] = nLoadFactorsSum[1] / nTotalCount;
			nFactors[2] = nLoadFactorsSum[2] / nTotalCount;
		}
		//! 소수점 2,3째 자리로 맞춤.
		nFactors[0] = SAFE_ROUND(nFactors[0] , 2);	//! LF
		nFactors[1] = SAFE_ROUND(nFactors[1] , 3);	//! PF
		nFactors[2] = SAFE_ROUND(nFactors[2] , 3);	//! EFF

		nKW[0] = SAFE_ROUND(nKW[0] , 2);
		nKW[1] = SAFE_ROUND(nKW[1] , 2);
		nKW[2] = SAFE_ROUND(nKW[2] , 2);
		nKVAR[0] = SAFE_ROUND(nKVAR[0] , 2);
		nKVAR[1] = SAFE_ROUND(nKVAR[1] , 2);
		nKVAR[2] = SAFE_ROUND(nKVAR[2] , 2);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	BusItem의 LoadSummary Data를 구한다.

	@author HumKyung Baek	

	@date 2009-04-01 오후 6:47:01	

	@param	

	@return		
*/
int ITEMS::CBusItem::GetLoadSummaryResultData(double& nKW , double& nKVAR , vector<string> *pLoadItemEntry)
{
	double nBusFactors[3] = {0.f , 0.f , 0.f} , nBusKW[3] = {0.f, 0.f , 0.f} , nBusKVAR[3] = {0.f , 0.f , 0.f};
	GetLoadSummaryCalcData(nBusFactors , nBusKW , nBusKVAR , pLoadItemEntry);
	
	const double nContinuous   = 0.01 * atof(prop()->GetValue(_T("General") , _T("Continuous")).c_str());
	const double nIntermittent = 0.01 * atof(prop()->GetValue(_T("General") , _T("Intermittent")).c_str());
	const double nStandby      = 0.01 * atof(prop()->GetValue(_T("General") , _T("Stand-by")).c_str());

	nKW  += nContinuous*nBusKW[0]   + nIntermittent*nBusKW[1]   + nStandby*nBusKW[2];
	nKVAR+= nContinuous*nBusKVAR[0] + nIntermittent*nBusKVAR[1] + nStandby*nBusKVAR[2];

	return ERROR_SUCCESS;
}

/**
	@brief	Bus에 속한 모든 Load들의 LoadSummary를 계산한다.

	@author BHK	

	@date 2009-04-06 오전 10:59:45	

	@param	

	@return		
*/
int ITEMS::CBusItem::CalcLoadSummary()
{
	//! 계산 결과를 담을 변수 초기화.
	double nKWSum[3]   = {0.f , 0.f , 0.f};
	double nKVARSum[3] = {0.f , 0.f , 0.f};

	CELoadDocData& docData = CELoadDocData::GetInstance();
	CItemSelectionSet SelectionSet;
	docData.GetLoadItemSetInBus(SelectionSet , GetName());
	for(int i = 0;i < SelectionSet.GetItemCount();++i)
	{
		CLoadItem* pLoadItem = static_cast<CLoadItem*>(SelectionSet.GetItemAt(i));
		if(pLoadItem)
		{
			pLoadItem->CalcLoadSummary();
			
			CString OP         = pLoadItem->prop()->GetValue(_T("Type") , _T("Operation Mode")).c_str();
			double nFactors[3] = {0.f , 0.f , 0.f} , nKW[3] = {0.f, 0.f , 0.f} , nKVAR[3] = {0.f , 0.f, 0.f};
			pLoadItem->GetLoadSummaryCalcData(nFactors , nKW , nKVAR);

			nKWSum[0] += nKW[0]; nKWSum[1] += nKW[1]; nKWSum[2] += nKW[2];
			nKVARSum[0] += nKVAR[0]; nKVARSum[1] += nKVAR[1]; nKVARSum[2] += nKVAR[2];
			/*
			if(_T("CONTINUOUS") == OP.MakeUpper())
			{
				nKWSum[0]  += nKW;
				nKVARSum[0]+= nKVAR;
			}
			else if(_T("INTERMITTENT") == OP.MakeUpper())
			{
				nKWSum[1]  += nKW;
				nKVARSum[1]+= nKVAR;
			}
			else
			{
				nKWSum[2]  += nKW;
				nKVARSum[2]+= nKVAR;
			}
			*/
		}
	}

	//! LoadSummary 계산 결과를 property에 저장한다.
	ostringstream oss;
	oss << SAFE_ROUND(nKWSum[0],2);
	prop()->SetValue(_T("Load Summary") , _T("KW_CON") , oss.str());
	oss.str("");oss << SAFE_ROUND(nKVARSum[0],2);
	prop()->SetValue(_T("Load Summary") , _T("KVAR_CON") , oss.str());

	oss.str("");oss << SAFE_ROUND(nKWSum[1],2);
	prop()->SetValue(_T("Load Summary") , _T("KW_INT") , oss.str());
	oss.str("");oss << SAFE_ROUND(nKVARSum[1],2);
	prop()->SetValue(_T("Load Summary") , _T("KVAR_INT") , oss.str());

	oss.str("");oss << SAFE_ROUND(nKWSum[2],2);
	prop()->SetValue(_T("Load Summary") , _T("KW_STD") , oss.str());
	oss.str("");oss << SAFE_ROUND(nKVARSum[2],2);
	prop()->SetValue(_T("Load Summary") , _T("KVAR_STD") , oss.str());

	return ERROR_SUCCESS;
}

/**
	@brief	BUS의 PF 값을 구한다.

	@author KHS	

	@date 2009-05-08 오후 3:06:21	

	@param	

	@return		
**/
double ITEMS::CBusItem::GetPowerFactor(void)
{
	double dAveragePF = 0.;
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CItemSelectionSet SelectionSet;
	docData.GetLoadItemSetInBus(SelectionSet,GetName());

	if(SelectionSet.GetItemCount() < 1) return 0.;

	for(int i = 0;i < SelectionSet.GetItemCount();++i)
	{
		CString rPF = SelectionSet.GetItemAt(i)->prop()->GetValue(_T("Load Summary"), _T("PF")).c_str(); 
		double dPF = atof(rPF);
		dAveragePF += dPF;
	}
	
	dAveragePF /= SelectionSet.GetItemCount();
	return dAveragePF;
}

/**
	@brief	BUS에 연결된 TR들을 구한다.

	@author BHK	

	@date 2009-05-15 오후 4:31:55	

	@param	

	@return		
*/
int ITEMS::CBusItem::GetConnectedTrItemEntry(vector<CTransformerItem*>& TrItemEntry)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	TrItemEntry.clear();

	//! 연결된 TR에서 LoadSummary를 구한다.
	list<CELoadItem*> TrItemList;
	docData.GetELoadItemListOf(TrItemList , CTransformerItem::TypeString());
	////if(pTrItemList)
	{
		for(list<CELoadItem*>::iterator itr = TrItemList.begin();itr != TrItemList.end();++itr)
		{
			if((*itr)->IsDeleted()) continue;

			const string rFromBusID = (*itr)->prop()->GetValue(_T("From") , _T("Bus Id"));
			if(rFromBusID == GetName())
			{
				vector<CTransformerItem*>::iterator where = find(TrItemEntry.begin() , TrItemEntry.end() , *itr);
				if(where == TrItemEntry.end()) TrItemEntry.push_back(static_cast<CTransformerItem*>(*itr));
			}
		}
		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	속한 BUS GROUP이 존재하는지 확인한다.

	@author BHK	

	@date 2009-06-24 오전 10:46:44	

	@param	

	@return		
*/
bool ITEMS::CBusItem::valid() const
{
	const string rBusGroupID = prop()->GetValue(_T("General") , _T("Bus Group"));
	CELoadDocData& docData = CELoadDocData::GetInstance();
	return (NULL != docData.FindBusGroupItemByName(rBusGroupID));
}

struct NotMatchPanelID : public binary_function<CELoadItem* , string , bool>
{
	bool operator()(const CELoadItem* lhs , const string& rhs) const
	{
		return (lhs->prop()->GetValue(_T("General") , _T("Panel ID")) != rhs);
	}
};

/**
	@brief	BUS에 속한 Cable Item 리스트를 구한다.

	@author BHK	

	@date 2009-09-03 오후 1:10:12	

	@param	

	@return		
*/
int ITEMS::CBusItem::GetCableList(list<CELoadItem*>& CableItemList)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	
	docData.GetELoadItemListOf(CableItemList , CCableItem::TypeString() , _T("*"));
	CableItemList.erase(
		remove_if(CableItemList.begin() , CableItemList.end() , bind2nd(NotMatchPanelID() , GetName())) , CableItemList.end());

	return ERROR_SUCCESS;
}
