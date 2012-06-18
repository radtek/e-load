
// ELOADDoc.cpp : CELOADDoc 클래스의 구현
//

#include "stdafx.h"
#include "ELOAD.h"

#include "ELOADDoc.h"
#include "ELoadDocData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CELOADDoc

IMPLEMENT_DYNCREATE(CELOADDoc, CDocument)

BEGIN_MESSAGE_MAP(CELOADDoc, CDocument)
END_MESSAGE_MAP()


// CELOADDoc 생성/소멸

CELOADDoc::CELOADDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CELOADDoc::~CELOADDoc()
{
}

BOOL CELOADDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.
	m_ItemEntry.clear();
	CELoadDocData& docData = CELoadDocData::GetInstance();	
	m_ItemEntry.insert(m_ItemEntry.begin() , docData.m_SelectedItemEntry.begin() , docData.m_SelectedItemEntry.end());
	return TRUE;
}


// CELOADDoc serialization

void CELOADDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}


// CELOADDoc 진단

#ifdef _DEBUG
void CELOADDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CELOADDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CELOADDoc 명령

BOOL CELOADDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
        ///if (!CDocument::OnOpenDocument(lpszPathName))
        ///        return FALSE;

        // TODO:  Add your specialized creation code here

        return TRUE;
}
