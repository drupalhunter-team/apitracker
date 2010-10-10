#include "ConsoleLoggerClient.h"
#include "../Share/ConsoleLogDefine.h"
#include "../Share/tstring.h"
using namespace std;

CLoggerAdder<ConsoleLoggerClient> ConsoleLogger(NULL);

ConsoleLoggerClient::ConsoleLoggerClient()
{
	m_hConsoleServerWnd = NULL;
}

ConsoleLoggerClient::~ConsoleLoggerClient()
{

}

typedef BOOL (WINAPI* PFN_AttachConsole)(DWORD dwProcessId);

BOOL ConsoleLoggerClient::Init(void* pContext)
{
	BOOL bResult = FALSE;
	return (GetServerWindow() != NULL);
}

HWND ConsoleLoggerClient::GetServerWindow()
{
	if(m_hConsoleServerWnd == NULL)
		m_hConsoleServerWnd = FindWindow(g_szConsoleServerWndName, g_szConsoleServerWndText);
	return m_hConsoleServerWnd;
}

int ConsoleLoggerClient::PrintLog(LPCSTR szModuleName, LPCSTR szLog)
{
	wstring wstrModuleName = string_helper::ToWideString(szModuleName, strlen(szModuleName), CP_ACP);
	wstring wstrLog = string_helper::ToWideString(szLog, strlen(szLog), CP_ACP);
	return PrintLog(wstrModuleName.c_str(), wstrLog.c_str());
}

int ConsoleLoggerClient::PrintLog(LPCWSTR szModuleName, LPCWSTR szLog)
{
	HWND hWnd = GetServerWindow();
	DWORD dwResult = 0;
	if(hWnd != NULL)
	{
		ConsoleLoggerData LogData;
		wcsncpy(LogData.szLog, szLog, g_nMaxConsoleLoggerDataLogLen);
		wcsncpy(LogData.szModuleName, szModuleName, g_nMaxConsoleLoggerDataModuleNameLen);
		COPYDATASTRUCT CopyData;
		CopyData.cbData = sizeof(LogData);
		CopyData.dwData = g_nConsoleLoggerID;
		CopyData.lpData = (void*)&LogData;
		//SendMessageTimeout(hWnd, WM_COPYDATA, NULL, (LPARAM)&CopyData, SMTO_ABORTIFHUNG, 200, &dwResult);
		SendMessage(hWnd, WM_COPYDATA, NULL, (LPARAM)&CopyData);
	}
	return dwResult;
}