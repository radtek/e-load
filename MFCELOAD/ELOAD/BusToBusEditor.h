#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <aDrawCtrl/inc/IsDrawEditor.h>
#include <aDrawCtrl/inc/IsDrawObject.h>
#include <aDrawCtrl/inc/IsDrawEntContainer.h>

#include "Relation.h"

#include <map>
using namespace std;

#define TINY_UML_PREMODIFY_PROPERTY	WM_USER + 1020
#define TINY_UML_MODIFY_PROPERTY	WM_USER + 1021

namespace BusToBus
{

class CBusToBusContainer : public CIsDrawEntContainer
{
public:
	CBusToBusContainer();
	virtual ~CBusToBusContainer();

	CRelation* FindRelation(CIsDrawEntity* pSrcEnt , CIsDrawEntity* pDestEnt);
	virtual void Append( CIsObject* obj );
public:
	CFont m_font;
};

class CBusToBusEditor : public CIsDrawEditor  
{
public:
	CBusToBusEditor(CBusToBusContainer* pEntContainer);
	virtual ~CBusToBusEditor();

	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
protected:
	virtual void DrawBackground(CDC* pDC,CRect rect);
	virtual void DrawGrid(CDC *pDC, CRect rect);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTinyUMLView)
	//}}AFX_VIRTUAL
protected:
	DECLARE_MESSAGE_MAP()
};
};