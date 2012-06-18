// CableSizingResultView.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "PowerCableVerificationView.h"
#include <vector>
#include "ELoadDocData.h"
#include "CableItem.h"
#include "PowerCableVerificationDoc.h"

#define ROUND(x) ((int)((x+0.005f)*100.0f))/100.0f //소숫점 2자리로 반올림
#define ROW_HEIGHT 13
// CPowerCableVerificationView

IMPLEMENT_DYNCREATE(CPowerCableVerificationView, CFormView)

CPowerCableVerificationView::CPowerCableVerificationView()
	: CFormView(CPowerCableVerificationView::IDD)
{

}

CPowerCableVerificationView::~CPowerCableVerificationView()
{
}

void CPowerCableVerificationView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FPSPREAD_CABLESIZING_RESULT_VIEW, m_wndSpread);
}

BEGIN_MESSAGE_MAP(CPowerCableVerificationView, CFormView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_SAVE, &CPowerCableVerificationView::OnFileSave)
END_MESSAGE_MAP()


// CPowerCableVerificationView diagnostics

#ifdef _DEBUG
void CPowerCableVerificationView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CPowerCableVerificationView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPowerCableVerificationView message handlers

void CPowerCableVerificationView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
}

int CPowerCableVerificationView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	
	return 0;
}

void CPowerCableVerificationView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if(m_wndSpread.GetSafeHwnd())
	{
		m_wndSpread.SetWindowPos(NULL , 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

/**
	@brief  CREATE CABLE SIZING RESULT SHEET

	@author KHS,BHK
*/
int CPowerCableVerificationView::DisplayCableResultData(void)
{
	CPowerCableVerificationDoc* pDoc = (CPowerCableVerificationDoc*)GetDocument();
	if(pDoc)
	{
		m_wndSpread.SetSheetCount(1);
		m_wndSpread.SetMaxRows(0);
		if(TRUE == m_wndSpread.LoadTextFile(pDoc->m_rFilePath.c_str() , "" , "\t" , "\n" , NULL , ""))
		{
			m_wndSpread.SetRowHeaderCols(1);
			m_wndSpread.SetColHeaderRows(2);
			m_wndSpread.SetRowHeadersShow(TRUE);
			m_wndSpread.SetMaxCols(30);
			m_wndSpread.SetColsFrozen(5);	//! UNIT까지 스크롤 고정

			//! 시트의 해더를 생성
			CreateHeader();

			//! 배경 색상 설정.
			CELoadDocData& docData = CELoadDocData::GetInstance();
			const COLORREF ItemIdColor   = docData.GetColorCode(_T("ITEM_ID_BACK_COLOR"));
			const COLORREF FrozenColColor= docData.GetColorCode(_T("LOAD_SUMMARY_TOTAL_BACK_COLOR"));

			const long nMaxRows = m_wndSpread.GetMaxRows();
			const long nMaxCols = m_wndSpread.GetMaxCols();
			for(int i = 1;i <= nMaxRows;++i)
			{
				m_wndSpread.SetRow(i);

				m_wndSpread.SetCol(1);
				m_wndSpread.SetBackColor(FrozenColColor);
				m_wndSpread.SetBackColorStyle(m_wndSpread.BackColorStyleUnderGrid);

				m_wndSpread.SetCol(2);
				m_wndSpread.SetBackColor(FrozenColColor);
				m_wndSpread.SetBackColorStyle(m_wndSpread.BackColorStyleUnderGrid);

				m_wndSpread.SetCol(3);
				m_wndSpread.SetBackColor(FrozenColColor);
				m_wndSpread.SetBackColorStyle(m_wndSpread.BackColorStyleUnderGrid);

				m_wndSpread.SetCol(4);
				m_wndSpread.SetBackColor(FrozenColColor);
				m_wndSpread.SetBackColorStyle(m_wndSpread.BackColorStyleUnderGrid);

				m_wndSpread.SetCol(5);
				m_wndSpread.SetBackColor(FrozenColColor);
				m_wndSpread.SetBackColorStyle(m_wndSpread.BackColorStyleUnderGrid);

				m_wndSpread.SetCol(6);
				m_wndSpread.SetBackColor(FrozenColColor);
				m_wndSpread.SetBackColorStyle(m_wndSpread.BackColorStyleUnderGrid);

				m_wndSpread.SetCol(14);
				m_wndSpread.SetBackColor(ItemIdColor);
				m_wndSpread.SetBackColorStyle(m_wndSpread.BackColorStyleUnderGrid);

				m_wndSpread.SetCol(17);
				m_wndSpread.SetBackColor(ItemIdColor);
				m_wndSpread.SetBackColorStyle(m_wndSpread.BackColorStyleUnderGrid);

				m_wndSpread.SetCol(18);
				m_wndSpread.SetBackColor(ItemIdColor);
				m_wndSpread.SetBackColorStyle(m_wndSpread.BackColorStyleUnderGrid);

				m_wndSpread.SetCol(25);
				m_wndSpread.SetBackColor(ItemIdColor);
				m_wndSpread.SetBackColorStyle(m_wndSpread.BackColorStyleUnderGrid);

				m_wndSpread.SetCol(28);
				m_wndSpread.SetBackColor(ItemIdColor);
				m_wndSpread.SetBackColorStyle(m_wndSpread.BackColorStyleUnderGrid);
			}
		}
	}

	return ERROR_SUCCESS;
}
/**
	@brief  SHEET의 해더를 생성한다.
	@author KHS
*/
int CPowerCableVerificationView::CreateHeader(void)
{
	m_wndSpread.put_RowHeight(m_wndSpread.SpreadHeader, ROW_HEIGHT * 2);
	m_wndSpread.put_RowHeight(m_wndSpread.SpreadHeader + 1, 60);
	
	//! SET COLUMN WIDTH
	m_wndSpread.put_ColWidth(1 , 13);	// BUS ID
	m_wndSpread.put_ColWidth(2 , 5);	// BUS ID
	m_wndSpread.put_ColWidth(3 , 13);	// LOAD ID
	m_wndSpread.put_ColWidth(14 , 15);	// CABLEID
	m_wndSpread.put_ColWidth(16 , 13);	// CABLE TYPE

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
	m_wndSpread.SetCol(0);
	m_wndSpread.SetText(_T("NO."));
	m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);
	m_wndSpread.AddCellSpan(0, m_wndSpread.SpreadHeader, 1, 2);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
	m_wndSpread.SetCol(1);
	m_wndSpread.SetText(_T("BUS NO."));
	m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);
	m_wndSpread.AddCellSpan(1, m_wndSpread.SpreadHeader, 1, 2);

	m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
	m_wndSpread.SetCol(2);
	m_wndSpread.SetText(_T("BUS VOLT."));
	m_wndSpread.SetTypeTextOrient(m_wndSpread.TypeTextOrientUp);
	m_wndSpread.AddCellSpan(2, m_wndSpread.SpreadHeader, 1, 2);

	//! LOAD DATA
	m_wndSpread.SetRow(m_wndSpread.SpreadHeader);
	m_wndSpread.SetCol(3);
	m_wndSpread.SetText(_T("LOAD DATA"));
	m_wndSpread.AddCellSpan(3, m_wndSpread.SpreadHeader, 11, 1);

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
	m_wndSpread.SetText(_T("VD [V]"));
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

	return ERROR_SUCCESS;
}
/**
	@brief  SPREAD SHEET에 칼럼을 추가한다.
	@author KHS
*/
int CPowerCableVerificationView::AddColumnItem(const int& nRow, const int& nColumn, const string& rText)
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
int CPowerCableVerificationView::AdjustSheetTextAlign(void)
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
			if(6 == __COLS__[j] || 17 == __COLS__[j])
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
void CPowerCableVerificationView::OnFileSave()
{
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
        if(pApp) pApp->OnSaveDataBase();
}
