#include "StdAfx.h"
#include "Eload.h"
#include "ELoadDocData.h"
#include <algorithm>
#include <Assert.h>
#include "Tables/CableSizeSettingTable.h"
#include "MainFrm.h"
#include "BusItem.h"
#include "LoadItemProp.h"
#include "VersionNo.h"
#include "RouteItem.h"
#include "TrayItem.h"
#include "UPSDCItem.h"
#include "DrumItem.h"
#include "Splash/SplashScreenFx.h"

using namespace TABLES;

#include "BusGroupItemProp.h"
typedef struct
{
        const char*     (*pfTypeString)();
	CELoadItem*     (*pfCreate)();
        CELoadItemProp* (*pfCreateProp)();
	string		(*pfTableName)();
}ELOAD_ITEM_CREATE;

ELOAD_ITEM_CREATE ELOAD_ITEM_CREATE_MAP[]=
{
	{CLoadItem::TypeString		, CLoadItem::CreateInstance		, CLoadItemProp::CreateInstance		, CLoadItem::TableName},
	{CBusGroupItem::TypeString	, CBusGroupItem::CreateInstance		, CBusGroupItemProp::CreateInstance	, CBusGroupItem::TableName},
	{CBusItem::TypeString		, CBusItem::CreateInstance		, CBusItemProp::CreateInstance		, CBusItem::TableName},
	{CTransformerItem::TypeString	, CTransformerItem::CreateInstance	, NULL					, CTransformerItem::TableName},
	{CCapacitorItem::TypeString	, CCapacitorItem::CreateInstance	, NULL					, CCapacitorItem::TableName},
	{CGeneratorItem::TypeString	, CGeneratorItem::CreateInstance	, NULL					, CGeneratorItem::TableName},
	{CCableItem::TypeString		, CCableItem::CreateInstance		, NULL					, CCableItem::TableName},
	{CRouteItem::TypeString		, CRouteItem::CreateInstance		, NULL					, CRouteItem::TableName},
	{CTrayItem::TypeString		, CTrayItem::CreateInstance		, NULL					, CTrayItem::TableName},
	{CUPSDCItem::TypeString		, CUPSDCItem::CreateInstance		, NULL					, CUPSDCItem::TableName},
	{CDrumItem::TypeString		, CDrumItem::CreateInstance		, NULL					, CDrumItem::TableName}
};

/**
	@brief	DocData가 생성될때...

	@author	BHK
*/
CELoadDocData::CELoadDocData(void) : m_pThread(NULL) , m_bIsModified(false) , m_bApplyTrToLoadSummary(TRUE) , m_nErrorCount(0)
{
	CString rExecFilePath = GetExecPath();
	int res = LoadPropFilesInDir(rExecFilePath);
	
	const string rRegExpFilePath = rExecFilePath + _T("Setting\\REGEXP.TBL");
	ReadRegExpFile(rRegExpFilePath);

	const string rColorFilePath = rExecFilePath + _T("Setting\\COLOR.TBL");
	ReadTableFile(m_ColorMap , rColorFilePath);

	const string rStringFilePath = rExecFilePath + _T("Setting\\STRING.TBL");
	ReadTableFile(m_StringMap , rStringFilePath);

	const string rErrorCodeFilePath = rExecFilePath + _T("Setting\\ERROR_CODE.DAT");
	ReadErrorCodeFile(rErrorCodeFilePath);

	///! Item을 저장하는 list를 생성하여 맵에 저장
	for(int i = 0;i < SIZE_OF_ARRAY(ELOAD_ITEM_CREATE_MAP);++i)
	{
		m_ELoadItemMap[ELOAD_ITEM_CREATE_MAP[i].pfTypeString()] = make_pair(ELOAD_ITEM_CREATE_MAP[i].pfTableName() , new list<ITEMS::CELoadItem*>);
	}

	//! read system.bus - 2011.02.10 added by HumKyung
	const string sSystemBusFilePath = rExecFilePath + _T("Setting\\SYSTEM.BUSGROUP");
	{
		ifstream ifile(sSystemBusFilePath.c_str());
		if(ifile.is_open())
		{
			string aLine;
			while(!ifile.eof())
			{
				getline(ifile , aLine);
				if(!aLine.empty()) m_lstSystemBusGroup.push_back(aLine);

				if(ifile.eof()) break;
			}
			ifile.close();
		}
	}
	//!
}

CELoadDocData& CELoadDocData::GetInstance()
{
	static CELoadDocData __INSTANCE__;

	return __INSTANCE__;
}

CELoadDocData::~CELoadDocData(void)
{
	try
	{
		{
			for(map<string,CELOADTable*>::iterator itr = m_ELoadTableMap.begin();itr != m_ELoadTableMap.end();++itr)
			{
				SAFE_DELETE(itr->second);
			}
		}

		for(map<string,CELoadItemProp*>::iterator itr = m_ItemPropMap.begin();itr != m_ItemPropMap.end();++itr)
		{
			SAFE_DELETE(itr->second);
		}
		m_ItemPropMap.clear();

		ClearELoadItemListOf(_T("*"));
		{
			for(MAP_T<string , pair<string,list<CELoadItem*>* > >::iterator itr = m_ELoadItemMap.begin();itr != m_ELoadItemMap.end();++itr)
			{
				SAFE_DELETE(itr->second.second);
			}
			m_ELoadItemMap.clear();
		}
	}
	catch(...)
	{
	}
}

/**
	@brief  Find Select BusGroup in Bus group Entry
        @author	KHS
*/
CBusGroupItem* CELoadDocData::FindBusGroupItemByName( const string& rName )
{
	list<CELoadItem*> BusGroupList;
	GetELoadItemListOf(BusGroupList , CBusGroupItem::TypeString());
        if(!rName.empty())
        {
		for(list<CELoadItem*>::iterator itr = BusGroupList.begin();itr != BusGroupList.end();++itr)
                {
			CBusGroupItem* pBusGroup = static_cast<CBusGroupItem*>(*itr);
			
			if(pBusGroup->IsDeleted()) continue;
                        if(rName == pBusGroup->GetName()) return pBusGroup;
                }
        }
	
	return NULL;
}

typedef struct
{
        const char*	(*pfTypeString)();
	string		rCateogry;
	string		rKey;
	CEventData*	(*pfCreateCommand)();
}COMMAND_COND;

#include "commands/CommandAutoCalculateFLC.h"
#include "commands/CommandAutoCalculateStartingAmp.h"
#include "commands/CommandAutoCalculateTotalDF.h"
#include "commands/CommandAutoCalculateVD.h"
#include "commands/CommandAutoFillMCCBProp.h"
#include "commands/CommandAutoFillLoadVoltage.h"
#include "commands/CommandAutoFillPhase.h"
#include "commands/CommandAutoChangeLoadVoltOfBus.h"
#include "commands/CommandAutoFillArea.h"
#include "commands/CommandAutoFillOnBusIDChanged.h"
#include "commands/CommandOnChangedRating.h"
#include "commands/CommandOnChangedUPSDCKVA.h"

COMMAND_COND COMMAND_COND_MAP[]=
{
	{CLoadItem::TypeString                  , _T("Rating")		, _T("Rated Voltage")	 , COMMAND::CCommandAutoCalculateFLC::CreateInstance},
	{CLoadItem::TypeString                  , _T("Rating")		, _T("Rating Capacity")	 , COMMAND::CCommandOnChangedRating::CreateInstance},
	{CLoadItem::TypeString                  , _T("Characteristic")	, _T("PF(100% Load")	 , COMMAND::CCommandAutoCalculateFLC::CreateInstance},
	{CLoadItem::TypeString                  , _T("Characteristic")	, _T("EFF(100% Load)")	 , COMMAND::CCommandAutoCalculateFLC::CreateInstance},
	{CLoadItem::TypeString                  , _T("Characteristic")	, _T("LRC")		 , COMMAND::CCommandAutoCalculateStartingAmp::CreateInstance},
	{CLoadItem::TypeString                  , _T("Rating")		, _T("FLC")		 , COMMAND::CCommandAutoCalculateStartingAmp::CreateInstance},
	{CLoadItem::TypeString                  , _T("Derating Factor")	, _T("Others DF")	 , COMMAND::CCommandAutoCalculateTotalDF::CreateInstance},
	{CLoadItem::TypeString                  , _T("Type")		, _T("Load Category")	 , COMMAND::CCommandAutoFillMCCBProp::CreateInstance},
	{CLoadItem::TypeString                  , _T("Location")	, _T("Area")		 , COMMAND::CCommandAutoFillArea::CreateInstance},
	
	{CCableItem::TypeString                 , _T("General")	,         _T("Core")	         , COMMAND::CCommandAutoCalculateVD::CreateInstance},
	{CCableItem::TypeString                 , _T("General")	,         _T("Size")	         , COMMAND::CCommandAutoCalculateVD::CreateInstance},
	{CCableItem::TypeString                 , _T("General")	,         _T("No Of Cables/Phase"),COMMAND::CCommandAutoCalculateVD::CreateInstance},

	{CBusItem::TypeString                   , _T("General")	,	  _T("Load Voltage")	 , COMMAND::CCommandAutoFillLoadVoltage::CreateInstance},
	{CBusItem::TypeString                   , _T("General")	,	  _T("Bus Voltage")	 , COMMAND::CCommandAutoChangeLoadVoltOfBus::CreateInstance},
	{CBusItem::TypeString                   , _T("General")	,	  _T("Phase")	 ,	   COMMAND::CCommandAutoFillPhase::CreateInstance},

	{CBusGroupItem::TypeString              , _T("General")	,	  _T("Bus Voltage")	 , COMMAND::CCommandAutoChangeLoadVoltOfBus::CreateInstance},

	///{CUPSDCItem::TypeString			, _T("UPS Capacity")	, _T("KVA")		, COMMAND::CCommandOnChangedUPSKVA::CreateInstance},
	///{CUPSDCItem::TypeString			, _T("DC Capacity")	, _T("AC Rating KVA")	, COMMAND::CCommandOnChangedDCKVA::CreateInstance}
};

COMMAND_COND PRE_EXECUTION_COMMAND_MAP[]=
{
	{CBusItem::TypeString                   , _T("General")	,	  _T("Bus ID")		 , COMMAND::CCommandAutoFillOnBusIDChanged::CreateInstance}
};

/**
	@brief	parameter에 대응하는 COMMAND를 생성한다.

	@param	rTypeString item type string
	@param	rCategory 
	@param	rKey

	@author	BHK
*/
CEventData* CELoadDocData::CreatePreExecutionCommandOf(const string& rTypeString , const string& rCategory , const string& rKey)
{
	CEventData* pCommand = NULL;
	const size_t nCount = SIZE_OF_ARRAY(PRE_EXECUTION_COMMAND_MAP);
	for(int i = 0;i < (int)nCount;++i)
	{
		if((rTypeString == PRE_EXECUTION_COMMAND_MAP[i].pfTypeString()) && (rCategory == PRE_EXECUTION_COMMAND_MAP[i].rCateogry) && (rKey == PRE_EXECUTION_COMMAND_MAP[i].rKey))
		{
			if(NULL != PRE_EXECUTION_COMMAND_MAP[i].pfCreateCommand)
				pCommand = PRE_EXECUTION_COMMAND_MAP[i].pfCreateCommand();
			break;
		}
	}

	return pCommand;
}

/**
	@brief	parameter에 대응하는 COMMAND를 생성한다.

	@param	rTypeString item type string
	@param	rCategory 
	@param	rKey

	@author	BHK
*/
CEventData* CELoadDocData::CreateAutoCalculateCommandOf(const string& rTypeString , const string& rCategory , const string& rKey)
{
	CEventData* pCommand = NULL;
	const size_t nCount = SIZE_OF_ARRAY(COMMAND_COND_MAP);
	for(int i = 0;i < (int)nCount;++i)
	{
		if((rTypeString == COMMAND_COND_MAP[i].pfTypeString()) && (rCategory == COMMAND_COND_MAP[i].rCateogry) && (rKey == COMMAND_COND_MAP[i].rKey))
		{
			if(NULL != COMMAND_COND_MAP[i].pfCreateCommand)
				pCommand = COMMAND_COND_MAP[i].pfCreateCommand();
			break;
		}
	}

	return pCommand;
}

/**
	@brief	REGEXP.TBL 파일을 읽는다.

	@author	BHK
*/
int CELoadDocData::ReadRegExpFile(const string& rRegExpFilePath)
{
	ifstream ifile(rRegExpFilePath.c_str());
	if(ifile.is_open())
	{
		string aLine;
		while(!ifile.eof())
		{
			getline(ifile , aLine);
			if('#' == aLine[0]) continue;	//! skip comments

			string::size_type at = aLine.find("=");
			if(string::npos != at)
			{
				const string rKey   = aLine.substr(0 , at);
				const string rValue = aLine.substr(at + 1, aLine.length() - at - 1);

				m_RegExpMap[rKey] = rValue;
			}

			if(ifile.eof()) break;
		}
		ifile.close();

		return ERROR_SUCCESS;
	}

	return ERROR_FILE_NOT_FOUND;
}

/**
	@brief	COLOR.TBL 파일을 읽는다.

	@author	BHK
*/
int CELoadDocData::ReadTableFile(map<string,string>& table , const string& rColorFilePath)
{
	ifstream ifile(rColorFilePath.c_str());
	if(ifile.is_open())
	{
		table.clear();

		string aLine;
		while(!ifile.eof())
		{
			getline(ifile , aLine);
			if('#' == aLine[0]) continue;	//! skip comments

			string::size_type at = aLine.find("=");
			if(string::npos != at)
			{
				const string rKey   = aLine.substr(0 , at);
				const string rValue = aLine.substr(at + 1, aLine.length() - at - 1);

				table.insert(make_pair(rKey,rValue));
			}

			if(ifile.eof()) break;
		}
		ifile.close();

		return ERROR_SUCCESS;
	}

	return ERROR_FILE_NOT_FOUND;
}

/**
	@brief	error code data파일을 읽습니다.

	@author BHK	

	@date 2009-03-31 오전 9:24:38	

	@param	

	@return		
*/
int CELoadDocData::ReadErrorCodeFile(const string& rErrorCodeFilePath)
{
	ifstream ifile(rErrorCodeFilePath.c_str());
	if(ifile.is_open())
	{
		string aLine;
		while(!ifile.eof())
		{
			getline(ifile , aLine);
			if('#' == aLine[0]) continue;	//! skip comments

			string::size_type at = aLine.find("=");
			if(string::npos != at)
			{
				const int nErrorCode     = atoi(aLine.substr(0 , at).c_str());
				const string rErrorValue = aLine.substr(at + 1, aLine.length() - at - 1);

				m_ErrorCodeMap.insert(make_pair(nErrorCode,rErrorValue));
			}

			if(ifile.eof()) break;
		}
		ifile.close();

		return ERROR_SUCCESS;
	}

	return ERROR_FILE_NOT_FOUND;
}

/**
	@brief	모듈 디렉토리 안의 PROP 파일을 찾아서 읽는다.
        
	@author	KHS
*/
int  CELoadDocData::LoadPropFilesInDir(const CString& rModulePath)
{
	CString strDir(rModulePath + "Setting\\*.PROP");

	CFileFind file;
	BOOL bFound = file.FindFile(strDir);
	if(bFound)
	{
		while(bFound)
		{
			bFound = file.FindNextFile();
			if(file.IsDots()) continue;
			CString rValue = file.GetFileTitle();
			
                        CELoadItemProp* pItemProp = NULL;
                        const size_t nCount = SIZE_OF_ARRAY(ELOAD_ITEM_CREATE_MAP);
                        for(int i = 0;i < nCount;++i)
                        {
                                if(rValue == ELOAD_ITEM_CREATE_MAP[i].pfTypeString())
                                {
					if(NULL != ELOAD_ITEM_CREATE_MAP[i].pfCreateProp)
						pItemProp = ELOAD_ITEM_CREATE_MAP[i].pfCreateProp();
					break;
                                }
                        }

			//! 등록된 값이 없으면 CELoadItemProp으로 생성한다.
			if(NULL == pItemProp) pItemProp = new CELoadItemProp;
			if(pItemProp)
			{
				pItemProp->Read(file.GetFilePath().operator LPCSTR());
				m_ItemPropMap[rValue.operator LPCSTR()] = pItemProp;	
			}
		}
		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
} 

/**
	@brief	observer를 등록한다.

	@author	BAEK HUM KYUNG
*/
int CELoadDocData::AttachObserver(CObserver* pObserver)
{
	assert(pObserver && "pObserver is NULL");

	if(pObserver)
	{
		m_ObserverEntry.push_back( pObserver );
		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	observer를 해지한다.

	@author	BHK
*/
int CELoadDocData::DetachObserver(CObserver* pObserver)
{
	assert(pObserver && "pObserver is NULL");

	if(pObserver)
	{
		vector<CObserver*>::iterator where = find(m_ObserverEntry.begin(),m_ObserverEntry.end() , pObserver);
		if(where != m_ObserverEntry.end()) m_ObserverEntry.erase(where);
	}

	return ERROR_INVALID_PARAMETER;
}

UINT CELoadDocData::StatusThreadEntry(LPVOID pVoid)
{
	return ((CELoadDocData::THREADPARAM*)pVoid)->_this->StatusThread((CELoadDocData::THREADPARAM*)pVoid);
}

/**
	@brief	

	@author	BHK
*/
UINT CELoadDocData::StatusThread(CELoadDocData::THREADPARAM* pThreadParam)
{
	assert(pThreadParam && "pThreadParam is NULL");
	if(pThreadParam)
	{
		for(vector<CObserver*>::iterator itr = m_ObserverEntry.begin();itr != m_ObserverEntry.end();++itr)
		{
			//! 이벤트를 발생시킨 곳에는 이벤트를 전달하지 않는다.
			if(pThreadParam->pFromObserver == (*itr)) continue;

			pThreadParam->EventData.m_pFromObserver = pThreadParam->pFromObserver;
			(*itr)->OnEvent( &(pThreadParam->EventData) );
		}
		
                delete((void*)pThreadParam);

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
@brief	EventData를 전달한다.

@author	BHK
*/
int CELoadDocData::DispatchEvent(CObserver* pFrom , const CEventData& EventData)
{
	assert(pFrom && "pFrom is NULL");

	if(pFrom)
	{
		THREADPARAM* pThreadParam   = new THREADPARAM();
		pThreadParam->_this         = this;
		pThreadParam->pFromObserver = pFrom;
		pThreadParam->EventData     = EventData;

		StatusThread( pThreadParam );
		if((CEventData::MODIFY_ITEM_PROP == EventData.m_eEventType) || (CEventData::DELETE_ITEM == EventData.m_eEventType)) m_bIsModified = true;

		if(true == m_bIsModified)
		{
			CMainFrame* pFrame = ELOADGetMainWnd();
			if(pFrame) pFrame->SetWindowText("ELECTRIC LOAD DATABASE SYSTEM - "STRPRODUCTVER"*");
		}

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
        @brief  Load Item을 삭제 한다.

        @author	KHS
*/
int CELoadDocData::DeleteLoadItemWithPt(CLoadItem* pLoadItem)
{
	assert(pLoadItem && "pLoadItem is NULL");

	if(pLoadItem)
	{
		try
		{
			list<CELoadItem*> LoadItemList;
			GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
			if(!LoadItemList.empty())
			{
				list<CELoadItem*>::iterator where = find(LoadItemList.begin(), LoadItemList.end(), pLoadItem);
				if(where != LoadItemList.end()) LoadItemList.erase(where);
			}
		}
		catch(exception& ex)
		{
			AfxMessageBox(ex.what());
			return ERROR_BAD_ENVIRONMENT;
		}

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	비교 연산자

	@author	BHK
*/
bool fnCompareBusGroupOrderNo ( CELoadItem* lhs , CELoadItem* rhs )
{
        assert(lhs && rhs && "lhs or rhs is NULL");

        if(lhs && rhs)
        {
	        try
	        {
			CBusGroupItem* _lhs = static_cast<CBusGroupItem*>(lhs);
			CBusGroupItem* _rhs = static_cast<CBusGroupItem*>(rhs);

		        return (_lhs->GetOrderNo() < _rhs->GetOrderNo());
	        }
	        catch(...)
	        {
		        ELOAD_LOG4CXX_ERROR(mylogger, "" , 3);
	        }
        }

	return false;
}

/**
	@brief	BUS GROUP을 ORDER NO 순으로 정렬한다. 이때 BUS GROUP에 속한 BUS또한 정렬한다.

	@author	BHK
*/
int CELoadDocData::SortBusGroupByOrderNo()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*>* pBusGroupList = GetELoadItemListPtrOf(CBusGroupItem::TypeString());
	if(pBusGroupList && !pBusGroupList->empty())
	{
		::stable_sort(pBusGroupList->begin() , pBusGroupList->end() , fnCompareBusGroupOrderNo );
		for(list<CELoadItem*>::iterator itr = pBusGroupList->begin();itr != pBusGroupList->end();++itr)
		{
			CBusGroupItem* pBusGroup = static_cast<CBusGroupItem*>(*itr);

			if(pBusGroup->IsDeleted()) continue;
			pBusGroup->SortBusByOrderNo();
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	return application setting file path

	@author	BHK
*/
CString CELoadDocData::GetAppSettingFilePath(void)
{
	CString rAppSettingFilePath = GetExecPath();
	rAppSettingFilePath += "Setting\\ELoad.ini";

	return rAppSettingFilePath;
}

/**
	@brief	사용자 ID를 설정합니다.

	@author	BHK
*/
int CELoadDocData::SetUserID(const string& rUserID)
{
	m_rUserID = rUserID;

	return ERROR_SUCCESS;
}

/**
	@brief	PROJECT MDB 파일의 경로를 리턴한다.

	@author	BHK
*/
string CELoadDocData::GetProjectMDBFilePath(void) const
{
	if(!m_ProjectInfo.m_rProjectNo.empty())
	{
		string rProjectMDB = GetProjectFolderPath();
		if('\\' != rProjectMDB.at(rProjectMDB.length() - 1)) rProjectMDB += "\\";
		rProjectMDB += GetProjectNo() + _T(".MDB");

		return rProjectMDB;
	}

	return string("");
}

/**
	@brief	COMMON LIBRARY MDB 파일의 경로를 리턴한다.

	@author	BHK
*/
STRING_T CELoadDocData::GetCommonLibraryMDBFilePath() const
{
	if(!m_ProjectInfo.m_rProjectNo.empty())
	{
		string rCommonLibraryMDB = GetProjectFolderPath();
		if('\\' != rCommonLibraryMDB.at(rCommonLibraryMDB.length() - 1)) rCommonLibraryMDB += "\\";
		rCommonLibraryMDB += _T("COMMON_LIBRARY.MDB");

		return rCommonLibraryMDB;
	}

	return string("");
}

/**
	@brief	서버 MDB 파일의 경로를 리턴한다.

	@author	BHK
*/
string CELoadDocData::GetServerMDBFilePath(void) const
{
	string rServerMDB = GetServerFolderPath();
	if('\\' != rServerMDB.at(rServerMDB.length() - 1)) rServerMDB += "\\";
	rServerMDB += "ELOAD.MDB";

	return rServerMDB;
}
/**
        @brief 현재 Nodule의 경로를 리턴한다.

	@author	BHK
*/
CString CELoadDocData::GetModulePath(void)
{
	char szFileName[MAX_PATH + 1] = {'\0', };
	GetModuleFileName(NULL , szFileName , MAX_PATH);
	CString rFilePath = szFileName;
	int at = rFilePath.ReverseFind('\\');
	rFilePath.Left(at);
	CString rModulePath = rFilePath.Left(at);
	if("\\" != rModulePath.Right(1)) rModulePath += "\\";

	return rModulePath;
}

struct MatchLoadName : binary_function<CELoadItem*,string,bool>
{
	bool operator()(CELoadItem* lhs , const string& rhs) const
	{
		return (!(lhs->IsDeleted()) && (rhs == lhs->GetName()));
	}
};

/**
	@brief	

	@author	BHK
*/
CLoadItem* CELoadDocData::FindLoadItemByName(const string& rName)
{
	list<CELoadItem*> LoadItemList;
	GetELoadItemListOf(LoadItemList , CLoadItem::TypeString(), "*");
	if(!LoadItemList.empty())
	{
		list<CELoadItem*>::iterator where = find_if(LoadItemList.begin(),LoadItemList.end(),bind2nd(MatchLoadName(),rName));
		if(where != LoadItemList.end()) return (static_cast<CLoadItem*>(*where));
	}

	return NULL;
}

/**
	@brief	BUS이름을 가지고 BUS를 찾는다.

	@author	BHK

	@return	CBusItem* or NULL
*/
CBusItem* CELoadDocData::FindBusItemByName(const string& rBusName)
{
	list<CELoadItem*> BusItemList;
	GetELoadItemListOf(BusItemList , CBusItem::TypeString(), "*");
	//if(pBusGroupList)
	{
		for(list<CELoadItem*>::iterator itr = BusItemList.begin();itr != BusItemList.end();++itr)
		{
			CBusItem* pBusItem = static_cast<CBusItem*>(*itr);

			if(pBusItem->IsDeleted()) continue;
			
			if(rBusName == pBusItem->GetName())
			{
				return pBusItem;
			}
		}
	}

	return NULL;
}

/**
	@brief	Transformer이름을 가지고 Transformer를 찾는다.

	@author	HumKyung

	@return	CTransformerItem* or NULL
*/
CTransformerItem* CELoadDocData::FindTransformerItemByName(const string& rTrName)
{
	list<CELoadItem*> TrItemList;
	GetELoadItemListOf(TrItemList , CTransformerItem::TypeString());
	//if(pList)
	{
		CString __rTrName__ = rTrName.c_str();
		__rTrName__.MakeUpper();

		for(list<CELoadItem*>::iterator itr = TrItemList.begin();itr != TrItemList.end();++itr)
		{
			if((*itr)->IsDeleted()) continue;

			CString rName = (*itr)->GetName().c_str();
			rName.MakeUpper();
			if(__rTrName__ == rName) return (CTransformerItem*)(*itr);
		}
	}

	return NULL;
}

/**
	@brief	Type과 Item 이름을 가지고 해당 Item를 찾는다.

	@author	BHK

	@return	CTransformerItem* or NULL
*/
CELoadItem* CELoadDocData::FindItemByName(const string& rTypeString , const string& rItemName)
{
	list<CELoadItem*> ItemList;
	GetELoadItemListOf(ItemList , rTypeString);
	//if(pList)
	{
		CString __rItemName__ = rItemName.c_str();
		__rItemName__.MakeUpper();

		for(list<CELoadItem*>::iterator itr = ItemList.begin();itr != ItemList.end();++itr)
		{
			if((*itr)->IsDeleted()) continue;

			CString rName = (*itr)->GetName().c_str();
			rName.MakeUpper();
			if(__rItemName__ == rName) return (*itr);
		}
	}

	return NULL;
}

/**
	@brief	return regular expression according to key

	@author	BHK
*/
string CELoadDocData::GetRegExp(const string& rKey)
{
	map<string,string>::iterator where = m_RegExpMap.find(rKey);
	if(where != m_RegExpMap.end()) return (where->second);

	return string("");
}

/**
	@brief	return color code according to color name

	@author	BHK
*/
COLORREF CELoadDocData::GetColorCode(const string& rColorName) const
{
	string res("0,0,0");

	map<string,string>::const_iterator where = m_ColorMap.find(rColorName);
	if(where != m_ColorMap.end()) res = (where->second);

	vector<string> oResult;
	CTokenizer<CIsComma>::Tokenize(oResult , res , CIsComma());
	if(3 == oResult.size())
	{
		return RGB(atoi(oResult[0].c_str()) , atoi(oResult[1].c_str()) , atoi(oResult[2].c_str()));
	}

	return RGB(0,0,0);
}

/**
	@brief	String Code에 대응하는 문자열을 리턴한다.

	@author BHK	

	@date 2009-04-23 오후 3:33:26	

	@param	

	@return		
*/
string CELoadDocData::GetStringOf(const UINT& nStringCode) const
{
	string res;

	static char __BUF__[8];
	memset(__BUF__ , '\0' , sizeof(char)*SIZE_OF_ARRAY(__BUF__));
	itoa(nStringCode , __BUF__ , 10);

	map<string,string>::const_iterator where = m_StringMap.find(__BUF__);
	if(where != m_StringMap.end()) res = (where->second);

	return res;
}

/**
	@brief 아이템 이름의 중복을 체크한다.

	@author	KHS,BHK
*/
bool CELoadDocData::IsExistItemName(const string& rTypeString , const string& rName)
{
	if(rTypeString == CLoadItem::TypeString())
	{
		list<CELoadItem*> LoadItemList;
		GetELoadItemListOf(LoadItemList , CLoadItem::TypeString() ,"*");
		if(!LoadItemList.empty())
		{
			for(list<CELoadItem*>::iterator itr = LoadItemList.begin(); itr != LoadItemList.end();++itr)
			{
				CLoadItem* pLoad = static_cast<CLoadItem*>(*itr);

				if(pLoad->IsDeleted()) continue;
				if(rName == pLoad->GetName().c_str()) return TRUE;
			}
		}
	}
	else if(rTypeString == CBusGroupItem::TypeString())
	{
		list<CELoadItem*> BusGroupList;
		GetELoadItemListOf(BusGroupList , CBusGroupItem::TypeString());
		{
			for(list<CELoadItem*>::iterator itr = BusGroupList.begin(); itr != BusGroupList.end();++itr)
			{
				CBusGroupItem* pBusGroup = static_cast<CBusGroupItem*>(*itr);

				if(pBusGroup->IsDeleted()) continue;
				if(rName == pBusGroup->GetName().c_str()) return TRUE;
			}
		}
	}
	else if(rTypeString == CBusItem::TypeString())
	{
		list<CELoadItem*> BusItemList;
		GetELoadItemListOf(BusItemList , CBusItem::TypeString());
		{
			for(list<CELoadItem*>::iterator itr = BusItemList.begin(); itr != BusItemList.end();++itr)
			{
				CBusItem* pBusItem = static_cast<CBusItem*>(*itr);

				if(pBusItem->IsDeleted()) continue;

				if(rName == (*itr)->GetName().c_str()) return TRUE;
			}
		}
	}
	else
	{
		MAP_T<string , pair<string,list<CELoadItem*>* > >::iterator where = m_ELoadItemMap.find(rTypeString);
		if(where != m_ELoadItemMap.end())
		{
			for(list<CELoadItem*>::iterator itr = where->second.second->begin(); itr != where->second.second->end();++itr)
			{
				if((*itr)->IsDeleted()) continue;
				if(rName == (*itr)->GetName().c_str()) return TRUE;
			}
		}
	}

        return FALSE;
}

/**
        @brief	ELoadItem을 저장한다.

	@param	rTableName 
	@param	rItemTypeString 

	@author	BHK
*/
int CELoadDocData::SaveELoadItemData(CADODB& adoDB , const string& rTableName , const string& rItemTypeString , 
				     CSimpleProgressBar& progressbar , const long& nTotalItemCount , long& nCurItemIndex)
{
	MAP_T<string , pair<string,list<CELoadItem*>* > >::iterator where = m_ELoadItemMap.find(rItemTypeString);
	if(where == m_ELoadItemMap.end())
	{
		AfxMessageBox(_T("해당 데이터를 찾을수 없습니다."));
		return ERROR_BAD_ENVIRONMENT;
	}
	list<CELoadItem*>* pItemList = where->second.second;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//! CABLE ITEM은 특수하게 처리한다.(저장하기 전에 기존의 모든 데이터들을 삭제한다.)
	if(rItemTypeString == CCableItem::TypeString())
	{
		adoDB.ExecuteQuery(_T("DELETE * FROM ") + rTableName);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////

	for(list<CELoadItem*>::iterator itr = pItemList->begin(); itr != pItemList->end();++itr)
	{
		const int nProgress = int((double(nCurItemIndex++) / double(nTotalItemCount)) * 100.);
		progressbar.SetStatusText(_T("Save Data") , nProgress);

		CString rQuery;
		const CString rID = (*itr)->GetId().c_str();

		if((*itr)->IsDeleted())
		{
			if((*itr)->IsDataBaseItem())
			{
				rQuery = "DELETE * FROM " + CString(rTableName.c_str()) + " WHERE C_ID='" + rID + "'";
				adoDB.ExecuteQuery(rQuery.operator LPCTSTR());
			}

			continue;
		}

		(*itr)->SaveData(adoDB , rTableName);
	}

        return ERROR_SUCCESS;
}

/**
	@brief	DocData에서 Type이 TypeString인 데이터를 Database에 저장한다.
	rTypeString이 ""이면 모든 데이터를 저장한다.

	@author	BHK
*/
int CELoadDocData::Save(const string& rTableName , const string& rTypeString , CWnd* pParentWnd)
{
	const string rProjectMDBFilePath = GetProjectMDBFilePath();

	//! MDB CONNECT
	CADODB adoDB;
	const STRING_T rDBPath = STRING_T(PROVIDER) + _T("Data Source=") + rProjectMDBFilePath + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rDBPath))
	{		
		CSimpleProgressBar progressbar;
		progressbar.Create(pParentWnd);
		
		long nTotalItemCount = 0 , nCurItemIndex = 0;
		if(rTypeString.empty())
		{
			//! 전체 아이템 개수를 구합니다.
			for(MAP_T<string , pair<string , list<CELoadItem*>*> >::iterator itr = m_ELoadItemMap.begin();itr != m_ELoadItemMap.end();++itr)
			{
				nTotalItemCount += itr->second.second->size();
			}

			for(MAP_T<string , pair<string , list<CELoadItem*>*> >::iterator itr = m_ELoadItemMap.begin();itr != m_ELoadItemMap.end();++itr)
			{
				SaveELoadItemData(adoDB , itr->second.first , itr->first , progressbar , nTotalItemCount , nCurItemIndex);
			}

			for(MAP_T<string , pair<string , list<CELoadItem*>*> >::iterator itr = m_ELoadItemMap.begin();itr != m_ELoadItemMap.end();++itr)
			{
				MAP_T<string , pair<string,list<CELoadItem*>* > >::iterator where = m_ELoadItemMap.find(itr->first);
				if(where == m_ELoadItemMap.end()) continue;
				
				list<CELoadItem*>* pItemList = where->second.second;
				for(list<CELoadItem*>::iterator jtr = pItemList->begin(); jtr != pItemList->end();++jtr)
				{
					(*jtr)->SaveBinaryData(adoDB , itr->second.first);
				}
			}
		}
		else if(!rTableName.empty() && !rTypeString.empty())
		{
			list<CELoadItem*> ItemList;
			GetELoadItemListOf(ItemList , rTypeString);
			nTotalItemCount = (nTotalItemCount + ItemList.size());

			SaveELoadItemData(adoDB , rTableName , rTypeString , progressbar , nTotalItemCount , nCurItemIndex);
			{
				MAP_T<string , pair<string,list<CELoadItem*>* > >::iterator where = m_ELoadItemMap.find(rTypeString);
				if(where != m_ELoadItemMap.end())
				{
					list<CELoadItem*>* pItemList = where->second.second;
					for(list<CELoadItem*>::iterator itr =pItemList->begin(); itr != pItemList->end();++itr)
					{
						(*itr)->SaveBinaryData(adoDB , rTableName);
					}
				}
			}
		}

		adoDB.DBDisConnect();
		m_bIsModified = false;

		return ERROR_SUCCESS;
	}
	else
	{
		AfxMessageBox("Not Connect DB... ");
		return ERROR_BAD_ENVIRONMENT;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	docdata가 수정되었는지 여부를 리턴한다.

	@author	BHK
*/
bool CELoadDocData::IsModified() const
{
	return m_bIsModified;
}

/**
	@brief	BUS에 속한 LOAD ITEM들을 구한다.
	rBusID가 Not Assigned이면 BUS에 할당되지 않은 모든 LOAD ITEM들을 구한다.

	@author	HumKyung.BAEK

	@date	?
*/
int CELoadDocData::GetLoadItemSetInBus(CItemSelectionSet& SelectionSet , const string& rBusID)
{
	list<CELoadItem*> LoadItemList;
	GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
	::stable_sort(LoadItemList.begin() , LoadItemList.end() , SortByName);
	if(NOT_ASSIGNED != rBusID)
	{
		for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
		{
			CLoadItem* pLoad = static_cast<CLoadItem*>(*itr);
			if(pLoad->GetBusId() == rBusID) SelectionSet.Add(*itr);
		}
	}
	else
	{
		for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
		{
			CLoadItem* pLoad = static_cast<CLoadItem*>(*itr);
			const string rBusID = pLoad->GetBusId();
			if(rBusID.empty() || (NULL == FindBusItemByName(rBusID))) SelectionSet.Add(*itr);
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	생성된 모든 BUS ITEM(삭제된 BUS ITEM은 제외)을 구합니다.

	@author	BHK
*/
int CELoadDocData::GetAllBusItems(vector<CBusItem*>& BusItemEntry)
{
	BusItemEntry.clear();
	
	list<CELoadItem*> BusGroupList;
	GetELoadItemListOf(BusGroupList , CBusGroupItem::TypeString());
	//if(pBusGroupList)
	{
		for(list<CELoadItem*>::iterator itr = BusGroupList.begin(); itr != BusGroupList.end();++itr)
		{
			CBusGroupItem* pBusGroup = static_cast<CBusGroupItem*>(*itr);

			if(pBusGroup->IsDeleted()) continue;
			for(vector<CBusItem*>::iterator jtr = pBusGroup->m_BusRefEntry.begin();jtr != pBusGroup->m_BusRefEntry.end();++jtr)
			{
				if((*jtr)->IsDeleted()) continue;
				BusItemEntry.push_back(*jtr);
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	Modified Flag를 설정합니다.

	@author	BHK
*/
int CELoadDocData::SetModifiedFlag(const bool& bModified)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	const CString rProjectNo = docData.GetProjectNo().c_str();
	const CString rProjectName = docData.GetProjectName().c_str();
	CString rTitle;

	m_bIsModified = bModified;
	if(true == m_bIsModified)
	{
		rTitle = _T("e-LOAD v") + CString(STRPRODUCTVER) + _T(" (") + rProjectNo + _T(", ") + rProjectName + _T(")*");
	}
	else
	{
		rTitle = _T("e-LOAD v") + CString(STRPRODUCTVER) + _T(" (") + rProjectNo + _T(", ") + rProjectName + _T(")");
	}

	CMainFrame* pFrame = ELOADGetMainWnd();
	if(pFrame) pFrame->SetTitle(rTitle);

	return ERROR_SUCCESS;
}

/**
	@brief	Property를 가지고 테이블을 생성한다.

	@param	rTableName 생성할 테이블 이름

	@author	BHK
*/
int CELoadDocData::CreateTableFromProp(CADODB& adoDB , const string& rTableName , const string& rTypeString)
{
	CELoadDocData& docData    = CELoadDocData::GetInstance();
	CELoadItemProp* pItemProp = docData.m_ItemPropMap[rTypeString];
	if(pItemProp)
	{
		const int nCategoryCount = pItemProp->GetCategoryCount();
		CString rFieldSet;
		for(int i = 0;i < nCategoryCount;++i)
		{
			CItemPropCategory* pItemCategory = pItemProp->GetCategoryAt(i);
			const string rCategoryName = pItemCategory->name();
			const int nItemCount = pItemCategory->GetItemCount();
			for(int j = 0;j < nItemCount;++j)
			{
				CItemPropCategory::ItemPropTag* pItemTag = pItemCategory->GetItemAt(j);
				const string rKey = pItemTag->rKey , rFieldType = pItemTag->rFieldType;
				string rTemp =  ("[" + rCategoryName + "," + rKey + "] " + rFieldType + " ," );
				
				//! MULTI UNIT일 경우에는 _UNIT필드를 생성한다.
				if(pItemTag->GetUnitCount() > 1) rTemp += ("[" + rCategoryName + "," + rKey + "_UNIT] " + rFieldType + " ," );
				//! "LOCK ITEM"일 경우 _LOCK 필드를 생성한다.
				if(pItemTag->IsLockItem()) rTemp += ("[" + rCategoryName + "," + rKey + "_LOCK] " + rFieldType + " ," );
				rFieldSet += rTemp.c_str();
			}
		}
		int at = rFieldSet.ReverseFind(',');
		rFieldSet = rFieldSet.Left(at);
		CString rQuery = _T("CREATE TABLE ") + CString(rTableName.c_str()) + " ( ";
		rQuery += rFieldSet;
		rQuery += _T(",[C_ID] VARCHAR(50), [C_REV] VARCHAR(50), [C_REV_DESC] VARCHAR(50), [C_DELETE] VARCHAR(50) default No);");
		try
		{
			adoDB.ExecuteQuery(rQuery.operator LPCTSTR());
		}
		catch(_com_error & e)
		{
			CString rMsg;
			rMsg.Format("*** DOG CHECK [ADO DB] => <ExecuteQuery 오류> 설명[%s] !! ***\n", (LPCSTR)e.Description());
			AfxMessageBox(rMsg);

			return ERROR_BAD_ENVIRONMENT;
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	rPropName에 대응하는 아이템을 생성한다.

	@author	HumKyung

	@date	????.??.??

	@param	rPropName

	@return	CELoadItem*
*/
CELoadItem* CELoadDocData::CreateELoadItemOf(const string& rPropName , const bool& bSetModifiedFlag)
{
	CELoadItem* pItem = NULL;
	const size_t nCount = SIZE_OF_ARRAY(ELOAD_ITEM_CREATE_MAP);
	for(int i = 0;i < (int)nCount;++i)
	{
		if(rPropName == ELOAD_ITEM_CREATE_MAP[i].pfTypeString())
		{
			if(NULL != ELOAD_ITEM_CREATE_MAP[i].pfCreate)
				pItem = ELOAD_ITEM_CREATE_MAP[i].pfCreate();
			break;
		}
	}

	if((NULL != pItem) && (true == bSetModifiedFlag)) SetModifiedFlag(true);

	return pItem;
}

/**
	@brief	컨테이너에서 rTypeString에 대응하는 아이템들을 삭제한다. 단 rTypeString이 *이면 모든 아이템들을 삭제한다.

	@author	BHK
*/
int CELoadDocData::ClearELoadItemListOf(const string& rTypeString)
{
	if(_T("*") == rTypeString)
	{
		for(MAP_T<string , pair<string,list<CELoadItem*>* > >::iterator itr = m_ELoadItemMap.begin();itr != m_ELoadItemMap.end();++itr)
		{
			for(list<CELoadItem*>::iterator jtr = itr->second.second->begin();jtr != itr->second.second->end();++jtr)
			{
				SAFE_DELETE(*jtr);
			}
			itr->second.second->clear();
		}
	}
	else
	{
		list<CELoadItem*>* pItemList = GetELoadItemListPtrOf(rTypeString);
		if(pItemList)
		{
			for(list<CELoadItem*>::iterator itr = pItemList->begin();itr != pItemList->end();++itr)
			{
				SAFE_DELETE(*itr);
			}
			pItemList->clear();
		}
	}

	return ERROR_SUCCESS;
}
/**
	@brief  CStringArray 에 같은 값이 있는지 비교, 없으면 TRUE

	@author KHS
*/
BOOL CELoadDocData::CompareCStringArray(const CStringArray& Array, const CString& rValue)
{
	for(int i=0; i < Array.GetCount();++i)
	{
		if(rValue == Array[i]) return FALSE;
	}
	return TRUE;
}

/**
	@brief  아이템에 해당하는 아이콘 NO를 리턴한다.

	@author HumKyung.BAEK
*/
int CELoadDocData::GetIconNo(CELoadItem* pItem)
{
	assert(pItem && "pItem is NULL");

	if(pItem)
	{
		const string rTypeString = pItem->GetTypeString();

		if(rTypeString == CLoadItem::TypeString())
		{
			const string rLoadCategory = pItem->prop()->GetValue(_T("Type") , _T("Load Category"));

			int nImage = 0;
			if(_T("MOTOR") == rLoadCategory) nImage = MOTOR_ICON;
			else if(SUBBUS == rLoadCategory) nImage = SUBBUS_ICON;
			else nImage = LOAD_ICON;
			
			return nImage;
		}
		else if(rTypeString == CBusItem::TypeString())
		{
			const string rBusGroup = pItem->prop()->GetValue(_T("General") , _T("Bus Group"));
			return ((SUBBUS == rBusGroup) ? SUBBUS_ICON : BUS_ICON);
		}
		else if(rTypeString == CCableItem::TypeString())
		{
			return CABLE_ICON;
		}
		else if(rTypeString == CTransformerItem::TypeString())
		{
			return TRANSFORMER_ICON;
		}
		else if(rTypeString == CCapacitorItem::TypeString())
		{
			return CAPACITOR_ICON;
		}
		else if(rTypeString == CGeneratorItem::TypeString())
		{
			return GENERATOR_ICON;
		}
		/*
		else if(rTypeString == CRouteItem::TypeString())
		{
			return ROUTE_ICON;
		}
		else if(rTypeString == CTrayItem::TypeString())
		{
			return TRAY_ICON;
		}
		*/
		else if(rTypeString == CUPSDCItem::TypeString())
		{
			return UPSDC_ICON;
		}
	}

	return 0;
}

/**
	@brief	Error Code에 해당하는 Error Value를 구합니다.

	@author BHK	

	@date 2009-03-31 오전 9:46:49	

	@param	

	@return		
*/
string CELoadDocData::GetErrorMessage(const int& nErrorCode)
{
	map<int,string>::iterator where = m_ErrorCodeMap.find(nErrorCode);
	if(where != m_ErrorCodeMap.end()) return where->second;

	return string();
}

/**
	@brief	SUBBUS ITEM 만 찾아서 리턴한다.

	@author HumKyung.BAEK	

	@date 2009-04-09 오후 1:02:11	

	@param	

	@return		
**/
int CELoadDocData::GetAllSubBUSItems(vector<CBusItem*>& SubBUSItemEntry)
{
	SubBUSItemEntry.clear();
	
	list<CELoadItem*> BusGroupList;
	GetELoadItemListOf(BusGroupList , CBusGroupItem::TypeString());
	//if(pBusGroupList)
	{
		for(list<CELoadItem*>::iterator itr = BusGroupList.begin(); itr != BusGroupList.end();++itr)
		{
			CBusGroupItem* pBusGroup = static_cast<CBusGroupItem*>(*itr);
			if(pBusGroup)
			{
				if(0 != stricmp(SUBBUS , pBusGroup->GetName().c_str())) continue;
				if(pBusGroup->IsDeleted()) continue;

				for(vector<CBusItem*>::iterator jtr = pBusGroup->m_BusRefEntry.begin();jtr != pBusGroup->m_BusRefEntry.end();++jtr)
				{
					if((*jtr)->IsDeleted()) continue;
					SubBUSItemEntry.push_back(*jtr);
				}
			}
		}
	}

	return ERROR_SUCCESS;
}
/**
	@brief	 SUBBUS를 제외한 BUS ITEM들을 리턴한다.

	@author HumKyung.BAEK	

	@date 2009-04-09 오후 1:07:20	

	@param	

	@return		
**/
int CELoadDocData::GetAllBusItemsExceptedSubBUS(vector<CBusItem*>& BusItemEntry)
{
	BusItemEntry.clear();
	
	list<CELoadItem*> BusGroupList;
	GetELoadItemListOf(BusGroupList , CBusGroupItem::TypeString());
	//if(pBusGroupList)
	{
		for(list<CELoadItem*>::iterator itr = BusGroupList.begin(); itr != BusGroupList.end();++itr)
		{
			CBusGroupItem* pBusGroup = static_cast<CBusGroupItem*>(*itr);
			if(pBusGroup)
			{
				if(SUBBUS == pBusGroup->GetName()) continue;

				if(pBusGroup->IsDeleted()) continue;

				for(vector<CBusItem*>::iterator jtr = pBusGroup->m_BusRefEntry.begin();jtr != pBusGroup->m_BusRefEntry.end();++jtr)
				{
					if((*jtr)->IsDeleted()) continue;
					BusItemEntry.push_back(*jtr);
				}
			}
		}
	}

	return ERROR_SUCCESS;
}

#include "Tables/ControlTypeTable.h"
#include "Tables/SchematicControlTypeMapTable.h"
#include "Tables/CableCreationSettingTable.h"

/**
	@brief	rTableName을 가진 Table을 리턴한다.(없을 시에는 새로 생성한다.)

	@author BHK	

	@date 2009-04-14 오후 2:41:18	

	@param	

	@return		
*/
CELOADTable* CELoadDocData::GetTableOf(const string& rTableName)
{
	struct tagTableCreationMap
	{
		const string (*fGetTableName)();
		CELOADTable* (*fCreateInstance)();
	}
	TableCreationMap[]=
	{
		{CProjectSettingTable::GetTableName		   , CProjectSettingTable::CreateInstance},
		{CControlTypeTable::GetTableName		   , CControlTypeTable::CreateInstance},
		{CSchematicControlTypeMapTable::GetTableName	   , CSchematicControlTypeMapTable::CreateInstance},
		{CMCCBSizeAutoSelectionFunctionTable::GetTableName , CMCCBSizeAutoSelectionFunctionTable::CreateInstance},
		{CPowerCableCreationSettingTable::GetTableName     , CPowerCableCreationSettingTable::CreateInstance}
	};

	map<string,CELOADTable*>::iterator where = m_ELoadTableMap.find(rTableName);
	if(where == m_ELoadTableMap.end())
	{
		CELOADTable* pTable = NULL;
		for(int i = 0;i < SIZE_OF_ARRAY(TableCreationMap);++i)
		{
			if(rTableName == TableCreationMap[i].fGetTableName())
			{
				pTable = TableCreationMap[i].fCreateInstance();
				break;
			}
		}
		if(NULL == pTable) pTable = new CELOADTable;
		if(pTable)
		{
			m_ELoadTableMap.insert(make_pair(rTableName , pTable));

			const string rProjectMDBFilePath= GetProjectMDBFilePath();
			pTable->Load(rProjectMDBFilePath , rTableName);
			return pTable;
		}

		return NULL;
	}
	return (where->second);
}

/**
	@brief	rTableName을 가진 Table을 삭제한다. rTableName이 *이면 모든 테이블을 삭제한다.

	@author HumKyung.BAEK

	@date 2010-03-08

	@param	

	@return		
*/
int CELoadDocData::ClearTableOf(const string& rTableName)
{
	map<string,CELOADTable*>::iterator where = m_ELoadTableMap.find(rTableName);
	if(where != m_ELoadTableMap.end())
	{
		SAFE_DELETE((where->second));

		m_ELoadTableMap.erase(where);
	}
	else if(rTableName == _T("*"))
	{
		for(map<string , CELOADTable* >::iterator itr = m_ELoadTableMap.begin();itr != m_ELoadTableMap.end();++itr)
		{
			SAFE_DELETE((itr->second));
		}
		m_ELoadTableMap.clear();
	}
		
	return ERROR_SUCCESS;
}

/**
	@brief	Load data from Database
	BUS나 BUS GROUP보다 LOAD를 미리 로딩해야 한다.

	@author	KHS,BHK

	@date	????.??.??

	@return	bool
*/
bool CELoadDocData::LoadFromDataBase(void)
{
	CELoadDocData& docData    = CELoadDocData::GetInstance();
	docData.ClearELoadItemListOf(_T("*"));
	CELoadItemProp* pLoadProp = docData.m_ItemPropMap[CLoadItem::TypeString()];

	//! MDB CONNECT
	string rProjectMDBFilePath = docData.GetProjectMDBFilePath();
	if(!rProjectMDBFilePath.empty())
	{
		CADODB adoDB;
		const STRING_T rDBPath = STRING_T(PROVIDER) + _T("Data Source=") + rProjectMDBFilePath + DB_PASSWORD;

		CSplashScreenFx* pSplash = CSplashScreenFx::GetInstance();
		if(TRUE == adoDB.DBConnect(rDBPath))
		{
			for(MAP_T<string , pair<string , list<CELoadItem*>*> >::iterator itr = m_ELoadItemMap.begin();itr != m_ELoadItemMap.end();++itr)
			{
				if(pSplash) pSplash->SetText(string(_T("Load ") + itr->first + _T("(s)")).c_str());
				LoadELoadItemData(adoDB , itr->second.first , itr->first);
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			//! loading하고 나서의 후처리....
			{
				list<CELoadItem*> BusItemList;
				docData.GetELoadItemListOf(BusItemList , CBusItem::TypeString());
				for(list<CELoadItem*>::iterator itr = BusItemList.begin();itr != BusItemList.end();++itr)
				{
					const string rValue = (*itr)->prop()->GetValue(_T("General") , _T("Bus Group"));
					if(!rValue.empty())
					{
						CBusGroupItem *pBusGroupItem = docData.FindBusGroupItemByName(rValue);
						if(pBusGroupItem) pBusGroupItem->AddBus(static_cast<CBusItem*>(*itr));
						/*else
						{
						(*itr)->SetDeleteFlag(true);
						}*/
					}
				}

				PostProcessAfterLoading();

				//!TRAY ITEM
				list<CELoadItem*> TrayList;
				docData.GetELoadItemListOf(TrayList , CTrayItem::TypeString());
				for(list<CELoadItem*>::iterator itr = TrayList.begin(); itr != TrayList.end();++itr)
				{
					CTrayItem* pTrayItem = static_cast<CTrayItem*>(*itr);
					const string rRouteID = pTrayItem->prop()->GetValue(_T("General"),_T("Route ID"));
					CRouteItem* pRouteItem = docData.FindRouteItemByName(rRouteID);
					if(pRouteItem) pRouteItem->AddTrayItem(pTrayItem);
				}

				//! LOAD ITEM의 value와 original value를 일치시킨다. Starting Amp값을 자동 계산한다.
				list<CELoadItem*> LoadItemList;
				docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
				for(list<CELoadItem*>::iterator itr = LoadItemList.begin();itr != LoadItemList.end();++itr)
				{
					//! BUS ID에 맞는 BUS ITEM이 없을 경우에 BUS ID를 ""로 변경한다.
					const string& rBusID = (*itr)->prop()->GetValue(_T("Panel") , _T("Bus ID"));
					CBusItem* pBusItem = docData.FindBusItemByName(rBusID);
					if(NULL == pBusItem)
					{
						(*itr)->prop()->SetOriginalValue(_T("Panel") , _T("Bus ID") , _T(""));
						(*itr)->prop()->SetValue(_T("Panel") , _T("Bus ID") , _T(""));
					}

					COMMAND::CCommandAutoCalculateStartingAmp cmdAutoCalculateStartingAmp;
					cmdAutoCalculateStartingAmp.m_SelectionSet.Add(*itr);
					cmdAutoCalculateStartingAmp.Execute(true);

					(*itr)->prop()->CommitModifiedValues();
				}

				/// Drum Item & Cable
				list<CELoadItem*> DrumItemList, CableItemList;
				docData.GetELoadItemListOf(DrumItemList, CDrumItem::TypeString());
				docData.GetELoadItemListOf(CableItemList, CCableItem::TypeString(), _T("*"));
				for(list<CELoadItem*>::iterator itr = DrumItemList.begin(); itr != DrumItemList.end();++itr)
				{
					CDrumItem* pDrumItem = static_cast<CDrumItem*>(*itr);
					if(pDrumItem)
					{
						const string rDrumId = pDrumItem->prop()->GetValue(_T("General") ,_T("Drum Id"));
						for(list<CELoadItem*>::iterator jtr = CableItemList.begin(); jtr != CableItemList.end();++jtr)
						{
							CCableItem* pCableItem = static_cast<CCableItem*>(*jtr);
							if(pCableItem)
							{
								if(rDrumId == pCableItem->prop()->GetValue(_T("General") ,_T("Drum Id")))
								{
									CDrumItem::CABLE_RECORD record;

									record.rCableId = pCableItem->GetName().c_str();
									record.rLength = pCableItem->prop()->GetValue(_T("General"),_T("Length")).c_str();
									record.rFrom = pCableItem->prop()->GetValue(_T("General"),_T("From")).c_str();
									record.rTo = pCableItem->prop()->GetValue(_T("General"),_T("To")).c_str();
									record.rArea = pCableItem->prop()->GetValue(_T("General"),_T("Area")).c_str();

									pDrumItem->AddCable(record);
								}
							}
						}
					}
				}
			}

			//! Now, sort bus group and bus by Order No.
			docData.SortBusGroupByOrderNo();
		}
		else
		{
			AfxMessageBox("Database connection error... !");
			//!CSplashScreenFx* pSplash = CSplashScreenFx::GetInstance();
			if(pSplash) pSplash->Hide();

			return false;
		}

		//!CSplashScreenFx* pSplash = CSplashScreenFx::GetInstance();
		if(pSplash) pSplash->Hide();
	}

	return true;
}

/**
	@brief	ITEM 테이블의 내용을 ITEM PROP의 내용을 비교한다.

	@param	

	@author	BHK
*/
int CELoadDocData::CompareFieldNameAndProp(const STRING_T& rFieldName , vector<STRING_T>& TableFieldNameEntry , 
					map<STRING_T , STRING_T>& ShouldBeAddedFieldNameEntry , 
					CItemPropCategory::ItemPropTag* pItemTag)
{
	assert(pItemTag && "pItemTag is NULL");

	if(pItemTag)
	{
		CString rString(rFieldName.c_str());
		rString.MakeUpper();
		vector<STRING_T>::iterator where = find(TableFieldNameEntry.begin() , TableFieldNameEntry.end() , STRING_T(rString.operator LPCTSTR()));
		if(where != TableFieldNameEntry.end())
		{
			TableFieldNameEntry.erase(where);
			if(pItemTag->GetUnitCount() > 1)
			{
				//! 만일 MULTI UNIT을 가지는 필드일 경우에 MULTI UNIT필드를 삭제한다.
				rString += _T("_UNIT");
				where = find(TableFieldNameEntry.begin(),TableFieldNameEntry.end() , STRING_T(rString.operator LPCTSTR()));
				if(where != TableFieldNameEntry.end())
					TableFieldNameEntry.erase(where);
				else	ShouldBeAddedFieldNameEntry.insert(make_pair(STRING_T(rString.operator LPCTSTR()) , pItemTag->rFieldType));

			}

			if(pItemTag->IsLockItem())
			{
				//! 만일 LOCK을 가지는 필드일 경우에 LOCK필드를 삭제한다.
				rString += _T("_LOCK");
				where = find(TableFieldNameEntry.begin(),TableFieldNameEntry.end() , STRING_T(rString.operator LPCTSTR()));
				if(where != TableFieldNameEntry.end())
					TableFieldNameEntry.erase(where);
				else	ShouldBeAddedFieldNameEntry.insert(make_pair(STRING_T(rString.operator LPCTSTR()) , pItemTag->rFieldType));
			}
		}
		else
		{
			ShouldBeAddedFieldNameEntry.insert(make_pair(rFieldName , pItemTag->rFieldType));
			if(pItemTag->IsLockItem()) ShouldBeAddedFieldNameEntry.insert(make_pair(rFieldName + _T("_LOCK") , pItemTag->rFieldType));
			if(pItemTag->GetUnitCount() > 1) ShouldBeAddedFieldNameEntry.insert(make_pair(rFieldName + _T("_UNIT") , pItemTag->rFieldType));
		}

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	DataBase에서 테이블을 읽어서 Item생성

	@author	BHK

	@param	rTableName 테이블 이름
	@param	rTypeString  item type string

	@author	BHK
*/
int CELoadDocData::LoadELoadItemData(CADODB& adoDB , const string& rTableName , const string& rTypeString)
{
	CELoadDocData& docData    = CELoadDocData::GetInstance();
	CELoadItemProp* pItemProp = docData.m_ItemPropMap[rTypeString];
	list<CELoadItem*> ELoadItemList;
	docData.GetELoadItemListOf(ELoadItemList , rTypeString);

	STRING_T rQuery(_T("SELECT * FROM ") + rTableName);
	try
	{
		if(TRUE == adoDB.OpenQuery(rQuery))
		{
			int i = 0;
			//! 우선 테이블의 필드명을 저장한다.
			vector<STRING_T> TableFieldNameEntry;
			map<STRING_T , STRING_T> ShouldBeAddedFieldNameEntry;
			const int nFieldCount = adoDB.GetFieldCount();
			for(i = 0;i < nFieldCount;++i) TableFieldNameEntry.push_back(adoDB.GetFieldName(i));

			LONG lRecordCount = 0L;
			adoDB.GetRecordCount(&lRecordCount);
			CSplashScreenFx* pSplash = CSplashScreenFx::GetInstance();
			if(pSplash)
			{
				pSplash->m_ctrlProgress.SetRange(0 , lRecordCount);
				pSplash->m_ctrlProgress.SetStep(1);
				pSplash->m_ctrlProgress.SetPos(0);
			}

			int nCount = 0;
			for(i = 0;i < lRecordCount;++i)
			{
				CELoadItem* pItem = docData.CreateELoadItemOf(rTypeString);
				if(pItem) pItem->LoadData(pItemProp , adoDB , i);
				if(pSplash) pSplash->m_ctrlProgress.SetPos(i);
			}

			//! 테이블 구조를 PROP와 일치시킨다.
			{
				const int nCategoryCount = pItemProp->GetCategoryCount();
				for(int j = 0;j < nCategoryCount;++j)
				{
					CItemPropCategory* pItemCategory = pItemProp->GetCategoryAt(j);
					const string rCategoryName = pItemCategory->name();
					const int nItemCount = pItemCategory->GetItemCount();
					for(int k = 0;k < nItemCount;++k)
					{
						CItemPropCategory::ItemPropTag* pItemTag = pItemCategory->GetItemAt(k);
						string rFieldName = rCategoryName + _T(",") + pItemTag->rKey;

						//! 테이블의 필드명과 PROP에서의 필드명을 확인하는 루틴.
						CompareFieldNameAndProp(rFieldName , TableFieldNameEntry , ShouldBeAddedFieldNameEntry , pItemTag);
						//!
					}
				}

				SyncToItemProp(adoDB , rTableName , TableFieldNameEntry , ShouldBeAddedFieldNameEntry);
			}


			return ERROR_SUCCESS;
		}
		else
		{
			//! 테이블이 없으므로 테이블을 생성한다.
			docData.CreateTableFromProp(adoDB , rTableName , rTypeString);

			return ERROR_SUCCESS;
		}
	}
	catch(_com_error &e)
	{
		CString rMsg;
		rMsg.Format("*** DOG CHECK [ADO DB] => <ExecuteQuery 오류> 설명[%s] !! ***\n", (LPCSTR)e.Description());
		AfxMessageBox(rMsg);
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	ITEM 테이블의 내용을 ITEM PROP과 일치시킨다.

	@param	rTableName 재구성할 테이블 이름.

	@author	BHK
*/
int CELoadDocData::SyncToItemProp(CADODB& adoDB , const string& rTableName , 
				const vector<STRING_T>& WillBeDeletedFieldNameEntry , 
				const map<STRING_T , STRING_T>& ShouldBeAddedFieldNameEntry)
{
	//! 필드 추가 부분
	for(map<STRING_T , STRING_T>::const_iterator itr  = ShouldBeAddedFieldNameEntry.begin();itr != ShouldBeAddedFieldNameEntry.end();++itr)
	{
		adoDB.ExecuteQuery(_T("ALTER TABLE ") + rTableName + _T(" ADD [") + itr->first + _T("] ") + itr->second);
	}
        
	//! 필드 삭제 부분
	for(vector<STRING_T>::const_iterator itr  = WillBeDeletedFieldNameEntry.begin();itr != WillBeDeletedFieldNameEntry.end();++itr)
	{
		bool bAddedField = false;
		const size_t nAddedCount = SIZE_OF_ARRAY(CELoadItem::ADDED_FIELD_NAME);
		for(int i = 0;i < (int)nAddedCount;++i)
		{
			if(*itr == CELoadItem::ADDED_FIELD_NAME[i].c_str())
			{
				bAddedField = true;
				break;
			}
		}

		if(!bAddedField) adoDB.ExecuteQuery(_T("ALTER TABLE ") + rTableName + _T(" DROP [") + *itr + _T("]"));
	}

	return ERROR_SUCCESS;
}

/**
	@brief	TRAY NAME으로 TRAY ITEM을 찾는다.

	@author KHS	

	@date 2009-05-20 오후 3:35:14	

	@param	

	@return		
**/
CTrayItem* CELoadDocData::FindTrayItemByName(const string& rTrayName)
{
	list<CELoadItem*> TrayList;
	GetELoadItemListOf(TrayList , CTrayItem::TypeString());
	if(!TrayList.empty())
	{
		for(list<CELoadItem*>::iterator itr = TrayList.begin();itr != TrayList.end();++itr)
		{
			CTrayItem* pTrayItem = static_cast<CTrayItem*>(*itr);

			if(pTrayItem->IsDeleted()) continue;
			if(rTrayName == pTrayItem->GetName()) return pTrayItem;
		}
	}

	return NULL;
}

/**
	@brief	TRAY NAME으로 TRAY ITEM을 찾는다.

	@author KHS	

	@date 2009-05-20 오후 3:35:14	

	@param	

	@return		
**/
CRouteItem* CELoadDocData::FindRouteItemByName(const string& rRouteName)
{
	list<CELoadItem*> RouteList;
	GetELoadItemListOf(RouteList , CRouteItem::TypeString());
	if(!rRouteName.empty())
	{
		for(list<CELoadItem*>::iterator itr = RouteList.begin();itr != RouteList.end();++itr)
		{
			CRouteItem* pRouteItem = static_cast<CRouteItem*>(*itr);

			if(pRouteItem->IsDeleted()) continue;
			if(rRouteName == pRouteItem->GetName()) return pRouteItem;
		}
	}

	return NULL;
}

/**
	@brief	Database에서 Item을 로딩하고 나서 후처리를 한다.

	@author HumKyung

	@date 2009-08-28 오후 3:17:06	

	@param	

	@return		
*/
int CELoadDocData::PostProcessAfterLoading(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	
	//! get bus list from bus group or bus when connection is set as 'BUS GROUP' or 'BUS'
	//! and then set connection 'DEL' - 2011.02.12 added by Humkyung
	list<CELoadItem*> TrItemList;
	docData.GetELoadItemListOf(TrItemList , CTransformerItem::TypeString());
	for(list<CELoadItem*>::iterator itr = TrItemList.begin();itr != TrItemList.end();++itr)
	{
		const string sToBus = (*itr)->prop()->GetValue(_T("To") , _T("Bus Id"));
		CBusItem* pBusItem = docData.FindBusItemByName(sToBus);
		if(pBusItem)
		{
			const string sToBusGroupName = pBusItem->prop()->GetValue(_T("General") , _T("Bus Group"));

			string sBusGroupNameList , sBusNameList;
			const string sConnection = (*itr)->prop()->GetValue(_T("Capacity") , _T("Connection"));
			if(_T("BUS") == sConnection)
			{
				sBusGroupNameList += sToBusGroupName;
				sBusNameList += sToBus;

			}
			else if(_T("BUS GROUP") == sConnection)
			{
				CBusGroupItem* pBusGroup = docData.FindBusGroupItemByName(sToBusGroupName);
				if(pBusGroup)
				{
					for(vector<CBusItem*>::iterator itr = pBusGroup->m_BusRefEntry.begin(); itr != pBusGroup->m_BusRefEntry.end();++itr)
					{
						if((*itr)->IsDeleted()) continue;
						
						sBusGroupNameList += sToBusGroupName;
						sBusNameList += (*itr)->GetName();
						if((itr + 1) != pBusGroup->m_BusRefEntry.end())
						{
							sBusGroupNameList += _T(",");
							sBusNameList += _T(",");
						}
					}
				}
			}

			if(_T("DEL") != sConnection)
			{
				(*itr)->prop()->SetValue(_T("Capacity") , _T("Bus Group") , sBusGroupNameList);
				(*itr)->prop()->SetOriginalValue(_T("Capacity") , _T("Bus Group") , sBusGroupNameList);
				(*itr)->prop()->SetValue(_T("Capacity") , _T("Bus") , sBusNameList);
				(*itr)->prop()->SetOriginalValue(_T("Capacity") , _T("Bus") , sBusNameList);

				(*itr)->prop()->SetValue(_T("Capacity") , _T("Connection") , _T("DEL"));
				(*itr)->prop()->SetOriginalValue(_T("Capacity") , _T("Connection") , _T("DEL"));

				SetModifiedFlag(true);
			}
		}
	}
	//!

	CELOADTable* pTable = GetTableOf(_T("T_ROUTE_GROUP"));

	list<CELoadItem*> CableList;
	docData.GetELoadItemListOf(CableList , CCableItem::TypeString());

	CSplashScreenFx* pSplash = CSplashScreenFx::GetInstance();
	if(pSplash)
	{
		pSplash->SetText(_T("Post-Processing"));
		pSplash->m_ctrlProgress.SetRange(0 , CableList.size());
		pSplash->m_ctrlProgress.SetStep(1);
		pSplash->m_ctrlProgress.SetPos(0);
	}

	long nIndex = 0;
	for(list<CELoadItem*>::iterator itr = CableList.begin();itr != CableList.end();++itr , ++nIndex)
	{
		CCableItem* pCableItem = static_cast<CCableItem*>(*itr);
		const string& rLoadID = pCableItem->prop()->GetValue(_T("General") , _T("Load Id"));
		/////// Add Cable In Tray
		const string rTrayList = pCableItem->prop()->GetValue(_T("Tray"), _T("Tray List"));
		if(!rTrayList.empty())
		{
			vector<string> oResult;
			CTokenizer<CIsComma>::Tokenize(oResult, rTrayList, CIsComma());
			for(int i=0;i < int(oResult.size());++i)
			{
				string rTrayID = oResult[i];
				CTrayItem* pTrayItem = docData.FindTrayItemByName(rTrayID);
				if(pTrayItem) pTrayItem->AddCableItem(pCableItem);
			}
		}

		CLoadItem* pLoadItem = docData.FindLoadItemByName(rLoadID);
		if(pLoadItem)
		{
			pLoadItem->AddCable(pCableItem);
			pCableItem->SetLoadItemPtr(pLoadItem);

			//!ROUTE에 CABLE ADD
			if(pTable)
			{
				const string rRouteGroup = pLoadItem->prop()->GetValue(_T("Cable Route"), _T("Route Group"));
				for(vector<CELOADTable::Record>::iterator jtr = pTable->m_RecordEntry.begin(); jtr != pTable->m_RecordEntry.end();++jtr)
				{
					if(rRouteGroup == jtr->FieldValueMap[_T("C_ROUTE_GROUP")])
					{
						const string rRouteID = jtr->FieldValueMap[_T("C_ROUTE_TYPE")];
						CRouteItem* pRouteItem = docData.FindRouteItemByName(rRouteID);
						if(pRouteItem)	
						{
							const string  rCableCategory = pCableItem->prop()->GetValue(_T("General"), _T("Cable Category"));
							const string  rCreatedBy = pCableItem->prop()->GetValue(_T("General") , _T("Created By"));
							if(_T("Power Cable") == rCableCategory)
							{
								const CString  rCableVoltType = pCableItem->prop()->GetValue(_T("General"), _T("Volt Type")).c_str();
								if(rCableVoltType.Find(_T("HV")) != -1)
								{
									pRouteItem->AddHVPowerCable(pCableItem);
								}
								else if(rCableVoltType.Find(_T("LV")) != -1)
								{
									pRouteItem->AddLVPowerCable(pCableItem);
								}
							}
							else if(_T("Control Cable") == rCableCategory)
							{
								pRouteItem->AddControlCable(pCableItem);
							}
						}
					}
				}
			}
		}
		else
		{
			CELoadItem* pItem = docData.FindItemByName(CTransformerItem::TypeString() , rLoadID);
			if(NULL != pItem) continue;
			pItem = docData.FindItemByName(CCapacitorItem::TypeString() , rLoadID);
			if(NULL != pItem) continue;
			pItem = docData.FindItemByName(CUPSDCItem::TypeString() , rLoadID);
			if(NULL != pItem) continue;

			//! 부모가 없는 Cable
			const string rCreatedBy = pCableItem->prop()->GetValue(_T("General") , _T("Created By"));
			//! set delete flag to true.
			if(_T("Auto") == rCreatedBy) pCableItem->SetDeleteFlag(true);
		}

		if(pSplash) pSplash->m_ctrlProgress.SetPos(nIndex);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	return project job no

	@author HumKyung.BAEK

	@date 2010-02-02

	@param	

	@return		
*/
string CELoadDocData::GetJobNo(void)
{
	CELoadDocData& docData    = CELoadDocData::GetInstance();
	CProjectSettingTable* pSettingTable = (CProjectSettingTable*)docData.GetTableOf(CProjectSettingTable::GetTableName());
	if(pSettingTable)
	{
		const string *rDocumentNo = pSettingTable->GetFieldValue(_T("C_DOCUMENT_NO"));
		return (*rDocumentNo);
	}

	return string();
}
