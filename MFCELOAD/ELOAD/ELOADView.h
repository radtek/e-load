
// ELOADView.h : CELOADView 클래스의 인터페이스
//


#pragma once
#include <util/ado/ADODB.h>
#include "Observer.h"
#include "GridCtrlEx.h"
#include "MultiSortDlg.h"

class CELOADView : public CView
{
protected: // serialization에서만 만들어집니다.
	CELOADView();
	DECLARE_DYNCREATE(CELOADView)

// 특성입니다.
public:
	CELOADDoc* GetDocument() const;
	CGridCtrlEx   m_wndGridCtrl;
	CMultiSortDlg m_dlgMultiSort;
// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 구현입니다.
public:
	virtual ~CELOADView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
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

#ifndef _DEBUG  // ELOADView.cpp의 디버그 버전
inline CELOADDoc* CELOADView::GetDocument() const
   { return reinterpret_cast<CELOADDoc*>(m_pDocument); }
#endif

