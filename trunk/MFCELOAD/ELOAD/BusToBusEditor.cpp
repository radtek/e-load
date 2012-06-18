// BusToBusEditor.cpp: implementation of the CBusToBusEditor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BusToBusEditor.h"
///#include "BusEntity.h"
#include "BusToBusRelation.h"
#include "BusToBusRelationController.h"
#include "BusToBusRelationDraw.h"
#include "ELoadDocData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace BusToBus;

/**
	@brief	

	@author	HumKyung

	@date	2010.12.23

	@param

	@return
 */
CBusToBusContainer::CBusToBusContainer() : CIsDrawEntContainer()
{
	m_font.CreateFont(-14,0,0,0,400,FALSE,FALSE,FALSE,HANGUL_CHARSET,3,2,1,VARIABLE_PITCH | FF_MODERN, _T("Arial"));
}

/**
	@brief	

	@author	HumKyung

	@date	2010.12.23

	@param

	@return
 */
CBusToBusContainer::~CBusToBusContainer()
{

}

/**
	@brief	

	@author	HumKyung

	@date	2010.12.23

	@param

	@return
 */
void CBusToBusContainer::Append(CIsObject *obj)
{
	assert(obj && "obj is NULL");
	if(obj)
	{
		m_objs.Add( obj );
		static_cast<CIsDrawEntity*>(obj)->SetDoc(this);
		
		//////////////////////////////////////////////////////////////////////////
		for(int i=0;i < m_reactorEntry.GetSize();++i)
		{
			CTyDrawContainerReactor* pReactor=m_reactorEntry.GetAt(i);
			pReactor->objectAppened((CIsDrawEntity*)obj);
		}
		//////////////////////////////////////////////////////////////////////////
		
		SetModified( TRUE );
	}
}

/**
	@brief	find a relation which has same src and same dest

	@author	HumKyung

	@date	2011.01.03

	@param

	@return
 */
CRelation* CBusToBusContainer::FindRelation(CIsDrawEntity* pSrcEnt , CIsDrawEntity* pDestEnt)
{
	auto_ptr<CTinyDocIterator> iter(CreateIteator());
	for(iter->First();iter->IsDone();iter->Next())
	{
		CIsDrawEntity* pEnt = iter->GetData();
		if(pEnt->IsKindOf(CBusToBusRelation::TypeString()))
		{
			CBusToBusRelation* pRel = static_cast<CBusToBusRelation*>(pEnt);
			if((pSrcEnt == pRel->GetSrc().pEnt) && (pDestEnt == pRel->GetDest().pEnt)) return pRel;
		}
	}

	return NULL;
}

static EntityTypeMap __EntityTypeMap[]=
{
	//////////////////////////////////////////////////////////////////////////
	{CBusEntity::TypeString , CBusEntityController::CreateInstance , CBusEntityDraw::CreateInstance},
	{CBusToBusRelation::TypeString , CBusToBusRelationController::CreateInstance , CBusToBusRelationDraw::CreateInstance}
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * CBusToBusEditor:
 *
 * @param pEntContainer 
 * @return  
 */
CBusToBusEditor::CBusToBusEditor(CBusToBusContainer* pEntContainer) : CIsDrawEditor(pEntContainer)
{
	m_pEditCtrl = NULL;
}

CBusToBusEditor::~CBusToBusEditor()
{
}

BEGIN_MESSAGE_MAP(CBusToBusEditor, CIsDrawEditor)
	//{{AFX_MSG_MAP(CBusToBusEditor)
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
END_MESSAGE_MAP()

/**
 * OnCreate:
 *
 * @param lpCreateStruct 
 * @return int 
 */
int CBusToBusEditor::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CIsDrawEditor::OnCreate(lpCreateStruct);

	const int nCount = sizeof(__EntityTypeMap) / sizeof(EntityTypeMap);
	for(int i = 0;i < nCount;++i)
	{
		AddEntityTypeMap(__EntityTypeMap[i].TypeString() , &(__EntityTypeMap[i]));
	}
	
	return 0;
}

/**
 * DrawGrid:
 *
 * @param pDC 
 * @param rect 
 * @return void 
 */
void CBusToBusEditor::DrawGrid(CDC *pDC, CRect rect)
{
	const COLORREF gridColor = RGB(100, 100, 100);
	
	CRect clipBox;
	pDC->GetClipBox(clipBox);
	clipBox.right = rect.right;clipBox.bottom = rect.bottom;
	clipBox.NormalizeRect();
	clipBox.InflateRect(m_gridSize.cx,m_gridSize.cy);
	
	
	int stepx = GetVirtualSize().cx / GetGridSize().cx;
	int stepy = GetVirtualSize().cy / GetGridSize().cy;
	for( int x = 0 ; x <= stepx ; x++ )
	{
		for(int y=0;y <= stepy; y++)
		{
			pDC->SetPixelV( round( static_cast< double >( GetGridSize().cx * x )),
					round( static_cast< double >( GetGridSize().cy * y )),
					gridColor);
		}
	}
}

/**
 * DrawBackground:
 *
 * @param pDC 
 * @param rect 
 * @return void 
 */
void CBusToBusEditor::DrawBackground(CDC *pDC, CRect rect)
{
	pDC->FillSolidRect( &rect, RGB(224 , 224 , 224));
}

/**
	@brief	

	@author	HumKyung

	@date	2011.01.03

	@param	wParam
	@param	lParam
	@param	pResult

	@return
*/
BOOL CBusToBusEditor::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	IS_NOTIFY* pNotify=(IS_NOTIFY*)(lParam);
	
	if(IS_DELETE_ENTITY == wParam)
	{
		if(IDYES == AfxMessageBox(_T("선택한 Entity를 삭제할까요?") , MB_YESNO|MB_ICONWARNING))
		{
			CELoadDocData& docData = CELoadDocData::GetInstance();
			const string sProjectMDBFilePath = docData.GetProjectMDBFilePath();
			//! MDB CONNECT
			CADODB adoDB;
			const CString sDBPath = CString(_T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=")) + sProjectMDBFilePath.c_str()  + DB_PASSWORD;
			const BOOL bOpened = adoDB.DBConnect(sDBPath);

			CIsDrawEntity* pEnt = NULL;
			int iIndex = 0;
			while(pEnt = m_pSelectionSet->GetAt(iIndex++))
			{
				if(pEnt->IsKindOf(CBusToBusRelation::TypeString()))
				{
					CBusToBusRelation* pRel = static_cast<CBusToBusRelation*>(pEnt);

					//! delete transformer which is associated relation
					CTransformerItem* pTrItem = pRel->GetTransformerItem();
					if(NULL != pTrItem)
					{
						if(bOpened)
						{
							//! delete transformer record from table
							const string sSql = _T("DELETE * FROM ") + CTransformerItem::TableName() + _T(" WHERE C_ID='") + string(pTrItem->id()) + _T("'");
							adoDB.ExecuteQuery(sSql.c_str());

							pTrItem->SetDeleteFlag(true);
						}

						if(pEnt->IsSelected()) pEnt->Select(FALSE);
						pEnt->Erase();
						if(m_pOnCursorEntity == pEnt) m_pOnCursorEntity = NULL;
					}
				}
			}
			if(bOpened) adoDB.DBDisConnect();
			RedrawWindow();	//! update drawing editor
		}
		return TRUE;
	}

	return CIsDrawEditor::OnNotify(wParam , lParam , pResult);;
}
