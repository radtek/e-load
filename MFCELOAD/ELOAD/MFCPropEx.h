#pragma once

#include "ELOADSpinButtonCtrl.h"
#include "ELoadItem.h"
#include "Resource.h"

#include <string>
using namespace std;

// CMFCPropEx command target
class CBCGPPropList;
class CPropertiesWnd;
///namespace MFCPROPERTY
///{
class CMFCPropEx : public CMFCPropertyGridProperty
{
	DECLARE_DYNCREATE(CMFCPropEx)

        friend class CBCGPPropList;
public:
	CMFCPropEx(const CString& strGroupName, DWORD_PTR dwData = 0, const CString& rLockType = UNLOCK_ITEM_STR ,
		BOOL bIsValueList = FALSE);

	// Simple property
	CMFCPropEx(const CString& strName, const _variant_t& varValue, const CString& rLockType = UNLOCK_ITEM_STR,
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL,
		LPCTSTR lpszValidChars = NULL);

	virtual ~CMFCPropEx();
protected:
	void OnDrawName (CDC* pDC, CRect rect);
	void OnClickName(  CPoint C );
// Overrides
public:
	void SetModifiedFlag( BOOL bFlag )
	{
		m_bIsModified = bFlag;
	}

	void SetValue (const _variant_t& varValue)
	{
		CMFCPropertyGridProperty::SetValue(varValue);

		///m_bIsModified = (IsValueChanged()) ? TRUE : FALSE;
	}

	void SetOriginalValue (const _variant_t& varValue)
	{
		CMFCPropertyGridProperty::SetOriginalValue(varValue);

		///m_bIsModified = (IsValueChanged()) ? TRUE : FALSE;
	}

	void SetUnitStr( const CString& rUnit )
	{
		m_rUnit = rUnit;
	}

	void SetDataType( const CString& rDataType )
	{
		m_rDataType = rDataType;
	}
	
	CMFCPropertyGridCtrl* GetPropertyGridCtrl() const
	{
		return m_pWndList;
	}
	void OnDrawValue (CDC* pDC, CRect rect);
	CWnd* CreateInPlaceEdit (CRect rectEdit, BOOL& bDefaultFormat);

	bool IsReadOnlyItem() const
	{
		return (READONLY_STR == m_rAccess);
	}

	void SetAccess(const CString& rAccess)
	{
		m_rAccess = rAccess;
		Enable(!IsReadOnlyItem());
		Redraw();
	}

	//! LOCK 관련 부분.
	bool IsGroupLockItem() const
	{
		return (GROUP_LOCK_ITEM_STR == m_rLockType);
	}

	void SetLock(const BOOL& bLock)
	{
		m_bLock = bLock;
		if(!IsReadOnlyItem()) Enable(!m_bLock);
		Redraw();
	}

	CString GetLockType() const
	{
		return m_rLockType;
	}

	static bool IsLockItemType( const CString& rLockType );
private:
	CString m_rUnit ,    m_rDataType , m_rAccess;
	CELOADSpinButtonCtrl m_wndSpinButtonCtrl;
	CWnd* m_pEditWnd;
protected:
	BOOL m_bLock;
	CString m_rLockType;
public:
	int SetCurUnitPos(const int& nPos);
};

////////////////////////////////////////////////////////////////////////////////////////////////////
class CMFCCheckBoxProp : public CMFCPropEx
{
	DECLARE_DYNCREATE(CMFCCheckBoxProp)
public:
	CMFCCheckBoxProp(
		const CString& strName,
		BOOL bCheck,
		const CString& rLockType,
		LPCTSTR lpszDescr = NULL,
		DWORD dwData = 0
	);

protected:
	virtual void SetValue (const _variant_t& varValue)
	{
		m_varValue = (COleVariant("Yes") == varValue) ? COleVariant("Yes") : COleVariant("No");
		if(m_pWndList) m_pWndList->InvalidateRect (NULL);
	}
	virtual BOOL OnEdit(LPPOINT lptClick);//						{	return FALSE;	}
	virtual void OnDrawValue(CDC* /*pDC*/, CRect /*rect*/);
	virtual BOOL HasButton () const									{	return FALSE;	}

	virtual BOOL PushChar (UINT nChar);
	virtual void OnDrawCheckBox (CDC * pDC, CRect rectCheck, BOOL bChecked);
	///virtual void OnDrawName (CDC* pDC, CRect rect);
	BOOL OnClickValue (UINT uiMsg, CPoint point);
	virtual BOOL OnDblClick (CPoint point);
public:
	int OnValueChanged();
	bool IsGroupLock(void) const;
	int GetGroupLockItemCount(void);
protected:
	CRect	m_rectCheck;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/*
class CMFCLockItemProp : public CMFCPropEx
{
	DECLARE_DYNCREATE(CMFCLockItemProp)
public:
	CMFCLockItemProp(const CString& strName , const _variant_t& varValue , const CString& rLockType = GROUP_LOCK_ITEM_STR) : CMFCPropEx(strName, COleVariant(varValue))
	{
		m_bLock = FALSE;
		m_rLockType = rLockType;
		AllowEdit((TRUE == m_bLock) ? FALSE : TRUE);
	}
public:
};
*/

////////////////////////////////////////////////////////////////////////////////////////////////////
class CMFCComboItemProp : public CMFCPropEx
{
	DECLARE_DYNCREATE(CMFCComboItemProp)
public:
	CMFCComboItemProp(const CString& strGroupName, DWORD_PTR dwData = 0 , const CString& rLockType = UNLOCK_ITEM_STR , BOOL bIsValueList = FALSE) : 
	CMFCPropEx(strGroupName, dwData , rLockType , bIsValueList)
	{
	}
	
	// Simple property
	CMFCComboItemProp(const CString& strName, const _variant_t& varValue, const CString& rLockType = UNLOCK_ITEM_STR ,
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL,
		LPCTSTR lpszValidChars = NULL) : 
	CMFCPropEx(strName,varValue, rLockType , lpszDescr,dwData,lpszEditMask,lpszEditTemplate, lpszValidChars)
	{}

	CWnd* CMFCComboItemProp::CreateInPlaceEdit (CRect rectEdit, BOOL& bDefaultFormat)
	{
		CEdit* pWndEdit = NULL;
		pWndEdit = (CEdit*)CMFCPropertyGridProperty::CreateInPlaceEdit(rectEdit , bDefaultFormat);
		return pWndEdit;
	}
public:
};

class CMFCBusVoltageComboProp : public CMFCPropEx
{
	DECLARE_DYNCREATE(CMFCBusVoltageComboProp)
public:
	CMFCBusVoltageComboProp(const CString& strGroupName, DWORD_PTR dwData = 0 , const CString& rLockType = UNLOCK_ITEM_STR , BOOL bIsValueList = FALSE) : 
      CMFCPropEx(strGroupName, dwData , rLockType , bIsValueList)
	{
	}
	
	// Simple property
	CMFCBusVoltageComboProp(const CString& strName, const _variant_t& varValue, const CString& rLockType = UNLOCK_ITEM_STR ,
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL,
		LPCTSTR lpszValidChars = NULL) : 
	CMFCPropEx(strName,varValue, rLockType , lpszDescr,dwData,lpszEditMask,lpszEditTemplate, lpszValidChars)
	{}
public:
	BOOL OnEdit(LPPOINT lptClick);
	CWnd* CMFCBusVoltageComboProp::CreateInPlaceEdit (CRect rectEdit, BOOL& bDefaultFormat)
	{
		CEdit* pWndEdit = NULL;
		pWndEdit = (CEdit*)CMFCPropertyGridProperty::CreateInPlaceEdit(rectEdit , bDefaultFormat);
		return pWndEdit;
	}
};

class CMFCLoadVoltageComboProp : public CMFCPropEx
{
	DECLARE_DYNCREATE(CMFCLoadVoltageComboProp)
public:
	CMFCLoadVoltageComboProp(const CString& strGroupName, DWORD_PTR dwData = 0 , const CString& rLockType = _T("UNLOCK") , BOOL bIsValueList = FALSE) : 
      CMFCPropEx(strGroupName, dwData , rLockType , bIsValueList)
	{
	}
	
	// Simple property
	CMFCLoadVoltageComboProp(const CString& strName, const _variant_t& varValue, const CString& rLockType = _T("UNLOCK") ,
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL,
		LPCTSTR lpszValidChars = NULL) : 
	CMFCPropEx(strName,varValue, lpszDescr , rLockType , dwData,lpszEditMask,lpszEditTemplate, lpszValidChars)
	{}
public:
	BOOL OnEdit(LPPOINT lptClick);
	CWnd* CMFCLoadVoltageComboProp::CreateInPlaceEdit (CRect rectEdit, BOOL& bDefaultFormat)
	{
		CEdit* pWndEdit = NULL;
		pWndEdit = (CEdit*)CMFCPropertyGridProperty::CreateInPlaceEdit(rectEdit , bDefaultFormat);
		return pWndEdit;
	}
};

class CMFCBusNameComboProp : public CMFCPropEx
{
	DECLARE_DYNCREATE(CMFCBusNameComboProp)
public:
	CMFCBusNameComboProp(const CString& strGroupName, DWORD_PTR dwData = 0 , const CString& rLockType = _T("UNLOCK") , BOOL bIsValueList = FALSE) : 
		CMFCPropEx(strGroupName, dwData , rLockType , bIsValueList)
	{
	}
	
	// Simple property
	CMFCBusNameComboProp(const CString& strName, const _variant_t& varValue, const CString& rLockType = _T("UNLOCK") ,
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL,
		LPCTSTR lpszValidChars = NULL) : 
	CMFCPropEx(strName,varValue, lpszDescr , rLockType , dwData,lpszEditMask,lpszEditTemplate, lpszValidChars)
	{}
public:
	void OnSelectCombo();
	BOOL OnEdit(LPPOINT lptClick);
	CWnd* CMFCBusNameComboProp::CreateInPlaceEdit (CRect rectEdit, BOOL& bDefaultFormat)
	{
		CEdit* pWndEdit = NULL;
		pWndEdit = (CEdit*)CMFCPropertyGridProperty::CreateInPlaceEdit(rectEdit , bDefaultFormat);
		return pWndEdit;
	}
};

class CMFCButtonProp : public CMFCPropEx
{
	DECLARE_DYNCREATE(CMFCButtonProp)
public:
	CMFCButtonProp(const CString& strGroupName, DWORD_PTR dwData = 0 , const CString& rLockType = _T("UNLOCK") , BOOL bIsValueList = FALSE) : 
		CMFCPropEx(strGroupName, dwData , rLockType , bIsValueList)
      {
      }

      // Simple property
      CMFCButtonProp(const CString& strName, const _variant_t& varValue, const CString& rLockType = _T("UNLOCK") ,
	      LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
	      LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL,
	      LPCTSTR lpszValidChars = NULL) : 
		CMFCPropEx(strName,varValue, lpszDescr , rLockType , dwData,lpszEditMask,lpszEditTemplate, lpszValidChars)
      {}
public:
	BOOL HasButton() const { return TRUE;}
};

class CMFCShowButtonProp : public CMFCPropEx
{
	DECLARE_DYNCREATE(CMFCShowButtonProp)
public:
	CMFCShowButtonProp(const CString& strGroupName, DWORD_PTR dwData = 0 , const CString& rLockType = _T("UNLOCK") , BOOL bIsValueList = FALSE) : 
		CMFCPropEx(strGroupName, dwData , rLockType , bIsValueList) , m_pItemPropTag(NULL)
	{
	}
	
	// Simple property
	CMFCShowButtonProp(const CString& strName, const _variant_t& varValue, const CString& rLockType = _T("UNLOCK") ,
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL,
		LPCTSTR lpszValidChars = NULL) : 
		CMFCPropEx(strName,varValue, lpszDescr , rLockType , dwData,lpszEditMask,lpszEditTemplate, lpszValidChars) , m_pItemPropTag(NULL)
	{}
public:
	///CWnd* CreateInPlaceEdit (CRect rectEdit, BOOL& bDefaultFormat);

	BOOL PushChar (UINT nChar);
	void OnClickButton(CPoint point );
	void OnDrawValue (CDC* pDC, CRect rect);
	BOOL HasButton() const { return TRUE;}

	ITEMS::CItemPropCategory::ItemPropTag* m_pItemPropTag;
};

class CMFCCableSizeComboProp : public CMFCPropEx
{
	DECLARE_DYNCREATE(CMFCCableSizeComboProp)
public:
	CMFCCableSizeComboProp(const CString& strGroupName, DWORD_PTR dwData = 0 , const CString& rLockType = _T("UNLOCK") , BOOL bIsValueList = FALSE) : 
		CMFCPropEx(strGroupName, dwData , rLockType , bIsValueList)
	{
		m_pItem = NULL;
	}
	
	// Simple property
	CMFCCableSizeComboProp(const CString& strName, const _variant_t& varValue, const CString& rLockType = _T("UNLOCK") ,
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL,
		LPCTSTR lpszValidChars = NULL) : 
	CMFCPropEx(strName,varValue, lpszDescr , rLockType , dwData,lpszEditMask,lpszEditTemplate, lpszValidChars)
	{
		m_pItem = NULL;
	}
public:
	///void OnSelectCombo();
	BOOL OnEdit(LPPOINT lptClick);
	CWnd* CMFCCableSizeComboProp::CreateInPlaceEdit (CRect rectEdit, BOOL& bDefaultFormat)
	{
		CEdit* pWndEdit = NULL;
		pWndEdit = (CEdit*)CMFCPropertyGridProperty::CreateInPlaceEdit(rectEdit , bDefaultFormat);
		return pWndEdit;
	}

	ITEMS::CELoadItem* m_pItem;
};

class CMFCCableCoreComboProp : public CMFCPropEx
{
	DECLARE_DYNCREATE(CMFCCableCoreComboProp)
public:
	CMFCCableCoreComboProp(const CString& strGroupName, DWORD_PTR dwData = 0 , const CString& rLockType = _T("UNLOCK") , BOOL bIsValueList = FALSE) : 
		CMFCPropEx(strGroupName, dwData , rLockType , bIsValueList)
	{
		m_pItem = NULL;
	}
	
	// Simple property
	CMFCCableCoreComboProp(const CString& strName, const _variant_t& varValue, const CString& rLockType = _T("UNLOCK") ,
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL,
		LPCTSTR lpszValidChars = NULL) : 
	CMFCPropEx(strName,varValue, lpszDescr , rLockType , dwData,lpszEditMask,lpszEditTemplate, lpszValidChars)
	{
		m_pItem = NULL;
	}
public:
	///void OnSelectCombo();
	BOOL OnEdit(LPPOINT lptClick);
	CWnd* CMFCCableCoreComboProp::CreateInPlaceEdit (CRect rectEdit, BOOL& bDefaultFormat)
	{
		CEdit* pWndEdit = NULL;
		pWndEdit = (CEdit*)CMFCPropertyGridProperty::CreateInPlaceEdit(rectEdit , bDefaultFormat);
		return pWndEdit;
	}

	ITEMS::CELoadItem* m_pItem;
};
///};