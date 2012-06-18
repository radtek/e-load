// ELoadMenuBar.cpp : implementation file
//

#include "stdafx.h"
#include "ELoad.h"
#include "ELoadMenuBar.h"
#include "ELoadDocData.h"

// CELoadMenuBar

CELoadMenuBar::CELoadMenuBar()
{

}

CELoadMenuBar::~CELoadMenuBar()
{
}

// CELoadMenuBar message handlers

BOOL CELoadMenuBar::RestoreOriginalstate ()
{
	if (!CMFCMenuBar::RestoreOriginalstate ())
	{
		return FALSE;
	}

	CELoadDocData& docData = CELoadDocData::GetInstance();
	if("ADMIN" == docData.GetUserID())
	{
		AddSubMenu ();
	}

	return TRUE;
}

/**
	@brief	사용자 정의 하위 메뉴를 추가한다.

	@author	BHK
*/
int CELoadMenuBar::AddSubMenu(void)
{
	/*CMenu menu;
	menu.LoadMenu (IDR_ADMIN_MENU); 

	CMFCToolBarButton(
	InsertButton (CMFCToolBarButton (IDR_ADMIN_MENU , -1, _T("&Admin")));
	CMFCMenuBar::InsertPane(,,);


	AdjustLayout();
	AdjustSizeImmediate ();*/

	return ERROR_SUCCESS;
}
