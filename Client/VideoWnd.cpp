// VideoWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "VideoWnd.h"


// CVideoWnd 对话框

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)


IMPLEMENT_DYNAMIC(CVideoWnd, CDialog)

void DrawDib(LPBITMAPINFO pInfo,LPBYTE pData,HDC hDC,LPRECT pDestRect,LPRECT pSrcRect);

CVideoWnd::CVideoWnd(CWnd* pParent /*=NULL*/)
	: CDialog(CVideoWnd::IDD, pParent)
{
	m_pInfo = NULL;
	m_pData = NULL;
	m_bShowVideoImage = FALSE;
}

CVideoWnd::~CVideoWnd()
{
	SAFE_DELETE(m_pInfo);
	SAFE_DELETE(m_pData);
}

void CVideoWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CVideoWnd, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CVideoWnd 消息处理程序

BOOL CVideoWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (!m_bShowVideoImage)
	{
		CRect Rect;
		GetClientRect(Rect);
		CBrush brush(RGB(0,0,0));
		CBrush* pOldBrush = pDC->SelectObject(&brush);
		pDC->FillRect(Rect, &brush);

		pDC->SetTextColor(RGB(0,255,0)); //设置字体颜色
		pDC->SetBkMode(TRANSPARENT);
		pDC->TextOut(0, 0, m_strID, m_strID.GetLength());
		pDC->SelectObject(pOldBrush);
		brush.DeleteObject();
	}

	return TRUE;

	return CDialog::OnEraseBkgnd(pDC);
}

void CVideoWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()

	CRect rt;
	GetClientRect(rt);

	if((m_pInfo!=NULL)&&(m_pData!=NULL))
	{
		DrawDib(m_pInfo,m_pData, dc.m_hDC, &rt, NULL);
	}
}

BOOL CVideoWnd::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CVideoWnd::SetImage(LPBITMAPINFO pInfo,LPBYTE pData,BOOL bBK/*=FALSE*/)
{
	
	if(!pInfo || !pData)
		return;
	
	if(m_pInfo!=NULL)
	{
		delete m_pInfo;
		m_pInfo = NULL;
	}
	if(m_pData!=NULL)
	{
		delete m_pData;
		m_pData = NULL;
	}
	
	m_pInfo = (BITMAPINFO*)new BYTE[sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)];
	memcpy(m_pInfo,pInfo,sizeof(BITMAPINFO));
	
	int nSize = pInfo->bmiHeader.biSizeImage;
	if(nSize  == 0)
	{
		nSize = pInfo->bmiHeader.biHeight * WIDTHBYTES(pInfo->bmiHeader.biBitCount * pInfo->bmiHeader.biWidth);
	}
	
	m_pData = new BYTE[nSize];
	memcpy(m_pData,pData,nSize);

	//m_nWidth = pInfo->bmiHeader.biWidth;
	//m_nHeight = pInfo->bmiHeader.biHeight;

	//m_DestRect = CalcDrawRect(m_nWidth,m_nHeight);

	m_bShowVideoImage = TRUE;
	Invalidate();
}


void DrawDib(LPBITMAPINFO pInfo,LPBYTE pData,HDC hDC,LPRECT pDestRect,LPRECT pSrcRect)
{
	
	SetStretchBltMode(hDC,COLORONCOLOR);

	LONG	srcX = 0,srcY = 0,srcW = pInfo->bmiHeader.biWidth,srcH = pInfo->bmiHeader.biHeight;
	if(pSrcRect)
	{
		srcX = pSrcRect->left;
		srcY = pSrcRect->top;
		srcW = pSrcRect->right-pSrcRect->left;
		srcH = pSrcRect->bottom-pSrcRect->top;
	}

	long nWidth=abs(pInfo->bmiHeader.biWidth);
	long nHeight = abs(pInfo->bmiHeader.biHeight);
	long nWndWidth = pDestRect->right-pDestRect->left;
	long nWndHeight = pDestRect->bottom - pDestRect->top;
	int nL,nT;
	if(nWidth > nWndWidth)
	{
		double radio = nWidth /nWndWidth;
		nWidth = nWndWidth;
		nHeight = (double)nHeight/radio;
		nL = 0;
	}
	else
	{
		nL = (nWndWidth-nWidth)/2;
	}
	if(nHeight > nWndHeight)
	{
		double radio = nHeight /nWndHeight;
		nHeight = nWndHeight;
		nWidth = (double)nWidth/radio;
		nT = 0;
		nL = (nWndWidth-nWidth)/2;
	}
	else
	{
		nT = (nWndHeight-nHeight)/2;
	}
	int m_nL =  pDestRect->left+nL;
	int m_nT = pDestRect->top+nT;
	int m_nR = pDestRect->left+nL+nWidth;
	int m_nB = pDestRect->top+nT+nHeight;

	::StretchDIBits(hDC,
		  m_nL, m_nT,nWidth,nHeight,
		  srcX,srcY,srcW,srcH,
		  pData,pInfo,DIB_RGB_COLORS,SRCCOPY);
	
}