
// CableSizingTableChildFrm.h : CCableSizingTableChildFrame Ŭ������ �������̽�
//

#pragma once

class CCableSizingTableChildFrame : public CMDIChildWndEx
{
	DECLARE_DYNCREATE(CCableSizingTableChildFrame)
public:
	CCableSizingTableChildFrame();

// Ư���Դϴ�.
public:
	CToolBar m_wndToolBar;
// �۾��Դϴ�.
public:

// �������Դϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// �����Դϴ�.
public:
	virtual ~CCableSizingTableChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
