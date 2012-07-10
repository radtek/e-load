#pragma once

namespace TABLES
{
	class CCableSizeSettingTable
	{
		CCableSizeSettingTable(void);
	public:
		typedef struct tagRecord
		{
			STRING_T m_rBusMotorVoltage[2];
			STRING_T m_rDeratingFactor;
			STRING_T m_rVoltageDrop[2];
			STRING_T m_rStartingVoltageDrop[2];
			STRING_T m_rStartingPowerFactor;
			STRING_T m_rInsulationMethod;
			STRING_T m_rCableType;
			STRING_T m_rCableDataTable , m_rConduitTable, m_rConduitDesc;
			STRING_T m_rSelectBus;

			STRING_T m_rMotorType , m_rPFForFeeder;
		}Record;

		static CCableSizeSettingTable& GetInstance();

		~CCableSizeSettingTable(void);
	public:
		CString GetCableDataTableName(const CString& rName) const { 
			map<CString , Record>::const_iterator where = m_RecordMap.find(rName);
			if(where != m_RecordMap.end()) return (where->second.m_rCableDataTable.c_str());
			return _T("");
		}
		CString SetCableDataTableName(const CString& rName , const CString& rCableDataTableName) { 
			map<CString , Record>::iterator where = m_RecordMap.find(rName);
			if(where != m_RecordMap.end()) where->second.m_rCableDataTable = rCableDataTableName;
			
			return ERROR_SUCCESS;
		}

		CString GetConduitTableName(const CString& rName) const { 
			map<CString , Record>::const_iterator where = m_RecordMap.find(rName);
			if(where != m_RecordMap.end()) return (where->second.m_rConduitTable.c_str());
			return _T("");
		}

		CString GetStartingPowerFactor(const CString& rName) const { 
			map<CString , Record>::const_iterator where = m_RecordMap.find(rName);
			if(where != m_RecordMap.end()) return (where->second.m_rStartingPowerFactor.c_str());
			return _T("");
		}
		CString GetStartingVoltageDrop2(const CString& rName) const {
			map<CString , Record>::const_iterator where = m_RecordMap.find(rName);
			if(where != m_RecordMap.end()) return (where->second.m_rStartingVoltageDrop[1].c_str());
			return _T("");
		}
		CString GetBusMotorVoltage1(const CString& rName) const {
			map<CString , Record>::const_iterator where = m_RecordMap.find(rName);
			if(where != m_RecordMap.end()) return (where->second.m_rBusMotorVoltage[0].c_str());
			return _T("");
		}
		CString GetBusMotorVoltage2(const CString& rName) const {
			map<CString , Record>::const_iterator where = m_RecordMap.find(rName);
			if(where != m_RecordMap.end()) return (where->second.m_rBusMotorVoltage[1].c_str());
			return _T("");
		}
		CString GetVoltageDrop2(const CString& rName) const {
			map<CString , Record>::const_iterator where = m_RecordMap.find(rName);
			if(where != m_RecordMap.end()) return (where->second.m_rVoltageDrop[1].c_str());
			return _T("");
		}
		CString GetCableType(const CString& rName) const{
			map<CString , Record>::const_iterator where = m_RecordMap.find(rName);
			if(where != m_RecordMap.end()) return (where->second.m_rCableType.c_str());
			return _T("");
		}
		CString GetMotorType(const CString& rName) const{
			map<CString , Record>::const_iterator where = m_RecordMap.find(rName);
			if(where != m_RecordMap.end()) return (where->second.m_rMotorType.c_str());
			return _T("");
		}
		int SetMotorType( const CString& rName , const CString& rMotorType )
		{
			map<CString , Record>::iterator where = m_RecordMap.find(rName);
			if(where != m_RecordMap.end()) where->second.m_rMotorType = rMotorType;

			return ERROR_SUCCESS;
		}
		
		CString GetPFForFeederValue(const CString& rName) const{
			map<CString , Record>::const_iterator where = m_RecordMap.find(rName);
			if(where != m_RecordMap.end()) return (where->second.m_rPFForFeeder.c_str());
			return _T("");
		}
		int SetPFForFeederValue( const CString& rName , const CString& rPFForFeederValue )
		{
			map<CString , Record>::iterator where = m_RecordMap.find(rName);
			if(where != m_RecordMap.end()) where->second.m_rPFForFeeder = rPFForFeederValue;

			return ERROR_SUCCESS;
		}
		int Save();
		int Load(void);
	public:
		map<CString , Record> m_RecordMap;
	};
};