#pragma once

class CAutoSection
{
public:
	CAutoSection(CRITICAL_SECTION* pSection)
	{
		m_pSection = pSection;
		EnterCriticalSection(m_pSection);
	}
	~CAutoSection()
	{
		LeaveCriticalSection(m_pSection);
	}
protected:
	CRITICAL_SECTION*	m_pSection;
};