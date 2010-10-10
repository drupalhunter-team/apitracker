#pragma once

const TCHAR g_szConsoleServerWndName[] = _T("ConsoleServerWindowClassName");
const TCHAR g_szConsoleServerWndText[] = _T("ConsoleServerWindowText");

const int g_nMaxConsoleLoggerDataModuleNameLen = 10;
const int g_nMaxConsoleLoggerDataLogLen = 1024;

struct ConsoleLoggerData
{
	wchar_t szModuleName[g_nMaxConsoleLoggerDataModuleNameLen];
	wchar_t szLog[g_nMaxConsoleLoggerDataLogLen];
};

const int g_nConsoleLoggerID = 0x23462324;