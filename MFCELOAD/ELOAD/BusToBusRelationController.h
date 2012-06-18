// TinyUMLAssociationController.h: interface for the CBusToBusRelationController class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <aDrawCtrl/inc/IsDrawEntController.h>

namespace BusToBus
{
class CBusToBusRelationController : public CIsDrawEntController  
{
public:
	virtual void OnLButtonUp(const IS_MOUSE_EVENT& mouse_event);
	virtual void OnMouseMove(const IS_MOUSE_EVENT& mouse_event);
	virtual void OnLButtonDown(const IS_MOUSE_EVENT& mouse_event);
	CBusToBusRelationController();
	virtual ~CBusToBusRelationController();

	IS_DECLARE_CONTROLLER_FUNC()
};
};