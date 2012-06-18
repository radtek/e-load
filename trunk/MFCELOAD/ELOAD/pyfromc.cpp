#include "stdafx.h"

#ifdef _DEBUG
#undef _DEBUG
#include <python.h>
#define _DEBUG
#else
#include <python.h>
#endif

#include "pyfromc.h"
#include "ELoadDocPy.h"
#include "PythonCall.h"
#include <sstream>
#include "ELoadDocData.h"
#include "Tables/LoadSummaryRefTable.h"

Python::Python()
:   m_pyMain( NULL ) , m_pyLoadSummary(NULL) , m_pyThreadState( NULL )
{
}

Python::~Python()
{
	cleanup();
}

void Python::cleanup()
{
	if( m_pyMain != NULL )
	{
		Py_DECREF(m_pyMain);
		m_pyMain = NULL;
	}
	if( m_pyThreadState )
	{
		PyEval_AcquireThread(m_pyThreadState);
		m_pyThreadState = NULL;
	}
	Py_Finalize();
}

#define CHECK_PYOBJECT(pyObj) (PyErr_Occurred() || NULL == pyObj)

bool RedirctStdErrToFile(char* pFileName, char* mode) 
{
    PyObject* objFD = PyFile_FromString(pFileName, mode);
    if(CHECK_PYOBJECT(objFD)) {
        return false;
    }
    PyFile_SetBufSize(objFD, 0);  
    int ret = PySys_SetObject("stderr", objFD);
    if(ret != 0){
        Py_XDECREF(objFD);
        return false;
    }
    Py_XDECREF(objFD);
    return true;
}

/**
	@brief	

	@author	BHK
*/
bool Python::startup()
{
	static bool initialized = false;
	if( !initialized )
	{
		initialized = true;
		char buffer[256] = {'\0' ,}, format[256] = {'\0' ,};
		
		Py_Initialize();
		PyEval_InitThreads();
		
		CString rExecPath = GetExecPath();
		if("\\" == rExecPath.Right(1)) rExecPath = rExecPath.Left(rExecPath.GetLength() - 1);

		Py_InitModule4("ELoadApp",ELoadApp_methods,"ELoadApp",(PyObject *)NULL,PYTHON_API_VERSION); 

		PyRun_SimpleString("import sys"); 
		PyRun_SimpleString("sys.path.append(\"" + rExecPath + "\\python\")");
		
		{
			PyObject* pName = PyString_FromString("ELOAD");
			if (NULL == pName)
			{
				PyErr_Print();
			}

			m_pyMain = PyImport_Import(pName);
			if (NULL == m_pyMain)
			{
				RedirctStdErrToFile("d:\\FileNameWhatYouWant.txt", "a"); // 리다이렉션!!

				PyErr_Print();
				AfxMessageBox("import went bang...\n");
			}
			Py_DECREF(pName);
		}

		/*{
			PyObject* pName = PyString_FromString("LogMsgToELOAD");
			if (NULL == pName)
			{
				PyErr_Print();
			}

			m_pyLoadSummary = PyImport_Import(pName);
			if (NULL == m_pyLoadSummary)
			{
				AfxMessageBox("import went bang...\n");
			}
			Py_DECREF(pName);
		}
		*/

		m_pyThreadState = PyThreadState_Get();
		PyEval_ReleaseThread(m_pyThreadState);

		return (m_pyMain != NULL);
	}

	return true;
}

/**
	@brief	call PyLoadSummary python code.

	@author	HumKyung.BAEK
*/
int Python::CallPyLoadSummary(string name , vector<double>& results)
{
	PythonCall c( m_pyMain , "PyLoadSummary" , 1);
	
	CStringList args;
	args.AddTail(name.c_str());
	c.set(0 , args);

	CString res = c.invokeString();
	
	return (_T("SUCCESS") != res) ? ERROR_BAD_ENVIRONMENT : ERROR_SUCCESS;
}

/**
	@brief	call python code.

	@author	BHK
*/
int Python::CallPyWriteRouoteCableListToExcel()
{
	PythonCall c( m_pyMain , "PyWriteRouoteCableListToExcel" , 1);
	
	CString rSrcFilePath = GetExecPath();
	if("\\" != rSrcFilePath.Right(1)) rSrcFilePath += "\\";
	rSrcFilePath += "Backup\\TEMPLATE_ROUTE_CABLE_LIST.xls";

	CELoadDocData& docData = CELoadDocData::GetInstance();
	CString rDestFilePath = docData.GetProjectFolderPath().c_str();
	if("\\" != rDestFilePath.Right(1)) rDestFilePath += "\\";
	rDestFilePath +=  "Working\\ROUTE_CABLE_LIST_RESULT.xls";

	CStringList args;
	args.AddTail(rSrcFilePath);
	args.AddTail(rDestFilePath);


	c.set(0 , args);
	CString res = c.invokeString();
	
	return ("SUCCESS" == res) ? ERROR_SUCCESS : ERROR_BAD_ENVIRONMENT;
}


/**
	@brief	call python code.

	@author	BHK

	@date	?

	@param

	@return
*/
int Python::CallPyWriteControlCableResultToExcel(const CString& rFolderPath)
{
	PythonCall c( m_pyMain , "PyWriteControlCableResultToExcel" , 1);
	
	CString rSrcFilePath = GetExecPath();
	if(_T("\\") != rSrcFilePath.Right(1)) rSrcFilePath += _T("\\");
	rSrcFilePath += _T("Backup\\TEMPLATE_CONTROL_CABLE_RESULT.xls");

	CELoadDocData& docData = CELoadDocData::GetInstance();
	CString rDestFilePath = docData.GetProjectFolderPath().c_str();
	if(_T("\\") != rDestFilePath.Right(1)) rDestFilePath += _T("\\");
	rDestFilePath += _T("Working\\AUTO_CONTROL_CABLE_RESULT.xls");

	CStringList args;
	args.AddTail(rSrcFilePath);
	args.AddTail(rDestFilePath);
	args.AddTail(rFolderPath);

	c.set(0 , args);
	CString res = c.invokeString();
	
	return ("SUCCESS" == res) ? ERROR_SUCCESS : ERROR_BAD_ENVIRONMENT;
}


/**
	@brief	call python code.

	@author	HumKyung Baek

	@date	????.??.??

	@param	folder path contins bus files.

	@return	
*/
int Python::CallPyWritePDBLoadSummaryToExcel(const CString& rFolderPath)
{
	PythonCall c( m_pyMain , "PyWritePDBLoadSummaryToExcel" , 1);
	
	CString rSrcFilePath = GetExecPath();
	if(_T("\\") != rSrcFilePath.Right(1)) rSrcFilePath += _T("\\");
	rSrcFilePath += _T("Backup\\TEMPLATE_LOAD_SUMMARY_PDB.xls");

	CELoadDocData& docData = CELoadDocData::GetInstance();
	CString rPDBDestFilePath = docData.GetProjectFolderPath().c_str();
	if(_T("\\") != rPDBDestFilePath.Right(1)) rPDBDestFilePath += "\\";
	rPDBDestFilePath += _T("Working\\LOAD_SUMMARY_PDB.xls");

	CStringList args;
	args.AddTail(rSrcFilePath);
	args.AddTail(rPDBDestFilePath);
	args.AddTail(rFolderPath);

	c.set(0 , args);
	CString res = c.invokeString();
	
	return ("SUCCESS" == res) ? ERROR_SUCCESS : ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	call python code.

	@author	BHK
	
	@date	?

	@param	folder path contins bus files.

	@return	
*/
int Python::CallPyWriteLoadSummaryToExcel(const CString& rFolderPath)
{
	PythonCall c( m_pyMain , "PyWriteLoadSummaryToExcel" , 1);
	
	CString rSrcFilePath = GetExecPath();
	if("\\" != rSrcFilePath.Right(1)) rSrcFilePath += _T("\\");
	rSrcFilePath += _T("Backup\\TEMPLATE_LOAD_SUMMARY.xls");

	CELoadDocData& docData = CELoadDocData::GetInstance();
	CString rBusDestFilePath = docData.GetProjectFolderPath().c_str();
	if("\\" != rBusDestFilePath.Right(1)) rBusDestFilePath += _T("\\");
	rBusDestFilePath += _T("Working\\LOAD_SUMMARY_BUS.xls");

	CStringList args;
	args.AddTail(rSrcFilePath);
	args.AddTail(rBusDestFilePath);
	args.AddTail(rFolderPath);

	c.set(0 , args);
	CString res = c.invokeString();
	
	return (_T("SUCCESS") == res) ? ERROR_SUCCESS : ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	call python code.

	@author KHS	

	@date 2009-07-06 오후 4:01:34	

	@param	

	@return		
**/
int Python::CallPyWriteCableBMSummaryToExcel()
{
	PythonCall c( m_pyMain , "PyWriteCableBMSummaryToExcel" , 1);
	
	CString rSrcFilePath = GetExecPath();
	if("\\" != rSrcFilePath.Right(1)) rSrcFilePath += "\\";
	rSrcFilePath += "Backup\\TEMPLATE_CABLE BM SUMMARY.xls";

	CELoadDocData& docData = CELoadDocData::GetInstance();
	CString rBusDestFilePath = docData.GetProjectFolderPath().c_str();
	if("\\" != rBusDestFilePath.Right(1)) rBusDestFilePath += "\\";
	rBusDestFilePath +=  "Working\\CABLE_BM_SUMMARY.xls";

	CStringList args;
	args.AddTail(rSrcFilePath);
	args.AddTail(rBusDestFilePath);

	c.set(0 , args);
	CString res = c.invokeString();
	
	return ("SUCCESS" == res) ? ERROR_SUCCESS : ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	Cable Schedule

	@author KHS	

	@date 2009-11-27 오후 12:53:23	

	@param	

	@return		
**/
int Python::CallPyWriteCableScheduleResultToExcel()
{
	PythonCall c( m_pyMain , "PyWriteCableScheduleResultToExcel" , 1);
	
	CStringList args;
	
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CString rCableScheduleFilePath = docData.GetProjectFolderPath().c_str();
	if("\\" != rCableScheduleFilePath.Right(1)) rCableScheduleFilePath += "\\";
	rCableScheduleFilePath +=  "Cable Schedule Result\\CABLE_SCHEDULE_RESULT.xls";
	
	CString rTempFilePath = GetExecPath();
	if("\\" != rTempFilePath.Right(1)) rTempFilePath += "\\";
	rTempFilePath += "Backup\\TEMPLATE_CABLE_SCHEDULE.xls";

	args.AddTail(rTempFilePath);
	args.AddTail(rCableScheduleFilePath);

	c.set(0 , args);
	CString res = c.invokeString();
	
	return ("SUCCESS" == res) ? ERROR_SUCCESS : ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	call python code.

	@author KHS	

	@date 2009-07-06 오후 4:01:34	

	@param	

	@return		
**/
int Python::CallPyWriteDataFileToExcel(const CString& rSrcFilePath, const CString& rStartRow, const CString& rProjectNo, 
					const CString& rProjectName, const CString& rClient, const CString& rDate , const CString& rDataFilePath)
{
	PythonCall c( m_pyMain , "PyWriteDataFileToExcel" , 1);
	
	CStringList args;
	args.AddTail(rSrcFilePath);
	args.AddTail(rStartRow);
	args.AddTail(rProjectNo);
	args.AddTail(rProjectName);
	args.AddTail(rClient);
	args.AddTail(rDate);
	args.AddTail(rDataFilePath);

	/*CELoadDocData& docData = CELoadDocData::GetInstance();
	CString rCableScheduleFilePath = docData.GetProjectFolderPath().c_str();
	if("\\" != rCableScheduleFilePath.Right(1)) rCableScheduleFilePath += "\\";
	rCableScheduleFilePath +=  "Cable Schedule Result\\CABLE_SCHEDULE_RESULT.xls";
	
	CString rTempFilePath = GetExecPath();
	if("\\" != rTempFilePath.Right(1)) rTempFilePath += "\\";
	rTempFilePath += "Backup\\TEMPLATE_CABLE_SCHEDULE.xls";

	args.AddTail(rTempFilePath);
	args.AddTail(rCableScheduleFilePath);*/

	c.set(0 , args);
	CString res = c.invokeString();
	
	return ("SUCCESS" == res) ? ERROR_SUCCESS : ERROR_BAD_ENVIRONMENT;
}


/**
	@brief	

	@author	BHK

	@date	?

	@param

	@return return ERROR_SUCCESS if success, otherwise return ERROR_BAD_ENVIRONMENT
*/
int Python::CallPyWriteLoadSummaryToDataFile()
{
	PythonCall c( m_pyMain , "PyWriteLoadSummaryToDataFile" , 1);
	
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CStringList args;
	CString rString;
	rString.Format("%s" , docData.m_bApplyTrToLoadSummary ? _T("TRUE") : _T("FALSE"));
	args.AddTail(rString);
	c.set(0 , args);

	CString res = c.invokeString();
	
	return ("SUCCESS" == res) ? ERROR_SUCCESS : ERROR_BAD_ENVIRONMENT;

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	humkyung

	@date	????.??.??
*/
int Python::CallPyPostImportLoadDataFromExcel(vector<CLoadItem*>& ImportedLoadList)
{
	PythonCall c( m_pyMain , "PyPostImportLoadDataFromExcel" , 1);
	
	int res = ERROR_SUCCESS;
	for(vector<CLoadItem*>::iterator itr = ImportedLoadList.begin();itr != ImportedLoadList.end();++itr)
	{
		CStringList args;
		args.AddTail((*itr)->GetName().c_str());
		c.set(0 , args);

		CString __res__ = c.invokeString();
		res |= (_T("SUCCESS") == __res__) ? ERROR_SUCCESS : ERROR_BAD_ENVIRONMENT;
	}
	
	return res;
}

/**
	@brief	

	@author	BHK
*/
int Python::CallPyPostLoadSummary(const BOOL& bApplyConnectedTransformer)
{
	PythonCall c( m_pyMain , "PyPostLoadSummary" , 1);
	
	CStringList args;
	args.AddTail((TRUE == bApplyConnectedTransformer) ? _T("TRUE") : _T("FALSE"));
	c.set(0 , args);

	CString res = c.invokeString();
	
	return ("SUCCESS" == res) ? ERROR_SUCCESS : ERROR_BAD_ENVIRONMENT;

	return ERROR_SUCCESS;
}

/**
	@brief	Cable Sizing시트를 txt 파일로 추출한다.

	@rDataName	생성할 파일 이름.

	@author	KHS
*/
int Python::CallPyWriteCableSizingToDataFile(const CString& rDataName)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

        CCableSizeSettingTable& CableSizeSettingTable = CCableSizeSettingTable::GetInstance();

        PythonCall c( m_pyMain , "PyWriteCableSizingToDataFile" , 1);
	
	CStringList args;
	args.AddTail(rDataName);				//			0
	args.AddTail(CableSizeSettingTable.m_RecordMap[rDataName].m_rDeratingFactor.c_str());	//DERATING FACTOR	1

	//!CABLE SIZE SETTING DATA를 추가로 넘겨준다
	args.AddTail(CableSizeSettingTable.m_RecordMap[rDataName].m_rCableDataTable.c_str());		//! CABLE DATA TABLE	2
	args.AddTail(CableSizeSettingTable.m_RecordMap[rDataName].m_rSelectBus.c_str());		//! SELECT BUS		3
	args.AddTail(CableSizeSettingTable.m_RecordMap[rDataName].m_rMotorType.c_str());		//! MOTOR TYPE		4
	args.AddTail(CableSizeSettingTable.m_RecordMap[rDataName].m_rBusMotorVoltage[0].c_str());	//! BUS VOLTAGE		5
	args.AddTail(CableSizeSettingTable.m_RecordMap[rDataName].m_rBusMotorVoltage[1].c_str());	//! MOTOR VOLTAGE	6
	args.AddTail(CableSizeSettingTable.m_RecordMap[rDataName].m_rVoltageDrop[0].c_str());		//! VOLTAGE DROP %	7
	args.AddTail(CableSizeSettingTable.m_RecordMap[rDataName].m_rVoltageDrop[1].c_str());		//! VOLTAGE DROP V	8
	args.AddTail(CableSizeSettingTable.m_RecordMap[rDataName].m_rPFForFeeder.c_str());		//! FOWER FACTOR FOR FEEDER 9
	args.AddTail(CableSizeSettingTable.m_RecordMap[rDataName].m_rStartingVoltageDrop[0].c_str());	//! STARTING VOLTAGE DROP 10 %
	args.AddTail(CableSizeSettingTable.m_RecordMap[rDataName].m_rStartingVoltageDrop[1].c_str());	//! STARTING VOLTAGE DROP 11 V
	args.AddTail(CableSizeSettingTable.m_RecordMap[rDataName].m_rStartingPowerFactor.c_str());	//! STARTING POWER FACTOR 12
	args.AddTail(CableSizeSettingTable.m_RecordMap[rDataName].m_rInsulationMethod.c_str());		//! INSULATION METHOD 13
	args.AddTail(CableSizeSettingTable.m_RecordMap[rDataName].m_rCableType.c_str());		//! CABLE TYPE	14
	args.AddTail(CableSizeSettingTable.m_RecordMap[rDataName].m_rConduitTable.c_str());		//! CONDUIT TABLE	15
	args.AddTail(CableSizeSettingTable.m_RecordMap[rDataName].m_rConduitDesc.c_str());		//! CONDUIT DESC 16
	args.AddTail(CableSizeSettingTable.m_RecordMap[rDataName].m_rMotorType.c_str());		//! MOTOR TYPE	 17

	c.set(0 , args);

	CString res = c.invokeString();
	
	return (_T("SUCCESS") == res) ? ERROR_SUCCESS : ERROR_BAD_ENVIRONMENT;

	return ERROR_SUCCESS;
}
/**
	@brief	generate bus diagram.

	@author	BHK
*/
int Python::CallPyGenerateBusDiagram()
{
	PythonCall c( m_pyMain , "PyGenerateBusDiagram" , 1);
	
	CStringList args;
	args.AddTail(" ");
	c.set(0 , args);

	CString res = c.invokeString();
	
	return ("SUCCESS" == res) ? ERROR_SUCCESS : ERROR_BAD_ENVIRONMENT;

	return ERROR_SUCCESS;
}

/**
	@brief	Call python function for calculating cable length.

	@author	HumKyung

	@date	????.??.??

	@param	pDataName

	@return	int
*/
int Python::CallPyCalculateCableLength(CString* pDataName)
{
	assert(pDataName && "pDataName is NULL");

	if(pDataName)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		TABLES::CELOADTable* pTable = docData.GetTableOf(CProjectSettingTable::GetTableName());
		const CString rDataName = pTable->m_RecordEntry[0].FieldValueMap[_T("C_CUR_CABLE_SIZING_SETTING_TABLE_NAME")].c_str();

		CCableSizeSettingTable& CableSizeSettingTable = CCableSizeSettingTable::GetInstance();

		{
			CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
			string* pValue = pSettingTable->GetFieldValue(_T("C_HERTZ"));
			const string rHertz = ((NULL != pValue) && !pValue->empty()) ? *pValue : _T("50");

			//! Rating Capacity , PF값등을 준비한다.
			docData.m_CableLengthDataSheet.PrepareCableLengthCalculation();

			CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(
				CableSizeSettingTable.GetCableType(rDataName).operator LPCSTR());
			if(NULL == pCableDataByType)
			{
				AfxMessageBox(_T("Can't find cabletype. please check before running."));
				return ERROR_INVALID_PARAMETER;
			}

			vector<CCableDataByType::Record*>* pRecord = pCableDataByType->GetRecordEntryOfHertz();
			if(pRecord)
			{
				int nCol = 0;
				for(vector<CCableDataByType::Record*>::iterator itr = pRecord->begin();itr != pRecord->end();++itr,++nCol)
				{
					int nRow = 0;
					for(vector<double>::iterator jtr = docData.m_CableLengthDataSheet.m_PFEntry.begin();jtr != docData.m_CableLengthDataSheet.m_PFEntry.end();++jtr,++nRow)
					{
						PythonCall c( m_pyMain , "PyCalculateCableLength" , 1);

						CStringList args;

						CString rString;
						//! Rating Capacity
						rString.Format("%lf" , docData.m_CableLengthDataSheet.m_RatingCapacityEntry[nRow]);
						args.AddTail(rString);

						//! PF
						rString.Format("%lf" , *jtr);
						args.AddTail(rString);

						//! MOTOR VOLTAGE
						args.AddTail(CableSizeSettingTable.GetBusMotorVoltage2(rDataName));

						//! VOLTAG DROP
						args.AddTail(CableSizeSettingTable.GetVoltageDrop2(rDataName));

						//! R
						args.AddTail((*itr)->m_FieldMap[_T("C_R")].c_str());

						CString X = (*itr)->m_FieldMap[_T("C_X")].c_str();
						args.AddTail(X);

						rString = CableSizeSettingTable.GetStartingVoltageDrop2(rDataName);
						args.AddTail(rString);

						rString = CableSizeSettingTable.GetStartingPowerFactor(rDataName);
						args.AddTail(rString);

						rString = CableSizeSettingTable.GetMotorType(rDataName);
						args.AddTail(rString);

						c.set(0 , args);
						CStringList list;
						c.invoke(list);

						if(2 == list.GetSize())
						{
							POSITION pos = list.FindIndex(0);
							const CString rLength = list.GetAt(pos);
							pos = list.FindIndex(1);
							const CString rFLC = list.GetAt(pos);

							docData.m_CableLengthDataSheet.m_FLCEntry[nRow] = atof(rFLC);
							docData.m_CableLengthDataSheet.SetValue(nRow , nCol , rLength.operator LPCSTR());
						}
					}
				}
				CallPyWriteCableSizingToDataFile(*pDataName);
			}
			else
			{
				AfxMessageBox(_T("Can't find record matching hertz."));
				return ERROR_INVALID_PARAMETER;
			}
		}
		
		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
        @brief  

        @author BHK
*/
int Python::CallPyExportCableSizingExcelFile(const CString& rFilePath, const CString& rHertz, const CString& rCableType)
{
        PythonCall c( m_pyMain , "PyExportCableSizingExcelFile" , 1);

        //! TEMPLATE EXCEL FILE NAME를 넘겨준다.
	CStringList args;
	args.AddTail(rFilePath);
	args.AddTail(rHertz);
	args.AddTail(rCableType);
	c.set(0 , args);

        CString res = c.invokeString();

        return ("SUCCESS" == res) ? ERROR_SUCCESS : ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author	BHK
*/
string Python::CallPyCalculateFLC(const string& rLoadName)
{
	PythonCall c( m_pyMain , "PyCalculateFLC" , 1);

	//! Load Name을 넘겨준다.
	CStringList args;
	args.AddTail(rLoadName.c_str());
	c.set(0 , args);

	return c.invokeString().operator LPCSTR();
}

Python python;

/**
	@brief	PYTHON CODE를 다시 로딩한다.

	@author	BHK
*/
int Python::ReloadPython(void)
{
	char buffer[256] = {'\0' ,}, format[256] = {'\0' ,};

	CString rExecPath = GetExecPath();
	if("\\" == rExecPath.Right(1)) rExecPath = rExecPath.Left(rExecPath.GetLength() - 1);

	///PyObject* pName = PyString_FromString("ELoadExtension");
	///if (NULL == pName)
	//{
	//	PyErr_Print();
	///}
	/*
	PyObject* sysModules = PySys_GetObject("modules");
	if(NULL != sysModules) 
	{
		int ret = PyDict_DelItem(sysModules, pName);
		// ..에러처리 코드 생략..
	}
	*/
	{
		PyGILState_STATE GIL = PyGILState_Ensure();
		m_pyMain = PyImport_ReloadModule(m_pyMain);
		PyGILState_Release(GIL);
	}
	
	/*
	{
		PyGILState_STATE GIL = PyGILState_Ensure();
		m_pyLoadSummary = PyImport_ReloadModule(m_pyLoadSummary);
		PyGILState_Release(GIL);
	}
	*/

	/*Py_DECREF(m_pyMain);
	m_pyMain = PyImport_Import(pName);
	if (NULL == m_pyMain)
	{
		AfxMessageBox("import went bang...\n");
	}
	Py_DECREF(pName);
	*/

	return ERROR_SUCCESS;
}

/**
	@brief	Control Cable Setting Excel Export

	@author KHS	

	@date 2009-06-17 오후 5:07:43	

	@param	

	@return		
**/
int Python::CallPyWriteControlCableSettingToExcel(void)
{
	PythonCall c( m_pyMain , "PyWriteControlCableSettingToExcel" , 1);
	
	CString rSrcFilePath = GetExecPath();
	if("\\" != rSrcFilePath.Right(1)) rSrcFilePath += "\\";
	rSrcFilePath += "Backup\\TEMPLATE_CONTROL_CABLE_SETTING.xls";

	CELoadDocData& docData = CELoadDocData::GetInstance();
	CString rDestFilePath = docData.GetProjectFolderPath().c_str();
	if("\\" != rDestFilePath.Right(1)) rDestFilePath += "\\";
	rDestFilePath +=  "Working\\CONTROL_CABLE_SETTING.xls";

	CStringList args;
	args.AddTail(rSrcFilePath);
	args.AddTail(rDestFilePath);


	c.set(0 , args);
	CString res = c.invokeString();
	
	return ("SUCCESS" == res) ? ERROR_SUCCESS : ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	Power Cable (Cable Sizing Result ) Excel Export

	@author KHS	

	@date 2009-06-17 오후 3:40:25	

	@param	

	@return		
**/
int Python::CallPyWriteCableSizingResultToExcel(const CString& rFolderPath)
{
	PythonCall c( m_pyMain , "PyWriteCableSizingResultToExcel" , 1);
	
	CString rSrcFilePath = GetExecPath();
	if("\\" != rSrcFilePath.Right(1)) rSrcFilePath += "\\";
	rSrcFilePath += _T("Backup\\TEMPLATE_CABLE_SIZING_RESULT.xls");

	CELoadDocData& docData = CELoadDocData::GetInstance();
	CString rDestFilePath = docData.GetProjectFolderPath().c_str();
	if(_T("\\") != rDestFilePath.Right(1)) rDestFilePath += _T("\\");
	rDestFilePath += _T("Working\\CABLE_SIZING_RESULT.xls");

	CStringList args;
	args.AddTail(rSrcFilePath);
	args.AddTail(rDestFilePath);
	args.AddTail(rFolderPath);

	c.set(0 , args);
	CString res = c.invokeString();
	
	return ("SUCCESS" == res) ? ERROR_SUCCESS : ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	Transformer Sizing Result Excel Export

	@author HumKyung.Baek

	@date 2010.08.04

	@param	

	@return		
**/
int Python::CallPyWriteTransformerSizingResultToExcel(const CString& rFilePath)
{
	PythonCall c( m_pyMain , "PyWriteTransformerSizingResultToExcel" , 1);
	
	CString rSrcFilePath = GetExecPath();
	if("\\" != rSrcFilePath.Right(1)) rSrcFilePath += "\\";
	rSrcFilePath += _T("Backup\\TEMPLATE_TR_SUMMARY.xls");

	CELoadDocData& docData = CELoadDocData::GetInstance();
	CString rDestFilePath = docData.GetProjectFolderPath().c_str();
	if(_T("\\") != rDestFilePath.Right(1)) rDestFilePath += _T("\\");
	rDestFilePath += _T("Working\\TEMPLATE_TR_SUMMARY.xls");

	CStringList args;
	args.AddTail(rSrcFilePath);
	args.AddTail(rDestFilePath);
	args.AddTail(rFilePath);

	c.set(0 , args);
	CString res = c.invokeString();
	
	return ("SUCCESS" == res) ? ERROR_SUCCESS : ERROR_BAD_ENVIRONMENT;
}

/**
	@brief  CABLE SIZING EXCEL EXPORT

	@author KHS
*/
int Python::CallPyWriteCableSizingToExcel(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	TABLES::CELOADTable* pTable = docData.GetTableOf(CProjectSettingTable::GetTableName());
	const CString rDataName = pTable->m_RecordEntry[0].FieldValueMap[_T("C_CUR_CABLE_SIZING_SETTING_TABLE_NAME")].c_str();

	CCableSizeSettingTable& CableSizeSettingTable = CCableSizeSettingTable::GetInstance();

	PythonCall c( m_pyMain , "PyWriteCableLength" , 1);

	CString rXlsPath = GetExecPath();
	if( "\\" != rXlsPath.Right(1)) rXlsPath += "\\";
	rXlsPath = rXlsPath + _T("BackUp\\TEMPLATE_CABLESIZING.xls");
	//! TEMPLATE EXCEL FILE NAME과 DERATING FACTOR를 넘겨준다.
	CStringList args;
	args.AddTail(rXlsPath);
	args.AddTail(CableSizeSettingTable.m_RecordMap[rDataName].m_rDeratingFactor.c_str());
	c.set(0 , args);

	CString res = c.invokeString();

	return 0;
}
/**
        @brief  TXT 파일을 엑셀로 출력
        
	@author KHS

	@date	????.??.??

	@param	rSheetName

	@return	int
*/
int Python::CallPyWriteCableSizingTxtToExcel(const CString& rSheetName)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	TABLES::CELOADTable* pTable = docData.GetTableOf(CProjectSettingTable::GetTableName());
	const CString rDataName = pTable->m_RecordEntry[0].FieldValueMap[_T("C_CUR_CABLE_SIZING_SETTING_TABLE_NAME")].c_str();

	CCableSizeSettingTable& CableSizeSettingTable = CCableSizeSettingTable::GetInstance();

	PythonCall c( m_pyMain , "PyWriteCableLengthReadTxtToExcel" , 1);

	CString rXlsPath = GetExecPath();
	if( "\\" != rXlsPath.Right(1)) rXlsPath += "\\";
	rXlsPath = rXlsPath + _T("BackUp\\TEMPLATE_CABLESIZING.xls");
	//! TEMPLATE EXCEL FILE NAME과 DERATING FACTOR를 넘겨준다.
	CStringList args;
	args.AddTail(rXlsPath);
	args.AddTail(CableSizeSettingTable.m_RecordMap[rDataName].m_rDeratingFactor.c_str());
	args.AddTail(rSheetName);
	c.set(0 , args);

	CString res = c.invokeString();

	return 0;
}

/**
	@brief	PyCheckLoadPropForPowerCableCreation 파이썬 코드 호출

	@author BHK	

	@date 2009-04-03 오후 10:31:04	

	@param	

	@return		
**/
int Python::CallPyCheckLoadPropForPowerCableCreation(const string& rLoadName)
{
	PythonCall c( m_pyMain , "PyCheckLoadPropForPowerCableCreation" , 1);

	CStringList args;
	args.AddTail(rLoadName.c_str());
	c.set(0 , args);

	return (_T("SUCCESS") == c.invokeString()) ? ERROR_SUCCESS : ERROR_BAD_ENVIRONMENT;
}

/**
	@brief  PyFindMinPowerCableSize 파이썬 코드 호출

	@author BHK
*/
int Python::CallPyFindMinPowerCableSize(const string& rInstall , const string& rTotalDF , const double& nFLC , const string& rSF ,
					const string& rUGCableAmbTemp , const string& rAGCableAmbTemp , const int& nNoOfCable)
{
	PythonCall c( m_pyMain , "PyFindMinPowerCableSize" , 1);

	CStringList args;
	args.AddTail(rInstall.c_str());
	args.AddTail(rTotalDF.c_str());
	
	CString rString;
	rString.Format("%lf" , nFLC);
	args.AddTail(rString);

	args.AddTail(rSF.c_str());
	args.AddTail(rUGCableAmbTemp.c_str());
	args.AddTail(rAGCableAmbTemp.c_str());
	
	rString.Format("%d" , nNoOfCable);
	args.AddTail(rString);
	c.set(0 , args);

	return (_T("SUCCESS") == c.invokeString()) ? ERROR_SUCCESS : ERROR_BAD_ENVIRONMENT;
}

/**
        @brief  PyFindPowerCableSize 파이썬 코드 호출

        @author BHK
*/
int Python::CallPyFindPowerCableSize(const string& rID , const string& rPhase , const string& rL , 
				     const string& R , const string& X , const double& nPF , const double& I , 
				     const double& nMotorVD , const int& nMultiply , double& nCalculatedVD)
{
	{
		PythonCall c( m_pyMain , "PyFindPowerCableSize" , 1);
		
		CStringList args;
		args.AddTail(rID.c_str());
		args.AddTail(rPhase.c_str());
		args.AddTail(rL.c_str());
		args.AddTail(R.c_str());
		args.AddTail(X.c_str());
		CString rString;
		rString.Format("%lf" , nPF);
		args.AddTail(rString);
		rString.Format("%lf" , I);
		args.AddTail(rString);
		rString.Format("%lf" , nMotorVD);
		args.AddTail(rString);
		rString.Format("%d" , nMultiply);
		args.AddTail(rString);
		c.set(0 , args);

		CStringList list;
		c.invoke(list);
		if(2 == list.GetSize())
		{
			POSITION pos = list.FindIndex(0);
			const CString rResult = list.GetAt(pos);
			pos = list.FindIndex(1);
			const CString rValue = list.GetAt(pos);
			nCalculatedVD = atof(rValue.operator LPCSTR());

			return (_T("SUCCESS") == rResult) ? ERROR_SUCCESS : ERROR_BAD_ENVIRONMENT;
		}
	}
	
	return ERROR_INVALID_PARAMETER;
}

/**
	@brief  PyAutoCalculateStartingAmp 파이썬 코드 호출

	@author BHK
*/
string Python::CallPyAutoCalculateStartingAmp(const string& rLoadID)
{
	PythonCall c( m_pyMain , "PyAutoCalculateStartingAmp" , 1);

	CStringList args;
	args.AddTail(rLoadID.c_str());
	c.set(0 , args);

	return c.invokeString();
}

/**
	@brief	

	@author BHK	

	@date 2009-05-08 오후 2:18:53	

	@param	

	@return		
*/
string Python::CallPyCalculatePG1(const double& nWL , const double& nL , const double& nCOS)
{
	PythonCall c( m_pyMain , "PyCalculatePG1" , 1);

	CStringList args;
	CString rString;
	rString.Format("%lf" , nWL);
	args.AddTail(rString);
	rString.Format("%lf" , nL);
	args.AddTail(rString);
	rString.Format("%lf" , nCOS);
	args.AddTail(rString);
	c.set(0 , args);

	return c.invokeString();
}

/**
	@brief	

	@author BHK	

	@date 2009-05-08 오후 8:11:26	

	@param	

	@return		
*/
string Python::CallPyCalculatePG2(const double& nPLmax , const double& nV , const double& nXd1 , const double& nXd2)
{
	PythonCall c( m_pyMain , "PyCalculatePG2" , 1);

	CStringList args;
	CString rString;
	rString.Format("%lf" , nPLmax);
	args.AddTail(rString);
	rString.Format("%lf" , nV);
	args.AddTail(rString);
	rString.Format("%lf" , nXd1);
	args.AddTail(rString);
	rString.Format("%lf" , nXd2);
	args.AddTail(rString);
	c.set(0 , args);

	return c.invokeString();
}

/**
	@brief	

	@author BHK	

	@date 2009-05-08 오후 8:35:16	

	@param	

	@return		
*/
string Python::CallPyCalculatePG3(const double& nW0 , const double& nQLMAX , const double& nCOSQL , const double& nK , const double& nCOSQG)
{
	PythonCall c( m_pyMain , "PyCalculatePG3" , 1);

	CStringList args;
	CString rString;
	rString.Format("%lf" , nW0);
	args.AddTail(rString);
	rString.Format("%lf" , nQLMAX);
	args.AddTail(rString);
	rString.Format("%lf" , nCOSQL);
	args.AddTail(rString);
	rString.Format("%lf" , nK);
	args.AddTail(rString);
	rString.Format("%lf" , nCOSQG);
	args.AddTail(rString);
	c.set(0 , args);

	return c.invokeString();
}


/**
	@brief	Drum Schedule

	@author KHS	

	@date 2009-09-29 오후 3:18:44	

	@param	

	@return		
**/
int Python::CallPyWriteDrumScheduleListToExcel(const CString& rFolderPath)
{
	PythonCall c( m_pyMain , "PyWriteCableListDrumScheduleToExcel" , 1);
	
	CString rSrcFilePath = GetExecPath();
	if(_T("\\") != rSrcFilePath.Right(1)) rSrcFilePath += _T("\\");
	rSrcFilePath += _T("Backup\\TEMPLATE_DRUM_SCHEDULE.xls");

	CELoadDocData& docData = CELoadDocData::GetInstance();
	CString rDestFilePath = docData.GetProjectFolderPath().c_str();
	if(_T("\\") != rDestFilePath.Right(1)) rDestFilePath += _T("\\");
	rDestFilePath += _T("Drum Schedule Result\\DRUM_SCHEDULE_RESULT.xls");

	CStringList args;
	args.AddTail(rSrcFilePath);
	args.AddTail(rDestFilePath);
	args.AddTail(rFolderPath);

	c.set(0 , args);
	CString res = c.invokeString();
	
	return ("SUCCESS" == res) ? ERROR_SUCCESS : ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author KHS	

	@date 2009-11-09 오후 5:03:16	

	@param	

	@return		
**/
int Python::CallPyWriteLoadListSldToExcel(const CString& rFolderPath)
{
	PythonCall c( m_pyMain , "PyWriteLoadListSldToExcel" , 1);
	
	CString rSrcFilePath = GetExecPath();
	if(_T("\\") != rSrcFilePath.Right(1)) rSrcFilePath += _T("\\");
	rSrcFilePath += _T("Backup\\TEMPLATE_LOAD_LIST_SLD.xls");

	CELoadDocData& docData = CELoadDocData::GetInstance();
	CString rDestFilePath = docData.GetProjectFolderPath().c_str();
	if(_T("\\") != rDestFilePath.Right(1)) rDestFilePath += _T("\\");
	rDestFilePath += _T("Load List Sld Result\\LOAD_LIST_SLD_RESULT.xls.xls");

	CStringList args;
	args.AddTail(rSrcFilePath);
	args.AddTail(rDestFilePath);
	args.AddTail(rFolderPath);

	c.set(0 , args);
	CString res = c.invokeString();
	
	return ("SUCCESS" == res) ? ERROR_SUCCESS : ERROR_BAD_ENVIRONMENT;
}