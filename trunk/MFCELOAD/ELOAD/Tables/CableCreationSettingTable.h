#pragma once

#include "CableCreationTable.h"
#include <vector>
using namespace std;

namespace TABLES
{
	class CHVCableCreationSettingTable : public CCableCreationTable
	{
	private:
		CHVCableCreationSettingTable::CHVCableCreationSettingTable();
	public:
		static CHVCableCreationSettingTable& GetInstance();
		CHVCableCreationSettingTable::~CHVCableCreationSettingTable();

		int Load(const string& rMDBFilePath);
	private:
		bool m_bAlreadyLoaded;
	};

	class CLV3PHCableCreationSettingTable : public CCableCreationTable
	{
	private:
		CLV3PHCableCreationSettingTable::CLV3PHCableCreationSettingTable();
	public:
		static CLV3PHCableCreationSettingTable& GetInstance();
		CLV3PHCableCreationSettingTable::~CLV3PHCableCreationSettingTable();

		int Load(const string& rMDBFilePath);
	private:
		bool m_bAlreadyLoaded;
	};

	class CLV1PHCableCreationSettingTable : public CCableCreationTable
	{
	private:
		CLV1PHCableCreationSettingTable::CLV1PHCableCreationSettingTable();
	public:
		static CLV1PHCableCreationSettingTable& GetInstance();
		CLV1PHCableCreationSettingTable::~CLV1PHCableCreationSettingTable();

		int Load(const string& rMDBFilePath);
	private:
		bool m_bAlreadyLoaded;
	};

	class CDCCableCreationSettingTable : public CCableCreationTable
	{
	private:
		CDCCableCreationSettingTable::CDCCableCreationSettingTable();
	public:
		static CDCCableCreationSettingTable& GetInstance();
		CDCCableCreationSettingTable::~CDCCableCreationSettingTable();

		int Load(const string& rMDBFilePath);
	private:
		bool m_bAlreadyLoaded;
	};

	class CPowerCableCreationSettingTable : public CCableCreationTable
	{
	private:
		CPowerCableCreationSettingTable::CPowerCableCreationSettingTable();
	public:
		CPowerCableCreationSettingTable::~CPowerCableCreationSettingTable();

		int Load(const string& rMDBFilePath,const string& rTableName);

		////////////////////////////////////////////////////////////////////////////////////////////////////
		static const string GetTableName();
		static CELOADTable* CreateInstance()
		{
			return (new CPowerCableCreationSettingTable);
		}
	private:
		bool m_bAlreadyLoaded;
	};

	class CCableNamingRuleSettingTable : public CCableCreationTable
	{
	private:
		CCableNamingRuleSettingTable::CCableNamingRuleSettingTable();
	public:
		static CCableNamingRuleSettingTable& GetInstance();
		CCableNamingRuleSettingTable::~CCableNamingRuleSettingTable();

		int Load(const string& rMDBFilePath);
	private:
		bool m_bAlreadyLoaded;
	};
};