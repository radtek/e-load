// RelationDraw.h: interface for the CRelationDraw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TINYUMLRELATIONDRAW_H__FCA5F255_886A_4775_B0E9_02491E5D64B0__INCLUDED_)
#define AFX_TINYUMLRELATIONDRAW_H__FCA5F255_886A_4775_B0E9_02491E5D64B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <aDrawCtrl/inc/IsDrawObject.h>

namespace BusToBus
{
class CRelationDraw : public CIsDrawObject  
{
public:
	virtual void DrawTracker(CDC* pDC , CIsDrawEntity* pEnt , CIsDrawEntContainer* pDoc , const CIsDrawEditor* pDrawEditor) const;
	virtual void DrawFocusObject(CDC *pDC, CIsDrawEntity *pEnt , CIsDrawEntContainer* pDoc , const CIsDrawEditor* pDrawEditor);
	CRelationDraw();
	virtual ~CRelationDraw();

	IS_DECLARE_DRAW_FUNC()
protected:
	virtual void DrawSelectionMarkers( CDC* dc, CIsDrawEntity* pEnt , const CIsDrawEditor* pDrawEditor) const;
	virtual void DrawObject(CDC *pDC, CIsDrawEntity *pEnt , CIsDrawEntContainer* pDoc , const CIsDrawEditor* pDrawEditor);
};
};

#endif // !defined(AFX_TINYUMLRELATIONDRAW_H__FCA5F255_886A_4775_B0E9_02491E5D64B0__INCLUDED_)
