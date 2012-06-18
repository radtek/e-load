// BCGPPropEx.cpp : implementation file
//

#include "stdafx.h"
///#include "bcgcbpro.h"
///#include "BCGPPropList.h"
#include "MFCPropEx.h"
#include "ELoadDocData.h"
#include "PropertiesWnd.h"
#include "Tables/CableCreationSettingTable.h"
#include <gui/FilterEdit/BoostBaseFilterEdit.h>
#include "ShowCableDlg.h"
#include "ShowRemarkDlg.h"
#include "commands/CommandCircleBusConnectionCheck.h"

#include <set>

#define	MFCPROPLIST_ID_INPLACE	0x01

///using namespace MFCPROPERTY;

#define	TEXT_MARGIN	3
#define	SPINCTRL_WIDTH	13
#define	RIGHT_MARGIN	16
/////////////////////////////////////////////////////////////////////////////
// CMFCPropEx

IMPLEMENT_DYNAMIC(CMFCPropEx, CMFCPropertyGridProperty)

#define PROP_HAS_LIST	0x0001
#define PROP_HAS_BUTTON	0x0002
#define PROP_HAS_SPIN	0x0004


CMFCPropEx::CMFCPropEx(const CString& strName, const _variant_t& varValue, const CString& rLockType ,
				   LPCTSTR lpszDescr, DWORD_PTR dwData, LPCTSTR lpszEditMask,
				   LPCTSTR lpszEditTemplate, LPCTSTR lpszValidChars) :
        CMFCPropertyGridProperty(strName,varValue, lpszDescr,dwData,lpszEditMask,lpszEditTemplate, lpszValidChars) , m_rUnit("NONE")
{
	m_bGroup = FALSE;
	m_bIsValueList = FALSE;

	Init ();
	SetFlags ();

	if (m_varValue.vt == VT_BOOL)
	{
		m_bAllowEdit = FALSE;
	}

	m_rAccess = READWRITABLE_STR;
	m_bLock = FALSE;
	m_rLockType = rLockType;
	AllowEdit((TRUE == m_bLock) ? FALSE : TRUE);
}
//******************************************************************************************
CMFCPropEx::CMFCPropEx(const CString& strGroupName, DWORD_PTR dwData , const CString& rLockType ,
					 BOOL bIsValueList) :
        CMFCPropertyGridProperty(strGroupName, dwData, bIsValueList) , m_rUnit("NONE")
{
	m_bGroup = TRUE;
	Init ();
	SetFlags ();

	m_rAccess = READWRITABLE_STR;
	m_bLock = FALSE;
	m_rLockType = rLockType;
	AllowEdit((TRUE == m_bLock) ? FALSE : TRUE);
}
//*******************************************************************************************
CMFCPropEx::~CMFCPropEx()
{
        while (!m_lstSubItems.IsEmpty ())
	{
		delete m_lstSubItems.RemoveHead ();
	}

	OnDestroyWindow ();
}

void PrintError(DWORD dwErrorNo)
{
	LPVOID lpMsgBuf;
	if (!FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwErrorNo,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),// Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL ))
	{
		// Handle the error.
		return;
	}

	// Process any inserts in lpMsgBuf.
	// ...
	// Display the string.

	// 이곳을 바꾸면 오류를 적당히 처리 가능하다
	::MessageBox(NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
	// 이런 식으로...
	//printf("%s ", (LPCTSTR)lpMsgBuf);


	// Free the buffer.
	LocalFree( lpMsgBuf ); 
}


//******************************************************************************************
/**
	@brief	VALUE의 값을 표기한다.

	@author	KHS
*/
void CMFCPropEx::OnDrawValue (CDC* pDC, CRect rect)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);
	ASSERT_VALID (m_pWndList);

	if ((IsGroup () && !m_bIsValueList) || !HasValueField ())
	{
		return;
	}

	//! UNIT를 표기한다.
	if(!m_rUnit.IsEmpty() && ("NONE" != m_rUnit))
	{
		COLORREF prevColor = pDC->GetTextColor();
		pDC->SetTextColor(RGB(177,177,177));

		CRect rectUnit(rect);
		CItemPropCategory::ItemPropTag* pItemPropTag = (CItemPropCategory::ItemPropTag*)GetData();
		if(pItemPropTag)
		{
			const int nUnitCount = pItemPropTag->GetUnitCount();
			(nUnitCount > 1) ? rectUnit.DeflateRect(0,0,RIGHT_MARGIN,0) : rectUnit.DeflateRect(0,0,3,0);
			
			pDC->DrawText ("[" + m_rUnit + "]", rectUnit, DT_RIGHT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
			pDC->SetTextColor(prevColor);

			if(nUnitCount > 1)
			{
				//! MULTI UNIT일 경우에 UNIT를 선택할수 있도록 SPINCTRL을 생성한다.
				if(!m_wndSpinButtonCtrl.GetSafeHwnd())
				{
					CRect rectSpin(rect);
					rectSpin.left = rectSpin.right - SPINCTRL_WIDTH;

					m_wndSpinButtonCtrl.Create(WS_CHILD | WS_VISIBLE , rectSpin, m_pWndList , 0x1001);
					m_wndSpinButtonCtrl.SetRange(0,nUnitCount - 1);
					m_wndSpinButtonCtrl.SetPos( pItemPropTag->GetUnitPos(m_rUnit.operator LPCSTR()) );
					m_wndSpinButtonCtrl.SetBuddy(this);
				}
				else
				{
					CRect rectSpin(rect);
					rectSpin.left = rectSpin.right - SPINCTRL_WIDTH;

					m_wndSpinButtonCtrl.SetWindowPos(NULL , rectSpin.left , rectSpin.top , rectSpin.Width() , rectSpin.Height() , SWP_NOACTIVATE | SWP_NOZORDER);
				}
			}
		}
	}

	CFont* pOldFont = NULL;
	//! 기존의 텍스트 칼라를 저장해둔다.
        const COLORREF OldColor = pDC->GetTextColor();
	if (IsValueChanged())
	{
		if(IS_NUMBER_TYPE(m_rDataType))
		{
			//! 숫자 형식일 경우에는 값이 변경되었는지 다시 한번 확인한다.
			const CString rValue         = CString(GetValue());
			const CString rOriginalValue = CString(GetOriginalValue());
			if(atof(rValue) != atof(rOriginalValue))
			{
				pOldFont = pDC->SelectObject (&m_pWndList->GetBoldFont());
				pDC->SetTextColor(RGB(255, 0, 0));      //! 추가 코드.
			}
		}
		else
		{
			pOldFont = pDC->SelectObject (&m_pWndList->GetBoldFont());
			pDC->SetTextColor(RGB(255, 0, 0));      //! 추가 코드.
		}
	}

	CString strVal = FormatProperty ();
	rect.DeflateRect (TEXT_MARGIN, 0);
        
	pDC->DrawText (strVal, rect, 
		DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);

        //! 원래 상태로 복귀.
        //if (IsModified ())
        {
                pDC->SetTextColor(OldColor);
        }

///	m_bValueIsTrancated = pDC->GetTextExtent (strVal).cx > rect.Width ();

	if (pOldFont != NULL)
	{
		pDC->SelectObject (pOldFont);
	}
}

/**
	@brief	property의 in place edit를 생성한다. Data Type에 정규식을 사용하면 CBoostBaseFilterEdit를 생성하고,
	그렇지 않을 경우에는 기본 Edit를 생성한다.

	@author	BHK
*/
CWnd* CMFCPropEx::CreateInPlaceEdit (CRect rectEdit, BOOL& bDefaultFormat)
{
	if(FALSE == IsAllowEdit()) return NULL;

	CEdit* pWndEdit = NULL;
	if((m_rDataType.GetLength() > 6) && ("REGEXP" == m_rDataType.Left(6)))
	{
		CString rRegExpKey = m_rDataType.Right(m_rDataType.GetLength() - 7);
		rRegExpKey = rRegExpKey.Left(rRegExpKey.GetLength() - 1);

		pWndEdit = new CBoostBaseFilterEdit;

		DWORD dwStyle = WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL;

		if (!m_bEnabled || !m_bAllowEdit)
		{
			dwStyle |= ES_READONLY;
		}

		if(pWndEdit->Create (dwStyle, rectEdit, m_pWndList, MFCPROPLIST_ID_INPLACE))
		{
			CBoostBaseFilterEdit* pWndFilterEdit = (CBoostBaseFilterEdit*)pWndEdit;
			if(pWndFilterEdit)
			{
				CELoadDocData& docData = CELoadDocData::GetInstance();
				const string rRegExp = docData.GetRegExp(rRegExpKey.operator LPCSTR());
				(!rRegExp.empty()) ? pWndFilterEdit->SetRegEx(rRegExp.c_str()) : pWndFilterEdit->SetRegEx(TEXT_REGEXP);

				CString rValue = GetValue();
				pWndFilterEdit->SetWindowText(rValue);

				pWndFilterEdit->CreateToolTip(m_pWndList , "데이터 형식이 일치하지 않습니다.");
				pWndFilterEdit->SetBackgroundColourFocus (RGB (150, 200, 255));
				pWndFilterEdit->SetForegroundColourError (RGB (255, 255, 255));
				pWndFilterEdit->SetBackgroundColourError (RGB (255, 0, 0));
			}
		}
	}
	else
	{
		pWndEdit = (CEdit*)CMFCPropertyGridProperty::CreateInPlaceEdit(rectEdit , bDefaultFormat);
	}

	bDefaultFormat = TRUE;
	
	return pWndEdit;
}

/**
	@brief	MULTI UNITS에서 사용자가 선택한 UNIT을 현재 UNIT으로 설정한다.

	@author	BHK
*/
int CMFCPropEx::SetCurUnitPos(const int& nPos)
{
	CItemPropCategory::ItemPropTag* pItemPropTagPrototype = (CItemPropCategory::ItemPropTag*)GetData();
	if(pItemPropTagPrototype)
	{
		m_rUnit = pItemPropTagPrototype->GetUnit( nPos ).c_str();

		CWnd *pWnd = m_pWndList->GetParent();
		CWnd *pActiveWnd = pWnd;
		while(pWnd)
		{
			if(pWnd->IsKindOf(RUNTIME_CLASS(CPropertiesWnd))) break;
			pActiveWnd = pWnd;
			pWnd = pWnd->GetParent();
		}

		if(pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CPropertiesWnd)))
		{
			CPropertiesWnd* pPropertiesWnd = (CPropertiesWnd*)pWnd;
			pPropertiesWnd->GetSelectionSet().SetUnitStr( pItemPropTagPrototype->pParent->name() , pItemPropTagPrototype->rKey , m_rUnit.operator LPCSTR() );
			
			//! property의 값(단위)이 변경되었다고 통보한다.
			if(pActiveWnd) pPropertiesWnd->SendMessage(AFX_WM_PROPERTY_CHANGED , WPARAM(pActiveWnd->GetDlgCtrlID()) , LPARAM(this));
		}

		Redraw();
	}

	return 0;
}

/**
	@brief	PROP NAME을 그려주는 부분(LOCK ITEM일 경우에는 왼쪽에 LOCK ICON을 그려준다.)

	@author	BHK
*/
void CMFCPropEx::OnDrawName (CDC* pDC, CRect rect)
{	
	if((GROUP_LOCK_ITEM_STR == m_rLockType) || (LOCK_ITEM_STR == m_rLockType))
	{
		CELoadDocData& docData = CELoadDocData::GetInstance();
		const COLORREF lockTextColor = docData.GetColorCode(_T("LOCK_ITEM_TEXT_COLOR"));
		const COLORREF lockBackColor = docData.GetColorCode(_T("LOCK_ITEM_BACK_COLOR"));
		const COLORREF textColor = docData.GetColorCode(_T("TEXT_COLOR"));
		COLORREF oldColor = pDC->SetTextColor (lockTextColor);
		if(IsSelected())
		{
			pDC->SetBkColor(textColor);
			CBrush brush(lockBackColor);
			pDC->FillRect(&rect,&brush);
		}

		pDC->DrawText(m_strName, CRect(rect.left + 16 , rect.top , rect.right , rect.bottom) , DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		pDC->SetTextColor (oldColor);

		HICON hIcon = AfxGetApp()->LoadIcon((TRUE == m_bLock) ? IDI_LOCK : IDI_UNLOCK);
		if(hIcon)
		{
			int res = ::DrawIconEx(pDC->GetSafeHdc(), rect.left + 2 , rect.top + 2 , hIcon , 10 , 10 , 0 , NULL , DI_NORMAL);
			///PrintError(GetLastError());
		}
		else	AfxMessageBox("hIcon is NULL");
	}
	else
	{
		CMFCPropertyGridProperty::OnDrawName(pDC , rect);
	}
}

/**
	@brief	PROP NAME부분을 클릭했을 때

	@author	BHK
*/
void CMFCPropEx::OnClickName(  CPoint C )
{
	if(LOCK_ITEM_STR == m_rLockType)
	{
		CRect rect = GetRect();
		rect.right = rect.left + 16;
		if(rect.PtInRect(C))
		{
			m_bLock = (TRUE == m_bLock) ? FALSE : TRUE;
			Enable(!m_bLock);

			CWnd* pParent = m_pWndList->GetParent();
			if(!pParent->IsKindOf(RUNTIME_CLASS(CPropertiesWnd))) pParent = pParent->GetParent();
			if(pParent->IsKindOf(RUNTIME_CLASS(CPropertiesWnd)))
			{
				CItemPropCategory::ItemPropTag* pItemPropTag = (CItemPropCategory::ItemPropTag*)GetData();
				if(pItemPropTag)
				{
					CPropertiesWnd* pPropertiesWnd = (CPropertiesWnd*)pParent;
					pPropertiesWnd->SetItemLockProp(pItemPropTag->pParent->name() , pItemPropTag->rKey , m_bLock);
				}
			}

			Redraw();
		}
	}
}

/**
	@brief	

	@author	BHK
*/
bool CMFCPropEx::IsLockItemType( const CString& rLockType )
{
	return ((GROUP_LOCK_ITEM_STR == rLockType) || (LOCK_ITEM_STR == rLockType));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///IMPLEMENT_DYNAMIC(CMFCLockItemProp, CMFCPropEx)

/**
	@brief	

	@author	BHK
*/


////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CMFCCheckBoxProp, CMFCPropEx)

CMFCCheckBoxProp::CMFCCheckBoxProp(const CString& strName, BOOL bCheck, const CString& rLockType , LPCTSTR lpszDescr, DWORD dwData)
	:	CMFCPropEx(strName, _variant_t (COleVariant("Yes")), rLockType , lpszDescr, dwData)
{
	m_rectCheck.SetRectEmpty ();
}
//************************************************************************************

bool CMFCCheckBoxProp::IsGroupLock(void) const
{
	return ((m_rLockType.GetLength() >= 10) && (_T("GROUP LOCK") == m_rLockType.Left(10)));
}

int CMFCCheckBoxProp::GetGroupLockItemCount(void)
{
	if(IsGroupLock())
	{
		CString rGroupLockCount = m_rLockType.Right(m_rLockType.GetLength() - 10);
		if(("(" == rGroupLockCount.Left(1)) && (")" == rGroupLockCount.Right(1)))
		{
			rGroupLockCount = rGroupLockCount.Mid(1 , rGroupLockCount.GetLength() - 2);
			return atoi(rGroupLockCount);
		}
	}

	return 0;
}

BOOL CMFCCheckBoxProp::OnEdit (LPPOINT lptClick)
{
        if(lptClick)
        {
                if(TRUE == IsKindOf(RUNTIME_CLASS(CMFCCheckBoxProp)))
                {
                        CPoint point(lptClick->x, lptClick->y);
                        m_varValue = (COleVariant("Yes") == m_varValue) ? COleVariant("No") : COleVariant("Yes");
                        m_pWndList->InvalidateRect (m_rectCheck);

                        m_pWndList->OnPropertyChanged (this);
                }
        }
        else
        {
                m_varValue = (COleVariant("Yes") == m_varValue) ? COleVariant("No") : COleVariant("Yes");
                m_pWndList->InvalidateRect (m_rectCheck);

                m_pWndList->OnPropertyChanged (this);
        }

	return FALSE;
}

void CMFCCheckBoxProp::OnDrawValue(CDC* pDC, CRect rect)
{
	m_rectCheck = rect;
	m_rectCheck.DeflateRect (1, 1);

	m_rectCheck.right = m_rectCheck.left + m_rectCheck.Height();

	rect.left = m_rectCheck.right + 1;
 
	COLORREF clr = pDC->GetTextColor();
	OnDrawCheckBox (pDC, m_rectCheck, (COleVariant("Yes") == m_varValue) ? true : false);///bool (m_varValue));

        CMFCPropEx::OnDrawValue (pDC, rect);
	pDC->SetTextColor(clr);
}
//************************************************************************************
BOOL CMFCCheckBoxProp::OnClickValue(UINT uiMsg, CPoint point)
{
	if (m_bEnabled && m_rectCheck.PtInRect (point)) 
	{
		///m_varValue = COleVariant(BOOL(m_varValue) ? TRUE : FALSE);
		m_pWndList->InvalidateRect (m_rectCheck);

		return TRUE;
	}

	return FALSE;
}

//************************************************************************************
BOOL CMFCCheckBoxProp::OnDblClick (CPoint point)
{
	/*if (m_bEnabled && m_rectCheck.PtInRect (point)) 
	{
		return TRUE;
	}
	m_varValue = (COleVariant("Yes") == m_varValue) ? COleVariant("No") : COleVariant("Yes");///!bool(m_varValue);
	m_pWndList->InvalidateRect (m_rectCheck);
	
	m_pWndList->OnPropertyChanged (this);*/
	
	return FALSE;
}
//************************************************************************************
void CMFCCheckBoxProp::OnDrawCheckBox (CDC * pDC, CRect rect, BOOL bChecked)
{
	COLORREF clrTextOld = pDC->GetTextColor ();

	CMFCVisualManager::GetInstance ()->OnDrawCheckBox (pDC, rect , FALSE, bChecked, m_bEnabled);
        
	pDC->SetTextColor (clrTextOld);
}
//************************************************************************************
BOOL CMFCCheckBoxProp::PushChar (UINT nChar)
{
	if (nChar == VK_SPACE)
	{
		OnDblClick (CPoint (-1, -1));
	}

	return FALSE;
}

//************************************************************************************
/**
	@brief	when value is changed

	@author	BHK
*/
int CMFCCheckBoxProp::OnValueChanged()
{
	if(IsGroupLock())
	{
		CString rValue = GetValue();
		rValue.MakeUpper();	//! 대문자로 변환

		CMFCPropertyGridProperty* pParent = GetParent();
		if(pParent && (_T("GROUP LOCK") == m_rLockType))
		{
			bool bBelowMe = false;
			for(int i = 0;i < pParent->GetSubItemsCount();++i)
			{
				CMFCPropertyGridProperty* pProp = pParent->GetSubItem(i);
				if(this == pProp)
				{
					bBelowMe = true;
					continue;
				}

				if(true == bBelowMe)
				{
					if(pProp->IsKindOf(RUNTIME_CLASS(CMFCPropEx)))
					{
						CMFCPropEx* pItemProp = (CMFCPropEx*)pProp;
						if(pItemProp->IsGroupLockItem()) 
							pItemProp->SetLock((_T("YES") == rValue) ? TRUE : FALSE);
						else	break;
					}else	break;
				}
			}
		}
	}

	return ERROR_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CMFCComboItemProp, CMFCPropEx)

////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CMFCBusVoltageComboProp, CMFCPropEx)

/**
	@brief	ComboBox가 생성될때 Bus Voltage들을 읽어와서 Option으로 추가한다.

	@author	BHK
*/
BOOL CMFCBusVoltageComboProp::OnEdit(LPPOINT lptClick)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string rProjectMDBFilePath = docData.GetProjectMDBFilePath();

	RemoveAllOptions();	//! 기존의 모든 OPTION들을 삭제한다.
	vector<string> OptionEntry;
	{
		CHVCableCreationSettingTable& VoltageSettingTable = CHVCableCreationSettingTable::GetInstance();
		VoltageSettingTable.Load(rProjectMDBFilePath);
		for(vector<CCableCreationTable::Record>::iterator itr = VoltageSettingTable.m_RecordEntry.begin();itr != VoltageSettingTable.m_RecordEntry.end();++itr)
		{
			map<string,string>::iterator where = itr->FieldValueMap.find(_T("C_BUS_VOLTAGE"));
			if(where != itr->FieldValueMap.end())
			{
				const string rVoltage = where->second;
				if(!rVoltage.empty()) OptionEntry.push_back(rVoltage);
			}
		}
	}

	{
		CLV3PHCableCreationSettingTable& VoltageSettingTable = CLV3PHCableCreationSettingTable::GetInstance();
		VoltageSettingTable.Load(rProjectMDBFilePath);
		for(vector<CCableCreationTable::Record>::iterator itr = VoltageSettingTable.m_RecordEntry.begin();itr != VoltageSettingTable.m_RecordEntry.end();++itr)
		{
			map<string,string>::iterator where = itr->FieldValueMap.find(_T("C_BUS_VOLTAGE"));
			if(where != itr->FieldValueMap.end())
			{
				const string rVoltage = where->second;
				if(!rVoltage.empty()) OptionEntry.push_back(rVoltage);
			}
		}
	}

	{
		CLV1PHCableCreationSettingTable& VoltageSettingTable = CLV1PHCableCreationSettingTable::GetInstance();
		VoltageSettingTable.Load(rProjectMDBFilePath);
		for(vector<CCableCreationTable::Record>::iterator itr = VoltageSettingTable.m_RecordEntry.begin();itr != VoltageSettingTable.m_RecordEntry.end();++itr)
		{
			map<string,string>::iterator where = itr->FieldValueMap.find(_T("C_BUS_VOLTAGE"));
			if(where != itr->FieldValueMap.end())
			{
				const string rVoltage = where->second;
				if(!rVoltage.empty()) OptionEntry.push_back(rVoltage);
			}
		}
	}

	{
		CDCCableCreationSettingTable& VoltageSettingTable = CDCCableCreationSettingTable::GetInstance();
		VoltageSettingTable.Load(rProjectMDBFilePath);
		for(vector<CCableCreationTable::Record>::iterator itr = VoltageSettingTable.m_RecordEntry.begin();itr != VoltageSettingTable.m_RecordEntry.end();++itr)
		{
			map<string,string>::iterator where = itr->FieldValueMap.find(_T("C_BUS_VOLTAGE"));
			if(where != itr->FieldValueMap.end())
			{
				const string rVoltage = where->second;
				if(!rVoltage.empty()) OptionEntry.push_back(rVoltage);
			}
		}
	}
	
	::stable_sort(OptionEntry.begin() , OptionEntry.end());
	if(!OptionEntry.empty())
	{
		for(vector<string>::iterator itr = OptionEntry.begin();itr != OptionEntry.end();++itr) AddOption(itr->c_str());
	}


	return CMFCPropEx::OnEdit(lptClick);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CMFCLoadVoltageComboProp, CMFCPropEx)

/**
	@brief	ComboBox가 생성될때 Load Voltage들을 읽어와서 Option으로 추가한다.

	@author	BHK
*/
BOOL CMFCLoadVoltageComboProp::OnEdit(LPPOINT lptClick)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();
	const string rProjectMDBFilePath = docData.GetProjectMDBFilePath();

	RemoveAllOptions();	//! 기존의 모든 OPTION들을 삭제한다.
	
	set<string> OptionEntry;
	{
		CHVCableCreationSettingTable& VoltageSettingTable = CHVCableCreationSettingTable::GetInstance();
		VoltageSettingTable.Load(rProjectMDBFilePath);
		for(vector<CCableCreationTable::Record>::iterator itr = VoltageSettingTable.m_RecordEntry.begin();itr != VoltageSettingTable.m_RecordEntry.end();++itr)
		{
			map<string,string>::iterator where = itr->FieldValueMap.find(_T("C_LOAD_VOLTAGE"));
			if(where != itr->FieldValueMap.end())
			{
				const string rVoltage = where->second;
				if(!rVoltage.empty()) OptionEntry.insert(rVoltage);
			}
		}
	}

	{
		CLV3PHCableCreationSettingTable& VoltageSettingTable = CLV3PHCableCreationSettingTable::GetInstance();
		VoltageSettingTable.Load(rProjectMDBFilePath);
		for(vector<CCableCreationTable::Record>::iterator itr = VoltageSettingTable.m_RecordEntry.begin();itr != VoltageSettingTable.m_RecordEntry.end();++itr)
		{
			map<string,string>::iterator where = itr->FieldValueMap.find(_T("C_LOAD_VOLTAGE"));
			if(where != itr->FieldValueMap.end())
			{
				const string rVoltage = where->second;
				if(!rVoltage.empty()) OptionEntry.insert(rVoltage);
			}
		}
	}

	{
		CLV1PHCableCreationSettingTable& VoltageSettingTable = CLV1PHCableCreationSettingTable::GetInstance();
		VoltageSettingTable.Load(rProjectMDBFilePath);
		for(vector<CCableCreationTable::Record>::iterator itr = VoltageSettingTable.m_RecordEntry.begin();itr != VoltageSettingTable.m_RecordEntry.end();++itr)
		{
			map<string,string>::iterator where = itr->FieldValueMap.find(_T("C_LOAD_VOLTAGE"));
			if(where != itr->FieldValueMap.end())
			{
				const string rVoltage = where->second;
				if(!rVoltage.empty()) OptionEntry.insert(rVoltage);
			}
		}
	}

	{
		CDCCableCreationSettingTable& VoltageSettingTable = CDCCableCreationSettingTable::GetInstance();
		VoltageSettingTable.Load(rProjectMDBFilePath);
		for(vector<CCableCreationTable::Record>::iterator itr = VoltageSettingTable.m_RecordEntry.begin();itr != VoltageSettingTable.m_RecordEntry.end();++itr)
		{
			map<string,string>::iterator where = itr->FieldValueMap.find(_T("C_LOAD_VOLTAGE"));
			if(where != itr->FieldValueMap.end())
			{
				const string rVoltage = where->second;
				if(!rVoltage.empty()) OptionEntry.insert(rVoltage);
			}
		}
	}
	
	///::stable_sort(OptionEntry.begin() , OptionEntry.end());
	if(!OptionEntry.empty())
	{
		for(set<string>::iterator itr = OptionEntry.begin();itr != OptionEntry.end();++itr) AddOption(itr->c_str());
	}

	return CMFCPropEx::OnEdit(lptClick);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CMFCBusNameComboProp, CMFCPropEx)

//! BUS NAME SORT 
class CSort_Comp
{
public:
	bool operator()(const CELoadItem* lhs , const CELoadItem* rhs) const
	{
		const string left = lhs->GetName();
		const string right = rhs->GetName();
		size_t nMax = 0;
		(left.length() < left.length())?nMax = right.length() : nMax = left.length();
		for(size_t i = 0; i < nMax; i++)
		{
			//! 스트링의 길이가 다를때 비교
			if(i == (nMax - 1) && left[nMax - 1] == right[nMax - 1])
			{
				return (left.length() < right.length());
			}
			//! 스트링의 길이가 같을때까지 비교
			if(left[i] == right[i])
			{
				continue;
			}
			else
				return (left[i] < right[i]);
		}
		return false;
	}
};

/**
	@brief	

	@author BHK	

	@date 2009-04-06 오후 1:06:06	

	@param	

	@return		
*/
void CMFCBusNameComboProp::OnSelectCombo()
{
	const CString rValue = GetValue();
	CMFCPropEx::OnSelectCombo();

	COMMAND::CCommandCircleBusConnectionCheck cmdCircleBusConnectionCheck;
	if(ERROR_SUCCESS != cmdCircleBusConnectionCheck.Execute())
	{
		AfxMessageBox(_T("BUS 연결 그래프가 순환되므로 선택할수 없습니다."));
		SetValue(COleVariant(rValue));
	}
}


/**
	@brief	ComboBox가 생성될때 Bus들을 읽어와서 이름을 Option으로 추가한다.

	@author	BHK
*/
BOOL CMFCBusNameComboProp::OnEdit(LPPOINT lptClick)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	RemoveAllOptions();	//! 기존의 모든 OPTION들을 삭제한다.

	list<CELoadItem*> BusItemList;
	docData.GetELoadItemListOf(BusItemList , CBusItem::TypeString());
	//if(pItemList)
	{
		//! BUS LIST를 BUS NAME으로 정렬
		BusItemList.sort(CSort_Comp());

		for(list<CELoadItem*>::iterator itr = BusItemList.begin();itr != BusItemList.end();++itr)
		{
			if((*itr)->IsDeleted()) continue;
			AddOption((*itr)->GetName().c_str());
		}
	}

	return CMFCPropEx::OnEdit(lptClick);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CMFCButtonProp, CMFCPropEx)

////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CMFCShowButtonProp, CMFCPropEx)

/**
	@brief	

	@author	BHK
*/
/*
CWnd* CMFCShowCableButtonProp::CreateInPlaceEdit (CRect rectEdit, BOOL& bDefaultFormat)
{
	CWnd* pWnd = new CWnd;
	if(pWnd)
	{
		pWnd->Create(NULL , _T("") , WS_CHILD | WS_VISIBLE , rectEdit , m_pWndList , BCGPROPLIST_ID_INPLACE);
		return pWnd;
	}

	return NULL;
}
*/

/**
	@brief	Cable을 보여주는 다이얼로그를 띄운다.

	@author	BHK
*/
void CMFCShowButtonProp::OnClickButton(CPoint point )
{
	CWnd* pParent = static_cast<CPropertiesWnd*>(m_pWndList->GetParent());
	if(pParent && pParent->IsKindOf(RUNTIME_CLASS(CELoadPropTabWnd))) pParent = pParent->GetParent();

	if(pParent && pParent->IsKindOf(RUNTIME_CLASS(CPropertiesWnd)))
	{
		CPropertiesWnd* pPropertiesWnd = static_cast<CPropertiesWnd*>(pParent);

		const int nItemCount = pPropertiesWnd->GetSelectionSet().GetItemCount();
		if(1 == nItemCount)
		{
			ITEMS::CELoadItem* pItem = pPropertiesWnd->GetSelectionSet().GetItemAt(0);
			if(pItem->IsKindOf(CLoadItem::TypeString()))
			{
				ITEMS::CLoadItem* pLoadItem = static_cast<ITEMS::CLoadItem*>(pItem);
				
				const CString rKey = CString(GetName()).MakeUpper();
				if(_T("SHOW CABLE") == rKey)
				{
					CShowCableDlg dlg(pLoadItem);
					(IDOK == dlg.DoModal());
				}
				else if(_T("REMARK") == rKey)
				{
					CShowRemarkDlg dlg(pLoadItem);
					(IDOK == dlg.DoModal());
				}
			}
		}
	}
}

/**
	@brief	

	@author	BHK
*/
BOOL CMFCShowButtonProp::PushChar (UINT nChar)
{
	if (nChar == VK_SPACE)
	{
		OnClickButton (CPoint (-1, -1));
	}

	return FALSE;
}

/**
	@brief	

	@author	BHK
*/
void CMFCShowButtonProp::OnDrawValue (CDC* pDC, CRect rect)
{
	CMFCPropEx::OnDrawValue(pDC , CRect(rect.left,rect.top,rect.right - 14,rect.bottom));
	
	rect.left = rect.right - 14;
	OnDrawButton(pDC , rect);	//! 항상 버튼을 그리도록 한다.
}

////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CMFCCableSizeComboProp, CMFCPropEx)

/**
	@brief	ComboBox가 생성될때 Cable Data Table을 읽어와서 Option으로 추가한다.

	@author	BHK
*/
BOOL CMFCCableSizeComboProp::OnEdit(LPPOINT lptClick)
{
	if(m_pWndList) m_pWndList->BeginWaitCursor();

	CELoadDocData& docData = CELoadDocData::GetInstance();
	RemoveAllOptions();	//! 기존의 모든 OPTION들을 삭제한다.
	
	CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
	const CString rDataName = pSettingTable->m_RecordEntry[0].FieldValueMap[_T("C_CUR_CABLE_SIZING_SETTING_TABLE_NAME")].c_str();

	CCableSizeSettingTable& CableSizeSettingTable = CCableSizeSettingTable::GetInstance();
	CableSizeSettingTable.Load();
	docData.m_ProjectCableDataTable.Load(CableSizeSettingTable.m_RecordMap[rDataName].m_rCableDataTable);

	if(m_pItem && m_pItem->IsKindOf(CCableItem::TypeString()))
	{
		const string rCableVolt = m_pItem->prop()->GetValue(_T("General") , _T("Cable Volt"));
		const string rCableType = m_pItem->prop()->GetValue(_T("General") , _T("Cable Type"));
		const string rCableCore = m_pItem->prop()->GetValue(_T("General") , _T("Core"));

		const string rType = (rCableVolt + _T("KV ") + rCableCore + _T(" CU ") + rCableType + _T(" (eLoad)"));
		
		CCableDataByType* pCableDataByType = docData.m_ProjectCableDataTable.GetRecordListOf(rType);
		if(pCableDataByType)
		{
			vector<CCableDataByType::Record*>* pRecordList = pCableDataByType->GetRecordEntryOfHertz();
			for(vector<CCableDataByType::Record*>::iterator itr = pRecordList->begin();itr != pRecordList->end();++itr)
			{
				const string rCableSize = (*itr)->m_FieldMap[_T("C_SIZE")];
				AddOption(rCableSize.c_str());
			}
		}
	}
	if(m_pWndList) m_pWndList->EndWaitCursor();

	return CMFCPropEx::OnEdit(lptClick);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CMFCCableCoreComboProp, CMFCPropEx)

/**
	@brief	ComboBox가 생성될때 Cable Data Table을 읽어와서 Core를 Option으로 추가한다.

	@author	BHK
*/
BOOL CMFCCableCoreComboProp::OnEdit(LPPOINT lptClick)
{
	if(m_pWndList) m_pWndList->BeginWaitCursor();

	CELoadDocData& docData = CELoadDocData::GetInstance();
	RemoveAllOptions();	//! 기존의 모든 OPTION들을 삭제한다.
	
	CProjectSettingTable* pSettingTable = static_cast<CProjectSettingTable*>(docData.GetTableOf(CProjectSettingTable::GetTableName()));
	const CString rDataName = pSettingTable->m_RecordEntry[0].FieldValueMap[_T("C_CUR_CABLE_SIZING_SETTING_TABLE_NAME")].c_str();

	CCableSizeSettingTable& CableSizeSettingTable = CCableSizeSettingTable::GetInstance();
	CableSizeSettingTable.Load();
	docData.m_ProjectCableDataTable.Load(CableSizeSettingTable.m_RecordMap[rDataName].m_rCableDataTable);

	if(m_pItem && m_pItem->IsKindOf(CCableItem::TypeString()))
	{
		string* pValue = pSettingTable->GetFieldValue(_T("C_HERTZ"));
		const int nHertz = ((NULL != pValue) && !pValue->empty()) ? atoi(pValue->c_str()) : 50;

		const string rCableVolt = m_pItem->prop()->GetValue(_T("General") , _T("Cable Volt"));
		const string rCableType = m_pItem->prop()->GetValue(_T("General") , _T("Cable Type"));
		
		vector<string> CableCoreList;
		docData.m_ProjectCableDataTable.GetCableCoreList(nHertz , rCableVolt , rCableType , CableCoreList);

		for(vector<string>::iterator itr = CableCoreList.begin();itr != CableCoreList.end();++itr)
		{
			AddOption(itr->c_str());
		}
	}
	if(m_pWndList) m_pWndList->EndWaitCursor();

	return CMFCPropEx::OnEdit(lptClick);
}