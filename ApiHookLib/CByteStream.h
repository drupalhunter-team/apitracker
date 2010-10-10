#pragma once

typedef unsigned int	uint32;
typedef unsigned char	uchar;
typedef unsigned short	ushort;

class ERegister
{
public:
	enum
	{
		REG_ESP = 0xC4,
	} ;
};

class CallConvention
{
public:
	enum
	{
		_unknown_call,
		_cdecl_call,
		_stdcall_call,
		_fastcall_call,
		_this_call,
	};
};

class CByteStream
{
public:
	CByteStream(void* pStream, uint32 nBytesCount);
	virtual ~CByteStream();

	operator void*();
	void Init(void* pStream, uint32 nBytesCount);

public:
	bool start(uint32 conv, uint32 uParamSize);

	bool end();

	uint32 remain();

	void* push(uchar s);
	void* push(uint32 n);

	void* call(void* pFunc);
	void* jump(void* pFunc);

	void* addReg(uint32 ereg, uchar c);

	void* ret();
	void* ret(ushort n);

private:
	uchar*			m_pStream;
	uint32			m_nBytesCount;
	uint32			m_nCurPos;
	uint32			m_nParamSize;
	uint32			m_CallConv;
};