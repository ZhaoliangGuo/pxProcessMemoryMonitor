
// pxProcessMemoryMonitor.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CpxProcessMemoryMonitorApp:
// �йش����ʵ�֣������ pxProcessMemoryMonitor.cpp
//

class CpxProcessMemoryMonitorApp : public CWinApp
{
public:
	CpxProcessMemoryMonitorApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CpxProcessMemoryMonitorApp theApp;