// BusToBusRelationDraw.h: interface for the CBusToBusRelationDraw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUSTOBUSRELATIONNDRAW_H__35CD9929_7587_452B_924B_B87A4C370EC3__INCLUDED_)
#define AFX_BUSTOBUSRELATIONNDRAW_H__35CD9929_7587_452B_924B_B87A4C370EC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RelationDraw.h"

namespace BusToBus
{
class CBusToBusRelationDraw : public CRelationDraw  
{
public:
	virtual void DrawFocusObject(CDC *pDC, CIsDrawEntity *pEnt , CIsDrawEntContainer* pDoc , const CIsDrawEditor* pDrawEditor);
	CBusToBusRelationDraw();
	virtual ~CBusToBusRelationDraw();

	IS_DECLARE_DRAW_FUNC()
protected:
	virtual void DrawObject(CDC *pDC, CIsDrawEntity *pEnt , CIsDrawEntContainer* pDoc , const CIsDrawEditor* pDrawEditor);
};
};

#endif // !defined(AFX_BUSTOBUSRELATIONNDRAW_H__35CD9929_7587_452B_924B_B87A4C370EC3__INCLUDED_)
