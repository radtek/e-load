#pragma once
#include "LabelStatic.h"
#include "EventData.h"
#include <gui/gridctrl/GridCtrl.h>

// CTransformerPropDlg dialog

class CTransformerPropDlg : public CDialog
{
	DECLARE_DYNAMIC(CTransformerPropDlg)

public:
	CTransformerPropDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTransformerPropDlg();
	
	static int IDEAL_WIDTH;
// Dialog Data
	enum { IDD = IDD_TRANSFORMER_PROP };
	///CLabelStatic m_wndBusNameListStatic;
	///int m_nConnType;
	CGridCtrl m_wndBusGrid;	//! 2011.02.07 added by HumKyung
	CMFCButton m_wndAddButton , m_wndDeleteButton , m_wndSaveButton;	//! 2011.02.07 added by HumKyung

	afx_msg void OnBnClickedOK();
	afx_msg void OnBnClickedCancel();
protected:
	afx_msg LRESULT OnItemSelected(WPARAM wParam, LPARAM lParam);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	int SetEventDataValues( CEventData* pEventData );
public:
	afx_msg void OnBnClickedRadioCable();
	afx_msg void OnBnClickedRadioBusDuct();
	afx_msg void OnCbnSelendokComboFromBusId();
	afx_msg void OnCbnSelendokComboToBusId();
	afx_msg void OnCbnSelendokComboFromCableVolt();
	afx_msg void OnCbnSelendokComboFromCableType();
	afx_msg void OnCbnSelendokComboToCableVolt();
	afx_msg void OnCbnSelendokComboToCableType();
	afx_msg void OnCbnSelendokComboFromCore();
	afx_msg void OnCbnSelendokComboToCore();
	afx_msg void OnEnChangeEditSelectedCapacity();
	afx_msg void OnEnChangeEditFromSpec();
	afx_msg void OnEnChangeEditToSpec();
private:
	int SetCalculatedCapacityText();
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnEnChangeEditTransformerPf();
	afx_msg void OnEnChangeEditTransformerLength();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonTrAdd();
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	afx_msg void OnBnClickedButtonTrSave();
private:
	int DisplayBusNameListOfTr(CTransformerItem* pTrItem);
public:
	afx_msg void OnBnClickedButtonTrDelete();
private:
	int SetCapacityProperty(CTransformerItem* pTrItem);
};
