// CableRouteCableListView.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "CableRouteCableListView.h"
#include "CableRouteCableListDoc.h"
#include "ELoadDocData.h"
#include "pyfromc.h"
#include "SimpleAnimationDlg.h"

// CCableRouteCableListView

IMPLEMENT_DYNCREATE(CCableRouteCableListView, CFormView)

CCableRouteCableListView::CCableRouteCableListView()
	: CFormView(CCableRouteCableListView::IDD)
{

}

CCableRouteCableListView::~CCableRouteCableListView()
{
}

void CCableRouteCableListView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FPSPREAD_CABLEROUTE_CABLE_LIST_VIEW, m_wndSpread);
}

BEGIN_MESSAGE_MAP(CCableRouteCableListView, CFormView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_PRINT, &CCableRouteCableListView::OnFilePrint)
	ON_COMMAND(ID_FILE_SAVE, &CCableRouteCableListView::OnFileSave)
END_MESSAGE_MAP()


// CCableRouteCableListView diagnostics

#ifdef _DEBUG
void CCableRouteCableListView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CCableRouteCableListView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CCableRouteCableListView message handlers

void CCableRouteCableListView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	m_wndSpread.SetReDraw(FALSE);
	DisplayCableRouteCableList();
	m_wndSpread.SetReDraw(TRUE);

	//ExportToTextFile();


}

/**
	@brief	Spread Sheet에 Cable Route 별 Cable List를 출력한다.

	@author KHS	

	@date 2009-06-01 오후 5:14:02	

	@param	

	@return		
**/
int CCableRouteCableListView::DisplayCableRouteCableList(void)
{
	CCableRouteCableListDoc* pDoc = (CCableRouteCableListDoc*)GetDocument();
	if(pDoc)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		list<CELoadItem*>  RouteItemList;
		docData.GetELoadItemListOf(RouteItemList , CRouteItem::TypeString());

		const COLORREF ItemIdColor   = docData.GetColorCode(_T("ITEM_ID_BACK_COLOR"));
		const string rProjectPath = docData.GetProjectFolderPath();

		const int nSheetCount =  RouteItemList.size();
		m_wndSpread.SetSheetCount(nSheetCount);

		int nSheet =1;
		for(list<CELoadItem*>::iterator itr = RouteItemList.begin(); itr != RouteItemList.end();++itr)
		{
			CRouteItem* pRouteItem = static_cast<CRouteItem*>(*itr);
			if(pRouteItem)	
			{
				const string rRouteName = pRouteItem->GetName();
				CString rPath = rProjectPath.c_str() + CString("Working\\") + CString(rRouteName.c_str()) + CString(".txt");
				const int nRowCount = pRouteItem->m_ControlCableEntry.size() + pRouteItem->m_HV_PowerCableEntry.size() + pRouteItem->m_LV_PowerCableEntry.size();
				m_wndSpread.SetSheet(nSheet);
				if(TRUE == m_wndSpread.LoadTextFile(rPath, "", "\t", "\n", NULL, ""))
				{
					m_wndSpread.SetColHeaderRows(2);
					m_wndSpread.SetRowHeaderCols(1);
					///m_wndSpread.SetRowHeadersShow(FALSE);
					m_wndSpread.SetMaxCols(16);
					m_wndSpread.SetMaxRows(nRowCount);
					m_wndSpread.SetSheetName(rRouteName.c_str());

					CreateSheetHeader();
					
					const long nMaxRows = m_wndSpread.GetMaxRows();
					const long nMaxCols = m_wndSpread.GetMaxCols();
					for(int i = 1;i <= nMaxRows;++i)
					{
						m_wndSpread.SetRow(i);
						m_wndSpread.SetCol(2);
						m_wndSpread.SetBackColor(ItemIdColor);
						m_wndSpread.SetBackColorStyle(m_wndSpread.BackColorStyleUnderGrid);

						m_wndSpread.SetCol(6);
						m_wndSpread.SetBackColor(ItemIdColor);
						m_wndSpread.SetBackColorStyle(m_wndSpread.BackColorStyleUnderGrid);
					}

					++nSheet;
					AdjustSheetTextAlign();
				}
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	SPREAD SHEET의 HEADER를 만든다.

	@author KHS	

	@date 2009-06-01 오후 5:29:27	

	@param	

	@return		
**/
int CCableRouteCableListView::CreateSheetHeader(void)
{
	try
	{
		//! SET COLUMN WIDTH	
		m_wndSpread.put_ColWidth(2 , 17);	
		m_wndSpread.put_ColWidth(6 , 17);
		m_wndSpread.put_ColWidth(7 , 17);
		m_wndSpread.put_ColWidth(8 , 17);
		m_wndSpread.put_ColWidth(9 , 25);
/*		
		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(1);
		m_wndSpread.SetText(_T("REV"));
		m_wndSpread.AddCellSpan(1, m_wndSpread.SpreadHeader, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(1);
		m_wndSpread.SetText(_T("NO."));
		m_wndSpread.AddCellSpan(1, m_wndSpread.SpreadHeader, 1, 2);
*/
		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(0);
		m_wndSpread.AddCellSpan(0, m_wndSpread.SpreadHeader, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(1);
		m_wndSpread.SetText(_T("AREA"));
		m_wndSpread.AddCellSpan(1, m_wndSpread.SpreadHeader, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(2);
		m_wndSpread.SetText(_T("LOAD DATA"));
		m_wndSpread.AddCellSpan(2, m_wndSpread.SpreadHeader, 4, 1);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(2);
		m_wndSpread.SetText(_T("ITEM NO."));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(3);
		m_wndSpread.SetText(_T("VOLT \n[KV]"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(4);
		m_wndSpread.SetText(_T("RATING\n CAPACITY"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(5);
		m_wndSpread.SetText(_T("UNIT"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(6);
		m_wndSpread.SetText(_T("CABLE DATA"));
		m_wndSpread.AddCellSpan(6, m_wndSpread.SpreadHeader, 9, 1);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(6);
		m_wndSpread.SetText(_T("CABLE ID"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(7);
		m_wndSpread.SetText(_T("FROM"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(8);
		m_wndSpread.SetText(_T("TO"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(9);
		m_wndSpread.SetText(_T("DESCRIPTION"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(10);
		m_wndSpread.SetText(_T("SPEC"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(11);
		m_wndSpread.SetText(_T("CORE"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(12);
		m_wndSpread.SetText(_T("SIZE"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(13);
		m_wndSpread.SetText(_T("LENGTH"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(14);
		m_wndSpread.SetText(_T("OD"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(15);
		m_wndSpread.SetText(_T("WEIGHT"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(16);
		m_wndSpread.SetText(_T("REMARK"));
		m_wndSpread.AddCellSpan(16, m_wndSpread.SpreadHeader, 1, 2);
		
	}
	catch(...)
	{
		return ERROR_BAD_ENVIRONMENT;
	}
	return ERROR_SUCCESS;
}

void CCableRouteCableListView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
	if(m_wndSpread.GetSafeHwnd())
	{
		m_wndSpread.SetWindowPos(NULL , 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

int CCableRouteCableListView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}


static UINT gfnPrintRouteCableList(LPVOID pVoid)
{
	CSimpleAnimationDlg& anidlg = CSimpleAnimationDlg::GetInstance();
	if(NULL == anidlg.GetSafeHwnd())
	{
		anidlg.DoModal();
	}

	return ERROR_SUCCESS;
}
/**
	@brief	Cable route Cable List Excel Export

	@author KHS	

	@date 2009-06-05 오후 3:08:40	

	@param	

	@return		
**/
void CCableRouteCableListView::OnFilePrint()
{
	CWinThread* pThread = AfxBeginThread(gfnPrintRouteCableList , NULL , THREAD_PRIORITY_NORMAL,0);
	
	python.CallPyWriteRouoteCableListToExcel();
	
	if(NULL != pThread)
	{
		CSimpleAnimationDlg& anidlg = CSimpleAnimationDlg::GetInstance();
		if(anidlg.GetSafeHwnd()) anidlg.SendMessage(WM_COMMAND , IDOK);
	}
}

/**
	@brief	SAVE DATA

	@author KHS	

	@date 2009-06-10 오전 10:54:22	

	@param	

	@return		
**/
void CCableRouteCableListView::OnFileSave()
{
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
        if(pApp) pApp->OnSaveDataBase();
}

/**
	@brief	

	@author KHS	

	@date 2009-07-09 오후 1:40:58	

	@param	

	@return		
**/
int CCableRouteCableListView::AdjustSheetTextAlign(void)
{
	static const int __COLS__[]={3,4,5,11,12,13,14,15};
	const int nColsCount = sizeof(__COLS__) / sizeof(__COLS__[0]);

	const long nMaxRows = m_wndSpread.GetMaxRows();
	for(int i = 1;i <= nMaxRows;++i)
	{
		m_wndSpread.put_RowHeight(i , 13);
		m_wndSpread.SetRow(i);

		for(int j = 0;j < nColsCount;++j)
		{
			m_wndSpread.SetCol(__COLS__[j]);
			if(5 == __COLS__[j] || 11 == __COLS__[j] )
				m_wndSpread.SetTypeHAlign(m_wndSpread.TypeHAlignCenter);
			else
				m_wndSpread.SetTypeHAlign(m_wndSpread.TypeHAlignRight);
		}
	}

	return ERROR_SUCCESS;
}
