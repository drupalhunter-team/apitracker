#include <windows.h>
#include <stdio.h>
#include <crtdbg.h>
#include "LogView.h"
#include "../Share/tstring.h"
using namespace std;

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)  (sizeof(x) / sizeof(x[0]))
#endif

/******************************************************/
CLoggerAdder<CViewWndLogger> ViewWndLogger(NULL);

CViewWndLogger::CViewWndLogger()
{
	m_hViewWnd = NULL;
}

CViewWndLogger::~CViewWndLogger()
{
}

BOOL CViewWndLogger::Init(void* pContext)
{
	HWND hWnd = GetViewWnd();
	return TRUE;
}

int CViewWndLogger::PrintLog(LPCSTR szModuleName, LPCSTR szLog)
{
	HWND hViewWnd = GetViewWnd(FALSE);
    if(hViewWnd == NULL)
        return 0;
	
    SYSTEMTIME T;
    ::GetLocalTime(&T);
	
    char wszBuffer[8192] = { 0 };
    _ASSERTE(strlen(szLog) <= 8000);
    int nLen = _snprintf(wszBuffer, ARRAY_SIZE(wszBuffer) - 1, "(%d) %02d:%02d:%02d.%03d000 %s: %s", ::GetCurrentThreadId(), 
        T.wHour, T.wMinute, T.wSecond, T.wMilliseconds, szModuleName, szLog);
	
    COPYDATASTRUCT Data;
    Data.dwData = 0;
    Data.lpData = (void *)wszBuffer;
    Data.cbData = nLen;
    DWORD dwResult = 0;
    SendMessageTimeout(hViewWnd, WM_COPYDATA, 0, (LPARAM)&Data, SMTO_BLOCK, 15000, &dwResult);
	
	return nLen;
}

int CViewWndLogger::PrintLog(LPCWSTR szModuleName, LPCWSTR szLog)
{
	string strModuleName = string_helper::ToMultiBytes(szModuleName, wcslen(szModuleName), CP_ACP);
	string strLog = string_helper::ToMultiBytes(szLog, wcslen(szLog), CP_ACP);
	return PrintLog(strModuleName.c_str(), strLog.c_str());
}

HWND CViewWndLogger::GetViewWnd(BOOL bShowViewWnd /* = FALSE */)
{
    m_hViewWnd = ::FindWindowA("Coobol_LogView", NULL);
    if(bShowViewWnd && m_hViewWnd != NULL)
    {
        if(::IsIconic(m_hViewWnd)) 
            ::ShowWindow(m_hViewWnd, SW_RESTORE);
        //SetForegroundWindow(m_hWndLogView);
    }
	return m_hViewWnd;
}
