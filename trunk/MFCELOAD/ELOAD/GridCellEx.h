#pragma once


// CGridCellEx
#include "GridCtrlEx.h"
#include <gui/GridCtrl/InPlaceBoostBaseFilterEdit.h>

#include <string>
using namespace std;

class CGridCellEx : public CGridCell
{
        DECLARE_DYNCREATE(CGridCellEx)
        friend CGridCtrlEx;
public:
        CGridCellEx();
        virtual ~CGridCellEx();
public:
	int OnDrawValue(CDC* pDC, CRect rect);
	BOOL Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
	void OnEndEdit();
	
	COLORREF GetTextClr() const;
	CFont* GetFontObject() const;

	static string DATA_TYPE_STRING;
protected:
	CString m_rDataType;
	int m_nRow , m_nCol;
private:
	CFont* m_pFont;
public:
	bool m_bEndEdit;
	CString m_rUnit;
};


