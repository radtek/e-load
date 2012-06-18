// BusToBusRelation.cpp: implementation of the CBusToBusRelation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "Relation.h"
#include "BusToBusRelation.h"
#include "ELoadDocData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace BusToBus;

IMPLEMENT_SERIAL(CBusToBusRelation, CRelation, 1)
IS_OBJ_IMPLEMENT_FUNC(CBusToBusRelation , CEntity , "_BUS_TO_BUS_RELATION_")
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBusToBusRelation::CBusToBusRelation() : m_pTrItem(NULL)
{
	SetTypeString( CBusToBusRelation::TypeString() );
}

CBusToBusRelation::CBusToBusRelation(CIsDrawEntContainer* pDoc, CIsControllerObject *pController,
					 const CPoint &point, CEntity *pEnt,bool bAggregation, bool bNavigable) 
					 : CRelation(pDoc,pController,point,pEnt) , m_pTrItem(NULL)
{
	SetTypeString( CBusToBusRelation::TypeString() );

	m_bAggregation = bAggregation;
	m_bNavigable   = bNavigable;
	m_srcMultiplicity  = _T("1");
	m_destMultiplicity = _T("1");
	m_bShowSrcMultiplicity  = false;
	m_bShowDestMultiplicity = false;
	m_srcMultiplicityOffset = CSize(0, 0);
	m_destMultiplicityOffset= CSize(0, 0);
}

CBusToBusRelation::~CBusToBusRelation()
{
	try
	{
		m_pTrItem = NULL;
	}
	catch(...)
	{}
}

CRect CBusToBusRelation::srcMultiplicityRect() const
{
	ASSERT(m_bCreated == true);
	
	long index = 0;
	CPoint A = m_src.point();
	CPoint B = index == m_nodes.GetSize() ? m_dest.point() : m_nodes[index];
	CRect R = m_src.pEnt->GetRect();
	R.InflateRect(16, 16);
	
	double dx = B.x - A.x;
	double dy = B.y - A.y;
	double ox = dx > 0 ? (R.right - A.x + 10) : (R.left - A.x - 10);
	double oy = ox * (dy / dx) - 14;
	
	if(A.y + oy > R.bottom || A.y + oy < R.top)
	{
		oy = dy > 0 ? (R.bottom - A.y + 6) : (R.top - A.y - 6);
		ox = oy * (dx / dy) + 18;
	}
	
	CPoint O(long(A.x + ox), long(A.y + oy));
	CRect rect(O, CSize(0, 0));
	rect.InflateRect(18, 8);
	rect.OffsetRect(m_srcMultiplicityOffset);
	return rect;
}

CRect CBusToBusRelation::destMultiplicityRect() const
{
	ASSERT(m_bCreated == true);
	
	long index = m_nodes.GetSize();
	CPoint B = index == 0 ? m_src.point() : m_nodes[index - 1];
	CPoint A = m_dest.point();
	CRect R = m_dest.pEnt->GetRect();
	R.InflateRect(16, 16);
	
	double dx = B.x - A.x;
	double dy = B.y - A.y;
	double ox = dx > 0 ? (R.right - A.x + 10) : (R.left - A.x - 10);
	double oy = ox * (dy / dx) - 14;
	
	if(A.y + oy > R.bottom || A.y + oy < R.top)
	{
		oy = dy > 0 ? (R.bottom - A.y + 6) : (R.top - A.y - 6);
		ox = oy * (dx / dy) + 18;
	}
	
	CPoint O(long(A.x + ox), long(A.y + oy));
	CRect rect(O, CSize(0, 0));
	rect.InflateRect(18, 8);
	rect.OffsetRect(m_destMultiplicityOffset);
	return rect;
}

/**
 * OnNotify:
 *
 * @param notify 
 * @return void 
 */
void CBusToBusRelation::OnNotify(const IS_NOTIFY& notify)
{
	if(m_bCreated)
	{
		CIsDrawEntity* pEnt=static_cast<CIsDrawEntity*>(notify.pEntFrom);
		if(IS_DELETE_ENTITY == notify.hdr.code)
		{
			if((pEnt == m_src.pEnt) || (pEnt == m_dest.pEnt)) Erase();
		}
		else
		{
			if(pEnt == m_src.pEnt){
				CPoint oldSrcPoint = m_src.point();
				m_src.updatePoint();
				
				CSize srcDelta = m_src.point() - oldSrcPoint;
				//CTinyUMLEditor* pTinyUMLEditor=static_cast<CTinyUMLEditor*>(CTinyDrawCtrlApp::GetCurEditor());
				//if(NULL != pTinyUMLEditor){
				//	CTinyUMLTheme* pUMLTheme=static_cast<CTinyUMLTheme*>(pTinyUMLEditor->theme());
				//	if(pUMLTheme->relativeNodes()){
						long i = 0;
						
						for(; i < m_nodes.GetSize() / 2; ++i)
							m_nodes[i] += srcDelta;
					//}
				//}
			}else if(pEnt == m_dest.pEnt){
				CPoint oldDestPoint = m_dest.point();
				m_dest.updatePoint();
				
				CSize destDelta = m_dest.point() - oldDestPoint;
				//CTinyUMLEditor* pTinyUMLEditor=static_cast<CTinyUMLEditor*>(CTinyDrawCtrlApp::GetCurEditor());
				//if(NULL != pTinyUMLEditor){
				//	CTinyUMLTheme* pUMLTheme=static_cast<CTinyUMLTheme*>(pTinyUMLEditor->theme());
				//	if(pUMLTheme->relativeNodes()){
						long i = 0;
						
						for(; i < m_nodes.GetSize(); ++i)
							m_nodes[i] += destDelta;
					//}
				//}
			}
		}		
	}
}

/**
	@brief	return hit code associated where mouse cursor is on.

	@author	HumKyung

	@date	2010.12.27

	@param	CPoint

	@return
*/
int CBusToBusRelation::HitTest(CPoint point)
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
		if(rectSelect.PtInRect(point))
		{
			const double dist = DistanceBetweenLineAndPoint(ptStart , ptEnd , point);
			if(dist <= nTol) return DEHT_BODY;
		}
		
		ptStart = ptEnd;
	}
	ptEnd = m_dest.point();
	
	rectSelect.left  = (ptStart.x < ptEnd.x) ? ptStart.x - 1: ptEnd.x - 1;
	rectSelect.top   = (ptStart.y < ptEnd.y) ? ptStart.y - 1: ptEnd.y - 1;
	rectSelect.right = (ptStart.x > ptEnd.x) ? ptStart.x + 1: ptEnd.x + 1;
	rectSelect.bottom= (ptStart.y > ptEnd.y) ? ptStart.y + 1: ptEnd.y + 1;
	if(rectSelect.PtInRect(point))
	{
		const double dist = DistanceBetweenLineAndPoint(ptStart , ptEnd , point);
		if(dist <= nTol) return DEHT_BODY;
	}
	
	return DEHT_NONE;
}

void CBusToBusRelation::Serialize(CArchive &ar)
{
	CRelation::Serialize(ar);
	
	if(ar.IsStoring())
	{
		ar << long(m_bAggregation);
		ar << long(m_bNavigable);
		ar << m_srcMultiplicity;
		ar << m_destMultiplicity;
		ar << long(m_bShowSrcMultiplicity);
		ar << long(m_bShowDestMultiplicity);
		ar << m_srcMultiplicityOffset;
		ar << m_destMultiplicityOffset;		
	}else{
		long _flag;
		
		ar >> _flag; m_bAggregation = _flag ? true : false;
		ar >> _flag; m_bNavigable = _flag ? true : false;
		ar >> m_srcMultiplicity;
		ar >> m_destMultiplicity;
		ar >> _flag; m_bShowSrcMultiplicity = _flag ? true : false;
		ar >> _flag; m_bShowDestMultiplicity = _flag ? true : false;
		ar >> m_srcMultiplicityOffset;
		ar >> m_destMultiplicityOffset;
	}
}

COLORREF CBusToBusRelation::GetFillColor() const
{
	return m_fillColor;
}

CFont* CBusToBusRelation::GetMultiplicityFont() const
{
	return CRelation::GetNameFont();
}

bool CBusToBusRelation::IsAggregation() const
{
	return m_bAggregation;
}

bool CBusToBusRelation::IsNavigable() const
{
	return m_bNavigable;
}

bool CBusToBusRelation::IsShowSrcMultiplicity() const
{
	return m_bShowSrcMultiplicity;
}

bool CBusToBusRelation::IsShowDestMultiplicity() const
{
	return m_bShowDestMultiplicity;
}

CString CBusToBusRelation::GetSrcMultiplicity() const
{
	return m_srcMultiplicity;
}

CString CBusToBusRelation::GetDestMultiplicity() const
{
	return m_destMultiplicity;
}

/**
	@brief	line과 point간의 최단 거리를 구한다.

	@author	HumKyung

	@date	2010.12.28

	@param

	@return
*/
double CBusToBusRelation::DistanceBetweenLineAndPoint(CPoint ptStart , CPoint ptEnd , CPoint point) const
{
	double dx = double(ptStart.x) - double(ptEnd.x);
	double dy = double(ptStart.y) - double(ptEnd.y);
	const double mag2 = dx*dx + dy*dy;
	if(mag2 > 0.0)
	{
		const double u = ( (double(point.x) - double(ptStart.x))*(double(ptEnd.x) - double(ptStart.x)) + 
			           (double(point.y) - double(ptStart.y))*(double(ptEnd.y) - double(ptStart.y)) ) / mag2;

		const double xp = double(ptStart.x) + u*(double(ptEnd.x) - double(ptStart.x));
		const double yp = double(ptStart.y) + u*(double(ptEnd.y) - double(ptStart.y));

		dx = xp - double(point.x);
		dy = yp - double(point.y);
	}
	else
	{
		dx = double(ptStart.x) - double(point.x);
		dy = double(ptStart.y) - double(point.y);
	}

	return sqrt(dx*dx + dy*dy);
}

/**
	@brief	save bus to bus relation

	@author	HumKyung

	@date	2011.01.03

	@param

	@return
*/
int CBusToBusRelation::Save(CADODB& adoDB)
{
	string sSrcAnchorName , sDstAnchorName;
	CRelation::Anchor srcAnchor = GetSrc();
	if(srcAnchor.pEnt)
	{
		sSrcAnchorName = ((CBusEntity*)srcAnchor.pEnt)->GetBusItem()->GetName();;
	}else	return ERROR_BAD_ENVIRONMENT;
	CRelation::Anchor dstAnchor = GetDest();
	if(dstAnchor.pEnt)
	{
		sDstAnchorName = ((CBusEntity*)dstAnchor.pEnt)->GetBusItem()->GetName();
	}else	return ERROR_BAD_ENVIRONMENT;

	CELoadDocData& docData = CELoadDocData::GetInstance();
	list<CELoadItem*> TrItemList;
	docData.GetELoadItemListOf(TrItemList , CTransformerItem::TypeString());
	for(list<CELoadItem*>::iterator itr = TrItemList.begin();itr != TrItemList.end();++itr)
	{
		if((*itr)->IsDeleted()) continue;

		CTransformerItem* pTrItem = static_cast<CTransformerItem*>(*itr);
		if(pTrItem)
		{
			const string& sFromBusId = pTrItem->prop()->GetValue(_T("From") , _T("Bus Id"));
			const string& sToBusId = pTrItem->prop()->GetValue(_T("To") , _T("Bus Id"));

			if((sFromBusId == sSrcAnchorName) && (sToBusId == sDstAnchorName))
			{
				m_pTrItem = pTrItem;
				//! update transformer record from table
				m_pTrItem->SaveData(adoDB , CTransformerItem::TableName());
				return ERROR_SUCCESS;
			}
		}
	}
	
	//! create a tranformer and insert a transformer record.
	m_pTrItem = static_cast<CTransformerItem*>(docData.CreateELoadItemOf(CTransformerItem::TypeString()));
	if(m_pTrItem)
	{
		const string sName(_T("DEL ") + sSrcAnchorName + _T("_") + sDstAnchorName);
		m_pTrItem->SetName(sName);
		m_pTrItem->prop()->SetValue(_T("From") , _T("Bus Id") , sSrcAnchorName);
		m_pTrItem->prop()->SetValue(_T("To") , _T("Bus Id") , sDstAnchorName);
		
		CELoadItem* pFrom = docData.FindItemByName(CBusItem::TypeString() , sSrcAnchorName);
		if(pFrom)
		{
			string sValue = pFrom->prop()->GetValue(_T("General") , _T("Bus Voltage"));
			m_pTrItem->prop()->SetValue(_T("From") , _T("Bus Volt") , sValue);
			sValue = pFrom->prop()->GetValue(_T("General") , _T("Load Voltage"));
			m_pTrItem->prop()->SetValue(_T("From") , _T("Load Volt") , sValue);
		}
		CELoadItem* pTo = docData.FindItemByName(CBusItem::TypeString() , sDstAnchorName);
		if(pTo)
		{
			string sValue = pFrom->prop()->GetValue(_T("General") , _T("Bus Voltage"));
			m_pTrItem->prop()->SetValue(_T("To") , _T("Bus Volt") , sValue);
			sValue = pFrom->prop()->GetValue(_T("General") , _T("Load Voltage"));
			m_pTrItem->prop()->SetValue(_T("To") , _T("Load Volt") , sValue);
		}
		
		const double dCapacity = m_pTrItem->CalculateCapacity();
		stringstream oss;
		oss << dCapacity;
		m_pTrItem->prop()->SetValue(_T("Capacity") , _T("Calculated Capacity") , oss.str());
		m_pTrItem->prop()->SetValue(_T("Capacity") , _T("Selected Capacity") , _T("0"));

		m_pTrItem->SaveData(adoDB , CTransformerItem::TableName());
	}

	return ERROR_SUCCESS;
}