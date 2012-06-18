#pragma once

#include <assert.h>
#include <util/ado/ADODB.h>
#include "ELoadItemProp.h"
#include "ItemMemStrategy.h"

#include <list>
#include <string>
using namespace std;

namespace ITEMS
{
#define	DECLARE_ELOAD_ITEM(classname)\
public:\
	virtual bool IsKindOf(const char* typeString) const;\
	static CELoadItem* CreateInstance();\
	static int DeleteInstance(classname* p);\
	CELoadItem* Clone();\
	string GetTypeString();\
	static const char* TypeString();\

#define	IMPLEMENTS_ELOAD_ITEM(classname,baseclassname,typestr)\
CELoadItem* classname::CreateInstance()\
{\
	classname* p = new classname;\
	if(p)\
	{\
		CItemMemStrategy& strategy = CItemMemStrategy::GetInstance();\
		strategy.OnCreate(p);\
	}\
	return p;\
}\
int classname::DeleteInstance(classname* p)\
{\
	assert(p && "p is NULL");\
	if(p)\
	{\
		CItemMemStrategy& strategy = CItemMemStrategy::GetInstance();\
		strategy.OnDelete(p);\
		SAFE_DELETE(p);\
	}\
	return ERROR_SUCCESS;\
}\
CELoadItem* classname::Clone()\
{\
	classname* p = static_cast<classname*>(classname::CreateInstance());\
	if(p) p->Copy(this);\
	return p;\
}\
bool classname::IsKindOf(const char* typeString) const\
{\
	if(0 == strcmp(classname::TypeString() , typeString)) return true;\
	return baseclassname::IsKindOf(typeString);\
}\
string classname::GetTypeString()\
{\
	return classname::TypeString();\
}\
const char* classname::TypeString()\
{\
	static const char* typeString=typestr;\
	return typestr;\
}\

class CELoadItem
{
protected:
	CELoadItem( );
public:
	enum
	{
		DATABASE_ITEM = 0x01,
		DELETED_ITEM  = 0x02,
		MODIFIED_ITEM = 0x04
	};

	static string ADDED_FIELD_NAME[4];

	virtual ~CELoadItem(void);

	const char* id() const;
	CELoadItemProp* prop() const
	{
		return m_pProp;
	}

	/**
		@brief	

		@author	BHK
	*/
	int SetProp( CELoadItemProp* pProp )
	{
		if(NULL != m_pProp) delete m_pProp;
		m_pProp = pProp;
		m_pProp->SetOwner(this);

		return ERROR_SUCCESS;
	}

	virtual string GetId() const
	{
                return m_prId->c_str();
	}
        virtual int SetId(const string& rId)
        {
                m_prId = new string(rId);
                if(m_prId) return ERROR_SUCCESS;
                return ERROR_BAD_ENVIRONMENT;
        }

	virtual int SaveBinaryData(CADODB& adoDB , const string& rTableName);	//! database에 데이터를 저장합니다.
	virtual int SaveData(CADODB& adoDB , const string& rTableName);	//! database에 데이터를 저장합니다.
	virtual int LoadData(CELoadItemProp* pItemProp , CADODB& adoDB , const int& nIndex);		//! database에서 value을 읽는다.
	virtual bool IsKindOf(const char* typeString) const;
	virtual CELoadItem* Clone() = 0;
        virtual int Copy(CELoadItem* pItem);
	virtual string GetName() const{ return string(""); }
	virtual int SetName( const string& name){ return ERROR_SUCCESS; }
	virtual bool valid() const{return true;}
	virtual string GetTypeString();
	static const char* TypeString();
protected:
	CELoadItemProp* m_pProp;

	int  m_nStatus;		//! item status
public:
	int SetDataBaseItem();
	bool IsDataBaseItem() const;
	bool IsDeleted(void) const;
	virtual int SetDeleteFlag(const bool& bDelete);
	bool IsModified() const;
	int SetModifiedFlag(const bool& bModified);

	static bool IsNotDeleted(CELoadItem* pItem);
private:
	string* m_prId;
};

struct MatchItemName : public binary_function<CELoadItem*,string,bool>
{
	bool operator()(CELoadItem* lhs , const string& rhs) const
	{
		return (!(lhs->IsDeleted()) && (lhs->GetName() == rhs));
	}
};
};