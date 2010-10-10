#pragma once
#pragma warning(disable: 4786)
#pragma warning(disable: 4996)
#include <tchar.h>
#include <windows.h>
#include <string>
#include <locale>

typedef std::basic_string<TCHAR> tstring;

static const TCHAR szSpaceString[] = _T(" \r\n\t");

class string_helper
{
public:

	static tstring& trim_left(tstring& str)
	{
		tstring::size_type n = str.find_first_not_of(szSpaceString);
		if(n == tstring::npos)
		{
			str.resize(0);
		}
		else
		{
			str = str.erase(0, n);
		}
		return str;
	}

	static tstring& trim_right(tstring& str)
	{
		tstring::size_type n = str.find_last_not_of(szSpaceString);
		if(n == tstring::npos)
		{
			str.resize(0);
		}
		else
		{
			str = str.erase(n + 1);
		}
		return str;
	}

	static tstring& trim_string(tstring& str)
	{
		return trim_left(trim_right(str));
	}

	static TCHAR* trim_left(TCHAR* pString)
	{
		tstring str(pString);
		int nLen = str.length();
		trim_left(str);
		_tcsncpy(pString, str.c_str(), nLen);
		return pString;
	}

	static TCHAR* trim_right(TCHAR* pString)
	{
		tstring str(pString);
		int nLen = str.length();
		trim_right(str);
		_tcsncpy(pString, str.c_str(), nLen);
		return pString;
	}

	static TCHAR* trim_string(TCHAR* pString)
	{
		return trim_left(trim_right(pString));
	}
	
	static std::string ToMultiBytes(const char* szString, unsigned int nCount, UINT uCodePage)
	{
		return std::string(szString);
	}
	static std::wstring ToWideString(const wchar_t* wszString, unsigned int nCount, UINT uCodePage)
	{
		return std::wstring(wszString);
	}

	static std::string ToMultiBytes(const wchar_t* szWideString, unsigned int nCount, UINT uCodePage)
	{
		std::string strResult;
		int nNeedSize = WideCharToMultiByte(uCodePage, 0, szWideString, nCount, NULL, 0, NULL, NULL);
		if(nNeedSize > 0)
		{
			char* pBuffer = new char[nNeedSize + 1];
			nNeedSize = WideCharToMultiByte(uCodePage, 0, szWideString, nCount, pBuffer, nNeedSize, NULL, NULL);
			if(nNeedSize > 0)
			{
				pBuffer[nNeedSize] = 0;
				strResult = pBuffer;
			}
			delete[] pBuffer;
		}
		return strResult;
	}

	static std::wstring ToWideString(const char* szMultiBytes, unsigned int nCount, UINT uCodePage)
	{
		std::wstring wstrResult;
		int nNeedSize = MultiByteToWideChar(uCodePage, 0, szMultiBytes, nCount, NULL, 0);
		if(nNeedSize > 0)
		{
			wchar_t* wpBuffer = new wchar_t[nNeedSize + 1];
			nNeedSize = MultiByteToWideChar(uCodePage, 0, szMultiBytes, nCount, wpBuffer, nNeedSize);
			if(nNeedSize > 0)
			{
				wpBuffer[nNeedSize] = 0;
				wstrResult = wpBuffer;
			}
			delete[] wpBuffer;
		}
		return wstrResult;
	}

	static tstring string_token(const tstring& str, const tstring& strToken, int& nCookie)
	{
		int nLen = str.length();
		if(nCookie >= nLen || nCookie < 0)
		{
			nCookie = tstring::npos;
			return tstring();
		}

		tstring strResult;
		tstring::size_type pos = str.find(strToken, nCookie);
		if(pos != tstring::npos)
		{
			strResult = str.substr(nCookie, pos - nCookie);
			nCookie = pos + 1;
		}
		else if(nCookie < nLen)
		{
			strResult = str.substr(nCookie, nLen - nCookie);
			nCookie = nLen;
		}
		else
		{
			nCookie = tstring::npos;
		}

		return strResult;
	}

};