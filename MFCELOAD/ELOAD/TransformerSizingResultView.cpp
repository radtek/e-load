// TransformerSizingResultView.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "TransformerSizingResultDoc.h"
#include "TransformerSizingResultView.h"
#include "ELoadDocData.h"
#include "SimpleAnimationDlg.h"
#include "pyfromc.h"
#include <iostream>
#include <sstream>

using namespace std;

#define ROW_HEIGHT 13
#define ROUND(x) ((int)((x+0.005f)*100.0f))/100.0f //소숫점 2자리로 반올
// CTransformerSizingResultView

IMPLEMENT_DYNCREATE(CTransformerSizingResultView, CFormView)

CTransformerSizingResultView::CTransformerSizingResultView()
	: CFormView(CTransformerSizingResultView::IDD)
{

}

CTransformerSizingResultView::~CTransformerSizingResultView()
{
}

void CTransformerSizingResultView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FPSPREAD_TRANSFORMER_RESULT_VIEW, m_wndSpread);
}

BEGIN_MESSAGE_MAP(CTransformerSizingResultView, CFormView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_SAVE, &CTransformerSizingResultView::OnFileSave)
	ON_COMMAND(ID_FILE_PRINT, &CTransformerSizingResultView::OnFilePrint)
END_MESSAGE_MAP()


// CTransformerSizingResultView diagnostics

#ifdef _DEBUG
void CTransformerSizingResultView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CTransformerSizingResultView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CTransformerSizingResultView message handlers
/**
	@brief	

	@author KHS	

	@date 2009-03-27 오후 7:44:29	

	@param	

	@return		
**/
void CTransformerSizingResultView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	m_wndSpread.SetReDraw(FALSE);
	DisplayTranformerSizingResultData();
	AdjustSheetTextAlign();
	SetSheetIdColor();
	m_wndSpread.SetReDraw(TRUE);
}

/**
	@brief	

	@author KHS	

	@date 2009-03-27 오후 7:43:28	

	@param	

	@return		
**/
void CTransformerSizingResultView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if(m_wndSpread.GetSafeHwnd())
	{
		m_wndSpread.SetWindowPos(NULL , 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

static bool SortByName(CELoadItem* lhs , CELoadItem* rhs)
{
	assert(lhs && rhs && "lhs or rhs is NULL");

	if(lhs && rhs) return (lhs->GetName() < rhs->GetName());

	return false;
}

/**
	@brief  CREATE TRANSFORMER SIZING SHEET

	@author KHS

	@date	????.??.??

	@param

	@return	int
*/
int CTransformerSizingResultView::DisplayTranformerSizingResultData(void)
{	
	m_wndSpread.SetSheetCount(1);

	m_wndSpread.SetSheet(1);
	m_wndSpread.SetMaxRows(0);
	CTransformerSizingResultDoc* pDoc = (CTransformerSizingResultDoc*)GetDocument();
	if(pDoc)
	{
		if(TRUE == m_wndSpread.LoadTextFile(pDoc->m_rFilePath.c_str() , _T("") , _T("\t") , _T("\n") , NULL , _T("")))
		{
			m_wndSpread.SetColHeaderRows(2);
			m_wndSpread.SetRowHeadersShow(FALSE);
			m_wndSpread.SetMaxCols(12);
			
			//! 시트의 해더를 생성
			CreateHeader(m_wndSpread);
		}
	}

	return ERROR_SUCCESS;
}
/**
	@brief	SHEET 정렬

	@author KHS	

	@date 2009-03-30 오전 10:06:54	

	@param	

	@return		
**/
int CTransformerSizingResultView::AdjustSheetTextAlign(void)
{
	static const int __COLS__[]={3,5,6,7,8,9,10,11};
	const int nColsCount = sizeof(__COLS__) / sizeof(__COLS__[0]);

	const long nMaxRows = m_wndSpread.GetMaxRows();
	for(int i = 1;i <= nMaxRows;++i)
	{
		m_wndSpread.put_RowHeight(i , ROW_HEIGHT);
		m_wndSpread.SetRow(i);

		for(int j = 0;j < nColsCount;++j)
		{
			m_wndSpread.SetCol(__COLS__[j]);
			m_wndSpread.SetTypeHAlign(m_wndSpread.TypeHAlignRight);
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief  SHEET의 해더를 생성한다.

	@author KHS
*/
int CTransformerSizingResultView::CreateHeader(CFpspread1& wndSpread)
{
	wndSpread.put_RowHeight(wndSpread.SpreadHeader + 1, 60);
	wndSpread.put_RowHeight(wndSpread.SpreadHeader, ROW_HEIGHT + 2);

	//! SET COLUMN WIDTH
	wndSpread.put_ColWidth(1 , ROW_HEIGHT);	// TR ID
	wndSpread.put_ColWidth(2 , ROW_HEIGHT);	// Pri BUS ID
	wndSpread.put_ColWidth(4 , ROW_HEIGHT);	// Sec Bus ID


	wndSpread.SetRow(wndSpread.SpreadHeader);
	wndSpread.SetCol(1);
	wndSpread.SetText(_T("TR. ID"));
	wndSpread.SetTypeTextOrient(wndSpread.TypeTextOrientUp);
	wndSpread.AddCellSpan(1, wndSpread.SpreadHeader, 1, 2);

	//! BUS CONNECTION
	wndSpread.SetRow(wndSpread.SpreadHeader);
	wndSpread.SetCol(2);
	wndSpread.SetText(_T("BUS CONNECTION"));
	wndSpread.AddCellSpan(2, wndSpread.SpreadHeader, 4, 1);

	wndSpread.SetRow(wndSpread.SpreadHeader + 1);
	wndSpread.SetCol(2);
	wndSpread.SetText(_T("Pri. BUS ID"));
	wndSpread.SetTypeTextOrient(wndSpread.TypeTextOrientUp);

	wndSpread.SetRow(wndSpread.SpreadHeader + 1);
	wndSpread.SetCol(3);
	wndSpread.SetText(_T("BUS VOLT."));
	wndSpread.SetTypeTextOrient(wndSpread.TypeTextOrientUp);

	wndSpread.SetRow(wndSpread.SpreadHeader + 1);
	wndSpread.SetCol(4);
	wndSpread.SetText(_T("Sec. BUS ID"));
	wndSpread.SetTypeTextOrient(wndSpread.TypeTextOrientUp);

	wndSpread.SetRow(wndSpread.SpreadHeader + 1);
	wndSpread.SetCol(5);
	wndSpread.SetText(_T("BUS VOLT."));
	wndSpread.SetTypeTextOrient(wndSpread.TypeTextOrientUp);

	//! LOAD SUMMARY
	wndSpread.SetRow(wndSpread.SpreadHeader);
	wndSpread.SetCol(6);
	wndSpread.SetText(_T("LOAD SUMMARY"));
	wndSpread.AddCellSpan(6, wndSpread.SpreadHeader, 3, 1);

	wndSpread.SetRow(wndSpread.SpreadHeader + 1);
	wndSpread.SetCol(6);
	wndSpread.SetText(_T("KW"));
	wndSpread.SetTypeTextOrient(wndSpread.TypeTextOrientUp);

	wndSpread.SetRow(wndSpread.SpreadHeader + 1);
	wndSpread.SetCol(7);
	wndSpread.SetText(_T("KVAR"));
	wndSpread.SetTypeTextOrient(wndSpread.TypeTextOrientUp);

	wndSpread.SetRow(wndSpread.SpreadHeader + 1);
	wndSpread.SetCol(8);
	wndSpread.SetText(_T("KVA"));
	wndSpread.SetTypeTextOrient(wndSpread.TypeTextOrientUp);

	//! TR. CAPACITY
	wndSpread.SetRow(wndSpread.SpreadHeader);
	wndSpread.SetCol(9);
	wndSpread.SetText(_T("TR. CAPACITY"));
	wndSpread.AddCellSpan(9, wndSpread.SpreadHeader, 3, 1);

	wndSpread.SetRow(wndSpread.SpreadHeader + 1);
	wndSpread.SetCol(9);
	wndSpread.SetText(_T("CALCULATED \nTR. CAPACITY \n[KVA]"));
	wndSpread.SetTypeTextOrient(wndSpread.TypeTextOrientUp);

	wndSpread.SetRow(wndSpread.SpreadHeader + 1);
	wndSpread.SetCol(10);
	wndSpread.SetText(_T("SELECTED \nTR. CAPACITY \n[KVA]"));
	wndSpread.SetTypeTextOrient(wndSpread.TypeTextOrientUp);

	wndSpread.SetRow(wndSpread.SpreadHeader + 1);
	wndSpread.SetCol(11);
	wndSpread.SetText(_T("Selected / Cal. \n[%]"));
	wndSpread.SetTypeTextOrient(wndSpread.TypeTextOrientUp);

	//! REMARK
	wndSpread.SetRow(wndSpread.SpreadHeader);
	wndSpread.SetCol(12);
	wndSpread.SetText(_T("REMARK"));
	wndSpread.SetTypeTextOrient(wndSpread.TypeTextOrientUp);
	wndSpread.AddCellSpan(12, wndSpread.SpreadHeader, 1, 2);

	return ERROR_SUCCESS;
}

/**
	@brief  SPREAD SHEET에 칼럼을 추가한다.

	@author KHS
*/
int CTransformerSizingResultView::AddColumnItem(const int& nRow , const int& nColumn , const string& rText)
{
	m_wndSpread.SetRow(nRow);
	m_wndSpread.SetCol(nColumn);
	m_wndSpread.SetText(rText.c_str());

	return ERROR_SUCCESS;
}

/**
	@brief	BUS ID로 구한 LOAD 의 SUM을 구한다.

	@author KHS	

	@date 2009-03-30 오전 9:11:40	

	@param	

	@return	KW, KVAR, KVA의 SUM
**/
string* CTransformerSizingResultView::GetLoadSummaryDataForBus(const string& rBusId)
{
	CELoadDocData &docData = CELoadDocData::GetInstance();
	
	static string pLoadSum[3] = {"","",""};
	pLoadSum[0] = _T("");
	pLoadSum[1] = _T("");
	pLoadSum[2] = _T("");

	vector<CBusItem*> BusItemEntry;
	docData.GetAllBusItems(BusItemEntry);

	vector<CBusItem*>::iterator where = find_if(BusItemEntry.begin(), BusItemEntry.end(), bind2nd(MatchBusName(), rBusId));
	if(where != BusItemEntry.end())
	{
		const string rContinuous = (*where)->prop()->GetValue(_T("General") , _T("Continuous"));
		const string rIntermittent = (*where)->prop()->GetValue(_T("General") , _T("Intermittent"));
		const string rStandby = (*where)->prop()->GetValue(_T("General") , _T("Stand-by"));

		CItemSelectionSet SelectionSet;
		docData.GetLoadItemSetInBus(SelectionSet , rBusId);

		double nKWTotal = 0.;
		double nKVARTotal = 0.;
		double nKVATotal = 0.;
		for(int i = 0;i < SelectionSet.GetItemCount();++i)
		{
			const string rKW = SelectionSet.GetItemAt(i)->prop()->GetValue("Load Summary" , "KW");
			const string rKVAR = SelectionSet.GetItemAt(i)->prop()->GetValue("Load Summary" , "KVAR");

			const string rOperationMode = SelectionSet.GetItemAt(i)->prop()->GetValue("Type" , "Operation Mode");
			if( "CONTINUOUS" == rOperationMode)
			{
				nKWTotal += atof(rKW.c_str()) * 0.01 * atof(rContinuous.c_str());
				nKVARTotal += atof(rKVAR.c_str()) * 0.01 * atof(rContinuous.c_str());
			}
			else if("INTERMITTENT" == rOperationMode)
			{
				nKWTotal += atof(rKW.c_str()) * 0.01 * atof(rIntermittent.c_str());
				nKVARTotal += atof(rKVAR.c_str()) * 0.01 * atof(rIntermittent.c_str());
			}
			else // STAND BY
			{
				nKWTotal += atof(rKW.c_str()) * 0.01 * atof(rStandby.c_str());
				nKVARTotal += atof(rKVAR.c_str()) * 0.01 * atof(rStandby.c_str());
			}
		}
		nKVATotal += sqrt(nKWTotal * nKWTotal + nKVARTotal * nKVARTotal);

		CString rTemp;
		rTemp.Format("%.2f", ROUND(nKWTotal));
		pLoadSum[0] = rTemp.operator LPCSTR();
		rTemp.Format("%.2f", ROUND(nKVARTotal));
		pLoadSum[1] = rTemp.operator LPCSTR();
		rTemp.Format("%.2f", ROUND(nKVATotal));
		pLoadSum[2] = rTemp.operator LPCSTR();

		return pLoadSum;
	}
	else
	{
		AfxMessageBox("Not Find Bus ID...!!");
	}
	return NULL;
}

/**
	@brief	

	@author KHS	

	@date 2009-03-30 오전 9:11:57	

	@param	

	@return		
**/
int CTransformerSizingResultView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}

/**
	@brief	SAVE DATA

	@author KHS	

	@date 2009-03-30 오전 10:45:07	

	@param	

	@return		
**/
void CTransformerSizingResultView::OnFileSave()
{
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp) pApp->OnSaveDataBase();
}

/**
	@brief	SHEET의 COLOR를 설정한다.

	@author KHS	

	@date 2009-03-30 오전 11:04:14	

	@param	

	@return		
**/
int CTransformerSizingResultView::SetSheetIdColor(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const COLORREF color = docData.GetColorCode("ITEM_ID_BACK_COLOR");
	
	static const int __COLS__[]={1,10};
	const int nColsCount = sizeof(__COLS__) / sizeof(__COLS__[0]);

	const int nColumnCount = m_wndSpread.GetMaxCols();
	const int nRowCount = m_wndSpread.GetMaxRows();
	if(nRowCount >0 )
	{
		for(int nRow = 1; nRow <= nRowCount;nRow++)
		{
			m_wndSpread.SetRow(nRow);
			for(int nCol = 0;nCol < nColsCount;nCol++)
			{
				m_wndSpread.SetCol(__COLS__[nCol]);
				m_wndSpread.SetBackColor(color);
				m_wndSpread.SetBackColorStyle(m_wndSpread.BackColorStyleUnderGrid);
			}
		}
	}
	return ERROR_SUCCESS;
}

/**
	@brief	Sizing 결과를 Excel 파일로 출력한다.

	@author	HumKyung.Baek

	@date	2010.08.04
*/
void CTransformerSizingResultView::OnFilePrint()
{
	CTransformerSizingResultDoc* pDoc = (CTransformerSizingResultDoc*)GetDocument();
	if(pDoc)
	{
		CTransformerSizingResultView::ExportToExcel(pDoc->m_rFilePath.c_str());
	}
}

/**
	@brief	animate until dialog box is alive

	@author	HumKyung.Baek

	@date	2010.08.04
*/
static UINT gfnPrintTransformerSizingResult(LPVOID pVoid)
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

	@author HumKyung.Baek

	@date 2010.08.04

	@param	

	@return		
**/
int CTransformerSizingResultView::ExportToExcel(const CString& rFilePath)
{
	CWinThread* pThread = AfxBeginThread(gfnPrintTransformerSizingResult , NULL , THREAD_PRIORITY_NORMAL,0);

	python.CallPyWriteTransformerSizingResultToExcel(rFilePath);

	if(NULL != pThread)
	{
		CSimpleAnimationDlg& anidlg = CSimpleAnimationDlg::GetInstance();
		if(anidlg.GetSafeHwnd()) anidlg.SendMessage(WM_COMMAND , IDOK);
	}

	return ERROR_SUCCESS;
}