// BusView.cpp : implementation file
//

#include "stdafx.h"
#include "ELoad.h"
#include "BusDoc.h"
#include "BusView.h"
#include "ximage.h"
#include "Cxmemdc.h"
#include "pyfromc.h"

#include "UPSDCItem.h"
#include "GeneratorItem.h"
#include "CapacitorItem.h"
#include "TransformerItem.h"
// CBusView

IMPLEMENT_DYNCREATE(CBusView, CScrollView)

CBusView::CBusView()
{
	m_pObserver = new CObserver(this , &CBusView::OnEventFromObserver);
}

CBusView::~CBusView()
{
	try
	{
		if(m_pObserver) SAFE_DELETE(m_pObserver);
	}
	catch(...)
	{
	}
}

BEGIN_MESSAGE_MAP(CBusView, CScrollView)
	ON_COMMAND(ID_FILE_SAVE, &CBusView::OnFileSave)
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_BUS_DIAGRAM_OPEN, &CBusView::OnBusDiagramOpen)
END_MESSAGE_MAP()


// CBusView drawing
/**
	@brief	OnDraw Function

	@author	KHS/BHK
*/
void CBusView::OnDraw(CDC* pDC)
{
	CBusDoc* pDoc = (CBusDoc*)GetDocument();
	
	BOOL bPrinting = pDC->IsPrinting();

	CxMemDC* pMemDC = NULL;
	if (!bPrinting) pDC = pMemDC = new CxMemDC(pDC);

	if (!bPrinting/* && m_brHatch.m_hObject*/)
	{
		CRect rect;
		GetClientRect(&rect);
		rect.right  = max(rect.right , m_totalDev.cx);
		rect.bottom = max(rect.bottom, m_totalDev.cy);
		///m_brHatch.UnrealizeObject();
		CPoint pt(0, 0);
		pDC->LPtoDP(&pt);
		pt = pDC->SetBrushOrg(pt.x % 8, pt.y % 8);
		///CBrush* old = pDC->SelectObject(&m_brHatch);
		///pDC->FillRect(&rect, &m_brHatch);
		///pDC->SelectObject(old);
	}

	CxImage* ima = pDoc->GetImage();
	if (ima) 
	{
		if (bPrinting) 
		{
			// get size of printer page (in pixels)
			int cxPage = pDC->GetDeviceCaps(HORZRES);
			int cyPage = pDC->GetDeviceCaps(VERTRES);
			//int dcbpp = pDC->GetDeviceCaps(BITSPIXEL);
			//int dcnc = pDC->GetDeviceCaps(NUMCOLORS);
			//int dcp = pDC->GetDeviceCaps(PLANES);
			// get printer pixels per inch
			int cxInch = pDC->GetDeviceCaps(LOGPIXELSX);
			int cyInch = pDC->GetDeviceCaps(LOGPIXELSY);
			// Best Fit case: create a rectangle which preserves the aspect ratio
			int cx = ima->GetXDPI() ? (ima->GetWidth()*cxInch)/ima->GetXDPI():ima->GetWidth()*cxInch/96;
			int cy = ima->GetYDPI() ? (ima->GetHeight()*cyInch)/ima->GetYDPI():ima->GetHeight()*cyInch/96;
			// print it!
			/*HDC TmpDC=CreateCompatibleDC(pDC->GetSafeHdc());
			HBITMAP bm =::CreateCompatibleBitmap(pDC->GetSafeHdc(), cx, cy);
			HBITMAP oldbm = (HBITMAP)::SelectObject(TmpDC,bm);
			BitBlt(TmpDC,0,0,cx,cy,0,0,0,WHITENESS);
			ima->Draw(TmpDC,CRect(0,0,cx,cy));
			BitBlt(pDC->GetSafeHdc(),100,100,cx,cy,TmpDC,0,0,SRCCOPY);
			DeleteObject(SelectObject(TmpDC,oldbm));
			DeleteDC(TmpDC);*/
			CxImage tmp;
			tmp.Copy(*ima);
			RGBQUAD c={255,255,255,0};
			if (tmp.GetTransIndex()>=0) tmp.SetPaletteColor((BYTE)tmp.GetTransIndex(),c);
			tmp.SetTransColor(c);
			tmp.AlphaStrip();
			tmp.Stretch(pDC->GetSafeHdc(), CRect(100,100,cx,cy));
		} else {
			/*
			if (pDoc->GetStretchMode()) 
			{
				CRect rect;
				GetClientRect(&rect);
				ima->Draw(pDC->GetSafeHdc(), rect,0,pDoc->GetSmoothMode()!=0);
			}
			else
			*/
			{
				float zoom=1.f;///pDoc->GetZoomFactor();
				if (zoom==1)
				{
					CRect rect;
					GetClientRect(&rect);
					int nWidth = ima->GetWidth();
					int nHeight= ima->GetHeight();

					double dw = (nWidth > rect.Width()) ?  double(nWidth) / (double)rect.Width() : 1.;
					double dh = (nHeight> rect.Height())?  double(nHeight)/ (double)rect.Height(): 1.;
					zoom = dw > dh ? dw : dh;
					zoom = 1.f / zoom;
					
					int x = int((rect.Width() - ima->GetWidth()*zoom) * 0.5);
					x = x < 0 ? 0 : x;
					int y = int((rect.Height()- ima->GetHeight()*zoom)* 0.5);
					y = y < 0 ? 0 : y;

					ima->Draw(pDC->GetSafeHdc(),
						CRect(x , y , x + (int)(ima->GetWidth()*zoom) , y + (int)(ima->GetHeight()*zoom)),
						0,false);///pDoc->GetSmoothMode()!=0);+

				}
				else
				{
					CRect rect;
					GetClientRect(&rect);
					int x = int((rect.Width() - ima->GetWidth()) * 0.5);
					x = x < 0 ? 0 : x;
					x  = 0;
					int y = int((rect.Height()- ima->GetHeight())* 0.5);
					y = y < 0 ? 0 : y;
					y = 0;

					ima->Draw(pDC->GetSafeHdc(),
						CRect(x , y , x + (int)(ima->GetWidth()*zoom) , y + (int)(ima->GetHeight()*zoom)),
						0,false);///pDoc->GetSmoothMode()!=0);
				}
			}

			///if ( m_tracker.m_rect.Width()>0 && m_tracker.m_rect.Height()>0 )
			///	m_tracker.Draw(pDC);
		}
	}

	delete pMemDC;
}


// CBusView diagnostics

#ifdef _DEBUG
void CBusView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CBusView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG


// CBusView message handlers

void CBusView::OnFileSave()
{
	CELOADApp* pApp = (CELOADApp*)AfxGetApp();
	pApp->OnSaveDataBase();
}

/**
	@brief	

	@author	BHK
*/
BOOL CBusView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

/**
	@brief	

	@author	BHK
*/
void CBusView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CBusDoc* pDoc = (CBusDoc*)GetDocument();
	if (pDoc)
	{
		CxImage*  ima  = pDoc->GetImage();
		if (ima)
		{
			double nZoom = 1.f;///pDoc->GetZoomFactor();
			
			int px=GetScrollPos(SB_HORZ);
			int py=GetScrollPos(SB_VERT);
			CSize sz(GetTotalSize());
			int x=(int)(ima->GetWidth()*nZoom);
			int y=(int)(ima->GetHeight()*nZoom);
			SetScrollSizes(MM_TEXT,	CSize(x,y));
			CSize sz2(GetTotalSize());

			RECT rClient;
			CWnd* pFrame = GetParentFrame();
			if(NULL != pFrame)
				pFrame->GetClientRect(&rClient);
			else	GetClientRect(&rClient);

			if (sz.cx!=0 && sz.cy!=0)
			{
				if (x>rClient.right) SetScrollPos(SB_HORZ,sz2.cx*px/sz.cx); else  SetScrollPos(SB_HORZ,0);
				if (y>rClient.bottom) SetScrollPos(SB_VERT,sz2.cy*py/sz.cy); else SetScrollPos(SB_VERT,0);
			}
		}else	SetScrollSizes(MM_TEXT,CSize(1,1));

		RedrawWindow();
	}
	else
	{
		SetScrollSizes(MM_TEXT,CSize(1,1));
	}

	CScrollView::OnUpdate(pSender, lHint, pHint);
}

/**
	@brief	show menu to open bus diagram
	@author KHS	
	@date 2009-06-03 오후 1:38:02	
	@param	
	@return		
**/
void CBusView::OnRButtonDown(UINT nFlags, CPoint point)
{
	CPoint ptTree = point;
	ClientToScreen(&point);

	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_BUS_DIAGRAM_OPEN,
		point.x, point.y, this, TRUE);

	CScrollView::OnRButtonDown(nFlags, point);
}

/**
	@brief	

	@author KHS	

	@date 2009-06-03 오후 1:58:26	

	@param	

	@return		
**/
BOOL CBusView::OnPreparePrinting(CPrintInfo* pInfo)
{
	pInfo->SetMaxPage(1);
	return DoPreparePrinting(pInfo);
}

/**
	@brief  On event from Observer
	@author	BHK
*/
int CBusView::OnEventFromObserver(CObserver* pObserver , CEventData* pEventData)
{
	if((CBusItem::TypeString() == pEventData->GetItemCategory()) || (CUPSDCItem::TypeString() == pEventData->GetItemCategory()) || 
		(CTransformerItem::TypeString() == pEventData->GetItemCategory()) || (CCapacitorItem::TypeString() == pEventData->GetItemCategory()) ||
		(CGeneratorItem::TypeString() == pEventData->GetItemCategory()))
	{
		if((CEventData::MODIFY_ITEM_PROP == pEventData->m_eEventType) || (CEventData::ADD_ITEM == pEventData->m_eEventType) || 
			(CEventData::DELETE_ITEM == pEventData->m_eEventType))
		{
			CBusDoc* pDoc = (CBusDoc*)(((CBusView*)(pObserver->owner()))->GetDocument());
			if(pDoc)
			{
				pDoc->BeginWaitCursor();

				/// BusDiagram을 생성을 시도한다.
				///python.CallPyGenerateBusDiagram();
				///pDoc->ViewBusDiagram(pDoc->GetPathName());
				pDoc->GenerateBusDiagram(pDoc->GetPathName());

				pDoc->EndWaitCursor();
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	open bus diagram image

	@author	HumKyung.Baek

	@date	2010.10.21

	@return
*/
void CBusView::OnBusDiagramOpen()
{
	const CString rImgFilePath = GetExecPath() + _T("Temp\\BusDiagram.jpg");
	::ShellExecute(NULL , _T("open") , rImgFilePath , NULL , NULL , SW_SHOWNORMAL);
}
