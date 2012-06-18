// RelationDraw.cpp: implementation of the CRelationDraw class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <assert.h>
#include "Relation.h"
#include "RelationDraw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace BusToBus;

IS_IMPLEMENT_DRAW_FUNC(CRelationDraw)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRelationDraw::CRelationDraw()
{

}

CRelationDraw::~CRelationDraw()
{

}

/**
 * DrawObject:
 *
 * @param pDC 
 * @param pEnt 
 * @param pDoc 
 * @return void 
 */
void CRelationDraw::DrawObject(CDC *pDC, CIsDrawEntity *pEnt, CIsDrawEntContainer *pDoc , const CIsDrawEditor* pDrawEditor)
{
	CRelation* pRel = static_cast<CRelation*>(pEnt);
	if(pRel->IsCreated())
	{
		CPen pen(pRel->GetPenStyle(), 0, pRel->GetLineColor()), *pOldPen;
		pOldPen = pDC->SelectObject(&pen);
		
		// polyline
		pDC->MoveTo(pRel->GetSrc().point());
		for(long i = 0; i < pRel->GetNodeSize(); ++i)
			pDC->LineTo(pRel->GetNodeAt(i));
		pDC->LineTo(pRel->GetDest().point());
		
		// name
		if(pRel->IsShowName())
		{
			CFont *pOldFont = pDC->SelectObject(pRel->GetNameFont());
			pDC->SetTextColor(pRel->GetLineColor());
			pDC->SetBkMode(TRANSPARENT);
			pDC->DrawText(pRel->GetName() , pRel->GetNameRect(),
				DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
			pDC->SelectObject(pOldFont);
		}
		
		pDC->SelectObject(pOldPen);
	}
}

/**
 * DrawFocusObject:
 *
 * @param pDC 
 * @param pEnt 
 * @param pDoc 
 * @return void 
 */
void CRelationDraw::DrawFocusObject(CDC *pDC, CIsDrawEntity *pEnt, CIsDrawEntContainer *pDoc , const CIsDrawEditor* pDrawEditor)
{
	CRelation* pRel = static_cast<CRelation*>(pEnt);
	if(pRel->IsCreated())
	{
		COLORREF line_color=RGB(0,0,255);
		CPen pen(pRel->GetPenStyle(), 0, line_color), *pOldPen;
		pOldPen = pDC->SelectObject(&pen);
		
		CBrush *pOldBrush;
		pOldBrush = (CBrush *)pDC->SelectStockObject(NULL_BRUSH);
		
		CSize markerSize = pRel->GetMarkerSize();
		CRect r;
		// polyline
		pDC->MoveTo(pRel->GetSrc().point());
		r = CRect(pRel->GetSrc().point(), CSize(0, 0));
		r.InflateRect(markerSize.cx * 0.5, markerSize.cy * 0.5);
		pDC->Ellipse(r);
		for(long i = 0; i < pRel->GetNodeSize(); ++i)
		{
			pDC->LineTo(pRel->GetNodeAt(i));
			
			r = CRect(pRel->GetNodeAt(i), CSize(0, 0));
			r.InflateRect(markerSize.cx * 0.5, markerSize.cy * 0.5);
			pDC->Ellipse(r);
		}
		pDC->LineTo(pRel->GetDest().point());
		r = CRect(pRel->GetDest().point(), CSize(0, 0));
		r.InflateRect(markerSize.cx * 0.5, markerSize.cy * 0.5);
		pDC->Ellipse(r);
		
		// name
		if(pRel->IsShowName())
		{
			CFont *pOldFont = pDC->SelectObject(pRel->GetNameFont());
			pDC->SetTextColor(line_color);
			pDC->SetBkMode(TRANSPARENT);
			pDC->DrawText(pRel->GetName(), pRel->GetNameRect(),
				DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
			pDC->SelectObject(pOldFont);
		}
		
		pDC->SelectObject(pOldPen);
	}
}

/**
 * DrawSelectionMarkers:
 *
 * @param dc 
 * @param pEnt 
 * @return void 
 */
void CRelationDraw::DrawSelectionMarkers(CDC *dc, CIsDrawEntity *pEnt , const CIsDrawEditor* pDrawEditor) const
{
	/// do nothing
}

/**
 * DrawTracker:
 *
 * @param pDC 
 * @param pEnt 
 * @param pDoc 
 * @return void 
 */
void CRelationDraw::DrawTracker(CDC *pDC, CIsDrawEntity *pEnt, CIsDrawEntContainer *pDoc , const CIsDrawEditor* pDrawEditor) const
{
	CRelation* pRel = static_cast<CRelation*>(pEnt);
	assert(NULL != pRel);

	long handleSize = pRel->GetHandleSize();
	
	CPen pen(PS_DOT, 0, pRel->GetLineColor()), *pOldPen;
	CPen handlePen(PS_SOLID, 0, RGB(100, 100, 255));
	CBrush *pOldBrush;
	
	pOldPen = pDC->SelectObject(&pen);
	pOldBrush = (CBrush *)pDC->SelectStockObject(NULL_BRUSH);
	pDC->SetBkMode(TRANSPARENT);
	
	if (pRel->IsCreated())
	{
		long i;
		CRect r;
		
		pDC->MoveTo(pRel->GetSrc().point());
		for(i = 0; i < pRel->GetNodeSize(); ++i)
			pDC->LineTo(pRel->GetNodeAt(i));
		pDC->LineTo(pRel->GetDest().point());
		
		pDC->SelectObject(&handlePen);
		
		r = CRect(pRel->GetSrc().point(), CSize(0, 0));
		r.InflateRect(handleSize, handleSize);
		pDC->Rectangle(r);
		
		r = CRect(pRel->GetDest().point(), CSize(0, 0));
		r.InflateRect(handleSize, handleSize);
		pDC->Rectangle(r);
		
		for(i = 0; i < pRel->GetNodeSize(); ++i)
		{
			r = CRect(pRel->GetNodeAt(i), CSize(0, 0));
			r.InflateRect(handleSize, handleSize);
			pDC->Ellipse(r);
		}
		
		if (pRel->IsShowName())
		{
			CRect selBox = pRel->GetNameRect();
			CBrush rectBrush(RGB(100, 100, 255)), *pOldBrush;
			pOldBrush = pDC->SelectObject(&rectBrush);
			pDC->FrameRect(selBox, &rectBrush);
			pDC->SelectObject(pOldBrush);
		}
		
	}
	else
	{
		pDC->MoveTo(pRel->GetSrc().point());
		pDC->LineTo(pRel->GetTempPos());
	}
	
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
}
