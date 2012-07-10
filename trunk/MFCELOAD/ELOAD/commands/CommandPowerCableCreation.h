#pragma once

#include "..\Tables/CableCreationTable.h"
#include "../Tables/CableDataTable.h"
#include "..\CableLengthDataSheet.h"
#include "../CableSizingResultDoc.h"

typedef struct tagParamForPowerCableCreation
{
	double nPF;
	double I;
	double nMotorVD;
	double nMinSize;
	string rPhase;
	double nBusVolt , nLoadVolt;
	double nFLC;
	string rSpec;
	string rFrom , rTo , rPanelID , rLoadID;
	string rArea;
}ParamForPowerCableCreation;

namespace COMMAND
{
class CCommandPowerCableCreation
{
protected:
	enum
	{
		CHECKING_RUNNING  = 0x01,
		CHECKING_STARTING = 0x02
	};
public:
	enum
	{
		BEFORE = 0x01,
		AFTER  = 0x02
	};
	typedef enum
	{
		LOAD	= 0x01,
		EQ	= 0x02
	}POWER_CABLE_OWNER_TYPE;

	CCommandPowerCableCreation(const POWER_CABLE_OWNER_TYPE nOwnerType = LOAD , const string& rAreaNameForEquip = _T(""));
	~CCommandPowerCableCreation(void);
	int Execute(const bool& bExecuteSelectedLoad = false);
	static UINT StatusThreadEntry(LPVOID pVoid);
	
	static string GetRunVoltageDrop(CLoadItem* pLoadItem);
	static double GetPowerFactor(CLoadItem* pLoadItem);
protected:
	virtual int CreatePowerCableInBus(double nMinSize , CCableCreationTable::Record &SettingRecord , vector<CBusItem*>& BusItemEntry);
			
	string m_rConduitTableName;
	double m_nConductors_1 , m_nConductors_2 , m_nConductors_Over2;
	int    m_nTotalCount , m_nIndex;
	bool m_bExecuteSelectedLoad;
	int    m_nTryCount , m_nSuccessCount , m_nFailCount;	//! 실행 시도 횟수 , 성공 횟수 , 실패 횟수
	POWER_CABLE_OWNER_TYPE m_nOwnerType;			//! LOAD / EQ
	string m_rAreaNameForEquip;				//! area name for equipment power cable.
	int m_nPowerCableRadio;					//! ALL_POWER_CABLE / UNSIZED_POWER_CABLE
	
	int ExecuteForEachRecord(vector<CBusItem*>& BusItemEntry , vector<CCableCreationTable::Record>& RecordList, double nMinSize , const string& rPhase);
	int StatusThread(void);
	int GetInterestingLoadItemListAndOption();
public:
	static double m_nRunningCalculatedVD;
	static int    m_nCheckingArea;	//! 현재 확인하고 있는 부분(CHECKING_RUNNING 혹은 CHECKING_STARTING 둘중 하나가 됨)
	static CCableLengthDataSheet::ConduitSizeList m_ConduitSizeList;
	static double m_nConduitLength;

	static CCableItem* CreatePowerCableItem(const string& rFromID , const string& rToID , const string& rPanelID , const string& rLoadID ,
		CCableCreationTable::Record &SettingRecord);

	static int AdjustCableNumbering(CELoadItem* pItem , CCableCreationTable::Record &SettingRecord , CCableItem* pCableItem);

	static CCableItem* CreateSpaceHeaterPowerCableItem(CLoadItem* pLoadItem , CCableCreationTable::Record &SettingRecord , const double& nLength , const string& rBusID);
	static int CreateSpaceHeaterPowerCableIfNeed(CLoadItem* pLoadItem, CCableCreationTable::Record &SettingRecord , CCableItem* pCableItem , const string& rBusID);
	string GetResultString(void);
	list<CELoadItem*> m_InterestingItemList;	//! Power Cable을 생성하고자 하는 Load들.
private:
	int CreateCableForTransformer(vector<CCableCreationTable::Record>& RecordList , double nMinSize);
	int CreateCableForCapacitor(vector<CCableCreationTable::Record>& RecordList , double nMinSize);
	int CreateCableForUPSDC(vector<CCableCreationTable::Record>& RecordList , double nMinSize);
	string GetCableNameFor(const string& rEqID , const string& rBusID);
	int GetPowerCableCreationSettingRecordForEqPowerCable(const string& rBusVolt , CCableCreationTable::Record& record , string& rPhase);
};
};