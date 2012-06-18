// DrumScheduleCreationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "DrumScheduleCreationDlg.h"
#include "ELoadDocData.h"
#include "DrumItem.h"
#include <set>
#include <string>
#include <vector>
#include <Tokenizer.h>

using namespace std;

static int CALLBACK CompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{

	int res = 0;

	CTreeListCtrlEx* pmyTreeCtrl = (CTreeListCtrlEx*)lParamSort;
	if(pmyTreeCtrl)
	{
		for(int i = 1; i <= 4;i++)
		{
			CString strItem1 = pmyTreeCtrl->GetItemText((HTREEITEM) lParam1, i);
			CString strItem2 = pmyTreeCtrl->GetItemText((HTREEITEM) lParam2, i);
			if(4 <= i)
			{
				if(atof(strItem1) > atof(strItem2)) return 1;
				else if(atof(strItem1) < atof(strItem2)) return -1;
				else continue;
			}
			else if(3 == i )
			{
				strItem1.Replace("C", "");
				strItem2.Replace("C", "");
				if(atoi(strItem1) > atoi(strItem2)) return 1;
				else if(atoi(strItem1) < atoi(strItem2)) return -1;
				else continue;

			}
			else if(1 == i)
			{
				vector<string> oResult1, oResult2;
				CTokenizer<CIsFromString>::Tokenize(oResult1, strItem1.operator LPCSTR(), CIsFromString("/"));
				CTokenizer<CIsFromString>::Tokenize(oResult2, strItem2.operator LPCSTR(), CIsFromString("/"));

				if(atof(oResult1[0].c_str()) < atof(oResult2[0].c_str())) return 1;
				else if(atof(oResult1[0].c_str()) > atof(oResult2[0].c_str())) return -1;
				else continue;
			}
			else if(2 == i)
			{
				res = strcmp(strItem1, strItem2);
				if(0 != res) return res;
			}
		}
	}

	return res;
}

int CALLBACK CompareFunc2(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CListCtrl *pListCtrl = (CListCtrl*)lParamSort;

	LVFINDINFO info1, info2;
	
	info1.flags = LVFI_PARAM;
	info1.lParam = lParam1;

	info2.flags = LVFI_PARAM;
	info2.lParam = lParam2;

	int iRow1 = pListCtrl->FindItem(&info1, -1);
	int iRow2 = pListCtrl->FindItem(&info2, -1);
	
	CString strItem1, strItem2;
	int res = 0;
	for(int i = 0; i < 4;i++)
	{
		strItem1 = pListCtrl->GetItemText(iRow1, i);
		strItem2 = pListCtrl->GetItemText(iRow2, i);
		if(3 <= i)
		{
			if(atof(strItem1) > atof(strItem2)) return 1;
			else if(atof(strItem1) < atof(strItem2)) return -1;
			else continue;
		}
		else if(2 == i )
		{
			strItem1.Replace("C", "");
			strItem2.Replace("C", "");
			if(atoi(strItem1) > atoi(strItem2)) return 1;
			else if(atoi(strItem1) < atoi(strItem2)) return -1;
			else continue;

		}
		else if(0 == i)
		{
			vector<string> oResult1, oResult2;
			CTokenizer<CIsFromString>::Tokenize(oResult1, strItem1.operator LPCSTR(), CIsFromString("/"));
			CTokenizer<CIsFromString>::Tokenize(oResult2, strItem2.operator LPCSTR(), CIsFromString("/"));
			
			if(atof(oResult1[0].c_str()) > atof(oResult2[0].c_str())) return 1;
			else if(atof(oResult1[0].c_str()) < atof(oResult2[0].c_str())) return -1;
			else continue;
		}
		else
		{
			res = strcmp(strItem1, strItem2);
			if(0 != res) return res;
		}
	}

	return res;
}

// CDrumScheduleCreationDlg dialog

IMPLEMENT_DYNAMIC(CDrumScheduleCreationDlg, CDialog)

CDrumScheduleCreationDlg::CDrumScheduleCreationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDrumScheduleCreationDlg::IDD, pParent) , m_bAppendDrumSchCreation(FALSE)
{
	m_bDragging = FALSE;
}

CDrumScheduleCreationDlg::~CDrumScheduleCreationDlg()
{
}

void CDrumScheduleCreationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_CABLE_SPEC_LIST , m_wndCableSpecListCtrl);
	DDX_Control(pDX , IDC_STD_CABLE_LENGTH_BUTTON , m_wndAddButton);
	DDX_Control(pDX , IDC_CREATION_BUTTON , m_wndCreationButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);
	DDX_Control(pDX, IDC_SAVE_BUTTON, m_wndSaveButton);
	DDX_Control(pDX, IDC_COMBO1, m_wndComBo1);
	DDX_Control(pDX, IDC_COMBO2, m_wndComBo2);
	DDX_Control(pDX, IDC_EDIT, m_wndEdit);

	DDX_Radio(pDX , IDC_RADIO1 , m_bAppendDrumSchCreation);
	DDX_Control(pDX, IDC_REFRESH_BUTTON, m_wndRefreshButton);
	DDX_Control(pDX, IDC_MIN_SPARE_EDIT, m_wndMinSpareLenEdit);
}


BEGIN_MESSAGE_MAP(CDrumScheduleCreationDlg, CDialog)
	ON_BN_CLICKED(IDC_STD_CABLE_LENGTH_BUTTON, &CDrumScheduleCreationDlg::OnBnClickedStdCableLengthButton)
	ON_WM_MOUSEMOVE()
	ON_NOTIFY(LVN_BEGINDRAG, IDC_CABLE_SPEC_LIST, &CDrumScheduleCreationDlg::OnLvnBegindragCableSpecList)
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_SAVE_BUTTON, &CDrumScheduleCreationDlg::OnBnClickedSaveButton)
	ON_BN_CLICKED(IDC_CREATION_BUTTON, &CDrumScheduleCreationDlg::OnBnClickedCreationButton)
	ON_WM_MEASUREITEM()
	ON_COMMAND(ID_CABLE_DATA_DELETE, &CDrumScheduleCreationDlg::OnCableDataDelete)
	ON_BN_CLICKED(IDC_REFRESH_BUTTON, &CDrumScheduleCreationDlg::OnBnClickedRefreshButton)
END_MESSAGE_MAP()


// CDrumScheduleCreationDlg message handlers

/**
	@brief	

	@author BHK	

	@date 2009-09-03 오후 7:34:45	

	@param	

	@return		
*/
BOOL CDrumScheduleCreationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//! Schematic type Tree List 해더생성
	if(ERROR_SUCCESS != InitSchematicTreeList())
	{
		AfxMessageBox(_T("Fail Create TreeList Ctrl..."));
		return FALSE;
	}
	
	LoadData();
	InitCableSpecList();
	DisplayCableSpec();
	DisplaySchematicTreeListCtrl();

	m_wndSaveButton.SetIcon(IDI_SAVE);
	m_wndCreationButton.SetIcon(IDI_OK);
	m_wndCancelButton.SetIcon(IDI_CANCEL);
	m_wndRefreshButton.SetIcon(IDI_REFRESH);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	SCHEMATIC TREE LIST CTRL를 생성한다.

	@author KHS	

	@return		
**/
int CDrumScheduleCreationDlg::InitSchematicTreeList(void)
{
	static const CString __HEADERS__[] = {_T("DRUM") , _T("VOLTAGE") , _T("SPEC.") , _T("CORE") , _T("SIZE") , _T("PREFIX") , _T("SUFFIX")};

	CRect rect;
	CPoint point(0,0);
	GetDlgItem(IDC_CABLE_SPEC_LIST_STATIC)->GetWindowRect(&rect);
	ScreenToClient(&rect);

	unsigned uStyle	= TVS_HASBUTTONS|TVS_HASLINES/*|TVS_FULLROWSELECT|TVS_NONEVENHEIGHT|TVS_CHECKBOXES*/;
	unsigned uExStyle = TVS_EX_SUBSELECT|TVS_EX_FULLROWMARK;

	if(m_wndDrumTreeListCtrl.Create(uStyle|WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL,rect,this, 123))
	{
		m_wndDrumTreeListCtrl.InsertColumn(0, __HEADERS__[0], LVCFMT_LEFT, 50);
		m_wndDrumTreeListCtrl.InsertColumn(1, __HEADERS__[1], LVCFMT_LEFT, 70);
		m_wndDrumTreeListCtrl.InsertColumn(2, __HEADERS__[2], LVCFMT_LEFT, 80);
		m_wndDrumTreeListCtrl.InsertColumn(3, __HEADERS__[3], LVCFMT_LEFT, 60);
		m_wndDrumTreeListCtrl.InsertColumn(4, __HEADERS__[4], LVCFMT_LEFT, 60);
		m_wndDrumTreeListCtrl.InsertColumn(5, __HEADERS__[5], LVCFMT_LEFT, 60);
		m_wndDrumTreeListCtrl.InsertColumn(6, __HEADERS__[6], LVCFMT_LEFT, 60);
		m_wndDrumTreeListCtrl.SetExtendedStyle(uExStyle);

		return ERROR_SUCCESS;
	}
	

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author BHK	

	@date 2009-09-03 오후 7:54:48	

	@param	

	@return		
*/
int CDrumScheduleCreationDlg::InitCableSpecList(void)
{
	static const CString __HEADERS__[] = { _T("VOLTAGE"), _T("SPEC."), _T("CORE") , _T("SIZE") , _T("PREFIX") , _T("SUFFIX")};
	static const int __WIDTHS__[] = { 70, 80 , 40 , 50 , 75 , 25};
	
	m_wndCableSpecListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	
	for(int i = 0;i < SIZE_OF_ARRAY(__HEADERS__);++i)
	{	
		m_wndCableSpecListCtrl.InsertColumn(i , __HEADERS__[i].operator LPCSTR() ,LVCFMT_LEFT , __WIDTHS__[i]);
	}
	m_wndCableSpecListCtrl.SetColumnEditor(4 , &CDrumScheduleCreationDlg::InitEditor , &CDrumScheduleCreationDlg::EndEditor, &m_wndEdit);
	m_wndCableSpecListCtrl.SetColumnColors(4, RGB(196,222,241) , NULL);
	
	m_wndCableSpecListCtrl.ModifyStyle( LVS_OWNERDRAWFIXED, 0, 0 );
	m_wndCableSpecListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author KHS	

	@date 2009-09-22 오전 9:50:59	

	@param	

	@return		
**/
BOOL CDrumScheduleCreationDlg::InitEditor(CWnd** pWnd, int nRow, int nColumn, CString& strSubItemText, DWORD_PTR dwItemData, void* pThis, BOOL bUpdate)
{
	ASSERT(*pWnd);
	switch(nColumn)
	{
		case 4:
		{
			CEdit *pEdit = dynamic_cast<CEdit *>(*pWnd);
			if(pEdit) pEdit->SetWindowText(strSubItemText);
		}
		break;
		case 5:
		{
			CComboBox *pCmb = dynamic_cast<CComboBox *>(*pWnd);
			if(pCmb)
			{
				pCmb->SelectString(0, strSubItemText);
			}
		}
		break;
		case 6:
		{
			CComboBox *pCmb = dynamic_cast<CComboBox *>(*pWnd);
			if(pCmb)
			{
				pCmb->SelectString(0, strSubItemText);
			}
		}
		break;
	}
	return TRUE;
}

/**
	@brief	

	@author KHS	

	@date 2009-09-22 오전 9:50:56	

	@param	

	@return		
**/
BOOL CDrumScheduleCreationDlg::EndEditor(CWnd** pWnd, int nRow, int nColumn, CString& strSubItemText, DWORD_PTR dwItemData, void* pThis, BOOL bUpdate)
{
	ASSERT(pWnd);
	switch(nColumn)
	{
		case 4:
		{
			CEdit *pEdit = dynamic_cast<CEdit *>(*pWnd);
			if(pEdit) pEdit->GetWindowText(strSubItemText);
		}
		break;
		case 5:
		{
			CComboBox *pCmb = dynamic_cast<CComboBox *>(*pWnd);
			int index = pCmb->GetCurSel();
			if(index >= 0) pCmb->GetLBText(index, strSubItemText);
		}
		break;
		case 6:
		{
			CComboBox *pCmb = dynamic_cast<CComboBox *>(*pWnd);
			int index = pCmb->GetCurSel();
			if(index >= 0) pCmb->GetLBText(index, strSubItemText);
		}
		break;
	}
	return TRUE;
}

/**
	@brief	

	@author KHS	

	@date 2009-09-04 오후 5:02:54	

	@param	

	@return		
**/
namespace DrumScheduleCreation
{
struct FindMatchCableItem : binary_function<CDrumScheduleCreationDlg::CABLE_ITEM_STRUCTURE , CDrumScheduleCreationDlg::CABLE_ITEM_STRUCTURE , bool>
{
	bool operator()(const CDrumScheduleCreationDlg::CABLE_ITEM_STRUCTURE& lhs , const CDrumScheduleCreationDlg::CABLE_ITEM_STRUCTURE& rhs) const
	{
		const bool res = ((lhs.rCableVolt ==  rhs.rCableVolt) && (lhs.rSpec == rhs.rSpec) && (lhs.rCore == rhs.rCore) &&
		(lhs.rSize == rhs.rSize));

		return res;
	}
};
};


struct MatchCableSys : binary_function<CELOADTable::Record , CDrumScheduleCreationDlg::CABLE_ITEM_STRUCTURE , bool>
{
	bool operator()(const CELOADTable::Record& lhs , const CDrumScheduleCreationDlg::CABLE_ITEM_STRUCTURE& rhs) const
	{
		string rCableVolt, rSpec, rCore, rSize;
		{
			const map<string,string>::const_iterator where = lhs.FieldValueMap.find(_T("C_VOLTAGE"));
			if(where != lhs.FieldValueMap.end())
			{
				rCableVolt = where->second;
			}
		}
		{
			const map<string,string>::const_iterator where = lhs.FieldValueMap.find(_T("C_SPEC"));
			if(where != lhs.FieldValueMap.end())
			{
				rSpec = where->second;
			}
		}
		{
			const map<string,string>::const_iterator where = lhs.FieldValueMap.find(_T("C_CORE"));
			if(where != lhs.FieldValueMap.end())
			{
				rCore = where->second;
			}
		}
		{
			const map<string,string>::const_iterator where = lhs.FieldValueMap.find(_T("C_SIZE"));
			if(where != lhs.FieldValueMap.end())
			{
				rSize = where->second;
			}
		}
		
		const bool res = ((rCableVolt ==  rhs.rCableVolt) && (rSpec == rhs.rSpec) && (rCore == rhs.rCore) &&
		(rSize == rhs.rSize));

		return res;
	}
};

/**
	@brief	Compare MatchCable

	@author KHS	

	@date 2009-09-04 오후 4:53:46	

	@param	

	@return		
**/
BOOL CDrumScheduleCreationDlg::CompareMatchCable(const CABLE_ITEM_STRUCTURE& record, vector<CABLE_ITEM_STRUCTURE> &CableItemEntry)
{
	if(!CableItemEntry.empty())
	{
		vector<CABLE_ITEM_STRUCTURE>::const_iterator where = find_if(CableItemEntry.begin() , CableItemEntry.end() , bind2nd(DrumScheduleCreation::FindMatchCableItem() , record));
		if(where != CableItemEntry.end())
		{
			return TRUE;
		}
	}

	return FALSE;
}

static bool SortCableSpecList(CDrumScheduleCreationDlg::CABLE_ITEM_STRUCTURE lhs , CDrumScheduleCreationDlg::CABLE_ITEM_STRUCTURE rhs)
{
	if(lhs.rCableVolt == rhs.rCableVolt)
	{
		if(lhs.rSpec == rhs.rSpec)
		{
			if(lhs.rCore == rhs.rCore)
			{
				if(lhs.rSize == rhs.rSize)
				{
					return false;
				}else	return (atof(lhs.rSize.c_str()) < atof(rhs.rSize.c_str()));
			}
			else
			{
				CString strItem1(lhs.rCore.c_str());
				CString strItem2(rhs.rCore.c_str());
				strItem1.Replace("C", "");
				strItem2.Replace("C", "");

				return (atoi(strItem1) < atoi(strItem2));
			}
		}else	return (lhs.rSpec < rhs.rSpec);
	}
	else
	{
		vector<string> oResult1, oResult2;
		CTokenizer<CIsFromString>::Tokenize(oResult1, lhs.rCableVolt, CIsFromString("/"));
		CTokenizer<CIsFromString>::Tokenize(oResult2, rhs.rCableVolt, CIsFromString("/"));

		return (atof(oResult1[0].c_str()) > atof(oResult2[0].c_str()));
	}
}


/**
	@brief	

	@author BHK	

	@date 2009-09-03 오후 8:14:02	

	@param	

	@return		
*/
int CDrumScheduleCreationDlg::DisplayCableSpec(bool bLoadDB)
{
	m_wndCableSpecListCtrl.DeleteAllItems();

	CELoadDocData& docData = CELoadDocData::GetInstance();
	
	list<CELoadItem*> CableItemList;
	docData.GetELoadItemListOf(CableItemList, CCableItem::TypeString(), _T("*"));

	vector<CABLE_ITEM_STRUCTURE> CableItemEntry;
	for(list<CELoadItem*>::iterator itr = CableItemList.begin(); itr != CableItemList.end();++itr)
	{
		CCableItem *pCableItem = static_cast<CCableItem*>(*itr);
		if(pCableItem)
		{
			CABLE_ITEM_STRUCTURE record;

			record.rCableVolt = pCableItem->prop()->GetValue(_T("General"), _T("Cable Volt"));
			record.rSpec = pCableItem->prop()->GetValue(_T("General"), _T("Cable Spec"));
			record.rCore = pCableItem->prop()->GetValue(_T("General"), _T("Core"));
			record.rSize = pCableItem->prop()->GetValue(_T("General"), _T("Size"));
			record.rCategory = pCableItem->prop()->GetValue(_T("General"), _T("Cable Category"));
			record.rVoltType = pCableItem->prop()->GetValue(_T("General"), _T("Volt Type")).c_str();

			if(record.rCableVolt.empty() && record.rSpec.empty() && record.rCore.empty() && record.rSize.empty()) continue;

			if(!CompareMatchCable(record, CableItemEntry)) CableItemEntry.push_back(record);
		}
	}
	::stable_sort(CableItemEntry.begin() , CableItemEntry.end() , SortCableSpecList);

	CELOADTable* pTable = docData.GetTableOf(_T("T_DRUM_SCHEDULE_SETTING"));
	CELOADTable* pCableSpecListSettingTable = docData.GetTableOf(_T("T_CABLE_SPEC_LIST_SETTING"));
	for(vector<CABLE_ITEM_STRUCTURE>::iterator jtr = CableItemEntry.begin(); jtr != CableItemEntry.end();++jtr)
	{
		if(pTable)
		{
			vector<TABLES::CELOADTable::Record>::iterator where = find_if(pTable->m_RecordEntry.begin(), pTable->m_RecordEntry.end(), bind2nd(MatchCableSys(), *jtr));
			if(where == pTable->m_RecordEntry.end())
			{
				int nIndex = m_wndCableSpecListCtrl.GetItemCount();
				int nItem = m_wndCableSpecListCtrl.InsertItem(nIndex, jtr->rCableVolt.c_str());
				if(-1 != nItem)
				{
					m_wndCableSpecListCtrl.SetItemText(nItem, 1, jtr->rSpec.c_str());
					m_wndCableSpecListCtrl.SetItemText(nItem, 2, jtr->rCore.c_str());
					m_wndCableSpecListCtrl.SetItemText(nItem, 3, jtr->rSize.c_str());

					vector<TABLES::CELOADTable::Record>::iterator where1 = 
						find_if(pCableSpecListSettingTable->m_RecordEntry.begin(), pCableSpecListSettingTable->m_RecordEntry.end(), bind2nd(MatchCableSys(), *jtr));
					if(where1 != pCableSpecListSettingTable->m_RecordEntry.end() && bLoadDB)
					{
						m_wndCableSpecListCtrl.SetItemText(nItem, 4 , where1->FieldValueMap[_T("C_PREFIX")].c_str());
					}
					else
					{
						TABLES::CELOADTable::Record record;
						record.FieldValueMap[_T("C_VOLTAGE")] = jtr->rCableVolt;
						record.FieldValueMap[_T("C_SPEC")] = jtr->rSpec;
						record.FieldValueMap[_T("C_CORE")] = jtr->rCore;
						record.FieldValueMap[_T("C_SIZE")] = jtr->rSize;
						
						//! default prefix
						string rCategory;
						if(_T("Power Cable") == jtr->rCategory)
						{
							if(string::npos != jtr->rVoltType.find(_T("HV")))
							{
								rCategory = _T("HV");
							}
							else if((string::npos != jtr->rVoltType.find(_T("LV"))) || (string::npos != jtr->rVoltType.find(_T("DC"))))
							{
								rCategory = _T("LV");
							}
						}
						else if(_T("Control Cable") == jtr->rCategory)
						{
							rCategory = _T("CV");
						}
						
						//! size 항목의 크기는 3이다. 3보다 작을때에는 맨앞에 0을 채운다.
						string rSizeExceptDot = jtr->rSize;
						string::size_type at = rSizeExceptDot.find(_T("."));
						if(string::npos != at) rSizeExceptDot = rSizeExceptDot.substr(0 , at);
						const int nPadding = 3 - rSizeExceptDot.length();
						for(int i = 0;i < nPadding;++i) rSizeExceptDot = _T("0") + rSizeExceptDot;
						
						//!Core의 C는 뺀다.
						CString rCoreTemp = jtr->rCore.c_str();
						rCoreTemp.Replace(_T("C"), _T(""));

						record.FieldValueMap[_T("C_PREFIX")] = rCategory + _T("-") + rCoreTemp.operator LPCSTR() + rSizeExceptDot;
						pCableSpecListSettingTable->Insert(record);

						m_wndCableSpecListCtrl.SetItemText(nItem, 4 , record.FieldValueMap[_T("C_PREFIX")].c_str());
					}
					m_wndCableSpecListCtrl.SetItemText(nItem, 5 , _T("-1"));
					//m_wndCableSpecListCtrl.SetItemData(nItem ,(DWORD)nItem);
				}
			}
		}
	}
	///m_wndCableSpecListCtrl.SortItems(CompareFunction, (LPARAM)&m_wndCableSpecListCtrl);
	
	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	ADD Length Button

	@author KHS	

	@date 2009-09-04 오전 9:33:35	

	@param	

	@return		
**/
void CDrumScheduleCreationDlg::OnBnClickedStdCableLengthButton()
{
	CString rLength;
	GetDlgItemText(IDC_STD_CABLE_LENGTH_EDIT, rLength);

	HTREEITEM hFirst = m_wndDrumTreeListCtrl.GetFirstVisibleItem();
	if(!rLength.IsEmpty() && !IsExistCableLength(hFirst, rLength))
	{
		TV_INSERTSTRUCT tvStruct;

		tvStruct.hParent = NULL;
		tvStruct.hInsertAfter = TVI_SORT;
		tvStruct.item.iImage = 1;
		tvStruct.item.iSelectedImage = 1;
		tvStruct.item.pszText = (LPSTR)(LPCTSTR)rLength;
		tvStruct.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

		HTREEITEM hItem = m_wndDrumTreeListCtrl.InsertItem(&tvStruct);
		m_wndDrumTreeListCtrl.Expand(NULL, TVE_EXPAND);
	}
}

/**
	@brief	

	@author KHS	

	@date 2009-09-04 오전 10:55:22	

	@param	

	@return		
**/
int CDrumScheduleCreationDlg::OnBeginDrag(LPNMLISTVIEW pNMLV , CListCtrl& ListCtrl)
{
	//// Save the index of the item being dragged in m_nDragIndex
	////  This will be used later for retrieving the info dragged
	m_nDragIndex = pNMLV->iItem;

	//// Create a drag image
	POINT pt;
	int nOffset = -10; //offset in pixels for drag image (positive is up and to the left; neg is down and to the right)
	if((pNMLV->hdr.hwndFrom == ListCtrl.GetSafeHwnd()) && (ListCtrl.GetSelectedCount() > 0)) //! more than one item is selected
	{
		pt.x = nOffset;
		pt.y = nOffset;

		m_pDragImage = ListCtrl.CreateDragImage(m_nDragIndex, &pt);
		ASSERT(m_pDragImage); //make sure it was created
		//We will call delete later (in LButtonUp) to clean this up

		CBitmap bitmap;
		if(ListCtrl.GetSelectedCount() > 1) //more than 1 item in list is selected
			//bitmap.LoadBitmap(IDB_BITMAP_MULTI);
			bitmap.LoadBitmap(IDB_BITMAP_MULTI_BOXES);
		else	bitmap.LoadBitmap(IDB_BITMAP_BOX);
		m_pDragImage->Replace(0, &bitmap, &bitmap);

		//// Change the cursor to the drag image
		////	(still must perform DragMove() in OnMouseMove() to show it moving)
		m_pDragImage->BeginDrag(0, CPoint(nOffset, nOffset - 4));
		m_pDragImage->DragEnter(GetDesktopWindow(), pNMLV->ptAction);

		//// Set dragging flag and others
		m_bDragging = TRUE;	//we are in a drag and drop operation
		m_nDropIndex = -1;	//we don't have a drop index yet
		m_pDragList = &ListCtrl; //make note of which list we are dragging from
		m_pDropList = &m_wndDrumTreeListCtrl; //at present the drag list is the drop list

		//// Capture all mouse messages
		SetCapture ();
	}

	return ERROR_SUCCESS;
}
/**
	@brief	mouse move

	@author KHS	

	@date 2009-09-04 오전 10:25:03	

	@param	

	@return		
**/
void CDrumScheduleCreationDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bDragging)
	{
		//// Move the drag image
		CPoint pt(point);	//get our current mouse coordinates
		ClientToScreen(&pt); //convert to screen coordinates
		m_pDragImage->DragMove(pt); //move the drag image to those coordinates
		// Unlock window updates (this allows the dragging image to be shown smoothly)
		m_pDragImage->DragShowNolock(false);

		//// Get the CWnd pointer of the window that is under the mouse cursor
		CWnd* pDropWnd = WindowFromPoint (pt);
		ASSERT(pDropWnd); //make sure we have a window

		//// If we drag outside current window we need to adjust the highlights displayed
		if (pDropWnd != m_pDropList)
		{
			if (m_nDropIndex != -1) //If we drag over the CListCtrl header, turn off the hover highlight
			{
				TRACE("m_nDropIndex is -1\n");
				CTreeListCtrl* pList = (CTreeListCtrl*)m_pDropList;
				m_nDropIndex = -1;
			}
			else //If we drag out of the CListCtrl altogether
			{
				TRACE("m_nDropIndex is not -1\n");
			}
		}

		// Save current window pointer as the CListCtrl we are dropping onto
		m_pDropList = (CTreeListCtrl*)pDropWnd;

		// Convert from screen coordinates to drop target client coordinates
		pDropWnd->ScreenToClient(&pt);

		//If we are hovering over a CListCtrl we need to adjust the highlights
		if(pDropWnd->IsKindOf(RUNTIME_CLASS(CTreeListCtrl)) && (pDropWnd->GetSafeHwnd() == m_wndDrumTreeListCtrl.GetSafeHwnd()))
		{
			TRACE("m_nDropIndex is not 100\n");
			//Note that we can drop here
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			UINT uFlags;
			CTreeListCtrl* pTreeList = (CTreeListCtrl*)pDropWnd;

			// Turn off hilight for previous drop target
			pTreeList->SetItemState(m_hDropItem , 0 , LVIS_DROPHILITED);
			// Redraw previous item
			//pTreeList->RedrawItems (m_nDropIndex, m_nDropIndex);
			///pTreeList->RedrawWindow();

			// Get the item that is below cursor
			m_hDropItem = ((CTreeListCtrl*)pDropWnd)->HitTest(pt, &uFlags);
			if(NULL != m_hDropItem)
			{
				if(m_pDragList && (m_pDragList->GetSafeHwnd() == m_wndCableSpecListCtrl.GetSafeHwnd()))
				{
					TRACE("m_nDropIndex is not 100\n");
					HTREEITEM hParentItem = ((CTreeListCtrl*)pDropWnd)->GetParentItem(m_hDropItem);
					if(NULL == hParentItem)
					{
						//Note that we can drop here
						SetCursor(LoadCursor(NULL, IDC_ARROW));
						//UINT uFlags;
						CListCtrl* pList = (CListCtrl*)pDropWnd;
					}
					else
					{
						m_pDropList->SetItemState (m_hDropItem, 0, LVIS_DROPHILITED);
						SetCursor(LoadCursor(NULL, IDC_NO));
						return;
					}
				}

				// Highlight it
				///pTreeList->SetItemState(m_nDropIndex, LVIS_DROPHILITED, LVIS_DROPHILITED);
				pTreeList->SetItemState(m_hDropItem, LVIS_DROPHILITED, LVIS_DROPHILITED);
				// Redraw item
				//pList->RedrawItems(m_nDropIndex, m_nDropIndex);
				pTreeList->RedrawWindow();
			}else	SetCursor(LoadCursor(NULL, IDC_NO));
		}
		else
		{
			//If we are not hovering over a CListCtrl, change the cursor
			// to note that we cannot drop here
			SetCursor(LoadCursor(NULL, IDC_NO));
		}
		// Lock window updates
		m_pDragImage->DragShowNolock(true);
	}
	CDialog::OnMouseMove(nFlags, point);
}

/**
	@brief	drop item on tree item.

	@author KHS	

	@date 2009-09-04 오전 11:15:22	

	@param	

	@return		
**/
void CDrumScheduleCreationDlg::DropItemOnList(CListCtrl* pDragList , CTreeListCtrl* pDropList)
{
	if((pDropList->GetSafeHwnd() == m_wndDrumTreeListCtrl.GetSafeHwnd()))
	{
		// Turn off hilight for previous drop target
		pDropList->SetItemState (m_hDropItem, 0, LVIS_DROPHILITED);
		// Redraw previous item
		//pDropList->RedrawItems (m_nDropIndex, m_nDropIndex);

		CString rCableLength;
		HTREEITEM hSelParent = pDropList->GetParentItem(m_hDropItem);
		if(m_hDropItem && hSelParent == NULL)
		{
			rCableLength = pDropList->GetItemText(m_hDropItem, 0);

			if(m_pDragList && (m_pDragList->GetSafeHwnd() == m_wndCableSpecListCtrl.GetSafeHwnd()))
			{
				for (int nItem = m_pDragList->GetItemCount() - 1 ; nItem >= 0 ; nItem--)
				{
					UINT  uState = m_pDragList->GetItemState(nItem, LVIS_SELECTED);
					if (LVIS_SELECTED == (uState & LVIS_SELECTED))
					{
						CString rVoltage = m_pDragList->GetItemText(nItem, 0);
						CString rSpec   = m_pDragList->GetItemText(nItem, 1);
						CString rCore   = m_pDragList->GetItemText(nItem, 2);
						CString rSize   = m_pDragList->GetItemText(nItem, 3);
						CString rPrefix = m_pDragList->GetItemText(nItem, 4);
						CString rSuffix = m_pDragList->GetItemText(nItem, 5);

						m_pDragList->DeleteItem(nItem);

						CELoadDocData& docData = CELoadDocData::GetInstance();
						CELOADTable* pTable = docData.GetTableOf(_T("T_DRUM_SCHEDULE_SETTING"));
						if(pTable)
						{
							TABLES::CELOADTable::Record record;
							record.FieldValueMap[_T("C_STD_CABLE_LENGTH")] = rCableLength.operator LPCSTR();
							record.FieldValueMap[_T("C_VOLTAGE")] = rVoltage.operator LPCSTR();
							record.FieldValueMap[_T("C_SPEC")]   = rSpec.operator LPCSTR();
							record.FieldValueMap[_T("C_CORE")]   = rCore.operator LPCSTR();
							record.FieldValueMap[_T("C_SIZE")]   = rSize.operator LPCSTR();
							record.FieldValueMap[_T("C_PREFIX")] = rPrefix.operator LPCSTR();

							pTable->Insert(record);

							TV_INSERTSTRUCT tvStruct;

							tvStruct.hParent = m_hDropItem;
							tvStruct.hInsertAfter = TVI_SORT;
							tvStruct.item.iImage = 1;
							tvStruct.item.iSelectedImage = 1;
							tvStruct.item.pszText = (LPSTR)(LPCTSTR)_T("");
							tvStruct.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

							HTREEITEM hChildItem = m_wndDrumTreeListCtrl.InsertItem(&tvStruct);
							if(hChildItem)
							{
								m_wndDrumTreeListCtrl.SetItemText(hChildItem, rVoltage, 1);
								m_wndDrumTreeListCtrl.SetItemText(hChildItem, rSpec   , 2);
								m_wndDrumTreeListCtrl.SetItemText(hChildItem, rCore   , 3);
								m_wndDrumTreeListCtrl.SetItemText(hChildItem, rSize   , 4);
								m_wndDrumTreeListCtrl.SetItemText(hChildItem, rPrefix , 5);
								m_wndDrumTreeListCtrl.SetItemText(hChildItem, rSuffix , 6);
								m_wndDrumTreeListCtrl.SetItemData(hChildItem, (DWORD_PTR)hChildItem);

								TVSORTCB tvs;

								tvs.hParent = m_hDropItem;
								tvs.lParam = (LPARAM)&m_wndDrumTreeListCtrl;
								tvs.lpfnCompare = CompareProc;

								m_wndDrumTreeListCtrl.SortChildrenCB(&tvs);

								m_wndDrumTreeListCtrl.Expand(m_hDropItem, TVE_EXPAND);
							}
						}
					}
				}
			}
		}
	}
	
}

/**
	@brief	TreeListCtrl에 같은 Cable Length가 있는지 검색

	@author KHS	

	@date 2009-09-04 오전 10:42:56	

	@param	

	@return	같은게 존재하면 TRUE
**/
bool CDrumScheduleCreationDlg::IsExistCableLength(HTREEITEM &hItem, const CString& rCableLength)
{
	if(hItem)
	{
		if(m_wndDrumTreeListCtrl.GetItemText(hItem) == rCableLength)
		{
			return true;
		} 
		else
		{
			HTREEITEM hSiblingItem = m_wndDrumTreeListCtrl.GetNextSiblingItem(hItem);
			if(hSiblingItem)
			{
				if(IsExistCableLength(hSiblingItem, rCableLength))
				{
					return true;
				}
			}
		}
	}

	return false;
}

/**
	@brief	

	@author KHS	

	@date 2009-09-04 오전 11:30:52	

	@param	

	@return		
**/
void CDrumScheduleCreationDlg::OnLvnBegindragCableSpecList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if(NULL == pNMLV) return;

	OnBeginDrag(pNMLV , m_wndCableSpecListCtrl);

	*pResult = 0;
}

/**
	@brief	

	@author KHS	

	@date 2009-09-04 오후 1:12:52	

	@param	

	@return		
**/
void CDrumScheduleCreationDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bDragging)
	{
		// Release mouse capture, so that other controls can get control/messages
		ReleaseCapture ();
		
		// Note that we are NOT in a drag operation
		m_bDragging = FALSE;
		
		// End dragging image
		m_pDragImage->DragLeave (GetDesktopWindow ());
		m_pDragImage->EndDrag ();
		delete m_pDragImage; //must delete it because it was created at the beginning of the drag
		
		CPoint pt (point); //Get current mouse coordinates
		ClientToScreen (&pt); //Convert to screen coordinates
		// Get the CWnd pointer of the window that is under the mouse cursor
		CWnd* pDropWnd = WindowFromPoint (pt);
		ASSERT (pDropWnd); //make sure we have a window pointer
		// If window is CListCtrl, we perform the drop
		if (pDropWnd->IsKindOf (RUNTIME_CLASS (CTreeListCtrl)))
		{
			m_pDropList = (CTreeListCtrl*)pDropWnd; //Set pointer to the list we are dropping on
			DropItemOnList(m_pDragList, m_pDropList); //Call routine to perform the actual drop
		}
	}


	CDialog::OnLButtonUp(nFlags, point);
}

/**
	@brief	DISPLAY DRUM SCHEDULE SETTING

	@author KHS	

	@date 2009-09-04 오후 3:05:38	

	@param	

	@return		
**/
int CDrumScheduleCreationDlg::DisplaySchematicTreeListCtrl(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pTable = docData.GetTableOf(_T("T_DRUM_SCHEDULE_SETTING"));
	if(pTable)
	{
		set<string> CableLengthSet;
		map<string, HTREEITEM> CablelengthTreeItemMap;
		{
			//! Cable Length 중복 제거
			for(vector<TABLES::CELOADTable::Record>::iterator jtr = pTable->m_RecordEntry.begin(); jtr != pTable->m_RecordEntry.end();++jtr)
			{
				CableLengthSet.insert(jtr->FieldValueMap[_T("C_STD_CABLE_LENGTH")]);
			}
		}
		{
			//! CableLength 먼저 Tree에 insert
			for(set<string>::iterator itr = CableLengthSet.begin(); itr != CableLengthSet.end();++itr)
			{
				TV_INSERTSTRUCT tvStruct;

				tvStruct.hParent = NULL;
				tvStruct.hInsertAfter = TVI_SORT;
				tvStruct.item.iImage = 1;
				tvStruct.item.iSelectedImage = 1;
				tvStruct.item.pszText = (LPSTR)(LPCTSTR)itr->c_str();
				tvStruct.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

				HTREEITEM hItem = m_wndDrumTreeListCtrl.InsertItem(&tvStruct);
				CablelengthTreeItemMap.insert(make_pair((*itr), hItem));
			}
		}

		//! CableLengthSet에 들어있는 Length 별로 아이템을 추가
		{
			for(map<string, HTREEITEM>::iterator itr = CablelengthTreeItemMap.begin(); itr != CablelengthTreeItemMap.end();++itr)
			{
				for(vector<TABLES::CELOADTable::Record>::iterator jtr = pTable->m_RecordEntry.begin(); jtr != pTable->m_RecordEntry.end();++jtr)
				{
					if(itr->first == jtr->FieldValueMap[_T("C_STD_CABLE_LENGTH")])
					{
						TV_INSERTSTRUCT tvStruct2;

						tvStruct2.hParent = itr->second;
						//tvStruct2.hInsertAfter = TVI_SORT;
						tvStruct2.item.iImage = 1;
						tvStruct2.item.iSelectedImage = 1;
						tvStruct2.item.pszText = (LPSTR)(LPCTSTR)_T("");
						tvStruct2.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

						HTREEITEM hChildItem = m_wndDrumTreeListCtrl.InsertItem(&tvStruct2);
						m_wndDrumTreeListCtrl.SetItemText(hChildItem, jtr->FieldValueMap[_T("C_VOLTAGE")].c_str(), 1);
						m_wndDrumTreeListCtrl.SetItemText(hChildItem, jtr->FieldValueMap[_T("C_SPEC")].c_str(), 2);
						m_wndDrumTreeListCtrl.SetItemText(hChildItem, jtr->FieldValueMap[_T("C_CORE")].c_str(), 3);
						m_wndDrumTreeListCtrl.SetItemText(hChildItem, jtr->FieldValueMap[_T("C_SIZE")].c_str(), 4);
						m_wndDrumTreeListCtrl.SetItemText(hChildItem, jtr->FieldValueMap[_T("C_PREFIX")].c_str(), 5);
						m_wndDrumTreeListCtrl.SetItemText(hChildItem, _T("-1") , 6);
						m_wndDrumTreeListCtrl.SetItemData(hChildItem, (DWORD_PTR)hChildItem);

						TVSORTCB tvs;
						
						tvs.hParent = itr->second;
						tvs.lParam = (LPARAM)&m_wndDrumTreeListCtrl;
						tvs.lpfnCompare = CompareProc;

						m_wndDrumTreeListCtrl.SortChildrenCB(&tvs);
						m_wndDrumTreeListCtrl.Expand(itr->second, TVE_EXPAND);
					}
				}
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	SAVE DATA

	@author KHS	

	@date 2009-09-04 오후 3:43:54	

	@param	

	@return		
**/
void CDrumScheduleCreationDlg::OnBnClickedSaveButton()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pTable = docData.GetTableOf(_T("T_DRUM_SCHEDULE_SETTING"));
	if(pTable)
	{
		pTable->Save();
	}

	CELOADTable* pDrumSpareTable = docData.GetTableOf(_T("T_DRUM_SCHEDULE_SPARE_SETTING"));
	if(pDrumSpareTable)
	{
		CString rValue;
		m_wndMinSpareLenEdit.GetWindowText(rValue);
		
		if(pDrumSpareTable->m_RecordEntry.empty())
		{
			TABLES::CELOADTable::Record sparerecord;
			sparerecord.FieldValueMap[_T("C_MIN_SPARE")] = rValue.operator LPCSTR();
			pDrumSpareTable->m_RecordEntry.push_back(sparerecord);
		}
		else
		{
			vector<TABLES::CELOADTable::Record>::iterator itr = pDrumSpareTable->m_RecordEntry.begin();
			itr->FieldValueMap[_T("C_MIN_SPARE")] = rValue.operator LPCSTR();
		}

		pDrumSpareTable->Save();
	}

	CELOADTable* pCableSpecListTable = docData.GetTableOf(_T("T_CABLE_SPEC_LIST_SETTING"));
	if(pCableSpecListTable)
	{
		HTREEITEM hRootItem = m_wndDrumTreeListCtrl.GetRootItem();
		while(hRootItem)
		{
			HTREEITEM hChildItem = m_wndDrumTreeListCtrl.GetChildItem(hRootItem);
			while(hChildItem)
			{
				CABLE_ITEM_STRUCTURE record;
				record.rCableVolt = m_wndDrumTreeListCtrl.GetItemText(hChildItem , 1);
				record.rSpec = m_wndDrumTreeListCtrl.GetItemText(hChildItem , 2);
				record.rCore = m_wndDrumTreeListCtrl.GetItemText(hChildItem , 3);
				record.rSize = m_wndDrumTreeListCtrl.GetItemText(hChildItem , 4);

				vector<TABLES::CELOADTable::Record>::iterator where = 
					find_if(pCableSpecListTable->m_RecordEntry.begin(), pCableSpecListTable->m_RecordEntry.end(), bind2nd(MatchCableSys(), record));
				if(where != pCableSpecListTable->m_RecordEntry.end())
				{
					const CString rPrefix = m_wndDrumTreeListCtrl.GetItemText(hChildItem , 5);

					where->FieldValueMap[_T("C_PREFIX")] = rPrefix.operator LPCTSTR();
				}

				hChildItem = m_wndDrumTreeListCtrl.GetNextSiblingItem(hChildItem);
			}

			hRootItem = m_wndDrumTreeListCtrl.GetNextSiblingItem(hRootItem);
		}

		for(int i = 0;i < m_wndCableSpecListCtrl.GetItemCount();++i)
		{
			CABLE_ITEM_STRUCTURE record;
			record.rCableVolt = m_wndCableSpecListCtrl.GetItemText(i , 0);
			record.rSpec = m_wndCableSpecListCtrl.GetItemText(i , 1);
			record.rCore = m_wndCableSpecListCtrl.GetItemText(i , 2);
			record.rSize = m_wndCableSpecListCtrl.GetItemText(i , 3);

			vector<TABLES::CELOADTable::Record>::iterator where = 
				find_if(pCableSpecListTable->m_RecordEntry.begin(), pCableSpecListTable->m_RecordEntry.end(), bind2nd(MatchCableSys(), record));
			if(where != pCableSpecListTable->m_RecordEntry.end())
			{
				const CString rPrefix = m_wndCableSpecListCtrl.GetItemText(i , 4);

				where->FieldValueMap[_T("C_PREFIX")] = rPrefix.operator LPCTSTR();
			}
		}

		pCableSpecListTable->Save();
	}

	::MessageBox(NULL, _T("Save is done!") , _T("Message") , MB_OK|MB_ICONINFORMATION);
}

/**
	@brief	DRUM SCHEDULE CREATION

	@author KHS	

	@date 2009-09-04 오후 8:57:11	

	@param	

	@return		
**/
void CDrumScheduleCreationDlg::OnBnClickedCreationButton()
{
	if(ERROR_SUCCESS == DrumScheduleCreation())
	{
		CELOADApp* pApp = (CELOADApp*)AfxGetApp();
		if(pApp) pApp->OpenNewDocumentIfNeed(_T("Drum Schedule") , _T("Drum Schedule"));
		OnOK();
	}
	/*else
	{
		::MessageBox(NULL, "Waring", "fail,Drum Schedule Creation..!!", MB_OK|MB_ICONWARNING);
	}*/
}

/**
	@brief	

	@author KHS	

	@date 2009-09-22 오후 1:29:03	

	@param	

	@return		
**/
BOOL CDrumScheduleCreationDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE) return TRUE;
		if(pMsg->wParam == VK_RETURN) return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

static bool fnCompareCableType(TABLES::CELOADTable::Record lhs, TABLES::CELOADTable::Record rhs/*CDrumScheduleCreationDlg::LOAD_CABLE_RECORD lhs , CDrumScheduleCreationDlg::LOAD_CABLE_RECORD rhs */)
{
	const CString lCableVolt    = lhs.FieldValueMap[_T("C_VOLTAGE")].c_str();
	const CString lCableSpec    = lhs.FieldValueMap[_T("C_SPEC")].c_str();
	const CString lCableCore    = lhs.FieldValueMap[_T("C_CORE")].c_str();
	const CString lCableSize    = lhs.FieldValueMap[_T("C_SIZE")].c_str();

	const CString rCableVolt    = rhs.FieldValueMap[_T("C_VOLTAGE")].c_str();
	const CString rCableSpec    = rhs.FieldValueMap[_T("C_SPEC")].c_str();
	const CString rCableCore    = rhs.FieldValueMap[_T("C_CORE")].c_str();
	const CString rCableSize    = rhs.FieldValueMap[_T("C_SIZE")].c_str();

	if(lCableVolt == rCableVolt)
	{
		if(lCableSpec == rCableSpec)
		{
			if(lCableCore == rCableCore)
			{
				return (atof(lCableSize) < atof(rCableSize));
			}
			else
			{
				CString strItem1(lCableCore);
				CString strItem2(rCableCore);
				strItem1.Replace("C", "");
				strItem2.Replace("C", "");

				return (atoi(strItem1) < atoi(strItem2));
			}
		}else	return (lCableSpec < rCableSpec);
	}
	else
	{
		vector<string> oResult1, oResult2;
		CTokenizer<CIsFromString>::Tokenize(oResult1, lCableVolt.operator LPCSTR(), CIsFromString("/"));
		CTokenizer<CIsFromString>::Tokenize(oResult2, rCableVolt.operator LPCSTR(), CIsFromString("/"));

		return (atof(oResult1[0].c_str()) > atof(oResult2[0].c_str()));
	}

}


static bool fnCompareCableItemLength(CELoadItem* lhs ,CELoadItem* rhs )
{
	CString lCableLen = lhs->prop()->GetValue(_T("General") ,_T("Length")).c_str();
	CString rCableLen = rhs->prop()->GetValue(_T("General") ,_T("Length")).c_str();
	
	return (atof(lCableLen) > atof(rCableLen));

	
	//return (nLTotalCableLength > nRTotalCableLength);
}
struct MatchLoadIdCable : public binary_function<CDrumScheduleCreationDlg::LOAD_CABLE_RECORD,CString,bool>
{
        bool operator()(const CDrumScheduleCreationDlg::LOAD_CABLE_RECORD lhs , const CString& rhs) const
	{
		return (lhs.rLoadID == rhs);
	}
};

struct MatchIsDeleted : public binary_function<CDrumScheduleCreationDlg::LOAD_CABLE_RECORD,bool,bool>
{
        bool operator()(const CDrumScheduleCreationDlg::LOAD_CABLE_RECORD lhs , const bool& rhs) const
	{
		return (lhs.bIsDeleted == rhs);
	}
};


struct MatchCableLength : public binary_function<CDrumScheduleCreationDlg::LOAD_CABLE_RECORD,double,bool>
{
        bool operator()(const CDrumScheduleCreationDlg::LOAD_CABLE_RECORD lhs , const double& rhs) const
	{
		return (lhs.nTotalCableLength == rhs);
	}
};
/**
	@brief	

	@author KHS	

	@date 2009-09-22 오후 2:57:05	

	@param	

	@return		
**/
int CDrumScheduleCreationDlg::DrumScheduleCreation(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pTable = docData.GetTableOf(_T("T_DRUM_SCHEDULE_SETTING"));

	//! 기존의 Drum 모두 삭제
	list<CELoadItem*> DrumItemList, CableItemList;
	docData.GetELoadItemListOf(DrumItemList, CDrumItem::TypeString());
	{
		for(list<CELoadItem*>::iterator ktr = DrumItemList.begin(); ktr != DrumItemList.end(); ++ktr)
		{
			(*ktr)->SetDeleteFlag(true);
		}
	}
	
	//!Cable에 Assign된 Drum Id를 초기화
	docData.GetELoadItemListOf(CableItemList, CCableItem::TypeString() , "*");
	{
		for(list<CELoadItem*>::iterator ktr = CableItemList.begin(); ktr != CableItemList.end();++ktr)
		{
			CCableItem* pCableItem = static_cast<CCableItem*>(*ktr);
			if(pCableItem)
			{
				pCableItem->prop()->SetValue(_T("General") ,_T("Drum Id"), _T(""));
				pCableItem->prop()->SetOriginalValue(_T("General") ,_T("Drum Id"), _T(""));
			}
		}
	}
	
	if(pTable)
	{
		CString rMinSpare;
		m_wndMinSpareLenEdit.GetWindowText(rMinSpare);
		if(rMinSpare.IsEmpty()) 
		{
			AfxMessageBox(_T("Minium Spare Length를 입력하세요!"), MB_OK|MB_ICONWARNING);
			return ERROR_BAD_ENVIRONMENT;
		}
		
		::stable_sort(pTable->m_RecordEntry.begin(), pTable->m_RecordEntry.end(), fnCompareCableType);
		//::stable_sort(CableItemList.begin(), CableItemList.end(), fnCompareCableItemLength);

		for(vector<TABLES::CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin(); itr != pTable->m_RecordEntry.end();++itr)
		{
			const string rStdCableLength  = itr->FieldValueMap[_T("C_STD_CABLE_LENGTH")];
			const string rCableVolt    = itr->FieldValueMap[_T("C_VOLTAGE")];
			const string rCableSpec    = itr->FieldValueMap[_T("C_SPEC")];
			const string rCableCore    = itr->FieldValueMap[_T("C_CORE")];
			const string rCableSize    = itr->FieldValueMap[_T("C_SIZE")];
			const string rPrefix       = itr->FieldValueMap[_T("C_PREFIX")];
			
			//!드럼 사이즈는 std cable len - min spare
			const double nDrumSize = atof(rStdCableLength.c_str()) -atof(rMinSpare);

			//! Prefix 같은게 존재하면
			if(ComparePrefix(rPrefix))
			{
				::MessageBox(NULL, "Prefix ( " + CString(rPrefix.c_str()) + " )가 이미 존재합니다","Warning", MB_OK|MB_ICONWARNING);
				continue;
			}

			vector<LOAD_CABLE_RECORD> TOTAL_CABLE_ENTRY;
			for(list<CELoadItem*>::iterator jtr = CableItemList.begin(); jtr != CableItemList.end();++jtr)
			{
				CCableItem* pCableItem = static_cast<CCableItem*>(*jtr);
				if(pCableItem)
				{
					if((rCableVolt == pCableItem->prop()->GetValue(_T("General") ,_T("Cable Volt"))) &&
						(rCableSpec == pCableItem->prop()->GetValue(_T("General") ,_T("Cable Spec"))) &&
						(rCableCore == pCableItem->prop()->GetValue(_T("General") ,_T("Core"))) &&
						(rCableSize == pCableItem->prop()->GetValue(_T("General") ,_T("Size"))))
					{
						//! Length 가 없으면 패스~ 
						const CString rCableLen = pCableItem->prop()->GetValue(_T("General") ,_T("Length")).c_str();
						if(rCableLen.IsEmpty()) continue;

						///! Assigned 된 CableItem을 골라낸다.
						const string rLoadId = pCableItem->prop()->GetValue(_T("General"), _T("Load Id")).c_str();
						
						CString rCableLoadId = rLoadId.c_str();
						// 그룹이 아닌 일반 Load
						char cAlpha = rCableLoadId[rCableLoadId.GetLength() - 1];
						if(isalpha(cAlpha))
						{
							rCableLoadId = rCableLoadId.Left(rCableLoadId.GetLength() -1);
						}

						vector<LOAD_CABLE_RECORD>::iterator where = find_if(TOTAL_CABLE_ENTRY.begin(), TOTAL_CABLE_ENTRY.end(), bind2nd(MatchLoadIdCable(), rCableLoadId));
						if(where !=TOTAL_CABLE_ENTRY.end())
						{
							where->m_CableItemEntry.push_back(pCableItem);
							where->nTotalCableLength += atof(rCableLen);
							//::stable_sort(where->m_CableItemEntry.begin(), where->m_CableItemEntry.end(), fnCompareCableItemLength); 
						}
						else
						{
							LOAD_CABLE_RECORD record;
							record.rCableVolt = rCableVolt.c_str();
							record.rCableSpec = rCableSpec.c_str();
							record.rCableCore = rCableCore.c_str();
							record.rLoadID = rCableLoadId;
							record.m_CableItemEntry.push_back(pCableItem);
							record.bIsDeleted = false;
							record.nTotalCableLength = atof(rCableLen);
							record.nExtraCableLength = 0.;

							TOTAL_CABLE_ENTRY.push_back(record);
						}

					}
				}
			}
			//! Cable Length 순으로 정렬 (길이 긴놈부터)
			//::stable_sort(TOTAL_CABLE_ENTRY.begin(), TOTAL_CABLE_ENTRY.end(), fnCompareCableType);
			
			int nDrumCount = 1;
			double nTotalLength = 0.;
			CDrumItem* pDrumItem = NULL;
			
			for(vector<LOAD_CABLE_RECORD>::iterator jtr = TOTAL_CABLE_ENTRY.begin(); jtr != TOTAL_CABLE_ENTRY.end();++jtr)
			{
				if(jtr->bIsDeleted) continue;
				
				//! 드럼보다 긴 케이블 (단일 케이블)
				if((nDrumSize < jtr->nTotalCableLength) && (1 == jtr->m_CableItemEntry.size())) 
				{
					jtr->nExtraCableLength = jtr->nTotalCableLength - nDrumSize;
					jtr->nTotalCableLength = nDrumSize;
				}
				else if((nDrumSize < jtr->nTotalCableLength) && (1 < jtr->m_CableItemEntry.size()))
				{
					//! 드럼보다 긴 케이블 (여러 케이블)
					pDrumItem = DrumScheduleOfGroupCable(pDrumItem, *itr, *jtr, nDrumCount, nTotalLength, rMinSpare, nDrumSize);
					continue;
				}
				if(0. == nTotalLength || ((nDrumSize - nTotalLength) < jtr->nTotalCableLength))
				{
					//! CABLE을 쌓고 난 뒤에 DRUM의 남는 부분에 맞는 CABLE을 찾는다.
					if(((nDrumSize - nTotalLength) < jtr->nTotalCableLength))
					{
						IsFoundCableForDrumSchedule(pDrumItem, TOTAL_CABLE_ENTRY, nDrumSize, nTotalLength, rMinSpare);
						
					}
					pDrumItem = static_cast<CDrumItem*>(docData.CreateELoadItemOf(CDrumItem::TypeString()));
					if(pDrumItem)
					{
						CString rTemp;
						rTemp.Format(CString(rPrefix.c_str()) + _T("-%d") , nDrumCount);
						pDrumItem->prop()->SetValue(_T("General"), _T("Drum Id"), rTemp.operator LPCSTR());
						pDrumItem->prop()->SetOriginalValue(_T("General"), _T("Drum Id"), rTemp.operator LPCSTR());

						pDrumItem->prop()->SetValue(_T("General"), _T("Cable Volt"), rCableVolt);
						pDrumItem->prop()->SetOriginalValue(_T("General"), _T("Cable Volt"), rCableVolt);

						pDrumItem->prop()->SetValue(_T("General"), _T("Cable Spec"), rCableSpec);
						pDrumItem->prop()->SetOriginalValue(_T("General"), _T("Cable Spec"), rCableSpec);

						pDrumItem->prop()->SetValue(_T("General"), _T("Core"), rCableCore);
						pDrumItem->prop()->SetOriginalValue(_T("General"), _T("Core"), rCableCore);

						pDrumItem->prop()->SetValue(_T("General"), _T("Size"), rCableSize);
						pDrumItem->prop()->SetOriginalValue(_T("General"), _T("Size"), rCableSize);

						nTotalLength = 0.;
						++nDrumCount;
					}
				}


				for(vector<CCableItem*>::iterator ktr = jtr->m_CableItemEntry.begin(); ktr != jtr->m_CableItemEntry.end();++ktr)
				{
					CDrumItem::CABLE_RECORD record;
					record.rCableId = (*ktr)->GetName().c_str();

					CString rCableLength;
					rCableLength = (*ktr)->prop()->GetValue(_T("General"),_T("Length")).c_str();

					if(nDrumSize < atof(rCableLength)) rCableLength.Format("%.f", jtr->nTotalCableLength);
					record.rLength = rCableLength;
					record.rFrom = (*ktr)->prop()->GetValue(_T("General"),_T("From")).c_str();
					record.rTo = (*ktr)->prop()->GetValue(_T("General"),_T("To")).c_str();
					record.rArea = (*ktr)->prop()->GetValue(_T("General"),_T("Area")).c_str();

					const string rDrumId = pDrumItem->prop()->GetValue(_T("General"), _T("Drum Id"));

					pDrumItem->AddCable(record);

					(*ktr)->prop()->SetValue(_T("General"),_T("Drum Id"), rDrumId);
					(*ktr)->prop()->SetOriginalValue(_T("General"),_T("Drum Id"), rDrumId);
				}

				nTotalLength += jtr->nTotalCableLength;
				jtr->bIsDeleted = true;

				CString rSpareLen;
				rSpareLen.Format("%.f", nTotalLength);

				rSpareLen = rSpareLen.Right(2);
				double nExtraSpare = (atof(rSpareLen) > 0. )?100. - atof(rSpareLen) :0.;
				rSpareLen.Format("%.f", (nExtraSpare >= atof(rMinSpare)) ? nExtraSpare : nExtraSpare + 100. );

				pDrumItem->prop()->SetValue(_T("General"), _T("Spare Length"), rSpareLen.operator LPCSTR());
				pDrumItem->prop()->SetOriginalValue(_T("General"), _T("SpareLength"), rSpareLen.operator LPCSTR());

				CString rDrumLen;
				rDrumLen.Format("%.f", nTotalLength + atof(rSpareLen));

				pDrumItem->prop()->SetValue(_T("General"), _T("Length"), rDrumLen.operator LPCSTR());
				pDrumItem->prop()->SetOriginalValue(_T("General"), _T("Length"), rDrumLen.operator LPCSTR());


				//! CABLE의 길이가 Drum보다 길때 
				if(jtr->nExtraCableLength != 0.)
				{
					while(jtr->nExtraCableLength != 0)
					{
						pDrumItem = DrumScheduleOfConnectCable(*itr, *jtr, nDrumCount, nTotalLength, rMinSpare, nDrumSize);
					}
				}
			}
		}
		return ERROR_SUCCESS;
	}
	return ERROR_BAD_ENVIRONMENT;
}


/**
	@brief	

	@author KHS	

	@date 2009-10-19 오후 2:20:05	

	@param	

	@return		
**/
void CDrumScheduleCreationDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if(IDC_CABLE_SPEC_LIST == nIDCtl)
	{
		lpMeasureItemStruct->itemHeight = 20;
	}else	CDialog::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

/**
	@brief	

	@author BHK	

	@date 2009-09-23 오전 9:13:16	

	@param	

	@return		
**/
BOOL CDrumScheduleCreationDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if(m_wndDrumTreeListCtrl.GetDlgCtrlID() == wParam)
	{
		TREELISTINFO* ptlistinfo = (TREELISTINFO*)(lParam);
		if(ptlistinfo && (WM_RBUTTONUP == ptlistinfo->code))
		{
			m_wndDrumTreeListCtrl.SelectItem(ptlistinfo->hItem , ptlistinfo->nCol);
			if(NULL != m_wndDrumTreeListCtrl.GetParentItem(ptlistinfo->hItem))
			{
				CPoint point;
				::GetCursorPos(&point);
				CMenu menu;
				if(menu.CreatePopupMenu())
				{
					menu.AppendMenu(0 , ID_CABLE_DATA_DELETE , _T("Delete"));
					menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, point.y, this);
				}
			}
		}

		return TRUE;
	}else return CDialog::OnNotify(wParam, lParam, pResult);
}

/**
	@brief	

	@author BHK	

	@date 2009-09-23 오전 9:09:34	

	@param	

	@return		
**/
void CDrumScheduleCreationDlg::OnCableDataDelete()
{
	HTREEITEM hSelItem = m_wndDrumTreeListCtrl.GetSelectedItem();
	if(hSelItem)
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		CString rSelItemText = m_wndDrumTreeListCtrl.GetItemText(hSelItem);

		CABLE_ITEM_STRUCTURE cableitems[2];

		cableitems[0].rCableVolt = m_wndDrumTreeListCtrl.GetItemText(hSelItem , 1);
		cableitems[0].rSpec = m_wndDrumTreeListCtrl.GetItemText(hSelItem , 2);
		cableitems[0].rCore = m_wndDrumTreeListCtrl.GetItemText(hSelItem , 3);
		cableitems[0].rSize = m_wndDrumTreeListCtrl.GetItemText(hSelItem , 4);
		CELOADTable* pTable = docData.GetTableOf(_T("T_DRUM_SCHEDULE_SETTING"));
		if(pTable)
		{
			for(vector<TABLES::CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin();itr != pTable->m_RecordEntry.end();++itr)
			{
				cableitems[1].rCableVolt = itr->FieldValueMap[_T("C_VOLTAGE")];
				cableitems[1].rSpec = itr->FieldValueMap[_T("C_SPEC")];
				cableitems[1].rCore = itr->FieldValueMap[_T("C_CORE")];
				cableitems[1].rSize = itr->FieldValueMap[_T("C_SIZE")];

				if(cableitems[0] == cableitems[1])
				{
					pTable->m_RecordEntry.erase(itr);
					break;
				}
			}
			pTable->Save();
		}

		m_wndDrumTreeListCtrl.DeleteItem(hSelItem);
		DisplayCableSpec();
	}
	else
	{
		::MessageBox(NULL, _T("먼저 Cable Data을 선택하세요") , _T("Warning") , MB_OK|MB_ICONWARNING);
		return;
	}
}

/**
	@brief	

	@author KHS	

	@date 2009-10-08 오후 2:12:26	

	@param	

	@return		
**/
bool CDrumScheduleCreationDlg::ComparePrefix(const string& rPrefix)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	CELOADTable* pTable = docData.GetTableOf(_T("T_DRUM_SCHEDULE_SETTING"));

	if(pTable)
	{
		int nCount = 0;
		for(vector<TABLES::CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin(); itr != pTable->m_RecordEntry.end();++itr)
		{
			if(rPrefix == itr->FieldValueMap[_T("C_PREFIX")])
			{
				++nCount;
			}
			if(1 < nCount) return true;
		}
	}
	return false;
}


/**
	@brief	Prefix를 다시 읽어온다.

	@author KHS	

	@date 2009-10-22 오전 9:54:19	

	@param	

	@return		
**/
void CDrumScheduleCreationDlg::OnBnClickedRefreshButton()
{
	DisplayCableSpec(false);
}

/**
	@brief	TOTAL_CABLE_ENTRY가 비었는지 확인

	@author KHS	

	@date 2009-10-23 오후 1:30:11	

	@param	

	@return		
**/
bool CDrumScheduleCreationDlg::IsTotalCableEntryEmpty(const vector<LOAD_CABLE_RECORD>& TotalCableEntry)
{
	vector<LOAD_CABLE_RECORD>::const_iterator where = find_if(TotalCableEntry.begin(), TotalCableEntry.end(), bind2nd(MatchIsDeleted(), false));
	if(where != TotalCableEntry.end()) return false;

	return true;
}

/**
	@brief	드럼에 다음으로 들어가기에 맞는 케이블을 찾은 후 cable을 쌓는다.

	@author KHS	

	@date 2009-10-26 오후 2:24:03	

	@param	

	@return	 찾아서 넣으면 true  못찾았으면 false
**/
bool CDrumScheduleCreationDlg::IsFoundCableForDrumSchedule(CDrumItem* pDrumItem ,vector<LOAD_CABLE_RECORD>& TotalCableEntry, const double& nDrumSize, double& nTotalLength, const CString& rMinSpare)
{
	const double nExtraDrum = nDrumSize - nTotalLength;
	for(vector<LOAD_CABLE_RECORD>::iterator itr = TotalCableEntry.begin(); itr != TotalCableEntry.end(); ++itr)
	{
		if((itr->bIsDeleted == false) && (nExtraDrum > itr->nTotalCableLength))
		{
			for(vector<CCableItem*>::iterator ktr = itr->m_CableItemEntry.begin(); ktr != itr->m_CableItemEntry.end();++ktr)
			{
				CDrumItem::CABLE_RECORD record;
				record.rCableId = (*ktr)->GetName().c_str();
				record.rLength = (*ktr)->prop()->GetValue(_T("General"),_T("Length")).c_str();
				record.rFrom = (*ktr)->prop()->GetValue(_T("General"),_T("From")).c_str();
				record.rTo = (*ktr)->prop()->GetValue(_T("General"),_T("To")).c_str();
				record.rArea = (*ktr)->prop()->GetValue(_T("General"),_T("Area")).c_str();

				const string rDrumId = pDrumItem->prop()->GetValue(_T("General"), _T("Drum Id"));

				pDrumItem->AddCable(record);

				(*ktr)->prop()->SetValue(_T("General"),_T("Drum Id"), rDrumId);
				(*ktr)->prop()->SetOriginalValue(_T("General"),_T("Drum Id"), rDrumId);

			}

			nTotalLength += itr->nTotalCableLength;
			itr->bIsDeleted = true;

			CString rSpareLen;
			rSpareLen.Format("%.f", nTotalLength);

			rSpareLen = rSpareLen.Right(2);
			double nExtraSpare = (atof(rSpareLen) > 0. )?100. - atof(rSpareLen) :0.;
			rSpareLen.Format("%.f", (nExtraSpare >= atof(rMinSpare)) ? nExtraSpare : nExtraSpare + 100. );

			pDrumItem->prop()->SetValue(_T("General"), _T("Spare Length"), rSpareLen.operator LPCSTR());
			pDrumItem->prop()->SetOriginalValue(_T("General"), _T("SpareLength"), rSpareLen.operator LPCSTR());

			CString rDrumLen;
			rDrumLen.Format("%.f", nTotalLength + atof(rSpareLen));

			pDrumItem->prop()->SetValue(_T("General"), _T("Length"), rDrumLen.operator LPCSTR());
			pDrumItem->prop()->SetOriginalValue(_T("General"), _T("Length"), rDrumLen.operator LPCSTR());

			return true;
		}
	}

	return false;
}

/**
	@brief	

	@author KHS	

	@date 2009-10-26 오후 4:45:38	

	@param	

	@return		
**/
CDrumItem* CDrumScheduleCreationDlg::DrumScheduleOfConnectCable(CELOADTable::Record& record, LOAD_CABLE_RECORD& LoadCablRecord, int& nDrumCount, double& nTotalLength, const CString& rMinSpare, const double& nDrumSize)
{
	const string rCableVolt    = record.FieldValueMap[_T("C_VOLTAGE")];
	const string rCableSpec    = record.FieldValueMap[_T("C_SPEC")];
	const string rCableCore    = record.FieldValueMap[_T("C_CORE")];
	const string rCableSize    = record.FieldValueMap[_T("C_SIZE")];
	const string rPrefix       = record.FieldValueMap[_T("C_PREFIX")];
	
	CELoadDocData& docData = CELoadDocData::GetInstance();
	
	LoadCablRecord.nTotalCableLength = LoadCablRecord.nExtraCableLength;

	if(LoadCablRecord.nTotalCableLength > nDrumSize)
	{
		LoadCablRecord.nExtraCableLength = LoadCablRecord.nTotalCableLength - nDrumSize;
		LoadCablRecord.nTotalCableLength = nDrumSize;
	}
	else
	{
		LoadCablRecord.nExtraCableLength = 0.;
	}

	CDrumItem* pDrumItem = static_cast<CDrumItem*>(docData.CreateELoadItemOf(CDrumItem::TypeString()));
	if(pDrumItem)
	{
		CString rTemp;
		rTemp.Format(CString(rPrefix.c_str()) + _T("-%d") , nDrumCount);
		pDrumItem->prop()->SetValue(_T("General"), _T("Drum Id"), rTemp.operator LPCSTR());
		pDrumItem->prop()->SetOriginalValue(_T("General"), _T("Drum Id"), rTemp.operator LPCSTR());

		pDrumItem->prop()->SetValue(_T("General"), _T("Cable Volt"), rCableVolt);
		pDrumItem->prop()->SetOriginalValue(_T("General"), _T("Cable Volt"), rCableVolt);

		pDrumItem->prop()->SetValue(_T("General"), _T("Cable Spec"), rCableSpec);
		pDrumItem->prop()->SetOriginalValue(_T("General"), _T("Cable Spec"), rCableSpec);

		pDrumItem->prop()->SetValue(_T("General"), _T("Core"), rCableCore);
		pDrumItem->prop()->SetOriginalValue(_T("General"), _T("Core"), rCableCore);

		pDrumItem->prop()->SetValue(_T("General"), _T("Size"), rCableSize);
		pDrumItem->prop()->SetOriginalValue(_T("General"), _T("Size"), rCableSize);

		nTotalLength = 0.;
		++nDrumCount;
	}

	for(vector<CCableItem*>::iterator ktr = LoadCablRecord.m_CableItemEntry.begin(); ktr != LoadCablRecord.m_CableItemEntry.end();++ktr)
	{
		CDrumItem::CABLE_RECORD cablerecord;
		cablerecord.rCableId = (*ktr)->GetName().c_str();

		CString rCableLength;
		rCableLength.Format("%.f", LoadCablRecord.nTotalCableLength);
		cablerecord.rLength = rCableLength;//(*ktr)->prop()->GetValue(_T("General"),_T("Length")).c_str();
		cablerecord.rFrom = (*ktr)->prop()->GetValue(_T("General"),_T("From")).c_str();
		cablerecord.rTo = (*ktr)->prop()->GetValue(_T("General"),_T("To")).c_str();
		cablerecord.rArea = (*ktr)->prop()->GetValue(_T("General"),_T("Area")).c_str();

		const string rDrumId = pDrumItem->prop()->GetValue(_T("General"), _T("Drum Id"));

		pDrumItem->AddCable(cablerecord);

		(*ktr)->prop()->SetValue(_T("General"),_T("Drum Id"), rDrumId);
		(*ktr)->prop()->SetOriginalValue(_T("General"),_T("Drum Id"), rDrumId);

	}

	nTotalLength += LoadCablRecord.nTotalCableLength;
	LoadCablRecord.bIsDeleted = true;

	CString rSpareLen;
	rSpareLen.Format("%.f", nTotalLength);

	rSpareLen = rSpareLen.Right(2);
	double nExtraSpare = (atof(rSpareLen) > 0. )?100. - atof(rSpareLen) :0.;
	rSpareLen.Format("%.f", (nExtraSpare >= atof(rMinSpare)) ? nExtraSpare : nExtraSpare + 100. );

	pDrumItem->prop()->SetValue(_T("General"), _T("Spare Length"), rSpareLen.operator LPCSTR());
	pDrumItem->prop()->SetOriginalValue(_T("General"), _T("SpareLength"), rSpareLen.operator LPCSTR());

	CString rDrumLen;
	rDrumLen.Format("%.f", nTotalLength + atof(rSpareLen));

	pDrumItem->prop()->SetValue(_T("General"), _T("Length"), rDrumLen.operator LPCSTR());
	pDrumItem->prop()->SetOriginalValue(_T("General"), _T("Length"), rDrumLen.operator LPCSTR());

	return pDrumItem;
}

/**
	@brief	Cable Group이 DrumSize 보다 클때 케이블을 나눠 쌓는다.

	@author KHS	

	@date 2009-11-02 오후 3:51:14	

	@param	

	@return		
**/
CDrumItem* CDrumScheduleCreationDlg::DrumScheduleOfGroupCable(CDrumItem* pDrumItem, CELOADTable::Record& record, LOAD_CABLE_RECORD& LoadCablRecord, int& nDrumCount, double& nTotalLength, const CString& rMinSpare, const double& nDrumSize)
{
	const string rCableVolt    = record.FieldValueMap[_T("C_VOLTAGE")];
	const string rCableSpec    = record.FieldValueMap[_T("C_SPEC")];
	const string rCableCore    = record.FieldValueMap[_T("C_CORE")];
	const string rCableSize    = record.FieldValueMap[_T("C_SIZE")];
	const string rPrefix       = record.FieldValueMap[_T("C_PREFIX")];
	
	CELoadDocData& docData = CELoadDocData::GetInstance();
	//nTotalLength = 0.;
	//CDrumItem* pDrumItem = NULL;
	for(vector<CCableItem*>::iterator ktr = LoadCablRecord.m_CableItemEntry.begin(); ktr != LoadCablRecord.m_CableItemEntry.end();++ktr)
	{
		CDrumItem::CABLE_RECORD cablerecord;
		cablerecord.rCableId = (*ktr)->GetName().c_str();
		CString rCableLength = (*ktr)->prop()->GetValue(_T("General"),_T("Length")).c_str();
		cablerecord.rLength = rCableLength;
		cablerecord.rFrom = (*ktr)->prop()->GetValue(_T("General"),_T("From")).c_str();
		cablerecord.rTo = (*ktr)->prop()->GetValue(_T("General"),_T("To")).c_str();
		cablerecord.rArea = (*ktr)->prop()->GetValue(_T("General"),_T("Area")).c_str();

		if(0. == nTotalLength || ((nDrumSize - nTotalLength) < atof(cablerecord.rLength)))
		{
			pDrumItem = static_cast<CDrumItem*>(docData.CreateELoadItemOf(CDrumItem::TypeString()));
			if(pDrumItem)
			{
				CString rTemp;
				rTemp.Format(CString(rPrefix.c_str()) + _T("-%d") , nDrumCount);
				pDrumItem->prop()->SetValue(_T("General"), _T("Drum Id"), rTemp.operator LPCSTR());
				pDrumItem->prop()->SetOriginalValue(_T("General"), _T("Drum Id"), rTemp.operator LPCSTR());

				pDrumItem->prop()->SetValue(_T("General"), _T("Cable Volt"), rCableVolt);
				pDrumItem->prop()->SetOriginalValue(_T("General"), _T("Cable Volt"), rCableVolt);

				pDrumItem->prop()->SetValue(_T("General"), _T("Cable Spec"), rCableSpec);
				pDrumItem->prop()->SetOriginalValue(_T("General"), _T("Cable Spec"), rCableSpec);

				pDrumItem->prop()->SetValue(_T("General"), _T("Core"), rCableCore);
				pDrumItem->prop()->SetOriginalValue(_T("General"), _T("Core"), rCableCore);

				pDrumItem->prop()->SetValue(_T("General"), _T("Size"), rCableSize);
				pDrumItem->prop()->SetOriginalValue(_T("General"), _T("Size"), rCableSize);

				nTotalLength = 0.;
				++nDrumCount;
			}
		}

		nTotalLength += atof(rCableLength);
		pDrumItem->AddCable(cablerecord);

		const string rDrumId = pDrumItem->prop()->GetValue(_T("General"), _T("Drum Id"));

		(*ktr)->prop()->SetValue(_T("General"),_T("Drum Id"), rDrumId);
		(*ktr)->prop()->SetOriginalValue(_T("General"),_T("Drum Id"), rDrumId);

		CString rSpareLen;
		rSpareLen.Format("%.f", nTotalLength);

		rSpareLen = rSpareLen.Right(2);
		double nExtraSpare = (atof(rSpareLen) > 0. )?100. - atof(rSpareLen) :0.;
		rSpareLen.Format("%.f", (nExtraSpare >= atof(rMinSpare)) ? nExtraSpare : nExtraSpare + 100. );

		pDrumItem->prop()->SetValue(_T("General"), _T("Spare Length"), rSpareLen.operator LPCSTR());
		pDrumItem->prop()->SetOriginalValue(_T("General"), _T("SpareLength"), rSpareLen.operator LPCSTR());

		CString rDrumLen;
		rDrumLen.Format("%.f", nTotalLength + atof(rSpareLen));

		pDrumItem->prop()->SetValue(_T("General"), _T("Length"), rDrumLen.operator LPCSTR());
		pDrumItem->prop()->SetOriginalValue(_T("General"), _T("Length"), rDrumLen.operator LPCSTR());

	}

	LoadCablRecord.bIsDeleted = true;


	return pDrumItem;
}

/**
	@brief	Minimum Spare 값을 가져온다.

	@author KHS	

	@date 2009-10-30 오전 9:22:28	

	@param	

	@return		
**/
int CDrumScheduleCreationDlg::LoadData(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string rMDBFilePath = docData.GetProjectMDBFilePath();

	CELOADTable* pDrumSpareTable = docData.GetTableOf(_T("T_DRUM_SCHEDULE_SPARE_SETTING"));
	if(pDrumSpareTable)
	{
		if(!pDrumSpareTable->m_RecordEntry.empty())
		{
			vector<TABLES::CELOADTable::Record>::iterator itr = pDrumSpareTable->m_RecordEntry.begin();
			{
				m_wndMinSpareLenEdit.SetWindowText(itr->FieldValueMap[_T("C_MIN_SPARE")].c_str());
				return ERROR_SUCCESS;
			}
		}
	}
	return ERROR_BAD_ENVIRONMENT;
}
