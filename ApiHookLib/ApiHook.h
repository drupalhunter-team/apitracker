#pragma once
#include "../Share/tstring.h"
#include "ApiHookDefine.h"

// Hook Functions, and return relative data.
StubCodeStream* HookFunc(PVOID *ppTrueFunc, PVOID pPrintFunc);

// Unhook functions, and free resource allocated for it.
void UnHookFunc(PVOID pTrueFunc);

// Unhook all functions.
void UnAllHookFunc();

// Initialize
void Init();
void UnInit();

// Analyze all files in target directory
BOOL AnalyzeApiHookConfigFolder(const tstring& strFilePath);

// Analyze api data file and hook functions.
BOOL AnalyzeFile(const tstring& strFilePath);

// Helper functions when analyzing api data files.
BOOL AnalyzeLine(const tstring& strLine, int nModuleIndex);
BOOL AnalyzeParam(const tstring& strParam, ValueType& Type);

// Helper functions.
CallConvention	StringToConvention(LPCTSTR szConvention);
ValueType		StringToValueType(LPCTSTR szValueType);