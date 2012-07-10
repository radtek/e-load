#pragma once

#include "..\ControlCableResultDoc.h"

namespace COMMAND
{
class CCommandControlCableCreation
{
public:
	CCommandControlCableCreation();
	~CCommandControlCableCreation(void);

	int Execute(const bool& bExecuteSelectedLoad = false);
	static UINT StatusThreadEntry(LPVOID pVoid);
	int StatusThread(void);
	string GetResultString(void);
public:
	list<CELoadItem*> m_InterestingItemList;
private:
	int GetInterestingLoadItemListAndOption();
	string GetControlTypeOf(CELOADTable::Record& record);
	BOOL IsExistLoadId(const CStringArray& LoadArray, const CString& rLoadId);
	int SetCableProp(CCableItem* pCableItem, CLoadItem* pLoadItem , CELOADTable::Record& record);

	bool	m_bExecuteSelectedLoad;
	int	m_nLoadControlCableRadio;
	double	m_nDCSDesignLength;
	int	m_nTryCount , m_nSuccessCount , m_nFailCount; //! ½ÇÇà ½Ãµµ È½¼ö , ¼º°ø È½¼ö , ½ÇÆÐ È½¼ö
};
};
