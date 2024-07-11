
// TextPolygonInterView.h : CTextPolygonInterView ��Ľӿ�
//

#pragma once
#include <vector>

#define MAX_POINT 400

const double INF = 1E200;
const double EP = 1E-10;
const int MAXV = 300;
const double PI = 3.14159265;

//��������ÿһ���ߵ���Ϣ
typedef struct {
	int y_top;
	float x_int;
	int delta_y;
	float x_change_per_scan;
} EACH_ENTRY;

//ÿһ��ˮƽɨ���������αߵĽ�����Ϣ
typedef struct {
	int count;		//����ĸ���
	char* isdraw;	//�Ƿ񻭹�
	int* interpx;	//�����x����
	int* interpy;	//�����y����
} INTER_POINT;

struct PT
{
	double x;
	double y;
};
struct LINESEG
{
	PT s;
	PT e;
};
struct LINKLS
{
	bool isdrawn;
	LINESEG ls;
};
struct LINE           // ֱ�ߵĽ������� a*x+b*y+c=0  Ϊͳһ��ʾ��Լ�� a >= 0 
{
	double a;
	double b;
	double c;
};
struct IPT
{
	PT p;
	int status;
};

struct NEWPOINT
{
	double x, y;

	bool operator==(const NEWPOINT& v) const
	{
		return ((x == v.x) && (y == v.y));
	}
	bool operator < (const NEWPOINT& v) const
	{
		if (x != v.x)
			return x < v.x;
		else
		{
			if (y != v.y)
				return y<v.y;
			else
				return y>v.y;
		}
	}

};

class CTextPolygonInterView : public CView
{
protected: // �������л�����
	CTextPolygonInterView();
	DECLARE_DYNCREATE(CTextPolygonInterView)

// ����
public:
	CTextPolygonInterDoc* GetDocument() const;

	CPoint p1[100];//���ģ����������
	CPoint p2[100];//���ģ����������

	CPen fill_pen;
	CPen* p_oldpen;

	int total_ls;
	int nPointCount1;
	int nPointCount2;
	int nPolCount;

	CPoint pt_start, pt_end;
	CPoint firstPoint;			//�򵥶���ε�һ������

	BOOL m_bStartPoly;			//�Ƿ�ʼ����һ���򵥶����
	BOOL m_bSecondPoly;			//�Ƿ�ʼ����һ���򵥶����
	BOOL m_bFirstPoint;			//�Ƿ�Ϊ��һ������
	BOOL m_bLBtnDown;			//�ж��������Ƿ���

	BOOL m_bEndInputPolygon;//��������������
	BOOL m_bDrawPolygon;//���Ի��������
	std::vector<NEWPOINT> tnPoint;

// ����
public:
	double multiply(PT, PT, PT);//������
	double _max(double, double);
	double _min(double, double);
	bool intersect(LINESEG, LINESEG);//�ж��߶��Ƿ��ཻ
	LINE makeline(PT, PT);//������������ֱ��
	double slope(LINE);//����ֱ�ߵ�б��
	bool isparallel(LINESEG, LINESEG);//�ж��߶��Ƿ�ƽ��
	bool online(LINESEG l, PT p);//�жϵ��Ƿ����߶���
	PT lineintersect(LINE, LINE);//����ֱ��L1��L2�Ľ���
	double angle(PT, PT);//����Ƕ�
	int io_polygon(PT, int, CPoint*);//�жϵ������εĹ�ϵ
	void segsortx(int, int, PT, IPT*);//���µĵ���x�����������ķ�ʽ�����,���뽻������
	void segsorty(int, int, PT, IPT*);//���µĵ���y�����������ķ�ʽ�����,���뽻������
	void ppintersect(int, LINESEG*, CPoint*, int, LINESEG*, CPoint*);//��
	void ppunion(int, LINESEG*, CPoint*, int, LINESEG*, CPoint*);//��
	void ppdifferenceset1(int, LINESEG*, CPoint*, int, LINESEG*, CPoint*);//���1
	void ppdifferenceset2(int, LINESEG*, CPoint*, int, LINESEG*, CPoint*);//���2
	void linklineseg(CDC *pDC);//����������������߶μ�����ʾ

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CTextPolygonInterView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRelationshipIntersection();
	afx_msg void OnRelationshipUnion();
	afx_msg void OnRelationshipDifferenceset();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPolygonPolygon1();
	afx_msg void OnPolygonPolygon2();
	afx_msg void OnCleanscreen();
};

#ifndef _DEBUG  // TextPolygonInterView.cpp �еĵ��԰汾
inline CTextPolygonInterDoc* CTextPolygonInterView::GetDocument() const
   { return reinterpret_cast<CTextPolygonInterDoc*>(m_pDocument); }
#endif

