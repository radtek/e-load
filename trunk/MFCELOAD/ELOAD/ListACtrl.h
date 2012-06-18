#pragma once
#include "stdafx.h"


#define DT_RATE          0x4//�յ��� ����
//��Ʈ�� ������
#define DFC_NOUSE       -1//�����ý��� �׸��� �����(0���÷��� ������� ����Ҷ� �̿��ϸ��)
#define DFC_STATIC       0
#define DFC_EDIT         1
#define DFC_COMBO        2
#define DFC_CHECK        3
#define DFC_DATE         4
#define DFC_TIME         5
#define DFC_PROGRESS     6
#define DFC_IMAGE        7
//������ ����ü
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
//������ ����ü
typedef struct tagLvColumnData
{
	tagLvColumnData()
	{
		memset(this,0,sizeof(*this));
	}
	CWnd *pWnd;//��Ʈ�� ������
	int nCtrlType;//��Ʈ�� ����
	int nDataType;//�ڷ�����
	int nDataFormat;//ǥ������

}LVCOLUMNDATA;

BOOL CALLBACK EnumChildProc(HWND hwnd,LPARAM lParam);//�ݹ��Լ�

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
	CFont	*m_font;//�⺻��Ʈ,������Ʈ
	CPtrArray	*m_pColumnsInfo;//�÷������迭
	CImageList *m_pImageList;
	int	m_nItemHeight;
	bool	m_bSelected;
	COLORREF m_clrBkHighLight,m_clrTextHighLight;
	int m_nHighLightMode;
//�ܺο��� ���ٺҰ�	
private:
	// ����Ʈ ��Ʈ�� �׸���
	void DrawEditCtrl(CEdit *pEdit,int nRow, int nCol);//����Ʈ ��Ʈ�� �׸���
	void DrawComboBoxCtrl(CComboBox *pCombo,int nRow,int nCol);//�޺���Ʈ��
	void DrawCheckBoxCtrl(CButton *pButton,int nRow,int nCol);//üũ���� �׸���
	void DrawDateTimeCtrl(CDateTimeCtrl *pDateTime,int nRow,int nCol);//��¥��Ʈ���׸���

	void AddColumnsCtrl(int nCol,int nCtrlType,int nDataType,int nDataFormat);//�÷��� �ڽ������� ��Ʈ�� ��ġ
	//�������� �����ۿ� üũ�ڽ��� �׸���.
	void DrawItemCheckBox(int nRow, int nCol, CDC *pDC,CRect &rect,	LVITEMDATA *pItemData);
	//�������� �����ۿ� �ؽ�Ʈ�� �׸���.
	void DrawItemText(int nRow, int nCol,CDC *pDC,CRect& rect,LVITEMDATA *pItemData);
	int DrawItemImage(int nRow,int nCol,CDC *pDC,CRect &rect,LVITEMDATA *pItemData);
	void DrawItemProgress(int nRow,int nCol,CDC *pDC,CRect &rect,LVITEMDATA *pItemData);
	int FindColumnCtrl(int nCol);
	bool IsBkImage();
	void GetItemFont(CFont &pFont,int nFontEffect,int nFontSize);
	void UpdateSubItem(int nRow,int nCol);
	void CheckItemColor(int nRow,int nCol,COLORREF &clrBkgnd,COLORREF &clrText);
	//�÷��� �������� �׵θ� ũ�⸦ ��´�.(0�� ����������� ũ�⸦ ����)
	bool GetColumnItemRect(int nRow,int nCol,int nArea,CRect &rect);
//�ܺο��� ��밡��	
public:
	//������ �÷��� ��Ʈ�� �����͸� ��´�.
	CWnd *GetColumnsCtrl(int nColumn);
	//�÷��� ������ ��´�.
	int	GetColumnsCnt();
	int GetItemHeight();//�������� ���̸� ��´�.
	//�������� ���õǾ����� ��������
	void SetItemHighLightColor(COLORREF clrBkgnd,COLORREF clrText);
	//�����ۼ��� ��� ����(0:0���÷���1:����÷��ٻ��2:���ý� ���� ���ڻ��� ����)
	void SetItemHighLightMode(int nHighLightMode);
	//���ื���� ���� ��ġ�� ���Ѵ�.
	void SetProgressPos(int nRow,int nCol,int nPos);
	//���ϴ� ������ ���� ���ڻ��� ���Ѵ�.
	void SetItemColor(int nRow1,int nRow2,int nCol1,int nCol2,
		COLORREF clrBkgnd=-1,COLORREF clrText=-1);
	//���ื���� ���� ������ġ�� ��´�.
	int GetProgressPos(int nRow,int nCol);
	//���ϴ� ���� ���� ���ڻ��� ��´�.(�⺻ ���� ���� ���õ� ��)
	void GetItemColor(int nRow,int nCol,COLORREF &clrBkgnd,COLORREF &clrText);
	//0:no check 1: check bUpdate:�������� �ٽñ׸� ���ΰ�? �⺻ �ٽñ׸���.
	//üũǥ���� ��ȭ�� �ٷ� ������ ���ΰ��� ����
	void SetItemCheck(int nRow,int nCol,bool bChkState,bool bUpdate=true);
	//üũ������ üũ ���θ� ��´�.
	bool GetItemCheck(int nRow,int nCol);
	//���ϴ� �����۸� ����ȿ���� �ش�(1.���� 2.���� 3. ����)
	void SetItemFont(int nRow1,int nRow2,int nCol1,int nCol2,
		int nFontEffect, int nFontSize=0);
	//���ϴ� ���� ���ڼӼ��� ����ũ�⸦ ��´�.
	void GetItemFont(int nRow,int nCo,int &nFontEffect, int &nFontSize);
	int ItemDataRowCnt();//���� �ڷᰡ �ԷµǾ� �ִ� �� ������ ��ȯ
	void ItemClearText();//�Էµ� ���ڸ� ��� �����(���ڸ� ����)
	bool GetItemModify(int nRow);//���� ������ ���� ���θ� �ý����� ��ȯ
	void SetItemModify(int nRow,bool bUpdate);//������ ��ȭ ���θ� ����ڰ� ����
	void AlignItemText(int nRow1,int nRow2,int nCol1,int nCol2,UINT nAlign);//�� ���� ���� ����
	void AddRow(int nMaxRow=500,int nItem=0);
	//�����Լ� ���
	virtual BOOL SetBkColor(COLORREF cr);//���� ����
	virtual BOOL SetItemText(int nItem,int nSubItem,LPCTSTR lpszText);//���� ���ڸ� �ִ´�.
	virtual BOOL DeleteColumn(int nCol);//�÷��� �����Ѵ�.

	//���ϴ� ��ġ(ĭ)�� ��Ʈ���� �׸���.(pWnd:��Ʈ��������)
	bool DrawColumnsCtrl(CWnd *pWnd,int nCol,int nRow);
	//���� �����, ���� ��Ʈ���� �����Ѵ�.(���� ��Ʈ�� �⺻�� static)
	//nDataType(0:����,1:����,2:��¥,3:�ð�)	//nDataFormat-nDataType�� ���� ����
	//����(0:���ٹ���ǥ��1:�����ٹ���ǥ��)����(0:3�ڸ������޸����1:�޸�������)
	//��¥(0:yyyy-mm-dd 1:yy-mm-dd) �ð�(0:HH-MM-SS 1:A-PM hh-mm-ss)
	virtual int InsertColumn(int nCol, const LVCOLUMN * pColumn
							,int nCtrlType=0,int nDataType=0,int nDataFormat=0);
	virtual int InsertColumn(int nCol, LPCTSTR lpszColumnHeading
			, int nFormat,int nWidth, int nSubItem,int nCtrlType=0,int nDataType=0,int nDataFormat=0);
	//������ �̹�������Ʈ�� ���� �̹�������Ʈ�� ġȯ�Ѵ�.  
	virtual CImageList *SetImageList(CImageList *pImageList, int nImageListType);
	virtual void SetFont(CFont *pFont,BOOL bRedraw=1);

	void SetItemHeight(long nHeight);//�������� ���̸� ����
	void SetItemImage(int nRow,int nCol,int nImageListImage);//�������� �̹����� ����
	CString InputComma(const TCHAR *szNumeral);//���� ������ �޸��� ��´�.
	//���ڸ� �յ����Ͽ� ��´�.
	void SpreadText(HDC hdc,CRect rect, const TCHAR *pStr,int margin);
	
//�ܺη� �ڵ����� ������ �ʴ� �޽���
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

//�ܺηε� ������ �޽���
public:
	//������� NM_CLICK�޽����� ��ӵ��� ���� �ݵ�� �̷������� �ؾ� ��
	afx_msg BOOL OnClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	void OnHdnTracking(NMHEADER *pnmhdr);
	afx_msg void OnLvnInsertitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//��ũ���� ���� �ٽ� �׷��ش�.
	afx_msg void OnLvnEndScroll(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);

	//Ű���� �޽��� ��ó��
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

inline int CListACtrl::GetColumnsCnt() {return GetHeaderCtrl()->GetItemCount();}
inline int CListACtrl::GetItemHeight(){return m_nItemHeight;}
inline void CListACtrl::SetItemHighLightMode(int nHighLightMode){m_nHighLightMode=nHighLightMode;}