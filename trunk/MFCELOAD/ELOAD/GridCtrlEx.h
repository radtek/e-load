#pragma once


// CGridCtrlEx
#include <gui/gridctrl/GridCtrl.h>
#include <vector>

using namespace std;

class CGridCtrlEx : public CGridCtrl
{
	DECLARE_DYNAMIC(CGridCtrlEx)
        friend class CGridCellBaseEx;

public:
	CGridCtrlEx();
	virtual ~CGridCtrlEx();

protected:
	DECLARE_MESSAGE_MAP()
public:
        afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
        afx_msg void OnPastectrl();
        afx_msg void OnCopyctrl();
        afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

	BOOL MultiSortItems(const int& low , const int& high);
private:
	int MultiCompare(const int& lhs, const int& rhs);
	vector<int> m_MultiSortCellNoEntry , m_MultiSortDirEntry;

protected:
	static const UINT m_FindDialogMessage;
	bool m_bRemoveShowSelAlwaysAtFindDialogExit;
	CFindReplaceDialog *m_pFindDialog;
	CString		m_rFindName;
	bool		m_bMatchCase;
	bool		m_bMatchWholeWord;
	bool		m_bSearchDown;

	bool FindWhatYouNeed(CString FindName, bool bMatchCase, bool bMatchWholeWord, bool bSearchDown);
	afx_msg LRESULT OnFindDialogMessage(WPARAM wParam, LPARAM lParam);
	CCellID GetNextCell(CCellID cell);
	CCellID GetPrevCell(CCellID cell);

	bool OnFindContinue(void);
	int ScrollMove(CCellID startCell, CCellID actCell);

public:
	int SetMultiSortInfoEntry(const vector<int>& MultiSortCellNoEntry , const vector<int>& MultiSortDirEntry);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	int OnFind(void);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	

};


