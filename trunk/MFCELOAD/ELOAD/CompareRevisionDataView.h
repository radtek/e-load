#pragma once
#include "fpspread1.h"

// CCompareRevisionDataView form view

class CCompareRevisionDataView : public CFormView
{
	DECLARE_DYNCREATE(CCompareRevisionDataView)

protected:
	CCompareRevisionDataView();           // protected constructor used by dynamic creation
	virtual ~CCompareRevisionDataView();

public:
	enum { IDD = IDD_VIEWREVISIONVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	CFpspread1 m_wndSpread;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
private:
	int DisplayCompareResultOfRevisionData(const int& nKeyIndex , const string& rFileExt);
	void CreateBusSheetHeader(void);
	int CreateTotalBusSheetHeader();
	int OnUpdateBUSRevisionData(void);
	
	int OnUpdateTransformerSizingResultRevisionData();
	
	int CreateCableSizingResultHeader();
	int OnUpdateCableSizingResultRevisionData(const CString& rExtension);

	int OnUpdateCableSizingTableRevisionData();
	
	int AdjustPowerCableSheetTextAlign(void);
	int AdjustTotalSheetTextAlign(void);
	int AdjustBusSheetTextAlign(void);
	int AdjustControlCableSheetTextAlign(void);
	int AdjustTransformerSheetTextAlign(void);
	int OnUpdateControlCableResultRevisonData(const CString& rExtension);
	int OnUpdateCableScheduleResultRevisonData(void);
	int OnUpdateDrumScheduleResultRevisonData(void);
	int CreateControlCableResultHeader(void);

	int CompareSafetyVoltage(void);
	int CompareValueOverThousand(void);
public:
	afx_msg void OnFilePrint();
private:
	int CollectItemsForComparing(set<string>& CompareItemSet , const CString& rExtension);
};


