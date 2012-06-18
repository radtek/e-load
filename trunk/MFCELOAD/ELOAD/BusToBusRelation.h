// BusToBusRelation.h: interface for the CBusToBusRelation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TINYUMLASSOCIATION_H__DD28489F_A652_478E_90FB_392BD6202F1F__INCLUDED_)
#define AFX_TINYUMLASSOCIATION_H__DD28489F_A652_478E_90FB_392BD6202F1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Relation.h"
#include "BusToBusRelationFactory.h"

namespace BusToBus
{
class CBusToBusRelation : public CRelation  
{
	DECLARE_SERIAL(CBusToBusRelation)
protected:
	CBusToBusRelation();
public:
	int Save(CADODB& adoDB);	//! 2011.01.03 added - by HumKyung
	int SetTransformerItem(CTransformerItem* pTrItem)
	{
		m_pTrItem = pTrItem;
		return ERROR_SUCCESS;
	}
	CTransformerItem* GetTransformerItem() const
	{
		return m_pTrItem;
	}

	CString GetDestMultiplicity() const;
	CString GetSrcMultiplicity() const;
	bool IsShowDestMultiplicity() const;
	bool IsShowSrcMultiplicity() const;
	bool IsNavigable() const;
	bool IsAggregation() const;
	CFont* GetMultiplicityFont() const;
	COLORREF GetFillColor() const;
	virtual void Serialize(CArchive &ar);
	virtual int HitTest(CPoint point);
	virtual void OnNotify(const IS_NOTIFY& notify);
	virtual CRect destMultiplicityRect() const;
	virtual CRect srcMultiplicityRect() const;

	CBusToBusRelation(CIsDrawEntContainer* pDoc,CIsControllerObject* pController,
		const CPoint &point, CEntity *pEnt,bool bAggregation, bool bNavigable);
	virtual ~CBusToBusRelation();

	IS_OBJ_DECLARE_FUNC()
protected:
	double DistanceBetweenLineAndPoint(CPoint ptStart , CPoint ptEnd , CPoint point) const;

	COLORREF m_fillColor;
	CFont *m_pMultiplicityFont;

	bool m_bAggregation;
	bool m_bNavigable;
	CString m_srcMultiplicity;
	CString m_destMultiplicity;
	bool m_bShowSrcMultiplicity;
	bool m_bShowDestMultiplicity;
	CSize m_srcMultiplicityOffset;
	CSize m_destMultiplicityOffset;
private:
	CTransformerItem* m_pTrItem;	//! 2011.01.03 - added by HumKyung

	friend class CBusToBusRelationController;
};
};

#endif // !defined(AFX_TINYUMLASSOCIATION_H__DD28489F_A652_478E_90FB_392BD6202F1F__INCLUDED_)
