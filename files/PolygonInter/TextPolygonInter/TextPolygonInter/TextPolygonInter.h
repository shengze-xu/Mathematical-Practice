
// TextPolygonInter.h : TextPolygonInter Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CTextPolygonInterApp:
// �йش����ʵ�֣������ TextPolygonInter.cpp
//

class CTextPolygonInterApp : public CWinApp
{
public:
	CTextPolygonInterApp();


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CTextPolygonInterApp theApp;
