
// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 일부 CString 생성자는 명시적으로 선언됩니다.

// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.


#include <afxdisp.h>        // MFC 자동화 클래스입니다.



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT

#ifndef WINVER
#define WINVER 0x0600
#endif

#include <afxcontrolbars.h>     // MFC의 리본 및 컨트롤 막대 지원

#include <IsTools.h>

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/fileappender.h>
#include <log4cxx/logmanager.h>
#include <log4cxx/ndc.h>
#include <afxdhtml.h>
#include <afxdlgs.h>
#include "ELoadLayout.h"
using namespace log4cxx;
using namespace log4cxx::helpers;

#define	BUS_GROUP_ICON		3
#define	BUS_ICON		4
#define	LOAD_ICON		5
#define	MOTOR_ICON		6
#define	TRANSFORMER_ICON	7
#define	CABLE_ICON		8
#define	REVISION_ICON		9
#define	CAPACITOR_ICON		10
#define	GENERATOR_ICON		11
#define UPSDC_ICON		12
#define	AREA_ICON		13
#define SUBBUS_ICON		14

///////////////////////////////////////
/////////// MDB PASSWORD
///#define DB_PASSWORD CString(";Jet OLEDB:Database Password=admin;")
#define	PROVIDER	_T("Provider=Microsoft.Jet.OLEDB.4.0;")	//! 2011.02.24 added by HumKyung
#define DB_PASSWORD	_T("")
#define	ADMIN_PASSWORD	_T("XXXXXX")

#define	MODIFIED_COLOR	RGB(255,0,0)
#define	LIGHT_YELLOW	RGB(255,255,224)

#define	IS_COMBOBOX(str)\
	((str.GetLength() > 10) && ("COMBOBOX[" == str.Left(9)) && ("]" == str.Right(1)))

#define	IS_NUMBER_TYPE(str)\
	(str == _T("REGEXP(NUMBER)"))

#define	NUMBER_REGEXP	"[-+]?([0-9]*\\.)?[0-9]+([eE][-+]?[0-9]+)?"
#define	TEXT_REGEXP	"([a-zA-Z0-9 -._,/])+"
#define	UNNAMED_ITEM	_T("unnamed")

#define	UNNAMED_STR		_T("unnamed")
#define	UNLOCK_ITEM_STR		_T("UNLOCK")
#define	GROUP_LOCK_ITEM_STR	_T("GROUP LOCK ITEM")
#define	LOCK_ITEM_STR		_T("LOCK ITEM")

#define	READWRITABLE_STR	_T("RW")
#define	READONLY_STR		_T("R")

#define NOT_ASSIGNED		_T("Not Assigned")
#define	INCOMING		_T("INCOMING")
#define	HV_BUSGROUP		_T("HV")
#define	LV_BUSGROUP		_T("LV")
#define	SUBBUS			_T("SubBus")

#include "ELoadItem.h"
#include "BusItem.h"
#include "functor.h"

extern bool SortByName(ITEMS::CELoadItem* lhs , ITEMS::CELoadItem* rhs);
extern BOOL FileExist(CString rFilePath);
extern CString GetExecPath(void);
extern void CreateOrClearFolder(const CString& strDirectory);
extern COLORREF DarkenColor(COLORREF col,double factor);

struct MatchBusName : public binary_function<ITEMS::CBusItem*,string,bool>
{
	bool operator()(const ITEMS::CBusItem* lhs , const string& rhs) const
	{
		return (lhs->GetName() == rhs);
	}
};

class CMFCTabCtrlEx : public CMFCTabCtrl
{
	DECLARE_DYNAMIC(CMFCTabCtrlEx)
public:
	CMFCTabCtrlEx(){}
	~CMFCTabCtrlEx(){}

protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};

class CMFCTextColorListCtrl : public CMFCListCtrl
{
	DECLARE_DYNAMIC(CMFCTextColorListCtrl)
public:
	CMFCTextColorListCtrl(){}
	~CMFCTextColorListCtrl(){}

	// your extended header control  
	///CHeaderCtrlEx m_hdrCtrl; 
	map<DWORD_PTR , vector<COLORREF> > m_ItemColorMap;
protected:	
	// override the base classes "subclassing"  
	///virtual void InitHeader();
	///CMFCHeaderCtrl& GetHeaderCtrl(){ return m_hdrCtrl; }

	COLORREF OnGetCellTextColor(int nRow,int nColumn);
public:
	DECLARE_MESSAGE_MAP()
	virtual int OnCompareItems (LPARAM lParam1, LPARAM lParam2, int iColumn);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
};

typedef struct ProjectInfoTag
{
	string m_rProjectNo , m_rProjectName , m_rProjectDesc;
	string m_rProjectClient;
}ProjectInfo;

extern double GetMinSizeFromTable(const string& rVoltTypeString);

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

#import "..\..\..\Is_Tools\lib\msado15.dll" \
		no_namespace \
		rename( "EOF" , "adoEOF" )