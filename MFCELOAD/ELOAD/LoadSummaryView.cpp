// LoadSummaryView.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "MainFrm.h"
#include "LoadSummaryView.h"
#include "LoadSummaryDoc.h"
#include "ELoadDocData.h"
#include "pyfromc.h"
#include "SimpleAnimationDlg.h"
#include "CompareRevisionDataDoc.h"
#include <Zip\inc\ZipArchive.h>

#define	ROW_HEIGHT	13
#define COL_WIDTH       12
#define BORDER_COLOR    (RGB(0,0,0))
#define TOTAL_COLOR     (RGB(185,243,230))
// CLoadSummaryView

IMPLEMENT_DYNCREATE(CLoadSummaryView, CFormView)

CLoadSummaryView::CLoadSummaryView()
	: CFormView(CLoadSummaryView::IDD)
{

}

CLoadSummaryView::~CLoadSummaryView()
{
}

void CLoadSummaryView::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX , IDC_FPSPREAD_LOAD_SUMMARY , m_wndSpread);

	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLoadSummaryView, CFormView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_SAVE, &CLoadSummaryView::OnFileSave)
	ON_COMMAND(ID_FILE_PRINT , &CLoadSummaryView::OnPrintLoadSummary)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CLoadSummaryView diagnostics

#ifdef _DEBUG
void CLoadSummaryView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CLoadSummaryView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CLoadSummaryView message handlers
int CLoadSummaryView::ReCalcLayout(void)
{
	return ERROR_SUCCESS;
}


int CLoadSummaryView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CLoadSummaryView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if(m_wndSpread.GetSafeHwnd())
	{
		m_wndSpread.SetWindowPos(NULL , 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

struct MatchTitleName : binary_function<CLoadSummaryDoc::LOAD_SUMMARY_ITEM , CString, bool>
{
	bool operator()(CLoadSummaryDoc::LOAD_SUMMARY_ITEM lhs , const CString& rhs) const
	{
		CString _rTemp = rhs;
		_rTemp.MakeUpper();
		return (_rTemp == lhs.rFileTitle.MakeUpper());
	}
};

/**
	@brief	database에서 load summary 데이터를 읽어 spread sheet에 표기한다.

	@author	KHS,HumKyung Baek

	@date	????.??.??

	@param

	@return
*/
void CLoadSummaryView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CString strDir = docData.GetProjectFolderPath().c_str();
	CLoadSummaryDoc* pDoc = (CLoadSummaryDoc*)GetDocument();
	if((_T("ALL BUS LOAD SUMMARY") == pDoc->m_rLoadSummaryType) || (_T("ALL PDB LOAD SUMMARY") == pDoc->m_rLoadSummaryType))
	{
		strDir += _T("Load Summary Result\\*.BUS");
	}
	else if((_T("ALL REV BUS LOAD SUMMARY") == pDoc->m_rLoadSummaryType) || (_T("ALL REV PDB LOAD SUMMARY") == pDoc->m_rLoadSummaryType))
	{
		const CString rRevPath = CCompareRevisionDataDoc::GetRevPath(pDoc->m_rProductName.c_str() , pDoc->m_rLoadSummaryRevNo.c_str());
		strDir = rRevPath + _T("\\*.BUS");
	}
	else if(_T("ONLY LOAD SUMMARY") == pDoc->m_rLoadSummaryType)
	{
		strDir += _T("TOTAL.BUS");
	}

	pDoc->m_LoadSummaryItemEntry.clear();
	//! BUS 파일 이름을 구한다.
	list<CELoadItem*> BusGroupList;
	docData.GetELoadItemListOf(BusGroupList , CBusGroupItem::TypeString());
	if(/*pBusGroupList && */((_T("ALL BUS LOAD SUMMARY") == pDoc->m_rLoadSummaryType) || (_T("ALL PDB LOAD SUMMARY") == pDoc->m_rLoadSummaryType)))
	{
		for(list<CELoadItem*>::iterator itr = BusGroupList.begin();itr != BusGroupList.end();++itr)
		{
			if((*itr)->IsDeleted()) continue;
			const string rBusGroupID = CString((*itr)->GetName().c_str()).MakeUpper().operator LPCSTR();
			if(	((_T("ALL BUS LOAD SUMMARY") == pDoc->m_rLoadSummaryType) && (CString(SUBBUS).MakeUpper() != CString(rBusGroupID.c_str()))) ||
				((_T("ALL PDB LOAD SUMMARY") == pDoc->m_rLoadSummaryType) && (CString(SUBBUS).MakeUpper() == CString(rBusGroupID.c_str()))))
			{
				CBusGroupItem* pBusGroupItem = static_cast<CBusGroupItem*>(*itr);
				for(vector<CBusItem*>::iterator jtr = pBusGroupItem->m_BusRefEntry.begin(); jtr != pBusGroupItem->m_BusRefEntry.end();++jtr)
				{
					///! 삭제가 안된 BUS만 넘겨준다.
					if(!(*jtr)->IsDeleted())
					{
						CLoadSummaryDoc::LOAD_SUMMARY_ITEM record;
						record.rFileTitle = (*jtr)->GetName().c_str();
						pDoc->m_LoadSummaryItemEntry.push_back(record);
					}
				}
			}
		}
	}

	CFileFind file;
	int nBusFileCount = 0;

	///! 확장자 *.bus 파일을 찾는다.
	BOOL bFound = file.FindFile(strDir);
	if(bFound)
	{
		while(bFound)
		{
			bFound = file.FindNextFile();
			if(file.IsDots()) continue;

			//! LOAD_SUMMARY_ITEM record;
			const CString rFileName = file.GetFileTitle().MakeUpper();
			if(((_T("ALL BUS LOAD SUMMARY") == pDoc->m_rLoadSummaryType) || (_T("ALL PDB LOAD SUMMARY") == pDoc->m_rLoadSummaryType)))
			{
				vector<CLoadSummaryDoc::LOAD_SUMMARY_ITEM>::iterator where = find_if(pDoc->m_LoadSummaryItemEntry.begin(),pDoc->m_LoadSummaryItemEntry.end(), bind2nd(MatchTitleName(),file.GetFileTitle()));
				if( where != pDoc->m_LoadSummaryItemEntry.end())
				{
					where->rFilePath = file.GetFilePath();
					nBusFileCount++;
				}
			}
			else if((_T("ALL REV BUS LOAD SUMMARY") == pDoc->m_rLoadSummaryType) || (_T("ALL REV PDB LOAD SUMMARY") == pDoc->m_rLoadSummaryType))
			{
				CLoadSummaryDoc::LOAD_SUMMARY_ITEM record;
				record.rFileTitle = rFileName;
				record.rFilePath  = file.GetFilePath().MakeUpper();
				pDoc->m_LoadSummaryItemEntry.push_back(record);
				nBusFileCount++;
			}

			if((_T("ALL BUS LOAD SUMMARY") == pDoc->m_rLoadSummaryType) && _T("TOTALBUS") == file.GetFileTitle().MakeUpper())
			{
				CLoadSummaryDoc::LOAD_SUMMARY_ITEM record;
				record.rFileTitle = file.GetFileTitle().MakeUpper();
				record.rFilePath  = file.GetFilePath();
				pDoc->m_LoadSummaryItemEntry.push_back(record);
				nBusFileCount++;
			}
			else if((_T("ALL PDB LOAD SUMMARY") == pDoc->m_rLoadSummaryType) && _T("TOTALSUBBUS") == file.GetFileTitle().MakeUpper())
			{
				CLoadSummaryDoc::LOAD_SUMMARY_ITEM record;
				record.rFileTitle = file.GetFileTitle().MakeUpper();
				record.rFilePath  = file.GetFilePath();
				pDoc->m_LoadSummaryItemEntry.push_back(record);
				nBusFileCount++;
			}
		}
		m_wndSpread.SetSheetCount(nBusFileCount);
	}
	else
	{
		return;
	}

	m_wndSpread.SetReDraw(FALSE);

	///! Total tmep를 찾아서 제일 먼저 시트를 만들어 준다.
	if(FALSE == LoadTotalTempSheet())
	{
		MessageBox(_T("Fail to read total file!"));
		return;
	}

	//! create tab according to bus order.
	vector<CBusItem*> BusItemEntry;
	docData.GetAllBusItems(BusItemEntry);
	
	///! 찾은 파일을 spread 각각의 시트로 뿌려준다.
	int nSheet = 2;
	for(vector<CBusItem*>::iterator itr = BusItemEntry.begin();itr != BusItemEntry.end();++itr)
	{
		//! 앞서 만들었으므로 PASS
		CString rString((*itr)->GetName().c_str());
		rString = rString.MakeUpper();
		if(0 == rString.Find(_T("TOTAL"))) continue;

		vector<CLoadSummaryDoc::LOAD_SUMMARY_ITEM>::iterator where = find_if(pDoc->m_LoadSummaryItemEntry.begin() , pDoc->m_LoadSummaryItemEntry.end() , bind2nd(MatchTitleName() , rString));
		if(where != pDoc->m_LoadSummaryItemEntry.end())
		{
			if(!((where)->rFilePath.IsEmpty()) && (TRUE == FileExist((where)->rFilePath)))
			{
				m_wndSpread.SetSheet(nSheet);
				if(TRUE == m_wndSpread.LoadTextFile(where->rFilePath, _T("") , _T("\t") , _T("\n") , NULL/*m_wndSpread.LoadTextFileColHeaders|m_wndSpread.LoadTextFileRowHeaders*/, _T("")))
				{
					m_wndSpread.SetColHeaderRows(3);
					m_wndSpread.put_ColWidth(1, COL_WIDTH);
					m_wndSpread.put_ColWidth(2, COL_WIDTH * 2);
					m_wndSpread.SetRowHeadersShow(TRUE);
					m_wndSpread.SetMaxCols(20);
					m_wndSpread.SetEditMode(FALSE);
					m_wndSpread.SetSheetName(where->rFileTitle);

					SetBusSheetItemIdColor();
					CreateBusSheetHeader();
					AdjustBusSheetTextAlign();

					ApplyModifiedColorToCell();
				}
				else
				{
					MessageBox("Fail to read text file!");
				}

				nSheet++;
			}
		}
	}

	//! zip display files
	CZipArchive zip;
	{
		CString strProjectPath = docData.GetProjectFolderPath().c_str();
		CString strZipFilePath;
		if((_T("ALL BUS LOAD SUMMARY") == pDoc->m_rLoadSummaryType))
			strZipFilePath = strProjectPath + _T("Load Summary Result\\BUS_LoadSummary.zip");
		else if((_T("ALL PDB LOAD SUMMARY") == pDoc->m_rLoadSummaryType))
			strZipFilePath = strProjectPath + _T("Load Summary Result\\SubBUS_LoadSummary.zip");
		if(!strZipFilePath.IsEmpty())
		{
			zip.Open(strZipFilePath , CZipArchive::create );

			for(vector<CLoadSummaryDoc::LOAD_SUMMARY_ITEM>::iterator itr = pDoc->m_LoadSummaryItemEntry.begin(); itr != pDoc->m_LoadSummaryItemEntry.end();++itr)
			{
				zip.AddNewFile(itr->rFilePath , -1 , false);
			}
			zip.Close();
		}
	}

	m_wndSpread.SetActiveSheet(1);
	m_wndSpread.SetReDraw(TRUE);
}

/**
        @brief 데이타를 저장한다.
*/
void CLoadSummaryView::OnFileSave()
{
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp) pApp->OnSaveDataBase();
}

static UINT gfnPrintLoadSummary(LPVOID pVoid)
{
	CSimpleAnimationDlg& anidlg = CSimpleAnimationDlg::GetInstance();
	if(NULL == anidlg.GetSafeHwnd())
	{
		anidlg.DoModal();
	}

	return ERROR_SUCCESS;
}

/**
	@brief Load Summary Data를 Excel File로 프린터한다.

	@author	HumKyung Baek

	@date	????.??.??

	@param	

	@return	NONE
*/
void CLoadSummaryView::OnPrintLoadSummary()
{	
	CLoadSummaryDoc* pDoc = (CLoadSummaryDoc*)GetDocument();
	if(pDoc) ExportToExcel(pDoc->m_rFolderPath.c_str() , pDoc->m_rLoadSummaryType.c_str());
}

/**
	@brief  TOTAL_TEMPLEATE 해더를 생성한다.

	@author KHS
*/
void CLoadSummaryView::CreateTotalSheetHeader(void)
{
	m_wndSpread.put_RowHeight(m_wndSpread.SpreadHeader + 1, 40);
	

	m_wndSpread.SetColHeaderRows(3);
	m_wndSpread.SetMaxCols(17);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
	m_wndSpread.SetCol(1);
	m_wndSpread.SetText("Bus No.");

	m_wndSpread.AddCellSpan(1, 0, 1, 3);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
	m_wndSpread.SetCol(2);
	m_wndSpread.SetText("Bus Description.");
	m_wndSpread.AddCellSpan(2, 0, 1, 3);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
	m_wndSpread.SetCol(3);
	m_wndSpread.SetText("Bus Volt.");
	m_wndSpread.AddCellSpan(3, 0, 1, 2);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 2);
	m_wndSpread.SetCol(3);
	m_wndSpread.SetText("[KV]");


	m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
	m_wndSpread.SetCol(4);
	m_wndSpread.SetText("Load Volt.");
	m_wndSpread.AddCellSpan(4, 0, 1, 2);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 2);
	m_wndSpread.SetCol(4);
	m_wndSpread.SetText("[KV]");

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
	m_wndSpread.SetCol(5);
	m_wndSpread.SetText("Maximum Load Summary");
	m_wndSpread.AddCellSpan(5, 0, 3, 1);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
	m_wndSpread.SetCol(5);
	m_wndSpread.SetText("[kW]");
	m_wndSpread.AddCellSpan(5, m_wndSpread.SpreadHeader + 1, 1, 2);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
	m_wndSpread.SetCol(6);
	m_wndSpread.SetText("[kVAr]");
	m_wndSpread.AddCellSpan(6, m_wndSpread.SpreadHeader + 1, 1, 2);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
	m_wndSpread.SetCol(7);
	m_wndSpread.SetText("[kVA]");
	m_wndSpread.AddCellSpan(7, m_wndSpread.SpreadHeader + 1, 1, 2);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
	m_wndSpread.SetCol(8);
	m_wndSpread.SetText("DUTY");
	m_wndSpread.AddCellSpan(8, 0, 3, 1);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
	m_wndSpread.SetCol(8);
	m_wndSpread.SetText("Continuous");
	m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);
	m_wndSpread.AddCellSpan(8, m_wndSpread.SpreadHeader + 1, 1, 2);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
	m_wndSpread.SetCol(9);
	m_wndSpread.SetText("Intermittent");
	m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);
	m_wndSpread.AddCellSpan(9, m_wndSpread.SpreadHeader + 1, 1, 2);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
	m_wndSpread.SetCol(10);
	m_wndSpread.SetText("Stand-by");
	m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);
	m_wndSpread.AddCellSpan(10, m_wndSpread.SpreadHeader + 1, 1, 2);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
        m_wndSpread.SetCol(11);
        m_wndSpread.SetText("Cosumed Load");
        m_wndSpread.AddCellSpan(11, 0, 6, 1);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
        m_wndSpread.SetCol(11);
        m_wndSpread.SetText("Continuous");
        m_wndSpread.AddCellSpan(11, m_wndSpread.SpreadHeader + 1, 2, 1);

        m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
        m_wndSpread.SetCol(13);
        m_wndSpread.SetText("Intermittent");
        m_wndSpread.AddCellSpan(13, m_wndSpread.SpreadHeader + 1, 2, 1);

        m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
        m_wndSpread.SetCol(15);
        m_wndSpread.SetText("Stand-by");
        m_wndSpread.AddCellSpan(15, m_wndSpread.SpreadHeader + 1, 2, 1);

        m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 2);
        m_wndSpread.SetCol(11);
        m_wndSpread.SetText("[kW]");

        m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 2);
        m_wndSpread.SetCol(12);
        m_wndSpread.SetText("[kVAr]");

        m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 2);
        m_wndSpread.SetCol(13);
        m_wndSpread.SetText("[kW]");

        m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 2);
        m_wndSpread.SetCol(14);
        m_wndSpread.SetText("[kVAr]");

        m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 2);
        m_wndSpread.SetCol(15);
        m_wndSpread.SetText("[kW]");

        m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 2);
        m_wndSpread.SetCol(16);
        m_wndSpread.SetText("[kVAr]");

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
        m_wndSpread.SetCol(17);
        m_wndSpread.SetText("Remark");
        m_wndSpread.AddCellSpan(17, m_wndSpread.SpreadHeader, 1, 3);
	
}
/**
        @brief  BUS_TEMPLEATE 해더를 생성한다.

        @author KHS
*/
void CLoadSummaryView::CreateBusSheetHeader(void)
{
	m_wndSpread.put_RowHeight(m_wndSpread.SpreadHeader + 1, 20);
	m_wndSpread.put_RowHeight(m_wndSpread.SpreadHeader + 2, 30);
	
	m_wndSpread.SetRowHeaderCols(1);
	m_wndSpread.SetColHeaderRows(4);
	m_wndSpread.SetMaxCols(18);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
	m_wndSpread.SetCol(0);
	m_wndSpread.SetText(_T("No."));
	m_wndSpread.AddCellSpan(m_wndSpread.SpreadHeader, m_wndSpread.SpreadHeader, 1, 4);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
	m_wndSpread.SetCol(1);
	m_wndSpread.SetText(_T("EQUIPMENT"));
	m_wndSpread.AddCellSpan(1, m_wndSpread.SpreadHeader, 11, 1);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
	m_wndSpread.SetCol(1);
	m_wndSpread.SetText(_T("Equipment\n No."));
	m_wndSpread.AddCellSpan(1, m_wndSpread.SpreadHeader + 1, 1, 3);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
	m_wndSpread.SetCol(2);
	m_wndSpread.SetText(_T("Description"));
	m_wndSpread.AddCellSpan(2, m_wndSpread.SpreadHeader + 1, 1, 3);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
	m_wndSpread.SetCol(3);
	m_wndSpread.SetText(_T("RATED VOLTS"));
	m_wndSpread.AddCellSpan(3, m_wndSpread.SpreadHeader + 1, 1, 2);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 3);
	m_wndSpread.SetCol(3);
	m_wndSpread.SetText(_T("[KV]"));

	///// -3
	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
	m_wndSpread.SetCol(4);
	m_wndSpread.SetText(_T("RATING\n CAPACITY"));
	m_wndSpread.AddCellSpan(4, m_wndSpread.SpreadHeader + 1, 1, 2);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 3);
	m_wndSpread.SetCol(4);
	m_wndSpread.SetText(_T(" "));

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
	m_wndSpread.SetCol(5);
	m_wndSpread.SetText(_T("UNIT"));
	m_wndSpread.AddCellSpan(5, m_wndSpread.SpreadHeader + 1, 1, 2);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 3);
	m_wndSpread.SetCol(5);
	m_wndSpread.SetText(_T(" "));

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
	m_wndSpread.SetCol(6);
	m_wndSpread.SetText(_T("ABSORBED\n LOAD"));
	m_wndSpread.AddCellSpan(6, m_wndSpread.SpreadHeader + 1, 1, 2);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 3);
	m_wndSpread.SetCol(6);
	m_wndSpread.SetText(_T(" "));

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
	m_wndSpread.SetCol(7);
	m_wndSpread.SetText(_T("TYPE"));
	m_wndSpread.AddCellSpan(7, m_wndSpread.SpreadHeader + 1, 1, 2);
	
	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 3);
	m_wndSpread.SetCol(7);
	m_wndSpread.SetText(_T(" "));

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
	m_wndSpread.SetCol(8);
	m_wndSpread.SetText(_T("DUTY"));
	m_wndSpread.AddCellSpan(8, m_wndSpread.SpreadHeader + 1, 1, 2);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 3);
	m_wndSpread.SetCol(8);
	m_wndSpread.SetText(_T("C,I,S"));


	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
	m_wndSpread.SetCol(9);
	m_wndSpread.SetText(_T("LOAD FACTOR"));
	m_wndSpread.AddCellSpan(9, m_wndSpread.SpreadHeader + 1, 1, 2);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 3);
	m_wndSpread.SetCol(9);
	m_wndSpread.SetText(_T(" "));

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
	m_wndSpread.SetCol(10);
	m_wndSpread.SetText(_T("PF."));
	m_wndSpread.AddCellSpan(10, m_wndSpread.SpreadHeader + 1, 1, 2);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 3);
	m_wndSpread.SetCol(10);
	m_wndSpread.SetText(_T(" "));

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
	m_wndSpread.SetCol(11);
	m_wndSpread.SetText(_T("EFF."));
	m_wndSpread.AddCellSpan(11, m_wndSpread.SpreadHeader + 1, 1, 2);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 3);
	m_wndSpread.SetCol(11);
	m_wndSpread.SetText(_T(" "));

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
	m_wndSpread.SetCol(12);
	m_wndSpread.SetText(_T("CONSUMED LOAD"));
	m_wndSpread.AddCellSpan(12, m_wndSpread.SpreadHeader, 6, 2);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 2);
	m_wndSpread.SetCol(12);
	m_wndSpread.SetText(_T("Continuous"));
	m_wndSpread.AddCellSpan(12, m_wndSpread.SpreadHeader + 2, 2, 1);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 3);
	m_wndSpread.SetCol(12);
	m_wndSpread.SetText(_T("[kW]"));
	m_wndSpread.AddCellSpan(12, m_wndSpread.SpreadHeader + 3, 1, 1);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 3);
	m_wndSpread.SetCol(13);
	m_wndSpread.SetText(_T("[kVAr]"));
	m_wndSpread.AddCellSpan(13, m_wndSpread.SpreadHeader + 3, 1, 1);


	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 2);
	m_wndSpread.SetCol(14);
	m_wndSpread.SetText(_T("Intermittent"));
	m_wndSpread.AddCellSpan(14, m_wndSpread.SpreadHeader + 2, 2, 1);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 3);
	m_wndSpread.SetCol(14);
	m_wndSpread.SetText(_T("[kW]"));
	m_wndSpread.AddCellSpan(14, m_wndSpread.SpreadHeader + 3, 1, 1);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 3);
	m_wndSpread.SetCol(15);
	m_wndSpread.SetText(_T("[kVAr]"));
	m_wndSpread.AddCellSpan(15, m_wndSpread.SpreadHeader + 3, 1, 1);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 2);
	m_wndSpread.SetCol(16);
	m_wndSpread.SetText(_T("Stand-by"));
	m_wndSpread.AddCellSpan(16, m_wndSpread.SpreadHeader + 2, 2, 1);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 3);
	m_wndSpread.SetCol(16);
	m_wndSpread.SetText(_T("[kW]"));
	m_wndSpread.AddCellSpan(16, m_wndSpread.SpreadHeader + 3, 1, 1);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 3);
	m_wndSpread.SetCol(17);
	m_wndSpread.SetText(_T("[kVAr]"));
	m_wndSpread.AddCellSpan(17, m_wndSpread.SpreadHeader + 3, 1, 1);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
	m_wndSpread.SetCol(18);
	m_wndSpread.SetText(_T("Remark"));
	m_wndSpread.AddCellSpan(18, m_wndSpread.SpreadHeader, 1, 4);

}

/**
	@brief  TotalBUS.bus 혹은 TotalSubBUS.bus 파일을 읽어서 Sheet를 만든다.

	@author KHS
*/
BOOL CLoadSummaryView::LoadTotalTempSheet(void)
{
	CLoadSummaryDoc* pDoc = (CLoadSummaryDoc*)GetDocument();
	if(pDoc)
	{
		CString rLoadSummaryType;
		if((_T("ALL BUS LOAD SUMMARY") == pDoc->m_rLoadSummaryType) || (_T("ALL REV BUS LOAD SUMMARY") == pDoc->m_rLoadSummaryType))
		{
			rLoadSummaryType = _T("TOTALBUS");
		}
		else
		{
			rLoadSummaryType = _T("TOTALSUBBUS");
		}
		vector<CLoadSummaryDoc::LOAD_SUMMARY_ITEM>::iterator where = find_if(pDoc->m_LoadSummaryItemEntry.begin(),pDoc->m_LoadSummaryItemEntry.end(), bind2nd(MatchTitleName(),rLoadSummaryType));
		if(where != pDoc->m_LoadSummaryItemEntry.end())
		{
			if(TRUE == FileExist(where->rFilePath))
			{
				m_wndSpread.SetSheet(1);
				if(TRUE == m_wndSpread.LoadTextFile(where->rFilePath, "", "\t", "\n", NULL, ""))
				{
					m_wndSpread.SetColHeaderRows(3);
					m_wndSpread.SetMaxCols(17);
					m_wndSpread.SetRowHeadersShow(FALSE);
					m_wndSpread.SetEditMode(FALSE);
					m_wndSpread.put_ColWidth(1, COL_WIDTH);
					m_wndSpread.SetSheetName(_T("SUMMARY"));
					SetTotalSheetItemIdColor();
					m_wndSpread.put_ColWidth(2 , ROW_HEIGHT);

					CreateTotalSheetHeader();
					AdjustTotalSheetTextAlign();
					
					ApplyModifiedColorToCell();
				}
				else
				{
					return FALSE;
				}
			}
		}
		else
		{
			return FALSE;
		}
		return TRUE;
	}

	return FALSE;
}

BOOL CLoadSummaryView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
}

/**
	@brief	

	@author	BHK
*/
int CLoadSummaryView::AdjustTotalSheetTextAlign(void)
{
	static const int COLS[]={3,4,5,6,7,8,9,10,11,12,13,14,15,16};
	const long nMaxRows = m_wndSpread.GetMaxRows();
	for(int i = 1;i <= nMaxRows;++i)
	{
		m_wndSpread.put_RowHeight(i , ROW_HEIGHT);
		m_wndSpread.SetRow(i);
		m_wndSpread.SetCol(1);

		const int nColsCount = sizeof(COLS) / sizeof(COLS[0]);
		for(int j = 0;j < nColsCount;++j)
		{
			m_wndSpread.SetCol(COLS[j]);
			if(COLS[j] == 8 || COLS[j] ==9 ||COLS[j] ==10)
			{
				m_wndSpread.SetTypeHAlign(m_wndSpread.TypeHAlignCenter);
			}
			else
			{
				m_wndSpread.SetTypeHAlign(m_wndSpread.TypeHAlignRight);
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	BUS SHEET의 텍스트 정렬을 맞춘다.

	@author	BHK
*/
int CLoadSummaryView::AdjustBusSheetTextAlign(void)
{
	static const int __COLS__[]={3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18};
	const int nColsCount = sizeof(__COLS__) / sizeof(__COLS__[0]);

	const long nMaxRows = m_wndSpread.GetMaxRows();
	for(int i = 1;i <= nMaxRows;++i)
	{
		m_wndSpread.put_RowHeight(i , ROW_HEIGHT);

		m_wndSpread.SetRow(i);
		m_wndSpread.SetCol(1);
		m_wndSpread.SetTypeHAlign(m_wndSpread.TypeHAlignLeft);

		for(int j = 0;j < nColsCount;++j)
		{
			m_wndSpread.SetCol(__COLS__[j]);
			///! UNIT , OP, LT, POLE은 중앙 정렬
			if((5 == __COLS__[j]) || (7 == __COLS__[j]) || (8 == __COLS__[j])) 
			{
				m_wndSpread.SetTypeHAlign(m_wndSpread.TypeHAlignCenter);
			}
			else
			{
				m_wndSpread.SetTypeHAlign(m_wndSpread.TypeHAlignRight);
			}
		}
	}

	return ERROR_SUCCESS;
}
/**
        @brief Sheet 의 색을 칠한다.
*/
void CLoadSummaryView::SetBusSheetItemIdColor()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const COLORREF color = docData.GetColorCode("ITEM_ID_BACK_COLOR");
	const COLORREF totalBackColor = docData.GetColorCode("LOAD_SUMMARY_TOTAL_BACK_COLOR");

	const int nMaxRows = m_wndSpread.GetMaxRows();
	const int nMaxCols = m_wndSpread.GetMaxCols();
	for(int i = 1;i < nMaxRows - 1;i++)
	{
		m_wndSpread.put_RowHeight(i , ROW_HEIGHT);
		m_wndSpread.SetCol(1);
		m_wndSpread.SetRow(i);
		m_wndSpread.SetAllowCellOverflow(FALSE);
		m_wndSpread.SetBackColor(color);
		m_wndSpread.SetBackColorStyle(m_wndSpread.BackColorStyleUnderGrid);
	}
	for(int j = 1;j <= nMaxCols;j++)
	{
		m_wndSpread.SetRow(nMaxRows);
		m_wndSpread.SetCol(j);
		m_wndSpread.SetBackColor(totalBackColor); 
	}
}
/**
        @brief total Sheet 의 ITEM ID의 색을 칠한다.

	@author	KHS,BHK
*/
void CLoadSummaryView::SetTotalSheetItemIdColor()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const COLORREF totalBackColor = docData.GetColorCode("LOAD_SUMMARY_TOTAL_BACK_COLOR");

	const int nMaxRows = m_wndSpread.GetMaxRows();
	const int nMaxCols = m_wndSpread.GetMaxCols();

	m_wndSpread.SetRow(nMaxRows);
	//! TOTAL 
	for(int i = 1;i <= nMaxCols;i++)
	{
		m_wndSpread.SetCol(i);
		m_wndSpread.SetBackColor(totalBackColor); 
	}
}

/**
	@brief	Load Summary Data Sheet를 xls로 출력한다.

	@author KHS	

	@date 2009-06-04 오전 11:12:50	

	@param	

	@return		
**/
int CLoadSummaryView::ExportToExcel(const CString& rFolderPath , const CString& rType)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CString rProjectFolderPath = docData.GetProjectFolderPath().c_str();
	const CString rOutputFilePath = rProjectFolderPath + _T("Working\\LOAD_SUMMARY_BUS.XLS");
	//! 기존의 파일을 지우도록 한다.
	::DeleteFile(rOutputFilePath);

	if(FALSE == FileExist(rOutputFilePath))
	{
		CWinThread* pThread = AfxBeginThread(gfnPrintLoadSummary , NULL , THREAD_PRIORITY_NORMAL,0);

		if((_T("ALL BUS LOAD SUMMARY") == rType) || (_T("ALL REV BUS LOAD SUMMARY") == rType))
		{
			python.CallPyWriteLoadSummaryToExcel(rFolderPath);
		}
		else if((_T("ALL PDB LOAD SUMMARY") == rType) || (_T("ALL REV PDB LOAD SUMMARY") == rType))
		{
			python.CallPyWritePDBLoadSummaryToExcel(rFolderPath);
		}
		
		if(NULL != pThread)
		{
			CSimpleAnimationDlg& anidlg = CSimpleAnimationDlg::GetInstance();
			if(anidlg.GetSafeHwnd()) anidlg.SendMessage(WM_COMMAND , IDOK);
		}
	}
	else
	{
		AfxMessageBox(docData.GetStringOf(51).c_str());
	}

	/*
	CELoadDocData &docData = CELoadDocData::GetInstance();
	const string rProjectPath = docData.GetProjectFolderPath();
	CString rPath = rProjectPath.c_str() + CString("LOAD_SUMMARY.xls");
	*/

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK

	@date	2009.10.26

	@param	

	@return
*/
int CLoadSummaryView::ApplyModifiedColorToCell(void)
{
	for(int i = 0;i < m_wndSpread.GetMaxRows();++i)
	{
		m_wndSpread.SetRow(i);
		for(int j = 0;j < m_wndSpread.GetMaxCols();++j)
		{
			m_wndSpread.SetCol(j);
			
			CString rText = m_wndSpread.GetText();
			const int nFound = rText.ReverseFind('*');
			if((-1 != nFound))
			{
				if(rText.GetLength() == (rText.ReverseFind('*') + 1))
				{
					rText = rText.Left(rText.GetLength() - 1);
					m_wndSpread.SetText(rText);
					m_wndSpread.SetBackColor(RGB(255,0,0));
					m_wndSpread.SetBackColorStyle(m_wndSpread.BackColorStyleUnderGrid);
				}
			}
		}
	}

	return ERROR_SUCCESS;
}
