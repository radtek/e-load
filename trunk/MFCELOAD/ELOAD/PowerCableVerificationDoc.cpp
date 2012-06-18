// CableSizingResultDoc.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "ELoadDocData.h"
#include "PowerCableVerificationDoc.h"
#include "CompareRevisionDataDoc.h"

int CPowerCableVerificationDoc::m_nLoadPowerCableCreation = CPowerCableVerificationDoc::ALL_POWER_CABLE;
list<CLoadItem*> CPowerCableVerificationDoc::m_LoadItemListCreateUnsizedPowerCable;

// CPowerCableVerificationDoc

IMPLEMENT_DYNCREATE(CPowerCableVerificationDoc, CDocument)

CPowerCableVerificationDoc::CPowerCableVerificationDoc()
{
}

BOOL CPowerCableVerificationDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CPowerCableVerificationDoc::~CPowerCableVerificationDoc()
{
}


BEGIN_MESSAGE_MAP(CPowerCableVerificationDoc, CDocument)
END_MESSAGE_MAP()


// CPowerCableVerificationDoc diagnostics

#ifdef _DEBUG
void CPowerCableVerificationDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CPowerCableVerificationDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CPowerCableVerificationDoc serialization

void CPowerCableVerificationDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}
#endif


// CPowerCableVerificationDoc commands
/**
	@brief	

	@author BHK	

	@date 2009-05-07 오후 1:30:35	

	@param	

	@return		
*/
BOOL CPowerCableVerificationDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	/*if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;*/
	
	return TRUE;
}

/**
	@brief	CableSizeResultData를 파일로 저장합니다.

	@author BHK	

	@date 2009-05-07 오후 1:31:06	

	@param	

	@return		
*/
int CPowerCableVerificationDoc::SaveCableSizeResultData(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string rInterestingFilePath = docData.GetProjectFolderPath() + _T("Cable Sizing Result\\Cable Sizing Result.txt");
	ofstream ofile(rInterestingFilePath.c_str());
	if(ofile.is_open())
	{
		list<CELoadItem*> BusGroupItemList;
		docData.GetELoadItemListOf(BusGroupItemList , CBusGroupItem::TypeString());
		
		if(!BusGroupItemList.empty())
		{
			for(list<CELoadItem*>::iterator itr = BusGroupItemList.begin();itr != BusGroupItemList.end();++itr)
			{
				CBusGroupItem* pBusGroupItem = static_cast<CBusGroupItem*>(*itr);
				const string rBusGroupName = pBusGroupItem->GetName();
				if((INCOMING == rBusGroupName) || pBusGroupItem->IsDeleted()) continue;
				
				for(vector<CBusItem*>::iterator jtr = pBusGroupItem->m_BusRefEntry.begin();jtr != pBusGroupItem->m_BusRefEntry.end();++jtr)
				{
					if((*jtr)->IsDeleted()) continue;

					SaveCableResultDataInBus(ofile , *jtr);
				}
			}
		}
		ofile.close();

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
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
	@brief	BUS에 들어 있는 CABLE DATA를 표기합니다.

	@author BHK	

	@date 2009-04-22 오후 7:07:03	

	@param	

	@return		
*/
int CPowerCableVerificationDoc::SaveCableResultDataInBus(ofstream& ofile , CBusItem* pBusItem)
{
	assert(pBusItem && "pBusItem is NULL");

	if(pBusItem)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();

		list<CELoadItem*> CableItemList;

		CItemSelectionSet SelectionSet;
		docData.GetLoadItemSetInBus(SelectionSet , pBusItem->GetName());
		for(int i = 0;i < SelectionSet.GetItemCount();++i)
		{
			if(SelectionSet.GetItemAt(i)->IsDeleted()) continue;
			CLoadItem* pLoadItem = static_cast<CLoadItem*>(SelectionSet.GetItemAt(i));
			
			if(CPowerCableVerificationDoc::UNSIZED_POWER_CABLE_ONLY == CPowerCableVerificationDoc::m_nLoadPowerCableCreation)
			{
				list<CLoadItem*>::iterator where = find(CPowerCableVerificationDoc::m_LoadItemListCreateUnsizedPowerCable.begin() , CPowerCableVerificationDoc::m_LoadItemListCreateUnsizedPowerCable.end() , pLoadItem);
				if(where != CPowerCableVerificationDoc::m_LoadItemListCreateUnsizedPowerCable.end())
				{
					CableItemList.insert(CableItemList.end() , pLoadItem->m_CableRefEntry.begin() , pLoadItem->m_CableRefEntry.end());
				}
			}else	CableItemList.insert(CableItemList.end() , pLoadItem->m_CableRefEntry.begin() , pLoadItem->m_CableRefEntry.end());
		}

		//! Cable Item을 LOAD,CABLE 순에 의거하여 SORTING한다.
		stable_sort(CableItemList.begin() , CableItemList.end() , SortCableItemFunc);

		for(list<CELoadItem*>::iterator itr = CableItemList.begin();itr != CableItemList.end();++itr)
		{
			if((*itr)->IsDeleted()) continue;

			CCableItem* pCableItem = static_cast<CCableItem*>(*itr);
			CELoadItemProp * pCableItemProp = pCableItem->prop();

			//! get load item
			CLoadItem* pLoadItem = pCableItem->GetLoadItemPtr();
			if(pLoadItem)
			{
				const string rBusID = pBusItem->GetName();
				ofile << rBusID.c_str() << "\t";

				const string rBusVolt = (pBusItem) ? pBusItem->prop()->GetValue(_T("General") , _T("Bus Voltage")) : _T("");
				ofile << rBusVolt.c_str() << "\t";
				
				//! LOAD ITEM
				CELoadItemProp* pLoadItemProp = pLoadItem->prop();
				const string rLoadID = pLoadItemProp->GetValue(_T("General") , _T("ITEM ID"));
				ofile << rLoadID.c_str() << "\t";

				const string rLoadVoltage = pLoadItemProp->GetValue(_T("Rating") , _T("Rated Voltage"));
				ofile << rLoadVoltage.c_str() << "\t";

				double nRatingCapacity = pLoadItem->GetRatingCapacityForPowerCableCreation();
				ostringstream oss;
				oss << nRatingCapacity;
				const string rRating = (0.f != nRatingCapacity) ? oss.str() : _T("");
				ofile << rRating.c_str() << "\t";

				const string rUnit = pLoadItemProp->GetUnitStr(_T("Rating") , _T("Rating Capacity"));
				ofile << rUnit.c_str() << "\t";

				const double nPF = pLoadItem->GetPFForPowerCableCreation();
				oss.str(""); oss << nPF;
				ofile << oss.str().c_str() << "\t";

				const double nEFF = pLoadItem->GetEFFForPowerCableCreation();
				oss.str(""); oss << nEFF;
				ofile << oss.str().c_str() << "\t";

				const double nFLC = pLoadItem->GetFLCForPowerCableCreation();
				oss.str(""); oss << nFLC;
				ofile << oss.str().c_str() << "\t";

				const string rSF = pLoadItemProp->GetValue(_T("Voltage Drop") , _T("SF For FLC"));
				ofile << rSF.c_str() << "\t";

				double nFLCxSF = nFLC * atof(rSF.c_str());
				nFLCxSF = SAFE_ROUND(nFLCxSF,2);
				CString rResult;
				rResult.Format("%.2f", nFLCxSF);
				const string rFLCxSF = rResult.operator LPCSTR();
				ofile << rFLCxSF.c_str() << "\t";

				const string rStartingAmp = pLoadItemProp->GetValue(_T("Rating") , _T("Starting Amp"));
				ofile << rStartingAmp.c_str() << "\t";

				const string rStartingPF = pLoadItemProp->GetValue(_T("Characteristic") , _T("PF at Starting"));
				ofile << rStartingPF.c_str() << "\t";

				//!CABLE ITEM
				const string rCableId = pCableItemProp->GetValue(_T("General") , _T("CABLE ID"));
				ofile << rCableId.c_str() << "\t";

				const string rCableVoltage = pCableItemProp->GetValue(_T("General") , _T("Cable Volt"));
				ofile << rCableVoltage.c_str() << "\t";

				const string rCableSpec = pCableItemProp->GetValue(_T("General") , _T("Cable Spec")); 
				ofile << rCableSpec.c_str() << "\t";

				const string rSelectedCable = pCableItemProp->GetValue(_T("General") , _T("Core"));
				ofile << rSelectedCable.c_str() << "\t";

				const string rCalculateCable = pCableItemProp->GetValue(_T("General") , _T("Size"));
				ofile << rCalculateCable.c_str() << "\t";

				const string rCableLegth = pCableItemProp->GetValue(_T("General") , _T("Length"));
				ofile << rCableLegth.c_str() << "\t";

				const string rR = pCableItemProp->GetValue(_T("Cable Spec") , _T("R"));
				ofile << rR.c_str() << "\t";

				const string rX = pCableItemProp->GetValue(_T("Cable Spec") , _T("X"));
				ofile << rX.c_str() << "\t";

				const string rCableAmp = pCableItemProp->GetValue(_T("Cable Spec") , _T("Nominal Current"));
				ofile << rCableAmp.c_str() << "\t";

				const string rDeratingFactor = pCableItemProp->GetValue(_T("Cable Spec") , _T("Derating Factor"));
				ofile << rDeratingFactor.c_str() << "\t";

				const string rVdAtRunning = pCableItem->prop()->GetValue(_T("General") , _T("Running VD"));
				ofile << rVdAtRunning.c_str() << "\t";

				string rValue = pCableItem->prop()->GetValue(_T("General") , _T("Running VD%"));
				ofile << ((rValue.empty() || _T("0") == rValue) ? _T("") : rValue + _T(" %")) << "\t";

				rValue	= pCableItem->prop()->GetValue(_T("General") , _T("Allow Running VD%"));
				ofile << ((rValue.empty() || _T("0") == rValue) ? _T("") : rValue + _T(" %")) << "\t";

				rValue	= pCableItem->prop()->GetValue(_T("General") , _T("Starting VD"));
				ofile << rValue.c_str() << "\t";

				rValue	= pCableItem->prop()->GetValue(_T("General") , _T("Starting VD%"));
				ofile << ((rValue.empty() || _T("0") == rValue) ? _T("") : rValue + _T(" %")) << "\t";

				rValue	= pCableItem->prop()->GetValue(_T("General") , _T("Allow Starting VD%"));
				ofile << ((rValue.empty() || _T("0") == rValue) ? _T("") : rValue + _T(" %")) << "\t";

				const string rRemark = pLoadItemProp->GetValue(_T("Remark") , _T("REMARK"));
				ofile << rRemark.c_str() << "\t" << std::endl;
			}
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	Product Output Folder가 생성되지 않았다면 폴더를 생성한다.

	@author BHK	

	@date 2009-05-07 오후 1:51:14	

	@param	

	@return		
*/
int CPowerCableVerificationDoc::CreateFolderIfNeed(const string& rFolderPath)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string rInterestingPath = (rFolderPath.empty()) ? 
		(docData.GetProjectFolderPath() + _T("Cable Sizing Result")) : rFolderPath;
	
	if(FALSE == ::PathIsDirectory(rInterestingPath.c_str()))
	{
		CreateDirectory(rInterestingPath.c_str() , NULL);
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
int CPowerCableVerificationMemStrategyState::OnCreate(ITEMS::CELoadItem* pItem)
{
	assert(pItem && "pItem is NULL");

	if(pItem)
	{
		ITEMS::CCableItem* pCableItem = static_cast<ITEMS::CCableItem*>(pItem);
		const string rLoadID = pCableItem->GetLoadItemPtr()->GetName();
		map<string , list<ITEMS::CELoadItem*>*>::iterator where = m_pDoc->m_LoadCableMap.find(rLoadID);
		if(where == m_pDoc->m_LoadCableMap.end()) m_pDoc->m_LoadCableMap.insert(make_pair(rLoadID , new list<ITEMS::CELoadItem*>));
		m_pDoc->m_LoadCableMap[rLoadID]->push_back(pItem);
	}

	return ERROR_BAD_ENVIRONMENT;
}

int CPowerCableVerificationMemStrategyState::OnDelete(ITEMS::CELoadItem* pItem)
{
	assert(pItem && "pItem is NULL");

	if(pItem)
	{
		ITEMS::CCableItem* pCableItem = static_cast<ITEMS::CCableItem*>(pItem);
		const string rLoadID = pCableItem->GetLoadItemPtr()->GetName();
		map<string , list<ITEMS::CELoadItem*>*>::iterator where = m_pDoc->m_LoadCableMap.find(rLoadID);
		if(where != m_pDoc->m_LoadCableMap.end())
		{
			list<ITEMS::CELoadItem*>::iterator at = find(where->second->begin() , where->second->end() , pItem);
			if(at != where->second->end()) where->second->erase(at);
		}
	}

	return ERROR_BAD_ENVIRONMENT;
}
