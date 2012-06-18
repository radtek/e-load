// Entity.h: interface for the CEntity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TINYUMLENTITY_H__F410B452_FEA7_41BD_94CB_13E5E8D364E9__INCLUDED_)
#define AFX_TINYUMLENTITY_H__F410B452_FEA7_41BD_94CB_13E5E8D364E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <aDrawCtrl/inc/IsDrawEntity.h>
#include <aDrawCtrl/inc/IsDrawEditor.h>
#include <aDrawCtrl/inc/IsDrawEntController.h>

namespace BusToBus
{
class CEntity : public CIsDrawEntity
{
protected:
	CEntity();
public:
	static const CString TypeString();
	virtual bool IsKindOf(CString rType);
	virtual void Serialize(CArchive &ar);
	CEntity(CIsDrawEntContainer* pDoc,CIsControllerObject* pController);
	virtual ~CEntity();
protected:
	virtual void Copy(CEntity* pEntity);
	long m_dragHandle;
	bool m_bDoneSetupTheme;
};

class CBusEntity : public CEntity
{
protected:
	CBusEntity();
public:
	int Save(CADODB& adoDB);	//! 2011.01.03 added - by HumKyung

	CBusItem* GetBusItem() const
	{
		return m_pBusItem;
	}
	int SetBusItem(CBusItem* pBusItem)
	{
		m_pBusItem = pBusItem;
		return ERROR_SUCCESS;
	}

	CString GetText()
	{
		return m_pBusItem->GetName().c_str();
	};

	CFont* GetTextFont();
	void SetTextFont(CFont *pFont);
	CRect GetTextRect() const;

	virtual void Serialize(CArchive &ar);
	CBusEntity(CIsDrawEntContainer* pDoc,CIsControllerObject* pController);
	virtual ~CBusEntity();

	IS_OBJ_DECLARE_FUNC()
protected:
	virtual void Copy(CBusEntity* pBusItemEnt);
	CFont m_TextFont;
	long m_cornerWidth;
	long m_cornerHeight;
private:
	CBusItem* m_pBusItem;
};

class CBusEntityDraw : public CIsDrawObject  
{
public:
	virtual void DrawFocusObject(CDC *pDC, CIsDrawEntity *pEnt , CIsDrawEntContainer* pDoc , const CIsDrawEditor* pDrawEditor);
	CBusEntityDraw();
	virtual ~CBusEntityDraw();

	IS_DECLARE_DRAW_FUNC()
protected:
	virtual void DrawObject(CDC *pDC, CIsDrawEntity *pEnt , CIsDrawEntContainer* pDoc , const CIsDrawEditor* pDrawEditor);
private:
	void DrawShadow(CDC* pDC,const CPoint& ptOrigin,const double& nWidth,const double& nHeight , 
		const long& cornerWidth , const long& cornerHeight);
	
	COLORREF m_fillColor;
};

class CBusEntityController : public CIsDrawEntController  
{
public:
	virtual void OnLButtonDblClk(const IS_MOUSE_EVENT& mouse_event);
	virtual void OnMouseMove(const IS_MOUSE_EVENT& mouse_event);
	virtual void OnLButtonDown(const IS_MOUSE_EVENT& mouse_event);
	CBusEntityController();
	virtual ~CBusEntityController();

	IS_DECLARE_CONTROLLER_FUNC()
public:
	virtual void OnKeyDown(const IS_KEY_EVENT& key_event);
	bool m_bInterface;
private:
	CPoint m_ptOffset;
};
};

#endif // !defined(AFX_TINYUMLENTITY_H__F410B452_FEA7_41BD_94CB_13E5E8D364E9__INCLUDED_)
