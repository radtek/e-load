// ELoadPropList.cpp : implementation file
//

#include "stdafx.h"
#include "ELoad.h"
#include "ELoadPropList.h"
#include "PropertiesWnd.h"
#include "ELoadDocData.h"
#include "commands/CommandPowerCableCreation.h"
#include "commands/CommandAutocalculateVD.h"
#include <assert.h>
// CELoadPropList

int CELoadPropList::IDEAL_WIDTH = 320;

IMPLEMENT_DYNAMIC(CELoadPropList, CMFCPropertyGridCtrl)

CELoadPropList::CELoadPropList()
{
}

CELoadPropList::~CELoadPropList()
{
}


BEGIN_MESSAGE_MAP(CELoadPropList, CMFCPropertyGridCtrl)
	ON_REGISTERED_MESSAGE(CEventData::SELECT_ITEM , OnItemSelected)
END_MESSAGE_MAP()

/**
	@brief	

	@author BHK	

	@date 2009-04-13 오전 11:02:22	

	@param	

	@return		
*/
/*
void CELoadPropList::OnPropertyChanged(CMFCPropertyGridProperty* pProp) const
{
	if(pProp->IsKindOf(RUNTIME_CLASS(CMFCCableSizeComboProp)))
	{
		CMFCCableSizeComboProp* pCableSizeComboProp = (CMFCCableSizeComboProp*)pProp;
		if(pCableSizeComboProp->m_pItem)
		{
			const string rCableSize = CString(pCableSizeComboProp->GetValue()).operator LPCSTR();
			pCableSizeComboProp->m_pItem->prop()->SetValue(_T("General") , _T("Size") , rCableSize);

			COMMAND::CCommandAutoCalculateVD cmdAutoCalculateVD;
			cmdAutoCalculateVD.m_SelectionSet.Add(pCableSizeComboProp->m_pItem);
			cmdAutoCalculateVD.Execute(false);

			CMFCPropertyGridProperty* pCableSpecProp = GetProperty(1);
			pProp = pCableSpecProp->GetSubItem(0);
			pProp->SetValue(COleVariant(cmdAutoCalculateVD.m_rR.c_str()));
			pProp = pCableSpecProp->GetSubItem(1);
			pProp->SetValue(COleVariant(cmdAutoCalculateVD.m_rX.c_str()));
			pProp = pCableSpecProp->GetSubItem(4);
			pProp->SetValue(COleVariant(cmdAutoCalculateVD.m_rCableDia.c_str()));
			pProp = pCableSpecProp->GetSubItem(5);
			pProp->SetValue(COleVariant(cmdAutoCalculateVD.m_rCableWeight.c_str()));

			const string rRunningVD = pCableSizeComboProp->m_pItem->prop()->GetValue(_T("General") , _T("Running VD"));
			GetProperty(0)->GetSubItem(16)->SetValue(COleVariant(rRunningVD.c_str()));
			const string rRunningVDPercent = pCableSizeComboProp->m_pItem->prop()->GetValue(_T("General") , _T("Running VD%"));
			GetProperty(0)->GetSubItem(17)->SetValue(COleVariant(rRunningVDPercent.c_str()));

			const string rStartingVD = pCableSizeComboProp->m_pItem->prop()->GetValue(_T("General") , _T("Starting VD"));
			GetProperty(0)->GetSubItem(19)->SetValue(COleVariant(rStartingVD.c_str()));
			const string rStartingVDPercent = pCableSizeComboProp->m_pItem->prop()->GetValue(_T("General") , _T("Starting VD%"));
			GetProperty(0)->GetSubItem(20)->SetValue(COleVariant(rStartingVDPercent.c_str()));
		}
	}

	CMFCPropertyGridCtrl::OnPropertyChanged(pProp);
}
*/

/**
	@brief	

	@author	BHK
*/
int CELoadPropList::InitPropList(CELoadItemProp* pItemProp)
{
	assert(pItemProp && "pItemProp is NULL");

	if(pItemProp)
	{
		const size_t nSize = pItemProp->GetCategoryCount();
		for(int i = 0;i < (int)nSize;++i)
		{
			CItemPropCategory* pPropCategory = pItemProp->GetCategoryAt(i);

			if(pPropCategory) AddCategory( pPropCategory );
		}

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

// CELoadPropList message handlers
/**
	@brief	

	@author	BHK
*/
int CELoadPropList::AddCategory(CItemPropCategory* pCategory)
{
	assert(pCategory && "pCategory is NULL");

	if(pCategory && (string("VISIBLE") == pCategory->attr()))
	{
		CMFCPropEx* pGroup = new CMFCPropEx(_T(pCategory->name()));;
		const size_t nItemCount = pCategory->GetItemCount();
		for(size_t j = 0;j < nItemCount;++j)
		{
			CItemPropCategory::ItemPropTag* pItemPropTag = pCategory->GetItemAt(j);
			if(pItemPropTag)
			{
				CMFCPropertyGridProperty* pProp = AddPropSubItem(pGroup , pItemPropTag);

				if(NULL != pProp)
				{
					//! Read Only Option set
					((CMFCPropEx*)pProp)->SetAccess(pItemPropTag->rAccess.c_str());
					pProp->SetData((DWORD_PTR)pItemPropTag);	//! CItemPropCategory::ItemPropTag의 prototype* 를 저장한다.
				}
			}
		}
		///pGroup->SetData((DWORD_PTR)this);					//! ProppertyList를 저장한다.

		AddProperty (pGroup);

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
        @brief  Propery창에 아이템을 추가한다.

	@author	BHK
*/
CMFCPropertyGridProperty* CELoadPropList::AddPropSubItem( CMFCPropEx* pGroup , CItemPropCategory::ItemPropTag* pItemPropTag )
{
	assert(pGroup && "pGroup is NULL");
	assert(pItemPropTag && "pItemPropTag is NULL");

	if(pGroup && pItemPropTag)
	{
		CMFCPropertyGridProperty* pProp = NULL;
		const CString rLockType = pItemPropTag->rLockFlag.c_str();
		if(_T("INVISIBLE") == rLockType) return NULL;

		CString rValueType = pItemPropTag->rValueType.c_str();
		if((rValueType.GetLength() > 10) && ("COMBOBOX[" == rValueType.Left(9)) && ("]" == rValueType.Right(1)))
		{
			vector<string> oResult;
			rValueType = rValueType.Right(rValueType.GetLength() - 9);
			rValueType = rValueType.Left(rValueType.GetLength() - 1);
			if(_T("$BUS_VOLTAGE") == rValueType)
			{
				CMFCPropEx* pBusVoltageComboProp = new CMFCBusVoltageComboProp(_T(pItemPropTag->rKey.c_str()), _T("Bus Voltage"));
				pProp = (CMFCPropEx*)pBusVoltageComboProp;
				if(pProp)
				{
					pProp->AddOption(_T(""));	//! 이름을 눌렀을때 COMBOBOX의 버튼을 보이기위한 트릭
					pProp->AllowEdit (FALSE);
					pGroup->AddSubItem (pProp);
				}
			}
			else if(_T("$LOAD_VOLTAGE") == rValueType)
			{
				CMFCPropEx* pLoadVoltageComboProp = new CMFCLoadVoltageComboProp(_T(pItemPropTag->rKey.c_str()), _T("Load Voltage"));
				pProp = (CMFCPropEx*)pLoadVoltageComboProp;
				if(pProp)
				{
					pProp->AddOption(_T(""));	//! 이름을 눌렀을때 COMBOBOX의 버튼을 보이기위한 트릭
					pProp->AllowEdit (FALSE);
					pGroup->AddSubItem (pProp);
				}
			}
			else if(_T("$BUS") == rValueType)
			{
				CMFCPropEx* pBusNameComboProp = new CMFCBusNameComboProp(_T(pItemPropTag->rKey.c_str()), _T("BusName"));
				pProp = (CMFCPropEx*)pBusNameComboProp;
				if(pProp)
				{
					pProp->AddOption(_T(""));	//! 이름을 눌렀을때 COMBOBOX의 버튼을 보이기위한 트릭
					pProp->AllowEdit (FALSE);
					pGroup->AddSubItem (pProp);
				}
			}
			else if(_T("$CABLESIZE") == rValueType)
			{
				CMFCPropEx* pCableSizeComboProp = new CMFCCableSizeComboProp(_T(pItemPropTag->rKey.c_str()), _T("CableSize"));
				pProp = (CMFCPropEx*)pCableSizeComboProp;
				if(pProp)
				{
					pProp->AddOption(_T(""));	//! 이름을 눌렀을때 COMBOBOX의 버튼을 보이기위한 트릭
					pProp->AllowEdit (FALSE);
					pGroup->AddSubItem (pProp);
				}
			}
			else if(_T("$CABLECORE") == rValueType)
			{
				CMFCPropEx* pCableCoreComboProp = new CMFCCableCoreComboProp(_T(pItemPropTag->rKey.c_str()), _T("CableCore"));
				pProp = (CMFCPropEx*)pCableCoreComboProp;
				if(pProp)
				{
					pProp->AddOption(_T(""));	//! 이름을 눌렀을때 COMBOBOX의 버튼을 보이기위한 트릭
					pProp->AllowEdit (FALSE);
					pGroup->AddSubItem (pProp);
				}
			}
			else
			{
				CTokenizer<CIsComma>::Tokenize(oResult , rValueType.operator LPCSTR() , CIsComma());
				if(!oResult.empty())
				{
					CString rDesc;
					for(vector<string>::iterator itr = oResult.begin();itr != oResult.end();++itr)
					{
						if(!rDesc.IsEmpty()) rDesc += ",";
						rDesc += itr->c_str();
					}

					///if("UNLOCK" == pItemPropCategory->rLockFlag)
					{
						pProp = new CMFCComboItemProp(_T(pItemPropTag->rKey.c_str()), _T(oResult[0].c_str()),rLockType,
							_T(CString("One of: " + rDesc).operator LPCSTR()));
					}
					
					/*else
					{
						CMFCPropEx* pComboItemProp = new CMFCComboItemProp(_T(pItemPropCategory->rKey.c_str()), rLockType , _T(oResult[0].c_str()));
						pProp = (CMFCPropEx*)pComboItemProp;
						
					}*/
					
					
					for(vector<string>::iterator itr = oResult.begin();itr != oResult.end();++itr)
					{
						pProp->AddOption (_T(itr->c_str()));
					}
					pProp->AllowEdit (FALSE);
					pGroup->AddSubItem (pProp);
				}
			}
		}
		else if("CHECKBOX" == rValueType)
		{
			CMFCCheckBoxProp* pCheckBoxProp = new CMFCCheckBoxProp(_T(pItemPropTag->rKey.c_str()), TRUE, rLockType , _T("LOCK YES"));
			if(pCheckBoxProp)
			{
                                pProp= (CMFCCheckBoxProp*)pCheckBoxProp;
				pGroup->AddSubItem (pProp);
			}
		}
		else if((rValueType.GetLength() > 8) && ("BUTTON[" == rValueType.Left(7)) && ("]" == rValueType.Right(1)))
		{
			CString rButtonType = rValueType.Right(rValueType.GetLength() - 7);
			rButtonType = rButtonType.Left(rButtonType.GetLength() - 1);
			if(_T("$SHOW") == rButtonType)
			{
				pProp = new CMFCShowButtonProp(_T(pItemPropTag->rKey.c_str()), (_variant_t) _T(""),rLockType , _T("SHOW"));
				if(pProp)
				{
					static_cast<CMFCShowButtonProp*>(pProp)->m_pItemPropTag = pItemPropTag;
					pProp->Enable(FALSE);
					pGroup->AddSubItem (pProp);
				}
			}
			else
			{
				pProp = new CMFCButtonProp(_T(pItemPropTag->rKey.c_str()), (_variant_t) _T(""),rLockType , _T("~~~"));
				if(pProp) pGroup->AddSubItem (pProp);
			}
		}
                else
                {
			pProp = new CMFCPropEx(_T(pItemPropTag->rKey.c_str()), (_variant_t) _T(""),rLockType , _T("ELOAD"));
			if(pProp)
			{
				CMFCPropEx* pMFCPropEx = static_cast<CMFCPropEx*>(pProp);
				pMFCPropEx->SetUnitStr(pItemPropTag->rUnitList.c_str());
				pGroup->AddSubItem (pProp);
			}
		}

		return pProp;
	}

	return NULL;
}

/**
	@brief	

	@author	BHK
*/
LRESULT CELoadPropList::OnItemSelected(WPARAM wParam, LPARAM lParam)
{
	CEventData* pEventData = (CEventData*)lParam;
	if(pEventData) SetEventDataValues(pEventData);
		
	return 0;
}

/**
	@brief	

	@author	BHK
*/
int CELoadPropList::SetEventDataValues( CEventData* pEventData )
{
	assert(pEventData && "pEventData is NULL");

	if(pEventData)
	{
		BeginWaitCursor();

		size_t nPropCount = 0;
		const size_t nCategoryCount = GetPropertyCount();
		for(int i = 0;i < (int)nCategoryCount;++i)
		{
			CMFCPropertyGridProperty* pGroup = (CMFCPropertyGridProperty*)GetProperty(i);
			if(pGroup)
			{
				const string rCategoryName = pGroup->GetName();
				const size_t nItemCount = pGroup->GetSubItemsCount();
				for(int j = 0;j < (int)nItemCount;++j)
				{
					CMFCPropEx* pProp = (CMFCPropEx*)pGroup->GetSubItem(j);
					if(pProp)
					{
						const string rKeyName = pProp->GetName();
						if((NULL != pProp) && (NULL != pEventData))
						{
							string rValue       = pEventData->m_SelectionSet.GetValue( rCategoryName , rKeyName );
							string rOriginValue = pEventData->m_SelectionSet.GetOriginalValue( rCategoryName , rKeyName );
							string rUnit        = pEventData->m_SelectionSet.GetUnitStr( rCategoryName , rKeyName );
							string rValueType   = pEventData->m_SelectionSet.GetDataType( rCategoryName , rKeyName );
							const bool   bIsModified  = pEventData->m_SelectionSet.IsModified( rCategoryName , rKeyName );
							{
								pProp->SetOriginalValue((_variant_t)_T(rOriginValue.c_str()));
								pProp->SetValue((_variant_t)_T(rValue.c_str()));
								//! cable 항목일 경우...
								if(pProp->IsKindOf(RUNTIME_CLASS(CMFCCableSizeComboProp)))
								{
									CMFCCableSizeComboProp* pCableSizeComboProp = (CMFCCableSizeComboProp*)pProp;
									pCableSizeComboProp->m_pItem = pEventData->m_SelectionSet.GetItemAt(0);
								}
								else if(pProp->IsKindOf(RUNTIME_CLASS(CMFCCableCoreComboProp)))
								{
									CMFCCableCoreComboProp* pCableCoreComboProp = (CMFCCableCoreComboProp*)pProp;
									pCableCoreComboProp->m_pItem = pEventData->m_SelectionSet.GetItemAt(0);
								}

								if(pProp->IsKindOf(RUNTIME_CLASS(CMFCCheckBoxProp)))
								{
									CMFCCheckBoxProp* pCheckBoxProp = (CMFCCheckBoxProp*)pProp;
									pCheckBoxProp->OnValueChanged();
								}
								else if(pProp->IsKindOf(RUNTIME_CLASS(CMFCPropEx)))
								{
									CMFCPropEx* pPropEx = static_cast<CMFCPropEx*>(pProp);
									pPropEx->SetUnitStr( rUnit.c_str() );
									pPropEx->SetDataType(rValueType.c_str());
								}
								
								if(pEventData->m_SelectionSet.IsLockItem(rCategoryName , rKeyName))
								{
									const bool  bLock = pEventData->m_SelectionSet.IsLock( rCategoryName , rKeyName );							
									pProp->SetLock(bLock);
								}
							}
							pProp->SetModifiedFlag( bIsModified );
						}
					}
				}
			}
		}
		ModifyPropertyEnableIfNeed(pEventData->m_SelectionSet);

		EndWaitCursor();

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	Property에 value와 original value를 설정한다.

	@author	BHK
*/
int CELoadPropList::SetValue( const string& rGroup , const string& rkey , const string& rValue , const string& rOriginalValue)
{
        size_t nPropCount = 0;
        const size_t nCategoryCount = GetPropertyCount();
        for(int i = 0;i < (int)nCategoryCount;++i)
        {
                CMFCPropEx* pGroup = (CMFCPropEx*)GetProperty(i);
                if(pGroup && (rGroup == pGroup->GetName()))
                {
                        const int nItemCount = pGroup->GetSubItemsCount();
                        for(int j = 0;j < nItemCount;++j)
                        {
                                CMFCPropEx* pProp = (CMFCPropEx*)pGroup->GetSubItem(j);
                                if(pProp && (rkey == pProp->GetName()))
                                {
					if(_T("IGNORE ORIGINAL VALUE") != rOriginalValue) pProp->SetOriginalValue(COleVariant(rOriginalValue.c_str()));
                                        pProp->SetValue( COleVariant(rValue.c_str()));
/*
					if(pProp->IsKindOf(RUNTIME_CLASS(CMFCCheckBoxProp)))
					{
						CMFCCheckBoxProp* pCheckBoxProp = (CMFCCheckBoxProp*)pProp;
						pCheckBoxProp->OnValueChanged();
					}
*/

                                        return ERROR_SUCCESS;
                                }
                        }
                }
        }

        return ERROR_INVALID_PARAMETER;
}

/**
	@brief	

	@author	BHK
*/
void CELoadPropList::CommitModifiedValues(CMFCPropertyGridProperty* pProp)
{
	assert(pProp && "pProp is NULL");

	if(pProp)
	{
		const int nSubItems = pProp->GetSubItemsCount();
		for(int i = 0;i < nSubItems;++i)
		{
			CMFCPropertyGridProperty* pSubProp = pProp->GetSubItem(i);
			if(pSubProp) CommitModifiedValues(pSubProp);
		}

		pProp->SetOriginalValue(pProp->GetValue());
		if(pProp->IsKindOf(RUNTIME_CLASS(CMFCPropEx))) ((CMFCPropEx*)pProp)->SetModifiedFlag(FALSE);
	}
}

/**
	@brief	

	@author	BHK
*/
void CELoadPropList::CommitModifiedValues(void)
{
	const int nCount = GetPropertyCount();
	for(int i = 0;i < nCount;++i)
	{
		CMFCPropertyGridProperty* pProp = GetProperty(i);
		if(pProp) CommitModifiedValues(pProp);
	}

	RedrawWindow();
}

/**
	@brief	Load Category의 값이 "SubBUS"일때 동적으로 GridProperty의 Enable을 바꾼다.

	@author BHK	

	@date 2009-04-17 오전 10:17:41	

	@param	

	@return		
*/
int CELoadPropList::ModifyPropertyEnableIfNeed(CItemSelectionSet& SelectionSet)
{
	if(CLoadItem::TypeString() == SelectionSet.GetTypeString())
	{
		const string rLoadCategory = SelectionSet.GetValue( _T("Type") , _T("Load Category") );
		const int nCount = GetPropertyCount();
		for(int i = 0;i < nCount;++i)
		{
			CMFCPropertyGridProperty* pProp = GetProperty(i);
			const int nSubCount = pProp->GetSubItemsCount();
			for(int j = 0;j < nSubCount;++j)
			{
				CMFCPropertyGridProperty* pSubProp = pProp->GetSubItem(j);
				CItemPropCategory::ItemPropTag* pItemTag = (CItemPropCategory::ItemPropTag*)pSubProp->GetData();
				if(pItemTag)
				{
					const string _rCategoryName = (pItemTag->pParent) ? pItemTag->pParent->name() : _T("");
					const string _rKeyName = pItemTag->rKey;

					if((_T("Rating") == _rCategoryName) && (_T("Rating Capacity") == _rKeyName))
					{
						if(SUBBUS == rLoadCategory)
						{
							SelectionSet.SetValue(_T("Rating") , _T("Rating Capacity") , _T(""));
							pSubProp->SetValue(CString(_T("")));
						}

						pSubProp->Enable(SUBBUS != rLoadCategory);
						pSubProp->Redraw();
					}
					else if((_T("Rating") == _rCategoryName) && (_T("FLC") == _rKeyName))
					{
						pSubProp->Enable(SUBBUS == rLoadCategory);
						pSubProp->Redraw();
					}
					else if((_T("Panel") == _rCategoryName) && (_T("Connect Bus") == _rKeyName))
					{
						if(SUBBUS != rLoadCategory)
						{
							SelectionSet.SetValue(_rCategoryName , _rKeyName , _T(""));
							pSubProp->SetValue(CString(_T("")));
						}
					}
				}
			}
		}
		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}
