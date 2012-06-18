// DrumScheduleResultView.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "DrumScheduleResultView.h"
#include "DrumScheduleResultDoc.h"
#include "ELoadDocData.h"
#include "pyfromc.h"
#include "SimpleAnimationDlg.h"

// CDrumScheduleResultView

IMPLEMENT_DYNCREATE(CDrumScheduleResultView, CFormView)

CDrumScheduleResultView::CDrumScheduleResultView()
	: CFormView(CDrumScheduleResultView::IDD)
{

}

CDrumScheduleResultView::~CDrumScheduleResultView()
{
}

void CDrumScheduleResultView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FPSPREAD_DRUM_SCHEDULE_RESULT_VIEW, m_wndSpread);
}

BEGIN_MESSAGE_MAP(CDrumScheduleResultView, CFormView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_PRINT, &CDrumScheduleResultView::OnFilePrint)
	ON_COMMAND(ID_FILE_SAVE, &CDrumScheduleResultView::OnFileSave)
END_MESSAGE_MAP()


// CDrumScheduleResultView diagnostics

#ifdef _DEBUG
void CDrumScheduleResultView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CDrumScheduleResultView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CDrumScheduleResultView message handlers

void CDrumScheduleResultView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if(m_wndSpread.GetSafeHwnd())
	{
		m_wndSpread.SetWindowPos(NULL , 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CDrumScheduleResultView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	BeginWaitCursor();
	m_wndSpread.SetReDraw(FALSE);
	DisplayDrumScheduleResult();
	m_wndSpread.SetReDraw(TRUE);
	EndWaitCursor();
}

int CDrumScheduleResultView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}


static UINT gfnPrintDrumScheduleList(LPVOID pVoid)
{
	CSimpleAnimationDlg& anidlg = CSimpleAnimationDlg::GetInstance();
	if(NULL == anidlg.GetSafeHwnd())
	{
		anidlg.DoModal();
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author KHS	

	@date 2009-09-28 오후 3:14:04	

	@param	

	@return		
**/
void CDrumScheduleResultView::OnFilePrint()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const CString rFolderPath = (docData.GetProjectFolderPath() + _T("Drum Schedule Result")).c_str();

	ExportToExcel(rFolderPath);
}

/**
	@brief	

	@author KHS	

	@date 2009-10-27

	@param	

	@return		
**/
int CDrumScheduleResultView::ExportToExcel(const CString& rFolderPath)
{
	CWinThread* pThread = AfxBeginThread(gfnPrintDrumScheduleList , NULL , THREAD_PRIORITY_NORMAL,0);
	
	python.CallPyWriteDrumScheduleListToExcel(rFolderPath);
	
	if(NULL != pThread)
	{
		CSimpleAnimationDlg& anidlg = CSimpleAnimationDlg::GetInstance();
		if(anidlg.GetSafeHwnd()) anidlg.SendMessage(WM_COMMAND , IDOK);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author KHS	

	@date 2009-09-28 오후 3:14:30	

	@param	

	@return		
**/
void CDrumScheduleResultView::OnFileSave()
{
	// TODO: Add your command handler code here
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	pApp->OnSaveDataBase();
}

/**
	@brief	

	@author KHS	

	@date 2009-09-28 오후 3:15:44	

	@param	

	@return		
**/
int CDrumScheduleResultView::DisplayDrumScheduleResult(void)
{
	m_wndSpread.SetMaxRows(0);
	m_wndSpread.SetSheetCount(3);
	
	CELoadDocData& docData = CELoadDocData::GetInstance();

	const COLORREF ItemIdColor   = docData.GetColorCode(_T("ITEM_ID_BACK_COLOR"));

	CDrumScheduleResultDoc* pDoc = (CDrumScheduleResultDoc*)GetDocument();
	const string rProjectPath = docData.GetProjectFolderPath();

	//! Drum Total Cable 
	CString rFilePath = pDoc->m_rFolderPath + CString(_T("Total_Drum_Cable_Result.txt"));
	m_wndSpread.SetSheet(1);
	if(TRUE == m_wndSpread.LoadTextFile(rFilePath , "" , "\t" , "\n" , NULL , ""))
	{
		m_wndSpread.SetSheetName(_T("CABLE-TOTAL"));

		m_wndSpread.SetColHeaderRows(3);
		m_wndSpread.SetRowHeadersShow(FALSE);
		m_wndSpread.SetMaxCols(8);

		CDrumScheduleResultView::CreateCableTotalSheetHeader(m_wndSpread);

		AdjustCableTotalSheetTextAlign();
	}

	//! TOTAL DRUM
	m_wndSpread.SetSheet(2);
	rFilePath = pDoc->m_rFolderPath + CString(_T("Total_DrumScheduleResult.txt"));
	if(TRUE == m_wndSpread.LoadTextFile(rFilePath , "" , "\t" , "\n" , NULL , ""))
	{
		m_wndSpread.SetSheetName(_T("DRUM-TOTAL"));
		m_wndSpread.SetColHeaderRows(3);
		m_wndSpread.SetRowHeadersShow(FALSE);
		m_wndSpread.SetMaxCols(9);

		CDrumScheduleResultView::CreateTotalSheetHeader(m_wndSpread);

		const long nMaxRows = m_wndSpread.GetMaxRows();
		const long nMaxCols = m_wndSpread.GetMaxCols();
		for(int i = 1;i <= nMaxRows;++i)
		{
			m_wndSpread.SetRow(i);
			m_wndSpread.SetCol(3);
			m_wndSpread.SetBackColor(ItemIdColor);
			m_wndSpread.SetBackColorStyle(m_wndSpread.BackColorStyleUnderGrid);
		}

		AdjustTotalSheetTextAlign();
	}

	//! DRUM
	rFilePath = pDoc->m_rFolderPath + CString(_T("DrumScheduleResult.txt"));
	m_wndSpread.SetSheet(3);
	if(TRUE == m_wndSpread.LoadTextFile(rFilePath , "" , "\t" , "\n" , NULL , ""))
	{
		m_wndSpread.SetSheetName(_T("DRUM"));

		m_wndSpread.SetColHeaderRows(3);
		m_wndSpread.SetRowHeadersShow(FALSE);
		m_wndSpread.SetMaxCols(13);

		CDrumScheduleResultView::CreateSheetHeader(m_wndSpread);

		const long nMaxRows = m_wndSpread.GetMaxRows();
		const long nMaxCols = m_wndSpread.GetMaxCols();
		for(int i = 1;i <= nMaxRows;++i)
		{
			m_wndSpread.SetRow(i);
			m_wndSpread.SetCol(2);
			m_wndSpread.SetBackColor(ItemIdColor);
			m_wndSpread.SetBackColorStyle(m_wndSpread.BackColorStyleUnderGrid);
			
			m_wndSpread.SetCol(5);
			m_wndSpread.SetBackColor(ItemIdColor);
			m_wndSpread.SetBackColorStyle(m_wndSpread.BackColorStyleUnderGrid);

			m_wndSpread.SetCol(6);
			m_wndSpread.SetBackColor(ItemIdColor);
			m_wndSpread.SetBackColorStyle(m_wndSpread.BackColorStyleUnderGrid);
		}

		AdjustSheetTextAlign();

		m_wndSpread.SetActiveSheet(1);

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author KHS	

	@date 2009-09-28 오후 3:58:26	

	@param	

	@return		
**/
int CDrumScheduleResultView::CreateSheetHeader(CFpspread1& m_wndSpread)
{
	try
	{
		//! SET COLUMN WIDTH	
		m_wndSpread.put_ColWidth(2 , 17);
		m_wndSpread.put_ColWidth(5 , 15);
		m_wndSpread.put_ColWidth(6 , 15);
		m_wndSpread.put_ColWidth(7 , 17);
		
		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(1);
		m_wndSpread.SetText(_T("Rev\n No"));
		m_wndSpread.AddCellSpan(1, m_wndSpread.SpreadHeader, 1, 3);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(2);
		m_wndSpread.SetText(_T("DRUM NO."));
		m_wndSpread.AddCellSpan(2, m_wndSpread.SpreadHeader, 1, 3);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(3);
		m_wndSpread.SetText(_T("DRUM LENGTH"));
		m_wndSpread.AddCellSpan(3, m_wndSpread.SpreadHeader, 1, 3);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(4);
		m_wndSpread.SetText(_T("AREA"));
		m_wndSpread.AddCellSpan(4, m_wndSpread.SpreadHeader, 1, 3);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(5);
		m_wndSpread.SetText(_T("CABLE NUMBER"));
		m_wndSpread.AddCellSpan(5, m_wndSpread.SpreadHeader, 1, 3);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(6);
		m_wndSpread.SetText(_T("FROM"));
		m_wndSpread.AddCellSpan(6, m_wndSpread.SpreadHeader, 1, 3);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(7);
		m_wndSpread.SetText(_T("TO"));
		m_wndSpread.AddCellSpan(7, m_wndSpread.SpreadHeader, 1, 3);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(8);
		m_wndSpread.SetText(_T("CABLE"));
		m_wndSpread.AddCellSpan(8, m_wndSpread.SpreadHeader, 5, 1);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(8);
		m_wndSpread.SetText(_T("VOLT\n(KV)"));
		m_wndSpread.AddCellSpan(8, m_wndSpread.SpreadHeader + 1, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(9);
		m_wndSpread.SetText(_T("TYPE"));
		m_wndSpread.AddCellSpan(9, m_wndSpread.SpreadHeader + 1, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(10);
		m_wndSpread.SetText(_T("SIZE"));
		m_wndSpread.AddCellSpan(10, m_wndSpread.SpreadHeader + 1, 2, 1);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 2);
		m_wndSpread.SetCol(10);
		m_wndSpread.SetText(_T("[C]"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 2);
		m_wndSpread.SetCol(11);
		m_wndSpread.SetText(_T("[SQMM]"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(12);
		m_wndSpread.SetText(_T("LENGTH\n (M)"));
		m_wndSpread.AddCellSpan(12, m_wndSpread.SpreadHeader + 1, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(13);
		m_wndSpread.SetText(_T("REMARK"));
		m_wndSpread.AddCellSpan(13, m_wndSpread.SpreadHeader, 1, 3);
		
	}
	catch(...)
	{
		return ERROR_BAD_ENVIRONMENT;
	}
	return ERROR_SUCCESS;
}

/**
	@brief	

	@author KHS	

	@date 2009-09-29 오후 2:34:21	

	@param	

	@return		
**/
int CDrumScheduleResultView::AdjustTotalSheetTextAlign(void)
{
	static const int __COLS__[]={2,4,5,7,8};
	const int nColsCount = sizeof(__COLS__) / sizeof(__COLS__[0]);

	const long nMaxRows = m_wndSpread.GetMaxRows();
	for(int i = 1;i <= nMaxRows;++i)
	{
		m_wndSpread.SetRow(i);

		for(int j = 0;j < nColsCount;++j)
		{
			m_wndSpread.SetCol(__COLS__[j]);
			m_wndSpread.put_RowHeight(i , 13);

			if(2 == __COLS__[j] || 7 == __COLS__[j])
				m_wndSpread.SetTypeHAlign(m_wndSpread.TypeHAlignCenter);
			else
				m_wndSpread.SetTypeHAlign(m_wndSpread.TypeHAlignRight);
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	Total Sheet 정렬

	@author KHS	

	@date 2009-11-20 오후 5:10:39	

	@param	

	@return		
**/
int CDrumScheduleResultView::AdjustSheetTextAlign(void)
{
	static const int __COLS__[]={1,8,10,11,12};
	const int nColsCount = sizeof(__COLS__) / sizeof(__COLS__[0]);

	const long nMaxRows = m_wndSpread.GetMaxRows();
	for(int i = 1;i <= nMaxRows;++i)
	{
		m_wndSpread.SetRow(i);

		for(int j = 0;j < nColsCount;++j)
		{
			m_wndSpread.SetCol(__COLS__[j]);
			m_wndSpread.put_RowHeight(i , 13);

			if(10 == __COLS__[j] || 1 == __COLS__[j])
				m_wndSpread.SetTypeHAlign(m_wndSpread.TypeHAlignCenter);
			else
				m_wndSpread.SetTypeHAlign(m_wndSpread.TypeHAlignRight);
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	CABLE TOTAL ALIGN

	@author KHS	

	@date 2009-11-27 오전 9:54:26	

	@param	

	@return		
**/
int CDrumScheduleResultView::AdjustCableTotalSheetTextAlign(void)
{
	static const int __COLS__[]={2,3,4,6,7};
	const int nColsCount = sizeof(__COLS__) / sizeof(__COLS__[0]);

	const long nMaxRows = m_wndSpread.GetMaxRows();
	for(int i = 1;i <= nMaxRows;++i)
	{
		m_wndSpread.SetRow(i);

		for(int j = 0;j < nColsCount;++j)
		{
			m_wndSpread.SetCol(__COLS__[j]);
			m_wndSpread.put_RowHeight(i , 13);

			if(6 == __COLS__[j])
				m_wndSpread.SetTypeHAlign(m_wndSpread.TypeHAlignCenter);
			else
				m_wndSpread.SetTypeHAlign(m_wndSpread.TypeHAlignRight);
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	Total Sheet Header

	@author KHS	

	@date 2009-11-20 오후 5:04:30	

	@param	

	@return		
**/
int CDrumScheduleResultView::CreateTotalSheetHeader(CFpspread1& m_wndSpread)
{
	try
	{
		//! SET COLUMN WIDTH	
		m_wndSpread.put_ColWidth(3 , 17);
		
		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(1);
		m_wndSpread.SetText(_T("REV\n No"));
		m_wndSpread.AddCellSpan(1, m_wndSpread.SpreadHeader, 1, 3);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(2);
		m_wndSpread.SetText(_T("NO."));
		m_wndSpread.AddCellSpan(2, m_wndSpread.SpreadHeader, 1, 3);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(3);
		m_wndSpread.SetText(_T("DRUM NO."));
		m_wndSpread.AddCellSpan(3, m_wndSpread.SpreadHeader, 1, 3);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(4);
		m_wndSpread.SetText(_T("DRUM LENGTH"));
		m_wndSpread.AddCellSpan(4, m_wndSpread.SpreadHeader, 1, 3);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(5);
		m_wndSpread.SetText(_T("CABLE"));
		m_wndSpread.AddCellSpan(5, m_wndSpread.SpreadHeader, 4, 1);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(5);
		m_wndSpread.SetText(_T("VOLT\n(KV)"));
		m_wndSpread.AddCellSpan(5, m_wndSpread.SpreadHeader + 1, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(6);
		m_wndSpread.SetText(_T("TYPE"));
		m_wndSpread.AddCellSpan(6, m_wndSpread.SpreadHeader + 1, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(7);
		m_wndSpread.SetText(_T("SIZE"));
		m_wndSpread.AddCellSpan(7, m_wndSpread.SpreadHeader + 1, 2, 1);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 2);
		m_wndSpread.SetCol(7);
		m_wndSpread.SetText(_T("[C]"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 2);
		m_wndSpread.SetCol(8);
		m_wndSpread.SetText(_T("[SQMM]"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(9);
		m_wndSpread.SetText(_T("REMARK"));
		m_wndSpread.AddCellSpan(9, m_wndSpread.SpreadHeader, 1, 3);
		
	}
	catch(...)
	{
		return ERROR_BAD_ENVIRONMENT;
	}
	return ERROR_SUCCESS;
}

/**
	@brief	CABLE TOTAL SHEET HEADER

	@author KHS	

	@date 2009-11-27 오전 9:51:49	

	@param	

	@return		
**/
int CDrumScheduleResultView::CreateCableTotalSheetHeader(CFpspread1& m_wndSpread)
{
	try
	{
		//! SET COLUMN WIDTH	
		m_wndSpread.put_ColWidth(3 , 13);
		
		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(1);
		m_wndSpread.SetText(_T("REV"));
		m_wndSpread.AddCellSpan(1, m_wndSpread.SpreadHeader, 1, 3);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(2);
		m_wndSpread.SetText(_T("NO"));
		m_wndSpread.AddCellSpan(2, m_wndSpread.SpreadHeader, 1, 3);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(3);
		m_wndSpread.SetText(_T("CABLE SIZE\nTOTAL LENGTH\n[M]"));
		m_wndSpread.AddCellSpan(3, m_wndSpread.SpreadHeader, 1, 3);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(4);
		m_wndSpread.SetText(_T("CABLE"));
		m_wndSpread.AddCellSpan(4, m_wndSpread.SpreadHeader, 4, 1);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(4);
		m_wndSpread.SetText(_T("VOLT\n(KV)"));
		m_wndSpread.AddCellSpan(4, m_wndSpread.SpreadHeader + 1, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(5);
		m_wndSpread.SetText(_T("TYPE"));
		m_wndSpread.AddCellSpan(5, m_wndSpread.SpreadHeader + 1, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(6);
		m_wndSpread.SetText(_T("SIZE"));
		m_wndSpread.AddCellSpan(6, m_wndSpread.SpreadHeader + 1, 2, 1);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 2);
		m_wndSpread.SetCol(6);
		m_wndSpread.SetText(_T("[C]"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 2);
		m_wndSpread.SetCol(7);
		m_wndSpread.SetText(_T("[SQMM]"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(8);
		m_wndSpread.SetText(_T("REMARK"));
		m_wndSpread.AddCellSpan(8, m_wndSpread.SpreadHeader, 1, 3);
		
	}
	catch(...)
	{
		return ERROR_BAD_ENVIRONMENT;
	}
	return ERROR_SUCCESS;
}
