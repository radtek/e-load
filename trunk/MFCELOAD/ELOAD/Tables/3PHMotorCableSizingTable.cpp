#include "StdAfx.h"
#include "../ELoad.h"
#include "../ELoadDocData.h"
#include <util/ado/SQLFile.h>
#include "3PHMotorCableSizingTable.h"

using namespace TABLES;

#define	T_3PH_MOTOR_CABLE_SIZING	_T("T_3PH_MOTOR_CABLE_SIZING")

C3PHMotorCableSizingTable::C3PHMotorCableSizingTable(void)
{
}

C3PHMotorCableSizingTable& C3PHMotorCableSizingTable::GetInstance(void)
{
	static C3PHMotorCableSizingTable __INSTANCE__;

	return __INSTANCE__;
}

C3PHMotorCableSizingTable::~C3PHMotorCableSizingTable(void)
{
	try
	{
		Clear();
	}
	catch(...)
	{
	}
}

/**
	@brief	

	@author	BHK
*/
int C3PHMotorCableSizingTable::Insert(C3PHMotorCableSizingTable::Record record)
{
	m_RecordEntry.push_back(record);
	
	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
int TABLES::C3PHMotorCableSizingTable::Clear(void)
{
	m_RecordEntry.clear();

	return ERROR_SUCCESS;
}

/**
	@brief

	@author	HumKyung

	@date	2011.02.18

	@param	lhs
	@param	rhs

	@return	bool
*/
static bool __SortByKW(const C3PHMotorCableSizingTable::Record& lhs , const C3PHMotorCableSizingTable::Record& rhs)
{
	return (atof(lhs.sKW.c_str()) < atof(rhs.sKW.c_str()));
}

/**
	@brief	

	@author	HumKyung

	@date	????.??.??

	@param

	@return	int
*/
int TABLES::C3PHMotorCableSizingTable::Load()
{
        static bool __ALREADY_LOADED__ = false;
	
	if(false == __ALREADY_LOADED__)
	{
		CELoadDocData& docData    = CELoadDocData::GetInstance();
		//! MDB CONNECT
		const STRING_T rLibraryMDBFilePath = docData.GetCommonLibraryMDBFilePath();

		CADODB adoDB;
		STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rLibraryMDBFilePath.c_str() + DB_PASSWORD;
		if(TRUE == adoDB.DBConnect(rConnectionString))
		{
			STRING_T rQuery(_T("SELECT * FROM ")T_3PH_MOTOR_CABLE_SIZING);
			try
			{ 
				adoDB.OpenQuery(rQuery);

				LONG lRecordCount = 0;
				adoDB.GetRecordCount(&lRecordCount);
				for(int i = 0;i < lRecordCount;++i)
				{
					C3PHMotorCableSizingTable::Record record;

					STRING_T rValue;
					adoDB.GetFieldValue(i , _T("C_KW") , &rValue);
					record.sKW = rValue;

					Insert(record);
				}
				stable_sort(m_RecordEntry.begin() , m_RecordEntry.end() , __SortByKW);	//! 2011.02.18

				__ALREADY_LOADED__ = true;
			}
			catch(_com_error &e)
			{
				CString rMsg;
				rMsg.Format("*** DOG CHECK [ADO DB] => <ExecuteQuery 오류> 설명[%s] !! ***\n", (LPCSTR)e.Description());
				ELOAD_LOG4CXX_WARN(mylogger , "" , 1);

				return ERROR_BAD_ENVIRONMENT;
			}
		}
		else
		{
			AfxMessageBox( _T("Fail to connect project database... !") );
			return ERROR_BAD_ENVIRONMENT;
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	CONDUIT 테이블을 저장한다.

	@author	BHK
*/
int TABLES::C3PHMotorCableSizingTable::Save()
{
	CELoadDocData& docData    = CELoadDocData::GetInstance();
	//! MDB CONNECT
	const STRING_T rLibraryMDBFilePath = docData.GetCommonLibraryMDBFilePath();

	CADODB adoDB;
	const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rLibraryMDBFilePath.c_str() + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rConnectionString))
	{
		if(FALSE == adoDB.OpenQuery("SELECT * FROM "T_3PH_MOTOR_CABLE_SIZING))
		{
			CString rExecPath = GetExecPath();
			if("\\" != rExecPath.Right(1)) rExecPath += "\\";
			CString rSqlFilePath = rExecPath + "Backup\\"T_3PH_MOTOR_CABLE_SIZING".SQL";

			CSQLFile sqlFile(rSqlFilePath.operator LPCSTR());
			sqlFile.Parse();
			const string rSql = sqlFile.GetCreateString();
			if(!rSql.empty()) adoDB.ExecuteQuery(rSql.c_str());
		}
		else	adoDB.ExecuteQuery("DELETE * FROM "T_3PH_MOTOR_CABLE_SIZING);

		for(vector<Record>::iterator itr = m_RecordEntry.begin();itr != m_RecordEntry.end();++itr)
		{
			string rSql = "INSERT INTO "T_3PH_MOTOR_CABLE_SIZING"(C_KW) VALUES('";
			rSql+= itr->sKW + "')";

			adoDB.ExecuteQuery(rSql.c_str());
		}

		return ERROR_SUCCESS;
	}
	else
	{
		AfxMessageBox("Fail to connect project database... !");
		return ERROR_BAD_ENVIRONMENT;
	}

	return ERROR_SUCCESS;
}