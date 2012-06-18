// ControlCableResultView.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "ControlCableResultView.h"
#include "CableItem.h"
#include "ControlCableResultDoc.h"
#include "ELoadDocData.h"
#include "pyfromc.h"
#include "SimpleAnimationDlg.h"

// CControlCableResultView

IMPLEMENT_DYNCREATE(CControlCableResultView, CFormView)

CControlCableResultView::CControlCableResultView()
	: CFormView(CControlCableResultView::IDD)
{

}

CControlCableResultView::~CControlCableResultView()
{
}

void CControlCableResultView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FPSPREAD_CONTROLCABLE_RESULT_VIEW, m_wndSpread);
}

BEGIN_MESSAGE_MAP(CControlCableResultView, CFormView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_PRINT, &CControlCableResultView::OnFilePrint)
	ON_COMMAND(ID_FILE_SAVE, &CControlCableResultView::OnFileSave)
END_MESSAGE_MAP()


// CControlCableResultView diagnostics

#ifdef _DEBUG
void CControlCableResultView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CControlCableResultView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CControlCableResultView message handlers

void CControlCableResultView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	BeginWaitCursor();
	m_wndSpread.SetReDraw(FALSE);
	DisplayControlCableResult();
	m_wndSpread.SetReDraw(TRUE);
	EndWaitCursor();
}

int CControlCableResultView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}

void CControlCableResultView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
	if(m_wndSpread.GetSafeHwnd())
	{
		m_wndSpread.SetWindowPos(NULL , 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

static UINT gfnPrintControlCableResult(LPVOID pVoid)
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

	@date 2009-06-09 오후 1:36:47	

	@param	

	@return		
**/
void CControlCableResultView::OnFilePrint()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const CString rFolderPath = (docData.GetProjectFolderPath() + _T("")).c_str();

	ExportToExcel(rFolderPath);
}

/**
	@brief	Excel Export

	@author BHK

	@date 2009-10-27

	@param	

	@return		
**/
int CControlCableResultView::ExportToExcel(const CString& rFolderPath)
{
	CWinThread* pThread = AfxBeginThread(gfnPrintControlCableResult , NULL , THREAD_PRIORITY_NORMAL,0);
	
	python.CallPyWriteControlCableResultToExcel(rFolderPath);
	
	if(NULL != pThread)
	{
		CSimpleAnimationDlg& anidlg = CSimpleAnimationDlg::GetInstance();
		if(anidlg.GetSafeHwnd()) anidlg.SendMessage(WM_COMMAND , IDOK);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	Control cable data 를 Sheet에 뿌린다.

	@author KHS	

	@date 2009-06-09 오후 1:38:50	

	@param	

	@return		
**/
int CControlCableResultView::DisplayControlCableResult(void)
{
	m_wndSpread.SetSheetCount(1);
	m_wndSpread.SetMaxRows(0);
	
	CControlCableResultDoc* pDoc = (CControlCableResultDoc*)GetDocument();
	CELoadDocData& docData = CELoadDocData::GetInstance();

	const string rProjectPath = docData.GetProjectFolderPath();
	CString rFilePath = pDoc->m_rRevFolderPath;

	int nSheet = 1;
	list<CELoadItem*> BusGroupItemList;
	docData.GetELoadItemListOf(BusGroupItemList , CBusGroupItem::TypeString());
	for(list<CELoadItem*>::iterator itr = BusGroupItemList.begin(); itr != BusGroupItemList.end();++itr)
	{
		CBusGroupItem *pBusGroupItem = static_cast<CBusGroupItem*>(*itr);
		if(INCOMING == pBusGroupItem->GetName()) continue;

		for(vector<CBusItem*>::iterator jtr = pBusGroupItem->m_BusRefEntry.begin(); jtr != pBusGroupItem->m_BusRefEntry.end();++jtr)
		{
			const string rBusId = (*jtr)->GetName();

			if(FileExist(CString(rFilePath + rBusId.c_str() + ".BUS")))
			{
				m_wndSpread.SetSheetCount(nSheet);
				m_wndSpread.SetSheet(nSheet);
				if(TRUE == m_wndSpread.LoadTextFile(rFilePath + rBusId.c_str() + ".BUS", "" , "\t" , "\n" , NULL , ""))
				{
					m_wndSpread.SetColHeaderRows(2);
					m_wndSpread.SetRowHeaderCols(1);
					m_wndSpread.SetMaxCols(14);
					m_wndSpread.SetSheetName(rBusId.c_str());

					CreateSheetHeader();
					SetBackgroundColor(m_wndSpread);

					++nSheet;
					AdjustSheetTextAlign();

				}
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	Sheet의 헤더를 생성

	@author KHS	

	@date 2009-06-09 오후 1:38:48	

	@param	

	@return		
**/
int CControlCableResultView::CreateSheetHeader(void)
{
	try
	{
		//! SET COLUMN WIDTH	
		m_wndSpread.put_ColWidth(3 , 17);	
		m_wndSpread.put_ColWidth(8 , 17);
		m_wndSpread.put_ColWidth(9 , 25);
		
		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(1);
		m_wndSpread.SetText(_T("REV"));
		m_wndSpread.AddCellSpan(1, m_wndSpread.SpreadHeader, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(2);
		m_wndSpread.SetText(_T("NO."));
		m_wndSpread.AddCellSpan(2, m_wndSpread.SpreadHeader, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(3);
		m_wndSpread.SetText(_T("LOAD DATA"));
		m_wndSpread.AddCellSpan(3, m_wndSpread.SpreadHeader, 5, 1);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(3);
		m_wndSpread.SetText(_T("ITEM NO."));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(4);
		m_wndSpread.SetText(_T("VOLT \n[KV]"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(5);
		m_wndSpread.SetText(_T("RATING"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(6);
		m_wndSpread.SetText(_T("UNIT"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(7);
		m_wndSpread.SetText(_T("SCHEMATIC TY."));


		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(8);
		m_wndSpread.SetText(_T("CABLE DATA"));
		m_wndSpread.AddCellSpan(8, m_wndSpread.SpreadHeader, 6, 1);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(8);
		m_wndSpread.SetText(_T("CABLE ID"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(9);
		m_wndSpread.SetText(_T("DESCRIPTION"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(10);
		m_wndSpread.SetText(_T("TYPE"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(11);
		m_wndSpread.SetText(_T("CORE"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(12);
		m_wndSpread.SetText(_T("SIZE"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(13);
		m_wndSpread.SetText(_T("LENGTH"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(14);
		m_wndSpread.SetText(_T("REMARK"));
		m_wndSpread.AddCellSpan(14, m_wndSpread.SpreadHeader, 1, 2);
		
	}
	catch(...)
	{
		return ERROR_BAD_ENVIRONMENT;
	}
	return ERROR_SUCCESS;
}

/**
	@brief	SHEET의 텍스트 정렬을 맞춘다.

	@author	KHS
*/
int CControlCableResultView::AdjustSheetTextAlign(void)
{
	static const int __COLS__[]={2,4,5,6,10,11,12,13};
	const int nColsCount = sizeof(__COLS__) / sizeof(__COLS__[0]);

	const long nMaxRows = m_wndSpread.GetMaxRows();
	for(int i = 1;i <= nMaxRows;++i)
	{
		m_wndSpread.put_RowHeight(i , 13);
		m_wndSpread.SetRow(i);

		for(int j = 0;j < nColsCount;++j)
		{
			m_wndSpread.SetCol(__COLS__[j]);
			if(2 == __COLS__[j] || 6 == __COLS__[j] || 11 == __COLS__[j])
				m_wndSpread.SetTypeHAlign(m_wndSpread.TypeHAlignCenter);
			else
				m_wndSpread.SetTypeHAlign(m_wndSpread.TypeHAlignRight);
		}
	}

	return ERROR_SUCCESS;
}
/**
	@brief	SAVE DATA

	@author KHS	

	@date 2009-06-10 오전 10:54:00	

	@param	

	@return		
**/
void CControlCableResultView::OnFileSave()
{
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
        if(pApp) pApp->OnSaveDataBase();
	
}

/**
	@brief	

	@author BHK

	@date 2009-12-29

	@param	

	@return		
**/
int CControlCableResultView::SetBackgroundColor(CFpspread1& wndSpread)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const COLORREF ItemIdColor   = docData.GetColorCode(_T("ITEM_ID_BACK_COLOR"));

	const long nMaxRows = wndSpread.GetMaxRows();
	const long nMaxCols = wndSpread.GetMaxCols();
	for(int i = 1;i <= nMaxRows;++i)
	{
		wndSpread.SetRow(i);
		wndSpread.SetCol(3);
		wndSpread.SetBackColor(ItemIdColor);
		wndSpread.SetBackColorStyle(wndSpread.BackColorStyleUnderGrid);

		wndSpread.SetCol(8);
		wndSpread.SetBackColor(ItemIdColor);
		wndSpread.SetBackColorStyle(wndSpread.BackColorStyleUnderGrid);
	}

	return ERROR_SUCCESS;
}
