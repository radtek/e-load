// SelectProject.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "SelectProjectDlg.h"
#include "ELoadDocData.h"


// CSelectProjectDlg dialog

IMPLEMENT_DYNAMIC(CSelectProjectDlg, CDialog)

CSelectProjectDlg::CSelectProjectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectProjectDlg::IDD, pParent)
        , m_rSelectedProjectEdit(_T(""))
{

}

CSelectProjectDlg::~CSelectProjectDlg()
{
}

void CSelectProjectDlg::DoDataExchange(CDataExchange* pDX)
{
        CDialog::DoDataExchange(pDX);
        DDX_Control(pDX, IDC_SELECT_PROJECT, m_wndSelectProjectList);
        DDX_Control(pDX, IDOK, m_wndOKButton);
        DDX_Control(pDX, IDCANCEL, m_wndCancelButton);
        DDX_Text(pDX, IDC_SELECT_PROJECT_EDIT, m_rSelectedProjectEdit);
}


BEGIN_MESSAGE_MAP(CSelectProjectDlg, CDialog)
        ON_NOTIFY(NM_CLICK, IDC_SELECT_PROJECT, &CSelectProjectDlg::OnNMClickSelectProject)
        ON_NOTIFY(NM_DBLCLK, IDC_SELECT_PROJECT, &CSelectProjectDlg::OnNMDblclkSelectProject)
        ON_NOTIFY(LVN_KEYDOWN, IDC_SELECT_PROJECT, &CSelectProjectDlg::OnLvnKeydownSelectProject)
END_MESSAGE_MAP()


// CSelectProjectDlg message handlers

BOOL CSelectProjectDlg::OnInitDialog()
{
        CDialog::OnInitDialog();

        // TODO:  Add extra initialization here
        m_wndSelectProjectList.InsertColumn(0 , _T("Project No.")   , LVCFMT_LEFT , 90);
	m_wndSelectProjectList.InsertColumn(1 , _T("Project Name.")   , LVCFMT_LEFT , 120);
	m_wndSelectProjectList.InsertColumn(2 , _T("Project Desc.") , LVCFMT_LEFT , 300);
        m_wndSelectProjectList.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0,
                LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);

        CELoadDocData& docData = CELoadDocData::GetInstance();

        m_rProjectNo = docData.GetProjectNo().c_str();
        m_rProjectName = docData.GetProjectName().c_str();

	LoadProjectRecordFromDatabase();

	DisplayProjectRecord();

	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	m_wndOKButton.SetIcon(IDI_OK_18X18);
	m_wndOKButton.SetTextHotColor(TextHotColor);

	m_wndCancelButton.SetIcon(IDI_CANCEL_18X18);
	m_wndCancelButton.SetTextHotColor(TextHotColor);

        return TRUE;  // return TRUE unless you set the focus to a control
        // EXCEPTION: OCX Property Pages should return FALSE
}
/**
	@brief	등록된 PROJECT RECORD를 DB에서 읽는다.

	@author	KHS
*/
int CSelectProjectDlg::LoadProjectRecordFromDatabase(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	string rServerMDB = docData.GetServerFolderPath();
	if('\\' != rServerMDB.at(rServerMDB.length() - 1)) rServerMDB += "\\";
	rServerMDB += "ELOAD.MDB";

	///
	CADODB adoDB;
	const STRING_T rProvider = STRING_T(PROVIDER) + _T("Data Source=") + rServerMDB + DB_PASSWORD;

        if(TRUE == adoDB.DBConnect(rProvider))
        {
                STRING_T rQuery("SELECT * FROM T_ELOAD_PROJECT");
                try
                { 
                        adoDB.OpenQuery(rQuery);

                        LONG lRecordCount = 0;
                        adoDB.GetRecordCount(&lRecordCount);
			for(int i = 0;i < lRecordCount;++i)
			{
				STRING_T rValue;

				PROJECT_RECORD record;
				adoDB.GetFieldValue(i , "C_PROJECT_NO" , &rValue);
				record.rProjectNo   = rValue;
				adoDB.GetFieldValue(i , "C_PROJECT_NAME" , &rValue);
				record.rProjectName = rValue;
				adoDB.GetFieldValue(i , "C_PROJECT_DESC" , &rValue);
				record.rProjectDesc = rValue;

				m_ProjectRecordEntry.push_back(record);
			}
		}
		catch(...)
		{
		}
        }
	else
	{
                AfxMessageBox("Not Find DB... !");
                return ERROR_BAD_ENVIRONMENT;
	}

	return ERROR_SUCCESS;
}
/**
	@brief	PROJECT RECORD를 리스트에 표기한다.

	@author KHS
*/
int CSelectProjectDlg::DisplayProjectRecord(void)
{
	m_wndSelectProjectList.DeleteAllItems();	//! 기존의 항목을 삭제한다.

        CELoadDocData& docData = CELoadDocData::GetInstance();

        string rCurProjectNo = docData.GetProjectNo();
        string rCurProjectName = docData.GetProjectName();
	int nIndex = 0;
	for(list<PROJECT_RECORD>::iterator itr = m_ProjectRecordEntry.begin();itr != m_ProjectRecordEntry.end();++itr)
	{
		const int nItem = m_wndSelectProjectList.InsertItem(nIndex++ , itr->rProjectNo.c_str());
		if(-1 != nItem)
		{
			m_wndSelectProjectList.SetItemText(nItem , 1 , itr->rProjectName.c_str());
			m_wndSelectProjectList.SetItemText(nItem , 2 , itr->rProjectDesc.c_str());
                        if(rCurProjectNo == itr->rProjectNo.c_str() && rCurProjectName == itr->rProjectName)
                        {
                                m_rSelectedProjectEdit = itr->rProjectNo.c_str();
                                m_wndSelectProjectList.SetCheck(nItem);
                                m_wndSelectProjectList.SetItemState(nItem,LVIS_SELECTED ,LVIS_SELECTED);
                        }
                }
	}

        UpdateData(FALSE);
	return 0;
}
/**
	@brief	PROJECT LIST SELECT

	@author KHS
*/
void CSelectProjectDlg::OnNMClickSelectProject(NMHDR *pNMHDR, LRESULT *pResult)
{
        // TODO: Add your control notification handler code here
        NMLISTVIEW* pNMLV = (NMLISTVIEW*)pNMHDR;

        CPoint ptList= pNMLV->ptAction;

        LVHITTESTINFO htinfo;
        htinfo.pt    = ptList;
        htinfo.flags = LVHT_ONITEMICON;
        int nItem = m_wndSelectProjectList.HitTest (&htinfo);
        if(-1 != nItem)
        {
                m_wndSelectProjectList.SetItemState(nItem,LVIS_SELECTED ,LVIS_SELECTED);

                int nCount = m_wndSelectProjectList.GetItemCount();	
                if(BST_UNCHECKED == m_wndSelectProjectList.GetCheck(nItem))
                {
                        m_wndSelectProjectList.SetCheck(nItem);
                        m_rSelectedProjectEdit = m_wndSelectProjectList.GetItemText(nItem, 0);
                        m_rProjectNo = m_wndSelectProjectList.GetItemText(nItem, 0);
                        m_rProjectName = m_wndSelectProjectList.GetItemText(nItem, 1);
                        UpdateData(FALSE);
                }
                for(int nItem = 0; nItem < nCount; nItem++)
                {
                        if( pNMLV->iItem == nItem) continue;
                        ListView_SetCheckState(m_wndSelectProjectList.GetSafeHwnd(), nItem, FALSE);
                }
                *pResult = 1;
        }
        else
        {
                *pResult = 0;
        }
}
/**
	@brief	PROJECT LIST SELECT

	@author KHS
*/
void CSelectProjectDlg::OnNMDblclkSelectProject(NMHDR *pNMHDR, LRESULT *pResult)
{
        NMLISTVIEW* pNMLV = (NMLISTVIEW*)pNMHDR;

        CPoint ptList= pNMLV->ptAction;

        LVHITTESTINFO htinfo;
        htinfo.pt    = ptList;
        htinfo.flags = LVHT_ONITEMICON;
        int nItem = m_wndSelectProjectList.HitTest (&htinfo);
        if(-1 != nItem)
        {
                m_wndSelectProjectList.SetItemState(nItem,LVIS_SELECTED ,LVIS_SELECTED);

                int nCount = m_wndSelectProjectList.GetItemCount();	
                if(BST_UNCHECKED == m_wndSelectProjectList.GetCheck(nItem))
                {
                        m_wndSelectProjectList.SetCheck(nItem);
                        m_rSelectedProjectEdit = m_wndSelectProjectList.GetItemText(nItem, 0);
                        m_rProjectNo = m_wndSelectProjectList.GetItemText(nItem, 0);
                        m_rProjectName = m_wndSelectProjectList.GetItemText(nItem, 1);

                        UpdateData(FALSE);
                }
                for(int nItem = 0; nItem < nCount; nItem++)
                {
                        if( pNMLV->iItem == nItem) continue;
                        ListView_SetCheckState(m_wndSelectProjectList.GetSafeHwnd(), nItem, FALSE);
                }
                *pResult = 1;
        }
        else
        {
                *pResult = 0;
        }
}
/**
	@brief	PROJECT LIST KEYDOWN

	@author KHS
*/
void CSelectProjectDlg::OnLvnKeydownSelectProject(NMHDR *pNMHDR, LRESULT *pResult)
{
        LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
        // TODO: Add your control notification handler code here
        *pResult = 0;
        int nCurItem = m_wndSelectProjectList.GetSelectionMark();
        switch (pLVKeyDow->wVKey)
        {
        case VK_UP:
                {
                        if(nCurItem - 1 != -1)
                        {
                                m_wndSelectProjectList.SetCheck(nCurItem - 1);
                                m_wndSelectProjectList.SetItemState(nCurItem - 1,LVIS_SELECTED ,LVIS_SELECTED);
                                m_rProjectNo = m_wndSelectProjectList.GetItemText(nCurItem - 1, 0);
                                m_rProjectName = m_wndSelectProjectList.GetItemText(nCurItem - 1, 1);
                                m_rSelectedProjectEdit = m_rProjectNo;
                                UpdateData(FALSE);
                                for(int nItem = 0; nItem < m_wndSelectProjectList.GetItemCount(); nItem++)
                                {
                                        if( nItem == nCurItem - 1) continue;
                                        ListView_SetCheckState(m_wndSelectProjectList.GetSafeHwnd(), nItem, FALSE);
                                }
                        }
                }
                break;
        case VK_DOWN:
                {
                        if(nCurItem + 1 < m_wndSelectProjectList.GetItemCount())
                        {
                                m_wndSelectProjectList.SetCheck(nCurItem + 1);
                                m_wndSelectProjectList.SetItemState(nCurItem + 1,LVIS_SELECTED ,LVIS_SELECTED);
                                m_rProjectNo = m_wndSelectProjectList.GetItemText(nCurItem + 1, 0);
                                m_rProjectName = m_wndSelectProjectList.GetItemText(nCurItem + 1, 1);
                                m_rSelectedProjectEdit = m_rProjectNo;
                                UpdateData(FALSE);
                                for(int nItem = 0; nItem < m_wndSelectProjectList.GetItemCount(); nItem++)
                                {
                                        if( nItem == nCurItem + 1) continue;
                                        ListView_SetCheckState(m_wndSelectProjectList.GetSafeHwnd(), nItem, FALSE);
                                }
                        }
                }
                break;
        }
}
