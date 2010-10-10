#include "stdafx.h"
#include "ApiHook.h"
#include "GlobalDefine.h"
#include <vector>
#include <fstream>
#include <detours.h>
#include <cassert>
#include <vector>
#include <iostream>
#include <cstdarg>
#include "cplusplusdef.h"
#include "../Logger/LoggerMgr.h"
#include "../Share/ConsoleLogDefine.h"
#include "../Share/AutoLock.h"
#include "../Share/LocaleSet.h"

using namespace std;

#pragma comment(lib, "detours.lib")
#pragma comment(lib, "detoured.lib")

#define ASSERT_TYPE(Type)
//#define ASSERT_TYPE(Type)		assert(Type != Value_Unknown)

StubCodeStreamMap		g_mapStubCodeStream;
vector<tstring>			g_vctModuleNames;
CRITICAL_SECTION		g_Section;

void Init()
{
	InitializeCriticalSection(&g_Section);
}

void UnInit()
{
	DeleteCriticalSection(&g_Section);
}

int PrintParam(StubCodeStream* pStream)
{
	CAutoSection AutoSection(&g_Section);
	SCOPED_LOCALE_KEEPER("");
	TCHAR szLog[g_nMaxConsoleLoggerDataLogLen] = { 0 };
	TCHAR* pLog = szLog;
	int nLogLen = g_nMaxConsoleLoggerDataLogLen;
	int nCount = 0;

	va_list argptr;
	va_start(argptr, pStream);
	va_arg(argptr, StubCodeStream*);

	nCount = _sntprintf(pLog, nLogLen, _T("%s: "), pStream->strFuncName.c_str());
	pLog += nCount;
	nLogLen -= nCount - 1;
	vector<ValueType>::iterator	Ite = pStream->vctParamType.begin();
	for(; Ite != pStream->vctParamType.end(); Ite++)
	{
		ValueType Type = *Ite;
		switch(Type)
		{
		case Value_Int:
			{
				int n = va_arg(argptr, int);
				nCount = _sntprintf(pLog, nLogLen, _T("%d, "), n);
				pLog += nCount;
				nLogLen -= nCount - 1;
				break;
			}
		case Value_BOOL:
			{
				BOOL b = va_arg(argptr, BOOL);
				nCount = _sntprintf(pLog, nLogLen, _T("%s, "), (b ? _T("TRUE") : _T("FALSE")));
				pLog += nCount;
				nLogLen -= nCount - 1;
				break;
			}
		case Value_LPCSTR:
		case Value_LPSTR:
			{
				LPCSTR b = va_arg(argptr, LPCSTR);
				if(b == NULL)
				{
					b = "Empty String";
				}
				else if(HIWORD(b) == 0)
				{
					b = "StringId";
				}
#ifdef _UNICODE
				wstring wstrLog = string_helper::ToWideString(b, strlen(b), CP_ACP);
				nCount = _snwprintf(pLog, nLogLen, _T("%s, "), wstrLog.c_str());
#else
				nCount = _snprintf(pLog, nLogLen, _T("%s, "), b);
#endif	//_UNICODE
				pLog += nCount;
				nLogLen -= nCount - 1;
				break;
			}
		case Value_LPCWSTR:
		case Value_LPWSTR:
			{
				LPCWSTR b = va_arg(argptr, LPCWSTR);
				if(b == NULL)
				{
					b = L"Empty String";
				}
				else if(HIWORD(b) == 0)
				{
					b = L"StringId";
				}
#ifdef _UNICODE
				nCount = _snwprintf(pLog, nLogLen, _T("%s, "), b);
#else
				string strLog = string_helper::ToMultiBytes(b, wcslen(b), CP_ACP);
				nCount = _snprintf(pLog, nLogLen, _T("%s, "), strLog.c_str());
#endif	//_UNICODE
				pLog += nCount;
				nLogLen -= nCount - 1;
				break;
			}
		case Value_PVoid:
		case Value_HWND:
		case Value_HDC:
		default:
			{
				int n = va_arg(argptr, int);
				nCount = _sntprintf(pLog, nLogLen, _T("0x%08x, "), n);
				pLog += nCount;
				nLogLen -= nCount - 1;
				break;
			}
		}
	}
	_sntprintf(pLog, nLogLen, _T("\r\n"));
	const TCHAR* pModuleName = g_vctModuleNames[pStream->nModuleIndex].c_str();
	LOG_PRINT(pModuleName, szLog);
	return NULL;
}

BOOL AnalyzeLine(const tstring& strLine, int nModuleIndex)
{
	tstring strLineCopy(strLine);
	string_helper::trim_string(strLineCopy);
	if(strLineCopy.empty())
	{
		return FALSE;
	}
	if(strLineCopy[0] == _T('#'))
	{
		return FALSE;
	}

	// Module Name.
	HMODULE hModule = NULL;
	int nCookie = 0;

	tstring strModule = string_helper::string_token(strLineCopy, _T(","), nCookie);
	string_helper::trim_string(strModule);
	hModule = LoadLibrary(strModule.c_str());
	if((nCookie == -1) || (hModule == NULL))
	{
		LOG_LEVELED_PRINT(LOG_ERROR_LEVELA, _T("AnalyzeLine"), _T("LoadLibrary Failed: %s"), strModule.c_str());
		return FALSE;
	}

	tstring strCallConvention = string_helper::string_token(strLineCopy, _T(","), nCookie);
	string_helper::trim_string(strCallConvention);
	CallConvention Convention = StringToConvention(strCallConvention.c_str());
	RETURN_VALUE_IF((nCookie == -1) || (Convention == unknownCall), FALSE);

	tstring strReturnType = string_helper::string_token(strLineCopy, _T(","), nCookie);
	string_helper::trim_string(strReturnType);
	ValueType ReturnType = StringToValueType(strReturnType.c_str());
	//RETURN_VALUE_IF((nCookie == -1) || (ReturnType == Value_Unknown), FALSE);
	if(nCookie == -1)
	{
		LOG_LEVELED_PRINT(LOG_WARNING_LEVELA, _T("AnalyzeLine"), _T("Return Type: %s"), strReturnType.c_str());
	}

	tstring strFuncName = string_helper::string_token(strLineCopy, _T(","), nCookie);
	string_helper::trim_string(strFuncName);
	string strName = string_helper::ToMultiBytes(strFuncName.c_str(), strFuncName.length(), CP_ACP);
	FARPROC FarProc = GetProcAddress(hModule, strName.c_str());
	if((nCookie == -1) || (FarProc == NULL))
	{
		LOG_LEVELED_PRINT(LOG_ERROR_LEVELA, _T("AnalyzeLine"), _T("GetProcAddress Failed: %s"), strName.c_str());
		return FALSE;
	}

	StubCodeStream* pStubCodeStream = HookFunc((void**)&FarProc, PrintParam);
	if(pStubCodeStream == NULL)
	{
		UnHookFunc(FarProc);
		LOG_LEVELED_PRINT(LOG_ERROR_LEVELA, _T("AnalyzeLine"), _T("HookFunc Failed: %s"), strName.c_str());
		return FALSE;
	}
	pStubCodeStream->Stream.PushData = (int)pStubCodeStream;
	pStubCodeStream->Convention = Convention;
	pStubCodeStream->ReturnType = ReturnType;
	pStubCodeStream->strFuncName = strFuncName;
	pStubCodeStream->nModuleIndex = nModuleIndex;

	LOG_PRINT(APIHOOKLIB_MODULE, _T("Hooked Function: %s"), pStubCodeStream->strFuncName.c_str());

	ValueType Type;
	tstring strParam;
	while(nCookie != -1)
	{
		strParam = string_helper::string_token(strLineCopy, _T(","), nCookie);
		string_helper::trim_string(strParam);
		if(strParam.empty())
		{
			continue;
		}
		if(AnalyzeParam(strParam, Type))
		{
			pStubCodeStream->vctParamType.push_back(Type);
		}
		else
		{
			assert(false);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AnalyzeFile(const tstring& strFilePath)
{
	ifstream InFile;
	InFile.open(strFilePath.c_str(), ios::in);
	if(!InFile.is_open())
	{
		LOG_LEVELED_PRINT(LOG_ERROR_LEVELA, APIHOOKLIB_MODULE, _T("File Analyzing Failed: %s"), strFilePath.c_str());
		return FALSE;
	}
	LOG_LEVELED_PRINT(LOG_ERROR_LEVELA, APIHOOKLIB_MODULE, _T("File Analyzing Succeededly: %s"), strFilePath.c_str());

	tstring strModuleName(strFilePath);
	strModuleName = strModuleName.substr(strModuleName.rfind(_T('\\')) + 1);
	g_vctModuleNames.push_back(strModuleName);

	int nIndex = g_vctModuleNames.size() - 1;
	tstring strLine;
	while(getline(InFile, strLine))
	{
		AnalyzeLine(strLine, nIndex);
	}
	InFile.close();
	return TRUE;
}

BOOL AnalyzeParam(const tstring& strParam, ValueType& Type)
{
	Type = StringToValueType(strParam.c_str());
	return TRUE;
}

StubCodeStream* HookFunc(PVOID *ppTrueFunc, PVOID pPrintFunc)
{
	const int nStubCodeStreamSize = sizeof(StubCodeStream);
	char* pCode = (char*)VirtualAlloc(0, nStubCodeStreamSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if(!pCode)
	{
		return NULL;
	}
	StubCodeStream* pStubCodeStream = (StubCodeStream*)pCode;
	pStubCodeStream->nSize = nStubCodeStreamSize;

	const int nStubContSize = sizeof(g_StreamBuffer);
	memcpy(&pStubCodeStream->Stream, g_StreamBuffer, sizeof(g_StreamBuffer));

	DetourUpdateThread(GetCurrentThread());
	DetourTransactionBegin();
	DetourAttach(ppTrueFunc, (void*)&pStubCodeStream->Stream);
	DetourTransactionCommit();
	pStubCodeStream->pTrueFunc = *ppTrueFunc;

	pStubCodeStream->Stream.StubFuncAddr = (char*)pPrintFunc - (char*)&pStubCodeStream->Stream.Call - 5;
	pStubCodeStream->Stream.TrueFuncAddr = (char*)*(int*)ppTrueFunc - (char*)&pStubCodeStream->Stream.Call2 - 5;

	g_mapStubCodeStream.insert(StubCodeStreamMap::value_type(*ppTrueFunc, pStubCodeStream));

	return pStubCodeStream;
}

StubCodeStreamMapIte UnHookFunc(StubCodeStreamMapIte Ite)
{
	StubCodeStream* pStubCodeStream = Ite->second;
	
	DetourUpdateThread(GetCurrentThread());
	DetourTransactionBegin();
	DetourDetach(&pStubCodeStream->pTrueFunc, (void*)&pStubCodeStream->Stream);
	DetourTransactionCommit();
	
	VirtualFree((void*)pStubCodeStream, pStubCodeStream->nSize, MEM_DECOMMIT);
	Ite = g_mapStubCodeStream.erase(Ite);
	return Ite;
}

void UnHookFunc(PVOID pTrueFunc)
{
	StubCodeStreamMapIte Ite = g_mapStubCodeStream.find(pTrueFunc);
	if(Ite != g_mapStubCodeStream.end())
	{
		UnHookFunc(Ite);
	}
}

void UnAllHookFunc()
{
	StubCodeStreamMapIte Ite = g_mapStubCodeStream.begin();
	while(Ite != g_mapStubCodeStream.end())
	{
		Ite = UnHookFunc(Ite);
	}
}

CallConvention	StringToConvention(LPCTSTR szConvention)
{
	CallConvention Convention = cdeclCall;
	if(_tcscmp(szConvention, _T("_stdcall")) == 0)
	{
		Convention = stdCall;
	}
	else if(_tcscmp(szConvention, _T("_fastcall")) == 0)
	{
		assert(false);
		Convention = fastCall;
	}
	return Convention;
}

#define BEGIN_PARSE_VALUE_TYPE()		\
	struct								\
	{									\
		ValueType	Type;				\
		LPCTSTR		szValueTypeName;	\
		int			nValueTypeNameLen;	\
	} _ValueMapData[] = {					

#define VALUE_TYPE_MAPPING(_TypeName, _ValueType)	\
		{_ValueType, _TypeName, sizeof(_TypeName) / sizeof(_TypeName[0]) - 1},

#define END_PARSE_VALUE_TYPE()	\
		};

BEGIN_PARSE_VALUE_TYPE()

	VALUE_TYPE_MAPPING(_T("int"),		Value_Int)
	VALUE_TYPE_MAPPING(_T("BOOL"),		Value_BOOL)
	VALUE_TYPE_MAPPING(_T("VOID"),		Value_Void)
	VALUE_TYPE_MAPPING(_T("void*"),		Value_PVoid)
	VALUE_TYPE_MAPPING(_T("HWND"),		Value_HWND)
	VALUE_TYPE_MAPPING(_T("HDC"),		Value_HDC)
	VALUE_TYPE_MAPPING(_T("LPCSTR"),	Value_LPCSTR)
	VALUE_TYPE_MAPPING(_T("LPCWSTR"),	Value_LPCWSTR)
	VALUE_TYPE_MAPPING(_T("LPSTR"),		Value_LPSTR)
	VALUE_TYPE_MAPPING(_T("LPWSTR"),	Value_LPWSTR)

END_PARSE_VALUE_TYPE()

ValueType		StringToValueType(LPCTSTR szValueType)
{
	ValueType Type = Value_Unknown;
	for(int i=0; i<sizeof(_ValueMapData) / sizeof(_ValueMapData[0]); i++)
	{
		if(_tcsncmp(_ValueMapData[i].szValueTypeName, szValueType, _ValueMapData[i].nValueTypeNameLen) == 0)
		{
			Type = _ValueMapData[i].Type;
			break;
		}
	}
	ASSERT_TYPE(Type);

	return Type;
}

BOOL AnalyzeApiHookConfigFolder(const tstring& strFilePath)
{
	if(strFilePath.empty())
	{
		return FALSE;
	}
	tstring strFolderPath(strFilePath);
	if(strFolderPath[strFolderPath.length() - 1] != _T('\\'))
	{
		strFolderPath += _T("\\");
	}
	tstring strFilter(strFolderPath);
	strFilter += _T("*.dat");

	WIN32_FIND_DATA Data = { 0 };
	HANDLE hFindFile = FindFirstFile(strFilter.c_str(), &Data);
	if(hFindFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	tstring strApiFilePath;
	BOOL bResult = FALSE;
	int nFileCount = 0;
	do 
	{
		if(_tcsncmp(Data.cFileName, _T("."), 1) == 0 || _tcsncmp(Data.cFileName, _T(".."), 2) == 0)
		{
			continue;
		}
		strApiFilePath = strFolderPath;
		strApiFilePath += Data.cFileName;
		bResult = AnalyzeFile(strApiFilePath);
		if(bResult)
			++ nFileCount;
	} while (FindNextFile(hFindFile, &Data));
	FindClose(hFindFile);
	return (nFileCount > 0);
}