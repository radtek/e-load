// Relation.cpp: implementation of the CRelation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <assert.h>
#include <math.h>
#include "Relation.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace BusToBus;

static CFont* __NAME_FONT__;

IMPLEMENT_SERIAL(CRelation, CEntity, 1)
IS_OBJ_IMPLEMENT_FUNC(CRelation , CEntity , "_TINY_UML_RELATION_")
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * CRelation:
 *
 * @return  
 */
CRelation::CRelation() : CEntity()
{
	m_handleSize= 4;
	///m_lineColor = NULL;

	m_name = "new relation";
	if(NULL == __NAME_FONT__)
	{
		__NAME_FONT__ = new CFont();
		__NAME_FONT__->CreateFont(-10,0,0,0,400,FALSE,FALSE,FALSE,HANGUL_CHARSET,3,2,1,VARIABLE_PITCH | FF_MODERN, _T("Arial"));
	}
}

/**
 * CRelation:
 *
 * @param pDoc 
 * @param pController 
 * @param point 
 * @param pEnt 
 * @return  
 */
CRelation::CRelation(CIsDrawEntContainer* pDoc,CIsControllerObject* pController,const CPoint &point, CEntity *pEnt)
 : CEntity(pDoc,pController)
{
	ASSERT(pEnt != NULL);
	
	m_handleSize= 4;
	m_bCreated = false;
	m_tempEnd = point;
	
	m_src.pEnt = pEnt;
	m_src.movePoint(point);
	
	m_name = "new relation";
	m_bShowName = false;
	m_nameOffset = CSize(0, -10);
	
	m_penStyle = PS_SOLID;
	
	m_dest.pEnt = NULL;	
}

/**
 * ~CRelation:
 *
 * @return  
 */
CRelation::~CRelation()
{
	try
	{
		if(NULL != m_src.pEnt)  m_src.pEnt->RemoveReactor(this);
		if(NULL != m_dest.pEnt) m_dest.pEnt->RemoveReactor(this);
	}
	catch(...)
	{
	}
}

/**
 * updatePoint:
 *
 * @return void 
 */
void CRelation::Anchor::updatePoint()
{
	ASSERT(rx >= 0 && rx <= 1);
	ASSERT(ry >= 0 && ry <= 1);
	
	CRect r = pEnt->GetRect();
	
	m_point.x = long(r.left + r.Width() * rx);
	m_point.y = long(r.top + r.Height() * ry);
}


/**
 * movePoint:
 *
 * @param point 
 * @return void 
 */
void CRelation::Anchor::movePoint(const CPoint &point)
{
	rx = (point.x - pEnt->GetRect().left) / double(pEnt->GetRect().Width());
	ry = (point.y - pEnt->GetRect().top) / double(pEnt->GetRect().Height());
	
	if(rx < 0)
		rx = 0;
	if(rx > 1)
		rx = 1;
	if(ry < 0)
		ry = 0;
	if(ry > 1)
		ry = 1;
	
	updatePoint();
}

/**
	@brief	

	@author	HumKyung

	@date	2011.01.03

	@param

	@return	int
*/
int CRelation::Anchor::setPoint(const CPoint &point)
{
	m_point = point;

	return ERROR_SUCCESS;
}

/**
 * Copy:
 *
 * @param pRelation 
 * @return void 
 */
void CRelation::Copy(CRelation *pRelation)
{
	CIsDrawEntity::Copy(pRelation);
	
	//////////////////////////////////////////////////////////////////////////
	/// font
	///m_pNameFont = pRelation->m_pNameFont;
	//////////////////////////////////////////////////////////////////////////
	
	m_name = pRelation->m_name;
	m_bCreated = pRelation->m_bCreated;
	m_bShowName = pRelation->m_bShowName;
	m_nameOffset = pRelation->m_nameOffset;
	m_penStyle = pRelation->m_penStyle;
	m_tempEnd = pRelation->m_tempEnd;
	
	m_nodes.Copy(pRelation->m_nodes);
	m_segTypes.Copy(pRelation->m_segTypes);
	
	m_src = pRelation->m_src;
	m_dest= pRelation->m_dest;
	//////////////////////////////////////////////////////////////////////////
	/// color
	///m_lineColor = pRelation->m_lineColor;
	//////////////////////////////////////////////////////////////////////////
}

inline void _transform(CPoint p[], long count,
	const CPoint &A, const CPoint &B, const CRect &R)
{
	double dx = B.x - A.x;
	double dy = B.y - A.y;
	double length = ::sqrt(dx * dx + dy * dy);
	//ASSERT(length != 0);

	if(length == 0)
	{
		for(long i = 0; i < count; ++i)
			p[i] = CPoint(0, 0);
		return;
	}

	double rx = dx / length;
	double ry = dy / length;

	double ox = dx > 0 ? R.right - A.x : R.left - A.x;
	double oy = ox * (dy / dx);

	if(A.y + oy > R.bottom || A.y + oy < R.top)
	{
		oy = dy > 0 ? R.bottom - A.y : R.top - A.y;
		ox = oy * (dx / dy);
	}

	CPoint O(long(A.x + ox), long(A.y + oy));

	for(long i = 0; i < count; ++i)
	{
		double ax = p[i].x;
		double ay = p[i].y;
		p[i].x = O.x + long(rx * ax - ry * ay);
		p[i].y = O.y + long(ry * ax + rx * ay);
	}
}

void CRelation::destTransform(CPoint p[], long count) const
{
	ASSERT(m_bCreated == true);
	
	long index = m_nodes.GetSize();
	CPoint B = index == 0 ? m_src.point() : m_nodes[index - 1];
	CPoint A = m_dest.point();
	CRect R = m_dest.pEnt->GetRect();
	
	_transform(p, count, A, B, R);
}

CRect CRelation::GetNameRect() const
{
	ASSERT(m_bCreated == true);
	
	CRect r(centerPoint() + m_nameOffset, CSize(0, 0));
	r.InflateRect(50, 8);
	return r;
}

CPoint CRelation::centerPoint() const
{
	ASSERT(m_bCreated == true);
	
	if(m_nodes.GetSize() % 2 == 0){
		long index = m_nodes.GetSize() / 2;
		CPoint p1 = index == 0 ? m_src.point() : m_nodes[index - 1];
		CPoint p2 = index == m_nodes.GetSize() ? m_dest.point() : m_nodes[index];
		return CPoint((p1.x + p2.x) / 2, (p1.y + p2.y) / 2);
	}else{
		return m_nodes[m_nodes.GetSize() / 2];
	}
}

/**
 * Finish:
 *
 * @param point 
 * @param pEnt 
 * @return bool 
 */
bool CRelation::Finish(const CPoint &point, CEntity* pEnt)
{
	assert(pEnt && "pEnt is NULL");

	if(pEnt && (pEnt != m_src.pEnt))
	{
		m_dest.pEnt = pEnt;
		m_dest.movePoint(point);
		m_bCreated = true;
		
		if(m_dest.pEnt == m_src.pEnt){
			BreakSeg(0, CPoint(m_src.point().x - 30, m_src.pEnt->GetRect().top - 40));
			BreakSeg(1, CPoint(m_src.point().x + 0, m_src.pEnt->GetRect().top - 40));
		}
		
		CRect rect;
		if(m_src.point().x < m_dest.point().y){
			rect.left = m_src.point().x;
			rect.right= m_dest.point().x;
		}else{
			rect.left = m_dest.point().x;
			rect.right= m_src.point().x;
		}
		if(m_src.point().y > m_dest.point().y){
			rect.top   = m_src.point().y;
			rect.bottom= m_dest.point().y;
		}else{
			rect.top   = m_dest.point().y;
			rect.bottom= m_src.point().y;
		}
		SetRect(rect);

		//////////////////////////////////////////////////////////////////////////
		/// add receiver
		m_src.pEnt->AddReactor(this);
		m_dest.pEnt->AddReactor(this);

		return true;
	}

	return false;
}

long CRelation::BreakSeg(long index, const CPoint &point)
{
	m_nodes.InsertAt(index, point);
	return index + 2;
}

/**
 * DragTemp:
 *
 * @param point 
 * @return void 
 */
void CRelation::DragTemp(const CPoint &point)
{
	ASSERT(m_bCreated == false);
	m_tempEnd = point;
}

/**
 * BeginDrag:
 *
 * @param point 
 * @param handle 
 * @return void 
 */
void CRelation::BeginDrag(const CPoint &point, long handle)
{
	//Shape::beginDrag(point, handle);
	//m_dragPoint = point;
	//m_lastPoint = point;
	m_dragHandle = handle;
	//m_bDragging = true;
	
	if(handle == NEW_NODE)
	{
		CRect r(point, CSize(0, 0));
		r.InflateRect(1, 1);
		
		if (m_bShowName && GetNameRect().PtInRect(point))
		{
			m_dragHandle = HANDLE_DRAG_TEXT;
			return;
		}
		
		bool bIntersect=false;
		long i = 0;
		for(i = 0; i <= m_nodes.GetSize(); ++i)
		{
			if(IntersectsSeg(r, i)){
				bIntersect = true;
				break;
			}
		}
		if(true == bIntersect)
			m_dragHandle = BreakSeg(i, point);
		else	m_dragHandle = -1;
	}
}

/**
 * _calcSafeBox:
 *
 * @param p1 
 * @param p2 
 * @param width 
 * @param p 
 * @return static void 
 */
static void _calcSafeBox(const CPoint &p1, const CPoint &p2, long width, CPoint p[4])
{
	double dx = p1.x - p2.x;
	double dy = p1.y - p2.y;
	double diag = ::sqrt(dx * dx + dy * dy);
	//ASSERT(diag != 0);

	long ox = 0;
	long oy = 0;

	if(diag != 0)
	{
		double rx = dx / diag;
		double ry = dy / diag;
		ox = long(width *-ry);
		oy = long(width * rx);
	}

	p[0] = p1 + CSize(-ox,-oy);
	p[1] = p1 + CSize( ox, oy);
	p[2] = p2 + CSize(-ox,-oy);
	p[3] = p2 + CSize(ox,oy);
}

/**
 * IntersectsSeg:
 *
 * @param rect 
 * @param index 
 * @return bool 
 */
bool CRelation::IntersectsSeg(const CRect &rect, long index) const
{
	int numNodes;
	
	if(NULL == m_dest.pEnt) return FALSE;
	
	numNodes = m_nodes.GetSize();
	ASSERT(index >= 0 && index <= numNodes);
	
	const long handleSize = m_handleSize;
	
	CRgn rgn;
	CPoint p[4];
	CPoint p1 = index == 0 ? m_src.point() : m_nodes[index - 1];
	CPoint p2 = index == numNodes ? m_dest.point() : m_nodes[index];
	
	_calcSafeBox(p1, p2, handleSize, p);
	rgn.CreatePolygonRgn(p, 4, ALTERNATE);

	return rgn.RectInRegion(rect) ? true : false;
}

/**
 * handlePoint:
 *
 * @param handle 
 * @return CPoint 
 */
CPoint CRelation::handlePoint(long handle) const
{
	ASSERT(handle >= 0 && handle < handlesCount());
	
	switch(handle)
	{
		case 0: return m_src.point();
		case 1: return m_dest.point();
		default: return m_nodes[handle - 2];
	}
	
	return CPoint();
}

/**
 * handlesCount:
 *
 * @return long 
 */
long CRelation::handlesCount() const
{
	return 2 + m_nodes.GetSize();
}

/**
 * Drag:
 *
 * @param point 
 * @return void 
 */
void CRelation::Drag(const CPoint &point)
{
	//ASSERT(m_dragHandle >= 0);
	
	if (m_dragHandle == HANDLE_DRAG_TEXT){
		CSize delta = point - m_ptLastPos;
		m_nameOffset += delta;
		m_ptLastPos = point;
		return;
	}
	
	if (m_dragHandle == 0)
		m_src.movePoint(point);
	else if(m_dragHandle == 1)
		m_dest.movePoint(point);
	else if(m_dragHandle > 1)
		m_nodes[m_dragHandle - 2] = point;	
}

/**
 * EndDrag:
 *
 * @return void 
 */
void CRelation::EndDrag()
{
	optimize();

	UpdateRect();
}

/**
 * optimize:
 *
 * @return bool 
 */
bool CRelation::optimize()
{
	//CTinyUMLEditor* pTinyUMLEditor=static_cast<CTinyUMLEditor*>(CTinyDrawCtrlApp::GetCurEditor());
	long minLength = 1;//pTinyUMLEditor->GetGridSize().cx;
	double minAngle = 0.12;
	long i;
	
	// don't optimize single segment links
	if(m_nodes.GetSize() == 0)
		return false;
	
	// 1. remove segments too small
	for(i = 0; i <= m_nodes.GetSize(); ++i){
		if(segLength(i) < minLength)
			RemoveSeg(i);
	}
	
	// 2. remove parallel segments
	for(i = 0; i < m_nodes.GetSize(); ++i){
		double relAngle = ::fabs(SegAngle(i) - SegAngle(i + 1));
		relAngle = relAngle > 3.14 ? (3.14 * 2 - relAngle) : relAngle;
		
		if(relAngle < minAngle){
			RemoveSeg(i);
			--i;
		}
		
		// add support 4 oposite segments!
	}
	
	// 3. self crossing polygons?
	// ...
	
	return true;
}

double CRelation::segLength(long index) const
{
	ASSERT(index >= 0 && index <= m_nodes.GetSize());
	
	CPoint p1 = index == 0 ? m_src.point() : m_nodes[index - 1];
	CPoint p2 = index == m_nodes.GetSize() ? m_dest.point() : m_nodes[index];
	double dx = p1.x - p2.x;
	double dy = p1.y - p2.y;
	
	return ::sqrt(dx * dx + dy * dy);
}

double CRelation::SegAngle(long index) const
{
	ASSERT(index >= 0 && index <= m_nodes.GetSize());
	
	CPoint p1 = index == 0 ? m_src.point() : m_nodes[index - 1];
	CPoint p2 = index == m_nodes.GetSize() ? m_dest.point() : m_nodes[index];
	double dx = p2.x - p1.x;
	double dy = p2.y - p1.y;
	double length = ::sqrt(dx * dx + dy * dy);
	
	if(length == 0)
		return 0;
	
	return dy > 0 ? ::acos(dx / length) : 2 * 3.141 - ::acos(dx / length);
}

void CRelation::RemoveSeg(long index)
{
	ASSERT(index >= 0 && index <= m_nodes.GetSize());
	
	if(index == m_nodes.GetSize())
		m_nodes.RemoveAt(index - 1);
	else	m_nodes.RemoveAt(index);
}

/**
 * handleAt:
 *
 * @param point 
 * @return long 
 */
long CRelation::handleAt(const CPoint &point) const
{
	long handleSize = m_handleSize;

	long handle = -1;
	for(long i = 0; i < handlesCount(); ++i)
	{
		CPoint pt(handlePoint(i));
		pt.x -= m_varMarkerSize.cx*0.5;
		pt.y -= m_varMarkerSize.cy*0.5;
		CRect r(pt,CSize(m_varMarkerSize.cx, m_varMarkerSize.cy));
		if(r.PtInRect(point))
		{
			handle = i;
			break;
		}
	}
	
	return handle != -1 ? handle : (intersects(point) ? NEW_NODE : -1);
}

/**
 * intersects:
 *
 * @param point 
 * @return bool 
 */
bool CRelation::intersects(const CPoint &point) const
{
	CRect r(point, CSize(0, 0));
	r.InflateRect(1, 1);
	return intersects(r);
}

/**
 * intersects:
 *
 * @param rect 
 * @return bool 
 */
bool CRelation::intersects(const CRect &rect) const
{
	if (m_bShowName && GetNameRect().PtInRect(rect.TopLeft()))
	{
		return true;
	}
	
	for(long i = 0; i <= m_nodes.GetSize(); ++i)
		if(IntersectsSeg(rect, i))
			return true;
		
		return false;
}

/**
 * srcTransform:
 *
 * @param p 
 * @param count 
 * @return void 
 */
void CRelation::srcTransform(CPoint p[], long count) const
{
	ASSERT(m_bCreated == true);
	
	long index = 0;
	CPoint A = m_src.point();
	CPoint B = index == m_nodes.GetSize() ? m_dest.point() : m_nodes[index];
	CRect R = m_src.pEnt->GetRect();
	
	_transform(p, count, A, B, R);
}

/**
 * GetHitCode:
 *
 * @param point 
 * @return int 
 */
int CRelation::GetHitCode(CPoint point)
{
	//////////////////////////////////////////////////////////////////////////
	/// check control points.
	int horz = m_varMarkerSize.cx * 0.5;
	int vert = m_varMarkerSize.cy * 0.5;
	CRect rectSelect;
	
	rectSelect.left = m_src.point().x - horz;rectSelect.top    = m_src.point().y - vert;
	rectSelect.right= m_src.point().x + horz;rectSelect.bottom = m_src.point().y + vert;
	if(rectSelect.PtInRect(point)) return DEHT_CTRLPOINT;
	
	long i = 0;
	for(i = 0; i < m_nodes.GetSize(); ++i)
	{
		rectSelect.left = m_nodes[i].x - horz;rectSelect.top    = m_nodes[i].y - vert;
		rectSelect.right= m_nodes[i].x + horz;rectSelect.bottom = m_nodes[i].y + vert;
		if(rectSelect.PtInRect(point)) return DEHT_CTRLPOINT;
	}
	rectSelect.left = m_dest.point().x - horz;rectSelect.top    = m_dest.point().y - vert;
	rectSelect.right= m_dest.point().x + horz;rectSelect.bottom = m_dest.point().y + vert;
	if(rectSelect.PtInRect(point)) return DEHT_CTRLPOINT;
	//////////////////////////////////////////////////////////////////////////
	
	
	const static float nTol=2.f;
	double dx[2]={0.f,},dy[2]={0.f,},dot=0.f,d=0.f,cross=0.,h=0.;
	CPoint ptStart=m_src.point(),ptEnd;
	for(i = 0; i < m_nodes.GetSize(); ++i)
	{
		ptEnd = m_nodes[i];
		
		rectSelect.left  = (ptStart.x < ptEnd.x) ? ptStart.x - 1: ptEnd.x - 1;
		rectSelect.top   = (ptStart.y < ptEnd.y) ? ptStart.y - 1: ptEnd.y - 1;
		rectSelect.right = (ptStart.x > ptEnd.x) ? ptStart.x + 1: ptEnd.x + 1;
		rectSelect.bottom= (ptStart.y > ptEnd.y) ? ptStart.y + 1: ptEnd.y + 1;
		if(rectSelect.PtInRect(point)){
			dx[0]=ptEnd.x - ptStart.x;dy[0]=ptEnd.y - ptStart.y;
			dx[1]=point.x - ptStart.x;dy[1]=point.y - ptStart.y;
			dot = dx[0]*dx[1] + dy[0]*dy[1];
			d = sqrt(dx[0]*dx[0] + dy[0]*dy[0]);
			dx[0] = dx[0]*dot/d;dy[0] = dy[0]*dot/d;
			cross = dx[0]*dy[1] - dy[0]*dx[1];
			h = cross/(2*dot);
			if(fabs(h) <= nTol) return DEHT_BODY;
		}
		
		ptStart = ptEnd;
	}
	ptEnd = m_dest.point();
	
	rectSelect.left  = (ptStart.x < ptEnd.x) ? ptStart.x - 1: ptEnd.x - 1;
	rectSelect.top   = (ptStart.y < ptEnd.y) ? ptStart.y - 1: ptEnd.y - 1;
	rectSelect.right = (ptStart.x > ptEnd.x) ? ptStart.x + 1: ptEnd.x + 1;
	rectSelect.bottom= (ptStart.y > ptEnd.y) ? ptStart.y + 1: ptEnd.y + 1;
	if(rectSelect.PtInRect(point)){
		dx[0]=ptEnd.x - ptStart.x;dy[0]=ptEnd.y - ptStart.y;
		dx[1]=point.x - ptStart.x;dy[1]=point.y - ptStart.y;
		dot = dx[0]*dx[1] + dy[0]*dy[1];
		d = sqrt(dx[0]*dx[0] + dy[0]*dy[0]);
		dx[0] = dx[0]*dot/d;dy[0] = dy[0]*dot/d;
		cross = dx[0]*dy[1] - dy[0]*dx[1];
		h = cross/(2*dot);
		if(fabs(h) <= nTol) return DEHT_BODY;
	}
	
	return DEHT_NONE;
}

/**
 * GetSourceEntity:
 *
 * @return CTinyDrawEntity* 
 */
CIsDrawEntity* CRelation::GetSourceEntity()
{
	return m_src.pEnt;
}

/**
 * GetDestEntity:
 *
 * @return CTinyDrawEntity* 
 */
CIsDrawEntity* CRelation::GetDestEntity()
{
	return m_dest.pEnt;
}

/**
 * IsInRect:
 *
 * @param rect 
 * @return bool 
 */
bool CRelation::IsInRect(CRect &rect)
{
	if((m_src.point().x >= rect.left) && (m_src.point().x <= rect.right) &&
	   (m_src.point().y >= rect.top) && (m_src.point().y <= rect.bottom)) return true;

	if((m_dest.point().x >= rect.left) && (m_dest.point().x <= rect.right) &&
	   (m_dest.point().y >= rect.top) && (m_dest.point().y <= rect.bottom)) return true;

	for(int i=0;i < m_nodes.GetSize();i++){
		if((m_nodes.ElementAt(i).x >= rect.left) && (m_nodes.ElementAt(i).x <= rect.right) &&
		   (m_nodes.ElementAt(i).y >= rect.top) && (m_nodes.ElementAt(i).y <= rect.bottom)) return true;
	}

	return false;
}

/**
 * Move:
 *
 * @param x 
 * @param y 
 * @return void 
 */
//DEL void CRelation::Move(const double x, const double y)
//DEL {
//DEL 	CPoint pt = m_src.point();
//DEL 	pt.x += x ; pt.y += y;
//DEL 	m_src = pt;
//DEL 	
//DEL 	pt = m_dest.point();
//DEL 	pt.x += x ; pt.y += y;
//DEL 	m_dest = pt;
//DEL 
//DEL 	for(int i=0;i < m_nodes.GetSize();i++)
//DEL 	{
//DEL 		m_nodes.ElementAt(i).x += x;
//DEL 		m_nodes.ElementAt(i).y += y;
//DEL 	}
//DEL 
//DEL 	CEntity::Move(x,y);
//DEL }

/**
 * UpdateRect:
 *
 * @return void 
 */
void CRelation::UpdateRect()
{
	SetRect(m_src.point().x,m_src.point().y,m_src.point().x,m_src.point().y);
	//////////////////////////////////////////////////////////////////////////
	/// update entity size
	const int nSiz=m_nodes.GetSize();
	for(int i=0;i < nSiz;i++){
		CRect rect=GetRect();
		if(rect.left > m_nodes.GetAt(i).x) rect.left = m_nodes.GetAt(i).x;
		if(rect.right< m_nodes.GetAt(i).x) rect.right= m_nodes.GetAt(i).x;
		if(rect.top   < m_nodes.GetAt(i).y) rect.top   = m_nodes.GetAt(i).y;
		if(rect.bottom> m_nodes.GetAt(i).y) rect.bottom= m_nodes.GetAt(i).y;
		SetRect(rect);
	}

	CRect rect=GetRect();
	if(rect.left > m_dest.point().x) rect.left = m_dest.point().x;
	if(rect.right< m_dest.point().x) rect.right= m_dest.point().x;
	if(rect.top   < m_dest.point().y) rect.top   = m_dest.point().y;
	if(rect.bottom> m_dest.point().y) rect.bottom= m_dest.point().y;
	SetRect(rect);
}

bool CRelation::IsCreated() const
{
	return m_bCreated;
}

COLORREF CRelation::GetLineColor() const
{
	return RGB(0,0,0);
}

int CRelation::GetPenStyle() const
{
	return m_penStyle;
}

CRelation::Anchor& CRelation::GetSrc()
{
	return m_src;
}

CRelation::Anchor& CRelation::GetDest()
{
	return m_dest;
}

CPoint CRelation::GetNodeAt(const int &at)
{
	return m_nodes.GetAt(at);
}

int CRelation::GetNodeSize() const
{
	return m_nodes.GetSize();
}

bool CRelation::IsShowName() const
{
	return m_bShowName;
}


CFont* CRelation::GetNameFont() const
{
	return __NAME_FONT__;
}

CString CRelation::GetName() const
{
	return m_name;
}

/**
 * GetHandleSize:
 *
 * @return long 
 */
long CRelation::GetHandleSize() const
{
	return m_handleSize;
}

CPoint CRelation::GetTempPos() const
{
	return m_tempEnd;
}
