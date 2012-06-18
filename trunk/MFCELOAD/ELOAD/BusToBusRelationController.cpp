// TinyUMLAssociationController.cpp: implementation of the CBusToBusRelationController class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <aDrawCtrl/inc/IsDrawEditor.h>
#include "BusToBusRelation.h"
#include "BusToBusRelationController.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace BusToBus;

IS_IMPLEMENT_CONTROLLER_FUNC(CBusToBusRelationController)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBusToBusRelationController::CBusToBusRelationController()
{

}

CBusToBusRelationController::~CBusToBusRelationController()
{

}

/**
 * OnLButtonDown:
 *
 * @param mouse_event 
 * @return void 
 */
void CBusToBusRelationController::OnLButtonDown(const IS_MOUSE_EVENT& mouse_event)
{
	CIsDrawEditor* pEditor=mouse_event.pEditor;

	if(NULL == mouse_event.pEnt)
	{
	}
	else
	{
		CBusToBusRelation* pEnt=static_cast<CBusToBusRelation*>(mouse_event.pEnt);
		m_nHitCode = mouse_event.pEnt->HitTest(mouse_event.xsnapoint);
		if(mouse_event.pEnt->IsSelected())
		{
			pEnt->BeginDrag(mouse_event.virtpoint, pEnt->handleAt(mouse_event.xsnapoint));
		}
		else
		{
			if(NULL != mouse_event.pEnt) mouse_event.pEnt->Select(TRUE);
			
			//////////////////////////////////////////////////////////////////////////
			IS_NOTIFY notify;
			notify.hdr.hwndFrom = pEditor->GetSafeHwnd();
			notify.pEntFrom = mouse_event.pEnt;
			pEditor->SendMessage(WM_NOTIFY,(WPARAM)IS_SELECT_ENTITY,(LPARAM)(&notify));

			pEnt->BeginDrag(mouse_event.virtpoint,pEnt->handleAt(mouse_event.xsnapoint));
		}
	}
}

/**	\brief	The CBusToBusRelationController::OnMouseMove function

	\param	mouse_event	a parameter of type const IS_MOUSE_EVENT&

	\return	void	
*/
void CBusToBusRelationController::OnMouseMove(const IS_MOUSE_EVENT& mouse_event)
{
	CIsDrawEditor* pEditor=mouse_event.pEditor;

	if(NULL != mouse_event.pEnt)
	{
		if(mouse_event.pEnt->IsSelected())
		{
			CBusToBusRelation* pEnt=static_cast<CBusToBusRelation*>(mouse_event.pEnt);
			pEnt->Drag(mouse_event.virtpoint);
			pEditor->RedrawWindow();

			pEnt->GetDoc()->SetModified(TRUE);
		}
	}
	else
	{

	}
}

/**	\brief	The CBusToBusRelationController::OnLButtonUp function

	\param	mouse_event	a parameter of type const IS_MOUSE_EVENT&

	\return	void	
*/
void CBusToBusRelationController::OnLButtonUp(const IS_MOUSE_EVENT& mouse_event)
{
	if(NULL == mouse_event.pEnt)
	{
	}
	else
	{
		CBusToBusRelation* pRel=static_cast<CBusToBusRelation*>(mouse_event.pEnt);
		pRel->EndDrag();
	}
}
