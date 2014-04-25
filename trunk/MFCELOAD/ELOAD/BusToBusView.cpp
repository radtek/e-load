// BusToBusView.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "MainFrm.h"
#include <gui/GridCtrl/GridCellCheck.h>
#include "ELoadDocData.h"

#include "BusDiagramChildFrame.h"
#include "BusToBusView.h"

#define	BUTTON_HEIGHT	32
#define	BUTTON_WIDTH	80

/////////////////////////////////////////////////////////////////////////////
// CGridCellControlTypeCombo 
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CGridCellBusGroupCombo, CGridCellCombo)

CGridCellBusGroupCombo::CGridCellBusGroupCombo() : CGridCellCombo()
{
}

/**
	@breif	compare entity names

	@author	HumKyung

	@date	2011.02.08

	@param

	@return
*/
static bool fnCompareName(CELoadItem* lhs , CELoadItem* rhs)
{
	return (lhs->GetName().compare(rhs->GetName()) < 0);
}

// Create a control to do the editing
/*
	@brief	

	@author	HumKyung

	@date	2011.02.08

	@param	
	@param	
	@param	
	@param
	@param
	@param

	@return	BOOL
*/
BOOL CGridCellBusGroupCombo::Edit(int nRow, int nCol, CRect rect, CPoint point , UINT nID, UINT nChar)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> BusGroupItemList;
	docData.GetELoadItemListOf(BusGroupItemList , CBusGroupItem::TypeString());
	if(!BusGroupItemList.empty())
	{
		::stable_sort(BusGroupItemList.begin() , BusGroupItemList.end() , fnCompareName);

		m_Strings.RemoveAll();
		for(list<CELoadItem*>::iterator itr = BusGroupItemList.begin(); itr != BusGroupItemList.end();++itr)
		{
			const string sName = (*itr)->GetName();
			m_Strings.Add(sName.c_str());
		}
	}
	CGridCellCombo::Edit(nRow , nCol , rect , point , nID , nChar);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGridCellControlTypeCombo 
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CGridCellBusCombo, CGridCellCombo)

CGridCellBusCombo::CGridCellBusCombo() : CGridCellCombo() , m_sBusGroupName(_T("*"))
{
}

// Create a control to do the editing
/*
	@brief	

	@author	HumKyung

	@date	????.??.??

	@param	
	@param	
	@param	
	@param
	@param
	@param

	@return	BOOL
*/
BOOL CGridCellBusCombo::Edit(int nRow, int nCol, CRect rect, CPoint point , UINT nID, UINT nChar)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> BusItemList;
	docData.GetELoadItemListOf(BusItemList , CBusItem::TypeString());
	if(!BusItemList.empty())
	{
		//! 2011.01.10 - added by HumKyung
		::stable_sort(BusItemList.begin() , BusItemList.end() , fnCompareName);

		m_Strings.RemoveAll();
		for(list<CELoadItem*>::iterator itr = BusItemList.begin(); itr != BusItemList.end();++itr)
		{
			//! compare bus group names - 2011.02.08 - added by HumKyung
			if(_T("*") != m_sBusGroupName)
			{
				const string sBusGroupName = (*itr)->prop()->GetValue(_T("General") , _T("Bus Group"));
				if(0 != m_sBusGroupName.CompareNoCase(sBusGroupName.c_str())) continue;
			}
			//!

			const STRING_T sName = (*itr)->GetName();
			if(this->m_sFromBus != sName.c_str())	/// add if it's not from bus - 2012.07.10 added by humkyung
			{
				m_Strings.Add(sName.c_str());
			}
		}
	}
	CGridCellCombo::Edit(nRow , nCol , rect , point , nID , nChar);

	return TRUE;
}

/*
	@brief	

	@author	HumKyung

	@date	2011.02.08

	@param	
	
	@return	int
*/
int CGridCellBusCombo::SetBusGroupName(const CString& sBusGroupName)
{
	m_sBusGroupName = sBusGroupName;
	EndEdit();
	const CString sBusName(GetText());

	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> BusItemList;
	docData.GetELoadItemListOf(BusItemList , CBusItem::TypeString());
	if(!BusItemList.empty())
	{
		::stable_sort(BusItemList.begin() , BusItemList.end() , fnCompareName);

		bool bFoundMatchBusName = false;
		m_Strings.RemoveAll();
		for(list<CELoadItem*>::iterator itr = BusItemList.begin(); itr != BusItemList.end();++itr)
		{
			if(_T("*") != m_sBusGroupName)
			{
				const string sBusGroupName = (*itr)->prop()->GetValue(_T("General") , _T("Bus Group"));
				if(0 != m_sBusGroupName.CompareNoCase(sBusGroupName.c_str())) continue;
			}
			//!

			const string sName = (*itr)->GetName();
			if(0 == sBusName.CompareNoCase(sName.c_str()))
			{
				bFoundMatchBusName = true;
			}

			m_Strings.Add(sName.c_str());
		}

		if(false == bFoundMatchBusName)
		{
			SetText(_T(""));
		}
	}

	return ERROR_SUCCESS;
}

// CBusToBusView

IMPLEMENT_DYNCREATE(CBusToBusView, CView)

CBusToBusView::CBusToBusView()/// : m_pWndDrawCtrl(NULL) , m_pBusToBusRelationFactory(NULL)
{
	///m_pBusToBusRelationFactory = new BusToBus::CBusToBusRelationFactory;
}

CBusToBusView::~CBusToBusView()
{
	try
	{
		///if(m_pWndDrawCtrl) SAFE_DELETE(m_pWndDrawCtrl);
		///if(m_pBusToBusRelationFactory) SAFE_DELETE(m_pBusToBusRelationFactory);
	}
	catch(...)
	{
	}
}

BEGIN_MESSAGE_MAP(CBusToBusView, CView)
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	///ON_COMMAND(ID_TOOL_CREATE_BUS_TO_BUS_RELATION , &CBusToBusView::OnCreateBusToBusRelation)
	ON_BN_CLICKED(0x102, &CBusToBusView::OnBnClickedAdd)
	ON_BN_CLICKED(0x103, &CBusToBusView::OnBnClickedDelete)
	ON_BN_CLICKED(ID_TOOL_SAVE_BUS_TO_BUS_RELATION, &CBusToBusView::OnBnClickedSave)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CBusToBusView drawing

void CBusToBusView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
}


// CBusToBusView diagnostics

#ifdef _DEBUG
void CBusToBusView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CBusToBusView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CBusToBusView message handlers

/**
	@brief	

	@author	HumKyung

	@date	2010.12.23

	@param

	@return
*/
void CBusToBusView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if(m_wndGridCtrl.GetSafeHwnd())
	{
		m_wndGridCtrl.SetWindowPos(NULL , 0 , 0 , cx , cy - BUTTON_HEIGHT - 3 , SWP_NOZORDER);
	}

	if(m_wndAddButton.GetSafeHwnd())
	{
		m_wndAddButton.SetWindowPos(NULL , 0 , cy - BUTTON_HEIGHT , BUTTON_WIDTH , BUTTON_HEIGHT , SWP_NOZORDER);
	}

	if(m_wndDeleteButton.GetSafeHwnd())
	{
		m_wndDeleteButton.SetWindowPos(NULL , BUTTON_WIDTH + 2 , cy - BUTTON_HEIGHT , BUTTON_WIDTH , BUTTON_HEIGHT , SWP_NOZORDER);
	}

	if(m_wndSaveButton.GetSafeHwnd())
	{
		m_wndSaveButton.SetWindowPos(NULL , (BUTTON_WIDTH + 2) * 2 , cy - BUTTON_HEIGHT , BUTTON_WIDTH , BUTTON_HEIGHT , SWP_NOZORDER);
	}
}

/**
	@brief	

	@author	HumKyung

	@date	2010.12.23

	@param

	@return
*/
void CBusToBusView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	CRect rect;
	rect.SetRectEmpty();
	if(NULL == m_wndGridCtrl.GetSafeHwnd())
	{
		if(TRUE == m_wndGridCtrl.Create(rect , this , 0x101))
		{
			m_wndGridCtrl.SetFixedRowCount(1);
			m_wndGridCtrl.SetColumnCount(3);

			m_wndGridCtrl.SetItemText(0 , 0 , _T(""));
			m_wndGridCtrl.SetItemText(0 , 1 , _T("From"));
			m_wndGridCtrl.SetItemText(0 , 2 , _T("To"));

			CELoadDocData& docData = CELoadDocData::GetInstance();
			CBusToBusDoc* pDoc = (CBusToBusDoc*)GetDocument();
			/// create bus to bus relations
			list<CELoadItem*> TransformerItemList;
			docData.GetELoadItemListOf(TransformerItemList , CTransformerItem::TypeString());
			for(list<CELoadItem*>::iterator itr = TransformerItemList.begin();itr != TransformerItemList.end();++itr)
			{
				CTransformerItem* pTrItem = static_cast<CTransformerItem*>(*itr);
				if(0 == pTrItem->GetName().find(_T("DEL ")))
				{
					m_wndGridCtrl.SetRowCount( m_wndGridCtrl.GetRowCount() + 1 );
					const int iRow = m_wndGridCtrl.GetRowCount() - 1;
	
					m_wndGridCtrl.SetCellType(iRow , 0 , RUNTIME_CLASS(CGridCellCheck));

					///CBusEntity *pFromBusEnt = NULL , *pToBusEnt = NULL;
					const string sFromBusName = pTrItem->prop()->GetValue(_T("From") , _T("Bus Id"));
					m_wndGridCtrl.SetItemText(iRow , 1 , sFromBusName.c_str());
					m_wndGridCtrl.SetCellType(iRow , 1 , RUNTIME_CLASS(CGridCellBusCombo));
					CGridCellBusCombo* pCell = (CGridCellBusCombo*)(m_wndGridCtrl.GetCell(iRow , 1));
					if(pCell) pCell->SetStyle(CBS_DROPDOWNLIST);
					
					const string sToBusName   = pTrItem->prop()->GetValue(_T("To") , _T("Bus Id"));
					m_wndGridCtrl.SetItemText(iRow , 2 , sToBusName.c_str());
					m_wndGridCtrl.SetCellType(iRow , 2 , RUNTIME_CLASS(CGridCellBusCombo));
					pCell = (CGridCellBusCombo*)(m_wndGridCtrl.GetCell(iRow , 2));
					if(pCell) pCell->SetStyle(CBS_DROPDOWNLIST);

					m_wndGridCtrl.SetItemData(iRow , 0 , LPARAM(pTrItem));
				}
			}

			m_wndGridCtrl.AutoSize();
			m_wndGridCtrl.SetColumnWidth(0 , 30);
			m_wndGridCtrl.SetColumnWidth(1 , 120);
			m_wndGridCtrl.SetColumnWidth(2 , 120);
		}
	}

	if(NULL == m_wndAddButton.GetSafeHwnd())
	{
		if(TRUE == m_wndAddButton.Create(_T("Add") , WS_CHILD | WS_VISIBLE , rect , this , 0x102)){}
	}

	if(NULL == m_wndDeleteButton.GetSafeHwnd())
	{
		if(TRUE == m_wndDeleteButton.Create(_T("Delete") , WS_CHILD | WS_VISIBLE , rect , this , 0x103)){}
	}

	if(NULL == m_wndSaveButton.GetSafeHwnd())
	{
		if(TRUE == m_wndSaveButton.Create(_T("Save") , WS_CHILD | WS_VISIBLE , rect , this , ID_TOOL_SAVE_BUS_TO_BUS_RELATION))
		{
			m_wndSaveButton.SetIcon(IDI_SAVE);
		}
	}
}

/**
	@brief	

	@author	HumKyung

	@date	2010.12.27

	@param

	@return
*/
void CBusToBusView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_CONTEXT_BUS_TO_BUS_MENU,
				point.x, point.y, this, TRUE);
}

/**
	@brief	callback function
	@author	HumKyung
	@date	2011.01.05
	@param
	@return
*/
void CBusToBusView::OnBnClickedAdd()
{
	if(m_wndGridCtrl.GetSafeHwnd())
	{
		m_wndGridCtrl.SetRowCount( m_wndGridCtrl.GetRowCount() + 1 );
		const int iRow = m_wndGridCtrl.GetRowCount() - 1;
		m_wndGridCtrl.SetCellType(iRow , 0 , RUNTIME_CLASS(CGridCellCheck));

		m_wndGridCtrl.SetCellType(iRow , 1 , RUNTIME_CLASS(CGridCellBusCombo));
		CGridCellBusCombo* pCell = (CGridCellBusCombo*)(m_wndGridCtrl.GetCell(iRow , 1));
		if(pCell) pCell->SetStyle(CBS_DROPDOWNLIST);

		m_wndGridCtrl.SetCellType(iRow , 2 , RUNTIME_CLASS(CGridCellBusCombo));
		pCell = (CGridCellBusCombo*)(m_wndGridCtrl.GetCell(iRow , 2));
		if(pCell) pCell->SetStyle(CBS_DROPDOWNLIST);

		m_wndGridCtrl.AutoSizeRows();

		GetDocument()->SetModifiedFlag();	/// 2014.04.04 added by humkyung
	}
}

/**
	@brief	callback function

	@author	HumKyung

	@date	2011.01.05

	@param

	@return
*/
void CBusToBusView::OnBnClickedDelete()
{
	if(m_wndGridCtrl.GetSafeHwnd())
	{
		if(IDYES == AfxMessageBox(_T("Check한 아이템을 삭제할까요?") , MB_YESNO|MB_ICONWARNING))
		{
			while(1)
			{
				bool bDeleted = false;
				const int iRowCount = m_wndGridCtrl.GetRowCount();
				for(int i = iRowCount - 1;i > 0;--i)
				{
					CGridCellCheck* pCellCheck = (CGridCellCheck*)(m_wndGridCtrl.GetCell(i , 0));
					if(pCellCheck)
					{
						if(pCellCheck->GetCheck())
						{
							m_wndGridCtrl.DeleteRow(i);
							bDeleted = true;
							break;
						}
					}
				}

				if(false == bDeleted) break;
			}

			m_wndGridCtrl.RedrawWindow();

			GetDocument()->SetModifiedFlag();	/// 2014.04.04 added by humkyung
		}
	}
}

/**
	@brief	call back function
	@author	HumKyung
	@date	2011.01.05
	@param
	@return
*/
void CBusToBusView::OnBnClickedSave()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const STRING_T sProjectMDBFilePath = docData.GetProjectMDBFilePath();
	/// MDB CONNECT
	CADODB adoDB;
	const STRING_T sDBPath = STRING_T(PROVIDER) + _T("Data Source=") + sProjectMDBFilePath  + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(sDBPath))
	{
		list<CELoadItem*> TrItemList;
		docData.GetELoadItemListOf(TrItemList , CTransformerItem::TypeString());

		const int iRowCount = m_wndGridCtrl.GetRowCount();
		for(int i = 1;i < iRowCount;++i)
		{
			CString sFromBus = m_wndGridCtrl.GetItemText(i , 1);
			CString sToBus   = m_wndGridCtrl.GetItemText(i , 2);
			if(sFromBus.IsEmpty() || sToBus.IsEmpty()) continue;

			CBusItem* pFromBus = (CBusItem*)(docData.FindItemByName(CBusItem::TypeString() , sFromBus.operator LPCSTR()));
			CBusItem* pToBus = (CBusItem*)(docData.FindItemByName(CBusItem::TypeString() , sToBus.operator LPCSTR()));

			CTransformerItem* pTrItem = (CTransformerItem*)(m_wndGridCtrl.GetItemData(i , 0));
			if(pTrItem)
			{
				pTrItem->prop()->SetValue(_T("From") , _T("Bus Id") , sFromBus.operator LPCSTR());
				pTrItem->prop()->SetValue(_T("To") , _T("Bus Id") , sToBus.operator LPCSTR());

				/// remove transformer item from list
				list<CELoadItem*>::iterator where = find(TrItemList.begin() , TrItemList.end() , pTrItem);
				if(where != TrItemList.end()) TrItemList.erase(where);
			}
			else
			{
				/// create a tranformer and insert a transformer record.
				pTrItem = static_cast<CTransformerItem*>(docData.CreateELoadItemOf(CTransformerItem::TypeString()));
				if(pTrItem)
				{
					const string sName(string(_T("DEL ")) + sFromBus.operator LPCSTR() + string(_T("_")) + sToBus.operator LPCSTR());
					pTrItem->SetName(sName);
					pTrItem->prop()->SetValue(_T("From") , _T("Bus Id") , sFromBus.operator LPCSTR());
					pTrItem->prop()->SetValue(_T("To") , _T("Bus Id") , sToBus.operator LPCSTR());

					CELoadItem* pFrom = docData.FindItemByName(CBusItem::TypeString() , sFromBus.operator LPCSTR());
					if(pFrom)
					{
						string sValue = pFrom->prop()->GetValue(_T("General") , _T("Bus Voltage"));
						pTrItem->prop()->SetValue(_T("From") , _T("Bus Volt") , sValue);
						sValue = pFrom->prop()->GetValue(_T("General") , _T("Load Voltage"));
						pTrItem->prop()->SetValue(_T("From") , _T("Load Volt") , sValue);
					}
					CELoadItem* pTo = docData.FindItemByName(CBusItem::TypeString() , sToBus.operator LPCSTR());
					if(pTo)
					{
						string sValue = pFrom->prop()->GetValue(_T("General") , _T("Bus Voltage"));
						pTrItem->prop()->SetValue(_T("To") , _T("Bus Volt") , sValue);
						sValue = pFrom->prop()->GetValue(_T("General") , _T("Load Voltage"));
						pTrItem->prop()->SetValue(_T("To") , _T("Load Volt") , sValue);
					}

					const double dCapacity = pTrItem->CalculateCapacity();
					stringstream oss;
					oss << dCapacity;
					pTrItem->prop()->SetValue(_T("Capacity") , _T("Calculated Capacity") , oss.str());
					pTrItem->prop()->SetValue(_T("Capacity") , _T("Selected Capacity") , _T("0"));

					m_wndGridCtrl.SetItemData(i , 0 , LPARAM(pTrItem));
				}
			}
			if(pTrItem) pTrItem->SaveData(adoDB , CTransformerItem::TableName());
		}

		/// delete database record which's item is deleted
		for(list<CELoadItem*>::iterator itr = TrItemList.begin();itr != TrItemList.end();++itr)
		{
			CTransformerItem* pTrItem = (CTransformerItem*)(*itr);
			if(0 != pTrItem->GetName().find(_T("DEL "))) continue;

			/// delete transformer record from table
			const string sSql = _T("DELETE * FROM ") + CTransformerItem::TableName() + _T(" WHERE C_ID='") + string(pTrItem->id()) + _T("'");
			adoDB.ExecuteQuery(sSql.c_str());

			pTrItem->SetDeleteFlag(true);
		}

		adoDB.DBDisConnect();
		GetDocument()->SetModifiedFlag(FALSE);	/// 2014.04.04 added by humkyung
	}

	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	if(pApp) pApp->OpenNewDocumentIfNeed(CBusItem::TypeString());

	AfxMessageBox(_T("Save is done."));
}

/**
	@brief	store view's width
	@author	HumKyung
	@date	2011.01.27
	@param
	@return
*/
void CBusToBusView::OnDestroy()
{
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	CRect rect;
	GetWindowRect(&rect);
	CString sWidth;
	sWidth.Format(_T("%d") , rect.Width());

	const CString sIniFilePath = GetExecPath() + _T("Setting\\ELoad.ini");
	WritePrivateProfileString(_T("ELoad") , _T("BusToBus_Width") , sWidth , sIniFilePath);

	GetDocument()->OnSaveDocument(NULL);

	CView::OnDestroy();
}

/******************************************************************************
    @author     humkyung
    @date       2012-07-10
    @class      CBusToBusView
    @function   OnNotify
    @return     BOOL
    @param      WPARAM      wParam
    @param      LPARAM      lParam
    @param      LRESULT*    pResult
    @brief
******************************************************************************/
BOOL CBusToBusView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if(wParam == m_wndGridCtrl.GetDlgCtrlID())
	{
		NM_GRIDVIEW* pGridView = (NM_GRIDVIEW*)(lParam);

		if((GVN_ENDLABELEDIT == pGridView->hdr.code) && (1 == pGridView->iColumn))
		{
			CGridCellBase* pCell = m_wndGridCtrl.GetCell(pGridView->iRow, pGridView->iColumn);
			CString sFromBus = pCell->GetText();

			CGridCellBusCombo* pBusCombo = (CGridCellBusCombo*)m_wndGridCtrl.GetCell(pGridView->iRow, pGridView->iColumn + 1);
			CString sToBus = pBusCombo->GetText();

			if(sFromBus == sToBus)
			{
				AfxMessageBox(_T("From and To Bus are same") , MB_OK | MB_ICONWARNING);
				m_wndGridCtrl.SetSelectedRange(pGridView->iRow , pGridView->iColumn,pGridView->iRow , pGridView->iColumn);
				m_wndGridCtrl.SetFocusCell(pGridView->iRow , pGridView->iColumn);
				return TRUE;
			}

			pBusCombo->m_sFromBus = sFromBus;

			GetDocument()->SetModifiedFlag(TRUE);
		}

		return TRUE;
	}
	
	return CView::OnNotify(wParam, lParam, pResult);
}
