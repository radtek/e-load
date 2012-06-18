
// CableSizingTableChildFrm.h : CCableSizingTableChildFrame 클래스의 인터페이스
//

#pragma once

class CCableSizingTableChildFrame : public CMDIChildWndEx
{
	DECLARE_DYNCREATE(CCableSizingTableChildFrame)
public:
	CCableSizingTableChildFrame();

// 특성입니다.
public:
	CToolBar m_wndToolBar;
// 작업입니다.
public:

// 재정의입니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
	virtual ~CCableSizingTableChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
