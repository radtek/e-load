#pragma once

#include <assert.h>
#include "Tokenizer.h"

#include <string>
#include <map>
#include <vector>
using namespace std;

namespace ITEMS
{
class CItemPropCategory
{
public:
	typedef struct tagItemPropTag
	{
		CItemPropCategory* pParent;

		string rKey;
		string rOriginalValue , rValue , rValueType;
		string rLockFlag , rCurLock;
		string rDesc;
                string rAccess;
		string rUnitList , rCurUnit;	//! UNIT LIST , CURRENT SETTED UNIT
		string rFieldType;		//! DATABASE FIELD TYPE

		tagItemPropTag(){
			rCurLock = _T("FALSE");
		}

		/**
			@brief	

			@author	BHK
		*/
		bool IsModified() const
		{ 
			CString lhs = rOriginalValue.c_str();
			CString rhs = rValue.c_str();

			if(IS_NUMBER_TYPE(rValueType))
				return (atof(lhs) != atof(rhs));
			else	return (lhs.MakeUpper() != rhs.MakeUpper());
		}

		bool IsLocked() const;
		bool IsUnLocked() const;
		
		bool IsGroupLock() const;
		bool IsLockItem() const;
		bool IsGroupLockItem() const;

		int GetUnitCount() const
		{
			vector<string> oResult;
			CTokenizer<CIsComma>::Tokenize(oResult , rUnitList.c_str() , CIsComma());
			return oResult.size();
		}

		string GetUnit( const int& nPos )
		{
			vector<string> oResult;
			CTokenizer<CIsComma>::Tokenize(oResult , rUnitList.c_str() , CIsComma());

			assert(nPos < (int)(oResult.size()) && "unit range error");
			if(nPos < (int)(oResult.size())) return oResult[nPos];
			
			return string("NONE");
		}

		int GetUnitPos( const string& rUnit )
		{
			vector<string> oResult;
			CTokenizer<CIsComma>::Tokenize(oResult , rUnitList.c_str() , CIsComma());
			for(int i = 0;i < (int)(oResult.size());++i)
			{
				if(rUnit == oResult[i]) return i;
			}

			return 0;
		}
	}ItemPropTag;

	CItemPropCategory( const string& rName , const string& rAttr = "VISIBLE")
	{
		m_pName = new string(rName);
		m_pAttr = new string(rAttr);
		m_pItemValueEntry = new vector<ItemPropTag*>;
	}
	
	virtual ~CItemPropCategory()
	{
		SAFE_DELETE(m_pName);
		SAFE_DELETE(m_pAttr);

		if(m_pItemValueEntry)
		{
			for(vector<ItemPropTag*>::iterator itr = m_pItemValueEntry->begin();itr != m_pItemValueEntry->end();++itr)
			{
				SAFE_RELEASE(*itr);
			}
			m_pItemValueEntry->clear();
			SAFE_DELETE(m_pItemValueEntry);
		}
	}
	CItemPropCategory* Clone();
	int Copy(CItemPropCategory* rhs);

	const char* name() const
	{
		return m_pName->c_str();
	}

	const char* attr() const
	{
		return m_pAttr->c_str();
	}

	ItemPropTag* GetItemPropTag(const string& rKey);

	string GetValue( const string& rKey ) const
	{
		CString rKeyText = rKey.c_str();
		rKeyText.MakeUpper();
		for(vector<ItemPropTag*>::const_iterator itr = m_pItemValueEntry->begin();itr != m_pItemValueEntry->end();++itr)
		{
                        CString rTemp = (*itr)->rKey.c_str();
                        if(rKeyText == rTemp.MakeUpper()) return (*itr)->rValue;
		}

		return "";
	}

	bool IsModified( const string& rKey ) const;
	bool IsLocked( const string& rKey ) const;

	string GetOriginalValue( const string& rKey ) const
	{
		CString rKeyText = rKey.c_str();
		rKeyText.MakeUpper();
		for(vector<ItemPropTag*>::const_iterator itr = m_pItemValueEntry->begin();itr != m_pItemValueEntry->end();++itr)
		{
                        CString rTemp = (*itr)->rKey.c_str();
                        if(rKeyText == rTemp.MakeUpper()) return (*itr)->rOriginalValue;
		}

		return "";
	}

	string GetUnitStr( const string& rKey ) const
	{
		for(vector<ItemPropTag*>::const_iterator itr = m_pItemValueEntry->begin();itr != m_pItemValueEntry->end();++itr)
		{
                        CString rTemp = (*itr)->rKey.c_str();
                        CString rKeyText = rKey.c_str();
                        if(rKeyText.MakeUpper() == rTemp.MakeUpper()) return (*itr)->rCurUnit;
		}

		return "NONE";
	}
	int SetUnitStr( const string& rKey , const string& rUnitStr );

	string GetValueType( const string& rKey ) const
	{
		for(vector<ItemPropTag*>::const_iterator itr = m_pItemValueEntry->begin();itr != m_pItemValueEntry->end();++itr)
		{
                        CString rTemp = (*itr)->rKey.c_str();
                        CString rKeyText = rKey.c_str();
                        if(rKeyText.MakeUpper() == rTemp.MakeUpper()) return (*itr)->rValueType;
		}

		return "";
	}

	/**
		@brief	

		@author	BHK
	*/
	int SetValue( const string& rKey , const string& rValue , const bool& bIgnoreLock = false);
	int SetOriginalValue( const string& rKey , const string& rOriginalValue , const bool& bIgnoreLock = false);
	int SetLock( const string& rKey , const bool& bLock );

	int CommitModifiedValues();	//! original value를 value로 설정한다.
	int ResetModifiedValues();	//! value를 original value로 설정한다.

	/**
		@brief	

		@author	BHK
	*/
	int ParseItemString( const string& rItemString )
	{
		vector<string> oResult;
		CTokenizer<CIsFromString>::Tokenize(oResult , rItemString , CIsFromString(":"));
		if(oResult.size() > 0)
		{
			for(vector<ItemPropTag*>::iterator itr = m_pItemValueEntry->begin();itr != m_pItemValueEntry->end();++itr)
			{
				if(oResult[0] == (*itr)->rKey)
				{
					(*itr)->pParent = this;

					//! 기존에 등록되어 있다면 기존 항목의 값을 변경한다.
					(*itr)->rValue = "XXXXXXXXXX";
					(*itr)->rValueType = (oResult.size() > 1) ? oResult[1] : "";
					(*itr)->rLockFlag  = (oResult.size() > 2) ? oResult[2] : _T("UNLOCK");
					(*itr)->rDesc      = "";
                                        (*itr)->rAccess    = (oResult.size() > 3) ? oResult[3] : "RW";
					(*itr)->rUnitList  = (oResult.size() > 4) ? oResult[4] : "NONE";
					if("NONE" != (*itr)->rUnitList)
					{
						CTokenizer<CIsComma>::Tokenize(oResult , (*itr)->rUnitList.c_str() , CIsComma());
						(*itr)->rCurUnit = (oResult.size() >= 1) ? oResult[0] : "";
					}else	(*itr)->rCurUnit = "";
					(*itr)->rFieldType = (oResult.size() > 5) ? oResult[5] : _T("VARCHAR(50)");
					return ERROR_SUCCESS;
				}
			}

			ItemPropTag* pItemPropTag = (ItemPropTag*)calloc(1 , sizeof(ItemPropTag));
			if(pItemPropTag)
			{
				pItemPropTag->pParent    = this;
				pItemPropTag->rKey       = oResult[0];
				pItemPropTag->rValue     = "";
				pItemPropTag->rValueType = (oResult.size() > 1) ? oResult[1] : "";
				pItemPropTag->rLockFlag  = (oResult.size() > 2) ? oResult[2] : "UNLOCK";
				pItemPropTag->rDesc      = "";
                                pItemPropTag->rAccess    = (oResult.size() > 3 ) ? oResult[3] : "RW";
				pItemPropTag->rUnitList  = (oResult.size() > 4) ? oResult[4] : "NONE";
				if("NONE" != pItemPropTag->rUnitList)
				{
					CTokenizer<CIsComma>::Tokenize(oResult , pItemPropTag->rUnitList.c_str() , CIsComma());
					pItemPropTag->rCurUnit = (oResult.size() >= 1) ? oResult[0] : "";
				}else	pItemPropTag->rCurUnit = "";
				pItemPropTag->rFieldType = (oResult.size() > 5) ? oResult[5] : _T("VARCHAR(50)");
				m_pItemValueEntry->push_back(pItemPropTag);
			}
			
			return ERROR_SUCCESS;
		}

		return ERROR_INVALID_PARAMETER;
	}

	string GetItemNameAt( const size_t& at ) const;
	ItemPropTag* GetItemAt( const size_t& at ) const;
	size_t GetItemCount() const;
private:
	string* m_pName;
	string* m_pAttr;
	vector<ItemPropTag*>* m_pItemValueEntry;
public:
	int SetOriginalValueFromNewValue(CItemPropCategory* rhs);
};

class CELoadItem;
class CELoadItemProp
{
protected:
	CELoadItemProp(const CELoadItemProp& rhs){}
public:
	CELoadItemProp(CELoadItem* pOwner = NULL);
	virtual ~CELoadItemProp(void);

	CItemPropCategory* GetCategoryByName( const string& rName ) const;
	CItemPropCategory* GetCategoryAt( const size_t& at ) const;
	size_t GetCategoryCount() const;
	int Read(const string& rFilePath);
	string GetValue( const string& rCategory , const string& rKey ) const;
	string GetOriginalValue(const string& rCategory , const string& rKey) const;
	string GetUnitStr( const string& rCategory , const string& rKey ) const;
	int    SetUnitStr( const string& rCategory , const string& rKey , const string& rUnitStr );
	string GetDataType( const string& rCategory , const string& rKey ) const;
	virtual int SetValue( const string& rCategory , const string& rKey , const string& rValue , const bool& bIgnoreLock = false);
	int SetOriginalValue( const string& rCategory , const string& rKey , const string& rOriginValue , const bool& bIgnoreLock = false);
	int CommitModifiedValues();	//! origianl value를 value로 설정한다.
	int ResetModifiedValues();	//! value를 original value로 설정한다.
	
	int SetLock(const string& rCategory , const string& rKey , const bool& bLock);
	///bool GetLock(const string& rCategory , const string& rKey);
	virtual CELoadItemProp* Clone();
	virtual int Copy(CELoadItemProp* pCopied);
private:
	int ParseCategory( const string& rCategoryName , wifstream& ifile );
protected:
        CELoadItem* m_pOwner;
	vector<CItemPropCategory*>* m_pItemPropCategory;
public:
        int SetOwner(CELoadItem* pItem);
	int IsModified();
	int SetOriginalValueFromNewValue(CELoadItemProp* pCopied);
};
};