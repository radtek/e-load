// ListACtrl.cpp : ���� �����Դϴ�.
//
#include "stdafx.h"
#include "ListACtrl.h"

#define IDC_EDIT_CHILD	     100//��Ƽ�� �ĺ���
#define IDC_NUMEDIT_CHILD    101//���ڿ뿡����
#define IDC_COMBO_CHILD      102//�޺����� �ĺ���
#define IDC_CHECK_CHILD      103//üũ���� �ĺ���
#define IDC_DATE_CHILD       104//��¥��Ʈ�� �ĺ���
#define IDC_TIME_CHILD       105//�ð���Ʈ�� �ĺ���

WNDPROC g_datetimeCtrlProc;//���� �ݹ��Լ� ������

COLORREF g_clrBkgnd,g_clrText;//���� �����󺯼�
LRESULT CALLBACK DateTimeCtrlProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{

	switch(iMsg)
	{
   
	case WM_ERASEBKGND://����� �����ִ� �޽����� �޾Ƽ�
		  {
            CRect rc;
			GetClientRect(hwnd,&rc);//�Ѿ�� �ڵ��� �̿��Ͽ� �׵θ��� ���ϰ�
			HBRUSH hbrBkgnd=CreateSolidBrush(g_clrBkgnd);
            GetClientRect(hwnd,&rc);//�Ѿ�� �ڵ��� �̿��Ͽ� �׵θ��� ���ϰ�
//�����׵θ��� �Ѿ�� DC�� �̿��Ͽ� ���� ���� ���� ����� ĥ�ϰ�
            ::FillRect((HDC)wParam,rc,hbrBkgnd);
			DeleteObject(hbrBkgnd);
//  ���ϰ��� ����ڰ� ����� ĥ������ �ý����� �׸��� ����� ��
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
	//�÷����� �迭 �ʱ�ȭ
	m_pColumnsInfo=new CPtrArray;
	m_pColumnsInfo->SetSize(0);
	//���ø��� �ʱ��������
	m_clrBkHighLight=0xFF8888;m_clrTextHighLight=0xFFFFFF;
	m_nHighLightMode=0;
	
}

CListACtrl::~CListACtrl()
{
	if(m_font){m_font->DeleteObject();delete m_font;}//���� ��Ʈ ����
	if(m_pImageList!=NULL) 
	{
		m_pImageList->DeleteImageList();
		delete m_pImageList;//���� �̸̹�������Ʈ ����
	}
	if(m_pColumnsInfo!=NULL)
	{
		int i=0;//���� �÷���Ʈ�� ���� ���� ����
		while(i<m_pColumnsInfo->GetSize())
		{
			delete m_pColumnsInfo->GetAt(i++);
		}
		m_pColumnsInfo->RemoveAll();
		delete m_pColumnsInfo;
	}
}


BEGIN_MESSAGE_MAP(CListACtrl, CListCtrl)
	//������� ������ NM_CLICK�޽����� ������ ���� ������� �ȵ�
	//������(�ؽ�Ʈ) �޽���
	ON_CONTROL(EN_KILLFOCUS,IDC_EDIT_CHILD,OnEditChild_KillFocus)
	ON_CONTROL(EN_UPDATE,IDC_EDIT_CHILD,OnEditChild_Update)
	//������(����) �޽���
	ON_CONTROL(EN_KILLFOCUS,IDC_NUMEDIT_CHILD,OnNumEditChild_KillFocus)
	ON_CONTROL(EN_UPDATE,IDC_NUMEDIT_CHILD,OnNumEditChild_Update)
	//�޺����� �޽���
	ON_CONTROL(CBN_KILLFOCUS,IDC_COMBO_CHILD,OnComboChild_LostFocus)
	ON_CONTROL(CBN_EDITUPDATE,IDC_COMBO_CHILD,OnComboChild_AfterUpdate)
	ON_CONTROL(CBN_SELCHANGE,IDC_COMBO_CHILD,OnComboChild_SelChange)
	//üũ���� �޽���
	ON_CONTROL(BN_CLICKED,IDC_CHECK_CHILD,OnCheckChild_Click)
	ON_CONTROL(BN_KILLFOCUS,IDC_CHECK_CHILD,OnCheckChild_LostFocus)
	//��¥�ð� ��Ʈ�� �޽���
	ON_NOTIFY_EX_RANGE(NM_KILLFOCUS,IDC_DATE_CHILD,IDC_TIME_CHILD,OnDtnKillFocus)//��¥��Ʈ�� ��Ŀ��
	ON_NOTIFY_EX_RANGE(DTN_DATETIMECHANGE,IDC_DATE_CHILD,IDC_TIME_CHILD,OnDtnChange)//��¥��Ʈ�� ��¥����
	//����Ʈ��Ʈ���� �θ𿡰� ������ �޽���
	ON_NOTIFY_REFLECT_EX(NM_CLICK, &CListACtrl::OnClick)
	ON_NOTIFY_REFLECT(LVN_INSERTITEM, OnLvnInsertitem)//����Ʈ��Ʈ�� �����
	ON_WM_CTLCOLOR()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CListACtrl::OnNMCustomdraw)
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, &CListACtrl::OnLvnDeleteitem)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(LVN_ENDSCROLL, &CListACtrl::OnLvnEndScroll)
	ON_WM_LBUTTONDOWN()
	ON_WM_NCHITTEST()
	ON_WM_NCLBUTTONDOWN()
END_MESSAGE_MAP()



// CListACtrl �޽��� ó�����Դϴ�.

void CListACtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	
if(m_pColumnsInfo->IsEmpty()) return;//����ڰ� �÷������� �Է��ߴ��� �˻�
	LPNMLVCUSTOMDRAW pnmlvcd = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
	
	switch(pnmlvcd->nmcd.dwDrawStage)//����� �׸��� ��ȣ ����
	{
	case CDDS_PREPAINT://��Ʈ���� �׸�����		
		*pResult = CDRF_NOTIFYITEMDRAW;//��� �������� �ý����� �׸���.
		break;
	case CDDS_ITEMPREPAINT://�������� �׸�����
		{
			if(m_nHighLightMode){}//���̶���Ʈ ��尡 0�� �÷������� �ƴҶ��� ����Ѵ�.
			//0������ ���� �����ϸ� ���ø�ũ�� ǥ������ �ʴ´�.
			else if(!m_bSelected) {SetItemState((int)pnmlvcd->nmcd.dwItemSpec,
					0,LVIS_FOCUSED|LVIS_SELECTED);}	
			*pResult = CDRF_NOTIFYSUBITEMDRAW;	//��� ����������� �ý����� �׸���.
		}
		break;
	case CDDS_ITEMPREPAINT | CDDS_SUBITEM://�����۰� ����������� �׸�����(������ ���ÿ���)
		{			
			LVITEMDATA *pItemData=(LVITEMDATA*)pnmlvcd->nmcd.lItemlParam;
			int nRow=static_cast <int>(pnmlvcd->nmcd.dwItemSpec);
			int nCol=pnmlvcd->iSubItem;
			CDC* pDC = pDC->FromHandle(pnmlvcd->nmcd.hdc);//HDC->CDC
			CRect rect;
			GetColumnItemRect(nRow, nCol, LVIR_BOUNDS, rect);		
			//Ŀ���ҵ�ο� ����ü�� ���� ��� ���� ������ �����۵���Ÿ ����ü ������ ��ġ��Ų��.	
			//�� �÷��� �����Ǿ� �ִ� ��Ʈ���� ������ �ľ��Ѵ�.
			int nColumnCtrl=FindColumnCtrl(nCol);
			//0�� �÷��� �׸��弱 �ٽ� �׸��⸦ �����ϱ� ���� �Ʒ��� ��輱���� 1��ŭ �÷��� �׸���. 
			if(nCol==0) rect.bottom--;	
			switch(nColumnCtrl)//�� �÷��� �����۱���
			{
			case DFC_NOUSE:{*pResult=CDRF_NOTIFYSUBITEMDRAW;return;	}//�⺻�� �׸��� ȣ��
			case DFC_CHECK:DrawItemCheckBox(nRow,nCol,pDC,rect,pItemData);//üũ���� �׸���
				break;
			case DFC_PROGRESS:DrawItemProgress(nRow,nCol,pDC,rect,pItemData);//���ื�� �׸���
				break;
			case DFC_IMAGE://�������� ������ ����
				{
					rect.left+=DrawItemImage(nRow,nCol,pDC,rect,pItemData);
					DrawItemText(nRow,nCol,pDC,rect,pItemData);
				}
				break;
			default:DrawItemText(nRow,nCol,pDC,rect,pItemData);//���ڸ� �׸���
			}
			*pResult = CDRF_SKIPDEFAULT;//��������� �׸����ȣ ȣ��

		}	
	}
}


BOOL CListACtrl::OnClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
	NMLISTVIEW *pNotifyStruct=(NMLISTVIEW *)pNMHDR;
	int nRow=pNotifyStruct->iItem;
	int nCol=pNotifyStruct->iSubItem;
//����������ġ ���� �ݵ�� ��Ʈ���� �������� ���� ó���Ѵ�.
	m_nRow=nRow;m_nCol=nCol;

	//�÷��� ����� ������ ������,�࿭�� ���õ��� �ʾ������� �⺻�� ����
	if(m_pColumnsInfo->IsEmpty() || nRow<0 || nCol<0) {}
	//��Ʈ���� ���ϴ� ��ġ�� �׸��� ���ν��� ȣ��
	else DrawColumnsCtrl(GetColumnsCtrl(nCol),nCol,nRow);
	
	return false;//�θ𿡰Ե� �޽����� ������.
	
}
//�÷��� ������ ��Ʈ���� ���ϴ� �࿡ �׸���.
bool CListACtrl::DrawColumnsCtrl(CWnd *pWnd, int nCol, int nRow)
{
	//���� �÷��� �����Ǿ� �ִ� ��Ʈ���� �ľ��Ѵ�.
	int nColumnsCtrl=((LVCOLUMNDATA*)m_pColumnsInfo->GetAt(nCol))->nCtrlType;

	if(nColumnsCtrl==DFC_COMBO)//�޺��ڽ�
		DrawComboBoxCtrl((CComboBox*)pWnd,nRow,nCol);
	else if(nColumnsCtrl==DFC_EDIT)//������
		DrawEditCtrl((CEdit *)pWnd,nRow,nCol);
	else if(nColumnsCtrl==DFC_CHECK)//üũ����
		DrawCheckBoxCtrl((CButton*)pWnd,nRow,nCol);
	else if(nColumnsCtrl==DFC_DATE || nColumnsCtrl==DFC_TIME)//��¥��Ʈ��
		DrawDateTimeCtrl((CDateTimeCtrl*)pWnd,nRow,nCol);

	//��� ���� ���Ѵ�.
	if (pWnd && ::IsWindow(pWnd->m_hWnd)) return true;	
	return false;
}

BOOL CListACtrl::SetItemText(int nItem,int nSubItem,LPCTSTR lpszText)
{

	BOOL bRtn=CListCtrl::SetItemText(nItem,nSubItem,lpszText);
	//0�� �÷��� �����Ҷ� �������� �ٽ� �׸���.
	if(nSubItem==0)	UpdateSubItem(nItem,nSubItem);

	return bRtn;
}

// ����Ʈ ��Ʈ�� �׸���
void CListACtrl::DrawEditCtrl(CEdit *pEdit,int nRow, int nCol)
{
	m_bEdit=false;//����Ʈ ������ ����(����)�� ����
	if ((nRow < 0) || nRow >=GetItemCount())	return;
	if ((nCol < 0) || nCol >= GetHeaderCtrl()->GetItemCount())	return;
	// Make sure that nSubItem is valid
	if (GetColumnWidth(nCol) < 5) return;

	// make sure that the item is visible
	if (!EnsureVisible(nRow, TRUE)) return;
	
	CRect rect;
	GetColumnItemRect(nRow,nCol, LVIR_BOUNDS, rect);


	rect.left+=3;
	rect.top += 3;//�׸����� �� ���� �Ʒ��ʿ�
	rect.bottom -= 3;//�׸����� �� ���� ���ʿ�
	//text align change
	LV_COLUMN lvcol;
	lvcol.mask = LVCF_FMT;
	GetColumn(nCol, &lvcol);
	//get window style
	DWORD dwStyle = GetWindowLong(pEdit->m_hWnd,GWL_STYLE);
	//�÷��� ������ ���� align ������ ��´�.
	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(nRow);
	//���� align������ �������ڿ� �����Ѵ�.
	SetWindowLong(pEdit->m_hWnd,GWL_STYLE,dwStyle|pItemData[nCol].nAlign);

	CString strValue = GetItemText(nRow, nCol);
	m_strValue=strValue;
	pEdit->MoveWindow(rect);
	pEdit->ShowWindow(SW_SHOW);
	pEdit->SetFocus();
	pEdit->SetWindowText(strValue);	
	m_bEdit=true;//����Ʈ ������ ����(����)	
		
}


void CListACtrl::OnEditChild_KillFocus()
{
	
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	//��Ŀ���� ������ �����͸� �����.
	GetColumnsCtrl(m_nCol)->ShowWindow(SW_HIDE);

}

void CListACtrl::OnEditChild_Update()
{
	//�ۻ����� ���뺯��� ������ ���뵵 ���ÿ� �����ؾ��Ѵ�.
	CString str;
	CEdit *pEdit=(CEdit*)GetColumnsCtrl(m_nCol);
	pEdit->GetWindowText(str);
	SetItemText(m_nRow, m_nCol, str);
	//������ �ٲ�� ������ �˸�
	if(pEdit->GetModify())
	{
		pEdit->SetModify(false);
		LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(m_nRow);
		if(!pItemData->bUpdate) pItemData->bUpdate=true;
	}
		
}
void CListACtrl::OnNumEditChild_KillFocus()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	//��Ŀ���� ������ �����͸� �����.
	GetColumnsCtrl(m_nCol)->ShowWindow(SW_HIDE);
}
void CListACtrl::OnNumEditChild_Update()
{
	//�ۻ����� ���뺯��� ������ ���뵵 ���ÿ� �����ؾ��Ѵ�.
	CString str;
	CEdit *pEdit=(CEdit*)GetColumnsCtrl(m_nCol);
	pEdit->GetWindowText(str);
	SetItemText(m_nRow, m_nCol, str);
	//������ �ٲ�� ������ �˸�
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
	//�޺����� ��ü�� ũ�⸦ �����ϴ� ��� ^^;;
	pCombo->SetItemHeight(-1,rect.Height());//���� ���� �޺��ڽ� ������
	
	//::SendMessage(m_pCombo->m_hWnd,CB_SETITEMHEIGHT, (WPARAM) -1, (LPARAM)rect.Height());
	pCombo->SetWindowText(strValue);
	pCombo->SetFocus();
	m_bEdit=true;//����Ʈ ������ ����(����)
	
	
}

void CListACtrl::OnComboChild_LostFocus()
{
	//combobox�� lost focus�� hide�Ѵ�.
	CComboBox *pCombo=(CComboBox*)GetColumnsCtrl(m_nCol);
	pCombo->ShowWindow(SW_HIDE);
}

void CListACtrl::OnComboChild_AfterUpdate()
{
//�ۻ����� ���뺯��� ������ �ؽ�Ʈ�� ���泻���� ��ٷ� ������
//�̷��� �ؾ� ���������� ������
	CString str = _T("");
	CComboBox *pCombo=(CComboBox*)GetColumnsCtrl(m_nCol);
	pCombo->GetWindowText(str);
	SetItemText(m_nRow, m_nCol, str);
	//������ �ٲ�� ������ �˸�
	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(m_nRow);
	if(!pItemData->bUpdate) pItemData->bUpdate=true;

}

void CListACtrl::OnComboChild_SelChange()
{
	CString str;
	CComboBox *pCombo=(CComboBox*)GetColumnsCtrl(m_nCol);
	pCombo->GetLBText(pCombo->GetCurSel(),str);
	
	//�޺����ڿ��� ������ ���� �ʰ� �ܼ� ������ ���� ����ؼ� �ٽ��ѹ� �����ۿ� ���ڸ� ����.
	SetItemText(m_nRow,m_nCol,str);
	//������ �ٲ�� ������ �˸�
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
	//m_bEdit=true;//����Ʈ ������ ����(����)

}
void CListACtrl::OnCheckChild_LostFocus()
{
	//üũ���ڰ� lost focus�� hide�Ѵ�.
	CWnd *pWnd=GetColumnsCtrl(m_nCol);
	pWnd->ShowWindow(SW_HIDE);
}
void CListACtrl::OnCheckChild_Click()
{
	CButton *pButton=(CButton*)GetColumnsCtrl(m_nCol);
	//üũ���°� �ٲ�� ������ �˸�
	int nCheckState=pButton->GetCheck();
	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(m_nRow);
	pItemData[m_nCol].nChkState=nCheckState;
	UpdateSubItem(m_nRow,m_nCol);
	if(!pItemData->bUpdate) pItemData->bUpdate=true;
}
//��¥��Ʈ�� �׸���
void CListACtrl::DrawDateTimeCtrl(CDateTimeCtrl *pDateTime, int nRow, int nCol)
{
	m_bEdit=false;//����Ʈ ������ ����(����)�� ����
	if ((nRow < 0) || nRow >=GetItemCount())	return;
	if ((nCol < 0) || nCol >= GetHeaderCtrl()->GetItemCount())	return;
	// Make sure that nSubItem is valid
	if (GetColumnWidth(nCol) < 5) return;

	// make sure that the item is visible
	if (!EnsureVisible(nRow, TRUE)) return;
	GetItemColor(nRow,nCol,g_clrBkgnd,g_clrText);//������� ���ڻ����� �������� ������.
	CRect rect;
	GetColumnItemRect(nRow,nCol, LVIR_BOUNDS, rect);

	CString strValue = GetItemText(nRow, nCol);
	//��¥��Ʈ���� �ð���Ʈ���� �����Ѵ�.
	DWORD dwStyle=GetWindowLong(pDateTime->m_hWnd,GWL_STYLE);
	//��¥�ð�����ü�� �ؽ�Ʈ�� �̿��Ͽ� ��¥�� �����Ѵ�.
	SYSTEMTIME st;
	memset(&st,0,sizeof(SYSTEMTIME));
	//��Ʈ���� ��¥�� �ʱ�ȭ �Ѵ�
	GetSystemTime(&st);
	pDateTime->SetTime(&st);

	if(dwStyle & DTS_TIMEFORMAT)
	{//�ð���Ʈ�� �� ���
		st.wHour=_ttoi(strValue.Left(2));
		st.wMinute=_ttoi(strValue.Mid(4,2));
		st.wSecond=_ttoi(strValue.Right(2));
	}
	else
	{//�޷���Ʈ���ϰ��
		//�� ���ڸ� �̿��Ͽ� ��¥�� �Է��Ѵ�.
		st.wYear=_ttoi(strValue.Left(4));
		st.wMonth=_ttoi(strValue.Mid(6,2));
		st.wDay=_ttoi(strValue.Right(2));
	}
	
	m_strValue=strValue;
	//�����찡 �׷����� ���� ��¥�� ��Ʈ���� �Է��Ѵ�.
	pDateTime->SetTime(&st);
	pDateTime->MoveWindow(rect);
	pDateTime->ShowWindow(SW_SHOW);
	pDateTime->SetFocus();

}
//��¥��Ʈ�� ��Ŀ�� �޽��� ó��
BOOL CListACtrl::OnDtnKillFocus(UINT id,NMHDR *pNMHDR, LRESULT *pResult)
{
	::ShowWindow(pNMHDR->hwndFrom,SW_HIDE);
	return false;

}
//��¥��Ʈ�� ��¥���� �޽���ó��
BOOL CListACtrl::OnDtnChange(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
	//��¥�����ȣ����ü�� ��ȣ�� ����
	NMDATETIMECHANGE *pDt=(NMDATETIMECHANGE*)pNMHDR;
	CString str;

	//��Ʈ������ ���� �Ѿ� �ö��� ������ �����Ѵ�.
	if(pDt->dwFlags!=GDT_NONE)
	{
		if(id==IDC_DATE_CHILD)
		//�Ѱ��� ����Ϸ� ��¥������ ����
			str.Format(_T("%04d-%02d-%02d"),pDt->st.wYear,pDt->st.wMonth,pDt->st.wDay);
		else if(id==IDC_TIME_CHILD)
			str.Format(_T("%02d:%02d:%02d"),pDt->st.wHour,pDt->st.wMinute,pDt->st.wSecond);
	}
	//����Ʈ��Ʈ���� ���ڸ� ����
	SetItemText(m_nRow,m_nCol,str);
	//���缿�� �ڷᰡ ����Ǿ����� �����Ѵ�.
	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(m_nRow);
	if(!pItemData->bUpdate) pItemData->bUpdate=true;

	return false;

}

//��� ��Ʈ���� ��輱�� ����� ��Ʈ���� ��Ŀ���� �����Ѵ�.
void CListACtrl::OnHdnTracking(NMHEADER *pnmhdr)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if(!GetItemCount()) return;//�������� ������ �׳� ������.
	::SetFocus(pnmhdr->hdr.hwndFrom);
}

//�������� �߰��ɶ�
void CListACtrl::OnLvnInsertitem(NMHDR *pNMHDR, LRESULT *pResult)
{

	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//�����۵���Ÿ�� �޸𸮿� ž������ ����Ʈ ��Ʈ�� �� �����ۿ� �����Ѵ�.
	LVITEMDATA *pItemData=new LVITEMDATA[GetColumnsCnt()];
	for(int i=0;i<GetColumnsCnt();i++)
	{
		pItemData[i].clrBkgnd=GetBkColor();
		pItemData[i].clrText=GetTextColor();
	}
	SetItemData(pNMLV->iItem,(DWORD)pItemData);
	
}

//�÷��� ��Ʈ�� �����͸� ��ȯ�Ѵ�.
CWnd * CListACtrl::GetColumnsCtrl(int nColumn)
{
	return ((LVCOLUMNDATA*)m_pColumnsInfo->GetAt(nColumn))->pWnd;
}

//�÷��� ��Ʈ�� ������ �����.

void CListACtrl::AddColumnsCtrl(int nCol,int nCtrlType,int nDataType,int nDataFormat)

{
	CRect rect(0,0,0,0);
	//��Ʈ�� �߰��� Ȯ��� �迭�� �����Ҷ� ����� �ӽ� ������
	CEdit* pEdit=NULL;CComboBox* pCombo=NULL;CButton* pButton=NULL;
	CDateTimeCtrl* pDateTime=NULL;

	LVCOLUMNDATA *pColumnData=new LVCOLUMNDATA;//�÷��� �÷����� ����ü ����
	pColumnData->nCtrlType=nCtrlType;//�÷��� ��Ʈ�� ��������
	pColumnData->nDataType=nDataType;//�÷��� �ڷ����� ����
	pColumnData->nDataFormat=nDataFormat;//�÷��� ǥ������ ����

	//��Ʈ�� Ÿ�Ժ��� ��Ʈ�� �����͸� �����.
	switch(nCtrlType)
	{
		case DFC_EDIT ://������
			{		
				pEdit=new CEdit;
				if(nDataType==1)//���� ������
					pEdit->Create(WS_CHILD|ES_NUMBER , rect, this,IDC_NUMEDIT_CHILD);
				else if(nDataType==0 && nDataFormat==1)//��Ƽ���� ������
					pEdit->Create(WS_CHILD|ES_MULTILINE , rect, this,IDC_NUMEDIT_CHILD);
				else//���� ������
					pEdit->Create(WS_CHILD, rect, this,IDC_EDIT_CHILD);

				pEdit->SetFont(m_font);
				pColumnData->pWnd=(CWnd*)pEdit;
				m_pColumnsInfo->SetAtGrow(nCol,pColumnData);break;			
			}
		case DFC_COMBO://�޺�����
			{
				pCombo=new CComboBox;
				//����ũ���� �ؾ߸� ��Ÿ����. �ڵ��� ��Ÿ���� �ʴ´�.
				pCombo->Create(WS_CHILD | CBS_DROPDOWN|WS_VSCROLL ,rect,this,IDC_COMBO_CHILD);
				pCombo->SetFont(m_font);
				pColumnData->pWnd=(CWnd*)pCombo;
				m_pColumnsInfo->SetAtGrow(nCol,pColumnData);break;
			}
		case DFC_CHECK://üũ����
			{
				pButton=new CButton;
				pButton->Create(_T(""),WS_CHILD|BS_NOTIFY|BS_AUTOCHECKBOX,rect,this,IDC_CHECK_CHILD);
				pButton->SetFont(m_font);
				pColumnData->pWnd=(CWnd*)pButton;
				m_pColumnsInfo->SetAtGrow(nCol,pColumnData);break;
			}
		case DFC_DATE://��¥��Ʈ��
			{
				pDateTime=new CDateTimeCtrl;
				pDateTime->Create(WS_CHILD|DTS_SHORTDATEFORMAT|DTS_SHOWNONE ,rect,this,IDC_DATE_CHILD);

				g_datetimeCtrlProc=(WNDPROC)SetWindowLong(pDateTime->m_hWnd,GWL_WNDPROC,(LONG)DateTimeCtrlProc);

				pDateTime->SetFont(m_font);
				pColumnData->pWnd=(CWnd*)pDateTime;
				m_pColumnsInfo->SetAtGrow(nCol,pColumnData);break;
			}
		case DFC_TIME://�ð���Ʈ��
			{
				pDateTime=new CDateTimeCtrl;
				pDateTime->Create(WS_CHILD|DTS_TIMEFORMAT|DTS_SHOWNONE ,rect,this,IDC_TIME_CHILD);

				g_datetimeCtrlProc=(WNDPROC)SetWindowLong(pDateTime->m_hWnd,GWL_WNDPROC,(LONG)DateTimeCtrlProc);

				pDateTime->SetFont(m_font);
				pDateTime->SetFormat(_T("HH:mm:ss"));
				pColumnData->pWnd=(CWnd*)pDateTime;
				m_pColumnsInfo->SetAtGrow(nCol,pColumnData);break;
			}
		default:m_pColumnsInfo->SetAtGrow(nCol,pColumnData);//�⺻��
	}//switch ����
}

//������ �÷��� �����Ǿ� �ִ� ��Ʈ���� ������ �˾Ƴ���
int CListACtrl::FindColumnCtrl(int nCol)
{
	return ((LVCOLUMNDATA*)m_pColumnsInfo->GetAt(nCol))->nCtrlType;
}
//����Ʈ ��Ʈ���� ���� �����ϰ�, ���� ��Ʈ���� ��ġ�Ѵ�.
int CListACtrl::InsertColumn(int nCol,const LVCOLUMN *pColumn,int nCtrlType,int nDataType,int nDataFormat)
{
	//�÷��� ��Ʈ�� ���� �Լ��� ȣ���Ѵ�.
	AddColumnsCtrl(nCol,nCtrlType,nDataType,nDataFormat);

	return CListCtrl::InsertColumn(nCol,pColumn);
}
int CListACtrl::InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat,
							 int nWidth, int nSubItem, int nCtrlType,int nDataType,int nDataFormat)
{
	//�÷��� ��Ʈ�� ���� �Լ��� ȣ���Ѵ�.
	AddColumnsCtrl(nCol,nCtrlType,nDataType,nDataFormat);
	
	return CListCtrl::InsertColumn(nCol,lpszColumnHeading,nFormat,nWidth,nSubItem);
}
BOOL CListACtrl::DeleteColumn(int nCol)
{
	delete m_pColumnsInfo->GetAt(nCol);
	m_pColumnsInfo->RemoveAt(nCol);
	
	return CListCtrl::DeleteColumn(nCol);
}
//����ڱ׸��� �������ؽ�Ʈ
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
	
	rect.left+=3;rect.right-=3;//���� ��¦ �ٴ� ���� ����
	rect.top+=3;rect.bottom-=3;
	CString str,strComma;
	str = GetItemText(nRow, nCol);
	//�÷��� ������ �����´�.
	LVCOLUMNDATA *pColumnData=(LVCOLUMNDATA*)m_pColumnsInfo->GetAt(nCol);
	

	if (!str.IsEmpty())
	{		
		if(pColumnData->nDataType==1 && !pColumnData->nDataFormat)
		{//�ڷ������� �����̰�, ������ 1�� �ƴϸ� 3�ڸ� ���� �޸��� ��´�.
			strComma=str;str=InputComma(strComma);
		}
		//����� ���� ����ü�� �����Ǿ��°� �˻��Ѵ�.
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
			else //��Ƽ������ ���������� �ȵȴ�. �ݵ�� DT_SINGLELINE�� �����ؾ� �Ѵ�.
			{//��Ƽ���� ����
				if(pColumnData->nDataType==0 && pColumnData->nDataFormat==1)
					pDC->DrawText(str, &rect,pItemData[nCol].nAlign|DT_WORDBREAK);//��Ƽ����
				else pDC->DrawText(str, &rect,pItemData[nCol].nAlign|DT_VCENTER|DT_SINGLELINE);//�̱۶���
			}
			pDC->SelectObject(prevFont);
			newFont.DeleteObject();
		}
		else
		{
			if(pItemData[nCol].nAlign==0x4)
				SpreadText(pDC->m_hDC,rect,str,3);
			else
			{//��Ƽ���� ����
				if(pColumnData->nDataType==0 && pColumnData->nDataFormat==1)
					pDC->DrawText(str, &rect,pItemData[nCol].nAlign|DT_WORDBREAK);//��Ƽ����
				else pDC->DrawText(str, &rect,pItemData[nCol].nAlign|DT_VCENTER|DT_SINGLELINE);//�̱۶���
			}
		}

	}
}

//�����ۿ� üũ���ڸ� �׸���.
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
	//�簢���� ������ �߾ӿ��� ���Ʒ��� 6��ŭ ����
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
	// üũ���� ������ �������� ĥ�Ѵ�.	
	pDC->FillSolidRect(&chkboxrect,0xFFFFFF);

	// ��ü �簢�׵θ�
	pDC->Draw3dRect(&chkboxrect,0x4B4B4B,0x989898);
	//üũ���� ������ üũ ���̸� �׸��� ��ƾ 
	if(pItemData[nCol].nChkState)
	{
		CPen *prevPen = NULL;
		COLORREF clrCheck=0x888800;

		CPen grayPen(PS_SOLID, 1,clrCheck);
		prevPen = pDC->SelectObject(&grayPen);
		
		// draw the checkmark��������
		int x = chkboxrect.right-4;
		ASSERT(x < chkboxrect.right);
		int y = chkboxrect.top + 2;
		//x���� �簢���� ������ 3/5���� ������ ���� �Ʒ��� �ߴ´�.
		while (x>=chkboxrect.right-4-(chkboxrect.Width()*3/5) &&
			y<=chkboxrect.bottom-2-(chkboxrect.Height()/2))//
		{
			pDC->MoveTo(x, y);
			pDC->LineTo(x, y+3);//y�� ������ �� ����
			x--;
			y++;
		}
		//x���� �簢���� ���� �𼭸� ���� ������ ���� �̾ ���� �ߴ´�.
		while (x>=chkboxrect.left+3)//i�� ���� �� ����
		{
			pDC->MoveTo(x, y);
			pDC->LineTo(x, y+3);//y���� ���ʼ��� ����
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

//�����ۿ� ������ �̹����� �׸���

int CListACtrl::DrawItemImage(int nRow, int nCol, CDC *pDC, CRect &rect,
									 LVITEMDATA *pItemData)
{
	if (rect.IsRectEmpty())
	{
		return 0;
	}
	//����̹����� �����Ǿ� ������ ������ ����� ĥ���� �ʴ´�.
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
//���ื�븦 �����ۿ� �׸���.
void CListACtrl::DrawItemProgress(int nRow, int nCol, CDC *pDC, CRect &rect,
								  LVITEMDATA *pItemData)
{
	UNUSED_ALWAYS(nRow);

	if (rect.IsRectEmpty())
	{
		return;
	}
	//������ ����
	COLORREF clrBkgnd,clrText;
	CheckItemColor(nRow,nCol,clrBkgnd,clrText);
	if(!IsBkImage()) pDC->FillSolidRect(rect,clrBkgnd);
	rect.left += 1;		// leave margin in case row is highlighted
	int nPercent=pItemData[nCol].nProgressPercent;
	// ���ื���� ������ 
	pDC->FillSolidRect(rect,clrBkgnd);

	if (nPercent >= 0)
	{
		// draw progress bar and text
		//���ื�� �׵θ�
		CRect LeftRect, RightRect;
		//���ื�� ����
		COLORREF clrProgBkgnd;
		LeftRect = rect;
		LeftRect.left += 1;
		RightRect = LeftRect;
		if(nPercent>100) nPercent=100;
		int w = (LeftRect.Width() * nPercent) / 100;
		LeftRect.right = LeftRect.left + w - 1;
		RightRect.left = LeftRect.right;
		//�������� ���õǾ� ������ ������ ���ø���� ��ġ��Ų��.
		if(GetItemState(nRow,LVIS_SELECTED)==LVIS_SELECTED) clrProgBkgnd=clrBkgnd;
		else clrProgBkgnd=0x0088FF;
		pDC->FillSolidRect(LeftRect,clrProgBkgnd);

			
		CString str;
		CFont *PrevFont=NULL;
		if(!nPercent)
			str=_T("���");
		else if(nPercent<100)
			str.Format(_T("%d%%"), nPercent);
		else if(nPercent>=100)
			str=_T("�Ϸ�");
			
		//���ื���� ���� ǥ��	
		pDC->SetBkMode(TRANSPARENT);

		CRect TextRect;
		TextRect = rect;
		TextRect.DeflateRect(1, 1);

		CRgn rgn;
		rgn.CreateRectRgn(LeftRect.left,LeftRect.top, LeftRect.right, 
				LeftRect.bottom);
		pDC->SelectClipRgn(&rgn);
		//���� ������� ���� ���� ����
		pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));//crBkgnd);
		pDC->DrawText(str, &TextRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		rgn.DeleteObject();
		
		rgn.CreateRectRgn(RightRect.left, RightRect.top, RightRect.right, 
				RightRect.bottom);
		pDC->SelectClipRgn(&rgn);
		//�̹� ����� ���� ���� ����
		pDC->SetTextColor(RGB(217,217,217));
		pDC->DrawText(str, &TextRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		rgn.DeleteObject();
		pDC->SelectClipRgn(NULL);
		
	}
	
}
//���ø����� ���� ���ڻ�����
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
	CWnd::SetFont(pFont,bRedraw);//���� ���� �����Լ� ȣ��
	if(m_font){m_font->DeleteObject();m_font=NULL;}//������ �۲��� ������ �����Ѵ�.
	//�Էµ� ���ο� ��Ʈ�� ������Ʈ�� �����Ѵ�.
	LOGFONT lf;
	memset(&lf,0,sizeof(LOGFONT));
	pFont->GetLogFont(&lf);
	m_font=new CFont;
	m_font->CreateFontIndirect(&lf);
	//��Ʈ����� ��Ʈ�� ��Ʈ�� �����Ѵ�.
	EnumChildWindows(m_hWnd,EnumChildProc,(LPARAM)m_font->operator HFONT());

	SetItemHeight(m_nItemHeight);
}
//������ ���̿� ���� �̹�������Ʈ�� �����̹��� ����Ʈ�� ġȯ�Ѵ�.
CImageList *CListACtrl::SetImageList(CImageList *pImageList,int nImageListType)
{
	//�̹���Ÿ���� LVSIL_SMALL�� �ƴϸ� ������ �Լ��� ȣ���Ѵ�.
	if(nImageListType!=LVSIL_SMALL)
		return CListCtrl::SetImageList(pImageList,nImageListType);
	//�̹���Ÿ���� LVSIL_SMALL�̸� �Ѿ�� �̹��� ����Ʈ�� �����̹��� ����Ʈ�� ġȯ�Ѵ�.
	CImageList *pImageListDst;
	pImageListDst=new CImageList;

	IMAGEINFO imageinfo;
	memset(&imageinfo,0,sizeof(imageinfo));
	CRect rcSrc,rcDst;
	CDC memDC;
	CBitmap bm, *pbmPrev;
	pImageList->GetImageInfo(0,&imageinfo);//�����̹�������Ʈ �̹��� ���� ȹ��
	rcSrc=imageinfo.rcImage;
	CPoint drawpoint;
	rcDst=rcSrc;//�����̹��� ũ�⸦ ����̹��� ũ��� �Ѱ��ش�.
	//����̹��� ũ�⸦ �������� ���̿� ���Ͽ� �������Ѵ�.
	if(m_nItemHeight>rcSrc.Height()) rcDst.bottom=m_nItemHeight-rcSrc.Height()+rcSrc.bottom;
	pImageListDst->Create(rcDst.Width(),rcDst.Height(),ILC_COLORDDB|ILC_MASK,0,-1);
	//����̹����� �����̹����� �׸� ��ġ��(�����߾�) ���Ѵ�.
	drawpoint.y=(rcDst.Height()-rcSrc.Height())/2;drawpoint.x=0;
	//�޸� ��ȭ���� ������.
	memDC.CreateCompatibleDC(GetDC());
   	
	for(int i=0;i<pImageList->GetImageCount();i++)
	{
		//��� ��Ʈ���� �����.
		bm.CreateCompatibleBitmap(GetDC(),rcDst.Width(),rcDst.Height());
		pbmPrev=memDC.SelectObject(&bm);//��ȭ���� ��Ʈ���� �����Ѵ�.
		memDC.FillSolidRect(rcDst,GetBkColor());//��ȭ���� �̹��� ����� ĥ�Ѵ�.
		pImageList->DrawIndirect(&memDC,i,drawpoint,
			CSize(rcSrc.Width(),rcSrc.Height()),CPoint(0,0));//������� ���� �̹����� �׸���.
		memDC.SelectObject(pbmPrev);//�޸� ��ȭ���� ����ġ ��Ų��.
		pImageListDst->Add(&bm,GetBkColor());//�̹��� ����Ʈ�� �׸� �׸��� �ִ´�.
		bm.DeleteObject();
	}
	if(m_pImageList){m_pImageList->DeleteImageList();m_pImageList=NULL;}
	m_pImageList=pImageListDst;
	return CListCtrl::SetImageList(pImageListDst,nImageListType);

}
//���÷��� �������� �׵θ� ũ�⸦ ��´�.
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



//���ื���� ��ġ�� �����Ѵ�.
void CListACtrl::SetProgressPos(int nRow, int nCol, int nPos)
{
	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(nRow);
	pItemData[nCol].nProgressPercent=nPos;
}
//���ื���� ��ġ�� ��´�.
int CListACtrl::GetProgressPos(int nRow, int nCol)
{
	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(nRow);
	return pItemData[nCol].nProgressPercent;

}
//�������� ���� ����
void CListACtrl::SetItemHeight(long nHeight)
{	
	//�α���Ʈ ����ü�� �������� ����	
	LOGFONT *plf=(LOGFONT*)malloc(sizeof(LOGFONT));
	memset(plf,0,sizeof(LOGFONT));
	GetFont()->GetLogFont(plf);//����� ���� ��Ʈ�� ��´�.
	if(plf->lfHeight<=0)
	{//�⺻ ��Ʈ�� ��쿡�� ��Ʈ�� �����.
		plf->lfHeight=nHeight+3;plf->lfWidth=0;
		_tcscpy_s(plf->lfFaceName,10,_T("����"));
		m_font=new CFont;
		m_font->CreateFontIndirect(plf);CWnd::SetFont(m_font);
	}
	//�������� �۲� ũ�⺸�� ������ �⺻ ������ �Ѵ�.
	if(nHeight<=plf->lfHeight+2) nHeight=plf->lfHeight+6;
	delete plf;
	m_nItemHeight=nHeight;
	//�̹�������Ʈ�� �����.(������ ���̸� �����ϱ� ���� Ʈ����)
	if(GetImageList(LVSIL_SMALL))//�̹��� ����Ʈ�� ������
	{
		SetImageList(GetImageList(LVSIL_SMALL),LVSIL_SMALL);
	}
	else//�̹��� ����Ʈ�� ������
	{
		m_pImageList=new CImageList;
		m_pImageList->Create(16,nHeight,ILC_COLORDDB|ILC_MASK,0,-1);
		CListCtrl::SetImageList(m_pImageList,LVSIL_SMALL);
	}
}
//�������� ���õǾ��°��� Ȯ���Ͽ� ������ ���Ѵ�.
void CListACtrl::CheckItemColor(int nRow,int nCol,COLORREF &clrBkgnd,COLORREF &clrText)
{
	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(nRow);
	if(pItemData)
	{
		clrBkgnd=pItemData[nCol].clrBkgnd;
		clrText=pItemData[nCol].clrText;

		if (GetItemState(nRow, LVIS_SELECTED))
		{
			if(m_nHighLightMode==2)//���ø�尡 ���ڻ��� �ٲܰ��
				clrText=0xFFFFFF-m_clrBkHighLight;
			else
			{
				clrBkgnd=m_clrBkHighLight;clrText=m_clrTextHighLight;
			}
		}
	}
}

//�� �������� ������� ���ڻ����� �����Ѵ�.
void CListACtrl::SetItemColor(int nRow1, int nRow2, int nCol1, int nCol2,
							  COLORREF clrBkgnd,COLORREF clrText)
{
	for(int i=nRow1;i<=nRow2;i++)
	{
		LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(i);
		for(int j=nCol1;j<=nCol2;j++)
		{
			//������ ���ڻ� �� ������ �͸� �����Ѵ�.
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
//�������� ������ ���ڻ��� ��´�.
void CListACtrl::GetItemColor(int nRow,int nCol,COLORREF &clrBkgnd,COLORREF &clrText)
{
	if(!nRow) nRow=m_nRow;//�⺻ ���� ���������� ����
	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(nRow);

	if(!nCol) nCol=m_nCol;//�⺻ ���� ���翭�� ����
	clrBkgnd=pItemData[nCol].clrBkgnd;
	clrText=pItemData[nCol].clrText;

}
//�������� üũ���¸� �Է��Ѵ�.
void CListACtrl::SetItemCheck(int nRow,int nCol,bool nChkState,bool bUpdate)
{
	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(nRow);
	if(nChkState)
		pItemData[nCol].nChkState=1;
	else
		pItemData[nCol].nChkState=0;
	//�������� �ٽñ׷��� üũ���¸� ȭ�鿡 ǥ���Ѵ�.
	if(bUpdate) UpdateSubItem(nRow,nCol);

	
}
//�������� üũ���¸� ��´�.
bool CListACtrl::GetItemCheck(int nRow, int nCol)
{
	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(nRow);
	return pItemData[nCol].nChkState;
}
//����̹����� �����Ǿ� �ִ��� �Ǵ��Ѵ�.
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
//����ڷ� ���� �۲� ������ �Է¹޾� �����۵���Ÿ�� �����Ѵ�.
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
//����ڿ��� �ƾ��۵���Ÿ�� ����� �۲� ������ �����ش�.
void CListACtrl::GetItemFont(int nRow, int nCol, int &nFontEffect, int &nFontSize)
{
	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(nRow);
	nFontEffect=pItemData[nCol].nFontEffect;
	nFontSize=pItemData[nCol].nFontSize;

}
//������ ��ȭ ���θ� �Ǻ��Ѵ�.
bool CListACtrl::GetItemModify(int nRow)
{
	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(nRow);
	return pItemData->bUpdate;
}
//������ ��ȭ�� �Է� �޴´�.
void CListACtrl::SetItemModify(int nRow, bool bUpdate)
{
	if(nRow==-1)
	{//��� �࿡ ������Ʈ
		for(int i=0;i<GetItemCount();i++)
		{//�������� ���� ��ŭ �� �������� 0�� ������ ����Ÿ�� ���� ������ �����Ѵ�.
			LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(i);
			pItemData->bUpdate=bUpdate;
		}
	}
	else if(nRow>=0)
	{//�� �ุ ������Ʈ
		LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(nRow);
		pItemData->bUpdate=bUpdate;
	}

}
//�������� ���� ���ļӼ� ����
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
//���� �ڷᰡ �Էµ� ���� ����(����)�� ��ȯ�Ѵ�.
int CListACtrl::ItemDataRowCnt()
{
	int nCols=GetHeaderCtrl()->GetItemCount();
	int nMaxRows=GetItemCount();
	if (!nMaxRows) return 0;//�������� ������ 0�� ��ȯ
	int i=0,nRows=-1,nCount=0;//�� �൵ ������ �ʱ� ���� �� ���� 0�̹Ƿ� -1�� ���Ѵ�.
	
	while(i<nMaxRows)
	{//�������� ���� ��ŭ ������ ������.
		for(int j=0;j<nCols;j++)
		{//�÷��� ��ŭ ��Ǫ�� ������.
			if(GetItemText(i,j)!=_T(""))
			{//�����ۿ� ���ڰ� �ԷµǾ����� Ȯ���Ѵ�.
				nRows=i;//���ڰ� �ԷµǾ� �ִ� ������ ����Ѵ�.
				nCount=0;//�� ���� ������ 0���� �ʱ�ȭ �Ѵ�.
				break;//for���� ������ while�� ���ư���.
			}
			
		}
		//�� ���� 50�� �̻��̸� ������ ������ ���� ��ȯ�Ѵ�.
		if(nCount>=50) return nRows+1;
		nCount++;//������ ������ �ø���.
		i++;
	}
	return nRows+1;//0���� ���� ���������� 1�� ���Ѵ�.(-1�ΰ�� �� 0�̵ȴ�.)
}
//�����ۿ� �Էµ� ���ڸ� �����
void CListACtrl::ItemClearText()
{
	//��Ŀ���� ��� ��Ʈ���� �ű��. ��������
	GetHeaderCtrl()->SetFocus();
	int nRows=ItemDataRowCnt();
	int nCols=GetHeaderCtrl()->GetItemCount();
	for(int i=0;i<nRows;i++)//���ڰ� �Էµ� ���α��� ������ ������.
	{
		//�������� ���õǾ� ������ ������ �����Ѵ�.
		if(GetItemState(i,LVIS_SELECTED)) SetItemState(i,0,LVIS_SELECTED);
		for(int j=0;j<nCols;j++)
			SetItemText(i,j,_T(""));
	}

}
//����ڷ� ���� �÷��� ��Ʈ�� ���� ������ �Է¹޴´�.

LRESULT CListACtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	//�ڽ��������� �޽����� WM_COMMAND
	//(LOWORD)WPARAM->�ڽ������� ID,(HIWORD)WPARAM->�޽��� ��
	//LPARM->�ڽ������� �ڵ�
	switch(message)
	{
	

	case WM_NOTIFY:
		{
			NMHEADER *pnmhdr=(NMHEADER*)lParam;
			if(pnmhdr->hdr.code==HDN_BEGINTRACK)
			{			
				if(pnmhdr->pitem->cxy==0)	{return (LRESULT)1;}
				OnHdnTracking(pnmhdr);
				return (LRESULT)0;//0�̸� �÷� ������������
			}
		
		}
	}
	

	return CListCtrl::DefWindowProc(message, wParam, lParam);//������ �޽����� ������ �ý����� ó��
}

//�ڽ��������� ��Ʈ�� ���󺯰�
HBRUSH CListACtrl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CListCtrl::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  ���⼭ DC�� Ư���� �����մϴ�.

	// TODO:  �⺻���� �������� ������ �ٸ� �귯�ø� ��ȯ�մϴ�.
	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(m_nRow);
	COLORREF clrBkgnd,clrText;
	clrBkgnd=pItemData[m_nCol].clrBkgnd;
	clrText=pItemData[m_nCol].clrText;
	if(nCtlColor==CTLCOLOR_STATIC )
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(clrText);
		
		if(IsBkImage()) return CreateSolidBrush(GetBkColor());
		return CreateSolidBrush(clrBkgnd); //������ �׸���� ���� �������� ����
	}
	
	else if(nCtlColor==CTLCOLOR_EDIT ||nCtlColor==CTLCOLOR_LISTBOX)
		
	{		
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(clrText);
		if(!IsBkImage()) return CreateSolidBrush(clrBkgnd); //������ �׸���� ���� �������� ����
	}
	
		

	return hbr;
}


void CListACtrl::OnLvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	LVITEMDATA *pItemData=(LVITEMDATA*)GetItemData(pNMLV->iItem);
	if(pItemData) 
	{
		delete [] pItemData;pItemData=NULL;
	}
}


//��ȭ���ڷ� ����� ������ ��Ʈ���� �ʱ�ȭ �� ó�� ������
//��ȭ���ڷ� ����� �������� WM_CREATE �� �߻����� ����

void CListACtrl::PreSubclassWindow()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	//��Ʈ���� ��Ʈ�� �����Ͽ� ����ڰ� �Է��Ͽ��� ���ܿ���
	//�⺻ ��Ʈ�� ���� �����.
	//�α���Ʈ ����ü�� �������� ����	
	LOGFONT *plf=(LOGFONT*)malloc(sizeof(LOGFONT));
	memset(plf,0,sizeof(LOGFONT));
	GetFont()->GetLogFont(plf);//����� ���� ��Ʈ�� ��´�.
	if(plf->lfHeight<=0)
	{//�⺻ ��Ʈ�� ��쿡�� ��Ʈ�� �����.
		plf->lfHeight=14;plf->lfWidth=0;
		_tcscpy_s(plf->lfFaceName,10,_T("����"));
		m_font=new CFont;
		m_font->CreateFontIndirect(plf);CWnd::SetFont(m_font);
	}
	m_nItemHeight=plf->lfHeight+6;
	delete plf;
	//����Ʈ��Ʈ���� Ȯ�� �Ӽ� �ο�(���� ��� �������� �����Ҽ� �ִ�,�׸��带 ǥ���Ѵ�.)
	long dwExStyle=GetExtendedStyle();
	dwExStyle |=LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	SetExtendedStyle(dwExStyle);

	//�̹�������Ʈ�� �����.(������ ���̸� �����ϱ� ���� Ʈ����)
	m_pImageList=new CImageList;
	m_pImageList->Create(16,m_nItemHeight,ILC_COLORDDB|ILC_MASK,0,-1);
	CListCtrl::SetImageList(m_pImageList,LVSIL_SMALL);	
		
	CListCtrl::PreSubclassWindow();
}
//��ȭ���ڸ� ������ ���� ������� ����� ������ �����Ѵ�. 

int CListACtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.
	//��Ʈ���� ��Ʈ�� �����Ͽ� ����ڰ� �Է��Ͽ��� ���ܿ���
	//�⺻ ��Ʈ�� ���� �����.
	LOGFONT *plf=(LOGFONT*)malloc(sizeof(LOGFONT));
	memset(plf,0,sizeof(LOGFONT));
	GetFont()->GetLogFont(plf);//����� ���� ��Ʈ�� ��´�.
	if(plf->lfHeight<=0)
	{//�⺻ ��Ʈ�� ��쿡�� ��Ʈ�� �����.
		plf->lfHeight=13;plf->lfWidth=0;
		_tcscpy_s(plf->lfFaceName,10,_T("����"));
		m_font=new CFont;
		m_font->CreateFontIndirect(plf);CWnd::SetFont(m_font);
	}
	m_nItemHeight=plf->lfHeight+6;
	delete plf;//�α���Ʈ ����ü ����
	//����Ʈ��Ʈ���� Ȯ�� �Ӽ� �ο�(���� ��� �������� �����Ҽ� �ִ�,�׸��带 ǥ���Ѵ�.)
	long dwExStyle=GetExtendedStyle();
	dwExStyle |=LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	SetExtendedStyle(dwExStyle);

	//�̹�������Ʈ�� �����.(������ ���̸� �����ϱ� ���� Ʈ����)
	m_pImageList=new CImageList;
	m_pImageList->Create(16,m_nItemHeight,ILC_COLORDDB|ILC_MASK,0,-1);
	CListCtrl::SetImageList(m_pImageList,LVSIL_SMALL);
	
	return 0;
}




void CListACtrl::OnLvnEndScroll(NMHDR *pNMHDR, LRESULT *pResult)
{
	// �� ����� ����Ϸ��� Internet Explorer 5.5 �̻��� �ʿ��մϴ�.
	// _WIN32_IE ��ȣ�� 0x0560���� ũ�ų� ���ƾ� �մϴ�.
	LPNMLVSCROLL pStateChanged = reinterpret_cast<LPNMLVSCROLL>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//static int nIndex=0;
	
	if(pStateChanged->dy>0) {}//������ũ�� ���
	
	else if(pStateChanged->dx>0)//����ũ�� ���
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
 
	 //������ ���� ���� ���(������ ���ȣ���� ���� ������ ���Ѵ�)
	 nItem2=nItem+nMaxRow;

	 //�� ���� ��ġ�� �����Ǹ� ������ �࿡ �����Ѵ�.(�⺻���� ó������)
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
			   //ù ��° �÷��� ��쿡�� InsertItem�� �̿�
			   case 0:
				
				     lvitem.pszText=_T("");
					 curitem=InsertItem(&lvitem);
					 break;
			   //�� ��° �÷��� ��쿡�� SetItem �Լ��� �̿��Ѵ�.
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
		if(!lParam) ShowWindow(hwnd, SW_HIDE);//��Ʈ���� ��� �����.
		else //��Ʈ ����� ��Ʈ���� ��Ʈ�� ��� �����Ѵ�.
		{
			SendMessage(hwnd,WM_SETFONT,(WPARAM)(HFONT)lParam,NULL);
		}
	}
 
    return TRUE;

}

//NM_CLICK������ �߻��Ѵ�.

void CListACtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if(m_nHighLightMode) 
	{
		CListCtrl::OnLButtonDown(nFlags, point);return;
	}
	LVHITTESTINFO lvhit;
	lvhit.pt = point;
	SubItemHitTest(&lvhit);
	//�������� Ŭ���������� ����
	if (lvhit.flags & LVHT_ONITEM)
	{
		if(lvhit.iSubItem>0) m_bSelected=false;
		else if(lvhit.iSubItem==0) m_bSelected=true;
	
	}	

	CListCtrl::OnLButtonDown(nFlags, point);
}
//����ڰ� ���� ������ ���콺�� Ŭ���ϸ� ��Ʈ���� �����.
void CListACtrl::OnNcLButtonDown(UINT nHitTest,CPoint point)
{
	EnumChildWindows(m_hWnd,EnumChildProc,NULL);

	CListCtrl::OnNcLButtonDown(nHitTest,point);

}
//Ű���� �޽��� ����
BOOL CListACtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	switch(pMsg->message)//�޽��� �и�
	{
	case WM_KEYDOWN://Ű������ Ű�� ����
		{	
			switch(pMsg->wParam)
			{
			case VK_RETURN:
				{
					CWnd *pWnd=GetColumnsCtrl(m_nCol);//�ش� ���� ��Ʈ���� ��´�.
					CWnd *pWndNew;
					//��Ʈ���� ������ ������.
					if(!pWnd) 
					{
						return CListCtrl::PreTranslateMessage(pMsg);
					}
					if(pMsg->hwnd==pWnd->m_hWnd || ::GetParent(pMsg->hwnd)==pWnd->m_hWnd)
					{//�޽����� �޴� ��Ʈ������ Ȯ���Ѵ�. �ۻ��ڴ� �ڽ��ڵ�, �޺����ڴ� �θ��ڵ��	
						bool bRtn=false;//�������� ��Ʈ���� �����Ǿ����� ����
						if(m_nCol>=0 && m_nCol<GetColumnsCnt()-1)
						{//���� ���� �������� �ƴҶ��� ����(�� ���� ������ 1��ŭ ���� ���������̵�)			
							m_nCol++;//�������� Ŀ���� �ű��.
							while(!bRtn)
							{//���� ���� �����̰ų�,�����Ұ��̸� ���� ���� ����.
								pWndNew=GetColumnsCtrl(m_nCol);
								bRtn=DrawColumnsCtrl(pWndNew,m_nCol,m_nRow);//��Ʈ���� �׸���.
								
								//��Ʈ���� ������ ���� ������ ������
								if(!bRtn || !GetColumnWidth(m_nCol))
								{ m_nCol++;bRtn=false; }//��� ���� false�� �����Ѵ�.
								if(m_nCol>GetColumnsCnt()-1) 
								{ m_nCol=0;m_nRow++;}//������ ���̸� ���������� ����.
							}
							if(bRtn)
							{
								SetFocus();pWnd->ShowWindow(SW_HIDE);//���� ��Ʈ���� �����.
								pWndNew->ShowWindow(SW_SHOW);pWndNew->SetFocus();//���� ��Ʈ���� ���̰�
								
							}
						}
						else if(m_nCol>=GetColumnsCnt()-1)
						{						
							SetItemState(m_nRow,0,LVIS_SELECTED|LVIS_FOCUSED);//���� ���õ� ���� ����ϰ�
							m_nCol=0;m_nRow++;//�������� ù��° ���� ����.
							//������ ���� ��� ù ������ �̵��Ѵ�.
							if(m_nRow>=GetItemCount()){m_nRow=0;}
							
							//���� ���� �����Ѵ�.
							SetItemState(m_nRow,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
							bRtn=false;
							while(!bRtn)
							{//���� ���� �����̰ų�,�����Ұ��̸� ���� ���� ����.
								pWndNew=GetColumnsCtrl(m_nCol);
								bRtn=DrawColumnsCtrl(pWndNew,m_nCol,m_nRow);//��Ʈ���� �׸���.
								//��Ʈ���� ������ ���� �����̸�
								if(!bRtn || !GetColumnWidth(m_nCol)) 
								{ m_nCol++;bRtn=false; }//������� false�� �������Ѵ�.
								if(m_nCol>GetColumnsCnt()-1) 
								{ m_nCol--;break;}//�������� ��Ʈ���� ���� ���ϸ� ������ Ż���Ѵ�.
							}
							if(bRtn)
							{
								SetFocus();pWnd->ShowWindow(SW_HIDE);//���� ��Ʈ���� �����.
								pWndNew->ShowWindow(SW_SHOW);pWndNew->SetFocus();//���� ��Ʈ���� ���̰�
								
							}
						}
					}//�޽����� �޴� ��Ʈ�� �˻� ����

				}break;
			}

		}
	}

	return CListCtrl::PreTranslateMessage(pMsg);
}
//�Է� ���ڿ��� 3�ڸ� ���� ','�� ��´�
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

	 ��� : ���ڸ� �յ���(��������)�Ѵ�.
	 HDC :���ڿ��� ����� DC
	 CRect : ���ڿ��� ��µ� ��ġ 
	 pStr : ȭ�鿡 ���� ���ڿ�
	 margin : ���� �� ����

*/
void CListACtrl::SpreadText(HDC hdc,CRect rect, const TCHAR *pStr,int margin)
{
    if(pStr==NULL) return;

	CSize size(0,0);
    int sum=0,len=0,i=0;
    TCHAR temp[2];//�ӽ÷� �ѱ��ڸ� ������ ���(����2�̻� ���Ѵ� �������� '\0'�� �ڸ�)
    memset(temp,0,sizeof(temp));//�����ʱ�ȭ.
	//���ڿ��� �� �ʺ�� ���ڼ��� ���Ѵ�.
    while(*(pStr+i)!='\0')//������ ������ �о� ���δ�.
    {
       	*temp=*(pStr+i);//�ѱ��ھ� �о
        GetTextExtentPoint32(hdc,temp,1,&size);//������ �ʺ� ����Ѵ�.
        sum+=size.cx;//�� �ʺ� ����Ѵ�.
        if(sum>=rect.right-margin)break;//�� �ʺ� DC�ʺ� ���� ũ�� ���� Ż��
        len++;//���ڼ� ����
        i++;//�����͸� ���� ���ڷ� �̵��Ѵ�.
    }
	
	int in_size=0;
	int out_point=rect.left+margin;//���ڿ��� ó�� ���� ��ġ
	int div=sum/len;//�ѱ����� �ʺ� ���Ѵ�.
	rect.top+=(rect.Height()-size.cy)/2;//���̿��� ���� ���̸� ���� 2�� ���� �� ���ʿ� ���Ѵ�.
	if(len>=2) len--;//������ ���ڴ� ��꿡�� �����Ѵ�.
	in_size=(rect.Width()-margin*2-sum)/len;//���ڻ��̿� �� ������ �ʺ� ���Ѵ�.
	*temp=*pStr;//1���ڸ� �ӽ� ���ڿ� ������ �����Ѵ�.
	TextOut(hdc,out_point,rect.top,temp,1);//�� ���� ���ڴ� �ݺ��� �������� ��´�.
	i=1;//�����͸� ���� ���ڷ� �̵��Ѵ�.
    //���ڿ��� ������ �ѹ��ھ� ȭ�鿡 ��´�..
	while(*(pStr+i)!='\0')
	{
		out_point=out_point+div+in_size;
		*temp=*(pStr+i);
		if(*(pStr+i+1)=='\0')//���������ڴ� ���� ��´�.
		{//������ ���ڰ� �龦������ ���°��� ����
			out_point=rect.right-margin-div;
		}
		TextOut(hdc,out_point,rect.top,temp,1);
		i++;//�����͸� �������ڷ� �̵��Ѵ�.
	}
	

}