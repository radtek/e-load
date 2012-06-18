#pragma once

#include "ELoadItem.h"
#include "MFCPropEx.h"

using namespace ITEMS;

class CItemSelectionSet
{
public:
	CItemSelectionSet(void);
	CItemSelectionSet( const CItemSelectionSet& rhs);
	CItemSelectionSet& operator =(const CItemSelectionSet& rhs);
	virtual ~CItemSelectionSet(void);

	int GetItemCount() const;
	CELoadItem* GetItemAt( const size_t& at );
	int RemoveAt( const size_t& at );
	int Add(CELoadItem* pItem);

	bool IsLockItem(const string& rCategory , const string& rKey );
	bool IsLock(const string& rCategory , const string& rKey );
	int SetLock(const string& rCategory , const string& rKey , const bool& bLock);
	bool IsModified(const string& rCategory , const string& rKey );
	string GetValue( const string& rCategory , const string& rKey );
	string GetOriginalValue( const string& rCategory , const string& rKey );
	string GetUnitStr( const string& rCategory , const string& rKey );
	int    SetUnitStr( const string& rCategory , const string& rKey , const string& rUnitStr );
	string GetDataType( const string& rCategory , const string& rKey );
	int SetValue( const string& rCategory , const string& rKey , const string& rValue);

	string GetTypeString();
private:
	BOOL CheckDuplicationOfName(const CString& rName);
	vector<CELoadItem*> m_ItemEntry;
public:
	int Clear(void);
};
