
// ELOADDoc.cpp : CELOADDoc Ŭ������ ����
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


// CELOADDoc ����/�Ҹ�

CELOADDoc::CELOADDoc()
{
	// TODO: ���⿡ ��ȸ�� ���� �ڵ带 �߰��մϴ�.

}

CELOADDoc::~CELOADDoc()
{
}

BOOL CELOADDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: ���⿡ ���ʱ�ȭ �ڵ带 �߰��մϴ�.
	// SDI ������ �� ������ �ٽ� ����մϴ�.
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
		// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	}
	else
	{
		// TODO: ���⿡ �ε� �ڵ带 �߰��մϴ�.
	}
}


// CELOADDoc ����

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


// CELOADDoc ���

BOOL CELOADDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
        ///if (!CDocument::OnOpenDocument(lpszPathName))
        ///        return FALSE;

        // TODO:  Add your specialized creation code here

        return TRUE;
}
