#include "StdAfx.h"
#include "GeneratorItem.h"
#include "ELoadDocData.h"

using namespace ITEMS;

IMPLEMENTS_ELOAD_ITEM(CGeneratorItem , CELoadItem , "GENERATOR")

CGeneratorItem::CGeneratorItem(void)
{
	CELoadDocData& docData = CELoadDocData::GetInstance();

	CELoadItemProp* pItemProp = docData.m_ItemPropMap[CGeneratorItem::TypeString()];
	if(pItemProp)
	{
		m_pProp = pItemProp->Clone();
		m_pProp->SetOwner(this);

		////! 필요한 속성 초기화 부분
		m_pProp->SetOriginalValue(_T("PG1") , _T("L") , _T("1"));
		m_pProp->SetValue(_T("PG1") , _T("L") , _T("1"));
		m_pProp->SetOriginalValue(_T("PG1") , _T("COS θG") , _T("0.8"));
		m_pProp->SetValue(_T("PG1") , _T("COS θG") , _T("0.8"));

		m_pProp->SetOriginalValue(_T("PG2") , _T("V") , _T("0.15"));
		m_pProp->SetValue(_T("PG2") , _T("V") , _T("0.15"));
		m_pProp->SetOriginalValue(_T("PG2") , _T("Xd1") , _T("0.26"));
		m_pProp->SetValue(_T("PG2") , _T("Xd1") , _T("0.26"));
		m_pProp->SetOriginalValue(_T("PG2") , _T("Xd2") , _T("0.16"));
		m_pProp->SetValue(_T("PG2") , _T("Xd2") , _T("0.16"));

		m_pProp->SetOriginalValue(_T("PG3") , _T("CosQL") , _T("0.25"));
		m_pProp->SetValue(_T("PG3") , _T("CosQL") , _T("0.25"));
		m_pProp->SetOriginalValue(_T("PG3") , _T("K") , _T("1.2"));
		m_pProp->SetValue(_T("PG3") , _T("K") , _T("1.2"));
		m_pProp->SetOriginalValue(_T("PG3") , _T("COS θG") , _T("0.8"));
		m_pProp->SetValue(_T("PG3") , _T("COS θG") , _T("0.8"));

	}
	else
	{
		AfxMessageBox(_T("Generator Property를 구할수 없습니다."));
	}
}

CGeneratorItem::~CGeneratorItem(void)
{
}

/**
	@brief

	@author	KHS
*/
int CGeneratorItem::SetName( const string& name)
{
	CELoadItemProp* pProp = prop();
	if(pProp) return pProp->SetValue("General" , "GENERATOR ID" , name);
        
        return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief

	@author	KHS
*/
string CGeneratorItem::TableName()
{
	static const string __TABLE_NAME__ = _T("T_GENERATOR_ITEM");

	return __TABLE_NAME__;
}

/**
	@brief

	@author	KHS
*/
string CGeneratorItem::GetName() const
{
	CELoadItemProp* pProp = prop();
	if(pProp) return pProp->GetValue("General" , "GENERATOR ID");
        
        return _T("");
}
