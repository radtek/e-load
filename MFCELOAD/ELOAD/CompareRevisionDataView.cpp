// ViewRevisionView.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include <IsString.h>

#include "TransformerSizingResultView.h"
#include "CableSizingTableView.h"
#include "CompareRevisionDataView.h"
#include "CompareRevisionDataDoc.h"
#include "ELoadDocData.h"

#include "LoadSummaryView.h"
#include "CableSizingResultView.h"
#include "ControlCableResultView.h"
#include "CableScheduleResultView.h"
#include "DrumScheduleResultView.h"

#include "CableScheduleResultView.h"
#include "DrumScheduleResultView.h"

#define	ROW_HEIGHT	13
#define COL_WIDTH       12
// CCompareRevisionDataView

IMPLEMENT_DYNCREATE(CCompareRevisionDataView, CFormView)

CCompareRevisionDataView::CCompareRevisionDataView()
	: CFormView(CCompareRevisionDataView::IDD)
{

}

CCompareRevisionDataView::~CCompareRevisionDataView()
{
}

void CCompareRevisionDataView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FPSPREAD_REVISION , m_wndSpread);
}

BEGIN_MESSAGE_MAP(CCompareRevisionDataView, CFormView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_PRINT, &CCompareRevisionDataView::OnFilePrint)
END_MESSAGE_MAP()


// CCompareRevisionDataView diagnostics

#ifdef _DEBUG
void CCompareRevisionDataView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CCompareRevisionDataView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CCompareRevisionDataView message handlers

/**
	@brief	

	@author BHK	

	@date 2009-05-06 오후 1:29:48	

	@param	

	@return		
*/
int CCompareRevisionDataView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

/**
	@brief	윈도우의 크기가 바뀜에 따라 Spread의 크기도 바꾸도록 한다.

	@author BHK	

	@date 2009-05-06 오후 1:33:25	

	@param	

	@return		
*/
void CCompareRevisionDataView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if(m_wndSpread.GetSafeHwnd())
	{
		m_wndSpread.SetWindowPos(NULL , 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

/**
	@brief	

	@author HumKyung	

	@date 2009-05-06 오후 1:37:05	

	@param	

	@return		
*/
void CCompareRevisionDataView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	CCompareRevisionDataDoc* pDoc = (CCompareRevisionDataDoc*)GetDocument();
	if(pDoc)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		if((_T("LOAD SUMMARY-BUS") == pDoc->m_rProductName) || (_T("LOAD SUMMARY-SUBBUS") == pDoc->m_rProductName))
		{
			OnUpdateBUSRevisionData();
		}
		else if((_T("TRANSFORMER SIZING") == pDoc->m_rProductName))
		{
			OnUpdateTransformerSizingResultRevisionData();
		}
		else if((_T("AUTO POWER CABLE") == pDoc->m_rProductName))
		{
			OnUpdateCableSizingResultRevisionData(_T(".BUS"));
		}
		else if((_T("CABLE SIZING TABLE") == pDoc->m_rProductName))
		{
			OnUpdateCableSizingTableRevisionData();
		}
		else if((_T("AUTO CONTROL CABLE") == pDoc->m_rProductName))
		{
			OnUpdateControlCableResultRevisonData(_T(".BUS"));
		}
		else if((_T("CABLE SCHEDULE") == pDoc->m_rProductName))
		{
			OnUpdateCableScheduleResultRevisonData();
		}
		else if((_T("DRUM SCHEDULE") == pDoc->m_rProductName))
		{
			OnUpdateControlCableResultRevisonData(_T(".TXT"));
		}

		pDoc->SetTitle(_T("COMPARE REVISION"));
	}
}


/**
	@brief	compare two documents

	@author HumKyung	

	@date 2009-05-06 오후 2:14:34

	@param	nKeyIndex index of key
	@param	rFileExt - file extension.

	@return	return ERROR_SUCCESS if success, otherwise return ERROR_BAD_ENVIRONEMTN.
*/
int CCompareRevisionDataView::DisplayCompareResultOfRevisionData(const int& nKeyIndex , const string& rFileExt)
{
	m_wndSpread.SetReDraw(FALSE);
	CCompareRevisionDataDoc* pDoc = (CCompareRevisionDataDoc*)GetDocument();
	if(pDoc)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		CString strFilePath = docData.GetProjectFolderPath().c_str() , rLeftSideRevNo , rRightSideRevNo;
		strFilePath = pDoc->GetLeftSideFolderPath();

		int nSheet = 1;
		{
			//! 비교할 두 REVISION 중 최근 REVISION을 SHEET에 뿌린다.
			for(vector<string>::iterator itr = pDoc->m_LeftSideItemEntry.begin();itr != pDoc->m_LeftSideItemEntry.end();++itr)
			{
				for(int nSheet = 1;nSheet <= m_wndSpread.GetSheetCount();++nSheet)
				{
					m_wndSpread.SetSheet(nSheet);
					if((_T("TOTALBUS") == *itr) || (_T("TOTALSUBBUS") == *itr))
					{
						if(0 == m_wndSpread.GetSheetName().Find(_T("TOTAL"))) break;
					}else	if(itr->c_str() == m_wndSpread.GetSheetName()) break;
				}
				
				//! data sheet를 준비한다.
				map<string,CCompareRevisionDataDoc::CompareRevisionDataSheet*>::iterator where = pDoc->m_CompareRevisionDataSheetMap.find(*itr);
				if(where == pDoc->m_CompareRevisionDataSheetMap.end()) pDoc->m_CompareRevisionDataSheetMap[*itr] = new CCompareRevisionDataDoc::CompareRevisionDataSheet;
				pDoc->m_CompareRevisionDataSheetMap[*itr]->clear();

				strFilePath = pDoc->GetLeftSideFolderPath() + CString(itr->c_str()) + CString(rFileExt.c_str());
				ifstream ifile(strFilePath.operator LPCSTR());
				if(ifile.is_open())
				{
					const long nMaxCols = m_wndSpread.GetMaxCols();
					long nRow = 1;
					vector<string> oResult;
					int nLineCount = 0;
					while(!ifile.eof())
					{
						long nCol = 1;
						string aLine;
						getline(ifile , aLine);

						if(_T("CABLE SIZING TABLE") == pDoc->m_rProductName)
						{
							//! parse CELL data.
							if(-1 == aLine.find(_T("CELL"))) continue;
							CTokenizer<CIsFromString>::Tokenize(oResult , aLine , CIsFromString(","));
							vector<string> oResult2;
							CTokenizer<CIsFromString>::Tokenize(oResult2 , oResult[0].c_str() , CIsFromString("="));
							oResult[0] = oResult2[1];
						}
						else
						{
							CTokenizer<CIsFromString>::Tokenize(oResult , aLine , CIsFromString("\t"));
						}

						//! display a line on spread sheet.
						for(vector<string>::iterator jtr = oResult.begin();(jtr != oResult.end()) && (nCol <= nMaxCols);++jtr)
						{
							m_wndSpread.SetText(nCol , nRow , COleVariant(CString(jtr->c_str())));
							++nCol;
						}
						nLineCount++;
						
						CString rLineCount;
						rLineCount.Format(_T("%d") , nLineCount);
						
						//! 실제적인 데이터를 채운다.
						CCompareRevisionDataDoc::CompareRevisionRowData RevRowData;
						if(_T("CABLE SIZING TABLE") == pDoc->m_rProductName)
						{
							//! cable sizing table's key is line number.
							RevRowData.key  = (int(oResult.size()) > nKeyIndex) ? rLineCount : _T("");
							CompareSafetyVoltage();
							CompareValueOverThousand();
						}else	RevRowData.key  = (int(oResult.size()) > nKeyIndex) ? oResult[nKeyIndex] : _T("");

						RevRowData.cols.insert(RevRowData.cols.begin(), oResult.begin() , oResult.end());
						pDoc->m_CompareRevisionDataSheetMap[*itr]->push_back(RevRowData);
						
						CString rString;
						rString.Format(_T("%d") , int(nRow / 2.) + 1);
						//m_wndSpread.SetText(1, nRow , COleVariant(rLeftSideRevNo));

						++nRow;
						if(ifile.eof()) break;
					}
					ifile.close();
					
					if((0 == pDoc->m_rProductName.CompareNoCase(_T("AUTO CONTROL CABLE")))  || 
						(0 == pDoc->m_rProductName.CompareNoCase(_T("AUTO POWER CABLE"))))
					{
						m_wndSpread.SetMaxRows(nRow - 2);
					}else	m_wndSpread.SetMaxRows(nRow - 1);
				}

				if(0 == pDoc->m_rProductName.CompareNoCase(_T("TRANSFORMER SIZING")))	AdjustTransformerSheetTextAlign();
				else if(0 == pDoc->m_rProductName.CompareNoCase(_T("AUTO CONTROL CABLE")))
				{
					AdjustControlCableSheetTextAlign();
					CControlCableResultView::SetBackgroundColor(m_wndSpread);
				}
				else if(0 == pDoc->m_rProductName.CompareNoCase(_T("CABLE SIZING TABLE")));
				else if(0 == pDoc->m_rProductName.CompareNoCase(_T("AUTO POWER CABLE")))
				{
					AdjustPowerCableSheetTextAlign();
					CCableSizingResultView::SetBackgroundColor(m_wndSpread);
				}
				else	AdjustBusSheetTextAlign();
			}
		}

		{
			int iDeletedBusCount = 0;
			for(vector<string>::iterator itr = pDoc->m_RightSideItemEntry.begin();itr != pDoc->m_RightSideItemEntry.end();++itr)
			{
				for(int nSheet = 1;nSheet <= m_wndSpread.GetSheetCount();++nSheet)
				{
					m_wndSpread.SetSheet(nSheet);
					if((_T("TOTALBUS") == *itr) || (_T("TOTALSUBBUS") == *itr))
					{
						if(0 == m_wndSpread.GetSheetName().Find(_T("TOTAL"))) break;
					}else	if(itr->c_str() == m_wndSpread.GetSheetName()) break;
				}
				
				map<string,CCompareRevisionDataDoc::CompareRevisionDataSheet*>::iterator where = pDoc->m_CompareRevisionDataSheetMap.find(*itr);
				if(where == pDoc->m_CompareRevisionDataSheetMap.end())
				{
					++iDeletedBusCount;
					continue;
				}

				int nMaxRows = m_wndSpread.GetMaxRows();
				
				BOOL bIdentical = TRUE;
				strFilePath = pDoc->GetRightSideFolderPath() + CString(itr->c_str()) + CString(rFileExt.c_str());
				ifstream ifile(strFilePath.operator LPCSTR());
				if(ifile.is_open())
				{
					vector<CCompareRevisionDataDoc::CompareRevisionRowData> DeletedItemEntry;
					vector<int> MatchingRowEntry;

					const long nMaxCols = m_wndSpread.GetMaxCols();
					vector<string> oResult;
					int nLineCount = 0;
					while(!ifile.eof())
					{
						long nCol = 1;
						string aLine;
						getline(ifile , aLine);

						if(0 == pDoc->m_rProductName.CompareNoCase(_T("CABLE SIZING TABLE")))
						{
							if(-1 == aLine.find(_T("CELL"))) continue;
							CTokenizer<CIsFromString>::Tokenize(oResult , aLine , CIsFromString(","));
							vector<string> oResult2;
							CTokenizer<CIsFromString>::Tokenize(oResult2 , oResult[0].c_str() , CIsFromString("="));
							oResult[0] = oResult2[1];
						}
						//! 2011.02.14 added by HumKyung
						else if(0 == pDoc->m_rProductName.CompareNoCase(_T("TRANSFORMER SIZING")))
						{
							string::size_type at = aLine.find(_T("DEL "));
							if(0 == at) continue;
						}
						//!
						else
						{
							CTokenizer<CIsFromString>::Tokenize(oResult , aLine , CIsFromString("\t"));
						}
						nLineCount++;
						
						CString rLineCount;
						rLineCount.Format(_T("%d") , nLineCount);

						CCompareRevisionDataDoc::CompareRevisionRowData RevRowData;
						//! set compare key.
						if(0 == pDoc->m_rProductName.CompareNoCase(_T("CABLE SIZING TABLE")))
							RevRowData.key  = (int(oResult.size()) > nKeyIndex) ? rLineCount : _T("");
						else	RevRowData.key  = (int(oResult.size()) > nKeyIndex) ? oResult[nKeyIndex] : _T("");
						RevRowData.cols.insert(RevRowData.cols.begin() , oResult.begin() , oResult.end());
						
						//! compare two lines.
						vector<int> CompareResult;
						const int nRow = pDoc->CompareRevisionData(CompareResult , *(where->second) , RevRowData);
						if(-1 != nRow)
						{
							for(vector<int>::iterator jtr = CompareResult.begin();(jtr != CompareResult.end()) && (nCol <= nMaxCols);++jtr)
							{
								m_wndSpread.SetCol(nCol);
								m_wndSpread.SetRow(nRow);
								//! 값을 비교해서 다르면 붉은색 처리한다.
								if(0 != (*jtr)) m_wndSpread.SetBackColor(RGB(255,0,0));
								m_wndSpread.SetBackColorStyle(m_wndSpread.BackColorStyleUnderGrid);
								++nCol;
								if(0 != (*jtr)) bIdentical = FALSE;
							}
							MatchingRowEntry.push_back(nRow);
						}
						else
						{
							bIdentical = FALSE;
							DeletedItemEntry.push_back(RevRowData);	//! This item is deleted.
						}

						if(ifile.eof()) break;
					}
					ifile.close();

					//! display added item with red color.
					for(int i = 1;i <= m_wndSpread.GetMaxRows();++i)
					{
						vector<int>::iterator where = find(MatchingRowEntry.begin() , MatchingRowEntry.end() , i);
						if((where == MatchingRowEntry.end()) && !pDoc->m_CompareRevisionDataSheetMap[*itr]->empty())
						{
							if(!pDoc->m_CompareRevisionDataSheetMap[*itr]->at(i - 1).key.empty())
							{
								bIdentical = FALSE;
								for(int j = 1;j < m_wndSpread.GetMaxCols();++j)
								{
									m_wndSpread.SetCol(j);
									m_wndSpread.SetRow(i);
									m_wndSpread.SetBackColor(RGB(255,0,0));
									m_wndSpread.SetBackColorStyle(m_wndSpread.BackColorStyleUnderGrid);
								}
							}
						}
					}

					//! display deleted item.
					long nMaxRows = m_wndSpread.GetMaxRows();
					m_wndSpread.SetMaxRows(m_wndSpread.GetMaxRows() + DeletedItemEntry.size());
					for(vector<CCompareRevisionDataDoc::CompareRevisionRowData>::iterator itr = DeletedItemEntry.begin();itr != DeletedItemEntry.end();++itr)
					{
						long nCol = 1;
						for(vector<string>::iterator jtr = itr->cols.begin();jtr != itr->cols.end();++jtr)
						{
							m_wndSpread.SetCol(nCol);
							m_wndSpread.SetRow(nMaxRows + 1);
							m_wndSpread.SetBackColor(RGB(255,0,0));
							m_wndSpread.SetText(nCol++ , nMaxRows + 1 , COleVariant(CString(jtr->c_str())));
							m_wndSpread.put_RowHeight(nMaxRows , ROW_HEIGHT);
						}
						m_wndSpread.SetText(0 , ++nMaxRows , COleVariant(CString(_T("-"))));
					}
				}
				//! this document is modified. so add '*' at end of file name
				if(FALSE == bIdentical)
				{
					CString rSheetName = m_wndSpread.GetSheetName();
					m_wndSpread.SetSheetName(rSheetName + _T("*"));
				}
			}

			//! delete sheets which are associated delete bus - 2011.01.27 added by HumKyung
			if(iDeletedBusCount > 0) m_wndSpread.DeleteSheets(m_wndSpread.GetSheetCount() + 1 - iDeletedBusCount, iDeletedBusCount);

		}

		m_wndSpread.SetReDraw(TRUE);
		m_wndSpread.SetActiveSheet(1);

		return ERROR_SUCCESS;
	}
	m_wndSpread.SetReDraw(TRUE);

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author KHS	

	@date 2009-08-24 오후 1:44:27	

	@param	

	@return		
**/
int CCompareRevisionDataView::CompareSafetyVoltage(void)
{
	int nRowCount = m_wndSpread.GetMaxRows();
	int nColCount = m_wndSpread.GetMaxCols();

	for(int r = 0; r < nRowCount;r++)
	{
		for(int c = 0; c < nColCount;c++)
		{
			m_wndSpread.SetCol(m_wndSpread.SpreadHeader + 1);
			m_wndSpread.SetRow(r + 1);
			CString rFlc = m_wndSpread.GetText();

			m_wndSpread.SetCol(c + 1);
			m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 3);
			CString rSafety = m_wndSpread.GetText();

			if(atof(rFlc) > atof(rSafety))
			{
				m_wndSpread.SetCol(c + 1);
				m_wndSpread.SetRow(r + 1);
				m_wndSpread.SetText(" ");
			}

		}
	}

	return ERROR_SUCCESS;
}


/**
	@brief	

	@author KHS	

	@date 2009-08-24 오후 1:42:59	

	@param	

	@return		
**/
int CCompareRevisionDataView::CompareValueOverThousand(void)
{
	int nRowCount = m_wndSpread.GetMaxRows();
	int nColCount = m_wndSpread.GetMaxCols();

	for(int r = 0; r < nRowCount;r++)
	{
		for(int c = 0; c < nColCount;c++)
		{
			m_wndSpread.SetCol(c + 1);
			m_wndSpread.SetRow(r + 1);
			CString rValue = m_wndSpread.GetText();

			if(atof(rValue) > 1000)
			{
				m_wndSpread.SetCol(c + 1);
				m_wndSpread.SetRow(r + 1);
				m_wndSpread.SetText(" ");
			}
		}
	}

	return ERROR_SUCCESS;
}


/**
	@brief	

	@author BHK	

	@date 2009-05-06 오후 3:38:49	

	@param	

	@return		
*/
int CCompareRevisionDataView::CreateTotalBusSheetHeader(void)
{
	m_wndSpread.SetRowHeaderCols(1);
	/*m_wndSpread.SetColHeaderRows(3);
	m_wndSpread.SetMaxCols(14);*/
	//m_wndSpread.put_RowHeight(m_wndSpread.SpreadHeader + 1, 40);
	
	m_wndSpread.SetColHeaderRows(3);
	m_wndSpread.SetMaxCols(17);
	m_wndSpread.put_RowHeight(m_wndSpread.SpreadHeader + 1, 40);
	m_wndSpread.put_ColWidth(1 , 15);	// BUS ID
	m_wndSpread.put_ColWidth(2 , 25);	// BUS ID

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
	m_wndSpread.SetCol(0);
	m_wndSpread.SetText(_T("Rev. \nNo."));
	m_wndSpread.AddCellSpan(0, 0, 1, 3);

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

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-06 오후 3:18:05	

	@param	

	@return		
*/
void CCompareRevisionDataView::CreateBusSheetHeader(void)
{
	m_wndSpread.SetRowHeaderCols(1);
	m_wndSpread.SetColHeaderRows(4);
	m_wndSpread.SetMaxCols(18);

	m_wndSpread.put_RowHeight(m_wndSpread.SpreadHeader + 1, 20);
	m_wndSpread.put_RowHeight(m_wndSpread.SpreadHeader + 2, 30);

	m_wndSpread.put_ColWidth(1 , 15);	// BUS ID
	m_wndSpread.put_ColWidth(2 , 25);	// BUS ID

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
	m_wndSpread.SetCol(0);
	m_wndSpread.SetText(_T("Rev. \nNo."));
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
	@brief	

	@author BHK	

	@date 2009-05-06 오후 3:24:23	

	@param	

	@return	ERROR_SUCCSS or ERROR_BAD_ENVIRONMENT
*/
int CCompareRevisionDataView::OnUpdateBUSRevisionData(void)
{
	CCompareRevisionDataDoc* pDoc = (CCompareRevisionDataDoc*)GetDocument();
	if(pDoc)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		{
			CString strDir = pDoc->GetLeftSideFolderPath() + _T("*.BUS") , rRevNo;
			if(-1 != pDoc->m_nLeftSideRevNo)
			{
				rRevNo.Format(_T("%d") , pDoc->m_nLeftSideRevNo);
				pDoc->m_LeftSideItemEntry.clear();

				CFileFind file;
				///! 확장자 *.bus 파일을 찾는다.
				BOOL bFound = file.FindFile(strDir);
				if(bFound)
				{
					while(bFound)
					{
						bFound = file.FindNextFile();
						if(file.IsDots()) continue;

						const CString rFileName = file.GetFileTitle().MakeUpper();
						pDoc->m_LeftSideItemEntry.push_back(rFileName.operator LPCSTR());
					}
				}
				else
				{
					return ERROR_BAD_ENVIRONMENT;
				}
			}
			else
			{
				///strDir = pDoc->GetLeftSideFolderPath();
				pDoc->m_LeftSideItemEntry.clear();

				CFileFind file;
				///! 확장자 *.bus 파일을 찾는다.
				BOOL bFound = file.FindFile(strDir);
				if(bFound)
				{
					while(bFound)
					{
						bFound = file.FindNextFile();
						if(file.IsDots()) continue;

						const CString rFileName = file.GetFileTitle().MakeUpper();
						pDoc->m_LeftSideItemEntry.push_back(rFileName.operator LPCSTR());
					}
				}
				else
				{
					return ERROR_BAD_ENVIRONMENT;
				}
			}
		}

		{
			CString strDir = docData.GetProjectFolderPath().c_str() , rRevNo;
			rRevNo.Format(_T("%d") , pDoc->m_nRightSideRevNo);
			strDir = CCompareRevisionDataDoc::GetRevPath(pDoc->m_rProductName , rRevNo) + _T("\\*.BUS");
			
			pDoc->m_RightSideItemEntry.clear();

			CFileFind file;
			///! 확장자 *.bus 파일을 찾는다.
			BOOL bFound = file.FindFile(strDir);
			if(bFound)
			{
				while(bFound)
				{
					bFound = file.FindNextFile();
					if(file.IsDots()) continue;

					const CString rFileName = file.GetFileTitle().MakeUpper();
					pDoc->m_RightSideItemEntry.push_back(rFileName.operator LPCSTR());
				}
			}
			else
			{
				return ERROR_BAD_ENVIRONMENT;
			}
		}
		//! delete all sheets already created.
		m_wndSpread.DeleteSheets(1, m_wndSpread.GetSheetCount());

		set<string> CompareItemSet;
		{
			for(vector<string>::iterator itr = pDoc->m_LeftSideItemEntry.begin();itr != pDoc->m_LeftSideItemEntry.end();++itr)
			{
				CompareItemSet.insert(*itr);
			}
		}
		{
			for(vector<string>::iterator itr = pDoc->m_RightSideItemEntry.begin();itr != pDoc->m_RightSideItemEntry.end();++itr)
			{
				CompareItemSet.insert(*itr);
			}
		}
		
		int nSheet = 1;
		m_wndSpread.SetSheetCount(CompareItemSet.size());
		m_wndSpread.SetSheet(nSheet++);
		m_wndSpread.SetSheetName(_T("TOTAL"));
		CreateTotalBusSheetHeader();
		
		//! create tab according to bus order.
		vector<CBusItem*> BusItemEntry;
		docData.GetAllBusItems(BusItemEntry);
		for(vector<CBusItem*>::iterator itr = BusItemEntry.begin();itr != BusItemEntry.end();++itr)
		{
			string sName((*itr)->GetName());
			IsString::ToUpper(sName);
			set<string>::iterator where = CompareItemSet.find(sName);
			if(where != CompareItemSet.end())
			{
				CString sString(sName.c_str());
				///rString = rString.MakeUpper();
				if(0 == sString.Find(_T("TOTAL"))) continue;
				m_wndSpread.SetSheet(nSheet++);
				m_wndSpread.SetSheetName(sString);
				CreateBusSheetHeader();
				CompareItemSet.erase(where);
			}
		}

		for(set<string>::iterator itr = CompareItemSet.begin();itr != CompareItemSet.end();++itr)
		{
			CString rString(itr->c_str());
			rString = rString.MakeUpper();
			if(0 == rString.Find(_T("TOTAL"))) continue;
			m_wndSpread.SetSheet(nSheet++);
			m_wndSpread.SetSheetName(itr->c_str());
			CreateBusSheetHeader();
		}
		//!

		DisplayCompareResultOfRevisionData(0 , _T(".BUS"));
		AdjustTotalSheetTextAlign();

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	compare transformer data

	@author BHK	

	@date 2009-05-07 오후 5:33:54	

	@param	

	@return	int
*/
int CCompareRevisionDataView::OnUpdateTransformerSizingResultRevisionData(void)
{
	CCompareRevisionDataDoc* pDoc = (CCompareRevisionDataDoc*)GetDocument();
	if(pDoc)
	{
		set<string> CompareItemSet;
		CollectItemsForComparing(CompareItemSet , _T(".TXT"));

		m_wndSpread.DeleteSheets(1, m_wndSpread.GetSheetCount());

		int nSheet = 1;
		m_wndSpread.SetSheetCount(CompareItemSet.size());
		m_wndSpread.SetSheet(nSheet++);
		
		m_wndSpread.SetColHeaderRows(2);
		m_wndSpread.SetRowHeadersShow(FALSE);
		m_wndSpread.SetMaxCols(12);
		CTransformerSizingResultView::CreateHeader(m_wndSpread);
		
		DisplayCompareResultOfRevisionData(0 , _T(".TXT"));

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-07 오후 5:05:55

	@param	

	@return		
*/
int CCompareRevisionDataView::CreateCableSizingResultHeader(void)
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
	@brief	

	@author BHK	

	@date 2009-05-07 오후 5:03:31	

	@param	

	@return		
*/
int CCompareRevisionDataView::OnUpdateCableSizingResultRevisionData(const CString& rExtension)
{
	CCompareRevisionDataDoc* pDoc = (CCompareRevisionDataDoc*)GetDocument();
	if(pDoc)
	{
		set<string> CompareItemSet;
		CollectItemsForComparing(CompareItemSet , rExtension);

		m_wndSpread.DeleteSheets(1, m_wndSpread.GetSheetCount());

		int nSheet = 1;
		m_wndSpread.SetSheetCount(CompareItemSet.size());
		for(set<string>::iterator itr = CompareItemSet.begin();itr != CompareItemSet.end();++itr)
		{
			CString rString(itr->c_str());
			rString = rString.MakeUpper();
			m_wndSpread.SetSheet(nSheet++);
			m_wndSpread.SetSheetName(itr->c_str());
			m_wndSpread.SetRowHeaderCols(1);
			m_wndSpread.SetColHeaderRows(2);
			m_wndSpread.SetRowHeadersShow(TRUE);
			m_wndSpread.SetMaxCols(30);
			m_wndSpread.SetColsFrozen(6);	
			
			CreateCableSizingResultHeader();
		}
		
		//! key index is 13.
		DisplayCompareResultOfRevisionData(13 , rExtension.operator LPCSTR());

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-07 오후 5:40:25	

	@param	

	@return		
*/
int CCompareRevisionDataView::OnUpdateCableSizingTableRevisionData(void)
{
	CCompareRevisionDataDoc* pDoc = (CCompareRevisionDataDoc*)GetDocument();
	if(pDoc)
	{
		set<string> CompareItemSet;
		CollectItemsForComparing(CompareItemSet , _T(".TXT"));
		
		m_wndSpread.SetSheetCount(CompareItemSet.size());

		int nSheet = 1;
		for(set<string>::iterator itr = CompareItemSet.begin();itr != CompareItemSet.end();++itr,++nSheet)
		{
			CString rRevNo;
			rRevNo.Format("%d" , pDoc->m_nLeftSideRevNo);
			CString strDir = CCompareRevisionDataDoc::GetRevPath(pDoc->m_rProductName , rRevNo) + CString(_T("\\")) + itr->c_str() + _T(".TXT");

			m_wndSpread.SetSheet(nSheet);
			m_wndSpread.SetSheetName(itr->c_str());

			m_wndSpread.SetColHeaderRows(9);
			m_wndSpread.SetRowHeaderCols(3);
			m_wndSpread.SetEditMode(FALSE);
			m_wndSpread.SetTypeHAlign(m_wndSpread.TypeHAlignRight);
			CCableSizingTableView::CreateHeaderAndFillData(m_wndSpread , strDir.operator LPCSTR());
		}
		
		DisplayCompareResultOfRevisionData(0, _T(".TXT"));

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-06 오후 3:44:54	

	@param	

	@return		
*/
int CCompareRevisionDataView::AdjustTotalSheetTextAlign(void)
{
	static const int COLS[]={3,4,5,6,7,8,9,10,11,12,13,14};
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
			m_wndSpread.SetTypeHAlign(m_wndSpread.TypeHAlignRight);
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author KHS	

	@date 2009-08-25 오전 10:41:33	

	@param	

	@return		
**/
int CCompareRevisionDataView::AdjustPowerCableSheetTextAlign(void)
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
	@brief	

	@author BHK	

	@date 2009-05-06 오후 3:46:03	

	@param	

	@return		
*/
int CCompareRevisionDataView::AdjustBusSheetTextAlign(void)
{
	static const int __COLS__[]={2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17};
	const int nColsCount = sizeof(__COLS__) / sizeof(__COLS__[0]);

	const long nMaxRows = m_wndSpread.GetMaxRows();
	for(int i = 1;i <= nMaxRows;++i)
	{
		m_wndSpread.put_RowHeight(i , ROW_HEIGHT);

		m_wndSpread.SetRow(i);
		m_wndSpread.SetCol(1);
		m_wndSpread.SetTypeHAlign(m_wndSpread.TypeHAlignCenter);

		for(int j = 0;j < nColsCount;++j)
		{
			m_wndSpread.SetCol(__COLS__[j]);
			///! UNIT , OP, LT, POLE은 중앙 정렬
			if((3 == __COLS__[j]) || (5 == __COLS__[j]) || (6 == __COLS__[j]) || (7 == __COLS__[j]) || (8 == __COLS__[j])) 
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
	@brief	

	@author KHS	

	@date 2009-08-21 오전 10:47:05	

	@param	

	@return		
**/
int CCompareRevisionDataView::AdjustControlCableSheetTextAlign(void)
{
	static const int __COLS__[]={1,2,4,5,6,10,11,12,13};
	const int nColsCount = sizeof(__COLS__) / sizeof(__COLS__[0]);

	const long nMaxRows = m_wndSpread.GetMaxRows();
	for(int i = 1;i <= nMaxRows;++i)
	{
		m_wndSpread.put_RowHeight(i , 13);
		m_wndSpread.SetRow(i);

		for(int j = 0;j < nColsCount;++j)
		{
			m_wndSpread.SetCol(__COLS__[j]);
			if(1 == __COLS__[j] || 2 == __COLS__[j] || 6 == __COLS__[j] || 11 == __COLS__[j])
				m_wndSpread.SetTypeHAlign(m_wndSpread.TypeHAlignCenter);
			else
				m_wndSpread.SetTypeHAlign(m_wndSpread.TypeHAlignRight);
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author KHS	

	@date 2009-08-21 오전 10:58:02	

	@param	

	@return		
**/
int CCompareRevisionDataView::AdjustTransformerSheetTextAlign(void)
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
	@brief	Control Cable의  Revision Data를 Update 한다.

	@author KHS	

	@date 2009-08-19 오후 1:50:40	

	@param	

	@return		
**/
int CCompareRevisionDataView::OnUpdateControlCableResultRevisonData(const CString& rExtension)
{
	CCompareRevisionDataDoc* pDoc = (CCompareRevisionDataDoc*)GetDocument();
	if(pDoc)
	{
		set<string> CompareItemSet;
		CollectItemsForComparing(CompareItemSet , rExtension);

		m_wndSpread.DeleteSheets(1, m_wndSpread.GetSheetCount());

		int nSheet = 1;
		m_wndSpread.SetSheetCount(CompareItemSet.size());
		for(set<string>::iterator itr = CompareItemSet.begin();itr != CompareItemSet.end();++itr)
		{
			CString rString(itr->c_str());
			rString = rString.MakeUpper();
			m_wndSpread.SetSheet(nSheet++);
			m_wndSpread.SetSheetName(rString);
			m_wndSpread.SetColHeaderRows(2);
			m_wndSpread.SetRowHeaderCols(1);
			m_wndSpread.SetMaxCols(14);
			
			CreateControlCableResultHeader();
		}
		
		DisplayCompareResultOfRevisionData(6 , rExtension.operator LPCSTR());

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	CABLE SCHEDULE의  Revision Data를 Update 한다.

	@author BHK

	@date 2009-10-27 오후 1:50:40	

	@param	

	@return		
**/
int CCompareRevisionDataView::OnUpdateCableScheduleResultRevisonData(void)
{
	CCompareRevisionDataDoc* pDoc = (CCompareRevisionDataDoc*)GetDocument();
	if(pDoc)
	{
		set<string> CompareItemSet;
		CollectItemsForComparing(CompareItemSet , _T(".TXT"));

		m_wndSpread.DeleteSheets(1, m_wndSpread.GetSheetCount());

		int nSheet = 1;
		m_wndSpread.SetSheetCount(CompareItemSet.size());
		for(set<string>::iterator itr = CompareItemSet.begin();itr != CompareItemSet.end();++itr)
		{
			CString rString(itr->c_str());
			rString = rString.MakeUpper();
			m_wndSpread.SetSheet(nSheet++);
			m_wndSpread.SetSheetName(rString);
			
			m_wndSpread.SetColHeaderRows(2);
			m_wndSpread.SetRowHeadersShow(FALSE);
			m_wndSpread.SetMaxCols(16);
			
			CCableScheduleResultView::CreateSheetHeader(m_wndSpread);
		}
		
		DisplayCompareResultOfRevisionData(6 , _T(".TXT"));

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	DRUM SCHEDULE의  Revision Data를 Update 한다.

	@author BHK

	@date 2009-10-27 오후 1:50:40	

	@param	

	@return		
**/
int CCompareRevisionDataView::OnUpdateDrumScheduleResultRevisonData(void)
{
	CCompareRevisionDataDoc* pDoc = (CCompareRevisionDataDoc*)GetDocument();
	if(pDoc)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		{
			CString strDir = docData.GetProjectFolderPath().c_str() , rRevNo;
			rRevNo.Format("%d" , pDoc->m_nLeftSideRevNo);
			strDir = CCompareRevisionDataDoc::GetRevPath(pDoc->m_rProductName , rRevNo) + _T("\\*.TXT");

			pDoc->m_LeftSideItemEntry.clear();

			CFileFind file;
			///! 확장자 *.TXT인 파일을 찾는다.
			BOOL bFound = file.FindFile(strDir);
			if(bFound)
			{
				while(bFound)
				{
					bFound = file.FindNextFile();
					if(file.IsDots()) continue;

					const CString rFileName = file.GetFileTitle().MakeUpper();
					pDoc->m_LeftSideItemEntry.push_back(rFileName.operator LPCSTR());
				}
			}
			else
			{
				return ERROR_BAD_ENVIRONMENT;
			}
		}

		{
			CString strDir = docData.GetProjectFolderPath().c_str() , rRevNo;
			rRevNo.Format("%d" , pDoc->m_nRightSideRevNo);
			strDir = CCompareRevisionDataDoc::GetRevPath(pDoc->m_rProductName , rRevNo) + _T("\\*.TXT");

			pDoc->m_RightSideItemEntry.clear();

			CFileFind file;
			///! 확장자 *.TXT인 파일을 찾는다.
			BOOL bFound = file.FindFile(strDir);
			if(bFound)
			{
				while(bFound)
				{
					bFound = file.FindNextFile();
					if(file.IsDots()) continue;

					const CString rFileName = file.GetFileTitle().MakeUpper();
					pDoc->m_RightSideItemEntry.push_back(rFileName.operator LPCSTR());
				}
			}
			else
			{
				return ERROR_BAD_ENVIRONMENT;
			}
		}

		set<string> CompareItemSet;
		{
			for(vector<string>::iterator itr = pDoc->m_LeftSideItemEntry.begin();itr != pDoc->m_LeftSideItemEntry.end();++itr)
			{
				CompareItemSet.insert(*itr);
			}
		}
		{
			for(vector<string>::iterator itr = pDoc->m_RightSideItemEntry.begin();itr != pDoc->m_RightSideItemEntry.end();++itr)
			{
				CompareItemSet.insert(*itr);
			}
		}

		m_wndSpread.DeleteSheets(1, m_wndSpread.GetSheetCount());

		int nSheet = 1;
		m_wndSpread.SetSheetCount(CompareItemSet.size());
		for(set<string>::iterator itr = CompareItemSet.begin();itr != CompareItemSet.end();++itr)
		{
			CString rString(itr->c_str());
			rString = rString.MakeUpper();
			m_wndSpread.SetSheet(nSheet++);
			m_wndSpread.SetSheetName(rString);
			
			m_wndSpread.SetColHeaderRows(3);
			m_wndSpread.SetRowHeadersShow(FALSE);
			m_wndSpread.SetMaxCols(13);
			CDrumScheduleResultView::CreateSheetHeader(m_wndSpread);
		}
		
		DisplayCompareResultOfRevisionData(1 , _T(".TXT"));

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author KHS	

	@date 2009-08-19 오후 2:42:43	

	@param	

	@return		
**/
int CCompareRevisionDataView::CreateControlCableResultHeader(void)
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
	@brief	

	@author	BHK

	@date	2009.10.26

	@param	

	@return
*/
void CCompareRevisionDataView::OnFilePrint()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	string rOutputFolderPath = docData.GetProjectFolderPath();
	
	CCompareRevisionDataDoc* pDoc = (CCompareRevisionDataDoc*)GetDocument();
	if(pDoc)
	{
		if((0 == pDoc->m_rProductName.CompareNoCase(_T("LOAD SUMMARY-BUS"))) || (0 == pDoc->m_rProductName.CompareNoCase(_T("LOAD SUMMARY-SUBBUS"))))
		{
			rOutputFolderPath += _T("Load Summary Result\\Compare Result");
		}
		else if(0 == pDoc->m_rProductName.CompareNoCase(_T("TRANSFORMER SIZING")))
		{
			rOutputFolderPath += _T("TRANSFORMER SIZING\\Compare Result");
		}
		else if(0 == pDoc->m_rProductName.CompareNoCase(_T("AUTO POWER CABLE")))
		{
			rOutputFolderPath += _T("Cable Sizing Result\\Compare Result");
		}
		else if(0 == pDoc->m_rProductName.CompareNoCase(_T("AUTO CONTROL CABLE")))
		{
			rOutputFolderPath += _T("Control Cable Result\\Compare Result");
		}
		else if(0 == pDoc->m_rProductName.CompareNoCase(_T("CABLE SCHEDULE")))
		{
			rOutputFolderPath += _T("CABLE SCHEDULE Result\\Compare Result");
		}
		else if(0 == pDoc->m_rProductName.CompareNoCase(_T("DRUM SCHEDULE")))
		{
			rOutputFolderPath += _T("DRUM SCHEDULE Result\\Compare Result");
		}
	}
	CreateOrClearFolder(rOutputFolderPath.c_str());
	::CreateDirectory(rOutputFolderPath.c_str() , NULL);
	
	for(int i = 0;i <= m_wndSpread.GetSheetCount();++i)
	{
		m_wndSpread.SetSheet(i);
		CString rSheetName = m_wndSpread.GetSheetName();
		if(_T("*") == rSheetName.Right(1)) rSheetName = rSheetName.Left(rSheetName.GetLength() - 1);
		if(0 == pDoc->m_rProductName.CompareNoCase(_T("LOAD SUMMARY-BUS")))
		{
			if(_T("TOTAL") == rSheetName) rSheetName = _T("TotalBUS");
		}
		else if (0 == pDoc->m_rProductName.CompareNoCase(_T("LOAD SUMMARY-SUBBUS")))
		{
			if(_T("TOTAL") == rSheetName) rSheetName = _T("TotalSubBUS");
		}
		
		string ofilepath = (rOutputFolderPath + _T("\\") + string(rSheetName.operator LPCTSTR()) + _T(".BUS")).c_str();
		if(0 == pDoc->m_rProductName.CompareNoCase(_T("CABLE SCHEDULE")))
		{
			ofilepath = rOutputFolderPath + _T("\\CableScheduleResult.txt");
		}
		else if(0 == pDoc->m_rProductName.CompareNoCase(_T("DRUM SCHEDULE")))
		{
			ofilepath = rOutputFolderPath + _T("\\DrumScheduleResult.txt");
		}
		else if(0 == pDoc->m_rProductName.CompareNoCase(_T("TRANSFORMER SIZING")))
		{
			ofilepath = rOutputFolderPath + _T("\\TransformerSizingResult.txt");
		}
		ofstream ofile(ofilepath.c_str());
		if(ofile.is_open())
		{
			for(int j = 1;j <= m_wndSpread.GetMaxRows();++j)
			{
				m_wndSpread.SetRow(j);
				m_wndSpread.SetCol(0);
				CString rText = m_wndSpread.GetText();
				if(_T("-") == rText) continue;

				for(int k = 1;k <= m_wndSpread.GetMaxCols();++k)
				{
					m_wndSpread.SetCol(k);

					rText = m_wndSpread.GetText();
					if(RGB(255,0,0) == m_wndSpread.GetBackColor())
					{
						rText += _T("*");
					}
					ofile << rText.operator LPCTSTR();
					if((k + 1) <= m_wndSpread.GetMaxCols()) ofile << _T("\t");
				}
				if((j + 1) <= m_wndSpread.GetMaxRows()) ofile << std::endl;
			}
			ofile.close();
		}
	}

	if(pDoc)
	{
		if(0 == pDoc->m_rProductName.CompareNoCase(_T("LOAD SUMMARY-BUS")))
		{
			CLoadSummaryView::ExportToExcel(rOutputFolderPath.c_str() , _T("ALL BUS LOAD SUMMARY"));
		}
		else if(_T("LOAD SUMMARY-SUBBUS") == pDoc->m_rProductName)
		{
			CLoadSummaryView::ExportToExcel(rOutputFolderPath.c_str() , _T("ALL PDB LOAD SUMMARY"));
		}
		else if((_T("TRANSFORMER SIZING") == pDoc->m_rProductName))
		{
			CTransformerSizingResultView::ExportToExcel(string(rOutputFolderPath + "\\TransformerSizingResult.txt").c_str());
		}
		else if((_T("AUTO POWER CABLE") == pDoc->m_rProductName))
		{
			CCableSizingResultView::ExportToExcel(rOutputFolderPath.c_str());
		}
		else if((_T("AUTO CONTROL CABLE") == pDoc->m_rProductName))
		{
			CControlCableResultView::ExportToExcel(rOutputFolderPath.c_str());
		}
		else if((_T("CABLE SCHEDULE") == pDoc->m_rProductName))
		{
			CCableScheduleResultView::ExportToExcel((rOutputFolderPath + _T("\\CableScheduleResult.txt")).c_str());
		}
		else if((_T("DRUM SCHEDULE") == pDoc->m_rProductName))
		{
			CDrumScheduleResultView::ExportToExcel(rOutputFolderPath.c_str());
		}
	}
}

/**
	@brief	collect file for comparing.

	@author	BHK

	@date	2009.12.29

	@param	

	@return
*/
int CCompareRevisionDataView::CollectItemsForComparing(set<string>& CompareItemSet , const CString& rExtension)
{
	CompareItemSet.clear();

	CCompareRevisionDataDoc* pDoc = (CCompareRevisionDataDoc*)GetDocument();
	CELoadDocData& docData = CELoadDocData::GetInstance();
	{
		CString strDir = pDoc->GetLeftSideFolderPath() + _T("*") + rExtension , rRevNo;
		if(-1 != pDoc->m_nLeftSideRevNo)
		{
			rRevNo.Format(_T("%d") , pDoc->m_nLeftSideRevNo);
			pDoc->m_LeftSideItemEntry.clear();

			CFileFind file;
			///! 확장자가 일치하는 파일을 찾는다.
			BOOL bFound = file.FindFile(strDir);
			if(bFound)
			{
				while(bFound)
				{
					bFound = file.FindNextFile();
					if(file.IsDots()) continue;

					const CString rFileName = file.GetFileTitle().MakeUpper();
					pDoc->m_LeftSideItemEntry.push_back(rFileName.operator LPCSTR());
				}
			}
		}
		else
		{
			pDoc->m_LeftSideItemEntry.clear();

			CFileFind file;
			///! 확장자가 일치하는 파일을 찾는다.
			BOOL bFound = file.FindFile(strDir);
			if(bFound)
			{
				while(bFound)
				{
					bFound = file.FindNextFile();
					if(file.IsDots()) continue;

					const CString rFileName = file.GetFileTitle().MakeUpper();
					pDoc->m_LeftSideItemEntry.push_back(rFileName.operator LPCSTR());
				}
			}
		}
	}

	{
		CString strDir = docData.GetProjectFolderPath().c_str() , rRevNo;
		rRevNo.Format(_T("%d") , pDoc->m_nRightSideRevNo);
		strDir = CCompareRevisionDataDoc::GetRevPath(pDoc->m_rProductName , rRevNo) + _T("\\*") + rExtension;

		pDoc->m_RightSideItemEntry.clear();

		CFileFind file;
		///! 확장자가 일치하는 파일을 찾는다.
		BOOL bFound = file.FindFile(strDir);
		if(bFound)
		{
			while(bFound)
			{
				bFound = file.FindNextFile();
				if(file.IsDots()) continue;

				const CString rFileName = file.GetFileTitle().MakeUpper();
				pDoc->m_RightSideItemEntry.push_back(rFileName.operator LPCSTR());
			}
		}
	}
		
	{
		for(vector<string>::iterator itr = pDoc->m_LeftSideItemEntry.begin();itr != pDoc->m_LeftSideItemEntry.end();++itr)
		{
			CompareItemSet.insert(*itr);
		}
	}
	{
		for(vector<string>::iterator itr = pDoc->m_RightSideItemEntry.begin();itr != pDoc->m_RightSideItemEntry.end();++itr)
		{
			CompareItemSet.insert(*itr);
		}
	}

	return ERROR_SUCCESS;
}
