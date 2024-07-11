
// TextPolygonInterView.h : CTextPolygonInterView 类的接口
//

#pragma once
#include <vector>

#define MAX_POINT 400

const double INF = 1E200;
const double EP = 1E-10;
const int MAXV = 300;
const double PI = 3.14159265;

//保存多边形每一条边的信息
typedef struct {
	int y_top;
	float x_int;
	int delta_y;
	float x_change_per_scan;
} EACH_ENTRY;

//每一条水平扫描线与多边形边的交点信息
typedef struct {
	int count;		//交点的个数
	char* isdraw;	//是否画过
	int* interpx;	//交点的x坐标
	int* interpy;	//交点的y坐标
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
struct LINE           // 直线的解析方程 a*x+b*y+c=0  为统一表示，约定 a >= 0 
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
protected: // 仅从序列化创建
	CTextPolygonInterView();
	DECLARE_DYNCREATE(CTextPolygonInterView)

// 特性
public:
	CTextPolygonInterDoc* GetDocument() const;

	CPoint p1[100];//存放模拟多边形数据
	CPoint p2[100];//存放模拟多边形数据

	CPen fill_pen;
	CPen* p_oldpen;

	int total_ls;
	int nPointCount1;
	int nPointCount2;
	int nPolCount;

	CPoint pt_start, pt_end;
	CPoint firstPoint;			//简单多边形第一个顶点

	BOOL m_bStartPoly;			//是否开始绘制一个简单多边形
	BOOL m_bSecondPoly;			//是否开始绘制一个简单多边形
	BOOL m_bFirstPoint;			//是否为第一个顶点
	BOOL m_bLBtnDown;			//判断鼠标左键是否按下

	BOOL m_bEndInputPolygon;//多边形输入结束？
	BOOL m_bDrawPolygon;//可以画多边形吗？
	std::vector<NEWPOINT> tnPoint;

// 操作
public:
	double multiply(PT, PT, PT);//计算差积
	double _max(double, double);
	double _min(double, double);
	bool intersect(LINESEG, LINESEG);//判断线段是否相交
	LINE makeline(PT, PT);//根据两点生成直线
	double slope(LINE);//计算直线的斜率
	bool isparallel(LINESEG, LINESEG);//判断线段是否平行
	bool online(LINESEG l, PT p);//判断点是否在线段上
	PT lineintersect(LINE, LINE);//计算直线L1、L2的交点
	double angle(PT, PT);//计算角度
	int io_polygon(PT, int, CPoint*);//判断点与多边形的关系
	void segsortx(int, int, PT, IPT*);//把新的点在x方向按升序或降序的方式排序后,插入交点数组
	void segsorty(int, int, PT, IPT*);//把新的点在y方向按升序或降序的方式排序后,插入交点数组
	void ppintersect(int, LINESEG*, CPoint*, int, LINESEG*, CPoint*);//求交
	void ppunion(int, LINESEG*, CPoint*, int, LINESEG*, CPoint*);//求并
	void ppdifferenceset1(int, LINESEG*, CPoint*, int, LINESEG*, CPoint*);//求差1
	void ppdifferenceset2(int, LINESEG*, CPoint*, int, LINESEG*, CPoint*);//求差2
	void linklineseg(CDC *pDC);//将经过舍弃处理的线段加以显示

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CTextPolygonInterView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
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

#ifndef _DEBUG  // TextPolygonInterView.cpp 中的调试版本
inline CTextPolygonInterDoc* CTextPolygonInterView::GetDocument() const
   { return reinterpret_cast<CTextPolygonInterDoc*>(m_pDocument); }
#endif

