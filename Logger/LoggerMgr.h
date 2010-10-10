#pragma once
#include <tchar.h>
#include <windows.h>
#include <vector>

const int g_nMaxLogLen = 1024;

class CLoggerBase
{
public:
	CLoggerBase(){}
	virtual ~CLoggerBase(){}

	virtual BOOL Init(void* pContext) = 0;

	virtual int PrintLog(LPCSTR szModuleName, LPCSTR szLog)
	{return 0;}
	virtual int PrintLog(LPCWSTR szModuleName, LPCWSTR szLog)
	{return 0;}
};

class CLoggerMgr
{
protected:
	CLoggerMgr();

public:
	virtual ~CLoggerMgr();

public:
	BOOL InitLoggerMgr();
	BOOL UnInitLoggerMgr();

	BOOL AddLogger(CLoggerBase* pLogger);
	BOOL DelLogger(CLoggerBase* pLogger);

	static CLoggerMgr* GetInstance();
	virtual int PrintLogV(LPCSTR szModuleName, LPCSTR szFormat, ...);
	virtual int PrintLogV(LPCWSTR szModuleName, LPCWSTR szFormat, ...);

	virtual int LeveledPrintLog(LPCSTR szLevel, LPCSTR szModuleName, LPCSTR szFormat, ...);
	virtual int LeveledPrintLog(LPCWSTR szLevel, LPCWSTR szModuleName, LPCWSTR szFormat, ...);

	virtual int PrintLog(LPCSTR szModuleName, LPCSTR szLog);
	virtual int PrintLog(LPCWSTR szModuleName, LPCWSTR szLog);

private:
	std::vector<CLoggerBase*>	m_vctLoggers;
};

template<typename LoggerClass>
class CLoggerAdder
{
public:
	CLoggerAdder(void* pContext)
	{
		m_pLogger = new LoggerClass();
		if(m_pLogger->Init(pContext))
		{
			CLoggerMgr::GetInstance()->AddLogger(m_pLogger);
		}
		else
		{
			delete m_pLogger;
			m_pLogger = NULL;
		}
	}
	~CLoggerAdder()
	{
		if(m_pLogger != NULL)
		{
			CLoggerMgr::GetInstance()->DelLogger(m_pLogger);
		}
	}

private:
	LoggerClass* m_pLogger;
};

#ifdef _DEBUG

#define LOG_PRINT			CLoggerMgr::GetInstance()->PrintLogV
#define LOG_LEVELED_PRINT	CLoggerMgr::GetInstance()->LeveledPrintLog

#else

#define LOG_PRINT
#define LOG_LEVELED_PRINT

#endif	// _DEBUG

#define LOG_ERROR_LEVELA		_T("Error")
#define LOG_NORMAL_LEVELA		_T("")
#define LOG_WARNING_LEVELA		_T("Warning")

#define LOG_ERROR_LEVELW		_T("Error")
#define LOG_NORMAL_LEVELW		_T("")
#define LOG_WARNING_LEVELW		_T("Warning")
