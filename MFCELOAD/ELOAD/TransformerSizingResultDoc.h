#pragma once

#include <sstream>
using namespace std;

// CTransformerSizingResultDoc document

class CTransformerSizingResultDoc : public CDocument
{
	DECLARE_DYNCREATE(CTransformerSizingResultDoc)

public:
	typedef struct
	{
		STRING_T rTRID , rPriBusID , rPriBusVolt;
		STRING_T rSecBusID , rSecBusVolt;
		STRING_T rKW , rKVAR , rKVA;
		STRING_T rCalcualtedCapacity , rSelectedCapacity , rSpare;
		STRING_T rRemark;
		
		int Reset()
		{
			rTRID = _T("");
			rPriBusID = _T("");
			rPriBusVolt = _T("");
			rSecBusID = _T("");
			rSecBusVolt = _T("");
			rKW = _T("");
			rKVAR = _T("");
			rKVA = _T("");
			rCalcualtedCapacity = _T("");
			rSelectedCapacity = _T("");
			rSpare = _T("");
			rRemark = _T("");

			return ERROR_SUCCESS;
		}
		
		int Write(ofstream& ifile)
		{
			ifile << rTRID.c_str() << _T("\t") << rPriBusID.c_str() << _T("\t") << rPriBusVolt << _T("\t");
			ifile << rSecBusID.c_str() << _T("\t") << rSecBusVolt << _T("\t");
			ifile << rKW << _T("\t") << rKVAR << _T("\t") << rKVA << _T("\t");
			ifile << rCalcualtedCapacity << _T("\t") << rSelectedCapacity << _T("\t") << rSpare << _T("\t");
			ifile << rRemark << std::endl;

			return ERROR_SUCCESS;
		}
	}RowData;

	CTransformerSizingResultDoc();
	virtual ~CTransformerSizingResultDoc();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	STRING_T m_rFilePath;
private:
	int CreateFolderIfNeed(void);
public:
	static int SaveTransformerSizingResultData(void);
};
