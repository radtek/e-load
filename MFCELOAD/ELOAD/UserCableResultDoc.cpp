// UserCableResultDoc.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "UserCableResultDoc.h"
#include "ELoadDocData.h"
#include "WorkStatusDlg.h"

// CUserCableResultDoc

IMPLEMENT_DYNCREATE(CUserCableResultDoc, CDocument)

CUserCableResultDoc::CUserCableResultDoc()
{
}

BOOL CUserCableResultDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CUserCableResultDoc::~CUserCableResultDoc()
{
}


BEGIN_MESSAGE_MAP(CUserCableResultDoc, CDocument)
END_MESSAGE_MAP()


// CUserCableResultDoc diagnostics

#ifdef _DEBUG
void CUserCableResultDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CUserCableResultDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CUserCableResultDoc serialization

void CUserCableResultDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}
#endif


// CUserCableResultDoc commands

BOOL CUserCableResultDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	/*if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;*/

	WriteUserCableDataInText();

	return TRUE;
}

/**
	@brief	WRITE USER CABLE DATA IN TEXT FILE

	@author KHS	

	@date 2009-06-25 ¿ÀÈÄ 2:46:15	

	@param	

	@return		
**/
int CUserCableResultDoc::WriteUserCableDataInText(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	double progress = 0.;
	int nIndex = 0;

	const string rProjectPath = docData.GetProjectFolderPath();
	CString rFilePath = rProjectPath.c_str() + CString("Working\\USER_CABLE_RESULT.TXT");
	
	CWorkStatusDlg* pDlg = CWorkStatusDlg::GetInstance();

	CString rTitle = _T("Write User Cable...");

	ofstream ofile(rFilePath);
	if(ofile.is_open())
	{
		list<CELoadItem*> CableItemList;
		docData.GetELoadItemListOf(CableItemList , CCableItem::TypeString(), _T("User"));

		const int nTotalCount = count_if(CableItemList.begin() , CableItemList.end() , CELoadItem::IsNotDeleted);

		for(list<CELoadItem*>::iterator itr = CableItemList.begin(); itr != CableItemList.end();++itr)
		{
			if((*itr)->IsDeleted()) continue;

			const string  rCreatedBy = (*itr)->prop()->GetValue(_T("General") , _T("Created By"));
			if(_T("User") ==  rCreatedBy)
			{
				const string rArea = (*itr)->prop()->GetValue(_T("General") ,_T("Area"));
				const string rPanelId = (*itr)->prop()->GetValue(_T("General"), _T("Panel ID"));
				const string rLoadId = (*itr)->prop()->GetValue(_T("General") ,_T("Load Id"));
				string rCableCategory = (*itr)->prop()->GetValue(_T("General") ,_T("Cable Category"));
				const string rFrom = (*itr)->prop()->GetValue(_T("General") ,_T("From"));
				const string rTo = (*itr)->prop()->GetValue(_T("General") ,_T("To"));
				const string rCableId = (*itr)->prop()->GetValue(_T("General") ,_T("CABLE ID"));
				const string rDesc = (*itr)->prop()->GetValue(_T("General") ,_T("Description"));
				const string rVolt = (*itr)->prop()->GetValue(_T("General") ,_T("Cable Volt"));
				const string rType = (*itr)->prop()->GetValue(_T("General") ,_T("Cable Spec"));
				const string rSpec = (*itr)->prop()->GetValue(_T("General") ,_T("Cable Spec"));
				const string rCore = (*itr)->prop()->GetValue(_T("General") ,_T("Core"));
				const string rSize = (*itr)->prop()->GetValue(_T("General") ,_T("Size"));
				const string rLength = (*itr)->prop()->GetValue(_T("General") ,_T("Length"));
				const string rOD = (*itr)->prop()->GetValue(_T("Cable Spec") ,_T("Cable OD"));
				const string rWeight = (*itr)->prop()->GetValue(_T("Cable Spec") ,_T("Weight"));

				if(_T("Power Cable") == rCableCategory)
				{
					rCableCategory = _T("P");
				}
				else if(_T("Control Cable") == rCableCategory)
				{
					rCableCategory = _T("C");
				}

				ofile << rArea + "\t" << rPanelId + "\t" << rLoadId + "\t" << rFrom + "\t" << rTo + "\t" << rDesc + "\t" << rCableId + "\t" << rCableCategory + "\t" << rVolt + "\t" << rSpec + "\t" << rCore + "\t" << rSize + "\t" << rLength+ "\t" << rOD + "\t" << rWeight + "\t" << "\t" <<endl;
			}
			int progress = int((double(nIndex) / double(nTotalCount))*100.);
			if(pDlg) pDlg->UpdateWorkStatus(rTitle , progress);
			++nIndex;
		}
		ofile.close();
		return ERROR_SUCCESS;
	}
	return ERROR_BAD_ENVIRONMENT;
}
