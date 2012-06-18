// ELoadPropTabWnd.cpp : implementation file
//

#include "stdafx.h"
#include "ELoad.h"
#include "ELoadPropTabWnd.h"
#include "ELoadPropList.h"
#include <assert.h>
#include <sstream>
#include "ELoadDocData.h"

// CELoadPropTabWnd

IMPLEMENT_DYNAMIC(CELoadPropTabWnd, CMFCTabCtrl)

CELoadPropTabWnd::CELoadPropTabWnd(const CString& rPropGroupFilePath) : m_rPropGroupFilePath(rPropGroupFilePath)
{

}

/**
        @brief  

        @author BHK
*/
CELoadPropTabWnd::~CELoadPropTabWnd()
{
        try
        {
                for(vector<CPropTabList*>::iterator itr = m_TabEntry.begin();itr != m_TabEntry.end();++itr)
                {
                        delete (*itr);
                }
                m_TabEntry.clear();
        }
        catch(...)
        {
        }
}


BEGIN_MESSAGE_MAP(CELoadPropTabWnd, CMFCTabCtrl)
	ON_REGISTERED_MESSAGE(CEventData::SELECT_ITEM , OnItemSelected)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()



// CELoadPropTabWnd message handlers
/**
	@brief	Prop Group파일을 파싱한 후에 Tab을 생성하고 Property List를 생성한다.

	@author	BHK
*/
int CELoadPropTabWnd::InitPropList(CELoadItemProp* pItemProp)
{
	if(ERROR_SUCCESS == ParsePropGroupFile( m_rPropGroupFilePath ))
	{
		UINT ID = 2;
		for(vector<CPropTabList*>::iterator itr = m_TabEntry.begin();itr != m_TabEntry.end();++itr)
		{
			CELoadPropList* pPropList = new CELoadPropList;
			if(pPropList)
			{
				CRect rectDummy;
				rectDummy.SetRectEmpty();
				if (!pPropList->Create (WS_VISIBLE | WS_CHILD, rectDummy, this, ID++))
				{
					TRACE0("Failed to create Properies Grid \n");
					continue;      // fail to create
				}
                                pPropList->EnableHeaderCtrl (FALSE);
                                pPropList->EnableDescriptionArea ();
                                pPropList->SetVSDotNetLook ();
                                pPropList->MarkModifiedProperties ();

				const int nSize = (*itr)->GetCategorySize();
				for(int i = 0;i < nSize;++i)
				{
					const string rCategoryName = (*itr)->GetCategoryNameAt(i);
					CItemPropCategory* pPropCategory = pItemProp->GetCategoryByName(rCategoryName);
				
					if(pPropCategory) pPropList->AddCategory( pPropCategory );
				}
				
				AddTab(pPropList , (*itr)->name().c_str());
			}
		}

		return ERROR_SUCCESS;
	}
	else
	{
		stringstream oss;
		oss << "Fail to parse " << m_rPropGroupFilePath;
		//! Logging request via macro
		ELOAD_LOG4CXX_WARN(mylogger, "" , 3);
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	PROP_GROUP 파일을 파싱한다.

	@author	BHK
*/
int CELoadPropTabWnd::ParsePropGroupFile(const CString& rPropGroupFilePath)
{
	ifstream ifile(rPropGroupFilePath);

	if(ifile.is_open())
	{
		string aLine;
		while(!ifile.eof())
		{
			getline(ifile , aLine);
			if(ifile.eof()) break;
			if(aLine.empty()) continue;

			IsString::TrimWhiteSpace(aLine);
			if(('[' == aLine[0]) && ']' == aLine[aLine.length() - 1])
			{
				const string& rTabName = aLine.substr(1 , aLine.length() - 2);
				CPropTabList* pTabList = new CPropTabList(rTabName);
				if(pTabList)
				{
					pTabList->Parse(ifile);
					m_TabEntry.push_back(pTabList);
				}
			}
		}

		ifile.close();

		return ERROR_SUCCESS;
	}else	ELOAD_LOG4CXX_WARN(mylogger, "" , 3);

	return ERROR_FILE_NOT_FOUND;
}

/**
	@brief	

	@author	BHK
*/
int CELoadPropTabWnd::SetEventDataValues( CEventData* pEventData )
{
	assert(pEventData && "pEventData is NULL");

	if(pEventData)
	{
		const int nTabCount = GetTabsNum();
		for(int i = 0;i < nTabCount;++i)
		{
			CELoadPropList* pPropList = static_cast<CELoadPropList*>(GetTabWnd(i));
			if(pPropList) pPropList->SendMessage(CEventData::SELECT_ITEM , 0 , (LPARAM)pEventData);
		}

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	각 Tab마다 SetValue를 호출한다.

	@author	BHK
*/
int CELoadPropTabWnd::SetValue( const string& rCategory , const string& rKey , const string& rValue , const string& rOriginalValue )
{
	const int nTabCount = GetTabsNum();
	for(int i = 0;i < nTabCount;++i)
	{
		CELoadPropList* pPropList = static_cast<CELoadPropList*>(GetTabWnd(i));
		if(pPropList) pPropList->SetValue(rCategory , rKey , rValue , rOriginalValue);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
LRESULT CELoadPropTabWnd::OnItemSelected(WPARAM wParam, LPARAM lParam)
{
	CEventData* pEventData = (CEventData*)lParam;
	if(pEventData) SetEventDataValues(pEventData);
		
	return 0;
}

/**
	@brief	called when a property is changed.

	@autor	BHK
*/
LRESULT CELoadPropTabWnd::OnPropertyChanged(WPARAM wp,  LPARAM lp)
{
        CMFCPropEx* pProp   = (CMFCPropEx*)(lp);                    //Name  =BUS ID  , Value = sds
        CMFCPropEx* pParent = (CMFCPropEx*)pProp->GetParent();
        if(pProp && pParent)
        {
                const int nTabCount = GetTabsNum();
                for(int i = 0;i < nTabCount;++i)
                {
                        CELoadPropList* pPropList = static_cast<CELoadPropList*>(GetTabWnd(i));
                        if(pPropList)
			{
				VARIANT var;
				var = pProp->GetOriginalValue();
				pPropList->SetValue(pParent->GetName() , pProp->GetName() , CString(pProp->GetValue()).operator LPCSTR() , CString(var).operator LPCSTR());
			}
                }
        }

	GetParent()->SendMessage(AFX_WM_PROPERTY_CHANGED , (WPARAM)GetDlgCtrlID() , lp);
		
        return 0;
}

/**
        @brief  

        @author BHK
*/
void CELoadPropTabWnd::SetAlphabeticMode(BOOL bSet)
{
        const int nTabCount = GetTabsNum();
        for(int i = 0;i < nTabCount;++i)
        {
                CELoadPropList* pPropList = static_cast<CELoadPropList*>(GetTabWnd(i));
                if(pPropList) pPropList->SetAlphabeticMode(bSet);
        }
}

/**
        @brief  

        @author BHK
*/
BOOL CELoadPropTabWnd::IsAlphabeticMode()
{
        const int nTabCount = GetTabsNum();
        for(int i = 0;i < nTabCount;++i)
        {
                CELoadPropList* pPropList = static_cast<CELoadPropList*>(GetTabWnd(i));
                if(pPropList) return pPropList->IsAlphabeticMode();
        }

        return FALSE;
}
/**
        @brief  : LOAD PROPERTY 창의 Original Value를 Value값으로 설정한다.

        @author : KHS
*/
void CELoadPropTabWnd::CommitModifiedValues(void)
{
        const int nTabCount = GetTabsNum();
        for(int i = 0;i < nTabCount;++i)
        {
                CELoadPropList* pPropList = static_cast<CELoadPropList*>(GetTabWnd(i));
                if(pPropList) pPropList->CommitModifiedValues();
        }
}
