/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	EnableBuddyButton.cpp
Owner:	russf@gipsysoft.com
Purpose:	A simple function to allow you to embed a button into an edit control
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "EnableBuddyButton.h"

static LPCTSTR g_szOldProc = _T("BuddyButtonOldProc");
static LPCTSTR g_szData = _T("BuddyButtonData");


class CData
{
public:
	UINT m_uButtonWidth;
	HWND m_hwndButton;
	UINT m_uStyle;
};



static LRESULT CALLBACK SubClassedProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	WNDPROC	WndProc = reinterpret_cast< WNDPROC>( ::GetProp( hwnd, g_szOldProc ) );
	CData *pData = reinterpret_cast< CData * >( ::GetProp( hwnd, g_szData ) );
	ASSERT( WndProc );

	switch( message )
	{
		case WM_NCDESTROY:
		{
			//
			//	Clean up our junk so we don't leak.
			SetWindowLong( hwnd, GWL_WNDPROC, (LONG) WndProc );
			RemoveProp( hwnd, g_szOldProc );
			RemoveProp( hwnd, g_szData );

			delete pData;
		}
		break;

		case WM_NCHITTEST:
			{
				//
				//	We need this so that mouse clicks get through to our button (or whatever) control
				LRESULT lr = CallWindowProc( WndProc, hwnd, message, wParam, lParam );
				if( lr == HTNOWHERE )
				{
					lr = HTTRANSPARENT;
				}
				return lr;
			}
		
		case WM_NCCALCSIZE:
			{
				//
				//	Adjust the client rect accordingly
				LRESULT lr = CallWindowProc( WndProc, hwnd, message, wParam, lParam );
				LPNCCALCSIZE_PARAMS lpnccs = reinterpret_cast<LPNCCALCSIZE_PARAMS>( lParam );
				if( pData->m_uStyle == BBS_RIGHT )
				{
					lpnccs->rgrc[ 0 ].right -= pData->m_uButtonWidth;
				}
				else
				{
					lpnccs->rgrc[ 0 ].left += pData->m_uButtonWidth;
				}
				
				return lr;
			}
			break;


		case WM_SIZE:
			{
				CRect rc;
				::GetClientRect( hwnd, rc );
				if( pData->m_uStyle == BBS_RIGHT )
				{
					rc.left = rc.right;
					rc.right = rc.left + pData->m_uButtonWidth;
				}
				else
				{
					rc.right = rc.left;
					rc.left = rc.left - pData->m_uButtonWidth;
				}
				
				//
				//	Change our coordinates to be client coordinates relative to our parent
				::MapWindowPoints( hwnd, GetParent( hwnd ), (LPPOINT)&rc, 2 );

				//
				//	Move the button but don't adjust it's z-order
				::SetWindowPos( pData->m_hwndButton, NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER );

			}
			break;
	}

	return CallWindowProc( WndProc, hwnd, message, wParam, lParam );
}



BOOL EnableBuddyButton( HWND hwndEdit, HWND hwndButton, UINT uStyle )
{
	//
	//	Quick check to amke sure our parameters are good
	if( uStyle == BBS_LEFT || uStyle == BBS_RIGHT )
	{
		if( IsWindow( hwndEdit ) && IsWindow( hwndButton ) )
		{
			//
			//	Subclass the edit control so we can catch some handy messages
			FARPROC lpfnWndProc = reinterpret_cast<FARPROC>( SetWindowLong( hwndEdit, GWL_WNDPROC, (LONG) SubClassedProc ) );
			ASSERT( lpfnWndProc != NULL );
			VERIFY( ::SetProp( hwndEdit, g_szOldProc, reinterpret_cast<HANDLE>( lpfnWndProc ) ) );

			//
			//	Create our data object. We later give this to our subclassed edit control so we can 
			CData *pData = new CData;
			pData->m_uStyle = uStyle;

			CRect rcButton;
			::GetWindowRect( hwndButton, rcButton );

			pData->m_uButtonWidth = rcButton.Width();
			pData->m_hwndButton = hwndButton;

			VERIFY( ::SetProp( hwndEdit, g_szData, reinterpret_cast<HANDLE>( pData ) ) );

			//
			//	Doing this forces our edit window to pay attention to our change in it's client size
			SetWindowPos( hwndEdit, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED );

			return TRUE;
		}
		else
		{
			//
			//	Set the last error to be something meaningful
			SetLastErrorEx( ERROR_INVALID_WINDOW_HANDLE, SLE_ERROR );
		}
	}
	else
	{
		//
		//	Set the last error to be something meaningful
		SetLastErrorEx( ERROR_INVALID_DATA, SLE_ERROR );
	}
	return FALSE;
}




