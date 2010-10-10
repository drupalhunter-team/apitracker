#include "StdAfx.h"
#include "CByteStream.h"
#include <cassert>

CByteStream::CByteStream(void* pStream, uint32 nBytesCount)
{
	Init(pStream, nBytesCount);
}

CByteStream::~CByteStream()
{
	Init(0, 0);
}

CByteStream::operator void*()
{
	return m_pStream;
}

void CByteStream::Init(void* pStream, uint32 nBytesCount)
{
	m_pStream		= (uchar*)pStream;
	m_nCurPos		= 0;
	m_CallConv		= CallConvention::_unknown_call;
	m_nParamSize	= 0;
	m_nBytesCount	= nBytesCount;
}

bool CByteStream::start(uint32 conv, uint32 uParamSize)
{
	m_CallConv = conv;
	m_nParamSize = uParamSize;
	return (m_nCurPos != 0);
}

bool CByteStream::end()
{
	bool bResult = false;
	if(m_CallConv == CallConvention::_stdcall_call)
	{
		bResult = ((m_nParamSize == 0) ? ret() : ret(m_nParamSize)) != 0;
	}
	else
	{
		assert(false);
	}
	return bResult;
}

void* CByteStream::push(uchar c)
{
	if(remain() < 2)
	{
		return 0;
	}

	uchar* pStream = m_pStream + m_nCurPos;
	pStream[0] = (uchar)0x6A;
	pStream[1] = c;
	m_nCurPos += 2;

	return pStream;
}

void* CByteStream::push(uint32 n)
{
	if(remain() < 3)
	{
		return 0;
	}
	assert(false);
	return 0;
}

void* CByteStream::call(void* pFunc)
{
	if(remain() < 5)
	{
		return 0;
	}

	uchar* pStream = m_pStream + m_nCurPos;
	pStream[0] = (uchar)0xE8;
	*(uint32*)(pStream + 1) = (uint32)(uint32*)((uchar*)pFunc - pStream - 5);
	m_nCurPos += 5;

	return pStream;
}

void* CByteStream::jump(void* pFunc)
{
	if(remain() < 5)
	{
		return 0;
	}

	uchar* pStream = m_pStream + m_nCurPos;
	pStream[0] = (uchar)0xE9;
	*(uint32*)(pStream + 1) = (uint32)(uint32*)((uchar*)pFunc - pStream - 5);
	m_nCurPos += 5;

	return pStream;
}

void* CByteStream::addReg(uint32 ereg, uchar c)
{
	if(remain() < 3)
	{
		return 0;
	}
	
	uchar* pStream = m_pStream + m_nCurPos;	
	pStream[0] = (uchar)0x83;
	pStream[1] = ereg;
	pStream[2] = c;
	m_nCurPos += 3;
	
	return pStream;
}

void* CByteStream::ret()
{
	if(remain() < 1)
	{
		return 0;
	}

	uchar* pStream = m_pStream + m_nCurPos;
	pStream[0] = 0xC3;
	++ m_nCurPos;
	return pStream;
}

void* CByteStream::ret(ushort s)
{
	if(remain() < 3)
	{
		return 0;
	}

	uchar* pStream = m_pStream + m_nCurPos;
	pStream[0] = 0xC2;
	*(ushort*)(pStream + 1) = s;

	m_nCurPos += 3;
	return pStream;
}

uint32 CByteStream::remain()
{
	return (m_nBytesCount - m_nCurPos - 1);
}
