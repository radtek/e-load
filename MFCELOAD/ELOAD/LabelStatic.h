#if !defined(AFX_LABELSTATIC_H__A4EABEC5_2E8C_11D1_B79F_00805F9ECE10__INCLUDED_)
#define AFX_LABELSTATIC_H__A4EABEC5_2E8C_11D1_B79F_00805F9ECE10__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Label.h : header file
//

#define	NM_LINKCLICK	WM_APP + 0x200

/////////////////////////////////////////////////////////////////////////////
// CLabelStatic window

class CLabelStatic : public CStatic
{
// Construction
public:


	static enum LinkStyle { LinkNone, HyperLink, MailLink };
	static enum FlashType {None, Text, Background };
	static enum Type3D { Raised, Sunken};
	static enum BackFillMode { Normal, HorzGradient , VertGradient};

	CLabelStatic();
	virtual CLabelStatic& SetBkColor(COLORREF crBkgnd, COLORREF crBkgndHigh = 0, BackFillMode mode = Normal);
	virtual CLabelStatic& SetTextColor(COLORREF crText);
	virtual CLabelStatic& SetText(const CString& strText);
	virtual CLabelStatic& SetFontBold(BOOL bBold);
	virtual CLabelStatic& SetFontName(const CString& strFont, BYTE byCharSet = ANSI_CHARSET);
	virtual CLabelStatic& SetFontUnderline(BOOL bSet);
	virtual CLabelStatic& SetFontItalic(BOOL bSet);
	virtual CLabelStatic& SetFontSize(int nSize);
	virtual CLabelStatic& SetSunken(BOOL bSet);
	virtual CLabelStatic& SetBorder(BOOL bSet);
	virtual CLabelStatic& SetTransparent(BOOL bSet);
	virtual CLabelStatic& FlashText(BOOL bActivate);
	virtual CLabelStatic& FlashBackground(BOOL bActivate);
	virtual CLabelStatic& SetLink(BOOL bLink,BOOL bNotifyParent);
	virtual CLabelStatic& SetLinkCursor(HCURSOR hCursor);
	virtual CLabelStatic& SetFont3D(BOOL bSet,Type3D type=Raised);
	virtual CLabelStatic& SetRotationAngle(UINT nAngle,BOOL bRotation);
	virtual CLabelStatic& SetText3DHiliteColor(COLORREF cr3DHiliteColor);
	virtual CLabelStatic& SetFont(LOGFONT lf);
	virtual CLabelStatic& SetMailLink(BOOL bEnable, BOOL bNotifyParent);
	virtual CLabelStatic& SetHyperLink(const CString& sLink);

// Attributes
public:
protected:
	void UpdateSurface();
	void ReconstructFont();
	void DrawGradientFill(CDC* pDC, CRect* pRect, COLORREF crStart, COLORREF crEnd, int nSegments);
	COLORREF		m_crText;
	COLORREF		m_cr3DHiliteColor;
	HBRUSH			m_hwndBrush;
	HBRUSH			m_hBackBrush;
	LOGFONT			m_lf;
	CFont			m_font;
	BOOL			m_bState;
	BOOL			m_bTimer;
	LinkStyle		m_Link;
	BOOL			m_bTransparent;
	BOOL			m_bFont3d;
	BOOL			m_bToolTips;
	BOOL			m_bNotifyParent;
	BOOL			m_bRotation;
	FlashType		m_Type;
	HCURSOR			m_hCursor;
	Type3D			m_3dType;
	BackFillMode	m_fillmode;
	COLORREF		m_crHiColor;
	COLORREF		m_crLoColor;
	CString			m_sLink;

	// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLabelStatic)
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLabelStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLabelStatic)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSysColorChange();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LABEL_H__A4EABEC5_2E8C_11D1_B79F_00805F9ECE10__INCLUDED_)
