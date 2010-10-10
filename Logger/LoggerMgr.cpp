#include "LoggerMgr.h"
#include <cassert>
#include <algorithm>
using namespace std;
#include "../Share/tstring.h"

CLoggerMgr::CLoggerMgr()
{
	;
}

CLoggerMgr::~CLoggerMgr()
{
	;
}

BOOL CLoggerMgr::InitLoggerMgr()
{
	return TRUE;
}

BOOL CLoggerMgr::UnInitLoggerMgr()
{
	return TRUE;
}

BOOL CLoggerMgr::AddLogger(CLoggerBase* pLogger)
{
	assert(find(m_vctLoggers.begin(), m_vctLoggers.end(), pLogger) == m_vctLoggers.end());
	m_vctLoggers.push_back(pLogger);
	return TRUE;
}

BOOL CLoggerMgr::DelLogger(CLoggerBase* pLogger)
{
	vector<CLoggerBase*>::iterator Ite = find(m_vctLoggers.begin(), m_vctLoggers.end(), pLogger);
	if(Ite != m_vctLoggers.end())
	{
		m_vctLoggers.erase(Ite);
	}
	return TRUE;
}

CLoggerMgr* CLoggerMgr::GetInstance()
{
	static CLoggerMgr _LoggerMgr;
	return &_LoggerMgr;
}

int CLoggerMgr::PrintLogV(LPCSTR szModuleName, LPCSTR szFormat, ...)
{
	char szBuffer[g_nMaxLogLen];
    va_list ArgList;
    va_start(ArgList, szFormat);
	
    int nReturn = _vsnprintf(szBuffer, g_nMaxLogLen, szFormat, ArgList);
	if(nReturn > 0)
	{
		PrintLog(szModuleName, szBuffer);
	}
	return nReturn;
}

int CLoggerMgr::PrintLogV(LPCWSTR szModuleName, LPCWSTR szFormat, ...)
{
	wchar_t szBuffer[g_nMaxLogLen];
    va_list ArgList;
    va_start(ArgList, szFormat);
	
    int nReturn = _vsnwprintf(szBuffer, g_nMaxLogLen, szFormat, ArgList);
	if(nReturn > 0)
	{
		PrintLog(szModuleName, szBuffer);
	}
	return nReturn;
}

int CLoggerMgr::LeveledPrintLog(LPCSTR szLevel, LPCSTR szModuleName, LPCSTR szFormat, ...)
{
	char szLeveledModuleName[MAX_PATH];
	_snprintf(szLeveledModuleName, MAX_PATH, "%s_%s", szLevel, szModuleName);

	char szBuffer[g_nMaxLogLen];
    va_list ArgList;
    va_start(ArgList, szFormat);

    int nReturn = _vsnprintf(szBuffer, g_nMaxLogLen, szFormat, ArgList);
	if(nReturn > 0)
	{
		PrintLog(szLeveledModuleName, szBuffer);
	}
	return nReturn;
}

int CLoggerMgr::LeveledPrintLog(LPCWSTR szLevel, LPCWSTR szModuleName, LPCWSTR szFormat, ...)
{
	wchar_t szLeveledModuleName[MAX_PATH];
	_snwprintf(szLeveledModuleName, MAX_PATH, L"%s_%s", szLevel, szModuleName);

	wchar_t szBuffer[g_nMaxLogLen];
    va_list ArgList;
    va_start(ArgList, szFormat);

    int nReturn = _vsnwprintf(szBuffer, g_nMaxLogLen, szFormat, ArgList);
	if(nReturn > 0)
	{
		PrintLog(szLeveledModuleName, szBuffer);
	}
	return nReturn;
}

int CLoggerMgr::PrintLog(LPCWSTR szModuleName, LPCWSTR szLog)
{
	vector<CLoggerBase*>::const_iterator ConstIte = m_vctLoggers.begin();
	for(; ConstIte != m_vctLoggers.end(); ConstIte ++)
	{
		CLoggerBase* pLoggerBase = *ConstIte;
		pLoggerBase->PrintLog(szModuleName, szLog);
	}
	return 1;
}

int CLoggerMgr::PrintLog(LPCSTR szModuleName, LPCSTR szLog)
{
	vector<CLoggerBase*>::const_iterator ConstIte = m_vctLoggers.begin();
	for(; ConstIte != m_vctLoggers.end(); ConstIte ++)
	{
		CLoggerBase* pLoggerBase = *ConstIte;
		pLoggerBase->PrintLog(szModuleName, szLog);
	}
	return 1;
}