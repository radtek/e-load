
// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// ELOAD.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"
#include "LoadItem.h"
#include "./Tables/CableCreationSettingTable.h"
#include "ELoadDocData.h"
#include "iphlpapi.h"

bool SortByName(ITEMS::CELoadItem* lhs , ITEMS::CELoadItem* rhs)
{
	assert(lhs && rhs && "lhs or rhs is NULL");

	if(lhs && rhs) return (lhs->GetName() < rhs->GetName());

	return false;
}

/**
	@brief	

	@author	BHK
*/
BOOL FileExist(CString rFilePath)
{
	OFSTRUCT of;
	if(HFILE_ERROR != OpenFile(rFilePath.operator LPCSTR() , &of , OF_EXIST)) return TRUE;

	return FALSE;
}

/**
        @brief  Get Module Path
        
	@author	KHS

	@date	????.??.??

	@return
*/
CString GetExecPath(void)
{
	char szFileName[MAX_PATH + 1] = {'\0', };
	GetModuleFileName(NULL , szFileName , MAX_PATH);
	CString rFilePath = szFileName;
	int at = rFilePath.ReverseFind('\\');
	if(-1 != at) rFilePath.Left(at);
	CString rModulePath = rFilePath.Left(at);
	if(_T("\\") != rModulePath.Right(1)) rModulePath += _T("\\");

	return rModulePath;

}

COLORREF DarkenColor(COLORREF col,double factor)
{
	if(factor>0.0&&factor<=1.0)
	{
		BYTE red,green,blue,lightred,lightgreen,lightblue;
		red = GetRValue(col);
		green = GetGValue(col);
		blue = GetBValue(col);
		lightred = (BYTE)(red-(factor*red));
		lightgreen = (BYTE)(green-(factor*green));
		lightblue = (BYTE)(blue-(factor*blue));
		col = RGB(lightred,lightgreen,lightblue);
	}
	return(col);
}

/**
	@brief	delete files in directory and directory

	@author	BHK

	@date	2009.11.11

	@param	strDirectory = directory path

	@return
*/
void CreateOrClearFolder(const CString& rDirectory)
{
	if(FALSE == ::PathIsDirectory(rDirectory))
	{
		CreateDirectory(rDirectory , NULL);
	}
	else
	{
		list<CString> FilePathEntry;

		CString strDir = rDirectory + _T("\\*.*");
		CFileFind file;
		BOOL bFound = file.FindFile(strDir);
		while(bFound)
		{
			bFound = file.FindNextFile();
			if(file.IsDots()) continue;

			FilePathEntry.push_back(file.GetFilePath());
		}

		for(list<CString>::iterator itr = FilePathEntry.begin();itr != FilePathEntry.end();++itr)
		{
			::DeleteFile(*itr);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CMFCTabCtrlEx, CMFCTabCtrl)

BOOL CMFCTabCtrlEx::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	CWnd* pWnd = GetActiveWnd();
	if(pWnd && (pWnd->GetDlgCtrlID() == wParam))
	{
		return GetParent()->SendMessage(WM_NOTIFY , wParam ,lParam);
	}

	return CMFCTabCtrl::OnNotify(wParam,lParam , pResult);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CMFCTextColorListCtrl,CMFCListCtrl)

BEGIN_MESSAGE_MAP(CMFCTextColorListCtrl, CMFCListCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, &CMFCTextColorListCtrl::OnLvnColumnclick)
END_MESSAGE_MAP()

// THE IMPORTANT UNDOCUMENTED FUNCTION!
/*
void CMFCTextColorListCtrl::InitHeader()
{
	// Initialize header control:
	///m_hdrCtrl.SubclassDlgItem(0, this);
	
	GetHeaderCtrl().SetFont(&m_NewHeaderFont);
}
*/

int CMFCTextColorListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMFCListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	/*
	HDLAYOUT  hdl;
	WINDOWPOS wpos;
	RECT rc;
	m_hdrCtrl.GetParent()->GetClientRect(&rc);
	hdl.prc = &rc;
	hdl.pwpos = &wpos;
	if(m_hdrCtrl.Layout(&hdl))
	{
		m_hdrCtrl.SetWindowPos(
			CWnd::FromHandle(wpos.hwndInsertAfter),
			wpos.x,
			wpos.y,
			wpos.cx,
			wpos.cy * 2,
			SWP_SHOWWINDOW);
	}
	*/

	return 0;
}

/**
	@brief	original value와 changed value가 서로 다를 경우에 붉은색을 리턴한다.그렇지 않을경우에는 검은색을 리턴한다.

	@author BHK	

	@date 2009-04-07 오전 11:16:22	

	@param	

	@return		
*/
COLORREF CMFCTextColorListCtrl::OnGetCellTextColor(int nRow,int nColumn)
{
	map<DWORD_PTR,vector<COLORREF>>::iterator where = m_ItemColorMap.find(GetItemData(nRow));
	if(where != m_ItemColorMap.end())
	{
		return (nColumn < int(where->second.size())) ? where->second[nColumn] : CMFCListCtrl::OnGetCellTextColor(nRow , nColumn);
	}

	return CMFCListCtrl::OnGetCellTextColor(nRow , nColumn);
}

/**
	@breif	this is sorting function.

	@author	BHK
*/
int CMFCTextColorListCtrl::OnCompareItems (LPARAM lParam1, LPARAM lParam2, int iColumn)
{
	ITEMS::CELoadItem* pItem1 = (ITEMS::CELoadItem*)lParam1;
	ITEMS::CELoadItem* pItem2 = (ITEMS::CELoadItem*)lParam2;

	if((0 == iColumn) && pItem1 && pItem2)
	{
		if(pItem1->IsKindOf(ITEMS::CLoadItem::TypeString()) && pItem2->IsKindOf(ITEMS::CLoadItem::TypeString()))
		{
			CString rString1 = pItem1->GetName().c_str();
			rString1.MakeUpper();
			CString rString2 = pItem2->GetName().c_str();
			rString2.MakeUpper();

			return strcmp(rString1 , rString2);
		}
	}
	else if((1 == iColumn) && pItem1 && pItem2)
	{
		if(pItem1->IsKindOf(ITEMS::CLoadItem::TypeString()) && pItem2->IsKindOf(ITEMS::CLoadItem::TypeString()))
		{
			double nRatingCapacity1 =0.f, nRatingCapacity2 =0.f;
			ITEMS::CLoadItem* pLoadItem1 = (ITEMS::CLoadItem*)pItem1;
			ITEMS::CLoadItem* pLoadItem2 = (ITEMS::CLoadItem*)pItem2;
			if(pLoadItem1 && pLoadItem2)
			{
				pLoadItem1->CalcRatingCapacity(nRatingCapacity1);
				pLoadItem2->CalcRatingCapacity(nRatingCapacity2);

				if(nRatingCapacity1 > nRatingCapacity2) return -1;
				else if(nRatingCapacity1 < nRatingCapacity2)return 1;
				else return 0;
			}
		}
	}
	else if((2 == iColumn) && pItem1 && pItem2)
	{
		if(pItem1->IsKindOf(ITEMS::CLoadItem::TypeString()) && pItem2->IsKindOf(ITEMS::CLoadItem::TypeString()))
		{
			CString rString1 = pItem1->prop()->GetValue(_T("Control Cable") , _T("Schematic Ty")).c_str();
			rString1.MakeUpper();
			CString rString2 = pItem2->prop()->GetValue(_T("Control Cable") , _T("Schematic Ty")).c_str();
			rString2.MakeUpper();

			return strcmp(rString1 , rString2);
		}
	}
	else if((3 == iColumn) && pItem1 && pItem2)
	{
		if(pItem1->IsKindOf(ITEMS::CLoadItem::TypeString()) && pItem2->IsKindOf(ITEMS::CLoadItem::TypeString()))
		{
			CString rString1 = pItem1->prop()->GetValue(_T("Panel") , _T("Bus ID")).c_str();
			rString1.MakeUpper();
			CString rString2 = pItem2->prop()->GetValue(_T("Panel") , _T("Bus ID")).c_str();
			rString2.MakeUpper();

			return strcmp(rString1 , rString2);
		}
	}
        
	return 0;

}

/**
	@brief	

	@author BHK	

	@date 2009-05-12 오후 2:59:24	

	@param	

	@return		
*/
void CMFCTextColorListCtrl::OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult)
{
        NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	ASSERT (pNMListView != NULL);

	int iColumn = pNMListView->iSubItem;
	BOOL bShiftIsPressed = (::GetAsyncKeyState (VK_SHIFT) & 0x8000);
	int nColumnState = GetHeaderCtrl ().GetColumnState (iColumn);
	BOOL bAscending = TRUE;
	
	if (nColumnState != 0)
	{
		bAscending = nColumnState <= 0;
	}

	Sort (iColumn, bAscending, bShiftIsPressed && IsMultipleSort ());
	*pResult = 0;

}

/**
	@brief	테이블에서 VoltTypeString에 부합하는 MinSize를 리턴한다.

	@author BHK	

	@date 2009-08-27 오전 12:26:53	

	@param	

	@return		
**/
double GetMinSizeFromTable(const string& rVoltTypeString)
{
	double nMinSize = -1.f;
	CELoadDocData& docData = CELoadDocData::GetInstance();

	TABLES::CPowerCableCreationSettingTable* pPowerCableCreationSettingTable = 
		static_cast<TABLES::CPowerCableCreationSettingTable*>(docData.GetTableOf(TABLES::CPowerCableCreationSettingTable::GetTableName()));
	if(pPowerCableCreationSettingTable)
	{
		if(1 == pPowerCableCreationSettingTable->GetRecordCount())
		{
			string rValue;
			if(string::npos != rVoltTypeString.find(_T("HV")))
			{	
				pPowerCableCreationSettingTable->GetValue(0 , _T("C_FAULT_CURRENT_AT_HV_CHECK") , rValue);
				CString rMinHvCheck = rValue.c_str();
				if(_T("YES") == rMinHvCheck.MakeUpper())
				{
					pPowerCableCreationSettingTable->GetValue(0 , _T("C_FAULT_CURRENT_AT_HV") , rValue);
					CString rMinHv = rValue.c_str();
					nMinSize = atof(rMinHv);
				}
			}
			else
			{
				pPowerCableCreationSettingTable->GetValue(0 , _T("C_MIN_LV_CHECK") , rValue);
				CString rMinLvCheck = rValue.c_str();
				if(_T("YES") == rMinLvCheck.MakeUpper())
				{
					pPowerCableCreationSettingTable->GetValue(0 , _T("C_MIN_LV") , rValue);
					CString rMinLv = rValue.c_str();
					nMinSize = atof(rMinLv);
				}
			}
		}
	}

	return nMinSize;
}
