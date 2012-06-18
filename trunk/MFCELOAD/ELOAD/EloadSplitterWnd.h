#pragma once


// CEloadSplitterWnd

class CEloadSplitterWnd : public CSplitterWnd
{
	DECLARE_DYNAMIC(CEloadSplitterWnd)

public:
	CEloadSplitterWnd();
	virtual ~CEloadSplitterWnd();

	void RecalcLayout();
protected:
	DECLARE_MESSAGE_MAP()
};


