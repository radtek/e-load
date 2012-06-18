/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	EnableBuddyButton.h
Owner:	russf@gipsysoft.com
Purpose:	A simple function to allow you to embed a button into an
					edit control

					Demonstrates:
					* Altering the client rectangle of a window you don't have
					  the code for
					* Subclassing the API way
					* 
----------------------------------------------------------------------*/
#ifndef ENABLEBUDDYBUTTON_H
#define ENABLEBUDDYBUTTON_H

#define BBS_LEFT	1
#define BBS_RIGHT	2

extern BOOL EnableBuddyButton( HWND hwndEdit, HWND hwndButton, UINT uStyle );

#endif //ENABLEBUDDYBUTTON_H