#include "StdAfx.h"
#include "ELoadItemProp.h"
#include "Tokenizer.h"
#include "IsString.h"
#include "ELoadItem.h"
#include <fstream>
#include <Assert.h>

using namespace ITEMS;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CItemPropCategory::ItemPropTag::IsLocked() const
{
	return (((_T("LOCK ITEM") == rLockFlag) || (_T("GROUP LOCK ITEM") == rLockFlag)) && (_T("TRUE") == rCurLock));
}

bool CItemPropCategory::ItemPropTag::IsUnLocked() const
{
	return (((_T("LOCK ITEM") == rLockFlag) || (_T("GROUP LOCK ITEM") == rLockFlag)) && (_T("FALSE") == rCurLock));
}

/**
	@brief	

	@author	BHK
*/
bool CItemPropCategory::ItemPropTag::IsGroupLock() const
{
	return ((_T("GROUP LOCK") == rLockFlag));
}

/**
	@brief	

	@author	BHK
*/
bool CItemPropCategory::ItemPropTag::IsLockItem() const
{
	return ((_T("LOCK ITEM") == rLockFlag));
}

/**
	@brief	

	@author	BHK
*/
bool CItemPropCategory::ItemPropTag::IsGroupLockItem() const
{
	return ((_T("GROUP LOCK ITEM") == rLockFlag));
}

/**
	@brief	property의 값을 설정합니다.

	@author	BHK
*/
int CItemPropCategory::SetValue( const string& rKey , const string& rValue , const bool& bIgnoreLock)
{
	for(vector<ItemPropTag*>::iterator itr = m_pItemValueEntry->begin();itr != m_pItemValueEntry->end();++itr)
	{
		CString rTemp = (*itr)->rKey.c_str();
		CString rKeyText  = rKey.c_str();
		if(rKeyText.MakeUpper() == rTemp.MakeUpper())
		{
			//! LOCK이 걸리지 않았을 경우에만 값을 대입한다.
			if((true == bIgnoreLock) || (false == (*itr)->IsLocked()))
			{
				(*itr)->rValue = rValue;
				
				if(_T("GROUP LOCK") == (*itr)->rLockFlag)
				{
					//! GROUP LOCK일 경우에는 하위의 GROUP LOCK ITEM의 값을 자동으로 갱신한다.
					CString _rValue = rValue.c_str();
					_rValue.MakeUpper();
					for(++itr;itr != m_pItemValueEntry->end();++itr)
					{
						if(_T("GROUP LOCK ITEM") == (*itr)->rLockFlag)
						{
							(*itr)->rCurLock = (_T("YES") == _rValue) ?  _T("TRUE") : _T("FALSE");
						}else	break;
					}
				}
			}

			return ERROR_SUCCESS;
		}
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	property의 원본 값을 설정합니다.

	@author	BHK
*/
int CItemPropCategory::SetOriginalValue( const string& rKey , const string& rOriginalValue , const bool& bIgnoreLock)
{
	for(vector<ItemPropTag*>::iterator itr = m_pItemValueEntry->begin();itr != m_pItemValueEntry->end();++itr)
	{
		CString rTemp = (*itr)->rKey.c_str();
		CString rKeyText  = rKey.c_str();
		if(rKeyText.MakeUpper() == rTemp.MakeUpper())
		{
			//! LOCK이 걸리지 않았을 경우에만 값을 대입한다.
			if((true == bIgnoreLock) || (false == (*itr)->IsLocked())) (*itr)->rOriginalValue = rOriginalValue;
			return ERROR_SUCCESS;
		}
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	

	@author	BHK
*/
bool CItemPropCategory::IsModified( const string& rKey ) const
{
	if(!rKey.empty())
	{
		for(vector<ItemPropTag*>::const_iterator itr = m_pItemValueEntry->begin();itr != m_pItemValueEntry->end();++itr)
		{
			CString rTemp = (*itr)->rKey.c_str();
			CString rKeyText = rKey.c_str();
			if(rKeyText.MakeUpper() == rTemp.MakeUpper()) return (*itr)->IsModified();
		}
	}
	else
	{
		for(vector<ItemPropTag*>::const_iterator itr = m_pItemValueEntry->begin();itr != m_pItemValueEntry->end();++itr)
		{
			if((*itr)->IsModified()) return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
	@brief	

	@author	BHK
*/
CItemPropCategory::ItemPropTag* CItemPropCategory::GetItemPropTag(const string& rKey)
{
	CString rKeyText = rKey.c_str();
	for(vector<ItemPropTag*>::const_iterator itr = m_pItemValueEntry->begin();itr != m_pItemValueEntry->end();++itr)
	{
		CString rTemp = (*itr)->rKey.c_str();
                if(rKeyText.MakeUpper() == rTemp.MakeUpper()) return (*itr);
	}

	return NULL;
}

string CItemPropCategory::GetItemNameAt( const size_t& at ) const
{
	assert((at < m_pItemValueEntry->size()) && "range error");

	if(at < m_pItemValueEntry->size())
	{
		ItemPropTag* p = (*m_pItemValueEntry)[at];
		return p->rKey;
	}

	return "";
}

CItemPropCategory::ItemPropTag* CItemPropCategory::GetItemAt( const size_t& at ) const
{
	assert((at < m_pItemValueEntry->size()) && "range error");

	if(at < m_pItemValueEntry->size()) return (*m_pItemValueEntry)[at];

	return NULL;
}

size_t CItemPropCategory::GetItemCount() const
{
	return m_pItemValueEntry->size();
}

/**
	@brief	CItemPropCategory의 복사본을 생성한다.

	@author	BHK
*/
CItemPropCategory* CItemPropCategory::Clone()
{
	CItemPropCategory* res = new CItemPropCategory(*m_pName);
	if(res)
	{
		for(vector<ItemPropTag*>::iterator itr = m_pItemValueEntry->begin();itr != m_pItemValueEntry->end();++itr)
		{
			ItemPropTag* ptr = (ItemPropTag*)calloc(1 , sizeof(ItemPropTag));
			if(ptr)
			{
				ptr->pParent        = res;

				ptr->rDesc		= (*itr)->rDesc;
				ptr->rKey		= (*itr)->rKey;
				ptr->rOriginalValue	= (*itr)->rOriginalValue;
				ptr->rValue		= (*itr)->rValue;
				ptr->rValueType		= (*itr)->rValueType;
				ptr->rAccess		= (*itr)->rAccess;
				ptr->rUnitList		= (*itr)->rUnitList;
				ptr->rCurUnit		= (*itr)->rCurUnit;
				ptr->rLockFlag		= (*itr)->rLockFlag;
				ptr->rCurLock		= (*itr)->rCurLock;
				ptr->rFieldType		= (*itr)->rFieldType;
			
				res->m_pItemValueEntry->push_back(ptr);
			}
		}
	}

	return res;
}

/**
	@brief	CItemPropCategory를 복사한다.

	@author	BHK
*/
int CItemPropCategory::Copy(CItemPropCategory* rhs)
{
	assert(rhs && "rhs is NULL");

	if(rhs && (m_pItemValueEntry->size() == rhs->m_pItemValueEntry->size()))
	{
		(*m_pName) = (*rhs->m_pName);
		(*m_pAttr) = (*rhs->m_pAttr);

		int nIndex = 0;
		for(vector<ItemPropTag*>::iterator itr = m_pItemValueEntry->begin();itr != m_pItemValueEntry->end();++itr,++nIndex)
		{
			ItemPropTag* pItemTag = rhs->m_pItemValueEntry->at(nIndex);

			(*itr)->pParent        = pItemTag->pParent;

			(*itr)->rDesc		= pItemTag->rDesc;
			(*itr)->rKey		= pItemTag->rKey;
			(*itr)->rOriginalValue	= pItemTag->rOriginalValue;
			(*itr)->rValue		= pItemTag->rValue;
			(*itr)->rValueType	= pItemTag->rValueType;
			(*itr)->rAccess		= pItemTag->rAccess;
			(*itr)->rUnitList	= pItemTag->rUnitList;
			(*itr)->rCurUnit	= pItemTag->rCurUnit;
			(*itr)->rLockFlag	= pItemTag->rLockFlag;
			(*itr)->rCurLock	= pItemTag->rCurLock;
			(*itr)->rFieldType	= pItemTag->rFieldType;
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author	BHK
*/
int CItemPropCategory::SetUnitStr( const string& rKey , const string& rUnitStr )
{
	for(vector<ItemPropTag*>::const_iterator itr = m_pItemValueEntry->begin();itr != m_pItemValueEntry->end();++itr)
	{
		CString rTemp = (*itr)->rKey.c_str();
		CString rKeyText = rKey.c_str();
		if(rKeyText.MakeUpper() == rTemp.MakeUpper())
		{
			(*itr)->rCurUnit = rUnitStr;
			return ERROR_SUCCESS;
		}
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	lock을 설정한다.

	@author	BHK
*/
int CItemPropCategory::SetLock( const string& rKey , const bool& bLock )
{
	for(vector<ItemPropTag*>::const_iterator itr = m_pItemValueEntry->begin();itr != m_pItemValueEntry->end();++itr)
	{
		CString rTemp = (*itr)->rKey.c_str();
		CString rKeyText = rKey.c_str();
		if(rKeyText.MakeUpper() == rTemp.MakeUpper())
		{
			(*itr)->rCurLock = (true == bLock) ? _T("TRUE") : _T("FALSE");
			return ERROR_SUCCESS;
		}
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	original value를 value로 설정한다.

	@author	BHK
*/
int CItemPropCategory::CommitModifiedValues()
{
	for(vector<ItemPropTag*>::iterator itr = m_pItemValueEntry->begin();itr != m_pItemValueEntry->end();++itr)
	{
		(*itr)->rOriginalValue = (*itr)->rValue;
	}

	return ERROR_SUCCESS;
}

/**
	@brief	value를 original value로 설정한다.

	@author	BHK
*/
int CItemPropCategory::ResetModifiedValues()
{
	for(vector<ItemPropTag*>::iterator itr = m_pItemValueEntry->begin();itr != m_pItemValueEntry->end();++itr)
	{
		(*itr)->rValue = (*itr)->rOriginalValue;
	}

	return ERROR_SUCCESS;
}

/**
	@brief	Lock여부를 리턴한다.

	@author	BHK
*/
bool CItemPropCategory::IsLocked( const string& rKey ) const
{
	for(vector<ItemPropTag*>::const_iterator itr = m_pItemValueEntry->begin();itr != m_pItemValueEntry->end();++itr)
	{
		CString rTemp = (*itr)->rKey.c_str();
		CString rKeyText = rKey.c_str();
		if(rKeyText.MakeUpper() == rTemp.MakeUpper()) return (*itr)->IsLocked();
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CELoadItemProp::CELoadItemProp(CELoadItem* pOwner) : m_pOwner(pOwner)
{
	m_pItemPropCategory = new vector<CItemPropCategory*>;
}

CELoadItemProp::~CELoadItemProp(void)
{
	try
	{
		for(vector<CItemPropCategory*>::iterator itr = m_pItemPropCategory->begin();itr != m_pItemPropCategory->end();++itr)
		{
			SAFE_DELETE(*itr);
		}
		m_pItemPropCategory->clear();
		SAFE_DELETE(m_pItemPropCategory);
	}
	catch(...)
	{
	}
}

/**
	@brief	parse category section.

	@author	humkyung

	@para	rCategoryName = 
	@param	ifile = 
*/
int CELoadItemProp::ParseCategory( const string& rCategoryName , wifstream& ifile )
{
	if(ifile.is_open())
	{
		wstring waLine;
		getline(ifile , waLine);
		if(ifile.eof()) return ERROR_BAD_ENVIRONMENT;

		STRING_T aLine = IsString::wcs_to_mbs(waLine);
                IsString::TrimWhiteSpace(aLine);
		if('{' != aLine[0]) return ERROR_BAD_ENVIRONMENT;
		
		vector<STRING_T> oResult;
		CTokenizer<CIsFromString>::Tokenize(oResult , rCategoryName.c_str() , CIsFromString(_T(":")));		
		CItemPropCategory* pCategory = (2 == oResult.size()) ? new CItemPropCategory(oResult[0] , oResult[1]) : new CItemPropCategory(oResult[0]);
		while(!ifile.eof())
		{
			getline(ifile , waLine);
			if(ifile.eof()) break;
			
			aLine = IsString::wcs_to_mbs(waLine);
			IsString::TrimWhiteSpace(aLine);
			if(aLine.empty()) continue;
			if(_T("}") == aLine) break;

			CString rStr(aLine.c_str());
			rStr.Trim();
			if(rStr.IsEmpty()) continue;
			pCategory->ParseItemString( rStr.operator LPCSTR() );
		}

		if(pCategory) m_pItemPropCategory->push_back(pCategory);

		return ERROR_SUCCESS;
	}

	return ERROR_FILE_NOT_FOUND;
}

/**
        @brief  Read '*.PROP' file

        @author	BHK
*/
int CELoadItemProp::Read(const string& rFilePath)
{
	//! 한글 파일명 파일을 읽기 위해...(앞서 setlocale(LC_ALL , "Korean")으로 설정해 줬음.)
	wifstream wifile(rFilePath.c_str());

	if(wifile.is_open())
	{
		wstring waLine;
		while(wifile.good() && !wifile.eof())
		{
			getline(wifile, waLine);
			if(wifile.eof()) break;
			if(waLine.empty()) continue;

			string aLine = IsString::wcs_to_mbs(waLine);
			ParseCategory( aLine , wifile );
		}

		wifile.close();
		return ERROR_SUCCESS;
	}
	else
	{
		throw exception(string("Not found, " + rFilePath ).c_str());

		return ERROR_FILE_NOT_FOUND;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
        @brief  Category 와 Key 값으로 Value를 찾아 리턴한다.
        @author	BHK
*/
string CELoadItemProp::GetValue(const string& rCategory , const string& rKey) const
{
	for(vector<CItemPropCategory*>::const_iterator itr = m_pItemPropCategory->begin();itr != m_pItemPropCategory->end();++itr)
	{
                CString rCategoryName = (*itr)->name();
                CString rCategoryText = rCategory.c_str();
                if(rCategoryText.MakeUpper() == rCategoryName.MakeUpper())
		{
			string rValue = (*itr)->GetValue(rKey);
			return rValue;
		}
	}

	return string("");
}

/**
        @brief  Category 와 Key 값으로 OriginValue를 찾아 리턴한다.

        @author	BHK
*/
string CELoadItemProp::GetOriginalValue(const string& rCategory , const string& rKey) const
{
	for(vector<CItemPropCategory*>::const_iterator itr = m_pItemPropCategory->begin();itr != m_pItemPropCategory->end();++itr)
	{
                CString rCategoryName = (*itr)->name();
                CString rCategoryText = rCategory.c_str();
                if(rCategoryText.MakeUpper() == rCategoryName.MakeUpper())
		{
			string rValue = (*itr)->GetOriginalValue(rKey);
			return rValue;
		}
	}

	return string("");
}

/**
        @brief  Category 와 Key 값으로 UNIT를 찾아 리턴한다.

        @author	BHK
*/
string CELoadItemProp::GetUnitStr(const string& rCategory , const string& rKey) const
{
	for(vector<CItemPropCategory*>::const_iterator itr = m_pItemPropCategory->begin();itr != m_pItemPropCategory->end();++itr)
	{
                CString rCategoryName = (*itr)->name();
                CString rCategoryText = rCategory.c_str();
                if(rCategoryText.MakeUpper() == rCategoryName.MakeUpper())
		{
			string rValue = (*itr)->GetUnitStr(rKey);
			return rValue;
		}
	}

	return string("");
}

/**
        @brief  Category 와 Key 값에 해당하는 ITEM의 UNIT를 설정한다.

        @author	BHK
*/
int CELoadItemProp::SetUnitStr( const string& rCategory , const string& rKey , const string& rUnitStr )
{
	for(vector<CItemPropCategory*>::const_iterator itr = m_pItemPropCategory->begin();itr != m_pItemPropCategory->end();++itr)
	{
                CString rCategoryName = (*itr)->name();
                CString rCategoryText = rCategory.c_str();
                if(rCategoryText.MakeUpper() == rCategoryName.MakeUpper())
		{
			return (*itr)->SetUnitStr( rKey , rUnitStr );
		}
	}

	return ERROR_INVALID_PARAMETER;
}

/**
        @brief  Category 와 Key 값으로 DATA TYPE를 찾아 리턴한다.

        @author	BHK
*/
string CELoadItemProp::GetDataType(const string& rCategory , const string& rKey) const
{
	for(vector<CItemPropCategory*>::const_iterator itr = m_pItemPropCategory->begin();itr != m_pItemPropCategory->end();++itr)
	{
                CString rCategoryName = (*itr)->name();
                CString rCategoryText = rCategory.c_str();
                if(rCategoryText.MakeUpper() == rCategoryName.MakeUpper())
		{
			string rValue = (*itr)->GetValueType(rKey);
			return rValue;
		}
	}

	return string("");
}

/**
	@brief	property의 값을 설정합니다.

	@author	BHK
*/
int CELoadItemProp::SetValue( const string& rCategory , const string& rKey , const string& rValue , const bool& bIgnoreLock)
{
	for(vector<CItemPropCategory*>::const_iterator itr = m_pItemPropCategory->begin();itr != m_pItemPropCategory->end();++itr)
	{
                CString rCategoryName = (*itr)->name();
                CString rCategoryText = rCategory.c_str();
                if(rCategoryText.MakeUpper() == rCategoryName.MakeUpper().operator LPCSTR())
		{
			return (*itr)->SetValue( rKey , rValue , bIgnoreLock);
		}
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	original 값을 설정한다.

	@author	BHK
*/
int CELoadItemProp::SetOriginalValue( const string& rCategory , const string& rKey , const string& rOriginalValue , const bool& bIgnoreLock)
{
	for(vector<CItemPropCategory*>::const_iterator itr = m_pItemPropCategory->begin();itr != m_pItemPropCategory->end();++itr)
	{
                CString rCategoryName = (*itr)->name();
                CString rCategoryText = rCategory.c_str();
                if(rCategoryText.MakeUpper() == rCategoryName.MakeUpper())
		{
			return (*itr)->SetOriginalValue( rKey , rOriginalValue , bIgnoreLock);
		}
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	

	@author	BHK
*/
size_t CELoadItemProp::GetCategoryCount() const
{
	return m_pItemPropCategory->size();
}

/**
	@brief	

	@author	BHK
*/
CItemPropCategory* CELoadItemProp::GetCategoryAt( const size_t& at ) const
{
	assert((at < m_pItemPropCategory->size()) && "range error");

	if(at < m_pItemPropCategory->size() ) return (*m_pItemPropCategory)[at];

	return NULL;
}

/**
	@brief	이름으로 Category를 찾는다.

	@author	BHK
*/
CItemPropCategory* CELoadItemProp::GetCategoryByName( const string& rName ) const
{
	for(vector<CItemPropCategory*>::const_iterator itr = m_pItemPropCategory->begin();itr != m_pItemPropCategory->end();++itr)
	{
		if(CString(rName.c_str()).MakeUpper() == CString((*itr)->name()).MakeUpper()) return (*itr);
	}

	return NULL;
}

/**
*/
CELoadItemProp* CELoadItemProp::Clone()
{
	CELoadItemProp* res = new CELoadItemProp;
	if(res)
	{
		for(vector<CItemPropCategory*>::iterator itr = m_pItemPropCategory->begin();itr != m_pItemPropCategory->end();++itr)
		{
			CItemPropCategory* pCategory = (*itr)->Clone();
			if(pCategory) res->m_pItemPropCategory->push_back( pCategory );
		}
	}

	return res;
}

/**
	@brief	

	@author	BHK
*/
int CELoadItemProp::Copy(CELoadItemProp* pCopied)
{
	assert(pCopied && "pCopied is NULL");

	if(pCopied && (m_pItemPropCategory->size() == pCopied->m_pItemPropCategory->size()))
	{
		m_pOwner = pCopied->m_pOwner;

		int nIndex = 0;
		for(vector<CItemPropCategory*>::iterator itr = m_pItemPropCategory->begin();itr != m_pItemPropCategory->end();++itr,++nIndex)
		{
			(*itr)->Copy(pCopied->m_pItemPropCategory->at(nIndex));
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author	BHK
*/
int CELoadItemProp::SetOwner(CELoadItem* pItem)
{
        m_pOwner = pItem;
        return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
int CELoadItemProp::IsModified()
{
	if(m_pItemPropCategory)
	{
		for(vector<CItemPropCategory*>::iterator itr = m_pItemPropCategory->begin();itr != m_pItemPropCategory->end();++itr)
		{
			if((*itr)->IsModified("")) return true;
		}
	}

	return false;
}

/**
	@brief	origianl value를 value로 설정한다.

	@author	BHK
*/
int CELoadItemProp::CommitModifiedValues()
{
	if(m_pItemPropCategory)
	{
		for(vector<CItemPropCategory*>::iterator itr = m_pItemPropCategory->begin();itr != m_pItemPropCategory->end();++itr)
		{
			(*itr)->CommitModifiedValues();
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	value를 origianl value로 설정한다.

	@author	BHK
*/
int CELoadItemProp::ResetModifiedValues()
{
	if(m_pItemPropCategory)
	{
		for(vector<CItemPropCategory*>::iterator itr = m_pItemPropCategory->begin();itr != m_pItemPropCategory->end();++itr)
		{
			(*itr)->ResetModifiedValues();
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	BHK
*/
int CELoadItemProp::SetLock(const string& rCategory , const string& rKey , const bool& bLock)
{
	for(vector<CItemPropCategory*>::const_iterator itr = m_pItemPropCategory->begin();itr != m_pItemPropCategory->end();++itr)
	{
                CString rCategoryName = (*itr)->name();
                CString rCategoryText = rCategory.c_str();
                if(rCategoryText.MakeUpper() == rCategoryName.MakeUpper())
		{
			return (*itr)->SetLock( rKey , bLock );
		}
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	DB 에 저장하지 않고 VALUE를 ORIGINAL VALUE에 저장

	@author KHS	

	@date 2009-07-08 오후 4:05:38	

	@param	

	@return		
**/
int ITEMS::CELoadItemProp::SetOriginalValueFromNewValue(CELoadItemProp* pCopied)
{
	assert(pCopied && "pCopied is NULL");

	if(pCopied && (m_pItemPropCategory->size() == pCopied->m_pItemPropCategory->size()))
	{
		m_pOwner = pCopied->m_pOwner;

		int nIndex = 0;
		for(vector<CItemPropCategory*>::iterator itr = m_pItemPropCategory->begin();itr != m_pItemPropCategory->end();++itr,++nIndex)
		{
			(*itr)->SetOriginalValueFromNewValue(pCopied->m_pItemPropCategory->at(nIndex));
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author KHS	

	@date 2009-07-08 오후 4:08:51	

	@param	

	@return		
**/
int ITEMS::CItemPropCategory::SetOriginalValueFromNewValue(CItemPropCategory* rhs)
{
	assert(rhs && "rhs is NULL");

	if(rhs && (m_pItemValueEntry->size() == rhs->m_pItemValueEntry->size()))
	{
		(*m_pName) = (*rhs->m_pName);
		(*m_pAttr) = (*rhs->m_pAttr);

		int nIndex = 0;
		for(vector<ItemPropTag*>::iterator itr = m_pItemValueEntry->begin();itr != m_pItemValueEntry->end();++itr,++nIndex)
		{
			ItemPropTag* pItemTag = rhs->m_pItemValueEntry->at(nIndex);

			(*itr)->rOriginalValue	= pItemTag->rValue;
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}
