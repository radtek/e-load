
// ELOADView.h : CELOADView Ŭ������ �������̽�
//


#pragma once
#include <util/ado/ADODB.h>
#include "Observer.h"
#include "GridCtrlEx.h"
#include "MultiSortDlg.h"

class CELOADView : public CView
{
protected: // serialization������ ��������ϴ�.
	CELOADView();
	DECLARE_DYNCREATE(CELOADView)

// Ư���Դϴ�.
public:
	CELOADDoc* GetDocument() const;
	CGridCtrlEx   m_wndGridCtrl;
	CMultiSortDlg m_dlgMultiSort;
// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// �����Դϴ�.
public:
	virtual ~CELOADView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnInitialUpdate();
	afx_msg void OnFileSave();
        static int OnEventFromObserver(CObserver* pObserver , CEventData* pEventData);
private:
        CObserver* m_pObserver;
        int WriteELoadItemsToGrid();
        bool m_bHeaderSort , m_bBlockMoveNext;
protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
        virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
        afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
        afx_msg void OnEditPaste();
        afx_msg void OnEditCopy();
        void OnHeaderSort();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	int UpdateELoadItems(CItemSelectionSet& ItemSelectionSet);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	CString GetCurSelColumnName(int& nColNo);
	int OnMultiSort(void);
};

#ifndef _DEBUG  // ELOADView.cpp�� ����� ����
inline CELOADDoc* CELOADView::GetDocument() const
   { return reinterpret_cast<CELOADDoc*>(m_pDocument); }
#endif

