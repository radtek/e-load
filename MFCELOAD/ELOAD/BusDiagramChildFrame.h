#pragma once

#ifdef _WIN32_WCE
#error "CMDIChildWnd is not supported for Windows CE."
#endif 

// CBusToBusChildFrame frame with splitter

class CBusDiagramChildFrame : public CMDIChildWndEx
{
	DECLARE_DYNCREATE(CBusDiagramChildFrame)
protected:
	CBusDiagramChildFrame();           // protected constructor used by dynamic creation
	virtual ~CBusDiagramChildFrame();

	///CToolBar m_wndDrawToolBar;
	CSplitterWnd m_wndSplitter ;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
};
