#pragma once

#include "LoggerMgr.h"

class ConsoleLoggerClient : public CLoggerBase
{
public:
	ConsoleLoggerClient();
	virtual ~ConsoleLoggerClient();

	virtual BOOL Init(void* pContext);
	virtual int PrintLog(LPCSTR szModuleName, LPCSTR szLog);
	virtual int PrintLog(LPCWSTR szModuleName, LPCWSTR szLog);

	HWND	GetServerWindow();

private:
	HWND	m_hConsoleServerWnd;
};
