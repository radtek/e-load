
// ELOADDoc.h : CELOADDoc Ŭ������ �������̽�
//


#pragma once


class CELOADDoc : public CDocument
{
protected: // serialization������ ��������ϴ�.
	CELOADDoc();
	DECLARE_DYNCREATE(CELOADDoc)

// Ư���Դϴ�.
public:

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// �����Դϴ�.
public:
	virtual ~CELOADDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
        vector<CELoadItem*> m_ItemEntry;
protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
public:
        virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
};
