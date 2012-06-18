// BusToBusRelationController.h: interface for the CBusToBusRelationFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TINYUMLASSOCIATIONCONTROLLER_H__44627728_0031_4BC1_91F8_23AA820EBA90__INCLUDED_)
#define AFX_TINYUMLASSOCIATIONCONTROLLER_H__44627728_0031_4BC1_91F8_23AA820EBA90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <aDrawCtrl/inc/IsDrawEntController.h>

namespace BusToBus
{
class CBusToBusRelationFactory : public CIsDrawEntController  
{
public:
	virtual void OnLButtonUp(const IS_MOUSE_EVENT& mouse_event);
	virtual void OnMouseMove(const IS_MOUSE_EVENT& mouse_event);
	virtual void OnLButtonDown(const IS_MOUSE_EVENT& mouse_event);
	CBusToBusRelationFactory();
	virtual ~CBusToBusRelationFactory();

	IS_DECLARE_CONTROLLER_FUNC()
};
};

#endif // !defined(AFX_TINYUMLASSOCIATIONCONTROLLER_H__44627728_0031_4BC1_91F8_23AA820EBA90__INCLUDED_)
