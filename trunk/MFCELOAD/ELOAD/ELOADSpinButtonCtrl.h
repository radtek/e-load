#pragma once


// CELOADSpinButtonCtrl
class CMFCPropEx;
class CELOADSpinButtonCtrl : public CMFCSpinButtonCtrl
{
	DECLARE_DYNAMIC(CELOADSpinButtonCtrl)

public:
	CELOADSpinButtonCtrl();
	virtual ~CELOADSpinButtonCtrl();

	CMFCPropEx* SetBuddy( CMFCPropEx* pBuddy );
	CMFCPropEx* GetBuddy( ) const;
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDeltapos (NMHDR *pNMHDR, LRESULT *pResult);
private:
	CMFCPropEx* m_pBuddy;
};


