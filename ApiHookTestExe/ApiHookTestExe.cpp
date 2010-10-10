// ApiHookTestExe.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cassert>
#include <iostream>
using namespace std;
#include "../ApiHookLib/cplusplusdef.h"
#include "../Logger/LoggerMgr.h"
#include "../Share/tstring.h"

// #define HOOK_SELF_TEST

enum HookType
{
	HOOK_UNKNOWN = 0,
	HOOK_PID,
	HOOK_IMAGE,
};

void Usage();
BOOL AnalyzeArg(int argc, TCHAR* argv[], HookType& enumHookType, DWORD& dwPid, TCHAR szImagePath[MAX_PATH], TCHAR szImageParam[MAX_PATH * 2]);
BOOL SetApiFilePath(LPCTSTR szFilePath);

BOOL HookPid(DWORD dwPid, LPCTSTR szHookImagePath);
BOOL HookImage(LPCTSTR szImagePath, LPCTSTR szHookImagePath, LPCTSTR szHookImageParam);
BOOL HookProcess(HANDLE hProcess, LPCTSTR szHookImagePath);

int _tmain(int argc, TCHAR* argv[])
{
	HookType enumHookType = HOOK_UNKNOWN;
	DWORD dwPid = 0;
	TCHAR szImagePath[MAX_PATH];
	TCHAR szImageParam[MAX_PATH * 2];
	TCHAR szModulePath[MAX_PATH] = { 0 };
	TCHAR szApiHookPath[MAX_PATH] = { 0 };

#ifndef HOOK_SELF_TEST

	// Analyze Parameters.
	if(!AnalyzeArg(argc, argv, enumHookType, dwPid, szImagePath, szImageParam))
	{
		return 0;
	}

#else

	GetModuleFileName(GetModuleHandle(NULL), szImagePath, MAX_PATH);
	szImageParam[0] = 0;
	enumHookType = HOOK_IMAGE;
	FreeConsole();

#endif	//HOOK_SELF_TEST

	// Set the folder path of api data files.
	GetModuleFileName(GetModuleHandle(NULL), szApiHookPath, MAX_PATH);
	*(_tcsrchr(szApiHookPath, _T('\\'))) = 0;
	*(_tcsrchr(szApiHookPath, _T('\\')) + 1) = 0;
	_tcsncat(szApiHookPath, _T("Config\\"), MAX_PATH);

	GetModuleFileName(GetModuleHandle(NULL), szModulePath, MAX_PATH);
	*(_tcsrchr(szModulePath, _T('\\')) + 1) = 0;
	_tcsncat(szModulePath, _T("ApiHookLib.dll"), MAX_PATH);

	SetApiFilePath(szApiHookPath);

	// Hook the process.
	BOOL bResult = FALSE;
	if(enumHookType == HOOK_PID)
	{
		bResult = HookPid(dwPid, szModulePath);
	}
	else if(enumHookType == HOOK_IMAGE)
	{
		bResult = HookImage(szImagePath, szModulePath, szImageParam);
	}

	tcout << _T("Hook ") << (bResult ? _T("Succeeded") : _T("Failed")) << _T("!") << endl;

	return 0;
}

BOOL SetApiFilePath(LPCTSTR szFilePath)
{
	HKEY hKey = NULL;
	BOOL bResult = FALSE;

	if(RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\BugMaker"), 0, 0, 0, KEY_WRITE, 0, &hKey, 0) == ERROR_SUCCESS)
	{
		DWORD dwLen = (1 + _tcslen(szFilePath)) * sizeof(TCHAR);
		bResult = (RegSetValueEx(hKey, _T("ApiHookConfigPath"), 0, REG_SZ, (const BYTE*)szFilePath, dwLen) == ERROR_SUCCESS);
		RegCloseKey(hKey);
	}
	return bResult;
}

void Usage()
{
	TCHAR szImageName[MAX_PATH] = { 0 };
	GetModuleFileName(GetModuleHandle(NULL), szImageName, MAX_PATH);
	TCHAR* pImageName = _tcsrchr(szImageName, _T('\\')) + 1;
	// ApiHook.exe <[-p 1234] | [-f c:\windows\notepad.exe]>
	tcout << pImageName
		<< _T(" <[-p 1234] | [-f \"c:\\windows\\notepad.exe\"]>") << endl;
}

BOOL AnalyzeArg(int argc, TCHAR* argv[], HookType& enumHookType, DWORD& dwPid, TCHAR szImagePath[MAX_PATH], TCHAR szImageParam[MAX_PATH * 2])
{
	if(argc < 3 )
	{
		Usage();
		return 0;
	}

	if(_tcslen(argv[1]) != 2
		|| (argv[1][0] != _T('-') && argv[1][0] != _T('/'))
		|| (argv[1][1] != _T('p') && argv[1][1] != _T('f')
			&&argv[1][1] != _T('P') && argv[1][1] != _T('F'))
		)
	{
		Usage();
		return FALSE;
	}
	if(argv[1][1] == _T('p') || argv[1][1] == _T('P'))
	{
		enumHookType = HOOK_PID;
		dwPid = _ttoi(argv[2]);
	}
	else if(argv[1][1] == _T('f') || argv[1][1] == _T('F'))
	{
		enumHookType = HOOK_IMAGE;
		_tcsncpy(szImagePath, argv[2], MAX_PATH);
		szImageParam[0] = 0;
		for(int i=0; i<argc-3; i++)
		{
			_tcsncat(szImageParam, argv[3], MAX_PATH * 2);
			_tcsncat(szImageParam, _T(" "), MAX_PATH * 2);
		}
	}
	else
	{
		Usage();
		return FALSE;
	}
	return TRUE;
}

BOOL HookPid(DWORD dwPid, LPCTSTR szHookImagePath)
{
	HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, dwPid);
	BOOL bResult = FALSE;
	if(hProcess != NULL)
	{
		bResult = HookProcess(hProcess, szHookImagePath);
		CloseHandle(hProcess);
	}
	return bResult;
}

BOOL HookImage(LPCTSTR szImagePath, LPCTSTR szHookImagePath, LPCTSTR szHookImageParam)
{
	STARTUPINFO Si = { 0 };
	PROCESS_INFORMATION Pi = { 0 };
	Si.cb = sizeof(Si);

	tstring strCmdLine(_T("\""));
	strCmdLine += szImagePath;
	strCmdLine += _T("\" ");
	strCmdLine += szHookImageParam;
	BOOL bResult = CreateProcess(NULL,
		(char*)strCmdLine.c_str(),
		NULL,
		NULL,
		FALSE,
		CREATE_SUSPENDED,
		NULL,
		NULL,
		&Si,
		&Pi);
	if(bResult)
	{
		bResult = HookProcess(Pi.hProcess, szHookImagePath);

		//todo: click off MessageBox(in dll) before this, the target process will exit silently.
		ResumeThread(Pi.hThread);
		CloseHandle(Pi.hProcess);
		CloseHandle(Pi.hThread);
	}

	return bResult;
}

BOOL HookProcess(HANDLE hProcess, LPCTSTR szHookImagePath)
{
	DWORD dwSize = (_tcslen(szHookImagePath) + 1) * sizeof(TCHAR);
	void* pData = VirtualAllocEx(hProcess, 0, dwSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	BOOL bResult = FALSE;
	DWORD dwExitCode = 0;
	HANDLE hThread = NULL;
	if(pData != NULL)
	{
		DWORD dwWritten = 0;
		bResult = WriteProcessMemory(hProcess, pData, (void*)szHookImagePath, dwSize, &dwWritten);
		if(bResult)
		{
			hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibrary, pData, 0, 0);
			if(hThread != NULL)
			{
				if(WaitForSingleObject(hThread, 100) == WAIT_OBJECT_0 && GetExitCodeThread(hThread, &dwExitCode) && dwExitCode)
				{
					bResult = TRUE;
				}
				CloseHandle(hThread);
			}
		}
		VirtualFreeEx(hProcess, pData, dwSize, MEM_DECOMMIT);
	}
	return bResult;
}
