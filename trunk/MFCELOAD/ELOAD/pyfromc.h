#ifndef PYFROMC_H
#define PYFROMC_H

#ifndef PyObject_VAR_HEAD

// forward declaration, so that you don't need to include "python.h" in other sources
class PyObject;
class PyThreadState;

#endif

#include <vector>
using namespace std;

#include "LoadItem.h"
using namespace ITEMS;
class Python
{
public:
	Python();
	virtual ~Python();

	// must be called from mainthread !!!
	bool startup();
	void cleanup();

	string CallPyCalculatePG3(const double& nW0 , const double& nQLMAX , const double& nCOSQL , const double& nK , const double& nCOSQG);
	string CallPyCalculatePG2(const double& nPLmax , const double& nV , const double& nXd1 , const double& nXd2);
	string CallPyCalculatePG1(const double& WL , const double& nL , const double& nCOS);
	string CallPyAutoCalculateStartingAmp(const string& rLoadID);
	int CallPyCheckLoadPropForPowerCableCreation(const string& rLoadName);
	int CallPyFindPowerCableSize(const string& rID , const string& rPhase , const string& rL ,
		const string& R , const string& X , const double& nPF , const double& I , 
		const double& nMotorVD , const int& nMultiply , double& nCalculatedVD);
	int CallPyFindMinPowerCableSize(const string& rInstall , const string& rTotalDF , const double& nFLC , const string& rSF ,
		const string& rUGCableAmbTemp , const string& rAGCableAmbTemp , const int& nNoOfCable);
	
	int CallPyPostLoadSummary(const BOOL& bApplyConnectedTransformer);
	string CallPyCalculateFLC(const string& rLoadName);
	int CallPyExportCableSizingExcelFile(const CString& rFilePath,const CString& rHertz, const CString& rCableType);
	int CallPyCalculateCableLength(CString* pDataName);
	int CallPyPostImportLoadDataFromExcel(vector<CLoadItem*>& ImportedLoadList);
	int CallPyGetPFandEFFForLoadSummary(string name , vector<string>& results);
	int CallPyLoadSummary(string name , vector<double>& results);
	int CallPyWriteLoadSummaryToExcel(const CString& rFolderPath);
	int CallPyWriteDataFileToExcel(const CString& rSrcFilePath , const CString& rStartRow , const CString& rProjectNo, 
		const CString& rProjectName, const CString& rClient, const CString& rDate , const CString& rDataFilePath);
	int CallPyWriteRouoteCableListToExcel();
	int CallPyWriteControlCableResultToExcel(const CString& rFolderPath);
	int CallPyWriteLoadSummaryToDataFile();
	int CallPyGenerateBusDiagram();
	int CallPyWritePDBLoadSummaryToExcel(const CString& rFolderPath);
	int CallPyWriteCableSizingToDataFile(const CString& rDataName);
	int CallPyWriteCableBMSummaryToExcel();
protected:
	friend class PythonCall;

	PyThreadState* m_pyThreadState;
	PyObject *m_pyMain , *m_pyLoadSummary;
public:
	int ReloadPython(void);
	int CallPyWriteCableSizingToExcel(void);
	int CallPyWriteCableSizingTxtToExcel(const CString& rSheetName = "");
	int CallPyWriteCableSizingResultToExcel(const CString& rFolderPath);
	int CallPyWriteTransformerSizingResultToExcel(const CString& rFilePath);
	int CallPyWriteControlCableSettingToExcel(void);
	int CallPyWriteDrumScheduleListToExcel(const CString& rFolderPath);
	int CallPyWriteLoadListSldToExcel(const CString& rFolderPath);
	int CallPyWriteCableScheduleResultToExcel(void);
};

extern Python python;

#endif /* PYFROMC_H */
