// CableSizingView.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "CableSizingTableView.h"
#include "ELoadDocData.h"
#include "SimpleAnimationDlg.h"
#include "CableSizingTableDoc.h"
#include "pyfromc.h"
#include <fstream>
#include <string>

using namespace std;

#define ROW_HEIGHT      13
#define COL_WIDTH       11
#define CABLE_BACK_COLOR	RGB(151,151,151)
// CCableSizingTableView

IMPLEMENT_DYNCREATE(CCableSizingTableView, CFormView)

CCableSizingTableView::CCableSizingTableView()
	: CFormView(CCableSizingTableView::IDD)
{
        m_nColCount = 0;
        m_nRowCount = 0;
}

CCableSizingTableView::~CCableSizingTableView()
{
}

void CCableSizingTableView::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX , IDC_BUTTON_DELETE , m_wndDeleteButton);
	DDX_Control(pDX , IDC_FPSPREAD_CABLE_SIZING, m_wndSpread);

	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCableSizingTableView, CFormView)
        ON_WM_SIZE()
        ON_WM_CREATE()
        ON_COMMAND(ID_FILE_SAVE, &CCableSizingTableView::OnFileSave)
        ON_COMMAND(ID_FILE_PRINT, &CCableSizingTableView::OnFilePrint)
        ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CCableSizingTableView::OnBnClickedButtonDelete)
END_MESSAGE_MAP()


// CCableSizingTableView diagnostics

#ifdef _DEBUG
void CCableSizingTableView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CCableSizingTableView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CCableSizingTableView message handlers

void CCableSizingTableView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if(m_wndSpread.GetSafeHwnd())
	{
		CRect rect;
		m_wndSpread.GetWindowRect(&rect);
		ScreenToClient(&rect);

		m_wndSpread.SetWindowPos(NULL , 0, 0, cx, cy - rect.top, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
	}
}

int CCableSizingTableView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CCableSizingTableView::OnFileSave()
{
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp) pApp->OnSaveDataBase();
}

/**
	@brief	when document is modified

	@author	HumKyung

	@date	????.??.??

	@param	pSender
	@param	lHint
	@param	pHint

	@return
*/
void CCableSizingTableView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	
	CCableSizingTableDoc* pDoc = (CCableSizingTableDoc*)GetDocument();
	if(pDoc)
	{
		CString strDir = pDoc->m_rFolderPath.c_str() + CString(_T("\\*.TXT"));
		CFileFind file;

		vector<string> rInterestingFileEntry;
		///! 확장자가 *.TXT 파일을 찾는다.
		BOOL bFound = file.FindFile(strDir);
		if(bFound)
		{
			while(bFound)
			{
				bFound = file.FindNextFile();
				if(file.IsDots()) continue;

				const CString rFileName = file.GetFileTitle().MakeUpper();
				rInterestingFileEntry.push_back(rFileName.operator LPCSTR());
			}
		}

		m_wndSpread.Reset();	//! 2011.02.24 added by HumKyung
		//! force sheet count to be more equal than 2.
		m_wndSpread.SetSheetCount((rInterestingFileEntry.size() < 2 ? 2 : rInterestingFileEntry.size()));
		
		int nSheet = 1;
		//! 데이터를 읽는 동안에는 화면에 뿌리지 않는다.
		m_wndSpread.SetReDraw(FALSE);
		for(vector<string>::iterator itr = rInterestingFileEntry.begin();itr != rInterestingFileEntry.end();++itr)
		{
			m_wndSpread.SetSheet(nSheet++);
			m_wndSpread.SetColHeaderRows(9);
			m_wndSpread.SetRowHeaderCols(3);
			m_wndSpread.SetEditMode(FALSE);
			m_wndSpread.SetSheetName(itr->c_str());
			m_wndSpread.SetTypeHAlign(m_wndSpread.TypeHAlignRight);

			const string rFilePath = pDoc->m_rFolderPath + _T("\\") + (*itr) + _T(".TXT");
			m_nColCount = GetPrivateProfileInt(_T("CABLE_SIZE_DATA") , _T("COLCOUNT") , 0 , rFilePath.c_str());
			m_nRowCount = GetPrivateProfileInt(_T("CABLE_SIZE_DATA") , _T("ROWCOUNT") , 0 , rFilePath.c_str());

			CreateHeaderAndFillData(m_wndSpread , rFilePath);
			
			CompareSafetyVoltage();
			ResetValueOverThousandExceptFirstColumn();
			CableSizingSettingTableDataColorSet();
		}
		
		m_wndSpread.SetActiveSheet(1);
		const short nSheetCount = m_wndSpread.GetSheetCount();
		for(int i = 1;i <= nSheetCount;++i)
		{
			m_wndSpread.SetSheet(i);
			const CString rSheetName = m_wndSpread.GetSheetName().MakeUpper();
			if(pDoc->m_rFilePath == rSheetName.operator LPCSTR())
			{
				m_wndSpread.SetActiveSheet(i);
				break;
			}
		}
		
		//! 데이터를 다 읽었으면 화면에 뿌리도록 한다.
		m_wndSpread.SetReDraw(TRUE);
	}
}

/**
	@brief  CABLE SIZING 시트의 해더 생성

	@author KHS,HumKyung

	@date	????.??.??

	@param	wndSprea
	@param	rFilePath

	@return	int
*/
int CCableSizingTableView::CreateHeaderAndFillData(CFpspread1& wndSpread , const string& rFilePath)
{
	CELoadDocData &docData = CELoadDocData::GetInstance();
	CCableSizeSettingTable& CableSizeSettingTable = CCableSizeSettingTable::GetInstance();

	static const CString rApp("CABLE_SIZE_DATA");
	const int MAX_BUF_SIZE = 512;
	char szBuf[MAX_BUF_SIZE + 1] = {'\0'};
	
	static char* aCol[3] = {"FLC", "KWL", "PF"};
	static char* aRow[7] = {"CONDUIT", "CABLESIZE", "ALLOWANCE", "SAFETY" , "REGISTANCE", "REACTANCE", "DIA"};

	CString rTemp, rColCount, rRowCount;
	CStringArray rKwl, rPf, rFlc, rCableSize, rDia, rAllowance, rSafety ,rRegistance, rReactance, ConduitStringArray;

	//! COLUMNE & ROW COUNT
	const int nColCount = GetPrivateProfileInt(rApp , _T("COLCOUNT") , 0 , rFilePath.c_str());
	const int nRowCount = GetPrivateProfileInt(rApp , _T("ROWCOUNT") , 0 , rFilePath.c_str());
	
	wndSpread.SetMaxCols(nColCount);
	wndSpread.SetMaxRows(nRowCount + 5);

	//! Row Header
	wndSpread.SetRow(wndSpread.SpreadHeader);
	wndSpread.SetCol(wndSpread.SpreadHeader);
	wndSpread.SetText(_T("CABLE (M)"));
	wndSpread.AddCellSpan(wndSpread.SpreadHeader, wndSpread.SpreadHeader, 1, 6);
	wndSpread.SetTypeHAlign(wndSpread.TypeHAlignCenter);

	wndSpread.SetRow(wndSpread.SpreadHeader);
	wndSpread.SetCol(wndSpread.SpreadHeader + 1);
	wndSpread.SetText(_T("CABLE SIZE                  (SQMM)"));
	wndSpread.AddCellSpan(wndSpread.SpreadHeader + 1, wndSpread.SpreadHeader, 2, 1);
	wndSpread.SetTypeHAlign(wndSpread.TypeHAlignLeft);

	wndSpread.SetRow(wndSpread.SpreadHeader + 1);
	wndSpread.SetCol(wndSpread.SpreadHeader + 1);
	wndSpread.SetText(_T("CABLE DIAMETER            (MM)"));
	wndSpread.AddCellSpan(wndSpread.SpreadHeader + 1, wndSpread.SpreadHeader + 1, 2, 1);
	wndSpread.SetTypeHAlign(wndSpread.TypeHAlignLeft);

	wndSpread.SetRow(wndSpread.SpreadHeader + 2);
	wndSpread.SetCol(wndSpread.SpreadHeader + 1);
	wndSpread.SetText("ALLOWANCE CURRENT     (A)");
	wndSpread.AddCellSpan(wndSpread.SpreadHeader + 1, wndSpread.SpreadHeader + 2, 2, 1);
	wndSpread.SetTypeHAlign(wndSpread.TypeHAlignLeft);

	wndSpread.SetRow(wndSpread.SpreadHeader + 3);
	wndSpread.SetCol(wndSpread.SpreadHeader + 1);
	wndSpread.SetText("SAFETY CURRENT              (A)");
	wndSpread.AddCellSpan(wndSpread.SpreadHeader + 1, wndSpread.SpreadHeader + 3, 2, 1);
	wndSpread.SetTypeHAlign(wndSpread.TypeHAlignLeft);

	wndSpread.SetRow(wndSpread.SpreadHeader + 4);
	wndSpread.SetCol(wndSpread.SpreadHeader + 1);
	wndSpread.SetText("REGISTANCE(90℃)  (Ohm/Km)");
	wndSpread.AddCellSpan(wndSpread.SpreadHeader + 1, wndSpread.SpreadHeader + 4, 2, 1);
	wndSpread.SetTypeHAlign(wndSpread.TypeHAlignLeft);

	wndSpread.SetRow(wndSpread.SpreadHeader + 5);
	wndSpread.SetCol(wndSpread.SpreadHeader + 1);
	wndSpread.SetText("REACTANCE              (Ohm/Km)");
	wndSpread.AddCellSpan(wndSpread.SpreadHeader + 1, wndSpread.SpreadHeader + 5, 2, 1);
	wndSpread.SetTypeHAlign(wndSpread.TypeHAlignLeft);

	//! ROW DATA
	for(int i =0;i < sizeof(aRow)/ sizeof(aRow[0]);i++)
	{
		if(GetPrivateProfileString(rApp , aRow[i] , "" , szBuf , MAX_BUF_SIZE , rFilePath.c_str()))
		{
			rTemp = szBuf;
			if("," == rTemp.Right(1))
			{
				int at = rTemp.ReverseFind(',');
				rTemp = rTemp.Left(at);
			}
			vector<string> oResult;
			CTokenizer<CIsFromString>::Tokenize(oResult, rTemp.operator LPCSTR(), CIsFromString(","));
			for(vector<string>::iterator itr = oResult.begin(); itr != oResult.end();++itr)
			{
				if(aRow[i] == "CONDUIT")
				{
					ConduitStringArray.Add((*itr).c_str());
				}
				else if(aRow[i] == "CABLESIZE")
				{
					rCableSize.Add((*itr).c_str());
				}
				else if(aRow[i] == "ALLOWANCE")
				{
					rAllowance.Add((*itr).c_str());
				}
				else if(aRow[i] == "SAFETY")
				{
					rSafety.Add((*itr).c_str());
				}
				else if(aRow[i] == "REGISTANCE")
				{
					rRegistance.Add((*itr).c_str());
				}
				else if(aRow[i] == "REACTANCE")
				{
					rReactance.Add((*itr).c_str());
				}
				else if(aRow[i] == "DIA")
				{
					rDia.Add((*itr).c_str());
				}
			}
		}
	}
	for(int c = 0; c < nColCount;++c)
	{
		wndSpread.put_ColWidth(c, COL_WIDTH * 0.7);

		wndSpread.SetRow(wndSpread.SpreadHeader);
		wndSpread.SetCol(1 + c);
		wndSpread.SetText(rCableSize[c]);

		wndSpread.SetRow(wndSpread.SpreadHeader + 1);
		wndSpread.SetCol(1 + c);
		wndSpread.SetText(rDia[c]);

		wndSpread.SetRow(wndSpread.SpreadHeader + 2);
		wndSpread.SetCol(1 + c);
		wndSpread.SetText(rAllowance[c]);

		wndSpread.SetRow(wndSpread.SpreadHeader + 3);
		wndSpread.SetCol(1 + c);
		wndSpread.SetText(rSafety[c]);

		wndSpread.SetRow(wndSpread.SpreadHeader + 4);
		wndSpread.SetCol(1 + c);
		wndSpread.SetText(rRegistance[c]);

		wndSpread.SetRow(wndSpread.SpreadHeader + 5);
		wndSpread.SetCol(1 + c);
		wndSpread.SetText(rReactance[c]);
	}

	//! Motor Type
	CString sMotorType;
	if(GetPrivateProfileString(rApp ,_T("MOTOR TYPE") , _T("") , szBuf , MAX_BUF_SIZE , rFilePath.c_str()))
	{
		sMotorType = szBuf;
	}

	wndSpread.SetRow(wndSpread.SpreadHeader + 6);
	wndSpread.SetCol(wndSpread.SpreadHeader);
	wndSpread.SetText(sMotorType);
	wndSpread.AddCellSpan(wndSpread.SpreadHeader, wndSpread.SpreadHeader + 6, 3, 1);
	wndSpread.SetTypeHAlign(wndSpread.TypeHAlignCenter);

	wndSpread.SetRow(wndSpread.SpreadHeader + 7);
	wndSpread.SetCol(wndSpread.SpreadHeader);
	if(_T("3PH MOTOR") == sMotorType)
	{
		wndSpread.SetText(_T("KW"));
	}
	else 
	{
		wndSpread.SetText(_T("KVA"));
	}

	wndSpread.AddCellSpan(wndSpread.SpreadHeader, wndSpread.SpreadHeader + 7, 1, 2);
	wndSpread.SetTypeHAlign(wndSpread.TypeHAlignCenter);

	wndSpread.SetRow(wndSpread.SpreadHeader + 7);
	wndSpread.SetCol(wndSpread.SpreadHeader + 1);
	wndSpread.SetText(_T("FLC (A)"));
	wndSpread.AddCellSpan(wndSpread.SpreadHeader + 1, wndSpread.SpreadHeader + 7, 1, 2);
	wndSpread.SetTypeHAlign(wndSpread.TypeHAlignCenter);

	wndSpread.SetRow(wndSpread.SpreadHeader + 7);
	wndSpread.SetCol(wndSpread.SpreadHeader + 2);
	wndSpread.SetText(_T("PF"));
	wndSpread.AddCellSpan(wndSpread.SpreadHeader + 2, wndSpread.SpreadHeader + 7, 1, 2);
	wndSpread.SetTypeHAlign(wndSpread.TypeHAlignCenter);

	wndSpread.SetRow(wndSpread.SpreadHeader + 6);
	wndSpread.SetCol(1);
	wndSpread.SetText(_T("CONDUIT SIZE (SQMM)"));///  (ONLY FOR POWER CABLE)");
	wndSpread.AddCellSpan(1, wndSpread.SpreadHeader + 6, ConduitStringArray.GetCount() , 2);
	wndSpread.SetTypeHAlign(wndSpread.TypeHAlignCenter);

	//! CONDUIT
	for(int i = 0; i < ConduitStringArray.GetCount();i++)
	{
		wndSpread.SetRow(wndSpread.SpreadHeader + 8);
		wndSpread.SetCol(i + 1);
		wndSpread.SetText(ConduitStringArray[i]);
		wndSpread.SetTypeHAlign(wndSpread.TypeHAlignCenter);
	}

	//! COL DATA
	for(int j =0; j < SIZE_OF_ARRAY(aCol);j++)
	{
		if(GetPrivateProfileString(rApp , aCol[j] , "" , szBuf , MAX_BUF_SIZE , rFilePath.c_str()))
		{
			rTemp = szBuf;
			if("," == rTemp.Right(1))
			{
				int at = rTemp.ReverseFind(',');
				rTemp = rTemp.Left(at);
			}
			vector<string> roResult;
			CTokenizer<CIsFromString>::Tokenize(roResult, rTemp.operator LPCSTR(), CIsFromString(","));
			for(vector<string>::iterator jtr = roResult.begin(); jtr != roResult.end();++jtr)
			{
				if(aCol[j] == "FLC")
				{
					rFlc.Add((*jtr).c_str());
				}
				else if(aCol[j] == "KWL")
				{
					rKwl.Add((*jtr).c_str());
				}
				else if(aCol[j] == "PF")
				{
					rPf.Add((*jtr).c_str());
				}
			}
		}
	}

	for(int k = 0;k < nRowCount; ++k)
	{
		//! SHEET ROW HEIGHT SETTING
		wndSpread.put_RowHeight(k, ROW_HEIGHT);

		wndSpread.SetRow(1 + k);
		wndSpread.SetCol(wndSpread.SpreadHeader);
		wndSpread.SetText((k < rKwl.GetCount()) ? rKwl[k] : _T(""));
		wndSpread.SetTypeHAlign(wndSpread.TypeHAlignCenter);

		wndSpread.SetRow(1 + k);
		wndSpread.SetCol(wndSpread.SpreadHeader + 1);
		wndSpread.SetText((k < rFlc.GetCount()) ? rFlc[k] : _T(""));
		wndSpread.SetTypeHAlign(wndSpread.TypeHAlignCenter);

		wndSpread.SetRow(1 + k);
		wndSpread.SetCol(wndSpread.SpreadHeader + 2);
		if(_T("DC FEEDER") != sMotorType)
		{
			wndSpread.SetText((k < rPf.GetCount()) ? rPf[k] : _T(" "));
		}
		else
		{
			wndSpread.SetText(_T(" "));
		}
		wndSpread.SetTypeHAlign(wndSpread.TypeHAlignCenter);
	}

	//! CELL DATA
	for(int nRow = 0;nRow < nRowCount;nRow++)
	{
		CString rMsg;
		rMsg.Format("CELL%d", nRow);
		if(GetPrivateProfileString(rApp , rMsg , "" , szBuf , MAX_PATH , rFilePath.c_str()))
		{
			rTemp = szBuf;
			if("," == rTemp.Right(1))
			{
				int at = rTemp.ReverseFind(',');
				rTemp = rTemp.Left(at);
			}
			vector<string> oResult;
			CTokenizer<CIsFromString>::Tokenize(oResult, rTemp.operator LPCSTR(), CIsFromString(","));
			int nCol = 0;
			for(vector<string>::iterator itr = oResult.begin(); itr != oResult.end();++itr)
			{
				wndSpread.SetRow(nRow + 1);
				wndSpread.SetCol(nCol + 1);
				wndSpread.SetText((*itr).c_str());
				wndSpread.SetTypeHAlign(wndSpread.TypeHAlignRight);
				nCol++;
			}
		}
	}
	//! HEADER ROW HEIGHT SETTING
	for(int a = 0; a < 9;a++)
	{
		wndSpread.put_RowHeight(wndSpread.SpreadHeader + a, ROW_HEIGHT);
	}

	wndSpread.put_ColWidth(wndSpread.SpreadHeader + 1, COL_WIDTH);
	wndSpread.put_ColWidth(wndSpread.SpreadHeader + 2, COL_WIDTH);

	//! CABLE SIZING SETTING DATA OUTPUT
	static const CString __ITEM__[] = 
	{
		"DERATING FACTOR" , "CABLE DATA TABLE" , "SELECT BUS" , "MOTOR TYPE",
		"BUS VOLTAGE" , "MOTOR VOLTAGE" , "VOLTAGE DROP %" , "VOLTAGE DROP V" , 
		"PF FOR FEEDER" , "STARTING VOLTAGE DROP %" , "STARTING VOLTAGE DROP V" , "STARTING POWER FACTOR",
		"INSULATION METHOD" , "MOTOR TYPE" , "CONDUIT TABLE" , "CONDUIT DESC" ,
		"CABLE TYPE"
	};
	for(int nItem = 0; nItem < SIZE_OF_ARRAY(__ITEM__);++nItem)
	{
		CString rValue;
		if(GetPrivateProfileString(rApp , __ITEM__[nItem] , _T("") , szBuf , MAX_BUF_SIZE , rFilePath.c_str()))
		{
			rValue = szBuf;
		}

		const int nRowIndex = nRowCount + (nItem / 4) + 1;
		wndSpread.SetRow(nRowIndex);
		wndSpread.put_RowHeight(nRowCount + (nItem / 4) + 1 , ROW_HEIGHT);
		if(0 == nItem || 4 == nItem || 8 == nItem || 12 == nItem)
		{
			wndSpread.SetCol(1);
			wndSpread.SetText(__ITEM__[nItem] + " : " + rValue);
			wndSpread.AddCellSpan(1, nRowIndex , 3, 1);
			wndSpread.SetTypeHAlign(wndSpread.TypeHAlignLeft);

			wndSpread.SetCol(wndSpread.SpreadHeader + 2);
			wndSpread.SetText(_T(" "));
		}
		else if(1 == nItem || 5 == nItem || 9 == nItem || 13 == nItem)
		{
			wndSpread.SetCol(5);
			if((9 == nItem) && (0 != sMotorType.CompareNoCase(_T("3PH MOTOR"))))
			{
				wndSpread.SetText(__ITEM__[nItem] + _T(" : N/A"));	//! 2011.02.21 added by HumKyung
			}
			else
			{
				wndSpread.SetText(__ITEM__[nItem] + " : " + rValue);
			}
			wndSpread.AddCellSpan(5, nRowIndex , 3, 1);
			wndSpread.SetTypeHAlign(wndSpread.TypeHAlignLeft);
		}
		else if(2 == nItem || 6 == nItem || 10 == nItem || 14 == nItem)
		{
			wndSpread.SetCol(9);
			wndSpread.SetText(__ITEM__[nItem] + " : " + rValue);
			wndSpread.AddCellSpan(9, nRowIndex , 3, 1);
			wndSpread.SetTypeHAlign(wndSpread.TypeHAlignLeft);
		}
		else if(3 == nItem || 7 == nItem || 11 == nItem || 15 == nItem)
		{
			wndSpread.SetCol(13);
			if((11 == nItem) && (0 != sMotorType.CompareNoCase(_T("3PH MOTOR"))))
			{
				wndSpread.SetText(__ITEM__[nItem] + _T(" : N/A"));	//! 2011.02.21 added by HumKyung
			}
			else
			{
				wndSpread.SetText(__ITEM__[nItem] + " : " + rValue);
			}
			wndSpread.AddCellSpan(13, nRowIndex , 3, 1);
			wndSpread.SetTypeHAlign(wndSpread.TypeHAlignLeft);
		}
		else if(16 == nItem)
		{
			wndSpread.SetCol(1);
			wndSpread.SetText(__ITEM__[nItem] + " : " + rValue);
			wndSpread.AddCellSpan(1, nRowIndex , 6, 1);
			wndSpread.SetTypeHAlign(wndSpread.TypeHAlignLeft);

			wndSpread.SetCol(wndSpread.SpreadHeader + 2);
			wndSpread.SetText(_T(" "));
		}
	}

	return ERROR_SUCCESS;
}


/**
	@brief  Safety Current값보다 값이 큰지 비교한다.

	@author KHS
*/
int CCableSizingTableView::CompareSafetyVoltage(void)
{
	int nRowCount = m_nRowCount;
	int nColCount = m_nColCount;

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

static UINT gfnPrintCableSizing(LPVOID pVoid)
{
	CSimpleAnimationDlg& anidlg = CSimpleAnimationDlg::GetInstance();
	if(NULL == anidlg.GetSafeHwnd())
	{
		anidlg.DoModal();
	}

	return ERROR_SUCCESS;
}

/**
        @brief  CABLE SIZING EXCEL EXPORT

        @author KHS
*/
void CCableSizingTableView::OnFilePrint()
{
        CWinThread* pThread = AfxBeginThread(gfnPrintCableSizing , NULL , THREAD_PRIORITY_NORMAL,0);
	if(pThread)
	{
		//python.CallPyWriteCableSizingToExcel(); // 데이타를 바로 엑셀로 쓴다.
		WriteCableSizingToTxtFile();
		/*int nSheet = m_wndSpread.GetSheet();
		m_wndSpread.SetSheet(nSheet);
		const CString rSheetName = m_wndSpread.GetSheetName();*/

		python.CallPyWriteCableSizingTxtToExcel();
		
		if(NULL != pThread)
		{
			CSimpleAnimationDlg& anidlg = CSimpleAnimationDlg::GetInstance();
			if(anidlg.GetSafeHwnd()) anidlg.SendMessage(WM_COMMAND , IDOK);
		}
	}else	AfxMessageBox("Fail to create thread.");
}
/**     
        @brief  CABLE SIZING SHEET 내용을 TXT 파일로 출력

        @author KHS
*/
int CCableSizingTableView::WriteCableSizingToTxtFile(void)
{
	CELoadDocData &docData = CELoadDocData::GetInstance();
	
	int nSheet = m_wndSpread.GetActiveSheet();
	m_wndSpread.SetSheet(nSheet);

	int nRowCount = m_wndSpread.GetMaxRows();//m_nRowCount;
	int nColCount = m_wndSpread.GetMaxCols();//m_nColCount;

	CString rTxtFilePath = docData.GetProjectFolderPath().c_str();
	rTxtFilePath += _T("Working\\CABLE_SIZING_DATA.txt");
	ofstream ofile(rTxtFilePath, std::iostream::out);

	if(ofile.is_open() && ofile.good())
	{
		//! CABLE_SIZE , DIA, ALLOWANCE , SAFETY, REGISTANCE , REACTANCE
		for(int r = 0; r < 6;r++)
		{
			for(int c = 0;c < nColCount + 1;c++)
			{
				m_wndSpread.SetRow(m_wndSpread.SpreadHeader + r);
				if(c == 0)
					m_wndSpread.SetCol(m_wndSpread.SpreadHeader + 1);
				else
					m_wndSpread.SetCol(c);
				CString rValue = m_wndSpread.GetText();
				ofile <<  rValue << ",";
			}
			ofile << endl;
		}

		//! KW , FLC, PF
		for(int c = 0; c < 3;c++)
		{
			for(int r = 0;r < nRowCount + 1;r++)
			{
				if(r == 0)
					m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 7);
				else
					m_wndSpread.SetRow(r);
				m_wndSpread.SetCol( m_wndSpread.SpreadHeader + c);
				CString rValue = m_wndSpread.GetText();
				ofile << rValue << ",";
			}
			ofile << endl;
		}

		//! COMDUIT
		ofile << "CONDUIT,";
		for(int c = 0; c < nColCount; c++)
		{
			m_wndSpread.SetRow(m_wndSpread.SpreadHeader + 8);
			m_wndSpread.SetCol(c + 1);
			CString rValue = m_wndSpread.GetText();
			ofile << rValue << ",";
		}
		ofile << endl;
		//! CELL DATA
		for(int r = 0; r < nRowCount;r++)
		{
			ofile << "CELL" << r << ",";
			for(int c = 0;c < nColCount;c++)
			{
				m_wndSpread.SetRow(r + 1);
				m_wndSpread.SetCol(c + 1);

				CString rValue = m_wndSpread.GetText();
				ofile << rValue << ",";
			}
			ofile << endl;
		}
		ofile.close();
	}

	return 0;
}
/**
        @brief  reset cell which's vaue is over 1000 except first column.

        @author KHS,HumKyung

	@date	????.??.??

	@param

	@return	int
*/
int CCableSizingTableView::ResetValueOverThousandExceptFirstColumn(void)
{
	const int nRowCount = m_nRowCount;
	const int nColCount = m_nColCount;
	for(int r = 0; r < nRowCount;++r)
	{
		for(int c = 1; c < nColCount;++c)
		{
			m_wndSpread.SetCol(c + 1);
			m_wndSpread.SetRow(r + 1);
			const CString rValue = m_wndSpread.GetText();

			if(atof(rValue) > 1000.0)
			{
				m_wndSpread.SetCol(c + 1);
				m_wndSpread.SetRow(r + 1);
				m_wndSpread.SetText(_T(" "));
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief   draw background with white color

	@author  HumKyung

	@date	????.??.??

	@param	pDC

	@return	BOOL
*/
BOOL CCableSizingTableView::OnEraseBkgnd(CDC* pDC)
{
	if(m_wndSpread.GetSafeHwnd())
	{
		CRect rect;
		m_wndSpread.GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.bottom = rect.top;
		rect.top = 0;

		CBrush brush(RGB(255 , 255 , 255));
		pDC->FillRect(&rect , &brush);
	}

	return TRUE;
}

/**
	@brief   CABLESIZINGSETTING TABLEDATA 시트의 색을 칠해준다.

	@author  KHS,HumKyung

	@date	????.??.??

	@param

	@return	int
*/
int CCableSizingTableView::CableSizingSettingTableDataColorSet(void)
{
	int nRowCount = m_nRowCount;
        int nColCount = m_nColCount;
	for(int nRow = 1; nRow <= m_wndSpread.GetMaxRows();nRow++)
	{
		for(int nCol = 1; nCol <= m_wndSpread.GetMaxCols();nCol++)
		{
			m_wndSpread.SetRow(nRow);
			m_wndSpread.SetCol(nCol);
			if(nRow <= nRowCount)
			{
				m_wndSpread.SetBackColor(RGB(255,255,255));
			}
			else
			{
				m_wndSpread.SetBackColor(CABLE_BACK_COLOR);
			}
			m_wndSpread.SetBackColorStyle(m_wndSpread.BackColorStyleUnderGrid);
		}
	}
	return 0;
}

/**
	@brief	spread에서 active sheet을 삭제합니다. 그리고 해당 파일도 함께 삭제합니다.

	@author BHK	

	@date 2009-09-14 오후 4:29:50	

	@param	

	@return		
*/
void CCableSizingTableView::OnBnClickedButtonDelete()
{
	if(IDOK == AfxMessageBox(_T("Do you want to delete active sheet?") , MB_OKCANCEL| MB_ICONWARNING))
	{
		m_wndSpread.SetSheet(m_wndSpread.GetActiveSheet());
		const CString rSheetName = m_wndSpread.GetSheetName();
		if(m_wndSpread.DeleteSheets(m_wndSpread.GetActiveSheet() ,1))
		{
			CCableSizingTableDoc* pDoc = (CCableSizingTableDoc*)GetDocument();
			if(pDoc)
			{
				CString rFilePath = CString(pDoc->m_rFolderPath.c_str()) + _T("\\") + rSheetName + _T(".TXT");
				::DeleteFile(rFilePath);
			}

			if(1 == m_wndSpread.GetSheetCount())
			{
				m_wndSpread.SetSheetCount(m_wndSpread.GetSheetCount() + 1);
			}
		}
	}
}

/**
	@brief	

	@author BHK	

	@date 2009-09-16 오전 10:10:54	

	@param	

	@return		
*/
void CCableSizingTableView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	m_wndDeleteButton.SetIcon(IDI_CANCEL_18X18);
}
