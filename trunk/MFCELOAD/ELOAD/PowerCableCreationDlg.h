#pragma once

#include "Tables/CableDataTable.h"
#include "GridCtrlEx.h"
#include <gui/FilterEdit/BoostBaseFilterEdit.h>
#include "LabelStatic.h"
#include "XGroupBox.h"
#include "GroupCheck.h"
#include "afxwin.h"
#include <string>
#include <vector>
#include "LoadItem.h"
#include "CableLengthDataSheet.h"

#include "Tables/ELOADTable.h"

using namespace TABLES;
// CPowerCableCreationDlg dialog

class CPowerCableCreationDlg : public CDialog
{
	DECLARE_DYNAMIC(CPowerCableCreationDlg)

public:
	CPowerCableCreationDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPowerCableCreationDlg();

	
// Dialog Data
	enum { IDD = IDD_POWER_CABLE_CREATION_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedPowerCableCreation();
	afx_msg void OnBnClickedCancel();

	CMFCButton m_wndCableCreationButton;
	CMFCButton m_wndCancelButton;

	int m_nLoadCableCreationRadio;
	int m_nBusCableCreationRadio;

	CGroupCheck m_wndLoadCableCreationCheckButton;
	CGroupCheck m_wndBusCableCreationCheckButton;
	
	//CXGroupBox m_StaticPowerCableCreation;
	CXGroupBox m_StaticLoadPowerCableCreation;
	CXGroupBox m_StaticBusPowerCableCreation;

	virtual BOOL OnInitDialog();
	int LoadData(void);
	int SaveData(void);
	int DisplayData(void);
	int LoadCableDataTable(void);
	afx_msg void OnBnClickedCheckEqBusPowerCableCreation();
	afx_msg void OnBnClickedCheckLoadPowerCableCreation();
};
