#pragma once
#include "LoggerMgr.h"

class CViewWndLogger : public CLoggerBase
{
public:
	CViewWndLogger();
	virtual ~CViewWndLogger();

	virtual BOOL Init(void* pContext);

	virtual int PrintLog(LPCSTR szModuleName, LPCSTR szLog);
	virtual int PrintLog(LPCWSTR szModuleName, LPCWSTR szLog);

	HWND	GetViewWnd(BOOL bShowViewWnd = FALSE);

protected:
	HWND	m_hViewWnd;
};

class CFileLogger : public CLoggerBase
{
public:
	CFileLogger();
	virtual ~CFileLogger();
	
	virtual BOOL Init(void* pContext);
	
	virtual int PrintLog(LPCSTR szModuleName, LPCSTR szLog);
	virtual int PrintLog(LPCWSTR szModuleName, LPCWSTR szLog);

protected:
	FILE*	m_hFileLogger;
};