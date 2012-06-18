// LoadSummaryStartUpDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "LoadSummaryStartUpDlg.h"
#include "MainFrm.h"
#include "EnableBuddyButton.h"
#include "LoadSummaryOptionData.h"
#include "ELoadDocData.h"
#include <util/ado/ADODB.h>
#include <util/ado/SQLFile.h>

BOOL CLoadSummaryStartUpDlg::__CHECKAPPLYALL__ = FALSE;

IMPLEMENT_DYNAMIC(CLoadSummaryStartUpDlg, CDialog)

CLoadSummaryStartUpDlg::CLoadSummaryStartUpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoadSummaryStartUpDlg::IDD, pParent)

{
}

CLoadSummaryStartUpDlg::~CLoadSummaryStartUpDlg()
{
}

void CLoadSummaryStartUpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Check(pDX , IDC_CHECK_APPLY_ALL , CLoadSummaryStartUpDlg::__CHECKAPPLYALL__);
	DDX_Control(pDX , IDC_BUTTON_RUN , m_wndRunButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);
	DDX_Control(pDX, IDC_EDIT_DOC_NO, m_wndDocNoEdit);
}


BEGIN_MESSAGE_MAP(CLoadSummaryStartUpDlg, CDialog)
        ON_BN_CLICKED(IDOK, &CLoadSummaryStartUpDlg::OnBnClickedOK)
	ON_BN_CLICKED(IDC_BUTTON_RUN, &CLoadSummaryStartUpDlg::OnBnClickedRun)
        ON_BN_CLICKED(IDCANCEL, &CLoadSummaryStartUpDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK_APPLY_CONNECTED_TRANSFORMER, &CLoadSummaryStartUpDlg::OnBnClickedCheckApplyConnectedTransformer)
END_MESSAGE_MAP()


// CLoadSummaryStartUpDlg message handlers
static const CString __HEADERS__[]=
	{
		_T("Bus ID") , _T("Bus Voltage") , _T("Load Voltage") , 
		_T("Continuous \n[%]") , _T("Intermittent \n[%]") , _T("Stand-by \n[%]")
	};

/**
	@brief	

	@author BHK	

	@date 2009-05-27 오후 1:09:45	

	@param	

	@return		
*/
BOOL CLoadSummaryStartUpDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
	if(pSettingTable)
	{
		string* pValue = pSettingTable->GetFieldValue(_T("C_APPLY_ALL_BUS_SAME_COINCIDENCE_FACTOR"));
		CLoadSummaryStartUpDlg::__CHECKAPPLYALL__ = ((NULL != pValue) && (_T("YES") == (*pValue))) ? TRUE : FALSE;
		CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_APPLY_ALL);
		if(pButton) pButton->SetCheck(CLoadSummaryStartUpDlg::__CHECKAPPLYALL__);

		pValue = pSettingTable->GetFieldValue(_T("C_APPLY_CONNECTED_TRANSFORMER"));
		docData.m_bApplyTrToLoadSummary = ((NULL != pValue) && (_T("YES") == (*pValue))) ? true : false;
		pButton = (CButton*)GetDlgItem(IDC_CHECK_APPLY_CONNECTED_TRANSFORMER);
		if(pButton) pButton->SetCheck(docData.m_bApplyTrToLoadSummary ? TRUE : FALSE);

		string *rDocumentNo = pSettingTable->GetFieldValue(_T("C_DOCUMENT_NO"));
		if(rDocumentNo) m_wndDocNoEdit.SetWindowText(rDocumentNo->c_str());
	}

	CreateTabCtrl();

	//! BUTTON ICON SETTING
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	m_wndRunButton.SetTextHotColor(TextHotColor);
	m_wndCancelButton.SetTextHotColor(TextHotColor);

	m_wndRunButton.SetIcon(IDI_OK_18X18);
	m_wndCancelButton.SetIcon(IDI_CANCEL_18X18);

	//! Dialog Box Image
	HICON hIcon = AfxGetApp()->LoadIcon(IDI_LOAD_SUMMARY);
	SetIcon(hIcon, FALSE);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control

	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	엔터키로 다이얼로그가 종료되는 것을 막기위해 OnOK를 호출하지 않는다.

	@author	BHK
*/
void CLoadSummaryStartUpDlg::OnBnClickedOK()
{
	///CDialog::OnOK();
}

/**
	@brief	

	@author	HumKyung

	@date	????.??.??

	@param

	@return
*/
void CLoadSummaryStartUpDlg::OnBnClickedRun()
{
	UpdateData(TRUE);

	const int nRowCount = m_GridCtrl.GetRowCount();
	const int nColCount = m_GridCtrl.GetColumnCount();
	for(int i = 1;i < nRowCount;++i)
	{
		CBusItem* pBusItem = (CBusItem*)m_GridCtrl.GetItemData(i , 0);
		if(pBusItem)
		{
			CGridCellBase* pCell = m_GridCtrl.GetCell(i , 3);
			pBusItem->prop()->SetValue(_T("General") , _T("Continuous") , pCell->GetText());
			pCell = m_GridCtrl.GetCell(i , 4);
			pBusItem->prop()->SetValue(_T("General") , _T("Intermittent") , pCell->GetText());
			pCell = m_GridCtrl.GetCell(i , 5);
			pBusItem->prop()->SetValue(_T("General") , _T("Stand-by") , pCell->GetText());
		}
	}

	SubBUSRun();
	
	//! Documnet no를 Project Setting table에 저장한다.
	CString rDocumentNo;
	m_wndDocNoEdit.GetWindowText(rDocumentNo);

	CELoadDocData& docData = CELoadDocData::GetInstance();
	CProjectSettingTable* pSettingTable = (CProjectSettingTable*)docData.GetTableOf(CProjectSettingTable::GetTableName());
	if(pSettingTable)
	{
		pSettingTable->SetFieldValue(_T("C_DOCUMENT_NO"), rDocumentNo.operator LPCSTR());
		
		CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_APPLY_ALL);
		if(pButton && pButton->GetCheck())
			pSettingTable->SetFieldValue(_T("C_APPLY_ALL_BUS_SAME_COINCIDENCE_FACTOR"), _T("YES"));
		else	pSettingTable->SetFieldValue(_T("C_APPLY_ALL_BUS_SAME_COINCIDENCE_FACTOR"), _T("NO"));

		pButton = (CButton*)GetDlgItem(IDC_CHECK_APPLY_CONNECTED_TRANSFORMER);
		if(pButton && pButton->GetCheck())
		{
			pSettingTable->SetFieldValue(_T("C_APPLY_CONNECTED_TRANSFORMER"), _T("YES"));
			docData.m_bApplyTrToLoadSummary = true;
		}
		else
		{
			pSettingTable->SetFieldValue(_T("C_APPLY_CONNECTED_TRANSFORMER"), _T("NO"));
			docData.m_bApplyTrToLoadSummary = false;
		}

		pSettingTable->Save();
	}

	OnOK();
}

void CLoadSummaryStartUpDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

/**
	@brief	GRID CTRL을 BUS ITEM으로 채운다.(INCOMING BUS는 나타내지 않는다.)

	@author	BHK
*/
int CLoadSummaryStartUpDlg::FillGridCtrl(void)
{
	int nIndex = 1;
	vector<CBusItem*> BusItemEntry;
	CELoadDocData& docData = CELoadDocData::GetInstance();
	docData.GetAllBusItemsExceptedSubBUS(BusItemEntry);
	m_GridCtrl.SetRowCount(BusItemEntry.size() + 1);

	const COLORREF ReadOnlyColor = docData.GetColorCode("READ_ONLY_BACK_COLOR");
	for(vector<CBusItem*>::iterator itr = BusItemEntry.begin();itr != BusItemEntry.end();++itr)
	{
		if((*itr)->GetBusGroupName() != SUBBUS)
		{
			const string rBusID = (*itr)->GetName();
			if(INCOMING == rBusID)
			{
				m_GridCtrl.SetRowCount(m_GridCtrl.GetRowCount() - 1);
				continue;
			}

			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = nIndex;
			Item.col = 0;
			Item.nFormat = DT_LEFT|DT_WORDBREAK | DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX;
			Item.strText = rBusID.c_str();
			m_GridCtrl.SetItem(&Item);
			m_GridCtrl.SetItemData(Item.row , Item.col , LPARAM(*itr));
			CGridCellBase* pCell = m_GridCtrl.GetCell(Item.row , Item.col);
			if(pCell)
			{
				pCell->SetState(pCell->GetState() | GVIS_READONLY);
				pCell->SetBackClr(ReadOnlyColor);
			}

			const string rBusVoltage = (*itr)->prop()->GetValue(_T("General") , _T("Bus Voltage"));
			Item.col = 1;
			Item.nFormat = DT_RIGHT|DT_WORDBREAK | DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX;
			Item.strText = rBusVoltage.c_str();
			m_GridCtrl.SetItem(&Item);
			pCell = m_GridCtrl.GetCell(Item.row , Item.col);
			if(pCell)
			{
				pCell->SetState(pCell->GetState() | GVIS_READONLY);
				pCell->SetBackClr(ReadOnlyColor);
			}

			const string rLoadVoltage = (*itr)->prop()->GetValue(_T("General") , _T("Load Voltage"));
			Item.col = 2;
			Item.strText = rLoadVoltage.c_str();
			m_GridCtrl.SetItem(&Item);
			pCell = m_GridCtrl.GetCell(Item.row , Item.col);
			if(pCell)
			{
				pCell->SetState(pCell->GetState() | GVIS_READONLY);
				pCell->SetBackClr(ReadOnlyColor);
			}

			//////////////////////////////////////////////////////////////////////////////////////////
			const string rContinuous = (*itr)->prop()->GetValue(_T("General") , _T("Continuous"));
			Item.col = 3;
			Item.strText = rContinuous.c_str();
			m_GridCtrl.SetItem(&Item);

			const string rIntermittent = (*itr)->prop()->GetValue(_T("General") , _T("Intermittent"));
			Item.col = 4;
			Item.strText = rIntermittent.c_str();
			m_GridCtrl.SetItem(&Item);

			const string rStandby = (*itr)->prop()->GetValue(_T("General") , _T("Stand-by"));
			Item.col = 5;
			Item.strText = rStandby.c_str();
			m_GridCtrl.SetItem(&Item);

			++nIndex;
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
BOOL CLoadSummaryStartUpDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if(wParam == m_GridCtrl.GetDlgCtrlID())
	{
		UpdateData(TRUE);

		CELoadDocData& docData = CELoadDocData::GetInstance();

		NM_GRIDVIEW* pGridView = (NM_GRIDVIEW*)(lParam);
                if(GVN_ENDLABELEDIT == pGridView->hdr.code)
                {
			CGridCellBase* pCell = m_GridCtrl.GetCell(pGridView->iRow, pGridView->iColumn);
			if(pCell)
			{
				CString rValue = pCell->GetText();
				if(CLoadSummaryStartUpDlg::__CHECKAPPLYALL__)
				{
					const int nRowCount = m_GridCtrl.GetRowCount();
					for(int i = 1;i < nRowCount;++i)
					{
						pCell = m_GridCtrl.GetCell(i, pGridView->iColumn);
						if(pCell) pCell->SetText( rValue );
					}
					m_GridCtrl.RedrawWindow();
				}
				docData.SetModifiedFlag(true);
			}
                }
        }
	else if(wParam == m_SubBUSGridCtrl.GetDlgCtrlID())
	{
		UpdateData(TRUE);

		CELoadDocData& docData = CELoadDocData::GetInstance();
		NM_GRIDVIEW* pGridView = (NM_GRIDVIEW*)(lParam);
                if(GVN_ENDLABELEDIT == pGridView->hdr.code)
                {
			CGridCellBase* pCell = m_SubBUSGridCtrl.GetCell(pGridView->iRow, pGridView->iColumn);
			if(pCell)
			{
				CString rValue = pCell->GetText();
				if(CLoadSummaryStartUpDlg::__CHECKAPPLYALL__)
				{
					const int nRowCount = m_SubBUSGridCtrl.GetRowCount();
					for(int i = 1;i < nRowCount;++i)
					{
						pCell = m_SubBUSGridCtrl.GetCell(i, pGridView->iColumn);
						if(pCell) pCell->SetText( rValue );
					}
					m_SubBUSGridCtrl.RedrawWindow();
				}
				docData.SetModifiedFlag(true);
			}
                }
        }

	return CDialog::OnNotify(wParam, lParam, pResult);
}

/**
	@brief	

	@author	BHK
*/
BOOL CLoadSummaryStartUpDlg::PreTranslateMessage(MSG* pMsg)
{
	//! 리턴키나 ESCAPE KEY로 윈도우가 닫기는 것을 방지.
	if((m_GridCtrl.GetSafeHwnd() != pMsg->hwnd) && (FALSE == ::IsChild(m_GridCtrl.GetSafeHwnd() , pMsg->hwnd)) && (m_SubBUSGridCtrl.GetSafeHwnd() != pMsg->hwnd) && (FALSE == ::IsChild(m_SubBUSGridCtrl.GetSafeHwnd() , pMsg->hwnd)) &&
		(pMsg->message == WM_KEYDOWN))
	{
		if(pMsg->wParam == VK_ESCAPE) return TRUE;
		if(pMsg->wParam == VK_RETURN) return TRUE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

/**
	@brief	CREATE TAB CTRL

	@author KHS	

	@return		
**/
int CLoadSummaryStartUpDlg::CreateTabCtrl(void)
{
	CRect rect;
	GetDlgItem(IDC_GRID)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	if (!m_LoadSummaryTab.Create (CMFCTabCtrl::STYLE_3D_VS2005, rect , this, 0x110))
	{
		TRACE0("Failed to create Properies Grid \n");
		return ERROR_BAD_ENVIRONMENT;      // fail to create
	}

	m_LoadSummaryTab.SetWindowPos(&(CWnd::wndTop) , 0,0,0,0,SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE);
	m_LoadSummaryTab.SetLocation(CMFCTabCtrl::LOCATION_TOP);
	m_LoadSummaryTab.SetActiveTabBoldFont(TRUE);
	m_LoadSummaryTab.EnableAutoColor(TRUE);

	if(ERROR_SUCCESS == CreateBusGridCtrl())
	{
		m_LoadSummaryTab.AddTab(&m_GridCtrl,_T("BUS"));
	}
	if(ERROR_SUCCESS == CreateSubBUSGridCtrl())
	{
		m_LoadSummaryTab.AddTab(&m_SubBUSGridCtrl , SUBBUS);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	CREATE SubBUS GRID CTRL

	@author KHS	

	@return		
**/
int CLoadSummaryStartUpDlg::CreateSubBUSGridCtrl(void)
{
	CRect rect;
	rect.SetRectEmpty();
	if(m_SubBUSGridCtrl.Create(rect , &m_LoadSummaryTab , 102))
	{
		m_SubBUSGridCtrl.SetColumnCount(sizeof(__HEADERS__) / sizeof(__HEADERS__[0]));
		m_SubBUSGridCtrl.SetFixedRowCount(1);

		m_SubBUSGridCtrl.SetEditable(TRUE);
		m_SubBUSGridCtrl.SetRowResize(FALSE);
		m_SubBUSGridCtrl.SetColumnResize(TRUE);
		
		// 해더
		const int nColumnCount = m_SubBUSGridCtrl.GetColumnCount();
		for(int i = 0;i < nColumnCount;++i)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = 0;
			Item.col = i;
			Item.nFormat = DT_LEFT|DT_WORDBREAK | DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX;
			Item.strText = __HEADERS__[i];
			m_SubBUSGridCtrl.SetItem(&Item);
		}

		FillPdbGridCtrl();

		m_SubBUSGridCtrl.AutoSizeColumns();
		m_SubBUSGridCtrl.AutoSizeRows();

		return ERROR_SUCCESS;
	}
	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	SubBUS GRID CTRL에 데이타를 채운다.

	@author KHS	

	@return		
**/
int CLoadSummaryStartUpDlg::FillPdbGridCtrl(void)
{
	int nIndex = 1;
	vector<CBusItem*> PdbItemEntry;
	CELoadDocData& docData = CELoadDocData::GetInstance();
	docData.GetAllSubBUSItems(PdbItemEntry);
	m_SubBUSGridCtrl.SetRowCount(PdbItemEntry.size() + 1);

	const COLORREF ReadOnlyColor = docData.GetColorCode("READ_ONLY_BACK_COLOR");
	for(vector<CBusItem*>::iterator itr = PdbItemEntry.begin();itr != PdbItemEntry.end();++itr)
	{
		if((*itr)->GetBusGroupName() == SUBBUS)
		{
			const string rBusID = (*itr)->GetName();
			if(INCOMING == rBusID)
			{
				m_SubBUSGridCtrl.SetRowCount(m_SubBUSGridCtrl.GetRowCount() - 1);
				continue;
			}

			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = nIndex;
			Item.col = 0;
			Item.nFormat = DT_LEFT|DT_WORDBREAK | DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX;
			Item.strText = rBusID.c_str();
			m_SubBUSGridCtrl.SetItem(&Item);
			m_SubBUSGridCtrl.SetItemData(Item.row , Item.col , LPARAM(*itr));
			CGridCellBase* pCell = m_SubBUSGridCtrl.GetCell(Item.row , Item.col);
			if(pCell)
			{
				pCell->SetState(pCell->GetState() | GVIS_READONLY);
				pCell->SetBackClr(ReadOnlyColor);
			}

			const string rBusVoltage = (*itr)->prop()->GetValue(_T("General") , _T("Bus Voltage"));
			Item.col = 1;
			Item.nFormat = DT_RIGHT|DT_WORDBREAK | DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX;
			Item.strText = rBusVoltage.c_str();
			m_SubBUSGridCtrl.SetItem(&Item);
			pCell = m_SubBUSGridCtrl.GetCell(Item.row , Item.col);
			if(pCell)
			{
				pCell->SetState(pCell->GetState() | GVIS_READONLY);
				pCell->SetBackClr(ReadOnlyColor);
			}

			const string rLoadVoltage = (*itr)->prop()->GetValue(_T("General") , _T("Load Voltage"));
			Item.col = 2;
			Item.strText = rLoadVoltage.c_str();
			m_SubBUSGridCtrl.SetItem(&Item);
			pCell = m_SubBUSGridCtrl.GetCell(Item.row , Item.col);
			if(pCell)
			{
				pCell->SetState(pCell->GetState() | GVIS_READONLY);
				pCell->SetBackClr(ReadOnlyColor);
			}

			//////////////////////////////////////////////////////////////////////////////////////////
			const string rContinuous = (*itr)->prop()->GetValue(_T("General") , _T("Continuous"));
			Item.col = 3;
			Item.strText = rContinuous.c_str();
			m_SubBUSGridCtrl.SetItem(&Item);

			const string rIntermittent = (*itr)->prop()->GetValue(_T("General") , _T("Intermittent"));
			Item.col = 4;
			Item.strText = rIntermittent.c_str();
			m_SubBUSGridCtrl.SetItem(&Item);

			const string rStandby = (*itr)->prop()->GetValue(_T("General") , _T("Stand-by"));
			Item.col = 5;
			Item.strText = rStandby.c_str();
			m_SubBUSGridCtrl.SetItem(&Item);

			++nIndex;
		}
	}

	return ERROR_SUCCESS;
}
/**
	@brief	CREATE BUS GRID CTRL

	@author KHS	

	@return		
**/
int CLoadSummaryStartUpDlg::CreateBusGridCtrl(void)
{
	CRect rect;
	rect.SetRectEmpty();
	///GetDlgItem(IDC_GRID)->GetWindowRect(&rect);
	///ScreenToClient(&rect);
	if(m_GridCtrl.Create(rect , &m_LoadSummaryTab , 101))
	{
		m_GridCtrl.SetColumnCount(sizeof(__HEADERS__) / sizeof(__HEADERS__[0]));
		m_GridCtrl.SetFixedRowCount(1);

		m_GridCtrl.SetEditable(TRUE);
		m_GridCtrl.SetRowResize(FALSE);
		m_GridCtrl.SetColumnResize(TRUE);
		
		// 해더
		const int nColumnCount = m_GridCtrl.GetColumnCount();
		for(int i = 0;i < nColumnCount;++i)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = 0;
			Item.col = i;
			Item.nFormat = DT_LEFT|DT_WORDBREAK | DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX;
			Item.strText = __HEADERS__[i];
			m_GridCtrl.SetItem(&Item);
		}

		FillGridCtrl();

		m_GridCtrl.AutoSizeColumns();
		m_GridCtrl.AutoSizeRows();

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	SubBUS GRID 데이타 저장

	@author KHS	

	@return		
**/
int CLoadSummaryStartUpDlg::SubBUSRun(void)
{
	UpdateData(TRUE);

	const int nRowCount = m_SubBUSGridCtrl.GetRowCount();
	const int nColCount = m_SubBUSGridCtrl.GetColumnCount();

	for(int i = 1;i < nRowCount;++i)
	{
		CBusItem* pBusItem = (CBusItem*)m_SubBUSGridCtrl.GetItemData(i , 0);
		if(pBusItem)
		{
			CGridCellBase* pCell = m_SubBUSGridCtrl.GetCell(i , 3);
			pBusItem->prop()->SetValue(_T("General") , _T("Continuous") , pCell->GetText());
			pCell = m_SubBUSGridCtrl.GetCell(i , 4);
			pBusItem->prop()->SetValue(_T("General") , _T("Intermittent") , pCell->GetText());
			pCell = m_SubBUSGridCtrl.GetCell(i , 5);
			pBusItem->prop()->SetValue(_T("General") , _T("Stand-by") , pCell->GetText());
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-27 오후 1:11:19	

	@param	

	@return		
*/
void CLoadSummaryStartUpDlg::OnBnClickedCheckApplyConnectedTransformer()
{
	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_APPLY_CONNECTED_TRANSFORMER);
	if(pButton)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		docData.m_bApplyTrToLoadSummary = pButton->GetCheck();
	}
}
