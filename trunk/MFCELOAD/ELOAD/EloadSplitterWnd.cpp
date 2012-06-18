// EloadSplitterWnd.cpp : implementation file
//

#include "stdafx.h"
#include "ELoad.h"
#include "EloadSplitterWnd.h"
#include "ELoadItemViewBar.h"

// CEloadSplitterWnd

IMPLEMENT_DYNAMIC(CEloadSplitterWnd, CSplitterWnd)

CEloadSplitterWnd::CEloadSplitterWnd()
{

}

CEloadSplitterWnd::~CEloadSplitterWnd()
{
}


BEGIN_MESSAGE_MAP(CEloadSplitterWnd, CSplitterWnd)
END_MESSAGE_MAP()



// CEloadSplitterWnd message handlers


void CEloadSplitterWnd::RecalcLayout()
{
	CSplitterWnd::RecalcLayout();

	CELoadItemViewBar* pParent = (CELoadItemViewBar*)GetParent();
	if(pParent) pParent->OnChangeLayoutSplitterWnd();
}