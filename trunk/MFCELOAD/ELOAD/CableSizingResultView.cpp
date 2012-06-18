// CableSizingResultView.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "CableSizingResultView.h"
#include <vector>
#include "ELoadDocData.h"
#include "CableItem.h"
#include "CableSizingResultDoc.h"
#include "SimpleAnimationDlg.h"
#include "pyfromc.h"
#include <sys/stat.h>

#define ROUND(x) ((int)((x+0.005f)*100.0f))/100.0f //소숫점 2자리로 반올림
#define ROW_HEIGHT 13
// CCableSizingResultView

IMPLEMENT_DYNCREATE(CCableSizingResultView, CFormView)

CCableSizingResultView::CCableSizingResultView()
	: CFormView(CCableSizingResultView::IDD)
{

}

CCableSizingResultView::~CCableSizingResultView()
{
}

void CCableSizingResultView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FPSPREAD_CABLESIZING_RESULT_VIEW, m_wndSpread);
}

BEGIN_MESSAGE_MAP(CCableSizingResultView, CFormView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_SAVE, &CCableSizingResultView::OnFileSave)
	ON_COMMAND(ID_FILE_PRINT, &CCableSizingResultView::OnFilePrint)
END_MESSAGE_MAP()


// CCableSizingResultView diagnostics

#ifdef _DEBUG
void CCableSizingResultView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CCableSizingResultView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CCableSizingResultView message handlers

void CCableSizingResultView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	m_wndSpread.SetReDraw(FALSE);
	DisplayCableResultData();
	m_wndSpread.SetReDraw(TRUE);
}

int CCableSizingResultView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

void CCableSizingResultView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if(m_wndSpread.GetSafeHwnd())
	{
		m_wndSpread.SetWindowPos(NULL , 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

__int64 getFileSize(const CString& rFilePath)
{
	struct _stati64 statbuf;

	if(_stati64(rFilePath , &statbuf)) return -1;

	return statbuf.st_size;
}

/**
	@brief  CREATE CABLE SIZING RESULT SHEET

	@author KHS,BHK

	@date	?

	@param	

	@return
*/
int CCableSizingResultView::DisplayCableResultData(void)
{
	CCableSizingResultDoc* pDoc = (CCableSizingResultDoc*)GetDocument();
	CELoadDocData& docData = CELoadDocData::GetInstance();

	const CString rInterestingFilePath = pDoc->m_rFolderPath;

	list<CELoadItem*> BusGroupItemList;
	docData.GetELoadItemListOf(BusGroupItemList , CBusGroupItem::TypeString());

	int nSheet = 1;
	for(list<CELoadItem*>::iterator itr = BusGroupItemList.begin(); itr != BusGroupItemList.end();++itr)
	{
		CBusGroupItem* pBusGroupItem = static_cast<CBusGroupItem*>(*itr);
		const string rBusGroupName = pBusGroupItem->GetName();
		//! INCOMING BUS도 표현하도록 변경됨.
		//! if(/*CCableSizingResultDoc::m_InterestingLoadItemList.front()->IsKindOf(CLoadItem::TypeString()) &&*/ (INCOMING == rBusGroupName) || pBusGroupItem->IsDeleted()) continue;

		for(vector<CBusItem*>::iterator jtr = pBusGroupItem->m_BusRefEntry.begin();jtr != pBusGroupItem->m_BusRefEntry.end();++jtr)
		{
			const string rBusId = (*jtr)->GetName();
			if(FileExist(rInterestingFilePath + CString(rBusId.c_str()) + ".BUS"))
			{
				CFile file;
				CFileStatus rStatus;
				file.GetStatus(rInterestingFilePath + CString(rBusId.c_str()) + ".BUS" , rStatus);
				if(rStatus.m_size < 1) continue;

				m_wndSpread.SetSheetCount(nSheet);
				m_wndSpread.SetSheet(nSheet);
				if(TRUE == m_wndSpread.LoadTextFile(rInterestingFilePath + rBusId.c_str() + ".BUS" ,"", "\t", "\n", NULL, ""))
				{
					m_wndSpread.SetSheetName(rBusId.c_str());
					m_wndSpread.SetColHeaderRows(2);
					m_wndSpread.SetRowHeaderCols(1);
					///m_wndSpread.SetRowHeadersShow(FALSE);
					m_wndSpread.SetMaxCols(30);
					m_wndSpread.SetColsFrozen(6);	//! UNIT까지 스크롤 고정

					//! 시트의 해더를 생성
					CreateHeader();

					//! 배경 색상 설정.
					SetBackgroundColor(m_wndSpread);

					AdjustSheetTextAlign();
					++nSheet;
				}
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief  SHEET의 해더를 생성한다.
	@author KHS
*/
int CCableSizingResultView::CreateHeader(void)
{
	try
	{
		m_wndSpread.put_RowHeight(m_wndSpread.SpreadHeader, ROW_HEIGHT * 2);
		m_wndSpread.put_RowHeight(m_wndSpread.SpreadHeader + 1, 60);

		//! SET COLUMN WIDTH
		m_wndSpread.put_ColWidth(1 , 3);	// REV
		m_wndSpread.put_ColWidth(2 , 3);	// NO
		m_wndSpread.put_ColWidth(3 , 13);	// LOAD ID
		m_wndSpread.put_ColWidth(14 , 15);	// CABLEID
		m_wndSpread.put_ColWidth(16 , 13);	// CABLE TYPE

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(1);
		m_wndSpread.SetText(_T("REV"));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);
		m_wndSpread.AddCellSpan(1, m_wndSpread.SpreadHeader, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(2);
		m_wndSpread.SetText(_T("NO"));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);
		m_wndSpread.AddCellSpan(2, m_wndSpread.SpreadHeader, 1, 2);

		//! LOAD DATA
		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(3);
		m_wndSpread.SetText(_T("LOAD DATA"));
		m_wndSpread.AddCellSpan(3, m_wndSpread.SpreadHeader, 4, 1);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(3);
		m_wndSpread.SetText(_T("ITEM NO."));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(4);
		m_wndSpread.SetText(_T("VOLT \n[KV]"));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(5);
		m_wndSpread.SetText(_T("RATING"));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);
		
		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(6);
		m_wndSpread.SetText(_T("UNIT"));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(7);
		m_wndSpread.SetText(_T(""));
		m_wndSpread.AddCellSpan(7, m_wndSpread.SpreadHeader, 7, 1);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(7);
		m_wndSpread.SetText(_T("PF"));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(8);
		m_wndSpread.SetText(_T("EFF"));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(9);
		m_wndSpread.SetText(_T("FLC"));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(10);
		m_wndSpread.SetText(_T("SF"));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(11);
		m_wndSpread.SetText(_T("FLC * SF"));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(12);
		m_wndSpread.SetText(_T("STARTING \nAMP."));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(13);
		m_wndSpread.SetText(_T("STARTING \nPF"));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);

		//!CABLE DATA
		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(14);
		m_wndSpread.SetText(_T("CABLE DATA"));
		m_wndSpread.AddCellSpan(14, m_wndSpread.SpreadHeader, 10, 1);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(14);
		m_wndSpread.SetText(_T("CABLE ID"));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(15);
		m_wndSpread.SetText(_T("CABLE \nVOLT"));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(16);
		m_wndSpread.SetText(_T("CABLE \nSPEC"));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(17);
		m_wndSpread.SetText(_T("CORE"));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(18);
		m_wndSpread.SetText(_T("CABLE \nSIZE"));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(19);
		m_wndSpread.SetText(_T("CABLE \nLENGTH"));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(20);
		m_wndSpread.SetText(_T("R \n[Ohm/km]"));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(21);
		m_wndSpread.SetText(_T("X \n[Ohm/km]"));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(22);
		m_wndSpread.SetText(_T("CABLE \nAMP."));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(23);
		m_wndSpread.SetText(_T("DERATING \nFACTOR"));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);

		//! RUNNING VOLTAGE DROP
		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(24);
		m_wndSpread.SetText(_T("RUNNING \nVOLTAGE DROP"));
		m_wndSpread.AddCellSpan(24, m_wndSpread.SpreadHeader, 3, 1);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(24);
		m_wndSpread.SetText(_T("VD [V]"));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(25);
		m_wndSpread.SetText(_T("VD [%]"));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(26);
		m_wndSpread.SetText(_T("ALLOW \nVD [%]"));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);

		//! STARTING VOLTAGE DROP
		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(27);
		m_wndSpread.SetText(_T("STARTING \nVOLTAGE DROP"));
		m_wndSpread.AddCellSpan(27, m_wndSpread.SpreadHeader, 3, 1);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(27);
		m_wndSpread.SetText(_T("VD [V]"));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(28);
		m_wndSpread.SetText(_T("VD [%]"));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(29);
		m_wndSpread.SetText(_T("ALLOW \nVD [%]"));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);

		//! REMARK
		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(30);
		m_wndSpread.SetText(_T("REMARK"));
		m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);
		m_wndSpread.AddCellSpan(30, m_wndSpread.SpreadHeader, 1, 2);
	}
	catch(...)
	{

		return ERROR_BAD_ENVIRONMENT;
	}

	return ERROR_SUCCESS;
}
/**
	@brief  SPREAD SHEET에 칼럼을 추가한다.
	@author KHS
*/
int CCableSizingResultView::AddColumnItem(const int& nRow, const int& nColumn, const string& rText)
{
	m_wndSpread.SetRow(nRow);
	m_wndSpread.SetCol(nColumn);
	m_wndSpread.SetText(rText.c_str());

	return ERROR_SUCCESS;
}

/**
	@brief	SHEET의 텍스트 정렬을 맞춘다.

	@author	KHS
*/
int CCableSizingResultView::AdjustSheetTextAlign(void)
{
	static const int __COLS__[]={2,4,5,6,7,8,9,10,11,12,13,17,18,19,20,21,22,23,24,25,26,27,28,29};
	const int nColsCount = sizeof(__COLS__) / sizeof(__COLS__[0]);

	const long nMaxRows = m_wndSpread.GetMaxRows();
	for(int i = 1;i <= nMaxRows;++i)
	{
		m_wndSpread.put_RowHeight(i , ROW_HEIGHT);
		m_wndSpread.SetRow(i);

		for(int j = 0;j < nColsCount;++j)
		{
			m_wndSpread.SetCol(__COLS__[j]);
			if(2 == __COLS__[j] || 6 == __COLS__[j] || 17 == __COLS__[j])
				m_wndSpread.SetTypeHAlign(m_wndSpread.TypeHAlignCenter);
			else
				m_wndSpread.SetTypeHAlign(m_wndSpread.TypeHAlignRight);
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	save database.

	@author	BHK
*/
void CCableSizingResultView::OnFileSave()
{
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp) pApp->OnSaveDataBase();
}

static UINT gfnPrintCableSizingResult(LPVOID pVoid)
{
	CSimpleAnimationDlg& anidlg = CSimpleAnimationDlg::GetInstance();
	if(NULL == anidlg.GetSafeHwnd())
	{
		anidlg.DoModal();
	}

	return ERROR_SUCCESS;
}

/**
	@brief	Excel Export

	@author KHS	

	@date 2009-06-17 오전 11:33:39	

	@param	

	@return		
**/
void CCableSizingResultView::OnFilePrint()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const CString rFolderPath = (docData.GetProjectFolderPath() + _T("Cable Sizing Result")).c_str();

	ExportToExcel(rFolderPath);
}

/**
	@brief	Excel Export

	@author BHK

	@date 2009-10-27

	@param	

	@return		
**/
int CCableSizingResultView::ExportToExcel(const CString& rFolderPath)
{
	CWinThread* pThread = AfxBeginThread(gfnPrintCableSizingResult , NULL , THREAD_PRIORITY_NORMAL,0);

	python.CallPyWriteCableSizingResultToExcel(rFolderPath);

	if(NULL != pThread)
	{
		CSimpleAnimationDlg& anidlg = CSimpleAnimationDlg::GetInstance();
		if(anidlg.GetSafeHwnd()) anidlg.SendMessage(WM_COMMAND , IDOK);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK

	@date 2009-12-29

	@param	

	@return		
**/
int CCableSizingResultView::SetBackgroundColor(CFpspread1& wndSpread)
{
	//! 배경 색상 설정.
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const COLORREF ItemIdColor   = docData.GetColorCode(_T("ITEM_ID_BACK_COLOR"));
	const COLORREF FrozenColColor= docData.GetColorCode(_T("LOAD_SUMMARY_TOTAL_BACK_COLOR"));

	const long nMaxRows = wndSpread.GetMaxRows();
	const long nMaxCols = wndSpread.GetMaxCols();
	for(int i = 1;i <= nMaxRows;++i)
	{
		wndSpread.SetRow(i);
		wndSpread.put_RowHeight(i, ROW_HEIGHT);

		wndSpread.SetCol(3);
		wndSpread.SetBackColor(FrozenColColor);
		wndSpread.SetBackColorStyle(wndSpread.BackColorStyleUnderGrid);

		wndSpread.SetCol(4);
		wndSpread.SetBackColor(FrozenColColor);
		wndSpread.SetBackColorStyle(wndSpread.BackColorStyleUnderGrid);

		wndSpread.SetCol(5);
		wndSpread.SetBackColor(FrozenColColor);
		wndSpread.SetBackColorStyle(wndSpread.BackColorStyleUnderGrid);

		wndSpread.SetCol(6);
		wndSpread.SetBackColor(FrozenColColor);
		wndSpread.SetBackColorStyle(wndSpread.BackColorStyleUnderGrid);

		wndSpread.SetCol(14);
		wndSpread.SetBackColor(ItemIdColor);
		wndSpread.SetBackColorStyle(wndSpread.BackColorStyleUnderGrid);

		wndSpread.SetCol(17);
		wndSpread.SetBackColor(ItemIdColor);
		wndSpread.SetBackColorStyle(wndSpread.BackColorStyleUnderGrid);

		wndSpread.SetCol(18);
		wndSpread.SetBackColor(ItemIdColor);
		wndSpread.SetBackColorStyle(wndSpread.BackColorStyleUnderGrid);

		wndSpread.SetCol(25);
		wndSpread.SetBackColor(ItemIdColor);
		wndSpread.SetBackColorStyle(wndSpread.BackColorStyleUnderGrid);

		wndSpread.SetCol(28);
		wndSpread.SetBackColor(ItemIdColor);
		wndSpread.SetBackColorStyle(wndSpread.BackColorStyleUnderGrid);
	}

	return ERROR_SUCCESS;
}
