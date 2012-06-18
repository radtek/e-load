// LoadListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "LoadListDlg.h"
#include "ELoadDocData.h"
#include "PropertiesWnd.h"
#include "GeneratorPropDlg.h"

// CLoadListDlg dialog

IMPLEMENT_DYNAMIC(CLoadListDlg, CDialog)

CLoadListDlg::CLoadListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoadListDlg::IDD, pParent) , m_pGeneratorItem(NULL)
{

}

CLoadListDlg::~CLoadListDlg()
{
}

void CLoadListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_LIST_LOAD , m_wndLoadListCtrl);
}


BEGIN_MESSAGE_MAP(CLoadListDlg, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_LIST_LOAD, &CLoadListDlg::OnNMClickListLoad)
END_MESSAGE_MAP()


// CLoadListDlg message handlers

/**
	@brief	BUS에 들어 있는 LOAD를 표시한다.

	@author BHK	

	@date 2009-05-27 오후 6:54:18	

	@param	

	@return		
*/
int CLoadListDlg::DisplayLoadList(const string& rBusID , const bool& bDisplayOnlySelectedLoad)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CBusItem* pBusItem = docData.FindBusItemByName(rBusID);
	if(pBusItem && m_pGeneratorItem)
	{
		m_wndLoadListCtrl.DeleteAllItems();
		
		vector<string> LoadItemEntry;
		CItemSelectionSet SelectionSet;
		docData.GetLoadItemSetInBus(SelectionSet , rBusID);
		for(int i = 0;i < SelectionSet.GetItemCount();++i)
		{
			CLoadItem* pLoadItem = static_cast<CLoadItem*>(SelectionSet.GetItemAt(i));
			LoadItemEntry.push_back(pLoadItem->GetName());
		}
		if(bDisplayOnlySelectedLoad)
		{
			const string& rSelectedLoadList = m_pGeneratorItem->prop()->GetValue(_T("General") , _T("Selected Load List"));
			if(!rSelectedLoadList.empty())
			{
				CTokenizer<CIsComma>::Tokenize(LoadItemEntry , rSelectedLoadList , CIsComma());
			}
		}

		CString rValue;
		for(int i = 0;i < SelectionSet.GetItemCount();++i)
		{
			const string rLoadID = SelectionSet.GetItemAt(i)->GetName();
			CLoadItem* pLoadItem = docData.FindLoadItemByName(rLoadID);
			if(pLoadItem)
			{
				const int nItem = m_wndLoadListCtrl.InsertItem(i , rLoadID.c_str());
				if(-1 != nItem)
				{
					const string& rLoadCategory = pLoadItem->prop()->GetValue(_T("Type") , _T("Load Category"));
					m_wndLoadListCtrl.SetItemText(nItem , 1 , rLoadCategory.c_str());

					const string& rRatingCapacity = pLoadItem->prop()->GetValue(_T("Rating") , _T("Rating Capacity"));
					m_wndLoadListCtrl.SetItemText(nItem , 2 , rRatingCapacity.c_str());
					const string& rBHP = pLoadItem->prop()->GetValue(_T("Load") , _T("BHP <1>"));
					m_wndLoadListCtrl.SetItemText(nItem , 3 , rBHP.c_str());
					
					vector<string>::iterator where = find(LoadItemEntry.begin() , LoadItemEntry.end() , rLoadID);
					m_wndLoadListCtrl.SetCheck(nItem , (where != LoadItemEntry.end()));

					rValue += CString(rLoadID.c_str()) + _T(",");
				}
			}
		}
		if(_T(",") == rValue.Right(1)) rValue = rValue.Left(rValue.GetLength() - 1);
		m_pGeneratorItem->prop()->SetValue(_T("General") , _T("Selected Load List") , rValue.operator LPCSTR());
	}

	return 0;
}

/**
	@brief	

	@author BHK	

	@date 2009-05-27 오후 6:58:26	

	@param	

	@return		
*/
BOOL CLoadListDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_wndLoadListCtrl.SetExtendedStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);
	m_wndLoadListCtrl.InsertColumn(0 , _T("Load ID") , LVCFMT_LEFT , 200);
	m_wndLoadListCtrl.InsertColumn(1 , _T("Category") , LVCFMT_LEFT , 100);
	m_wndLoadListCtrl.InsertColumn(2 , _T("KW") , LVCFMT_LEFT , 100);
	m_wndLoadListCtrl.InsertColumn(3 , _T("BHP") , LVCFMT_LEFT , 50);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	선택한 Load ID를 리스트로 Selected Load List에 저장한다.

	@author BHK	

	@date 2009-05-27 오후 7:13:25	

	@param	

	@return		
*/
void CLoadListDlg::OnNMClickListLoad(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	CPoint point;
	GetCursorPos(&point);
	m_wndLoadListCtrl.ScreenToClient(&point);

	UINT uFlags;
	const int nItem = m_wndLoadListCtrl.HitTest(point , &uFlags);
	if((-1 != nItem) && (uFlags & LVHT_ONITEMSTATEICON))	//! ICON부분을 클릭 했을때...
	{
		m_wndLoadListCtrl.SetCheck(nItem , !m_wndLoadListCtrl.GetCheck(nItem));

		CPropertiesWnd* pPropWnd = (CPropertiesWnd*)GetParent()->GetParent()->GetParent();
		if(pPropWnd)
		{
			CGeneratorItem* pGeneratorItem = (CGeneratorItem*)pPropWnd->GetSelectionSet().GetItemAt(0);
			if(pGeneratorItem)
			{
				CString rValue;
				const int nItemCount = m_wndLoadListCtrl.GetItemCount();
				for(int i = 0;i < nItemCount;++i)
				{
					if(TRUE == m_wndLoadListCtrl.GetCheck(i))
					{
						rValue += m_wndLoadListCtrl.GetItemText(i , 0) + _T(",");
					}
				}
				if(_T(",") == rValue.Right(1)) rValue = rValue.Left(rValue.GetLength() - 1);
				pGeneratorItem->prop()->SetValue(_T("General") , _T("Selected Load List") , rValue.operator LPCSTR());
				
				//! Generator Property를 계산한다.
				CGeneratorPropDlg *pPropDlg = (CGeneratorPropDlg*)(GetParent()->GetParent());
				if(pPropDlg)
				{
					const string rBusID = pGeneratorItem->prop()->GetValue(_T("To") , _T("Bus Id"));
					pPropDlg->CalcGeneratorProp(rBusID);
				}
			}
		}
	}

	*pResult = 1;
}
