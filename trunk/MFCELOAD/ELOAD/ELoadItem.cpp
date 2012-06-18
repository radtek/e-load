#include "StdAfx.h"
#include "ELoad.h"
#include "ELoadItem.h"
#include "ELoadDocData.h"
#include "GUID.h"
#include <Assert.h>
#include "CableItem.h"

using namespace ITEMS;

//! SYSTEM에서 추가하는 필드들...
string CELoadItem::ADDED_FIELD_NAME[4]=
{
	"C_ID" , "C_REV" , "C_REV_DESC" , "C_DELETE"
};

CELoadItem::CELoadItem( ) : m_pProp(NULL) , m_nStatus(0)
{
	CGUID guid;
	m_prId = new string(guid.m_str.operator LPCSTR());
}

CELoadItem::~CELoadItem(void)
{
	try
	{
		if(m_prId) delete m_prId;
		if(NULL != m_pProp) SAFE_DELETE(m_pProp);
	}
	catch(...)
	{
	}
}


const char* CELoadItem::id() const
{
	return m_prId->c_str();
}

/**
	@brief	database에서 한 아이템에 대한 정보를 읽는다.

	@author BHK	

	@date 2009-05-19 오후 1:11:18	

	@param	

	@return		
*/
int CELoadItem::LoadData(CELoadItemProp* pItemProp , CADODB& adoDB , const int& nIndex)
{
	assert(pItemProp && "pItemProp is NULL");

	if(pItemProp)
	{
		CString rBusGroupName;
		const int nCategoryCount = pItemProp->GetCategoryCount();
		for(int j = 0;j < nCategoryCount;++j)
		{
			CItemPropCategory* pItemCategory = pItemProp->GetCategoryAt(j);
			const string rCategoryName = pItemCategory->name();
			const int nItemCount = pItemCategory->GetItemCount();
			for(int k = 0;k < nItemCount;++k)
			{
				CItemPropCategory::ItemPropTag* pItemTag = pItemCategory->GetItemAt(k);
				const string rKey = pItemTag->rKey;
				const CString _rKey = rKey.c_str();

				STRING_T rFieldName = rCategoryName + _T(",") + rKey;	//! ,로 category와 key값을 연결한다.
				STRING_T _rFieldValue;
				CString rFieldValue;
				if(_T("LONGBINARY") == pItemTag->rFieldType)
				{
					//! this field contains binary data.
					char *pData = NULL;
					long lSize = 0L;
					adoDB.GetFieldSize(nIndex , rFieldName.c_str() , lSize);
					if(pData = (char*)calloc(1 , sizeof(char)*(lSize + 1)))
					{
						adoDB.GetBinaryValue(nIndex , rFieldName.c_str() , (LPVOID)pData , lSize);
						rFieldValue = pData;
					}

					if(pData)
					{
						free((void*)pData);
						pData = NULL;
					}
				}
				else
				{
					adoDB.GetFieldValue(nIndex , rFieldName , &_rFieldValue);
					rFieldValue = _rFieldValue.c_str();
				}
				if( _T("-") == rFieldValue.Trim()) rFieldValue = _T("");

				prop()->SetOriginalValue( rCategoryName , rKey , rFieldValue.operator LPCSTR() , true);
				prop()->SetValue( rCategoryName , rKey , rFieldValue.operator LPCSTR() , true);
				if((_T("CHECKBOX") == pItemTag->rValueType) && rFieldValue.IsEmpty())
				{
					prop()->SetValue( rCategoryName , rKey , "No" );
					prop()->SetOriginalValue(rCategoryName , rKey , "No");
				}

				//! MULTI UNIT일 경우에는 SETTING된 값을 가져온다.
				if(pItemTag->GetUnitCount() > 1)
				{
					rFieldName += _T("_UNIT");
					if(TRUE == adoDB.GetFieldValue(nIndex , rFieldName , &_rFieldValue))
					{
						rFieldValue = _rFieldValue.c_str();
						if( _T("-") == rFieldValue.Trim()) rFieldValue = _T("");
					}else	rFieldValue.Empty();

					//! SETTING된 값이 없을 경우에는 MULTI UNIT중 맨 처음것으로 한다.
					if(rFieldValue.IsEmpty() || (_T("NONE") == rFieldValue)) rFieldValue = pItemTag->GetUnit(0).c_str();

					prop()->SetUnitStr( rCategoryName.c_str() , rKey.c_str() , rFieldValue.operator LPCSTR() );
				}

				//! "LOCK ITEM"일 경우 LOCK 정보를 가져온다.
				if(pItemTag->IsLockItem())
				{
					rFieldName += _T("_LOCK");
					if(TRUE == adoDB.GetFieldValue(nIndex , rFieldName.c_str() , &_rFieldValue))
					{
						rFieldValue = _rFieldValue.c_str();
						if( _T("-") == rFieldValue.Trim()) rFieldValue = _T("");
					}else	rFieldValue.Empty();

					//! 기본값은 FALSE이다.
					if(rFieldValue.IsEmpty() || (_T("NONE") == rFieldValue)) rFieldValue = _T("FALSE");

					prop()->SetLock( rCategoryName.c_str() , rKey.c_str() , (_T("TRUE") == rFieldValue) ? true : false);
				}
			}
		}

		const size_t nAddedCount = SIZE_OF_ARRAY(ADDED_FIELD_NAME);
		for(int j = 0;j < nAddedCount;++j)
		{
			STRING_T rFieldValue;
			adoDB.GetFieldValue(nIndex , ADDED_FIELD_NAME[j].c_str() , &rFieldValue);
			IsString::TrimWhiteSpace(rFieldValue);
			if( _T("-") == rFieldValue) rFieldValue = _T("");

			if(_T("C_ID") == ADDED_FIELD_NAME[j])
				SetId( rFieldValue );
			else if(_T("C_REV") == ADDED_FIELD_NAME[j])
			{
			}
			else if(_T("C_REV_DESC") == ADDED_FIELD_NAME[j])
			{
			}
			else if(_T("C_DELETE") == ADDED_FIELD_NAME[j])
			{
				SetDeleteFlag(("0" == rFieldValue) ? false : true);
			}
		}
		SetDataBaseItem();

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	아이템의 내용을 database에 저장한다.

	@author BHK	

	@date 2009-05-19 오후 3:41:04	

	@param	

	@return		
*/
int CELoadItem::SaveData(CADODB& adoDB , const string& rTableName)
{
	vector<pair<string,string> > LongBinaryFieldEntry;
	const int nCategoryCount = prop()->GetCategoryCount();
	CString rFieldSet , rValueSet, rUpdateSet;
	for(int i = 0 ;i < nCategoryCount;++i)
	{
		CItemPropCategory* pItemCategory = prop()->GetCategoryAt(i);
		const string rCategoryName = pItemCategory->name();
		const int nItemCount = pItemCategory->GetItemCount();
		for(int j = 0; j < nItemCount;j++)
		{
			CItemPropCategory::ItemPropTag* pItemTag = pItemCategory->GetItemAt(j);
			const string rKey = pItemTag->rKey;
			const string rFieldName = rCategoryName + _T(",") + rKey;
			if(_T("LONGBINARY") == pItemTag->rFieldType)
			{
				LongBinaryFieldEntry.push_back(make_pair(rCategoryName , rKey));
				continue;
			}

			string rValue = prop()->GetValue(rCategoryName , rKey);
			prop()->SetOriginalValue(rCategoryName , rKey , rValue);	//! original value를 저장한다.

			if(!rValue.empty())
			{
				rUpdateSet += ("[" + CString(rFieldName.c_str()) + "] ='" + CString(rValue.c_str()) + "',");
				rFieldSet += ( "[" + CString(rFieldName.c_str()) + "],");
				rValueSet += ( "'" + CString(rValue.c_str()) + "',");
			}
			else
			{
				rUpdateSet += ("[" + CString(rFieldName.c_str()) + "] ='',");
				rFieldSet += ( "[" + CString(rFieldName.c_str()) + "],");
				rValueSet += ( "'',");
			}

			//! MULTI UNIT일 경우...
			if(pItemTag->GetUnitCount() > 1)
			{
				rValue = pItemTag->rCurUnit;
				if(!rValue.empty())
				{
					rUpdateSet += ("[" + CString(rFieldName.c_str()) + "_UNIT] ='" + CString(rValue.c_str()) + "',");
					rFieldSet += ( "[" + CString(rFieldName.c_str()) + "_UNIT],");
					rValueSet += ( "'" + CString(rValue.c_str()) + "',");
				}
				else
				{
					rUpdateSet += ("[" + CString(rFieldName.c_str()) + "_UNIT] ='',");
					rFieldSet += ( "[" + CString(rFieldName.c_str()) + "_UNIT],");
					rValueSet += ( "'',");
				}
			}

			//! LOCK ITEM일 경우...
			if(pItemTag->IsLockItem())
			{
				rValue = pItemTag->rCurLock;
				if(!rValue.empty())
				{
					rUpdateSet += ("[" + CString(rFieldName.c_str()) + "_LOCK] ='" + CString(rValue.c_str()) + "',");
					rFieldSet += ( "[" + CString(rFieldName.c_str()) + "_LOCK],");
					rValueSet += ( "'" + CString(rValue.c_str()) + "',");
				}
				else
				{
					rUpdateSet += ("[" + CString(rFieldName.c_str()) + "_LOCK] ='',");
					rFieldSet += ( "[" + CString(rFieldName.c_str()) + "_LOCK],");
					rValueSet += ( "'',");
				}
			}
		}
	}
	int at = rFieldSet.ReverseFind(',');
	rFieldSet = rFieldSet.Left(at);
	at = rValueSet.ReverseFind(',');
	rValueSet = rValueSet.Left(at);
	at = rUpdateSet.ReverseFind(',');
	rUpdateSet = rUpdateSet.Left(at);

	CString rQuery;
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//! CABLE ITEM은 특수하게 처리한다.(저장하기 전에 기존의 모든 데이터들을 삭제했음으로 무조건 INSERT한다.)
	if(GetTypeString() == CCableItem::TypeString())
	{
		rQuery = "INSERT INTO " + CString(rTableName.c_str()) + " ( ";
		rQuery += rFieldSet + ",[C_ID]";
		rQuery += ") VALUES ( " + rValueSet + ",'" + CString(GetId().c_str()) + "');";
	}
	else
	{
		if(!IsDataBaseItem() && !IsDeleted())
		{
			rQuery = "INSERT INTO " + CString(rTableName.c_str()) + " ( ";
			rQuery += rFieldSet + ",[C_ID]";
			rQuery += ") VALUES ( " + rValueSet + ",'" + CString(GetId().c_str()) + "');";

			SetDataBaseItem();	//! 이제 데이터베이스에 저장된 아이템이 된다.
		}
		else
		{
			rQuery = _T("UPDATE ") + CString(rTableName.c_str()) + _T(" SET ") + rUpdateSet + _T(" WHERE C_ID='") + CString(GetId().c_str()) + _T("'");
		}
	}

	try
	{
		adoDB.ExecuteQuery(rQuery.operator LPCTSTR());
	}
	catch(_com_error & e)
	{
		CString rMsg;
		rMsg.Format("*** DOG CHECK [ADO DB] => <ExecuteQuery 오류> 설명[%s] !! ***\n", (LPCSTR)e.Description());
		ELOAD_LOG4CXX_ERROR(mylogger , "" , 2);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	Binary 아이템의 내용을 database에 저장한다.

	@author BHK	

	@date 2009-05-19 오후 3:41:04	

	@param	

	@return		
*/
int CELoadItem::SaveBinaryData(CADODB& adoDB , const string& rTableName)
{
	vector<pair<string,string> > LongBinaryFieldEntry;
	const int nCategoryCount = prop()->GetCategoryCount();
	CString rFieldSet , rValueSet, rUpdateSet;
	for(int i = 0 ;i < nCategoryCount;i++)
	{
		CItemPropCategory* pItemCategory = prop()->GetCategoryAt(i);
		const string rCategoryName = pItemCategory->name();
		const int nItemCount = pItemCategory->GetItemCount();
		for(int j = 0; j < nItemCount;j++)
		{
			CItemPropCategory::ItemPropTag* pItemTag = pItemCategory->GetItemAt(j);
			const string rKey = pItemTag->rKey;
			const string rFieldName = rCategoryName + _T(",") + rKey;
			if(_T("LONGBINARY") != pItemTag->rFieldType) continue;
			
			const CString rSql = string("SELECT [" + rFieldName + "] FROM " + rTableName + " WHERE C_ID='" + GetId() + "'").c_str();
			const string rValue = prop()->GetValue(rCategoryName , rKey);
			prop()->SetOriginalValue(rCategoryName , rKey , rValue);	//! original value를 저장한다.

			LONG lRecordCount = 0L;
			adoDB.OpenQuery(rSql.operator LPCTSTR());
			adoDB.GetRecordCount(&lRecordCount);
			if((1 == lRecordCount) && !rValue.empty())
			{
				adoDB.AppendChunk(0 , 0 , (LPVOID)(rValue.c_str()) , rValue.length());
			}
			adoDB.CloseRecordSet();
		}
	}

	return ERROR_SUCCESS;
}
/**
	@brief	

	@author	BHK
*/
bool CELoadItem::IsKindOf(const char* typeString) const
{
	if(0 == strcmp(CELoadItem::TypeString() , typeString)) return true;
	return false;
}

/**
	@brief	

	@author	BAEK HUM KYUNG
*/
int CELoadItem::Copy(CELoadItem* pCopied)
{
	assert(pCopied && "pCopied is NULL");

	if(pCopied)
	{
		///*(m_prId) = *(pCopied->m_prId);
                SetProp(pCopied->prop()->Clone());

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

string CELoadItem::GetTypeString()
{
	return CELoadItem::TypeString();
}

/**
	@brief	

	@author	BHK
*/
const char* CELoadItem::TypeString()
{
	static const char* typeString="";
	return typeString;
}

/**
	@brief	삭제된 아이템 여부를 리턴한다.

	@author	BHK
*/
bool CELoadItem::IsDeleted(void) const
{
	return (CELoadItem::DELETED_ITEM & m_nStatus) ? true : false;
}

/**
	@brief	

	@author	BHK
*/
int CELoadItem::SetDeleteFlag(const bool& bDelete)
{
	if(true == bDelete)
		m_nStatus |= CELoadItem::DELETED_ITEM;
	else	m_nStatus &= ~CELoadItem::DELETED_ITEM;
	
	return ERROR_SUCCESS;
}

/**
	@brief	property of item is modified?

	@author	BHK

	@return	 true when modified, otherwise false
*/
bool CELoadItem::IsModified() const 
{
	return (m_nStatus & CELoadItem::MODIFIED_ITEM) ? true : false;
}

/**
	@brief	

	@author	BHK

	@return	
*/
int CELoadItem::SetModifiedFlag(const bool& bModifiedFlag)
{
	if(true == bModifiedFlag)
		m_nStatus |= CELoadItem::MODIFIED_ITEM;
	else	m_nStatus &= ~CELoadItem::MODIFIED_ITEM;

	return ERROR_SUCCESS;
}

/**
	@brief	property of item is modified?

	@author	BHK

	@return	 true when modified, otherwise false
*/
bool CELoadItem::IsDataBaseItem() const 
{
	return (m_nStatus & CELoadItem::DATABASE_ITEM);
}

/**
	@brief	

	@author	BHK

	@return	
*/
int CELoadItem::SetDataBaseItem()
{
	m_nStatus |= CELoadItem::DATABASE_ITEM;

	return ERROR_SUCCESS;
}

/**
	@brief	item is Not deleted?

	@param	pItem

	@author	BHK
*/
bool CELoadItem::IsNotDeleted(CELoadItem* pItem)
{
	assert(pItem && "pItem is NULL");

	if(pItem) return (!pItem->IsDeleted());

	return false;
}