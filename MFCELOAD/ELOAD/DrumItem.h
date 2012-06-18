#pragma once
#include "eloaditem.h"

#include <list>
using namespace std;

namespace ITEMS
{
class CDrumItem : public CELoadItem
{

public:
	CDrumItem(void);
	virtual~CDrumItem(void);

	typedef struct
	{
		CString rCableId;
		CString rFrom;
		CString rTo;
		CString rLength;
		CString rArea;
	}CABLE_RECORD;

	static string TableName();
	string GetName() const;
	int SetName( const string& name );
	int Copy(CELoadItem* pCopied);
	string GetCategoryString();
private:
	CELoadItemProp* m_pCurProp;
	CLoadItem *m_pLoadItem;
	list<CABLE_RECORD> m_CableRecordList;

	DECLARE_ELOAD_ITEM(CDrumItem)
public:
	int AddCable(const CABLE_RECORD& record);
	int GetCableRecordList(list<CABLE_RECORD>& CableRecordList);
	int ClearCableRecord(void);
};
};