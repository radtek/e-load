// BusToBusRelationController.cpp: implementation of the CBusToBusRelationFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <aDrawCtrl/inc/IsDrawEditor.h>
#include "BusToBusView.h"
#include "BusToBusRelation.h"
#include "BusToBusRelationFactory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace BusToBus;

IS_IMPLEMENT_CONTROLLER_FUNC(CBusToBusRelationFactory)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBusToBusRelationFactory::CBusToBusRelationFactory()
{

}

CBusToBusRelationFactory::~CBusToBusRelationFactory()
{

}

/**
	@brief	����ڰ� BusEnt���� ���콺�� Ŭ�������� BusToBus Relation�� �����Ѵ�.

	@author	HumKyung

	@date	2010.12.27

	@param

	@return
*/
void CBusToBusRelationFactory::OnLButtonDown(const IS_MOUSE_EVENT& mouse_event)
{
	CIsDrawEditor* pEditor=mouse_event.pEditor;

	long nHitCode=DEHT_NONE;
	CBusEntity* pBusEnt = static_cast<CBusEntity*>(pEditor->GetAt(nHitCode , mouse_event.virtpoint));
	if((NULL == pBusEnt) || !pBusEnt->IsKindOf(CBusEntity::TypeString()))
	{
		IS_NOTIFY notify;
		notify.hdr.hwndFrom = pEditor->GetSafeHwnd();
		notify.pEntFrom = NULL;
		pEditor->SendMessage(WM_NOTIFY,(WPARAM)IS_ABORT,(LPARAM)(&notify));

		AfxMessageBox(_T("Bus�� ������ �ּ���"));
		return;
	}
	
	Set(new CBusToBusRelation(pEditor->GetDoc(), this , mouse_event.virtpoint, pBusEnt, false , false ) , NULL);
	if(NULL != pEditor)
	{
		IS_NOTIFY notify;
		notify.hdr.hwndFrom = pEditor->GetSafeHwnd();
		notify.pEntFrom = GetEntity();
		pEditor->SendMessage(WM_NOTIFY,(WPARAM)IS_START_CREATE_ENTITY,(LPARAM)(&notify));
	}
}

/**
	@brief	

	@author	HumKyung

	@date	2010.12.27

	@param

	@return
*/
void CBusToBusRelationFactory::OnMouseMove(const IS_MOUSE_EVENT& mouse_event)
{
	assert(NULL == mouse_event.pEnt);

	if(NULL == GetEntity()) return;
	CPoint point=mouse_event.virtpoint;
	CBusToBusRelation* pRel=static_cast<CBusToBusRelation*>(GetEntity());
	if(pRel) pRel->DragTemp(point);

	CIsDrawEditor* pEditor=mouse_event.pEditor;
	if(pEditor) pEditor->RedrawWindow();
}

/**
	@brief	

	@author	HumKyung

	@date	2010.12.27

	@param

	@return
*/
void CBusToBusRelationFactory::OnLButtonUp(const IS_MOUSE_EVENT& mouse_event)
{
	assert(NULL == mouse_event.pEnt);
	CPoint point = mouse_event.virtpoint;

	CIsDrawEditor* pEditor=mouse_event.pEditor;
	long nHitCode=DEHT_NONE;
	CEntity* pEnt = static_cast<CEntity*>(pEditor->GetAt(nHitCode,point));

	if((NULL == pEnt) || !(pEnt->IsKindOf(CBusEntity::TypeString())))
	{
		//////////////////////////////////////////////////////////////////////////
		IS_NOTIFY notify;
		notify.hdr.hwndFrom = pEditor->GetSafeHwnd();
		notify.pEntFrom = NULL;
		pEditor->SendMessage(WM_NOTIFY,(WPARAM)IS_START_CREATE_ENTITY,(LPARAM)(&notify));

		delete GetEntity();
		Set(NULL,mouse_event.pEditor);

		notify.hdr.hwndFrom = pEditor->GetSafeHwnd();
		notify.pEntFrom = NULL;
		pEditor->SendMessage(WM_NOTIFY,(WPARAM)IS_ABORT,(LPARAM)(&notify));

		AfxMessageBox(_T("Bus�� ������ �ּ���"));
		pEditor->RedrawWindow();
		return;
	}
	CBusToBusRelation* pRel=static_cast<CBusToBusRelation*>(GetEntity());
	//! check duplicated relation
	if(pRel)
	{
		CIsDrawEntity* pSrcEnt = pRel->GetSrc().pEnt;
		CIsDrawEntity* pDestEnt= pEnt;
		CBusToBusContainer* pContainer = static_cast<CBusToBusContainer*>(pEditor->GetDoc());
		CRelation* pRel = pContainer->FindRelation(pSrcEnt , pDestEnt);
		if(NULL != pRel)
		{
			AfxMessageBox(_T("������ Relation�� �̹� �����Ǿ� �ֽ��ϴ�."));
			return;
		}
	};

	if(pRel) pRel->Finish(point, pEnt);

	//////////////////////////////////////////////////////////////////////////
	IS_NOTIFY notify;
	notify.hdr.hwndFrom = pEditor->GetSafeHwnd();
	notify.pEntFrom = NULL;
	pEditor->SendMessage(WM_NOTIFY,(WPARAM)IS_START_CREATE_ENTITY,(LPARAM)(&notify));

	//////////////////////////////////////////////////////////////////////////
	notify.hdr.hwndFrom = pEditor->GetSafeHwnd();
	notify.pEntFrom = GetEntity();
	pEditor->SendMessage(WM_NOTIFY,(WPARAM)IS_CREATE_ENTITY,(LPARAM)(&notify));

	pEditor->SendToBack(pRel);

	//! align src point to grid
	pRel->BeginDrag(pRel->handlePoint(0), 0);
	pRel->Drag(pRel->handlePoint(0));
	pRel->EndDrag();

	//! align dest point to grid
	pRel->BeginDrag(pRel->handlePoint(1), 1);
	pRel->Drag(pRel->handlePoint(1));
	pRel->EndDrag();

	Set(NULL,mouse_event.pEditor);

	//! set default controller
	CIsDrawSelectionController* pController=static_cast<CIsDrawSelectionController*>(CIsDrawEditor::GetSelectionController());
	if(pEditor) pEditor->SetCurController(pController);	
}