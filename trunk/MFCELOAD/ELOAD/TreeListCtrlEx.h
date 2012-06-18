#pragma once

#include "TreeListCtrl.h"

// CTreeListCtrlEx

struct TREELISTINFO : public NMHDR
{
	HTREEITEM hItem;
	int nCol;
	CRect rect;
};

class CTreeListCtrlEx : public CTreeListCtrl
{
	DECLARE_DYNAMIC(CTreeListCtrlEx)

public:
	CTreeListCtrlEx();
	virtual ~CTreeListCtrlEx();

protected:
	DECLARE_MESSAGE_MAP()
private:
	bool m_bDeleteEvent;
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnCableRouteDelete();
	
	int SetDeleteEvent(bool bDeleteEvent = false);
};


