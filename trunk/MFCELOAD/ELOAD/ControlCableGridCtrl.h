#pragma once

#include <gui/gridctrl/GridCtrl.h>

// CControlCableGridCtrl

class CControlCableGridCtrl : public CGridCtrl
{
	DECLARE_DYNAMIC(CControlCableGridCtrl)
        friend class CGridCellBaseEx;

public:
	CControlCableGridCtrl()
	{};
	virtual~CControlCableGridCtrl(){};

protected:
	DECLARE_MESSAGE_MAP()
public:
        afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnControlcableInsert();
	afx_msg void OnControlCableDelete();
public:
	CCellID m_MouseClickCell;

	afx_msg void OnEnterIdle(UINT nWhy, CWnd* pWho);
	BOOL FindString(const int& nCol, const CString& rString);
};