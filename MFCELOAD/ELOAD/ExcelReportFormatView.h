#pragma once
#include "afxwin.h"
#include "fpspread1.h"
#include "MFCButtonEx.h"
#include <gui/EasySize.h>

// CExcelReportFormatView form view

class CExcelReportFormatView : public CFormView
{
	DECLARE_DYNCREATE(CExcelReportFormatView)
protected:
	CExcelReportFormatView();           // protected constructor used by dynamic creation
	virtual ~CExcelReportFormatView();

public:
	enum { IDD = IDD_EXCEL_REPORT_FORMAT_VIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	CComboBox	m_wndSelectTableCombo;

	CMFCButtonEx	m_wndSaveButton;
	CMFCButtonEx	m_wndExcelImportButton;

	CFpspread1	m_wndSpread, m_wndEditSpread;

	CEdit		m_wndExcelNameEdit;
	CEdit		m_wndStartRowEdit;
	CStatic		m_wndTemplateNameStatic;
	CComboBox	m_wndTemplateNameCombo;
	CEdit		m_wndTemplateNameEdit;

	CEdit m_wndProjectNoEdit;
	CEdit m_wndProjectNameEdit;
	CEdit m_wndClientEdit;
	CEdit m_wndDateEdit;

	CString		m_rExcelFileName;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedExcelImportButton();
	
	void ComboSelChangeFpspread1(long Col, long Row);
	DECLARE_EVENTSINK_MAP()

	int FillTableNameCombo(void);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	int DisplayEditSpread(void);
	afx_msg void OnCbnSelchangeSelectTableCombo();
	int InsertItemInSpreadCombo(const int& nCol, const CString& rTableName);
	afx_msg void OnBnClickedSaveButton();
	int SaveData(const CString& rTemplateName, const CString& rRecordName, const CString& rStartRow, const CString& rExcelPath, const CString& rMapingData);
	int LoadTemplate(void);
	afx_msg void OnCbnSelchangeTemplateNameCombo();
	int UncompressExcelFile(const CString& rExcelPath, const CString& rFolderPath);
	int FindStringInSpread(const CString& rText);
	
};


