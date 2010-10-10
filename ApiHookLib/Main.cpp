#include "StdAfx.h"
#include "ApiHook.h"
#include "../Logger/LoggerMgr.h"

#pragma data_seg("ApiHookLib")
TCHAR szImageName[] = _T("ApiHookLib");
#pragma data_seg()
#pragma comment(linker, "/section:ApiHookLib,rws")

HMODULE g_hModule = NULL;
BOOL GetApiHookConfigFolderPath(TCHAR szFilePath[], DWORD dwLen);

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID)
{
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		{
			Init();
			//MessageBox(NULL, _T("TEST"), _T("TEST"), MB_OK);
			Sleep(500);
			LOG_PRINT(APIHOOKLIB_MODULE, _T(" "));
			LOG_PRINT(APIHOOKLIB_MODULE, _T("Locale: %s"), setlocale(LC_ALL, "C"));
			g_hModule = hModule;
			BOOL bResult = FALSE;
			TCHAR szFilePath[MAX_PATH];
			DWORD dwLen = sizeof(szFilePath);
			if(GetApiHookConfigFolderPath(szFilePath, dwLen))
			{
				LOG_PRINT(APIHOOKLIB_MODULE, _T("ApiHookConfigPath: %s"), szFilePath);
				tstring strFilePath(szFilePath);
				bResult = AnalyzeApiHookConfigFolder(strFilePath.c_str()); // AnalyzeFile(strFilePath);
			}
			if(!bResult)
			{
				MessageBox(NULL, _T("Failed to Analyze Api Data File!"), _T("API Hooker"), MB_OK | MB_ICONSTOP | MB_APPLMODAL);
			}
			break;
		}
	case DLL_PROCESS_DETACH:
		{
			UnAllHookFunc();
			UnInit();
			break;
		}
	}
	return TRUE;
}

BOOL GetApiHookConfigFolderPath(TCHAR szFilePath[], DWORD dwLen)
{
	HKEY hKey = NULL;
	BOOL bResult = FALSE;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\BugMaker"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		if(RegQueryValueEx(hKey, _T("ApiHookConfigPath"), 0, NULL, (LPBYTE)szFilePath, &dwLen) == ERROR_SUCCESS)
		{
			bResult = TRUE;
		}
		RegCloseKey(hKey);
	}
	if(bResult)
	{
		bResult = (GetFileAttributes(szFilePath) != (DWORD)-1);
	}

	return bResult;
}