
// TextPolygonInterView.cpp : CTextPolygonInterView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "TextPolygonInter.h"
#endif

#include "TextPolygonInterDoc.h"
#include "TextPolygonInterView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

EACH_ENTRY sides[MAX_POINT];  //��Ŷ���εı���Ϣ
INTER_POINT* inter_p;//��Ŷ���α���ˮƽɨ���ߵĽ�����Ϣ
LINESEG *p1_l;
LINESEG *p2_l;
LINKLS p_l[100];
IPT intsp[20];

// CTextPolygonInterView

IMPLEMENT_DYNCREATE(CTextPolygonInterView, CView)

BEGIN_MESSAGE_MAP(CTextPolygonInterView, CView)
	// ��׼��ӡ����
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

// CTextPolygonInterView ����/����

CTextPolygonInterView::CTextPolygonInterView()
{
	// TODO:  �ڴ˴���ӹ������
	fill_pen.CreatePen(PS_SOLID, 2, RGB(255, 0, 0));//��ɫ
	//��ʼ��
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
	// TODO:  �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CTextPolygonInterView ����

void CTextPolygonInterView::OnDraw(CDC* pDC)
{
	CTextPolygonInterDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  �ڴ˴�Ϊ����������ӻ��ƴ���
	CPen pen(PS_SOLID, 2, COLORREF(RGB(0, 0, 255))), *oldp;
	oldp = pDC->SelectObject(&pen);
	
	//��һ�������
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

   //�ڶ��������
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

	//��ʾ�������µĶ����
	p_oldpen = pDC->SelectObject(&fill_pen);
	linklineseg(pDC);
	pDC->SelectObject(p_oldpen);
}


// CTextPolygonInterView ��ӡ

BOOL CTextPolygonInterView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CTextPolygonInterView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CTextPolygonInterView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  ��Ӵ�ӡ����е��������
}


// CTextPolygonInterView ���

#ifdef _DEBUG
void CTextPolygonInterView::AssertValid() const
{
	CView::AssertValid();
}

void CTextPolygonInterView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTextPolygonInterDoc* CTextPolygonInterView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTextPolygonInterDoc)));
	return (CTextPolygonInterDoc*)m_pDocument;
}
#endif //_DEBUG


// CTextPolygonInterView ��Ϣ�������

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
	// TODO:  �ڴ���������������
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
	//�������ת��Ϊ�߶�
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
	//�ֱ�����������εĽ���
	ppintersect(vcount1, p1_l, p1, vcount2, p2_l, p2);
	ppintersect(vcount2, p2_l, p2, vcount1, p1_l, p1);

	Invalidate();
}


void CTextPolygonInterView::OnRelationshipUnion()
{
	// TODO:  �ڴ���������������
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
	//�������ת��Ϊ�߶�
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
	//�ֱ�����������εĽ���
	ppunion(vcount1, p1_l, p1, vcount2, p2_l, p2);
	ppunion(vcount2, p2_l, p2, vcount1, p1_l, p1);

	Invalidate();
}


void CTextPolygonInterView::OnRelationshipDifferenceset()
{
	// TODO:  �ڴ���������������
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
	//�������ת��Ϊ�߶�
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
	//�ֱ�����������εĽ���
	ppdifferenceset1(vcount1, p1_l, p1, vcount2, p2_l, p2);
	ppdifferenceset2(vcount2, p2_l, p2, vcount1, p1_l, p1);

	Invalidate();
}

void CTextPolygonInterView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
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
			firstPoint = point;//����ε�һ������
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
			firstPoint = point;//����ε�һ������
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
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
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
	// TODO:  �ڴ���������������
	total_ls = 0;
	nPointCount1 = nPointCount2 = 0;
	Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
// CTextPolygonInterView message handlers
//������
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
//�ж��߶��Ƿ��ཻ
bool CTextPolygonInterView::intersect(LINESEG u, LINESEG v)
{
	return((_max(u.s.x, u.e.x) >= _min(v.s.x, v.e.x)) &&                     //�ų�ʵ�� 
		(_max(v.s.x, v.e.x) >= _min(u.s.x, u.e.x)) &&
		(_max(u.s.y, u.e.y) >= _min(v.s.y, v.e.y)) &&
		(_max(v.s.y, v.e.y) >= _min(u.s.y, u.e.y)) &&
		(multiply(v.s, u.e, u.s)*multiply(u.e, v.e, u.s) >= 0) &&         //����ʵ�� 
		(multiply(u.s, v.e, v.s)*multiply(v.e, u.e, v.s) >= 0));
}
//������������ֱ��
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
//����ֱ�ߵ�б��
double CTextPolygonInterView::slope(LINE l)
{
	if (fabs(l.a) < 1e-20)return 0;
	if (fabs(l.b) < 1e-20)return INF;
	return -(l.a / l.b);
}
//�ж��߶��Ƿ�ƽ��
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
//�жϵ��Ƿ����߶���
bool CTextPolygonInterView::online(LINESEG l, PT p)
{
	return((multiply(l.e, p, l.s) == 0) && (((p.x - l.s.x)*(p.x - l.e.x) <= 0) && ((p.y - l.s.y)*(p.y - l.e.y) <= 0)));
}
//����ֱ��L1��L2�Ľ���
PT CTextPolygonInterView::lineintersect(LINE l1, LINE l2) // �� L1��L2 
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
//����Ƕ� 
double CTextPolygonInterView::angle(PT s, PT e)
{
	double a;
	a = atan2(e.y - s.y, e.x - s.x);
	return a;
}
//�жϵ������εĹ�ϵ
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
			return 2;//���ڶ���αߵĶ˵���
			break;
		}
		//����P1P��P2Pʸ���ļн�
		a = angle(p, p1) - angle(p, p2);
		if (fabs(fabs(a) - PI)<0.00001)
		{
			return 3;  //���ڶ���εı��ϣ��������˵㣩
			break;
		}

		if (a>PI)
			a = a - 2 * PI;
		if (a < -PI)
			a = 2 * PI + a;
		sum_angle = sum_angle + a;
	}
	if (fabs(sum_angle) < EP)
		return 0;    //���ڶ���ε�����
	else if (fabs(fabs(sum_angle) - 2 * PI)<EP)
		return 1;   //���ڶ���ε��ڲ�
	else
		return 2;   //���ڶ���αߵĶ˵���
}
//���µĵ���x�����������ķ�ʽ�����,���뽻������
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
//���µĵ���y�����������ķ�ʽ�����,���뽻������
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

//�����A��ÿһ���߷ֱ�������B��ÿһ������
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
			//����ཻ
			if (intersect(l1, l2))
			{	//���ƽ��,�����߶�L1���߶�L2�ص�
				if (isparallel(l1, l2))
				{
					//�߶�L1����㲻���߶�L2��,�յ����߶�L2��
					//����L1��������߶�L2��,�յ㲻�����߶�L2��
					//�򽫽���L1�������յ�֮���L2�������յ�
					//��Ϊ����
					if (((!online(l2, l1.s)) && online(l2, l1.e)) || ((online(l2, l1.s)) && (!online(l2, l1.e))))
					{
						//�߶�L1��y��ƽ��
						if (fabs(l1.s.x - l1.e.x)<EP)
						{
							//�����㰴y�Ĵ�С����,���뽻��������
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
						}//�����y�᲻ƽ��
						else
						{	//�����㰴x�Ĵ�С����,���뽻��������
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
					//�߶�L1����㲻���߶�L2��,�յ�Ҳ�����߶�L2��
					//��L2�������յ���Ϊ����
					else if ((!online(l2, l1.s)) && (!online(l2, l1.e)))
					{
						//����߶�L1��y��ƽ��
						if (fabs(l1.s.x - l1.e.x) < EP)
						{	//�����㰴y�Ĵ�С����,���뽻��������
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
						}//�����ƽ��
						else
						{	//�����㰴x�Ĵ�С����,���뽻��������
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
				//����߶�L1���߶�L2��ƽ��
				else
				{	//�ų��߶�L1�Ķ˵���߶�L2�Ķ˵㲿�ֻ�ȫ���غϵ����
					if (ip1.status != 2 || ip2.status != 2)
					{
						//�󽻵�
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
		//���߶κͶ���εĽ�����д���,���ݽ����ڶԷ�����ε�λ��,�����䱣������
		for (n = 0; n < k - 1; n++)
		{
			//�����ʼ���ڶ���εı߻�˵��ϲ����յ��ڶ���ε��ڲ�,����
			if ((intsp[n].status == 2 || intsp[n].status == 3) && (intsp[n + 1].status == 1))
			{
				p_l[total_ls].ls.s = intsp[n].p;
				p_l[total_ls].ls.e = intsp[n + 1].p;
				p_l[total_ls].isdrawn = false;
				total_ls++;
			}
			//�����ʼ���ڶ���ε��ڲ������յ��ڶ���εı߻�˵���,����
			else if ((intsp[n].status == 1) && (intsp[n + 1].status == 2 || intsp[n + 1].status == 3))
			{
				p_l[total_ls].ls.s = intsp[n].p;
				p_l[total_ls].ls.e = intsp[n + 1].p;
				p_l[total_ls].isdrawn = false;
				total_ls++;
			}
			//�����ʼ���ڶ���ε��ڲ������յ��ڶ���ε��ڲ�,����
			else if ((intsp[n].status == 1) && (intsp[n + 1].status == 1))
			{
				p_l[total_ls].ls.s = intsp[n].p;
				p_l[total_ls].ls.e = intsp[n + 1].p;
				p_l[total_ls].isdrawn = false;
				total_ls++;
			}
			//�����ʼ����յ�ֱ��ڶ���εı߻�˵���,�������ʼ����յ���е��ڶ���ε�
			//������ڲ�,��������(������)����(���ڲ�)
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
			//����ཻ
			if (intersect(l1, l2))
			{	//���ƽ��,�����߶�L1���߶�L2�ص�
				if (isparallel(l1, l2))
				{
					//�߶�L1����㲻���߶�L2��,�յ����߶�L2��
					//����L1��������߶�L2��,�յ㲻�����߶�L2��
					//�򽫽���L1�������յ�֮���L2�������յ�
					//��Ϊ����
					if (((!online(l2, l1.s)) && online(l2, l1.e)) || ((online(l2, l1.s)) && (!online(l2, l1.e))))
					{
						//�߶�L1��y��ƽ��
						if (fabs(l1.s.x - l1.e.x)<EP)
						{
							//�����㰴y�Ĵ�С����,���뽻��������
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
						}//�����y�᲻ƽ��
						else
						{	//�����㰴x�Ĵ�С����,���뽻��������
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
					//�߶�L1����㲻���߶�L2��,�յ�Ҳ�����߶�L2��
					//��L2�������յ���Ϊ����
					else if ((!online(l2, l1.s)) && (!online(l2, l1.e)))
					{
						//����߶�L1��y��ƽ��
						if (fabs(l1.s.x - l1.e.x) < EP)
						{	//�����㰴y�Ĵ�С����,���뽻��������
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
						}//�����ƽ��
						else
						{	//�����㰴x�Ĵ�С����,���뽻��������
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
				//����߶�L1���߶�L2��ƽ��
				else
				{	//�ų��߶�L1�Ķ˵���߶�L2�Ķ˵㲿�ֻ�ȫ���غϵ����
					if (ip1.status != 2 || ip2.status != 2)
					{
						//�󽻵�
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
		//���߶κͶ���εĽ�����д���,���ݽ����ڶԷ�����ε�λ��,�����䱣������
		for (n = 0; n < k - 1; n++)
		{
			//�����ʼ���ڶ���ε�����,����
			if (intsp[n].status == 0)
			{
				p_l[total_ls].ls.s = intsp[n].p;
				p_l[total_ls].ls.e = intsp[n + 1].p;
				p_l[total_ls].isdrawn = false;
				total_ls++;
			}
			//�����ʼ����յ�ֱ��ڶ���εı߻�˵���,�������ʼ����յ���е��ڶ���ε�
			//������ڲ�,��������(������)����(���ڲ�)
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
			//�����ʼ���ڶ���εı߻�˵���,�յ��ڶ���ε��ⲿ,����
			else if ((intsp[n].status == 2 || intsp[n].status == 3) && (intsp[n + 1].status == 0))
			{
				p_l[total_ls].ls.s = intsp[n].p;
				p_l[total_ls].ls.e = intsp[n + 1].p;
				p_l[total_ls].isdrawn = false;
				total_ls++;
			}//�������������
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
			//����ཻ
			if (intersect(l1, l2))
			{	//���ƽ��,�����߶�L1���߶�L2�ص�
				if (isparallel(l1, l2))
				{
					//�߶�L1����㲻���߶�L2��,�յ����߶�L2��
					//����L1��������߶�L2��,�յ㲻�����߶�L2��
					//�򽫽���L1�������յ�֮���L2�������յ�
					//��Ϊ����
					if (((!online(l2, l1.s)) && online(l2, l1.e)) || ((online(l2, l1.s)) && (!online(l2, l1.e))))
					{
						//�߶�L1��y��ƽ��
						if (fabs(l1.s.x - l1.e.x)<EP)
						{
							//�����㰴y�Ĵ�С����,���뽻��������
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
						}//�����y�᲻ƽ��
						else
						{	//�����㰴x�Ĵ�С����,���뽻��������
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
					//�߶�L1����㲻���߶�L2��,�յ�Ҳ�����߶�L2��
					//��L2�������յ���Ϊ����
					else if ((!online(l2, l1.s)) && (!online(l2, l1.e)))
					{
						//����߶�L1��y��ƽ��
						if (fabs(l1.s.x - l1.e.x) < EP)
						{	//�����㰴y�Ĵ�С����,���뽻��������
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
						}//�����ƽ��
						else
						{	//�����㰴x�Ĵ�С����,���뽻��������
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
				//����߶�L1���߶�L2��ƽ��
				else
				{	//�ų��߶�L1�Ķ˵���߶�L2�Ķ˵㲿�ֻ�ȫ���غϵ����
					if (ip1.status != 2 || ip2.status != 2)
					{
						//�󽻵�
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
		//���߶κͶ���εĽ�����д���,���ݽ����ڶԷ�����ε�λ��,�����䱣������
		for (n = 0; n < k - 1; n++)
		{
			//�����ʼ���ڶ���ε�����,����
			if (intsp[n].status == 0)
			{
				p_l[total_ls].ls.s = intsp[n].p;
				p_l[total_ls].ls.e = intsp[n + 1].p;
				p_l[total_ls].isdrawn = false;
				total_ls++;
			}
			//�����ʼ����յ�ֱ��ڶ���εı߻�˵���,�������ʼ����յ���е��ڶ���ε�
			//������ڲ�,��������(������)����(���ڲ�)
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
			//�����ʼ���ڶ���εı߻�˵���,�յ��ڶ���ε��ⲿ,����
			else if ((intsp[n].status == 2 || intsp[n].status == 3) && (intsp[n + 1].status == 0))
			{
				p_l[total_ls].ls.s = intsp[n].p;
				p_l[total_ls].ls.e = intsp[n + 1].p;
				p_l[total_ls].isdrawn = false;
				total_ls++;
			}//�������������
		}

	}
}
//�����B��ÿһ���߷ֱ�������A��ÿһ������
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
			//����ཻ
			if (intersect(l1, l2))
			{	//���ƽ��,�����߶�L1���߶�L2�ص�
				if (isparallel(l1, l2))
				{
					//�߶�L1����㲻���߶�L2��,�յ����߶�L2��
					//����L1��������߶�L2��,�յ㲻�����߶�L2��
					//�򽫽���L1�������յ�֮���L2�������յ�
					//��Ϊ����
					if (((!online(l2, l1.s)) && online(l2, l1.e)) || ((online(l2, l1.s)) && (!online(l2, l1.e))))
					{
						//�߶�L1��y��ƽ��
						if (fabs(l1.s.x - l1.e.x)<EP)
						{
							//�����㰴y�Ĵ�С����,���뽻��������
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
						}//�����y�᲻ƽ��
						else
						{	//�����㰴x�Ĵ�С����,���뽻��������
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
					//�߶�L1����㲻���߶�L2��,�յ�Ҳ�����߶�L2��
					//��L2�������յ���Ϊ����
					else if ((!online(l2, l1.s)) && (!online(l2, l1.e)))
					{
						//����߶�L1��y��ƽ��
						if (fabs(l1.s.x - l1.e.x) < EP)
						{	//�����㰴y�Ĵ�С����,���뽻��������
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
						}//�����ƽ��
						else
						{	//�����㰴x�Ĵ�С����,���뽻��������
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
				//����߶�L1���߶�L2��ƽ��
				else
				{	//�ų��߶�L1�Ķ˵���߶�L2�Ķ˵㲿�ֻ�ȫ���غϵ����
					if (ip1.status != 2 || ip2.status != 2)
					{
						//�󽻵�
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
		//���߶κͶ���εĽ�����д���,���ݽ����ڶԷ�����ε�λ��,�����䱣������
		for (n = 0; n < k - 1; n++)
		{
			//�����ʼ����յ�ֱ��ڶ���εı߻�˵���,�յ��ڶ���ε��ڲ�
			if ((intsp[n].status == 2 || intsp[n].status == 3) && (intsp[n + 1].status == 1))
			{
				p_l[total_ls].ls.s = intsp[n].p;
				p_l[total_ls].ls.e = intsp[n + 1].p;
				p_l[total_ls].isdrawn = false;
				total_ls++;
			}
			//�����ʼ����յ�ֱ��ڶ���εı߻�˵���,�������ʼ����յ���е��ڶ���ε�
			//������ڲ�,��������(������)����(���ڲ�)
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
			//�����ʼ���ڶ���ε��ڲ����յ��ڶ���εı߻�˵��ϻ��ڲ�,����
			else if ((intsp[n].status == 1) && (intsp[n + 1].status == 1 || intsp[n + 1].status == 2 || intsp[n + 1].status == 3))
			{
				p_l[total_ls].ls.s = intsp[n].p;
				p_l[total_ls].ls.e = intsp[n + 1].p;
				p_l[total_ls].isdrawn = false;
				total_ls++;
			}//�������������
		}

	}
}

//����������������߶μ�����ʾ
void CTextPolygonInterView::linklineseg(CDC *pDC)
{
	for (int i = 0; i < total_ls; i++)
	{
		pDC->MoveTo((int)(p_l[i].ls.s.x), (int)(p_l[i].ls.s.y));
		pDC->LineTo((int)(p_l[i].ls.e.x), (int)(p_l[i].ls.e.y));
	}
	//sort(tnPoint.begin(), tnPoint.end()); //�������ݽ�����������
	//tnPoint.erase(unique(tnPoint.begin(), tnPoint.end()), tnPoint.end());//����ʣ���ظ�Ԫ��
}

