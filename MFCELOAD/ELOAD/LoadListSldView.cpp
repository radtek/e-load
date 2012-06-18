// LoadListSldView.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "LoadListSldView.h"
#include "ELoadDocData.h"
#include "LoadListSldDoc.h"
#include "pyfromc.h"
#include "SimpleAnimationDlg.h"
#include "CompareRevisionDataDoc.h"
#include <Zip\inc\ZipArchive.h>

// CLoadListSldView

IMPLEMENT_DYNCREATE(CLoadListSldView, CFormView)

CLoadListSldView::CLoadListSldView()
	: CFormView(CLoadListSldView::IDD)
{

}

CLoadListSldView::~CLoadListSldView()
{
}

void CLoadListSldView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FPSPREAD_LOAD_LIST_SLD, m_wndSpread);
}

BEGIN_MESSAGE_MAP(CLoadListSldView, CFormView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_PRINT, &CLoadListSldView::OnFilePrint)
END_MESSAGE_MAP()


// CLoadListSldView diagnostics

#ifdef _DEBUG
void CLoadListSldView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CLoadListSldView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CLoadListSldView message handlers

void CLoadListSldView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if(m_wndSpread.GetSafeHwnd())
	{
		m_wndSpread.SetWindowPos(NULL , 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

int CLoadListSldView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CLoadListSldView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: Add your specialized code here and/or call the base class
	m_wndSpread.SetReDraw(FALSE);
	DisplayLoadListSldData();
	m_wndSpread.SetReDraw(TRUE);
}

/**
	@brief	

	@author KHS	

	@date 2009-11-09 오후 1:15:22	

	@param	

	@return		
	**/
int CLoadListSldView::CreateSheetHeader(void)
{
	try
	{
		//! SET COLUMN WIDTH	
		m_wndSpread.put_ColWidth(1 , 5);
		m_wndSpread.put_ColWidth(2 , 15);
		m_wndSpread.put_ColWidth(3 , 30);
		m_wndSpread.put_ColWidth(8 , 20);
		m_wndSpread.put_ColWidth(9 , 20);
		m_wndSpread.put_ColWidth(10 , 20);
		m_wndSpread.put_ColWidth(11 , 7);
		m_wndSpread.put_RowHeight(m_wndSpread.SpreadHeader +1, 30);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(1);
		m_wndSpread.SetText(_T("NO"));
		m_wndSpread.AddCellSpan(1, m_wndSpread.SpreadHeader, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(2);
		m_wndSpread.SetText(_T("ITEM NO."));
		m_wndSpread.AddCellSpan(2, m_wndSpread.SpreadHeader, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(3);
		m_wndSpread.SetText(_T("ITEM DESCRIPTION"));
		m_wndSpread.AddCellSpan(3, m_wndSpread.SpreadHeader, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(4);
		m_wndSpread.SetText(_T("RATED\nCAPA.\n[KW,\nKVA]"));
		m_wndSpread.AddCellSpan(4, m_wndSpread.SpreadHeader, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(5);
		m_wndSpread.SetText(_T("LOAD\nDUTY"));
		m_wndSpread.AddCellSpan(5, m_wndSpread.SpreadHeader, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(6);
		m_wndSpread.SetText(_T("BAKER"));
		m_wndSpread.AddCellSpan(6, m_wndSpread.SpreadHeader, 2, 1);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(6);
		m_wndSpread.SetText(_T("BKR\nTYPE"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(7);
		m_wndSpread.SetText(_T("FR./TR.\nSIZE [A]"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(8);
		m_wndSpread.SetText(_T("CABLE SIZE"));
		m_wndSpread.AddCellSpan(8, m_wndSpread.SpreadHeader, 3, 1);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(8);
		m_wndSpread.SetText(_T("POWER"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(9);
		m_wndSpread.SetText(_T("CONTROL"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 1);
		m_wndSpread.SetCol(10);
		m_wndSpread.SetText(_T("INST/\nSIGNAL"));

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(11);
		m_wndSpread.SetText(_T("SCHEM.\nTYPE"));
		m_wndSpread.AddCellSpan(11, m_wndSpread.SpreadHeader, 1, 2);

		m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
		m_wndSpread.SetCol(12);
		m_wndSpread.SetText(_T("REMARK"));
		m_wndSpread.AddCellSpan(12, m_wndSpread.SpreadHeader, 1, 2);
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

	@date 2009-11-09 오후 1:52:49	

	@param	

	@return		
**/
int CLoadListSldView::DisplayLoadListSldData(void)
{
	m_wndSpread.SetSheetCount(1);
	m_wndSpread.SetMaxRows(0);

	CELoadDocData& docData = CELoadDocData::GetInstance();

	const COLORREF ItemIdColor = docData.GetColorCode(_T("ITEM_ID_BACK_COLOR"));

	list<CELoadItem*> BusGroupItemList;
	docData.GetELoadItemListOf(BusGroupItemList, CBusGroupItem::TypeString());

	CLoadListSldDoc* pDoc = (CLoadListSldDoc*)GetDocument();
	const string rProjectPath = docData.GetProjectFolderPath();
	
	int nSheet = 1;
	for(list<CELoadItem*>::iterator itr = BusGroupItemList.begin(); itr != BusGroupItemList.end();++itr)
	{
		CBusGroupItem* pBusGroupItem = static_cast<CBusGroupItem*>(*itr);
		if(pBusGroupItem)
		{
			for(vector<CBusItem*>::iterator jtr = pBusGroupItem->m_BusRefEntry.begin(); jtr != pBusGroupItem->m_BusRefEntry.end();++jtr)
			{
				const string rBusID = (*jtr)->GetName();
				CString rFilePath = pDoc->m_rFolderPath + CString(rBusID.c_str()) + _T(".txt");
				if(FileExist(rFilePath))
				{
					m_wndSpread.SetSheetCount(nSheet);
					m_wndSpread.SetSheet(nSheet);
					if(TRUE == m_wndSpread.LoadTextFile(rFilePath , "" , "\t" , "\n" , NULL , ""))
					{
						m_wndSpread.SetColHeaderRows(2);
						m_wndSpread.SetRowHeadersShow(FALSE);
						m_wndSpread.SetMaxCols(12);
						m_wndSpread.SetSheetName(rBusID.c_str());

						CreateSheetHeader();
						AdjustSheetTextAlign();

						const long nMaxRows = m_wndSpread.GetMaxRows();
						const long nMaxCols = m_wndSpread.GetMaxCols();
						for(int i = 1;i <= nMaxRows;++i)
						{
							m_wndSpread.SetRow(i);
							m_wndSpread.SetCol(2);
							m_wndSpread.SetBackColor(ItemIdColor);
							m_wndSpread.SetBackColorStyle(m_wndSpread.BackColorStyleUnderGrid);
						}
						++nSheet;
					}
				}
			}
		}
	}
	return 0;
}

/**
	@brief	

	@author KHS	

	@date 2009-11-09 오후 2:07:24	

	@param	

	@return		
**/
int CLoadListSldView::AdjustSheetTextAlign(void)
{
	static const int __COLS__[]={1,4,5,6,7,8,9,10,11};
	const int nColsCount = sizeof(__COLS__) / sizeof(__COLS__[0]);

	const long nMaxRows = m_wndSpread.GetMaxRows();
	for(int i = 1;i <= nMaxRows;++i)
	{
		m_wndSpread.SetRow(i);

		for(int j = 0;j < nColsCount;++j)
		{
			m_wndSpread.SetCol(__COLS__[j]);
			m_wndSpread.put_RowHeight(i , 13);
			if(4 == __COLS__[j])
				m_wndSpread.SetTypeHAlign(m_wndSpread.TypeHAlignRight);
			else
				m_wndSpread.SetTypeHAlign(m_wndSpread.TypeHAlignCenter);
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	Excel Export

	@author KHS	

	@date 2009-11-09 오후 4:59:08	

	@param	

	@return		
**/
void CLoadListSldView::OnFilePrint()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CString rFolderPath = docData.GetProjectFolderPath().c_str();
	if("\\" != rFolderPath.Right(1)) rFolderPath += _T("\\");
	rFolderPath += _T("Load List Sld Result");
	
	ExportToExcel(rFolderPath);
}

static UINT gfnPrintLoadListSld(LPVOID pVoid)
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

	@date 2009-11-09 오후 5:06:48	

	@param	

	@return		
**/
int CLoadListSldView::ExportToExcel(const CString& rFolderPath)
{
	CWinThread* pThread = AfxBeginThread(gfnPrintLoadListSld , NULL , THREAD_PRIORITY_NORMAL,0);
	
	python.CallPyWriteLoadListSldToExcel(rFolderPath);
	
	if(NULL != pThread)
	{
		CSimpleAnimationDlg& anidlg = CSimpleAnimationDlg::GetInstance();
		if(anidlg.GetSafeHwnd()) anidlg.SendMessage(WM_COMMAND , IDOK);
	}

	return ERROR_SUCCESS;
}

