#pragma once

#include "Observer.h"
#include "Tables/ProjectSettingTable.h"
#include "Tables/CableDataListTable.h"
#include "Tables/CableDataTable.h"
#include "Tables/ConduitListTable.h"
#include "Tables/ConduitTable.h"
#include "Tables/CableSizeSettingTable.h"
#include "Tables/FLCTable.h"
#include "LoadSummaryOptionData.h"
#include "CableLengthDataSheet.h"
#include "ItemSelectionSet.h"
#include "SimpleProgressBar.h"

/// ITEMS
#include "BusGroupItem.h"
#include "LoadItem.h"
#include "TransformerItem.h"
#include "CableItem.h"
#include "CapacitorItem.h"
#include "GeneratorItem.h"
#include "RouteItem.h"
#include "UPSDCItem.h"

#include <vector>
#include <list>
#include <map>

#if (_MSC_VER >= 1500)
#include <unordered_map>
#define	MAP_T	std::tr1::unordered_map
#else
#define	MAP_T	std::map
#endif

using namespace std;
using namespace TABLES;
using namespace ITEMS;

class CELoadDocData
{
	CELoadDocData(void);
public:
	class THREADPARAM
	{
        public:
                THREADPARAM() : EventData("" , CEventData::TYPE_NULL) {}

		CELoadDocData* _this;
		CObserver*     pFromObserver;
		CEventData     EventData;
	}; 

	static CELoadDocData& GetInstance();
	~CELoadDocData(void);

	CBusGroupItem* FindBusGroupItemByName( const string& rName );
	
	CLoadSummaryOptionData  m_LoadSummaryOptionData;	//! Load Summary를 돌리기 전에 채워넣어야 함.
	CCableDataTable		m_ProjectCableDataTable;
	CConduitTable		m_ConduitTable;
	CCableLengthDataSheet   m_CableLengthDataSheet;
	CFLCTable		m_FLCTable;
	int			m_nErrorCount;

	map<string,CELoadItemProp*> m_ItemPropMap;	
        vector<CELoadItem*> m_SelectedItemEntry;	//! Selected Load Item을 저장하는 벡터

	list<string> m_lstSystemBusGroup;		//! 2011.02.10
private:
	int CompareFieldNameAndProp(const STRING_T& rFieldName , vector<STRING_T>& TableFieldNameEntry , 
		map<STRING_T , STRING_T>& ShouldBeAddedFieldNameEntry , 
		CItemPropCategory::ItemPropTag* pItemTag);
	int SyncToItemProp(CADODB& adoDB , const string& rTableName , 
		const vector<STRING_T>& WillBeDeletedFieldNameEntry , const map<STRING_T, STRING_T>& ShouldBeAddedFieldNameEntry);
	int LoadELoadItemData(CADODB& adoDB , const string& rTableName , const string& rTypeString);
	int SaveELoadItemData(CADODB& adoDB , const string& rTableName , const string& rItemTypeString , 
		CSimpleProgressBar& progressbar , const long& nTotalItemCount , long& nCurItemIndex);
	static UINT StatusThreadEntry(LPVOID pVoid);
	UINT StatusThread(CELoadDocData::THREADPARAM* pThreadParam);
	int LoadPropFilesInDir(const CString& rModulePath);
	int ReadRegExpFile(const string& rRegExpFilePath);
	int ReadTableFile(map<string,string>& table , const string& rRegExpFilePath);
	int ReadErrorCodeFile(const string& rErrorCodeFilePath);
	
	bool m_bIsModified;	//! docdata의 data가 수정되었는가?
	CWinThread* m_pThread;
	vector<CObserver*> m_ObserverEntry;

	ProjectInfo m_ProjectInfo;
	string m_rUserID;
	string m_rServerFolderPath;
	map<string,string> m_RegExpMap,m_ColorMap,m_StringMap;
	map<int , string>  m_ErrorCodeMap;
	
	MAP_T<string , pair<string , list<CELoadItem*>*> > m_ELoadItemMap;
	map<string,CELOADTable*> m_ELoadTableMap;
public:
	BOOL m_bApplyTrToLoadSummary;

	bool LoadFromDataBase(void);
	CELOADTable* GetTableOf(const string& rTableName);
	int ClearTableOf(const string& rTableName);
	int GetELoadItemListOf(list<CELoadItem*>& ELoadItemList , const string& rTypeString, const string& rCreatedBy = _T("Auto"))
	{
		ELoadItemList.clear();
		MAP_T<string , pair<string,list<CELoadItem*>* > >::iterator where = m_ELoadItemMap.find(rTypeString);
		if(where != m_ELoadItemMap.end()) 
		{
			for(list<CELoadItem*>::iterator itr = where->second.second->begin(); itr != where->second.second->end();++itr)
			{
				if((*itr)->IsDeleted()) continue;
				
				if((rCreatedBy == _T("Auto")) && ((*itr)->IsKindOf(CBusItem::TypeString()) || (*itr)->IsKindOf(CLoadItem::TypeString()) || (*itr)->IsKindOf(CCableItem::TypeString())))
				{
					if(_T("User") == (*itr)->prop()->GetValue(_T("General"), _T("Created By")))
					{
						continue;
					}
				}
				else if((rCreatedBy == _T("User")) && ((*itr)->IsKindOf(CBusItem::TypeString()) || (*itr)->IsKindOf(CLoadItem::TypeString()) || (*itr)->IsKindOf(CCableItem::TypeString())))
				{
					if(_T("User") != (*itr)->prop()->GetValue(_T("General"), _T("Created By")))
					{
						continue;
					}
				}

				ELoadItemList.push_back(*itr);
			}
			return ERROR_SUCCESS;
		}

		return ERROR_BAD_ENVIRONMENT;
	}
	list<CELoadItem*>* GetELoadItemListPtrOf(const string& rTypeString)
	{
		MAP_T<string , pair<string,list<CELoadItem*>* > >::iterator where = m_ELoadItemMap.find(rTypeString);
		if(where != m_ELoadItemMap.end()) return (where->second.second);

		return NULL;
	}

	bool IsModified() const;
	int Save(const string& rTableName = _T("") , const string& rTypeString = _T("") , CWnd* pParentWnd = NULL);
	int SortBusGroupByOrderNo();
	int AttachObserver(CObserver* pObserver);
	int DetachObserver(CObserver* pObserver);
	int DispatchEvent(CObserver* pFrom , const CEventData& EventData);
        int DeleteLoadItemWithPt(CLoadItem* pLoadItem);
	CString GetAppSettingFilePath(void);
	int SetUserID(const string& rUserID);
	string GetUserID() const{ return m_rUserID; }
	int SetProjectNo( const string& rProjectNo )
	{
		m_ProjectInfo.m_rProjectNo = rProjectNo;
		return ERROR_SUCCESS;
	}
	string GetProjectNo() const
	{
		return m_ProjectInfo.m_rProjectNo;
	}
	int SetProjectName(const string& rProjectName )
	{
		m_ProjectInfo.m_rProjectName = rProjectName;
		return ERROR_SUCCESS;
	}
	string GetProjectName() const
	{
		return m_ProjectInfo.m_rProjectName;
	}
	int SetProjectDesc( const string& rProjectDesc )
	{
		m_ProjectInfo.m_rProjectDesc = rProjectDesc;
		return ERROR_SUCCESS;
	}
	string GetProjectDesc() const
	{
		return m_ProjectInfo.m_rProjectDesc;
	}
	int SetProjectClient(const string& rProjectClient )
	{
		m_ProjectInfo.m_rProjectClient = rProjectClient;
		return ERROR_SUCCESS;
	}
	string GetProjectClient() const
	{
		return m_ProjectInfo.m_rProjectClient;
	}
	string GetProjectFolderPath() const
	{
		string rProjectFolderPath = m_rServerFolderPath;
		if('\\' != rProjectFolderPath.at(rProjectFolderPath.length() - 1)) rProjectFolderPath += "\\";
		rProjectFolderPath += m_ProjectInfo.m_rProjectNo + "\\";

		return rProjectFolderPath;
	}
	string GetProjectMDBFilePath(void) const;
	STRING_T GetCommonLibraryMDBFilePath() const;

	int SetServerFolderPath( const string& rServerFolderPath )
	{ 
		m_rServerFolderPath = rServerFolderPath;
		return ERROR_SUCCESS;
	}
	string GetServerFolderPath() const{ return m_rServerFolderPath; }
	string GetServerMDBFilePath(void) const;
        CString GetModulePath(void);
	CLoadItem* FindLoadItemByName(const string& rName);
	CBusItem* FindBusItemByName(const string& rBusName);
	CTransformerItem* FindTransformerItemByName(const string& rTrName);
	CELoadItem* FindItemByName(const string& rTypeString , const string& rItemName);
	string GetRegExp(const string& rKey);
	COLORREF GetColorCode(const string& rColorName) const;
	string GetStringOf(const UINT& nStringCode) const;

	bool IsExistItemName(const string& rTypeString , const string& rName);
	int GetLoadItemSetInBus(CItemSelectionSet& SelectionSet , const string& rBusID);
	int GetAllBusItems(vector<CBusItem*>& BusItemEntry);
	int SetModifiedFlag(const bool& bModified);
	int CreateTableFromProp(CADODB& adoDB , const string& rTableName , const string& rTypeString);
	CELoadItem* CreateELoadItemOf(const string& rPropName , const bool& bSetModifiedFlag = false);
	CEventData* CreatePreExecutionCommandOf(const string& rTypeString , const string& rCategory , const string& rKey);
	CEventData* CreateAutoCalculateCommandOf(const string& rTypeString , const string& rCategory , const string& rKey);
	int ClearELoadItemListOf(const string& rTypeString);
	BOOL CompareCStringArray(const CStringArray& Array, const CString& rValue);

	int GetIconNo(CELoadItem* pItem);
	string GetErrorMessage(const int& nErrorCode);
	int GetAllSubBUSItems(vector<CBusItem*>& PdbItemEntry);
	int GetAllBusItemsExceptedSubBUS(vector<CBusItem*>& BusItemEntry);
	CTrayItem* FindTrayItemByName(const string& rTrayName);
	CRouteItem* CELoadDocData::FindRouteItemByName(const string& rRouteName);
private:
	int PostProcessAfterLoading(void);
public:
	string GetJobNo(void);
};
