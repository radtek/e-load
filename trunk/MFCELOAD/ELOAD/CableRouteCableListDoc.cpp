// CableRouteCableListDoc.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "CableRouteCableListDoc.h"
#include "WorkStatusDlg.h"
#include "ELoadDocData.h"
// CCableRouteCableListDoc

IMPLEMENT_DYNCREATE(CCableRouteCableListDoc, CDocument)

CCableRouteCableListDoc::CCableRouteCableListDoc()
{
}

BOOL CCableRouteCableListDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CCableRouteCableListDoc::~CCableRouteCableListDoc()
{
}


BEGIN_MESSAGE_MAP(CCableRouteCableListDoc, CDocument)
END_MESSAGE_MAP()


// CCableRouteCableListDoc diagnostics

#ifdef _DEBUG
void CCableRouteCableListDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CCableRouteCableListDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CCableRouteCableListDoc serialization

void CCableRouteCableListDoc::Serialize(CArchive& ar)
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


// CCableRouteCableListDoc commands

BOOL CCableRouteCableListDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	/*if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;*/

	// TODO:  Add your specialized creation code here
	WriteCableRouteCableListInTextFile();

	return TRUE;
}


/**
	@brief	

	@author KHS	

	@date 2009-07-09 오전 11:13:33	

	@param	

	@return		
**/
int CCableRouteCableListDoc::WriteCableRouteCableListInTextFile(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*>  RouteItemList;
	docData.GetELoadItemListOf(RouteItemList , CRouteItem::TypeString());

	const string rProjectPath = docData.GetProjectFolderPath();
	for(list<CELoadItem*>::iterator itr = RouteItemList.begin(); itr != RouteItemList.end();++itr)
	{
		CRouteItem* pRouteItem = static_cast<CRouteItem*>(*itr);
		if(pRouteItem)	
		{
			const string rRouteName = pRouteItem->GetName();

			CString rPath = rProjectPath.c_str() + CString("Working\\") + CString(rRouteName.c_str()) + CString(".txt");
			ofstream ofile(rPath);
			if(ofile.is_open())
			{	
				int nRow = 1;

				DisplayRouteCableList(pRouteItem->m_HV_PowerCableEntry , nRow, ofile);
				DisplayRouteCableList(pRouteItem->m_LV_PowerCableEntry , nRow, ofile);
				DisplayRouteCableList(pRouteItem->m_ControlCableEntry , nRow, ofile);

				ofile.close();
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	종류별 케이블 마다 출력 HV & LV POWER CABLE, CONTROL CABLE

	@author KHS	

	@date 2009-07-09 오전 11:42:57	

	@param	

	@return		
**/
int CCableRouteCableListDoc::DisplayRouteCableList(vector<CCableItem*> CableItemEntry, int& nRow, ofstream& ofile)
{
	if(!CableItemEntry.empty()) 
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		for(vector<CCableItem*>::iterator jtr = CableItemEntry.begin(); jtr != CableItemEntry.end();++jtr,++nRow)
		{
			const string rCableId = (*jtr)->GetName();
			const string rDesc = (*jtr)->prop()->GetValue(_T("General"), _T("Description"));
			const string rFrom = (*jtr)->prop()->GetValue(_T("General"), _T("From"));
			const string rTo = (*jtr)->prop()->GetValue(_T("General"), _T("To"));
			const string rArea = (*jtr)->prop()->GetValue(_T("General"), _T("Area"));
			const string rEquipNo = (*jtr)->prop()->GetValue(_T("General"), _T("Load Id"));

			string rLoadVolt, rRatingCapacity, rLoadUnit;

			CLoadItem * pLoadItem = docData.FindLoadItemByName(rEquipNo);
			if(pLoadItem)
			{
				rLoadVolt = pLoadItem->prop()->GetValue(_T("Rating"), _T("Rated Voltage"));
				rRatingCapacity = pLoadItem->prop()->GetValue(_T("Rating"), _T("Rating Capacity"));
				rLoadUnit = pLoadItem->prop()->GetUnitStr(_T("Rating"), _T("Rating Capacity"));
			}

			const string rCableType = (*jtr)->prop()->GetValue(_T("General"), _T("Cable Spec"));
			const string rCableCore = (*jtr)->prop()->GetValue(_T("General"), _T("Core"));
			const string rCableSize = (*jtr)->prop()->GetValue(_T("General"), _T("Size"));
			const string rCableLength = (*jtr)->prop()->GetValue(_T("General"), _T("Length"));
			const string rCableOD = (*jtr)->prop()->GetValue(_T("Cable Spec"), _T("Cable OD"));
			const string rCableWeight = (*jtr)->prop()->GetValue(_T("Cable Spec"), _T("Weight"));
			const string rConduit = (*jtr)->prop()->GetValue(_T("Conduit"), _T("Conduit Length"));

			CString rRow;
			rRow.Format("%d", nRow);
			ofile << rArea + "\t" << rEquipNo + "\t" << rLoadVolt + "\t" << rRatingCapacity + "\t" << rLoadUnit + "\t";
			ofile << rCableId + "\t" << rFrom + "\t" << rTo + "\t" << rDesc  + "\t" << rCableType  + "\t" << rCableCore + "\t" << rCableSize + "\t";
			ofile << rCableLength  + "\t" << rCableOD + "\t" << rCableWeight  + "\t" << endl;
		}
		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}
