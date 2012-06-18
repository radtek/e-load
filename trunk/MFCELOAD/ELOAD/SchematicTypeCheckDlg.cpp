// SchematicTypeCheckDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "SchematicTypeCheckDlg.h"
#include "ELoadDocData.h"

// CSchematicTypeCheckDlg dialog

IMPLEMENT_DYNAMIC(CSchematicTypeCheckDlg, CDialog)

CSchematicTypeCheckDlg::CSchematicTypeCheckDlg(const CString& rSchematicType, CWnd* pParent /*=NULL*/)
	: CDialog(CSchematicTypeCheckDlg::IDD, pParent), m_rSchematicType(rSchematicType)
{

}

CSchematicTypeCheckDlg::~CSchematicTypeCheckDlg()
{
}

void CSchematicTypeCheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_LIST, m_wndSchematicTypeListCtrl);
	DDX_Control(pDX, IDOK, m_wndOkButton);
}


BEGIN_MESSAGE_MAP(CSchematicTypeCheckDlg, CDialog)
END_MESSAGE_MAP()


// CSchematicTypeCheckDlg message handlers

BOOL CSchematicTypeCheckDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_hIcon = AfxGetApp()->LoadIcon(IDI_GRABBER);

	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	
	static const CString __HEADERS__[] = {_T("Schematic Type") , _T("Desc.")};
	
	m_wndSchematicTypeListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES);
	for(int i = 0;i < SIZE_OF_ARRAY(__HEADERS__);++i)
	{
		m_wndSchematicTypeListCtrl.InsertColumn(i , __HEADERS__[i]  , LVCFMT_LEFT , 150);
	}
	m_wndSchematicTypeListCtrl.SetImageList(&(pApp->m_ELoadImages), LVSIL_NORMAL);

	//! BUTTON ICON SETTING
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));

	m_wndOkButton.SetTextHotColor(TextHotColor);
	m_wndOkButton.SetIcon(IDI_OK_18X18);


	DisplaySchematicType();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	Schematic Type check list

	@author KHS	

	@date 2009-06-17 오후 1:36:49	

	@param	

	@return		
**/
int CSchematicTypeCheckDlg::DisplaySchematicType(void)
{
	m_wndSchematicTypeListCtrl.DeleteAllItems();

	vector<string> oResult;
	CTokenizer<CIsFromString>::Tokenize(oResult, m_rSchematicType.operator LPCSTR(), CIsFromString("\n"));
	if(!oResult.empty())
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		CELOADTable* pTable = docData.GetTableOf(_T("T_SCHEMATIC_TYPE"));
		
		int i = 0;
		for(vector<string>::iterator itr = oResult.begin(); itr != oResult.end();++itr,++i)
		{
			const int nItem = m_wndSchematicTypeListCtrl.InsertItem(m_wndSchematicTypeListCtrl.GetItemCount() , itr->c_str(), 1);
			if(pTable)
			{
				//! Schematic의 Desc.를 표기한다.
				for(vector<CELOADTable::Record>::iterator jtr = pTable->m_RecordEntry.begin();jtr != pTable->m_RecordEntry.end();++jtr)
				{
					const string rSchemType = jtr->FieldValueMap[_T("C_TYPE")];
					if(rSchemType == (*itr))
					{
						const string rSchemDesc = jtr->FieldValueMap[_T("C_DESC")];
						m_wndSchematicTypeListCtrl.SetItemText(nItem , 1 , rSchemDesc.c_str());
						break;
					}
				}
			}
		}
	}
	
	return 0;
}
