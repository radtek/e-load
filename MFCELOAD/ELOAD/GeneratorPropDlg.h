#pragma once

#include "LoadListDlg.h"
#include "PG1Dlg.h"
#include "PG2Dlg.h"
#include "PG3Dlg.h"

#include "EventData.h"
// CGeneratorPropDlg dialog

class CGeneratorPropDlg : public CDialog
{
	DECLARE_DYNAMIC(CGeneratorPropDlg)

public:
	CGeneratorPropDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGeneratorPropDlg();

// Dialog Data
	enum { IDD = IDD_GENERATOR_PROP };
	static int IDEAL_WIDTH;
	
	CLoadListDlg m_wndLoadList;
	CPG1Dlg m_wndPG1;
	CPG2Dlg m_wndPG2;
	CPG3Dlg m_wndPG3;

	CMFCTabCtrl m_wndTab;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg LRESULT OnItemSelected(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	int SetEventDataValues( CEventData* pEventData );
public:
	afx_msg void OnCbnSelendokComboToBusId();
	afx_msg void OnCbnSelendokComboToCableVolt();
	afx_msg void OnCbnSelendokComboToCableType();
	afx_msg void OnEnChangeEditToSpec();
public:
	afx_msg void OnEnChangeEditPg1Cos();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEnChangeEditPg2();
	afx_msg void OnCbnSelendokComboToCore();
	afx_msg void OnEnChangeEditPg1();
	afx_msg void OnEnChangeEditPg3();
	int CalcGeneratorProp(const string& rBusID);
	afx_msg void OnEnChangeEditSelectedValue();
};
