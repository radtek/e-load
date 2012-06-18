#pragma once

#include "EditTreeCtrl.h"
#include "BusItem.h"
#include "Observer.h"
#include "ClassTreeWnd.h"


// CBusEditTreeCtrl

class CBusEditTreeCtrl : public CEditTreeCtrl
{
	DECLARE_DYNAMIC(CBusEditTreeCtrl)

public:
        CBusEditTreeCtrl();
	virtual ~CBusEditTreeCtrl();

        static int OnEventFromObserver(CObserver* pObserver , CEventData* pEventData);
protected:
        afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
        afx_msg void OnCreateBus();
	DECLARE_MESSAGE_MAP()

	bool CanDragItem(TVITEM & item);
	bool CanDropItem(HTREEITEM hDrag, HTREEITEM hDrop, EDropHint hint);
        void DragEnd();
private:
        HTREEITEM FindBusGroupItem( CBusGroupItem* pBusGroupItem , HTREEITEM hItem );
private:
        CObserver*      m_pObserver;
        bool bLbuttonClick;

        HTREEITEM m_hNotAssignedItem;
public:
        HTREEITEM AddBusGroupTreeCtrlItem( CBusGroupItem* pBusGroupItem );
        int AddBusTreeCtrlItem( CBusItem* pBusItem );
        int UpdateBusTreeView(HTREEITEM hItem , CELoadItem* pItem );

        afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
        afx_msg void OnCopyBusGroup();
        afx_msg void OnCopyBus();
        afx_msg void OnDeleteBusGroup();
        afx_msg void OnDeleteBus();
        afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
        void DeleteKeyDown(void);
        afx_msg void OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
        afx_msg void OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
        afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

        virtual BOOL PreTranslateMessage(MSG* pMsg);
        int SetOrderNo(void);

private:
        CString m_rPreBusItemName;
        int SelectedItemPropView(void);

	string m_rActionString;
public:
	int SetActionString(const string& rActionString);
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
};


