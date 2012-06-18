#pragma once
#include "stdafx.h"


#define DT_RATE          0x4//균등배분 정렬
//컨트럴 구분자
#define DFC_NOUSE       -1//원래시스템 그리기 사용함(0번컬럼을 원래대로 사용할때 이용하면됨)
#define DFC_STATIC       0
#define DFC_EDIT         1
#define DFC_COMBO        2
#define DFC_CHECK        3
#define DFC_DATE         4
#define DFC_TIME         5
#define DFC_PROGRESS     6
#define DFC_IMAGE        7
//행정보 구조체
typedef struct tagLvItemData
{
	tagLvItemData()
	{
		memset(this,0,sizeof(*this));
	}

	COLORREF clrText;
	COLORREF clrBkgnd;
	int nFontEffect;
	int nFontSize;//0:normal
	bool nChkState;
	int nImage;
	int nProgressPercent;
	UINT nAlign;
	bool bUpdate;

}LVITEMDATA;
//열정보 구조체
typedef struct tagLvColumnData
{
	tagLvColumnData()
	{
		memset(this,0,sizeof(*this));
	}
	CWnd *pWnd;//컨트럴 포인터
	int nCtrlType;//컨트럴 종류
	int nDataType;//자료유형
	int nDataFormat;//표시형식

}LVCOLUMNDATA;

BOOL CALLBACK EnumChildProc(HWND hwnd,LPARAM lParam);//콜백함수

// CListACtrl
class CListACtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CListACtrl)

public:
	CListACtrl();
	virtual ~CListACtrl();
private:

	bool	m_bEdit;
	int	m_nRow,m_nCol;
	CString	m_strValue;
	CFont	*m_font;//기본폰트,작은폰트
	CPtrArray	*m_pColumnsInfo;//컬럼정보배열
	CImageList *m_pImageList;
	int	m_nItemHeight;
	bool	m_bSelected;
	COLORREF m_clrBkHighLight,m_clrTextHighLight;
	int m_nHighLightMode;
//외부에서 접근불가	
private:
	// 에디트 컨트럴 그리기
	void DrawEditCtrl(CEdit *pEdit,int nRow, int nCol);//에디트 컨트럴 그리기
	void DrawComboBoxCtrl(CComboBox *pCombo,int nRow,int nCol);//콤보컨트럴
	void DrawCheckBoxCtrl(CButton *pButton,int nRow,int nCol);//체크상자 그리기
	void DrawDateTimeCtrl(CDateTimeCtrl *pDateTime,int nRow,int nCol);//날짜컨트럴그리기

	void AddColumnsCtrl(int nCol,int nCtrlType,int nDataType,int nDataFormat);//컬럼에 자식윈도우 컨트럴 배치
	//수동으로 아이템에 체크박스를 그린다.
	void DrawItemCheckBox(int nRow, int nCol, CDC *pDC,CRect &rect,	LVITEMDATA *pItemData);
	//수동으로 아이템에 텍스트를 그린다.
	void DrawItemText(int nRow, int nCol,CDC *pDC,CRect& rect,LVITEMDATA *pItemData);
	int DrawItemImage(int nRow,int nCol,CDC *pDC,CRect &rect,LVITEMDATA *pItemData);
	void DrawItemProgress(int nRow,int nCol,CDC *pDC,CRect &rect,LVITEMDATA *pItemData);
	int FindColumnCtrl(int nCol);
	bool IsBkImage();
	void GetItemFont(CFont &pFont,int nFontEffect,int nFontSize);
	void UpdateSubItem(int nRow,int nCol);
	void CheckItemColor(int nRow,int nCol,COLORREF &clrBkgnd,COLORREF &clrText);
	//컬럼별 아이템의 테두리 크기를 얻는다.(0번 서브아이템의 크기를 구함)
	bool GetColumnItemRect(int nRow,int nCol,int nArea,CRect &rect);
//외부에서 사용가능	
public:
	//지정한 컬럼의 컨트럴 포인터를 얻는다.
	CWnd *GetColumnsCtrl(int nColumn);
	//컬럼의 갯수를 얻는다.
	int	GetColumnsCnt();
	int GetItemHeight();//아이템의 높이를 얻는다.
	//아이템이 선택되었을때 색상지정
	void SetItemHighLightColor(COLORREF clrBkgnd,COLORREF clrText);
	//아이템선택 방법 지정(0:0번컬럼만1:모든컬럼다사용2:선택시 지정 글자색만 변경)
	void SetItemHighLightMode(int nHighLightMode);
	//진행막대의 진행 위치를 정한다.
	void SetProgressPos(int nRow,int nCol,int nPos);
	//원하는 블럭셀에 배경과 글자색을 정한다.
	void SetItemColor(int nRow1,int nRow2,int nCol1,int nCol2,
		COLORREF clrBkgnd=-1,COLORREF clrText=-1);
	//진행막대의 현재 진행위치를 얻는다.
	int GetProgressPos(int nRow,int nCol);
	//원하는 셀의 배경과 글자색을 얻는다.(기본 셀은 현재 선택된 셀)
	void GetItemColor(int nRow,int nCol,COLORREF &clrBkgnd,COLORREF &clrText);
	//0:no check 1: check bUpdate:아이템을 다시그릴 것인가? 기본 다시그린다.
	//체크표시의 변화를 바로 적용할 것인가를 정함
	void SetItemCheck(int nRow,int nCol,bool bChkState,bool bUpdate=true);
	//체크상자의 체크 여부를 얻는다.
	bool GetItemCheck(int nRow,int nCol);
	//원하는 아이템만 글자효과를 준다(1.굵게 2.기울게 3. 밑줄)
	void SetItemFont(int nRow1,int nRow2,int nCol1,int nCol2,
		int nFontEffect, int nFontSize=0);
	//원하는 셀의 글자속성과 글자크기를 얻는다.
	void GetItemFont(int nRow,int nCo,int &nFontEffect, int &nFontSize);
	int ItemDataRowCnt();//실제 자료가 입력되어 있는 행 갯수를 반환
	void ItemClearText();//입력된 글자를 모두 지운다(글자만 지움)
	bool GetItemModify(int nRow);//행의 내용의 편집 여부를 시스템이 반환
	void SetItemModify(int nRow,bool bUpdate);//내용의 변화 여부를 사용자가 지정
	void AlignItemText(int nRow1,int nRow2,int nCol1,int nCol2,UINT nAlign);//셀 글자 정렬 지정
	void AddRow(int nMaxRow=500,int nItem=0);
	//가상함수 사용
	virtual BOOL SetBkColor(COLORREF cr);//배경색 지정
	virtual BOOL SetItemText(int nItem,int nSubItem,LPCTSTR lpszText);//셀에 글자를 넣는다.
	virtual BOOL DeleteColumn(int nCol);//컬럼을 삭제한다.

	//원하는 위치(칸)에 컨트럴을 그린다.(pWnd:컨트럴포인터)
	bool DrawColumnsCtrl(CWnd *pWnd,int nCol,int nRow);
	//열을 만들고, 열에 컨트럴을 생성한다.(생성 컨트럴 기본값 static)
	//nDataType(0:문자,1:숫자,2:날짜,3:시간)	//nDataFormat-nDataType에 따라 변동
	//문자(0:한줄문자표시1:여러줄문자표시)숫자(0:3자리마다콤마사용1:콤마사용않함)
	//날짜(0:yyyy-mm-dd 1:yy-mm-dd) 시간(0:HH-MM-SS 1:A-PM hh-mm-ss)
	virtual int InsertColumn(int nCol, const LVCOLUMN * pColumn
							,int nCtrlType=0,int nDataType=0,int nDataFormat=0);
	virtual int InsertColumn(int nCol, LPCTSTR lpszColumnHeading
			, int nFormat,int nWidth, int nSubItem,int nCtrlType=0,int nDataType=0,int nDataFormat=0);
	//지정한 이미지리스트를 전역 이미지리스트와 치환한다.  
	virtual CImageList *SetImageList(CImageList *pImageList, int nImageListType);
	virtual void SetFont(CFont *pFont,BOOL bRedraw=1);

	void SetItemHeight(long nHeight);//아이템의 높이를 지정
	void SetItemImage(int nRow,int nCol,int nImageListImage);//아이템의 이미지를 지정
	CString InputComma(const TCHAR *szNumeral);//숫자 구분자 콤마를 찍는다.
	//문자를 균등배분하여 찍는다.
	void SpreadText(HDC hdc,CRect rect, const TCHAR *pStr,int margin);
	
//외부로 자동으로 나가지 않는 메시지
private:
	afx_msg void OnEditChild_KillFocus();
	afx_msg void OnEditChild_Update();
	afx_msg void OnNumEditChild_KillFocus();
	afx_msg void OnNumEditChild_Update();
	afx_msg void OnComboChild_LostFocus();
	afx_msg void OnComboChild_AfterUpdate();
	afx_msg void OnComboChild_SelChange();
	afx_msg void OnCheckChild_LostFocus();
	afx_msg void OnCheckChild_Click();
	afx_msg BOOL OnDtnKillFocus(UINT id,NMHDR *pNMHDR,LRESULT *pResult);
	afx_msg BOOL OnDtnChange(UINT id,NMHDR *pNMHDR,LRESULT *pResult);

protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PreSubclassWindow();
	DECLARE_MESSAGE_MAP()

//외부로도 나가는 메시지
public:
	//마법사는 NM_CLICK메시지가 상속되지 않음 반드시 이런식으로 해야 함
	afx_msg BOOL OnClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	void OnHdnTracking(NMHEADER *pnmhdr);
	afx_msg void OnLvnInsertitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//스크롤을 사용시 다시 그려준다.
	afx_msg void OnLvnEndScroll(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);

	//키보드 메시지 전처리
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

inline int CListACtrl::GetColumnsCnt() {return GetHeaderCtrl()->GetItemCount();}
inline int CListACtrl::GetItemHeight(){return m_nItemHeight;}
inline void CListACtrl::SetItemHighLightMode(int nHighLightMode){m_nHighLightMode=nHighLightMode;}