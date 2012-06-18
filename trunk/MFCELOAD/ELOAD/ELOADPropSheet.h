#pragma once


// CELOADPropSheet

class CELOADPropSheet : public CMFCPropertySheet
{
	DECLARE_DYNAMIC(CELOADPropSheet)

public:
	CELOADPropSheet();
	virtual ~CELOADPropSheet();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

class CELOADPropPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CELOADPropPage)

public:
	CELOADPropPage(UINT nIDTemplate,UINT nIDCaption=0 );
	virtual ~CELOADPropPage();
	virtual int SaveData(){ return ERROR_SUCCESS; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};