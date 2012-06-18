// SaveProductToRevisionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ELOAD.h"
#include "SaveProductToRevisionDlg.h"
#include "ELoadDocData.h"
#include "Tables/ELOADTable.h"
#include "TransformerSizingResultDoc.h"
#include "CableSizingResultDoc.h"
#include "ControlCableResultDoc.h"
#include <Zip\inc\ZipArchive.h>

// CSaveProductToRevisionDlg dialog

IMPLEMENT_DYNAMIC(CSaveProductToRevisionDlg, CDialog)

CSaveProductToRevisionDlg::CSaveProductToRevisionDlg(const CString& rProductName , CWnd* pParent /*=NULL*/)
	: CDialog(CSaveProductToRevisionDlg::IDD, pParent) , m_rProductName(rProductName)
{
	m_nLastRevNo = 0;
}

CSaveProductToRevisionDlg::~CSaveProductToRevisionDlg()
{
}

void CSaveProductToRevisionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_LIST_REVISION , m_wndRevisionListCtrl);
	DDX_Control(pDX , IDOK , m_wndOKButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);
}


BEGIN_MESSAGE_MAP(CSaveProductToRevisionDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSaveProductToRevisionDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSaveProductToRevisionDlg message handlers
/**
	@brief	

	@author humkyung	

	@date 2009-04-28 오전 10:30:03	

	@param	

	@return		
*/
BOOL CSaveProductToRevisionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	HICON hIcon = AfxGetApp()->LoadIcon(IDI_REVISION);
	SetIcon(hIcon , FALSE);

	//! 테이블이 없으면 생성합니다.
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string rProjectMDBFilePath = docData.GetProjectMDBFilePath();

	TABLES::CELOADTable table;
	table.CreateTable(rProjectMDBFilePath , _T("T_PRODUCT_REVISION"));
	//!

	CreateRevisionListCtrl();
	DisplayRevisionData();
	
	//!BUTTON SETTING
	const COLORREF TextHotColor = docData.GetColorCode(_T("TEXT_HOT_COLOR"));
	m_wndOKButton.SetTextHotColor(TextHotColor);
	m_wndCancelButton.SetTextHotColor(TextHotColor);

	m_wndOKButton.SetIcon(IDI_OK_18X18);
	m_wndCancelButton.SetIcon(IDI_CANCEL_18X18);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	

	@author BHK	

	@date 2009-04-28 오전 10:51:03	

	@param	

	@return		
*/
int CSaveProductToRevisionDlg::CreateRevisionListCtrl(void)
{
	m_wndRevisionListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES);
	m_wndRevisionListCtrl.InsertColumn(0 , _T("PRODUCT NAME") , LVCFMT_LEFT , 150);
	m_wndRevisionListCtrl.InsertColumn(1 , _T("REVISION NO") , LVCFMT_LEFT , 100);
	m_wndRevisionListCtrl.InsertColumn(2 , _T("DATE") , LVCFMT_LEFT , 200);
	m_wndRevisionListCtrl.InsertColumn(3 , _T("DESC") , LVCFMT_LEFT , 400);

	return ERROR_SUCCESS;
}

/**
	@brief	C_PRODUCT_NAME에 해당하는 데이터를 REVISION 역순으로 표시한다.

	@author BHK	

	@date 2009-04-28 오전 10:57:04	

	@param	

	@return		
*/
int CSaveProductToRevisionDlg::DisplayRevisionData(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string rMDBFilePath = docData.GetProjectMDBFilePath();

	CADODB adoDB;
	const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rConnectionString))
	{
		CString sSql = _T("SELECT * FROM T_PRODUCT_REVISION WHERE C_PRODUCT_NAME = '") + m_rProductName + _T("' ORDER BY C_REVISION_NO DESC");
		adoDB.OpenQuery(sSql.operator LPCTSTR());
		LONG lRecordCount = 0L;
		adoDB.GetRecordCount(&lRecordCount);

		for(int i = 0;i < lRecordCount;++i)
		{
			const int nItem = m_wndRevisionListCtrl.InsertItem(i , m_rProductName);
			if(-1 != nItem)
			{
				STRING_T strValue;
				adoDB.GetFieldValue(i , _T("C_REVISION_NO") , &strValue);
				m_nLastRevNo = (int(m_nLastRevNo) < atoi(strValue.c_str())) ? atoi(strValue.c_str()) : m_nLastRevNo;
				m_wndRevisionListCtrl.SetItemText(nItem , 1 , strValue.c_str());
				adoDB.GetFieldValue(i , _T("C_DATE") , &strValue);
				m_wndRevisionListCtrl.SetItemText(nItem , 2 , strValue.c_str());
				adoDB.GetFieldValue(i , _T("C_DESC") , &strValue);
				m_wndRevisionListCtrl.SetItemText(nItem , 3 , strValue.c_str());
			}
		}
	}
	else
	{
		AfxMessageBox(_T("Fail to connect database.h"));
	}

	return 0;
}

/**
	@brief	

	@author BHK	

	@date 2009-04-28 오전 11:10:50	

	@param	

	@return		
*/
void CSaveProductToRevisionDlg::OnBnClickedOk()
{
	char* pData = NULL;

	CELoadDocData& docData = CELoadDocData::GetInstance();
	
	InsertLoadItemInCableSizingResultDoc();

	const string rMDBFilePath = docData.GetProjectMDBFilePath();

	CADODB adoDB;
	const STRING_T rConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + rMDBFilePath + DB_PASSWORD;
	if(TRUE == adoDB.DBConnect(rConnectionString))
	{
		CString strDesc;
		GetDlgItemText(IDC_EDIT_DESC , strDesc);

		CString strZipFilePath;
		map<CString , CString > ZipFilePathMap;
		if((_T("LOAD SUMMARY-BUS") == m_rProductName) || (_T("LOAD SUMMARY-SUBBUS") == m_rProductName))
		{
			strZipFilePath = string(docData.GetProjectFolderPath() + _T("Load Summary Result\\BUS_LoadSummary.zip")).c_str();
			ZipFilePathMap.insert(make_pair(_T("Load Summary-BUS") , strZipFilePath));
			strZipFilePath = string(docData.GetProjectFolderPath() + _T("Load Summary Result\\SubBUS_LoadSummary.zip")).c_str();
			ZipFilePathMap.insert(make_pair(_T("Load Summary-SubBUS") , strZipFilePath));
		}
		else if(_T("TRANSFORMER SIZING") == m_rProductName)
		{
			CTransformerSizingResultDoc::SaveTransformerSizingResultData();

			CZipArchive zip;
			CString strProjectPath = docData.GetProjectFolderPath().c_str();
			strZipFilePath = strProjectPath + _T("Transformer Sizing\\Transformer Sizing.zip");
			{
				if(!strZipFilePath.IsEmpty())
				{
					zip.Open(strZipFilePath , CZipArchive::create );
					zip.AddNewFile(strProjectPath + _T("Transformer Sizing\\Transformer Sizing.txt") , -1 , false);
					zip.Close();
				}
			}
			ZipFilePathMap.insert(make_pair(m_rProductName , strZipFilePath));
		}
		else if(_T("AUTO POWER CABLE") == m_rProductName)
		{
			CZipArchive zip;
			CString strProjectPath = docData.GetProjectFolderPath().c_str();
			strZipFilePath = strProjectPath + _T("Cable Sizing Result\\Cable Sizing Result.zip");
			{
				if(!strZipFilePath.IsEmpty())
				{
					zip.Open(strZipFilePath , CZipArchive::create );
					CString strDir = strProjectPath + CString(_T("Cable Sizing Result\\*.BUS"));
					CFileFind file;

					vector<string> rInterestingFileEntry;
					///! 확장자가 *.BUS 파일을 찾는다.
					BOOL bFound = file.FindFile(strDir);
					if(bFound)
					{
						while(bFound)
						{
							bFound = file.FindNextFile();
							if(file.IsDots()) continue;

							const CString rFileName = file.GetFileTitle().MakeUpper();
							zip.AddNewFile(strProjectPath + CString(_T("Cable Sizing Result\\")) + rFileName + _T(".BUS") , -1 , false);
						}
					}
					zip.Close();
				}
			}
			ZipFilePathMap.insert(make_pair(m_rProductName , strZipFilePath));
		}
		else if(_T("AUTO CONTROL CABLE") == m_rProductName)
		{
			CZipArchive zip;
			CString strProjectPath = docData.GetProjectFolderPath().c_str();
			strZipFilePath = strProjectPath + _T("Control Cable Result\\Control Cable Result.zip");
			{
				if(!strZipFilePath.IsEmpty())
				{
					zip.Open(strZipFilePath , CZipArchive::create );
					CString strDir = strProjectPath + CString(_T("Control Cable Result\\*.BUS"));
					CFileFind file;

					vector<string> rInterestingFileEntry;
					///! 확장자가 *.BUS 파일을 찾는다.
					BOOL bFound = file.FindFile(strDir);
					if(bFound)
					{
						while(bFound)
						{
							bFound = file.FindNextFile();
							if(file.IsDots()) continue;

							const CString rFileName = file.GetFileTitle().MakeUpper();
							zip.AddNewFile(strProjectPath + CString(_T("Control Cable Result\\")) + rFileName + _T(".BUS") , -1 , false);
						}
					}
					zip.Close();
				}
			}
			ZipFilePathMap.insert(make_pair(m_rProductName , strZipFilePath));
		}
		else if(_T("CABLE SIZING TABLE") == m_rProductName)
		{
			CZipArchive zip;
			CString strProjectPath = docData.GetProjectFolderPath().c_str();
			strZipFilePath = strProjectPath + _T("Cable Sizing Table\\Cable Sizing Table.zip");
			{
				if(!strZipFilePath.IsEmpty())
				{
					zip.Open(strZipFilePath , CZipArchive::create );
					
					CString strDir = strProjectPath + CString(_T("Cable Sizing Table\\*.TXT"));
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
							zip.AddNewFile(strProjectPath + CString(_T("Cable Sizing Table\\")) + rFileName + _T(".TXT") , -1 , false);
						}
					}

					zip.Close();
				}
			}
			ZipFilePathMap.insert(make_pair(m_rProductName , strZipFilePath));
		}
		else if((_T("CABLE SCHEDULE") == m_rProductName) || (_T("DRUM SCHEDULE") == m_rProductName))
		{
			CZipArchive zip;
			CString strProjectPath = docData.GetProjectFolderPath().c_str() , rKeyString;
			if(_T("Cable Schedule") == m_rProductName)
				rKeyString = _T("Cable Schedule Result");
			else	rKeyString = _T("Drum Schedule Result");
			strZipFilePath = strProjectPath + rKeyString + _T("\\") + rKeyString + _T(".zip");
			{
				if(!strZipFilePath.IsEmpty())
				{
					zip.Open(strZipFilePath , CZipArchive::create );
					
					CString strDir = strProjectPath + rKeyString + CString(_T("\\*.TXT"));
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
							zip.AddNewFile(strProjectPath + rKeyString + CString(_T("\\")) + rFileName + _T(".TXT") , -1 , false);
						}
					}

					zip.Close();
				}
			}
			ZipFilePathMap.insert(make_pair(m_rProductName , strZipFilePath));
		}

		if(!ZipFilePathMap.empty())
		{
			++m_nLastRevNo;
			for(map<CString , CString>::iterator itr = ZipFilePathMap.begin();itr != ZipFilePathMap.end();++itr)
			{
				CString strFormat = _T("INSERT INTO T_PRODUCT_REVISION(C_PRODUCT_NAME,C_REVISION_NO,C_DATE,C_DESC) VALUES('%s',%d,'%s','%s')");
				CString strSQL;

				CTime CurTime = CTime::GetCurrentTime();
				CString strTimeData = CurTime.Format(_T("%Y년 %m월 %d일 - %H시 %M분"));

				strSQL.Format(strFormat , itr->first , m_nLastRevNo , strTimeData , strDesc);
				adoDB.ExecuteQuery(strSQL.operator LPCTSTR());

				strSQL = _T("SELECT * FROM T_PRODUCT_REVISION WHERE C_PRODUCT_NAME = '") + itr->first + _T("' ORDER BY C_REVISION_NO DESC");
				adoDB.OpenQuery(strSQL.operator LPCTSTR());
				LONG lRecordCount = 0L;
				adoDB.GetRecordCount(&lRecordCount);

				CFile f(itr->second , CFile::modeRead);
				ULONGLONG fSize = f.GetLength();
				if(NULL == pData) pData = (char*)calloc(1 , sizeof(char)*fSize);
				if(pData)
				{
					f.Read(pData,fSize);
					const int nFieldCount = adoDB.GetFieldCount();
					for(int i = 0;i < nFieldCount;++i)
					{
						if(_T("C_DATA") == adoDB.GetFieldName(i))
						{
							adoDB.AppendChunk(0 , i , pData , fSize);
							break;
						}
					}
					free((void*)pData);
					pData = NULL;
				}
			}
		}
	}
	else
	{
		AfxMessageBox(_T("Fail to connect database"));
	}
	
	OnOK();
}

/**
	@brief	CCableSizingResultDoc::m_InterestingLoadItemList 에 LoadItem을 넣어준다

	@author KHS	

	@date 2009-08-19 오후 1:15:56	

	@param	

	@return		
**/
int CSaveProductToRevisionDlg::InsertLoadItemInCableSizingResultDoc(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	
	list<CELoadItem*> LoadItemList;
	docData.GetELoadItemListOf(LoadItemList , CLoadItem::TypeString());
	{
		for(list<CELoadItem*>::iterator itr = LoadItemList.begin(); itr != LoadItemList.end();++itr)
		{
			CCableSizingResultDoc::m_InterestingLoadItemList.push_back(*itr);
		}
	}

	list<CELoadItem*> TrItemList;
	docData.GetELoadItemListOf(TrItemList, CTransformerItem::TypeString());
	{
		for(list<CELoadItem*>::iterator itr = TrItemList.begin(); itr != TrItemList.end();++itr)
		{
			CCableSizingResultDoc::m_InterestingLoadItemList.push_back(*itr);
		}
	}

	list<CELoadItem*> CapacitorItemList;
	docData.GetELoadItemListOf(CapacitorItemList, CCapacitorItem::TypeString());
	{
		for(list<CELoadItem*>::iterator itr = CapacitorItemList.begin(); itr != CapacitorItemList.end();++itr)
		{
			CCableSizingResultDoc::m_InterestingLoadItemList.push_back(*itr);
		}
	}

	list<CELoadItem*> UpsDcItemList;
	docData.GetELoadItemListOf(UpsDcItemList, CUPSDCItem::TypeString());
	{
		for(list<CELoadItem*>::iterator itr = UpsDcItemList.begin(); itr != UpsDcItemList.end();++itr)
		{
			CCableSizingResultDoc::m_InterestingLoadItemList.push_back(*itr);
		}
	}

	if(!CCableSizingResultDoc::m_InterestingLoadItemList.empty())	return ERROR_SUCCESS;
	else	return ERROR_BAD_ENVIRONMENT;
}
