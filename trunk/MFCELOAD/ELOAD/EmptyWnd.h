#pragma once


// CEmptyWnd

class CEmptyWnd : public CWnd
{
	DECLARE_DYNAMIC(CEmptyWnd)

public:
	CEmptyWnd();
	virtual ~CEmptyWnd();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


