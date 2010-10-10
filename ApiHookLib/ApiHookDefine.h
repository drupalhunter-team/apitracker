#pragma once
#include <map>
#include <vector>

#pragma pack(1)
struct CodeStream
{
	// jmp next
	char	Jmp[1];
	char	TrueAddr[4];
	
	// push 123
	char	Push;
	int		PushData;
	
	// call Print
	char	Call;
	int		StubFuncAddr;
	
	// add esp, 4
	char	nAddEsp[3];
	
	// jmp TrueTest
	char	Call2;
	int		TrueFuncAddr;
	
	// ret
	char	Ret;
};

enum CallConvention
{
	unknownCall = 0,
		cdeclCall,
		stdCall,
		fastCall,
};

enum ValueType
{
	Value_Unknown,
		Value_Int,
		Value_BOOL,
		Value_Void,
		Value_PVoid,
		Value_HWND,
		Value_HDC,
		Value_LPSTR,
		Value_LPWSTR,
		Value_LPCSTR,
		Value_LPCWSTR,
};

struct StubCodeStream
{
	void*				pTrueFunc;
	unsigned int		nSize;
	tstring				strFuncName;

	CallConvention		Convention;

	ValueType			ReturnType;
	// Information about parameters of the function
	std::vector<ValueType>	vctParamType;
	// Stub Code
	CodeStream			Stream;
	// Module index
	int					nModuleIndex;
};

const char g_StreamBuffer[] = 
{
	// jmp next
	'\xE9', '\x00', '\x00', '\x00', '\x00',
	// push 0xEEEEEEEE
	'\x68', '\x00', '\x00', '\x00', '\x00',
	// call Print
	'\xE8', '\x12', '\x34', '\x56', '\x78',
	// add esp, 4
	'\x83', '\xC4', '\x04',
	// jmp TrueTest
	'\xE9', '\x12', '\x23', '\x34', '\x45',
	// ret
	'\xC3'
};

typedef std::map<PVOID, StubCodeStream*> StubCodeStreamMap;
typedef StubCodeStreamMap::iterator StubCodeStreamMapIte;