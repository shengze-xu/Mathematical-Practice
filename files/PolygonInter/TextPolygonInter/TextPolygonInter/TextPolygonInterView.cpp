
// TextPolygonInterView.cpp : CTextPolygonInterView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "TextPolygonInter.h"
#endif

#include "TextPolygonInterDoc.h"
#include "TextPolygonInterView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

EACH_ENTRY sides[MAX_POINT];  //存放多边形的边信息
INTER_POINT* inter_p;//存放多边形边与水平扫描线的交点信息
LINESEG *p1_l;
LINESEG *p2_l;
LINKLS p_l[100];
IPT intsp[20];

// CTextPolygonInterView

IMPLEMENT_DYNCREATE(CTextPolygonInterView, CView)

BEGIN_MESSAGE_MAP(CTextPolygonInterView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_RELATIONSHIP_INTERSECTION, &CTextPolygonInterView::OnRelationshipIntersection)
	ON_COMMAND(ID_RELATIONSHIP_UNION, &CTextPolygonInterView::OnRelationshipUnion)
	ON_COMMAND(ID_RELATIONSHIP_DIFFERENCESET, &CTextPolygonInterView::OnRelationshipDifferenceset)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_POLYGON_POLYGON1, &CTextPolygonInterView::OnPolygonPolygon1)
	ON_COMMAND(ID_POLYGON_POLYGON2, &CTextPolygonInterView::OnPolygonPolygon2)
	ON_COMMAND(ID_CLEANSCREEN, &CTextPolygonInterView::OnCleanscreen)
END_MESSAGE_MAP()

// CTextPolygonInterView 构造/析构

CTextPolygonInterView::CTextPolygonInterView()
{
	// TODO:  在此处添加构造代码
	fill_pen.CreatePen(PS_SOLID, 2, RGB(255, 0, 0));//红色
	//初始化
	total_ls = 0;
	nPolCount = 0;
	nPointCount1 = nPointCount2 = 0;
	m_bEndInputPolygon = FALSE;
	m_bDrawPolygon = FALSE;

	m_bSecondPoly = FALSE;
	m_bStartPoly = FALSE;
	m_bFirstPoint = TRUE;
	m_bLBtnDown = FALSE;
}

CTextPolygonInterView::~CTextPolygonInterView()
{
	free(p1_l);
	free(p2_l);
	fill_pen.DeleteObject();
}

BOOL CTextPolygonInterView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CTextPolygonInterView 绘制

void CTextPolygonInterView::OnDraw(CDC* pDC)
{
	CTextPolygonInterDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  在此处为本机数据添加绘制代码
	CPen pen(PS_SOLID, 2, COLORREF(RGB(0, 0, 255))), *oldp;
	oldp = pDC->SelectObject(&pen);
	
	//第一个多边形
	int nCount1 = nPointCount1;
	if (nCount1 < 1) return;
	CPoint pt1;
	pt1 = CPoint(p1[0].x, p1[0].y);
	pDC->MoveTo(pt1);
	for (int i = 1; i <= nCount1; i++)
	{
		pt1 = CPoint(p1[i%nCount1].x, p1[i%nCount1].y);
		pDC->LineTo(pt1);
	}

   //第二个多边形
	int nCount2 = nPointCount2;
	if (nCount2 < 1) return;
	CPoint pt2;
	pt2 = CPoint(p2[0].x, p2[0].y);
	pDC->MoveTo(pt2);
	for (int i = 1; i <= nCount2; i++)
	{
		pt2 = CPoint(p2[i%nCount2].x, p2[i%nCount2].y);
		pDC->LineTo(pt2);
	}
	pDC->SelectObject(oldp);

	//显示操作后新的多边形
	p_oldpen = pDC->SelectObject(&fill_pen);
	linklineseg(pDC);
	pDC->SelectObject(p_oldpen);
}


// CTextPolygonInterView 打印

BOOL CTextPolygonInterView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CTextPolygonInterView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加额外的打印前进行的初始化过程
}

void CTextPolygonInterView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加打印后进行的清理过程
}


// CTextPolygonInterView 诊断

#ifdef _DEBUG
void CTextPolygonInterView::AssertValid() const
{
	CView::AssertValid();
}

void CTextPolygonInterView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTextPolygonInterDoc* CTextPolygonInterView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTextPolygonInterDoc)));
	return (CTextPolygonInterDoc*)m_pDocument;
}
#endif //_DEBUG


// CTextPolygonInterView 消息处理程序

void CTextPolygonInterView::OnPolygonPolygon1()
{
	m_bStartPoly = TRUE;
}


void CTextPolygonInterView::OnPolygonPolygon2()
{
	m_bSecondPoly = TRUE;
}


void CTextPolygonInterView::OnRelationshipIntersection()
{
	// TODO:  在此添加命令处理程序代码
	int i;
	int vcount1 = nPointCount1, vcount2 = nPointCount2;
	if (total_ls != 0)
	{
		free(p1_l);
		free(p2_l);
		total_ls = 0;
	}

	p1_l = (LINESEG *)malloc(vcount1*sizeof(LINESEG));
	p2_l = (LINESEG *)malloc(vcount2*sizeof(LINESEG));
	//将多边形转化为线段
	for (i = 0; i < vcount1; i++)
	{
		p1_l[i].s.x = p1[i%vcount1].x;
		p1_l[i].s.y = p1[i%vcount1].y;
		p1_l[i].e.x = p1[(i + 1) % vcount1].x;
		p1_l[i].e.y = p1[(i + 1) % vcount1].y;
	}

	for (i = 0; i < vcount2; i++)
	{
		p2_l[i].s.x = p2[i%vcount2].x;
		p2_l[i].s.y = p2[i%vcount2].y;
		p2_l[i].e.x = p2[(i + 1) % vcount2].x;
		p2_l[i].e.y = p2[(i + 1) % vcount2].y;
	}
	//分别求两个多边形的交点
	ppintersect(vcount1, p1_l, p1, vcount2, p2_l, p2);
	ppintersect(vcount2, p2_l, p2, vcount1, p1_l, p1);

	Invalidate();
}


void CTextPolygonInterView::OnRelationshipUnion()
{
	// TODO:  在此添加命令处理程序代码
	int i;
	int vcount1 = nPointCount1, vcount2 = nPointCount2;
	if (total_ls != 0)
	{
		free(p1_l);
		free(p2_l);
		total_ls = 0;
	}

	p1_l = (LINESEG *)malloc(vcount1*sizeof(LINESEG));
	p2_l = (LINESEG *)malloc(vcount2*sizeof(LINESEG));
	//将多边形转化为线段
	for (i = 0; i < vcount1; i++)
	{
		p1_l[i].s.x = p1[i%vcount1].x;
		p1_l[i].s.y = p1[i%vcount1].y;
		p1_l[i].e.x = p1[(i + 1) % vcount1].x;
		p1_l[i].e.y = p1[(i + 1) % vcount1].y;
	}

	for (i = 0; i < vcount2; i++)
	{
		p2_l[i].s.x = p2[i%vcount2].x;
		p2_l[i].s.y = p2[i%vcount2].y;
		p2_l[i].e.x = p2[(i + 1) % vcount2].x;
		p2_l[i].e.y = p2[(i + 1) % vcount2].y;
	}
	//分别求两个多边形的交点
	ppunion(vcount1, p1_l, p1, vcount2, p2_l, p2);
	ppunion(vcount2, p2_l, p2, vcount1, p1_l, p1);

	Invalidate();
}


void CTextPolygonInterView::OnRelationshipDifferenceset()
{
	// TODO:  在此添加命令处理程序代码
	int i;
	int vcount1 = nPointCount1, vcount2 = nPointCount2;
	if (total_ls != 0)
	{
		free(p1_l);
		free(p2_l);
		total_ls = 0;
	}

	p1_l = (LINESEG *)malloc(vcount1*sizeof(LINESEG));
	p2_l = (LINESEG *)malloc(vcount2*sizeof(LINESEG));
	//将多边形转化为线段
	for (i = 0; i < vcount1; i++)
	{
		p1_l[i].s.x = p1[i%vcount1].x;
		p1_l[i].s.y = p1[i%vcount1].y;
		p1_l[i].e.x = p1[(i + 1) % vcount1].x;
		p1_l[i].e.y = p1[(i + 1) % vcount1].y;
	}

	for (i = 0; i < vcount2; i++)
	{
		p2_l[i].s.x = p2[i%vcount2].x;
		p2_l[i].s.y = p2[i%vcount2].y;
		p2_l[i].e.x = p2[(i + 1) % vcount2].x;
		p2_l[i].e.y = p2[(i + 1) % vcount2].y;
	}
	//分别求两个多边形的交点
	ppdifferenceset1(vcount1, p1_l, p1, vcount2, p2_l, p2);
	ppdifferenceset2(vcount2, p2_l, p2, vcount1, p1_l, p1);

	Invalidate();
}

void CTextPolygonInterView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (m_bStartPoly)
	{

		m_bLBtnDown = TRUE;
		if (m_bFirstPoint)
		{
			m_bFirstPoint = FALSE;
			pt_start = point;
			pt_end = point;
			p1[nPointCount1] = point;
			nPointCount1++;
			firstPoint = point;//多边形第一个顶点
		}
		else
		{
			pt_start = pt_end;
			pt_end = point;

			CPoint lPt = firstPoint, rPt = firstPoint;
			lPt.Offset(-4, -4);
			rPt.Offset(4, 4);
			CRect rect(lPt, rPt);
			if (rect.PtInRect(pt_end))
			{
				m_bStartPoly = FALSE;
				m_bLBtnDown = FALSE;
				m_bFirstPoint = TRUE;
				this->Invalidate();
			}
			else
			{
				p1[nPointCount1] = point;
				nPointCount1++;
			}
		}
	}

	if (m_bSecondPoly)
	{

		m_bLBtnDown = TRUE;
		if (m_bFirstPoint)
		{
			m_bFirstPoint = FALSE;
			pt_start = point;
			pt_end = point;
			p2[nPointCount2] = point;
			nPointCount2++;
			firstPoint = point;//多边形第一个顶点
		}
		else
		{
			pt_start = pt_end;
			pt_end = point;

			CPoint lPt = firstPoint, rPt = firstPoint;
			lPt.Offset(-4, -4);
			rPt.Offset(4, 4);
			CRect rect(lPt, rPt);
			if (rect.PtInRect(pt_end))
			{
				m_bSecondPoly = FALSE;
				m_bLBtnDown = FALSE;
				m_bFirstPoint = TRUE;
				this->Invalidate();
			}
			else
			{
				p2[nPointCount2] = point;
				nPointCount2++;
			}
		}
	}

	CView::OnLButtonDown(nFlags, point);
}

void CTextPolygonInterView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (m_bStartPoly&&m_bLBtnDown || m_bSecondPoly&&m_bLBtnDown)
	{
		CPoint lPt = firstPoint, rPt = firstPoint;
		lPt.Offset(-4, -4);
		rPt.Offset(4, 4);
		CRect rect(lPt, rPt);
		CClientDC dc(this);
		dc.Rectangle(rect);
		dc.SetROP2(R2_NOTXORPEN);
		dc.MoveTo(pt_start);
		dc.LineTo(pt_end);
		pt_end = point;
		dc.MoveTo(pt_start);
		dc.LineTo(pt_end);
	}
	CView::OnMouseMove(nFlags, point);
}

void CTextPolygonInterView::OnCleanscreen()
{
	// TODO:  在此添加命令处理程序代码
	total_ls = 0;
	nPointCount1 = nPointCount2 = 0;
	Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
// CTextPolygonInterView message handlers
//计算差积
double CTextPolygonInterView::multiply(PT sp, PT ep, PT op)
{
	return((sp.x - op.x)*(ep.y - op.y) - (ep.x - op.x)*(sp.y - op.y));
}
double CTextPolygonInterView::_max(double a, double b)
{
	if (a >= b)
		return a;
	else
		return b;
}
double CTextPolygonInterView::_min(double a, double b)
{
	if (a <= b)
		return a;
	else
		return b;
}
//判断线段是否相交
bool CTextPolygonInterView::intersect(LINESEG u, LINESEG v)
{
	return((_max(u.s.x, u.e.x) >= _min(v.s.x, v.e.x)) &&                     //排斥实验 
		(_max(v.s.x, v.e.x) >= _min(u.s.x, u.e.x)) &&
		(_max(u.s.y, u.e.y) >= _min(v.s.y, v.e.y)) &&
		(_max(v.s.y, v.e.y) >= _min(u.s.y, u.e.y)) &&
		(multiply(v.s, u.e, u.s)*multiply(u.e, v.e, u.s) >= 0) &&         //跨立实验 
		(multiply(u.s, v.e, v.s)*multiply(v.e, u.e, v.s) >= 0));
}
//根据两点生成直线
LINE CTextPolygonInterView::makeline(PT p1, PT p2)
{
	LINE tl;
	int sign = 1;
	tl.a = p2.y - p1.y;
	if (tl.a < 0)
	{
		sign = -1;
		tl.a = sign*tl.a;
	}
	tl.b = sign*(p1.x - p2.x);
	tl.c = sign*(p1.y*p2.x - p1.x*p2.y);
	return tl;
}
//计算直线的斜率
double CTextPolygonInterView::slope(LINE l)
{
	if (fabs(l.a) < 1e-20)return 0;
	if (fabs(l.b) < 1e-20)return INF;
	return -(l.a / l.b);
}
//判断线段是否平行
bool CTextPolygonInterView::isparallel(LINESEG u, LINESEG v)
{
	LINE l1, l2;
	l1 = makeline(u.s, u.e);
	l2 = makeline(v.s, v.e);
	if (fabs(fabs(slope(l1)) - fabs(slope(l2))) < EP)
		return true;
	else
		return false;
}
//判断点是否在线段上
bool CTextPolygonInterView::online(LINESEG l, PT p)
{
	return((multiply(l.e, p, l.s) == 0) && (((p.x - l.s.x)*(p.x - l.e.x) <= 0) && ((p.y - l.s.y)*(p.y - l.e.y) <= 0)));
}
//计算直线L1、L2的交点
PT CTextPolygonInterView::lineintersect(LINE l1, LINE l2) // 是 L1，L2 
{
	PT p;
	double d = l1.a*l2.b - l2.a*l1.b;
	if (fabs(d) < EP)
	{
		p.x = -1;
		p.y = -1;
	}
	else
	{
		double d = l1.a*l2.b - l2.a*l1.b;
		p.x = (l2.c*l1.b - l1.c*l2.b) / d;
		p.y = (l2.a*l1.c - l1.a*l2.c) / d;
	}
	return p;
}
//计算角度 
double CTextPolygonInterView::angle(PT s, PT e)
{
	double a;
	a = atan2(e.y - s.y, e.x - s.x);
	return a;
}
//判断点与多边形的关系
int CTextPolygonInterView::io_polygon(PT p, int vcount, CPoint* pa)
{
	double sum_angle, a;
	PT p1, p2;
	int i;
	sum_angle = 0;
	for (i = 0; i < vcount; i++)
	{
		p1.x = pa[(i + 1) % vcount].x;
		p1.y = pa[(i + 1) % vcount].y;
		p2.x = pa[i%vcount].x;
		p2.y = pa[i%vcount].y;
		if (((fabs(p.x - p1.x) < EP) && (fabs(p.y - p1.y) < EP)) || ((fabs(p.x - p2.x) < EP) && (fabs(p.y - p2.y) < EP)))
		{
			return 2;//点在多边形边的端点上
			break;
		}
		//计算P1P、P2P矢量的夹角
		a = angle(p, p1) - angle(p, p2);
		if (fabs(fabs(a) - PI)<0.00001)
		{
			return 3;  //点在多边形的边上（不包括端点）
			break;
		}

		if (a>PI)
			a = a - 2 * PI;
		if (a < -PI)
			a = 2 * PI + a;
		sum_angle = sum_angle + a;
	}
	if (fabs(sum_angle) < EP)
		return 0;    //点在多边形的外面
	else if (fabs(fabs(sum_angle) - 2 * PI)<EP)
		return 1;   //点在多边形的内部
	else
		return 2;   //点在多边形边的端点上
}
//把新的点在x方向按升序或降序的方式排序后,插入交点数组
void CTextPolygonInterView::segsortx(int flag, int index, PT p, IPT* ls)
{
	if (flag == 1)
	{
		while ((index>0) && (ls[index - 1].p.x > p.x))
		{
			ls[index] = ls[index - 1];
			index--;
		}
		ls[index].p.x = p.x;
		ls[index].p.y = p.y;
		ls[index].status = 3;
	}
	else
	{
		while ((index > 0) && (ls[index - 1].p.x < p.x))
		{
			ls[index] = ls[index - 1];
			index--;
		}
		ls[index].p.x = p.x;
		ls[index].p.y = p.y;
		ls[index].status = 3;
	}
}
//把新的点在y方向按升序或降序的方式排序后,插入交点数组
void CTextPolygonInterView::segsorty(int flag, int index, PT p, IPT* ls)
{
	if (flag == 1)
	{
		while ((index > 0) && (ls[index - 1].p.y < p.y))
		{
			ls[index] = ls[index - 1];
			index--;
		}
		ls[index].p.x = p.x;
		ls[index].p.y = p.y;
		ls[index].status = 3;
	}
	else
	{
		while ((index > 0) && (ls[index - 1].p.y > p.y))
		{
			ls[index] = ls[index - 1];
			index--;
		}
		ls[index].p.x = p.x;
		ls[index].p.y = p.y;
		ls[index].status = 3;
	}
}

//多边形A的每一条边分别与多边形B的每一条边求交
void CTextPolygonInterView::ppintersect(int vcount1, LINESEG* lseg1, CPoint* pa1, int vcount2, LINESEG* lseg2, CPoint* pa2)
{
	int i, j, k, n;
	LINESEG l1, l2;
	IPT ip1, ip2;
	PT p;
	for (i = 0; i < vcount1; i++)
	{
		l1 = lseg1[i];
		ip1.p = lseg1[i].s;
		ip1.status = io_polygon(lseg1[i].s, vcount2, pa2);
		intsp[0] = ip1;

		ip2.p = lseg1[i].e;
		ip2.status = io_polygon(lseg1[i].e, vcount2, pa2);
		intsp[1] = ip2;
		k = 2;
		for (j = 0; j < vcount2; j++)
		{
			l2 = lseg2[j];
			//如果相交
			if (intersect(l1, l2))
			{	//如果平行,表明线段L1和线段L2重叠
				if (isparallel(l1, l2))
				{
					//线段L1的起点不在线段L2上,终点在线段L2上
					//或者L1的起点在线段L2上,终点不在在线段L2上
					//则将界于L1的起点和终点之间的L2的起点或终点
					//作为交点
					if (((!online(l2, l1.s)) && online(l2, l1.e)) || ((online(l2, l1.s)) && (!online(l2, l1.e))))
					{
						//线段L1与y轴平行
						if (fabs(l1.s.x - l1.e.x)<EP)
						{
							//将交点按y的大小排序,插入交点数组中
							if ((l2.e.y>l1.s.y) && (l2.e.y<l1.e.y))
							{
								segsorty(1, k, l2.e, intsp);
								k = k + 1;
							}
							else if ((l2.e.y<l1.s.y) && (l2.e.y>l1.e.y))
							{
								segsorty(-1, k, l2.e, intsp);
								k = k + 1;
							}
							else if ((l2.s.y>l1.s.y) && (l2.s.y < l1.e.y))
							{
								segsorty(1, k, l2.s, intsp);
								k = k + 1;
							}
							else if ((l2.s.y<l1.s.y) && (l2.s.y>l1.e.y))
							{
								segsorty(-1, k, l2.s, intsp);
								k = k + 1;
							}
						}//如果与y轴不平行
						else
						{	//将交点按x的大小排序,插入交点数组中
							if ((l2.e.x > l1.s.x) && (l2.e.x<l1.e.x))
							{
								segsortx(1, k, l2.e, intsp);
								k = k + 1;
							}
							else if ((l2.e.x<l1.s.x) && (l2.e.x>l1.e.x))
							{
								segsortx(-1, k, l2.e, intsp);
								k = k + 1;
							}
							else if ((l2.s.x>l1.s.x) && (l2.s.x < l1.e.x))
							{
								segsortx(1, k, l2.s, intsp);
								k = k + 1;
							}
							else if ((l2.s.x<l1.s.x) && (l2.s.x>l1.e.x))
							{
								segsortx(-1, k, l2.s, intsp);
								k = k + 1;
							}
						}
					}
					//线段L1的起点不在线段L2上,终点也不在线段L2上
					//则将L2的起点和终点作为交点
					else if ((!online(l2, l1.s)) && (!online(l2, l1.e)))
					{
						//如果线段L1与y轴平行
						if (fabs(l1.s.x - l1.e.x) < EP)
						{	//将交点按y的大小排序,插入交点数组中
							if (l1.s.y < l1.e.y)
							{
								segsorty(1, k, l2.e, intsp);
								k++;
								segsorty(1, k, l2.s, intsp);
								k++;
							}
							else
							{
								segsorty(-1, k, l2.e, intsp);
								k++;
								segsorty(-1, k, l2.s, intsp);
								k++;
							}
						}//如果不平行
						else
						{	//将交点按x的大小排序,插入交点数组中
							if (l1.s.x < l1.e.x)
							{
								segsortx(1, k, l2.e, intsp);
								k++;
								segsortx(1, k, l2.s, intsp);
								k++;
							}
							else
							{
								segsortx(-1, k, l2.e, intsp);
								k++;
								segsortx(-1, k, l2.s, intsp);
								k++;
							}
						}
					}
				}
				//如果线段L1和线段L2不平行
				else
				{	//排除线段L1的端点和线段L2的端点部分或全部重合的情况
					if (ip1.status != 2 || ip2.status != 2)
					{
						//求交点
						p = lineintersect(makeline(l1.s, l1.e), makeline(l2.s, l2.e));
						if (fabs(l1.s.x - l1.e.x)<EP)
						{
							if ((p.y>l1.s.y) && (p.y < l1.e.y))
							{
								segsorty(1, k, p, intsp);
								k = k + 1;
							}
							else if ((p.y<l1.s.y) && (p.y>l1.e.y))
							{
								segsorty(-1, k, p, intsp);
								k = k + 1;
							}
						}
						else
						{
							if ((p.x > l1.s.x) && (p.x < l1.e.x))
							{
								segsortx(1, k, p, intsp);
								k = k + 1;
							}
							else if ((p.x<l1.s.x) && (p.x>l1.e.x))
							{
								segsortx(-1, k, p, intsp);
								k = k + 1;
							}
						}
					}
				}
			}
		}
		//对线段和多边形的交点进行处理,根据交点在对方多边形的位置,决定其保留或丢弃
		for (n = 0; n < k - 1; n++)
		{
			//如果起始点在多边形的边或端点上并且终点在多边形的内部,则保留
			if ((intsp[n].status == 2 || intsp[n].status == 3) && (intsp[n + 1].status == 1))
			{
				p_l[total_ls].ls.s = intsp[n].p;
				p_l[total_ls].ls.e = intsp[n + 1].p;
				p_l[total_ls].isdrawn = false;
				total_ls++;
			}
			//如果起始点在多边形的内部并且终点在多边形的边或端点上,则保留
			else if ((intsp[n].status == 1) && (intsp[n + 1].status == 2 || intsp[n + 1].status == 3))
			{
				p_l[total_ls].ls.s = intsp[n].p;
				p_l[total_ls].ls.e = intsp[n + 1].p;
				p_l[total_ls].isdrawn = false;
				total_ls++;
			}
			//如果起始点在多边形的内部并且终点在多边形的内部,则保留
			else if ((intsp[n].status == 1) && (intsp[n + 1].status == 1))
			{
				p_l[total_ls].ls.s = intsp[n].p;
				p_l[total_ls].ls.e = intsp[n + 1].p;
				p_l[total_ls].isdrawn = false;
				total_ls++;
			}
			//如果起始点和终点分别在多边形的边或端点上,则根据起始点和终点的中点在多边形的
			//外面或内部,决定保留(在外面)或丢弃(在内部)
			else if ((intsp[n].status == 2 || intsp[n].status == 3) && (intsp[n + 1].status == 2 || intsp[n + 1].status == 3))
			{
				p.x = (intsp[n].p.x + intsp[n + 1].p.x) / 2;
				p.y = (intsp[n].p.y + intsp[n + 1].p.y) / 2;
				if (io_polygon(p, vcount2, pa2) == 1)
				{
					p_l[total_ls].ls.s = intsp[n].p;
					p_l[total_ls].ls.e = intsp[n + 1].p;
					p_l[total_ls].isdrawn = false;
					total_ls++;
				}
			}
		}

	}
}

void CTextPolygonInterView::ppunion(int vcount1, LINESEG* lseg1, CPoint* pa1, int vcount2, LINESEG* lseg2, CPoint* pa2)
{
	int i, j, k, n;
	LINESEG l1, l2;
	IPT ip1, ip2;
	PT p;
	for (i = 0; i < vcount1; i++)
	{
		l1 = lseg1[i];
		ip1.p = lseg1[i].s;
		ip1.status = io_polygon(lseg1[i].s, vcount2, pa2);
		intsp[0] = ip1;

		ip2.p = lseg1[i].e;
		ip2.status = io_polygon(lseg1[i].e, vcount2, pa2);
		intsp[1] = ip2;
		k = 2;
		for (j = 0; j < vcount2; j++)
		{
			l2 = lseg2[j];
			//如果相交
			if (intersect(l1, l2))
			{	//如果平行,表明线段L1和线段L2重叠
				if (isparallel(l1, l2))
				{
					//线段L1的起点不在线段L2上,终点在线段L2上
					//或者L1的起点在线段L2上,终点不在在线段L2上
					//则将界于L1的起点和终点之间的L2的起点或终点
					//作为交点
					if (((!online(l2, l1.s)) && online(l2, l1.e)) || ((online(l2, l1.s)) && (!online(l2, l1.e))))
					{
						//线段L1与y轴平行
						if (fabs(l1.s.x - l1.e.x)<EP)
						{
							//将交点按y的大小排序,插入交点数组中
							if ((l2.e.y>l1.s.y) && (l2.e.y<l1.e.y))
							{
								segsorty(1, k, l2.e, intsp);
								k = k + 1;
							}
							else if ((l2.e.y<l1.s.y) && (l2.e.y>l1.e.y))
							{
								segsorty(-1, k, l2.e, intsp);
								k = k + 1;
							}
							else if ((l2.s.y>l1.s.y) && (l2.s.y < l1.e.y))
							{
								segsorty(1, k, l2.s, intsp);
								k = k + 1;
							}
							else if ((l2.s.y<l1.s.y) && (l2.s.y>l1.e.y))
							{
								segsorty(-1, k, l2.s, intsp);
								k = k + 1;
							}
						}//如果与y轴不平行
						else
						{	//将交点按x的大小排序,插入交点数组中
							if ((l2.e.x > l1.s.x) && (l2.e.x<l1.e.x))
							{
								segsortx(1, k, l2.e, intsp);
								k = k + 1;
							}
							else if ((l2.e.x<l1.s.x) && (l2.e.x>l1.e.x))
							{
								segsortx(-1, k, l2.e, intsp);
								k = k + 1;
							}
							else if ((l2.s.x>l1.s.x) && (l2.s.x < l1.e.x))
							{
								segsortx(1, k, l2.s, intsp);
								k = k + 1;
							}
							else if ((l2.s.x<l1.s.x) && (l2.s.x>l1.e.x))
							{
								segsortx(-1, k, l2.s, intsp);
								k = k + 1;
							}
						}
					}
					//线段L1的起点不在线段L2上,终点也不在线段L2上
					//则将L2的起点和终点作为交点
					else if ((!online(l2, l1.s)) && (!online(l2, l1.e)))
					{
						//如果线段L1与y轴平行
						if (fabs(l1.s.x - l1.e.x) < EP)
						{	//将交点按y的大小排序,插入交点数组中
							if (l1.s.y < l1.e.y)
							{
								segsorty(1, k, l2.e, intsp);
								k++;
								segsorty(1, k, l2.s, intsp);
								k++;
							}
							else
							{
								segsorty(-1, k, l2.e, intsp);
								k++;
								segsorty(-1, k, l2.s, intsp);
								k++;
							}
						}//如果不平行
						else
						{	//将交点按x的大小排序,插入交点数组中
							if (l1.s.x < l1.e.x)
							{
								segsortx(1, k, l2.e, intsp);
								k++;
								segsortx(1, k, l2.s, intsp);
								k++;
							}
							else
							{
								segsortx(-1, k, l2.e, intsp);
								k++;
								segsortx(-1, k, l2.s, intsp);
								k++;
							}
						}
					}
				}
				//如果线段L1和线段L2不平行
				else
				{	//排除线段L1的端点和线段L2的端点部分或全部重合的情况
					if (ip1.status != 2 || ip2.status != 2)
					{
						//求交点
						p = lineintersect(makeline(l1.s, l1.e), makeline(l2.s, l2.e));
						if (fabs(l1.s.x - l1.e.x)<EP)
						{
							if ((p.y>l1.s.y) && (p.y < l1.e.y))
							{
								segsorty(1, k, p, intsp);
								k = k + 1;
							}
							else if ((p.y<l1.s.y) && (p.y>l1.e.y))
							{
								segsorty(-1, k, p, intsp);
								k = k + 1;
							}
						}
						else
						{
							if ((p.x > l1.s.x) && (p.x < l1.e.x))
							{
								segsortx(1, k, p, intsp);
								k = k + 1;
							}
							else if ((p.x<l1.s.x) && (p.x>l1.e.x))
							{
								segsortx(-1, k, p, intsp);
								k = k + 1;
							}
						}
					}
				}
			}
		}
		//对线段和多边形的焦点进行处理,根据交点在对方多边形的位置,决定其保留或丢弃
		for (n = 0; n < k - 1; n++)
		{
			//如果起始点在多边形的外面,则保留
			if (intsp[n].status == 0)
			{
				p_l[total_ls].ls.s = intsp[n].p;
				p_l[total_ls].ls.e = intsp[n + 1].p;
				p_l[total_ls].isdrawn = false;
				total_ls++;
			}
			//如果起始点和终点分别在多边形的边或端点上,则根据起始点和终点的中点在多边形的
			//外面或内部,决定保留(在外面)或丢弃(在内部)
			else if ((intsp[n].status == 2 || intsp[n].status == 3) && (intsp[n + 1].status == 2 || intsp[n + 1].status == 3))
			{
				p.x = (intsp[n].p.x + intsp[n + 1].p.x) / 2;
				p.y = (intsp[n].p.y + intsp[n + 1].p.y) / 2;
				if (io_polygon(p, vcount2, pa2) == 0)
				{
					p_l[total_ls].ls.s = intsp[n].p;
					p_l[total_ls].ls.e = intsp[n + 1].p;
					p_l[total_ls].isdrawn = false;
					total_ls++;
				}
			}
			//如果起始点在多边形的边或端点上,终点在多边形的外部,则保留
			else if ((intsp[n].status == 2 || intsp[n].status == 3) && (intsp[n + 1].status == 0))
			{
				p_l[total_ls].ls.s = intsp[n].p;
				p_l[total_ls].ls.e = intsp[n + 1].p;
				p_l[total_ls].isdrawn = false;
				total_ls++;
			}//其余情况都丢弃
		}

	}
}

void CTextPolygonInterView::ppdifferenceset1(int vcount1, LINESEG* lseg1, CPoint* pa1, int vcount2, LINESEG* lseg2, CPoint* pa2)
{
	int i, j, k, n;
	LINESEG l1, l2;
	IPT ip1, ip2;
	PT p;
	for (i = 0; i < vcount1; i++)
	{
		l1 = lseg1[i];
		ip1.p = lseg1[i].s;
		ip1.status = io_polygon(lseg1[i].s, vcount2, pa2);
		intsp[0] = ip1;

		ip2.p = lseg1[i].e;
		ip2.status = io_polygon(lseg1[i].e, vcount2, pa2);
		intsp[1] = ip2;
		k = 2;
		for (j = 0; j < vcount2; j++)
		{
			l2 = lseg2[j];
			//如果相交
			if (intersect(l1, l2))
			{	//如果平行,表明线段L1和线段L2重叠
				if (isparallel(l1, l2))
				{
					//线段L1的起点不在线段L2上,终点在线段L2上
					//或者L1的起点在线段L2上,终点不在在线段L2上
					//则将界于L1的起点和终点之间的L2的起点或终点
					//作为交点
					if (((!online(l2, l1.s)) && online(l2, l1.e)) || ((online(l2, l1.s)) && (!online(l2, l1.e))))
					{
						//线段L1与y轴平行
						if (fabs(l1.s.x - l1.e.x)<EP)
						{
							//将交点按y的大小排序,插入交点数组中
							if ((l2.e.y>l1.s.y) && (l2.e.y<l1.e.y))
							{
								segsorty(1, k, l2.e, intsp);
								k = k + 1;
							}
							else if ((l2.e.y<l1.s.y) && (l2.e.y>l1.e.y))
							{
								segsorty(-1, k, l2.e, intsp);
								k = k + 1;
							}
							else if ((l2.s.y>l1.s.y) && (l2.s.y < l1.e.y))
							{
								segsorty(1, k, l2.s, intsp);
								k = k + 1;
							}
							else if ((l2.s.y<l1.s.y) && (l2.s.y>l1.e.y))
							{
								segsorty(-1, k, l2.s, intsp);
								k = k + 1;
							}
						}//如果与y轴不平行
						else
						{	//将交点按x的大小排序,插入交点数组中
							if ((l2.e.x > l1.s.x) && (l2.e.x<l1.e.x))
							{
								segsortx(1, k, l2.e, intsp);
								k = k + 1;
							}
							else if ((l2.e.x<l1.s.x) && (l2.e.x>l1.e.x))
							{
								segsortx(-1, k, l2.e, intsp);
								k = k + 1;
							}
							else if ((l2.s.x>l1.s.x) && (l2.s.x < l1.e.x))
							{
								segsortx(1, k, l2.s, intsp);
								k = k + 1;
							}
							else if ((l2.s.x<l1.s.x) && (l2.s.x>l1.e.x))
							{
								segsortx(-1, k, l2.s, intsp);
								k = k + 1;
							}
						}
					}
					//线段L1的起点不在线段L2上,终点也不在线段L2上
					//则将L2的起点和终点作为交点
					else if ((!online(l2, l1.s)) && (!online(l2, l1.e)))
					{
						//如果线段L1与y轴平行
						if (fabs(l1.s.x - l1.e.x) < EP)
						{	//将交点按y的大小排序,插入交点数组中
							if (l1.s.y < l1.e.y)
							{
								segsorty(1, k, l2.e, intsp);
								k++;
								segsorty(1, k, l2.s, intsp);
								k++;
							}
							else
							{
								segsorty(-1, k, l2.e, intsp);
								k++;
								segsorty(-1, k, l2.s, intsp);
								k++;
							}
						}//如果不平行
						else
						{	//将交点按x的大小排序,插入交点数组中
							if (l1.s.x < l1.e.x)
							{
								segsortx(1, k, l2.e, intsp);
								k++;
								segsortx(1, k, l2.s, intsp);
								k++;
							}
							else
							{
								segsortx(-1, k, l2.e, intsp);
								k++;
								segsortx(-1, k, l2.s, intsp);
								k++;
							}
						}
					}
				}
				//如果线段L1和线段L2不平行
				else
				{	//排除线段L1的端点和线段L2的端点部分或全部重合的情况
					if (ip1.status != 2 || ip2.status != 2)
					{
						//求交点
						p = lineintersect(makeline(l1.s, l1.e), makeline(l2.s, l2.e));
						if (fabs(l1.s.x - l1.e.x)<EP)
						{
							if ((p.y>l1.s.y) && (p.y < l1.e.y))
							{
								segsorty(1, k, p, intsp);
								k = k + 1;
							}
							else if ((p.y<l1.s.y) && (p.y>l1.e.y))
							{
								segsorty(-1, k, p, intsp);
								k = k + 1;
							}
						}
						else
						{
							if ((p.x > l1.s.x) && (p.x < l1.e.x))
							{
								segsortx(1, k, p, intsp);
								k = k + 1;
							}
							else if ((p.x<l1.s.x) && (p.x>l1.e.x))
							{
								segsortx(-1, k, p, intsp);
								k = k + 1;
							}
						}
					}
				}
			}
		}
		//对线段和多边形的焦点进行处理,根据交点在对方多边形的位置,决定其保留或丢弃
		for (n = 0; n < k - 1; n++)
		{
			//如果起始点在多边形的外面,则保留
			if (intsp[n].status == 0)
			{
				p_l[total_ls].ls.s = intsp[n].p;
				p_l[total_ls].ls.e = intsp[n + 1].p;
				p_l[total_ls].isdrawn = false;
				total_ls++;
			}
			//如果起始点和终点分别在多边形的边或端点上,则根据起始点和终点的中点在多边形的
			//外面或内部,决定保留(在外面)或丢弃(在内部)
			else if ((intsp[n].status == 2 || intsp[n].status == 3) && (intsp[n + 1].status == 2 || intsp[n + 1].status == 3))
			{
				p.x = (intsp[n].p.x + intsp[n + 1].p.x) / 2;
				p.y = (intsp[n].p.y + intsp[n + 1].p.y) / 2;
				if (io_polygon(p, vcount2, pa2) == 0)
				{
					p_l[total_ls].ls.s = intsp[n].p;
					p_l[total_ls].ls.e = intsp[n + 1].p;
					p_l[total_ls].isdrawn = false;
					total_ls++;
				}
			}
			//如果起始点在多边形的边或端点上,终点在多边形的外部,则保留
			else if ((intsp[n].status == 2 || intsp[n].status == 3) && (intsp[n + 1].status == 0))
			{
				p_l[total_ls].ls.s = intsp[n].p;
				p_l[total_ls].ls.e = intsp[n + 1].p;
				p_l[total_ls].isdrawn = false;
				total_ls++;
			}//其余情况都丢弃
		}

	}
}
//多边形B的每一条边分别与多边形A的每一条边求交
void CTextPolygonInterView::ppdifferenceset2(int vcount1, LINESEG* lseg1, CPoint* pa1, int vcount2, LINESEG* lseg2, CPoint* pa2)
{
	int i, j, k, n;
	LINESEG l1, l2;
	IPT ip1, ip2;
	PT p;
	for (i = 0; i < vcount1; i++)
	{
		l1 = lseg1[i];
		ip1.p = lseg1[i].s;
		ip1.status = io_polygon(lseg1[i].s, vcount2, pa2);
		intsp[0] = ip1;

		ip2.p = lseg1[i].e;
		ip2.status = io_polygon(lseg1[i].e, vcount2, pa2);
		intsp[1] = ip2;
		k = 2;
		for (j = 0; j < vcount2; j++)
		{
			l2 = lseg2[j];
			//如果相交
			if (intersect(l1, l2))
			{	//如果平行,表明线段L1和线段L2重叠
				if (isparallel(l1, l2))
				{
					//线段L1的起点不在线段L2上,终点在线段L2上
					//或者L1的起点在线段L2上,终点不在在线段L2上
					//则将界于L1的起点和终点之间的L2的起点或终点
					//作为交点
					if (((!online(l2, l1.s)) && online(l2, l1.e)) || ((online(l2, l1.s)) && (!online(l2, l1.e))))
					{
						//线段L1与y轴平行
						if (fabs(l1.s.x - l1.e.x)<EP)
						{
							//将交点按y的大小排序,插入交点数组中
							if ((l2.e.y>l1.s.y) && (l2.e.y<l1.e.y))
							{
								segsorty(1, k, l2.e, intsp);
								k = k + 1;
							}
							else if ((l2.e.y<l1.s.y) && (l2.e.y>l1.e.y))
							{
								segsorty(-1, k, l2.e, intsp);
								k = k + 1;
							}
							else if ((l2.s.y>l1.s.y) && (l2.s.y < l1.e.y))
							{
								segsorty(1, k, l2.s, intsp);
								k = k + 1;
							}
							else if ((l2.s.y<l1.s.y) && (l2.s.y>l1.e.y))
							{
								segsorty(-1, k, l2.s, intsp);
								k = k + 1;
							}
						}//如果与y轴不平行
						else
						{	//将交点按x的大小排序,插入交点数组中
							if ((l2.e.x > l1.s.x) && (l2.e.x<l1.e.x))
							{
								segsortx(1, k, l2.e, intsp);
								k = k + 1;
							}
							else if ((l2.e.x<l1.s.x) && (l2.e.x>l1.e.x))
							{
								segsortx(-1, k, l2.e, intsp);
								k = k + 1;
							}
							else if ((l2.s.x>l1.s.x) && (l2.s.x < l1.e.x))
							{
								segsortx(1, k, l2.s, intsp);
								k = k + 1;
							}
							else if ((l2.s.x<l1.s.x) && (l2.s.x>l1.e.x))
							{
								segsortx(-1, k, l2.s, intsp);
								k = k + 1;
							}
						}
					}
					//线段L1的起点不在线段L2上,终点也不在线段L2上
					//则将L2的起点和终点作为交点
					else if ((!online(l2, l1.s)) && (!online(l2, l1.e)))
					{
						//如果线段L1与y轴平行
						if (fabs(l1.s.x - l1.e.x) < EP)
						{	//将交点按y的大小排序,插入交点数组中
							if (l1.s.y < l1.e.y)
							{
								segsorty(1, k, l2.e, intsp);
								k++;
								segsorty(1, k, l2.s, intsp);
								k++;
							}
							else
							{
								segsorty(-1, k, l2.e, intsp);
								k++;
								segsorty(-1, k, l2.s, intsp);
								k++;
							}
						}//如果不平行
						else
						{	//将交点按x的大小排序,插入交点数组中
							if (l1.s.x < l1.e.x)
							{
								segsortx(1, k, l2.e, intsp);
								k++;
								segsortx(1, k, l2.s, intsp);
								k++;
							}
							else
							{
								segsortx(-1, k, l2.e, intsp);
								k++;
								segsortx(-1, k, l2.s, intsp);
								k++;
							}
						}
					}
				}
				//如果线段L1和线段L2不平行
				else
				{	//排除线段L1的端点和线段L2的端点部分或全部重合的情况
					if (ip1.status != 2 || ip2.status != 2)
					{
						//求交点
						p = lineintersect(makeline(l1.s, l1.e), makeline(l2.s, l2.e));
						if (fabs(l1.s.x - l1.e.x)<EP)
						{
							if ((p.y>l1.s.y) && (p.y < l1.e.y))
							{
								segsorty(1, k, p, intsp);
								k = k + 1;
							}
							else if ((p.y<l1.s.y) && (p.y>l1.e.y))
							{
								segsorty(-1, k, p, intsp);
								k = k + 1;
							}
						}
						else
						{
							if ((p.x > l1.s.x) && (p.x < l1.e.x))
							{
								segsortx(1, k, p, intsp);
								k = k + 1;
							}
							else if ((p.x<l1.s.x) && (p.x>l1.e.x))
							{
								segsortx(-1, k, p, intsp);
								k = k + 1;
							}
						}
					}
				}
			}
		}
		//对线段和多边形的焦点进行处理,根据交点在对方多边形的位置,决定其保留或丢弃
		for (n = 0; n < k - 1; n++)
		{
			//如果起始点和终点分别在多边形的边或端点上,终点在多边形的内部
			if ((intsp[n].status == 2 || intsp[n].status == 3) && (intsp[n + 1].status == 1))
			{
				p_l[total_ls].ls.s = intsp[n].p;
				p_l[total_ls].ls.e = intsp[n + 1].p;
				p_l[total_ls].isdrawn = false;
				total_ls++;
			}
			//如果起始点和终点分别在多边形的边或端点上,则根据起始点和终点的中点在多边形的
			//外面或内部,决定保留(在外面)或丢弃(在内部)
			else if ((intsp[n].status == 2 || intsp[n].status == 3) && (intsp[n + 1].status == 2 || intsp[n + 1].status == 3))
			{
				p.x = (intsp[n].p.x + intsp[n + 1].p.x) / 2;
				p.y = (intsp[n].p.y + intsp[n + 1].p.y) / 2;
				if (io_polygon(p, vcount2, pa2) == 1)
				{
					p_l[total_ls].ls.s = intsp[n].p;
					p_l[total_ls].ls.e = intsp[n + 1].p;
					p_l[total_ls].isdrawn = false;
					total_ls++;
				}
			}
			//如果起始点在多边形的内部，终点在多边形的边或端点上或内部,则保留
			else if ((intsp[n].status == 1) && (intsp[n + 1].status == 1 || intsp[n + 1].status == 2 || intsp[n + 1].status == 3))
			{
				p_l[total_ls].ls.s = intsp[n].p;
				p_l[total_ls].ls.e = intsp[n + 1].p;
				p_l[total_ls].isdrawn = false;
				total_ls++;
			}//其余情况都丢弃
		}

	}
}

//将经过舍弃处理的线段加以显示
void CTextPolygonInterView::linklineseg(CDC *pDC)
{
	for (int i = 0; i < total_ls; i++)
	{
		pDC->MoveTo((int)(p_l[i].ls.s.x), (int)(p_l[i].ls.s.y));
		pDC->LineTo((int)(p_l[i].ls.e.x), (int)(p_l[i].ls.e.y));
	}
	//sort(tnPoint.begin(), tnPoint.end()); //顶点数据进行重新排序
	//tnPoint.erase(unique(tnPoint.begin(), tnPoint.end()), tnPoint.end());//擦除剩余重复元素
}

