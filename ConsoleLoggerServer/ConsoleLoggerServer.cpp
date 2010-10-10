// ConsoleLoggerServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../Share/ConsoleLogDefine.h"
#include <locale.h>
#include <stdio.h>

HWND CreateServerWindow();
LRESULT WINAPI ServerWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

int main(int argc, char* argv[])
{
	printf("Locale: %s\r\n", setlocale(LC_ALL, "C"));
	HWND hServerWnd = CreateServerWindow();
	MSG msg;
	while(GetMessage(&msg, hServerWnd, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

HWND CreateServerWindow()
{
	WNDCLASSEX WndClassEx = { 0 };
	WndClassEx.cbSize = sizeof(WndClassEx);
	WndClassEx.lpfnWndProc = ServerWindowProc;
	WndClassEx.lpszClassName = g_szConsoleServerWndName;
	ATOM atom = RegisterClassEx(&WndClassEx);
	HWND hWnd = CreateWindowEx(0, g_szConsoleServerWndName,
		g_szConsoleServerWndText,
		0, 0, 0,
		0, 0,
		NULL, NULL,
		GetModuleHandle(NULL),
		0);
	return hWnd;
}

LRESULT WINAPI ServerWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
	case WM_COPYDATA:
		{
			COPYDATASTRUCT* pCopyData = (COPYDATASTRUCT*)lParam;
			if(pCopyData->dwData == g_nConsoleLoggerID && pCopyData->cbData == sizeof(ConsoleLoggerData))
			{
				static int nIndex = 0;
				ConsoleLoggerData* pConsoleLoggerData = (ConsoleLoggerData*)pCopyData->lpData;
				if(wcscmp(pConsoleLoggerData->szLog, L" ") == 0)
				{
					nIndex = 0;
					system("cls");
				}
				pConsoleLoggerData->szModuleName[g_nMaxConsoleLoggerDataModuleNameLen - 1] = 0;
				pConsoleLoggerData->szLog[g_nMaxConsoleLoggerDataLogLen - 1] = 0;
				wprintf(L"%d, %s: %s\r\n", nIndex, pConsoleLoggerData->szModuleName, pConsoleLoggerData->szLog);
				if(++nIndex > 65535)
				{
					nIndex = 0;
				}
			}
			break;
		}
	}
	return DefWindowProc(hWnd, Msg, wParam, lParam);
}