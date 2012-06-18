#pragma once


// CELoadMenuBar

class CELoadMenuBar : public CMFCMenuBar
{
public:
	CELoadMenuBar();
	virtual ~CELoadMenuBar();
public:
	int AddSubMenu(void);
protected:
	virtual BOOL RestoreOriginalstate ();
};


