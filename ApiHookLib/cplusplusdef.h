#pragma once
#include <tchar.h>
#include <string>
#include <strstream>
#include <iostream>

namespace cplusplusdef
{
#ifdef _IOSTREAM_

typedef std::basic_ifstream<TCHAR, std::char_traits<TCHAR> >	tifstream;
typedef std::basic_ofstream<TCHAR, std::char_traits<TCHAR> >	tofstream;
typedef std::basic_fstream<TCHAR, std::char_traits<TCHAR> >		tsftream;

typedef std::basic_istream<TCHAR, std::char_traits<TCHAR> >		tistream;
typedef std::basic_ostream<TCHAR, std::char_traits<TCHAR> >		tostream;

#endif	//_IOSTREAM_

#ifdef _UNICODE

#define tcin		wcin
#define tcout		wcout

#defien tstrstream	wstrstream

#else

#define tcin	cin
#define tcout	cout

#define tstrstream	strstream

#endif	// _UNICODE

}