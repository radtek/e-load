// FillGridCellComboBox.h: interface for the CFillGridCellComboBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FillGridCellComboBox_H__E81AEECF_802C_40A2_A0A9_513C089DF2B1__INCLUDED_)
#define AFX_FillGridCellComboBox_H__E81AEECF_802C_40A2_A0A9_513C089DF2B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <gui/gridctrl/GridCtrl.h>
#include "GridCellEx.h"
#include <string>

using namespace std;


class CFillGridCellComboBox  : public CGridCellEx
{
	DECLARE_DYNCREATE(CFillGridCellComboBox)
public:
	CFillGridCellComboBox();
        
	virtual ~CFillGridCellComboBox();
public:

        BOOL Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
        void EndEdit(const CString& rComboEdit = "");
private:
        CFont m_font;
        bool IsEnd;

        CString m_rValue;
public:
};

#endif // !defined(AFX_FillGridCellComboBox_H__E81AEECF_802C_40A2_A0A9_513C089DF2B1__INCLUDED_)
#pragma once


// CMyComboBox

class CMyComboBox : public CComboBox
{
        DECLARE_DYNAMIC(CMyComboBox)

public:
        CMyComboBox(CFillGridCellComboBox* pCell);
        virtual ~CMyComboBox();
protected:
        DECLARE_MESSAGE_MAP()
public:
        afx_msg void OnCbnSelchange();
private:
        CFillGridCellComboBox* m_pCell;
};


