// Relation.h: interface for the CRelation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TINYUMLRELATION_H__D19F862F_FF19_428E_B9F6_AB18435DABD7__INCLUDED_)
#define AFX_TINYUMLRELATION_H__D19F862F_FF19_428E_B9F6_AB18435DABD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BusEntity.h"

#define HANDLE_DRAG_TEXT		-2
#define HANDLE_DRAG_SOURCE_MULTIPLICITY	-3
#define HANDLE_DRAG_DEST_MULTIPLICITY	-4

namespace BusToBus
{
class CRelation : public CEntity  
{
	DECLARE_SERIAL(CRelation)
protected:
	struct Anchor
	{
		CEntity *pEnt;
		double rx;
		double ry;
		
		const CPoint &point() const { return m_point; }
		void updatePoint();
		void movePoint(const CPoint &point);
		int  setPoint(const CPoint& point);
		bool operator = (const CPoint& point)
		{
			m_point = point;
			return true;
		}
	protected:
		CPoint m_point;
	};
	
	enum { NEW_NODE = 1024 };
	CRelation();
public:
	CPoint GetTempPos() const;
	long GetHandleSize() const;
	virtual CRect GetNameRect() const;
	CString GetName() const;
	CFont* GetNameFont() const;
	bool IsShowName() const;
	int GetNodeSize() const;
	CPoint GetNodeAt(const int& at);
	Anchor& GetDest();
	Anchor& GetSrc();
	int GetPenStyle() const;
	COLORREF GetLineColor() const;
	bool IsCreated() const;
	virtual bool  IsInRect(CRect& rect);
	CIsDrawEntity* GetDestEntity();
	CIsDrawEntity* GetSourceEntity();
	virtual int GetHitCode(CPoint point);
	virtual bool intersects(const CRect &rect) const;
	virtual bool intersects(const CPoint &point) const;
	virtual long handleAt(const CPoint &point) const;
	virtual void EndDrag();
	virtual void Drag(const CPoint &point);
	virtual long handlesCount() const;
	virtual CPoint handlePoint(long handle) const;
	virtual void BeginDrag(const CPoint &point, long handle=-1);
	virtual void DragTemp(const CPoint &point);
	virtual bool Finish(const CPoint &point, CEntity *pEnt);
	virtual void destTransform(CPoint p[], long count) const;
	virtual void srcTransform(CPoint p[], long count) const;
	virtual void Copy(CRelation* pRelation);
	CRelation(CIsDrawEntContainer* pDoc,CIsControllerObject* pController,const CPoint &point, CEntity *pEnt);
	virtual ~CRelation();

	IS_OBJ_DECLARE_FUNC()
protected:
	virtual void RemoveSeg(long index);
	virtual double SegAngle(long index) const;
	virtual double segLength(long index) const;
	virtual bool optimize();
	virtual bool IntersectsSeg(const CRect &rect, long index) const;
	virtual long BreakSeg(long index, const CPoint &point);
	virtual CPoint centerPoint() const;
	int m_penStyle;
///	COLORREF* m_lineColor;
///	CFont *m_pNameFont;
	
	CArray<CPoint, const CPoint &> m_nodes;
	CArray<long, long> m_segTypes;
	
	Anchor m_src;
	Anchor m_dest;
	
	CString m_name;
	CSize m_nameOffset;
	bool m_bShowName;
	long m_handleSize;
	
	bool m_bCreated;
	CPoint m_tempEnd;
	CPoint m_ptLastPos;
private:
	void UpdateRect();
};
};

#endif // !defined(AFX_TINYUMLRELATION_H__D19F862F_FF19_428E_B9F6_AB18435DABD7__INCLUDED_)
