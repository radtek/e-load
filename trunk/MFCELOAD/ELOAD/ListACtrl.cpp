// ListACtrl.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "ListACtrl.h"

#define IDC_EDIT_CHILD	     100//에티터 식별자
#define IDC_NUMEDIT_CHILD    101//숫자용에디터
#define IDC_COMBO_CHILD      102//콤보상자 식별자
#define IDC_CHECK_CHILD      103//체크상자 식별자
#define IDC_DATE_CHILD       104//날짜컨트럴 식별자
#define IDC_TIME_CHILD       105//시간컨트럴 식별자

WNDPROC g_datetimeCtrlProc;//전역 콜백함수 포인터

COLORREF g_clrBkgnd,g_clrText;//전역 셀색상변수
LRESULT CALLBACK DateTimeCtrlProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{

	switch(iMsg)
	{
   
	case WM_ERASEBKGND://배경을 지워주는 메시지를 받아서
		  {
            CRect rc;
			GetClientRect(hwnd,&rc);//넘어온 핸들을 이용하여 테두리를 구하고
			HBRUSH hbrBkgnd=CreateSolidBrush(g_clrBkgnd);
            GetClientRect(hwnd,&rc);//넘어온 핸들을 이용하여 테두리를 구하고
//구한테두리에 넘어온 DC를 이용하여 배경색 붙을 만들어서 배경을 칠하고
            ::FillRect((HDC)wParam,rc,hbrBkgnd);
			DeleteObject(hbrBkgnd);
//  리턴값은 사용자가 배경을 칠했으니 시스템은 그리지 마라고 함
            return 1;
		  } 
   }
 return CallWindowProc (g_datetimeCtrlProc, hwnd, iMsg, wParam, lParam) ; 

}




// CListACtrl

IMPLEMENT_DYNAMIC(CListACtrl, CListCtrl)

CListACtrl::CListACtrl()
{
	m_font=NULL;
	m_pColumnsInfo=NULL;
	m_pImageList=NULL;
	m_bSelected=false;
	//컬럼정보 배열 초기화
	m_pColumnsInfo=new CPtrArray;
	m_pColumnsInfo->SetSize(0);
	//선택막대 초기색상지정
	m_clrBkHighLight=0xFF8888;m_clrTextHighLight=0xFFFFFF;
	m_nHighLightMode=0;
	
}

CListACtrl::~CListACtrl()
{
	if(m_font){m_font->DeleteObject();delete m_font;}//전역 폰트 삭제
	if(m_pImageList!=NULL) 
	{
		m_pImageList->DeleteImageList();
		delete m_pImageList;//전역 이미리지리스트 제거
	}
	if(m_pColumnsInfo!=NULL)
	{
		int i=0;//전역 컬럼컨트럴 정보 변수 삭제
		while(i<m_pColumnsInfo->GetSize())
		{
			delete m_pColumnsInfo->GetAt(i++);
		}
		m_pColumnsInfo->RemoveAll();
		delete m_pColumnsInfo;
	}
}


BEGIN_MESSAGE_MAP(CListACtrl, CListCtrl)
	//마법사는 통하지 NM_CLICK메시지가 통하지 않음 마법사는 안됨
	//에디터(텍스트) 메시지
	ON_CONTROL(EN_KILLFOCUS,IDC_EDIT_CHILD,OnEditChild_KillFocus)
	ON_CONTROL(EN_UPDATE,IDC_EDIT_CHILD,OnEditChild_Update)
	//에디터(숫자) 메시지
	ON_CONTROL(EN_KILLFOCUS,IDC_NUMEDIT_CHILD,OnNumEditChild_KillFocus)
	ON_CONTROL(EN_UPDATE,IDC_NUMEDIT_CHILD,OnNumEditChild_Update)
	//콤보상자 메시지
	ON_CONTROL(CBN_KILLFOCUS,IDC_COMBO_CHILD,OnComboChild_LostFocus)
	ON_CONTROL(CBN_EDITUPDATE,IDC_COMBO_CHILD,OnComboChild_AfterUpdate)
	ON_CONTROL(CBN_SELCHANGE,IDC_COMBO_CHILD,OnComboChild_SelChange)
	//체크상자 메시지
	ON_CONTROL(BN_CLICKED,IDC_CHECK_CHILD,OnCheckChild_Click)
	ON_CONTROL(BN_KILLFOCUS,IDC_CHECK_CHILD,OnCheckChild_LostFocus)
	//날짜시간 컨트럴 메시지
	ON_NOTIFY_EX_RANGE(NM_KILLFOCUS,IDC_DATE_CHILD,IDC_TIME_CHILD,OnDtnKillFocus)//날짜컨트럴 포커스
	ON_NOTIFY_EX_RANGE(DTN_DATETIMECHANGE,IDC_DATE_CHILD,IDC_TIME_CHILD,OnDtnChange)//날짜컨트럴 날짜변경
	//리스트컨트럴이 부모에게 보내는 메시지
	ON_NOTIFY_REFLECT_EX(NM_CLICK, &CListACtrl::OnClick)
	ON_NOTIFY_REFLECT(LVN_INSERTITEM, OnLvnInsertitem)//리스트컨트럴 행삽입
	ON_WM_CTLCOLOR()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CListACtrl::OnNMCustomdraw)
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, &CListACtrl::OnLvnDeleteitem)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(LVN_ENDSCROLL, &CListACtrl::OnLvnEndScroll)
	ON_WM_LBUTTONDOWN()
	ON_WM_NCHITTEST()
	ON_WM_NCLBUTTONDOWN()
END_MESSAGE_MAP()



// CListACtrl 메시지 처리기입니다.

void CListACtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	
if(m_pColumnsInfo->IsEmpty()) return;//사용자가 컬럼정보를 입력했는지 검사
	LPNMLVCUSTOMDRAW pnmlvcd = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	
	switch(pnmlvcd->nmcd.dwDrawStage)//사용자 그리기 신호 구분
	{
	case CDDS_PREPAINT://컨트럴을 그리기전		
		*pResult = CDRF_NOTIFYITEMDRAW;//모든 아이템을 시스템이 그린다.
		break;
	case CDDS_ITEMPREPAINT://아이템을 그리기전
		{
			if(m_nHighLightMode){}//하이라이트 모드가 0번 컬럼선택이 아닐때는 통과한다.
			//0번외의 열을 선택하면 선택마크를 표시하지 않는다.
			else if(!m_bSelected) {SetItemState((int)pnmlvcd->nmcd.dwItemSpec,
					0,LVIS_FOCUSED|LVIS_SELECTED);}	
			*pResult = CDRF_NOTIFYSUBITEMDRAW;	//모든 서브아이템을 시스템이 그린다.
		}
		break;
	case CDDS_ITEMPREPAINT | CDDS_SUBITEM://아이템과 서브아이템을 그리기전(리포터 모드시에만)
		{			
			LVITEMDATA *pItemData=(LVITEMDATA*)pnmlvcd->nmcd.lItemlParam;
			int nRow=static_cast <int>(pnmlvcd->nmcd.dwItemSpec);
			int nCol=pnmlvcd->iSubItem;
			CDC* pDC = pDC->FromHandle(pnmlvcd->nmcd.hdc);//HDC->CDC
			CRect rect;
			GetColumnItemRect(nRow, nCol, LVIR_BOUNDS, rect);		
			//커스텀드로우 구조체에 글자 배경 색상 정보를 아이템데이타 구조체 정보와 일치시킨다.	
			//각 컬럼에 지정되어 있는 컨트럴의 종류를 파악한다.
			int nColumnCtrl=FindColumnCtrl(nCol);
			//0번 컬럼은 그리드선 다시 그리기를 방지하기 위해 아랫쪽 경계선에서 1만큼 올려서 그린다. 
			if(nCol==0) rect.bottom--;	
			switch(nColumnCtrl)//각 컬럼별 아이템구분
			{
			case DFC_NOUSE:{*pResult=CDRF_NOTIFYSUBITEMDRAW;return;	}//기본값 그리기 호출
			case DFC_CHECK:DrawItemCheckBox(nRow,nCol,pDC,rect,pItemData);//체크상자 그리기
				break;
			case DFC_PROGRESS:DrawItemProgress(nRow,nCol,pDC,rect,pItemData);//진행막대 그리기
				break;
			case DFC_IMAGE://아이콘을 포함한 글자
				{
					rect.left+=DrawItemImage(nRow,nCol,pDC,rect,pItemData);
					DrawItemText(nRow,nCol,pDC,rect,pItemData);
				}
				break;
			default:DrawItemText(nRow,nCol,pDC,rect,pItemData);//글자만 그리기
			}
			*pResult = CDRF_SKIPDEFAULT;//사용자지정 그리기신호 호출

		}	
	}
}


BOOL CListACtrl::OnClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	NMLISTVIEW *pNotifyStruct=(NMLISTVIEW *)pNMHDR;
	int nRow=pNotifyStruct->iItem;
	int nCol=pNotifyStruct->iSubItem;
//전역현재위치 저장 반드시 컨트럴이 보여지기 전에 처리한다.
	m_nRow=nRow;m_nCol=nCol;

	//컬럼에 사용자 정보가 없을때,행열이 선택되지 않았을때는 기본값 수행
	if(m_pColumnsInfo->IsEmpty() || nRow<0 || nCol<0) {}
	//컨트럴을 원하는 위치에 그리는 프로시져 호출
	else DrawColumnsCtrl(GetColumnsCtrl(nCol),nCol,nRow);
	
	return false;//부모에게도 메시지를 보낸다.
	
}
//컬럼에 지정된 컨트럴을 원하는 행에 그린다.
bool CListACtrl::DrawColumnsCtrl(CWnd *pWnd, int nCol, int nRow)
{
	//현재 컬럼에 설정되어 있는 컨트럴을 파악한다.
	int nColumnsCtrl=((LVCOLUMNDATA*)m_pColumnsInfo->GetAt(nCol))->nCtrlType;

	if(nColumnsCtrl==DFC_COMBO)//콤보박스
		DrawComboBoxCtrl((CComboBox*)pWnd,nRow,nCol);
	else if(nColumnsCtrl==DFC_EDIT)//에디터
		DrawEditCtrl((CEdit *)pWnd,nRow,nCol);
	else if(nColumnsCtrl==DFC_CHECK)//체크상자
		DrawCheckBoxCtrl((CButton*)pWnd,nRow,nCol);
	else if(nColumnsCtrl==DFC_DATE || nColumnsCtrl==DFC_TIME)//날짜컨트럴
		DrawDateTimeCtrl((CDateTimeCtrl*)pWnd,nRow,nCol);

	//결과 값을 전한다.
	if (pWnd && ::IsWindow(pWnd->m_hWnd)) return true;	
	return false;
}

BOOL CListACtrl::SetItemText(int nItem,int nSubItem,LPCTSTR lpszText)
{

	BOOL bRtn=CListCtrl::SetItemText(nItem,nSubItem,lpszText);
	//0번 컬럼을 수정할때 아이템을 다시 그린다.
	if(nSubItem==0)	UpdateSubItem(nItem,nSubItem);

	return bRtn;
}

// 에디트 컨트럴 그리기
void CListACtrl::DrawEditCtrl(CEdit *pEdit,int nRow, int nCol)
{
	m_bEdit=false;//에디트 상자의 상태(보통)를 설정
	if ((nRow < 0) || nRow >=GetItemCount())	return;
	if ((nCol < 0) || nCol >= GetHeaderCtrl()->GetItemCount())	return;
	// Make sure that nSubItem is valid
	if (GetColumnWidth(nCol) < 5) return;

	// make sure that the item is visible
	if (!EnsureVisible(nRow, TRUE)) return;
	
	CRect rect;
	GetColumnItemRect(nRow,nCol, LVIR_BOUNDS, rect);


	rect.left+=3;
	rect.top += 3;//그리드의 선 보다 아래쪽에
	rect.bottom -= 3;//그리드의 선 보다 윗쪽에
	//text align change
	LV_COLUMN lvcol;
	lvcol.mask = LVCF_FMT;
	GetColumn(nCol, &lvcol);
	//get window style
	DWORD dwStyle = GetWindowLong(pEdit->m_hWnd,GWL_STYLE);
	//컬럼의 정보로 부터 align 정보를 얻는다.
	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(nRow);
	//얻어온 align정보를 편집상자에 지정한다.
	SetWindowLong(pEdit->m_hWnd,GWL_STYLE,dwStyle|pItemData[nCol].nAlign);

	CString strValue = GetItemText(nRow, nCol);
	m_strValue=strValue;
	pEdit->MoveWindow(rect);
	pEdit->ShowWindow(SW_SHOW);
	pEdit->SetFocus();
	pEdit->SetWindowText(strValue);	
	m_bEdit=true;//에디트 상자의 상태(편집)	
		
}


void CListACtrl::OnEditChild_KillFocus()
{
	
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//포커스를 잃으면 에디터를 숨긴다.
	GetColumnsCtrl(m_nCol)->ShowWindow(SW_HIDE);

}

void CListACtrl::OnEditChild_Update()
{
	//글상자의 내용변경시 아이템 내용도 동시에 변경해야한다.
	CString str;
	CEdit *pEdit=(CEdit*)GetColumnsCtrl(m_nCol);
	pEdit->GetWindowText(str);
	SetItemText(m_nRow, m_nCol, str);
	//내용이 바뀌면 변경을 알림
	if(pEdit->GetModify())
	{
		pEdit->SetModify(false);
		LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(m_nRow);
		if(!pItemData->bUpdate) pItemData->bUpdate=true;
	}
		
}
void CListACtrl::OnNumEditChild_KillFocus()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//포커스를 잃으면 에디터를 숨긴다.
	GetColumnsCtrl(m_nCol)->ShowWindow(SW_HIDE);
}
void CListACtrl::OnNumEditChild_Update()
{
	//글상자의 내용변경시 아이템 내용도 동시에 변경해야한다.
	CString str;
	CEdit *pEdit=(CEdit*)GetColumnsCtrl(m_nCol);
	pEdit->GetWindowText(str);
	SetItemText(m_nRow, m_nCol, str);
	//내용이 바뀌면 변경을 알림
	if(pEdit->GetModify())
	{
		pEdit->SetModify(false);
		LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(m_nRow);
		if(!pItemData->bUpdate) pItemData->bUpdate=true;
	}
}


void CListACtrl::DrawComboBoxCtrl(CComboBox *pCombo,int nRow,int nCol)
{
	
	// Make sure that nSubItem is valid
	
	if ((nRow < 0) || nRow >=GetItemCount())	return;
	if ((nCol < 0) || nCol >= GetHeaderCtrl()->GetItemCount())	return;
	if (GetColumnWidth(nCol) < 5)	return;
	
	CRect rect;
	GetColumnItemRect(nRow, nCol, LVIR_BOUNDS, rect);
	
	rect.bottom-=6;
	
	CString	strValue=GetItemText(nRow,nCol);
	m_strValue=strValue;
	
	rect.bottom += 500;
	pCombo->MoveWindow(rect);
	pCombo->ShowWindow(SW_SHOW);	
	//콤보상자 자체의 크기를 조정하는 방법 ^^;;
	pCombo->SetItemHeight(-1,rect.Height());//높이 조정 콤보박스 에디터
	
	//::SendMessage(m_pCombo->m_hWnd,CB_SETITEMHEIGHT, (WPARAM) -1, (LPARAM)rect.Height());
	pCombo->SetWindowText(strValue);
	pCombo->SetFocus();
	m_bEdit=true;//에디트 상자의 상태(편집)
	
	
}

void CListACtrl::OnComboChild_LostFocus()
{
	//combobox가 lost focus면 hide한다.
	CComboBox *pCombo=(CComboBox*)GetColumnsCtrl(m_nCol);
	pCombo->ShowWindow(SW_HIDE);
}

void CListACtrl::OnComboChild_AfterUpdate()
{
//글상자의 내용변경시 아이템 텍스트도 변경내용을 곧바로 적용함
//이렇게 해야 정상적으로 동작함
	CString str = _T("");
	CComboBox *pCombo=(CComboBox*)GetColumnsCtrl(m_nCol);
	pCombo->GetWindowText(str);
	SetItemText(m_nRow, m_nCol, str);
	//내용이 바뀌면 변경을 알림
	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(m_nRow);
	if(!pItemData->bUpdate) pItemData->bUpdate=true;

}

void CListACtrl::OnComboChild_SelChange()
{
	CString str;
	CComboBox *pCombo=(CComboBox*)GetColumnsCtrl(m_nCol);
	pCombo->GetLBText(pCombo->GetCurSel(),str);
	
	//콤보상자에서 편집이 되지 않고 단순 선택일 때를 대배해서 다시한번 아이템에 글자를 쓴다.
	SetItemText(m_nRow,m_nCol,str);
	//내용이 바뀌면 변경을 알림
	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(m_nRow);
	if(!pItemData->bUpdate) pItemData->bUpdate=true;

}
void CListACtrl::DrawCheckBoxCtrl(CButton *pButton, int nRow, int nCol)
{
	// Make sure that nSubItem is valid
	
	if ((nRow < 0) || nRow >=GetItemCount())	return;
	if ((nCol < 0) || nCol >= GetHeaderCtrl()->GetItemCount())	return;
	if (GetColumnWidth(nCol) < 5)	return;
	
	CRect rect,chkboxrect;
	GetColumnItemRect(nRow, nCol, LVIR_BOUNDS, rect);
	
	chkboxrect=rect;

	chkboxrect.top+=1;chkboxrect.left += 3;chkboxrect.bottom-=1;

	CString	strValue=GetItemText(nRow,nCol);
	m_strValue=strValue;
	if (strValue.IsEmpty())
	{
		// center the checkbox
		chkboxrect.left = rect.left + rect.Width()/2 - rect.Height()/2;
		chkboxrect.right = chkboxrect.left + chkboxrect.Height();
	}
	
	pButton->MoveWindow(chkboxrect);
	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(nRow);
	pButton->SetCheck(pItemData[nCol].nChkState);
	pButton->ShowWindow(SW_SHOW);	
	pButton->SetWindowText(strValue);
	pButton->SetFocus();
	//m_bEdit=true;//에디트 상자의 상태(편집)

}
void CListACtrl::OnCheckChild_LostFocus()
{
	//체크상자가 lost focus면 hide한다.
	CWnd *pWnd=GetColumnsCtrl(m_nCol);
	pWnd->ShowWindow(SW_HIDE);
}
void CListACtrl::OnCheckChild_Click()
{
	CButton *pButton=(CButton*)GetColumnsCtrl(m_nCol);
	//체크상태가 바뀌면 변경을 알림
	int nCheckState=pButton->GetCheck();
	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(m_nRow);
	pItemData[m_nCol].nChkState=nCheckState;
	UpdateSubItem(m_nRow,m_nCol);
	if(!pItemData->bUpdate) pItemData->bUpdate=true;
}
//날짜컨트럴 그리기
void CListACtrl::DrawDateTimeCtrl(CDateTimeCtrl *pDateTime, int nRow, int nCol)
{
	m_bEdit=false;//에디트 상자의 상태(보통)를 설정
	if ((nRow < 0) || nRow >=GetItemCount())	return;
	if ((nCol < 0) || nCol >= GetHeaderCtrl()->GetItemCount())	return;
	// Make sure that nSubItem is valid
	if (GetColumnWidth(nCol) < 5) return;

	// make sure that the item is visible
	if (!EnsureVisible(nRow, TRUE)) return;
	GetItemColor(nRow,nCol,g_clrBkgnd,g_clrText);//배경색상과 글자색상을 전역으로 보낸다.
	CRect rect;
	GetColumnItemRect(nRow,nCol, LVIR_BOUNDS, rect);

	CString strValue = GetItemText(nRow, nCol);
	//날짜컨트럴과 시간컨트럴을 구분한다.
	DWORD dwStyle=GetWindowLong(pDateTime->m_hWnd,GWL_STYLE);
	//날짜시간구조체에 텍스트를 이용하여 날짜를 지정한다.
	SYSTEMTIME st;
	memset(&st,0,sizeof(SYSTEMTIME));
	//컨트럴의 날짜를 초기화 한다
	GetSystemTime(&st);
	pDateTime->SetTime(&st);

	if(dwStyle & DTS_TIMEFORMAT)
	{//시간컨트럴 일 경우
		st.wHour=_ttoi(strValue.Left(2));
		st.wMinute=_ttoi(strValue.Mid(4,2));
		st.wSecond=_ttoi(strValue.Right(2));
	}
	else
	{//달력컨트럴일경우
		//셀 글자를 이용하여 날짜를 입력한다.
		st.wYear=_ttoi(strValue.Left(4));
		st.wMonth=_ttoi(strValue.Mid(6,2));
		st.wDay=_ttoi(strValue.Right(2));
	}
	
	m_strValue=strValue;
	//윈도우가 그려지기 전에 날짜를 컨트럴에 입력한다.
	pDateTime->SetTime(&st);
	pDateTime->MoveWindow(rect);
	pDateTime->ShowWindow(SW_SHOW);
	pDateTime->SetFocus();

}
//날짜컨트럴 포커스 메시지 처리
BOOL CListACtrl::OnDtnKillFocus(UINT id,NMHDR *pNMHDR, LRESULT *pResult)
{
	::ShowWindow(pNMHDR->hwndFrom,SW_HIDE);
	return false;

}
//날짜컨트럴 날짜변경 메시지처리
BOOL CListACtrl::OnDtnChange(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
	//날짜변경신호구조체로 신호를 변경
	NMDATETIMECHANGE *pDt=(NMDATETIMECHANGE*)pNMHDR;
	CString str;

	//컨트럴에서 값이 넘어 올때만 포맷을 구성한다.
	if(pDt->dwFlags!=GDT_NONE)
	{
		if(id==IDC_DATE_CHILD)
		//넘겨준 년월일로 날짜포맷을 구성
			str.Format(_T("%04d-%02d-%02d"),pDt->st.wYear,pDt->st.wMonth,pDt->st.wDay);
		else if(id==IDC_TIME_CHILD)
			str.Format(_T("%02d:%02d:%02d"),pDt->st.wHour,pDt->st.wMinute,pDt->st.wSecond);
	}
	//리스트컨트럴의 글자를 변경
	SetItemText(m_nRow,m_nCol,str);
	//현재셀의 자료가 변경되었음을 저장한다.
	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(m_nRow);
	if(!pItemData->bUpdate) pItemData->bUpdate=true;

	return false;

}

//헤더 컨트럴의 경계선을 끌경우 컨트럴의 포커스를 제거한다.
void CListACtrl::OnHdnTracking(NMHEADER *pnmhdr)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(!GetItemCount()) return;//아이템이 없으면 그냥 끝낸다.
	::SetFocus(pnmhdr->hdr.hwndFrom);
}

//아이템이 추가될때
void CListACtrl::OnLvnInsertitem(NMHDR *pNMHDR, LRESULT *pResult)
{

	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//아이템데이타를 메모리에 탑재한후 리스트 컨트럴 각 아이템에 저장한다.
	LVITEMDATA *pItemData=new LVITEMDATA[GetColumnsCnt()];
	for(int i=0;i<GetColumnsCnt();i++)
	{
		pItemData[i].clrBkgnd=GetBkColor();
		pItemData[i].clrText=GetTextColor();
	}
	SetItemData(pNMLV->iItem,(DWORD)pItemData);
	
}

//컬럼의 컨트럴 포인터를 반환한다.
CWnd * CListACtrl::GetColumnsCtrl(int nColumn)
{
	return ((LVCOLUMNDATA*)m_pColumnsInfo->GetAt(nColumn))->pWnd;
}

//컬럼에 컨트럴 실제로 만든다.

void CListACtrl::AddColumnsCtrl(int nCol,int nCtrlType,int nDataType,int nDataFormat)

{
	CRect rect(0,0,0,0);
	//컨트럴 추가시 확장된 배열을 증가할때 사용할 임시 포인터
	CEdit* pEdit=NULL;CComboBox* pCombo=NULL;CButton* pButton=NULL;
	CDateTimeCtrl* pDateTime=NULL;

	LVCOLUMNDATA *pColumnData=new LVCOLUMNDATA;//컬럼별 컬럼정보 구조체 생성
	pColumnData->nCtrlType=nCtrlType;//컬럼별 컨트럴 종류지정
	pColumnData->nDataType=nDataType;//컬럼별 자료유형 지정
	pColumnData->nDataFormat=nDataFormat;//컬럼별 표시형식 지정

	//컨트럴 타입별로 컨트럴 포인터를 만든다.
	switch(nCtrlType)
	{
		case DFC_EDIT ://에디터
			{		
				pEdit=new CEdit;
				if(nDataType==1)//숫자 에디터
					pEdit->Create(WS_CHILD|ES_NUMBER , rect, this,IDC_NUMEDIT_CHILD);
				else if(nDataType==0 && nDataFormat==1)//멀티라인 에디터
					pEdit->Create(WS_CHILD|ES_MULTILINE , rect, this,IDC_NUMEDIT_CHILD);
				else//보통 에디터
					pEdit->Create(WS_CHILD, rect, this,IDC_EDIT_CHILD);

				pEdit->SetFont(m_font);
				pColumnData->pWnd=(CWnd*)pEdit;
				m_pColumnsInfo->SetAtGrow(nCol,pColumnData);break;			
			}
		case DFC_COMBO://콤보상자
			{
				pCombo=new CComboBox;
				//수평스크롤을 해야만 나타난다. 자동을 나타나지 않는다.
				pCombo->Create(WS_CHILD | CBS_DROPDOWN|WS_VSCROLL ,rect,this,IDC_COMBO_CHILD);
				pCombo->SetFont(m_font);
				pColumnData->pWnd=(CWnd*)pCombo;
				m_pColumnsInfo->SetAtGrow(nCol,pColumnData);break;
			}
		case DFC_CHECK://체크상자
			{
				pButton=new CButton;
				pButton->Create(_T(""),WS_CHILD|BS_NOTIFY|BS_AUTOCHECKBOX,rect,this,IDC_CHECK_CHILD);
				pButton->SetFont(m_font);
				pColumnData->pWnd=(CWnd*)pButton;
				m_pColumnsInfo->SetAtGrow(nCol,pColumnData);break;
			}
		case DFC_DATE://날짜컨트럴
			{
				pDateTime=new CDateTimeCtrl;
				pDateTime->Create(WS_CHILD|DTS_SHORTDATEFORMAT|DTS_SHOWNONE ,rect,this,IDC_DATE_CHILD);

				g_datetimeCtrlProc=(WNDPROC)SetWindowLong(pDateTime->m_hWnd,GWL_WNDPROC,(LONG)DateTimeCtrlProc);

				pDateTime->SetFont(m_font);
				pColumnData->pWnd=(CWnd*)pDateTime;
				m_pColumnsInfo->SetAtGrow(nCol,pColumnData);break;
			}
		case DFC_TIME://시간컨트럴
			{
				pDateTime=new CDateTimeCtrl;
				pDateTime->Create(WS_CHILD|DTS_TIMEFORMAT|DTS_SHOWNONE ,rect,this,IDC_TIME_CHILD);

				g_datetimeCtrlProc=(WNDPROC)SetWindowLong(pDateTime->m_hWnd,GWL_WNDPROC,(LONG)DateTimeCtrlProc);

				pDateTime->SetFont(m_font);
				pDateTime->SetFormat(_T("HH:mm:ss"));
				pColumnData->pWnd=(CWnd*)pDateTime;
				m_pColumnsInfo->SetAtGrow(nCol,pColumnData);break;
			}
		default:m_pColumnsInfo->SetAtGrow(nCol,pColumnData);//기본값
	}//switch 닫음
}

//지정된 컬럼에 설정되어 있는 컨트럴의 종류를 알아낸다
int CListACtrl::FindColumnCtrl(int nCol)
{
	return ((LVCOLUMNDATA*)m_pColumnsInfo->GetAt(nCol))->nCtrlType;
}
//리스트 컨트럴에 열을 생성하고, 열에 컨트럴을 배치한다.
int CListACtrl::InsertColumn(int nCol,const LVCOLUMN *pColumn,int nCtrlType,int nDataType,int nDataFormat)
{
	//컬럼별 컨트럴 생성 함수를 호출한다.
	AddColumnsCtrl(nCol,nCtrlType,nDataType,nDataFormat);

	return CListCtrl::InsertColumn(nCol,pColumn);
}
int CListACtrl::InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat,
							 int nWidth, int nSubItem, int nCtrlType,int nDataType,int nDataFormat)
{
	//컬럼별 컨트럴 생성 함수를 호출한다.
	AddColumnsCtrl(nCol,nCtrlType,nDataType,nDataFormat);
	
	return CListCtrl::InsertColumn(nCol,lpszColumnHeading,nFormat,nWidth,nSubItem);
}
BOOL CListACtrl::DeleteColumn(int nCol)
{
	delete m_pColumnsInfo->GetAt(nCol);
	m_pColumnsInfo->RemoveAt(nCol);
	
	return CListCtrl::DeleteColumn(nCol);
}
//사용자그리기 아이템텍스트
void CListACtrl::DrawItemText(int nRow, int nCol, CDC *pDC,CRect &rect,
									LVITEMDATA *pItemData)
{

	if (rect.IsRectEmpty())
	{
		return;
	}
	COLORREF clrBkgnd,clrText;
	CheckItemColor(nRow,nCol,clrBkgnd,clrText);
	
	if(!IsBkImage() || GetItemState(nRow,LVIS_SELECTED))pDC->FillSolidRect(&rect,clrBkgnd);	
	
	rect.left+=3;rect.right-=3;//선에 바짝 붙는 것을 방지
	rect.top+=3;rect.bottom-=3;
	CString str,strComma;
	str = GetItemText(nRow, nCol);
	//컬럼의 정보를 가져온다.
	LVCOLUMNDATA *pColumnData=(LVCOLUMNDATA*)m_pColumnsInfo->GetAt(nCol);
	

	if (!str.IsEmpty())
	{		
		if(pColumnData->nDataType==1 && !pColumnData->nDataFormat)
		{//자료종류가 숫자이고, 형식이 1이 아니면 3자리 구분 콤마를 찍는다.
			strComma=str;str=InputComma(strComma);
		}
		//사용자 지정 글자체가 지정되었는가 검사한다.
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(clrText);
		pDC->SetBkColor(clrBkgnd);
		if(pItemData[nCol].nFontEffect || pItemData[nCol].nFontSize)
		{
			CFont newFont;
			GetItemFont(newFont,pItemData[nCol].nFontEffect,pItemData[nCol].nFontSize);
			CFont *prevFont=pDC->SelectObject(&newFont);		
			if(pItemData[nCol].nAlign==0x4)
				SpreadText(pDC->m_hDC,rect,str,3);
			else //멀티라인은 수직정렬이 안된다. 반드시 DT_SINGLELINE를 지정해야 한다.
			{//멀티라인 구분
				if(pColumnData->nDataType==0 && pColumnData->nDataFormat==1)
					pDC->DrawText(str, &rect,pItemData[nCol].nAlign|DT_WORDBREAK);//멀티라인
				else pDC->DrawText(str, &rect,pItemData[nCol].nAlign|DT_VCENTER|DT_SINGLELINE);//싱글라인
			}
			pDC->SelectObject(prevFont);
			newFont.DeleteObject();
		}
		else
		{
			if(pItemData[nCol].nAlign==0x4)
				SpreadText(pDC->m_hDC,rect,str,3);
			else
			{//멀티라인 구분
				if(pColumnData->nDataType==0 && pColumnData->nDataFormat==1)
					pDC->DrawText(str, &rect,pItemData[nCol].nAlign|DT_WORDBREAK);//멀티라인
				else pDC->DrawText(str, &rect,pItemData[nCol].nAlign|DT_VCENTER|DT_SINGLELINE);//싱글라인
			}
		}

	}
}

//아이템에 체크상자를 그린다.
void CListACtrl::DrawItemCheckBox(int nRow, int nCol, CDC *pDC,CRect &rect,
									LVITEMDATA *pItemData)
{
	if (rect.IsRectEmpty())
	{
		return;
	}
	COLORREF clrBkgnd,clrText;
	CheckItemColor(nRow,nCol,clrBkgnd,clrText);

	if(!IsBkImage() || GetItemState(nRow,LVIS_SELECTED)) pDC->FillSolidRect(&rect,clrBkgnd);

	CRect chkboxrect;
	chkboxrect = rect;
	//사각형의 높이의 중앙에서 위아래로 6만큼 제함
	chkboxrect.top=rect.Height()/2-6+chkboxrect.top;
	chkboxrect.bottom=chkboxrect.bottom-rect.Height()/2+6;
	
	chkboxrect.left += 3;		// line up checkbox with header checkbox
	chkboxrect.right = chkboxrect.left + chkboxrect.Height();	// width = height
	CString str=GetItemText(nRow,nCol);
	if (str.IsEmpty())
	{
		// center the checkbox

		chkboxrect.left = rect.left + rect.Width()/2 - rect.Height()/2 ;
		chkboxrect.right = chkboxrect.left + chkboxrect.Height();
	}
	// 체크상자 안쪽을 배경색으로 칠한다.	
	pDC->FillSolidRect(&chkboxrect,0xFFFFFF);

	// 입체 사각테두리
	pDC->Draw3dRect(&chkboxrect,0x4B4B4B,0x989898);
	//체크상자 안쪽의 체크 무늬를 그리는 루틴 
	if(pItemData[nCol].nChkState)
	{
		CPen *prevPen = NULL;
		COLORREF clrCheck=0x888800;

		CPen grayPen(PS_SOLID, 1,clrCheck);
		prevPen = pDC->SelectObject(&grayPen);
		
		// draw the checkmark시작지점
		int x = chkboxrect.right-4;
		ASSERT(x < chkboxrect.right);
		int y = chkboxrect.top + 2;
		//x값이 사각형의 넓이의 3/5까지 오도록 선을 아래로 긋는다.
		while (x>=chkboxrect.right-4-(chkboxrect.Width()*3/5) &&
			y<=chkboxrect.bottom-2-(chkboxrect.Height()/2))//
		{
			pDC->MoveTo(x, y);
			pDC->LineTo(x, y+3);//y값 오른쪽 선 굵기
			x--;
			y++;
		}
		//x값이 사각형의 왼쪽 모서리 까지 오도록 선을 이어서 위로 긋는다.
		while (x>=chkboxrect.left+3)//i값 왼쪽 선 길이
		{
			pDC->MoveTo(x, y);
			pDC->LineTo(x, y+3);//y값은 왼쪽선의 굵기
			x--;
			y--;
		}

		if (prevPen)
		{
			pDC->SelectObject(prevPen);
			grayPen.DeleteObject();
		}
	}
	

	if (!str.IsEmpty())
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(clrText);
		CRect textrect;
		textrect = rect;
		textrect.left = chkboxrect.right + 4;

		UINT nFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE;
		
		pDC->DrawText(str, &textrect, nFormat);
	}
}

//아이템에 아이콘 이미지를 그린다

int CListACtrl::DrawItemImage(int nRow, int nCol, CDC *pDC, CRect &rect,
									 LVITEMDATA *pItemData)
{
	if (rect.IsRectEmpty())
	{
		return 0;
	}
	//배경이미지가 지정되어 있으면 아이템 배경을 칠하지 않는다.
	COLORREF clrBkgnd,clrText;
	CheckItemColor(nRow,nCol,clrBkgnd,clrText);
	if(!IsBkImage() ||GetItemState(nRow,LVIS_SELECTED)) pDC->FillSolidRect(&rect,clrBkgnd);

	int nWidth = 0;
	rect.left+=1;

	CImageList* pImageList = GetImageList(LVSIL_SMALL);
	if (pImageList)
	{
		SIZE sizeImage;
		sizeImage.cx = sizeImage.cy = 0;
		IMAGEINFO info;

		int nImage = -1;
		if (pItemData)
			nImage = pItemData[nCol].nImage;

		if (nImage == -1)
			return 0;

		if (pImageList->GetImageInfo(nImage, &info))
		{
			sizeImage.cx = info.rcImage.right - info.rcImage.left;
			sizeImage.cy = info.rcImage.bottom - info.rcImage.top;
		}

		if (nImage >= 0)
		{
			if (rect.Width() > 0)
			{
				POINT point;

				point.y = rect.CenterPoint().y - (sizeImage.cy >> 1);
				point.x = rect.left;

				SIZE size;
				size.cx = rect.Width() < sizeImage.cx ? rect.Width() : sizeImage.cx;
				size.cy = rect.Height() < sizeImage.cy ? rect.Height() : sizeImage.cy;

				// save image list background color
				COLORREF rgb = pImageList->GetBkColor();

				// set image list background color
				pImageList->SetBkColor(clrBkgnd);
				pImageList->DrawIndirect(pDC, nImage, point, size, CPoint(0, 0));
				pImageList->SetBkColor(rgb);

				//nWidth = sizeImage.cx + m_HeaderCtrl.GetSpacing();
				nWidth=sizeImage.cx;
			}
		}
	}

	return nWidth;
}
//진행막대를 아이템에 그린다.
void CListACtrl::DrawItemProgress(int nRow, int nCol, CDC *pDC, CRect &rect,
								  LVITEMDATA *pItemData)
{
	UNUSED_ALWAYS(nRow);

	if (rect.IsRectEmpty())
	{
		return;
	}
	//배경색상 적용
	COLORREF clrBkgnd,clrText;
	CheckItemColor(nRow,nCol,clrBkgnd,clrText);
	if(!IsBkImage()) pDC->FillSolidRect(rect,clrBkgnd);
	rect.left += 1;		// leave margin in case row is highlighted
	int nPercent=pItemData[nCol].nProgressPercent;
	// 진행막대의 배경색상 
	pDC->FillSolidRect(rect,clrBkgnd);

	if (nPercent >= 0)
	{
		// draw progress bar and text
		//진행막대 테두리
		CRect LeftRect, RightRect;
		//진행막대 색상
		COLORREF clrProgBkgnd;
		LeftRect = rect;
		LeftRect.left += 1;
		RightRect = LeftRect;
		if(nPercent>100) nPercent=100;
		int w = (LeftRect.Width() * nPercent) / 100;
		LeftRect.right = LeftRect.left + w - 1;
		RightRect.left = LeftRect.right;
		//아이템이 선택되어 있으면 배경색을 선택막대와 일치시킨다.
		if(GetItemState(nRow,LVIS_SELECTED)==LVIS_SELECTED) clrProgBkgnd=clrBkgnd;
		else clrProgBkgnd=0x0088FF;
		pDC->FillSolidRect(LeftRect,clrProgBkgnd);

			
		CString str;
		CFont *PrevFont=NULL;
		if(!nPercent)
			str=_T("대기");
		else if(nPercent<100)
			str.Format(_T("%d%%"), nPercent);
		else if(nPercent>=100)
			str=_T("완료");
			
		//진행막대의 글자 표시	
		pDC->SetBkMode(TRANSPARENT);

		CRect TextRect;
		TextRect = rect;
		TextRect.DeflateRect(1, 1);

		CRgn rgn;
		rgn.CreateRectRgn(LeftRect.left,LeftRect.top, LeftRect.right, 
				LeftRect.bottom);
		pDC->SelectClipRgn(&rgn);
		//아직 진행되지 않은 글자 색상
		pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));//crBkgnd);
		pDC->DrawText(str, &TextRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		rgn.DeleteObject();
		
		rgn.CreateRectRgn(RightRect.left, RightRect.top, RightRect.right, 
				RightRect.bottom);
		pDC->SelectClipRgn(&rgn);
		//이미 진행된 곳의 글자 색상
		pDC->SetTextColor(RGB(217,217,217));
		pDC->DrawText(str, &TextRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		rgn.DeleteObject();
		pDC->SelectClipRgn(NULL);
		
	}
	
}
//선택막대의 배경과 글자색지정
void CListACtrl::SetItemHighLightColor(COLORREF clrBkgnd,COLORREF clrText)
{
	m_clrBkHighLight=clrBkgnd;m_clrTextHighLight=clrText;
}

void CListACtrl::SetItemImage(int nRow,int nCol,int nImageListImage)
{
	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(nRow);
	pItemData[nCol].nImage=nImageListImage;

}
void CListACtrl::SetFont(CFont *pFont, BOOL bRedraw)
{
	CWnd::SetFont(pFont,bRedraw);//원래 글자 지정함수 호출
	if(m_font){m_font->DeleteObject();m_font=NULL;}//지정된 글꼴이 있으면 삭제한다.
	//입력된 새로운 폰트를 전역폰트로 지정한다.
	LOGFONT lf;
	memset(&lf,0,sizeof(LOGFONT));
	pFont->GetLogFont(&lf);
	m_font=new CFont;
	m_font->CreateFontIndirect(&lf);
	//폰트변경시 컨트럴 폰트를 변경한다.
	EnumChildWindows(m_hWnd,EnumChildProc,(LPARAM)m_font->operator HFONT());

	SetItemHeight(m_nItemHeight);
}
//아이템 높이에 따라 이미지리스트를 전역이미지 리스트로 치환한다.
CImageList *CListACtrl::SetImageList(CImageList *pImageList,int nImageListType)
{
	//이미지타입이 LVSIL_SMALL이 아니면 원래의 함수를 호출한다.
	if(nImageListType!=LVSIL_SMALL)
		return CListCtrl::SetImageList(pImageList,nImageListType);
	//이미지타입이 LVSIL_SMALL이면 넘어온 이미지 리스트를 전역이미지 리스트로 치환한다.
	CImageList *pImageListDst;
	pImageListDst=new CImageList;

	IMAGEINFO imageinfo;
	memset(&imageinfo,0,sizeof(imageinfo));
	CRect rcSrc,rcDst;
	CDC memDC;
	CBitmap bm, *pbmPrev;
	pImageList->GetImageInfo(0,&imageinfo);//원본이미지리스트 이미지 정보 획득
	rcSrc=imageinfo.rcImage;
	CPoint drawpoint;
	rcDst=rcSrc;//원본이미지 크기를 대상이미지 크기로 넘겨준다.
	//대상이미지 크기를 아이템의 높이와 비교하여 재조정한다.
	if(m_nItemHeight>rcSrc.Height()) rcDst.bottom=m_nItemHeight-rcSrc.Height()+rcSrc.bottom;
	pImageListDst->Create(rcDst.Width(),rcDst.Height(),ILC_COLORDDB|ILC_MASK,0,-1);
	//대상이미지에 원본이미지를 그릴 위치를(세로중앙) 정한다.
	drawpoint.y=(rcDst.Height()-rcSrc.Height())/2;drawpoint.x=0;
	//메모리 도화지를 꺼낸다.
	memDC.CreateCompatibleDC(GetDC());
   	
	for(int i=0;i<pImageList->GetImageCount();i++)
	{
		//대상 비트맵을 만든다.
		bm.CreateCompatibleBitmap(GetDC(),rcDst.Width(),rcDst.Height());
		pbmPrev=memDC.SelectObject(&bm);//도화지에 비트맵을 선택한다.
		memDC.FillSolidRect(rcDst,GetBkColor());//도화지에 이미지 배경을 칠한다.
		pImageList->DrawIndirect(&memDC,i,drawpoint,
			CSize(rcSrc.Width(),rcSrc.Height()),CPoint(0,0));//배경위에 원본 이미지를 그린다.
		memDC.SelectObject(pbmPrev);//메모리 도화지를 원위치 시킨다.
		pImageListDst->Add(&bm,GetBkColor());//이미지 리스트에 그린 그림을 넣는다.
		bm.DeleteObject();
	}
	if(m_pImageList){m_pImageList->DeleteImageList();m_pImageList=NULL;}
	m_pImageList=pImageListDst;
	return CListCtrl::SetImageList(pImageListDst,nImageListType);

}
//각컬럼별 아이템의 테두리 크기를 얻는다.
bool CListACtrl::GetColumnItemRect(int nRow, int nCol, int nArea, CRect &rect)
{
	if(!GetColumnsCnt()) return false;
	
	bool bRtn=GetSubItemRect(nRow,nCol,nArea,rect);
	if(nCol==0)
	{
		if(bRtn) rect.right=rect.left+GetColumnWidth(nCol);		
	}
	return bRtn;
}



//진행막대의 위치를 지정한다.
void CListACtrl::SetProgressPos(int nRow, int nCol, int nPos)
{
	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(nRow);
	pItemData[nCol].nProgressPercent=nPos;
}
//진행막대의 위치를 얻는다.
int CListACtrl::GetProgressPos(int nRow, int nCol)
{
	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(nRow);
	return pItemData[nCol].nProgressPercent;

}
//아이템의 높이 조정
void CListACtrl::SetItemHeight(long nHeight)
{	
	//로그폰트 구조체를 동적으로 생성	
	LOGFONT *plf=(LOGFONT*)malloc(sizeof(LOGFONT));
	memset(plf,0,sizeof(LOGFONT));
	GetFont()->GetLogFont(plf);//사용자 설정 폰트를 얻는다.
	if(plf->lfHeight<=0)
	{//기본 폰트일 경우에는 폰트를 만든다.
		plf->lfHeight=nHeight+3;plf->lfWidth=0;
		_tcscpy_s(plf->lfFaceName,10,_T("굴림"));
		m_font=new CFont;
		m_font->CreateFontIndirect(plf);CWnd::SetFont(m_font);
	}
	//설정값이 글꼴 크기보다 작으면 기본 값으로 한다.
	if(nHeight<=plf->lfHeight+2) nHeight=plf->lfHeight+6;
	delete plf;
	m_nItemHeight=nHeight;
	//이미지리스트를 만든다.(각행의 넓이를 조정하기 위한 트릭임)
	if(GetImageList(LVSIL_SMALL))//이미지 리스트가 있을때
	{
		SetImageList(GetImageList(LVSIL_SMALL),LVSIL_SMALL);
	}
	else//이미지 리스트가 없을때
	{
		m_pImageList=new CImageList;
		m_pImageList->Create(16,nHeight,ILC_COLORDDB|ILC_MASK,0,-1);
		CListCtrl::SetImageList(m_pImageList,LVSIL_SMALL);
	}
}
//아이템이 선택되었는가를 확인하여 색상을 정한다.
void CListACtrl::CheckItemColor(int nRow,int nCol,COLORREF &clrBkgnd,COLORREF &clrText)
{
	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(nRow);
	if(pItemData)
	{
		clrBkgnd=pItemData[nCol].clrBkgnd;
		clrText=pItemData[nCol].clrText;

		if (GetItemState(nRow, LVIS_SELECTED))
		{
			if(m_nHighLightMode==2)//선택모드가 글자색만 바꿀경우
				clrText=0xFFFFFF-m_clrBkHighLight;
			else
			{
				clrBkgnd=m_clrBkHighLight;clrText=m_clrTextHighLight;
			}
		}
	}
}

//각 아이템의 배경색상과 글자색상을 지정한다.
void CListACtrl::SetItemColor(int nRow1, int nRow2, int nCol1, int nCol2,
							  COLORREF clrBkgnd,COLORREF clrText)
{
	for(int i=nRow1;i<=nRow2;i++)
	{
		LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(i);
		for(int j=nCol1;j<=nCol2;j++)
		{
			//배경색과 글자색 중 지정된 것만 변경한다.
			if(clrBkgnd!=-1) pItemData[j].clrBkgnd=clrBkgnd;
			if(clrText!=-1) pItemData[j].clrText=clrText;
		}
	}


}
BOOL CListACtrl::SetBkColor(COLORREF cr)
{
	int nRowCnt=GetItemCount();
	int nColCnt=GetHeaderCtrl()->GetItemCount();
	if(!nRowCnt || !nColCnt) return CListCtrl::SetBkColor(cr);
	for(int i=0;i<nRowCnt;i++)
	{
		LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(i);
		for(int j=0;j<nColCnt;j++)
		{
			pItemData[j].clrBkgnd=cr;
		}
	}


	return CListCtrl::SetBkColor(cr);
}
//아이템의 배경색과 글자색을 얻는다.
void CListACtrl::GetItemColor(int nRow,int nCol,COLORREF &clrBkgnd,COLORREF &clrText)
{
	if(!nRow) nRow=m_nRow;//기본 행을 현재행으로 지정
	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(nRow);

	if(!nCol) nCol=m_nCol;//기본 열을 현재열로 지정
	clrBkgnd=pItemData[nCol].clrBkgnd;
	clrText=pItemData[nCol].clrText;

}
//아이템의 체크상태를 입력한다.
void CListACtrl::SetItemCheck(int nRow,int nCol,bool nChkState,bool bUpdate)
{
	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(nRow);
	if(nChkState)
		pItemData[nCol].nChkState=1;
	else
		pItemData[nCol].nChkState=0;
	//아이템을 다시그려서 체크상태를 화면에 표시한다.
	if(bUpdate) UpdateSubItem(nRow,nCol);

	
}
//아이템의 체크상태를 얻는다.
bool CListACtrl::GetItemCheck(int nRow, int nCol)
{
	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(nRow);
	return pItemData[nCol].nChkState;
}
//배경이미지가 지정되어 있는지 판단한다.
bool CListACtrl::IsBkImage()
{
	LVBKIMAGE bki;
	if(GetBkImage(&bki) && (bki.ulFlags != LVBKIF_SOURCE_NONE))
		return true;

	return false;


}

void CListACtrl::GetItemFont(CFont &pFont,int nFontEffect,int nFontSize)
{
	
	LOGFONT *plf=(LOGFONT*)malloc(sizeof(LOGFONT));
	memset(plf,0,sizeof(LOGFONT));
	GetFont()->GetLogFont(plf);
	plf->lfWidth=nFontSize;
	switch(nFontEffect)
	{
	case 1://bold
		plf->lfWeight=FW_BOLD;
		break;
	case 2://italic
		plf->lfItalic=true;
		break;
	case 3://underline
		plf->lfUnderline=true;

	}
	pFont.CreateFontIndirect(plf);
	delete plf;
}
//사용자로 부터 글꼴 정보를 입력받아 아이템데이타에 저장한다.
void CListACtrl::SetItemFont(int nRow1,int nRow2,int nCol1,int nCol2,
							 int nFontEffect, int nFontSize)
{
	for(int i=nRow1;i<=nRow2;i++)
	{
		LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(i);
	
		for(int j=nCol1;j<=nCol2;j++)
		{
				pItemData[j].nFontEffect=nFontEffect;
				pItemData[j].nFontSize=nFontSize;
		}
	}
}
//사용자에게 아아템데이타에 저장된 글꼴 정보를 보여준다.
void CListACtrl::GetItemFont(int nRow, int nCol, int &nFontEffect, int &nFontSize)
{
	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(nRow);
	nFontEffect=pItemData[nCol].nFontEffect;
	nFontSize=pItemData[nCol].nFontSize;

}
//내용의 변화 여부를 판별한다.
bool CListACtrl::GetItemModify(int nRow)
{
	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(nRow);
	return pItemData->bUpdate;
}
//내용의 변화를 입력 받는다.
void CListACtrl::SetItemModify(int nRow, bool bUpdate)
{
	if(nRow==-1)
	{//모든 행에 업데이트
		for(int i=0;i<GetItemCount();i++)
		{//아이템의 수량 만큼 각 아이템의 0번 아이템 데이타에 편집 정보를 저장한다.
			LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(i);
			pItemData->bUpdate=bUpdate;
		}
	}
	else if(nRow>=0)
	{//한 행만 업데이트
		LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(nRow);
		pItemData->bUpdate=bUpdate;
	}

}
//아이템의 글자 정렬속성 지정
void CListACtrl::AlignItemText(int nRow1,int nRow2,int nCol1,int nCol2,UINT nAlign)
{
	for(int i=nRow1;i<=nRow2;i++)
	{
		LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(i);
	
		for(int j=nCol1;j<=nCol2;j++)
		{
				pItemData[j].nAlign=nAlign;
		}
	}

}
//실제 자료가 입력된 행의 숫자(갯수)를 반환한다.
int CListACtrl::ItemDataRowCnt()
{
	int nCols=GetHeaderCtrl()->GetItemCount();
	int nMaxRows=GetItemCount();
	if (!nMaxRows) return 0;//아이템이 없으면 0을 반환
	int i=0,nRows=-1,nCount=0;//한 행도 없으면 초기 값이 똑 같이 0이므로 -1로 정한다.
	
	while(i<nMaxRows)
	{//아이템의 갯수 만큼 루프를 돌린다.
		for(int j=0;j<nCols;j++)
		{//컬럼수 만큼 루푸를 돌린다.
			if(GetItemText(i,j)!=_T(""))
			{//아이템에 글자가 입력되었는지 확인한다.
				nRows=i;//글자가 입력되어 있는 라인을 기억한다.
				nCount=0;//빈 줄의 갯수를 0으로 초기화 한다.
				break;//for문을 끝내고 while로 돌아간다.
			}
			
		}
		//빈 줄이 50개 이상이면 루프를 끝내고 값을 반환한다.
		if(nCount>=50) return nRows+1;
		nCount++;//빈줄의 갯수를 늘린다.
		i++;
	}
	return nRows+1;//0번행 부터 시작함으로 1을 더한다.(-1인경우 만 0이된다.)
}
//아이템에 입력된 글자를 지운다
void CListACtrl::ItemClearText()
{
	//포커스를 헤더 컨트럴로 옮긴다. 오류방지
	GetHeaderCtrl()->SetFocus();
	int nRows=ItemDataRowCnt();
	int nCols=GetHeaderCtrl()->GetItemCount();
	for(int i=0;i<nRows;i++)//글자가 입력된 라인까지 루프를 돌린다.
	{
		//아이템이 선택되어 있을때 선택을 해제한다.
		if(GetItemState(i,LVIS_SELECTED)) SetItemState(i,0,LVIS_SELECTED);
		for(int j=0;j<nCols;j++)
			SetItemText(i,j,_T(""));
	}

}
//사용자로 부터 컬럼별 컨트럴 종류 정보를 입력받는다.

LRESULT CListACtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	//자식윈도우의 메시지는 WM_COMMAND
	//(LOWORD)WPARAM->자식윈도우 ID,(HIWORD)WPARAM->메시지 값
	//LPARM->자식윈도우 핸들
	switch(message)
	{
	

	case WM_NOTIFY:
		{
			NMHEADER *pnmhdr=(NMHEADER*)lParam;
			if(pnmhdr->hdr.code==HDN_BEGINTRACK)
			{			
				if(pnmhdr->pitem->cxy==0)	{return (LRESULT)1;}
				OnHdnTracking(pnmhdr);
				return (LRESULT)0;//0이면 컬럼 넓이조정가능
			}
		
		}
	}
	

	return CListCtrl::DefWindowProc(message, wParam, lParam);//나머지 메시지가 들어오면 시스템이 처리
}

//자식윈도우의 컨트럴 색상변경
HBRUSH CListACtrl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CListCtrl::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(m_nRow);
	COLORREF clrBkgnd,clrText;
	clrBkgnd=pItemData[m_nCol].clrBkgnd;
	clrText=pItemData[m_nCol].clrText;
	if(nCtlColor==CTLCOLOR_STATIC )
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(clrText);
		
		if(IsBkImage()) return CreateSolidBrush(GetBkColor());
		return CreateSolidBrush(clrBkgnd); //배경색을 그리드와 같은 배경색으로 변경
	}
	
	else if(nCtlColor==CTLCOLOR_EDIT ||nCtlColor==CTLCOLOR_LISTBOX)
		
	{		
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(clrText);
		if(!IsBkImage()) return CreateSolidBrush(clrBkgnd); //배경색을 그리드와 같은 배경색으로 변경
	}
	
		

	return hbr;
}


void CListACtrl::OnLvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(pNMLV->iItem);
	if(pItemData) 
	{
		delete [] pItemData;pItemData=NULL;
	}
}


//대화상자로 만들어 졌을때 컨트럴이 초기화 후 처음 동작함
//대화상자로 만들어 졌을때는 WM_CREATE 가 발생하지 않음

void CListACtrl::PreSubclassWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	//컨트럴의 폰트를 조사하여 사용자가 입력하였을 경우외에는
	//기본 폰트를 새로 만든다.
	//로그폰트 구조체를 동적으로 생성	
	LOGFONT *plf=(LOGFONT*)malloc(sizeof(LOGFONT));
	memset(plf,0,sizeof(LOGFONT));
	GetFont()->GetLogFont(plf);//사용자 설정 폰트를 얻는다.
	if(plf->lfHeight<=0)
	{//기본 폰트일 경우에는 폰트를 만든다.
		plf->lfHeight=14;plf->lfWidth=0;
		_tcscpy_s(plf->lfFaceName,10,_T("굴림"));
		m_font=new CFont;
		m_font->CreateFontIndirect(plf);CWnd::SetFont(m_font);
	}
	m_nItemHeight=plf->lfHeight+6;
	delete plf;
	//리스트컨트럴의 확장 속성 부여(행의 모든 아이템을 선택할수 있다,그리드를 표시한다.)
	long dwExStyle=GetExtendedStyle();
	dwExStyle |=LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	SetExtendedStyle(dwExStyle);

	//이미지리스트를 만든다.(각행의 넓이를 조정하기 위한 트릭임)
	m_pImageList=new CImageList;
	m_pImageList->Create(16,m_nItemHeight,ILC_COLORDDB|ILC_MASK,0,-1);
	CListCtrl::SetImageList(m_pImageList,LVSIL_SMALL);	
		
	CListCtrl::PreSubclassWindow();
}
//대화상자를 통하지 않은 방법으로 만들어 졌을때 동작한다. 

int CListACtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	//컨트럴의 폰트를 조사하여 사용자가 입력하였을 경우외에는
	//기본 폰트를 새로 만든다.
	LOGFONT *plf=(LOGFONT*)malloc(sizeof(LOGFONT));
	memset(plf,0,sizeof(LOGFONT));
	GetFont()->GetLogFont(plf);//사용자 설정 폰트를 얻는다.
	if(plf->lfHeight<=0)
	{//기본 폰트일 경우에는 폰트를 만든다.
		plf->lfHeight=13;plf->lfWidth=0;
		_tcscpy_s(plf->lfFaceName,10,_T("굴림"));
		m_font=new CFont;
		m_font->CreateFontIndirect(plf);CWnd::SetFont(m_font);
	}
	m_nItemHeight=plf->lfHeight+6;
	delete plf;//로그폰트 구조체 제거
	//리스트컨트럴의 확장 속성 부여(행의 모든 아이템을 선택할수 있다,그리드를 표시한다.)
	long dwExStyle=GetExtendedStyle();
	dwExStyle |=LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	SetExtendedStyle(dwExStyle);

	//이미지리스트를 만든다.(각행의 넓이를 조정하기 위한 트릭임)
	m_pImageList=new CImageList;
	m_pImageList->Create(16,m_nItemHeight,ILC_COLORDDB|ILC_MASK,0,-1);
	CListCtrl::SetImageList(m_pImageList,LVSIL_SMALL);
	
	return 0;
}




void CListACtrl::OnLvnEndScroll(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 이 기능을 사용하려면 Internet Explorer 5.5 이상이 필요합니다.
	// _WIN32_IE 기호는 0x0560보다 크거나 같아야 합니다.
	LPNMLVSCROLL pStateChanged = reinterpret_cast<LPNMLVSCROLL>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//static int nIndex=0;
	
	if(pStateChanged->dy>0) {}//수직스크롤 사용
	
	else if(pStateChanged->dx>0)//수평스크롤 사용
	{
		EnumChildWindows(m_hWnd,EnumChildProc,NULL);
		//Invalidate();
	}

	*pResult = 0;

}

void CListACtrl::UpdateSubItem(int nRow, int nCol)
{
	CRect rc;
	GetColumnItemRect(nRow,nCol,LVIR_BOUNDS,rc);
	InvalidateRect(rc,false);
}

void CListACtrl::AddRow(int nMaxRow, int nItem)
{
	LVITEM lvitem;
	int nItem2=0;
	int curitem=0;
 
	 //삽입할 행의 갯수 계산(삽입할 행번호에서 행의 수량을 더한다)
	 nItem2=nItem+nMaxRow;

	 //행 삽입 위치가 지정되면 지정된 행에 삽입한다.(기본값은 처음부터)
	 while(nItem<nItem2)
	 {
		  for (int j = 0; j <GetHeaderCtrl()->GetItemCount(); j++) 
		  {
			  lvitem.mask = (j==0)?LVIF_TEXT|LVIF_IMAGE:LVIF_TEXT;

			   lvitem.iItem = (j==0)?nItem:curitem;
			 
			   lvitem.iSubItem = j;
			   lvitem.iImage=0;
			  
			   switch(j)
			   {
			   //첫 번째 컬럼일 경우에는 InsertItem을 이용
			   case 0:
				
				     lvitem.pszText=_T("");
					 curitem=InsertItem(&lvitem);
					 break;
			   //두 번째 컬럼일 경우에는 SetItem 함수를 이용한다.
			   default:
				 
				   lvitem.pszText=_T("");
					SetItem(&lvitem);
					
					break;
			   }
			   
		  }

		nItem++;
	 }
}

BOOL CALLBACK EnumChildProc(HWND hwnd,LPARAM lParam)
{
	// Make sure the child window is visible. 
	int idChild;

	idChild=GetWindowLong(hwnd, GWL_ID); 
	if(idChild==IDC_COMBO_CHILD || idChild==IDC_EDIT_CHILD || idChild==IDC_CHECK_CHILD
		|| idChild==IDC_NUMEDIT_CHILD || idChild==IDC_DATE_CHILD || idChild==IDC_TIME_CHILD)
		
	{
		if(!lParam) ShowWindow(hwnd, SW_HIDE);//컨트럴을 모두 숨긴다.
		else //폰트 변경시 컨트럴의 폰트를 모두 변경한다.
		{
			SendMessage(hwnd,WM_SETFONT,(WPARAM)(HFONT)lParam,NULL);
		}
	}
 
    return TRUE;

}

//NM_CLICK다음에 발생한다.

void CListACtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(m_nHighLightMode) 
	{
		CListCtrl::OnLButtonDown(nFlags, point);return;
	}
	LVHITTESTINFO lvhit;
	lvhit.pt = point;
	SubItemHitTest(&lvhit);
	//아이템을 클릭했을때만 수행
	if (lvhit.flags & LVHT_ONITEM)
	{
		if(lvhit.iSubItem>0) m_bSelected=false;
		else if(lvhit.iSubItem==0) m_bSelected=true;
	
	}	

	CListCtrl::OnLButtonDown(nFlags, point);
}
//사용자가 비사용 영역을 마우스로 클릭하면 컨트럴을 숨긴다.
void CListACtrl::OnNcLButtonDown(UINT nHitTest,CPoint point)
{
	EnumChildWindows(m_hWnd,EnumChildProc,NULL);

	CListCtrl::OnNcLButtonDown(nHitTest,point);

}
//키보드 메시지 제어
BOOL CListACtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	switch(pMsg->message)//메시지 분리
	{
	case WM_KEYDOWN://키보드의 키를 누름
		{	
			switch(pMsg->wParam)
			{
			case VK_RETURN:
				{
					CWnd *pWnd=GetColumnsCtrl(m_nCol);//해당 열의 컨트럴을 얻는다.
					CWnd *pWndNew;
					//컨트럴이 없으면 끝낸다.
					if(!pWnd) 
					{
						return CListCtrl::PreTranslateMessage(pMsg);
					}
					if(pMsg->hwnd==pWnd->m_hWnd || ::GetParent(pMsg->hwnd)==pWnd->m_hWnd)
					{//메시지를 받는 컨트럴인지 확인한다. 글상자는 자신핸들, 콤보상자는 부모핸들비교	
						bool bRtn=false;//다음열의 컨트럴이 생성되었는지 여부
						if(m_nCol>=0 && m_nCol<GetColumnsCnt()-1)
						{//현재 열이 마지막이 아닐때만 수행(총 열의 수에서 1만큼 빼면 마지막열이됨)			
							m_nCol++;//다음열로 커서를 옮긴다.
							while(!bRtn)
							{//다음 열이 숨김이거나,편집불가이면 다음 열로 간다.
								pWndNew=GetColumnsCtrl(m_nCol);
								bRtn=DrawColumnsCtrl(pWndNew,m_nCol,m_nRow);//컨트럴을 그린다.
								
								//컨트럴이 없던지 열이 보이지 않으면
								if(!bRtn || !GetColumnWidth(m_nCol))
								{ m_nCol++;bRtn=false; }//결과 값을 false로 조정한다.
								if(m_nCol>GetColumnsCnt()-1) 
								{ m_nCol=0;m_nRow++;}//마지막 열이면 다음행으로 간다.
							}
							if(bRtn)
							{
								SetFocus();pWnd->ShowWindow(SW_HIDE);//이전 컨트럴을 숨긴다.
								pWndNew->ShowWindow(SW_SHOW);pWndNew->SetFocus();//다음 컨트럴을 보이고
								
							}
						}
						else if(m_nCol>=GetColumnsCnt()-1)
						{						
							SetItemState(m_nRow,0,LVIS_SELECTED|LVIS_FOCUSED);//현재 선택된 행을 취소하고
							m_nCol=0;m_nRow++;//다음줄의 첫번째 열로 간다.
							//마지막 행일 경우 첫 행으로 이동한다.
							if(m_nRow>=GetItemCount()){m_nRow=0;}
							
							//다음 행을 선택한다.
							SetItemState(m_nRow,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
							bRtn=false;
							while(!bRtn)
							{//다음 열이 숨김이거나,편집불가이면 다음 열로 간다.
								pWndNew=GetColumnsCtrl(m_nCol);
								bRtn=DrawColumnsCtrl(pWndNew,m_nCol,m_nRow);//컨트럴을 그린다.
								//컨트럴이 없던지 열이 숨김이면
								if(!bRtn || !GetColumnWidth(m_nCol)) 
								{ m_nCol++;bRtn=false; }//결과값을 false로 재지정한다.
								if(m_nCol>GetColumnsCnt()-1) 
								{ m_nCol--;break;}//끝열까지 컨트럴을 얻지 못하면 루프를 탈출한다.
							}
							if(bRtn)
							{
								SetFocus();pWnd->ShowWindow(SW_HIDE);//이전 컨트럴을 숨긴다.
								pWndNew->ShowWindow(SW_SHOW);pWndNew->SetFocus();//다음 컨트럴을 보이고
								
							}
						}
					}//메시지를 받는 컨트럴 검사 종료

				}break;
			}

		}
	}

	return CListCtrl::PreTranslateMessage(pMsg);
}
//입력 문자열에 3자리 마다 ','를 찍는다
CString CListACtrl::InputComma(const TCHAR *szNumeral)

{

	NUMBERFMT nFmt = { 0, 0, 3,_T("."),_T(","), 1/* 0:(1) 1:-1*/ };
	TCHAR temp[50]={0};
	CString strNumeral;


	GetNumberFormat(NULL, NULL,szNumeral, &nFmt,temp,50);
	strNumeral.Format(_T("%s"),temp);

	return strNumeral;
}

/*
	 Function  SpreadText     (c)2008-02-04 make by park jong churl  

	 기능 : 문자를 균등배분(양쪽정렬)한다.
	 HDC :문자열을 출력할 DC
	 CRect : 문자열이 출력될 위치 
	 pStr : 화면에 찍을 문자열
	 margin : 양쪽 옆 여백

*/
void CListACtrl::SpreadText(HDC hdc,CRect rect, const TCHAR *pStr,int margin)
{
    if(pStr==NULL) return;

	CSize size(0,0);
    int sum=0,len=0,i=0;
    TCHAR temp[2];//임시로 한글자를 저장할 장소(최하2이상 정한다 마지막은 '\0'의 자리)
    memset(temp,0,sizeof(temp));//버퍼초기화.
	//문자열의 총 너비와 문자수를 구한다.
    while(*(pStr+i)!='\0')//문서의 끝까지 읽어 들인다.
    {
       	*temp=*(pStr+i);//한글자씩 읽어서
        GetTextExtentPoint32(hdc,temp,1,&size);//글자의 너비를 계산한다.
        sum+=size.cx;//총 너비를 계산한다.
        if(sum>=rect.right-margin)break;//총 너비가 DC너비 보다 크면 루프 탈출
        len++;//문자수 증가
        i++;//포인터를 다음 문자로 이동한다.
    }
	
	int in_size=0;
	int out_point=rect.left+margin;//문자열이 처음 찍힐 위치
	int div=sum/len;//한글자의 너비를 구한다.
	rect.top+=(rect.Height()-size.cy)/2;//높이에서 문자 높이를 뺀후 2로 나눈 후 위쪽에 더한다.
	if(len>=2) len--;//마지막 문자는 계산에서 제외한다.
	in_size=(rect.Width()-margin*2-sum)/len;//문자사이에 들어갈 공백의 너비를 구한다.
	*temp=*pStr;//1문자만 임시 문자열 변수에 저장한다.
	TextOut(hdc,out_point,rect.top,temp,1);//맨 앞쪽 문자는 반복문 시작전에 찍는다.
	i=1;//포인터를 다음 문자로 이동한다.
    //문자열의 끝까지 한문자씩 화면에 찍는다..
	while(*(pStr+i)!='\0')
	{
		out_point=out_point+div+in_size;
		*temp=*(pStr+i);
		if(*(pStr+i+1)=='\0')//마지막문자는 따로 찍는다.
		{//마지막 문자가 들쑥날쑥해 지는것을 방지
			out_point=rect.right-margin-div;
		}
		TextOut(hdc,out_point,rect.top,temp,1);
		i++;//포인터를 다음문자로 이동한다.
	}
	

}