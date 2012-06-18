//*****************************************************************************
//*
//*
//*		TreeListWnd.cpp
//*
//*
//*****************************************************************************
//
//	Dieser Code erstellt ein Tree-Fenster mit einer Liste
//
//
//	Copyright (C) Anton Zechner 2007
//
//	TreeListWnd is distributed under the GNU GENERAL PUBLIC LICENSE (GPL)
//	Sourcecode which use TreeListWnd must be published. Commercial users
//	must published their code too, or make an licence agreement with me.
//
//
//	TreeListWnd wird unter GNU GENERAL PUBLIC LICENSE (GPL) vertreiben.
//	Sourcecode welcher TreeListWnd verwendet muss ver?fendlicht werden.
//	Komerzielle Nutzer m?sen ihren Code ebenfalls ver?fentlichen, oder
//	eine Nutzungsvereinbarung mit mir treffen.
//
//	az_software@inode.at
//
//
#ifdef		UNICODE
#ifndef	   _UNICODE
#define    _UNICODE
#endif
#endif

#include	<windows.h>
#include	<string.h>
#include	<tchar.h>
#include <StdAfx.h>
#include	"TreeListWnd.h"

#ifdef	   _MSC_VER
#pragma		warning( disable :4996 )
#endif

#ifndef	   _WIN64
#ifndef		SetWindowLongPtr
#define		SetWindowLongPtr	SetWindowLong
#endif
#ifndef		GetWindowLongPtr
#define		GetWindowLongPtr	GetWindowLong
#endif
#ifndef		DWORD_PTR
#define		DWORD_PTR			DWORD
#endif
#ifndef		LONG_PTR
#define		LONG_PTR			LONG
#endif
#endif
#ifdef		UNICODE
#define 	str_len				wcslen
#define 	str_cmp				wcscmp
#define 	str_ncpy			wcsncpy
#define 	str_ncmp			wcsncmp
#define 	str_icmp			wcsicmp
#else
#define 	str_len				strlen
#define 	str_cmp				strcmp
#define 	str_ncpy			strncpy
#define 	str_ncmp			strncmp
#define 	str_icmp			stricmp
#endif

#ifndef		WM_MOUSEWHEEL
#define		WM_MOUSEWHEEL		0x020A
#endif
#ifndef		WHEEL_DELTA
#define		WHEEL_DELTA			120
#endif


#define 	MAX_COLUMNS			32
#define		MAX_COLORS			12
#define 	VK_DBLCLK			0xFEDC
#define 	VK_ICONCLK			0xFEDB
#define 	EN_RETURN			0x1578
#define 	EN_ESCAPE			0x1579
#define 	ID_TOOLTIPCHECK		0x3912
#define 	SORT_NOUPDATE		1234567
#define 	FIRST_LINE			0xFFFFFFFE
#define 	I_CCB				I_CHILDRENCALLBACK
#define 	U(h)				((unsigned)(h))
#define 	GetHandle(h)		((TreeListData*)GetWindowLongPtr(h,0))
#define 	TVIF_ALL			(TVIF_CHILDREN|TVIF_HANDLE|TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_STATE|TVIF_TEXT)
#define 	UNLOCK(d)			ReleaseSemaphore(d->hSem,1,NULL)
#define 	LOCK(d)				WaitForSingleObject(d->hSem,INFINITE)
#define 	TVIS_BASEFLAGS		(TVIS_EXPANDED|TVIS_EXPANDEDONCE|TVIS_EXPANDPARTIAL|TVIS_SELECTED)
#define 	TVIS_TRACKED		(TVIX_TRACKED<<16)
#define 	TVIS_BKCOLOR		(TVIX_BKCOLOR<<16)
#define 	TVIS_FOCUSED		(TVIX_FOCUSED<<16)
#define 	TVIS_TEXTCOLOR		(TVIX_TEXTCOLOR<<16)
#define 	TVC_DESELECT		0x8000
#define 	DEFAULT_IDENT		19
#define 	DEFAULT_SHIFT		7

#define 	TVAX_NONE			(TVAE_NONE   >>TVAE_MODEPOS)// Kein atomatisches editieren
#define 	TVAX_EDIT			(TVAE_EDIT	 >>TVAE_MODEPOS)// Atomatisches editieren mit Edit-Fenster
#define 	TVAX_COMBO			(TVAE_COMBO	 >>TVAE_MODEPOS)// Atomatisches editieren mit ComboBox
#define 	TVAX_CBLIST			(TVAE_CBLIST >>TVAE_MODEPOS)// Atomatisches editieren mit ComboListBox
#define 	TVAX_STEP			(TVAE_STEP	 >>TVAE_MODEPOS)// Einzelnes Weiterschalten mit Enter
#define 	TVAX_STEPED			(TVAE_STEPED >>TVAE_MODEPOS)// Einzelnes Weiterschalten mit Enter und Edit
#define 	TVAX_CHECK			(TVAE_CHECK	 >>TVAE_MODEPOS)// Atomatisches editieren mit CheckBox
#define 	TVAX_CHECKED		(TVAE_CHECKED>>TVAE_MODEPOS)// Atomatisches editieren mit CheckBox und Edit

#define 	TVIX_VARBUTTON		0x01						// Schaltfl?hen sind nicht fix eingestellt
#define 	TVIX_HASBUTTON		0x02						// Der Eintrag hat eine Schaltfl?he
#define 	TVIX_HASIMAGE		0x04						// Der Eintrag hat ein Icon
#define 	TVIX_TRACKED		0x08						// Der Eintrag unter dem Cursor
#define 	TVIX_TEXTCOLOR		0x10						// Der Eintrag hat eine eigene Textfarbe
#define 	TVIX_BKCOLOR		0x20						// Der Eintrag hat eine eigene Hintergrundfarbe
#define 	TVIX_FOCUSED		0x40						// Der Eintrag hat einen Eingabe-Focus


typedef struct
{
	LPARAM		lParam;										// Ist der LPARAM Wert f? den Eintrag
	LPTSTR		pText;										// Zeiger auf Tree-Text
	UINT		uState;										// Zustand des Eintrages
	int			iImage;										// Ist die Nummer des an zu zeigenden Icons
	int			iSelectedImage; 							// Ist die Nummer des an zu zeigenden Icons (wenn ausgew?lt)
	unsigned	uShowPos;									// Ist die Position in der Sichtbarliste (0=unsichtbar)
	unsigned	uFirstChild;								// Ist die Nummer des ersten Kind-Eintrages (0=keines)
	unsigned	uLastChild;									// Ist die Nummer des letzen Kind-Eintrages (0=keines)
	unsigned	uPrevItem;									// Ist die Nummer des vorherigen Eintrages (0=keines)
	unsigned	uNextItem; 									// Ist die Nummer des n?hsten Eintrages (0=keines)
	unsigned	uParent; 									// Ist die Nummer des Elterneintrages (0=Root)
	unsigned 	uLevel;										// Ist die Ebene des Eintrages (0=Root)
	int 		iTextPixels;								// Ist die Breites des Textes in Pixel
	WORD		uTextSize;									// L?ge des Textes in Zeichen
	BYTE		bCallback;									// Sind Bits f? Callbacks
	BYTE		bFlags;										// Diverse Flags
	COLORREF	uColorText;									// Spezielle Textfarbe
	COLORREF	uColorBk;									// Spezielle Hintergrundfarbe
}BaseItem;

typedef struct
{
	LPTSTR		pText;										// Zeiger auf Tree-Text
	UINT		uState;										// Zustand des Eintrages
	int			iImage;										// Ist die Nummer des an zu zeigenden Icons
	int 		iTextPixels;								// Ist die Breites des Textes in Pixel
	WORD		uTextSize;									// L?ge des Textes in Zeichen
	BYTE		bCallback;									// Sind Bits f? Callbacks
	BYTE		bFlags;										// Diverse Flags
	COLORREF	uColorText;									// Spezielle Textfarbe
	COLORREF	uColorBk;									// Spezielle Hintergrundfarbe
}ExtraItem;

typedef struct
{
	HWND		hWnd;										// Fenster Handle
	HANDLE		hSem;										// Zugriffssemaphore
	HIMAGELIST	hStates;									// Handle der Icon-Liste f? States und Overlay
	HIMAGELIST	hImages;									// Handle der Icon-Liste
	HIMAGELIST	hChecks;									// Handle der Icon-Liste
	HFONT		hFontN;										// Ist der normale Zeichensatz
	HFONT		hFontB;										// Ist der fette Zeichensatz
	HFONT		hFontL;										// Der letzte zugewiesene Font
	HFONT		hFontT;										// Ist der Font f? das Tooltip
	HWND		hEdit;										// Handle des Edit-Fensters
	HWND		hHeader;									// Handle des Header Fensters
	HWND		hToolTip;									// Handle des Tooltip-Fensters
	COLORREF	uColors[MAX_COLORS];						// 0=Hintergrundfarbe  1=Abwechselnte Farbe  2=Farbe f? Trennlinien  3=Textfarbe
	int			iFontHeight;								// Ist die H?e des Fonts
	int			iFontLine;									// Ist die Position der Linie beim unterstreichen
	int			iFontOff;									// Ist die Position um der ein Text horizontal verschoben wird
	int			iStatesMode;								// Die hStates Image-Liste wurde f? die Checkboxen erzeugt
	int			iStatesXsize;								// Breite der States und Overlay Icons
	int			iStatesYsize;								// H?e   der States und Overlay Icons
	int			iChecksMode;								// Die hChecks Image-Liste wurde f? die Checkboxen erzeugt
	int			iChecksXsize;								// Breite der States und Overlay Icons
	int			iChecksYsize;								// H?e   der States und Overlay Icons
	int			iImagesXsize;								// Breite der Icons
	int			iImagesYsize;								// H?e   der Icons
	int			iRowHeight;									// Ist die H?e einer
	int			iIndent;									// Einr?kung der Kindereint?e
	int			iShift;										// Einr?kung der vertikalen Linien
	int			iMaxSizeX;									// Die Größe des breitesten sichtbaren Eintrages
	unsigned	uItemPosCount;								// Anzahl der sichtbaren Eintr?e
	unsigned   *pItemPos;									// Liste mit den Offsets der sichtbaren Eintr?e
	BaseItem  **pTreeItems;									// Zeiger auf Item Zeiger
	ExtraItem **pExtraItems[MAX_COLUMNS-1];					// Zeiger auf die Spalteneintr?e
	unsigned	uTreeItemsMax;								// Größe der Liste mit den vorhanden Eintr?e (alociert um 1 größer)
	unsigned	uTreeItemsCount;							// Anzahl der vorhanden Eintr?e
	unsigned	uNextSeachPos;								// N?hste Position zum suchen von freien Eintr?en
	unsigned	uUserDataSize;								// Ist die Größe der Userdaten in einem Eintrag
	unsigned	uFirstChild;								// Ist die Nummer des ersten Kind-Eintrages (0=keines)
	unsigned	uLastChild;									// Ist die Nummer des letzen Kind-Eintrages (0=keines)
	unsigned	uScrollY;									// Aktuelle X-Scroll-Position
	unsigned	uScrollX;									// Aktuelle Y-Scroll-Position
	unsigned	uSizeX;										// Aktuelle X-Fenster-Größe
	unsigned	uSizeY;										// Aktuelle Y-Fenster-Größe
	unsigned	uSizeYsub;									// Aktuelle Y-Fenster-Größe ohne Header
	unsigned	uStyle;										// Ist der aktuele Style des Fensters
	unsigned	uStyleEx;									// Erweiterte Sytle-Flags (siehe TVS_EX_???)
	unsigned	uStartPixel;								// Ist die Y-Koortinate bei der der erste Eintrag beginnt
	unsigned	uMaxEnties;									// Anzahl der sichtbaren Eintr?e (inkl. halbsichtbare)
	unsigned	uPageEnties;								// Anzahl der sichtbaren Eintr?e (ohne  halbsichtbare)
	unsigned	uColumnCount;								// Anzahl der Spalten
	unsigned	uColumnCountVar;							// Anzahl der variabeln Spalten
	unsigned	uSelectedCount;								// Anzahl der ausgew?lten Eintr?e
	unsigned	uSelectedBase;								// Ist der Eintrag ab dem gew?lt wurde
	unsigned	uSelectedItem;								// Ist der Eintrag der gerade gew?lt ist
	unsigned	uSelectedSub;								// Ist die Spalte die gerade gew?lt ist
	unsigned	uFocusItem;									// Ist der Eintrag der einen leeren Focus hat
	unsigned	uFocusSub;									// Ist die Spalte die einen leeren Focus hat
	unsigned	uToolTipItem;								// Ist der ToolTip-Eintrag der gerade gew?lt ist
	unsigned	uToolTipShow;								// Ist die Zeitverz?erung in 500 ms Schritten f? das Tooltip
	unsigned	uToolTipSub;								// Ist die ToolTip-Spalte die gerade gew?lt ist
	POINT		sToolTipPos;								// Ist die globale Koordinate des ToolTips
	unsigned	uEditMode;									// Ist der Modus des Editfensters (0=Edit 1=ComboBox 2=ComboBox fix)
	unsigned	uEditItem;									// Ist der Eintrag der gerade editiert wird
	unsigned	uEditSub;									// Ist die Spalte die gerade editiert wird
	unsigned	uOldXPage;									// Alte Werte f? X-Scroll-Bar
	unsigned	uOldXCount;									//			*
	unsigned	uOldYPage;									// Alte Werte f? Y-Scroll-Bar
	unsigned	uOldYCount;									//			*
	unsigned	uTrippleB;									// Bereite des "..." Strings f? den fetten Fonts
	unsigned	uTrippleN;									// Bereite des "..." Strings f? den normalen Fonts
	unsigned	uTrackedItem;								// Ist der Eintrag der unterstrichen werden soll
	unsigned	uTrackedSub;								// Ist die Spalte des Eintrages der unterstrichen werden soll
	unsigned	uInsertMark;								// Ist der Eintrag mit der Einf?emarke
	unsigned	uDragFlags;									// Welche Maustasten sind an
	unsigned	uDragItem;									// Eintrag f? Dragoperation
	unsigned	uDragSub;									// Untereintrag f? Dragoperation
	TCHAR		cTempText1   [260];							// Erster Textpuffer f? Callbacks
	TCHAR		cTempText2   [260];							// Zeiter Textpuffer f? Callbacks
	TCHAR		cToolTipText [260];							// Textspeicher f? Tooltip
	BYTE  	    bColumnEdit  [MAX_COLUMNS];					// Automaisches Editiern einer Spalte (siehe TVAE_???>>7)
	BYTE  	    bColumnFlags [MAX_COLUMNS];					// Automaisches Editiern einer Spalte (siehe TVAE_???)
	BYTE  	    bColumnEnable[MAX_COLUMNS];					// Automaisches einer mit Statebits aktivieren 
	BYTE  	    bColumnCbSize[MAX_COLUMNS];					// Maximale Anzahl der Eintr?e in der Datenliste
	BYTE  	    bColumnCbChar[MAX_COLUMNS];					// Trennzeichen f? die Datenliste
	INT  	    iColumnCbIcon[MAX_COLUMNS];					// Startoffset f? in Iconliste f? Autoedit
	void 	   *pColumnCbData[MAX_COLUMNS];					// Daten f? das Autoeditieren
	int			iColumnXpos  [MAX_COLUMNS+2];				// Array mit den Positionen der Spalten
	char		cColumnVar   [MAX_COLUMNS];					// Markiert jene Spalten die eine variable Breite haben
	char		cColumnAlign [MAX_COLUMNS];					// Ist die Ausrichtung einer Spalte
	char		cColorChanged[MAX_COLORS ];					// Welche Farbe wurden ver?dert
	char 		cFixedHeight;								// Ist eine fixe H?e eingestellt
	char		cLockChanges;								// Sperren von Fenster?derungen
	char		cHasRootRow;								// Wird gesetz wenn eine Root-Spalte eingef?t wird
	char		cHasFocus;									// Hat das Fenster den Focus
	char		cIsEnabled;									// Ist das Fenster freigegeben
	char		cEditCb;									// Muss das Edit-Fenster einen Callback aufrufen
	char		cClickFlag;									// Merker f? LBUTTON-DOWN bei Multiselct
	char		cReSelect;									// Soll die Auswahl neu Selektiert werden
}TreeListData;

static LONG				lWindowCount  = -1;
static HPEN				hPatternPen   = NULL;
static HFONT			hDefaultFontN = NULL;
static HFONT			hDefaultFontB = NULL;
static RECT				sToolRect={-2,0,2,64};
static void				TreeListDraw(HWND hWnd,HDC hDc,RECT *pRect);
static LRESULT CALLBACK TreeListProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
static int				TreeListSelectItem   (TreeListData *pData,unsigned uItem,unsigned uSubItem,int iMode);
static int				TreeListGetItemRect  (TreeListData *pData,unsigned uItem,unsigned uFlags,RECT *pRect);
static int				TreeListStartAutoEdit(TreeListData *pData,unsigned uColumn,WPARAM wParam,LPARAM lParam);
static int				TreeListEndLabelEdit (TreeListData *pData,int iMode);

//*****************************************************************************
//*
//*		TreeListRegister
//*
//*****************************************************************************
//	Registiert das TreeList Fenster.
//	Ergibt 1 wenn das Fenster erfolgreich registiert wurde.
int TreeListRegister()
{
	static int 		iIsRegistered=FALSE;
	WNDCLASSEX		sClass;


	if(iIsRegistered)return TRUE;



	memset(&sClass,0,sizeof(sClass));

	sClass.cbSize		= sizeof(sClass);
	sClass.style		= CS_DBLCLKS;
	sClass.lpfnWndProc	= TreeListProc;
	sClass.cbClsExtra	= 0;
	sClass.cbWndExtra	= sizeof(TreeListData*);
	sClass.hInstance	= NULL;
	sClass.hIcon		= NULL;
	sClass.hCursor		= LoadCursor(NULL,IDC_ARROW);
	sClass.hbrBackground= NULL;
	sClass.lpszMenuName	= NULL;
	sClass.lpszClassName= _T("TreeList");
	sClass.hIconSm		= NULL;


	if(!RegisterClassEx(&sClass))return 0;

	iIsRegistered = TRUE;


	return TRUE;
}


//*****************************************************************************
//*
//*		GlobalInit
//*
//*****************************************************************************
static void GlobalInit()
{
	LOGBRUSH	sLog;
	long		lCount;



	lCount = InterlockedIncrement(&lWindowCount);
	if(lCount>0)return;

	sLog.lbColor = GetSysColor(COLOR_BTNSHADOW);
	sLog.lbStyle = PS_SOLID;
	sLog.lbHatch = 0;
	hPatternPen  = ExtCreatePen(PS_COSMETIC|PS_ALTERNATE,1,&sLog,0,NULL);

	if(!hPatternPen)
	{
		hPatternPen = CreatePen(PS_DOT,1,RGB(0,0,0));
	}

}


//*****************************************************************************
//*
//*		GlobalDeinit
//*
//*****************************************************************************
static void GlobalDeinit()
{
	int	lCount;


	lCount=InterlockedDecrement(&lWindowCount);
	if(lCount>=0)return;

	if(hDefaultFontN)
	{
		DeleteObject(hDefaultFontN);
		hDefaultFontN = NULL;
	}


	if(hDefaultFontB)
	{
		DeleteObject(hDefaultFontB);
		hDefaultFontB = NULL;
	}

	if(hPatternPen)
	{
		DeleteObject(hPatternPen);
		hPatternPen = NULL;
	}


}


//*****************************************************************************
//*
//*		SendNotify
//*
//*****************************************************************************
//	Sendet eine WM_NOTIFY Nachricht and das Elternfenster
//	pData		: Zeiger auf die Fensterdaten
//	pNotify		: Zeiger auf die Notify-Daten
//	Ergibt den R?kgabewert der WM_NOTIFY Nachrich
static LRESULT SendNotify(TreeListData *pData,NMHDR	*pNotify)
{


	pNotify->hwndFrom	= pData->hWnd;
	pNotify->idFrom		= GetWindowLong(pNotify->hwndFrom,GWL_ID);


	return SendMessage(GetParent(pNotify->hwndFrom),WM_NOTIFY,pNotify->idFrom,(LPARAM)pNotify);
}


//*****************************************************************************
//*
//*		CallbackEntry
//*
//*****************************************************************************
//	Sendet eine WM_NOTIFY Nachricht and das Elternfenster um Daten zuholen
//	pData		: Zeiger auf die Fensterdaten
//	pEntry		: Zeiger auf den Eintrag
//	uItem		: Nummer des Eintrages
//	uFlags		: Welche Daten sollen abgefragt werden
//	Ergibt den R?kgabewert der WM_NOTIFY Nachrich
static void CallbackEntry(TreeListData *pData,BaseItem *pEntry,unsigned uItem,unsigned uFlags,int &iImage,unsigned &uTextSize,LPCTSTR &pText)
{
	NMTVDISPINFO	sInfo;


	sInfo.item.mask				= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|uFlags;
	sInfo.item.lParam			= pEntry->lParam;
	sInfo.item.hItem			= (HTREEITEM)uItem;
	sInfo.item.state			= pEntry->uState;
	sInfo.item.stateMask		= 0xFFFFFFFF;
	sInfo.item.iImage			= I_IMAGECALLBACK;
	sInfo.item.iSelectedImage	= I_IMAGECALLBACK;
	sInfo.item.cChildren		= I_CHILDRENCALLBACK;

	if(uFlags&TVIF_TEXT)
	{
		if(uTextSize)
		{
			pData->cTempText2[sizeof(pData->cTempText2)/sizeof(TCHAR)-1]	= 0;
			pData->cTempText2[0]	= 0;
			sInfo.item.pszText		= pData->cTempText2;
			sInfo.item.cchTextMax	= sizeof(pData->cTempText2)/sizeof(TCHAR)-1;
		}
		else{
			pData->cTempText1[sizeof(pData->cTempText1)/sizeof(TCHAR)-1]	= 0;
			pData->cTempText1[0]	= 0;
			sInfo.item.pszText		= pData->cTempText1;
			sInfo.item.cchTextMax	= sizeof(pData->cTempText1)/sizeof(TCHAR)-1;
		}
	}
	else{
		sInfo.item.pszText		= 0;
		sInfo.item.cchTextMax	= 0;
	}

	sInfo.hdr.hwndFrom	= pData->hWnd;
	sInfo.hdr.idFrom	= GetWindowLong(pData->hWnd,GWL_ID);
	sInfo.hdr.code		= TVN_GETDISPINFO;

	UNLOCK(pData);
	SendMessage(GetParent(sInfo.hdr.hwndFrom),WM_NOTIFY,sInfo.hdr.idFrom,(LPARAM)&sInfo);
	LOCK(pData);


	if(uFlags&TVIF_IMAGE)
	{
		if(!(pEntry->uState&TVIS_SELECTED))iImage = sInfo.item.iImage;
	}
	else if(uFlags&TVIF_SELECTEDIMAGE)
	{
		if(pEntry->uState&TVIS_SELECTED)iImage = sInfo.item.iSelectedImage;
	}

	if(uFlags&TVIF_CHILDREN)
	{
		switch(sInfo.item.cChildren)
		{
		case  0:		pEntry->bFlags    &= ~TVIX_HASBUTTON;
			pEntry->bFlags    &=  TVIX_VARBUTTON;
			break;

		case  1:		pEntry->bFlags    &=  TVIX_VARBUTTON;
			pEntry->bFlags    |=  TVIX_HASBUTTON;
			break;

		default:		pEntry->bFlags    |=  TVIX_VARBUTTON;

			if(pEntry->uFirstChild)
				pEntry->bFlags |=  TVIX_HASBUTTON;
			else	pEntry->bFlags &= ~TVIX_HASBUTTON;
		}
	}

	if(uFlags&TVIF_TEXT)
	{
		pText		= sInfo.item.pszText;
		uTextSize	= str_len(sInfo.item.pszText);
		pEntry->iTextPixels = 0;
	}

}

//*****************************************************************************
//*
//*		CallbackExtra
//*
//*****************************************************************************
//	Sendet eine WM_NOTIFY Nachricht and das Elternfenster um Daten zuholen
//	pData		: Zeiger auf die Fensterdaten
//	pEntry		: Zeiger auf den Eintrag
//	uItem		: Nummer des Eintrages
//	uFlags		: Welche Daten sollen abgefragt werden
//	Ergibt den R?kgabewert der WM_NOTIFY Nachrich
static void CallbackExtra(TreeListData *pData,BaseItem *pEntry,ExtraItem *pExtra,unsigned uItem,unsigned uSub,unsigned uFlags,int &iImage,unsigned &uTextSize,LPCTSTR &pText)
{
	NMTVDISPINFO	sInfo;


	sInfo.item.mask				= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM|uFlags;
	sInfo.item.lParam			= pEntry->lParam;
	sInfo.item.hItem			= (HTREEITEM)uItem;
	sInfo.item.state			= pExtra->uState;
	sInfo.item.state		   |=(pEntry->uState&TVIS_BASEFLAGS);
	sInfo.item.stateMask		= 0xFFFFFFFF;
	sInfo.item.iImage			= I_IMAGECALLBACK;
	sInfo.item.iSelectedImage	= I_IMAGECALLBACK;
	sInfo.item.cChildren		= uSub;

	if(uFlags&TVIF_TEXT)
	{
		pData->cTempText1[sizeof(pData->cTempText1)/sizeof(TCHAR)-1]	= 0;
		pData->cTempText1[0]	= 0;
		sInfo.item.pszText		= pData->cTempText1;
		sInfo.item.cchTextMax	= sizeof(pData->cTempText1)/sizeof(TCHAR)-1;
	}
	else{
		sInfo.item.pszText		= 0;
		sInfo.item.cchTextMax	= 0;
	}

	sInfo.hdr.hwndFrom	= pData->hWnd;
	sInfo.hdr.idFrom	= GetWindowLong(pData->hWnd,GWL_ID);
	sInfo.hdr.code		= TVN_GETDISPINFO;

	UNLOCK(pData);
	SendMessage(GetParent(sInfo.hdr.hwndFrom),WM_NOTIFY,sInfo.hdr.idFrom,(LPARAM)&sInfo);
	LOCK(pData);


	if(uFlags&TVIF_IMAGE)iImage = sInfo.item.iImage;
	if(uFlags&TVIF_TEXT)
	{
		pText		= sInfo.item.pszText;
		uTextSize	= str_len(sInfo.item.pszText);
	}

}

//*****************************************************************************
//*
//*		EditProc
//*
//*****************************************************************************
//	Ist die Fensterfunktion f? das Edit Fenster
static LRESULT CALLBACK EditProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	WNDPROC			pProc;
	int				iDelta;
	int				iState;
	int				iPos;



	pProc = (WNDPROC)GetWindowLongPtr(hWnd,GWL_USERDATA);



	if(uMsg==WM_KEYDOWN)
	{
		if(wParam==VK_RETURN)
		{
			SendMessage(GetParent(hWnd),WM_COMMAND,MAKELONG(3,EN_RETURN),(LPARAM)hWnd);
			return 0;
		}

		if(wParam==VK_ESCAPE)
		{
			SendMessage(GetParent(hWnd),WM_COMMAND,MAKELONG(3,EN_ESCAPE),(LPARAM)hWnd);
			return 0;
		}
	}
	else if(uMsg==WM_COMMAND)
	{
		if(wParam==MAKELONG(3,EN_ESCAPE) || wParam==MAKELONG(3,EN_RETURN))
		{
			SendMessage(GetParent(hWnd),WM_COMMAND,wParam,(LPARAM)hWnd);
			return 0;
		}
	}
	else if(uMsg==WM_MOUSEWHEEL)
	{
		iState=CallWindowProc(pProc,hWnd,CB_GETDROPPEDSTATE,0,0);
		if(iState)
		{
			iDelta  = (short)HIWORD(wParam);
			iDelta /= 120;
			iPos	= CallWindowProc(pProc,hWnd,CB_GETTOPINDEX,0,0);
			iPos   -= iDelta;
			CallWindowProc(pProc,hWnd,CB_SETTOPINDEX,iPos,0);
			return 0;
		}
	}
	else{
		if(uMsg==WM_GETDLGCODE)										// Welsche Tasten werden im Dialog benutzt
		{
			return DLGC_WANTALLKEYS;
		}
	}


	return CallWindowProc(pProc,hWnd,uMsg,wParam,lParam);
}


//*****************************************************************************
//*
//*		CrateToolTip
//*
//*****************************************************************************
//	Erzeugt ein ToolTip Fenster
//	pData		: Zeiger auf die Fensterdaten
static void CrateToolTip(TreeListData *pData)
{
	TOOLINFO sInfo;


	if(pData->hToolTip)return;

	pData->hToolTip	   = CreateWindow(TOOLTIPS_CLASS,NULL,WS_POPUP,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,pData->hWnd,NULL,NULL,NULL);


	sInfo.cbSize       = sizeof(TOOLINFO);
	sInfo.uFlags       = TTF_ABSOLUTE|TTF_TRACK|TTF_IDISHWND;
	sInfo.hwnd         = pData->hWnd;
	sInfo.hinst		   = NULL;
	sInfo.uId          = (LPARAM)(pData->hWnd);
	sInfo.lpszText     = LPSTR_TEXTCALLBACK;

	GetClientRect(pData->hWnd,&sInfo.rect);
	SendMessage  (pData->hToolTip,TTM_ADDTOOL,0,(LPARAM)&sInfo);
	SendMessage  (pData->hToolTip,TTM_SETMAXTIPWIDTH,0,10000);
	SetWindowLong(pData->hToolTip,GWL_ID,2);

}

//*****************************************************************************
//*
//*		CreateStateImageList
//*
//*****************************************************************************
//	Erzeugt eine Image-Liste mit zwei Checkboxen
//	pData		: Zeiger auf die Fensterdaten
//	iMode		: Welche Imageliste soll erzeugt werden (0=hStates 1=hChecks)
static void CreateStateImageList(TreeListData *pData,int iMode)
{
	BITMAPINFO	sInfo;
	BYTE		aMem[0x1000];
	HDC			hDcSrc;
	HDC			hDc;
	HBITMAP		hBmp;
	HBITMAP		hBmpNew;
	RECT		sRect;
	int			iBits;



	if(iMode)
	{
		if(pData->hChecks)return;
	}
	else{
		if(pData->hStates)return;
	}

	hDcSrc	= GetDC(NULL);
	hDc		= CreateCompatibleDC(NULL);
	hBmp    = CreateCompatibleBitmap(hDcSrc,16*3,16);

	SelectObject(hDc,hBmp);
	SelectObject(hDc,GetStockObject(NULL_PEN));
	SetBkMode   (hDc,OPAQUE);
	SetBkColor	(hDc,GetSysColor(COLOR_WINDOW));
	SelectObject(hDc,GetSysColorBrush(COLOR_HIGHLIGHT));
	Rectangle   (hDc,-1,-1,16*3+2,16+2);

	sRect.top	  = 8-6;
	sRect.bottom  = 8+7;
	sRect.left    = 16*1+8-7;
	sRect.right   = 16*1+8+6;

	DrawFrameControl(hDc,&sRect,DFC_BUTTON,DFCS_BUTTONCHECK|DFCS_FLAT);

	sRect.left    = 16*2+8-7;
	sRect.right   = 16*2+8+6;

	DrawFrameControl(hDc,&sRect,DFC_BUTTON,DFCS_BUTTONCHECK|DFCS_CHECKED|DFCS_FLAT);


	iBits = GetDeviceCaps(hDc,BITSPIXEL);

	sInfo.bmiHeader.biSize		  	= sizeof(BITMAPINFOHEADER);
	sInfo.bmiHeader.biWidth			= 16*3;
	sInfo.bmiHeader.biHeight		= 16;
	sInfo.bmiHeader.biPlanes	  	= 1;
	sInfo.bmiHeader.biBitCount	  	= (WORD)iBits;
	sInfo.bmiHeader.biCompression 	= BI_RGB;
	sInfo.bmiHeader.biSizeImage		= 0;
	sInfo.bmiHeader.biXPelsPerMeter	= 0;
	sInfo.bmiHeader.biYPelsPerMeter	= 0;
	sInfo.bmiHeader.biClrUsed		= (iBits>8)? 0:1<<iBits;;
	sInfo.bmiHeader.biClrImportant	= (iBits>8)? 0:1<<iBits;;

	GetDIBits(hDc,hBmp,0,0 ,NULL,&sInfo,(iBits>8)?DIB_RGB_COLORS:DIB_PAL_COLORS);
	GetDIBits(hDc,hBmp,0,16,aMem,&sInfo,(iBits>8)?DIB_RGB_COLORS:DIB_PAL_COLORS);

	hBmpNew = CreateCompatibleBitmap(hDc,16*3,16);

	SetDIBits(hDc,hBmpNew,0,16,aMem,&sInfo,(iBits>8)?DIB_RGB_COLORS:DIB_PAL_COLORS);

	if(iMode==0)
	{
		pData->hStates		= ImageList_Create(16,16,ILC_COLORDDB|ILC_MASK,3,14);
		pData->iStatesXsize	= 16;
		pData->iStatesYsize	= 16;
		pData->iStatesMode	= 1;

		ImageList_AddMasked(pData->hStates,hBmpNew,GetSysColor(COLOR_HIGHLIGHT));
	}
	else{
		pData->hChecks		= ImageList_Create(16,16,ILC_COLORDDB|ILC_MASK,3,14);
		pData->iChecksXsize	= 16;
		pData->iChecksYsize	= 16;
		pData->iChecksMode	= 1;

		ImageList_AddMasked(pData->hChecks,hBmpNew,GetSysColor(COLOR_HIGHLIGHT));
	}


	DeleteObject(hBmpNew);
	DeleteObject(hBmp);
	DeleteDC(hDc);
	ReleaseDC(NULL,hDcSrc);


}

//*****************************************************************************
//*
//*		CreateDragImage
//*
//*****************************************************************************
//	Erzeugt eine Image-Liste mit zwei Checkboxen
//	pData		: Zeiger auf die Fensterdaten
//	uSub		: Ist die Spalte f? die das Drag-Image erzeugt werden soll
//	Ergibt ein Handle mit der Imageliste oder NULL bei einem Fehler
static HIMAGELIST CreateDragImage(TreeListData *pData,unsigned uItem,unsigned uSub)
{
	ExtraItem  *pExtra;
	BaseItem   *pEntry;
	HIMAGELIST	hList;
	BITMAPINFO	sInfo;
	BYTE	   *pMem;
	HDC			hDcSrc;
	HDC			hDc;
	HBITMAP		hBmp;
	HBITMAP		hBmpNew;
	RECT		sRect;
	unsigned	uTSize;
	int			iAdd;
	int			iBits;
	int			iWidth;
	int			iHeigh;
	int			iImage;
	int			iYpos;
	LPCTSTR		pText;



	if(uItem>pData->uTreeItemsMax)return NULL;

	pEntry = pData->pTreeItems[uItem];
	if(!pEntry)return 0;

	iHeigh = pData->iFontHeight;

	if(uSub)												// Image f? Extraeintrag erzeugen
	{
		if(uSub>=pData->uColumnCount)return 0;

		pExtra = pData->pExtraItems[uSub-1][uItem];
		if(!pExtra)
		{
			pText  = _T("????");
			uTSize = 4;
			iImage = -1;
			iWidth = pData->iFontHeight*4;
		}
		else{
			pText  = pExtra->pText;
			uTSize = pExtra->uTextSize;
			iImage = pExtra->iImage;
			iWidth = pExtra->iTextPixels;

			if(pExtra->bCallback&(TVIF_IMAGE|TVIF_TEXT))
			{
				CallbackExtra(pData,pEntry,pExtra,uItem,uSub,pExtra->bCallback,iImage,uTSize,pText);
			}
		}
	}
	else{													// Image f? Haupteintrag erzeugen
		pText  = pEntry->pText;
		uTSize = pEntry->uTextSize;
		iImage = pEntry->iImage;
		iWidth = pEntry->iTextPixels;

		if(pEntry->bCallback&(TVIF_IMAGE|TVIF_TEXT))
		{
			CallbackEntry(pData,pEntry,uItem,pEntry->bCallback,iImage,uTSize,pText);
		}
	}

	if(pData->hImages && iImage>=0)							// Größen f? Images anpassen
	{
		if(iHeigh<pData->iImagesYsize)iHeigh=pData->iImagesYsize;
		iAdd    = pData->iImagesXsize+2;
		iWidth += iAdd;
	}
	else{
		iAdd   = 0;
		iImage = 1;
	}


	if(iWidth>240)iWidth=240;
	if(iHeigh>32 )iHeigh=32;

	pMem = new BYTE[iHeigh*(iWidth+4)*4+1024];
	if(!pMem)return NULL;

	hDcSrc	= GetDC(NULL);
	hDc		= CreateCompatibleDC(NULL);
	hBmp    = CreateCompatibleBitmap(hDcSrc,iWidth,iHeigh);

	SelectObject(hDc,hBmp);
	SelectObject(hDc,GetStockObject(NULL_PEN));
	SelectObject(hDc,(pEntry->uState&TVIS_BOLD)? pData->hFontB:pData->hFontN);
	SetTextColor(hDc,pData->uColors[TVC_TEXT]);
	SetBkColor	(hDc,RGB(123,77,91));

	sRect.top	  = 0;
	sRect.bottom  = iHeigh;
	sRect.left    = 0;
	sRect.right   = iWidth;
	iYpos		  =(iHeigh-pData->iFontHeight)/2;

	ExtTextOut(hDc,iAdd,iYpos,ETO_OPAQUE|ETO_CLIPPED,&sRect,pText,uTSize,NULL);

	if(iImage>=0)
	{
		SetBkColor(hDc,GetSysColor(COLOR_WINDOW));
		ImageList_Draw(pData->hImages,iImage,hDc,0,0,ILD_TRANSPARENT);
	}



	iBits = GetDeviceCaps(hDc,BITSPIXEL);

	sInfo.bmiHeader.biSize		  	= sizeof(BITMAPINFOHEADER);
	sInfo.bmiHeader.biWidth			= iWidth;
	sInfo.bmiHeader.biHeight		= iHeigh;
	sInfo.bmiHeader.biPlanes	  	= 1;
	sInfo.bmiHeader.biBitCount	  	= (WORD)iBits;
	sInfo.bmiHeader.biCompression 	= BI_RGB;
	sInfo.bmiHeader.biSizeImage		= 0;
	sInfo.bmiHeader.biXPelsPerMeter	= 0;
	sInfo.bmiHeader.biYPelsPerMeter	= 0;
	sInfo.bmiHeader.biClrUsed		= (iBits>8)? 0:1<<iBits;;
	sInfo.bmiHeader.biClrImportant	= (iBits>8)? 0:1<<iBits;;

	GetDIBits(hDc,hBmp,0,0     ,NULL,&sInfo,(iBits>8)?DIB_RGB_COLORS:DIB_PAL_COLORS);
	GetDIBits(hDc,hBmp,0,iHeigh,pMem,&sInfo,(iBits>8)?DIB_RGB_COLORS:DIB_PAL_COLORS);

	hBmpNew = CreateCompatibleBitmap(hDc,iWidth,iHeigh);

	SetDIBits(hDc,hBmpNew,0,iHeigh,pMem,&sInfo,(iBits>8)?DIB_RGB_COLORS:DIB_PAL_COLORS);

	hList = ImageList_Create(iWidth,iHeigh,ILC_COLORDDB|ILC_MASK,1,0);

	ImageList_AddMasked(hList,hBmpNew,RGB(123,77,91));

	DeleteObject(hBmpNew);
	DeleteObject(hBmp);
	DeleteDC(hDc);
	ReleaseDC(NULL,hDcSrc);

	delete pMem;


	return hList;
}


//*****************************************************************************
//*
//*		UpdateColors
//*
//*****************************************************************************
//	Aktualisiert alle Farben
//	pData		: Zeiger auf die Fensterdaten
static void UpdateColors(TreeListData *pData)
{
	int 	iDiff;
	int 	iSum;

	if(!pData->cColorChanged[TVC_BK    ])pData->uColors[TVC_BK    ]  = GetSysColor(COLOR_WINDOW);
	if(!pData->cColorChanged[TVC_BOX   ])pData->uColors[TVC_BOX   ]  = GetSysColor(COLOR_BTNSHADOW);
	if(!pData->cColorChanged[TVC_EVEN  ])pData->uColors[TVC_EVEN  ]  = GetSysColor(COLOR_WINDOW);
	if(!pData->cColorChanged[TVC_TEXT  ])pData->uColors[TVC_TEXT  ]  = GetSysColor(COLOR_WINDOWTEXT);
	if(!pData->cColorChanged[TVC_LINE  ])pData->uColors[TVC_LINE  ]  = GetSysColor(COLOR_WINDOWTEXT);
	if(!pData->cColorChanged[TVC_FRAME ])pData->uColors[TVC_FRAME ]  = GetSysColor(COLOR_3DFACE);
	if(!pData->cColorChanged[TVC_TRACK ])pData->uColors[TVC_TRACK ]  = GetSysColor(COLOR_WINDOWTEXT)^RGB(0,0,255);
	if(!pData->cColorChanged[TVC_INSERT])pData->uColors[TVC_INSERT]  = GetSysColor(COLOR_INFOBK);
	if(!pData->cColorChanged[TVC_ODD   ])pData->uColors[TVC_ODD   ]  = GetSysColor(COLOR_INFOBK);
	if(!pData->cColorChanged[TVC_BOXBG ])pData->uColors[TVC_BOXBG ]  = GetSysColor(COLOR_WINDOW);

	if(!pData->cColorChanged[TVC_ODD])
	{
		iDiff = ((pData->uColors[TVC_ODD]    )&0xFF)-((pData->uColors[TVC_EVEN]    )&0xFF); iSum  = iDiff*iDiff;
		iDiff = ((pData->uColors[TVC_ODD]>> 8)&0xFF)-((pData->uColors[TVC_EVEN]>> 8)&0xFF); iSum += iDiff*iDiff;
		iDiff = ((pData->uColors[TVC_ODD]>>16)&0xFF)-((pData->uColors[TVC_EVEN]>>16)&0xFF); iSum += iDiff*iDiff;

		if(iSum<64)											// Ist die alternierente Farbe fast gleich ?
		{
			pData->uColors[TVC_ODD]  =   pData->uColors[TVC_EVEN]&0x0000FFFF;
			pData->uColors[TVC_ODD] |= ((pData->uColors[TVC_EVEN]&0x00FF0000)-0x00080000)&0x00FF0000;
		}
	}

	//if(!pData->cColorChanged[TVC_MARKODD ])pData->uColors[TVC_MARKODD ]  =((pData->uColors[TVC_ODD     ]>>3)&0x1F1F1F)*7;
	//if(!pData->cColorChanged[TVC_MARKODD ])pData->uColors[TVC_MARKODD ] +=((GetSysColor(COLOR_HIGHLIGHT)>>3)&0x1F1F1F)*1;
	//if(!pData->cColorChanged[TVC_MARKEVEN])pData->uColors[TVC_MARKEVEN]  =((pData->uColors[TVC_EVEN    ]>>3)&0x1F1F1F)*7;
	//if(!pData->cColorChanged[TVC_MARKEVEN])pData->uColors[TVC_MARKEVEN] +=((GetSysColor(COLOR_HIGHLIGHT)>>3)&0x1F1F1F)*1;

	if(!pData->cColorChanged[TVC_MARKODD ])pData->uColors[TVC_MARKODD ]  =((pData->uColors[TVC_ODD     ]))*9;
	if(!pData->cColorChanged[TVC_MARKODD ])pData->uColors[TVC_MARKODD ] +=((GetSysColor(COLOR_HIGHLIGHT)))*1;
	if(!pData->cColorChanged[TVC_MARKEVEN])pData->uColors[TVC_MARKEVEN]  =((pData->uColors[TVC_EVEN    ]))*9;
	if(!pData->cColorChanged[TVC_MARKEVEN])pData->uColors[TVC_MARKEVEN] +=((GetSysColor(COLOR_HIGHLIGHT)))*1;

}

//*****************************************************************************
//*
//*		UpdateHeight
//*
//*****************************************************************************
//	Pr?t ob die Zeilenh?e sich ge?tert hat
//	pData		: Zeiger auf die Fensterdaten
//	Ergibt 1 bei Änderungen sonst 0
static int UpdateHeight(TreeListData *pData)
{
	int		iHeight;
	RECT	sRect;


	if(pData->cFixedHeight)return 0;

	iHeight = 10;

	if(pData->hChecks)
		if(iHeight<pData->iChecksYsize+2)iHeight=pData->iChecksYsize+2;
	if(pData->hStates)
		if(iHeight<pData->iStatesYsize+2)iHeight=pData->iStatesYsize+2;
	if(iHeight<pData->iImagesYsize+2)iHeight=pData->iImagesYsize+2;
	if(iHeight<pData->iFontHeight +2)iHeight=pData->iFontHeight +2;
	if(pData->uStyleEx&TVS_EX_ITEMLINES)iHeight++;
	if(pData->uStyle  &TVS_NONEVENHEIGHT && (iHeight&1))iHeight++;
	if(pData->iRowHeight==iHeight)return 0;

	pData->iRowHeight	 = iHeight;
	pData->uMaxEnties	 = pData->uSizeY;
	pData->uMaxEnties	-= pData->uStartPixel;
	pData->uPageEnties	 = pData->uMaxEnties;
	pData->uMaxEnties	+= pData->iRowHeight-1;
	pData->uMaxEnties   /= pData->iRowHeight;
	pData->uPageEnties  /= pData->iRowHeight;

	GetClientRect (pData->hWnd,&sRect);
	InvalidateRect(pData->hWnd,&sRect,FALSE);


	return 1;
}

//*****************************************************************************
//*
//*		UpdateRect
//*
//*****************************************************************************
//	Zeichnet einen Eintrag neu
//	pData		: Zeiger auf die Fensterdaten
//	uItem		: Ist die Nummer des Eintrages
//	uSub		: Ist die Spaltennummer
//	Ergibt 1 wenn der Eintrag sichtbar war
static int UpdateRect(TreeListData *pData,unsigned uItem,unsigned uSub)
{
	BaseItem   *pEntry;
	RECT		sRect;
	unsigned	uPos;



	pEntry = pData->pTreeItems[uItem];
	if(!pEntry || !pEntry->uShowPos)return 0;				// Ist der Eintrag aufgeklappt

	uPos = pEntry->uShowPos-pData->uScrollY-1;
	if(uPos>=pData->uMaxEnties)return 0;					// Eintrag im Fenstersichtbar

	sRect.left    = pData->iColumnXpos[uSub  ];
	sRect.left   -= pData->uScrollX;
	sRect.right	  = pData->iColumnXpos[uSub+1];
	sRect.right  -= pData->uScrollX;
	sRect.top     = pData->uStartPixel;
	sRect.top    += pData->iRowHeight*uPos;
	sRect.bottom  = pData->iRowHeight+sRect.top;

	InvalidateRect(pData->hWnd,&sRect,FALSE);



	return 1;
}

//*****************************************************************************
//*
//*		UpdateColRect
//*
//*****************************************************************************
//	Zeichnet einen ganze Spalte neu 
//	pData		: Zeiger auf die Fensterdaten
//	uColumn		: Die nummer der Spalte
//	Ergibt 1 wenn der Eintrag sichtbar war
static int UpdateColRect(TreeListData *pData,unsigned uColumn)
{
	RECT		sRect;



	if(uColumn>=pData->uColumnCount)return 0;


	sRect.left  = pData->iColumnXpos[uColumn  ];
	sRect.left -= pData->uScrollX;
	if(sRect.left>(int)pData->uSizeX)return 0;

	sRect.right = pData->iColumnXpos[uColumn+1];
	sRect.right-= pData->uScrollX;
	if(sRect.right<0)return 0;

	sRect.top      = 0;
	sRect.bottom   = pData->uSizeY;

	InvalidateRect(pData->hWnd,&sRect,FALSE);


	return 1;
}

//*****************************************************************************
//*
//*		UpdateRow
//*
//*****************************************************************************
//	Zeichnet einen Eintrag ?er die ganze Zeile neu
//	pData		: Zeiger auf die Fensterdaten
//	uItem		: Ist die Nummer des Eintrages
//	Ergibt 1 wenn der Eintrag sichtbar war
static int UpdateRow(TreeListData *pData,unsigned uItem)
{
	BaseItem   *pEntry;
	RECT		sRect;
	unsigned	uPos;



	pEntry = pData->pTreeItems[uItem];
	if(!pEntry || !pEntry->uShowPos)return 0;				// Ist der Eintrag aufgeklappt

	uPos = pEntry->uShowPos-pData->uScrollY-1;
	if(uPos>=pData->uMaxEnties)return 0;					// Eintrag im Fenstersichtbar

	sRect.left    = 0;
	sRect.right   = pData->uSizeX;
	sRect.top     = pData->uStartPixel;
	sRect.top    += pData->iRowHeight*uPos;
	sRect.bottom  = pData->iRowHeight+sRect.top;
	InvalidateRect(pData->hWnd,&sRect,FALSE);



	return 1;
}

//*****************************************************************************
//*
//*		UpdateView
//*
//*****************************************************************************
//	Zeichnet das ganze Fenster neu
//	pData		: Zeiger auf die Fensterdaten
//	Ergibt 1 wenn der Eintrag sichtbar war
static void UpdateView(TreeListData *pData)
{
	RECT	sRect;


	GetClientRect (pData->hWnd,&sRect);
	sRect.top =    pData->uStartPixel;
	InvalidateRect(pData->hWnd,&sRect,FALSE);

}

//*****************************************************************************
//*
//*		UpdateScrollX
//*
//*****************************************************************************
//	Aktualisiert die X-Scroolbar
//	pData		: Zeiger auf die Fensterdaten
//	Ergibt 1 wenn der sich Einstellungen ver?dert haben
static void UpdateScrollX(TreeListData *pData)
{
	SCROLLINFO  sInfo;
	unsigned	uSize;
	unsigned	uCols;


	uCols = pData->uColumnCount;
	if(uCols)
		uSize = pData->iColumnXpos[uCols]-1;
	else	uSize = pData->iMaxSizeX;


	if(pData->uOldXCount==uSize)
		if(pData->uOldXPage ==pData->uSizeX)
		{
			return;
		}

		pData->uOldYPage	= pData->uSizeX;
		pData->uOldYCount	= uSize;

		UNLOCK(pData);

		sInfo.cbSize	= sizeof(SCROLLINFO);
		sInfo.fMask		= SIF_ALL;
		sInfo.nMin		= 0;
		sInfo.nMax  	= uSize;
		sInfo.nPage 	= pData->uSizeX;
		sInfo.nPos  	= pData->uScrollX;
		sInfo.nTrackPos	= 0;

		if(pData->uStyle&TVS_NOSCROLL)
		{
			sInfo.nMax = 0;
		}
		else if(pData->uStyleEx&TVS_EX_AUTOHSCROLL)
		{
			sInfo.nMax = 0;
		}


		if((int)sInfo.nPage>=sInfo.nMax && pData->uScrollX>0)
		{
			sInfo.nPos      = 0;
			pData->uScrollX = 0;

			UpdateView(pData);

			if(pData->hHeader)
			{
				MoveWindow(pData->hHeader,0,0,pData->uSizeX,pData->uStartPixel,TRUE);
			}
		}

		SetScrollInfo(pData->hWnd,SB_HORZ,&sInfo,TRUE);

		LOCK(pData);

}

//*****************************************************************************
//*
//*		UpdateScrollY
//*
//*****************************************************************************
//	Aktualisiert die Y-Scroolbar
//	pData		: Zeiger auf die Fensterdaten
//	Ergibt 1 wenn der sich Einstellungen ver?dert haben
static void UpdateScrollY(TreeListData *pData)
{
	SCROLLINFO  sInfo;


	if(pData->uOldYCount==pData->uItemPosCount)
		if(pData->uOldYPage ==pData->uPageEnties  )
		{
			return;
		}

		pData->uOldYPage	= pData->uPageEnties;
		pData->uOldYCount	= pData->uItemPosCount;

		UNLOCK(pData);

		sInfo.cbSize	= sizeof(SCROLLINFO);
		sInfo.fMask		= SIF_ALL;
		sInfo.nMin		= 0;
		sInfo.nMax  	= pData->uItemPosCount;
		sInfo.nPage 	= pData->uPageEnties;
		sInfo.nPos  	= pData->uScrollY;
		sInfo.nTrackPos	= 0;

		if(pData->uStyle&TVS_NOSCROLL)
		{
			sInfo.nMax = 0;
		}

		if((int)sInfo.nPage>=sInfo.nMax && pData->uScrollY>0)
		{
			sInfo.nPos      = 0;
			pData->uScrollY = 0;

			UpdateView(pData);
		}


		SetScrollInfo(pData->hWnd,SB_VERT,&sInfo,TRUE);


		LOCK(pData);




}

//*****************************************************************************
//*
//*		UpdateToolTip
//*
//*****************************************************************************
//	Ist die Fensterfunktion f? das TreeList Fenster
//	pData	: Zeiger auf Fensterdaten
//	uItem	: Item auf den der Mauszeiger zeigt
//	uFlags	: Flags vom HitTest
static void UpdateToolTip(TreeListData *pData,unsigned uItem,unsigned uFlags)
{
	HWND		hToolTip;
	TOOLINFO	sInfo;
	ExtraItem  *pExtra;
	BaseItem   *pEntry;
	LPCTSTR	    pText;
	RECT		sRect;
	unsigned	uSize;
	unsigned	uCol;
	LRESULT		lRet;
	int			iTemp;




	if(!uItem)
	{
		if(pData->uToolTipItem)goto ExitTip;
		return;
	}


	pEntry=pData->pTreeItems[uItem];

	if(uFlags&TVHT_ONITEM)
	{
		if(pData->uToolTipItem!=uItem || pData->uToolTipSub!=0)
		{
			if(!pData->pTreeItems[uItem])					// Existiert der Eintag noch ?
			{
				goto ExitTip;
			}

			TreeListGetItemRect(pData,uItem,TVIR_GETCOLUMN|TVIR_TEXT,&sRect);

			if(sRect.right>(int)pData->uSizeX)
			{
				sRect.right = pData->uSizeX;
			}

			if(pData->uStyleEx&TVS_EX_TOOLTIPNOTIFY)		// Tooltip-Daten via Notify holen
			{
				NMTREEVIEW	sNotify;

				sNotify.hdr.code			= TVN_ITEMTOOLTIP;
				sNotify.action				= 0;
				sNotify.itemNew.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM;
				sNotify.itemNew.hItem		= (HTREEITEM)uItem;
				sNotify.itemNew.stateMask	= 0xFFFFFFFF;
				sNotify.itemNew.state		= pEntry->uState;
				sNotify.itemNew.lParam		= pEntry->lParam;
				sNotify.itemNew.pszText		= pEntry->pText;
				sNotify.itemNew.cchTextMax	= pEntry->uTextSize;
				sNotify.itemNew.cChildren	= 0;
				sNotify.itemOld.mask		= 0;
				sNotify.ptDrag.x			= sRect.left;
				sNotify.ptDrag.y			= sRect.top;

				UNLOCK(pData);
				lRet=SendNotify(pData,&sNotify.hdr);
				LOCK(pData);

				if(lRet)									// Soll ein User-Tooltip angezeigt werden
				{
					pText	   = sNotify.itemNew.pszText;
					uSize	   = sNotify.itemNew.cchTextMax;
					sRect.left = sNotify.ptDrag.x;
					sRect.top  = sNotify.ptDrag.y;

					if(!pText ||*pText==0)goto ExitTip;

					str_ncpy(pData->cToolTipText,pText,sizeof(pData->cToolTipText)/sizeof(TCHAR)-1);
					pData->hFontT =(pEntry->uState&TVIS_BOLD)? pData->hFontB:pData->hFontN;

					ClientToScreen(pData->hWnd,&sNotify.ptDrag);
					pData->sToolTipPos		= sNotify.ptDrag;
					hToolTip				= pData->hToolTip;

					if(sNotify.itemNew.mask&TVIF_TOOLTIPTIME && sNotify.itemNew.lParam>0)
					{
						pData->uToolTipShow = (unsigned)(sNotify.itemNew.lParam+499)/500;
						pData->uToolTipItem = uItem;
						pData->uToolTipSub	= 0;

						SetTimer(pData->hWnd,ID_TOOLTIPCHECK,500,NULL);
					}
					else{
						UNLOCK(pData);

						sInfo.cbSize		= sizeof(sInfo);
						sInfo.hwnd			= pData->hWnd;
						sInfo.uId			= (UINT_PTR)pData->hWnd;

						if(pData->uToolTipItem)
						{
							SendMessage(pData->hToolTip,TTM_TRACKACTIVATE,0,(LPARAM)&sInfo);
							pData->uToolTipItem = 0;
						}

						SendMessage(pData->hToolTip,TTM_TRACKPOSITION,0,MAKELONG(sNotify.ptDrag.x,sNotify.ptDrag.y));
						SendMessage(pData->hToolTip,TTM_TRACKACTIVATE,1,(LPARAM)&sInfo);

						LOCK(pData);

						pData->uToolTipShow = 0;
						pData->uToolTipItem = uItem;
						pData->uToolTipSub	= 0;

						SetTimer(pData->hWnd,ID_TOOLTIPCHECK,1500,NULL);
					}

					return;
				}
			}
			// Passt der Text in die Spalte
			if(sRect.right-sRect.left<=pEntry->iTextPixels+4)
			{
				pText	= pEntry->pText;
				uSize	= pEntry->uTextSize;

				if(pEntry->bCallback&TVIF_TEXT)
				{
					CallbackEntry(pData,pEntry,uItem,TVIF_TEXT,iTemp,uSize,pText);
				}

				if(!pText || *pText==0)goto ExitTip;

				str_ncpy(pData->cToolTipText,pText,sizeof(pData->cToolTipText)/sizeof(TCHAR)-1);
				pData->hFontT =(pEntry->uState&TVIS_BOLD)? pData->hFontB:pData->hFontN;


				hToolTip				= pData->hToolTip;
				pData->sToolTipPos.x	= sRect.left;
				pData->sToolTipPos.y	= sRect.top;

				ClientToScreen(pData->hWnd,&pData->sToolTipPos);

				UNLOCK(pData);

				sInfo.cbSize			= sizeof(sInfo);
				sInfo.hwnd				= pData->hWnd;
				sInfo.uId				= (UINT_PTR)pData->hWnd;

				if(pData->uToolTipItem)
				{
					SendMessage(pData->hToolTip,TTM_TRACKACTIVATE,0,(LPARAM)&sInfo);
					pData->uToolTipItem = 0;
				}

				SendMessage(pData->hToolTip,TTM_TRACKPOSITION,0,MAKELONG(pData->sToolTipPos.x,pData->sToolTipPos.y));
				SendMessage(pData->hToolTip,TTM_TRACKACTIVATE,1,(LPARAM)&sInfo);

				LOCK(pData);

				pData->uToolTipItem		= uItem;
				pData->uToolTipShow 	= 0;
				pData->uToolTipSub		= 0;

				SetTimer(pData->hWnd,ID_TOOLTIPCHECK,1500,NULL);
			}
			else{
				if(pData->uToolTipItem)goto ExitTip;
			}
		}

		return;
	}

	if(uFlags&(TVHT_ONSUBICON|TVHT_ONSUBLABEL))
	{
		if(pData->uToolTipItem!=uItem || TVHT_SUBTOCOL(uFlags)!=pData->uToolTipSub)
		{
			uCol   = TVHT_SUBTOCOL(uFlags);
			pExtra = pData->pExtraItems[uCol-1][uItem];

			if(pData->uStyleEx&TVS_EX_TOOLTIPNOTIFY)		// Tooltip-Daten via Notify holen
			{
				NMTREEVIEW	sNotify;

				TreeListGetItemRect(pData,uItem,TVIR_GETCOLUMN|TVIR_TEXT|TVIR_COLTOSUB(uCol),&sRect);

				if(sRect.right>(int)pData->uSizeX)
				{
					sRect.right = pData->uSizeX;
				}

				if(pExtra)
				{
					sNotify.itemNew.state		= pExtra->uState;
					sNotify.itemNew.pszText		= pExtra->pText;
					sNotify.itemNew.cchTextMax	= pExtra->uTextSize;
				}
				else{
					sNotify.itemNew.state		= 0;
					sNotify.itemNew.cchTextMax	= 0;
					sNotify.itemNew.pszText		= _T("");
				}

				sNotify.hdr.code			= TVN_ITEMTOOLTIP;
				sNotify.action				= 0;
				sNotify.itemNew.lParam		= pEntry->lParam;
				sNotify.itemNew.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM;
				sNotify.itemNew.hItem		= (HTREEITEM)uItem;
				sNotify.itemNew.stateMask	= 0xFFFFFFFF;
				sNotify.itemNew.cChildren	= uCol;
				sNotify.itemOld.mask		= 0;
				sNotify.ptDrag.x			= sRect.left;
				sNotify.ptDrag.y			= sRect.top;

				UNLOCK(pData);
				lRet=SendNotify(pData,&sNotify.hdr);
				LOCK(pData);

				if(lRet)									// Soll ein User-Tooltip angezeigt werden
				{
					pText		= sNotify.itemNew.pszText;
					uSize		= sNotify.itemNew.cchTextMax;
					sRect.left	= sNotify.ptDrag.x;
					sRect.top	= sNotify.ptDrag.y;

					if(!pText || *pText==0)goto ExitTip;

					str_ncpy(pData->cToolTipText,pText,sizeof(pData->cToolTipText)/sizeof(TCHAR)-1);
					pData->hFontT =(pExtra && pExtra->uState&TVIS_BOLD)? pData->hFontB:pData->hFontN;


					ClientToScreen(pData->hWnd,&sNotify.ptDrag);
					pData->sToolTipPos		= sNotify.ptDrag;
					hToolTip				= pData->hToolTip;

					if(sNotify.itemNew.mask&TVIF_TOOLTIPTIME && sNotify.itemNew.lParam>0)
					{
						pData->uToolTipShow = (unsigned)(sNotify.itemNew.lParam+499)/500;
						pData->uToolTipItem = uItem;
						pData->uToolTipSub	= uCol;

						SetTimer(pData->hWnd,ID_TOOLTIPCHECK,500,NULL);
					}
					else{
						UNLOCK(pData);

						sInfo.cbSize		= sizeof(sInfo);
						sInfo.hwnd			= pData->hWnd;
						sInfo.uId			= (UINT_PTR)pData->hWnd;

						if(pData->uToolTipItem)
						{
							SendMessage(pData->hToolTip,TTM_TRACKACTIVATE,0,(LPARAM)&sInfo);
							pData->uToolTipItem = 0;
						}

						SendMessage(pData->hToolTip,TTM_TRACKPOSITION,0,MAKELONG(sNotify.ptDrag.x,sNotify.ptDrag.y));
						SendMessage(pData->hToolTip,TTM_TRACKACTIVATE,1,(LPARAM)&sInfo);

						LOCK(pData);

						pData->uToolTipItem = uItem;
						pData->uToolTipSub	= uCol;
						pData->uToolTipShow = 0;

						SetTimer(pData->hWnd,ID_TOOLTIPCHECK,1500,NULL);
					}

					return;
				}
			}

			if(pExtra)										// Tooltip auf Unterspalte ?
			{
				TreeListGetItemRect(pData,uItem,TVIR_GETCOLUMN|TVIR_TEXT|TVIR_COLTOSUB(uCol),&sRect);

				if(sRect.right>(int)pData->uSizeX)
				{
					sRect.right = pData->uSizeX;
				}

				if(sRect.right-sRect.left<=pExtra->iTextPixels+4)
				{
					pText	= pExtra->pText;
					uSize	= pExtra->uTextSize;

					if(pExtra->bCallback&TVIF_TEXT)
					{
						CallbackExtra(pData,pEntry,pExtra,uItem,uCol,TVIF_TEXT,iTemp,uSize,pText);
					}

					if(!pText || *pText==0)goto ExitTip;

					str_ncpy(pData->cToolTipText,pText,sizeof(pData->cToolTipText)/sizeof(TCHAR)-1);
					pData->hFontT =(pExtra->uState&TVIS_BOLD)? pData->hFontB:pData->hFontN;

					pData->sToolTipPos.x	= sRect.left;
					pData->sToolTipPos.y	= sRect.top;
					hToolTip				= pData->hToolTip;

					ClientToScreen(pData->hWnd,&pData->sToolTipPos);

					UNLOCK(pData);

					sInfo.cbSize			= sizeof(sInfo);
					sInfo.hwnd				= pData->hWnd;
					sInfo.uId				= (UINT_PTR)pData->hWnd;

					if(pData->uToolTipItem)
					{
						SendMessage(pData->hToolTip,TTM_TRACKACTIVATE,0,(LPARAM)&sInfo);
						pData->uToolTipItem = 0;
					}

					SendMessage(pData->hToolTip,TTM_TRACKPOSITION,0,MAKELONG(pData->sToolTipPos.x,pData->sToolTipPos.y));
					SendMessage(pData->hToolTip,TTM_TRACKACTIVATE,1,(LPARAM)&sInfo);

					LOCK(pData);

					pData->uToolTipItem		= uItem;
					pData->uToolTipSub		= uCol;
					pData->uToolTipShow 	= 0;

					SetTimer(pData->hWnd,ID_TOOLTIPCHECK,1500,NULL);
				}
				else{
					if(pData->uToolTipItem)goto ExitTip;
				}
			}
			else{
				if(pData->uToolTipItem)goto ExitTip;
			}
		}

		return;
	}

ExitTip:

	if(pData->uToolTipItem)									// Tooltip ausblenden
	{
		UNLOCK(pData);

		sInfo.cbSize			= sizeof(sInfo);
		sInfo.hwnd				= pData->hWnd;
		sInfo.uId				= (UINT_PTR)pData->hWnd;

		SendMessage(pData->hToolTip,TTM_TRACKACTIVATE,0,(LPARAM)&sInfo);

		LOCK(pData);

		pData->uToolTipItem = 0;
		pData->uToolTipSub	= 0;
		pData->uToolTipShow = 0;

		KillTimer(pData->hWnd,ID_TOOLTIPCHECK);
	}


}

//*****************************************************************************
//*
//*		UpdateFont
//*
//*****************************************************************************
//	Erzeugt einen den fetten Font f? das Fenster
//	pData		: Zeiger auf die Fensterdaten
//	iRedraw		: Soll das Fenster neugezeichnet werden
//	Ergibt 1 wenn der Font ver?dert wurde
static int UpdateFont(TreeListData *pData)
{
	int			iPos;
	int			iRet;
	HDC			hDc;
	LOGFONT		sLog;
	SIZE		sSize;
	TEXTMETRIC	sMetrics;
	BaseItem   *pEntry;
	BaseItem  **pList;
	ExtraItem  *pExtra;
	ExtraItem **pItems;
	unsigned	uSub;



	if(!hDefaultFontN)										// Den Standard-Font erzeugen
	{
		SystemParametersInfo(SPI_GETICONTITLELOGFONT,sizeof(sLog),&sLog,0);
		sLog.lfWeight=FW_NORMAL;hDefaultFontN=CreateFontIndirect(&sLog);
		sLog.lfWeight=FW_BOLD  ;hDefaultFontB=CreateFontIndirect(&sLog);
	}


	if(!pData->hFontN)pData->hFontN = hDefaultFontN;

	if(pData->hFontN==hDefaultFontN)						// Ist der Standard-Font eingestellt
	{
		pData->hFontB=hDefaultFontB;
	}
	else{
		pData->hFontB=pData->hFontN;
	}

	if(pData->hFontN!=pData->hFontL)
	{
		pData->hFontL=pData->hFontN;

		hDc = GetDC(NULL);
		SelectObject(hDc,pData->hFontN);
		GetTextMetrics(hDc,&sMetrics);
		pData->iFontHeight =  sMetrics.tmHeight;
		pData->iFontLine   =  sMetrics.tmAscent+1;
		pData->iFontOff    = (sMetrics.tmPitchAndFamily&TMPF_FIXED_PITCH)? 0:-1;
		ReleaseDC(NULL,hDc);

		pList = pData->pTreeItems;
		iPos  = pData->uTreeItemsMax;

		for(;iPos>=0;iPos--)								// Alle Textbreiten zur?ksetzen
		{
			pEntry = pList[iPos];
			if(!pEntry)continue;

			pEntry->iTextPixels=0;
		}


		for(uSub=1;uSub<pData->uColumnCount;uSub++)
		{
			iPos    = pData->uTreeItemsMax;
			pItems 	= pData->pExtraItems[uSub-1];

			for(;iPos>=0;iPos--)
			{
				pExtra = pItems[iPos];
				if(!pExtra)continue;

				pExtra->iTextPixels=0;
			}
		}


		iRet=1;
	}
	else{
		iRet=0;
	}


	hDc = GetDC(NULL);										// Breite der "..." Texte
	SelectObject(hDc,pData->hFontN);
	GetTextExtentExPoint(hDc,_T("..."),3,256,NULL,NULL,&sSize);
	pData->uTrippleN =  sSize.cx;
	SelectObject(hDc,pData->hFontB);
	GetTextExtentExPoint(hDc,_T("..."),3,256,NULL,NULL,&sSize);
	pData->uTrippleB =  sSize.cx;
	ReleaseDC(NULL,hDc);




	return iRet;
}


//*****************************************************************************
//*
//*		UpdateItems
//*
//*****************************************************************************
//	Berechnet die Positionen der Zeilen f? die sichtbaren Eintr?e
//	pData		: Zeiger auf die Fensterdaten
//	uItem		: Ist der Eintrag ab dem begonnen wird
static void UpdateItems(TreeListData *pData,unsigned uItem)
{
	unsigned	uPos;
	unsigned	uOld;
	unsigned	uNum;
	unsigned	uTemp;
	unsigned	uStart;
	unsigned   *pLines;
	BaseItem  **pItems;
	BaseItem   *pEntry;
	BaseItem   *pTemp;
	RECT		sRect;


	uOld	= pData->uItemPosCount;
	pLines	= pData->pItemPos;
	pItems	= pData->pTreeItems;


	if(!uItem)												// Am Anfang beginnen
	{
		uItem = pData->uFirstChild;
		if(!uItem)											// Leere Liste
		{
			if(!uOld)return;

			for(uNum=0;uNum<uOld;uNum++)					// Die alten Eintr?e zur?ksetzen
			{
				uTemp = pLines[uNum];
				if(!uTemp)continue;
				pLines[uNum]=0;
				pTemp = pItems[uTemp];
				if(!pTemp)continue;
				pTemp->uShowPos = 0;
			}

			pData->uItemPosCount = 0;

			GetClientRect (pData->hWnd,&sRect);
			InvalidateRect(pData->hWnd,&sRect,TRUE);

			memset(pLines,0,sizeof(unsigned)*uOld);
			return;
		}


		for(uNum=0;uNum<uOld;uNum++)						// Die alten Eintr?e zur?ksetzen
		{
			uTemp = pLines[uNum];
			if(!uTemp)continue;
			pLines[uNum]=0;
			pTemp = pItems[uTemp];
			if(!pTemp)continue;
			pTemp->uShowPos = 0;
		}

		pEntry			 = pItems[uItem];
		pEntry->uShowPos = 1;
		pLines[0]		 = uItem;
		uPos			 = 1;
		uStart			 = 0;
	}
	else{													// Bei einem Eintrag beginnen
		pEntry			 = pItems[uItem];
		uPos			 = pEntry->uShowPos;
		if(uPos)uStart	 = uPos-1;
		else	uStart	 = 0;

		for(uNum=uPos;uNum<uOld;uNum++)						// Die alten Eintr?e zur?ksetzen
		{
			uTemp = pLines[uNum];
			if(!uTemp)continue;
			pLines[uNum]=0;
			pTemp = pItems[uTemp];
			if(!pTemp)continue;
			pTemp->uShowPos = 0;
		}
	}


	for(;;)													// Die Zeilen neu zuordnen
	{
		if(pEntry->uFirstChild && (pEntry->uState&TVIS_EXPANDED))
		{
			uItem=pEntry->uFirstChild;
		}
		else if(pEntry->uNextItem)
		{
			uItem=pEntry->uNextItem;
		}
		else{
			for(;;)
			{
				uItem = pEntry->uParent;
				if(!uItem)break;

				pEntry = pItems[uItem];
				if(pEntry->uNextItem)						// Gibt es etwas in der gleichen Ebene
				{
					uItem = pEntry->uNextItem;
					break;
				}
			}

			if(!uItem)break;
		}

		pEntry = pItems[uItem];

		if(pLines[uPos]!=uItem)
		{
			pLines[uPos] = uItem;
		}
		else{
			if(uStart==uPos)uStart++;
		}

		uPos++;
		pEntry->uShowPos = uPos;
	}


	pData->uItemPosCount = uPos;


	if(uStart>pData->uScrollY)								// Neu zu zeichnenten Bereich bestimmen
		uStart-= pData->uScrollY;
	else	uStart = 0;

	GetClientRect (pData->hWnd,&sRect);

	sRect.top    = pData->iRowHeight*uStart;
	if(sRect.top<=sRect.bottom)
	{
		InvalidateRect(pData->hWnd,&sRect,FALSE);
	}

	if(uOld!=uPos)UpdateScrollY(pData);



}


//*****************************************************************************
//*
//*		UpdateColumns
//*
//*****************************************************************************
//	Pr?t ob es Ver?derungen in Spaltenbreiten gab
//	pData		: Zeiger auf die Fensterdaten
//	Ergibt die Breite ab der die Spalten ver?dert wurden oder 0x10000
static int UpdateColumns(TreeListData *pData)
{
	HWND		hHeader;
	RECT		sRect;
	unsigned	uPos;
	int			iNum;
	int			iRet=0x10000;


	hHeader = pData->hHeader;
	pData->iColumnXpos[0] = 0;

	for(uPos=0;uPos<pData->uColumnCount;)
	{
		Header_GetItemRect(hHeader,uPos,&sRect);

		uPos++;
		sRect.right--;
		iNum=pData->iColumnXpos[uPos];

		if(iNum==sRect.right)continue;
		if(iNum==0)iNum=sRect.left;
		if(iNum>=sRect.right)
		{
			iRet = sRect.left;
		}
		else{
			iRet = sRect.left;

			if(pData->uSelectedItem)						// Problem bei ausgew?lten leeren Eintr?en
				if(pData->uSelectedSub+1==uPos)
				{
					UpdateRect(pData,pData->uSelectedItem,pData->uSelectedSub);
				}

				if(pData->uTrackedItem)
					if(pData->uTrackedSub+1==uPos)
					{
						UpdateRect(pData,pData->uTrackedItem,pData->uTrackedSub);
					}
		}


		pData->iColumnXpos[uPos] = sRect.right;
		break;
	}


	for(;uPos<pData->uColumnCount;)
	{
		Header_GetItemRect(hHeader,uPos,&sRect);

		uPos++;
		sRect.right--;
		pData->iColumnXpos[uPos] = sRect.right;
	}

	pData->iColumnXpos[pData->uColumnCount+1] = pData->uSizeX;



	return iRet;
}


//*****************************************************************************
//*
//*		TreeListToggleItem
//*
//*****************************************************************************
//	Klappt bei einem Eintrag die Kinder um, und schickt alle Notify-Nachrichten.
//	pData		: Zeiger auf die Fensterdaten
//	uItem		: Ist die Nummer des Eintrages
//	uAddFlags	: Sind die State-Flags die hinzugef?t werden sollen
//	Ergibt  -1 = Fehler
//			 0 = Ausgef?rt
//			 1 = Abbruch
static int TreeListToggleItem(TreeListData *pData,unsigned uItem,unsigned uAddFlags)
{
	NMTREEVIEW	sNotify;
	BaseItem  **pList;
	BaseItem   *pEntry;
	BaseItem   *pTemp;
	unsigned	uLevel;
	unsigned	uMask;
	unsigned	uBits;
	unsigned	uNext;
	LRESULT		lRet;



	if(uItem>pData->uTreeItemsMax)return 0;

	pList  = pData->pTreeItems;
	pEntry = pList[uItem];
	if(!pEntry)return -1;


	UNLOCK(pData);

	uBits						= pEntry->uState&TVIS_EXPANDEDONCE;
	uMask						=(pEntry->uState&TVIS_EXPANDED)? ~(TVIS_EXPANDEDONCE|TVIS_EXPANDED):~0;
	sNotify.hdr.code			= TVN_ITEMEXPANDING;
	sNotify.action				= (pEntry->uState&TVIS_EXPANDED)? TVE_COLLAPSE:TVE_EXPAND;
	sNotify.itemNew.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE;
	sNotify.itemNew.hItem		= (HTREEITEM)uItem;
	sNotify.itemNew.stateMask	= 0xFFFFFFFF;
	sNotify.itemNew.state		=(pEntry->uState^TVIS_EXPANDED)&uMask;
	sNotify.itemNew.lParam		= pEntry->lParam;
	sNotify.itemNew.pszText		= (LPTSTR)-1;
	sNotify.itemNew.cchTextMax	= -1;
	sNotify.itemOld.mask		= 0;
	sNotify.ptDrag.x			= 0;
	sNotify.ptDrag.y			= 0;

	lRet=SendNotify(pData,&sNotify.hdr);

	LOCK(pData);


	pEntry = pList[uItem];
	if(!pEntry)return -1;

	if(lRet)return 1;										// User-Abbruch ?


	pList =  pData->pTreeItems;


	pEntry->uState ^=  TVIS_EXPANDED;					// Teile auf/zuklappen
	pEntry->uState &= ~TVIS_EXPANDPARTIAL;
	pEntry->uState |=  uAddFlags;
	if(pEntry->uShowPos)
	{
		if(pEntry->uState&TVIS_EXPANDED)					// Kinderfenster aktuallisieren
		{
			uLevel			=  0;
			uNext			=  pEntry->uFirstChild;

			while(uNext)
			{
				pTemp = pList[uNext];
				pTemp->uShowPos = 0;

				if(pTemp->uFirstChild)
				{
					uNext=pTemp->uFirstChild;
					uLevel++;
					continue;
				}

				if(pTemp->uNextItem)
				{
					uNext=pTemp->uNextItem;
					continue;
				}

				if(uLevel==0)break;

				uNext = pList[pTemp->uParent]->uNextItem;
				uLevel--;
			}
		}

		UpdateItems(pData,uItem);
	}


	UNLOCK(pData);

	sNotify.hdr.code			= TVN_ITEMEXPANDED;
	sNotify.action				= (pEntry->uState&TVIS_EXPANDED)? TVE_COLLAPSE:TVE_EXPAND;
	sNotify.itemNew.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE;
	sNotify.itemNew.hItem		= (HTREEITEM)uItem;
	sNotify.itemNew.stateMask	= 0xFFFFFFFF;
	sNotify.itemNew.state		=(pEntry->uState|uBits)&uMask;
	sNotify.itemNew.lParam		= pEntry->lParam;
	sNotify.itemNew.pszText		= (LPTSTR)-1;
	sNotify.itemNew.cchTextMax	= -1;
	sNotify.itemOld.mask		= 0;
	sNotify.ptDrag.x			= 0;
	sNotify.ptDrag.y			= 0;

	SendNotify(pData,&sNotify.hdr);

	LOCK(pData);

	pEntry = pData->pTreeItems[uItem];
	if(!pEntry)return -1;


	if(pData->uSelectedItem)								// Ist der ausgew?lten Eintrag sichtbar ?
	{
		pEntry = pList[pData->uSelectedItem];
		if(!pEntry)
		{
			pData->uSelectedItem = 0;
			pData->uSelectedSub	 = 0;
		}
		else if(!pEntry->uShowPos)
		{
			while(!pEntry->uShowPos)
			{
				uItem  = pEntry->uParent;
				pEntry = pList[uItem];
			}

			TreeListSelectItem(pData,uItem,pData->uSelectedSub,TVC_UNKNOWN);
		}
	}


	return 0;
}

//*****************************************************************************
//*
//*		TreeListGetItemRect
//*
//*****************************************************************************
//	Zeichnet einen Eintrag neu
//	pData		: Zeiger auf die Fensterdaten
//	uItem		: Ist die Nummer des Eintrages
//	uFlags		: Bit 0   : 0=volle Zeile  1=nur Text
//				  Bit 7   : 1=nur Spalte
//				  Bit 8.. : Spaltennummer
//	Ergibt 1 wenn der Eintrag sichtbar war
static int TreeListGetItemRect(TreeListData *pData,unsigned uItem,unsigned uFlags,RECT *pRect)
{
	ExtraItem  *pExtra;
	BaseItem   *pEntry;
	unsigned	uPos;
	unsigned	uSub;



	if(uItem>pData->uTreeItemsMax)
	{
		memset(pRect,0,sizeof(RECT));
		return 0;
	}

	pEntry = pData->pTreeItems[uItem];
	if(!pEntry->uShowPos)									// Ist der Eintrag aufgeklappt
	{
		memset(pRect,0,sizeof(RECT));
		return 0;
	}

	uPos = pEntry->uShowPos-pData->uScrollY-1;
	if(uPos>=pData->uMaxEnties)								// Eintrag im Fenstersichtbar
	{
		memset(pRect,0,sizeof(RECT));
		return 0;
	}

	pRect->top     = pData->uStartPixel;
	pRect->top    += pData->iRowHeight*uPos;
	pRect->bottom  = pData->iRowHeight+pRect->top;

	if((uFlags&0xFE)==TVIR_GETCOLUMN)						// Nur Spalten
	{
		uSub = uFlags>>24;
		if(uSub>=pData->uColumnCount)uSub=0;

		pRect->left    = pData->iColumnXpos[uSub  ];
		pRect->left   -= pData->uScrollX;
		pRect->right   = pData->iColumnXpos[uSub+1];
		pRect->right  -= pData->uScrollX;
	}
	else{
		uSub		   = 0;
		pRect->left    = 0;
		pRect->left   -= pData->uScrollX;
		pRect->right   = pData->uSizeX;
	}


	if(uFlags&TVIR_TEXT)									// Nur Text augeben
	{
		if(uSub>0)
		{
			pExtra =  pData ->pExtraItems[uSub-1][uItem];

			if(pData->bColumnEdit[uSub]==TVAX_CHECK)
			{
				pRect->left+=pData->iChecksXsize;
				if(pRect->left>pRect->right)pRect->left=pRect->right;
			}
			else if(pExtra && pExtra->bFlags&TVIX_HASIMAGE)
			{
				pRect->left+=pData->iImagesXsize;
				if(pRect->left>pRect->right)pRect->left=pRect->right;
			}
		}
		else{
			if(pData->cHasRootRow)							// Root-Linien ausgleichen
			{
				pRect->left += pData->iIndent;
			}

			pRect->left += pData->iIndent*pEntry->uLevel;

			if(pData->hStates)
			{
				pRect->left += pData->iStatesXsize;
			}

			if(!(pData->uStyle&TVS_HASLINES))
			{
				pRect->left -= 1;
			}

			if(pData->uStyleEx&TVS_EX_ITEMLINES)
			{
				pRect->left += 1;
				if(pEntry->bFlags&TVIX_HASIMAGE)pRect->left++;
			}

			if(pEntry->bFlags&TVIX_HASIMAGE)
			{
				pRect->left += pData->iImagesXsize;
			}

			if(pRect->left>pRect->right)
			{
				pRect->left=pRect->right;
			}
		}
	}



	return 1;
}

//*****************************************************************************
//*
//*		TreeListEnsureVisible
//*
//*****************************************************************************
//	Macht einen Eintrag sichtbar
//	pData		: Zeiger auf die Fensterdaten
//	uItem		: Ist die Nummer des Eintrages
//	uSub		: Untereintrag der sichtbar sein soll
//						0xFFFFFFFF nur Zeile
//						FIRST_LINE als oberster Eintrag
//	Ergibt 1 wenn nur zum Eintrag gescrollt wurde bzw. 0 wenn aufgeklapt wurde
static int TreeListEnsureVisible(TreeListData *pData,unsigned uItem,unsigned uSub)
{
	BaseItem   *pEntry;
	BaseItem   *pTemp;
	unsigned	uTemp;
	unsigned	uPos;
	int			iNum;
	int			iAnf;
	int			iOff;
	int			iEnd;
	int			iMax;
	int			iRet;



	if(uItem>pData->uTreeItemsMax)return -1;

	pEntry = pData->pTreeItems[uItem];
	if(!pEntry)return -1;

	uPos = pEntry->uShowPos;
	if(!uPos)												// Zweige aufklappen wenn Eintrag zugeklappt
	{
		iRet = 0;

		for(pTemp=pEntry;;)
		{
			uTemp = pTemp->uParent;
			pTemp = pData->pTreeItems[uTemp];
			if(!pTemp)break;
			if((pTemp->uState&TVIS_EXPANDED)==0)
			{
				if(TreeListToggleItem(pData,uTemp,0))return 0;
			}
		}

		pEntry = pData->pTreeItems[uItem];
		if(!pEntry)return 0;

		uPos = pEntry->uShowPos;
		if(!uPos)return 0;
	}
	else{													// Nur Scrollen
		iRet = 1;
	}


	uPos--;
	if(uPos<pData->uScrollY)								// Vor erster Zeile
	{
		pData->uScrollY	= uPos;
		SetScrollPos(pData->hWnd,SB_VERT,uPos,TRUE);
		UpdateView(pData);
	}
	else if(uSub==FIRST_LINE)								// Als ersten Eintrag
	{
		if(uPos!=pData->uScrollY)
		{
			pData->uScrollY	= uPos;
			SetScrollPos(pData->hWnd,SB_VERT,uPos,TRUE);
			UpdateView(pData);
		}

		return iRet;
	}
	else if(uPos>=pData->uScrollY+pData->uPageEnties)		// Nach letzter Zeile
	{
		iOff  = uPos-(pData->uPageEnties-1);
		iMax  = pData->uItemPosCount;
		iMax -=	pData->uPageEnties-1;

		if(iOff>=iMax)iOff=iMax;
		if(iOff<0)iOff=0;
		if(iOff!=(int)pData->uScrollY)
		{
			pData->uScrollY	= iOff;
			SetScrollPos(pData->hWnd,SB_VERT,iOff,TRUE);
			UpdateView(pData);
		}
	}


	if(uSub<pData->uColumnCount)							// Horizontal einrichten
	{
		iNum  = pData->uSizeX;
		iOff  = pData->uScrollX;
		iAnf  = pData->iColumnXpos[uSub  ];
		iEnd  = pData->iColumnXpos[uSub+1];

		if(iOff+iNum< iAnf)iOff=iAnf;
		if(iOff     >=iEnd)iOff=iAnf;
		if(iOff+iNum< iEnd)iOff=iEnd-iNum;
		if(iOff     > iAnf)iOff=iAnf;

		iMax  = pData->iColumnXpos[pData->uColumnCount];
		iMax -= pData->uSizeX/2;

		if(iOff>iMax)iOff=iMax;
		if(iOff<   0)iOff=0;
		if(iOff!=(int)pData->uScrollX)
		{
			pData->uScrollX	= iOff;
			SetScrollPos(pData->hWnd,SB_HORZ,iOff,TRUE);
			UpdateView(pData);
			MoveWindow(pData->hHeader,-iOff,0,iNum+iOff,pData->uStartPixel,TRUE);
		}
	}


	return iRet;
}

//*****************************************************************************
//*
//*		TreeListDeleteItem
//*
//*****************************************************************************
//	L?cht einen Eintrag aus dem Fenster
//	pData		: Zeiger auf die Fensterdaten
//	uItem		: Ist die Nummer des Eintrages der gel?cht werden soll
//	iUpdateList	: Soll die Anzeigeliste aktualisiert werden
//	Ergibt 1 wenn der Eintrag gel?cht wurde
static int TreeListDeleteItem(TreeListData *pData,unsigned uItem,int iUpdateList)
{
	NMTREEVIEW	sNotify;
	ExtraItem **pList;
	ExtraItem  *pExtra;
	BaseItem   *pEntry;
	BaseItem   *pTemp;
	unsigned	uPos;
	int			iOff;
	int			iMax;


	if(pData->cLockChanges)return 0;

	if(uItem>pData->uTreeItemsMax)							// Pr?e den Eintrag
	{
		if(uItem != U(TVI_ROOT))return 0;					// Alles l?chen
		if(pData->uLastChild==0)return 0;

		while(pData->uLastChild)
		{
			TreeListDeleteItem(pData,pData->uLastChild,0);
		}

		pData->uItemPosCount = 0;

		UpdateScrollY(pData);
		UpdateView   (pData);

		return 1;
	}

	pEntry = pData->pTreeItems[uItem];
	if(!pEntry)												// Pr?e den Eintrag
	{
		if(uItem!=NULL)return 0;							// Alles l?chen
		if(pData->uLastChild==0)return 0;

		while(pData->uLastChild)
		{
			TreeListDeleteItem(pData,pData->uLastChild,0);
		}

		pData->uItemPosCount = 0;

		UpdateScrollY(pData);
		UpdateView   (pData);

		return 1;
	}


	while(pEntry->uLastChild)								// Alle Kinder l?chen
	{
		TreeListDeleteItem(pData,pEntry->uLastChild,0);
	}


	if(uItem==pData->uSelectedItem)							// Einen ausgew?lten Eintrag l?chen
	{
		sNotify.hdr.code			= TVN_SELCHANGED;
		sNotify.action				= TVC_UNKNOWN;
		sNotify.itemOld.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM;
		sNotify.itemOld.hItem		= (HTREEITEM)uItem;
		sNotify.itemOld.stateMask	= 0xFFFFFFFF;
		sNotify.itemOld.state		= pEntry->uState&~TVIS_SELECTED;
		sNotify.itemOld.cChildren	= 0;
		sNotify.itemOld.pszText		= (LPTSTR)-1;
		sNotify.itemOld.cchTextMax	= -1;
		sNotify.itemNew.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM;
		sNotify.itemNew.hItem		= NULL;
		sNotify.itemNew.stateMask	= 0xFFFFFFFF;
		sNotify.itemNew.state		= 0;
		sNotify.itemNew.cChildren	= 0;
		sNotify.itemNew.pszText		= (LPTSTR)-1;
		sNotify.itemNew.cchTextMax	= -1;
		sNotify.ptDrag.x			= 0;
		sNotify.ptDrag.y			= 0;

		UNLOCK(pData);
		SendNotify(pData,&sNotify.hdr);						// Bekanndgeben das der Eintrag nicht mehr ausgew?lt ist
		LOCK(pData);

		pData->uSelectedItem = 0;
		pData->uSelectedSub  = 0;
	}


	sNotify.hdr.code			= TVN_DELETEITEM;
	sNotify.itemNew.mask		= 0;
	sNotify.itemOld.mask		= TVIF_HANDLE|TVIF_PARAM;
	sNotify.itemOld.hItem		= (HTREEITEM)uItem;
	sNotify.itemOld.lParam		= pEntry->lParam;
	sNotify.itemOld.pszText		= (LPTSTR)-1;
	sNotify.itemOld.cchTextMax	= -1;
	sNotify.ptDrag.x			= 0;
	sNotify.ptDrag.y			= 0;

	UNLOCK(pData);
	SendNotify(pData,&sNotify.hdr);
	LOCK(pData);

	pEntry = pData->pTreeItems[uItem];					// Pr?en ob der Eintrag noch existiert
	if(!pEntry)return 0;



	if(uItem==pData->uTrackedItem)							// Einen unterstrichenen Eintrag l?chen
	{
		pData->uTrackedItem  = 0;
		pData->uTrackedSub   = 0;
	}

	if(pData->uInsertMark==uItem)
	{
		pData->uInsertMark = 0;
	}


	if(pEntry->uPrevItem)									// Gibt es einen vorherigen Eintrag
	{
		pTemp			 = pData->pTreeItems[pEntry->uPrevItem];
		pTemp->uNextItem = pEntry->uNextItem;
	}
	else{
		if(pEntry->uParent)									// Neues erstes Kind in Elterneintrag
		{
			pTemp			   = pData->pTreeItems[pEntry->uParent];
			pTemp->uFirstChild = pEntry->uNextItem;
		}
		else{
			pData->uFirstChild = pEntry->uNextItem;
		}
	}

	if(pEntry->uNextItem)									// Gibt es einen vorherigen Eintrag
	{
		pTemp			 = pData->pTreeItems[pEntry->uNextItem];
		pTemp->uPrevItem = pEntry->uPrevItem;
	}
	else{
		if(pEntry->uParent)									// Neues letztes Kind in Elterneintrag
		{
			pTemp			  = pData->pTreeItems[pEntry->uParent];
			pTemp->uLastChild = pEntry->uPrevItem;

			if(pTemp->uFirstChild==0 && pTemp->uLastChild==0)
			{
				pTemp->bFlags &= ~TVIX_HASBUTTON;
			}
		}
		else{
			pData->uLastChild = pEntry->uPrevItem;
		}
	}


	for(uPos=1;uPos<pData->uColumnCount;uPos++)				// Alle Extraeintr?e l?chen
	{
		pList = pData->pExtraItems[uPos-1];

		pExtra = pList[uItem];
		if(!pExtra)continue;

		pList[uItem] = NULL;

		if(pExtra->pText)
		{
			pExtra->uTextSize=0;
			delete pExtra->pText;
		}

		delete pExtra;
	}


	pData->pTreeItems[uItem] = NULL;						// Den Eintrag l?chen

	if(pEntry->pText)
	{
		pEntry->uTextSize=0;
		delete pEntry->pText;
	}


	if(iUpdateList)											// Den Eintrag neuzeichen
	{
		uItem = pEntry->uPrevItem;
		if(!uItem)uItem=pEntry->uParent;

		if(pEntry->uShowPos)
		{
			UpdateItems(pData,uItem);
		}
	}



	if(pEntry->uState&TVIS_SELECTED)						// Ausgew?lte Eintr?e runterz?len
		if(pData->uSelectedCount>0)
		{
			pData->uSelectedCount--;
		}

		delete pEntry;

		pData->uTreeItemsCount--;


		iOff  = pData->uScrollY;								// Pr?e die Scrollposition
		iMax  = pData->uItemPosCount;
		iMax -=	pData->uPageEnties-1;

		if(iOff>=iMax)iOff=iMax;
		if(iOff<0)iOff=0;
		if(iOff!=(int)pData->uScrollY)
		{
			pData->uScrollY	= iOff;
			SetScrollPos(pData->hWnd,SB_VERT,iOff,TRUE);
			UpdateView(pData);
		}



		return 1;
}



//*****************************************************************************
//*
//*		TreeListXorSelectItem
//*
//*****************************************************************************
//	W?lt einen Eintrag ab bzw. an
//	pData		: Zeiger auf die Fensterdaten
//	uItem		: Ist die Nummer des Eintrages der ausgew?lt werden soll
//	iMode		: Ist der Grund f? die Änderung
//						TVC_BYKEYBOARD
//						TVC_BYMOUSE
//						TVC_UNKNOWN
//	Ergibt		1 wenn der Eintrag ab/angew?lt wurde
//				0 wenn der Eintrag nicht ver?dert wurde
static int TreeListXorSelectItem(TreeListData *pData,unsigned uItem,int iMode)
{
	NMTREEVIEW	sNotify;
	BaseItem   *pEntry;
	unsigned	uRet;



	pEntry = pData->pTreeItems[uItem];
	if(!pEntry)return 0;
	if(uItem==pData->uSelectedItem)return 0;

	sNotify.hdr.code			= TVN_SELCHANGING;
	sNotify.action				= iMode;
	sNotify.itemNew.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM;
	sNotify.itemNew.hItem		= (HTREEITEM)uItem;
	sNotify.itemNew.stateMask	= 0xFFFFFFFF;
	sNotify.itemNew.state		= pEntry->uState^TVIS_SELECTED;
	sNotify.itemNew.cChildren	= 0;
	sNotify.itemNew.pszText		= (LPTSTR)-1;
	sNotify.itemNew.cchTextMax	= -1;
	sNotify.itemOld.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM;
	sNotify.itemOld.hItem		= NULL;
	sNotify.itemOld.stateMask	= 0xFFFFFFFF;
	sNotify.itemOld.state		= 0;
	sNotify.itemOld.cChildren	= 0;
	sNotify.itemOld.pszText		= (LPTSTR)-1;
	sNotify.itemOld.cchTextMax	= -1;
	sNotify.ptDrag.x			= 0;
	sNotify.ptDrag.y			= 0;

	UNLOCK(pData);
	uRet=SendNotify(pData,&sNotify.hdr);
	LOCK(pData);

	if(uRet)return 0;

	pEntry = pData->pTreeItems[uItem];
	if(!pEntry)return 0;

	sNotify.hdr.code			= TVN_SELCHANGED;
	sNotify.action				= iMode;
	sNotify.itemNew.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM;
	sNotify.itemNew.hItem		= (HTREEITEM)uItem;
	sNotify.itemNew.stateMask	= 0xFFFFFFFF;
	sNotify.itemNew.state		= pEntry->uState^TVIS_SELECTED;
	sNotify.itemNew.cChildren	= 0;
	sNotify.itemNew.pszText		= (LPTSTR)-1;
	sNotify.itemNew.cchTextMax	= -1;
	sNotify.itemOld.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM;
	sNotify.itemOld.hItem		= NULL;
	sNotify.itemOld.stateMask	= 0xFFFFFFFF;
	sNotify.itemOld.state		= 0;
	sNotify.itemOld.cChildren	= 0;
	sNotify.itemOld.pszText		= (LPTSTR)-1;
	sNotify.itemOld.cchTextMax	= -1;
	sNotify.ptDrag.x			= 0;
	sNotify.ptDrag.y			= 0;

	UNLOCK(pData);
	SendNotify(pData,&sNotify.hdr);
	LOCK(pData);

	pEntry = pData->pTreeItems[uItem];
	if(!pEntry)return 0;
	pEntry->uState ^= TVIS_SELECTED;
	if( pEntry->uShowPos)
	{
		if(pData->uStyleEx&TVS_EX_FULLROWMARK)
			UpdateRow (pData,uItem);
		else	UpdateRect(pData,uItem,0);
	}

	if(pEntry->uState&TVIS_SELECTED)
		pData->uSelectedCount++;
	else	pData->uSelectedCount--;




	return 1;
}


//*****************************************************************************
//*
//*		TreeListRemoveFocus
//*
//*****************************************************************************
//	W?lt den Focus ab
//	pData		: Zeiger auf die Fensterdaten
static void TreeListRemoveFocus(TreeListData *pData)
{
	ExtraItem  *pExtra;
	BaseItem   *pEntry;
	unsigned	uItem;
	unsigned	uSub;


	if(!pData->uFocusItem)return;

	uItem  = pData->uFocusItem;
	pEntry = pData->pTreeItems[uItem];

	if(!pEntry)return;

	pEntry->bFlags &= ~TVIX_FOCUSED;

	uSub = pData->uFocusSub;

	if(uSub)
	{
		pExtra= pData->pExtraItems[uSub-1][uItem];
		if(pExtra)pExtra->bFlags &= ~TVIX_FOCUSED;
	}

	UpdateRect(pData,uItem,uSub);

	pData->uFocusItem = 0;
	pData->uFocusSub  = 0;

}

//*****************************************************************************
//*
//*		TreeListSelectItem
//*
//*****************************************************************************
//	W?lt einen Eintrag aus
//	pData		: Zeiger auf die Fensterdaten
//	uItem		: Ist die Nummer des Eintrages der ausgew?lt werden soll
//	uSubItem	: Ist die Spalte die gew?lt werden soll
//	iMode		: Ist der Grund f? die Änderung
//						TVC_BYKEYBOARD
//						TVC_BYMOUSE
//						TVC_UNKNOWN
//	Ergibt		2 wenn der Eintrag gew?lt und umgeklapt wurde
//				1 wenn der Eintrag gew?lt wurde
//				0 wenn der Eintrag nicht gew?lt wurde
static int TreeListSelectItem(TreeListData *pData,unsigned uItem,unsigned uSubItem,int iMode)
{
	NMTREEVIEW	sNotify;
	ExtraItem  *pExtra;
	BaseItem   *pEntry;
	BaseItem   *pTemp;
	unsigned	uState;
	unsigned	uStOld;
	unsigned	uNext;
	unsigned	uPos;
	unsigned	uOld;
	unsigned	uSub;
	unsigned	uRet;
	int			iDel;



	uOld = pData->uSelectedItem;
	uSub = pData->uSelectedSub;


	if(uSubItem>=pData->uColumnCount && uSubItem>0)return 0;
	if(uItem   > pData->uTreeItemsMax)return 0;
	if(uItem   ==uOld)
		if(uSubItem==uSub)
			if(pData->uSelectedCount<=1 || !(pData->uStyleEx&TVS_EX_MULTISELECT))
			{
				return 1;
			}


			if(pData->uStyleEx&TVS_EX_MULTISELECT)					// Ist die Mehrfachauswahl m?lich
			{
				iDel = iMode&TVC_DESELECT;
				if(!iDel)
				{
					if(pData->uStyleEx&TVS_EX_FULLROWMARK)
						UpdateRow (pData,uOld);
					else	UpdateRect(pData,uOld,uSub);

					uOld = 0;
					uSub = 0;
				}
				else{												// Alle gew?lten Eintr?e abw?len
					if(pData->uSelectedCount>1 && pData->uTreeItemsMax)
					{
						for(uPos=pData->uTreeItemsMax;uPos;uPos--)
						{
							pEntry = pData->pTreeItems[uPos];
							if(!pEntry || !(pEntry->uState&TVIS_SELECTED))continue;
							if(TreeListXorSelectItem(pData,uPos,iMode))
								if(!pData->uSelectedCount)break;		// Wurden alle Eintr?e abgew?lt
						}
					}
				}
			}
			else{
				iDel = 1;
			}

			iMode &= ~TVC_DESELECT;


			pEntry = pData->pTreeItems[uItem];
			if(!pEntry)												// Neuen Statatus holen
			{
				if(uItem)return 0;
				uState = 0;
			}
			else{
				uState = pEntry->uState|TVIS_SELECTED;

				if(uSubItem)
				{
					uState &= TVIS_BASEFLAGS;
					pExtra  = pData->pExtraItems[uSubItem-1][uItem];
					if(pExtra)uState|=pExtra->uState;
				}
			}

			pTemp = pData->pTreeItems[uOld];
			if(!pTemp)												// Alten Status holen
			{
				uStOld = 0;
			}
			else{
				uStOld = pTemp->uState|TVIS_SELECTED;

				if(uSub)
				{
					uStOld &= TVIS_BASEFLAGS;
					pExtra  = pData->pExtraItems[uSub-1][uOld];
					if(pExtra)uStOld|=pExtra->uState;
				}
			}

			sNotify.hdr.code			= TVN_SELCHANGING;
			sNotify.action				= iMode;
			sNotify.itemNew.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM;
			sNotify.itemNew.hItem		= (HTREEITEM)uItem;
			sNotify.itemNew.stateMask	= 0xFFFFFFFF;
			sNotify.itemNew.state		= uState;
			sNotify.itemNew.cChildren	= uSubItem;
			sNotify.itemNew.pszText		= (LPTSTR)-1;
			sNotify.itemNew.cchTextMax	= -1;
			sNotify.itemOld.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM;
			sNotify.itemOld.hItem		= (HTREEITEM)uOld;
			sNotify.itemOld.stateMask	= 0xFFFFFFFF;
			sNotify.itemOld.state		= uStOld;
			sNotify.itemOld.cChildren	= uSub;
			sNotify.itemOld.pszText		= (LPTSTR)-1;
			sNotify.itemOld.cchTextMax	= -1;
			sNotify.ptDrag.x			= 0;
			sNotify.ptDrag.y			= 0;


			UNLOCK(pData);

			if(SendNotify(pData,&sNotify.hdr))						// Abfragen ob der Eintrag gew?lt werden darf
			{
				LOCK(pData);
				return 0;
			}

			LOCK(pData);

			if(uItem)												// Pr?en ob der Eintrag noch existiert
			{
				pEntry = pData->pTreeItems[uItem];
				if(!pEntry)return 0;
			}


			if(iDel)
			{
				uOld    = pData->uSelectedItem;
				pTemp	= pData->pTreeItems[uOld];
			}


			if(pTemp)												// Den alten Eintrag abw?len
			{
				if(pTemp->uShowPos)									// Den Eintrag neu zeichnen
				{
					if(pData->uStyleEx&TVS_EX_FULLROWMARK)
						UpdateRow (pData,uOld);
					else	UpdateRect(pData,uOld,uSub);
				}

				if(pTemp->uState&TVIS_SELECTED)
				{
					pTemp->uState &= ~TVIS_SELECTED;
					pData->uSelectedCount -= 1;
				}

				pData->uSelectedSub    = 0;
				pData->uSelectedItem   = 0;
			}
			else{
				uOld  = 0;
			}


			if(uItem)												// Den neuen Eintrag w?len
			{
				if(uSubItem && uSubItem<pData->uColumnCount)
				{
					pExtra = pData->pExtraItems[uSubItem-1][uItem];
					if(!pExtra)
					{
						pExtra  = new ExtraItem;
						memset(pExtra,0,sizeof(ExtraItem));
						pExtra->iImage = TV_NOIMAGE;
						pExtra->uState = pEntry->uState&(TVIS_BOLD|TVIS_UNTERLINE);
						pData->pExtraItems[uSubItem-1][uItem] = pExtra;
					}

					uState  = pExtra->uState;
					uState |= pEntry->uState&TVIS_BASEFLAGS;
				}
				else{
					uState  = pEntry->uState;
				}

				if(pEntry->uShowPos)								// Den Eintrag neu zeichnen
				{
					if(pData->uStyleEx&TVS_EX_FULLROWMARK)
						UpdateRow (pData,uItem);
					else	UpdateRect(pData,uItem,uSubItem);
				}

				if(!(pEntry->uState&TVIS_SELECTED))
				{
					pEntry->uState |= TVIS_SELECTED;
					pData->uSelectedCount += 1;
				}

				pData->uSelectedSub    = uSubItem;
				pData->uSelectedItem   = uItem;
			}
			else{
				pData->uSelectedItem = 0;
				pData->uSelectedSub  = 0;
				uState				 = 0;
			}


			sNotify.hdr.code			= TVN_SELCHANGED;
			sNotify.action				= iMode;
			sNotify.itemNew.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM;
			sNotify.itemNew.hItem		= (HTREEITEM)uItem;
			sNotify.itemNew.stateMask	= 0xFFFFFFFF;
			sNotify.itemNew.state		= uState;
			sNotify.itemNew.cChildren	= uSubItem;
			sNotify.itemNew.pszText		= (LPTSTR)-1;
			sNotify.itemNew.cchTextMax	= -1;
			sNotify.itemOld.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBITEM;
			sNotify.itemOld.hItem		= (HTREEITEM)uOld;
			sNotify.itemOld.stateMask	= 0xFFFFFFFF;
			sNotify.itemOld.state		= uStOld;
			sNotify.itemOld.cChildren	= uSub;
			sNotify.itemOld.pszText		= (LPTSTR)-1;
			sNotify.itemOld.cchTextMax	= -1;
			sNotify.ptDrag.x			= 0;
			sNotify.ptDrag.y			= 0;

			UNLOCK(pData);
			SendNotify(pData,&sNotify.hdr);
			LOCK(pData);


			if(!(pData->uStyle&TVS_SINGLEEXPAND))					// Einzelmodus aktiv
			{
				if(pData->uStyle&TVS_SHOWSELALWAYS)
					if(pData->uSelectedItem)
					{
						TreeListEnsureVisible(pData,pData->uSelectedItem,pData->uSelectedSub);
					}

					return 1;
			}


			//*****************************************************************************



			sNotify.hdr.code			= TVN_SINGLEEXPAND;
			sNotify.action				= iMode;
			sNotify.itemNew.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE;
			sNotify.itemNew.hItem		= (HTREEITEM)uItem;
			sNotify.itemNew.stateMask	= 0xFFFFFFFF;
			sNotify.itemNew.state		= (pEntry)? pEntry->uState:0;
			sNotify.itemNew.cChildren	= 0;
			sNotify.itemNew.pszText		= (LPTSTR)-1;
			sNotify.itemNew.cchTextMax	= -1;
			sNotify.itemOld.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE;
			sNotify.itemOld.hItem		= (HTREEITEM)uOld;
			sNotify.itemOld.stateMask	= 0xFFFFFFFF;
			sNotify.itemOld.state		= (pTemp)? pTemp->uState:0;
			sNotify.itemOld.cChildren	= 0;
			sNotify.itemOld.pszText		= (LPTSTR)-1;
			sNotify.itemOld.cchTextMax	= -1;
			sNotify.ptDrag.x			= 0;
			sNotify.ptDrag.y			= 0;

			UNLOCK(pData);
			uRet=SendNotify(pData,&sNotify.hdr);					// Anfragen ob die Zweige umgeklappt werden d?fen
			LOCK(pData);


			pTemp	= pData->pTreeItems[uOld ];						// Zeiger neu holen falls es Änderungen gab
			pEntry	= pData->pTreeItems[uItem];



			while(pTemp && pEntry)									// Beide Zweige sysnchronisieren
			{
				if(pEntry->uLevel>pTemp->uLevel)
				{
					uNext  = pEntry->uParent;

					if(!(uRet&TVNRET_SKIPNEW))
						if(!(pEntry->uState&TVIS_EXPANDED))
						{
							TreeListToggleItem(pData,uItem,0);
						}

						pEntry = pData->pTreeItems[uNext];
						uItem  = uNext;

						if(!uItem)break;

						continue;
				}

				if(uItem==uOld)goto EndSel;							// Bis zum gleichen Knoten

				uNext  = pTemp->uParent;

				if(!(uRet&TVNRET_SKIPOLD))
					if(pTemp->uState&TVIS_EXPANDED)
					{
						TreeListToggleItem(pData,uOld,0);
					}

					pTemp  = pData->pTreeItems[uNext];
					uOld   = uNext;
			}

			if(!uItem)
			{
				if(!(uRet&TVNRET_SKIPOLD))
					while(pTemp)										// Alten Zweig zuklappen
					{
						uNext = pTemp->uParent;

						if(pTemp->uState&TVIS_EXPANDED)
						{
							TreeListToggleItem(pData,uOld,0);
						}

						pTemp = pData->pTreeItems[uNext];
						uOld  = uNext;
					}

					goto EndSel;
			}

			if(!uOld)
			{
				if(!(uRet&TVNRET_SKIPNEW))
					while(pEntry)										// Neuen Zweig aufklappen
					{
						uNext = pEntry->uParent;

						if(!(pEntry->uState&TVIS_EXPANDED))
						{
							TreeListToggleItem(pData,uItem,0);
						}

						pEntry = pData->pTreeItems[uNext];
						uItem  = uNext;
					}
			}

EndSel:

			if(pData->uStyle&TVS_SHOWSELALWAYS)
				if(pData->uSelectedItem)
				{
					TreeListEnsureVisible(pData,pData->uSelectedItem,pData->uSelectedSub);
				}


				return 2;
}

//*****************************************************************************
//*
//*		TreeListInsertItem
//*
//*****************************************************************************
//	F?t einen Eintrag ins Fenster ein
//	pData		: Zeiger auf die Fensterdaten
//	pInsert		: Zeiger auf die ein zu f?enden Daten
//	Ergibt die Einf?eposition des neuen Eintrages oder 0 bei einem Fehler
static unsigned TreeListInsertItem(TreeListData *pData,TV_INSERTSTRUCT *pInsert)
{
	char	   *pTemp;
	BYTE		bFlag;
	LPCTSTR	    pText;
	LPCTSTR	    pTextTemp;
	ExtraItem **pExOld[MAX_COLUMNS];
	ExtraItem **pExNew[MAX_COLUMNS];
	BaseItem   *pNew;
	BaseItem  **pOld;
	BaseItem  **pItems;
	BaseItem   *pEntry;
	BaseItem   *pParent;
	unsigned   *pPosNew;
	unsigned   *pPosOld;
	unsigned   *pFirst;
	unsigned   *pLast;
	unsigned	uBefore;
	unsigned	uParent;
	unsigned	uAfter;
	unsigned	uFirst;
	unsigned	uSize;
	unsigned	uBits;
	unsigned	uItem;
	unsigned	uNext;
	unsigned	uMax;
	unsigned	uPos;
	unsigned	uNum;
	int			iCmp;
	int			iNone;
	int			iCount;
	int			iShift;


	if(pData->cLockChanges)return 0;


	uParent=U(pInsert->hParent);
	if(uParent>pData->uTreeItemsMax)						// Pr?e das Elternelement
	{
		if(pInsert->hParent!=TVI_ROOT)
		{
			return 0;
		}

		pParent = NULL;
	}
	else{
		pParent = pData->pTreeItems[uParent];
		if(!pParent)
		{
			if(uParent)return 0;
			pParent = NULL;
		}
	}



	if(pData->uTreeItemsCount+1>pData->uTreeItemsMax)		// Größe der Liste erh?en
	{
		pPosOld = pData->pItemPos;
		pOld	= pData->pTreeItems;
		uMax	= pData->uTreeItemsMax;
		uMax   += pData->uTreeItemsMax/2;
		uMax   += 64;
		pItems  = new BaseItem*[uMax+1];

		if(!pItems)
		{
			return 0;
		}

		pPosNew = new unsigned[uMax];
		if(!pPosNew)
		{
			delete pItems;
			return 0;
		}

		for(uPos=1;uPos<pData->uColumnCount;uPos++)
		{
			pExOld[uPos] = pData->pExtraItems[uPos-1];
			pExNew[uPos] = new ExtraItem*[uMax+1];

			if(!pExNew[uPos])
			{
				for(uPos--;uPos>0;uPos--)delete pExNew[uPos];
				delete pPosNew;
				delete pItems;
				return 0;
			}
		}

		memcpy(pItems ,pData->pTreeItems       ,sizeof(BaseItem*)*(     pData->uTreeItemsMax+1));
		memset(pItems +pData->uTreeItemsMax+1,0,sizeof(BaseItem*)*(uMax-pData->uTreeItemsMax  ));
		memcpy(pPosNew,pData->pItemPos         ,sizeof(unsigned )*(     pData->uTreeItemsCount));
		memset(pPosNew+pData->uTreeItemsCount,0,sizeof(unsigned )*(uMax-pData->uTreeItemsCount));

		for(uPos=1;uPos<pData->uColumnCount;uPos++)
		{
			memcpy(pExNew[uPos],pExOld[uPos]            ,sizeof(ExtraItem*)*(     pData->uTreeItemsMax+1));
			memset(pExNew[uPos]+pData->uTreeItemsMax+1,0,sizeof(ExtraItem*)*(uMax-pData->uTreeItemsMax  ));
			pData->pExtraItems[uPos-1]=pExNew[uPos];
			delete pExOld[uPos];
		}

		pData->uTreeItemsMax = uMax;
		pData->pTreeItems	 = pItems;
		pData->pItemPos		 = pPosNew;
		delete pPosOld;
		delete pOld;
	}


	//******************** Den neuen Eintrag erzeugen *****************************

	pItems	= pData->pTreeItems;
	uPos	= pData->uNextSeachPos+1;
	pTemp	= new char[sizeof(BaseItem)+pData->uUserDataSize];
	pNew	= (BaseItem*)pTemp;

	if(!pNew)												// Konnte der Speicher reserviert werden
	{
		return 0;
	}

	if(pData->uUserDataSize)								// Die Userdaten auf 0 setzen
	{
		memset(pTemp+sizeof(BaseItem),0,pData->uUserDataSize);
	}

	for(;;uPos++)											// Suche freie Position
	{
		if(uPos>pData->uTreeItemsMax)uPos=1;
		if(pItems[uPos]==NULL)break;
	}

	pData->uNextSeachPos = uPos;

	memset(pNew,0,sizeof(BaseItem));						// Erstelle den neuen Eintrag

	uBits = pInsert->item.mask;

	if(uBits&TVIF_STATE)
	{
		pNew->uState = pInsert->item.state&pInsert->item.stateMask;
	}
	else{
		if(pData->uStyle&TVS_CHECKBOXES)pNew->uState=0x1000;
	}

	if(uBits&TVIF_PARAM)
	{
		pNew->lParam = pInsert->item.lParam;
	}

	if(uBits&TVIF_IMAGE)
	{
		pNew->iImage = pInsert->item.iImage;
		if(pNew->iImage==I_IMAGECALLBACK)pNew->bCallback|=TVIF_IMAGE;
	}

	if(uBits&TVIF_SELECTEDIMAGE)
	{
		pNew->iSelectedImage = pInsert->item.iSelectedImage;
		if(pNew->iSelectedImage==I_IMAGECALLBACK)pNew->bCallback|=TVIF_SELECTEDIMAGE;
	}

	if(uBits&TVIF_CHILDREN)									// Art der Schaltfl?hen
	{
		switch(pInsert->item.cChildren)
		{
		case  0:											break;
		case  1:		pNew->bFlags   |=TVIX_HASBUTTON;	break;
		case  I_CCB:	pNew->bCallback|=TVIF_CHILDREN;		break;
		default:		pNew->bFlags   |=TVIX_VARBUTTON;	break;
		}
	}
	else{
		pNew->bFlags |=TVIX_VARBUTTON;
	}

	if(pData->uStyle&TVS_SINGLEEXPAND)						// Nicht aufklappen bei Einzelmodus
	{
		pNew->uState &= ~TVIS_EXPANDED;
	}

	pNew->uState|=TVIS_EXPANDEDONCE;

	if(uBits&TVIF_TEXT)										// Text einf?en
	{
		if(pInsert->item.pszText==LPSTR_TEXTCALLBACK)
		{
			pNew->bCallback|= TVIF_TEXT;
			pNew->uTextSize = 0;
			pNew->pText		= 0;
		}
		else{
			pNew->uTextSize	= (WORD)str_len(pInsert->item.pszText);
			pNew->pText		= new TCHAR[pNew->uTextSize+1];
			memcpy(pNew->pText,pInsert->item.pszText,sizeof(TCHAR)*(pNew->uTextSize+1));
		}
	}
	else{
		pNew->pText		= new TCHAR[1];
		pNew->pText[0]	= 0;
		pNew->uTextSize	= 0;
	}


	if(!pParent)											// Einen Root-Eintrag einf?en
	{
		pNew->uParent	= 0;
		uParent			= 0;
		bFlag			= 0;
		uFirst			= 0xFFFFFFFF;
		pFirst			= &pData->uFirstChild;
		pLast			= &pData->uLastChild;
	}
	else{													// Einen Tree-Eintrag einf?en
		pNew->uParent	=  uParent;
		pNew->uLevel	=  pParent->uLevel+1;
		uFirst			=  pParent->uFirstChild;
		pFirst			= &pParent->uFirstChild;
		pLast			= &pParent->uLastChild;
		bFlag			=  pParent->bFlags;

		if(pParent->bFlags&TVIX_VARBUTTON)
		{
			pParent->bFlags|=TVIX_HASBUTTON;
		}
	}


	//******************** Eintrage einf?en **************************************


	uAfter = U(pInsert->hInsertAfter);

	switch(uAfter)
	{
	case U(TVI_BEFORE):										// Nach einem Eintrag einf?en

		if(pParent)										// Einen Root-Eintrag einf?en
		{
			pEntry  = pParent;
			pParent ->bFlags  = bFlag;
			uParent = pParent->uParent;
			pParent = pItems  [uParent];

			if(!pParent)
			{
				pNew->uParent	= 0;
				pNew->uLevel	= 0;
				uParent			= 0;
				uFirst			= 0xFFFFFFFF;
				pFirst			= &pData->uFirstChild;
				pLast			= &pData->uLastChild;
			}
			else{										// Einen Tree-Eintrag einf?en
				pNew->uParent	=  uParent;
				pNew->uLevel	=  pParent->uLevel+1;
				uFirst			=  pParent->uFirstChild;
				pFirst			= &pParent->uFirstChild;
				pLast			= &pParent->uLastChild;

				if(pParent->bFlags&TVIX_VARBUTTON)
				{
					pParent->bFlags|=TVIX_HASBUTTON;
				}
			}

			if(pEntry->uPrevItem)
			{
				uAfter = pEntry->uPrevItem;
				goto DoInsert;
			}
		}

	case U(TVI_FIRST):										// Am Anfang einf?en

		if(pFirst[0])									// Gibt es schon Eintr?e
		{
			pEntry = pItems[pFirst[0]];
			pEntry->uPrevItem = uPos;
		}
		else{
			pFirst[0] = uPos;
			pLast [0] = uPos;
			break;
		}

		pNew ->uNextItem = pFirst[0];					// Eintrag einf?en
		pFirst[0]		 = uPos;

		break;


	case U(TVI_ROOT):										// Als Root-Eintrag einf?en

		pNew->uParent	= 0;
		uParent			= 0;
		pFirst			= &pData->uFirstChild;
		pLast			= &pData->uLastChild;


	case U(TVI_LAST):										// Am Ende einf?en

		if(pLast[0])									// Gibt es schon Eintr?e
		{
			pEntry = pItems[pLast[0]];
			pEntry->uNextItem = uPos;
		}
		else{
			pFirst[0] = uPos;
			pLast [0] = uPos;
			break;
		}

		pNew ->uPrevItem = pLast[0];					// Eintrag einf?en
		pLast[0]		 = uPos;


		break;


	case U(TVI_SORT):										// Alphapetisch einf?en

		uItem = pFirst[0];
		if(!uItem)										// Gibt es keine Kindeintr?e
		{
			pFirst[0] = uPos;
			pLast [0] = uPos;
			break;
		}

		if(pNew->bCallback&TVIF_TEXT)					// Text ?er Callback holen
		{
			uSize=1;
			LOCK(pData);
			CallbackEntry(pData,pNew,uPos,TVIF_TEXT,iNone,uSize,pText);
			UNLOCK(pData);
		}
		else{
			pText = pNew->pText;
		}


		pData->cLockChanges=1;

		uNext	= uItem;
		iCount	= 0;
		uBefore = 0;

		while(uNext)									// Z?le die Eintr?e
		{
			iCount++;
			uNext = pItems[uNext]->uNextItem;
		}

		while(iCount>0)
		{											// Binary-Seach Algorithnus
			iShift = iCount/2;
			uNext  = uItem;


			while(iShift>0)
			{
				uNext  = pItems[uNext]->uNextItem;
				iShift--;
			}


			pEntry  = pItems[uNext];
			if(pEntry->bCallback&TVIF_TEXT)				// Text ?er Callback holen
			{
				uSize=0;
				LOCK(pData);
				CallbackEntry(pData,pEntry,uItem,TVIF_TEXT,iNone,uSize,pTextTemp);
				UNLOCK(pData);
			}
			else{
				pTextTemp = pEntry->pText;
			}

			iCmp = str_icmp(pText,pTextTemp);

			if(iCmp<0)
			{
				iCount -= (iCount+1)/2;
				continue;
			}

			if(iCmp>0)
			{
				iCount -= iCount/2+1;
				uBefore = uNext;
				uItem   = pItems[uNext]->uNextItem;
				continue;
			}

			uBefore	= pEntry->uPrevItem;
			uItem	= uNext;
			break;
		}


		pData->cLockChanges=0;

		pNew->uNextItem = uItem;
		pNew->uPrevItem = uBefore;

		if(uBefore)										// Vorherigen Eintrag anpassen
		{
			pEntry = pItems[uBefore];
			pEntry->uNextItem = uPos;
		}
		else{											// Am Anfang einf?en
			pFirst[0] = uPos;
		}

		if(uItem)										// N?hsten Eintrag anpassen
		{
			pEntry = pItems[uItem];
			pEntry->uPrevItem = uPos;
		}
		else{											// Am Ende anh?gen
			pLast[0] = uPos;
		}

		break;


	case U(TVI_AFTER):										// Nach einem Eintrag einf?en

		uAfter  = uParent;

		if(pParent)										// Einen Root-Eintrag einf?en
		{
			pParent ->bFlags  = bFlag;
			uParent = pParent->uParent;
			pParent = pItems  [uParent];

			if(!pParent)
			{
				pNew->uParent	= 0;
				pNew->uLevel	= 0;
				uParent			= 0;
				uFirst			= 0xFFFFFFFF;
				pFirst			= &pData->uFirstChild;
				pLast			= &pData->uLastChild;
			}
			else{										// Einen Tree-Eintrag einf?en
				pNew->uParent	=  uParent;
				pNew->uLevel	=  pParent->uLevel+1;
				uFirst			=  pParent->uFirstChild;
				pFirst			= &pParent->uFirstChild;
				pLast			= &pParent->uLastChild;

				if(pParent->bFlags&TVIX_VARBUTTON)
				{
					pParent->bFlags|=TVIX_HASBUTTON;
				}
			}
		}

	default:												// Hinter einen Eintrag einf?en
DoInsert:
		uItem = pFirst[0];
		if(!uItem)										// Gibt es keine Kindeintr?e
		{
			pFirst[0] = uPos;
			pLast [0] = uPos;
			break;
		}

		if(uAfter>pData->uTreeItemsMax)
		{
			if((uAfter&0xFFF00000)==0xFFE00000)			// In einer genauen Reihe nach Patent einf?en
			{
				uAfter &= 0xFFFFF;

				uItem = pFirst[0];
				if(!uItem)								// Gibt es keine Kindeintr?e
				{
					pFirst[0] = uPos;
					pLast [0] = uPos;
					break;
				}

				if(uAfter==0)							// In die erste Reihe einf?en
				{
					pEntry = pItems[uItem];
					pEntry->uPrevItem = uPos;
					pNew  ->uNextItem = uItem;
					pFirst[0]		  = uPos;
					break;
				}

				uNum    = 1;
				uBefore	= 0;
				// Suche Einf?ereihe
				for(;uItem;uItem=pItems[uItem]->uNextItem)
				{
					uBefore = uItem;

					if(uNum==uAfter)
					{
						uItem = pItems[uItem]->uNextItem;
						break;
					}

					uNum++;
				}

				pNew->uNextItem = uItem;
				pNew->uPrevItem = uBefore;

				if(uBefore)								// Vorherigen Eintrag anpassen
				{
					pEntry = pItems[uBefore];
					pEntry->uNextItem = uPos;
				}
				else{									// Am Anfang einf?en
					pFirst[0] = uPos;
				}

				if(uItem)								// N?hsten Eintrag anpassen
				{
					pEntry = pItems[uItem];
					pEntry->uPrevItem = uPos;
				}
				else{									// Am Ende anh?gen
					pLast[0] = uPos;
				}

				break;
			}

			pEntry  = NULL;
		}
		else{
			pEntry  = pItems[uAfter];
		}


		if(pEntry && uParent==pEntry->uParent)			// Stimmt der Elterneintrag ?
		{
			uItem   = pEntry->uNextItem;
			uBefore = uAfter;
		}
		else{
			uItem   = 0;
			uBefore = pLast[0];
			pEntry  = pItems[uBefore];
		}

		pNew->uNextItem = uItem;
		pNew->uPrevItem = uBefore;

		if(uBefore)										// Vorherigen Eintrag anpassen
		{
			pEntry->uNextItem = uPos;
		}
		else{											// Am Anfang einf?en
			pFirst[0] = uPos;
		}

		if(uItem)										// N?hsten Eintrag anpassen
		{
			pEntry = pItems[uItem];
			pEntry->uPrevItem = uPos;
		}
		else{											// Am Ende anh?gen
			pLast[0] = uPos;
		}

		break;
	}


	pItems[uPos] = pNew;
	pData->uTreeItemsCount++;
	// Die Anzeigezeilen akualisieren
	if(!pParent || !uFirst || (pParent->uState&TVIS_EXPANDED))
	{
		uItem = pNew->uPrevItem;
		if(!uItem)uItem = uParent;

		if(!uItem)UpdateItems(pData,0);
		else{
			pEntry = pItems[uItem];
			if(pEntry && pEntry->uShowPos)UpdateItems(pData,uItem);
		}
	}

	if(pNew->uState&TVIS_SELECTED)							// Den ausgew?lten Eintrag ausw?len
	{
		TreeListSelectItem(pData,uPos,0,TVC_UNKNOWN);
	}




	return uPos;
}

//*****************************************************************************
//*
//*		TreeListSetItem
//*
//*****************************************************************************
//	Ändert einen Eintrag im Fenster
//	pData		: Zeiger auf die Fensterdaten
//	pItem		: Zeiger auf die ein zu ?dernden Daten
//	Ergibt 0 wenn ok oder -1 bei einem Fehler
static int TreeListSetItem(TreeListData *pData,const TV_ITEM *pItem)
{
	BYTE			bCall;
	BYTE			bFlags;
	ExtraItem	  **pList;
	ExtraItem	   *pExtra;
	BaseItem	   *pEntry;
	unsigned		uChange;
	unsigned		uMask;
	unsigned		uBits;
	unsigned		uItem;
	unsigned		uSub;
	unsigned		uLen;
	int				iVal;
	int				iRet;


	uChange = 0;


	uItem = U(pItem->hItem);
	if(uItem>pData->uTreeItemsMax)return -1;

	pEntry = pData->pTreeItems[uItem];
	if(!pEntry)return -1;


	uBits = pItem->mask;

	if(uBits&TVIF_SUBITEM)									// Einen Extraeintrag ?dern
	{
		uSub = pItem->cChildren;
		if(uSub>0)
		{
			if(uSub>= pData->uColumnCount)return -1;
			pList	= pData->pExtraItems[uSub-1];
			pExtra	= pList[uItem];

			if(!pExtra)										// Einen neuen Eintrag erzeugen
			{
				pExtra  = new ExtraItem;
				memset(pExtra,0,sizeof(ExtraItem));
				pExtra->iImage = TV_NOIMAGE;
				pExtra->uState = pEntry->uState&(TVIS_BOLD|TVIS_UNTERLINE);
				pList[uItem]   = pExtra;
			}

			if(uBits&TVIF_PARAM)
			{
				pEntry->lParam	= pItem->lParam;
			}

			if((uBits&TVIF_IMAGE) && pExtra->iImage!=pItem->iImage)
			{
				if(pData->hImages)uChange=1;
				pExtra->iImage =pItem->iImage;
				if( pExtra->iImage==I_IMAGECALLBACK)
					pExtra->bCallback |= TVIF_IMAGE;
				else	pExtra->bCallback &= TVIF_IMAGE;
			}

			if(uBits&TVIF_TEXT)								// Einen neuen Text einstellen
			{
				if(pItem->pszText==LPSTR_TEXTCALLBACK)
				{
					if(pExtra->pText)delete pExtra->pText;
					pExtra->bCallback|= TVIF_TEXT;
					pExtra->uTextSize = 0;
					pExtra->pText	  = 0;
					uChange			  = 1;
				}
				else{
					uLen = str_len(pItem->pszText);

					if(uLen>pExtra->uTextSize || !pExtra->pText)
					{
						if(pExtra->pText)delete pExtra->pText;
						pExtra->pText=new TCHAR[uLen+1];
					}

					memcpy(pExtra->pText,pItem->pszText,(uLen+1)*sizeof(TCHAR));
					pExtra->bCallback  &=~TVIF_TEXT;
					pExtra->uTextSize   = (WORD)uLen;
					pExtra->iTextPixels = 0;
					uChange=1;
				}
			}

			if(uBits&TVIF_STATE)							// Den Status ?dern
			{
				uMask			=  pItem->stateMask&~TVIS_BASEFLAGS;
				uBits			=  uMask&(pExtra->uState^pItem->state);
				uBits		   |= (pItem->stateMask& TVIS_BASEFLAGS)&(pEntry->uState^pItem->state);
				pExtra->uState &= ~uMask;
				pExtra->uState |=  uMask&pItem->state;

				if((uBits&TVIS_OVERLAYMASK|TVIS_CUT) && pData->hImages)
				{
					uChange=1;
				}

				if(uBits&(TVIS_BOLD|TVIS_DROPHILITED))
				{
					pExtra->iTextPixels=0;
					uChange=1;
				}

				if((uBits&TVIS_EXPANDED) && pEntry->uFirstChild)
				{
					iVal=TreeListToggleItem(pData,uItem,0);
					if(iVal<0)return -1;

					pEntry = pData->pTreeItems[uItem];
					if(!pEntry)return -1;
				}

				if(uBits&TVIS_SELECTED)
				{
					iVal = (pData->uStyleEx&TVS_EX_SUBSELECT)? uSub:0;

					if(pItem->state&TVIS_SELECTED)
						iRet = TreeListSelectItem(pData,uItem,iVal,TVC_UNKNOWN);
					else	iRet = TreeListSelectItem(pData,0    ,0   ,TVC_UNKNOWN);

					pEntry = pData->pTreeItems[uItem];
					if(!pEntry)return -1;

					if(iRet>=2)
					{
						pList	= pData->pExtraItems[uSub-1];
						pExtra	= pList[uItem];
						if(!pExtra)return -1;
					}
					else if(iRet==1)
					{
						uChange=1;
					}
				}
			}

			if(!uChange || !pEntry->uShowPos)return 0;		// Neuzeichnen des Eintrages

			UpdateRect(pData,uItem,uSub);

			return 0;
		}

		uBits &= ~TVIF_CHILDREN;
	}


	//******************** Einen Basis Eintrag ?dern *****************************


	if(uBits&TVIF_PARAM)
	{
		pEntry->lParam	= pItem->lParam;
	}

	if((uBits&TVIF_IMAGE) && pEntry->iImage!=pItem->iImage)
	{
		pEntry->iImage = pItem->iImage;
		if(!(pEntry->uState&TVIS_SELECTED) && pData->hImages)uChange=1;
		if(  pEntry->iImage==I_IMAGECALLBACK)
			pEntry->bCallback |= TVIF_IMAGE;
		else	pEntry->bCallback &= TVIF_IMAGE;
	}

	if((uBits&TVIF_SELECTEDIMAGE) && pEntry->iSelectedImage!=pItem->iSelectedImage)
	{
		pEntry->iSelectedImage = pItem->iSelectedImage;
		if((pEntry->uState&TVIS_SELECTED) && pData->hImages)uChange=1;
		if( pEntry->iSelectedImage==I_IMAGECALLBACK)
			pEntry->bCallback |= TVIF_SELECTEDIMAGE;
		else	pEntry->bCallback &= TVIF_SELECTEDIMAGE;
	}

	if(uBits&TVIF_CHILDREN)
	{
		bCall  = pEntry->bCallback;
		bFlags = pEntry->bFlags;

		switch(pItem->cChildren)
		{
		case  0:		pEntry->bCallback &= ~TVIF_CHILDREN;
			pEntry->bFlags    &= ~TVIX_HASBUTTON;
			pEntry->bFlags    &=  TVIX_VARBUTTON;
			break;

		case  1:		pEntry->bCallback &= ~TVIF_CHILDREN;
			pEntry->bFlags    &=  TVIX_VARBUTTON;
			pEntry->bFlags    |=  TVIX_HASBUTTON;
			break;

		case  I_CCB:	pEntry->bCallback |=  TVIF_CHILDREN;
			pEntry->bFlags    &=  TVIX_VARBUTTON;
			break;

		default:		pEntry->bCallback &= ~TVIF_CHILDREN;
			pEntry->bFlags    |=  TVIX_VARBUTTON;

			if(pEntry->uFirstChild)
				pEntry->bFlags |= TVIX_HASBUTTON;
			else	pEntry->bFlags &=~TVIX_HASBUTTON;
		}

		if(bCall!=pEntry->bCallback || bFlags!=pEntry->bFlags)
		{
			uChange=1;
		}
	}

	if(uBits&TVIF_TEXT)										// Einen neuen Text einstellen
	{
		if(pItem->pszText==LPSTR_TEXTCALLBACK)
		{
			if(pEntry->pText)delete pEntry->pText;
			pEntry->bCallback|= TVIF_TEXT;
			pEntry->uTextSize = 0;
			pEntry->pText	  = 0;
			uChange			  = 1;
		}
		else{
			uLen = str_len(pItem->pszText);

			if(uLen>pEntry->uTextSize)
			{
				if(pEntry->pText)delete pEntry->pText;
				pEntry->pText=new TCHAR[uLen+1];
			}

			memcpy(pEntry->pText,pItem->pszText,(uLen+1)*sizeof(TCHAR));
			pEntry->bCallback  &=~TVIF_TEXT;
			pEntry->uTextSize   = (WORD)uLen;
			pEntry->iTextPixels = 0;
			uChange=1;
		}
	}

	if(uBits&TVIF_STATE)
	{
		uMask = pItem->stateMask;

		if(pData->uStyle&TVS_SINGLEEXPAND)					// Nicht aufklappen bei Einzelmodus
		{
			uMask &= ~TVIS_EXPANDED;
		}

		uBits			=  uMask&(pEntry->uState^pItem->state);
		pEntry->uState &= ~uMask;
		pEntry->uState |=  uMask&pItem->state;


		if((uBits&(TVIS_OVERLAYMASK|TVIS_CUT)) && pData->hImages)
		{
			uChange=1;
		}

		if((uBits&TVIS_STATEIMAGEMASK) && pData->hStates)
		{
			uChange=1;
		}

		if(uBits&(TVIS_BOLD|TVIS_DROPHILITED))
		{
			pEntry->iTextPixels = 0;
			uChange=1;
		}

		if(uBits&TVIS_SELECTED)
		{
			pEntry->uState ^= TVIS_SELECTED;

			if(pItem->state&TVIS_SELECTED)
				iRet = TreeListSelectItem(pData,uItem,0,TVC_UNKNOWN);
			else	iRet = TreeListSelectItem(pData,0    ,0,TVC_UNKNOWN);

			pEntry = pData->pTreeItems[uItem];
			if(!pEntry)return -1;

			if(iRet==1)
			{
				uChange=1;
			}
		}

		if((uBits&TVIS_EXPANDED) && pEntry->uFirstChild)	// Sollen Teile auf/zugeklappt werden
		{
			pEntry->uState^= TVIS_EXPANDED;

			iRet=TreeListToggleItem(pData,uItem,0);
			if(iRet<1)return iRet;
		}
	}

	if(uChange && pEntry->uShowPos)							// Neuzeichnen des Eintrages
	{
		UpdateRect(pData,uItem,0);
	}



	return 0;
}


//*****************************************************************************
//*
//*		TreeListGetItem
//*
//*****************************************************************************
//	Daten vone ienem Eintrag abfragen
//	pData		: Zeiger auf die Fensterdaten
//	pItem		: Zeiger auf die den Datenspeicher
//	Ergibt 1 wenn ok oder 0 bei einem Fehler
static unsigned TreeListGetItem(TreeListData *pData,TV_ITEM *pItem)
{
	ExtraItem	  **pList;
	ExtraItem	   *pExtra;
	BaseItem	   *pEntry;
	unsigned		uBits;
	unsigned		uItem;
	unsigned		uSub;
	unsigned		uLen;




	uItem = U(pItem->hItem);
	if(uItem>pData->uTreeItemsMax)return 0;

	pEntry = pData->pTreeItems[uItem];
	if(!pEntry)return 0;


	uBits = pItem->mask;

	if(uBits&TVIF_SUBITEM)									// Einen Extraeintrag abfragen
	{
		uSub = pItem->cChildren;
		if(uSub>0)
		{
			if(uSub>= pData->uColumnCount)return 0;
			pList	= pData->pExtraItems[uSub-1];
			pExtra	= pList[uItem];

			if(!pExtra)										// Einen neuen Eintrag erzeugen
			{
				pExtra  = new ExtraItem;
				memset(pExtra,0,sizeof(ExtraItem));
				pExtra->iImage = TV_NOIMAGE;
				pExtra->uState = pEntry->uState&(TVIS_BOLD|TVIS_UNTERLINE);
				pList[uItem]   = pExtra;
			}

			if(uBits&TVIF_PARAM)
			{
				pItem->lParam = pEntry->lParam;
			}

			if(uBits&TVIF_IMAGE)
			{
				pItem->iImage = pExtra->iImage;
			}

			if(uBits&TVIF_TEXT)								// Einen neuen Text einstellen
			{
				if(pExtra->pText==LPSTR_TEXTCALLBACK)
				{
					pItem->pszText = LPSTR_TEXTCALLBACK;
				}
				else if(uBits&TVIF_TEXTPTR)
				{
					if(!pExtra->pText)
					{
						pItem->pszText    = _T("");
						pItem->cchTextMax = 0;
					}
					else{
						pItem->pszText    = pExtra->pText;
						pItem->cchTextMax = pExtra->uTextSize+1;
					}
				}
				else{
					if(pExtra->pText)
					{
						uLen = pExtra->uTextSize+1;
						if(pItem->cchTextMax<(int)uLen)
						{
							if(pItem->cchTextMax<=0)
							{
								uLen=0;
							}
							else{
								uLen=pItem->cchTextMax-1;
								pItem->pszText[uLen]=0;
							}
						}

						memcpy(pItem->pszText,pExtra->pText,uLen*sizeof(TCHAR));
					}
					else{
						if(pItem->cchTextMax>0)
						{
							pItem->pszText[0]=0;
						}
					}
				}
			}

			if(uBits&TVIF_STATE)
			{
				pItem->state  = pExtra->uState&~TVIS_BASEFLAGS;
				pItem->state &= pItem->stateMask;
			}

			return 1;
		}

		if(pEntry->bCallback&TVIF_CHILDREN)
			pItem->cChildren = (pEntry->uFirstChild)? 1:0;
		else	pItem->cChildren = I_CHILDRENCALLBACK;


		uBits &= ~TVIF_CHILDREN;
	}


	//******************** Einen Basis Eintrag ?dern *****************************


	if(uBits&TVIF_PARAM)
	{
		pItem->lParam = pEntry->lParam;
	}

	if(uBits&TVIF_IMAGE)
	{
		pItem->iImage = pEntry->iImage;
	}

	if(uBits&TVIF_SELECTEDIMAGE)
	{
		pItem->iSelectedImage = pEntry->iSelectedImage;
	}

	if(uBits&TVIF_CHILDREN)
	{
		if(pEntry->bCallback&TVIF_CHILDREN)
			pItem->cChildren = (pEntry->uFirstChild)? 1:0;
		else	pItem->cChildren = I_CHILDRENCALLBACK;
	}

	if(uBits&TVIF_TEXT)										// Einen neuen Text einstellen
	{
		if(pEntry->pText==LPSTR_TEXTCALLBACK)
		{
			pItem->pszText = LPSTR_TEXTCALLBACK;
		}
		else if(uBits&TVIF_TEXTPTR)
		{
			pItem->pszText    = pEntry->pText;
			pItem->cchTextMax = pEntry->uTextSize+1;
		}
		else{
			uLen = pEntry->uTextSize+1;
			if(pItem->cchTextMax<(int)uLen)
			{
				if(pItem->cchTextMax<=0)
				{
					uLen=0;
				}
				else{
					uLen=pItem->cchTextMax-1;
					pItem->pszText[uLen]=0;
				}
			}

			memcpy(pItem->pszText,pEntry->pText,uLen*sizeof(TCHAR));
		}
	}

	if(uBits&TVIF_STATE)
	{
		pItem->state  = pEntry->uState;
		pItem->state &= pItem->stateMask;
	}


	return 1;
}


//*****************************************************************************
//*
//*		TreeListDeleteColumn
//*
//*****************************************************************************
//	L?cht eine Spalte aus dem Header
//	pData		: Zeiger auf die Fensterdaten
//	uCol		: Ist die Nummer der Spalte die gel?cht werden soll
//	Ergibt 1 wenn die Spalte gel?cht wurde
static int TreeListDeleteColumn(TreeListData *pData,unsigned uCol)
{
	ExtraItem **pList;
	ExtraItem  *pExtra;
	HDITEM		sItem;
	RECT		sRect;
	int			iSize;
	int			iXoff;
	int			iNum;
	int			iVar=0;
	unsigned	uItem;
	unsigned	uPos;



	if(uCol>=pData->uColumnCount)return 0;

	if(uCol && uCol==pData->uSelectedSub)					// Ist die Auswahl in der Spalte
	{
		TreeListSelectItem(pData,pData->uSelectedItem,0,TVC_UNKNOWN);
	}

	if(uCol && uCol==pData->uEditSub)
	{
		pData->uEditSub  = 0;
		pData->uEditItem = 0;
		TreeListEndLabelEdit(pData,0);
	}

	if(uCol && uCol==pData->uFocusSub)
	{
		pData->uFocusSub  = 0;
		pData->uFocusItem = 0;
	}

	if(uCol==pData->uTrackedSub)
	{
		pData->uTrackedSub  = 0;
		pData->uTrackedItem = 0;
	}



	GetClientRect(pData->hWnd,&sRect);


	iSize = pData->uSizeX;

	for(uPos=0;uPos<pData->uColumnCount;uPos++)
	{
		if(pData->cColumnVar[uPos]){iVar++;continue;}
		if(uPos==uCol)continue;
		iSize-=pData->iColumnXpos[uPos+1]-pData->iColumnXpos[uPos];
	}


	Header_DeleteItem(pData->hHeader,uCol);
	pData->uColumnCount--;

	if(pData->uColumnCount>0)								// Liste mit Extraeintr?en l?chen
	{
		iNum=uCol-1;
		if(iNum<0)iNum=0;

		pList = pData->pExtraItems[iNum];
		if(pList)
		{
			for(uItem=0;uItem<=pData->uTreeItemsMax;uItem++)// Alle Eintr?e aus der Liste l?chen
			{
				pExtra = pList[uItem];
				if(!pExtra)continue;

				if(pExtra->pText)
				{
					pExtra->uTextSize=0;
					delete pExtra->pText;
				}

				delete pExtra;
			}

			memmove(pData->pExtraItems+iNum,pData->pExtraItems+iNum+1,sizeof(pList)*(MAX_COLUMNS-1-iNum));
			pData->pExtraItems[pData->uColumnCount]=NULL;
			delete pList;
		}
	}
	else{
		iNum=MAX_COLUMNS;
	}

	if(pData->cColumnVar[uCol])
	{
		pData->uColumnCountVar--;
	}

	memmove(pData->cColumnAlign+uCol,pData->cColumnAlign+uCol+1,MAX_COLUMNS-1-uCol);
	memmove(pData->cColumnVar  +uCol,pData->cColumnVar  +uCol+1,MAX_COLUMNS-1-uCol);
	pData->cColumnVar[pData->uColumnCount]=0;

	if(pData->uColumnCountVar>0)							// Variable Breiten anpassen
	{
		sItem.mask	= HDI_WIDTH;
		iNum		= pData->uColumnCountVar;

		for(uPos=0;uPos<pData->uColumnCount;uPos++)
		{
			if(!pData->cColumnVar[uPos])continue;
			sItem.cxy   = iSize/iNum;
			Header_SetItem(pData->hHeader,uPos,&sItem);
			iSize	   -= sItem.cxy;
			iNum	   -= 1;
		}
	}

	if(pData->uSelectedSub>uCol)							// Ist die Auswahl vor der Spalte
	{
		pData->uSelectedSub--;
	}

	if(pData->uEditSub>uCol)
	{
		pData->uEditSub--;
	}

	if(pData->uFocusSub>uCol)
	{
		pData->uFocusSub--;
	}

	if(pData->uTrackedSub>uCol)
	{
		pData->uTrackedSub--;
	}


	if(!pData->uColumnCount)								// Den Header l?chen
	{
		DestroyWindow(pData->hHeader);
		pData->hHeader	   = NULL;
		pData->uStartPixel = 0;
		pData->iRowHeight  = 1;
		UpdateHeight  (pData);
		InvalidateRect(pData->hWnd,&sRect,FALSE);
	}


	iXoff=UpdateColumns(pData);							// Hat sich die Spaltenbreiten ver?dert
	if(iXoff<0x10000)
	{
		sRect.left  = iXoff;
		sRect.left -= pData->uScrollX;
		sRect.top   = pData->uStartPixel;
		InvalidateRect(pData->hWnd,&sRect,FALSE);
	}


	UpdateScrollX(pData);




	return 1;
}


//*****************************************************************************
//*
//*		TreeListInsertColumn
//*
//*****************************************************************************
//	F?t eine neue Spalte in den Header ein
//	pData		: Zeiger auf die Fensterdaten
//	iCol		: Ist die Nummer der Spalte die eingef?t wird
//	pInsert		: Zeiger auf die ein zu f?enden Daten
//	Ergibt die Einf?eposition der neuen Spalte bzw. -1 bei eimem Fehler
static int TreeListInsertColumn(TreeListData *pData,int iCol,TV_COLUMN *pColumn)
{
	ExtraItem **pList;
	HDITEM	 	sItem;
	RECT		sRect;
	int			iSize;
	int			iXoff;
	int			iYoff;
	int			iNum;
	int			iAdd=0;
	int			iVar=0;
	char		cAlign;
	char 		cFlag;
	unsigned	uPos;



	GetClientRect(pData->hWnd,&sRect);


	if(!pData->hHeader)										// Einen neuen Header erzeugen
	{
		iYoff = sRect.top+GetSystemMetrics(SM_CYHSCROLL);

		pData->hHeader=CreateWindow(WC_HEADER,NULL,WS_VISIBLE|WS_CHILD|HDS_HORZ|HDS_BUTTONS,sRect.left,sRect.top,sRect.right,iYoff,pData->hWnd,NULL,NULL,NULL);
		if(!pData->hHeader)return -1;

		pData->uStartPixel = GetSystemMetrics(SM_CYHSCROLL);
		pData->iRowHeight  = 1;
		UpdateHeight(pData);


		InvalidateRect(pData->hWnd,&sRect,FALSE);
		SendMessage   (pData->hHeader,HDM_SETIMAGELIST,0,(LPARAM)pData->hImages);
		SendMessage   (pData->hHeader,WM_SETFONT,(WPARAM)hDefaultFontN,0);
		SetWindowLong (pData->hHeader,GWL_ID,1);

		if(pData->uSizeX<=pData->uStartPixel)
			pData->uSizeYsub = 0;
		else	pData->uSizeYsub = pData->uSizeX-pData->uStartPixel;
	}


	if(pData->uColumnCount>=MAX_COLUMNS)					// Pr?e die Anzahl der Spalten
	{
		return -1;
	}


	memset(&sItem,0,sizeof(sItem));							// Die Spaltendaten zusammenstellen

	iSize = pData->uSizeX;

	for(uPos=0;uPos<pData->uColumnCount;uPos++)				// Restbreite berechnen
	{
		if(pData->cColumnVar[uPos]==1)
		{
			iVar++;
			continue;
		}

		iSize -= pData->iColumnXpos[uPos+1];
		iSize += pData->iColumnXpos[uPos  ];
	}


	if(pColumn->mask&TVCF_FMT)								// Welche Textausrichtung
	{
		sItem.mask    |= HDI_FORMAT;
		sItem.fmt      = pColumn->fmt;

		switch(sItem.fmt&HDF_JUSTIFYMASK)
		{
		case HDF_CENTER: cAlign = DT_CENTER;	break;
		case HDF_RIGHT:	 cAlign = DT_RIGHT;		break;
		default: 		 cAlign = DT_LEFT;		break;
		}
	}
	else{
		cAlign = DT_LEFT;
	}

	if(pColumn->mask&TVCF_IMAGE)
	{
		sItem.mask    |= HDI_IMAGE;
		sItem.iImage   = pColumn->iImage;
	}

	if(pColumn->mask&TVCF_TEXT)
	{
		sItem.mask    |= HDI_TEXT;
		sItem.pszText  = pColumn->pszText;
	}

	if(pColumn->mask&TVCF_WIDTH)							// Fixe Breite f? die Spalte
	{
		if(iVar)
		{
			iSize -= pColumn->cx;
			iAdd   = iSize%iVar;
			iSize /= iVar;
		}

		if(iSize<16)iSize=16;

		sItem.mask    |= HDI_WIDTH;
		sItem.cxy	   = pColumn->cx;
		cFlag		   = 0;
	}
	else if(pColumn->mask&TVCF_VWIDTH)						// Variable vordefinierte Breite
	{
		if(iVar)
		{
			iSize -= pColumn->cx;
			iAdd   = iSize%iVar;
			iSize /= iVar;
		}

		if(iSize<16)iSize=16;

		sItem.mask    |= HDI_WIDTH;
		sItem.cxy      = pColumn->cx;
		cFlag		   = 2;
		iVar          += 2;
	}
	else{													// Variable Breite berechen
		iVar  += 1;
		iAdd   = iSize%iVar;
		iSize /= iVar;
		if(iSize<16)iSize=16;

		sItem.mask    |= HDI_WIDTH;
		sItem.cxy      = iSize;
		cFlag		   = 1;
	}



	iCol = Header_InsertItem(pData->hHeader,iCol,&sItem);
	if(iCol<0)return -1;



	if(pData->uColumnCount>0)								// Liste mit Extraeintr?en erzeugen
	{
		pList = new ExtraItem*[pData->uTreeItemsMax+1];
		if(!pList)
		{
			Header_DeleteItem(pData->hHeader,iCol);
			return -1;
		}

		memset(pList,0,sizeof(ExtraItem*)*(pData->uTreeItemsMax+1));

		iNum = iCol-1;
		if(iNum<0)iNum=0;

		memmove(pData->pExtraItems+iNum+1,pData->pExtraItems+iNum,sizeof(pList)*(MAX_COLUMNS-2-iNum));
		pData->pExtraItems[iNum]=pList;
	}


	memmove(pData->cColumnAlign+iCol+1,pData->cColumnAlign+iCol,MAX_COLUMNS-1-iCol);
	memmove(pData->cColumnVar  +iCol+1,pData->cColumnVar  +iCol,MAX_COLUMNS-1-iCol);
	pData->cColumnAlign[iCol]   =  cAlign;
	pData->cColumnVar  [iCol]   =  cFlag;
	pData->uColumnCountVar     += (cFlag)? 1:0;
	pData->uColumnCount	       +=  1;


	if(pData->uSelectedSub>0 && pData->uSelectedSub>=(unsigned)iCol)
	{
		pData->uSelectedSub++;
	}

	if(pData->uTrackedSub>0 && pData->uTrackedSub>=(unsigned)iCol)
	{
		pData->uTrackedSub++;
	}

	if(pData->uFocusSub>0 && pData->uFocusSub>=(unsigned)iCol)
	{
		pData->uFocusSub++;
	}

	if(pData->uEditSub>0 && pData->uEditSub>=(unsigned)iCol)
	{
		pData->uEditSub++;
	}


	iNum = iVar-cFlag;
	if(iNum>0)												// Variable Breiten anpassen
	{
		sItem.mask	= HDI_WIDTH;
		sItem.cxy   = iSize;

		for(uPos=0;uPos<pData->uColumnCount;uPos++)
		{
			if(pData->cColumnVar[uPos]!=1)continue;
			if( uPos==(unsigned)iCol)continue;
			iNum--;
			if(!iNum)sItem.cxy+=iAdd;
			Header_SetItem(pData->hHeader,uPos,&sItem);
		}
	}


	iXoff=UpdateColumns(pData);							// Hat sich die Spaltenbreiten ver?dert
	if(iXoff<0x10000)
	{
		sRect.left  = iXoff;
		sRect.left -= pData->uScrollX;
		sRect.top   = pData->uStartPixel;
		InvalidateRect(pData->hWnd,&sRect,FALSE);
	}


	UpdateScrollX(pData);


	if(pData->uInsertMark)									// Fehlende Infomarken einf?en
	{
		TV_ITEM		sSet;
		ExtraItem  *pExtra;

		sSet.mask		= TVIF_SUBITEM;
		sSet.hItem		= (HTREEITEM)pData->uInsertMark;
		sSet.cChildren	= iCol;

		TreeListSetItem(pData,&sSet);

		pExtra = pData->pExtraItems[iCol-1][pData->uInsertMark];
		if(pExtra)
		{
			pExtra->uColorBk  = pData->uColors[TVC_INSERT];
			pExtra->bFlags	 |= TVIX_BKCOLOR;
		}
	}



	return iCol;
}

//*****************************************************************************
//*
//*		TreeListScanColumn
//*
//*****************************************************************************
//	Berechnet die Breite der sichtbaren Eintr?e einer Spalte
//	pData		: Zeiger auf die Fensterdaten
//	uSub		: Ist die Spalte
//	Ergibt sie gescannte Breite
static int TreeListScanColumn(TreeListData *pData,unsigned uSub)
{
	BaseItem  **pList;
	BaseItem   *pEntry;
	ExtraItem  *pExtra;
	ExtraItem **pItems;
	unsigned   *pPList;
	unsigned	uPos;
	int			iMax;
	int			iPos;



	if(uSub>=pData->uColumnCount)return 0;

	if(uSub>0)												// Extraspalte
	{
		pItems = pData->pExtraItems[uSub-1];
		pPList = pData->pItemPos;
		iMax   = 0;

		for(uPos=0;uPos<pData->uItemPosCount;uPos++)
		{
			pExtra = pItems[pPList[uPos]];
			if(!pExtra)
			{
				if(iMax<8)iMax=8;
				continue;
			}

			if(pData->hImages && (pExtra->bFlags&TVIX_HASIMAGE))
				iPos = pData->iImagesXsize;
			else	iPos = 0;

			iPos += pExtra->iTextPixels+8;
			if(iPos>iMax)iMax=iPos;
		}

		return iMax;
	}


	pList  = pData->pTreeItems;
	pPList = pData->pItemPos;
	iMax   = 0;


	for(uPos=0;uPos<pData->uItemPosCount;uPos++)			// Erste Spalte
	{
		pEntry = pList[pPList[uPos]];

		if(pEntry->bFlags&TVIX_HASIMAGE)
			iPos = pData->iImagesXsize;
		else	iPos = 0;

		iPos += pEntry->uLevel*pData->iIndent;
		iPos += pEntry->iTextPixels+8;
		if(iPos>iMax)iMax=iPos;
	}

	if(pData->uStyleEx&TVS_EX_ITEMLINES)
	{
		iMax += 1;
	}

	if(pData->cHasRootRow)
	{
		iMax += pData->iIndent;
	}

	if(pData->hStates)
	{
		iMax += pData->iStatesXsize;
	}



	return iMax;
}


//*****************************************************************************
//*
//*		TreeListHitTest
//*
//*****************************************************************************
//	Pr?t wo eine Koortinate im Fenster ist
//	pData		: Zeiger auf die Fensterdaten
//	pInfo		: Zeiger auf die ein zu f?enden Daten
//	Ergibt das Item auf dem die Koortinate zeigt
static unsigned TreeListHitTest(TreeListData *pData,TV_HITTESTINFO *pInfo)
{
	int			iXpos;
	int			iYpos;
	int			iZpos;
	int			iWidth;
	int			iIcon;
	BaseItem   *pEntry;
	ExtraItem  *pExtra;
	unsigned	uItem;
	unsigned	uSub;



	iXpos = pInfo->pt.x;
	iYpos = pInfo->pt.y;

	if((unsigned)iXpos>=pData->uSizeX)
	{
		pInfo->hItem = NULL;
		pInfo->flags = (iXpos<0)? TVHT_TOLEFT:TVHT_TORIGHT;
		return 0;
	}

	iYpos-=pData->uStartPixel;

	if((unsigned)iYpos>=pData->uSizeY)
	{
		pInfo->hItem = NULL;
		pInfo->flags = (iYpos<0)? TVHT_ABOVE:TVHT_BELOW;
		return 0;
	}


	iZpos  = iYpos/pData->iRowHeight;
	iZpos +=       pData->uScrollY;

	if((unsigned)iZpos>=pData->uItemPosCount)
	{
		pInfo->hItem = NULL;
		pInfo->flags = TVHT_NOWHERE;
		return 0;
	}

	iXpos		+= pData->uScrollX;
	uItem		 = pData->pItemPos  [iZpos];
	pEntry		 = pData->pTreeItems[uItem];
	pInfo->hItem = (HTREEITEM)uItem;


	if(!pEntry)return 0;


	if(iXpos>=pData->iColumnXpos[1])						// Aus Extraeintrag
	{
		for(uSub=1;uSub<pData->uColumnCount;uSub++)
		{
			if(iXpos>=pData->iColumnXpos[uSub+1])continue;
			iXpos-=pData->iColumnXpos[uSub];

			pExtra = pData->pExtraItems[uSub-1][uItem];


			if(pData->bColumnEdit[uSub]>=TVAX_CHECK)		// Hat der Extraeintrag ein Icon
			{
				iIcon = pData->iChecksXsize;
			}
			else if(pExtra && (pExtra->bFlags&TVIX_HASIMAGE))
			{
				iIcon = pData->iImagesXsize;
			}
			else{
				iIcon = 0;
			}

			pInfo->flags = uSub<<24;

			if(iXpos<iIcon)									// Auf Icon
			{
				pInfo->flags |= TVHT_ONSUBICON;
				return uItem;
			}

			if(!pExtra || !pExtra->uTextSize)				// Auf Text wenn leerer Eintrag
			{
				pInfo->flags |= TVHT_ONSUBLABEL;
				return uItem;
			}

			switch(pData->cColumnAlign[uSub])				// Textausrichtung
			{
			default:		if(iXpos-iIcon<pExtra->iTextPixels+5)
						{
							pInfo->flags |= TVHT_ONSUBLABEL;
							return uItem;
						}
						break;

			case DT_RIGHT:	iWidth  = pData->iColumnXpos[uSub+1];
				iWidth -= pData->iColumnXpos[uSub  ];

				if(iXpos>=iWidth-pExtra->iTextPixels-5)
				{
					pInfo->flags |= TVHT_ONSUBLABEL;
					return uItem;
				}
				break;

			case DT_CENTER:	iWidth  = pData->iColumnXpos[uSub+1];
				iWidth -= pData->iColumnXpos[uSub  ];
				iWidth += iIcon;
				iWidth /= 2;

				if(iXpos>=iWidth-pExtra->iTextPixels/2-3)
					if(iXpos<=iWidth+pExtra->iTextPixels/2+3)
					{
						pInfo->flags |= TVHT_ONSUBLABEL;
						return uItem;
					}
					break;
			}

			pInfo->flags |= TVHT_ONSUBRIGHT;
			return uItem;
		}


		pInfo->flags = TVHT_ONRIGHTSPACE;

		return uItem;
	}


	if(!pData->cHasRootRow)									// Root-Linien ausgleichen
	{
		iXpos += pData->iIndent;
	}


	iXpos -= pData->iIndent*pEntry->uLevel;

	if(iXpos<pData->iIndent)								// Auf einger?ktem Bereich
	{
		if(pData->uStyle &TVS_HASBUTTONS)
			if(pEntry->bFlags&TVIX_HASBUTTON)
			{
				if(iXpos>=pData->iShift-6)
					if(iXpos<=pData->iShift+7)
					{
						iYpos %= pData->iRowHeight;
						iYpos -= pData->iRowHeight/2;

						if(iYpos>=-6 && iYpos<=7)
						{
							pInfo->flags = TVHT_ONITEMBUTTON;
							return uItem;
						}
					}
			}

			pInfo->flags = TVHT_ONITEMINDENT;
			return uItem;
	}


	iXpos -= pData->iIndent;

	if(pData->uStyleEx&TVS_EX_ITEMLINES)
	{
		iXpos--;
	}

	if(pData->hStates)										// Auf der Auswahl-Box
	{
		iXpos -= pData->iStatesXsize;

		if(iXpos<0)
		{
			pInfo->flags = TVHT_ONITEMSTATEICON;
			return uItem;
		}
	}


	if(pEntry->bFlags&TVIX_HASIMAGE)						// Auf dem Icon
	{
		iXpos -= pData->iImagesXsize;

		if(pData->uStyleEx&TVS_EX_ITEMLINES)iXpos--;

		if(iXpos<0)
		{
			pInfo->flags = TVHT_ONITEMICON;
			return uItem;
		}
	}

	if(iXpos<pEntry->iTextPixels+5)							// Auf Text
	{
		pInfo->flags = TVHT_ONITEMLABEL;
	}
	else{
		pInfo->flags = TVHT_ONITEMRIGHT;
	}



	return uItem;
}

//*****************************************************************************
//*
//*		TreeListSetTrackItem
//*
//*****************************************************************************
//	Setzt das den Einf?eeintrag
//	pData	: Zeiger auf die Fensterdaten
//	uItem	: Ist die Nummer des Eintrages bei dem eingef?t werden soll
//	iMode	: 0=davor einf?en  1=nachher einf?en
//	Ergibt 1 wenn der Eintrag eingef?t wurde
static unsigned TreeListSetInsertMark(TreeListData *pData,unsigned uItem,int iMode)
{
	TV_INSERTSTRUCT	sInsert;
	ExtraItem	   *pExtra;
	BaseItem	   *pEntry;
	unsigned		uSub;
	int				iRet;



	if(pData->uInsertMark)
	{
		iRet=TreeListDeleteItem(pData,pData->uInsertMark,1);
		pData->uInsertMark=0;
	}
	else{
		iRet=0;
	}

	if(uItem==0)return iRet;
	if(uItem>pData->uTreeItemsMax)return 0;

	pEntry = pData->pTreeItems[uItem];
	if(!pEntry)return 0;


	if(iMode)
	{
		uItem=pEntry->uPrevItem;
		if(!uItem)uItem=U(TVI_FIRST);
	}

	sInsert.hParent		 		= (HTREEITEM)pEntry->uParent;
	sInsert.hInsertAfter 		= (HTREEITEM)uItem;
	sInsert.item.mask	 		= TVIF_SELECTEDIMAGE|TVIF_IMAGE;
	sInsert.item.iImage			= TV_NOIMAGE;
	sInsert.item.iSelectedImage	= TV_NOIMAGE;

	uItem = TreeListInsertItem(pData,&sInsert);
	if(!uItem)return 0;


	pEntry				= pData->pTreeItems[uItem];
	pEntry->uColorBk	= pData->uColors[TVC_INSERT];
	pEntry->bFlags	   |= TVIX_BKCOLOR;
	sInsert.item.mask  |= TVIF_SUBITEM;
	sInsert.item.hItem	= (HTREEITEM)uItem;

	for(uSub=1;uSub<pData->uColumnCount;uSub++)
	{
		sInsert.item.cChildren	= uSub;

		TreeListSetItem(pData,&sInsert.item);

		pExtra = pData->pExtraItems[uSub-1][uItem];
		pExtra->uColorBk	= pData->uColors[TVC_INSERT];
		pExtra->bFlags	   |= TVIX_BKCOLOR;
	}


	pData->uInsertMark = uItem;



	return uItem;
}

//*****************************************************************************
//*
//*		TreeListGetItemColor
//*
//*****************************************************************************
//	Holt die Farbe eins Feldes
//	pData	: Zeiger auf die Fensterdaten
//	uItem	: Ist die Nummer des Eintrages
//	uSub	: Ist die Spalte
//	iMode	: 0=Hintergrundfarbe 1=Textfarbe abfragen
//	Ergibt die alte Farbe oder TV_NOCOLOR wenn keine Farbe eingestellt war
static COLORREF TreeListGetItemColor(TreeListData *pData,unsigned uItem,unsigned uSub,int iMode)
{
	COLORREF	uColor;
	ExtraItem  *pExtra;
	BaseItem   *pEntry;



	if(uItem>pData->uTreeItemsMax)return TV_NOCOLOR;

	pEntry = pData->pTreeItems[uItem];
	if(!pEntry)return TV_NOCOLOR;


	if(uSub)												// Extra-Eintrag abfragen
	{
		if(uSub>=pData->uColumnCount)return TV_NOCOLOR;

		pExtra = pData->pExtraItems[uSub-1][uItem];
		if(!pExtra)return TV_NOCOLOR;

		if(iMode)
			uColor = (pExtra->bFlags&TVIX_TEXTCOLOR)? pExtra->uColorText:TV_NOCOLOR;
		else	uColor = (pExtra->bFlags&TVIX_BKCOLOR  )? pExtra->uColorBk  :TV_NOCOLOR;
	}
	else{
		if(iMode)
			uColor = (pEntry->bFlags&TVIX_TEXTCOLOR)? pEntry->uColorText:TV_NOCOLOR;
		else	uColor = (pEntry->bFlags&TVIX_BKCOLOR  )? pEntry->uColorBk  :TV_NOCOLOR;
	}



	return uColor;
}

//*****************************************************************************
//*
//*		TreeListSetItemColor
//*
//*****************************************************************************
//	Setzt die Farbe eines Feldes
//	pData	: Zeiger auf die Fensterdaten
//	uItem	: Ist die Nummer des Eintrages
//	uSub	: Ist die Spalte
//	uColor	: Ist die neue Farbe
//	iMode	: 0=Hintergrundfarbe 1=Textfarbe einstellen
//	Ergibt die alte Farbe oder TV_NOCOLOR wenn keine Farbe eingestellt war
static COLORREF TreeListSetItemColor(TreeListData *pData,unsigned uItem,unsigned uSub,COLORREF uColor,int iMode)
{
	TV_ITEM		sSet;
	COLORREF	uOld;
	ExtraItem  *pExtra;
	BaseItem   *pEntry;



	if(uItem>pData->uTreeItemsMax)return TV_NOCOLOR;

	pEntry = pData->pTreeItems[uItem];
	if(!pEntry)return TV_NOCOLOR;


	if(uSub)												// Extra-Eintrag ver?dern
	{
		if(uSub>=pData->uColumnCount)return TV_NOCOLOR;

		pExtra = pData->pExtraItems[uSub-1][uItem];
		if(!pExtra)											// Extra-Eintrag erzeugen
		{
			sSet.mask		= TVIF_SUBITEM;
			sSet.hItem		= (HTREEITEM)uItem;
			sSet.cChildren	= uSub;

			if(TreeListSetItem(pData,&sSet))return TV_NOCOLOR;

			pExtra = pData->pExtraItems[uSub-1][uItem];
		}

		if(iMode)											// Textfarbe
		{
			uOld = (pExtra->bFlags&TVIX_TEXTCOLOR)? pExtra->uColorText:TV_NOCOLOR;

			if(uColor==TV_NOCOLOR)
			{
				pExtra->bFlags	   &= ~TVIX_TEXTCOLOR;
			}
			else{
				pExtra->bFlags	   |=  TVIX_TEXTCOLOR;
				pExtra->uColorText  =  uColor;
			}
		}
		else{												// Hintergrund
			uOld = (pExtra->bFlags&TVIX_BKCOLOR)? pExtra->uColorBk:TV_NOCOLOR;

			if(uColor==TV_NOCOLOR)
			{
				pExtra->bFlags	   &= ~TVIX_BKCOLOR;
			}
			else{
				pExtra->bFlags	   |=  TVIX_BKCOLOR;
				pExtra->uColorBk    =  uColor;
			}
		}
	}
	else{
		if(iMode)											// Textfarbe
		{
			uOld = (pEntry->bFlags&TVIX_TEXTCOLOR)? pEntry->uColorText:TV_NOCOLOR;

			if(uColor==TV_NOCOLOR)
			{
				pEntry->bFlags	   &= ~TVIX_TEXTCOLOR;
			}
			else{
				pEntry->bFlags	   |=  TVIX_TEXTCOLOR;
				pEntry->uColorText  =  uColor;
			}
		}
		else{												// Hintergrund
			uOld = (pEntry->bFlags&TVIX_BKCOLOR)? pEntry->uColorBk:TV_NOCOLOR;

			if(uColor==TV_NOCOLOR)
			{
				pEntry->bFlags	   &= ~TVIX_BKCOLOR;
			}
			else{
				pEntry->bFlags	   |=  TVIX_BKCOLOR;
				pEntry->uColorBk    =  uColor;
			}
		}
	}


	if(uColor!=uOld)										// Neu zeichnen
	{
		UpdateRect(pData,uItem,uSub);
	}



	return uOld;
}

//*****************************************************************************
//*
//*		TreeListSetTrackItem
//*
//*****************************************************************************
//	Setzt das unterstrichene Item
//	pData	: Zeiger auf die Fensterdaten
//	uItem	: Ist die Nummer des Eintrages
//	uSub	: Ist die Spalte
//	Ergibt 1 wenn der Eintrag unterstrichen wurde
static int TreeListSetTrackItem(TreeListData *pData,unsigned uItem,unsigned uSub)
{
	ExtraItem	   *pExtra;
	BaseItem	   *pEntry;
	int				iRet=1;


	if(!(pData->uStyleEx&TVS_EX_SUBSELECT))
	{
		uSub=0;
	}
	else{
		if(uSub>=pData->uColumnCount)
		{
			uItem = 0;
			uSub  = 0;
			iRet  = 0;
		}
	}

	if(uItem>pData->uTreeItemsMax)
	{
		uItem = 0;
		uSub  = 0;
		iRet  = 0;
	}
	else{
		if(uItem==pData->uTrackedItem)
			if(uSub ==pData->uTrackedSub )
			{
				return iRet;
			}
	}

	if(pData->uTrackedItem)									// Den alten Eintrag zur?ksetzen
	{
		if(pData->uTrackedSub)
		{
			pExtra = pData->pExtraItems[pData->uTrackedSub-1][pData->uTrackedItem];
			if(pExtra)
			{
				pExtra->bFlags &= ~TVIX_TRACKED;
				UpdateRect(pData,pData->uTrackedItem,pData->uTrackedSub);
			}
		}
		else{
			pEntry = pData->pTreeItems[pData->uTrackedItem];
			if(pEntry)
			{
				pEntry->bFlags &= ~TVIX_TRACKED;
				UpdateRect(pData,pData->uTrackedItem,0);
			}
		}
	}


	if(uItem)												// Den neuen Eintrag setzen
	{
		if(uSub)
		{
			pExtra = pData->pExtraItems[uSub-1][uItem];
			if(pExtra)
			{
				pData->uTrackedSub  = uSub;
				pData->uTrackedItem = uItem;
				pExtra->bFlags     |= TVIX_TRACKED;
				UpdateRect(pData,uItem,uSub);
			}
			else{
				iRet=0;
			}
		}
		else{
			pEntry = pData->pTreeItems[uItem];
			if(pEntry)
			{
				pData->uTrackedSub  = 0;
				pData->uTrackedItem = uItem;
				pEntry->bFlags     |= TVIX_TRACKED;
				UpdateRect(pData,uItem,0);
			}
			else{
				iRet=0;
			}
		}
	}



	return iRet;
}

//*****************************************************************************
//*
//*		TreeListGetNextItem
//*
//*****************************************************************************
//	Setzt sucht den n?hsten Eintrag
//	pData	: Zeiger auf die Fensterdaten
//	uItem	: Ist die Nummer des Eintrages
//	uFlags	: Bestimmt nach welchem Eintrag gesucht werden soll
//	Ergibt 1 wenn der Eintrag unterstrichen wurde
static unsigned TreeListGetNextItem(TreeListData *pData,unsigned uItem,unsigned uFlags)
{
	BaseItem	   *pEntry;
	unsigned		uStop;
	unsigned		uPos;


	switch(uFlags)
	{
	case TVGN_ROOT:				return pData->uFirstChild;

	case TVGN_NEXT:				if(uItem>pData->uTreeItemsMax)
						{
							return 0;
						}

						pEntry = pData->pTreeItems[uItem];
						if(!pEntry)return 0;

						return pEntry->uNextItem;

	case TVGN_PREVIOUS:			if(uItem>pData->uTreeItemsMax)
						{
							return 0;
						}

						pEntry = pData->pTreeItems[uItem];
						if(!pEntry)return 0;

						return pEntry->uPrevItem;


	case TVGN_PARENT:			if(uItem>pData->uTreeItemsMax)
						{
							return 0;
						}

						pEntry = pData->pTreeItems[uItem];
						if(!pEntry)return 0;

						return pEntry->uParent;


	case TVGN_CHILD:			if(uItem>pData->uTreeItemsMax)
						{
							if(uItem==U(TVI_ROOT))return pData->uFirstChild;
							return 0;
						}

						pEntry = pData->pTreeItems[uItem];
						if(!pEntry)return 0;

						return pEntry->uFirstChild;


	case TVGN_FIRSTVISIBLE:		if(pData->uItemPosCount<=0)return 0;
		if(pData->uItemPosCount<=pData->uScrollY)return 0;

		return pData->pItemPos[pData->uScrollY];

	case TVGN_NEXTVISIBLE:		if(uItem>pData->uTreeItemsMax)
					{
						if(uItem==U(TVI_ROOT))return pData->uFirstChild;
						return 0;
					}

					pEntry = pData->pTreeItems[uItem];
					if(!pEntry)return 0;

					uPos=pEntry->uShowPos;
					if(uPos< pData->uScrollY)return 0;
					if(uPos>=pData->uScrollY+pData->uPageEnties-1)return 0;

					return pData->pItemPos[uPos+1];


	case TVGN_NEXTSELECTED:		if(uItem>pData->uTreeItemsMax)
					{
						if(uItem!=U(TVI_ROOT))return 0;

						uItem = pData->uFirstChild;

						pEntry = pData->pTreeItems[uItem];
						if(!pEntry)return 0;
						if( pEntry->uState&TVIS_SELECTED)return uItem;
					}

					pEntry = pData->pTreeItems[uItem];
					if(!pEntry)return 0;

					for(;;)
					{
						if(pEntry->uFirstChild)
						{
							uItem = pEntry->uFirstChild;
						}
						else if(pEntry->uNextItem)
						{
							uItem = pEntry->uNextItem;
						}
						else{
							for(;;)
							{
								uItem  = pEntry->uParent;
								pEntry = pData ->pTreeItems[uItem];
								if(!pEntry)return 0;
								if( pEntry->uNextItem)
								{
									uItem=pEntry->uNextItem;
									break;
								}
							}
						}

						pEntry = pData->pTreeItems[uItem];
						if(!pEntry)break;
						if( pEntry->uState&TVIS_SELECTED)return uItem;
					}

					return 0;

	case TVGN_NEXTSELCHILD:		if(uItem>pData->uTreeItemsMax)
					{
						if(uItem!=U(TVI_ROOT))return 0;

						uItem = pData->uFirstChild;

						pEntry = pData->pTreeItems[uItem];
						if(!pEntry)return 0;
						if( pEntry->uState&TVIS_SELECTED)return uItem;

						uStop = 0;
					}
					else{
						pEntry = pData->pTreeItems[uItem];
						if(!pEntry || !pEntry->uFirstChild)return 0;

						uStop = uItem;
					}

					for(;;)
					{
						if(pEntry->uFirstChild)
						{
							uItem = pEntry->uFirstChild;
						}
						else if(pEntry->uNextItem)
						{
							uItem = pEntry->uNextItem;
						}
						else{
							for(;;)
							{
								uItem= pEntry->uParent;
								if( uItem==uStop)return 0;
								pEntry = pData ->pTreeItems[uItem];
								if(!pEntry)return 0;
								if( pEntry->uNextItem)
								{
									uItem=pEntry->uNextItem;
									break;
								}
							}
						}

						pEntry = pData ->pTreeItems[uItem];
						if(!pEntry)break;
						if( pEntry->uState&TVIS_SELECTED)return uItem;
					}

					return 0;


	case TVGN_PREVIOUSVISIBLE:	if(uItem>pData->uTreeItemsMax)
					{
						if(uItem==U(TVI_ROOT))return pData->uFirstChild;
						return 0;
					}

					pEntry = pData->pTreeItems[uItem];
					if(!pEntry)return 0;

					uPos=pEntry->uShowPos;
					if(uPos<=pData->uScrollY)return 0;
					if(uPos>=pData->uScrollY+pData->uPageEnties)return 0;
					if(uPos<2)return 0;

					return pData->pItemPos[uPos-1];

	case TVGN_LASTVISIBLE:		uPos=pData->uItemPosCount;
		if(uPos<=0)return 0;
		return pData->pItemPos[uPos-1];

	case TVGN_DROPHILITE:		return pData->uTrackedItem;

	case TVGN_DROPHILITESUB:	return pData->uTrackedSub;

	case TVGN_CARET:			return pData->uSelectedItem;

	case TVGN_CARETSUB:			return pData->uSelectedSub;

	case TVGN_FOCUS:			return (pData->uFocusItem)? pData->uFocusItem:pData->uSelectedItem;

	case TVGN_FOCUSSUB:			return (pData->uFocusItem)? pData->uFocusSub:pData->uSelectedSub;
	}


	return 0;
}

//*****************************************************************************
//*
//*		TreeListNextSelUntil
//*
//*****************************************************************************
//	Sucht rekursiv ausgew?lte Eintr?e
//	pData	: Zeiger auf die Fensterdaten
//	uItem	: Ist die Nummer des Eintrages nach dem mit der Suche begonnen werden soll
//	uStop	: Bestimmt bei welchem Eintrag abgebrochen werden soll werden soll
//	Ergibt 1 wenn der Eintrag unterstrichen wurde
static unsigned TreeListNextSelUntil(TreeListData *pData,unsigned uItem,unsigned uStop)
{
	BaseItem	   *pEntry;



	if(uItem>pData->uTreeItemsMax)
	{
		if(uItem!=U(TVI_ROOT))return 0;

		uItem = pData->uFirstChild;

		pEntry = pData->pTreeItems[uItem];
		if(!pEntry)return 0;
		if( pEntry->uState&TVIS_SELECTED)return uItem;

		uStop = 0;
	}
	else{
		pEntry = pData->pTreeItems[uItem];
		if(!pEntry)return 0;
		if(!pEntry->uFirstChild && uItem==uStop)return 0;
	}

	for(;;)
	{
		if(pEntry->uFirstChild)
		{
			uItem= pEntry->uFirstChild;
			if(uItem==uStop)return 0;
		}
		else if(pEntry->uNextItem)
		{
			uItem= pEntry->uNextItem;
			if(uItem==uStop)return 0;
		}
		else{
			for(;;)
			{
				uItem= pEntry->uParent;
				if( uItem==uStop)return 0;
				pEntry = pData ->pTreeItems[uItem];
				if(!pEntry)return 0;
				if( pEntry->uNextItem)
				{
					uItem=pEntry->uNextItem;
					if(uItem==uStop)return 0;
					break;
				}
			}
		}

		pEntry = pData ->pTreeItems[uItem];
		if(!pEntry)break;
		if( pEntry->uState&TVIS_SELECTED)return uItem;
	}


	return 0;
}

//*****************************************************************************
//*
//*		TreeListNextUnselUntil
//*
//*****************************************************************************
//	Sucht rekursiv nicht ausgew?lte Eintr?e
//	pData	: Zeiger auf die Fensterdaten
//	uItem	: Ist die Nummer des Eintrages nach dem mit der Suche begonnen werden soll
//	uStop	: Bestimmt bei welchem Eintrag abgebrochen werden soll werden soll
//	Ergibt 1 wenn der Eintrag unterstrichen wurde
static unsigned TreeListNextUnselUntil(TreeListData *pData,unsigned uItem,unsigned uStop)
{
	BaseItem	   *pEntry;



	if(uItem>pData->uTreeItemsMax)
	{
		if(uItem!=U(TVI_ROOT))return 0;

		uItem = pData->uFirstChild;

		pEntry = pData->pTreeItems[uItem];
		if(!pEntry)return 0;
		if((pEntry->uState&TVIS_SELECTED)==0)return uItem;

		uStop = 0;
	}
	else{
		pEntry = pData->pTreeItems[uItem];
		if(!pEntry)return 0;
		if(!pEntry->uFirstChild && uItem==uStop)return 0;
	}

	for(;;)
	{
		if(pEntry->uFirstChild)
		{
			uItem= pEntry->uFirstChild;
			if(uItem==uStop)return 0;
		}
		else if(pEntry->uNextItem)
		{
			uItem= pEntry->uNextItem;
			if(uItem==uStop)return 0;
		}
		else{
			for(;;)
			{
				uItem= pEntry->uParent;
				if( uItem==uStop)return 0;
				pEntry=pData->pTreeItems[uItem];
				if(!pEntry)return 0;
				if( pEntry->uNextItem)
				{
					uItem=pEntry->uNextItem;
					if(uItem==uStop)return 0;
					break;
				}
			}
		}

		pEntry = pData->pTreeItems[uItem];
		if(!pEntry)break;
		if((pEntry->uState&TVIS_SELECTED)==0)return uItem;
	}


	return 0;
}

//*****************************************************************************
//*
//*		TreeListMouseClick
//*
//*****************************************************************************
//	Soll ein Mausklick ausgef?rt werden. ACHTUNG UNLOCK wird ausgef?rt
//	pData	: Zeiger auf die Fensterdaten
//	uMsg	: Message des Mausklicks
//	wParam	: WPARAM des Mausklicks
//	lParam	: LPARAM des Mausklicks
static void TreeListMouseClick(TreeListData *pData,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	ExtraItem      *pExtra;
	BaseItem	   *pEntry;
	BaseItem	   *pTemp;
	TV_HITTESTINFO	sInfo;
	NMTREEVIEW		sNotify;
	unsigned		uMaskItem;
	unsigned		uMaskSub;
	unsigned		uMsgOld;
	unsigned		uItem;
	unsigned		uLine;
	unsigned		uTemp;
	unsigned		uStop;
	unsigned		uSub;
	unsigned		uSel;
	unsigned		uPos;
	unsigned		uNum;
	int				iMode;
	int				iAdd;



	if(!pData->cIsEnabled)									// Ist das Fenster freigegeben
	{
		return;
	}

	if(!pData->cHasFocus)									// Hat das Fenster den Focus
	{
		UNLOCK(pData);
		SetFocus(pData->hWnd);
		LOCK(pData);
	}

	sInfo.flags = (UINT) wParam;
	sInfo.pt.x	= LOWORD(lParam);
	sInfo.pt.y	= HIWORD(lParam);
	uItem		= TreeListHitTest(pData,&sInfo);
	uMsgOld		= uMsg;


	if(uItem)												// Wurde auf einen Eintrag getr?kt
	{
		pEntry = pData->pTreeItems[uItem];

		if(pData->uStyle&TVS_FULLROWSELECT)
		{
			uMaskItem = TVHT_ONITEMICON|TVHT_ONITEMLABEL|TVHT_ONITEMSTATEICON|TVHT_ONITEMRIGHT;
			uMaskSub  =	TVHT_ONSUBICON |TVHT_ONSUBLABEL |TVHT_ONSUBRIGHT;
		}
		else{
			uMaskItem = TVHT_ONITEMICON|TVHT_ONITEMLABEL|TVHT_ONITEMSTATEICON;
			uMaskSub  =	TVHT_ONSUBICON |TVHT_ONSUBLABEL;
		}


		if(sInfo.flags&TVHT_ONITEMBUTTON)					// Eintrag aufklappen
		{
			if(uMsg==WM_LBUTTONDOWN || uMsg==WM_LBUTTONDBLCLK)
			{
				pEntry = pData->pTreeItems[uItem];
				if(pEntry && (pEntry->uState&(TVIS_EXPANDED|TVIS_EXPANDPARTIAL))==(TVIS_EXPANDED|TVIS_EXPANDPARTIAL))
				{
					pEntry->uState &= ~TVIS_EXPANDPARTIAL;
					UpdateRect(pData,uItem,0);
				}
				else{
					TreeListToggleItem(pData,uItem,0);
				}
			}
		}
		else if(sInfo.flags&TVHT_ONITEMSTATEICON)			// Checkbox umschalten
		{
			if(pData->uStyle&TVS_CHECKBOXES)
				if(uMsg==WM_LBUTTONDOWN || uMsg==WM_LBUTTONDBLCLK)
				{
					TV_ITEM		sItem;
					NMTREEVIEW	sNotify;

					sItem.hItem		= (HTREEITEM)uItem;
					sItem.mask		= TVIF_STATE;
					sItem.stateMask	= TVIS_STATEIMAGEMASK;

					if(pData->uStyleEx&TVS_EX_BITCHECKBOX)
						sItem.state	= (pEntry->uState^0x1000);
					else	sItem.state	= (pEntry->uState&0x1000)?	0x2000:0x1000;

					TreeListSetItem(pData,&sItem);

					sNotify.hdr.code			= TVN_CBSTATECHANGED;
					sNotify.action				= 0;
					sNotify.itemNew.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_TEXT;
					sNotify.itemNew.hItem		= (HTREEITEM)uItem;
					sNotify.itemNew.stateMask	= 0xFFFFFFFF;
					sNotify.itemNew.state		= pEntry->uState;
					sNotify.itemNew.lParam		= pEntry->lParam;
					sNotify.itemNew.pszText		= pEntry->pText;
					sNotify.itemNew.cchTextMax	= pEntry->uTextSize;
					sNotify.itemNew.cChildren	= 0;
					sNotify.itemOld.mask		= 0;
					sNotify.ptDrag.x			= sInfo.pt.x;
					sNotify.ptDrag.y			= sInfo.pt.y;

					UNLOCK(pData);
					SendNotify(pData,&sNotify.hdr);
					LOCK(pData);
				}
		}
		else if(sInfo.flags&uMaskItem)						// Eintrag ausw?len
		{
			if(!(pData->uStyle&TVS_DISABLEDRAGDROP))
			{
				if(uMsg==WM_LBUTTONDOWN)
				{
					pData->uDragFlags = MK_LBUTTON;
					pData->uDragItem  = uItem;
					pData->uDragSub   = 0;
				}

				if(uMsg==WM_RBUTTONDOWN)
				{
					pData->uDragFlags = MK_RBUTTON;
					pData->uDragItem  = uItem;
					pData->uDragSub   = 0;
				}
			}

			if(pData->uStyleEx&TVS_EX_MULTISELECT)			// Mehrfachauswahl erlaubt
			{
				if(uMsg==WM_RBUTTONDOWN)					// Keine Abwahl wenn mehrer Eintr?e aus gew?lt
					if(pData->uSelectedCount>1)
					{
						pEntry=pData->pTreeItems[uItem];
						if(pEntry->uState&TVIS_SELECTED)
						{
							goto End;
						}
					}

					if(uMsg==WM_LBUTTONDOWN)					// Spezialsteuerung f? Multiselect bei Darg
						if(!(wParam&(MK_CONTROL|MK_SHIFT)))
							if(pData->uSelectedCount>0)
							{
								pEntry =   pData->pTreeItems[uItem];
								if(pEntry && (pEntry->uState&TVIS_SELECTED))
								{
									pData->cClickFlag = 1;
									goto End;
								}
							}

							if(uMsg==WM_LBUTTONUP && pData->cClickFlag)
							{
								wParam &= ~(MK_CONTROL|MK_SHIFT);
								uMsg    =   WM_LBUTTONDOWN;
							}


							if(wParam&MK_SHIFT)							// Bis zum angeklicken Ausw?len
							{
								if(uMsg!=WM_LBUTTONDOWN)
									if(uMsg!=WM_LBUTTONDBLCLK)goto End;

								uTemp = pData->uSelectedItem;
								if(!uTemp)
								{
									uTemp = pData->uFocusItem;
									if(!uTemp || !pData->cReSelect)goto End;
								}

								pEntry = pData->pTreeItems[uTemp];
								uLine  = pEntry->uShowPos;
								if(!uLine)goto End;

								pEntry = pData->pTreeItems[uItem];
								uStop  = pEntry->uShowPos;
								if(!uStop)goto End;

								if(pData->uSelectedCount<=1)
								{
									pData->uSelectedBase = uTemp;
								}

								// Shift-Select neu ausw?len
								if(pData->cReSelect && pData->uSelectedBase)
								{
									TreeListSelectItem   (pData,uItem,0,TVC_BYMOUSE|TVC_DESELECT);
									TreeListEnsureVisible(pData,pData->uSelectedItem,pData->uSelectedSub);

									uTemp = pData->uSelectedBase;
									pTemp = pData->pTreeItems[uTemp];
									uStop = uItem;


									while(pTemp && pTemp->uShowPos==0)
									{
										pTemp = pData->pTreeItems[pTemp->uParent];
										if(!pTemp)break;
									}

									if(pTemp)
									{
										if(!(pTemp->uState&TVIS_SELECTED))
										{
											TreeListXorSelectItem(pData,uTemp,TVC_BYKEYBOARD);
										}

										if(pTemp->uShowPos<pEntry->uShowPos)
										{
											uStop = uItem;
										}
										else{
											uStop = uTemp;
											uTemp = uItem;
										}

										for(;;)
										{
											uTemp=TreeListNextUnselUntil(pData,uTemp,uStop);
											if(!uTemp)break;
											TreeListXorSelectItem(pData,uTemp,TVC_BYKEYBOARD);
										}
									}


									pData->cReSelect = 0;
									goto End;
								}


								TreeListSelectItem(pData,uItem,0,TVC_BYMOUSE);

								iAdd = (uLine>uStop)? -1:1;
								uPos =  uLine;

								if(uPos!=uStop)
								{
									pEntry = pData->pTreeItems[pData->uSelectedBase];

									while(pEntry && pEntry->uShowPos==0)
									{
										pEntry = pData->pTreeItems[pEntry->uParent];
									}

									if(pEntry)
									{
										if(iAdd<0)
										{if(pEntry->uShowPos>uPos && uPos>uStop)uPos--;}
										else{if(pEntry->uShowPos<uPos && uPos<uStop)uPos++;}
									}
								}



								for(;;uPos+=iAdd)						// Eintr?e w?len
								{
									uTemp  = pData->pItemPos  [uPos-1];
									pEntry = pData->pTreeItems[uTemp ];
									uSel   = pEntry->uState&TVIS_SELECTED;

									if(uTemp!=pData->uSelectedBase || !uSel)
									{
										TreeListXorSelectItem(pData,uTemp,TVC_BYMOUSE);
										uSel = pEntry->uState&TVIS_SELECTED;
									}

									// Auch unsichtbare Kinder w?len
									pTemp =   pData->pTreeItems[pEntry->uFirstChild];
									if(pTemp && !pTemp->uShowPos)
									{
										if(uSel)
										{
											for(uNum=uTemp;;)			// Kinder ausw?len
											{
												uNum=TreeListNextUnselUntil(pData,uNum,uTemp);
												if(!uNum)break;
												TreeListXorSelectItem(pData,uNum,TVC_BYMOUSE);
											}
										}
										else{
											for(uNum=uTemp;;)			// Kinder abw?len
											{
												uNum=TreeListNextSelUntil(pData,uNum,uTemp);
												if(!uNum)break;
												TreeListXorSelectItem(pData,uNum,TVC_BYMOUSE);
											}
										}
									}

									if(uPos==uStop)break;
								}


								TreeListRemoveFocus(pData);

								pEntry = pData->pTreeItems[uItem];
								uSub   = TVHT_SUBTOCOL(sInfo.flags);
								if(!(pData->uStyleEx&TVS_EX_SUBSELECT))uSub=0;

								pData->uFocusSub     = uSub;
								pData->uFocusItem    = uItem;

								if(uSub)
								{
									pExtra =pData->pExtraItems[uSub-1][uItem];
									if(pExtra)pExtra->bFlags |= TVIX_FOCUSED;
								}
								else{
									pEntry->bFlags |= TVIX_FOCUSED;
								}

								UpdateRect(pData,uItem,uSub);

								TreeListSelectItem   (pData,uItem,0,TVC_BYMOUSE);
								TreeListEnsureVisible(pData,pData->uSelectedItem,pData->uSelectedSub);

								goto End;
							}


							if(wParam&MK_CONTROL)						// Einzelen Eintrag umschalten
							{
								pData->uSelectedBase = uItem;
								pData->cReSelect	 = 1;


								pEntry = pData->pTreeItems[uItem];
								if(pEntry && (pEntry->uState&TVIS_SELECTED))
								{
									if(uMsg!=WM_LBUTTONDOWN)
										if(uMsg!=WM_LBUTTONDBLCLK)goto End;

									TreeListSelectItem(pData,0,0,TVC_BYMOUSE);

									if(TreeListXorSelectItem(pData,uItem,TVC_BYMOUSE))
									{								// Auch unsichtbare Kinder abw?len
										pTemp =   pData->pTreeItems[pEntry->uFirstChild];
										if( pTemp && !pTemp->uShowPos)
											for(uTemp=uItem;;)
											{
												uTemp=TreeListNextSelUntil(pData,uTemp,uItem);
												if(!uTemp)break;
												TreeListXorSelectItem(pData,uTemp,TVC_BYMOUSE);
											}
									}

									uTemp = pData->uFocusItem;
									if(uTemp)							// Ist ein Focus definiert
									{
										TreeListRemoveFocus(pData);
									}

									uSub = TVHT_SUBTOCOL(sInfo.flags);
									if(!(pData->uStyleEx&TVS_EX_SUBSELECT))uSub=0;

									pData->uFocusItem = uItem;
									pData->uFocusSub  = uSub;

									if(uSub)
									{
										pExtra =pData->pExtraItems[uSub-1][uItem];
										if(pExtra)pExtra->bFlags |= TVIX_FOCUSED;
									}
									else{
										pEntry->bFlags |= TVIX_FOCUSED;
									}

									UpdateRect(pData,uItem,uSub);

									goto End;
								}
							}
			}

			if(wParam&MK_CONTROL)							// Ist die Ctrl-Taste gedr?kt
			{
				iMode = TVC_BYMOUSE;
			}
			else{
				iMode = TVC_BYMOUSE|TVC_DESELECT;
			}

			TreeListRemoveFocus  (pData);
			TreeListSelectItem   (pData,uItem,0,iMode);
			TreeListEnsureVisible(pData,pData->uSelectedItem,pData->uSelectedSub);
			pEntry = pData->pTreeItems[uItem];

			if(uMsg==WM_LBUTTONDBLCLK)
			{										
				if(pData->bColumnFlags[0]&TVAE_DBLCLICK)	// Auto-Edit mit Doppelklick
				{
					TreeListStartAutoEdit(pData,0,VK_DBLCLK,lParam);
				}
				else if(pEntry && (pEntry->uState&(TVIS_EXPANDED|TVIS_EXPANDPARTIAL))==(TVIS_EXPANDED|TVIS_EXPANDPARTIAL))
				{
					pEntry->uState &= ~TVIS_EXPANDPARTIAL;
					UpdateRect(pData,uItem,0);
				}
				else{
					TreeListToggleItem(pData,uItem,0);
				}
			}

		}
		else if(sInfo.flags&uMaskSub)						// Extra-Eintrag ausw?len
		{
			if(pData->uStyleEx&TVS_EX_SUBSELECT)
				uSub = TVHT_SUBTOCOL(sInfo.flags);
			else	uSub = 0;

			if(!(pData->uStyle&TVS_DISABLEDRAGDROP))
			{
				if(uMsg==WM_LBUTTONDOWN)
				{
					pData->uDragFlags = MK_LBUTTON;
					pData->uDragItem  = uItem;
					pData->uDragSub   = uSub;
				}

				if(uMsg==WM_RBUTTONDOWN)
				{
					pData->uDragFlags = MK_RBUTTON;
					pData->uDragItem  = uItem;
					pData->uDragSub   = uSub;
				}
			}

			if(wParam&MK_CONTROL)
			{
				if(pData->uStyleEx&TVS_EX_MULTISELECT)		// Auswahl umschalten
				{
					pEntry = pData->pTreeItems[uItem];
					if(pEntry && (pEntry->uState&TVIS_SELECTED))
					{
						if(uMsg!=WM_LBUTTONDOWN)
							if(uMsg!=WM_LBUTTONDBLCLK)goto End;

						TreeListSelectItem(pData,0,0,TVC_BYMOUSE);

						if(TreeListXorSelectItem(pData,uItem,TVC_BYMOUSE))
						{								// Auch unsichtbare Kinder abw?len
							pTemp =   pData->pTreeItems[pEntry->uFirstChild];
							if( pTemp && !pTemp->uShowPos)
								for(uTemp=uItem;;)
								{
									uTemp=TreeListNextSelUntil(pData,uTemp,uItem);
									if(!uTemp)break;
									TreeListXorSelectItem(pData,uTemp,TVC_BYMOUSE);
								}
						}

						TreeListRemoveFocus  (pData);

						pData->uFocusItem = uItem;
						pData->uFocusSub  = uSub;

						if(uSub)
						{
							pExtra =pData->pExtraItems[uSub-1][uItem];
							if(pExtra)pExtra->bFlags |= TVIX_FOCUSED;
						}
						else{
							pEntry->bFlags |= TVIX_FOCUSED;
						}

						UpdateRect(pData,uItem,uSub);

						goto End;
					}
				}

				iMode = TVC_BYMOUSE;
			}
			else{
				iMode = TVC_BYMOUSE|TVC_DESELECT;
			}

			TreeListRemoveFocus  (pData);
			TreeListSelectItem   (pData,uItem,uSub,(wParam&MK_CONTROL)? TVC_BYMOUSE:TVC_BYMOUSE|TVC_DESELECT);
			TreeListEnsureVisible(pData,pData->uSelectedItem,pData->uSelectedSub);
			pEntry = pData->pTreeItems[uItem];

			if(uMsg==WM_LBUTTONDBLCLK)
			{											
				if(pData->bColumnFlags[uSub]&TVAE_DBLCLICK) // Auto-Edit mit Doppelklick
				{
					TreeListStartAutoEdit(pData,uSub,(sInfo.flags&TVHT_ONSUBICON)? VK_ICONCLK:VK_DBLCLK,lParam);
				}
				else{
					TreeListToggleItem(pData,uItem,0);		// Eintrag auflappen
				}
			}
			else if(uMsg==WM_LBUTTONDOWN)
			{
				if(pData->bColumnFlags[uSub]&TVAE_ICONCLICK) // Auto-Edit mit Klick auf Icon
					if(sInfo.flags&TVHT_ONSUBICON)
					{
						TreeListStartAutoEdit(pData,uSub,VK_ICONCLK,lParam);
					}
			}
		}
	}
	else{
		pEntry = NULL;
		uSub   = 0;
	}

End:


	switch(uMsgOld)
	{
	case WM_LBUTTONDOWN:	sNotify.hdr.code = NM_CLICK;	break;
	case WM_LBUTTONDBLCLK:	sNotify.hdr.code = NM_DBLCLK;	break;
	case WM_RBUTTONDOWN:	sNotify.hdr.code = NM_RCLICK;	break;
	case WM_RBUTTONDBLCLK:	sNotify.hdr.code = NM_RDBLCLK;	break;
	default:				UNLOCK(pData);					return;
	}


	if(pEntry)
	{
		sNotify.itemNew.stateMask	= 0xFFFFFFFF;
		sNotify.itemNew.state		= pEntry->uState;
		sNotify.itemNew.lParam		= pEntry->lParam;
		sNotify.itemNew.cChildren	= TVHT_SUBTOCOL(sInfo.flags);
	}
	else{
		sNotify.itemNew.stateMask	= 0;
		sNotify.itemNew.state		= 0;
		sNotify.itemNew.lParam		= 0;
	}

	sNotify.action					= 0;
	sNotify.itemNew.mask			= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBNUMBER;
	sNotify.itemNew.hItem			= (HTREEITEM)uItem;
	sNotify.itemNew.pszText			= (LPTSTR)-1;
	sNotify.itemNew.cchTextMax		= -1;
	sNotify.itemOld.mask			= 0;
	sNotify.ptDrag.x				= LOWORD(lParam);
	sNotify.ptDrag.y				= HIWORD(lParam);


	UNLOCK(pData);

	SendNotify(pData,&sNotify.hdr);


}

//*****************************************************************************
//*
//*		TreeListKeyDown
//*
//*****************************************************************************
//	Soll ein Tastentruck ausgef?rt werden. ACHTUNG UNLOCK wird ausgef?rt
//	pData	: Zeiger auf die Fensterdaten
//	wParam	: WPARAM des Mausklicks
//	lParam	: LPARAM des Mausklicks
//	Ergibt 0 wenn die Taste verarbeitet wurde
static void TreeListKeyDown(TreeListData *pData,WPARAM wParam,LPARAM lParam)
{
	BaseItem	   *pTemp;
	BaseItem	   *pEntry;
	TV_KEYDOWN		sNotify;
	unsigned		uTemp;
	unsigned		uItem;
	unsigned		uStop;
	unsigned		uVal;
	int				iLineCmp;
	int				iOldLine;
	int				iOldSub;
	int				iBase;
	int				iLine;
	int				iAdd;
	int				iPos;
	int				iSub;
	int				iMax;
	int				iDel;
	int				iScr;
	int				iSel;


	if(!pData->cIsEnabled)									// Ist das Fenster freigegeben
	{
		return;
	}

	if(!pData->cHasFocus)									// Hat das Fenster den Focus
	{
		UNLOCK(pData);
		SetFocus(pData->hWnd);
		LOCK(pData);
	}

	if(wParam==VK_RETURN)									// Wurde Enter gedr?kt
	{
		UNLOCK(pData);

		sNotify.hdr.code	=  NM_RETURN;
		sNotify.wVKey		= (WORD)wParam;
		sNotify.flags		= (UINT)lParam;

		SendNotify(pData,&sNotify.hdr);

		iSub = pData->uSelectedSub;							// Auto-Edit starten
		if(pData->bColumnFlags[iSub]&7)
		{
			if(GetAsyncKeyState(VK_CONTROL)&0x8000)
			{
				wParam = VK_ICONCLK;
				lParam = 0;
			}

			TreeListStartAutoEdit(pData,iSub,wParam,lParam);
		}

		return;
	}


	pEntry = pData->pTreeItems[pData->uSelectedItem];
	iDel   = (GetAsyncKeyState(VK_SHIFT  )&0x8000)? 0:TVC_DESELECT;
	iScr   =  GetAsyncKeyState(VK_CONTROL)&0x8000;



	if(iDel && iScr)										// Das Fenster scrollen
	{
		iLine = pData->uScrollY;
		iSub  = pData->uScrollX;

		switch(wParam)
		{
		case VK_END:	iLine  = pData->uTreeItemsCount;	// Ans Ende scrollen
			iLine -= pData->uPageEnties;
			break;

		case VK_HOME:	iLine=0;							// Zum Anfang scrollen
			break;

		case VK_LEFT:	iSub-=16;							// Links scrollen
			break;

		case VK_RIGHT:	iSub+=16;							// Rechts scrollen
			break;

		case VK_UP:		iLine--;							// Nach oben scrollen
			break;

		case VK_DOWN:	iLine++;							// Nach unten scrollen
			break;

		case VK_PRIOR:	iLine-=pData->uSizeX;				// Eine Seite nach oben
			break;

		case VK_NEXT:	iLine+=pData->uSizeX;				// Eine Seite nach unten
			break;

		case VK_SPACE:	if(pEntry==NULL)break;				// Expantieren und schlie?n
			if(pEntry->bFlags&TVIX_HASBUTTON)
			{
				TreeListToggleItem(pData,pData->uSelectedItem,0);
			}


		}

		if(iLine!=(int)pData->uScrollY)
		{
			iMax  = pData->uItemPosCount;
			iMax -=	pData->uPageEnties-1;

			if(iLine>=iMax)iLine=iMax;
			if(iLine<0)iLine=0;
			if(iLine!=(int)pData->uScrollY)
			{
				pData->uScrollY = iLine;
				SetScrollPos(pData->hWnd,SB_VERT,iLine,TRUE);
				UpdateView(pData);
			}
		}

		if(iSub!=(int)pData->uScrollX)
		{
			uVal = pData->uColumnCount;
			if(uVal)iMax  = pData->iColumnXpos[uVal]-pData->uSizeX/2;
			else	iMax  = pData->iMaxSizeX;
			iMax -=	pData->uSizeX/2;

			if(iSub>=iMax)iSub=iMax;
			if(iSub<0)iSub=0;
			if(iSub!=(int)pData->uScrollX)
			{
				pData->uScrollX = iSub;
				SetScrollPos(pData->hWnd,SB_HORZ,iSub,TRUE);
				UpdateView(pData);

				if(pData->hHeader)
				{
					MoveWindow(pData->hHeader,-iSub,0,pData->uSizeX+iSub,pData->uStartPixel,TRUE);
				}
			}
		}

	}
	else{													// Einen anderen Eintrag ausw?len
		iSub  = pData->uSelectedSub;

		if(!pEntry)
		{
			iLine    =  0;
			iSub     =  0;
			iOldSub	 =  0;
			iOldLine = -1;

			uTemp = pData->uFocusItem;
			if(uTemp)										// Ist ein Focus definiert
			{
				pTemp = pData->pTreeItems[uTemp];

				while(pTemp && !pTemp->uShowPos)
				{
					pTemp = pData->pTreeItems[pTemp->uParent];
				}

				if(pTemp)
				{
					iLine = pTemp->uShowPos-1;
					iSub  = pData->uFocusSub;
				}


				if(pData->uSelectedCount>1)
				{
					iOldSub	 = iSub;
					iOldLine = iLine;
				}
				else{
					TreeListRemoveFocus(pData);
				}
			}
		}
		else{
			iLine =  pEntry->uShowPos-1;
			if(iLine<0)iLine=0;

			uTemp	 =  pData->uSelectedItem;
			iOldLine =  iLine;
			iOldSub	 =  iSub;
		}


		switch(wParam)
		{
		case VK_END:	iLine=pData->uItemPosCount-1;	break;
		case VK_HOME:	iLine=0;						break;
		case VK_LEFT:	if(pData->uColumnCount>1 && (pData->uStyleEx&TVS_EX_SUBSELECT))
				{
					iSub--;
				}
				else{
					if(! pEntry)break;
					if(!(pEntry->uState&TVIS_EXPANDED) || !pEntry->uFirstChild)
					{
						pTemp = pData->pTreeItems[pEntry->uParent];
						if(pTemp)iLine=pTemp->uShowPos-1;
						break;
					}

					if(pEntry->bFlags&TVIX_HASBUTTON)
					{
						TreeListToggleItem(pData,pData->uSelectedItem,0);
					}
				}

				break;

		case VK_RIGHT:	if(pData->uColumnCount>1 && (pData->uStyleEx&TVS_EX_SUBSELECT))
				{
					iSub++;
				}
				else{
					if(!pEntry)break;
					if( pEntry->uState&TVIS_EXPANDED)
					{
						iLine++;
						break;
					}

					if(pEntry->bFlags&TVIX_HASBUTTON)
					{
						TreeListToggleItem(pData,pData->uSelectedItem,0);
					}
				}

				break;

		case VK_UP:		iLine--;						break;
		case VK_DOWN:	iLine++;						break;

		case VK_PRIOR:	iAdd=pData->uPageEnties-1;			// Eine Seite nach oben
			if(iAdd<=0)iAdd=1;
			iLine-=iAdd;
			break;

		case VK_NEXT:	iAdd=pData->uPageEnties-1;			// Eine Seite nach unten
			if(iAdd<=0)iAdd=1;
			iLine+=iAdd;
			break;

		case VK_BACK:		  								// Eine Ebene h?er
			if(pEntry)
			{
				uItem = pEntry->uParent;
				if(!uItem)iLine=0;
				else{
					iLine = pData->pTreeItems[uItem]->uShowPos-1;
					if(iLine<0)iLine=0;
				}
			}

			break;

		case VK_SPACE:
			if(pEntry && iSub==0 && (pData->uStyle&TVS_CHECKBOXES))
			{
				TV_ITEM	 sItem;

				sItem.hItem		= (HTREEITEM)pData->uSelectedItem;
				sItem.mask		= TVIF_STATE;
				sItem.stateMask	= TVIS_STATEIMAGEMASK;
				sItem.state		= (pEntry->uState&0x1000)?	0x2000:0x1000;

				TreeListSetItem(pData,&sItem);
			}

		}

		if(iSub>=(int)pData->uColumnCount)iSub=pData->uColumnCount-1;
		if(iSub<0)iSub=0;
		if(iLine>=(int)pData->uItemPosCount)iLine=pData->uItemPosCount-1;
		if(iLine<0)iLine=0;

		if(!(pData->uStyleEx&TVS_EX_SUBSELECT))iSub=0;


		if(pData->uItemPosCount>0)
			if(iLine!=iOldLine || iSub!=iOldSub)				// Wurde ein anderer Eintrag ausgew?lt
			{
				if(pData->uSelectedCount>1)
				{
					TreeListRemoveFocus(pData);
				}
				else{
					pData->uSelectedBase = uTemp;
				}

				if(pData->cReSelect)							// Die Shift-Auswahl neu erstellen
				{
					iDel = TVC_DESELECT;
				}

				uItem = pData->pItemPos[iLine];
				TreeListSelectItem(pData,uItem,iSub,TVC_BYKEYBOARD|iDel);
				TreeListEnsureVisible(pData,pData->uSelectedItem,pData->uSelectedSub);

				if(pData->cReSelect && pData->uSelectedBase)	// Shift-Select neu ausw?len
				{
					uTemp = pData->uSelectedBase;
					pTemp = pData->pTreeItems[uTemp];
					uStop = uItem;


					while(pTemp && pTemp->uShowPos==0)
					{
						pTemp = pData->pTreeItems[pTemp->uParent];
						if(!pTemp)break;
					}

					if(pTemp)
					{
						if(!(pTemp->uState&TVIS_SELECTED))
						{
							TreeListXorSelectItem(pData,uTemp,TVC_BYKEYBOARD);
						}

						iLineCmp = pTemp->uShowPos-1;
						if(iLineCmp<iLine)
						{
							uStop = uItem;
						}
						else{
							uStop = uTemp;
							uTemp = uItem;
						}

						for(;;)
						{
							uTemp=TreeListNextUnselUntil(pData,uTemp,uStop);
							if(!uTemp)break;
							TreeListXorSelectItem(pData,uTemp,TVC_BYKEYBOARD);
						}
					}

					pData->cReSelect = 0;
				}


				if((pData->uStyleEx&TVS_EX_MULTISELECT) && !iDel)
				{
					uVal  = pData->uSelectedBase;
					pTemp = pData->pTreeItems[uVal];
					iBase =(pTemp)? pTemp->uShowPos-1:-1;

					if(iLine>iOldLine)							// Nach oben oder nach unten
					{
						iMax = (iOldLine>iBase)? iOldLine+1:iOldLine;
						if(iMax<0)iMax=0;
						iPos = iLine;
					}
					else{
						iMax = iLine;
						iPos = (iOldLine<iBase)? iOldLine-1:iOldLine;
						if(iPos<0)iPos=0;
					}

					for(;iPos>=iMax;iPos--)						// ?ersprungene Eintr?e mit aus/abw?len
					{
						uItem = pData->pItemPos[iPos];

						if(iPos!=iLine && iPos!=iBase)
						{
							if(!TreeListXorSelectItem(pData,uItem,TVC_BYKEYBOARD))continue;
						}

						pTemp = pData->pTreeItems[uItem];	// Auch unsichtbare Kinder abw?len
						if(pTemp)
						{
							iSel  =  pTemp->uState&TVIS_SELECTED;
							pTemp =  pData->pTreeItems[pTemp->uFirstChild];
							if(!pTemp || pTemp->uShowPos)continue;
							if(!iSel)
							{
								for(uTemp=uItem;;)
								{
									uTemp=TreeListNextSelUntil(pData,uTemp,uItem);
									if(!uTemp)break;
									TreeListXorSelectItem(pData,uTemp,TVC_BYKEYBOARD);
								}
							}
							else{
								for(uTemp=uItem;;)
								{
									uTemp=TreeListNextUnselUntil(pData,uTemp,uItem);
									if(!uTemp)break;
									TreeListXorSelectItem(pData,uTemp,TVC_BYKEYBOARD);
								}
							}
						}
					}
				}
			}
	}


	sNotify.hdr.code	=  TVN_KEYDOWN;
	sNotify.wVKey		= (WORD)wParam;
	sNotify.flags		= (UINT)lParam;

	UNLOCK(pData);

	SendNotify(pData,&sNotify.hdr);

	iSub = pData->uSelectedSub;								// Auto-Edit starten
	if(!iScr && pData->bColumnEdit[iSub])
	{
		TreeListStartAutoEdit(pData,iSub,wParam,lParam);
	}



}

//*****************************************************************************
//*
//*		TreeListSortItemsEx
//*
//*****************************************************************************
//	Sortiert die Kindereintr?e eines Eintrages mitteles einer Funktion
//	pData		: Zeiger auf die Fensterdaten
//	pSortData	: Ist ein Zeiger auf die Sortiertaten
//	iMode		: 1=Rekursiv sortieren
//	Ergibt 1 wenn ok sonst 0
static int TreeListSortItemsEx(TreeListData *pData,TV_SORTEX *pSortData,int iMode)
{
	TV_SORTEX	sSort;
	unsigned	uParent;
	unsigned	uFirst;
	unsigned	uItem;
	BaseItem   *pEntry;
	BaseItem   *pNext;
	BaseItem  **pList;



	uParent=U(pSortData->hParent);
	if(uParent>pData->uTreeItemsMax)						// Root-Eintrag sortieren
	{
		if(uParent!=U(TVI_ROOT))return 0;

		uFirst = pData->uFirstChild;
		if(uFirst==0)return 1;
		if(uFirst==pData->uLastChild)
		{
			if(uFirst)
			{
				sSort.hParent		= (HTREEITEM)uFirst;
				sSort.lParam		= pSortData->lParam;
				sSort.lpfnCompare	= pSortData->lpfnCompare;
				TreeListSortItemsEx(pData,&sSort,iMode);
			}
			return 1;
		}

		pList   = pData->pTreeItems;
		uParent = 0;
	}
	else{													// Untereintrag sortieren
		pList = pData->pTreeItems;

		pEntry = pList[uParent];
		if(pEntry==NULL)return 0;

		uFirst = pEntry->uFirstChild;
		if(uFirst==0)return 1;
		if(uFirst==pEntry->uLastChild)
		{
			if(uFirst)
			{
				sSort.hParent		= (HTREEITEM)uFirst;
				sSort.lParam		= pSortData->lParam;
				sSort.lpfnCompare	= pSortData->lpfnCompare;
				TreeListSortItemsEx(pData,&sSort,iMode);
			}
			return 1;
		}
	}


	if(iMode)												// Sortiere die Untereintr?e
	{
		pNext				= pList[uFirst];
		sSort.hParent		= (HTREEITEM)uFirst;
		sSort.lParam		= pSortData->lParam;
		sSort.lpfnCompare	= pSortData->lpfnCompare;

		do	{
			TreeListSortItemsEx(pData,&sSort,SORT_NOUPDATE);
			sSort.hParent	= (HTREEITEM)pNext->uNextItem;
			pNext			= pList[pNext->uNextItem];
		}while(pNext);
	}


	//******************** Erzeuge Eintragsliste **********************************
	unsigned   *pItemList;
	unsigned   *pItemNew;
	unsigned	uEnties[128];
	unsigned	uPos;
	unsigned	uMax;


	uItem		= uFirst;
	pItemList	= uEnties;
	uMax		= 128;
	uPos		= 0;

	do	{													// Alle Kindeintr?e suchen
		if(uPos>=uMax)
		{
			uMax *= 2;
			pItemNew	= new unsigned[uMax];
			memcpy(pItemNew,pItemList,uPos*sizeof(pItemList[0]));
			if(uPos>128)delete pItemList;
			pItemList = pItemNew;
		}

		pItemList[uPos] = uItem;
		pNext			= pList[uItem];
		uItem			= pNext->uNextItem;
		uPos++;
	}while(uItem);




	//************************* Qsort-Algorithmus *********************************
#define XCHANGE_MEM(a,b)	uTemp=pItemList[a];pItemList[a]=pItemList[b];pItemList[b]=uTemp;
	PFNTVCOMPAREEX	pCompare;
	HTREEITEM		hItemTemp;
	LPARAM			lParamTemp;
	LPARAM			lParamSort;
	HWND			hTreeWnd;
	int				iLower,iUpper,iMiddle,iCmp;
	int				uMemL[30],uMemU[30];
	int				iStart,iLast;
	int				iLevel;
	unsigned		uTemp;


	pData->cLockChanges=1;
	UNLOCK(pData);

	hTreeWnd	= pData->hWnd;
	pCompare	= pSortData->lpfnCompare;
	lParamSort	= pSortData->lParam;
	iLast		= uPos-1;
	iStart		= 0;
	iLevel		= 0;

	for(;;)
	{
		iLower	=  iStart;
		iMiddle = (iStart+iLast)>>1;						// Mitte bereichnen
		iUpper	=  iLast +1;


		XCHANGE_MEM(iMiddle,iLower);

		uItem	   = pItemList[iStart];
		hItemTemp  = (HTREEITEM)uItem;
		lParamTemp = pList[uItem]->lParam;


		for(;;)
		{
			do  {
				iLower++;
				if(iLower>iLast)break;
				uItem = pItemList[iLower];
				iCmp=pCompare(hTreeWnd,(HTREEITEM)uItem,hItemTemp,pList[uItem]->lParam,lParamTemp,lParamSort);
			}while(iCmp<=0);


			do  {
				iUpper--;
				if(iUpper<=iStart)break;
				uItem = pItemList[iUpper];
				iCmp=pCompare(hTreeWnd,(HTREEITEM)uItem,hItemTemp,pList[uItem]->lParam,lParamTemp,lParamSort);
			}while(iCmp>=0);

			if(iUpper<iLower)break;

			XCHANGE_MEM(iUpper,iLower);
		}


		XCHANGE_MEM(iStart,iUpper);


		if(iUpper-1-iStart >= iLast-iLower )
		{
			if(iStart+1 < iUpper)
			{
				uMemL[iLevel] = iStart;
				uMemU[iLevel] = iUpper-1;
				iLevel++;
			}
			if(iLower < iLast)
			{
				iStart = iLower;
				continue;
			}
		}
		else{
			if(iLower<iLast)
			{
				uMemL[iLevel] = iLower;
				uMemU[iLevel] = iLast;
				iLevel++;
			}

			if(iStart+1 < iUpper)
			{
				iLast = iUpper-1;
				continue;
			}
		}
		// Eine Ebene absteigen
		iLevel--;

		if(iLevel>=0)										// Noch Ebenen vorhanden
		{
			iStart = uMemL[iLevel];
			iLast  = uMemU[iLevel];
			continue;
		}

		break;
	}


	LOCK(pData);
	pData->cLockChanges=0;


	//******************** Eintr?e neu einsortirenen *****************************
	unsigned	uNum;
	unsigned	uLast;



	uPos--;


	pEntry = pList[uParent];
	if(!pEntry)
	{
		pData->uFirstChild  = pItemList[ 0  ];
		pData->uLastChild   = pItemList[uPos];
	}
	else{
		pEntry->uFirstChild = pItemList[ 0  ];
		pEntry->uLastChild  = pItemList[uPos];
	}


	uLast  = 0;
	uItem  = pItemList[0];

	for(uNum=0;uNum<uPos;)									// Kinder neu einh?gen
	{
		pEntry = pList[uItem];
		pEntry->uPrevItem = uLast;

		uNum++;
		uLast  = uItem;
		uItem  = pItemList[uNum];

		pEntry->uNextItem = uItem;
	}

	pEntry = pList[uItem];
	pEntry->uPrevItem = uLast;
	pEntry->uNextItem = 0;


	if(iMode!=SORT_NOUPDATE)								// Ausgabeliste neuerstellen
	{
		UpdateItems(pData,uParent);

		if(pData->uStyle&TVS_SHOWSELALWAYS)
			if(pData->uSelectedItem)
			{
				TreeListEnsureVisible(pData,pData->uSelectedItem,pData->uSelectedSub);
			}
	}


	if(uMax>128)delete pItemList;



	return 1;
}

//*****************************************************************************
//*
//*		TreeListSortItemsCb
//*
//*****************************************************************************
//	Sortiert die Kindereintr?e eines Eintrages mitteles einer Funktion
//	pData		: Zeiger auf die Fensterdaten
//	pSortData	: Ist ein Zeiger auf die Sortiertaten
//	iMode		: 1=Rekursiv sortieren
//	Ergibt 1 wenn ok sonst 0
static int TreeListSortItemsCb(TreeListData *pData,TV_SORTCB *pSortData,int iMode)
{
	TV_SORTCB	sSort;
	unsigned	uParent;
	unsigned	uFirst;
	unsigned	uItem;
	BaseItem   *pEntry;
	BaseItem   *pNext;
	BaseItem  **pList;



	uParent=U(pSortData->hParent);
	if(uParent>pData->uTreeItemsMax)						// Root-Eintrag sortieren
	{
		if(uParent!=U(TVI_ROOT))return 0;

		uFirst = pData->uFirstChild;
		if(uFirst==0)return 1;
		if(uFirst==pData->uLastChild)
		{
			if(uFirst)
			{
				sSort.hParent		= (HTREEITEM)uFirst;
				sSort.lParam		= pSortData->lParam;
				sSort.lpfnCompare	= pSortData->lpfnCompare;
				TreeListSortItemsCb(pData,&sSort,iMode);
			}
			return 1;
		}

		pList   = pData->pTreeItems;
		uParent = 0;
	}
	else{													// Untereintrag sortieren
		pList = pData->pTreeItems;

		pEntry = pList[uParent];
		if(pEntry==NULL)return 0;

		uFirst = pEntry->uFirstChild;
		if(uFirst==0)return 1;
		if(uFirst==pEntry->uLastChild)
		{
			if(uFirst)
			{
				sSort.hParent		= (HTREEITEM)uFirst;
				sSort.lParam		= pSortData->lParam;
				sSort.lpfnCompare	= pSortData->lpfnCompare;
				TreeListSortItemsCb(pData,&sSort,iMode);
			}
			return 1;
		}
	}


	if(iMode)												// Sortiere die Untereintr?e
	{
		pNext				= pList[uFirst];
		sSort.hParent		= (HTREEITEM)uFirst;
		sSort.lParam		= pSortData->lParam;
		sSort.lpfnCompare	= pSortData->lpfnCompare;

		do	{
			TreeListSortItemsCb(pData,&sSort,SORT_NOUPDATE);
			sSort.hParent	= (HTREEITEM)pNext->uNextItem;
			pNext			= pList[pNext->uNextItem];
		}while(pNext);
	}


	//******************** Erzeuge Eintragsliste **********************************
	unsigned   *pItemList;
	unsigned   *pItemNew;
	unsigned	uEnties[128];
	unsigned	uPos;
	unsigned	uMax;


	uItem		= uFirst;
	pItemList	= uEnties;
	uMax		= 128;
	uPos		= 0;

	do	{													// Alle Kindeintr?e suchen
		if(uPos>=uMax)
		{
			uMax *= 2;
			pItemNew	= new unsigned[uMax];
			memcpy(pItemNew,pItemList,uPos*sizeof(pItemList[0]));
			if(uPos>128)delete pItemList;
			pItemList = pItemNew;
		}

		pItemList[uPos] = uItem;
		pNext			= pList[uItem];
		uItem			= pNext->uNextItem;
		uPos++;
	}while(uItem);




	//************************* Qsort-Algorithmus *********************************
#define XCHANGE_MEM(a,b)	uTemp=pItemList[a];pItemList[a]=pItemList[b];pItemList[b]=uTemp;
	PFNTVCOMPARE	pCompare;
	LPARAM			lParamTemp;
	LPARAM			lParamSort;
	int				iLower,iUpper,iMiddle,iCmp;
	int				uMemL[30],uMemU[30];
	int				iStart,iLast;
	int				iLevel;
	unsigned		uTemp;


	pData->cLockChanges=1;
	UNLOCK(pData);

	pCompare	= pSortData->lpfnCompare;
	lParamSort	= pSortData->lParam;
	iLast		= uPos-1;
	iStart		= 0;
	iLevel		= 0;

	for(;;)
	{
		iLower	=  iStart;
		iMiddle = (iStart+iLast)>>1;						// Mitte bereichnen
		iUpper	=  iLast +1;


		XCHANGE_MEM(iMiddle,iLower);

		uItem	   = pItemList[iStart];
		lParamTemp = pList[uItem]->lParam;

		for(;;)
		{
			do  {
				iLower++;
				if(iLower>iLast)break;
				uItem = pItemList[iLower];
				iCmp=pCompare(pList[uItem]->lParam,lParamTemp,lParamSort);
			}while(iCmp<=0);

			do  {
				iUpper--;
				if(iUpper<=iStart)break;
				uItem = pItemList[iUpper];
				iCmp=pCompare(pList[uItem]->lParam,lParamTemp,lParamSort);
			}while(iCmp>=0);

			if(iUpper<iLower)break;

			XCHANGE_MEM(iUpper,iLower);
		}


		XCHANGE_MEM(iStart,iUpper);


		if(iUpper-1-iStart >= iLast-iLower )
		{
			if(iStart+1 < iUpper)
			{
				uMemL[iLevel] = iStart;
				uMemU[iLevel] = iUpper-1;
				iLevel++;
			}
			if(iLower < iLast)
			{
				iStart = iLower;
				continue;
			}
		}
		else{
			if(iLower<iLast)
			{
				uMemL[iLevel] = iLower;
				uMemU[iLevel] = iLast;
				iLevel++;
			}

			if(iStart+1 < iUpper)
			{
				iLast = iUpper-1;
				continue;
			}
		}
		// Eine Ebene absteigen
		iLevel--;

		if(iLevel>=0)										// Noch Ebenen vorhanden
		{
			iStart = uMemL[iLevel];
			iLast  = uMemU[iLevel];
			continue;
		}

		break;
	}


	LOCK(pData);
	pData->cLockChanges=0;


	//******************** Eintr?e neu einsortirenen *****************************
	unsigned	uNum;
	unsigned	uLast;



	uPos--;


	pEntry = pList[uParent];
	if(!pEntry)
	{
		pData->uFirstChild  = pItemList[ 0  ];
		pData->uLastChild   = pItemList[uPos];
	}
	else{
		pEntry->uFirstChild = pItemList[ 0  ];
		pEntry->uLastChild  = pItemList[uPos];
	}


	uLast  = 0;
	uItem  = pItemList[0];

	for(uNum=0;uNum<uPos;)									// Kinder neu einh?gen
	{
		pEntry = pList[uItem];
		pEntry->uPrevItem = uLast;

		uNum++;
		uLast  = uItem;
		uItem  = pItemList[uNum];

		pEntry->uNextItem = uItem;
	}

	pEntry = pList[uItem];
	pEntry->uPrevItem = uLast;
	pEntry->uNextItem = 0;


	if(iMode!=SORT_NOUPDATE)								// Ausgabeliste neuerstellen
	{
		UpdateItems(pData,uParent);

		if(pData->uStyle&TVS_SHOWSELALWAYS)
			if(pData->uSelectedItem)
			{
				TreeListEnsureVisible(pData,pData->uSelectedItem,pData->uSelectedSub);
			}
	}


	if(uMax>128)delete pItemList;



	return 1;
}

//*****************************************************************************
//*
//*		TreeListSortItems
//*
//*****************************************************************************
//	Sortiert die Kindereintr?e eines Eintrages via Text
//	pData		: Zeiger auf die Fensterdaten
//	pSortData	: Ist ein Zeiger auf die Sortiertaten
//	iMode		: 1=Rekursiv sortieren
//	Ergibt 1 wenn ok sonst 0
static int TreeListSortItems(TreeListData *pData,unsigned uParent,int iMode)
{
	unsigned	uItem;
	unsigned	uFirst;
	BaseItem   *pEntry;
	BaseItem   *pNext;
	BaseItem  **pList;



	if(uParent>pData->uTreeItemsMax)						// Root-Eintrag sortieren
	{
		if(uParent!=U(TVI_ROOT))return 0;

		uFirst = pData->uFirstChild;
		if(uFirst==0)return 1;
		if(uFirst==pData->uLastChild)
		{
			if(uFirst)
			{
				TreeListSortItems(pData,uFirst,iMode);
			}
			return 1;
		}

		pList   = pData->pTreeItems;
		uParent = 0;
	}
	else{													// Untereintrag sortieren
		pList = pData->pTreeItems;

		pEntry = pList[uParent];
		if(pEntry==NULL)return 0;

		uFirst = pEntry->uFirstChild;
		if(uFirst==0)return 1;
		if(uFirst==pEntry->uLastChild)
		{
			if(uFirst)
			{
				TreeListSortItems(pData,uFirst,iMode);
			}
			return 1;
		}
	}


	if(iMode)												// Sortiere die Untereintr?e
	{
		uItem = uFirst;

		do	{
			TreeListSortItems(pData,uItem,SORT_NOUPDATE);
			pNext = pList[uItem];
			uItem = pNext->uNextItem;
		}while(uItem);
	}


	//******************** Erzeuge Eintragsliste **********************************
	unsigned   *pItemList;
	unsigned   *pItemNew;
	unsigned	uEnties[128];
	unsigned	uPos;
	unsigned	uMax;


	uItem		= uFirst;
	pItemList	= uEnties;
	uMax		= 128;
	uPos		= 0;

	do	{													// Alle Kindeintr?e suchen
		if(uPos>=uMax)
		{
			uMax *= 2;
			pItemNew	= new unsigned[uMax];
			memcpy(pItemNew,pItemList,uPos*sizeof(pItemList[0]));
			if(uPos>128)delete pItemList;
			pItemList = pItemNew;
		}

		pItemList[uPos] = uItem;
		pNext			= pList[uItem];
		uItem			= pNext->uNextItem;
		uPos++;
	}while(uItem);




	//************************* Qsort-Algorithmus *********************************
#define XCHANGE_MEM(a,b)	uTemp=pItemList[a];pItemList[a]=pItemList[b];pItemList[b]=uTemp;
	LPCTSTR			pTextTemp;
	LPCTSTR			pText;
	int				iLower,iUpper,iMiddle,iCmp;
	int				uMemL[30],uMemU[30];
	int				iStart,iLast;
	int				iLevel;
	int				iNone;
	unsigned		uSize;
	unsigned		uTemp;


	pData->cLockChanges=1;
	UNLOCK(pData);

	iLast		= uPos-1;
	iStart		= 0;
	iLevel		= 0;

	for(;;)
	{
		iLower	=  iStart;
		iMiddle = (iStart+iLast)>>1;						// Mitte bereichnen
		iUpper	=  iLast +1;


		XCHANGE_MEM(iMiddle,iLower);

		uItem  = pItemList[iStart];
		pEntry = pList[uItem];
		if(pEntry->bCallback&TVIF_TEXT)
		{
			uSize=0;
			LOCK(pData);
			CallbackEntry(pData,pEntry,uItem,TVIF_TEXT,iNone,uSize,pTextTemp);
			UNLOCK(pData);
		}
		else{
			pTextTemp = pEntry->pText;
		}


		for(;;)
		{
			do  {
				iLower++;
				if(iLower>iLast)break;

				uItem  = pItemList[iLower];
				pEntry = pList[uItem];
				if(pEntry->bCallback&TVIF_TEXT)
				{
					uSize=1;
					LOCK(pData);
					CallbackEntry(pData,pEntry,uItem,TVIF_TEXT,iNone,uSize,pText);
					UNLOCK(pData);
				}
				else{
					pText = pEntry->pText;
				}

				iCmp=str_icmp(pText,pTextTemp);
			}while(iCmp<=0);

			do  {
				iUpper--;

				if(iUpper<=iStart)break;
				uItem  = pItemList[iUpper];
				pEntry = pList[uItem];
				if(pEntry->bCallback&TVIF_TEXT)
				{
					LOCK(pData);
					CallbackEntry(pData,pEntry,uItem,TVIF_TEXT,iNone,uSize,pText);
					UNLOCK(pData);
				}
				else{
					pText = pEntry->pText;
				}

				iCmp=str_icmp(pText,pTextTemp);
			}while(iCmp>=0);

			if(iUpper<iLower)break;

			XCHANGE_MEM(iUpper,iLower);
		}


		XCHANGE_MEM(iStart,iUpper);


		if(iUpper-1-iStart >= iLast-iLower )
		{
			if(iStart+1 < iUpper)
			{
				uMemL[iLevel] = iStart;
				uMemU[iLevel] = iUpper-1;
				iLevel++;
			}
			if(iLower < iLast)
			{
				iStart = iLower;
				continue;
			}
		}
		else{
			if(iLower<iLast)
			{
				uMemL[iLevel] = iLower;
				uMemU[iLevel] = iLast;
				iLevel++;
			}

			if(iStart+1 < iUpper)
			{
				iLast = iUpper-1;
				continue;
			}
		}
		// Eine Ebene absteigen
		iLevel--;

		if(iLevel>=0)										// Noch Ebenen vorhanden
		{
			iStart = uMemL[iLevel];
			iLast  = uMemU[iLevel];
			continue;
		}

		break;
	}


	LOCK(pData);
	pData->cLockChanges=0;


	//******************** Eintr?e neu einsortirenen *****************************
	unsigned	uNum;
	unsigned	uLast;



	uPos--;


	pEntry = pList[uParent];
	if(!pEntry)
	{
		pData->uFirstChild  = pItemList[ 0  ];
		pData->uLastChild   = pItemList[uPos];
	}
	else{
		pEntry->uFirstChild = pItemList[ 0  ];
		pEntry->uLastChild  = pItemList[uPos];
	}


	uLast  = 0;
	uItem  = pItemList[0];

	for(uNum=0;uNum<uPos;)									// Kinder neu einh?gen
	{
		pEntry = pList[uItem];
		pEntry->uPrevItem = uLast;

		uNum++;
		uLast  = uItem;
		uItem  = pItemList[uNum];

		pEntry->uNextItem = uItem;
	}

	pEntry = pList[uItem];
	pEntry->uPrevItem = uLast;
	pEntry->uNextItem = 0;


	if(iMode!=SORT_NOUPDATE)								// Ausgabeliste neuerstellen
	{
		UpdateItems(pData,uParent);

		if(pData->uStyle&TVS_SHOWSELALWAYS)
			if(pData->uSelectedItem)
			{
				TreeListEnsureVisible(pData,pData->uSelectedItem,pData->uSelectedSub);
			}
	}


	if(uMax>128)delete pItemList;



	return 1;
}

//*****************************************************************************
//*
//*		TreeListEndLabelEdit
//*
//*****************************************************************************
//	Beendet das editiert eines Eintrages
//	pData	: Zeiger auf Fensterdaten
//	iMode	: Gibt an wie das Editieren beendet wurde 
//				0 = Es gab es einen Abbruch mit ESC
//				1 = Eingabe ohne Enter
//				2 = Eingabe mit  Enter
//	Ergibt das Handle des Edit-Controlls oder NULL bei einem Fehler
static int TreeListEndLabelEdit(TreeListData *pData,int iMode)
{
	TCHAR			cText[260];
	NMTVDISPINFO	sNotify;
	TV_ITEM			sSet;
	LRESULT			lRet;
	unsigned		uSub;
	unsigned		uItem;
	ExtraItem	   *pExtra;
	BaseItem	   *pEntry;
	LPCTSTR		   *pList;	
	LPCTSTR			pText;
	TCHAR			cChar;
	int 			iAuto;
	int				iIcon;
	int				iLine;
	int				iPos;
	int				iLen;
	int				iMax;
	int				iSel;
	char			cCb;




	uItem = pData->uEditItem;
	uSub  = pData->uEditSub;
	cCb	  =	pData->cEditCb;

	pData->uEditItem = 0;
	pData->uEditSub  = 0;
	pData->cEditCb	 = 0;


	if(uItem>pData->uTreeItemsMax || uSub>=pData->uColumnCount)
	{
		return 0;
	}

	pEntry = pData->pTreeItems[uItem];
	if(!pEntry)
	{
		return 0;
	}


	if(iMode)												// Eingabe
	{
		GetWindowText(pData->hEdit,cText,sizeof(cText)/sizeof(cText[0]));
		cText[sizeof(cText)/sizeof(cText[0])-1]=0;
		sNotify.item.cchTextMax = sizeof(cText)/sizeof(cText[0])-1;
		sNotify.item.pszText	= cText;
	}
	else{													// Abbruch
		sNotify.item.cchTextMax = 0;
		sNotify.item.pszText	= NULL;
	}


	sNotify.hdr.code			= TVN_ENDLABELEDIT;
	sNotify.item.mask			= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_TEXT|TVIF_SUBITEM;
	sNotify.item.hItem			= (HTREEITEM)uItem;
	sNotify.item.stateMask		= 0xFFFFFFFF;
	sNotify.item.cChildren		= uSub;

	if(uSub)
	{
		pExtra = pData->pExtraItems[uSub-1][uItem];
		if(!pExtra)
		{
			sNotify.item.state	= 0;
			sNotify.item.lParam	= 0;
		}
		else{
			sNotify.item.state  = pEntry->uState&TVIS_BASEFLAGS;
			sNotify.item.state |= pExtra->uState;
			sNotify.item.lParam	= pEntry->lParam;
		}
	}
	else{
		sNotify.item.state	= pEntry->uState;
		sNotify.item.lParam	= pEntry->lParam;
	}


	UNLOCK(pData);
	ShowWindow(pData->hEdit,SW_HIDE);
	lRet=SendNotify(pData,&sNotify.hdr);
	LOCK(pData);

	if(lRet || !iMode)return 0;



	if(cCb)													// Callback aufrufen
	{
		sNotify.hdr.code		= TVN_SETDISPINFO;
		sNotify.item.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_TEXT|TVIF_SUBITEM;
		sNotify.item.stateMask	= (UINT)~TVIS_BASEFLAGS;
		sNotify.item.hItem		= (HTREEITEM)uItem;
		sNotify.item.cchTextMax = sizeof(cText)/sizeof(cText[0])-1;
		sNotify.item.pszText	= cText;
		sNotify.item.cChildren	= uSub;

		UNLOCK(pData);
		SendNotify(pData,&sNotify.hdr);
		LOCK(pData);
	}
	else{													// Neuen Text eingeben
		sSet.mask		= TVIF_SUBITEM|TVIF_TEXT;
		sSet.cchTextMax	= sizeof(cText)/sizeof(cText[0]);
		sSet.hItem		= (HTREEITEM)uItem;
		sSet.cChildren	= uSub;
		sSet.pszText	= cText;
		iIcon			= pData->iColumnCbIcon[uSub];
		iAuto			= pData->bColumnEdit  [uSub];


		//if(iIcon>=0)										// Auch ein Icon zuweisen
		//	{
		//	iPos = -1;
		//	
		//	if((1<<iAuto)&((1<<TVAX_CBLIST)|(1<<TVAX_COMBO)))
		//		{
		//		iPos = SendMessage(pData->hEdit,CB_GETCURSEL,0,0);
		//		sSet.mask		   |= TVIF_IMAGE|TVIF_SELECTEDIMAGE;
		//		sSet.iImage			= iIcon+iPos;
		//		sSet.iSelectedImage	= sSet.iImage;
		//		}
		//	
		//	if(iPos<=-1)
		//		{
		//		   iMax =pData->bColumnCbSize[uSub];
		//		if(iMax<=0)iMax=1024;

		//		iSel = -1;


		//		if(pData->bColumnEdit[uSub]&TVAE_PTRLIST)	// Zeigerliste  char *pTexte[]={"1","2",NULL};
		//			{
		//			pList = (LPCTSTR*)pData->pColumnCbData[uSub];

		//			for(iPos=0;iPos<iMax;iPos++)
		//				{
		//				if(!pList[iPos])break;
		//				if(!str_cmp(cText,pList[iPos]))iSel=iPos;
		//				}
		//			}
		//		else{										// Textliste  char *pText="1|2|3";
		//			pText = (LPTSTR)pData->pColumnCbData[uSub];
		//			cChar = (TCHAR )pData->bColumnCbChar[uSub];

		//			for(iPos=0;iPos<iMax;iPos++)
		//				{
		//				for(iLen=0;pText[iLen];iLen++)
		//					{
		//					if(pText[iLen]==cChar)break;
		//					}

		//				if(str_ncmp(cText,pText,iLen)==0)
		//					{
		//					iSel = iPos;
		//					break;
		//					}

		//				   pText+=iLen;
		//				if(pText[0]==cChar)pText++;
		//				if(pText[0]==0)break;
		//				}
		//			}

		//		sSet.mask		   |= TVIF_IMAGE|TVIF_SELECTEDIMAGE;
		//		sSet.iImage			= iIcon+iSel;
		//		sSet.iSelectedImage	= sSet.iImage;
		//		}
		//	}

		TreeListSetItem(pData,&sSet);

		if(pData->bColumnFlags[uSub]&TVAE_NEXTLINE)			// In die n?hste Zeile springen
			if(iMode==2 && iAuto!=TVAX_NONE && iAuto!=TVAX_STEP)
			{
				iLine = pEntry->uShowPos;
				if(iLine<(int)pData->uItemPosCount)
				{
					uItem = pData->pItemPos[iLine];
					TreeListSelectItem(pData,uItem,pData->uSelectedSub,TVC_BYKEYBOARD|TVC_DESELECT);
				}
			}
	}



	return 1;
}

//*****************************************************************************
//*
//*		TreeListEditLabel
//*
//*****************************************************************************
//	Startet das editiert eines Eintrages
//	pData	: Zeiger auf Fensterdaten
//	uItem	: Item das editiert werden soll
//	uSub	: Spalte die editiert werden soll
//	Ergibt das Handle des Edit-Controlls oder NULL bei einem Fehler
static HWND TreeListEditLabel(TreeListData *pData,unsigned uItem,unsigned uSub)
{
	HDC				hDc;
	HWND			hWnd;
	LRESULT			lRet;
	LPARAM			lParam;
	ExtraItem	   *pExtra;
	BaseItem	   *pEntry;
	NMTVDISPINFO	sNotify;
	WNDPROC			pProc;
	LPCTSTR			pText;
	RECT			sRect;
	HFONT			hFont;
	unsigned		uCol;
	unsigned		uBits;
	unsigned		uSize;
	unsigned		uState;
	unsigned		uHeight;
	int				iPixels;
	int				iWidth;
	int				iTemp;



	uBits	= uSub>>29;
	uSub   &= 0x1FFFFFFF;


	if( uSub >=pData->uColumnCount )return NULL;
	if( uItem> pData->uTreeItemsMax)return NULL;
	pEntry=pData->pTreeItems[uItem];
	if(!pEntry)return NULL;


	if(uItem!=pData->uSelectedItem || uSub!=pData->uSelectedSub)
	{
		uCol  = (pData->uStyleEx&TVS_EX_SUBSELECT)? uSub:0;
		iTemp = TreeListSelectItem(pData,uItem,uCol,TVC_UNKNOWN);
	}

	TreeListEnsureVisible(pData,uItem,uSub);

	if(pData->hEdit)										// Editfenster l?chen
	{
		DestroyWindow(pData->hEdit);
		pData->hEdit=0;
	}


	switch(uBits&3)											// Editfenster neu erzeugen
	{
	case 1:	pData->hEdit=CreateWindow(_T("COMBOBOX"),NULL,WS_BORDER|WS_CHILD|CBS_AUTOHSCROLL|CBS_DROPDOWN,0,0,0,0,pData->hWnd,NULL,NULL,NULL);
		pData->uEditMode=1;
		break;

	case 2:	pData->hEdit=CreateWindow(_T("COMBOBOX"),NULL,WS_BORDER|WS_CHILD|CBS_AUTOHSCROLL|CBS_DROPDOWNLIST,0,0,0,0,pData->hWnd,NULL,NULL,NULL);
		pData->uEditMode=2;
		break;

	default:pData->hEdit=CreateWindow(_T("EDIT"    ),NULL,WS_BORDER|WS_CHILD|ES_AUTOHSCROLL|ES_LEFT,0,0,0,0,pData->hWnd,NULL,NULL,NULL);
		pData->uEditMode=0;
		break;
	}



	if(!pData->hEdit)return NULL;

	pProc = (WNDPROC)GetWindowLongPtr(pData->hEdit,GWL_WNDPROC);

	SetWindowLongPtr(pData->hEdit,GWL_USERDATA,(LPARAM)pProc);
	SetWindowLongPtr(pData->hEdit,GWL_WNDPROC ,(LPARAM)EditProc);
	SetWindowLong   (pData->hEdit,GWL_ID,3);

	hWnd = GetWindow(pData->hEdit,GW_CHILD);

	while(hWnd)
	{
		pProc = (WNDPROC)GetWindowLongPtr(hWnd,GWL_WNDPROC);
		SetWindowLongPtr(hWnd,GWL_USERDATA,(LPARAM)pProc);
		SetWindowLongPtr(hWnd,GWL_WNDPROC ,(LPARAM)EditProc);
		hWnd = GetNextWindow(hWnd,GW_HWNDNEXT);
	}

	if(!pData->hEdit)return NULL;


	pData->cEditCb = 0;

	if(pData->uEditMode>=1)									// ComboBox leeren
	{
		SendMessage(pData->hEdit,CB_RESETCONTENT,0,0);
	}



	if(uSub==0)												// Haupteintrag bearbeiten
	{
		TreeListGetItemRect(pData,uItem,TVIR_GETCOLUMN|TVIR_TEXT,&sRect);

		pText	= pEntry->pText;
		uSize	= pEntry->uTextSize;
		iPixels	= pEntry->iTextPixels+10;
		lParam	= pEntry->lParam;
		uState	= pEntry->uState;
		hFont	=(uState&TVIS_BOLD)? pData->hFontB:pData->hFontN;

		if(pEntry->bCallback&TVIF_TEXT)
		{
			CallbackEntry(pData,pEntry,uItem,TVIF_TEXT,iTemp,uSize,pText);
			hDc=GetDC    (pData->hWnd);
			SelectObject (hDc,hFont);
			DrawText     (hDc,pText,uSize,&sRect,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_CALCRECT);
			ReleaseDC    (pData->hWnd,hDc);
			iPixels  = sRect.right-sRect.left+10;
			pData->cEditCb=1;
		}

		if(uBits&4)											// Volle Spaltenbreite
		{
			if(pEntry->iImage!=TV_NOIMAGE)
			{
				sRect.right  = pData->iColumnXpos[1];
				sRect.right -= pData->uScrollX;
			}

			iPixels	= sRect.right-sRect.left-2;
		}
		else{
			if(pText)iPixels += str_len(pText);
		}
	}
	else{													// Extraeintrag bearbeiten
		if(uBits&4)
		{
			TreeListGetItemRect(pData,uItem,TVIR_GETCOLUMN|TVIR_COLTOSUB(uSub),&sRect);

			if(pData->bColumnEdit[uSub]>=TVAX_CHECK)
			{
				sRect.left += pData->iChecksXsize;
				if(sRect.right<sRect.left)sRect.right=sRect.left+1;
			}
		}
		else{
			TreeListGetItemRect(pData,uItem,TVIR_GETCOLUMN|TVIR_TEXT|TVIR_COLTOSUB(uSub),&sRect);
		}

		pExtra = pData->pExtraItems[uSub-1][uItem];
		if(!pExtra)
		{
			pData->cTempText1[0] = 0;
			pText	= pData->cTempText1;
			uSize	= sizeof(pData->cTempText1)/sizeof(pData->cTempText1[0]);
			iPixels	= sRect.right-sRect.left+10;
			hFont	= pData->hFontN;
			uState  = pEntry->uState&TVIS_BASEFLAGS;
			lParam	= 0;
		}
		else{
			pText	= pExtra->pText;
			uSize	= pExtra->uTextSize;
			iPixels	= pExtra->iTextPixels+10;
			lParam	= pEntry->lParam;
			uState	= pExtra->uState;
			uState |= pEntry->uState&TVIS_BASEFLAGS;
			hFont	=(uState&TVIS_BOLD)? pData->hFontB:pData->hFontN;


			if(pExtra->bCallback&TVIF_TEXT)
			{
				CallbackExtra(pData,pEntry,pExtra,uItem,uSub,TVIF_TEXT,iTemp,uSize,pText);
				hDc=GetDC    (pData->hWnd);
				SelectObject (hDc,hFont);
				DrawText     (hDc,pText,uSize,&sRect,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_CALCRECT);
				ReleaseDC    (pData->hWnd,hDc);
				iPixels=sRect.right-sRect.left;
				pData->cEditCb=1;
			}
		}

		if(uBits&4)											// Volle Spaltenbreite
		{
			if(pExtra && pExtra->iImage!=TV_NOIMAGE)
			{
				sRect.left += pData->iImagesXsize+1;
			}

			iPixels	= sRect.right-sRect.left-2;
		}
		else{
			if(pText)iPixels += str_len(pText);

			switch(pData->cColumnAlign[uSub])
			{
			case DT_RIGHT:	iWidth		= sRect.right-sRect.left;
				sRect.left += iWidth-iPixels;
				break;

			case DT_CENTER:	iWidth		= sRect.right-sRect.left;
				sRect.left += (iWidth-iPixels)/2;
				break;
			}
		}
	}


	UNLOCK(pData);

	sNotify.hdr.code			= TVN_BEGINLABELEDIT;
	sNotify.item.mask			= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_TEXT|TVIF_SUBITEM;
	sNotify.item.hItem			= (HTREEITEM)uItem;
	sNotify.item.pszText		= (LPTSTR)pText;
	sNotify.item.lParam			= lParam;
	sNotify.item.state			= uState;
	sNotify.item.cchTextMax		= uSize;
	sNotify.item.stateMask		= 0xFFFFFFFF;
	sNotify.item.cChildren		= uSub;

	lRet=SendNotify(pData,&sNotify.hdr);

	LOCK(pData);

	if(lRet)												// Das Editieren abbrechen
	{
		TreeListEndLabelEdit(pData,0);
		return NULL;
	}


	pEntry=pData->pTreeItems[uItem];
	if(!pEntry)return NULL;


	if(iPixels<32)iPixels=32;

	UNLOCK(pData);
	SetFocus(pData->hEdit);
	LOCK(pData);

	if(pData->uEditMode)
	{
		sRect.top--;
		uHeight	= 260;
	}
	else{
		uHeight=pData->iFontHeight+4;
	}

	SetWindowText(pData->hEdit,pText);
	SendMessage  (pData->hEdit,WM_SETFONT,(WPARAM)hFont,0);

	switch(pData->uEditMode)
	{
	case 0:	SendMessage(pData->hEdit,EM_SETSEL,0,uSize);						break;
	case 1:	SendMessage(pData->hEdit,CB_SETEDITSEL,MAKELPARAM(0,255),uSize);	break;
	}

	SetWindowPos (pData->hEdit,HWND_TOP,sRect.left+2,sRect.top+1,iPixels,uHeight,SWP_SHOWWINDOW);
	RedrawWindow (pData->hEdit,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE);

	pData->uEditItem = uItem;
	pData->uEditSub	 = uSub;



	return pData->hEdit;
}

//*****************************************************************************
//*
//*		TreeListStartAutoEdit
//*
//*****************************************************************************
//	Startet die Autoeditierung f? eine Spalte
//	pData		: Zeiger auf die Fensterdaten
//	uColumn		: Ist die Nummer der Spalte
//	wParam		: Ist der W-Parameter des Tastendrucks
//					VK_ICONCLK	bei einem Clickauf das Iion
//					VK_DBLCLK	bei einem Doppelclick
//	lParam		: Ist der L-Parameter des Tastendrucks (bzw. die Koordinaten)
//	Ergibt 1 das Editieren gestartet wurde, ansonsten 0
//				0 wenn der Eintrag nicht gew?lt wurde
static int TreeListStartAutoEdit(TreeListData *pData,unsigned uColumn,WPARAM wParam,LPARAM lParam)
{
	TCHAR		cBuffer[256];
	NMTREEVIEW	sNotify;
	ExtraItem  *pExtra;
	BaseItem   *pEntry;
	LPTSTR	   *pList;
	LPTSTR	    pText;
	TV_ITEM		sItem;
	TCHAR		cChar;
	unsigned	uMode;
	unsigned	uBits;
	unsigned	uFlag;
	unsigned	uMax;
	unsigned	uPos;
	unsigned	uLen;
	unsigned	uSub;
	HWND		hWnd;
	int			iSel;
	int			iRet;
	int			iIcon;


	if(  pData->uEditItem			  )return 0;
	if(  pData->uColumnCount<=uColumn )return 0;
	if(!(pData->uStyle&TVS_EDITLABELS))return 0;


	uBits = pData->bColumnFlags[uColumn];
	uMode = pData->bColumnEdit [uColumn];

	if(uMode==0)return 0;


	if(uBits&TVAE_STATEENABLE)									// Kann das Editieren gesperrt werden 
	{
		if(uColumn)
		{
			pExtra  =  pData->pExtraItems[uColumn-1][pData->uSelectedItem];
			if( pExtra && (pExtra->uState&TVIS_DISABLEBIT))return 0;
		}
		else{
			pEntry  =  pData->pTreeItems[pData->uSelectedItem];
			if(!pEntry || (pEntry->uState&TVIS_DISABLEBIT))return 0;
		}
	}


	if(wParam!=VK_RETURN && wParam!=VK_DBLCLK && wParam!=VK_ICONCLK)
	{
		if(uMode>=TVAX_CBLIST      )
			if(uMode!=TVAX_STEPED      )
				if(uMode!=TVAX_CHECKED     )return 0;
		if(uBits &TVAE_ONLYRETURN  )return 0;
		if(wParam<'0' || wParam>'Z')return 0;
	}



	uMax=pData->bColumnCbSize[uColumn];
	if(!uMax)uMax=1024;


	//******************** Weiterschalten mit Return ******************************


	if(uMode>=TVAX_STEP)
	{
		sItem.mask		 = TVIF_TEXT|TVIF_SUBITEM|TVIF_TEXTPTR|((uMode<TVAX_CHECK)? 0:TVIF_STATE|TVIF_PARAM);
		sItem.stateMask	 = 0xFFFFFFFF;
		sItem.hItem		 = (HTREEITEM)pData->uSelectedItem;
		sItem.cChildren	 = uColumn;

		if(!TreeListGetItem(pData,&sItem))return 0;

		if((uMode&1) && wParam!=VK_ICONCLK)						// Ein Edit-Feld ?fnen
		{
			uMode = TVAX_EDIT;
			goto EditField;
		}

		if(uMode>=TVAX_CHECK)									// State-Bits bei Checkboxen setzen								
		{
			if(uBits&TVAE_STATEENABLE)
				uFlag = sItem.state&(~TVIS_STATEIMAGEMASK|TVIS_DISABLEBIT);
			else	uFlag = sItem.state&(~TVIS_STATEIMAGEMASK);

			if(pData->uStyleEx&TVS_EX_BITCHECKBOX)
				sItem.state	= (sItem.state^0x1000);
			else	sItem.state	= (sItem.state&0x1000)?	0x2000:0x1000;

			sItem.state |= uFlag;
		}	 
		else{
			sItem.mask  &= ~TVIF_STATE;
		}

		if(uBits&TVAE_PTRLIST)									// Zeigerliste  char *pTexte[]={"1","2",NULL};
		{
			pList = (LPTSTR*)pData->pColumnCbData[uColumn];

			if(!pList)
			{	
				if(uMode<TVAX_CHECK)return 0;
				sItem.mask &= ~TVIF_TEXT;
				pText       =  sItem.pszText;
				uPos		=  0;
				goto NoTextChange;
			}

			for(uPos=0;;uPos++)
			{
				if(uPos>=uMax || !pList[uPos]){uPos=0;break;}
				if(str_cmp(sItem.pszText,pList[uPos]))continue;
				uPos++;
				if(uPos>=uMax || !pList[uPos])uPos=0;
				break;
			}

			pText = pList[uPos];
			if(!pText)
			{
				if(uMode<TVAX_CHECK)return 0;
				sItem.mask &= ~TVIF_TEXT;
				pText       =  sItem.pszText;
			}
		}
		else{													// Textliste  char *pText="1|2|3";
			pText = (LPTSTR)pData->pColumnCbData[uColumn];
			cChar = (TCHAR )pData->bColumnCbChar[uColumn];
			sItem.cchTextMax--;

			if(!pText)
			{
				if(uMode<TVAX_CHECK)return 0;
				sItem.mask &= ~TVIF_TEXT;
				pText       =  sItem.pszText;
				uPos		=  0;
				goto NoTextChange;
			}

			for(uPos=0;;uPos++)
			{
				if(uPos>=uMax)
				{
					pText = (LPTSTR)pData->pColumnCbData[uColumn];
					uPos  = 0;
					break;
				}

				for(uLen=0;pText[uLen];uLen++)					// Subtextl?ge bestimmen
				{
					if(pText[uLen]==cChar)break;
				}

				if(sItem.cchTextMax==(int)uLen)					// Ist der Subtext der aktulle Text
					if(str_ncmp(sItem.pszText,pText,uLen)==0)
					{
						uPos++;

						if(uPos>=uMax)
						{
							pText = (LPTSTR)pData->pColumnCbData[uColumn];
							uPos  = 0;
							break;
						}

						pText+=uLen;
						if(pText[0]==cChar)pText++;
						if(pText[0]==0)
						{
							pText = (LPTSTR)pData->pColumnCbData[uColumn];
							uPos  = 0;
						}

						break;
					}

					pText+=uLen;
					if(pText[0]==cChar)pText++;
					if(pText[0]==0)
					{
						pText = (LPTSTR)pData->pColumnCbData[uColumn];
						uPos  = 0;
						break;
					}

			}

			for(uLen=0;pText[uLen] && uLen<256;uLen++)			// Ausgew?lten Text kopiren 
			{
				if(pText[uLen]==cChar)break;
				cBuffer[uLen] = pText[uLen];
			}

			cBuffer[uLen] = 0;
			pText = cBuffer;
		}


NoTextChange:

		sItem.mask		&= ~TVIF_TEXTPTR;
		sItem.stateMask	&=  TVIS_STATEIMAGEMASK;
		sItem.hItem		 =  (HTREEITEM)pData->uSelectedItem;
		sItem.pszText	 =  pText;
		sItem.cchTextMax =  256;
		sItem.cChildren	 =  uColumn;
		iIcon			 =  pData->iColumnCbIcon[uColumn];

		if(iIcon>=0)											// Auch ein Icon zuweisen
		{
			sItem.mask	|= TVIF_IMAGE|TVIF_SELECTEDIMAGE;
			sItem.iImage			= iIcon+uPos;
			sItem.iSelectedImage	= sItem.iImage;
		}

		iRet = TreeListSetItem(pData,&sItem);

		sNotify.hdr.code			= (uMode<TVAX_CHECK)? TVN_STEPSTATECHANGED:TVN_CBSTATECHANGED;		
		sNotify.action				= 0;
		sNotify.itemNew.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_PARAM|TVIF_TEXT|TVIF_SUBITEM;
		sNotify.itemNew.stateMask	= 0xFFFFFFFF;
		sNotify.itemNew.hItem		= sItem.hItem;
		sNotify.itemNew.state		= sItem.state;
		sNotify.itemNew.lParam		= sItem.lParam;
		sNotify.itemNew.pszText		= sItem.pszText;
		sNotify.itemNew.cchTextMax	= sItem.cchTextMax;
		sNotify.itemNew.cChildren	= uColumn;
		sNotify.itemOld.mask		= 0;
		sNotify.ptDrag.x			= LOWORD(lParam);
		sNotify.ptDrag.y			= HIWORD(lParam);

		UNLOCK(pData);
		SendNotify(pData,&sNotify.hdr);
		LOCK(pData);

		return iRet;
	}


	//******************** Edit oder Combobox *************************************

EditField:

	uSub	 = (uMode-1)<<29;
	uSub    |=  uColumn;

	if(uBits&TVAE_FULLWIDTH)
	{
		uSub |= 0x80000000;
	}

	hWnd = TreeListEditLabel(pData,pData->uSelectedItem,uSub);
	if(!hWnd)return 0;


	if(uMode!=TVAX_EDIT)
	{
		iSel			 = (uMode!=TVAX_CBLIST)? -1:0;
		sItem.mask		 = TVIF_TEXT|TVIF_SUBITEM|TVIF_TEXTPTR;
		sItem.hItem		 = (HTREEITEM)pData->uSelectedItem;
		sItem.cChildren	 = uColumn;

		if(!TreeListGetItem(pData,&sItem))return 0;


		if(uBits&TVAE_PTRLIST)									// Zeigerliste  char *pTexte[]={"1","2",NULL};
		{
			pList = (LPTSTR*)pData->pColumnCbData[uColumn];

			for(uPos=0;uPos<uMax;uPos++)
			{
				if(!pList[uPos])break;
				if(!str_cmp(sItem.pszText,pList[uPos]))iSel=uPos;
				SendMessage(hWnd,CB_ADDSTRING,0,(LPARAM)pList[uPos]);
			}
		}
		else{													// Textliste  char *pText="1|2|3";
			pText = (LPTSTR)pData->pColumnCbData[uColumn];
			cChar = (TCHAR )pData->bColumnCbChar[uColumn];
			sItem.cchTextMax--;

			for(uPos=0;uPos<uMax;uPos++)
			{
				for(uLen=0;pText[uLen];uLen++)
				{
					if(pText[uLen]==cChar)break;
				}

				if(sItem.cchTextMax==(int)uLen)
					if(str_ncmp(sItem.pszText,pText,uLen)==0)
					{
						iSel = uPos;
					}

					if(cChar)
					{
						if(uLen<256){memcpy(cBuffer,pText,sizeof(TCHAR)*uLen);cBuffer[uLen]=0;}
						else		{memcpy(cBuffer,pText,sizeof(TCHAR)*255 );cBuffer[255 ]=0;}
						SendMessage(hWnd,CB_ADDSTRING,0,(LPARAM)cBuffer);
					}
					else{
						SendMessage(hWnd,CB_ADDSTRING,0,(LPARAM)pText);
					}

					pText+=uLen;
					if(pText[0]==cChar)pText++;
					if(pText[0]==0)break;
			}
		}

		if(iSel>=0)
		{
			SendMessage(hWnd,CB_SETCURSEL,iSel,0);
		}
	}



	if(uMode<TVAX_CBLIST && wParam!=VK_RETURN)					// Ersten Buchstaben ans Fenster ?ergeben
	{
		MSG		sMsg;

		sMsg.hwnd	 = hWnd;
		sMsg.message = WM_KEYDOWN;
		sMsg.wParam	 = wParam;
		sMsg.lParam	 = lParam;
		sMsg.time	 = 0;
		sMsg.pt.x	 = 0;
		sMsg.pt.y	 = 0;
		TranslateMessage(&sMsg);
	}




	return 1;
}

//*****************************************************************************
//*
//*		TreeListProc
//*
//*****************************************************************************
//	Ist die Fensterfunktion f? das TreeList Fenster
static LRESULT CALLBACK TreeListProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	TreeListData   *pData;
	MSG			   *pMsg;
	LPARAM			lRet;
	TV_HITTESTINFO	sInfo;
	unsigned		uChange;
	unsigned		uFlag;
	unsigned		uMode;
	unsigned		uVal;
	int				iPos;
	int				iMax;
	HDC				hDc;




	switch(uMsg)
	{
	case WM_CREATE:											// Das Fenster erzeugen

		pData= new TreeListData;
		if(!pData)return -1;

		memset(pData,0,sizeof(TreeListData));


		pData->pTreeItems = new BaseItem*[1];
		if(!pData->pTreeItems)
		{
			delete pData;
			return -1;
		}

		pData->pItemPos  = new unsigned[1];
		if(!pData->pItemPos)
		{
			delete pData->pTreeItems;
			delete pData;
			return -1;
		}

		GlobalInit();

		pData->pItemPos  [0] = 0;
		pData->pTreeItems[0] = NULL;

		SetWindowLongPtr(hWnd,0,(LONG_PTR)pData);
		pData->iIndent		 = DEFAULT_IDENT;
		pData->iShift		 = DEFAULT_SHIFT;
		pData->uStyle		 = GetWindowLong(hWnd,GWL_STYLE);
		pData->hSem			 = CreateSemaphore(0,1,0x70000000,0);
		pData->hWnd			 = hWnd;
		pData->cIsEnabled	 = (char)IsWindowEnabled(hWnd);

		if(!(pData->uStyle&(TVS_HASBUTTONS|TVS_HASLINES)))
		{
			pData->cHasRootRow = 0;
		}
		else{
			pData->cHasRootRow = (char)((pData->uStyle&TVS_LINESATROOT)? 1:0);
		}

		if(!(pData->uStyle&TVS_NOTOOLTIPS))
		{
			CrateToolTip(pData);
		}

		UpdateFont   (pData);
		UpdateHeight (pData);
		UpdateColors (pData);
		UpdateScrollY(pData);


		if(pData->uStyle&TVS_CHECKBOXES)
		{
			CreateStateImageList(pData,0);
		}


		return 0;

	case WM_DESTROY:										// Das Fenster zerst?en

		pData = GetHandle(hWnd);

		LOCK(pData);

		TreeListDeleteItem(pData,U(TVI_ROOT),0);
		if(pData->hEdit   )DestroyWindow    (pData->hEdit   );
		if(pData->hHeader )DestroyWindow    (pData->hHeader );
		if(pData->hToolTip)DestroyWindow    (pData->hToolTip);
		if(pData->hStates )ImageList_Destroy(pData->hStates );
		if(pData->hChecks )ImageList_Destroy(pData->hChecks );
		if(pData->hImages )ImageList_Destroy(pData->hImages );


		for(uVal=1;uVal<pData->uColumnCount;uVal++)
		{
			delete pData->pExtraItems[uVal-1];
		}

		delete pData->pTreeItems;
		delete pData->pItemPos;

		UNLOCK(pData);

		CloseHandle(pData->hSem);
		memset(pData,0,sizeof(TreeListData));
		delete pData;

		GlobalDeinit();


		return 0;

	case WM_SIZE:											// Die Fenstergröße wurde ver?dert

		pData = GetHandle(hWnd);
		uFlag = 0;

		LOCK(pData);

		uVal = LOWORD(lParam);
		if(uVal!=pData->uSizeX)
		{
			if(pData->uColumnCountVar)					// Spalten mit variabler Breite nach f?ren
			{
				unsigned 	uPos;
				HDITEM	 	sItem;
				RECT		sRect;
				int			iDelta;
				int			iSize;
				int			iRest;
				int			iDiff;
				int			iOld;
				int			iNum;
				int			iOff;
				int			iAdd;



				uFlag		= 0;
				sItem.mask	= HDI_WIDTH;
				iDelta		= uVal-pData->uSizeX;
				iNum		= pData->uColumnCountVar;
				iRest		= iDelta%iNum;
				iOff		= uVal  %iNum;
				iDelta     -= iRest;


				if(iRest<0)
				{
					iRest  += iNum;
					iDelta -= iNum;
				}


				for(uPos=0;uPos<pData->uColumnCount;uPos++)
				{
					if(!pData->cColumnVar[uPos])continue;

					iOld	=  pData->iColumnXpos[uPos+1];
					iOld   -=  pData->iColumnXpos[uPos  ];
					iDiff	=  iDelta/iNum;
					iSize	=  iOld+iDiff;
					iAdd	= (iOff<iRest)? 1:0;
					iSize  +=  iAdd;
					uFlag	=  1;
					iOff   +=  1;

					if(iOff>=(int)pData->uColumnCountVar)
					{
						iOff=0;
					}

					if(iOld<=16 && iSize<16)			// Darf die Spalte ge?dert werden
					{
						iDelta += iAdd;
						iNum--;
						continue;
					}

					if(iSize<16)						// Wurde die Spalte zu klein
					{
						iDiff   = 16-iOld;
						iSize   = 16;
						iDelta += iAdd;
					}

					sItem.cxy = iSize;
					if(uPos==0)sItem.cxy++;

					Header_SetItem(pData->hHeader,uPos,&sItem);

					iDelta -= iDiff;
					iNum   -= 1;
				}

				iNum = UpdateColumns(pData);
				GetClientRect(hWnd,&sRect);
				sRect.left  = iNum;
				sRect.left -= pData->uScrollX;
				sRect.top   = pData->uStartPixel;
				InvalidateRect(hWnd,&sRect,FALSE);
			}

			if(uVal>pData->uSizeX)						// Hat sich die Breite vergrößert
			{
				RECT	sRect;

				GetClientRect (hWnd,&sRect);
				sRect.right  = uVal;
				sRect.left   = pData->uSizeX;
				InvalidateRect(hWnd,&sRect,FALSE);
			}

			pData->uSizeX							  = uVal;
			pData->iColumnXpos[pData->uColumnCount+1] = uVal;

			MoveWindow(pData->hHeader,-(int)pData->uScrollX,0,pData->uSizeX+pData->uScrollX,pData->uStartPixel,FALSE);
			UpdateScrollX(pData);
		}

		uVal = HIWORD(lParam);
		if(uVal!=pData->uSizeY)
		{
			if(uVal>pData->uSizeY)						// Hat sich die H?e vergrößert
			{
				RECT	sRect;

				GetClientRect (hWnd,&sRect);
				sRect.bottom  = uVal;
				sRect.top     = pData->uSizeY;
				InvalidateRect(hWnd,&sRect,FALSE);
			}

			pData->uSizeY		 =  uVal;
			pData->uSizeYsub	 = (uVal<=pData->uStartPixel)? 0:uVal-pData->uStartPixel;
			pData->uMaxEnties	 = pData->uSizeY;
			pData->uMaxEnties	-= pData->uStartPixel;
			pData->uPageEnties	 = pData->uMaxEnties;
			pData->uMaxEnties	+= pData->iRowHeight-1;
			pData->uMaxEnties   /= pData->iRowHeight;
			pData->uPageEnties  /= pData->iRowHeight;

			UpdateScrollY(pData);
		}


		UNLOCK(pData);

		if(uFlag)
		{
			RedrawWindow(pData->hHeader,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE);
		}

		return 0;

	case WM_ENABLE:											// Das Fenster feischalten oder sperren

		pData = GetHandle(hWnd);
		if(!pData->cIsEnabled!=(char)wParam)
		{
			LOCK(pData);
			pData->cIsEnabled=(char)wParam;
			UpdateView(pData);
			if(!pData->cIsEnabled)UpdateToolTip(pData,0,0);
			UNLOCK(pData);
		}

		return 0;

	case WM_SETFOCUS:										// Das Fenster bekommt den Focus

		pData = GetHandle(hWnd);
		if(!pData->cHasFocus)
		{
			NMHDR	sNotify;

			LOCK(pData);
			pData->cHasFocus=1;

			if(pData->uSelectedCount<=1)
			{
				if(pData->uSelectedItem)
				{
					if(pData->uStyleEx&TVS_EX_FULLROWMARK)
						UpdateRow (pData,pData->uSelectedItem);
					else	UpdateRect(pData,pData->uSelectedItem,pData->uSelectedSub);
				}
			}
			else{
				UpdateView(pData);
			}

			UNLOCK(pData);

			sNotify.code = NM_SETFOCUS;
			SendNotify(pData,&sNotify);
		}

		return 0;

	case WM_KILLFOCUS:										// Das Fenster bekommt den Focus

		pData = GetHandle(hWnd);
		if(pData->cHasFocus)
		{
			NMHDR	sNotify;

			LOCK(pData);
			pData->cHasFocus=0;

			if(pData->uSelectedCount<=1)
			{
				if(pData->uSelectedItem)
				{
					if(pData->uStyleEx&TVS_EX_FULLROWMARK)
						UpdateRow (pData,pData->uSelectedItem);
					else	UpdateRect(pData,pData->uSelectedItem,pData->uSelectedSub);
				}
			}
			else{
				UpdateView(pData);
			}

			UNLOCK(pData);

			sNotify.code = NM_KILLFOCUS;
			SendNotify(pData,&sNotify);
		}

		return 0;

	case WM_MOUSEMOVE:										// Gab es eine Mausbewegung

		pData = GetHandle(hWnd);
		sInfo.hItem = 0;
		sInfo.flags = 0;

		if(!(pData->uStyle&TVS_NOTOOLTIPS))
		{
			if(!pData->cIsEnabled)						// Ist das Fenster freigegeben
			{
				return 0;
			}

			LOCK(pData);

			sInfo.pt.x	= LOWORD(lParam);
			sInfo.pt.y	= HIWORD(lParam);
			TreeListHitTest(pData,&sInfo);
			UpdateToolTip(pData,U(sInfo.hItem),sInfo.flags);
			UNLOCK(pData);
		}

		if(pData->uStyle&TVS_TRACKSELECT)
		{
			if(!pData->cIsEnabled)						// Ist das Fenster freigegeben
			{
				return 0;
			}

			LOCK(pData);

			if(!sInfo.hItem)
			{
				sInfo.pt.x	= LOWORD(lParam);
				sInfo.pt.y	= HIWORD(lParam);
				TreeListHitTest(pData,&sInfo);
			}

			if(sInfo.hItem && (sInfo.flags&(TVHT_ONSUBITEM|TVHT_ONSUBLABEL|TVHT_ONITEM)))
			{
				TreeListSetTrackItem(pData,U(sInfo.hItem),TVHT_SUBTOCOL(sInfo.flags));
			}

			UNLOCK(pData);
		}

		if(wParam&pData->uDragFlags)					// Drag beginnen
		{
			NMTREEVIEW	sNotify;
			BaseItem   *pEntry;


			LOCK(pData);


			if(pData->uDragItem>pData->uTreeItemsMax)
			{
				UNLOCK(pData);
				return 0;
			}

			pEntry = pData->pTreeItems[pData->uDragItem];
			if(!pEntry)
			{
				UNLOCK(pData);
				return 0;
			}

			sNotify.hdr.code			= (pData->uDragFlags&MK_LBUTTON)? TVN_BEGINDRAG:TVN_BEGINRDRAG;
			sNotify.action				= (pEntry->uState&TVIS_EXPANDED)? TVE_COLLAPSE:TVE_EXPAND;
			sNotify.itemNew.mask		= TVIF_HANDLE|TVIF_PARAM|TVIF_STATE|TVIF_SUBNUMBER;
			sNotify.itemNew.hItem		= (HTREEITEM)pData->uDragItem;
			sNotify.itemNew.stateMask	= 0xFFFFFFFF;
			sNotify.itemNew.state		= pEntry->uState;
			sNotify.itemNew.lParam		= pEntry->lParam;
			sNotify.itemNew.cChildren	= pData->uDragSub;
			sNotify.itemNew.pszText		= (LPTSTR)-1;
			sNotify.itemNew.cchTextMax	= -1;
			sNotify.itemOld.mask		= 0;
			sNotify.ptDrag.x			= LOWORD(lParam);
			sNotify.ptDrag.y			= HIWORD(lParam);
			pData->uDragFlags			= 0;
			pData->cClickFlag			= 0;


			UNLOCK(pData);

			SendNotify(pData,&sNotify.hdr);
		}

		return 0;

	case WM_LBUTTONUP:										// Linken Mausklick aufheben

		pData = GetHandle(hWnd);
		pData->uDragFlags &= ~MK_LBUTTON;

		if(pData->cClickFlag)
		{
			TreeListMouseClick(pData,uMsg,wParam,lParam);
			pData->cClickFlag = 0;
		}

		return 0;


	case WM_RBUTTONUP:										// Linken Mausklick aufheben

		pData = GetHandle(hWnd);
		pData->uDragFlags &= ~MK_RBUTTON;
		return 0;

	case WM_LBUTTONDOWN:									// Mausklick
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:

		pData	= GetHandle(hWnd);

		LOCK(pData);
		TreeListMouseClick(pData,uMsg,wParam,lParam);

		return 0;

	case WM_KEYDOWN:										// Tastendruck

		pData	= GetHandle(hWnd);
		LOCK(pData);
		TreeListKeyDown(pData,wParam,lParam);

		return 0;

	case WM_KEYUP:											// Tastendruck

		DefWindowProc(hWnd,uMsg,wParam,lParam);
		return 1;

	case WM_MOUSEWHEEL:										// Scrollen mit dem Mausrad

		if(!(LOWORD(wParam)&(MK_CONTROL|MK_LBUTTON|MK_MBUTTON|MK_RBUTTON|MK_SHIFT)))
		{
			pData = GetHandle(hWnd);

			if(pData->uEditItem)return 0;



			iPos  = pData->uScrollY;
			iPos -= (short)HIWORD(wParam)/(WHEEL_DELTA/2);
			iMax  = pData->uItemPosCount;
			iMax -=	pData->uPageEnties-1;

			if(iPos>=iMax)iPos=iMax;
			if(iPos<0)iPos=0;
			if(iPos!=(int)pData->uScrollY)
			{
				pData->uScrollY = iPos;
				SetScrollPos(hWnd,SB_VERT,iPos,TRUE);
				UpdateView(pData);
			}

			return 1;
		}

		return 0;


	case WM_HSCROLL:										// Vertikalles scrollen

		pData	= GetHandle(hWnd);
		iPos	= pData->uScrollX;

		if(pData->uEditItem)return 0;

		switch(LOWORD(wParam))
		{
		case SB_LINEDOWN:		iPos+=16;				break;
		case SB_LINEUP:			iPos-=16;				break;
		case SB_PAGEDOWN:		iPos+=pData->uSizeX;	break;
		case SB_PAGEUP:			iPos-=pData->uSizeX;	break;
		case SB_THUMBPOSITION:	iPos =HIWORD(wParam);  	break;
		case SB_THUMBTRACK:		iPos =HIWORD(wParam);  	break;
		}

		uVal = pData->uColumnCount;
		if(uVal)iMax  = pData->iColumnXpos[uVal]-pData->uSizeX/2;
		else	iMax  = pData->iMaxSizeX;
		iMax -=	pData->uSizeX/2;

		if(iPos>=iMax)iPos=iMax;
		if(iPos<0)iPos=0;
		if(iPos!=(int)pData->uScrollX)
		{
			pData->uScrollX = iPos;
			SetScrollPos(hWnd,SB_HORZ,iPos,TRUE);
			UpdateView(pData);

			if(pData->hHeader)
			{
				MoveWindow(pData->hHeader,-iPos,0,pData->uSizeX+iPos,pData->uStartPixel,TRUE);
			}
		}

		return 0;

	case WM_VSCROLL:										// Vertikalles scrollen

		pData	= GetHandle(hWnd);
		iPos	= pData->uScrollY;

		if(pData->uEditItem)return 0;

		switch(LOWORD(wParam))
		{
		case SB_LINEDOWN:		iPos++;													break;
		case SB_LINEUP:			iPos--;													break;
		case SB_PAGEDOWN:		iPos+=(pData->uPageEnties>1)? pData->uPageEnties-1:1;	break;
		case SB_PAGEUP:			iPos-=(pData->uPageEnties>1)? pData->uPageEnties-1:1;	break;
		case SB_THUMBPOSITION:	iPos =HIWORD(wParam);  									break;
		case SB_THUMBTRACK:		iPos =HIWORD(wParam);  									break;
		}

		iMax  = pData->uItemPosCount;
		iMax -=	pData->uPageEnties-1;

		if(iPos>=iMax)iPos=iMax;
		if(iPos<0)iPos=0;
		if(iPos!=(int)pData->uScrollY)
		{
			pData->uScrollY = iPos;
			SetScrollPos(hWnd,SB_VERT,iPos,TRUE);
			UpdateView(pData);
		}

		return 0;

	case WM_SYSCOLORCHANGE:									// Wurden die Systemfarben ver?dert

		pData	= GetHandle(hWnd);

		LOCK(pData);
		if(pData->hHeader)SendMessage(pData->hHeader,WM_SYSCOLORCHANGE,0,0);
		UpdateColors(pData);
		UNLOCK(pData);

		return 0;


	case WM_SETFONT:										// Einen neuen Font zuweisen

		pData	= GetHandle(hWnd);

		LOCK(pData);

		pData->hFontN=(HFONT)wParam;
		if(UpdateFont(pData))UpdateView(pData);

		UNLOCK(pData);

		return 0;

	case WM_STYLECHANGED:									// Hat sich der Fenstersytle ge?dert

		if(wParam==GWL_STYLE)
		{
			pData	= GetHandle(hWnd);
			lParam	= ((STYLESTRUCT*)lParam)->styleNew;

			LOCK(pData);
			uChange		   = (unsigned)lParam^pData->uStyle;
			pData->uStyle  = (unsigned)lParam;

			if(uChange&(TVS_CHECKBOXES|TVS_NONEVENHEIGHT))
			{
				if(lParam&TVS_CHECKBOXES)
				{
					if(pData->hStates==NULL)
					{
						CreateStateImageList(pData,0);
					}
				}
				else{
					if(pData->iStatesMode && pData->hStates)
					{
						ImageList_Destroy(pData->hStates);
						pData->hStates     = NULL;
						pData->iStatesMode = 0;
					}
				}

				UpdateHeight (pData);
				UpdateScrollY(pData);
			}

			if(uChange&(TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT|TVS_RTLREADING|TVS_CHECKBOXES|TVS_TRACKSELECT))
			{
				if(!(pData->uStyle&(TVS_HASBUTTONS|TVS_HASLINES)))
				{
					pData->cHasRootRow = 0;
				}
				else{
					pData->cHasRootRow = (char)((pData->uStyle&TVS_LINESATROOT)? 1:0);
				}

				UpdateView(pData);
			}

			if(uChange&TVS_NOSCROLL)
			{
				UpdateScrollX(pData);
				UpdateScrollY(pData);
			}

			if(uChange&TVS_NOTOOLTIPS)
			{
				if(pData->uStyle&TVS_NOTOOLTIPS)
				{
					CrateToolTip(pData);
				}
				else{
					UpdateToolTip(pData,0,0);
				}
			}

			UNLOCK(pData);
		}

		return 0;

	case WM_PAINT:											// Das Fenster zeichnen

		if(wParam)
		{
			TreeListDraw(hWnd,(HDC)wParam,NULL);
		}
		else{
			PAINTSTRUCT sPaint;

			hDc=BeginPaint  (hWnd,&sPaint);
			TreeListDraw(hWnd, hDc,&sPaint.rcPaint);
			EndPaint    (hWnd,&sPaint);
		}

		return 0;

	case WM_TIMER:											// Timer Funktion

		if(wParam==ID_TOOLTIPCHECK)
		{
			pData	= GetHandle(hWnd);
			if(pData->uToolTipItem)
			{
				if(pData->uToolTipShow)					// Verz?ertes einblenden
				{
					pData->uToolTipShow--;
					if(pData->uToolTipShow)return 0;

					LOCK(pData);

					GetCursorPos(&sInfo.pt);
					ScreenToClient(hWnd,&sInfo.pt);
					TreeListHitTest(pData,&sInfo);


					if(sInfo.flags&TVHT_ONITEM)
						if(pData->uToolTipItem==U(sInfo.hItem) && pData->uToolTipSub==0)
						{
							TOOLINFO sInfo;

							UNLOCK(pData);

							sInfo.cbSize	= sizeof(sInfo);
							sInfo.hwnd		= pData->hWnd;
							sInfo.uId		= (UINT_PTR)pData->hWnd;

							if(pData->uToolTipItem)
							{
								SendMessage(pData->hToolTip,TTM_TRACKACTIVATE,0,(LPARAM)&sInfo);
								pData->uToolTipItem = 0;
							}

							SendMessage(pData->hToolTip,TTM_TRACKPOSITION,0,MAKELONG(pData->sToolTipPos.x,pData->sToolTipPos.y));
							SendMessage(pData->hToolTip,TTM_TRACKACTIVATE,1,(LPARAM)&sInfo);

							return 0;
						}

						if(sInfo.flags&(TVHT_ONSUBICON|TVHT_ONSUBLABEL))
							if(pData->uToolTipItem==U(sInfo.hItem) && TVHT_SUBTOCOL(sInfo.flags)==pData->uToolTipSub)
							{
								TOOLINFO sInfo;

								UNLOCK(pData);

								sInfo.cbSize	= sizeof(sInfo);
								sInfo.hwnd		= pData->hWnd;
								sInfo.uId		= (UINT_PTR)pData->hWnd;

								if(pData->uToolTipItem)
								{
									SendMessage(pData->hToolTip,TTM_TRACKACTIVATE,0,(LPARAM)&sInfo);
									pData->uToolTipItem = 0;
								}

								SendMessage(pData->hToolTip,TTM_TRACKPOSITION,0,MAKELONG(pData->sToolTipPos.x,pData->sToolTipPos.y));
								SendMessage(pData->hToolTip,TTM_TRACKACTIVATE,1,(LPARAM)&sInfo);

								return 0;
							}

							// ToolTip wieder entferenen
							UpdateToolTip(pData,U(sInfo.hItem),sInfo.flags);
							UNLOCK(pData);
				}

				LOCK(pData);

				GetCursorPos(&sInfo.pt);
				ScreenToClient(hWnd,&sInfo.pt);
				TreeListHitTest(pData,&sInfo);
				UpdateToolTip(pData,U(sInfo.hItem),sInfo.flags);

				UNLOCK(pData);
			}
		}

		return 0;

	case WM_COMMAND:										// Kommando Nachrichnten

		if(wParam==MAKELONG(3,EN_KILLFOCUS) || wParam==MAKELONG(3,EN_RETURN) || wParam==MAKELONG(3,CBN_KILLFOCUS))
		{
			pData = GetHandle(hWnd);
			if(pData->uEditItem)
			{
				LOCK(pData);
				TreeListEndLabelEdit(pData,(wParam==MAKELONG(3,EN_RETURN))? 2:1);
				UNLOCK(pData);
				if(pData->uEditMode)
					if(wParam!=MAKELONG(3,EN_KILLFOCUS))
					{
						SetFocus(pData->hWnd);
					}
			}
		}

		if(wParam==MAKELONG(3,EN_ESCAPE))
		{
			pData = GetHandle(hWnd);
			if(pData->uEditItem)
			{
				LOCK(pData);
				TreeListEndLabelEdit(pData,0);
				UNLOCK(pData);
				if(pData->uEditMode)SetFocus(pData->hWnd);
			}
		}

		return 0;

	case WM_NOTIFY:											// Notify Nachrichnten

		if(wParam==1)
		{
			NMHEADER   *pHdr=(NMHEADER*)lParam;
			HDITEM		sItem;
			RECT		sRect;
			int			iSize;
			int			iCol;

			if(pHdr->hdr.code==HDN_DIVIDERDBLCLICK)		// Doppelcklick auf Spaltentrenner
			{
				int		iSize;

				pData	= GetHandle(hWnd);

				LOCK(pData);

				iSize=TreeListScanColumn(pData,pHdr->iItem);
				if(iSize)
				{
					sItem.cxy		 = iSize;
					pHdr->hdr.code	 = HDN_ENDTRACK;
					pHdr->pitem		 = &sItem;
				}

				UNLOCK(pData);
			}
			// Wurde die Spaltenbreite ver?dert
			if(pHdr->hdr.code==HDN_TRACK || pHdr->hdr.code==HDN_ENDTRACK)
			{
				pData	= GetHandle(hWnd);
				LOCK(pData);

				iCol		= pHdr->iItem;
				sItem.mask	= HDI_WIDTH;
				sItem.cxy	= pHdr->pitem->cxy;

				Header_SetItem(pData->hHeader,iCol,&sItem);

				iSize = UpdateColumns(pData);
				if(iSize<0x10000)
				{
					GetClientRect (hWnd,&sRect);
					sRect.left  = iSize;
					sRect.left -= pData->uScrollX;
					InvalidateRect(hWnd,&sRect,FALSE);
				}

				UpdateScrollX(pData);
				UNLOCK(pData);

				if(!pData->cHasFocus && pHdr->hdr.code==HDN_ENDTRACK)
				{
					SetFocus(pData->hWnd);
				}
			}

			if(pHdr->hdr.code==HDN_ITEMCLICK || pHdr->hdr.code==HDN_ITEMDBLCLICK)
			{
				pData	= GetHandle(hWnd);
				SendNotify(pData,&pHdr->hdr);
			}
		}
		else{
			NMHDR		   *pHdr = (NMHDR*)lParam;
			NMTTDISPINFO   *pInfo;

			switch(pHdr->code)
			{
			case TTN_GETDISPINFO:

				pData			= GetHandle(hWnd);
				pInfo			= (NMTTDISPINFO*)pHdr;
				pInfo->lpszText = pData->cToolTipText;
				SendMessage(pData->hToolTip,WM_SETFONT,(WPARAM)pData->hFontT,0);
				break;
			}
		}


		return 0;

	case WM_GETDLGCODE:										// Welche Tasten werden im Dialog benutzt

		pMsg=(MSG*)lParam;
		if(pMsg && pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN)
		{
			return DLGC_WANTALLKEYS;
		}

		return DLGC_WANTCHARS|DLGC_WANTARROWS;

	case TVM_SETIMAGELIST:									// Die Image-Liste einstellen

		pData	= GetHandle(hWnd);

		LOCK(pData);

		switch((int)wParam)
		{
		case TVSIL_NORMAL:	   lRet=(LPARAM)pData->hImages;
			if(lRet==lParam)break;

			pData->hImages=(HIMAGELIST)lParam;
			if(!pData->hImages)
			{
				pData->iImagesXsize=0;
				pData->iImagesYsize=0;
			}
			else{
				IMAGEINFO sInfo;
				ImageList_GetImageInfo(pData->hImages,0,&sInfo);
				pData->iImagesXsize=sInfo.rcImage.right -sInfo.rcImage.left;
				pData->iImagesYsize=sInfo.rcImage.bottom-sInfo.rcImage.top;
				if(pData->hHeader)SendMessage(pData->hHeader,HDM_SETIMAGELIST,0,(LPARAM)pData->hImages);
			}

			if(!pData->cFixedHeight)
			{
				pData->iRowHeight=1;
				UpdateHeight (pData);
				UpdateScrollY(pData);
			}
			else{
				UpdateView(pData);
			}

			break;

		case TVSIL_STATE:	   lRet=(LPARAM)pData->hStates;
			if(lRet==lParam)break;

			if(pData->iStatesMode)
			{
				ImageList_Destroy(pData->hStates);
				pData->iStatesMode = 0;
			}

			pData->hStates=(HIMAGELIST)lParam;
			if(!pData->hStates)
			{
				pData->iStatesXsize=0;
				pData->iStatesYsize=0;

				if(pData->uStyle&TVS_CHECKBOXES)
				{
					CreateStateImageList(pData,0);
				}
			}
			else{
				IMAGEINFO sInfo;
				ImageList_GetImageInfo(pData->hStates,0,&sInfo);
				pData->iStatesXsize=sInfo.rcImage.right -sInfo.rcImage.left;
				pData->iStatesYsize=sInfo.rcImage.bottom-sInfo.rcImage.top;
				pData->iStatesMode = 0;
			}

			if(!pData->cFixedHeight)
			{
				pData->iRowHeight=1;
				UpdateHeight (pData);
				UpdateScrollY(pData);
			}
			else{
				UpdateView(pData);
			}

			break;

		case TVSIL_CHECK:	   lRet=(LPARAM)pData->hChecks;
			if(lRet==lParam)break;

			if(pData->iChecksMode)
			{
				ImageList_Destroy(pData->hChecks);
				pData->iChecksMode = 0;
			}

			pData->hChecks=(HIMAGELIST)lParam;
			if(!pData->hChecks)
			{
				pData->iChecksXsize=0;
				pData->iChecksYsize=0;

				for(uVal=0;uVal<pData->uColumnCount;uVal++)
				{
					if(pData->bColumnEdit[uVal]<TVSIL_CHECK)continue;
					CreateStateImageList(pData,1);
					break;
				}
			}
			else{
				IMAGEINFO sInfo;
				ImageList_GetImageInfo(pData->hChecks,0,&sInfo);
				pData->iChecksXsize=sInfo.rcImage.right -sInfo.rcImage.left;
				pData->iChecksYsize=sInfo.rcImage.bottom-sInfo.rcImage.top;
				pData->iChecksMode = 0;
			}

			if(!pData->cFixedHeight)
			{
				pData->iRowHeight=1;
				UpdateHeight (pData);
				UpdateScrollY(pData);
			}
			else{
				UpdateView(pData);
			}

			break;

		default:			lRet=0;
		}


		UNLOCK(pData);

		return lRet;

	case TVM_GETIMAGELIST:									// Die Image-Liste abfragen

		pData	= GetHandle(hWnd);

		switch((int)wParam)
		{
		case TVSIL_NORMAL:	lRet=(LPARAM)pData->hImages; break;
		case TVSIL_STATE:	lRet=(LPARAM)pData->hImages; break;
		case TVSIL_CHECK:	lRet=(LPARAM)pData->hChecks; break;
		default:			lRet=0;
		}

		return lRet;

	case TVM_GETITEMSTATE:									// Die Statusbits eines Eintrags abfragen

		pData = GetHandle(hWnd);
		LOCK(pData);

		if((unsigned)wParam<=pData->uTreeItemsMax)
		{
			BaseItem *pEntry;

			pEntry = pData->pTreeItems[(unsigned)wParam];
			if(!pEntry)lRet = 0;
			{
				lRet = pEntry->uState;
			}
		}
		else{
			lRet = 0;
		}

		UNLOCK(pData);

		return lRet;

	case TVM_GETITEM:										// Eintr?e abfragen

		pData = GetHandle(hWnd);
		LOCK(pData);
		lRet  = TreeListGetItem(pData,(TV_ITEM*)lParam);
		UNLOCK(pData);

		return lRet;

	case TVM_SETITEM:										// Eintr?e einf?en

		pData = GetHandle(hWnd);
		LOCK(pData);
		lRet  = TreeListSetItem(pData,(TV_ITEM*)lParam);
		UNLOCK(pData);

		return lRet;

	case TVM_INSERTITEM:									// Eintr?e einf?en

		pData = GetHandle(hWnd);
		LOCK(pData);
		lRet  = TreeListInsertItem(pData,(TV_INSERTSTRUCT*)lParam);
		UNLOCK(pData);

		return lRet;

	case TVM_DELETEITEM:									// Eintr?e l?chen

		pData = GetHandle(hWnd);
		LOCK(pData);
		lRet  = TreeListDeleteItem(pData,(unsigned)lParam,1);
		UNLOCK(pData);

		return lRet;

	case TVM_DELETECOLUMN:									// L?chen einer Salte im Header

		pData = GetHandle(hWnd);
		LOCK(pData);
		lRet  = TreeListDeleteColumn(pData,(unsigned)wParam);
		UNLOCK(pData);

		return lRet;

	case TVM_INSERTCOLUMN:									// Einf?en einer Salte im Header

		pData = GetHandle(hWnd);
		LOCK(pData);
		lRet  = TreeListInsertColumn(pData,(int)wParam,(TV_COLUMN*)lParam);
		UNLOCK(pData);

		return lRet;

	case TVM_GETCOLUMNCOUNT:								// Abfragen der Spaltenanzahl

		pData = GetHandle(hWnd);
		return (LRESULT)pData->uColumnCount;

	case TVM_GETHEADER:										// Abfragen des Header-Fensters

		pData = GetHandle(hWnd);
		return (LRESULT)pData->hHeader;

	case TVM_GETEXTENDEDSTYLE:								// Abfragen der erweiterten Style-Bits

		pData = GetHandle(hWnd);
		return pData->uStyleEx;

	case TVM_SETEXTENDEDSTYLE:								// Einstellen der erweiterten Style-Bits

		pData = GetHandle(hWnd);
		if(!wParam)wParam=0xFFFFFFFF;

		uVal  = pData->uStyleEx &~(unsigned)wParam;
		uVal |=	(unsigned)lParam& (unsigned)wParam;

		if(pData->uStyleEx!=uVal)						// Gab es Änderungen
		{
			LOCK(pData);
			uChange = pData->uStyleEx^uVal;
			pData->uStyleEx = uVal;

			if(uChange&TVS_EX_AUTOHSCROLL)
			{
				UpdateScrollX(pData);
			}

			if(uChange&TVS_EX_ITEMLINES)
			{
				UpdateHeight (pData);
				UpdateScrollY(pData);
			}

			if(uChange&(TVS_EX_ALTERNATECOLOR|TVS_EX_SUBSELECT|TVS_EX_MULTISELECT|TVS_EX_FULLROWMARK))
			{
				UpdateView(pData);
			}

			if((uChange&~lParam)&TVS_EX_SUBSELECT)
			{
				TreeListSelectItem(pData,pData->uSelectedItem,0,TVC_UNKNOWN);
			}

			UNLOCK(pData);
		}

		return uVal;

	case TVM_GETINSERTMARKCOLOR:							// Farben abfragen
		wParam=TVC_INSERT;goto ColGet;
	case TVM_GETLINECOLOR:	wParam=TVC_LINE;  goto ColGet;
	case TVM_GETTEXTCOLOR:	wParam=TVC_TEXT;
	case TVM_GETBKCOLOR:

ColGet:	if(wParam<0 || wParam>=MAX_COLORS)return 0xFFFFFFFF;
		pData = GetHandle(hWnd);
		return pData->uColors[(unsigned)wParam];


	case TVM_SETINSERTMARKCOLOR:							// Farben einstellen
		wParam=TVC_INSERT;goto ColSet;
	case TVM_SETLINECOLOR:	wParam=TVC_LINE;  goto ColSet;
	case TVM_SETTEXTCOLOR:	wParam=TVC_TEXT;
	case TVM_SETBKCOLOR:

ColSet:	if(wParam<0 || wParam>=MAX_COLORS)return 0xFFFFFFFF;

		pData = GetHandle(hWnd);

		LOCK(pData);

		lRet=pData->uColors[(unsigned)wParam];

		if((COLORREF)lParam==TV_NOCOLOR)				// Standartfarbe einstellen
		{
			if(pData->cColorChanged[(unsigned)wParam])
			{
				pData->cColorChanged[(unsigned)wParam]=0;
				UpdateColors(pData);
				UpdateView  (pData);
			}
		}
		else{
			pData->cColorChanged[(unsigned)wParam]=1;
			pData->uColors      [(unsigned)wParam]=(COLORREF)lParam;

			if(lRet!=lParam)UpdateView(pData);
		}

		UNLOCK(pData);

		return lRet;

	case TVM_HITTEST:										// Abfragen von Koortinatenpositionen

		pData = GetHandle(hWnd);

		LOCK(pData);
		lRet=TreeListHitTest(pData,(LPTV_HITTESTINFO)lParam);
		UNLOCK(pData);

		return lRet;

	case TVM_SELECTSUBITEM:									// Einen (Sub)Eintrag ausw?len

		pData = GetHandle(hWnd);

		LOCK(pData);
		if(!(pData->uStyleEx&TVS_EX_SUBSELECT))wParam=0;
		lRet=TreeListSelectItem(pData,(unsigned)lParam,(unsigned)wParam,TVC_UNKNOWN);
		if(lRet>1)lRet=1;
		UNLOCK(pData);

		return lRet;

	case TVM_SELECTDROP:									// Den unterstrichenen Eintrags ausw?len

		pData = GetHandle(hWnd);

		LOCK(pData);
		lRet=TreeListSetTrackItem(pData,(unsigned)lParam,(unsigned)wParam);
		UNLOCK(pData);

		return lRet;

	case TVM_SELECTITEM:									// Einen Eintrag ausw?len

		pData = GetHandle(hWnd);

		LOCK(pData);

		switch(wParam)
		{
		case TVGN_CARET:		lRet=TreeListSelectItem(pData,(unsigned)lParam,0,TVC_UNKNOWN);
			if(lRet>1)lRet=1;
			break;
		case TVGN_DROPHILITE:	lRet=TreeListSetTrackItem(pData,(unsigned)lParam,0);
			break;

		case TVGN_FIRSTVISIBLE:	lRet=TreeListEnsureVisible(pData,(unsigned)lParam,FIRST_LINE);
			lRet=(lRet<0)? 0:1;
		default:				lRet=0;
		}

		UNLOCK(pData);

		return lRet;

	case TVM_GETCOUNT:										// Anzahl der Eintr?e abfragen

		pData = GetHandle(hWnd);
		return pData->uTreeItemsCount;

	case TVM_GETINDENT:										// Einr?kung abfragen

		pData = GetHandle(hWnd);
		return pData->iIndent;

	case TVM_SETINDENT:										// Einr?kung einstellen

		pData = GetHandle(hWnd);
		lRet  = pData->iIndent;
		if(wParam< 5)wParam= 5;
		if(wParam>64)wParam=64;
		if(lRet!=(LPARAM)wParam)
		{
			LOCK(pData);
			pData->iIndent = (int)wParam;
			UpdateView(pData);
			UNLOCK(pData);
		}

		return lRet;

	case TVM_GETITEMHEIGHT:									// Zeilenh?e abfragen

		pData = GetHandle(hWnd);
		return pData->iRowHeight;

	case TVM_SETITEMHEIGHT:									// Zeilenh?e abfragen

		pData = GetHandle(hWnd);
		lRet  = pData->iRowHeight;

		if(wParam==-1)
		{
			LOCK(pData);
			pData->cFixedHeight=0;
			UpdateHeight (pData);
			UpdateScrollY(pData);
			UNLOCK(pData);
			return lRet;
		}

		if(wParam&1 )
			if(!(pData->uStyleEx&TVS_NONEVENHEIGHT))wParam--;
		if(wParam<  1)wParam=  1;
		if(wParam>256)wParam=256;

		if(lRet!=(LPARAM)wParam)
		{
			LOCK(pData);
			pData->cFixedHeight = 1;
			pData->iRowHeight   = (int)wParam;
			UpdateView(pData);
			UNLOCK(pData);
		}

		return lRet;

	case TVM_GETVISIBLECOUNT:								// Abfragen der sichtbaren Zeilen

		pData = GetHandle(hWnd);
		return (pData->uSizeY-pData->uStartPixel)/pData->iRowHeight;

	case TVM_ENSUREVISIBLE:									// Einen Eintrag ins Sichtfenster legen

		pData = GetHandle(hWnd);
		LOCK(pData);
		lRet=TreeListEnsureVisible(pData,(unsigned)lParam,(int)wParam);
		UNLOCK(pData);

		return lRet;

	case TVM_GETNEXTITEM:									// Einen Eintrag suchen

		pData = GetHandle(hWnd);
		LOCK(pData);
		lRet=TreeListGetNextItem(pData,(unsigned)lParam,(int)wParam);
		UNLOCK(pData);

		return lRet;

	case TVM_GETITEMRECT:									// Das Rechteck eines Eintrages abfragen

		pData = GetHandle(hWnd);
		LOCK(pData);
		uVal=*(unsigned*)lParam;
		lRet=TreeListGetItemRect(pData,uVal,(unsigned)wParam,(RECT*)lParam);
		UNLOCK(pData);

		return lRet;

	case TVM_EXPAND:										// Einen Eintrag umklappen

		pData = GetHandle(hWnd);
		LOCK(pData);
		lRet=0;

		if((unsigned)lParam<=pData->uTreeItemsMax)
		{
			BaseItem *pEntry;
			POINT	  sPoint;

			pEntry = pData->pTreeItems[(unsigned)lParam];
			if(pEntry)
			{
				sPoint.x = 0;
				sPoint.y = 0;

				switch(wParam&0x0F)
				{
				case TVE_COLLAPSE:	if(pEntry->uState&TVIS_EXPANDED)
							{
								lRet=(TreeListToggleItem(pData,lParam,0))? 0:1;
							}
							else{
								if(pEntry->uState&TVIS_EXPANDPARTIAL)
								{
									pEntry->uState &= ~TVIS_EXPANDPARTIAL;
									UpdateRect(pData,(unsigned)lParam,0);
								}

								lRet=1;
							}

							if(wParam&TVE_COLLAPSERESET)
							{
								pEntry->uState &= TVIS_EXPANDEDONCE;

								while(pEntry->uLastChild)
								{
									if(!TreeListDeleteItem(pData,pEntry->uLastChild,1))break;
									pEntry = pData->pTreeItems[(unsigned)lParam];
								}
							}

							break;

				case TVE_EXPAND:	if(!(pEntry->uState&TVIS_EXPANDED))
							{
								uVal = (wParam&TVE_EXPANDPARTIAL)? TVIS_EXPANDPARTIAL:0;
								lRet = (TreeListToggleItem(pData,(unsigned)lParam,uVal))? 0:1;
							}
							else{
								if(wParam&TVE_EXPANDPARTIAL)
								{
									if(!(pEntry->uState&TVIS_EXPANDPARTIAL))
									{
										pEntry->uState |= TVIS_EXPANDPARTIAL;
										UpdateRect(pData,(unsigned)lParam,0);
									}
								}
								else{
									if(pEntry->uState&TVIS_EXPANDPARTIAL)
									{
										pEntry->uState &= ~TVIS_EXPANDPARTIAL;
										UpdateRect(pData,(unsigned)lParam,0);
									}
								}

								lRet = 1;
							}

							break;

				case TVE_TOGGLE:	if(pEntry->uState&TVIS_EXPANDPARTIAL)
							{
								if(pEntry->uState&TVIS_EXPANDED)
								{
									pEntry->uState &= ~TVIS_EXPANDPARTIAL;
									UpdateRect(pData,(unsigned)lParam,0);
									lRet = 1;
									break;
								}
							}
							else{
								lRet = 1;
							}

							lRet=(TreeListToggleItem(pData,lParam,0))? 0:1;
							break;
				}
			}
		}

		UNLOCK(pData);

		return lRet;

	case TVM_SETINSERTMARK:									// Einf?emarke eintellen

		pData = GetHandle(hWnd);

		LOCK(pData);
		lRet=TreeListSetInsertMark(pData,(unsigned)lParam,(int)wParam);
		UNLOCK(pData);

		return lRet;

	case TVM_SETITEMBKCOLOR:								// Hintergrundfarbe eines Eintrages ?dern

		pData = GetHandle(hWnd);

		LOCK(pData);
		lRet=TreeListSetItemColor(pData,(unsigned)wParam&0xFFFFFF,wParam>>24,(COLORREF)lParam,0);
		UNLOCK(pData);

		return lRet;

	case TVM_SETITEMTEXTCOLOR:								// Textfarbe eines Eintrages ?dern

		pData = GetHandle(hWnd);

		LOCK(pData);
		lRet=TreeListSetItemColor(pData,(unsigned)wParam&0xFFFFFF,wParam>>24,(COLORREF)lParam,1);
		UNLOCK(pData);

		return lRet;

	case TVM_GETITEMBKCOLOR:								// Hintergrundfarbe eines Eintrages abfragen

		pData = GetHandle(hWnd);

		LOCK(pData);
		lRet=TreeListGetItemColor(pData,(unsigned)wParam,(unsigned)lParam,0);
		UNLOCK(pData);

		return lRet;


	case TVM_GETITEMTEXTCOLOR:								// Textfarbe eines Eintrages abfragen

		pData = GetHandle(hWnd);

		LOCK(pData);
		lRet=TreeListGetItemColor(pData,(unsigned)wParam,(unsigned)lParam,1);
		UNLOCK(pData);

		return lRet;

	case TVM_SORTCHILDRENEX:								// Sortieren mit Funktion

		pData = GetHandle(hWnd);

		LOCK(pData);
		lRet=TreeListSortItemsEx(pData,(TV_SORTEX*)lParam,(int)wParam);
		UNLOCK(pData);

		return lRet;

	case TVM_SORTCHILDRENCB:								// Sortieren mit Funktion

		pData = GetHandle(hWnd);

		LOCK(pData);
		lRet=TreeListSortItemsCb(pData,(TV_SORTCB*)lParam,(int)wParam);
		UNLOCK(pData);

		return lRet;

	case TVM_SORTCHILDREN:									// Sortieren der Kindereintr?e

		pData = GetHandle(hWnd);

		LOCK(pData);
		lRet=TreeListSortItems(pData,(unsigned)lParam,(int)wParam);
		UNLOCK(pData);

		return lRet;

	case TVM_GETITEMOFROW:									// Hole den Eintrag von einer Reihe

		pData = GetHandle(hWnd);

		LOCK(pData);
		if((unsigned)lParam<pData->uItemPosCount)
			lRet=pData->pItemPos[(unsigned)lParam];
		else	lRet=0;
		UNLOCK(pData);

		return lRet;

	case TVM_GETROWCOUNT:									// Hole die Anzahl der sichtbaren Reihen

		pData = GetHandle(hWnd);

		return pData->uItemPosCount;

	case TVM_GETCOUNTPERPAGE:								// Hole die Anzahl der darstelbaren Reihen

		pData = GetHandle(hWnd);

		return pData->uMaxEnties;

	case TVM_GETROWOFITEM:									// Suche Reihe zu einem Eintrag

		pData = GetHandle(hWnd);

		LOCK(pData);
		if((unsigned)lParam<=pData->uTreeItemsMax)
		{
			BaseItem *pEntry;

			pEntry=pData->pTreeItems[(unsigned)lParam];
			if(!pEntry)
				lRet = -1;
			else	lRet = pEntry->uShowPos-1;
		}
		else{
			lRet=-1;
		}
		UNLOCK(pData);

		return lRet;

	case TVM_EDITLABEL:										// Einen Eintrag editieren

		pData = GetHandle(hWnd);
		if(!(pData->uStyle&TVS_EDITLABELS))return 0;

		LOCK(pData);
		lRet=(LRESULT)TreeListEditLabel(pData,(unsigned)lParam,(unsigned)wParam);
		UNLOCK(pData);

		return lRet;

	case TVM_GETEDITCONTROL:								// Das Handle des Edit-Fensters abfragen

		pData = GetHandle(hWnd);
		return (LRESULT)pData->hEdit;

	case TVM_GETTOOLTIPS:									// Das Handle des ToolTip-Fensters abfragen

		pData = GetHandle(hWnd);
		return (LRESULT)pData->hToolTip;

	case TVM_SETTOOLTIPS:									// Das Handle f? das ToolTip-Fensters setzen

		pData = GetHandle(hWnd);
		lRet  = (LRESULT)pData->hToolTip;
		pData->hToolTip = (HWND)wParam;
		return lRet;

	case TVM_SETUSERDATASIZE:								// Einstellen der Größe der User-Daten


		if(lParam<0 || lParam>0x1000000)return -1;
		pData = GetHandle(hWnd);

		LOCK(pData);

		if(pData->uTreeItemsCount>0)
		{
			lRet = 0;
		}
		else{
			pData->uUserDataSize = lParam;
			lRet = lParam;
		}

		UNLOCK(pData);

		return lRet;

	case TVM_GETUSERDATASIZE:								// Abfragen der Größe der User-Daten

		pData = GetHandle(hWnd);
		return pData->uUserDataSize;

	case TVM_GETUSERDATA:									// Einen Zeiger auf die User-Daten holen

		pData = GetHandle(hWnd);

		if(pData->uUserDataSize && (unsigned)lParam<=pData->uTreeItemsMax)
		{
			BaseItem   *pEntry;

			pEntry = pData->pTreeItems[(unsigned)lParam];
			if(pEntry)return (LRESULT)(pEntry+1);
		}

		return 0;

	case TVM_SETCOLUMN:										// Einen Spalten-Header einstellen

		pData = GetHandle(hWnd);
		if(pData->hHeader)
		{
			HDITEM		sItem;
			TV_COLUMN  *pCol=(TV_COLUMN*)lParam;

			sItem.mask = 0;
			if(pCol->mask&TVCF_FMT  ){sItem.mask|=HDI_FORMAT; sItem.fmt    =pCol->fmt;    }
			if(pCol->mask&TVCF_IMAGE){sItem.mask|=HDI_IMAGE;  sItem.iImage =pCol->iImage; }
			if(pCol->mask&TVCF_WIDTH){sItem.mask|=HDI_WIDTH;  sItem.cxy    =pCol->cx;     }
			if(pCol->mask&TVCF_TEXT ){sItem.mask|=HDI_TEXT;   sItem.pszText=pCol->pszText;  sItem.cchTextMax=pCol->cchTextMax;}

			lRet=SendMessage(pData->hHeader,HDM_SETITEM,wParam,(LPARAM)&sItem);
		}
		else
		{
			lRet =0;
		}

		return lRet;


	case TVM_GETCOLUMN:										// Einen Spalten-Header abfragen

		pData = GetHandle(hWnd);
		if(pData->hHeader)
		{
			HDITEM		sItem;
			TV_COLUMN  *pCol=(TV_COLUMN*)lParam;

			sItem.mask = 0;
			if(pCol->mask&TVCF_FMT  ){sItem.mask|=HDI_FORMAT;}
			if(pCol->mask&TVCF_IMAGE){sItem.mask|=HDI_IMAGE; }
			if(pCol->mask&TVCF_WIDTH){sItem.mask|=HDI_WIDTH; }
			if(pCol->mask&TVCF_TEXT ){sItem.mask|=HDI_TEXT ;sItem.pszText=pCol->pszText;  sItem.cchTextMax=pCol->cchTextMax;}

			lRet=SendMessage(pData->hHeader,HDM_GETITEM,wParam,(LPARAM)&sItem);

			pCol->mask = 0;

			if(sItem.mask|=HDI_FORMAT){pCol->mask|=TVCF_FMT  ; pCol->fmt    =sItem.fmt;    }
			if(sItem.mask|=HDI_IMAGE ){pCol->mask|=TVCF_IMAGE; pCol->iImage =sItem.iImage; }
			if(sItem.mask|=HDI_WIDTH ){pCol->mask|=TVCF_WIDTH; pCol->cx     =sItem.cxy;    }
			if(sItem.mask|=HDI_TEXT  ){pCol->mask|=TVCF_TEXT ; pCol->pszText=sItem.pszText;  pCol->cchTextMax=sItem.cchTextMax;}
		}
		else{
			lRet =0;
		}

		return lRet;

	case TVM_SETCOLUMNWIDTH:								// Die Spaltenbreite einstellen

		pData = GetHandle(hWnd);
		if(pData->hHeader)
		{
			HDITEM sItem;

			sItem.mask = HDI_WIDTH;
			sItem.cxy  = (int)lParam;
			lRet=SendMessage(pData->hHeader,HDM_SETITEM,wParam,(LPARAM)&sItem);
		}
		else{
			lRet=0;
		}

		return lRet;

	case TVM_COLUMNAUTOEDIT:								// AutoEdit f? eine Spalte einstellen

		pData = GetHandle(hWnd);
		uVal  = (wParam>>10)&0x3F;

		if(uVal>=pData->uColumnCount)
		{
			return 0;
		}

		uMode = ((unsigned)wParam>>TVAE_MODEPOS)&7;

		if(uMode==TVAX_NONE)						
		{
			wParam = 0;
		}
		else if(uVal==0 && uMode>=TVAX_CHECK)			// Checkboxes are not allowed in the first column
		{
			return 0;
		}

		LOCK(pData);

		uChange  = pData->bColumnEdit [uVal]<<TVAE_MODEPOS;	
		uChange |= pData->bColumnFlags[uVal]&TVAE_STATEENABLE;

		pData->bColumnEdit  [uVal] = (BYTE)(uMode     );
		pData->bColumnFlags [uVal] = (BYTE)(wParam    );
		pData->bColumnCbChar[uVal] = (BYTE)(wParam>>16);
		pData->bColumnCbSize[uVal] = (BYTE)(wParam>>24);
		pData->pColumnCbData[uVal] = (void*)lParam;
		pData->iColumnCbIcon[uVal] = -1;


		if(uMode>=TVAX_CHECK)							// Sollen Checkboxen dargestetllt werden
		{
			if(!pData->hChecks)
			{
				CreateStateImageList(pData,1);
				UpdateHeight(pData);
			}

			if((uChange^wParam)&((TVAE_MODEMASK^TVAE_CHECK^TVAE_CHECKED)|TVAE_STATEENABLE))
			{
				UpdateColRect(pData,uVal);
			}
		}
		else if((uChange&TVAE_MODEMASK)>=TVAE_CHECK)	// Waren Checkboxen dargestetllt
		{
			if((uChange^wParam)&((TVAE_MODEMASK^TVAE_CHECK^TVAE_CHECKED)|TVAE_STATEENABLE))
			{
				UpdateColRect(pData,uVal);
			}
		}

		UNLOCK(pData);

		return 1;

	case TVM_COLUMNAUTOICON:								// Icons f? AutoEdit einstellen

		pData = GetHandle(hWnd);
		uVal  = U(wParam);

		if(uVal>=pData->uColumnCount || !(pData->bColumnFlags[uVal]&7))
		{
			return 0;
		}

		LOCK(pData);

		pData->iColumnCbIcon[uVal] = (int)lParam;

		UNLOCK(pData);


		return 1;

	case TVM_GETCOLUMNWIDTH:								// Die Spaltenbreite abfragen

		pData = GetHandle(hWnd);
		if(pData->hHeader)
		{
			HDITEM sItem;

			sItem.mask = HDI_WIDTH;
			sItem.cxy  = (int)lParam;
			SendMessage(pData->hHeader,HDM_GETITEM,wParam,(LPARAM)&sItem);
			lRet = sItem.cxy;
		}
		else{
			lRet=0;
		}

		return lRet;

	case TVM_CREATEDRAGIMAGE:								// Ein Drag-Image erzeugen

		pData = GetHandle(hWnd);
		LOCK(pData);
		lRet = (LRESULT)CreateDragImage(pData,(unsigned)lParam,(unsigned)wParam);
		UNLOCK(pData);
		return 0;

	}


	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}


//*****************************************************************************
//*
//*		TreeListDraw
//*
//*****************************************************************************
//	Zeichnet das Fenster
static void TreeListDraw(HWND hWnd,HDC hDc,RECT *pRect)
{
	COLORREF		uEvColor;
	COLORREF		uBkColor;
	COLORREF		uBxColor;
	COLORREF		uBtColor;
	COLORREF		uOdColor;
	COLORREF		uFrColor;
	COLORREF		uInColor;
	COLORREF		uOldColor;
	COLORREF		uOutColor;
	COLORREF		uTempColor;
	HRGN			hRgnMain;
	HRGN			hRgn[MAX_COLUMNS+1];
	SIZE			sSize;
	RECT			sRect;
	RECT			sArea;
	RECT			sButton;
	TreeListData   *pData;
	BaseItem	   *pTemp;
	BaseItem	   *pEntry;
	ExtraItem	   *pExtra;
	LPCTSTR			pText;
	HIMAGELIST		hImgList;
	unsigned		uTextSize;
	unsigned		uRgnCount;
	unsigned		uStyleEx;
	unsigned		uStyle;
	unsigned		uExtra;
	unsigned		uMark;
	unsigned		uItem;
	unsigned		uBits;
	unsigned		uPos;
	unsigned		uMax;
	int			   *pOffsets;
	int				iRnType[MAX_COLUMNS+1];
	int				iXscroll;
	int				iHeight;
	int				iIndent;
	int				iDelta;
	int				iImage;
	int				iShift;
	int				iStart;
	int				iCount;
	int				iLevel;
	int				iLast;
	int				iSize;
	int				iXpos;
	int				iYpos;
	int				iMaxX;
	int				iAdd;
	int				i;





	pData=GetHandle(hWnd);

	LOCK(pData);

	GetClientRect(hWnd,&sRect);

	if(!pRect)pRect=&sRect;

	iXscroll = -(int)pData->uScrollX;
	pOffsets = pData->iColumnXpos;
	hRgnMain = CreateRectRgn(pRect->left,pRect->top,pRect->right,pRect->bottom);

	uMax = pData->uColumnCount;
	if(!uMax)
	{
		hRgn   [  0 ] = CreateRectRgn(sRect.left,sRect.top,sRect.right,sRect.bottom);
		iRnType[  0 ] = CombineRgn(hRgn[0],hRgn[0],hRgnMain,RGN_AND);
		uRgnCount	  = 1;
	}
	else{
		for(uPos=0;uPos<uMax;uPos++)
		{
			hRgn   [uPos] = CreateRectRgn(sRect.left+pOffsets[uPos]+iXscroll,sRect.top,sRect.left+pOffsets[uPos+1]+iXscroll,sRect.bottom);
			iRnType[uPos] = CombineRgn(hRgn[uPos],hRgn[uPos],hRgnMain,RGN_AND);
		}

		hRgn   [uPos] = CreateRectRgn(sRect.left+pOffsets[uPos]+iXscroll,sRect.top,sRect.right,sRect.bottom);
		iRnType[uPos] = CombineRgn(hRgn[uPos],hRgn[uPos],hRgnMain,RGN_AND);

		uRgnCount = uMax+1;
	}



	iHeight		  = pData->iRowHeight;
	uStyleEx 	  = pData->uStyleEx;
	uStyle	 	  = pData->uStyle;
	iIndent	 	  = pData->iIndent;
	iShift	 	  = pData->iShift;
	uPos	 	  = pData->uScrollY;
	uMax	 	  = pData->uMaxEnties+uPos;


	if(iRnType[0]==NULLREGION)
		iMaxX = pData->iMaxSizeX;
	else	iMaxX = pData->iColumnXpos[1];

	if(uStyleEx&TVS_EX_ITEMLINES)
	{
		iHeight--;
	}

	if(uMax>pData->uItemPosCount)
	{
		uMax=pData->uItemPosCount;
	}

	if(pData->cIsEnabled)									// Ist das Fenster gessperrt
	{
		uBkColor  = pData->uColors[TVC_BK   ];
		uOdColor  = pData->uColors[TVC_ODD  ];
		uFrColor  = pData->uColors[TVC_FRAME];
		if(pData->uStyleEx&(TVS_EX_ITEMLINES|TVS_EX_ALTERNATECOLOR))
			uEvColor = pData->uColors[TVC_EVEN];
		else	uEvColor = uBkColor;
	}
	else{
		uBxColor  = GetSysColor(COLOR_WINDOW);
		uBkColor  = GetSysColor(COLOR_SCROLLBAR);
		uFrColor  = ((((uBkColor    )&0xFF)*2  +((uBxColor    )&0xFF))/3);
		uFrColor |= ((((uBkColor>> 8)&0xFF)*2  +((uBxColor>> 8)&0xFF))/3)<<8;
		uFrColor |= ((((uBkColor>>16)&0xFF)*2  +((uBxColor>>16)&0xFF))/3)<<16;
		uOdColor  = ((((uBkColor>> 1)&0x7F7F7F)+((uFrColor>> 1)&0x7F7F7F)));
		uEvColor  =     uBkColor;
	}

	uInColor = pData->uColors[TVC_LINE];
	uBtColor = pData->uColors[TVC_BOX ];
	uBxColor = uBkColor;
	iStart	 = 0;
	iLast	 = 0;



	sArea.top = sRect.top+pData->uStartPixel;
	SelectObject(hDc,pData->hFontN);
	SelectObject(hDc,hPatternPen);
	SetBkColor	(hDc,uBkColor);
	SetBkMode   (hDc,TRANSPARENT);
	SetTextAlign(hDc,TA_LEFT|TA_TOP);
	SetTextColor(hDc,pData->uColors[TVC_TEXT]);


	//******************** Eintr?e zeichnen ***************************************

	for(;uPos<uMax;uPos++)									// Alle Eintr?e ausgeben
	{
		uItem = pData->pItemPos[uPos];

		pEntry	= pData->pTreeItems[uItem];
		if(!pEntry)break;


		if((uStyleEx&TVS_EX_FULLROWMARK) && (pEntry->uState&TVIS_SELECTED))
		{
			if(uStyleEx&TVS_EX_ALTERNATECOLOR)
				uOutColor = (uPos&1)? pData->uColors[TVC_MARKODD]:pData->uColors[TVC_MARKEVEN];
			else	uOutColor = pData->uColors[TVC_MARK];

			uMark     = (unsigned)~TVIS_BKCOLOR;
		}
		else if(uStyleEx&TVS_EX_ALTERNATECOLOR)				// Farbe wechselweise ?dern
		{
			uOutColor = (uPos&1)? uOdColor:uEvColor;
			uMark     = 0xFFFFFFFF;
		}
		else{
			uOutColor = uBxColor;
			uMark     = 0xFFFFFFFF;
		}


		sArea.bottom	= sArea.top+pData->iRowHeight;
		sArea.left		= iXscroll;
		iLevel			= pEntry->uLevel;

		if(iRnType[0]==NULLREGION)
		{
			goto ExtraDraw;
		}

		uBits		=  pEntry->uState&0xFFFF;
		uBits      |=  pEntry->bFlags<<16;
		uBits      &=  uMark;
		iImage		= (uBits&LVIS_SELECTED)? pEntry->iSelectedImage:pEntry->iImage;
		pText		=  pEntry->pText;
		uTextSize	=  pEntry->uTextSize;

		if(pData->uSelectedSub && uItem==pData->uSelectedItem)
		{
			uBits &= ~TVIS_SELECTED;
		}

		if(pEntry->bCallback)
		{
			CallbackEntry(pData,pEntry,uItem,pEntry->bCallback,iImage,uTextSize,pText);

			pEntry	= pData->pTreeItems[uItem];
			if(!pEntry)break;
		}

		SelectObject(hDc,hRgn[0] );
		SetBkColor  (hDc,uBkColor);

		if(pData->cHasRootRow)iLevel++;

		if(iLevel<=0)goto NoRootLines;

		if(uStyle&(TVS_HASBUTTONS|TVS_HASLINES))
		{
			iLevel--;
		}

		if(uStyle&TVS_HASLINES)
		{
			pTemp		= pData->pTreeItems[pEntry->uParent];
			sArea.right = sArea.left+1;						// Eine leerer Pixelreihe am Anfang
			ExtTextOut(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);
			sArea.left += iIndent*iLevel+1;

			for(i=iLevel;i>0;i--)							// Bereich vor Schaltfl?hen
			{
				sArea.right = sArea.left;
				sArea.left -= iIndent;

				ExtTextOut(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);

				iXpos = sArea.left+iShift;

				if(pTemp)
				{
					if(pTemp->uNextItem)					// Zeichne vertikale Linien
					{
						MoveToEx(hDc,iXpos,sArea.top|1,NULL);
						LineTo  (hDc,iXpos,sArea.bottom);
					}

					pTemp = pData->pTreeItems[pTemp->uParent];
				}
			}

			sArea.left += iIndent*iLevel;
		}
		else{												// Ohne Linien zeichnen
			if(iLevel>0)
			{
				sArea.right = sArea.left+iIndent*iLevel;
				ExtTextOut(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);
				sArea.left += sArea.right;
			}
		}


		if(uStyle&TVS_HASBUTTONS)							// Fenster mit Schaltfl?hen ?
		{
			sArea.right = sArea.left+iIndent;
			iXpos		= sArea.left+iShift;
			iYpos		= sArea.top+pData->iRowHeight/2;

			ExtTextOut(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);

			if(uStyle&TVS_HASLINES)							// Linien unter Schaltfl?hen
			{
				MoveToEx(hDc,iXpos,sArea.top|1,NULL);

				if(pEntry->uNextItem)
					LineTo(hDc,iXpos,sArea.bottom);
				else	LineTo(hDc,iXpos,iYpos+1);


				MoveToEx(hDc,iXpos+1+(iYpos&1),iYpos,NULL);
				LineTo  (hDc,sArea.right      ,iYpos);
			}

			if(pEntry->bFlags&TVIX_HASBUTTON)				// Schaltfl?hen zeichnen
			{
				sButton.left    = iXpos-4;
				sButton.top	    = iYpos-4;
				sButton.right   = iXpos+5;
				sButton.bottom  = iYpos+5;

				SetBkColor(hDc,uBtColor);
				ExtTextOut(hDc,0,0,ETO_OPAQUE,&sButton,NULL,0,NULL);

				sButton.left   += 1;
				sButton.top	   += 1;
				sButton.right  -= 1;
				sButton.bottom -= 1;

				SetBkColor(hDc,pData->uColors[TVC_BOXBG]);
				ExtTextOut(hDc,0,0,ETO_OPAQUE,&sButton,NULL,0,NULL);

				sButton.left    = iXpos-2;
				sButton.top	    = iYpos  ;
				sButton.right   = iXpos+3;
				sButton.bottom  = iYpos+1;

				SetBkColor(hDc,uInColor);
				ExtTextOut(hDc,0,0,ETO_OPAQUE,&sButton,NULL,0,NULL);

				// '+' statt '-' Schaltfl?che zeichenen
				if((uBits^TVIS_EXPANDED)&(TVIS_EXPANDED|TVIS_EXPANDPARTIAL))
				{
					sButton.left    = iXpos  ;
					sButton.top	    = iYpos-2;
					sButton.right   = iXpos+1;
					sButton.bottom  = iYpos+3;
					ExtTextOut(hDc,0,0,ETO_OPAQUE,&sButton,NULL,0,NULL);
				}

				SetBkColor(hDc,uBkColor);
			}


			sArea.left += iIndent;
		}
		else if(uStyle&TVS_HASLINES)						// Nur Linien zeichnen ohne Schaltfl?hen
		{
			sArea.right = sArea.left+iIndent;
			ExtTextOut(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);

			iYpos	= sArea.top+pData->iRowHeight/2;
			iXpos	= sArea.left+iShift;
			MoveToEx(hDc,iXpos,sArea.top|1,NULL);

			if(pEntry->uNextItem)
				LineTo(hDc,iXpos,sArea.bottom);
			else	LineTo(hDc,iXpos,iYpos+1);


			MoveToEx(hDc,iXpos+1+(iYpos&1),iYpos,NULL);
			LineTo  (hDc,sArea.right      ,iYpos);

			sArea.left += iIndent;
		}

NoRootLines:

		if(uStyleEx&TVS_EX_ITEMLINES)						// Linien um den Eintrag zeichnen
		{
			iAdd		= 1;
			sArea.right = sArea.left+1;

			if(iLevel>=0)
			{
				SetBkColor(hDc,uFrColor);
				ExtTextOut(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);

				sArea.left++;
				sArea.bottom--;
				iStart=sArea.left;
			}
			else{
				ExtTextOut(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);

				sArea.left++;
				sArea.bottom--;
				iStart=sArea.left-1;
			}
		}
		else{
			iAdd = 0;
		}

		SetBkColor  (hDc,(uBits&TVIS_BKCOLOR)? pEntry->uColorBk:uOutColor);
		SelectObject(hDc,(uBits&TVIS_BOLD   )? pData->hFontB:pData->hFontN);

		if(pData->hStates)									// State-Icons anzeigen
		{
			sArea.right		= sArea.left+pData->iStatesXsize;
			iYpos			= sArea.top+(iHeight-pData->iStatesYsize)/2;
			i				= (uBits&LVIS_STATEIMAGEMASK)>>12;

			sArea.right    += iAdd;
			ExtTextOut(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);
			sArea.left     += iAdd;
			ImageList_Draw(pData->hStates,i,hDc,sArea.left,iYpos,ILD_TRANSPARENT);

			sArea.left     += pData->iStatesXsize;
			iAdd		    = 0;
		}

		if(pData->hImages && iImage!=TV_NOIMAGE)			// Icon zeichnen vom Haupteintrag
		{
			sArea.right		= sArea.left+pData->iImagesXsize;
			iYpos			= sArea.top+(iHeight-pData->iImagesYsize)/2;
			pEntry->bFlags |= TVIX_HASIMAGE;

			sArea.right    += iAdd;
			ExtTextOut(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);
			sArea.left     += iAdd;
			ImageList_Draw(pData->hImages,iImage,hDc,sArea.left,iYpos,ILD_TRANSPARENT|(uBits&(TVIS_OVERLAYMASK|LVIS_CUT)));

			sArea.left     += pData->iImagesXsize;
			iAdd		    = 0;
		}
		else{
			pEntry->bFlags &= ~TVIX_HASIMAGE;
		}

		sArea.right = pData->iColumnXpos[1];				// Text ausgeben vom Haupteintrag
		iYpos		= sArea.top+(iHeight-pData->iFontHeight)/2;

		if(uBits&(TVIS_SELECTED|TVIS_DROPHILITED|TVIS_UNTERLINE|TVIS_TRACKED|TVIS_TEXTCOLOR|TVIS_FOCUSED))
		{												// Das Feld speziel zeichen
			TCHAR	cTemp[256];
			INT		aPos [256];

			ExtTextOut(hDc,0,0,ETO_OPAQUE|ETO_CLIPPED,&sArea,NULL,0,NULL);

			sButton.top		= iYpos;
			sButton.left    = sArea.left+4;
			sButton.right	= sArea.right;
			sButton.bottom	= iYpos+pData->iFontHeight+2;

			if(!uTextSize)									// Feld ohne Text ?
			{
				sButton.right-=2;
				sButton.bottom--;
				pEntry->iTextPixels = 0;
			}
			else{
				DrawText(hDc,pText,uTextSize,&sButton,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_CALCRECT);
				pEntry->iTextPixels = sButton.right-sButton.left;
			}

			// Passt der Text in die Spalte
			if(sButton.left+pEntry->iTextPixels>=(int)(sArea.right-pData->uScrollX))
			{
				if(uTextSize>509)uTextSize=509;

				iSize  = sArea.right-pData->uScrollX-sButton.left-2;
				iSize -= (uBits&TVIS_BOLD)? pData->uTrippleB:pData->uTrippleN;
				if(iSize<3)
				{
					iCount = 0;
				}
				else{
					GetTextExtentExPoint(hDc,pText,uTextSize,iSize,&iCount,aPos,&sSize);
				}
				// Tempor?en Text mit "..." erzeugen
				memcpy(cTemp       ,pText,iCount*sizeof(TCHAR));
				memcpy(cTemp+iCount,_T("..."),3 *sizeof(TCHAR));

				pText		  = cTemp;
				uTextSize	  = iCount+3;
				sButton.right = sArea.right-2;
			}

			// Das Feld selektiert zeichen
			if((uBits&TVIS_SELECTED) && pData->cHasFocus && uItem==pData->uSelectedItem && !pData->uSelectedSub)
			{
				uTempColor=GetSysColor(COLOR_HIGHLIGHTTEXT);
				SelectObject(hDc,GetSysColorBrush(COLOR_HIGHLIGHT));
				Rectangle   (hDc,sButton.left-2,sButton.top-1,sButton.right+2,sButton.bottom+1);
			}
			else{
				if(uBits&TVIS_DROPHILITED)
				{
					uTempColor =     GetSysColor     (COLOR_HIGHLIGHTTEXT);
					SelectObject(hDc,GetSysColorBrush(COLOR_HIGHLIGHT   ));
					SelectObject(hDc,GetStockObject  (NULL_PEN          ));
					Rectangle   (hDc,sButton.left-2,sButton.top-1,sButton.right+2,sButton.bottom+1);
				}
				else if(uBits&TVIS_SELECTED)				// Ist das Feld ausgew?lt
				{
					if(pData->cHasFocus)
					{
						uTempColor =     GetSysColor     (COLOR_HIGHLIGHTTEXT);
						SelectObject(hDc,GetSysColorBrush(COLOR_HIGHLIGHT));
						SelectObject(hDc,GetStockObject  (NULL_PEN       ));
					}
					else{
						uTempColor = pData->uColors[TVC_TEXT];
						if(pData->uStyleEx&TVS_EX_FULLROWMARK)
							SelectObject(hDc,GetStockObject  (NULL_BRUSH));
						else	SelectObject(hDc,GetSysColorBrush(COLOR_MENU));
						SelectObject(hDc,GetStockObject  (NULL_PEN  ));
					}

					Rectangle(hDc,sButton.left-2,sButton.top-1,sButton.right+2,sButton.bottom+1);
				}
				else{										// Das Feld normal zeichen
					if(uBits&TVIS_TRACKED  )uTempColor = pData ->uColors[TVC_TRACK];
					else if(uBits&TVIS_TEXTCOLOR)uTempColor = pEntry->uColorText;
					else						 uTempColor = pData ->uColors[TVC_TEXT ];
					sButton.right--;
					sButton.left --;
				}

				SelectObject(hDc,hPatternPen);

				if(uBits&TVIS_FOCUSED)						// Einen punktierten Rahmen um den Text zeichen
				{
					SelectObject(hDc,GetStockObject(NULL_BRUSH));
					Rectangle   (hDc,sButton.left-2,sButton.top-1,sButton.right+2,sButton.bottom+1);
				}
			}

			SetTextColor(hDc,uTempColor);
			sButton.left+=pData->iFontOff;
			DrawText(hDc,pText,uTextSize,&sButton,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX);


			if(uBits&(TVIS_UNTERLINE|TVIS_TRACKED))			// Text unterstreichen
				if(pText && *pText)
				{
					sButton.left   -= pData->iFontOff;
					sButton.right  -= pData->iFontOff+1;
					sButton.top	   += pData->iFontLine;
					sButton.bottom  = sButton.top+1;
					uOldColor=SetBkColor(hDc,uTempColor);
					ExtTextOut(hDc,0,0,ETO_OPAQUE|ETO_CLIPPED,&sButton,NULL,0,NULL);
					SetBkColor(hDc,uOldColor);
				}

				SetTextColor(hDc,pData->uColors[TVC_TEXT]);
		}
		else{												// Das Feld normal ausgeben
			if(!pEntry->iTextPixels && uTextSize)
			{
				sButton.top		= iYpos;
				sButton.left    = sArea.left+4;
				sButton.right	= sArea.right;
				sButton.bottom	= iYpos+pData->iFontHeight+2;
				// Textbreite berechen
				DrawText(hDc,pText,uTextSize,&sButton,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_CALCRECT);

				pEntry->iTextPixels = sButton.right-sButton.left;
			}

			// Ist der Text größer als die Spalte
			if(sArea.left+pEntry->iTextPixels>=(int)(sArea.right-pData->uScrollX))	
			{
				INT	aPos[256];

				if(uTextSize>256)uTextSize=256;

				iSize  = sArea.right-sArea.left-pData->uScrollX;
				iSize -= (uBits&TVIS_BOLD)? pData->uTrippleB:pData->uTrippleN;
				if(iSize<3)
				{
					iCount = 0;
				}
				else{
					GetTextExtentExPoint(hDc,pText,uTextSize,iSize,&iCount,aPos,&sSize);
				}

				if(iCount>0)								// Passen noch Buchstaben vor "..."
				{
					sButton.right	= sArea.right;
					sArea.right		= sArea.left+2+aPos[iCount-1];

					ExtTextOut(hDc,sArea.left+2,iYpos,ETO_OPAQUE|ETO_CLIPPED,&sArea,pText,iCount,NULL);

					sArea.left		= sArea.right;
					sArea.right		= sButton.right;

					ExtTextOut(hDc,sArea.left  ,iYpos,ETO_OPAQUE|ETO_CLIPPED,&sArea,_T("..."),3,NULL);
				}
				else{
					ExtTextOut(hDc,sArea.left+2,iYpos,ETO_OPAQUE|ETO_CLIPPED,&sArea,_T("..."),3,NULL);
				}
			}
			else{
				ExtTextOut(hDc,sArea.left+2,iYpos,ETO_OPAQUE|ETO_CLIPPED,&sArea,pText,uTextSize,NULL);
			}
		}

		i  = sArea.left-iXscroll;
		i += pEntry->iTextPixels+5;
		if(i>iMaxX)iMaxX=i;

		if(uStyleEx&TVS_EX_ITEMLINES)						// Linien um den Eintrag zeichnen
		{
			SetBkColor(hDc,uFrColor);

			if(iLast>iStart)								// Ist das Feld nach links einger?kt gegen?er dem oberen
			{
				sArea.top--;
				sArea.bottom = sArea.top+1;
				sArea.left	 = iStart-1;
				ExtTextOut(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);
				sArea.top++;
			}

			iLast		  = iStart;							// Linie unter Feld zeichen
			sArea.top    += iHeight;
			sArea.left	  = iStart;
			sArea.bottom  = sArea.top+1;

			ExtTextOut(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);

			sArea.top    -= iHeight;
		}

		//************ Extraeintr?e zeichnen *********************************

ExtraDraw:

		for(uExtra=1;uExtra<=pData->uColumnCount;uExtra++)	// Extra Spalten zeichnen
		{
			GetRgnBox(hRgn[uExtra],&sButton);

			if(iRnType[uExtra]==NULLREGION)continue;

			SelectObject(hDc,hRgn[uExtra]);

			sArea.left  = pData->iColumnXpos[uExtra];
			sArea.left += iXscroll;


			if(uStyleEx&TVS_EX_ITEMLINES)					// Linie um den Eintrag zeichnen
			{
				SetBkColor(hDc,uFrColor);
				// Linke Linie
				sArea.right   = sArea.left+1;
				sArea.bottom += 1;

				ExtTextOut(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);

				sArea.left   += 1;							// Untere Linie
				sArea.top    += iHeight;
				sArea.bottom  = sArea.top+1;
				sArea.right	  = pData->iColumnXpos[uExtra+1];
				if(uExtra<pData->uColumnCount)sArea.right+=iXscroll;

				ExtTextOut(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);

				sArea.top    -= iHeight;
				sArea.bottom -= 1;
				iAdd		  = 1;
			}

			if(sArea.left>(int)pData->uSizeX)break;			// Noch im sichtbaren Bereich

			sArea.right	 = pData->iColumnXpos[uExtra+1];


			if(uExtra<pData->uColumnCount)					// Ist es die letze Spalte ?
			{
				sArea.right += iXscroll;
				pExtra       = pData->pExtraItems[uExtra-1][uItem];
			}
			else{
				pExtra = 0;
				if(uExtra<pData->uColumnCount)sArea.right+=iXscroll;
			}

			if(!pExtra)										// Leeres Feld zeichen
			{
				SetBkColor(hDc,uOutColor);
				ExtTextOut(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);
			}
			else{
				iSize		=  pData->iImagesXsize;
				hImgList	=  pData->hImages;
				iImage		=  pExtra->iImage;
				pText		=  pExtra->pText;
				uTextSize	=  pExtra->uTextSize;
				uBits		= (pExtra->uState&0xFFFF);
				uBits	   |= (pExtra->bFlags<<16);
				uBits	   |=  pEntry->uState&TVIS_BASEFLAGS;
				uBits	   &=  uMark;

				if(uExtra!=pData->uSelectedSub)
				{
					uBits &= ~TVIS_SELECTED;
				}

				if(pExtra->bCallback)						// Text ?er Callback holen
				{
					CallbackExtra(pData,pEntry,pExtra,uItem,uExtra,pExtra->bCallback,iImage,uTextSize,pText);
					pExtra = pData->pExtraItems[uExtra-1][uItem];
					if(!pExtra)break;
				}

				SetBkColor(hDc,(uBits&TVIS_BKCOLOR)? pExtra->uColorBk:uOutColor);



				if(pData->bColumnEdit[uExtra]>=TVAX_CHECK)	// Checkboxen statt Icons
				{
					hImgList =  pData->hChecks;
					iSize	 =  pData->iChecksXsize;
					iImage   = (pExtra->uState&TVIS_STATEIMAGEMASK)>>12;
					uBits   &= ~TVIS_OVERLAYMASK;

					if(iImage&8)
						if(pData->bColumnFlags[uExtra]&TVAE_STATEENABLE)
						{
							iImage &= 7;
						}
				}

				if(hImgList && iImage>TV_NOIMAGE)		// Icon zeichnen
				{
					sArea.right     = sArea.left+iSize+2;
					iYpos		    = sArea.top+(iHeight-pData->iImagesYsize)/2;
					pExtra->bFlags |= TVIX_HASIMAGE;

					SelectObject(hDc,(uBits&TVIS_BOLD)? pData->hFontB:pData->hFontN);
					ExtTextOut(hDc,0,0,ETO_OPAQUE,&sArea,NULL,0,NULL);
					ImageList_Draw(hImgList,iImage,hDc,sArea.left+1,iYpos,ILD_TRANSPARENT|(uBits&(TVIS_OVERLAYMASK|LVIS_CUT)));

					sArea.left  += iSize+1;
					sArea.right  = pData->iColumnXpos[uExtra+1];
					sArea.right += iXscroll;
				}
				else{
					pExtra->bFlags &= ~TVIX_HASIMAGE;
				}

				iYpos = sArea.top+(iHeight-pData->iFontHeight)/2;
				SelectObject(hDc,(uBits&TVIS_BOLD)? pData->hFontB:pData->hFontN);


				if(uBits&(TVIS_SELECTED|TVIS_DROPHILITED|TVIS_UNTERLINE|TVIS_TRACKED|TVIS_TEXTCOLOR|TVIS_FOCUSED))
				{										// Text ausgeben in spezilem Format
					TCHAR	cTemp[256];
					INT		aPos [256];


					ExtTextOut(hDc,0,0,ETO_OPAQUE|ETO_CLIPPED,&sArea,NULL,0,NULL);

					sButton.top		= iYpos;
					sButton.left    = sArea.left+4;
					sButton.right	= sArea.right;
					sButton.bottom	= iYpos+pData->iFontHeight+2;

					if(!uTextSize)
					{
						sButton.left--;
						sButton.right-=2;
						sButton.bottom--;
						pExtra->iTextPixels = 0;
					}
					else{
						DrawText(hDc,pText,uTextSize,&sButton,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_CALCRECT);
						pExtra->iTextPixels = sButton.right-sButton.left;
					}

					// Passt der Text in die Spalte
					if(sButton.left+pExtra->iTextPixels>=sArea.right)
					{
						if(uTextSize>253)uTextSize=253;

						iSize  = sArea.right-sButton.left-2;
						iSize -= (uBits&TVIS_BOLD)? pData->uTrippleB:pData->uTrippleN;
						if(iSize<3)
						{
							iCount = 0;
						}
						else{
							GetTextExtentExPoint(hDc,pText,uTextSize,iSize,&iCount,aPos,&sSize);
						}

						memcpy(cTemp       ,pText,iCount*sizeof(TCHAR));
						memcpy(cTemp+iCount,_T("..."),3 *sizeof(TCHAR));

						pText		  = cTemp;
						uTextSize	  = iCount+3;
						sButton.right = sArea.right-2;
					}


					switch(pData->cColumnAlign[uExtra])		// Textausrichtung ausgleichen
					{
					case DT_CENTER:	iDelta	= sArea  .right-sArea  .left;
						iDelta -= sButton.right-sButton.left;
						iDelta -= 6;
						iDelta /= 2;
						sButton.right += iDelta;
						sButton.left  += iDelta;
						break;

					case DT_RIGHT:	iDelta	= sArea  .right-sArea  .left;
						iDelta -= sButton.right-sButton.left;
						iDelta -= 6;
						sButton.right += iDelta;
						sButton.left  += iDelta;
						break;

					}
					// Ist das Feld ausgew?lt
					if((uBits&TVIS_SELECTED) && pData->cHasFocus && uItem==pData->uSelectedItem && uExtra==pData->uSelectedSub)
					{
						uTempColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
						SelectObject(hDc,GetSysColorBrush(COLOR_HIGHLIGHT));
						Rectangle   (hDc,sButton.left-2,sButton.top-1,sButton.right+2,sButton.bottom+1);
					}
					else{
						if(uBits&TVIS_DROPHILITED)
						{
							uTempColor =     GetSysColor     (COLOR_HIGHLIGHTTEXT);
							SelectObject(hDc,GetSysColorBrush(COLOR_HIGHLIGHT   ));
							SelectObject(hDc,GetStockObject  (NULL_PEN          ));
							Rectangle   (hDc,sButton.left-2,sButton.top-1,sButton.right+2,sButton.bottom+1);
						}
						else if(uBits&TVIS_SELECTED && uItem==pData->uSelectedItem)
						{
							if(pData->cHasFocus)
							{
								uTempColor =     GetSysColor     (COLOR_HIGHLIGHTTEXT);
								SelectObject(hDc,GetSysColorBrush(COLOR_HIGHLIGHT));
								SelectObject(hDc,GetStockObject  (NULL_PEN       ));
							}
							else{
								uTempColor = pData->uColors[TVC_TEXT];
								if(pData->uStyleEx&TVS_EX_FULLROWMARK)
									SelectObject(hDc,GetStockObject  (NULL_BRUSH));
								else	SelectObject(hDc,GetSysColorBrush(COLOR_MENU));
								SelectObject(hDc,GetStockObject  (NULL_PEN  ));
							}

							Rectangle(hDc,sButton.left-2,sButton.top-1,sButton.right+2,sButton.bottom+1);
						}
						else{
							if(uBits&TVIS_TRACKED  )uTempColor = pData ->uColors[TVC_TRACK];
							else if(uBits&TVIS_TEXTCOLOR)uTempColor = pExtra->uColorText;
							else						 uTempColor = pData ->uColors[TVC_TEXT ];
							sButton.right--;
							sButton.left --;
						}

						SelectObject(hDc,hPatternPen);

						if(uBits&TVIS_FOCUSED)				// Punktierten Rahmen um Text zeichen
						{
							SelectObject(hDc,GetStockObject(NULL_BRUSH));
							Rectangle   (hDc,sButton.left-2,sButton.top-1,sButton.right+2,sButton.bottom+1);
						}
					}

					SetTextColor(hDc,uTempColor);
					sButton.left+=pData->iFontOff;

					DrawText(hDc,pText,uTextSize,&sButton,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX);

					if(uBits&(TVIS_UNTERLINE|TVIS_TRACKED))	// Text unterstreichen
						if(pText && *pText)
						{
							sButton.left   -= pData->iFontOff;
							sButton.right  -= pData->iFontOff+1;
							sButton.top	   += pData->iFontLine;
							sButton.bottom  = sButton.top+1;
							uOldColor=SetBkColor(hDc,uTempColor);
							ExtTextOut(hDc,0,0,ETO_OPAQUE|ETO_CLIPPED,&sButton,NULL,0,NULL);
							SetBkColor(hDc,uOldColor);
						}

						SetTextColor(hDc,pData->uColors[TVC_TEXT]);
				}
				else{										// Den Text ganz normal ausgeben
					if(!pExtra->iTextPixels)
					{
						sButton.top		= iYpos;
						sButton.left    = sArea.left+4;
						sButton.right	= sArea.right;
						sButton.bottom	= iYpos+pData->iFontHeight+2;

						if(uTextSize)						// Textbreite berechen
						{
							DrawText(hDc,pText,uTextSize,&sButton,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_CALCRECT);
							pExtra->iTextPixels = sButton.right-sButton.left;
						}
						else{
							pExtra->iTextPixels = 0;
						}
					}
					// Ist der Text größer als die Spalte
					if(sArea.left+pExtra->iTextPixels>=sArea.right)
					{
						INT	aPos[256];

						if(uTextSize>256)uTextSize=256;

						iSize  = sArea.right-sArea.left;
						iSize -= (uBits&TVIS_BOLD)? pData->uTrippleB:pData->uTrippleN;
						if(iSize<3)
						{
							iCount = 0;
						}
						else{
							GetTextExtentExPoint(hDc,pText,uTextSize,iSize,&iCount,aPos,&sSize);
						}

						if(iCount>0)						// Passen noch Buchstaben vor "..."
						{
							sButton.right	= sArea.right;
							sArea.right		= sArea.left+2+aPos[iCount-1];

							ExtTextOut(hDc,sArea.left+2,iYpos,ETO_OPAQUE|ETO_CLIPPED,&sArea,pText,iCount,NULL);

							sArea.left		= sArea.right;
							sArea.right		= sButton.right;

							ExtTextOut(hDc,sArea.left  ,iYpos,ETO_OPAQUE|ETO_CLIPPED,&sArea,_T("..."),3,NULL);
						}
						else{
							ExtTextOut(hDc,sArea.left+2,iYpos,ETO_OPAQUE|ETO_CLIPPED,&sArea,_T("..."),3,NULL);
						}
					}
					else{
						switch(pData->cColumnAlign[uExtra])	// Textausrichtung
						{
						case DT_CENTER:	SetTextAlign(hDc,TA_CENTER|TA_TOP);
							ExtTextOut(hDc,(sArea.right+sArea.left)/2,iYpos,ETO_OPAQUE|ETO_CLIPPED,&sArea,pText,uTextSize,NULL);
							SetTextAlign(hDc,TA_LEFT|TA_TOP);
							break;

						case DT_RIGHT:	SetTextAlign(hDc,TA_RIGHT|TA_TOP);
							ExtTextOut(hDc,sArea.right-2,iYpos,ETO_OPAQUE|ETO_CLIPPED,&sArea,pText,uTextSize,NULL);
							SetTextAlign(hDc,TA_LEFT|TA_TOP);
							break;

						default:		ExtTextOut(hDc,sArea.left+2,iYpos,ETO_OPAQUE|ETO_CLIPPED,&sArea,pText,uTextSize,NULL);
							break;
						}
					}
				}
			}
		}


		sArea.top += pData->iRowHeight;
	}


	if(sArea.top<sRect.bottom)								// Untere Fl?he ohne Eintr?e f?len
	{
		sRect.top =	sArea.top;
		SelectObject(hDc,hRgnMain);
		SetBkColor  (hDc,uBkColor);
		ExtTextOut  (hDc,0,0,ETO_OPAQUE,&sRect,NULL,0,NULL);
	}


	if(pData->iMaxSizeX!=iMaxX)								// X-Scrollbar aktuallisieren
	{
		pData->iMaxSizeX=iMaxX;
		if(pData->uColumnCount==0)
			if(pData->iMaxSizeX!=(int)pData->uOldXCount)
			{
				UpdateScrollX(pData);
			}
	}


	UNLOCK(pData);

	DeleteObject(hRgnMain);

	for(uPos=0;uPos<uRgnCount;uPos++)
	{
		DeleteObject(hRgn[uPos]);
	}

}


