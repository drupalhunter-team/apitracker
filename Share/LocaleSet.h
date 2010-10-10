#pragma once

class LocaleKeeper
{
public:
	LocaleKeeper(const char* pLocaleString)
	{
		m_pLocaleString = setlocale(LC_ALL, pLocaleString);
	}
	~LocaleKeeper()
	{
		setlocale(LC_ALL, m_pLocaleString);
	}

protected:
	char*	m_pLocaleString;
};

#define SCOPED_LOCALE_KEEPER(x) LocaleKeeper _LocaleKeeper(x)