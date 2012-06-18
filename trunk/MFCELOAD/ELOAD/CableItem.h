#pragma once
#include "eloaditem.h"

#include <list>
using namespace std;

namespace ITEMS
{
class CLoadItem;
class CSpaceHeaterPowerCableItem;
class CCableItem : public CELoadItem
{
public:
	enum
	{
		TRY_CALCULATED_RESULTS	= 5
	};

	CCableItem(void);
	virtual ~CCableItem(void);

	static string TableName();
	void SetLoadItemPtr( CLoadItem* pLoadItem );
	CLoadItem* GetLoadItemPtr() const;
	string GetCategoryString();
	string GetName() const;
	int SetName( const string& name );
	int Copy(CELoadItem* pCopied);
private:
	CELoadItemProp* m_pCurProp;
	CLoadItem *m_pLoadItem;
	list<CELoadItemProp*> m_BelowCalculatedResultList , m_OverCalculatedResultList;

	DECLARE_ELOAD_ITEM(CCableItem)
public:
	bool IsEquipmentCable() const
	{
		const string rSubCategory = prop()->GetValue(_T("General") , _T("SubCategory"));
		return (_T("Equipment") == rSubCategory);
	}
	int GetCalculatedResultList(list<CELoadItemProp*>& rhs);
	CELoadItemProp* GetPropForBelowCalculatedResult();
	CELoadItemProp* GetPropForOverCalculatedResult();
	int ClearCalculatedResult(void);
};
};