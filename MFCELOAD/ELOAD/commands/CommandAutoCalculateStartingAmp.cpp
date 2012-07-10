#include "StdAfx.h"
#include "..\LoadItem.h"
#include "..\pyfromc.h"
#include "CommandAutoCalculateStartingAmp.h"

using namespace COMMAND;
CCommandAutoCalculateStartingAmp::CCommandAutoCalculateStartingAmp(void) : CEventData("" , CEventData::TYPE_NULL)
{
}

CCommandAutoCalculateStartingAmp::~CCommandAutoCalculateStartingAmp(void)
{
}

/**
	@brief	[Rating,Starting Amp]를 계산합니다.

	@param bSetOriginalValue가 true이면 original value도 value와 같은 값으로 설정한다.

	@author	BHK
*/
int COMMAND::CCommandAutoCalculateStartingAmp::Execute(const bool& bSetOriginalValue)
{
	const int nItemCount = m_SelectionSet.GetItemCount();
	for(int i = 0;i < nItemCount;++i)
	{
		CELoadItem* pItem = m_SelectionSet.GetItemAt(i);
		if(pItem->IsKindOf(CLoadItem::TypeString()))
		{
			CLoadItem* pLoadItem = static_cast<CLoadItem*>(pItem);
			const string rLoadID = pLoadItem->GetName();
			const string rStartingAmp = python.CallPyAutoCalculateStartingAmp(rLoadID);
			if(bSetOriginalValue) pLoadItem->prop()->SetOriginalValue(_T("Rating") , _T("Starting Amp") , rStartingAmp);
			pLoadItem->prop()->SetValue(_T("Rating") , _T("Starting Amp") , rStartingAmp);
/*
			const string rLRC = pLoad->prop()->GetValue(_T("Characteristic") , _T("LRC"));
			const string rFLC = pLoad->prop()->GetValue(_T("Rating") , _T("FLC"));
			if(!rLRC.empty() && !rFLC.empty())
			{
				double nStartingAmp = atof(rLRC.c_str()) * atof(rFLC.c_str()) * 0.01;
				nStartingAmp = SAFE_ROUND(nStartingAmp , 2);

				stringstream oss;
				oss.precision( 2 );					// 소수점 자릿수 2
				oss.setf(ios_base:: fixed, ios_base:: floatfield);	// 소수점 방식으로 표시됨
				oss << nStartingAmp;
				if(bSetOriginalValue) pLoad->prop()->SetOriginalValue(_T("Rating") , _T("Starting Amp") , oss.str());
				pLoad->prop()->SetValue(_T("Rating") , _T("Starting Amp") , oss.str());
			}
*/
		}
	}

	return ERROR_SUCCESS;
}
