#include "StdAfx.h"
#include "CommandAutoFillArea.h"
#include "..\ELoadDocData.h"
#include "..\MainFrm.h"
using namespace COMMAND;

CCommandAutoFillArea::CCommandAutoFillArea(void): CEventData("" , CEventData::TYPE_NULL)
{
}

CCommandAutoFillArea::~CCommandAutoFillArea(void)
{
}

/**
	@brief	LOCATION AREA �� �����ϰų� �߰��ϸ� Sort Area�� Area�� �߰��Ѵ�.

	@author KHS	

	@date 2009-06-01 ���� 1:24:41	

	@param	

	@return		
**/
int CCommandAutoFillArea::Execute(const bool& bSetOriginalValue)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if(pFrame)
	{
		pFrame->DisplayArea();
	}

	return ERROR_SUCCESS;
}