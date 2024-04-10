#include "pch.h"
#include "CriticalSection.h"


_CCriticalSection::_CCriticalSection(CRITICAL_SECTION* CriticalSection)
{
	m_CriticalSection = CriticalSection;   //指针
	Lock();
}
_CCriticalSection::~_CCriticalSection()
{
	Unlock();
}
void _CCriticalSection::Unlock()
{
	LeaveCriticalSection(m_CriticalSection);
}
void _CCriticalSection::Lock()
{
	EnterCriticalSection(m_CriticalSection);
}

