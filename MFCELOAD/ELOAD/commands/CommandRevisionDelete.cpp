#include "StdAfx.h"
#include "CommandRevisionDelete.h"
#include "../ELoadDocData.h"
using namespace COMMAND;

CCommandRevisionDelete::CCommandRevisionDelete(void): CEventData("" , CEventData::TYPE_NULL)
{
}

CCommandRevisionDelete::~CCommandRevisionDelete(void)
{
}

/**
	@brief	delete revision row from table.

	@author	KHS
*/
int CCommandRevisionDelete::Execute(const string& rRevisionNo, const string& rProductName)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string rMDBFilePath = docData.GetProjectMDBFilePath();

	CADODB adoDB;
	const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rConnectionString))
	{
		const CString rFormat(_T("DELETE * FROM T_PRODUCT_REVISION WHERE C_PRODUCT_NAME = '%s' AND C_REVISION_NO = %d"));
		CString rSQL;
		
		const int nRevisionNo = atoi(rRevisionNo.c_str());
		rSQL.Format(rFormat , rProductName.c_str() , nRevisionNo);
		if(!adoDB.ExecuteQuery(rSQL.operator LPCTSTR()))
		{
			AfxMessageBox(_T("Fail to delete data"));
			return ERROR_BAD_ENVIRONMENT;
		}
	}
	else
	{
		AfxMessageBox(_T("Fail to connect database"));
		return ERROR_BAD_ENVIRONMENT;
	}

	return ERROR_SUCCESS;
}
