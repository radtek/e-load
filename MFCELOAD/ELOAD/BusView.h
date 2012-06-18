#pragma once
#include "Observer.h"

// CBusView view

class CBusView : public CScrollView
{
	DECLARE_DYNCREATE(CBusView)

protected:
	CBusView();           // protected constructor used by dynamic creation
	virtual ~CBusView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
        afx_msg void OnFileSave();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	static int OnEventFromObserver(CObserver* pObserver , CEventData* pEventData);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
private:
	CObserver*	m_pObserver;
public:
	afx_msg void OnBusDiagramOpen();
};


