// CableScheduleSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "CableScheduleSettingDlg.h"
#include "ELoadDocData.h"
#include "MainFrm.h"

// CCableScheduleSettingDlg dialog

IMPLEMENT_DYNAMIC(CCableScheduleSettingDlg, CDialog)

CCableScheduleSettingDlg::CCableScheduleSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCableScheduleSettingDlg::IDD, pParent)
{

}

CCableScheduleSettingDlg::~CCableScheduleSettingDlg()
{
	try
	{
		if(m_pFont) SAFE_DELETE(m_pFont);
	}
	catch(...)
	{
		
	}
}

void CCableScheduleSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_LIST_BUS , m_wndBusIDListBox);
	DDX_Control(pDX , IDC_BUTTON_CREATION , m_wndCreationButton);
	DDX_Control(pDX , IDOK , m_wndOKButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);
}


BEGIN_MESSAGE_MAP(CCableScheduleSettingDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CCableScheduleSettingDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CCableScheduleSettingDlg::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_CREATION, &CCableScheduleSettingDlg::OnBnClickedButtonCreation)
END_MESSAGE_MAP()


// CCableScheduleSettingDlg message handlers

BOOL CCableScheduleSettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pFont = new CFont;
	m_pFont->CreateFont(16,0,0,0,600,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,_T("Arial"));
	m_wndBusIDListBox.SetFont(m_pFont);

	CELoadDocData& docData = CELoadDocData::GetInstance();
	TABLES::CELOADTable* pTable = docData.GetTableOf(_T("T_CABLE_SCHEDULE_SETTING"));
	if(pTable)
	{
		list<string> BusIDEntry;
		list<CELoadItem*> BusGroupList;
		docData.GetELoadItemListOf(BusGroupList , CBusGroupItem::TypeString());
		for(list<CELoadItem*>::iterator itr = BusGroupList.begin();itr != BusGroupList.end();++itr)
		{
			CBusGroupItem* pBusGroup = static_cast<CBusGroupItem*>(*itr);
			if(pBusGroup)
			{
				for(vector<CBusItem*>::iterator jtr = pBusGroup->m_BusRefEntry.begin(); jtr != pBusGroup->m_BusRefEntry.end();++jtr)
				{
					///! 삭제가 안된 BUS만 넘겨준다.
					if((*jtr)->IsDeleted()) continue;
					BusIDEntry.push_back((*jtr)->GetName());
				}
			}
		}
		
		//! USER BUS ITEM ADD
		{
			list<CELoadItem*> UserBusItemList;
			docData.GetELoadItemListOf(UserBusItemList, CBusItem::TypeString(), _T("User"));
			for(list<CELoadItem*>::iterator itr = UserBusItemList.begin(); itr != UserBusItemList.end();++itr)
			{
				const string rBusID = (*itr)->GetName();
				list<string>::iterator where = find(BusIDEntry.begin(), BusIDEntry.end(), rBusID);
				if(where == BusIDEntry.end())
				{
					CBusItem* pBusItem = static_cast<CBusItem*>(*itr);
					list<CELoadItem*> CableItemList;
					pBusItem->GetCableList(CableItemList);
					if(!CableItemList.empty()) BusIDEntry.push_back((*itr)->GetName());
				}
			}
		}

		{
			list<CELoadItem*> CableItemList;
			docData.GetELoadItemListOf(CableItemList , CCableItem::TypeString());
			for(list<CELoadItem*>::iterator itr = CableItemList.begin();itr != CableItemList.end();++itr)
			{
				const string rCreatedBy = (*itr)->prop()->GetValue(_T("General") , _T("Created By"));
				if(_T("User") == rCreatedBy)
				{
					const string rPanelID = (*itr)->prop()->GetValue(_T("General") , _T("Panel ID"));
					list<string>::iterator where = find(BusIDEntry.begin() , BusIDEntry.end() , rPanelID);
					if(where == BusIDEntry.end()) BusIDEntry.push_back(rPanelID);
				}
			}
		}

		{
			for(vector<CELOADTable::Record>::iterator itr = pTable->m_RecordEntry.begin();itr != pTable->m_RecordEntry.end();)
			{
				const string rBusID = itr->FieldValueMap[_T("C_BUS_ID")];
				list<string>::iterator where = find(BusIDEntry.begin() , BusIDEntry.end() , rBusID);
				if(where != BusIDEntry.end())
				{
					BusIDEntry.erase(where);
					m_wndBusIDListBox.AddString(rBusID.c_str());
					++itr;
				}
				else
				{
					itr = pTable->m_RecordEntry.erase(itr);
				}
			}

			for(list<string>::iterator itr = BusIDEntry.begin();itr != BusIDEntry.end();++itr)
			{
				m_wndBusIDListBox.AddString(itr->c_str());
			}
		}
	}

	m_wndCreationButton.SetIcon(IDI_OK_18X18);
	m_wndOKButton.SetIcon(IDI_SAVE);
	m_wndCancelButton.SetIcon(IDI_CANCEL_18X18);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	사용자가 정의한 순서대로 BUS ID를 Database에 저장합니다.

	@author BHK	

	@date 2009-06-30 오후 4:27:52	

	@param	

	@return		
*/
void CCableScheduleSettingDlg::OnBnClickedOk()
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	TABLES::CELOADTable* pTable = docData.GetTableOf(_T("T_CABLE_SCHEDULE_SETTING"));
	if(pTable)
	{
		pTable->m_RecordEntry.clear();

		const int nItemCount = m_wndBusIDListBox.GetCount();
		for(int i = 0;i < nItemCount;++i)
		{
			CString rString;
			m_wndBusIDListBox.GetText(i , rString);

			CELOADTable::Record record;
			record.FieldValueMap.insert(make_pair(_T("C_BUS_ID") , rString.operator LPCSTR()));
			pTable->m_RecordEntry.push_back(record);
		}
		//! 기존 테이블의 내용을 삭제하고 INSERT하도록 한다.
		pTable->Save();
	}

	OnOK();
}

/**
	@brief	현재 선택한 BUS ID를 리스트에서 삭제합니다.

	@author BHK	

	@date 2009-09-03 오후 12:58:00	

	@param	

	@return		
*/
void CCableScheduleSettingDlg::OnBnClickedButtonDelete()
{
	const int nSel = m_wndBusIDListBox.GetCurSel();
	if(-1 != nSel) m_wndBusIDListBox.DeleteString(nSel);
}

/**
	@brief	Cable Schedule을 생성합니다.

	@author BHK	

	@date 2009-09-09 오후 4:43:09	

	@param	

	@return		
*/
void CCableScheduleSettingDlg::OnBnClickedButtonCreation()
{
	OnBnClickedOk();

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if(pFrame) pFrame->OnCableScheduleCreation();
}
