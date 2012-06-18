// Entity.cpp: implementation of the CEntity class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BusEntity.h"
#include "BusToBusDoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace BusToBus;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CEntity::CEntity() : CIsDrawEntity()
{
	m_bDoneSetupTheme = false;
}

/**
 * CEntity:
 *
 * @param pDoc 
 * @param pController 
 * @return  
 */
CEntity::CEntity(CIsDrawEntContainer* pDoc,CIsControllerObject* pController) : CIsDrawEntity(pDoc,pController)
{
	m_bDoneSetupTheme = false;
}

CEntity::~CEntity()
{

}

/**
 * Serialize:
 *
 * @param ar 
 * @return void 
 */
void CEntity::Serialize(CArchive &ar)
{
	CIsDrawEntity::Serialize(ar);
	
	if(ar.IsStoring())
	{
	}
	else
	{
	}
}

/**
 * Copy:
 *
 * @param pEntity 
 * @return void 
 */
void CEntity::Copy(CEntity *pEntity)
{
	CIsDrawEntity::Copy(pEntity);
	
	m_bDoneSetupTheme = pEntity->m_bDoneSetupTheme;
	m_dragHandle = pEntity->m_dragHandle;
}

/**
 * IsKindOf:
 *
 * @param rType 
 * @return bool 
 */
bool CEntity::IsKindOf(CString rType)
{
	if(TypeString() == rType) return true;

	return CIsDrawEntity::IsKindOf(rType);
}

/**
 * TypeString:
 *
 * @return const CString 
 */
const CString CEntity::TypeString()
{
	return _T("_tiny_uml_entity");
}

IS_OBJ_IMPLEMENT_FUNC(CBusEntity , CEntity , "_e-LOAD_BUS_")
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBusEntity::CBusEntity() : CEntity() , m_pBusItem(NULL)
{
}

/**
	@brief	CTinyUMLEntity:

	@author	HumKyung

	@date	2010.12.23

	@param

	@return
 */
CBusEntity::CBusEntity(CIsDrawEntContainer* pDoc,CIsControllerObject* pController) : CEntity(pDoc,pController) , m_pBusItem(NULL)
{
}

CBusEntity::~CBusEntity()
{

}

/**
	@brief

	@author	HumKyung

	@date	2010.12.23

	@param

	@return
 */
void CBusEntity::Serialize(CArchive &ar)
{
	CIsDrawEntity::Serialize(ar);
	
	if(ar.IsStoring())
	{
	}
	else
	{
	}
}

/**
	@brief

	@author	HumKyung

	@date	2010.12.23

	@param

	@return
 */
void CBusEntity::Copy(CBusEntity *pBusItemEnt)
{
	if(this != pBusItemEnt)
	{
		CEntity::Copy(pBusItemEnt);
	}
}

/**
	@brief

	@author	HumKyung

	@date	2010.12.23

	@param

	@return
 */
CFont* CBusEntity::GetTextFont()
{
	return &(m_TextFont);
}

/**
	@brief

	@author	HumKyung

	@date	2010.12.23

	@param

	@return
 */
CRect CBusEntity::GetTextRect() const
{
	CPoint ptOrigin(GetLeft(),GetTop());
	DWORD nWidth=GetRight() - GetLeft(),nHeight=GetBottom() - GetTop();
	CRect r(ptOrigin, CSize(nWidth, nHeight));
	r.DeflateRect(8, 4, 3, 3);
	
	return r;
}

/**
	@brief

	@author	HumKyung

	@date	2010.12.23

	@param

	@return
 */
void CBusEntity::SetTextFont(CFont *pFont)
{
	assert(pFont && "pFont is NULL");

	if(pFont)
	{		
		LOGFONT logFont;
		if(pFont->GetLogFont(&logFont))
		{
			m_TextFont.DeleteObject();
			m_TextFont.CreateFontIndirect(&logFont);
		}
	}
}

/**
	@brief	update bus location

	@author	HumKyung

	@date	2011.01.03

	@param

	@return
 */
int CBusEntity::Save(CADODB& adoDB)
{
	if(m_pBusItem)
	{
		stringstream oss;

		const LONG lLeft = GetLeft();
		oss << lLeft;
		m_pBusItem->prop()->SetValue(_T("Location") , _T("X") , oss.str());
		const LONG lTop = GetTop();
		oss.str(_T(""));
		oss << lTop;
		m_pBusItem->prop()->SetValue(_T("Location") , _T("Y") , oss.str());

		m_pBusItem->SaveData(adoDB , CBusItem::TableName());

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

//////////////////////////////////////////////////////////////////////
IS_IMPLEMENT_DRAW_FUNC(CBusEntityDraw)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBusEntityDraw::CBusEntityDraw()
{
	m_fillColor = RGB(200, 255, 255);
}

CBusEntityDraw::~CBusEntityDraw()
{

}

/**
	@brief

	@author	HumKyung

	@date	2010.12.23

	@param

	@return
 */
void CBusEntityDraw::DrawObject(CDC *pDC, CIsDrawEntity *pEnt, CIsDrawEntContainer *pDoc , const CIsDrawEditor* pDrawEditor)
{
	CBusEntity* pBusItemEnt = static_cast<CBusEntity*>(pEnt);
	CBusToBusContainer* pContainer = (CBusToBusContainer*)(pDoc);

	CPen pen(PS_SOLID, 0, RGB(0,0,0));
	CPen *pOldPen;
	CBrush brush(m_fillColor);
	CBrush shadowBrush(RGB(155,155,155));
	CBrush *pOldBrush = NULL;
	CFont *pOldFont = NULL , *pBusItemEntFont = pBusItemEnt->GetTextFont();
	
	pOldPen = (CPen *)pDC->SelectStockObject(NULL_PEN);
	pOldBrush = pDC->SelectObject(&shadowBrush);
	if(NULL != pBusItemEntFont)
		pOldFont = pDC->SelectObject(&(pContainer->m_font));
	
	CPoint ptOrigin(pBusItemEnt->GetLeft(),pBusItemEnt->GetTop());
	DWORD nWidth=pBusItemEnt->GetRight() - pBusItemEnt->GetLeft(),nHeight=pBusItemEnt->GetBottom() - pBusItemEnt->GetTop();
	
	//! shape
	CSize size = pDC->GetTextExtent(pBusItemEnt->GetText());
	pBusItemEnt->SetRect(ptOrigin.x , ptOrigin.y , ptOrigin.x + size.cx + 16 , ptOrigin.y + size.cy + 6);

	CPoint p[6];
	p[0] = ptOrigin;
	p[1] = ptOrigin + CSize(0, size.cy + 6);
	p[2] = ptOrigin + CSize(size.cx + 16 , size.cy + 6);
	p[3] = ptOrigin + CSize(size.cx + 16 , 0);
	pDC->SelectObject(&pen);
	pDC->SelectObject(&brush);
	pDC->Polygon(p, 4);

	//! text
	CRect rect(ptOrigin , size);
	pDC->SetTextColor(RGB(0,0,0));
	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(pBusItemEnt->GetText(), pBusItemEnt->GetTextRect(), DT_WORDBREAK | DT_NOPREFIX);
		
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldFont);
}

/**
	@brief

	@author	HumKyung

	@date	2010.12.23

	@param

	@return
 */
void CBusEntityDraw::DrawFocusObject(CDC *pDC, CIsDrawEntity *pEnt, CIsDrawEntContainer *pDoc , const CIsDrawEditor* pDrawEditor)
{
	CBusEntity* pBusItemEnt = static_cast<CBusEntity*>(pEnt);
	CBusToBusContainer* pContainer = (CBusToBusContainer*)(pDoc);

	CPen pen(PS_SOLID, 0, RGB(0,0,0));
	CPen *pOldPen;
	CBrush brush(m_fillColor);
	CBrush shadowBrush(RGB(155,155,155));
	CBrush *pOldBrush = NULL;
	CFont *pOldFont , *pBusItemEntFont = pBusItemEnt->GetTextFont();;
	
	pOldPen = (CPen *)pDC->SelectStockObject(NULL_PEN);
	pOldBrush = pDC->SelectObject(&shadowBrush);
	if(NULL != pBusItemEntFont)
		pOldFont = pDC->SelectObject(&(pContainer->m_font));
	
	CPoint ptOrigin(pBusItemEnt->GetLeft(),pBusItemEnt->GetTop());
	DWORD nWidth=pBusItemEnt->GetRight() - pBusItemEnt->GetLeft();
	DWORD nHeight=pBusItemEnt->GetBottom() - pBusItemEnt->GetTop();
	DrawShadow(pDC,ptOrigin,nWidth,nHeight , 0 , 0);
	
	// shape
	CPoint p[6];
	p[0] = ptOrigin;
	p[1] = ptOrigin + CSize(0, nHeight);
	p[2] = ptOrigin + CSize(nWidth, nHeight);
	p[3] = ptOrigin + CSize(nWidth, 0);
	p[4] = ptOrigin + CSize(nWidth, 0);
	p[5] = ptOrigin + CSize(nWidth, 0);
	pDC->SelectObject(&pen);
	pDC->SelectObject(&brush);
	pDC->Polygon(p, 5);
	
	// text
	pDC->SetTextColor(RGB(0,0,0));
	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(pBusItemEnt->GetText(), pBusItemEnt->GetTextRect(), DT_WORDBREAK | DT_NOPREFIX);
	
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldFont);

	if(pBusItemEnt->IsSelected()) DrawSelectionMarkers(pDC , pBusItemEnt , pDrawEditor);
}

/**
	@brief

	@author	HumKyung

	@date	2010.12.23

	@param

	@return
 */
void CBusEntityDraw::DrawShadow(CDC *pDC, const CPoint &ptOrigin, const double &nWidth, const double &nHeight
				  , const long& cornerWidth , const long& cornerHeight)
{
	CPoint sp[5];
	sp[0] = ptOrigin + CSize(5, 5);
	sp[1] = ptOrigin + CSize(5, nHeight + 5);
	sp[2] = ptOrigin + CSize(nWidth + 5, nHeight + 5);
	sp[3] = ptOrigin + CSize(nWidth + 5, cornerHeight + 5);
	sp[4] = ptOrigin + CSize(nWidth - cornerWidth + 5, 5);
	pDC->Polygon(sp, 5);
}

//////////////////////////////////////////////////////////////////////
IS_IMPLEMENT_CONTROLLER_FUNC(CBusEntityController)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBusEntityController::CBusEntityController()
{
	m_bInterface = false;
}

CBusEntityController::~CBusEntityController()
{

}

/**	\brief	The CBusEntityController::OnLButtonDown function

	\param	mouse_event	a parameter of type const IS_MOUSE_EVENT&

	\return	void
*/
void CBusEntityController::OnLButtonDown(const IS_MOUSE_EVENT& mouse_event)
{
	CIsDrawEditor* pEditor=mouse_event.pEditor;

	if(NULL != mouse_event.pEnt)
	{
		//////////////////////////////////////////////////////////////////////////
		/// select entity
		CPoint point(mouse_event.xsnapoint);
		m_nHitCode = mouse_event.pEnt->HitTest(point);
		
		IS_NOTIFY notify;
		notify.hdr.hwndFrom = pEditor->GetSafeHwnd();
		notify.pEntFrom = mouse_event.pEnt;
		pEditor->SendMessage(WM_NOTIFY,(WPARAM)IS_SELECT_ENTITY,(LPARAM)(&notify));
	}

	m_bLButtonDown = true;
	m_ptLDownPos = m_ptLastPos = mouse_event.virtpoint;
}

/**	\brief	The CBusEntityController::OnLButtonDblClk function

	\param	mouse_event	a parameter of type const IS_MOUSE_EVENT&

	\return	void	
*/
void CBusEntityController::OnLButtonDblClk(const IS_MOUSE_EVENT& mouse_event)
{
	assert(mouse_event.pEnt && "mouse_event.pEnt is NULL");

	if(mouse_event.pEnt)
	{
		/*
		CBusEntity* pEnt = static_cast<CBusEntity*>(mouse_event.pEnt);
		if(pEnt->GetNameRect().PtInRect(mouse_event.virtpoint))
		{
			CIsDrawEditor* pEditor = static_cast<CIsDrawEditor*>(mouse_event.pEditor);
			CDrawTextEdit* pEditCtrl = pEditor->BeginEditText(pEnt->Name(),pEnt->GetNameRect(),pEnt->GetNameFont(),ES_CENTER);
			pEditCtrl->SetAssociate(pEnt);
			pEditCtrl->RegisterCallbackFunc(SetClassName);
		}
		else
		{
			Set(mouse_event.pEnt,mouse_event.pEditor);
			ModifyProperties();
		}
		*/
	}
}

/**
 * OnMouseMove:
 *
 * @param mouse_event 
 * @return void 
 */
void CBusEntityController::OnMouseMove(const IS_MOUSE_EVENT& mouse_event)
{
	CIsDrawEditor* pEditor=mouse_event.pEditor;

	if(NULL != mouse_event.pEnt)
	{
		if(DEHT_BODY != m_nHitCode)
		{
			IS_NOTIFY notify;
			notify.hdr.hwndFrom = pEditor->GetSafeHwnd();
			notify.nHitCode = m_nHitCode;
			notify.virtPoint[0] = mouse_event.virtpoint;
			pEditor->SendMessage(WM_NOTIFY,(WPARAM)(IS_RESIZE_ENTITY),(LPARAM)(&notify));
		}
		else
		{
			IS_NOTIFY notify;
			notify.hdr.hwndFrom = pEditor->GetSafeHwnd();
			notify.virtPoint[0] = m_ptLastPos;
			notify.virtPoint[1] = mouse_event.virtpoint;
			pEditor->SendMessage(WM_NOTIFY,(WPARAM)(IS_MOVE_ENTITY),(LPARAM)(&notify));
		}
	}

	m_ptLastPos = mouse_event.virtpoint;
}

/**
 * OnKeyDown:
 *
 * @param key_event 
 * @return void 
 */
void CBusEntityController::OnKeyDown(const IS_KEY_EVENT &key_event)
{
	CIsDrawEditor* pEditor=key_event.pEditor;

	switch(key_event.nChar)
	{
		case VK_DELETE:
		{
			IS_NOTIFY notify;
			notify.hdr.hwndFrom = pEditor->GetSafeHwnd();
			notify.pEntFrom = GetEntity();
			pEditor->SendMessage(WM_NOTIFY,(WPARAM)IS_DELETE_ENTITY,(LPARAM)(&notify));
		}
			break;
	}
}